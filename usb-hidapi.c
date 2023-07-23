#include "usb-hidapi.h"

#include <stdbool.h>
#include <stdio.h>

#include "hidapi.h"

#define USB_TIMEOUT 50 // timeout for usb reads
#define MAX_STR 255

hid_device *device;

unsigned char *bufferI, *bufferU;
unsigned char bufferI0[128],bufferU0[128];

// _info: If specified, prints device manufacturer & product strings to stdout
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
    if (_info) {
        wchar_t wstr[MAX_STR];
        int res = hid_get_manufacturer_string(device, wstr, MAX_STR);
	    if (res < 0) { PrintMessage("Cannot read manufacturer string\n"); }
        else { wprintf(L"Manufacturer String: %ls\n", wstr); }
    	res = hid_get_product_string(device, wstr, MAX_STR);
        if (res < 0) { PrintMessage("Cannot read product string\n"); }
        else { wprintf(L"Product String: %ls\n", wstr); }
    }
    return TRUE;
};

void PacketIO(double delay) {
	if(saveLog&&logfile) fprintf(logfile,"PacketIO(%.2f)\n",delay);
    
	double delay0 = delay;

    delay-=USB_TIMEOUT-10;	//shorter delays are covered by 50ms timeout
	if(delay<MIN_DELAY) delay=MIN_DELAY;

#if !defined _WIN32 && !defined __CYGWIN__	//Linux
	uint64_t start,stop;
	struct timespec ts;
	clock_gettime( CLOCK_REALTIME, &ts );
	start=ts.tv_nsec/1000;
#else // Windows
	__int64 start,stop,freq,timeout;
	QueryPerformanceCounter((LARGE_INTEGER *)&start);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
#endif

    int res;
    bufferU0[0] = 0; // Report ID
    res = hid_write(device, bufferU0, DIMBUF);
    if (res == -1) {
        PrintMessage("Write failed\n");
        hid_close(device);
        return;
    }

    usleep(delay*1000);
    bufferI0[0] = 0; // Report ID
    res = hid_read_timeout(device, bufferI0, DIMBUF+1, USB_TIMEOUT); // +1 byte for report ID
    if (res == -1) {
        PrintMessage("Read failed\n");
        hid_close(device);
        return;
    }
	if (saveLog && logfile) {
		fprintf(logfile,"bufferU=[");
		for(int i=0;i<DIMBUF;i++){
			if(i%32==0) fprintf(logfile,"\n");
			fprintf(logfile,"%02X ",bufferU[i]);
		}
		fprintf(logfile,"]\n");
	}
    if (res == 0) {
        PrintMessage("No data read within timeout\n");
    }
    else {
		if (saveLog && logfile) {
			fprintf(logfile,"bufferI=[");
			for(int i=0;i<DIMBUF;i++){
				if(i%32==0) fprintf(logfile,"\n");
				fprintf(logfile,"%02X ",bufferI[i]);
			}
			fprintf(logfile,"]\n");
		}
	}
#if !defined _WIN32 && !defined __CYGWIN__	//Linux
		clock_gettime( CLOCK_REALTIME, &ts );
		stop  = ts.tv_nsec / 1000;
		if(saveLog&&logfile) fprintf(logfile,"T=%.2f ms (%+.2f ms)\n",(stop-start)/1000.0,(stop-start)/1000.0-delay0);
#else	// Windows
		QueryPerformanceCounter((LARGE_INTEGER *)&stop);
		if(saveLog&&logfile) fprintf(logfile,"T=%.2f ms (%+.2f ms)\n",(stop-start)*1000.0/freq,(stop-start)*1000.0/freq-delay0);
#endif
    return;
    
};
