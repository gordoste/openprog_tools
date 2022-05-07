// Dummy functions required to make icdgui compile (kludge)

// progP24.h
void Read24Fx(int dim,int dim2,int options,int appIDaddr,int executiveArea) {}
void Write24Fx(int dim,int dim2,int options,int appIDaddr,int rowSize, double wait) {}
void DisplayCODE24F(int dim) {}
void DisplayEE24F() {}
void Read24Ex(int dim,int dim2,int options,int appIDaddr,int executiveArea) {}
void Write24Ex(int dim,int dim2,int options,int appIDaddr,int rowSize, double wait) {}

// progP18.h
void Read18Fx(int dim,int dim2,int options) {}
void Read18FKx(int dim,int dim2,int options) {}
void Write18Fx(int dim,int dim2,int wbuf,int eraseW1,int eraseW2,int options) {}
void Write18FKx(int dim,int dim2,int options,int nu1,int nu2, int nu3) {}
void DisplayCODE18F(int dim) {}

// progP16.h
void Read16Fxxx(int dim,int dim2,int dim3,int vdd) {}
void Write12F6xx(int dim,int dim2) {}
void Write12F61x(int dim,int d,int d2) {}
void Write12F62x(int dim,int dim2) {}
void Write16F7x(int dim,int vdd) {}
void Write16F71x(int dim,int vdd) {}
void Write16F72x(int dim,int d,int d2) {}
void Write16F62x (int dim,int dim2) {}
void Write16F8x(int dim,int dim2) {}
void Write16F81x(int dim,int dim2) {}
void Write16F87x(int dim,int dim2) {}
void Write16F87xA(int dim,int dim2,int seq) {}
void Write16F88x(int dim,int dim2) {}
void Read16F1xxx(int dim,int dim2,int dim3,int options) {}
void Write16F1xxx(int dim,int dim2,int options) {}
void Read16F18xxx(int dim,int dim2,int dim3,int options) {}
void Write16F18xxx(int dim,int dim2,int options) {}
void DisplayCODE16F(int size) {}
void DisplayEE16F(int size) {}

// progP12.h
void Read12F5xx(int dim, int dim2) {}
void Write12F5xx(int dim,int OscAddr) {}
void Write12C5xx(int dim,int dummy) {}

// progEEPROM.h
void ReadI2C(int dim,int addr) {}
void WriteI2C(int dim,int addr,int page) {}
void Read93x(int dim,int na,int options) {}
void Write93Sx(int dim,int na,int page) {}
void Write93Cx(int dim,int na, int options) {}
void Read25xx(int dim) {}
void Write25xx(int dim,int options) {}
void ReadOneWireMem(int dim,int options) {}
void WriteOneWireMem(int dim,int options) {}
void ReadDS1820() {}
void Read11xx(int dim) {}
void Write11xx(int dim,int page) {}

// progAVR.h
void ReadAT(int dim, int dim2, int options) {}
void ReadAT_HV(int dim, int dim2, int options) {}
void WriteAT(int dim, int dim2, int dummy1, int dummy2) {}
void WriteATmega(int dim, int dim2, int page, int options) {}
void WriteAT_HV(int dim, int dim2, int page, int options) {}
void DisplayCODEAVR(int dim) {}
void WriteATfuseSlow(int fuse) {}
