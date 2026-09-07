/*
 *  @file
 *  @brief  TMDui for WIN
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.27
 *
 *  Copyright (C) 2011 miragekiller
 */

#include "../tmhook.h"
#include "../tmgdi.h"
#include "../tmduiacc.h"
#include "../gximage.h"
#include "tmmouse.h"

#include "uiwindow_win.h"
#include "platformproc.h"
#include "uiwindow_win_theme.h"
#include "tmduiatl.h"
#include "tooltip.h"


#ifdef DINPUT_MOUSE_HOOK
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#endif

#pragma comment(lib, "Imm32.lib")


#ifdef TM_WIN_DESKTOP


#define TRACE_MOUSE

#ifdef TRACE_MOUSE
#   define tm_trace_mouse DbgTrace
#else
#   define tm_trace_mouse DbgTraceNull
#endif

#define tm_trace_ime    DbgTrace
// #define tm_trace_ime DbgTraceNull



TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////
// used to track popup menu
struct TraceMenu : public TM::CHookMsgFilterImpl<TraceMenu>
{
    TraceMenu()
    {
        SetHook();
    }
    
    ~TraceMenu()
    {
        RemoveHook();
    }
    
    TMBool OnMsgFilter(int nEvent, MSG* pMsg)
    {
        if (nEvent != MSGF_MENU)
            return 0;
        if (pMsg->message == WM_LBUTTONDBLCLK)
            PostMessage (pMsg->hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
        return 0;
    }
    
    TMHMenu InitSysMenu(TM::CTuiWgt* pWnd, TMHMenu hMenu)
    {
        int nItem = GetMenuItemCount(hMenu);
        
        TM::CTuiWidget wnd(pWnd);
        
        // iterate all the menu items looking for something resembling a sys menu item
        while (nItem--)
        {
            TMUInt uID = GetMenuItemID(hMenu,  nItem);
            if (uID >= 0xF000)
            {
                TMBool bEnable = TMTrue;
                DISABLE_SPY();
                switch (uID & 0xFFF0)
                {
                case SC_MINIMIZE:
                    bEnable = (wnd.GetStyle() & (WS_MINIMIZEBOX | WS_MINIMIZE)) == WS_MINIMIZEBOX;// && !wnd.IsIconic();
                    break;
                    
                case SC_MAXIMIZE:
                    bEnable = (wnd.GetStyle() & (WS_MAXIMIZEBOX | WS_MAXIMIZE)) == WS_MAXIMIZEBOX;// && !wnd.IsZoomed();
                    break;
                    
                case SC_RESTORE:
                    bEnable = wnd.GetStyle() & (WS_MINIMIZE | WS_MAXIMIZE);//  wnd.IsIconic() || wnd.IsZoomed();
                    break;
                    
                case SC_MOVE:
                    bEnable = !(wnd.GetStyle() & (WS_MINIMIZE | WS_MAXIMIZE));// !wnd.IsIconic() && !wnd.IsZoomed();
                    break;
                    
                case SC_SIZE:
                    bEnable = (wnd.GetStyle() & (WS_MINIMIZE | WS_MAXIMIZE | WS_SIZEBOX)) == WS_SIZEBOX;// (wnd.GetStyle() & WS_SIZEBOX) && !wnd.IsIconic() && !wnd.IsZoomed();
                    break;
                }
                
                EnableMenuItem(hMenu, uID, bEnable ? MF_ENABLED : MF_GRAYED);
            }
        }
        
        return hMenu;
    }
};

///////////////////////////////////////////////////////////////////////////
// used to HookGlobalMouseDown
struct CMouseInputBase
{
    static void NotifyBy(HWND hWndWgt, HWND hWnd)
    {
        if (!hWnd)
            return;

        // skip same thread window
        TMLParam nType = WIN::GetWinPropType(hWnd);
        TMBool bSameThead = ((nType & (WIN_PROP_TYPE_MGR | WIN_PROP_TYPE_FLOAT))
            && (::GetWindowThreadProcessId(hWnd, 0) == ::GetWindowThreadProcessId(hWndWgt, 0)));
        if (bSameThead)
            return;

        // skip menu window;
        // test case: right click on edit, pop up menu, click it.
        TCHAR e[1024] = {0};
        ::GetClassName(hWnd, e, 1024);
        tm_trace_mouse(L"CDirectMouseInput::Notify(hWnd:%x, %s) check", hWnd, e);
        if (StrCompareI(e, L"#32768") == 0)
            return;

        // skip focus-able and active-able window
//         DWORD dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
//         if (dwStyle & WS_CHILD)
//         {
//             if (dwStyle & WS_TABSTOP)
//                 return;
//         }
//         else
//         {
//             DWORD dwExStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
//             if (!(dwStyle & WS_EX_NOACTIVATE))
//                 return;
//         }

        // skip IME window
        DWORD dwCLStyle = ::GetClassLong(hWnd, GCL_STYLE);
        if (dwCLStyle & CS_IME)
            return;
            
        tm_trace_mouse(L"CDirectMouseInput::Notify(hWnd:%x, %s) notify", hWnd, e);
        ::PostMessage(hWndWgt, WM_CANCELMODE, 0, 0);
    }

    static void Notify(HWND hWndWgt)
    {
        CPoint pt(CMouse::GetCursorPos());
        HWND hWnd = WindowFromPoint(pt);
        NotifyBy(hWndWgt, hWnd);
    }
};

#ifdef DINPUT_MOUSE_HOOK

struct CDirectMouseInput
    : public CMouseInputBase
    , public CThread
{
    typedef HRESULT (WINAPI *PFNDirectInput8Create)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);
    enum { SAMPLE_BUFFER_SIZE = 16 };

    void Start(HTUI hWnd)
    {
        CreateBy(_ThreadProc, hWnd->GetHWND());
    }

    void Close()
    {
        while(IsAlive())
        {
            PostThreadMessage(m_obj.dwID, WM_QUIT, 0, 0);
            Sleep(1);
        }
        Join();
    }

    static TM_DECALRE_THREAD_PROC (_ThreadProc)
    {
        HWND hWnd = (HWND)(pArg);
        CDll dllDInput;
        HRESULT hr;
        CComPtr<IDirectInputDevice8> ptrMouseDevice;

        // create device
        {
            dllDInput.LoadLibrary(TM_T("dinput8.dll"));
            if (!dllDInput)
                return -1;

            PFNDirectInput8Create pfn = (PFNDirectInput8Create)dllDInput.GetProcAddress("DirectInput8Create");
            if (!pfn)
                return -1;

            CComPtr<IDirectInput8> ptrDirectInput;
            if (FAILED(hr = pfn(GetModuleHandle(0), DIRECTINPUT_VERSION,
                IID_IDirectInput8, (void**)&ptrDirectInput, NULL)))
                return hr;
            
            if (FAILED(hr = ptrDirectInput->CreateDevice(GUID_SysMouse, &ptrMouseDevice, NULL)))
                return hr;
        }
        
        // init device
        if (FAILED(hr = ptrMouseDevice->SetDataFormat(&c_dfDIMouse)))
            return hr;
        
        hr = ptrMouseDevice->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
        
        DIPROPDWORD dipdw;
        dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj        = 0;
        dipdw.diph.dwHow        = DIPH_DEVICE;
        dipdw.dwData            = SAMPLE_BUFFER_SIZE;
        if (FAILED(hr = ptrMouseDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
            return hr;
        
        CEvent event;
        ptrMouseDevice->SetEventNotification(event.m_obj);
        ptrMouseDevice->Acquire();

        while (WAIT_OBJECT_0 == MsgWaitForMultipleObjects(1, &event.m_obj, FALSE, INFINITE, QS_ALLEVENTS))
        {
            if (IsMouseDown(ptrMouseDevice))
                Notify(hWnd);
        }

        ptrMouseDevice->Unacquire();
        return 0;
    }
    
    static TMBool IsMouseDown(IDirectInputDevice8* ptrMouse)
    {
        DbgAssert(ptrMouse);

        DIDEVICEOBJECTDATA didod[SAMPLE_BUFFER_SIZE];  // Receives buffered data
        DWORD dwElements = SAMPLE_BUFFER_SIZE;
        HRESULT hr = ptrMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0);
        
        if (hr != DI_OK)
        {
            if (hr == DIERR_INPUTLOST)
                hr = ptrMouse->Acquire();
            return false;
        }
        
        for (DWORD i = 0; i < dwElements; i++)
        {
            switch (didod[i].dwOfs)
            {
            case DIMOFS_BUTTON0:
            case DIMOFS_BUTTON1:
            case DIMOFS_BUTTON2:
            case DIMOFS_BUTTON3:
                if (didod[i].dwData & 0x80)
                    return true;
            }
        }

        return false;
    }
};
typedef CDirectMouseInput CMouseInput;

#else

struct CRawMouseInput
    : public CMouseInputBase
    , public CTuiDialogImpl<CRawMouseInput>
{
    enum { SAMPLE_BUFFER_SIZE = 16 };

    static void InitRID(RAWINPUTDEVICE& rid, DWORD dwFlag, HWND hWnd)
    {
        rid.usUsagePage = 0x01; 
        rid.usUsage = 0x02; 
        rid.dwFlags = dwFlag;   // adds HID mouse and also ignores legacy mouse messages   RIDEV_NOLEGACY RIDEV_INPUTSINK
        rid.hwndTarget = hWnd;
    }

    TMBool Start(HTUI hWnd)
    {
        CTuiWnd* pMgr = hWnd->GetOwner();
        if (!SubclassWindow(pMgr))
            return false;
        DbgAssert(m_hWnd);
        RAWINPUTDEVICE rid;
        InitRID(rid, RIDEV_INPUTSINK, m_hWnd);
        DbgVerify(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
        return true;
    }

    void Close()
    {
        if (!m_hWnd)
            return;
        RAWINPUTDEVICE rid;
        InitRID(rid, RIDEV_REMOVE, 0);
        DbgVerify(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
        UnsubclassWindow();
    }

    TMBool ProcessWindowMessage(TMHWnd hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0)
    {
        if (uMsg == WM_INPUT)
        {
            UINT dwSize=0;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
            RAWINPUT* raw = (RAWINPUT*)alloca(dwSize);
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, raw, &dwSize, sizeof(RAWINPUTHEADER));
            enum { nFlag = RI_MOUSE_LEFT_BUTTON_DOWN | RI_MOUSE_LEFT_BUTTON_UP
                | RI_MOUSE_RIGHT_BUTTON_DOWN | RI_MOUSE_RIGHT_BUTTON_UP };
            if ((raw->header.dwType == RIM_TYPEMOUSE) && (raw->data.mouse.usButtonFlags & nFlag))
                Notify(m_hWnd);
        }
        return false;
    }
};
typedef CRawMouseInput CMouseInput;

#endif // #ifdef DINPUT_MOUSE_HOOK


TM_END_NAMESPACE


#endif // TM_WIN_DESKTOP


#if TM_WIN_DESKTOP

#   include <windows.h>
#   include <imm.h>
#   include "../tmplatform_win.h"

TM_BGN_NAMESPACE



///////////////////////////////////////////////////////////////////////////////
// CFlipChain
struct CFlipChain : public CVniImpl<CFlipChain>
{
    typedef CSet<CRealWindow* > CFlipMap;
    CFlipMap m_aInvalidate;
    CFlipMap m_aFlip;
    
    void Add(CRealWindow* h, HWND hWnd, TMBool bForce)
    {
        DbgAssert(h);
        
        if (!hWnd)
            return;
        if (h->GetLayer()->IsLifeInvalid())
            return;
        if (!bForce && !::IsWindowVisible(hWnd))
            return;
        
        m_aInvalidate.insert(h);
//         int dclk = h->GetLayer()->GetView()->GetNextVBlankTicket() - TimeClock();
        SetAni(eVniPriorityHigh);
    }
    
    void Remove(CRealWindow* h)
    {
        m_aInvalidate.erase(h);
        m_aFlip.erase(h);
    }
    
    void OnAni(...)
    {
        if (!m_aInvalidate.size())
        {
            KillAni();
            return;
        }
        
        m_aFlip.swap(m_aInvalidate);
        for (; m_aFlip.size();)
        {
            CFlipMap::iterator iter = m_aFlip.begin();
            CRealWindow* pWnd = *iter;
            m_aFlip.erase(iter);
            Ref ref(pWnd->GetLayer());
            pWnd->PaintToScreen();
        }
    }
};

static CFlipChain s_aFlip;


///////////////////////////////////////////////////////////////////////////////
// CRealWindow
CRealWindow::~CRealWindow()
{
    s_aFlip.Remove(this);
}

void CRealWindow::AddFlipChain()
{
    MergeNativeInvalidateRect();
    s_aFlip.Add(this, m_hWnd, true);
}

void CRealWindow::MergeNativeInvalidateRect(const TMRect& rc)
{
    TMBool b = m_rcUpdate.Invalidate(rc);
    if (b)
        Edit_SpyInvalidate(0, GetLayer(), &m_rcUpdate.GetDirty(), 0);
}

void CRealWindow::MergeNativeInvalidateRect()
{
    CRectI rcy;
    CTuiLayerWnd* hWgt = GetLayer();
    CZoom wz(hWgt->GetDPI());
//     DbgAssert(hWgt->m_rcDirty.IsDirty());
    CTuiRect rcDirty(hWgt->m_rcDirty.GetDirty());
    rcDirty.OffsetRect(-hWgt->GetLeft(), -hWgt->GetTop());
    wz.LPtoDPCeil(rcDirty, rcy);
    MergeNativeInvalidateRect(rcy);
}

bool CRealWindow::PaintToScreen()
{
    CTuiLayerWnd* p = GetLayer();
    ERenderBK eRet = p->RenderScene(_gxApp);
    if (eRet == eRenderBkOk)
    {
        if (p->Is_MS_PRI_NEEDRESETSIZEOBX())
        {
            CWindowAT(m_hWnd).ModifyStyle(0, WS_SIZEBOX);
            p->Reset_MS_PRI_NEEDRESETSIZEOBX();
        }
        {
            CPaintDC dc(m_hWnd);
            MergeNativeInvalidateRect(dc.m_ps.rcPaint);
        }
        TMBool bFlip = m_pTheme->Present(p, m_hWnd, p->m_txBuffer, m_rcUpdate);// DoFlip();
        if (bFlip)
        {
            TMResult lResult;
            return m_pTheme->DefProc(p, m_hWnd, WM_PAINT, 0, 0, lResult), true;
        }

        // layered window moving, d3d device lost will return false;
        DbgTrace("CRealWindow::PaintToScreen::Flip() error: maybe window is moving or device lost");
        //             DoneNativeInvalidateRectNow(m_hWnd, SPY_INVALIDATE_PARAM(m_pTheme->IsThemeLayered(), this));
        return false;
    }
    
    if (eRet == eRenderBkResize)
    {
        DbgTrace("CRealWindow::PaintToScreen() warning: window id(%d) resize window on paint() ", p->GetObjectID())
            ("dpi:%d width:%g height:%g", p->GetDPI(), p->GetWidth(), p->GetHeight());
        RealChangeFrame(true);
    }

    DbgTrace("CRealWindow::PaintToScreen() warning: window id(%d) error", p->GetObjectID())
        ("dpi:%d width:%g height:%g", p->GetDPI(), p->GetWidth(), p->GetHeight());
    return false;
}

void CRealWindow::UpdateWindow()
{
    s_aFlip.Remove(this);
    MergeNativeInvalidateRect();
    PaintToScreen();
}

bool CRealWindow::SetTheme(EThemeType eType, TMBool bForce)
{
    ITheme* pOldFrame = m_pTheme;
    CTuiLayerWnd* hWgt = GetLayer();
    
    m_pTheme = ITheme::GetThemeCurrent(hWgt, eType);
    if (m_pTheme != pOldFrame || bForce)
    {
        pOldFrame->Enable(hWgt, m_hWnd, false);
        m_pTheme->Enable(hWgt, m_hWnd, true);
        hWgt->ChangeFrame(false);
        m_pTheme->FrameChange(m_hWnd);
        return true;
    }
    return false;
}

CFrame* CRealWindow::GetFrame()
{
    return m_pTheme->IsThemeSys() ? 0 : GetLayer()->CTuiWgt::GetFrame();
}

void CRealWindow::RealUpdateAlpha(TMBool bUpdateNow)
{
    EThemeType type = m_pTheme->GetType();
    if (type != eThemeTypeLayered && type != eThemeTypeDWMTransparent)
    {
        DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
        DWORD dwStyleNew;
        CTuiLayerWnd* hWgt = GetLayer();
        if (hWgt->m_alpha == 255)
            dwStyleNew = dwStyle & ~WS_ADV_LAYERED;
        else
            dwStyleNew = dwStyle | WS_ADV_LAYERED;
        if (dwStyle != dwStyleNew)
            ::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwStyleNew);
        if (hWgt->m_alpha != 255)
            SetLayeredWindowAttributes(m_hWnd, 0, hWgt->m_alpha, LWA_ALPHA);
        return;
    }
    
//     DbgTrace("CRealWindow::UpdateAlpha(%d)", this->GetAlpha());
    if (bUpdateNow)
        UpdateWindow();
}

struct _CRealWindow_ChangFrame : public CWidgetCallT<_CRealWindow_ChangFrame, CTuiLayerWnd*>
{
    static void Done(CTuiLayerWnd* pMgr)
    {
        if (pMgr->Is_WS_PRI_DIRTY() && !pMgr->IsLifeInvalid())
        {
            pMgr->Reset_WS_PRI_DIRTY();
            ITheme::FrameChange(pMgr->GetHWND());
        }
    }
};

void CRealWindow::RealChangeFrame(TMBool bPost)
{
    CTuiLayerWnd* hWgt = GetLayer();
    if (bPost)
    {
        if (hWgt->Is_WS_PRI_DIRTY())
            return;
        hWgt->Set_WS_PRI_DIRTY();
        _CRealWindow_ChangFrame::Call(hWgt);
    }
    else
        ITheme::FrameChange(hWgt->GetHWND());
}

void CRealWindow::RealInvalidate(TMBool bDrawHide, TMBool bUpdate)
{
    // return if window hide.
    if (!bDrawHide && !::IsWindowVisible(this->m_hWnd))
        return;
    
    if (bUpdate)
    {
        this->UpdateWindow();
        return;
    }
    
    CTuiLayerWnd* layer = this->GetLayer();
    DbgAssert(layer);
    //         DbgAssert(layer->m_rcDirty.IsDirty());
    TMUInt interval = layer->GetInterval();
    if ((!interval && m_ani) || (m_ani && m_ani.GetDelay() != interval))
        m_ani.KillAni();
    
    if (interval)
    {
        if (!m_ani)
            m_ani.SetAni(this, interval);
        else
            return;
    }
    
    this->AddFlipChain();
}

void CRealWindow::OnAni(TMHAni, ...)
{
    CTuiLayerWnd* layer = this->GetLayer();
    DbgAssert(layer);
    DbgAssert(!layer->IsPainting());
    if(layer->m_rcDirty.IsDirty())
    {
        this->AddFlipChain();
        m_ani.SetCurrentFrame(0);
    }
    else if (m_ani.GetCurrentFrame() > 30) // auto close if no animation.
        m_ani.KillAni();
}


//////////////////////////////////////////////////////////////////////////////
// CTuiWnd_Win
class CViewWin;

TM_REGISTER_CLASS4(CTuiWnd_Win, TM_CS(TUI_CLASS_WINDOW), false, true);

CAtlThunk CTuiWnd_Win::s_thunkCache;

CTuiWnd_Win::~CTuiWnd_Win()
{
    m_thunk.Init(_WNDPROC_Null, 0);
    s_thunkCache.Swap(m_thunk);
    m_mgrWndProc.ForceClear(m_hWnd);
}

void CTuiWnd_Win::CalcWindowPos(CWndWindowPos& wp, TMUInt32 dwStyle)
{
    DbgAssert(m_posing);
    DbgAssert(&wp == m_posing);

    // max rect on maximize mode
    CRect rcWin;
    rcWin.SetLTWH(wp.x, wp.y, wp.cx, wp.cy);
    TMBool bMaximize = ((dwStyle & (WS_MAXIMIZE | WS_CHILD)) == WS_MAXIMIZE);
    if (bMaximize)
    {
        // get monitor rect.
        HMONITOR hm = MonitorFromRect(&rcWin, MONITOR_DEFAULTTONEAREST);// WIN::GetMonitor(pwp->hwnd);
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfo(hm, &mi);
        
        // caption window can't full-screen
        // auto hide task bar can't show when manager active.
        // XP not support ABM_GETAUTOHIDEBAREX, but MSDN says support.
        TMBool bFullScreen = Is_DS_FULLSCREEN() && ((dwStyle & WS_CAPTION) != WS_CAPTION);
        if (!bFullScreen)
        {
            //                     mi.rcWork.left += 1;
            APPBARDATA abd = { sizeof(APPBARDATA) };
            DWORD dwMessage = ABM_GETAUTOHIDEBAR;
            if (mi.rcMonitor.left != 0 || mi.rcMonitor.top != 0) // not primary monitor
            {
                abd.rc = mi.rcMonitor;
                dwMessage = ABM_GETAUTOHIDEBAREX;
            }
            for (UINT edge = 0; edge <= 3; ++edge)
            {
                abd.uEdge = edge;
                HWND edgeWindow = (HWND)SHAppBarMessage(dwMessage, &abd);
                if (edgeWindow)
                {
                    DbgTrace("find auto hide app bar:%d,0x%X", edge, edgeWindow);
                    int nAdd[] = {1,1,-1,-1};
                    ((LONG*)(&mi.rcWork.left))[edge] += nAdd[edge];
                }
            }
            rcWin = mi.rcWork;
            wp.ResetBound(rcWin);
        }
        else
        {
            if (wp.flags & SWP_NOZORDER)
            {
                wp.hwndInsertAfter = HWND_TOP;
                wp.flags &= ~SWP_NOZORDER;
            }
            rcWin = mi.rcMonitor;
            wp.ResetBound(rcWin);
        }
    }
    
    // calc windows border width;
    CRect rcBorder(rcWin); // sample: 4,28,-4,-4
    this->CallWindowMessageChain(WM_NCCALCSIZE, false, (TMLParam)&rcBorder);
     tm_setmin(rcBorder.bottom, rcWin.bottom);
    rcBorder.DistanceRect(rcWin);

    // make manager context.
    // warning: in sys-theme and full-screen mode, real window not same wp:
    //          wp:0,0,1920,1280 - 1912-1048; real window: -4,-4,1924,1052 - 0,24,1920,1048(1920,1024)
    CZoom zoom(this->m_nDPI);
    long xx = wp.cx - rcBorder.left - rcBorder.right;
    long yy = wp.cy - rcBorder.top - rcBorder.bottom;
    for (int i = 0; i < 100; ++i)
    {
        if (i > 0)
            DbgTrace("CTuiWnd_Win::CalcWindowPos::render(%d, objid:%d, width:%d, height:%d)", i, GetObjectID(), xx, yy);

        CTuiWgt::SetWindowPos(0, 0, 0, zoom.DPtoLP(xx), zoom.DPtoLP(yy), SWP_NOZORDER | SWP_NOACTIVATE);// | SWP_NOSENDCHANGING);
        if (!m_posing)
            return;
        if (Is_WS_PRI_FRAMECHANGED())
            continue;
        xx = zoom.LPtoDP(this->GetWidth());
        yy = zoom.LPtoDP(this->GetHeight());
        if ((CWindowAT(wp.hwnd).IsWindowVisible() || (wp.flags & SWP_SHOWWINDOW)) && !(wp.flags & SWP_HIDEWINDOW))
        {
            ERenderBK b = RenderScene(_gxApp);
            if (!m_posing)
                return;
            if (b == eRenderBkResize)
                continue;
            break;
        }
        break;
    }
    wp.DeltaSize(m_dwLayout, xx - wp.cx, yy - wp.cy);
    CallWindowMessageChain(CM_PRI_POSCHANGING, (TMWParam)&rcBorder, (TMLParam)&wp);

    if (!IsLifeInvalid())
    {
        CRect rcNow;
        CWindowAT wnd(m_hWnd);
        wnd.GetWindowRectToParent(rcNow);
        wp.ResetFlag(rcNow);
        wp.ResetVisible(wnd.GetStyle());
    }

    if (bMaximize)
        DbgTrace("CTuiWnd_Win::CalcWindowPos(bMaximize): %d,%d,%d,%d", wp.x, wp.y, wp.cx, wp.cy);
}

void CTuiWnd_Win::GetWindowRectNow(CRect& rc)
{
    if (m_posing)
        rc.SetLTWH(m_posing.m_pos->x, m_posing.m_pos->y, m_posing.m_pos->cx, m_posing.m_pos->cy);
    else
        CWindowAT(m_hWnd).GetWindowRectToParent(rc);
}

static void _DBG_CopyDC(HDC hdc, HWND hWnd, int nObjid)
{
    CDCHandle dc(hdc);
    CRect rc;
    int n = dc.GetClipBox(&rc);
    CImage img;
    img.CreateIndirect(rc.Width(), rc.Height(), eGXFormat32A);
    if (img)
    {
        {
            CImageDC dc2(img);
            BitBlt(dc2, 0, 0, rc.Width(), rc.Height(), dc, rc.Left(), rc.Top(), SRCCOPY);
        }
        CRect rcW;
        ::GetWindowRect(hWnd, &rcW);
        static int  i = 0;
        CString str;
        str.Format(L"c:/temp/%05d-%05d, rcWin-%d,%d,%d,%d(%d-%d), rcClip-%d,%d,%d,%d(%d-%d).png",
            i++, nObjid, rcW, rcW.Width(), rcW.Height(), rc, rc.Width(), rc.Height());
        ImageSaveFile(img, str, false);
    }
}

LRESULT CALLBACK CTuiWnd_Win::_WNDPROC_Before(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//     DbgTrace(DbgFormatMessage(uMsg));
    CTuiWnd_Win* p = (CTuiWnd_Win*)hWnd;//dwRefData;
    DbgAssert(p->m_pTheme);
    LRESULT lResult = 0;
    if (p->m_pTheme->DefProc(p, p->m_hWnd, uMsg, wParam, lParam, lResult))
        return lResult;
    lResult = DefWindowProc(p->m_hWnd, uMsg, wParam, lParam);
//    lResult = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    return lResult;
}

/* undocumented SWP flags - from SDK 3.1 */
// #define SWP_NOCLIENTSIZE    0x0800
// #define SWP_NOCLIENTMOVE    0x1000
// #define SWP_STATECHANGED    0x8000

LRESULT CALLBACK CTuiWnd_Win::_WNDPROC(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CTuiWnd_Win* p = (CTuiWnd_Win*)hWnd;//dwRefData;
//     DbgAssert(::GetWindowLongPtr(p->m_hWnd, GWL_WNDPROC) == (LONG_PTR)p->m_thunk.GetWNDPROC());

//     DbgTrace(L"CTuiWnd_Win::_WNDPROC(%s)", DbgFormatMessage(uMsg, wParam, lParam).c_str());

    Ref ref(p);
    hWnd = p->m_hWnd;

//      CCopyScreen cs(p, 31212, uMsg);

    LOG_MSG_FOR_SPY(hWnd, p, uMsg, wParam, lParam);

    CAutoMsgAnalysis ama("window", p->GetObjectID(), uMsg, wParam);

    switch (uMsg)
    {
//     case WM_DWMCOMPOSITIONCHANGED:
//         p->Dispatch(WM_DWMCOMPOSITIONCHANGED, 0, 0);
//         break;

    case WM_WINDOWPOSCHANGING:
        {
            ::DefWindowProc(hWnd, uMsg, wParam, lParam);

            if (p->IsLifeInvalid())
                break;

            // sync style minimize/maximize flag.
            TMUInt32 dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
            p->m_dwStyle &= ~(WS_MINIMIZE | WS_MAXIMIZE);
            p->m_dwStyle |= (dwStyle & (WS_MINIMIZE | WS_MAXIMIZE));
            if (dwStyle & WS_MINIMIZE)
                break;

            CWndWindowPos* pwp = (CWndWindowPos*)lParam;
            if (pwp->cx < 0)
                break;

            if (p->Is_WS_PRI_DIRTY())
            {
                p->Reset_WS_PRI_DIRTY();
                pwp->flags |= SWP_FRAMECHANGED;
            }

            if (!p->m_posing && p->m_txBuffer)
            {
                TMBool bKeepSize = (dwStyle & WS_MINIMIZE)
                    || ((pwp->flags & SWP_NOSIZE)
                    && (pwp->flags & SWP_NOMOVE)
                    && !(pwp->flags & SWP_FRAMECHANGED)
                    && !p->Is_WS_PRI_FRAMECHANGED());
                if (bKeepSize)
                    break;
                if (p->m_pTheme->IsThemeSys() && (dwStyle & WS_MAXIMIZE)
                    && p->m_nMaxHeight < 0 && p->m_nMaxWidth < 0
                    && !p->IsAutoHeight() && !p->IsAutoWidth())
                    break;
            }

            p->m_posing.Init(p, pwp);

            // get window rect && preprocessing pwp member
//             CRect rcw;
//             DbgVerify(::GetWindowRect(pwp->hwnd, &rcw));
//             pwp->Init(rcw);


//             DbgTrace("WM_WINDOWPOSCHANGING1: id:%d; x:%d, y:%d, cx:%d, cy:%d, flag:%x, isvisible:%d", p->GetObjectID(), pwp->x, pwp->y, pwp->cx, pwp->cy, pwp->flags, ::IsWindowVisible(hWnd));
            p->CalcWindowPos(*pwp, dwStyle);
            if (!p->m_posing)
            {
                pwp->flags = SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER;
                DbgTrace("WM_WINDOWPOSCHANGING1: id:%d; descard", p->GetObjectID());
                return 0;
            }
            p->m_posing.Release();
//             DbgTrace("WM_WINDOWPOSCHANGING2: id:%d; x:%d, y:%d, cx:%d, cy:%d, flag:%x", p->GetObjectID(), pwp->x, pwp->y, pwp->cx, pwp->cy, pwp->flags);
            if (pwp->IsSize())
                pwp->flags |= SWP_NOCOPYBITS;
        }
        break;

    case WM_WINDOWPOSCHANGED:
        {
            CWndWindowPos* pwp = (CWndWindowPos*)lParam;
            if (!(p->GetStyle() & WS_CHILD) && (p->GetExStyle() & WS_EX_NOACTIVATE))
            {
                if (pwp->IsShow() || pwp->IsHide())
                    p->SetDefaultFocus(pwp->flags);
            }
            if ((pwp->flags & SWP_SHOWWINDOW) || (pwp->flags & SWP_HIDEWINDOW))
                p->SendBroadcastMessage(CM_SHOWWINDOW_P, !(pwp->flags & SWP_HIDEWINDOW), 0, true);
//             if (pwp->IsShow())
//                 p->AddFlipChain(pwp->hwnd, true);
        }
        break;

    case WM_COMMAND:
    case WM_SYSCOMMAND:
//         case WM_INITMENU:
//         case WM_INITMENUPOPUP:
//         case WM_MENUSELECT:
//         case WM_MENUCHAR:
//         case WM_ENTERIDLE:
//         case WM_MENURBUTTONUP:
//         case WM_MENUDRAG:
//         case WM_MENUGETOBJECT:
//         case WM_UNINITMENUPOPUP:
//         case WM_MENUCOMMAND:
        p->SendMessage(uMsg, wParam, lParam);
        return __msgLog.lr = 1;

    case WM_SYSCOLORCHANGE:
        p->Invalidate();
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
        {
            HWND hFocus = ::GetFocus();
            if (hFocus && ::IsChild(p->m_hWnd, hFocus))
                p->m_hWndFocus = hFocus;
            else
                p->m_hWndFocus = 0;
//            DbgTrace("CTuiWnd_Win(%d)::backup focus(HWND:%p)", p->GetObjectID(), p->m_hWndFocus);
        }
        else if (p->m_hWndFocus)
        {
//            DbgTrace("CTuiWnd_Win(%d)::restore focus(HWND:%p)", p->GetObjectID(), p->m_hWndFocus);
            ::SetFocus(p->m_hWndFocus);
        }
        else
        {
//            DbgTrace("CTuiWnd_Win(%d)::restore focus(widget:%d)", p->GetObjectID(), p->GetObjectID());
            p->GetView()->InternalSetFocus(p);
        }
        tm_fallthrough;
    case WM_ACTIVATEAPP:
        {
            HWND hWnd = GetForegroundWindow();
            DWORD pid = 0;
            GetWindowThreadProcessId(hWnd, &pid);
            TMBool bActivateApp = (pid == GetCurrentProcessId());
            g_input.OnActivateApp(bActivateApp);
            if (!bActivateApp)
                g_mgrTooltip.SetToolTip(0, eTTActiveNormal);
        }
        break;

    case WM_SETFOCUS:
        g_input.FocusOnSet(*p);
        p->SetDefaultFocus(SWP_SHOWWINDOW);
        break;

    case WM_KILLFOCUS:
        g_input.FocusOnKill(*p);
        p->SetDefaultFocus(SWP_HIDEWINDOW);
        break;

    case WM_ENABLE:
        p->SendBroadcastMessage(CM_ENABLE_P, wParam, 0, true);
        break;

    // let window show min/max,close,help tooltip
    case WM_NCHITTEST:
        {
            CFrame* pf;
            if (p->m_pTheme->IsThemeSys() || !(pf = p->CTuiWgt::GetFrame()))
                break;
            CPoint pt(lParam);
            TUIHITINFO ht;
            ht.hWnd = p;
            ht.hHint = 0;
            ht.pTaget = 0;
            ht.bNC = false;
            ht.dwCode = 0;
            p->ScreenToClient(&pt, 1, &ht.pt);
            //             DbgTrace(L"WM_NCHITTEST(%g,%g)", ht.pt.x, ht.pt.y);
            LRESULT lResult;
            pf->ProcessFrameMessage(p, CM_HITTEST, 0, (TMLParam)&ht, lResult);
            if (!ht.bNC)
                break;
            if (ht.hid == eHitIDMin || ht.hid == eHitIDMax || ht.hid == eHitIDClose)
                return TUIHITGETOBJ(ht.hid);
        }
        break;
        
    case WM_NCMOUSEMOVE:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
        {
            CFrame* pf;
            if (p->m_pTheme->IsThemeSys() || !(pf = p->CTuiWgt::GetFrame()))
                break;
            if (wParam != HTMINBUTTON && wParam != HTMAXBUTTON && wParam != HTCLOSE)
                break;
            if (uMsg == WM_NCMOUSEMOVE)
                WIN::TrackMouseLeave(hWnd, false);
            uMsg += (WM_MOUSEMOVE - WM_NCMOUSEMOVE);
            CPoint pt(lParam);
            CWindowAT(hWnd).ScreenToClient(&pt);
            lParam = pt.ToLPARAM();
            wParam = 0;
            if ((GetAsyncKeyState(VK_LBUTTON) & 0xFF00) > 0) wParam |= MK_LBUTTON;
            if ((GetAsyncKeyState(VK_MBUTTON) & 0xFF00) > 0) wParam |= MK_MBUTTON;
            if ((GetAsyncKeyState(VK_RBUTTON) & 0xFF00) > 0) wParam |= MK_RBUTTON;
            //             DbgTrace(L"WM_NCMOUSEMOVE(%d,%d)", pt.x, pt.y);
        }
        break;

    case WM_MOUSEMOVE:
        WIN::TrackMouseLeave(hWnd, true);
        break;

    // eat mouse up message on menu pop.
    case WM_RBUTTONUP:
    case WM_NCRBUTTONUP:
        {
            GUITHREADINFO gti = { sizeof(GUITHREADINFO) };
            DbgVerify(GetGUIThreadInfo(0, &gti));
            if (gti.flags & GUI_INMENUMODE)
                return 0;
        }
        break;

//     case WM_INPUTLANGCHANGE:
//     case WM_INPUTLANGCHANGEREQUEST:
//     case WM_IME_SETCONTEXT:
//         tm_trace_ime(L"hWnd:%p, %s", p->m_hWnd, DbgFormatMessage(uMsg, wParam, lParam).c_str());
//         break;

//    case WM_INPUTLANGCHANGE:
    case WM_IME_STARTCOMPOSITION:
        g_input.ImeOnStartCompostion();
        WIN::ImeOnStartCompostion();
        tm_trace_ime("WM_IME_STARTCOMPOSITION(hWnd:%p, wp:0X%X, lp:0X%X, bKorea:%d)", p->GetHWND(), wParam, lParam, g_input.m_bKorea);
        if (g_input.m_bKorea) // defproc will show composition window
            return 0; //lParam &= ~ISC_SHOWUICOMPOSITIONWINDOW;
        break;

    case WM_IME_COMPOSITION:
        tm_trace_ime("WM_IME_COMPOSITION(hWnd:%p, wp:0X%X, lp:0X%X)", p->GetHWND(), wParam, lParam);
        WIN::ImeOnComposition(p, wParam, lParam);
        break;

    default:
        break;
    }

    TMBool b = g_input.Gesture().ProcessMessage(p, uMsg, wParam, lParam);
    if (b)
        return 0;

    return __msgLog.lr = p->CallWindowMessageChain(uMsg, wParam, lParam);
}

TMResult CTuiWnd_Win::CallWindowMessageChain(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    TMResult lr = 0; 
    if (GetView()->DoPreTranslateAppMessage(this, m_hWnd, uMsg, wParam, lParam, lr))
        return lr;
    return m_mgrWndProc.SendProcMessage(m_hWnd, uMsg, wParam, lParam);
}

// center window logic:
// child: center by parent, don't check screen area
// owner: center by owner, check screen area
// screen: center by screen, don't check screen area
// nType:0, DS_CENTEROWNER, DS_CENTERSCREEN
static void CenterDialog(HWND hWnd, TMUInt32 style)
{
    if (!(style & (DS_CENTEROWNER | DS_CENTERSCREEN)))
        return;

    DbgAssert(::IsWindow(hWnd));

      CRect rcDlg;
    ::GetWindowRect(hWnd, &rcDlg);

    // determine owner window to center against
    CRect rcCenter;
    MONITORINFO mi = { sizeof(mi) };
    if (style & WS_CHILD)
    {
        HWND hParent = ::GetParent(hWnd);
        DbgAssert(::IsWindow(hParent));
        ::GetClientRect(hParent, &rcCenter);
    }
    else
    {
        HWND hOwner = ::GetWindow(hWnd, GW_OWNER);
        HMONITOR hm = MonitorFromWindow(hOwner, MONITOR_DEFAULTTOPRIMARY);
        GetMonitorInfo(hm, &mi);
        if ((style & DS_CENTEROWNER) && hOwner)
            ::GetWindowRect(hOwner, &rcCenter);
        else
            rcCenter = mi.rcWork;
    }

    int DlgWidth = rcDlg.Width();
    int DlgHeight = rcDlg.Height();

    // find dialog's upper left based on rcCenter
    int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
    int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

    // if the dialog is outside the screen, move it inside
    if (style & DS_CENTEROWNER)
    {
        if (xLeft < mi.rcWork.left)
            xLeft = mi.rcWork.left;
        else if (xLeft + DlgWidth > mi.rcWork.right)
            xLeft = mi.rcWork.right - DlgWidth;

        if (yTop < mi.rcWork.top)
            yTop = mi.rcWork.top;
        else if (yTop + DlgHeight > mi.rcWork.bottom)
            yTop = mi.rcWork.bottom - DlgHeight;
    }

    // map screen coordinates to child coordinates
    ::SetWindowPos(hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

HTUI CTuiWnd_Win::ManagerInit(TUIFACTORY* pFactory, TMHModule hInst, TMHWnd hWnd, void* param, TMBool bShowWindow, const CRect* prc)
{
    DbgAssert(m_nRef > 0);

    // init
    m_hWnd = hWnd;
    WIN::SetWinProp(m_hWnd, WIN_PROP_TYPE_MGR);
    WIN::ImeEnable(this, false); // can't init IME if remove the line.
    CenterDialog(m_hWnd, m_dwStyle);
    RegisterDragDrop(m_hWnd, &m_drop);
    //m_mgrWndProc.Subclass(m_hWnd, this, this);
    m_dwStyle |= WS_VISIBLE;
//     Set_WS_PRI_DIRTY();

    // frame
    DbgAssert(!m_pTheme);
    m_pTheme = ITheme::GetThemeCurrent(this);
    DbgAssert(m_pTheme);
    m_pTheme->Enable(this, m_hWnd, true);

    // proc
    m_procOld = (WNDPROC)::GetWindowLongPtr(m_hWnd, GWLP_WNDPROC);
    m_thunk.Init(_WNDPROC_Before, this);
    ::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_thunk.GetWNDPROC());
    // set windows frame border width = 0 or default after set m_pTheme;
    if ((m_dwAdvStyle & WS_ADV_LAYERED) && !bShowWindow)
    {
        // window bug: SWP_FRAMECHANGED unused for hide layered window. so need resize.
        CRect rc;
        if (!prc)
        {
            prc = &rc;
            ::GetWindowRect(m_hWnd, &rc);
        }
        enum { eFlag = SWP_NOZORDER | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOACTIVATE };
        DbgVerify(::SetWindowPos(m_hWnd, 0, 0, 0, prc->Width(), prc->Height() + 1, eFlag));
        DbgVerify(::SetWindowPos(m_hWnd, 0, 0, 0, prc->Width(), prc->Height(), eFlag));
    }
    else if (prc)
        DbgVerify(::SetWindowPos(m_hWnd, 0, 0, 0, prc->Width(), prc->Height(), SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOACTIVATE));
    else
        DbgVerify(::SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOACTIVATE));
    m_thunk.Init(_WNDPROC, this);
    ::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_thunk.GetWNDPROC());

    Subclass_wself();

    // every step maybe destroy by user, so add if (m_hWnd)... every step
    DbgAssert(::IsWindow(m_hWnd));
    if (!CTuiWnd::InitManager(pFactory, param))
        return 0;

    DbgAssert(::IsWindow(m_hWnd));
    ::SendMessage(m_hWnd, WM_CHANGEUISTATE, MAKEWPARAM(UIS_INITIALIZE, NULL), 0);
    if (IsLifeDestroyed())
        return 0;

#ifdef TM_DBG
    CRect rcw,rcc;
    ::GetWindowRect(m_hWnd, &rcw);
    ::GetClientRect(m_hWnd, &rcc);
    EThemeType eType = m_pTheme->GetType();
    if (eType == eThemeTypeLayered || eType == eThemeTypeDWM || eType == eThemeTypeDWMBlur)
        DbgAssert(rcw.Width() == rcc.Width() && (rcw.Height() - rcc.Height() <= 1));
#endif

    GetReal()->RealUpdateAlpha(false);
//     if (::GetWindowLong(m_hWnd, GWL_STYLE) & WS_VISIBLE)
//          AddFlipChain();

    DbgAssert(!IsLifeDestroyed());
   return this;
}

TMResult CTuiWnd_Win::Dispatch(TMUInt msg, TMWParam wp, TMLParam lp)
{
    if (msg == WM_DWMCOMPOSITIONCHANGED) // call when CTuiApp::SetUseSystemFrame
    {
        DbgAssert(m_pTheme);
//        SetThemeFrame(eThemeTypeAuto);
        CallWindowMessageChain(WM_DWMCOMPOSITIONCHANGED, wp, lp);
    }
    else
        if (msg == CM_LANGUAGECHANGED)
    {
        WIN::SetRTLOnLanguageChanged(m_hWnd, lp);
        Invalidate();
    }

    return CTuiWnd::Dispatch(msg, wp, lp);
}

///////////////////////////////////////////////////////////////////////////////
// CViewWin
CViewWin::CViewWin()
{
    //     gxInit();
    //         SetHighContrast();
}

CViewWin::~CViewWin()
{
    //     gxTerm();
}

void CTuiWgt::OSSetCursor(CCursorAT::cursor_id nID)
{
    HCURSOR hCursor = LoadCursor(0, nID);
    ::SetCursor(hCursor);
}

TMBool WIN::TrackMouseLeave(HWND hWnd, TMBool bClient)
{
    const DWORD _TME_NONCLIENT = 0x00000010;

    TRACKMOUSEEVENT tm = { sizeof(tm), TME_LEAVE | TME_QUERY, 0 };
    DbgVerify(::TrackMouseEvent(&tm));
    //    DbgTrace("TrackMouseLeave() query:%p", tm.hwndTrack);
    tm.dwFlags &= (TME_LEAVE | _TME_NONCLIENT);

    DWORD flag = bClient ? TME_LEAVE : (TME_LEAVE | _TME_NONCLIENT);
    if (hWnd == tm.hwndTrack && flag == tm.dwFlags)
        return true;

//    DbgTrace("WIN::TrackMouseLeave(hOld:%p, hNew:%p, %d)", tm.hwndTrack, hWnd, bClient);
    
    // remove old
    tm.dwFlags |= TME_CANCEL;
    TMBool b;
    DbgVerify(b = ::TrackMouseEvent(&tm));

    // maybe leave message have been send.
    TMBool bb = RemoveMesssage(tm.hwndTrack, WM_NCMOUSELEAVE, WM_MOUSELEAVE);

    // new track
    tm.hwndTrack = hWnd;
    tm.dwFlags = flag;
    DbgVerify(b = ::TrackMouseEvent(&tm));

    return b;
}

TMBool WIN::RemoveMesssage(TMHWnd hWnd, TMUInt uMsgMin, TMUInt uMsgMax)
{
//     DbgAssert(hWnd);
    MSG msg;
    TMBool b;
    b = ::PeekMessage(&msg, hWnd, uMsgMin, uMsgMax, PM_REMOVE);
    return b;
}

void WIN::SetRTLOnLanguageChanged(HWND hWnd, TMLParam lp)
{
    DbgAssert(hWnd);

    TMUInt32 dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
    TMUInt32 dwNew;
    TMBool bRTL = CLocal(lp).IsRTL();
    if (bRTL)
        dwNew = dwExStyle | WS_EX_LAYOUTRTL;
    else
        dwNew = dwExStyle & ~WS_EX_LAYOUTRTL;
    if (dwNew != dwExStyle)
        ::SetWindowLong(hWnd, GWL_EXSTYLE, dwNew);
}

TMBool WIN::IsAllStyle(HWND hWnd, TMUInt32 dwStyle, TMUInt32 dwMask)
{
    DbgAssert(hWnd);
    for (; hWnd; hWnd = (HWND)::GetWindowLongPtr(hWnd, GWLP_HWNDPARENT))
    {
        DWORD dwStyleNow = ::GetWindowLong(hWnd, GWL_STYLE);
        if ((dwStyleNow & dwMask) != dwStyle)
            return false;
        if (!(dwStyleNow & WS_CHILD))
            return true;
    }
    return true;
}

TMResult WIN::OnMouseActivate(HWND hWnd, HWND hOwner, TMUInt uMsg, TMWParam wp, TMLParam lp)
{
    DbgAssert(::GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE);
//     DbgAssert(::GetAncestor(hWnd, GA_ROOTOWNER) == hOwner);
    if (hOwner != hWnd)
    {
        TMResult lr = ::SendMessage(hOwner, uMsg, wp, lp);
        if (lr == MA_ACTIVATE || lr == MA_ACTIVATEANDEAT)
        {
            DECLARE_DLL_FUNC(dll_user32, BOOL, WINAPI, AllowSetForegroundWindow, (DWORD dwProcessId));
            DbgVerify(_AllowSetForegroundWindow(GetCurrentProcessId()));
            DbgVerify(::SetForegroundWindow(hOwner));
        }
    }
    return MA_NOACTIVATE;
}


TMBool WIN::OnGetObject(TM::HTUI hWgt, TMWParam wParam, TMLParam lParam, TMResult& lr)
{
#ifdef _USRDLL

    struct CDllProxyFN
    {
        fnGetAccObject pFnGetObject;

        CDllProxyFN()
        {
            CDllHandle dllRef;

#if defined(TM_DBG) && defined(TM_WIN64)        // debug tool
            dllRef.LoadLibraryFromIf(TUI_ACC_DLL64, _ModuleATL.GetModuleInstance());
            if (!dllRef)
                dllRef.LoadLibraryFromIf(TUI_ACC_DLL, _ModuleATL.GetModuleInstance());
#elif defined(FOR_PT) && defined(TM_WIN64)      // PT
            dllRef.LoadLibraryFromIf(TUI_ACC_DLL64, _ModuleATL.GetModuleInstance());
#else                                           // other
            dllRef.LoadLibraryFromIf(TUI_ACC_DLL, _ModuleATL.GetModuleInstance());
#endif
            
            if (dllRef)
                pFnGetObject = (fnGetAccObject)dllRef.GetProcAddress("GetAccObject");
            else
                pFnGetObject = 0;
        }
    };

    static CDllProxyFN s_fn;

    if (s_fn.pFnGetObject)
        return s_fn.pFnGetObject(hWgt, wParam, lParam, lr);
    return false;

#else   

    return LibGetAccObject(hWgt, wParam, lParam, lr);

#endif // _USRDLL
}

void WIN::SetWinProp(HWND hWnd, int nType)
{
    SetProp(hWnd, WIN_PROP_TYPE, (HANDLE)nType);
}

TMLParam WIN::GetWinPropType(TMHWnd hWnd)
{
    return (TMLParam)::GetProp(hWnd, WIN_PROP_TYPE);
}

TMBool WIN::HookGlobalMouseDown(HTUI hWnd)
{
    static CMouseInput di;
    if (hWnd)
    {
        DbgAssert(!di);
        return di.Start(hWnd);
    }
    else
        di.Close();
    return true;
}

static BOOL _WIN_ImmAssociateContextEnable(HWND hWnd)
{
    // may be call 3rd dll exception.
    TM_TRY_ALL(return ImmAssociateContextEx(hWnd, 0, IACE_DEFAULT));
    return false;
}

void WIN::ImeEnable(CTuiWnd* mgr, TMBool bEnable)
{
    tm_trace_ime("ImeEnable(%p, %d)", mgr->GetHWND(), bEnable);
    if (bEnable)
    {
        DbgAssert(::GetFocus() == mgr->GetHWND());
        DbgVerify(_WIN_ImmAssociateContextEnable(mgr->GetHWND()));
    }
    else
        { DbgVerify(ImmAssociateContext(mgr->GetHWND(), 0)); }
}

void WIN::ImeMoveTo(CTuiWnd* mgr, HTUI hCaret, const CRect& rc)
{
    if (g_input.m_bKorea) // only for korean.
        return;

    COMPOSITIONFORM compFrom;
    compFrom.rcArea = rc;
    compFrom.ptCurrentPos.x = compFrom.rcArea.left;
    compFrom.ptCurrentPos.y = compFrom.rcArea.top;
    HWND hWndFoucs = mgr->GetHWND();
    MapWindowPoints(hCaret->GetHWND(), hWndFoucs, &compFrom.ptCurrentPos, 1);
    
    HIMC hIMC = ::ImmGetContext(hWndFoucs);

    // reset font
    LOGFONT lf;
    DbgVerify(ImmGetCompositionFont(hIMC, &lf));
    LONG lfHeight = -(compFrom.rcArea.bottom - compFrom.rcArea.top);
    if (lf.lfHeight != lfHeight)
    {
        lf.lfHeight = lfHeight;
        lf.lfWidth = 0;
        lf.lfFaceName[0] = 0;
        DbgVerify(ImmSetCompositionFont(hIMC, &lf));
    }

    // update composition window position
    compFrom.dwStyle = CFS_POINT;
    DbgVerify(::ImmSetCompositionWindow(hIMC, &compFrom));
    tm_trace_ime("ImmSetCompositionWindow(hwnd:%p, pos:%d,%d)", hWndFoucs, compFrom.ptCurrentPos);
    
// not all IME use composition position
//     DbgVerify(CreateCaret(hWndFoucs, 0, 0, compFrom.rcArea.bottom - compFrom.rcArea.top));
//     DbgVerify(SetCaretPos(compFrom.ptCurrentPos.x, compFrom.ptCurrentPos.y));

    DbgVerify(::ImmReleaseContext(hWndFoucs, hIMC));
}

void WIN::ImeOnStartCompostion()
{
    HKL hkl = GetKeyboardLayout(0);
    if (ImmIsIME(hkl))
    {
        if (PRIMARYLANGID(LOWORD(HandleToUlong(hkl))) == LANG_KOREAN)
        {
            tm_trace_ime("WIN::ImeOnSetcontext() bKorea = true");
            g_input.m_bKorea = true;
            return;
        }
    }
    tm_trace_ime("WIN::ImeOnSetcontext() bKorea = false");
    g_input.m_bKorea = false;
}

void WIN::ImeOnComposition(CTuiWnd* mgr, TMWParam wp, TMLParam& lp)
{
    if (!g_input.m_bKorea || !g_input.m_hCaret) // only for korean.
        return;

    if (lp & (GCS_RESULTSTR | CS_INSERTCHAR))
    {
        HIMC himc = ImmGetContext(mgr->GetHWND());
        if (!himc)
            return;
        
        TCHAR c[256];
        CTuiEdit edit(g_input.m_hCaret);
        if (lp & GCS_RESULTSTR)
        {
            int dwLen = ImmGetCompositionString(himc, GCS_RESULTSTR, c, 256);
            c[dwLen / sizeof(TCHAR)] = 0;
            DISABLE_SPY();
            edit.ReplaceSel(c);
            lp &= ~GCS_RESULTSTR;
            tm_trace_ime(L"WIN::ImeOnComposition(hWnd:%p) result str:%s", mgr->GetHWND(), c);
        }
        else
        {
            int dwLen = ImmGetCompositionString(himc, GCS_COMPSTR, c, 256) / sizeof(TCHAR);
            c[dwLen] = 0;
            TMSel0 n1 = 0, n2;
            DISABLE_SPY(); edit.GetSel(n1, n2);
            DISABLE_SPY(); edit.ReplaceSel(c);
            DISABLE_SPY(); edit.SetSel(n1, n1 + dwLen);
            tm_trace_ime(L"WIN::ImeOnComposition(hWnd:%p) comp str:%s", mgr->GetHWND(), c);
        }
        
        DbgVerify(ImmReleaseContext(mgr->GetHWND(), himc));
    }
}

///////////////////////////////////////////////////////////////////////////////
// CViewWin
TMBool CViewWin::InternalIsWindowActive(const CTuiWgt* hWgt)
{
    return GetActiveWindow() == hWgt->GetHWND();
}

CPoint CViewWin::InternalGetScreenOrg(HTUI hWgt)
{
    CPointI pt(0,0);
    DbgVerify(::ClientToScreen(hWgt->GetHWND(), &pt));
    return pt;
}

static int _CViewWin_GetDPI(HWND hWnd, TMPoint* ptMonitor)
{
    CMonitor monitor(CMonitor::FromWindow(hWnd));
    if (ptMonitor)
    {
        CRect rc;
        monitor.GetRect(&rc, 0);
        *ptMonitor = rc.TopLeft();
    }
    if (!CAppAT::IsDPIAware())
        return TUI_DPI_SYSTEM_DEFAULT;
    return monitor.GetDPI();
}

HTUI CViewWin::CreateDlgEx (TMHModule hInst, int nID, TMHWnd hWndParent, TUIFACTORY* pFactory, void* param)
{
    TMPoint ptMonitor = {0};
    CTuiWnd* pMgr = CreateMgr(hInst, nID, _CViewWin_GetDPI(hWndParent, &ptMonitor));
    if (!pMgr)
        return 0;

    // create window
    TMUInt32 dwStyle = pMgr->GetStyle();
    TMUInt32 dwExStyle = pMgr->GetExStyle();
    TMBool bVisible = dwStyle & WS_VISIBLE;
    
    CRect rc;
    CTuiZoom zoom(pMgr->GetDPI());
    zoom.LPtoDP(pMgr->GetBoundRect(), rc);

    struct Register
    {
        static ATOM Exec1 ()
        {
            WNDCLASSEX  wc = 
            {
                sizeof (WNDCLASSEX),
                    CS_DBLCLKS | CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW,// | CS_OWNDC,// | CS_OWNDC,
                    ::DefWindowProc, 0, 0, 0, 0,
                    ::LoadCursor(0, IDC_ARROW),
                    0, //(HBRUSH)GetStockObject(NULL_BRUSH),
                    0, TUI_CLASS_WIN_MAIN, 0
            };
            
            return RegisterClassEx (&wc); 
        }
    };
    
    static ATOM atom1 = Register::Exec1();
    const TMUInt32 WINDOW_CREATE_STYLE_MASK     = WS_CHILD | WS_DISABLED;// /*| WS_CAPTION  | WS_THICKFRAME | WS_BORDER*/ | WS_SYSMENU | WS_GROUP | WS_TABSTOP;//  0x58CF0000
    const TMUInt32 WINDOW_CREATE_EXSTYLE_MASK   = WS_ADV_NOACTIVATE | WS_EX_ACCEPTFILES | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW;// | WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE; // WS_ADV_LAYERED | 0x080806A8

    if (dwStyle & WS_CHILD)
        ptMonitor.x = ptMonitor.y = 0;
    HWND hWnd = ::CreateWindowEx(
        (dwExStyle & WINDOW_CREATE_EXSTYLE_MASK),// | WS_EX_NOREDIRECTIONBITMAP,
        //L"#32770", 
        //WC_TREEVIEW, 
        TUI_CLASS_WIN_MAIN,
        pMgr->GetWindowText().c_str(),
        (dwStyle & WINDOW_CREATE_STYLE_MASK),
        ptMonitor.x, ptMonitor.y, rc.Width(), rc.Height(), hWndParent, 0, hInst, 0);
    DbgAssert(!::IsWindowVisible(hWnd));
    if (!hWnd)
    {
        pMgr->UnInitOnManagerFail();
        return 0;
    }

    CTuiWnd_Win* p = static_cast<CTuiWnd_Win*>(pMgr);
    Ref ref(pMgr);
    if (!p->ManagerInit(pFactory, hInst, hWnd, param, bVisible, &rc))
        return 0;

    if (bVisible && !(::GetWindowLong(hWnd, GWL_STYLE) & WS_VISIBLE))
    {
        DbgAssert(::IsWindow(hWnd));
        TMUInt uFlag = SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW;
        if (WS_ADV_NOACTIVATE & p->m_dwAdvStyle)
            uFlag |= SWP_NOACTIVATE;
        DbgVerify(::SetWindowPos(hWnd, 0, 0, 0, 0, 0, uFlag)); 
        if (p->IsLifeDestroyed())
            return 0;
    }

    return p;
}

HTUI CViewWin::CreateDlgByHWND(TMHModule hInst, int nID, TMHWnd hWnd, TUIFACTORY* pFactory, void* param)
{
    CTuiWnd* pMgr = CreateMgr(hInst, nID, _CViewWin_GetDPI(hWnd, 0));
    if (!pMgr)
        return 0;
    
    CTuiWnd_Win* p = static_cast<CTuiWnd_Win*>(pMgr);
    Ref ref(pMgr);
    if (!p->ManagerInit(pFactory, hInst, hWnd, param, false, 0))
        return 0;

    return p;
}

void CTuiWnd_Win::TrackSysMenu(TMLParam lParam, TMPoint* pt)
{
    TMHWnd hWnd = GetHWND();
    TMHMenu hm = GetSystemMenu (hWnd, true);
    TMHMenu hMenu = 0;
    if (!hm)
    {
        TMHModule hUser = DllHandle(TM_T("user32.dll"));
        hMenu = LoadMenu (hUser, MAKEINTRESOURCE(16));
        hm = GetSubMenu (hMenu, 0);
    }
    
    TMPoint ptx;
    if (pt)
        ptx = *pt;
    else
    {
        const CTuiRect& rc = GetClientRect();
        ptx.x = rc.Left();
        ptx.y = rc.Top();
        DbgVerify(::ClientToScreen (hWnd, &ptx));
    }
    
    TraceMenu tm;
    tm.InitSysMenu (this, hm);
    const TMUInt uAlignFlags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_VERTICAL | TPM_RETURNCMD;
    TMUInt uID = GetModule()->TrackPopupMenu(hm, uAlignFlags, ptx.x, ptx.y, hWnd);
    
    if (hMenu)
        DbgVerify(DestroyMenu (hMenu));
    
    if (uID & 0xf000) // syscommand
        DbgVerify(::PostMessage (hWnd, WM_SYSCOMMAND, (uID & 0xfff0), lParam));
    
    WIN::RemoveMesssage(hWnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN);
}



TMBool CTuiWnd_Win::ProcessWindowMessage(TMHWnd hWnd, TMUInt uMsg, TMWParam wParam, 
                                            TMLParam lParam, TMResult& lResult,
                                            TMULong dwMsgMapID)
{
    return _ProcessMgrNativeMessage(*this, hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
}

TMBool CTuiWnd_Win::ProcessWindowMessage(CTuiWgt* hWnd, TMUInt uMsg, TMWParam wParam,
                            TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    switch (uMsg)
    {
    case WM_SETTEXT:
    case CM_SETTEXT:
        {
            CTuiWnd::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
            CGXStrPtr str(m_gxText.GetString());
            DbgVerify(::SetWindowText(m_hWnd, str.c_str()));
        }
        return 0;

    case WM_STYLECHANGED:
        {
            enum { eStyle = WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | DS_NODWMSHADOW | DS_DWMBLUR | DS_NODWMTRANSPARENT };
            enum { eExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_ADV_NOACTIVATE | WS_EX_LAYERED };
            STYLESTRUCT* p = (STYLESTRUCT*)lParam;
            DWORD dwXor = (p->styleNew ^ p->styleOld);
            if ((wParam == GWL_STYLE && (dwXor & eStyle)) ||
                (wParam == GWL_EXSTYLE && (dwXor & eExStyle)))
                SetTheme(eThemeTypeAuto, true);
            if (wParam == GWL_STYLE && (dwXor & DS_FULLSCREEN) && (m_dwStyle & WS_MAXIMIZE))
                ITheme::FrameChange(m_hWnd);
        }
        break;

//     case CM_SETSCROLLRANGE:
//         CTuiWnd::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
//         if (lResult)
//             this->SetDirtyAndInvalidate();
//         return true;

    case CM_COLORVISUAL:
        Invalidate();
        break;

    case WM_DPICHANGED:
        {
#ifdef TM_DBG
            // check is system Infinite loop call "WM_DPICHANGED".
            typedef std::pair<DWORD,int> CALLINFO; // first call time, call count
            typedef CMap<HTUI, CALLINFO> CDpiHistroyList; // HTUI, CALLINFO
            static CDpiHistroyList s_aHistroy;
            static DWORD s_nFirstTime;
            DWORD now = GetTickCount();
            enum { eMaxInterval = 2000 };
            enum { eMaxCallCount = 20 };
            if (now - s_nFirstTime > eMaxInterval)
            {
                s_aHistroy.clear();
                s_nFirstTime = now;
            }
            CALLINFO& cf = s_aHistroy[this];
            if (now - cf.first > eMaxInterval)
            {
                cf.first = now;
                cf.second = 0;
            }
            DbgAssert(cf.second <= eMaxCallCount);
            ++cf.second;
#endif

            int x = LOWORD(wParam);
            int nOldDPI = GetDPI();
            if (x != nOldDPI)
                SendMessage(CM_SETDPI, x);

            if (lParam)
            {
                CRect* rc = (CRect*)lParam;
// #ifdef TM_DBG
//                 CMonitor monitor1(CMonitor::FromWindow(m_hWnd));
//                 CMonitor monitor2(CMonitor::FromRect(*rc));
//                 DbgAssert(monitor1 == monitor2);
//                 RECT rcOld;
//                 ::GetWindowRect(m_hWnd, &rcOld);
// #endif
                DbgVerify(::SetWindowPos(m_hWnd, 0, rc->left, rc->top, rc->Width(), rc->Height(),
                    SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING));
// #ifdef TM_DBG
//                 RECT rcNew;
//                 ::GetWindowRect(m_hWnd, &rcNew);
//                 CMonitor monitor3(CMonitor::FromWindow(m_hWnd));
//                 CMonitor monitor4(CMonitor::FromRect(rcNew));
//                 DbgAssert(monitor1 == monitor4);
//                 DbgTrace("WM_DPICHANGED: oid:%d; olddpi:%d; oldRect:%d,%d,%d,%d; newdpi:%d; rect:%d,%d,%d,%d; afterrect:%d,%d,%d,%d", GetObjectID(), nOldDPI, rcOld, x, *rc, rcNew);
// #endif
            }
        }
        break;

    case WM_GETDPISCALEDSIZE:
        {
            SIZE* p = (SIZE*)lParam;
            GXSize sz(GetWidth(), GetHeight());
            CTuiZoom(wParam).LPtoDP(sz, *p);
            lResult = true;
            return true;
        }
        break;

    case WM_SETTINGCHANGE:
        GetView()->OnSettingChanged();
        break;

//     case WM_DISPLAYCHANGE:
//         GetView()->OnDisplayChanged();
//         break;

    case WM_GETMINMAXINFO:
        ::DefWindowProc (m_hWnd, uMsg, wParam, lParam);
        {
            MINMAXINFO* p = (MINMAXINFO*)lParam;
            CTuiZoom zoom(GetDPI());

            // min
            if (m_nMinWidth >= 0)
                p->ptMinTrackSize.x = zoom.LPtoDP(m_nMinWidth);
            if (m_nMinHeight >= 0)
                p->ptMinTrackSize.y = zoom.LPtoDP(m_nMinHeight);

            // max
            if (m_nMaxWidth >= 0)
                p->ptMaxTrackSize.x = p->ptMaxSize.x = zoom.LPtoDP(m_nMaxWidth);
            if (m_nMaxHeight >= 0)
                p->ptMaxTrackSize.y = p->ptMaxSize.y = zoom.LPtoDP(m_nMaxHeight);
        }
        return 0;

    case WM_COMMAND:
        // through
    case WM_INITDIALOG:
    case CM_MSGROUTE:
//    case CM_SETALPHA:
    case CM_MSGBUBBLE:
        return lResult = CallWindowMessageChain(uMsg, wParam, lParam);

    case CM_GET_THEME_TYPE:
        lResult = m_pTheme->GetType();
        return true;

    case CM_SET_THEME_TYPE:
        return true;

    case CM_RENDERCHANGED:
        Invalidate();
        CTuiWnd::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
        return CallWindowMessageChain(uMsg, wParam, lParam);

    case CM_LANGUAGECHANGED:
        WIN::SetRTLOnLanguageChanged(m_hWnd, lParam);
        Invalidate();
        return CallWindowMessageChain(uMsg, wParam, lParam);

    case WM_SYSCOMMAND:
        wParam = GetModule()->GetLocal().ConvertSC(wParam);
        return CallWindowMessageChain(uMsg, wParam, lParam);

    //   XP mode: maximize window move caption will do nothing
    // win7 mode: maximize window move caption will restore window first
    case WM_NCLBUTTONDOWN:
        {
            EHitID hidNew = GetView()->GetHitInfo().hid;
            if (hidNew == eHitIDCaption)
            {
                lResult = m_procOld(m_hWnd, uMsg, HTCAPTION, lParam);
//                lResult = DefSubclassProc(m_hWnd, uMsg, HTCAPTION, lParam);
                return true; //OnNcLButtonDown(lParam);
            }
        }
        break;

    case WM_NCDESTROY:
        if (!(m_dwPriStyle & MS_PRI_IS_DESTROYWIN))
            ::DestroyWindow(m_hWnd);
        break;
    }

    return CTuiWnd::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
}

TMBool CTuiWnd_Win::SetWindowPos(CTuiWgt* pInsertAfter, GXDips x, GXDips y, GXDips cx, GXDips cy, TMUInt nFlags)
{
    if (IsLifeInvalid())
        return false;
    
    if (nFlags & SWP_NOSIZE)
    {
        cx = m_rcWnd.Width();
        cy = m_rcWnd.Height();
    }
    if (nFlags & SWP_NOMOVE)
    {
        x = m_rcWnd.Left();
        y = m_rcWnd.Top();
    }

    CTuiRect rcNew;
    rcNew.SetLTWH(x, y, cx, cy);
    CRect rcNewW;
    ClientToScreen(rcNew, rcNewW);

    CRect rcOldW;
    ClientToScreen(m_rcWnd, rcOldW);
    rcOldW.DistanceRect(rcNewW);
    
    CRect rcNowW;
    GetWindowRectNow(rcNowW);
    rcNowW.DistanceRect(rcOldW);

    nFlags &= (SWP_SHOWWINDOW | SWP_HIDEWINDOW);
    return ::SetWindowPos(m_hWnd, 0, rcNowW.Left(), rcNowW.Top(), rcNowW.Width(), rcNowW.Height(),
        nFlags | SWP_NOZORDER | SWP_NOACTIVATE);
}

    // CTuiLayerImpl
void CTuiWnd_Win::CreateBkBuffer(IGXApp* gapp, const CSize& sz)
{
    m_txBuffer.CreateSwapChain(m_hWnd, sz.cx, sz.cy, eGXFormat32A, gapp);
}

#ifdef TUI_EDIT_MODE

CString CTuiWnd_Win::GetDebugInfo()
{
    DECLARE_DLL_FUNC(dll_user32, int, WINAPI, GetWindowDpiAwarenessContext, (HWND hWnd));
    int nMode = 0;
    if (_GetWindowDpiAwarenessContext)
    {
        nMode = _GetWindowDpiAwarenessContext(m_hWnd);
        if (nMode >= -5 && nMode <= -1)
            nMode = -nMode + 1;
        else if (nMode >= 16 && nMode <= 18)
            nMode -= 9;
        else
            nMode = 1;
    }

    static LPCTSTR pMode[] = { 
        L"not support",
        L"error",
        L"DPI_AWARENESS_CONTEXT_UNAWARE",
        L"DPI_AWARENESS_CONTEXT_SYSTEM_AWARE",
        L"DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE",
        L"DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2",
        L"DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED",
        L"DPI_AWARENESS_CONTEXT_UNAWARE",
        L"DPI_AWARENESS_CONTEXT_SYSTEM_AWARE",
        L"DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE",
    };

    TM::CString str;
    str.Format(L"\r\n\r\nDPI mode: %s\r\ntheme mode: ", pMode[nMode]);
    str += m_pTheme->m_pType;
    return CTuiWnd::GetDebugInfo() + str;
}

#endif // TUI_EDIT_MODE

///////////////////////////////////////////////////////////////////////////////
// CViewWin
void CViewWin::InternalSetCapture(const CTuiWgt* hWgt)
{
    HWND hWnd = hWgt->GetHWND();
    ::SetCapture(hWnd);
//     DbgAssert(::GetCapture() == hWnd);
}

void CViewWin::InternalReleaseCapture()
{
    DbgVerify(::ReleaseCapture());
}

TMBool CViewWin::InternalIsCapture(const CTuiWgt* hWgt)
{
    return ::GetCapture() == hWgt->GetHWND();
}

void CViewWin::InternalSetFocus(const CTuiWgt* hWgt)
{
    if (hWgt)
    {
        CTuiWnd* pMgr = hWgt->GetOwner();
        if (WS_ADV_NOACTIVATE & pMgr->GetExStyle())
        {
            HWND hFore = ::GetForegroundWindow();
            DWORD pFore = GetWindowThreadProcessId(hFore, 0);
            DWORD pSelf = ThreadSelfId();
            if (pFore != pSelf)
            {
                AttachThreadInput(pFore, pSelf, true);
                SetForegroundWindow(pMgr->GetHWND());
                AttachThreadInput(pFore, pSelf, false);
            }
        }
        ::SetFocus(pMgr->GetHWND());
    }
}


TM_BGN_PRIVATE_NAMESPACE


#if !TM_WIN_METRO

#ifndef TUI_EDIT_MODE

CTuiApp* tuiGetAPP(HTUI hWnd)
{
    if (hWnd)
    {
        ASSERT_CLASS(hWnd);
        return hWnd->GetView();
    }
    else
    {
        static CViewWin _mdlWin;
        return &_mdlWin;
    }
}
#endif // TUI_EDIT_MODE

#endif // TM_WIN_METRO

#define DIAOG_END_PROP          TM_T("TUI_ENDDIAOG_PROP")
#define DIAOG_PROP_MASK         0x80000000

int tuiDoDialogBox(HTUI hMgr, TMHWnd hOwner)
{
    ASSERT_CLASS(hMgr);

    {
        TMHWnd hCap = ::GetCapture();
        if (hCap)
            ::SendMessage(hCap, WM_CANCELMODE, hMgr->GetDlgCtrlID(), (TMLParam)tm_addressof(CTuiMsg(hMgr, WM_INITDIALOG, 0, 0)));
    }
    TMHWnd hOldWin = hOwner;
    if (hOwner)
    {
        if (hOwner == ::GetDesktopWindow())
        {
            hOwner = 0;
        }
        else
        {
            while (_NS_SDK::GetWindowLong(hOwner, GWL_STYLE) & WS_CHILD)
            {
                hOwner = _NS_SDK::GetParent(hOwner);
            }
            
            if (hOwner && ::IsWindowEnabled(hOwner))
            {
                ::EnableWindow (hOwner, false);
            }
            else
            {
                hOwner = 0;
            }
        }
    }
    
    TMHWnd hWnd = hMgr->GetHWND();
    //::ShowWindow(hWnd, SW_SHOWNA);//SW_SHOWNORMAL);
    if (::IsZoomed(hWnd))
        ::ShowWindow(hWnd, SW_SHOWMAXIMIZED);
    else
        ::ShowWindow(hWnd, SW_SHOWNORMAL);
//         ::ShowWindow(hWnd, SW_MINIMIZE);
    
    MSG msg;
    while (::GetMessage(&msg, 0, 0, 0))
    {
        TMBool b = ::IsWindow (hWnd);
        
        if (b || (msg.hwnd && msg.hwnd != hWnd))
        {
//             if (!_tuiApp.PreTranslateMessage(&msg))
            {
                ::TranslateMessage (&msg);
                ::DispatchMessage (&msg);
            }
        }
        
        if (!b)
        {
            if (hOwner)
            {
                ::EnableWindow (hOwner, true);
                ::SetFocus(hOldWin);
            }
            return 0;
        }
        
        TMLParam lprop = (TMLParam)GetProp(hWnd, DIAOG_END_PROP);
        if (lprop & DIAOG_PROP_MASK)
        {
            if (hOwner)
            {
                ::EnableWindow(hOwner, true);
            }
            
            ::SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                SWP_HIDEWINDOW);
            
            ::DestroyWindow(hWnd);
            
            return lprop & (~DIAOG_PROP_MASK);
        }
    }
    
    // when msg == WM_QUIT
    DbgVerify(::DestroyWindow(hWnd));
    DbgVerify(::PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam));
    return -1;
}

TMBool tuiEndDialog(HTUI hMgr, int nCode)
{
    ASSERT_CLASS(hMgr);
    HWND hWnd = hMgr->GetHWND();
    DbgVerify(::PostMessage(hWnd, WM_NULL, 0, 0));
    return SetProp(hWnd, DIAOG_END_PROP, (HANDLE)(nCode | DIAOG_PROP_MASK));
}

TM_END_PRIVATE_NAMESPACE


TM_END_NAMESPACE

#endif // TM_WIN_DESKTOP
