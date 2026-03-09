using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace Uninstaller
{
    internal class MainApplication : ApplicationContext
    {
        private readonly string[] _args;

        public MainApplication(string[] args)
        {
            _args = args;

            if (_args.Length >= 2 && _args[0] == "--install-dir")
            {
                RunUninstall(_args[1]);
            }
            else
            {
                ValidateEnv();
                BeginPipeline();
            }
        }

        private static void ValidateEnv()
        {
            Process[] activeProcesses = Process.GetProcessesByName("DDLCDesktop");
            if (activeProcesses.Length > 0)
            {
                MessageBox.Show("DDLCDesktop is currently running. Please close the application before uninstalling.", "Uninstall Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Environment.Exit(0);
                return;
            }

            string installDir = AppDomain.CurrentDomain.BaseDirectory;
            if (!File.Exists(Path.Combine(installDir, "DDLCDesktop.exe")))
            {
                MessageBox.Show("DDLCDesktop does not appear to be installed in this directory. Please run this uninstaller from the DDLCDesktop installation directory.", "Uninstall Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Environment.Exit(0);
                return;
            }
        }

        private static void BeginPipeline()
        {
            var res = MessageBox.Show("This application will uninstall DDLCDesktop from your system. Do you want to proceed?", "Uninstall DDLCDesktop", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (res == DialogResult.Yes)
            {
                RelaunchFromTemp();
            }
        }

        private static void RelaunchFromTemp()
        {
            string installDir = AppDomain.CurrentDomain.BaseDirectory.TrimEnd(Path.DirectorySeparatorChar);
            string selfPath = Environment.ProcessPath!;
            string tempExePath = Path.Combine(Path.GetTempPath(), "DDLCDesktop_Uninstaller.exe");

            File.Copy(selfPath, tempExePath, overwrite: true);

            Process.Start(new ProcessStartInfo
            {
                FileName = tempExePath,
                Arguments = $"--install-dir \"{installDir}\"",
                UseShellExecute = false,
                WorkingDirectory = Path.GetTempPath()
            });

            Environment.Exit(0);
        }

        private static void RunUninstall(string installDir)
        {
            try
            {
                UninstallHelper.Uninstall(installDir);
                MessageBox.Show("Application uninstalled successfully.", "Uninstall Successful", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"An error occurred during uninstallation: {ex.Message}", "Uninstall Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            finally
            {
                Environment.Exit(0);
            }
        }
    }
}
