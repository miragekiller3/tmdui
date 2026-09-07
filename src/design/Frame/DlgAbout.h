/**
 *    @file
 *    @brief
 *    CDlgAbout
 *
 *
 *    @author 
 *    @date    2015-2-8
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2015 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGABOUT_H
#define DLGABOUT_H

#include "tuires.h"

class CDlgAbout
    : public TM::CTuiWidgetImpl<CDlgAbout, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgAbout>
{
public:
    CDlgAbout();
    ~CDlgAbout();
    
public:
    enum { IDD = IDC_MANAGE_ABOUT };
//    enum { IDD = IDC_WINDOW_ABOUT };
    
    BEGIN_TUIMSG_MAP (CDlgAbout)
//       BEGIN_MSG_MAP (CDlgAbout)
       MESSAGE_HANDLER      (WM_INITDIALOG,                     OnInitDialog)
       MESSAGE_HANDLER      (WM_DWMCOMPOSITIONCHANGED,          OnCompositionChanged)

//         MESSAGE_HANDLER     (WM_DESTROY,                        OnDestroy)
//           ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
//        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                          OnCancel)
        COMMAND_ID_HANDLER  (IDABORT,                           OnCancel)
//         COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_URL1,               OnURL)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_URL2,               OnURL)
        ROUTE_ID_HANDLER    (IDCANCEL, CM_MOUSEENTRY,            OnMouseEntry)
        ROUTE_ID_HANDLER    (IDABORT, CM_MOUSEENTRY,            OnMouseCloseEntry)
        ROUTE_ID_HANDLER    (IDC_PUSHBUTTON_URL1, WM_SETCURSOR,    OnSetCursor)
        ROUTE_ID_HANDLER    (IDC_PUSHBUTTON_URL2, WM_SETCURSOR,    OnSetCursor)
//         ROUTE_ID_HANDLER    (IDC_PUSHBUTTON_URL2, WM_PRINTCLIENT,    OnP)
//         ROUTE_OID_HANDLER   (IDC_WINDOW_CLR, WM_PRINTCLIENT,    OnPrintClientClr)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnCompositionChanged (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//       TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnURL (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnMouseEntry(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnMouseCloseEntry(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnSetCursor(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnP(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
//     {
//         bHandled = false;
//         ::SetWindowPos(GetHWND(), 0, 0, 0, 800, 600, SWP_NOZORDER | SWP_NOMOVE);
//         return 0;
//     }

//     TMResult OnPrintClientClr(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

};

// extern CDlgAbout g_dlgAbout;

#endif // DLGABOUT_H