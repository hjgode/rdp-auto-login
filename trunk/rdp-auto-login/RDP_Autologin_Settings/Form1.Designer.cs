namespace RDP_Autologin_Settings
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.MainMenu mainMenu1;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.Label label;
            System.Windows.Forms.Label label14;
            System.Windows.Forms.Label label15;
            System.Windows.Forms.Label label16;
            System.Windows.Forms.Label label17;
            System.Windows.Forms.Label label18;
            System.Windows.Forms.Label label19;
            this.mainMenu1 = new System.Windows.Forms.MainMenu();
            this.mnuExit = new System.Windows.Forms.MenuItem();
            this.mnuSave = new System.Windows.Forms.MenuItem();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabConnection = new System.Windows.Forms.TabPage();
            this.chkSavePassword = new System.Windows.Forms.CheckBox();
            this.txtDomain = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.txtPassword = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.txtUserName = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.txtComputer = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tabScreen = new System.Windows.Forms.TabPage();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.cboColors = new System.Windows.Forms.ComboBox();
            this.chkFitRemoteDesktop = new System.Windows.Forms.CheckBox();
            this.chkFullScreen = new System.Windows.Forms.CheckBox();
            this.label5 = new System.Windows.Forms.Label();
            this.tab_resources = new System.Windows.Forms.TabPage();
            this.cbo_RemoteDesktopSound = new System.Windows.Forms.ComboBox();
            this.cbo_DeviceStorage = new System.Windows.Forms.ComboBox();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.pictureBox2 = new System.Windows.Forms.PictureBox();
            this.label11 = new System.Windows.Forms.Label();
            this.tabExtras = new System.Windows.Forms.TabPage();
            this.btnBrowse = new System.Windows.Forms.Button();
            this.cboEmulationMode = new System.Windows.Forms.ComboBox();
            this.txtArguments = new System.Windows.Forms.TextBox();
            this.txtStartOnExit = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.tabKeepBusy = new System.Windows.Forms.TabPage();
            this.numericX = new System.Windows.Forms.NumericUpDown();
            this.numericY = new System.Windows.Forms.NumericUpDown();
            this.numericLen = new System.Windows.Forms.NumericUpDown();
            this.numericWidth = new System.Windows.Forms.NumericUpDown();
            this.numericInterval = new System.Windows.Forms.NumericUpDown();
            this.chkMouse = new System.Windows.Forms.CheckBox();
            this.chkKey = new System.Windows.Forms.CheckBox();
            label = new System.Windows.Forms.Label();
            label14 = new System.Windows.Forms.Label();
            label15 = new System.Windows.Forms.Label();
            label16 = new System.Windows.Forms.Label();
            label17 = new System.Windows.Forms.Label();
            label18 = new System.Windows.Forms.Label();
            label19 = new System.Windows.Forms.Label();
            this.tabControl1.SuspendLayout();
            this.tabConnection.SuspendLayout();
            this.tabScreen.SuspendLayout();
            this.tab_resources.SuspendLayout();
            this.tabExtras.SuspendLayout();
            this.tabKeepBusy.SuspendLayout();
            this.SuspendLayout();
            // 
            // mainMenu1
            // 
            this.mainMenu1.MenuItems.Add(this.mnuExit);
            this.mainMenu1.MenuItems.Add(this.mnuSave);
            // 
            // mnuExit
            // 
            this.mnuExit.Text = "Exit";
            this.mnuExit.Click += new System.EventHandler(this.mnuExit_Click);
            // 
            // mnuSave
            // 
            this.mnuSave.Text = "Save";
            this.mnuSave.Click += new System.EventHandler(this.mnuSave_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabConnection);
            this.tabControl1.Controls.Add(this.tabScreen);
            this.tabControl1.Controls.Add(this.tab_resources);
            this.tabControl1.Controls.Add(this.tabExtras);
            this.tabControl1.Controls.Add(this.tabKeepBusy);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(240, 268);
            this.tabControl1.TabIndex = 13;
            // 
            // tabConnection
            // 
            this.tabConnection.BackColor = System.Drawing.Color.Tan;
            this.tabConnection.Controls.Add(this.chkSavePassword);
            this.tabConnection.Controls.Add(this.txtDomain);
            this.tabConnection.Controls.Add(this.label4);
            this.tabConnection.Controls.Add(this.txtPassword);
            this.tabConnection.Controls.Add(this.label3);
            this.tabConnection.Controls.Add(this.txtUserName);
            this.tabConnection.Controls.Add(this.label2);
            this.tabConnection.Controls.Add(this.txtComputer);
            this.tabConnection.Controls.Add(this.label1);
            this.tabConnection.Location = new System.Drawing.Point(0, 0);
            this.tabConnection.Name = "tabConnection";
            this.tabConnection.Size = new System.Drawing.Size(240, 245);
            this.tabConnection.Text = "Connection";
            // 
            // chkSavePassword
            // 
            this.chkSavePassword.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.chkSavePassword.Location = new System.Drawing.Point(84, 96);
            this.chkSavePassword.Name = "chkSavePassword";
            this.chkSavePassword.Size = new System.Drawing.Size(114, 21);
            this.chkSavePassword.TabIndex = 17;
            this.chkSavePassword.Text = "Save password";
            // 
            // txtDomain
            // 
            this.txtDomain.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.txtDomain.Location = new System.Drawing.Point(86, 130);
            this.txtDomain.Name = "txtDomain";
            this.txtDomain.Size = new System.Drawing.Size(145, 19);
            this.txtDomain.TabIndex = 12;
            // 
            // label4
            // 
            this.label4.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label4.Location = new System.Drawing.Point(9, 130);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(71, 14);
            this.label4.Text = "Domain:";
            // 
            // txtPassword
            // 
            this.txtPassword.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.txtPassword.Location = new System.Drawing.Point(86, 71);
            this.txtPassword.Name = "txtPassword";
            this.txtPassword.Size = new System.Drawing.Size(145, 19);
            this.txtPassword.TabIndex = 11;
            // 
            // label3
            // 
            this.label3.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label3.Location = new System.Drawing.Point(9, 71);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(71, 14);
            this.label3.Text = "Password:";
            // 
            // txtUserName
            // 
            this.txtUserName.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.txtUserName.Location = new System.Drawing.Point(86, 46);
            this.txtUserName.Name = "txtUserName";
            this.txtUserName.Size = new System.Drawing.Size(145, 19);
            this.txtUserName.TabIndex = 10;
            // 
            // label2
            // 
            this.label2.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label2.Location = new System.Drawing.Point(9, 46);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(71, 14);
            this.label2.Text = "User name:";
            // 
            // txtComputer
            // 
            this.txtComputer.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.txtComputer.Location = new System.Drawing.Point(86, 21);
            this.txtComputer.Name = "txtComputer";
            this.txtComputer.Size = new System.Drawing.Size(145, 19);
            this.txtComputer.TabIndex = 9;
            // 
            // label1
            // 
            this.label1.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label1.Location = new System.Drawing.Point(9, 21);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(71, 14);
            this.label1.Text = "Computer:";
            // 
            // tabScreen
            // 
            this.tabScreen.BackColor = System.Drawing.Color.Silver;
            this.tabScreen.Controls.Add(this.pictureBox1);
            this.tabScreen.Controls.Add(this.label10);
            this.tabScreen.Controls.Add(this.label9);
            this.tabScreen.Controls.Add(this.cboColors);
            this.tabScreen.Controls.Add(this.chkFitRemoteDesktop);
            this.tabScreen.Controls.Add(this.chkFullScreen);
            this.tabScreen.Controls.Add(this.label5);
            this.tabScreen.Location = new System.Drawing.Point(0, 0);
            this.tabScreen.Name = "tabScreen";
            this.tabScreen.Size = new System.Drawing.Size(232, 242);
            this.tabScreen.Text = "Screen";
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackColor = System.Drawing.Color.Black;
            this.pictureBox1.Location = new System.Drawing.Point(0, 26);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(237, 2);
            // 
            // label10
            // 
            this.label10.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold);
            this.label10.ForeColor = System.Drawing.SystemColors.Highlight;
            this.label10.Location = new System.Drawing.Point(7, 6);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(180, 22);
            this.label10.Text = "Options";
            // 
            // label9
            // 
            this.label9.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold);
            this.label9.Location = new System.Drawing.Point(8, 97);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(180, 18);
            this.label9.Text = "Remote desktop display";
            // 
            // cboColors
            // 
            this.cboColors.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.cboColors.Location = new System.Drawing.Point(85, 48);
            this.cboColors.Name = "cboColors";
            this.cboColors.Size = new System.Drawing.Size(103, 20);
            this.cboColors.TabIndex = 14;
            // 
            // chkFitRemoteDesktop
            // 
            this.chkFitRemoteDesktop.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.chkFitRemoteDesktop.Location = new System.Drawing.Point(8, 145);
            this.chkFitRemoteDesktop.Name = "chkFitRemoteDesktop";
            this.chkFitRemoteDesktop.Size = new System.Drawing.Size(222, 21);
            this.chkFitRemoteDesktop.TabIndex = 13;
            this.chkFitRemoteDesktop.Text = "Fit remote desktop to screen";
            // 
            // chkFullScreen
            // 
            this.chkFullScreen.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.chkFullScreen.Location = new System.Drawing.Point(8, 118);
            this.chkFullScreen.Name = "chkFullScreen";
            this.chkFullScreen.Size = new System.Drawing.Size(114, 21);
            this.chkFullScreen.TabIndex = 12;
            this.chkFullScreen.Text = "Full screen";
            // 
            // label5
            // 
            this.label5.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label5.Location = new System.Drawing.Point(8, 48);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(71, 14);
            this.label5.Text = "Colors:";
            // 
            // tab_resources
            // 
            this.tab_resources.Controls.Add(this.cbo_RemoteDesktopSound);
            this.tab_resources.Controls.Add(this.cbo_DeviceStorage);
            this.tab_resources.Controls.Add(this.label13);
            this.tab_resources.Controls.Add(this.label12);
            this.tab_resources.Controls.Add(this.pictureBox2);
            this.tab_resources.Controls.Add(this.label11);
            this.tab_resources.Location = new System.Drawing.Point(0, 0);
            this.tab_resources.Name = "tab_resources";
            this.tab_resources.Size = new System.Drawing.Size(232, 242);
            this.tab_resources.Text = "Resources";
            // 
            // cbo_RemoteDesktopSound
            // 
            this.cbo_RemoteDesktopSound.Location = new System.Drawing.Point(7, 122);
            this.cbo_RemoteDesktopSound.Name = "cbo_RemoteDesktopSound";
            this.cbo_RemoteDesktopSound.Size = new System.Drawing.Size(217, 22);
            this.cbo_RemoteDesktopSound.TabIndex = 7;
            // 
            // cbo_DeviceStorage
            // 
            this.cbo_DeviceStorage.Location = new System.Drawing.Point(7, 64);
            this.cbo_DeviceStorage.Name = "cbo_DeviceStorage";
            this.cbo_DeviceStorage.Size = new System.Drawing.Size(217, 22);
            this.cbo_DeviceStorage.TabIndex = 7;
            // 
            // label13
            // 
            this.label13.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold);
            this.label13.Location = new System.Drawing.Point(7, 101);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(180, 18);
            this.label13.Text = "Remote desktop sound";
            // 
            // label12
            // 
            this.label12.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold);
            this.label12.Location = new System.Drawing.Point(7, 43);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(180, 18);
            this.label12.Text = "Device storage";
            // 
            // pictureBox2
            // 
            this.pictureBox2.BackColor = System.Drawing.Color.Black;
            this.pictureBox2.Location = new System.Drawing.Point(0, 24);
            this.pictureBox2.Name = "pictureBox2";
            this.pictureBox2.Size = new System.Drawing.Size(237, 2);
            // 
            // label11
            // 
            this.label11.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold);
            this.label11.ForeColor = System.Drawing.SystemColors.Highlight;
            this.label11.Location = new System.Drawing.Point(7, 4);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(180, 22);
            this.label11.Text = "Options";
            // 
            // tabExtras
            // 
            this.tabExtras.BackColor = System.Drawing.SystemColors.Info;
            this.tabExtras.Controls.Add(this.btnBrowse);
            this.tabExtras.Controls.Add(this.cboEmulationMode);
            this.tabExtras.Controls.Add(this.txtArguments);
            this.tabExtras.Controls.Add(this.txtStartOnExit);
            this.tabExtras.Controls.Add(this.label7);
            this.tabExtras.Controls.Add(this.label8);
            this.tabExtras.Controls.Add(this.label6);
            this.tabExtras.Location = new System.Drawing.Point(0, 0);
            this.tabExtras.Name = "tabExtras";
            this.tabExtras.Size = new System.Drawing.Size(232, 242);
            this.tabExtras.Text = "Extras";
            // 
            // btnBrowse
            // 
            this.btnBrowse.Location = new System.Drawing.Point(198, 24);
            this.btnBrowse.Name = "btnBrowse";
            this.btnBrowse.Size = new System.Drawing.Size(35, 18);
            this.btnBrowse.TabIndex = 15;
            this.btnBrowse.Text = "...";
            this.btnBrowse.Click += new System.EventHandler(this.btnBrowse_Click);
            // 
            // cboEmulationMode
            // 
            this.cboEmulationMode.Location = new System.Drawing.Point(85, 131);
            this.cboEmulationMode.Name = "cboEmulationMode";
            this.cboEmulationMode.Size = new System.Drawing.Size(118, 22);
            this.cboEmulationMode.TabIndex = 11;
            // 
            // txtArguments
            // 
            this.txtArguments.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.txtArguments.Location = new System.Drawing.Point(85, 73);
            this.txtArguments.Name = "txtArguments";
            this.txtArguments.Size = new System.Drawing.Size(148, 19);
            this.txtArguments.TabIndex = 8;
            // 
            // txtStartOnExit
            // 
            this.txtStartOnExit.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.txtStartOnExit.Location = new System.Drawing.Point(8, 48);
            this.txtStartOnExit.Name = "txtStartOnExit";
            this.txtStartOnExit.Size = new System.Drawing.Size(225, 19);
            this.txtStartOnExit.TabIndex = 7;
            // 
            // label7
            // 
            this.label7.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label7.Location = new System.Drawing.Point(6, 131);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(71, 14);
            this.label7.Text = "Emul. Mode:";
            // 
            // label8
            // 
            this.label8.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label8.Location = new System.Drawing.Point(8, 73);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(71, 14);
            this.label8.Text = "Arguments:";
            // 
            // label6
            // 
            this.label6.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label6.Location = new System.Drawing.Point(8, 24);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(71, 14);
            this.label6.Text = "Start On Exit:";
            // 
            // tabKeepBusy
            // 
            this.tabKeepBusy.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
            this.tabKeepBusy.Controls.Add(this.chkKey);
            this.tabKeepBusy.Controls.Add(this.chkMouse);
            this.tabKeepBusy.Controls.Add(this.numericInterval);
            this.tabKeepBusy.Controls.Add(this.numericWidth);
            this.tabKeepBusy.Controls.Add(this.numericLen);
            this.tabKeepBusy.Controls.Add(this.numericY);
            this.tabKeepBusy.Controls.Add(label17);
            this.tabKeepBusy.Controls.Add(this.numericX);
            this.tabKeepBusy.Controls.Add(label16);
            this.tabKeepBusy.Controls.Add(label15);
            this.tabKeepBusy.Controls.Add(label14);
            this.tabKeepBusy.Controls.Add(label19);
            this.tabKeepBusy.Controls.Add(label18);
            this.tabKeepBusy.Controls.Add(label);
            this.tabKeepBusy.Location = new System.Drawing.Point(0, 0);
            this.tabKeepBusy.Name = "tabKeepBusy";
            this.tabKeepBusy.Size = new System.Drawing.Size(240, 245);
            this.tabKeepBusy.Text = "RDM_KeepBusy";
            // 
            // label
            // 
            label.Location = new System.Drawing.Point(10, 11);
            label.Name = "label";
            label.Size = new System.Drawing.Size(87, 18);
            label.Text = "Indicator:";
            // 
            // label14
            // 
            label14.Location = new System.Drawing.Point(6, 34);
            label14.Name = "label14";
            label14.Size = new System.Drawing.Size(28, 17);
            label14.Text = "X:";
            label14.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label15
            // 
            label15.Location = new System.Drawing.Point(7, 63);
            label15.Name = "label15";
            label15.Size = new System.Drawing.Size(27, 17);
            label15.Text = "Y:";
            label15.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // numericX
            // 
            this.numericX.Location = new System.Drawing.Point(40, 35);
            this.numericX.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericX.Name = "numericX";
            this.numericX.Size = new System.Drawing.Size(58, 22);
            this.numericX.TabIndex = 3;
            this.numericX.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // numericY
            // 
            this.numericY.Location = new System.Drawing.Point(40, 63);
            this.numericY.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericY.Name = "numericY";
            this.numericY.Size = new System.Drawing.Size(58, 22);
            this.numericY.TabIndex = 3;
            this.numericY.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label16
            // 
            label16.Location = new System.Drawing.Point(114, 34);
            label16.Name = "label16";
            label16.Size = new System.Drawing.Size(46, 17);
            label16.Text = "Len:";
            label16.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label17
            // 
            label17.Location = new System.Drawing.Point(114, 63);
            label17.Name = "label17";
            label17.Size = new System.Drawing.Size(46, 17);
            label17.Text = "Width:";
            label17.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // numericLen
            // 
            this.numericLen.Location = new System.Drawing.Point(166, 35);
            this.numericLen.Minimum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numericLen.Name = "numericLen";
            this.numericLen.Size = new System.Drawing.Size(58, 22);
            this.numericLen.TabIndex = 3;
            this.numericLen.Value = new decimal(new int[] {
            30,
            0,
            0,
            0});
            // 
            // numericWidth
            // 
            this.numericWidth.Location = new System.Drawing.Point(166, 63);
            this.numericWidth.Maximum = new decimal(new int[] {
            15,
            0,
            0,
            0});
            this.numericWidth.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericWidth.Name = "numericWidth";
            this.numericWidth.Size = new System.Drawing.Size(58, 22);
            this.numericWidth.TabIndex = 3;
            this.numericWidth.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label18
            // 
            label18.Location = new System.Drawing.Point(10, 99);
            label18.Name = "label18";
            label18.Size = new System.Drawing.Size(87, 18);
            label18.Text = "Interval:";
            // 
            // numericInterval
            // 
            this.numericInterval.Location = new System.Drawing.Point(103, 99);
            this.numericInterval.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numericInterval.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericInterval.Name = "numericInterval";
            this.numericInterval.Size = new System.Drawing.Size(58, 22);
            this.numericInterval.TabIndex = 3;
            this.numericInterval.Value = new decimal(new int[] {
            240,
            0,
            0,
            0});
            // 
            // label19
            // 
            label19.Location = new System.Drawing.Point(166, 101);
            label19.Name = "label19";
            label19.Size = new System.Drawing.Size(58, 18);
            label19.Text = "sec.";
            // 
            // chkMouse
            // 
            this.chkMouse.Location = new System.Drawing.Point(103, 127);
            this.chkMouse.Name = "chkMouse";
            this.chkMouse.Size = new System.Drawing.Size(121, 19);
            this.chkMouse.TabIndex = 8;
            this.chkMouse.Text = "emulate mouse";
            // 
            // chkKey
            // 
            this.chkKey.Location = new System.Drawing.Point(103, 152);
            this.chkKey.Name = "chkKey";
            this.chkKey.Size = new System.Drawing.Size(121, 19);
            this.chkKey.TabIndex = 8;
            this.chkKey.Text = "emulate key";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 268);
            this.ControlBox = false;
            this.Controls.Add(this.tabControl1);
            this.Menu = this.mainMenu1;
            this.MinimizeBox = false;
            this.Name = "Form1";
            this.Text = "RDP AutoLogin Settings";
            this.tabControl1.ResumeLayout(false);
            this.tabConnection.ResumeLayout(false);
            this.tabScreen.ResumeLayout(false);
            this.tab_resources.ResumeLayout(false);
            this.tabExtras.ResumeLayout(false);
            this.tabKeepBusy.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.MenuItem mnuExit;
        private System.Windows.Forms.MenuItem mnuSave;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabConnection;
        private System.Windows.Forms.TabPage tabScreen;
        private System.Windows.Forms.TabPage tabExtras;
        private System.Windows.Forms.CheckBox chkSavePassword;
        private System.Windows.Forms.TextBox txtDomain;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtPassword;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtUserName;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtComputer;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox cboColors;
        private System.Windows.Forms.CheckBox chkFitRemoteDesktop;
        private System.Windows.Forms.CheckBox chkFullScreen;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox txtArguments;
        private System.Windows.Forms.TextBox txtStartOnExit;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox cboEmulationMode;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button btnBrowse;
        private System.Windows.Forms.TabPage tab_resources;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.PictureBox pictureBox2;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ComboBox cbo_RemoteDesktopSound;
        private System.Windows.Forms.ComboBox cbo_DeviceStorage;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.TabPage tabKeepBusy;
        private System.Windows.Forms.NumericUpDown numericWidth;
        private System.Windows.Forms.NumericUpDown numericLen;
        private System.Windows.Forms.NumericUpDown numericY;
        private System.Windows.Forms.NumericUpDown numericX;
        private System.Windows.Forms.NumericUpDown numericInterval;
        private System.Windows.Forms.CheckBox chkKey;
        private System.Windows.Forms.CheckBox chkMouse;
    }
}

