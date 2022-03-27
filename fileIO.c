/*
 * fileIO.c - file read and write
 * Copyright (C) 2010-2021 Alberto Maccioni
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
#include "progP12.h"
#include "progP16.h"
#include "progP18.h"
#include "progP24.h"
#include "progAVR.h"

unsigned int htoi(const char *hex, int length)
{
	int i;
	unsigned int v = 0;
	for (i = 0; i < length; i++) {
		v <<= 4;
		if (hex[i] >= '0' && hex[i] <= '9') v += hex[i] - '0';
		else if (hex[i] >= 'a' && hex[i] <= 'f') v += hex[i] - 'a' + 10;
		else if (hex[i] >= 'A' && hex[i] <= 'F') v += hex[i] - 'A' + 10;
		else PrintMessage1(strings[S_Inohex],hex);	//"Error: '%.4s' doesn't look very hexadecimal, right?\n"
	}
	return v;
}

void Save(char* dev,char* savefile)
{
	FILE* f=fopen(savefile,"w");
	if(!f) return;
	char str[512],str1[512]="";
	int i,sum=0,count=0,ext=0,s,base;
//**************** 10-16F *******************************************
	if(!strncmp(dev,"10",2)||!strncmp(dev,"12",2)||!strncmp(dev,"16",2)){
		int x=0x3fff,addr;
		//if(!strncmp(dev,"16",2)||!strncmp(dev,"12F6",4)) x=0x3fff;
		fprintf(f,":020000040000FA\n");			//extended address=0
		for(i=0;i<sizeW;i++) memCODE_W[i]&=x;
		for(i=0;i<sizeW&&memCODE_W[i]>=x;i++); //remove leading 0xFFF
		for(;i<sizeW;i++){
			sum+=(memCODE_W[i]>>8)+(memCODE_W[i]&0xff);
			sprintf(str,"%02X%02X",memCODE_W[i]&0xff,memCODE_W[i]>>8);
			strcat(str1,str);
			count++;
			if(count==8||i==sizeW-1){
				base=i-count+1;
				for(s=i;s>=base&&memCODE_W[s]>=x;s--){	//remove trailing 0xFFF
					sum-=(memCODE_W[s]>>8)+(memCODE_W[s]&0xff);
					str1[strlen(str1)-4]=0;
				}
				count-=i-s;
				addr=(s-count+1)*2;
				sum+=count*2+(addr&0xff)+(addr>>8);
				if(base>>15>ext){
					ext=base>>15;
					fprintf(f,":02000004%04X%02X\n",ext,(-6-ext)&0xff);
				}
				if(count) fprintf(f,":%02X%04X00%s%02X\n",count*2,addr&0xFFFF,str1,(-sum)&0xff);
				str1[0]=0;
				count=sum=0;
			}
		}
		if(sizeEE){		//this is only for 16F1xxx
			if(ext!=0x01) fprintf(f,":020000040001F9\n");
			for(i=0,count=sum=0;i<sizeEE;i++){
				sum+=memEE[i];
				sprintf(str,"%02X00",memEE[i]&0xff);
				strcat(str1,str);
				count++;
				if(count==8||i==sizeEE-1){
					for(s=i;s>i-count&&memEE[s]>=0xff;s--){	//remove trailing 0xFF
						sum-=memEE[s]&0xff;
						str1[strlen(str1)-4]=0;
					}
					count-=i-s;
					addr=(s-count+1)*2+0xE000;
					sum+=count*2+(addr&0xff)+(addr>>8);
					if(count){
						fprintf(f,":%02X%04X00%s%02X\n",count*2,addr,str1,(-sum)&0xff);
					}
					str1[0]=0;
					count=sum=0;
				}
			}
		}
		fprintf(f,":00000001FF\n");
	}
//**************** 18F *******************************************
	else if(!strncmp(dev,"18F",3)){
		fprintf(f,":020000040000FA\n");			//extended address=0
		for(i=0;i<size&&memCODE[i]==0xff;i++); //remove leading 0xFF
		for(;i<size;i++){
			sum+=memCODE[i];
			sprintf(str,"%02X",memCODE[i]);
			strcat(str1,str);
			count++;
			if(count==16||i==size-1){
				base=i-count+1;
				for(s=i;s>=base&&memCODE[s]==0xff;s--){	//remove trailing 0xFF
					sum-=memCODE[s];
					str1[strlen(str1)-2]=0;
				}
				count-=i-s;
				sum+=count+(base&0xff)+((base>>8)&0xff);
				if(base>>16>ext){
					ext=base>>16;
					fprintf(f,":02000004%04X%02X\n",ext,(-6-ext)&0xff);
				}
				if(count){
					fprintf(f,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
				}
				str1[0]=0;
				count=sum=0;
			}
		}
		for(i=0,count=sum=0;i<8;i++){
			sum+=memID[i];
			sprintf(str,"%02X",memID[i]&0xff);
			strcat(str1,str);
			count++;
			if(count==8){
				fprintf(f,":020000040020DA\n");
					base=i-count+1;
				for(s=i;s>i-count&&memID[s]>=0xff;s--){	//remove trailing 0xFF
					sum-=memID[s]&0xff;
					str1[strlen(str1)-2]=0;
				}
				count-=i-s;
				sum+=count+(base&0xff)+((base>>8)&0xff);
				if(count){
					fprintf(f,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
				}
				str1[0]=0;
				count=sum=0;
			}
		}
		for(i=0,count=sum=0;i<14;i++){
			sum+=memCONFIG[i];
			sprintf(str,"%02X",memCONFIG[i]&0xff);
			strcat(str1,str);
			count++;
			if(count==14){
				fprintf(f,":020000040030CA\n");
				base=i-count+1;
				for(s=i;s>i-count&&memCONFIG[s]>=0xff;s--){	//remove trailing 0xFF
					sum-=memCONFIG[s]&0xff;
					str1[strlen(str1)-2]=0;
				}
				count-=i-s;
				sum+=count+(base&0xff)+((base>>8)&0xff);
				if(count){
					fprintf(f,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
				}
				str1[0]=0;
				count=sum=0;
			}
		}
		if(sizeEE){
			fprintf(f,":0200000400F00A\n");
			for(i=0,count=sum=0;i<sizeEE;i++){
				sum+=memEE[i];
				sprintf(str,"%02X",memEE[i]&0xff);
				strcat(str1,str);
				count++;
				if(count==16||i==sizeEE-1){
					base=i-count+1;
					for(s=i;s>i-count&&memEE[s]>=0xff;s--){	//remove trailing 0xFF
						sum-=memEE[s]&0xff;
						str1[strlen(str1)-2]=0;
					}
					count-=i-s;
					sum+=count+(base&0xff)+((base>>8)&0xff);
					if(count){
						fprintf(f,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
					}
					str1[0]=0;
					count=sum=0;
				}
			}
		}
		fprintf(f,":00000001FF\n");
	}
//**************** 24F *******************************************
	else if((!strncmp(dev,"24F",3)||!strncmp(dev,"24H",3)||!strncmp(dev,"24E",3)||!strncmp(dev,"30F",3)||!strncmp(dev,"33F",3)||!strncmp(dev,"33E",3))){
		int valid;
		fprintf(f,":020000040000FA\n");			//extended address=0
		int sum=0,count=0,s,word;
		word=memCODE[0]+(memCODE[1]<<8)+(memCODE[2]<<16)+(memCODE[3]<<24);
		for(i=0;i<size&&word==0xffffffff;i+=4) //remove leading 0xFFFFFFFF
			word=memCODE[i]+(memCODE[i+1]<<8)+(memCODE[i+2]<<16)+(memCODE[i+3]<<24);
		for(;i<size;i++){
			sum+=memCODE[i];
			sprintf(str,"%02X",memCODE[i]);
			strcat(str1,str);
			count++;
			if(count==16||i==size-1){
				base=i-count+1;
				for(s=base,valid=0;s<=i&&!valid;s+=4){	//remove empty lines
					if(memCODE[s]<0xFF||memCODE[s+1]<0xFF||+memCODE[s+2]<0xFF) valid=1;
				}
				sum+=count+(base&0xff)+((base>>8)&0xff);
				if(base>>16>ext){
					ext=base>>16;
					fprintf(f,":02000004%04X%02X\n",ext,(-6-ext)&0xff);
				}
				if(count&&valid){
					fprintf(f,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
				}
				str1[0]=0;
				count=sum=0;
			}
		}
		if(sizeCONFIG){
			fprintf(f,":0200000401F009\n");
			for(i=0,count=sum=0;i<sizeCONFIG&&i<48;i++){
				sum+=memCONFIG[i];
				sprintf(str,"%02X",memCONFIG[i]);
				strcat(str1,str);
				count++;
				if(count==4||i==sizeCONFIG-1){
					base=i-count+1;
					sum+=count+(base&0xff)+((base>>8)&0xff);
					if(count){
						fprintf(f,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
					}
					str1[0]=0;
					count=sum=0;
				}
			}
		}
		if(sizeEE){
			fprintf(f,":0200000400FFFB\n");
			str1[0]=0;
			for(i=0,count=sum=0;i<sizeEE;i+=2){		//append 0000 every 2 bytes
				sum+=memEE[i]+memEE[i+1];
				sprintf(str,"%02X%02X0000",memEE[i]&0xff,memEE[i+1]&0xff);
				strcat(str1,str);
				count+=4;
				if(count==16||i==sizeEE-2){
					base=2*i-count+4;
					for(s=base/2,valid=0;s<=i&&!valid;s+=2){	//remove empty lines
						if(memEE[s]<0xFF||memEE[s+1]<0xFF) valid=1;
					}
					sum+=0xE0+count+(base&0xff)+(base>>8);
					if(count&&valid){
						fprintf(f,":%02X%04X00%s%02X\n",count,base+0xE000,str1,(-sum)&0xff);
					}
					str1[0]=0;
					count=sum=0;
				}
			}
		}
		fprintf(f,":00000001FF\n");
	}
//**************** ATxxxx *******************************************
	else if(!strncmp(dev,"AT",2)){
		fprintf(f,":020000040000FA\n");			//extended address=0
		for(i=0;i<size&&memCODE[i]==0xff;i++); //remove leading 0xFF
		for(;i<size;i++){
			sum+=memCODE[i];
			sprintf(str,"%02X",memCODE[i]);
			strcat(str1,str);
			count++;
			if(count==16||i==size-1){
				base=i-count+1;
				for(s=i;s>=base&&memCODE[s]==0xff;s--){	//remove trailing 0xFF
					sum-=memCODE[s];
					str1[strlen(str1)-2]=0;
				}
				count-=i-s;
				sum+=count+(base&0xff)+((base>>8)&0xff);
				if(base>>16>ext){
					ext=base>>16;
					fprintf(f,":02000004%04X%02X\n",ext,(-6-ext)&0xff);
				}
				if(count){
					fprintf(f,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
				}
				str1[0]=0;
				count=sum=0;
			}
		}
		fprintf(f,":00000001FF\n");
	}
//**************** 24xxx / 93xxx / 25xxx / 95xxx / DSxxxx *******************************************
	else if(!strncmp(dev,"24",2)||!strncmp(dev,"93",2)||!strncmp(dev,"25",2)||!strncmp(dev,"95",2)||\
		!strncmp(dev,"DS",2)||!strncmp(dev,"11",2)){
		if(strstr(savefile,".bin")||strstr(savefile,".BIN")){
			#ifdef _WIN32
			//brain-damaged op. systems need this to avoid messing with some bytes
			f=freopen(savefile,"wb",f);
			if(!f) return;
			#endif
			fwrite(memEE,1,sizeEE,f);
		}
		else{			//HEX
			int valid;
			fprintf(f,":020000040000FA\n");			//extended address=0
			for(i=0;i<sizeEE;i++){
				sum+=memEE[i];
				sprintf(str,"%02X",memEE[i]);
				strcat(str1,str);
				count++;
				if(count==16||i==sizeEE-1){
					for(s=valid=0;str1[s]&&!valid;s++) if(str1[s]!='F') valid=1;
					if(valid){
						base=i-count+1;
						sum+=count+(base&0xff)+((base>>8)&0xff);
						if(base>>16>ext){
							ext=base>>16;
							fprintf(f,":02000004%04X%02X\n",ext,(-6-ext)&0xff);
						}
						if(count){
							fprintf(f,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
						}
					}
					str1[0]=0;
					count=sum=0;
				}
			}
			fprintf(f,":00000001FF\n");
		}
	}
	if(f) fclose(f);
}

void SaveEE(char* dev,char* savefile){
	FILE* f=fopen(savefile,"w");
	if(!f) return;
//**************** ATMEL *******************************************
	if(!strncmp(dev,"AT",2)){
		char str[512],str1[512]="";
		int i,base;
		fprintf(f,":020000040000FA\n");			//extended address=0
		int sum=0,count=0,s;
		for(i=0,count=sum=0;i<sizeEE;i++){
			sum+=memEE[i];
			sprintf(str,"%02X",memEE[i]&0xff);
			strcat(str1,str);
			count++;
			if(count==16||i==sizeEE-1){
				base=i-count+1;
				for(s=i;s>i-count&&memEE[s]>=0xff;s--){	//remove trailing 0xFF
					sum-=memEE[s]&0xff;
					str1[strlen(str1)-2]=0;
				}
				count-=i-s;
				sum+=count+(base&0xff)+((base>>8)&0xff);
				if(count){
					fprintf(f,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
				}
				str1[0]=0;
				count=sum=0;
			}
		}
		fprintf(f,":00000001FF\n");
	}
	if(f) fclose(f);
}

int Load(char*dev,char*loadfile){
	int i,input_address=0,ext_addr=0,sum,valid;
	char line[256];
	FILE* f=fopen(loadfile,"r");
	if(!f) return -1;
	PrintMessage1("%s :\r\n",loadfile);
//**************** 10-16F *******************************************
	if(!strncmp(dev,"10",2)||!strncmp(dev,"12",2)||!strncmp(dev,"16",2)){
		unsigned char buffer[0x20000],bufferEE[0x1000];
		int sizeM=0;
		memset(buffer,0xFF,sizeof(buffer));
		memset(bufferEE,0xFF,sizeof(bufferEE));
		sizeEE=0;
		for(;fgets(line,256,f);){
			if(strlen(line)>9&&line[0]==':'){
				int hex_count = htoi(line+1, 2);
					if((int)strlen(line)-11<hex_count*2) {
						PrintMessage1(strings[S_IhexShort],line);	//"Intel hex8 line too short:\r\n%s\r\n"
				}
				else{
					input_address=(ext_addr<<16)+htoi(line+3,4);
					sum=0;
					for (i=1;i<=hex_count*2+9;i+=2) sum += htoi(line+i,2);
					if ((sum & 0xff)!=0) {
							PrintMessage1(strings[S_IhexChecksum],line);	//"Intel hex8 checksum error in line:\r\n%s\r\n"
					}
					else{
						switch(htoi(line+7,2)){
							case 0:		//Data record
								if(input_address<0x1E000){		//Code
									sizeM=input_address+hex_count;
									if(sizeM>sizeW) sizeW=sizeM;
									for (i=0;i<hex_count;i++){
										buffer[input_address+i]=htoi(line+9+i*2,2);
									}
								}
								else if(input_address>=0x1E000&&input_address<0x1F000){	//EEPROM
									sizeM=(input_address-0x1E000+hex_count)/2;
									if(sizeM>sizeEE) sizeEE=sizeM;
									for (i=0;i<hex_count;i+=2){
										bufferEE[(input_address-0x1E000)/2+i/2]=htoi(line+9+i*2,2);
									}
								}
								break;
							case 4:		//extended linear address record
								if(strlen(line)>14)	ext_addr=htoi(line+9,4);
								break;
							default:
								break;
						}
					}
				}
			}
		}
		sizeW/=2;
		if(memCODE_W) free(memCODE_W);
		memCODE_W=(WORD*)malloc(sizeof(WORD)*sizeW);
		for(i=0;i<sizeW;i++){		//Swap bytes
			memCODE_W[i]=(buffer[i*2+1]<<8)+buffer[i*2];
		}
		if(memEE) free(memEE);
		if(sizeEE){
			memEE=(unsigned char*)malloc(sizeEE);
			memcpy(memEE,bufferEE,sizeEE);
		}
		else memEE=0;
		PrintMessage(strings[S_CodeMem]);	//"\r\nCode memory:\r\n"
		int imax=sizeW>0x8000?0x8500:0x2100;
		DisplayCODE16F(imax);
		if(sizeW>=0x2100&&sizeW<0x3000){	//EEPROM@0x2100
			PrintMessage(strings[S_EEMem]);	//"\r\nEEPROM memory:\r\n"
			DisplayEE16F(0x700);
		}
		else if(sizeEE) DisplayEE();
		PrintMessage("\r\n");
	}
//**************** 18F *******************************************
	else if(!strncmp(dev,"18F",3)){
		unsigned char buffer[0x30000],bufferEE[0x1000];
		int sizeM;
		memset(buffer,0xFF,sizeof(buffer));
		memset(bufferEE,0xFF,sizeof(bufferEE));
		memset(memID,0xFF,sizeof(memID));
		memset(memCONFIG,0xFF,sizeof(memCONFIG));
		for(;fgets(line,256,f);){
			if(strlen(line)>9&&line[0]==':'){
				int hex_count = htoi(line+1, 2);
				if((int)strlen(line) - 11 < hex_count * 2) {
					PrintMessage1(strings[S_IhexShort],line);	//"Intel hex8 line too short:\r\n%s\r\n"
				}
				else{
					input_address=htoi(line+3,4);
					sum=0;
					for (i=1;i<=hex_count*2+9;i+=2)
						sum += htoi(line+i,2);
					if ((sum & 0xff)!=0) {
						PrintMessage1(strings[S_IhexChecksum],line);	//"Intel hex8 checksum error in line:\r\n%s\r\n"
					}
					else{
						switch(htoi(line+7,2)){
							case 0:		//Data record
								if(ext_addr<0x20){		//Code	<0x200000
									sizeM=(ext_addr<<16)+input_address+hex_count;
									if(sizeM>size) size=sizeM;
									for (i=0;i<hex_count;i++){
										buffer[(ext_addr<<16)+input_address+i]=htoi(line+9+i*2,2);
									}
								}
								else if(ext_addr==0x20&&input_address<64){	//ID: 0x200000
									for (i=0;i<hex_count;i++){
										memID[input_address+i]=htoi(line+9+i*2,2);
									}
								}
								else if(ext_addr==0x30&&input_address<14){	//CONFIG: 0x300000
									for (i=0;i<hex_count;i++){
										memCONFIG[input_address+i]=htoi(line+9+i*2,2);
									}
								}
								else if((ext_addr==0xF0||ext_addr==0x31||ext_addr==0x38)&&input_address<0x1000){	
								//EEPROM: 0xF00000, 0x310000, 0x380000
									for (i=0;i<hex_count;i++){
										bufferEE[input_address+i]=htoi(line+9+i*2,2);
									}
									sizeM=input_address+hex_count;
									if(sizeM>sizeEE) sizeEE=sizeM;

								}
								break;
							case 4:		//extended linear address record
								if(strlen(line)>14)	ext_addr=htoi(line+9,4);
								break;
							default:
								break;
						}
					}
				}
			}
		}
		if(memCODE) free(memCODE);
		memCODE=(unsigned char*)malloc(size);
		memcpy(memCODE,buffer,size);
		if(memEE) free(memEE);
		memEE=(unsigned char*)malloc(sizeEE);
		memcpy(memEE,bufferEE,sizeEE);
		PrintMessage(strings[S_IDMem]);	//"ID memory:\r\n"
		for(i=0;i<8;i+=2)	PrintMessage4("ID%d: 0x%02X   ID%d: 0x%02X\r\n",i,memID[i],i+1,memID[i+1]);
		PrintMessage(strings[S_ConfigMem]);	//"CONFIG memory:\r\n"
		for(i=0;i<7;i++){
			PrintMessage2(strings[S_ConfigWordH],i+1,memCONFIG[i*2+1]);	//"CONFIG%dH: 0x%02X\t"
			PrintMessage2(strings[S_ConfigWordL],i+1,memCONFIG[i*2]);	//"CONFIG%dL: 0x%02X\r\n"
		}
		PrintMessage(strings[S_CodeMem]);	//"\r\nCODE memory:\r\n"
		DisplayCODE18F(size);
		if(sizeEE) DisplayEE();
		PrintMessage("\r\n");
	}
//**************** 24F *******************************************
	else if(!strncmp(dev,"24F",3)||!strncmp(dev,"24H",3)||!strncmp(dev,"24E",3)||!strncmp(dev,"30F",3)||!strncmp(dev,"33F",3)||!strncmp(dev,"33E",3)){
		unsigned char *buffer,bufferEE[0x2000];
		int d;
		buffer=(unsigned char*)malloc(0x100000);
		memset(buffer,0xFF,0x100000);
		memset(bufferEE,0xFF,sizeof(bufferEE));
		memset(memCONFIG,0xFF,sizeof(memCONFIG));
		memset(memUSERID,0xFF,sizeof(memUSERID));
		sizeUSERID=0;
		for(;fgets(line,256,f);){
			if(strlen(line)>9&&line[0]==':'){
				int hex_count = htoi(line+1, 2);
				if((int)strlen(line) - 11 < hex_count * 2) {
						PrintMessage1(strings[S_IhexShort],line);	//"Intel hex8 line too short:\r\n%s\r\n"
				}
				else{
					input_address=htoi(line+3,4);
					sum=0;
					for (i=1;i<=hex_count*2+9;i+=2)
						sum += htoi(line+i,2);
					if ((sum & 0xff)!=0) {
							PrintMessage1(strings[S_IhexChecksum],line);	//"Intel hex8 checksum error in line:\r\n%s\r\n"
					}
					else{
						switch(htoi(line+7,2)){
							case 0:		//Data record
								if(ext_addr<0x20){		//Code
									int end1=(ext_addr<<16)+input_address+hex_count;
									if(size<end1) size=end1;
									for (i=0;i<hex_count;i++){
										buffer[(ext_addr<<16)+input_address+i]=htoi(line+9+i*2,2);
									}
								}
								else if(ext_addr==0x1F0&&input_address<48){	//CONFIG
									sizeCONFIG=input_address+hex_count;
									for (i=0;i<hex_count;i++){
										memCONFIG[input_address+i]=htoi(line+9+i*2,2);
									}
								}
								else if(ext_addr==0xFF&&input_address>=0xE000){	//EEPROM
									for (i=0;i<hex_count;i++){
										bufferEE[input_address-0xE000+i]=htoi(line+9+i*2,2);
									}
									sizeEE=input_address-0xE000+hex_count;
								}
								else if(ext_addr==0x100&&input_address<8){	//USER ID
									sizeUSERID=input_address+hex_count;
									for (i=0;i<hex_count&&(i+input_address)<8;i++){
										memUSERID[input_address+i]=htoi(line+9+i*2,2);
									}
								}
								break;
							case 4:		//extended linear address record
								if(strlen(line)>14)	ext_addr=htoi(line+9,4);
								break;
							default:
								break;
						}
					}
				}
			}
		}
		if(memCODE) free(memCODE);
		memCODE=(unsigned char*)malloc(size);
		memcpy(memCODE,buffer,size);
		free(buffer);
		sizeEE=sizeEE?0x1000:0;
		if(memEE) free(memEE);
		memEE=(unsigned char*)malloc(sizeEE);
		for(i=0;i<sizeEE;i+=2){		//skip voids in the hex file organization
			memEE[i]=bufferEE[i*2]; 	//0 1 4 5 8 9 12 13 ...
			memEE[i+1]=bufferEE[i*2+1];
		}
		for(i=valid=0;i<48;i++) if(memCONFIG[i]<0xFF) valid=1;
		if(valid){
			PrintMessage(strings[S_ConfigMem]);				//"\r\nCONFIG memory:\r\n"
			for(i=0;i<48;i+=4){
				d=(memCONFIG[i+1]<<8)+memCONFIG[i];
				if(i<36||d<0xFFFF)PrintMessage2("0xF800%02X: 0x%04X\r\n",i/2,d);
			}
		}
		if(size) PrintMessage(strings[S_CodeMem]);	//"\r\nCODE memory:\r\n"
		DisplayCODE24F(size);
		if(sizeEE){			//show eeprom with address offset by 0x7FF000
			PrintMessage(strings[S_EEMem]);	//"\r\nEEPROM memory:\r\n"
			DisplayEE24F();
		}
		PrintMessage("\r\n");
	}
//**************** ATxxxx *******************************************
	else if(!strncmp(dev,"AT",2)){
		unsigned char buffer[0x30000];
		memset(buffer,0xFF,sizeof(buffer));
		for(;fgets(line,256,f);){
			if(strlen(line)>9&&line[0]==':'){
				int hex_count = htoi(line+1, 2);
				if((int)strlen(line) - 11 < hex_count * 2) {
					PrintMessage1(strings[S_IhexShort],line);	//"Intel hex8 line too short:\r\n%s\r\n"
				}
				else{
					input_address=htoi(line+3,4);
					sum=0;
					for (i=1;i<=hex_count*2+9;i+=2)
						sum += htoi(line+i,2);
					if ((sum & 0xff)!=0) {
							PrintMessage1(strings[S_IhexChecksum],line);	//"Intel hex8 checksum error in line:\r\n%s\r\n"
					}
					else{
						switch(htoi(line+7,2)){
							case 0:		//Data record
								if(ext_addr<0x20){		//Code
									size=input_address+hex_count;
									for (i=0;i<hex_count;i++){
										buffer[(ext_addr<<16)+input_address+i]=htoi(line+9+i*2,2);
									}
								}
								break;
							case 4:		//extended linear address record
								if(strlen(line)>14)	ext_addr=htoi(line+9,4);
								break;
							default:
								break;
						}
					}
				}
			}
		}
		if(memCODE) free(memCODE);
		memCODE=(unsigned char*)malloc(size);
		memcpy(memCODE,buffer,size);
		if(size) PrintMessage(strings[S_CodeMem]);	//"\r\nmemoria CODICE:\r\n"
		DisplayCODEAVR(size);
		PrintMessage("\r\n");
	}
//**************** 24xxx / 93xxx / 25xxx / 95xxx / DSxxxx /11xxx *******************************
	else if(!strncmp(dev,"24",2)||!strncmp(dev,"93",2)||!strncmp(dev,"25",2)||!strncmp(dev,"95",2)||\
		!strncmp(dev,"DS",2)||!strncmp(dev,"11",2)){
		if(strstr(loadfile,".bin")||strstr(loadfile,".BIN")){
			#ifdef _WIN32
			//brain-damaged op. systems need this to avoid messing with some bytes
			f=freopen(loadfile,"rb",f);
			if(!f) return -1;
			#endif
			fseek(f, 0L, SEEK_END);
			sizeEE=ftell(f);
			fseek(f, 0L, SEEK_SET);
			if(sizeEE>0x1000000) sizeEE=0x1000000;	//max 16MB
			if(memEE) free(memEE);
			memEE=(unsigned char*)malloc(sizeEE);
			sizeEE=fread(memEE,1,sizeEE,f);
		}
		else{			//Hex file
			int bufSize=0x40000;	//256K
			unsigned char *bufferEE=(unsigned char*)malloc(bufSize);
			memset(bufferEE,0xFF,bufSize);
			for(;fgets(line,256,f);){
				if(strlen(line)>9&&line[0]==':'){
					int hex_count = htoi(line+1, 2);
					if((int)strlen(line) - 11 < hex_count * 2) {
							PrintMessage1(strings[S_IhexShort],line);	//"Intel hex8 line too short:\r\n%s\r\n"
					}
					else{
						input_address=htoi(line+3,4);
						sum=0;
						int end1;
						for (i=1;i<=hex_count*2+9;i+=2) sum+=htoi(line+i,2);
						if ((sum & 0xff)!=0) {
								PrintMessage1(strings[S_IhexChecksum],line);	//"Intel hex8 checksum error in line:\r\n%s\r\n"
						}
						else{
							switch(htoi(line+7,2)){
								case 0:		//Data record
									end1=(ext_addr<<16)+input_address+hex_count;
									if(end1>=0x1000000) break; //max 16MB
									if(sizeEE<end1){			//grow array
										sizeEE=end1;
									}
									if(bufSize<=end1){			//grow buffer
										int newsize=(end1&0xFFFC0000)+0x40000;
										bufferEE=(unsigned char*)realloc(bufferEE,newsize);
										memset(bufferEE+bufSize,0xFF,newsize-bufSize);
										bufSize=newsize;
									}
									for (i=0;i<hex_count;i++){
										bufferEE[(ext_addr<<16)+input_address+i]=htoi(line+9+i*2,2);
									}
									break;
								case 4:		//extended linear address record
									if(strlen(line)>14)	ext_addr=htoi(line+9,4);
									break;
								default:
									break;
							}
						}
					}
				}
			}
			if(memEE) free(memEE);
			memEE=(unsigned char*)malloc(sizeEE);
			memcpy(memEE,bufferEE,sizeEE);
			free(bufferEE);
		}
		DisplayEE();	//visualize
		int sum=0;
		for(i=0;i<sizeEE;i++) sum+=memEE[i];
		PrintMessage1("Checksum: 0x%X\r\n",sum&0xFFFF);
		PrintMessage("\r\n");
	}
	fclose(f);
	return 0;
}

void LoadEE(char*dev,char*loadfile){
	FILE* f=fopen(loadfile,"r");
	if(!f) return;
	int i;
//**************** ATMEL *******************************************
	if(!strncmp(dev,"AT",2)){
		char line[256];
		int input_address=0,ext_addr=0;
		unsigned char bufferEE[0x1000];
		PrintMessage1("%s :\r\n\r\n",loadfile);
		memset(bufferEE,0xFF,sizeof(bufferEE));
		for(;fgets(line,256,f);){
			if(strlen(line)>9&&line[0]==':'){
				int hex_count = htoi(line+1, 2);
				if((int)strlen(line) - 11 < hex_count * 2) {
					PrintMessage1(strings[S_IhexShort],line);	//"Intel hex8 line too short:\r\n%s\r\n"
				}
				else{
					input_address=htoi(line+3,4);
					int sum = 0;
					for (i=1;i<=hex_count*2+9;i+=2)	sum+=htoi(line+i,2);
					if ((sum & 0xff)!=0) {
						PrintMessage1(strings[S_IhexChecksum],line);	//"Intel hex8 checksum error in line:\r\n%s\r\n"
					}
					else{
						switch(htoi(line+7,2)){
							case 0:		//Data record
								if(ext_addr==0&&input_address<0x1000){	//EEPROM
									for (i=0;i<hex_count;i++){
										bufferEE[input_address+i]=htoi(line+9+i*2,2);
									}
									sizeEE=input_address+hex_count;
								}
								break;
							case 4:		//extended linear address record
								if(strlen(line)>14)	ext_addr=htoi(line+9,4);
								break;
							default:
								break;
						}
					}
				}
			}
		}
		if(memEE) free(memEE);
		memEE=(unsigned char*)malloc(sizeEE);
		memcpy(memEE,bufferEE,sizeEE);
		if(sizeEE) DisplayEE();	//visualize
		PrintMessage("\r\n");
		fclose(f);
	}
}

void OpenLogFile()
{
	logfile=fopen(LogFileName,"w");
	if(!logfile) return;
	fprintf(logfile,"%s version %s (%s)\n",appName,VERSION,SYSNAME);
	fprintf(logfile,"Firmware version %d.%d.%d\n",FWVersion>>16,(FWVersion>>8)&0xFF,FWVersion&0xFF);
	struct tm * timeinfo;
	time_t rawtime;
	time( &rawtime );                /* Get time as long integer. */
	timeinfo = localtime( &rawtime ); /* Convert to local time. */
	fprintf(logfile,"%s\n", asctime (timeinfo) );
}

void CloseLogFile()
{
	if(logfile)fclose(logfile);
	logfile=0;
}

void WriteLogIO()
{
	int i;
	//fprintf(logfile,"bufferU=[%02X\n",bufferU[0]);
	fprintf(logfile,"bufferU=[");
	for(i=0;i<DIMBUF;i++){
		if(i%32==0) fprintf(logfile,"\n");
		fprintf(logfile,"%02X ",bufferU[i]);
	}
	fprintf(logfile,"]\n");
	//fprintf(logfile,"bufferI=[%02X\n",bufferI[0]);
	fprintf(logfile,"bufferI=[");
		for(i=0;i<DIMBUF;i++){
		if(i%32==0) fprintf(logfile,"\n");
		fprintf(logfile,"%02X ",bufferI[i]);
	}
	fprintf(logfile,"]\n");
}
