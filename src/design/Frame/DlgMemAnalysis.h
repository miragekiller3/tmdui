/**
 *  @file
 *  @brief
 *  CDlgMemAnalysis
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025-6-16
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2025 miragekiller                             
 *  All rights reserved  
 */

#ifndef DLGMEMANALYSIS_H
#define DLGMEMANALYSIS_H

#include "tuires.h"


class CDlgMemAnalysis
    : public TM::CTuiWidgetImpl<CDlgMemAnalysis> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgMemAnalysis>
//     , public TM::CTuiMsgFilter
{
public:
    CDlgMemAnalysis();
    ~CDlgMemAnalysis();
    
public:
    enum { IDD = IDC_MANAGE_MEM_ANALYSIS };
    
    BEGIN_TUIMSG_MAP (CDlgMemAnalysis)
//     BEGIN_MSG_MAP (CDlgMemAnalysis)
        MESSAGE_HANDLER     (WM_INITDIALOG,                     OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)
        COMMAND_HANDLER     (IDC_LISTBOX_30395, LBN_SELCHANGE,  OnSelChange1)
        COMMAND_HANDLER     (IDC_LISTBOX_30395, LBN_SORTED,     OnSorted1)
        COMMAND_HANDLER     (IDC_LISTBOX_30397, LBN_SELCHANGE,  OnSelChange2)
        CMNOTIFY_HANDLER(IDC_LISTBOX_30395, HDN_ITEMCLICK, OnHeaderClicked)

        COMMAND_ID_HANDLER  (IDOK,                              OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                          OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnSelChange1(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnSorted1(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnSelChange2(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnHeaderClicked(int idCtrl, TM::NMBASE* pnmh, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    void Init();

};


#endif // DLGMEMANALYSIS_H