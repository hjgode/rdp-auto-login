using System;

using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;



namespace RDP_Autologin_Settings
{
    public partial class Form1 : Form
    {
        RegSettings regSettings;
        RDMKeepBusySettings rdmSett;
        public Form1()
        {
            InitializeComponent();
            
            regSettings = new RegSettings();
            cboColors.Items.Clear();
            cboColors.Items.Insert(0, "8 Bit 256 colors");
            cboColors.Items.Insert(1, "16 Bit HiColor");

            txtComputer.Text = regSettings._sComputer;
            txtUserName.Text = regSettings._sUserName;
            txtPassword.Text = regSettings._sPassword;

            chkSavePassword.Checked = regSettings._bSavePassword;
            txtDomain.Text = regSettings._sDomain;

            if (regSettings._iColorDepth == 1)
                cboColors.SelectedIndex = 0;
            else
                cboColors.SelectedIndex = 1;

            chkFullScreen.Checked = regSettings._bFullScreen;
            chkFitRemoteDesktop.Checked = regSettings._bFitToScreen;

            txtStartOnExit.Text = regSettings._sStartOnExit;
            txtArguments.Text = regSettings._sExecArgs;

            cboEmulationMode.Items.Clear();
            cboEmulationMode.Items.Insert(0, "Use mouse click");
            cboEmulationMode.Items.Insert(1, "Use keyboard");

            if (regSettings._bUseMouseClick)
                cboEmulationMode.SelectedIndex = 0;
            else
                cboEmulationMode.SelectedIndex = 1;

            int iSett = (int)regSettings._eAudioRedirectionMode;
            cbo_RemoteDesktopSound.Items.Clear();
            cbo_RemoteDesktopSound.Items.Insert(0, "Mute");
            cbo_RemoteDesktopSound.Items.Insert(1, "Play on remote computer");
            cbo_RemoteDesktopSound.Items.Insert(2, "Play on this device");

            cbo_RemoteDesktopSound.SelectedIndex = iSett;

            iSett = (int)regSettings._eDeviceStorage;
            cbo_DeviceStorage.Items.Clear();
            cbo_DeviceStorage.Items.Insert(0, "Do not map to remote machine");
            cbo_DeviceStorage.Items.Insert(1, "Map to remote machine");

            cbo_DeviceStorage.SelectedIndex = iSett;

            rdmSett = new RDMKeepBusySettings();
            numericX.Value = rdmSett._lineX;
            numericY.Value = rdmSett._lineY;
            numericLen.Value = rdmSett._lineLen;
            numericWidth.Value = rdmSett._lineWidth;
            numericInterval.Value = rdmSett._sleepTime;
            if (rdmSett._useKey==1)
                chkKey.Checked = true;
            else
                chkKey.Checked = false;
            if (rdmSett._useMouse == 1)
                chkMouse.Checked = true;
            else
                chkMouse.Checked = false;

            enableChangeEvent();
        }

        private void storeSettings()
        {
            regSettings._sComputer = txtComputer.Text;
            regSettings._sUserName = txtUserName.Text;
            regSettings._sPassword = txtPassword.Text;
            regSettings._bSavePassword = chkSavePassword.Checked;
            regSettings._sDomain = txtDomain.Text;

            if (cboColors.SelectedIndex == 0)
                regSettings._iColorDepth = 1;
            else
                regSettings._iColorDepth = 3;

            regSettings._bFullScreen = chkFullScreen.Checked;
            regSettings._bFitToScreen = chkFitRemoteDesktop.Checked;

            if (cboEmulationMode.SelectedIndex == 0)
                regSettings._bUseMouseClick = true;
            else
                regSettings._bUseMouseClick = false;

            regSettings._sStartOnExit = txtStartOnExit.Text;
            regSettings._sExecArgs = txtArguments.Text;

            //redirect local drive
            regSettings._eDeviceStorage = (RegSettings.enuDeviceStorage)Enum.Parse(typeof(RegSettings.enuDeviceStorage), cbo_DeviceStorage.SelectedIndex.ToString(), true);

            //redirect audio
            regSettings._eAudioRedirectionMode = (RegSettings.enuAudioRedirectionMode)Enum.Parse(typeof(RegSettings.enuAudioRedirectionMode), cbo_RemoteDesktopSound.SelectedIndex.ToString(), true);

            rdmSett._lineX = (int)numericX.Value;
            rdmSett._lineY = (int)numericY.Value;
            rdmSett._lineLen = (int)numericLen.Value;
            rdmSett._lineWidth = (int)numericWidth.Value;
            rdmSett._sleepTime = (int)numericInterval.Value;
            if (chkMouse.Checked)
                rdmSett._useMouse = 1;
            else
                rdmSett._useMouse = 0;
            if (chkKey.Checked)
                rdmSett._useKey = 1;
            else
                rdmSett._useKey = 0;

        }

        private void RecursivelyAddChangeEvent(Control c)
        {
            for(int x = 0; x < c.Controls.Count; x++)
            {
                if(c.Controls.Count > 0)
                {
                    RecursivelyAddChangeEvent(c.Controls[x]);
                }
            }
            if ( c.GetType() == typeof(TextBox) )
            {
                ((TextBox)c).TextChanged += new EventHandler(txt_TextChanged);
            }
        }

        private void enableChangeEvent()
        {
            for (int x = 0; x < this.Controls.Count; x++)
            {
                RecursivelyAddChangeEvent(this.Controls[x]);
            }
        }

        private bool bChanges = false;
        private void txt_TextChanged(object sender, EventArgs e)
        {
            bChanges = true;
        }

        private void mnuExit_Click(object sender, EventArgs e)
        {
            if (bChanges || regSettings._bNewRegWritten)
                if (MessageBox.Show("Save changes?", "RDP AutoLogin Settings", MessageBoxButtons.YesNo, MessageBoxIcon.Question, MessageBoxDefaultButton.Button1) == DialogResult.Yes)
                {
                    storeSettings();
                    regSettings.saveAll();
                    rdmSett.saveAll();
                }
            Application.Exit();
        }

        private void mnuSave_Click(object sender, EventArgs e)
        {
            storeSettings();
            regSettings.saveAll();
            bChanges = false;
            MessageBox.Show("Settings saved");
        }

        private void btnBrowse_Click(object sender, EventArgs e)
        {
            FileDialog2 dlg = new FileDialog2();
            if (dlg.ShowDialog() == DialogResult.OK)
                txtStartOnExit.Text = dlg.fileName;
        }
    }
}