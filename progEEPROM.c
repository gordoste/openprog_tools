/**
 * \file progEEPROM.c
 * algorithms to program various EEPROM types
 * Copyright (C) 2009-2016 Alberto Maccioni
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

void ReadI2C(int dim,int addr)
// read I2C memories
// dim=size in bytes
// addr:
//      [3:0]  =0: 1 byte address        =1: 2 byte address
//      [7:4]  A2:A0 value
//      [11:8] 17th address bit location (added to control byte)
{
	int k=0,z=0,i,j;
	int AX=(addr>>4)&7;
	int addr17=(addr>>8)&0xF;
	addr&=1;
	if(dim>0x30000||dim<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"ReadI2C(%d,%d)    (0x%X,0x%X)\n",dim,addr,dim,addr);
	}
	sizeEE=dim;
	if(memEE) free(memEE);
	memEE=(unsigned char*)malloc(dim);			//EEPROM
	unsigned int start=GetTickCount();
	hvreg=0;
	j=0;
	bufferU[j++]=VREG_DIS;
	bufferU[j++]=I2C_INIT;
	bufferU[j++]=AX;			//100k
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
//****************** read ********************
	PrintMessage(strings[S_ReadEE]);		//read EEPROM ...
	PrintStatusSetup();
	int inc;
	for(i=0,j=0;i<dim;i+=inc){
		if(i<0x10000&&i>0x10000-(DIMBUF-4)) inc=0x10000-i;	//do not cross 64KB boundary
		else inc=i<dim-(DIMBUF-4)?DIMBUF-4:dim-i;
		if(!addr){									//1 byte address
			bufferU[j++]=I2C_READ;
			bufferU[j++]=inc;
			bufferU[j++]=0xA0+(i>>7&0x0E);
			bufferU[j++]=i&0xFF;
		}
		else{										//2 byte address
			bufferU[j++]=I2C_READ2;
			bufferU[j++]=inc;
			bufferU[j++]=0xA0+(i>0xFFFF?addr17:0); //17th bit if>64K
			bufferU[j++]=(i>>8)&0xFF;
			bufferU[j++]=i&0xFF;
		}
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(8);
		for(j=0;j<DIMBUF-1&&bufferI[j]!=I2C_READ&&bufferI[j]!=I2C_READ2;j++);
		if(j<DIMBUF-1&&bufferI[j+1]<0xFA){
			for(z=j+2;z<j+2+bufferI[j+1]&&z<DIMBUF;z++) memEE[k++]=bufferI[z];
		}
		PrintStatus(strings[S_CodeReading2],i*100/(dim),i);	//"Read: %d%%, addr. %05X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X) \n"
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadEEErr],dim,k);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
		sizeEE=k;
	}
	else PrintMessage(strings[S_Compl]);
//****************** exit ********************
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
	DisplayEE();	//visualize
	int sum=0;
	for(i=0;i<sizeEE;i++) sum+=memEE[i];
	PrintMessage1("Checksum: 0x%X\r\n",sum&0xFFFF);
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

void WriteI2C(int dim,int addr,int page)
// write I2C memories
// dim=size in bytes
// addr:
//      [3:0]  =0: 1 byte address        =1: 2 byte address
//      [7:4]  A2:A0 value
//      [11:8] 17th address bit location (added to control byte)
// page=page size
{
	int k=0,z=0,i,j;
	int err=0;
	int AX=(addr>>4)&7;
	int addr17=(addr>>8)&0xF;
	addr&=1;
	hvreg=0;
	if(dim>0x30000||dim<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"WriteI2C(%d,%d,%d)    (0x%X,0x%X)\n",dim,addr,page,dim,addr);
	}
	if(dim>sizeEE){
		i=sizeEE;
		memEE=(unsigned char*)realloc(memEE,dim);
		for(;i<dim;i++) memEE[i]=0xFF;
		sizeEE=dim;
	}
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Data area is empty\r\n"
		return;
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=VREG_DIS;
	bufferU[j++]=I2C_INIT;
	bufferU[j++]=AX;			//100k
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
//****************** write ********************
	PrintMessage(strings[S_EEAreaW]);	//"Write EEPROM ... "
	PrintStatusSetup();
	for(;page>=DIMBUF-6;page>>=1);
	for(i=0,j=0;i<dim;i+=page){
		bufferU[j++]=I2C_WRITE;
		if(!addr){									//1 byte address
			bufferU[j++]=page;
			bufferU[j++]=0xA0+(i>>7&0x0E);
			bufferU[j++]=i&0xFF;
		}
		else{										//2 byte address
			bufferU[j++]=page+1;
			bufferU[j++]=0xA0+(i>0xFFFF?addr17:0); //17th bit if>64K
			bufferU[j++]=(i>>8)&0xFF;
			bufferU[j++]=i&0xFF;
		}
		for(k=0;k<page;k++) bufferU[j++]=memEE[i+k];
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(3);
		for(j=0;j<DIMBUF-1&&bufferI[j]!=I2C_WRITE;j++);
		if(bufferI[j]!=I2C_WRITE||bufferI[j+1]>=0xFA) i=dim+10;
		PrintStatus(strings[S_CodeWriting2],i*100/(dim),i);	//"Write: %d%%, addr. %04X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
		}
		bufferU[j++]=I2C_WRITE;
		bufferU[j++]=0;
		bufferU[j++]=0xA0; 	//ACK polling
		bufferU[j++]=0;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		int ack=0xFD;
		for(j=0;ack==0xFD&&j<20;j++){	//ACK polling until write complete
			PacketIO(2);
			for(j=0;j<DIMBUF-1&&bufferI[j]!=I2C_WRITE;j++);
			if(bufferI[j]!=I2C_WRITE||bufferI[j+1]>=0xFA) ack=0xFD;
			else ack=bufferI[j+1];
		}
		j=0;
	}
	PrintStatusEnd();
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** verify EEPROM ********************
	PrintMessage(strings[S_EEV]);	//"Verify EEPROM ... "
	PrintStatusSetup();
	k=0;
	int inc;
	for(i=0,j=0;i<dim;i+=inc){
		if(i<0x10000&&i>0x10000-(DIMBUF-4)) inc=0x10000-i;	//do not cross 64KB boundary
		else inc=i<dim-(DIMBUF-4)?DIMBUF-4:dim-i;
		if(!addr){									//1 byte address
			bufferU[j++]=I2C_READ;
			bufferU[j++]=inc;
			bufferU[j++]=0xA0+(i>>7&0x0E);
			bufferU[j++]=i&0xFF;
		}
		else{										//2 byte address
			bufferU[j++]=I2C_READ2;
			bufferU[j++]=inc;
			bufferU[j++]=0xA0+(i>0xFFFF?addr17:0); //17th bit if>64K
			bufferU[j++]=(i>>8)&0xFF;
			bufferU[j++]=i&0xFF;
		}
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(8);
		for(j=0;j<DIMBUF-1&&bufferI[j]!=I2C_READ&&bufferI[j]!=I2C_READ2;j++);
		if((bufferI[j]==I2C_READ||bufferI[j]==I2C_READ2)&&bufferI[j+1]<0xFA){
			for(z=j+2;z<(j+2+bufferI[j+1])&&z<DIMBUF;z++){
				if(memEE[k++]!=bufferI[z]){
					PrintMessage("\r\n");
					PrintMessage4(strings[S_CodeVError],i+z-3,i+z-3,memEE[k-1],bufferI[z]);	//"Error verifying address %04X (%d), written %02X, read %02X\r\n"
					err++;
				}
			}
		}
		PrintStatus(strings[S_CodeV2],i*100/(dim),i);	//"Verify: %d%%, addr. %04X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, err=%d\n"
		}
		if(err>=max_err) break;
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadEEErr],dim,k);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
	}
	PrintMessage1(strings[S_ComplErr],err);	//"completed: %d errors\r\n"
//****************** exit ********************
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

#define PRE 0x08	//RB3
#define S 0x10		//RB4
#define W 0x20		//RB5
#define ORG 0x20	//RB5

void Read93x(int dim,int na,int options)
// read 93Sx6 uW memories
// dim=size in bytes
// na=address bits
// options=0: x16 organization     =1: x8 organization
{
	int k=0,z=0,i,j,x8;
	hvreg=0;
	if(dim>0x3000||dim<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(na>13) na=13;
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"Read93x(%d,%d,%d)    (0x%X,0x%X)\n",dim,na,options,dim,na);
	}
	x8=options&1;
	sizeEE=dim;
	if(memEE) free(memEE);
	memEE=(unsigned char*)malloc(dim);			//EEPROM
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=VREG_DIS;
	bufferU[j++]=uW_INIT;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=x8?S:S+ORG;
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
//****************** read ********************
	PrintMessage(strings[S_ReadEE]);		//read EEPROM ...
	PrintStatusSetup();
	int dim2=x8?dim:dim/2;
	for(i=0;i<dim2;){
		for(j=0;j<DIMBUF-14&&i<dim2;){
			bufferU[j++]=uWTX;
			bufferU[j++]=na+3;				//READ
			bufferU[j++]=0xC0+((i>>(na-5))&0x1F);				//110aaaaa aaax0000
			bufferU[j++]=(i<<(13-na))&0xFF;
			bufferU[j++]=uWRX;
			bufferU[j++]=x8?8:16;
			bufferU[j++]=EXT_PORT;
			bufferU[j++]=x8?0:ORG;
			bufferU[j++]=0;
			bufferU[j++]=EXT_PORT;
			bufferU[j++]=x8?S:S+ORG;
			bufferU[j++]=0;
			i++;
		}
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		for(z=0;z<DIMBUF-3;z++){
			for(;bufferI[z]!=uWRX&&z<DIMBUF-3;z++);
			if(bufferI[z]==uWRX){
				if(x8) memEE[k++]=bufferI[z+2];
				else{
					memEE[k+1]=bufferI[z+2];
					memEE[k]=bufferI[z+3];
					k+=2;
				}
				z+=3;
			}
		}
		PrintStatus(strings[S_CodeReading2],i*100/dim2,i);	//"Read: %d%%, addr. %05X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X) \n"
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadEEErr],dim,k);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
		sizeEE=k;
	}
	else PrintMessage(strings[S_Compl]);
//****************** exit ********************
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
	DisplayEE();	//visualize
	int sum=0;
	for(i=0;i<sizeEE;i++) sum+=memEE[i];
	PrintMessage1("Checksum: 0x%X\r\n",sum&0xFFFF);
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

void Write93Sx(int dim,int na,int page)
// write 93Sx6 uW memories
// dim=size in bytes
// na=address bits
// page=page size (bytes)
// automatic write delay
{
	int k=0,z=0,i,j;
	int err=0;
	hvreg=0;
	if(dim>0x1000||dim<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(na>13) na=13;
	if(page>48) page=48;
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"Write93Sx(%d,%d,%d)    (0x%X,0x%X)\n",dim,na,page,dim,na);
	}
	if(dim>sizeEE){
		i=sizeEE;
		memEE=(unsigned char*)realloc(memEE,dim);
		for(;i<dim;i++) memEE[i]=0xFF;
		sizeEE=dim;
	}
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Data area is empty\r\n"
		return;
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=VREG_DIS;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=uW_INIT;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=S+W;
	bufferU[j++]=0;
	bufferU[j++]=uWTX;
	bufferU[j++]=na+3;
	bufferU[j++]=0x98;				//100 11xxx write enable
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=W;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=S+W+PRE;
	bufferU[j++]=0;
	bufferU[j++]=uWTX;
	bufferU[j++]=na+3;
	bufferU[j++]=0x98;				//100 11xxx Prot. reg. enable
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=W+PRE;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=S+W+PRE;
	bufferU[j++]=0;
	bufferU[j++]=uWTX;
	bufferU[j++]=na+3;
	bufferU[j++]=0xFF;				//111 11111111 Prot. reg. clear
	bufferU[j++]=0xF0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=W+PRE;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=S+W;
	bufferU[j++]=0;
	bufferU[j++]=uWTX;
	bufferU[j++]=na+3;
	bufferU[j++]=0x98;				//100 11xxx write enable
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=W;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=S+W+PRE;
	bufferU[j++]=0;
	bufferU[j++]=uWTX;
	bufferU[j++]=na+3;
	bufferU[j++]=0xC0;				//110 xxxxx Prot. reg. read
	bufferU[j++]=0;
	bufferU[j++]=uWRX;
	bufferU[j++]=10;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=W+PRE;
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
//****************** write ********************
	PrintMessage(strings[S_EEAreaW]);	//"Write EEPROM ... "
	PrintStatusSetup();
	int addr=0;
	for(i=0,j=0;i<dim;i+=page,addr+=(0x10000>>na)*page/2){
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=W;				//make sure to start with S=0
		bufferU[j++]=0;
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=S+W;
		bufferU[j++]=0;
		bufferU[j++]=uWTX;
		bufferU[j++]=3;
		bufferU[j++]=0xE0;			//111aaaaa aaa(a) D page write
		bufferU[j++]=uWTX;
		bufferU[j++]=na;
		bufferU[j++]=addr>>8;
		if(na>8) bufferU[j++]=addr&0xFF;
		bufferU[j++]=uWTX;
		bufferU[j++]=8*page;
		for(k=0;k<page;k+=2){
			bufferU[j++]=memEE[i+k+1];
			bufferU[j++]=memEE[i+k];
		}
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=W;
		bufferU[j++]=0;
		bufferU[j++]=EXT_PORT;
		bufferU[j++]=S+W;		//S=1 to check status
		bufferU[j++]=0;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		for(j=0;j<DIMBUF-1&&bufferI[j]!=uWTX;j++);
		if(bufferI[j]!=uWTX||bufferI[j+1]>=0xFA) i=dim+10;
		bufferU[j++]=uWRX;
		bufferU[j++]=1;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		for(z=0,k=0;z<30&&!k;z++){		//Wait until ready
			PacketIO(2);
			for(j=0;j<DIMBUF-1&&bufferI[j]!=uWRX;j++);
			if(bufferI[j]==uWRX) k=bufferI[j+2];
		}
		j=0;
		PrintStatus(strings[S_CodeWriting2],i*100/(dim),i);	//"Write: %d%%, addr. %04X"
		if(RWstop) i=dim;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
		}
	}
	PrintStatusEnd();
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** verify EEPROM ********************
	PrintMessage(strings[S_EEV]);	//"Verify EEPROM ... "
	PrintStatusSetup();
	j=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=S;
	bufferU[j++]=0;
	bufferU[j++]=uWTX;
	bufferU[j++]=na+3;				//READ (16bit)
	bufferU[j++]=0xC0;				//110aaaaa aaax0000
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	k=0;
	int n=(DIMBUF-2);
	if(n>30) n=30;	//max 240 bit = 30 Byte
	for(i=0,j=0;i<dim;i+=n){
		bufferU[j++]=uWRX;
		bufferU[j++]=i<(dim-n)?n*8:(dim-i)*8;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		for(j=0;bufferI[j]!=uWRX&&j<DIMBUF-1;j++);
		if(bufferI[j]==uWRX){
			for(z=j+2;z<j+2+bufferI[j+1]/8&&z<DIMBUF;z+=2,k+=2){
				if(memEE[k+1]!=bufferI[z]){
					PrintMessage("\r\n");
					PrintMessage4(strings[S_CodeVError],i+z-3,i+z-3,memEE[k+1],bufferI[z]);	//"Error verifying address %04X (%d), written %02X, read %02X\r\n"
					err++;
				}
				if(memEE[k]!=bufferI[z+1]){
					PrintMessage("\r\n");
					PrintMessage4(strings[S_CodeVError],i+z-3,i+z-3,memEE[k],bufferI[z+1]);	//"Error verifying address %04X (%d), written %02X, read %02X\r\n"
					err++;
				}
			}
		}
		PrintStatus(strings[S_CodeV2],i*100/(dim),i);	//"Verify: %d%%, addr. %04X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, err=%d\n"
		}
		if(err>=max_err) break;
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadEEErr],dim,k);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
	}
	PrintMessage1(strings[S_ComplErr],err);	//"completed: %d errors\r\n"
//****************** exit ********************
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

void Write93Cx(int dim,int na, int options)
// write 93Cx6 uW memories
// dim=size in bytes
// na=address bits
// options=0: x16 organization     =1: x8 organization
{
	int k=0,z=0,i,j;
	int err=0;
	hvreg=0;
	if(dim>0x1000||dim<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(na>13) na=13;
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"Write93Cx(%d,%d,%d)    (0x%X,0x%X)\n",dim,na,options,dim,na);
	}
	if(dim>sizeEE){
		i=sizeEE;
		memEE=(unsigned char*)realloc(memEE,dim);
		for(;i<dim;i++) memEE[i]=0xFF;
		sizeEE=dim;
	}
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Data area is empty\r\n"
		return;
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=VREG_DIS;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=uW_INIT;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=options==0?S+ORG+PRE:S+PRE;
	bufferU[j++]=0;
	bufferU[j++]=uWTX;
	bufferU[j++]=na+3;
	bufferU[j++]=0x98;				//100 11xxx EWEN
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=options==0?ORG+PRE:PRE;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=options==0?S+ORG+PRE:S+PRE;
	bufferU[j++]=0;
	bufferU[j++]=uWTX;
	bufferU[j++]=na+3;
	bufferU[j++]=0x90;				//100 10xxx ERAL
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=options==0?ORG+PRE:PRE;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=options==0?S+ORG+PRE:S+PRE;
	bufferU[j++]=0;
	bufferU[j++]=uWRX;
	bufferU[j++]=1;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	j=0;
	bufferU[j++]=uWRX;
	bufferU[j++]=1;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	for(i=0,k=0;i<30&&!k;i++){		//Wait until ready
		PacketIO(2);
		for(j=0;j<DIMBUF-1&&bufferI[j]!=uWRX;j++);
		if(bufferI[j]==uWRX) k=bufferI[j+2];
	}
//****************** write ********************
	PrintMessage(strings[S_EEAreaW]);	//"Write EEPROM ... "
	PrintStatusSetup();
	int addr=0;
	j=0;
	for(i=0;i<dim;i+=options==0?2:1,addr+=0x10000>>na){
		if(memEE[i]<0xFF||(options==0&&memEE[i+1]<0xFF)){
			bufferU[j++]=EXT_PORT;
			bufferU[j++]=options==0?ORG+PRE:PRE;
			bufferU[j++]=0;
			bufferU[j++]=EXT_PORT;
			bufferU[j++]=options==0?S+ORG+PRE:S+PRE;
			bufferU[j++]=0;
			bufferU[j++]=uWTX;
			bufferU[j++]=3;
			bufferU[j++]=0xA0;			//101aaaaa aaa(a) write
			bufferU[j++]=uWTX;
			bufferU[j++]=na;
			bufferU[j++]=addr>>8;
			if(na>8) bufferU[j++]=addr&0xFF;
			bufferU[j++]=uWTX;
			if(options==0){		//x16
				bufferU[j++]=16;
				bufferU[j++]=memEE[i+1];
				bufferU[j++]=memEE[i];
			}
			else{				//x8
				bufferU[j++]=8;
				bufferU[j++]=memEE[i];
			}
			bufferU[j++]=EXT_PORT;
			bufferU[j++]=options==0?ORG+PRE:PRE;
			bufferU[j++]=0;
			bufferU[j++]=EXT_PORT;
			bufferU[j++]=options==0?S+ORG+PRE:S+PRE;
			bufferU[j++]=0;
			bufferU[j++]=uWRX;
			bufferU[j++]=1;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(2);
			PrintStatus(strings[S_CodeWriting2],i*100/(dim),i);	//"Write: %d%%, addr. %04X"
			if(RWstop) i=dim;
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
			bufferU[j++]=uWRX;
			bufferU[j++]=1;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			for(z=0,k=0;z<30&&!k;z++){		//Wait until ready
				PacketIO(2);
				for(j=0;j<DIMBUF-1&&bufferI[j]!=uWRX;j++);
				if(bufferI[j]==uWRX) k=bufferI[j+2];
			}
			j=0;
		}
	}
	msDelay(2);
	PrintStatusEnd();
	if(i!=dim){
		PrintMessage2(strings[S_CodeWError4],i,dim);	//"Error writing code area, requested %d bytes, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** verify EEPROM ********************
	PrintMessage(strings[S_EEV]);	//"Verify EEPROM ... "
	PrintStatusSetup();
	j=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=options==0?ORG+PRE:PRE;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=options==0?S+ORG+PRE:S+PRE;
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	k=0;
	int dim2=options==0?dim/2:dim;
	for(i=0;i<dim2;){
		for(j=0;j<DIMBUF-14&&i<dim2;){
			bufferU[j++]=uWTX;
			bufferU[j++]=na+3;				//READ
			bufferU[j++]=0xC0+((i>>(na-5))&0x1F);				//110aaaaa aaax0000
			bufferU[j++]=(i<<(13-na))&0xFF;
			bufferU[j++]=uWRX;
			bufferU[j++]=options==0?16:8;
			bufferU[j++]=EXT_PORT;
			bufferU[j++]=options==0?ORG:0;
			bufferU[j++]=0;
			bufferU[j++]=EXT_PORT;
			bufferU[j++]=options==0?S+ORG:S;
			bufferU[j++]=0;
			i++;
		}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
		for(z=0;z<DIMBUF-3;z++){
			for(;bufferI[z]!=uWRX&&z<DIMBUF-3;z++);
			if(bufferI[z]==uWRX){
				if(options==1){		//x8
					if(memEE[k]!=bufferI[z+2]){
						PrintMessage("\r\n");
						PrintMessage4(strings[S_CodeVError],k,k,memEE[k],bufferI[z+2]);	//"Error verifying address %04X (%d), written %02X, read %02X\r\n"
						err++;
					}
					k++;
				}
				else{				//x16
					if(memEE[k]!=bufferI[z+3]){
						PrintMessage("\r\n");
						PrintMessage4(strings[S_CodeVError],k,k,memEE[k],bufferI[z+3]);	//"Error verifying address %04X (%d), written %02X, read %02X\r\n"
						err++;
					}
					if(memEE[k+1]!=bufferI[z+2]){
						PrintMessage("\r\n");
						PrintMessage4(strings[S_CodeVError],k+1,k+1,memEE[k+1],bufferI[z+2]);	//"Error verifying address %04X (%d), written %02X, read %02X\r\n"
						err++;
					}
					k+=2;
				}
				z+=3;
			}
		}
		PrintStatus(strings[S_CodeV2],i*100/dim2,i);	//"Verify: %d%%, addr. %04X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, err=%d\n"
		}
		if(err>=max_err) break;
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadEEErr],dim,k);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
	}
	PrintMessage1(strings[S_ComplErr],err);	//"completed: %d errors\r\n"
//****************** exit ********************
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

#define CS 8
#define HLD 16		//Hold
#define WP 0x40		//Write protect

void Read25xx(int dim)
// read 25xx SPI memories
// dim=size in bytes
{
	int k=0,z=0,i,j,ID;
	hvreg=0;
	if(dim>0x1000000||dim<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"Read25xx(%d)    (0x%X)\n",dim,dim);
	}
	sizeEE=dim;
	if(memEE) free(memEE);
	memEE=(unsigned char*)malloc(dim);			//EEPROM
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=VREG_DIS;
	bufferU[j++]=SPI_INIT;
	bufferU[j++]=3;				//0=100k, 1=200k, 2=300k, 3=500k (in reality 200k)
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=2;			//force T=28us -> 286 kbps
	bufferU[j++]=0;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=0
	bufferU[j++]=CS+HLD;
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
	bufferU[j++]=HLD;
	bufferU[j++]=WP;
	bufferU[j++]=SPI_WRITE;		//READ ID
	bufferU[j++]=1;
	bufferU[j++]=0x9F;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=3;
	bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
	bufferU[j++]=CS+HLD;
	bufferU[j++]=WP;
	bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=0
	bufferU[j++]=HLD;
	bufferU[j++]=0;
	bufferU[j++]=SPI_WRITE;		//Read
	if(dim>0x10000){				//24 bit address
		bufferU[j++]=4;
		bufferU[j++]=3;
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=0;
	}
	else if(dim>0x200){				//16 bit address
		bufferU[j++]=3;
		bufferU[j++]=3;
		bufferU[j++]=0;
		bufferU[j++]=0;
	}
	else{						//8 bit address
		bufferU[j++]=2;
		bufferU[j++]=3;
		bufferU[j++]=0;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	for(z=0;z<DIMBUF-4&&bufferI[z]!=SPI_READ;z++);
	ID=(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
	if(ID>0&&ID!=0xFFFFFF) 	PrintMessage1("DEVICE ID=0x%06X\r\n",ID);
//****************** read ********************
	PrintMessage(strings[S_ReadEE]);		//read EEPROM ...
	PrintStatusSetup();
	for(i=0,j=0;i<dim;i+=DIMBUF-4){
		bufferU[j++]=SPI_READ;
		bufferU[j++]=i<dim-(DIMBUF-4)?DIMBUF-4:dim-i;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(4);
		for(j=0;j<DIMBUF-1&&bufferI[j]!=SPI_READ;j++);
		if(bufferI[j]==SPI_READ&&bufferI[j+1]<0xFA){
			for(z=j+2;z<j+2+bufferI[j+1]&&z<DIMBUF;z++) memEE[k++]=bufferI[z];
		}
		PrintStatus(strings[S_CodeReading2],i*100/(dim),i);	//"Read: %d%%, addr. %05X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X) \n"
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadEEErr],dim,k);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
		sizeEE=k;
	}
	else PrintMessage(strings[S_Compl]);
//****************** exit ********************
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
	DisplayEE();	//visualize
	int sum=0;
	for(i=0;i<sizeEE;i++) sum+=memEE[i];
	PrintMessage1("Checksum: 0x%X\r\n",sum&0xFFFF);
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

void Write25xx(int dim,int options)
// write SPI memories
// dim=size in bytes
// options:
//        [11:0]=page size
//        [12]=erase before write
//        [13]=use status register 2
// automatic write delay
{
	int k=0,z=0,i,j,ID;
	int err=0;
	hvreg=0;
	int page=options&0xFFF;
	if(dim>0x1000000||dim<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"Write25xx(%d,%d)    (0x%X,0x%X)\n",dim,options,dim,options);
	}
	if(dim>sizeEE){
		i=sizeEE;
		memEE=(unsigned char*)realloc(memEE,dim);
		for(;i<dim;i++) memEE[i]=0xFF;
		sizeEE=dim;
	}
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Data area is empty\r\n"
		return;
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=VREG_DIS;
	bufferU[j++]=SPI_INIT;
	bufferU[j++]=3;				//0=100k, 1=200k, 2=300k, 3=500k
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
	bufferU[j++]=CS+HLD;
	bufferU[j++]=WP;
	bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
	bufferU[j++]=HLD;
	bufferU[j++]=WP;
	bufferU[j++]=SPI_WRITE;		//READ ID
	bufferU[j++]=1;
	bufferU[j++]=0x9F;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=3;
	bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
	bufferU[j++]=CS+HLD;
	bufferU[j++]=WP;
	bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
	bufferU[j++]=HLD;
	bufferU[j++]=WP;
	bufferU[j++]=SPI_WRITE;		//READ STATUS
	bufferU[j++]=1;
	bufferU[j++]=5;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=1;
	bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
	bufferU[j++]=CS+HLD;
	bufferU[j++]=WP;
	if(options&0x2000){
		bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
		bufferU[j++]=HLD;
		bufferU[j++]=WP;
		bufferU[j++]=SPI_WRITE;		//READ STATUS2
		bufferU[j++]=1;
		bufferU[j++]=0x35;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
		bufferU[j++]=CS+HLD;
		bufferU[j++]=WP;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	j=0;
	for(z=0;z<DIMBUF-4&&bufferI[z]!=SPI_READ;z++);
	ID=(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
	if(ID>0&&ID!=0xFFFFFF) 	PrintMessage1("DEVICE ID=0x%06X\r\n",ID);
	msDelay(10);	//wait power-up timer in some devices
	bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
	bufferU[j++]=HLD;
	bufferU[j++]=WP;
	bufferU[j++]=SPI_WRITE;		//WRITE ENABLE
	bufferU[j++]=1;
	bufferU[j++]=6;
	bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
	bufferU[j++]=CS+HLD;
	bufferU[j++]=WP;
	bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
	bufferU[j++]=HLD;
	bufferU[j++]=WP;
	if(options&0x2000){		//status 1+2
		bufferU[j++]=SPI_WRITE;		//WRITE STATUS
		bufferU[j++]=3;
		bufferU[j++]=1;
		bufferU[j++]=0;
		bufferU[j++]=0;
	}
	else{
		bufferU[j++]=SPI_WRITE;		//WRITE STATUS
		bufferU[j++]=2;
		bufferU[j++]=1;
		bufferU[j++]=0;
	}
	bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
	bufferU[j++]=CS+HLD;
	bufferU[j++]=WP;
	bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
	bufferU[j++]=HLD;
	bufferU[j++]=WP;
	bufferU[j++]=SPI_WRITE;		//READ STATUS
	bufferU[j++]=1;
	bufferU[j++]=5;
	bufferU[j++]=SPI_READ;
	bufferU[j++]=1;
	bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
	bufferU[j++]=CS+HLD;
	bufferU[j++]=WP;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(25);
	j=0;
	if(options&0x1000){		//erase before write
		PrintMessage(strings[S_StartErase]);	//"Erasing ... "
		bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
		bufferU[j++]=HLD;
		bufferU[j++]=WP;
		bufferU[j++]=SPI_WRITE;		//READ STATUS
		bufferU[j++]=1;
		bufferU[j++]=5;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
		bufferU[j++]=CS+HLD;
		bufferU[j++]=WP;
		if(options&0x2000){
			bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
			bufferU[j++]=HLD;
			bufferU[j++]=WP;
			bufferU[j++]=SPI_WRITE;		//READ STATUS2
			bufferU[j++]=1;
			bufferU[j++]=0x35;
			bufferU[j++]=SPI_READ;
			bufferU[j++]=1;
			bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
			bufferU[j++]=CS+HLD;
			bufferU[j++]=WP;
		}
		bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
		bufferU[j++]=HLD;
		bufferU[j++]=WP;
		bufferU[j++]=SPI_WRITE;		//WRITE ENABLE
		bufferU[j++]=1;
		bufferU[j++]=6;
		bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
		bufferU[j++]=CS+HLD;
		bufferU[j++]=WP;
		bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
		bufferU[j++]=HLD;
		bufferU[j++]=WP;
		bufferU[j++]=SPI_WRITE;		//CHIP ERASE
		bufferU[j++]=1;
		bufferU[j++]=0xC7;
		bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
		bufferU[j++]=CS+HLD;
		bufferU[j++]=WP;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(5);
		j=0;
		bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
		bufferU[j++]=HLD;
		bufferU[j++]=WP;
		bufferU[j++]=SPI_WRITE;		//READ STATUS
		bufferU[j++]=1;
		bufferU[j++]=5;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
		bufferU[j++]=CS+HLD;
		bufferU[j++]=WP;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		int pp;
		for(pp=1,j=0;pp&&j<400;j++){	//wait for erase completion max 40s
			PacketIO(2);
			msDelay(100);
			for(z=0;z<DIMBUF-1&&bufferI[z]!=SPI_READ;z++);
			pp=bufferI[z+2]&1;	//WIP bit
		}
		if(saveLog) fprintf(logfile,"Erase time %d ms\n",j*100);
		PrintMessage(strings[S_Compl]);	//"completed"
//		msDelay(100);
	}
//****************** write ********************
	PrintMessage(strings[S_EEAreaW]);	//"Write EEPROM ... "
	if(saveLog) fprintf(logfile,"%s\n",strings[S_EEAreaW]);	//"Write EEPROM ... "
	int pp;
	for(i=0,j=0;i<dim;i+=page){
		if(options&0x1000){		//if chip erase skip empty pages
			for(k=page;k==page&&i<dim-page;){
				for(k=0;k<page;k++){ if(memEE[i+k]<0xFF) k=page;}
				if(k==page) i+=page;
			}
		}
		bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
		bufferU[j++]=HLD;
		bufferU[j++]=WP;
		bufferU[j++]=SPI_WRITE;		//WRITE ENABLE
		bufferU[j++]=1;
		bufferU[j++]=6;
		bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
		bufferU[j++]=CS+HLD;
		bufferU[j++]=WP;
		bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
		bufferU[j++]=HLD;
		bufferU[j++]=WP;
		bufferU[j++]=SPI_WRITE;		//WRITE
		if(dim>0x10000){				//24 bit address
			bufferU[j++]=4;
			bufferU[j++]=2;
			bufferU[j++]=i>>16;
			bufferU[j++]=(i>>8)&0xFF;
			bufferU[j++]=i&0xFF;
		}
		else if(dim>0x200){				//16 bit address
			bufferU[j++]=3;
			bufferU[j++]=2;
			bufferU[j++]=i>>8;
			bufferU[j++]=i&0xFF;
		}
		else{						//8 bit address
			bufferU[j++]=2;
			bufferU[j++]=2+(i&0x100?8:0);
			bufferU[j++]=i&0xFF;
		}
		pp=page<DIMBUF-j-4?page:DIMBUF-j-4;
		for(k=0;k<page;){
			bufferU[j++]=SPI_WRITE;
			bufferU[j++]=pp;
			for(;k<page&&pp;k++,pp--) bufferU[j++]=memEE[i+k];
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(2);
			for(z=0;z<DIMBUF-1&&bufferI[z]!=SPI_WRITE;z++);
			if(bufferI[z+1]>=0xFA) k=i=dim+10;
			pp=(page-k)<DIMBUF-4?page-k:DIMBUF-4;
			j=0;
		}
		bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
		bufferU[j++]=CS+HLD;
		bufferU[j++]=WP;

		bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
		bufferU[j++]=HLD;
		bufferU[j++]=WP;
		bufferU[j++]=SPI_WRITE;		//READ STATUS
		bufferU[j++]=1;
		bufferU[j++]=5;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
		bufferU[j++]=CS+HLD;
		bufferU[j++]=WP;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=1
		bufferU[j++]=HLD;
		bufferU[j++]=WP;
		bufferU[j++]=SPI_WRITE;		//READ STATUS
		bufferU[j++]=1;
		bufferU[j++]=5;
		bufferU[j++]=SPI_READ;
		bufferU[j++]=1;
		bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
		bufferU[j++]=CS+HLD;
		bufferU[j++]=WP;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		for(pp=1,j=0;pp&&j<50;j++){	//wait for write completion
			PacketIO(2);
			for(z=0;z<DIMBUF-1&&bufferI[z]!=SPI_READ;z++);
			pp=bufferI[z+2]&1;	//WIP bit
		}
		PrintStatus(strings[S_CodeWriting2],i*100/(dim),i);	//"Write: %d%%, addr. %04X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
		}
	}
	PrintStatusEnd();
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** verify EEPROM ********************
	PrintMessage(strings[S_EEV]);	//"Verify EEPROM ... "
	if(saveLog) fprintf(logfile,"%s\n",strings[S_EEV]);	//"Verify EEPROM ... "
	PrintStatusSetup();
	j=0;
	bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=0
	bufferU[j++]=HLD;
	bufferU[j++]=0;
	bufferU[j++]=SPI_WRITE;		//Read
	if(dim>0x10000){				//24 bit address
		bufferU[j++]=4;
		bufferU[j++]=3;
		bufferU[j++]=0;
		bufferU[j++]=0;
		bufferU[j++]=0;
	}
	else if(dim>0x200){				//16 bit address
		bufferU[j++]=3;
		bufferU[j++]=3;
		bufferU[j++]=0;
		bufferU[j++]=0;
	}
	else{						//8 bit address
		bufferU[j++]=2;
		bufferU[j++]=3;
		bufferU[j++]=0;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	int i0,k2,valid;
	k=0;
	for(i=0,j=0;i<dim;i+=DIMBUF-4){
		if(options&0x1000){		//skip empty space if erase before write
			i0=i;
			for(valid=0;!valid&&i<dim;i+=valid?0:DIMBUF-4){		//skip verification if 0xFF
				for(k2=0;k2<DIMBUF-4&&!valid&&i+k2<dim;k2++) if(memEE[i+k2]<0xFF) valid=1;
			}
			if(i>=dim) break;
			if(i>i0){				//some data was skipped; update current address
				j=0;
				bufferU[j++]=EXT_PORT;	//CS=1, HLD=1, WP=1
				bufferU[j++]=CS+HLD;
				bufferU[j++]=WP;
				bufferU[j++]=EXT_PORT;	//CS=0, HLD=1, WP=0
				bufferU[j++]=HLD;
				bufferU[j++]=0;
				bufferU[j++]=SPI_WRITE;		//Read
				if(dim>0x10000){				//24 bit address
					bufferU[j++]=4;
					bufferU[j++]=3;
					bufferU[j++]=i>>16;
					bufferU[j++]=(i>>8)&0xFF;
					bufferU[j++]=i&0xFF;
				}
				else if(dim>0x200){				//16 bit address
					bufferU[j++]=3;
					bufferU[j++]=3;
					bufferU[j++]=i>>8;
					bufferU[j++]=i&0xFF;
				}
				else{						//8 bit address
					bufferU[j++]=2;
					bufferU[j++]=3+(i&0x100?8:0);
					bufferU[j++]=i&0xFF;
				}
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(2);
				j=0;
			}
		}
		bufferU[j++]=SPI_READ;
		bufferU[j++]=i<dim-(DIMBUF-4)?DIMBUF-4:dim-i;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(4);
		for(j=0;j<DIMBUF-1&&bufferI[j]!=SPI_READ;j++);
		if(bufferI[j]==SPI_READ&&bufferI[j+1]<0xFA){
			for(z=0;z<bufferI[j+1]&&z<DIMBUF;z++){
				if(memEE[i+z]!=bufferI[z+j+2]){
					PrintMessage4(strings[S_CodeVError],i+z,i+z,memEE[i+z],bufferI[z+3]);	//"Error verifying address %04X (%d), written %02X, read %02X\r\n"
					err++;
				}
			}
		}
		PrintStatus(strings[S_CodeV2],i*100/(dim),i);	//"Verify: %d%%, addr. %04X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, err=%d\n"
		}
		if(err>=max_err) break;
	}
	PrintStatusEnd();
	if(i<dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadEEErr],dim,i);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
	}
	PrintMessage1(strings[S_ComplErr],err);	//"completed: %d errors\r\n"
//****************** exit ********************
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

struct ID_OW{
	int id;
	char *device;
} OW_LIST[]={
	{0x10,"DS1820\r\n"},
	{0x14,"DS2430\r\n"},
	{0x23,"DS2433\r\n"},
	{0x28,"DS18B20\r\n"},
	{0x2D,"DS2431\r\n"},
	{0x43,"DS28EC20\r\n"},
};

void OW_ID(int id)
{
	char s[64];
	int i;
	for(i=0;i<sizeof(OW_LIST)/sizeof(OW_LIST[0]);i++){
		if(id==OW_LIST[i].id){
			sprintf(s,OW_LIST[i].device,id);
			PrintMessage(s);
			return;
		}
	}
	sprintf(s,"%s",strings[S_nodev]); //"Unknown device\r\n");
	PrintMessage(s);
}

#define READ_ROM 0x33
#define MATCH_ROM 0x55
#define SKIP_ROM 0xCC
#define SEARCH_ROM 0xF0
#define WRITE_SCRATCHPAD 0x0F
#define READ_SCRATCHPAD 0xAA
#define COPY_SCRATCHPAD 0x55
#define READ_MEMORY 0xF0
#define WRITE_APP_REGISTER 0x99
#define READ_STAT_REGISTER 0x66
#define READ_APP_REGISTER 0xC3
#define COPY_LOCK_APP_REGISTER 0x5A

void ReadOneWireMem(int dim,int options)
// read OneWire memories
// dim=size in bytes
// options:
//          1=status register + application register
//          2=protection bytes + ID after memory area
{
	int k=0,z=0,i,j;
	hvreg=0;
	if(FWVersion<0x800){
		PrintMessage1(strings[S_FWver2old],"0.8.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(dim>0x10000||dim<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"ReadOneWireMem(%d)    (0x%X)\n",dim,dim);
	}
	sizeEE=dim;
	if(memEE) free(memEE);
	memEE=(unsigned char*)malloc(dim);			//EEPROM
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=VREG_DIS;
	bufferU[j++]=uW_INIT;	//set RB1=0 to use as GND terminal beside RB0
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=OW_RESET;
	bufferU[j++]=OW_WRITE;
	bufferU[j++]=1;
	bufferU[j++]=READ_ROM;
	bufferU[j++]=OW_READ;
	bufferU[j++]=8;
	if(dim<=32){	//1 byte address
		bufferU[j++]=OW_WRITE;
		bufferU[j++]=2;
		bufferU[j++]=READ_MEMORY;
		bufferU[j++]=0;	//address
	}
	else{			//2 byte address
		bufferU[j++]=OW_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=READ_MEMORY;
		bufferU[j++]=0;	//address
		bufferU[j++]=0;	//address
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(12);
	j=0;
	for(z=0;bufferI[z]!=OW_RESET&&z<DIMBUF;z++);
	if(bufferI[z]==OW_RESET&&bufferI[z+1]==0){	//no presence pulse
		PrintMessage(strings[S_ComErr]);		//communication error
		bufferU[j++]=EN_VPP_VCC;		//turn off
		bufferU[j++]=0x0;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		return;
	}
	for(;bufferI[z]!=OW_READ&&z<DIMBUF;z++);
	if(bufferI[z]==OW_READ&&z<DIMBUF-9){
		PrintMessage1("Family code: 0x%02X ",bufferI[z+2]);
		OW_ID(bufferI[z+2]);
		PrintMessage3("Serial ID: 0x%02X%02X%02X",bufferI[z+3],bufferI[z+4],bufferI[z+5]);
		PrintMessage3("%02X%02X%02X",bufferI[z+6],bufferI[z+7],bufferI[z+8]);
		PrintMessage1("\r\nCRC: 0x%02X\r\n",bufferI[z+9]);
	}
//****************** read ********************
	PrintStatusSetup();
	for(i=0,j=0;i<dim;i+=DIMBUF-4){
		bufferU[j++]=OW_READ;
		bufferU[j++]=i<dim-(DIMBUF-4)?DIMBUF-4:dim-i;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(37);
		for(j=0;bufferI[j]!=OW_READ&&j<DIMBUF;j++);
		if(bufferI[j]==OW_READ&&bufferI[j+1]<0xFA){
			for(z=j+2;z<j+2+bufferI[j+1]&&z<DIMBUF;z++) memEE[k++]=bufferI[z];
		}
		PrintStatus(strings[S_CodeReading2],i*100/(dim),i);	//"Read: %d%%, addr. %05X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X) \n"
		}
	}
	if(options==1){		//read status register + application register
		bufferU[j++]=OW_RESET;
		bufferU[j++]=OW_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=SKIP_ROM;
		bufferU[j++]=READ_STAT_REGISTER;
		bufferU[j++]=0;
		bufferU[j++]=OW_READ;
		bufferU[j++]=1;
		bufferU[j++]=OW_RESET;
		bufferU[j++]=OW_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=SKIP_ROM;
		bufferU[j++]=READ_APP_REGISTER;
		bufferU[j++]=0;
		bufferU[j++]=OW_READ;
		bufferU[j++]=8;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(15);
		j=0;
		for(z=0;bufferI[z]!=OW_READ&&z<DIMBUF-2;z++);
		if(z<DIMBUF-2) PrintMessage1("Status register: 0x%02X\r\n",bufferI[z+2]);
		for(z+=2;bufferI[z]!=OW_READ&&z<DIMBUF-10;z++);
		PrintMessage("Application register: 0x");
		for(i=z+2;i<z+10&&i<DIMBUF;i++) PrintMessage1("%02X",bufferI[i]);
		PrintMessage("\r\n");
	}
	else if(options==2){		//read protection & ID bytes
		bufferU[j++]=OW_READ;
		if(dim==0xA00) bufferU[j++]=0x24;
		else bufferU[j++]=8;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(25);
		j=0;
		for(z=0;bufferI[z]!=OW_READ&&z<DIMBUF-2;z++);
		if(bufferI[z]==OW_READ&&bufferI[z+1]<0xFA){
			if(dim==0xA00){
				for(i=0;i<10;i++) PrintMessage2("Protection Control Byte Block %d: 0x%02X\r\n",i,bufferI[z+2+i]);
				PrintMessage("User EEPROM:\r\n");
				for(i=0;i<10;i++) PrintMessage1("%02X",bufferI[z+2+i+10]);
				PrintMessage("\r\n");
				for(i=0;i<10;i++) PrintMessage1("%02X",bufferI[z+2+i+20]);
				PrintMessage1("\r\nMemory Block Lock: 0x%02X\r\n",bufferI[z+2+30]);
				PrintMessage1("Register Page Lock: 0x%02X\r\n",bufferI[z+2+31]);
				PrintMessage1("Factory Byte: 0x%02X\r\n",bufferI[z+2+32]);
				PrintMessage2("Factory Trim Bytes: 0x%02X%02X\r\n",bufferI[z+2+33],bufferI[z+2+34]);
				PrintMessage2("Manufacturer ID: 0x%02X%02X\r\n",bufferI[z+2+35],bufferI[z+2+36]);
			}
			else{
				PrintMessage1("Protection Control Byte Page 0: 0x%02X\r\n",bufferI[z+2]);
				PrintMessage1("Protection Control Byte Page 1: 0x%02X\r\n",bufferI[z+2+1]);
				PrintMessage1("Protection Control Byte Page 2: 0x%02X\r\n",bufferI[z+2+2]);
				PrintMessage1("Protection Control Byte Page 3: 0x%02X\r\n",bufferI[z+2+3]);
				PrintMessage1("Copy Protection Byte: 0x%02X\r\n",bufferI[z+2+4]);
				PrintMessage1("Factory Byte: 0x%02X\r\n",bufferI[z+2+5]);
				PrintMessage2("User Bytes/Manufacturer ID: 0x%02X%02X\r\n",bufferI[z+2+6],bufferI[z+2+7]);
			}
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadEEErr],dim,k);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
		sizeEE=k;
	}
//****************** exit ********************
	bufferU[j++]=EN_VPP_VCC;		//turn off
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
	DisplayEE();	//visualize
	int sum=0;
	for(i=0;i<sizeEE;i++) sum+=memEE[i];
	PrintMessage1("Checksum: 0x%X\r\n",sum&0xFFFF);
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

void WriteOneWireMem(int dim,int options)
// write OneWire memories
// dim=size in bytes
// options:
//          0 = 8 byte scratchpad
//          1 = 32 byte scratchpad
{
	int k=0,z=0,i,j;
	int err=0;
	hvreg=0;
	if(FWVersion<0x800){
		PrintMessage1(strings[S_FWver2old],"0.8.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(dim>0x10000||dim<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"WriteOneWireMem(%d,%d)    (0x%X,0x%X)\n",dim,options,dim,options);
	}
	if(dim>sizeEE){
		i=sizeEE;
		memEE=(unsigned char*)realloc(memEE,dim);
		for(;i<dim;i++) memEE[i]=0xFF;
		sizeEE=dim;
	}
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Data area is empty\r\n"
		return;
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=VREG_DIS;
	bufferU[j++]=uW_INIT;	//set RB1=0 to use as GND terminal beside RB0
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=OW_RESET;
	bufferU[j++]=OW_WRITE;
	bufferU[j++]=1;
	bufferU[j++]=READ_ROM;
	bufferU[j++]=OW_READ;
	bufferU[j++]=8;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(12);
	j=0;
	for(z=0;bufferI[z]!=OW_RESET&&z<DIMBUF;z++);
	if(bufferI[z]==OW_RESET&&bufferI[z+1]==0){	//no presence pulse
		PrintMessage(strings[S_ComErr]);		//communication error
		bufferU[j++]=EN_VPP_VCC;		//turn off
		bufferU[j++]=0x0;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		if(saveLog) CloseLogFile();
		return;
	}
	for(z=0;bufferI[z]!=OW_READ&&z<DIMBUF;z++);
	if(bufferI[z]==OW_READ){
		PrintMessage1("Family code: 0x%02X ",bufferI[z+2]);
		OW_ID(bufferI[z+2]);
		PrintMessage3("Serial ID: 0x%02X%02X%02X",bufferI[z+3],bufferI[z+4],bufferI[z+5]);
		PrintMessage3("%02X%02X%02X",bufferI[z+6],bufferI[z+7],bufferI[z+8]);
		PrintMessage1("\r\nCRC: 0x%02X\r\n",bufferI[z+9]);
	}
//****************** write ********************
	PrintMessage(strings[S_EEAreaW]);	//"Write EEPROM ... "
	int page=options==0?8:32;
	for(i=0,j=0;i<dim;i+=page){
		bufferU[j++]=OW_RESET;
		bufferU[j++]=OW_WRITE;
		bufferU[j++]=page+(dim<=32?3:4);
		bufferU[j++]=SKIP_ROM;
		bufferU[j++]=WRITE_SCRATCHPAD;
		bufferU[j++]=i&0xFF;
		if(dim>32) bufferU[j++]=i>>8;
		for(k=0;k<page;k++) bufferU[j++]=memEE[i+k];
		bufferU[j++]=OW_RESET;
		bufferU[j++]=OW_WRITE;
		bufferU[j++]=dim<=32?3:5;
		bufferU[j++]=SKIP_ROM;
		bufferU[j++]=COPY_SCRATCHPAD;
		if(dim<=32) bufferU[j++]=0xA5;
		else{
			bufferU[j++]=i&0xFF;
			bufferU[j++]=i>>8;
			bufferU[j++]=page-1;
		}
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(6+0.8*page);
		msDelay(10); //TPROG
		j=0;
		PrintStatus(strings[S_CodeWriting2],i*100/(dim),i);	//"Write: %d%%, addr. %04X"
		if(RWstop) i=dim;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
		}
	}
	PrintStatusEnd();
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** verify EEPROM ********************
	PrintMessage(strings[S_EEV]);	//"Verify EEPROM ... "
	if(saveLog)	fprintf(logfile,"%s\n",strings[S_EEV]);	//"Verify EEPROM ... "
	PrintStatusSetup();
	k=0;
	bufferU[j++]=OW_RESET;
	bufferU[j++]=OW_WRITE;
	bufferU[j++]=1;
	bufferU[j++]=SKIP_ROM;
	if(dim<=32){	//1 byte address
		bufferU[j++]=OW_WRITE;
		bufferU[j++]=2;
		bufferU[j++]=READ_MEMORY;
		bufferU[j++]=0;	//address
	}
	else{			//2 byte address
		bufferU[j++]=OW_WRITE;
		bufferU[j++]=3;
		bufferU[j++]=READ_MEMORY;
		bufferU[j++]=0;	//address
		bufferU[j++]=0;	//address
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(10);
	j=0;
	for(i=0,j=0;i<dim;i+=DIMBUF-4){
		bufferU[j++]=OW_READ;
		bufferU[j++]=i<dim-(DIMBUF-4)?DIMBUF-4:dim-i;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(37);
		for(j=0;bufferI[j]!=OW_READ&&j<DIMBUF;j++);
		if(bufferI[j]==OW_READ&&bufferI[j+1]<0xFA){
			for(z=j+2;z<j+2+bufferI[j+1]&&z<DIMBUF;z++){
				if(memEE[k++]!=bufferI[z]){
					PrintMessage("\r\n");
					PrintMessage4(strings[S_CodeVError],i+z-(j+2),i+z-(j+2),memEE[k-1],bufferI[z]);	//"Error verifying address %04X (%d), written %02X, read %02X\r\n"
					err++;
				}
			}
		}
		PrintStatus(strings[S_CodeV2],i*100/(dim),i);	//"Verify: %d%%, addr. %04X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, err=%d\n"
		}
		if(err>=max_err) break;
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadEEErr],dim,k);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
		sizeEE=k;
	}
	PrintMessage1(strings[S_ComplErr],err);	//"completed: %d errors\r\n"
//****************** exit ********************
	bufferU[j++]=EN_VPP_VCC;		//turn off
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

#define READ_SCRATCHPAD2 0xBE
#define CONVERT_TEMP 0x44
#define RECALL_EE 0xB8
#define READ_PWSUP 0xB4

void ReadDS1820()
// read DS1820 one-wire digital thermometer
{
	int z=0,j;
	double TLSB=0.5;
	hvreg=0;
	if(FWVersion<0x800){
		PrintMessage1(strings[S_FWver2old],"0.8.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"ReadDS1820()\n");
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=VREG_DIS;
	bufferU[j++]=uW_INIT;	//set RB1=0 to use as GND terminal beside RB0
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=OW_RESET;
	bufferU[j++]=OW_WRITE;
	bufferU[j++]=1;
	bufferU[j++]=READ_ROM;
	bufferU[j++]=OW_READ;
	bufferU[j++]=8;
	bufferU[j++]=OW_RESET;
	bufferU[j++]=OW_WRITE;
	bufferU[j++]=2;
	bufferU[j++]=SKIP_ROM;
	bufferU[j++]=CONVERT_TEMP;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(16);
	j=0;
	for(z=0;bufferI[z]!=OW_RESET&&z<DIMBUF;z++);
	if(bufferI[z]==OW_RESET&&bufferI[z+1]==0){	//no presence pulse
		PrintMessage(strings[S_ComErr]);		//communication error
		bufferU[j++]=EN_VPP_VCC;		//turn off
		bufferU[j++]=0x0;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		if(saveLog) CloseLogFile();
		return;
	}
	for(z=0;bufferI[z]!=OW_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-9){
		PrintMessage1("Family code: 0x%02X ",bufferI[z+2]);
		OW_ID(bufferI[z+2]);
		if(bufferI[z+2]==0x10) TLSB=0.5;			//DS1820
		else if(bufferI[z+2]==0x28) TLSB=0.0625;	//DS18B20
		PrintMessage3("Serial ID: 0x%02X%02X%02X",bufferI[z+3],bufferI[z+4],bufferI[z+5]);
		PrintMessage3("%02X%02X%02X",bufferI[z+6],bufferI[z+7],bufferI[z+8]);
		PrintMessage1("\r\nCRC: 0x%02X\r\n",bufferI[z+9]);
	}
//****************** read ********************
	msDelay(800);
	bufferU[j++]=OW_RESET;
	bufferU[j++]=OW_WRITE;
	bufferU[j++]=2;
	bufferU[j++]=SKIP_ROM;
	bufferU[j++]=READ_SCRATCHPAD2;
	bufferU[j++]=OW_READ;
	bufferU[j++]=8;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(10);
	j=0;
	for(z=0;bufferI[z]!=OW_READ&&z<DIMBUF-2;z++);
	//PrintMessage("0x");
	//for(i=z+2;i<z+10&&i<DIMBUF;i++) PrintMessage1("%02X",bufferI[i]);
	//PrintMessage("\r\n");
	int T=bufferI[z+2]+(bufferI[z+3]<<8);
	if(T>0xF000) T|=0xFFFF0000; //adjust negative value
#ifdef _MSC_VER
	PrintMessage2("T=%.4f°C  (0x%04X)\r\n",T*TLSB,T);
#else
	PrintMessage2("T=%.4fÂ°C  (0x%04X)\r\n",T*TLSB,T);
#endif
//****************** exit ********************
	bufferU[j++]=EN_VPP_VCC;		//turn off
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}


#define UNIO_READ 0x03
#define UNIO_CRRD 0x06
#define UNIO_WRITE 0x6C
#define UNIO_WREN 0x96
#define UNIO_WRDI 0x91
#define UNIO_RDSR 0x05
#define UNIO_WRSR 0x6E
#define UNIO_ERAL 0x6D
#define UNIO_SETAL 0x67

void Read11xx(int dim)
// read 11xx UNIO memories
// dim=size in bytes
{
	int k=0,z=0,i,j;
	hvreg=0;
	if(FWVersion<0x800){
		PrintMessage1(strings[S_FWver2old],"0.8.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(dim>=0x10000||dim<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"Read11xx(%d)    (0x%X)\n",dim,dim);
	}
	sizeEE=dim;
	if(memEE) free(memEE);
	memEE=(unsigned char*)malloc(dim);			//EEPROM
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=VREG_DIS;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=UNIO_COM;
	bufferU[j++]=2;		//write x bytes
	bufferU[j++]=1;		//read x bytes
	bufferU[j++]=0xA0;
	bufferU[j++]=UNIO_RDSR;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
//****************** read ********************
	PrintMessage(strings[S_ReadEE]);		//read EEPROM ...
	PrintStatusSetup();
	for(i=0,j=0;i<dim;i+=DIMBUF-5){
		bufferU[j++]=UNIO_COM;
		bufferU[j++]=4;		//write x bytes
		bufferU[j++]=i<dim-(DIMBUF-5)?DIMBUF-5:dim-i;		//read x bytes
		bufferU[j++]=0xA0;
		bufferU[j++]=UNIO_READ;
		bufferU[j++]=i>>8;
		bufferU[j++]=i&0xFF;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(14);
		for(j=0;j<DIMBUF-1&&bufferI[j]!=UNIO_COM;j++);
		if(bufferI[j]==UNIO_COM&&bufferI[j+1]<0xFA){
			for(z=j+2;z<j+2+bufferI[j+1]&&z<DIMBUF;z++) memEE[k++]=bufferI[z];
		}
		PrintStatus(strings[S_CodeReading2],i*100/(dim),i);	//"Read: %d%%, addr. %05X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X) \n"
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadEEErr],dim,k);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
		sizeEE=k;
	}
	else PrintMessage(strings[S_Compl]);
//****************** exit ********************
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
	DisplayEE();	//visualize
	int sum=0;
	for(i=0;i<sizeEE;i++) sum+=memEE[i];
	PrintMessage1("Checksum: 0x%X\r\n",sum&0xFFFF);
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

void Write11xx(int dim,int page)
// write 11xx UNIO memories
// dim=size in bytes
// page=page size
{
	int k=0,z=0,i,j;
	int err=0;
	if(FWVersion<0x800){
		PrintMessage1(strings[S_FWver2old],"0.8.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	hvreg=0;
	if(dim>=0x10000||dim<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"Write11xx(%d,%d)    (0x%X,0x%X)\n",dim,page,dim,page);
	}
	if(dim>sizeEE){
		i=sizeEE;
		memEE=(unsigned char*)realloc(memEE,dim);
		for(;i<dim;i++) memEE[i]=0xFF;
		sizeEE=dim;
	}
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Data area is empty\r\n"
		return;
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=VREG_DIS;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=UNIO_STBY;
	bufferU[j++]=UNIO_COM;
	bufferU[j++]=2;		//write x bytes
	bufferU[j++]=0;		//read x bytes
	bufferU[j++]=0xA0;
	bufferU[j++]=UNIO_WREN;
	bufferU[j++]=UNIO_COM;
	bufferU[j++]=2;		//write x bytes
	bufferU[j++]=1;		//read x bytes
	bufferU[j++]=0xA0;
	bufferU[j++]=UNIO_RDSR;
	bufferU[j++]=UNIO_COM;
	bufferU[j++]=3;		//write x bytes
	bufferU[j++]=0;		//read x bytes
	bufferU[j++]=0xA0;
	bufferU[j++]=UNIO_WRSR;
	bufferU[j++]=0;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=UNIO_COM;
	bufferU[j++]=2;		//write x bytes
	bufferU[j++]=1;		//read x bytes
	bufferU[j++]=0xA0;
	bufferU[j++]=UNIO_RDSR;
	bufferU[j++]=UNIO_COM;
	bufferU[j++]=2;		//write x bytes
	bufferU[j++]=0;		//read x bytes
	bufferU[j++]=0xA0;
	bufferU[j++]=UNIO_WREN;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(25);
//****************** write ********************
	PrintMessage(strings[S_EEAreaW]);	//"Write EEPROM ... "
	PrintStatusSetup();
	for(;page>=DIMBUF-8;page>>=1);
	for(i=0,j=0;i<dim;i+=page){
		bufferU[j++]=UNIO_COM;
		bufferU[j++]=4+page;	//write x bytes
		bufferU[j++]=0;			//read x bytes
		bufferU[j++]=0xA0;
		bufferU[j++]=UNIO_WRITE;
		bufferU[j++]=i>>8;
		bufferU[j++]=i&0xFF;
		for(k=0;k<page;k++) bufferU[j++]=memEE[i+k];
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO((5+page)*0.2+2);
		for(j=0;j<DIMBUF-1&&bufferI[j]!=UNIO_COM;j++);
		if(bufferI[j]!=UNIO_COM||bufferI[j+1]>=0xFA) i=dim+10;
		PrintStatus(strings[S_CodeWriting2],i*100/(dim),i);	//"Write: %d%%, addr. %04X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
		}
		bufferU[j++]=UNIO_COM;
		bufferU[j++]=2;		//write x bytes
		bufferU[j++]=1;		//read x bytes
		bufferU[j++]=0xA0;
		bufferU[j++]=UNIO_RDSR;
		bufferU[j++]=UNIO_COM;
		bufferU[j++]=2;		//write x bytes
		bufferU[j++]=0;		//read x bytes
		bufferU[j++]=0xA0;
		bufferU[j++]=UNIO_WREN;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		int status=1;
		for(k=0;status&&k<20;k++){	//status polling until write complete
			PacketIO(2);
			for(j=0;j<DIMBUF-1&&bufferI[j]!=UNIO_COM;j++);
			if(bufferI[j]==UNIO_COM) status=bufferI[j+2]&1;
		}
		j=0;
	}
	PrintStatusEnd();
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** verify EEPROM ********************
	PrintMessage(strings[S_EEV]);	//"Verify EEPROM ... "
	PrintStatusSetup();
	k=0;
	for(i=0,j=0;i<dim;i+=DIMBUF-4){
		bufferU[j++]=UNIO_COM;
		bufferU[j++]=4;		//write x bytes
		bufferU[j++]=i<dim-(DIMBUF-4)?DIMBUF-4:dim-i;		//read x bytes
		bufferU[j++]=0xA0;
		bufferU[j++]=UNIO_READ;
		bufferU[j++]=i>>8;
		bufferU[j++]=i&0xFF;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(14);
		for(j=0;j<DIMBUF-1&&bufferI[j]!=UNIO_COM;j++);
		if(bufferI[j]==UNIO_COM&&bufferI[j+1]<0xFA){
			for(z=j+2;z<j+2+bufferI[j+1]&&z<DIMBUF;z++){
				if(memEE[k++]!=bufferI[z]){
					PrintMessage("\r\n");
					PrintMessage4(strings[S_CodeVError],i+z-3,i+z-3,memEE[k-1],bufferI[z]);	//"Error verifying address %04X (%d), written %02X, read %02X\r\n"
					err++;
				}
			}
		}
		PrintStatus(strings[S_CodeV2],i*100/(dim),i);	//"Verify: %d%%, addr. %04X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, err=%d\n"
		}
		if(err>=max_err) break;
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadEEErr],dim,k);	//"Error reading EEPROM area, requested %d bytes, read %d\r\n"
	}
	PrintMessage1(strings[S_ComplErr],err);	//"completed: %d errors\r\n"
//****************** exit ********************
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}
