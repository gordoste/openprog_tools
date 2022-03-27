/**
 * \file progP32.c - algorithms to program the PIC32 family of microcontrollers
 * Copyright (C) 2019 Alberto Maccioni
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

/*#ifdef _MSC_VER
void COpenProgDlg::CheckData(int a,int b,int addr,int *err)
#else
void CheckData(int a,int b,int addr,int *err)
#endif
{
	if(a!=b){
#ifdef _MSC_VER
		CString str;
#endif
		PrintMessage(strings[S_NL]);
		PrintMessage4(strings[S_CodeVError],addr,addr,a,b);	//"Error writing address %4X: written %02X, read %02X\r\n"
		(*err)++;
	}
}
*/
struct ID32{
	int id;
	char *device;
} PIC32LIST[]={
		{0x0938053,"PIC32MX360F512L"},
		{0x0934053,"PIC32MX360F256L"},
		{0x092D053,"PIC32MX340F128L"},
		{0x092A053,"PIC32MX320F128L"},
		{0x0916053,"PIC32MX340F512H"},
		{0x0912053,"PIC32MX340F256H"},
		{0x090D053,"PIC32MX340F128H"},
		{0x090A053,"PIC32MX320F128H"},
		{0x0906053,"PIC32MX320F064H"},
		{0x0902053,"PIC32MX320F032H"},
		{0x0978053,"PIC32MX460F512L"},
		{0x0974053,"PIC32MX460F256L"},
		{0x096D053,"PIC32MX440F128L"},
		{0x0952053,"PIC32MX440F256H"},
		{0x0956053,"PIC32MX440F512H"},
		{0x094D053,"PIC32MX440F128H"},
		{0x0942053,"PIC32MX420F032H"},
		{0x04317053,"PIC32MX575F256H"},
		{0x0430B053,"PIC32MX675F256H"},
		{0x04303053,"PIC32MX775F256H"},
		{0x04309053,"PIC32MX575F512H"},
		{0x0430C053,"PIC32MX675F512H"},
		{0x04325053,"PIC32MX695F512H"},
		{0x0430D053,"PIC32MX775F512H"},
		{0x0430E053,"PIC32MX795F512H"},
		{0x04333053,"PIC32MX575F256L"},
		{0x04305053,"PIC32MX675F256L"},
		{0x04312053,"PIC32MX775F256L"},
		{0x0430F053,"PIC32MX575F512L"},
		{0x04311053,"PIC32MX675F512L"},
		{0x04341053,"PIC32MX695F512L"},
		{0x04307053,"PIC32MX775F512L"},
		{0x04307053,"PIC32MX795F512L"},
		{0x04400053,"PIC32MX534F064H"},
		{0x04401053,"PIC32MX564F064H"},
		{0x04403053,"PIC32MX564F128H"},
		{0x04405053,"PIC32MX664F064H"},
		{0x04407053,"PIC32MX664F128H"},
		{0x0440B053,"PIC32MX764F128H"},
		{0x0440C053,"PIC32MX534F064L"},
		{0x0440D053,"PIC32MX564F064L"},
		{0x0440F053,"PIC32MX564F128L"},
		{0x04411053,"PIC32MX664F064L"},
		{0x04413053,"PIC32MX664F128L"},
		{0x04417053,"PIC32MX764F128L"},
		{0x04A07053,"PIC32MX110F016B"},
		{0x04A09053,"PIC32MX110F016C"},
		{0x04A0B053,"PIC32MX110F016D"},
		{0x04A01053,"PIC32MX210F016B"},
		{0x04A03053,"PIC32MX210F016C"},
		{0x04A05053,"PIC32MX210F016D"},
		{0x04A06053,"PIC32MX120F032B"},
		{0x04A08053,"PIC32MX120F032C"},
		{0x04A0A053,"PIC32MX120F032D"},
		{0x04A00053,"PIC32MX220F032B"},
		{0x04A02053,"PIC32MX220F032C"},
		{0x04A04053,"PIC32MX220F032D"},
		{0x04D07053,"PIC32MX130F064B"},
		{0x04D09053,"PIC32MX130F064C"},
		{0x04D0B053,"PIC32MX130F064D"},
		{0x04D01053,"PIC32MX230F064B"},
		{0x04D03053,"PIC32MX230F064C"},
		{0x04D05053,"PIC32MX230F064D"},
		{0x04D06053,"PIC32MX150F128B"},
		{0x04D08053,"PIC32MX150F128C"},
		{0x04D0A053,"PIC32MX150F128D"},
		{0x04D00053,"PIC32MX250F128B"},
		{0x04D02053,"PIC32MX250F128C"},
		{0x04D04053,"PIC32MX250F128D"},
		{0x06610053,"PIC32MX170F256B"},
		{0x0661A053,"PIC32MX170F256D"},
		{0x06600053,"PIC32MX270F256B"},
		{0x0660A053,"PIC32MX270F256D"},
		{0x0660C053,"PIC32MX270F256DB"},
		{0x06703053,"PIC32MX130F256B"},
		{0x06705053,"PIC32MX130F256D"},
		{0x06700053,"PIC32MX230F256B"},
		{0x06702053,"PIC32MX230F256D"},
		{0x05600053,"PIC32MX330F064H"},
		{0x05601053,"PIC32MX330F064L"},
		{0x05704053,"PIC32MX350F256H"},
		{0x05705053,"PIC32MX350F256L"},
		{0x05602053,"PIC32MX430F064H"},
		{0x05603053,"PIC32MX430F064L"},
		{0x05706053,"PIC32MX450F256H"},
		{0x05707053,"PIC32MX450F256L"},
		{0x0570C053,"PIC32MX350F128H"},
		{0x0570D053,"PIC32MX350F128L"},
		{0x0570E053,"PIC32MX450F128H"},
		{0x0570F053,"PIC32MX450F128L"},
		{0x05808053,"PIC32MX370F512H"},
		{0x05809053,"PIC32MX370F512L"},
		{0x0580A053,"PIC32MX470F512H"},
		{0x0580B053,"PIC32MX470F512L"},
		{0x05710053,"PIC32MX450F256HB"},
		{0x05811053,"PIC32MX470F512LB"},
		{0x05103053,"PIC32MZ1024ECG064"},
		{0x05108053,"PIC32MZ1024ECH064"},
		{0x05130053,"PIC32MZ1024ECM064"},
		{0x05104053,"PIC32MZ2048ECG064"},
		{0x05109053,"PIC32MZ2048ECH064"},
		{0x05131053,"PIC32MZ2048ECM064"},
		{0x0510D053,"PIC32MZ1024ECG100"},
		{0x05112053,"PIC32MZ1024ECH100"},
		{0x0513A053,"PIC32MZ1024ECM100"},
		{0x0510E053,"PIC32MZ2048ECG100"},
		{0x05113053,"PIC32MZ2048ECH100"},
		{0x0513B053,"PIC32MZ2048ECM100"},
		{0x05117053,"PIC32MZ1024ECG124"},
		{0x0511C053,"PIC32MZ1024ECH124"},
		{0x05144053,"PIC32MZ1024ECM124"},
		{0x05118053,"PIC32MZ2048ECG124"},
		{0x0511D053,"PIC32MZ2048ECH124"},
		{0x05145053,"PIC32MZ2048ECM124"},
		{0x05121053,"PIC32MZ1024ECG144"},
		{0x05126053,"PIC32MZ1024ECH144"},
		{0x0514E053,"PIC32MZ1024ECM144"},
		{0x05122053,"PIC32MZ2048ECG144"},
		{0x05127053,"PIC32MZ2048ECH144"},
		{0x0514F053,"PIC32MZ2048ECM144"},
		{0x06A10053,"PIC32MX150F256H"},
		{0x06A11053,"PIC32MX150F256L"},
		{0x06A30053,"PIC32MX170F512H"},
		{0x06A31053,"PIC32MX170F512L"},
		{0x06A12053,"PIC32MX250F256H"},
		{0x06A13053,"PIC32MX250F256L"},
		{0x06A32053,"PIC32MX270F512H"},
		{0x06A33053,"PIC32MX270F512L"},
		{0x06A14053,"PIC32MX550F256H"},
		{0x06A15053,"PIC32MX550F256L"},
		{0x06A34053,"PIC32MX570F512H"},
		{0x06A35053,"PIC32MX570F512L"},
		{0x06A50053,"PIC32MX120F064H"},
		{0x06A00053,"PIC32MX130F128H"},
		{0x06A01053,"PIC32MX130F128L"},
		{0x06A02053,"PIC32MX230F128H"},
		{0x06A03053,"PIC32MX230F128L"},
		{0x06A04053,"PIC32MX530F128H"},
		{0x06A05053,"PIC32MX530F128L"},
		{0x07201053,"PIC32MZ0512EFE064"},
		{0x07206053,"PIC32MZ0512EFF064"},
		{0x0722E053,"PIC32MZ0512EFK064"},
		{0x07202053,"PIC32MZ1024EFE064"},
		{0x07207053,"PIC32MZ1024EFF064"},
		{0x0722F053,"PIC32MZ1024EFK064"},
		{0x07203053,"PIC32MZ1024EFG064"},
		{0x07208053,"PIC32MZ1024EFH064"},
		{0x07230053,"PIC32MZ1024EFM064"},
		{0x07204053,"PIC32MZ2048EFG064"},
		{0x07209053,"PIC32MZ2048EFH064"},
		{0x07231053,"PIC32MZ2048EFM064"},
		{0x0720B053,"PIC32MZ0512EFE100"},
		{0x07210053,"PIC32MZ0512EFF100"},
		{0x07238053,"PIC32MZ0512EFK100"},
		{0x0720C053,"PIC32MZ1024EFE100"},
		{0x07211053,"PIC32MZ1024EFF100"},
		{0x07239053,"PIC32MZ1024EFK100"},
		{0x0720D053,"PIC32MZ1024EFG100"},
		{0x07212053,"PIC32MZ1024EFH100"},
		{0x0723A053,"PIC32MZ1024EFM100"},
		{0x0720E053,"PIC32MZ2048EFG100"},
		{0x07213053,"PIC32MZ2048EFH100"},
		{0x0723B053,"PIC32MZ2048EFM100"},
		{0x07215053,"PIC32MZ0512EFE124"},
		{0x0721A053,"PIC32MZ0512EFF124"},
		{0x07242053,"PIC32MZ0512EFK124"},
		{0x07216053,"PIC32MZ1024EFE124"},
		{0x0721B053,"PIC32MZ1024EFF124"},
		{0x07243053,"PIC32MZ1024EFK124"},
		{0x07217053,"PIC32MZ1024EFG124"},
		{0x0721C053,"PIC32MZ1024EFH124"},
		{0x07244053,"PIC32MZ1024EFM124"},
		{0x07218053,"PIC32MZ2048EFG124"},
		{0x0721D053,"PIC32MZ2048EFH124"},
		{0x07245053,"PIC32MZ2048EFM124"},
		{0x0721F053,"PIC32MZ0512EFE144"},
		{0x07224053,"PIC32MZ0512EFF144"},
		{0x0724C053,"PIC32MZ0512EFK144"},
		{0x07220053,"PIC32MZ1024EFE144"},
		{0x07225053,"PIC32MZ1024EFF144"},
		{0x0724D053,"PIC32MZ1024EFK144"},
		{0x07221053,"PIC32MZ1024EFG144"},
		{0x07226053,"PIC32MZ1024EFH144"},
		{0x0724E053,"PIC32MZ1024EFM144"},
		{0x07222053,"PIC32MZ2048EFG144"},
		{0x07227053,"PIC32MZ2048EFH144"},
		{0x0724F053,"PIC32MZ2048EFM144"},
		{0x05F0C053,"PIC32MZ1025DAA169"},
		{0x05F0D053,"PIC32MZ1025DAB169"},
		{0x05F0F053,"PIC32MZ1064DAA169"},
		{0x05F10053,"PIC32MZ1064DAB169"},
		{0x05F15053,"PIC32MZ2025DAA169"},
		{0x05F16053,"PIC32MZ2025DAB169"},
		{0x05F18053,"PIC32MZ2064DAA169"},
		{0x05F19053,"PIC32MZ2064DAB169"},
		{0x05F42053,"PIC32MZ1025DAG169"},
		{0x05F43053,"PIC32MZ1025DAH169"},
		{0x05F45053,"PIC32MZ1064DAG169"},
		{0x05F46053,"PIC32MZ1064DAH169"},
		{0x05F4B053,"PIC32MZ2025DAG169"},
		{0x05F4C053,"PIC32MZ2025DAH169"},
		{0x05F4E053,"PIC32MZ2064DAG169"},
		{0x05F4F053,"PIC32MZ2064DAH169"},
		{0x05F78053,"PIC32MZ1025DAA176"},
		{0x05F79053,"PIC32MZ1025DAB176"},
		{0x05F7B053,"PIC32MZ1064DAA176"},
		{0x05F7C053,"PIC32MZ1064DAB176"},
		{0x05F81053,"PIC32MZ2025DAA176"},
		{0x05F82053,"PIC32MZ2025DAB176"},
		{0x05F84053,"PIC32MZ2064DAA176"},
		{0x05F85053,"PIC32MZ2064DAB176"},
		{0x05FAE053,"PIC32MZ1025DAG176"},
		{0x05FAF053,"PIC32MZ1025DAH176"},
		{0x05FB1053,"PIC32MZ1064DAG176"},
		{0x05FB2053,"PIC32MZ1064DAH176"},
		{0x05FB7053,"PIC32MZ2025DAG176"},
		{0x05FB8053,"PIC32MZ2025DAH176"},
		{0x05FBA053,"PIC32MZ2064DAG176"},
		{0x05FBB053,"PIC32MZ2064DAH176"},
		{0x05F5D053,"PIC32MZ1025DAA288"},
		{0x05F5E053,"PIC32MZ1025DAB288"},
		{0x05F60053,"PIC32MZ1064DAA288"},
		{0x05F61053,"PIC32MZ1064DAB288"},
		{0x05F66053,"PIC32MZ2025DAA288"},
		{0x05F67053,"PIC32MZ2025DAB288"},
		{0x05F69053,"PIC32MZ2064DAA288"},
		{0x05F6A053,"PIC32MZ2064DAB288"},
		{0x07800053,"PIC32MX154F128B"},
		{0x07804053,"PIC32MX154F128D"},
		{0x07808053,"PIC32MX155F128B"},
		{0x0780C053,"PIC32MX155F128D"},
		{0x07801053,"PIC32MX174F256B"},
		{0x07805053,"PIC32MX174F256D"},
		{0x07809053,"PIC32MX175F256B"},
		{0x0780D053,"PIC32MX175F256D"},
		{0x07802053,"PIC32MX254F128B"},
		{0x07806053,"PIC32MX254F128D"},
		{0x0780A053,"PIC32MX255F128B"},
		{0x0780E053,"PIC32MX255F128D"},
		{0x07803053,"PIC32MX274F256B"},
		{0x07807053,"PIC32MX274F256D"},
		{0x0780B053,"PIC32MX275F256B"},
		{0x0780F053,"PIC32MX275F256D"},
		{0x06211053,"PIC32MK0512GPD064"},
		{0x0620E053,"PIC32MK1024GPD064"},
		{0x06210053,"PIC32MK0512GPD100"},
		{0x0620D053,"PIC32MK1024GPD100"},
		{0x0620B053,"PIC32MK0512GPE064"},
		{0x06208053,"PIC32MK1024GPE064"},
		{0x0620A053,"PIC32MK0512GPE100"},
		{0x06207053,"PIC32MK1024GPE100"},
		{0x06205053,"PIC32MK0512MCF064"},
		{0x06202053,"PIC32MK1024MCF064"},
		{0x06201053,"PIC32MK0512MCF100"},
		{0x06201053,"PIC32MK1024MCF100"},
};

void PIC32_ID(int id)
{
	char s[64];
	int i;
	for(i=0;i<sizeof(PIC32LIST)/sizeof(PIC32LIST[0]);i++){
		if((id&0x0FFFFFFF)==PIC32LIST[i].id){
			strcpy(s,PIC32LIST[i].device);
			strcat(s,"\r\n");
			PrintMessage(s);
			return;
		}
	}
	PrintMessage(strings[S_nodev]); //"Unknown device\r\n");
}

/*#ifdef _MSC_VER
void COpenProgDlg::DisplayCODE24F(int dim){
#else
void DisplayCODE24F(int dim){
#endif
// display 16 bit PIC CODE memory
	char s[256]="",t[256]="";
	char* aux=(char*)malloc((dim/COL+1)*2*(16+COL*9));
	aux[0]=0;
	int valid=0,empty=1,i,j,d,lines=0;
	for(i=0;i<dim&&i<size;i+=COL*2){
		valid=0;
		for(j=i;j<i+COL*2&&j<dim;j+=4){
			d=(memCODE[j+3]<<24)+(memCODE[j+2]<<16)+(memCODE[j+1]<<8)+memCODE[j];
			sprintf(t,"%08X ",d);
			strcat(s,t);
			if(d!=0xffffffff) valid=1;
		}
		if(valid){
			sprintf(t,"%06X: %s\r\n",i/2,s);
			empty=0;
			strcat(aux,t);
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

*/
/*
TDI=RB7   ->
TCK=RB6   ->
TDO=RB5   <-
TMS=RB4   ->
MCLR=RB3  ->
*/
//MCHP TAP INSTRUCTIONS (sent via JTAGSendCommand)
#define MTAP_COMMAND 0x07 	//TDI and TDO connected to MCHP Command Shift register
#define MTAP_SW_MTAP 0x04 	//Switch TAP controller to MCHP TAP controller
#define MTAP_SW_ETAP 0x05 	//Switch TAP controller to EJTAG TAP controller
#define MTAP_IDCODE 0x01 	//Select Chip Identification Data register
//MTAP_COMMAND DR COMMANDS (sent via xferData)
#define MCHP_STATUS 0x00 	//NOP and return Status.
#define MCHP_ASSERT_RST 0xD1 	//Requests the reset controller to assert device Reset.
#define MCHP_DE_ASSERT_RST 0xD0 //Removes the request for device Reset, which causes the reset controller
								//to deassert device Reset if there is no other source requesting Reset (i.e., MCLR).
#define MCHP_ERASE 0xFC 		//Cause the Flash controller to perform a Chip Erase.
#define MCHP_FLASH_ENABLE 0xFE 	//Enables fetches and loads to the Flash (from the processor).
#define MCHP_FLASH_DISABLE 0xFD //Disables fetches and loads to the Flash (from the processor).
//EJTAG TAP INSTRUCTIONS
#define ETAP_ADDRESS 0x08 	//Select Address register.
#define ETAP_DATA 0x09 		//Select Data register.
#define ETAP_CONTROL 0x0A 	//Select EJTAG Control register.
#define ETAP_EJTAGBOOT 0x0C	//Set EjtagBrk, ProbEn and ProbTrap to ‘1’ as the reset value.
#define ETAP_FASTDATA 0x0E 	//Selects the Data and Fastdata registers.

void XferInstruction(int* j,int instruction){
// fill buffer with XferInstruction (14 bytes)
// if j<0 use separate counter and perform IO directly (also wait for CPU ready)
	int i=0,z,w0=0;
	int IO=0;
	if((int)j==-1){
		j=&i;
		IO=1;
	}
	if(IO){
//		bufferU[(*j)++]=JTAG_SEND_CMD;
//		bufferU[(*j)++]=MTAP_SW_ETAP;
//		bufferU[(*j)++]=JTAG_SET_MODE;
//		bufferU[(*j)++]=0x3E;//0x1F;
		bufferU[(*j)++]=JTAG_SEND_CMD;
		bufferU[(*j)++]=ETAP_CONTROL;
		bufferU[(*j)++]=FLUSH;
		for(;(*j)<DIMBUF;(*j)++) bufferU[(*j)]=0x0;
		PacketIO(5);
		i=0;
		bufferU[(*j)++]=JTAG_XFER_DATA;
		bufferU[(*j)++]=0x00;
		bufferU[(*j)++]=0x04;
		bufferU[(*j)++]=0xC0;
		bufferU[(*j)++]=0x00;
		bufferU[(*j)++]=FLUSH;
		for(;(*j)<DIMBUF;(*j)++) bufferU[(*j)]=0x0;
		for(i=0;i<20;i++){	//Wait for CPU ready
			PacketIO(5);
			for(z=0;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
			if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
			if(w0&0x40000) break;
		}
		if(i==20) PrintMessage1("i=20 ECR=0x%08X\r\n",w0);
		i=0;
	}
	bufferU[(*j)++]=JTAG_SEND_CMD;
	bufferU[(*j)++]=ETAP_DATA;
	bufferU[(*j)++]=JTAG_XFER_DATA;
	bufferU[(*j)++]=(instruction>>24)&0xFF;
	bufferU[(*j)++]=(instruction>>16)&0xFF;
	bufferU[(*j)++]=(instruction>>8)&0xFF;
	bufferU[(*j)++]=instruction&0xFF;
	bufferU[(*j)++]=JTAG_SEND_CMD;
	bufferU[(*j)++]=ETAP_CONTROL;
	bufferU[(*j)++]=JTAG_XFER_DATA;
	bufferU[(*j)++]=0x00;
	bufferU[(*j)++]=0x00;
	bufferU[(*j)++]=0xC0;
	bufferU[(*j)++]=0x00;
	if(IO){
		bufferU[(*j)++]=FLUSH;
		for(;(*j)<DIMBUF;(*j)++) bufferU[(*j)]=0x0;
		PacketIO(5);
		i=0;
	}
}



void Read32x(int dim,int options){
// read 32 bit PIC32x
// deviceID @ 0xFF0000
// dim=program size (bytes)
// options:
//	bit [3:0]
	int k=0,k2=0,z=0,i,j;
	if(FWVersion<0xA00){
		PrintMessage1(strings[S_FWver2old],"0.10.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(!CheckV33Regulator()){
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	if(saveLog){
		OpenLogFile();
		fprintf(logfile,"Read32x(%d,%d)    (0x%X,0x%X)\n",dim,options,dim,options);
	}
	//dim*=2;		//from words to bytes
	if(dim>0x80000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	//unsigned char *memExec=0;
	//executiveArea*=2;
	//if(executiveArea) memExec=(unsigned char *)malloc(executiveArea);
	//for(i=0;i<executiveArea;i++) memExec[i]=0xFF;
	size=dim;
	if(memCODE) free(memCODE);
	memCODE=(unsigned char*)malloc(size);		//CODE
	memset(memCODE,0xFF,dim);
	StartHVReg(-1);		//disable HV
	unsigned int start=GetTickCount();
	int w0=0;
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=10000>>8;
	bufferU[j++]=10000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T2;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=SET_PORT_DIR;
	bufferU[j++]=0x27;	//RB7-6-4-3 output
	bufferU[j++]=0xFF;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0x08;	//MCLR=1
	bufferU[j++]=0;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;	//MCLR=0
	bufferU[j++]=0;
	bufferU[j++]=JTAG_SET_MODE;
	bufferU[j++]=0x3E;//0x1F;
	bufferU[j++]=JTAG_SEND_CMD;
	bufferU[j++]=MTAP_SW_MTAP;
	bufferU[j++]=JTAG_SET_MODE;
	bufferU[j++]=0x3E;//0x1F;
	bufferU[j++]=JTAG_SEND_CMD;
	bufferU[j++]=MTAP_COMMAND;
	bufferU[j++]=JTAG_XFER_DATA;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=MCHP_STATUS;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(25);
	j=0;
	for(z=0;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
	if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
//	PrintMessage1("w0: (0x%08X)\r\n",w0);
	bufferU[j++]=JTAG_XFER_DATA;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=MCHP_STATUS;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	j=0;
	for(i=0;(w0&0xC)!=0x8&&i<20;i++){
		PacketIO(5);
		for(z=0;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
		if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
//		PrintMessage1("w0: (0x%08X)\r\n",w0);
	}
	PrintMessage3("Device status: CFGRDY=%d FCBUSY=%d (0x%02X)\r\n",w0&0x8?1:0,w0&0x4?1:0,w0&0xFF);
	bufferU[j++]=JTAG_SEND_CMD;
	bufferU[j++]=MTAP_IDCODE;
	bufferU[j++]=JTAG_XFER_DATA;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	PacketIO(5);
	j=0;
	for(z=0;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
	if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
	PrintMessage1("IDCODE=0x%08X\r\n",w0);
	PIC32_ID(w0);
	//enable serial execution mode
	bufferU[j++]=JTAG_SEND_CMD;
	bufferU[j++]=MTAP_SW_MTAP;
	bufferU[j++]=JTAG_SET_MODE;
	bufferU[j++]=0x3E;//0x1F;
	bufferU[j++]=JTAG_SEND_CMD;
	bufferU[j++]=MTAP_COMMAND;
	bufferU[j++]=JTAG_XFER_DATA;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=0;
	bufferU[j++]=MCHP_STATUS;
	bufferU[j++]=JTAG_SEND_CMD;
	bufferU[j++]=MTAP_SW_ETAP;
	bufferU[j++]=JTAG_SET_MODE;
	bufferU[j++]=0x3E;//0x1F;
	bufferU[j++]=JTAG_SEND_CMD;
	bufferU[j++]=ETAP_EJTAGBOOT;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0x8;	//MCLR=1
	bufferU[j++]=0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	j=0;
//	msDelay(5);
	/*
	PrintMessage2("DevID: 0x%04X\r\nDevRev: 0x%04X\r\n",w0,w1);
	PIC24_ID(w0);
	//Read ApplicationID @ appIDaddr
	j=1;
	for(z=1;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2) w0=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage2("ApplicationID @ 0x80%04X:  0x%04X\r\n",appIDaddr,w0);
//****************** read code ********************
	PrintMessage(strings[S_CodeReading1]);		//code read ...
	PrintStatusSetup();
//Read 6 24 bit words packed in 9 16 bit words
//memory address advances by 24 bytes because of alignment
	int High=0;
	for(i=0;i<dim;i+=24){
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		writeP();
		msDelay(3);
		readP();
		for(z=1;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+1]=bufferI[z+1];	//M0
			memCODE[k]=bufferI[z+2];	//L0
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+2]=bufferI[z+2];	//H0
			memCODE[k+6]=bufferI[z+1];	//H1
		}
		PrintStatus(strings[S_CodeReading2],i*100/dim,i/2);	//"Read: %d%%, addr. %05X"
		j=1;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
			WriteLogIO();
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadCodeErr2],dim,k);	//"Error reading code area, requested %d bytes, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);*/
//****************** read config area ********************
//0x1FC00BF0-0BFF
	bufferU[j++]=JTAG_SEND_CMD;
	bufferU[j++]=MTAP_SW_ETAP;
	bufferU[j++]=JTAG_SET_MODE;
	bufferU[j++]=0x3E;//0x1F;

	bufferU[j++]=JTAG_SEND_CMD;
	bufferU[j++]=ETAP_CONTROL;
	bufferU[j++]=JTAG_XFER_DATA;
	bufferU[j++]=0x00;
	bufferU[j++]=0x00;
	bufferU[j++]=0x00;
	bufferU[j++]=0x00;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	j=0;
	for(i=0;i<20;i++){	//Wait for CPU ready
		PacketIO(5);
		for(z=0;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
		if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
		if(w0&0x40000) break;
	}
	//PrintMessage1("i=%d\r\n",i);
	PrintMessage1("ECR=0x%08X\r\n",w0);
	for(i=0xbFC00BF0;i<0xbFC00C10;i+=4){
/*		XferInstruction(&j,0x3C13FF20);				//lui $s3, 0xFF20
		XferInstruction(&j,0x3C080000+(i>>16));		//lui $t0,<FLASH_WORD_ADDR(31:16)>
		XferInstruction(&j,0x35080000+(i%0xFFFF));	//ori $t0,<FLASH_WORD_ADDR(15:0)>
		XferInstruction(&j,0x8d090000);				//lw $t1, 0($t0)
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(5);
		j=0;
		XferInstruction(&j,0xae690000);				//sw $t1, 0($s3)
		XferInstruction(&j,0);						//nop
*/
		XferInstruction(-1,0x3C13FF20);				//lui $s3, 0xFF20
		XferInstruction(-1,0x3C080000+(i>>16));		//lui $t0,<FLASH_WORD_ADDR(31:16)>
		XferInstruction(-1,0x35080000+(i&0xFFFF));	//ori $t0,<FLASH_WORD_ADDR(15:0)>
		XferInstruction(-1,0x8d090000);				//lw $t1, 0($t0)
		XferInstruction(-1,0xae690000);				//sw $t1, 0($s3)
		XferInstruction(-1,0);						//nop
		j=0;
		bufferU[j++]=JTAG_SEND_CMD;
		bufferU[j++]=ETAP_FASTDATA;
		bufferU[j++]=JTAG_XFER_F_DATA;
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(5);
		j=0;
		w0=-10;
		//for(z=0;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
		//for(z+=4;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
		for(z=0;bufferI[z]!=JTAG_XFER_F_DATA&&z<DIMBUF;z++);
		if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
		PrintMessage2("[0x%08X]=0x%08X\r\n",i,w0);
		//msDelay(5);
	}
	i=0xBF80F230;	//DEV ID
	XferInstruction(-1,0x3C13FF20);				//lui $s3, 0xFF20
	XferInstruction(-1,0x3C080000+(i>>16));		//lui $t0,<FLASH_WORD_ADDR(31:16)>
	XferInstruction(-1,0x35080000+(i&0xFFFF));	//ori $t0,<FLASH_WORD_ADDR(15:0)>
	XferInstruction(-1,0x8d090000);				//lw $t1, 0($t0)
	XferInstruction(-1,0xae690000);				//sw $t1, 0($s3)
	XferInstruction(-1,0);						//nop
	j=0;

/*	XferInstruction(&j,0x3C13FF20);				//lui $s3, 0xFF20
	XferInstruction(&j,0x3C080000+(i>>16));		//lui $t0,<FLASH_WORD_ADDR(31:16)>
	XferInstruction(&j,0x35080000+(i%0xFFFF));	//ori $t0,<FLASH_WORD_ADDR(15:0)>
	XferInstruction(&j,0x8d090000);				//lw $t1, 0($t0)
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	j=0;
	XferInstruction(&j,0xae690000);				//sw $t1, 0($s3)
	*/
	bufferU[j++]=JTAG_SEND_CMD;
	bufferU[j++]=ETAP_FASTDATA;
	bufferU[j++]=JTAG_XFER_F_DATA;
	bufferU[j++]=0x00;
	bufferU[j++]=0x00;
	bufferU[j++]=0x00;
	bufferU[j++]=0x00;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	j=0;
	w0=-10;
//	for(z=0;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
//	for(z+=4;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
	for(z=0;bufferI[z]!=JTAG_XFER_F_DATA&&z<DIMBUF;z++);
	if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
	PrintMessage1("DEVID=0x%08X\r\n",w0);
	PacketIO(5);
	for(z=0;bufferI[z]!=JTAG_XFER_F_DATA&&z<DIMBUF;z++);
	if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
	PrintMessage1("DEVID=0x%08X\r\n",w0);
	PacketIO(5);
	for(z=0;bufferI[z]!=JTAG_XFER_F_DATA&&z<DIMBUF;z++);
	if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
	PrintMessage1("DEVID=0x%08X\r\n",w0);
	PacketIO(5);
	for(z=0;bufferI[z]!=JTAG_XFER_F_DATA&&z<DIMBUF;z++);
	if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
	PrintMessage1("DEVID=0x%08X\r\n",w0);
	PacketIO(5);
	for(z=0;bufferI[z]!=JTAG_XFER_F_DATA&&z<DIMBUF;z++);
	if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
	PrintMessage1("DEVID=0x%08X\r\n",w0);
	XferInstruction(-1,0x3C13FF20);				//lui $s3, 0xFF20
	XferInstruction(-1,0x3C080000+(i>>16));		//lui $t0,<FLASH_WORD_ADDR(31:16)>
	XferInstruction(-1,0x35080000+(i&0xFFFF));	//ori $t0,<FLASH_WORD_ADDR(15:0)>
	XferInstruction(-1,0x8d090000);				//lw $t1, 0($t0)
	XferInstruction(-1,0xae690000);				//sw $t1, 0($s3)
	XferInstruction(-1,0);						//nop
	j=0;
	bufferU[j++]=JTAG_SEND_CMD;
	bufferU[j++]=ETAP_FASTDATA;
	bufferU[j++]=JTAG_XFER_F_DATA;
	bufferU[j++]=0x00;
	bufferU[j++]=0x00;
	bufferU[j++]=0x00;
	bufferU[j++]=0x00;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	j=0;
	w0=-10;
//	for(z=0;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
//	for(z+=4;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
	for(z=0;bufferI[z]!=JTAG_XFER_F_DATA&&z<DIMBUF;z++);
	if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
	PrintMessage1("DEVID=0x%08X\r\n",w0);
// Boot block
//	PrintMessage("BOOT BLOCK @0x1FC00000\r\n");
	for(i=0x1FC00000;i<0x1FC00BF0&&0;i+=4){
		XferInstruction(&j,0x3C13FF20);				//lui $s3, 0xFF20
		XferInstruction(&j,0x3C080000+(i>>16));		//lui $t0,<FLASH_WORD_ADDR(31:16)>
		XferInstruction(&j,0x35080000+(i%0xFFFF));	//ori $t0,<FLASH_WORD_ADDR(15:0)>
		XferInstruction(&j,0x8d090000);				//lw $t1, 0($t0)
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(5);
		j=0;
		XferInstruction(&j,0xae690000);				//sw $t1, 0($s3)
		bufferU[j++]=JTAG_SEND_CMD;
		bufferU[j++]=ETAP_FASTDATA;
		bufferU[j++]=JTAG_XFER_F_DATA;
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(5);
		j=0;
		for(z=0;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
		for(z+=4;bufferI[z]!=JTAG_XFER_DATA&&z<DIMBUF;z++);
		for(z+=4;bufferI[z]!=JTAG_XFER_F_DATA&&z<DIMBUF;z++);
		if(z<DIMBUF-4) w0=(bufferI[z+1]<<24)+(bufferI[z+2]<<16)+(bufferI[z+3]<<8)+bufferI[z+4];
		PrintMessage2("[0x%08X]=0x%08X\r\n",i,w0);
	}

/*	if(config>2){					//config area @ 0xF80000
		if(saveLog)	fprintf(logfile,"\nCONFIG:\n");
		bufferU[j++]=SIX_N;
		bufferU[j++]=4;
		bufferU[j++]=0x20;				//MOV XXXX,W0
		bufferU[j++]=0x0F;
		bufferU[j++]=0x80;
		bufferU[j++]=0x88;				//MOV W0,TABLPAG
		if(newTABLPAG){
			bufferU[j++]=0x02;
			bufferU[j++]=0xA0;
		}
		else{
			bufferU[j++]=0x01;
			bufferU[j++]=0x90;
		}
		bufferU[j++]=REGOUT;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		writeP();
		msDelay(3);
		readP();
		j=1;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],0xF80000,0xF80000,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
			WriteLogIO();
		}
		//save 0xF800000 to 0xF80010
		for(i=0,z=1;i<9;i++){
			for(;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			memCONFIG[i*4]=bufferI[z+2];	//Low byte
			memCONFIG[i*4+1]=bufferI[z+1];	//High byte
			z+=3;
		}
		writeP();
		msDelay(3);
		readP();
		j=1;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],0xF80000,0xF80000,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
			WriteLogIO();
		}
		//save 0xF800012 to 0xF80016
		for(i=9,z=1;i<12;i++){
			for(;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			memCONFIG[i*4]=bufferI[z+2];	//Low byte
			memCONFIG[i*4+1]=bufferI[z+1];	//High byte
			z+=3;
		}
		PrintStatusClear();
	}*/
//****************** exit ********************
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=0;	//MCLR=0
	bufferU[j++]=0;
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	writeP();
	msDelay(2);
	readP();
	if(saveLog)	WriteLogIO();
	unsigned int stop=GetTickCount();
	PrintStatusClear();
//****************** visualize ********************
/*	if(config>2){					//only if separate config area
		PrintMessage(strings[S_ConfigMem]);				//"\r\nConfig Memory:\r\n"
		if(config==3||config==4||config==6){
			PrintMessage1("0xF80000: FBS = 0x%02X\r\n",memCONFIG[0]);
			if(config==4){			//0xF80000-16
				PrintMessage1("0xF80002: FSS = 0x%02X\r\n",memCONFIG[4]);
			}
			PrintMessage1("0xF80004: FGS = 0x%02X\r\n",memCONFIG[8]);
			PrintMessage1("0xF80006: FOSCSEL = 0x%02X\r\n",memCONFIG[12]);
			PrintMessage1("0xF80008: FOSC = 0x%02X\r\n",memCONFIG[16]);
			PrintMessage1("0xF8000A: FWDT = 0x%02X\r\n",memCONFIG[20]);
			PrintMessage1("0xF8000C: FPOR = 0x%02X\r\n",memCONFIG[24]);
			PrintMessage1("0xF8000E: FICD = 0x%02X\r\n",memCONFIG[28]);
			if(config==3){			//0xF80000-10 except 02
				PrintMessage1("0xF80010: FDS = 0x%02X\r\n",memCONFIG[32]);
			}
			else if(config==4){			//0xF80000-16
				PrintMessage1("0xF80010: UID0 = 0x%02X\r\n",memCONFIG[32]);
				PrintMessage1("0xF80012: UID1 = 0x%02X\r\n",memCONFIG[36]);
				PrintMessage1("0xF80014: UID2 = 0x%02X\r\n",memCONFIG[40]);
				PrintMessage1("0xF80016: UID3 = 0x%02X\r\n",memCONFIG[44]);
			}
		}
		else if(config==5){			//0xF80000-0C (16 bit)
			PrintMessage2("0xF80000: FOSC = 0x%02X%02X\r\n",memCONFIG[1],memCONFIG[0]);
			PrintMessage2("0xF80002: FWDT = 0x%02X%02X\r\n",memCONFIG[5],memCONFIG[4]);
			PrintMessage2("0xF80004: FBORPOR = 0x%02X%02X\r\n",memCONFIG[9],memCONFIG[8]);
			PrintMessage2("0xF80006: FBS = 0x%02X%02X\r\n",memCONFIG[13],memCONFIG[12]);
			PrintMessage2("0xF80008: FSS = 0x%02X%02X\r\n",memCONFIG[17],memCONFIG[16]);
			PrintMessage2("0xF8000A: FGS = 0x%02X%02X\r\n",memCONFIG[21],memCONFIG[20]);
			PrintMessage2("0xF8000C: FICD = 0x%02X%02X\r\n",memCONFIG[25],memCONFIG[24]);
		}
	}
	else{
			//last 2 program words
			PrintMessage2("CONFIG1: 0x%04X\r\nCONFIG2: 0x%04X\r\n",(memCODE[dim-3]<<8)+memCODE[dim-4]\
				,(memCODE[dim-7]<<8)+memCODE[dim-8]);
			if(config==1){			//last 3 program words
				PrintMessage1("CONFIG3: 0x%04X\r\n",(memCODE[dim-11]<<8)+memCODE[dim-12]);
			}
			if(config==2){			//last 4 program words
				PrintMessage1("CONFIG4: 0x%04X\r\n",(memCODE[dim-15]<<8)+memCODE[dim-16]);
			}
		}
	PrintMessage(strings[S_CodeMem]);	//"\r\nCode memory:\r\n"
	DisplayCODE24F(dim);
	if(dim2){
		PrintMessage(strings[S_EEMem]);	//"\r\nEEPROM memory:\r\n"
		DisplayEE24F();
	}
	if(executiveArea){
		char s[256],t[256];
		int d,valid,empty=1;
		char* aux=(char*)malloc((dim/COL+1)*2*(16+COL*9));
		aux[0]=0;
		s[0]=0;
		empty=1;
		PrintMessage(strings[S_ExeMem]);	//"\r\nExecutive memory:\r\n"
		for(i=0;i<executiveArea;i+=COL*2){
			valid=0;
			for(j=i;j<i+COL*2&&j<executiveArea;j+=4){
				d=(memExec[j+3]<<24)+(memExec[j+2]<<16)+(memExec[j+1]<<8)+memExec[j];
				sprintf(t,"%08X ",d);
				strcat(s,t);
				if(d!=0xffffffff) valid=1;
			}
			if(valid){
				sprintf(t,"%06X: %s\r\n",i/2,s);
				empty=0;
				strcat(aux,t);
			}
			s[0]=0;
		}
		if(empty) PrintMessage(strings[S_Empty]);	//empty
		else PrintMessage(aux);
		free(aux);
	}*/
	PrintMessage1(strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	if(saveLog) CloseLogFile();
}

/*
#ifdef _MSC_VER
void COpenProgDlg::Write24Fx(int dim,int dim2,int options,int appIDaddr,int rowSize, double wait){
#else
void Write24Fx(int dim,int dim2,int options,int appIDaddr,int rowSize, double wait){
#endif
// write 16 bit PIC 24Fxxxx
// deviceID @ 0xFF0000
// dim=program size (16 bit words)
// dim2=eeprom size (in bytes, area starts at 0x800000-size)
// options:
//	bit [3:0]
//     0 = low voltage ICSP entry
//     1 = High voltage ICSP entry (6V)
//     2 = High voltage ICSP entry (12V) + PIC30F sequence (additional NOPs)
//     3 = low voltage ICSP entry (5V power supply)
//	bit [7:4]
//	   0 = config area in the last 2 program words
//	   1 = config area in the last 3 program words
//	   2 = config area in the last 4 program words
//	   3 = 0xF80000 to 0xF80010 except 02 (24F)
//     4 = 0xF80000 to 0xF80016 (24H-33F)
//     5 = 0xF80000 to 0xF8000C (x16 bit, 30F)
//     6 = 0xF80000 to 0xF8000E (30FSMPS)
//	bit [11:8]
//	   0 = code erase word is 0x4064, row write is 0x4004
//	   1 = code erase word is 0x404F, row write is 0x4001
//	   2 = code erase word is 0x407F, row write is 0x4001, 55AA unlock and external timing (2 ms)
//	   3 = code erase word is 0x407F, row write is 0x4001, 55AA unlock and external timing (200 ms)
//	bit [15:12]
//	   0 = eeprom erase word is 0x4050, write word is 0x4004
//	   1 = eeprom erased with bulk erase, write word is 0x4004
//	   2 = eeprom erased with special sequence, write word is 0x4004
//	bit [19:16]
//	   0 = config write is 0x4000
//	   1 = config write is 0x4003
//	   2 = config write is 0x4004
//	   3 = config write is 0x4008
//	bit [20]
//	   0 = standard TABLPAG address
//	   1 = new TABLPAG address
// appIDaddr = application ID word lower address (high is 0x80)
// rowSize = row size in instruction words (a row is written altogether)
// wait = write delay in ms
	int k=0,k2=0,z=0,i,j;
	int entry=options&0xF;
	int config=(options>>4)&0xF;
	int EEbaseAddr=0x1000-dim2;
	int newTABLPAG=options&0x100000?1:0;
	int err=0;
	if(FWVersion<0x700){
		PrintMessage1(strings[S_FWver2old],"0.7.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(entry<2&&!CheckV33Regulator()){		//except 30Fxx which is on 5V
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	if(saveLog){
		OpenLogFile();
		fprintf(logfile,"Write24Fx(%d,%d,%d,%d,%d,%.1f)    (0x%X,0x%X,0x%X,0x%X,0x%X,%.3f)\n"
			,dim,dim2,options,appIDaddr,rowSize,wait,dim,dim2,options,appIDaddr,rowSize,wait);
	}
	dim*=2;		//from words to bytes
	if(dim>0x80000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	if(dim2>0x1000||dim2<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	j=size;
	if(j%(rowSize*4)){			//grow to an integer number of rows
		size=(j/(rowSize*4)+1)*rowSize*4;
		memCODE=(unsigned char*)realloc(memCODE,size);
		for(;j<size;j++) memCODE[j]=0xFF;
	}
	if(dim>size) dim=size;
	if(sizeEE<0x1000) dim2=0;
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Empty code area\r\n"
		return;
	}
	if((entry==1)||(entry==2)){			//High voltage programming: 3.3V + 1.5V + R drop + margin
		if(!StartHVReg(entry==2?12:6)){	//12V only for 30Fxx !!!
			PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
			return;
		}
	}
	else StartHVReg(-1);		//LVP: current limited to (5-0.7-3.6)/10k = 50uA
	if(config>2){					//only if separate config area
		PrintMessage(strings[S_ConfigMem]);				//"\r\nConfig Memory:\r\n"
		if(config==3||config==4||config==6){
			PrintMessage1("0xF80000: FBS = 0x%02X\r\n",memCONFIG[0]);
			if(config==4){			//0xF80000-16
				PrintMessage1("0xF80002: FSS = 0x%02X\r\n",memCONFIG[4]);
			}
			PrintMessage1("0xF80004: FGS = 0x%02X\r\n",memCONFIG[8]);
			PrintMessage1("0xF80006: FOSCSEL = 0x%02X\r\n",memCONFIG[12]);
			PrintMessage1("0xF80008: FOSC = 0x%02X\r\n",memCONFIG[16]);
			PrintMessage1("0xF8000A: FWDT = 0x%02X\r\n",memCONFIG[20]);
			PrintMessage1("0xF8000C: FPOR = 0x%02X\r\n",memCONFIG[24]);
			PrintMessage1("0xF8000E: FICD = 0x%02X\r\n",memCONFIG[28]);
			if(config==3){			//0xF80000-10 except 02
				PrintMessage1("0xF80010: FDS = 0x%02X\r\n",memCONFIG[32]);
			}
			else if(config==4){			//0xF80000-16
				PrintMessage1("0xF80010: UID0 = 0x%02X\r\n",memCONFIG[32]);
				PrintMessage1("0xF80012: UID1 = 0x%02X\r\n",memCONFIG[36]);
				PrintMessage1("0xF80014: UID2 = 0x%02X\r\n",memCONFIG[40]);
				PrintMessage1("0xF80016: UID3 = 0x%02X\r\n",memCONFIG[44]);
			}
		}
		else if(config==5){			//0xF80000-0C (16 bit)
			PrintMessage2("0xF80000: FOSC = 0x%02X%02X\r\n",memCONFIG[1],memCONFIG[0]);
			PrintMessage2("0xF80002: FWDT = 0x%02X%02X\r\n",memCONFIG[5],memCONFIG[4]);
			PrintMessage2("0xF80004: FBORPOR = 0x%02X%02X\r\n",memCONFIG[9],memCONFIG[8]);
			PrintMessage2("0xF80006: FBS = 0x%02X%02X\r\n",memCONFIG[13],memCONFIG[12]);
			PrintMessage2("0xF80008: FSS = 0x%02X%02X\r\n",memCONFIG[17],memCONFIG[16]);
			PrintMessage2("0xF8000A: FGS = 0x%02X%02X\r\n",memCONFIG[21],memCONFIG[20]);
			PrintMessage2("0xF8000C: FICD = 0x%02X%02X\r\n",memCONFIG[25],memCONFIG[24]);
		}
	}
	else{
			//last 2 program words
			PrintMessage2("CONFIG1: 0x%04X\r\nCONFIG2: 0x%04X\r\n",(memCODE[dim-3]<<8)+memCODE[dim-4]\
				,(memCODE[dim-7]<<8)+memCODE[dim-8]);
			if(config==1){			//last 3 program words
				PrintMessage1("CONFIG3: 0x%04X\r\n",(memCODE[dim-11]<<8)+memCODE[dim-12]);
			}
			if(config==2){			//last 4 program words
				PrintMessage1("CONFIG4: 0x%04X\r\n",(memCODE[dim-15]<<8)+memCODE[dim-16]);
			}
		}
	unsigned int start=GetTickCount();
	bufferU[0]=0;
	j=1;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T2;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;		//VDD + VPP
	bufferU[j++]=0x5;
	if((entry==0)||(entry==3)){		//LVP: pulse on MCLR
		bufferU[j++]=EN_VPP_VCC;	//VDD
		bufferU[j++]=0x1;
	}
	else PrintMessage(strings[S_HVICSP]); //"High Voltage ICSP\r\n"
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=TX16;
	bufferU[j++]=2;
	bufferU[j++]=0x4D;
	bufferU[j++]=0x43;
	bufferU[j++]=0x48;
	bufferU[j++]=0x51;
	bufferU[j++]=WAIT_T3;			//min 1ms
	bufferU[j++]=EN_VPP_VCC;		//VDD + VPP
	bufferU[j++]=0x5;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=30000>>8;
	bufferU[j++]=30000&0xff;
	bufferU[j++]=WAIT_T3;			//min 25ms
	if(entry==2){					//30Fx entry
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=EN_VPP_VCC;	//VDD
		bufferU[j++]=0x1;
		bufferU[j++]=EN_VPP_VCC;		//VDD + VPP
		bufferU[j++]=0x5;
		bufferU[j++]=ICSP_NOP;
	}
	else{
		//Additional 5 clock cycles upon entering program mode
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;				//CK=0
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x4;				//CK=1
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;				//CK=0
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x4;				//CK=1
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;				//CK=0
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x4;				//CK=1
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;				//CK=0
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x4;				//CK=1
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;				//CK=0
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x4;				//CK=1
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;				//CK=0
	}
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX;				//GOTO 0x200
	bufferU[j++]=0x04;
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	writeP();
	msDelay(37);
	readP();
	if(saveLog)WriteLogIO();
	j=1;
	//Read DeviceID @0xFF0000, DevRev @0xFF0002
	bufferU[j++]=SIX_N;
	bufferU[j++]=4;
	bufferU[j++]=0x20;				//MOV XXXX,W0
	bufferU[j++]=0x0F;
	bufferU[j++]=0xF0;				//0xFF
	bufferU[j++]=0x88;				//MOV W0,TABLPAG
	if(newTABLPAG){
		bufferU[j++]=0x02;
		bufferU[j++]=0xA0;
	}
	else{
		bufferU[j++]=0x01;
		bufferU[j++]=0x90;
	}
	bufferU[j++]=0x20;				//MOV XXXX,W6
	bufferU[j++]=0x00;
	bufferU[j++]=0x06;				//0x0000
	bufferU[j++]=0x20;				//MOV #VISI,W7
	bufferU[j++]=0x78;
	bufferU[j++]=0x47;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0xB6;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0xB6;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX;				//GOTO 0x200
	bufferU[j++]=0x04;
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	writeP();
	msDelay(2);
	readP();
	if(saveLog)WriteLogIO();
	j=1;
	int w0=0,w1=0;
	for(z=1;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2) w0=(bufferI[z+1]<<8)+bufferI[z+2];
	for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2) w1=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage2("DevID: 0x%04X\r\nDevRev: 0x%04X\r\n",w0,w1);
	PIC24_ID(w0);
	//Read ApplicationID @ appIDaddr
	bufferU[j++]=SIX_N;
	bufferU[j++]=0x44;				//append 1 NOP
	bufferU[j++]=0x20;				//MOV XXXX,W0
	bufferU[j++]=0x08;
	bufferU[j++]=0x00;
	bufferU[j++]=0x88;				//MOV W0,TABLPAG
	if(newTABLPAG){
		bufferU[j++]=0x02;
		bufferU[j++]=0xA0;
	}
	else{
		bufferU[j++]=0x01;
		bufferU[j++]=0x90;
	}
	bufferU[j++]=0x20+((appIDaddr>>12)&0xF);	//MOV XXXX,W6
	bufferU[j++]=(appIDaddr>>4)&0xFF;
	bufferU[j++]=((appIDaddr<<4)&0xF0)+6;
	bufferU[j++]=0x20;				//MOV #VISI,W7
	bufferU[j++]=0x78;
	bufferU[j++]=0x47;
	bufferU[j++]=SIX_LONG;				//TBLRDL [W6],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0x96;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG;				//GOTO 0x200
	bufferU[j++]=0x04;
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	writeP();
	msDelay(2);
	readP();
	j=1;
	if(saveLog)WriteLogIO();
	for(z=1;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2) w0=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage2("ApplicationID @ 0x80%04X:  0x%04X\r\n",appIDaddr,w0);
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	if(saveLog)	fprintf(logfile,"\nERASE:\n");
	int erase=(options&0xF00)>>8;
	//bulk erase command
	bufferU[j++]=SIX_N;
	bufferU[j++]=4;
	bufferU[j++]=0x24;				//MOV XXXX,W10
	if(erase==0){			//0x4064
		bufferU[j++]=0x06;
		bufferU[j++]=0x4A;
	}
	else if(erase==1){		//0x404F
		bufferU[j++]=0x04;
		bufferU[j++]=0xFA;
	}
	else if(erase>=2){		//0x407F
		bufferU[j++]=0x07;
		bufferU[j++]=0xFA;
	}
	bufferU[j++]=0x88;				//MOV W10,NVMCON
	bufferU[j++]=0x3B;
	bufferU[j++]=0x0A;
	bufferU[j++]=0x20;				//MOV XXXX,W0
	bufferU[j++]=0x00;
	bufferU[j++]=0x00;
	bufferU[j++]=0x88;				//MOV W0,TABLPAG
	if(newTABLPAG){
		bufferU[j++]=0x02;
		bufferU[j++]=0xA0;
	}
	else{
		bufferU[j++]=0x01;
		bufferU[j++]=0x90;
	}
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX_LONG;			//TBLWTL W0,[W0] (dummy write)
	bufferU[j++]=0xBB;
	bufferU[j++]=0x08;
	bufferU[j++]=0x00;
	if(erase>=2){				//30Fx, unlock and external timing
		bufferU[j++]=SIX_N;
		bufferU[j++]=4;
		bufferU[j++]=0x20;				//MOV 0x55,W8
		bufferU[j++]=0x05;
		bufferU[j++]=0x58;
		bufferU[j++]=0x88;				//MOV W8,NVMKEY
		bufferU[j++]=0x3B;
		bufferU[j++]=0x38;
		bufferU[j++]=0x20;				//MOV 0xAA,W8
		bufferU[j++]=0x0A;
		bufferU[j++]=0xA8;
		bufferU[j++]=0x88;				//MOV W8,NVMKEY
		bufferU[j++]=0x3B;
		bufferU[j++]=0x38;
		bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
		bufferU[j++]=0xA8;
		bufferU[j++]=0xE7;
		bufferU[j++]=0x61;
		bufferU[j++]=WAIT_T3;
		if(erase==3){				//200 ms timing
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			writeP();
			msDelay(200);
			readP();
			j=1;
			if(saveLog)WriteLogIO();
		}
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=SIX_LONG;				//BCLR NVMCON,#WR
		bufferU[j++]=0xA9;
		bufferU[j++]=0xE7;
		bufferU[j++]=0x61;
	}
	else{		//internal timing
		bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
		bufferU[j++]=0xA8;
		bufferU[j++]=0xE7;
		bufferU[j++]=0x61;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=ICSP_NOP;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=SIX;				//MOV NVMCON,W2
	bufferU[j++]=0x80;
	bufferU[j++]=0x3B;
	bufferU[j++]=0x02;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX;				//MOV W2,VISI
	bufferU[j++]=0x88;
	bufferU[j++]=0x3C;
	bufferU[j++]=0x22;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=ICSP_NOP;
	bufferU[j++]=REGOUT;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	writeP();
	msDelay(7);
	readP();
	j=1;
	if(saveLog)WriteLogIO();
	for(z=1;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	w0=bufferI[z+1]&0x80;
	//Wait for erase completion (max 1s)
	for(i=0;erase<2&&i<100&&w0;i++){
	bufferU[j++]=SIX;				//MOV NVMCON,W2
	bufferU[j++]=0x80;
	bufferU[j++]=0x3B;
	bufferU[j++]=0x02;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX;				//MOV W2,VISI
	bufferU[j++]=0x88;
	bufferU[j++]=0x3C;
	bufferU[j++]=0x22;
	bufferU[j++]=REGOUT;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
		writeP();
		msDelay(10);
		readP();
		j=1;
		if(saveLog)WriteLogIO();
		for(z=1;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		w0=bufferI[z+1]&0x80;
	}
//****************** prepare write ********************
	bufferU[j++]=SIX_N;
	bufferU[j++]=5;
	bufferU[j++]=0x24;				//MOV XXXX,W10
	bufferU[j++]=0x00;
	bufferU[j++]=erase>0?0x1A:0x4A;	//0x4001/0x4004
	bufferU[j++]=0x88;				//MOV W10,NVMCON
	bufferU[j++]=0x3B;
	bufferU[j++]=0x0A;
	bufferU[j++]=0x20;				//MOV XXXX,W0
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
	bufferU[j++]=0x88;				//MOV W0,TABLPAG
	if(newTABLPAG){
		bufferU[j++]=0x02;
		bufferU[j++]=0xA0;
	}
	else{
		bufferU[j++]=0x01;
		bufferU[j++]=0x90;
	}
	bufferU[j++]=0x04;				//GOTO 0x200
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	writeP();
	msDelay(2);
	readP();
	j=1;
	if(saveLog)WriteLogIO();
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	if(saveLog)	fprintf(logfile,"\nWRITE CODE:\n");
	PrintStatusSetup();
//	instruction words are stored in code memory array as follows:
//	L0 M0 H0 FF L1 M1 H1 FF
	int valid,High=0;
		for(i=0,k=0;i<dim;i+=4*4){		//write 4 instruction words
			if(k==0){				//skip row if empty
				for(valid=0;!valid&&i<dim;i+=valid?0:rowSize*4){
					for(k2=0;k2<rowSize*4&&!valid;k2++) if(memCODE[i+k2]<0xFF) valid=1;
				}
				if(i>=dim) break;
			}
			if((i>>17)!=High){	//advance page
				bufferU[j++]=SIX;				//MOV XXXX,W0
				bufferU[j++]=0x20;
				bufferU[j++]=(i>>21)&0xFF;
				bufferU[j++]=(i>>13)&0xF0;
				bufferU[j++]=SIX;				//MOV W0,TABLPAG
				bufferU[j++]=0x88;
				if(newTABLPAG){
					bufferU[j++]=0x02;
					bufferU[j++]=0xA0;
				}
				else{
					bufferU[j++]=0x01;
					bufferU[j++]=0x90;
				}
				bufferU[j++]=SIX;				//GOTO 0x200
				bufferU[j++]=0x04;
				bufferU[j++]=0x02;
				bufferU[j++]=0x00;
				bufferU[j++]=ICSP_NOP;
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				writeP();
				msDelay(2);
				readP();
				j=1;
				if(saveLog)WriteLogIO();
				High=i>>17;
			}
			bufferU[j++]=SIX_N;
			bufferU[j++]=8;
			bufferU[j++]=0x20+((i>>13)&0xF);						//MOV i/2,W7
			bufferU[j++]=(i>>5)&0xFF;
			bufferU[j++]=((i<<3)&0xF0)+7;
			bufferU[j++]=0x20+((memCODE[i+1]>>4)&0xF);				//MOV LSW0,W0
			bufferU[j++]=((memCODE[i+1]<<4)&0xF0)+((memCODE[i]>>4)&0xF);
			bufferU[j++]=(memCODE[i]<<4)&0xF0;
			bufferU[j++]=0x20+((memCODE[i+6]>>4)&0xF);				//MOV MSB1:MSB0,W1
			bufferU[j++]=((memCODE[i+6]<<4)&0xF0)+((memCODE[i+2]>>4)&0xF);
			bufferU[j++]=((memCODE[i+2]<<4)&0xF0)+1;
			bufferU[j++]=0x20+((memCODE[i+5]>>4)&0xF);				//MOV LSW1,W2
			bufferU[j++]=((memCODE[i+5]<<4)&0xF0)+((memCODE[i+4]>>4)&0xF);
			bufferU[j++]=((memCODE[i+4]<<4)&0xF0)+2;
			bufferU[j++]=0x20+((memCODE[i+9]>>4)&0xF);				//MOV LSW2,W3
			bufferU[j++]=((memCODE[i+9]<<4)&0xF0)+((memCODE[i+8]>>4)&0xF);
			bufferU[j++]=((memCODE[i+8]<<4)&0xF0)+3;
			bufferU[j++]=0x20+((memCODE[i+14]>>4)&0xF);				//MOV MSB3:MSB2,W4
			bufferU[j++]=((memCODE[i+14]<<4)&0xF0)+((memCODE[i+10]>>4)&0xF);
			bufferU[j++]=((memCODE[i+10]<<4)&0xF0)+4;
			bufferU[j++]=0x20+((memCODE[i+13]>>4)&0xF);				//MOV LSW3,W5
			bufferU[j++]=((memCODE[i+13]<<4)&0xF0)+((memCODE[i+12]>>4)&0xF);
			bufferU[j++]=((memCODE[i+12]<<4)&0xF0)+5;
			bufferU[j++]=0xEB;				//CLR W6
			bufferU[j++]=0x03;
			bufferU[j++]=0x00;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=SIX_N;
			bufferU[j++]=0x88;				//Append 2 NOP
			bufferU[j++]=0xBB;				//TBLWTL [W6++],[W7]
			bufferU[j++]=0x0B;
			bufferU[j++]=0xB6;
			bufferU[j++]=0xBB;				//TBLWTH.B [W6++],[W7++]
			bufferU[j++]=0xDB;
			bufferU[j++]=0xB6;
			bufferU[j++]=0xBB;				//TBLWTH.B [W6++],[++W7]
			bufferU[j++]=0xEB;
			bufferU[j++]=0xB6;
			bufferU[j++]=0xBB;				//TBLWTL [W6++],[W7++]
			bufferU[j++]=0x1B;
			bufferU[j++]=0xB6;
			bufferU[j++]=0xBB;				//TBLWTL [W6++],[W7]
			bufferU[j++]=0x0B;
			bufferU[j++]=0xB6;
			bufferU[j++]=0xBB;				//TBLWTH.B [W6++],[W7++]
			bufferU[j++]=0xDB;
			bufferU[j++]=0xB6;
			bufferU[j++]=0xBB;				//TBLWTH.B [W6++],[++W7]
			bufferU[j++]=0xEB;
			bufferU[j++]=0xB6;
			bufferU[j++]=0xBB;				//TBLWTL [W6++],[W7++]
			bufferU[j++]=0x1B;
			bufferU[j++]=0xB6;
			k++;
			if(k==rowSize/4){	//Write row
				if(erase>1){				//30Fx, unlock and external timing
					bufferU[j++]=FLUSH;
					for(;j<DIMBUF;j++) bufferU[j]=0x0;
					writeP();
					msDelay(3);
					readP();
					j=1;
					if(saveLog){
						fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d, k=%d 0=%d\n"
						WriteLogIO();
					}
					bufferU[j++]=SIX_N;
					bufferU[j++]=6;
					bufferU[j++]=0x24;				//MOV XXXX,W10
					bufferU[j++]=0x00;
					bufferU[j++]=erase>0?0x1A:0x4A;	//0x4001/0x4004
					bufferU[j++]=0x88;				//MOV W10,NVMCON
					bufferU[j++]=0x3B;
					bufferU[j++]=0x0A;
					bufferU[j++]=0x20;				//MOV 0x55,W8
					bufferU[j++]=0x05;
					bufferU[j++]=0x58;
					bufferU[j++]=0x88;				//MOV W8,NVMKEY
					bufferU[j++]=0x3B;
					bufferU[j++]=0x38;
					bufferU[j++]=0x20;				//MOV 0xAA,W8
					bufferU[j++]=0x0A;
					bufferU[j++]=0xA8;
					bufferU[j++]=0x88;				//MOV W8,NVMKEY
					bufferU[j++]=0x3B;
					bufferU[j++]=0x38;
					bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
					bufferU[j++]=0xA8;
					bufferU[j++]=0xE7;
					bufferU[j++]=0x61;
					bufferU[j++]=ICSP_NOP;
					bufferU[j++]=ICSP_NOP;
					bufferU[j++]=WAIT_T3;
					bufferU[j++]=WAIT_T3;
					bufferU[j++]=ICSP_NOP;
					bufferU[j++]=ICSP_NOP;
					bufferU[j++]=SIX_LONG;				//BCLR NVMCON,#WR
					bufferU[j++]=0xA9;
					bufferU[j++]=0xE7;
					bufferU[j++]=0x61;
					bufferU[j++]=SIX_LONG;				//GOTO 0x200
					bufferU[j++]=0x04;
					bufferU[j++]=0x02;
					bufferU[j++]=0x00;
				}
				else{		//internal timing
				bufferU[j++]=SIX_LONG;				//GOTO 0x200
				bufferU[j++]=0x04;
				bufferU[j++]=0x02;
				bufferU[j++]=0x00;
				bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
				bufferU[j++]=0xA8;
				bufferU[j++]=0xE7;
				bufferU[j++]=0x61;
				}
				k=0;
			}
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			writeP();
			msDelay(3);
			readP();
			j=1;
			PrintStatus(strings[S_CodeWriting2],i*100/(dim+dim2),i/2);	//"Write: %d%%,addr. %04X"
			if(RWstop) i=dim;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i/2,i/2,k,k);	//"i=%d, k=%d 0=%d\n"
				WriteLogIO();
			}
		}
	PrintStatusEnd();
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** verify code ********************
	PrintMessage(strings[S_CodeV]);	//"Verify code ... "
	if(saveLog)	fprintf(logfile,"\nVERIFY CODE:\n");
	PrintStatusSetup();
//Read 4 24 bit words packed in 6 16 bit words
//memory address advances by 16 bytes because of alignment
	High=0xE0000000;
	int r0=0,r1=0,r2=0,r3=0,w3,w2;
	for(i=0;i<dim;i+=16){
		//skip row if empty
		for(valid=0;!valid&&i<dim;i+=valid?0:16){
			for(k2=0;k2<16&&!valid;k2++) if(memCODE[i+k2]<0xFF) valid=1;
		}
		if(i>=dim) break;
		if((i>>17)!=High){	//advance page
			bufferU[j++]=SIX;				//MOV XXXX,W0
			bufferU[j++]=0x20;
			bufferU[j++]=(i>>21)&0xFF;
			bufferU[j++]=(i>>13)&0xF0;
			bufferU[j++]=SIX;				//MOV W0,TABLPAG
			bufferU[j++]=0x88;
			if(newTABLPAG){
				bufferU[j++]=0x02;
				bufferU[j++]=0xA0;
			}
			else{
				bufferU[j++]=0x01;
				bufferU[j++]=0x90;
			}
			bufferU[j++]=SIX;				//GOTO 0x200
			bufferU[j++]=0x04;
			bufferU[j++]=0x02;
			bufferU[j++]=0x00;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			writeP();
			msDelay(2);
			readP();
			j=1;
			if(saveLog)WriteLogIO();
			High=i>>17;
		}
		bufferU[j++]=SIX;				//MOV i/2,W6
		bufferU[j++]=0x20+((i>>13)&0xF);
		bufferU[j++]=(i>>5)&0xFF;
		bufferU[j++]=((i<<3)&0xF0)+6;
		bufferU[j++]=SIX;				//MOV #VISI,W7
		bufferU[j++]=0x20;
		bufferU[j++]=0x78;
		bufferU[j++]=0x47;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0x96;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDH.B [W6++],[W7++]
		bufferU[j++]=0xBA;
		bufferU[j++]=0xDB;
		bufferU[j++]=0xB6;
		bufferU[j++]=SIX_LONG;				//TBLRDH.B [++W6],[W7--]
		bufferU[j++]=0xBA;
		bufferU[j++]=0xD3;
		bufferU[j++]=0xD6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0x96;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDH.B [W6++],[W7++]
		bufferU[j++]=0xBA;
		bufferU[j++]=0xDB;
		bufferU[j++]=0xB6;
		bufferU[j++]=SIX_LONG;				//TBLRDH.B [++W6],[W7--]
		bufferU[j++]=0xBA;
		bufferU[j++]=0xD3;
		bufferU[j++]=0xD6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//GOTO 0x200
		bufferU[j++]=0x04;
		bufferU[j++]=0x02;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		writeP();
		msDelay(3);
		readP();
		PrintStatus(strings[S_CodeV2],i*100/(dim+dim2),i/2);	//"Verify: %d%%, addr. %04X"
		if(RWstop) i=dim;
		for(z=1;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			r0=(bufferI[z+1]<<8)+bufferI[z+2];
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			r0+=bufferI[z+2]<<16;
			r1=bufferI[z+1]<<16;
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			r1+=(bufferI[z+1]<<8)+bufferI[z+2];
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			r2=(bufferI[z+1]<<8)+bufferI[z+2];
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			r2+=bufferI[z+2]<<16;
			r3=bufferI[z+1]<<16;
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			r3+=(bufferI[z+1]<<8)+bufferI[z+2];
		}
		w0=(memCODE[i+2]<<16)+(memCODE[i+1]<<8)+memCODE[i];
		w1=(memCODE[i+6]<<16)+(memCODE[i+5]<<8)+memCODE[i+4];
		w2=(memCODE[i+10]<<16)+(memCODE[i+9]<<8)+memCODE[i+8];
		w3=(memCODE[i+14]<<16)+(memCODE[i+13]<<8)+memCODE[i+12];
		CheckData(w0,r0,i/2,&err);
		CheckData(w1,r1,i/2+2,&err);
		CheckData(w2,r2,i/2+4,&err);
		CheckData(w3,r3,i/2+6,&err);
		PrintStatus(strings[S_CodeV2],i*100/dim,i/2);	//"Verify: %d%%, addr. %05X"
		j=1;
		if(saveLog){
			fprintf(logfile,strings[S_Log8],i/2,i/2,k,k,err);	//"i=%d, k=%d, errori=%d\n"
			WriteLogIO();
		}
		if(err>=max_err) break;
	}
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed: %d errors\r\n"
	if(err>=max_err){
		PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
	}
//****************** erase, write and verify EEPROM ********************
	if(dim2&&err<max_err){
		//EEPROM @ 0x7F(EEbaseAddr)
		PrintMessage(strings[S_EEAreaW]);	//"Write EEPROM ... "
		PrintStatusSetup();
		if(saveLog)	fprintf(logfile,"\nWRITE EEPROM:\n");
		int eewrite=(options&0xf000)>>12;
		if(eewrite==0){		//24FxxKAxx
			bufferU[j++]=SIX;				//MOV 0x4050,W10
			bufferU[j++]=0x24;
			bufferU[j++]=0x05;
			bufferU[j++]=0x0A;
			bufferU[j++]=SIX;				//MOV W10,NVMCON
			bufferU[j++]=0x88;
			bufferU[j++]=0x3B;
			bufferU[j++]=0x0A;
			bufferU[j++]=SIX;				//MOV 0x7F,W0
			bufferU[j++]=0x20;
			bufferU[j++]=0x07;
			bufferU[j++]=0xF0;
			bufferU[j++]=SIX;				//MOV W0,TABLPAG
			bufferU[j++]=0x88;
			if(newTABLPAG){
				bufferU[j++]=0x02;
				bufferU[j++]=0xA0;
			}
			else{
				bufferU[j++]=0x01;
				bufferU[j++]=0x90;
			}
			bufferU[j++]=SIX;				//MOV EEbaseAddr,W0
			bufferU[j++]=0x2F;
			bufferU[j++]=EEbaseAddr>>4;
			bufferU[j++]=0x00;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=SIX;				//TBLWTL W0,[W0]
			bufferU[j++]=0xBB;
			bufferU[j++]=0x08;
			bufferU[j++]=0x00;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=ICSP_NOP;/**/
	/*		//Erase EEPROM
			bufferU[j++]=SIX;				//BSET NVMCON,#WR
			bufferU[j++]=0xA8;
			bufferU[j++]=0xE7;
			bufferU[j++]=0x61;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=WAIT_T3;
			bufferU[j++]=WAIT_T3;
			bufferU[j++]=WAIT_T3;
			bufferU[j++]=SIX;				//MOV 0x4004,W10
			bufferU[j++]=0x24;
			bufferU[j++]=0x00;
			bufferU[j++]=0x4A;
			bufferU[j++]=SIX;				//MOV W10,NVMCON
			bufferU[j++]=0x88;
			bufferU[j++]=0x3B;
			bufferU[j++]=0x0A;
			bufferU[j++]=SIX;				//GOTO 0x200
			bufferU[j++]=0x04;
			bufferU[j++]=0x02;
			bufferU[j++]=0x00;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			writeP();
			msDelay(9);
			readP();
			j=1;
			if(saveLog)WriteLogIO();
		}
		else if(eewrite==2){		//separate erase
			bufferU[j++]=SIX_N;
			bufferU[j++]=6;
			bufferU[j++]=0x24;				//MOV 4046,W10
			bufferU[j++]=0x04;
			bufferU[j++]=0x6A;
			bufferU[j++]=0x88;				//MOV W10,NVMCON
			bufferU[j++]=0x3B;
			bufferU[j++]=0x0A;
			bufferU[j++]=0x20;				//MOV 0x55,W8
			bufferU[j++]=0x05;
			bufferU[j++]=0x58;
			bufferU[j++]=0x88;				//MOV W8,NVMKEY
			bufferU[j++]=0x3B;
			bufferU[j++]=0x38;
			bufferU[j++]=0x20;				//MOV 0xAA,W8
			bufferU[j++]=0x0A;
			bufferU[j++]=0xA8;
			bufferU[j++]=0x88;				//MOV W8,NVMKEY
			bufferU[j++]=0x3B;
			bufferU[j++]=0x38;
			bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
			bufferU[j++]=0xA8;
			bufferU[j++]=0xE7;
			bufferU[j++]=0x61;
			bufferU[j++]=WAIT_T3;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=SIX_LONG;				//BCLR NVMCON,#WR
			bufferU[j++]=0xA9;
			bufferU[j++]=0xE7;
			bufferU[j++]=0x61;
			bufferU[j++]=SIX_N;
			bufferU[j++]=6;
			bufferU[j++]=0x24;				//MOV 4056,W10
			bufferU[j++]=0x05;
			bufferU[j++]=0x6A;
			bufferU[j++]=0x88;				//MOV W10,NVMCON
			bufferU[j++]=0x3B;
			bufferU[j++]=0x0A;
			bufferU[j++]=0x20;				//MOV 0x55,W8
			bufferU[j++]=0x05;
			bufferU[j++]=0x58;
			bufferU[j++]=0x88;				//MOV W8,NVMKEY
			bufferU[j++]=0x3B;
			bufferU[j++]=0x38;
			bufferU[j++]=0x20;				//MOV 0xAA,W8
			bufferU[j++]=0x0A;
			bufferU[j++]=0xA8;
			bufferU[j++]=0x88;				//MOV W8,NVMKEY
			bufferU[j++]=0x3B;
			bufferU[j++]=0x38;
			bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
			bufferU[j++]=0xA8;
			bufferU[j++]=0xE7;
			bufferU[j++]=0x61;
			bufferU[j++]=WAIT_T3;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=SIX_LONG;				//BCLR NVMCON,#WR
			bufferU[j++]=0xA9;
			bufferU[j++]=0xE7;
			bufferU[j++]=0x61;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			writeP();
			msDelay(6);
			readP();
			j=1;
			if(saveLog)WriteLogIO();
			bufferU[j++]=SIX_N;
			bufferU[j++]=6;
			bufferU[j++]=0x24;				//MOV 4066,W10
			bufferU[j++]=0x06;
			bufferU[j++]=0x6A;
			bufferU[j++]=0x88;				//MOV W10,NVMCON
			bufferU[j++]=0x3B;
			bufferU[j++]=0x0A;
			bufferU[j++]=0x20;				//MOV 0x55,W8
			bufferU[j++]=0x05;
			bufferU[j++]=0x58;
			bufferU[j++]=0x88;				//MOV W8,NVMKEY
			bufferU[j++]=0x3B;
			bufferU[j++]=0x38;
			bufferU[j++]=0x20;				//MOV 0xAA,W8
			bufferU[j++]=0x0A;
			bufferU[j++]=0xA8;
			bufferU[j++]=0x88;				//MOV W8,NVMKEY
			bufferU[j++]=0x3B;
			bufferU[j++]=0x38;
			bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
			bufferU[j++]=0xA8;
			bufferU[j++]=0xE7;
			bufferU[j++]=0x61;
			bufferU[j++]=WAIT_T3;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=SIX_LONG;				//BCLR NVMCON,#WR
			bufferU[j++]=0xA9;
			bufferU[j++]=0xE7;
			bufferU[j++]=0x61;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			writeP();
			msDelay(4);
			readP();
			j=1;
			if(saveLog)WriteLogIO();
		}
		else if(eewrite==1){		//30Fxx
			bufferU[j++]=SIX;				//MOV 0x7F,W0
			bufferU[j++]=0x20;
			bufferU[j++]=0x07;
			bufferU[j++]=0xF0;
			bufferU[j++]=SIX;				//MOV W0,TABLPAG
			bufferU[j++]=0x88;
			if(newTABLPAG){
				bufferU[j++]=0x02;
				bufferU[j++]=0xA0;
			}
			else{
				bufferU[j++]=0x01;
				bufferU[j++]=0x90;
			}
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			writeP();
			msDelay(1);
			readP();
			j=1;
			if(saveLog)WriteLogIO();
		}
		//Write EEPROM
		for(k2=0,i=0x1000-dim2;i<0x1000;i+=2){	//write 1 word (2 bytes)
			if(memEE[i]<0xFF||memEE[i+1]<0xFF){
			bufferU[j++]=SIX;				//MOV i,W7
				bufferU[j++]=0x2F;
				bufferU[j++]=i>>4;
				bufferU[j++]=((i<<4)&0xF0)+7;
				bufferU[j++]=SIX;				//MOV XXXX,W0
				bufferU[j++]=0x20+((memEE[i+1]>>4)&0xF);
				bufferU[j++]=((memEE[i+1]<<4)&0xF0)+((memEE[i]>>4)&0xF);
				bufferU[j++]=(memEE[i]<<4)&0xF0;
				bufferU[j++]=ICSP_NOP;
				bufferU[j++]=SIX_LONG;				//TBLWTL W0,[W7++]
				bufferU[j++]=0xBB;
				bufferU[j++]=0x1B;
				bufferU[j++]=0x80;
				if(eewrite==0){		//24FxxKAxx
					bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
					bufferU[j++]=0xA8;
					bufferU[j++]=0xE7;
					bufferU[j++]=0x61;
				}
				else if(eewrite==1){		//30Fxxxx
					bufferU[j++]=SIX_N;
					bufferU[j++]=6;
					bufferU[j++]=0x24;				//MOV 0x4004,W10
					bufferU[j++]=0x00;
					bufferU[j++]=0x4A;
					bufferU[j++]=0x88;				//MOV W10,NVMCON
					bufferU[j++]=0x3B;
					bufferU[j++]=0x0A;
					bufferU[j++]=0x20;				//MOV 0x55,W8
					bufferU[j++]=0x05;
					bufferU[j++]=0x58;
					bufferU[j++]=0x88;				//MOV W8,NVMKEY
					bufferU[j++]=0x3B;
					bufferU[j++]=0x38;
					bufferU[j++]=0x20;				//MOV 0xAA,W8
					bufferU[j++]=0x0A;
					bufferU[j++]=0xA8;
					bufferU[j++]=0x88;				//MOV W8,NVMKEY
					bufferU[j++]=0x3B;
					bufferU[j++]=0x38;
					bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
					bufferU[j++]=0xA8;
					bufferU[j++]=0xE7;
					bufferU[j++]=0x61;
					bufferU[j++]=WAIT_T3;
					bufferU[j++]=ICSP_NOP;
					bufferU[j++]=ICSP_NOP;
					bufferU[j++]=SIX_LONG;				//BCLR NVMCON,#WR
					bufferU[j++]=0xA9;
					bufferU[j++]=0xE7;
					bufferU[j++]=0x61;
				}
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				writeP();
				msDelay(wait+2);		//write delay
				readP();
				j=1;
				PrintStatus(strings[S_CodeWriting],(i-0x1000+dim2)*100/(dim2),i);	//"Scrittura: %d%%, ind. %03X"
				if(RWstop) i=dim;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d, k=%d 0=%d\n"
					WriteLogIO();
				}
			}
		}
		//Verify EEPROM
		if(saveLog)	fprintf(logfile,"\nVERIFY EEPROM:\n");
		bufferU[j++]=SIX;				//MOV 0xFE00,W6
		bufferU[j++]=0x2F;
		bufferU[j++]=EEbaseAddr>>4;
		bufferU[j++]=0x06;
		bufferU[j++]=SIX;				//MOV #VISI,W7
		bufferU[j++]=0x20;
		bufferU[j++]=0x78;
		bufferU[j++]=0x47;
		bufferU[j++]=SIX;				//GOTO 0x200
		bufferU[j++]=0x04;
		bufferU[j++]=0x02;
		bufferU[j++]=0x00;
		bufferU[j++]=ICSP_NOP;
		int errE=0;
		for(i=k2=EEbaseAddr;i<EEbaseAddr+dim2;i+=2){
			bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
			bufferU[j++]=0xBA;
			bufferU[j++]=0x0B;
			bufferU[j++]=0xB6;
			bufferU[j++]=REGOUT;
			if(j>DIMBUF-7||i==dim2-4){
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				writeP();
				msDelay(3);
				readP();
				for(z=1;z<DIMBUF-2;z++){
					if(bufferI[z]==REGOUT){
						CheckData(memEE[k2],bufferI[z+2],i,&errE);
						CheckData(memEE[k2+1],bufferI[z+1],i+1,&errE);
						z+=3;
						k2+=2;
					}
				}
				PrintStatus(strings[S_CodeReading],(i-EEbaseAddr)*100/(dim2),i);	//"Read: %d%%, addr. %03X"
				if(RWstop) i=EEbaseAddr+dim2;
				j=1;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
					WriteLogIO();
				}
			}
		}
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],errE);	//"completed: %d errors \r\n"
		err+=errE;
		PrintStatusClear();
	}
//****************** write CONFIG ********************
	int written, read;
	j=1;
	if(config>2&&config<5&&err<max_err){	//config area @ 0xF80000
		PrintMessage(strings[S_ConfigW]);	//"Write CONFIG ..."
		if(saveLog)	fprintf(logfile,"\nWRITE CONFIG:\n");
		int confword=(options&0xF0000)>>16;
		bufferU[j++]=SIX_N;
		bufferU[j++]=6;
		bufferU[j++]=0x20;				//MOV 0xF8,W0
		bufferU[j++]=0x0F;
		bufferU[j++]=0x80;
		bufferU[j++]=0x88;				//MOV W0,TABLPAG
		if(newTABLPAG){
			bufferU[j++]=0x02;
			bufferU[j++]=0xA0;
		}
		else{
			bufferU[j++]=0x01;
			bufferU[j++]=0x90;
		}
		bufferU[j++]=0x24;				//MOV 0x400x,W10
		bufferU[j++]=0x00;
		if(confword==0)bufferU[j++]=0x0A;		//0x4000
		else if(confword==1)bufferU[j++]=0x3A;	//0x4003
		else if(confword==2)bufferU[j++]=0x4A;	//0x4004
		else if(confword==3)bufferU[j++]=0x8A;	//0x4008
		bufferU[j++]=0x88;				//MOV W10,NVMCON
		bufferU[j++]=0x3B;
		bufferU[j++]=0x0A;
		bufferU[j++]=0x20;				//MOV 0,W7
		bufferU[j++]=0x00;
		bufferU[j++]=0x07;
		bufferU[j++]=0x04;				//GOTO 0x200
		bufferU[j++]=0x02;
		bufferU[j++]=0x00;
		bufferU[j++]=ICSP_NOP;
		for(i=0;i<12;i++){
			//Write CONFIG
			bufferU[j++]=SIX;				//MOV XXXX,W0
			bufferU[j++]=0x20;
			bufferU[j++]=(memCONFIG[i*4]>>4)&0xF;
			bufferU[j++]=(memCONFIG[i*4]<<4)&0xF0;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=SIX_LONG;				//TBLWTL W0,[W7++]
			bufferU[j++]=0xBB;
			bufferU[j++]=0x1B;
			bufferU[j++]=0x80;
			//if(memCONFIG[i*4]<0xFF){			//write if not empty
			bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
			bufferU[j++]=0xA8;
			bufferU[j++]=0xE7;
			bufferU[j++]=0x61;
				bufferU[j++]=ICSP_NOP;
				bufferU[j++]=ICSP_NOP;
			//}
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			writeP();
			msDelay(27);
			readP();
			j=1;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
				WriteLogIO();
			}
		}
		//Verify write
		int errC=0;
		bufferU[j++]=SIX;
		bufferU[j++]=0x20;				//MOV XXXX,W6
		bufferU[j++]=0x00;
		bufferU[j++]=0x06;
		bufferU[j++]=SIX;
		bufferU[j++]=0x20;				//MOV #VISI,W7
		bufferU[j++]=0x78;
		bufferU[j++]=0x47;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		writeP();
		msDelay(3);
		readP();
		j=1;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],0xF80000,0xF80000,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
			WriteLogIO();
		}
		for(i=0,z=1;i<9;i++){
			for(;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			written=memCONFIG[i*4];
			read=bufferI[z+2];								//Low byte
			if(~written&read)CheckData(written,read,0xF80000+i*2,&errC);
			z+=3;
		}
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		writeP();
		msDelay(3);
		readP();
		j=1;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],0xF80000,0xF80000,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
			WriteLogIO();
		}
		for(z=1;i<12;i++){
			for(;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			written=memCONFIG[i*4];
			read=bufferI[z+2];								//Low byte
			if(~written&read)CheckData(written,read,0xF80000+i*2,&errC);
			z+=3;
		}
		PrintMessage1(strings[S_ComplErr],errC);	//"completed: %d errors \r\n"
		PrintStatusClear();
		err+=errC;
	}
	else if(config>=5&&err<max_err){	//16 bit config area (30Fxxxx)
		PrintMessage(strings[S_ConfigW]);	//"Write CONFIG ..."
		if(saveLog)	fprintf(logfile,"\nWRITE CONFIG:\n");
		int Nconf=config==5?7:8;
		bufferU[j++]=SIX_N;
		bufferU[j++]=4;
		bufferU[j++]=0x20;				//MOV 0xF8,W0
		bufferU[j++]=0x0F;
		bufferU[j++]=0x80;
		bufferU[j++]=0x88;				//MOV W0,TABLPAG
		if(newTABLPAG){
			bufferU[j++]=0x02;
			bufferU[j++]=0xA0;
		}
		else{
			bufferU[j++]=0x01;
			bufferU[j++]=0x90;
		}
		bufferU[j++]=0x20;				//MOV 0,W7
		bufferU[j++]=0x00;
		bufferU[j++]=0x07;
		bufferU[j++]=0x04;				//GOTO 0x200
		bufferU[j++]=0x02;
		bufferU[j++]=0x00;
		bufferU[j++]=ICSP_NOP;
		for(i=0;i<Nconf;i++){
			//Erase CONFIG
			bufferU[j++]=SIX;				//MOV 0xFFFF,W0
			bufferU[j++]=0x2F;
			bufferU[j++]=0xFF;
			bufferU[j++]=0xF0;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=SIX_LONG;				//TBLWTL W0,[W7++]
		bufferU[j++]=0xBB;
		bufferU[j++]=0x1B;
		bufferU[j++]=0x80;
			bufferU[j++]=SIX_N;
			bufferU[j++]=6;
			bufferU[j++]=0x24;				//MOV 0x400x,W10
			bufferU[j++]=0x00;
			bufferU[j++]=0x8A;				//0x4008
			bufferU[j++]=0x88;				//MOV W10,NVMCON
			bufferU[j++]=0x3B;
			bufferU[j++]=0x0A;
			bufferU[j++]=0x20;				//MOV 0x55,W8
			bufferU[j++]=0x05;
			bufferU[j++]=0x58;
			bufferU[j++]=0x88;				//MOV W8,NVMKEY
			bufferU[j++]=0x3B;
			bufferU[j++]=0x38;
			bufferU[j++]=0x20;				//MOV 0xAA,W8
			bufferU[j++]=0x0A;
			bufferU[j++]=0xA8;
			bufferU[j++]=0x88;				//MOV W8,NVMKEY
			bufferU[j++]=0x3B;
			bufferU[j++]=0x38;
		bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
		bufferU[j++]=0xA8;
		bufferU[j++]=0xE7;
		bufferU[j++]=0x61;
		bufferU[j++]=WAIT_T3;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=SIX_LONG;				//BCLR NVMCON,#WR
			bufferU[j++]=0xA9;
			bufferU[j++]=0xE7;
			bufferU[j++]=0x61;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			writeP();
			msDelay(3);
			readP();
			j=1;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
				WriteLogIO();
			}
		}
		bufferU[j++]=SIX;
		bufferU[j++]=0x20;				//MOV 0,W7
		bufferU[j++]=0x00;
		bufferU[j++]=0x07;
		for(i=0;i<Nconf;i++){
			int value=memCONFIG[i*4]+(memCONFIG[i*4+1]<<8);
			//Write CONFIG
			bufferU[j++]=SIX;				//MOV XXXX,W0
			bufferU[j++]=0x20+(value>>12);
			bufferU[j++]=(value>>4);
			bufferU[j++]=(value<<4);
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=SIX_LONG;				//TBLWTL W0,[W7++]
			bufferU[j++]=0xBB;
			bufferU[j++]=0x1B;
			bufferU[j++]=0x80;
			if(value<0xFFFF){			//write if not empty
				bufferU[j++]=SIX_N;
				bufferU[j++]=6;
				bufferU[j++]=0x24;				//MOV 0x400x,W10
				bufferU[j++]=0x00;
				bufferU[j++]=0x8A;				//0x4008
				bufferU[j++]=0x88;				//MOV W10,NVMCON
				bufferU[j++]=0x3B;
				bufferU[j++]=0x0A;
				bufferU[j++]=0x20;				//MOV 0x55,W8
				bufferU[j++]=0x05;
				bufferU[j++]=0x58;
				bufferU[j++]=0x88;				//MOV W8,NVMKEY
				bufferU[j++]=0x3B;
				bufferU[j++]=0x38;
				bufferU[j++]=0x20;				//MOV 0xAA,W8
				bufferU[j++]=0x0A;
				bufferU[j++]=0xA8;
				bufferU[j++]=0x88;				//MOV W8,NVMKEY
				bufferU[j++]=0x3B;
				bufferU[j++]=0x38;
				bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
				bufferU[j++]=0xA8;
				bufferU[j++]=0xE7;
				bufferU[j++]=0x61;
				bufferU[j++]=WAIT_T3;
				bufferU[j++]=ICSP_NOP;
				bufferU[j++]=ICSP_NOP;
				bufferU[j++]=SIX_LONG;				//BCLR NVMCON,#WR
				bufferU[j++]=0xA9;
				bufferU[j++]=0xE7;
				bufferU[j++]=0x61;
			}
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			writeP();
			msDelay(3);
			readP();
			j=1;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
				WriteLogIO();
			}
		}
		//Verify write
		int errC=0;
		bufferU[j++]=SIX;
		bufferU[j++]=0x20;				//MOV XXXX,W6
		bufferU[j++]=0x00;
		bufferU[j++]=0x06;
		bufferU[j++]=SIX;
		bufferU[j++]=0x20;				//MOV #VISI,W7
		bufferU[j++]=0x78;
		bufferU[j++]=0x47;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		writeP();
		msDelay(3);
		readP();
		j=1;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],0xF80000,0xF80000,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
			WriteLogIO();
		}
		for(i=0,z=1;i<7;i++){
			for(;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			written=memCONFIG[i*4+1]+(memCONFIG[i*4]<<8);
			read=bufferI[z+1]+(bufferI[z+2]<<8);
			if(~written&read)CheckData(written,read,0xF80000+i*2,&errC);	//16 bit
			z+=3;
		}
		PrintMessage1(strings[S_ComplErr],errC);	//"completed: %d errors \r\n"
		PrintStatusClear();
		err+=errC;
	}
//****************** exit ********************
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	writeP();
	msDelay(2);
	readP();
	if(saveLog)WriteLogIO();
	j=1;
	unsigned int stop=GetTickCount();
	PrintStatusClear();
	PrintMessage3(strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	if(saveLog) CloseLogFile();
}
*/
