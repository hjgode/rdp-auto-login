using System;

using System.Collections.Generic;
using System.Text;
using Microsoft.Win32;
using DWORD = System.UInt32;

namespace RDP_Autologin_Settings
{
    /*
    REGEDIT4

    [HKEY_LOCAL_MACHINE\Software\RDMKeepbusy]
    "lineX"=dword:00000001
    "lineY"=dword:00000001
    "lineLen"=dword:0000000F
    "lineWidth"=dword:00000002
    "sleepTime"=dword:000000F0
    "mouseY"=dword:00000020
    "mouseX"=dword:00000020
    "useMouse"=dword:00000000
    "useKeyboard"=dword:00000001
    "Key2"=dword:00000027
    "Key1"=dword:00000025
    "lParam22"=dword:C14B0001
    "lParam21"=dword:014B0001
    "lParam12"=dword:C14D0001
    "lParam11"=dword:014D0001
    */

    class RDMKeepBusySettings
    {
        private const string sRegKey = @"HKEY_LOCAL_MACHINE\Software\RDMKeepbusy";
        private const string sRegSubKey = @"Software\RDMKeepbusy";
        private RegistryKey rk;

        #region fields
        int lineX = 1;
        public int _lineX
        {
            get { return lineX; }
            set { lineX = value; }
        }
        int lineY = 1;
        public int _lineY
        {
            get { return lineY; }
            set { lineY = value; }
        }
        int lineLen = 0x0f;
        public int _lineLen
        {
            get { return lineLen; }
            set { lineLen = value; }
        }
        int lineWidth = 2;
        public int _lineWidth
        {
            get { return lineWidth; }
            set { lineWidth = value; }
        }
        int sleepTime = 240; //0x0f0;  //240 seconds
        public int _sleepTime
        {
            get { return sleepTime; }
            set { sleepTime = value; }
        }
        int mouseX = 0x20;
        public int _mouseX
        {
            get { return mouseX; }
            set { mouseX = value; }
        }
        int mouseY = 0x20;
        public int _mouseY
        {
            get { return mouseY; }
            set { mouseY = value; }
        }
        int useMouse = 1;
        public int _useMouse
        {
            get { return useMouse; }
            set { useMouse = value; }
        }
        int useKey = 1;
        public int _useKey
        {
            get { return useKey; }
            set { useKey = value; }
        }
        public byte key1 = 0x25;
        public byte key2 = 0x27;
        public DWORD lParam11 = 0x014D0001;
        public DWORD lParam12 = 0xC14D0001;
        public DWORD lParam21 = 0x014B0001;
        public DWORD lParam22 = 0xC14B0001;
        #endregion

        bool bNewRegWritten = false;
        public bool _bNewRegWritten
        {
            get { return bNewRegWritten; }
        }
        public RDMKeepBusySettings()
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
        void readAll()
        {
            lineX = readIntFromDWORD("lineX");
            lineY = readIntFromDWORD("lineY");
            lineLen = readIntFromDWORD("lineLen");
            lineWidth = readIntFromDWORD("lineWidth");
            sleepTime = readIntFromDWORD("sleepTime");
            mouseX = readIntFromDWORD("mouseX");
            mouseY = readIntFromDWORD("mouseY");
            useMouse = readIntFromDWORD("useMouse");
            useKey = readIntFromDWORD("useKey");
            key1 = (byte)readIntFromDWORD("key1");
            key2 = (byte)readIntFromDWORD("key2");
            lParam11 = (UInt32)readIntFromDWORD("lParam11");
            lParam12 = (UInt32)readIntFromDWORD("lParam12");
            lParam21 = (UInt32)readIntFromDWORD("lParam21");
            lParam22 = (UInt32)readIntFromDWORD("lParam22");
        }

        void writeAll()
        {
            writeInt("lineX", lineX);
            writeInt("lineY", lineY);
            writeInt("lineLen", lineLen);
            writeInt("lineWidth", lineWidth);
            writeInt("sleepTime", sleepTime);
            writeInt("mouseX", mouseX);
            writeInt("mouseY", mouseY);
            writeInt("useMouse", useMouse);
            writeInt("useKey", useKey);
            writeInt("key1", key1);
            writeInt("key2", key2);
            writeInt("lParam11", lParam11);
            writeInt("lParam12", lParam12);
            writeInt("lParam21", lParam21);
            writeInt("lParam22", lParam22);
        }
        void writeDefaultReg()
        {
            writeInt("lineX", 1);
            writeInt("lineY", 1);
            writeInt("lineLen", 0x0f);
            writeInt("lineWidth", 2);
            writeInt("sleepTime", 0xf0);
            writeInt("mouseX", 0x20);
            writeInt("mouseY", 0x20);
            writeInt("useMouse", 1);
            writeInt("useKey", 1);
            writeInt("key1", 0x27);
            writeInt("key2", 0x25);
            writeInt("lParam11", 0x014D0001);
            writeInt("lParam12", 0xC14D0001);
            writeInt("lParam21", 0x014B0001);
            writeInt("lParam22", 0xC14B0001);
        }

        void writeInt(string sName, int iVal)
        {
            rk.SetValue(sName, iVal, RegistryValueKind.DWord);
        }
        void writeInt(string sName, UInt32 iVal)
        {
            //stupid VS: http://stackoverflow.com/questions/6608400/how-to-put-a-dword-in-the-registry-with-the-highest-bit-set
            rk.SetValue(sName, unchecked((int)iVal), RegistryValueKind.DWord); //0xc14d0001
        }

        int readIntFromDWORD(string sName)
        {
            int iRet = 0;
            iRet = (int)rk.GetValue(sName, 0);
            return iRet;
        }

        private bool readBoolFromString(string sName)
        {
            string sTemp = (string)rk.GetValue(sName, "");
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
            int iTemp = 0;
            object o = rk.GetValue(sName, null);
            if (o != null)
            {
                //sTemp = (string)o;
                iTemp = Convert.ToInt32(o);
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
                object o = Registry.GetValue(sRegKey, "lineX", null);
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
