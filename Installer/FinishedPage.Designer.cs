namespace Installer
{
    partial class FinishedPage
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FinishedPage));
            label1 = new Label();
            panel1 = new Panel();
            linkLabel1 = new LinkLabel();
            startDDLCDesktop = new CheckBox();
            openFontSite = new CheckBox();
            label2 = new Label();
            panel1.SuspendLayout();
            SuspendLayout();
            // 
            // label1
            // 
            label1.Location = new Point(13, 16);
            label1.Name = "label1";
            label1.Size = new Size(341, 19);
            label1.TabIndex = 0;
            label1.Text = "If you have any problems, feel free to join the discord server.\r\n\r\n";
            // 
            // panel1
            // 
            panel1.Controls.Add(linkLabel1);
            panel1.Controls.Add(startDDLCDesktop);
            panel1.Controls.Add(openFontSite);
            panel1.Controls.Add(label2);
            panel1.Controls.Add(label1);
            panel1.Dock = DockStyle.Fill;
            panel1.Location = new Point(0, 0);
            panel1.Name = "panel1";
            panel1.Size = new Size(432, 243);
            panel1.TabIndex = 1;
            // 
            // linkLabel1
            // 
            linkLabel1.AutoSize = true;
            linkLabel1.Location = new Point(13, 35);
            linkLabel1.Name = "linkLabel1";
            linkLabel1.Size = new Size(172, 15);
            linkLabel1.TabIndex = 6;
            linkLabel1.TabStop = true;
            linkLabel1.Text = "https://discord.gg/CyerbjvyMb";
            // 
            // startDDLCDesktop
            // 
            startDDLCDesktop.AutoSize = true;
            startDDLCDesktop.Checked = true;
            startDDLCDesktop.CheckState = CheckState.Checked;
            startDDLCDesktop.Location = new Point(13, 91);
            startDDLCDesktop.Name = "startDDLCDesktop";
            startDDLCDesktop.Size = new Size(126, 19);
            startDDLCDesktop.TabIndex = 5;
            startDDLCDesktop.Text = "Start DDLCDesktop";
            startDDLCDesktop.UseVisualStyleBackColor = true;
            // 
            // openFontSite
            // 
            openFontSite.AutoSize = true;
            openFontSite.Checked = true;
            openFontSite.CheckState = CheckState.Checked;
            openFontSite.Location = new Point(13, 116);
            openFontSite.Name = "openFontSite";
            openFontSite.Size = new Size(198, 19);
            openFontSite.TabIndex = 5;
            openFontSite.Text = "Open DDLC font download page";
            openFontSite.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            label2.Font = new Font("Segoe UI", 8.25F, FontStyle.Italic, GraphicsUnit.Point, 0);
            label2.Location = new Point(13, 138);
            label2.Name = "label2";
            label2.Size = new Size(341, 66);
            label2.TabIndex = 0;
            label2.Text = resources.GetString("label2.Text");
            // 
            // FinishedPage
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            Controls.Add(panel1);
            Name = "FinishedPage";
            Size = new Size(432, 243);
            panel1.ResumeLayout(false);
            panel1.PerformLayout();
            ResumeLayout(false);
        }

        #endregion

        private Label label1;
        private Panel panel1;
        private CheckBox openFontSite;
        private LinkLabel linkLabel1;
        private Label label2;
        private CheckBox startDDLCDesktop;
    }
}
