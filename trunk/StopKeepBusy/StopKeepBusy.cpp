// StopKeepBusy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define sEvent L"STOPRDMKEEPBUSY"

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD dwRet=0;
	HANDLE hEventWatch= CreateEvent(NULL, TRUE, FALSE, sEvent);
	if(hEventWatch!=NULL){
		if(GetLastError()==ERROR_ALREADY_EXISTS){
			SetEvent(hEventWatch);
			MessageBox(NULL, L"RDPKeepbusy should stop now.\n", L"Stop RDPKeepbusy", MB_OK);
		}
		else{
			MessageBox(NULL, L"CreateEvent: does not yet exist\n", L"Stop RDPKeepbusy", MB_OK);
		}
	}
	else{
		MessageBox(NULL, L"CreateEvent failed\n", L"Stop RDPKeepbusy", MB_OK);
	}
	return 0;
}

