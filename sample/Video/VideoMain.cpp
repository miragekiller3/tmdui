#include "stdafx.h"
#include "videomain.h"
//#include "../../../tmgdix.h"

// #include "athtml.cpp"

using namespace TM;


static int s_nGrid[] = { 1, 2, 3 };


// static CMain::Register<0> s_callback_CMain;


//  class CAAA;
// 
// TM::CTuiPtr<CAAA> s_ptr;


void CMain::ShowFloat(bool bShow)
{
    Item(IDC_WINDOW_37004).ShowWindow(bShow);
    for (CVideoList::recorder rec(m_aViedo); rec; ++rec)
    {
        CTuiVideo wnd = rec.get_data();
        wnd.Item(IDW_WINDOW_30078).ShowWindow(bShow);
    }
}

bool CMain::SetFullScreen(bool bExit)
{
    bool b = false;
    DWORD s = GetStyle();
    int nCmdShow;
    if (s & DS_FULLSCREEN)
    {
        (s &= ~DS_FULLSCREEN), (s |= WS_CAPTION);
        nCmdShow = SW_RESTORE;
        b = true;
    }
    else if (bExit)
        return false;
    else
    {
        (s &= ~WS_CAPTION), (s |= DS_FULLSCREEN);
        nCmdShow = SW_MAXIMIZE;
    }
    SetStyle(s);
    ::ShowWindow(GetHWND(), nCmdShow);
    return b;
}


void CMain::OnTimer(TMHTimer hTimer, TMClock tmNow)
{
    TUIHITINFO wh;
    _tuiApp.GetMouseHotState(wh);
    if (!wh.hWnd || CTuiWidget(wh.hWnd).GetDlgCtrlObjectID() == IDW_VIDEO_21025)
    {
        m_tmHide.KillTimer();
        ShowFloat(false);
    }
}


TMResult CMain::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_aViedo.push_back((HTUI)Item(IDW_VIDEO_21025));
    if (m_lpCmdLine && m_lpCmdLine[0])
    {
        TM::CStrViewA cmd(m_lpCmdLine);
        cmd = cmd.Mid(1, cmd.length() - 2);
        m_aViedo[0].SetSrc(TM_A2T(cmd));
    }
//     ::MessageBox(0, TM_A2T(m_lpCmdLine), 0, MB_OK);

    return 0;
}

TMResult CMain::OnSize(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    CTuiButton but(Item(IDC_4WIN));
    Layout(but.GetCheck());
    return 0;
}

TMResult CMain::OnClose(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    EndDialog(IDCANCEL);
    return 0;
}


TMResult CMain::OnLButtonDbClick(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    bHandled = false;
    SetFullScreen(false);
    return 0;
}

TMResult CMain::OnMouseMove(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    bHandled = false;
    if (!m_tmHide)
        ShowFloat(true);
    m_tmHide.KillTimer();
    if (m_aViedo[0].GetStyle() & VS_PLAYING)
        m_tmHide.SetTimer(3000, this);
    return 0;
}


TMResult CMain::OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    SetFullScreen(true);
    return 0;
}

// static int s_zoom = 100;
// 
// TMResult CMain::OnZoomIn(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
// //     int nx = GetDPI();
// //     nx -= 10;
// //     SetDPI(nx);
//     s_zoom -= 10;
//     _tuiApp.SetZoom(s_zoom);
//     return 0;
// }
// 
// TMResult CMain::OnZoomOut(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
// //     int nx = GetDPI();
// //     nx += 10;
// //     SetDPI(nx);
//     s_zoom += 10;
//     _tuiApp.SetZoom(s_zoom);
//     return 0;
// }

static void _CMain_DestroyChild(CTuiWidget wnd)
{
    CTuiWidget wc(wnd.GetWindow(GW_CHILD));
    if (wc)
        wc.DestroyWindow();
}

TMResult CMain::On4Win(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiButton btn((HTUI)hWndCtl);
    int nCheck = btn.GetCheck();
    Layout(btn.GetCheck());
    return 0;
}


TMResult CMain::OnPicInPic(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiWidget wnd1(Item(IDW_VIDEO_21025));
    CTuiWidget wnd2;
    wnd2.Create(wnd1, IDW_VIDEO_21025);
    wnd2.SetLayOut(ANCHOR_LEFT | ANCHOR_TOP);
    wnd2.SetStyle(WS_VISIBLE | WS_OVERLAPPEDWINDOW);
    wnd2.MoveWindow(100, 100, 400, 300);
    wnd2.BringWindowToTop();
    return 0;
}

TMResult CMain::OnMirror(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TMUInt32 dwStyle = m_aViedo[0].GetStyle();
    if (dwStyle & VS_R2L)
        m_aViedo[0].SetStyle(dwStyle & ~VS_R2L);
    else
        m_aViedo[0].SetStyle(dwStyle | VS_R2L);
    return 0;
}

TMResult CMain::OnRotate(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    int n = CTuiButton((HTUI)hWndCtl).GetCheck();
    TMUInt32 dwStyle = m_aViedo[0].GetStyle();
    TMUInt32 dwRotate = (dwStyle & VS_ROTATE_MASK);
    dwStyle &= ~VS_ROTATE_MASK;
    switch(dwRotate)
    {
    case VS_ROTATE_0:       dwStyle |= VS_ROTATE_90;    break;
    case VS_ROTATE_0R:      dwStyle |= VS_ROTATE_90R;   break;
    case VS_ROTATE_90:      dwStyle |= VS_ROTATE_180;   break;
    case VS_ROTATE_90R:     dwStyle |= VS_ROTATE_180R;  break;
    case VS_ROTATE_180:     dwStyle |= VS_ROTATE_270;   break;
    case VS_ROTATE_180R:    dwStyle |= VS_ROTATE_270R;  break;
    case VS_ROTATE_270:     dwStyle |= VS_ROTATE_0;     break;
    case VS_ROTATE_270R:    dwStyle |= VS_ROTATE_0R;    break;
    default: DbgAssert(0);
    }
    m_aViedo[0].SetStyle(dwStyle);
    return 0;
}


void CMain::Layout(int nCheck)
{
    int nGrid = s_nGrid[nCheck];
    int nMax = nGrid * nGrid;

    // re-create
    if (nMax != m_aViedo.size())
    {
        for (; m_aViedo.size() > 1;)
        {
            m_aViedo[1].DestroyWindow();
            m_aViedo.erase(m_aViedo.begin() + 1);
        }
        
        for (int i = 1; i < nMax; ++i)
        {
            CTuiWidget wnd;
            wnd.Create(Item(IDC_WINDOW_36000), IDW_VIDEO_21025);
            m_aViedo.push_back((HTUI)wnd);
        }
    }

    CTuiRect rc;
    Item(IDC_WINDOW_36000).GetWindowRect(&rc);
    GXDips w = rc.Width() / nGrid;
    GXDips h = rc.Height() / nGrid;
    for (int i = 0; i < nGrid; ++i)
    {
        for (int j = 0; j < nGrid; ++j)
        {
            int id = i * nGrid + j;
            CTuiRect rcx;
            rcx.SetLTWH(i * w + rc.Left(), j * h + rc.Top(), w, h);
            m_aViedo[id].MoveWindow(rcx);
        }
    }
}

TMResult CMain::OnOpen(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    HTUI hWnd = (HTUI)hWndCtl;
    if (m_aViedo[0].IsChild(hWnd)) // from 0, sync all video
    {
        CStdStr str;
        m_aViedo[0].GetSrc(str);
        for (int i = 1; i < m_aViedo.size(); ++i)
        {
            m_aViedo[i].SetSrc(str);
            m_aViedo[i].Load();
        }
    }
    return 0;
}

TMResult CMain::OnPlay(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    HTUI hWnd = (HTUI)hWndCtl;
    if (m_aViedo[0].IsChild(hWnd)) // from 0, sync all video
    {
        for (int i = 1; i < m_aViedo.size(); ++i)
            m_aViedo[i].Play();
    }
    return 0;
}


//
//
//
//TMResult CMain::OnApplyFont(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//{
//    LOGFONT ft;
//    GetFont(ft);
//    GXFont gft;
//    gft.FromLOGFONT(ft);
//    CTuiRich(Item(IDC_MEMO_20039)).SetSelectedTextFont(&gft);
//    return 0;
//}
//
//TMResult CMain::OnSelColor(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//{
//    COLORREF clr = Item(IDC_CLR).GetTag();
//    CColorDialog dlg;
//    if (IDOK != dlg.DoModal(0, clr))
//        return 0;
//    Item(IDC_CLR).SetTag(dlg.GetColor());
//    return 0;
//}
//
//TMResult CMain::OnApplyColor(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//{
//    COLORREF clr = Item(IDC_CLR).GetTag();
//    CTuiRich(Item(IDC_MEMO_20039)).SetSelectedTextColor(ColorFromCOLORREF(clr));
//    return 0;
//}
//
//TMResult CMain::OnSendHtml(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//{
//    CString str;
//    Item(IDC_MEMO_20058).GetWindowText(str);
//    CTuiRich(Item(IDC_MEMO_20039)).ReplaceSelHtml(str, true);
//    return 0;
//}
//
//TMResult CMain::OnSendTxt(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//{
//    CString str;
//    Item(IDC_MEMO_20058).GetWindowText(str);
////    CTuiMemo(Item(IDC_MEMO_20039)).SetWindowText(str);
//    CTuiMemo(Item(IDC_MEMO_20039)).ReplaceSel(str, true);
//    return 0;
//}
//
//TMResult CMain::OnPrintColor(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
//{
//    TMRect rc;
//    CTuiWidget wnd(pMsg->hWnd);
//    wnd.GetClientRect(&rc);
//    TMColor clr = ColorFromCOLORREF(wnd.GetTag());
//    gxFillRect(rc, clr);
//    return 0;
//}
//
//
//void CMain::GetFont(LOGFONT& ft)
//{
//    MemZeroT(ft);
//    CString str;
//    Item(IDC_EDIT_20043).GetWindowText(str);
//    StrCopy(ft.lfFaceName, sizeof(ft.lfFaceName), str);
//    ft.lfHeight = GetDlgItemInt(IDC_EDIT_20046);
//    ft.lfWeight = CTuiButton(Item(IDC_PUSHBUTTON_20052)).GetCheck() ? FW_BOLD : FW_NORMAL;
//    ft.lfItalic = CTuiButton(Item(IDC_PUSHBUTTON_20053)).GetCheck();
//    ft.lfUnderline = CTuiButton(Item(IDC_PUSHBUTTON_20054)).GetCheck();
//}
//
//void CMain::SetFont(const LOGFONT& ft)
//{
//    Item(IDC_EDIT_20043).SetWindowText(ft.lfFaceName);
//    SetDlgItemInt(IDC_EDIT_20046, ft.lfHeight);
//    CTuiButton(Item(IDC_PUSHBUTTON_20052)).SetCheck(ft.lfWeight > FW_NORMAL);
//    CTuiButton(Item(IDC_PUSHBUTTON_20053)).SetCheck(ft.lfItalic != 0);
//    CTuiButton(Item(IDC_PUSHBUTTON_20054)).SetCheck(ft.lfUnderline != 0);
//}


