#ifndef ANICIRCLE_H
#define ANICIRCLE_H


class CCircleWnd
    : public TM::CTuiWidgetImpl<CCircleWnd>
    , public TM::CAniImpl<CCircleWnd>
{
private:
    enum { e_Step = 1 };
    enum { e_Size = 100 };

    TM::CTuiPoint    m_pt[10];
    TM::CTuiPicPtr    m_pic;
    int             m_nAlpha;

public:
    BEGIN_TUIMSG_MAP        (CFadeWnd)
        MESSAGE_HANDLER     (WM_PRINTCLIENT,    OnPrintClient)
    END_MSG_MAP()

    TMResult OnPrintClient(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        DefWindowProc (uMsg, wParam, lParam);

//         int rx = m_nRandX / (SHRT_MAX / 20);
//         int ry = m_nRandY / (SHRT_MAX / 20);
//         DbgTrace("rand x:%d, %d, %d, %d", m_nRandX, m_nRandY, rx, ry);

        for (int j = 0; j < 10; ++j)
        {
            int dx = j * e_Size / 10;
            TM::CPoint ptx(m_pt[j]);
            TM::CRect rc(ptx.x - dx, ptx.y - dx, ptx.x + dx, ptx.y + dx);
//             rc.OffsetRect(rx * j / 10, ry * j / 10);
            m_pic->SetAlpha((10 - j) * m_nAlpha / 15);
            m_pic->Render(0, 0, rc);
        }

        return 0;
    }

    void OnAni(TMHAni hAni, ...)
//    virtual void OnAni(TMHAni tm, TMUInt nStep, TMUInt nFrame, void* data)
    {
        Invalidate();

        TM::CPoint pt;
        TM::CTuiPoint ptx;
        GetCursorPos(&pt);
        ScreenToClient(&pt, 1, &ptx);

//         DbgTrace("curpos x:%d, %d, %d, %d", pt, m_pt);

        if (ptx == m_pt[0])
        {
            m_nAlpha -= 5;
            if (m_nAlpha <= 0)
                m_nAlpha = 0;
        }
        else
        {
            m_nAlpha += 30;
            if (m_nAlpha > 255)
                m_nAlpha = 255;
        }

        m_pt[0] = ptx;

        for (int j = 1; j < 10; ++j)
        {
            ptx.x = m_pt[j].x + MulDiv(ptx.x - m_pt[j].x, 70, 300);
            ptx.y = m_pt[j].y + MulDiv(ptx.y - m_pt[j].y, 70, 300);
            m_pt[j] = ptx;
        }

//         m_nRandX += rand() % 10000;
//         m_nRandY += rand() % 10000;
    }


    CCircleWnd()
        : m_nAlpha (255)
        , m_pic (IDP_BMP_20020)
    {
//        m_pic =  _tuiApp.GetPic(_Module.GetResourceInstance(),);
    }

    ~CCircleWnd()
    {
    }

    void EnabelAni (TM::HTUI hWgt, TMBool bCreate)
    {
        if (bCreate && !m_hWnd)
        {
            SetAni(20);
            SubclassWindow(hWgt);
            ShowWindow(SW_SHOW);
        }
        else if (m_hWnd && !bCreate)
        {
            ShowWindow(SW_HIDE);
            KillAni();
            UnsubclassWindow();
        }
    }

};    






#endif // ANICIRCLE_H