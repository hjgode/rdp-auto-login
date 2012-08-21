//tscDialog.cpp
#include "stdafx.h"

#include "tlhelp32.h"
#pragma comment(lib, "toolhelp.lib")

/*
REGEDIT4

[HKEY_LOCAL_MACHINE\Software\RDP_autologin]
"FitToScreen"="1"
"FullScreen"="1"
"Status"="connecting..."
"Save Password"="1"
"Domain"=""
"Password"="Intermec+2004"
"Username"="rdesktop"
"Computer"="192.168.0.130"
"DesktopWidth"=640
"DesktopHeight"=480
"startOnExit"="\\rdp_keepBusy.exe"
"execargs"="noRDPstart"
"UseMouseClick"="0"	//added with version 3 to switch between mouse and keyboard simulation
"ColorDepth"=1		//added with version 4 to enable switching between 8 and 16 Bit colors
*/

//#include "C:/Programme/Windows Mobile 6 SDK/PocketPC/Include/Armv4i/uniqueid.h"
#include <uniqueid.h>

#ifndef TH32CS_SNAPNOHEAPS
	#define TH32CS_SNAPNOHEAPS 0x40000000
#endif

#include "registry.h"

#include "rdp_file.h"

#include "getUUID.h"

// GLOBALS :-(
BOOL foundIt;
BOOL killedIt;
HWND hWindow;
int iScreenWidth=240;	// assume QVGA
int iScreenHeight=320;
TCHAR sScreenWidth[10];
TCHAR sScreenHeight[10];

DWORD iDesktopWidth=640;
DWORD iDesktopHeight=480;

TCHAR sAppOnExit[MAX_PATH]=L"";
TCHAR sExeArgs[MAX_PATH]=L"";

bool bUseMouseClick=false;

DWORD dwColorDepth = 1;
DWORD dwEnableDriveRedirection = 0;	//new in v5
DWORD dwAudioRedirectionMode = 1;	//new in v5

typedef struct{
	TCHAR*	szLabel;
	DWORD	dwCtrlID;
	TCHAR	szValue[MAX_PATH];
}myDlgItem;

myDlgItem myDlgItems[] = {
	{L"Computer", 0x403, L"192.168.0.5"}, //192.168.0.2
	{L"Username", 0x3ef, L"rdesktop"},
	{L"Password", 0x3f0, L"rdesktop"},// L"Intermec+2004"},
	{L"Domain",   0x3f1, L""},
	{L"Save Password", 0x3f2, L"1"},
	{L"Status", 0x410, L"connecting..."},
};
#define COUNT_DLG_ITEMS 6

//additional options
BOOL g_bUseFullscreen=false;
BOOL g_bUseFitToScreen=false;

BOOL getScreenSize(){
	int iScreenX = GetSystemMetrics(SM_CXSCREEN);
	int iScreenY = GetSystemMetrics(SM_CYSCREEN);
	DEBUGMSG(1, (L"\ngetScreenSize: x=%i, y=%i\n", iScreenX, iScreenY));
	if(iScreenX>0)
		iScreenWidth=iScreenX;
	if(iScreenY>0)
		iScreenHeight=iScreenY;
	if(iScreenX+iScreenY > 0){
		_itow(iScreenWidth, sScreenWidth, 10);
		_itow(iScreenHeight, sScreenHeight, 10);
		return TRUE;
	}
	else
		return FALSE;
} 

//############################### CODE ######################################
//write an RDP file with settings stored in Registry
void writeRDP(){
	FILE  *pFile=NULL;
	TCHAR *filename = L"\\Windows\\default.rdp";
	
	byte buffer[2];
	buffer[0]=0xFF;
	buffer[1]=0xFE;

	TCHAR szTemp[MAX_PATH*2];
	size_t str_length = 0;

	TCHAR sDesktopWidth[10];
	TCHAR sDesktopHeight[10];
	_ultow(iDesktopWidth, sDesktopWidth, 10);
	_ultow(iDesktopHeight, sDesktopHeight, 10);

	pFile = _wfopen(filename, L"w+b");
	if(pFile != NULL)
	{
		fwrite(buffer, 2, 1, pFile);
		int c=1;
		do{
			//extra processing
			//Fullscreen?
			//desktop size
			if(wcsstr(rdpLines[c].line, L"UserName")!=NULL)
				wsprintf(szTemp, rdpLines[c].line, myDlgItems[1].szValue);
			else if(wcsstr(rdpLines[c].line, L"Domain")!=NULL)
				wsprintf(szTemp, rdpLines[c].line, myDlgItems[3].szValue);
			else if(wcsstr(rdpLines[c].line, L"ServerName")!=NULL)
				wsprintf(szTemp, rdpLines[c].line, myDlgItems[0].szValue);
			else if(wcsstr(rdpLines[c].line, L"SavePassword")!=NULL)
				wsprintf(szTemp, rdpLines[c].line, myDlgItems[4].szValue);

			else if(wcsstr(rdpLines[c].line, L"DesktopHeight")!=NULL)

				if(g_bUseFitToScreen)   //3=FullScreen or 1=normal
					wsprintf(szTemp, rdpLines[c].line, sScreenHeight /* L"320" */);
				else
					wsprintf(szTemp, rdpLines[c].line, sDesktopHeight);// L"640");

			else if(wcsstr(rdpLines[c].line, L"DesktopWidth")!=NULL)

				if(g_bUseFitToScreen)   //3=FullScreen or 1=normal
					wsprintf(szTemp, rdpLines[c].line, sScreenWidth /* L"240" */);
				else
					wsprintf(szTemp, rdpLines[c].line, sDesktopWidth);// L"480");

			else if(wcsstr(rdpLines[c].line, L"ScreenStyle")!=NULL){
				//0=no fullscreen + no fit, 1= fit to screen+no fullscreen, 2=fullscreen+no fit, 3=fit+fullscreen
				if(g_bUseFullscreen && g_bUseFitToScreen)   
					wsprintf(szTemp, rdpLines[c].line, L"3");
				else if (!g_bUseFullscreen && !g_bUseFitToScreen)   
					wsprintf(szTemp, rdpLines[c].line, L"0");
				else if (!g_bUseFullscreen && g_bUseFitToScreen)   
					wsprintf(szTemp, rdpLines[c].line, L"1");
				else if (g_bUseFullscreen && !g_bUseFitToScreen)   
					wsprintf(szTemp, rdpLines[c].line, L"2");
				else
					wsprintf(szTemp, rdpLines[c].line, L"0");
			}

			else if(wcsstr(rdpLines[c].line, L"ColorDepthID")!=NULL){
				wsprintf(szTemp, rdpLines[c].line, dwColorDepth);  //3=HighColor(16Bit) or 1=(8Bit) color
			}

			else if(wcsstr(rdpLines[c].line, L"EnableDriveRedirection")!=NULL){
				wsprintf(szTemp, rdpLines[c].line, dwEnableDriveRedirection);  //0=no redirection to host (default), 1=EnableDriveRedirection
			}

			else if(wcsstr(rdpLines[c].line, L"AudioRedirectionMode")!=NULL){
				wsprintf(szTemp, rdpLines[c].line, dwAudioRedirectionMode);  //0=Redirect sounds to the client, 
																			 //1=Play sounds at the remote computer, 
																			 //2=Disable sound redirection; do not play sounds at the server
			}
			else
				wsprintf(szTemp, rdpLines[c].line);

			//write line by line
			str_length = wcslen(szTemp) * sizeof(TCHAR); //unicode!

			fwrite(szTemp , str_length, 1, pFile);
			DEBUGMSG(1, (L"write: '%s'\n", szTemp));

			c++;
		}while( rdpLines[c].line != NULL );
		fclose(pFile);
		DEBUGMSG(1, (L"\nFile write complete\n"));
	}
	//CryptProtectData
}

void readReg(){
	TCHAR szTemp[MAX_PATH];
	DWORD dwTemp=0;
	if(OpenKey()==0){
		for (int i=0; i<COUNT_DLG_ITEMS; i++){
			if (RegReadStr(myDlgItems[i].szLabel, szTemp)==0)
				wsprintf(myDlgItems[i].szValue, szTemp);
			DEBUGMSG(1, (L"%s=%s\n", myDlgItems[i].szLabel, myDlgItems[i].szValue));
		}

		//g_bUseFitToScreen "FitToScreen"
		if(RegReadStr(L"FitToScreen", szTemp) == 0){
			if(wcscmp(szTemp, L"0")==0)
				g_bUseFitToScreen=false;
			else
				g_bUseFitToScreen=true;
		}

		//g_bUseFullscreen
		if(RegReadStr(L"FullScreen", szTemp) == 0){
			if(wcscmp(szTemp, L"0")==0)
				g_bUseFullscreen=false;
			else
				g_bUseFullscreen=true;
		}
		
		if(RegReadStr(L"UseMouseClick", szTemp) == 0){
			if(wcscmp(szTemp, L"0")==0)
				bUseMouseClick=false;
			else
				bUseMouseClick=true;
		}
		//read DesktopWidth and Height
		//"DesktopWidth"=240
		//"DesktopHeight"=320
		DWORD dwDesktopWidth=240;
		DWORD dwDesktopHeight=320;

		if(RegReadDword(L"DesktopWidth", &dwDesktopWidth) == 0)
			iDesktopWidth=dwDesktopWidth;
		
		if(RegReadDword(L"DesktopHeight", &dwDesktopHeight) ==0)
			iDesktopHeight=dwDesktopHeight;
		
		DEBUGMSG(1, (L"readReg: Desktop width=%u and height=%u\n", iDesktopWidth, iDesktopHeight));

		//sAppOnExit
		if(RegReadStr(L"startOnExit", szTemp) == 0)
			wsprintf(sAppOnExit, L"%s", szTemp);
		else
			wsprintf(sAppOnExit, L"");
		DEBUGMSG(1, (L"readReg: sAppOnExit='%s'\n", sAppOnExit));

		// "execargs"="noRDPstart"
		if(RegReadStr(L"execArgs", szTemp) == 0)
			wsprintf(sExeArgs, L"%s", szTemp);
		else
			wsprintf(sExeArgs, L"");
		DEBUGMSG(1, (L"readReg: sExeArgs='%s'\n", sExeArgs));

		//ColorDepth new with version 4
		if(RegReadDword(L"ColorDepth", &dwTemp) == 0)
			dwColorDepth=dwTemp;

		//EnableDriveRedirection new with version 5
		if(RegReadDword(L"EnableDriveRedirection", &dwTemp) == 0)
			dwEnableDriveRedirection=dwTemp;
		
		//AudioRedirectionMode new with version 5
		if(RegReadDword(L"AudioRedirectionMode", &dwTemp) == 0)
			dwAudioRedirectionMode=dwTemp;
	}
}

void writeReg(){
	if(OpenCreateKey(L"Software\\RDP_autologin")==0){
		for (int i=0; i<COUNT_DLG_ITEMS; i++){
			if (RegWriteStr(myDlgItems[i].szLabel, myDlgItems[i].szValue)==0)
				DEBUGMSG(1, (L"WriteReg OK: %s=%s\n", myDlgItems[i].szLabel, myDlgItems[i].szValue));
			else
				DEBUGMSG(1, (L"WriteReg failed: %s=%s\n", myDlgItems[i].szLabel, myDlgItems[i].szValue));
		}
		if(RegWriteStr(L"FullScreen", L"1")==0)
			DEBUGMSG(1, (L"WriteReg OK: FullScreen=1\n"));
		else
			DEBUGMSG(1, (L"WriteReg FAILED: FullScreen=1\n"));

		g_bUseFullscreen=false;
		if(RegWriteStr(L"FitToScreen", L"0")==0)
			DEBUGMSG(1, (L"WriteReg OK: FitToScreen=0\n"));
		else
			DEBUGMSG(1, (L"WriteReg FAILED: FitToScreen=0\n"));
		
		g_bUseFitToScreen=false;
		if(RegWriteDword(L"DesktopWidth", &iDesktopWidth)==0)
			DEBUGMSG(1, (L"WriteReg OK: DesktopWidth=%u\n", iDesktopWidth));
		else
			DEBUGMSG(1, (L"WriteReg FAILED: DesktopWidth=%u\n", iDesktopWidth));

		if(RegWriteStr(L"UseMouseClick", L"0")==0)
			DEBUGMSG(1, (L"WriteReg OK: UseMouseClick=0\n"));
		else
			DEBUGMSG(1, (L"WriteReg FAILED: UseMouseClick=0\n"));
		bUseMouseClick=false;

		if(RegWriteDword(L"DesktopHeight", &iDesktopHeight)==0)
			DEBUGMSG(1, (L"WriteReg OK: DesktopHeight=%u\n", iDesktopHeight));
		else
			DEBUGMSG(1, (L"WriteReg FAILED: DesktopHeight=%u\n", iDesktopHeight));

		wsprintf(sAppOnExit, L"\\Windows\\rdp_keepbusy.exe");
		if(RegWriteStr(L"StartOnExit", sAppOnExit)==0)
			DEBUGMSG(1, (L"WriteReg OK: sAppOnExit='%s'\n", sAppOnExit));
		else
			DEBUGMSG(1, (L"WriteReg FAILED: sAppOnExit='%s'\n", sAppOnExit));

		// "execargs"="noRDPstart"
		if(RegWriteStr(L"ExecArgs", sExeArgs)==0)
			DEBUGMSG(1, (L"WriteReg OK: sExeArgs='%s'\n", sExeArgs));
		else
			DEBUGMSG(1, (L"WriteReg FAILED: sExeArgs='%s'\n", sExeArgs));

		//color depth
		if(RegWriteDword(L"ColorDepth", &dwColorDepth)==0)
			DEBUGMSG(1, (L"WriteReg OK: ColorDepth=%u\n", dwColorDepth));
		else
			DEBUGMSG(1, (L"WriteReg FAILED: ColorDepth=%u\n", dwColorDepth));

		//drive redirection
		if(RegWriteDword(L"EnableDriveRedirection", &dwEnableDriveRedirection)==0)
			DEBUGMSG(1, (L"WriteReg OK: EnableDriveRedirection=%u\n", dwEnableDriveRedirection));
		else
			DEBUGMSG(1, (L"WriteReg FAILED: EnableDriveRedirection=%u\n", dwEnableDriveRedirection));

		//audio redirection
		if(RegWriteDword(L"AudioRedirectionMode", &dwAudioRedirectionMode)==0)
			DEBUGMSG(1, (L"WriteReg OK: AudioRedirectionMode=%u\n", dwAudioRedirectionMode));
		else
			DEBUGMSG(1, (L"WriteReg FAILED: AudioRedirectionMode=%u\n", dwAudioRedirectionMode));
	}
}


/*
	preset for reg:

	[HKEY_CURRENT_USER\Software\Microsoft\Terminal Server Client]
	"MRU"="192.168.0.130;"

	[HKEY_LOCAL_MACHINE\Software\Microsoft\MSLicensing\HardwareID]
	"ClientHWID"=hex(3):01,00,02,00,
		00,40,05,6a,
			0e,9d,
				01,08,
					00,20,
						4f,ac,10,40,60,00

GetUUID=
    6A054000-9D0E-0801-0020-4FAC10406000

	[HKEY_LOCAL_MACHINE\Software\Microsoft\Terminal Server Client]
	"BitmapPersistCacheLocation"="\\Temp"
	"EditBoxPositionY"=dword:00000000
	"MapVirtualKeyLib"="\\Windows\\MvkLib.dll"
	"Shadow Bitmap Enabled"=dword:00000001
-->	"UUID"=hex(3):00,40,05,6a,0e,9d,01,08,00,20,4f,ac,10,40,60,00

	[HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\KEYBD]
	"Keyboard Function Keys"=dword:0000000C
	"Keyboard SubType"=dword:00000000
	"Keyboard Type"=dword:00000004


*/


void writeMRU(){
	TCHAR szMRU[MAX_PATH];
	wsprintf(szMRU, L";", myDlgItems[0].szValue);
	if(OpenCreateKey(L"Software\\Microsoft\\Terminal Server Client")==0){
		if (RegWriteStr(L"MRU", szMRU)==0)
			DEBUGMSG(1, (L"WriteReg OK: %s=%s\n", L"Software\\Microsoft\\Terminal Server Client", myDlgItems[0].szValue));
		else
			DEBUGMSG(1, (L"WriteReg #failed#: %s=%s\n", L"Software\\Microsoft\\Terminal Server Client", myDlgItems[0].szValue));
	}

	const int Size = 20;
	char OutBuff[Size];
	unsigned long OutBytes = 0;
	KernelIoControl(IOCTL_HAL_GET_DEVICEID, 0, 0, OutBuff, Size, &OutBytes);
	
 }

/*$DOCBEGIN$
 =======================================================================================================================
 *    £
 *    BOOL IsProcessRunning( TCHAR * pname ); £
 *    * Description: Get process table snapshot, look for pname running. £
 *    * Arguments: pname - pointer to name of program to look for. £
 *    for example, app.exe. £
 *    * Returns: TRUE - process is running. £
 *    FALSE - process is not running. £
 *    $DOCEND$ £
 *
 =======================================================================================================================
 */

BOOL IsProcessRunning( TCHAR *pname )
{
    HANDLE          hProcList;
    PROCESSENTRY32  peProcess;
    DWORD           thDeviceProcessID;
    TCHAR           lpname[MAX_PATH];
    if ( wcslen(pname)==0 )
    {
        return FALSE;
    }

    wcscpy( lpname, pname );
    _tcslwr( lpname );
    hProcList = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS, 0 );
    if ( hProcList == INVALID_HANDLE_VALUE )
    {
        return FALSE;
    }       /* end if */

    memset( &peProcess, 0, sizeof(peProcess) );
    peProcess.dwSize = sizeof( peProcess );
    if ( !Process32First( hProcList, &peProcess ) )
    {
        CloseToolhelp32Snapshot( hProcList );
        return FALSE;
    }       /* end if */

    thDeviceProcessID = 0;
    do
    {
        //_wcslwr( peProcess.szExeFile );
        if ( wcsicmp( peProcess.szExeFile, lpname ) == 0) //replaced wcsstr by wcsicmp
        {
            thDeviceProcessID = peProcess.th32ProcessID;
            break;
        }   /* end if */
    }
    while ( Process32Next( hProcList, &peProcess ) );
    if ( (GetLastError() == ERROR_NO_MORE_FILES) && (thDeviceProcessID == 0) )
    {
        CloseToolhelp32Snapshot( hProcList );
        return FALSE;
    }       /* end if */

    CloseToolhelp32Snapshot( hProcList );
    return TRUE;
}           /* IsProcessRunning */


void scanTscWindow(HWND hWndStart){

	HWND hWnd = NULL;
	HWND hWnd1 = NULL;


	hWnd = hWndStart;
	TCHAR cszWindowString [MAX_PATH]; // = new TCHAR(MAX_PATH);
	TCHAR cszClassString [MAX_PATH]; //= new TCHAR(MAX_PATH);
	TCHAR cszTemp [MAX_PATH]; //= new TCHAR(MAX_PATH);

	while (hWnd!=NULL){
		//do some formatting

		//cszWindowString=GetOurWindowString(hWnd);
		//cszClassString=GetOurClassString(hWnd);

		GetClassName(hWnd, cszClassString, MAX_PATH);
		GetWindowText(hWnd, cszWindowString, MAX_PATH);

		//cszTemp.Format(L"\"%s\"  \"%s\"\n", (LPCTSTR) cszClassString, (LPCTSTR) cszWindowString);//buf);
		wsprintf(cszTemp, L"\"%s\"  \"%s\"\t0x%0x\n", cszClassString, cszWindowString, GetDlgCtrlID(hWnd));//buf);
		DEBUGMSG(1, (cszTemp));

		// Do Next Window
		hWnd1 = GetWindow(hWnd, GW_CHILD);
		if( hWnd1 != NULL ){ 
			scanTscWindow(hWnd1);
		}
		hWnd=GetWindow(hWnd,GW_HWNDNEXT);		// Get Next Window
	}
}


DWORD FindPID(HWND hWnd){
	DWORD dwProcID = 0;
	DWORD threadID = GetWindowThreadProcessId(hWnd, &dwProcID);
	if(dwProcID != 0)
		return dwProcID;
	return 0;
}

//
// FindPID will return ProcessID for an ExeName
// retruns 0 if no window has a process created by exename
//
DWORD FindPID(TCHAR *exename)
{
	DWORD dwPID=0;
	TCHAR exe[MAX_PATH];
	wsprintf(exe, L"%s", exename);
	//Now make a snapshot for all processes and find the matching processID
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS, 0);
  if (hSnap != NULL)
  {
	  PROCESSENTRY32 pe;
	  pe.dwSize = sizeof(PROCESSENTRY32);
	  if (Process32First(hSnap, &pe))
	  {
		do
		{
		  if (wcsicmp (pe.szExeFile, exe) == 0)
		  {
			CloseToolhelp32Snapshot(hSnap);
			dwPID=pe.th32ProcessID ;
			return dwPID;
		}
		} while (Process32Next(hSnap, &pe));
	  }//processFirst
  }//hSnap
  CloseToolhelp32Snapshot(hSnap);
  return 0;

}

BOOL KillExeWindow(TCHAR* exefile)
{
	//go thru all top level windows
	//get ProcessInformation for every window
	//compare szExeName to exefile
	// upper case conversion?
	foundIt=FALSE;
	killedIt=FALSE;
	//first find a processID for the exename
	DWORD dwPID = FindPID(exefile);
	if (dwPID != 0)
	{
		//now find the handle for the window that was created by the exe via the processID
		EnumWindows(EnumWindowsProc, (LPARAM) dwPID);
		if (foundIt)
		{
			//now try to close the window
			if (PostMessage(hWindow, WM_QUIT, 0, 0) == 0) //did not success?
			{
				//try the hard way
				HANDLE hProcess = OpenProcess(0, FALSE, dwPID);
				if (hProcess != NULL)
				{
					DWORD uExitCode=0;
					if ( TerminateProcess (hProcess, uExitCode) != 0)
					{
						//app terminated
						killedIt=TRUE;
					}
				}

			}
			else
				killedIt=TRUE;
		}
		else
		{
			//no window
			//try the hard way
			HANDLE hProcess = OpenProcess(0, FALSE, dwPID);
			if (hProcess != NULL)
			{
				DWORD uExitCode=0;
				if ( TerminateProcess (hProcess, uExitCode) != 0)
				{
					//app terminated
					killedIt=TRUE;
				}
				else
					killedIt=FALSE;
			}
		}
	}
	return killedIt;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) //find window for PID
{
	DWORD wndPid;
//	TCHAR Title[128];
	// lParam = procInfo.dwProcessId;
	// lParam = exename;

	// This gets the windows handle and pid of enumerated window.
	GetWindowThreadProcessId(hwnd, &wndPid);

	// This gets the windows title text
	// from the window, using the window handle
	//  GetWindowText(hwnd, Title, sizeof(Title)/sizeof(Title[0]));
	if (wndPid == (DWORD) lParam)
	{
		//found matching PID
		foundIt=TRUE;
		hWindow=hwnd;
		return FALSE; //stop EnumWindowsProc
	}
	return TRUE;
}

//find the real RDM dialog by comparing the PID of the Dialog with the one of TSSHELLWND
//helper struct
typedef struct{
	DWORD procID;
	HWND  foundHwnd;
}procWnd;
BOOL CALLBACK EnumWindowsProcRDM(HWND hWnd, LPARAM lParam)
{
	procWnd* myProcWnd1=(procWnd*) lParam;
	
	//test, if this a window with class name = "Dialog"
	TCHAR strClass[MAX_PATH];
	int iRes = GetClassName(hWnd, strClass, MAX_PATH);
	if(iRes!=0){
		if(wcscmp(strClass, L"Dialog")==0){ //the windows class name matches
			//test if this window has the same PID
			if(FindPID(hWnd)==myProcWnd1->procID){ //the PIDs match
				myProcWnd1->foundHwnd=hWnd;
				return FALSE; //dont enum more windows
			}
		}
	}
	return TRUE; //continue enumWindows
}
HWND getTscDialog(DWORD tscPID){
	procWnd myProcWnd;
	myProcWnd.procID=tscPID;
	myProcWnd.foundHwnd=NULL;

	EnumWindows(EnumWindowsProcRDM, (LPARAM) &myProcWnd);
	if(myProcWnd.foundHwnd!=NULL)
		return myProcWnd.foundHwnd;
	else
		return NULL;
}

BOOL setDlgText(HWND hDlg, TCHAR* text, DWORD ctrlID){
	int cnt = 0; //to count repeat tries
	LRESULT lRes = 0;
	BOOL isOK = false;
	HWND hWndCtrl=NULL;
	TCHAR test[MAX_PATH];
	if(hDlg==NULL)
		return FALSE; //basic error handling

	//try to set text and verify!
	do{
		cnt++;
		hWndCtrl = GetDlgItem(hDlg, ctrlID);
		lRes = SetWindowText(hWndCtrl, text); //zero=failed
		DEBUGMSG(1, (L"Try %i, SetWindowText: '%s', lRes=0x%0x\n",cnt, text, lRes));
		if(lRes!=0){ //verify text is in window
			lRes = GetWindowText(hWndCtrl, test, MAX_PATH-1);
			if(wcslen(text)>0){ //is the text non empty?
				if(lRes>0){ //did we get some text back?
					if(wcscmp(text, test)==0){ //is this the text we have set?
						isOK = TRUE;
					}
				}
			}
			else { //empty text
				if(lRes==0)
					isOK = TRUE;
			}
		}
	}while (!isOK && cnt<4);
	return isOK;
}

//try to launch RDM
int startRDM(){
	DEBUGMSG(1, (L"Entering startRDM()\n"));
	int cnt=0; //repeat counter
	BOOL isOK = FALSE;
	int iRet=0;

	do{
		cnt++;
		//if tsc is already running, kill it

		//first ensure TSSHELLWND is not minimized or connect will hang (why?)
		HWND hwndTSC = FindWindow(L"TSSHELLWND",NULL);
		if(hwndTSC!=NULL)
			ShowWindow(hwndTSC, SW_SHOWNORMAL);

		if(IsProcessRunning(L"wpctsc.exe")){
			if( KillExeWindow(L"wpctsc.exe") ){
				//killedit OK
				Sleep(1000);
			}
			else{
				//was unable to kill
				iRet = -1; //unable to kill wpctsc
				continue;
			}
		}

		//write defaults to REG and default.rdp
		writeRDP();
		writeMRU(); //ensure at least one entry is in MRU

		DWORD dProcIDTSC=0; //to save proc ID of TSC main window
		//start a new instance of tsc
		PROCESS_INFORMATION pi;
		if (CreateProcess(L"\\windows\\wpctsc.exe", L"", NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi)!=0)
		{
			//OK
			Sleep(1000); //give some time to setup
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		else
		{
			//start failed
			iRet = -2; //unable to start wpctsc
			continue;
		}

		//find the "Remote Desktop Mobile" dialog window
		DWORD pidTSC = FindPID(L"wpctsc.exe");
		HWND hTscDialog = getTscDialog(pidTSC);  //FindWindow(L"Dialog", NULL);

		if(hTscDialog!=NULL){
			//check if this is the right window
			if(FindPID(L"wpctsc.exe") != 0){
				if(FindPID(hTscDialog)!= FindPID(L"wpctsc.exe")){
					iRet = -4; //error finding TSC dialog
					continue;
				}
				else{
					#ifdef DEBUG
					DEBUGMSG(1, (L" ### ScanTSCwindow ### \r\n"));
					scanTscWindow(hTscDialog); //scan TSC window and list child windows and CtrlIDs
					DEBUGMSG(1, (L" --- ScanTSCwindow --- \r\n"));
					#endif
					isOK=TRUE;
					iRet=0;
				}
			}
		}
		else{
			iRet = -3; //could not find tsc dialog
			continue;
		}
	}while (!isOK && cnt<3);
	DEBUGMSG(1, (L"Leaving startRDM() with code: %i\n", iRet));
	return iRet;
}

/*
TSC dialog elements:
	class		text		ctrlID
	"Dialog"  ""	0x0
	"static"  "Status:"	0x40e
	"static"  "Not connected"	0x410		//status
	
	"combobox"  "192.168.0.2"	0x403		//Computer COMBO BOX!
	"Edit"  "192.168.0.2"	0x3e9			//Computer
	"sbedit"  "rdesktop"	0x3ef			//Username
	"sbedit"  "Intermec+2004"	0x3f0		//Password
	"sbedit"  ""	0x3f1					//Domain
	"Button"  "Save password"	0x3f2		//scave pw option

	"static"  "Computer:"	0x3f7
	"static"  "User name:"	0x3f8
	"static"  "Password:"	0x3f9
	"static"  "Domain:"	0x3fa
	"SIPPREF"  "SIP1"	0x41c
*/

//######################### Main Function ##############################


int startTSC()
{
	TCHAR* szHWID = new TCHAR[MAX_PATH];
	GetUUID(szHWID);

	LRESULT lRes=0;
#ifdef DEBUG1
	writeReg(); //write default settings to reg
#endif
	readReg();

	//get the screen size
	getScreenSize();

	int iRet = startRDM();
	if (iRet != 0)
		return iRet;

	HWND hTscDialog = FindWindow(L"Dialog", NULL);

	//fill in the values

		//find window handle to each control and send new values
		//use know CtrlIDs to send DlgItem-messages
		TCHAR strText[MAX_PATH];

		//EnableWindow(hTscDialog, FALSE);
		
		//int idDlgItem = GetDlgCtrlID(hDialogItem);
		// use this or SendMessage with handle of control
		//WM_SETTEXT wParam = 0; lParam = (LPARAM)(LPCTSTR) lpsz;
		wsprintf(strText, L"start filling fields");
		SendDlgItemMessage(hTscDialog, 0x3f1, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) strText);
		Sleep(500);
		UpdateWindow(hTscDialog);

		for (int i=0; i < COUNT_DLG_ITEMS; i++){

			if(i!=4){ //special handling or the checkbox
				setDlgText(hTscDialog, myDlgItems[i].szValue, myDlgItems[i].dwCtrlID);

				//lRes = SendDlgItemMessage(hTscDialog, myDlgItems[i].dwCtrlID, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) myDlgItems[i].szValue);
				//DEBUGMSG(1, (L"Changing: '%s', lRes=0x%0x\n",myDlgItems[i].szLabel, lRes));
			}
			else
			{
				//change the Save Password checkbox
				if(wcscmp(myDlgItems[i].szValue, L"1")==0){
					lRes = SendMessage(GetDlgItem(hTscDialog, myDlgItems[i].dwCtrlID), BM_SETCHECK, BST_CHECKED, 0);
					DEBUGMSG(1, (L"Changing: '%s', lRes=0x%0x\n",myDlgItems[i].szLabel, lRes));
				}
				else{
					lRes = SendMessage(GetDlgItem(hTscDialog, myDlgItems[i].dwCtrlID), BM_SETCHECK, BST_UNCHECKED, 0);
					DEBUGMSG(1, (L"Changing: '%s', lRes=0x%0x\n",myDlgItems[i].szLabel, lRes));
				}
			}
			UpdateWindow(hTscDialog);
			EnableWindow(GetDlgItem(hTscDialog, myDlgItems[i].dwCtrlID), FALSE);
			Sleep(500);
		}

		//SetForegroundWindow(hTscDialog);

		Sleep(300);

		//working solution one to start the Connect
		//hacked by ceSpy: send RDM WM_USER+1001 with wParam=0 and lParam=0, works if one manually connect was OK
		//SendMessage(FindWindow(L"TSSHELLWND", NULL), WM_USER+1001, 0, 0);
		//test with WM_KEY...DOES not work
		//SendMessage(GetDesktopWindow(), WM_KEYDOWN, VK_F1, 0);
		//Sleep(10);
		//SendMessage(GetDesktopWindow(), WM_KEYUP, VK_F1, 0);


		if(bUseMouseClick){
			//Solution two with mouse_event, click at 13,306. The 13 comes from the assumption that hight of
			//menu bar is 26 pixel and I want to click in the mid
			//this solution does work as keyb_event does work
			//	normalized coordinates:
			//	(0,0) = upper left corner
			//	(0xFFFF,0xFFFF) = lower right corner
			DWORD dX = (0xFFFF / iScreenWidth) * (80); // changed from 13 to width=240, 1/3=80
			DWORD dY = (0xFFFF / iScreenHeight) * (iScreenHeight - 13);
			DEBUGMSG(1, (L"mouse click at: %u, %u\n", dX * 0xFFFFFFFF / 240, dY * 0xFFFFFFFF / 320));
			//SetForegroundWindow(hTscDialog); //dont mess with windows z-order
			
			//this will make a cursor visible
			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, dX, dY, 0, 0);
			Sleep(5);
			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, dX, dY, 0, 0);
			Sleep(30);
			/*
			//this is what happens, if you tap the screen
			mouse_event(MOUSEEVENTF_TOUCH | MOUSEEVENTF_LEFTDOWN, dX, dY, 0, 0);
			mouse_event(MOUSEEVENTF_TOUCH | MOUSEEVENTF_LEFTUP, dX, dY, 0, 0);
			//Sleep(3000);
			*/
		}
		else{

			//Solution three, best solution, ensure you use the scancode value too! AFAIK the scancode for F1 is always 0x70

			keybd_event(VK_F1, 0x70, 0, 0);
			Sleep(30);
			keybd_event(VK_F1, 0x70, KEYEVENTF_KEYUP, 0);

		}//bUseMouseClick
/*
		//test with WM_LBUTTONDOWN, did not work
		HWND hClickWnd = FindWindow(L"TSSHELLWND", NULL);
		lRes = PostMessage(hClickWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(60,307));
		DEBUGMSG(1, (L"SendMessage(WM_LBUTTONDOWN, MK_LBUTTON) Result=0x%0x, GetLastError=0x%0x\n", lRes, GetLastError()));
		Sleep(30);
		lRes = PostMessage(hClickWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(60,307));
		DEBUGMSG(1, (L"SendMessage(WM_LBUTTONUP, MK_LBUTTON) Result=0x%0x, GetLastError=0x%0x\n", lRes, GetLastError()));
*/


		//SendMessage(FindWindow(L"TSSHELLWND", NULL), WM_SETTEXT, 0, (LPARAM)(LPCTSTR) L"PLEASE WAIT");

		//SetForegroundWindow(FindWindow(L"TSSHELLWND", NULL));
		
		//ShowWindow(FindWindow(L"TSSHELLWND", NULL), SW_SHOWNORMAL);

		//need this?
		//UpdateWindow(hTscDialog);

		//now wait for the Dialog to disapear and start rdm_keep_busy so the session will not timeout
		
		if(wcslen(sAppOnExit)>0){
			TCHAR* strExeFile; int ch = '\\';
			strExeFile = wcsrchr(sAppOnExit, ch); 
			if(strExeFile!=NULL)
				strExeFile++; //add one position as the found backslash is part of the pointer
			else
				strExeFile = sAppOnExit; //if not found, just let it point to the original string

			if(!IsProcessRunning(strExeFile)){
				SHELLEXECUTEINFO sei = {0};
				sei.cbSize = sizeof(sei);
				sei.nShow = SW_SHOWNORMAL; 
				sei.lpFile = sAppOnExit;
				sei.lpParameters = sExeArgs;// L"noRdpStart";
				if (!ShellExecuteEx(&sei)){
					DEBUGMSG(1, (L"Starting '%s' FAILED\n", sAppOnExit));
				}
				else{
					DEBUGMSG(1, (L"Starting '%s' OK\n", sAppOnExit));
				}
			}
		}
	return 0;
}
