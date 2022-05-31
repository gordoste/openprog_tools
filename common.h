#ifndef _COMMON_H
#define _COMMON_H
//#define DEBUG

#if !defined _WIN32 && !defined __CYGWIN__
	#include <sys/ioctl.h>
	#include <sys/select.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <asm/types.h>
	#include <fcntl.h>
	#include <linux/input.h>
	#include <sys/timeb.h>
	#include <stdint.h>
	#include <errno.h>
#else
	#include <windows.h>
	#include <setupapi.h>
	#include <math.h>
	#include <sys/timeb.h>
	#include <wchar.h>
	#include "conio.h"
#endif

#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>

#include "instructions.h"
#include "strings.h"
#include "usb.h"

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;

#define OPROG_OLD_VID	0x04D8
#define OPROG_OLD_PID	0x0100
#define OPROG_NEW_VID	0x1209
#define OPROG_NEW_PID	0x5432

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
	int kbhit();
	extern unsigned char bufferU[128],bufferI[128];
#else	//Windows
	#define SYSNAME "Windows"
	#define DIMBUF 64
#endif

extern unsigned char *bufferU,*bufferI;

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
extern int DeviceDetected;
extern FILE* logfile;
extern char LogFileName[512];
extern char loadfile[512],savefile[512];
extern WORD *memCODE_W;
extern int size,sizeW,sizeEE,sizeCONFIG,sizeUSERID;
extern unsigned char *memCODE,*memEE,memID[64],memCONFIG[48],memUSERID[8];
extern double hvreg;
extern int RWstop;

extern char appName[6];

#define PrintMessage1(s,p) {sprintf(str,s,p); PrintMessage(str);}
#define PrintMessage2(s,p1,p2) {sprintf(str,s,p1,p2); PrintMessage(str);}
#define PrintMessage3(s,p1,p2,p3) {sprintf(str,s,p1,p2,p3); PrintMessage(str);}
#define PrintMessage4(s,p1,p2,p3,p4) {sprintf(str,s,p1,p2,p3,p4); PrintMessage(str);}

// Macros for debug statements
#ifdef VERBOSE
#define Debug0(s) {fprintf(stderr,s); fflush(stderr);}
#define Debug1(s,p) {fprintf(stderr,s,p); fflush(stderr);}
#define Debug2(s,p1,p2) {fprintf(stderr,s,p1,p2); fflush(stderr);}
#define Debug3(s,p1,p2,p3) {fprintf(stderr,s,p1,p2,p3); fflush(stderr);}
#define Debug4(s,p1,p2,p3,p4) {fprintf(stderr,s,p1,p2,p3,p4); fflush(stderr);}
#else
#define Debug0(s) {}
#define Debug1(s,p) {}
#define Debug2(s,p1,p2) {}
#define Debug3(s,p1,p2,p3) {}
#define Debug4(s,p1,p2,p3,p4) {}
#endif

// These functions have a single implementation in common.c
// ********************************************************
int SearchDevice(int *_vid, int *_pid, bool _info);
void msDelay(double delay);
char *strcasestr(const char *haystack, const char *needle);
int CheckV33Regulator();
int StartHVReg(double V);
void ProgID();
int CheckS1();

// These functions have different implementations in opgui.c/op.c 
// **************************************************************
void PrintStatus(char *s,  uint16_t p1, uint16_t p2);
void PrintStatusSetup();
void PrintStatusEnd();
void PrintStatusClear();
void PrintMessage(const char *msg);
void PrintMessageI2C(const char *msg);
// Convert src to correct encoding for output and concatenate to dst
void StrcatConvert(char *dst, const char *src);
void DisplayEE();
void OpenLogFile(void);
void WriteLogIO();
void CloseLogFile();
unsigned int htoi(const char *hex, int length);

#endif // _COMMON_H
