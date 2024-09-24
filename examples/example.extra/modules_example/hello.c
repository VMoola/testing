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


//init - on load module
int __init load(void)
{
	pr_info("Hello world");
	return 0;
}

//exit - on unload module
void __exit unload(void)
{
	pr_info("Goodbye world");
}

module_init(load);
module_exit(unload);

MODULE_LICENSE("This is necessary for modules");
MODULE_AUTHOR("Vishal Moola");
MODULE_DESCRIPTION("Test module");
//MODULE_PARM_DESC(varname, "Deets");
