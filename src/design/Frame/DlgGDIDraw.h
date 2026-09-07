/**
 *    @file
 *    @brief
 *    CDlgDrawXXX
 *
 *
 *    @author 
 *    @date    2015-4-23
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2015 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGDRAWXXX_H
#define DLGDRAWXXX_H

#include "tuires.h"


class CDlgDrawXXX
    : public TM::CTuiWidgetImpl<CDlgDrawXXX, TM::CTuiWindow>
//     : public TM::CTuiWidgetImpl<CDlgDrawXXX>
//     : public TM::CTuiDialogImpl<CDlgDrawXXX>
{
public:
    CDlgDrawXXX();
    ~CDlgDrawXXX();
    
public:
    enum { IDD = IDC_MANAGE_DRAW_XXX };
    
    BEGIN_TUIMSG_MAP (CDlgDrawXXX)
//       BEGIN_MSG_MAP (CDlgDrawXXX)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
        ROUTE_OID_HANDLER   (IDC_WINDOW_31145, WM_PRINTCLIENT,       OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_WINDOW_31153, WM_PRINTCLIENT,       OnPrintClient2)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        COMMAND_CODE_HANDLER  (LBN_SELCHANGE,           OnSelChange)
        
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnPrintClient2(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSelChange (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
};


#endif // DLGDRAWXXX_H