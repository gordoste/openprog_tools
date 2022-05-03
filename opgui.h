#ifndef OPGUI_H
#define OPGUI_H

#define _APPNAME "OPGUI"

#include <gtk/gtk.h>

#include "deviceRW.h"

extern int StatusID;
extern GtkWidget *status_bar;

void onActivate(GtkApplication *_app, gpointer user_data);
void onShutdown(GtkApplication *_app, gpointer user_data);

void Connect(GtkWidget *widget,GtkWidget *window);
void I2cspiR();
void I2cspiS();
void ProgID();
void ShowContext();
void TestHw();
int CheckS1();
void AddDevices(enum group_t groupFilter, char *textFilter);
void GetSelectedDevice();

enum
{
  DEVICE_ID_COLUMN = 0,
  DEVICE_NAME_COLUMN,
  DEVICE_FAMILY_COLUMN,
  DEVICE_N_COLUMNS
};

#endif
