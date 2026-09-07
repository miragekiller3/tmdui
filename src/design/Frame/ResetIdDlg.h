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

#ifndef DLGRESETID_H
#define DLGRESETID_H

#include "tuires.h"

class IProj;

class CDlgResetId
    : public TM::CTuiWidgetImpl<CDlgResetId> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgResetId>
//     , public TM::CTuiMsgFilter
{
public:
    IProj* m_proj;
    TM::CString m_strStatic1;
    TM::CString m_strStatic2;

public:
    CDlgResetId();
    ~CDlgResetId();
    
public:
    enum { IDD = IDC_MANAGE_RESET_ID };
    
    BEGIN_TUIMSG_MAP (CDlgResetId)
//     BEGIN_MSG_MAP (CDlgResetId)
        MESSAGE_HANDLER     (WM_INITDIALOG,                     OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT      OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                              OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                          OnCancel)
        COMMAND_HANDLER     (IDC_RESETID_MERGE, BN_CLICKED,     OnMerge)
        COMMAND_HANDLER     (IDC_RESETID_RESET, BN_CLICKED,     OnReset)
        COMMAND_CODE_HANDLER(LBN_SELCHANGE,                     OnSelChange)
    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnMerge(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnReset(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnSelChange(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    void ResetText();
    void Init();

};


#endif // DLGRESETID_H