/**
 *    @file
 *    @brief
 *    CDlgMain
 *
 *
 *    @author 
 *    @date    2014-4-14
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2014 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGMAIN_H
#define DLGMAIN_H

#include "tuires.h"


class CDlgMain
    : public TM::CTuiWidgetImpl<CDlgMain, TM::CTuiWindow>
//     : public TM::CTuiWidgetImpl<CDlgMain>
//     : public TM::CTuiDialogImpl<CDlgMain>
{
public:
    CDlgMain();
    ~CDlgMain();
    
public:
    enum { IDD = IDD_TUI_MAIN };
    
    BEGIN_TUIMSG_MAP (CDlgMain)
//       BEGIN_MSG_MAP (CDlgMain)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//           ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDC_TEST1,                 OnTest1)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_21014,      OnEmbed1)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_21015,      OnEmbed2)

        ROUTE_OID_HANDLER   (IDC_PUSHBUTTON_21004, CM_MOUSEENTRY, OnDestroy2)
        ROUTE_OID_HANDLER   (IDC_PUSHBUTTON_21005, CM_MOUSEENTRY, OnDestroy30)
        
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//       TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnTest1 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnEmbed1 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnEmbed2 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnDestroy2(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnDestroy30(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    

};


#endif // DLGMAIN_H