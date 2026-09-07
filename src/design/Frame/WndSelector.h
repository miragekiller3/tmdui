/**
 *    @file
 *    @brief
 *    CWndSelector
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2019-5-29
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2019 miragekiller                               
 *    All rights reserved  
 */

#ifndef WNDSELECTOR_H
#define WNDSELECTOR_H

#include "tuires.h"

class CWndPanel;
class IProjMgr;
class IObjEdit;

class CWndSelector
    : public TM::CTuiWidgetImpl<CWndSelector> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CWndSelector>
//     , public TM::CTuiMsgFilter
{
public:
    CWndSelector();
    ~CWndSelector();
    
public:
    enum { IDD = IDC_WINDOW_SELECTOR };
    
    BEGIN_TUIMSG_MAP (CWndSelector)
//     BEGIN_MSG_MAP (CWndSelector)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//        ROUTE_OID_HANDLER    (IDC_WINDOW_31237, WM_PRINTCLIENT,       OnChildPrintClient)
//        ROUTE_OID_HANDLER    (IDC_RADIOBUTTON_30332, WM_LBUTTONDOWN,       OnChildLButtonDown)
//         COMMAND_ID_HANDLER  (IDOK,                      OnOk)
//         COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
       COMMAND_ID_HANDLER  (IDC_RADIOBUTTON_30332,                      OnChildLButtonDown)

    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

//     TMResult OnChildPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnChildLButtonDown(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    void EnumProc(IProjMgr* mgr, IObjEdit* wnd, TM::CTuiWidget wndParent, const TM::CTuiPoint& pt, TMBool bRoot, TM::CTuiRect& rc);
    void Selector(const TM::CTuiPoint& pt, IProjMgr* mgr);

    CWndPanel& GetPanel();

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};


#endif // WNDSELECTOR_H