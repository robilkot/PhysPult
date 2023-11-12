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
            this.comPortsComboBox = new System.Windows.Forms.ComboBox();
            this.logListBox = new System.Windows.Forms.ListBox();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.statusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.panel1 = new System.Windows.Forms.Panel();
            this.cancelButton = new System.Windows.Forms.Button();
            this.valueTextBox = new System.Windows.Forms.TextBox();
            this.hueTextBox = new System.Windows.Forms.TextBox();
            this.setHueButton = new System.Windows.Forms.Button();
            this.requestUpdateButton = new System.Windows.Forms.Button();
            this.setValueButton = new System.Windows.Forms.Button();
            this.disconnectButton = new System.Windows.Forms.Button();
            this.connectButton = new System.Windows.Forms.Button();
            this.statusStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // comPortsComboBox
            // 
            this.comPortsComboBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.comPortsComboBox.FormattingEnabled = true;
            this.comPortsComboBox.Location = new System.Drawing.Point(15, 12);
            this.comPortsComboBox.Name = "comPortsComboBox";
            this.comPortsComboBox.Size = new System.Drawing.Size(238, 28);
            this.comPortsComboBox.TabIndex = 0;
            this.comPortsComboBox.SelectedIndexChanged += new System.EventHandler(this.comPortsComboBox_SelectedIndexChanged);
            // 
            // logListBox
            // 
            this.logListBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.logListBox.FormattingEnabled = true;
            this.logListBox.ItemHeight = 20;
            this.logListBox.Location = new System.Drawing.Point(13, 12);
            this.logListBox.Name = "logListBox";
            this.logListBox.Size = new System.Drawing.Size(505, 404);
            this.logListBox.TabIndex = 1;
            // 
            // statusStrip1
            // 
            this.statusStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusLabel});
            this.statusStrip1.Location = new System.Drawing.Point(0, 424);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(800, 26);
            this.statusStrip1.TabIndex = 2;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // statusLabel
            // 
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(50, 20);
            this.statusLabel.Text = "Idling.";
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.IsSplitterFixed = true;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.panel1);
            this.splitContainer1.Panel1.Controls.Add(this.disconnectButton);
            this.splitContainer1.Panel1.Controls.Add(this.connectButton);
            this.splitContainer1.Panel1.Controls.Add(this.comPortsComboBox);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.logListBox);
            this.splitContainer1.Size = new System.Drawing.Size(800, 424);
            this.splitContainer1.SplitterDistance = 266;
            this.splitContainer1.TabIndex = 3;
            // 
            // panel1
            // 
            this.panel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.panel1.Controls.Add(this.cancelButton);
            this.panel1.Controls.Add(this.valueTextBox);
            this.panel1.Controls.Add(this.hueTextBox);
            this.panel1.Controls.Add(this.setHueButton);
            this.panel1.Controls.Add(this.requestUpdateButton);
            this.panel1.Controls.Add(this.setValueButton);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 79);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(266, 345);
            this.panel1.TabIndex = 10;
            // 
            // cancelButton
            // 
            this.cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cancelButton.Location = new System.Drawing.Point(15, 284);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(238, 27);
            this.cancelButton.TabIndex = 8;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            // 
            // valueTextBox
            // 
            this.valueTextBox.Location = new System.Drawing.Point(15, 126);
            this.valueTextBox.MaxLength = 3;
            this.valueTextBox.Name = "valueTextBox";
            this.valueTextBox.PlaceholderText = "0-255";
            this.valueTextBox.Size = new System.Drawing.Size(138, 27);
            this.valueTextBox.TabIndex = 4;
            this.valueTextBox.Text = "128";
            this.valueTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.valueTextBox_KeyDown);
            // 
            // hueTextBox
            // 
            this.hueTextBox.Location = new System.Drawing.Point(15, 93);
            this.hueTextBox.MaxLength = 3;
            this.hueTextBox.Name = "hueTextBox";
            this.hueTextBox.PlaceholderText = "0-255";
            this.hueTextBox.Size = new System.Drawing.Size(138, 27);
            this.hueTextBox.TabIndex = 1;
            this.hueTextBox.Text = "128";
            this.hueTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.setHueButton_KeyDown);
            // 
            // setHueButton
            // 
            this.setHueButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.setHueButton.Location = new System.Drawing.Point(159, 93);
            this.setHueButton.Name = "setHueButton";
            this.setHueButton.Size = new System.Drawing.Size(94, 27);
            this.setHueButton.TabIndex = 2;
            this.setHueButton.Text = "Set Hue";
            this.setHueButton.UseVisualStyleBackColor = true;
            this.setHueButton.Click += new System.EventHandler(this.setHueButton_Click);
            // 
            // requestUpdateButton
            // 
            this.requestUpdateButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.requestUpdateButton.Location = new System.Drawing.Point(15, 60);
            this.requestUpdateButton.Name = "requestUpdateButton";
            this.requestUpdateButton.Size = new System.Drawing.Size(238, 27);
            this.requestUpdateButton.TabIndex = 3;
            this.requestUpdateButton.Text = "Request Update";
            this.requestUpdateButton.UseVisualStyleBackColor = true;
            this.requestUpdateButton.Click += new System.EventHandler(this.requestUpdateButton_Click);
            // 
            // setValueButton
            // 
            this.setValueButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.setValueButton.Location = new System.Drawing.Point(159, 126);
            this.setValueButton.Name = "setValueButton";
            this.setValueButton.Size = new System.Drawing.Size(94, 27);
            this.setValueButton.TabIndex = 5;
            this.setValueButton.Text = "Set Value";
            this.setValueButton.UseVisualStyleBackColor = true;
            this.setValueButton.Click += new System.EventHandler(this.setValueButton_Click);
            // 
            // disconnectButton
            // 
            this.disconnectButton.Location = new System.Drawing.Point(159, 46);
            this.disconnectButton.Name = "disconnectButton";
            this.disconnectButton.Size = new System.Drawing.Size(94, 27);
            this.disconnectButton.TabIndex = 9;
            this.disconnectButton.Text = "Disconnect";
            this.disconnectButton.UseVisualStyleBackColor = true;
            this.disconnectButton.Click += new System.EventHandler(this.disconnectButton_Click);
            // 
            // connectButton
            // 
            this.connectButton.Location = new System.Drawing.Point(15, 46);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(94, 27);
            this.connectButton.TabIndex = 8;
            this.connectButton.Text = "Connect";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // PhysPultForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.statusStrip1);
            this.Name = "PhysPultForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Async COM controller";
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private ComboBox comPortsComboBox;
        private ListBox logListBox;
        private StatusStrip statusStrip1;
        private ToolStripStatusLabel statusLabel;
        private SplitContainer splitContainer1;
        private Button requestUpdateButton;
        private Button setHueButton;
        private TextBox hueTextBox;
        private Button setValueButton;
        private TextBox valueTextBox;
        private Button disconnectButton;
        private Button connectButton;
        private Panel panel1;
        private Button cancelButton;
    }
}