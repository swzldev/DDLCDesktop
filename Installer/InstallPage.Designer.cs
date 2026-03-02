namespace Installer
{
    partial class InstallPage
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            label1 = new Label();
            panel1 = new Panel();
            installStatusLabel = new Label();
            installProgress = new ProgressBar();
            panel1.SuspendLayout();
            SuspendLayout();
            // 
            // label1
            // 
            label1.Location = new Point(13, 16);
            label1.Name = "label1";
            label1.Size = new Size(341, 24);
            label1.TabIndex = 0;
            label1.Text = "Please wait whilst DDLCDesktop is installed.\r\n";
            // 
            // panel1
            // 
            panel1.Controls.Add(installStatusLabel);
            panel1.Controls.Add(installProgress);
            panel1.Controls.Add(label1);
            panel1.Dock = DockStyle.Fill;
            panel1.Location = new Point(0, 0);
            panel1.Name = "panel1";
            panel1.Size = new Size(432, 243);
            panel1.TabIndex = 1;
            // 
            // installStatusLabel
            // 
            installStatusLabel.AutoSize = true;
            installStatusLabel.Location = new Point(13, 124);
            installStatusLabel.Name = "installStatusLabel";
            installStatusLabel.Size = new Size(64, 15);
            installStatusLabel.TabIndex = 2;
            installStatusLabel.Text = "Installing...";
            // 
            // installProgress
            // 
            installProgress.Location = new Point(13, 98);
            installProgress.Name = "installProgress";
            installProgress.Size = new Size(404, 23);
            installProgress.TabIndex = 1;
            // 
            // InstallPage
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            Controls.Add(panel1);
            Name = "InstallPage";
            Size = new Size(432, 243);
            panel1.ResumeLayout(false);
            panel1.PerformLayout();
            ResumeLayout(false);
        }

        #endregion

        private Label label1;
        private Panel panel1;
        private ProgressBar installProgress;
        private Label installStatusLabel;
    }
}
