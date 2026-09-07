#include "stdafx.h"
#include "wndvideo.h"
#include "tmwnd.h"
//#include "tmgdix.h"
#include "tmtime.h"

// #include "athtml.cpp"

using namespace TM;



//static CWndVideo::Register<0> s_callback_CMain;


//  class CAAA;
// 
// TM::CTuiPtr<CAAA> s_ptr;


TMResult CWndVideo::OnCamera(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//     this->SetSrc(0);
//     this->Load();
// 
//     IBaseFilter* pfRender = this->GetRender();
// 
//     m_cg.CloseVMR(pfRender);
//     m_cg.CreateVMR(pfRender);
    CGraphBuilderCapture::CMonikerList aList;
    CGraphBuilderCapture::EnumerateDevices(aList);
    if (aList.size())
    {
        this->SetSrcMoniker(aList[0]);
        this->Load();

//         m_cg.Open(aList[0], 0, pfRender);
//         m_cg.RawSetState(State_Running);
    }

//     if (m_cv.m_ptrGraphBuilder)
//         m_cv.CloseVMR(pfRender);
//     m_cv.CreateVMR(pfRender);
//     m_cv.InitGraph(m_cv.m_ptrGraphBuilder);
//     m_cv.RawRenderFile(L"d:\\1.jpg");
//     m_cv.RawSetState(State_Running);

    return 0;
}

TMResult CWndVideo::OnOpen(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TMCStr lpcstrFilter = 
        _T("All Files (*.*)\0*.*\0")
        _T("");
    
    TM::CFileOpenDialog dlg;
    int nRet = dlg.DoModal (GetHWND(), lpcstrFilter, 0, 0, 0, OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
    if (nRet != IDOK)
        return 0;

    if (m_cg)
    {
        m_cg.DetachVideo();
        m_cg.RawClose();
    }
    this->SetSrc(dlg.GetPathName());
    this->Load();

    m_secNow = -1;
    bHandled = false;

    return 0;
}

TMResult CWndVideo::OnPlay(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    if (this->GetStyle() & VS_PLAYING)
        this->Pause();
    else
        this->Play();
    bHandled = false;
    return 0;
}

TMResult CWndVideo::OnPause(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    this->Pause();
    this->SetCurrentTime(0);
    return 0;
}

TMResult CWndVideo::OnTrack(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    if (wNotifyCode >= SB_LINEUP && wNotifyCode <= SB_ENDSCROLL)
    {
        CTuiTrackBar tb(Item(IDW_TRACKBAR_30082));
        TMUInt64 n = tb.GetPos();
        this->SetCurrentTime(n * 1000000000);
    }
    return 0;
}

TMResult CWndVideo::OnVolume(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiTrackBar tb((HTUI)hWndCtl);
    float n = tb.GetPos();
    this->SetVolume(n / 100.0f);
    return 0;
}


TMResult CWndVideo::OnPreCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    switch (HIWORD(wParam))
    {
    case CTuiVideo::VN_SEEKING:
    case CTuiVideo::VN_PAUSED:
    case CTuiVideo::VN_LOAD:
        {
            CSeconds nPos = this->GetCurrentTime() / 1000000000;
            if (m_secNow == nPos)
                return 0;
            m_secNow = nPos;
            CSeconds nAll = this->GetDuration() / 1000000000;
            CTuiTrackBar wndBar(Item(IDW_TRACKBAR_30082));
            wndBar.SetRange(0, nAll);
            wndBar.SetPos(nPos);

            CString str;
            str.Format(L"%02d:%02d:%02d/%02d:%02d:%02d",
                nPos.GetHours(), nPos.GetMinuteOfHour(), nPos.GetSecondOfMinute(),
                nAll.GetHours(), nAll.GetMinuteOfHour(), nAll.GetSecondOfMinute());
            Item(IDW_STATIC_30083).SetWindowText(str);
        }
        return 0;
    }
    return 0;
}

TMResult CWndVideo::OnLButtonDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (GetStyle() & VS_PLAYING)
        Pause();
    else
        Play();
    return 0;
}

TMResult CWndVideo::OnVol(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    Item(IDC_TRACKBAR_1116).ShowWindow(CTuiButton((TM::HTUI)hWndCtl).GetCheck());
    return 0;
}