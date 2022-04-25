/**
 * \file opgui.c
 * main control program for the open programmer
 *
 * Copyright (C) 2009-2021 Alberto Maccioni
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

#include "opgui.h"
#include "common.h"

#include "I2CSPI.h"
#include "coff.h"
#include "icd.h"
#include "deviceRW.h"
#include "fileIO.h"
#include "icons.h"
#include "progAVR.h"

#define MAXLINES 600
#define  CONFIG_FILE "opgui.ini"
#define  CONFIG_DIR ".opgui"
#define MinDly 0

void Connect(GtkWidget *widget,GtkWidget *window);
void I2cspiR();
void I2cspiS();
void ProgID();
void ShowContext();
int FindDevice(int vid,int pid);
void TestHw();
int CheckS1();
char** strings;	//!localized strings
int cmdline=0;
int saveLog=0,programID=0,load_osccal=0,load_BKosccal=0;
int use_osccal=1,use_BKosccal=0;
int load_calibword=0,max_err=200;
int AVRlock=0x100,AVRfuse=0x100,AVRfuse_h=0x100,AVRfuse_x=0x100;
int ICDenable=0,ICDaddr=0x1FF0;
int FWVersion=0,HwID=0;
FILE* logfile=0;
char appName[6]=_APPNAME;
char LogFileName[512]="";
char loadfile[512]="",savefile[512]="";
char loadfileEE[512]="",savefileEE[512]="";
char CoffFileName[512]="";
int vid=0x1209,pid=0x5432;
int new_vid=0x1209,new_pid=0x5432;
int old_vid=0x04D8,old_pid=0x0100;
WORD *memCODE_W=0;
int size=0,sizeW=0,sizeEE=0,sizeCONFIG=0,sizeUSERID=0;
unsigned char *memCODE=0,*memEE=0,memID[64],memCONFIG[48],memUSERID[8];
double hvreg=0;
int DeviceDetected=0;
int UseCoff=0;
struct src_i source_info[LMAX];
struct srcfile *s_files;
struct symbol *sym;
int nsym=0;
char* Slabel[LMAX],*Sulabel[ULMAX];
//struct var{	char* name;	int display;} variables[0x200];
struct symbol *watch;
int nwatch=0;
unsigned short coff_data[DATA_MAX];
int ver=0,reset=1,freeze=0,icdConnected=0,running=0;
int break_addr,print_addr;
#define Tck 30
double Tcom=0.001*Tck*18+0.03; //communication time for a 16 bit tranfer (ms)
guint icdTimer=0,IOTimer=0;
int currentSource=-1;
int sourceHilight=0;
char lastCmd[64]="";
int skipV33check=0;
int waitS1=0,waitingS1=0;
int progress=0;
int RWstop=0;
int forceConfig=0;
#ifdef DEBUG
	int addrDebug=0;
	unsigned short dataDebug=0;
	unsigned short statusDebug=0x3FFF;
#endif
//List of gtk controls
GtkTextBuffer * dataBuf;
GtkWidget * data;
GtkWidget * data_scroll;
GtkWidget * window;
GtkWidget * toolbar;
GtkWidget * button;
GtkWidget * b_open;
GtkWidget * b_save;
GtkWidget * b_read;
GtkWidget * b_write;
GtkWidget * notebook;
GtkWidget * label;
GtkWidget * status_bar;
GtkWidget * img;
GtkWidget * devCombo;
GtkWidget * devTypeCombo;
GtkWidget * devFramePIC;
GtkWidget * ICD_check;
GtkWidget * ICD_addr_entry;
GtkWidget * EEPROM_RW;
GtkWidget * ReadReserved;
GtkWidget * Write_ID_BKCal;
GtkWidget * WriteCalib12;
GtkWidget * UseOSCCAL;
GtkWidget * UseBKOSCCAL;
GtkWidget * UseFileCal;
GtkWidget * devFrameAVR;
GtkWidget * AVR_FuseLow;
GtkWidget * AVR_FuseLowWrite;
GtkWidget * AVR_FuseHigh;
GtkWidget * AVR_FuseHighWrite;
GtkWidget * AVR_FuseExt;
GtkWidget * AVR_FuseExtWrite;
GtkWidget * AVR_Lock;
GtkWidget * AVR_LockWrite;
GtkWidget * b_WfuseLF;
GtkWidget * b_connect;
GtkWidget * b_testhw;
GtkWidget * b_log;
GtkWidget * VID_entry;
GtkWidget * PID_entry;
GtkWidget * Errors_entry;
GtkWidget * I2C8bit;
GtkWidget * I2C16bit;
GtkWidget * SPI00;
GtkWidget * SPI01;
GtkWidget * SPI10;
GtkWidget * SPI11;
GtkWidget * I2CDataSend;
GtkWidget * I2CDataReceive;
GtkWidget * I2CSendBtn;
GtkWidget * I2CReceiveBtn;
GtkWidget * I2CNbyte;
GtkWidget * I2CSpeed;
GtkWidget * statusTxt;
GtkTextBuffer * statusBuf;
GtkWidget * sourceTxt;
GtkTextBuffer * sourceBuf;
GtkWidget * icdVbox1;
GtkWidget * icdMenuPC;
GtkWidget * icdMenuSTAT;
GtkWidget * icdMenuBank0;
GtkWidget * icdMenuBank1;
GtkWidget * icdMenuBank2;
GtkWidget * icdMenuBank3;
GtkWidget * icdMenuEE;
GtkWidget * icdCommand;
GtkWidget * DCDC_ON;
GtkWidget * DCDC_voltage;
GtkWidget * VPP_ON;
GtkWidget * VDD_ON;
GtkWidget * b_io_active;
GtkWidget * b_V33check;
GtkWidget * Hex_entry;
GtkWidget * Address_entry;
GtkWidget * Data_entry;
GtkWidget * Hex_data;
GtkWidget * Hex_data2;
GtkWidget * CW1_entry;
GtkWidget * CW2_entry;
GtkWidget * CW3_entry;
GtkWidget * CW4_entry;
GtkWidget * CW5_entry;
GtkWidget * CW6_entry;
GtkWidget * CW7_entry;
GtkWidget * ConfigForce;
GtkWidget * b_WaitS1;
GtkWidget * devFrameConfigW;
GtkWidget * devFrameICD;
GtkWidget * devFrameOsc;
GtkWidget * devPIC_CW1;
GtkWidget * devPIC_CW2;
GtkWidget * devPIC_CW3;
GtkWidget * devPIC_CW4;
GtkWidget * devPIC_CW5;
GtkWidget * devPIC_CW6;
GtkWidget * devPIC_CW7;
GtkWidget * devinfo;
GtkToolItem* btnStop;
GtkStyleContext *styleCtx;

///array of radio buttons for IO manual control
struct io_btn {	char * name;
				int x;
				int y;
				GtkWidget * r_0;	//radio button 0
				GtkWidget * r_1;	//radio button 1
				GtkWidget * r_I;	//radio button I
				GtkWidget * e_I;	//entry
				} ioButtons[13];
int statusID;
int ee = 0;
int readRes=0;
char dev[64]="";
int devType=-1;
char str[4096]="";
char* cur_path=0;
char* cur_pathEE=0;
#if !defined _WIN32 && !defined __CYGWIN__	//Linux
	int fd = -1;
#ifdef hiddevIO	
	struct hiddev_report_info rep_info_i,rep_info_u;
	struct hiddev_usage_ref_multi ref_multi_i,ref_multi_u;
#endif
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
///
///Exit
gint delete_event( GtkWidget *widget,GdkEvent *event,gpointer data )
{
gtk_main_quit ();
return FALSE;
}
///
///Append a message on the data tab; shorten the length of the entry field to MAXLINES
void PrintMessage(const char *msg){
	if(cmdline) return;	//do not print anything if using command line mode
	GtkTextIter iter,iter2;
	gtk_text_buffer_get_end_iter(dataBuf,&iter);
	gtk_text_buffer_insert(dataBuf,&iter,msg,-1);
	gtk_text_buffer_get_start_iter(dataBuf,&iter2);
	gtk_text_buffer_get_end_iter(dataBuf,&iter);
	int l=gtk_text_buffer_get_line_count(dataBuf);
	if(l>MAXLINES+10){ 	//MAXLINES
		gtk_text_iter_set_line(&iter,l-MAXLINES);
		gtk_text_buffer_delete(dataBuf,&iter2,&iter);
	}
	while (gtk_events_pending ()) gtk_main_iteration();
	gtk_text_buffer_get_end_iter(dataBuf,&iter);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(data),&iter,0.0,FALSE,0,0);
}
///
///Print a message on the I2C data field
void PrintMessageI2C(const char *msg){
	GtkTextIter iter;
	GtkTextBuffer * dataBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(I2CDataReceive));
	gtk_text_buffer_set_text(dataBuf,msg,-1);
	gtk_text_buffer_get_end_iter(dataBuf,&iter);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(I2CDataReceive),&iter,0.0,FALSE,0,0);
	while (gtk_events_pending ()) gtk_main_iteration();
}
///
///Print a message on the ICD data field
void PrintMessageICD(const char *msg){
	GtkTextIter iter;
	//GtkWidget * dataBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(statusTxt));
	gtk_text_buffer_set_text(statusBuf,msg,-1);
	gtk_text_buffer_get_start_iter(statusBuf,&iter);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(statusTxt),&iter,0.0,FALSE,0,0);
	while (gtk_events_pending ()) gtk_main_iteration();
}
///
///Append a message on the ICD data field
void AppendMessageICD(const char *msg){
	GtkTextIter iter;
	//GtkWidget * dataBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(statusTxt));
	gtk_text_buffer_get_end_iter(statusBuf,&iter);
	gtk_text_buffer_insert(statusBuf,&iter,msg,-1);
	gtk_text_buffer_get_start_iter(statusBuf,&iter);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(statusTxt),&iter,0.0,FALSE,0,0);
	while (gtk_events_pending ()) gtk_main_iteration();
}
///
///Update option variables according to actual control values
void getOptions()
{
	vid=htoi(gtk_entry_get_text(GTK_ENTRY(VID_entry)),4);
	pid=htoi(gtk_entry_get_text(GTK_ENTRY(PID_entry)),4);
	saveLog = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b_log));
	ee = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(EEPROM_RW))?0xFFFF:0;
	programID = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Write_ID_BKCal));
	max_err=atoi(gtk_entry_get_text(GTK_ENTRY(Errors_entry)));
	load_calibword= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(WriteCalib12));
	load_osccal= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(UseOSCCAL));
	load_BKosccal= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(UseBKOSCCAL));
	ICDenable= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ICD_check));
	readRes= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ReadReserved));
	skipV33check=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b_V33check));
	waitS1=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b_WaitS1));
	int i=sscanf(gtk_entry_get_text(GTK_ENTRY(ICD_addr_entry)),"%x",&ICDaddr);
	if(i!=1||ICDaddr<0||ICDaddr>0xFFFF) ICDaddr=0x1FF0;
	char *str=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(devCombo));
	if(str) strncpy(dev,str,sizeof(dev)-1);
	g_free(str);
	AVRfuse=AVRfuse_h=AVRfuse_x=AVRlock=0x100;
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(AVR_FuseLowWrite))){
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(AVR_FuseLow)),"%x",&AVRfuse);
		if(i!=1||AVRfuse<0||AVRfuse>0xFF) AVRfuse=0x100;
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(AVR_FuseHighWrite))){
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(AVR_FuseHigh)),"%x",&AVRfuse_h);
		if(i!=1||AVRfuse_h<0||AVRfuse_h>0xFF) AVRfuse_h=0x100;
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(AVR_FuseExtWrite))){
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(AVR_FuseExt)),"%x",&AVRfuse_x);
		if(i!=1||AVRfuse_x<0||AVRfuse_x>0xFF) AVRfuse_x=0x100;
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(AVR_LockWrite))){
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(AVR_Lock)),"%x",&AVRlock);
		if(i!=1||AVRlock<0||AVRlock>0xFF) AVRlock=0x100;
	}
	str=malloc(128);
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ConfigForce))){
		int cw1,cw2,cw3,cw4,cw5,cw6,cw7;
		cw1=cw2=cw3=cw4=cw5=cw6=cw7=0x10000;
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW1_entry)),"%x",&cw1);
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW2_entry)),"%x",&cw2);
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW3_entry)),"%x",&cw3);
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW4_entry)),"%x",&cw4);
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW5_entry)),"%x",&cw5);
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW6_entry)),"%x",&cw6);
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW7_entry)),"%x",&cw7);
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
	}
	free(str);
}
///
///Choose a file to open and call Load()
void Fopen(GtkWidget *widget,GtkWidget *window)
{
	char *str=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(devCombo));
	if(progress) return;
	progress=1;
	if(str) strncpy(dev,str,sizeof(dev)-1);
	g_free(str);
	GtkFileChooser *dialog;
	dialog = (GtkFileChooser*) gtk_file_chooser_dialog_new ("Open File",
				      GTK_WINDOW(window),
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      "_Cancel", GTK_RESPONSE_CANCEL,
				      "_Open", GTK_RESPONSE_ACCEPT,
				      NULL);
	if(cur_path) gtk_file_chooser_set_current_folder(dialog,cur_path);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
	    char *filename;
	    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		if(cur_path) free(cur_path);
		cur_path = gtk_file_chooser_get_current_folder(dialog);
	    Load(dev,filename);
	    g_free (filename);
		if(!strncmp(dev,"AT",2)){	//load EEPROM from separate file for ATMEL chips
			GtkFileChooser *dialog2;
			dialog2 = (GtkFileChooser*) gtk_file_chooser_dialog_new (strings[S_openEEfile],
						      GTK_WINDOW(window),
						      GTK_FILE_CHOOSER_ACTION_OPEN,
						      "_Cancel", GTK_RESPONSE_CANCEL,
						      "_Open", GTK_RESPONSE_ACCEPT,
						      NULL);
			if(!cur_pathEE) cur_pathEE = gtk_file_chooser_get_current_folder(dialog);
			if(cur_pathEE) gtk_file_chooser_set_current_folder(dialog2,cur_pathEE);
			if (gtk_dialog_run (GTK_DIALOG (dialog2)) == GTK_RESPONSE_ACCEPT){
			    char *filename2;
			    filename2 = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog2));
				if(cur_pathEE) free(cur_pathEE);
				cur_pathEE = gtk_file_chooser_get_current_folder(dialog2);
			//CFileDialog dlgA(TRUE,"hex",NULL,OFN_HIDEREADONLY,strings[S_fileEEP]);	//"File Hex8 (*.hex;.eep ..."
				LoadEE(dev,filename2);
				g_free (filename2);
			}
			gtk_widget_destroy(GTK_WIDGET(dialog2));
		}
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
	progress=0;
}
///
///Choose a file to save and call Save()
void Fsave(GtkWidget *widget,GtkWidget *window)
{
	if(progress) return;
	progress=1;
	GtkFileChooser *dialog;
	dialog = (GtkFileChooser*) gtk_file_chooser_dialog_new ("Save File",
				      GTK_WINDOW(window),
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      "_Cancel", GTK_RESPONSE_CANCEL,
				      "_Save", GTK_RESPONSE_ACCEPT,
				      NULL);
	if(cur_path) gtk_file_chooser_set_current_folder(dialog,cur_path);
	gtk_file_chooser_set_do_overwrite_confirmation(dialog,TRUE);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	  {
	    char *filename;
	    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		if(cur_path) free(cur_path);
		cur_path = gtk_file_chooser_get_current_folder(dialog);
   		Save(dev,filename);
		PrintMessage1(strings[S_FileSaved],filename);
	    g_free (filename);
		if(!strncmp(dev,"AT",2)&&sizeEE){	//save EEPROM on separate file for ATMEL chips
			GtkFileChooser *dialog2;
			dialog2 = (GtkFileChooser*) gtk_file_chooser_dialog_new (strings[S_saveEEfile],
						      GTK_WINDOW(window),
						      GTK_FILE_CHOOSER_ACTION_SAVE,
						      "_Cancel", GTK_RESPONSE_CANCEL,
							  "_Save", GTK_RESPONSE_ACCEPT,
						      NULL);
			if(!cur_pathEE) cur_pathEE = gtk_file_chooser_get_current_folder(dialog);
			if(cur_pathEE) gtk_file_chooser_set_current_folder(dialog2,cur_pathEE);
			gtk_file_chooser_set_do_overwrite_confirmation(dialog2,TRUE);
			if (gtk_dialog_run (GTK_DIALOG (dialog2)) == GTK_RESPONSE_ACCEPT){
			    char *filename2;
			    filename2 = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog2));
				if(cur_pathEE) free(cur_pathEE);
				cur_pathEE = gtk_file_chooser_get_current_folder(dialog2);
				SaveEE(dev,filename2);
				PrintMessage1(strings[S_FileSaved],filename2);
				g_free (filename2);
			}
			gtk_widget_destroy(GTK_WIDGET(dialog2));
		}
	  }
	gtk_widget_destroy (GTK_WIDGET(dialog));
	progress=0;
}
///
///Call device write function
void DevWrite(GtkWidget *widget,GtkWidget *window)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,"");
	RWstop=0;
	getOptions();
	if(!waitingS1&&waitS1){
		waitingS1=1;
		int i,S1=0;
		PrintMessage(strings[I_PRESSS1]); //"press S1 to start"
		for(i=0;!S1&&waitS1&&waitingS1;i++){
			S1=CheckS1();
			msDelay(50);
			PrintMessage(".");
			if(i%64==63) PrintMessage(strings[S_NL]); //"\n"
			while (gtk_events_pending ()) gtk_main_iteration(); //handle UI events, including write button
			msDelay(50);
		}
		PrintMessage(strings[S_NL]); //"\n"
		if(!progress&&S1){
			gtk_widget_set_sensitive(GTK_WIDGET(btnStop),TRUE);
			progress=1;
			Write(dev,ee);	//choose the right function
			progress=0;
			gtk_widget_set_sensitive(GTK_WIDGET(btnStop),FALSE);
		}
		waitingS1=0;
	}
	else if(waitingS1) waitingS1=0;
	else if(!progress){
		gtk_widget_set_sensitive(GTK_WIDGET(btnStop),TRUE);
		progress=1;
		Write(dev,ee);	//choose the right function
		progress=0;
		gtk_widget_set_sensitive(GTK_WIDGET(btnStop),FALSE);
	}
}
///
///Call device read function
void DevRead(GtkWidget *widget,GtkWidget *window)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,"");
	getOptions();
	RWstop=0;
	if(!waitingS1&&waitS1){
		waitingS1=1;
		int i,S1=0;
		PrintMessage(strings[I_PRESSS1]); //"press S1 to start"
		for(i=0;!S1&&waitS1&&waitingS1;i++){
			S1=CheckS1();
			msDelay(50);
			PrintMessage(".");
			if(i%64==63) PrintMessage(strings[S_NL]); //"\n"
			while (gtk_events_pending ()) gtk_main_iteration(); //handle UI events, including write button
			msDelay(50);
		}
		PrintMessage(strings[S_NL]); //"\n"
		if(!progress&&S1){
			gtk_widget_set_sensitive(GTK_WIDGET(btnStop),TRUE);
			progress=1;
			Read(dev,ee,readRes);	//choose the right function
			progress=0;
			gtk_widget_set_sensitive(GTK_WIDGET(btnStop),FALSE);
		}
		waitingS1=0;
	}
	else if(waitingS1) waitingS1=0;
	else if(!progress){
		gtk_widget_set_sensitive(GTK_WIDGET(btnStop),TRUE);
		progress=1;
		Read(dev,ee,readRes);	//choose the right function
		progress=0;
		gtk_widget_set_sensitive(GTK_WIDGET(btnStop),FALSE);
	}
}
///
/// Write fuse low byte at low frequency
void WriteATfuseLowLF(GtkWidget *widget,GtkWidget *window){
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	if(progress) return;
	getOptions();
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(AVR_FuseLowWrite))){
		progress=1;
		if(AVRfuse<0x100) WriteATfuseSlow(AVRfuse);
		progress=0;
	}
}
///
///Callback function to set available options for each device type
void DeviceChanged(GtkWidget *widget,GtkWidget *window)
{
	struct DevInfo info;
	char str2[256],str3[64],strF[32];
	double x;
	char *str=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(devCombo));
	if(str) strncpy(dev,str,sizeof(dev)-1);
	else return;
	g_free(str);
	info=GetDevInfo(dev);
	devType=info.type;
/*	str2[0]=0;
	x=info.size/1024.0;
	if(x-(int)x) sprintf(strF,"%.1f",x);
	else sprintf(strF,"%d",(int)x);
	switch(info.type){
	case -1:
		sprintf(str2,"?? ");
		break;
	case PIC12:
		sprintf(str2,"PIC12, ");
		if(info.size<1024) sprintf(str3,"%dW FLASH",info.size);
		else sprintf(str3,"%sKW FLASH",strF);
		strcat(str2,str3);
		break;
	case PIC16:
		sprintf(str2,"PIC16, ");
		if(info.size<1024) sprintf(str3,"%dW FLASH",info.size);
		else sprintf(str3,"%sKW FLASH",strF);
		strcat(str2,str3);
		if(info.sizeEE){
			int ee=info.sizeEE;
			if(ee<0) ee=-ee;
			sprintf(str3," + %dB EEPROM",ee);
			strcat(str2,str3);
		}
		break;
	case PIC18:
		sprintf(str2,"PIC18, ");
		if(info.size<1024) sprintf(str3,"%dB FLASH ",info.size);
		else sprintf(str3,"%sKB FLASH ",strF);
		strcat(str2,str3);
		if(info.sizeEE){
			sprintf(str3,"+ %dB EEPROM ",info.sizeEE);
			strcat(str2,str3);
		}
		break;
	case PIC24:
		sprintf(str2,"PIC24, ");
		if(info.size<1024) sprintf(str3,"%dW FLASH",info.size);
		else sprintf(str3,"%sKW FLASH",strF);
		strcat(str2,str3);
		if(info.sizeEE){
			sprintf(str3," + %dB EEPROM",info.sizeEE);
			strcat(str2,str3);
		}
		break;
	case PIC32:
		sprintf(str2,"PIC32, ");
		sprintf(str3,"%sKB FLASH",strF);
		strcat(str2,str3);
		break;
	case AVR:
		sprintf(str2,"AVR, ");
		if(info.size<1024) sprintf(str3,"%dB FLASH",info.size);
		else sprintf(str3,"%sKB FLASH",strF);
		strcat(str2,str3);
		if(info.sizeEE){
			sprintf(str3," + %dB EEPROM",info.sizeEE);
			strcat(str2,str3);
		}
		break;
	case I2CEE:
		if(info.size<1024) sprintf(str2,"%s, %dB",strings[I_I2CMEM],info.size); //I2C Memory
		else sprintf(str2,"%s, %sKB",strings[I_I2CMEM],strF); //I2C Memory
		break;
	case SPIEE:
		if(info.size<1024) sprintf(str2,"%s, %dB",strings[I_SPIMEM],info.size); //SPI Memory
		else sprintf(str2,"%s, %sKB",strings[I_SPIMEM],strF); //SPI Memory
		break;
	case UWEE:
		if(info.size<1024) sprintf(str2,"%s, %dB",strings[I_UWMEM],info.size); //Microwire Memory
		else sprintf(str2,"%s,%sKB",strings[I_UWMEM],strF);
		break;
	case OWEE:
		if(info.size<0) sprintf(str2,strings[I_OWDEV]); //OneWire device
		else if(info.size<1024) sprintf(str2,"%s, %dB",strings[I_OWMEM],info.size); //OneWire Memory
		else sprintf(str2,"%s, %sKB",strings[I_OWMEM],strF);
		break;
	case UNIOEE:
		if(info.size<1024) sprintf(str2,"%s, %dB",strings[I_UNIOMEM],info.size); //UNI/O Memory
		else sprintf(str2,"%s, %sKB",strings[I_UNIOMEM],strF);
		break;
	}
	if(info.HV>0){
		sprintf(str3,", %.1fV",info.HV);
		strcat(str2,str3);
	}
	if(info.V33){
		strcat(str2,", ");
		strcat(str2,strings[I_3V3REQUIRED]); // 3.3V adapter
	}*/
//	gtk_label_set_text(GTK_LABEL(devinfo),str2);
	gtk_label_set_text(GTK_LABEL(devinfo),info.features);
	//printf("%s=%d\n",dev,GetDevType(dev));fflush(stdout);
	if(devType==PIC12||devType==PIC16||devType==PIC18||devType==PIC24){
		gtk_widget_show_all(GTK_WIDGET(devFramePIC));
		gtk_widget_hide(GTK_WIDGET(devFrameAVR));
		gtk_widget_show_all(GTK_WIDGET(EEPROM_RW));
	}
	else if(devType==AVR){	//ATMEL
		gtk_widget_hide(GTK_WIDGET(devFramePIC));
		gtk_widget_show_all(GTK_WIDGET(devFrameAVR));
		gtk_widget_show_all(GTK_WIDGET(EEPROM_RW));
	}
	else{
		gtk_widget_hide(GTK_WIDGET(devFramePIC));
		gtk_widget_hide(GTK_WIDGET(devFrameAVR));
		gtk_widget_hide(GTK_WIDGET(EEPROM_RW));
	}
	if(devType==PIC16)		//ICD
		gtk_widget_show_all(GTK_WIDGET(devFrameICD));
	else gtk_widget_hide(GTK_WIDGET(devFrameICD));
	if(devType==PIC12||devType==PIC16)	//Osc options
		gtk_widget_show_all(GTK_WIDGET(devFrameOsc));
	else gtk_widget_hide(GTK_WIDGET(devFrameOsc));
	if(devType==PIC12||devType==PIC16||devType==PIC18)	//program ID
		gtk_widget_show_all(GTK_WIDGET(Write_ID_BKCal));
	else gtk_widget_hide(GTK_WIDGET(Write_ID_BKCal));
	if(devType==PIC16)	//Program Calib
		gtk_widget_show_all(GTK_WIDGET(WriteCalib12));
	else gtk_widget_hide(GTK_WIDGET(WriteCalib12));
	if(devType==PIC12||devType==PIC16||devType==PIC18){	//Force config
		//gtk_widget_set_sensitive(GTK_WIDGET(devFrameConfigW),TRUE);
		gtk_widget_show_all(GTK_WIDGET(devFrameConfigW));
		//gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW1),TRUE);
		//gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW2),FALSE);
		gtk_widget_hide(GTK_WIDGET(devPIC_CW2));
		//gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW3),FALSE);
		gtk_widget_hide(GTK_WIDGET(devPIC_CW3));
		//gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW4),FALSE);
		gtk_widget_hide(GTK_WIDGET(devPIC_CW4));
		//gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW5),FALSE);
		gtk_widget_hide(GTK_WIDGET(devPIC_CW5));
		//gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW6),FALSE);
		gtk_widget_hide(GTK_WIDGET(devPIC_CW6));
		//gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW7),FALSE);
		gtk_widget_hide(GTK_WIDGET(devPIC_CW7));
		//gtk_widget_set_sensitive(GTK_WIDGET(devFrameOsc),FALSE);
		if(devType==PIC16){
			//gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW2),TRUE);
			gtk_widget_show_all(GTK_WIDGET(devPIC_CW2));
		}
		else if(devType==PIC18){
			/*gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW2),TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW3),TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW4),TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW5),TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW6),TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(devPIC_CW7),TRUE);*/
			gtk_widget_show_all(GTK_WIDGET(devPIC_CW2));
			gtk_widget_show_all(GTK_WIDGET(devPIC_CW3));
			gtk_widget_show_all(GTK_WIDGET(devPIC_CW4));
			gtk_widget_show_all(GTK_WIDGET(devPIC_CW5));
			gtk_widget_show_all(GTK_WIDGET(devPIC_CW6));
			gtk_widget_show_all(GTK_WIDGET(devPIC_CW7));
		}
	}
	else{
		gtk_widget_hide(GTK_WIDGET(devFrameConfigW));
		//gtk_widget_set_sensitive(GTK_WIDGET(devFrameConfigW),FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ConfigForce),FALSE);
	}
	gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,dev);
}
///
///Filter device list according to type selected
void FilterDevType(GtkWidget *widget,GtkWidget *window)
{
	char *str=0;
	g_signal_handlers_disconnect_by_func(G_OBJECT(devCombo),G_CALLBACK(DeviceChanged),NULL); //disconnect callback while adding items
	gtk_combo_box_set_wrap_width(GTK_COMBO_BOX(devCombo),0);
	GtkTreeModel *store = gtk_combo_box_get_model( GTK_COMBO_BOX(devCombo) );
	gtk_list_store_clear( GTK_LIST_STORE( store ) );
	int i=gtk_combo_box_get_active(GTK_COMBO_BOX(devTypeCombo));
	switch(i){
		case 1:		//10F 12F
			for(i=0;i<Ndevices;i++) if(!strncmp(devices[i],"10F",3)||!strncmp(devices[i],"12F",3))
				gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devCombo),devices[i]);
		break;
		case 2:		//16F
			for(i=0;i<Ndevices;i++) if(!strncmp(devices[i],"16F",3)) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devCombo),devices[i]);
		break;
		case 3: 	//18F
			for(i=0;i<Ndevices;i++) if(!strncmp(devices[i],"18F",3)) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devCombo),devices[i]);
		break;
		case 4:		//24F
			for(i=0;i<Ndevices;i++) if(!strncmp(devices[i],"24F",3)||!strncmp(devices[i],"24H",3)||!strncmp(devices[i],"24E",3))
				gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devCombo),devices[i]);
		break;
		case 5:		//30F 33F
			for(i=0;i<Ndevices;i++) if(!strncmp(devices[i],"30F",3)||!strncmp(devices[i],"33F",3)||!strncmp(devices[i],"33E",3))
				gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devCombo),devices[i]);
		break;
		case 6:		//ATMEL
			for(i=0;i<Ndevices;i++) if(!strncmp(devices[i],"AT",2)) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devCombo),devices[i]);
		break;
		case 7:		//24 25 93 DS 11
			for(i=0;i<Ndevices;i++) if( (strncmp(devices[i],"24F",3)&&strncmp(devices[i],"24H",3)&&strncmp(devices[i],"24E",3))&&\
				(!strncmp(devices[i],"24",2)||!strncmp(devices[i],"25",2)||!strncmp(devices[i],"93",2)|| \
				 !strncmp(devices[i],"11",2)||!strncmp(devices[i],"DS",2))) \
				gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devCombo),devices[i]);
		break;
		default:
			for(i=0;i<Ndevices;i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devCombo),devices[i]);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(devCombo),0);
	for(i=0;(str=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(devCombo)))&&strcmp(str,dev)&&i<10000;i++){
		gtk_combo_box_set_active(GTK_COMBO_BOX(devCombo),i);	//need to set item to parse all items
		g_free(str);
	}
//	printf("i=%d str=%p\n",i,str);fflush(stdout);
	if(i>=10000||!str)gtk_combo_box_set_active(GTK_COMBO_BOX(devCombo),0);
	gtk_combo_box_set_wrap_width(GTK_COMBO_BOX(devCombo),6);
	g_signal_connect(G_OBJECT(devCombo),"changed",G_CALLBACK(DeviceChanged),NULL);	//enable callback
	DeviceChanged(NULL,NULL);
}
///
///Scroll source file
void scrollToLine(int line)
{
	GtkTextIter iter,iter2;
	gtk_text_buffer_get_end_iter(sourceBuf,&iter);
	if(line>0){
		gtk_text_iter_set_line(&iter,line-1);
		iter2=iter;
		gtk_text_iter_forward_char(&iter2);
		gtk_text_iter_forward_to_line_end(&iter2);
	}
	else{
		gtk_text_buffer_get_selection_bounds(sourceBuf,&iter,&iter2);
		iter2=iter;
	}
	gtk_text_buffer_select_range(sourceBuf,&iter,&iter2);
	while (gtk_events_pending ()) gtk_main_iteration();
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(sourceTxt),&iter,0.0,TRUE,0,0.5);
}
///
///Hilight line in source code
void SourceHilightLine(int line)
{
	GtkTextIter iter,iter2;
	GtkTextTag* tag;
	if(line>0){
		tag=gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(sourceBuf),"break_text");
		if(!tag) tag=gtk_text_buffer_create_tag(sourceBuf,"break_text","background","red", NULL);
		gtk_text_buffer_get_end_iter(sourceBuf,&iter);
		gtk_text_iter_set_line(&iter,line-1);
		iter2=iter;
		gtk_text_iter_forward_char(&iter2);
		gtk_text_iter_forward_to_line_end(&iter2);
		gtk_text_buffer_apply_tag (sourceBuf,tag,&iter,&iter2);
	}
	while (gtk_events_pending ()) gtk_main_iteration();
}
///
///Remove hilight line in source code
void SourceRemoveHilightLine(int line)
{
	GtkTextIter iter,iter2;
	if(line>0){
		gtk_text_buffer_get_end_iter(sourceBuf,&iter);
		gtk_text_iter_set_line(&iter,line-1);
		iter2=iter;
		gtk_text_iter_forward_char(&iter2);
		gtk_text_iter_forward_to_line_end(&iter2);
		gtk_text_buffer_remove_tag_by_name(sourceBuf,"break_text",&iter,&iter2);
	}
	while (gtk_events_pending ()) gtk_main_iteration();
}
///
///load source file into source pane
int loadSource(FILE *f){
	if(!f) return 0;
	fseek(f,0,SEEK_END);
	int size=ftell(f);
	fseek(f,0,SEEK_SET);
	char* tmp=(char*)malloc(size+1);
	size=fread(tmp,1,size,f);
	tmp[size]=0;
	char* g=g_locale_to_utf8(tmp,-1,NULL,NULL,NULL);
	gtk_text_buffer_set_text(sourceBuf,g,-1);
	free(tmp);
	g_free(g);
	return 1;
}
///
///load and analyze coff file
void loadCoff(GtkWidget *widget,GtkWidget *window)
{
	GtkFileChooser *dialog;
	dialog = (GtkFileChooser*) gtk_file_chooser_dialog_new ("Open Coff File",
				      GTK_WINDOW(window),
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      "_Cancel", GTK_RESPONSE_CANCEL,
				      "_Open", GTK_RESPONSE_ACCEPT,
				      NULL);
	if(cur_path) gtk_file_chooser_set_current_folder(dialog,cur_path);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
	    char *filename;
	    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		if(cur_path) free(cur_path);
		cur_path = gtk_file_chooser_get_current_folder(dialog);
		UseCoff=analyzeCOFF(filename,Slabel,Sulabel,source_info,&s_files,coff_data,&sym,&nsym);
		g_free (filename);
		//load source for address 0
		if(source_info[0].src_file!=-1){
			if(currentSource==source_info[0].src_file){
				scrollToLine(source_info[0].src_line);
			}
			else if(loadSource(s_files[source_info[0].src_file].ptr)){
				scrollToLine(source_info[0].src_line);
				currentSource=source_info[0].src_file;
			}
		}
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}
///
/// List of variables used when decoding an assembly word
void initVar(){
	int i;
	for(i=0;i<0x200;i++){//clear variable list
		variables[i].name=0;
		variables[i].display=0;
	}
	variables[0].name="INDF";
	variables[1].name="TMR0";
	variables[2].name="PCL";
	variables[3].name="STATUS";
	variables[4].name="FSR";
	variables[5].name="PORTA";
	variables[6].name="PORTB";
	variables[7].name="PORTC";
	variables[8].name="PORTD";
	variables[9].name="PORTE";
	variables[10].name="PCLATH";
	variables[11].name="INTCON";
	variables[12].name="PIR1";
	variables[13].name="PIR2";
	variables[14].name="TMR1L";
	variables[15].name="TMR1H";
	variables[16].name="T1CON";
	variables[17].name="TMR2";
	variables[18].name="T2CON";
	variables[19].name="SSPBUF";
	variables[20].name="SSPCON";
	variables[21].name="CCPR1L";
	variables[22].name="CCPR1H";
	variables[23].name="CCP1CON";
	variables[24].name="RCSTA";
	variables[25].name="TXREG";
	variables[26].name="RCREG";
	variables[27].name="CCPR2L";
	variables[28].name="CCPR2H";
	variables[29].name="CCP2CON";
	variables[30].name="ADRESH";
	variables[31].name="ADCON0";
	variables[0x6B].name="DEBUG_VAR1";
	variables[0x6C].name="DEBUG_VAR2";
	variables[0x6D].name="DEBUG_VAR3";
	variables[0x6E].name="DEBUG_VAR4";
	variables[0x6F].name="DEBUG_VAR5";
	variables[0x70].name="DEBUG_VAR6";
	variables[0x71].name="DEBUG_VAR7";
	variables[0x72].name="DEBUG_VAR8";
	variables[0x80].name="INDF";
	variables[0x81].name="OPTION_REG";
	variables[0x82].name="PCL";
	variables[0x83].name="STATUS";
	variables[0x84].name="FSR";
	variables[0x85].name="TRISA";
	variables[0x86].name="TRISB";
	variables[0x87].name="TRISC";
	variables[0x88].name="TRISD";
	variables[0x89].name="TRISE";
	variables[0x8A].name="PCLATH";
	variables[0x8B].name="INTCON";
	variables[0x8C].name="PIE1";
	variables[0x8D].name="PIE2";
	variables[0x8E].name="PCON";
	variables[0x91].name="SSPCON2";
	variables[0x92].name="PR2";
	variables[0x93].name="SSPADD";
	variables[0x94].name="SSPSTAT";
	variables[0x98].name="TXSTA";
	variables[0x99].name="SPBRG";
	variables[0x9E].name="ADRESL";
	variables[0x9F].name="ADCON1";
	variables[0x100].name="INDF";
	variables[0x101].name="TMR0";
	variables[0x102].name="PCL";
	variables[0x103].name="STATUS";
	variables[0x104].name="FSR";
	variables[0x106].name="PORTB";
	variables[0x10A].name="PCLATH";
	variables[0x10B].name="INTCON";
	variables[0x10C].name="EEDATA";
	variables[0x10D].name="EEADR";
	variables[0x10E].name="EEDATH";
	variables[0x10F].name="EEADRH";
	variables[0x180].name="INDF";
	variables[0x181].name="OPTION_REG";
	variables[0x182].name="PCL";
	variables[0x183].name="STATUS";
	variables[0x184].name="FSR";
	variables[0x186].name="TRISB";
	variables[0x18A].name="PCLATH";
	variables[0x18B].name="INTCON";
	variables[0x18C].name="EECON1";
	variables[0x18D].name="EECON2";
}
///
///Show program info window
void info(GtkWidget *widget,GtkWidget *window)
{
  const gchar *license =
    "This program is free software; you can redistribute it and/or\n"
    "modify it under the terms of the GNU Library General Public License as\n"
    "published by the Free Software Foundation; either version 2 of the\n"
    "License, or (at your option) any later version.\n"
    "\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
    "Library General Public License for more details.\n"
    "\n"
    "You should have received a copy of the GNU Library General Public\n"
    "License along with the Gnome Library; see the file COPYING.LIB.  If not,\n"
    "write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,\n"
    "Boston, MA 02111-1307, USA.\n";
	GdkPixbuf *logo = gdk_pixbuf_new_from_resource_at_scale("/openprog/icons/opgui.svg",
			200,-1,TRUE,NULL);
	gtk_show_about_dialog (NULL,
		//"artists"                  GStrv*                : Read / Write
		//"authors"                  GStrv*                : Read / Write
		//"authors","Alberto Maccioni",NULL,
		"comments", "A graphical interface for the Open Programmer",
		"copyright",
		"Copyright (C) Alberto Maccioni 2009-2021\n\n"
		"This program is free software; you can \n"
		"redistribute it and/or modify it under \n"
		"the terms of the GNU General Public License \n"
		"as published by the Free Software Foundation;\n"
		"either version 2 of the License, or \n"
		"(at your option) any later version.",
		//"documenters"              GStrv*                : Read / Write
		"license",license,
		"logo",logo,
		//  "logo-icon-name"           gchar*                : Read / Write
		"program-name", "OPGUI",
		//  "translator-credits"       gchar*                : Read / Write
		"version",VERSION,
		"website","www.openprog.altervista.org",
		//  "website-label"            gchar*                : Read / Write
  		"wrap-license",TRUE,
		"title","Info about OPGUI",
		NULL);
}
///
///Show ICD help window
void ICDHelp(GtkWidget *widget,GtkWidget *window)
{
	GtkWidget* dialog = gtk_message_dialog_new (GTK_WINDOW(window),
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_INFO,
                                 GTK_BUTTONS_CLOSE,
                                 strings[I_ICD_HELP_TXT]);
	/* Destroy the dialog when the user responds to it (e.g. clicks a button) */
	g_signal_connect_swapped (GTK_WINDOW(dialog), "response",G_CALLBACK (gtk_widget_destroy),dialog);
	gtk_window_set_title(GTK_WINDOW(dialog),strings[I_ICD_HELP]);
	gtk_widget_show_all (dialog);
}
///
///ICD: check if program is running
gboolean icdCheck(GtkWidget *widget,GtkWidget *window)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return G_SOURCE_CONTINUE;
#endif
	if(!isRunning()){
		g_source_remove(icdTimer);
		ShowContext();
	}
}
///
///ICD: run program
void icdRun(GtkWidget *widget,GtkWidget *window)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	if(!icdConnected){
		saveLog = (int) gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b_log));
		if(saveLog){
			OpenLogFile();	//"Log.txt"
			fprintf(logfile,"ICD start\n");
		}
		startICD(Tck);	//start ICD mode by supplying the target and forcing a reset
		run();			//remove reset
		icdConnected=1;
		icdTimer=g_timeout_add(20,(GSourceFunc)icdCheck,NULL);
		PrintMessageICD("running");
	}
	else if(!running){
		cont(break_addr,freeze);	//continue execution
		icdTimer=g_timeout_add(20,(GSourceFunc)icdCheck,NULL);
		PrintMessageICD("running");
	}
}
///
///ICD: halt program
void icdHalt(GtkWidget *widget,GtkWidget *window)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	if(running){
		g_source_remove(icdTimer);
		Halt();
		ShowContext();
	}
}
///
///ICD: step program
void icdStep(GtkWidget *widget,GtkWidget *window)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	if(running){
		g_source_remove(icdTimer);
		Halt();
	}
	step();
#ifdef DEBUG
	addrDebug++;
#endif
	ShowContext();
}
///
///ICD: step program jumping over calls
void icdStepOver(GtkWidget *widget,GtkWidget *window)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	int addr,data;
	if(running){
		g_source_remove(icdTimer);
		Halt();
	}
	addr=((ReadRegister(0x18E)&0x1F)<<8)+ReadRegister(0x18F);
	data=ReadProgMem(addr);
	if((data>>11)==4){	//if call break at return address
		cont(addr+1,freeze);
		icdTimer=g_timeout_add(20,(GSourceFunc)icdCheck,NULL);
	}
	else{		//normal step
		step();
		#ifdef DEBUG
		addrDebug++;
		#endif
		ShowContext();
	}
}
///
///ICD: stop program
void icdStop(GtkWidget *widget,GtkWidget *window)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	if(running){
		g_source_remove(icdTimer);
		Halt();
	}
//	bufferU[0]=0;
	int j=0;
	bufferU[j++]=EN_VPP_VCC;		// reset target
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x2;				//set D as input
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(3);
	if(saveLog)WriteLogIO();
	icdConnected=0;
	PrintMessageICD("stopped");
	scrollToLine(source_info[0].src_line);
}
///
///ICD: refresh status
void icdRefresh(GtkWidget *widget,GtkWidget *window)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	if(!running){
		ShowContext();
	}
}
///
/// Read and display an entire bank of memory
void ShowBank(int bank,char* status){
	if(bank>3) bank=3;
	if(bank<0) bank=0;
	int b[128];
	char temp[128];
	int i;
	sprintf(temp,"bank %d:",bank);
	strcat(status,temp);
	ReadRegisterN(bank*0x80,128,b);
	for(i=0;i<128;i++){
		if(i%16==0){
			sprintf(temp,"\n0x%03X:",i+bank*0x80);
			strcat(status,temp);
		}
		sprintf(temp,"%02X",b[i]);
		strcat(status,temp);
	}
	strcat(status,"\n");
}
///
/// Main ICD show function:
/// prints status info according to selected options
/// and the value of variables in the watch list
void ShowContext(){
	int i,addr,data,s;
	char cmd[32]="";
	char status[4096]="",temp[128];
	addr=((ReadRegister(0x18E)&0x1F)<<8)+ReadRegister(0x18F);
	data=ReadProgMem(addr);
	s=ReadRegister(status_temp);
	s=(s>>4)+((s<<4)&0xF0);		//STATUS is swapped
//	printf("addr %X, status %X, data %X\n",addr,s,data);
#ifdef DEBUG
	addr=addrDebug;
	s=statusDebug;
	if(UseCoff) data=coff_data[addr];
#endif
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdMenuPC))){
		sprintf(temp,"%s: %s (0x%04X) \nPC=0x%04X\n",strings[S_NextIns],decodeCmd(data,cmd,(s&0x60)<<2),data,addr); //"Next instruction"
		strcat(status,temp);
	}
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdMenuSTAT))){
		sprintf(temp,"STATUS=0x%02X (",s);
		strcat(status,temp);
		sprintf(temp,"%s ",s&0x80?"IRP":"   ");
		strcat(status,temp);
		sprintf(temp,"%s ",s&0x40?"RP1":"   ");
		strcat(status,temp);
		sprintf(temp,"%s ",s&0x20?"RP0":"   ");
		strcat(status,temp);
		sprintf(temp,"%s ",s&0x10?"TO":"  ");
		strcat(status,temp);
		sprintf(temp,"%s ",s&0x8?"PD":"  ");
		strcat(status,temp);
		sprintf(temp,"%s ",s&0x4?"Z":" ");
		strcat(status,temp);
		sprintf(temp,"%s ",s&0x2?"DC":"  ");
		strcat(status,temp);
		sprintf(temp,"%s)\n",s&0x1?"C":" ");
		strcat(status,temp);
		sprintf(temp,"W=0x%02X PCLATH=0x%02X FSR=0x%02X\n",ReadRegister(w_temp),ReadRegister(pclath_temp),ReadRegister(fsr_temp));
		strcat(status,temp);
	}
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdMenuBank0))) ShowBank(0,status);
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdMenuBank1))) ShowBank(1,status);
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdMenuBank2))) ShowBank(2,status);
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdMenuBank3))) ShowBank(3,status);
	int rawsource=0;
	if(UseCoff){	//hilight corresponding source line
		if(data!=coff_data[addr]){
			sprintf(temp,"code at address 0x%04X (0x%04X) is different than what specified in coff file (0x%04X)\n",addr,data,coff_data[addr]);
			strcat(status,temp);
			rawsource=1;
		}
#ifndef DEBUG
		else{
#endif
//			printf("addr %d, file %d, line %d, current %d, ptr %X\n",addr,source_info[addr].src_file,source_info[addr].src_line,currentSource,s_files[source_info[addr].src_file].ptr);
			if(source_info[addr].src_file!=-1){
				if(currentSource==source_info[addr].src_file)	scrollToLine(source_info[addr].src_line);
				else if(loadSource(s_files[source_info[addr].src_file].ptr)){
					scrollToLine(source_info[addr].src_line);
					currentSource=source_info[addr].src_file;
				}
				else rawsource=1;
			}
			else rawsource=1;
#ifndef DEBUG
		}
#endif
	}
	if(!UseCoff || rawsource==1){	//show raw source if no source file is available
		#define LINES_BEFORE 5
		#define LINES_AFTER 7
		#define NLINES LINES_BEFORE + LINES_AFTER
		int addr0,addr1,line_pc=0;
		char tmp[64*NLINES],t2[64];
		tmp[0]=0;
		int progmem[NLINES];
		addr0=addr-LINES_BEFORE<0?0:addr-LINES_BEFORE;
		addr1=addr+LINES_AFTER>0x1FFF?0x1FFF:addr+LINES_AFTER;
		ReadProgMemN(addr0,addr1-addr0,progmem);
		for(i=addr0;i<addr1;i++){
			sprintf(t2,"0x%04X: %s (0x%04X)\n",i,decodeCmd(progmem[i-addr0],cmd,(s&0x60)<<2),progmem[i-addr0]);
			strcat(tmp,t2);
			if(i==addr) line_pc=i;
		}
		gtk_text_buffer_set_text(sourceBuf,tmp,-1);
		currentSource=-1;
		scrollToLine(line_pc-addr0+1);
	}
	for(i=0;i<nwatch;i++){
		sprintf(temp,"%s=0x%02X\n",watch[i].name,ReadRegister(watch[i].value));
		strcat(status,temp);
	}
	PrintMessageICD(status);
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdMenuEE))){
		unsigned char data[256];
		str[0]=0;
		char s[64],t[9],*g;
		t[8]=0;
		ReadDataMemN(0,256,data);
		strcat(str,"EEPROM:\n");
		for(i=0;i<0x100;i++){
			if(i%8==0){
				sprintf(s,"\n0x%02X: ",i);
				strcat(str,s);
			}
			sprintf(s,"%02X ",data[i]);
			strcat(str,s);
			t[i&0x7]=isprint(data[i])?data[i]:'.';
			if(i%8==7){
				g=g_locale_to_utf8(t,-1,NULL,NULL,NULL);
				if(g) strcat(str,g);
				g_free(g);
			}
		}
		AppendMessageICD(str);
	}
}
///
///Add symbol to the list of watched variables
int addWatch(struct symbol s){
	int i;
	for(i=0;i<nwatch&&strcmp(watch[i].name,s.name);i++);
	if(i<nwatch){	//remove watch
		for(;i<nwatch-1;i++){
			watch[i].name=watch[i+1].name;
			watch[i].value=watch[i+1].value;
		}
		nwatch--;
		watch=realloc(watch,nwatch*sizeof(struct symbol));
	}
	else{			//add watch
		nwatch++;
		watch=realloc(watch,nwatch*sizeof(struct symbol));
		watch[nwatch-1].name=s.name;
		watch[nwatch-1].value=s.value;
		return 1;
	}
	return 0;
}
///
/// ICD Command parser
int executeCommand(char *command){
//******************* break ********************************
	if(strstr(command,"break")){
		if(sscanf(command,"break %x",&break_addr)==1){
			if(running) Halt();
			break_addr&=0x1FFF;
			sprintf(str,"break at address 0x%04X\n",break_addr);
			AppendMessageICD(str);
			SourceRemoveHilightLine(sourceHilight);
			SourceHilightLine(source_info[break_addr].src_line);
			sourceHilight=source_info[break_addr].src_line;
		}
	}
//******************* clear ********************************
	if(strstr(command,"clear")){
		PrintMessageICD("");
	}
//******************* freeze ********************************
	else if(strstr(command,"freeze")){
		char option[32];
		if(sscanf(command,"freeze %s",option)==1){
			if(running) Halt();
			if(!strcmp(option,"on")) freeze=1;
			if(!strcmp(option,"off")) freeze=0;
			WriteRegister(0x18E,(break_addr>>8)+(freeze?0x40:0));
		}
		sprintf(str,"peripheral freeze is %s\n",freeze?"on":"off");
		AppendMessageICD(str);
	}
//******************* halt ********************************
	else if(!strcmp(command,"h")||!strcmp(command,"halt")){
		icdHalt(NULL,NULL);
	}
//******************* help ********************************
	else if(!strcmp(command,"help")){
		ICDHelp(NULL,NULL);
	}
//******************* list ********************************
	else if(strstr(command,"list ")){
		#define LISTLINES 10
		int addr,i;
		char tmp[32*LISTLINES],t2[32],cmd[32]="";
		tmp[0]=0;
		int progmem[LISTLINES];
		if(sscanf(command,"list %x",&addr)==1){
			addr&=0x1FFF;
			ReadProgMemN(addr,LISTLINES,progmem);
			for(i=0;i<LISTLINES;i++){
				sprintf(t2,"0x%04X: %s (0x%04X)\n",i+addr,decodeCmd(progmem[i],cmd,0),progmem[i]);
				strcat(tmp,t2);
			}
			//printf(tmp);
			AppendMessageICD(tmp);
		}
	}
//******************* print ********************************
	else if(strstr(command,"print ")||strstr(command,"p ")){
		int bank,i,addr,data;
		char var[128];
		if(strstr(command,"print p")||strstr(command,"p p")){	//program memory
			int addr;
			if(sscanf(command,"print p %x",&addr)==1||sscanf(command,"p p %x",&addr)==1){
				addr&=0x1FFF;
				if(running) Halt();
				data=ReadProgMem(addr);
				sprintf(str,"0x%04X: %s (0x%04X)\n",addr,decodeCmd(data,var,0x1000),data);
				AppendMessageICD(str);
			}
		}
		else if(!strcmp(command,"print ee")||!strcmp(command,"p ee")){	//eeprom
			unsigned char data[256];
			str[0]=0;
			char s[64],t[9],*g;
			t[8]=0;
			if(running) Halt();
			ReadDataMemN(0,256,data);
			sprintf(str,"EEPROM:\n");
			for(i=0;i<0x100;i++){
				if(i%8==0){
					sprintf(s,"\n0x%02X: ",i);
					strcat(str,s);
				}
				sprintf(s,"%02X ",data[i]);
				strcat(str,s);
				t[i&0x7]=isprint(data[i])?data[i]:'.';
				if(i%8==7){
					g=g_locale_to_utf8(t,-1,NULL,NULL,NULL);
					if(g) strcat(str,g);
					g_free(g);
				}
			}
			strcat(str,"\n");
			//printf("EEPROM:\n");fflush(stdout);
			AppendMessageICD(str);
		}
		else if(sscanf(command,"print ee %x",&addr)==1||sscanf(command,"p ee %x",&addr)==1){ //single EE address
			addr&=0xFF;
			if(running) Halt();
			data=ReadDataMem(addr);
			sprintf(str,"eeprom memory at 0x%02X=0x%02X (%c)\n",addr,data,isprint(data)?data:'.');
			AppendMessageICD(str);
		}
		else if(sscanf(command,"print bank %x",&bank)==1||sscanf(command,"p bank %x",&bank)==1){	//memory bank
			str[0]=0;
			bank&=0x1FF;
			if(bank>3) bank/=0x80;
			if(running) Halt();
			ShowBank(bank,str);
			AppendMessageICD(str);
		}
		else if(sscanf(command,"print 0x%x",&print_addr)==1||sscanf(command,"p 0x%x",&print_addr)==1){ //mem address
			print_addr&=0x1FF;
			if(running) Halt();
			sprintf(str,"[0x%03X]=0x%02X\n",print_addr,ReadRegister(print_addr));
			AppendMessageICD(str);
		}
		else if(sscanf(command,"print %s",var)==1||sscanf(command,"p %s",var)==1){ //var name
			str[0]=0;
			if(running) Halt();
			if(!strcmp("W",var)||!strcmp("w",var)) sprintf(str,"W = 0x%02X\n",ReadRegister(w_temp));
			else if(!strcmp("STATUS",var)) sprintf(str,"0x003: STATUS = 0x%02X\n",ReadRegister(status_temp));
			else if(!strcmp("FSR",var)) sprintf(str,"0x004: FSR = 0x%02X\n",ReadRegister(fsr_temp));
			else if(!strcmp("PCLATH",var)) sprintf(str,"0x00A: PCLATH = 0x%02X\n",ReadRegister(pclath_temp));
			else{
				for(i=0;i<nsym&&strcmp(var,sym[i].name);i++);
				if(i<nsym){
					sprintf(str,"0x%03X: %s = 0x%02X\n",sym[i].value,sym[i].name,ReadRegister(sym[i].value));
				}
				else{	//look in standard variables
					for(i=0;i<0x200;i++){
						if(variables[i].name&&!strcmp(var,variables[i].name)){
							sprintf(str,"0x%03X: %s = 0x%02X\n",i,variables[i].name,ReadRegister(i));
							i=0x200;
						}
					}
				}
			}
			AppendMessageICD(str);
		}
	}
//******************* run ********************************
	else if(!strcmp(command,"r")||!strcmp(command,"run")){
		icdRun(NULL,NULL);
	}
//******************* step ********************************
	else if(!strcmp(command,"s")||!strcmp(command,"step")||strstr(command,"step")||strstr(command,"s ")){
		int i,n=1;
		sscanf(command,"step %d",&n);
		sscanf(command,"s %d",&n);
#ifdef DEBUG
		addrDebug+=n;
#endif
		if(running) Halt();
		for(i=0;i<n;i++) step();
		if(n>1)sprintf(str,"step %d\n",n);
		else str[0]=0;
		ShowContext();
		AppendMessageICD(str);
	}
//******************* step over ********************************
	else if(!strcmp(command,"ss")||!strcmp(command,"step over")||strstr(command,"step over ")||strstr(command,"ss ")){
		int i,n=1;
		sscanf(command,"step over %d",&n);
		sscanf(command,"ss %d",&n);
#ifdef DEBUG
		addrDebug+=n;
#endif
		for(i=0;i<n;i++) icdStepOver(NULL,NULL);
		if(n>1)sprintf(str,"step over %d\n",n);
		AppendMessageICD(str);
	}
//******************* version ********************************
	else if(!strcmp(command,"ver")||!strcmp(command,"version")){
		if(running) Halt();
		sprintf(str,"debugger version: %.1f\n",version()/10.0);
		AppendMessageICD(str);
	}
//******************* watch ********************************
	else if(strstr(command,"watch ")||strstr(command,"w ")){
		int i,var_addr;
		char var[64];
		if(sscanf(command,"watch 0x%x",&var_addr)||sscanf(command,"w 0x%x",&var_addr)){
			struct symbol s;
			sprintf(var,"[0x%X]",var_addr);
			s.name=strdup(var);
			s.value=var_addr;
			addWatch(s);
			if(!running) ShowContext();
		}
		else if(sscanf(command,"watch %s",var)||sscanf(command,"w %s",var)){
			for(i=0;i<nsym&&strcmp(var,sym[i].name);i++);
			if(i<nsym){
				addWatch(sym[i]);
				if(!running) ShowContext();
			}
			else{	//look in standard variables
				for(i=0;i<0x200;i++){
					if(variables[i].name&&!strcmp(var,variables[i].name)){
						struct symbol s;
						s.name=variables[i].name;
						s.value=i;
						addWatch(s);
						if(!running) ShowContext();
						i=0x200;
					}
				}
			}
		}
	}
//******************* set variable ********************************
//to do: special addresses (PC, status ecc)
	else{
		char var[64],*p;
		int data,i,addr=-1;
		if((p=strchr(command,'='))){
			*p=0;
			if(sscanf(command,"[%x]",&addr)&&sscanf(p+1,"%x",&data)){
				if(running) Halt();
				WriteRegister(addr,data);
				ShowContext();
				sprintf(str,"[0x%x]=0x%02X\n",addr,data);
				AppendMessageICD(str);
			}
			else if(sscanf(command,"%s",var)&&sscanf(p+1,"%x",&data)){
				for(i=0;i<nsym&&strcmp(var,sym[i].name);i++);
				if(i<nsym&&sym[i].value<0x400){
					addr=sym[i].value;
				}
				else{	//look in standard variables
					for(i=0;i<0x200;i++){
						if(variables[i].name&&!strcmp(var,variables[i].name)){
							addr=i;
							i=0x200;
						}
					}
				}
				if(addr!=-1){
					if(running) Halt();
					WriteRegister(addr,data);
					ShowContext();
					sprintf(str,"%s=0x%02X\n",var,data);
					AppendMessageICD(str);
				}
			}
		}
		else return 0;
	}
	return 1;
}
///
///Remove variable from watch list
int removeWatch(char* name){
	int i;
	for(i=0;i<nwatch&&strcmp(watch[i].name,name);i++);
	if(i<nwatch){	//remove watch
		for(;i<nwatch-1;i++){
			watch[i].name=watch[i+1].name;
			watch[i].value=watch[i+1].value;
		}
		nwatch--;
		watch=realloc(watch,nwatch*sizeof(struct symbol));
		return 1;
	}
	return 0;
}
///
///Handle mouse events in source code window
gint source_mouse_event(GtkWidget *widget, GdkEventButton *event, gpointer func_data)
{
	if(GTK_IS_TEXT_VIEW(widget)&&event->type==GDK_2BUTTON_PRESS){
		gint x,y,i;
		GtkTextIter iter,iter2,itx;
		gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(widget),GTK_TEXT_WINDOW_WIDGET,event->x,event->y,&x,&y);
		gtk_text_view_get_iter_at_location(GTK_TEXT_VIEW(widget),&iter,x,y);
//		printf("x %d y %d\n",x,y);
		iter2=iter;
		char c;
		for(itx=iter2,c=gtk_text_iter_get_char(&itx);isalnum(c)||c=='_';iter2=itx){
			gtk_text_iter_forward_char(&itx);
			c=gtk_text_iter_get_char(&itx);
		}
		for(itx=iter,c=gtk_text_iter_get_char(&itx);isalnum(c)||c=='_';iter=itx){
			gtk_text_iter_backward_char(&itx);
			c=gtk_text_iter_get_char(&itx);
		}
		gtk_text_iter_forward_char(&iter);
		char* selection=gtk_text_buffer_get_text(sourceBuf,&iter,&iter2,FALSE);
		for(i=0;i<nsym&&strcmp(selection,sym[i].name);i++);
		if(i<nsym){
			addWatch(sym[i]);
			ShowContext();
		}
		else{	//set breakpoint
			int line=gtk_text_iter_get_line(&iter)+1;
			for(i=0;i<LMAX;i++) if(source_info[i].src_line==line){
				//if(UseCoff && i>0 && (coff_data[i-1]>>11)!=4) i--; //if not a call break at previous address;
				break_addr=i;
				sprintf(str,"break at address 0x%x\n",i);
				AppendMessageICD(str);
				SourceRemoveHilightLine(sourceHilight);
				SourceHilightLine(line);
				sourceHilight=line;
				break;
			}
		}
	}
	return FALSE;
}
///
///Handle mouse events in ICD status window
gint icdStatus_mouse_event(GtkWidget *widget, GdkEventButton *event, gpointer func_data)
{
	if(GTK_IS_TEXT_VIEW(widget)&&event->type==GDK_2BUTTON_PRESS){
		gint x,y;
		GtkTextIter iter,iter2,itx;
		gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(widget),GTK_TEXT_WINDOW_WIDGET,event->x,event->y,&x,&y);
		gtk_text_view_get_iter_at_location  (GTK_TEXT_VIEW(widget),&iter,x,y);
		iter2=iter;
		char c;
		for(itx=iter2,c=gtk_text_iter_get_char(&itx);isalnum(c)||c=='_';iter2=itx){
			gtk_text_iter_forward_char(&itx);
			c=gtk_text_iter_get_char(&itx);
		}
		for(itx=iter,c=gtk_text_iter_get_char(&itx);isalnum(c)||c=='_';iter=itx){
			gtk_text_iter_backward_char(&itx);
			c=gtk_text_iter_get_char(&itx);
		}
		gtk_text_iter_forward_char(&iter);
		char* selection=gtk_text_buffer_get_text(statusBuf,&iter,&iter2,FALSE);
		if(removeWatch(selection)) ShowContext();
	}
	return FALSE;
}
///
///Handle keyboard events in ICD command edit box
gint icdCommand_key_event(GtkWidget *widget, GdkEventButton *event, gpointer func_data)
{
	if(event->type==GDK_KEY_PRESS&&((GdkEventKey*)event)->keyval==0xFF0D){	//enter
		char s[64];
		strncpy(s,gtk_entry_get_text(GTK_ENTRY(icdCommand)),63);
		if(!strlen(s)){
			strcpy(s,lastCmd);
			gtk_entry_set_text(GTK_ENTRY(icdCommand),s);	//briefly flash last command
			while (gtk_events_pending ()) gtk_main_iteration();
			msDelay(60);
		}
		else strcpy(lastCmd,s);
		if(executeCommand(s)) gtk_entry_set_text(GTK_ENTRY(icdCommand),"");
//		sprintf(s,"k=%X\n",((GdkEventKey*)event)->keyval);
//		AppendMessageICD(gtk_entry_get_text(icdCommand));
	}
	return FALSE;
}
///
///Handle keyboard events in ICD tab
gint icd_key_event(GtkWidget *widget, GdkEventButton *event, gpointer func_data)
{
	while (gtk_events_pending ()) gtk_main_iteration();	//wait completion of other tasks
	if(event->type==GDK_KEY_PRESS){
		switch(((GdkEventKey*)event)->keyval){
			case 0xFFBE:
				ICDHelp(NULL,NULL);	//F1 = help
				break;
			case 0xFFC2:
				icdHalt(NULL,NULL);	//F5 = halt
				break;
			case 0xFFC4:
				icdStep(NULL,NULL);	//F7 = step
				break;
			case 0xFFC5:
				icdStepOver(NULL,NULL);	//F8 = step over
				break;
			case 0xFFC6:
				icdRun(NULL,NULL);	//F9 = run
				break;
		}
//		char s[64];
//		sprintf(s,"k=%X\n",((GdkEventKey*)event)->keyval);
//		AppendMessageICD(s);
	}
	return FALSE;
}
///
/// Check or set IO signals according to IO tab controls
gboolean IOchanged(GtkWidget *widget,GtkWidget *window)
{
	if(progress) return G_SOURCE_CONTINUE;
#ifndef DEBUG
	if(DeviceDetected!=1) return G_SOURCE_CONTINUE;
#endif
	if(!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b_io_active))) return G_SOURCE_CONTINUE;
	int i,j=0;
	int trisa=1,trisb=0,trisc=0x30,latac=0,latb=0;
	int port=0,z;
	//char str[128]="IO:";
	char s2[64];
	str[0]=0;
	for(i=0;i<sizeof(ioButtons)/sizeof(ioButtons[0]);i++){
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ioButtons[i].r_0))){
			//strcat(str,"0");
		}
		else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ioButtons[i].r_1))){
			//strcat(str,"1");
			if(i<8) latb|=1<<i;
			else if(i==8) latac|=0x80; //RC7
			else if(i==9) latac|=0x40; //RC6
			else if(i==10) latac|=0x20; //RA5
			else if(i==11) latac|=0x10; //RA4
			else if(i==12) latac|=0x08; //RA3
		}
		else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ioButtons[i].r_I))){
			//strcat(str,"I");
			if(i<8)	trisb|=1<<i;
			else if(i==8) trisc|=0x80; //RC7
			else if(i==9) trisc|=0x40; //RC6
			else if(i==10) trisa|=0x20; //RA5
			else if(i==11) trisa|=0x10; //RA4
			else if(i==12) trisa|=0x8; //RA3
		}
	}
	//sprintf(s2," trisb=%02X latb=%02X trisc=%02X trisa=%02X latac=%02X",trisb,latb,trisc,trisa,latac);
	//strcat(str,s2);
	//gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,str);
	bufferU[j++]=READ_RAM;
	bufferU[j++]=0x0F;
	bufferU[j++]=0x80;	//PORTA
	bufferU[j++]=READ_RAM;
	bufferU[j++]=0x0F;
	bufferU[j++]=0x81;	//PORTB
	bufferU[j++]=READ_RAM;
	bufferU[j++]=0x0F;
	bufferU[j++]=0x82;	//PORTC
	bufferU[j++]=WRITE_RAM;
	bufferU[j++]=0x0F;
	bufferU[j++]=0x92;	//TRISA
	bufferU[j++]=trisa;
	bufferU[j++]=WRITE_RAM;
	bufferU[j++]=0x0F;
	bufferU[j++]=0x93;	//TRISB
	bufferU[j++]=trisb;
	bufferU[j++]=WRITE_RAM;
	bufferU[j++]=0x0F;
	bufferU[j++]=0x94;	//TRISC
	bufferU[j++]=trisc;
	bufferU[j++]=EXT_PORT;
	bufferU[j++]=latb;
	bufferU[j++]=latac;
	bufferU[j++]=READ_ADC;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	for(z=0;z<DIMBUF-3&&bufferI[z]!=READ_RAM;z++);
	port=bufferI[z+3];	//PORTA
	//sprintf(s2," porta=%02X",port);
	//strcat(str,s2);
	//gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,str);
	gtk_entry_set_text(GTK_ENTRY(ioButtons[10].e_I),(port&0x20)?"1":"0");
	gtk_entry_set_text(GTK_ENTRY(ioButtons[11].e_I),(port&0x10)?"1":"0");
	gtk_entry_set_text(GTK_ENTRY(ioButtons[12].e_I),(port&0x8)?"1":"0");
	for(z+=4;z<DIMBUF-3&&bufferI[z]!=READ_RAM;z++);
	port=bufferI[z+3];	//PORTB
	for(i=0;i<8;i++) gtk_entry_set_text(GTK_ENTRY(ioButtons[i].e_I),(port&(1<<i))?"1":"0");
	for(z+=4;z<DIMBUF-3&&bufferI[z]!=READ_RAM;z++);
	port=bufferI[z+3];	//PORTC
	gtk_entry_set_text(GTK_ENTRY(ioButtons[8].e_I),(port&0x80)?"1":"0");
	gtk_entry_set_text(GTK_ENTRY(ioButtons[9].e_I),(port&0x40)?"1":"0");
	for(z+=4;z<DIMBUF-2&&bufferI[z]!=READ_ADC;z++);
	double vpp=((bufferI[z+1]<<8)+bufferI[z+2])/1024.0*5*34/12;	//VPP
	sprintf(str,"VPP=%.2fV",vpp);
	gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,str);
	return G_SOURCE_CONTINUE;
}
///
/// Start/stop timer to check for IO status
void IOactive(GtkWidget *widget,GtkWidget *window)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b_io_active))&&!icdTimer){
		IOTimer=g_timeout_add(100,(GSourceFunc)IOchanged,NULL);
	}
	else if(IOTimer){
		g_source_remove(IOTimer);
	}
}
///
/// Enable/disable VPP and VCC from IO tab
void VPPVDDactive(GtkWidget *widget,GtkWidget *window)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	int j=0,vdd_vpp=0;
	char str[16]="";
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(VPP_ON))){
		vdd_vpp+=4;
		strcat(str,"VPP ");
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(VDD_ON))){
		vdd_vpp+=1;
		strcat(str,"VDD ");
	}
	gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,str);
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=vdd_vpp;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
}
///
/// Enable/disable DCDC from IO tab or update DCDC voltage
void DCDCactive(GtkWidget *widget,GtkWidget *window)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(DCDC_ON))){
		int j=0,vreg=0;
		char str[16];
		double voltage=gtk_range_get_value(GTK_RANGE(DCDC_voltage));
		vreg=voltage*10.0;
		sprintf(str,"DCDC %.1fV",voltage);
		gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,str);
		bufferU[j++]=VREG_EN;			//enable HV regulator
		bufferU[j++]=SET_VPP;
		bufferU[j++]=vreg;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
	}
	else{
		int j=0;
		bufferU[j++]=VREG_DIS;			//disable HV regulator
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(2);
	}
}
///
/// convert hex line
void HexConvert(GtkWidget *widget,GtkWidget *window)
{
	char hex[256],str[256],s2[32];
	int i,address,length,sum=0;
	strncpy(hex,(const char *)gtk_entry_get_text(GTK_ENTRY(Hex_entry)),sizeof(hex));
	if(strlen(hex)>0){
		if(hex[0]==':'&&strlen(hex)>8){
			length=htoi(hex+1,2);
			address=htoi(hex+3,4);
			if(strlen(hex)<11+length*2) gtk_entry_set_text(GTK_ENTRY(Hex_data),"__line too short");
			else{
				for (i=1;i<=length*2+9;i+=2) sum += htoi(hex+i,2);
				if ((sum & 0xff)!=0){
					sprintf(str,"__checksum error, expected 0x%02X",(-sum+htoi(hex+9+length*2,2))&0xFF);
					gtk_entry_set_text(GTK_ENTRY(Hex_data),str);
				}
				else{
					switch(htoi(hex+7,2)){
						case 0:		//Data record
							sprintf(str,"address: 0x%04X ",address);
							if(i&&length) strcat(str,"data: 0x");
							for (i=0;i<length;i++){
								sprintf(s2,"%02X",htoi(hex+9+i*2,2));
								strcat(str,s2);
							}
							gtk_entry_set_text(GTK_ENTRY(Hex_data),str);
							break;
						case 4:		//extended linear address record
							if(strlen(hex)>14){
								sprintf(str,"extended linear address = %04X",htoi(hex+9,4));
								gtk_entry_set_text(GTK_ENTRY(Hex_data),str);
							}
							break;
						default:
							gtk_entry_set_text(GTK_ENTRY(Hex_data),"__unknown record type");
							break;
					}
				}
			}
		}
		else gtk_entry_set_text(GTK_ENTRY(Hex_data),"__invalid line");
	}
	else gtk_entry_set_text(GTK_ENTRY(Hex_entry),"");
}
///
/// convert address & data to hex line
void DataToHexConvert(GtkWidget *widget,GtkWidget *window)
{
	char hex[256],str[256],s2[32];
	int i,address,length,sum=0,x;
	i=sscanf(gtk_entry_get_text(GTK_ENTRY(Address_entry)),"%x",&address);
	if(i!=1) address=0;
	strncpy(hex,(const char *)gtk_entry_get_text(GTK_ENTRY(Data_entry)),sizeof(hex));
	length=strlen(hex);
	length&=0xFF;
	if(length>0){
		sprintf(str,":--%04X00",address&0xFFFF);
		for(i=0;i+1<length;i+=2){
			x=htoi(hex+i,2);
			//x&=0xFF;
			sum+=x;
			sprintf(s2,"%02X",x);
			strcat(str,s2);
		}
		sprintf(s2,"%02X",i/2);
		str[1]=s2[0];
		str[2]=s2[1];
		x=sum;
		sum+=i/2+(address&0xff)+((address>>8)&0xff);
		sprintf(s2,"%02X",(-sum)&0xFF);
		strcat(str,s2);
		gtk_entry_set_text(GTK_ENTRY(Hex_data2),str);
	}
}
///
///Choose a file to save a hex line
void HexSave(GtkWidget *widget,GtkWidget *window)
{
	GtkFileChooser *dialog;
	if(strlen((const char *)gtk_entry_get_text(GTK_ENTRY(Hex_data2)))<11) return;
	dialog = (GtkFileChooser*) gtk_file_chooser_dialog_new ("Save File",
				      GTK_WINDOW(window),
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      "_Cancel", GTK_RESPONSE_CANCEL,
				      "_Save", GTK_RESPONSE_ACCEPT,
				      NULL);
	if(cur_path) gtk_file_chooser_set_current_folder(dialog,cur_path);
	gtk_file_chooser_set_do_overwrite_confirmation(dialog,TRUE);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	  {
	    char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		if(cur_path) free(cur_path);
		cur_path = gtk_file_chooser_get_current_folder(dialog);
		FILE* f=fopen(filename,"w");
		if(f){
			fprintf(f,(const char *)gtk_entry_get_text(GTK_ENTRY(Hex_data2)));
			fclose(f);
		}
	    g_free (filename);
	  }
	gtk_widget_destroy (GTK_WIDGET(dialog));
}
///
/// Stop read or write
void Stop(GtkWidget *widget,GtkWidget *window)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	if(progress==1&&RWstop==0){
		RWstop=1;
		PrintMessage(strings[I_STOP]);
	}
}
///
///Close program
void Xclose(){
	char *str=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(devCombo));
	if(str) strncpy(dev,str,sizeof(dev)-1);
	gtk_main_quit();
}
///-----------------------------------
///Main function
///-----------------------------------
int main( int argc, char *argv[])
{
	//int langID=GetUserDefaultLangID();
	FILE *f;
	gchar *homedir,*config_dir,*fname=0;
	char lang[32]="";
	int langfile=0;
	homedir = (gchar *) g_get_home_dir ();
	if(homedir){
		config_dir=g_build_path(G_DIR_SEPARATOR_S,homedir,CONFIG_DIR, NULL);
		if(!g_file_test(config_dir,G_FILE_TEST_IS_DIR))
	#if defined _WIN32
		mkdir(config_dir);
	#else
		mkdir(config_dir,0755);
	#endif
		fname = g_build_path(G_DIR_SEPARATOR_S,config_dir,CONFIG_FILE, NULL);
		f=fopen(fname,"r");
		if(f){
			char temp[256],line[256];
			int X;
			for(;fgets(line,256,f);){
				if(sscanf(line,"device %s",temp)>0) strcpy(dev,temp);
				else if(sscanf(line,"vid %X",&X)>0) vid=X;
				else if(sscanf(line,"pid %X",&X)>0) pid=X;
				else sscanf(line,"maxerr %d",&max_err);
			}
			fclose(f);
		}
	}
	char dev_ini[64];
	strncpy(dev_ini,dev,sizeof(dev_ini));
	int vid_ini=vid,pid_ini=pid,max_err_ini=max_err;
	vid_ini=vid;
	pid_ini=pid;
	max_err_ini=max_err;
#if defined _WIN32 || defined __CYGWIN__	//Windows
	bufferI=bufferI0+1;
	bufferU=bufferU0+1;
	bufferI0[0]=0;
	bufferU0[0]=0;
#endif
	gtk_init(&argc, &argv);
	unsigned int tmpbuf[128];
	opterr = 0;
	int option_index = 0;
	int help=0,command=0,i,j;
	char c;
	struct option long_options[] =
	{
		{"?",             no_argument,           &help, 1},
		{"h",             no_argument,           &help, 1},
		{"help",          no_argument,           &help, 1},
		{"c",             no_argument,         &command, 1},
		{"command",       no_argument,         &command, 1},
		{"lang",          required_argument,       0, 'l'},
		{"langfile",      no_argument,       &langfile, 1},
		{0, 0, 0, 0}
	};
	while ((j = getopt_long_only (argc, argv, "",long_options,&option_index)) != -1){
		if(j=='l'){ //language
			strncpy(lang,optarg,sizeof(lang)-1);
		}
	}
	for(j=0,i = optind; i < argc&&i<128; i++,j++) sscanf(argv[i], "%x", &tmpbuf[j]);
	for(;j<128;j++) tmpbuf[j]=0;
	strinit();
	char* langid=0;
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
	if(langfile) GenerateLangFile(langid,"languages.rc");
	if(help){
		printf(strings[I_GUI_CMD_HELP]);
		exit(0);
	}
	if(command){
		cmdline=1;
		DeviceDetected=FindDevice(vid,pid);	//connect to USB programmer
		if(!DeviceDetected){
			DeviceDetected=FindDevice(new_vid,new_pid);	//try default
			if(DeviceDetected){
				vid=new_vid;
				pid=new_pid;
			}
		}
		if(!DeviceDetected) DeviceDetected=FindDevice(old_vid,old_pid); //try old one
		if(DeviceDetected){
			bufferU[0]=0;
			for(i=1;i<DIMBUF;i++) bufferU[i]=(char) tmpbuf[i-1];
			PacketIO(100);
			printf("> ");
			for(i=1;i<DIMBUF;i++) printf("%02X ",bufferU[i]);
			printf("\n< ");
			for(i=1;i<DIMBUF;i++) printf("%02X ",bufferI[i]);
			printf("\n");
		}
		else printf(strings[S_noprog]);
		exit(0);
	}
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	sprintf(str,"opgui v%s",VERSION);
	gtk_window_set_title(GTK_WINDOW(window),str);
	gtk_window_set_default_size(GTK_WINDOW(window), 750, 250);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	g_signal_connect(GTK_WINDOW(window), "destroy",G_CALLBACK(Xclose),NULL);
	gtk_window_set_icon(GTK_WINDOW(window),gdk_pixbuf_new_from_resource("/openprog/icons/sys.png", NULL));
	GtkWidget * vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);

	GtkCssProvider *cssProv = gtk_css_provider_new();
	gtk_css_provider_load_from_resource(cssProv, "/openprog/css/style.css");
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProv), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

//------toolbar-------------

	GtkWidget* iconOpen = gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_BUTTON);
	GtkToolItem* btnOpen = gtk_tool_button_new(iconOpen, strings[I_Fopen]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnOpen), strings[I_Fopen]);
	g_signal_connect(G_OBJECT(btnOpen), "clicked", G_CALLBACK(Fopen),NULL);

	GtkWidget* iconSave = gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_BUTTON);
	GtkToolItem* btnSave = gtk_tool_button_new(iconSave, strings[I_Fsave]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnSave), strings[I_Fsave]);
	g_signal_connect(G_OBJECT(btnSave), "clicked", G_CALLBACK(Fsave),NULL);

	GtkWidget* iconRead = gtk_image_new_from_resource("/openprog/icons/read.png");
	GtkToolItem* btnRead = gtk_tool_button_new(iconRead, strings[I_DevR]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnRead), strings[I_DevR]);
	g_signal_connect(G_OBJECT(btnRead), "clicked", G_CALLBACK(DevRead),NULL);

	GtkWidget* iconWrite = gtk_image_new_from_resource("/openprog/icons/write.png");
	GtkToolItem* btnWrite = gtk_tool_button_new(iconWrite, strings[I_DevW]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnWrite), strings[I_DevW]);
	g_signal_connect(G_OBJECT(btnWrite), "clicked", G_CALLBACK(DevWrite),NULL);

	GtkWidget* iconStop = gtk_image_new_from_icon_name("process-stop", GTK_ICON_SIZE_BUTTON);
	btnStop = gtk_tool_button_new(iconStop, strings[I_ICD_STOP]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnStop), strings[I_ICD_STOP]);
	g_signal_connect(G_OBJECT(btnStop), "clicked", G_CALLBACK(Stop),NULL);

	GtkWidget* iconConnect = gtk_image_new_from_icon_name("network-wired", GTK_ICON_SIZE_BUTTON);
	GtkToolItem* btnConnect = gtk_tool_button_new(iconConnect, strings[I_CONN]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnConnect), strings[I_CONN]);
	g_signal_connect(G_OBJECT(btnConnect), "clicked", G_CALLBACK(Connect),NULL);

	GtkWidget* iconInfo = gtk_image_new_from_icon_name("dialog-information", GTK_ICON_SIZE_BUTTON);
	GtkToolItem* btnInfo = gtk_tool_button_new(iconInfo, strings[I_Info]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnInfo), strings[I_Info]);
	g_signal_connect(G_OBJECT(btnInfo), "clicked", G_CALLBACK(info),NULL);

	toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar),GTK_TOOLBAR_ICONS);
	gtk_box_pack_start(GTK_BOX(vbox),toolbar,FALSE,FALSE,0);

	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btnOpen, -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btnSave, -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_separator_tool_item_new(), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btnRead, -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btnWrite, -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_separator_tool_item_new(), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btnStop, -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btnConnect, -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btnInfo, -1);

	gtk_widget_set_sensitive(GTK_WIDGET(btnStop), FALSE);

//------tab widget-------------
	notebook = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(vbox),notebook,TRUE,TRUE,0);
//------data tab-------------
	data_scroll = gtk_scrolled_window_new(NULL,NULL);
	data = gtk_text_view_new();
	dataBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(data),FALSE);
	gtk_container_add(GTK_CONTAINER(data_scroll),data);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),data_scroll,gtk_label_new(strings[I_Data])); //"Data"

	styleCtx = gtk_widget_get_style_context(GTK_WIDGET(data));
	gtk_style_context_add_class(styleCtx, "mono");

//------device tab-------------
	label = gtk_label_new(strings[I_Dev]);	//"Device"
	GtkWidget * devGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(devGrid), 5);
	gtk_grid_set_row_spacing(GTK_GRID(devGrid), 5);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),devGrid,label);
	GtkWidget * devHbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);

	gtk_grid_attach(GTK_GRID(devGrid),devHbox1,0,0,2,1);
	gtk_box_pack_start(GTK_BOX(devHbox1),gtk_label_new(strings[I_Type]),FALSE,TRUE,0); //"Type"
	devTypeCombo = gtk_combo_box_text_new();
	gtk_box_pack_start(GTK_BOX(devHbox1),devTypeCombo,FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(devHbox1),gtk_label_new("   "),FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(devHbox1),gtk_label_new(strings[I_Dev]),FALSE,TRUE,0); //"Device"
	devCombo = gtk_combo_box_text_new();
	gtk_box_pack_start(GTK_BOX(devHbox1),devCombo,FALSE,TRUE,0);
	GtkWidget * devHbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);

	gtk_grid_attach(GTK_GRID(devGrid),devHbox2,0,1,2,1);
	gtk_box_pack_start(GTK_BOX(devHbox2),gtk_label_new("info: "),FALSE,FALSE,0);
	devinfo = gtk_label_new("i");
	gtk_box_pack_start(GTK_BOX(devHbox2),devinfo,FALSE,FALSE,0);
	EEPROM_RW = gtk_check_button_new_with_label(strings[I_EE]);	//"Read and write EEPROM"
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(EEPROM_RW),TRUE);

	gtk_grid_attach(GTK_GRID(devGrid),EEPROM_RW,0,2,1,1);
	devFramePIC = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);//gtk_frame_new(NULL);	//"PIC configuration"

	gtk_grid_attach(GTK_GRID(devGrid),devFramePIC,0,3,1,1);

	GtkWidget * table_PIC = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(table_PIC), 5);
	gtk_grid_set_row_spacing(GTK_GRID(table_PIC), 5);

	gtk_box_pack_start(GTK_BOX(devFramePIC),table_PIC,FALSE,TRUE,0);
	GtkWidget * devVboxPIC = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_grid_attach(GTK_GRID(table_PIC),devVboxPIC,0,0,1,1);

	ReadReserved = gtk_check_button_new_with_label(strings[I_ReadRes]);	//"Read reserved area"
	gtk_container_add(GTK_CONTAINER(devVboxPIC),GTK_WIDGET(ReadReserved));
	Write_ID_BKCal = gtk_check_button_new_with_label(strings[I_ID_BKo_W]);	//"Write ID and BKOscCal"
	gtk_container_add(GTK_CONTAINER(devVboxPIC),GTK_WIDGET(Write_ID_BKCal));
	WriteCalib12 = gtk_check_button_new_with_label(strings[I_CalW]);	//"Write Calib 1 and 2"
	gtk_container_add(GTK_CONTAINER(devVboxPIC),GTK_WIDGET(WriteCalib12));
	devFrameOsc = gtk_frame_new(strings[I_OSCW]);	//"Write OscCal"

	gtk_grid_attach(GTK_GRID(table_PIC),devFrameOsc,0,1,1,1);
	GtkWidget * devVboxOsc = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_container_add(GTK_CONTAINER(devFrameOsc),GTK_WIDGET(devVboxOsc));
	UseOSCCAL = gtk_radio_button_new_with_label(NULL,strings[I_OSC]);	//"OSCCal"
	UseBKOSCCAL = gtk_radio_button_new_with_label(\
		gtk_radio_button_get_group(GTK_RADIO_BUTTON(UseOSCCAL)),strings[I_BKOSC]);	//"Backup OSCCal"
	UseFileCal = gtk_radio_button_new_with_label(\
		gtk_radio_button_get_group(GTK_RADIO_BUTTON(UseOSCCAL)),strings[I_OSCF]);	//"From file"
	gtk_container_add(GTK_CONTAINER(devVboxOsc),GTK_WIDGET(UseOSCCAL));
	gtk_container_add(GTK_CONTAINER(devVboxOsc),GTK_WIDGET(UseBKOSCCAL));
	gtk_container_add(GTK_CONTAINER(devVboxOsc),GTK_WIDGET(UseFileCal));
	devFrameICD = gtk_frame_new("ICD");
	gtk_grid_attach(GTK_GRID(table_PIC),devFrameICD,0,2,2,1);

	GtkWidget * devVboxICD = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,25);
	gtk_container_add(GTK_CONTAINER(devFrameICD),GTK_WIDGET(devVboxICD));
	ICD_check = gtk_check_button_new_with_label(strings[I_ICD_ENABLE]);	//"Enable ICD"
	gtk_container_add(GTK_CONTAINER(devVboxICD),ICD_check);
	GtkWidget * devHboxICD = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	label = gtk_label_new(strings[I_ICD_ADDRESS]);	//"ICD routine address"
	gtk_box_pack_start(GTK_BOX(devHboxICD),GTK_WIDGET(label),0,0,1);
	ICD_addr_entry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(ICD_addr_entry),4);
	gtk_box_pack_start(GTK_BOX(devHboxICD),GTK_WIDGET(ICD_addr_entry),0,0,2);
	gtk_container_add(GTK_CONTAINER(devVboxICD),GTK_WIDGET(devHboxICD));
	devFrameConfigW = gtk_frame_new("Config Word");

	gtk_grid_attach(GTK_GRID(table_PIC),devFrameConfigW,1,0,1,2);
	GtkWidget * cwGrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(devFrameConfigW),GTK_WIDGET(cwGrid));
	ConfigForce = gtk_check_button_new_with_label(strings[I_PIC_FORCECW]); //"force config word"
	gtk_grid_attach(GTK_GRID(cwGrid),ConfigForce,0,0,2,1);
#define CWX(y) 	devPIC_CW##y = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);\
				label = gtk_label_new("CW"#y);\
				gtk_box_pack_start(GTK_BOX(devPIC_CW##y),GTK_WIDGET(label),0,0,1);\
				CW##y##_entry = gtk_entry_new();\
				gtk_entry_set_width_chars(GTK_ENTRY(CW##y##_entry),4);\
				gtk_box_pack_start(GTK_BOX(devPIC_CW##y),GTK_WIDGET(CW##y##_entry),0,0,1);
	CWX(1);
	CWX(2);
	CWX(3);
	CWX(4);
	CWX(5);
	CWX(6);
	CWX(7);
	gtk_grid_attach(GTK_GRID(cwGrid),devPIC_CW1,0,1,1,1);
	gtk_grid_attach(GTK_GRID(cwGrid),devPIC_CW2,1,1,1,1);
	gtk_grid_attach(GTK_GRID(cwGrid),devPIC_CW3,0,2,1,1);
	gtk_grid_attach(GTK_GRID(cwGrid),devPIC_CW4,1,2,1,1);
	gtk_grid_attach(GTK_GRID(cwGrid),devPIC_CW5,0,3,1,1);
	gtk_grid_attach(GTK_GRID(cwGrid),devPIC_CW6,1,3,1,1);
	gtk_grid_attach(GTK_GRID(cwGrid),devPIC_CW7,0,4,1,1);
	devFrameAVR = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);	//Atmel configuration

	gtk_grid_attach(GTK_GRID(devGrid),devFrameAVR,0,3,1,1);
	GtkWidget * avrGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(avrGrid), 5);
	gtk_grid_set_row_spacing(GTK_GRID(avrGrid), 2);

	gtk_box_pack_start(GTK_BOX(devFrameAVR),avrGrid,FALSE,TRUE,0);
	AVR_FuseLow = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(AVR_FuseLow),4);
	gtk_grid_attach(GTK_GRID(avrGrid),AVR_FuseLow,0,1,1,1);
	AVR_FuseLowWrite = gtk_check_button_new_with_label(strings[I_AT_FUSE]);	//"Write Fuse Low"
	gtk_grid_attach(GTK_GRID(avrGrid),AVR_FuseLowWrite,1,1,1,1);
	AVR_FuseHigh = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(AVR_FuseHigh),4);
	gtk_grid_attach(GTK_GRID(avrGrid),AVR_FuseHigh,0,2,1,1);
	AVR_FuseHighWrite = gtk_check_button_new_with_label(strings[I_AT_FUSEH]);	//"Write Fuse High"
	gtk_grid_attach(GTK_GRID(avrGrid),AVR_FuseHighWrite,1,2,1,1);
	AVR_FuseExt = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(AVR_FuseExt),4);
	gtk_grid_attach(GTK_GRID(avrGrid),AVR_FuseExt,0,3,1,1);
	AVR_FuseExtWrite = gtk_check_button_new_with_label(strings[I_AT_FUSEX]);	//"Write Extended Fuse"
	gtk_grid_attach(GTK_GRID(avrGrid),AVR_FuseExtWrite,1,3,1,1);
	AVR_Lock = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(AVR_Lock),4);
	gtk_grid_attach(GTK_GRID(avrGrid),AVR_Lock,0,4,1,1);
	AVR_LockWrite = gtk_check_button_new_with_label(strings[I_AT_LOCK]);	//"Write Lock"
	gtk_grid_attach(GTK_GRID(avrGrid),AVR_LockWrite,1,4,1,1);
	b_WfuseLF = gtk_button_new_with_label(strings[I_AT_FUSELF]);		//"Write Fuse Low @3kHz"
	gtk_grid_attach(GTK_GRID(avrGrid),b_WfuseLF,0,5,1,1);
//------options tab-------------
	GtkWidget * optGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(optGrid), 5);
	gtk_grid_set_row_spacing(GTK_GRID(optGrid), 5);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),optGrid,gtk_label_new(strings[I_Opt])); //"Options"
	b_connect = gtk_button_new_with_label(strings[I_CONN]);	//"Reconnect"
	gtk_grid_attach(GTK_GRID(optGrid),b_connect,0,0,1,1);
	GtkWidget * optHboxVid = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_grid_attach(GTK_GRID(optGrid),optHboxVid,0,1,1,1);
	gtk_box_pack_start(GTK_BOX(optHboxVid),gtk_label_new("VID:"),FALSE,TRUE,0);
	VID_entry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(VID_entry),4);
	gtk_box_pack_start(GTK_BOX(optHboxVid),VID_entry,FALSE,TRUE,0);
	GtkWidget * optHboxPid = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_grid_attach(GTK_GRID(optGrid),optHboxPid,1,1,1,1);
	gtk_box_pack_start(GTK_BOX(optHboxPid),gtk_label_new("PID:"),FALSE,TRUE,0);
	PID_entry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(PID_entry),4);
	gtk_box_pack_start(GTK_BOX(optHboxPid),PID_entry,FALSE,TRUE,0);

	b_testhw = gtk_button_new_with_label(strings[I_TestHWB]);	//"Hardware test"
	gtk_grid_attach(GTK_GRID(optGrid),b_testhw,0,2,1,1);
	b_log = gtk_check_button_new_with_label(strings[I_LOG]);	//"Log activity"
	gtk_grid_attach(GTK_GRID(optGrid),b_log,0,6,2,1);
	b_V33check = gtk_check_button_new_with_label(strings[I_CK_V33]);	//"Don't check for 3.3V regulator"
	gtk_grid_attach(GTK_GRID(optGrid),b_V33check,0,7,2,1);
	b_WaitS1 = gtk_check_button_new_with_label(strings[I_WAITS1]);	//"Wait for S1 before read/write"
	gtk_grid_attach(GTK_GRID(optGrid),b_WaitS1,0,8,2,1);
	GtkWidget * optHboxErr = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_grid_attach(GTK_GRID(optGrid),optHboxErr,0,9,2,1);
	gtk_container_add(GTK_CONTAINER(optHboxErr),gtk_label_new(strings[I_MAXERR])); //"Max errors"
	Errors_entry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(Errors_entry),6);
	gtk_container_add(GTK_CONTAINER(optHboxErr),GTK_WIDGET(Errors_entry));
//------I2C tab-------------
	GtkWidget * i2cGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(i2cGrid),5);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),i2cGrid,gtk_label_new("I2C/SPI"));
	GtkWidget * i2cModeFrame = gtk_frame_new(strings[I_I2CMode]);	//"Mode"
	gtk_grid_attach(GTK_GRID(i2cGrid),i2cModeFrame,0,0,1,5);
	GtkWidget * i2cVboxMode = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
	gtk_container_add(GTK_CONTAINER(i2cModeFrame),i2cVboxMode);
	I2C8bit = gtk_radio_button_new_with_label(NULL,"I2C 8 Bit");
	I2C16bit = gtk_radio_button_new_with_label(\
		gtk_radio_button_get_group(GTK_RADIO_BUTTON(I2C8bit)),"I2C 16 Bit");
	SPI00 = gtk_radio_button_new_with_label(\
		gtk_radio_button_get_group(GTK_RADIO_BUTTON(I2C8bit)),"SPI(00)");
	SPI01 = gtk_radio_button_new_with_label(\
		gtk_radio_button_get_group(GTK_RADIO_BUTTON(I2C8bit)),"SPI(01)");
	SPI10 = gtk_radio_button_new_with_label(\
		gtk_radio_button_get_group(GTK_RADIO_BUTTON(I2C8bit)),"SPI(10)");
	SPI11 = gtk_radio_button_new_with_label(\
		gtk_radio_button_get_group(GTK_RADIO_BUTTON(I2C8bit)),"SPI(11)");
	gtk_container_add(GTK_CONTAINER(i2cVboxMode),GTK_WIDGET(I2C8bit));
	gtk_container_add(GTK_CONTAINER(i2cVboxMode),GTK_WIDGET(I2C16bit));
	gtk_container_add(GTK_CONTAINER(i2cVboxMode),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
	gtk_container_add(GTK_CONTAINER(i2cVboxMode),GTK_WIDGET(SPI00));
	gtk_container_add(GTK_CONTAINER(i2cVboxMode),GTK_WIDGET(SPI01));
	gtk_container_add(GTK_CONTAINER(i2cVboxMode),GTK_WIDGET(SPI10));
	gtk_container_add(GTK_CONTAINER(i2cVboxMode),GTK_WIDGET(SPI11));
	GtkWidget * i2cVboxTX = gtk_box_new(GTK_ORIENTATION_VERTICAL,2);
	gtk_grid_attach(GTK_GRID(i2cGrid),i2cVboxTX,1,3,2,1);
	label = gtk_label_new(strings[I_I2CDATAOUT]);	//"Data to send"
	gtk_container_add(GTK_CONTAINER(i2cVboxTX),label);
	I2CDataSend = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(i2cVboxTX),GTK_WIDGET(I2CDataSend));
	GtkWidget * i2cVboxRX = gtk_box_new(GTK_ORIENTATION_VERTICAL,2);
	gtk_grid_attach(GTK_GRID(i2cGrid),i2cVboxRX,1,4,2,1);
	label = gtk_label_new(strings[I_I2CDATATR]);	//"Data transferred"
	gtk_container_add(GTK_CONTAINER(i2cVboxRX),label);
	I2CDataReceive = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(I2CDataReceive),FALSE);
	gtk_container_add(GTK_CONTAINER(i2cVboxRX),GTK_WIDGET(I2CDataReceive));
	gtk_widget_set_size_request(I2CDataReceive,100,60);
	GtkWidget * i2cHboxNB = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_grid_attach(GTK_GRID(i2cGrid),i2cHboxNB,1,0,2,1);
	label = gtk_label_new(strings[I_I2C_NB]);	//"Byes to read/write"
	gtk_container_add(GTK_CONTAINER(i2cHboxNB),label);
	I2CNbyte = 	gtk_spin_button_new_with_range(0,64,1);
	gtk_container_add(GTK_CONTAINER(i2cHboxNB),GTK_WIDGET(I2CNbyte));
	GtkWidget * i2cHboxSpeed = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_grid_attach(GTK_GRID(i2cGrid),i2cHboxSpeed,1,1,2,1);
	label = gtk_label_new(strings[I_Speed]);	//"Speed"
	gtk_container_add(GTK_CONTAINER(i2cHboxSpeed),label);
	I2CSpeed = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(I2CSpeed),"100 kbps");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(I2CSpeed),"200 kbps");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(I2CSpeed),"300/400 kbps");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(I2CSpeed),"500/800 kbps");
	gtk_combo_box_set_active(GTK_COMBO_BOX(I2CSpeed),0);
	gtk_container_add(GTK_CONTAINER(i2cHboxSpeed),GTK_WIDGET(I2CSpeed));
	I2CSendBtn = gtk_button_new_with_label(strings[I_I2CSend]);	//"Send"
	gtk_grid_attach(GTK_GRID(i2cGrid),I2CSendBtn,1,2,1,1);
	I2CReceiveBtn = gtk_button_new_with_label(strings[I_I2CReceive]);	//"Receive"
	gtk_grid_attach(GTK_GRID(i2cGrid),I2CReceiveBtn,2,2,1,1);
//------ICD tab-------------
	label = gtk_label_new("ICD");
	icdVbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),icdVbox1,label);
	//menu
	GtkWidget * icdMenuBar = gtk_menu_bar_new ();
	GtkWidget * icdRootMenu = gtk_menu_item_new_with_label(strings[I_Opt]); //"Options";
    gtk_menu_shell_append (GTK_MENU_SHELL (icdMenuBar), icdRootMenu);
	GtkWidget * icdMenu = gtk_menu_new();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (icdRootMenu), icdMenu);
	icdMenuPC = gtk_check_menu_item_new_with_label(strings[I_SHOW_PC]); //"show Program Counter"
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(icdMenuPC),TRUE);
	icdMenuSTAT = gtk_check_menu_item_new_with_label(strings[I_SHOW_STATUS]); //"show status registers"
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(icdMenuSTAT),TRUE);
	icdMenuBank0 = gtk_check_menu_item_new_with_label(strings[I_SHOW_BANK0]); //"show memory bank 0"
	icdMenuBank1 = gtk_check_menu_item_new_with_label(strings[I_SHOW_BANK1]); //"show memory bank 1"
	icdMenuBank2 = gtk_check_menu_item_new_with_label(strings[I_SHOW_BANK2]); //"show memory bank 2"
	icdMenuBank3 = gtk_check_menu_item_new_with_label(strings[I_SHOW_BANK3]); //"show memory bank 3"
	icdMenuEE = gtk_check_menu_item_new_with_label(strings[I_SHOW_EE]); //"show EEPROM"
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdMenuPC);
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdMenuSTAT);
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdMenuBank0);
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdMenuBank1);
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdMenuBank2);
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdMenuBank3);
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdMenuEE);
	//toolbar
	GtkWidget* iconGO = gtk_image_new_from_resource("/openprog/icons/go.png");
	GtkToolItem* btnICDGo = gtk_tool_button_new(iconGO, strings[I_ICD_RUN]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnICDGo), strings[I_ICD_RUN]);
	g_signal_connect(G_OBJECT(btnICDGo), "clicked", G_CALLBACK(icdRun),NULL);

	GtkWidget* iconHALT = gtk_image_new_from_resource("/openprog/icons/halt.png");
	GtkToolItem* btnICDHalt = gtk_tool_button_new(iconHALT, strings[I_ICD_HALT]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnICDHalt), strings[I_ICD_HALT]);
	g_signal_connect(G_OBJECT(btnICDHalt), "clicked", G_CALLBACK(icdHalt),NULL);

	GtkWidget* iconSTEP = gtk_image_new_from_resource("/openprog/icons/step.png");
	GtkToolItem* btnICDStep = gtk_tool_button_new(iconSTEP, strings[I_ICD_STEP]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnICDStep), strings[I_ICD_STEP]);
	g_signal_connect(G_OBJECT(btnICDStep), "clicked", G_CALLBACK(icdStep),NULL);

	GtkWidget* iconSTEPOVER = gtk_image_new_from_resource("/openprog/icons/stepover.png");
	GtkToolItem* btnICDStepover = gtk_tool_button_new(iconSTEPOVER, strings[I_ICD_STEPOVER]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnICDStepover), strings[I_ICD_STEPOVER]);
	g_signal_connect(G_OBJECT(btnICDStepover), "clicked", G_CALLBACK(icdStepOver),NULL);

	GtkWidget* iconSTOP = gtk_image_new_from_resource("/openprog/icons/stop.png");
	GtkToolItem* btnICDStop = gtk_tool_button_new(iconSTOP, strings[I_ICD_STOP]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnICDStop), strings[I_ICD_STOP]);
	g_signal_connect(G_OBJECT(btnICDStop), "clicked", G_CALLBACK(icdStop),NULL);

	GtkWidget* iconREFRESH = gtk_image_new_from_icon_name("view-refresh", GTK_ICON_SIZE_BUTTON);
	GtkToolItem* btnICDRefresh = gtk_tool_button_new(iconREFRESH, strings[I_ICD_REFRESH]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnICDRefresh), strings[I_ICD_REFRESH]);
	g_signal_connect(G_OBJECT(btnICDRefresh), "clicked", G_CALLBACK(icdRefresh),NULL);

	GtkToolItem* btnICDLoadCOFF = gtk_tool_button_new(NULL, strings[I_LOAD_COFF]); //"load COFF file..."
	g_signal_connect(G_OBJECT(btnICDLoadCOFF), "clicked", G_CALLBACK(loadCoff),window);

	GtkToolItem* itemICDCommand = gtk_tool_item_new();
	icdCommand = gtk_entry_new();
	gtk_widget_set_tooltip_text(icdCommand, strings[I_ICD_CMD]);
	gtk_container_add(GTK_CONTAINER(itemICDCommand), icdCommand);

	GtkWidget* iconHELP = gtk_image_new_from_icon_name("system-help", GTK_ICON_SIZE_BUTTON);
	GtkToolItem* btnICDHelp = gtk_tool_button_new(iconHELP, strings[I_ICD_HELP]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btnICDHelp), strings[I_ICD_HELP]);
	g_signal_connect(G_OBJECT(btnICDHelp), "clicked", G_CALLBACK(ICDHelp),NULL);

	GtkWidget * icdtoolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(icdtoolbar),GTK_TOOLBAR_ICONS);
	gtk_box_pack_start(GTK_BOX(icdVbox1),icdtoolbar,FALSE,FALSE,0);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),btnICDGo,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),btnICDHalt,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),btnICDStep,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),btnICDStepover,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),gtk_separator_tool_item_new(),-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),btnICDStop,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),gtk_separator_tool_item_new(),-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),btnICDRefresh,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),gtk_separator_tool_item_new(),-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),btnICDLoadCOFF,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),gtk_separator_tool_item_new(),-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),itemICDCommand,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),gtk_separator_tool_item_new(),-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdtoolbar),btnICDHelp,-1);

	GtkWidget *hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(icdVbox1),hpaned,TRUE,TRUE,0);
	gint width,height;
	gtk_window_get_size(GTK_WINDOW(window),&width,&height);
	gtk_paned_set_position(GTK_PANED (hpaned),width/2);
	//source
	GtkWidget * icdVbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_paned_pack1 (GTK_PANED (hpaned), icdVbox2, TRUE, FALSE);
	label = gtk_label_new(strings[I_ICD_SOURCE]);	//"Source"
	gtk_box_pack_start(GTK_BOX(icdVbox2),label,FALSE,FALSE,0);
	GtkWidget * sourceScroll = gtk_scrolled_window_new(NULL,NULL);
	sourceTxt = gtk_text_view_new();
	sourceBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sourceTxt));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(sourceTxt),FALSE);
	gtk_container_add(GTK_CONTAINER(sourceScroll),sourceTxt);

	styleCtx = gtk_widget_get_style_context(GTK_WIDGET(sourceTxt));
	gtk_style_context_add_class(styleCtx, "mono");

	gtk_box_pack_start(GTK_BOX(icdVbox2),sourceScroll,TRUE,TRUE,0);
	//status
	GtkWidget * icdVbox3 = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_paned_pack2 (GTK_PANED (hpaned), icdVbox3, TRUE, FALSE);
	GtkWidget * icdHbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,20);
	gtk_box_set_homogeneous(GTK_BOX(icdHbox2), TRUE);
	gtk_box_pack_start(GTK_BOX(icdVbox3),icdHbox2,FALSE,FALSE,0);
	label = gtk_label_new(strings[I_ICD_STATUS]);	//"Status"
	gtk_box_pack_start(GTK_BOX(icdHbox2),label,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(icdHbox2),icdMenuBar,FALSE,FALSE,0);
	GtkWidget * statusScroll = gtk_scrolled_window_new(NULL,NULL);
	statusTxt = gtk_text_view_new();
	statusBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(statusTxt));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(statusTxt),FALSE);
	gtk_container_add(GTK_CONTAINER(statusScroll),statusTxt);

	styleCtx = gtk_widget_get_style_context(GTK_WIDGET(statusTxt));
	gtk_style_context_add_class(styleCtx, "mono");

	gtk_box_pack_start(GTK_BOX(icdVbox3),statusScroll,TRUE,TRUE,0);
//------IO tab-------------
	label = gtk_label_new("I/O");
	GtkWidget * ioVbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),ioVbox1,label);
	GtkWidget * ioFrameIO = gtk_frame_new("I/O");
	gtk_box_pack_start(GTK_BOX(ioVbox1),ioFrameIO,FALSE,FALSE,0);
	GtkWidget * ioGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(ioGrid),20);

	gtk_container_add(GTK_CONTAINER(ioFrameIO),GTK_WIDGET(ioGrid));
	b_io_active = gtk_check_button_new_with_label(strings[I_IO_Enable]);	//"Enable IO"
	gtk_grid_attach(GTK_GRID(ioGrid),b_io_active,0,0,1,1);
	int ii;
	for(ii=0;ii<=7;ii++){
		char ss[16];
		sprintf(ss,"RB%d",ii);
		ioButtons[ii].name=strdup(ss);
		ioButtons[ii].x=0;
		ioButtons[ii].y=8-ii;
	}
	ioButtons[8].name="RC7";
	ioButtons[8].x=1;
	ioButtons[8].y=1;
	ioButtons[9].name="RC6";
	ioButtons[9].x=1;
	ioButtons[9].y=2;
	ioButtons[10].name="RA5";
	ioButtons[10].x=1;
	ioButtons[10].y=3;
	ioButtons[11].name="RA4";
	ioButtons[11].x=1;
	ioButtons[11].y=4;
	ioButtons[12].name="RA3";
	ioButtons[12].x=1;
	ioButtons[12].y=5;
	for(ii=0;ii<sizeof(ioButtons)/sizeof(ioButtons[0]);ii++){
		GtkWidget * ioBoxRBx = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
		gtk_grid_attach(GTK_GRID(ioGrid),ioBoxRBx,ioButtons[ii].x,ioButtons[ii].y,1,1);
		label = gtk_label_new(ioButtons[ii].name);
		gtk_box_pack_start(GTK_BOX(ioBoxRBx),label,FALSE,TRUE,0);
		ioButtons[ii].r_0 = gtk_radio_button_new_with_label(NULL,"0");
		ioButtons[ii].r_1 = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(ioButtons[ii].r_0)),"1");
		ioButtons[ii].r_I = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(ioButtons[ii].r_0)),"INPUT:");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ioButtons[ii].r_I),TRUE);
		gtk_box_pack_start(GTK_BOX(ioBoxRBx),ioButtons[ii].r_0,FALSE,TRUE,0);
		gtk_box_pack_start(GTK_BOX(ioBoxRBx),ioButtons[ii].r_1,FALSE,TRUE,0);
		gtk_box_pack_start(GTK_BOX(ioBoxRBx),ioButtons[ii].r_I,FALSE,TRUE,0);
		ioButtons[ii].e_I = gtk_entry_new();
		gtk_entry_set_width_chars(GTK_ENTRY(ioButtons[ii].e_I),1);
		gtk_editable_set_editable(GTK_EDITABLE(ioButtons[ii].e_I),FALSE);
		gtk_entry_set_has_frame(GTK_ENTRY(ioButtons[ii].e_I),FALSE);
		gtk_box_pack_start(GTK_BOX(ioBoxRBx),ioButtons[ii].e_I,FALSE,TRUE,0);
		g_signal_connect(G_OBJECT(ioButtons[ii].r_0),"toggled",G_CALLBACK(IOchanged),NULL);
		g_signal_connect(G_OBJECT(ioButtons[ii].r_1),"toggled",G_CALLBACK(IOchanged),NULL);
		g_signal_connect(G_OBJECT(ioButtons[ii].r_I),"toggled",G_CALLBACK(IOchanged),NULL);
	}
	GtkWidget * ioBoxDCDC = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_box_pack_start(GTK_BOX(ioVbox1),ioBoxDCDC,FALSE,FALSE,0);
	VDD_ON = gtk_check_button_new_with_label("VDDU");	//""
	gtk_box_pack_start(GTK_BOX(ioBoxDCDC),VDD_ON,FALSE,TRUE,2);
	VPP_ON = gtk_check_button_new_with_label("VPPU");	//""
	gtk_box_pack_start(GTK_BOX(ioBoxDCDC),VPP_ON,FALSE,TRUE,2);
	DCDC_ON = gtk_check_button_new_with_label("DCDC");	//""
	gtk_box_pack_start(GTK_BOX(ioBoxDCDC),DCDC_ON,FALSE,TRUE,2);
	DCDC_voltage=gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,5,15,0.1);
	gtk_box_pack_start(GTK_BOX(ioBoxDCDC),DCDC_voltage,TRUE,TRUE,2);
//------Utility tab-------------
	GtkWidget * utVbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),utVbox1,gtk_label_new("Utility"));
	GtkWidget * utFrameH2D = gtk_frame_new("HEX -> DATA");
	gtk_box_pack_start(GTK_BOX(utVbox1),utFrameH2D,FALSE,FALSE,5);
	GtkWidget * utVboxHex = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_container_add(GTK_CONTAINER(utFrameH2D),utVboxHex);
	GtkWidget * utHboxHex = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_container_add(GTK_CONTAINER(utVboxHex),utHboxHex);
	gtk_box_pack_start(GTK_BOX(utHboxHex),gtk_label_new("Hex"),FALSE,TRUE,5);
	Hex_entry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(utHboxHex),Hex_entry,TRUE,TRUE,5);
	Hex_data = gtk_entry_new();
	gtk_editable_set_editable(GTK_EDITABLE(Hex_data),0);
	gtk_box_pack_start(GTK_BOX(utVboxHex),Hex_data,TRUE,TRUE,5);
	GtkWidget * utFrameD2H = gtk_frame_new("DATA -> HEX");
	gtk_box_pack_start(GTK_BOX(utVbox1),utFrameD2H,FALSE,FALSE,5);
	GtkWidget * utGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(utGrid),5);
	gtk_grid_set_row_spacing(GTK_GRID(utGrid),5);
	gtk_container_add(GTK_CONTAINER(utFrameD2H),utGrid);
	gtk_grid_attach(GTK_GRID(utGrid),gtk_label_new(strings[I_ADDR]),0,0,1,1); //"Address"
	Address_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(utGrid),Address_entry,1,0,1,1);
	gtk_grid_attach(GTK_GRID(utGrid),gtk_label_new(strings[I_Data]),0,1,1,1); //Data
	Data_entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(utGrid),Data_entry,1,1,1,1);
	gtk_grid_attach(GTK_GRID(utGrid),gtk_label_new("Hex"),0,2,1,1);
	Hex_data2 = gtk_entry_new();
	gtk_editable_set_editable(GTK_EDITABLE(Hex_data2),0);
	gtk_grid_attach(GTK_GRID(utGrid),Hex_data2,1,2,1,1);
	GtkWidget * b_hexsave = gtk_button_new_with_label(strings[I_Fsave]);
	gtk_grid_attach(GTK_GRID(utGrid),b_hexsave,0,3,1,1);
//------status bar-------------
	status_bar = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(vbox),status_bar,FALSE,TRUE,0);
	statusID=gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar),"ID");
	g_signal_connect(G_OBJECT(b_testhw),"clicked",G_CALLBACK(TestHw),window);
	g_signal_connect(G_OBJECT(b_connect),"clicked",G_CALLBACK(Connect),window);
	g_signal_connect(G_OBJECT(I2CReceiveBtn),"clicked",G_CALLBACK(I2cspiR),window);
	g_signal_connect(G_OBJECT(I2CSendBtn),"clicked",G_CALLBACK(I2cspiS),window);
	g_signal_connect(G_OBJECT(sourceTxt),"button_press_event",G_CALLBACK(source_mouse_event),NULL);
	g_signal_connect(G_OBJECT(statusTxt),"button_press_event",G_CALLBACK(icdStatus_mouse_event),NULL);
	g_signal_connect(G_OBJECT(icdCommand),"key_press_event",G_CALLBACK(icdCommand_key_event),NULL);
	g_signal_connect(G_OBJECT(icdVbox1),"key_press_event",G_CALLBACK(icd_key_event),NULL);
	g_signal_connect(G_OBJECT(devTypeCombo),"changed",G_CALLBACK(FilterDevType),NULL);
	g_signal_connect(G_OBJECT(devCombo),"changed",G_CALLBACK(DeviceChanged),NULL);
	g_signal_connect(G_OBJECT(b_io_active),"toggled",G_CALLBACK(IOactive),NULL);
	g_signal_connect(G_OBJECT(VDD_ON),"toggled",G_CALLBACK(VPPVDDactive),NULL);
	g_signal_connect(G_OBJECT(VPP_ON),"toggled",G_CALLBACK(VPPVDDactive),NULL);
	g_signal_connect(G_OBJECT(DCDC_ON),"toggled",G_CALLBACK(DCDCactive),NULL);
	g_signal_connect(G_OBJECT(DCDC_voltage),"value_changed",G_CALLBACK(DCDCactive),NULL);
	g_signal_connect(G_OBJECT(Hex_entry),"changed",G_CALLBACK(HexConvert),NULL);
	g_signal_connect(G_OBJECT(Address_entry),"changed",G_CALLBACK(DataToHexConvert),NULL);
	g_signal_connect(G_OBJECT(Data_entry),"changed",G_CALLBACK(DataToHexConvert),NULL);
	g_signal_connect(G_OBJECT(b_hexsave),"clicked",G_CALLBACK(HexSave),window);
	g_signal_connect(G_OBJECT(b_WfuseLF),"clicked",G_CALLBACK(WriteATfuseLowLF),window);
	gtk_widget_show_all(window);
//********Init*************
	char text[16];
	sprintf(text,"%04X",vid);
	gtk_entry_set_text(GTK_ENTRY(VID_entry),text);
	sprintf(text,"%04X",pid);
	gtk_entry_set_text(GTK_ENTRY(PID_entry),text);
	sprintf(text,"%d",max_err);
	gtk_entry_set_text(GTK_ENTRY(Errors_entry),text);
	sizeW=0x8400;
	memCODE_W=malloc(sizeW*sizeof(WORD));
	initVar();
	for(i=0;i<0x8400;i++) memCODE_W[i]=0x3fff;
	strncpy(LogFileName,strings[S_LogFile],sizeof(LogFileName));
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devTypeCombo),"*");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devTypeCombo),"PIC10/12");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devTypeCombo),"PIC16");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devTypeCombo),"PIC18");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devTypeCombo),"PIC24");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devTypeCombo),"PIC30/33");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devTypeCombo),"ATMEL AVR");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devTypeCombo),"EEPROM");
	gtk_combo_box_set_wrap_width(GTK_COMBO_BOX(devCombo),6);
	int tt=0;
	if(!strncmp(dev,"10F",3)||!strncmp(dev,"12F",3)) tt=1;	//10F 12F
	else if(!strncmp(dev,"16F",3)) tt=2;	//16F
	else if(!strncmp(dev,"18F",3)) tt=3;	//18F
	else if(!strncmp(dev,"24F",3)||!strncmp(dev,"24H",3)||!strncmp(dev,"24E",3)) tt=4;	//24F
	else if(!strncmp(dev,"30F",3)||!strncmp(dev,"33F",3)||!strncmp(dev,"33E",3)) tt=5;	//30/33
	else if(!strncmp(dev,"AT",2)) tt=6;	//AVR
	else if((strncmp(dev,"24F",3)&&strncmp(dev,"24H",3)&&strncmp(dev,"24E",3))&&\
				(!strncmp(dev,"24",2)||!strncmp(dev,"25",2)||!strncmp(dev,"93",2)|| \
				 !strncmp(dev,"95",2)||!strncmp(dev,"11",2)||!strncmp(dev,"DS",2))) \
				tt=7;	//EEPROM
	gtk_combo_box_set_active(GTK_COMBO_BOX(devTypeCombo),tt);
//	AddDevices();	//populate device list
	DeviceDetected=FindDevice(vid,pid);	//connect to USB programmer
	if(!DeviceDetected){
		DeviceDetected=FindDevice(new_vid,new_pid);	//try default
		if(DeviceDetected){
			vid=new_vid;
			pid=new_pid;
		}
	}
	if(!DeviceDetected) DeviceDetected=FindDevice(old_vid,old_pid); //try old one
	ProgID();		//get firmware version and reset
	gtk_main();
//	printf(ListDevices());
//******Save ini file******
// only if parameters are changed
	if(strcmp(dev_ini,dev)||vid_ini!=vid||pid_ini!=pid||max_err_ini!=max_err){
		if(homedir){
			f=fopen(fname,"w");
			if(f){
				fprintf(f,"device %s\n",dev);
				fprintf(f,"maxerr %d\n",max_err);
				fprintf(f,"vid %X\n",vid);
				fprintf(f,"pid %X\n",pid);
			}
			fclose(f);
		}
	}
	return 0;
}
///
/// Show a message box
void MsgBox(const char* msg)
{
	GtkWidget * dialog = gtk_message_dialog_new (GTK_WINDOW(window),
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_INFO,
                                 GTK_BUTTONS_OK,
                                 msg);
    gtk_window_set_title(GTK_WINDOW(dialog)," ");
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}
///
/// Find the programmer and setup communication
void Connect(GtkWidget *widget,GtkWidget *window){
	vid=htoi(gtk_entry_get_text(GTK_ENTRY(VID_entry)),4);
	pid=htoi(gtk_entry_get_text(GTK_ENTRY(PID_entry)),4);
	DeviceDetected=FindDevice(vid,pid);	//connect to USB programmer
	if(!DeviceDetected){
		DeviceDetected=FindDevice(new_vid,new_pid);	//try default
		if(DeviceDetected){
			vid=new_vid;
			pid=new_pid;
		}
	}
	if(!DeviceDetected) DeviceDetected=FindDevice(old_vid,old_pid); //try old one
	hvreg=0;
	ProgID();
}
///
/// I2C/SPI receive
void I2cspiR()
{
	//if(DeviceDetected!=1) return;
	gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,"");
	saveLog = (int) gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b_log));
	int nbyte=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(I2CNbyte));
	if(nbyte<0) nbyte=0;
	if(nbyte>60) nbyte=60;
	int mode=0;
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(I2C16bit))) mode=1;	//I2C mode
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(SPI00))) mode=2;	//SPI mode 00
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(SPI01))) mode=3;	//SPI mode 01
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(SPI10))) mode=4;	//SPI mode 10
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(SPI11))) mode=5;	//SPI mode 11
	char* tok;
    char tokbuf[512];
	BYTE tmpbuf[128];
	int i=0,x;
    strncpy(tokbuf, (const char *)gtk_entry_get_text(GTK_ENTRY(I2CDataSend)), sizeof(tokbuf));
	for(tok=strtok(tokbuf," ");tok&&i<128;tok=strtok(NULL," ")){
		if(sscanf(tok,"%x",&x)){
			tmpbuf[i] = (BYTE)x;
			i++;
		}
	}
	for(;i<128;i++) tmpbuf[i]=0;
	I2CReceive(mode,gtk_combo_box_get_active(GTK_COMBO_BOX(I2CSpeed)),nbyte,tmpbuf);
}
///
/// I2C/SPI send
void I2cspiS()
{
	//if(DeviceDetected!=1) return;
	gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,"");
	saveLog = (int) gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b_log));
	int nbyte=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(I2CNbyte));
	if(nbyte<0) nbyte=0;
	if(nbyte>57) nbyte=57;
	int mode=0;
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(I2C16bit))) mode=1;	//I2C mode
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(SPI00))) mode=2;	//SPI mode 00
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(SPI01))) mode=3;	//SPI mode 01
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(SPI10))) mode=4;	//SPI mode 10
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(SPI11))) mode=5;	//SPI mode 11
	char* tok;
    char tokbuf[512];
	BYTE tmpbuf[128];
	int i=0,x;
    strncpy(tokbuf, (const char *)gtk_entry_get_text(GTK_ENTRY(I2CDataSend)), sizeof(tokbuf));
	for(tok=strtok(tokbuf," ");tok&&i<128;tok=strtok(NULL," ")){
		if(sscanf(tok,"%x",&x)){
			tmpbuf[i] = (BYTE)x;
			i++;
		}
	}
	for(;i<128;i++) tmpbuf[i]=0;
	I2CSend(mode,gtk_combo_box_get_active(GTK_COMBO_BOX(I2CSpeed)),nbyte,tmpbuf);
}
///
///Display contents of EEprom memory
void DisplayEE(){
	char s[256],t[256],v[256],*aux,*g;
	int valid=0,empty=1,lines=0;
	int i,j,max;
	s[0]=0;
	v[0]=0;
	aux=malloc((sizeEE/COL+1)*(16+COL*5));
	aux[0]=0;
	PrintMessage(strings[S_EEMem]);	//"\r\nEEPROM memory:\r\n"
	max=sizeEE>7000?7000:sizeEE;
	for(i=0;i<max;i+=COL){
		valid=0;
		for(j=i;j<i+COL&&j<sizeEE;j++){
			sprintf(t,"%02X ",memEE[j]);
			strcat(s,t);
			sprintf(t,"%c",isprint(memEE[j])&&(memEE[j]<0xFF)?memEE[j]:'.');
			g=g_locale_to_utf8(t,-1,NULL,NULL,NULL);
			if(g) strcat(v,g);
			g_free(g);
			if(memEE[j]<0xff) valid=1;
		}
		if(valid){
			sprintf(t,"%04X: %s %s\r\n",i,s,v);
			strcat(aux,t);
			empty=0;
			lines++;
			if(lines>500){	//limit number of lines printed
				strcat(aux,"(...)\r\n");
				i=max-COL*2;
				lines=490;
			}
		}
		s[0]=0;
		v[0]=0;
	}
	if(empty) PrintMessage(strings[S_Empty]);	//empty
	else{
		PrintMessage(aux);
		if(sizeEE>max) PrintMessage("(...)\r\n");
	}
	free(aux);
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
void TestHw(GtkWidget *widget,GtkWindow* parent)
{
#ifndef DEBUG
	if(DeviceDetected!=1) return;
#endif
	char str[256];
	StartHVReg(13);
	int j=0;
	MsgBox(strings[I_TestHW]);		//"Test hardware ..."
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	strcpy(str,strings[I_TestMSG]);
	strcat(str,"\n VDDU=5V\n VPPU=13V\n PGD(RB5)=0V\n PGC(RB6)=0V\n PGM(RB7)=0V");
	MsgBox(str);
	j=0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x15;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;			//VDD
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	strcpy(str,strings[I_TestMSG]);
	strcat(str,"\n VDDU=5V\n VPPU=0V\n PGD(RB5)=5V\n PGC(RB6)=5V\n PGM(RB7)=5V");
	MsgBox(str);
	j=0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x4;			//VPP
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	strcpy(str,strings[I_TestMSG]);
	strcat(str,"\n VDDU=0V\n VPPU=13V\n PGD(RB5)=5V\n PGC(RB6)=0V\n PGM(RB7)=0V");
	MsgBox(str);
	j=0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x4;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	strcpy(str,strings[I_TestMSG]);
	strcat(str,"\n VDDU=0V\n VPPU=0V\n PGD(RB5)=0V\n PGC(RB6)=5V\n PGM(RB7)=0V");
	MsgBox(str);
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
/*	if(info){
		PrintMessage3("Device detected: vid=0x%04X pid=0x%04X\nPath: %s\n",vid,pid,MyDevicePathName);
		if(HidD_GetManufacturerString(DeviceHandle,string,sizeof(string))==TRUE) wprintf(L"Manufacturer string: %s\n",string);
		if(HidD_GetProductString(DeviceHandle,string,sizeof(string))==TRUE) wprintf(L"Product string: %s\n",string);
	}*/
#endif
	if (MyDeviceDetected == FALSE){
		PrintMessage(strings[S_noprog]);	//"Programmer not detected\r\n"
		//gtk_statusbar_push(status_bar,statusID,strings[S_noprog]);
	}
	else{
		PrintMessage(strings[S_prog]);	//"Programmer detected\r\n");
		PrintMessage2("VID=0x%04X PID=0x%04X\r\n",vid,pid);
		//gtk_statusbar_push(status_bar,statusID,strings[S_prog]);
	}
	return MyDeviceDetected;
}

void PrintStatus(char *s,  uint16_t p1, uint16_t p2) {
	sprintf(str,s,p1,p2);
	gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,str);
	while (gtk_events_pending ()) gtk_main_iteration();
}

void PrintStatusSetup() {}
void PrintStatusEnd() {}

void PrintStatusClear() {
	gtk_statusbar_push(GTK_STATUSBAR(status_bar), statusID,"");
}

void StrcatConvert(char *dst, const char *src) {
	char *g = g_locale_to_utf8(src,-1,NULL,NULL,NULL);
	if(g) strcat(dst,g);
	g_free(g);
}

//Add all devices to the appropriate structure
void AddDevices() {
	int i;
	for(i=0;i<Ndevices;i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(devCombo),devices[i]);
}
