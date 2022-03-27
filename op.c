/*
 * op.c - control program for the open programmer
 * Copyright (C) 2009-2020 Alberto Maccioni
 * for detailed info see:
 * http://openprog.altervista.org/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 * or see <http://www.gnu.org/licenses/>
 */


#include "op.h"
#include "common.h"

#include "I2CSPI.h"
#include "deviceRW.h"
#include "fileIO.h"
#include "progAVR.h"

#if !defined _WIN32 && !defined __CYGWIN__
DWORD GetTickCount();
#include <sys/select.h>
int kbhit()
{
	struct timeval tv;
	fd_set read_fd;
	tv.tv_sec=0;
	tv.tv_usec=0;
	FD_ZERO(&read_fd);
	FD_SET(0,&read_fd);
	if(select(1, &read_fd, NULL, NULL, &tv) == -1) return 0;
	if(FD_ISSET(0,&read_fd)) return 1;
	return 0;
}
#define getch getchar
#else
#include "conio.h"
#endif
#define MinDly 0

void msDelay(double delay);
void TestHw();
int StartHVReg(double V);
void ProgID();
void DisplayEE();
int FindDevice();
int CheckS1();

char** strings;
int saveLog=0,programID=0,load_osccal=0,load_BKosccal=0;
int use_osccal=1,use_BKosccal=0;
int load_calibword=0,max_err=200;
int AVRlock=0x100,AVRfuse=0x100,AVRfuse_h=0x100,AVRfuse_x=0x100;
int ICDenable=0,ICDaddr=0x1FF0;
int FWVersion=0,HwID=0;

FILE* logfile=0;
char LogFileName[512]="";
char loadfile[512]="",savefile[512]="";
char loadfileEE[512]="",savefileEE[512]="";
int info=0;
int vid=0x1209,pid=0x5432;
int new_vid=0x1209,new_pid=0x5432;
int old_vid=0x04D8,old_pid=0x0100;

WORD *memCODE_W=0;
int size=0,sizeW=0,sizeEE=0,sizeCONFIG=0,sizeUSERID=0;
unsigned char *memCODE=0,*memEE=0,memID[64],memCONFIG[48],memUSERID[8];
double hvreg=0;
int DeviceDetected=0;
int skipV33check=0;
int progress=0;
int RWstop=0;
int forceConfig=0;
char dev[64]="";
int devType=0x10000;
char str[4096];

#if !defined _WIN32 && !defined __CYGWIN__	//Linux
	int fd = -1;
	struct hiddev_report_info rep_info_i,rep_info_u;
	struct hiddev_usage_ref_multi ref_multi_i,ref_multi_u;
	char path[512]="";
	unsigned char bufferU[128],bufferI[128];
#else	//Windows
	unsigned char bufferU0[128],bufferI0[128];
	unsigned char *bufferU,*bufferI;
	DWORD NumberOfBytesRead,BytesWritten;
	ULONG Result;
	HANDLE WriteHandle,ReadHandle;
	OVERLAPPED HIDOverlapped;
	HANDLE hEventObject;
#endif



int main (int argc, char **argv) {
	strcpy(appName, _APPNAME);
	int ee=0,r=0,ver=0,c=0,support=0,i2c=0,spi_mode=0,i,j,testhw=0,s1=0;
	int spi_speed=0,command=0,langfile=0,help=0;
	char lang[32]="";
	unsigned char tmpbuf[128];
	opterr = 0;
	int option_index = 0;
	int fuse3k=0;
	char* langid=0;
	int cw1,cw2,cw3,cw4,cw5,cw6,cw7;
	cw1=cw2=cw3=cw4=cw5=cw6=cw7=0x10000;
#if defined _WIN32 || defined __CYGWIN__	//Windows
	bufferI=bufferI0+1;
	bufferU=bufferU0+1;
	bufferI0[0]=0;
	bufferU0[0]=0;
#endif

	struct option long_options[] =
	{
		{"BKosccal",      no_argument,  &load_BKosccal, 1},
		{"calib",         no_argument, &load_calibword, 1},
		{"command",       no_argument,        &command, 1},
		{"cw1",           required_argument,       0, '1'},
		{"cw2",           required_argument,       0, '2'},
		{"cw3",           required_argument,       0, '3'},
		{"cw4",           required_argument,       0, '4'},
		{"cw5",           required_argument,       0, '5'},
		{"cw6",           required_argument,       0, '6'},
		{"cw7",           required_argument,       0, '7'},
		{"d",             required_argument,       0, 'd'},
		{"device",        required_argument,       0, 'd'},
		{"ee",            no_argument,             &ee, 1},
		{"err",           required_argument,       0, 'e'},
		{"fuse",          required_argument,       0, 'f'},
		{"fuseh",         required_argument,       0, 'F'},
		{"fusex",         required_argument,       0, 'X'},
		{"fuse3k",        required_argument,       0, 'k'},
		{"h",             no_argument,            &help,1},
		{"help",          no_argument,            &help,1},
		{"HWtest",        no_argument,         &testhw, 1},
		{"info",          no_argument,           &info, 1},
		{"i",             no_argument,           &info, 1},
		{"i2c_r",         no_argument,            &i2c, 1},
		{"i2c_r2",        no_argument,            &i2c, 2},
		{"i2c_w",         no_argument,            &i2c, 3},
		{"i2c_w2",        no_argument,            &i2c, 4},
		{"i2cspeed",      required_argument,       0, 'D'},
		{"id",            no_argument,      &programID, 1},
		{"icd",           required_argument,       0, 'I'},
		{"l",             optional_argument,       0, 'l'}, //-l=val
		{"log",           optional_argument,       0, 'l'},
		{"lang",          required_argument,       0, 'n'},
		{"langfile",      no_argument,       &langfile, 1},
		{"lock",          required_argument,       0, 'L'},
		{"mode",          required_argument,       0, 'm'},
		{"nolvcheck",     no_argument,   &skipV33check, 1},
		{"osccal",        no_argument,    &load_osccal, 1},
#if !defined _WIN32 && !defined __CYGWIN__
		{"p",             required_argument,       0, 'p'},
		{"path",          required_argument,       0, 'p'},
#endif
		{"pid",           required_argument,       0, 'P'},
		{"rep" ,          required_argument,       0, 'R'},
		{"reserved",      no_argument,              &r, 1},
		{"r",             no_argument,              &r, 1},
		{"s1",            no_argument,             &s1, 1},
		{"S1",            no_argument,             &s1, 1},
		{"s",             required_argument,       0, 's'},
		{"save",          required_argument,       0, 's'},
		{"saveEE",        required_argument,       0, 'S'},
		{"se",            required_argument,       0, 'S'},
		{"spi_r",         no_argument,            &i2c, 5},
		{"spi_w",         no_argument,            &i2c, 6},
		{"spispeed",      required_argument,       0, 'D'},
		{"support",       no_argument,        &support, 1},
		{"use_BKosccal",  no_argument,   &use_BKosccal, 1},
		{"version",       no_argument,            &ver, 1},
		{"v",             no_argument,            &ver, 1},
		{"vid",           required_argument,       0, 'V'},
		{"w",             required_argument,       0, 'w'},
		{"write",         required_argument,       0, 'w'},
		{"we",            required_argument,       0, 'W'},
		{"writeEE",       required_argument,       0, 'W'},
		{0, 0, 0, 0}
	};
	while ((c = getopt_long_only (argc, argv, "",long_options,&option_index)) != -1)
	/*{	printf("optarg=%X\n",optarg);
		if(optarg) printf("%s\n",optarg);
		printf("c=%X %c\noption_index=%d name=%s\n",c,c,option_index,long_options[option_index].name);	}
		exit(0);*/
		switch (c)
		{
			case '1':	//force config word 1
				i=sscanf(optarg, "%x", &cw1);
				if(i!=1||cw1<0||cw1>0x3FFF) cw1=0x10000;
				break;
			case '2':	//force config word 2
				i=sscanf(optarg, "%x", &cw2);
				if(i!=1||cw2<0||cw2>0x3FFF) cw2=0x10000;
				break;
			case '3':	//force config word 3
				i=sscanf(optarg, "%x", &cw3);
				if(i!=1||cw3<0||cw3>0x3FFF) cw3=0x10000;
				break;
			case '4':	//force config word 4
				i=sscanf(optarg, "%x", &cw4);
				if(i!=1||cw4<0||cw4>0x3FFF) cw4=0x10000;
				break;
			case '5':	//force config word 5
				i=sscanf(optarg, "%x", &cw5);
				if(i!=1||cw5<0||cw5>0x3FFF) cw5=0x10000;
				break;
			case '6':	//force config word 6
				i=sscanf(optarg, "%x", &cw6);
				if(i!=1||cw6<0||cw6>0x3FFF) cw6=0x10000;
				break;
			case '7':	//force config word 7
				i=sscanf(optarg, "%x", &cw7);
				if(i!=1||cw7<0||cw7>0x3FFF) cw7=0x10000;
				break;
			case 'd':	//device
				strncpy(dev,optarg,sizeof(dev)-1);
				break;
			case 'e':	//max write errors
				max_err = atoi(optarg);
				break;
			case 'D':	//spi speed
				spi_speed = atoi(optarg);
				if(spi_speed<0)spi_speed=0;
				if(spi_speed>3)spi_speed=3;
				break;
			case 'f':	//Atmel FUSE low
				i=sscanf(optarg, "%x", &AVRfuse);
				if(i!=1||AVRfuse<0||AVRfuse>0xFF) AVRfuse=0x100;
				break;
			case 'F':	//Atmel FUSE high
				i=sscanf(optarg, "%x", &AVRfuse_h);
				if(i!=1||AVRfuse_h<0||AVRfuse_h>0xFF) AVRfuse_h=0x100;
				break;
			case 'I':	//ICD routine address
				i=sscanf(optarg, "%x", &ICDaddr);
				if(i!=1||ICDaddr<0||ICDaddr>0xFFFF) ICDaddr=0x1F00;
				ICDenable=1;
				break;
			case 'k':	//Atmel FUSE low @ 3kHz
				i=sscanf(optarg, "%x", &AVRfuse);
				if(i!=1||AVRfuse<0||AVRfuse>0xFF) AVRfuse=0x100;
				else fuse3k=1;
				break;
			case 'l':	//save Log
				saveLog=1;
				if(optarg) strncpy(LogFileName,optarg,sizeof(LogFileName));
				break;
			case 'L':	//Atmel LOCK
				i=sscanf(optarg, "%x", &AVRlock);
				if(i!=1||AVRlock<0||AVRlock>0xFF) AVRlock=0x100;
				break;
			case 'm':	//SPI mode
				spi_mode = atoi(optarg);
				if(spi_mode<0) spi_mode=0;
				if(spi_mode>3) spi_mode=3;
				break;
			case 'n':	//language
				strncpy(lang,optarg,sizeof(lang)-1);
				break;
#if !defined _WIN32 && !defined __CYGWIN__
			case 'p':	//hiddev path
				strncpy(path,optarg,sizeof(path)-1);
				break;
#endif
			case 'P':	//pid
				sscanf(optarg, "%x", &pid);
				break;
			case 'R':	//USB HID report size
				//DIMBUF = atoi(optarg);
				break;
			case 's':	//save
				strncpy(savefile,optarg,sizeof(savefile)-1);
				break;
			case 'S':	//save EE
				strncpy(savefileEE,optarg,sizeof(savefileEE)-1);
				break;
			case 'V':	//vid
				sscanf(optarg, "%x", &vid);
				break;
			case 'X':	//Atmel extended FUSE
				i=sscanf(optarg, "%x", &AVRfuse_x);
				if(i!=1||AVRfuse_x<0||AVRfuse_x>0xFF) AVRfuse_x=0x100;
				break;
			case 'w':	//write file
				strncpy(loadfile,optarg,sizeof(loadfile)-1);
				break;
			case 'W':	//write EE file
				strncpy(loadfileEE,optarg,sizeof(loadfileEE)-1);
				break;
			case '?':
				fprintf (stderr,strings[L_OPTERR]);		//errore opzioni
				return 1;
			default:

				break;
		}

	for(j=0,i = optind; i < argc&&i<128; i++,j++) sscanf(argv[i], "%x", &tmpbuf[j]);
	for(;j<128;j++) tmpbuf[j]=0;

	strinit();
	i=0;
	if(lang[0]){	//explicit language selection
		if(lang[0]=='i'&&langid[1]=='t'){  //built-in
			strings=strings_it;
			i=1;
		}
		else if(lang[0]=='e'&&lang[1]=='n'){  //built-in
			strings=strings_en;
			i=1;
		}
		else i=strfind(lang,"languages.rc"); //file look-up
	}
	if(i==0){
		#if defined _WIN32
		langid=malloc(19);
		int n=GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_SISO639LANGNAME,langid,9);
		langid[n-1] = '-';
		GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_SISO3166CTRYNAME,langid+n, 9);
		//printf("%d >%s<\n",n,langid);
		#else
		langid=getenv("LANG");
		#endif
		if(langid){
			if(langid[0]=='i'&&langid[1]=='t') strings=strings_it;
			else if(langid[0]=='e'&&langid[1]=='n') strings=strings_en;
			else if(strfind(langid,"languages.rc")); //first try full code
			else {	//then only first language code
				char* p=strchr(langid,'-');
				if(p) *p=0;
				if(!strfind(langid,"languages.rc")) strings=strings_en;
			}
		}
		else strings=strings_en;
	}

	strncpy(LogFileName,strings[S_LogFile],sizeof(LogFileName));
	if(argc==1){
		printf(strings[L_HELP]);
		exit(1);
	}

	if(help){
		printf(strings[L_HELP]);
		return 1;
	}

	if(langfile) GenerateLangFile(langid,"languages.rc");

	if (ver){
		printf("OP v%s\nCopyright (C) Alberto Maccioni 2009-2019\
\n	For detailed info see http://openprog.altervista.org/\
\nThis program is free software; you can redistribute it and/or modify it under \
the terms of the GNU General Public License as published by the Free Software \
Foundation; either version 2 of the License, or (at your option) any later version.\
			\n",VERSION);
		return 0;
	}
	if (support){
		char list[20000]; //make sure list is long enough!!
		AddDevices(list);
		printf("%s\n",list);
		return 0;
	}
	DeviceDetected=FindDevice(vid,pid);	//connect to USB programmer
	if(!DeviceDetected){
		DeviceDetected=FindDevice(new_vid,new_pid);	//try default
		if(DeviceDetected){
			vid=new_vid;
			pid=new_pid;
		}
	}
	if(!DeviceDetected) DeviceDetected=FindDevice(old_vid,old_pid); //try old one
	if(!DeviceDetected) exit(1);
	ProgID();		//get firmware version and reset

#if !defined _WIN32 && !defined __CYGWIN__
	if(info){
		struct hiddev_devinfo device_info;
		ioctl(fd, HIDIOCGDEVINFO, &device_info);
		printf(strings[L_INFO1],device_info.vendor, device_info.product, device_info.version);
		printf(strings[L_INFO2],device_info.busnum, device_info.devnum, device_info.ifnum);
		char name[256];
		strcpy(name,strings[L_UNKNOWN]);//"Unknown"
		if(ioctl(fd, HIDIOCGNAME(sizeof(name)), name) < 0) perror("evdev ioctl");
		printf(strings[L_NAME], path, name);//"The device on %s says its name is %s\n"
		return 0;
	}
#endif

	DWORD t0,t;
	t=t0=GetTickCount();
	if(testhw){
		TestHw();
		return 0;
	}

	if(command){
		bufferU[0]=0;
		for(i=1;i<DIMBUF;i++) bufferU[i]=(char) tmpbuf[i-1];
		PacketIO(100);
		printf("> ");
		for(i=1;i<DIMBUF;i++) printf("%02X ",bufferU[i]);
		printf("\n< ");
		for(i=1;i<DIMBUF;i++) printf("%02X ",bufferI[i]);
		printf("\n");
		return 0;
	}

#define CS 8
#define HLD 16
	if(i2c){	//I2C, SPI
		if(i2c==1){							//I2C receive 8 bit mode
			I2CReceive(0,spi_speed,tmpbuf[0],tmpbuf+1);
		}
		else if(i2c==2){					//I2C receive 16 bit mode
			I2CReceive(1,spi_speed,tmpbuf[0],tmpbuf+1);
		}
		else if(i2c==3){					//I2C transmit 8 bit mode
			I2CSend(0,spi_speed,tmpbuf[0],tmpbuf+1);
		}
		else if(i2c==4){					//I2C transmit 16 bit mode
			I2CSend(1,spi_speed,tmpbuf[0],tmpbuf+1);
		}
		else if(i2c==5){					//SPI receive
			I2CReceive(2+spi_mode,spi_speed,tmpbuf[0],tmpbuf+1);
		}
		else if(i2c==6){					//SPI receive
			I2CSend(2+spi_mode,spi_speed,tmpbuf[0],tmpbuf+1);
		}
		return 0 ;
	}

	struct DevInfo info;
	info=GetDevInfo(dev);
	devType=info.type;

	if(fuse3k){ //write fuse low @ 3kHz
		if(AVRfuse<0x100) WriteATfuseSlow(AVRfuse);
		return 0;
	}

	if(loadfile[0]){ 		//write
		if(Load(dev,loadfile)==-1){
			PrintMessage(strings[S_NoCode2]);
			PrintMessage("\n");
			exit(-1);
		}
		if(!strncmp(dev,"AT",2)&&loadfileEE[0]) LoadEE(dev,loadfileEE);
		//force config words
		if(devType==PIC16){
			if((!strncmp(dev,"16F1",4)||!strncmp(dev,"12F1",4))&&sizeW>0x8008){		//16F1xxx
				if(cw1<=0x3FFF){
					memCODE_W[0x8007]=cw1;
					PrintMessage3(strings[S_ForceConfigWx],1,0x8007,cw1); //"forcing config word%d [0x%04X]=0x%04X"
				}
				if(cw2<=0x3FFF){
					memCODE_W[0x8008]=cw2;
					PrintMessage3(strings[S_ForceConfigWx],2,0x8008,cw2); //"forcing config word%d [0x%04X]=0x%04X"
				}
			}
			else{	//16Fxxx
				if(cw1<=0x3FFF&&sizeW>0x2007){
					memCODE_W[0x2007]=cw1;
					PrintMessage3(strings[S_ForceConfigWx],1,0x2007,cw1); //"forcing config word%d [0x%04X]=0x%04X"
				}
				if(cw2<=0x3FFF&&sizeW>0x2008){
					memCODE_W[0x2008]=cw2;
					printf("2\n");
					PrintMessage3(strings[S_ForceConfigWx],2,0x2008,cw2); //"forcing config word%d [0x%04X]=0x%04X"
				}
			}
		}
		else if(devType==PIC12){	//12Fxxx
			if(cw1<=0xFFF&&sizeW>0xFFF){
				memCODE_W[0xFFF]=cw1;
				PrintMessage3(strings[S_ForceConfigWx],1,0xFFF,cw1); //"forcing config word%d [0x%04X]=0x%04X"
			}
		}
		else if(devType==PIC18){	//18Fxxx
			if(cw1<=0xFFFF){
				memCONFIG[0]=cw1&0xFF;
				memCONFIG[1]=(cw1>>8)&0xFF;
			}
			if(cw2<=0xFFFF){
				memCONFIG[2]=cw2&0xFF;
				memCONFIG[3]=(cw2>>8)&0xFF;
			}
			if(cw3<=0xFFFF){
				memCONFIG[4]=cw3&0xFF;
				memCONFIG[5]=(cw3>>8)&0xFF;
			}
			if(cw4<=0xFFFF){
				memCONFIG[6]=cw4&0xFF;
				memCONFIG[7]=(cw4>>8)&0xFF;
			}
			if(cw5<=0xFFFF){
				memCONFIG[8]=cw5&0xFF;
				memCONFIG[9]=(cw5>>8)&0xFF;
			}
			if(cw6<=0xFFFF){
				memCONFIG[10]=cw6&0xFF;
				memCONFIG[11]=(cw6>>8)&0xFF;
			}
			if(cw7<=0xFFFF){
				memCONFIG[12]=cw7&0xFF;
				memCONFIG[13]=(cw7>>8)&0xFF;
			}
			PrintMessage(strings[S_ForceConfigW]); //"forcing config words"
			for(i=0;i<7;i++){
				PrintMessage2(strings[S_ConfigWordH],i+1,memCONFIG[i*2+1]);	//"CONFIG%dH: 0x%02X\t"
				PrintMessage2(strings[S_ConfigWordL],i+1,memCONFIG[i*2]);	//"CONFIG%dL: 0x%02X\r\n"
			}
		}
/*		if(CW1_force!=-1||CW2_force!=-1){
			if((!strncmp(dev,"16F1",4)||!strncmp(dev,"12F1",4))){		//16F1xxx
				if(CW1_force!=-1&&sizeW>0x8007) memCODE_W[0x8007]=CW1_force;
				if(CW2_force!=-1&&sizeW>0x8008) memCODE_W[0x8008]=CW2_force;
			}
			else if((!strncmp(dev,"16F",3)||!strncmp(dev,"12F6",4))&&strncmp(dev,"16F5",4)){	//16Fxxx
				if(CW1_force!=-1&&sizeW>0x2007) memCODE_W[0x2007]=CW1_force;
				if(CW2_force!=-1&&sizeW>0x2008) memCODE_W[0x2008]=CW2_force;
			}
			else if((!strncmp(dev,"12F",3)||!strncmp(dev,"10F",3)||!strncmp(dev,"16F5",4))){	//12Fxxx
				if(CW1_force!=-1&&sizeW>0xFFF) memCODE_W[0xFFF]=CW1_force&0xFFF;
			}
			if(CW1_force!=-1) PrintMessage1(strings[S_ForceConfigWx],CW1_force); //"forcing config word1 (0x%04X)"
			if(CW2_force!=-1) PrintMessage1(strings[S_ForceConfigWx],CW2_force); //"forcing config word2 (0x%04X)"
		}
*/
		//Start with button
		if(s1){
			PrintMessage(strings[S_WaitS1W]);	//"Press S1 to program, any key to exit"
			fflush(stdout);
			for(;!kbhit();msDelay(50)){
				if(CheckS1()){	//wait for S1
					Write(dev,ee);	//choose the right function
					PrintMessage(strings[S_WaitS1W]);	//"Press S1 to program, any key to exit"
					fflush(stdout);
					for(;CheckS1();msDelay(50));	//wait for S1 open
				}
			}
			getch();
		}
		else Write(dev,ee);	//choose the right function
	}
	else{		//read
		if(s1){
			PrintMessage(strings[S_WaitS1R]);	//"Press S1 to read, any key to exit"
			fflush(stdout);
			for(;!kbhit();msDelay(50)){
				if(CheckS1()){	//wait for S1
					Read(dev,ee,r);	//choose the right function
					PrintMessage(strings[S_WaitS1R]);	//"Press S1 to read, any key to exit"
					fflush(stdout);
					for(;CheckS1();msDelay(50));	//wait for S1 open
				}
			}
			getch();
		}
		else Read(dev,ee,r);	//choose the right function

		if(savefile[0]) Save(dev,savefile);
		if(!strncmp(dev,"AT",2)&&savefileEE[0]) SaveEE(dev,savefileEE);
	}

#if !defined _WIN32 && !defined __CYGWIN__	//Linux
	close(fd);
#endif
	return 0 ;
}


///
///Display contents of EEprom memory
void DisplayEE(){
	char s[256],t[256],v[256];
	int valid=0,empty=1;
	int i,j;
	s[0]=0;
	v[0]=0;
	PrintMessage(strings[S_EEMem]);	//"\r\nmemoria EEPROM:\r\n"
	for(i=0;i<sizeEE;i+=COL){
		valid=0;
		for(j=i;j<i+COL&&j<sizeEE;j++){
			sprintf(t,"%02X ",memEE[j]);
			strcat(s,t);
			sprintf(t,"%c",isprint(memEE[j])?memEE[j]:'.');
			strcat(v,t);
			if(memEE[j]<0xff) valid=1;
		}
		if(valid){
			PrintMessage("%04X: %s %s\r\n",i,s,v);
			empty=0;
		}
		s[0]=0;
		v[0]=0;
	}
	if(empty) PrintMessage(strings[S_Empty]);	//empty
}

///
///Start HV regulator
int StartHVReg(double V){
	int j=0,z;
	int vreg=(int)(V*10.0);
	if(saveLog&&logfile) fprintf(logfile,"StartHVReg(%.2f)\n",V);
	DWORD t0,t;
	if(V==-1){
		bufferU[j++]=VREG_DIS;			//disable HV regulator
		bufferU[j++]=FLUSH;
		PacketIO(5);
		msDelay(40);
		return -1;
	}
	t=t0=GetTickCount();
	bufferU[j++]=VREG_EN;			//enable HV regulator
	bufferU[j++]=SET_VPP;
	bufferU[j++]=vreg;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=READ_ADC;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	msDelay(20);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_ADC;z++);
	int v=(bufferI[z+1]<<8)+bufferI[z+2];
//	PrintMessage2("v=%d=%fV\n",v,v/G);
	if(v==0){
		PrintMessage(strings[S_lowUsbV]);	//"Tensione USB troppo bassa (VUSB<4.5V)\r\n"
		return 0;
	}
		j=0;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=READ_ADC;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
	for(;(v<(vreg/10.0-1)*G||v>(vreg/10.0+1)*G)&&t<t0+1500;t=GetTickCount()){
		PacketIO(5);
		msDelay(20);
		for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_ADC;z++);
		v=(bufferI[z+1]<<8)+bufferI[z+2];
		if(HwID==3) v>>=2;		//if 12 bit ADC
//		PrintMessage2("v=%d=%fV\n",v,v/G);
	}
	if(v>(vreg/10.0+1)*G){
		PrintMessage(strings[S_HiVPP]);	//"Attenzione: tensione regolatore troppo alta\r\n\r\n"
		return 0;
	}
	else if(v<(vreg/10.0-1)*G){
		PrintMessage(strings[S_LowVPP]);	//"Attenzione: tensione regolatore troppo bassa\r\n\r\n"
		return 0;
	}
	else if(v==0){
		PrintMessage(strings[S_lowUsbV]);	//"Tensione USB troppo bassa (VUSB<4.5V)\r\n"
		return 0;
	}
	else{
		PrintMessage2(strings[S_reg],t-t0,v/G);	//"Regolatore avviato e funzionante dopo T=%d ms VPP=%.1f\r\n\r\n"
		if(saveLog&&logfile) fprintf(logfile,strings[S_reg],t-t0,v/G);
		return vreg;
	}
}

///
///Read programmer ID
void ProgID()
{
	if(DeviceDetected!=1) return;
	int j=0;
	bufferU[j++]=PROG_RST;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	for(j=0;j<DIMBUF-7&&bufferI[j]!=PROG_RST;j++);
	PrintMessage3(strings[S_progver],bufferI[j+1],bufferI[j+2],bufferI[j+3]); //"FW versione %d.%d.%d\r\n"
	FWVersion=(bufferI[j+1]<<16)+(bufferI[j+2]<<8)+bufferI[j+3];
	PrintMessage3(strings[S_progid],bufferI[j+4],bufferI[j+5],bufferI[j+6]);	//"ID Hw: %d.%d.%d"
	HwID=bufferI[j+6];
	if(HwID==1) PrintMessage(" (18F2550)\r\n\r\n");
	else if(HwID==2) PrintMessage(" (18F2450)\r\n\r\n");
	else if(HwID==3) PrintMessage(" (18F2458/2553)\r\n\r\n");
	else PrintMessage(" (?)\r\n\r\n");
}

///
///Check if a 3.3V regulator is present
int CheckV33Regulator()
{
	int i,j=0;
	if(skipV33check) return 1;
	bufferU[j++]=WRITE_RAM;
	bufferU[j++]=0x0F;
	bufferU[j++]=0x93;
	bufferU[j++]=0xFE;	//B0 = output
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0x01;	//B0=1
	bufferU[j++]=0;
	bufferU[j++]=READ_RAM;
	bufferU[j++]=0x0F;
	bufferU[j++]=0x81;	//Check if B1=1
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0x00;	//B0=0
	bufferU[j++]=0;
	bufferU[j++]=READ_RAM;
	bufferU[j++]=0x0F;
	bufferU[j++]=0x81;	//Check if B1=0
	bufferU[j++]=WRITE_RAM;
	bufferU[j++]=0x0F;
	bufferU[j++]=0x93;
	bufferU[j++]=0xFF;	//BX = input
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	for(j=0;j<DIMBUF-3&&bufferI[j]!=READ_RAM;j++);
	i=bufferI[j+3]&0x2;		//B1 should be high
	for(j+=3;j<DIMBUF-3&&bufferI[j]!=READ_RAM;j++);
	return (i+(bufferI[j+3]&0x2))==2?1:0;
}

///
///Check if S1 is pressed
int CheckS1()
{
	int i,j=0;
	bufferU[j++]=READ_RAM;
	bufferU[j++]=0x0F;
	bufferU[j++]=0x84;	//READ PORTE
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	for(j=0;j<DIMBUF-3&&bufferI[j]!=READ_RAM;j++);
	i=bufferI[j+3]&0x8;		//i=E3
	return i?0:1;			//S1 open -> E3=1
}

///
///Execute hardware test
void TestHw()
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	char str[256];
	StartHVReg(13);
	int j=0;
	PrintMessage(strings[I_TestHW]);		//"Test hardware ..."
	fflush(stdout);
	getchar();
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	strcpy(str,strings[I_TestMSG]);
	strcat(str,"\n VDD=5V\n VPP=13V\n PGD(RB5)=0V\n PGC(RB6)=0V\n PGM(RB7)=0V");
	PrintMessage(str);
	fflush(stdout);
	getchar();
	j=0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x15;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;			//VDD
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	strcpy(str,strings[I_TestMSG]);
	strcat(str,"\n VDD=5V\n VPP=0V\n PGD(RB5)=5V\n PGC(RB6)=5V\n PGM(RB7)=5V");
	PrintMessage(str);
	fflush(stdout);
	getchar();
	j=0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x4;			//VPP
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	strcpy(str,strings[I_TestMSG]);
	strcat(str,"\n VDD=0V\n VPP=13V\n PGD(RB5)=5V\n PGC(RB6)=0V\n PGM(RB7)=0V");
	PrintMessage(str);
	fflush(stdout);
	getchar();
	j=0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x4;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	strcpy(str,strings[I_TestMSG]);
	strcat(str,"\n VDD=0V\n VPP=0V\n PGD(RB5)=0V\n PGC(RB6)=5V\n PGM(RB7)=0V");
	PrintMessage(str);
	fflush(stdout);
	getchar();
	j=0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	if(FWVersion>=0x900){	//IO test
		int j=0,i,x,r;
		strcpy(str,"0000000000000");
		PrintMessage("IO test\nRC|RA|--RB--|\n");
		for(i=0;i<13;i++){
			x=1<<i;
			j=0;
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=0x0;
			bufferU[j++]=SET_PORT_DIR;
			bufferU[j++]=0x0;	//TRISB
			bufferU[j++]=0x0;	//TRISA-C  (RC7:RC6:RA5:RA4:RA3:X:X:X)
			bufferU[j++]=EXT_PORT;
			bufferU[j++]=x&0xFF;	//PORTB
			bufferU[j++]=(x>>5)&0xFF;	//PORTA-C
			bufferU[j++]=READ_B;
			bufferU[j++]=READ_AC;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(j=0;j<DIMBUF-1&&bufferI[j]!=READ_B;j++);
			r=bufferI[j+1];
			for(j+=2;j<DIMBUF-1&&bufferI[j]!=READ_AC;j++);
			r+=(bufferI[j+1]&0xF8)<<5;
			for(j=0;j<13;j++) str[12-j]=x&(1<<j)?'1':'0';
			PrintMessage(str);
			PrintMessage1(" (%s)\n",r==x?"OK":strings[S_ErrSing]);
		}
	}
}

///
///Wait for X milliseconds
void msDelay(double delay)
{
#if !defined _WIN32 && !defined __CYGWIN__
	long x=(int)(delay*1000.0);
	usleep(x>MinDly?x:MinDly);
#else
//	Sleep((long)ceil(delay)>MinDly?(long)ceil(delay):MinDly);
	__int64 stop,freq,timeout;
	QueryPerformanceCounter((LARGE_INTEGER *)&stop);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
	timeout=stop+delay*freq/1000.0;
	while(stop<timeout) QueryPerformanceCounter((LARGE_INTEGER *)&stop);
#endif
}

#if !defined _WIN32 && !defined __CYGWIN__	//Linux
///
/// Get system time
DWORD GetTickCount(){
	struct timeb now;
	ftime(&now);
	return now.time*1000+now.millitm;
}
#endif

///
///Write data packet, wait for X milliseconds, read response
void PacketIO(double delay){
	#define TIMEOUT 50
	if(saveLog&&logfile) fprintf(logfile,"PacketIO(%.2f)\n",delay);
	int delay0=delay;
#if !defined _WIN32 && !defined __CYGWIN__	//Linux
	struct timespec ts;
	uint64_t start,stop;
	fd_set set;
	struct timeval timeout;
	int rv,i;
	FD_ZERO(&set); /* clear the set */
	FD_SET(fd, &set); /* add our file descriptor to the set */
	timeout.tv_sec = 0;
	timeout.tv_usec = TIMEOUT*1000;
	clock_gettime( CLOCK_REALTIME, &ts );
	start=ts.tv_nsec/1000;
	delay-=TIMEOUT-10;	//shorter delays are covered by 50ms timeout
	if(delay<MinDly) delay=MinDly;
	#ifndef hiddevIO	//use raw USB device
	//wait before writing
/*	rv = select(fd + 1, NULL, &set, NULL, &timeout); //wait for write event
	if(rv == -1){
		PrintMessage(strings[S_ErrSing]);	//error
		if(saveLog&&logfile) fprintf(logfile,strings[S_ErrSing]);
		return;
	}
	else if(rv == 0){
		PrintMessage(strings[S_comTimeout]);	//"comm timeout\r\n"
		if(saveLog&&logfile) fprintf(logfile,strings[S_comTimeout]);
		return;
	}*/
	//write
	int res = write(fd,bufferU,DIMBUF);
	if (res < 0) {
		printf("Error: %d\n", errno);
		perror("write");
	}
	usleep((int)(delay*1000.0));
	//wait before reading
	rv = select(fd + 1, &set, NULL, NULL, &timeout); //wait for event
	if(rv == -1){
		PrintMessage(strings[S_ErrSing]);	/*error*/
		if(saveLog&&logfile) fprintf(logfile,strings[S_ErrSing]);
		return;
	}
	else if(rv == 0){
		PrintMessage(strings[S_comTimeout]);	/*"comm timeout\r\n"*/
		if(saveLog&&logfile) fprintf(logfile,strings[S_comTimeout]);
		return;
	}
	//read
	res = read(fd, bufferI, DIMBUF);
	if (res < 0) {
		perror("read");
	}
	#else		//use hiddev device (old method)
	struct hiddev_event ev[80];
	int n=DIMBUF;
	for(i=0;i<DIMBUF;i++) ref_multi_u.values[i]=bufferU[i];
	//write
	ioctl(fd, HIDIOCSUSAGES, &ref_multi_u);
	ioctl(fd,HIDIOCSREPORT, &rep_info_u);
	usleep((int)(delay*1000.0));
	//read
	rv = select(fd + 1, &set, NULL, NULL, &timeout); //wait for event
	if(rv == -1){
		PrintMessage(strings[S_ErrSing]);	/*error*/
		if(saveLog&&logfile) fprintf(logfile,strings[S_ErrSing]);
	}
	else if(rv == 0){
		PrintMessage(strings[S_comTimeout]);	/*"comm timeout\r\n"*/
		if(saveLog&&logfile) fprintf(logfile,strings[S_comTimeout]);
	}
	else{
	//		ioctl(fd, HIDIOCGUSAGES, &ref_multi_i);
	//		ioctl(fd,HIDIOCGREPORT, &rep_info_i);
	#undef read()
		rv=read(fd, ev,sizeof(struct hiddev_event) *n);
		for(i=0;(ev[0].value!=bufferU[0])&&i<40;i++){		//read too early; try again after 5ms
			msDelay(5);
			rv=read(fd, ev,sizeof(struct hiddev_event) *n);
			if(saveLog&&logfile) fprintf(logfile,"Packet not ready, wait extra time\n");
		}
		if(i==40) fprintf(logfile,"Cannot read correct packet!!\n");
		for(i=0;i<n;i++) bufferI[i]=ev[i].value&0xFF;
	}
	#endif
	clock_gettime( CLOCK_REALTIME, &ts );
	stop  = ts.tv_nsec / 1000;
	if(saveLog&&logfile){
		WriteLogIO();
		fprintf(logfile,"T=%.2f ms (%+.2f ms)\n",(stop-start)/1000.0,(stop-start)/1000.0-delay0);
		if(bufferU[0]!=bufferI[0]) fprintf(logfile,"Cannot read correct packet!!\n");
	}
#else	//Windows
	__int64 start,stop,freq,timeout;
	QueryPerformanceCounter((LARGE_INTEGER *)&start);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
	delay-=TIMEOUT-10;	//shorter delays are covered by 50ms timeout
	if(delay<MinDly) delay=MinDly;
	//write
	Result = WriteFile(WriteHandle,bufferU0,DIMBUF+1,&BytesWritten,NULL);
	QueryPerformanceCounter((LARGE_INTEGER *)&stop);
	timeout=stop+delay*freq/1000.0;
	while(stop<timeout) QueryPerformanceCounter((LARGE_INTEGER *)&stop);
	//read
	Result = ReadFile(ReadHandle,bufferI0,DIMBUF+1,&NumberOfBytesRead,(LPOVERLAPPED) &HIDOverlapped);
	Result = WaitForSingleObject(hEventObject,TIMEOUT);
	if(saveLog&&logfile) WriteLogIO();
	ResetEvent(hEventObject);
	if(Result!=WAIT_OBJECT_0){
		PrintMessage(strings[S_comTimeout]);	/*"comm timeout\r\n"*/
		if(saveLog&&logfile) fprintf(logfile,strings[S_comTimeout]);
	}
	QueryPerformanceCounter((LARGE_INTEGER *)&stop);
	if(saveLog&&logfile) fprintf(logfile,"T=%.2f ms (%+.2f ms)\n",(stop-start)*1000.0/freq,(stop-start)*1000.0/freq-delay0);
#endif
}

///
///Find the USB peripheral with proper vid&pid code
/// return 0 if not found
int FindDevice(int vid,int pid){
	int MyDeviceDetected = FALSE;
#if !defined _WIN32 && !defined __CYGWIN__	//Linux
	#ifndef hiddevIO	//use raw USB device
	struct hidraw_devinfo device_info;
	int i=-1;
	if(path[0]==0){	//search all devices
		if((fd = open("/dev/openprogrammer", O_RDWR|O_NONBLOCK))>0){ //try with this first
			sprintf(path,"/dev/openprogrammer");
			ioctl(fd, HIDIOCGRAWINFO, &device_info);
			if(device_info.vendor==vid&&device_info.product==pid) i=0;
			else{
				close(fd);
				i=-1;
			}
		}
		if(i){
			for(i=0;i<16;i++){
				sprintf(path,"/dev/hidraw%d",i);
				if((fd = open(path, O_RDWR|O_NONBLOCK))>0){
					ioctl(fd, HIDIOCGRAWINFO, &device_info);
					if(device_info.vendor==vid&&device_info.product==pid) break;
					else close(fd);
				}
			}
		}
		if(i==16){
			PrintMessage(strings[S_noprog]);
			path[0]=0;
			return 0;
		}
	}
	else{	//user supplied path
		if((fd = open(path, O_RDWR|O_NONBLOCK)) < 0) {
			PrintMessage1(strings[S_DevPermission],path); //"cannot open %s, make sure you have read permission on it",path);
			return 0;
		}
		ioctl(fd, HIDIOCGRAWINFO, &device_info);
		if(device_info.vendor!=vid||device_info.product!=pid){
			PrintMessage(strings[S_noprog]);
			return 0;
		}
	}
	printf(strings[S_progDev],path);
	return 1;
	#else		//use hiddev device (old method)
	struct hiddev_devinfo device_info;
	int i=-1;
	if(path[0]==0){	//search all devices
		if((fd = open("/dev/openprogrammer", O_RDONLY ))>0){ //try with this first
			ioctl(fd, HIDIOCGDEVINFO, &device_info);
			if(device_info.vendor==vid&&device_info.product==pid) i=0;
			else{
				close(fd);
				i=-1;
			}
		}
		if(i){
		for(i=0;i<16;i++){
			sprintf(path,"/dev/usb/hiddev%d",i);
			if((fd = open(path, O_RDONLY ))>0){
				ioctl(fd, HIDIOCGDEVINFO, &device_info);
				if(device_info.vendor==vid&&device_info.product==pid) break;
				else close(fd);
				}
			}
		}
		if(i==16){
			PrintMessage(strings[S_noprog]);
			path[0]=0;
			return 0;
		}
	}
	else{	//user supplied path
		if ((fd = open(path, O_RDONLY )) < 0) {
			PrintMessage1(strings[S_DevPermission],path); //"cannot open %s, make sure you have read permission on it",path);
			return 0;
		}
		ioctl(fd, HIDIOCGDEVINFO, &device_info);
		if(device_info.vendor!=vid||device_info.product!=pid){
			PrintMessage(strings[S_noprog]);
			return 0;
		}
	}
	printf(strings[S_progDev],path);
	MyDeviceDetected = TRUE;
	rep_info_u.report_type=HID_REPORT_TYPE_OUTPUT;
	rep_info_i.report_type=HID_REPORT_TYPE_INPUT;
	rep_info_u.report_id=rep_info_i.report_id=HID_REPORT_ID_FIRST;
	rep_info_u.num_fields=rep_info_i.num_fields=1;
	ref_multi_u.uref.report_type=HID_REPORT_TYPE_OUTPUT;
	ref_multi_i.uref.report_type=HID_REPORT_TYPE_INPUT;
	ref_multi_u.uref.report_id=ref_multi_i.uref.report_id=HID_REPORT_ID_FIRST;
	ref_multi_u.uref.field_index=ref_multi_i.uref.field_index=0;
	ref_multi_u.uref.usage_index=ref_multi_i.uref.usage_index=0;
	ref_multi_u.num_values=ref_multi_i.num_values=DIMBUF;
	#endif
#else		//Windows
	PSP_DEVICE_INTERFACE_DETAIL_DATA detailData;
	HANDLE DeviceHandle;
	HANDLE hDevInfo;
	GUID HidGuid;
	char MyDevicePathName[1024];
	ULONG Length;
	ULONG Required;
	typedef struct _HIDD_ATTRIBUTES {
	    ULONG   Size;
	    USHORT  VendorID;
	    USHORT  ProductID;
	    USHORT  VersionNumber;
	} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;
	typedef void (__stdcall*GETHIDGUID) (OUT LPGUID HidGuid);
	typedef BOOLEAN (__stdcall*GETATTRIBUTES)(IN HANDLE HidDeviceObject,OUT PHIDD_ATTRIBUTES Attributes);
	typedef BOOLEAN (__stdcall*SETNUMINPUTBUFFERS)(IN  HANDLE HidDeviceObject,OUT ULONG  NumberBuffers);
	typedef BOOLEAN (__stdcall*GETNUMINPUTBUFFERS)(IN  HANDLE HidDeviceObject,OUT PULONG  NumberBuffers);
	typedef BOOLEAN (__stdcall*GETFEATURE) (IN  HANDLE HidDeviceObject, OUT PVOID ReportBuffer, IN ULONG ReportBufferLength);
	typedef BOOLEAN (__stdcall*SETFEATURE) (IN  HANDLE HidDeviceObject, IN PVOID ReportBuffer, IN ULONG ReportBufferLength);
	typedef BOOLEAN (__stdcall*GETREPORT) (IN  HANDLE HidDeviceObject, OUT PVOID ReportBuffer, IN ULONG ReportBufferLength);
	typedef BOOLEAN (__stdcall*SETREPORT) (IN  HANDLE HidDeviceObject, IN PVOID ReportBuffer, IN ULONG ReportBufferLength);
	typedef BOOLEAN (__stdcall*GETMANUFACTURERSTRING) (IN  HANDLE HidDeviceObject, OUT PVOID ReportBuffer, IN ULONG ReportBufferLength);
	typedef BOOLEAN (__stdcall*GETPRODUCTSTRING) (IN  HANDLE HidDeviceObject, OUT PVOID ReportBuffer, IN ULONG ReportBufferLength);
	typedef BOOLEAN (__stdcall*GETINDEXEDSTRING) (IN  HANDLE HidDeviceObject, IN ULONG  StringIndex, OUT PVOID ReportBuffer, IN ULONG ReportBufferLength);
	HIDD_ATTRIBUTES Attributes;
	SP_DEVICE_INTERFACE_DATA devInfoData;
	int LastDevice = FALSE;
	int MemberIndex = 0;
	LONG Result;
	Length=0;
	detailData=NULL;
	DeviceHandle=NULL;
	HMODULE hHID=0;
	GETHIDGUID HidD_GetHidGuid=0;
	GETATTRIBUTES HidD_GetAttributes=0;
	SETNUMINPUTBUFFERS HidD_SetNumInputBuffers=0;
	GETNUMINPUTBUFFERS HidD_GetNumInputBuffers=0;
	GETFEATURE HidD_GetFeature=0;
	SETFEATURE HidD_SetFeature=0;
	GETREPORT HidD_GetInputReport=0;
	SETREPORT HidD_SetOutputReport=0;
	GETMANUFACTURERSTRING HidD_GetManufacturerString=0;
	GETPRODUCTSTRING HidD_GetProductString=0;
	hHID = LoadLibrary("hid.dll");
	if(!hHID){
		PrintMessage("Can't find hid.dll");
		return 0;
	}
	HidD_GetHidGuid=(GETHIDGUID)GetProcAddress(hHID,"HidD_GetHidGuid");
	HidD_GetAttributes=(GETATTRIBUTES)GetProcAddress(hHID,"HidD_GetAttributes");
	HidD_SetNumInputBuffers=(SETNUMINPUTBUFFERS)GetProcAddress(hHID,"HidD_SetNumInputBuffers");
	HidD_GetNumInputBuffers=(GETNUMINPUTBUFFERS)GetProcAddress(hHID,"HidD_GetNumInputBuffers");
	HidD_GetFeature=(GETFEATURE)GetProcAddress(hHID,"HidD_GetFeature");
	HidD_SetFeature=(SETFEATURE)GetProcAddress(hHID,"HidD_SetFeature");
	HidD_GetInputReport=(GETREPORT)GetProcAddress(hHID,"HidD_GetInputReport");
	HidD_SetOutputReport=(SETREPORT)GetProcAddress(hHID,"HidD_SetOutputReport");
	HidD_GetManufacturerString=(GETMANUFACTURERSTRING)GetProcAddress(hHID,"HidD_GetManufacturerString");
	HidD_GetProductString=(GETPRODUCTSTRING)GetProcAddress(hHID,"HidD_GetProductString");
	if(HidD_GetHidGuid==NULL\
		||HidD_GetAttributes==NULL\
		||HidD_GetFeature==NULL\
		||HidD_SetFeature==NULL\
		||HidD_GetInputReport==NULL\
		||HidD_SetOutputReport==NULL\
		||HidD_GetManufacturerString==NULL\
		||HidD_GetProductString==NULL\
		||HidD_SetNumInputBuffers==NULL\
		||HidD_GetNumInputBuffers==NULL) return 0;
	HMODULE hSAPI=0;
	hSAPI = LoadLibrary("setupapi.dll");
	if(!hSAPI){
		PrintMessage("Can't find setupapi.dll");
		return 0;
	}
	typedef HDEVINFO (WINAPI* SETUPDIGETCLASSDEVS) (CONST GUID*,PCSTR,HWND,DWORD);
	typedef BOOL (WINAPI* SETUPDIENUMDEVICEINTERFACES) (HDEVINFO,PSP_DEVINFO_DATA,CONST GUID*,DWORD,PSP_DEVICE_INTERFACE_DATA);
	typedef BOOL (WINAPI* SETUPDIGETDEVICEINTERFACEDETAIL) (HDEVINFO,PSP_DEVICE_INTERFACE_DATA,PSP_DEVICE_INTERFACE_DETAIL_DATA_A,DWORD,PDWORD,PSP_DEVINFO_DATA);
	typedef BOOL (WINAPI* SETUPDIDESTROYDEVICEINFOLIST) (HDEVINFO);
	SETUPDIGETCLASSDEVS SetupDiGetClassDevsA=0;
	SETUPDIENUMDEVICEINTERFACES SetupDiEnumDeviceInterfaces=0;
	SETUPDIGETDEVICEINTERFACEDETAIL SetupDiGetDeviceInterfaceDetailA=0;
	SETUPDIDESTROYDEVICEINFOLIST SetupDiDestroyDeviceInfoList=0;
	SetupDiGetClassDevsA=(SETUPDIGETCLASSDEVS) GetProcAddress(hSAPI,"SetupDiGetClassDevsA");
	SetupDiEnumDeviceInterfaces=(SETUPDIENUMDEVICEINTERFACES) GetProcAddress(hSAPI,"SetupDiEnumDeviceInterfaces");
	SetupDiGetDeviceInterfaceDetailA=(SETUPDIGETDEVICEINTERFACEDETAIL) GetProcAddress(hSAPI,"SetupDiGetDeviceInterfaceDetailA");
	SetupDiDestroyDeviceInfoList=(SETUPDIDESTROYDEVICEINFOLIST) GetProcAddress(hSAPI,"SetupDiDestroyDeviceInfoList");
	if(SetupDiGetClassDevsA==NULL\
		||SetupDiEnumDeviceInterfaces==NULL\
		||SetupDiDestroyDeviceInfoList==NULL\
		||SetupDiGetDeviceInterfaceDetailA==NULL) return 0;
	/*
	The following code is adapted from Usbhidio_vc6 application example by Jan Axelson
	for more information see see http://www.lvr.com/hidpage.htm
	*/
	/*
	API function: HidD_GetHidGuid
	Get the GUID for all system HIDs.
	Returns: the GUID in HidGuid.
	*/
	HidD_GetHidGuid(&HidGuid);
	/*
	API function: SetupDiGetClassDevs
	Returns: a handle to a device information set for all installed devices.
	Requires: the GUID returned by GetHidGuid.
	*/
	hDevInfo=SetupDiGetClassDevs(&HidGuid,NULL,NULL,DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
	devInfoData.cbSize = sizeof(devInfoData);
	//Step through the available devices looking for the one we want.
	//Quit on detecting the desired device or checking all available devices without success.
	MemberIndex = 0;
	LastDevice = FALSE;
	do
	{
		/*
		API function: SetupDiEnumDeviceInterfaces
		On return, MyDeviceInterfaceData contains the handle to a
		SP_DEVICE_INTERFACE_DATA structure for a detected device.
		Requires:
		The DeviceInfoSet returned in SetupDiGetClassDevs.
		The HidGuid returned in GetHidGuid.
		An index to specify a device.
		*/
		Result=SetupDiEnumDeviceInterfaces (hDevInfo, 0, &HidGuid, MemberIndex, &devInfoData);
		if (Result != 0)
		{
			//A device has been detected, so get more information about it.
			/*
			API function: SetupDiGetDeviceInterfaceDetail
			Returns: an SP_DEVICE_INTERFACE_DETAIL_DATA structure
			containing information about a device.
			To retrieve the information, call this function twice.
			The first time returns the size of the structure in Length.
			The second time returns a pointer to the data in DeviceInfoSet.
			Requires:
			A DeviceInfoSet returned by SetupDiGetClassDevs
			The SP_DEVICE_INTERFACE_DATA structure returned by SetupDiEnumDeviceInterfaces.
			The final parameter is an optional pointer to an SP_DEV_INFO_DATA structure.
			This application doesn't retrieve or use the structure.
			If retrieving the structure, set
			MyDeviceInfoData.cbSize = length of MyDeviceInfoData.
			and pass the structure's address.
			*/
			//Get the Length value.
			//The call will return with a "buffer too small" error which can be ignored.
			Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &Length, NULL);
			//Allocate memory for the hDevInfo structure, using the returned Length.
			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
			//Set cbSize in the detailData structure.
			detailData -> cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			//Call the function again, this time passing it the returned buffer size.
			Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, Length,&Required, NULL);
			// Open a handle to the device.
			// To enable retrieving information about a system mouse or keyboard,
			// don't request Read or Write access for this handle.
			/*
			API function: CreateFile
			Returns: a handle that enables reading and writing to the device.
			Requires:
			The DevicePath in the detailData structure
			returned by SetupDiGetDeviceInterfaceDetail.
			*/
			DeviceHandle=CreateFile(detailData->DevicePath,
				0, FILE_SHARE_READ|FILE_SHARE_WRITE,
				(LPSECURITY_ATTRIBUTES)NULL,OPEN_EXISTING, 0, NULL);
			/*
			API function: HidD_GetAttributes
			Requests information from the device.
			Requires: the handle returned by CreateFile.
			Returns: a HIDD_ATTRIBUTES structure containing
			the Vendor ID, Product ID, and Product Version Number.
			Use this information to decide if the detected device is
			the one we're looking for.
			*/
			//Set the Size to the number of bytes in the structure.
			Attributes.Size = sizeof(Attributes);
			Result = HidD_GetAttributes(DeviceHandle,&Attributes);
			//Is it the desired device?
			MyDeviceDetected = FALSE;
			if (Attributes.VendorID == vid)
			{
				if (Attributes.ProductID == pid)
				{
					//Both the Vendor ID and Product ID match.
					MyDeviceDetected = TRUE;
					strcpy(MyDevicePathName,detailData->DevicePath);
					// Get a handle for writing Output reports.
					WriteHandle=CreateFile(detailData->DevicePath,
						GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
						(LPSECURITY_ATTRIBUTES)NULL,OPEN_EXISTING,0,NULL);
					//Get a handle to the device for the overlapped ReadFiles.
					ReadHandle=CreateFile(detailData->DevicePath,
						GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,(LPSECURITY_ATTRIBUTES)NULL,
						OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
					if (hEventObject) CloseHandle(hEventObject);
					hEventObject = CreateEvent(NULL,TRUE,TRUE,"");
					//Set the members of the overlapped structure.
					HIDOverlapped.hEvent = hEventObject;
					HIDOverlapped.Offset = 0;
					HIDOverlapped.OffsetHigh = 0;
					Result=HidD_SetNumInputBuffers(DeviceHandle,64);
				}
				else
					//The Product ID doesn't match.
					CloseHandle(DeviceHandle);
			}
			else
				//The Vendor ID doesn't match.
				CloseHandle(DeviceHandle);
		//Free the memory used by the detailData structure (no longer needed).
		free(detailData);
		}
		else
			//SetupDiEnumDeviceInterfaces returned 0, so there are no more devices to check.
			LastDevice=TRUE;
		//If we haven't found the device yet, and haven't tried every available device,
		//try the next one.
		MemberIndex = MemberIndex + 1;
	} //do
	while ((LastDevice == FALSE) && (MyDeviceDetected == FALSE));
	//Free the memory reserved for hDevInfo by SetupDiClassDevs.
	SetupDiDestroyDeviceInfoList(hDevInfo);

	if(info&&MyDeviceDetected == TRUE){
		char string[1024];
		PrintMessage3("Device detected: vid=0x%04X pid=0x%04X\nPath: %s\n",vid,pid,MyDevicePathName);
		if(HidD_GetManufacturerString(DeviceHandle,string,sizeof(string))==TRUE) wprintf(L"Manufacturer string: %s\n",string);
		if(HidD_GetProductString(DeviceHandle,string,sizeof(string))==TRUE) wprintf(L"Product string: %s\n",string);
	}
#endif
	if (MyDeviceDetected == FALSE){
		PrintMessage(strings[S_noprog]);	//"Programmer not detected\r\n"
		//gtk_statusbar_push(status_bar,statusID,strings[S_noprog]);
	}
	else{
		PrintMessage(strings[S_prog]);	//"Programmer detected\r\n");
		//gtk_statusbar_push(status_bar,statusID,strings[S_prog]);
	}
	return MyDeviceDetected;
}

void PrintMessage(str) { puts(str); }

void PrintStatus(s,p1,p2) { printf("\b\b\b\b%3d%",p1); fflush(stdout); }
void PrintStatusSetup() { printf("    "); }
void PrintStatusEnd() { printf("\b\b\b\b"); }
void PrintStatusClear() {}
