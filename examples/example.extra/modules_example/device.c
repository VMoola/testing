#include <linux/init.h> // Needed for __init/__exit macros
#include <linux/module.h> // Required for all modules
#include <linux/printk.h>
#include <linux/vmalloc.h>

//Ensure this precisely matches the application!
#define ALLOC _IOWR('a',1,int)
#define FREE _IOWR('a',2,int)

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
	free_page((unsigned long)allocations);
	//vfree(allocations);
	pr_info("Goodbye world\n");
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

static int device_mmap(struct file *file, struct vm_area_struct *vm)
{
	//printk("mmaped the file\n");
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
	if (copy_from_user(&size, (int *)args, sizeof(size)))
		pr_err("Failed to store size!\n");
	switch (ioctl_enum) {
		case ALLOC:
			alloc(size);
			break;
		case FREE:
			frees(size);
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
