/**
 *  @file
 *  @brief
 *  CWndThumb
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2022-7-11
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2022 miragekiller                             
 *  All rights reserved  
 */

#ifndef WNDTHUMB_H
#define WNDTHUMB_H

#include "WndPerformance.h"


class CWndThumb
//     : public TM::CTuiWidgetImpl<CWndThumb> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CWndThumb>
//     , public TM::CTuiMsgFilter
    : public CWndRefeshWindow<CWndThumb, 3000>
{
public:
    typedef  CWndRefeshWindow<CWndThumb, 3000> base_class;
    CWndThumb();
    ~CWndThumb();
    
public:
    enum { IDD = IDC_WINDOW_THUMB };
    
    BEGIN_TUIMSG_MAP (CWndThumb)
//     BEGIN_MSG_MAP (CWndThumb)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
        MESSAGE_HANDLER     (WM_PRINTCLIENT,                OnPrintClient)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)
//         COMMAND_ID_HANDLER  (IDOK,                      OnOk)
//         COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        CHAIN_MSG_MAP(base_class)
    END_MSG_MAP()
    
//     TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnPrintClient(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
//     TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    //  CAniProc
    void OnTimer(TMHTimer, ...);

    CWndPanel& GetPanel();

};


#endif // WNDTHUMB_H