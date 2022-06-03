#ifndef _USB_H
#define _USB_H

#define MIN_DELAY 0

#include "common.h"

#include <stdbool.h>

extern char path[512];

#if !defined _WIN32 && !defined __CYGWIN__
	#include <linux/hiddev.h>
	#include <linux/hidraw.h>
#else
	#include <hidusage.h>
	#include <hidpi.h>
#endif


int FindDevice(int vid,int pid, bool _info);
void PacketIO(double delay);

#endif // #ifndef _USB_H
