/*
 *  @file
 *  @brief  frame render
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.6.23
 *
 *  Copyright (C) 2016 miragekiller
 */

#pragma once

#ifndef PLATFORM_WIN_THEME_H
#define PLATFORM_WIN_THEME_H

#include "tmatl.h"
#include "view.h"


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// base class
class ITheme
{
public:
    TM_IF_DBG(TMCStr m_pType);

public:
    virtual EThemeType GetType() = 0;
    virtual void Enable(CTuiLayerWnd* hWgt, TMHWnd hWnd, TMBool bEnable) = 0;
    virtual bool DefProc(CTuiLayerWnd* pMgr, TMHWndNative hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& ret) { return false; }
    virtual bool Present(CTuiLayerWnd* hWgt, TMHWnd hWnd, GXPTexture tex, CDirtyRectT<CRect>& drc)
    {
        if (!tex)
            return false;
        TUI_IF_EDIT(CAutoFps _autofps(hWgt->m_fpsFlip));
        CRect rc(0,0,tex->GetWidth(), tex->GetHeight());
        rc &= drc.GetDirty();
        GXPRESENTFX gfx = { 0, 0, 0, {0, 0}, &rc };
        //         DbgTrace("ITheme::Present(%d, %d,%d,%d,%d)", hWgt->GetObjectID(), rc);
        EGXRet ret = tex->Present(hWnd, eGXPresentDCClient, &gfx);
        return CheckPresentRet(ret, hWgt,hWnd, tex, drc);
    }

    bool IsThemeLayered() const { return this == GetThemeLayered(); }
    bool IsThemeSys() const { return this == GetThemeSys(); }

    static ITheme* GetThemeSys();
    static ITheme* GetThemeLayered();
    static ITheme* GetThemeTransparent(TMUInt32 dwStyle, TMUInt32 dwExStyle);
    static ITheme* GetThemeCurrent(CTuiLayerWnd* hMgr, EThemeType eDft = eThemeTypeAuto);

    static void FrameChange(TMHWnd hWnd)
    {
        CWindowAT(hWnd).ChangeFrame();
    }

protected:
    static void ResetStyle(CTuiLayerWnd* hWgt, TMHWnd hWnd, TMUInt32 dwStyleAdd, TMUInt32 dwExStyleAdd = 0)
    {
        enum
        {
            dwMask = WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CHILD,// | DS_CENTER | WS_SIZEBOX,// | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            dwMaskEx = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_ADV_NOACTIVATE,
        };
        
        TMUInt32 dwStyle = hWgt->GetThemeStyle();
        TMUInt32 dwExStyle = hWgt->GetThemeExStyle();
        dwStyle &= dwMask;
        dwExStyle &= dwMaskEx;
        CWindowAT wnd(hWnd);
        TMUInt32 dwStyleWnd = wnd.GetStyle();
        dwStyle |= (dwStyleWnd & (WS_MAXIMIZE | WS_MINIMIZE | WS_CHILD | WS_VISIBLE | WS_DISABLED));

        if (dwStyleWnd & WS_CHILD)
        {
            if (!(dwExStyle & WS_ADV_TABGROUP))
                dwExStyle |= WS_EX_CONTROLPARENT;
            if (dwStyle & WS_TABSTOP)
                dwExStyle &= ~WS_EX_CONTROLPARENT;
        }
        else
        {
            dwStyle |= dwStyleAdd;
            dwExStyle |= dwExStyleAdd;
        }
        
        if (hWgt->GetModule()->GetLocal().IsRTL())
            dwExStyle |= WS_EX_LAYOUTRTL;
        if (eGXGraDevGDI != (_gxApp->GetDeviceType() & eGXGraDevMask))
            dwStyle |= WS_CLIPCHILDREN;
        
        if (dwStyleWnd != dwStyle)
            wnd.SetStyle(dwStyle);
        wnd.SetExStyle(dwExStyle);
    }

#ifdef TM_WIN_NO_QT
    static void EnableAreo(TMHWnd hWnd, TMBool bEnable, int nClient)
    {
        if (bEnable)
            CWindowAT(hWnd).EnableAreo(nClient);
    }

    static inline bool CheckPresentRet(EGXRet ret, CTuiLayerWnd* hWgt, HWND hWnd, GXPTexture tex, CDirtyRectT<CRect>& drc)
    {
        if (eGXRetSuccess == ret)
        {
            Edit_SaveFlip(Edit_IsObjectInSpyMode(hWgt), hWgt->GetObjectID(), hWnd, tex, drc.m_rcDirty, 0);
            drc.Validate();  // _ValidateUpdateRect(hWgt);
            return true;
        }
        if (eGXRetDeviceChanged == ret)
        {
            drc.Validate(); // _ValidateUpdateRect(hWgt);
            return true;
        }
        return false;
    }

    static bool FlipToLayered(CTuiLayerWnd* hWgt, TMHWnd hWnd, GXPTexture tex, const CPoint& pt, const CSize& sz, CDirtyRectT<CRect>& drc)
    {
        DbgAssert(tex);
        TUI_IF_EDIT(CAutoFps _autofps(hWgt->m_fpsFlip));
        drc.m_rcDirty &= CRect(0, 0, sz.Width(), sz.Height());
        CRect rc(drc.m_rcDirty);
        if (hWgt->GetModule()->GetLocal().IsRTL())
            rc.KeepWSetL(sz.Width() - rc.Right());
        GXPRESENTFX fx = { hWgt->GetAlpha(), &pt, &sz, {0}, &rc };
        EGXRet ret = tex->Present(hWnd, eGXPresentLayered, &fx);
        { CPaintDC dc(hWnd); }
        return CheckPresentRet(ret, hWgt, hWnd, tex, drc);
    }

    static bool FlipToNull(CTuiLayerWnd* hWgt, TMHWnd hWnd, const CPoint& pt, CDirtyRectT<CRect>& drc)
    {
        DbgAssert(hWgt->GetWidth() <= 0 || hWgt->GetHeight() <= 0);
        DbgTrace("CThemeLayered::Present(%d,%d,0,0)", pt);
        DbgVerify(::SetWindowPos(hWnd, 0, pt.X(), pt.Y(), 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOACTIVATE |SWP_NOREDRAW));
        drc.Validate(); // _ValidateUpdateRect(hWgt); // hWgt->m_rcUpdate.Validate();
        CPaintDC dc(hWnd);
        return true;
    }
#endif // #ifdef TM_WIN_NO_QT
};


///////////////////////////////////////////////////////////////////////////////
// use system frame
class CThemeSys : public ITheme
{
public:
    CThemeSys() { TM_IF_DBG(m_pType = TM_T("SYS")); }
    virtual EThemeType GetType() tm_override { return eThemeTypeSystem; }
    virtual void Enable(CTuiLayerWnd* hWgt, TMHWnd hWnd, TMBool bEnable) tm_override
    {
        if (bEnable)
        {
            enum { dwMask = WS_CAPTION | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX };
            enum { dwMaskEx = WS_EX_CLIENTEDGE };
            ResetStyle(hWgt, hWnd, hWgt->GetThemeStyle() & dwMask, hWgt->GetThemeExStyle() & dwMaskEx);
        }
    }
};

ITheme* ITheme::GetThemeSys()
{
    static CThemeSys s_renderSys;
    return &s_renderSys;
}


///////////////////////////////////////////////////////////////////////////////
// default frame
class CThemeXP : public ITheme
{
public:
    CThemeXP() { TM_IF_DBG(m_pType = TM_T("XP")); }
    
    virtual EThemeType GetType() tm_override { return eThemeTypeXP; }

    virtual void Enable(CTuiLayerWnd* hWgt, TMHWnd hWnd, TMBool bEnable) tm_override
    {
        if (bEnable)
        {
            ResetStyle(hWgt, hWnd, WS_POPUP);
//             EnableTheme(hWnd, !bEnable, true, true);
        }
    }
};


#if defined(TM_WIN_NO_QT)

struct PROCESSCHANGING
{
    SIZE sz;      // new window size
    POINT pt;     // new window top left
    POINT org;    // x > 0 && y > 0:  drag left to right or drag top to bottom; x < 0 && y < 0:  drag left to left or drag top to top
    
    TMBool IsDragLeftOrTop(CTuiLayerWnd* pMgr, TMLParam lParam)
    {
        WINDOWPOS* pwp = (WINDOWPOS*)lParam;
        if (!pMgr->IsPainting() && pwp->cx > 0 && pwp->cy > 0 && pMgr->GetTarget() &&
            ((pwp->flags & (SWP_NOSIZE /*| SWP_NOMOVE*/ | SWP_HIDEWINDOW)) == 0) &&
            !(GetWindowLong(pwp->hwnd, GWL_STYLE) & (WS_MINIMIZE | WS_MAXIMIZE))) // && ::GetCapture() == pwp->hwnd)
        {
            RECT rcw;
            ::GetWindowRect(pwp->hwnd, &rcw);
            int x = pwp->x;
            int y = pwp->y;
            org.x = x - rcw.left;
            org.y = y - rcw.top;
            return (org.x || org.y); // drag left or top side.
        }
        return false;
    }
};

///////////////////////////////////////////////////////////////////////////////
// layered mode frame
class CThemeLayered : public ITheme
{
public:
    CThemeLayered() { TM_IF_DBG(m_pType = TM_T("Layered")); }

    virtual EThemeType GetType() tm_override { return eThemeTypeLayered; }

    virtual void Enable(CTuiLayerWnd* hWgt, TMHWnd hWnd, TMBool bEnable) tm_override
    {
        if (bEnable)
            ResetStyle(hWgt, hWnd, WS_POPUP, WS_ADV_LAYERED);
    }

    virtual bool Present(CTuiLayerWnd* hWgt, TMHWnd hWnd, GXPTexture tex, CDirtyRectT<CRect>& drc) tm_override
    {
        CPoint pt;
        if (hWgt->Is_WS_PRI_IS_WINDOW())
        {
            CRectI rc;
            DbgVerify(::GetWindowRect(hWnd, &rc));
            pt.SetPoint(rc.Left(), rc.Top());
            if (tex)
            {
                CSize sz(tex->GetWidth(), tex->GetHeight());
                if (rc.Width() != sz.Width() || rc.Height() != sz.Height())
                    return false;
                return FlipToLayered(hWgt, hWnd, tex, pt, sz, drc);
            }
        }
        else
        {
            CTuiPoint rcw(hWgt->GetModule()->GetLocal().IsRTL() ? hWgt->GetRight() : hWgt->GetLeft(), hWgt->GetTop());
            hWgt->ClientToScreen(&rcw, 1, &pt);
            if (tex)
            {
                CSize sz(tex->GetWidth(), tex->GetHeight());
                return FlipToLayered(hWgt, hWnd, tex, pt, sz, drc);
            }
        }

        // no texture
        FlipToNull(hWgt, hWnd, pt, drc);
        return true;
    }

#if defined(TM_WIN) && !defined(TM_QT)
    static TMBool ProcessChanging(CTuiLayerWnd* pMgr, TMLParam lParam, PROCESSCHANGING& pc)
    {
        if (pc.IsDragLeftOrTop(pMgr, lParam))
        {
            WINDOWPOS* pwp = (WINDOWPOS*)lParam;
            pc.org.x = 0;//x - rcw.left;
            pc.org.y = 0;//y - rcw.top;
            
            int w = pwp->cx;
            int h = pwp->cy;
            pc.sz.cx = w;
            pc.sz.cy = h;
            pc.pt.x = pwp->x;
            pc.pt.y = pwp->y;
            return true;
        }
        return false;
    }
    
    bool DefProc(CTuiLayerWnd* pMgr, HWND hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& ret) tm_override
    {
        // only for drag window left border
        if (uMsg == WM_WINDOWPOSCHANGING)
        {
//             if (_gxApp->GetDeviceType() == eGXDeviceD3D11)
//             {
//                 DwmFlush();
//                 return false;
//             }

            DbgAssert(WS_ADV_LAYERED & ::GetWindowLong(hWnd, GWL_EXSTYLE));
            PROCESSCHANGING pc;
            if (ProcessChanging(pMgr, lParam, pc))
            {
                DbgTrace("CThemeLayered::WM_WINDOWPOSCHANGING(oid:%d, pt:%d,%d, size:%d,%d, org:%d,%d)",
                    pMgr->GetObjectID(), pc.pt, pc.sz, pc.org);
                DbgAssert(pMgr->GetTarget());
                GXPRESENTFX fx = { pMgr->GetAlpha(), &pc.pt, &pc.sz, { pc.org.x, pc.org.y } };
                pMgr->GetTarget()->Present(hWnd, eGXPresentLayered, &fx);
                { CPaintDC dc(hWnd); }
                DbgVerify(::SendMessage(hWnd, WM_WINDOWPOSCHANGED, wParam, lParam));
            }
        }
        else if (uMsg == WM_NCCALCSIZE)
        {
            RECT rcx = *(RECT*)lParam;
            DefWindowProc(hWnd, uMsg, wParam, lParam);
            *(RECT*)lParam = rcx;
            return true;
        }
        else if (uMsg == WM_WINDOWPOSCHANGED)
        {
            WINDOWPOS* pws = (WINDOWPOS*)lParam;
            if (pws->flags & SWP_SHOWWINDOW) // layered window not paint when show / hide.
                pMgr->GetReal()->RealInvalidate(true, true);
        }
        else if (uMsg == WM_SHOWWINDOW)
        {
            if (lParam)
                pMgr->GetReal()->RealInvalidate(true, true);
        }

        return false;
    }
#endif // #ifdef TM_WIN
};


///////////////////////////////////////////////////////////////////////////////
// DWM mode frame
struct CThemeDWMFlickerGhost
{
    typedef CTuiWidgetPosT<WINDOWPOS, CRectNative> CWindowPosNative;
    
    struct CGhostWindow
        : public CRefWindowWinT<CGhostWindow>
        , public CTimerImpl<CGhostWindow>
    {
        CGhostWindow()
        {
        }
        
        ~CGhostWindow()
        {
            if (m_hWnd)
            {
                ::DestroyWindow(m_hWnd);
                m_hWnd = 0;
            }
        }
        
        TM_UNUSED_ADDREF_RELEASE_NO_OVERRIDE();
        
        static TMCStr GetWndClassName() { return TUI_CLASS_WIN_GHOST; }
        
        void Show(CTuiLayerWnd* pMgr, CWindowPosNative* wp)//lParam HWND hTarget)//, int x, int y, int cx, int cy)
        {
            if (Edit_IsDisableGhost())
                return;

            if (wp->IsHide())
            {
                if (m_hWnd)
                    Hide();
                return;
            }
            
            if (CWindow(wp->hwnd).GetStyle() & WS_CHILD)
                return;
            
            if (!wp->IsShow())
            {
                if (!wp->IsSize() || !::IsWindowVisible(wp->hwnd))
                    return;
            }
            else
            {
                CRectNative rc;
                CWindow(wp->hwnd).GetWindowRect(&rc);
                wp->ReplaceFromRect(rc);
            }
            
            if (!CAppAT::IsDPIAware())
                return;
            
            if (!m_hWnd)
                Create(0, CRectNative(0,0,0,0), 0, WS_POPUP, WS_ADV_NOACTIVATE | WS_ADV_TOOLWINDOW | WS_EX_TRANSPARENT);
            
            CRectNative rc;
            rc.SetLTWH(wp->x, wp->y, wp->cx, wp->cy);
            Hide();
            SetAni(1000);
            SetWindowPos(HWND_TOPMOST,
                rc, SWP_SHOWWINDOW | SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOREDRAW);
            
            CWindowDC(m_hWnd).BitBlt(0, 0, rc.Width(), rc.Height(), CWindowDC(0), rc.left, rc.top, SRCCOPY);
            
            if (wp->hwndInsertAfter == HWND_TOPMOST)
                wp->hwndInsertAfter = m_hWnd;
            _DwmFlush();
            
            DbgTrace("CGhostWindow::Show(%x, id:%d, rect:" TM_FMT_RCI_STR ", flag:%x)",
                m_hWnd, pMgr->GetObjectID(), TM_FMT_RCI_VAL(rc), wp->flags);
        }
        
        void Hide()
        {
            if (m_tmID)
                DbgTrace("CGhostWindow::Hide(%x)", m_hWnd);

            KillAni();
            _DwmFlush();
            ::ShowWindow(m_hWnd, SW_HIDE);
        }
        
        TMBool IsVisible()
        {
            return ::IsWindowVisible(m_hWnd);
        }
        
        // CTimerImpl
        void OnTimer(...)
        {
            Hide();
        }
    };

    struct CDWMFlushOpt
    {
        TMBool bFlushing;

        CDWMFlushOpt() : bFlushing(false) {}

        void Set()
        {
            if (bFlushing)
                return;
            bFlushing = true;
            _DwmFlush();
        }
        void Reset()
        {
            bFlushing = false;
        }
    };

    CGhostWindow m_wndGhost;
    CDWMFlushOpt m_optDWMFlush;

    bool DefProc(CTuiLayerWnd* pMgr, HWND hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam)
    {
        switch (uMsg)
        {
        case WM_NULL:
            if (m_wndGhost.IsVisible())
                m_wndGhost.Hide();
            return false;

        case WM_PAINT:
        case WM_DESTROY:
            if (m_wndGhost.IsVisible())
            {
                //                 ::UpdateWindow(hWnd);
                // D3DKMTWaitForVerticalBlankEvent(&waitData);
//                 DwmFlush();
                DbgTrace("CGhostWindow::Hide(id:%d)", pMgr->GetObjectID());
                m_wndGhost.Hide();
            }
            m_optDWMFlush.Reset();
            return false;
            
        case WM_WINDOWPOSCHANGING:
            {
                PROCESSCHANGING pc;
                if (!pc.IsDragLeftOrTop(pMgr, lParam))
                    return false;
            }
            if (1) //_gxApp->GetDeviceType() == eGXDeviceGDI)
                m_wndGhost.Show(pMgr, (CWindowPosNative*)lParam);
            else
                //m_optDWMFlush.Set();
                _DwmFlush();
            return false;

        case WM_NCCALCSIZE:
            {
                RECT rcx = *(RECT*)lParam;
                ::DefWindowProc(hWnd, uMsg, wParam, lParam);
                *(RECT*)lParam = rcx;
            }
            return true;
        }
        
        return false;
    }
};

struct CThemeDWMFlickerDWM
{
    typedef CTuiWidgetPosT<WINDOWPOS, CRectNative> CWindowPosNative;

    bool DefProc(CTuiLayerWnd* pMgr, HWND hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam)
    {
        static TMBool s_bFlick;
        switch (uMsg)
        {
        case WM_NCCALCSIZE:
        {
            RECT rcx = *(RECT*)lParam;
            ::DefWindowProc(hWnd, uMsg, wParam, lParam);
            *(RECT*)lParam = rcx;
            s_bFlick = true;
            _DwmFlush();
        }
        return true;

        case WM_NCPAINT:
            if (s_bFlick)
            {
//                ::DefWindowProc(hWnd, uMsg, wParam, lParam);
                s_bFlick = false;
//                 TMResult ret;
                pMgr->GetReal()->RealInvalidate(false, true); // Paint(ret);
                return true;
            }
                //::DefWindowProc(hWnd, uMsg, wParam, lParam);
        return false;
        }

        return false;
    }
};

class CThemeDWMBase
    : public ITheme
{
public:
    virtual bool DefProc(CTuiLayerWnd* pMgr, HWND hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& ret) tm_override
    {
        switch (uMsg)
        {
        case WM_NCHITTEST:
            ret = HTCLIENT;
            return true;
            
        case WM_NCACTIVATE:
            ret = true;
            return true;
        }
        
//        TMBool bGDI = (_gxApp->GetDeviceType() == eGXDeviceGDI);
        static TMBool bWin11 = (COSVersion().GetVersion() >= COSVersion::eVersionWin11);
        if (bWin11)
        {
            static CThemeDWMFlickerDWM s_impl;
            return s_impl.DefProc(pMgr, hWnd, uMsg, wParam, lParam);
        }
        else
        {
            static CThemeDWMFlickerGhost s_impl;
            return s_impl.DefProc(pMgr, hWnd, uMsg, wParam, lParam);
        }
    }

protected:
    void _ResetStyle(CTuiLayerWnd* hWgt, TMHWnd hWnd, TMBool bEnable, TMUInt32 dwStyle)
    {
        if (bEnable)
        {
            TMUInt32 dwAdd = WS_POPUP; // WS_CAPTION will display error when maximize;
            if (CWindowAT(hWnd).IsWindowVisible())
                dwAdd |= WS_SIZEBOX;
            else
                hWgt->Set_MS_PRI_NEEDRESETSIZEOBX();
            ResetStyle(hWgt, hWnd, dwAdd);
        }
    }

    void _EnableAreo(TMHWnd hWnd, TMBool bEnable, TMUInt32 dwStyle)
    {
        // Extend the frame into the client area.
        int n = (dwStyle & DS_NODWMSHADOW) ? 0 : 1;
        EnableAreo(hWnd, bEnable, n);

        TMResult ret;
        CThemeDWMBase::DefProc(0, 0, WM_NULL, 0, 0, ret);
    }
};

class CThemeDWMNormal
    : public CThemeDWMBase
{
public:
#ifdef TM_WIN
    virtual bool DefProc(CTuiLayerWnd* pMgr, HWND hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& ret) tm_override
    {
        switch (uMsg)
        {
        // MS_PRI_NEEDERASEBKGND use to fix some PC bug:
        //  min window and restore it, then invalidate sub rect.
        //  the window only display sub rect and lost other rect.
        case WM_NCPAINT:
            if (eGXGraDevGDI == (gxGetApp()->GetDeviceType() & eGXGraDevMask))
                pMgr->Set_MS_PRI_NEEDERASEBKGND();
            break;

        case WM_ERASEBKGND:
            if (pMgr->Is_MS_PRI_NEEDERASEBKGND() && !pMgr->Is_MS_PRI_NEEDRESETSIZEOBX())
            {
                CDCHandle dc((HDC)wParam);
                CRect rcWindow;
                CWindowAT wnd(pMgr->GetHWND());
                wnd.GetClientRect(&rcWindow);
                dc.FillRect(&rcWindow, ::GetSysColorBrush(COLOR_WINDOW));
                pMgr->Reset_MS_PRI_NEEDERASEBKGND();
            }
            return ret = true;
        }
        
        return CThemeDWMBase::DefProc(pMgr, hWnd, uMsg, wParam, lParam, ret);
    }

#endif // #ifdef TM_WIN

    CThemeDWMNormal() { TM_IF_DBG(m_pType = TM_T("DWM")); }

protected:
    virtual EThemeType GetType() tm_override { return eThemeTypeDWM; }

    virtual void Enable(CTuiLayerWnd* hWgt, TMHWnd hWnd, TMBool bEnable) tm_override
    {
        TMUInt32 dwStyle = hWgt->GetThemeStyle();
        _ResetStyle(hWgt, hWnd, bEnable, dwStyle);
        _EnableAreo(hWnd, bEnable, dwStyle);
    }
};


///////////////////////////////////////////////////////////////////////////////
// DWM + Blur
class CThemeDWMBlur : public CThemeDWMBase
{
public:
    CThemeDWMBlur() { TM_IF_DBG(m_pType = TM_T("DWM-Blur")); }

    virtual EThemeType GetType() tm_override { return eThemeTypeDWMBlur; }

    virtual void Enable(CTuiLayerWnd* hWgt, TMHWnd hWnd, TMBool bEnable) tm_override
    {
        TMUInt32 dwStyle = hWgt->GetThemeStyle();
        _ResetStyle(hWgt, hWnd, bEnable, dwStyle);
        CWindowAT(hWnd).EnableBlur(bEnable ? ACCENT_ENABLE_BLURBEHIND : ACCENT_ENABLE_GRADIENT);
        _EnableAreo(hWnd, bEnable, dwStyle);
    }
};

class CThemeDWMTransparent : public CThemeDWMBase
{
public:
    CThemeDWMTransparent() { TM_IF_DBG(m_pType = TM_T("DWM-Transparent")); }
    
    virtual EThemeType GetType() tm_override { return eThemeTypeDWMTransparent; }
    
    virtual void Enable(CTuiLayerWnd* hWgt, TMHWnd hWnd, TMBool bEnable) tm_override
    {
        TMUInt32 dwStyle = hWgt->GetThemeStyle();
//        _ResetStyle(hWgt, hWnd, bEnable, dwStyle);
        ResetStyle(hWgt, hWnd, WS_POPUP);

        CWindowAT(hWnd).EnableBlur(bEnable ? ACCENT_DISABLED : ACCENT_ENABLE_GRADIENT);
//        CWindowAT(hWnd).EnableBlur(bEnable ? ACCENT_ENABLE_GRADIENT : ACCENT_ENABLE_GRADIENT);
        EnableAreo(hWnd, bEnable, -1);
    }
};

static TMBool _ISysFrame_IsEnableDWM()
{
#ifdef TM_WIN
    if (_DwmIsCompositionEnabled)
    {
        BOOL bEnable = false;
        DbgVerifyHResult(_DwmIsCompositionEnabled(&bEnable));
        return bEnable;
    }
#endif
    return false;
}

ITheme* ITheme::GetThemeLayered()
{
    static CThemeLayered s_renderLayered;
    return &s_renderLayered;
}

ITheme* ITheme::GetThemeTransparent(TMUInt32 dwStyle, TMUInt32 dwExStyle)
{
    return GetThemeLayered();

//     static CThemeDWMTransparent s_renderDWMTransparent;
//     if (!(dwStyle & DS_NODWMTRANSPARENT) && !(dwExStyle & WS_ADV_PENETRATE) && _ISysFrame_IsEnableDWM())
//         return &s_renderDWMTransparent;
//     else
//         return GetThemeLayered();
}

#endif // #if defined(TM_WIN_NO_QT)

ITheme* ITheme::GetThemeCurrent(CTuiLayerWnd* hMgr, EThemeType eDft)
{
#ifdef TM_QT

    static CThemeXP         s_renderXP;
    ITheme* pRet;
    
    TMUInt32 dwStyle = hMgr->GetThemeStyle();
    TMUInt32 dwExStyle = hMgr->GetThemeExStyle();
    if (dwStyle & WS_CHILD)
        pRet = &s_renderXP;
    else if ((eDft == eThemeTypeSystem || hMgr->GetView()->IsSysFrame()) && !(dwExStyle & WS_ADV_TOOLWINDOW))
        pRet = GetThemeSys();
    else if (eDft == eThemeTypeXP)
        pRet = &s_renderXP;
    else
        pRet = &s_renderXP;

#else // TM_QT 

    static CThemeXP         s_renderXP;
    static CThemeDWMNormal  s_renderDWM;
    static CThemeDWMBlur    s_renderDWMBlur;
    ITheme* pRet;

    TMUInt32 dwStyle = hMgr->GetThemeStyle();
    TMUInt32 dwExStyle = hMgr->GetThemeExStyle();
    if (dwStyle & WS_CHILD)
        pRet = &s_renderXP;
    else if ((dwStyle & DS_DWMBLUR) && _ISysFrame_IsEnableDWM())
        pRet = &s_renderDWMBlur;
    else if (dwExStyle & WS_ADV_LAYERED)
        pRet = GetThemeTransparent(dwStyle, dwExStyle);
    else if ((eDft == eThemeTypeSystem || hMgr->GetView()->IsSysFrame()) && !(dwExStyle & WS_ADV_TOOLWINDOW))
        pRet = GetThemeSys();
    else if (eDft == eThemeTypeXP)
        pRet = &s_renderXP;
    else if (_ISysFrame_IsEnableDWM())
        pRet = &s_renderDWM;
    else
        pRet = &s_renderXP;

#endif // TM_QT else

#ifdef TM_DBG
    DbgTrace(TM_T("ITheme::GetThemeCurrent(%d) type:%s"), hMgr->GetObjectID(), pRet->m_pType);
#endif
    return pRet;
}



TM_END_NAMESPACE



#endif // PLATFORM_WIN_THEME_H
