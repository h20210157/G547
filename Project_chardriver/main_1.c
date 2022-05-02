//header files

#include <linux/kernel.h>      
#include <linux/module.h>      
#include <linux/fs.h>
#include <asm/uaccess.h>       
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/gpio.h> 

#include "chardev.h"              

typedef unsigned char		u8;
#define DEVICE_NAME "adxl_file"	//name of device in dev directory
#define MAJOR_NO 115		//major number of device


#define REG_DEVID 0x00
#define REG_DATA_FORMAT 0x31	//address of data format register in adxl345 sensor

#define REG_POWER_CTL 0x2D	//address of power control register

#define REG_DATAX0 0x32	//address of lower byte of x-axis data register
#define REG_DATAX1 0x33	//address of higher byte of x-axis data register
#define REG_DATAY0 0x34	//address of lower byte of y-axis data register
#define REG_DATAY1 0x35	//address of higher byte of y-axis data register
#define REG_DATAZ0 0x36	//address of lower byte of z-axis data register
#define REG_DATAZ1 0x37	//address of higher byte of z-axis data register

#define SUCCESS 1



struct class *cl;


static struct i2c_client * my_i2c_client;
struct i2c_adapter * my_i2c_adap;
static dev_t first;
static struct cdev c_dev;





//function to read data from adxl sensor
static u8 adxl_read(struct i2c_client *client, u8 reg)
{
	int ret;

	ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0)
		dev_err(&client->dev,
			"can not read register, returned %d\n", ret);

	return (u8)ret;
}
//function to write data in adxl sensor
static int adxl_write(struct i2c_client *client, u8 reg, u8 data)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client, reg, data);
	if (ret < 0)
		dev_err(&client->dev,"can not write register, returned %d\n", ret);

	return ret;
}
//function to print 16 bit X,Y and Z axis data
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off) {	


	printk(KERN_INFO "X_READ = %d\n", adxl_read(my_i2c_client, 0x32) | (adxl_read(my_i2c_client, 0x33)<<8));
	printk(KERN_INFO "Y_READ = %d\n", adxl_read(my_i2c_client, 0x34) | (adxl_read(my_i2c_client, 0x35)<<8));
	printk(KERN_INFO "Z_READ = %d\n", adxl_read(my_i2c_client, 0x36) | (adxl_read(my_i2c_client, 0x37)<<8));
		return 0;
}

long device_ioctl(struct file *file,             
                  unsigned int ioctl_num,      /* number and param for ioctl */  
                  unsigned long ioctl_param)
{
	
	int x, y, z;
	u8 temp;
	u8 range_read, range_user;
	int y1;
//switch case for ioctl calls
   switch (ioctl_num) 
    {
    	case IOCTL_PWR_CTL://device_write(file, temp, 1, 0);
         	temp = adxl_read(my_i2c_client,REG_POWER_CTL);
			temp = temp | (1<<3);
			adxl_write(my_i2c_client, REG_POWER_CTL,temp);

			 break;

    	case ACC_X_READ://ioctl call for reading X axis data
			x = adxl_read(my_i2c_client, REG_DATAX0) + ((adxl_read(my_i2c_client, REG_DATAX1)&0x03)<<8);//getting 16 bits of data both lower and higher 10 bit
			if (x>511)// data is signed and of 10 bit resolution 
			x=x-1024;// so if data is negative take 2's complement of data

			printk("RAW DATA X0 = %x\n", adxl_read(my_i2c_client, REG_DATAY0));
			printk("RAW DATA X1 = %x\n", adxl_read(my_i2c_client, REG_DATAY1));
			raw_copy_to_user((int *)ioctl_param, &x, 2);//function to transfer data from kernel to user space
		
              	break;
        
	case ACC_Y_READ:
			y = adxl_read(my_i2c_client, REG_DATAY0) + ((adxl_read(my_i2c_client, REG_DATAY1)&0x03)<<8);//getting 16 bits of data both lower and higher 10 bit
			if (y>511)// data is signed and of 10 bit resolution 
				y1=y-1024;// so if data is negative take 2's complement of data
			printk(KERN_INFO "y = %d\n", y);
			
			raw_copy_to_user((int *)ioctl_param, &y1, 2);//function to transfer data from kernel to user space
        	break;
        	
        case ACC_Z_READ:
			z = adxl_read(my_i2c_client, REG_DATAZ0) + ((adxl_read(my_i2c_client, REG_DATAZ1)&0x03)<<8);//getting 16 bits of data both lower and higher 10 bit
				if (z>511)// data is signed and of 10 bit resolution
			z=z-1024;// so if data is negative take 2's complement of data
			raw_copy_to_user((int *)ioctl_param, &z, 2);//function to transfer data from kernel to user space
		       	break;
		       	
	case RANGE_SETTING:	//in adxl345 we can set range , so ioctl call is provided to set range
		
		range_read =adxl_read(my_i2c_client,REG_DATA_FORMAT);
		
		raw_copy_from_user(&range_user, (u8 *)ioctl_param, 1);
		if(range_user == 1)	//if range specified by user is 1
			adxl_write(my_i2c_client, REG_DATA_FORMAT, 0x01);	// update REG_DATA_FORMAT	
			
		else if(range_user == 2)//if range specified by user is 2
			adxl_write(my_i2c_client, REG_DATA_FORMAT, 0x02);	// update REG_DATA_FORMAT
			
		else if(range_user == 3)//if range specified by user is 3
			adxl_write(my_i2c_client, REG_DATA_FORMAT, 0x03);	// update REG_DATA_FORMAT
			

	default:
		return 0;
    
    }
    return SUCCESS;
}

static int my_open(struct inode *i, struct file *f)
{
	  printk(KERN_INFO "Driver: open()\n");
	    return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	  printk(KERN_INFO "Driver: close()\n");
	    return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf,
		   size_t len, loff_t *off)
{
	  printk(KERN_INFO "Driver: write()\n");
	    return len;
}

//file operations structure
static struct file_operations my_dev_ops = {.read = my_read,
						.owner = THIS_MODULE,
						.open = my_open,
						.release = my_close,
						.write = my_write,
						.unlocked_ioctl = device_ioctl,
};





//init function
static int __init adxl_init(void)
{

	u8 readvalue,temp; 
        printk(KERN_INFO "ACCELEROMETER DRIVER ADDED\n");
       
        if (alloc_chrdev_region(&first, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_DEBUG "Cannot register device\n");
        return -1;
        }

	if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv"))){
			unregister_chrdev_region(first, 1);
			}

	if (IS_ERR(device_create(cl, NULL, first, NULL, "adxl_file"))) {     //adxl_file is the device file name.
		class_destroy(cl);
		unregister_chrdev_region(first,1);
	}

	cdev_init(&c_dev,&my_dev_ops);	
	if (cdev_add(&c_dev, first,1) < 0)
	{
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first,1);
	return -1;
	}

	my_i2c_adap = i2c_get_adapter(1); // 1 means i2c-1 bus
	if (!(my_i2c_client = i2c_new_dummy_device(my_i2c_adap, 0x53))){//0X53 is the address of sensor adxl345
		printk(KERN_INFO "I2C device couldnt be acquired");
		unregister_chrdev(MAJOR_NO, DEVICE_NAME);
		device_destroy(cl, first);
		class_destroy(cl);
		return -1;
	}
		
	//accelerometer detection	
	readvalue = adxl_read(my_i2c_client,0x00);
	if (readvalue == (0b11100101)) // DEVID register holds a fixed device ID code of 0xE5 (345 octal)
	{
		printk("Accelerometer detected, value = %d\r\n",readvalue);
	}


	return 0;
}
 
//exit function
static void __exit adxl_exit(void)
{
        printk(KERN_INFO "ADXL: Removing module");
	i2c_unregister_device(my_i2c_client);//unregister device
        cdev_del(&c_dev);
 	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);

}


module_init(adxl_init);
module_exit(adxl_exit);

MODULE_DESCRIPTION("driver");
MODULE_AUTHOR("2021H1400157P,2021H1400156P,2021H1400164P");
MODULE_LICENSE("GPL");
