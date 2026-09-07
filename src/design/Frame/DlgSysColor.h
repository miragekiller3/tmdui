/**
 *    @file
 *    @brief
 *    CDlgSysColor
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2017-1-18
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGSYSCOLOR_H
#define DLGSYSCOLOR_H

#include "tuires.h"



class CDlgSysColor
    : public TM::CTuiWidgetImpl<CDlgSysColor, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgSysColor>
{
private:
    const COLORREF* m_pClr;
    TMBool m_bDark;

public:
    CDlgSysColor();
    ~CDlgSysColor();
    
public:
    enum { IDD = IDC_MANAGE_SYS_COLOR };
    
    BEGIN_TUIMSG_MAP (CDlgSysColor)
//       BEGIN_MSG_MAP (CDlgSysColor)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//           ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                          OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                      OnCancel)

        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_31250,          OnSet)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_31251,          OnSet)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_31252,          OnSet)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_31253,          OnSet)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_31254,          OnSet)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_31255,          OnSet)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_31255,          OnSet)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_30381,          OnSet)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_SYS,            OnSet)

        ROUTE_ID_HANDLER    (IDC_COLOR, WM_PRINTCLIENT,     OnColorPrintClient)
        ROUTE_ID_HANDLER    (IDC_COLOR, WM_LBUTTONDOWN,     OnLButtonDown)
        ROUTE_ID_HANDLER    (IDC_COLOR, WM_LBUTTONDBLCLK,   OnLButtonDblClk)
    END_MSG_MAP()
    
//     TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//       TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnSys (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSet (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnColorPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnLButtonDown(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnLButtonDblClk(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
};


#endif // DLGSYSCOLOR_H