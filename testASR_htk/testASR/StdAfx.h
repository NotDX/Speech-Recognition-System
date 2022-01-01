// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__47375C13_DB84_4F03_9C37_0D9526E61607__INCLUDED_)
#define AFX_STDAFX_H__47375C13_DB84_4F03_9C37_0D9526E61607__INCLUDED_
 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _AFXDLL

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxpriv2.h>
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <comdef.h>//初始化一下com口
//#ifndef ULONG_PTR
//#define ULONG_PTR unsigned long*
//#include "GdiPlus.h"
//using namespace Gdiplus;
//#endif
//#pragma comment(lib,"gdiplus.lib")
//
//
////#include <mmsystem.h>       //多媒体支持
////#pragma comment(lib,"WINMM.LIB")//多媒体支持
//#pragma comment(lib,"lib\\MPR.LIB")

//#pragma comment(lib,"lib\\libVPR.lib") 


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__47375C13_DB84_4F03_9C37_0D9526E61607__INCLUDED_)
