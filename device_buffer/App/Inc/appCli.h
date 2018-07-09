
#ifndef INC_APPCLI_H_
#define INC_APPCLI_H_

#define SUCCESS  0
#define FAILURE -1
 
int openFile(int argc,char *argv[]);
int readFile(int argc,char *argv[]);
int writeFile(int argc,char *argv[]);
int closeFile(int argc,char *argv[]);
int ioctlGetSize(int argc,char *argv[]);
int ioctlGetIndexData(int argc,char *argv[]);
int ioctlFillBuffer(int argc,char *argv[]);
int ioctlClearBuffer(int argc,char *argv[]);
int quitApp(int argc,char *argv[]);
int dispHlp(int argc,char *argv[]);

#endif /* INC_APPCLI_H_ */
