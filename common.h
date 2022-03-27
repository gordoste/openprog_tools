#ifndef _COMMON_DECLARATIONS
#define _COMMON_DECLARATIONS
//#define DEBUG

#if !defined _WIN32 && !defined __CYGWIN__
	#include <sys/ioctl.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <asm/types.h>
	#include <fcntl.h>
	#include <linux/hiddev.h>
	#include <linux/hidraw.h>
	#include <linux/input.h>
	#include <sys/timeb.h>
	#include <stdint.h>
	#include <errno.h>
#else
	#include <windows.h>
	#include <setupapi.h>
	#include <hidusage.h>
	#include <hidpi.h>
	#include <math.h>
	#include <sys/timeb.h>
	#include <wchar.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>
#include "strings.h"
#include "instructions.h"

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define COL 16
//Version defined in makefile
#if !defined VERSION
  #define VERSION "unknown"
#endif
#define G (12.0/34*1024/5)		//=72,2823529412
#define LOCK	1
#define FUSE	2
#define FUSE_H  4
#define FUSE_X	8
#define CAL 	16
#define SLOW	256

#if !defined _WIN32 && !defined __CYGWIN__		//Linux
	#define SYSNAME "Linux"
	#define DIMBUF 64
	DWORD GetTickCount();
	extern unsigned char bufferU[128],bufferI[128];
#else	//Windows
	#define SYSNAME "Windows"
	#define DIMBUF 64
	extern unsigned char bufferU0[128],bufferI0[128];
	extern unsigned char *bufferU,*bufferI;
	extern DWORD NumberOfBytesRead,BytesWritten;
	extern ULONG Result;
	extern HANDLE WriteHandle,ReadHandle;
	extern OVERLAPPED HIDOverlapped;
	extern HANDLE hEventObject;
#endif

extern char str[4096];
extern int saveLog;
extern char** strings;
extern int fd;
extern int saveLog,programID,MinDly,load_osccal,load_BKosccal;
extern int use_osccal,use_BKosccal;
extern int load_calibword,max_err;
extern int AVRlock,AVRfuse,AVRfuse_h,AVRfuse_x;
extern int ICDenable,ICDaddr;
extern int FWVersion,HwID;
extern FILE* logfile;
extern char LogFileName[512];
extern char loadfile[512],savefile[512];
extern WORD *memCODE_W;
extern int size,sizeW,sizeEE,sizeCONFIG,sizeUSERID;
extern unsigned char *memCODE,*memEE,memID[64],memCONFIG[48],memUSERID[8];
extern double hvreg;
extern int RWstop;

const char appName[6];

#define PrintMessage1(s,p) {sprintf(str,s,p); PrintMessage(str);}
#define PrintMessage2(s,p1,p2) {sprintf(str,s,p1,p2); PrintMessage(str);}
#define PrintMessage3(s,p1,p2,p3) {sprintf(str,s,p1,p2,p3); PrintMessage(str);}
#define PrintMessage4(s,p1,p2,p3,p4) {sprintf(str,s,p1,p2,p3,p4); PrintMessage(str);}

void PrintStatus(s,p1,p2);
void PrintStatusSetup();
void PrintStatusEnd();
void PrintStatusClear();

int StartHVReg(double V);
void msDelay(double delay);
void DisplayEE();
void PrintMessageI2C(const char *msg);
int CheckV33Regulator(void);
void OpenLogFile(void);
void WriteLogIO();
void CloseLogFile();
unsigned int htoi(const char *hex, int length);
void PacketIO(double delay);
#endif
