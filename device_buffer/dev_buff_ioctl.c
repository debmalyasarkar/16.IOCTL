#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include "dev_buff.h"

#define SUCCESS  0

/* PAGE_SIZE is the default Macro */
#define NO_OF_PAGES 10
#define SIZE (NO_OF_PAGES * PAGE_SIZE)

static int inuse = 0;
char *ptr = NULL;

static int dev_buff_open(struct inode *inode, struct file *file)
{
  if(inuse)
  {
    pr_err("Device Busy %s\r\n",DEV_BUFF_NAME);
    return -EBUSY;
  }
  inuse = 1;
  pr_info("Open Operation Invoked\r\n");
  return SUCCESS;
}

static int dev_buff_release(struct inode *inode, struct file *file)
{
  inuse = 0;
  pr_info("Release Operation Invoked\r\n");
  return SUCCESS;
}

static ssize_t dev_buff_read(struct file *file, char __user *buf, size_t size, loff_t *off)
{
  int retval = 0;
  copy_to_user(buf,ptr,size);
  pr_info("Read Operation Invoked\r\n");
  return retval;
}

static ssize_t dev_buff_write(struct file *file, const char __user *buf, size_t size, loff_t *off)
{
  int retval = 0;
  copy_from_user(ptr,buf,size);
  pr_info("Write Operation Invoked\r\n");
  return retval;
}

static long dev_buff_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  /* This parameter can be either a data or an address, here we need to save it into
     an appropriate datatype depending on the application */
  dev_buff *p = (dev_buff *)arg;

#if 0
  /* Validating the MAGIC_NUMBER after extracting it from the cmd parameter */
  if(_IOC_TYPE(cmd) != RTC_MAGIC)
    return -ENOTTY;

  /* This step validates the user space address range accessibility */
  /* Takes starting address from arg and extracts the size from cmd */
  /* Prevents segmentation faults due to incorrect parameters from user */

  if(_IOC_DIR(cmd) & _IOC_READ)
    if(!access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd)));
      return -EFAULT;

  if(_IOC_DIR(cmd) & _IOC_WRITE)
    if(!access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd)));
      return -EFAULT;
#endif

  switch(cmd)
  {
    case GET_SIZE:
      p->totalSize = SIZE;
      break;

    case GET_INDEX_DATA:
      p->data = (uint8_t)ptr[p->index];
      break;

    case FILL_BUFFER:
      memset(ptr,arg,SIZE);
      break;

    case CLEAR_BUFFER:
      memset(ptr,0,SIZE);
      break;
  }
  pr_info("IOCTL Operation Invoked\r\n");
  return 0;
}

static struct file_operations dev_buff_fops = {
  .owner   = THIS_MODULE,
  .open    = dev_buff_open,
  .release = dev_buff_release,
  .read    = dev_buff_read,
  .write   = dev_buff_write,
  .unlocked_ioctl = dev_buff_ioctl
};

static struct miscdevice dev_buff_misc = {
  .minor = MISC_DYNAMIC_MINOR,
  .name  = DEV_BUFF_NAME,
  .fops  = &dev_buff_fops
};

/* Register Driver with Misc Interface */
static int __init dev_buff_init(void)
{
  int retval;

  /* Register a Miscellaneous Device */
  /* If MISC_DYNAMIC_MINOR is Specified then tries to allocate dynmically */
  /* Otherwise tries to assign specified Minor Number */
  /* Automatically creates an entry in /dev */
  retval = misc_register(&dev_buff_misc);
  if(retval < 0)
  {  
    pr_err("Device Registration Failed with Minor Number %d\r\n",dev_buff_misc.minor);
    return retval;
  }
  pr_info("Device Registered : %s with Minor Number : %d\r\n",DEV_BUFF_NAME, dev_buff_misc.minor);

  /* Allocate Slab */
  ptr = kmalloc(SIZE, GFP_KERNEL);
  if(NULL == ptr)
  {
    pr_err("Slab Allocation Failed\r\n");
    return -ENOMEM;
  }
  return SUCCESS;
}

/* Unregister Driver */
static void __exit dev_buff_exit(void)
{  
  /* Free the allocated slab */
  kfree(ptr);

  /* Unregister the Miscellaneous Device */
  misc_deregister(&dev_buff_misc);

  pr_info("Device Unregistered : %s with Minor Number : %d\r\n",DEV_BUFF_NAME, dev_buff_misc.minor);
}

/* Register the names of custom entry and exit routines */
module_init(dev_buff_init);
module_exit(dev_buff_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Driver to test IOCTL commands with Virtual Buffer Device");
MODULE_LICENSE("GPL");

