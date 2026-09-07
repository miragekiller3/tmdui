/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2007.01.26
 *
 *  Copyright (C) 2017 miragekiller
 */

#include "uicombox.h"


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// ICbList
struct ICbList
{
    virtual TMIntPtr GetSel(HTUI hWnd) = 0;
    virtual bool SetSel(HTUI hWnd, TMIntPtr now, int nOffset) = 0;
    virtual bool GetText(HTUI hWnd, TMIntPtr now, CString& str) = 0;
    virtual void GetHInt(HTUI hWnd, TMIntPtr now, CString& str) = 0;
    virtual void Init(HTUI hWnd, const CString& strItems) = 0;
};

class CCbListBox : public ICbList
{
    virtual TMIntPtr GetSel(HTUI hWnd) tm_override
    {
        DISABLE_SPY();
        return CTuiListBox(hWnd).GetCurSel() + 1;
    }
    
    virtual bool SetSel(HTUI hWnd, TMIntPtr now, int nOffset) tm_override
    {
        int n = (now + nOffset - 1);
        if (n < 0)
            return false;
        DISABLE_SPY();
        CTuiListBox(hWnd).SetCurSel(n);
        return true;
    }
    
    virtual bool GetText(HTUI hWnd, TMIntPtr now, CString& str) tm_override
    {
        DISABLE_SPY();
        return CTuiListBox(hWnd).GetText(now - 1, str) >= 0;
    }

    virtual void GetHInt(HTUI hWnd, TMIntPtr now, CString& str) tm_override
    {
        DISABLE_SPY();
        str = CTuiListBox(hWnd).GetItemHint(now - 1);
    }

    virtual void Init(HTUI hWnd, const CString& strItems) tm_override
    {
        DISABLE_SPY();
        CTuiListBox lb(hWnd);
        lb.ResetContent();
        CString str;
        for (int n = 0; n = strItems.SplitLine(n, str); )
        {
            str.Trim();
            lb.AddString(str);
        }
    }
};

class CCbTree : public ICbList
{
    virtual TMIntPtr GetSel(HTUI hWnd) tm_override
    {
        DISABLE_SPY();
        return (TMIntPtr)CTuiTree(hWnd).GetSelectedItem().m_hItem;
    }
    
    virtual bool SetSel(HTUI hWnd, TMIntPtr now, int nOffset) tm_override
    {
        DISABLE_SPY();
        CTVItem itm(now ? (HTVITEM)now : (HTVITEM)CTuiTree(hWnd).GetRootItem());
        if (nOffset > 0)
            itm = itm.GetNextVisible();
        else
        {
            DbgAssert(nOffset < 0);
            if (!itm)
                return false;
            itm = itm.GetPrevVisible();
        }
        if (!itm)
            return false;
        itm.SelectItem();
        return true;
    }
    
    virtual bool GetText(HTUI hWnd, TMIntPtr now, CString& str) tm_override
    {
        DISABLE_SPY();
        CTVItem itm((HTVITEM)now);
        if (itm.IsDisable())
            return false;
        itm.GetText(str);
        return true;
    }
    
    virtual void GetHInt(HTUI hWnd, TMIntPtr now, CString& str) tm_override
    {
        DISABLE_SPY();
        CTVItem itm((HTVITEM)now);
        itm.GetHint(str);
    }

    virtual void Init(HTUI hWnd, const CString& strItems) tm_override
    {
        DISABLE_SPY();
        CTuiTree tree(hWnd);
        CTVItem itm(tree.GetRootItem());
        itm.RemoveChild();
        CString str;
        for (int n = 0; n = strItems.SplitLine(n, str); )
        {
            str.Trim();
            itm.AppendChild(str);
            itm.SetState(TVIS_EXPANDED, TVIS_EXPANDED);
        }
    }
};

// static ICbList     s_cbNull;
static CCbListBox  s_cbList;
static CCbTree     s_cbTree;


////////////////////////////////////////////////////////////////////////////////
// CTuiComboButtonImpl
TM_REGISTER_CLASS1 (CTuiComboButton);

CTuiComboButtonImpl::CTuiComboButtonImpl()
    : m_nDlgID      (0)
    , m_nWndID      (0)
    , m_nReferID    (0)
    , m_iList       (&s_cbList)
//     , m_nDefaut (-1)
{
    m_dwStyle &= ~WS_TABSTOP;
    m_dwStyle |= (CBS_CLICKTOPOP | CBS_ARROWTOPOP | BS_NOCLICKFOCUS);
    m_nNumChecks = 2;
//     m_pFore.GetPre(IDP_PRE_PIC_COMB);
}

CTuiComboButtonImpl::~CTuiComboButtonImpl ()
{
//     if (Is_Style_CBS_NOEDIT())
//         m_wndEdit.m_hWnd = 0;
}

TMBool CTuiComboButtonImpl::ProcessEditMessage(
    HTUI hEdit, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult)
{
    if (this->IsWindowEnabled())
    {
        if (uMsg == WM_LBUTTONDOWN)
            return this->DoMousePop();
        else if (uMsg == WM_MOUSEWHEEL)
        {
            if (IsPop() == ePopUp)
                return false;
            
            // fix: scroll parent will let mouse entry combo.
            if (!hEdit->IsFocus() && !this->IsFocus())
                return false;
            
            DISABLE_SPY();
            TMWParam wp = ((short)HIWORD(wParam)) < 0 ? 1 : -1;
            ICbList* ilist = this->GetICB();
            TMIntPtr now = ilist->GetSel(m_wndList);
            if (ilist->SetSel(m_wndList, now, wp))
            {
                if (now != ilist->GetSel(m_wndList))
                    this->SubmitText(true);
            }
            return true;
        }
        else if (uMsg == WM_CHAR)
        {
            if ((wParam == VK_SPACE) && this->Is_Style_CBS_SPACETOPOP() && (IsPop() == ePopColl))
            {
                this->CheckSetCheck(1);
                return lResult = true;
            }
            return false;
        }
        else if (uMsg == WM_MOVE)
        {
            this->CheckSetCheck(0);
        }
    }
    
    return this->ProcessKeyDown(hEdit, uMsg, wParam, lParam, lResult);    
}

TMBool CTuiComboButtonImpl::ProcessListMessage(
    HTUI hList, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult)
{
    if (!this->IsWindowEnabled())
        return false;
    
    if (uMsg == WM_LBUTTONDOWN)
        hList->SetCapture();
    else if (uMsg == WM_LBUTTONUP)
    {
        TMResult lr = 0;
        hList->GetView()->ReleaseCapture();
        this->ProcessKeyDown(hList, WM_KEYDOWN, VK_RETURN, 0, lr);
        return false;
    }
    
    return this->ProcessKeyDown(hList, uMsg, wParam, lParam, lResult);
}

TMBool CTuiComboButtonImpl::ProcessPopMessage(HTUI hPop, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult)
{
    if (uMsg != CM_ENDMENUPOP) // don't close window
        return false;

    CheckSetCheck(false);
    if (wParam == IDOK)
        Notify(CBN_SELENDOK);
    else
        Notify(CBN_SELENDCANCEL);
    Notify(CBN_CLOSEUP);

    if (hPop->Is_WS_PRI_IS_WINDOW())
        CWindowAT(hPop->GetHWND()).ShowWindow(false);
    else
        hPop->SetWindowPos(0,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_HIDEWINDOW);
    return true;
}

HTUI CTuiComboButtonImpl::GetRefer()
{
    if (!m_nReferID)
        return m_wndEdit;
    else
        return GetOwner()->GetDlgItem(m_nReferID);
}

void CTuiComboButtonImpl::OnCreate()
{
    DISABLE_SPY();
    if (Is_Style_CBS_NOEDIT())
        m_wndEdit = this;
    else
        Subclass_m_wndEdit(m_wndEdit = GetParent());//.SubclassWindow(GetParent());

    if (!Edit_IsObjectInDocMode(this))
    {
        HTUI hDlg = 0;
        HTUI hList = 0;
        if (m_nDlgID)
        {
            hDlg = GetView()->CreateDlg(GetModule()->m_hRes, m_nDlgID, GetHWND());
            if (hDlg && m_nWndID)
                hList = hDlg->SearchNearestItemInDlg(m_nWndID);
        }
        else if (m_nWndID)
            hList = SearchNearestItemInDlg(m_nWndID);

        if (hList)
        {
            Subclass_m_wndList(m_wndList = hList);
            CString strClassName(hList->GetClassName());
            if (strClassName == CTuiListBox::GetWndClassName())
                m_iList = &s_cbList;
            else if (strClassName == CTuiTree::GetWndClassName())
                m_iList = &s_cbTree;
        }

        if (hDlg)
            Subclass_m_wndPop(m_wndPop = hDlg); //.SubclassWindow(hDlg);
        else if (hList)
        {
//            m_wndPop.SubclassWindow(hList);
            Subclass_m_wndPop(m_wndPop = hList);
            hList->Set_WS_PRI_BINDCBLIST();
        }


#ifndef TUI_TEST_MODE
        DbgAssert(m_wndPop);
#endif

        InitListOnPop();
    }
}

TMBool CTuiComboButtonImpl::SubmitText(bool bNotifyOk)
{
    ICbList* ilist = GetICB();
    TMIntPtr now = ilist->GetSel(m_wndList);
    if (!now)
        return false;

    CString str;
    if (!ilist->GetText(m_wndList, now, str))
        return false;

    DISABLE_SPY();
    CTuiWidget(m_wndEdit).SetWindowText(str);
    if (!Is_Style_CBS_NOSYNCHINT())
    {
        str.clear();
        ilist->GetHInt(m_wndList, now, str);
        DISABLE_SPY();
        CTuiWidget(m_wndEdit).SetHint(str);
    }

    DISABLE_SPY();
    Notify(CBN_SELCHANGE);
    if (bNotifyOk)
        Notify(CBN_SELENDOK);
    return true;
}

TMBool CTuiComboButtonImpl::DoMousePop()
{
    if (!(m_dwStyle & CBS_CLICKTOPOP))
        return false;
    DISABLE_SPY();
    CTuiButton btn(this);
    btn.SetCheck(!btn.GetCheck());
    return true;
}

void CTuiComboButtonImpl::InitListOnPop()
{
    if (!m_wndList || m_strItems.IsEmpty())
        return;

    if (!Edit_IsObjectInDocMode(this))
    {
        DISABLE_SPY();
        GetICB()->Init(m_wndList, m_strItems);
    }
}

struct CTuiComboButtonImpl_SetPopAnchor
{
    Ref m_wnd;
    TMUInt32 m_dwLayout;

    CTuiComboButtonImpl_SetPopAnchor(HTUI hWnd, GXDips topPop, GXDips topRef)
        : m_wnd(hWnd)
        , m_dwLayout(hWnd->m_dwLayout)
    {
        if (hWnd->IsAnchorVNone() && hWnd->IsAutoHeight())
        {
            hWnd->m_dwLayout &= ~LAYOUT_VMASK;
            if (topPop > topRef)
                hWnd->m_dwLayout |= LAYOUT_TOP;
            else
                hWnd->m_dwLayout |= LAYOUT_BOTTOM;
        }
    }

    ~CTuiComboButtonImpl_SetPopAnchor()
    {
        m_wnd->m_dwLayout = m_dwLayout;
    }
};

void CTuiComboButtonImpl::PopAsDialog(int x, int y, int cx, int cy, TMUInt nFlags)
{
    DbgAssert(m_wndPop->Is_WS_PRI_IS_WINDOW());
    HWND hAfter = m_wndPop->GetExStyle() & WS_EX_TOPMOST ? HWND_TOPMOST : HWND_TOP;
    CWindowAT wnd(m_wndPop->GetHWND());
    wnd.SetWindowPos((TMHWnd)hAfter, x, y, cx, cy, nFlags);
    (static_cast<CTuiWnd*>(m_wndPop.p))->SetDefaultFocus(nFlags);
}

void CTuiComboButtonImpl::Pop(TMBool bMousePop)
{
    if (m_dwStyle & CBS_SIMPLE) // CBS_SIMPLE always show pop
        return;

    if (IsPop() != ePopColl)
        return;

    Notify(CBN_DROPDOWN);
    DISABLE_SPY();
    CTuiWidget(m_wndEdit).SetFocus(eFocusReasonPopup);

    // refer rect
    TM::CTuiRect rc;
    HTUI hRefer = GetRefer();
    if (hRefer)
    {
        DbgAssert(hRefer);
        hRefer->GetWindowRect(&rc);
        hRefer->WindowRectToMarginRect(rc);
        
        // list rect
        CTuiRect rcList;
        m_wndPop->GetWindowRect(&rcList);
        GXDips h = rcList.Height();
        rcList.SetLeft(rc.Left());
        rcList.SetRight(rc.Right());
        rcList.SetTop(rc.Bottom());
        if (!IsHighContrast())
        {
            m_wndPop->MarginRectToWindowRect(rcList);
            rc.OffsetTopBottom(m_wndPop->m_bdMargin.bottom, m_wndPop->m_bdMargin.top);
        }
        rcList.SetHeight(h);

        if (m_wndPop == m_wndList) // pop list
        {
            const CTuiRect& rcMgr = GetOwner()->GetWindowRect();
            rcList.CalcUpDownBestPos(rc, rcMgr);
            DISABLE_SPY();
            CTuiComboButtonImpl_SetPopAnchor spa(m_wndPop, rcList.Top(), rc.Top());
            m_wndPop->SetWindowPos(HTUI_TOP, &rcList, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
        else // pop dialog
        {
            CRect rcRefer, rcPop;
            hRefer->ClientToScreen(rc, rcRefer);
            hRefer->ClientToScreen(rcList, rcPop);
            rcRefer.NormalizeRect();
            rcPop.NormalizeRect();

            CRect rcWork;
            CMonitor::FromWindow(GetHWND()).GetRect(0, &rcWork);
            rcPop.CalcUpDownBestPos(rcRefer, rcWork);

            DISABLE_SPY();
            CTuiWindow(m_wndPop).SetDPI(GetDPI());
            // need call twice: first: let system change dpi; 2:move window
            CTuiComboButtonImpl_SetPopAnchor spa(m_wndPop, rcPop.Top(), rcRefer.Top());
            PopAsDialog(rcPop.Left(), rcPop.Top(), rcPop.Width(), rcPop.Height(), SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
    }
    else // if (hRefer) ~
    {
        DISABLE_SPY();
        if (m_wndPop == m_wndList)
            m_wndPop->SetWindowPos(HTUI_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
        else
            PopAsDialog(0, 0, 0, 0, SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
    }

    DISABLE_SPY();
    CTuiWidget(m_wndPop).DoMenuPop(m_wndEdit, 0);
}

CTuiComboButtonImpl::EPop CTuiComboButtonImpl::IsPop()
{
    DISABLE_SPY();
    if(!m_wndPop || m_wndPop->IsLifeInvalid())
        return ePopNull;
    if (m_wndPop == m_wndList)
        return m_wndPop->Is_WS_VISIBLE() ? ePopUp : ePopColl;
    return CWindowAT(m_wndPop->GetHWND()).IsWindowVisible() ? ePopUp : ePopColl;
}

void CTuiComboButtonImpl::Filter()
{
    if (m_iList != &s_cbTree)
        return;

    CString str(m_wndEdit->GetWindowText());

    DISABLE_SPY();
    CTuiTree tree(m_wndList);
    CTVItem itm(tree.GetRootItem());
//     CTVItem itmFitst;
    for (itm = itm.GetChild(); itm; itm = itm.GetNext())
    {
        CString strX;
        itm.GetText(strX);
        bool bShow = strX.FindI(str) >= 0;
        itm.ShowItem(bShow, false);
//         if (!bShow && bShow)
//             itm.SelectItem(true);
    }
    DISABLE_SPY();
    itm = tree.GetFirstVisible();
    if (itm)
        itm.SelectItem(true);
}

// void CTuiComboButtonImpl::OnEndMenuPop(TMWParam wParam)
// {
//     CheckSetCheck(false);
//     if (wParam == IDOK)
//         Notify(CBN_SELENDOK);
//     else
//         Notify(CBN_SELENDCANCEL);
//     Notify(CBN_CLOSEUP);
// }

TMBool CTuiComboButtonImpl::ProcessKeyDown(HTUI hWgt, TMUInt uMsg, TMWParam wp, TMLParam lp, TMResult& ret)
{
    switch (uMsg)
    {
    case CCB_GETPOPLIST:
        ret = (TMResult)(HTUI)m_wndList;
        return true;
        
    case CCB_GETBUTTON:
        ret = (TMResult)(HTUI)this;
        return true;

    case CCB_GETEDIT:
        ret = (TMResult)(HTUI)m_wndEdit;
        return true;

    case CCB_GETPOPDLG:
        ret = (TMResult)(HTUI)m_wndPop;
        return true;

    case CCB_ISDROPDOWN:
        ret = (IsPop() == ePopUp);
        return true;

    case CCB_SHOWDROPDOWN:
        CheckSetCheck(0 != wp);
        return true;

    case CCB_SUBMIT:
        if (IsPop() == ePopUp)
        {
            DISABLE_SPY();
            CTuiWidget(m_wndEdit).EndOwnedMenuPop(IDOK);
            return true;
        }
        return false;

    case CCB_SETREFERID:
        m_nReferID = lp;
        return true;

    case CCB_SETEDITSEL:
        if (m_wndList)
            SubmitText(false);
        return false;
   }

    if (!IsWindowEnabled())
        return false;

    switch (uMsg)
    {
    case CM_PRECOMMAND:
        if ((HIWORD(wp) == EN_CHANGEEX) && (lp == (TMLParam)(HTUI)m_wndEdit))
        {
            if (!IsUpDownMode() && Is_Style_CBS_FILTER())
            {
                Pop(false);
                PostMessage(CCB_FILTER, 0, 0);
            }
            return false;
        }
        break;

    case WM_SYSKEYDOWN:
        if (wp == VK_DOWN || wp == VK_UP)
        {
            DbgAssert(IsWindowEnabled());
            switch (IsPop())
            {
            case ePopUp:
                {
                    DISABLE_SPY();
                    if (CTuiWidget(m_wndEdit).EndOwnedMenuPop(IDCANCEL))
                        return true;
                }
                break;

            case ePopColl:
                CheckSetCheck(1);
                return true;
            }
        }
        break;

    case WM_KEYDOWN:
        {
            DbgAssert(IsWindowEnabled());
            ret = true;
            ResetUpDownMode();
            switch (wp)
            {
            case VK_RETURN:
            case VK_EXECUTE:
                if (IsPop() == ePopUp)
                {
                    TMBool b = SubmitText(false);
                    DISABLE_SPY();
                    CTuiWidget(m_wndEdit).EndOwnedMenuPop(b ? IDOK : IDCANCEL);
                }
                else
                    m_wndEdit->Notify(BN_CLICKED);
                return true;

            case VK_SPACE:
                return !Is_Style_CBS_SPACETOPOP();

            case VK_TAB:
                {
                    DISABLE_SPY();
                    CTuiWidget(m_wndEdit).EndOwnedMenuPop(IDCANCEL);
                }
                return ret = (IsPop() == ePopUp);

            case VK_DOWN:
            case VK_UP:
                if (hWgt == m_wndList)
                {
                    SetUpDownMode();
                    DISABLE_SPY();
                    
                    ICbList* ilist = GetICB();
                    TMUIntPtr now = ilist->GetSel(hWgt);
                    CTuiWidget(m_wndList).DefWindowProc(uMsg, wp, lp);
                    if (ilist->GetSel(hWgt) != now && (!Is_Style_CBS_NOIMEDIALTY() || (IsPop() == ePopColl)))
                        SubmitText(true);
                    return true;
                }
                else if ((ePopColl == IsPop()) && (m_dwStyle & (CBS_ARROWTOPOP | CBS_FILTER)))
                {
                    CheckSetCheck(1);
                    return true;
                }
                else
                {
                    DISABLE_SPY();
                    ret = m_wndList->SendMessage(WM_KEYDOWN, wp, 0);
                    return true;
                }
                break;
            }
            ret = false;
            break;
        }
    case WM_CHAR:
        {
            DbgAssert(IsWindowEnabled());
            DISABLE_SPY();
            if (m_wndEdit != this)
                ret = m_wndEdit->SendMessage(uMsg, wp, lp);
            return true;
        }

    }
    
    return false;
}

TMBool CTuiComboButtonImpl::ProcessWindowMessage(CTuiWgt* hWnd, TMUInt uMsg,
                                                TMWParam wParam, TMLParam lParam,
                                                TMResult& lResult, TMULong dwMsgMapID)
{
    switch (uMsg)
    {
    case WM_NCCREATE:
        OnCreate();
        break;

    case WM_NCDESTROY:
        {
            DISABLE_SPY();
            Unsubclass_m_wndEdit(m_wndEdit);
            m_wndEdit.Release();
            DISABLE_SPY();
            if (m_wndPop)
            {
                if (m_wndPop == m_wndList)
                    Unsubclass_m_wndPop(m_wndPop);
                else
                    m_wndPop->DestroyWindow();
                m_wndPop.Release();
            }
            DISABLE_SPY();
            if (m_wndList)
            {
                Unsubclass_m_wndList(m_wndList);
                m_wndList.Release();
            }
        }
        break;

    case WM_ENABLE:
//         m_wndEdit.EnableWindow(wParam);
    case CM_ENABLE_P:
        if (!wParam)
        {
            DISABLE_SPY();
            CTuiWidget(m_wndEdit).EndOwnedMenuPop(IDCANCEL);
        }
        break;

    case CM_LANGUAGECHANGED:
        InitListOnPop();
        break;
        
    case CBM_SETCHECK:
        ResetUpDownMode();
        CTuiPushButtonImpl::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
        if (m_nCurChecks)
            Pop(true);
        else
            CTuiWidget(m_wndEdit).EndOwnedMenuPop(IDCANCEL);
        return true;

    case CCB_FILTER:
        Filter();
        return true;

    case CCB_SETLISTITEMS:
        m_strItems = (TMCStr)lParam;
        return lResult = true;

    default:
        if (ProcessKeyDown(hWnd, uMsg, wParam, lParam, lResult))
            return true;
        break;
    }

    return CTuiPushButtonImpl::ProcessWindowMessage(hWnd, uMsg,
            wParam, lParam, lResult, dwMsgMapID);
}


TM_END_NAMESPACE

