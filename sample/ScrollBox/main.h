#ifndef MAIN_H
#define MAIN_H

#include "Resource.h"
#include "tuires.h"
#include "../h/zoom.h"

class CMain
    : public TM::CTuiWidgetImpl<CMain, TM::CTuiWindow>
    , public CZoomImplT<CMain>
{
private:
    int m_nChilds;

public:
    enum { IDD = IDD_TUI_MANAGE_10002 };
    
public:
    BEGIN_TUIMSG_MAP        (CMain)
        COMMAND_ID_HANDLER  (IDOK,                      OnCancel)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        COMMAND_ID_HANDLER  (IDC_ZOOMIN,                OnZoomIn)
        COMMAND_ID_HANDLER  (IDC_ZOOMOUT,               OnZoomOut)
//         COMMAND_ID_HANDLER  (IDC_ADD100,                OnAdd100)
        COMMAND_ID_HANDLER  (IDC_ADD1000,               OnAdd1000)
        COMMAND_ID_HANDLER  (IDC_ADD1S,                 OnAdd1S)
        ROUTE_ID_HANDLER    (IDC_WINDOW_20024,  CM_SCROLLTO ,        OnScrollTo)
        CHAIN_MSG_MAP       (CZoomImpl)
    END_TUIMSG_MAP()
    
    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnZoomIn(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnZoomOut(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnAdd100(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnAdd1000(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnAdd1S(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    
    TMResult OnScrollTo(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
    {
        bHandled = false;
        return 0;
    }

    CMain()
        : m_nChilds(0)
    {
    }

    void Add(int n);
};    


#endif // MAIN_H