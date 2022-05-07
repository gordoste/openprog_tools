#ifndef ICDGUI_H
#define ICDGUI_H

#define _APPNAME "ICDGUI"

#include <gtk/gtk.h>

#include "deviceRW.h"

extern int StatusID;
extern GtkWidget *status_bar;

void onActivate(GtkApplication *_app, gpointer user_data);

int sortIterCompareFunc(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer userdata);

void Connect(GtkWidget *widget,GtkWidget *window);
void ProgID();
void ShowContext();

enum dev_column_t {
  DEVICE_ID_COLUMN = 0,
  DEVICE_NAME_COLUMN,
  DEVICE_GROUP_COLUMN,
  DEVICE_N_COLUMNS
};

enum sort_type_t {
  SORT_STRING_NAME = 0,
  SORT_STRING_GROUP
};

enum sort_data_type_t { SDT_STRING = 0 };

#endif
