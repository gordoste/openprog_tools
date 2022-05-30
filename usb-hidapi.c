#include "usb-hidapi.h"

#include <stdbool.h>
#include "hidapi.h"

hid_device *device;

int FindDevice(int vid,int pid, bool _info) {
    if (!(device = hid_open(vid, pid, NULL))) {
		PrintMessage(strings[S_noprog]);	//"Programmer not detected\r\n"
        return FALSE;
    }
	PrintMessage(strings[S_prog]);	//"Programmer detected\r\n");
    return TRUE;
};

void PacketIO(double delay) {

};
