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

        private string? GetImagesRpaPath(string folder)
        {
            if (string.IsNullOrEmpty(folder))
            {
                return null;
            }
                
            if (!Directory.Exists(folder))
            {
                return null;
            }

            // check if the folder contains the "game" folder
            string gameFolderPath = Path.Combine(folder, "game");
            if (Directory.Exists(gameFolderPath))
            {
                folder = gameFolderPath;
            }
            // otherwise we can assume the folder path is already the "game" folder

            string imagesRpaPath = Path.Combine(folder, "images.rpa");
            if (!File.Exists(imagesRpaPath))
            {
                return null;
            }
            return imagesRpaPath;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new();
            if (fbd.ShowDialog() == DialogResult.OK)
            {
                string? imagesRpaPath = GetImagesRpaPath(fbd.SelectedPath);
                if (imagesRpaPath != null)
                {
                    mf.SetButtonsEnabled(true, true);
                    textBox1.Text = fbd.SelectedPath;
                    ctx.imagesRpaPath = imagesRpaPath!;
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
