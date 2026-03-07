using System.Security.AccessControl;
using System.Security.Principal;
using IWshRuntimeLibrary;

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

            if (RequiresAdmin(folder))
            {
                if (!displayErrors) return false;
                var res = MessageBox.Show("The selected folder appears to require administrator permissions to write. This may cause issues with the application and is not recommended. Do you want to continue?", "Warning", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);
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
            string appDataPath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
            ValidateInstallPath(appDataPath, out string? installPath, false);

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

        private static bool RequiresAdmin(string folder)
        {
            SecurityIdentifier usersGroup = new(WellKnownSidType.BuiltinUsersSid, null);
            DirectorySecurity acl = new DirectoryInfo(folder).GetAccessControl();
            AuthorizationRuleCollection rules = acl.GetAccessRules(true, true, typeof(SecurityIdentifier));

            bool allowWrite = false;
            bool denyWrite = false;

            foreach (FileSystemAccessRule rule in rules)
            {
                if (rule.IdentityReference != usersGroup)
                    continue;

                if (rule.FileSystemRights.HasFlag(FileSystemRights.Write))
                {
                    if (rule.AccessControlType == AccessControlType.Deny)
                        denyWrite = true;
                    else
                        allowWrite = true;
                }
            }

            return denyWrite || !allowWrite;
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
