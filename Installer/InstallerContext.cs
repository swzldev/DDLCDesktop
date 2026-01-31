using System;
using System.Collections.Generic;
using System.Text;

namespace Installer
{
    public class InstallerContext
    {
        public string ddlcFolder = string.Empty;
        public string ddlcGameFolder = string.Empty;
        public string installPath = string.Empty;

        public bool createStartMenuShortcut = true;
        public bool createDesktopShortcut = true;
    }
}
