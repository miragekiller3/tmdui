/**
 *    @file
 *    @brief
 *    CDlgTestDlg
 *
 *
 *    @author 
 *    @date    2016-11-2
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2016 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGTESTDLG_H
#define DLGTESTDLG_H

#include "tuires.h"


class CDlgTestDlg
//     : public TM::CDynamicWindowImpl<CDlgTestDlg> //  ,TM::CTuiWindow>
//     : public TM::CDynamicDialogImpl<CDlgTestDlg> //  ,TM::CTuiWindow>
//    : public TM::CTuiWidgetImpl<CDlgTestDlg> //  ,TM::CTuiWindow>
     : public TM::CTuiDialogImpl<CDlgTestDlg> //  ,TM::CTuiWindow>
{
public:
    CDlgTestDlg();
    ~CDlgTestDlg();
    
public:
    enum { IDD = IDC_MANAGE_TEST_DLG };
    
    BEGIN_MSG_MAP (CDlgTestDlg)
//       BEGIN_MSG_MAP (CDlgTestDlg)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//           ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//       TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
};


#endif // DLGTESTDLG_H