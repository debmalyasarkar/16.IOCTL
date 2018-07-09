/* CMOS RTC is DS 12887 */

#define CMOS_RTC_NAME   "rtcDevice"

/* Address of RTC Registers */
#define ADDRESS_REG     0x70
#define DATA_REG        0x71

/* Values/Commands for RTC Registers */
#define SECOND          0x00    //Second      00-59
#define MINUTE          0x02    //Minute      00-59
#define HOUR            0x04    //Hour        00-23
#define DAY_OF_WEEK     0x06    //Day of Week 01-0DAY
#define DAY             0x07    //Day         00-31
#define MONTH           0x08    //Month       00-12
#define YEAR            0x09    //Year        00-99

typedef struct _rtc_t
{
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint8_t year;
}rtc_t;

/* IOCTL Macros for RTC Configuration Operations */
#define RTC_MAGIC 'D'

#define SET_SECOND _IOW(RTC_MAGIC,1,uint8_t)
#define SET_MINUTE _IOW(RTC_MAGIC,2,uint8_t)
#define SET_HOUR   _IOW(RTC_MAGIC,3,uint8_t)
#define SET_DAY    _IOW(RTC_MAGIC,4,uint8_t)
#define SET_MONTH  _IOW(RTC_MAGIC,5,uint8_t)
#define SET_YEAR   _IOW(RTC_MAGIC,6,uint8_t)

#define GET_SECOND _IOR(RTC_MAGIC,7 ,uint8_t)
#define GET_MINUTE _IOR(RTC_MAGIC,8 ,uint8_t)
#define GET_HOUR   _IOR(RTC_MAGIC,9 ,uint8_t)
#define GET_DAY    _IOR(RTC_MAGIC,10,uint8_t)
#define GET_MONTH  _IOR(RTC_MAGIC,11,uint8_t)
#define GET_YEAR   _IOR(RTC_MAGIC,12,uint8_t)

