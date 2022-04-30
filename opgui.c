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
GtkApplication *app;
GtkTextBuffer * dataBuf;
GtkWidget *data, *data_scroll;
GtkWidget *window, *toolbar, *button, *notebook, *status_bar;
GtkToolItem *openToolItem, *saveToolItem, *readToolItem, *writeToolItem, *connectToolItem, *stopToolItem, *infoToolItem;
GtkWidget *devCombo, *devTypeCombo, *picOptsBox, *avrOptsBox, *configWOptsBox, *icdOptsBox, *oscOptsBox, *devInfoLabel;
GtkWidget *icdCheckToggle, *icdAddrEntry;
GtkWidget *eepromRWToggle, *readReservedToggle, *writeIDBKCalToggle, *writeCalib12Toggle, *useOSCCALToggle, *useBKOSCCALToggle, *useFileCalToggle;
GtkWidget *avrFuseLowEntry, *avrFuseLowWriteToggle, *avrFuseHighEntry, *avrFuseHighWriteToggle, *avrFuseExtEntry, *avrFuseExtWriteToggle, *avrLockEntry, *avrLockWriteToggle;
GtkWidget *wFuseLFBtn, *connectBtn, *testHWBtn, *logBtn;
GtkWidget *vidEntry, *pidEntry, *errorsEntry;
GtkWidget *I2C8bit, *I2C16bit, *SPI00, *SPI01, *SPI10, *SPI11;
GtkWidget *i2cDataSendEntry, *i2cDataRcvEntry, *i2cSendBtn, *i2cRcvBtn, *i2cNbyteSpinBtn, *i2cSpeedCombo;
GtkWidget *statusTextView, *sourceTextView;
GtkTextBuffer *statusBuf, *sourceBuf;
GtkWidget *icdVBox, *icdPC_CheckMenuItem, *icdSTAT_CheckMenuItem, *icdBank0_CheckMenuItem, *icdBank1_CheckMenuItem, *icdBank2_CheckMenuItem, *icdBank3_CheckMenuItem, *icdEE_CheckMenuItem, *icdCommandEntry;
GtkWidget *dcdcOnToggle, *dcdcVoltageRange, *vppOnToggle, *vddOnToggle;
GtkWidget *ioActiveToggle, *V33CheckToggle, *waitS1Toggle;
GtkWidget *hexEntry, *addressEntry, *dataEntry, *hexDataEntry, *hexDataEntry2;
GtkWidget *CW1Entry, *CW2Entry, *CW3Entry, *CW4Entry, *CW5Entry, *CW6Entry, *CW7Entry;
GtkWidget *CW1Box, *CW2Box, *CW3Box, *CW4Box, *CW5Box, *CW6Box, *CW7Box;
GtkWidget *configForceToggle;
GtkStyleContext *styleCtx;
GtkListStore *devStore;
GtkWidget *devTree;
GtkTreeIter iter;

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

gchar *homedir, *fname;
char dev_ini[64];
int vid_ini,pid_ini,max_err_ini;

char dev[64]="";
int devType=-1;
char str[4096]="";
char* cur_path=0;
char* cur_pathEE=0;

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
	GtkTextBuffer * dataBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(i2cDataRcvEntry));
	gtk_text_buffer_set_text(dataBuf,msg,-1);
	gtk_text_buffer_get_end_iter(dataBuf,&iter);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(i2cDataRcvEntry),&iter,0.0,FALSE,0,0);
	while (gtk_events_pending ()) gtk_main_iteration();
}
///
///Print a message on the ICD data field
void PrintMessageICD(const char *msg){
	GtkTextIter iter;
	gtk_text_buffer_set_text(statusBuf,msg,-1);
	gtk_text_buffer_get_start_iter(statusBuf,&iter);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(statusTextView),&iter,0.0,FALSE,0,0);
	while (gtk_events_pending ()) gtk_main_iteration();
}
///
///Append a message on the ICD data field
void AppendMessageICD(const char *msg){
	GtkTextIter iter;
	gtk_text_buffer_get_end_iter(statusBuf,&iter);
	gtk_text_buffer_insert(statusBuf,&iter,msg,-1);
	gtk_text_buffer_get_start_iter(statusBuf,&iter);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(statusTextView),&iter,0.0,FALSE,0,0);
	while (gtk_events_pending ()) gtk_main_iteration();
}
///
///Update option variables according to actual control values
void getOptions()
{
	vid=htoi(gtk_entry_get_text(GTK_ENTRY(vidEntry)),4);
	pid=htoi(gtk_entry_get_text(GTK_ENTRY(pidEntry)),4);
	saveLog = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(logBtn));
	ee = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(eepromRWToggle))?0xFFFF:0;
	programID = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(writeIDBKCalToggle));
	max_err=atoi(gtk_entry_get_text(GTK_ENTRY(errorsEntry)));
	load_calibword= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(writeCalib12Toggle));
	load_osccal= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(useOSCCALToggle));
	load_BKosccal= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(useBKOSCCALToggle));
	ICDenable= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(icdCheckToggle));
	readRes= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(readReservedToggle));
	skipV33check=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(V33CheckToggle));
	waitS1=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(waitS1Toggle));
	int i=sscanf(gtk_entry_get_text(GTK_ENTRY(icdAddrEntry)),"%x",&ICDaddr);
	if(i!=1||ICDaddr<0||ICDaddr>0xFFFF) ICDaddr=0x1FF0;
	char *str=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(devCombo));
	if(str) strncpy(dev,str,sizeof(dev)-1);
	g_free(str);
	AVRfuse=AVRfuse_h=AVRfuse_x=AVRlock=0x100;
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(avrFuseLowWriteToggle))){
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(avrFuseLowEntry)),"%x",&AVRfuse);
		if(i!=1||AVRfuse<0||AVRfuse>0xFF) AVRfuse=0x100;
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(avrFuseHighWriteToggle))){
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(avrFuseHighEntry)),"%x",&AVRfuse_h);
		if(i!=1||AVRfuse_h<0||AVRfuse_h>0xFF) AVRfuse_h=0x100;
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(avrFuseExtWriteToggle))){
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(avrFuseExtEntry)),"%x",&AVRfuse_x);
		if(i!=1||AVRfuse_x<0||AVRfuse_x>0xFF) AVRfuse_x=0x100;
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(avrLockWriteToggle))){
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(avrLockEntry)),"%x",&AVRlock);
		if(i!=1||AVRlock<0||AVRlock>0xFF) AVRlock=0x100;
	}
	str=malloc(128);
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configForceToggle))){
		int cw1,cw2,cw3,cw4,cw5,cw6,cw7;
		cw1=cw2=cw3=cw4=cw5=cw6=cw7=0x10000;
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW1Entry)),"%x",&cw1);
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW2Entry)),"%x",&cw2);
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW3Entry)),"%x",&cw3);
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW4Entry)),"%x",&cw4);
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW5Entry)),"%x",&cw5);
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW6Entry)),"%x",&cw6);
		i=sscanf(gtk_entry_get_text(GTK_ENTRY(CW7Entry)),"%x",&cw7);
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
			gtk_widget_set_sensitive(GTK_WIDGET(stopToolItem),TRUE);
			progress=1;
			Write(dev,ee);	//choose the right function
			progress=0;
			gtk_widget_set_sensitive(GTK_WIDGET(stopToolItem),FALSE);
		}
		waitingS1=0;
	}
	else if(waitingS1) waitingS1=0;
	else if(!progress){
		gtk_widget_set_sensitive(GTK_WIDGET(stopToolItem),TRUE);
		progress=1;
		Write(dev,ee);	//choose the right function
		progress=0;
		gtk_widget_set_sensitive(GTK_WIDGET(stopToolItem),FALSE);
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
			gtk_widget_set_sensitive(GTK_WIDGET(stopToolItem),TRUE);
			progress=1;
			Read(dev,ee,readRes);	//choose the right function
			progress=0;
			gtk_widget_set_sensitive(GTK_WIDGET(stopToolItem),FALSE);
		}
		waitingS1=0;
	}
	else if(waitingS1) waitingS1=0;
	else if(!progress){
		gtk_widget_set_sensitive(GTK_WIDGET(stopToolItem),TRUE);
		progress=1;
		Read(dev,ee,readRes);	//choose the right function
		progress=0;
		gtk_widget_set_sensitive(GTK_WIDGET(stopToolItem),FALSE);
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
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(avrFuseLowWriteToggle))){
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
	gtk_label_set_text(GTK_LABEL(devInfoLabel),info.features);
	if(devType==PIC12||devType==PIC16||devType==PIC18||devType==PIC24){
		gtk_widget_show_all(GTK_WIDGET(picOptsBox));
		gtk_widget_hide(GTK_WIDGET(avrOptsBox));
		gtk_widget_show_all(GTK_WIDGET(eepromRWToggle));
	}
	else if(devType==AVR){	//ATMEL
		gtk_widget_hide(GTK_WIDGET(picOptsBox));
		gtk_widget_show_all(GTK_WIDGET(avrOptsBox));
		gtk_widget_show_all(GTK_WIDGET(eepromRWToggle));
	}
	else{
		gtk_widget_hide(GTK_WIDGET(picOptsBox));
		gtk_widget_hide(GTK_WIDGET(avrOptsBox));
		gtk_widget_hide(GTK_WIDGET(eepromRWToggle));
	}
	if(devType==PIC16)		//ICD
		gtk_widget_show_all(GTK_WIDGET(icdOptsBox));
	else gtk_widget_hide(GTK_WIDGET(icdOptsBox));
	if(devType==PIC12||devType==PIC16)	//Osc options
		gtk_widget_show_all(GTK_WIDGET(oscOptsBox));
	else gtk_widget_hide(GTK_WIDGET(oscOptsBox));
	if(devType==PIC12||devType==PIC16||devType==PIC18)	//program ID
		gtk_widget_show_all(GTK_WIDGET(writeIDBKCalToggle));
	else gtk_widget_hide(GTK_WIDGET(writeIDBKCalToggle));
	if(devType==PIC16)	//Program Calib
		gtk_widget_show_all(GTK_WIDGET(writeCalib12Toggle));
	else gtk_widget_hide(GTK_WIDGET(writeCalib12Toggle));
	if(devType==PIC12||devType==PIC16||devType==PIC18){	//Force config
		gtk_widget_show_all(GTK_WIDGET(configWOptsBox));
		gtk_widget_hide(GTK_WIDGET(CW2Box));
		gtk_widget_hide(GTK_WIDGET(CW3Box));
		gtk_widget_hide(GTK_WIDGET(CW4Box));
		gtk_widget_hide(GTK_WIDGET(CW5Box));
		gtk_widget_hide(GTK_WIDGET(CW6Box));
		gtk_widget_hide(GTK_WIDGET(CW7Box));
		if(devType==PIC16){
			gtk_widget_show_all(GTK_WIDGET(CW2Box));
		}
		else if(devType==PIC18){
			gtk_widget_show_all(GTK_WIDGET(CW2Box));
			gtk_widget_show_all(GTK_WIDGET(CW3Box));
			gtk_widget_show_all(GTK_WIDGET(CW4Box));
			gtk_widget_show_all(GTK_WIDGET(CW5Box));
			gtk_widget_show_all(GTK_WIDGET(CW6Box));
			gtk_widget_show_all(GTK_WIDGET(CW7Box));
		}
	}
	else{
		gtk_widget_hide(GTK_WIDGET(configWOptsBox));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configForceToggle),FALSE);
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
	GtkTreeModel *devStore = gtk_combo_box_get_model( GTK_COMBO_BOX(devCombo) );
	gtk_list_store_clear( GTK_LIST_STORE( devStore ) );
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
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(sourceTextView),&iter,0.0,TRUE,0,0.5);
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
		saveLog = (int) gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(logBtn));
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
#ifdef DEBUG
	addr=addrDebug;
	s=statusDebug;
	if(UseCoff) data=coff_data[addr];
#endif
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdPC_CheckMenuItem))){
		sprintf(temp,"%s: %s (0x%04X) \nPC=0x%04X\n",strings[S_NextIns],decodeCmd(data,cmd,(s&0x60)<<2),data,addr); //"Next instruction"
		strcat(status,temp);
	}
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdSTAT_CheckMenuItem))){
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
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdBank0_CheckMenuItem))) ShowBank(0,status);
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdBank1_CheckMenuItem))) ShowBank(1,status);
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdBank2_CheckMenuItem))) ShowBank(2,status);
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdBank3_CheckMenuItem))) ShowBank(3,status);
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
	if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(icdEE_CheckMenuItem))){
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
			if(i%8==7) {
				StrcatConvert(str,t);
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
					StrcatConvert(str,t);
				}
			}
			strcat(str,"\n");
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
		strncpy(s,gtk_entry_get_text(GTK_ENTRY(icdCommandEntry)),63);
		if(!strlen(s)){
			strcpy(s,lastCmd);
			gtk_entry_set_text(GTK_ENTRY(icdCommandEntry),s);	//briefly flash last command
			while (gtk_events_pending ()) gtk_main_iteration();
			msDelay(60);
		}
		else strcpy(lastCmd,s);
		if(executeCommand(s)) gtk_entry_set_text(GTK_ENTRY(icdCommandEntry),"");
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
	if(!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ioActiveToggle))) return G_SOURCE_CONTINUE;
	int i,j=0;
	int trisa=1,trisb=0,trisc=0x30,latac=0,latb=0;
	int port=0,z;
	char s2[64];
	str[0]=0;
	for(i=0;i<sizeof(ioButtons)/sizeof(ioButtons[0]);i++){
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ioButtons[i].r_1))){
			if(i<8) latb|=1<<i;
			else if(i==8) latac|=0x80; //RC7
			else if(i==9) latac|=0x40; //RC6
			else if(i==10) latac|=0x20; //RA5
			else if(i==11) latac|=0x10; //RA4
			else if(i==12) latac|=0x08; //RA3
		}
		else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ioButtons[i].r_I))){
			if(i<8)	trisb|=1<<i;
			else if(i==8) trisc|=0x80; //RC7
			else if(i==9) trisc|=0x40; //RC6
			else if(i==10) trisa|=0x20; //RA5
			else if(i==11) trisa|=0x10; //RA4
			else if(i==12) trisa|=0x8; //RA3
		}
	}
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
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ioActiveToggle))&&!icdTimer){
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
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(vppOnToggle))){
		vdd_vpp+=4;
		strcat(str,"VPP ");
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(vddOnToggle))){
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
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dcdcOnToggle))){
		int j=0,vreg=0;
		char str[16];
		double voltage=gtk_range_get_value(GTK_RANGE(dcdcVoltageRange));
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
	strncpy(hex,(const char *)gtk_entry_get_text(GTK_ENTRY(hexEntry)),sizeof(hex));
	if(strlen(hex)>0){
		if(hex[0]==':'&&strlen(hex)>8){
			length=htoi(hex+1,2);
			address=htoi(hex+3,4);
			if(strlen(hex)<11+length*2) gtk_entry_set_text(GTK_ENTRY(hexDataEntry),"__line too short");
			else{
				for (i=1;i<=length*2+9;i+=2) sum += htoi(hex+i,2);
				if ((sum & 0xff)!=0){
					sprintf(str,"__checksum error, expected 0x%02X",(-sum+htoi(hex+9+length*2,2))&0xFF);
					gtk_entry_set_text(GTK_ENTRY(hexDataEntry),str);
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
							gtk_entry_set_text(GTK_ENTRY(hexDataEntry),str);
							break;
						case 4:		//extended linear address record
							if(strlen(hex)>14){
								sprintf(str,"extended linear address = %04X",htoi(hex+9,4));
								gtk_entry_set_text(GTK_ENTRY(hexDataEntry),str);
							}
							break;
						default:
							gtk_entry_set_text(GTK_ENTRY(hexDataEntry),"__unknown record type");
							break;
					}
				}
			}
		}
		else gtk_entry_set_text(GTK_ENTRY(hexDataEntry),"__invalid line");
	}
	else gtk_entry_set_text(GTK_ENTRY(hexEntry),"");
}
///
/// convert address & data to hex line
void DataToHexConvert(GtkWidget *widget,GtkWidget *window)
{
	char hex[256],str[256],s2[32];
	int i,address,length,sum=0,x;
	i=sscanf(gtk_entry_get_text(GTK_ENTRY(addressEntry)),"%x",&address);
	if(i!=1) address=0;
	strncpy(hex,(const char *)gtk_entry_get_text(GTK_ENTRY(dataEntry)),sizeof(hex));
	length=strlen(hex);
	length&=0xFF;
	if(length>0){
		sprintf(str,":--%04X00",address&0xFFFF);
		for(i=0;i+1<length;i+=2){
			x=htoi(hex+i,2);
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
		gtk_entry_set_text(GTK_ENTRY(hexDataEntry2),str);
	}
}
///
///Choose a file to save a hex line
void HexSave(GtkWidget *widget,GtkWidget *window)
{
	GtkFileChooser *dialog;
	if(strlen((const char *)gtk_entry_get_text(GTK_ENTRY(hexDataEntry2)))<11) return;
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
			fprintf(f,(const char *)gtk_entry_get_text(GTK_ENTRY(hexDataEntry2)));
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
	gtk_widget_destroy(window);
}

///-----------------------------------
///Main function
///-----------------------------------
int main( int argc, char *argv[])
{
	//int langID=GetUserDefaultLangID();
	FILE *f;
	gchar *config_dir;
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
	strncpy(dev_ini,dev,sizeof(dev_ini));
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
		DeviceDetected=SearchDevice(&vid,&pid,false);
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

	app = gtk_application_new("openprog.gui", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(onActivate),NULL);
	g_signal_connect(app, "shutdown", G_CALLBACK(onShutdown),NULL);
	int status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref(app);
	return status;
}

void onActivate(GtkApplication *_app, gpointer user_data) {
	window = gtk_application_window_new(app);
	sprintf(str,"opgui v%s",VERSION);
	gtk_window_set_title(GTK_WINDOW(window),str);
	gtk_window_set_default_size(GTK_WINDOW(window), 750, 250);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	g_signal_connect(GTK_WINDOW(window), "destroy",G_CALLBACK(Xclose),NULL);
	gtk_window_set_icon(GTK_WINDOW(window),gdk_pixbuf_new_from_resource("/openprog/icons/sys.png", NULL));
	GtkWidget * mainVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_container_add(GTK_CONTAINER(window),mainVbox);

	GtkCssProvider *cssProv = gtk_css_provider_new();
	gtk_css_provider_load_from_resource(cssProv, "/openprog/css/style.css");
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProv), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

//------toolbar-------------
	openToolItem = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_BUTTON), strings[I_Fopen]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(openToolItem), strings[I_Fopen]);
	g_signal_connect(G_OBJECT(openToolItem), "clicked", G_CALLBACK(Fopen),NULL);

	saveToolItem = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_BUTTON), strings[I_Fsave]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(saveToolItem), strings[I_Fsave]);
	g_signal_connect(G_OBJECT(saveToolItem), "clicked", G_CALLBACK(Fsave),NULL);

	readToolItem = gtk_tool_button_new(\
		gtk_image_new_from_resource("/openprog/icons/read.png"), strings[I_DevR]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(readToolItem), strings[I_DevR]);
	g_signal_connect(G_OBJECT(readToolItem), "clicked", G_CALLBACK(DevRead),NULL);

	writeToolItem = gtk_tool_button_new(\
		gtk_image_new_from_resource("/openprog/icons/write.png"), strings[I_DevW]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(writeToolItem), strings[I_DevW]);
	g_signal_connect(G_OBJECT(writeToolItem), "clicked", G_CALLBACK(DevWrite),NULL);

	stopToolItem = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("process-stop", GTK_ICON_SIZE_BUTTON), strings[I_ICD_STOP]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(stopToolItem), strings[I_ICD_STOP]);
	g_signal_connect(G_OBJECT(stopToolItem), "clicked", G_CALLBACK(Stop),NULL);

	connectToolItem = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("network-wired", GTK_ICON_SIZE_BUTTON), strings[I_CONN]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(connectToolItem), strings[I_CONN]);
	g_signal_connect(G_OBJECT(connectToolItem), "clicked", G_CALLBACK(Connect),NULL);

	infoToolItem = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("dialog-information", GTK_ICON_SIZE_BUTTON), strings[I_Info]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(infoToolItem), strings[I_Info]);
	g_signal_connect(G_OBJECT(infoToolItem), "clicked", G_CALLBACK(info),NULL);

	toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar),GTK_TOOLBAR_ICONS);
	//gtk_box_pack_start(GTK_BOX(mainVbox),toolbar,FALSE,FALSE,0);

	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(openToolItem), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(saveToolItem), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_separator_tool_item_new(), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(readToolItem), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(writeToolItem), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_separator_tool_item_new(), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(stopToolItem), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(connectToolItem), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(infoToolItem), -1);

	gtk_widget_set_sensitive(GTK_WIDGET(stopToolItem), FALSE);

//------tab widget-------------
	notebook = gtk_notebook_new();
	//gtk_box_pack_start(GTK_BOX(mainVbox),notebook,FALSE,FALSE,0);
//------logging window
	data_scroll = gtk_scrolled_window_new(NULL,NULL);
	data = gtk_text_view_new();
	dataBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(data),FALSE);
	gtk_container_add(GTK_CONTAINER(data_scroll),data);
	styleCtx = gtk_widget_get_style_context(GTK_WIDGET(data));
	gtk_style_context_add_class(styleCtx, "mono");
	//gtk_box_pack_start(GTK_BOX(mainVbox),data_scroll,TRUE,TRUE,0);
//------device tab-------------
	GtkWidget * devGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(devGrid), 5);
	gtk_grid_set_row_spacing(GTK_GRID(devGrid), 5);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),devGrid,gtk_label_new(strings[I_Dev])); //"Device"
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
	devInfoLabel = gtk_label_new("i");
	gtk_box_pack_start(GTK_BOX(devHbox2),devInfoLabel,FALSE,FALSE,0);
	eepromRWToggle = gtk_check_button_new_with_label(strings[I_EE]);	//"Read and write EEPROM"
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(eepromRWToggle),TRUE);

	gtk_grid_attach(GTK_GRID(devGrid),eepromRWToggle,0,2,1,1);
	picOptsBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);//gtk_frame_new(NULL);	//"PIC configuration"

	gtk_grid_attach(GTK_GRID(devGrid),picOptsBox,0,3,1,1);

	GtkWidget *picGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(picGrid), 5);
	gtk_grid_set_row_spacing(GTK_GRID(picGrid), 5);

	gtk_box_pack_start(GTK_BOX(picOptsBox),picGrid,FALSE,TRUE,0);
	GtkWidget * picVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_grid_attach(GTK_GRID(picGrid),picVbox,0,0,1,1);

	readReservedToggle = gtk_check_button_new_with_label(strings[I_ReadRes]);	//"Read reserved area"
	gtk_container_add(GTK_CONTAINER(picVbox),GTK_WIDGET(readReservedToggle));
	writeIDBKCalToggle = gtk_check_button_new_with_label(strings[I_ID_BKo_W]);	//"Write ID and BKOscCal"
	gtk_container_add(GTK_CONTAINER(picVbox),GTK_WIDGET(writeIDBKCalToggle));
	writeCalib12Toggle = gtk_check_button_new_with_label(strings[I_CalW]);	//"Write Calib 1 and 2"
	gtk_container_add(GTK_CONTAINER(picVbox),GTK_WIDGET(writeCalib12Toggle));
	oscOptsBox = gtk_frame_new(strings[I_OSCW]);	//"Write OscCal"

	gtk_grid_attach(GTK_GRID(picGrid),oscOptsBox,0,1,1,1);
	GtkWidget * oscOptsVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_container_add(GTK_CONTAINER(oscOptsBox),GTK_WIDGET(oscOptsVbox));
	useOSCCALToggle = gtk_radio_button_new_with_label(NULL,strings[I_OSC]);	//"OSCCal"
	useBKOSCCALToggle = gtk_radio_button_new_with_label(\
		gtk_radio_button_get_group(GTK_RADIO_BUTTON(useOSCCALToggle)),strings[I_BKOSC]);	//"Backup OSCCal"
	useFileCalToggle = gtk_radio_button_new_with_label(\
		gtk_radio_button_get_group(GTK_RADIO_BUTTON(useOSCCALToggle)),strings[I_OSCF]);	//"From file"
	gtk_container_add(GTK_CONTAINER(oscOptsVbox),GTK_WIDGET(useOSCCALToggle));
	gtk_container_add(GTK_CONTAINER(oscOptsVbox),GTK_WIDGET(useBKOSCCALToggle));
	gtk_container_add(GTK_CONTAINER(oscOptsVbox),GTK_WIDGET(useFileCalToggle));

	icdOptsBox = gtk_frame_new("ICD");
	GtkWidget * icdOptsHbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,25);
	icdCheckToggle = gtk_check_button_new_with_label(strings[I_ICD_ENABLE]);	//"Enable ICD"
	icdAddrEntry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(icdAddrEntry),4);
	GtkWidget * icdOptsHbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_box_pack_start(GTK_BOX(icdOptsHbox2),GTK_WIDGET(gtk_label_new(strings[I_ICD_ADDRESS])),0,0,1); //"ICD routine address"
	gtk_box_pack_start(GTK_BOX(icdOptsHbox2),GTK_WIDGET(icdAddrEntry),0,0,2);
	gtk_container_add(GTK_CONTAINER(icdOptsHbox1),icdCheckToggle);
	gtk_container_add(GTK_CONTAINER(icdOptsHbox1),GTK_WIDGET(icdOptsHbox2));
	gtk_container_add(GTK_CONTAINER(icdOptsBox),GTK_WIDGET(icdOptsHbox1));
	gtk_grid_attach(GTK_GRID(picGrid),icdOptsBox,0,2,2,1);

	configWOptsBox = gtk_frame_new("Config Word");
	gtk_grid_attach(GTK_GRID(picGrid),configWOptsBox,1,0,1,2);
	GtkWidget * cwGrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(configWOptsBox),GTK_WIDGET(cwGrid));
	configForceToggle = gtk_check_button_new_with_label(strings[I_PIC_FORCECW]); //"force config word"
	gtk_grid_attach(GTK_GRID(cwGrid),configForceToggle,0,0,2,1);
#define CWX(y) 	CW##y##Box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);\
				gtk_box_pack_start(GTK_BOX(CW##y##Box),GTK_WIDGET(gtk_label_new("CW"#y)),0,0,1);\
				CW##y##Entry = gtk_entry_new();\
				gtk_entry_set_width_chars(GTK_ENTRY(CW##y##Entry),4);\
				gtk_box_pack_start(GTK_BOX(CW##y##Box),GTK_WIDGET(CW##y##Entry),0,0,1);\
				gtk_grid_attach(GTK_GRID(cwGrid),CW##y##Box,(y+1)%2,(y+1)/2,1,1);
	CWX(1);
	CWX(2);
	CWX(3);
	CWX(4);
	CWX(5);
	CWX(6);
	CWX(7);

	avrOptsBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);	//Atmel configuration
	gtk_grid_attach(GTK_GRID(devGrid),avrOptsBox,0,3,1,1);
	GtkWidget * avrGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(avrGrid), 5);
	gtk_grid_set_row_spacing(GTK_GRID(avrGrid), 2);
	gtk_box_pack_start(GTK_BOX(avrOptsBox),avrGrid,FALSE,TRUE,0);

	avrFuseLowEntry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(avrFuseLowEntry),4);
	gtk_grid_attach(GTK_GRID(avrGrid),avrFuseLowEntry,0,1,1,1);
	avrFuseLowWriteToggle = gtk_check_button_new_with_label(strings[I_AT_FUSE]);	//"Write Fuse Low"
	gtk_grid_attach(GTK_GRID(avrGrid),avrFuseLowWriteToggle,1,1,1,1);
	avrFuseHighEntry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(avrFuseHighEntry),4);
	gtk_grid_attach(GTK_GRID(avrGrid),avrFuseHighEntry,0,2,1,1);
	avrFuseHighWriteToggle = gtk_check_button_new_with_label(strings[I_AT_FUSEH]);	//"Write Fuse High"
	gtk_grid_attach(GTK_GRID(avrGrid),avrFuseHighWriteToggle,1,2,1,1);
	avrFuseExtEntry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(avrFuseExtEntry),4);
	gtk_grid_attach(GTK_GRID(avrGrid),avrFuseExtEntry,0,3,1,1);
	avrFuseExtWriteToggle = gtk_check_button_new_with_label(strings[I_AT_FUSEX]);	//"Write Extended Fuse"
	gtk_grid_attach(GTK_GRID(avrGrid),avrFuseExtWriteToggle,1,3,1,1);
	avrLockEntry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(avrLockEntry),4);
	gtk_grid_attach(GTK_GRID(avrGrid),avrLockEntry,0,4,1,1);
	avrLockWriteToggle = gtk_check_button_new_with_label(strings[I_AT_LOCK]);	//"Write Lock"
	gtk_grid_attach(GTK_GRID(avrGrid),avrLockWriteToggle,1,4,1,1);
	wFuseLFBtn = gtk_button_new_with_label(strings[I_AT_FUSELF]);		//"Write Fuse Low @3kHz"
	gtk_grid_attach(GTK_GRID(avrGrid),wFuseLFBtn,0,5,1,1);

//------options tab-------------
	GtkWidget * optGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(optGrid), 5);
	gtk_grid_set_row_spacing(GTK_GRID(optGrid), 5);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),optGrid,gtk_label_new(strings[I_Opt])); //"Options"
	connectBtn = gtk_button_new_with_label(strings[I_CONN]);	//"Reconnect"
	gtk_grid_attach(GTK_GRID(optGrid),connectBtn,0,0,1,1);
	GtkWidget * optHboxVid = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_grid_attach(GTK_GRID(optGrid),optHboxVid,0,1,1,1);
	gtk_box_pack_start(GTK_BOX(optHboxVid),gtk_label_new("VID:"),FALSE,TRUE,0);
	vidEntry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(vidEntry),4);
	gtk_box_pack_start(GTK_BOX(optHboxVid),vidEntry,FALSE,TRUE,0);
	GtkWidget * optHboxPid = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_grid_attach(GTK_GRID(optGrid),optHboxPid,1,1,1,1);
	gtk_box_pack_start(GTK_BOX(optHboxPid),gtk_label_new("PID:"),FALSE,TRUE,0);
	pidEntry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(pidEntry),4);
	gtk_box_pack_start(GTK_BOX(optHboxPid),pidEntry,FALSE,TRUE,0);

	testHWBtn = gtk_button_new_with_label(strings[I_TestHWB]);	//"Hardware test"
	gtk_grid_attach(GTK_GRID(optGrid),testHWBtn,0,2,1,1);
	logBtn = gtk_check_button_new_with_label(strings[I_LOG]);	//"Log activity"
	gtk_grid_attach(GTK_GRID(optGrid),logBtn,0,6,2,1);
	V33CheckToggle = gtk_check_button_new_with_label(strings[I_CK_V33]);	//"Don't check for 3.3V regulator"
	gtk_grid_attach(GTK_GRID(optGrid),V33CheckToggle,0,7,2,1);
	waitS1Toggle = gtk_check_button_new_with_label(strings[I_WAITS1]);	//"Wait for S1 before read/write"
	gtk_grid_attach(GTK_GRID(optGrid),waitS1Toggle,0,8,2,1);
	GtkWidget * optHboxErr = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_grid_attach(GTK_GRID(optGrid),optHboxErr,0,9,2,1);
	gtk_container_add(GTK_CONTAINER(optHboxErr),gtk_label_new(strings[I_MAXERR])); //"Max errors"
	errorsEntry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(errorsEntry),6);
	gtk_container_add(GTK_CONTAINER(optHboxErr),GTK_WIDGET(errorsEntry));
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
	gtk_container_add(GTK_CONTAINER(i2cVboxTX),gtk_label_new(strings[I_I2CDATAOUT])); //"Data to send"
	i2cDataSendEntry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(i2cVboxTX),GTK_WIDGET(i2cDataSendEntry));
	GtkWidget * i2cVboxRX = gtk_box_new(GTK_ORIENTATION_VERTICAL,2);
	gtk_grid_attach(GTK_GRID(i2cGrid),i2cVboxRX,1,4,2,1);
	gtk_container_add(GTK_CONTAINER(i2cVboxRX),gtk_label_new(strings[I_I2CDATATR])); //"Data transferred"
	i2cDataRcvEntry = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(i2cDataRcvEntry),FALSE);
	gtk_container_add(GTK_CONTAINER(i2cVboxRX),GTK_WIDGET(i2cDataRcvEntry));
	gtk_widget_set_size_request(i2cDataRcvEntry,100,60);
	GtkWidget * i2cHboxNB = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_grid_attach(GTK_GRID(i2cGrid),i2cHboxNB,1,0,2,1);
	gtk_container_add(GTK_CONTAINER(i2cHboxNB),gtk_label_new(strings[I_I2C_NB])); //"Byes to read/write"
	i2cNbyteSpinBtn = 	gtk_spin_button_new_with_range(0,64,1);
	gtk_container_add(GTK_CONTAINER(i2cHboxNB),GTK_WIDGET(i2cNbyteSpinBtn));
	GtkWidget * i2cHboxSpeed = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_grid_attach(GTK_GRID(i2cGrid),i2cHboxSpeed,1,1,2,1);
	gtk_container_add(GTK_CONTAINER(i2cHboxSpeed),gtk_label_new(strings[I_Speed])); //"Speed"
	i2cSpeedCombo = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(i2cSpeedCombo),"100 kbps");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(i2cSpeedCombo),"200 kbps");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(i2cSpeedCombo),"300/400 kbps");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(i2cSpeedCombo),"500/800 kbps");
	gtk_combo_box_set_active(GTK_COMBO_BOX(i2cSpeedCombo),0);
	gtk_container_add(GTK_CONTAINER(i2cHboxSpeed),GTK_WIDGET(i2cSpeedCombo));
	i2cSendBtn = gtk_button_new_with_label(strings[I_I2CSend]);	//"Send"
	gtk_grid_attach(GTK_GRID(i2cGrid),i2cSendBtn,1,2,1,1);
	i2cRcvBtn = gtk_button_new_with_label(strings[I_I2CReceive]);	//"Receive"
	gtk_grid_attach(GTK_GRID(i2cGrid),i2cRcvBtn,2,2,1,1);
//------ICD tab-------------
	icdVBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),icdVBox,gtk_label_new("ICD"));
	//menu
	GtkWidget * icdMenuBar = gtk_menu_bar_new ();
	GtkWidget * icdRootMenu = gtk_menu_item_new_with_label(strings[I_Opt]); //"Options";
    gtk_menu_shell_append (GTK_MENU_SHELL (icdMenuBar), icdRootMenu);
	GtkWidget * icdMenu = gtk_menu_new();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (icdRootMenu), icdMenu);
	icdPC_CheckMenuItem = gtk_check_menu_item_new_with_label(strings[I_SHOW_PC]); //"show Program Counter"
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(icdPC_CheckMenuItem),TRUE);
	icdSTAT_CheckMenuItem = gtk_check_menu_item_new_with_label(strings[I_SHOW_STATUS]); //"show status registers"
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(icdSTAT_CheckMenuItem),TRUE);
	icdBank0_CheckMenuItem = gtk_check_menu_item_new_with_label(strings[I_SHOW_BANK0]); //"show memory bank 0"
	icdBank1_CheckMenuItem = gtk_check_menu_item_new_with_label(strings[I_SHOW_BANK1]); //"show memory bank 1"
	icdBank2_CheckMenuItem = gtk_check_menu_item_new_with_label(strings[I_SHOW_BANK2]); //"show memory bank 2"
	icdBank3_CheckMenuItem = gtk_check_menu_item_new_with_label(strings[I_SHOW_BANK3]); //"show memory bank 3"
	icdEE_CheckMenuItem = gtk_check_menu_item_new_with_label(strings[I_SHOW_EE]); //"show EEPROM"
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdPC_CheckMenuItem);
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdSTAT_CheckMenuItem);
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdBank0_CheckMenuItem);
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdBank1_CheckMenuItem);
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdBank2_CheckMenuItem);
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdBank3_CheckMenuItem);
	gtk_menu_shell_append (GTK_MENU_SHELL (icdMenu), icdEE_CheckMenuItem);
	//toolbar
	GtkToolItem* icdGoBtn = gtk_tool_button_new(\
		gtk_image_new_from_resource("/openprog/icons/go.png"), strings[I_ICD_RUN]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(icdGoBtn), strings[I_ICD_RUN]);
	g_signal_connect(G_OBJECT(icdGoBtn), "clicked", G_CALLBACK(icdRun),NULL);

	GtkToolItem* icdHaltBtn = gtk_tool_button_new(\
		gtk_image_new_from_resource("/openprog/icons/halt.png"), strings[I_ICD_HALT]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(icdHaltBtn), strings[I_ICD_HALT]);
	g_signal_connect(G_OBJECT(icdHaltBtn), "clicked", G_CALLBACK(icdHalt),NULL);

	GtkToolItem* icdStepBtn = gtk_tool_button_new(\
		gtk_image_new_from_resource("/openprog/icons/step.png"), strings[I_ICD_STEP]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(icdStepBtn), strings[I_ICD_STEP]);
	g_signal_connect(G_OBJECT(icdStepBtn), "clicked", G_CALLBACK(icdStep),NULL);

	GtkToolItem* icdStepoverBtn = gtk_tool_button_new(\
		gtk_image_new_from_resource("/openprog/icons/stepover.png"), strings[I_ICD_STEPOVER]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(icdStepoverBtn), strings[I_ICD_STEPOVER]);
	g_signal_connect(G_OBJECT(icdStepoverBtn), "clicked", G_CALLBACK(icdStepOver),NULL);

	GtkToolItem* icdStopBtn = gtk_tool_button_new(\
		gtk_image_new_from_resource("/openprog/icons/stop.png"), strings[I_ICD_STOP]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(icdStopBtn), strings[I_ICD_STOP]);
	g_signal_connect(G_OBJECT(icdStopBtn), "clicked", G_CALLBACK(icdStop),NULL);

	GtkToolItem* icdRefreshBtn = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("view-refresh", GTK_ICON_SIZE_BUTTON), strings[I_ICD_REFRESH]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(icdRefreshBtn), strings[I_ICD_REFRESH]);
	g_signal_connect(G_OBJECT(icdRefreshBtn), "clicked", G_CALLBACK(icdRefresh),NULL);

	GtkToolItem* icdLoadCoffBtn = gtk_tool_button_new(NULL, strings[I_LOAD_COFF]); //"load COFF file..."
	g_signal_connect(G_OBJECT(icdLoadCoffBtn), "clicked", G_CALLBACK(loadCoff),window);

	GtkToolItem* icdCommandToolItem = gtk_tool_item_new();
	icdCommandEntry = gtk_entry_new();
	gtk_widget_set_tooltip_text(icdCommandEntry, strings[I_ICD_CMD]);
	gtk_container_add(GTK_CONTAINER(icdCommandToolItem), icdCommandEntry);

	GtkToolItem* icdHelpBtn = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("system-help", GTK_ICON_SIZE_BUTTON), strings[I_ICD_HELP]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(icdHelpBtn), strings[I_ICD_HELP]);
	g_signal_connect(G_OBJECT(icdHelpBtn), "clicked", G_CALLBACK(ICDHelp),NULL);

	GtkWidget * icdToolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(icdToolbar),GTK_TOOLBAR_ICONS);
	gtk_box_pack_start(GTK_BOX(icdVBox),icdToolbar,FALSE,FALSE,0);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),icdGoBtn,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),icdHaltBtn,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),icdStepBtn,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),icdStepoverBtn,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),gtk_separator_tool_item_new(),-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),icdStopBtn,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),gtk_separator_tool_item_new(),-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),icdRefreshBtn,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),gtk_separator_tool_item_new(),-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),icdLoadCoffBtn,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),gtk_separator_tool_item_new(),-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),icdCommandToolItem,-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),gtk_separator_tool_item_new(),-1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar),icdHelpBtn,-1);

	GtkWidget *icdHpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(icdVBox),icdHpaned,TRUE,TRUE,0);
	gint width,height;
	gtk_window_get_size(GTK_WINDOW(window),&width,&height);
	gtk_paned_set_position(GTK_PANED (icdHpaned),width/2);
	//source
	GtkWidget * icdVbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_paned_pack1(GTK_PANED (icdHpaned), icdVbox2, TRUE, FALSE);
	gtk_box_pack_start(GTK_BOX(icdVbox2),gtk_label_new(strings[I_ICD_SOURCE]),FALSE,FALSE,0); //"Source"
	GtkWidget * sourceScroll = gtk_scrolled_window_new(NULL,NULL);
	sourceTextView = gtk_text_view_new();
	sourceBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sourceTextView));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(sourceTextView),FALSE);
	gtk_container_add(GTK_CONTAINER(sourceScroll),sourceTextView);

	styleCtx = gtk_widget_get_style_context(GTK_WIDGET(sourceTextView));
	gtk_style_context_add_class(styleCtx, "mono");

	gtk_box_pack_start(GTK_BOX(icdVbox2),sourceScroll,TRUE,TRUE,0);
	//status
	GtkWidget * icdVbox3 = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_paned_pack2 (GTK_PANED (icdHpaned), icdVbox3, TRUE, FALSE);
	GtkWidget * icdHbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,20);
	gtk_box_set_homogeneous(GTK_BOX(icdHbox2), TRUE);
	gtk_box_pack_start(GTK_BOX(icdVbox3),icdHbox2,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(icdHbox2),gtk_label_new(strings[I_ICD_STATUS]),FALSE,FALSE,0); //"Status"
	gtk_box_pack_start(GTK_BOX(icdHbox2),icdMenuBar,FALSE,FALSE,0);
	GtkWidget * statusScroll = gtk_scrolled_window_new(NULL,NULL);
	statusTextView = gtk_text_view_new();
	statusBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(statusTextView));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(statusTextView),FALSE);
	gtk_container_add(GTK_CONTAINER(statusScroll),statusTextView);

	styleCtx = gtk_widget_get_style_context(GTK_WIDGET(statusTextView));
	gtk_style_context_add_class(styleCtx, "mono");

	gtk_box_pack_start(GTK_BOX(icdVbox3),statusScroll,TRUE,TRUE,0);
//------IO tab-------------
	GtkWidget * ioVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),ioVbox,gtk_label_new("I/O"));
	GtkWidget * ioFrame = gtk_frame_new("I/O");
	gtk_box_pack_start(GTK_BOX(ioVbox),ioFrame,FALSE,FALSE,0);
	GtkWidget * ioGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(ioGrid),20);

	gtk_container_add(GTK_CONTAINER(ioFrame),GTK_WIDGET(ioGrid));
	ioActiveToggle = gtk_check_button_new_with_label(strings[I_IO_Enable]);	//"Enable IO"
	gtk_grid_attach(GTK_GRID(ioGrid),ioActiveToggle,0,0,1,1);
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
		gtk_box_pack_start(GTK_BOX(ioBoxRBx),gtk_label_new(ioButtons[ii].name),FALSE,TRUE,0);
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
	GtkWidget * dcdcHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	gtk_box_pack_start(GTK_BOX(ioVbox),dcdcHbox,FALSE,FALSE,0);
	vddOnToggle = gtk_check_button_new_with_label("VDDU");	//""
	gtk_box_pack_start(GTK_BOX(dcdcHbox),vddOnToggle,FALSE,TRUE,2);
	vppOnToggle = gtk_check_button_new_with_label("VPPU");	//""
	gtk_box_pack_start(GTK_BOX(dcdcHbox),vppOnToggle,FALSE,TRUE,2);
	dcdcOnToggle = gtk_check_button_new_with_label("DCDC");	//""
	gtk_box_pack_start(GTK_BOX(dcdcHbox),dcdcOnToggle,FALSE,TRUE,2);
	dcdcVoltageRange=gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,5,15,0.1);
	gtk_box_pack_start(GTK_BOX(dcdcHbox),dcdcVoltageRange,TRUE,TRUE,2);
//------Utility tab-------------
	GtkWidget * utVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),utVbox,gtk_label_new("Utility"));

	GtkWidget * utFrameH2D = gtk_frame_new("HEX -> DATA");
	gtk_box_pack_start(GTK_BOX(utVbox),utFrameH2D,FALSE,FALSE,5);
	GtkWidget * utHexVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	GtkWidget * utHexHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	hexEntry = gtk_entry_new();
	hexDataEntry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(utFrameH2D),utHexVbox);
	gtk_container_add(GTK_CONTAINER(utHexVbox),utHexHbox);
	gtk_box_pack_start(GTK_BOX(utHexHbox),gtk_label_new("Hex"),FALSE,TRUE,5);
	gtk_box_pack_start(GTK_BOX(utHexHbox),hexEntry,TRUE,TRUE,5);
	gtk_editable_set_editable(GTK_EDITABLE(hexDataEntry),0);
	gtk_box_pack_start(GTK_BOX(utHexVbox),hexDataEntry,TRUE,TRUE,5);

	GtkWidget * utFrameD2H = gtk_frame_new("DATA -> HEX");
	gtk_box_pack_start(GTK_BOX(utVbox),utFrameD2H,FALSE,FALSE,5);
	GtkWidget * utGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(utGrid),5);
	gtk_grid_set_row_spacing(GTK_GRID(utGrid),5);
	gtk_container_add(GTK_CONTAINER(utFrameD2H),utGrid);
	gtk_grid_attach(GTK_GRID(utGrid),gtk_label_new(strings[I_ADDR]),0,0,1,1); //"Address"
	addressEntry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(utGrid),addressEntry,1,0,1,1);
	gtk_grid_attach(GTK_GRID(utGrid),gtk_label_new(strings[I_Data]),0,1,1,1); //Data
	dataEntry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(utGrid),dataEntry,1,1,1,1);
	gtk_grid_attach(GTK_GRID(utGrid),gtk_label_new("Hex"),0,2,1,1);
	hexDataEntry2 = gtk_entry_new();
	gtk_editable_set_editable(GTK_EDITABLE(hexDataEntry2),0);
	gtk_grid_attach(GTK_GRID(utGrid),hexDataEntry2,1,2,1,1);
	GtkWidget * hexSaveBtn = gtk_button_new_with_label(strings[I_Fsave]);
	gtk_grid_attach(GTK_GRID(utGrid),hexSaveBtn,0,3,1,1);
//------status bar-------------
	status_bar = gtk_statusbar_new();
	//gtk_box_pack_start(GTK_BOX(mainVbox),status_bar,FALSE,TRUE,0);
	statusID=gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar),"ID");
	g_signal_connect(G_OBJECT(testHWBtn),"clicked",G_CALLBACK(TestHw),window);
	g_signal_connect(G_OBJECT(connectBtn),"clicked",G_CALLBACK(Connect),window);
	g_signal_connect(G_OBJECT(i2cRcvBtn),"clicked",G_CALLBACK(I2cspiR),window);
	g_signal_connect(G_OBJECT(i2cSendBtn),"clicked",G_CALLBACK(I2cspiS),window);
	g_signal_connect(G_OBJECT(sourceTextView),"button_press_event",G_CALLBACK(source_mouse_event),NULL);
	g_signal_connect(G_OBJECT(statusTextView),"button_press_event",G_CALLBACK(icdStatus_mouse_event),NULL);
	g_signal_connect(G_OBJECT(icdCommandEntry),"key_press_event",G_CALLBACK(icdCommand_key_event),NULL);
	g_signal_connect(G_OBJECT(icdVBox),"key_press_event",G_CALLBACK(icd_key_event),NULL);
	g_signal_connect(G_OBJECT(devTypeCombo),"changed",G_CALLBACK(FilterDevType),NULL);
	g_signal_connect(G_OBJECT(devCombo),"changed",G_CALLBACK(DeviceChanged),NULL);
	g_signal_connect(G_OBJECT(ioActiveToggle),"toggled",G_CALLBACK(IOactive),NULL);
	g_signal_connect(G_OBJECT(vddOnToggle),"toggled",G_CALLBACK(VPPVDDactive),NULL);
	g_signal_connect(G_OBJECT(vppOnToggle),"toggled",G_CALLBACK(VPPVDDactive),NULL);
	g_signal_connect(G_OBJECT(dcdcOnToggle),"toggled",G_CALLBACK(DCDCactive),NULL);
	g_signal_connect(G_OBJECT(dcdcVoltageRange),"value_changed",G_CALLBACK(DCDCactive),NULL);
	g_signal_connect(G_OBJECT(hexEntry),"changed",G_CALLBACK(HexConvert),NULL);
	g_signal_connect(G_OBJECT(addressEntry),"changed",G_CALLBACK(DataToHexConvert),NULL);
	g_signal_connect(G_OBJECT(dataEntry),"changed",G_CALLBACK(DataToHexConvert),NULL);
	g_signal_connect(G_OBJECT(hexSaveBtn),"clicked",G_CALLBACK(HexSave),window);
	g_signal_connect(G_OBJECT(wFuseLFBtn),"clicked",G_CALLBACK(WriteATfuseLowLF),window);

	//*********Device tree******
  	devTree = gtk_tree_view_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(devTree),
		-1, "Name", gtk_cell_renderer_text_new(), "text", DEVICE_NAME_COLUMN, NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(devTree),
		-1, "Family", gtk_cell_renderer_text_new(), "text", DEVICE_FAMILY_COLUMN, NULL);

  	devStore = gtk_list_store_new (DEVICE_N_COLUMNS,
	  						  G_TYPE_UINT,
                              G_TYPE_STRING,
                              G_TYPE_STRING);

	AddDevices();

	gtk_tree_view_set_model(GTK_TREE_VIEW(devTree), GTK_TREE_MODEL(devStore));
 	g_object_unref (G_OBJECT (devStore));
	gtk_box_pack_start(GTK_BOX(mainVbox),devTree,TRUE,TRUE,0);

	gtk_widget_show_all(window);
//********Init*************
	char text[16];
	sprintf(text,"%04X",vid);
	gtk_entry_set_text(GTK_ENTRY(vidEntry),text);
	sprintf(text,"%04X",pid);
	gtk_entry_set_text(GTK_ENTRY(pidEntry),text);
	sprintf(text,"%d",max_err);
	gtk_entry_set_text(GTK_ENTRY(errorsEntry),text);
	sizeW=0x8400;
	memCODE_W=malloc(sizeW*sizeof(WORD));
	initVar();
	for(int i=0;i<0x8400;i++) memCODE_W[i]=0x3fff;
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
	DeviceDetected=SearchDevice(&vid,&pid,false);
	ProgID();		//get firmware version and reset
}

void onShutdown(GtkApplication *_app, gpointer user_data) {
//******Save ini file******
// only if parameters are changed
	if(strcmp(dev_ini,dev)||vid_ini!=vid||pid_ini!=pid||max_err_ini!=max_err){
		if(homedir){
			FILE *f=fopen(fname,"w");
			if(f){
				fprintf(f,"device %s\n",dev);
				fprintf(f,"maxerr %d\n",max_err);
				fprintf(f,"vid %X\n",vid);
				fprintf(f,"pid %X\n",pid);
			}
			fclose(f);
		}
	}
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
	vid=htoi(gtk_entry_get_text(GTK_ENTRY(vidEntry)),4);
	pid=htoi(gtk_entry_get_text(GTK_ENTRY(pidEntry)),4);
	DeviceDetected=SearchDevice(&vid,&pid,false);
	if(DeviceDetected) { // Populate detected VID/PID to textboxes
		char text[16];
		sprintf(text,"%04X",vid);
		gtk_entry_set_text(GTK_ENTRY(vidEntry),text);
		sprintf(text,"%04X",pid);
		gtk_entry_set_text(GTK_ENTRY(pidEntry),text);
	}
	hvreg=0;
	ProgID();
}
///
/// I2C/SPI receive
void I2cspiR()
{
	gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,"");
	saveLog = (int) gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(logBtn));
	int nbyte=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(i2cNbyteSpinBtn));
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
    strncpy(tokbuf, (const char *)gtk_entry_get_text(GTK_ENTRY(i2cDataSendEntry)), sizeof(tokbuf));
	for(tok=strtok(tokbuf," ");tok&&i<128;tok=strtok(NULL," ")){
		if(sscanf(tok,"%x",&x)){
			tmpbuf[i] = (BYTE)x;
			i++;
		}
	}
	for(;i<128;i++) tmpbuf[i]=0;
	I2CReceive(mode,gtk_combo_box_get_active(GTK_COMBO_BOX(i2cSpeedCombo)),nbyte,tmpbuf);
}
///
/// I2C/SPI send
void I2cspiS()
{
	//if(DeviceDetected!=1) return;
	gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,"");
	saveLog = (int) gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(logBtn));
	int nbyte=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(i2cNbyteSpinBtn));
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
    strncpy(tokbuf, (const char *)gtk_entry_get_text(GTK_ENTRY(i2cDataSendEntry)), sizeof(tokbuf));
	for(tok=strtok(tokbuf," ");tok&&i<128;tok=strtok(NULL," ")){
		if(sscanf(tok,"%x",&x)){
			tmpbuf[i] = (BYTE)x;
			i++;
		}
	}
	for(;i<128;i++) tmpbuf[i]=0;
	I2CSend(mode,gtk_combo_box_get_active(GTK_COMBO_BOX(i2cSpeedCombo)),nbyte,tmpbuf);
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
			StrcatConvert(v,t);
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
	int i,j=0,type=-1;
	char *str=0,*tok;
	for(i=0;i<NDEVLIST;i++){
		if(str) free(str);
		str=malloc(strlen(DEVLIST[i].device)+1);
		strcpy(str,DEVLIST[i].device);
		for(tok=strtok(str,",");tok;tok=strtok(NULL,",")){
			type=DEVLIST[i].family;
			gtk_list_store_insert_with_values(devStore, NULL, -1,
				DEVICE_ID_COLUMN, j++,
				DEVICE_NAME_COLUMN, tok,
				DEVICE_FAMILY_COLUMN, familyNames[type], -1);
		}
	}
	free(str);
}
