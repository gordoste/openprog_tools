/*
 * progAVR.c - algorithms to program the Atmel AVR family of microcontrollers
 * Copyright (C) 2009-2021 Alberto Maccioni
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

#include "common.h"

#define  LOCK	1
#define  FUSE	2
#define  FUSE_H 4
#define  FUSE_X	8
#define  CAL	16
#define  SLOW	256
#define  RST 0x40

struct AVRID{
	int id;
	char *device;
} AVRLIST[]={
	//1K
	{0x9001,"AT90S1200"},
	{0x9004,"ATtiny11"},
	{0x9005,"ATtiny12"},
	{0x9007,"ATtiny13"},
	//2K
	{0x9101,"AT90S2313"},
	{0x9109,"ATtiny26"},
	{0x910A,"ATtiny2313"},
	{0x910B,"ATtiny24"},
	{0x910C,"ATtiny261"},
	//4K
	{0x9205,"ATmega48"},
	{0x920A,"ATmega48PA"},
	{0x9207,"ATtiny44"},
	{0x9208,"ATtiny461"},
	{0x9209,"ATtiny48"},
	{0x920D,"ATtiny4313"},
	//8K
	{0x9301,"AT90S8515"},
	{0x9303,"AT90S8535"},
	{0x9306,"ATmega8515"},
	{0x9307,"ATmega8"},
	{0x9308,"ATmega8535"},
	{0x930A,"ATmega88"},
	{0x930C,"ATtiny84"},
	{0x930D,"ATtiny861"},
	{0x930F,"ATmega88PA"},
	{0x9311,"ATtiny88"},
	//16K
	{0x9403,"ATmega16"},
	{0x9406,"ATmega168"},
	{0x940A,"ATmega164PA"},
	{0x940B,"ATmega168PA"},
	{0x940F,"ATmega164A"},
	//32K
	{0x950F,"ATmega328P"},
	{0x9511,"ATmega324PA"},
	{0x9514,"ATmega328"},
	{0x9515,"ATmega324A"},
	//64K
	{0x9602,"ATmega64"},
	{0x9609,"ATmega644A"},
	{0x960A,"ATmega644PA"},
	//128K
	{0x9705,"ATmega1284P"},
	{0x9706,"ATmega1284"},
};

void AtmelID(BYTE id[])
{
	char str[128]="";
	int i,idw=(id[1]<<8)+id[2];
	if(id[0]==0&&id[1]==1&&id[2]==2){
		PrintMessage(strings[S_Protected]);		//"Device protected"
		return;
	}
	if(id[0]==0x1E) strcat(str,"Atmel ");
	for(i=0;i<sizeof(AVRLIST)/sizeof(AVRLIST[0]);i++){
		if(idw==AVRLIST[i].id){
			strcat(str,AVRLIST[i].device);
			i=1000;
		}
	}
	switch(id[1]){
		case 0x90:
			strcat(str," 1KB Flash");
			break;
		case 0x91:
			strcat(str," 2KB Flash");
			break;
		case 0x92:
			strcat(str," 4KB Flash");
			break;
		case 0x93:
			strcat(str," 8KB Flash");
			break;
		case 0x94:
			strcat(str," 16KB Flash");
			break;
		case 0x95:
			strcat(str," 32KB Flash");
			break;
		case 0x96:
			strcat(str," 64KB Flash");
			break;
		case 0x97:
			strcat(str," 128KB Flash");
			break;
	}
	if(i!=1001) strcat(str,strings[S_nodev]); //"Unknown device\r\n");
	else strcat(str,"\r\n");
	PrintMessage(str);
}

void DisplayCODEAVR(int dim){
// display AVR CODE memory
	char s[256]="",t[256]="";
	int valid=0,empty=1,i,j,lines=0;
	char* aux=(char*)malloc((dim/COL+1)*(16+COL*6));
	aux[0]=0;
	s[0]=0;
	for(i=0;i<dim&&i<size;i+=COL*2){
		valid=0;
		for(j=i;j<i+COL*2&&j<dim;j++){
			sprintf(t,"%02X ",memCODE[j]);
			strcat(s,t);
			if(memCODE[j]<0xff) valid=1;
		}
		if(valid){
			sprintf(t,"%04X: %s\r\n",i,s);
			strcat(aux,t);
			empty=0;
			lines++;
			if(lines>500){	//limit number of lines printed
				strcat(aux,"(...)\r\n");
				i=(dim<size?dim:size)-COL*4;
				lines=490;
			}
		}
		s[0]=0;
	}
	if(empty) PrintMessage(strings[S_Empty]);	//empty
	else PrintMessage(aux);
	free(aux);
}

//Software SPI for very slow speed
//RB1=CLK, RB0=MISO, RC7=MOSI, RC6=RESET
//speed in bit/s  min 10
int SWSPI(int data, int speed){
	int i,j,din=0;
	double Tbit=1.0/speed*1e6;		//Tbit in us
	if(saveLog)	fprintf(logfile,"SWSPI(0x%X,%d)\n",data,speed);
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=(int)(Tbit/2)>>8;
	bufferU[j++]=(int)(Tbit/2)&0xff;
	bufferU[j++]=SET_PORT_DIR;
	bufferU[j++]=0xF5;	//TRISB
	bufferU[j++]=0x3F;	//TRISA-C  (RC7:RC6:RA5:RA4:RA3:X:X:X)
	for(i=0;i<4;i++){	//first nibble
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=0;			//PORTB CLK=0
		bufferU[j++]=data&0x80;	//PORTA-C
		bufferU[j++]=WAIT_T3;	//half bit delay
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=2;			//PORTB CLK=1
		bufferU[j++]=data&0x80;	//PORTA-C
		bufferU[j++]=READ_B;
		bufferU[j++]=WAIT_T3;	//half bit delay
		data<<=1;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(1+4*(Tbit/1000.0+0.5));
	for(j=0;j<DIMBUF-1&&bufferI[j]!=READ_B;j++);
	din+=bufferI[j+1]&1;
	din<<=1;
	for(j+=2;j<DIMBUF-1&&bufferI[j]!=READ_B;j++);
	din+=bufferI[j+1]&1;
	din<<=1;
	for(j+=2;j<DIMBUF-1&&bufferI[j]!=READ_B;j++);
	din+=bufferI[j+1]&1;
	din<<=1;
	for(j+=2;j<DIMBUF-1&&bufferI[j]!=READ_B;j++);
	din+=bufferI[j+1]&1;
	din<<=1;
	j=0;
	for(i=0;i<4;i++){	//second nibble
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=0;			//PORTB CLK=0
		bufferU[j++]=data&0x80;	//PORTA-C
		bufferU[j++]=WAIT_T3;	//half bit delay
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=2;			//PORTB CLK=1
		bufferU[j++]=data&0x80;	//PORTA-C
		bufferU[j++]=READ_B;
		bufferU[j++]=WAIT_T3;	//half bit delay
		data<<=1;
	}
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;			//PORTB CLK=0
	bufferU[j++]=0;			//PORTA-C
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(1+4*(Tbit/1000.0+0.5));
	for(j=0;j<DIMBUF-1&&bufferI[j]!=READ_B;j++);
	din+=bufferI[j+1]&1;
	din<<=1;
	for(j+=2;j<DIMBUF-1&&bufferI[j]!=READ_B;j++);
	din+=bufferI[j+1]&1;
	din<<=1;
	for(j+=2;j<DIMBUF-1&&bufferI[j]!=READ_B;j++);
	din+=bufferI[j+1]&1;
	din<<=1;
	for(j+=2;j<DIMBUF-1&&bufferI[j]!=READ_B;j++);
	din+=bufferI[j+1]&1;
	if(saveLog)	fprintf(logfile,"Read 0x%X\n",din);
	return din;
}

//Write fuse at low speed
//speed in bit/s  min 10
void WriteATfuseSlow(int fuse){
#define SPEED 3000
	int j,d;
	fuse&=0xFF;
	if(FWVersion<0x900){
		PrintMessage1(strings[S_FWver2old],"0.9.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"WriteATfuseSlow(0x%X)\n",fuse);
	}
	PrintMessage(strings[S_FuseAreaW]);	//"Write Fuse ... "
	j=0;
	bufferU[j++]=VREG_DIS;		//Disable HV reg
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=SET_PORT_DIR;
	bufferU[j++]=0xF5;	//TRISB
	bufferU[j++]=0x3F;	//TRISA-C  (RC7:RC6:RA5:RA4:RA3:X:X:X)
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=CLOCK_GEN;
	bufferU[j++]=1;				//0=100k,200k,500k,1M,2M,3M
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	msDelay(20);	//min 20ms from VDD
	SWSPI(0xAC,SPEED);
	SWSPI(0x53,SPEED);
	d=SWSPI(0,SPEED);
	SWSPI(0,SPEED);
	if(d!=0x53){	//does not respond
		j=0;
		bufferU[j++]=EN_VPP_VCC;	//VDD
		bufferU[j++]=0x0;
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		PrintMessage(strings[S_SyncErr]);	//"Synchronization error\r\n"
		if(saveLog) CloseLogFile();
		return;
	}
	SWSPI(0xAC,SPEED);		//Write fuse
	SWSPI(0xA0,SPEED);
	SWSPI(0x00,SPEED);
	SWSPI(fuse,SPEED);
	msDelay(9);
	SWSPI(0x50,SPEED);		//Read fuse
	SWSPI(0x00,SPEED);
	SWSPI(0x00,SPEED);
	d=SWSPI(0,SPEED);
	if(d!=fuse){
		PrintMessage3(strings[S_WErr1],"fuse",fuse,d);	//"Error writing %s: written %02X, read %02X" NL;
	}
	j=0;
	bufferU[j++]=CLOCK_GEN;
	bufferU[j++]=0xFF;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
}

//Find right SPI speed to enter
//test mode and synchronize with AVR
//return Tbyte or 0 when sync was not found
double SyncSPI(){
	int z=0,i,j;
	double Tbyte;	//byte delay in ms
	j=0;
	bufferU[j++]=SPI_INIT;
	bufferU[j++]=2;				//0~=100k, 1~=200k, 2~=300k but really is 200k
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	for(i=1;i<256;i++){		//increase T more than linearly; Tmin=1
		if(i>10) i++;
		if(i>20) i+=2;
		if(i>40) i+=4;
		if(i>80) i+=4;
		if(i>160) i+=8;
		Tbyte=(20+i*4)/1000.0;  //from firmware simulation
		j=0;
		bufferU[j++]=CLOCK_GEN;	//reset with clock at 0
		bufferU[j++]=0xFF;
		bufferU[j++]=EN_VPP_VCC;	//VDD=0
		bufferU[j++]=0x0;
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=EN_VPP_VCC;	//VDD
		bufferU[j++]=0x1;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T1T2;
		bufferU[j++]=i;			//force T
		bufferU[j++]=0;
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=0;
		bufferU[j++]=RST;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=CLOCK_GEN;
		bufferU[j++]=5;				//0=100k,200k,500k,1M,2M,3M
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(9);
		msDelay(20); 	//min 20ms from reset
		j=0;
		bufferU[j++]=SPI_WRITE;		//Programming enable
		bufferU[j++]=2;
		bufferU[j++]=0xAC;
		bufferU[j++]=0x53;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=2;
		bufferU[j++]=SPI_WRITE;		//just to test communication
		bufferU[j++]=11;
		bufferU[j++]=0x55;
		bufferU[j++]=0x00;
		bufferU[j++]=0xAA;
		bufferU[j++]=0x00;
		bufferU[j++]=0x55;
		bufferU[j++]=0x00;
		bufferU[j++]=0xAA;
		bufferU[j++]=0x00;
		bufferU[j++]=0x30;
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x30;
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x30;
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x30;
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x30;
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=SPI_WRITE;		//Programming enable
		bufferU[j++]=2;
		bufferU[j++]=0xAC;
		bufferU[j++]=0x53;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=2;
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x30;
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(1.5+32*Tbyte*1.1);
		int d0,d1,d2,d3,d4,d5,d6,d7;
		for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		//if(bufferI[z+2]==0x53) break;
		d0=bufferI[z+2];
		for(z+=bufferI[z+1]+1;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		d1=bufferI[z+2];
		for(z+=bufferI[z+1]+1;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		d2=bufferI[z+2];
		for(z+=bufferI[z+1]+1;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		d3=bufferI[z+2];
		for(z+=bufferI[z+1]+1;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		d4=bufferI[z+2];
		for(z+=bufferI[z+1]+1;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		d5=bufferI[z+2];
		for(z+=bufferI[z+1]+1;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		d6=bufferI[z+2];
		for(z+=bufferI[z+1]+1;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		d7=bufferI[z+2];
		//verify that received data is correct
		if(d0==0x53&&d6==0x53&&d1==d2&&d1==d3&&d1==d4&&d1==d5&&d1==d7) break;
	}
	if(i>256){
		PrintMessage(strings[S_SyncErr]);	//"Synchronization error\r\n"
		if(saveLog) fprintf(logfile,strings[S_SyncErr]);
		return 0;
	}
	//Add some margin
	i++;
	i+=i/10;
	if(i>255) i=255;
	Tbyte=(20+i*4)/1000.0;  //from firmware simulation
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=i;			//force T
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	sprintf(str,strings_it[I_AT_COMM],8/Tbyte);	//"Communicating @ %.0f kbps\r\n"
	PrintMessage(str);
	if(saveLog)	fprintf(logfile,str);
	return Tbyte;
}

void ReadAT(int dim, int dim2, int options)
// read ATMEL AVR
// dim=FLASH size in bytes, dim2=EEPROM size
// options: LOCK,FUSE,FUSE_H,FUSE_X,CAL
{
	int k=0,k2=0,z=0,i,j;
	double Tbyte;	//byte delay in ms
	BYTE signature[]={0,0,0};
	if(dim>0x20000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	if(dim2>0x1000||dim2<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"ReadAT(0x%X,0x%X,0x%X)\n",dim,dim2,options);
	}
	size=dim;
	sizeEE=dim2;
	if(memCODE) free(memCODE);
	memCODE=(unsigned char*)malloc(dim);		//CODE
	if(memEE) free(memEE);
	memEE=(unsigned char*)malloc(dim2);			//EEPROM
	for(j=0;j<size;j++) memCODE[j]=0xFF;
	for(j=0;j<sizeEE;j++) memEE[j]=0xFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=VREG_DIS;		//Disable HV reg
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	Tbyte=SyncSPI();
	if(Tbyte==0){
		if(saveLog) CloseLogFile();
		return;
	}
	j=0;
	bufferU[j++]=SPI_WRITE;		//Read signature bytes
	bufferU[j++]=3;
	bufferU[j++]=0x30;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=1;
	bufferU[j++]=SPI_WRITE;		//Read signature bytes
	bufferU[j++]=3;
	bufferU[j++]=0x30;
	bufferU[j++]=0;
	bufferU[j++]=1;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=1;
	bufferU[j++]=SPI_WRITE;		//Read signature bytes
	bufferU[j++]=3;
	bufferU[j++]=0x30;
	bufferU[j++]=0;
	bufferU[j++]=2;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=1;
	if(options&LOCK){			//LOCK byte
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x58;
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
	}
	if(options&FUSE){			//FUSE byte
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x50;
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
	}
	if(options&FUSE_H){			//FUSE high byte
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x58;
		bufferU[j++]=8;
		bufferU[j++]=0;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
	}
	if(options&FUSE_X){			//extended FUSE byte
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x50;
		bufferU[j++]=8;
		bufferU[j++]=0;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(1.5+7*4.5*Tbyte);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
	signature[0]=bufferI[z+2];
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
	signature[1]=bufferI[z+2];
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
	signature[2]=bufferI[z+2];
	PrintMessage3("CHIP ID:%02X%02X%02X\r\n",signature[0],signature[1],signature[2]);
	AtmelID(signature);
	if(options&LOCK){			//LOCK byte
		for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		PrintMessage1("LOCK bits:\t  0x%02X\r\n",bufferI[z+2]);
	}
	if(options&FUSE){			//FUSE byte
		for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		PrintMessage1("FUSE bits:\t  0x%02X\r\n",bufferI[z+2]);
	}
	if(options&FUSE_H){			//FUSE high byte
		for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		PrintMessage1("FUSE HIGH bits:\t  0x%02X\r\n",bufferI[z+2]);
	}
	if(options&FUSE_X){			//extended FUSE byte
		for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		PrintMessage1("Extended FUSE bits: 0x%02X\r\n",bufferI[z+2]);
	}
	if(options&CAL){			//calibration byte
		j=0;
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x38;
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x38;
		bufferU[j++]=0;
		bufferU[j++]=1;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x38;
		bufferU[j++]=0;
		bufferU[j++]=2;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x38;
		bufferU[j++]=0;
		bufferU[j++]=3;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(1.5+4.5*4*Tbyte);
		for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		PrintMessage1("Calibration bits:\t  0x%02X",bufferI[z+2]);
		for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		PrintMessage1(",0x%02X",bufferI[z+2]);
		for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		PrintMessage1(",0x%02X",bufferI[z+2]);
		for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		PrintMessage1(",0x%02X\r\n",bufferI[z+2]);
	}
//****************** read code ********************
	PrintMessage(strings[S_CodeReading1]);		//read code ...
	if(saveLog) fprintf(logfile,"%s\n",strings[S_CodeReading1]);		//read code ...
	PrintStatusSetup();
	int c=(DIMBUF-5)/2;
	for(i=0,j=0;i<dim;i+=c*2){
		bufferU[j++]=AT_READ_DATA;
		bufferU[j++]=i<(dim-2*c)?c:(dim-i)/2;
		bufferU[j++]=i>>9;
		bufferU[j++]=i>>1;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(1.5+240*Tbyte);
		if(bufferI[0]==AT_READ_DATA){	//static references, verify!!
			for(z=2;z<bufferI[1]*2+2&&z<DIMBUF;z++) memCODE[k++]=bufferI[z];
		}
		PrintStatus(strings[S_CodeReading],i*100/(dim+dim2),i);	//"Reading: %3d%%, add. %03X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog) fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadCodeErr2],dim,k);	//"Errore in lettura area programma, richiesti %d byte, letti %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** read eeprom ********************
	if(dim2){
		PrintMessage(strings[S_ReadEE]);		//read EE ...
		if(saveLog) fprintf(logfile,"%s\n",strings[S_ReadEE]);		//Read EEPROM ...
		PrintStatusSetup();
		int n=0;
		for(k2=0,i=0,j=0;i<dim2;i++){
			bufferU[j++]=SPI_WRITE;		//Read eeprom memory
			bufferU[j++]=3;
			bufferU[j++]=0xA0;
			bufferU[j++]=i>>8;
			bufferU[j++]=i;
			bufferU[j++]=SPI_READ;
			bufferU[j++]=1;
			n++;
			if(j>DIMBUF-9||i==dim-1){
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(1.5+4*n*Tbyte*1.3);
				for(z=0;z<DIMBUF-2;z++){
					if(bufferI[z]==SPI_READ&&bufferI[z+1]==1){
						memEE[k2++]=bufferI[z+2];
						z+=3;
					}
				}
				PrintStatus(strings[S_CodeReading],(i+dim)*100/(dim+dim2),i);	//"Reading: %3d%%, add. %03X"
				if(RWstop) i=dim2;
				j=0;
				n=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
			}
		}
		PrintStatusEnd();
		if(k2!=dim2){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ReadEEErr],dim2,k2);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
		}
		else PrintMessage(strings[S_Compl]);	//"completed\r\n"
	}
//****************** exit program mode ********************
	bufferU[j++]=CLOCK_GEN;
	bufferU[j++]=0xFF;
	bufferU[j++]=SPI_WRITE;
	bufferU[j++]=1;
	bufferU[j++]=0;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
//****************** visualize ********************
	PrintMessage(strings[S_CodeMem]);	//"\r\nProgram memory\r\n"
	DisplayCODEAVR(dim);
	if(dim2){
		DisplayEE();	//visualize EE
	}
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

// function 18F ATtiny1x ATtiny84
// SCI (I)  RC6   PB3      PB0
// SII (I)  RB0   PB1      PA5
// SDI (I)  RC7   PB0      PA6
// SDO (O)  RB1   PB2      PA4
#define SDI 0x10
#define SII 0x1
#define SDO 0x2
#define SCI 0x8
/// read ATMEL AVR using HV serial programming
/// dim=FLASH size in bytes, dim2=EEPROM size
/// options: LOCK,FUSE,FUSE_H,FUSE_X,CAL
void ReadAT_HV(int dim, int dim2, int options)
{
	int k=0,z=0,i,j;
	BYTE signature[]={0,0,0};
	if(FWVersion<0x900){
		PrintMessage1(strings[S_FWver2old],"0.9.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(dim>0x20000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	if(dim2>0x800||dim2<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"ReadAT_HV(0x%X,0x%X,0x%X)\n",dim,dim2,options);
	}
	size=dim;
	sizeEE=dim2;
	if(memCODE) free(memCODE);
	memCODE=(unsigned char*)malloc(dim);		//CODE
	if(memEE) free(memEE);
	memEE=(unsigned char*)malloc(dim2);			//EEPROM
	for(j=0;j<size;j++) memCODE[j]=0xFF;
	for(j=0;j<sizeEE;j++) memEE[j]=0xFF;
	if(!StartHVReg(12)){
		PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
		return;
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=SET_PORT_DIR;
	bufferU[j++]=0xFC;
	bufferU[j++]=0x7;
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=SCI;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=SCI;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=SCI;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=SCI;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=SCI;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=SCI;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EN_VPP_VCC;	//VDD + VPP
	bufferU[j++]=0x5;
	bufferU[j++]=SET_PORT_DIR;	//RELEASE SDO
	bufferU[j++]=0xFE;
	bufferU[j++]=0x7;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	j=0;
	bufferU[j++]=AT_HV_RTX;		//Read signature bytes
	bufferU[j++]=4;
	bufferU[j++]=0x4C;
	bufferU[j++]=0x08;
	bufferU[j++]=0x0C;
	bufferU[j++]=0x00;
	bufferU[j++]=0x68;
	bufferU[j++]=0x00;
	bufferU[j++]=0x6C;
	bufferU[j++]=0x00;
	bufferU[j++]=AT_HV_RTX;		//Read signature bytes
	bufferU[j++]=4;
	bufferU[j++]=0x4C;
	bufferU[j++]=0x08;
	bufferU[j++]=0x0C;
	bufferU[j++]=0x01;
	bufferU[j++]=0x68;
	bufferU[j++]=0x00;
	bufferU[j++]=0x6C;
	bufferU[j++]=0x00;
	bufferU[j++]=AT_HV_RTX;		//Read signature bytes
	bufferU[j++]=4;
	bufferU[j++]=0x4C;
	bufferU[j++]=0x08;
	bufferU[j++]=0x0C;
	bufferU[j++]=0x02;
	bufferU[j++]=0x68;
	bufferU[j++]=0x00;
	bufferU[j++]=0x6C;
	bufferU[j++]=0x00;
	if(options&LOCK){			//LOCK byte
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=3;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x04;
		bufferU[j++]=0x78;
		bufferU[j++]=0x00;
		bufferU[j++]=0x7C;
		bufferU[j++]=0x00;
	}
	if(options&FUSE){			//FUSE byte
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=3;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x04;
		bufferU[j++]=0x68;
		bufferU[j++]=0x00;
		bufferU[j++]=0x6C;
		bufferU[j++]=0x00;
	}
	if(options&FUSE_H){			//FUSE high byte
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=3;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x04;
		bufferU[j++]=0x7A;
		bufferU[j++]=0x00;
		bufferU[j++]=0x7E;
		bufferU[j++]=0x00;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	for(z=0;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
	signature[0]=bufferI[z+1];
	for(z+=2;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
	signature[1]=bufferI[z+1];
	for(z+=2;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
	signature[2]=bufferI[z+1];
	PrintMessage3("CHIP ID:%02X%02X%02X\r\n",signature[0],signature[1],signature[2]);
	AtmelID(signature);
	if(options&LOCK){			//LOCK byte
		for(z+=2;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
		PrintMessage1("LOCK byte:\t  0x%02X\r\n",bufferI[z+1]);
	}
	if(options&FUSE){			//FUSE byte
		for(z+=2;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
		PrintMessage1("FUSE byte:\t  0x%02X\r\n",bufferI[z+1]);
	}
	if(options&FUSE_H){			//FUSE high byte
		for(z+=2;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
		PrintMessage1("FUSE HIGH byte:\t  0x%02X\r\n",bufferI[z+1]);
	}
	if(options&FUSE_X){			//extended FUSE byte
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=3;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x04;
		bufferU[j++]=0x6A;
		bufferU[j++]=0x00;
		bufferU[j++]=0x6E;
		bufferU[j++]=0x00;
	}
	if(options&CAL){			//calibration byte
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=4;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x08;
		bufferU[j++]=0x0C;
		bufferU[j++]=0x00;
		bufferU[j++]=0x78;
		bufferU[j++]=0x00;
		bufferU[j++]=0x7C;
		bufferU[j++]=0x00;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	j=0;
	PacketIO(2);
	z=0;
	if(options&FUSE_X){			//extended FUSE byte
		for(;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
		PrintMessage1("Extended FUSE byte: 0x%02X\r\n",bufferI[z+1]);
		z+=2;
	}
	if(options&CAL){			//calibration byte
		for(;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
		PrintMessage1("Calibration byte: 0x%02X\r\n",bufferI[z+1]);
	}
//****************** read code ********************
	if(saveLog)fprintf(logfile,"READ CODE\n");
	PrintMessage(strings[S_CodeReading1]);		//read code ...
	PrintStatusSetup();
	j=0;
	bufferU[j++]=AT_HV_RTX;		//Read FLASH
	bufferU[j++]=1;
	bufferU[j++]=0x4C;
	bufferU[j++]=0x02;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	for(i=0;i<dim;){
		if((i&511)==0){ //change high address after 256 words
			bufferU[j++]=AT_HV_RTX;
			bufferU[j++]=1;
			bufferU[j++]=0x1C;
			bufferU[j++]=i>>9;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(2);
			j=0;
		}
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=3;
		bufferU[j++]=0x0C;
		bufferU[j++]=(i>>1)&0xFF;
		bufferU[j++]=0x68;
		bufferU[j++]=0x00;
		bufferU[j++]=0x6C;
		bufferU[j++]=0x00;
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=2;
		bufferU[j++]=0x78;
		bufferU[j++]=0x00;
		bufferU[j++]=0x7C;
		bufferU[j++]=0x00;
		i+=2;
		if(j>DIMBUF-14||i>=dim-2){
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(2);
			for(z=0;z<DIMBUF-1;z++){
				if(bufferI[z]==AT_HV_RTX){
					memCODE[k++]=bufferI[z+1];
					z+=1;
				}
			}
			PrintStatus(strings[S_CodeReading],i*100/(dim+dim2),i);	//"Read: %d%%, addr. %03X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadCodeErr],dim,k);	//"Error reading code area, requested %d words, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);
//****************** read eeprom ********************
	if(dim2){
		if(saveLog)fprintf(logfile,"READ EEPROM\n");
		PrintMessage(strings[S_ReadEE]);		//read EE ...
		PrintStatusSetup();
		j=0;
		k=0;
		bufferU[j++]=AT_HV_RTX;		//Read EEPROM
		bufferU[j++]=1;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x03;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		for(i=0;i<dim2;i++){
			if((i&255)==0){
				bufferU[j++]=AT_HV_RTX;
				bufferU[j++]=1;
				bufferU[j++]=0x1C;
				bufferU[j++]=i>>8;
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(2);
				j=0;
			}
			bufferU[j++]=AT_HV_RTX;
			bufferU[j++]=3;
			bufferU[j++]=0x0C;
			bufferU[j++]=i&0xFF;
			bufferU[j++]=0x68;
			bufferU[j++]=0x00;
			bufferU[j++]=0x6C;
			bufferU[j++]=0x00;
			if(j>DIMBUF-8||i>=dim2-2){
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(2);
				for(z=0;z<DIMBUF-1;z++){
					if(bufferI[z]==AT_HV_RTX){
						memEE[k++]=bufferI[z+1];
						z+=1;
					}
				}
				PrintStatus(strings[S_CodeReading],i*100/(dim+dim2),i);	//"Read: %d%%, addr. %03X"
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
			}
		}
		PrintStatusEnd();
		if(k!=dim2){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ReadEEErr],dim2,k);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
		}
		else PrintMessage(strings[S_Compl]);
	}
//****************** exit program mode ********************
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=SET_PORT_DIR; //All input
	bufferU[j++]=0xFF;
	bufferU[j++]=0xFF;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
//****************** visualize ********************
	PrintMessage(strings[S_CodeMem]);	//"\r\nProgram memory\r\n"
	DisplayCODEAVR(dim);
	if(dim2){
		DisplayEE();	//visualize EE
	}
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

void WriteAT(int dim, int dim2, int dummy1, int dummy2)
// write ATMEL micro
// dim=FLASH size in bytes, dim2=EEPROM size
{
	int k=0,z=0,i,j;
	int err=0;//,Rtry=0,maxTry=0;
	double Tbyte;
	BYTE signature[]={0,0,0};
	if(dim>0x8000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	if(dim2>0x800||dim2<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"WriteAT(0x%X,0x%X)\n",dim,dim2);
	}
	if(dim>size) dim=size;
	if(dim2>sizeEE) dim2=sizeEE;
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Data area is empty\r\n"
		return;
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=VREG_DIS;		//Disable HV reg
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	Tbyte=SyncSPI();
	if(Tbyte==0){
		if(saveLog) CloseLogFile();
		return;
	}
	j=0;

/*	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=20000>>8;
	bufferU[j++]=20000&0xff;
	bufferU[j++]=VREG_DIS;		//Disable HV reg
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x0;
	bufferU[j++]=SPI_INIT;
	bufferU[j++]=1;
	bufferU[j++]=CLOCK_GEN;
	bufferU[j++]=5;
	bufferU[j++]=CLOCK_GEN;
	bufferU[j++]=0xFF;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=WAIT_T3;		//20ms
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;		//20ms
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	writeP();
	msDelay(50);
	readP();
	if(saveLog)WriteLogIO();
	for(i=0;i<32;i++){
		j=0;
		bufferU[j++]=CLOCK_GEN;
		bufferU[j++]=0xFF;
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=0;
		bufferU[j++]=RST;
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=CLOCK_GEN;
		bufferU[j++]=5;
		bufferU[j++]=WAIT_T3;		//20ms
		bufferU[j++]=SPI_WRITE;		//Programming enable
		bufferU[j++]=2;
		bufferU[j++]=0xAC;
		bufferU[j++]=0x53;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=2;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		writeP();
		msDelay(25);
		readP();
		if(saveLog)WriteLogIO();
		for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		//PrintMessage("i=%d z=%d   rx:%02X%02X\r\n",i,z,bufferI[z+2],bufferI[z+3]);
		if(bufferI[z+2]==0x53) i=32;
	}
	if(i<33){
		j=0;
		bufferU[j++]=EN_VPP_VCC;	//VDD
		bufferU[j++]=0x0;
		bufferU[j++]=SPI_INIT;
		bufferU[j++]=0xFF;
		bufferU[j++]=CLOCK_GEN;
		bufferU[j++]=0xFF;
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		writeP();
		msDelay(1);
		readP();
		if(saveLog)WriteLogIO();
		PrintMessage(strings[S_SyncErr]);	//"Synchronization error\r\n"
		if(saveLog) CloseLogFile();
		return;
	}
	j=0;*/
	bufferU[j++]=SPI_WRITE;		//Read signature bytes
	bufferU[j++]=3;
	bufferU[j++]=0x30;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=1;
	bufferU[j++]=SPI_WRITE;		//Read signature bytes
	bufferU[j++]=3;
	bufferU[j++]=0x30;
	bufferU[j++]=0;
	bufferU[j++]=1;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=1;
	bufferU[j++]=SPI_WRITE;		//Read signature bytes
	bufferU[j++]=3;
	bufferU[j++]=0x30;
	bufferU[j++]=0;
	bufferU[j++]=2;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=1;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(1+3*4*Tbyte);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
	signature[0]=bufferI[z+2];
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
	signature[1]=bufferI[z+2];
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
	signature[2]=bufferI[z+2];
	PrintMessage3("CHIP ID:%02X%02X%02X\r\n",signature[0],signature[1],signature[2]);
	AtmelID(signature);
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	if(saveLog)	fprintf(logfile,"%s\n",strings[S_StartErase]);
	j=0;
	bufferU[j++]=SPI_WRITE;		//Chip erase
	bufferU[j++]=4;
	bufferU[j++]=0xAC;
	bufferU[j++]=0x80;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(1+4*Tbyte);
	msDelay(25);	//Erase time
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	if(saveLog)	fprintf(logfile,"%s\n",strings[S_StartCodeProg]);
	PrintStatusSetup();
	for(i=0,j=0;i<dim;i++){
		if(memCODE[i]!=0xFF){
			bufferU[j++]=SPI_WRITE;		//Write program memory
			bufferU[j++]=4;
			bufferU[j++]=0x40+(i&1?8:0);
			bufferU[j++]=i>>9;
			bufferU[j++]=i>>1;
			bufferU[j++]=memCODE[i];
			bufferU[j++]=WAIT_T3;		//6ms
			bufferU[j++]=WAIT_T3;
			bufferU[j++]=WAIT_T3;
			bufferU[j++]=SPI_WRITE;		//Read program memory
			bufferU[j++]=3;
			bufferU[j++]=0x20+(i&1?8:0);
			bufferU[j++]=i>>9;
			bufferU[j++]=i>>1;
			bufferU[j++]=SPI_READ;
			bufferU[j++]=1;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			j=0;
			PacketIO(7+2*4*Tbyte);
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Write: %d%%, addr. %03X"
			for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
			if(z==DIMBUF-2||memCODE[i]!=bufferI[z+2]){
					err++;
			}
			if(max_err&&err>max_err){
				PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
				PrintMessage(strings[S_IntW]);	//"Write interrupted"
				i=dim;
			}
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, err=%d\n"
			}
		}
	}
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write eeprom ********************
	if(dim2){
		PrintMessage(strings[S_EEAreaW]);	//"Write EEPROM ... "
		if(saveLog)	fprintf(logfile,"%s\n",strings[S_EEAreaW]);
		PrintStatusSetup();
		int errEE=0;
		for(i=0,j=0;i<dim2;i++){
			if(memEE[i]!=0xFF){
				bufferU[j++]=SPI_WRITE;		//Write EEPROM memory
				bufferU[j++]=4;
				bufferU[j++]=0xC0;
				bufferU[j++]=i>>8;
				bufferU[j++]=i;
				bufferU[j++]=memEE[i];
				bufferU[j++]=WAIT_T3;		//6ms
				bufferU[j++]=WAIT_T3;
				bufferU[j++]=WAIT_T3;
				bufferU[j++]=SPI_WRITE;		//Read EEPROM memory
				bufferU[j++]=3;
				bufferU[j++]=0xA0;
				bufferU[j++]=i>>8;
				bufferU[j++]=i;
				bufferU[j++]=SPI_READ;
				bufferU[j++]=1;
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				j=0;
				PacketIO(7+2*4*Tbyte);
				PrintStatus(strings[S_CodeWriting],i*100/dim2,i);	//"Write: %d%%, addr. %03X"
				for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
				if(z==DIMBUF-2||memEE[i]!=bufferI[z+2]){
						errEE++;
				}
				if(max_err&&err+errEE>max_err){
					PrintMessage1(strings[S_MaxErr],err+errEE);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
					PrintMessage(strings[S_IntW]);	//"Write interrupted"
					i=dim2;
				}
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,errEE);	//"i=%d, k=%d, err=%d\n"
				}
			}
		}
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],errEE);	//"completed, %d errors\r\n"
		err+=errEE;
	}
//	if(maxTry) PrintMessage1(strings[S_MaxRetry],maxTry); 	//"Max retries in writing: %d\r\n"
//****************** write FUSE ********************
	if(AVRlock<0x100){
		PrintMessage(strings[S_FuseAreaW]);	//"Write Fuse ... "
		if(saveLog)	fprintf(logfile,"%s\n",strings[S_FuseAreaW]);
		bufferU[j++]=SPI_WRITE;		//Write lock
		bufferU[j++]=4;
		bufferU[j++]=0xAC;
		bufferU[j++]=0xF9+(AVRlock&0x06);
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=WAIT_T3;		//9ms
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		j=0;
		PacketIO(3+4*Tbyte);
		msDelay(15);
		PrintMessage(strings[S_Compl]);	//"completed\r\n"
	}
//****************** exit program mode ********************
	bufferU[j++]=CLOCK_GEN;
	bufferU[j++]=0xFF;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();
}


void WriteATmega(int dim, int dim2, int page, int options)
// write ATMEL micro
// dim=FLASH size in bytes, dim2=EEPROM, page=FLASH page size in bytes
{
	int k=0,z=0,i,j;
	double Tbyte;	//byte delay in ms
	int err=0,Rtry=0,maxTry=0;
	BYTE signature[]={0,0,0};
	if(dim>0x20000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	if(dim2>0x1000||dim2<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"WriteATmega(0x%X,0x%X,0x%X,0x%X)\n",dim,dim2,page,options);
	}
	if(dim>size) dim=size;
	else{
		size=dim;
		memCODE=(unsigned char*)realloc(memCODE,dim);
	}
	if(size%(page*2)){	//grow to an integer number of pages
		j=size;
		dim=(j/(page*2)+1)*page*2;
		memCODE=(unsigned char*)realloc(memCODE,dim);
		for(;j<dim;j++) memCODE[j]=0xFF;
	}
	if(dim2>sizeEE) dim2=sizeEE;
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Data area is empty\r\n"
		return;
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=VREG_DIS;		//Disable HV reg
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	Tbyte=SyncSPI();
	if(Tbyte==0){
		if(saveLog) CloseLogFile();
		return;
	}
	j=0;
	bufferU[j++]=SPI_WRITE;		//Read signature bytes
	bufferU[j++]=3;
	bufferU[j++]=0x30;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=1;
	bufferU[j++]=SPI_WRITE;		//Read signature bytes
	bufferU[j++]=3;
	bufferU[j++]=0x30;
	bufferU[j++]=0;
	bufferU[j++]=1;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=1;
	bufferU[j++]=SPI_WRITE;		//Read signature bytes
	bufferU[j++]=3;
	bufferU[j++]=0x30;
	bufferU[j++]=0;
	bufferU[j++]=2;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=1;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(1.5+3*4.5*Tbyte);
	if(saveLog)WriteLogIO();
	for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
	signature[0]=bufferI[z+2];
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
	signature[1]=bufferI[z+2];
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
	signature[2]=bufferI[z+2];
	PrintMessage3("CHIP ID:%02X%02X%02X\r\n",signature[0],signature[1],signature[2]);
	AtmelID(signature);
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	if(saveLog)	fprintf(logfile,"%s\n",strings[S_StartErase]);
	j=0;
	bufferU[j++]=SPI_WRITE;		//Chip erase
	bufferU[j++]=4;
	bufferU[j++]=0xAC;
	bufferU[j++]=0x80;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=5000>>8;
	bufferU[j++]=5000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(1+4*Tbyte);
	msDelay(15);	//erase time 9ms min
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	if(saveLog)	fprintf(logfile,"%s\n",strings[S_StartCodeProg]);
	PrintStatusSetup();
	int w=0,v,c;
	for(i=0;i<dim;i+=page*2){
		for(z=i,v=0;z<i+page*2;z++) if(memCODE[z]<0xFF)v=1;
		if(v){
			for(k=0,j=0,v=0;k<page;k+=w){
				w=(page-k)<(DIMBUF-6)/2?(page-k):(DIMBUF-6)/2;
				bufferU[j++]=AT_LOAD_DATA;
				bufferU[j++]=w;
				bufferU[j++]=k>>8;
				bufferU[j++]=k;
				for(z=0;z<w*2;z++)	bufferU[j++]=memCODE[i+k*2+z];
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				j=0;
				PacketIO(1.5+w*9*Tbyte);
			}
			bufferU[j++]=SPI_WRITE;		//Write program memory page
			bufferU[j++]=4;
			bufferU[j++]=0x4C;
			bufferU[j++]=i>>9;
			bufferU[j++]=i>>1;
			bufferU[j++]=0;
			bufferU[j++]=WAIT_T3;		//5ms
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			j=0;
			PacketIO(6+4*Tbyte);
			//msDelay(5);
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Write: %d%%, addr. %03X"
			if(RWstop) i=dim;
			//write verification
			c=(DIMBUF-5)/2;
			for(k=0,j=0;k<page;k+=c){
				for(Rtry=0;Rtry<5;Rtry++){		//Try to read a few times
					bufferU[j++]=AT_READ_DATA;
					bufferU[j++]=k<(page-c)?c:page-k;
					bufferU[j++]=(i+k*2)>>9;
					bufferU[j++]=(i+k*2)>>1;
					bufferU[j++]=FLUSH;
					for(;j<DIMBUF;j++) bufferU[j]=0x0;
					PacketIO(1.5+240*Tbyte);
					if(bufferI[0]==AT_READ_DATA){	//Fixed reference!!
						for(w=0,z=2;z<bufferI[1]*2+2&&z<DIMBUF;z++){
							if(memCODE[i+k*2+w]!=bufferI[z]){
								if(Rtry<4)	z=DIMBUF;
								else err++;
							}
							w++;
						}
						if(z<DIMBUF) Rtry=100;
					}
					j=0;
				}
			}
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,w,w,err);	//"i=%d, k=%d, err=%d\n"
			}
			if(max_err&&err>max_err){
				PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
				PrintMessage(strings[S_IntW]);	//"Write interrupted"
				i=dim;
			}
		}
	}
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write eeprom ********************
	if(dim2){
		PrintMessage(strings[S_EEAreaW]);	//"Write EEPROM ... "
		if(saveLog)	fprintf(logfile,"%s\n",strings[S_EEAreaW]);
		PrintStatusSetup();
		int errEE=0;
		for(i=0,j=0;i<dim2;i++){
			if(memEE[i]!=0xFF){
				bufferU[j++]=SPI_WRITE;		//Write EEPROM memory
				bufferU[j++]=4;
				bufferU[j++]=0xC0;
				bufferU[j++]=i>>8;
				bufferU[j++]=i;
				bufferU[j++]=memEE[i];
				bufferU[j++]=WAIT_T3;		//5ms
				bufferU[j++]=WAIT_T3;		//5ms
				bufferU[j++]=SPI_WRITE;		//Read EEPROM memory
				bufferU[j++]=3;
				bufferU[j++]=0xA0;
				bufferU[j++]=i>>8;
				bufferU[j++]=i;
				bufferU[j++]=SPI_READ;
				bufferU[j++]=1;
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				j=0;
				PacketIO(11+8.5*Tbyte);
				PrintStatus(strings[S_CodeWriting],i*100/dim2,i);	//"Write: %d%%, addr. %03X"
				for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
				if(z==DIMBUF-2||memEE[i]!=bufferI[z+2]){
					if(Rtry<4){
						Rtry++;
						if (Rtry>maxTry) maxTry=Rtry;
						i--;
					}
					else{
						errEE++;
						Rtry=0;
					}
				}
				if(max_err&&err+errEE>max_err){
					PrintMessage1(strings[S_MaxErr],err+errEE);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
					PrintMessage(strings[S_IntW]);	//"Write interrupted"
					i=dim2;
				}
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,errEE);	//"i=%d, k=%d, err=%d\n"
				}
			}
		}
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],errEE);	//"completed, %d errors\r\n"
		err+=errEE;
	}
//****************** write FUSE ********************
	int err_f=0;
	if(AVRlock<0x100||AVRfuse<0x100||AVRfuse_h<0x100||AVRfuse_x<0x100){
		PrintMessage(strings[S_FuseAreaW]);	//"Write Fuse ... "
		if(saveLog)	fprintf(logfile,"%s\n",strings[S_FuseAreaW]);
	}
	if(AVRfuse<0x100){
		bufferU[j++]=SPI_WRITE;		//Write fuse
		bufferU[j++]=4;
		bufferU[j++]=0xAC;
		bufferU[j++]=0xA0;
		bufferU[j++]=0;
		bufferU[j++]=AVRfuse;
		bufferU[j++]=WAIT_T3;		//5ms
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x50;
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		j=0;
		PacketIO(6+8.5*Tbyte);
		for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		if(z==DIMBUF-2||AVRfuse!=bufferI[z+2]) err_f++;
	}
	if(AVRfuse_h<0x100){
		bufferU[j++]=SPI_WRITE;		//Write fuse_h
		bufferU[j++]=4;
		bufferU[j++]=0xAC;
		bufferU[j++]=0xA8;
		bufferU[j++]=0;
		bufferU[j++]=AVRfuse_h;
		bufferU[j++]=WAIT_T3;		//5ms
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x58;
		bufferU[j++]=8;
		bufferU[j++]=0;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		j=0;
		PacketIO(6+8.5*Tbyte);
		for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		if(z==DIMBUF-2||AVRfuse_h!=bufferI[z+2]) err_f++;
	}
	if(AVRfuse_x<0x100){
		bufferU[j++]=SPI_WRITE;		//Write ext fuse
		bufferU[j++]=4;
		bufferU[j++]=0xAC;
		bufferU[j++]=0xA4;
		bufferU[j++]=0;
		bufferU[j++]=AVRfuse_x;
		bufferU[j++]=WAIT_T3;		//5ms
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x50;
		bufferU[j++]=8;
		bufferU[j++]=0;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		j=0;
		PacketIO(6+8.5*Tbyte);
		for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		if(z==DIMBUF-2||AVRfuse_x!=bufferI[z+2]) err_f++;
	}
	if(AVRlock<0x100){
		bufferU[j++]=SPI_WRITE;		//Write lock
		bufferU[j++]=4;
		bufferU[j++]=0xAC;
		bufferU[j++]=0xE0;
		bufferU[j++]=0;
		bufferU[j++]=AVRlock;
		bufferU[j++]=WAIT_T3;		//5ms
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=0x58;
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		j=0;
		PacketIO(6+8.5*Tbyte);
		for(z=0;z<DIMBUF-2&&bufferI[z]!=SPI_READ;z++);
		if(z==DIMBUF-2||AVRlock!=bufferI[z+2]) err_f++;
	}
	err+=err_f;
	if(AVRlock<0x100||AVRfuse<0x100||AVRfuse_h<0x100||AVRfuse_x<0x100){
		PrintMessage1(strings[S_ComplErr],err_f);	//"completed, %d errors\r\n"
	}
//	if(maxTry) PrintMessage(strings[S_MaxRetry],maxTry); 	//"Max retries in writing: %d\r\n"
//****************** exit program mode ********************
	bufferU[j++]=CLOCK_GEN;
	bufferU[j++]=0xFF;
	bufferU[j++]=SPI_WRITE;
	bufferU[j++]=1;
	bufferU[j++]=0;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();
}

/// Write ATMEL AVR using HV serial programming
/// dim=FLASH size in bytes, dim2=EEPROM, page=FLASH page size in words (0 if page write not supported)
/// options: not used
void WriteAT_HV(int dim, int dim2, int page, int options)
{
	int k=0,z=0,i,j,t,sdo,err=0;
	BYTE signature[]={0,0,0};
	if(FWVersion<0x900){
		PrintMessage1(strings[S_FWver2old],"0.9.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(dim>0x10000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	if(dim2>0x800||dim2<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"WriteAT_HV(0x%X,0x%X,0x%X,0x%X)\n",dim,dim2,page,options);
	}
	if(dim>size) dim=size;
	else{
		size=dim;
		memCODE=(unsigned char*)realloc(memCODE,dim);
	}
	if(page&&(size%(page*2))){	//grow to an integer number of pages
		j=size;
		dim=(j/(page*2)+1)*page*2;
		memCODE=(unsigned char*)realloc(memCODE,dim);
		for(;j<dim;j++) memCODE[j]=0xFF;
	}
	if(dim2>sizeEE) dim2=sizeEE;
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Data area is empty\r\n"
		return;
	}
	if(!StartHVReg(12)){
		PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
		return;
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=SET_PORT_DIR;
	bufferU[j++]=0xFC;
	bufferU[j++]=0x7;
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=SCI;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=SCI;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=SCI;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=SCI;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=SCI;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=SCI;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EN_VPP_VCC;	//VDD + VPP
	bufferU[j++]=0x5;
	bufferU[j++]=SET_PORT_DIR;	//RELEASE SDO
	bufferU[j++]=0xFE;
	bufferU[j++]=0x7;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	j=0;
	bufferU[j++]=AT_HV_RTX;		//Read signature bytes
	bufferU[j++]=4;
	bufferU[j++]=0x4C;
	bufferU[j++]=0x08;
	bufferU[j++]=0x0C;
	bufferU[j++]=0x00;
	bufferU[j++]=0x68;
	bufferU[j++]=0x00;
	bufferU[j++]=0x6C;
	bufferU[j++]=0x00;
	bufferU[j++]=AT_HV_RTX;		//Read signature bytes
	bufferU[j++]=4;
	bufferU[j++]=0x4C;
	bufferU[j++]=0x08;
	bufferU[j++]=0x0C;
	bufferU[j++]=0x01;
	bufferU[j++]=0x68;
	bufferU[j++]=0x00;
	bufferU[j++]=0x6C;
	bufferU[j++]=0x00;
	bufferU[j++]=AT_HV_RTX;		//Read signature bytes
	bufferU[j++]=4;
	bufferU[j++]=0x4C;
	bufferU[j++]=0x08;
	bufferU[j++]=0x0C;
	bufferU[j++]=0x02;
	bufferU[j++]=0x68;
	bufferU[j++]=0x00;
	bufferU[j++]=0x6C;
	bufferU[j++]=0x00;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(8);
	for(z=0;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
	signature[0]=bufferI[z+1];
	for(z+=2;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
	signature[1]=bufferI[z+1];
	for(z+=2;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
	signature[2]=bufferI[z+1];
	PrintMessage3("CHIP ID:%02X%02X%02X\r\n",signature[0],signature[1],signature[2]);
	AtmelID(signature);
//****************** erase memory ********************
	if(saveLog)fprintf(logfile,"CHIP ERASE\n");
	j=0;
	bufferU[j++]=AT_HV_RTX;	//Chip erase
	bufferU[j++]=3;
	bufferU[j++]=0x4C;
	bufferU[j++]=0x80;
	bufferU[j++]=0x64;
	bufferU[j++]=0x00;
	bufferU[j++]=0x6C;
	bufferU[j++]=0x00;
	bufferU[j++]=READ_B;	//check SDO
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	bufferU[j++]=READ_B;	//check SDO
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	for(t=0,sdo=0;t<20&&sdo==0;t++){
		PacketIO(2);
		for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
		sdo=bufferI[z+1]&2;
	}
	if(sdo==0&&saveLog) fprintf(logfile,"SDO=0\r\n");
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	if(saveLog)fprintf(logfile,"WRITE CODE\n");
	int currPage=-1;
	j=0;
	if(page==0){		//byte write
		for(i=0,k=0;i<dim;i+=2){
			if(memCODE[i]!=0xFF||memCODE[i+1]!=0xFF){
				bufferU[j++]=AT_HV_RTX;
				bufferU[j++]=6;
				bufferU[j++]=0x4C;	//Write FLASH
				bufferU[j++]=0x10;
				bufferU[j++]=0x1C;
				bufferU[j++]=i>>9;
				bufferU[j++]=0x0C;
				bufferU[j++]=(i/2)&0xFF;
				bufferU[j++]=0x2C;
				bufferU[j++]=memCODE[i];
				bufferU[j++]=0x64;		//write data low
				bufferU[j++]=0x00;
				bufferU[j++]=0x6C;
				bufferU[j++]=0x00;
				bufferU[j++]=READ_B;	//check SDO
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(2);
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
				bufferU[j++]=READ_B;	//check SDO
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				j=0;
				for(t=0,sdo=0;t<20&&sdo==0;t++){
					PacketIO(2);
					for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
					sdo=bufferI[z+1]&2;
				}
				if(saveLog&&sdo==0) fprintf(logfile,"SDO=0\r\n");
				bufferU[j++]=AT_HV_RTX;
				bufferU[j++]=3;
				bufferU[j++]=0x3C;
				bufferU[j++]=memCODE[i+1];
				bufferU[j++]=0x74;		//write data high
				bufferU[j++]=0x00;
				bufferU[j++]=0x7C;
				bufferU[j++]=0x00;
				bufferU[j++]=READ_B;	//check SDO
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(2);
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
				bufferU[j++]=READ_B;	//check SDO
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				j=0;
				for(t=0,sdo=0;t<20&&sdo==0;t++){
					PacketIO(2);
					for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
					sdo=bufferI[z+1]&2;
				}
				if(saveLog&&sdo==0) fprintf(logfile,"SDO=0\r\n");
				PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Write: %d%%, addr. %03X"
				//write verification
				bufferU[j++]=AT_HV_RTX;		//Read FLASH
				bufferU[j++]=3;
				bufferU[j++]=0x4C;
				bufferU[j++]=0x02;
				bufferU[j++]=0x68;
				bufferU[j++]=0x00;
				bufferU[j++]=0x6C;
				bufferU[j++]=0x00;
				bufferU[j++]=AT_HV_RTX;
				bufferU[j++]=2;
				bufferU[j++]=0x78;
				bufferU[j++]=0x00;
				bufferU[j++]=0x7C;
				bufferU[j++]=0x00;
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(2);
				j=0;
				k=i;
				for(z=0;z<DIMBUF-1;z++){
					if(bufferI[z]==AT_HV_RTX){
						if(memCODE[k]!=bufferI[z+1]){
							PrintMessage4(strings[S_CodeVError],k,k,memCODE[k],bufferI[z+1]);	//"Error writing address %4X: written %02X, read %02X\r\n"
							err++;
						}
						k++;
						z++;
					}
				}
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, err=%d\n"
				}
				if(max_err&&err>max_err){
					PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
					PrintMessage(strings[S_IntW]);	//"Write interrupted"
					i=dim;
				}
			}
		}
	}
	else{		//page write
		for(i=0;i<dim;i+=page*2){	//page in words
			for(k=0;k<page;k++){
				if(memCODE[i+k*2]!=0xFF||memCODE[i+k*2+1]!=0xFF) k=page;
			}
			if(k>page){	//only pages with data!=0xFF
				bufferU[j++]=AT_HV_RTX;
				bufferU[j++]=1;
				bufferU[j++]=0x4C;	//Write FLASH
				bufferU[j++]=0x10;
				for(k=0;k<page;k++){
					bufferU[j++]=AT_HV_RTX;
					bufferU[j++]=5;
					bufferU[j++]=0x0C;
					bufferU[j++]=(i/2+k)&0xFF;
					bufferU[j++]=0x2C;
					bufferU[j++]=memCODE[i+k*2];	//data low
					bufferU[j++]=0x3C;
					bufferU[j++]=memCODE[i+k*2+1];	//data high
					bufferU[j++]=0x7D;
					bufferU[j++]=0x00;
					bufferU[j++]=0x7C;
					bufferU[j++]=0x00;
					if(j>DIMBUF-13||k>=page||i>=dim-2){
						bufferU[j++]=FLUSH;
						for(;j<DIMBUF;j++) bufferU[j]=0x0;
						PacketIO(2);
						j=0;
						if(saveLog){
							fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
						}
					}
				}
				if((i>>9)!=currPage){	//change high address if changed
					bufferU[j++]=AT_HV_RTX;
					bufferU[j++]=1;
					bufferU[j++]=0x1C;
					bufferU[j++]=i>>9;
					currPage=i>>9;
				}
				bufferU[j++]=AT_HV_RTX;	//write page
				bufferU[j++]=2;
				bufferU[j++]=0x64;
				bufferU[j++]=0x00;
				bufferU[j++]=0x6C;
				bufferU[j++]=0x00;
				bufferU[j++]=READ_B;	//check SDO
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(2);
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
				bufferU[j++]=READ_B;	//check SDO
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				for(t=0,sdo=0;t<20&&sdo==0;t++){
					PacketIO(2);
					for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
					sdo=bufferI[z+1]&2;
				}
				if(sdo==0&&saveLog) fprintf(logfile,"SDO=0\r\n");
				PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Write: %d%%, addr. %03X"
				j=0;
				//write verification
				int m=0;
				for(k=0;k<page;k++){
					if(k==0){
						bufferU[j++]=AT_HV_RTX;		//Read FLASH
						bufferU[j++]=4;
						bufferU[j++]=0x4C;
						bufferU[j++]=0x02;
					}
					else{
						bufferU[j++]=AT_HV_RTX;
						bufferU[j++]=3;
					}
					bufferU[j++]=0x0C;
					bufferU[j++]=(i/2+k)&0xFF;
					bufferU[j++]=0x68;
					bufferU[j++]=0x00;
					bufferU[j++]=0x6C;
					bufferU[j++]=0x00;
					bufferU[j++]=AT_HV_RTX;
					bufferU[j++]=2;
					bufferU[j++]=0x78;
					bufferU[j++]=0x00;
					bufferU[j++]=0x7C;
					bufferU[j++]=0x00;
					if(j>DIMBUF-14||k>=page||i>=dim-2){
						bufferU[j++]=FLUSH;
						for(;j<DIMBUF;j++) bufferU[j]=0x0;
						PacketIO(2);
						j=0;
						for(z=0;z<DIMBUF-1;z++){
							if(bufferI[z]==AT_HV_RTX){
								if(memCODE[i+m]!=bufferI[z+1]){
									PrintMessage4(strings[S_CodeVError],i+m,i+m,memCODE[i+m],bufferI[z+1]);	//"Error writing address %4X: written %02X, read %02X\r\n"
									err++;
								}
								m++;
								z++;
							}
						}
						if(saveLog){
							fprintf(logfile,strings[S_Log8],i,i,m,m,err);	//"i=%d, k=%d, err=%d\n"
						}
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
						}
					}
				}
			}
		}
	}
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write eeprom ********************
	if(dim2){
		int errEE=0;
		PrintMessage(strings[S_EEAreaW]);	//"Write EEPROM ... "
		PrintStatusSetup();
		if(saveLog)fprintf(logfile,"WRITE EEPROM\n");
		j=0;
		for(i=0;i<dim2;i++){
			if(memEE[i]!=0xFF){
				bufferU[j++]=AT_HV_RTX;		//Write EEPROM
				bufferU[j++]=7;
				bufferU[j++]=0x4C;
				bufferU[j++]=0x11;
				bufferU[j++]=0x0C;
				bufferU[j++]=i&0xFF;
				bufferU[j++]=0x1C;
				bufferU[j++]=i>>8;
				bufferU[j++]=0x2C;
				bufferU[j++]=memEE[i];
				bufferU[j++]=0x6D;
				bufferU[j++]=0x00;
				bufferU[j++]=0x64;
				bufferU[j++]=0x00;
				bufferU[j++]=0x6C;
				bufferU[j++]=0x00;
				bufferU[j++]=READ_B;	//check SDO
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				j=0;
				PacketIO(2);
				PrintStatus(strings[S_CodeWriting],i*100/dim2,i);	//"Write: %d%%, addr. %03X"
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
				bufferU[j++]=READ_B;	//check SDO
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				j=0;
				for(t=0,sdo=0;t<20&&sdo==0;t++){
					PacketIO(2);
					for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
					sdo=bufferI[z+1]&2;
				}
				if(sdo==0&&saveLog) fprintf(logfile,"SDO=0\r\n");
				//write verification
				bufferU[j++]=AT_HV_RTX;		//Read EEPROM
				bufferU[j++]=5;
				bufferU[j++]=0x4C;
				bufferU[j++]=0x03;
				bufferU[j++]=0x1C;
				bufferU[j++]=i>>8;
				bufferU[j++]=0x0C;
				bufferU[j++]=i&0xFF;
				bufferU[j++]=0x68;
				bufferU[j++]=0x00;
				bufferU[j++]=0x6C;
				bufferU[j++]=0x00;
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(2);
				for(z=0;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
				if(memEE[i]!=bufferI[z+1]){
					PrintMessage4(strings[S_CodeVError],i,i,memEE[i],bufferI[z+1]);	//"Error writing address %4X: written %02X, read %02X\r\n"
					errEE++;
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,errEE);	//"i=%d, k=%d, errors=%d\n"
				}
				if(err+errEE>=max_err) break;
			}
		}
		PrintStatusEnd();
		err+=errEE;
		if(err>=max_err){
			PrintMessage("\r\n");
			PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
		}
		PrintMessage1(strings[S_ComplErr],errEE);	//"completed: %d errors\r\n"
	}
//****************** write FUSE ********************
	int err_f=0;
	if(AVRlock<0x100||AVRfuse<0x100||AVRfuse_h<0x100||AVRfuse_x<0x100)PrintMessage(strings[S_FuseAreaW]);	//"Write Fuse ... "
	if(AVRfuse<0x100){
		if(saveLog)fprintf(logfile,"WRITE FUSE\n");
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=4;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x40;
		bufferU[j++]=0x2C;
		bufferU[j++]=AVRfuse;
		bufferU[j++]=0x64;
		bufferU[j++]=0x00;
		bufferU[j++]=0x6C;
		bufferU[j++]=0x00;
		bufferU[j++]=READ_B;	//check SDO
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		j=0;
		PacketIO(2);
		for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
		sdo=bufferI[z+1]&2;
		bufferU[j++]=READ_B;	//check SDO
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		for(i=0;i<20&&sdo==0;i++){
			PacketIO(2);
			for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
			sdo=bufferI[z+1]&2;
		}
		if(sdo==0&&saveLog) fprintf(logfile,"SDO=0\r\n");
		j=0;
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=3;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x04;
		bufferU[j++]=0x68;
		bufferU[j++]=0x00;
		bufferU[j++]=0x6C;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		for(z=0;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
		if(z==DIMBUF-1||AVRfuse!=bufferI[z+1]){
			PrintMessage3(strings[S_WErr1],"fuse",AVRfuse,bufferI[z+1]);	//"Error writing %s: written %02X, read %02X"
			err_f++;
		}
	}
	if(AVRfuse_h<0x100){
		if(saveLog)fprintf(logfile,"WRITE FUSEH\n");
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=4;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x40;
		bufferU[j++]=0x2C;
		bufferU[j++]=AVRfuse_h;
		bufferU[j++]=0x74;
		bufferU[j++]=0x00;
		bufferU[j++]=0x7C;
		bufferU[j++]=0x00;
		bufferU[j++]=READ_B;	//check SDO
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		j=0;
		PacketIO(2);
		for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
		sdo=bufferI[z+1]&2;
		bufferU[j++]=READ_B;	//check SDO
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		for(i=0;i<20&&sdo==0;i++){
			PacketIO(2);
			for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
			sdo=bufferI[z+1]&2;
		}
		if(sdo==0&&saveLog) fprintf(logfile,"SDO=0\r\n");
		j=0;
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=3;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x04;
		bufferU[j++]=0x7A;
		bufferU[j++]=0x00;
		bufferU[j++]=0x7E;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		for(z=0;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
		if(z==DIMBUF-1||AVRfuse_h!=bufferI[z+1]){
			PrintMessage3(strings[S_WErr1],"fuseH",AVRfuse_h,bufferI[z+1]);	//"Error writing %s: written %02X, read %02X"
			err_f++;
		}
	}
	if(AVRfuse_x<0x100){
		if(saveLog)fprintf(logfile,"WRITE FUSEX\n");
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=4;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x40;
		bufferU[j++]=0x2C;
		bufferU[j++]=AVRfuse_x;
		bufferU[j++]=0x66;
		bufferU[j++]=0x00;
		bufferU[j++]=0x6E;
		bufferU[j++]=0x00;
		bufferU[j++]=READ_B;	//check SDO
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		j=0;
		PacketIO(2);
		for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
		sdo=bufferI[z+1]&2;
		bufferU[j++]=READ_B;	//check SDO
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		for(i=0;i<20&&sdo==0;i++){
			PacketIO(2);
			for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
			sdo=bufferI[z+1]&2;
		}
		if(sdo==0&&saveLog) fprintf(logfile,"SDO=0\r\n");
		j=0;
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=3;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x04;
		bufferU[j++]=0x6A;
		bufferU[j++]=0x00;
		bufferU[j++]=0x6E;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		for(z=0;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
		if(z==DIMBUF-1||AVRfuse_x!=bufferI[z+1]){
			PrintMessage3(strings[S_WErr1],"fuseX",AVRfuse_x,bufferI[z+1]);	//"Error writing %s: written %02X, read %02X"
			err_f++;
		}
	}
	if(AVRlock<0x100){
		if(saveLog)fprintf(logfile,"WRITE LOCK\n");
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=4;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x20;
		bufferU[j++]=0x2C;
		bufferU[j++]=AVRlock;
		bufferU[j++]=0x64;
		bufferU[j++]=0x00;
		bufferU[j++]=0x6C;
		bufferU[j++]=0x00;
		bufferU[j++]=READ_B;	//check SDO
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		j=0;
		PacketIO(2);
		for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
		sdo=bufferI[z+1]&2;
		bufferU[j++]=READ_B;	//check SDO
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		for(i=0;i<20&&sdo==0;i++){
			PacketIO(2);
			for(z=0;z<DIMBUF-1&&bufferI[z]!=READ_B;z++);
			sdo=bufferI[z+1]&2;
		}
		if(sdo==0&&saveLog) fprintf(logfile,"SDO=0\r\n");
		j=0;
		bufferU[j++]=AT_HV_RTX;
		bufferU[j++]=3;
		bufferU[j++]=0x4C;
		bufferU[j++]=0x04;
		bufferU[j++]=0x78;
		bufferU[j++]=0x00;
		bufferU[j++]=0x7C;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		for(z=0;z<DIMBUF-1&&bufferI[z]!=AT_HV_RTX;z++);
		if(z==DIMBUF-1||AVRlock!=bufferI[z+1]){
			PrintMessage3(strings[S_WErr1],"lock",AVRlock,bufferI[z+1]);	//"Error writing %s: written %02X, read %02X"
			err_f++;
		}
	}
	err+=err_f;
	if(AVRlock<0x100||AVRfuse<0x100||AVRfuse_h<0x100||AVRfuse_x<0x100){
		PrintMessage1(strings[S_ComplErr],err_f);	//"completed, %d errors\r\n"
	}
//****************** exit program mode ********************
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=SET_PORT_DIR; //All input
	bufferU[j++]=0xFF;
	bufferU[j++]=0xFF;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();
}
