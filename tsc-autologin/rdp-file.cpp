#include "stdafx.h"

#include "TextDocument.h"

BOOL isFile(TCHAR* fname){
	HANDLE hFile=NULL;
	hFile = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	// Result on failure. 
	if (dwFileSize == 0xFFFFFFFF) { 	 
		// Obtain the error code. 
		DWORD dwError = GetLastError() ;
		DEBUGMSG(1, (L"GetFileSize error: %i\n", dwError));
		CloseHandle(hFile);
		return FALSE;
	}
	else{
		DEBUGMSG(1, (L"GetFileSize dwSize: %i\n", dwFileSize));
		CloseHandle(hFile);
		return TRUE;
	}
}

ULONG getFileSize(TCHAR* fname){
	ULONG length=0;
	HANDLE hFile=NULL;
	hFile = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return 0;
	else{
		length = GetFileSize(hFile, 0);
		CloseHandle(hFile);
		return length;
	}
}

TCHAR* getServer(TCHAR* rdpFile){
	static TCHAR* serverHost=new TCHAR(64);
	wsprintf(serverHost, L"");
	if(isFile(rdpFile)){
		ULONG length = getFileSize(rdpFile);
		if(length != 0){
			//read file in one shot
			DWORD numread=0;
			HANDLE hFile = CreateFile(rdpFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile==INVALID_HANDLE_VALUE){
				return L"";
			}
			// allocate new file-buffer
			char *buffer = new char[length];
			if (!ReadFile(hFile, buffer, length, &numread, 0)){
				CloseHandle(hFile);
				return L"";
			}
			CloseHandle(hFile);
			if(buffer  != 0){
				//find entry "full address:s:"
				TCHAR *substr = wcsstr((TCHAR*)buffer, L"full address:s:");
				if(substr != NULL){
					TCHAR* result=&substr[wcslen(L"full address:s:")];
					//DEBUGMSG(1, (L"found: %s\n", result)); //&substr[wcslen(L"full address:s:")] gives starting point
					//remove ending \r\n
					memset(serverHost,0,64*sizeof(TCHAR));
					if(wcslen(result)>0){
						int ix=0;
						do{
							wcsncat(serverHost, &result[ix],1);
							ix++;
						}while ( (wcsncmp(&result[ix],L"\r",1)!=0) && //stop if \r
							(wcsncmp(&result[ix],L"\n",1)!=0) && 
							(ix<length) );
						wcscat(serverHost, L"\0");
						DEBUGMSG(1, (L"%s\n", serverHost));
					}
				}
			}
			else{
				//no buffer
			}
		}
		else{ //length is 0
		}
	}
	else{
		//not a file
	}
	return serverHost;
}