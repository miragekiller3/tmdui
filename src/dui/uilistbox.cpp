/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2008.08.21
 *
 *  Copyright (C) 2008 miragekiller
 */

#include "uilistbox.h"
#include "uiheader.h"
#include <algorithm>
#include "pic.h"
#include "view.h"
#include "helper.h"
#include "tmfilesystem.h"


TM_BGN_NAMESPACE

#define ODS_HOTLIGHT        0x0040
// #define HSCROLL_ENTENT      100

////////////////////////////////////////////////////////////////////////////
//Register
TM_REGISTER_CLASS1(CTuiListBox);


////////////////////////////////////////////////////////////////////////////
// CColumnImpl
int CColumnImpl::GetColumnCount()
{
    DbgAssert(m_pHeader);
    return m_pHeader->GetItemCount();
}

TMLParam CColumnImpl::GetColumnData(int nCol)
{
    DbgAssert(m_pHeader);
    m_pHeader->_Check(nCol);
    return m_pHeader->m_aItem[nCol].lParam;
}

GXDips CColumnImpl::GetColumnWidth(int nCol)
{
    DbgAssert(m_pHeader);
    m_pHeader->_Check(nCol);
    return m_pHeader->m_aItem[nCol].nWidth;
}

int CColumnImpl::GetColumnFormat(int nCol)
{
    DbgAssert(m_pHeader);
    m_pHeader->_Check(nCol);
    int nf = m_pHeader->m_aItem[nCol].nFmt;
    if (nCol == m_nCurColumn)
        nf |= HDF_CURRENTSEL;
    return nf;
}


////////////////////////////////////////////////////////////////////////////
// LBITEM
TMBool LBITEM::GetRectForInvalidate(HTUI hSelf, CTuiRect& rect)
{
    CTuiListBoxImpl* listbox = static_cast<CTuiListBoxImpl*>(hSelf->GetParent());
    int n = this - listbox->m_aItem.data();
    return !listbox->GetItemRect(n, &rect, true);
}


////////////////////////////////////////////////////////////////////////////
// sort
struct _CTuiListBoxImpl_Sort
{
    PFNLBCOMPARE comp;
    TMLParam lp;
    
    bool operator () (const LBITEM& l1, const LBITEM& l2) const
    {
        return comp(l1.lParam, l2.lParam, lp);
    }
};

struct _CTuiListBoxImpl_Sort_Null
{
    bool operator () (const LBITEM& l1, const LBITEM& l2) const
    {
        return l1.strText() <  l2.strText();
    }
};

template <int bUp>
struct _CTuiListBoxImpl_Sort_Proxy
{
    IColumnProxy* proxy;
    TMLParam lp;
    
    bool operator () (const LBITEM& l1, const LBITEM& l2) const
    {
        if (bUp)
            return proxy->Sort(l1.lParam, l2.lParam, lp, bUp);
        else
            return proxy->Sort(l2.lParam, l1.lParam, lp, bUp);
    }
};


////////////////////////////////////////////////////////////////////////////
//CTuiListBoxImpl
struct _CTuiListBoxImpl
{
    static void GetItemTB(CTuiListBoxImpl* hWnd, TMULong n, CTuiRect& rc, const CTuiRect& rcPadding)
    {
        rc.SetTop(rcPadding.Top() + n * hWnd->m_nItemHeight - hWnd->GetScrollTop());
        rc.SetHeight(hWnd->m_nItemHeight);
    }
};

CTuiListBoxImpl::CTuiListBoxImpl()
    : m_nCurSel(-1)
    , m_LdownSel(-1)
    , m_nItemHeight(-1)
    , m_nFirstVisible(0)
    , m_nEndVisible(0)
    , m_uIndent(4)
    , m_pfnDelete(0)
    , m_pfnBindBackup(0)
{
    m_dwStyle |= WS_TABSTOP;
    SetAutoHScroll();
    SetAutoVScroll();
}

CTuiListBoxImpl::~CTuiListBoxImpl()
{
    DbgAssert(m_aItem.empty());
}

// CTuiWgt
void CTuiListBoxImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    if (Is_WS_PRI_DIRTY())
    {
        Reset_WS_PRI_DIRTY();
        if (m_nItemHeight < 0)
            m_nItemHeight = gapp->FontGetHeight();
        GXDips y = m_nItemHeight * GetCount();
        GXDips x;
        GXDips* px = 0;
        if (m_pHeader)
        {
            x = m_pHeader->GetScrollWidth();
            px = &x;
        }
        if (SendScrollRangeMessage(px, &y))
        {
//             AutoScroll();
            Invalidate();
            return;
        }
    }

    if (gapp->ClipAndPush(m_rcClient, true, eGXPropClip | eGXPropTextColor))
    {
        TMColor clr = ColorFromSys(COLOR_WINDOWTEXT);
        if (!IsHighContrast())
            clr = GetModule()->ConvertColor(GetObjectID(), clr);
        gapp->SetTextColor(clr);
        NativePrintClient(gapp, lParam);
        gapp->Pop();
    }
}

void CTuiListBoxImpl::ReclaimItems(TMUInt32 nFirst, TMUInt32 nLast)
{
    tm_setmin(nLast, m_aItem.size());
    for (; nFirst < nLast; ++nFirst)
    {
        LBITEM& itm = m_aItem[nFirst];
        itm.gxText.Reclaim();
        itm.BindReclaim(m_pfnBindBackup, this, nFirst, itm.lParam);
        if (m_proxy)
            m_proxy->Reclaim(itm.lParam);
    }
}

bool CTuiListBoxImpl::GetWindowHint(ETTActive ett, CString* str)
{
    CView* view = GetView();
    int sel;
    int selHeader = -1;
    if (ett == eTTActvieTab)
        sel = m_nCurSel;
    else
    {
        TUIHITINFO& hi = view->GetHitInfo();
        sel = (hi.bNC || hi.hWnd != this) ? LB_ERR : hi.dwCode;
        if (m_pHeader)
            selHeader = ((HITCODE_HEADER&)hi.dwCode2).iItem;
    }
    
    if (sel != LB_ERR)
    {
        LBITEM& itm = m_aItem[sel];
        if (selHeader >= 0)
        {
            DbgAssert(m_pHeader);
            if (_MakeWindowHInt(str, m_proxy, itm.lParam, m_pHeader->m_aItem[selHeader].lParam))
                return true;

//             CHIntSourceLB hslb;
//             hslb.proxy = m_proxy;
//             hslb.lPItem = itm.lParam;
//             hslb.lPHead = m_pHeader->m_aItem[selHeader].lParam;
//             if (hslb.MakeHInt(this, ett, str))
//                 return true;
//             if (str)
//             {
//                 *str = m_proxy->GetHInt(itm.lParam, m_pHeader->m_aItem[selHeader].lParam);
//                 if (*str)
//                     return true;
// //                 m_proxy->GetRenderText()
//             }
        }

        if (_MakeWindowHInt(str, itm.strHint, itm.gxText))
            return true;

//         if (itm.strHint)
//         {
//             if (str)
//                 *str = itm.strHint;
//             return true;
//         }
//         
//         if (Text2Hint(str, view, itm.gxText))
//             return true;
    }
    
    return CTuiWgt::GetWindowHint(ett, str);
}

void CTuiListBoxImpl::Reclaim()
{
    ReclaimItems(m_nFirstVisible, m_nEndVisible);
    CForePicWin<CTuiWgt>::Reclaim();
}

TMBool CTuiListBoxImpl::EnableItem(int nIndex, TMBool bEnable)
{
    if (nIndex < 0 || nIndex >= m_aItem.size())
        return false;
    m_aItem[nIndex].bEnable = (0 != bEnable);
    return true;
}

struct _CTuiListBoxImpl_MakeVisible
{
    GXDips nOrgY;
    GXDips nItemHeight;
    int nCount;
    int n1;
    int n2;

    _CTuiListBoxImpl_MakeVisible(GXDips orgY, int count, GXDips high)
        : nOrgY(orgY)
        , nCount(count)
        , nItemHeight(high)
    {
    }

    void Make(GXDips top, GXDips bottom)
    {
        n1 = (top - nOrgY) / nItemHeight;
        n2 = (bottom - nOrgY) / nItemHeight + 1;
        tm_setmax(n1, 0);
        tm_setmin(n2, nCount);
    }
};

void CTuiListBoxImpl::NativePrintClient(IGXApp* gapp, TMLParam lParam)
{
    CTuiRect rcPadding;
    GetPaddingRect(&rcPadding);
    TUIDRAWITEM  drwP = {0};
    drwP.CtlID = GetDlgCtrlID();
//     drwP.hDC = hDC;
    drwP.rcItem = rcPadding;
    drwP.dwDrawStage = CDDS_PREPAINT;
    drwP.hWnd = this;
    drwP.gxApp = gapp;
    if (m_pHeader && m_pHeader->m_aItem.size())
    {
        DbgAssert(m_proxy);
        drwP.pColumn = this;
        drwP.proxy   = m_proxy;
    }
    SendMessage(CM_DRAWITEM, 0, TMLParam(&drwP));

    // calc first and last item then reclaim
    _CTuiListBoxImpl_MakeVisible mv(GetOrgY(), GetCount(), m_nItemHeight);

    // free memory before first visible
    mv.Make(m_rcClient.Top(), m_rcClient.Bottom());
    ReclaimItems(m_nFirstVisible, mv.n1);
    ReclaimItems(mv.n2, m_nEndVisible);
    m_nFirstVisible = mv.n1;
    m_nEndVisible = mv.n2;

    // calc real first and last item then render
    CTuiRect rcClip;
    gapp->GetClip(rcClip);
    rcClip &= rcPadding;
    mv.Make(rcClip.Top(), rcClip.Bottom());

    CTuiRect rcItmBase(rcPadding);
    rcItmBase.KeepROffsetL(-GetScrollLeft());// .left -= GetScrollLeft();

    int nHot = -1;
    const TUIHITINFO& ht = GetView()->GetHitInfo();
    if ((GetView()->GetOver() == this) && !ht.bNC) // && (m_dwStyle & LBS_TRACKMOUSE))
        nHot = ht.dwCode;

    TUIDRAWITEM  drwS = drwP;
    drwS.rcItem = rcItmBase;

    for (int n1 = mv.n1; n1 < mv.n2; ++n1)
    {
        LBITEM& itm = m_aItem[n1];

        drwS.itemID = n1;
        drwS.hWndBind = itm.m_hBind;
        drwS.dwDrawStage = CDDS_ITEMPREPAINT;
        drwS.itemData = itm.lParam;
        drwS.itemState = 0;
        drwS.gxText = &(itm.gxText.p);
        _CTuiListBoxImpl::GetItemTB(this, n1, (CTuiRect&)drwS.rcItem, rcPadding);
        if (m_dwStyle & WS_DISABLED || itm.bEnable == TMFalse)
            drwS.itemState = ODS_DISABLED;
        else
        {
            if (IsFocus())
                drwS.itemState |= ODS_FOCUS;
            if (itm.bSel)
                drwS.itemState |= ODS_SELECTED;
            if ((nHot == n1) && Is_LBS_TRACKMOUSE())
                drwS.itemState |= ODS_HOTLIGHT;
//         else
//             drwS.itemState = itm.bSel ? ODS_SELECTED: ((nHot == n1) ? ODS_HOTLIGHT : ODS_DEFAULT);
//         drwS.itemAction = ODA_SELECT;
        }

        if (itm.BindRestore(m_pfnBindBackup, this, &itm, n1, itm.lParam))
        {
            itm.m_hBind->ResetBindItemPosition(drwS.rcItem, SWP_NOREDRAW);
            if (Is_WS_PRI_DIRTY())
                break;
        }

        SendMessage(CM_DRAWITEM, 0, TMLParam(&drwS));
        if (Is_WS_PRI_DIRTY())
            break;

        if (itm.m_hBind)
        {
            CWidgetHelper::DoPrintSelf(gapp, itm.m_hBind, c_lpPrint | PRF_BIND);
            if (Is_WS_PRI_DIRTY())
                break;
        }
    }

    drwP.rcItem = rcPadding;
    drwP.dwDrawStage = CDDS_POSTPAINT;
    SendMessage(CM_DRAWITEM, 0, TMLParam(&drwP));
}

void CTuiListBoxImpl::OnDrawItem(TMWParam wParam, TMLParam lParam)
{
    TUIDRAWITEM* pDrawS = (TUIDRAWITEM*)lParam;
    if (pDrawS->dwDrawStage == CDDS_PREPAINT)
        RenderSimpleBk(pDrawS->gxApp, 0, CAppColor::GetColorWindowBk);
    else if (pDrawS->dwDrawStage == CDDS_ITEMPREPAINT)
    {
        CPropColor clr[] = { m_clrNormal, m_clrSelect, m_clrHilight, CPropColor::eClrDefault, m_clrFocus };
        RenderListItem(pDrawS, m_pFore, m_uIndent, clr);
    }
}

TMResult CTuiListBoxImpl::SelItem(int nItem, int nColumn, TMUInt32 dwStyle, TMUInt16 nModifier, TMBool bEnsureVisible)
{
    //DbgAssert(nItem>=0 && nItem<GetCount());
    if (nItem >= GetCount() || nItem < -1)
        return LB_ERR;

    if (m_nCurSel != -1)
        m_aItem[m_nCurSel].Bind_Unselect();

    m_nCurSel = nItem;
    m_nCurColumn = nColumn;
    if (nItem == -1)
    {
        for (CItemRec rec(m_aItem); rec; ++rec)
            rec->bSel = false;
        m_LdownSel = nItem;
    }
    else if (dwStyle & LBS_NOSEL)
    {
    }
    else if ((dwStyle & LBS_MULTIPLESEL) || ((dwStyle & LBS_EXTENDEDSEL) && (nModifier == MOD_CONTROL)))
    {
        LBITEM& item = m_aItem[nItem];
        item.bSel = !item.bSel;
        m_LdownSel = nItem;
    }
    else if ((dwStyle & LBS_EXTENDEDSEL) && (nModifier == MOD_SHIFT) && (m_LdownSel != -1))
    {
        int n1 = tm_min(nItem, m_LdownSel);
        int n2 = tm_max(nItem, m_LdownSel);
        for (int i = m_aItem.size() - 1; i >= 0; --i)
        {
            m_aItem[i].bSel = ((i >= n1) && (i <= n2));
        }
    }
    else
    {
        for (CItemRec rec(m_aItem); rec; ++rec)
            rec->bSel = false;
        m_LdownSel = nItem;
        m_aItem[nItem].bSel = true;
    }

    if (m_nCurSel != -1)
        m_aItem[m_nCurSel].Bind_Select();
    
    if (bEnsureVisible)
        AutoScroll();
    SetDirtyAndInvalidate();
    Notify(LBN_SELCHANGE);
    return 0;
}


void CTuiListBoxImpl::OnLButtonDown(TMWParam wParam, TMLParam lParam)
{
    TUIHITINFO hi;
    GetView()->GetMouseLDownState(hi);
    int n = hi.dwCode;
    if (n < 0 || n >= GetCount())
        return;
    HITCODE_HEADER hih = hi.dwCode2;
    SelItem(n, hih.iItem, GetStyle(), g_input.m_key.GetModifiersFast(), true);
}

void CTuiListBoxImpl::OnMouseSwitch(TMWParam wParam, TMLParam lParam)
{
//     if (!(m_dwStyle & (LBS_INFOTIP | LBS_TRACKMOUSE | LBS_TRACKSEL | LBS_MENUITEM)))
//         return;

    TUIHITINFO* pho = (TUIHITINFO*)wParam; // old
    TUIHITINFO* phn = (TUIHITINFO*)lParam; // new
    if (phn->hWnd == this)
    {
        // set hint
        if (g_mgrTooltip.IsTooltipTarget(this))
            g_mgrTooltip.SetToolTip(this, eTTActiveNormal);

        if (!phn->bNC)
        {
            // track select
            if (Is_LBS_MENUITEM() || Is_LBS_TRACKSEL())
                SelItem(phn->dwCode, HITCODE_HEADER(phn->dwCode2).iItem, 0, 0, true);

            // menu item
            if (Is_LBS_MENUITEM())
                SetFocus(eFocusReasonMenuBar);

            // redraw
//             if (m_dwStyle & (LBS_TRACKMOUSE | LBS_TRACKSEL))
            InvalidateHT(phn);
        }
    }

    // redraw
    if (pho->hWnd == this) // && (m_dwStyle & (LBS_TRACKMOUSE | LBS_TRACKSEL)))
        InvalidateHT(pho);
}

// void CTuiListBoxImpl::OnMouseMove(TMWParam wParam, TMLParam lParam)
// {
//     if (!GetView()->IsMouseDown())
//         return;
// 
//     int yPos = (int)(short)HIWORD(lParam);
//     int yScroll = GetScrollTop();
//     int nItem = (yPos - m_rcClient.Top() - m_bdPadding.top + yScroll) / m_nItemHeight;
//     if (nItem >= GetCount() || nItem == m_nCurSel || nItem < 0)
//         return;
// 
//     TMUInt32 dwStyle = GetStyle();
//     if (dwStyle & LBS_MULTIPLESEL)
//     {
//         dwStyle = LBS_NOSEL;
//     }
//     else if (dwStyle & LBS_EXTENDEDSEL)
//     {
//         wParam = MK_SHIFT;
//     }
// 
//     SelItem(nItem, dwStyle, CKeyboard::GetSimpleModifiers(), false);
// }

// TMResult CTuiListBoxImpl::OnDrag(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
// {
//     DROPEVENT* pDropEvent = (DROPEVENT*)lParam;
//     if (!pDropEvent)
//     {
//         return S_OK;
//     }
//     CTuiPoint pt;
//     //ScreenToClient(&pDropEvent->ptScreen, 1, &pt);
//     CTuiZoom(GetDPI()).DPtoLP(&pDropEvent->ptWindow, 1, &pt);
//     int nIndex = GetItem(pt.Y());
//     if (nIndex == LB_ERR)
//     {
//         return S_OK;
//     }
//     HTUI hItem = m_aItem[nIndex].m_hBind;
//     if (!hItem)
//     {
//         return S_OK;
//     }
//     return hItem->SendMessage(uMsg, wParam, lParam);
// }

void CTuiListBoxImpl::AutoScroll()
{
    if (m_nCurSel < 0)
        return;

    GXDips yScroll = GetScrollTop();
//    GXDips n1 = m_rcClient.top + m_bdPadding.top;
//    GXDips n2 = m_rcClient.bottom - m_bdPadding.bottom;
    GXDips nTop = m_nCurSel * (GXDips)m_nItemHeight;
    GXDips nBottom = nTop + m_nItemHeight;
    if (nTop < yScroll)
        SetScrollTop(nTop);
    else if (nBottom > yScroll + m_rcClient.Bottom() - m_rcClient.Top() - m_bdPadding.top - m_bdPadding.bottom)
        SetScrollTop(nBottom + m_rcClient.Top() - m_rcClient.Bottom() + m_bdPadding.top + m_bdPadding.bottom);
}

TMBool CTuiListBoxImpl::OnKeyDown(int key, TMLParam lParam)
{
    int nItem;

    switch (key)
    {
    case VK_UP:
//    case VK_LEFT:
        if (m_nCurSel <= 0)
            return (m_dwStyle & LBS_MENUITEM) ? false : true;
        nItem = m_nCurSel - 1;
        break;

//    case VK_RIGHT:
    case VK_DOWN:
        if (m_nCurSel >= GetCount() - 1)
            return (m_dwStyle & LBS_MENUITEM) ? false : true;
        nItem = m_nCurSel + 1;
        break;

    case VK_HOME:
        if (m_nCurSel == 0)
            return false;
        nItem = 0;
        break;
        
    case VK_END:
        nItem = GetCount() - 1;
        if (nItem == m_nCurSel)
            return false;
        break;

    case VK_PRIOR: // page up
        nItem = GetScrollTop() / m_nItemHeight;
        if (nItem == m_nCurSel)
            nItem -= GetViewHeight() / m_nItemHeight;
        tm_setmax(nItem, 0);
        if (nItem == m_nCurSel)
            return false;
        break;

    case VK_NEXT: // page down
        nItem = (GetScrollTop() + GetViewHeight() - 1) / m_nItemHeight;
        if (m_nCurSel == nItem)
            nItem += GetViewHeight() / m_nItemHeight;
        tm_setmin(nItem, GetCount() - 1);
        if (m_nCurSel == nItem)
            return false;
        break;

    case VK_SPACE:
        nItem = m_nCurSel;
        break;

    default:
        return false;
    }

    TMUInt32 dwStyle = GetStyle();
    int nSelOld = m_nCurSel;
    SelItem(nItem, m_nCurColumn, dwStyle, CKeyInput(key, lParam).GetModifiersFast(), true);
    if (m_nCurSel != nSelOld)
        g_mgrTooltip.SetTabToolTip(this);

    return true;
}

// static int GetFontHeight(HTUI hWnd)
// {
//     GXFont ft;
//     hWnd->GetRealFont(ft);
// 
// //    wnd.GetFont(&ft);
// //    gxFontGet(hWnd->GetFont(), ft);
//     return ft.height > 0 ? ft.height : -ft.height;
// 
// //    CSize size(0,0);
// //    GetCharDimensions(TMFont2Font(hWnd->GetFont()), size);
// //    return size.cy;
// }

TMBool CTuiListBoxImpl::ProcessWindowMessage(
    CTuiWgt* hWnd, TMUInt uMsg,
    TMWParam wParam, TMLParam lParam,
    TMResult& lResult, TMULong dwMsgMapID)
{
    DbgAssert(!IsLifeDestroyed());

    TMBool bHandled = true;

    switch (uMsg)
    {
    case WM_CREATE:
        {
            bHandled = false;

//             if (!Edit_IsObjectInDocMode(this) && m_nItemHeight == -1)
//                 m_nItemHeight = GetFontHeight(this);

            //CTuiWidget(this).SetRedraw(false);
            ResetFlagRedraw();
            CGXStrPtr strAll(m_gxText.GetString());
            CString str;
            for (int n = 0; n = strAll.Split (n, '\n', str);)
            {
                str = str.Trim();
                AddString(str);
            }
//             CTuiWidget(this).SetRedraw(true);
            SetFlagRedraw();
        }
        break;

//     case CM_ZOOMED:
//         bHandled = false;
//         m_pFore.ChangeDPI(wParam);
//         break;

    case CM_SETFONT:
        CTuiWgt::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);
//         m_nItemHeight = GetFontHeight(this);
        SetDirtyAndInvalidate();
        break;

    case WM_SETFOCUS: // up/down case; tab/shift/tab case;
        Notify(LBN_SETFOCUS);
        Invalidate();
        if ((LBS_MENUITEM & m_dwStyle) && (m_nCurSel == -1))
        {
            TMBool b = g_input.m_key.IsKey(VK_UP) || (g_input.m_key.IsKeyTab() < 0);
            int itm = b ? m_aItem.size() - 1 : 0;
            CTuiListBox(this).SetCurSel(itm);
        }
        bHandled = false;
        break;

    case WM_KILLFOCUS:
        if (LBS_MENUITEM & m_dwStyle)
            CTuiListBox(this).SetCurSel(-1);
        Notify(LBN_KILLFOCUS);
        Invalidate();
        bHandled = false;
        break;

    case WM_LBUTTONDBLCLK:
        Notify(LBN_DBLCLK);
        break;

    case WM_LBUTTONDOWN:
        if (m_dwStyle & WS_TABSTOP)
            SetFocus(eFocusReasonMouse);
//        SetCapture();
        OnLButtonDown(wParam, lParam);
        break;

    case WM_LBUTTONUP:
//        ReleaseCapture();
        break;

//     case WM_MOUSEMOVE:
//         OnMouseMove(wParam, lParam);
//         break;

    case CM_MOUSESWITCH:
        OnMouseSwitch(wParam, lParam);
        break;

    case CM_HITTEST:
        CTuiWgt::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);
        if (!((TUIHITINFO*)lParam)->bNC)
            OnHitTest(wParam, *(TUIHITINFO*)lParam);
        break;

    case CM_DRAWITEM:
        OnDrawItem(wParam, lParam);
        break;

    case CM_SETDELETEDATA:
        m_pfnDelete = (PFNDELETEPROC)lParam;
        break;

    case CM_SETPFNBINDBACKUP:
        m_pfnBindBackup = (PFNBINDBACKUP)lParam;
        break;

//     case CM_DRAGENTRY:
//     case CM_DRAGOVER:
//     case CM_DRAGLEAVE:
//     case CM_DRAGDROP:
//         lResult = OnDrag(uMsg, wParam, lParam);
//         break;

    case CLB_ISITEMENABLE:
        lResult = (wParam < 0 || wParam >= m_aItem.size()) ? false : m_aItem[wParam].bEnable;
        break;

    case CLB_ENABLEITEM:
        lResult = EnableItem(wParam, lParam);
        break;

//     case WM_GETDLGCODE:
//         lResult = /*DLGC_HASSETSEL |*/ DLGC_WANTCHARS | DLGC_WANTARROWS;
//         break;

    case WM_KEYDOWN:
        bHandled = lResult = OnKeyDown(wParam, lParam);
        break;

//     case WM_SETREDRAW:
//         if (wParam)
//         {
//             SetScroll();
//             AutoScroll();
//             CTuiWidget(this).ChangeFrame();
//         }
//         bHandled = false;
//         break;

    case CM_SETTEXTCOLOR:
        m_clrNormal = (lParam);
        Invalidate();
        break;

    case LB_ADDSTRING:
        {
            LBITEM itm;
            itm.InitString(this, lParam);
            lResult = AddString(itm);
        }
        break;
        
    case LB_INSERTSTRING:
        lResult = InsertString(wParam, lParam);
        break;

    case LB_DELETESTRING:
        lResult = DeleteString(wParam);
        break;

    case LB_SELECTSTRING: 
        lResult = FindString((int)wParam, (TMCStr)lParam, true, false);
        break;

    case LB_FINDSTRING:
        lResult = FindString((int)wParam, (TMCStr)lParam, false, false);
        break;

    case LB_FINDSTRINGEXACT:
        lResult = FindString((int)wParam, (TMCStr)lParam, false, true);
        break;

    case WM_DESTROY:
        bHandled = false;
    case LB_RESETCONTENT:
        {
            CItemList lst;
            lst.swap(m_aItem);
            for (CItemList::recorder rec(lst); rec; ++rec)
                CleanItem(rec.get_data(), 0);
        }
        m_nCurSel = -1;
        SetDirtyAndInvalidate();
        break; 

    case LB_SETSEL:
        if (lParam ==(TMLParam)-1)
        {
            for (CItemRec rec(m_aItem); rec; ++rec)
                rec->bSel = (0 != wParam);
        }
        else if (lParam >= GetCount())
        {
            lResult = -1;
            break;
        }
        else 
        {
            m_aItem[lParam].bSel = (0 != wParam);
        }
        Notify(LBN_SELCHANGE);
        Invalidate();
        break;

    case LB_SETCURSEL:
        lResult = SelItem(wParam, m_nCurColumn, 0, 0, lParam);
        break;

    case LB_GETSEL:
        lResult = ((wParam < GetCount()) && (!lParam || m_aItem[wParam].bEnable))
            ? m_aItem[wParam].bSel : false;
        break;

    case LB_GETCURSEL:
        lResult = (m_nCurSel >= 0 && m_aItem[m_nCurSel].bSel && (!lParam || m_aItem[m_nCurSel].bEnable))
            ? m_nCurSel : -1;
        break;

    case CLB_SETTEXT:
        if (lParam == 0 || wParam >= GetCount())
        {
            lResult = LB_ERR;
        }
        else
        {
            m_aItem[wParam].InitString(this, lParam);
            lResult = wParam;
            Invalidate();
        }
        break;

    case CLB_GETTEXT:
        if (lParam == 0 || wParam >= GetCount())
        {
            lResult = LB_ERR;
        }
        else
        {
            *((CStrPtr*)lParam) = m_aItem[wParam].strText();
        }
        break;

    case CM_SETITEMACCTEXT:
        if (lParam == 0 || wParam >= GetCount())
            lResult = LB_ERR;
        else
        {
            StringMessageToString(lParam, m_aItem[wParam].strAcc);
            lResult = wParam;
        }
        break;

    case CM_GETITEMACCTEXT:
        if (lParam == 0 || wParam >= GetCount())
            lResult = LB_ERR;
        else
        {
            CStrPtr* str = (CStrPtr*)lParam;
            *str = m_aItem[wParam].strAcc;
            lResult = wParam;
        }
        break;

    case CM_SETITEMHINT:
        if (lParam == 0 || wParam >= GetCount())
            lResult = LB_ERR;
        else
        {
            StringMessageToString(lParam, m_aItem[wParam].strHint);
            lResult = wParam;
        }
        break;

    case CM_GETITEMHINT:
        if (lParam == 0 || wParam >= GetCount())
            lResult = LB_ERR;
        else
        {
            CStrPtr* str = (CStrPtr*)lParam;
            *str = m_aItem[wParam].strHint;
            lResult = wParam;
        }
        break;

    case LB_GETTEXTLEN :
        lResult = (wParam >= GetCount())
            ? LB_ERR
            : m_aItem[wParam].strText().GetLength();
        break;

    case LB_GETCOUNT: 
        lResult = GetCount();
        break;

//     case LB_GETHORIZONTALEXTENT:
//         lResult = m_cxExtent;
//         break;
// 
//     case LB_SETHORIZONTALEXTENT:
//         m_cxExtent = (int)wParam;
//         ResetScroll();
//         //CTuiWidget(this).ChangeFrame();
//         SetScroll();
//         break;

    case LB_GETITEMDATA:
        lResult = (wParam >= GetCount()) ? LB_ERR : m_aItem[wParam].lParam;
        break;

    case LB_SETITEMDATA:
        if (wParam >= GetCount())
        {
            lResult = LB_ERR;
        }
        else
        {
            m_aItem[wParam].lParam = lParam;
        }
        break;

    case LB_SELITEMRANGE:
        lResult = SelItemRange(0 != wParam, HIWORD(lParam), LOWORD(lParam));
        break;

    case LB_SELITEMRANGEEX:
        if (wParam<lParam)
        {
            lResult = SelItemRange(true, wParam, lParam);
        }
        else
        {
            lResult = SelItemRange(false, lParam, wParam);
        }
        break;

    case LB_GETTOPINDEX:
        lResult = GetScrollTop() / m_nItemHeight;
        break;
        
    case LB_DIR:
        {
            DISABLE_SPY();
            CTuiWidget(this).SetRedraw(false);
            {
                wParam &= 0xFF;
                for (TM::CFindFile f((TMCStr)lParam); f; f.FindNext())
                {
                    if (f.MatchesMask(wParam))
                        AddString(CString(f.GetFileName()));
                }
            }
            DISABLE_SPY();
            CTuiWidget(this).SetRedraw(true);
            lResult = GetCount() - 1;
        }
        break;

    case LB_GETSELCOUNT:
        if (GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL))
        {
            lResult = 0;
            for (CItemRec rec(m_aItem); rec; ++rec)
            {
                if (rec->bSel)
                {
                    ++lResult;
                }
            }
        }
        else
        {
            lResult = LB_ERR;
        }
        break;

    case LB_GETSELITEMS:
        if (GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL))
        {
            lResult = 0;
            int i = 0;
            int* p = (int*)lParam;
            for (CItemRec rec(m_aItem); rec && lResult < wParam; ++rec, ++i)
            {
                if (rec->bSel)
                {
                    p[lResult] = i;
                    ++lResult;
                }
            }
        }
        else
        {
            lResult = LB_ERR;
        }
        break;
        
// #define LB_SETTABSTOPS          0x0192
// #define LB_SETCOLUMNWIDTH       0x0195

    case LB_ADDFILE:
        lResult = AddString(CString((TMCStr)lParam));
        break;

    case LB_SETTOPINDEX:
        SetScrollTop(wParam* m_nItemHeight);
        Invalidate();
        break;

    case LB_GETITEMRECT:
        lResult = GetItemRect(wParam, (CTuiRect*)lParam);
        break;

    case LB_SETANCHORINDEX:
        if (wParam >= GetCount())
        {
            lResult = LB_ERR;
        }
        else
        {
            m_LdownSel = wParam;
        }
        break;

    case LB_GETANCHORINDEX:
        lResult = m_LdownSel;
        break;

    case LB_SETCARETINDEX:
        if (wParam >= GetCount())
        {
            lResult = LB_ERR;
        }
        else
        {
            m_nCurSel = wParam;
            SetDirtyAndInvalidate();
        }
        break;
        
    case LB_GETCARETINDEX:
        lResult = m_nCurSel;
        break;

    case LB_SETITEMHEIGHT:
        m_nItemHeight = lParam;
        SetDirtyAndInvalidate();
        break;

    case LB_GETITEMHEIGHT:
        lResult = m_nItemHeight;
        break;

    case CLB_DOWNCOLOR:
        m_clrSelect = wParam;
        break;

//     case LB_SETLOCALE:
//     case LB_GETLOCALE:

    case LB_SETCOUNT:
        m_aItem.resize(wParam);
        SetDirtyAndInvalidate();
        break;

    case CLB_CREATEBINDWIN:
        lResult = (TMResult)CreateBindWin(wParam, lParam);
        break;
        
    case CLB_BINDWIN:
        lResult = BindWin(wParam, (HTUI)lParam);
        break;

    case CLB_DETACHBINDWIN:
        lResult = (TMResult)DetachBindWin(wParam, (HTUI)lParam);
        break;
        
    case CLB_GETBINDWIN:
        lResult = wParam >= GetCount() ? 0 : (TMResult)(HTUI)m_aItem[wParam].m_hBind;
        break;

    case CM_MSGROUTE:
        bHandled = CWidgetHelper::OnBindModeRoute(this, (TUIMSG*)(lParam), lResult);
        break;

    case CM_MSGBUBBLE:
        bHandled = CWidgetHelper::OnBindModeBubble(this, (TUIMSG*)(lParam), lResult);
        break;

    case CLB_SORT:
        OnSort((PFNLBCOMPARE)wParam, lParam);
        break;

    case CLB_ENUM:
        return (lResult = OnEnum((PFNLBENUM)wParam, lParam)), true;

    case CM_SHOWWINDOW_P:
    case CM_ENABLE_P:
    case CM_SETDPI:
    case CM_LANGUAGECHANGED:
    case CM_RENDERCHANGED:
        tm_for (CItemRec rec(m_aItem); rec; ++rec)
        {
            LBITEM& p = rec.get_data();
            if (p.m_hBind)
                p.m_hBind->SendBroadcastMessage(uMsg, wParam, lParam);
        };
        bHandled = false;
        break;

    // header
    case CLB_SETHEADERPROXY:
        if (lParam)
        {
            HTUI hHeader = (HTUI)wParam;
            if (!hHeader)
                hHeader = GetChild();
            if (hHeader && hHeader->GetClassName() != TUI_CLASS_HEADER)
                hHeader = 0;
            m_pHeader = (CTuiHeaderImpl*)hHeader;
            if (!m_pHeader)
                m_proxy = 0;
            else
                m_proxy = (IColumnProxy*)lParam;
        }
        else
        {
            m_pHeader = 0;
            m_proxy = 0;
        }
        break;

    case CLB_EXPROTCSV:
        lResult = ExportToCSV((IStream*)lParam, *(const CStringA*)wParam);
        return true;

    case CM_NOTIFY:
        {
            NMBASE* p = (NMBASE*)lParam;
            if (p->hwndFrom != m_pHeader)
                break;
            if (p->code == HDN_ITEMCLICK && p->lp2)
            {
                if (p->lp2 & HDF_SORTUP)
                {
                    _CTuiListBoxImpl_Sort_Proxy<true> sp = { m_proxy, p->lpItemData };
                    std::sort(m_aItem.begin(), m_aItem.end(), sp);
                }
                else if (p->lp2 & HDF_SORTDOWN)
                {
                    _CTuiListBoxImpl_Sort_Proxy<false> sp = { m_proxy, p->lpItemData };
                    std::sort(m_aItem.begin(), m_aItem.end(), sp);
                }
                else
                {
                    _CTuiListBoxImpl_Sort_Proxy<true> sp = { m_proxy, -1 };
                    std::sort(m_aItem.begin(), m_aItem.end(), sp);
                }
                Notify(LBN_SORTED);
            }
            else if ((p->code == HDN_ITEMCHANGED) && (((HDITEM*)p->lp2)->mask & HDI_WIDTH))
                SetDirtyAndInvalidate();
        }
        Invalidate();
        break;

    default:
        bHandled = false;
        break;
    }

    if (bHandled)
        return TMTrue;

    CHAIN_MSG_MAP(CForePicWin<CTuiWgt>)

    return false;
}

HTUI CTuiListBoxImpl::CreateBindWin(TMWParam nIndex, TMLParam dwObjID)
{
    if (nIndex >= GetCount())
        return 0;
    LBITEM& itm = m_aItem[nIndex];
    return itm.BindCreate(m_pfnBindBackup, this, dwObjID, &itm, nIndex, itm.lParam);
}

TMBool CTuiListBoxImpl::BindWin(TMWParam nIndex, HTUI hWnd)
{
    if (nIndex >= GetCount())
        return false;
    LBITEM& itm = m_aItem[nIndex];
    if (itm.m_hBind != hWnd)
        itm.DoBindWin(hWnd, this, &itm);
    return true;
}

HTUI CTuiListBoxImpl::DetachBindWin(TMWParam nIndex, HTUI hNewParent)
{
    if (nIndex >= GetCount())
        return 0;

    HTUI hWnd = m_aItem[nIndex].m_hBind;
    if (!hWnd)
        return 0;

    m_aItem[nIndex].m_hBind = 0;
    hWnd->Bind_Reset(hNewParent ? hNewParent : this);
    return hWnd;
}


void CTuiListBoxImpl::OnHitTest(TMWParam wParam, TUIHITINFO& hit)
{
    if (hit.bNC)
        return;

    GXDips y = PointY(hit.pt);
    GXDips nTop = (y - m_rcClient.Top() - m_bdPadding.top + GetScrollTop());
    int nCode = nTop / m_nItemHeight;
    if (nCode < 0 || nCode >= GetCount())
    {
        hit.dwCode = -1;
        return;
    }

    hit.dwCode = nCode;
    if (m_pHeader)
    {
        TUIHITINFO hht(hit);
//        hht.pt.x += GetScrollLeft();
        hht.pt.y = (m_pHeader->m_rcClient.Top() + m_pHeader->m_rcClient.Bottom()) / 2;
        m_pHeader->OnHitTest(hht);
        hit.dwCode2 = hht.dwCode;
    }

    LBITEM& item = m_aItem[nCode];
    if (item.m_hBind)
    {
        TUIHITINFO htNew(hit);
        item.m_hBind->GetAndHitMouseTarget(htNew, wParam, 0);
        if (htNew.hWnd)
            hit = htNew;
    }
}

int CTuiListBoxImpl::AddString(const LBITEM& itm)
{
    TMUInt32 dwStyle = GetStyle();

    int n = 0;
    if ((LBS_SORT & dwStyle) && (m_aItem.size() > 0))
    {
        CItemList::iterator it = m_aItem.binary_insert(itm);
        n = it - m_aItem.begin();
    }
    else
    {
        m_aItem.push_back(itm);
        n = GetCount() - 1;
    }

    SetDirtyAndInvalidate();

    if (m_nCurSel >= n)
        ++m_nCurSel;

    return n;
}

int CTuiListBoxImpl::InsertString(int nIndex, TMLParam lp)
{
    int nCount = GetCount();
    if (nIndex > nCount || (GetStyle() & LBS_SORT))
        return LB_ERR;

    if (nIndex < 0)
        nIndex = nCount;

    LBITEM itm;
    itm.InitString(this, lp);
    m_aItem.insert(m_aItem.begin() + nIndex, itm);

    SetDirtyAndInvalidate();

    if (m_nCurSel >= nIndex)
        ++m_nCurSel;

    return nIndex;
}

int CTuiListBoxImpl::DeleteString(int nIndex)
{
    if (nIndex >= GetCount() || nIndex < -1)
        return -1;

//    LBITEM& itm = m_aItem[nIndex];
    
    {
        LBITEM& itm = m_aItem[nIndex];
        LPARAM lp = itm.lParam;
        Ref m_hBind = itm.m_hBind;
        m_aItem.erase(m_aItem.begin() + nIndex);
        CleanItem(lp, m_hBind, nIndex);
    }
    
    if (m_nCurSel == -1)
        ;
    else if (m_nCurSel == nIndex)
        m_nCurSel = -1;
    else if (m_nCurSel > nIndex)
        --m_nCurSel;

    SetDirtyAndInvalidate();

    return GetCount();
}

int CTuiListBoxImpl::FindString(int nStart, TMCStr lStr, TMBool bSel, TMBool bExact)
{
    int nMax = GetCount();
    if (nStart < -1 || !lStr || nStart >= nMax)
        return -1;
    int nLen = StrLen(lStr);
    if (nStart < 0)
        nStart = 0;
    
    for (int i = 0; i < nMax; ++i)
    {
        int x = i + nStart;
        if (x >= nMax)
            x -= nMax;

        LBITEM& itm = m_aItem[x];
        const CString& str = itm.strText();
        if (str.IsEmpty())
        {
            continue;
        }
        if ((!bExact || nLen == str.GetLength())
            && (str.CompareNoCase(lStr) == 0)) //  _tcsncicmp(str, lStr, nLen)
        {
            if (bSel)
            {
                SelItem(x, m_nCurColumn, GetStyle(), MOD_CONTROL, true);
            }
            return x;
        }
    }

    return -1;
}

TMResult CTuiListBoxImpl::GetItemRect(TMULong n, CTuiRect* prc, TMBool bForInvalidate)
{
    if (n >= GetCount())
        return LB_ERR;

    GetPaddingRect(prc);
    if (!bForInvalidate)
        prc->KeepROffsetL(GetScrollLeft());
    _CTuiListBoxImpl::GetItemTB(this, n, *prc, *prc);
    return 0;
}

void CTuiListBoxImpl::InvalidateHT (TUIHITINFO* p)
{
    if (p->hWnd == this)
    {
        CTuiRect rc;
        if (GetItemRect (p->dwCode, &rc) != LB_ERR)
            InvalidateRect(rc);
    }
}

int CTuiListBoxImpl::SelItemRange(bool bSelect, int nFirst, int nLast)
{
    int    nCount = GetCount();

    if (nFirst < -1 || nLast >= nCount)
        return LB_ERR;

    for (; nFirst <= nLast; ++nFirst)
        m_aItem[nFirst].bSel = bSelect;

    Invalidate();
    return 0;
}

void CTuiListBoxImpl::OnSort(PFNLBCOMPARE pfn, TMLParam lp)
{
    if (pfn)
    {
        _CTuiListBoxImpl_Sort st = { pfn, lp };
        std::sort(m_aItem.begin(), m_aItem.end(), st);
    }
    else
        std::sort(m_aItem.begin(), m_aItem.end(), _CTuiListBoxImpl_Sort_Null());
    Notify(LBN_SORTED);
}

int CTuiListBoxImpl::OnEnum(PFNLBENUM pfn, TMLParam lp)
{
    LBENUM* penum = (LBENUM*)lp;
    tm_setmin(penum->nBgn, m_aItem.size());
    tm_setmin(penum->nEnd, m_aItem.size());
    int i = penum->nBgn;
    int j = i;
    for (; i < penum->nEnd; ++i)
    {
        if (pfn(i, m_aItem[i].lParam, penum->dataUser))
            break;
    }
    return i - j;
}

void CTuiListBoxImpl::CleanItem(TMLParam lParam, HTUI hBind, int idx)
{
    if (lParam && m_pfnDelete)
        m_pfnDelete(this, idx, lParam);
    
    if (hBind)
        hBind->DestroyWindow();
}

HRESULT _CTuiListBoxImpl_WriteString(IStream* stream, const CStringA& str)
{
    stream->Write("\"", 1, 0);
    HRESULT hr = stream->Write(str, str.length(), 0);
    stream->Write("\"", 1, 0);
    return hr;
}

bool CTuiListBoxImpl::ExportToCSV(IStream* stream, const CStringA& StrSplit)
{
    // write none proxy
    if (!m_proxy)
    {
        for (CItemList::recorder rec(m_aItem); rec; ++rec)
        {
            LBITEM& itm = rec.get_data();
            if (FAILED(_CTuiListBoxImpl_WriteString(stream, itm.gxText.GetString())))
                return false;
            stream->Write("\r\n", 2, 0);
        }
        return true;
    }

    // write header
    DbgAssert(m_pHeader);
    tm_for(CTuiHeaderImpl::CItemList::recorder il(m_pHeader->m_aItem); il; ++il)
    {
        CTuiHeaderImpl::ITEM& itm = il.get_data();
        CStdStrA str(itm.gxText.GetString()); 
        stream->Write(str, str.length(), 0);
        stream->Write(StrSplit, StrSplit.length(), 0);
    }
    stream->Write("\r\n", 2, 0);

    // write column
    int nh = m_pHeader->GetItemCount();
    for (CItemList::recorder rec(m_aItem); rec; ++rec)
    {
        LBITEM& itm = rec.get_data();
        for (int i = 0; i < nh; ++i)
        {
            CText gt;
            m_proxy->GetRenderText(itm.lParam, m_pHeader->m_aItem[i].lParam, gt);
            if (FAILED(_CTuiListBoxImpl_WriteString(stream, gt.GetString())))
                return false;
            stream->Write(StrSplit, StrSplit.length(), 0);
        }
        stream->Write("\r\n", 2, 0);
    }
    return true;
}

// int CTuiListBoxImpl::GetItem(GXDips nPosY)
// {
//     if (nPosY < 0)
//     {
//         return LB_ERR;
//     }
//     GXDips yScroll = GetScrollTop();
//     int nItem = (nPosY - m_rcClient.Top() - m_bdPadding.top + yScroll) / m_nItemHeight;
//     if (nItem >= m_aItem.size())
//     {
//         return LB_ERR;
//     }
//     return nItem;
// }

#ifdef TUI_EDIT_MODE

CString CTuiListBoxImpl::GetDebugInfo()
{
    CString str;
    int n = m_aItem.size();
    str.Format(L"\r\nitem count: %d; current:%d; firstvisible:%d; endvisible:%d",
        n, m_nCurSel, m_nFirstVisible, m_nEndVisible);
    str += L"\r\nitems:";
    tm_setmin(n, 10);
    for (int i = 0; i < n; ++i)
    {
        str += L"\r\n  text:\"";
        str += m_aItem[i].strText();
        str += L"\" hint:\"";
        str += m_aItem[i].strHint;
        str += L"\" acc:\"";
        str += m_aItem[i].strAcc;
        str += L"\"";
    }
    return CForePicWin<CTuiWgt>::GetDebugInfo() + str;
}

#endif // TUI_EDIT_MODE

TM_END_NAMESPACE
