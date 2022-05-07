/**
 * \file icdgui.c
 * interactive debugger for the open programmer
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

#include "icdgui.h"
#include "common.h"

#include "coff.h"
// #include "deviceRW.h"
#include "fileIO.h"
#include "icons.h"
#include "icd.h"

#define MAXLINES 600
#define  CONFIG_FILE "opgui.ini"
#define  CONFIG_DIR ".opgui"
#define MinDly 0

char** strings;	//!localized strings
int cmdline=0;
int saveLog=0;
FILE* logfile=0;
char appName[6]=_APPNAME;
char LogFileName[512]="";
char CoffFileName[512]="";
int vid=0x1209,pid=0x5432;
WORD *memCODE_W=0;
int size=0,sizeW=0,sizeEE=0,sizeCONFIG=0,sizeUSERID=0;
unsigned char *memCODE=0,*memEE=0,memID[64],memCONFIG[48],memUSERID[8];
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
GtkWidget *window, *button, *notebook, *status_bar;
GtkWidget *connectBtn, *logBtn;
GtkWidget *vidEntry, *pidEntry;
GtkWidget *icdStatusTextView, *icdSourceTextView;
GtkTextBuffer *icdStatusBuf, *icdSourceBuf;
GtkWidget *icdVBox, *icdPC_CheckMenuItem, *icdSTAT_CheckMenuItem, *icdBank0_CheckMenuItem, *icdBank1_CheckMenuItem, *icdBank2_CheckMenuItem, *icdBank3_CheckMenuItem, *icdEE_CheckMenuItem, *icdCommandEntry;

int statusID;
int ee = 0;
int readRes=0;

gchar *homedir, *fname;
int vid_ini,pid_ini;

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
///Print a message on the ICD data field
void PrintMessageICD(const char *msg){
	GtkTextIter iter;
	gtk_text_buffer_set_text(icdStatusBuf,msg,-1);
	gtk_text_buffer_get_start_iter(icdStatusBuf,&iter);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(icdStatusTextView),&iter,0.0,FALSE,0,0);
	while (gtk_events_pending ()) gtk_main_iteration();
}
///
///Append a message on the ICD data field
void AppendMessageICD(const char *msg){
	GtkTextIter iter;
	gtk_text_buffer_get_end_iter(icdStatusBuf,&iter);
	gtk_text_buffer_insert(icdStatusBuf,&iter,msg,-1);
	gtk_text_buffer_get_start_iter(icdStatusBuf,&iter);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(icdStatusTextView),&iter,0.0,FALSE,0,0);
	while (gtk_events_pending ()) gtk_main_iteration();
}
///
///Update option variables according to actual control values
void getOptions()
{
	vid=htoi(gtk_entry_get_text(GTK_ENTRY(vidEntry)),4);
	pid=htoi(gtk_entry_get_text(GTK_ENTRY(pidEntry)),4);
	saveLog = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(logBtn));
}
///
///Scroll source file
void scrollToLine(int line)
{
	GtkTextIter iter,iter2;
	gtk_text_buffer_get_end_iter(icdSourceBuf,&iter);
	if(line>0){
		gtk_text_iter_set_line(&iter,line-1);
		iter2=iter;
		gtk_text_iter_forward_char(&iter2);
		gtk_text_iter_forward_to_line_end(&iter2);
	}
	else{
		gtk_text_buffer_get_selection_bounds(icdSourceBuf,&iter,&iter2);
		iter2=iter;
	}
	gtk_text_buffer_select_range(icdSourceBuf,&iter,&iter2);
	while (gtk_events_pending ()) gtk_main_iteration();
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(icdSourceTextView),&iter,0.0,TRUE,0,0.5);
}
///
///Hilight line in source code
void SourceHilightLine(int line)
{
	GtkTextIter iter,iter2;
	GtkTextTag* tag;
	if(line>0){
		tag=gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(icdSourceBuf),"break_text");
		if(!tag) tag=gtk_text_buffer_create_tag(icdSourceBuf,"break_text","background","red", NULL);
		gtk_text_buffer_get_end_iter(icdSourceBuf,&iter);
		gtk_text_iter_set_line(&iter,line-1);
		iter2=iter;
		gtk_text_iter_forward_char(&iter2);
		gtk_text_iter_forward_to_line_end(&iter2);
		gtk_text_buffer_apply_tag (icdSourceBuf,tag,&iter,&iter2);
	}
	while (gtk_events_pending ()) gtk_main_iteration();
}
///
///Remove hilight line in source code
void SourceRemoveHilightLine(int line)
{
	GtkTextIter iter,iter2;
	if(line>0){
		gtk_text_buffer_get_end_iter(icdSourceBuf,&iter);
		gtk_text_iter_set_line(&iter,line-1);
		iter2=iter;
		gtk_text_iter_forward_char(&iter2);
		gtk_text_iter_forward_to_line_end(&iter2);
		gtk_text_buffer_remove_tag_by_name(icdSourceBuf,"break_text",&iter,&iter2);
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
	gtk_text_buffer_set_text(icdSourceBuf,g,-1);
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
		"comments", "A graphical debugger for the Open Programmer",
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
		"program-name", _APPNAME,
		//  "translator-credits"       gchar*                : Read / Write
		"version",VERSION,
		"website","www.openprog.altervista.org",
		//  "website-label"            gchar*                : Read / Write
  		"wrap-license",TRUE,
		"title","Info about ICDGUI",
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
		gtk_text_buffer_set_text(icdSourceBuf,tmp,-1);
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
		char* selection=gtk_text_buffer_get_text(icdSourceBuf,&iter,&iter2,FALSE);
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
		char* selection=gtk_text_buffer_get_text(icdStatusBuf,&iter,&iter2,FALSE);
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
///Close program
void Xclose(){
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
				if(sscanf(line,"vid %X",&X)>0) vid=X;
				else if(sscanf(line,"pid %X",&X)>0) pid=X;
			}
			fclose(f);
		}
	}
	vid_ini=vid;
	pid_ini=pid;
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
	int status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref(app);
	return status;
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

	logBtn = gtk_check_button_new_with_label(strings[I_LOG]);	//"Log activity"
	gtk_grid_attach(GTK_GRID(optGrid),logBtn,0,6,2,1);
	return optGrid;
}

GtkWidget * buildICDTab() {
	icdVBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);

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
		gtk_image_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON), strings[I_ICD_RUN]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(icdGoBtn), strings[I_ICD_RUN]);
	g_signal_connect(G_OBJECT(icdGoBtn), "clicked", G_CALLBACK(icdRun),NULL);

	GtkToolItem* icdHaltBtn = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("media-playback-pause", GTK_ICON_SIZE_BUTTON), strings[I_ICD_HALT]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(icdHaltBtn), strings[I_ICD_HALT]);
	g_signal_connect(G_OBJECT(icdHaltBtn), "clicked", G_CALLBACK(icdHalt),NULL);

	GtkToolItem* icdStepBtn = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("media-seek-forward", GTK_ICON_SIZE_BUTTON), strings[I_ICD_STEP]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(icdStepBtn), strings[I_ICD_STEP]);
	g_signal_connect(G_OBJECT(icdStepBtn), "clicked", G_CALLBACK(icdStep),NULL);

	GtkToolItem* icdStepoverBtn = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("media-skip-forward", GTK_ICON_SIZE_BUTTON), strings[I_ICD_STEPOVER]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(icdStepoverBtn), strings[I_ICD_STEPOVER]);
	g_signal_connect(G_OBJECT(icdStepoverBtn), "clicked", G_CALLBACK(icdStepOver),NULL);

	GtkToolItem* icdStopBtn = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("media-playback-stop", GTK_ICON_SIZE_BUTTON), strings[I_ICD_STOP]);
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
		gtk_image_new_from_icon_name("help-browser", GTK_ICON_SIZE_BUTTON), strings[I_ICD_HELP]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(icdHelpBtn), strings[I_ICD_HELP]);
	g_signal_connect(G_OBJECT(icdHelpBtn), "clicked", G_CALLBACK(ICDHelp),NULL);

	GtkToolItem* connectToolItem = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("network-wired", GTK_ICON_SIZE_BUTTON), strings[I_CONN]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(connectToolItem), strings[I_CONN]);
	g_signal_connect(G_OBJECT(connectToolItem), "clicked", G_CALLBACK(Connect),NULL);

	GtkToolItem* infoToolItem = gtk_tool_button_new(\
		gtk_image_new_from_icon_name("help-about", GTK_ICON_SIZE_BUTTON), strings[I_Info]);
	gtk_widget_set_tooltip_text(GTK_WIDGET(infoToolItem), strings[I_Info]);
	g_signal_connect(G_OBJECT(infoToolItem), "clicked", G_CALLBACK(info),NULL);

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
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar), GTK_TOOL_ITEM(connectToolItem), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(icdToolbar), GTK_TOOL_ITEM(infoToolItem), -1);

	GtkWidget * icdHpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(icdVBox),icdHpaned,TRUE,TRUE,0);
	gint width,height;
	gtk_window_get_size(GTK_WINDOW(window),&width,&height);
	gtk_paned_set_position(GTK_PANED (icdHpaned),width/2);
	//source
	GtkWidget * icdVbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	gtk_paned_pack1(GTK_PANED (icdHpaned), icdVbox2, TRUE, FALSE);
	gtk_box_pack_start(GTK_BOX(icdVbox2),gtk_label_new(strings[I_ICD_SOURCE]),FALSE,FALSE,0); //"Source"
	GtkWidget * sourceScroll = gtk_scrolled_window_new(NULL,NULL);
	icdSourceTextView = gtk_text_view_new();
	icdSourceBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(icdSourceTextView));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(icdSourceTextView),FALSE);
	gtk_container_add(GTK_CONTAINER(sourceScroll),icdSourceTextView);

	gtk_style_context_add_class(gtk_widget_get_style_context(GTK_WIDGET(icdSourceTextView)), "mono");

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
	icdStatusTextView = gtk_text_view_new();
	icdStatusBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(icdStatusTextView));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(icdStatusTextView),FALSE);
	gtk_container_add(GTK_CONTAINER(statusScroll),icdStatusTextView);

	gtk_style_context_add_class(gtk_widget_get_style_context(GTK_WIDGET(icdStatusTextView)), "mono");

	gtk_box_pack_start(GTK_BOX(icdVbox3),statusScroll,TRUE,TRUE,0);
	return icdVBox;
}

void onActivate(GtkApplication *_app, gpointer user_data) {
	window = gtk_application_window_new(app);
	sprintf(str,"icdgui v%s",VERSION);
	gtk_window_set_title(GTK_WINDOW(window),str);
	gtk_window_set_default_size(GTK_WINDOW(window), 750, 250);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_icon(GTK_WINDOW(window),gdk_pixbuf_new_from_resource("/openprog/icons/sys.png", NULL));
	GtkWidget * mainVbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_container_add(GTK_CONTAINER(window),mainVbox);

	GtkCssProvider *cssProv = gtk_css_provider_new();
	gtk_css_provider_load_from_resource(cssProv, "/openprog/css/style.css");
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProv), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

//------tab widget-------------
	notebook = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(mainVbox),notebook,TRUE,TRUE,0);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),buildICDTab(),gtk_label_new("ICD"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),buildOptionsTab(),gtk_label_new(strings[I_Opt])); //"Options"

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
	g_signal_connect(G_OBJECT(connectBtn),"clicked",G_CALLBACK(Connect),window);
	g_signal_connect(G_OBJECT(icdSourceTextView),"button_press_event",G_CALLBACK(source_mouse_event),NULL);
	g_signal_connect(G_OBJECT(icdStatusTextView),"button_press_event",G_CALLBACK(icdStatus_mouse_event),NULL);
	g_signal_connect(G_OBJECT(icdCommandEntry),"key_press_event",G_CALLBACK(icdCommand_key_event),NULL);
	g_signal_connect(G_OBJECT(icdVBox),"key_press_event",G_CALLBACK(icd_key_event),NULL);

	gtk_widget_show_all(window);

//********Init*************
	char text[16];
	sprintf(text,"%04X",vid);
	gtk_entry_set_text(GTK_ENTRY(vidEntry),text);
	sprintf(text,"%04X",pid);
	gtk_entry_set_text(GTK_ENTRY(pidEntry),text);
	sizeW=0x8400;
	memCODE_W=malloc(sizeW*sizeof(WORD));
	initVar();
	for(int i=0;i<0x8400;i++) memCODE_W[i]=0x3fff;
	strncpy(LogFileName,strings[S_LogFile],sizeof(LogFileName));
	DeviceDetected=SearchDevice(&vid, &pid, false);
	ProgID();		//get firmware version and reset
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

