//Assignment 1,  Vishal Chekale , 2021H1400157P

//kernel version   
#include <linux/kernel.h>       // We're doing kernel work 
#include <linux/module.h>       // Specifically, a module 
#include <linux/fs.h>
#include <linux/uaccess.h>       // for get user and put user access
#include <linux/time.h>
#include<linux/init.h>
#include <linux/cdev.h>
#include<linux/types.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/random.h>
#include <linux/ioctl.h>


#define MAGIC_NUM 100


#define IOCTL_GET_allignment _IOW(MAGIC_NUM, 0, char *) //select alignment of data in register

#define IOCTL_GET_Channel _IOW(MAGIC_NUM, 1, int *) //select channel of adc

#define SEL_CONV _IOR(MAGIC_NUM, 3, int *)    //Select the conversion mode(single shot or continuous) 

#define DEVICE_FILE_NAME "/dev/adc-dev"
#define SUCCESS 0
#define DEVICE_NAME "adc-dev"

static dev_t first;        // variable for device number
static struct cdev c_dev;  // variable for the character device structure
static struct class *cls;  // variable for the device class
//static unit16_t ch=0;
static char al='r';

/*
 * Is the device open right now? Used to prevent
 * concurent access into the same device
 */
static int Device_Open = 0;

 static int ch_no;
 static char align;

 uint16_t adc_val,i ;

 
 //file open 
static int adc_open(struct inode *inode, struct file *file)
{

        printk(KERN_INFO "device_open(%p)\n", file);


    //To avoid intantiating two process simultaneously
    if (Device_Open)
        return -EBUSY;

    Device_Open++;
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

static int adc_close(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "device_release(%p,%p)\n", inode, file);

    Device_Open--;

    module_put(THIS_MODULE);
    return SUCCESS;
}

/*
 * This function is called whenever a process which has already opened the
 * device file attempts to read from it.
 */
static ssize_t adc_read(struct file *file,   // see include/linux/fs.h   
                           char __user * buffer,        /* buffer to be
                                                         * filled with data */
                           size_t length,       /* length of the buffer     */
                           loff_t * offset)
{
   
    int bytes_read = 0;

    get_random_bytes(&i,sizeof(i));

        if(align=='L')               //for left allignment
         { 
           adc_val= 16*(i%4096);    // one bit left shift means *2 so 4 bit leftshift is *16
           printk("Shifted Value : %d ",adc_val);
         }
	else if (align=='R')           //for right allignment
	{
	  adc_val= i%4096 ;           
        }
        int retval= copy_to_user(buffer, &adc_val , sizeof(adc_val));

	return retval;



    

    return bytes_read;
}



/*
 * This function is called whenever a process tries to do an ioctl on our
 * device file.
 */
long adc_ioctl(struct file *file,             /* ditto */
                  unsigned int ioctl_num,        /* number and param for ioctl */
                  unsigned long ioctl_param)
{
    int *temp_ch_no;
    char *temp_align;

    /*
     * Switch according to the ioctl called
     */
    switch (ioctl_num) {
    case IOCTL_GET_allignment:
         temp_align = (char *)ioctl_param;
         get_user(align,temp_align);
         printk(KERN_INFO "Channel alignment - %c is selected\n",align);
         

         break;

    case IOCTL_GET_Channel:
         temp_ch_no = (int *)ioctl_param;
         get_user(ch_no,temp_ch_no);
         printk(KERN_INFO "Channel - %d is selected\n",ch_no);
         

         break;
     case SEL_CONV:
          al = ioctl_param;

         //printk(KERN_INFO "current request code for conversion is %u",ioctl_num);
          printk(KERN_INFO "conv SELECTED - %c",al);
         break;

        }

    return SUCCESS;
}

/* Module Declarations */

/*
 *flie operations mapping to system calls
 */
struct file_operations fops = {
        .read = adc_read,
        .unlocked_ioctl = adc_ioctl,
        .open = adc_open,
        .release = adc_close,      /* close */
};

/*
 * Initialize the module - Register the character device
 */
int myadc_init(void)
{
   // reserve <major, minor>
	if (alloc_chrdev_region(&first, 0, 1, "adc-dev") < 0)
	{
		return -1;
	}
	
	// dynamically create device node in /dev directory
    if ((cls = class_create(THIS_MODULE, "charadc")) == NULL)
	{
		unregister_chrdev_region(first, 1);
		return -1;
	}
    if (device_create(cls, NULL, first, NULL, DEVICE_NAME ) == NULL)
	{
		class_destroy(cls);
		unregister_chrdev_region(first, 1);
		return -1;
	}
	
	// Link fops and cdev to device node
    cdev_init(&c_dev, &fops);
    if (cdev_add(&c_dev, first, 1) == -1)
	{
		device_destroy(cls, first);
		class_destroy(cls);
		unregister_chrdev_region(first, 1);
		return -1;
	}
      
    	printk(KERN_INFO "ADC driver is registered successfully\n");

   
    return 0;
}

/*
 * Cleanup - unregister the appropriate file from /proc
 */
void myadc_exit(void)
{

        cdev_del(&c_dev);
	device_destroy(cls, first);
	class_destroy(cls);
	unregister_chrdev_region(first, 1);
	printk(KERN_INFO "ADC driver unregistered\n\n");


}


module_init(myadc_init);
module_exit(myadc_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishal Chekale");
MODULE_DESCRIPTION("12 bit ADC Driver");

