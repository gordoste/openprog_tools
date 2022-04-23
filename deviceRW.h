extern char* devices[];
extern int Ndevices;
void Write(char* dev,int ee);
void Read(char* dev,int ee,int r);
enum family {PIC12,PIC16,PIC18,PIC24,PIC32,AVR,I2CEE,SPIEE,UWEE,OWEE,UNIOEE};
int GetDevType(const char* device);
char* ListDevices();

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
