/////////////////////////////////////////////////////////////////////////////// 
// Copyright (C) 2003-2011, Open Design Alliance (the "Alliance"). 
// All rights reserved. 
// 
// This software and its documentation and related materials are owned by 
// the Alliance. The software may only be incorporated into application 
// programs owned by members of the Alliance, subject to a signed 
// Membership Agreement and Supplemental Software License Agreement with the
// Alliance. The structure and organization of this software are the valuable  
// trade secrets of the Alliance and its suppliers. The software is also 
// protected by copyright law and international treaty provisions. Application  
// programs incorporating this software must include the following statement 
// with their copyright notices:
//   
//   This application incorporates Teigha(R) software pursuant to a license 
//   agreement with Open Design Alliance.
//   Teigha(R) for .dwg files Copyright (C) 2003-2011 by Open Design Alliance. 
//   All rights reserved.
//
// By use of this software, its documentation or related materials, you 
// acknowledge and accept the above terms.
//
//
// *DWG is the native and proprietary file format for AutoCAD(R) and a trademark  
// of Autodesk, Inc.  Open Design Alliance is not associated with Autodesk.
///////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* This console application reads a DWG file and writes WMF or EMF file */
/*                                                                      */
/* Calling sequence:                                                    */
/*                                                                      */
/*   OdWmfExport <input file> <output file> <file type> <xSize> <ySize> */
/*               <disable background>                                   */
/************************************************************************/

#include "OdaCommon.h"
#include "DbDatabase.h"
#include "DbGsManager.h"
#include "RxVariantValue.h"
#include "AbstractViewPE.h"
#include "ColorMapping.h"

#include "Ge/GeExtents2d.h"
#include "DbBlockTableRecord.h"

#include "RxObjectImpl.h"
#define STL_USING_IOSTREAM
#include "OdaSTL.h"
#include <fstream>
#include <locale.h>
#include <tchar.h>

#include "ExSystemServices.h"
#include "ExHostAppServices.h"
#include "GiContextForDbDatabase.h"

#include "RxDynamicModule.h"

#if defined(TARGET_OS_MAC) && !defined(__MACH__)
#include <console.h>
#endif

/************************************************************************/
/* Define a Custom Services class.                                      */
/*                                                                      */
/* Combines the platform dependent functionality of                     */
/* ExSystemServices and ExHostAppServices                               */ 
/************************************************************************/
class MyServices : public ExSystemServices, public ExHostAppServices
{
protected:
	ODRX_USING_HEAP_OPERATORS(ExSystemServices);
};

/************************************************************************/
/* Define a our own Assert function.                                    */
/************************************************************************/
#include "diagnostics.h"

static void MyAssert(const char* expresssion, const char* fileName, int nLineNo)
{
	std::cout << "\nAssertion Failed: " << expresssion
		<< "\nfile: " << fileName << "\nline: " << nLineNo << "\n";
}

/************************************************************************/
/* Define a module map for statically linked modules                    */
/************************************************************************/
#if !defined(_TOOLKIT_IN_DLL_) || defined(__MWERKS__)

ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ModelerModule);
ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(ExRasterModule);
ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdRasterProcessingServicesImpl);
ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(OdOleItemHandlerModuleImpl);
ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(WinGDIModule);

ODRX_BEGIN_STATIC_MODULE_MAP()
ODRX_DEFINE_STATIC_APPLICATION(OdModelerGeometryModuleName,  ModelerModule)
ODRX_DEFINE_STATIC_APPLICATION(RX_RASTER_SERVICES_APPNAME, ExRasterModule)
ODRX_DEFINE_STATIC_APPLICATION(OdRasterProcessorModuleName,  OdRasterProcessingServicesImpl)
ODRX_DEFINE_STATIC_APPLICATION(OdOleItemHandlerModuleName, OdOleItemHandlerModuleImpl)
ODRX_DEFINE_STATIC_APPLICATION(OdWinGDIModuleName, WinGDIModule)
ODRX_END_STATIC_MODULE_MAP()

#endif

/************************************************************************/
/* This method computes exact extents of layout paper.                  */
/************************************************************************/
static OdGeExtents2d paperSpaceLayoutSize(OdDbDatabase *pDb)
{
	OdGeExtents2d extents;
	OdDbLayoutPtr pLt = OdDbLayout::cast(OdDbBlockTableRecord::cast(pDb->getActiveLayoutBTRId().openObject())->getLayoutId().openObject());
	if (!pLt.isNull())
	{
		double dPaperWidth, dPaperHeight;
		pLt->getPlotPaperSize(dPaperWidth, dPaperHeight);
		bool bStandartScale = pLt->useStandardScale();
		double dStdScale; pLt->getStdScale(dStdScale);
		double dRwdUnits, dDrawingUnits;
		pLt->getCustomPrintScale(dRwdUnits, dDrawingUnits);
		double dUnitsConv = (bStandartScale) ? dStdScale : (dRwdUnits / dDrawingUnits);
		dUnitsConv = OdNonZero(dUnitsConv) ? (1.0 / dUnitsConv) : 1.0;
		dPaperWidth *= dUnitsConv; dPaperHeight *= dUnitsConv;
		if (pLt->plotPaperUnits() == OdDbPlotSettings::kInches)
		{
			dPaperWidth /= kMmPerInch;
			dPaperHeight /= kMmPerInch;
		}
		switch (pLt->plotRotation())
		{
		case OdDbPlotSettings::k90degrees:
		case OdDbPlotSettings::k270degrees:
			std::swap(dPaperWidth, dPaperHeight);
			break;
		default: break;
		}
		OdGePoint2d origin = pLt->getPaperImageOrigin();
		OdGeVector2d size(dPaperWidth, dPaperHeight);
		extents.set(origin - size * 0.5, origin + size * 0.5);
	}
	return extents;
}
#define PIPE_M2CPARAMETER	_T("\\\\.\\pipe\\m2cParameter")
#define BUFSIZE 512
/************************************************************************/
/* Main                                                                 */
/************************************************************************/
#if defined(_UNICODE) && (defined(WIN32) || defined(WIN64)) && !defined(__MINGW32__)
int wmain(int argc, wchar_t* argv[])
#else
int main(int argc, char* argv[])
#endif
{
	int nRes = 1; // Return value for main

#if defined(TARGET_OS_MAC) && !defined(__MACH__)
	argc = ccommand(&argv);
#endif
	setlocale(LC_ALL,"");
	/**********************************************************************/
	/* Verify the argument count and display an error message as required */
	/**********************************************************************/
	//if (argc < 6) 
	//{
	//	odPrintConsoleString(L"usage: OdWmfExport <input file> <output file> <file type \"WMF\" or \"EMF\"> <xSize> <ySize> <disable background (optional)>\n");
	//	return 1;
	//}

#if !defined(_TOOLKIT_IN_DLL_) || defined(__MWERKS__)
	ODRX_INIT_STATIC_MODULE_MAP();
#endif
	odSetAssertFunc(MyAssert);

	// Create a custom Services object.
	OdStaticRxObject<MyServices> svcs;
	// initialize Teigha
	odInitialize(&svcs);
	// initialize GS subsystem
	//odgsInitialize(); // Not necessary for WinGDI module

	odPrintConsoleString(L"Developed using %ls ver %ls\n", svcs.product().c_str(), svcs.versionString().c_str());

	try 
	{
		//创建通知事件
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = FALSE;
		sa.lpSecurityDescriptor = NULL;
		HANDLE hConvert = OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("beginConvert"));
		if (hConvert == NULL)
		{
			odPrintConsoleString(L"Open beginConvert Event failed\n");
			return 0;
		}
		//打开命名管道
		HANDLE hPipe;
		//odPrintConsoleString(L"wait for named pipe.\n"); 
		//if ( ! WaitNamedPipe(PIPE_M2CPARAMETER, NMPWAIT_WAIT_FOREVER)) 
		//{ 
		//	odPrintConsoleString(L"Could not open pipe: 20 second wait timed out."); 
		//	return -1;
		//} 
		//odPrintConsoleString(L"get the waitable pipe.\n");

		while (1)
		{
			hPipe = CreateFile(PIPE_M2CPARAMETER, 
				GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hPipe != INVALID_HANDLE_VALUE)
			{
				odPrintConsoleString(L"the pipe is valid %d\n", GetLastError());
				break;
			}
			if (GetLastError() != ERROR_PIPE_BUSY) 
			{
				odPrintConsoleString( L"Could not open pipe. GLE=%d\n", GetLastError() ); 
				return -1;
			}
			//if ( ! WaitNamedPipe(PIPE_M2CPARAMETER, 10000)) 
			//{ 
			//	printf("Could not open pipe: 20 second wait timed out."); 
			//	return -1;
			//} 
		}
		DWORD dwMode = PIPE_READMODE_MESSAGE; 
		BOOL fSuccess = SetNamedPipeHandleState( 
			hPipe,    // pipe handle 
			&dwMode,  // new pipe mode 
			NULL,     // don't set maximum bytes 
			NULL);    // don't set maximum time 
		if (!fSuccess) 
		{
			odPrintConsoleString( TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError() ); 
			return -1;
		}
		TCHAR buf[BUFSIZE] = {0};
		while (1)
		{
			odPrintConsoleString(L"wait for convert event \n");
			//等待开始转换事件
			WaitForSingleObject(hConvert, INFINITE);
			ResetEvent(hConvert);
			//取得调用方写入数据
			odPrintConsoleString(L"wait for pipe\n");

			//if (!WaitNamedPipe(PIPE_M2CPARAMETER, 2000))
			//{
			//	odPrintConsoleString(L"wait event time out, continue %d\n", GetLastError());
			//	continue;
			//}
			BOOL bSuc = FALSE;
			DWORD dwRead = 0;
			//写入管道中数据，进行数据请求
			//取得管道中的数据
			ZeroMemory(buf, BUFSIZE * sizeof(TCHAR));
			odPrintConsoleString(L"get data from pipe\n");
			do 
			{
				bSuc = ReadFile(hPipe, buf, BUFSIZ*sizeof(TCHAR), &dwRead, NULL);
				if (!bSuc && GetLastError() != ERROR_MORE_DATA)
					break;
			} while (bSuc);
			//为参数赋值
			OdString wmfpath(buf);
			int pos = wmfpath.reverseFind('.');
			if (pos != -1)
			{
				wmfpath.setAt(pos+1, 'w');
				wmfpath.setAt(pos+2, 'm');
				wmfpath.setAt(pos+3, 'f');
			}
			printf("wmf address is : %s\n", wmfpath.c_str());
			/******************************************************************/
			/* Create a database and load the drawing into it.                */
			/*                                                                */
			/* Specified arguments are as followed:                           */
			/*     filename, allowCPConversion, partialLoad, openMode         */
			/******************************************************************/
			OdDbDatabasePtr pDb;
			OdString f(buf/*argv[1]*/);//!<刘国元修改
			pDb = svcs.readFile( f.c_str(), false, false, Oda::kShareDenyNo ); 

			if (!pDb.isNull())
			{
				/****************************************************************/
				/* Create the vectorization context.                            */
				/* This class defines the operations and properties that are    */
				/* used in the Teigha vectorization of an OdDbDatabase.         */
				/****************************************************************/
				OdGiContextForDbDatabasePtr pDwgContext = OdGiContextForDbDatabase::createObject();

				/****************************************************************/
				/* Create the WinGDI rendering device, and set the output       */
				/* stream for the device.                                       */
				/****************************************************************/
				OdGsModulePtr pGs = ::odrxDynamicLinker()->loadModule(OdWinGDIModuleName);
				OdGsDevicePtr pDevice = pGs->createDevice();

				/****************************************************************/
				/* Create metafile context and set it to vectorizer.            */
				/****************************************************************/
				HDC hDC = NULL;
				//!<刘国元修改
				//if (OdString(argv[3]).compare(OD_T("EMF")) == 0)
				//{
					// Enhanced windows MetaFile (EMF)
					hDC = ::CreateEnhMetaFile(NULL, OdString(wmfpath/*argv[2]*/).c_str(), NULL, OD_T("Metafile"));
				//}
				//else
				//{
				//	// Windows MetaFile (WMF)
				//	hDC = ::CreateMetaFile(OdString(argv[2]).c_str());
				//}
				OdRxDictionaryPtr pProperties = pDevice->properties();
				pProperties->putAt(OD_T("WindowHDC"), OdRxVariantValue((OdIntPtr)hDC));
				/****************************************************************/
				/* Disable background output.                                   */
				/****************************************************************/
				//if (argc > 6)//!<刘国元修改
				{
					pProperties->putAt(OD_T("ClearScreen"), OdRxVariantValue((wcstol(argv[6], NULL, 10) == 0) ? false : true));
				}

				/****************************************************************/
				/* Set the database to be vectorized.                           */
				/****************************************************************/
				pDwgContext->setDatabase(pDb);

				/****************************************************************/
				/* Prepare the device to render the active layout in            */
				/* this database.                                               */
				/****************************************************************/
				pDevice = OdDbGsManager::setupActiveLayoutViews(pDevice, pDwgContext);

				/****************************************************************/
				/* Setup background.                                            */
				/****************************************************************/
				if (pDb->getTILEMODE())
				{
					// Use black background for ModelSpace
					pDevice->setLogicalPalette(::odcmAcadDarkPalette(), 256);
					pDevice->setBackgroundColor(ODRGB(0, 0, 0));
					pDwgContext->setPaletteBackground(ODRGB(0, 0, 0));
				}
				else
				{
					// Use white paper and grey background for ModelSpace
					pDevice->setLogicalPalette(::odcmAcadLightPalette(), 256);
					pDevice->setBackgroundColor(ODRGB(168, 168, 168));
					pDwgContext->setPaletteBackground(ODRGB(255, 255, 255));
				}

				/****************************************************************/
				/* Set output screen rectangle.                                 */
				/****************************************************************/
				OdGsDCRect screenRect(OdGsDCPoint(0, 768/*wcstol(argv[5], NULL, 10)*/), OdGsDCPoint(1366/*wcstol(argv[4], NULL, 10)*/, 0));
				pDevice->onSize(screenRect);

				/****************************************************************/
				/* Zoom to extents.                                             */
				/****************************************************************/
				OdGeExtents2d extents;
				if (!pDb->getTILEMODE())
					extents = paperSpaceLayoutSize(pDb);
				if (extents.isValidExtents())
				{
					OdGeBoundBlock3d ext3d;
					ext3d.set(OdGePoint3d(extents.minPoint().x, extents.minPoint().y, 0.0),
						OdGePoint3d(extents.maxPoint().x, extents.maxPoint().y, 0.0));
					OdAbstractViewPEPtr(pDevice->viewAt(0))->zoomExtents(pDevice->viewAt(0), &ext3d);
				}
				else
				{
					OdAbstractViewPEPtr(pDevice->viewAt(0))->zoomExtents(pDevice->viewAt(0));
				}

				/****************************************************************/
				/* Initiate Vectorization                                       */
				/****************************************************************/
				pDevice->update();

				/****************************************************************/
				/* Destroy metafile.                                            */
				/****************************************************************/
				int type1 = ::GetDeviceCaps(hDC, TECHNOLOGY);
				DWORD type2 = ::GetObjectType(hDC);
				if (type1 != DT_METAFILE && type2 == OBJ_ENHMETADC)
				{
					// EMF
					::DeleteEnhMetaFile(::CloseEnhMetaFile(hDC));
				}
				else
				{
					// WMF
					::DeleteMetaFile(::CloseMetaFile(hDC));
				}
			}
			odPrintConsoleString(L"\nDone.\n");
			nRes = 0;
		}
		CloseHandle(hConvert);
	}

	/********************************************************************/
	/* Display the error                                                */
	/********************************************************************/
	catch (const OdError& e)
	{
		odPrintConsoleString(L"\nException Caught: %ls\n", e.description().c_str());
	}
	catch (...)
	{
		odPrintConsoleString(L"\nUnknown Exception Caught\n");
	}
	//odgsUninitialize(); // Not necessary for WinGDI
	odUninitialize();

	if (!nRes)
	{
		odPrintConsoleString(L"\nOdWmfExport Finished Successfully");
	}
	return nRes;
}

//
