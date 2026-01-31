using System;
using System.Collections.Generic;
using System.Text;

namespace Installer
{
    internal interface IWizardPage
    {
        string Title { get; }
        string Description { get; }
        ButtonLayout ButtonLayout { get; }
        bool CanButtonRight();
        bool CanButtonLeft();
        void OnEnterPage();
        void OnLeavePage();
    }
}
