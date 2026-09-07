/**
 *  @file
 *  @brief
 *  CWndMemRecorder
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025-2-13
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2025 miragekiller                             
 *  All rights reserved  
 */

#ifndef WNDMEMRECORDER_H
#define WNDMEMRECORDER_H

#include "tuires.h"
#include "tmani.h"
#include "../../tmdebugext.h"
#include "../../tmgdix.h"

class CWndMemRecorder
    : public TM::CTuiWidgetImpl<CWndMemRecorder> //, TM::CTuiWindow>
    , public TM::CAniImpl<CWndMemRecorder>
//     : public TM::CTuiDialogImpl<CWndMemRecorder>
//     , public TM::CTuiMsgFilter
{
public:
    enum { IDD = IDC_WINDOW_MEM_RECORDER };
    enum { eTimeCount = 2000 };
    enum { eObjCount = 10 };

    typedef TM::CMap<time_t, CMemDump::CObjMap> CObjTimeList;

    CComPtr<TM::ITuiPic> m_ptrBk;
    TM::CTexture m_tex;
    CObjTimeList m_aObj;
    int m_nMaxY;

public:
    CWndMemRecorder();
    ~CWndMemRecorder();
    
    BEGIN_TUIMSG_MAP (CWndMemRecorder)
//     BEGIN_MSG_MAP (CWndMemRecorder)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)
        MESSAGE_HANDLER     (WM_SHOWWINDOW,             OnShowWindow)
        MESSAGE_HANDLER     (WM_PRINTCLIENT,            OnPrintClient)
//         COMMAND_ID_HANDLER  (IDOK,                      OnOk)
//         COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnShowWindow(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        if (wParam)
            SetAni(1000);
        bHandled = false;
        return 0;
    }
    TMResult OnPrintClient(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    //CAniImpl
    void OnAni(TMHAni hAni, ...);

};


#endif // WNDMEMRECORDER_H