/**
 *  @file
 *  @brief
 *  CDlgResetId
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021-7-22
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2021 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "ResetIdDlg.h"
#include "../Design.h"

using namespace TM;


#define _USER_DEFAULT L"default"


CDlgResetId::CDlgResetId()
{
}

CDlgResetId::~CDlgResetId()
{
}

TMResult CDlgResetId::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Item(IDC_STATIC_33011).GetWindowText(m_strStatic1);
    Item(IDC_STATIC_33013).GetWindowText(m_strStatic2);
    Init();

//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgResetId::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgResetId::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgResetId::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgResetId::OnMerge(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    CTuiListBox lb(Item(IDC_LISTBOX_33004));
    int n = lb.GetCurSel();
    if (n < 0)
        return 0;

    CPopIDList aList;
    TCHAR c[1024];
    lb.GetText(n, c);
    aList.insert(CPopIDList::value_type(c, 0));

    TMInt nSel[1024];
    int nAll = lb.GetSelItems(1024, nSel);
    for (int i = 0; i < nAll; ++i)
    {
        if (nSel[i] == n)
            continue;
        TCHAR c[1024];
        if (lb.GetText(nSel[i], c) <= 0)
            break;
        aList.insert(CPopIDList::value_type(c, 0));
    }

    m_proj->MergeUsers(aList);
    Init();
    return 0;
}

TMResult CDlgResetId::OnReset(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    CTuiListBox lb(Item(IDC_LISTBOX_33004));
    int n = lb.GetCurSel();
    if (n < 0)
        return 0;
    TCHAR c[1024];
    lb.GetText(n, c);
    if (CStrView(_USER_DEFAULT) == c)
        c[0] = 0;
    CString strNum;
    m_proj->ResetUserIds(c, Item(IDC_EDIT_33014).GetWindowText().ToLong());
    Init();
    return 0;
}


//CTuiMsgFilter
// BOOL CDlgResetId::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }

void CDlgResetId::ResetText()
{
    CTuiListBox lb(Item(IDC_LISTBOX_33004));
    int n = lb.GetCurSel();
    CString str1;
    CString str2;
    if (n >= 0)
    {
        TCHAR c[1024];
        lb.GetText(n, c);
        int nData = lb.GetItemData(n);
        CString strTmp;
        strTmp.Format(L"%s(%d)", c, nData);
        str1.Format(m_strStatic1, strTmp.c_str());
        str2.Format(m_strStatic2, strTmp.c_str());
    }
    else
    {
        str1.Format(m_strStatic1, L"?");
        str2.Format(m_strStatic2, L"?");
    }
    Item(IDC_STATIC_33011).SetWindowText(str1);
    Item(IDC_STATIC_33013).SetWindowText(str2);
}

TMResult CDlgResetId::OnSelChange(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    ResetText();
    return 0;
}

void CDlgResetId::Init()
{
    ResetText();
    
    CTuiListBox lb(Item(IDC_LISTBOX_33004));
    lb.ResetContent();

    CPopIDList aMacro;
    int n = m_proj->GetUsers(aMacro);
    for (CPopIDList::recorder rec(aMacro); rec; ++rec)
    {
        CStrView strUSer = rec.get_data().first;
        if (strUSer.IsEmpty())
            strUSer = _USER_DEFAULT;
        int n = lb.AddString(CString(strUSer));
        lb.SetItemData(n, rec.get_data().second);
    }
    
    Item(IDC_EDIT_33014).SetWindowText(TM_L2T(n));
}
