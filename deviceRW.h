#ifndef _DEVICERW_H
#define _DEVICERW_H

#include <stdbool.h>

extern char* devices[];
extern int Ndevices;
extern char *familyNames[];
extern char *groupNames[];
extern char *GROUP_ALL;

enum family_t {PIC12=0,PIC16,PIC18,PIC24,PIC32,AVR,I2CEE,SPIEE,UWEE,OWEE,UNIOEE,NUM_FAMILIES};
enum group_t {G_PIC_10_12=0,G_PIC_16,G_PIC_18,G_PIC_24,G_PIC_30_33,G_ATMEL,G_EEPROM,NUM_GROUPS};

// This struct stores information about how to program a set of devices
struct DEVICES {
	char *device;				//comma-separated list of device names which share the same method of programming
	enum family_t family;		//type
	double HV;					//High voltage value (-1= turn off HV)
	int V33;					//3.3V regulator required (0=not required)
	void (*ReadPtr)();			//Read function pointer
	int ReadParam[4];			//Read function parameters; -10 = NU
	int ResArea;				//reserved area size
	void (*WritePtr)();			//Write function pointer
	int WriteParam[6];			//Write function parameters; -10 = NU
	double WriteParamD;			//Write function parameter, double
};

// This struct stores information about a specific device
struct DevInfo{
	char *device;				//device name
	enum group_t group;			//device group (used for selection in GUI only) - see nameToGroup()
	enum family_t family;		//type
	double HV;					//High voltage value (-1= turn off HV)
	int V33;					//3.3V regulator required (0=not required)
	int size;					//memory size
	int sizeEE;					//EE memory size
	char *features;				//device features string
};

extern int NDEVLIST;
extern struct DEVICES DEVLIST[];

/// Determine the group of a specific device
enum group_t nameToGroup(const char *devName);

/// Copies the device info from the passed devlist entry into the passed DevInfo
/// Does NOT populate fields 'device' and 'group' as these are device-specific
void populateDevInfo(struct DevInfo *info, const struct DEVICES *devlistEntry);

///Search and return device info. Uses strtok (so don't call inside a strtok loop)
struct DevInfo GetDevInfo(const char* dev);

void Read(char* dev,int ee,int r);
void Write(char* dev,int ee);

#endif // _DEVICERW_H