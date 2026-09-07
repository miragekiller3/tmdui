/**
 *    @file
 *    @brief
 *    CDlgArrowMain
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-7-8
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGSIGHTMAIN_H
#define DLGSIGHTMAIN_H

#include "tuires.h"
#include "../h/zoom.h"


class CDlgArrowMain
    : public TM::CTuiWidgetImpl<CDlgArrowMain, TM::CTuiWindow>
    , public CZoomImplT<CDlgArrowMain>
//     : public TM::CTuiDialogImpl<CDlgArrowMain>
//     , public TM::CTuiMsgFilter
{
public:
    CDlgArrowMain();
    ~CDlgArrowMain();
//     TM::CTuiEdit wndTxt;
//     TM::CTuiArrow wndArrow;
//     TM::CTuiArrow wndArrow3;
    // 0: IDC_FLOAT_2006; 1: IDC_MANAGE_TIPS; 2: IDC_MANAGE_UP
    TM::CTuiWidget  wndPop[3];  
    TM::CTuiArrow   wndArrow[3];
    
public:
    enum { IDD = IDC_MANAGE_SIGHT_MAIN };
    
    BEGIN_TUIMSG_MAP (CDlgArrowMain)
//     BEGIN_MSG_MAP (CDlgSpyMsg)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_2012,       OnText)
        COMMAND_ID_HANDLER  (IDC_CREATE,                OnCreatePop)
        
        COMMAND_ID_HANDLER  (IDC_BIND1,                 OnBind)
        COMMAND_ID_HANDLER  (IDC_UNBIND1,               OnBind)
        COMMAND_ID_HANDLER  (IDC_BIND2,                 OnBind)
        COMMAND_ID_HANDLER  (IDC_UNBIND2,               OnBind)
        COMMAND_ID_HANDLER  (IDC_BIND3,                 OnBind)
        COMMAND_ID_HANDLER  (IDC_UNBIND3,               OnBind)
        COMMAND_ID_HANDLER  (IDC_MINCREATE,             OnMinCreate)
        CHAIN_MSG_MAP       (CZoomImpl)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCreatePop (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnText (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnBind (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnMinCreate (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};


#endif // DLGSIGHTMAIN_H