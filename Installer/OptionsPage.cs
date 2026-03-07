using IWshRuntimeLibrary;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace Installer
{
    public partial class OptionsPage : UserControl, IWizardPage
    {
        private readonly MainForm mf;
        private readonly InstallerContext ctx;

        public string Title => "Options";
        public string Description => "Installation options.";
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

        private static bool ValidateInstallPath(string folder, out string? finalInstallPath, bool displayErrors = true)
        {
            finalInstallPath = null;
            if (string.IsNullOrEmpty(folder))
            {
                if (displayErrors) MessageBox.Show("Please select a valid folder.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            if (!Directory.Exists(folder))
            {
                if (displayErrors) MessageBox.Show("The selected folder does not exist.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            if (folder.Contains("OneDrive", StringComparison.OrdinalIgnoreCase))
            {
                if (!displayErrors) return false;
                var res = MessageBox.Show("The selected folder has been detected to be in OneDrive. This may cause issues with the installation. Do you want to continue?", "Warning", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);
                if (res != DialogResult.Yes)
                {
                    return false;
                }
            }

            finalInstallPath = Path.Combine(folder, "DDLCDesktop");
            return true;
        }

        private void SetDefaultInstallPath()
        {
            string pfPath = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
            string? installPath = null;
            if (!ValidateInstallPath(pfPath, out installPath, false))
            {
                string myGamesPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "Documents", "My Games");
                ValidateInstallPath(pfPath, out installPath, false);
            }

            if (Directory.Exists(installPath))
            {
                var res = MessageBox.Show($"A DDLCDesktop installation has been detected at the default installation path ({installPath}). Overwrite?", "Installation Detected", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                if (res != DialogResult.Yes)
                {
                    installPath = null;
                }
            }

            if (installPath != null)
            {
                UpdateInstallPath(installPath);
            }
            else
            {
                MessageBox.Show("Could not find a valid default installation path. Please select one manually.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                UpdateCanInstall();
            }
        }

        private void UpdateInstallPath(string path)
        {
            textBox1.Text = path;
            ctx.installPath = path;
            UpdateCanInstall();
        }

        private void UpdateCanInstall()
        {
            mf.SetButtonsEnabled(true, !string.IsNullOrEmpty(ctx.installPath));
        }

        private void button1_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new();
            if (fbd.ShowDialog() == DialogResult.OK)
            {
                if (ValidateInstallPath(fbd.SelectedPath, out string? finalPath, true))
                {
                    if (Directory.Exists(finalPath))
                    {
                        var res = MessageBox.Show("A DDLCDesktop installation has been detected in the selected folder. Do you want to overwrite it?", "Installation Detected", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                        if (res != DialogResult.Yes)
                        {
                            return;
                        }
                    }

                    UpdateInstallPath(finalPath!);
                }
                else
                {
                    UpdateCanInstall();
                }
            }
        }

        private void createStartMenuShortcut_CheckedChanged(object sender, EventArgs e)
        {
            ctx.createStartMenuShortcut = createStartMenuShortcut.Checked;
        }

        private void createDesktopShortcut_CheckedChanged(object sender, EventArgs e)
        {
            ctx.createDesktopShortcut = createDesktopShortcut.Checked;
        }

        private void installFonts_CheckedChanged(object sender, EventArgs e)
        {
            ctx.installDDLCFonts = installFonts.Checked;
        }
    }
}
