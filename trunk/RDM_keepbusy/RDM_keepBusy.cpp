// RDM_keepBusy.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

BOOL bUseLogging=FALSE;
#if bUseLogging
	#pragma comment (exestr, "with logging")
	#pragma message ("### compiled with logging ###")
#else
	#pragma comment (exestr, "no logging")
	#pragma message ("### compiled without logging ###")
#endif

#define TEST
#undef TEST

#ifdef TEST
	//4 second
	DWORD _dwSLEEPTIME = 4000;
#else
	//4 minutes 
	DWORD _dwSLEEPTIME = 1000*60*4;
#endif

#define sMUTEX L"RDMKEEPBUSY"		//allow only one instance
#define sEvent L"STOPRDMKEEPBUSY"	//let RDMKeepbusy shutdown
BOOL _bRunApp = TRUE;				//the var that lets exit the code
DWORD idThreadWatch=0;				//the thread that waits for shutdown event
HANDLE hEventWatch=NULL;			//let the background thread show the actual status

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
		return lRes;
	}
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);

	DWORD dwData = drawPointX;
	if( lRes = RegSetValueEx (hKey, L"lineX", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;
	
	dwData = drawPointY; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lineY", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;

	dwData = lineLength; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lineLen", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;

	dwData = lineHeight; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lineWidth", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;

	dwData = _dwKey1; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"Key1", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;

	dwData = _dwKey2; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"Key2", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;

	dwData = _dwLPARAM11; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lParam11", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;

	dwData = _dwLPARAM12; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lParam12", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;

	dwData = _dwLPARAM21; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lParam21", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;

	dwData = _dwLPARAM22; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"lParam22", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;

	dwData = _dwUseMouse; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"useMouse", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;
	
	dwData = _dwMouseX; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"mouseX", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;

	dwData = _dwMouseY; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"mouseY", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;

	dwData = _dwSLEEPTIME / 1000; dwSize = sizeof(DWORD);
	if( lRes = RegSetValueEx (hKey, L"sleepTime", 0, dwType, (BYTE*) &dwData, dwSize) != ERROR_SUCCESS)
		return lRes;

	RegCloseKey(hKey);
	return lRes;
}

int readReg(){
	int iRet=0;
	HKEY hKey;
	LRESULT lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\RDMKeepbusy", 0, KEY_ALL_ACCESS, &hKey); 
	if(lRes!=ERROR_SUCCESS){		
		return writeReg();
	}
	DWORD dwData=0, dwSize=sizeof(DWORD), dwType = REG_DWORD;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"lineX", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -51;
	drawPointX=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"lineY", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -52;
	drawPointY=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"lineLen", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -53;
	lineLength=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"lineWidth", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -54;
	lineHeight=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"Key1", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -11;
	_dwKey1=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"lParam11", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -12;
	_dwLPARAM11=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"lParam12", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -13;
	_dwLPARAM12=dwData;

	dwSize=sizeof(DWORD); dwType = REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"Key2", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -21;
	_dwKey2=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"lParam21", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -22;
	_dwLPARAM21=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"lParam22", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -23;
	_dwLPARAM22=dwData;

	lRes = RegQueryValueEx(hKey, L"useMouse", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -31;
	if(dwData==1)
		_dwUseMouse=1;
	else
		_dwUseMouse=0;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"mouseX", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -32;
	_dwMouseX=dwData;

	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"mouseY", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -33;
	_dwMouseY=dwData;

	//read sleep time in seconds
	dwSize=sizeof(DWORD); dwType=REG_DWORD;
	lRes = RegQueryValueEx(hKey, L"sleepTime", NULL, &dwType, (LPBYTE)&dwData, &dwSize);
	if(lRes!=ERROR_SUCCESS)
		return -41;
	_dwSLEEPTIME=dwData*1000; 

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
			nclog(L"findWindow: \"%s\"  \"%s\"\n", cszClassString, cszWindowString);

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
			nclog(L"findWindow: \"%s\"  \"%s\"\n", cszClassString, cszWindowString);

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
	HWND hTSCWND = findWindow(hWndRDM, L"Input Capture Window");
	if(hTSCWND)
		DEBUGMSG(1, (L"getTSChandle(): Found Input Capture Window, Handle=0x%0x\n", hTSCWND));
	else
		DEBUGMSG(1, (L"getTSChandle(): No Input Capture Windowfound\n"));
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
	HWND hWndRDM = FindWindow(_T("TSSHELLWND"), NULL);

	//init find vars moved from being static inside findWindow() and findWindowByClass(), v15.01.2013
	bFoundWindow=FALSE;
	bFoundClass=FALSE;
	hFoundHWND=NULL;
	hFoundHWNDClass=NULL;
	HWND hTSCWND = findWindow(hWndRDM, L"Input Capture Window");
	if(hTSCWND)
		DEBUGMSG(1, (L"getTSChandle(): Found Input Capture Window, Handle=0x%0x\n", hTSCWND));
	else
		DEBUGMSG(1, (L"getTSChandle(): No Input Capture Windowfound\n"));
	return hTSCWND;
}


//thread that controls the exit of the app
DWORD myWatchThread(LPVOID lpParam){
	DWORD dwRet=0;
	hEventWatch= CreateEvent(NULL, TRUE, FALSE, sEvent);
	if(hEventWatch!=NULL){
		if(GetLastError()==ERROR_ALREADY_EXISTS){
			if(bUseLogging)
				nclog(L"CreateEvent: Event '%s' already exists\n", sEvent);
		}
		else
			if(bUseLogging)
				nclog(L"CreateEvent: Event '%s' did not yet exist\n", sEvent);
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
			nclog(L"CreateEvent failed: GetLastError=%u\n", GetLastError());
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

		DEBUGMSG(1,(L"Red: 0x%02x\n", _red));
		DEBUGMSG(1,(L"Green: 0x%02x\n", _green));
		DEBUGMSG(1,(L"Blue: 0x%02x\n", _blue));
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
			nclog(L"GetWindowRect: %i,%i : %i,%i\n", rect.left, rect.top, rect.right, rect.bottom);
		if(rect.top<0 || rect.bottom<0 || rect.left<0 || rect.right<0)
		{
			if(bUseLogging){
				nclog(L"RDM active window outside screen (inactive?)\n");
				nclog(L"RDM hwnd=0x%x, foreground hwnd=0x%x\n", hWndRDM, GetForegroundWindow());
			}
		}
	}
	else{
		if(bUseLogging)
			nclog(L"GetWindowRect()failed. GetLastError()=%i\n", GetLastError());	//get 1400=invalid window handle on subsequent calls!?
	}
    ////Get foreground window -- this is not needed if RDM is launched Full-Screen as it was in this case
    //hWndForeground = GetForegroundWindow();
    //Sleep(500);

    //Give focus to the RDP Client window (even if the logon screen, in case user logged out in the meantime)
	if(bUseLogging)
		nclog(L"SetForGroundWindow\n");
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
			nclog(L"WM_MOUSEMOVE 1, lRes=%u\n", lRes);
	Sleep(20);
	lRes = SendMessage (hWndRDM, WM_MOUSEMOVE, NULL, MAKELPARAM(-0x100,0));
		if(bUseLogging)
			nclog(L"WM_MOUSEMOVE 2, lRes=%u\n", lRes);

	if(_dwUseMouse==1){
		lRes = SendMessage(hWndRDM, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(_dwMouseX, _dwMouseY));
		if(bUseLogging)
			nclog(L"MOUSEDOWN/UP 1, lRes=%u\n", lRes);
		Sleep(20);
		lRes = SendMessage(hWndRDM, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(_dwMouseX, _dwMouseY));
		if(bUseLogging)
			nclog(L"MOUSEUP 2 DONE, lRes=%u\n", lRes);
	}
	else
		if(bUseLogging)
			nclog(L"MouseClick will not be used\n");

	if(_dwUseKeyboard==1){
		//right
		//msg: 0x100, wParam: 0x27, lParam: 0x14d0001
		//msg: 0x101, wParam: 0x27, lParam: 0xc14d0001
		lRes = SendMessage(hWndRDM, WM_KEYDOWN, _dwKey1, _dwLPARAM11);
		if(bUseLogging)
			nclog(L"WM_KEYDOWN 1, lRes=%u\n", lRes);
		Sleep(20);
		lRes = SendMessage(hWndRDM, WM_KEYUP, _dwKey1, _dwLPARAM12);
		if(bUseLogging)
			nclog(L"WM_KEYUP 1, lRes=%u\n", lRes);
		//left
		//msg: 0x100, wParam: 0x25, lParam: 0x14b0001
		//msg: 0x101, wParam: 0x25, lParam: 0xc14b0001
		lRes = SendMessage(hWndRDM, WM_KEYDOWN, _dwKey2, _dwLPARAM21);
		if(bUseLogging)
			nclog(L"WM_KEYDOWN 2, lRes=%u\n", lRes);
		Sleep(20);
		lRes = SendMessage(hWndRDM, WM_KEYUP, _dwKey2, _dwLPARAM22);
		if(bUseLogging)
			nclog(L"WM_KEYUP 2, lRes=%u\n", lRes);
	}
	else
		if(bUseLogging)
			nclog(L"Keyboard will not be used\n");

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
	
	//allow only one instance
	if(bUseLogging)
		nclog(L"CreateMutex...");
	HANDLE hMutex = CreateMutex(NULL, bOwnerShip, sMUTEX);
	DWORD dwErr = GetLastError();
	if(bUseLogging)
		nclog(L"hMutex=0x0%0x, LastError=0x%0x\n", hMutex, dwErr);

	if(hMutex!=NULL){
		if(dwErr==ERROR_ALREADY_EXISTS){
			//allow only one instance
			if(bUseLogging)
				nclog(L"Mutex already exists, Exit!\n");
			ReleaseMutex(hMutex);
			return -3;
		}
	}
	nclog(L"no Mutex. Continueing..!\n");

    HWND hWndRDM = NULL;
	HWND hWinShell = NULL;
	HWND hTSC = NULL;

	//check command line args
	if(bUseLogging)
		nclog(L"lpCmdLine= '%s'\n", lpCmdLine);

	//enable logging to file?
	if( wcslen(lpCmdLine)>0 && wcsstr(lpCmdLine, L"dologging")!=0)
	{
		bUseLogging=TRUE;
		ncLogEnabled=TRUE;
	}
	else{
		bUseLogging=FALSE;
		ncLogEnabled=FALSE;
	}
	if( wcslen(lpCmdLine)>0 && _wcsicmp(L"noRDPstart", lpCmdLine)==0)
	{
		if(bUseLogging)
			nclog(L"RDP client will not be started, lpCmdLine 'noRDPstart' found\n");
	}
	else{
		//First check if TSC is already running???
		HWND hwndTemp = getTSChandle();
		if(hwndTemp!=NULL){	//found a running TSC window
			if(bUseLogging)
				nclog(L"TSC is already running\n");
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
					nclog(L"RDP client started, sleeping 500ms...\n");
				Sleep(500);
			}
		}
	}

	//read reg
	readReg();

	//get current color for restore
	COLORREF _colorOrg = getColor();
	lineOrg = CreatePen(PS_SOLID, lineHeight, _colorOrg);

	//thread to animate line bar at bottom
	hThread = CreateThread(NULL, 0, myThread, NULL, 0, &idThread);
	DEBUGMSG(1, (L"Animation CreateThread handle=%u\n", hThread));

	//create the watch thread to be able to exit the app
	HANDLE hThreadWatch = CreateThread(NULL, 0, myWatchThread, NULL, 0, &idThreadWatch);
	DEBUGMSG(1, (L"WatchDog thread handle=%u\n", hThreadWatch));

	// Initialize the critical section one time only.
    InitializeCriticalSection(&myCriticalSection);

	//Now every X minutes check if it's still running and if so "refresh" its window
    //if it's no longer running, exit
	uint ii=0;
    do 
    {
        //check if RDP Client is running, otherwise exit?
		if(bUseLogging)
			nclog(L"FindWindow 'TSSHELLWND'...\n");
        hWndRDM = getTSChandle();	//get correct window handle?, test with GetWindowRect()
		if(hWndRDM!=NULL){
			if(bUseLogging)
				nclog(L"'TSSHELLWND' found.\n");
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
			TCHAR* szSearch = L"Remote Desktop Mobile";
			HWND hwndTest = FindWindow(L"TSSHELLWND", NULL);
#endif
			if(hwndTest!=NULL){
				nclog(L"check for TSSHELLWND is active\n");
				GetWindowText(hwndTest, szTitle, MAX_PATH);
				pFound=wcsstr(szTitle, szSearch);
				if(pFound!=NULL){
					if(bUseLogging)
						nclog(L"Found 'Remote Desktop Mobile/Terminal Services Client' in title\n");
					pos=(int)(pFound-szTitle + 1);
					if(pos==1) //the window text starts with "Remote Desktop Mobile"
					{
						if(bUseLogging)
							nclog(L"'Remote Desktop Mobile/Terminal Services Client' is idle\n");
						g_dwStatus=idle;
					}
					else
					{
						if(bUseLogging)
							nclog(L"'Remote Desktop Mobile/Terminal Services Client' is active\n");
						g_dwStatus=active;
					}
				}
				else{
					if(bUseLogging)
						nclog(L"'Remote Desktop Mobile/Terminal Services Client' is not active\n");
					g_dwStatus=inactive;
				}
			}
			else{
				if(bUseLogging)
					nclog(L"'Remote Desktop Mobile/Terminal Services Client' is not found\n");
				g_dwStatus=notfound;
			}

			LeaveCriticalSection(&myCriticalSection);

			//did we reach the timeout
			if(ii==0 && g_dwStatus==active){
				if(bUseLogging)
					nclog(L"Calling doMouseMove()\n");
				doMouseMove(hWndRDM);
			}
        }//hWndRDM
        else 
        {	
			//window not found
			if(bUseLogging)
				nclog(L"FindWindow failed for 'TSSHELLWND'\n");
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
				nclog(L"Counter reset\n");
			ii=0; //reset counter
		}
        //Sleep 
		if(bUseLogging)
			nclog(L"Sleep ...\n");
		Sleep(1000);
    }
    while (_bRunApp); //The check (NULL != hWndRDM) is already done inside the loop


Exit:
	if(bUseLogging)
		nclog(L"Exit!\n");
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

