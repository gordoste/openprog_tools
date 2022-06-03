#ifndef _USB_HIDAPI_H
#define _USB_HIDAPI_H

#define MIN_DELAY 0

#include "common.h"

int FindDevice(int vid,int pid, bool _info);
void PacketIO(double delay);

#endif // #ifndef _USB_HIDAPI_H