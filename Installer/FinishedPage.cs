using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace Installer
{
    public partial class FinishedPage : UserControl, IWizardPage
    {
        private InstallerContext ctx;

        public string Title => "Finished";
        public string Description => "DDLCDesktop has successfully been installed onto your system.";
        public ButtonLayout ButtonLayout => ButtonLayout.Finish;

        public bool CanButtonRight() => true;
        public bool CanButtonLeft() => false;
        public void OnEnterPage() { }
        public void OnLeavePage()
        {
            if (startDDLCDesktop.Checked)
            {
                StartDDLCDesktop();
            }
            if (openFontSite.Checked)
            {
                OpenDDLCFontPage();
            }
        }

        public FinishedPage(InstallerContext ctx)
        {
            this.ctx = ctx;
            InitializeComponent();
        }

        private void StartDDLCDesktop()
        {
            ProcessStartInfo psi = new()
            {
                FileName = Path.Combine(ctx.installPath, "DDLCDesktop.exe"),
                WorkingDirectory = ctx.installPath
            };

            try
            {
                Process.Start(psi);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Failed to start DDLCDesktop: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void OpenDDLCFontPage()
        {
            try
            {
                Process.Start(new ProcessStartInfo
                {
                    FileName = "https://www.1001fonts.com/aller-font.html",
                    UseShellExecute = true
                });
            }
            catch (Exception ex)
            {
                MessageBox.Show("Failed to open web page: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                // Copy the URL to clipboard as a fallback
                Clipboard.SetText("https://www.1001fonts.com/aller-font.html");
                MessageBox.Show("Link has been copied to clipboard.", "Info", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
    }
}
