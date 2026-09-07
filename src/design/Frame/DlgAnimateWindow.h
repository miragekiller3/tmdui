/**
 *    @file
 *    @brief
 *    CDlgAnimateWindow
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2019-1-25
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2019 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGANIMATEWINDOW_H
#define DLGANIMATEWINDOW_H

#include "tuires.h"


class CDlgAnimateWindow
//     : public TM::CTuiWidgetImpl<CDlgAnimateWindow> //, TM::CTuiWindow>
    : public TM::CTuiDialogImpl<CDlgAnimateWindow>
//     , public TM::CTuiMsgFilter
{
public:
    CWindow m_win;
    TM::CTuiWidget m_mgr;
    TM::CTuiWidget m_obj;
    int m_nRight;

public:
    CDlgAnimateWindow();
    ~CDlgAnimateWindow();
    
public:
    enum { IDD = IDC_MANAGE_ANIMATE_WINDOW };
    
    BEGIN_MSG_MAP (CDlgAnimateWindow)
//     BEGIN_MSG_MAP (CDlgAnimateWindow)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
        MESSAGE_HANDLER     (WM_MOVE,                   OnMove)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDC_EASEFUNC,              OnEaseFunc)
        COMMAND_HANDLER     (IDC_LISTBOX_FLAG, LBN_SELCHANGE, OnSetCode)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMove(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnEaseFunc(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSetCode(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    void SyncWindowPos(int dx, TMUInt nFlag = 0);

};


#endif // DLGANIMATEWINDOW_H