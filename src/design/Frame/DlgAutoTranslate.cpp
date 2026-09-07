/**
 *  @file
 *  @brief
 *  CDlgAutoTranslate
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021-10-20
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2021 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgAutoTranslate.h"
#include "tmwnd.h"

using namespace TM;


CDlgAutoTranslate::CDlgAutoTranslate()
{
}

CDlgAutoTranslate::~CDlgAutoTranslate()
{
}

TMResult CDlgAutoTranslate::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgAutoTranslate::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

// TMResult CDlgAutoTranslate::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
// {
// //    EndDialog(wID);
//     DestroyWindow();
//     return 0;
// }

TMResult CDlgAutoTranslate::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgAutoTranslate::OnLoad(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    return 0;
}

TMResult CDlgAutoTranslate::OnSave(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    TMCStr lpcstrFilter = 
        _T("Eng XML Files (*.xml)\0*.xml\0")
        _T("");
    
    TM::CFileOpenDialog dlg;
    int nRet = dlg.DoModal(GetHWND(), lpcstrFilter, 0, 0, 0,
        OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
    if (nRet != IDOK)
        return 0;

    m_StrFile = dlg.GetPathName();
    CTuiString str;
    str.LoadString(GetModule(), IDC_MANAGE_AUTO_TRANSLATE);
    str += L" - ";
    str += m_StrFile;
    SetWindowText(str);

    return 0;
}


//CTuiMsgFilter
// BOOL CDlgAutoTranslate::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
