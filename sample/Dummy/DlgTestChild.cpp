/**
 *    @file
 *    @brief
 *    CDlgTestChild
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2017-2-28
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgTestChild.h"


using namespace TM;

CDlgTestChild::CDlgTestChild()
    : m_hWin (0)
    , m_bLayeredChild (0)
{
}

CDlgTestChild::~CDlgTestChild()
{
}

HTUI CDlgTestChild::CreateWCL(int id)
{
    CTuiWidget wndParent(Item(IDC_WINDOW_20054));
    CTuiWidget wnd;
    wnd.Create(wndParent, id);
    TM::CTuiRect rc;
    wndParent.GetClientRect(&rc);
    rc.InflateRect(-10,-10,-10,-10);
    wnd.MoveWindow(&rc);

    if (m_bLayeredChild)
        wnd.ModifyStyle(0, 0x08);

    return wnd;
}

HWND CDlgTestChild::CreateWin(TMUInt uID, TMUInt32 dwStyle)
{
    CWindow wnd;
    wnd.Create(_T("button"), GetHWND(), tm_addressof(CRect(100,100,300,200)), 0, dwStyle, WS_EX_CLIENTEDGE);
    DbgAssert(wnd);
    TM::CTuiDummy wndDummy(Item(uID));
    wndDummy.SetWindow (wnd);
    wnd.SetWindowText(m_strTitle);
    return wnd;
}

HWND CDlgTestChild::CreateDlg(TMUInt uID, TMUInt uDlgID)
{
    HWND hWnd = CreateDialog(_Module.GetResourceInstance(), MAKEINTRESOURCE(uDlgID), GetHWND(), 0);
    TM::CTuiDummy(Item(uID)).SetWindow(hWnd);
    return hWnd;
}

TMResult CDlgTestChild::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    SetWindowText(m_strTitle);

    switch (m_mode)
    {
    case IDC_TESTCHILD:
        CreateWCL(IDC_DUMMY_21017);
        CreateWin(IDC_DUMMY_21017, WS_CHILD | WS_TABSTOP);
        break;

    case IDC_TESTCHILDDLG:
        CreateWCL(IDC_DUMMY_21017);
        CreateDlg(IDC_DUMMY_21017, IDD_DIALOG2);
        break;

    case IDC_TESTPOPDLG:
        CreateWCL(IDC_DUMMY_21017);
        CreateDlg(IDC_DUMMY_21017, IDD_DIALOG3);
        break;

    case IDC_TESTPOPNOACTIVEDLG:
        CreateWCL(IDC_DUMMY_21017);
        CreateDlg(IDC_DUMMY_21017, IDD_DIALOG5);
        break;

    case IDC_TESTCHILDWCL:
        CreateWCL(IDC_DUMMY_21016);
        break;
        
    case IDC_TESTPOPWCL:
        CreateWCL(IDC_DUMMY_21024);
        break;

    case IDC_TESTFOLLOW:
        CreateWCL(IDC_DUMMY_21018);
        CreateWin(IDC_DUMMY_21018, WS_OVERLAPPEDWINDOW | WS_CHILD | WS_VISIBLE | WS_TABSTOP);
        break;

    }
    return 0;
}

TMResult CDlgTestChild::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
//     ::ShowWindow(GetHWND(), SW_MINIMIZE);
    DestroyWindow();
    return 0;
}

TMResult CDlgTestChild::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    ::ShowWindow(GetHWND(), SW_MINIMIZE);
//    ::SetWindowPos(GetHWND(), 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE);
    DestroyWindow();
    return 0;
}

TMResult CDlgTestChild::OnShow1 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTuiWidget wnd(Item(IDC_DUMMY_PARENT));
    wnd.ShowWindow(!CTuiButton((HTUI)hWndCtl).GetCheck());
    return 0;
}

TMResult CDlgTestChild::OnEnable1 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTuiWidget wnd(Item(IDC_DUMMY_PARENT));
    wnd.EnableWindow(!CTuiButton((HTUI)hWndCtl).GetCheck());
    return 0;
}

TMResult CDlgTestChild::OnAttach (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTuiDummy wndDummy(Item(IDC_WINDOW_20054).GetWindow(GW_CHILD));
    if (m_hWin)
    {
        wndDummy.SetWindow(m_hWin);
        m_hWin = 0;
    }
    else
    {
        m_hWin = wndDummy.GetWindow();
        wndDummy.SetWindow(0);
    }
    return 0;
}

