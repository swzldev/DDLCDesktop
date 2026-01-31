namespace Installer
{
    partial class OptionsPage
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
            createDesktopShortcut = new CheckBox();
            createStartMenuShortcut = new CheckBox();
            label2 = new Label();
            button1 = new Button();
            textBox1 = new TextBox();
            panel1.SuspendLayout();
            SuspendLayout();
            // 
            // label1
            // 
            label1.Location = new Point(13, 16);
            label1.Name = "label1";
            label1.Size = new Size(341, 61);
            label1.TabIndex = 0;
            label1.Text = "Please choose a location where DDLCDesktop will be installed.\r\n\r\nThen click 'Install' to begin the download process.\r\n";
            // 
            // panel1
            // 
            panel1.Controls.Add(createDesktopShortcut);
            panel1.Controls.Add(createStartMenuShortcut);
            panel1.Controls.Add(label2);
            panel1.Controls.Add(button1);
            panel1.Controls.Add(textBox1);
            panel1.Controls.Add(label1);
            panel1.Dock = DockStyle.Fill;
            panel1.Location = new Point(0, 0);
            panel1.Name = "panel1";
            panel1.Size = new Size(432, 243);
            panel1.TabIndex = 1;
            // 
            // createDesktopShortcut
            // 
            createDesktopShortcut.AutoSize = true;
            createDesktopShortcut.Checked = true;
            createDesktopShortcut.CheckState = CheckState.Checked;
            createDesktopShortcut.Location = new Point(13, 105);
            createDesktopShortcut.Name = "createDesktopShortcut";
            createDesktopShortcut.Size = new Size(161, 19);
            createDesktopShortcut.TabIndex = 5;
            createDesktopShortcut.Text = "Create a desktop shortcut";
            createDesktopShortcut.UseVisualStyleBackColor = true;
            createDesktopShortcut.CheckedChanged += createDesktopShortcut_CheckedChanged;
            // 
            // createStartMenuShortcut
            // 
            createStartMenuShortcut.AutoSize = true;
            createStartMenuShortcut.Checked = true;
            createStartMenuShortcut.CheckState = CheckState.Checked;
            createStartMenuShortcut.Location = new Point(13, 80);
            createStartMenuShortcut.Name = "createStartMenuShortcut";
            createStartMenuShortcut.Size = new Size(176, 19);
            createStartMenuShortcut.TabIndex = 5;
            createStartMenuShortcut.Text = "Create a start menu shortcut";
            createStartMenuShortcut.UseVisualStyleBackColor = true;
            createStartMenuShortcut.CheckedChanged += createStartMenuShortcut_CheckedChanged;
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(13, 165);
            label2.Name = "label2";
            label2.Size = new Size(144, 15);
            label2.TabIndex = 4;
            label2.Text = "DDLCDesktop install path:";
            // 
            // button1
            // 
            button1.Location = new Point(344, 183);
            button1.Name = "button1";
            button1.Size = new Size(75, 23);
            button1.TabIndex = 3;
            button1.Text = "Open";
            button1.UseVisualStyleBackColor = true;
            button1.Click += button1_Click;
            // 
            // textBox1
            // 
            textBox1.Location = new Point(13, 183);
            textBox1.Name = "textBox1";
            textBox1.ReadOnly = true;
            textBox1.Size = new Size(325, 23);
            textBox1.TabIndex = 2;
            // 
            // OptionsPage
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            Controls.Add(panel1);
            Name = "OptionsPage";
            Size = new Size(432, 243);
            panel1.ResumeLayout(false);
            panel1.PerformLayout();
            ResumeLayout(false);
        }

        #endregion

        private Label label1;
        private Panel panel1;
        private Button button1;
        private TextBox textBox1;
        private Label label2;
        private CheckBox createDesktopShortcut;
        private CheckBox createStartMenuShortcut;
    }
}
