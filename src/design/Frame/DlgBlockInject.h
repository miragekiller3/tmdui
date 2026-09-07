/**
 *  @file
 *  @brief
 *  CDlgBlockInject
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2023-8-4
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2023 miragekiller                             
 *  All rights reserved  
 */

#ifndef DLGBLOCKINJECT_H
#define DLGBLOCKINJECT_H

#include "tuires.h"


class CDlgBlockInject
    : public TM::CTuiWidgetImpl<CDlgBlockInject> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgBlockInject>
//     , public TM::CTuiMsgFilter
{
public:
    CDlgBlockInject();
    ~CDlgBlockInject();

public:
    enum { IDD = IDC_MANAGE_BLOCK_INJECT };
    
    BEGIN_TUIMSG_MAP (CDlgBlockInject)
//     BEGIN_MSG_MAP (CDlgBlockInject)
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


#endif // DLGBLOCKINJECT_H