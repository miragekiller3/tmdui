#ifndef MAIN_H
#define MAIN_H

#include "Resource.h"
#include "tuires.h"
#include "../h/zoom.h"

class CMain
    : public TM::CTuiWidgetImpl<CMain, TM::CTuiWindow>
    , public CZoomImplT<CMain>
{
public:
    enum { IDD = IDD_TUI_MANAGE_10002 };
    
public:
    BEGIN_TUIMSG_MAP        (CMain)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
        COMMAND_ID_HANDLER  (IDOK,                      OnCancel)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
//         COMMAND_ID_HANDLER  (IDC_ZOOMIN,                OnZoomIn)
//         COMMAND_ID_HANDLER  (IDC_ZOOMOUT,               OnZoomOut)
//         COMMAND_ID_HANDLER  (IDC_ADD100,                OnAdd100)
        COMMAND_ID_HANDLER  (IDC_ADD1000,               OnAdd1000)
        COMMAND_ID_HANDLER  (IDC_ADDNOBIND,             OnAdd1000)
        
        COMMAND_ID_HANDLER  (IDC_DIR,                   OnAddDir)
        ROUTE_OID_HANDLER   (IDC_LISTBOX_20034, CM_HITTEST, OnHitTest)
        CHAIN_MSG_MAP(CZoomImpl)
    END_TUIMSG_MAP()

    BEGIN_MSG_MAP        (CMain)
    END_MSG_MAP()

    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnZoomIn(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnZoomOut(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnAdd100(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnAdd1000(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnAddDir(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnHitTest(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

    CMain()
    {
    }

    void Add(int n, int id);
};    


#endif // MAIN_H