#include <linux/init.h>
#include <linux/module.h>

#define DRIVER_AUTHOR "NVN"
#define DRIVER_DESC "LedBlink"
#define DRIVER_VERSION "1"

static int __init led_init(void)
{
	printk("led: starting ...");
	printk("led: starting done.");
	return 0;
}

static void __exit led_exit(void)
{
	printk("led: stopping ...");
	printk("led: stopping done ...");
}

module_init(led_init);
module_exit(led_exit);


MODULE_LICENSE("GPL");

