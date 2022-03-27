/**
 * \file progP18F.c - algorithms to program the PIC18 family of microcontrollers
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

//This cannot be executed conditionally on MSVC
//#include "stdafx.h"

#include "common.h"

struct ID18{
	int id;
	char *device;
	int revmask;
} PIC18LIST[]={
	{0x0400,"18F252/2539 rev%d\r\n",0x1F},
	{0x0420,"18F452/4539 rev%d\r\n",0x1F},
	{0x0480,"18F242/2439 rev%d\r\n",0x1F},
	{0x04A0,"18F442/4439 rev%d\r\n",0x1F},
	{0x0500,"18F2320 rev%d\r\n",0x1F},
	{0x0520,"18F4320 rev%d\r\n",0x1F},
	{0x0580,"18F2220 rev%d\r\n",0x1F},
	{0x05A0,"18F4220 rev%d\r\n",0x1F},
	{0x0600,"18F8720 rev%d\r\n",0x1F},
	{0x0620,"18F6720 rev%d\r\n",0x1F},
	{0x0640,"18F8620 rev%d\r\n",0x1F},
	{0x0660,"18F6620 rev%d\r\n",0x1F},
	{0x07C0,"18F1320 rev%d\r\n",0x1F},
	{0x07E0,"18F1220 rev%d\r\n",0x1F},
	{0x0800,"18F248 rev%d\r\n",0x1F},
	{0x0820,"18F448 rev%d\r\n",0x1F},
	{0x0840,"18F258 rev%d\r\n",0x1F},
	{0x0860,"18F458 rev%d\r\n",0x1F},
	{0x0880,"18F4431 rev%d\r\n",0x1F},
	{0x08A0,"18F4331 rev%d\r\n",0x1F},
	{0x08C0,"18F2431 rev%d\r\n",0x1F},
	{0x08E0,"18F2331 rev%d\r\n",0x1F},
	{0x0B20,"18F6520 rev%d\r\n",0x1F},
	{0x0B00,"18F8520 rev%d\r\n",0x1F},
	{0x0C00,"18F4620 rev%d\r\n",0x1F},
	{0x0C20,"18F4610 rev%d\r\n",0x1F},
	{0x0C40,"18F4525 rev%d\r\n",0x1F},
	{0x0C60,"18F4515 rev%d\r\n",0x1F},
	{0x0C80,"18F2620 rev%d\r\n",0x1F},
	{0x0CA0,"18F2610 rev%d\r\n",0x1F},
	{0x0CC0,"18F2525 rev%d\r\n",0x1F},
	{0x0CE0,"18F2515 rev%d\r\n",0x1F},
	{0x0E80,"18F4680 rev%d\r\n",0x1F},
	{0x0EA0,"18F4585 rev%d\r\n",0x1F},
	{0x0EC0,"18F2680 rev%d\r\n",0x1F},
	{0x0EE0,"18F2585 rev%d\r\n",0x1F},
	{0x1081,"18F4523 rev%d\r\n",0x0F},
	{0x1080,"18F4520 rev%d\r\n",0x0F},
	{0x10A0,"18F4510 rev%d\r\n",0x1F},
	{0x10C1,"18F4423 rev%d\r\n",0x0F},
	{0x10C0,"18F4420 rev%d\r\n",0x0F},
	{0x10E0,"18F4410 rev%d\r\n",0x1F},
	{0x1100,"18F2523 rev%d\r\n",0x0F},
	{0x1100,"18F2520 rev%d\r\n",0x0F},
	{0x1120,"18F2510 rev%d\r\n",0x1F},
	{0x1140,"18F2423 rev%d\r\n",0x0F},
	{0x1140,"18F2420 rev%d\r\n",0x0F},
	{0x1160,"18F2410 rev%d\r\n",0x1F},
	{0x1200,"18F4550 rev%d\r\n",0x1F},
	{0x1220,"18F4455 rev%d\r\n",0x1F},
	{0x1240,"18F2550 rev%d\r\n",0x1F},
	{0x1260,"18F2455 rev%d\r\n",0x1F},
	{0x1340,"18F6527 rev%d\r\n",0x1F},
	{0x1360,"18F8527 rev%d\r\n",0x1F},
	{0x1380,"18F6622 rev%d\r\n",0x1F},
	{0x13A0,"18F8622 rev%d\r\n",0x1F},
	{0x13C0,"18F6627 rev%d\r\n",0x1F},
	{0x13E0,"18F8627 rev%d\r\n",0x1F},
	{0x1400,"18F6722 rev%d\r\n",0x1F},
	{0x1420,"18F8722 rev%d\r\n",0x1F},
	{0x1520,"18F65J10 rev%d\r\n",0x1F},
	{0x1540,"18F65J15 rev%d\r\n",0x1F},
	{0x1560,"18F66J10 rev%d\r\n",0x1F},
	{0x1580,"18F66J15 rev%d\r\n",0x1F},
	{0x15A0,"18F67J10 rev%d\r\n",0x1F},
	{0x15E0,"18F85J10 rev%d\r\n",0x1F},
	{0x1700,"18F85J15 rev%d\r\n",0x1F},
	{0x1720,"18F86J10 rev%d\r\n",0x1F},
	{0x1740,"18F86J15 rev%d\r\n",0x1F},
	{0x1760,"18F87J10 rev%d\r\n",0x1F},
	{0x1800,"18F66J60 rev%d\r\n",0x1F},
	{0x1820,"18F86J60 rev%d\r\n",0x1F},
	{0x1840,"18F96J60 rev%d\r\n",0x1F},
	{0x1A80,"18F4580 rev%d\r\n",0x1F},
	{0x1AA0,"18F4480 rev%d\r\n",0x1F},
	{0x1AC0,"18F2580 rev%d\r\n",0x1F},
	{0x1AE0,"18F2480 rev%d\r\n",0x1F},
	{0x1C00,"18F25J10 rev%d\r\n",0x1F},
	{0x1C20,"18F45J10 rev%d\r\n",0x1F},
	{0x1C40,"18LF25J10 rev%d\r\n",0x1F},
	{0x1C60,"18LF45J10 rev%d\r\n",0x1F},
	{0x1D00,"18F24J10 rev%d\r\n",0x1F},
	{0x1D20,"18F44J10 rev%d\r\n",0x1F},
	{0x1D40,"18LF24J10 rev%d\r\n",0x1F},
	{0x1D60,"18LF44J10 rev%d\r\n",0x1F},
	{0x1E00,"18F1230 rev%d\r\n",0x1F},
	{0x1E20,"18F1330 rev%d\r\n",0x1F},
	{0x1F00,"18F66J65 rev%d\r\n",0x1F},
	{0x1F20,"18F67J60 rev%d\r\n",0x1F},
	{0x1F40,"18F86J65 rev%d\r\n",0x1F},
	{0x1F60,"18F87J60 rev%d\r\n",0x1F},
	{0x1F80,"18F96J65 rev%d\r\n",0x1F},
	{0x1FA0,"18F97J60 rev%d\r\n",0x1F},
	{0x1FE0,"18F1330-ICD rev%d\r\n",0x1F},
	{0x2000,"18F46K20 rev%d\r\n",0x1F},
	{0x2020,"18F26K20 rev%d\r\n",0x1F},
	{0x2040,"18F45K20 rev%d\r\n",0x1F},
	{0x2060,"18F25K20 rev%d\r\n",0x1F},
	{0x2080,"18F44K20 rev%d\r\n",0x1F},
	{0x20A0,"18F24K20 rev%d\r\n",0x1F},
	{0x20C0,"18F43K20 rev%d\r\n",0x1F},
	{0x20E0,"18F23K20 rev%d\r\n",0x1F},
	{0x2100,"18F4321 rev%d\r\n",0x1F},
	{0x2120,"18F2321 rev%d\r\n",0x1F},
	{0x2140,"18F4221 rev%d\r\n",0x1F},
	{0x2160,"18F2221 rev%d\r\n",0x1F},
	{0x2400,"18F4450 rev%d\r\n",0x1F},
	{0x2420,"18F2450 rev%d\r\n",0x1F},
	{0x2700,"18F2682 rev%d\r\n",0x1F},
	{0x2720,"18F2685 rev%d\r\n",0x1F},
	{0x2740,"18F4682 rev%d\r\n",0x1F},
	{0x2760,"18F4685 rev%d\r\n",0x1F},
	{0x2A00,"18F4553 rev%d\r\n",0x1F},
	{0x2A20,"18F4458 rev%d\r\n",0x1F},
	{0x2A40,"18F2553 rev%d\r\n",0x1F},
	{0x2A60,"18F2458 rev%d\r\n",0x1F},
	{0x3800,"18F63J90 rev%d\r\n",0x1F},
	{0x3820,"18F64J90 rev%d\r\n",0x1F},
	{0x3860,"18F65J90 rev%d\r\n",0x1F},
	{0x3880,"18F83J90 rev%d\r\n",0x1F},
	{0x38A0,"18F84J90 rev%d\r\n",0x1F},
	{0x38E0,"18F85J90 rev%d\r\n",0x1F},
	{0x3900,"18F63J11 rev%d\r\n",0x1F},
	{0x3920,"18F64J11 rev%d\r\n",0x1F},
	{0x3960,"18F65J11 rev%d\r\n",0x1F},
	{0x3980,"18F83J11 rev%d\r\n",0x1F},
	{0x39A0,"18F84J11 rev%d\r\n",0x1F},
	{0x39E0,"18F85J11 rev%d\r\n",0x1F},
	{0x4100,"18F65J50 rev%d\r\n",0x1F},
	{0x4140,"18F66J50 rev%d\r\n",0x1F},
	{0x4160,"18F66J55 rev%d\r\n",0x1F},
	{0x4180,"18F67J50 rev%d\r\n",0x1F},
	{0x41A0,"18F85J50 rev%d\r\n",0x1F},
	{0x41E0,"18F86J50 rev%d\r\n",0x1F},
	{0x4200,"18F86J55 rev%d\r\n",0x1F},
	{0x4220,"18F87J50 rev%d\r\n",0x1F},
	{0x4440,"18F66J11 rev%d\r\n",0x1F},
	{0x4460,"18F66J16 rev%d\r\n",0x1F},
	{0x4480,"18F67J11 rev%d\r\n",0x1F},
	{0x44E0,"18F86J11 rev%d\r\n",0x1F},
	{0x4500,"18F86J16 rev%d\r\n",0x1F},
	{0x4520,"18F87J11 rev%d\r\n",0x1F},
	{0x4700,"18LF13K50 rev%d\r\n",0x1F},
	{0x4720,"18LF14K50 rev%d\r\n",0x1F},
	{0x4740,"18F13K50 rev%d\r\n",0x1F},
	{0x4760,"18F14K50 rev%d\r\n",0x1F},
	{0x49C0,"18F6628 rev%d\r\n",0x1F},
	{0x49E0,"18F8628 rev%d\r\n",0x1F},
	{0x4A00,"18F6723 rev%d\r\n",0x1F},
	{0x4A20,"18F8723 rev%d\r\n",0x1F},
	{0x4C00,"18F24J50 rev%d\r\n",0x1F},
	{0x4C20,"18F25J50 rev%d\r\n",0x1F},
	{0x4C40,"18F26J50 rev%d\r\n",0x1F},
	{0x4C60,"18F44J50 rev%d\r\n",0x1F},
	{0x4C80,"18F45J50 rev%d\r\n",0x1F},
	{0x4D80,"18F24J11 rev%d\r\n",0x1F},
	{0x4DA0,"18F25J11 rev%d\r\n",0x1F},
	{0x4DC0,"18F26J11 rev%d\r\n",0x1F},
	{0x4DE0,"18F44J11 rev%d\r\n",0x1F},
	{0x4E00,"18F45J11 rev%d\r\n",0x1F},
	{0x4E20,"18F46J11 rev%d\r\n",0x1F},
	{0x4E60,"18LF25J11 rev%d\r\n",0x1F},
	{0x4E80,"18LF26J11 rev%d\r\n",0x1F},
	{0x4EA0,"18LF44J11 rev%d\r\n",0x1F},
	{0x4EC0,"18LF45J11 rev%d\r\n",0x1F},
	{0x4EE0,"18LF46J11 rev%d\r\n",0x1F},
	{0x4F20,"18F14K22 rev%d\r\n",0x1F},
	{0x4F40,"18F13K22 rev%d\r\n",0x1F},
	{0x4F60,"18LF14K22 rev%d\r\n",0x1F},
	{0x4F80,"18LF13K22 rev%d\r\n",0x1F},
	{0x5000,"18F66J90 rev%d\r\n",0x1F},
	{0x5020,"18F67J90 rev%d\r\n",0x1F},
	{0x5040,"18F66J93/86J72 rev%d\r\n",0x1F},
	{0x5060,"18F67J93/87J72 rev%d\r\n",0x1F},
	{0x5080,"18F86J90 rev%d\r\n",0x1F},
	{0x50A0,"18F87J90 rev%d\r\n",0x1F},
	{0x50C0,"18F86J93 rev%d\r\n",0x1F},
	{0x50E0,"18F87J93 rev%d\r\n",0x1F},
	{0x5400,"18F46K22 rev%d\r\n",0x1F},
	{0x5420,"18LF46K22 rev%d\r\n",0x1F},
	{0x5440,"18F26K22 rev%d\r\n",0x1F},
	{0x5460,"18LF26K22 rev%d\r\n",0x1F},
	{0x5500,"18F45K22 rev%d\r\n",0x1F},
	{0x5520,"18LF45K22 rev%d\r\n",0x1F},
	{0x5540,"18F25K22 rev%d\r\n",0x1F},
	{0x5560,"18LF25K22 rev%d\r\n",0x1F},
	{0x5600,"18F44K22 rev%d\r\n",0x1F},
	{0x5620,"18LF44K22 rev%d\r\n",0x1F},
	{0x5640,"18F24K22 rev%d\r\n",0x1F},
	{0x5660,"18LF24K22 rev%d\r\n",0x1F},
	{0x5700,"18F43K22 rev%d\r\n",0x1F},
	{0x5720,"18LF43K22 rev%d\r\n",0x1F},
	{0x5740,"18F23K22 rev%d\r\n",0x1F},
	{0x5760,"18LF23K22 rev%d\r\n",0x1F},
	{0x5820,"18F26J53 rev%d\r\n",0x1F},
	{0x5860,"18F27J53 rev%d\r\n",0x1F},
	{0x58A0,"18F46J53 rev%d\r\n",0x1F},
	{0x58E0,"18F47J53 rev%d\r\n",0x1F},
	{0x5920,"18F26J13 rev%d\r\n",0x1F},
	{0x5960,"18F27J13 rev%d\r\n",0x1F},
	{0x59A0,"18F46J13 rev%d\r\n",0x1F},
	{0x59E0,"18F47J13 rev%d\r\n",0x1F},
	{0x5A20,"18LF26J53 rev%d\r\n",0x1F},
	{0x5A60,"18LF27J53 rev%d\r\n",0x1F},
	{0x5AA0,"18LF46J53 rev%d\r\n",0x1F},
	{0x5AE0,"18LF47J53 rev%d\r\n",0x1F},
	{0x5B20,"18LF26J13 rev%d\r\n",0x1F},
	{0x5B60,"18LF27J13 rev%d\r\n",0x1F},
	{0x5BA0,"18LF46J13 rev%d\r\n",0x1F},
	{0x5BE0,"18LF47J13 rev%d\r\n",0x1F},
	{0x5C00,"18F45K50 rev%d\r\n",0x1F},
	{0x5C20,"18F25K50 rev%d\r\n",0x1F},
	{0x5C60,"18F24K50 rev%d\r\n",0x1F},
	{0x5C80,"18LF45K50 rev%d\r\n",0x1F},
	{0x5CA0,"18LF25K50 rev%d\r\n",0x1F},
	{0x5CE0,"18LF24K50 rev%d\r\n",0x1F},
	{0x5D00,"18F46K50 rev%d\r\n",0x1F},
	{0x5D20,"18F26K50 rev%d\r\n",0x1F},
	{0x5D40,"18LF46K50 rev%d\r\n",0x1F},
	{0x5D60,"18LF26K50 rev%d\r\n",0x1F},
	{0x60E0,"18F66K80 rev%d\r\n",0x1F},
	{0x6100,"18F46K80 rev%d\r\n",0x1F},
	{0x6120,"18F26K80 rev%d\r\n",0x1F},
	{0x6140,"18F65K80 rev%d\r\n",0x1F},
	{0x6160,"18F45K80 rev%d\r\n",0x1F},
	{0x6180,"18F25K80 rev%d\r\n",0x1F},
	{0x61C0,"18LF66K80 rev%d\r\n",0x1F},
	{0x61E0,"18LF46K80 rev%d\r\n",0x1F},
	{0x6200,"18LF26K80 rev%d\r\n",0x1F},
	{0x6220,"18LF65K80 rev%d\r\n",0x1F},
	{0x6240,"18LF45K80 rev%d\r\n",0x1F},
	{0x6260,"18LF25K80 rev%d\r\n",0x1F},
	{0x6B80,"18F57K42 rev%c%d\r\n",0xFFFF},
	{0x6BA0,"18F56K42 rev%c%d\r\n",0xFFFF},
	{0x6BC0,"18F55K42 rev%c%d\r\n",0xFFFF},
	{0x6BE0,"18F47K42 rev%c%d\r\n",0xFFFF},
	{0x6C00,"18F46K42 rev%c%d\r\n",0xFFFF},
	{0x6C20,"18F45K42 rev%c%d\r\n",0xFFFF},
	{0x6C40,"18F27K42 rev%c%d\r\n",0xFFFF},
	{0x6C60,"18F26K42 rev%c%d\r\n",0xFFFF},
	{0x6CC0,"18LF57K42 rev%c%d\r\n",0xFFFF},
	{0x6CE0,"18LF56K42 rev%c%d\r\n",0xFFFF},
	{0x6D00,"18LF55K42 rev%c%d\r\n",0xFFFF},
	{0x6D20,"18LF47K42 rev%c%d\r\n",0xFFFF},
	{0x6D40,"18LF46K42 rev%c%d\r\n",0xFFFF},
	{0x6D60,"18LF45K42 rev%c%d\r\n",0xFFFF},
	{0x6D80,"18LF27K42 rev%c%d\r\n",0xFFFF},
	{0x6DA0,"18LF26K42 rev%c%d\r\n",0xFFFF},
	{0x6EC0,"18F26K83 rev%c%d\r\n",0xFFFF},
	{0x6EE0,"18F25K83 rev%c%d\r\n",0xFFFF},
	{0x6F00,"18LF26K83 rev%c%d\r\n",0xFFFF},
	{0x6F20,"18LF25K83 rev%c%d\r\n",0xFFFF},
	{0x73C0,"18F25Q43 rev%c%d\r\n",0xFFFF},
	{0x73E0,"18F45Q43 rev%c%d\r\n",0xFFFF},
	{0x7400,"18F55Q43 rev%c%d\r\n",0xFFFF},
	{0x7420,"18F26Q43 rev%c%d\r\n",0xFFFF},
	{0x7440,"18F46Q43 rev%c%d\r\n",0xFFFF},
	{0x7460,"18F56Q43 rev%c%d\r\n",0xFFFF},
	{0x7480,"18F27Q43 rev%c%d\r\n",0xFFFF},
	{0x74A0,"18F47Q43 rev%c%d\r\n",0xFFFF},
	{0x74C0,"18F57Q43 rev%c%d\r\n",0xFFFF},
};
	
void PIC18_ID(int id)
{
	char s[128];
	int i;
	for(i=0;i<sizeof(PIC18LIST)/sizeof(PIC18LIST[0]);i++){
		if(PIC18LIST[i].revmask!=0xFFFF&&PIC18LIST[i].id==(id&~PIC18LIST[i].revmask)){
			sprintf(s,PIC18LIST[i].device,id&PIC18LIST[i].revmask);
			PrintMessage(s);
			return;
		}
		else if(PIC18LIST[i].revmask==0xFFFF&&PIC18LIST[i].id==(id&0xFFFF)){
			int MJrev=(id>>22)&0x3F;
			int MNrev=(id>>16)&0x3F;
			sprintf(s,PIC18LIST[i].device,MJrev+'A',MNrev);
			PrintMessage(s);
			return;
		}
	}
	sprintf(s,"%s",strings[S_nodev]); //"Unknown device\r\n");
	PrintMessage(s);
}

void DisplayCODE18F(int dim){
// display  PIC18F CODE memory
	char s[256]="",t[256]="";
	char* aux=(char*)malloc((dim/COL+1)*(16+COL*6));
	aux[0]=0;
	int valid=0,empty=1,i,j,lines=0;
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

void Read18Fx(int dim,int dim2,int options)
// read 16 bit PIC 18Fxxxx
// dim=program size (bytes) 	dim2=eeprom size (bytes)
// options:
//	bit [3:0]
//   0 = vdd before vpp (12V)
//   1 = vdd before vpp (9V)
//   2 = low voltage entry with 32 bit key
//   3 = vdd before vpp (9V) with 32 bit key
//	bit [7:4]
//     0 = normal control registers address
//     1 = new control registers address (18FxxK80)
{
	int k=0,k2=0,z=0,i,j;
	int entry=options&0xF;
	int newAddr=(options>>4)&0xF;
	if(dim>0x1FFFFF||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	if(dim2>0x800||dim2<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(entry>0&&!CheckV33Regulator()){
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	double vpp=-1;
	if(entry==0) vpp=12;
	else if(entry==1||entry==3) vpp=8.5;
	if(!StartHVReg(vpp)){
		PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Read18F(%d,%d,%d)    (0x%X,0x%X,0x%X)\n",dim,dim2,options,dim,dim2,options);
	}
	size=dim;
	sizeEE=dim2;
	if(memCODE) free(memCODE);
	memCODE=(unsigned char*)malloc(size);		//CODE
	if(memEE) free(memEE);
	memEE=(unsigned char*)malloc(sizeEE);			//EEPROM
	for(j=0;j<8;j++) memID[j]=0xFF;
	for(j=0;j<14;j++) memCONFIG[j]=0xFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	if(entry==2){					//LV entry with key
		bufferU[j++]=EN_VPP_VCC;		//VDD
		bufferU[j++]=0x1;
	}
	if(entry==2||entry==3){			//entry with key
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=TX16;
		bufferU[j++]=2;
		bufferU[j++]=0x4D;
		bufferU[j++]=0x43;
		bufferU[j++]=0x48;
		bufferU[j++]=0x50;
		bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
		bufferU[j++]=0x5;
		bufferU[j++]=WAIT_T3;
	}
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x0E;			//3F
	bufferU[j++]=0x3F;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6E;			//-> TBLPTRU
	bufferU[j++]=0xF8;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x0E;			//FF
	bufferU[j++]=0xFF;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6E;			//-> TBLPTRH
	bufferU[j++]=0xF7;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x0E;			//FE
	bufferU[j++]=0xFE;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6E;			//-> TBLPTRL
	bufferU[j++]=0xF6;
	bufferU[j++]=TBLR_INC_N;		//DevID1-2	0x3FFFFE-F
	bufferU[j++]=2;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//CLRF TBLPTRU
	bufferU[j++]=0xF8;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//CLRF TBLPTRH
	bufferU[j++]=0xF7;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//CLRF TBLPTRL
	bufferU[j++]=0xF6;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(entry==2?11:4);
	for(z=0;bufferI[z]!=TBLR_INC_N&&z<DIMBUF;z++);
	if(z<DIMBUF-3){
		PrintMessage2(strings[S_DevID2],bufferI[z+3],bufferI[z+2]);	//"DevID: 0x%02X%02X\r\n"
		PIC18_ID(bufferI[z+2]+(bufferI[z+3]<<8));
	}
//****************** read code ********************
	PrintMessage(strings[S_CodeReading1]);		//code read ...
	PrintStatusSetup();
	if(saveLog)	fprintf(logfile,"Read code\n");
	for(i=0,j=0;i<dim;i+=DIMBUF-4){
		bufferU[j++]=TBLR_INC_N;
		bufferU[j++]=i<dim-(DIMBUF-4)?DIMBUF-4:dim-i;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		for(z=0;bufferI[z]!=TBLR_INC_N&&z<DIMBUF;z++);
		for(j=z+2;(j<z+2+bufferI[z+1])&&j<DIMBUF;j++) memCODE[k++]=bufferI[j];
		PrintStatus(strings[S_CodeReading2],i*100/(dim+dim2),i);	//"Read: %d%%, addr. %05X"
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
//****************** read config area ********************
	if(saveLog)	fprintf(logfile,"Read config\n");
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x0E;			//TBLPTRU	ID 0x200000
	bufferU[j++]=0x20;			//TBLPTRU	ID 0x200000
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6E;			//TBLPTRU
	bufferU[j++]=0xF8;			//TBLPTRU
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//TBLPTRH
	bufferU[j++]=0xF7;			//TBLPTRH
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//TBLPTRL
	bufferU[j++]=0xF6;			//TBLPTRL
	bufferU[j++]=TBLR_INC_N;
	bufferU[j++]=8;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x0E;			//TBLPTRU	CONFIG 0x300000
	bufferU[j++]=0x30;			//TBLPTRU	CONFIG 0x300000
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6E;			//TBLPTRU
	bufferU[j++]=0xF8;			//TBLPTRU
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//TBLPTRH
	bufferU[j++]=0xF7;			//TBLPTRH
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//TBLPTRL
	bufferU[j++]=0xF6;			//TBLPTRL
	bufferU[j++]=TBLR_INC_N;
	bufferU[j++]=14;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(8);
	for(z=0;bufferI[z]!=TBLR_INC_N&&z<DIMBUF-28;z++);
	if(z<DIMBUF-28){
		for(i=0;i<8;i++) memID[k2++]=bufferI[z+i+2];
		for(;i<14+8;i++) memCONFIG[-8+k2++]=bufferI[z+i+8];
	}
	j=0;
	if(saveLog){
		fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
	}
	if(k2!=22){
		PrintMessage2(strings[S_ReadConfigErr],22,k2);	//"Error reading config area, requested %d bytes, read %d\r\n"
	}
//****************** read eeprom ********************
	if(dim2){
		PrintMessage(strings[S_ReadEE]);		//read eeprom ...
		PrintStatusSetup();
		if(saveLog)	fprintf(logfile,"Read EEPROM\n");
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x9E;				//EEPGD=0
		bufferU[j++]=newAddr==0?0xA6:0x7F;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x9C;				//CFGS=0
		bufferU[j++]=newAddr==0?0xA6:0x7F;
		for(k2=0,i=0;i<dim2;i++){
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x0E;			//MOVLW
			bufferU[j++]=i&0xFF;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x6E;					//MOVWF
			bufferU[j++]=newAddr==0?0xA9:0x74;	//ADDR
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x0E;		//MOVLW
			bufferU[j++]=(i>>8)&0xFF;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x6E;					//MOVWF
			bufferU[j++]=newAddr==0?0xAA:0x75;	//ADDRH
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x80;			//RD=1
			bufferU[j++]=newAddr==0?0xA6:0x7F;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x50;			//MOVF EEDATA,w
			bufferU[j++]=newAddr==0?0xA8:0x73;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x6E;			//MOVWF TABLAT
			bufferU[j++]=0xF5;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x00;			//NOP
			bufferU[j++]=0x00;
			bufferU[j++]=SHIFT_TABLAT;
			if(j>DIMBUF-26||i==dim2-1){
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(10);
				for(z=0;z<DIMBUF-1;z++){
					if(bufferI[z]==SHIFT_TABLAT){
						memEE[k2++]=bufferI[z+1];
						z+=8;
					}
				}
				PrintStatus(strings[S_CodeReading],(i+dim)*100/(dim+dim2),i);	//"Read: %d%%, addr. %03X"
				if(RWstop) i=dim2;
				j=0;
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
	PrintMessage("\r\n");
//****************** exit ********************
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=1;
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(1);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
//****************** visualize ********************
	for(i=0;i<8;i+=2){
		PrintMessage4(strings[S_ChipID2],i,memID[i],i+1,memID[i+1]);	//"ID%d: 0x%02X   ID%d: 0x%02X\r\n"
	}
	for(i=0;i<7;i++){
		PrintMessage2(strings[S_ConfigWordH],i+1,memCONFIG[i*2+1]);	//"CONFIG%dH: 0x%02X\t"
		PrintMessage2(strings[S_ConfigWordL],i+1,memCONFIG[i*2]);	//"CONFIG%dL: 0x%02X\r\n"
	}
	PrintMessage(strings[S_CodeMem]);	//"\r\nCode memory:\r\n"
	DisplayCODE18F(dim);
	if(dim2){
		DisplayEE();	//visualize
	}
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

void Write18Fx(int dim,int dim2,int wbuf,int eraseW1,int eraseW2,int options)
// write 16 bit PIC 18Fxxxx
// dim=program size  (bytes)	dim2=eeprom size (bytes)	wbuf=write buffer size {<=64}
// eraseW1=erase word @3C0005	(not used if >= 0x10000)
// eraseW2=erase word @3C0004	(not used if >= 0x10000)
// options:
//	bit [3:0]
//     0 = vdd before vpp (12V)
//     1 = vdd before vpp (9V)
//     2 = low voltage entry with 32 bit key
//     3 = vdd before vpp (9V) with 32 bit key
//	bit [7:4]
//     0 = normal eeprom write algoritm
//     1 = with unlock sequence 55 AA
//	bit [11:8]
//     0 = 15ms erase delay, 1ms code write time, 5ms EE write, 5ms config write
//     1 = 550ms erase delay, 1.2ms code write time, no config or EEPROM
//     2 = 550ms erase delay, 3.4ms code write time, no config or EEPROM
//     3 = separate block erase with 5ms delay, 1ms code write, 5ms EE write, 5ms config write
//         + new control registers address (18FxxK80)
{
	int k=0,k2,z=0,i,j;
	int err=0;
	int EEalgo=(options>>4)&0xF,entry=options&0xF,optWrite=(options>>8)&0xF;
	if(dim>0x1FFFFF||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	if(dim2>0x800||dim2<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(wbuf>64){
		PrintMessage(strings[S_WbufLim]);	//"Write buffer size out of limits\r\n"
		return;
	}
	if(entry>0&&!CheckV33Regulator()){
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	double vpp=-1;
	if(entry==0) vpp=12;
	else if(entry==1||entry==3) vpp=8.5;
	if(!StartHVReg(vpp)){
		PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write18F(%d,%d,%d,%d)    (0x%X,0x%X,0x%X,0x%X)\n",dim,dim2,wbuf,options,dim,dim2,wbuf,options);
	}
	if(dim>size) dim=size;
	if(dim%wbuf){			//grow to an integer number of rows
		dim+=wbuf-dim%wbuf;
		j=size;
		if(j<dim){
			size=dim;
			memCODE=(unsigned char*)realloc(memCODE,size);
			for(;j<dim;j++) memCODE[j]=0xFF;
		}
	}
	if(dim2>sizeEE) dim2=sizeEE;
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Empty data area\r\n"
		return;
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	if(entry==2){					//LV entry with key
		bufferU[j++]=EN_VPP_VCC;		//VDD
		bufferU[j++]=0x1;
	}
	if(entry==2||entry==3){			//entry with key
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=TX16;
		bufferU[j++]=2;
		bufferU[j++]=0x4D;
		bufferU[j++]=0x43;
		bufferU[j++]=0x48;
		bufferU[j++]=0x50;
		bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
		bufferU[j++]=0x5;
		bufferU[j++]=WAIT_T3;
	}
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x0E;			//3F
	bufferU[j++]=0x3F;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6E;			//-> TBLPTRU
	bufferU[j++]=0xF8;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x0E;			//FF
	bufferU[j++]=0xFF;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6E;			//-> TBLPTRH
	bufferU[j++]=0xF7;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x0E;			//FE
	bufferU[j++]=0xFE;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6E;			//-> TBLPTRL
	bufferU[j++]=0xF6;
	bufferU[j++]=TBLR_INC_N;		//DevID1-2	0x3FFFFE-F
	bufferU[j++]=2;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=5100>>8;
	bufferU[j++]=5100&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(entry==2?10:3);
	for(z=0;bufferI[z]!=TBLR_INC_N&&z<DIMBUF;z++);
	if(z<DIMBUF-3){
		PrintMessage2(strings[S_DevID2],bufferI[z+3],bufferI[z+2]);	//"DevID: 0x%02X%02X\r\n"
		PIC18_ID(bufferI[z+2]+(bufferI[z+3]<<8));
	}
	j=0;
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	if(optWrite!=3){	//chip erase
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//3C
		bufferU[j++]=0x3C;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//-> TBLPTRU
		bufferU[j++]=0xF8;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6A;			//TBLPTRH=0
		bufferU[j++]=0xF7;
		if(eraseW1<0x10000){
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x0E;			//05
			bufferU[j++]=0x05;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x6E;			//-> TBLPTRL
			bufferU[j++]=0xF6;
			bufferU[j++]=TABLE_WRITE;		// eraseW1@3C0005
			bufferU[j++]=(eraseW1>>8)&0xFF; 	//0x3F;
			bufferU[j++]=eraseW1&0xFF; 		//0x3F;
		}
		if(eraseW2<0x10000){
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x0E;			//04
			bufferU[j++]=0x04;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x6E;			//-> TBLPTRL
			bufferU[j++]=0xF6;
			bufferU[j++]=TABLE_WRITE;		// eraseW2@3C0004
			bufferU[j++]=(eraseW2>>8)&0xFF; 	//0x8F;
			bufferU[j++]=eraseW2&0xFF; 		//0x8F;
		}
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(16);	
		if(optWrite!=0)	msDelay(550);//bulk erase delay
		j=0;
	}
	else{	//separate block erase
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 3C
		bufferU[j++]=0x3C;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRU
		bufferU[j++]=0xF8;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6A;			//CLRF TBLPTRH
		bufferU[j++]=0xF7;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 4
		bufferU[j++]=0x04;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x04 @ 0x3C0004
		bufferU[j++]=0x04;
		bufferU[j++]=0x04;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 5
		bufferU[j++]=0x05;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x01 @ 0x3C0005
		bufferU[j++]=0x01;
		bufferU[j++]=0x01;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 6
		bufferU[j++]=0x06;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x80 @ 0x3C0006
		bufferU[j++]=0x80;
		bufferU[j++]=0x80;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(7);	//block erase delay
		j=0;
		if(saveLog){
			fprintf(logfile,"ERASE BLOCK0\n");
		}
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 4
		bufferU[j++]=0x04;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x04 @ 0x3C0004
		bufferU[j++]=0x04;
		bufferU[j++]=0x04;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 5
		bufferU[j++]=0x05;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x02 @ 0x3C0005
		bufferU[j++]=0x02;
		bufferU[j++]=0x02;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 6
		bufferU[j++]=0x06;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x80 @ 0x3C0006
		bufferU[j++]=0x80;
		bufferU[j++]=0x80;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(7);	//block erase delay
		j=0;
		if(saveLog){
			fprintf(logfile,"ERASE BLOCK1\n");
		}
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 4
		bufferU[j++]=0x04;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x04 @ 0x3C0004
		bufferU[j++]=0x04;
		bufferU[j++]=0x04;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 5
		bufferU[j++]=0x05;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x04 @ 0x3C0005
		bufferU[j++]=0x04;
		bufferU[j++]=0x04;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 6
		bufferU[j++]=0x06;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x80 @ 0x3C0006
		bufferU[j++]=0x80;
		bufferU[j++]=0x80;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(7);	//block erase delay
		j=0;
		if(saveLog){
			fprintf(logfile,"ERASE BLOCK2\n");
		}
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 4
		bufferU[j++]=0x04;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x04 @ 0x3C0004
		bufferU[j++]=0x04;
		bufferU[j++]=0x04;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 5
		bufferU[j++]=0x05;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x08 @ 0x3C0005
		bufferU[j++]=0x08;
		bufferU[j++]=0x08;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 6
		bufferU[j++]=0x06;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x80 @ 0x3C0006
		bufferU[j++]=0x80;
		bufferU[j++]=0x80;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(7);	//block erase delay
		j=0;
		if(saveLog){
			fprintf(logfile,"ERASE BLOCK3\n");
		}
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 4
		bufferU[j++]=0x04;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x05 @ 0x3C0004
		bufferU[j++]=0x05;
		bufferU[j++]=0x05;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 5
		bufferU[j++]=0x05;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x00 @ 0x3C0005
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 6
		bufferU[j++]=0x06;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x80 @ 0x3C0006
		bufferU[j++]=0x80;
		bufferU[j++]=0x80;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(7);	//block erase delay
		j=0;
		if(saveLog){
			fprintf(logfile,"ERASE BOOT BLOCK\n");
		}
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 4
		bufferU[j++]=0x04;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x02 @ 0x3C0004
		bufferU[j++]=0x02;
		bufferU[j++]=0x02;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 5
		bufferU[j++]=0x05;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x00 @ 0x3C0005
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//MOVLW 6
		bufferU[j++]=0x06;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRL
		bufferU[j++]=0xF6;
		bufferU[j++]=TABLE_WRITE;	//0x80 @ 0x3C0006
		bufferU[j++]=0x80;
		bufferU[j++]=0x80;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=CORE_INS;		//NOP
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(7);	//block erase delay
		j=0;
		if(saveLog){
			fprintf(logfile,"ERASE CONFIG\n");
		}
		if(programID){
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x8E;			//EEPGD=1
			bufferU[j++]=0x7F;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x9C;			//CFGS=0
			bufferU[j++]=0x7F;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x84;			//WREN=1
			bufferU[j++]=0x7F;
			bufferU[j++]=CORE_INS;		//ID 0x200000
			bufferU[j++]=0x0E;			//MOVLW
			bufferU[j++]=0x20;			//
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x6E;			//MOVWF TBLPTRU
			bufferU[j++]=0xF8;			//
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x6A;			//CLRF TBLPTRH
			bufferU[j++]=0xF7;			//CLRF TBLPTRH
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x6A;			//CLRF TBLPTRL
			bufferU[j++]=0xF6;			//CLRF TBLPTRL
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x88;			//FREE=1
			bufferU[j++]=0x7F;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x82;			//WR=1
			bufferU[j++]=0x7F;
			bufferU[j++]=CORE_INS;		//NOP
			bufferU[j++]=0x00;
			bufferU[j++]=0x00;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(7);	//row erase delay
			j=0;
			if(saveLog){
				fprintf(logfile,"ERASE ID\n");
			}
		}
	}
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** prepare write ********************
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x8E;			//EEPGD=1
	bufferU[j++]=optWrite!=3?0xA6:0x7F;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x9C;			//CFCGS=0
	bufferU[j++]=optWrite!=3?0xA6:0x7F;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x84;			//WREN=1
	bufferU[j++]=optWrite!=3?0xA6:0x7F;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//TBLPTRU
	bufferU[j++]=0xF8;			//TBLPTRU
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//TBLPTRH
	bufferU[j++]=0xF7;			//TBLPTRH
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//TBLPTRL
	bufferU[j++]=0xF6;			//TBLPTRL
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(1);
	j=0;
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"code write ... "
	PrintStatusSetup();
	int ww;
	double wdly=1.0;
	if(optWrite==1) wdly=1.2;
	if(optWrite==2) wdly=3.4;
	if(saveLog){
		fprintf(logfile,"WRITE CODE\ndim=%d(0x%X)\n",dim,dim);	//
	}
	int valid,i0;
	j=0;
	for(i=k=0;i<dim;){		//write xx instruction words
		if(k==0){				//skip row if empty
			i0=i;
			for(valid=0;!valid&&i<dim;i+=valid?0:wbuf){
				for(k2=0;k2<wbuf&&!valid;k2++) if(memCODE[i+k2]<0xFF) valid=1;
			}
			if(i>=dim) break;
			if(i>i0){				//some rows were skipped; update current address
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x0E;
				bufferU[j++]=(i>>16)&0xFF;
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x6E;			//MOVWF TBLPTRU
				bufferU[j++]=0xF8;
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x0E;
				bufferU[j++]=(i>>8)&0xFF;
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x6E;			//MOVWF TBLPTRH
				bufferU[j++]=0xF7;
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x0E;
				bufferU[j++]=i&0xFF;
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x6E;			//MOVWF TBLPTRL
				bufferU[j++]=0xF6;
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(2);
				j=0;
			}
		}
		if(DIMBUF-4-j<wbuf-2-k)	ww=(DIMBUF-4-j)/2;	//split data for a total of wbuf-2
		else ww=(wbuf-2-k)/2;
		bufferU[j++]=TBLW_INC_N;
		bufferU[j++]=ww;
		for(z=0;z<ww;z++){
			bufferU[j++]=memCODE[i+1];
			bufferU[j++]=memCODE[i];
			i+=2;
		}
		k+=ww*2;
		if(k==wbuf-2){	//Write row
			if(j>DIMBUF-8){
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(3);
				j=0;
			}
			bufferU[j++]=TBLW_PROG_INC;
			bufferU[j++]=memCODE[i+1];
			bufferU[j++]=memCODE[i];
			if(optWrite==1){	//1.2ms
				bufferU[j++]=1200>>8;
				bufferU[j++]=1200&0xFF;
			}
			else if(optWrite==2){	//3.4ms
				bufferU[j++]=3400>>8;
				bufferU[j++]=3400&0xFF;
			}
			else{				//1ms
				bufferU[j++]=1000>>8;
				bufferU[j++]=1000&0xFF;
			}
			i+=2;
			k=0;
		}
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(k==0?wdly:2);
		j=0;
		PrintStatus(strings[S_CodeWriting2],i*100/(dim+dim2),i/2);	//"Write: %d%%,addr. %04X"
		if(RWstop) i=dim;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d, k=%d 0=%d\n"
		}
	}
	PrintStatusEnd();
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write ID ********************
	if(programID&&(optWrite==0||optWrite==3)){
		PrintMessage(strings[S_IDW]);	//"Write ID ... "
		if(saveLog)	fprintf(logfile,"Write ID\n");
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x8E;			//EEPGD=1
		bufferU[j++]=optWrite!=3?0xA6:0x7F;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x9C;			//CFCGS=0
		bufferU[j++]=optWrite!=3?0xA6:0x7F;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//TBLPTRU	ID 0x200000
		bufferU[j++]=0x20;			//TBLPTRU	ID 0x200000
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//TBLPTRU
		bufferU[j++]=0xF8;			//TBLPTRU
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6A;			//CLRF TBLPTRH
		bufferU[j++]=0xF7;			//CLRF TBLPTRH
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6A;			//CLRF TBLPTRL
		bufferU[j++]=0xF6;			//CLRF TBLPTRL
		bufferU[j++]=TBLW_INC_N;
		bufferU[j++]=3;
		for(i=0;i<3;i++){
			bufferU[j++]=memID[i*2+1];
			bufferU[j++]=memID[i*2];
		}
		bufferU[j++]=TBLW_PROG;
		bufferU[j++]=memID[i*2+1];
		bufferU[j++]=memID[i*2];
		bufferU[j++]=1000>>8;		//only one write so no need to specify different delay
		bufferU[j++]=1000&0xFF;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(5);
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d, k=%d 0=%d\n"
		}
		PrintMessage(strings[S_Compl]);	//"completed\r\n"
	}
//****************** write and verify EEPROM ********************
	if(dim2&&(optWrite==0||optWrite==3)){
		PrintMessage(strings[S_EEAreaW]);	//"Write EEPROM ... "
		PrintStatusSetup();
		if(saveLog)	fprintf(logfile,"Write EEPROM\n");
		int errEE=0;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x9E;			//EEPGD=0
		bufferU[j++]=optWrite!=3?0xA6:0x7F;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x9C;			//CFGS=0
		bufferU[j++]=optWrite!=3?0xA6:0x7F;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x84;			//WREN=1
		bufferU[j++]=optWrite!=3?0xA6:0x7F;
		for(i=0;i<dim2&&err<=max_err;i++){
			if(memEE[i]!=0xFF){
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x0E;			//MOVLW
				bufferU[j++]=i&0xFF;
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x6E;					//MOVWF
				bufferU[j++]=optWrite!=3?0xA9:0x74;	//ADDR
				if(EEalgo==0){
					bufferU[j++]=CORE_INS;
					bufferU[j++]=0x0E;		//MOVLW
					bufferU[j++]=(i>>8)&0xFF;
					bufferU[j++]=CORE_INS;
					bufferU[j++]=0x6E;					//MOVWF
					bufferU[j++]=optWrite!=3?0xAA:0x75;	//ADDRH
				}
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x0E;			//MOVLW
				bufferU[j++]=memEE[i];
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x6E;					//MOVWF
				bufferU[j++]=optWrite!=3?0xA8:0x73;	//EEDATA
				if(EEalgo==1){				//memory unlock
					bufferU[j++]=CORE_INS;
					bufferU[j++]=0x0E;		//MOVLW
					bufferU[j++]=0x55;
					bufferU[j++]=CORE_INS;
					bufferU[j++]=0x6E;		//MOVWF
					bufferU[j++]=0xA7;			//EECON2
					bufferU[j++]=CORE_INS;
					bufferU[j++]=0x0E;		//MOVLW
					bufferU[j++]=0xAA;
					bufferU[j++]=CORE_INS;
					bufferU[j++]=0x6E;		//MOVWF
					bufferU[j++]=0xA7;			//EECON2
				}
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x82;
				bufferU[j++]=optWrite!=3?0xA6:0x7F;	//WR=1
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x00;
				bufferU[j++]=0x00;			//NOP
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x00;
				bufferU[j++]=0x00;			//NOP
				bufferU[j++]=WAIT_T3;		//write delay
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x80;			//RD=1
				bufferU[j++]=optWrite!=3?0xA6:0x7F;
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x50;			//MOVF EEDATA,w
				bufferU[j++]=optWrite!=3?0xA8:0x73;
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x6E;			//MOVWF TABLAT
				bufferU[j++]=0xF5;
				bufferU[j++]=CORE_INS;
				bufferU[j++]=0x00;			//NOP
				bufferU[j++]=0x00;
				bufferU[j++]=SHIFT_TABLAT;
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(8);
				PrintStatus(strings[S_CodeWriting],(i+dim)*100/(dim+dim2),i);	//"Writing: %d%%, addr. %03X"
				if(RWstop) i=dim2;
				j=0;
				for(z=DIMBUF-1;z&&!(bufferI[z]==SHIFT_TABLAT&&bufferI[z-1]==CORE_INS);z--);
				if(z&&memEE[i]!=bufferI[z+1]) errEE++;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,errEE);	//"i=%d, k=%d, errors=%d\n"
				}
			}
		}
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],errEE);	//"completed: %d errors \r\n"
		err+=errEE;
	}
//****************** verify code ********************
	PrintMessage(strings[S_CodeV]);	//"Verify code ... "
	PrintStatusSetup();
	if(saveLog)fprintf(logfile,"VERIFY CODE\n");
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x8E;			//EEPGD=1
	bufferU[j++]=optWrite!=3?0xA6:0x7F;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x9C;			//CFCGS=0
	bufferU[j++]=optWrite!=3?0xA6:0x7F;
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//CLRF TBLPTRU
	bufferU[j++]=0xF8;			//CLRF TBLPTRU
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//CLRF TBLPTRH
	bufferU[j++]=0xF7;			//CLRF TBLPTRH
	bufferU[j++]=CORE_INS;
	bufferU[j++]=0x6A;			//CLRF TBLPTRL
	bufferU[j++]=0xF6;			//CLRF TBLPTRL
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	if(saveLog){
		fprintf(logfile,"\n\n");
	}
	for(i=0,j=0,k=0;i<dim;i+=DIMBUF-4){
		i0=i;
		for(valid=0;!valid&&i<dim;i+=valid?0:DIMBUF-4){		//skip verification if 0xFF
			for(k2=0;k2<DIMBUF-4&&!valid&&i+k2<dim;k2++) if(memCODE[i+k2]<0xFF) valid=1;
		}
		if(i>=dim) break;
		if(i>i0){				//some data was skipped; update current address
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x0E;
			bufferU[j++]=(i>>16)&0xFF;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x6E;			//MOVWF TBLPTRU
			bufferU[j++]=0xF8;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x0E;
			bufferU[j++]=(i>>8)&0xFF;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x6E;			//MOVWF TBLPTRH
			bufferU[j++]=0xF7;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x0E;
			bufferU[j++]=i&0xFF;
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x6E;			//MOVWF TBLPTRL
			bufferU[j++]=0xF6;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(2);
			j=0;
		}
		bufferU[j++]=TBLR_INC_N;
		bufferU[j++]=i<dim-(DIMBUF-4)?DIMBUF-4:dim-i;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		if(bufferI[1]==TBLR_INC_N){
			for(z=0;z<bufferI[2]&&z<DIMBUF;z++){
				if(memCODE[i+z]!=bufferI[z+3]){
					PrintMessage4(strings[S_CodeVError],i+z,i+z,memCODE[i+z],bufferI[z+3]);	//"Error writing address %4X: written %02X, read %02X\r\n"
					err++;
				}
				//k++;
			}
		}
		PrintStatus(strings[S_CodeV2],i*100/(dim+dim2),i);	//"Verifica: %d%%, ind. %04X"
		if(RWstop) i=dim;
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d\n"
		}
		if(err>=max_err) break;
	}
	PrintStatusEnd();
	if(i<dim){
		PrintMessage2(strings[S_CodeVError2],dim,i);	//"Error verifying code area, requested %d bytes, read %d\r\n"
	}
	PrintMessage1(strings[S_ComplErr],err);	//"completed: %d errors\r\n"
	if(err>=max_err){
		PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
	}
//****************** verify ID ********************
	if(programID&&err<max_err&&optWrite==0){
		PrintMessage(strings[S_IDV]);	//"Verify ID ... "
		int errID=0;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//TBLPTRU	ID 0x200000
		bufferU[j++]=0x20;			//TBLPTRU	ID 0x200000
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//TBLPTRU
		bufferU[j++]=0xF8;			//TBLPTRU
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6A;			//CLRF TBLPTRH
		bufferU[j++]=0xF7;			//CLRF TBLPTRH
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6A;			//CLRF TBLPTRL
		bufferU[j++]=0xF6;			//CLRF TBLPTRL
		bufferU[j++]=TBLR_INC_N;
		bufferU[j++]=8;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		for(z=0;bufferI[z]!=TBLR_INC_N&&z<DIMBUF;z++);
		for(i=0;i<8;i++) if(memID[i]!=0xFF&&memID[i]!=bufferI[z+i+2]) errID++;
		PrintMessage1(strings[S_ComplErr],errID);	//"completed: %d errors\r\n"
		err+=errID;
		if(err>=max_err){
			PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
		}
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log8],i,i,0,0,err);	//"i=%d, k=%d, errors=%d\n"
		}
	}
//****************** write CONFIG ********************
	if(err<max_err&&(optWrite==0||optWrite==3)){
		PrintMessage(strings[S_ConfigW]);	//"Write CONFIG ..."
		if(saveLog)	fprintf(logfile,"Write CONFIG\n");
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x8E;			//EEPGD=1
		bufferU[j++]=optWrite!=3?0xA6:0x7F;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x8C;			//CFCGS=1
		bufferU[j++]=optWrite!=3?0xA6:0x7F;
		bufferU[j++]=CORE_INS;		//CONFIG 0x300000
		bufferU[j++]=0x0E;			//MOVLW
		bufferU[j++]=0x30;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRU
		bufferU[j++]=0xF8;			//MOVWF TBLPTRU
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6A;			//CLRF TBLPTRH
		bufferU[j++]=0xF7;			//CLRF TBLPTRH
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6A;			//CLRF TBLPTRL
		bufferU[j++]=0xF6;			//CLRF TBLPTRL
		for(i=0;i<14;i++){
			if(memCONFIG[i]<0xFF){
				bufferU[j++]=TBLW_PROG;
				bufferU[j++]=0;
				bufferU[j++]=memCONFIG[i];
				bufferU[j++]=5000>>8;
				bufferU[j++]=5000&0xFF;
			}
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x2A;			//INCF
			bufferU[j++]=0xF6;			//TBLPTRL
			i++;
			if(memCONFIG[i]<0xFF){
				bufferU[j++]=TBLW_PROG;
				bufferU[j++]=memCONFIG[i];
				bufferU[j++]=0;
				bufferU[j++]=5000>>8;
				bufferU[j++]=5000&0xFF;
			}
			bufferU[j++]=CORE_INS;
			bufferU[j++]=0x2A;			//INCF
			bufferU[j++]=0xF6;			//TBLPTRL
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(12);
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d, k=%d\n"
			}
		}
		PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** verify CONFIG ********************
		PrintMessage(strings[S_ConfigV]);	//"Verify CONFIG ... "
		int errC=0;
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x0E;			//TBLPTRU	CONFIG 0x300000
		bufferU[j++]=0x30;			//
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6E;			//MOVWF TBLPTRU
		bufferU[j++]=0xF8;			//MOVWF TBLPTRU
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6A;			//CLRF TBLPTRH
		bufferU[j++]=0xF7;			//CLRF TBLPTRH
		bufferU[j++]=CORE_INS;
		bufferU[j++]=0x6A;			//CLRF TBLPTRL
		bufferU[j++]=0xF6;			//CLRF TBLPTRL
		bufferU[j++]=TBLR_INC_N;
		bufferU[j++]=14;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		for(z=0;bufferI[z]!=TBLR_INC_N&&z<DIMBUF-16;z++);
		if(z<DIMBUF-16){
			for(i=0;i<14;i++) if(~memCONFIG[i]&bufferI[z+i+2]) errC++;	//error if written 0 and read 1 (~W&R)
		}
		PrintMessage1(strings[S_ComplErr],errC);	//"completed: %d errors\r\n"
		err+=errC;
		if(err>=max_err){
			PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
		}
		j=0;
		if(saveLog){
			fprintf(logfile,strings[S_Log8],i,i,0,0,err);	//"i=%d, k=%d, errors=%d\n"
		}
	}
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=1;
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(1);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

#define LOAD_PC_ADDR 0x80
#define BULK_ERASE 0x18
#define ROW_ERASE_PROGRAM_MEM 0xF0
#define LOAD_NVM 0x00
#define LOAD_NVM_INC 0x02
#define READ_NVM 0xFC
#define READ_NVM_INC 0xFE
#define INC_ADDR8 0xF8
#define BEGIN_INT_PROG 0xE0
#define BEGIN_EXT_PROG 0xC0
#define PROGRAM_DATA 0xC0
#define PROGRAM_DATA_INC 0xE0
#define END_EXT_PROG 0x82

void Read18FKx(int dim,int dim2,int options)
// read 16 bit PIC 18FxxKx3 with new 8b commands
// dim=program size (bytes)
// dim2=if>0 use eeprom, size is automatic when reading DCI
// options:
//	bit [3:0]
//   	0 = vpp before vdd (8.5V)
//	bit [7:4] 	memory layout
//		code		EE			DCI			DIA		model
//		 0		0x310000	0x3FFF00	0x3F0000	K42 K83
//		 1		0x380000	0x3C0000	0x2C0000	Q43
{
	int k=0,k2=0,z=0,i,j;
	int devID=0,devREV=0;
	int MemAddr;
	int EEaddr=0,DCIaddr=0,DIAaddr=0,DIAlen=0,UIDlen=0,CONFIGlen=0;
	int type=(options>>4)&0xF;
	if(type==0){		//K42 K83
		EEaddr=0x310000;
		DCIaddr=0x3FFF00;
		DIAaddr=0x3F0000;
		DIAlen=0x40;
		UIDlen=0x10;
		CONFIGlen=5;
	}
	else if(type==1){		//Q43
		EEaddr=0x380000;
		DCIaddr=0x3C0000;
		DIAaddr=0x2C0000;
		DIAlen=0x100;
		UIDlen=0x40;
		CONFIGlen=10;
	}
	if(dim>0x1FFFFF||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	if(dim2>0x800||dim2<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(!CheckV33Regulator()){
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	double vpp=8.5;
	if(!StartHVReg(vpp)){
		PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Read18FKx(%d,%d,%d)    (0x%X,0x%X,0x%X)\n",dim,dim2,options,dim,dim2,options);
	}
	size=dim;
	if(memCODE) free(memCODE);
	memCODE=(unsigned char*)malloc(size);		//CODE
	unsigned char* memDIA=(unsigned char*)malloc(DIAlen);		//DIA
	for(j=0;j<64;j++) memID[j]=0xFF;
	for(j=0;j<14;j++) memCONFIG[j]=0xFF;
	for(j=0;j<DIAlen;j++) memDIA[j]=0xFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=2;						//T1=2u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=WAIT_T3;
	MemAddr=0x3FFFFc<<1;		//devREV
	bufferU[j++]=TX16;
	bufferU[j++]=2;
	bufferU[j++]=0x80;
	bufferU[j++]=MemAddr>>16;
	bufferU[j++]=(MemAddr>>8)&0xFF;
	bufferU[j++]=MemAddr&0xFF;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	MemAddr=DCIaddr<<1;		//DCI
	bufferU[j++]=TX16;
	bufferU[j++]=2;
	bufferU[j++]=0x80;
	bufferU[j++]=MemAddr>>16;
	bufferU[j++]=(MemAddr>>8)&0xFF;
	bufferU[j++]=MemAddr&0xFF;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	bufferU[j++]=TX16;
	bufferU[j++]=2;
	bufferU[j++]=0x80;
	bufferU[j++]=0x0;
	bufferU[j++]=0x0;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(4);
	for(z=0;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		devREV=(bufferI[z+1]<<8)+bufferI[z+2];
	}
	for(z+=3;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		devID=(bufferI[z+1]<<8)+bufferI[z+2];
	}
	PrintMessage1("DevID: 0x%04X\r\n",devID);
	PrintMessage1("RevID: 0x%04X\r\n",devREV);
	PIC18_ID(devID+(devREV<<16));
	PrintMessage("Device Configuration Information:\r\n");
	for(z+=3;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		PrintMessage1("Erase row size: %d\r\n",(bufferI[z+1]<<8)+bufferI[z+2]);
	}
	for(z+=3;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		PrintMessage1("Write latches per row: %d\r\n",(bufferI[z+1]<<8)+bufferI[z+2]);
	}
	for(z+=3;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		PrintMessage1("User rows: %d\r\n",(bufferI[z+1]<<8)+bufferI[z+2]);
	}
	for(z+=3;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		PrintMessage1("EEPROM size: %d\r\n",(bufferI[z+1]<<8)+bufferI[z+2]);
		sizeEE=(bufferI[z+1]<<8)+bufferI[z+2];
	}
	for(z+=3;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-3){
		PrintMessage1("Pin count: %d\r\n",(bufferI[z+1]<<8)+bufferI[z+2]);
	}
//****************** read code ********************
	PrintMessage(strings[S_CodeReading1]);		//read code ...
	if(saveLog) fprintf(logfile,"%s\n",strings[S_CodeReading1]);		//read code ...
	PrintStatusSetup();
	for(i=0,j=0;i<dim;i+=2){		//2 bytes per read
		bufferU[j++]=ICSP8_READ;
		bufferU[j++]=READ_NVM_INC;
		if((j+1)/2*3+3>DIMBUF||i==dim-2){		//2B cmd -> 3B data
			bufferU[j++]=FLUSH;				//remember: FLUSH generates a response (even empty)!
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(z=0;z<DIMBUF-2;z++){
				if(bufferI[z]==ICSP8_READ){
					memCODE[k++]=bufferI[z+2];	//Low byte
					memCODE[k++]=bufferI[z+1];	//High byte
					z+=2;
				}
			}
			PrintStatus(strings[S_CodeReading],(i+sizeEE+10+UIDlen+DIAlen)*100/(dim+sizeEE+10+UIDlen+DIAlen),i);	//"Read: %d%%, addr. %03X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadCodeErr2],dim,k);	//"Error reading code area, requested %d bytes, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);
//****************** read config area ********************
	PrintMessage(strings[S_Read_CONFIG_A]);		//read config ...
	if(saveLog) fprintf(logfile,"%s\n",strings[S_Read_CONFIG_A]);		//read config ...
	MemAddr=0x300000<<1;		//config area
	bufferU[j++]=TX16;
	bufferU[j++]=2;
	bufferU[j++]=0x80;
	bufferU[j++]=MemAddr>>16;
	bufferU[j++]=(MemAddr>>8)&0xFF;
	bufferU[j++]=MemAddr&0xFF;
	k2=0;
	for(i=0;i<CONFIGlen;i++){		//Config
		bufferU[j++]=ICSP8_READ;
		bufferU[j++]=READ_NVM_INC;
		if((j+1)/2*3+3>DIMBUF||i==CONFIGlen-1){		//2B cmd -> 3B data
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(z=0;z<DIMBUF-2;z++){
				if(bufferI[z]==ICSP8_READ){
					memCONFIG[k2++]=bufferI[z+2];	//LSB
					if(type==0) memCONFIG[k2++]=bufferI[z+1];	//MSB
					z+=2;
				}
			}
			PrintStatus(strings[S_CodeReading],(dim+sizeEE+i+UIDlen+DIAlen)*100/(dim+sizeEE+CONFIGlen+UIDlen+DIAlen),i);	//"Read: %d%%, addr %03X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
	}
	if(k2!=10){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadConfigErr],10,k2);	//"Error reading config area, requested %d bytes, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);
//****************** read user ID ********************
	if(saveLog) fprintf(logfile,"Read user ID\n");
	MemAddr=0x200000<<1;		//user ID
	bufferU[j++]=TX16;
	bufferU[j++]=2;
	bufferU[j++]=0x80;
	bufferU[j++]=MemAddr>>16;
	bufferU[j++]=(MemAddr>>8)&0xFF;
	bufferU[j++]=MemAddr&0xFF;
	k2=0;
	for(i=0;i<UIDlen;i+=2){
		bufferU[j++]=ICSP8_READ;
		bufferU[j++]=READ_NVM_INC;
		if((j+1)/2*3+3>DIMBUF||i==UIDlen-2){		//2B cmd -> 3B data
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(z=0;z<DIMBUF-2;z++){
				if(bufferI[z]==ICSP8_READ){
					memID[k2++]=bufferI[z+2];	//Low byte
					memID[k2++]=bufferI[z+1];	//High byte
					z+=2;
				}
			}
			PrintStatus(strings[S_CodeReading],(dim+sizeEE+10+i+DIAlen)*100/(dim+sizeEE+10+UIDlen+DIAlen),i);	//"Read: %d%%, addr. %03X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
	}
	if(k2!=UIDlen){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadConfigErr],UIDlen,k2);	//"Error reading config area, requested %d bytes, read %d\r\n"
	}
//****************** read device info area (DIA) ********************
	if(saveLog) fprintf(logfile,"Read device info area\n");
	MemAddr=DIAaddr<<1;		//DIA
	bufferU[j++]=TX16;
	bufferU[j++]=2;
	bufferU[j++]=0x80;
	bufferU[j++]=MemAddr>>16;
	bufferU[j++]=(MemAddr>>8)&0xFF;
	bufferU[j++]=MemAddr&0xFF;
	k2=0;
	for(i=0;i<DIAlen;i+=2){		//DIA
		bufferU[j++]=ICSP8_READ;
		bufferU[j++]=READ_NVM_INC;
		if((j+1)/2*3+3>DIMBUF||i==DIAlen-2){		//2B cmd -> 3B data
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(z=0;z<DIMBUF-2;z++){
				if(bufferI[z]==ICSP8_READ){
					memDIA[k2++]=bufferI[z+2];
					memDIA[k2++]=bufferI[z+1];
					z+=2;
				}
			}
			PrintStatus(strings[S_CodeReading],(dim+sizeEE+10+UIDlen+i)*100/(dim+sizeEE+10+UIDlen+DIAlen),i);	//"Read: %d%%, addr. %03X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
	}
	if(k2!=DIAlen){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadConfigErr],DIAlen,k2);	//"Error reading config area, requested %d bytes, read %d\r\n"
	}
	//****************** read eeprom ********************
	if(dim2){					//EEPROM
		if(sizeEE>0x1000||sizeEE<0){		//Max 4K
			PrintMessage(strings[S_EELim]);	//"EEPROM size exceeds limits\r\n"
			return;
		}
		if(memEE) free(memEE);
		memEE=(unsigned char*)malloc(sizeEE);			//EEPROM
		for(i=0;i<sizeEE;i++) memEE[i]=0xFF;
		PrintMessage(strings[S_ReadEE]);		//Read EEPROM ...
		if(saveLog) fprintf(logfile,"%s\n",strings[S_ReadEE]);		//Read EEPROM ...
		MemAddr=EEaddr<<1;		//EE base address
		bufferU[j++]=TX16;
		bufferU[j++]=2;
		bufferU[j++]=0x80;
		bufferU[j++]=MemAddr>>16;
		bufferU[j++]=(MemAddr>>8)&0xFF;
		bufferU[j++]=MemAddr&0xFF;
		for(i=k=0;i<sizeEE;i++){
			bufferU[j++]=ICSP8_READ;
			bufferU[j++]=READ_NVM_INC;
			if((j+1)/2*3+3>DIMBUF||i==sizeEE-1){		//2B cmd -> 3B answer
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(5);
				for(z=0;z<DIMBUF-2;z++){
					if(bufferI[z]==ICSP8_READ){
						memEE[k++]=bufferI[z+2];	//LSB
						z+=2;
					}
				}
				PrintStatus(strings[S_CodeReading],(dim+i+10+UIDlen+DIAlen)*100/(dim+sizeEE+10+UIDlen+DIAlen),i);	//"Read: %d%%, addr %03X"
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
			}
		}
		PrintStatusEnd();
		if(k!=sizeEE){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ReadEEErr],sizeEE,k);	//"Error reading EE area, ..."
		}
		else PrintMessage(strings[S_Compl]);
	}
//****************** exit ********************
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=1;
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(1);
	unsigned int stop=GetTickCount();
	PrintStatusClear();
//****************** visualize ********************
	PrintMessage(strings[S_ConfigMem]);	//"CONFIG"
	for(i=0;i<UIDlen;i+=2){
		if(memID[i]<0xFF||memID[i+1]<0xFF) PrintMessage4(strings[S_ChipID2],i,memID[i],i+1,memID[i+1]);	//"ID%d: 0x%02X   ID%d: 0x%02X\r\n"
	}
	if(type==0){	//K42 K83 etc.
		for(i=0;i<5;i++){
			PrintMessage2(strings[S_ConfigWordH],i+1,memCONFIG[i*2+1]);	//"CONFIG%dH: 0x%02X\t"
			PrintMessage2(strings[S_ConfigWordL],i+1,memCONFIG[i*2]);	//"CONFIG%dL: 0x%02X\r\n"
		}
	}
	else if(type==1){	//Q43 etc.
		for(i=0;i<10;i++)	PrintMessage2("CONFIG%d: 0x%02X\r\n",i+1,memCONFIG[i]);
	}
	PrintMessage(strings[S_CodeMem]);	//"\r\nCode memory:\r\n"
	DisplayCODE18F(dim);
	if(dim2){
		DisplayEE();	//visualize
	}
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
}

void Write18FKx(int dim,int dim2,int options,int nu1,int nu2, int nu3)
// write 16 bit PIC 18FxxKx3 with new 8b commands
// dim=program size (bytes) 	dim2=eeprom size (bytes)
// options:
//	bit [3:0]
//   	0 = vpp before vdd (8.5V)
//	bit [7:4] 	memory layout
//		code		EE			DCI			DIA		model	program cycle	delays ms (E,W,EE)
//		 0		0x310000	0x3FFF00	0x3F0000	K42 K83	multiple-word	25.5 2.8 5.6
//		 1		0x380000	0x3C0000	0x2C0000	Q43    	one-word 		11 0.075 11
{
	int k=0,k2,z=0,i,j,x,w=0;
	int err=0;
	int devID=0,devREV=0;
	int MemAddr,rowN=0;
	int EEaddr=0,DCIaddr=0,DIAaddr=0,DIAlen=0,UIDlen=0,CONFIGlen=0;
	int type=(options>>4)&0xF;
	if(type==0){		//K42 K83
		EEaddr=0x310000;
		DCIaddr=0x3FFF00;
		DIAaddr=0x3F0000;
		DIAlen=0x40;
		UIDlen=0x10;
		CONFIGlen=5;
	}
	else if(type==1){		//Q43
		EEaddr=0x380000;
		DCIaddr=0x3C0000;
		DIAaddr=0x2C0000;
		DIAlen=0x100;
		UIDlen=0x40;
		CONFIGlen=10;
	}
	if(dim>0x1FFFFF||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size out of limits\r\n"
		return;
	}
	if(dim2>0x800||dim2<0){
		PrintMessage(strings[S_EELim]);	//"EEPROM size out of limits\r\n"
		return;
	}
	if(!CheckV33Regulator()){
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	double vpp=8.5;
	if(!StartHVReg(vpp)){
		PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write18FKx(%d,%d,%d)    (0x%X,0x%X,0x%X)\n",dim,dim2,options,dim,dim2,options);
	}
	if(dim>size) dim=size;
	if(dim2>sizeEE) dim2=sizeEE;
	if(dim<1){
		PrintMessage(strings[S_NoCode]);	//"Empty data area\r\n"
		return;
	}
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=2;						//T1=2u
	bufferU[j++]=35;					//T2 adjusted to obtain ~80us when needed
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=11000>>8;
	bufferU[j++]=11000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=WAIT_T3;
	MemAddr=0x3FFFFc<<1;		//devREV
	bufferU[j++]=TX16;
	bufferU[j++]=2;
	bufferU[j++]=0x80;
	bufferU[j++]=MemAddr>>16;
	bufferU[j++]=(MemAddr>>8)&0xFF;
	bufferU[j++]=MemAddr&0xFF;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	MemAddr=DCIaddr<<1;		//DCI
	bufferU[j++]=TX16;
	bufferU[j++]=2;
	bufferU[j++]=0x80;
	bufferU[j++]=MemAddr>>16;
	bufferU[j++]=(MemAddr>>8)&0xFF;
	bufferU[j++]=MemAddr&0xFF;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;
	bufferU[j++]=TX16;
	bufferU[j++]=2;
	bufferU[j++]=0x80;
	bufferU[j++]=0x0;
	bufferU[j++]=0x0;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(14);
	for(z=0;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		devREV=(bufferI[z+1]<<8)+bufferI[z+2];
	}
	for(z+=3;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		devID=(bufferI[z+1]<<8)+bufferI[z+2];
	}
	PrintMessage1("DevID: 0x%04X\r\n",devID);
	PrintMessage1("RevID: 0x%04X\r\n",devREV);
	PIC18_ID(devID+(devREV<<16));
	PrintMessage("Device Configuration Information:\r\n");
	for(z+=3;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		PrintMessage1("Erase row size: %d\r\n",(bufferI[z+1]<<8)+bufferI[z+2]);
	}
	for(z+=3;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		rowN=(bufferI[z+1]<<8)+bufferI[z+2];
		PrintMessage1("Write latches per row: %d\r\n",rowN);
	}
	for(z+=3;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		PrintMessage1("User rows: %d\r\n",(bufferI[z+1]<<8)+bufferI[z+2]);
	}
	for(z+=3;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-2){
		PrintMessage1("EEPROM size: %d\r\n",(bufferI[z+1]<<8)+bufferI[z+2]);
		sizeEE=(bufferI[z+1]<<8)+bufferI[z+2];
	}
	for(z+=3;bufferI[z]!=ICSP8_READ&&z<DIMBUF;z++);
	if(z<DIMBUF-3){
		PrintMessage1("Pin count: %d\r\n",(bufferI[z+1]<<8)+bufferI[z+2]);
	}
	j=0;
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	if(type==0){		//K42 K83
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=25500>>8;
		bufferU[j++]=25500&0xff;
		MemAddr=0x300000<<1;	//address to erase Flash+User+Config
		bufferU[j++]=TX16;
		bufferU[j++]=2;
		bufferU[j++]=0x80;
		bufferU[j++]=MemAddr>>16;
		bufferU[j++]=(MemAddr>>8)&0xFF;
		bufferU[j++]=MemAddr&0xFF;
		bufferU[j++]=ICSP8_SHORT;
		bufferU[j++]=BULK_ERASE;
		bufferU[j++]=WAIT_T3;	//TERAB
		MemAddr=0x310000<<1;	//address to erase EEPROM
		bufferU[j++]=TX16;
		bufferU[j++]=2;
		bufferU[j++]=0x80;
		bufferU[j++]=MemAddr>>16;
		bufferU[j++]=(MemAddr>>8)&0xFF;
		bufferU[j++]=MemAddr&0xFF;
		bufferU[j++]=ICSP8_SHORT;
		bufferU[j++]=BULK_ERASE;
		bufferU[j++]=WAIT_T3;	//TERAB
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=2800>>8;
		bufferU[j++]=2800&0xff;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(52);
	}
	else if(type==1){	//Q43
		bufferU[j++]=ICSP8_LOAD;
		bufferU[j++]=BULK_ERASE;
		bufferU[j++]=0;
		bufferU[j++]=0xF;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(1);
		msDelay(12);		//bulk erase delay
	}
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"code write ... "
	PrintStatusSetup();
	if(saveLog){
		fprintf(logfile,"WRITE CODE\ndim=%d(0x%X)\n",dim,dim);	//
	}
	if(type==1){	//Q43
		j=0;
		bufferU[j++]=TX16;
		bufferU[j++]=2;
		bufferU[j++]=0x80;
		bufferU[j++]=0x0;
		bufferU[j++]=0x0;
		bufferU[j++]=0x0;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		for(i=k=0;i<dim;i+=2){
			if(memCODE[i]<0xFF||memCODE[i+1]<0xFF){
				bufferU[j++]=ICSP8_LOAD;
				bufferU[j++]=PROGRAM_DATA;
				bufferU[j++]=memCODE[i+1];	//High byte
				bufferU[j++]=memCODE[i];	//Low byte
				bufferU[j++]=WAIT_T2;		//Tprogram 75us
				bufferU[j++]=ICSP8_READ;
				bufferU[j++]=READ_NVM_INC;
			}
			else{
				for(x=0;i+x<dim&&memCODE[i+x]==0xFF&&memCODE[i+x+1]==0xFF;x+=2);
				if(x>9){	//set new address
					i+=x;
					MemAddr=i<<1;
					bufferU[j++]=TX16;
					bufferU[j++]=2;
					bufferU[j++]=0x80;
					bufferU[j++]=MemAddr>>16;
					bufferU[j++]=(MemAddr>>8)&0xFF;
					bufferU[j++]=MemAddr&0xFF;
					i-=2;
				}
				else{
					bufferU[j++]=ICSP8_SHORT;
					bufferU[j++]=INC_ADDR8;
				}
			}
			if(j>DIMBUF-8||i==dim-2){
				PrintStatus(strings[S_CodeWriting],i*100/(dim+dim2),i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(2);
				for(z=0;z<DIMBUF-4;z++){
					if(bufferI[z]==ICSP8_SHORT&&memCODE[k]==0xff&&memCODE[k+1]==0xff) k+=2;
					else if(bufferI[z]==TX16){		//need to find the next valid address
						for(;k<dim&&memCODE[k]==0xFF&&memCODE[k+1]==0xFF;k+=2);
					}
					else if(bufferI[z]==ICSP8_LOAD&&bufferI[z+2]==ICSP8_READ){
						if(memCODE[k]!=bufferI[z+4]||memCODE[k+1]!=bufferI[z+3]){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError2],k,(memCODE[k+1]<<8)+memCODE[k],(bufferI[z+3]<<8)+bufferI[z+4]);	//"Error writing address %3X: written %04X, read %04X\r\n"
							err++;
							if(max_err&&err>max_err){
								PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								i=dim;
								z=DIMBUF;
							}
						}
						k+=2;
						z+=4;
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d,\n"
				}
			}
		}
		err+=i-k;
	}
	else if(type==0){		//K42 K83
		int valid;
		for(;dim>0&&memCODE[dim-1]==0xff&&memCODE[dim-2]==0xff;dim-=2); //skip empty space at end
		if(dim%rowN) dim+=rowN-dim%rowN;		//grow to N byte multiple
		if(dim>size){
			memCODE=(unsigned char*)realloc(memCODE,dim);
			for(;size<dim;size++) memCODE[size]=0xFF;
		}
		for(i=k=0,j=0;i<dim;i+=rowN){
			for(valid=0;i<dim&&!valid;i+=valid?0:rowN){	//skip empty locations (N words)
				valid=0;
				for(k=0;k<rowN;k++){	//check row
					if(memCODE[i+k]<0xff){
						valid=1;
						k=rowN;
					}
				}
			}
			MemAddr=i<<1;
			bufferU[j++]=TX16;	//load new address
			bufferU[j++]=2;
			bufferU[j++]=0x80;
			bufferU[j++]=MemAddr>>16;
			bufferU[j++]=(MemAddr>>8)&0xFF;
			bufferU[j++]=MemAddr&0xFF;
			for(k=0;k<rowN;k+=2){	//load all latches
				bufferU[j++]=ICSP8_LOAD;
				bufferU[j++]=k<(rowN-2)?LOAD_NVM_INC:LOAD_NVM;
				bufferU[j++]=memCODE[i+k+1];	//High byte
				bufferU[j++]=memCODE[i+k];	//Low byte
				if(j>DIMBUF-5){
					bufferU[j++]=FLUSH;
					for(;j<DIMBUF;j++) bufferU[j]=0x0;
					PacketIO(2);
					j=0;
				}
			}
			if(i<dim){
				bufferU[j++]=ICSP8_SHORT;
				bufferU[j++]=BEGIN_INT_PROG;			//internally timed, T=2.8ms
				bufferU[j++]=WAIT_T3;
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(6);
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d, k=%d 0=%d\n"
				}
				PrintStatus(strings[S_CodeWriting],i*100/(dim*2+dim2),i);	//"Writing: %d%%, addr. %03X"
			}
		}
		PrintStatusEnd();
		PrintMessage(strings[S_Compl]);	//"completed\r\n"
		//****************** verify code ********************
		PrintMessage(strings[S_CodeV]);	//"Verifying code ... "
		PrintStatusSetup();
		if(saveLog)	fprintf(logfile,"%s\n",strings[S_CodeV]);
		j=0;
		bufferU[j++]=TX16;
		bufferU[j++]=2;
		bufferU[j++]=0x80;
		bufferU[j++]=0x0;
		bufferU[j++]=0x0;
		bufferU[j++]=0x0;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
		j=0;
		for(i=k=0;i<dim;i+=2){
			if(j==0){				//skip empty locations (only at the beginning of a packet)
				for(x=0;i+x<dim&&memCODE[i+x]==0xFF&&memCODE[i+x+1]==0xFF;x+=2);
				if(x>9){	//set new address
					i+=x;
					k=i;
					MemAddr=i<<1;
					bufferU[j++]=TX16;
					bufferU[j++]=2;
					bufferU[j++]=0x80;
					bufferU[j++]=MemAddr>>16;
					bufferU[j++]=(MemAddr>>8)&0xFF;
					bufferU[j++]=MemAddr&0xFF;
				}
			}
			bufferU[j++]=ICSP8_READ;
			bufferU[j++]=READ_NVM_INC;
			if((j+1)/2*3+3>DIMBUF||i==dim-2){		//2B cmd -> 3B data
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(2);
				for(z=0;z<DIMBUF-2;z++){
					if(bufferI[z]==ICSP8_READ){
						if(memCODE[k]!=bufferI[z+2]||memCODE[k+1]!=bufferI[z+1]){
							PrintMessage3(strings[S_CodeWError2],k,memCODE[k]+(memCODE[k+1]<<8),(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing address %3X: written %04X, read %04X\r\n"
							err++;
						}
						z+=2;
						k+=2;
					}
				}
				PrintStatus(strings[S_CodeV2],(i+dim)/(dim*2+dim2)*100,i);	//"Verify: %d%%, addr. %04X"
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d\n"
				}
				if(err>=max_err){
					PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
					i=dim;
				}	
			}
		}
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=5600>>8;
		bufferU[j++]=5600&0xff;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(1);
		j=0;
	}
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write and verify EEPROM ********************
	if(dim2){
		PrintMessage(strings[S_EEAreaW]);	//"Write EEPROM ... "
		PrintStatusSetup();
		if(saveLog)	fprintf(logfile,"WRITE EEPROM\ndim2=%d(0x%X)\n",dim2,dim2);
		int errEE=0;
		j=0;
		MemAddr=EEaddr<<1;
		bufferU[j++]=TX16;
		bufferU[j++]=2;
		bufferU[j++]=0x80;
		bufferU[j++]=MemAddr>>16;
		bufferU[j++]=(MemAddr>>8)&0xFF;
		bufferU[j++]=MemAddr&0xFF;
		for(i=k=0;i<dim2;i++){
			if(memEE[i]<0xFF){
				if(type==1){	//Q43
					bufferU[j++]=ICSP8_LOAD;
					bufferU[j++]=PROGRAM_DATA;
					bufferU[j++]=0;
					bufferU[j++]=memEE[i];
				}
				else if(type==0){		//K42 K83
					bufferU[j++]=ICSP8_LOAD;
					bufferU[j++]=LOAD_NVM;
					bufferU[j++]=0;	//High byte
					bufferU[j++]=memEE[i];	//Low byte
					bufferU[j++]=ICSP8_SHORT;
					bufferU[j++]=BEGIN_INT_PROG;			//internally timed
				}
				bufferU[j++]=WAIT_T3;				//Tprogram 11ms or 5.6ms
				bufferU[j++]=ICSP8_READ;
				bufferU[j++]=READ_NVM_INC;
				w++;
			}
			else{
				bufferU[j++]=ICSP8_SHORT;
				bufferU[j++]=INC_ADDR8;
			}
			if(j>DIMBUF-8||i==dim2-1){
				PrintStatus(strings[S_CodeWriting],(i+dim)*100/(dim+dim2),i+EEaddr);	//"Writing: %d%%, addr. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				w*=type==1?11:5.6;	//total write time
				PacketIO(w+10);
				for(z=0;z<DIMBUF-4;z++){
					if(bufferI[z]==ICSP8_SHORT&&memEE[k]==0xff) k++;
					else if(bufferI[z]==ICSP8_LOAD){
						w=type==1?bufferI[z+4]:bufferI[z+5];
						if(memEE[k]!=w){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError2],k,memEE[k],w);	//"Error writing address %3X: written %04X, read %04X\r\n"
							errEE++;
							if(max_err&&err+errEE>max_err){
								PrintMessage1(strings[S_MaxErr],err+errEE);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"Write interrupted"
								i=dim2;
								z=DIMBUF;
							}
						}
						k++;
						z+=type==1?4:5;
					}
				}
				j=0;
				w=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,errEE);	//"i=%d, k=%d, errors=%d,\n"
				}
			}
		}
		errEE+=i-k;
		err+=errEE;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],errEE);	//"completed, %d errors\r\n"
	}
//****************** write and verify ID ********************
	#define IDaddr 0x200000
	if(programID){
		PrintMessage(strings[S_IDW]);	//"Write ID ... "
		if(saveLog)	fprintf(logfile,"Write ID\n");
		int errID=0;
		j=0;
		MemAddr=IDaddr<<1;
		bufferU[j++]=TX16;
		bufferU[j++]=2;
		bufferU[j++]=0x80;
		bufferU[j++]=MemAddr>>16;
		bufferU[j++]=(MemAddr>>8)&0xFF;
		bufferU[j++]=MemAddr&0xFF;
		for(i=k=0;i<UIDlen;i+=2){
			if(memID[i]<0xFF||memID[i+1]<0xFF){
				if(type==1){	//Q43
					bufferU[j++]=ICSP8_LOAD;
					bufferU[j++]=PROGRAM_DATA;
					bufferU[j++]=memID[i+1];
					bufferU[j++]=memID[i];
				}
				else if(type==0){		//K42 K83
					bufferU[j++]=ICSP8_LOAD;
					bufferU[j++]=LOAD_NVM;
					bufferU[j++]=memID[i+1];
					bufferU[j++]=memID[i];
					bufferU[j++]=ICSP8_SHORT;
					bufferU[j++]=BEGIN_INT_PROG;			//internally timed
				}
				bufferU[j++]=WAIT_T3;				//Tprogram 11ms or 5.6ms
				bufferU[j++]=ICSP8_READ;
				bufferU[j++]=READ_NVM_INC;
			}
			else{
				bufferU[j++]=ICSP8_SHORT;
				bufferU[j++]=INC_ADDR8;
			}
			if(j>DIMBUF-8||i==UIDlen-2){
				PrintStatus(strings[S_CodeWriting],99,IDaddr+i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(80);
				for(z=0;z<DIMBUF-4;z++){
					if(bufferI[z]==ICSP8_SHORT&&memID[k]==0xff&&memID[k+1]==0xff) k+=2;
					else if(bufferI[z]==ICSP8_LOAD){
						if(type==1) w=(bufferI[z+3]<<8)+bufferI[z+4];
						else if(type==0) w=(bufferI[z+4]<<8)+bufferI[z+5];
						x=(memID[k+1]<<8)+memID[k];
						if(x!=w){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError2],k,x,w);	//"Error writing address %3X: written %04X, read %04X\r\n"
							errID++;
							if(max_err&&err+errID>max_err){
								PrintMessage1(strings[S_MaxErr],err+errID);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"Write interrupted"
								i=UIDlen;
								z=DIMBUF;
							}
						}
						k+=2;
						z+=type==1?4:5;
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,errID);	//"i=%d, k=%d, errors=%d,\n"
				}
			}
		}
		errID+=i-k;
		err+=errID;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],errID);	//"completed, %d errors\r\n"
	}
//****************** write and verify CONFIG ********************
	#define CONFIGaddr 0x300000
	if(err<max_err){
		PrintMessage(strings[S_ConfigW]);	//"Write CONFIG ..."
		if(saveLog)	fprintf(logfile,"Write CONFIG\n");
		int errC=0;
		j=0;
		MemAddr=CONFIGaddr<<1;
		bufferU[j++]=TX16;
		bufferU[j++]=2;
		bufferU[j++]=0x80;
		bufferU[j++]=MemAddr>>16;
		bufferU[j++]=(MemAddr>>8)&0xFF;
		bufferU[j++]=MemAddr&0xFF;
		for(i=k=0;i<CONFIGlen;i++){
			if(type==1){	//Q43
				bufferU[j++]=ICSP8_LOAD;
				bufferU[j++]=PROGRAM_DATA;
				bufferU[j++]=0;
				bufferU[j++]=memCONFIG[i];
			}
			else if(type==0){		//K42 K83
				bufferU[j++]=ICSP8_LOAD;
				bufferU[j++]=LOAD_NVM;
				bufferU[j++]=memCONFIG[i*2+1];	//High byte
				bufferU[j++]=memCONFIG[i*2];	//Low byte
				bufferU[j++]=ICSP8_SHORT;
				bufferU[j++]=BEGIN_INT_PROG;			//internally timed
			}
			bufferU[j++]=WAIT_T3;				//Tprogram 11ms or 5.6ms
			bufferU[j++]=ICSP8_READ;
			bufferU[j++]=READ_NVM_INC;
			if(j>DIMBUF-8||i==CONFIGlen-1){
				PrintStatus(strings[S_CodeWriting],99,i+CONFIGaddr);	//"Writing: %d%%, addr. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(80);
				for(z=0;z<DIMBUF-4;z++){
					if(bufferI[z]==ICSP8_LOAD){
						if(type==1){	//Q43 single byte
							w=bufferI[z+4];
							x=memCONFIG[k];
						}
						else if(type==0){	//K42 K83 2 bytes
							w=(bufferI[z+4]<<8)+bufferI[z+5];
							x=(memCONFIG[k+1]<<8)+memCONFIG[k];
						}
						if(~x&w){	//error if written 0 and read 1 (~Written&Read)
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError2],k+CONFIGaddr,x,w);	//"Error writing address %3X: written %04X, read %04X\r\n"
							errC++;
							if(max_err&&err+errC>max_err){
								PrintMessage1(strings[S_MaxErr],err+errC);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"Write interrupted"
								i=dim2;
								z=DIMBUF;
							}
						}
						if(type==1){	//Q43 single byte
							k++;
							z+=4;
						}
						else if(type==0){	//K42 K83 2 bytes
							k+=2;
							z+=5;
						}
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,errC);	//"i=%d, k=%d, errors=%d,\n"
				}
			}
		}
		errC+=10-k;
		err+=errC;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],errC);	//"completed, %d errors\r\n"
	}
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=1;
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(1);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}


