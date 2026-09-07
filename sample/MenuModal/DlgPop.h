/**
 *    @file
 *    @brief
 *    CDlgPop
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

#ifndef DLGPOP_H
#define DLGPOP_H

#include "tuires.h"


class CDlgPop
    : public TM::CTuiWidgetImpl<CDlgPop>
//     : public TM::CTuiWidgetImpl<CDlgPop>
//     : public TM::CTuiDialogImpl<CDlgPop>
{
public:
    CDlgPop();
    ~CDlgPop();
    
public:
    enum { IDD = IDW_WINDOW_POP };
    
    BEGIN_TUIMSG_MAP (CDlgPop)
//       BEGIN_MSG_MAP (CDlgPop)

//         MESSAGE_HANDLER     (CM_BEGINMENUPOP,           OnBeginMenuPop)
//        MESSAGE_HANDLER     (CM_ENDMENUPOP,              OnEndMenuPop)
//           ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDC_POP,                      OnPop)
        COMMAND_ID_HANDLER  (IDC_POP_DLG,                  OnPopDlg)
        COMMAND_ID_HANDLER  (IDC_CLOSE,                    OnClose)
        COMMAND_ID_HANDLER  (IDC_CLOSEC,                    OnCloseChild)
        COMMAND_ID_HANDLER  (IDCANCEL,                     OnCancel)
    END_MSG_MAP()
    
    TMResult OnEndMenuPop (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//       TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnPop (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnPopDlg (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnClose (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCloseChild (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
};


#endif // DLGPOP_H