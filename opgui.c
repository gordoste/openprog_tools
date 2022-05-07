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
#define Tck 30
double Tcom=0.001*Tck*18+0.03; //communication time for a 16 bit tranfer (ms)
guint icdTimer=0,IOTimer=0;
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
GtkWidget *devTypeCombo, *picOptsBox, *avrOptsBox, *configWOptsFrame, *icdOptsFrame, *oscOptsFrame, *devInfoLabel;
GtkWidget *icdCheckToggle, *icdAddrEntry;
GtkWidget *eepromRWToggle, *readReservedToggle, *writeIDBKCalToggle, *writeCalib12Toggle, *useOSCCALToggle, *useBKOSCCALToggle, *useFileCalToggle;
GtkWidget *avrFuseLowEntry, *avrFuseLowWriteToggle, *avrFuseHighEntry, *avrFuseHighWriteToggle, *avrFuseExtEntry, *avrFuseExtWriteToggle, *avrLockEntry, *avrLockWriteToggle;
GtkWidget *wFuseLFBtn, *connectBtn, *testHWBtn, *logBtn;
GtkWidget *vidEntry, *pidEntry, *errorsEntry;
GtkWidget *I2C8bit, *I2C16bit, *SPI00, *SPI01, *SPI10, *SPI11;
GtkWidget *i2cDataSendEntry, *i2cDataRcvEntry, *i2cSendBtn, *i2cRcvBtn, *i2cNbyteSpinBtn, *i2cSpeedCombo;
GtkWidget *dcdcOnToggle, *dcdcVoltageRange, *vppOnToggle, *vddOnToggle;
GtkWidget *ioActiveToggle, *V33CheckToggle, *waitS1Toggle;
GtkWidget *hexEntry, *addressEntry, *dataEntry, *hexDataEntry, *hexDataEntry2, *hexSaveBtn;
GtkWidget *CW1Entry, *CW2Entry, *CW3Entry, *CW4Entry, *CW5Entry, *CW6Entry, *CW7Entry;
GtkWidget *CW1Box, *CW2Box, *CW3Box, *CW4Box, *CW5Box, *CW6Box, *CW7Box;
GtkWidget *configForceToggle;
GtkListStore *devStore;
GtkWidget *devTree, *devFiltEntry, *devFrame;
GtkTreeSelection *devSel;

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
	GetSelectedDevice();
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
}
///
///Choose a file to open and call Load()
void Fopen(GtkWidget *widget,GtkWidget *window)
{
	GetSelectedDevice();
	if(progress) return;
	progress=1;
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
			Read33(dev,ee,readRes,skipV33check);	//choose the right function
			progress=0;
			gtk_widget_set_sensitive(GTK_WIDGET(stopToolItem),FALSE);
		}
		waitingS1=0;
	}
	else if(waitingS1) waitingS1=0;
	else if(!progress){
		gtk_widget_set_sensitive(GTK_WIDGET(stopToolItem),TRUE);
		progress=1;
		Read33(dev,ee,readRes,skipV33check);	//choose the right function
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
///Check GUI for selected device and put in variable 'dev'. Also enable/disable R/W buttons
void GetSelectedDevice() {
	GtkTreeModel *tmpModel;
	GtkTreeIter tmpIter;
	char *devName;
	if (!GTK_IS_TREE_SELECTION(devSel)) { // Not initialised yet
		return;
	}
	if (gtk_tree_selection_get_selected(devSel, &tmpModel, &tmpIter)) {
		gtk_tree_model_get(tmpModel, &tmpIter, DEVICE_NAME_COLUMN, &devName, -1);
		strcpy(dev,devName);
		gtk_widget_set_sensitive(GTK_WIDGET(readToolItem), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(writeToolItem), TRUE);
		g_free(devName);
	} else { // Shouldn't ever happen, but just in case
		dev[0] = '\0';
		gtk_widget_set_sensitive(GTK_WIDGET(readToolItem), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(writeToolItem), FALSE);
	}
}

///
///Callback function to set available options for each device type
void onDevSel_Changed(GtkWidget *widget,GtkWidget *window)
{
	struct DevInfo info;
	char str2[256],str3[64],strF[32];
	double x;
	GetSelectedDevice();
	if (strlen(dev) == 0) return; // None selected
	info=GetDevInfo(dev);

	sprintf(str, "<b>%s: %s</b>", strings[I_Dev], dev);
	gtk_label_set_markup(GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(devFrame))), str);

	devType=info.family;
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
		gtk_widget_show_all(GTK_WIDGET(icdOptsFrame));
	else gtk_widget_hide(GTK_WIDGET(icdOptsFrame));
	if(devType==PIC12||devType==PIC16)	//Osc options
		gtk_widget_show_all(GTK_WIDGET(oscOptsFrame));
	else gtk_widget_hide(GTK_WIDGET(oscOptsFrame));
	if(devType==PIC12||devType==PIC16||devType==PIC18)	//program ID
		gtk_widget_show_all(GTK_WIDGET(writeIDBKCalToggle));
	else gtk_widget_hide(GTK_WIDGET(writeIDBKCalToggle));
	if(devType==PIC16)	//Program Calib
		gtk_widget_show_all(GTK_WIDGET(writeCalib12Toggle));
	else gtk_widget_hide(GTK_WIDGET(writeCalib12Toggle));
	if(devType==PIC12||devType==PIC16||devType==PIC18){	//Force config
		gtk_widget_show_all(GTK_WIDGET(configWOptsFrame));
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
		gtk_widget_hide(GTK_WIDGET(configWOptsFrame));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configForceToggle),FALSE);
	}
	gtk_statusbar_push(GTK_STATUSBAR(status_bar),statusID,dev);
}
///
///Filter device list according to type selected
void FilterDevType(GtkWidget *widget,GtkWidget *window)
{
	char *selGroupName = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(devTypeCombo));
	const char *filtText = gtk_entry_get_text(GTK_ENTRY(devFiltEntry));
	Debug2("FilterDevType():group='%s',filt='%s'\n",selGroupName,filtText);
	enum group_t selGroup = -1;
	for (int i=0; i<NUM_GROUPS; i++) {
		if (strcmp(selGroupName, groupNames[i]) == 0) {
			selGroup = i;
			i = NUM_GROUPS;
		}
	}
	// If no specific group selected, ALL should be selected
	if (selGroup == -1 && strcmp(GROUP_ALL, selGroupName)) {
		PrintMessage1("ERR: group name '%s' invalid", selGroupName);
		return;
	}
	Debug2("group %d (%s) selected\n", selGroup, selGroup == -1 ? GROUP_ALL : groupNames[selGroup]);
	AddDevices(selGroup, filtText);
	g_free(selGroupName);
	onDevSel_Changed(NULL, NULL);
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
	GetSelectedDevice();
	gtk_widget_destroy(window);
}

/// Walk the TreeModel until we find an entry with the passed device name
/// Select that entry and then stop walking
gboolean selectDev_ForeachFunc(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, char *devNameToSelect) {
	char *thisEntryDevName;
	gtk_tree_model_get(model, iter, DEVICE_NAME_COLUMN, &thisEntryDevName, -1);
	bool matched = (strcmp(thisEntryDevName, devNameToSelect) == 0);
	if (matched) gtk_tree_selection_select_iter(devSel, iter);
	g_free(thisEntryDevName);
	return matched;
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

void buildMainToolbar() {
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
		gtk_image_new_from_icon_name("help-about", GTK_ICON_SIZE_BUTTON), strings[I_Info]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(infoToolItem), strings[I_Info]);
	g_signal_connect(G_OBJECT(infoToolItem), "clicked", G_CALLBACK(info),NULL);

	toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar),GTK_TOOLBAR_ICONS);

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
}

GtkWidget * buildDeviceTab() {
	GtkWidget *devBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);

	//*********Device tree******
	GtkWidget *devScroll = gtk_scrolled_window_new(NULL,NULL);
  	devTree = gtk_tree_view_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(devTree),
		-1, strings[I_Dev], gtk_cell_renderer_text_new(), "text", DEVICE_NAME_COLUMN, NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(devTree),
		-1, strings[I_Type], gtk_cell_renderer_text_new(), "text", DEVICE_GROUP_COLUMN, NULL);

	// AddDevices() gets called when an entry in devTypeCombo is selected during init
	gtk_container_add(GTK_CONTAINER(devScroll), devTree);

	devFrame = gtk_frame_new("");
	GtkWidget * devVboxDevSel = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	GtkWidget * devHboxDevType = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
	devTypeCombo = gtk_combo_box_text_new();
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(devTypeCombo), GROUP_ALL, GROUP_ALL);
	for (int i=0;i<NUM_GROUPS;i++)
		gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(devTypeCombo), groupNames[i], groupNames[i]);
	gtk_box_pack_start(GTK_BOX(devHboxDevType),gtk_label_new(strings[I_FiltType]),FALSE,TRUE,0); // "Filter by type"
	gtk_box_pack_start(GTK_BOX(devHboxDevType),devTypeCombo,FALSE,TRUE,0);

	GtkWidget * devHboxDevFilt = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
	devFiltEntry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(devHboxDevFilt),gtk_label_new(strings[I_FiltDev]),FALSE,FALSE,0); // "Filter by name"
	gtk_box_pack_start(GTK_BOX(devHboxDevFilt),devFiltEntry,TRUE,TRUE,0);

	gtk_box_pack_start(GTK_BOX(devVboxDevSel),devHboxDevType,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(devVboxDevSel),devHboxDevFilt,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(devVboxDevSel),devScroll,TRUE,TRUE,0);

	GtkWidget * devGrid = gtk_grid_new();
	GtkWidget * devHboxDevInfo = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	devInfoLabel = gtk_label_new("i");
	gtk_box_pack_start(GTK_BOX(devHboxDevInfo),gtk_label_new("info: "),FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(devHboxDevInfo),devInfoLabel,FALSE,FALSE,0);
	gtk_grid_attach(GTK_GRID(devGrid),devHboxDevInfo,0,0,2,1);

	eepromRWToggle = gtk_check_button_new_with_label(strings[I_EE]);	//"Read and write EEPROM"
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(eepromRWToggle),TRUE);
	gtk_grid_attach(GTK_GRID(devGrid),eepromRWToggle,0,1,2,1);

	// ----- Begin PIC options
	picOptsBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);	//"PIC configuration"
	gtk_grid_attach(GTK_GRID(devGrid),picOptsBox,0,2,2,1);

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
	
	oscOptsFrame = gtk_frame_new(strings[I_OSCW]);	//"Write OscCal"
	gtk_grid_attach(GTK_GRID(picGrid),oscOptsFrame,0,1,1,1);
	GtkWidget * oscOptsVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_container_add(GTK_CONTAINER(oscOptsFrame),GTK_WIDGET(oscOptsVbox));
	useOSCCALToggle = gtk_radio_button_new_with_label(NULL,strings[I_OSC]);	//"OSCCal"
	useBKOSCCALToggle = gtk_radio_button_new_with_label(\
		gtk_radio_button_get_group(GTK_RADIO_BUTTON(useOSCCALToggle)),strings[I_BKOSC]);	//"Backup OSCCal"
	useFileCalToggle = gtk_radio_button_new_with_label(\
		gtk_radio_button_get_group(GTK_RADIO_BUTTON(useOSCCALToggle)),strings[I_OSCF]);	//"From file"
	gtk_container_add(GTK_CONTAINER(oscOptsVbox),GTK_WIDGET(useOSCCALToggle));
	gtk_container_add(GTK_CONTAINER(oscOptsVbox),GTK_WIDGET(useBKOSCCALToggle));
	gtk_container_add(GTK_CONTAINER(oscOptsVbox),GTK_WIDGET(useFileCalToggle));

	icdOptsFrame = gtk_frame_new("ICD");
	GtkWidget * icdOptsHbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,25);
	icdCheckToggle = gtk_check_button_new_with_label(strings[I_ICD_ENABLE]);	//"Enable ICD"
	icdAddrEntry = gtk_entry_new();
	gtk_entry_set_width_chars(GTK_ENTRY(icdAddrEntry),4);
	GtkWidget * icdOptsHbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_box_pack_start(GTK_BOX(icdOptsHbox2),GTK_WIDGET(gtk_label_new(strings[I_ICD_ADDRESS])),0,0,1); //"ICD routine address"
	gtk_box_pack_start(GTK_BOX(icdOptsHbox2),GTK_WIDGET(icdAddrEntry),0,0,2);
	gtk_container_add(GTK_CONTAINER(icdOptsHbox1),icdCheckToggle);
	gtk_container_add(GTK_CONTAINER(icdOptsHbox1),GTK_WIDGET(icdOptsHbox2));
	gtk_container_add(GTK_CONTAINER(icdOptsFrame),GTK_WIDGET(icdOptsHbox1));
	gtk_grid_attach(GTK_GRID(picGrid),icdOptsFrame,0,2,2,1);

	configWOptsFrame = gtk_frame_new("Config Word");
	gtk_grid_attach(GTK_GRID(picGrid),configWOptsFrame,1,0,1,2);
	GtkWidget * cwGrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(configWOptsFrame),GTK_WIDGET(cwGrid));
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
	// ----- End PIC options

	avrOptsBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);	//Atmel configuration
	gtk_grid_attach(GTK_GRID(devGrid),avrOptsBox,0,3,2,1);

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

	gtk_container_add(GTK_CONTAINER(devFrame), devGrid);
	gtk_box_pack_start(GTK_BOX(devBox),devVboxDevSel,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(devBox),devFrame,TRUE,TRUE,0);
	return devBox;
}

GtkWidget * buildOptionsTab() {
	GtkWidget * optGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(optGrid), 5);
	gtk_grid_set_row_spacing(GTK_GRID(optGrid), 5);
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
	return optGrid;
}

GtkWidget * buildI2CTab() {
	GtkWidget * i2cGrid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(i2cGrid),5);
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
	return i2cGrid;
}

GtkWidget * buildIOTab() {
	GtkWidget * ioVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
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
	return ioVbox;
}

GtkWidget * buildUtilTab() {
	GtkWidget * utVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
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
	hexSaveBtn = gtk_button_new_with_label(strings[I_Fsave]);
	gtk_grid_attach(GTK_GRID(utGrid),hexSaveBtn,0,3,1,1);
	return utVbox;
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

	buildMainToolbar();
	gtk_box_pack_start(GTK_BOX(mainVbox),toolbar,FALSE,FALSE,0);
	
//------tab widget-------------
	notebook = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(mainVbox),notebook,FALSE,FALSE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),buildDeviceTab(),gtk_label_new(strings[I_Dev])); //"Device"
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),buildOptionsTab(),gtk_label_new(strings[I_Opt])); //"Options"
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),buildI2CTab(),gtk_label_new("I2C/SPI"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),buildIOTab(),gtk_label_new("I/O"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),buildUtilTab(),gtk_label_new("Utility"));

//------logging window
	data_scroll = gtk_scrolled_window_new(NULL,NULL);
	data = gtk_text_view_new();
	dataBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(data),FALSE);
	gtk_container_add(GTK_CONTAINER(data_scroll),data);
	gtk_style_context_add_class(gtk_widget_get_style_context(GTK_WIDGET(data)), "mono");
	gtk_box_pack_start(GTK_BOX(mainVbox),data_scroll,TRUE,TRUE,0);

//------status bar-------------
	status_bar = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(mainVbox),status_bar,FALSE,TRUE,0);
	statusID=gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar),"ID");

//------event handlers
	g_signal_connect(G_OBJECT(testHWBtn),"clicked",G_CALLBACK(TestHw),window);
	g_signal_connect(G_OBJECT(connectBtn),"clicked",G_CALLBACK(Connect),window);
	g_signal_connect(G_OBJECT(i2cRcvBtn),"clicked",G_CALLBACK(I2cspiR),window);
	g_signal_connect(G_OBJECT(i2cSendBtn),"clicked",G_CALLBACK(I2cspiS),window);
	g_signal_connect(G_OBJECT(devTypeCombo),"changed",G_CALLBACK(FilterDevType),NULL);
	g_signal_connect(G_OBJECT(devFiltEntry),"changed",G_CALLBACK(FilterDevType),NULL);
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
	for(int i=0;i<0x8400;i++) memCODE_W[i]=0x3fff;
	strncpy(LogFileName,strings[S_LogFile],sizeof(LogFileName));
	// These will trigger AddDevices to populate the device tree
	if (strlen(dev)>0) {
		struct DevInfo info = GetDevInfo(dev);
		gtk_combo_box_set_active_id(GTK_COMBO_BOX(devTypeCombo), groupNames[info.group]);
	}
	else {
		gtk_combo_box_set_active_id(GTK_COMBO_BOX(devTypeCombo), GROUP_ALL);
	}
	DeviceDetected=SearchDevice(&vid, &pid, false);
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

///Add devices to the device ListStore (which may not have been created)
///groupFilter: add devices in this group (-1 for all)
///textFilter: only add devices containing this string (NULL for all)
void AddDevices(enum group_t groupFilter, const char *textFilter) {
	Debug2("AddDevices(%d,%s) start\n", groupFilter, textFilter?textFilter:"(null)");
	if (GTK_IS_TREE_SELECTION(devSel))
		g_signal_handlers_disconnect_by_func(G_OBJECT(devSel),G_CALLBACK(onDevSel_Changed),NULL);

	if (!GTK_IS_LIST_STORE(devStore)) {
		devStore = gtk_list_store_new (DEVICE_N_COLUMNS,
	  						  G_TYPE_UINT,
                              G_TYPE_STRING,
                              G_TYPE_STRING);

		gtk_tree_view_set_model(GTK_TREE_VIEW(devTree), GTK_TREE_MODEL(devStore));
		
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(devStore), SORT_STRING_NAME,
			sortIterCompareFunc, GINT_TO_POINTER(SORT_STRING_NAME), NULL);
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(devStore), SORT_STRING_GROUP,
			sortIterCompareFunc, GINT_TO_POINTER(SORT_STRING_GROUP), NULL);
	 	gtk_tree_view_column_set_sort_column_id(
			gtk_tree_view_get_column(GTK_TREE_VIEW(devTree), 0), SORT_STRING_NAME);
		gtk_tree_view_column_set_sort_column_id(
			gtk_tree_view_get_column(GTK_TREE_VIEW(devTree), 1), SORT_STRING_GROUP);
		g_object_unref (G_OBJECT(devStore));
	}
	else gtk_list_store_clear(devStore);

	int i,j=0;
	char *devices=0,*tok;
	for(i=0;i<NDEVLIST;i++) {
		if(devices) free(devices);
		devices=malloc(strlen(DEVLIST[i].device)+1);
		strcpy(devices,DEVLIST[i].device);
		struct DevInfo info;
		populateDevInfo(&info, &(DEVLIST[i]));
		Debug1("devs=%s\n",devices);
		for(tok=strtok(devices,",");tok;tok=strtok(NULL,",")) {
			info.device=malloc(strlen(tok)+1);
			strcpy(info.device,tok);
			info.group=nameToGroup(tok);
			if ((!textFilter || strlen(textFilter) == 0 || strcasestr(tok, textFilter)) &&
				(groupFilter == -1 || info.group == groupFilter)) {
					gtk_list_store_insert_with_values(devStore, NULL, -1,
						DEVICE_ID_COLUMN, j++,
						DEVICE_NAME_COLUMN, tok,
						DEVICE_GROUP_COLUMN, groupNames[info.group], -1);
					Debug1("inserted %s\n", tok);
			}
		}
	}
	free(devices);

	if(GTK_IS_TREE_SELECTION(devSel)) {
		gtk_tree_selection_unselect_all(devSel);
	}
	else {
		devSel = gtk_tree_view_get_selection(GTK_TREE_VIEW(devTree));
		gtk_tree_selection_set_mode(devSel, GTK_SELECTION_SINGLE);
	}
	
	g_signal_connect(G_OBJECT(devSel),"changed",G_CALLBACK(onDevSel_Changed),NULL);
	gtk_tree_model_foreach(GTK_TREE_MODEL(devStore),
		(GtkTreeModelForeachFunc)selectDev_ForeachFunc,
	 	dev);
	
	Debug2("AddDevices(%d,%s) end\n", groupFilter, textFilter?textFilter:"(null)");
}

int sortIterCompareFunc(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer userdata) {
	enum sort_type_t sortcol = GPOINTER_TO_INT(userdata);
	int ret = 0;
	enum sort_data_type_t sortDataType;
	int dataCol;
	switch (sortcol) {
		case SORT_STRING_NAME:
			sortDataType = SDT_STRING;
			dataCol = DEVICE_NAME_COLUMN;
			break;
		case SORT_STRING_GROUP:
			sortDataType = SDT_STRING;
			dataCol = DEVICE_GROUP_COLUMN;
			break;
		default:
			Debug1("invalid sortcol '%d'",sortcol)
			return 0;
	}
	switch (sortDataType) {
		case SDT_STRING:
			char *nameA, *nameB;
			gtk_tree_model_get(model, a, dataCol, &nameA, -1);
			gtk_tree_model_get(model, b, dataCol, &nameB, -1);
			if (nameA == NULL || nameB == NULL) {
				if (nameA == NULL && nameB == NULL) break; // Both null. return 0 (no need to free)
				ret = (nameA == NULL) ? -1 : 1;
			}
			else {
				ret = g_utf8_collate(nameA,nameB);
			}
			Debug3("%s-%s-%d\n",nameA,nameB,ret);
			g_free(nameA);
			g_free(nameB);
			break;
		default:
			g_return_val_if_reached(0);
	}
	return ret;
}