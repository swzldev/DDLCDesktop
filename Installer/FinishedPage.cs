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
    }
}
