/**
 *    @file
 *    @brief
 *    CDlgAutoSize
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-9-27
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGAUTOSIZE_H
#define DLGAUTOSIZE_H

#include "tuires.h"


class CDlgAutoSize
    : public TM::CTuiWidgetImpl<CDlgAutoSize> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgAutoSize>
//     , public TM::CTuiMsgFilter
{
public:
    CDlgAutoSize();
    ~CDlgAutoSize();
    
public:
    enum { IDD = IDC_MANAGE_AUTO_SIZE };
    
    BEGIN_TUIMSG_MAP (CDlgAutoSize)
//     BEGIN_MSG_MAP (CDlgSpyMsg)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_1003,       OnText1)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_1004,       OnText2)
        COMMAND_ID_HANDLER  (IDC_LAYOUT,                OnLayout)

        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnText1 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnText2 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnLayout (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};


#endif // DLGAUTOSIZE_H