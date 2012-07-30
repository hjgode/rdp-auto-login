using System;

using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using System.IO;

namespace RDP_Autologin_Settings
{
    public partial class FileDialog2 : Form
    {
        const int icoOpen = 0;
        const int icoClose = 1;
        const int icoFile = 2;

        bool copy = true;        // whether it is a copy operation
        public string fileName = null;  // filename of file to be copied or cut
        string pathName = null; // full pathname of file to be copied or cut

        public FileDialog2()
        {
            InitializeComponent();
        }

        private void FileDialog2_Load(object sender, EventArgs e)
        {
            TreeNode node = new TreeNode();
            try
            {
                // ---create the root node---
                node.ImageIndex = icoClose;
                node.SelectedImageIndex = icoOpen;
                node.Text = @"\";
                treeView1.Nodes.Add(node);
                // ---Add the dummy node---
                node.Nodes.Add("");
                treeView1.Nodes[0].Expand();
            }
            catch (Exception err)
            {
                MessageBox.Show(err.ToString());
            }
        }

        private void treeView1_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            // ---if leaf node then exit---
            if (e.Node.ImageIndex == icoFile)
                return;
            // ---remove the dummy node and display 
            // the subdirectories and files---
            try
            {
                e.Node.Nodes.Clear();
                // clears all the nodes and...
                displayChildNodes(e.Node);
                // create the nodes again
            }
            catch (Exception err)
            {
                MessageBox.Show(err.ToString());
            }
            // ---change the icon for this node to open---
            if (e.Node.GetNodeCount(false) > 0)
            {
                e.Node.ImageIndex = icoClose;
                e.Node.SelectedImageIndex = icoOpen;
            }
        }
        public string stripExtraSlash(string str)
        {
            // ---strip away the extra "\" for 
            // subdirectories. e.g. \\My documents
            string path = null;
            if (str.Length > 1 & (str.StartsWith(@"\\")))
            {
                path = str.Substring(1, str.Length - 1);
            }
            else
            {
                path = str;
            }
            return path;
        }
        private void displayChildNodes(System.Windows.Forms.TreeNode parentNode)
        {
	        DirectoryInfo FS = new DirectoryInfo(stripExtraSlash(parentNode.FullPath));
	        //DirectoryInfo dirInfo = new DirectoryInfo(parentNode.FullPath);
	        //FileInfo fileInfo = default(FileInfo);
	        try {
		        // ---displays all dirs---
		        foreach (DirectoryInfo dirInfo in FS.GetDirectories()) { //foreach ( dirInfo in FS.GetDirectories()) {
			        // ---create a new node to be added---
			        TreeNode node = new TreeNode();
			        node.Text = dirInfo.Name;
			        // name of file or dir
			        node.ImageIndex = icoClose;
			        node.SelectedImageIndex = icoOpen;
			        parentNode.Nodes.Add(node);
			        // ---add the dummy node---
			        node.Nodes.Add("");
		        }
	        } catch (Exception err) {
		        MessageBox.Show(err.ToString());
	        }
	        try {
		        // --display all files---
		        foreach (FileInfo fileInfo in FS.GetFiles()) {
			        // ---create a new node to be added---
			        TreeNode node = new TreeNode();
			        node.Text = fileInfo.Name;
			        node.ImageIndex = icoFile;
			        node.SelectedImageIndex = icoFile;
			        parentNode.Nodes.Add(node);
		        }
	        } catch (Exception err) {
		        MessageBox.Show(err.ToString());
	        }
        }

        private void mnuOK_Click(object sender, EventArgs e)
        {
            pathName = stripExtraSlash(treeView1.SelectedNode.FullPath);
            if (!pathName.EndsWith(@"\"))
                pathName += @"\";
            fileName = pathName + treeView1.SelectedNode.Text;
            DialogResult = DialogResult.OK;
        }

        private void mnuCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }

    }
}