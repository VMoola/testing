#include <linux/init.h> // Needed for __init/__exit macros
#include <linux/module.h> // Required for all modules
#include <linux/printk.h>

int __init load(void);
void __exit unload(void);

/*
 *
 * module_param(varname, vartype, params); - for arguments passed to cmd line
 * module_param_array(varname, vartype, &lenvar, params); - for arrays
 */

static int MAJOR;

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

	/* Userspace can now open the /dev/[name] file using
	 * syscalls like open() and the fops will handle the call by
	 * linking it to the relevant driver fxns.
	 */
	return 0;
}

//exit - on unload module
void __exit unload(void)
{
	pr_info("Goodbye world\n");
}

module_init(load);
module_exit(unload);

static int device_open(struct inode *inode, struct file *file)
{
	printk("opened the device file, we have grabbed a ref\n");
	try_module_get(THIS_MODULE);
	return 0;
}

static int device_close(struct inode *inode, struct file *file)
{
	printk("closed the device file, we have dropped the ref\n");
	module_put(THIS_MODULE);
	return 0;
}

static int device_mmap(struct file *file, struct vm_area_struct *vm)
{
	printk("mmaped the file\n");
	return 0;
}

static long device_ioctl(struct file *file, unsigned int ioctl_enum,
			unsigned long args)
{
	//since args is a pointer, we can cast to what we expect
	//we only need copy_{to,from}_user when we are getting a pointer
	printk("ioctl triggered, cmd number %d args %s\n", ioctl_enum, (char *)args);

	//see lookup_fd... to get a struct file given a file descriptor

	return 0;
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishal Moola");
MODULE_DESCRIPTION("Test module");
