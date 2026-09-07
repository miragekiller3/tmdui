/**
 *    @file
 *    @brief
 *    CDlgAnimateWindow
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2019-1-25
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2019 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgAnimateWindow.h"
#include "tmani.h"

using namespace TM;

CDlgAnimateWindow::CDlgAnimateWindow()
{
}

CDlgAnimateWindow::~CDlgAnimateWindow()
{
}

TMResult CDlgAnimateWindow::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
//     bHandled = false;
    CTuiListBox lb(Item(IDC_LISTBOX_FLAG));

#define ADD(x,y)  { int n = lb.AddString(L###x L##y); lb.SetItemData(n, x); }
    ADD(AW_HOR_POSITIVE,    "(L2R)");
    ADD(AW_HOR_NEGATIVE,    "(R2L)");
    ADD(AW_VER_POSITIVE,    "(T2B)");
    ADD(AW_VER_NEGATIVE,    "(B2T)");
    ADD(AW_CENTER,          "");
    ADD(AW_HIDE,            "");
    ADD(AW_ACTIVATE,        "");
    ADD(AW_SLIDE,           "(unused)");
    ADD(AW_BLEND,           "(only window,float,layer)");
    ADD(AW_ERASEMODEIN ,    "(widget)");
    ADD(AW_ERASEMODEINOUT,  "(widget)");

    CTuiApp* app = m_tuiWindow.GetAPP();
    CTuiWidget win(app->CreateDlg(_ModuleATL.GetModuleInstance(), IDC_MANAGE_ANIMATE_WIN, GetHWND()));
    m_win = win.GetHWND();
    m_mgr = app->CreateDlg(_ModuleATL.GetModuleInstance(), IDC_MANAGE_ANIMATE_MGR, GetHWND());
    m_obj = app->CreateDlg(_ModuleATL.GetModuleInstance(), IDC_MANAGE_ANIMATE_WCL, GetHWND());

    m_nRight = 0;
    SyncWindowPos(99999, SWP_SHOWWINDOW);

    OnSetCode(0,0,0,bHandled);

    return 0;
}

static void _CDlgAnimateWindow_MoveTo(CWindow wnd, int lOld, int l, int t, int dx, TMUInt nFlag)
{
    CRect rc;
    wnd.GetWindowRect(&rc);
    if (CMath::Equal0((int)rc.left - lOld, dx))
        wnd.SetWindowPos(0, l, t, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | nFlag);
}

void CDlgAnimateWindow::SyncWindowPos(int dx, TMUInt nFlag)
{
    CRect rc;
    GetWindowRect(rc);
    int l = rc.right;
    int t = rc.top;
    int step = rc.Height() / 3;

    _CDlgAnimateWindow_MoveTo(m_obj.GetHWND(), m_nRight, l, t, dx, nFlag);
    _CDlgAnimateWindow_MoveTo(m_mgr.GetHWND(), m_nRight, l, t + step, dx, nFlag);
    _CDlgAnimateWindow_MoveTo(m_win, m_nRight, l, t + step + step, dx, nFlag);

    m_nRight = l;
}

TMResult CDlgAnimateWindow::OnMove (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    SyncWindowPos(10);

    bHandled = false;
    return 0;
}

// TMResult CDlgAnimateWindow::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
// //     _tuiApp.RemoveMessageFilter(this);
//     return 0;
// }

TMResult CDlgAnimateWindow::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    // get ease func
    CRefPtr<IEase> ptrEase;
    CTuiRadioButton rd(Item(IDC_RADIOBUTTON_BACK));
    rd = rd.GetCheckedButton();
    switch (rd.GetDlgCtrlObjectID())
    {
    case IDC_RADIOBUTTON_BACK:
        EaseCreateBack(&ptrEase, m_tuiWindow.GetDlgItemDouble(IDC_EDIT_PARAM1));
        break;
        
    case IDC_RADIOBUTTON_CIRCLE:
        EaseCreateCircle(&ptrEase);
        break;

    case IDC_RADIOBUTTON_BOUNCE:
        EaseCreateBounce(&ptrEase, m_tuiWindow.GetDlgItemInt(IDC_EDIT_PARAM1), m_tuiWindow.GetDlgItemDouble(IDC_EDIT_PARAM2));
        break;

    case IDC_RADIOBUTTON_ELASTIC:
        EaseCreateElastic(&ptrEase, m_tuiWindow.GetDlgItemInt(IDC_EDIT_PARAM1), m_tuiWindow.GetDlgItemDouble(IDC_EDIT_PARAM2));
        break;

    case IDC_RADIOBUTTON_EXPONENTIAL:
        EaseCreateExponential(&ptrEase, m_tuiWindow.GetDlgItemDouble(IDC_EDIT_PARAM1));
        break;

    case IDC_RADIOBUTTON_POWER:
        EaseCreatePower(&ptrEase, m_tuiWindow.GetDlgItemDouble(IDC_EDIT_PARAM1));
        break;

    case IDC_RADIOBUTTON_SINE:
        EaseCreateSine(&ptrEase);
        break;
    }

    // get flag
    DWORD dwFlag = 0;
    CTuiListBox lb(Item(IDC_LISTBOX_FLAG));
    TMInt nSel[10];
    int n = lb.GetSelItems(10, nSel);
    for (int i = 0; i < n; ++i)
    {
        dwFlag |= lb.GetItemData(nSel[i]);
    }
    
    // get time
    int nTime = m_tuiWindow.GetDlgItemInt(IDC_EDIT_31189);

    // do animate
    ::AnimateWindow(m_win, nTime, dwFlag);

    m_obj.Item(IDC_WINDOW_31183).AnimateWindow(nTime, dwFlag, 0, 0, ptrEase);
    m_mgr.AnimateWindow(nTime, dwFlag, 0, 0, ptrEase);
//    EndDialog(wID);
//     DestroyWindow();
    return 0;
}

TMResult CDlgAnimateWindow::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgAnimateWindow::OnEaseFunc (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiRadioButton rd((HTUI)hWndCtl);
    rd = rd.GetCheckedButton();
    
    // reset param
    CTuiWidget itm1(Item(IDC_EDIT_PARAM1)), itm2(Item(IDC_EDIT_PARAM2));
    itm1.SetWindowText(L"");
    itm2.SetWindowText(L"");
    switch (rd.GetDlgCtrlObjectID())
    {
    case IDC_RADIOBUTTON_BACK:
        itm1.SetWindowText(L"1");
        break;
        
    case IDC_RADIOBUTTON_BOUNCE:
        itm1.SetWindowText(L"3");
        itm2.SetWindowText(L"2");
        break;
        
    case IDC_RADIOBUTTON_ELASTIC:
        itm1.SetWindowText(L"3");
        itm2.SetWindowText(L"3");
        break;
        
    case IDC_RADIOBUTTON_EXPONENTIAL:
        itm1.SetWindowText(L"2");
        break;
        
    case IDC_RADIOBUTTON_POWER:
        itm1.SetWindowText(L"2");
        break;
    }
    
    OnSetCode(0,0,0,bHandled);

    return 0;
}

TMResult CDlgAnimateWindow::OnSetCode(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiRadioButton rd(Item(IDC_RADIOBUTTON_BACK));
    rd = rd.GetCheckedButton();
    
    // init code text
    LPCTSTR pCode;
    
    // set helper text
    CString str;
    rd.GetHint(str);
    m_mgr.Item(IDC_MEMO_31219).SetWindowText(str);
    
    // reset param
    switch (rd.GetDlgCtrlObjectID())
    {
    case IDC_RADIOBUTTON_BACK:
        pCode = L"EaseCreateBack(&ptrEase, 1.0f)";
        break;
        
    case IDC_RADIOBUTTON_CIRCLE:
        pCode = L"EaseCreateCircle(&ptrEase)";
        break;
        
    case IDC_RADIOBUTTON_BOUNCE:
        pCode = L"EaseCreateBounce(&ptrEase, 3, 2.0f)";
        break;
        
    case IDC_RADIOBUTTON_ELASTIC:
        pCode = L"EaseCreateElastic(&ptrEase, 3, 3.0f)";
        break;
        
    case IDC_RADIOBUTTON_EXPONENTIAL:
        pCode = L"EaseCreateExponential(&ptrEase, 2.0f)";
        break;
        
    case IDC_RADIOBUTTON_POWER:
        pCode = L"EaseCreatePower(&ptrEase, 2.0f)";
        break;
        
    case IDC_RADIOBUTTON_SINE:
        pCode = L"EaseCreateSine(&ptrEase)";
        break;

    default:
        return 0;
    }

    // make flag string
    CString strFlag;
    CTuiListBox lb(Item(IDC_LISTBOX_FLAG));
    TMInt nSel[10];
    int n = lb.GetSelItems(10, nSel);
    for (int i = 0; i < n; ++i)
    {
        CString strf;
        lb.GetText(nSel[i], strf);
        int nx = strf.Find(L"(");
        if (nx > 0)
            strf.SetLength(nx);
        strf = strf.Trim();
        if (i != 0)
            strFlag += L" | ";
        strFlag += strf;
    }

    
    // make code string
    CString strCode;
    LPCTSTR pCodeFmt =
        L"CRefPtr<IEase> ptrEase;\r\n"
        L"%s;\r\n"
        L"m_obj.AnimateWindow(2000, %s, 0, 0, ptrEase);";
    strCode.Format(pCodeFmt, pCode, strFlag.c_str());
    m_obj.Item(IDC_MEMO_31184).SetWindowText(strCode);

    return 0;
}


//CTuiMsgFilter
// BOOL CDlgAnimateWindow::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
