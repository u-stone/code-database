using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;
using CORE = Microsoft.Office.Core;
using PPT = Microsoft.Office.Interop.PowerPoint;

namespace Office2Pdf
{
    class PowerPoint2Pdf
    {
        private string m_FilePathPowerPoint = @"C:\";
        public void ToPdf()
        {
            bool occupy = true;
            while (occupy)
            {
                try
                {
                    using (File.Open(m_FilePathPowerPoint, FileMode.Open, FileAccess.ReadWrite, FileShare.None))
                    { }
                    occupy = false;//如果可以运行至此那么就是
                }
                catch (IOException e)
                {
                    e.ToString();
                    Thread.Sleep(100);
                }
            }
            PPT.Application pptApp = null;
            PPT.Presentation presentation = null;
            try
            {
                pptApp = new PPT.Application();
                presentation = pptApp.Presentations.Open(m_FilePathPowerPoint,
                    Microsoft.Office.Core.MsoTriState.msoCTrue,
                    Microsoft.Office.Core.MsoTriState.msoFalse,
                    Microsoft.Office.Core.MsoTriState.msoFalse);
                string strPdf = m_FilePathPowerPoint.Substring(0, m_FilePathPowerPoint.LastIndexOf('.')) + ".pdf";
                if (presentation != null)
                {
                    presentation.ExportAsFixedFormat(strPdf, PPT.PpFixedFormatType.ppFixedFormatTypePDF,
                        PPT.PpFixedFormatIntent.ppFixedFormatIntentPrint,
                        CORE.MsoTriState.msoFalse, PPT.PpPrintHandoutOrder.ppPrintHandoutVerticalFirst,
                        PPT.PpPrintOutputType.ppPrintOutputSlides, Microsoft.Office.Core.MsoTriState.msoFalse,
                        null, PPT.PpPrintRangeType.ppPrintAll, "",
                        false, false, false, true, true, System.Reflection.Missing.Value);
                }
                if (presentation != null)
                {
                    presentation.Close();
                    System.Runtime.InteropServices.Marshal.ReleaseComObject(presentation);
                }
                if (pptApp != null)
                {
                    pptApp.Quit();
                    System.Runtime.InteropServices.Marshal.ReleaseComObject(pptApp);
                }
                PostHttpMsg pm = new PostHttpMsg();
                pm.DataPost = strPdf;
                pm.PostMsg();
            }
            catch (System.Exception e)
            {
                e.ToString();
            }
            finally
            {
                presentation = null;
                pptApp = null;
                GC.Collect();
                GC.WaitForPendingFinalizers();
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }

        }
        public string FilePathPowerPoint
        {
            get { return m_FilePathPowerPoint;  }
            set { m_FilePathPowerPoint = value; }
        }
    }
}
