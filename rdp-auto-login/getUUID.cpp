#include <WinBase.h>
#include <winioctl.h>

extern "C" {

  BOOL KernelIoControl(DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD );

 #define IOCTL_HAL_GET_DEVICE_INFO   CTL_CODE(FILE_DEVICE_HAL, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
 #define IOCTL_HAL_SEND_UDP          CTL_CODE(FILE_DEVICE_HAL, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)
 #define IOCTL_HAL_GET_IP_ADDR       CTL_CODE(FILE_DEVICE_HAL, 3, METHOD_BUFFERED, FILE_ANY_ACCESS)
 #define IOCTL_HAL_RADIO_CNTRL       CTL_CODE(FILE_DEVICE_HAL, 4, METHOD_BUFFERED, FILE_ANY_ACCESS)
 #define IOCTL_HAL_DDK_CALL          CTL_CODE(FILE_DEVICE_HAL, 5, METHOD_BUFFERED, FILE_ANY_ACCESS)

 #define IOCTL_EDBG_REGISTER_CLIENT  CTL_CODE(FILE_DEVICE_HAL, 6, METHOD_BUFFERED, FILE_ANY_ACCESS)
 #define IOCTL_EDBG_DEREGISTER_CLIENT CTL_CODE(FILE_DEVICE_HAL, 7, METHOD_BUFFERED, FILE_ANY_ACCESS)
 #define IOCTL_EDBG_REGISTER_DFLT_CLIENT CTL_CODE(FILE_DEVICE_HAL, 8, METHOD_BUFFERED, FILE_ANY_ACCESS)
 #define IOCTL_EDBG_SEND             CTL_CODE(FILE_DEVICE_HAL, 9, METHOD_BUFFERED, FILE_ANY_ACCESS)
 #define IOCTL_EDBG_RECV             CTL_CODE(FILE_DEVICE_HAL, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)
 #define IOCTL_EDBG_SET_DEBUG        CTL_CODE(FILE_DEVICE_HAL,11, METHOD_BUFFERED, FILE_ANY_ACCESS)

 #define IOCTL_SET_KERNEL_COMM_DEV   CTL_CODE(FILE_DEVICE_HAL, 12, METHOD_BUFFERED, FILE_ANY_ACCESS)
 #define IOCTL_HAL_GET_UUID          CTL_CODE(FILE_DEVICE_HAL, 13, METHOD_BUFFERED,FILE_ANY_ACCESS)

 #define IOCTL_HAL_INIT_RTC          CTL_CODE(FILE_DEVICE_HAL, 14, METHOD_BUFFERED, FILE_ANY_ACCESS)
 #define IOCTL_HAL_REBOOT            CTL_CODE(FILE_DEVICE_HAL, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)

 #define IOCTL_HAL_ENABLE_SCHEDULER_LOGGING CTL_CODE(FILE_DEVICE_HAL, 16, METHOD_BUFFERED, FILE_ANY_ACCESS)

 #define IOCTL_SET_KERNEL_DEV_PORT   CTL_CODE(FILE_DEVICE_HAL, 17, METHOD_BUFFERED, FILE_ANY_ACCESS)

 #define IOCTL_HAL_GET_DEVICEID      CTL_CODE(FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)

 typedef struct _DEVICE_ID {
  DWORD    dwSize;
     DWORD    dwPresetIDOffset;
  DWORD    dwPresetIDBytes;
     DWORD    dwPlatformIDOffset;
  DWORD    dwPlatformIDBytes;
 } DEVICE_ID, *PDEVICE_ID;

}

//Use it this way:
//  This works!
/*
 {
  DEVICE_ID DevInfo;
  DWORD  RetBytes;
  DevInfo.dwSize = sizeof(DEVICE_ID );
  BOOL bError;

//  KernelIoControl(IOCTL_HAL_REBOOT, NULL, 0, NULL, 0, NULL);
  bError = KernelIoControl ( IOCTL_HAL_GET_DEVICEID,
         NULL, 0,
         &DevInfo, sizeof(DEVICE_ID), &RetBytes);
 } 
*/

//#define IOCTL_HAL_GET_UUID CTL_CODE(FILE_DEVICE_HAL, 13,   METHOD_BUFFERED, FILE_ANY_ACCESS)

BOOL GetUUID (TCHAR* szClientHWID) 
{
  GUID myUUID;
  BOOL bRetVal;
  DWORD dwBytesReturned;

  bRetVal = KernelIoControl (IOCTL_HAL_GET_UUID, NULL, 0, &myUUID, 
                             sizeof (myUUID), &dwBytesReturned);
  if (!bRetVal) 
  {
    //MessageBox(NULL, L"KernelIoControl call failed!\r\n", L"getUUID", MB_OK | MB_TOPMOST | MB_SETFOREGROUND);
    DEBUGMSG(1, (L"KernelIoControl call failed!\r\n"));
    return FALSE;
  }

  TCHAR txt[MAX_PATH];
/*
	[HKEY_LOCAL_MACHINE\Software\Microsoft\MSLicensing\HardwareID]
	"ClientHWID"=hex(3):01,00,02,00,
		00,40,05,6a,
			0e,9d,
				01,08,
					00,20,
						4f,ac,10,40,60,00
	[HKEY_LOCAL_MACHINE\Software\Microsoft\Terminal Server Client]
	"UUID"=hex(3):
		00,40,05,6a,
			0e,9d,
				01,08,
					00,20,
						4f,ac,10,40,60,00

*/
  // "6A054000-9D0E-0801-0020-4FAC10406000"
  wsprintf(txt, L"UUID: \n%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\r\n",
    myUUID.Data1,
    myUUID.Data2,
    myUUID.Data3,
    myUUID.Data4[0],
    myUUID.Data4[1],
    myUUID.Data4[2],
    myUUID.Data4[3],
    myUUID.Data4[4],
    myUUID.Data4[5],
    myUUID.Data4[6],
    myUUID.Data4[7]);

  RETAILMSG(
    1,
    (txt));

  wsprintf(szClientHWID, txt);
  //MessageBox(NULL, txt, L"getUUID", MB_OK | MB_TOPMOST | MB_SETFOREGROUND);
  return TRUE;
}
