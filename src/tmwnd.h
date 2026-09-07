/*
 *  @file
 *  @brief  windows
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2004.08.24
 *
 *  Copyright (C) 2004 miragekiller
 */

#pragma once

#ifndef TMWND_H
#define TMWND_H

#include "tmdef.h"

#ifdef TM_WIN

#include "tmgdi.h"
#include "tmlist.h"
#include "tmrange.h"
#include "tmplatform_win.h"
#include "tmtime.h"

#include <Shlobj.h>
#include <shellapi.h>
#include <commdlg.h>

#pragma warning (push)

// class 'xxx' needs to have dll-interface to be used by clients of class 'yyy'
#pragma warning (disable : 4251)
// non dll-interface class 'xxx' used as base for dll-interface class 'yyy'
#pragma warning (disable : 4275)


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// class CTrayIcon
class CTrayIcon
{
protected:
    NOTIFYICONDATA    m_tnd;
    HICON           m_hIcon;

public:
    CTrayIcon() : m_hIcon (0)
    {
        MemZeroT(m_tnd);
        m_tnd.cbSize = sizeof(NOTIFYICONDATA);
    }

    ~CTrayIcon ()
    {
        RemoveIcon ();
    }


public:
    //Create the tray icon
    BOOL Create (HWND hWnd, UINT uCallbackMessage, LPCTSTR szToolTip,
        HICON icon, UINT uID)
    {
        DbgAssert(StrLen(szToolTip, 70) <= 64);
        DbgAssert(!m_tnd.hWnd);
        DbgAssert(!m_tnd.uID);
        DbgAssert(!m_tnd.hIcon);
        DbgAssert(!m_tnd.uFlags);

        // load up the NOTIFYICONDATA structure
        m_tnd.hWnd     = hWnd;
        m_tnd.uID     = uID;
        m_tnd.hIcon  = icon;
        m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        m_tnd.uCallbackMessage = uCallbackMessage;
        StrCopy (m_tnd.szTip, 64, szToolTip);
        //    _tcscpy_s (m_tnd.szTip, _countof(m_tnd.szTip), szToolTip);

        m_hIcon = icon;

        // Set the tray icon
        return Shell_NotifyIcon (NIM_ADD, &m_tnd);
    }

    //Change or retrieve the Tooltip text
    BOOL SetTooltipText (LPCTSTR pszTip)
    {
        DbgAssert(StrLen(pszTip, 70) <= 64);

        m_tnd.uFlags = NIF_TIP;
        StrCopy(m_tnd.szTip, 64, pszTip);
        //    _tcscpy_s(m_tnd.szTip, _countof(m_tnd.szTip), pszTip);

        return Shell_NotifyIcon (NIM_MODIFY, &m_tnd);
    }

    LPCTSTR GetTooltipText() const { return m_tnd.szTip; }

    //Change or retrieve the icon displayed
    BOOL SetIcon (HICON hIcon)
    {
        m_tnd.uFlags = NIF_ICON;
        m_tnd.hIcon = hIcon;

        m_hIcon = hIcon;

        return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
    }

    HICON GetIcon() const { return m_tnd.hIcon; }

    void HideIcon()
    {
        m_tnd.uFlags = NIF_ICON;
        Shell_NotifyIcon (NIM_DELETE, &m_tnd);
    }

    void ShowIcon()
    {
        m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        Shell_NotifyIcon(NIM_ADD, &m_tnd);
    }


    void RemoveIcon()
    {
        m_tnd.uFlags = 0;
        Shell_NotifyIcon (NIM_DELETE, &m_tnd);
    }

    void MoveToRight()
    {
        HideIcon();
        ShowIcon();
    }

    void SetIconAnimate (HICON* pFirst, HICON* pLast, UINT nOverTime);
    void StopIconAnimate ();

    //Change or retrieve the window to send notification messages to
    BOOL SetNotificationWnd (HWND hWnd)
    {
        m_tnd.hWnd = hWnd;
        m_tnd.uFlags = 0;
        return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
    }

    HWND GetNotificationWnd() const
    {
        return m_tnd.hWnd;
    }
};


////////////////////////////////////////////////////////////////////////////////
// class CPathDialog
class CPathDialog
{
private:
    struct BrowseParam
    {
        BOOL bString;
        LPARAM lpSel;
    };

    static int __stdcall _BrowseProc (HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
    {
        BrowseParam* p = (BrowseParam*) lpData;
        if (uMsg == BFFM_INITIALIZED)
            SendMessage (hwnd, BFFM_SETSELECTION, p->bString, p->lpSel);
        return 0;
    }

    static inline void _BrowseFolder (CPathDialog& idl,
        HWND hOwner, LPCTSTR pTitle, const void* pInit, BOOL bTxt,
        UINT uFlag, LPCITEMIDLIST pRoot)
    {
        BrowseParam bp = { bTxt, (LPARAM) pInit };

        BROWSEINFO bi = { 0 };
        bi.hwndOwner = hOwner;
        bi.ulFlags = uFlag;
        bi.lpszTitle = pTitle;
        bi.pidlRoot = pRoot;
        if (pInit)
        {
            bi.lpfn = _BrowseProc;
            bi.lParam = reinterpret_cast < LPARAM > (&bp);
        }

        CPathDialog(SHBrowseForFolder(&bi)).Swap (idl) ;
    }

private:
    CPathDialog (const CPathDialog&) {}
    CPathDialog& operator = (const CPathDialog&) { return *this; }

protected:
    LPITEMIDLIST m_idl;

public:
    CPathDialog (LPITEMIDLIST idl = 0) : m_idl (idl) {}
    ~CPathDialog () { Free (); }

    void Free ()
    {
        if (m_idl)
        {
            IMalloc *pMalloc = NULL;
            if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
            {
                pMalloc->Free(m_idl);
                pMalloc->Release();
            }
            m_idl = 0;
        }
    }

    void Swap (CPathDialog& rhs) { tm_swap (m_idl, rhs.m_idl); }

    operator LPITEMIDLIST () const { return m_idl; }

    void DoModal (HWND hOwner, LPCTSTR pTitle,
        LPCITEMIDLIST pInitDir = 0, UINT uFlag = BIF_RETURNONLYFSDIRS,
        LPCITEMIDLIST pRoot = 0)
    {
        _BrowseFolder (*this, hOwner, pTitle, pInitDir, false, uFlag, pRoot);
    }

    void DoModal (HWND hOwner, LPCTSTR pTitle,
        LPCTSTR pInitDir, UINT uFlag = BIF_RETURNONLYFSDIRS,
        LPCITEMIDLIST pRoot = 0)
    {
        _BrowseFolder (*this, hOwner, pTitle, pInitDir, true, uFlag, pRoot);
    }

    BOOL GetPath (LPTSTR pBuf)
    {
        return SHGetPathFromIDList (m_idl, pBuf);
    }

#ifdef TMSTRING_H
    BOOL GetPath (CString& __str)
    {
        BOOL b = SHGetPathFromIDList (m_idl, __str.GetBuffer(MAX_PATH));
        __str.ReleaseBuffer();
        return b;
    }
#endif
};


////////////////////////////////////////////////////////////////////////////////
// class CFileDialog
class CFileDialog
{
private:
    enum { eMaxPath = MAX_PATH };

    TCHAR   m_szPath[eMaxPath]; // contains full path name after return
    WORD    m_nName;
    WORD    m_nExt;

protected:
    template < class T >
        inline BOOL _DoModal (HWND hOwner, LPCTSTR pFilter,
        LPCTSTR pWndTitle, LPCTSTR pInitDir, LPCTSTR pInitFile, LPCTSTR lpszDefExt, DWORD dwFlags, T _func)
    {
        if (pInitFile)
            StrCopy(m_szPath, eMaxPath, pInitFile);
        else
            m_szPath[0] = 0;
        m_nName = m_nExt = 0;

        OPENFILENAME ofn = { sizeof(OPENFILENAME), 0 };

        ofn.hwndOwner       = hOwner;
        //        ofn.hInstance         = 0; // Not supported
        ofn.lpstrFilter     = pFilter;
        //        ofn.lpstrCustomFilter = 0; // Not supported
        //        ofn.nMaxCustFilter    = 0; // Not supported
        //        ofn.nFilterIndex      = 0;
        ofn.lpstrFile       = m_szPath;
        ofn.nMaxFile        = eMaxPath;
        //        ofn.lpstrFileTitle  = m_szFileTitle;
        //        ofn.nMaxFileTitle   = sizeof(m_szFileTitle)/sizeof(TCHAR);
        ofn.lpstrInitialDir = pInitDir;
        ofn.lpstrTitle      = pWndTitle;
        //        ofn.nFileOffset       = 0;
        //        ofn.nFileExtension    = 0;
       ofn.lpstrDefExt       = lpszDefExt;//"*.x";
        //        ofn.lCustData         = 0;

        ofn.Flags           = dwFlags;

        if (_func (&ofn))
        {
            m_nName = ofn.nFileOffset;
            m_nExt = ofn.nFileExtension;
            return true;
        }

        return false;
    }


public:
    // return full path and filename
    LPCTSTR GetPathName() const { return m_szPath; }

    // return only filename
    LPCTSTR GetFileName() const { DbgAssert(m_nName<eMaxPath); return &(m_szPath[m_nName]); }

    // return only ext
    LPCTSTR GetFileExt() const { DbgAssert(m_nName<eMaxPath);  return &(m_szPath[m_nExt]); }

    // get path name,exclude filename
    std::_tm_string_view<TMCharT> GetFilePath() const { return StrMakeView(m_szPath, m_nName); }
};

//////////////////////////////////////////////////////////////////////////////
//     CFileOpenDialog fo;
//     if (!fo.DoModal(GetHWND(), TM_T("csv files\0*.csv\0all files\0*.*\0")))
//          return false;
//     x.OpenSCV(fo.GetPathName());

class CFileOpenDialog : public CFileDialog
{
public:
    // sample of pFilter:
    // "all files\0*.*\0cpp files\0*.cpp\0"
    BOOL DoModal (HWND hOwner, LPCTSTR pFilter,
        LPCTSTR pWndTitle, LPCTSTR pInitDir, LPCTSTR pInitFile,
        LPCTSTR lpszDefExt,
        DWORD dwFlags = OFN_SHOWHELP | OFN_PATHMUSTEXIST |
        OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR)
    {
            return _DoModal (hOwner, pFilter, pWndTitle, pInitDir, pInitFile, lpszDefExt, dwFlags, GetOpenFileName);
    }

    BOOL DoModal (HWND hOwner, LPCTSTR pFilter = 0,
        LPCTSTR pWndTitle = 0, LPCTSTR pInitDir = 0, LPCTSTR pInitFile = 0,
        DWORD dwFlags = OFN_SHOWHELP | OFN_PATHMUSTEXIST |
        OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR)
    {
        return _DoModal (hOwner, pFilter, pWndTitle, pInitDir, pInitFile, 0, dwFlags, GetOpenFileName);
    }
};

class CFileSaveDialog : public CFileDialog
{
public:
    // sample of pFilter:
    // "all files\0*.*\0cpp files\0*.cpp\0"
    BOOL DoModal (HWND hOwner, LPCTSTR pFilter,
        LPCTSTR pWndTitle, LPCTSTR pInitDir, LPCTSTR pInitFile,
        LPCTSTR lpszDefExt,
        DWORD dwFlags = OFN_SHOWHELP | OFN_PATHMUSTEXIST |
        OFN_NOCHANGEDIR)
    {
        return _DoModal (hOwner, pFilter, pWndTitle, pInitDir, pInitFile, lpszDefExt, dwFlags, GetSaveFileName);
    }

    BOOL DoModal (HWND hOwner, LPCTSTR pFilter = 0,
        LPCTSTR pWndTitle = 0, LPCTSTR pInitDir = 0, LPCTSTR pInitFile = 0,
        DWORD dwFlags = OFN_SHOWHELP | OFN_PATHMUSTEXIST |
        OFN_NOCHANGEDIR)
    {
        return _DoModal (hOwner, pFilter, pWndTitle, pInitDir, pInitFile, 0, dwFlags, GetSaveFileName);
    }
};


////////////////////////////////////////////////////////////////////////////////
// class CColorDlg
// color select dialog
class CColorDialog
{
private:
    COLORREF m_clr;

public:
    TMBool DoModal (HWND hWnd, COLORREF clrInit = 0)
    {
        static COLORREF rgbCustom[16] =
        {
            RGB(255, 255, 255), RGB(239, 239, 239),
            RGB(223, 223, 223), RGB(207, 207, 207),
            RGB(191, 191, 191), RGB(175, 175, 175),
            RGB(159, 159, 159), RGB(143, 143, 143),
            RGB(127, 127, 127), RGB(111, 111, 111),
            RGB(95, 95, 95),    RGB(79, 79, 79),
            RGB(63, 63, 63),    RGB(47, 47, 47),
            RGB(31, 31, 31),    RGB(15, 15, 15)
        };
        CHOOSECOLOR csl = { sizeof (CHOOSECOLOR), hWnd };
        csl.Flags = CC_ANYCOLOR | CC_RGBINIT | CC_FULLOPEN;
        csl.rgbResult = clrInit;
        csl.lpCustColors = rgbCustom;
        if (ChooseColor(&csl))
        {
            m_clr = csl.rgbResult;
            return true;
        }
        else
        {
            m_clr = clrInit;
            return false;
        }
    }

    COLORREF GetColor () const { return m_clr; }
};

class CFontDialog
{
public:
    enum { _cchStyleName = 64 };

    CHOOSEFONT m_cf;
    TCHAR m_szStyleName[_cchStyleName];  // contains style name after return

// Constructors
    CFontDialog(LPLOGFONT lplfInitial = NULL,
            DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
            HDC hDCPrinter = NULL,
            HWND hWndParent = NULL)
    {
        MemZeroT(m_cf);
        MemZeroT(m_szStyleName);

        m_cf.lStructSize = sizeof(m_cf);
        m_cf.hwndOwner = hWndParent;
        m_cf.rgbColors = RGB(0, 0, 0);
        m_cf.lpszStyle = (LPTSTR)&m_szStyleName;
        m_cf.Flags = dwFlags;// | CF_ENABLEHOOK;
        m_cf.lpfnHook = 0;//(LPCFHOOKPROC)T::HookProc;

        m_cf.lpLogFont = lplfInitial;
        m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;

        if (hDCPrinter != NULL)
        {
            m_cf.hDC = hDCPrinter;
            m_cf.Flags |= CF_PRINTERFONTS;
        }
    }

// Operations
    INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
    {
        if (m_cf.hwndOwner == NULL)          // set only if not specified before
            m_cf.hwndOwner = hWndParent;

        BOOL bRet = ::ChooseFont(&m_cf);

        return bRet ? IDOK : IDCANCEL;
    }

    void SetFlags(DWORD dwFlags)
    {
        m_cf.Flags = dwFlags;
    }

    // Helpers for parsing information after successful return
    LPCTSTR GetFaceName() const   // return the face name of the font
    {
        return (LPCTSTR)m_cf.lpLogFont->lfFaceName;
    }

    LPCTSTR GetStyleName() const  // return the style name of the font
    {
        return m_cf.lpszStyle;
    }

    int GetSize() const           // return the pt size of the font
    {
        return m_cf.iPointSize;
    }

    COLORREF GetColor() const     // return the color of the font
    {
        return m_cf.rgbColors;
    }

    int GetWeight() const         // return the chosen font weight
    {
        return (int)m_cf.lpLogFont->lfWeight;
    }

    BOOL IsStrikeOut() const      // return TRUE if strikeout
    {
        return (m_cf.lpLogFont->lfStrikeOut) ? TRUE : FALSE;
    }

    BOOL IsUnderline() const      // return TRUE if underline
    {
        return (m_cf.lpLogFont->lfUnderline) ? TRUE : FALSE;
    }

    BOOL IsBold() const           // return TRUE if bold font
    {
        return (m_cf.lpLogFont->lfWeight == FW_BOLD) ? TRUE : FALSE;
    }

    BOOL IsItalic() const         // return TRUE if italic font
    {
        return m_cf.lpLogFont->lfItalic ? TRUE : FALSE;
    }
};

inline void ClientToWindow (HWND hWnd, RECT& rc)
{
    RECT rc1, rc2;
    GetWindowRect (hWnd, &rc1);
    GetClientRect (hWnd, &rc2);
    ClientToScreen (hWnd, (LPPOINT)&rc2);
    ClientToScreen (hWnd, ((LPPOINT)&rc2)+1);
    rc.left -= (rc1.left - rc2.left);
    rc.top -= (rc1.top - rc2.top);
    rc.right -= (rc1.right - rc2.right);
    rc.bottom -= (rc1.bottom - rc2.bottom);
}

inline void WindowToClient (HWND hWnd, RECT& rc)
{
    RECT rc1, rc2;
    GetWindowRect (hWnd, &rc1);
    GetClientRect (hWnd, &rc2);
    ClientToScreen (hWnd, (LPPOINT)&rc2);
    ClientToScreen (hWnd, ((LPPOINT)&rc2)+1);
    rc.left += (rc1.left - rc2.left);
    rc.top += (rc1.top - rc2.top);
    rc.right += (rc1.right - rc2.right);
    rc.bottom += (rc1.bottom - rc2.bottom);
}

inline void GetTrueClientRect (HWND hWnd, RECT& rc)
{
    RECT rc1;
    GetWindowRect (hWnd, &rc1);
    GetClientRect (hWnd, &rc);
    POINT pt = { 0 };
    ClientToScreen (hWnd, &pt);
    int dx = pt.x - rc1.left;
    int dy = pt.y - rc1.top;
    rc.left += dx;
    rc.top += dy;
    rc.right += dx;
    rc.bottom += dy;
}

inline HINSTANCE OpenFolderAndSelectFile(LPCTSTR name)
{
    TCHAR c[1024];
    StrPrintf(c, 1023, TM_T("/select, %s"), name);
    return ::ShellExecute(NULL, TM_T("open"), TM_T("explorer.exe"), c, NULL, SW_SHOWNORMAL);
}

inline void MsgSleep(int nMS)
{
    TM::CCycle clk;
    for (int n = clk.ElapseMS(); n < nMS;)
    {
        if (WAIT_TIMEOUT == MsgWaitForMultipleObjects(0, 0, false, nMS - n, QS_ALLEVENTS))
            return;
        MSG msg;
        while(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                PostQuitMessage(msg.wParam);
                return;
            }
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

#pragma warning (pop)




TM_END_NAMESPACE


#endif // #ifdef TM_WIN
#endif // #define TMWND_H
