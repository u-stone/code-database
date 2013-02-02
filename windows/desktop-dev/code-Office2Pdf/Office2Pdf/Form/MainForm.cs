using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO;

namespace Office2Pdf
{
    public partial class MainForm : Form
    {
        private List<string> m_strPathArray = new List<string>();
        private List<FileWatcher> m_FileWatcherArray = new List<FileWatcher>();
        private bool m_bMonThreadState = false;
        //FileWatcher m_fw = new FileWatcher();
        public MainForm()
        {
            InitializeComponent();
        }

        private void ChooseMonitorFolder_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fdlg = new FolderBrowserDialog();
            fdlg.Description = "选择要监视的路径";
            fdlg.ShowNewFolderButton = false;
            string str = @"C:\";
            if (fdlg.ShowDialog() == DialogResult.OK)
                str = fdlg.SelectedPath;
            //需要做判断，比如说路径前缀，后缀的匹配问题
            addValidPath(str);
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            //添加关闭窗口时的事件处理函数
            this.FormClosing += new FormClosingEventHandler(beforeCloseForm);
            SizeChanged += new EventHandler(formSizeChanged);
        }
        public void beforeCloseForm(object obj, FormClosingEventArgs arg)
        {
            //退出窗口之前要关闭监视
            //m_fw.Stop();
            foreach (FileWatcher fw in m_FileWatcherArray)
            {
                fw.Stop();
            }
        }
        private void formSizeChanged(object sender, EventArgs e)
        {
            if (this.WindowState == FormWindowState.Minimized)
            {
                this.Hide();
                ShowInTaskbar = false;
                this.Office2Pdf.Visible = true;
            }
        }

        private void startMon_Click(object sender, EventArgs e)
        {
            if (m_bMonThreadState == false)
            {
                foreach (string strPath in m_strPathArray)
                {
                    FileWatcher fw = new FileWatcher();
                    fw.PathToWatch = strPath;
                    m_FileWatcherArray.Add(fw);
                    ThreadPool.QueueUserWorkItem(new WaitCallback(fw.StartWatch));
                }
                MonActNote.Text = "已开启监视线程";
                startMon.Text = "关闭监视线程";
                m_bMonThreadState = true;
            }
            else
            {
                foreach (FileWatcher fw in m_FileWatcherArray)
                {
                    fw.Stop();
                }
                Thread.Sleep(100);
                GC.Collect();
                GC.WaitForPendingFinalizers();
                GC.Collect();
                GC.WaitForPendingFinalizers();
                MonActNote.Text = "未开启监视线程";
                startMon.Text = "开启监视线程";
                m_bMonThreadState = false;
            }
        }
        //检查当前添加进来的路径Z与在路径数组中的路径的包含关系，最后取一个较大的路径
        private bool addValidPath(string str)
        {
            //先考虑该路径是否存在
            if (!Directory.Exists(str))
                return false;
            //考虑新加路径是已有路径的子路径的情况（即存在已有的路径字符串是新加路径字符串的前缀）
            foreach (string strPath in m_strPathArray)
            {
                if (strPath.Length <= str.Length)
                {
                    if (str.Substring(0, strPath.Length).CompareTo(strPath) == 0)
                        return false;
                }
            }
            //考虑新添加路径是已有路径的父级目录（即新添加目录是已有的目录的前缀）

            for (int i = 0; i < m_strPathArray.Count; ++i)
            {
                if (m_strPathArray[i].Length >= str.Length)
                {
                    if (m_strPathArray[i].Substring(0, str.Length).CompareTo(str) == 0)
                        m_strPathArray[i] = "";
                }
            }
            if (m_strPathArray.Count > 0)
            {
                for (int i = m_strPathArray.Count - 1; i >= 0; --i)
                {
                    if (m_strPathArray[i].Length == 0)
                        m_strPathArray.RemoveAt(i);
                }
            }
            m_strPathArray.Add(str);
            MonPathList.Items.Clear();
            foreach (string strPath in m_strPathArray)
            {
                MonPathList.Items.Add(strPath);
            }
            return true;
        }

        private void Office2Pdf_MouseClick(object sender, MouseEventArgs e)
        {
            Visible = true;
            WindowState = FormWindowState.Normal;
            ShowInTaskbar = true;
            Office2Pdf.Visible = false;
        }
    }
}
