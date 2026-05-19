#include <linux/init.h> // Needed for __init/__exit macros
#include <linux/module.h> // Required for all modules
#include <linux/printk.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>

//Ensure this precisely matches the application!
#define ALLOC _IOWR('a',1,int)
#define FREE _IOWR('a',2,int)
#define RD _IOWR('a', 3, int)

int __init load(void);
void __exit unload(void);

/*
 *
 * module_param(varname, vartype, params); - for arguments passed to cmd line
 * module_param_array(varname, vartype, &lenvar, params); - for arrays
 */

static int MAJOR;

int current_allocs = 0;
void **allocations;

static int device_open(struct inode *inode, struct file *file);
static int device_close(struct inode *inode, struct file *file);
static int device_mmap(struct file *file, struct vm_area_struct *vm);
static long device_ioctl(struct file *file, unsigned int ioctl_enum,
			unsigned long args); //effectively a void *

#define PROC_NAME "my_module"
static int my_knob_value = 0;
static struct proc_dir_entry *proc_entry;


static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
    char kbuf[16];
    int len = snprintf(kbuf, sizeof(kbuf), "%d\n", my_knob_value);

    return simple_read_from_buffer(buf, count, pos, kbuf, len);
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *pos)
{
    char kbuf[16];

    if (count > sizeof(kbuf) - 1)
	    return -EINVAL;
    if (copy_from_user(kbuf, buf, count))
	    return -EFAULT;
    kbuf[count] = '\0';

    // Convert string to integer and update setting
    if(kstrtoint(kbuf, 10, &my_knob_value)) {
	//Failed on overflow or invalid
    }

    return count;
}

/* We've implemented read and write */
static const struct proc_ops my_proc_ops = {
    .proc_read = my_read,
    .proc_write = my_write,
};

//block devices use "struct block_device_operations"
static struct file_operations fops = {
	.open = device_open,
	.release = device_close,
	.mmap = device_mmap,
	.unlocked_ioctl = device_ioctl
};

//init - on load module
int __init load(void)
{
	pr_info("Hello world");
	MAJOR = register_chrdev(0, "test_mod", &fops);

	//Makes a proc entry at /proc/PROC_NAME
	proc_entry = proc_create(PROC_NAME, 0666, NULL, &my_proc_ops);
	/*the command (from userspace) makes the character
	 * device file for our fops to act on.
	 */
	printk("mknod /dev/[name] c %d 0\n", MAJOR);

	//allocations = kmalloc(512 * sizeof(void*), GFP_KERNEL); 	//smaller than PAGE_SIZE bytes
	allocations = (void **)__get_free_page(GFP_KERNEL); /* 4096 */	//size of PAGE_SIZE bytes
	//allocations = vmalloc(512 * sizeof(void*)); 			//larger than PAGE_SIZE

	/* Userspace can now open the /dev/[name] file using
	 * syscalls like open() and the fops will handle the call by
	 * linking it to the relevant driver fxns.
	 */
	return 0;
}

//exit - on unload module
void __exit unload(void)
{
	//kfree(allocations);
	while (current_allocs) {
		struct page *p = allocations[--current_allocs];
		__free_pages(p, compound_order(p));
	};
	free_page((unsigned long)allocations);
	//vfree(allocations);
	pr_info("Goodbye world\n");
	remove_proc_entry(PROC_NAME, NULL);
}

module_init(load);
module_exit(unload);

static int device_open(struct inode *inode, struct file *file)
{
	//printk("opened the device file, we have grabbed a ref\n");
	try_module_get(THIS_MODULE);
	return 0;
}

static int device_close(struct inode *inode, struct file *file)
{
	//printk("closed the device file, we have dropped the ref\n");
	module_put(THIS_MODULE);
	return 0;
}

static int write_to_page(struct page **allocs, unsigned int idx,
			unsigned int len, unsigned long data)
{
	void *vaddr;
	char buf[8] = "HARDCODE";
	struct page *page;

	/*
	 * Obviously, if we kmalloc or vmap we don't need this.
	 * We should just do page_address for 1 page, but this
	 * is just for the example
	 */
	page = allocations[idx];	//index of allocation
	vaddr = vmap((struct page **)&allocations[idx], 1, VM_MAP, PAGE_KERNEL);

	memcpy(vaddr, buf, len);
	vunmap(vaddr);
	return 0;
}

static int read_from_page(struct page **allocs, unsigned int idx,
			unsigned int len, unsigned long data)
{
	void *vaddr;
	char *buf;
	struct page *page;

	buf = kmalloc(len, GFP_KERNEL);
	/*
	 * Obviously, if we kmalloc or vmap we don't need this.
	 * We should just do page_address for 1 page, but this
	 * is just for the example
	 */
	page = allocations[idx];	//index of allocation
	vaddr = vmap((struct page **)&page, 1, VM_MAP, PAGE_KERNEL);

	memcpy(buf, vaddr, len);
	printk("The message from userspace through the page is : %s\n", buf);
	vunmap(vaddr);
	kfree(buf);
	return 0;
}

static int device_mmap(struct file *file, struct vm_area_struct *vm)
{
	if (!current_allocs) {
		//WARN(1, "We don't have any memory to mmap.");
		return -EINVAL;
	}

	vm_insert_page(vm, vm->vm_start, (struct page*) allocations[current_allocs-1]);
	printk("mmaped a page\n");
	return 0;
}

static int alloc(int size)
{
	if (size <= 0) {
		printk("Nothing to alloc\n");
		return -1;
	}
	if (current_allocs >= 512) {
		printk("No more space to alloc\n");
		return -1;
	}

	allocations[current_allocs++] = alloc_pages(GFP_KERNEL | __GFP_COMP, size);
	write_to_page((struct page**) allocations, current_allocs-1, 8, 0);
	return 0;
}

static void frees(int size)
{
	if (current_allocs <= 0) {
		printk("Nothing to free\n");
		return;
	}

	//This will break if size > allocation order, but its just an example
	__free_pages(allocations[--current_allocs], size);
}

static long device_ioctl(struct file *file, unsigned int ioctl_enum,
			unsigned long args)
{
	//since args is a pointer, we can cast to what we expect
	//printk("ioctl called %d\n", ioctl_enum);
	int size;

	//Structurally, this just copies a userspace virtual address
	//This means we can actually capture this address and use it
	//later so long as the virtual address remains alive. Its a
	//bit silly so we don't do it here, but theres definitely use
	//cases for it.
	if (copy_from_user(&size, (int *)args, sizeof(size)))
		pr_err("Failed to store size!\n");
	switch (ioctl_enum) {
		case ALLOC:
			alloc(size);
			break;
		case FREE:
			frees(size);
			break;
		case RD:
			//Read size bytes most recently added page
			//If we change our ioctl to take a different
			//structure, we can supply multiple arguments
			read_from_page((struct page **) allocations,
					current_allocs-1, size, 0);
			break;
	};
	if (copy_to_user((int *)args, &current_allocs, sizeof(current_allocs)))
		pr_err("Failed to report size!\n");
	//see lookup_fd... to get a struct file given a file descriptor

	return current_allocs;
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishal Moola");
MODULE_DESCRIPTION("Test module");
