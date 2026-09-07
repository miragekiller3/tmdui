/**
 *    @file
 *    @brief
 *    CDlgControls
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2012-10-2
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2012 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGCONTROLS_H
#define DLGCONTROLS_H

#include "tuires.h"

class IProj;

class CDlgControls
//    : public TM::CDynamicWindowImpl<CDlgControls>
    : public TM::CTuiWidgetImpl<CDlgControls>
//    : public TM::CTuiDialogImpl<CDlgControls>
{
// private:
//     int m_nCount;

public:
    CDlgControls();
    ~CDlgControls();

public:
    enum { IDD = IDW_TUI_CONTROLS_VIEW };
    
    BEGIN_TUIMSG_MAP (CDlgControls)
//     BEGIN_MSG_MAP (CDlgControls)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog);
//        COMMAND_ID_HANDLER    (IDOK,                        OnOk)
//         COMMAND_ID_HANDLER    (IDC_UPDOWN,        OnPop)
//        MESSAGE_HANDLER (WM_CREATE,             OnCreate);
        ROUTE_HANDLER   (WM_MOUSEMOVE,          OnMouseMove)
        ROUTE_HANDLER   (WM_LBUTTONDBLCLK,      OnLButtonDbClick)
    END_MSG_MAP()

//    TMResult OnCreate (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseMove(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnLButtonDbClick(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnPop (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
        
//    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    static void Init(TM::HTUI hWnd, IProj* proj);
};


#endif // DLGCONTROLS_H