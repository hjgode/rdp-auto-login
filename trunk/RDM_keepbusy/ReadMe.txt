========================================================================
       Windows CE APPLICATION : RDM_keepBusy
========================================================================

cmdLine:
	noRDPstart		do not start wpcstc
	dologging		enable logging to file (and netcat broadcasts)

REGEDIT4

[HKEY_LOCAL_MACHINE\Software\RDPKeepbusy]
"sleepTime"=dword:000000F0

"useMouse"=dword:00000000

"mouseY"=dword:00000020
"mouseX"=dword:00000020

"useKeyboard"=dword:00000001

"Key1"=dword:00000025
"lParam11"=dword:014D0001
"lParam12"=dword:C14D0001

"Key2"=dword:00000027
"lParam21"=dword:014B0001
"lParam22"=dword:C14B0001

/////////////////////////////////////////////////////////////////////////////
