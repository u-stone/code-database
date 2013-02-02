using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Security.Permissions;
using System.Threading;
using System.Windows.Forms;

namespace Office2Pdf
{
    ///参考资料:
    ///http://msdn.microsoft.com/zh-cn/library/system.io.filesystemwatcher.notifyfilter.aspx?cs-save-lang=1&cs-lang=csharp#code-snippet-2

    public delegate void Convert2Pdf();

    class FileWatcher
    {
        private static List<string> m_FilesWord = new List<string>();
        private static List<string> m_FilesExcel = new List<string>();
        private static List<string> m_FilesPowerPoint = new List<string>();
        private static List<string> m_Files1 = new List<string>();
        private static List<string> m_Files2 = new List<string>();
        private static string m_PathToWatch;

        private static EventWaitHandle m_hWait = new AutoResetEvent(false);
        //private static long m_ThreadCount = 0;
        private static Convert2Pdf []m_DeConvert = new Convert2Pdf[3];

        public void StartWatch(object obj)
        {
            FileSystemWatcher fw = new FileSystemWatcher();
            fw.Path = m_PathToWatch;
            fw.NotifyFilter = NotifyFilters.LastAccess | 
                NotifyFilters.CreationTime | 
                NotifyFilters.FileName | 
                NotifyFilters.DirectoryName | 
                NotifyFilters.LastWrite;
            fw.Filter = "*.*";
            fw.IncludeSubdirectories = true;//监视子文件夹
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
            try
            {
                //限定最大线程，如果当前线程数超过线程数，那么就保存数据直接返回，在已有的线程执行完成之后检查是否有新的文件
                string strName = e.FullPath;
                m_Files1.Add(strName);
                //在打开并转化office文件的过程中会产生两种文件，一个是备份文件~$FileName.*，一个是临时文件*.tmp，需要排除掉前面那种情况
                if (e.Name.Substring(e.Name.LastIndexOf('\\') + 1, 2).CompareTo("~$") == 0 || e.Name.Substring(0, 8).CompareTo("RECYCLER") == 0)
                    return;
                m_Files2.Add(strName);
                string strExt = strName.Substring(strName.LastIndexOf('.', strName.Length - 1) + 1).ToUpper();
                switch (strExt)
                {
                    //Word类型的文件
                    case "DOC":
                    case "DOCX":
                        //if (m_ThreadCount > 20)
                        //{
                        //    Monitor.Enter(m_FilesWord);
                        //    m_FilesWord.Add(strName);
                        //    Monitor.Exit(m_FilesWord);
                        //}
                        //else
                        AsyncWord2Pdf(strName);
                        break;

                    //PowerPoint类型的文件
                    case "PPSX":
                    case "PPTX":
                    case "PPT":
                        //if (m_ThreadCount > 20)
                        //{
                        //    Monitor.Enter(m_FilesPowerPoint);
                        //    m_FilesPowerPoint.Add(strName);
                        //    Monitor.Exit(m_FilesPowerPoint);
                        //}
                        //else
                        AsyncPowerPoint2Pdf(strName);
                        break;

                    //Excel类型的文件
                    case "XLSX":
                    case "XLS":
                        //if (m_ThreadCount > 20)
                        //{
                        //    Monitor.Enter(m_FilesExcel);
                        //    m_FilesExcel.Add(strName);
                        //    Monitor.Exit(m_FilesExcel);
                        //}
                        //else
                        AsyncExcel2Pdf(strName);
                        break;

                    default:
                        break;
                }
                GC.Collect();
                GC.WaitForPendingFinalizers();
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }
            catch (System.Exception err)
            {
                err.ToString();
            }
        }
        private void AsyncWord2Pdf(string strDocPath)
        {
            //Interlocked.Increment(ref m_ThreadCount);
            Word2Pdf w2p = new Word2Pdf();
            w2p.FilePathWord = strDocPath;
            m_DeConvert[0] = w2p.ToPdf;
            IAsyncResult ar = m_DeConvert[0].BeginInvoke(new AsyncCallback(callbackWord2PdfConvert), m_DeConvert[0]);
            //ThreadPool.QueueUserWorkItem(new WaitCallback(callbackWord2PdfConvert), m_DeConvert[0]);
        }
        private void AsyncExcel2Pdf(string strXlsPath)
        {
            //Interlocked.Increment(ref m_ThreadCount);
            Excel2Pdf xls2Pdf = new Excel2Pdf();
            xls2Pdf.FilePathExcel = strXlsPath;
            m_DeConvert[1] = xls2Pdf.ToPdf;
            IAsyncResult ar = m_DeConvert[1].BeginInvoke(new AsyncCallback(callbackExcel2PdfConvert), m_DeConvert[1]);
            //ThreadPool.QueueUserWorkItem(new WaitCallback(callbackExcel2PdfConvert), m_DeConvert[1]);
        }
        private void AsyncPowerPoint2Pdf(string strPPTPath)
        {
            //Interlocked.Increment(ref m_ThreadCount);
            PowerPoint2Pdf p2p = new PowerPoint2Pdf();
            p2p.FilePathPowerPoint = strPPTPath;
            m_DeConvert[2] = p2p.ToPdf;
            IAsyncResult ar = m_DeConvert[2].BeginInvoke(new AsyncCallback(callbackPowerPoint2PdfConvert), m_DeConvert[2]);
            //ThreadPool.QueueUserWorkItem(new WaitCallback(callbackPowerPoint2PdfConvert), m_DeConvert[2]);
        }
        //检查是否有文件在队列中等待
        private bool checkWordBuf(IAsyncResult result)
        {
            bool ret = false;

            Monitor.Enter(m_FilesWord);
            if (m_FilesWord.Count() > 0)
                ret = true;
            Monitor.Exit(m_FilesWord);

            return ret;
        }
        //检查是否有文件在队列中等待
        private bool checkExcelBuf(IAsyncResult result)
        {
            bool ret = false;

            Monitor.Enter(m_FilesExcel);
            if (m_FilesExcel.Count() > 0)
                ret = true;
            Monitor.Exit(m_FilesExcel);

            return ret;
        }
        //检查是否有文件在队列中等待
        private bool checkPowerPointBuf(IAsyncResult result)
        {
            bool ret = false;

            Monitor.Enter(m_FilesPowerPoint);
            if (m_FilesPowerPoint.Count() > 0)
                ret = true;
            Monitor.Exit(m_FilesPowerPoint);

            return ret;
        }
        //WORD -> pdf线程数目的递减
        private void callbackWord2PdfConvert(IAsyncResult result)
        {
            //Interlocked.Decrement(ref m_ThreadCount);
            Convert2Pdf cv = (Convert2Pdf)result.AsyncState;
            //cv.Invoke();
            cv.EndInvoke(result);

            //if (checkWordBuf(null))
            //{
            //    string strFile = "";
            //    Monitor.Enter(m_FilesWord);
            //    if (m_FilesWord.Count() > 0)
            //    {
            //        strFile = m_FilesWord[0];
            //        m_FilesWord.RemoveAt(0);
            //    }
            //    Monitor.Exit(m_FilesWord);
            //    AsyncWord2Pdf(strFile);
            //    //Word2Pdf w2p = new Word2Pdf();
            //    //w2p.FilePathWord = strFile;
            //    //m_DeConvert[0] = w2p.ToPdf;
            //    //m_DeConvert[0].BeginInvoke(callbackWord2PdfConvert, null);
            //}
        }
        //Excel -> pdf线程数目的递减
        private void callbackExcel2PdfConvert(IAsyncResult result)
        {
            //Interlocked.Decrement(ref m_ThreadCount);
            Convert2Pdf cv = (Convert2Pdf)result.AsyncState;
            //cv.Invoke();
            cv.EndInvoke(result);

            //if (checkExcelBuf(null))
            //{
            //    string strFile = "";
            //    Monitor.Enter(m_FilesExcel);
            //    if (m_FilesExcel.Count() > 0)
            //    {
            //        strFile = m_FilesExcel[0];
            //        m_FilesExcel.RemoveAt(0);
            //    }
            //    Monitor.Exit(m_FilesExcel);
            //    AsyncExcel2Pdf(strFile);
            //    //Excel2Pdf x2p = new Excel2Pdf();
            //    //x2p.FilePathExcel = strFile;
            //    //m_DeConvert[1] = x2p.ToPdf;
            //    //m_DeConvert[1].BeginInvoke(callbackExcel2PdfConvert, null);
            //}

        }
        //PPT -> pdf线程数目的递减
        private void callbackPowerPoint2PdfConvert(IAsyncResult result)
        {
            //Interlocked.Decrement(ref m_ThreadCount);
            Convert2Pdf cv = (Convert2Pdf)result.AsyncState;
            //cv.Invoke();
            cv.EndInvoke(result);

            //if (checkPowerPointBuf(null))
            //{
            //    string strFile = "";
            //    Monitor.Enter(m_FilesPowerPoint);
            //    if (m_FilesPowerPoint.Count() > 0)
            //    {
            //        strFile = m_FilesPowerPoint[0];
            //        m_FilesPowerPoint.RemoveAt(0);
            //    }
            //    Monitor.Exit(m_FilesPowerPoint);
            //    AsyncPowerPoint2Pdf(strFile);
            //    //PowerPoint2Pdf p2p = new PowerPoint2Pdf();
            //    //p2p.FilePathPowerPoint = strFile;
            //    //m_DeConvert[2] = p2p.ToPdf;
            //    //m_DeConvert[2].BeginInvoke(callbackPowerPoint2PdfConvert, null);
            //}
        }
    }

}
