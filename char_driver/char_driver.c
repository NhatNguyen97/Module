#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include "char_driver.h"
#include <linux/uaccess.h>

#define DRIVER_AUTHOR "Nguyen Van Nhat"
#define DRIVER_DESC "Charater Device Driver"
#define DRIVER_VERSION "2"

/****************************** Declare data **************************************/

typedef struct char_dev{
        unsigned char *control_regs;
        unsigned char *status_regs;
        unsigned char *data_regs;
} char_dev_t;

struct _char_drv{
        dev_t dev_num;
        struct class *dev_class;
        struct device *dev;
        char_dev_t *char_hw;
	struct cdev *cdev;
	unsigned int open_cnt;
} char_drv;

/***************************** device specific - START ***************************** */
/* Initialize devide */
int char_drv_init(char_dev_t *hw)
{
	char * buf;
	buf = kzalloc(NUM_DEV_REGS * REG_SIZE, GFP_KERNEL);
	if( !buf )
	{
		return -ENOMEM;
	}
	hw->control_regs = buf;
	hw->status_regs = hw->control_regs + NUM_CTRL_REGS;
	hw->data_regs = hw->status_regs + NUM_STS_REGS;

	/* Initialize value of register */
	hw->control_regs[CONTROL_ACCESS_REG] = 0X03;
	hw->status_regs[DEVICE_STATUS_REG] = 0X03;

	return 0;
}

int char_hw_read_data(char_dev_t *hw, int start_reg, int num_regs, char* kbuf)
{
	int read_bytes = num_regs;
	if ((hw->control_regs[CONTROL_ACCESS_REG] & CTRL_READ_DATA_BIT) == DISABLE)
		return -1;
	if (start_reg > NUM_DATA_REGS)
		return -1;
	if (num_regs > NUM_DATA_REGS)
		return -1;
	if (num_regs > (NUM_DATA_REGS - start_reg))
		read_bytes = NUM_DATA_REGS - start_reg;
	memcpy(kbuf, hw->data_regs + start_reg, read_bytes);
	hw->status_regs[READ_COUNT_L_REG] += 1;
	if (hw->status_regs[READ_COUNT_L_REG] == 0)
		hw->status_regs[READ_COUNT_H_REG] += 1;
	return read_bytes;
}

int char_hw_write_data(char_dev_t *hw, int start_reg, int num_regs, char* kbuf)
{
        int write_bytes = num_regs;
        if ((hw->control_regs[CONTROL_ACCESS_REG] & CTRL_WRITE_DATA_BIT) == DISABLE)
        	return -1;
        if (kbuf == NULL)
        	return -1;
        if (start_reg > NUM_DATA_REGS)
        	return -1;
        if (num_regs > NUM_DATA_REGS)
 	      	return -1;
        if (num_regs > (NUM_DATA_REGS - start_reg)){
        	write_bytes = NUM_DATA_REGS - start_reg;
        	hw->status_regs[DEVICE_STATUS_REG] |= STS_DATAREGS_OVERFLOW_BIT;
        }
        memcpy(kbuf, hw->data_regs + start_reg, write_bytes);
        hw->status_regs[WRITE_COUNT_L_REG] += 1;
        if (hw->status_regs[WRITE_COUNT_L_REG] == 0)
        	hw->status_regs[READ_COUNT_H_REG] += 1;
        return write_bytes;
}

/* Free devide */
void char_hw_exit(char_dev_t *hw)
{
	kfree(hw->control_regs);
}

/***************************** device specific - END *********************************/



/***************************** OS specific - START ***********************************/

/* Initialize driver*/
/*START ..................................*/

static int char_driver_open(struct inode *inode, struct file *filp)
{
        char_drv.open_cnt++;
        printk("Hanlde opened event (%d)\n", char_drv.open_cnt);
        return 0;
} 

static int char_driver_release(struct inode *inode, struct file *filp)
{
        printk("Handle closed event\n");
        return 0;
}
static ssize_t char_driver_read(struct file *flip, char __user *user_buf, size_t len, loff_t *off)
{
	char *kernel_buf = NULL;
	int num_bytes = 0;
	printk("Handle read event start from %lld, %zu bytes\n", *off, len);
	kernel_buf = kzalloc(len, GFP_KERNEL);
	if (kernel_buf == NULL)
		return 0;
	num_bytes = char_hw_read_data(char_drv.char_hw, *off, len, kernel_buf);
	printk("read %d bytes from HW\n", num_bytes);
	if (num_bytes < 0)
		return -EFAULT;
	if (copy_to_user(user_buf, kernel_buf, num_bytes))
		return -EFAULT;
	*off += num_bytes;
	return num_bytes;
}

static ssize_t char_driver_write(struct file *flip, const char __user *user_buf, size_t len, loff_t *off)
{
	char *kernel_buf = NULL;
	int num_bytes = 0;
	printk("Handle write event start from %lld, %zu bytes\n", *off, len);
	kernel_buf = kzalloc(len, GFP_KERNEL);
	if(copy_from_user(kernel_buf, user_buf, len))
		return EFAULT;
	num_bytes = char_hw_read_data(char_drv.char_hw, *off, len, kernel_buf);
	printk("read %d bytes from HW\n", num_bytes);
	if (num_bytes < 0)
		return -EFAULT;
	*off += num_bytes;
	return num_bytes;
}
static struct file_operations fops =
{
        .owner = THIS_MODULE,
        .open  = char_driver_open,
        .release = char_driver_release,
	.read = char_driver_read,
	.write = char_driver_write,
};

static int __init char_driver_init(void)
{		
	/* Declare variable */
	int ret = 0;
	char_drv.dev_num = 0;
	
	/* Allocate memory for device number */
	ret = alloc_chrdev_region(&char_drv.dev_num, 0, 1, "char_device");
	if(ret < 0)
	{
		printk("failed to register device number statically \n");
		goto failed_register_devnum;
	}
	printk("allocated device number (%d,%d)\n", MAJOR(char_drv.dev_num), MINOR(char_drv.dev_num));
	
	/* Create device file */
	char_drv.dev_class = class_create(THIS_MODULE, "class_char_dev"); /* Create class device */
	if(char_drv.dev_class == NULL)
	{
		printk("failed to create a device class\n");
		goto failed_create_class;
	}
	char_drv.dev = device_create(char_drv.dev_class, NULL, char_drv.dev_num, NULL, "char_dev"); /*Create device */
	if(IS_ERR(char_drv.dev))
	{
		printk("failed to create a device\n");
		goto failed_create_device;
	}

	char_drv.char_hw = kzalloc(sizeof(char_dev_t), GFP_KERNEL);
	if ( !char_drv.char_hw)
	{
		printk("failed to allocate data structure of the driver\n");
		ret = -ENOMEM;
		goto failed_init_hw;
	}
	
	char_drv.cdev = cdev_alloc();
	if(char_drv.cdev == NULL)
	{
		printk("failed to allocate cdev structure\n");
		goto failed_allocate_cdev;
	}
	cdev_init(char_drv.cdev, &fops);
	ret = cdev_add(char_drv.cdev, char_drv.dev_num, 1);
	if( ret < 0 )
	{
		printk("failed to add a char device to the system\n");
		goto failed_allocate_cdev;	
	}
	printk("Initialize char driver successfully\n");
	return 0;

	/* Free devide*/
	failed_allocate_cdev:
		char_hw_exit(char_drv.char_hw);
	failed_init_hw:
		kfree(char_drv.char_hw);
	failed_create_device:
		class_destroy(char_drv.dev_class);
	failed_create_class:
		unregister_chrdev_region(char_drv.dev_num, 1);
	failed_register_devnum:
		return ret;
}

static void __exit char_driver_exit(void)
{
	cdev_del(char_drv.cdev);
	char_hw_exit(char_drv.char_hw);
	device_destroy(char_drv.dev_class, char_drv.dev_num);
	class_destroy(char_drv.dev_class);
	unregister_chrdev_region(char_drv.dev_num, 1);
	printk("Exit char driver\n");
}


/***************************** OS specific - END **********************************/

module_init(char_driver_init);
module_exit(char_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_VERSION(DRIVER_VERSION);
MODULE_SUPPORTED_DEVICE("test device");

