========================================================================
    CONSOLE APPLICATION : tsc-autologin Project Overview
========================================================================

tool to automate login within Remote Desktop Connection application cetsc.exe

will fill in user name and password automatically into dialogs opened 
by cetsc.exe

use with arguments for server, user and password. For example:

"\Program Files\tsc-autologin\tsc-autologin.exe" "199.64.70.66" "rdesktop" "rdesktop"

or use with arguments for rdp file, user and password. For example:

"\Program Files\tsc-autologin\tsc-autologin.exe" "\windows\default.rdp" "rdesktop" "rdesktop"

If a rdp file is provided, tsc-autologin will launch cetsc.exe with the rdp file, then
fills in user name and password and connects.

If a server address is provided, tsc-autologin will launch cetsc.exe with the /v:<server> 
option and then fills in user name and password and connects.

If the rdp-file has an "alternate shell" setting and the listed application
is authorized to be run as Remote App, then cetsc.exe will start directly
into the remote app.

See also https://technet.microsoft.com/en-us/library/cc753610(v=ws.11).aspx

The rdp file option lines:
remoteapplicationprogram:s:
remoteapplicationcmdline:s:
remoteapplicationmode:i:1
may not work (not supported on WEC7)

SUPPORTED RDP FILE ENTRIES (default.rdp):

alternate shell:s:C:\Windows\System32\notepad.exe
screen mode id:i:2
span monitors:i:0
desktopwidth:i:1920
desktopheight:i:1080
session bpp:i:16
winposstr:s:0,1,0,0,640,480
full address:s:199.64.70.66
compression:i:1
keyboardhook:i:2
audiomode:i:0
audiocapturemode:i:0
redirectprinters:i:0
redirectcomports:i:0
redirectsmartcards:i:1
redirectclipboard:i:1
redirectposdevices:i:0
redirectdrives:i:0
displayconnectionbar:i:1
autoreconnection enabled:i:1
authentication level:i:2
prompt for credentials:i:0
negotiate security layer:i:1
shell working directory:s:
disable wallpaper:i:0
disable full window drag:i:0
allow desktop composition:i:0
allow font smoothing:i:0
disable menu anims:i:0
disable themes:i:0
disable cursor setting:i:0
bitmapcachepersistenable:i:1
connection type:i:6
use multimon:i:0
videoplaybackmode:i:1
redirectdirectx:i:1
gatewayhostname:s:
gatewayusagemethod:i:4
gatewaycredentialssource:i:4
gatewayprofileusagemethod:i:0
promptcredentialonce:i:1
use redirection server name:i:0

/////////////////////////////////////////////////////////////////////////////s