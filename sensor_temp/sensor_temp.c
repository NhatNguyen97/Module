#include <linux/module.h> 
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>

#define DRIVER_AUTHOR "Nhat Nguyen"
#define DRIVER_DESC   "sensor_temperature"

/************************* Declare *******************************/
int gpioSENSOR = 26; /* use GPIO 26*/


/* create struct sensor_drv */
struct _sensor_drv
{
	dev_t sensor_num; /*device number*/
	struct class *sensor_class; /* class led*/
	struct device *sensor;
	struct cdev *vcdev;
	unsigned int open_cnt;
}sensor_drv;

/************************** LED Device ***************************/

void request_sensor(void)
{
	gpio_request(gpioSENSOR,"sensor");
	gpio_direction_output(gpioSENSOR,1);
	gpio_set_value(gpioSENSOR,0);
	msleep(18);
	gpio_set_value(gpioSENSOR,1);
	udelay(40);
	gpio_direction_input(gpioSENSOR);
	printk("request successfully!");
}

void response_sensor(void)
{
	int c=0;
	printk("gpio1: %d",gpio_get_value(gpioSENSOR));
	while( 1 == gpio_get_value(gpioSENSOR) && c <1000000)
	{	c++;
		udelay(40);
	}
	printk("c: %d",c);
	printk("gpio2: %d",gpio_get_value(gpioSENSOR));
	printk("response successfully!");
}
void sensor_hw_read_data(void)
{
	int buffer[5];
	int i,j,checksum,temp,humi;
	/*
	gpio_request(gpioSENSOR, "sensor");
	gpio_direction_output(gpioSENSOR,1);
	gpio_set_value(gpioSENSOR,1);
	udelay(10);
	gpio_set_value(gpioSENSOR,0);
	udelay(30);
	gpio_set_value(gpioSENSOR,1);
	// Direct to output
	gpio_direction_input(gpioSENSOR);
	udelay(10);
	// Get current value
	
	printk("current state: %d",gpio_get_value(gpioSENSOR));
	if(1 == gpio_get_value(gpioSENSOR))
	{
		printk("failed 1");
	       	return -1;
	}
	else
		while(0 == gpio_get_value(gpioSENSOR));
	udelay(30);
	if(0 == gpio_get_value(gpioSENSOR))
	{	
		printk("failed 0");
		return -1;
	}
	else 
		while(1 == gpio_get_value(gpioSENSOR));
	*/
	request_sensor();
	response_sensor();
	//Read data
	/*
	for(i=0 ; i<5 ; i++)
	{
		for(j=0 ; j<8 ; j++)
		{
			while(0 == gpio_get_value(gpioSENSOR));
			udelay(30);
			if(1 == gpio_get_value(gpioSENSOR))
			{
				buffer[i] = (1 << (7-j));
				while(gpio_get_value(gpioSENSOR));
			}
		}
	}
	//Checksum
	checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
	if(checksum != buffer[4])
		return -1;
	//Get data
	temp = buffer[2];
	humi  = buffer[0];
	printk("temperature: %d\n",temp);
	printk("humidity: %d\n",humi);
	*/
}

/************************** LED OS ******************************/

/* entry points function */
static int sensor_driver_open(struct inode *inode, struct file *filp)
{
	sensor_drv.open_cnt ++;
	printk("Handle opened event (%d)\n", sensor_drv.open_cnt);
	return 0;
}

static int sensor_driver_release(struct inode *inode, struct file *filp)
{
	printk("Handle closed event\n");
	return 0;
}
/*
static ssize_t sensor_driver_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
	char *kernel_buf = NULL;
	printk("Handle write event start from %lld, %zu bytes\n", *off, len);
	kernel_buf = kzalloc(len, GFP_KERNEL);
	kernel_buf = kzalloc(len, GFP_KERNEL);
	if (copy_from_user(kernel_buf, user_buf, len))
		return -EFAULT;
	printk("%s",kernel_buf);
	
	sensor_hw_read_data(user_buf);	
	return 0;
} */

static ssize_t sensor_driver_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{
	sensor_hw_read_data();
	return 0;
}


static struct file_operations fops =
{
	.owner 	= THIS_MODULE,
	.open	= sensor_driver_open,
	.release = sensor_driver_release,
	.read 	= sensor_driver_read, 
};


/* Initialized led*/
static int __init init_sensor_blink(void)
{
	int ret = 0;
	
	/* Allocate device number */
	//sensor_drv.sensor_num = MKDEV(500,0);
	ret = alloc_chrdev_region(&sensor_drv.sensor_num, 0, 1, "led_device");
	if( ret < 0 )
	{
		printk("failed to register device numver dynamically\n");
		goto failed_register_sensor_num;
	}
	printk("allocated device number (%d%d)\n", MAJOR(sensor_drv.sensor_num), MINOR(sensor_drv.sensor_num));
	
	/* Create device file */
	/* Create class device */
	sensor_drv.sensor_class = class_create(THIS_MODULE, "class_sensor_device");
	if(sensor_drv.sensor_class == NULL)
	{
		printk("failed to create a device class\n");
		goto failed_create_class;
	}
	/* Create specfic device file*/
	sensor_drv.sensor = device_create(sensor_drv.sensor_class, NULL, sensor_drv.sensor_num, NULL, "sensor_device");
	if(IS_ERR(sensor_drv.sensor))
	{
		printk("failed to create a device\n");
		goto failed_create_device;
	}

	/* Register entry points with kernel */
	sensor_drv.vcdev = cdev_alloc();
	if (sensor_drv.vcdev == NULL)
	{
		printk("failed to allocate cdev structure\n");
		goto failed_allocate_cdev;	
	}	
	cdev_init(sensor_drv.vcdev, &fops);
	ret = cdev_add(sensor_drv.vcdev, sensor_drv.sensor_num, 1);
	if (ret < 0)
	{
		printk("failed to add a char device to the system\n");
		goto failed_allocate_cdev;
	}   	
	return 0;
	
	/* Return failed */
	failed_allocate_cdev:
		device_destroy(sensor_drv.sensor_class, sensor_drv.sensor_num);
	failed_create_device:
		class_destroy(sensor_drv.sensor_class);
	failed_create_class:
		unregister_chrdev_region(sensor_drv.sensor_num,1);
	failed_register_sensor_num:
		return ret;
}

/* Exit led*/
static void __exit exit_sensor_blink(void)
{
	/* Delete entry point*/
	cdev_del(sensor_drv.vcdev);
	/* Delete device file */
	device_destroy(sensor_drv.sensor_class, sensor_drv.sensor_num);
	class_destroy(sensor_drv.sensor_class);
	/* Dealocate device number */
	unregister_chrdev_region(sensor_drv.sensor_num, 1);
	/* Notice to screen */
	printk("sensor was off ...\n");
   	gpio_set_value(gpioSENSOR, 0);         
   	gpio_unexport(gpioSENSOR);
      	gpio_free(gpioSENSOR);	
}

module_init(init_sensor_blink);
module_exit(exit_sensor_blink);

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR(DRIVER_AUTHOR); 
MODULE_DESCRIPTION(DRIVER_DESC); 
