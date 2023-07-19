#include "common.h"

int DeviceDetected=0;
double hvreg=0;
int FWVersion=0,HwID=0;
int skipV33check=0;

#if !defined _WIN32 && !defined __CYGWIN__
	char path[512]="";

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
#endif

///
///Wait for X milliseconds
void msDelay(double delay)
{
#if !defined _WIN32 && !defined __CYGWIN__
	long x=(int)(delay*1000.0);
	usleep(x>MIN_DELAY?x:MIN_DELAY);
#else
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
///Find the USB peripheral
/// Will try the specified VID/PID first, and then some default values
/// Return 0 if not found. If found, actual VID/PID will be in the supplied variables
int SearchDevice(int *_vid, int *_pid, bool _info) {
	int ret = 0;
	ret=FindDevice(*_vid,*_pid,_info);
	if (ret) return 1;
	ret=FindDevice(OPROG_NEW_VID,OPROG_NEW_PID,_info);	//try default
	if(ret) {
		*_vid=OPROG_NEW_VID;
		*_pid=OPROG_NEW_PID;
		return 1;
	}
	ret=FindDevice(OPROG_OLD_VID,OPROG_OLD_PID,_info); //try old one
	if (ret) {
		*_vid=OPROG_OLD_VID;
		*_pid=OPROG_OLD_PID;
		return 1;
	}
	return 0;
}

char *strcasestr(const char *haystack, const char *needle) {
    char *result = NULL;
    int needle_len, length;
    int idx, jdx;

    if (haystack != NULL && needle != NULL) {
        needle_len = (int)strlen(needle);
        length = strlen(haystack) - needle_len;
        idx = 0;
        while ((result=strchr(&haystack[idx],tolower(needle[0])))!=NULL ||
               (result=strchr(&haystack[idx],toupper(needle[0])))!=NULL) {
            idx = (int)(result - haystack);
            if (idx > length) {
                /* needle cannot fit into remaining characters */
                result = NULL;
                break;
            }
            for (jdx=0; jdx<needle_len; jdx++) {
                if (tolower(result[jdx]) != tolower(needle[jdx])) break;
            }
            if (jdx == needle_len) break; /* we found needle! */
            else idx++;
        }
    }

    return result;
}

///
///Check if a 3.3V regulator is present
int CheckV33Regulator()
{
	if (skipV33check) return 1;
	int i,j=0;
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
///Start HV regulator
int StartHVReg(double V){
	int j=0,z;
	int vreg=(int)(V*10.0);
	if(saveLog&&logfile) fprintf(logfile,"StartHVReg(%.2f)\n",V);
	DWORD t0,t;
	if(V==-1){
		bufferU[j++]=VREG_DIS;			//disable HV regulator
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
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
	if (DeviceDetected!=1)return;
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
