#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main (int argc, char **argv) {
	int i,s=0,t;
	for (i = 1; i < argc; i++){
		sscanf(argv[i], "%X", &t);
		s+=t;
	}
	printf("checksum=%02X\n",(-s)&0xff);
	return 0;
}

