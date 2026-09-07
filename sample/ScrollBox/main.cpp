#include "stdafx.h"
#include "main.h"


using namespace TM;



//static CMain::Register<0> s_callback_CMain;




TMResult CMain::OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    EndDialog(wID);
    return 0;
}
// static s_zoom = 100;
// 
// TMResult CMain::OnZoomIn(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
//     s_zoom -= 10;
//     _tuiApp.SetZoom(s_zoom);
//     return 0;
// }
// 
// TMResult CMain::OnZoomOut(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
//     s_zoom += 10;
//     _tuiApp.SetZoom(s_zoom);
//     return 0;
// }
// 
// TMResult CMain::OnAdd100(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
//     Add(100);
//     return 0;
// }

TMResult CMain::OnAdd1000(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    int n = GetDlgItemInt(IDC_EDIT_21020);
    Add(n);
    return 0;
}

TMResult CMain::OnAdd1S(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    Item(IDC_WINDOW_20029).SetLayOut(ANCHOR_FRAME | ANCHOR_BOTTOM | LAYOUT_AUTOHEIGHT);
    Add(1);
    return 0;
}


void CMain::Add(int n)
{
    CTuiWidget wndParent(Item(IDC_WINDOW_20029));
    for (int i = 0; i < n; ++i)
    {
        CTuiWidget wnd;
        wnd.Create(wndParent, IDC_WINDOW_20024);
        CString str;
        str.Format(L"button %d", m_nChilds + i);
        wnd.Item(IDC_PUSHBUTTON_20019).SetWindowText(str);
        if (n == 1)
            wnd.BringWindowToTop();
    }
    m_nChilds += n;
//     int nHeight = m_nChilds * 50;
//     TM::CRect rc;
//     wndParent.GetWindowRect(&rc);
//     rc.SetHeight(nHeight);
//     wndParent.SetWindowPos(0, rc, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
//     Item(IDC_SCROLLBOX_20028).ChangeFrame();
}
