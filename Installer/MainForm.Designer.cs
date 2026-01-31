namespace Installer
{
    partial class MainForm
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            contentPanel = new Panel();
            rightBtn = new Button();
            leftBtn = new Button();
            pictureBox1 = new PictureBox();
            panel1 = new Panel();
            panel2 = new Panel();
            descLabel = new Label();
            titleLabel = new Label();
            ((System.ComponentModel.ISupportInitialize)pictureBox1).BeginInit();
            panel1.SuspendLayout();
            panel2.SuspendLayout();
            SuspendLayout();
            // 
            // contentPanel
            // 
            contentPanel.Location = new Point(152, 71);
            contentPanel.Name = "contentPanel";
            contentPanel.Size = new Size(432, 243);
            contentPanel.TabIndex = 0;
            // 
            // rightBtn
            // 
            rightBtn.Location = new Point(341, 14);
            rightBtn.Name = "rightBtn";
            rightBtn.Size = new Size(75, 23);
            rightBtn.TabIndex = 1;
            rightBtn.Text = "Next";
            rightBtn.UseVisualStyleBackColor = true;
            rightBtn.Click += rightBtn_Click;
            // 
            // leftBtn
            // 
            leftBtn.Location = new Point(260, 14);
            leftBtn.Name = "leftBtn";
            leftBtn.Size = new Size(75, 23);
            leftBtn.TabIndex = 1;
            leftBtn.Text = "Back";
            leftBtn.UseVisualStyleBackColor = true;
            leftBtn.Click += leftBtn_Click;
            // 
            // pictureBox1
            // 
            pictureBox1.BackgroundImage = Properties.Resources.ddlc_desktop_setupbanner;
            pictureBox1.BackgroundImageLayout = ImageLayout.Stretch;
            pictureBox1.Dock = DockStyle.Left;
            pictureBox1.Location = new Point(0, 0);
            pictureBox1.Name = "pictureBox1";
            pictureBox1.Size = new Size(154, 361);
            pictureBox1.TabIndex = 2;
            pictureBox1.TabStop = false;
            // 
            // panel1
            // 
            panel1.BackColor = SystemColors.ControlLight;
            panel1.Controls.Add(leftBtn);
            panel1.Controls.Add(rightBtn);
            panel1.Dock = DockStyle.Bottom;
            panel1.Location = new Point(154, 312);
            panel1.Name = "panel1";
            panel1.Size = new Size(430, 49);
            panel1.TabIndex = 3;
            // 
            // panel2
            // 
            panel2.BackColor = SystemColors.ControlLight;
            panel2.Controls.Add(descLabel);
            panel2.Controls.Add(titleLabel);
            panel2.Dock = DockStyle.Top;
            panel2.Location = new Point(154, 0);
            panel2.Name = "panel2";
            panel2.Size = new Size(430, 72);
            panel2.TabIndex = 4;
            // 
            // descLabel
            // 
            descLabel.Location = new Point(6, 44);
            descLabel.Margin = new Padding(100, 100, 3, 0);
            descLabel.Name = "descLabel";
            descLabel.Size = new Size(410, 28);
            descLabel.TabIndex = 1;
            descLabel.Text = "label1";
            // 
            // titleLabel
            // 
            titleLabel.Font = new Font("Segoe UI", 15.75F, FontStyle.Regular, GraphicsUnit.Point, 0);
            titleLabel.Location = new Point(6, 9);
            titleLabel.Name = "titleLabel";
            titleLabel.Size = new Size(410, 31);
            titleLabel.TabIndex = 0;
            titleLabel.Text = "Title";
            // 
            // MainForm
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(584, 361);
            Controls.Add(panel2);
            Controls.Add(panel1);
            Controls.Add(pictureBox1);
            Controls.Add(contentPanel);
            FormBorderStyle = FormBorderStyle.FixedDialog;
            Name = "MainForm";
            Text = "DDLCDesktop Installer";
            ((System.ComponentModel.ISupportInitialize)pictureBox1).EndInit();
            panel1.ResumeLayout(false);
            panel2.ResumeLayout(false);
            ResumeLayout(false);
        }

        #endregion

        private Panel contentPanel;
        private Button rightBtn;
        private Button leftBtn;
        private PictureBox pictureBox1;
        private Panel panel1;
        private Panel panel2;
        private Label titleLabel;
        private Label descLabel;
    }
}