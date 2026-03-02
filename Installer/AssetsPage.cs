using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace Installer
{
    public partial class AssetsPage : UserControl, IWizardPage
    {
        private readonly MainForm mf;
        private readonly InstallerContext ctx;

        public string Title => "Assets";
        public string Description => "How the game assets should be handled.";
        public ButtonLayout ButtonLayout => ButtonLayout.BackNext;

        public bool CanButtonRight() => false;
        public bool CanButtonLeft() => true;
        public void OnEnterPage() { }
        public void OnLeavePage() { }

        public AssetsPage(MainForm mf, InstallerContext ctx)
        {
            this.mf = mf;
            this.ctx = ctx;
            InitializeComponent();
        }

        private bool IsValidDDLCDir(string folder)
        {
            if (string.IsNullOrEmpty(folder))
            {
                return false;
            }
                
            if (!Directory.Exists(folder))
            {
                return false;
            }

            // Check if the folder contains the "game" folder
            string gameFolderPath = Path.Combine(folder, "game");
            if (!Directory.Exists(gameFolderPath))
            {
                return false;
            }

            string imagesRpaPath = Path.Combine(gameFolderPath, "images.rpa");
            if (!File.Exists(imagesRpaPath))
            {
                return false;
            }
            return true;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new();
            if (fbd.ShowDialog() == DialogResult.OK)
            {
                if (IsValidDDLCDir(fbd.SelectedPath))
                {
                    mf.SetButtonsEnabled(true, true);
                    string gf = fbd.SelectedPath;
                    textBox1.Text = gf;
                    ctx.ddlcFolder = gf;
                    ctx.ddlcGameFolder = Path.Combine(gf, "game");
                }
                else
                {
                    mf.SetButtonsEnabled(true, false);
                    MessageBox.Show("The selected folder does not contain a valid DDLC installation (images.rpa not found).", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }
    }
}
