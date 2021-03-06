// RDM_keepBusy.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "nclog.h"

#pragma comment(user, "version 20181115")

BOOL bUseLogging=TRUE;

#define TEST
#undef TEST

#ifdef TEST
	//4 second
	DWORD _dwSLEEPTIME = 4000;
#else
	//4 minutes 
	DWORD _dwSLEEPTIME = 1000*60*4;
#endif

#define MYDEBUGMSG1(x, ...) DEBUGMSG(1,(__VA_ARGS__)) 

	void MYDEBUGMSG(const wchar_t *fmt, ...){
        va_list vl;
		va_start(vl,fmt);
		wchar_t bufW[10240]; // to bad CE hasn't got wvnsprintf
		wvsprintf(bufW,fmt,vl);
#ifdef DEBUG
		MYDEBUGMSG1(1, (bufW));
#endif
		if(bUseLogging){
			nclog(bufW);
		}
	}

//	DEBUGMSG(cond,printf_exp); \
//	NKDbgPrintfW(printf_exp); \	


#define sMUTEX L"RDMKEEPBUSY"		//allow only one instance
#define sEvent L"STOPRDMKEEPBUSY"	//let RDMKeepbusy shutdown
BOOL _bRunApp = TRUE;				//the var that lets exit the code
DWORD idThreadWatch=0;				//the thread that waits for shutdown event
HANDLE hEventWatch=NULL;			//let the background thread show the actual status

//text used to find RDM main window class and connected session window
// for PPC2003:
/*	"192.168.128.5 - Terminal Services Client" (UIMainClass)	// top level window: FindWindow
		+	"<No name>"	UIContainerClass						// child window to UIMainClass
			+	"Input Capture Window" (IHWindowClass)			// child window to TerminalServerClient
*/
TCHAR szMainRDMWindowsClass[MAX_PATH]; // for WM2005 and up: "TSSHELLWND" and 
TCHAR szUIcaptureChildWindowClass[MAX_PATH];	//then child window "Input Capture Window"
TCHAR szRDMconnectedSessionText[MAX_PATH]; // <something>- Remote Desktop Mobile"

//registry args for keyboard SendMessage
DWORD _dwUseKeyboard = 0;			//use keybd_event keyboard simulation?
DWORD _dwKey1 = VK_LEFT;
DWORD _dwLPARAM11 = 0x14d0001;
DWORD _dwLPARAM12 = 0xc14d0001;

DWORD _dwKey2 = VK_RIGHT;
DWORD _dwLPARAM21 = 0x14b0001;
DWORD _dwLPARAM22 = 0xc14b0001;

//registry args for mouse click events
DWORD _dwUseMouse=0;				//use mouse_event mouse simulation
DWORD _dwMouseX=0x20;
DWORD _dwMouseY=0x20;

//the animation line showing the status
int		lineHeight=1;		//progress bar height
int		drawPointX=1;		//start to draw a line (horizontal pos)
int		drawPointY=1;		//start to draw a line (vertical pos)
int		lineLength = 30; //GetSystemMetrics(SM_CXSCREEN);
HDC		hdc = NULL;	//get DC of desktop window
HPEN	po;	// Pen Progress bar
HPEN	lineBack, lineRed, lineYellow, lineGreen, lineOrange;
HPEN	lineOrg;

// Global variable to avoid concurrent access
CRITICAL_SECTION myCriticalSection; 
//the possible app states
enum stati{
	stopped,
	notfound,
	active,
	inactive,
	idle
};
//the var to hold the current status
DWORD g_dwStatus=stopped;

//thread vars
DWORD idThread=0;
HANDLE hThread=NULL;
BOOL runThread=TRUE;

BOOL isIntermec(){
	HMODULE hLib = LoadLibrary(L"itc50.dll");
	if(hLib==NULL)
		return FALSE;
	FARPROC fProc = GetProcAddress(hLib, L"ITCGetDeviceType");
	if(fProc==NULL){
		FreeLibrary(hLib);
		return FALSE;
	}
	FreeLibrary(hLib);
	return TRUE;
/*	//this lib will load a lot bunch of other DLLs
	HMODULE hLib = LoadLibrary(L"itcscan.dll");
	if(hLib==NULL)
		return FALSE;
	FARPROC fProc = GetProcAddress(hLib, L"ITCSCAN_Open");
	if(fProc==NULL){
		FreeLibrary(hLib);
		return FALSE;
	}
	FreeLibrary(hLib);
	return TRUE;
*/
}

int writeReg(){
	HKEY hKey =NULL;
	DWORD dwDispo=0;
	LRESULT lRes = RegCreateKeyEx(HKEY_LOCAL_MACHINE,  L"Software\\RDMKeepbusy", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDispo);
	if(lRes==ERROR_SUCCESS){
		if(dwDispo==REG_OPENED_EXISTING_KEY)
			return 0;
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\RDMKeepbusy", 0, KEY_ALL_ACCESS, &hKey);
	}
	else{
		goto exit_regwrite;
	}
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwData = 0;

	dwData = bUseLogging; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"doLogging", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	dwData = drawPointX;
	if( lRes = RegSetValueEx (hKey, L"lineX", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;
	
	dwData = drawPointY; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lineY", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	dwData = lineLength; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lineLen", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	dwData = lineHeight; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lineWidth", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	dwData = _dwKey1; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"Key1", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	dwData = _dwKey2; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"Key2", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	dwData = _dwLPARAM11; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lParam11", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	dwData = _dwLPARAM12; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lParam12", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	dwData = _dwLPARAM21; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lParam21", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	dwData = _dwLPARAM22; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lParam22", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	dwData = _dwUseMouse; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"useMouse", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;
	
	dwData = _dwMouseX; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"mouseX", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	dwData = _dwMouseY; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"mouseY", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	dwData = _dwSLEEPTIME / 1000; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"sleepTime", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;
	
	dwType=REG_SZ;
	wsprintf(szMainRDMWindowsClass, L"TSSHELLWND");
	dwSize=wcslen(szMainRDMWindowsClass)*sizeof(TCHAR);
	if( lRes = RegSetValueEx (hKey, L"MainRDMWindowsClass", 0, dwType, (BYTE*) &szMainRDMWindowsClass, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	wsprintf(szUIcaptureChildWindowClass, L"Input Capture Window");
	dwSize=wcslen(szUIcaptureChildWindowClass)*sizeof(TCHAR);
	if( lRes = RegSetValueEx (hKey, L"UIcaptureChildWindowClass", 0, dwType, (BYTE*) &szUIcaptureChildWindowClass, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

	wsprintf(szRDMconnectedSessionText, L"Remote Desktop Mobile");
	dwSize=wcslen(szRDMconnectedSessionText)*sizeof(TCHAR);
	if( lRes = RegSetValueEx (hKey, L"RDMconnectedSessionText", 0, dwType, (BYTE*) &szRDMconnectedSessionText, dwSize) != ERROR_SUCCESS)
		goto exit_regwrite;

exit_regwrite:
	RegCloseKey(hKey);
	return lRes;
}

int readReg(){
	//added defaults instead of return error code
	int iRet=0;
	HKEY hKey;
	LRESULT lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\RDMKeepbusy", 0, KEY_ALL_ACCESS, &hKey); 
	if(lRes!=ERROR_SUCCESS){		
		return writeReg();
	}
	DWORD dwData=0, dwSize=sizeof(DWORD), dwType = REG_DWORD;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"lineX", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes==ERROR_SUCCESS)
		drawPointX=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"lineY", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes==ERROR_SUCCESS)
		drawPointY=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"lineLen", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes==ERROR_SUCCESS)
		lineLength=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"lineWidth", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes==ERROR_SUCCESS)
		lineHeight=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"Key1", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes==ERROR_SUCCESS)
	{	//if key is not defined, we will not get here
		_dwKey1=dwData;

		dwSize=sizeof(DWORD); dwType=REG_DWORD;
		lRes = RegQueryValueEx(hKey, L"lParam11", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
		if(lRes==ERROR_SUCCESS)
			_dwLPARAM11=dwData;

		dwSize=sizeof(DWORD); dwType=REG_DWORD;
		lRes = RegQueryValueEx(hKey, L"lParam12", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
		if(lRes==ERROR_SUCCESS)
			_dwLPARAM12=dwData;
	}

	dwSize=sizeof(DWORD); dwType = REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"Key2", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes==ERROR_SUCCESS)
	{
		_dwKey2=dwData;

		dwSize=sizeof(DWORD); dwType=REG_DWORD;
		lRes = RegQueryValueEx(hKey, L"lParam21", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
		if(lRes==ERROR_SUCCESS)
			_dwLPARAM21=dwData;

		dwSize=sizeof(DWORD); dwType=REG_DWORD;
		lRes = RegQueryValueEx(hKey, L"lParam22", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
		if(lRes==ERROR_SUCCESS)
			_dwLPARAM22=dwData;
	}

	lRes = RegQueryValueEx(hKey, L"useMouse", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes==ERROR_SUCCESS)
	{
		if(dwData==1)
			_dwUseMouse=1;
		else
			_dwUseMouse=0;

		dwSize=sizeof(DWORD); dwType=REG_DWORD;
		lRes = RegQueryValueEx(hKey, L"mouseX", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
		if(lRes==ERROR_SUCCESS)
			_dwMouseX=dwData;

		dwSize=sizeof(DWORD); dwType=REG_DWORD;
		lRes = RegQueryValueEx(hKey, L"mouseY", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
		if(lRes==ERROR_SUCCESS)
			_dwMouseY=dwData;
	}


	//read sleep time in seconds
	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"sleepTime", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		_dwSLEEPTIME=dwData*1000; 

	//bdoLogging
	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"doLogging", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		bUseLogging=TRUE;
	else
		bUseLogging=FALSE; 

	nclog_LogginEnabled=bUseLogging;
	MYDEBUGMSG(bUseLogging?L"Logging enabled":L"Logging disabled");

	dwSize = MAX_PATH;
	TCHAR szTemp[MAX_PATH];
	dwType=REG_SZ;
	dwSize=dwSize*sizeof(TCHAR); //byte size is 
	lRes = RegQueryValueEx(hKey, L"MainRDMWindowsClass", NULL, &dwType, (BYTE*) szTemp, &dwSize);
	if(lRes==ERROR_SUCCESS)
		wsprintf(szMainRDMWindowsClass, L"%s", szTemp);
	else
		wsprintf(szMainRDMWindowsClass, L"TSSHELLWND");

	memset(szTemp,0,MAX_PATH*sizeof(TCHAR));
	dwType=REG_SZ;
	dwSize=dwSize*sizeof(TCHAR); //byte size is 
	lRes = RegQueryValueEx(hKey, L"UIcaptureChildWindowClass", NULL, &dwType, (BYTE*) szTemp, &dwSize);
	if(lRes==ERROR_SUCCESS)
		wsprintf(szUIcaptureChildWindowClass, L"%s", szTemp);
	else
		wsprintf(szUIcaptureChildWindowClass, L"Input Capture Window");

	memset(szTemp,0,MAX_PATH*sizeof(TCHAR));
	dwType=REG_SZ;
	dwSize=dwSize*sizeof(TCHAR); //byte size is 
	lRes = RegQueryValueEx(hKey, L"RDMconnectedSessionText", NULL, &dwType, (BYTE*) szTemp, &dwSize);
	if(lRes==ERROR_SUCCESS)
		wsprintf(szRDMconnectedSessionText, L"%s", szTemp);
	else
		wsprintf(szRDMconnectedSessionText, L"Remote Desktop Mobile");

	MYDEBUGMSG(L"Windows text and class use: \nMainRDMWindowsClass: %s\nUIcaptureChildWindowClass; %s\nszRDMconnectedSessionText; %s\n", 
		szMainRDMWindowsClass, szUIcaptureChildWindowClass, szRDMconnectedSessionText);

	RegCloseKey(hKey);
	return 0;
}

BOOL bFoundWindow=FALSE;
HWND hFoundHWND=NULL;
HWND findWindow(HWND hWndStart, TCHAR* szTitle){

	HWND hWnd = NULL;
	HWND hWnd1 = NULL;
	
	hWnd = hWndStart;
	TCHAR cszWindowString [MAX_PATH]; // = new TCHAR(MAX_PATH);
	TCHAR cszClassString [MAX_PATH]; //= new TCHAR(MAX_PATH);

	while (hWnd!=NULL && !bFoundWindow){
		GetClassName(hWnd, cszClassString, MAX_PATH);
		GetWindowText(hWnd, cszWindowString, MAX_PATH);
		if(bUseLogging)
			MYDEBUGMSG(L"findWindow: \"%s\"  \"%s\"\n", cszClassString, cszWindowString);

		if(wcscmp(cszWindowString, szTitle)==0){
			bFoundWindow=TRUE;
			hFoundHWND=hWnd;
			return hFoundHWND;
		}

		// Do Next Window
		hWnd1 = GetWindow(hWnd, GW_CHILD);
		if( hWnd1 != NULL ){ 
			findWindow(hWnd1, szTitle);
		}
		hWnd=GetWindow(hWnd,GW_HWNDNEXT);		// Get Next Window
	}
	return hFoundHWND;
}
BOOL bFoundClass=FALSE;
HWND hFoundHWNDClass=NULL;
HWND findWindowByClass(HWND hWndStart, TCHAR* szClass){

	HWND hWnd = NULL;
	HWND hWnd1 = NULL;

	hWnd = hWndStart;
	TCHAR cszWindowString [MAX_PATH]; // = new TCHAR(MAX_PATH);
	TCHAR cszClassString [MAX_PATH]; //= new TCHAR(MAX_PATH);

	while (hWnd!=NULL && bFoundClass==FALSE){
		GetClassName(hWnd, cszClassString, MAX_PATH);
		GetWindowText(hWnd, cszWindowString, MAX_PATH);
		if(bUseLogging)
			MYDEBUGMSG(L"findWindow: \"%s\"  \"%s\"\n", cszClassString, cszWindowString);

		if(wcscmp(cszWindowString, szClass)==0){
			bFoundClass=TRUE;
			hFoundHWNDClass=hWnd;
			return hFoundHWND;
		}

		// Do Next Window
		hWnd1 = GetWindow(hWnd, GW_CHILD);
		if( hWnd1 != NULL ){ 
			findWindowByClass(hWnd1, szClass);
		}
		hWnd=GetWindow(hWnd,GW_HWNDNEXT);		// Get Next Window
	}
	return hFoundHWND;
}

HWND getMSTSChandle(){	//for use with PPC2003 Terminal Service Client
/*
	window text (window class)
	"192.168.128.5 - Terminal Services Client" (UIMainClass)	// top level window: FindWindow
		+	"<No name>"	UIContainerClass						// child window to UIMainClass
			+	"Input Capture Window" (IHWindowClass)			// child window to TerminalServerClient
*/
	//find the "Input Capture Window" child Window
	//Find the top levvel window
	HWND hWndRDM = FindWindow(_T("UIMainClass"), NULL);

	//init find vars moved from being static inside findWindow() and findWindowByClass(), v15.01.2013
	bFoundWindow=FALSE;
	bFoundClass=FALSE;
	hFoundHWND=NULL;
	hFoundHWNDClass=NULL;
	HWND hTSCWND = findWindow(hWndRDM, szUIcaptureChildWindowClass);
	if(hTSCWND)
		MYDEBUGMSG( L"getTSChandle(): Found '%s', Handle=0x%0x\n", szUIcaptureChildWindowClass, hTSCWND);
	else
		MYDEBUGMSG(L"getTSChandle(): No '%s' found\n", szUIcaptureChildWindowClass);
	return hTSCWND;
}

HWND getTSChandle(){
#if _WIN32_WCE == 0x420
	return getMSTSChandle();		//use alternative window for PPC2003
#endif
/*
	window text (window class)
	"192.168.0.2 - Remote Desktop Mobile" (TSSHELLWND)					// top level window: FindWindow
		+	"WinShell" ()												// child window to TSSHELLWND
			+	"NavWnd" ()												// child window to WinShell
			+	"TerminalServerClient" (TerminalServerClient)			// child window to WinShell
				+	"<No Name>" (UIMainClass)							// child window to TerminalServerClient
					+	"<No Name>" (UIContainerClass)					// child
						+	"Input Capture Window" (IHWindowClass)		// child
*/
	//find the "Input Capture Window" child Window

	//Find the top levvel window
	HWND hWndRDM = FindWindow(szMainRDMWindowsClass, NULL);

	//init find vars moved from being static inside findWindow() and findWindowByClass(), v15.01.2013
	bFoundWindow=FALSE;
	bFoundClass=FALSE;
	hFoundHWND=NULL;
	hFoundHWNDClass=NULL;
	HWND hTSCWND = findWindow(hWndRDM, szUIcaptureChildWindowClass);
	if(hTSCWND)
		MYDEBUGMSG(L"getTSChandle(): Found '%s', Handle=0x%0x\n", szUIcaptureChildWindowClass, hTSCWND);
	else
		MYDEBUGMSG(L"getTSChandle(): No I'%s' found\n", szUIcaptureChildWindowClass);
	return hTSCWND;
}


//thread that controls the exit of the app
DWORD myWatchThread(LPVOID lpParam){
	DWORD dwRet=0;
	hEventWatch= CreateEvent(NULL, TRUE, FALSE, sEvent);
	if(hEventWatch!=NULL){
		if(GetLastError()==ERROR_ALREADY_EXISTS){
			if(bUseLogging)
				MYDEBUGMSG(L"CreateEvent: Event '%s' already exists\n", sEvent);
		}
		else
			if(bUseLogging)
				MYDEBUGMSG(L"CreateEvent: Event '%s' did not yet exist\n", sEvent);
		//now start to wait for the event to be signaled
		do{
		DWORD dwRes = WaitForSingleObject(hEventWatch, INFINITE);
			switch(dwRes){
				case WAIT_ABANDONED:
					break;
				case WAIT_TIMEOUT:
					break;
				case WAIT_OBJECT_0:
					_bRunApp=FALSE; //let the main loop exit
					break;
				case WAIT_FAILED:
					break;
				default:
					break;
			}
		}while(_bRunApp);
		dwRet=1;
	}
	else{
		if(bUseLogging)
			MYDEBUGMSG(L"CreateEvent failed: GetLastError=%u\n", GetLastError());
		dwRet=-1;
	}
	return dwRet;
}

int DrawLine(int pos){
	DWORD dwStatus=stopped;
	static BOOL bToggle=false;
	
	//hdc = GetDC(GetForegroundWindow());
	HWND hwndDraw;
	hwndDraw=FindWindow(L"HHTASKBAR", NULL);
	hdc = GetDC(hwndDraw);

	EnterCriticalSection(&myCriticalSection);
	dwStatus=g_dwStatus;
	LeaveCriticalSection(&myCriticalSection);

	//move start point to x,y (ie 0,319
	MoveToEx(hdc,drawPointX,drawPointY,NULL);

	//draw a line to pos/drawpointY
	//Sleep(300);
	if(bToggle){
		switch (dwStatus){
			case idle:
				po=(HPEN)SelectObject(hdc,lineYellow);
				break;
			case stopped:
				po=(HPEN)SelectObject(hdc,lineBack);
				break;
			case active:
				po=(HPEN)SelectObject(hdc,lineGreen);
				break;
			case inactive:
				po=(HPEN)SelectObject(hdc,lineOrange);
				break;
			case notfound:
				po=(HPEN)SelectObject(hdc,lineRed);
				break;
			default:
				po=(HPEN)SelectObject(hdc,lineBack);
				break;
		}
		//ie to 30/2,319
		LineTo(hdc, lineLength, drawPointY);
	}
	else{
		po = (HPEN)SelectObject(hdc,lineBack); 
		//ie to 30,319
		LineTo(hdc, lineLength, drawPointY); 
	}
	bToggle=!bToggle;

	MoveToEx(hdc,drawPointX,drawPointY,NULL);
	ReleaseDC(hwndDraw, hdc);
	UpdateWindow(hwndDraw);
	return 0;
}

void restoreBackground(int pos){
	//hdc = GetDC(GetForegroundWindow());
	HWND hwndDraw;
	hwndDraw=FindWindow(L"HHTASKBAR", NULL);
	hdc = GetDC(hwndDraw);

	//move start point to x,y (ie 0,319
	MoveToEx(hdc,drawPointX,drawPointY,NULL);

	//draw a line to pos/drawpointY
	HPEN pen = (HPEN)SelectObject(hdc,lineOrg);	//save
	//ie to 30/2,319
	LineTo(hdc, lineLength, drawPointY);

	MoveToEx(hdc,drawPointX,drawPointY,NULL);
	
	SelectObject(hdc, pen);	//restore
	ReleaseDC(hwndDraw, hdc);
	UpdateWindow(hwndDraw);
}

//background thread to draw an animation
DWORD myThread(LPVOID lpParam){
	static BOOL bToggle=false;

	//line color pens
	lineBack = CreatePen(PS_SOLID, lineHeight, RGB(127,127,127));
	lineYellow = CreatePen(PS_SOLID, lineHeight, RGB(255,255,0));
	lineRed = CreatePen(PS_SOLID, lineHeight, RGB(255,0,0));
	lineGreen = CreatePen(PS_SOLID, lineHeight, RGB(0,255,0));
	lineOrange = CreatePen(PS_SOLID, lineHeight, RGB(0xff,0x90,0));

//	HWND hDesk;
	do{
		DrawLine(lineLength);
		bToggle=!bToggle;
		Sleep(1000);
	}while(runThread);
	ExitThread(0);
	return 0;
}

COLORREF getColor(){
	COLORREF _color = RGB(0,0,0);	//default to black
	/*
	HWND hwndDraw;
	hwndDraw=FindWindow(L"HHTASKBAR", NULL);
	hdc = GetDC(hwndDraw);

	EnterCriticalSection(&myCriticalSection);
	dwStatus=g_dwStatus;
	LeaveCriticalSection(&myCriticalSection);

	//move start point to x,y (ie 0,319
	MoveToEx(hdc,drawPointX,drawPointY,NULL);
	*/
	HDC _hdc = GetDC(FindWindow(L"HHTASKBAR", NULL));//NULL);
	if(_hdc)
	{
		POINT _cursor;
		//GetCursorPos(&_cursor);
		_cursor.x=drawPointX;
		_cursor.y=drawPointY;
		_color = GetPixel(_hdc, _cursor.x, _cursor.y);
		int _red = GetRValue(_color);
		int _green = GetGValue(_color);
		int _blue = GetBValue(_color);

		MYDEBUGMSG(L"Red: 0x%02x\n", _red);
		MYDEBUGMSG(L"Green: 0x%02x\n", _green);
		MYDEBUGMSG(L"Blue: 0x%02x\n", _blue);
	}
	return _color;
}

//simulate a mouse move or keydown
void doMouseMove(HWND hWndRDM){
	LRESULT lRes =0;

	//get some info on the window
	RECT rect;
	if(GetWindowRect(hWndRDM, &rect)){
		if(bUseLogging)
			MYDEBUGMSG(L"GetWindowRect: %i,%i : %i,%i\n", rect.left, rect.top, rect.right, rect.bottom);
		if(rect.top<0 || rect.bottom<0 || rect.left<0 || rect.right<0)
		{
			if(bUseLogging){
				MYDEBUGMSG(L"RDM active window outside screen (inactive?)\n");
				MYDEBUGMSG(L"RDM hwnd=0x%x, foreground hwnd=0x%x\n", hWndRDM, GetForegroundWindow());
			}
		}
	}
	else{
		if(bUseLogging)
			MYDEBUGMSG(L"GetWindowRect()failed. GetLastError()=%i\n", GetLastError());	//get 1400=invalid window handle on subsequent calls!?
	}
    ////Get foreground window -- this is not needed if RDM is launched Full-Screen as it was in this case
    //hWndForeground = GetForegroundWindow();
    //Sleep(500);

    //Give focus to the RDP Client window (even if the logon screen, in case user logged out in the meantime)
	if(bUseLogging)
		MYDEBUGMSG(L"SetForGroundWindow\n");
    SetForegroundWindow(hWndRDM);

    //The timer is reset when the rdp window receives mouse or keyboard input
    //with no MOUSEEVENTF_ABSOLUTE the move is relative
#ifdef USE_MOUSE
	//looks actually, as this (mouse_movement) does not help with TSSHELLWND, does not work for WinMo 6.5.3
	if(bUseLogging)
		nclog(L"MOUSEEVENTF_MOVE 1\n");
    mouse_event(MOUSEEVENTF_MOVE, 100, 0, 0, 0);	
    Sleep(250);
	if(bUseLogging)
		nclog(L"MOUSEEVENTF_MOVE 2\n");
    mouse_event(MOUSEEVENTF_MOVE, -100, 0, 0, 0);
#endif
	//mouse move: For lParam, the low word represents the x coordinate of the mouse and the High word represents the y coordinate of the mouse.
	// LRESULT SendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	lRes = SendMessage (hWndRDM, WM_MOUSEMOVE, NULL, MAKELPARAM(0x100,0));
		if(bUseLogging)
			MYDEBUGMSG(L"WM_MOUSEMOVE 1, lRes=%u\n", lRes);
	Sleep(20);
	lRes = SendMessage (hWndRDM, WM_MOUSEMOVE, NULL, MAKELPARAM(-0x100,0));
		if(bUseLogging)
			MYDEBUGMSG(L"WM_MOUSEMOVE 2, lRes=%u\n", lRes);

	if(_dwUseMouse==1){
		lRes = SendMessage(hWndRDM, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(_dwMouseX, _dwMouseY));
		if(bUseLogging)
			MYDEBUGMSG(L"MOUSEDOWN/UP 1, lRes=%u\n", lRes);
		Sleep(20);
		lRes = SendMessage(hWndRDM, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(_dwMouseX, _dwMouseY));
		if(bUseLogging)
			MYDEBUGMSG(L"MOUSEUP 2 DONE, lRes=%u\n", lRes);
	}
	else
		if(bUseLogging)
			MYDEBUGMSG(L"MouseClick will not be used\n");

	if(_dwUseKeyboard==1){
		//right
		//msg: 0x100, wParam: 0x27, lParam: 0x14d0001
		//msg: 0x101, wParam: 0x27, lParam: 0xc14d0001
		lRes = SendMessage(hWndRDM, WM_KEYDOWN, _dwKey1, _dwLPARAM11);
		if(bUseLogging)
			MYDEBUGMSG(L"WM_KEYDOWN 1, lRes=%u\n", lRes);
		Sleep(20);
		lRes = SendMessage(hWndRDM, WM_KEYUP, _dwKey1, _dwLPARAM12);
		if(bUseLogging)
			MYDEBUGMSG(L"WM_KEYUP 1, lRes=%u\n", lRes);
		//left
		//msg: 0x100, wParam: 0x25, lParam: 0x14b0001
		//msg: 0x101, wParam: 0x25, lParam: 0xc14b0001
		lRes = SendMessage(hWndRDM, WM_KEYDOWN, _dwKey2, _dwLPARAM21);
		if(bUseLogging)
			MYDEBUGMSG(L"WM_KEYDOWN 2, lRes=%u\n", lRes);
		Sleep(20);
		lRes = SendMessage(hWndRDM, WM_KEYUP, _dwKey2, _dwLPARAM22);
		if(bUseLogging)
			MYDEBUGMSG(L"WM_KEYUP 2, lRes=%u\n", lRes);
	}
	else
		if(bUseLogging)
			MYDEBUGMSG(L"Keyboard will not be used\n");

#ifdef USE_KEYBD
	////using keyboard event, does not work for WinMo 6.5.3
		if(bUseLogging)
			nclog(L"keybd_event 1\n");
	keybd_event(VK_PRINT, 0x37, KEYEVENTF_SILENT, 0);
	Sleep(10);
	if(bUseLogging)
		nclog(L"keybd_event 2\n");
	keybd_event(VK_PRINT, 0x37, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, 0);

	//using keyboard event, does not work for WinMo 6.5.3 with VK_NONAME
	if(bUseLogging)
		nclog(L"keybd_event 1\n");
	keybd_event(VK_NONAME, 0x00, KEYEVENTF_SILENT, 0);
	Sleep(10);
		if(bUseLogging)
			nclog(L"keybd_event 2\n");
	keybd_event(VK_NONAME, 0x00, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, 0);
#endif
	//one more unsuccessfull test
	//nclog(L"using setfocus\n");
	//SetFocus(hWndRDM);
	
    ////Give focus back to previous foreground window
    //SetForegroundWindow(hWndForeground);
}

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
 	// TODO: Place code here.
#if !DEBUG
	if(!isIntermec()){
		MessageBox(NULL, L"This is not an intermec handheld computer. Execution will be terminated.", 
			L"Intermec only check", MB_OK | MB_TOPMOST | MB_SETFOREGROUND);
		return -11;
	}
#endif
	//already running?
	BOOL bOwnerShip = false;
	LRESULT lRes =0;

	int iCounter=0;
	
	//read reg
	readReg();

	//allow only one instance
	if(bUseLogging)
		MYDEBUGMSG(L"CreateMutex...");
	HANDLE hMutex = CreateMutex(NULL, bOwnerShip, sMUTEX);
	DWORD dwErr = GetLastError();
	if(bUseLogging)
		MYDEBUGMSG(L"hMutex=0x0%0x, LastError=0x%0x\n", hMutex, dwErr);

	if(hMutex!=NULL){
		if(dwErr==ERROR_ALREADY_EXISTS){
			//allow only one instance
			if(bUseLogging)
				MYDEBUGMSG(L"Mutex already exists, Exit!\n");
			ReleaseMutex(hMutex);
			return -3;
		}
	}
	MYDEBUGMSG(L"no Mutex. Continueing..!\n");

    HWND hWndRDM = NULL;
	HWND hWinShell = NULL;
	HWND hTSC = NULL;

	//check command line args
	if(bUseLogging)
		MYDEBUGMSG(L"lpCmdLine= '%s'\n", lpCmdLine);

	//enable logging to file?
	if( wcslen(lpCmdLine)>0 && wcsstr(lpCmdLine, L"dologging")!=0)
	{
		bUseLogging=TRUE;
//		ncLogEnabled=TRUE;
	}
	else{
		bUseLogging=FALSE;
//		ncLogEnabled=FALSE;
	}
	if( wcslen(lpCmdLine)>0 && _wcsicmp(L"noRDPstart", lpCmdLine)==0)
	{
		if(bUseLogging)
			MYDEBUGMSG(L"RDP client will not be started, lpCmdLine 'noRDPstart' found\n");
	}
	else{
		//First check if TSC is already running???
		HWND hwndTemp = getTSChandle();
		if(hwndTemp!=NULL){	//found a running TSC window
			if(bUseLogging)
				MYDEBUGMSG(L"TSC is already running\n");
		}
		else{ //start a new TSC client
			//Firstly launch RDP Client
			SHELLEXECUTEINFO sei = {0};
			sei.cbSize = sizeof(sei);
			sei.nShow = SW_SHOWNORMAL;
#if _WIN32_WCE == 0x420
			sei.lpFile = TEXT("\\Windows\\mstsc40.exe");
#else
			sei.lpFile = TEXT("\\Windows\\wpctsc.exe");
#endif
			sei.lpParameters = TEXT(" ");
			if (!ShellExecuteEx(&sei))
			{
				MessageBox(NULL, TEXT("Couldn't launch RDP Client"), TEXT("Remote Desktop Launcher"), MB_OK | MB_TOPMOST | MB_SETFOREGROUND);
				goto Exit;
			}
			else{
				if(bUseLogging)
					MYDEBUGMSG(L"RDP client started, sleeping 500ms...\n");
				Sleep(500);
			}
		}
	}


	//get current color for restore
	COLORREF _colorOrg = getColor();
	lineOrg = CreatePen(PS_SOLID, lineHeight, _colorOrg);

	//thread to animate line bar at bottom
	hThread = CreateThread(NULL, 0, myThread, NULL, 0, &idThread);
	MYDEBUGMSG(L"Animation CreateThread handle=%u\n", hThread);

	//create the watch thread to be able to exit the app
	HANDLE hThreadWatch = CreateThread(NULL, 0, myWatchThread, NULL, 0, &idThreadWatch);
	MYDEBUGMSG(L"WatchDog thread handle=%u\n", hThreadWatch);

	// Initialize the critical section one time only.
    InitializeCriticalSection(&myCriticalSection);

	//Now every X minutes check if it's still running and if so "refresh" its window
    //if it's no longer running, exit
	uint ii=0;
    do 
    {
        //check if RDP Client is running, otherwise exit?
		if(bUseLogging)
			MYDEBUGMSG(L"FindWindow '%s'...\n", szMainRDMWindowsClass);
        hWndRDM = getTSChandle();	//get correct window handle?, test with GetWindowRect()
		if(hWndRDM!=NULL){
			if(bUseLogging)
				MYDEBUGMSG(L"'%s' found.\n", szMainRDMWindowsClass);
			//window found
			EnterCriticalSection(&myCriticalSection);

			TCHAR szTitle[MAX_PATH];
			TCHAR* pFound=NULL;
			int pos;
			//find if window text starts with text
#if _WIN32_WCE == 0x420
			TCHAR* szSearch = L"Terminal Services Client";
			HWND hwndTest = FindWindow(L"UIMainClass", NULL);
#else
			TCHAR* szSearch = szRDMconnectedSessionText;
			HWND hwndTest = FindWindow(szMainRDMWindowsClass, NULL);
#endif
			if(hwndTest!=NULL){
				MYDEBUGMSG(L"check for %s is active\n", szMainRDMWindowsClass);
				GetWindowText(hwndTest, szTitle, MAX_PATH);
				pFound=wcsstr(szTitle, szSearch);
				if(pFound!=NULL){
					MYDEBUGMSG(L"Found '%s' in title\n", szRDMconnectedSessionText);
					pos=(int)(pFound-szTitle + 1);
					if(pos==1) //the window text starts with "Remote Desktop Mobile"
					{
						MYDEBUGMSG(L"'Remote Desktop Mobile/Terminal Services Client' is idle\n");
						g_dwStatus=idle;
					}
					else
					{
						MYDEBUGMSG(L"'Remote Desktop Mobile/Terminal Services Client' is active\n");
						g_dwStatus=active;
					}
				}
				else{
					MYDEBUGMSG(L"'Remote Desktop Mobile/Terminal Services Client' is not active\n");
					g_dwStatus=inactive;
				}
			}
			else{
				MYDEBUGMSG(L"'Remote Desktop Mobile/Terminal Services Client' is not found\n");
				g_dwStatus=notfound;
			}

			LeaveCriticalSection(&myCriticalSection);

			//did we reach the timeout
			if(ii==0 && g_dwStatus==active){
				MYDEBUGMSG(L"Calling doMouseMove()\n");
				doMouseMove(hWndRDM);
			}
        }//hWndRDM
        else 
        {	
			//window not found
			MYDEBUGMSG(L"FindWindow failed for '%s'\n", szMainRDMWindowsClass);
            //Sleep(5000);
			EnterCriticalSection(&myCriticalSection);
			g_dwStatus=notfound;
			LeaveCriticalSection(&myCriticalSection);
			//RDP Client is not running - let's exit
            //goto Exit;
        }//hWndRDM

		//sleep and count
		ii++;
		if(ii > _dwSLEEPTIME/1000){
			if(bUseLogging)
				MYDEBUGMSG(L"Counter reset\n");
			ii=0; //reset counter
		}
        //Sleep 
		if(bUseLogging)
			MYDEBUGMSG(L"Sleep ...\n");
		Sleep(1000);
    }
    while (_bRunApp); //The check (NULL != hWndRDM) is already done inside the loop


Exit:
	MYDEBUGMSG(L"Exit!\n");
    // Release resources used by the critical section object.
	EnterCriticalSection(&myCriticalSection);
	g_dwStatus=stopped;
	LeaveCriticalSection(&myCriticalSection);

    DeleteCriticalSection(&myCriticalSection);

	restoreBackground(lineLength);

    if (NULL != hWndRDM)
        CloseHandle(hWndRDM);
	if(hMutex!=NULL){
		ReleaseMutex(hMutex);
	}
	if(hThread){
		runThread=FALSE;
		Sleep(1000);
	}
	if(hThreadWatch){
		if(hEventWatch){
			SetEvent(hEventWatch);
		}
	}
    return 0;

}

