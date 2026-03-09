using Microsoft.Win32;
using System.Diagnostics;

namespace Uninstaller
{
    internal static class UninstallHelper
    {
        private const string RUN_KEY = @"Software\Microsoft\Windows\CurrentVersion\Run";
        private const string APP_NAME = "DDLCDesktop";

        public static void Uninstall(string installPath)
        {
            CleanRegistry();
            CleanShortcuts();
            DeleteFiles(installPath);
        }

        private static void CleanRegistry()
        {
            using RegistryKey? key = Registry.CurrentUser.OpenSubKey(RUN_KEY, writable: true);
            key?.DeleteValue(APP_NAME, throwOnMissingValue: false);
        }

        private static void CleanShortcuts()
        {
            // Delete DDLC Desktop.lnk from desktop and start menu
            string desktopPath = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
            string desktopEntry = Path.Combine(desktopPath, "DDLC Desktop.lnk");

            string startMenuPath = Environment.GetFolderPath(Environment.SpecialFolder.StartMenu);
            string startMenuEntry = Path.Combine(startMenuPath, "DDLC Desktop");

            if (Directory.Exists(startMenuEntry))
                Directory.Delete(startMenuEntry, true);

            if (File.Exists(desktopEntry))
                File.Delete(desktopEntry);
        }

        private static void DeleteFiles(string installPath)
        {
            if (Directory.Exists(installPath))
                Directory.Delete(installPath, recursive: true);
        }
    }
}