using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Installer
{
    public partial class MainForm : Form
    {
        List<UserControl> pages = [];
        int currentPageIndex = 0;
        ButtonLayout currentLayout;
        IWizardPage? currentPage;
        readonly InstallerContext sharedCtx;

        public MainForm()
        {
            sharedCtx = new();

            InitializeComponent();

            InitializePages();
            ShowPage(0);
        }

        public void SetButtonsEnabled(bool leftEnabled, bool rightEnabled)
        {
            leftBtn.Enabled = leftEnabled;
            rightBtn.Enabled = rightEnabled;
        }

        public void ShowPage(int pageIndex)
        {
            currentPage?.OnLeavePage();

            contentPanel.Controls.Clear();
            UserControl page = pages[pageIndex];

            if (page is IWizardPage wizardPage)
            {
                titleLabel.Text = wizardPage.Title;
                descLabel.Text = wizardPage.Description;

                leftBtn.Enabled = wizardPage.CanButtonLeft();
                rightBtn.Enabled = wizardPage.CanButtonRight();

                SetButtonLayout(wizardPage.ButtonLayout);

                currentPage = wizardPage;
            }
            else throw new InvalidOperationException("Page does not implement IWizardPage interface.");

            page.Dock = DockStyle.Fill;
            contentPanel.Controls.Add(page);
            currentPageIndex = pageIndex;

            currentPage.OnEnterPage();
        }

        private void InitializePages()
        {
            pages = [
                new WelcomePage(),
                new AssetsPage(this, sharedCtx),
                new OptionsPage(this, sharedCtx)
            ];
        }

        private void SetButtonLayout(ButtonLayout layout)
        {
            currentLayout = layout;
            switch (layout)
            {
                case ButtonLayout.CancelNext:
                    leftBtn.Text = "Cancel";
                    rightBtn.Text = "Next";
                    break;
                case ButtonLayout.BackNext:
                    leftBtn.Text = "Back";
                    rightBtn.Text = "Next";
                    break;
                case ButtonLayout.BackInstall:
                    leftBtn.Text = "Back";
                    rightBtn.Text = "Install";
                    break;
                case ButtonLayout.Finish:
                    leftBtn.Visible = false;
                    rightBtn.Text = "Finish";
                    break;
            }
        }

        private void rightBtn_Click(object sender, EventArgs e)
        {
            currentPageIndex++;
            if (currentPageIndex >= pages.Count)
            {
                Application.Exit();
                return;
            }

            ShowPage(currentPageIndex);
        }

        private void leftBtn_Click(object sender, EventArgs e)
        {
            if (currentLayout == ButtonLayout.CancelNext)
            {
                Application.Exit();
                return;
            }
            currentPageIndex--;
            ShowPage(currentPageIndex);
        }
    }
}
