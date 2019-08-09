#include <linux/module.h> 
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>
#define DRIVER_AUTHOR "Nhat Nguyen"
#define DRIVER_DESC   "led_blink"

int gpioLED = 26; /* use GPIO 26*/

/* create struct led_drv */
struct _led_drv
{
	dev_t led_num; /*device number*/
	struct class *led_class; /* class led*/
	struct device *led;
	struct cdev *vcdev;
	unsigned int open_cnt;
}led_drv;

/************************** LED Device ***************************/
void led_hw_write_data(char *value_crl)
{
	
	printk("led is lighting ...\n");
   	gpio_request(gpioLED, "sysfs");          
   	gpio_direction_output(gpioLED, *value_crl);   
   	gpio_set_value(gpioLED, *value_crl);          
   	gpio_export(gpioLED, true);  
}

/************************** LED OS ******************************/

/* entry points function */
static int led_driver_open(struct inode *inode, struct file *filp)
{
	led_drv.open_cnt ++;
	printk("Handle opened event (%d)\n", led_drv.open_cnt);
	return 0;
}

static int led_driver_release(struct inode *inode, struct file *filp)
{
	printk("Handle closed event\n");
	return 0;
}

static ssize_t led_driver_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
	char *kernel_buf = NULL;
	printk("Handle write event start form %lld, %zu bytes\n", *off, len);
	kernel_buf = kzalloc(len, GFP_KERNEL);
	if (copy_from_user(kernel_buf, user_buf, len))
		return -EFAULT;
	led_hw_write_data(kernel_buf);
	return 0;
}
 

static struct file_operations fops =
{
	.owner 	= THIS_MODULE,
	.open	= led_driver_open,
	.release = led_driver_release,
	.write 	= led_driver_write, 
};


/* Initialized led*/
static int __init init_led_blink(void)
{
	int ret = 0;
	
	/* Allocate device number */
	led_drv.led_num = MKDEV(500,0);
	ret = alloc_chrdev_region(&led_drv.led_num, 0, 1, "led_device");
	if( ret < 0 )
	{
		printk("failed to register device numver dynamically\n");
		goto failed_register_led_num;
	}
	printk("allocated device number (%d%d)\n", MAJOR(led_drv.led_num), MINOR(led_drv.led_num));
	
	/* Create device file */
	/* Create class device */
	led_drv.led_class = class_create(THIS_MODULE, "class_led_device");
	if(led_drv.led_class == NULL)
	{
		printk("failed to create a device class\n");
		goto failed_create_class;
	}
	/* Create specfic device file*/
	led_drv.led = device_create(led_drv.led_class, NULL, led_drv.led_num, NULL, "led_device");
	if(IS_ERR(led_drv.led))
	{
		printk("failed to create a device\n");
		goto failed_create_device;
	}

	/* Register entry points with kernel */
	led_drv.vcdev = cdev_alloc();
	if (led_drv.vcdev == NULL)
	{
		printk("failed to allocate cdev structure\n");
		goto failed_allocate_cdev;	
	}	
	cdev_init(led_drv.vcdev, &fops);
	ret = cdev_add(led_drv.vcdev, led_drv.led_num, 1);
	if (ret < 0)
	{
		printk("failed to add a char device to the system\n");
		goto failed_allocate_cdev;
	}   	
	return 0;
	
	/* Return failed */
	failed_allocate_cdev:
		device_destroy(led_drv.led_class, led_drv.led_num);
	failed_create_device:
		class_destroy(led_drv.led_class);
	failed_create_class:
		unregister_chrdev_region(led_drv.led_num,1);
	failed_register_led_num:
		return ret;
}

/* Exit led*/
static void __exit exit_led_blink(void)
{
	/* Delete entry point*/
	cdev_del(led_drv.vcdev);
	/* Delete device file */
	device_destroy(led_drv.led_class, led_drv.led_num);
	class_destroy(led_drv.led_class);
	/* Dealocate device number */
	unregister_chrdev_region(led_drv.led_num, 1);
	/* Notice to screen */
	printk("led was light off ...\n");
   	gpio_set_value(gpioLED, 0);         
   	gpio_unexport(gpioLED);  
}

module_init(init_led_blink);
module_exit(exit_led_blink);

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR(DRIVER_AUTHOR); 
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_SUPPORTED_DEVICE("led_blink"); 
