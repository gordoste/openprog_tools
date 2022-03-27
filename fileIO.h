unsigned int htoi(const char *hex, int length);
void Save(char* dev,char* savefile);
int Load(char*dev,char*loadfile);
void SaveEE(char* dev,char* savefile);
int LoadEE(char*dev,char*loadfile);
void OpenLogFile(void);
void WriteLogIO();
void CloseLogFile();
