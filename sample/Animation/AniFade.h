#ifndef ANIFADE_H
#define ANIFADE_H


class CFadeWnd
    : public TM::CTuiWidgetImpl<CFadeWnd>
    , public TM::CAniImpl<CFadeWnd>
{
private:
    enum { e_Step = 1 };

    int m_nAlpha;
    int m_forward;

public:
    BEGIN_TUIMSG_MAP        (CFadeWnd)
        MESSAGE_HANDLER     (WM_PRINTCLIENT,    OnPrintClient)
    END_MSG_MAP()

    TMResult OnPrintClient(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        TM::CTuiPicPtr p(IDP_BMP_10004);
        int nAlpha = m_nAlpha / e_Step;
        p->SetAlpha(nAlpha);
        DefWindowProc (uMsg, wParam, lParam);
        p->SetAlpha(255);
        return 0;
    }

    void OnAni(TMHAni hAni, TMClock tmNow, TMUInt nFrame)
    {
        Invalidate();

        if (m_forward)
        {
            m_nAlpha = 255 - nFrame;
            if (m_nAlpha <= 0)
            {
                ShowWindow(SW_HIDE);
                KillAni();
                UnsubclassWindow();
            }
        }
        else
        {
            m_nAlpha = nFrame;
            if (m_nAlpha >= 255)
            {
                KillAni();
                UnsubclassWindow();
            }
        }

    }

    CFadeWnd()
    {
    }

    void CreateAni (TM::HTUI hSrc, TMBool bFoword)
    {
        if (!m_hWnd)
        {
            SetAni(5);
            SubclassWindow (hSrc);
            m_nAlpha = bFoword ? 255 : 0;
        }
        m_forward = bFoword;
        ShowWindow (SW_SHOW);
        OutPut (_T("CAniFade: CreateAni(HTUI:%x)"), hSrc);
    }
};    






#endif // ANIFADE_H