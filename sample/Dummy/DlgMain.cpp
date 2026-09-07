/**
 *    @file
 *    @brief
 *    CDlgMain
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
#include "DlgMain.h"
#include "DlgTestChild.h"
#include "DlgTestDlg.h"


CDlgMain::CDlgMain()
{
}

CDlgMain::~CDlgMain()
{
}

TMResult CDlgMain::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    return 0;
}

TMResult CDlgMain::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgMain::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgMain::OnTestChild (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
#ifdef __cpp_lib_enable_shared_from_this
    auto dlg = std::make_shared<CDlgTestChild>();
#else
    CDlgTestChild::CPtr dlg;
    dlg.CreateInstance();
#endif
    dlg->m_mode = wID;
    dlg->m_bLayeredChild = TM::CTuiButton(Item(IDC_CHECKBOX_20072)).GetCheck();
    Item(wID).GetWindowText(dlg->m_strTitle);
    dlg->Create(GetHWND());
    return 0;
}

TMResult CDlgMain::OnTestTab (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CDlgTestDlg dlg;
    dlg.DoModal(0);
    return 0;
}

