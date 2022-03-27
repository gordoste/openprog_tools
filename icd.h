//General routines to communicate via ICD with a target

//The following commands are implemented in the debugger monitor
//routine which is written in the last memory page on the target chip.
#define VER 	1	//;version
#define STEP 	2	//;step
#define GO 		3	//;go
#define RREG 	4	//;read register
#define WREG 	5	//;write register
#define EEADR 0x10D
#define EEADRH 0x10F
#define EEDATA 0x10C
#define EEDATH 0x10E
#define EECON1 0x18C
#define EECON2 0x18D
#define w_temp 0x6B
#define status_temp 0x6C
#define pclath_temp 0x6D
#define fsr_temp 0x6E

extern struct var{	char* name;	int display;} variables[0x200];

//Prepare ICD interface by resetting the target with a power-up sequence.
//MCLR is low so the target is reset even if power is not supplied by the programmer.
//Set communication speed at 1/(2*Tck us)
void startICD(int Tck);

//Check whether the target is running or is executing the debug routine.
//This is signaled by RB7 (Data): D=1 -> debugger monitor running
int isRunning();

//Set the next breakpoint address, the freeze bit, 
//and continue execution.
//This is necessary because at every break 
//the ICD register is loaded with the last address.
void cont(int break_addr, int freeze);

//Execute a single step
void step();

//Remove reset so that the target can start executing its code.
void run();

//Get the debugger monitor version
int version();

//Halt execution by setting RB6 (Clock) low
void Halt();

//Read register at address addr
int ReadRegister(int addr);

//Read n registers starting at address addr
int ReadRegisterN(int addr,int n,int* buf);

//Write data at address addr
void WriteRegister(int addr,int data);

//Read program memory at address addr
int ReadProgMem(int addr);

//Read program memory at address addr
int ReadProgMemN(int addr,int n,int* buf);

//Read data memory at address addr
int ReadDataMem(int addr);

//Read data memory at address addr
int ReadDataMemN(int addr,int n,unsigned char* buf);

//Disassemble a command and return string
char* decodeCmd(int cmd,char *str, int addrH);

// get register name from list
char* getVar(int addr,char *var);
