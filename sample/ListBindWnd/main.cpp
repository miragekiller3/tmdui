#include "stdafx.h"
#include "main.h"


using namespace TM;



// static CMain::Register<0> s_callback_CMain;

struct CData
{
    TMBool bCheck;
    CString strEdit;
//     CString strButton;
};

static TMBool CALLBACK BindBackup(HTUI hWnd, HTUI hBind, TMBool bBackUp, TMLParam lIDorHITEM, TMLParam lData)
{
    DbgAssert(hBind);
    CData* pData = (CData*)lData;
    if (bBackUp)
    {
        CTuiWidget(hBind).Item(IDC_PUSHBUTTON_20027).GetWindowText(pData->strEdit);
//         CTuiWidget(hBind).Item(IDC_PUSHBUTTON_20019).GetWindowText(pData->strButton);
        pData->bCheck = CTuiButton(CTuiWidget(hBind).Item(IDC_PUSHBUTTON_20019)).GetCheck();
    }
    else
    {
        CTuiWidget(hBind).Item(IDC_PUSHBUTTON_20027).SetWindowText(pData->strEdit);
        CString strBut;
        CTuiListBox(hWnd).GetText(lIDorHITEM, strBut);
        //            wnd.Item(IDC_PUSHBUTTON_20019).SetWindowText(str);

        CTuiWidget(hBind).Item(IDC_PUSHBUTTON_20019).SetWindowText(strBut);
        CTuiButton(CTuiWidget(hBind).Item(IDC_PUSHBUTTON_20019)).SetCheck(pData->bCheck);
    }
    return true;
}

TMResult CMain::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    CTuiListBox lb(Item(IDC_LISTBOX_20034));
    lb.SetPfnAutoDeleteData(CTuiProcDelete<CData>::done);
    lb.SetPfnBindBackupRestore(BindBackup);
    return 0;
}

TMResult CMain::OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    EndDialog(wID);
//      SubclassDialog();
    return 0;
}

// TMResult CMain::OnZoomIn(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
//     int nx = _tuiApp.GetZoom();
//     nx -= 10;
//     _tuiApp.SetZoom(nx);
//     return 0;
// }
// 
// TMResult CMain::OnZoomOut(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
//     int nx = _tuiApp.GetZoom();
//     nx += 10;
//     _tuiApp.SetZoom(nx);
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
    int n = GetDlgItemInt(IDC_EDIT_21003);
    Add(n, wID);
    return 0;
}

TMResult CMain::OnAddDir(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiListBox wndParent(Item(IDC_LISTBOX_20034));
    wndParent.Dir(-1, L"c:\\*.*");
    wndParent.SetItemAccText(0, L"acc text0");
    return 0;
}

void CMain::Add(int n, int id)
{
    CTuiListBox wndParent(Item(IDC_LISTBOX_20034));
    int nCount = wndParent.GetCount();
    for (int i = 0; i < n; ++i)
    {
        CString str;
        str.Format(L"button %d", nCount + i);
        int j = wndParent.InsertString(-1, str);
        if (id != IDC_ADDNOBIND)
        {
            CData* pData = new CData();
//             pData->strButton = str;
            wndParent.SetItemDataPtr(j, pData);
            CTuiWidget wnd = wndParent.CreateBindItemWindow(j, IDC_WINDOW_20024);
//            wnd.Item(IDC_PUSHBUTTON_20019).SetWindowText(str);
        }
    }
    int nx = wndParent.GetCount();
    wndParent.SetWindowText(TM::VA2T(L"%d items", nx));
    wndParent.SetItemAccText(0, L"acc text0");
//     wndParent.SetCurSel(nCount + n - 1);
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
    str.Format(L"hit test: pt(%d,%d), hTarget(0x%x - %d - %s), hHit(0x%x)",
        pht->pt, pht->hWnd, nid, strClassName.c_str(), pht->hHint);
    Item(IDC_EDIT_20036).SetWindowText(str);
    return 0;
}
