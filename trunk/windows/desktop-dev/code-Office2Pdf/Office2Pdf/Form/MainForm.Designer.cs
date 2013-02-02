namespace Office2Pdf
{
    partial class MainForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.ChooseMonitorFolder = new System.Windows.Forms.Button();
            this.MonPathList = new System.Windows.Forms.ListBox();
            this.startMon = new System.Windows.Forms.Button();
            this.MonActNote = new System.Windows.Forms.Label();
            this.Office2Pdf = new System.Windows.Forms.NotifyIcon(this.components);
            this.SuspendLayout();
            // 
            // ChooseMonitorFolder
            // 
            this.ChooseMonitorFolder.Location = new System.Drawing.Point(12, 12);
            this.ChooseMonitorFolder.Name = "ChooseMonitorFolder";
            this.ChooseMonitorFolder.Size = new System.Drawing.Size(111, 23);
            this.ChooseMonitorFolder.TabIndex = 0;
            this.ChooseMonitorFolder.Text = "选择监视路径";
            this.ChooseMonitorFolder.UseVisualStyleBackColor = true;
            this.ChooseMonitorFolder.Click += new System.EventHandler(this.ChooseMonitorFolder_Click);
            // 
            // MonPathList
            // 
            this.MonPathList.FormattingEnabled = true;
            this.MonPathList.ItemHeight = 12;
            this.MonPathList.Location = new System.Drawing.Point(12, 36);
            this.MonPathList.Name = "MonPathList";
            this.MonPathList.Size = new System.Drawing.Size(468, 112);
            this.MonPathList.TabIndex = 2;
            // 
            // startMon
            // 
            this.startMon.Location = new System.Drawing.Point(12, 163);
            this.startMon.Name = "startMon";
            this.startMon.Size = new System.Drawing.Size(111, 23);
            this.startMon.TabIndex = 3;
            this.startMon.Text = "开启监视线程";
            this.startMon.UseVisualStyleBackColor = true;
            this.startMon.Click += new System.EventHandler(this.startMon_Click);
            // 
            // MonActNote
            // 
            this.MonActNote.AutoSize = true;
            this.MonActNote.Location = new System.Drawing.Point(127, 168);
            this.MonActNote.Name = "MonActNote";
            this.MonActNote.Size = new System.Drawing.Size(89, 12);
            this.MonActNote.TabIndex = 4;
            this.MonActNote.Text = "未开启监视线程";
            // 
            // Office2Pdf
            // 
            this.Office2Pdf.Icon = ((System.Drawing.Icon)(resources.GetObject("Office2Pdf.Icon")));
            this.Office2Pdf.Text = "TaskBarIcon";
            this.Office2Pdf.Visible = true;
            this.Office2Pdf.MouseClick += new System.Windows.Forms.MouseEventHandler(this.Office2Pdf_MouseClick);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(492, 197);
            this.Controls.Add(this.MonActNote);
            this.Controls.Add(this.startMon);
            this.Controls.Add(this.MonPathList);
            this.Controls.Add(this.ChooseMonitorFolder);
            this.Name = "MainForm";
            this.Text = "Office转Pdf";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button ChooseMonitorFolder;
        private System.Windows.Forms.ListBox MonPathList;
        private System.Windows.Forms.Button startMon;
        private System.Windows.Forms.Label MonActNote;
        private System.Windows.Forms.NotifyIcon Office2Pdf;
    }
}

