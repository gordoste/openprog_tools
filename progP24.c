/**
 * \file progP24F.c - algorithms to program the PIC24 family of microcontrollers
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

/*
                ICSP  Code(KWord) EE(B) Config     Code_er Row Row_wr EE_er EE_wr Conf_wr wr_seq
24F0xKAxxx      HV|LV  1.375-5.5   512  00-10(8b)   4064   32  4004   4050  4004   4004    NO
24FJxxGA0xx     LV     5.5-44      NO   Code-2(16b) 404F   64  4001   -     -      4003    NO
24FJxxGA1x/GB0x LV     11-22       NO   Code-4(16b) 404F   64  4001   -     -      4003    NO
xDA/GB2/GA3/GC   "     22-87                   "            "                 "
(new TABLPAG)
24FJxxGB1x      LV     22-87       NO   Code-3(16b) 404F   64  4001   -     -      4003    NO
24HJ-33FJ       LV      2-88       NO  00-12/16(8b) 404F   64  4001   -     -      4000    NO
24E-33E         LV     11-88       NO   Code-20(8b) 400F    2  4001   -     -      4001   55-AA
(new TABLPAG, new algorithm)
30Fxx10-16      HV2     4-48      0-4K  00-0C(16b)  407F   32  4001   seq.  4004   4008   55-AA
30F2020-23      LV5V    2-4        NO   00-0E(16b)  407F   32  4001   -     -      4008   55-AA

*/

#include "common.h"

void CheckData(int a,int b,int addr,int *err)
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

struct ID24{
	int id;
	char *device;
} PIC24LIST[]={
	{0x0040,"30F2010"},
	{0x0041,"24HJ64GP206"},
	{0x0047,"24HJ64GP210"},
	{0x0049,"24HJ64GP506"},
	{0x004B,"24HJ64GP510"},
	{0x005D,"24HJ128GP206"},
	{0x005F,"24HJ128GP210"},
	{0x0061,"24HJ128GP506"},
	{0x0063,"24HJ128GP510"},
	{0x0065,"24HJ128GP306"},
	{0x0067,"24HJ128GP310"},
	{0x0071,"24HJ256GP206"},
	{0x0073,"24HJ256GP210"},
	{0x007B,"24HJ256GP610"},
	{0x0080,"30F5011"},
	{0x0081,"30F5013"},
	{0x0089,"33FJ64MC506"},
	{0x008A,"33FJ64MC508"},
	{0x008B,"33FJ64MC510"},
	{0x0091,"33FJ64MC706"},
	{0x0097,"33FJ64MC710"},
	{0x00A1,"33FJ128MC506"},
	{0x00A3,"33FJ128MC510"},
	{0x00A9,"33FJ128MC706"},
	{0x00AE,"33FJ128MC708"},
	{0x00AF,"33FJ128MC710"},
	{0x00B7,"33FJ256MC510"},
	{0x00BF,"33FJ256MC710"},
	{0x00C1,"30F3012/33FJ64GP206"},
	{0x00C3,"30F3013"},
	{0x00CD,"33FJ64GP306"},
	{0x00CF,"33FJ64GP310"},
	{0x00D5,"33FJ64GP706"},
	{0x00D6,"33FJ64GP708"},
	{0x00D7,"33FJ64GP710"},
	{0x00D9,"33FJ128GP206"},
	{0x00E5,"33FJ128GP306"},
	{0x00E7,"33FJ128GP310"},
	{0x00ED,"33FJ128GP706"},
	{0x00EE,"33FJ128GP708"},
	{0x00EF,"33FJ128GP710"},
	{0x00F5,"33FJ256GP506"},
	{0x00F7,"33FJ256GP510"},
	{0x00FF,"33FJ256GP710"},
	{0x0100,"30F4012"},
	{0x0101,"30F4011"},
	{0x0141,"30F4013"},
	{0x0160,"30F3014"},
	{0x0188,"30F6010"},
	{0x0192,"30F6011"},
	{0x0193,"30F6012"},
	{0x0197,"30F6013"},
	{0x0198,"30F6014"},
	{0x01C0,"30F3010"},
	{0x01C1,"30F3011"},
	{0x0200,"30F5015"},
	{0x0201,"30F5016"},
	{0x0240,"30F2011"},
	{0x0241,"30F2012"},
	{0x0280,"30F6015"},
	{0x0281,"30F6010A"},
	{0x02C0,"30F6011A"},
	{0x02C1,"30F6013A"},
	{0x02C2,"30F6012A"},
	{0x02C3,"30F6014A"},
	{0x0400,"30F2020"},
	{0x0403,"30F2023"},
	{0x0404,"30F1010"},
	{0x0405,"24FJ64GA006"},
	{0x0406,"24FJ96GA006"},
	{0x0407,"24FJ128GA006"},
	{0x0408,"24FJ64GA008"},
	{0x0409,"24FJ96GA008"},
	{0x040A,"24FJ128GA008"},
	{0x040B,"24FJ64GA010"},
	{0x040C,"24FJ96GA010"},
	{0x040D,"24FJ128GA010"},
	{0x0444,"24FJ16GA002"},
	{0x0445,"24FJ32GA002"},
	{0x0446,"24FJ48GA002"},
	{0x0447,"24FJ64GA002"},
	{0x044C,"24FJ16GA004"},
	{0x044D,"24FJ32GA004"},
	{0x044E,"24FJ48GA004"},
	{0x044F,"24FJ64GA004"},
	{0x0601,"33FJ32MC302"},
	{0x0603,"33FJ32MC304"},
	{0x0605,"33FJ32GP302"},
	{0x0607,"33FJ32GP304"},
	{0x0611,"33FJ64MC202"},
	{0x0613,"33FJ64MC204"},
	{0x0615,"33FJ64GP202"},
	{0x0617,"33FJ64GP204"},
	{0x0619,"33FJ64MC802"},
	{0x061B,"33FJ64MC804"},
	{0x061D,"33FJ64GP802"},
	{0x061F,"33FJ64GP804"},
	{0x0621,"33FJ128MC202"},
	{0x0623,"33FJ128MC204"},
	{0x0625,"33FJ128GP202"},
	{0x0627,"33FJ128GP204"},
	{0x0629,"33FJ128MC802"},
	{0x062B,"33FJ128MC804"},
	{0x062D,"33FJ128GP802"},
	{0x062F,"33FJ128GP804"},
	{0x0645,"24HJ32GP302"},
	{0x0647,"24HJ32GP304"},
	{0x0655,"24HJ64GP202"},
	{0x0657,"24HJ64GP204"},
	{0x0665,"24HJ128GP202"},
	{0x0667,"24HJ128GP204"},
	{0x0675,"24HJ64GP502"},
	{0x0677,"24HJ64GP504"},
	{0x067D,"24HJ128GP502"},
	{0x067F,"24HJ128GP504"},
	{0x0771,"24HJ256GP206A"},
	{0x0773,"24HJ256GP210A"},
	{0x077B,"24HJ256GP610A"},
	{0x07B7,"33FJ256MC510A"},
	{0x07BF,"33FJ256MC710A"},
	{0x07F5,"33FJ256GP506A"},
	{0x07F7,"33FJ256GP510A"},
	{0x07FF,"33FJ256GP710A"},
	{0x0800,"33FJ12MC201"},
	{0x0801,"33FJ12MC202"},
	{0x0802,"33FJ12GP201"},
	{0x0803,"33FJ12GP202"},
	{0x080A,"24HJ12GP201"},
	{0x080B,"24HJ12GP202"},
	{0x0C00,"33FJ06GS101"},
	{0x0C01,"33FJ06GS102"},
	{0x0C02,"33FJ06GS202"},
	{0x0C03,"33FJ16GS502"},
	{0x0C04,"33FJ16GS402"},
	{0x0C05,"33FJ16GS504"},
	{0x0C06,"33FJ16GS404"},
	{0x0D00,"24F04KA201"},
	{0x0D01,"24F16KA101"},
	{0x0D02,"24F04KA200"},
	{0x0D03,"24F16KA102"},
	{0x0D08,"24F08KA101"},
	{0x0D0A,"24F08KA102"},
	{0x0F03,"33FJ16MC304"},
	{0x0F07,"33FJ16GP304"},
	{0x0F09,"33FJ32MC202"},
	{0x0F0B,"33FJ32MC204"},
	{0x0F0D,"33FJ32GP202"},
	{0x0F0F,"33FJ32GP204"},
	{0x0F17,"24HJ16GP304"},
	{0x0F1D,"24HJ32GP202"},
	{0x0F1F,"24HJ32GP204"},
	{0x1001,"24FJ64GB106"},
	{0x1003,"24FJ64GB108"},
	{0x1007,"24FJ64GB110"},
	{0x1008,"24FJ128GA106"},
	{0x1009,"24FJ128GB106"},
	{0x100A,"24FJ128GA100"},
	{0x100B,"24FJ128GB108"},
	{0x100E,"24FJ128GA110"},
	{0x100F,"24FJ128GB110"},
	{0x1010,"24FJ192GA106"},
	{0x1011,"24FJ192GB106"},
	{0x1012,"24FJ192GA108"},
	{0x1013,"24FJ192GB108"},
	{0x1016,"24FJ192GA110"},
	{0x1017,"24FJ192GB110"},
	{0x1018,"24FJ256GA106"},
	{0x1019,"24FJ256GB106"},
	{0x101A,"24FJ256GA108"},
	{0x101B,"24FJ256GB108"},
	{0x101E,"24FJ256GA110"},
	{0x101F,"24FJ256GB110"},
	{0x1780,"33EP512MC204"},
	{0x1781,"33EP512MC202"},
	{0x1783,"33EP512MC206"},
	{0x1784,"33EP512MC504"},
	{0x1785,"33EP512MC502"},
	{0x1787,"33EP512MC506"},
	{0x178F,"33EP512GP506"},
	{0x1790,"24EP512MC204"},
	{0x1791,"24EP512MC202"},
	{0x1793,"24EP512MC206"},
	{0x1798,"24EP512GP204"},
	{0x1799,"24EP512GP202"},
	{0x179B,"24EP512GP206"},
	{0x178C,"33EP512GP504"},
	{0x178D,"33EP512GP502"},
	{0x1C00,"33EP32MC204"},
	{0x1C01,"33EP32MC202"},
	{0x1C02,"33EP32MC203"},
	{0x1C04,"33EP32MC504"},
	{0x1C05,"33EP32MC502"},
	{0x1C06,"33EP32MC503"},
	{0x1C0C,"33EP32GP504"},
	{0x1C0D,"33EP32GP502"},
	{0x1C0E,"33EP32GP503"},
	{0x1C10,"24EP32MC204"},
	{0x1C11,"24EP32MC202"},
	{0x1C12,"24EP32MC203"},
	{0x1C18,"24EP32GP204"},
	{0x1C19,"24EP32GP202"},
	{0x1C1A,"24EP32GP203"},
	{0x1D20,"33EP64MC204"},
	{0x1D21,"33EP64MC202"},
	{0x1D22,"33EP64MC203"},
	{0x1D23,"33EP64MC206"},
	{0x1D24,"33EP64MC504"},
	{0x1D25,"33EP64MC502"},
	{0x1D26,"33EP64MC503"},
	{0x1D27,"33EP64MC506"},
	{0x1D2C,"33EP64GP504"},
	{0x1D2D,"33EP64GP502"},
	{0x1D2E,"33EP64GP503"},
	{0x1D2F,"33EP64GP506"},
	{0x1D30,"24EP64MC204"},
	{0x1D31,"24EP64MC202"},
	{0x1D32,"24EP64MC203"},
	{0x1D33,"24EP64MC206"},
	{0x1D38,"24EP64GP204"},
	{0x1D39,"24EP64GP202"},
	{0x1D3A,"24EP64GP203"},
	{0x1D3B,"24EP64GP206"},
 	{0x1E40,"33EP128MC204"},
 	{0x1E41,"33EP128MC202"},
 	{0x1E43,"33EP128MC206"},
 	{0x1E44,"33EP128MC504"},
 	{0x1E45,"33EP128MC502"},
 	{0x1E47,"33EP128MC506"},
 	{0x1E4C,"33EP128GP504"},
 	{0x1E4D,"33EP128GP502"},
 	{0x1E4F,"33EP128GP506"},
 	{0x1E50,"24EP128MC204"},
 	{0x1E51,"24EP128MC202"},
 	{0x1E53,"24EP128MC206"},
 	{0x1E58,"24EP128GP204"},
 	{0x1E59,"24EP128GP202"},
 	{0x1E5B,"24EP128GP206"},
 	{0x1F60,"33EP256MC204"},
 	{0x1F61,"33EP256MC202"},
 	{0x1F63,"33EP256MC206"},
 	{0x1F64,"33EP256MC504"},
 	{0x1F65,"33EP256MC502"},
 	{0x1F67,"33EP256MC506"},
 	{0x1F6C,"33EP256GP504"},
 	{0x1F6D,"33EP256GP502"},
 	{0x1F6F,"33EP256GP506"},
 	{0x1F70,"24EP256MC204"},
 	{0x1F71,"24EP256MC202"},
 	{0x1F73,"24EP256MC206"},
 	{0x1F78,"24EP256GP204"},
 	{0x1F79,"24EP256GP202"},
 	{0x1F7B,"24EP256GP206"},
	{0x4000,"33FJ32GS406"},
	{0x4001,"33FJ64GS406"},
	{0x4002,"33FJ32GS606"},
	{0x4003,"33FJ64GS606"},
	{0x4004,"33FJ32GS608"},
	{0x4005,"33FJ64GS608"},
	{0x4008,"33FJ32GS610"},
	{0x4009,"33FJ64GS610"},
	{0x4100,"24FJ128GB206"},
	{0x4102,"24FJ128GB210"},
	{0x4104,"24FJ256GB206"},
	{0x4106,"24FJ256GB210"},
	{0x4108,"24FJ128DA206"},
	{0x4109,"24FJ128DA106"},
	{0x410A,"24FJ128DA210"},
	{0x410B,"24FJ128DA110"},
	{0x410C,"24FJ256DA206"},
	{0x410D,"24FJ256DA106"},
	{0x410E,"24FJ256DA210"},
	{0x410F,"24FJ256DA110"},
	{0x4202,"24FJ32GA102"},
	{0x4203,"24FJ32GB002"},
	{0x4206,"24FJ64GA102"},
	{0x4207,"24FJ64GB002"},
	{0x420A,"24FJ32GA104"},
	{0x420B,"24FJ32GB004"},
	{0x420E,"24FJ64GA104"},
	{0x420F,"24FJ64GB004"},
	{0x46C0,"24FJ64GA306"},
	{0x46C2,"24FJ128GA306"},
	{0x46C4,"24FJ64GA308"},
	{0x46C6,"24FJ128GA308"},
	{0x46C8,"24FJ64GA310"},
	{0x46CA,"24FJ128GA310"},
	{0x4884,"24FJ64GC010"},
	{0x4885,"24FJ128GC010"},
	{0x4888,"24FJ64GC006"},
	{0x4889,"24FJ128GC006"},
	{0x488A,"24FJ64GC008"},
	{0x488B,"24FJ128GC008"},
};

void PIC24_ID(int id)
{
	char s[64];
	int i;
	for(i=0;i<sizeof(PIC24LIST)/sizeof(PIC24LIST[0]);i++){
		if(id==PIC24LIST[i].id){
			strcpy(s,PIC24LIST[i].device);
			strcat(s,"\r\n");
			PrintMessage(s);
			return;
		}
	}
	PrintMessage(strings[S_nodev]); //"Unknown device\r\n");
}

void DisplayCODE24F(int dim){
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

void DisplayEE24F(){
// display EE memory with address offset by 0x7FF000
	char s[256],t[256],v[256],*aux;
	int valid=0,empty=1;
	int i,j;
	s[0]=0;
	v[0]=0;
	aux=(char*)malloc(0x1000/COL*(16+COL*5));
	aux[0]=0;
	for(i=0;i<0x1000&&i<sizeEE;i+=COL){
		valid=0;
		for(j=i;j<i+COL&&j<0x1000&&i<sizeEE;j+=2){
			sprintf(t,"%02X %02X ",memEE[j],memEE[j+1]);
			strcat(s,t);
			sprintf(t,"%c",isprint(memEE[j])&&(memEE[j]<0xFF)?memEE[j]:'.');
			StrcatConvert(v,t);
			if(memEE[j]<0xff) valid=1;
			sprintf(t,"%c",isprint(memEE[j+1])&&(memEE[j+1]<0xFF)?memEE[j+1]:'.');
			StrcatConvert(v,t);
			if(memEE[j+1]<0xff) valid=1;
		}
		if(valid){
			sprintf(t,"%04X: %s %s\r\n",i+0xF000,s,v);		//back to the device address
			strcat(aux,t);
			empty=0;
		}
		s[0]=0;
		v[0]=0;
	}
	if(empty) PrintMessage(strings[S_Empty]);	//empty
	else PrintMessage(aux);
	free(aux);
}

void Read24Fx(int dim,int dim2,int options,int appIDaddr,int executiveArea){
// read 16 bit PIC 24Fxxxx
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
//	bit [8]
//	   0 = standard TABLPAG address
//	   1 = new TABLPAG address
// appIDaddr = application ID word lower address (high is 0x80)
// executiveArea = size of executive area (16 bit words, starting at 0x800000)
	int k=0,k2=0,z=0,i,j;
	int entry=options&0xF;
	int config=(options>>4)&0xF;
	int EEbaseAddr=0x1000-dim2;
	int newTABLPAG=options&0x100?1:0;
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
		fprintf(logfile,"Read24Fx(%d,%d,%d,%d,%d)    (0x%X,0x%X,0x%X,0x%X,0x%X)\n",dim,dim2,options,appIDaddr,executiveArea,dim,dim2,options,appIDaddr,executiveArea);
	}
	dim*=2;		//from words to bytes
	if(dim>0x80000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	if(dim2>0x4000||dim2<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	unsigned char *memExec=0;
	executiveArea*=2;
	if(executiveArea) memExec=(unsigned char *)malloc(executiveArea);
	for(i=0;i<executiveArea;i++) memExec[i]=0xFF;
	size=dim;
	sizeEE=0x1000;
	if(memCODE) free(memCODE);
	memCODE=(unsigned char*)malloc(size);		//CODE
	if(memEE) free(memEE);
	memEE=(unsigned char*)malloc(0x1000);		//EEPROM
	memset(memCODE,0xFF,dim);
	memset(memEE,0xFF,0x1000);
	if(config>2){					//only if separate config area
		sizeCONFIG=48;
		for(i=0;i<48;i++) memCONFIG[i]=0xFF;
	}
	if((entry==1)||(entry==2)){			//High voltage programming: 3.3V + 1.5V + R drop + margin
		if(!StartHVReg(entry==2?12:6)){	//12V only for 30Fxx !!!
			PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
			return;
		}
	}
	else StartHVReg(-1);		//LVP: current limited to (5-0.7-3.6)/10k = 50uA
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=10000>>8;
	bufferU[j++]=10000&0xff;
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
	bufferU[j++]=WAIT_T3;	//>10ms before key (GA3xx-GC0xx)
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
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(55);
	j=0;
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
	PacketIO(2);
	j=0;
	int w0=0,w1=0;
	for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
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
	bufferU[j++]=SIX_N;
	bufferU[j++]=4;
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
	bufferU[j++]=0x20;				//MOV 0,W6
	bufferU[j++]=0x00;
	bufferU[j++]=0x06;
	bufferU[j++]=0x20;				//MOV #VISI,W7
	bufferU[j++]=0x78;
	bufferU[j++]=0x47;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2) w0=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage2("ApplicationID @ 0x80%04X:  0x%04X\r\n",appIDaddr,w0);
//****************** read code ********************
	PrintMessage(strings[S_CodeReading1]);		//code read ...
	PrintStatusSetup();
//Read 6 24 bit words packed in 9 16 bit words
//memory address advances by 24 bytes because of alignment
	int High=0;
	for(i=0;i<dim;i+=24){
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
			PacketIO(2);
			j=0;
			High=i>>17;
		}
		bufferU[j++]=SIX_LONG;				//GOTO 0x200
		bufferU[j++]=0x04;
		bufferU[j++]=0x02;
		bufferU[j++]=0x00;
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
		if(i<dim-16){		//skip last read (dim is never multiple of 6)
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
		}
		if(i<dim-8){		//skip last read (dim is never multiple of 6)
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
		}

		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(3);
		for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+1]=bufferI[z+1];	//M0
			memCODE[k]=bufferI[z+2];	//L0
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+2]=bufferI[z+2];	//H0
			memCODE[k+6]=bufferI[z+1];	//H1
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+5]=bufferI[z+1];	//M1
			memCODE[k+4]=bufferI[z+2];	//L1
			k+=8;
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+1]=bufferI[z+1];	//M2
			memCODE[k+0]=bufferI[z+2];	//L2
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+2]=bufferI[z+2];	//H2
			memCODE[k+6]=bufferI[z+1];	//H3
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+5]=bufferI[z+1];	//M3
			memCODE[k+4]=bufferI[z+2];	//L3
			k+=8;
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+1]=bufferI[z+1];	//M4
			memCODE[k+0]=bufferI[z+2];	//L4
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+2]=bufferI[z+2];	//H4
			memCODE[k+6]=bufferI[z+1];	//H5
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+5]=bufferI[z+1];	//M5
			memCODE[k+4]=bufferI[z+2];	//L5
			k+=8;
		}
		PrintStatus(strings[S_CodeReading2],i*100/dim,i/2);	//"Read: %d%%, addr. %05X"
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadCodeErr2],dim,k);	//"Error reading code area, requested %d bytes, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);
//****************** read config area ********************
	if(config>2){					//config area @ 0xF80000
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
		bufferU[j++]=0x20;				//MOV XXXX,W6
		bufferU[j++]=0x00;
		bufferU[j++]=0x06;
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
		PacketIO(3);
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],0xF80000,0xF80000,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
		}
		//save 0xF800000 to 0xF80010
		for(i=0,z=0;i<9;i++){
			for(;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			memCONFIG[i*4]=bufferI[z+2];	//Low byte
			memCONFIG[i*4+1]=bufferI[z+1];	//High byte
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
		PacketIO(3);
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],0xF80000,0xF80000,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
		}
		//save 0xF800012 to 0xF80016
		for(i=9,z=0;i<12;i++){
			for(;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			memCONFIG[i*4]=bufferI[z+2];	//Low byte
			memCONFIG[i*4+1]=bufferI[z+1];	//High byte
			z+=3;
		}
		PrintStatusClear();
	}
//****************** read eeprom ********************
	if(dim2){
		if(saveLog)	fprintf(logfile,"\nEEPROM:\n");
		PrintMessage(strings[S_ReadEE]);		//read eeprom ...
		PrintStatusSetup();
		bufferU[j++]=SIX_N;
		bufferU[j++]=0x45;				//append 1 NOP
		bufferU[j++]=0x20;				//MOV #0x7F,W0
		bufferU[j++]=0x07;
		bufferU[j++]=0xF0;
		bufferU[j++]=0x88;				//MOV W0,TABLPAG
		if(newTABLPAG){
			bufferU[j++]=0x02;
			bufferU[j++]=0xA0;
		}
		else{
			bufferU[j++]=0x01;
			bufferU[j++]=0x90;
		}
		bufferU[j++]=0x2F;		//MOV #<ADDR[15:0]>,W6   (base address)
		bufferU[j++]=(EEbaseAddr>>4)&0xFF;
		bufferU[j++]=(EEbaseAddr&0xF0)+6;
		bufferU[j++]=0x20;				//MOV #VISI,W7
		bufferU[j++]=0x78;
		bufferU[j++]=0x47;
		bufferU[j++]=0x04;				//GOTO 0x200
		bufferU[j++]=0x02;
		bufferU[j++]=0x00;
		for(k2=0,i=0;i<dim2;i+=2){
			bufferU[j++]=SIX_LONG;				//TBLRDL [W6++],[W7]
			bufferU[j++]=0xBA;
			bufferU[j++]=0x0B;
			bufferU[j++]=0xB6;
			bufferU[j++]=REGOUT;
			if(j>DIMBUF-6||i==dim2-2){
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(3);
				j=0;
				for(z=0;z<DIMBUF-2;z++){
					if(bufferI[z]==REGOUT){
						memEE[EEbaseAddr+k2++]=bufferI[z+2];
						memEE[EEbaseAddr+k2++]=bufferI[z+1];
						z+=3;
						//k2+=2;		//skip high word
					}
				}
				PrintStatus(strings[S_CodeReading],(i+dim)*100/(dim+dim2),i);	//"Read: %d%%, addr. %03X"
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
		else PrintMessage(strings[S_Compl]);
	}
//****************** read executive area ********************
	if(executiveArea){
		j=0;
		PrintMessage(strings[S_Read_EXE_A]);		//read executive area ...
		PrintStatusSetup();
		if(saveLog)	fprintf(logfile,"\nExecutive area:\n");
		bufferU[j++]=SIX_N;
		bufferU[j++]=0x45;				//append 1 NOP
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
		bufferU[j++]=0x20;				//MOV XXXX,W6
		bufferU[j++]=0x00;
		bufferU[j++]=0x06;
		bufferU[j++]=0x20;				//MOV #VISI,W7
		bufferU[j++]=0x78;
		bufferU[j++]=0x47;
		bufferU[j++]=0x04;				//GOTO 0x200
		bufferU[j++]=0x02;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		for(i=0,k=0;i<executiveArea;i+=16){
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
			bufferU[j++]=SIX_LONG;
			bufferU[j++]=0x04;				//GOTO 0x200
			bufferU[j++]=0x02;
			bufferU[j++]=0x00;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(3);
			j=0;
			for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			if(z<DIMBUF-2){
				memExec[k+1]=bufferI[z+1];	//M0
				memExec[k]=bufferI[z+2];	//L0
			}
			for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			if(z<DIMBUF-2){
				memExec[k+2]=bufferI[z+2];	//H0
				memExec[k+6]=bufferI[z+1];	//H1
			}
			for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			if(z<DIMBUF-2){
				memExec[k+5]=bufferI[z+1];	//M1
				memExec[k+4]=bufferI[z+2];	//L1
			}
			for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			if(z<DIMBUF-2){
				memExec[k+9]=bufferI[z+1];	//M2
				memExec[k+8]=bufferI[z+2];	//L2
			}
			for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			if(z<DIMBUF-2){
				memExec[k+10]=bufferI[z+2];	//H2
				memExec[k+14]=bufferI[z+1];	//H3
			}
			for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			if(z<DIMBUF-2){
				memExec[k+13]=bufferI[z+1];	//M3
				memExec[k+12]=bufferI[z+2];	//L3
			}
			k+=16;
			PrintStatus(strings[S_CodeReading2],i*100/executiveArea,0x800000+i/2);	//"Read: %d%%, addr. %05X"
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
		PrintStatusEnd();
		if(k!=executiveArea){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ReadCodeErr2],executiveArea,k);	//"Error reading code area, requested %d bytes, read %d\r\n"
		}
		else PrintMessage(strings[S_Compl]);
	}
	PrintMessage("\r\n");
//****************** exit ********************
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
//****************** visualize ********************
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
	}
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

void Read24Ex(int dim,int dim2,int options,int appIDaddr,int executiveArea){
// read 16 bit PIC 24Exxxx
// deviceID @ 0xFF0000
// dim=program size (16 bit words)
// dim2=dummy parameter
// options: not used
// appIDaddr = application ID word lower address (high is 0x80)
// executiveArea = size of executive area (16 bit words, starting at 0x800000)
	int k=0,z=0,i,j;
	unsigned char UID[16];
	for(i=0;i<16;i++) UID[i]=0xFF;
	if(FWVersion<0x900){
		PrintMessage1(strings[S_FWver2old],"0.9.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(!CheckV33Regulator()){
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	if(saveLog){
		OpenLogFile();
		fprintf(logfile,"Read24Ex(%d,%d,%d,%d,%d)    (0x%X,0x%X,0x%X,0x%X,0x%X)\n",dim,dim2,options,appIDaddr,executiveArea,dim,dim2,options,appIDaddr,executiveArea);
	}
	dim*=2;		//from words to bytes
	if(dim>0x100000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	unsigned char *memExec=0;
	executiveArea*=2;
	if(executiveArea) memExec=(unsigned char *)malloc(executiveArea);
	for(i=0;i<executiveArea;i++) memExec[i]=0xFF;
	size=dim;
	if(memCODE) free(memCODE);
	memCODE=(unsigned char*)malloc(size);		//CODE
	memset(memCODE,0xFF,dim);
	StartHVReg(-1);		//LVP: current limited to (5-0.7-3.6)/10k = 50uA
	unsigned int start=GetTickCount();
	j=0;
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
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x1;
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
	bufferU[j++]=55000>>8;
	bufferU[j++]=55000&0xff;
	bufferU[j++]=WAIT_T3;			//min 50ms
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
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX;				//GOTO 0x200
	bufferU[j++]=0x04;
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(65);
	j=0;
	//Read DeviceID @0xFF0000, DevRev @0xFF0002
	bufferU[j++]=SIX_N;
	bufferU[j++]=4;
	bufferU[j++]=0x20;				//MOV XXXX,W0
	bufferU[j++]=0x0F;
	bufferU[j++]=0xF0;				//0xFF
	bufferU[j++]=0x88;				//MOV W0,TABLPAG
	bufferU[j++]=0x02;
	bufferU[j++]=0xA0;
	bufferU[j++]=0x20;				//MOV XXXX,W6
	bufferU[j++]=0x00;
	bufferU[j++]=0x06;				//0x0000
	bufferU[j++]=0x20;				//MOV #VISI,W7
	bufferU[j++]=0xF8;
	bufferU[j++]=0x87;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0xB6;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0xB6;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG5;				//GOTO 0x200
	bufferU[j++]=0x04;
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	int w0=0,w1=0;
	for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2) w0=(bufferI[z+1]<<8)+bufferI[z+2];
	for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2) w1=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage2("DevID: 0x%04X\r\nDevRev: 0x%04X\r\n",w0,w1);
	PIC24_ID(w0);
	//Read ApplicationID @ appIDaddr
	bufferU[j++]=SIX_N;
	bufferU[j++]=0x4;
	bufferU[j++]=0x20;				//MOV XXXX,W0
	bufferU[j++]=0x08;
	bufferU[j++]=0x00;
	bufferU[j++]=0x88;				//MOV W0,TABLPAG
	bufferU[j++]=0x02;
	bufferU[j++]=0xA0;
	bufferU[j++]=0x20+((appIDaddr>>12)&0xF);	//MOV XXXX,W6
	bufferU[j++]=(appIDaddr>>4)&0xFF;
	bufferU[j++]=((appIDaddr<<4)&0xF0)+6;
	bufferU[j++]=0x20;				//MOV #VISI,W7
	bufferU[j++]=0xF8;
	bufferU[j++]=0x87;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX_LONG5;				//TBLRDL [W6],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0x96;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG5;				//GOTO 0x200
	bufferU[j++]=0x04;
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=SIX_N;
	bufferU[j++]=4;
	bufferU[j++]=0x20;				//MOV XXXX,W0
	bufferU[j++]=0x00;
	bufferU[j++]=0x00;
	bufferU[j++]=0x88;				//MOV W0,TABLPAG
	bufferU[j++]=0x02;
	bufferU[j++]=0xA0;
	bufferU[j++]=0x20;				//MOV 0,W6
	bufferU[j++]=0x00;
	bufferU[j++]=0x06;
	bufferU[j++]=0x20;				//MOV #VISI,W7
	bufferU[j++]=0xF8;
	bufferU[j++]=0x87;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2) w0=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage2("ApplicationID @ 0x80%04X:  0x%04X\r\n",appIDaddr,w0);
//****************** read code ********************
	PrintMessage(strings[S_CodeReading1]);		//code read ...
	PrintStatusSetup();
//Read 6 24 bit words packed in 9 16 bit words
//memory address advances by 24 bytes because of alignment
	int High=0;
	for(i=0;i<dim;i+=24){
		if((i>>17)!=High){	//advance page
			bufferU[j++]=SIX;				//MOV XXXX,W0
			bufferU[j++]=0x20;
			bufferU[j++]=(i>>21)&0xFF;
			bufferU[j++]=(i>>13)&0xF0;
			bufferU[j++]=SIX;				//MOV W0,TABLPAG
			bufferU[j++]=0x88;
			bufferU[j++]=0x02;
			bufferU[j++]=0xA0;
			bufferU[j++]=SIX_LONG;				//GOTO 0x200
			bufferU[j++]=0x04;
			bufferU[j++]=0x02;
			bufferU[j++]=0x00;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(2);
			j=0;
			High=i>>17;
		}
		bufferU[j++]=SIX_LONG5;				//GOTO 0x200
		bufferU[j++]=0x04;
		bufferU[j++]=0x02;
		bufferU[j++]=0x00;
		bufferU[j++]=SIX_LONG5;				//TBLRDL [W6],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0x96;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG5;				//TBLRDH.B [W6++],[W7++]
		bufferU[j++]=0xBA;
		bufferU[j++]=0xDB;
		bufferU[j++]=0xB6;
		bufferU[j++]=SIX_LONG5;				//TBLRDH.B [++W6],[W7--]
		bufferU[j++]=0xBA;
		bufferU[j++]=0xD3;
		bufferU[j++]=0xD6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		if(i<dim-16){		//skip last read (dim is never multiple of 6)
			bufferU[j++]=SIX_LONG5;				//TBLRDL [W6],[W7]
			bufferU[j++]=0xBA;
			bufferU[j++]=0x0B;
			bufferU[j++]=0x96;
			bufferU[j++]=REGOUT;
			bufferU[j++]=SIX_LONG5;				//TBLRDH.B [W6++],[W7++]
			bufferU[j++]=0xBA;
			bufferU[j++]=0xDB;
			bufferU[j++]=0xB6;
			bufferU[j++]=SIX_LONG5;				//TBLRDH.B [++W6],[W7--]
			bufferU[j++]=0xBA;
			bufferU[j++]=0xD3;
			bufferU[j++]=0xD6;
			bufferU[j++]=REGOUT;
			bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
			bufferU[j++]=0xBA;
			bufferU[j++]=0x0B;
			bufferU[j++]=0xB6;
			bufferU[j++]=REGOUT;
		}
		if(i<dim-8){		//skip last read (dim is never multiple of 6)
			bufferU[j++]=SIX_LONG5;				//TBLRDL [W6],[W7]
			bufferU[j++]=0xBA;
			bufferU[j++]=0x0B;
			bufferU[j++]=0x96;
			bufferU[j++]=REGOUT;
			bufferU[j++]=SIX_LONG5;				//TBLRDH.B [W6++],[W7++]
			bufferU[j++]=0xBA;
			bufferU[j++]=0xDB;
			bufferU[j++]=0xB6;
			bufferU[j++]=SIX_LONG5;				//TBLRDH.B [++W6],[W7--]
			bufferU[j++]=0xBA;
			bufferU[j++]=0xD3;
			bufferU[j++]=0xD6;
			bufferU[j++]=REGOUT;
			bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
			bufferU[j++]=0xBA;
			bufferU[j++]=0x0B;
			bufferU[j++]=0xB6;
			bufferU[j++]=REGOUT;
		}
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(4);
		for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+1]=bufferI[z+1];	//M0
			memCODE[k]=bufferI[z+2];	//L0
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+2]=bufferI[z+2];	//H0
			memCODE[k+6]=bufferI[z+1];	//H1
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+5]=bufferI[z+1];	//M1
			memCODE[k+4]=bufferI[z+2];	//L1
			k+=8;
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+1]=bufferI[z+1];	//M2
			memCODE[k+0]=bufferI[z+2];	//L2
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+2]=bufferI[z+2];	//H2
			memCODE[k+6]=bufferI[z+1];	//H3
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+5]=bufferI[z+1];	//M3
			memCODE[k+4]=bufferI[z+2];	//L3
			k+=8;
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+1]=bufferI[z+1];	//M4
			memCODE[k+0]=bufferI[z+2];	//L4
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+2]=bufferI[z+2];	//H4
			memCODE[k+6]=bufferI[z+1];	//H5
		}
		for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		if(z<DIMBUF-2){
			memCODE[k+5]=bufferI[z+1];	//M5
			memCODE[k+4]=bufferI[z+2];	//L5
			k+=8;
		}
		PrintStatus(strings[S_CodeReading2],i*100/dim,i/2);	//"Read: %d%%, addr. %05X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadCodeErr2],dim,k);	//"Error reading code area, requested %d bytes, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);
//****************** read executive area ********************
	if(executiveArea){
		j=0;
		PrintMessage(strings[S_Read_EXE_A]);		//read executive area ...
		PrintStatusSetup();
		if(saveLog)	fprintf(logfile,"\nExecutive area:\n");
		bufferU[j++]=SIX_N;
		bufferU[j++]=0x45;				//append 1 NOP
		bufferU[j++]=0x20;				//MOV XXXX,W0
		bufferU[j++]=0x08;
		bufferU[j++]=0x00;
		bufferU[j++]=0x88;				//MOV W0,TABLPAG
		bufferU[j++]=0x02;
		bufferU[j++]=0xA0;
		bufferU[j++]=0x20;				//MOV XXXX,W6
		bufferU[j++]=0x00;
		bufferU[j++]=0x06;
		bufferU[j++]=0x20;				//MOV #VISI,W7
		bufferU[j++]=0xF8;
		bufferU[j++]=0x87;
		bufferU[j++]=0x04;				//GOTO 0x200
		bufferU[j++]=0x02;
		bufferU[j++]=0x00;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		for(i=0,k=0;i<executiveArea;i+=16){
			bufferU[j++]=SIX_LONG5;				//TBLRDL [W6],[W7]
			bufferU[j++]=0xBA;
			bufferU[j++]=0x0B;
			bufferU[j++]=0x96;
			bufferU[j++]=REGOUT;
			bufferU[j++]=SIX_LONG5;				//TBLRDH.B [W6++],[W7++]
			bufferU[j++]=0xBA;
			bufferU[j++]=0xDB;
			bufferU[j++]=0xB6;
			bufferU[j++]=SIX_LONG5;				//TBLRDH.B [++W6],[W7--]
			bufferU[j++]=0xBA;
			bufferU[j++]=0xD3;
			bufferU[j++]=0xD6;
			bufferU[j++]=REGOUT;
			bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
			bufferU[j++]=0xBA;
			bufferU[j++]=0x0B;
			bufferU[j++]=0xB6;
			bufferU[j++]=REGOUT;
			bufferU[j++]=SIX_LONG5;				//TBLRDL [W6],[W7]
			bufferU[j++]=0xBA;
			bufferU[j++]=0x0B;
			bufferU[j++]=0x96;
			bufferU[j++]=REGOUT;
			bufferU[j++]=SIX_LONG5;				//TBLRDH.B [W6++],[W7++]
			bufferU[j++]=0xBA;
			bufferU[j++]=0xDB;
			bufferU[j++]=0xB6;
			bufferU[j++]=SIX_LONG5;				//TBLRDH.B [++W6],[W7--]
			bufferU[j++]=0xBA;
			bufferU[j++]=0xD3;
			bufferU[j++]=0xD6;
			bufferU[j++]=REGOUT;
			bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
			bufferU[j++]=0xBA;
			bufferU[j++]=0x0B;
			bufferU[j++]=0xB6;
			bufferU[j++]=REGOUT;
			bufferU[j++]=SIX_LONG;
			bufferU[j++]=0x04;				//GOTO 0x200
			bufferU[j++]=0x02;
			bufferU[j++]=0x00;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(3);
			j=0;
			for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			if(z<DIMBUF-2){
				memExec[k+1]=bufferI[z+1];	//M0
				memExec[k]=bufferI[z+2];	//L0
			}
			for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			if(z<DIMBUF-2){
				memExec[k+2]=bufferI[z+2];	//H0
				memExec[k+6]=bufferI[z+1];	//H1
			}
			for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			if(z<DIMBUF-2){
				memExec[k+5]=bufferI[z+1];	//M1
				memExec[k+4]=bufferI[z+2];	//L1
			}
			for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			if(z<DIMBUF-2){
				memExec[k+9]=bufferI[z+1];	//M2
				memExec[k+8]=bufferI[z+2];	//L2
			}
			for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			if(z<DIMBUF-2){
				memExec[k+10]=bufferI[z+2];	//H2
				memExec[k+14]=bufferI[z+1];	//H3
			}
			for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
			if(z<DIMBUF-2){
				memExec[k+13]=bufferI[z+1];	//M3
				memExec[k+12]=bufferI[z+2];	//L3
			}
			k+=16;
			PrintStatus(strings[S_CodeReading2],i*100/executiveArea,0x800000+i/2);	//"Read: %d%%, addr. %05X"
			if(RWstop) i=dim;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
		PrintStatusEnd();
		if(k!=executiveArea){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ReadCodeErr2],executiveArea,k);	//"Error reading code area, requested %d bytes, read %d\r\n"
		}
		else PrintMessage(strings[S_Compl]);
	}
	PrintMessage("\r\n");
//****************** read user ID ********************
	j=0;
	if(saveLog)	fprintf(logfile,"\nUser ID:\n");
	bufferU[j++]=SIX_N;
	bufferU[j++]=0x45;				//append 1 NOP
	bufferU[j++]=0x20;				//MOV XXXX,W0
	bufferU[j++]=0x08;
	bufferU[j++]=0x00;
	bufferU[j++]=0x88;				//MOV W0,TABLPAG
	bufferU[j++]=0x02;
	bufferU[j++]=0xA0;
	bufferU[j++]=0x20;				//MOV XXXX,W6
	bufferU[j++]=0xFF;
	bufferU[j++]=0x86;
	bufferU[j++]=0x20;				//MOV #VISI,W7
	bufferU[j++]=0xF8;
	bufferU[j++]=0x87;
	bufferU[j++]=0x04;				//GOTO 0x200
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	bufferU[j++]=SIX_LONG5;				//TBLRDL [W6],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0x96;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG5;				//TBLRDH.B [W6++],[W7++]
	bufferU[j++]=0xBA;
	bufferU[j++]=0xDB;
	bufferU[j++]=0xB6;
	bufferU[j++]=SIX_LONG5;				//TBLRDH.B [++W6],[W7--]
	bufferU[j++]=0xBA;
	bufferU[j++]=0xD3;
	bufferU[j++]=0xD6;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0xB6;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG5;				//TBLRDL [W6],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0x96;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG5;				//TBLRDH.B [W6++],[W7++]
	bufferU[j++]=0xBA;
	bufferU[j++]=0xDB;
	bufferU[j++]=0xB6;
	bufferU[j++]=SIX_LONG5;				//TBLRDH.B [++W6],[W7--]
	bufferU[j++]=0xBA;
	bufferU[j++]=0xD3;
	bufferU[j++]=0xD6;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0xB6;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG5;
	bufferU[j++]=0x04;				//GOTO 0x200
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(3);
	j=0;
	k=0;
	for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		UID[k+1]=bufferI[z+1];	//M0
		UID[k]=bufferI[z+2];	//L0
	}
	for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		UID[k+2]=bufferI[z+2];	//H0
		UID[k+6]=bufferI[z+1];	//H1
	}
	for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		UID[k+5]=bufferI[z+1];	//M1
		UID[k+4]=bufferI[z+2];	//L1
	}
	for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		UID[k+9]=bufferI[z+1];	//M2
		UID[k+8]=bufferI[z+2];	//L2
	}
	for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		UID[k+10]=bufferI[z+2];	//H2
		UID[k+14]=bufferI[z+1];	//H3
	}
	for(z+=3;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		UID[k+13]=bufferI[z+1];	//M3
		UID[k+12]=bufferI[z+2];	//L3
	}
	PrintMessage("\r\n");
//****************** exit ********************
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
//****************** visualize ********************
	for(i=0;i<10;i++) PrintMessage3("CONFIG%d(0x%05X): 0x%02X\r\n",i,(dim-40+i*4)/2,memCODE[dim-40+i*4]);
	PrintMessage(strings[S_CodeMem]);	//"\r\nCode memory:\r\n"
	DisplayCODE24F(dim);
	if(executiveArea){
		char s[256],t[256];
		int d,valid,empty=1;
		char* aux=(char*)malloc((executiveArea/COL+1)*2*(16+COL*9));
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
	}
	for(i=0;i<4;i++){
		if(UID[i*4]!=0xFF||UID[i*4+1]!=0xFF) PrintMessage2("UserID%d=%04X",i,(UID[i*4]<<8)+UID[i*4+1]);
	}
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}


void Write24Fx(int dim,int dim2,int options,int appIDaddr,int rowSize, double wait){
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
	j=0;
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
	PacketIO(37);
	j=0;
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
	PacketIO(2);
	j=0;
	int w0=0,w1=0;
	for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
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
	PacketIO(2);
	j=0;
	for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
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
			PacketIO(20);
			msDelay(200);
			j=0;
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
	PacketIO(7);
	j=0;
	for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	w0=bufferI[z+1]&0x80;
	//Wait for erase completion (max 1s)
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
	j=0;
	for(i=0;erase<2&&i<100&&w0;i++){
		PacketIO(5);
		for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		w0=bufferI[z+1]&0x80;
		msDelay(10);
	}
	if(saveLog)	fprintf(logfile,"\nErase time: %dms\n",i*12);
//****************** prepare write ********************
	msDelay(10);
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
	PacketIO(5);
	j=0;
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
				PacketIO(2);
				j=0;
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
					PacketIO(3);
					j=0;
					if(saveLog){
						fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d, k=%d 0=%d\n"
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
			PacketIO(3);
			j=0;
			PrintStatus(strings[S_CodeWriting2],i*100/(dim+dim2),i/2);	//"Write: %d%%,addr. %04X"
			if(RWstop) i=dim;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i/2,i/2,k,k);	//"i=%d, k=%d 0=%d\n"
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
			PacketIO(2);
			j=0;
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
		PacketIO(3);
		PrintStatus(strings[S_CodeV2],i*100/(dim+dim2),i/2);	//"Verify: %d%%, addr. %04X"
		if(RWstop) i=dim;
		for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
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
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log8],i/2,i/2,k,k,err);	//"i=%d, k=%d, errori=%d\n"
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
			//Erase EEPROM
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
			PacketIO(9);
			j=0;
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
			PacketIO(6);
			j=0;
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
			PacketIO(4);
			j=0;
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
			PacketIO(1);
			j=0;
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
				PacketIO(wait+2);
				j=0;
				PrintStatus(strings[S_CodeWriting],(i-0x1000+dim2)*100/(dim2),i);	//"Scrittura: %d%%, ind. %03X"
				if(RWstop) i=dim;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d, k=%d 0=%d\n"
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
				PacketIO(3);
				for(z=0;z<DIMBUF-2;z++){
					if(bufferI[z]==REGOUT){
						CheckData(memEE[k2],bufferI[z+2],i,&errE);
						CheckData(memEE[k2+1],bufferI[z+1],i+1,&errE);
						z+=3;
						k2+=2;
					}
				}
				PrintStatus(strings[S_CodeReading],(i-EEbaseAddr)*100/(dim2),i);	//"Read: %d%%, addr. %03X"
				if(RWstop) i=EEbaseAddr+dim2;
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
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
	j=0;
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
			if(memCONFIG[i*4]<0xFF){			//write if not empty
				bufferU[j++]=SIX_LONG;				//BSET NVMCON,#WR
				bufferU[j++]=0xA8;
				bufferU[j++]=0xE7;
				bufferU[j++]=0x61;
				bufferU[j++]=ICSP_NOP;
				bufferU[j++]=ICSP_NOP;
			}
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			msDelay(27);
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
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
		PacketIO(3);
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],0xF80000,0xF80000,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
		}
		for(i=0,z=0;i<9;i++){
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
		PacketIO(3);
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],0xF80000,0xF80000,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
		}
		for(z=0;i<12;i++){
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
			PacketIO(3);
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
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
			PacketIO(3);
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
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
		PacketIO(3);
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],0xF80000,0xF80000,0,0);	//"i=%d(0x%X), k=%d(0x%X)\n"
		}
		for(i=0,z=0;i<7;i++){
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
	PacketIO(2);
	j=0;
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write24Ex(int dim,int dim2,int options,int appIDaddr,int rowSize, double wait){
// write 16 bit PIC 24Exxxx
// deviceID @ 0xFF0000
// dim = program size (16 bit words)
// dim2 = dummy parameter
// options = dummy parameter
// appIDaddr = dummy parameter
// rowSize = dummy parameter
// wait = dummy parameter
	int k=0,k2=0,z=0,i,j;
	int err=0;
	if(FWVersion<0x900){
		PrintMessage1(strings[S_FWver2old],"0.9.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(!CheckV33Regulator()){
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	if(saveLog){
		OpenLogFile();
		fprintf(logfile,"Write24Ex(%d,%d,%d,%d,%d,%.1f)    (0x%X,0x%X,0x%X,0x%X,0x%X,%.3f)\n"
			,dim,dim2,options,appIDaddr,rowSize,wait,dim,dim2,options,appIDaddr,rowSize,wait);
	}
	dim*=2;		//from words to bytes
	if(dim>0x100000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	if(dim>size) for(i=size;i<dim;i++) memCODE[i]=0xFF;	//fill with 0xFF to avoid unintended writing
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Empty code area\r\n"
		return;
	}
	StartHVReg(-1);		//LVP: current limited to (5-0.7-3.6)/10k = 50uA
	//last 10 program words
	for(i=0;i<10;i++) PrintMessage3("CONFIG%d(0x%05X): 0x%02X\r\n",i,(dim-40+i*4)/2,memCODE[dim-40+i*4]);
	unsigned int start=GetTickCount();
	j=0;
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
	bufferU[j++]=EN_VPP_VCC;	//VDD
	bufferU[j++]=0x1;
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
	bufferU[j++]=55000>>8;
	bufferU[j++]=55000&0xff;
	bufferU[j++]=WAIT_T3;			//min 50ms
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
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX;				//GOTO 0x200
	bufferU[j++]=0x04;
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(65);
	j=0;
	//Read DeviceID @0xFF0000, DevRev @0xFF0002
	bufferU[j++]=SIX_N;
	bufferU[j++]=4;
	bufferU[j++]=0x20;				//MOV XXXX,W0
	bufferU[j++]=0x0F;
	bufferU[j++]=0xF0;				//0xFF
	bufferU[j++]=0x88;				//MOV W0,TABLPAG
	bufferU[j++]=0x02;
	bufferU[j++]=0xA0;
	bufferU[j++]=0x20;				//MOV XXXX,W6
	bufferU[j++]=0x00;
	bufferU[j++]=0x06;				//0x0000
	bufferU[j++]=0x20;				//MOV #VISI,W7
	bufferU[j++]=0xF8;
	bufferU[j++]=0x87;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0xB6;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0xB6;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG5;				//GOTO 0x200
	bufferU[j++]=0x04;
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	int w0=0,w1=0;
	for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
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
	bufferU[j++]=0x02;
	bufferU[j++]=0xA0;
	bufferU[j++]=0x20+((appIDaddr>>12)&0xF);	//MOV XXXX,W6
	bufferU[j++]=(appIDaddr>>4)&0xFF;
	bufferU[j++]=((appIDaddr<<4)&0xF0)+6;
	bufferU[j++]=0x20;				//MOV #VISI,W7
	bufferU[j++]=0xF8;
	bufferU[j++]=0x87;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX_LONG5;				//TBLRDL [W6],[W7]
	bufferU[j++]=0xBA;
	bufferU[j++]=0x0B;
	bufferU[j++]=0x96;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_LONG5;				//GOTO 0x200
	bufferU[j++]=0x04;
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	if(z<DIMBUF-2) w0=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage2("ApplicationID @ 0x80%04X:  0x%04X\r\n",appIDaddr,w0);
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	if(saveLog)	fprintf(logfile,"\nERASE:\n");
	//bulk erase command
	bufferU[j++]=SIX_N;
	bufferU[j++]=2;
	bufferU[j++]=0x24;				//MOV 400D,W10
	bufferU[j++]=0x00;
	bufferU[j++]=0xDA;
	bufferU[j++]=0x88;				//MOV W10,NVMCON
	bufferU[j++]=0x39;
	bufferU[j++]=0x4A;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX;				//MOV NVMCON,W0
	bufferU[j++]=0x80;
	bufferU[j++]=0x39;
	bufferU[j++]=0x40;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX;				//MOV W0,VISI
	bufferU[j++]=0x88;
	bufferU[j++]=0x7C;
	bufferU[j++]=0x40;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=REGOUT;
	bufferU[j++]=SIX_N;
	bufferU[j++]=4;
	bufferU[j++]=0x20;				//MOV 0x55,W1
	bufferU[j++]=0x05;
	bufferU[j++]=0x51;
	bufferU[j++]=0x88;				//MOV W1,NVMKEY
	bufferU[j++]=0x39;
	bufferU[j++]=0x71;
	bufferU[j++]=0x20;				//MOV 0xAA,W1
	bufferU[j++]=0x0A;
	bufferU[j++]=0xA1;
	bufferU[j++]=0x88;				//MOV W8,NVMKEY
	bufferU[j++]=0x39;
	bufferU[j++]=0x71;
	bufferU[j++]=SIX_LONG5;				//BSET NVMCON,#WR
	bufferU[j++]=0xA8;
	bufferU[j++]=0xE7;
	bufferU[j++]=0x29;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX;				//MOV NVMCON,W0
	bufferU[j++]=0x80;
	bufferU[j++]=0x39;
	bufferU[j++]=0x40;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX;				//MOV W0,VISI
	bufferU[j++]=0x88;
	bufferU[j++]=0x7C;
	bufferU[j++]=0x40;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=REGOUT;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(4);
	j=0;
	for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
	w0=bufferI[z+1]&0x80;
	w0=1;
	//Wait for erase completion (max 500m)
	bufferU[j++]=SIX;				//MOV NVMCON,W0
	bufferU[j++]=0x80;
	bufferU[j++]=0x39;
	bufferU[j++]=0x40;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX;				//MOV W0,VISI
	bufferU[j++]=0x88;
	bufferU[j++]=0x7C;
	bufferU[j++]=0x40;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=REGOUT;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	j=0;
	for(i=0;i<50&&w0;i++){
		PacketIO(5);
		for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
		w0=bufferI[z+1]&0x80;
		msDelay(10);
	}
	if(saveLog)	fprintf(logfile,"ERASE TIME: %d ms\n",i*10);
	msDelay(70);/*erase time seems always too low*/
//****************** prepare write ********************
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX;
	bufferU[j++]=0x04;				//GOTO 0x200
	bufferU[j++]=0x02;
	bufferU[j++]=0x00;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=ICSP_NOP;
	bufferU[j++]=SIX_N;
	bufferU[j++]=4;
	bufferU[j++]=0x20;				//MOV 0xFA,W12
	bufferU[j++]=0x0F;
	bufferU[j++]=0xAC;
	bufferU[j++]=0x88;				//MOV W12,TABLPAG
	bufferU[j++]=0x02;
	bufferU[j++]=0xAC;
	bufferU[j++]=0x24;				//MOV XXXX,W10
	bufferU[j++]=0x00;
	bufferU[j++]=0x1A;				//0x4001
	bufferU[j++]=0x88;				//MOV W10,NVMCON
	bufferU[j++]=0x39;
	bufferU[j++]=0x4A;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	if(saveLog)	fprintf(logfile,"\nWRITE CODE:\n");
	PrintStatusSetup();
	int valid,High=0;
//	instruction words are stored in code memory array as follows:
//	L0 M0 H0 FF L1 M1 H1 FF ...
//	use 2 write latches @ 0xFA0000
	for(i=0,k=0;i<dim;i+=2*4){		//write 2 instruction words
		//skip row if empty
		for(valid=0;!valid&&i<dim;i+=valid?0:2*4){
			for(k2=0;k2<2*4&&!valid;k2++) if(memCODE[i+k2]<0xFF) valid=1;
		}
		if(i>=dim) break;
		bufferU[j++]=SIX_N;
		bufferU[j++]=5;
		bufferU[j++]=0xEB;				//CLR W6
		bufferU[j++]=0x03;
		bufferU[j++]=0x00;
		bufferU[j++]=0xEB;				//CLR W7
		bufferU[j++]=0x03;
		bufferU[j++]=0x80;
		bufferU[j++]=0x20+((memCODE[i+1]>>4)&0xF);				//MOV LSW0,W0
		bufferU[j++]=((memCODE[i+1]<<4)&0xF0)+((memCODE[i]>>4)&0xF);
		bufferU[j++]=(memCODE[i]<<4)&0xF0;
		bufferU[j++]=0x20+((memCODE[i+6]>>4)&0xF);				//MOV MSB1:MSB0,W1
		bufferU[j++]=((memCODE[i+6]<<4)&0xF0)+((memCODE[i+2]>>4)&0xF);
		bufferU[j++]=((memCODE[i+2]<<4)&0xF0)+1;
		bufferU[j++]=0x20+((memCODE[i+5]>>4)&0xF);				//MOV LSW1,W2
		bufferU[j++]=((memCODE[i+5]<<4)&0xF0)+((memCODE[i+4]>>4)&0xF);
		bufferU[j++]=((memCODE[i+4]<<4)&0xF0)+2;
		bufferU[j++]=SIX_N;
		bufferU[j++]=0x84;				//Append 2 NOP
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
		bufferU[j++]=SIX_N;
		bufferU[j++]=8;
		bufferU[j++]=0x20+((i>>13)&0xF);		//MOV i/2,W3
		bufferU[j++]=(i>>5)&0xFF;
		bufferU[j++]=((i<<3)&0xF0)+3;
		bufferU[j++]=0x20;						//MOV i/2/64K,W4
		bufferU[j++]=(i>>21)&0xFF;
		bufferU[j++]=((i>>13)&0xF0)+4;
		bufferU[j++]=0x88;				//MOV W3,NVMADR
		bufferU[j++]=0x39;
		bufferU[j++]=0x53;
		bufferU[j++]=0x88;				//MOV W4,NVMADRU
		bufferU[j++]=0x39;
		bufferU[j++]=0x64;
		bufferU[j++]=0x20;				//MOV 0x55,W1
		bufferU[j++]=0x05;
		bufferU[j++]=0x51;
		bufferU[j++]=0x88;				//MOV W1,NVMKEY
		bufferU[j++]=0x39;
		bufferU[j++]=0x71;
		bufferU[j++]=0x20;				//MOV 0xAA,W1
		bufferU[j++]=0x0A;
		bufferU[j++]=0xA1;
		bufferU[j++]=0x88;				//MOV W1,NVMKEY
		bufferU[j++]=0x39;
		bufferU[j++]=0x71;
		bufferU[j++]=SIX_LONG5;			//BSET NVMCON,#WR
		bufferU[j++]=0xA8;
		bufferU[j++]=0xE7;
		bufferU[j++]=0x29;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		PrintStatus(strings[S_CodeWriting2],i*100/(dim+dim2),i/2);	//"Write: %d%%,addr. %04X"
		if(RWstop) i=dim;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i/2,i/2,k,k);	//"i=%d, k=%d 0=%d\n"
		}
		//GOTO 0x200 is necessary to correctly write (???)
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=SIX;				//MOV NVMCON,W2
		bufferU[j++]=0x80;
		bufferU[j++]=0x39;
		bufferU[j++]=0x42;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=SIX;				//MOV W2,VISI
		bufferU[j++]=0x88;
		bufferU[j++]=0x7C;
		bufferU[j++]=0x42;
		bufferU[j++]=REGOUT;/**/
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=SIX;
		bufferU[j++]=0x04;				//GOTO 0x200
		bufferU[j++]=0x02;
		bufferU[j++]=0x00;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		j=0;
		PacketIO(2);
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
			bufferU[j++]=0x02;
			bufferU[j++]=0xA0;
			bufferU[j++]=SIX_LONG;				//GOTO 0x200
			bufferU[j++]=0x04;
			bufferU[j++]=0x02;
			bufferU[j++]=0x00;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=ICSP_NOP;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(2);
			j=0;
			High=i>>17;
		}
		bufferU[j++]=SIX;				//MOV i/2,W6
		bufferU[j++]=0x20+((i>>13)&0xF);
		bufferU[j++]=(i>>5)&0xFF;
		bufferU[j++]=((i<<3)&0xF0)+6;
		bufferU[j++]=SIX;				//MOV #VISI,W7
		bufferU[j++]=0x20;
		bufferU[j++]=0xF8;
		bufferU[j++]=0x87;
		bufferU[j++]=ICSP_NOP;
		bufferU[j++]=SIX_LONG5;				//TBLRDL [W6],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0x96;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG5;				//TBLRDH.B [W6++],[W7++]
		bufferU[j++]=0xBA;
		bufferU[j++]=0xDB;
		bufferU[j++]=0xB6;
		bufferU[j++]=SIX_LONG5;				//TBLRDH.B [++W6],[W7--]
		bufferU[j++]=0xBA;
		bufferU[j++]=0xD3;
		bufferU[j++]=0xD6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG5;				//TBLRDL [W6],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0x96;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG5;				//TBLRDH.B [W6++],[W7++]
		bufferU[j++]=0xBA;
		bufferU[j++]=0xDB;
		bufferU[j++]=0xB6;
		bufferU[j++]=SIX_LONG5;				//TBLRDH.B [++W6],[W7--]
		bufferU[j++]=0xBA;
		bufferU[j++]=0xD3;
		bufferU[j++]=0xD6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG5;				//TBLRDL [W6++],[W7]
		bufferU[j++]=0xBA;
		bufferU[j++]=0x0B;
		bufferU[j++]=0xB6;
		bufferU[j++]=REGOUT;
		bufferU[j++]=SIX_LONG5;				//GOTO 0x200
		bufferU[j++]=0x04;
		bufferU[j++]=0x02;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(4);
		PrintStatus(strings[S_CodeV2],i*100/(dim+dim2),i/2);	//"Verify: %d%%, addr. %04X"
		if(RWstop) i=dim;
		for(z=0;bufferI[z]!=REGOUT&&z<DIMBUF;z++);
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
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log8],i/2,i/2,k,k,err);	//"i=%d, k=%d, errori=%d\n"
		}
		if(err>=max_err) break;
	}
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed: %d errors\r\n"
	if(err>=max_err){
		PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
	}
//****************** exit ********************
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}
