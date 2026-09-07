/**
 *    @file
 *    @brief
 *    CDlgImportWindow
 *
 *
 *    @author 
 *    @date    2016-9-6
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2016 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgImportWindow.h"
#include "MainWnd.h"
#include "ProjMgr.h"
#include "tmdrag.h"
#include "tmxmlstring.h"
#include "tmdom.h"

using namespace TM;

#define WINDOW_IMPORT_STYLE_MASK    0x58CF0000
#define WINDOW_IMPORT_EXSTYLE_MASK  0x080806A8

CDlgImportWindow::CDlgImportWindow()
    : m_hTarget (0)
{
}

CDlgImportWindow::~CDlgImportWindow()
{
//     ::DestroyWindow(m_hSel);
    DbgAssert(!::IsWindow(m_hSel));
}

TMResult CDlgImportWindow::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    CTuiWidget wnd = m_tuiWindow.GetAPP()->CreateDlg(_Module.GetResourceInstance(), IDC_MANAGE_TARGET_SELECT, GetHWND());
    m_hSel = wnd.GetHWND();
    return 0;
}

TMResult CDlgImportWindow::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    if (m_hTarget)
        ImportFromWindow(m_hTarget);

    EndDialog(wID);
//    DestroyWindow();
    return 0;
}

TMResult CDlgImportWindow::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    EndDialog(wID);
//    DestroyWindow();
    return 0;
}

class CImportDragData : public CDragData
{
public:
    CDlgImportWindow* pwnd;

    virtual TMBool OnDraging(POINT pt, DWORD dwKeyState)
    {
        HWND hWnd = WindowFromPoint(pt);
        pwnd->SetTarget(hWnd);
        return true;
    }
};

TMResult CDlgImportWindow::OnDragMove(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    TUIHITINFO hinfo;
    if (m_tuiWindow.GetAPP()->GetMouseLDownState(hinfo) && hinfo.hWnd == pMsg->hWnd)
    {
        CImportDragData dd;
        dd.pwnd = this;
        CTuiWidget wnd(pMsg->hWnd);
        TM::CTuiRect rc;
        wnd.GetWindowRect(&rc);
        wnd.SetDragImage(dd, CPoint(0, rc.Height()));
        ShowCursor(false);
        wnd.ShowWindow(0);
        wnd.DoDrag(dd, eDragEffNone);
        wnd.ShowWindow(true);
        ShowCursor(true);
    }
    return 0;
}

static void _CDlgImportWindow_ChecRC(CRect& rc)
{
    if (rc.Width() > 4000)
        rc.SetWidth(4000);
    if (rc.Height() > 4000)
        rc.SetHeight(4000);
}

void CDlgImportWindow::SetTarget(HWND hWnd)
{
    CString str;
    m_hTarget = hWnd;
    ::ShowWindow(m_hSel, hWnd ? SW_SHOWNOACTIVATE : SW_HIDE);
    if (hWnd)
    {
        TCHAR c[1024];
        ::GetWindowText(hWnd, c, 1024);
        CRect rc;
        ::GetWindowRect(hWnd, &rc);
        str.Format(L"HWND:%x\r\nrect:%d,%d,%d,%d\r\ntext:%s", hWnd, rc.left, rc.top, rc.right, rc.bottom, c);
        _CDlgImportWindow_ChecRC(rc);
        ::MoveWindow(m_hSel, rc.left, rc.top, rc.Width(), rc.Height(), false);
//        DbgTrace("move target sel window:%d,%d,%d,%d", rc);
    }
    Item(IDC_MEMO_30226).SetWindowText(str);
}

TMResult CDlgImportWindow::OnParent (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    HWND hWnd = ::GetParent(m_hTarget);
    SetTarget(hWnd);
    return 0;
}

TMResult CDlgImportWindow::OnChild (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    HWND hWnd = ::GetWindow(m_hTarget, GW_CHILD);
    if (hWnd)
        SetTarget(hWnd);
    return 0;
}


struct CImportParam
{
    IDomDocument* m_pDoc;
    IDomNode* m_pParent;
    CSize m_ptOrg;

    IDomElement* CreateElement(IDomNode* pParent, const CString& strClass, const CString& strName, WINDOWINFO& wi, int nid)
    {
        DbgAssert(strClass);
        
        IDomElement* p = m_pDoc->createElement(TM_CL("L0"));
        p->setAttribute(TM_CL("class"), strClass);
        p->setAttribute(TM_CL("Text"), strName);
        p->setAttributeLong(TM_CL("Style"), wi.dwStyle);
        p->setAttributeLong(TM_CL("AdvStyle"), wi.dwExStyle);
//        typedef CRect::value_type value_type;
        p->setAttribute(TM_CL("Rect"), XmlStrFromInts((CRect&)wi.rcWindow));
        p->setAttributeLong(TM_CL("ID"), nid);
        pParent->appendChild(p);
        return p;
    }

    IDomElement* AddWindow(HWND hWnd, TMBool bMgr)
    {
        CWindow wnd(hWnd);
        WINDOWINFO wi = {sizeof(WINDOWINFO)};
        GetWindowInfo(hWnd, &wi);

        CRect& rc = (CRect&)(wi.rcWindow);
        if (bMgr)
        {
            m_ptOrg.cx = rc.left;
            m_ptOrg.cy = rc.top;
        }
        rc -= m_ptOrg;
        _CDlgImportWindow_ChecRC(rc);

        CString str;
        int nLen = wnd.GetWindowTextLength();
        if (nLen)
            wnd.GetWindowText(str.GetBuffer(nLen), nLen + 1);

        int nid = GetDlgCtrlID(wnd);

        CString strClass;
        if (bMgr)
        {
            strClass = TUI_CLASS_WINDOW;
            wi.dwStyle &= WINDOW_IMPORT_STYLE_MASK;
            wi.dwExStyle &= WINDOW_IMPORT_EXSTYLE_MASK;
        }
        else
        {
            wi.dwStyle &= ~(WS_CHILD | WS_GROUP);
            int n = GetClassName(hWnd, strClass.GetBuffer(1024), 1024);
            strClass.SetLength(n);
            
            if (strClass == L"Static")
                strClass = CTuiStatic::GetWndClassName();
            else if (strClass == L"Edit")
            {
                if (wi.dwStyle & ES_MULTILINE)
                {
                    strClass = CTuiMemo::GetWndClassName();
                    wi.dwStyle &= ~ES_MULTILINE;
                }
                else
                    strClass = CTuiEdit::GetWndClassName();
            }
            else if (strClass == L"ListBox")
                strClass = CTuiListBox::GetWndClassName();
            else if (strClass == L"Button")
            {
                switch (wi.dwStyle & 0x0f)
                {
                case BS_CHECKBOX:
                case BS_AUTOCHECKBOX:
                    strClass = CTuiCheckBox::GetWndClassName();
                    break;

                case BS_RADIOBUTTON:
                case BS_AUTORADIOBUTTON:
                    strClass = CTuiRadioButton::GetWndClassName();
                    break;

                case BS_GROUPBOX:
                    strClass = CTuiGroupBox::GetWndClassName();
                    break;

                default:
                    strClass = CTuiPushButton::GetWndClassName();
                    break;
                }
                wi.dwStyle &= ~0x0f;
            }
            else if (strClass == L"ComboBox" || strClass == WC_COMBOBOXEX)
            {
                IDomElement* p = CreateElement(m_pParent, CTuiEdit::GetWndClassName(), str, wi, nid);
                IDomElement* pList = CreateElement(m_pParent, CTuiListBox::GetWndClassName(), L"", wi, 0);
                wi.rcWindow.left = wi.rcWindow.right - 20;
                IDomElement* pBut = CreateElement(p, CTuiComboButton::GetWndClassName(), L"...", wi, 0);
                return p;
            }
            else if (strClass == WC_TABCONTROL)
                strClass = CTuiTabButton::GetWndClassName();
            else if (strClass == UPDOWN_CLASS)
                strClass = CTuiUpDown::GetWndClassName();
            else if (strClass == WC_LISTVIEW || strClass == WC_TREEVIEW)
                strClass = CTuiTree::GetWndClassName();
            else if (strClass == PROGRESS_CLASS)
                strClass = CTuiProgress::GetWndClassName();
            else if (strClass == ANIMATE_CLASS)
                strClass = CTuiAnimate::GetWndClassName();
            else if (strClass == WC_HEADER)
                strClass = CTuiHeader::GetWndClassName();
            else if (strClass == TRACKBAR_CLASS)
                strClass = CTuiTrackBar::GetWndClassName();
            else 
                strClass = TUI_CLASS_WIDGET;
        }

        return CreateElement(m_pParent, strClass, str, wi, nid);
    }
};

void EnumImport(HWND hParent, CImportParam* p)
{
    for (HWND hWnd = GetWindow(hParent, GW_CHILD); hWnd; hWnd = GetWindow(hWnd, GW_HWNDNEXT))
    {
        IDomNode* pParent = p->m_pParent;
        p->m_pParent = p->AddWindow(hWnd, false);
        EnumImport(hWnd, p);
        p->m_pParent = pParent;
    }
}

void CDlgImportWindow::ImportFromWindow(HWND hWnd)
{
    CImportParam ip;
    CDomDocument doc;
    ip.m_pDoc = doc;
    ip.m_pParent = doc;
    ip.m_pParent = ip.AddWindow(hWnd, true);
    EnumImport(hWnd, &ip);

//    g_curPanel->GetUndo().PasteItem(ip.m_pParent, (IObjEdit*)TM::e_otWnd);
    m_pUndo->PasteItem(ip.m_pParent, (IObjEdit*)TM::e_otWnd);
}
