// tsc-autologin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "./cetsc_dialog.h"

int _tmain(int argc, _TCHAR* argv[])
{
	DEBUGMSG(1, (L"argc=%i\n", argc));
	for (int i=0; i<argc; i++)
		DEBUGMSG(1, (L"[%i] '%s'\n", i, argv[i]));

	if(argc==4){
		startCETSC(NULL, argv[1],argv[2],argv[3]);
	}
	else if(argc==5){
		// rdpfile server user password
		startCETSC(argv[1],argv[2],argv[3],argv[4]);
	}
	else
		startCETSC(NULL, L"",L"",L"");
	return 0;
}

