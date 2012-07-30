using System;

using System.Collections.Generic;
using System.Text;
using Microsoft.Win32;

/*
REGEDIT4

[HKEY_LOCAL_MACHINE\Software\RDP_autologin]
"FitToScreen"="1"
"FullScreen"="1"
"Status"="connecting..."
"Save Password"="1"
"Domain"=""
"Password"="Intermec+2004"
"Username"="rdesktop"
"Computer"="192.168.0.130"
"DesktopWidth"=640
"DesktopHeight"=480
"startOnExit"="\\rdp_keepBusy.exe"
"execargs"="noRDPstart"
"UseMouseClick"="0"	//added with version 3 to switch between mouse and keyboard simulation
"ColorDepth"=1		//added with version 4 to enable switching between 8 and 16 Bit colors
*/

namespace RDP_Autologin_Settings
{
    class RegSettings
    {
#region Fields
        //string sFitToScreen = "1";
        bool bFitToScreen = false;
        public bool _bFitToScreen
        {
            get {
                return bFitToScreen;
            }
            set
            {
                bFitToScreen = value;
            }
        }
        //string sFullScreen = "1";
        bool bFullScreen = false;
        public bool _bFullScreen
        {
            get {
                return bFullScreen;
            }
            set
            {
                bFullScreen = value;
            }
        }

        public string _sStatus
        {
            get { return "connecting"; }
        }

        //string sSavePassword = "1";
        bool bSavePassword = true;
        public bool _bSavePassword
        {
            get
            {
                return bSavePassword;
            }
            set
            {
                bSavePassword = value;
            }
        }
        
        string sDomain = "";
        public string _sDomain
        {
            get
            {
                return sDomain;
            }
            set
            {
                sDomain = value;
            }
        }

        string sPassword = "rdesktop";
        public string _sPassword
        {
            get
            {
                return sPassword;
            }
            set
            {
                sPassword = value;
            }
        }
        string sUserName = "rdesktop";
        public string _sUserName
        {
            get
            {
                return sUserName;
            }
            set
            {
                sUserName = value;
            }
        }
        string sComputer = "192.168.128.5";
        public string _sComputer
        {
            get
            {
                return sComputer;
            }
            set
            {
                sComputer = value;
            }
        }
        
        //string sDesktopWidth = "640";
        uint iDesktopWidth = 640;
        public uint _iDesktopWidth
        {
            get { return iDesktopWidth; }
            set { iDesktopWidth = value; }
        }
        
        //string sDesktopHeight = "480";
        uint iDesktopHeight = 480;
        public uint _iDesktopHeight
        {
            get { return iDesktopHeight; }
            set { iDesktopHeight = value; }
        }
        
        string sStartOnExit = "\\rdp_keepBusy.exe";
        public string _sStartOnExit
        {
            get { return sStartOnExit; }
            set { sStartOnExit = value; }
        }
        string sExecArgs = "noRDPstart";
        public string _sExecArgs
        {
            get { return sExecArgs; }
            set { sExecArgs = value; }
        }
        string sUseMouseClick = "0";
        public bool _bUseMouseClick
        {
            get
            {
                if (sUseMouseClick == "1")
                    return true;
                else
                    return false;
            }
            set
            {
                if (value)
                    sUseMouseClick = "1";
                else
                    sUseMouseClick = "0";
            }
        }

        uint iColorDepth = 1;
        //colorDepths eColorDepth;
        enum colorDepths : uint
        {
            Bit8=1,
            Bit16=3
        }
        public uint _iColorDepth
        {
            get { return iColorDepth; }
            set { iColorDepth = value; }
        }

        // device storage mapping
        // EnableDriveRedirection=1, enables access to local files inside the host session
        public enum enuDeviceStorage
        {
            DoNotMap = 0,
            MapToRemote = 1,
        }

        enuDeviceStorage eDeviceStorage = enuDeviceStorage.DoNotMap;
        public enuDeviceStorage _eDeviceStorage
        {
            get { return eDeviceStorage; }
            set { eDeviceStorage = value; }
        }

        //audio redirection
        //0=Redirect sounds to the client, 
        //1=Play sounds at the remote computer, 
        //2=Disable sound redirection; do not play sounds at the server
        public enum enuAudioRedirectionMode
        {
            RedirectToClient = 0,
            PlaySoundRemote = 1,
            DisableSoundRedirect = 2
        }
        enuAudioRedirectionMode eAudioRedirectionMode = enuAudioRedirectionMode.RedirectToClient;
        public enuAudioRedirectionMode _eAudioRedirectionMode
        {
            get { return eAudioRedirectionMode; }
            set { eAudioRedirectionMode = value; }
        }

#endregion
        private const string sRegKey = @"HKEY_LOCAL_MACHINE\Software\RDP_autologin";
        private const string sRegSubKey = @"Software\RDP_autologin";
        private RegistryKey rk;
        bool bNewRegWritten = false;
        public bool _bNewRegWritten
        {
            get { return bNewRegWritten; }
        }

        public RegSettings()
        {
            try
            {
                rk = Registry.LocalMachine.OpenSubKey(sRegSubKey,true);
                if(rk==null)
                    rk = Registry.LocalMachine.CreateSubKey(sRegSubKey);
            }
            catch (Exception)
            {
                rk = Registry.LocalMachine.CreateSubKey(sRegSubKey);
            }
            //test if exists
            if (regExists())
                readAll();
            else
            {
                writeDefaultReg();
                bNewRegWritten = true;
            }
        }
        public void saveAll()
        {
            this.writeAll();
            bNewRegWritten = false;
        }
        private void readAll(){
            bFullScreen = readBoolFromString("FullScreen");
            bFitToScreen = readBoolFromString("FitToScreen");

            sComputer = (string)rk.GetValue("Computer", "192.168.128.5");
            sUserName = (string)rk.GetValue("UserName", "rdesktop");
            sPassword = (string)rk.GetValue("Password", "rdesktop");
            sDomain = (string)rk.GetValue("Domain", "");
            
            iDesktopHeight = (uint)readIntFromString("DesktopHeight");
            iDesktopWidth = (uint)readIntFromString("DesktopWidth");
            
            sStartOnExit = (string)rk.GetValue("StartOnExit", "");
            sExecArgs = (string)rk.GetValue("ExecArgs", "");

            bSavePassword = readBoolFromString("SavePassword");
            sUseMouseClick = (string)rk.GetValue("UseMouseClick", "0");

            object o = rk.GetValue("ColorDepth", 1);
            iColorDepth = (uint)(int)o;

            int uiVal = (int)rk.GetValue("DeviceStorage", 0);
            eDeviceStorage = (enuDeviceStorage)uiVal;// Enum.Parse(typeof(enuDeviceStorage), uiVal.ToString(), true);

            uiVal = (int)rk.GetValue("AudioRedirectionMode", 0);
            eAudioRedirectionMode = (enuAudioRedirectionMode)uiVal;// Enum.Parse(typeof(enuAudioRedirectionMode), (string)o, true);

        }
        private void writeAll()
        {
            writeStringFromBool("FullScreen", bFullScreen);
            writeStringFromBool("FitToScreen", bFitToScreen);

            rk.SetValue("Computer", sComputer, RegistryValueKind.String);
            rk.SetValue("UserName", sUserName, RegistryValueKind.String);
            rk.SetValue("Password", sPassword, RegistryValueKind.String);
            rk.SetValue("Domain", sDomain, RegistryValueKind.String);

            writeStringFromInt("DesktopHeight", (int)iDesktopHeight);
            writeStringFromInt("DesktopWidth", (int)iDesktopWidth);

            rk.SetValue("StartOnExit", sStartOnExit, RegistryValueKind.String);
            rk.SetValue("ExecArgs", sExecArgs, RegistryValueKind.String);

            writeStringFromBool("SavePassword", bSavePassword);
            rk.SetValue("ColorDepth", iColorDepth, RegistryValueKind.DWord);

            rk.SetValue("DeviceStorage", (int)eDeviceStorage, RegistryValueKind.DWord);

            rk.SetValue("AudioRedirectionMode", (int)eAudioRedirectionMode, RegistryValueKind.DWord);

        }
        private void writeDefaultReg()
        {
            writeStringFromBool("FullScreen", bFullScreen);
            writeStringFromBool("FitToScreen", bFitToScreen);

            rk.SetValue("Computer", "192.168.128.5", RegistryValueKind.String);
            rk.SetValue("UserName", "rdesktop", RegistryValueKind.String);
            rk.SetValue("Password", "rdesktop", RegistryValueKind.String);
            rk.SetValue("Domain", "", RegistryValueKind.String);

            writeStringFromInt("DesktopHeight", 640);
            writeStringFromInt("DesktopWidth", 480);

            rk.SetValue("StartOnExit", "", RegistryValueKind.String);
            rk.SetValue("ExecArgs", "", RegistryValueKind.String);

            writeStringFromBool("SavePassword", true);
            rk.SetValue("ColorDepth", 1, RegistryValueKind.DWord);
        }

        private bool readBoolFromString(string sName)
        {
            string sTemp = (string) rk.GetValue(sName, "");
            if (sTemp == "1")
                return true;
            if (sTemp == "0")
                return false;
            return false;
        }
        private void writeStringFromBool(string sName, bool bValue)
        {
            string sTemp = "";
            if (bValue)
                sTemp = "1";
            else
                sTemp = "0";
            rk.SetValue(sName, sTemp, RegistryValueKind.String);
        }

        private int readIntFromString(string sName)
        {
            string sTemp = "";
            int iTemp = 0;
            object o = rk.GetValue(sName, null);
            if (o != null)
            {
                //sTemp = (string)o;
                iTemp = Convert.ToInt16(o);
            }
            return iTemp;
        }
        private void writeStringFromInt(string sName, int iValue)
        {
            string sTemp = "";
            sTemp = iValue.ToString();
            rk.SetValue(sName, sTemp, RegistryValueKind.String);
        }
        private bool regExists()
        {
            bool bRet = false;
            try
            {
                object o = Registry.GetValue(sRegKey, "Computer", null);
                if (o != null)
                    bRet = true;
                else
                    bRet = false;
            }
            catch (Exception)
            {
                
            }
            return bRet;
        }
    }
}
