#include "stdafx.h"
#include "main.h"
#include "clipboard.h"

// #include "athtml.cpp"

using namespace TM;




class CDlgPopTest : public CDialogImpl<CDlgPopTest>
{
public:
    CString m_str;

    enum { IDD = IDD_DIALOG2 };

    BEGIN_MSG_MAP(CDlgPopTest)
         MESSAGE_HANDLER(WM_SIZE, OnSize)
    END_MSG_MAP()

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CWindow hWnd = GetDlgItem(IDC_EDIT1);
        hWnd.MoveWindow(0, 0, LOWORD(lParam), HIWORD(lParam));
//        hWnd.ShowWindow(SW_MAXIMIZE);
        bHandled = false;
        return 0;
    }

    void SetText(const CString& str)
    {
        if (str != m_str)
        {
            m_str = str;
            CWindow hWnd = GetDlgItem(IDC_EDIT1);
            hWnd.SetWindowText(str);
        }
    }

    void Init(LPCTSTR pName)
    {
        SetWindowText(pName);
//         CWindow hWnd = GetDlgItem(IDC_EDIT1);
//         hWnd.ShowWindow(SW_MAXIMIZE);
    }
};

//  class CAAA;
// 
// TM::CTuiPtr<CAAA> s_ptr;

static CDlgPopTest s_dlgPop;
static CDlgPopTest s_dlgHtml;
static CDlgPopTest s_dlgRuns;
static CDlgPopTest s_dlgClipBoard;


LRESULT CMain::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    s_dlgPop.Create(GetHWND());
    s_dlgPop.Init(L"win32 view");
    s_dlgHtml.Create(GetHWND());
    s_dlgHtml.Init(L"HTML view");
    s_dlgRuns.Create(GetHWND());
    s_dlgRuns.Init(L"runs view");
    s_dlgClipBoard.Create(GetHWND());
    s_dlgClipBoard.Init(L"clip board view");

    CreateItem(IDC_RICH_21004);
    SetAni(500);
    bHandled = false;

    enum { ww = 640, hh = 250 };

    s_dlgPop.MoveWindow(0,0,ww, hh);
    s_dlgHtml.MoveWindow(0,hh,ww, hh);
    s_dlgRuns.MoveWindow(0,hh*2,ww, hh);
    s_dlgClipBoard.MoveWindow(ww,0,ww, hh);
    ::MoveWindow(GetHWND(), ww, hh, ww, hh*2, true);

#define LB_INSERT(x) lb.SetItemData(lb.AddString(TM_T(#x)), x)
    TM::CTuiListBox lb(Item(IDC_LISTBOX_22000));
    LB_INSERT(LAYOUT_HSCROLL);
    LB_INSERT(LAYOUT_VSCROLL);
    LB_INSERT(ES_CENTER);
    LB_INSERT(ES_RIGHT);
//     LB_INSERT(ES_UPPERCASE);
//     LB_INSERT(ES_LOWERCASE);
    LB_INSERT(ES_NOHIDESEL);
//     LB_INSERT(ES_OEMCONVERT);
    LB_INSERT(ES_READONLY);
    LB_INSERT(ES_WANTRETURN);
    LB_INSERT(ES_WANTTAB);
    LB_INSERT(ES_NUMBER);
    LB_INSERT(ES_NOPOPUPMENU);
    LB_INSERT(ES_NOAUTOSELECT);
    LB_INSERT(ES_NOCARET);
    LB_INSERT(ES_FOCUSPROMPT);
    LB_INSERT(ES_WORD_ELLIPSIS);
    LB_INSERT(ES_NOLINKUNDERLINE);
    LB_INSERT(ES_HYPERLINK);
#undef LB_INSERT
    
    return 0;
}

LRESULT CMain::OnLanguageChagned(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = false;
    CLocal local(lParam);
    CWindow hWnd = s_dlgPop.GetDlgItem(IDC_EDIT1);
    if (local.IsRTL())
    {
        s_dlgPop.ModifyStyleEx(0, WS_EX_LAYOUTRTL);
        hWnd.ModifyStyleEx(0, WS_EX_RIGHT | WS_EX_RTLREADING);
    }
    else
    {
        s_dlgPop.ModifyStyleEx(WS_EX_LAYOUTRTL, 0);
        hWnd.ModifyStyleEx(WS_EX_RIGHT | WS_EX_RTLREADING, 0);
    }
    return 0;
}


TMResult CMain::OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    KillAni();
    EndDialog(wID);
    return 0;
}

// static s_zoom = 100;
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

TMResult CMain::OnSelFont(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//     Item(IDC_RICH_TARGET).ScrollToY(9999);
//     return 0;

    LOGFONT ft;
    GetFont(ft);
    CFontDialog dlg (&ft, CF_EFFECTS | CF_SCREENFONTS | CF_TTONLY);
    if (IDOK != dlg.DoModal(GetHWND()))
        return 0;
    SetFont(ft);
    return 0;
}

TMResult CMain::OnApplyFont(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    LOGFONT ft;
    GetFont(ft);
    GXFont gft;
    gft.FromLOGFONT(ft);
    if (wID == IDC_APPLYFONT)
    {
        CString str;
        CTuiRich rich(Item(IDC_RICH_TARGET));
        rich.GetSelHtml(str);
        CString str2;
        str2.Format(L"<font name=%s size=%dpt>", (TMCStr)gft.name, gft.height);
        str2 += str;
        str2 += L"</font>";
        rich.ReplaceSelHtml(str2, true);
    }
    else
        CTuiRich(Item(IDC_RICH_TARGET)).SetFont(&gft);
    return 0;
}

TMResult CMain::OnSelColor(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    COLORREF clr = Item(IDC_CLR).GetTag();
    CColorDialog dlg;
    if (IDOK != dlg.DoModal(0, clr))
        return 0;
    Item(IDC_CLR).SetTag(dlg.GetColor());
    return 0;
}

TMResult CMain::OnApplyColor(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    COLORREF clr = Item(IDC_CLR).GetTag();
    CTuiRich(Item(IDC_RICH_TARGET)).SetTextColor(e_bsNormal, ColorFromCOLORREF(clr));
    return 0;
}

TMResult CMain::OnSendHtml(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CString str;
    Item(IDC_MEMO_20058).GetWindowText(str);
    CTuiRich(Item(IDC_RICH_TARGET)).ReplaceSelHtml(str, true);
//    CTuiRich(Item(IDC_RICH_TARGET)).SetWindowText(str);
    ::SetWindowText(s_dlgPop.GetDlgItem(IDC_EDIT1), str);
    return 0;
}

TMResult CMain::OnSendTxt(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CString str;
    Item(IDC_MEMO_20058).GetWindowText(str);
    CTuiMemo(Item(IDC_RICH_TARGET)).ReplaceSel(str, true);
//    CTuiMemo(Item(IDC_RICH_TARGET)).ReplaceSel(str, true);
    ::SetWindowText(s_dlgPop.GetDlgItem(IDC_EDIT1), str);
    return 0;
}

TMResult CMain::OnRadio(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiWidget wnd((HTUI)hWndCtl);
    int nTag = wnd.GetTag();
    Item(IDC_RICH_TARGET).DestroyWindow();
    CreateItem(nTag);
    return 0;
}

TMResult CMain::OnEnUpdate(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiRich rich((HTUI)hWndCtl);
    TMRANGE rg = { 0, -1 };
    CString str;
    rich.GetSelHtml(str, 0, -1);
    //Item(IDC_MEMO_HTMLVIEW).SetWindowText(str);
    s_dlgHtml.SetText(str);

//     rich.getsel
    return 0;
}

TMResult CMain::OnSelChange(WORD wNotifyCode, WORD wID, void* hWndCtl, TMBool& bHandled)
{
    CTuiWidget wndTarget(Item(IDC_RICH_TARGET));

    CTuiListBox lb((HTUI)hWndCtl);
    
    TMUInt32 dwLayout = wndTarget.GetLayOut();
    dwLayout &= ~(LAYOUT_HSCROLL | LAYOUT_VSCROLL | LAYOUT_AUTOHSCROLL | LAYOUT_AUTOVSCROLL);
    if (lb.GetSel(0))
        dwLayout |= (LAYOUT_HSCROLL | LAYOUT_AUTOHSCROLL);
    if (lb.GetSel(1))
        dwLayout |= (LAYOUT_VSCROLL | LAYOUT_AUTOVSCROLL);
    wndTarget.SetLayOut(dwLayout);

    TMUInt32 dwStyle = wndTarget.GetStyle();
    dwStyle &= 0xFFFF0000;
    for (int i = 2; ; ++i)
    {
        int n = lb.GetItemData(i);
        if (n <= 0)
            break;
        if (lb.GetSel(i))
            dwStyle |= n;
    }
    wndTarget.SetStyle(dwStyle);

    return 0;
}


TMResult CMain::OnPrintColor(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    TM::CTuiRect rc;
    CTuiWidget wnd(pMsg->hWnd);
    wnd.GetClientRect(&rc);
    TMColor clr = ColorFromCOLORREF(wnd.GetTag());
    _gxApp->FillRect(rc, clr);
    return 0;
}


void CMain::GetFont(LOGFONT& ft)
{
    MemZeroT(ft);
    CString str;
    Item(IDC_EDIT_20043).GetWindowText(str);
    StrCopy(ft.lfFaceName, sizeof(ft.lfFaceName), str);
    ft.lfHeight = GetDlgItemInt(IDC_EDIT_20046);
    ft.lfWeight = CTuiButton(Item(IDC_PUSHBUTTON_20052)).GetCheck() ? FW_BOLD : FW_NORMAL;
    ft.lfItalic = CTuiButton(Item(IDC_PUSHBUTTON_20053)).GetCheck();
    ft.lfUnderline = CTuiButton(Item(IDC_PUSHBUTTON_20054)).GetCheck();
}

void CMain::SetFont(const LOGFONT& ft)
{
    Item(IDC_EDIT_20043).SetWindowText(ft.lfFaceName);
    SetDlgItemInt(IDC_EDIT_20046, abs(ft.lfHeight));
    CTuiButton(Item(IDC_PUSHBUTTON_20052)).SetCheck(ft.lfWeight > FW_NORMAL);
    CTuiButton(Item(IDC_PUSHBUTTON_20053)).SetCheck(ft.lfItalic != 0);
    CTuiButton(Item(IDC_PUSHBUTTON_20054)).SetCheck(ft.lfUnderline != 0);
}

void CMain::CreateItem(int oid)
{
    CTuiWidget wndNew;
    wndNew.Create(Item(IDC_WINDOW_20066), oid);
    wndNew.BringWindowToTop();
}


void CMain::OnAni(...)
{
    s_dlgClipBoard.SetText(GetClipText(GetHWND()));

    CTuiRadioButton but(Item(IDC_RADIOBUTTON_21006));
    but = but.GetCheckedButton();
    int nTag = but.GetTag();

    CString str;
    CTuiRich rich(Item(nTag));
    _GetRichRunInfo(rich, str);
    s_dlgRuns.SetText(str);
}

