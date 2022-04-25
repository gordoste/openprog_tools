#ifndef OPGUI_H
#define OPGUI_H

#define _APPNAME "OPGUI"

#include <gtk/gtk.h>

extern int StatusID;
extern GtkWidget *status_bar;

void Connect(GtkWidget *widget,GtkWidget *window);
void I2cspiR();
void I2cspiS();
void ProgID();
void ShowContext();
int FindDevice(int vid,int pid);
void TestHw();
int CheckS1();
void AddDevices();

#endif
