#ifndef MAIN_H
#define MAIN_H

#include "Resource.h"
#include "tuires.h"
#include "WndVideo.h"
#include "../h/zoom.h"
#include "tmtimer.h"

class CMain
    : public TM::CTuiWidgetImpl<CMain, TM::CTuiWindow>
    , public CZoomImplT<CMain>
{
public:
    enum { IDD = IDD_MANAGE_21001 };
    
public:
    typedef TM::CVector<TM::CTuiVideo> CVideoList;
    CVideoList      m_aViedo;
    LPSTR           m_lpCmdLine;
    TM::CTimerID    m_tmHide;

public:
    BEGIN_TUIMSG_MAP        (CMain)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
        MESSAGE_HANDLER     (WM_SIZE,                   OnSize)
        MESSAGE_HANDLER     (WM_CLOSE,                  OnClose)
        ROUTE_HANDLER       (WM_LBUTTONDBLCLK,          OnLButtonDbClick)
        ROUTE_HANDLER       (WM_MOUSEMOVE,              OnMouseMove)

        COMMAND_ID_HANDLER  (IDOK,                      OnCancel)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
//         COMMAND_ID_HANDLER  (IDC_ZOOMIN,                OnZoomIn)
//         COMMAND_ID_HANDLER  (IDC_ZOOMOUT,               OnZoomOut)

        COMMAND_ID_HANDLER  (IDC_4WIN,                  On4Win)
        COMMAND_ID_HANDLER  (IDC_PICINPIC,              OnPicInPic)

        COMMAND_ID_HANDLER  (IDC_MIRROR,                OnMirror)
        COMMAND_ID_HANDLER  (IDC_ROTATE,                OnRotate)

        COMMAND_ID_HANDLER  (IDC_OPEN,                  OnOpen)
        COMMAND_ID_HANDLER  (IDC_PLAY,                  OnPlay)

//         
//         COMMAND_ID_HANDLER  (IDC_OPEN,                  OnOpen)
//         COMMAND_ID_HANDLER  (IDC_PLAY,                  OnPlay)
//         COMMAND_ID_HANDLER  (IDC_PAUSE,                 OnPause)
//         
//         
// 
//         COMMAND_ID_HANDLER  (IDC_APPLYFONT,             OnApplyFont)
//         
//         COMMAND_ID_HANDLER  (IDC_CLR,                   OnSelColor)
//         COMMAND_ID_HANDLER  (IDC_APPLYCLR,              OnApplyColor)
// 
//         COMMAND_ID_HANDLER  (IDC_SENDHTM,               OnSendHtml)
//         COMMAND_ID_HANDLER  (IDC_SENDTXT,               OnSendTxt)
// 
        CHAIN_MSG_MAP       (CZoomImpl)
    END_TUIMSG_MAP()

    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSize(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnClose(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnLButtonDbClick(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnMouseMove(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnZoomIn(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnZoomOut(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult On4Win(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnPicInPic(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnMirror(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnRotate(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnApplyFont(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    
    TMResult OnSelColor(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnApplyColor(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnSendHtml(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSendTxt(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);


    TMResult OnOpen(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnPlay(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    CMain()
    {
    }

    void ShowFloat(bool bShow);
    bool SetFullScreen(bool bExit);
    void OnTimer(TMHTimer hTimer, TMClock tmNow);

    void GetFont(LOGFONT& ft);
    void SetFont(const LOGFONT& ft);
    void Layout(int nCheck);
};    


#endif // MAIN_H