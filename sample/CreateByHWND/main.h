#ifndef MAIN_H
#define MAIN_H

#include "Resource.h"
#include "tuires.h"
#include "../h/zoom.h"

class CWinTest : public CWindowImpl<CWinTest>
{
    BEGIN_MSG_MAP(CWinTest)
    END_MSG_MAP()
};


class CMain
    : public TM::CTuiWidgetImpl<CMain, TM::CTuiWindow>
    , public CZoomImplT<CMain>
{
public:
    enum { IDD = IDD_MANAGE_21001 };

private:
    CWinTest m_hTest;

public:
    BEGIN_TUIMSG_MAP        (CMain)
        COMMAND_ID_HANDLER  (IDOK,                          OnCancel)
        COMMAND_ID_HANDLER  (IDCANCEL,                      OnCancel)
        COMMAND_ID_HANDLER  (IDC_ZOOMIN,                    OnZoomIn)
        COMMAND_ID_HANDLER  (IDC_ZOOMOUT,                   OnZoomOut)
//         ROUTE_ID_HANDLER    (IDC_ZOOMOUT,  WM_LBUTTONDOWN,    OnTest)
        CHAIN_MSG_MAP(CZoomImpl)
    END_TUIMSG_MAP()
    
    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnTest(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
//     {
//         DestroyWindow();
//         bHandled = false;
//         return 0;
//     }

    CMain()
    {
    }

    void Create();

};    


#endif // MAIN_H