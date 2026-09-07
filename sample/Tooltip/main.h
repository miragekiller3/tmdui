#ifndef MAIN_H
#define MAIN_H

#include "../h/zoom.h"
#include "tmtimer.h"

class CMain
    : public TM::CTuiWidgetImpl<CMain, TM::CTuiWindow>
    , public CZoomImplT<CMain>
    , public TM::CTimerImpl<CMain>
{
public:
    enum { IDD = IDD_TUI_MANAGE_10002 };
    
public:
    BEGIN_TUIMSG_MAP           (CMain)
        COMMAND_ID_HANDLER  (IDOK,                      OnCancel)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        COMMAND_HANDLER     (IDC_EDIT_20024, EN_UPDATE, OnUpdate1)
        COMMAND_HANDLER     (IDC_EDIT_20025, EN_UPDATE, OnUpdate1)
        COMMAND_HANDLER     (IDC_PUSHBUTTON_21003, BN_CLICKED, OnHideC)
        COMMAND_HANDLER     (ID_DRAG1,             BN_CLICKED, OnResetHInt)
        COMMAND_HANDLER     (ID_DRAG2,             BN_CLICKED, OnResetHInt)

        ROUTE_HANDLER       (WM_KEYDOWN,                OnKeyDown)
        ROUTE_OID_HANDLER   (IDC_PUSHBUTTON_21004, CM_HINTONADDTOOL, OnHintAddTool)
        ROUTE_OID_HANDLER   (IDC_PUSHBUTTON_21004, CM_HINTONSHOW,    OnHintShow)

        COMMAND_ID_HANDLER  (IDC_CHECKBOX_1000,         OnSetting)
        COMMAND_ID_HANDLER  (IDC_CHECKBOX_1001,         OnSetting)

        
//         ROUTE_HANDLER       (CM_HINT_INIT,              OnHintInit)
        
//         COMMAND_ID_HANDLER  (IDC_ZOOMIN,                OnZoomIn)
//         COMMAND_ID_HANDLER  (IDC_ZOOMOUT,               OnZoomOut)        
        CHAIN_MSG_MAP(CZoomImpl)
    END_TUIMSG_MAP()
    
    TMResult OnUpdate1(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnHideC(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnResetHInt(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSetting(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    //     TMResult OnZoomIn(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnZoomOut(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnHintInit(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnKeyDown(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnHintAddTool(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnHintShow(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

    // CTimerImpl
    void OnTimer(...);

    static void ResetPos(WINDOWPOS* wp);
};    


#endif // MAIN_H