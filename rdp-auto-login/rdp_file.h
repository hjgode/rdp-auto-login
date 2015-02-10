//rdp_file.h

typedef struct{
	TCHAR* line;
} rdpLine;

rdpLine rdpLines[] = {
	{L"\xFF\xFE"}, //this is NOT used, it is the Unicode identifier
	//new with 3. dec 2013
	{L"WorkingDir:s:C:\\Program Files (x86)"},	// NOT IMPLEMENTED IN RDM!
	{L"AlternateShell:s:C:\\Program Files (x86)\\notepad.exe"},	// NOT IMPLEMENTED IN RDM!
	{L"EnableClipboardRedirection:i:%s"},	//0 = disabled, 1 = enable clipboard sharing, see also g_bEnableClipboardRedirection
	{L"RDPIdleTimeout:i:0"},				// NOT IMPLEMENTED IN RDM!
	//older...
	{L"Domain:s:%s\r\n"},
	{L"GrabFocusOnConnect:i:0\r\n"},
	{L"MinutesToIdleTimeout:i:0\r\n"}, //was 5, has no function, tested with 0 but session did timeout anyway
	{L"DisableFileAccess:i:0\r\n"},
	{L"BBarEnabled:i:0\r\n"},
	{L"EnablePrinterRedirection:i:0\r\n"},
	{L"EnableSCardRedirection:i:1\r\n"},
	{L"AutoReconnectEnabled:i:1\r\n"},
	{L"EnableDriveRedirection:i:%i\r\n"},	//EnableDriveRedirection=1, enables access to local files inside the host session
	{L"EnablePortRedirection:i:0\r\n"},
	{L"AudioRedirectionMode:i:%i\r\n"},		//0=Redirect sounds to the client, 1=Play sounds at the remote computer, 2=Disable sound redirection; do not play sounds at the server
	{L"BitmapPersistenceEnabled:i:0\r\n"},
	{L"BBarShowPinBtn:i:0\r\n"},
	{L"Compress:i:1\r\n"},
	{L"KeyboardHookMode:i:0\r\n"},
	{L"MaxReconnectAttempts:i:20\r\n"},
	{L"Disable Wallpaper:i:1\r\n"},
	{L"Disable Full Window Drag:i:1\r\n"},
	{L"Disable Menu Anims:i:1\r\n"},
	{L"Disable Themes:i:0\r\n"},
	{L"KeyboardLayoutString:s:0xE0010409\r\n"},
	{L"KeyboardType:i:4\r\n"},
	{L"KeyboardSubType:i:0\r\n"},
	{L"KeyboardFunctionKey:i:12\r\n"},
	{L"BitmapCacheSize:i:21\r\n"},
	{L"BitmapPersistCacheSize:i:1\r\n"},
	{L"Keyboard Layout:s:00000409\r\n"},
	{L"MCSPort:i:3389\r\n"},

	{L"Password:b:0200000000000000000000000000000000000000000000000800000072006400700000000E660000100000001000000042CC2095244E1C87923AC5BC2014D0A6000000000480000010000000100000004207CC82911829FA8E78AE77404B256820000000D1759CC8A4025896DC5C7599484D7D0CCEF9C4BBBF5A44DC5D766B25A02E32001400000011F4A0E6AD9236475C1AD25AE113EE331D893929\r\n"},
	{L"ServerName:s:%s\r\n"},
	{L"UserName:s:%s\r\n"},
	{L"SavePassword:i:%s\r\n"},
	{L"DesktopHeight:i:%s\r\n"},
	{L"DesktopWidth:i:%s\r\n"},
	{L"ScreenStyle:i:%s\r\n"},  //0=no fullscreen + no fit, 1= fit to screen+no fullscreen, 2=fullscreen+no fit, 3=fit+fullscreen
	{L"ColorDepthID:i:%i\r\n"}, //changed from %s to %i with version 4
	NULL
};

/*
	Fit remote Screen to screen
		DesktopHeight:i:320
		DesktopWidth:i:240

	Fullscreen:
		ScreenStyle:i:3		Fullscreen
		ScreenStyle:i:0		no FullScreen

	ColorCount 
		ColorDepthID:i:3	HighColor (16bit)
		ColorDepthID:i:1	256 Colors

*/