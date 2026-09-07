#ifndef WNDVIDEO_H
#define WNDVIDEO_H

#include "Resource.h"
#include "tuires.h"

class CWndVideo : public TM::CTuiWidgetImpl<CWndVideo, TM::CTuiVideo>
{
protected:
    TM::CGraphBuilderCapture m_cg;
    TM::CSeconds m_secNow;
//     TM::CCurstomVideo m_cv;

public:
    enum { IDD = IDW_VIDEO_21025 };

public:
    BEGIN_TUIMSG_MAP            (CWndVideo)
        COMMAND_ID_HANDLER      (IDC_CAMERA,                OnCamera)
        COMMAND_ID_HANDLER      (IDC_OPEN,                  OnOpen)
        COMMAND_ID_HANDLER      (IDC_PLAY,                  OnPlay)
        COMMAND_ID_HANDLER      (IDC_PAUSE,                 OnPause)
        COMMAND_ID_HANDLER      (IDW_TRACKBAR_30082,        OnTrack)
        COMMAND_ID_HANDLER      (IDC_TRACKBAR_1116,         OnVolume)
        
        MESSAGE_HANDLER         (CM_PRECOMMAND,             OnPreCommand)
        MESSAGE_HANDLER         (WM_LBUTTONDOWN,            OnLButtonDown)

        COMMAND_ID_HANDLER      (IDW_VOL,                   OnVol)

    END_TUIMSG_MAP()
    
    TMResult OnCamera(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnOpen(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnPlay(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnPause(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnTrack(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnVolume(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnPreCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnLButtonDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    TMResult OnVol(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    CWndVideo()
    {
    }
};    


#endif // WNDVIDEO_H