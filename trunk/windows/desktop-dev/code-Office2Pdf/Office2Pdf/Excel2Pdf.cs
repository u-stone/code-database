using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CORE = Microsoft.Office.Core;
using XLS = Microsoft.Office.Interop.Excel;


namespace Office2Pdf
{
    public delegate void ExcelConvert2Pdf();
    class Excel2Pdf
    {
        private string m_FileExcel = @"C:\";
        public void ToPdf()
        {
            XLS.Application xlsApp = new Microsoft.Office.Interop.Excel.Application();
            object paramMissing = Type.Missing;
            string strXls = m_FileExcel.Substring(0, m_FileExcel.LastIndexOf('.')) + "pdf";
            try
            {
                XLS.Workbook workbook = xlsApp.Workbooks.Open(m_FileExcel, paramMissing, paramMissing,
                    paramMissing, paramMissing, paramMissing, paramMissing, paramMissing, paramMissing,
                    paramMissing, paramMissing, paramMissing, paramMissing, paramMissing, paramMissing);
                workbook.ExportAsFixedFormat(XLS.XlFixedFormatType.xlTypePDF, strXls, paramMissing,
                    paramMissing, paramMissing, paramMissing, paramMissing, paramMissing, paramMissing);
                //xlsApp.Workbooks.Close();
                xlsApp.Quit();
            }
            catch (System.Exception e)
            {
                e.ToString();
            }
            finally
            {
                if (xlsApp != null)
                {
                    xlsApp.Workbooks.Close();
                    xlsApp.Quit();
                }
            }
        }
        public string FilePathExcel
        {
            get { return m_FileExcel; }
            set { m_FileExcel = value; }
        }
    }
}
