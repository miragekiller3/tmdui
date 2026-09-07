/**
 *  @file
 *  @brief
 *  CDlgHeader
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025-6-27
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2025 miragekiller                             
 *  All rights reserved  
 */

#ifndef DLGHEADER_H
#define DLGHEADER_H

#include "tuires.h"


class CDlgHeader
    : public TM::CTuiWidgetImpl<CDlgHeader> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgHeader>
//     , public TM::CTuiMsgFilter
{
public:
    CDlgHeader();
    ~CDlgHeader();
    
public:
    enum { IDD = IDC_MANAGE_HEADER };
    
    BEGIN_TUIMSG_MAP (CDlgHeader)
//     BEGIN_MSG_MAP (CDlgHeader)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};


#endif // DLGHEADER_H