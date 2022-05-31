#include "usb-hidapi.h"

#include <stdbool.h>
#include <stdio.h>

#include "hidapi.h"

#define USB_TIMEOUT 50 // timeout for usb reads

hid_device *device;

unsigned char *bufferI, *bufferU;
unsigned char bufferI0[128],bufferU0[128];

int FindDevice(int vid,int pid, bool _info) {
    bufferI = bufferI0; // hidapi puts the data where you point to
    bufferU = bufferU0+1; // hidapi expects byte 0 to be the report ID and data to start at byte 1
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

    Debug0("-> ");
    for(int i=1;i<DIMBUF+1;i++) Debug1("%02X ",(unsigned char)bufferU0[i]);
    Debug0("\n");

    int res;
    bufferU0[0] = 0; // Report ID
    res = hid_write(device, bufferU0, DIMBUF);
    if (res == -1) {
        fprintf(stderr, "Write failed\n");
        hid_close(device);
        return;
    }

    usleep(delay*1000);
    bufferI0[0] = 0; // Report ID
    res = hid_read_timeout(device, bufferI0, DIMBUF+1, USB_TIMEOUT); // +1 byte for report ID
    if (res == -1) {
        fprintf(stderr, "Read failed\n");
        hid_close(device);
        return;
    }
    if (res == 0) {
        fprintf(stderr, "No data read within timeout\n");
    }
    else {
        Debug0("<- ");
        for(int i=0;i<DIMBUF+1;i++) Debug1("%02X ",(unsigned char)bufferI0[i]);
        Debug0("\n");
    }
    return;
    
};
