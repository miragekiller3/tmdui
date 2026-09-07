/**
 *    @file
 *    @brief
 *    CDlgSetUser
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2017-1-13
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgSetUser.h"
#include "cfg.h"

using namespace TM;


CSetUserDlg::CSetUserDlg()
{
    DoProfileInput(g_cfg);
}

TMResult CSetUserDlg::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    CenterWindow(GetParent());
    DoSerializeInput(g_cfg);
    bHandled = false;
    return 0;
}

LRESULT CSetUserDlg::OnOk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    DoSerializeOutput(g_cfg);
    DoProfileInput(g_cfg);
    EndDialog(wID);
    return 0;
}

LRESULT CSetUserDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    EndDialog(wID);
    return 0;
}

// void CSetUserDlg::SetUser()
// {
//     ProfileExchangeLoad();
//     s_aInfo.SetUser(strUser);
// }

TM::CString CSetUserDlg::GetUser() const
{
//     ProfileExchangeLoad();
    CString str(strUser);
    if (!strEmail.IsEmpty())
    {
        str += L" <";
        str += strEmail;
        str += L">";
    }
    return str;
}