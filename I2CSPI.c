/*
 * I2CSPI.c - algorithms to interface generic I2C/SPI devices
 * Copyright (C) 2010 Alberto Maccioni
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


//configure for GUI or command-line
#include "common.h"

#define CS 8
#define HLD 16

void I2CReceive(int mode,int speed,int N,BYTE *buffer)
// I2C/SPI receive
// mode:
// 0 = I2C 8bit
// 1 = I2C 16bit
// 2 = SPI 00
// 3 = SPI 01
// 4 = SPI 10
// 5 = SPI 11
// speed:
// 0 = 100 kbps
// 1 = 200 kbps
// 2 = 300/400 kbps (SPI/I2C)
// 3 = 500/800 kbps (SPI/I2C)
{
	int j=0;
	if(N<0) N=0;
	if(N>60) N=60;
	if(mode<0) mode=0;
	if(mode>5) mode=5;
	if(speed<0) speed=0;
	if(speed>3) speed=3;
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"I2C-SPI receive\tmode=%d\tspeed=%d\n",mode,speed);
	}
	bufferU[j++]=VREG_DIS;		//Disable HV reg
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x1;
	if(mode<2){					//I2C mode
		bufferU[j++]=I2C_INIT;
		bufferU[j++]=(speed<<3)+(speed>0?0x40:0);	//enable slew control if >100k
	}
	else{						//SPI mode
		bufferU[j++]=EXT_PORT;	//CS=1
		bufferU[j++]=CS;
		bufferU[j++]=0;
		bufferU[j++]=EXT_PORT;	//CS=0
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=SPI_INIT;
		bufferU[j++]=speed+((mode-2)<<2);
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	if(mode==0){					//I2C read
		bufferU[j++]=I2C_READ;
		bufferU[j++]=N>(DIMBUF-4)?DIMBUF-4:N;
		bufferU[j++]=buffer[0];		//Control byte
		bufferU[j++]=buffer[1];		//Address;
	}
	else if(mode==1){				//I2C read 16bit
		bufferU[j++]=I2C_READ2;
		bufferU[j++]=N>(DIMBUF-4)?DIMBUF-4:N;
		bufferU[j++]=buffer[0];		//Control byte
		bufferU[j++]=buffer[1];		//Address H;
		bufferU[j++]=buffer[2];		//Address L;
	}
	else if(mode>=2){					//SPI read
		bufferU[j++]=SPI_READ;
		bufferU[j++]=N>(DIMBUF-5)?DIMBUF-5:N;
		bufferU[j++]=EXT_PORT;		//CS=1
		bufferU[j++]=CS;
		bufferU[j++]=0;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(10);
	if(saveLog){
		CloseLogFile();
	}
	if(bufferI[1]==I2C_READ||bufferI[1]==I2C_READ2||bufferI[1]==SPI_READ){
		if(bufferI[2]==0xFD){
			PrintMessageI2C(strings[S_I2CAckErr]); //"acknowledge error"
		}
		else if(bufferI[2]>0xFA){
			PrintMessageI2C(strings[S_InsErr]); //"unknown instruction"
		}
		else{
			char str[1024];
			char t[16];
			int i;
			if(mode==0)	sprintf(str,"> %02X %02X\r\n",bufferU[3],bufferU[4]);
			if(mode==1)	sprintf(str,"> %02X %02X %02X\r\n",bufferU[3],bufferU[4],bufferU[5]);
			strcat(str,"< ");
			for(i=0;i<bufferI[2];i++){
				sprintf(t,"%02X ",(BYTE)bufferI[i+3]);
				strcat(str,t);
				if(i&&i%16==15){
					strcat(str,"\r\n");
				}
			}
			strcat(str,"\r\n");
			PrintMessageI2C(str);
			fflush(stdout);
		}
	}
	else PrintMessageI2C(strings[S_ComErr]);
}

void I2CSend(int mode,int speed,int N,BYTE *buffer)
{
// I2C/SPI send
// mode:
// 0 = I2C 8bit
// 1 = I2C 16bit
// 2 = SPI 00
// 3 = SPI 01
// 4 = SPI 10
// 5 = SPI 11
// speed:
// 0 = 100 kbps
// 1 = 200 kbps
// 2 = 300/400 kbps (SPI/I2C)
// 3 = 500/800 kbps (SPI/I2C)
	int i,j=0;
	if(N<0) N=0;
	if(N>57) N=57;
	if(mode<0) mode=0;
	if(mode>5) mode=5;
	if(speed<0) speed=0;
	if(speed>3) speed=3;
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"I2C-SPI send\tmode=%d\tspeed=%d\n",mode,speed);
	}
	bufferU[j++]=VREG_DIS;		//Disable HV reg
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x1;
	if(mode<2){					//I2C mode
		bufferU[j++]=I2C_INIT;
		bufferU[j++]=(speed<<3)+(speed>0?0x40:0);	//enable slew control if >100k
	}
	else{						//SPI mode
		bufferU[j++]=EXT_PORT;	//CS=1
		bufferU[j++]=CS;
		bufferU[j++]=0;
		bufferU[j++]=EXT_PORT;	//CS=0
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=SPI_INIT;
		bufferU[j++]=speed+((mode-2)<<2);
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	if(mode==0){					//I2C write
		bufferU[j++]=I2C_WRITE;
		bufferU[j++]=N>(DIMBUF-5)?DIMBUF-5:N;
		bufferU[j++]=buffer[0];		//Control byte
		bufferU[j++]=buffer[1];		//Address
		for(i=0;i<bufferU[2];i++) bufferU[j++]=buffer[i+2];
	}
	else if(mode==1){				//I2C write 16bit
		bufferU[j++]=I2C_WRITE;
		bufferU[j++]=N+1>(DIMBUF-5)?DIMBUF-5:N+1;
		bufferU[j++]=buffer[0];		//Control byte
		bufferU[j++]=buffer[1];		//Address
		bufferU[j++]=buffer[2];		//Address L
		for(i=0;i<bufferU[2]-1;i++) bufferU[j++]=buffer[i+3];
	}
	if(mode==2){					//SPI write
		bufferU[j++]=SPI_WRITE;
		bufferU[j++]=N>(DIMBUF-5)?DIMBUF-5:N;
		for(i=0;i<bufferU[2];i++) bufferU[j++]=buffer[i];
		bufferU[j++]=EXT_PORT;	//CS=1
		bufferU[j++]=CS;
		bufferU[j++]=0;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(20);
	if(saveLog){
		CloseLogFile();
	}
	if(bufferI[1]==I2C_WRITE||bufferI[1]==SPI_WRITE){
		if(bufferI[2]==0xFD){
			PrintMessageI2C(strings[S_I2CAckErr]); //"acknowledge error"
		}
		else if(bufferI[2]>0xFA){
			PrintMessageI2C(strings[S_InsErr]); //"unknown instruction"
		}
		else{
			char str[1024];
			char t[16];
			int n=3;
			int i;
			sprintf(str,"> ");
			if(mode<2) n=5;
			for(i=3;i<bufferU[2]+n;i++){
				sprintf(t,"%02X ",(BYTE)bufferU[i]);
				strcat(str,t);
				if(i&&i%16==15){
					strcat(str,"\r\n");
				}
			}
			strcat(str,"\r\n");
			PrintMessageI2C(str);
		}
	}
	else PrintMessageI2C(strings[S_ComErr]);	//"communication error \r\n"
}

