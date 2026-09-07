// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__14EEFDC6_7CC9_4ED7_B45D_26DC96EECD38__INCLUDED_)
#define AFX_STDAFX_H__14EEFDC6_7CC9_4ED7_B45D_26DC96EECD38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if _MSC_VER < 1300 // vc6:120X
#   define _WIN32_WINNT 0x0500
#   define _WIN32_IE    0x0600
#   define WINVER 0x050a
#else
#   define _WIN32_WINNT 0x0501
#   define _WIN32_IE    0x0600
#   define WINVER 0x050a
#endif


#include <atlbase.h>

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module


#pragma warning(push)
#pragma warning(disable : 4996) // warning C4996: 'GetVersionExW': was declared deprecated
#include <atlapp.h>
#pragma warning(pop)

extern CComModule _Module;

#include <atlcom.h>
#include <comdef.h>


#include "tmlist.h"
#include "tmstring.h"
#include "tmduiacc.h"
#include "tmcom_win.h"
#include "tmformatguid.h"
#include "tmplatform_win.h"
#include "tmgdix.h"
#include "tmkeyboard.h"
#include "tmani.h"

#ifndef DEFINE_ENUM_FLAG_OPERATORS
#   define DEFINE_ENUM_FLAG_OPERATORS(x) TM_ENUM_2_FLAG(x)
#endif

#include "tmformatacc.h"
#include "tmformatmsg.h"
#include "tmformatguid.h"


#ifndef STATE_SYSTEM_PROTECTED
#define    STATE_SYSTEM_PROTECTED    (0x20000000)
#endif

using TM::HTUI;
typedef TM::CTuiWidget CAccessibleWnd;
typedef TMUInt32 AccHash32;

typedef TM::CMap<TM::CString, int> CUIElmentUsageList;
typedef CUIElmentUsageList::recorder CElmentUsageRec;
// TODO: reference additional headers your program requires here


#define ACC_TRACE DbgTrace
//#define ACC_TRACE DbgTraceNull

//#define ACC_HRTRACE(hr) if (FAILED(hr)) DbgTrace
#define ACC_HRTRACE(hr) DbgTrace
//#define ACC_HRTRACE(hr) DbgTraceNull

TM::CString _acc_dinfo(CAccessibleWnd& wnd);
TM::CString _acc_dinfo2(HTUI hWnd);
// #define _DINF(h) _acc_dinfo2(h).c_str()
#define _DINFO() _acc_dinfo(GetOwner()).c_str()


#define FUNC_CHECK_(p,func,ret,wgt) \
    if (!p) \
    { \
        ACC_TRACE(TM_T(func) TM_T("() return ") TM_T(#ret) TM_T("; %s"), _acc_dinfo(wgt).c_str()); \
        return ret; \
    }



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__14EEFDC6_7CC9_4ED7_B45D_26DC96EECD38__INCLUDED_)
