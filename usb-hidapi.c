#include "usb-hidapi.h"

#include <stdbool.h>
#include <stdio.h>

#include "hidapi.h"

#define USB_TIMEOUT 50 // timeout for usb reads

hid_device *device;

unsigned char *bufferI, *bufferU;
unsigned char bufferI0[128],bufferU0[128];

int FindDevice(int vid,int pid, bool _info) {
    bufferI = bufferI0+1;
    bufferU = bufferU0+1;
    bufferI0[0] = 0;
    bufferU0[0] = 0;

    if (!(device = hid_open(vid, pid, NULL))) {
		PrintMessage(strings[S_noprog]);	//"Programmer not detected\r\n"
        return FALSE;
    }
	PrintMessage(strings[S_prog]);	//"Programmer detected\r\n");
    return TRUE;
};

void PacketIO(double delay) {
	if(saveLog&&logfile) fprintf(logfile,"PacketIO(%.2f)\n",delay);
    
    delay-=USB_TIMEOUT-10;	//shorter delays are covered by 50ms timeout
	if(delay<MIN_DELAY) delay=MIN_DELAY;
    // write bytes to usb, no Block
    // wait for delay
    // read bytes blocking timeout 50ms
    
};
