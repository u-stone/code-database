using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CORE = Microsoft.Office.Core;
using PPT = Microsoft.Office.Interop.PowerPoint;

namespace Office2Pdf
{
    public delegate void ConvertPowerPoint2Pdf();
    class PowerPoint2Pdf
    {
        private string m_FilePathPowerPoint = @"C:\";
        public void ToPdf()
        {
            PPT.Application pptApp = new PPT.Application();
            PPT.Presentation presentation = pptApp.Presentations.Open(m_FilePathPowerPoint,
                Microsoft.Office.Core.MsoTriState.msoCTrue,
                Microsoft.Office.Core.MsoTriState.msoFalse,
                Microsoft.Office.Core.MsoTriState.msoFalse);
            string strPdf = m_FilePathPowerPoint.Substring(0, m_FilePathPowerPoint.LastIndexOf('.')) + "pdf";
            presentation.ExportAsFixedFormat(strPdf, PPT.PpFixedFormatType.ppFixedFormatTypePDF,
                PPT.PpFixedFormatIntent.ppFixedFormatIntentPrint,
                CORE.MsoTriState.msoFalse, PPT.PpPrintHandoutOrder.ppPrintHandoutVerticalFirst,
                PPT.PpPrintOutputType.ppPrintOutputSlides, Microsoft.Office.Core.MsoTriState.msoFalse,
                null, PPT.PpPrintRangeType.ppPrintAll, "",
                false, false, false, true, true, System.Reflection.Missing.Value);
            presentation.Close();
            presentation = null;
            pptApp = null;
            GC.Collect();
        }
        public string FilePathPowerPoint
        {
            get { return m_FilePathPowerPoint;  }
            set { m_FilePathPowerPoint = value; }
        }
    }
}
