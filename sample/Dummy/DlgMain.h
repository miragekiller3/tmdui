/**
 *    @file
 *    @brief
 *    CDlgMain
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2017-2-28
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGMAIN_H
#define DLGMAIN_H

#include "tuires.h"
#include "../h/zoom.h"


class CDlgMain
    : public TM::CTuiWidgetImpl<CDlgMain, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgMain>
    , public CZoomImplT<CDlgMain>
{
public:
    CDlgMain();
    ~CDlgMain();
    
public:
    enum { IDD = IDC_MANAGE_MAIN };
    
    BEGIN_TUIMSG_MAP (CDlgMain)
//       BEGIN_MSG_MAP (CDlgMain)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//           ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDC_TESTCHILD,             OnTestChild)     
        COMMAND_ID_HANDLER  (IDC_TESTCHILDDLG,          OnTestChild)     
        COMMAND_ID_HANDLER  (IDC_TESTPOPNOACTIVEDLG,    OnTestChild)     
        COMMAND_ID_HANDLER  (IDC_TESTPOPDLG,            OnTestChild)     
        COMMAND_ID_HANDLER  (IDC_TESTCHILDWCL,          OnTestChild)     
        COMMAND_ID_HANDLER  (IDC_TESTPOPWCL,            OnTestChild)     
        COMMAND_ID_HANDLER  (IDC_TESTFOLLOW,            OnTestChild)     

        COMMAND_ID_HANDLER  (IDC_TESTTAB,               OnTestTab)     

        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        CHAIN_MSG_MAP (CZoomImpl)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//       TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnTestChild (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnTestTab (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

};


#endif // DLGMAIN_H