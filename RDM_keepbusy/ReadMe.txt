========================================================================
       Windows CE APPLICATION : RDM_keepBusy
========================================================================

cmdLine:
	noRDPstart		do not start wpcstc
	dologging		enable logging to file (and netcat broadcasts)

REGEDIT4

[HKEY_LOCAL_MACHINE\Software\RDPKeepbusy]
;the following lines define the posisition and length of an indicator line, do not touch if you do not know what that means
;the indicator line will blink green or red as long as RDM_KeepBusy is running
;red=no RDM running
;green=RDM running found by tool
"lineX"=dword:00000001
"lineY"=dword:00000001
"lineLen"=dword:0000000F
"lineWidth"=dword:00000002

;interval between simulated mouse click or key press
"sleepTime"=dword:000000F0

;use mouse click to reset idle state
"useMouse"=dword:00000000
;mouse click positions, do not touch
"mouseY"=dword:00000020
"mouseX"=dword:00000020

;use keyboard msessage to supress idle timeout
"useKeyboard"=dword:00000001
;which key, default is VK_LEFT, see winuser.h, lparam11 and lparam12 define the scan codes to be used with the key
"Key1"=dword:00000025
"lParam11"=dword:014D0001
"lParam12"=dword:C14D0001

;second key press definition, default is VK_RIGHT, lparam21 and 22 define the scan codes of key2 messages
"Key2"=dword:00000027
"lParam21"=dword:014B0001
"lParam22"=dword:C14B0001

;use logging?, default is enabled
"doLogging"=00000001

build instructions:
	obsolete(wm653pro): for whatever reason Release builds of RDM_KeepBusy will not run on devices, so use Debug builds instead
	
history:
	15.01.2013	
	fixed a bug where first RDM session was handled correctly, but after (Disconnect) and (X) close further
	RDM sessions were not protected. I have used static vars which were set as when a valid input window was
	found. These vars kept there 'wrong' values for new started sessions.
	See note "init find vars moved from being static inside findWindow() and findWindowByClass()"
	
	12.march 2015
	Added registry key to enable/disable logging. If key is not set, bUseLogging is enabled
	added reg key description
	changed readReg to use default values if a reg key does not exist
	changed readReg to read key and mouse values as block					

/////////////////////////////////////////////////////////////////////////////
