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
    public partial class OptionsPage : UserControl, IWizardPage
    {
        private readonly MainForm mf;
        private readonly InstallerContext ctx;

        public string Title => "Assets";
        public string Description => "How the game assets should be handled.";
        public ButtonLayout ButtonLayout => ButtonLayout.BackInstall;

        public bool CanButtonRight() => false;
        public bool CanButtonLeft() => true;
        public void OnEnterPage()
        {
            SetDefaultInstallPath();
        }
        public void OnLeavePage() { }

        public OptionsPage(MainForm mf, InstallerContext ctx)
        {
            this.mf = mf;
            this.ctx = ctx;
            InitializeComponent();
        }

        private string? GetInstallPath(string folder)
        {
            if (string.IsNullOrEmpty(folder))
            {
                MessageBox.Show("Please select a valid folder.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return null;
            }

            if (!Directory.Exists(folder))
            {
                MessageBox.Show("The selected folder does not exist.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return null;
            }

            if (Directory.Exists(Path.Combine(folder, "DDLCDesktop")))
            {
                var res = MessageBox.Show("The selected folder already contains a DDLCDesktop installation. Overwrite?", "Error", MessageBoxButtons.YesNo, MessageBoxIcon.Error);
                if (res != DialogResult.Yes)
                {
                    return null;
                }
            }

            return Path.Combine(folder, "DDLCDesktop");
        }

        private void SetDefaultInstallPath()
        {
            string userProfile = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
            string? installPath = GetInstallPath(userProfile);
            if (installPath != null)
            {
                textBox1.Text = installPath!;
                ctx.installPath = installPath!;
                mf.SetButtonsEnabled(true, true);
            }
            else
            {
                mf.SetButtonsEnabled(true, false);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new();
            if (fbd.ShowDialog() == DialogResult.OK)
            {
                string? installPath = GetInstallPath(fbd.SelectedPath);
                if (installPath != null)
                {
                    mf.SetButtonsEnabled(true, true);
                    textBox1.Text = installPath!;
                    ctx.installPath = installPath!;
                }
                else
                {
                    mf.SetButtonsEnabled(true, false);
                }
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            ctx.openFontPage = checkBox1.Checked;
        }
    }
}
