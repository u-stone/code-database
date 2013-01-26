using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Security.Permissions;
using System.Threading;

namespace Office2Pdf
{
    ///参考资料:
    ///http://msdn.microsoft.com/zh-cn/library/system.io.filesystemwatcher.notifyfilter.aspx?cs-save-lang=1&cs-lang=csharp#code-snippet-2

    class FileWatcher
    {
        private static List<string> m_FilesWord = new List<string>();
        private static List<string> m_FilesExcel = new List<string>();
        private static List<string> m_FilesPowerPoint = new List<string>();
        private static string m_PathToWatch;
        private static EventWaitHandle m_hWait = new AutoResetEvent(false);
        private static long m_ThreadCount = 0;
        public void StartWatch()
        {
            FileSystemWatcher fw = new FileSystemWatcher();
            fw.Path = m_PathToWatch;
            fw.NotifyFilter = NotifyFilters.LastAccess | 
                NotifyFilters.CreationTime | 
                NotifyFilters.FileName | 
                NotifyFilters.DirectoryName | 
                NotifyFilters.LastWrite;
            fw.Filter = "*.*";
            //fw.Changed += new FileSystemEventHandler(OnChanged);
            fw.Created += new FileSystemEventHandler(OnCreated);
            //fw.Deleted += new FileSystemEventHandler(OnDeleted);
            //fw.Renamed += new RenamedEventHandler(OnRenamed);

            fw.EnableRaisingEvents = true;
            m_hWait.WaitOne();
        }
        public string PathToWatch
        {
            get { return m_PathToWatch; }
            set { m_PathToWatch = value; }
        }
        public List<string> FileWord
        {
            get { return m_FilesWord; }
        }
        public List<string> FileExcel
        {
            get { return m_FilesExcel; }
        }
        public List<string> FilepowerPoint
        {
            get { return m_FilesPowerPoint; }
        }
        public void Stop()
        {
            m_hWait.Set();
        }
        private void OnChanged(object source, FileSystemEventArgs e)
        {
        }
        private void OnRenamed(object source, RenamedEventArgs e)
        {
        }
        private void OnDeleted(object source, FileSystemEventArgs e)
        {
        }
        private void OnCreated(object source, FileSystemEventArgs e)
        {
            //限定最大线程，如果当前线程数超过线程数，那么就保存数据直接返回，在已有的线程执行完成之后检查是否有新的文件
            string strName = e.Name;
            string strExt = strName.Substring(strName.LastIndexOf('.', strName.Length - 1) + 1).ToUpper();
            switch (strExt)
            {
                    //Word类型的文件
                case "DOC":
                case "DOCX":
                    if (m_ThreadCount > 20)
                    {
                        Monitor.Enter(m_FilesWord);
                        m_FilesWord.Add(strName);
                        Monitor.Exit(m_FilesWord);
                    }
                    else
                        AsyncWord2Pdf(strName);
                    break;

                    //PowerPoint类型的文件
                case "PPSX":
                case "PPTX":
                case "PPT":
                    if (m_ThreadCount > 20)
                    {
                        Monitor.Enter(m_FilesPowerPoint);
                        m_FilesPowerPoint.Add(strName);
                        Monitor.Exit(m_FilesPowerPoint);
                    }
                    else
                        AsyncPowerPoint2Pdf(strName);
                    break;

                    //Excel类型的文件
                case "XLSX":
                case "XLS":
                    if (m_ThreadCount > 20)
                    {
                        Monitor.Enter(m_FilesExcel);
                        m_FilesExcel.Add(strName);
                        Monitor.Exit(m_FilesExcel);
                    }
                    else
                        AsyncExcel2Pdf(strName); 
                    break;
                default:
                    break;
            }
            Interlocked.Increment(ref m_ThreadCount);
        }
        private string AsyncWord2Pdf(string strDocPath)
        {
            Word2Pdf w2p = new Word2Pdf();
            w2p.FilePathWord = strDocPath;
            //Thread thrd = new Thread(w2p.ToPdf);
            //thrd.Start();
            WordConvert2Pdf wcp = w2p.ToPdf;
            IAsyncResult ar = wcp.BeginInvoke(null, null);
            wcp.EndInvoke(ar);
            Interlocked.Decrement(ref m_ThreadCount);
            if (checkWordBuf())
            {
                string strFile = "";
                Monitor.Enter(m_FilesWord);
                if (m_FilesWord.Count() > 0)
                {
                    strFile = m_FilesWord[0];
                    m_FilesWord.RemoveAt(0);
                }
                Monitor.Exit(m_FilesWord);
                return strFile;
            }
            return null;
        }
        private void AsyncExcel2Pdf(string strXlsPath)
        {
            Excel2Pdf xls2Pdf = new Excel2Pdf();
            xls2Pdf.FilePathExcel = strXlsPath;
            ExcelConvert2Pdf e2p = xls2Pdf.ToPdf;
            IAsyncResult ar = e2p.BeginInvoke(null, null);
            e2p.EndInvoke(ar);
            Interlocked.Decrement(ref m_ThreadCount);
            checkExcelBuf();
        }
        private void AsyncPowerPoint2Pdf(string strPPTPath)
        {
            PowerPoint2Pdf p2p = new PowerPoint2Pdf();
            p2p.FilePathPowerPoint = strPPTPath;
            ConvertPowerPoint2Pdf dp2p = p2p.ToPdf;
            IAsyncResult ar = dp2p.BeginInvoke(null, null);
            dp2p.EndInvoke(ar);
            Interlocked.Decrement(ref m_ThreadCount);
            checkPowerPointBuf();
        }
        //检查是否有文件在队列中等待
        private bool checkWordBuf()
        {
            bool ret = false;

            Monitor.Enter(m_FilesWord);
            if (m_FilesWord.Count() > 0)
                ret = true;
            Monitor.Exit(m_FilesWord);

            return ret;
        }
        //检查是否有文件在队列中等待
        private bool checkExcelBuf()
        {
            bool ret = false;

            Monitor.Enter(m_FilesExcel);
            if (m_FilesExcel.Count() > 0)
                ret = true;
            Monitor.Exit(m_FilesExcel);

            return ret;
        }
        //检查是否有文件在队列中等待
        private bool checkPowerPointBuf()
        {
            bool ret = false;

            Monitor.Enter(m_FilesPowerPoint);
            if (m_FilesPowerPoint.Count() > 0)
                ret = true;
            Monitor.Exit(m_FilesPowerPoint);

            return ret;
        }
    }

}
