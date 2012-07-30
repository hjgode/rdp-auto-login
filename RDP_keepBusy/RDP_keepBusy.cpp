// RDP_keepBusy.cpp : Defines the entry point for the application.
//

#include "stdafx.h"


#ifdef DEBUG
	//1 second
	#define SLEEPTIME 1000 
#else
	//4 minutes 
	#define SLEEPTIME 1000*60*4 
#endif

//at least define one of both to use to keep RDM busy
#define USE_MOUSE
//#define USE_KEYBD

#define sMUTEX L"RDMKEEPBUSY"

int		lineHeight=1;		//progress bar height
int		drawPoint=319;
int		BarPosition=0;
int		screenWidth = GetSystemMetrics(SM_CXSCREEN);
HDC		hdc = GetDC(NULL);
HPEN	lineProgress,lineBackground,po;	// Pen Progress bar

HWND findWindow(HWND hWndStart, TCHAR* szTitle){

	HWND hWnd = NULL;
	HWND hWnd1 = NULL;

	static BOOL bFoundIt=FALSE;
	static HWND hFoundHWND=NULL;

	hWnd = hWndStart;
	TCHAR cszWindowString [MAX_PATH]; // = new TCHAR(MAX_PATH);
	TCHAR cszClassString [MAX_PATH]; //= new TCHAR(MAX_PATH);

	while (hWnd!=NULL && !bFoundIt){
		GetClassName(hWnd, cszClassString, MAX_PATH);
		GetWindowText(hWnd, cszWindowString, MAX_PATH);
		nclog(L"findWindow: \"%s\"  \"%s\"\n", cszClassString, cszWindowString);

		if(wcscmp(cszWindowString, szTitle)==0){
			bFoundIt=TRUE;
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

HWND getTSChandle(){
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

	HWND hTSCWND = findWindow(hWndRDM, L"Input Capture Window");
	return hTSCWND;
}

int DrawLine(int pos){
	MoveToEx(hdc,0,drawPoint,NULL);

	po = (HPEN)SelectObject(hdc,lineBackground); 
	LineTo(hdc,pos,drawPoint); 

    SelectObject(hdc,lineProgress);
    LineTo(hdc,screenWidth,drawPoint);

	SelectObject(hdc,lineBackground);
    LineTo(hdc,screenWidth,drawPoint);

return 0;
}

VOID CALLBACK myTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){
	static BOOL bToggle=false;
	HWND hDesk = GetForegroundWindow();
	//HDC hdc = GetDC(hDesk);
	//HBRUSH hBrushGreen = CreateSolidBrush(RGB(0,255,0));
	//HBRUSH hBrushRed = CreateSolidBrush(RGB(255,0,0));

	RECT rect;
	GetWindowRect(hDesk, &rect);
	//rect.top=0;
	//rect.left=0;
	//rect.bottom=1;
	if(bToggle)
		DrawLine(rect.right);
		//FillRect(hdc, &rect, hBrushRed);
	else
		DrawLine(rect.right/2);
		//FillRect(hdc, &rect, hBrushGreen);
	bToggle=!bToggle;
	//ReleaseDC(hDesk, hdc);
}

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
 	// TODO: Place code here.
	//already running?
	BOOL bOwnerShip = false;

	//allow only one instance
	nclog(L"CreateMutex...");
	HANDLE hMutex = CreateMutex(NULL, bOwnerShip, sMUTEX);
	DWORD dwErr = GetLastError();
	nclog(L"hMutex=0x0%0x, LastError=0x%0x\n", hMutex, dwErr);

	if(hMutex!=NULL){
		if(dwErr==ERROR_ALREADY_EXISTS){
			//allow only one instance
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
	nclog(L"lpCmdLine= '%s'\n", lpCmdLine);

	if( wcslen(lpCmdLine)>0 && _wcsicmp(L"noRDPstart", lpCmdLine)==0)
	{
		nclog(L"RDP client will not be started, lpCmdLine 'noRDPstart' found\n");
	}
	else{
		//First check if TSC is already running???
		//Firstly launch RDP Client
		SHELLEXECUTEINFO sei = {0};
		sei.cbSize = sizeof(sei);
		sei.nShow = SW_SHOWNORMAL; 
		sei.lpFile = TEXT("\\Windows\\wpctsc.exe");
		sei.lpParameters = TEXT(" ");
		if (!ShellExecuteEx(&sei))
		{
			MessageBox(NULL, TEXT("Couldn't launch RDP Client"), TEXT("Remote Desktop Launcher"), MB_OK | MB_TOPMOST | MB_SETFOREGROUND);
			goto Exit;
		}
		else{
			nclog(L"RDP client started, sleeping 500ms...\n");
			Sleep(500);
		}
	}
	lineProgress = CreatePen(PS_SOLID,lineHeight,RGB(255,0,0)); // Progress bar
	SetTimer(NULL, 11011, 1000, myTimerProc);
    //Now every X minutes check if it's still running and if so "refresh" its window
    //if it's no longer running, exit
    do 
    {
        //check if RDP Client is running, otherwise exit
		nclog(L"FindWindow 'TSSHELLWND'...\n");
        hWndRDM = getTSChandle();
		if(hWndRDM!=NULL){

			//get some info on the window
			RECT rect;
			GetWindowRect(hWndRDM, &rect);
			nclog(L"GetWindowRect: %i,%i : %i,%i\n", rect.left, rect.top, rect.right, rect.bottom);
            ////Get foreground window -- this is not needed if RDM is launched Full-Screen as it was in this case
            //hWndForeground = GetForegroundWindow();
            //Sleep(500);

            //Give focus to the RDP Client window (even if the logon screen, in case user logged out in the meantime)
			nclog(L"SetForGroundWindow\n");
            SetForegroundWindow(hWndRDM);

            //The timer is reset when the rdp window receives mouse or keyboard input
            //with no MOUSEEVENTF_ABSOLUTE the move is relative
#ifdef USE_MOUSE
			//looks actually, as this (mouse_movement) does not help with TSSHELLWND
			nclog(L"MOUSEEVENTF_MOVE 1\n");
            mouse_event(MOUSEEVENTF_MOVE, 100, 0, 0, 0);	
            Sleep(250);
			nclog(L"MOUSEEVENTF_MOVE 2\n");
            mouse_event(MOUSEEVENTF_MOVE, -100, 0, 0, 0);
#endif
#ifdef USE_KEYBD
			//using keyboard event
			nclog(L"keybd_event 1\n");
			keybd_event(VK_PRINT, 0x37, KEYEVENTF_SILENT, 0);
			Sleep(10);
			nclog(L"keybd_event 2\n");
			keybd_event(VK_PRINT, 0x37, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, 0);
#endif
			//one more unsuccessfull test
			//nclog(L"using setfocus\n");
			//SetFocus(hWndRDM);
			
            ////Give focus back to previous foreground window
            //SetForegroundWindow(hWndForeground);

            //Sleep 
			nclog(L"Sleep ...\n");
            Sleep(SLEEPTIME);
        }
        else 
        {
			nclog(L"FindWindow failed for 'TSSHELLWND'\n");
            //RDP Client is not running - let's exit
            goto Exit;
        }
    }
    while (TRUE); //The check (NULL != hWndRDM) is already done inside the loop


Exit:
	nclog(L"Exit!\n");
    if (NULL != hWndRDM)
        CloseHandle(hWndRDM);
	if(hMutex!=NULL){
		ReleaseMutex(hMutex);
	}
    return 0;

}

