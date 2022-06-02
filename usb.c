#include "usb.h"

#if !defined _WIN32 && !defined __CYGWIN__	//Linux
	int fd = -1;
	struct hiddev_report_info rep_info_i,rep_info_u;
	struct hiddev_usage_ref_multi ref_multi_i,ref_multi_u;
	char path[512]="";
	unsigned char bufferU[128],bufferI[128];
#else	//Windows
	unsigned char bufferU0[128],bufferI0[128];
	unsigned char *bufferU,*bufferI;
	DWORD NumberOfBytesRead,BytesWritten;
	ULONG Result;
	HANDLE WriteHandle,ReadHandle;
	OVERLAPPED HIDOverlapped;
	HANDLE hEventObject;
#endif

///
///Find the USB peripheral with proper vid&pid code
/// return 0 if not found
int FindDevice(int vid,int pid,bool _info){
	int l_DeviceDetected = FALSE;
#if !defined _WIN32 && !defined __CYGWIN__	//Linux
	#ifndef hiddevIO	//use raw USB device
	struct hidraw_devinfo device_info;
	int i=-1;
	if(path[0]==0){	//search all devices
		if((fd = open("/dev/openprogrammer", O_RDWR|O_NONBLOCK))>0){ //try with this first
			sprintf(path,"/dev/openprogrammer");
			ioctl(fd, HIDIOCGRAWINFO, &device_info);
			if(device_info.vendor==vid&&device_info.product==pid) i=0;
			else{
				close(fd);
				i=-1;
			}
		}
		if(i){
			for(i=0;i<16;i++){
				sprintf(path,"/dev/hidraw%d",i);
				if((fd = open(path, O_RDWR|O_NONBLOCK))>0){
					ioctl(fd, HIDIOCGRAWINFO, &device_info);
					if(device_info.vendor==vid&&device_info.product==pid) break;
					else close(fd);
				}
			}
		}
		if(i==16){
			PrintMessage(strings[S_noprog]);
			path[0]=0;
			return 0;
		}
	}
	else{	//user supplied path
		if((fd = open(path, O_RDWR|O_NONBLOCK)) < 0) {
			PrintMessage1(strings[S_DevPermission],path); //"cannot open %s, make sure you have read permission on it",path);
			return 0;
		}
		ioctl(fd, HIDIOCGRAWINFO, &device_info);
		if(device_info.vendor!=vid||device_info.product!=pid){
			PrintMessage(strings[S_noprog]);
			return 0;
		}
	}
	printf(strings[S_progDev],path);
	#else		//use hiddev device (old method)
	struct hiddev_devinfo device_info;
	int i=-1;
	if(path[0]==0){	//search all devices
		if((fd = open("/dev/openprogrammer", O_RDONLY ))>0){ //try with this first
			ioctl(fd, HIDIOCGDEVINFO, &device_info);
			if(device_info.vendor==vid&&device_info.product==pid) i=0;
			else{
				close(fd);
				i=-1;
			}
		}
		if(i){
		for(i=0;i<16;i++){
			sprintf(path,"/dev/usb/hiddev%d",i);
			if((fd = open(path, O_RDONLY ))>0){
				ioctl(fd, HIDIOCGDEVINFO, &device_info);
				if(device_info.vendor==vid&&device_info.product==pid) break;
				else close(fd);
				}
			}
		}
		if(i==16){
			PrintMessage(strings[S_noprog]);
			path[0]=0;
			return 0;
		}
	}
	else{	//user supplied path
		if ((fd = open(path, O_RDONLY )) < 0) {
			PrintMessage1(strings[S_DevPermission],path); //"cannot open %s, make sure you have read permission on it",path);
			return 0;
		}
		ioctl(fd, HIDIOCGDEVINFO, &device_info);
		if(device_info.vendor!=vid||device_info.product!=pid){
			PrintMessage(strings[S_noprog]);
			return 0;
		}
	}
	printf(strings[S_progDev],path);
	MyDeviceDetected = TRUE;
	rep_info_u.report_type=HID_REPORT_TYPE_OUTPUT;
	rep_info_i.report_type=HID_REPORT_TYPE_INPUT;
	rep_info_u.report_id=rep_info_i.report_id=HID_REPORT_ID_FIRST;
	rep_info_u.num_fields=rep_info_i.num_fields=1;
	ref_multi_u.uref.report_type=HID_REPORT_TYPE_OUTPUT;
	ref_multi_i.uref.report_type=HID_REPORT_TYPE_INPUT;
	ref_multi_u.uref.report_id=ref_multi_i.uref.report_id=HID_REPORT_ID_FIRST;
	ref_multi_u.uref.field_index=ref_multi_i.uref.field_index=0;
	ref_multi_u.uref.usage_index=ref_multi_i.uref.usage_index=0;
	ref_multi_u.num_values=ref_multi_i.num_values=DIMBUF;
	#endif
	if(_info){
		struct hiddev_devinfo device_info;
		ioctl(fd, HIDIOCGDEVINFO, &device_info);
		printf(strings[L_INFO1],device_info.vendor, device_info.product, device_info.version);
		printf(strings[L_INFO2],device_info.busnum, device_info.devnum, device_info.ifnum);
		char name[256];
		strcpy(name,strings[L_UNKNOWN]);//"Unknown"
		if(ioctl(fd, HIDIOCGNAME(sizeof(name)), name) < 0) perror("evdev ioctl");
		printf(strings[L_NAME], path, name);//"The device on %s says its name is %s\n"
	}
#else		//Windows
	// On Windows, report ID (zero) must be put at the beginning of the buffer
	bufferI=bufferI0+1;
	bufferU=bufferU0+1;
	bufferI0[0]=0;
	bufferU0[0]=0;

	PSP_DEVICE_INTERFACE_DETAIL_DATA detailData;
	HANDLE DeviceHandle;
	HANDLE hDevInfo;
	GUID HidGuid;
	char MyDevicePathName[1024];
	ULONG Length;
	ULONG Required;
	typedef struct _HIDD_ATTRIBUTES {
	    ULONG   Size;
	    USHORT  VendorID;
	    USHORT  ProductID;
	    USHORT  VersionNumber;
	} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;
	typedef void (__stdcall*GETHIDGUID) (OUT LPGUID HidGuid);
	typedef BOOLEAN (__stdcall*GETATTRIBUTES)(IN HANDLE HidDeviceObject,OUT PHIDD_ATTRIBUTES Attributes);
	typedef BOOLEAN (__stdcall*SETNUMINPUTBUFFERS)(IN  HANDLE HidDeviceObject,OUT ULONG  NumberBuffers);
	typedef BOOLEAN (__stdcall*GETNUMINPUTBUFFERS)(IN  HANDLE HidDeviceObject,OUT PULONG  NumberBuffers);
	typedef BOOLEAN (__stdcall*GETFEATURE) (IN  HANDLE HidDeviceObject, OUT PVOID ReportBuffer, IN ULONG ReportBufferLength);
	typedef BOOLEAN (__stdcall*SETFEATURE) (IN  HANDLE HidDeviceObject, IN PVOID ReportBuffer, IN ULONG ReportBufferLength);
	typedef BOOLEAN (__stdcall*GETREPORT) (IN  HANDLE HidDeviceObject, OUT PVOID ReportBuffer, IN ULONG ReportBufferLength);
	typedef BOOLEAN (__stdcall*SETREPORT) (IN  HANDLE HidDeviceObject, IN PVOID ReportBuffer, IN ULONG ReportBufferLength);
	typedef BOOLEAN (__stdcall*GETMANUFACTURERSTRING) (IN  HANDLE HidDeviceObject, OUT PVOID ReportBuffer, IN ULONG ReportBufferLength);
	typedef BOOLEAN (__stdcall*GETPRODUCTSTRING) (IN  HANDLE HidDeviceObject, OUT PVOID ReportBuffer, IN ULONG ReportBufferLength);
	typedef BOOLEAN (__stdcall*GETINDEXEDSTRING) (IN  HANDLE HidDeviceObject, IN ULONG  StringIndex, OUT PVOID ReportBuffer, IN ULONG ReportBufferLength);
	HIDD_ATTRIBUTES Attributes;
	SP_DEVICE_INTERFACE_DATA devInfoData;
	int LastDevice = FALSE;
	int MemberIndex = 0;
	LONG Result;
	Length=0;
	detailData=NULL;
	DeviceHandle=NULL;
	HMODULE hHID=0;
	GETHIDGUID HidD_GetHidGuid=0;
	GETATTRIBUTES HidD_GetAttributes=0;
	SETNUMINPUTBUFFERS HidD_SetNumInputBuffers=0;
	GETNUMINPUTBUFFERS HidD_GetNumInputBuffers=0;
	GETFEATURE HidD_GetFeature=0;
	SETFEATURE HidD_SetFeature=0;
	GETREPORT HidD_GetInputReport=0;
	SETREPORT HidD_SetOutputReport=0;
	GETMANUFACTURERSTRING HidD_GetManufacturerString=0;
	GETPRODUCTSTRING HidD_GetProductString=0;
	hHID = LoadLibrary("hid.dll");
	if(!hHID){
		PrintMessage("Can't find hid.dll");
		return 0;
	}
	HidD_GetHidGuid=(GETHIDGUID)GetProcAddress(hHID,"HidD_GetHidGuid");
	HidD_GetAttributes=(GETATTRIBUTES)GetProcAddress(hHID,"HidD_GetAttributes");
	HidD_SetNumInputBuffers=(SETNUMINPUTBUFFERS)GetProcAddress(hHID,"HidD_SetNumInputBuffers");
	HidD_GetNumInputBuffers=(GETNUMINPUTBUFFERS)GetProcAddress(hHID,"HidD_GetNumInputBuffers");
	HidD_GetFeature=(GETFEATURE)GetProcAddress(hHID,"HidD_GetFeature");
	HidD_SetFeature=(SETFEATURE)GetProcAddress(hHID,"HidD_SetFeature");
	HidD_GetInputReport=(GETREPORT)GetProcAddress(hHID,"HidD_GetInputReport");
	HidD_SetOutputReport=(SETREPORT)GetProcAddress(hHID,"HidD_SetOutputReport");
	HidD_GetManufacturerString=(GETMANUFACTURERSTRING)GetProcAddress(hHID,"HidD_GetManufacturerString");
	HidD_GetProductString=(GETPRODUCTSTRING)GetProcAddress(hHID,"HidD_GetProductString");
	if(HidD_GetHidGuid==NULL\
		||HidD_GetAttributes==NULL\
		||HidD_GetFeature==NULL\
		||HidD_SetFeature==NULL\
		||HidD_GetInputReport==NULL\
		||HidD_SetOutputReport==NULL\
		||HidD_GetManufacturerString==NULL\
		||HidD_GetProductString==NULL\
		||HidD_SetNumInputBuffers==NULL\
		||HidD_GetNumInputBuffers==NULL) return 0;
	HMODULE hSAPI=0;
	hSAPI = LoadLibrary("setupapi.dll");
	if(!hSAPI){
		PrintMessage("Can't find setupapi.dll");
		return 0;
	}
	typedef HDEVINFO (WINAPI* SETUPDIGETCLASSDEVS) (CONST GUID*,PCSTR,HWND,DWORD);
	typedef BOOL (WINAPI* SETUPDIENUMDEVICEINTERFACES) (HDEVINFO,PSP_DEVINFO_DATA,CONST GUID*,DWORD,PSP_DEVICE_INTERFACE_DATA);
	typedef BOOL (WINAPI* SETUPDIGETDEVICEINTERFACEDETAIL) (HDEVINFO,PSP_DEVICE_INTERFACE_DATA,PSP_DEVICE_INTERFACE_DETAIL_DATA_A,DWORD,PDWORD,PSP_DEVINFO_DATA);
	typedef BOOL (WINAPI* SETUPDIDESTROYDEVICEINFOLIST) (HDEVINFO);
	SETUPDIGETCLASSDEVS SetupDiGetClassDevsA=0;
	SETUPDIENUMDEVICEINTERFACES SetupDiEnumDeviceInterfaces=0;
	SETUPDIGETDEVICEINTERFACEDETAIL SetupDiGetDeviceInterfaceDetailA=0;
	SETUPDIDESTROYDEVICEINFOLIST SetupDiDestroyDeviceInfoList=0;
	SetupDiGetClassDevsA=(SETUPDIGETCLASSDEVS) GetProcAddress(hSAPI,"SetupDiGetClassDevsA");
	SetupDiEnumDeviceInterfaces=(SETUPDIENUMDEVICEINTERFACES) GetProcAddress(hSAPI,"SetupDiEnumDeviceInterfaces");
	SetupDiGetDeviceInterfaceDetailA=(SETUPDIGETDEVICEINTERFACEDETAIL) GetProcAddress(hSAPI,"SetupDiGetDeviceInterfaceDetailA");
	SetupDiDestroyDeviceInfoList=(SETUPDIDESTROYDEVICEINFOLIST) GetProcAddress(hSAPI,"SetupDiDestroyDeviceInfoList");
	if(SetupDiGetClassDevsA==NULL\
		||SetupDiEnumDeviceInterfaces==NULL\
		||SetupDiDestroyDeviceInfoList==NULL\
		||SetupDiGetDeviceInterfaceDetailA==NULL) return 0;
	/*
	The following code is adapted from Usbhidio_vc6 application example by Jan Axelson
	for more information see see http://www.lvr.com/hidpage.htm
	*/
	/*
	API function: HidD_GetHidGuid
	Get the GUID for all system HIDs.
	Returns: the GUID in HidGuid.
	*/
	HidD_GetHidGuid(&HidGuid);
	/*
	API function: SetupDiGetClassDevs
	Returns: a handle to a device information set for all installed devices.
	Requires: the GUID returned by GetHidGuid.
	*/
	hDevInfo=SetupDiGetClassDevs(&HidGuid,NULL,NULL,DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
	devInfoData.cbSize = sizeof(devInfoData);
	//Step through the available devices looking for the one we want.
	//Quit on detecting the desired device or checking all available devices without success.
	MemberIndex = 0;
	LastDevice = FALSE;
	do
	{
		/*
		API function: SetupDiEnumDeviceInterfaces
		On return, MyDeviceInterfaceData contains the handle to a
		SP_DEVICE_INTERFACE_DATA structure for a detected device.
		Requires:
		The DeviceInfoSet returned in SetupDiGetClassDevs.
		The HidGuid returned in GetHidGuid.
		An index to specify a device.
		*/
		Result=SetupDiEnumDeviceInterfaces (hDevInfo, 0, &HidGuid, MemberIndex, &devInfoData);
		if (Result != 0)
		{
			//A device has been detected, so get more information about it.
			/*
			API function: SetupDiGetDeviceInterfaceDetail
			Returns: an SP_DEVICE_INTERFACE_DETAIL_DATA structure
			containing information about a device.
			To retrieve the information, call this function twice.
			The first time returns the size of the structure in Length.
			The second time returns a pointer to the data in DeviceInfoSet.
			Requires:
			A DeviceInfoSet returned by SetupDiGetClassDevs
			The SP_DEVICE_INTERFACE_DATA structure returned by SetupDiEnumDeviceInterfaces.
			The final parameter is an optional pointer to an SP_DEV_INFO_DATA structure.
			This application doesn't retrieve or use the structure.
			If retrieving the structure, set
			MyDeviceInfoData.cbSize = length of MyDeviceInfoData.
			and pass the structure's address.
			*/
			//Get the Length value.
			//The call will return with a "buffer too small" error which can be ignored.
			Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &Length, NULL);
			//Allocate memory for the hDevInfo structure, using the returned Length.
			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
			//Set cbSize in the detailData structure.
			detailData -> cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			//Call the function again, this time passing it the returned buffer size.
			Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, Length,&Required, NULL);
			// Open a handle to the device.
			// To enable retrieving information about a system mouse or keyboard,
			// don't request Read or Write access for this handle.
			/*
			API function: CreateFile
			Returns: a handle that enables reading and writing to the device.
			Requires:
			The DevicePath in the detailData structure
			returned by SetupDiGetDeviceInterfaceDetail.
			*/
			DeviceHandle=CreateFile(detailData->DevicePath,
				0, FILE_SHARE_READ|FILE_SHARE_WRITE,
				(LPSECURITY_ATTRIBUTES)NULL,OPEN_EXISTING, 0, NULL);
			/*
			API function: HidD_GetAttributes
			Requests information from the device.
			Requires: the handle returned by CreateFile.
			Returns: a HIDD_ATTRIBUTES structure containing
			the Vendor ID, Product ID, and Product Version Number.
			Use this information to decide if the detected device is
			the one we're looking for.
			*/
			//Set the Size to the number of bytes in the structure.
			Attributes.Size = sizeof(Attributes);
			Result = HidD_GetAttributes(DeviceHandle,&Attributes);
			//Is it the desired device?
			l_DeviceDetected = FALSE;
			if (Attributes.VendorID == vid)
			{
				if (Attributes.ProductID == pid)
				{
					//Both the Vendor ID and Product ID match.
					l_DeviceDetected = TRUE;
					strcpy(MyDevicePathName,detailData->DevicePath);
					// Get a handle for writing Output reports.
					WriteHandle=CreateFile(detailData->DevicePath,
						GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
						(LPSECURITY_ATTRIBUTES)NULL,OPEN_EXISTING,0,NULL);
					//Get a handle to the device for the overlapped ReadFiles.
					ReadHandle=CreateFile(detailData->DevicePath,
						GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,(LPSECURITY_ATTRIBUTES)NULL,
						OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
					if (hEventObject) CloseHandle(hEventObject);
					hEventObject = CreateEvent(NULL,TRUE,TRUE,"");
					//Set the members of the overlapped structure.
					HIDOverlapped.hEvent = hEventObject;
					HIDOverlapped.Offset = 0;
					HIDOverlapped.OffsetHigh = 0;
					Result=HidD_SetNumInputBuffers(DeviceHandle,64);
				}
				else
					//The Product ID doesn't match.
					CloseHandle(DeviceHandle);
			}
			else
				//The Vendor ID doesn't match.
				CloseHandle(DeviceHandle);
		//Free the memory used by the detailData structure (no longer needed).
		free(detailData);
		}
		else
			//SetupDiEnumDeviceInterfaces returned 0, so there are no more devices to check.
			LastDevice=TRUE;
		//If we haven't found the device yet, and haven't tried every available device,
		//try the next one.
		MemberIndex = MemberIndex + 1;
	} //do
	while ((LastDevice == FALSE) && (l_DeviceDetected == FALSE));
	//Free the memory reserved for hDevInfo by SetupDiClassDevs.
	SetupDiDestroyDeviceInfoList(hDevInfo);

	if(_info&&l_DeviceDetected == TRUE){
		char string[1024];
		PrintMessage3("Device detected: vid=0x%04X pid=0x%04X\nPath: %s\n",vid,pid,MyDevicePathName);
		if(HidD_GetManufacturerString(DeviceHandle,string,sizeof(string))==TRUE) wprintf(L"Manufacturer string: %s\n",string);
		if(HidD_GetProductString(DeviceHandle,string,sizeof(string))==TRUE) wprintf(L"Product string: %s\n",string);
	}
#endif
	if (l_DeviceDetected == FALSE){
		PrintMessage(strings[S_noprog]);	//"Programmer not detected\r\n"
	}
	else{
		PrintMessage(strings[S_prog]);	//"Programmer detected\r\n");
	}
	return l_DeviceDetected;
}

///
///Write data packet, wait for X milliseconds, read response
void PacketIO(double delay) {
	#define TIMEOUT 50
	if(saveLog&&logfile) fprintf(logfile,"PacketIO(%.2f)\n",delay);
	int delay0=delay;
#if !defined _WIN32 && !defined __CYGWIN__	//Linux
	struct timespec ts;
	uint64_t start,stop;
	fd_set set;
	struct timeval timeout;
	int rv,i;
	FD_ZERO(&set); /* clear the set */
	FD_SET(fd, &set); /* add our file descriptor to the set */
	timeout.tv_sec = 0;
	timeout.tv_usec = TIMEOUT*1000;
	clock_gettime( CLOCK_REALTIME, &ts );
	start=ts.tv_nsec/1000;
	delay-=TIMEOUT-10;	//shorter delays are covered by 50ms timeout
	if(delay<MIN_DELAY) delay=MIN_DELAY;
	#ifndef hiddevIO	//use raw USB device
	//wait before writing
/*	rv = select(fd + 1, NULL, &set, NULL, &timeout); //wait for write event
	if(rv == -1){
		PrintMessage(strings[S_ErrSing]);	//error
		if(saveLog&&logfile) fprintf(logfile,strings[S_ErrSing]);
		return;
	}
	else if(rv == 0){
		PrintMessage(strings[S_comTimeout]);	//"comm timeout\r\n"
		if(saveLog&&logfile) fprintf(logfile,strings[S_comTimeout]);
		return;
	}*/
	//write
	int res = write(fd,bufferU,DIMBUF);
	if (res < 0) {
		printf("Error: %d\n", errno);
		perror("write");
	}
	usleep((int)(delay*1000.0));
	//wait before reading
	rv = select(fd + 1, &set, NULL, NULL, &timeout); //wait for event
	if(rv == -1){
		PrintMessage(strings[S_ErrSing]);	/*error*/
		if(saveLog&&logfile) fprintf(logfile,strings[S_ErrSing]);
		return;
	}
	else if(rv == 0){
		PrintMessage(strings[S_comTimeout]);	/*"comm timeout\r\n"*/
		if(saveLog&&logfile) fprintf(logfile,strings[S_comTimeout]);
		return;
	}
	//read
	res = read(fd, bufferI, DIMBUF);
	if (res < 0) {
		perror("read");
	}
	#else		//use hiddev device (old method)
	struct hiddev_event ev[80];
	int n=DIMBUF;
	for(i=0;i<DIMBUF;i++) ref_multi_u.values[i]=bufferU[i];
	//write
	ioctl(fd, HIDIOCSUSAGES, &ref_multi_u);
	ioctl(fd,HIDIOCSREPORT, &rep_info_u);
	usleep((int)(delay*1000.0));
	//read
	rv = select(fd + 1, &set, NULL, NULL, &timeout); //wait for event
	if(rv == -1){
		PrintMessage(strings[S_ErrSing]);	/*error*/
		if(saveLog&&logfile) fprintf(logfile,strings[S_ErrSing]);
	}
	else if(rv == 0){
		PrintMessage(strings[S_comTimeout]);	/*"comm timeout\r\n"*/
		if(saveLog&&logfile) fprintf(logfile,strings[S_comTimeout]);
	}
	else{
	//		ioctl(fd, HIDIOCGUSAGES, &ref_multi_i);
	//		ioctl(fd,HIDIOCGREPORT, &rep_info_i);
	#undef read()
		rv=read(fd, ev,sizeof(struct hiddev_event) *n);
		for(i=0;(ev[0].value!=bufferU[0])&&i<40;i++){		//read too early; try again after 5ms
			msDelay(5);
			rv=read(fd, ev,sizeof(struct hiddev_event) *n);
			if(saveLog&&logfile) fprintf(logfile,"Packet not ready, wait extra time\n");
		}
		if(i==40) fprintf(logfile,"Cannot read correct packet!!\n");
		for(i=0;i<n;i++) bufferI[i]=ev[i].value&0xFF;
	}
	#endif
	clock_gettime( CLOCK_REALTIME, &ts );
	stop  = ts.tv_nsec / 1000;
	if(saveLog&&logfile){
		WriteLogIO();
		fprintf(logfile,"T=%.2f ms (%+.2f ms)\n",(stop-start)/1000.0,(stop-start)/1000.0-delay0);
		if(bufferU[0]!=bufferI[0]) fprintf(logfile,"Cannot read correct packet!!\n");
	}
#else	//Windows
	__int64 start,stop,freq,timeout;
	QueryPerformanceCounter((LARGE_INTEGER *)&start);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
	delay-=TIMEOUT-10;	//shorter delays are covered by 50ms timeout
	if(delay<MIN_DELAY) delay=MIN_DELAY;
	//write
	Result = WriteFile(WriteHandle,bufferU0,DIMBUF+1,&BytesWritten,NULL);
	QueryPerformanceCounter((LARGE_INTEGER *)&stop);
	timeout=stop+delay*freq/1000.0;
	while(stop<timeout) QueryPerformanceCounter((LARGE_INTEGER *)&stop);
	//read
	Result = ReadFile(ReadHandle,bufferI0,DIMBUF+1,&NumberOfBytesRead,(LPOVERLAPPED) &HIDOverlapped);
	Result = WaitForSingleObject(hEventObject,TIMEOUT);
	if(saveLog&&logfile) WriteLogIO();
	ResetEvent(hEventObject);
	if(Result!=WAIT_OBJECT_0){
		PrintMessage(strings[S_comTimeout]);	/*"comm timeout\r\n"*/
		if(saveLog&&logfile) fprintf(logfile,strings[S_comTimeout]);
	}
	QueryPerformanceCounter((LARGE_INTEGER *)&stop);
	if(saveLog&&logfile) fprintf(logfile,"T=%.2f ms (%+.2f ms)\n",(stop-start)*1000.0/freq,(stop-start)*1000.0/freq-delay0);
#endif
}
