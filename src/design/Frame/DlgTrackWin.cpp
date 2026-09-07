/**
 *    @file
 *    @brief
 *    CDlgTrackWin
 *
 *  @author miragekiller <3916345933@qq.com>
 *    @date    2018-6-21
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgTrackWin.h"
#include "MainWnd.h"
#include "tmdebugext.h"
#include "resource.h"
#include <oleacc.h>
#include "tmkeyboard.h"

using namespace TM;

CDlgTrackWin::CDlgTrackWin()
{
    CAutoMsgAnalysis ama("CDlgTrackWin::CDlgTrackWin()");
}

CDlgTrackWin::~CDlgTrackWin()
{
    if (m_hWnd)
        DestroyWindow();
}

TMResult CDlgTrackWin::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
//     HTUI hFocus = AppGetTester()->GetFocus();
//     _OnSetFocus(m_wndPanel, hFocus, GetHWND());
    this->SetAni(100);
    return 0;
}

TMResult CDlgTrackWin::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    KillAni();
    return 0;
}

// TMResult CDlgTrackWin::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
// //    EndDialog(wID);
//     DestroyWindow();
//     return 0;
// }
// 
// TMResult CDlgTrackWin::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
// //    EndDialog(wID);
//     DestroyWindow();
//     return 0;
// }

BOOL WINAPI _EnumFindIMEProc(HWND hWnd, LPARAM lp)
{
    if (!IsWindowVisible(hWnd))
        return true;
    TCHAR c[256];
    GetClassName(hWnd, c, 256);
    if (StrCompare(c, L"MSCTFIME Composition") != 0)
        return true;
    *(HWND*)lp = hWnd;
    return false;
}

static HWND _CDlgTrackWin_GetWin(int nMode)
{
    if (nMode == IDM_TRACK_IME)
    {
        HWND hWnd = 0;
        EnumWindows(_EnumFindIMEProc, (LPARAM)&hWnd);
        return hWnd;
    }

    HWND hWnd = ::GetForegroundWindow();
    if (nMode == IDM_TRACK_FG_WIN)
        return hWnd;

    DWORD tid = GetWindowThreadProcessId(hWnd, 0);
    GUITHREADINFO gti = {sizeof(gti)};
    GetGUIThreadInfo(tid, &gti);
    switch (nMode)
    {
    case IDM_TRACK_FOCUS_WIN: return gti.hwndFocus;
    case IDM_TRACK_CAPTURE_WIN: return gti.hwndCapture;
    }
    DbgAssert(0);
    return 0;
}

STDAPI _AccessibleObjectFromPoint(POINT ptScreen, IAccessible **ppAcc,
                                 VARIANT * pvarChild)
{
    HRESULT hr;
    IAccessible * pAcc;
    VARIANT varChild;
    HWND    hwndPoint;
    
    if (IsBadWritePtr(ppAcc,sizeof(void*)) || IsBadWritePtr (pvarChild,sizeof(VARIANT)))
        return (E_INVALIDARG);
    
    *ppAcc = NULL;
    pvarChild->vt = VT_EMPTY;
    
    //
    // Is this a valid screen point?
    //
    hwndPoint = WindowFromPoint(ptScreen);
    if (!hwndPoint)
        return(E_INVALIDARG);
    
    //
    // Get the top level window of this one and work our way down.  We have
    // to do this because applications may implement Acc at an intermediate
    // level above the child window.  Our default implementation will let us
    // get there and mesh.
    //
    hwndPoint = GetAncestor(hwndPoint, GA_ROOT);
    if (!hwndPoint)
        return(E_FAIL);
    
    hr = AccessibleObjectFromWindow(hwndPoint, OBJID_WINDOW, IID_IAccessible,
        (void **)&pAcc);
    
    //
    // OK, now we are cooking.
    //
    while (SUCCEEDED(hr))
    {
        //
        // Get the child at this point in the container object.
        //
        VariantInit(&varChild);
        hr = pAcc->accHitTest(ptScreen.x, ptScreen.y, &varChild);
        if (!SUCCEEDED(hr))
        {
            // Uh oh, error.  This should never happen--something moved.
            pAcc->Release();
            return(hr);
        }
        
        //
        // Did we get back a VT_DISPATCH?  If so, there is a child object.
        // Otherwise, we have our thing (container object or child element
        // too small for object).
        //
        if (varChild.vt == VT_DISPATCH)
        {
            pAcc->Release();
            
            if (! varChild.pdispVal)
                return(E_POINTER);
            
            pAcc = NULL;
            hr = varChild.pdispVal->QueryInterface(IID_IAccessible,
                (void **)&pAcc);
            
            varChild.pdispVal->Release();
        }
        else if ((varChild.vt == VT_I4) || (varChild.vt == VT_EMPTY))
        {
            //
            // accHitTest should ALWAYS return an object if the child is
            // an object.  Unlike with accNavigate, where you usually
            // have to pick by-index or by_object only and intermixed means
            // get_accChild is needed.
            //
            *ppAcc = pAcc;
            VariantCopy(pvarChild, &varChild);
            return(S_OK);
        }
        else
        {
            //
            // Failure.  Shouldn't have been returned.
            //
            VariantClear(&varChild);
            pAcc->Release();
            hr = E_INVALIDARG;
        }
    }
    
    return(hr);
}


// TM::CAni
void CDlgTrackWin::OnTimer(...)
{
    if (m_nMode == IDM_TRACK_ACC)
    {
        if (CKeyboard::IsKeyDown(VK_RBUTTON))
            m_pMain->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_TRACK_ACC, BN_CLICKED));
        else
        {
            CComPtr<IAccessible> acc;
            CComVariant var;
            POINT pt;
            GetCursorPos(&pt);
            _AccessibleObjectFromPoint(pt, &acc, &var);
            if (!acc)
                ::ShowWindow(GetHWND(), 0);
            else
            {
                long l, t, w, h;
                if (SUCCEEDED(acc->accLocation(&l, &t, &w, &h, var)))
                    ::SetWindowPos(GetHWND(), HWND_TOPMOST, l - 2, t - 2, w + 4, h + 4,
                    SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOOWNERZORDER | SWP_NOZORDER);
                //m_pMain->GetPanel().SelectByAcc(acc);
            }
        }
    }
    else
    {
        
        HWND hWnd = _CDlgTrackWin_GetWin(m_nMode);
        
        if (!hWnd)
            ::ShowWindow(GetHWND(), 0);
        else
        {
            TM::CRect rc;
            ::GetWindowRect(hWnd, &rc);
            rc.InflateRect(2, 2);
            TM::CRect rcs;
            CMonitor::FromWindow(hWnd).GetRect(&rcs, 0);
            rc &= rcs;
            ::SetWindowPos(GetHWND(), HWND_TOPMOST, rc.Left(), rc.Top(), rc.Width(), rc.Height(),
                SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOOWNERZORDER | SWP_NOZORDER);
            static HWND hOld = 0;
            if (hOld != hWnd)
            {
                TCHAR c[1024];
                TCHAR d[1024];
                ::GetWindowText(hWnd, c, 1024);
                ::GetClassName(hWnd, d, 1024);
                DbgTrace(L"CDlgTrackWin::_OnSetFocus(%p, %s, %s)", hWnd, d, c);
                hOld = hWnd;
            }
        }
    }
}

void CDlgTrackWin::SetTrack(CMainWnd* pMain, int nMode, TMBool bShow)
{
    static CDlgTrackWin s_dlgTrackWin[5];

    DbgAssert(nMode >= IDM_TRACK_FOCUS_WIN && nMode <= IDM_TRACK_ACC);
    CDlgTrackWin& dlg = s_dlgTrackWin[nMode - IDM_TRACK_FOCUS_WIN];
    dlg.m_nMode = nMode;
    dlg.m_pMain = pMain;
    if (dlg.IsWindow())
        dlg.DestroyWindow();
    if (bShow)
    {
        dlg.Create(*pMain->GetAPP(), (HWND)0);
        if (nMode == IDM_TRACK_ACC)
            pMain->GetPanel().Alert(L"press mouse rbutton to stop track acc");
    }
}