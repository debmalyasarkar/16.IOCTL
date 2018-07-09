
typedef struct dev_buff_t
{
  int totalSize;
  int index;
  uint8_t data;
}dev_buff;

/* VIRTUAL DEVICE WITH BUFFER */

#define DEV_BUFF_NAME   "buffDevice"

/* IOCTL Macros for Buffer Operations */
#define DEV_BUFF_MAGIC 'D'

#define GET_SIZE        _IOR(DEV_BUFF_MAGIC,1,dev_buff *)
#define GET_INDEX_DATA  _IOR(DEV_BUFF_MAGIC,2,dev_buff *)
#define FILL_BUFFER     _IOW(DEV_BUFF_MAGIC,3,uint8_t)
#define CLEAR_BUFFER    _IOW(DEV_BUFF_MAGIC,4,uint8_t)
