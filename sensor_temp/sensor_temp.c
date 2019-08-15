#include <linux/module.h> 
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#define DRIVER_AUTHOR "Nhat Nguyen"
#define DRIVER_DESC   "sensor_temperature"

#define HIGH 1
#define LOW 0
#define DHT_PULSES 41
#define DHT_MAXCOUNT 32000

/************************* Declare *******************************/
#define gpioSENSOR 26 /* use GPIO 26*/


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

void sensor_hw_read_data(char *kernel_buf)
{
	int i=0,threshold=0;
	int pulseCounts[DHT_PULSES*2] = {0};
	int dht11_dat[5] = {0};
	gpio_request(gpioSENSOR,"sensor");
	gpio_direction_output(gpioSENSOR,HIGH);
	gpio_set_value(gpioSENSOR,LOW);
	mdelay(20);
	gpio_set_value(gpioSENSOR,HIGH);
	udelay(35);
	printk("MCU request successfully!");
	gpio_direction_input(gpioSENSOR);
	udelay(80);
	printk("DH11 response successfully!");

	for (i=0; i < DHT_PULSES*2; i+=2)
	{
		while(0 == gpio_get_value(gpioSENSOR))
		{
			if(++pulseCounts[i] >= DHT_MAXCOUNT)
			{
				return -1;
			}
		
		}
		while(1 == gpio_get_value(gpioSENSOR))
		{
			if(++pulseCounts[i+1] >= DHT_MAXCOUNT)
			{
				pulseCounts[i+1] = DHT_MAXCOUNT;
				return -1;
			}
		}
	}
	for (i=2; i < DHT_PULSES*2; i+=2)
	{
		threshold += pulseCounts[i];
	}
	threshold /= DHT_PULSES-1;
	for (i=3; i < DHT_PULSES*2; i+=2) 
	{
		int index = (i-3)/16;
		dht11_dat[index] <<= 1;
		if (pulseCounts[i] >= threshold) {
			dht11_dat[index] |= 1;
		}
	}	
	if ( dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) )
	{
		
		for(i=0 ; i<4 ; i++)
		{
			kernel_buf[i] = dht11_dat[i];
		}
		//printk("kernel_buf: %s",kernel_buf);
		//f = dht11_dat[2] * 9. / 5. + 32;
		//printk("Humidity = %d.%d %% Temperature = %d.%d *Cn",
		//	(int)dht11_dat[0],(int) dht11_dat[1],(int) dht11_dat[2],(int)dht11_dat[3]);
	}
	else
 	{
		printk("Data not good, skip\n" );
	}
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

static ssize_t sensor_driver_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{
	char *kernel_buf = NULL;
	kernel_buf = kzalloc(5, GFP_KERNEL);
	sensor_hw_read_data(kernel_buf);
	if( copy_to_user(user_buf, kernel_buf, 5) )
		return -EFAULT;
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
