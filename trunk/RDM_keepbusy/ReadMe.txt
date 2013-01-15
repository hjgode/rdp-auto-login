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

build instructions:
	for whatever reason Release builds of RDM_KeepBusy will not run on devices, so use Debug builds instead
	
history:
	15.01.2013	
	fixed a bug where first RDM session was handled correctly, but after (Disconnect) and (X) close further
	RDM sessions were not protected. I have used static vars which were set as when a valid input window was
	found. These vars kept there 'wrong' values for new started sessions.
	See note "init find vars moved from being static inside findWindow() and findWindowByClass()"
				

/////////////////////////////////////////////////////////////////////////////
