/**
 *    @file
 *    @brief
 *    CDlgStressTesting
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-12-12
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGSTRESSTESTING_H
#define DLGSTRESSTESTING_H

#include "tuires.h"


#define WM_ISAPPWIN     WM_USER + 300       // lp: HWND; return: bAppWindow;
#define WM_MAKEFRONT    WM_USER + 301       // wp: TMBool bMax


class CDlgStressTesting
    : public TM::CTuiWidgetImpl<CDlgStressTesting> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgStressTesting>
//     , public TM::CTuiMsgFilter
{
private:
//     TCHAR m_c[1024];
    
public:
    CDlgStressTesting();
    ~CDlgStressTesting();
    
public:
    enum { IDD = IDD_TUI_STRESS_TESTING };
    
    BEGIN_TUIMSG_MAP (CDlgStressTesting)
//     BEGIN_MSG_MAP (CDlgSpyMsg)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
        MESSAGE_HANDLER     (WM_ISAPPWIN,               OnIsAppWin)
        MESSAGE_HANDLER     (WM_MAKEFRONT,              OnMakeFront)
        MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnIsAppWin (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMakeFront (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};

DECLARE_DLL_FUNC(dll_kernel32, HANDLE, WINAPI, OpenThread, (DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId));


typedef TM::CVector<DWORD> CTIDList;
void ListThreads(DWORD pid, CTIDList& aTid);


#endif // DLGSTRESSTESTING_H