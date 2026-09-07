/**
 *    @file
 *    @brief
 *    CDlgTestDlg
 *
 *
 *    @author 
 *    @date    2016-11-2
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2016 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgTestDlg.h"



using namespace TM;


CDlgTestDlg::CDlgTestDlg()
{
}

CDlgTestDlg::~CDlgTestDlg()
{
}

TMResult CDlgTestDlg::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    HWND hWnd = CreateDialog(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDD_DIALOG2), GetHWND(), 0);
    TM::CTuiDummy(Item(IDC_DUMMY_20029)).SetWindow(hWnd);
    
    hWnd = CreateDialog(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDD_DIALOG2), GetHWND(), 0);
    TM::CTuiDummy(Item(IDC_DUMMY_20032)).SetWindow(hWnd);
    HWND hWnd2 = ::GetDlgItem(hWnd, IDC_BUTTON1);
    ::SetParent(hWnd2, GetHWND());
    TM::CTuiDummy(Item(IDC_DUMMY_20036)).SetWindow(hWnd2);
    HWND hWnd3 = ::GetDlgItem(hWnd, IDCANCEL);
    ::SetParent(hWnd3, GetHWND());
    CWindow(hWnd3).ModifyStyle(WS_TABSTOP, 0);
    TM::CTuiDummy(Item(IDC_DUMMY_20038)).SetWindow(hWnd3);

    hWnd = CreateDialog(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDD_DIALOG2), GetHWND(), 0);
    TM::CTuiDummy(Item(IDC_DUMMY_20033)).SetWindow(hWnd);

    return 0;
}

TMResult CDlgTestDlg::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
//     DestroyWindow();
    return 0;
}

TMResult CDlgTestDlg::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    EndDialog(wID);
//     DestroyWindow();
    return 0;
}

