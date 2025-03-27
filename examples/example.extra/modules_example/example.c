#include <linux/init.h> // Needed for __init/__exit macros
#include <linux/module.h> // Required for all modules
#include <linux/printk.h>

#include <linux/vmalloc.h>
#include "linux/shmem_fs.h"

int __init load(void);
void __exit unload(void);

/*
 * My test cases (offset, pre_boundary_offset)
 * 1) Perfectly aligned start, r/w to 1 page (0, 0)
 * 2) Early start + end, w 2 pages, r 1 at a time (0, 1)
 */

static int offset = 0;
static int pre_boundary_offset = 0;
static char *msg = "Chappel Roan makes good songs";
module_param(pre_boundary_offset, int, 0);
module_param(offset, int, 0);
module_param(msg, charp, 0);
MODULE_PARM_DESC(offset, "for reading only");
MODULE_PARM_DESC(pre_boundary_offset, "for mapping the write");
MODULE_PARM_DESC(msg, "for custom messages");

static int order;
static int payload_size;
static struct page *head;
static struct file *file;
static int nr_pages;
static void *vaddr = NULL;
static void *vaddrW = NULL; //virtual address for writing

static loff_t start = 0;

static int read_test(void)
{
	payload_size = strlen(msg);

	gfp_t gfp = __GFP_COMP | GFP_KERNEL;
	order = 4;
	head = alloc_pages(gfp, order);

	file = shmem_file_setup("/testing_file", folio_size((struct folio*) head), VM_NORESERVE);
	if (IS_ERR(file))
		return 0;

	nr_pages = (int) file->f_mapping->host->i_size >> PAGE_SHIFT;
	printk("shmem file of size %d, created", nr_pages << PAGE_SHIFT);

	start = PAGE_SIZE - pre_boundary_offset;
	loff_t end = start + payload_size;
	unsigned long flags = VM_MAP;
	pgprot_t prot = PAGE_KERNEL;

	//load pages into pagecache
	for (int i = 0; i < nr_pages; i++) {
		struct folio *folio = shmem_read_folio_gfp(file->f_mapping, i, GFP_KERNEL);
		if (IS_ERR(folio))
			printk("failed reading to pagecache");
		i += folio_nr_pages(folio) - 1;
		folio_put(folio);
	}

	printk("vmapping start: %d, end: %d", (int) start, (int) end);
	//the idea here is to map from the 2nd page to kernel virtual space
	//maps only 2nd page for reading (end gets send up to the next page boundary)
	vaddr = vmap_file(file->f_mapping, start + pre_boundary_offset, end, flags, prot);
	if (!vaddr) {
		goto out;
	}

	//this maps both pages (vaddrW is PAGE_ALIGNED to bottom of start)
	vaddrW = vmap_file(file->f_mapping, start, end, flags, prot);
	if (!vaddrW) {
		vunmap(vaddr);
		goto out;
	}

	//write msg from the start point
	//if pre_boundary_offset <= 0 we only map the second page
	//otherwise we need to shift the write so our read mapping exists
	if (pre_boundary_offset <= 0)
		start = 0;
	printk("file has been mapped, writing the %s payload_size %d", msg, payload_size);
	memcpy(vaddrW + start, msg, payload_size);

	//payload_size has to be less than this constant otherwise buffers will overflow
	char new_msg_expected[50];
	char new_msg[50];
	int read_size = payload_size;
	//load the raw message into the expected output for debugging
	memcpy(new_msg_expected, vaddrW + start + offset, payload_size);

	int start_read = start + offset;

	printk("reading size: %d, start offset : %d", read_size, start_read);

	//if offset is negative, we expect an oops here (not_present page)
	//obviously we don't want to return the middle of a mapped address
	printk("raw message is: %s, read_size: %d", (char *)new_msg_expected, payload_size - offset);
	memcpy(new_msg, vaddr + offset, read_size - offset);
	printk("Successful memcpy read - file read is: %s, read_size: %d", (char *)new_msg, payload_size - pre_boundary_offset - offset);

	if (memcmp(new_msg, new_msg_expected, payload_size))
		pr_info("We can't read the start of the message");

	//trivial free stuff

	vunmap(vaddr);
	vunmap(vaddrW);
out:
	__free_pages(head, order);
	fput(file);
	return 0;
}

//init - on load module
int __init load(void)
{
	pr_info("start tests");
	read_test(); // this checks reading to our virtual address
	pr_info("no failure OOPs at this point means vmap_file() is ok");
	return 0;
}

//exit - on unload module
void __exit unload(void)
{

}

module_init(load);
module_exit(unload);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishal Moola");
MODULE_DESCRIPTION("Test module");
