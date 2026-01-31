using System.IO.Compression;
using System.Reflection;
using System.Diagnostics;
using System.Runtime.InteropServices;

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
            ExtractRPAs();
            CreateShortcuts();
            mf.NextPage();
        }

        private void ExtractInstallerFiles()
        {
            installStatusLabel.Text = "Extracting files...";
            try
            {
                var assembly = Assembly.GetExecutingAssembly();
                string resourceName = InstallerResources.payloadZipResourceName;

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
                    if (!string.IsNullOrEmpty(entry.Name))
                    {
                        // Extract the file
                        entry.ExtractToFile(destinationPath, overwrite: true);
                    }

                    currentEntry++;
                    installProgress.Value = currentEntry;
                    Application.DoEvents();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("An error occurred while extracting files: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Application.Exit();
            }
        }

        private void ExtractRPAs()
        {
            installStatusLabel.Text = "Extracting game assets...";
            installProgress.Maximum = 6;
            installProgress.Value = 0;
            try
            {
                var assembly = Assembly.GetExecutingAssembly();
                string resourceName = InstallerResources.rpaExtractResourceName;
                using Stream? stream = assembly.GetManifestResourceStream(resourceName);
                if (stream == null)
                {
                    throw new FileNotFoundException("Could not find embedded resource", resourceName);
                }
                installProgress.Value = 1;
                Application.DoEvents();

                // Copy rpaExtract.exe to installation directory
                string rpaExtractPath = Path.Combine(ctx.installPath, "rpaExtract.exe");
                using (var fileStream = new FileStream(rpaExtractPath, FileMode.Create, FileAccess.Write))
                {
                    stream.CopyTo(fileStream);
                    fileStream.Flush();
                }
                installProgress.Value = 2;
                Application.DoEvents();

                // Run on images.rpa
                string imagesRpaPath = Path.Combine(ctx.ddlcGameFolder, "images.rpa");
                ProcessStartInfo startInfo = new()
                {
                    FileName = rpaExtractPath,
                    Arguments = $"\"{imagesRpaPath}\"",
                    CreateNoWindow = true,
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    RedirectStandardInput = true,
                };

                using Process process = Process.Start(startInfo)!;
                process.StandardInput.WriteLine();
                process.WaitForExit();
                if (process.ExitCode != 0)
                {
                    string errorOutput = process.StandardError.ReadToEnd();
                    throw new Exception("RPA extraction failed: " + errorOutput);
                }
                installProgress.Value = 5;
                Application.DoEvents();

                string imagesFolder = Path.Combine(ctx.ddlcGameFolder, "images");
                string guiFolder = Path.Combine(ctx.ddlcGameFolder, "gui");
                if (!Directory.Exists(imagesFolder) || !Directory.Exists(guiFolder))
                {
                    throw new Exception("RPA extraction did not produce expected folders.");
                }

                // Move extracted folders to installation directory
                string assetsFolder = Path.Combine(ctx.installPath, "assets");
                if (Directory.Exists(assetsFolder))
                {
                    Directory.Delete(assetsFolder, true);
                }
                Directory.CreateDirectory(assetsFolder);

                string destImagesFolder = Path.Combine(assetsFolder, "images");
                string destGuiFolder = Path.Combine(assetsFolder, "gui");
                Directory.Move(imagesFolder, destImagesFolder);
                Directory.Move(guiFolder, destGuiFolder);
                installProgress.Value = 6;
                Application.DoEvents();
            }
            catch (Exception ex)
            {
                MessageBox.Show("An error occurred while extracting game assets: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Application.Exit();
            }
        }

        private void CreateShortcuts()
        {
            installStatusLabel.Text = "Creating shortcuts...";
            try
            {
                // Get paths
                string executablePath = Path.Combine(ctx.installPath, "DDLCDesktop.exe");
                string desktopPath = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
                string startMenuPath = Path.Combine(
                    Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
                    "Microsoft", "Windows", "Start Menu", "Programs", "DDLCDesktop"
                );

                // Create Start Menu folder if it doesn't exist
                if (!Directory.Exists(startMenuPath))
                {
                    Directory.CreateDirectory(startMenuPath);
                }

                // Create shortcuts using Shell.Link COM object
                dynamic shell = Activator.CreateInstance(Type.GetTypeFromProgID("WScript.Shell")!)!;

                // Desktop shortcut
                string desktopShortcutPath = Path.Combine(desktopPath, "DDLCDesktop.lnk");
                dynamic desktopShortcut = shell.CreateShortcut(desktopShortcutPath);
                desktopShortcut.TargetPath = executablePath;
                desktopShortcut.WorkingDirectory = ctx.installPath;
                desktopShortcut.Description = "DDLC Desktop";
                desktopShortcut.IconLocation = executablePath;
                desktopShortcut.Save();

                // Start Menu shortcut
                string startMenuShortcutPath = Path.Combine(startMenuPath, "DDLCDesktop.lnk");
                dynamic startMenuShortcut = shell.CreateShortcut(startMenuShortcutPath);
                startMenuShortcut.TargetPath = executablePath;
                startMenuShortcut.WorkingDirectory = ctx.installPath;
                startMenuShortcut.Description = "DDLC Desktop";
                startMenuShortcut.IconLocation = executablePath;
                startMenuShortcut.Save();

                // Clean up COM objects
                Marshal.FinalReleaseComObject(desktopShortcut);
                Marshal.FinalReleaseComObject(startMenuShortcut);
                Marshal.FinalReleaseComObject(shell);
            }
            catch (Exception ex)
            {
                MessageBox.Show("An error occurred while creating shortcuts: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Application.Exit();
            }
        }
    }
}
