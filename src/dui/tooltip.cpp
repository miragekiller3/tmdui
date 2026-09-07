/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.6.16
 *
 *  Copyright (C) 2016 miragekiller
 */

#include "tooltip.h"
#include "uiwidget.h"


TM_BGN_NAMESPACE

// #define tm_trace_tt DbgTrace
#define tm_trace_tt DbgTraceNull

#define TimerMouseCreate() m_aniMouse.SetAni(this, 500)



CTooltipMgr g_mgrTooltip;


///////////////////////////////////////////////////////////////////////////////
// CTooltipSys
static void _CTooltip_AddTool(TMHWnd hTooltip, TMHWnd hTarget, TMCStr pStr, HTUI hWgt, CToolTipTui* dlg)
{
    tm_trace_tt(TM_T("_CTooltip_AddTool(hTooltip:%p, hTarget:%p, str:%s)"), hTooltip, hTarget, pStr);
    TMTOOLINFO _tiTool = { TTTOOLINFO_V1_SIZE, TTF_TRANSPARENT | TTF_IDISHWND | TTF_SUBCLASS }; // TTF_SUBCLASS
    _tiTool.hwnd = hTarget;
    _tiTool.uId = (TMUIntPtr)hTarget;
    _tiTool.lpszText = (TMStr)pStr;
    _tiTool.lParam = (TMLParam)hWgt;
    if (dlg && !dlg->Notify(hWgt, CM_HINTONADDTOOL, (TMLParam)&_tiTool))
        return;
    DbgVerify(CWindowAT(hTooltip).SendMessage(TTM_ADDTOOL, 0, (TMLParam)&_tiTool));

    if (hWgt->Is_WS_ADV_L2RHINT())
    {
        DbgVerify(CWindowAT(hTooltip).SendMessage(TTM_GETTOOLINFO, 0, (TMLParam)&_tiTool));
        _tiTool.uFlags &= ~TTF_RTLREADING;
        DbgVerify(CWindowAT(hTooltip).SendMessage(TTM_SETTOOLINFO, 0, (TMLParam)&_tiTool));
    }
}

static void _CTooltip_DelTool(TMHWnd hTooltip, TMHWnd hTarget)
{
    tm_trace_tt(TM_T("_CTooltip_DelTool(hTooltip:%p, hTarget:%p)"), hTooltip, hTarget);
    DbgAssert(CWindowAT(hTooltip).IsWindow());
  
    TMTOOLINFO _tiTool = { TTTOOLINFO_V1_SIZE, TTF_IDISHWND };
    _tiTool.hwnd = hTarget;
    _tiTool.uId = (TMUIntPtr)hTarget;
    CWindowAT(hTooltip).SendMessage(TTM_DELTOOL, 0, (TMLParam)&_tiTool);
}

#ifdef TM_QT

bool CTooltipSys::SetNativeToolTip(HTUI hWnd, TMCStr pStr, int nTuiID, ETTActive eActive, ETTActive eOld)
{
    tm_trace_tt(TM_T("CTooltipSys::SetNativeToolTip(hWnd:%p, str:%s, nTuiID:%d)"), hWnd, pStr, nTuiID);

    if (hWnd && pStr && pStr[0] && !nTuiID)
    {
        hWnd->GetOverlayHWND()->setToolTip(TM_T2Q(pStr));
        return true;
    }
    else
    {
        QToolTip::hideText();
        if (hWnd)
            hWnd->GetOverlayHWND()->setToolTip(QString());
    }
    return false;
}

void CTooltipSys::SetTipBkColor(TMColor clr) {}
void CTooltipSys::SetTipTextColor(TMColor clr) {}
void CTooltipSys::SetToLayered(TMBool bLayed) {}

#elif defined(TM_WIN)

void CTooltipSys::DestroyTip()
{
    tm_trace_tt(TM_T("CTooltipSys::DestroyTip(hWnd:%p)"), m_tooltip);
    ::DestroyWindow(m_tooltip); m_tooltip = 0;
}


static LRESULT CALLBACK _TTSubclassMove(
    HWND hWnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if (uMsg == WM_WINDOWPOSCHANGING)
    {
        WINDOWPOS* wp = (WINDOWPOS*)lParam;
        CRect* rc = (CRect*)dwRefData;
        if (wp->flags & SWP_NOSIZE)
        {
            CRect rcx;
            ::GetWindowRect(hWnd, &rcx);
            wp->cx = rcx.Width();
            wp->cy = rcx.Height();
        }
        wp->x = rc->Left() + (rc->Width() - wp->cx) / 2;
        wp->y = rc->Bottom();
        wp->flags &= ~SWP_NOMOVE;
        tm_trace_tt("_TTSubclassMove(%d,%d,%d,%d, %x)", wp->x, wp->y, wp->cx, wp->cy, wp->flags);
    }
    return CWindowAT(hWnd).DefSubclassProc(uMsg, wParam, lParam);
}

bool CTooltipSys::SetNativeToolTip(HTUI hWnd, LPCTSTR pStr, int nTuiID, ETTActive eActive, ETTActive eOld)
{
    tm_trace_tt(TM_T("CTooltipSys::SetNativeToolTip(hWgt:%p, str:%s, nTuiID:%d)"), hWnd, pStr, nTuiID);

    if (!hWnd || !pStr || !pStr[0] || nTuiID)
    {
        DestroyTip();
        return false;
    }

    // delete old tooltip
    HWND hTarget = hWnd->GetOverlayHWND();
    if (m_tooltip)
    {
        // immediately need destroy first.
        if (eActive != eOld || eActive != eTTActiveNormal || m_tooltip.GetWindow(GW_OWNER) != hTarget)
            DestroyTip();
        else
            _CTooltip_DelTool(m_tooltip, hTarget);
    }

    // create new tooltip
    if (!m_tooltip)
    {
        m_tooltip.Create(hTarget, 0, 0,
            TTS_NOPREFIX | TTS_ALWAYSTIP | WS_POPUP,// | 0x40,
            WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
//         SetWindowTheme(m_wndTool.m_hWnd, L"", L"");
        tm_trace_tt(TM_T("CTooltipSys::SetNativeToolTip::CreateWindow() HWND:%p"), m_tooltip.m_hWnd);
    }

    // font for HI-DPI
    TM::CZoom dpi = hWnd->GetDPI();
    DbgAssert(dpi);
    NONCLIENTMETRICS info = { sizeof(info), 0 };
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
    info.lfStatusFont.lfHeight = dpi.LPtoDP(-15); // info.lfStatusFont.lfHeight);
    CFontHandle font;
    DbgVerify(font.CreateFontIndirect(&info.lfStatusFont));
    m_tooltip.SetFont(font);

    // max width
    m_tooltip.SetMaxTipWidth(dpi.LPtoDP(500));

    // add tool
    _CTooltip_AddTool(m_tooltip, hTarget, pStr, hWnd, 0);

    // track activate
    if (eActive == eTTActiveNormal)
        return true;

    HWND h = hWnd->GetHWND();
    if (eActive == eTTActvieTab)
    {
        CRect rcs;
        hWnd->ClientToScreen(hWnd->GetWindowRect(), rcs);
        DbgVerify(m_tooltip.SetWindowSubclass(_TTSubclassMove, 0, (DWORD_PTR)&rcs));
        m_tooltip.TrackActivate(h, (TMUIntPtr)h, eActive);
        DbgVerify(m_tooltip.RemoveWindowSubclass(_TTSubclassMove, 0));
    }
    else
    {
        tm_trace_tt("CTooltipSys::TrackActivate() 1");
        m_tooltip.TrackActivate(h, (TMUIntPtr)h, eActive);
        tm_trace_tt("CTooltipSys::TrackActivate() 2");
    }


//     WIN::RemoveMesssage(0, WM_MOUSEMOVE);
//     WIN::RemoveMesssage(0, WM_NCMOUSEMOVE);
//     WIN::RemoveMesssage(0, WM_MOUSEMOVE);
//     WIN::RemoveMesssage(0, WM_NCMOUSEMOVE);

    return true;
}

void CTooltipSys::SetTipBkColor(TMColor clr)
{
    m_tooltip.SetTipBkColor(ColorToCOLORREF(clr));
}

void CTooltipSys::SetTipTextColor(TMColor clr)
{
    m_tooltip.SetTipTextColor(ColorToCOLORREF(clr));
}

void CTooltipSys::SetToLayered(TMBool bLayed)
{
    if (bLayed)
    {
        ::SetWindowLong(m_tooltip, GWL_EXSTYLE, ::GetWindowLong(m_tooltip, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(m_tooltip, 0, 255, LWA_ALPHA);
    }
    else
        ::SetWindowLong(m_tooltip, GWL_EXSTYLE, ::GetWindowLong(m_tooltip, GWL_EXSTYLE) & ~WS_EX_LAYERED);
}

#endif


///////////////////////////////////////////////////////////////////////////////
// class CToolTipTui
void CToolTipTui::SetNativeToolTip(HTUI hWnd, TMCStr pStr, int nTuiID, ETTActive eActive)
{
    // if mouse on tooltip, do nothing
    if (m_hWnd && hWnd)
    {
        if (g_mgrTooltip.IsTooltipChild(hWnd))
            return;
    }

    if (*this)
    {
        if (Edit_IsKeepMenuPop(Edit_IsObjectInSpyMode(m_tuiWindow.m_hWnd)))
            return;
        DestroyWindow();
    }

    if (!hWnd || !pStr || !pStr[0] || !nTuiID)
        return;

    TMHWnd hTarget = hWnd->GetOverlayHWND();
    this->Create(hTarget, 0, nTuiID, hWnd->GetModuleHandle());// .SubclassWindow(hWgt);
    DbgAssert(IsWindow());
    this->ShowWindow(SW_HIDE);
    _CTooltip_AddTool(m_hWnd, hTarget, pStr, hWnd, this);
    tm_trace_tt(L"CToolTipTui::SetNativeToolTip(hTarget:0x%p) ret:0x%p", hTarget, this);

    if (eActive)
        CToolTipCtrl(m_hWnd).TrackActivate(hTarget, (TMUIntPtr)hTarget, eActive);
}

TMBool CToolTipTui::Notify(HTUI hTarget, TMUInt msg, TMLParam lp)
{
    DbgAssert(hTarget);
    hTarget->SendMessage(msg, (TMWParam)m_tuiWindow.m_hWnd, lp);
    return m_tuiWindow && !m_tuiWindow.m_hWnd->IsLifeInvalid();
}

void CToolTipTui::SetTool(TMHWnd hTarget, TMBool bDestroying)
{
    tm_trace_tt("CToolTarget::SetTool(hNew:0x%p)", hTarget);
    
    // hide
    m_aniShow.KillAni();
    m_aniMouse.KillAni();
    ShowWindow(SW_HIDE);
    
    if (hTarget)
        TimerMouseCreate();
}

void CToolTipTui::TrackActivate()
{
    m_tuiWindow.SetAlpha(0);
    WINDOWPOS wp = { m_hWnd, 0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER };
    if (g_mgrTooltip.m_hTarget && !Notify(g_mgrTooltip.m_hTarget, CM_HINTONSHOW, (TMLParam)&wp))
        return;
    SetWindowPos(wp.hwndInsertAfter, wp.x, wp.y, wp.cx, wp.cy, wp.flags); // ShowWindow(SW_SHOWNOACTIVATE);
    m_aniMouse.KillAni();
    m_aniShow.SetAni(this, 1);
}

void CToolTipTui::OnAni(const CAniID0& tm, ...)
{
    tm_trace_tt("CToolTipTui::OnAni - ShowWindow(TMHWnd:0x%X)", m_hWnd);
    DbgAssert(!IsWindowVisible());
    DbgAssert(!m_aniShow);
    TrackActivate();
}

void CToolTipTui::OnAni(const CAniID1& tm, ...)
{
    TMUInt nFrm = m_aniShow.GetCurrentFrame();
    nFrm *= 1.2f;
    if (nFrm >= 255)
    {
        nFrm = 255;
        m_aniShow.KillAni();
    }
    tm_trace_tt("CToolTipTui::OnAni - SetAlpha(TMHWnd:0x%X, alpha:%d)", m_hWnd, nFrm);
    DbgAssert(CWindowAT(m_hWnd).IsWindowVisible());
    m_tuiWindow.SetAlpha(nFrm);
}

TMBool CToolTipTui::ProcessWindowMessage(TMHWnd hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{ 
    switch (uMsg)
    {
    case WM_INITDIALOG:
        tm_trace_tt(L"CToolTipTui::ProcessWindowMessage(WM_INITDIALOG)");
        m_tuiWindow.SetAlpha(0);
        break;

    case TTM_DELTOOL:
        if (Edit_IsKeepMenuPop(Edit_IsObjectInSpyMode(m_tuiWindow.m_hWnd)))
            break;
    case WM_DESTROY:
        tm_trace_tt(L"CToolTipTui::ProcessWindowMessage(TTM_DELTOOL or WM_DESTROY)");
        SetTool(0, uMsg == WM_DESTROY);
        break;

    case TTM_ADDTOOL:
        tm_trace_tt(L"CToolTipTui::ProcessWindowMessage(TTM_ADDTOOL)");
        SetTool(lParam ? ((TMTOOLINFO*)lParam)->hwnd : 0, false);
        break; //return (lResult = true);

    case TTM_RELAYEVENT:
//         tm_trace_tt(L"CToolTipTui::ProcessWindowMessage(TTM_RELAYEVENT)");
        TargetProc((TMMSG*)lParam);
        break;

    case TTM_TRACKACTIVATE:
        if (wParam)
            TrackActivate();
//         {
//             SetAlpha(255);
//             ShowWindow(SW_SHOWNOACTIVATE);
//             m_aniMouse.KillAni();
//             m_aniShow.KillAni();
//         }
        else
            SetTool(0, false);
        break;
    }
    return false;
}

void CToolTipTui::TargetProc(TMMSG* msg)
{ 
    switch (msg->message)
    {
    case WM_MOUSEMOVE:
        if (m_aniMouse)
        {
            tm_trace_tt(L"CToolTipTui::TargetProc(WM_MOUSEMOVE) SetCurrentFrame(0)");
            if (m_aniMouse)
                TimerMouseCreate();
        }
        break;

    case WM_MOUSELEAVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:

    case CM_NCMOUSELEAVE:
//     case WM_NCLBUTTONDOWN:
//     case WM_NCLBUTTONUP:
//     case WM_NCLBUTTONDBLCLK:
//     case WM_NCRBUTTONDOWN:
//     case WM_NCRBUTTONUP:
//     case WM_NCRBUTTONDBLCLK:
//     case WM_NCMBUTTONDOWN:
//     case WM_NCMBUTTONUP:
//     case WM_NCMBUTTONDBLCLK:
        if (Edit_IsKeepMenuPop(Edit_IsObjectInSpyMode(m_tuiWindow.m_hWnd)))
            break;
        tm_trace_tt(L"CToolTipTui::TargetProc(%s)", DbgFormatMessage(msg->message).c_str());
        SetTool(0, false);
        break;
    }
}


///////////////////////////////////////////////////////////////////////////////
// class CTooltipMgr
CTooltipMgr::CTooltipMgr()
    : m_bLayedTooltip (false)
    , m_clrBk(cColorInvalid)
    , m_clrText(cColorInvalid)
    , m_hTarget(0)
    , m_mode(eTTActiveNormal)
{
}

TMBool CTooltipMgr::IsPop()
{
    return (m_tuiTool && m_tuiTool.IsWindowVisible())
        || (m_wndTool.m_tooltip && m_wndTool.m_tooltip.IsWindowVisible());
}

void CTooltipMgr::Clear()
{
    m_wndTool.SetNativeToolTip(0,0,0, eTTActiveNormal, eTTActiveNormal);
    m_tuiTool.SetNativeToolTip(0,0,0, eTTActiveNormal);
}

void CTooltipMgr::SetToolTip(HTUI pNew, ETTActive eActive)
{
    CString str;
    int nObjIDTT;
    if (pNew)
    {
        pNew->GetWindowHint(eActive, &str);
        nObjIDTT = pNew->GetIDHint();
        if (nObjIDTT)
        {
            int x  = 0;
        }
        tm_trace_tt(L"CTooltipMgr::SetToolTip(pNew:%d, str:%s)", pNew->GetObjectID(), str.c_str());
    }
    else
        nObjIDTT = 0;

    if (m_wndTool.SetNativeToolTip(pNew, str, nObjIDTT, eActive, m_mode))
        UpdateProp(0);
    m_tuiTool.SetNativeToolTip(pNew, str, nObjIDTT, eActive);

    m_hTarget = pNew;
    m_mode = eActive;
}

void CTooltipMgr::SetTabToolTip(HTUI pNew)
{
    if (!pNew || pNew->GetView()->m_bShowHintOnTabKey)
        SetToolTip(pNew, eTTActvieTab);
}

void CTooltipMgr::UpdateProp(int id)
{
    if (id == eAppPropHintLayered || !id)
        m_wndTool.SetToLayered(m_bLayedTooltip);

    if (id == eAppPropHintColorBk || !id)
    {
        COLORREF clr = (m_clrBk == cColorInvalid ? ColorFromSys(COLOR_INFOBK) : m_clrBk);
        m_wndTool.SetTipBkColor(clr);
    }

    if (id == eAppPropHintColorText || !id)
    {
        COLORREF clr = (m_clrText == cColorInvalid ? ColorFromSys(COLOR_INFOTEXT) : m_clrText);
        m_wndTool.SetTipTextColor(clr);
    }
}

TMBool CTooltipMgr::IsTooltipChild(HTUI hWnd)
{
    if (!m_tuiTool)
        return false;
    DbgAssert(hWnd);
    TMHWnd hWin = hWnd->GetHWND();
    TMHWnd hTool = m_tuiTool;
    return hWin == hTool || CWindowAT(hTool).IsChild(hWin);
}

void CTooltipMgr::RelayMouseEvent(HTUI hOwner, TMUInt uMsg, TMWParam wParam, TMLParam lParam)
{
    if (m_tuiTool && !IsTooltipChild(hOwner))
    {
        TMMSG msg = { hOwner->GetHWND(), uMsg, wParam, lParam };
        m_tuiTool.SendMessage(TTM_RELAYEVENT, 0, (TMLParam)&msg);
    }
}

TM_END_NAMESPACE



