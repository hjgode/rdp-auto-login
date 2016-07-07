#include "stdafx.h"

#include "tlhelp32.h"
#pragma comment(lib, "toolhelp.lib")
#ifndef TH32CS_SNAPNOHEAPS
	#define TH32CS_SNAPNOHEAPS 0x40000000
#endif

#define TSC_EXE L"cetsc.exe"

TCHAR* szRDPfile;
TCHAR szServer[MAX_PATH];
TCHAR szUser[MAX_PATH];
TCHAR szPassword[MAX_PATH];

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

/*
"Dialog"  "Remote Desktop Connection"	0x0
"static"  "Enter your credentials for 199.64.70.66. These credentials will be used when you connect to the remote computer."	0x3f1
"static"  "&User name:"	0xffff
"combobox"  "GE3SDT8KLRL4J\E841719"	0x3e8
"Edit"  "GE3SDT8KLRL4J\E841719"	0x3e9
"Button"  "&..."	0x3e9
"static"  "&Password:"	0x6a
"Edit"  ""	0x3ea
"Button"  "&Remember my password"	0x3eb
"Button"  "OK"	0x1
"Button"  "Cancel"	0x2

"Dialog"  "Remote Desktop Connection"	0x0
"static"  "&Computer:"	0x1393
"combobox"  "199.64.70.66"	0x1394
"Edit"  "199.64.70.66"	0x3e9
"Button"  "&Options >>"	0x1396
"Button"  "Co&nnect"	0x1
"SysTabControl32"  "Tab1"	0x1397
"listbox"  ""	0x3e8

"RdpClipRdrWindowClass"  ""	0x0
"TSSHELLWND"  "Remote Desktop Connection"	0x0
"TSCAXHOST"  ""	0x0
"ATL:43C75A38"  ""	0x2b57dc8
*/

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
	DEBUGMSG(1, (L"IsProcessRunning-called for '%s'\n", pname));
    HANDLE          hProcList;
    PROCESSENTRY32  peProcess;
    DWORD           thDeviceProcessID;
    TCHAR           lpname[MAX_PATH];
    if ( wcslen(pname)==0 )
    {
        return FALSE;
    }

    wsprintf( lpname, L"%s", pname );
    _tcslwr( lpname );
	DEBUGMSG(1, (L"IsProcessRunning-looking for '%s'\n", lpname));
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
		DEBUGMSG(1, (L"IsProcessRunning-found: %s\n", peProcess.szExeFile));
        if ( wcsicmp( peProcess.szExeFile, lpname ) == 0) //replaced wcsstr by wcsicmp
        {
			DEBUGMSG(1, (L"IsProcessRunning-MATCH: %s\n", lpname));
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
BOOL foundIt;
BOOL killedIt;
HWND hWindow;

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

int KillAllExe(TCHAR* exefile){
	int numKilled=0;
	DWORD dwPID = FindPID(exefile);
	DEBUGMSG(1, (L"KillAllExe for %s\n", exefile));
	if ( (dwPID=FindPID(exefile)) != 0)
	{
		DEBUGMSG(1, (L"KillAllExe-found pID: 0x%08x", dwPID));
		//try the hard way
		HANDLE hProcess = OpenProcess(0, FALSE, dwPID);
		if (hProcess != NULL)
		{
			DEBUGMSG(1, (L"KillAllExe-found hProcess: 0x%08x\n", hProcess));
			DWORD uExitCode=0;
			if ( TerminateProcess (hProcess, uExitCode) != 0)
			{
				DEBUGMSG(1, (L"KillAllExe-TerminateProcess OK\n"));
				//app terminated
				numKilled++;
			}
			else
				DEBUGMSG(1, (L"KillAllExe-TerminateProcess failed: %i\n", GetLastError()));
		}
		else
			DEBUGMSG(1, (L"KillAllExe-hProcess is NULL\n"));

	}
	return numKilled;
}

int fillTSCdialog(HWND hwnd){
	//fill in server in first dialog
	//TODO: skip if rdp file has been provided as then the second dialog is shown directly!
	SetDlgItemText(hwnd, 0x3e9, szServer);
	//click OK
	HWND hwndConnect=GetDlgItem(hwnd, 0x01);
	PostMessage(hwndConnect, BM_CLICK, NULL, NULL);
	Sleep(1000);
	//find new dialog
	TCHAR winText[64];
	HWND hwndDlg2 = FindWindow(L"Dialog", L"Remote Desktop Connection");
	BOOL bFound=FALSE;
	if(hwndDlg2 != NULL){
		if(hwndDlg2 == hwnd){
			//get next window
			while( (hwndDlg2=GetWindow(hwnd, GW_HWNDNEXT)) != NULL && (hwndDlg2!=hwnd) ){
				GetWindowText(hwndDlg2, winText, 64);
				if(wcsicmp(winText, L"Remote Desktop Connection")){
					DEBUGMSG(1, (L"fillTSCdialog-GetWindow()=0x%08x\n", hwndDlg2));
					bFound=TRUE;
					break;
				}
			}
		}
		else{
			bFound=TRUE;
		}
	}
	if(bFound){
		//fill in user and password
		SetDlgItemText(hwndDlg2, 0x3e8, szUser);
		SetDlgItemText(hwndDlg2, 0x3ea, szPassword);
		HWND hwndOK = GetDlgItem(hwndDlg2, 0x01);
		PostMessage(hwndOK, BM_CLICK, NULL, NULL);
#if DEBUG
		//scanTscWindow(hwndDlg2);
#endif
	}
	return 0;
}

int startCETSC(TCHAR* rdpfile, TCHAR* server, TCHAR* user, TCHAR* pass){
	
	if(rdpfile!=NULL && wcslen(rdpfile)>0){
		szRDPfile=new TCHAR(wcslen(rdpfile)*sizeof(TCHAR));
		wsprintf(szRDPfile, L"%s", rdpfile);
	}
	else{
		szRDPfile=new TCHAR(2*sizeof(TCHAR));
		wsprintf(szRDPfile, L"");
	}
	//get data
	if(wcslen(server)>0)
		wsprintf(szServer, L"%s", server);
	else
		wsprintf (szServer, L"199.64.70.66");
	if(wcslen(user)>0)
		wsprintf(szUser, L"%s", user);
	else
		wsprintf (szUser, L"rdesktop");
	if(wcslen(pass)>0)
		wsprintf(szPassword, L"%s", pass);
	else
		wsprintf (szPassword, L"rdesktop");

//try to launch cetsc
	DEBUGMSG(1, (L"Entering startTSC()\n"));
	int cnt=0; //repeat counter
	BOOL isOK = FALSE;
	int iRet=0;

	do{
		cnt++;
		//if tsc is already running, kill it

		//first ensure TSSHELLWND is not minimized or connect will hang (why?)
#if _WIN32_WCE == 0x420
		HWND hwndTSC = FindWindow(L"UIMainClass", NULL);//FindWindow(NULL, L"Terminal Services Client");	
		if(hwndTSC==NULL)
			hwndTSC = FindWindow(NULL, L"Terminal Services Client");
		//at start we see the 'connect' dialog
		//in a connected session the class and title changes!
#else
		HWND hwndTSC = FindWindow(L"TSSHELLWND",NULL);
#endif
		DEBUGMSG(1, (L"TSC is running as window: 0x%08x\n",hwndTSC));

		if(hwndTSC!=NULL)
			//ShowWindow(hwndTSC, SW_SHOWNORMAL);
#if _WIN32_WCE == 0x420
		if(IsProcessRunning(L"mstsc40.exe")){		//on pocketpc we have mstsc40.exe
			if( KillExeWindow(L"mstsc40.exe") ){
#elif _WIN32_WCE == 0x700 || _WIN32_WCE == 0x500
		while(IsProcessRunning(L"cetsc.exe")){
			if( KillAllExe(L"cetsc.exe")>0 ){
#else
		if(IsProcessRunning(L"wpctsc.exe")){
			if( KillExeWindow(L"wpctsc.exe") ){
#endif
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
//		writeRDP();
//		writeMRU(); //ensure at least one entry is in MRU

		DWORD dProcIDTSC=0; //to save proc ID of TSC main window
		//start a new instance of tsc
		PROCESS_INFORMATION pi;
		TCHAR lpCmdLine[MAX_PATH];
		if(rdpfile==NULL)
			wsprintf(lpCmdLine, L"");
		else
			wsprintf(lpCmdLine, L"%s /v:%s", szRDPfile, szServer);
#if _WIN32_WCE == 0x420
		if (CreateProcess(L"\\windows\\mstsc40.exe", lpCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi)!=0)
#elif _WIN32_WCE == 0x700 || _WIN32_WCE == 0x500
		if (CreateProcess(L"\\windows\\cetsc.exe", lpCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi)!=0)
#else
		if (CreateProcess(L"\\windows\\wpctsc.exe", L"", NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi)!=0)
#endif
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
#if _WIN32_WCE == 0x420
		DWORD pidTSC = FindPID(L"mstsc40.exe");
#elif _WIN32_WCE == 0x700 || _WIN32_WCE == 0x500
		DWORD pidTSC = FindPID(L"cetsc.exe");
#else
		DWORD pidTSC = FindPID(L"wpctsc.exe");
#endif
		HWND hTscDialog = getTscDialog(pidTSC);  //FindWindow(L"Dialog", NULL);

		if(hTscDialog!=NULL){
			//check if this is the right window
#if _WIN32_WCE == 0x420
			if(FindPID(L"mstsc40.exe") != 0){
				if(FindPID(hTscDialog)!= FindPID(L"mstsc40.exe")){
#elif _WIN32_WCE == 0x700 || _WIN32_WCE == 0x500
			if(FindPID(L"cetsc.exe") != 0){
				if(FindPID(hTscDialog)!= FindPID(L"cetsc.exe")){
#else
			if(FindPID(L"wpctsc.exe") != 0){
				if(FindPID(hTscDialog)!= FindPID(L"wpctsc.exe")){
#endif
					iRet = -4; //error finding TSC dialog
					continue;
				}
				else{
#ifdef DEBUG
					DEBUGMSG(1, (L" ### ScanTSCwindow ### \r\n"));
					scanTscWindow(hTscDialog); //scan TSC window and list child windows and CtrlIDs
					DEBUGMSG(1, (L" --- ScanTSCwindow --- \r\n"));
#endif
					fillTSCdialog(hTscDialog);
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