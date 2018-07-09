#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include "cmos_rtc.h"

#define SUCCESS  0

static int inuse = 0;

/* Function to Get any RTC Parameter */
uint8_t get_rtc_val(uint8_t param)
{
  outb(param, ADDRESS_REG);
  return inb(DATA_REG);
}

/* Function to Set any RTC Parameter */
void set_rtc_val(uint8_t param, uint8_t setVal)
{
   outb(param, ADDRESS_REG);
   outb(setVal, DATA_REG);
}

static int cmos_rtc_open(struct inode *inode, struct file *file)
{
  if(inuse)
  {
    pr_err("Device Busy %s\r\n",CMOS_RTC_NAME);
    return -EBUSY;
  }
  inuse = 1;
  pr_info("Open Operation Invoked\r\n");
  return SUCCESS;
}

static int cmos_rtc_release(struct inode *inode, struct file *file)
{
  inuse = 0;
  pr_info("Release Operation Invoked\r\n");
  return SUCCESS;
}

static ssize_t cmos_rtc_read(struct file *file, char __user *buf, size_t size, loff_t *off)
{
  int retval = 0;

  rtc_t rtcObj = {0};

  if(size != sizeof(rtcObj))
  {
    pr_err("Invalid Request\r\n");
    return -EINVAL;
  }
  pr_info("Read Operation Invoked\r\n");

  rtcObj.second = get_rtc_val(SECOND);
  rtcObj.minute = get_rtc_val(MINUTE);
  rtcObj.hour   = get_rtc_val(HOUR);
  rtcObj.day    = get_rtc_val(DAY);
  rtcObj.month  = get_rtc_val(MONTH);
  rtcObj.year   = get_rtc_val(YEAR);
  
  retval  = copy_to_user(buf, &rtcObj, size);

  return retval;
}

static ssize_t cmos_rtc_write(struct file *file, const char __user *buf, size_t size, loff_t *off)
{
  int retval = 0;
  
  rtc_t rtcObj = {0};

  if(size != sizeof(rtcObj))
  {
    pr_err("Invalid Request\r\n");
    return -EINVAL;
  }
  pr_info("Write Operation Invoked\r\n");

  retval = copy_from_user(&rtcObj, buf, size);

  set_rtc_val(SECOND, rtcObj.second);
  set_rtc_val(MINUTE, rtcObj.minute);
  set_rtc_val(HOUR,   rtcObj.hour);
  set_rtc_val(DAY,    rtcObj.day);
  set_rtc_val(MONTH,  rtcObj.month);
  set_rtc_val(YEAR,   rtcObj.year);

  return retval;
}

static long cmos_rtc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  /* This parameter can be either a data or an address, here we need to save it into
     an appropriate datatype depending on the application */
  int *ptr = (int *)arg;

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
    case SET_SECOND:
      set_rtc_val(SECOND, *ptr);
      break;

    case SET_MINUTE:
      set_rtc_val(MINUTE, *ptr);
      break;

    case SET_HOUR:
      set_rtc_val(HOUR, *ptr);
      break;

    case SET_DAY:
      set_rtc_val(DAY, *ptr);
      break;

    case SET_MONTH:
      set_rtc_val(MONTH, *ptr);
      break;

    case SET_YEAR:
      set_rtc_val(YEAR, *ptr);
      break;

    case GET_SECOND:
      *ptr = get_rtc_val(SECOND);
      break;

    case GET_MINUTE:
      *ptr = get_rtc_val(MINUTE);
      break;

    case GET_HOUR:
      *ptr = get_rtc_val(HOUR);
      break;

    case GET_DAY:
      *ptr = get_rtc_val(DAY);
      break;

    case GET_MONTH:
      *ptr = get_rtc_val(MONTH);
      break;

    case GET_YEAR:
      *ptr = get_rtc_val(YEAR);
      break;

    default:
      break;
  }
  pr_info("IOCTL Operation Invoked\r\n");
  return 0;
}

static struct file_operations cmos_rtc_fops = {
  .owner   = THIS_MODULE,
  .open    = cmos_rtc_open,
  .release = cmos_rtc_release,
  .read    = cmos_rtc_read,
  .write   = cmos_rtc_write,
  .unlocked_ioctl = cmos_rtc_ioctl
};

static struct miscdevice cmos_rtc_misc = {
  .minor = MISC_DYNAMIC_MINOR,
  .name  = CMOS_RTC_NAME,
  .fops  = &cmos_rtc_fops
};

/* Register Driver with Misc Interface */
static int __init cmos_rtc_init(void)
{
  int retval;

  /* Register a Miscellaneous Device */
  /* If MISC_DYNAMIC_MINOR is Specified then tries to allocate dynmically */
  /* Otherwise tries to assign specified Minor Number */
  /* Automatically creates an entry in /dev */
  retval = misc_register(&cmos_rtc_misc);
  if(retval < 0)
  {  
    pr_err("Device Registration Failed with Minor Number %d\r\n",cmos_rtc_misc.minor);
    return retval;
  }
  pr_info("Device Registered : %s with Minor Number : %d\r\n",CMOS_RTC_NAME, cmos_rtc_misc.minor);
  return SUCCESS;
}

/* Unregister Driver */
static void __exit cmos_rtc_exit(void)
{  
  /* Unregister the Miscellaneous Device */
  misc_deregister(&cmos_rtc_misc);

  pr_info("Device Unregistered : %s with Minor Number : %d\r\n",CMOS_RTC_NAME, cmos_rtc_misc.minor);
}

/* Register the names of custom entry and exit routines */
module_init(cmos_rtc_init);
module_exit(cmos_rtc_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Driver to test IOCTL commands with CMOS RTC");
MODULE_LICENSE("GPL");

