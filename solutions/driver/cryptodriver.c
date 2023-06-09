#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/io.h>   //required for ioremap
#define  DEVICE_NAME "CRYPTOCORE"    ///< The device will appear at /dev/CRYPTOCORE using this value
#define  CLASS_NAME  "LM"        ///< The device class -- this is a character device driver
 
MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Giuseppe La Capra");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver for a crypto core");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users


#define BASE_ADDRESS 0x43c00000
#define OFFSET_INPUT_LOW 0x0 
#define OFFSET_INPUT_HIGH 0x4
#define OFFSET_KEY_LOW 0x8
#define OFFSET_KEY_HIGH 0xC
#define OFFSET_CONTROL_IN 0x10
#define OFFSET_OUTPUT_LOW 0x0
#define OFFSET_OUTPUT_HIGH 0x4
#define OFFSET_CONTROL_OUT 0x8

#define LM_MAGIC_NUMBER 0XFF
#define ENCRYPT _IOW(LM_MAGIC_NUMBER, 1, int)
#define DECRYPT _IOW(LM_MAGIC_NUMBER, 2, int)
#define RESTART _IOW(LM_MAGIC_NUMBER, 3, int)
#define DONE _IOW(LM_MAGIC_NUMBER, 4, int)

//definition of a custom macro for pointer arithmetics
#define PNTR_ARITH(x,y) (void __iomem*)((size_t)(x) + (size_t)(y))


 
static int    majorNumber;                  ///< Stores the device number -- determined automatically
static struct class*  LMcharClass  = NULL; ///< The device-driver class struct pointer
static struct device* LMcharDevice = NULL; ///< The device-driver device struct pointer
static void __iomem * virtual_address = NULL;
 
// The prototype functions for the character driver -- must come before the struct definition
static int     LM_open_close(struct inode *, struct file *);
static ssize_t LM_read(struct file *, char *, size_t, loff_t *);
static ssize_t LM_write(struct file *, const char *, size_t, loff_t *);
static long int LM_ioctl(struct file *, unsigned int , unsigned long ); 
/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .owner = THIS_MODULE,	
   .open = LM_open_close,
   .read = LM_read,
   .write = LM_write,
   .release = LM_open_close,
   .unlocked_ioctl = LM_ioctl,
   .compat_ioctl = LM_ioctl
};


/** @brief Function to provide commands to the device 
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param cmd A command
 *  @param arg The arguments for the command 
 */

static long int LM_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
	long int flag = 0;
	switch (cmd) {
		case ENCRYPT:
			
			writel((u32)0x00000001,PNTR_ARITH(virtual_address,OFFSET_CONTROL_IN));

			break;
		case DECRYPT:
		
			writel((u32)0x00000003,PNTR_ARITH(virtual_address,OFFSET_CONTROL_IN));
			
			break;
		case RESTART:
		
			writel((u32)0x00000004,PNTR_ARITH(virtual_address,OFFSET_CONTROL_IN));

			break;
		case DONE:
		
			if (readl(PNTR_ARITH(virtual_address,OFFSET_CONTROL_OUT)) == (u32)0x00000004){
				flag = 1;
			}

			break;
	}
	
	
   	printk(KERN_INFO "LM: Executing IOCTL\n");
	
	return flag;
}


/** @brief The device release function that is called whenever the device is initialized/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */

static int  LM_open_close(struct inode* inodep, struct file * filep) {
	/*
	selected_register = ENABLE;
	WRITE_DATA_TO_THE_HW(0);
	selected_register = BLINK_RATE;
	WRITE_DATA_TO_THE_HW(0);
	*/
	
   	printk(KERN_INFO "LM: Executing OPENRELEASE\n");
	return 0;

}


/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t LM_read(struct file * filep, char * buffer, size_t len, loff_t * offset) {
	
	// check if 
	
  	u32 data;
  	
  	unsigned long error;
	
	if((*offset % 4) != 0 || *offset > 12){
		
		printk(KERN_INFO "Offset out of range\n");
		return -1;
		
	}
	
	data = readl(PNTR_ARITH(virtual_address,*offset));
	
	// access_ok() checks for invalid pointer or block memory, already done by copy_to_user
	error = copy_to_user(buffer, &data, sizeof(u32));
	if(error != 0){
		
		printk(KERN_INFO "INTERNAL ERROR: couldn't read all bytes of data\n");
		return -1;
	}
	
	
	
   	printk(KERN_INFO "LM: Executing READ\n");
	return 4;
}


/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. 
 *  @param file A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t LM_write(struct file * filep, const char * buffer, size_t len, loff_t * offset) {
	//WRITE_DATA_TO_THE_HW( buffer);
	
	
	u32 data;
  	
	// access_ok() checks for invalid pointer or block memory, already done by copy_from_user
	unsigned long error = copy_from_user(&data, buffer, sizeof(uint32_t));
	if(error != 0){
		
		printk(KERN_INFO "INTERNAL ERROR: couldn't read all bytes of data\n");
		return -1;
	}
	
	if((*offset % 4) != 0 || *offset > 16){
		
		printk(KERN_INFO"Offset out of range\n");
		return -1;
		
	}
	
	writel((u32)data,PNTR_ARITH(virtual_address,*offset));
   	printk(KERN_INFO "LM: Executing WRITE\n");
	return 4;	
}

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init LM_module_init(void){

     printk(KERN_INFO "LM: Initializing the LMODULE LKM\n");
  
   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "LM failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "LM: registered correctly with major number %d\n", majorNumber);
 
   // Register the device class
   LMcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(LMcharClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(LMcharClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "LM: device class registered correctly\n");
 
   // Register the device driver
   LMcharDevice = device_create(LMcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(LMcharDevice)){               // Clean up if there is an error
      class_destroy(LMcharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(LMcharDevice);
   }
   
   //creating virtual memory mapping for device
   virtual_address = ioremap((phys_addr_t)BASE_ADDRESS, (size_t)5*4); //base address and size (5 times 4 bytes)
   if (virtual_address == NULL){               // Clean up if there is an error
      class_destroy(LMcharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device, failure to ioremap\n");
      iounmap(virtual_address);
      return PTR_ERR(LMcharDevice);
   }
   
   //asserts restart
   writel((u32)0x00000004,PNTR_ARITH(virtual_address,OFFSET_CONTROL_IN)); 
   printk(KERN_INFO "trying to restart crypto core");
   int flag = 0;
   while(flag == 0){
   	//checking for successful restart
   	if (readl(PNTR_ARITH(virtual_address,OFFSET_CONTROL_OUT)) == (u32)0x00000001){
	    flag = 1;
	}
   }
   printk(KERN_INFO "restart success");
   //unasserts restart
   writel((u32)0x00000000,PNTR_ARITH(virtual_address,OFFSET_CONTROL_IN));
   
   
   printk(KERN_INFO "LM: device class created correctly\n"); // Made it! device was initialized
   return 0;
}
 
/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit LM_module_exit(void){
   device_destroy(LMcharClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(LMcharClass);                          // unregister the device class
   class_destroy(LMcharClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   
   
   
   printk(KERN_INFO "LM: Goodbye from the LKM!\n");
}
 
 
 
 
 
/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(LM_module_init);
module_exit(LM_module_exit);
