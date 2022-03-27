/*
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


#include <hidapi.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>

hid_device *device;

int FindDevice();

unsigned char bufferU[129],bufferI[129];

int vid=0x1209,pid=0x5432,info=0;

int main (int argc, char **argv) {
#define L_IT 0
#define L_EN 1
	int d=0,v=0,q=0,r=1,lang=L_EN,c,i,j,block=0;
	char path[512]="";
	char buf[256];
	for(i=0;i<256;i++) buf[i]=0;
	if (hid_init()!=0) {
		fprintf(stderr, "Failed to init hidapi.\n");
		return 1;
	}

	int n=64; int t=2000;
	
	#if defined _WIN32
	int langID=GetUserDefaultLangID();
	if((langID&0xFF)==0x10) lang=L_IT;
	#else
	if(getenv("LANG")&&strstr(getenv("LANG"),"it")!=0) lang=L_IT;
	#endif
	opterr = 0;
	int option_index = 0;
	struct option long_options[] =
	{
		{"b",       no_argument,    &block, 1},
		{"block",   no_argument,    &block, 1},
		{"verbose", no_argument,        &v, 1},
		{"v",       no_argument,        &v, 1},
		{"quiet",   no_argument,        &q, 1},
		{"q",       no_argument,        &q, 1},
		{"info",    no_argument,     &info, 1},
		{"i",       no_argument,     &info, 1},
		{"help",    no_argument,       0, 'h'},
		{"timeout", required_argument, 0, 't'},
		{"path",    required_argument, 0, 'p'},
		{"read",    required_argument, 0, 'r'},
		{"delay",   required_argument, 0, 'd'},
		{0, 0, 0, 0}
	};
	while ((c = getopt_long_only (argc, argv, "n:d:p:hr:",long_options,&option_index)) != -1)
        switch (c)
           {
           case 'h':	//guida
             if(!lang) printf("hid_test [opzioni] [dati]\n"
				"opzioni: \n"
				"-b, block\tusa lettura bloccante\n"
				"-d, delay\tritardo lettura (ms) [0]\n"
				"-h, help\tguida\n"
				"-i, info\tinformazioni sul dispositivo [no]\n"
				"-n\t\tdimensione report [64]\n"
				"-p, path\tpercorso dispositivo [/dev/usb/hiddev0] \n"
				"-q, quiet\tmostra solo risposta [no]\n"
				"-r, read\tread N packets [1]\n"
				"-t, timeout\tread timeout(ms) [2000]\n"
				"-v, verbose\tmostra funzioni [no]\n"
				"es.  hid_test -i 1 2 3 4\n");
             else printf("hid_test [options] [data]\n"
				"Write 1 packet and read 1 or more packets\n"
			 	"options: \n"
				"-b, block\tuse blocking read\n"
				"-d, delay\tread delay (ms) [0]\n"
				"-h, help\thelp\n"
				"-i, info\tdevice info [no]\n"
				"-n\t\treport size [64]\n"
				"-p, path\tdevice path [/dev/usb/hiddev0]\n"
				"-q, quiet\tprint response only [no]\n"
				"-r, read\tread N packets [1]\n"
				"-t, timeout\tread timeout (ms) [2000]\n"
				"-v, verbose\tshow functions [no]\n"
				"e.g.  hid_test -i 1 2 3 4\n");
		exit(1);
            break;
            case 'n':	//dim report
             n = atoi(optarg);
             break;
           case 'd':	//ritardo lettura
             d = atoi(optarg);
             break;
			case 't':	// read timeout
				t = atoi(optarg);
				break;
			case 'p':	//percorso hiddev
             strncpy(path, optarg, sizeof(path));
             break;
            case 'r':	//ripeti lettura
             r = atoi(optarg);
             break;
			case '?':
             if (optopt == 'c')
               fprintf (stderr, "Option -%c requires an argument.\n", optopt);
             else if (isprint (optopt))
               fprintf (stderr, "Unknown option `-%c'.\n", optopt);
             else
               fprintf (stderr, "Unknown option character 0x%02x\n", optopt);
             return 1;
           default:
             //abort ();
            break;
           }

	for (j=0,i = optind; i < argc&&i<128; i++,j++) sscanf(argv[i], "%x", &buf[j]);
	for (;j<n;j++) buf[j]=0;

	int fd = -1;

	if(FindDevice()<0) exit(1);

	if(!q){
		printf("-> ");
	 	for(i=0;i<j;i++) printf("%02X ",(unsigned char)buf[i]);
		printf("\n");
	}

	bufferU[0] = 0; // Report ID
	memcpy(bufferU+1, buf, n);
	
	int res;
	res=hid_write(device, bufferU, n);
	if (res == -1) {
		fprintf(stderr, "Write failed\n");
		hid_close(device);
		hid_exit();
		return 1;
	}
	if (v) printf("Wrote %d bytes.\n", n);

	if (hid_set_nonblocking(device, block?0:1)) {
		fprintf(stderr, "Can't set nonblocking to %d\n", block?0:1);
	}
	if(v) printf("%slocking read\n",block?"B":"Non b");
	for (j=0; j<r; j++) {
		usleep(d*1000);
		bufferI[0] = 0; // Report ID
		if (block) {
			res = hid_read(device, bufferI, n+1); // +1 byte for report ID
		}
		else {
			res = hid_read_timeout(device, bufferI, n+1, t); // +1 byte for report ID
		}
		if (res == -1) {
			fprintf(stderr, "Read failed\n");
			hid_close(device);
			hid_exit();
			return 1;
		}
		if (res == 0) {
			printf("No data read within timeout.\n");
		}
		else {
			if (v) printf("Read %d bytes.\n", res);
			if (!q) {
				printf("<- ");
				for (i=0; i<n; i++) printf("%02X ", bufferI[i]);
				printf("\n");
			}
		}
	}
	return 0;
}


int FindDevice(){
	if (!(device = hid_open(vid, pid, NULL))) {
		printf("Can't find device\n");
		return -1;
	}
	printf("Device opened successfully.\n");

	return 0;
}
