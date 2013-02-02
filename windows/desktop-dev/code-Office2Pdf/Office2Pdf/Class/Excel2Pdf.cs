using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;
using CORE = Microsoft.Office.Core;
using XLS = Microsoft.Office.Interop.Excel;


namespace Office2Pdf
{
    class Excel2Pdf
    {
        private string m_FilePathExcel = @"C:\";
        public void ToPdf()
        {
            bool occupy = true;
            while (occupy)
            {
                try
                {
                    using (File.Open(m_FilePathExcel, FileMode.Open, FileAccess.ReadWrite, FileShare.None))
                    { }
                    occupy = false;//如果可以运行至此那么就是
                }
                catch (IOException e)
                {
                    e.ToString();
                    Thread.Sleep(100);
                }
            }
            XLS.Application xlsApp = new Microsoft.Office.Interop.Excel.Application();
            object paramMissing = Type.Missing;
            string strXls = m_FilePathExcel.Substring(0, m_FilePathExcel.LastIndexOf('.')) + ".pdf";
            XLS.Workbook workbook = null;
            try
            {
                workbook = xlsApp.Workbooks.Open(m_FilePathExcel, paramMissing, paramMissing,
                    paramMissing, paramMissing, paramMissing, paramMissing, paramMissing, paramMissing,
                    paramMissing, paramMissing, paramMissing, paramMissing, paramMissing, paramMissing);
                if (workbook != null)
                {
                    workbook.ExportAsFixedFormat(XLS.XlFixedFormatType.xlTypePDF, strXls, paramMissing,
                        paramMissing, paramMissing, paramMissing, paramMissing, paramMissing, paramMissing);
                }
                if (workbook != null)
                {
                    workbook.Close(Type.Missing, Type.Missing, Type.Missing);
                    System.Runtime.InteropServices.Marshal.ReleaseComObject(workbook);
                }
                xlsApp.Workbooks.Close();
                if (xlsApp != null)
                {
                    xlsApp.Workbooks.Close();
                    xlsApp.Quit();
                    System.Runtime.InteropServices.Marshal.ReleaseComObject(xlsApp);
                }
                PostHttpMsg pm = new PostHttpMsg();
                pm.DataPost = strXls;
                pm.PostMsg();
            }
            catch (System.Exception e)
            {
                e.ToString();
            }
            finally
            {
                workbook = null;
                xlsApp = null;
                GC.Collect();
                GC.WaitForPendingFinalizers();
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }
        }
        public string FilePathExcel
        {
            get { return m_FilePathExcel; }
            set { m_FilePathExcel = value; }
        }
    }
}
