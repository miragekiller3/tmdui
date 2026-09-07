#ifndef MAIN_H
#define MAIN_H

#include "Resource.h"
#include "tuires.h"
#include "../h/zoom.h"

class CMain
    : public TM::CTuiWidgetImpl<CMain, TM::CTuiWindow>
    , public CZoomImplT<CMain>
    , public TM::CAniImpl<CMain>
{
public:
    enum { IDD = IDD_TUI_MANAGE_10002 };

public:
    BEGIN_TUIMSG_MAP        (CMain)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
        MESSAGE_HANDLER     (CM_LANGUAGECHANGED,        OnLanguageChagned)

        COMMAND_ID_HANDLER  (IDOK,                      OnCancel)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
//         COMMAND_ID_HANDLER  (IDC_ZOOMIN,                OnZoomIn)
//         COMMAND_ID_HANDLER  (IDC_ZOOMOUT,               OnZoomOut)
        
        COMMAND_ID_HANDLER  (IDC_SELFONT,               OnSelFont)
        COMMAND_ID_HANDLER  (IDC_APPLYFONT,             OnApplyFont)
        COMMAND_ID_HANDLER  (IDC_APPLYFONT2,            OnApplyFont)
        
        
        COMMAND_ID_HANDLER  (IDC_CLR,                   OnSelColor)
        COMMAND_ID_HANDLER  (IDC_APPLYCLR,              OnApplyColor)

        COMMAND_ID_HANDLER  (IDC_SENDHTM,               OnSendHtml)
        COMMAND_ID_HANDLER  (IDC_SENDTXT,               OnSendTxt)

        COMMAND_HANDLER     (IDC_RADIOBUTTON_21006, BN_CLICKED, OnRadio)
        COMMAND_HANDLER     (IDC_RICH_TARGET, EN_UPDATE, OnEnUpdate)
        COMMAND_HANDLER     (IDC_LISTBOX_22000, LBN_SELCHANGE, OnSelChange)


        ROUTE_ID_HANDLER    (IDC_CLR, WM_PRINTCLIENT,   OnPrintColor)

        CHAIN_MSG_MAP(CZoomImpl)

    END_TUIMSG_MAP()
    
    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnLanguageChagned(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnZoomIn(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnZoomOut(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnSelFont(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnApplyFont(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    
    TMResult OnSelColor(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnApplyColor(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnSendHtml(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSendTxt(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnRadio(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnEnUpdate(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSelChange(WORD wNotifyCode, WORD wID, void* hWndCtl, TMBool& bHandled);

    TMResult OnPrintColor(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

    void OnAni(...);

    CMain()
    {
    }

    void GetFont(LOGFONT& ft);
    void SetFont(const LOGFONT& ft);
    void CreateItem(int oid);
};    


#endif // MAIN_H