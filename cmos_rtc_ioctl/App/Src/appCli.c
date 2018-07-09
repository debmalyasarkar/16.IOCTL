#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <cli.h>
#include <errno.h>
#include <appCli.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <../../cmos_rtc.h>

#define DEVICE_FILE_NAME "/dev/rtcDevice" 

int fd;
rtc_t rdbuff;
rtc_t wrbuff;

extern int quitFlag;

int openFile(int argc,char *argv[])
{
  fd = open(DEVICE_FILE_NAME, O_RDWR);
  if(fd < 0)
  {
    perror("Open Failed : ");
    return FAILURE;
  }
  printf("File Open Success\n");
  return SUCCESS;
}

int readFile(int argc,char *argv[])
{
  int retval;

  retval = read(fd, &rdbuff, sizeof(rdbuff));
  if(retval < 0)
  {
    perror("Read Failed : ");
    return FAILURE;
  }
  printf("File Read Success\n");

  printf("%02x:%02x:%02x, %02x/%02x/%02x\n", 
    rdbuff.hour, rdbuff.minute, rdbuff.second, 
    rdbuff.day , rdbuff.month , rdbuff.year);

  return SUCCESS;
}

int writeFile(int argc,char *argv[])
{
  int retval;

  if(argc != 3)
  {
    printf("Usage <Cmd> <hhmmss> <ddmmyy>\n");
    return FAILURE;
  }
  sscanf(argv[1],"%hhx:%hhx:%hhx",&wrbuff.hour,&wrbuff.minute,&wrbuff.second);
  sscanf(argv[2],"%hhx/%hhx/%hhx",&wrbuff.day ,&wrbuff.month ,&wrbuff.year);

  printf("%02x:%02x:%02x, %02x/%02x/%02x\n",
    wrbuff.hour, wrbuff.minute, wrbuff.second,
    wrbuff.day , wrbuff.month , wrbuff.year);

  retval = write(fd, &wrbuff, sizeof(wrbuff));
  if(retval < 0)
  {
    perror("Write Failed : ");
    return FAILURE;
  }
  printf("File Write Success\n");

  return SUCCESS;
}

int closeFile(int argc,char *argv[])
{
  if(0 > close(fd))
  {
    perror("Close Failed : ");
    return FAILURE;
  }
  printf("File Close Success\n");
  return SUCCESS;
}

int ioctlFile(int argc,char *argv[])
{
  int retval, cmd, dir = 0, num = 0, val = 0;

  if(argc < 3)
  {
    printf("Usage <Cmd> <Dir 0->R,1->W> <Number> <ValueToSet(if Dir = W)>\n");
    printf("<Number>\n");
    printf("1  = SET_SECOND\n");
    printf("2  = SET_MINUTE\n");
    printf("3  = SET_HOUR\n");
    printf("4  = SET_DAY\n");
    printf("5  = SET_MONTH\n");
    printf("6  = SET_YEAR\n");
    printf("7  = GET_SECOND\n");
    printf("8  = GET_MINUTE\n");
    printf("9  = GET_HOUR\n");
    printf("10 = GET_DAY\n");
    printf("11 = GET_MONTH\n");
    printf("12 = GET_YEAR\n");
    return FAILURE;
  }
  sscanf(argv[1],"%d",&dir);
  sscanf(argv[2],"%d",&num);
  if(num > 12)
  {
    printf("Invalid Number\n");
    return FAILURE;
  }
  if(dir == 1)
    sscanf(argv[3],"%d",&val);

  switch(num)
  {
    case 1:
      cmd = SET_SECOND;
      break;
    case 2:
      cmd = SET_MINUTE;
      break;
    case 3:
      cmd = SET_HOUR;
      break;
    case 4:
      cmd = SET_DAY;
      break;
    case 5:
      cmd = SET_MONTH;
      break;
    case 6:
      cmd = SET_YEAR;
      break;
    case 7:
      cmd = GET_SECOND;
      break;
    case 8:
      cmd = GET_MINUTE;
      break;
    case 9:
      cmd = GET_HOUR;
      break;
    case 10:
      cmd = GET_DAY;
      break;
    case 11:
      cmd = GET_MONTH;
      break;
    case 12:
      cmd = GET_YEAR;
      break;
  }
  retval = ioctl(fd, cmd, &val);
  if(retval < 0)
  {
    perror("IOCTL Failed : ");
    return FAILURE;
  }
  printf("File IOCTL Success\n");
  if(dir == 0)
    printf("Value = %02x\n",val);
  return 0;
}

int quitApp(int argc,char *argv[])
{
  quitFlag = 1;
  return 0;
}

int dispHlp(int argc,char *argv[])
{
  int ii;
  for(ii = 0; ii < (sizeof(commandTable)/sizeof(cmdFun_t)); ii++)
    printf("%s\t : %s\n",commandTable[ii].cmd,commandTable[ii].hlpStr);
  printf("\n");
  return 0;
}
