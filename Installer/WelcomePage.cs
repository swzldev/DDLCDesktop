using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Installer
{
    public partial class WelcomePage : UserControl, IWizardPage
    {
        public string Title => "Welcome";
        public string Description => "Welcome to the installer wizard.";
        public ButtonLayout ButtonLayout => ButtonLayout.CancelNext;

        public bool CanButtonRight() => true;
        public bool CanButtonLeft() => true;
        public void OnEnterPage() { }
        public void OnLeavePage() { }

        public WelcomePage()
        {
            InitializeComponent();
        }
    }
}
