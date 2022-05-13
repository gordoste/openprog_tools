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

char** strings;
int saveLog=0,programID=0,load_osccal=0,load_BKosccal=0;
int use_osccal=1,use_BKosccal=0;
int load_calibword=0,max_err=200;
int AVRlock=0x100,AVRfuse=0x100,AVRfuse_h=0x100,AVRfuse_x=0x100;
int ICDenable=0,ICDaddr=0x1FF0;

FILE* logfile=0;
char appName[6]=_APPNAME;
char LogFileName[512]="";
char loadfile[512]="",savefile[512]="";
char loadfileEE[512]="",savefileEE[512]="";
int info=0;
int vid=0x1209,pid=0x5432;

WORD *memCODE_W=0;
int size=0,sizeW=0,sizeEE=0,sizeCONFIG=0,sizeUSERID=0;
unsigned char *memCODE=0,*memEE=0,memID[64],memCONFIG[48],memUSERID[8];
int skipV33check=0;
int progress=0;
int RWstop=0;
int forceConfig=0;
char dev[64]="";
int devType=0x10000;
char str[4096];

int main (int argc, char **argv) {
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
	DeviceDetected=SearchDevice(&vid,&pid,info);	//connect to USB programmer
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
	devType=info.family;

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
			PrintMessage3("%04X: %s %s\r\n",i,s,v);
			empty=0;
		}
		s[0]=0;
		v[0]=0;
	}
	if(empty) PrintMessage(strings[S_Empty]);	//empty
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

void AddDevices(char *list){		//make sure list is long enough
	int i;
	static char last[8]="";
	for(i=0;i<Ndevices;i++){
		if(last[0]) strcat(list,", ");
		if(strncmp(devices[i],last,2)){
			strcat(list,"\n");
			last[0]=devices[i][0];
			last[1]=devices[i][1];
			last[2]=0;
		}
		strcat(list,devices[i]);
	}
}

void PrintMessage(const char *str) { puts(str); }
void PrintMessageI2C(const char *str) { puts(str); }

void PrintStatus(char *s,  uint16_t p1, uint16_t p2) { printf("\b\b\b\b%3d%",p1); fflush(stdout); }
void PrintStatusSetup() { printf("    "); }
void PrintStatusEnd() { printf("\b\b\b\b"); }
void PrintStatusClear() {}

void StrcatConvert(char *dst, const char *src) { strcat(dst, src); }
