/*
 *  @file
 *  @brief  ATL for all platform support
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.8.6
 *
 *  Copyright (C) 2019 miragekiller
 */

#pragma once

#ifndef TMATL_WIN_H
#define TMATL_WIN_H

#ifndef TMATL_H
#   error the file must include from "tmatl.h"
#endif

#include "tmdll.h"
#include "tmplatform_win.h"
#include "tmgdi_win.h"
#include "tmthunk.h"
#include "tmatomic.h"


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// CWindowWin
class CWindowWin : public CWindow
{
public:
    typedef CWindowWin      class_type;

    CWindowWin(HWND hWnd = 0)
        : CWindow(hWnd)
    {
    }

    TMBool IsWindow() const
    {
        return ::IsWindow(m_hWnd);
    }

    static ATOM NativeRegister(
        LPCTSTR lpszClassName,
        UINT    style = CS_DBLCLKS | CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW,
        HBRUSH  hbrBackground = 0, //(HBRUSH)GetStockObject(BLACK_BRUSH),// (HBRUSH)(COLOR_WINDOWTEXT + 1),
        HCURSOR hCursor = ::LoadCursor(0, IDC_ARROW),
        int     cbWndExtra = 0,  
        HICON   hIcon = 0,
        HICON   hIconSm = 0)
    {
        WNDCLASSEX  wc = 
        {
            sizeof (WNDCLASSEX),
            style,
            ::DefWindowProc,
            0,
            cbWndExtra,
            0,
            hIcon,
            hCursor,
            hbrBackground,
            0,
            lpszClassName,
            hIconSm,
        };
        return RegisterClassEx(&wc); 
    }

    static HWND NativeCreate(
        LPCTSTR lpClassName, HWND hWndParent, LPCTSTR szWindowName = NULL,
        DWORD dwStyle = 0, DWORD dwExStyle = 0,
        UINT nID = 0, LPVOID lpCreateParam = NULL)
    {
        return ::CreateWindowEx(dwExStyle, lpClassName, szWindowName, dwStyle, 0, 0, 0, 0, hWndParent, (HMENU)nID, 0, lpCreateParam);
    }

    static HWND NativeCreate1(
        LPCTSTR lpClassName, HWND hWndParent, WNDPROC proc, LPCTSTR szWindowName = NULL,
        DWORD dwStyle = 0, DWORD dwExStyle = 0,
        UINT nID = 0, LPVOID lpCreateParam = NULL)
    {
        HWND hWnd = NativeCreate(lpClassName, hWndParent, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
        ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (ULONG_PTR)proc);
        return hWnd;
    }
    
    static HWND NativeCreate2(
        LPCTSTR lpClassName, HWND hWndParent, WNDPROC proc, void* userdata, LPCTSTR szWindowName = NULL,
        DWORD dwStyle = 0, DWORD dwExStyle = 0,
        UINT nID = 0, LPVOID lpCreateParam = NULL)
    {
        HWND hWnd = NativeCreate1(lpClassName, hWndParent, proc, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (ULONG_PTR)userdata);
        return hWnd;
    }

    void* GetUserData()
    {
        DbgAssert(IsWindow());
        return (void*)::GetWindowLongPtr(m_hWnd, GWLP_USERDATA);
    }

    void SetStyle(DWORD dwStyle)
    {
        DbgAssert(IsWindow());
        ::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
    }

    void SetExStyle(DWORD dwStyle)
    {
        DbgAssert(IsWindow());
        ::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwStyle);
    }

    void Move(int x, int y)
    {
        SetWindowPos(0, x, y, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
    }

    void Resize(int w, int h)
    {
        SetWindowPos(0, 0, 0, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
    }

    void ChangeFrame()
    {
        SetWindowPos(0, 0, 0, 0, 0,
            SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    }

    void ShowMinimized()
    {
        ShowWindow(SW_SHOWMINIMIZED);
    }

    void ShowMaximized()
    {
        ShowWindow(SW_SHOWMINIMIZED);
    }

    void ShowNormal()
    {
        ShowWindow(SW_SHOWNORMAL);
    }

    BOOL SetWindowSubclass(SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SetWindowSubclass(m_hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    BOOL RemoveWindowSubclass(SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::RemoveWindowSubclass (m_hWnd, pfnSubclass, uIdSubclass);
    }

    LRESULT DefSubclassProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::DefSubclassProc(m_hWnd, uMsg, wParam, lParam);
    }

    BOOL CreateUnunsedCaret(int nWidth, int nHeight) throw()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::CreateCaret(m_hWnd, (HBITMAP)2, nWidth, nHeight);
    }

    static LRESULT CALLBACK SubclassProcDisableDPIChangeResize(
        HWND hWnd, UINT uMsg, WPARAM wParam,
        LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
    {
        CWindowWin wnd(hWnd);
        if (uMsg == WM_DPICHANGED)
        {
            RECT rc;
            ::GetWindowRect(hWnd, &rc);
            lParam = (LPARAM)&rc;
            return wnd.DefSubclassProc(uMsg, wParam, lParam);
        }
        return wnd.DefSubclassProc(uMsg, wParam, lParam);
    }

    void DisableDPIChangeResize()
    {
        DbgVerify(SetWindowSubclass(SubclassProcDisableDPIChangeResize, 0, 0));
    }

    void EnableDPIChangeResize()
    {
        DbgVerify(RemoveWindowSubclass(SubclassProcDisableDPIChangeResize, 0));
    }

    TMBool IsSelfVisible() const
    {
        return GetStyle() & WS_VISIBLE;
    }

    TMBool IsActiveWindow() const
    {
        DbgAssert(IsWindow());
        return ::GetActiveWindow() == m_hWnd;
    }

    void SetHideFocus()
    {
        DbgAssert(IsWindow());
        SendMessage(WM_CHANGEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);
    }

    void ResetHideFocus()
    {
        DbgAssert(IsWindow());
        SendMessage(WM_CHANGEUISTATE, MAKEWPARAM(UIS_CLEAR, UISF_HIDEFOCUS), 0);
    }

    TMBool IsFocus()
    {
        return m_hWnd == ::GetFocus();
    }

    void ScreenToParent(TMRectNative& rc)
    {
        DbgAssert(IsWindow());
        HWND hParent;
        if ((WS_CHILD & ::GetWindowLong(m_hWnd, GWL_STYLE))
            && (hParent = (HWND)::GetWindowLongPtr(m_hWnd, GWLP_HWNDPARENT)))
        {
            DbgVerify(CWindow(hParent).ScreenToClient(&rc));
            ((CRectNative&)rc).NormalizeRect();
        }
    }

    void GetWindowRectToParent(TMRectNative& rc)
    {
        DbgAssert(IsWindow());
        DbgVerify(::GetWindowRect(m_hWnd, &rc));
        ScreenToParent(rc);
    }

    void CenterWindow2(TMBool bCenterScreen, TMBool bCenterOwner)
    {
        _Monitor_CenterWindow(((CMonitorWin*)0), *this, bCenterScreen, bCenterOwner);
    }

    void EnableAreo(int nClient)
    {
        DbgAssert(IsWindow());
        DbgAssert(_DwmExtendFrameIntoClientArea);
//*
        MARGINS margins = { nClient, nClient, nClient, nClient };// {10,10,10,10};
        DbgVerifyHResult(_DwmExtendFrameIntoClientArea(*this, &margins));
/*/
        DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
        SetWindowAttribute(m_hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &ncrp, sizeof(ncrp));
//*/
    }

    TMBool EnableBlur(ACCENT_STATE state)
    {
        DbgAssert(IsWindow());

        if (_SetWindowCompositionAttribute)
        {
            ACCENT_POLICY accent = { state, 0, 0, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data;
            data.dwAttrib = WCA_ACCENT_POLICY;
            data.pvData = &accent;
            data.cbData = sizeof(accent);
            if (_SetWindowCompositionAttribute(*this, &data))
                return true;
        }

        DWM_BLURBEHIND bb = {0};
        // Enable Blur Behind and apply to the entire client area
        bb.dwFlags = DWM_BB_ENABLE;
        if (state == ACCENT_ENABLE_BLURBEHIND) 
            bb.fEnable = true;
        else if (state == ACCENT_ENABLE_GRADIENT)
            bb.fEnable = false;
        else
            return false;
//        bb.hRgnBlur = NULL;
        DbgAssert(_DwmEnableBlurBehindWindow);
        HRESULT hr;
        DbgVerifyHResult(hr = _DwmEnableBlurBehindWindow(*this, &bb));
        return S_OK != hr;
    }

    TMBool SetWindowAttribute(DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
    {
        DbgAssert(IsWindow());
        if (!_DwmSetWindowAttribute)
            return false;
        TMResult hr;
        DbgVerifyHResult(hr = _DwmSetWindowAttribute(m_hWnd, dwAttribute, pvAttribute, cbAttribute));
        return hr == S_OK;
    }

    
    TMBool SetWindowAttribute_NCRENDERING_POLICY(__DWMNCRENDERINGPOLICY policy)
    {
        return SetWindowAttribute(_DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));
    }
    
    TMBool SetWindowAttribute_ALLOW_NCPAINT(TMBool b)
    {
        return SetWindowAttribute(_DWMWA_ALLOW_NCPAINT, &b, sizeof(b));
    }

    TMBool SetWindowAttribute_CLOAKED(TMBool b)
    {
        return SetWindowAttribute(_DWMWA_CLOAKED, &b, sizeof(b));
    }

    TMBool SetWindowAttribute_USE_HOSTBACKDROPBRUSH(TMBool b)
    {
        return SetWindowAttribute(_DWMWA_USE_HOSTBACKDROPBRUSH, &b, sizeof(b));
    }

    TMBool SetWindowAttribute_SYSTEMBACKDROP_TYPE(__DWM_SYSTEMBACKDROP_TYPE type)
    {
        return SetWindowAttribute(_DWMWA_SYSTEMBACKDROP_TYPE, &type, sizeof(type));
    }

    TMBool SetWindowAttribute_USE_IMMERSIVE_DARK_MODE(TMBool b)
    {
        return SetWindowAttribute(_DWMWA_USE_IMMERSIVE_DARK_MODE, &b, sizeof(b));
    }
    
};


///////////////////////////////////////////////////////////////////////////////
// CRefWindowT
template <class T, class T_Base = CWindowWin>
class CRefWindowWinT : public T_Base
{
protected:
    CAtlThunk       m_thunk;
    
    static LRESULT CALLBACK _WNDPROC(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        T* pThis = (T*)hWnd;//dwRefData;
        hWnd = pThis->m_hWnd;
        DbgAssert(hWnd);
        CRefPtr<T> ptr(pThis);
        if (uMsg == WM_NCDESTROY)
        {
            ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)::DefWindowProc);
            //            pThis->m_hWnd = 0;
            //             pThis->OnFinalMessage(hWnd);
            pThis->Release();
        }
        LRESULT lr = pThis->WndProc(uMsg, wParam, lParam);
        return lr;
    }
    
public:
    typedef CRefWindowWinT CRefWindow;
    
    ~CRefWindowWinT()
    {
        DbgAssert(!this->m_hWnd);
    }
    
    static LPCTSTR GetWndClassName() { return L"TM window"; }
    
    LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        //    return DefSubclassProc(m_hWnd, uMsg, wParam, lParam);
        return DefWindowProc(this->m_hWnd, uMsg, wParam, lParam);
    }
    
    //     void OnFinalMessage(HWND hWnd) {}
    void NativeCreate(HWND hWndParent, const TMRectNative& rcPos, LPCTSTR szWindowName = NULL,
        DWORD dwStyle = 0, DWORD dwExStyle = 0,
        UINT nID = 0, LPVOID lpCreateParam = NULL, HMODULE hModule = _ModuleATL.GetModuleInstance())
    {
        struct Register
        {
            static ATOM Exec1 ()
            {
                WNDCLASSEX  wc =
                {
                    sizeof (WNDCLASSEX),
                    CS_DBLCLKS | CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW,// | CS_OWNDC,
                    ::DefWindowProc, 0, 0, 0, 0,
                    ::LoadCursor(0, IDC_ARROW),
                    0, //(HBRUSH)GetStockObject(NULL_BRUSH),
                    0, T::GetWndClassName(), 0
                };
                
                return RegisterClassEx(&wc);
            }
        };
        static ATOM atom1 = Register::Exec1();
        
        this->m_hWnd = ::CreateWindowEx(
            dwExStyle, T::GetWndClassName(), szWindowName, dwStyle,
            RectLeft(rcPos), RectTop(rcPos),
            RectWidth(rcPos), RectHeight(rcPos),
            hWndParent, (HMENU)nID,
            hModule, lpCreateParam);
        DbgAssert(this->m_hWnd);
        
        T* pThis = static_cast<T*>(this);
        m_thunk.Init (T::_WNDPROC, pThis);
        ::SetWindowLongPtr(this->m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_thunk.GetWNDPROC());
        //         SetWindowSubclass(m_hWnd, _WNDPROC, 0, (DWORD_PTR)pThis);
    }
    
    void Create(HWND hWndParent, const TMRectNative& rcPos, LPCTSTR szWindowName = NULL,
        DWORD dwStyle = 0, DWORD dwExStyle = 0,
        UINT nID = 0, LPVOID lpCreateParam = NULL, HMODULE hModule = _ModuleATL.GetModuleInstance())
    {
        NativeCreate(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam, hModule);
        DbgAssert(this->m_hWnd);
        T* pThis = static_cast<T*>(this);
        pThis->AddRef();
    }

    void Recreate(HWND hWndParent, const TMRectNative& rcPos, LPCTSTR szWindowName = NULL,
        DWORD dwStyle = 0, DWORD dwExStyle = 0,
        UINT nID = 0, LPVOID lpCreateParam = NULL, HMODULE hModule = _ModuleATL.GetModuleInstance())
    {
        if (this->m_hWnd)
        {
            ::SetWindowLongPtr(this->m_hWnd, GWLP_WNDPROC, (LONG_PTR)::DefWindowProc);
            ::DestroyWindow(this->m_hWnd);
        }
        Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam, hModule);
    }
    
    void SendPostedEvents()
    {
        DbgAssert(this->m_hWnd);
        MSG msg;
        for (; ::PeekMessage(&msg, this->m_hWnd, 0, 0, PM_REMOVE);)
            (static_cast<T*>(this))->WndProc(msg.message, msg.wParam, msg.lParam);
    }
};

// template <class T>
// class CMessageWindowNativeT : public CRefWindowT<T>
// {
// public:
//     static LRESULT CALLBACK _WNDPROC(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//     {
//         T* pThis = (T*)hWnd;//dwRefData;
//         hWnd = pThis->m_hWnd;
//         DbgAssert(hWnd);
//         LRESULT lr = pThis->WndProc(uMsg, wParam, lParam);
//         return lr;
//     }
//     
//     void Create()
//     {
//         CRefWindowT<T>::NativeCreate(HWND_MESSAGE, TM::CRectNative(0,0,100,100), 0, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0);
//     }
//     
//     void DestroyWindow()
//     {
//         ::DestroyWindow(m_hWnd);
//         m_hWnd = 0;
//     }
// };


///////////////////////////////////////////////////////////////////////////////
// CIconWinT
template <bool t_bManaged>
class CIconWinT : public CIconT<t_bManaged>
{
public:
    TM_NO_COPYABLE(CIconWinT);
    
public:
    CIconWinT(HICON hIcon = 0) : CDCT<t_bManaged>(hIcon) {}
    
    typedef LPCTSTR icon_id;
    static icon_id icon_APPLICATION()   { return IDI_APPLICATION; }
    static icon_id icon_ASTERISK()      { return IDI_ASTERISK; }
    static icon_id icon_EXCLAMATION()   { return IDI_EXCLAMATION; }
    static icon_id icon_HAND()          { return IDI_HAND; }
    static icon_id icon_IDI_QUESTION()  { return IDI_QUESTION; }
    static icon_id icon_WINLOGO()       { return IDI_WINLOGO; }
};

typedef CIconWinT<true>     CIconWin;
typedef CIconWinT<false>    CIconHandleWin;


///////////////////////////////////////////////////////////////////////////////
// CCursorWinT
template <bool t_bManaged>
class CCursorWinT : public CCursorT<t_bManaged>
{
public:
    TM_NO_COPYABLE(CCursorWinT);
    
public:
    CCursorWinT(HICON hIcon = 0) : CCursorT<t_bManaged>(hIcon) {}

    typedef TMCStr cursor_id;

    static cursor_id cursor_NULL()          { return 0; }
    static cursor_id cursor_ARROW()         { return IDC_ARROW; }
    static cursor_id cursor_IBEAM()         { return IDC_IBEAM; }
    static cursor_id cursor_WAIT()          { return IDC_WAIT; }
    static cursor_id cursor_CROSS()         { return IDC_CROSS; }
    static cursor_id cursor_UPARROW()       { return IDC_UPARROW; }
    static cursor_id cursor_SIZENWSE()      { return IDC_SIZENWSE; }
    static cursor_id cursor_SIZENESW()      { return IDC_SIZENESW; }
    static cursor_id cursor_SIZEWE()        { return IDC_SIZEWE; }
    static cursor_id cursor_SIZENS()        { return IDC_SIZENS; }
    static cursor_id cursor_SIZEALL()       { return IDC_SIZEALL; }
    static cursor_id cursor_NO()            { return IDC_NO; }
    static cursor_id cursor_HAND()          { return IDC_HAND; }
    static cursor_id cursor_APPSTARTING()   { return IDC_APPSTARTING; }
    static cursor_id cursor_HELP()          { return IDC_HELP; }
    static cursor_id cursor_SPLITV()        { return IDC_SIZEWE; }
    static cursor_id cursor_SPLITH()        { return IDC_SIZENS; }
};

typedef CCursorWinT<true>     CCursorWin;
typedef CCursorWinT<false>    CCursorHandleWin;


///////////////////////////////////////////////////////////////////////////////
// CDCWinT
template <bool t_bManaged>
class CDCWinT : public CDCT<t_bManaged>
{
public:
    TM_NO_COPYABLE(CDCWinT);

public:
    CDCWinT(HDC hdc = 0)
        : CDCT<t_bManaged>(hdc)
    {
    }

    TMBool Blt24To32(
        int xDst, int yDst, int wDst, int hDst, HDC hImg, 
        int xSrc, int ySrc, int wSrc, int hSrc, DWORD dwRop = 0) // dwRop: CAPTUREBLT, NOMIRRORBITMAP
    {
        DbgAssert(hImg);
        DbgAssert(this->m_hDC);
//      if (GetDeviceCaps(hdc, BITSPIXEL) != 32)
//          __BBlt(hImg, hdc, uFlag, xDst, yDst, wDst, hDst, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
//      else
        {
            static HBRUSH hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
            HGDIOBJ hOld = SelectObject(this->m_hDC, hbr);
            DbgAssert(hbr);
            DbgAssert(hOld);
            enum { rop = 0x00CF0224 }; // Dest = (NOT Patten) OR Src
            GDIBlt(this->m_hDC, xDst, yDst, wDst, hDst, hImg, xSrc, ySrc, wSrc, hSrc, rop | dwRop);
            SelectObject(this->m_hDC, hOld);
            return true;
        }
    }

    BOOL AlphaBlend(
        int xDst, int yDst, int wDst, int hDst, HDC hImg, 
        int xSrc, int ySrc, int wSrc, int hSrc, BYTE AlphaFormat, TMUInt8 alpha)
    {
        DbgAssert(this->m_hDC != NULL);
        DbgAssert(hImg);
        DbgAssert(GetObjectType(hImg) == OBJ_DC || GetObjectType(hImg) == OBJ_MEMDC);
        
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = alpha;
        bf.AlphaFormat = AlphaFormat; //(hImg->dib.bpp == 32) ? 1 : 0;// AC_SRC_NO_PREMULT_ALPHA;
        
        TMBool b;
        DbgVerify(b = ::AlphaBlend(this->m_hDC, xDst, yDst, wDst, hDst, hImg, xSrc, ySrc, wSrc, hSrc, bf))
            ("(%p, %d, %d, %d, %d, %p, %d, %d, %d, %d, %d)", this->m_hDC, xDst, yDst, wDst, hDst, hImg, xSrc, ySrc, wSrc, hSrc, alpha);
        return b;
    }

    inline void ClearRect(const RECT& rc, TMColor clr)
    {
        DbgAssert(this->m_hDC != NULL);
        GDIFillRect(this->m_hDC, rc, clr);
    }

    // not cross platform
    int SetGraphicsMode(int iMode)
    {
        DbgAssert(this->m_hDC != NULL);
        return ::SetGraphicsMode(this->m_hDC, iMode);
    }

    // not cross platform
    int GetGraphicsMode() const
    {
        DbgAssert(this->m_hDC != NULL);
        return ::GetGraphicsMode(this->m_hDC);
    }

    void SetGraphicsModeAdv(TMBool bAdv) const
    {
        SetGraphicsMode(bAdv ? GM_ADVANCED : GM_COMPATIBLE);
    }

    bool IsGraphicsModeAdv() const
    {
        return GetGraphicsMode() == GM_ADVANCED;
    }

    BOOL GetWorldTransform(TMAffine& xform)
    {
        DbgAssert(this->m_hDC != NULL);
        return ::GetWorldTransform(this->m_hDC, &xform);
    }

    // not cross platform
    BOOL SetWorldTransform(const TMAffine& xform)
    {
        DbgAssert(this->m_hDC != NULL);
        return ::SetWorldTransform(this->m_hDC, &xform);
    }

    BOOL LeftMultiplyWorldTransform(const TMAffine* xform)
    {
        DbgAssert(this->m_hDC != NULL);
        return ::ModifyWorldTransform(this->m_hDC, xform, MWT_LEFTMULTIPLY);
    }

    BOOL RightMultiplyWorldTransform(const TMAffine* xform)
    {
        DbgAssert(this->m_hDC != NULL);
        return ::ModifyWorldTransform(this->m_hDC, xform, MWT_RIGHTMULTIPLY);
    }
    
    BOOL ModifyWorldTransform(const TMAffine* xform, DWORD iMode)
    {
        DbgAssert(this->m_hDC != NULL);
        return ::ModifyWorldTransform(this->m_hDC, xform, iMode);
    }
    
};

typedef CDCWinT<false>      CDCHandleWin;
typedef CDCWinT<true>       CDCWin;


///////////////////////////////////////////////////////////////////////////////
// CATScreenDC
struct CScreenDCWriteWin : public CDCWin
{
    CScreenDCWriteWin(const CRectNative& rc)
        : CDCWin(::GetWindowDC(0))
    {
    }

    ~CScreenDCWriteWin()
    {
        ::ReleaseDC(0, m_hDC);
    }
};


///////////////////////////////////////////////////////////////////////////////
// CAppAT
class CAppWin
{
public:
    static void SetDPIAware()
    {
#ifndef DPI_ENUMS_DECLARED
        typedef enum _PROCESS_DPI_AWARENESS {
            PROCESS_DPI_UNAWARE = 0,
            PROCESS_SYSTEM_DPI_AWARE = 1,
            PROCESS_PER_MONITOR_DPI_AWARE = 2
        } PROCESS_DPI_AWARENESS;
#endif // DPI_ENUMS_DECLARED

        DECLARE_DLL_FUNC(dll_shcore, HRESULT, WINAPI, SetProcessDpiAwareness, (PROCESS_DPI_AWARENESS value));
        if (_SetProcessDpiAwareness)
            _SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    }

    static TMBool IsDPIAware()
    {
        DECLARE_DLL_FUNC(dll_user32, BOOL, WINAPI, IsProcessDPIAware, ());
        return _IsProcessDPIAware && _IsProcessDPIAware();
    }

    static void Quit(int nExitCode)
    {
        ::PostQuitMessage(nExitCode);
    }

    static void ReleaseCapture()
    {
        DbgVerify(::ReleaseCapture());
    }

    static HWND GetCapture()
    {
        return ::GetCapture();
    }

    static TMBool IsHighContrast()
    {
        HIGHCONTRAST hc = { sizeof(HIGHCONTRAST) };
        SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, FALSE);
        return hc.dwFlags & HCF_HIGHCONTRASTON;
    }
};



TM_END_NAMESPACE


#endif // TMATL_WIN_H
