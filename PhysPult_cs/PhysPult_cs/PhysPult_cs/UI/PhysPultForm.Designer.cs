namespace PhysPult.UI
{
    partial class PhysPultForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.statusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.comLogListBox = new System.Windows.Forms.ListBox();
            this.clearLogButton = new System.Windows.Forms.Button();
            this.comPortsComboBox = new System.Windows.Forms.ComboBox();
            this.connectButton = new System.Windows.Forms.Button();
            this.disconnectButton = new System.Windows.Forms.Button();
            this.setValueButton = new System.Windows.Forms.Button();
            this.requestUpdateButton = new System.Windows.Forms.Button();
            this.setHueButton = new System.Windows.Forms.Button();
            this.hueTextBox = new System.Windows.Forms.TextBox();
            this.valueTextBox = new System.Windows.Forms.TextBox();
            this.setSensitivityButton = new System.Windows.Forms.Button();
            this.sensitivityTextBox = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.statusStrip.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip
            // 
            this.statusStrip.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusLabel});
            this.statusStrip.Location = new System.Drawing.Point(0, 427);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(282, 26);
            this.statusStrip.TabIndex = 2;
            // 
            // statusLabel
            // 
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(50, 20);
            this.statusLabel.Text = "Idling.";
            // 
            // comLogListBox
            // 
            this.comLogListBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.comLogListBox.FormattingEnabled = true;
            this.comLogListBox.ItemHeight = 20;
            this.comLogListBox.Location = new System.Drawing.Point(11, 239);
            this.comLogListBox.Name = "comLogListBox";
            this.comLogListBox.Size = new System.Drawing.Size(256, 184);
            this.comLogListBox.TabIndex = 1;
            // 
            // clearLogButton
            // 
            this.clearLogButton.Location = new System.Drawing.Point(11, 202);
            this.clearLogButton.Name = "clearLogButton";
            this.clearLogButton.Size = new System.Drawing.Size(95, 29);
            this.clearLogButton.TabIndex = 2;
            this.clearLogButton.Text = "Clear log";
            this.clearLogButton.UseVisualStyleBackColor = true;
            this.clearLogButton.Click += new System.EventHandler(this.clearLogButton_Click);
            // 
            // comPortsComboBox
            // 
            this.comPortsComboBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.comPortsComboBox.FormattingEnabled = true;
            this.comPortsComboBox.Location = new System.Drawing.Point(10, 26);
            this.comPortsComboBox.Name = "comPortsComboBox";
            this.comPortsComboBox.Size = new System.Drawing.Size(257, 28);
            this.comPortsComboBox.TabIndex = 0;
            this.comPortsComboBox.SelectedIndexChanged += new System.EventHandler(this.comPortsComboBox_SelectedIndexChanged);
            // 
            // connectButton
            // 
            this.connectButton.Location = new System.Drawing.Point(10, 60);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(94, 27);
            this.connectButton.TabIndex = 8;
            this.connectButton.Text = "Connect";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // disconnectButton
            // 
            this.disconnectButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.disconnectButton.Location = new System.Drawing.Point(173, 60);
            this.disconnectButton.Name = "disconnectButton";
            this.disconnectButton.Size = new System.Drawing.Size(94, 27);
            this.disconnectButton.TabIndex = 9;
            this.disconnectButton.Text = "Disconnect";
            this.disconnectButton.UseVisualStyleBackColor = true;
            this.disconnectButton.Click += new System.EventHandler(this.disconnectButton_Click);
            // 
            // setValueButton
            // 
            this.setValueButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.setValueButton.Location = new System.Drawing.Point(139, 136);
            this.setValueButton.Name = "setValueButton";
            this.setValueButton.Size = new System.Drawing.Size(129, 27);
            this.setValueButton.TabIndex = 5;
            this.setValueButton.Text = "Set min value";
            this.setValueButton.UseVisualStyleBackColor = true;
            this.setValueButton.Click += new System.EventHandler(this.setValueButton_Click);
            // 
            // requestUpdateButton
            // 
            this.requestUpdateButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.requestUpdateButton.Location = new System.Drawing.Point(138, 202);
            this.requestUpdateButton.Name = "requestUpdateButton";
            this.requestUpdateButton.Size = new System.Drawing.Size(129, 29);
            this.requestUpdateButton.TabIndex = 3;
            this.requestUpdateButton.Text = "Request state";
            this.requestUpdateButton.UseVisualStyleBackColor = true;
            this.requestUpdateButton.Click += new System.EventHandler(this.requestUpdateButton_Click);
            // 
            // setHueButton
            // 
            this.setHueButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.setHueButton.Location = new System.Drawing.Point(139, 103);
            this.setHueButton.Name = "setHueButton";
            this.setHueButton.Size = new System.Drawing.Size(129, 27);
            this.setHueButton.TabIndex = 2;
            this.setHueButton.Text = "Set hue";
            this.setHueButton.UseVisualStyleBackColor = true;
            this.setHueButton.Click += new System.EventHandler(this.setHueButton_Click);
            // 
            // hueTextBox
            // 
            this.hueTextBox.Location = new System.Drawing.Point(11, 103);
            this.hueTextBox.MaxLength = 3;
            this.hueTextBox.Name = "hueTextBox";
            this.hueTextBox.PlaceholderText = "0-255";
            this.hueTextBox.Size = new System.Drawing.Size(94, 27);
            this.hueTextBox.TabIndex = 1;
            this.hueTextBox.Text = "128";
            this.hueTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.hueTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.setHueButton_KeyDown);
            // 
            // valueTextBox
            // 
            this.valueTextBox.Location = new System.Drawing.Point(11, 136);
            this.valueTextBox.MaxLength = 3;
            this.valueTextBox.Name = "valueTextBox";
            this.valueTextBox.PlaceholderText = "0-255";
            this.valueTextBox.Size = new System.Drawing.Size(94, 27);
            this.valueTextBox.TabIndex = 4;
            this.valueTextBox.Text = "55";
            this.valueTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.valueTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.valueTextBox_KeyDown);
            // 
            // setSensitivityButton
            // 
            this.setSensitivityButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.setSensitivityButton.Location = new System.Drawing.Point(138, 169);
            this.setSensitivityButton.Name = "setSensitivityButton";
            this.setSensitivityButton.Size = new System.Drawing.Size(129, 27);
            this.setSensitivityButton.TabIndex = 10;
            this.setSensitivityButton.Text = "Set sensitivity";
            this.setSensitivityButton.UseVisualStyleBackColor = true;
            this.setSensitivityButton.Click += new System.EventHandler(this.setSensitivityButton_Click);
            // 
            // sensitivityTextBox
            // 
            this.sensitivityTextBox.Location = new System.Drawing.Point(11, 169);
            this.sensitivityTextBox.MaxLength = 3;
            this.sensitivityTextBox.Name = "sensitivityTextBox";
            this.sensitivityTextBox.PlaceholderText = "0.0-4.0";
            this.sensitivityTextBox.Size = new System.Drawing.Size(95, 27);
            this.sensitivityTextBox.TabIndex = 9;
            this.sensitivityTextBox.Text = "3";
            this.sensitivityTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.sensitivityTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox1_KeyDown);
            // 
            // groupBox1
            // 
            this.groupBox1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.groupBox1.Controls.Add(this.comPortsComboBox);
            this.groupBox1.Controls.Add(this.comLogListBox);
            this.groupBox1.Controls.Add(this.clearLogButton);
            this.groupBox1.Controls.Add(this.disconnectButton);
            this.groupBox1.Controls.Add(this.connectButton);
            this.groupBox1.Controls.Add(this.sensitivityTextBox);
            this.groupBox1.Controls.Add(this.requestUpdateButton);
            this.groupBox1.Controls.Add(this.setSensitivityButton);
            this.groupBox1.Controls.Add(this.setHueButton);
            this.groupBox1.Controls.Add(this.hueTextBox);
            this.groupBox1.Controls.Add(this.valueTextBox);
            this.groupBox1.Controls.Add(this.setValueButton);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(282, 427);
            this.groupBox1.TabIndex = 11;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "COM port";
            // 
            // PhysPultForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(282, 453);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.statusStrip);
            this.MinimumSize = new System.Drawing.Size(300, 500);
            this.Name = "PhysPultForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Multicontrol";
            this.statusStrip.ResumeLayout(false);
            this.statusStrip.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private StatusStrip statusStrip;
        private ToolStripStatusLabel statusLabel;
        private ListBox comLogListBox;
        private Button clearLogButton;
        private ComboBox comPortsComboBox;
        private Button connectButton;
        private Button disconnectButton;
        private Button setValueButton;
        private Button requestUpdateButton;
        private Button setHueButton;
        private TextBox hueTextBox;
        private TextBox valueTextBox;
        private Button setSensitivityButton;
        private TextBox sensitivityTextBox;
        private GroupBox groupBox1;
    }
}