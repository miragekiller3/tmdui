/**
 *    @file
 *    @brief
 *    CDlgFlashMain
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-7-8
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGFLASHMAIN_H
#define DLGFLASHMAIN_H

#include "tuires.h"
#include "../h/zoom.h"


class CDlgFlashMain
    : public TM::CTuiWidgetImpl<CDlgFlashMain, TM::CTuiWindow>
    , public CZoomImplT<CDlgFlashMain>
//     : public TM::CTuiDialogImpl<CDlgFlashMain>
//     , public TM::CTuiMsgFilter
{
public:
    CDlgFlashMain();
    ~CDlgFlashMain();
    
public:
    enum { IDD = IDC_MANAGE_SIGHT_MAIN };
    
    BEGIN_TUIMSG_MAP (CDlgFlashMain)
//     BEGIN_MSG_MAP (CDlgSpyMsg)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDC_PLAY,                  OnPlay)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        
        COMMAND_CODE_HANDLER    (XN_INIT, OnObjInit)
        
        CHAIN_MSG_MAP       (CZoomImpl)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnObjInit(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnPlay (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);



    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};


#endif // DLGFLASHMAIN_H