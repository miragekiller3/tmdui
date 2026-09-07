/**
 *    @file
 *    @brief
 *    CDlgMain
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2017-2-28
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#ifndef ZOOM_H
#define ZOOM_H

#include "tuires.h"
#include "tmplatform_win.h"
#include "tmtime.h"
#include "tmgdix.h"
#include "tmatl.h"


inline void SetR2L(TMHModule hModule, TMBool bRtl)
{
    DbgAssert(bRtl == 0 || bRtl == 1);
    LPCTSTR p[2] =
    {
        TM_T("<tmdui encoding=\"utf-8\" localid=\"1033\"></tmdui>"), // English(AM)
        TM_T("<tmdui encoding=\"utf-8\" localid=\"1025\"></tmdui>"), // Arabic
    };
    _tuiApp.LoadXml(hModule, 0, p[bRtl]);
}

#ifndef IDC_R2L
#   define IDC_R2L -9999
#endif

#ifndef IDC_DEV
#   define IDC_DEV -9999
#endif

template <class T>
class CZoomImplT
{
public:
    typedef CZoomImplT<T> CZoomImpl;
    
public:
    CZoomImplT()
    {
    }

    BEGIN_TUIMSG_MAP (CZoomImplT)
        COMMAND_ID_HANDLER  (IDC_ZOOMIN,        OnZoomIn)
        COMMAND_ID_HANDLER  (IDC_ZOOMOUT,       OnZoomOut)     
        COMMAND_ID_HANDLER  (IDC_R2L,           OnR2L)     
        COMMAND_ID_HANDLER  (IDC_DEV,           OnDev)     
    END_MSG_MAP()
    
    TMResult OnZoomIn(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        TM::CTuiWindow* p = static_cast<T*>(this);
        p->SetDPI(p->GetDPI() - 10);
        return 0;
    }
    
    TMResult OnZoomOut(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        TM::CTuiWindow* p = static_cast<T*>(this);
        p->SetDPI(p->GetDPI() + 10);
        return 0;
    }

    TMResult OnR2L(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        TM::CTuiWindow* p = static_cast<T*>(this);
        SetR2L(p->GetModule(), (TM::CTuiButton((TM::HTUI)hWndCtl)).GetCheck());
        return 0;
    }

    TMResult OnDev(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        int n = TM::CTuiButton((TM::HTUI)hWndCtl).GetCheck();
        EGXDevice dev[] = { eGXGraDevGDI, eGXGraDevD3D9, eGXGraDevD3D11, eGXGraDevGL };
        gxSetDefaultGraDevices(true, dev[n]);
        return 0;
    }
};


// inline int SetDPIAware()
// {
//     TM::CAppAT::SetDPIAware();
//     return 0;
// }
// 
// static int x = SetDPIAware();


template <int nid>
class CFpsWindow : public TM::CTuiDialogImpl<CFpsWindow<nid> >
{
public:
    TM::CFps m_fps;

    BEGIN_MSG_MAP (CFpsWindow)
        MESSAGE_HANDLER     (WM_PAINT,             OnPaint)
    END_MSG_MAP()

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        TM::CString str;
        str.Format(L"avg render time:%g, FPS:%g", m_fps.GetAvgDTime().ToMillionSecondF(), m_fps.GetFPS());
        Item(nid).SetWindowText(str);

        m_fps.Reset();
        this->DefWindowProc(uMsg, wParam, lParam);
        m_fps.StepUp();
        //bHandled = false;
        //::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
//         CPaintDC dc(m_hWnd);
        return 0;
    }
};



#endif // DLGMAIN_H