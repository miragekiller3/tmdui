/**
 *    @file
 *    @brief
 *    CDlgWin2pic
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2020-4-28
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2020 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGWIN2PIC_H
#define DLGWIN2PIC_H

#include "tuires.h"


class CDlgWin2pic
    : public TM::CTuiWidgetImpl<CDlgWin2pic> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgWin2pic>
//     , public TM::CTuiMsgFilter
{
public:
    CDlgWin2pic();
    ~CDlgWin2pic();
    
public:
    enum { IDD = IDC_MANAGE_WIN2PIC };
    
    BEGIN_TUIMSG_MAP (CDlgWin2pic)
//     BEGIN_MSG_MAP (CDlgWin2pic)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_1002,       OnSet)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_1005,       OnSet)
        COMMAND_ID_HANDLER  (IDC_RADIOBUTTON_1009,      OnSetFrm)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSet (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSetFrm (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};


#endif // DLGWIN2PIC_H