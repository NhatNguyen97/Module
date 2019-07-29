#include <linux/module.h>
#define DRIVER_AUTHOR "Nhat Nguyen"
#define DRIVER_DESC "Example"

static int __init init_hello(void)
{
	printk("Hello VN\n");
	return 0;
}
static void __exit exit_hello(void)
{
	printk("Goodbye VN\n");
}

module_init(init_hello);
module_exit(exit_hello);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
//MODULE_DESCRIPTION(DRIVER_DESC);
//MODULE_SUPPORT_DEVIDE("test driver");
