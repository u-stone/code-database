using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CORE = Microsoft.Office.Core;
using WORD = Microsoft.Office.Interop.Word;

namespace Office2Pdf
{
    public delegate void WordConvert2Pdf();//ToPdf()函数的委托
    class Word2Pdf
    {
        public string m_FilePathWord;
        public void ToPdf()
        {
            WORD.ApplicationClass wordApp = new WORD.ApplicationClass();

            WORD.Document wordDoc = null;
            object paramSourceDoc = m_FilePathWord;
            object paramMissing = Type.Missing;
            string strPdf = m_FilePathWord.Substring(0, m_FilePathWord.LastIndexOf('.')) + "pdf";
            string paramExportFilePath = strPdf;
            WORD.WdExportFormat paramexportFormat = WORD.WdExportFormat.wdExportFormatPDF;
            bool paramOpenAfterExport = false;
            WORD.WdExportOptimizeFor parameExportOptimizeFor = WORD.WdExportOptimizeFor.wdExportOptimizeForPrint;
            WORD.WdExportRange parameExportRange = WORD.WdExportRange.wdExportAllDocument;
            int paramStartPage = 0;
            int paramEndPage = 0;
            WORD.WdExportItem paramExportItem = WORD.WdExportItem.wdExportDocumentContent;
            bool paramIncludeDocProps = true;
            bool paramKeepIRM = true;
            WORD.WdExportCreateBookmarks paramCreateBookMarks = WORD.WdExportCreateBookmarks.wdExportCreateWordBookmarks;
            bool paramDocStructureTags = true;
            bool paramBitmapMissingFonts = true;
            bool paramUseISO19005_1 = false;
            try
            {
                wordDoc = wordApp.Documents.Open(
                    ref paramSourceDoc, ref paramMissing, ref paramMissing,
                    ref paramMissing, ref paramMissing, ref paramMissing,
                    ref paramMissing, ref paramMissing, ref paramMissing,
                    ref paramMissing, ref paramMissing, ref paramMissing,
                    ref paramMissing, ref paramMissing, ref paramMissing,
                    ref paramMissing
                    );
                if (wordDoc != null)
                {
                    wordDoc.ExportAsFixedFormat(paramExportFilePath,
                        paramexportFormat, paramOpenAfterExport,
                        parameExportOptimizeFor, parameExportRange, paramStartPage,
                        paramEndPage, paramExportItem, paramIncludeDocProps,
                        paramKeepIRM, paramCreateBookMarks, paramDocStructureTags,
                        paramBitmapMissingFonts, paramUseISO19005_1, ref paramMissing);

                }
            }
            catch (System.Exception ex)
            {
                ex.ToString();
                //no
            }
            finally
            {
                if (wordDoc != null)
                {
                    wordDoc.Close(ref paramMissing, ref paramMissing, ref paramMissing);
                    wordDoc = null;
                }
                if (wordApp != null)
                {
                    wordApp.Quit(ref paramMissing, ref paramMissing, ref paramMissing);
                    wordApp = null;
                }
                GC.Collect();
                GC.WaitForPendingFinalizers();
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }
        }
        public string FilePathWord
        {
            get { return m_FilePathWord; }
            set { m_FilePathWord = value; }
        }
    }
}
