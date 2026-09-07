#include "stdafx.h"
#include "demodlg.h"
#include "../Design.h"



using namespace TM;



int CTestXMLDlg::IDD = 0;


TMResult CTestXMLDlg::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    IDD = 0;
    bHandled = false;
    TesterZoom(0);
    return 0;
}

TMResult CDemoDlg::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    HWND h1 = GetDlgItem(IDC_RADIO1);
    HWND h2 = GetDlgItem(IDC_BUTTON3);
    ::SetParent(h1, h2);

    m_dlgC.Create(*AppGetTester(), m_hWnd, 0, CTestXMLDlg::IDD, HEDITOR);
    if (!(m_dlgC.GetStyle() & WS_CHILD))
        SetWindowPos(0, -10000,-10000,0,0, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

    TesterZoom(0);

    return false;
}

TMResult CDemoDlg::OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    m_dlgC.DestroyWindow();
    EndDialog(wID);
    return 0;
}


class CTestMsgFilter : public CTuiMsgFilter
{
    static CTVItem AddTreeItem(CTVItem itm, const CString& name)
    {
        itm = itm.AppendChild(name);
        itm.SetHint(name + L"_hint");
        itm.Expand();
        return itm;
    }

    virtual TMBool PreTranslateMessage(TUIMSG* pMsg, TMResult& lResult) tm_override
    {
        if (pMsg->message == WM_NCCREATE)
        {
            CString str;
            CTuiWidget(pMsg->hWnd).GetClassName(str);
            if (str == TUI_CLASS_TREE)
            {
                CTuiTree wt(pMsg->hWnd);
                CTVItem ti(wt.GetRootItem());
                CTVItem tc = AddTreeItem(ti, L"item1");
                AddTreeItem(tc, L"item1_c1");
                AddTreeItem(tc, L"item1_c2");
                tc = AddTreeItem(ti, L"item2");
                AddTreeItem(tc, L"item2_c1");
                AddTreeItem(tc, L"item2_c2");
            }
            else if (str == TUI_CLASS_LISTBOX)
            {
                CTuiListBox lb(pMsg->hWnd);
                int n = lb.AddString(L"lb_item1");
                lb.SetItemHint(n, L"lb_item1_hint");
                n = lb.AddString(L"lb_item2");
                lb.SetItemHint(n, L"lb_item2_hint");
                lb.AddString(L"lb_item3");
            }
            else if (str == TUI_CLASS_HEADER)
            {
                CTuiHeader head(pMsg->hWnd);
                HDITEM itm = { HDI_TEXT | HDI_WIDTH | HDI_FORMAT, 80, (TMStrW)L"left" };
                head.InsertItem(0, &itm);
                itm.pszText = (TMStrW)L"center"; itm.fmt = HDF_CENTER | HDF_AUTOSORTDOWN; head.InsertItem(1, &itm);
                itm.pszText = (TMStrW)L"right"; itm.fmt = HDF_RIGHT | HDF_AUTOSORTUP | HDF_AUTOSORTDOWN; head.InsertItem(2, &itm);
            }
        }
        return false;
    }
};

void TesterLoad(IProj* proj, TMHWnd hWnd, LPCTSTR pFile, int nID)
{
    static CDemoDlg s_dlgW;
    static CTestMsgFilter s_mfTest;

    if (s_dlgW.m_dlgC || s_dlgW)
    {
        if (s_dlgW.m_dlgC)
            s_dlgW.m_dlgC.DestroyWindow();
        if (s_dlgW)
            s_dlgW.EndDialog(0);
        return;
    }

    if (!nID)
        return;

    proj->SaveFile(pFile, eSaveModeTest);
    AppGetTester()->AddMessageFilter(&s_mfTest);
    AppGetTester()->LoadXml(HEDITOR, pFile);
    proj->TesterRTL(true);

    CTestXMLDlg::IDD = nID;
    s_dlgW.m_dlgC.m_hParent = hWnd;
    s_dlgW.m_dlgC.DoModal(*AppGetTester(), 0, 0, nID, HEDITOR);

    if (CTestXMLDlg::IDD)
        s_dlgW.DoModal(0);

    AppGetTester()->UnLoadXml(HEDITOR);
    AppGetTester()->RemoveMessageFilter(&s_mfTest);
}

TMBool CALLBACK EnumZoomProc(HTUI hwnd, TMLParam lParam)
{
    CTuiWindow mgr(hwnd);
    int dpi = mgr.GetDPI();
    mgr.SetDPI(dpi + lParam);
    return true;
}

void TesterZoom(int n)
{
    AppGetTester()->EnumDialogs(EnumZoomProc, n);
}

// void ShowSysFrame()
// {
//     AppGetTester()->SetUseSystemFrame(!AppGetTester()->IsUseSystemFrame());
// }


