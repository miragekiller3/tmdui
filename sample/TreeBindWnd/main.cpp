#include "stdafx.h"
#include "main.h"


using namespace TM;




TMResult CMain::OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    EndDialog(wID);
    return 0;
}

// static int s_zoom = 100;
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

TMResult CMain::OnAdd1(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    int n = GetDlgItemInt(IDC_EDIT_21003);
    Add(n, wID);
    return 0;
}

void CMain::Add(int n, int nID)
{
    CTuiTree wndParent(Item(IDC_LISTBOX_20034));
    CTVItem itmRoot = wndParent.GetRootItem();
    CTVItem itm = wndParent.GetSelectedItem();
    if (!itm)
        itm = itmRoot;
    int nCount = itmRoot.GetAllChildCount();
    for (int i = 0; i < n; ++i)
    {
        CString str;
        str.Format(L"button %d", nCount + i);
        CTVItem itmc = itm.InsertChild(str);
        if (rand() % 2)
            itmc.SetState(TVIS_AUTOHEIGHT, TVIS_AUTOHEIGHT);
        itmc.Expand();
        itmc.SetHeight(300);
        if (nID == IDC_ADDBIND)
        {
            CTuiWidget wnd = itmc.CreateBindItemWindow(IDC_WINDOW_20024);
            int n = itmc.GetLevel();
            wnd.SetMargin(n * 20, 0, 0, 0);
            wnd.Item(IDC_PUSHBUTTON_20019).SetWindowText(str);
        }
    }
    
    wndParent.SetWindowText(TM::VA2T(L"%d items", nCount + n));
}

TMResult CMain::OnHitTest(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    DefRouteProc(pMsg);
    TUIHITINFO* pht = (TUIHITINFO*)pMsg->lParam;
    CString strClassName;
    int nid = 0;
    if (pht->hWnd)
    {
        CTuiWidget wnd(pht->hWnd);
        wnd.GetClassName(strClassName);
        nid = wnd.GetDlgCtrlObjectID();
    }
    CString str;
    str.Format(L"hit test: pt(%d,%d), hTarget(0x%x - %d - %s), hHit(0x%x), pTaget(0x%x), code(%d)",
        pht->pt, pht->hWnd, nid, strClassName.c_str(), pht->hHint, pht->pTaget, pht->dwCode);
    Item(IDC_EDIT_20036).SetWindowText(str);
    return 0;
}
