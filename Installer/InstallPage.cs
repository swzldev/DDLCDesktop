using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.IO.Compression;
using System.Reflection;

namespace Installer
{
    public partial class InstallPage : UserControl, IWizardPage
    {
        private readonly MainForm mf;
        private readonly InstallerContext ctx;

        public string Title => "Installing";
        public string Description => "DDLCDesktop is installing...";
        public ButtonLayout ButtonLayout => ButtonLayout.BackNext;

        public bool CanButtonRight() => false;
        public bool CanButtonLeft() => false;
        public void OnEnterPage()
        {
            BeginInstall();
        }
        public void OnLeavePage() { }

        public InstallPage(MainForm mf, InstallerContext ctx)
        {
            this.mf = mf;
            this.ctx = ctx;
            InitializeComponent();
        }

        private void BeginInstall()
        {
            ExtractInstallerFiles();
        }

        private void ExtractInstallerFiles()
        {
            installStatusLabel.Text = "Extracting files...";
            try
            {
                var assembly = Assembly.GetExecutingAssembly();
                string resourceName = "Installer.Resources.DDLCDesktop";

                using Stream? stream = assembly.GetManifestResourceStream(resourceName);
                if (stream == null)
                {
                    throw new FileNotFoundException("Could not find embedded resource", resourceName);
                }

                using ZipArchive archive = new(stream);

                installProgress.Maximum = archive.Entries.Count;
                installProgress.Value = 0;

                int currentEntry = 0;
                foreach (ZipArchiveEntry entry in archive.Entries)
                {
                    installStatusLabel.Text = "Extracting: " + entry.FullName;
                    string destinationPath = Path.GetFullPath(Path.Combine(ctx.installPath, entry.FullName));
                    // Ensure the destination directory exists
                    string destinationDir = Path.GetDirectoryName(destinationPath)!;
                    if (!Directory.Exists(destinationDir))
                    {
                        Directory.CreateDirectory(destinationDir);
                    }
                    // Extract the file
                    entry.ExtractToFile(destinationPath, overwrite: true);

                    currentEntry++;
                    installProgress.Value = currentEntry;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("An error occurred while extracting files: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}
