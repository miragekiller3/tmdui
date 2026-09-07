/**
 *    @file
 *    @brief
 *    CWndPerformance
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-6-27
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#ifndef WNDPERFORMANCE_H
#define WNDPERFORMANCE_H

#include "tuires.h"
#include "tmtimer.h"

class CWndPanel;


template <class T, int nDelay = 1000>
class CWndRefeshWindow
    : public TM::CTuiWidgetImpl<T>
    , public TM::CTimerImpl<T>
{
public:
    typedef CWndRefeshWindow base_wnd;
    
    BEGIN_TUIMSG_MAP (CWndRefeshWindow)
        MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
        MESSAGE_HANDLER     (WM_SHOWWINDOW,             OnShowWindow)
    END_MSG_MAP()

    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        bHandled = false;
        this->KillAni();
        return 0;
    }

    TMResult OnShowWindow (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        if (wParam)
            this->SetAni(nDelay);
        else
            this->KillAni();
        bHandled = false;
        return 0;
    }
};


class CWndPerformance
//      : public TM::CTuiWidgetImpl<CWndPerformance, CWndRefeshWindow> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CWndPerformance>
//     , public TM::CTuiMsgFilter
//     , public TM::CAni
    : public CWndRefeshWindow<CWndPerformance>
{
public:
    CWndPerformance();
    ~CWndPerformance();
    
public:
    enum { IDD = IDC_MEMO_PERFORMANCE };
    
    BEGIN_TUIMSG_MAP (CWndPerformance)
//     BEGIN_MSG_MAP (CWndPerformance)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         MESSAGE_HANDLER     (WM_SHOWWINDOW,                OnShowWindow)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
//         COMMAND_ID_HANDLER  (IDOK,                      OnOk)
//         COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        MESSAGE_HANDLER     (CM_PRECOMMAND,             OnPreCommand)
        CHAIN_MSG_MAP(base_wnd)
    END_MSG_MAP()
//     
//     TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnShowWindow (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
// //     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnPreCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);
    CWndPanel& GetPanel();

    //  CAniProc
    void OnTimer(TMHTimer, ...);
};


class CWndObjInfo
    //      : public TM::CTuiWidgetImpl<CWndPerformance, CWndRefeshWindow> //, TM::CTuiWindow>
    //     : public TM::CTuiDialogImpl<CWndPerformance>
    //     , public TM::CTuiMsgFilter
    //     , public TM::CAni
    : public CWndRefeshWindow<CWndObjInfo>
{
public:
    enum { IDD = IDC_MEMO_INFO };
    
    //     BEGIN_TUIMSG_MAP (CWndPerformance)
    // //     BEGIN_MSG_MAP (CWndPerformance)
    // //         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
    //         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
    //         MESSAGE_HANDLER     (WM_SHOWWINDOW,                OnShowWindow)
    // //         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
    //         COMMAND_ID_HANDLER  (IDOK,                      OnOk)
    //         COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    //     END_MSG_MAP()
    //     
    //     TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    //     TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    //     TMResult OnShowWindow (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    // //     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    //     TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    //     TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    
    //CTuiMsgFilter
    //     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);
    CWndPanel& GetPanel();
   
    //  CAniProc
    void OnTimer(TMHTimer, ...);
};



#endif // WNDPERFORMANCE_H