extern char* devices[];
extern int Ndevices;
extern char *familyNames[];

void Write(char* dev,int ee);
void Read(char* dev,int ee,int r);
enum family {PIC12=0,PIC16,PIC18,PIC24,PIC32,AVR,I2CEE,SPIEE,UWEE,OWEE,UNIOEE,NUM_FAMILIES};
int GetDevType(const char* device);
char* ListDevices();

struct DEVICES {
	char *device;				//list of device names
	int family;					//type
	double HV;					//High voltage value (-1= turn off HV)
	int V33;					//3.3V regulator required (0=not required)
	void (*ReadPtr)();			//Read function pointer
	int ReadParam[4];			//Read function parameters; -10 = NU
	int ResArea;				//reserved area size
	void (*WritePtr)();			//Write function pointer
	int WriteParam[6];			//Write function parameters; -10 = NU
	double WriteParamD;			//Write function parameter, double
};

struct DevInfo{
	char *device;				//device name
	int type;					//type
	double HV;					//High voltage value (-1= turn off HV)
	int V33;					//3.3V regulator required (0=not required)
	int size;					//memory size
	int sizeEE;					//EE memory size
	char *features;				//device features string
};

struct DevInfo GetDevInfo(const char* dev);

extern int NDEVLIST;
extern struct DEVICES DEVLIST[];
