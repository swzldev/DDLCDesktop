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
        public void OnEnterPage()
        {
            TryFindDDLC();
        }
        public void OnLeavePage() { }

        public AssetsPage(MainForm mf, InstallerContext ctx)
        {
            this.mf = mf;
            this.ctx = ctx;
            InitializeComponent();
        }

        private static bool IsValidDDLCDir(string folder)
        {
            if (string.IsNullOrEmpty(folder))
            {
                return false;
            }
                
            if (!Directory.Exists(folder))
            {
                return false;
            }

            // Check if the folder contains the "game" folder
            string gameFolderPath = Path.Combine(folder, "game");
            if (!Directory.Exists(gameFolderPath))
            {
                return false;
            }

            string imagesRpaPath = Path.Combine(gameFolderPath, "images.rpa");
            if (!File.Exists(imagesRpaPath))
            {
                return false;
            }
            return true;
        }

        private void TryFindDDLC()
        {
            string? steamPath = GetSteamInstallPath();
            if (steamPath == null) return;

            string vdfPath = Path.Combine(steamPath, "steamapps", "libraryfolders.vdf");
            if (!File.Exists(vdfPath)) return;

            var libraryPaths = ParseSteamLibraryFolders(vdfPath);
            libraryPaths.Insert(0, steamPath);

            foreach (string library in libraryPaths)
            {
                string ddlcPath = Path.Combine(library, "steamapps", "common", "Doki Doki Literature Club");
                if (IsValidDDLCDir(ddlcPath))
                {
                    SetDDLCDir(ddlcPath);
                    return;
                }
            }
        }

        private static string? GetSteamInstallPath()
        {
            string[] keys =
            [
                @"SOFTWARE\WOW6432Node\Valve\Steam",
                @"SOFTWARE\Valve\Steam"
            ];

            foreach (string key in keys)
            {
                using var reg = Microsoft.Win32.Registry.LocalMachine.OpenSubKey(key);
                if (reg?.GetValue("InstallPath") is string path && Directory.Exists(path))
                    return path;
            }
            return null;
        }

        private static List<string> ParseSteamLibraryFolders(string vdfPath)
        {
            var paths = new List<string>();
            foreach (string line in File.ReadLines(vdfPath))
            {
                string trimmed = line.Trim();
                // VDF format: "path"   "C:\\SteamLibrary"
                if (!trimmed.StartsWith("\"path\"", StringComparison.OrdinalIgnoreCase))
                    continue;

                int start = trimmed.IndexOf('"', 6);
                int end = trimmed.LastIndexOf('"');
                if (start == -1 || end <= start) continue;

                string path = trimmed[(start + 1)..end].Replace("\\\\", "\\");
                if (Directory.Exists(path))
                    paths.Add(path);
            }
            return paths;
        }

        private void UpdateCanContinue()
        {
            mf.SetButtonsEnabled(true, IsValidDDLCDir(textBox1.Text));
        }

        private void SetDDLCDir(string path)
        {
            textBox1.Text = path;
            ctx.ddlcFolder = path;
            UpdateCanContinue();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new();
            if (fbd.ShowDialog() == DialogResult.OK)
            {
                if (IsValidDDLCDir(fbd.SelectedPath))
                {
                    string gf = fbd.SelectedPath;
                    SetDDLCDir(gf);
                }
                else
                {
                    MessageBox.Show("The selected folder does not contain a valid DDLC installation (game/images.rpa not found).", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    UpdateCanContinue();
                }
            }
        }
    }
}
