/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.06.29
 *
 *  Copyright (C) 2025 miragekiller
 */

#include "uiheader.h"
#include "view.h"

TM_BGN_NAMESPACE



////////////////////////////////////////////////////////////////////////////////
//CButtonLayout
struct CHeaderLayout : public ITextPicLayout
{
    CTuiHeaderImpl*         wnd;
    CTuiHeaderImpl::ITEM*   itm;
    enum { eMagic = (HDF_RIGHT << 4) | (HDF_CENTER << 2) };

//#   define HF2DF(x) (0x03 & (((x) << 1) | (((x) & 0x02) >> 1)))
//#   define HF2DF(x) (0x03 & (1 << (0x03 & (2 - (x)))))
#   define HF2DF(x) (0x03 & (eMagic >> (((x) & 0x03) << 1)))

    DbgAssertS1(HF2DF(0x04 | HDF_LEFT)   == DT_LEFT);
    DbgAssertS2(HF2DF(0x04 | HDF_RIGHT)  == DT_RIGHT);
    DbgAssertS3(HF2DF(0x04 | HDF_CENTER) == DT_CENTER);
    
    // only for LayoutPT
    virtual TMBool IsAutoWidth() const { return false; }
    virtual TMBool IsAutoHeight() const { return false; }
    virtual GXDips GetMaxWidth() { return SHRT_MAX; }
    
    // for render/LayoutPT
    virtual EAlignMode GetAlignMode() const { return (itm->nFmt & HDF_BITMAP_ON_RIGHT) ? e_amRight : e_amLeft; }
    virtual TMUInt GetTextFormat() const { return DT_VCENTER | HF2DF(itm->nFmt); }
    virtual GXDips GetPicMargin() { return wnd->m_nPicMargin; }
    
    // only for render
    virtual CModule* GetModule() { return wnd->GetModule(); }
    virtual ITuiPic* GetHCForePic()
    {
        return (wnd->IsHighContrast()) ? wnd->GetDftForePic(itm->nFmt) : 0;
    }
    virtual void AniRender(IGXApp* gapp, ITuiPic* pFore, int nFrame, EButStates ebsBk, const CTuiRect& rcFore)
    {
        pFore->Render(nFrame, 0, rcFore, 0, gapp);
    }
};


////////////////////////////////////////////////////////////////////////////////
//CTuiHeaderImpl
#define DIVIDER_WIDTH  10
#define HOT_DIVIDER_WIDTH 2

void CTuiHeaderImpl::FinalConstruct(CTuiObject* pp)
{
    m_nCurSel       = -1;
    m_clrNormal     = CPropColor::eClrDefault;
    m_clrHilight    = CPropColor::eClrDefault;
    m_pArray[0]     = GetPrePic(HEADER_UP);
    m_pArray[1]     = GetPrePic(HEADER_DOWN);
    m_pItem         = GetPrePic(BUTTON);
    CTuiWgt::FinalConstruct(pp);
}

ITuiPic* CTuiHeaderImpl::GetDftForePic(int fmt)
{
    if (fmt & HDF_SORTUP)
        return GetPrePic(HEADER_UP);
    if (fmt & HDF_SORTDOWN)
        return GetPrePic(HEADER_DOWN);
    return 0;
}

ITuiPic* CTuiHeaderImpl::GetForePic(int fmt)
{
    if (fmt & HDF_SORTUP)
        return m_pArray[0];
    if (fmt & HDF_SORTDOWN)
        return m_pArray[1];
    return 0;
}


bool CTuiHeaderImpl::IsItemFixed(const ITEM& item) const
{
    return Is_HDS_NOSIZING() || (item.nFmt & HDF_FIXEDWIDTH);
}

void CTuiHeaderImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    CTuiRect rc;
    GetPaddingRect(&rc);
    TMBool bHighContrast = IsHighContrast();
    
    if (m_pBk)
    {
        if (bHighContrast)
            gapp->FillRect(rc, CAppColor::GetColorButtonBk(0));
        else
        {
            CWinState ws(this);
            ws.RenderBk(gapp, rc);
        }
    }
    
   
    CView* view = GetView();
    TMBool bDown = (view->GetCapture() == this)
        && (view->m_dwState & MK_LBUTTON)
        && !(HITCODE_HEADER(view->m_htOver.dwCode).flags & HHT_ONDIVIDER);
    int nHitID = (view->m_htOver.hWnd == this && !view->m_htOver.bNC)
        ? HITCODE_HEADER(view->m_htOver.dwCode).iItem
        : -1;

    // render
    gapp->Push(eGXPropTextColor);
    CPropColor clrs[] = { m_clrNormal, m_clrHilight, m_clrHilight, GetView()->m_clrDisableText, m_clrHilight };
    CPropColor* clrss[] = { clrs, 0 };
    int uNumItem = m_aItem.size();
    GXDips l = rc.Left();
    for (int i = 0; i < uNumItem; ++i, rc.left = rc.right) //m_aRight.size(); ++i)
    {
        ITEM& itm = m_aItem[i];
        rc.SetWidth(itm.nWidth);
        int nFrame = 0;
        if (nHitID == i)
            nFrame = bDown ? e_bsDown : e_bsHigh;//IsPriInPop() ? e_bsDown : e_bsHigh; // (m_dwPriStyle & MS_PRI_INPOP) ? e_bsDown : e_bsHigh;
        
        // text color
        RenderSetTextColor(gapp, nFrame, clrss, CAppColor::GetColorButtonText);
        
        // button background 
        if (m_pItem)
        {
            ITuiPic* pFore = bHighContrast ? GetPrePic(BUTTON) : m_pItem;
            pFore->Render(nFrame, 0, rc, 0, gapp);
        }
        
        // text
        CTuiRect rcx(rc);
        rcx.InflateRect(-2, -2);
        CTuiRect rcFore;
        if (nFrame == e_bsDown)
            rcx.OffsetRectX(2), rcx.OffsetRectY(2);
        CHeaderLayout hl;
        hl.wnd = this;
        hl.itm = &itm;
//         itm.gxText->Paint(rcx, 0, gapp);
        hl.Render(
            gapp, itm.gxText, GetForePic(itm.nFmt), -1, -1,
            nFrame, e_bssNormal,
            rcx,
            rcx, // in/out
            rcFore); // out

        rc.left = rc.right;
    }

    // drag image
    if (m_texDrag)
    {
        CTuiRect rcDrag, rcDivide;
        int n = GetDragRect(rcDrag);
        GetDividerRect(n, rcDrag, rcDivide);
        gapp->FillRect(rcDivide, ColorFromSys(eColorIndexHighlight));
        m_texDrag->Blt(rcDrag, CRect(0,0,0,0), GX_BLT_DFTSRCWH, 128);
    }

    gapp->Pop();

    if (Is_WS_PRI_DIRTY())
    {
        Reset_WS_PRI_DIRTY();
        GXDips x = rc.Right() - l;
        if (SendScrollRangeMessage(&x, 0))
            Invalidate();
    }
}

bool CTuiHeaderImpl::GetWindowHint(ETTActive ett, CString* str)
{
    CView* view = GetView();
    int sel;
    if (ett == eTTActvieTab)
        sel = m_nCurSel;
    else
    {
        TUIHITINFO& hi = view->GetHitInfo();
        sel = (hi.bNC || hi.hWnd != this) ? -1 : ((HITCODE_HEADER&)hi.dwCode).iItem;
    }
    
    if (sel != -1)
    {
        ITEM& itm = m_aItem[sel];
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


GXPTexture CTuiHeaderImpl::CreateDragImage(int iItem)
{
    _Check(iItem);

    CTuiRect rc;
    GetItemRect(iItem, &rc);
    return PRI::tuiMakeImage(this, &rc);
}

TMResult CTuiHeaderImpl::NotifyEx_Safe(UINT code, TMLParam lItm, TMLParam l1, TMLParam l2)
{
    const void* p = m_aItem.data();
    TMResult ret = CTuiWgt::NotifyEx(code, lItm, l1, l2);
    if (ret)
        return ret;
    return p != m_aItem.data(); // check memory invalid on CM_NOTIFY.
}

LRESULT CTuiHeaderImpl::DeleteItem (int iItem)
{
    if ((iItem < 0) || (iItem >= m_aItem.size()))
        return FALSE;

    m_aItem.erase(m_aItem.begin() + iItem);
    SetDirtyAndInvalidate();

    return TRUE;
}

LRESULT CTuiHeaderImpl::OnGetItem(int nItem, HDITEM* phdi)
{
    if (!phdi)
        return false;
    
    if ((nItem < 0) || (nItem >= m_aItem.size()))
        return false;
    
    const ITEM& itm = m_aItem[nItem];
    TMUInt mask = phdi->mask;
    
    if (mask & HDI_FORMAT)
        phdi->fmt = itm.nFmt;
    if (mask & HDI_WIDTH)
        phdi->cxy = itm.nWidth;
    if (mask & HDI_LPARAM)
        phdi->lParam = itm.lParam;
    if (mask & HDI_TEXT)
        itm.gxText.GetString().CopyTo(phdi->pszText, phdi->cchTextMax);
    return true;
}

LRESULT CTuiHeaderImpl::GetItemRect(int iItem, TUIRect* lpRect)
{
    GetPaddingRect(lpRect);
    int nSize = m_aItem.size();
    for (int i = 0; i < nSize; ++i, lpRect->left = lpRect->right)
    {
        lpRect->right = lpRect->left + m_aItem[i].nWidth;
        if (i == iItem)
            return true;
    }
    return false;
}

int CTuiHeaderImpl::GetDragRect(TUIRect& rcDrag)
{
    CView* view = GetView();
    
    DbgAssert(m_texDrag);
    DbgAssert(view->GetCapture() == this);
    DbgAssert(view->m_htDownL.hWnd == this);
    DbgAssert(!view->m_htDownL.bNC);
    

    GXDips dx = view->m_htOver.pt.x - view->m_htDownL.pt.x;
    HITCODE_HEADER hc = view->m_htDownL.dwCode;
    GetItemRect(hc.iItem, &rcDrag);
    ((CTuiRect&)rcDrag).OffsetRectX(dx);
    return hc.iItem;
}

int CTuiHeaderImpl::GetDividerRect(int nSel, const TUIRect& rcDrag, TUIRect& rc)
{
    CTuiRect rcItem;
    GetPaddingRect(&rcItem);
    int nSize = m_aItem.size();
    int i = 0;
    for (; i < nSize; ++i, rcItem.left = rcItem.right)
    {
        if (i <= nSel && rcDrag.left <= rcItem.left)
            break;

        rcItem.right = rcItem.left + m_aItem[i].nWidth;

        if (i >= nSel && rcDrag.right < rcItem.right)
            break;
    }
    rc = rcItem;
    rc.left -= HOT_DIVIDER_WIDTH / 2;
    rc.right = rc.left + HOT_DIVIDER_WIDTH;
    return i;
}

LRESULT CTuiHeaderImpl::OnHitTest(TUIHITINFO& hti)
{
    HITCODE_HEADER* hc = (HITCODE_HEADER*)&hti.dwCode;
    CTuiPoint& pt = (CTuiPoint&)hti.pt;

    CTuiRect rc;
    GetPaddingRect(&rc);
    hti.bNC = false;
    hti.pTaget = 0;
    hc->iItem = -1;

    if (pt.x < rc.left)
        return (hc->flags = HHT_TOLEFT), true;
    if (pt.x > rc.right)
        return (hc->flags = HHT_TORIGHT), true;
    if (pt.y < rc.top)
        return (hc->flags = HHT_ABOVE), true;
    if (pt.y > rc.bottom)
        return (hc->flags = HHT_BELOW), true;

    GXDips l = rc.left;
    GXDips r = l;
    int nSize = m_aItem.size();
    for (int i = 0; i < nSize; ++i, l = r)
    {
        ITEM& itm = m_aItem[i];
        if (itm.nWidth < 0.001)
            continue;
        r += itm.nWidth;
        if (pt.x >= r + (IsItemFixed(itm) ? 0 : DIVIDER_WIDTH))
            continue;
        hc->iItem = i;
        hc->flags = HHT_ONHEADER;
        if (itm.nWidth <= 2 * DIVIDER_WIDTH && pt.x < r)
            return true;
        if (pt.x < r - DIVIDER_WIDTH)
            return true;
        hc->flags |= HHT_ONDIVIDER;
        return true;
    }

    hc->iItem = -1;
    hc->flags = HHT_NOWHERE;
    return true;
}

void CTuiHeaderImpl::CopyFmt(int nItem, ITEM& itm, int fmt)
{
    if (fmt & (HDF_SORTUP | HDF_SORTDOWN))
    {
        for (int i = 0; i < m_aItem.size(); ++i)
            m_aItem[i].nFmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
    }
    itm.nFmt = fmt;
}

void CTuiHeaderImpl::CopyItem(int nItemCheck, ITEM& itm, const HDITEM* phdi)
{
    TMBool bChange = false;
    TMBool bChangeW = false;
    if ((phdi->mask & HDI_WIDTH) && (itm.nWidth != phdi->cxy))
        (itm.nWidth = phdi->cxy), bChangeW = true;
    if (phdi->mask & HDI_TEXT)
        itm.gxText.ReCreate(phdi->pszText, false), bChange = true;
    if ((phdi->mask & HDI_LPARAM) && (itm.lParam != phdi->lParam))
        (itm.lParam = phdi->lParam), bChange = true;
    if ((phdi->mask & HDI_FORMAT) && (itm.nFmt != phdi->fmt))
    {
        bChange = true;
        if (nItemCheck < 0 || (itm.nFmt & (HDF_SORTUP | HDF_SORTDOWN)) == (phdi->fmt & (HDF_SORTUP | HDF_SORTDOWN)))
            itm.nFmt = phdi->fmt;
        else
            CopyFmt(nItemCheck, itm, phdi->fmt);
    }
    if (bChangeW)
        SetDirtyAndInvalidate();
    else if (bChange)
        Invalidate();
}

void CTuiHeaderImpl::CheckItem(const ITEM& itm, HDITEM *phdi)
{
    if ((phdi->mask & HDI_WIDTH) && (itm.nWidth != phdi->cxy))
        phdi->mask &= ~HDI_WIDTH;
    if ((phdi->mask & HDI_TEXT) && (itm.gxText.GetString() != phdi->pszText))
        phdi->mask &= ~HDI_TEXT;
    if ((phdi->mask & HDI_FORMAT) && (itm.nFmt != phdi->fmt))
        phdi->mask &= ~HDI_FORMAT;
    if ((phdi->mask & HDI_LPARAM) && (itm.lParam != phdi->lParam))
        phdi->mask &= ~HDI_LPARAM;
}

LRESULT CTuiHeaderImpl::InsertItem(int nItem, const HDITEM* phdi)
{
    if ((phdi == NULL) || (nItem < 0) || (phdi->mask == 0))
	    return -1;

    tm_setmin(nItem, m_aItem.size());
    CopyItem(-1, *m_aItem.insert(m_aItem.begin() + nItem, ITEM()), phdi);
    return nItem;
}

LRESULT CTuiHeaderImpl::SetItem(int nItem, const HDITEM* phdi)
{
    if ((phdi == NULL) || (nItem < 0) || nItem >= m_aItem.size() || (phdi->mask == 0))
        return false;
    HDITEM tmp = *phdi;
    ITEM& itm = m_aItem[nItem];
    CheckItem(itm, &tmp);
    if (!tmp.mask || NotifyEx_Safe(HDN_ITEMCHANGING, itm.lParam, nItem, (TMLParam)phdi))
        return false;
    CopyItem(nItem, itm, phdi);
    NotifyEx(HDN_ITEMCHANGED, itm.lParam, nItem, (TMLParam)phdi);
    return true;
}

LRESULT CTuiHeaderImpl::OnSetItemAccHInt(TMWParam wp, TMLParam lp, TMUInt msg)
{
    if (lp == 0 || wp >= m_aItem.size())
        return -1;
    ITEM& itm = m_aItem[wp];
    StringMessageToString(lp, msg == CM_SETITEMACCTEXT ? itm.strAcc : itm.strHint);
    return wp;
}

LRESULT CTuiHeaderImpl::OnGetItemAccHInt(TMWParam wp, TMLParam lp, TMUInt msg)
{
    if (lp == 0 || wp >= m_aItem.size())
        return -1;
    CStrPtr* str = (CStrPtr*)lp;
    ITEM& itm = m_aItem[wp];
    *str = msg == CM_GETITEMACCTEXT ? itm.strAcc : itm.strHint;
    return wp;
}


LRESULT CTuiHeaderImpl::SetBitmapMargin(int _iMargin)
{
    INT oldMargin = m_nPicMargin;
    m_nPicMargin = _iMargin;
    Invalidate();
    return oldMargin;
}

LRESULT CTuiHeaderImpl::OnCreate()
{
    m_nPicMargin = 3 * GetSystemMetrics(SM_CXEDGE);
    return 0;
}

LRESULT CTuiHeaderImpl::OnLButtonDblClk()
{
    HITCODE_HEADER hc = GetView()->m_htDownL.dwCode;
    UINT code;
    if ((Is_HDS_BUTTONS()) && (hc.flags == HHT_ONHEADER))
        code = HDN_ITEMDBLCLICK;
    else if ((hc.flags == HHT_ONDIVIDER) || (hc.flags == HHT_ONDIVOPEN))
        code = HDN_DIVIDERDBLCLICK;
    else
        return 0;
    NotifyEx(code, m_aItem[hc.iItem].lParam, hc.iItem, 0);
    return 0;
}

LRESULT CTuiHeaderImpl::OnLButtonDown(LPARAM lParam)
{
    Reset_HDS_BEGINTRACK_();
    Reset_HDS_HAVEMOVE_();
    SetCapture();
    Invalidate();
    UpdateWindow();
    return 0;
}

LRESULT CTuiHeaderImpl::OnLButtonUp(int x, int y)
{
    CView* view = GetView();
    if (m_texDrag)
    {
        CTuiRect rcDrag, rcDivide;
        int nSel = GetDragRect(rcDrag);
        int nDivider = GetDividerRect(nSel, rcDrag, rcDivide);
        ITEM itm = m_aItem[nSel];
        if (nDivider != nSel && nDivider != nSel + 1 && !NotifyEx_Safe(HDN_ENDDRAG, itm.lParam, nSel, nDivider))
        {
            m_aItem.erase(m_aItem.begin() + nSel);
            if (nDivider > nSel)
                --nDivider;
            m_aItem.insert(m_aItem.begin() + nDivider, itm);
        }
    }
    else if (view->GetCapture() == this && Is_HDS_BEGINTRACK_())
    {
        int iItem = HITCODE_HEADER(view->m_htDownL.dwCode).iItem;
        _Check(iItem);
        ITEM itm = m_aItem[iItem];
        NotifyEx(HDN_ENDTRACK, itm.lParam, iItem, itm.nWidth);
    }
    else if (!Is_HDS_HAVEMOVE_())
    {
        int iItem = HITCODE_HEADER(view->m_htDownL.dwCode).iItem;
        ITEM& itm = m_aItem[iItem];
        int nFmt = itm.nFmt;
        if (nFmt & (HDF_AUTOSORTUP | HDF_AUTOSORTDOWN))
        {
            if (nFmt & HDF_SORTUP)
            {
                nFmt &= ~HDF_SORTUP;
                if (nFmt & HDF_AUTOSORTDOWN)
                    nFmt |= HDF_SORTDOWN;
            }
            else if (nFmt & HDF_SORTDOWN)
                nFmt &= ~HDF_SORTDOWN;
            else
            {
                if (nFmt & HDF_AUTOSORTUP)
                    nFmt |= HDF_SORTUP;
                else
                {
                    DbgAssert(nFmt & HDF_AUTOSORTDOWN);
                    nFmt |= HDF_SORTDOWN;
                }
            }
            CopyFmt(iItem, itm, nFmt);
        }
        else
            nFmt = 0;
        NotifyEx(HDN_ITEMCLICK, itm.lParam, iItem, nFmt);
    }

    ReleaseCapture();

    return 0;
}

void CTuiHeaderImpl::OnCaptureChanged()
{
    m_texDrag.Release();
    Invalidate();
}

LRESULT CTuiHeaderImpl::OnMouseMove(LPARAM lParam)
{
    CView* view = GetView();
    Set_HDS_HAVEMOVE_();
    if (view->GetCapture() != this)
        return 0;
    TUIHITINFO& hti = view->m_htDownL;
    if (hti.bNC || hti.hWnd != this || !(view->m_dwState & MK_LBUTTON))
        return 0;
    HITCODE_HEADER hc = hti.dwCode;
    if (!(hc.flags & HHT_ONHEADER))
        return 0;

    _Check(hc.iItem);
    if (hc.flags & HHT_ONDIVIDER)
    {
        ITEM& itm = m_aItem[hc.iItem];
        if (!Is_HDS_BEGINTRACK_())
        {
            if (NotifyEx_Safe(HDN_BEGINTRACK, itm.lParam, hc.iItem, itm.nWidth))
                return ReleaseCapture(), 0;
            else
                Set_HDS_BEGINTRACK_();
        }
        else
        {
            if (NotifyEx_Safe(HDN_TRACK, itm.lParam, hc.iItem, itm.nWidth))
                return ReleaseCapture(), 0;
        }
        CTuiPoint pt(lParam);
        GXDips dx = pt.X() - view->m_htPrev.pt.x;
        GXDips w = itm.nWidth + dx;
        if (w < DIVIDER_WIDTH)
            w = DIVIDER_WIDTH;
        CString str(itm.gxText.GetString());
        HDITEM hi = { HDI_WIDTH, w, (TMStr)str.c_str(), 0, 0, itm.nFmt, itm.lParam };
        if (!NotifyEx_Safe(HDN_ITEMCHANGING, itm.lParam, hc.iItem, (TMLParam)&hi))
        {
            itm.nWidth = w;
            hi.cxy = w;
            NotifyEx(HDN_ITEMCHANGED, itm.lParam, hc.iItem, (TMLParam)&hi);
            SetDirtyAndInvalidate();
        }
    }
    else if (Is_HDS_DRAGDROP())
    {
        if (!m_texDrag)
        {
            CTuiPoint pt(lParam);
            if (CView::IsDragDistance(pt, hti.pt))
            {
                if (NotifyEx_Safe(HDN_BEGINDRAG, m_aItem[hc.iItem].lParam, hc.iItem, 0))
                    ReleaseCapture();
                else
                    m_texDrag.Attach(CreateDragImage(hc.iItem));
            }
        }
        Invalidate();
    }

    return 0;
}

void CTuiHeaderImpl::OnMouseSwitch(const TUIHITINFO& hti)
{
    if (hti.hWnd != this || hti.bNC)
        return;

    // hint
    if (g_mgrTooltip.IsTooltipTarget(this))
        g_mgrTooltip.SetToolTip(this, eTTActiveNormal);

    // cursor
    HITCODE_HEADER code = hti.dwCode;
    if (code.flags & HHT_ONDIVIDER)
        OSSetCursor(CCursorAT::cursor_SPLITV());
    else
        OSSetCursor(CCursorAT::cursor_ARROW());

    Invalidate();
}

TMBool CTuiHeaderImpl::ProcessWindowMessage(
    CTuiWgt* hWnd, TMUInt uMsg,
    TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    switch (uMsg)
    {
	case HDM_DELETEITEM:
	    return (lResult = DeleteItem((INT)wParam)), true;

	case HDM_GETBITMAPMARGIN:
	    return (lResult = m_nPicMargin), true;

    case HDM_GETITEMW:
        return (lResult = OnGetItem((int)wParam, (HDITEM*)lParam)), true;

	case HDM_GETITEMCOUNT:
	    return (lResult = GetItemCount()), true;

	case HDM_GETITEMRECT:
	    return (lResult = GetItemRect((int)wParam, (TUIRect*)lParam)), true;

    case CM_HITTEST:
        CTuiWgt::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);
        if (!((TUIHITINFO*)lParam)->bNC)
            OnHitTest(*(TUIHITINFO*)lParam);
        return true;

	case HDM_INSERTITEM:
	    return (lResult = InsertItem((INT)wParam, (LPHDITEM)lParam)), true;

	case HDM_SETBITMAPMARGIN:
	    return (lResult = SetBitmapMargin((INT)wParam)), true;

 	case HDM_SETITEM:
 	    return (lResult = SetItem((INT)wParam, (LPHDITEM)lParam)), true;

    case WM_CREATE:
        OnCreate();
        break;

    case WM_LBUTTONDBLCLK:
        return (lResult = OnLButtonDblClk()), true;

    case WM_LBUTTONDOWN:
        return (lResult = OnLButtonDown(lParam)), true;

    case WM_LBUTTONUP:
        return (lResult = OnLButtonUp((SHORT)LOWORD(lParam), (SHORT)HIWORD(lParam))), true;

    case WM_MOUSEMOVE:
        return (lResult = OnMouseMove(lParam)), true;

    case CM_MOUSESWITCH:
        OnMouseSwitch(*(TUIHITINFO*)lParam);
        break;

    case WM_CAPTURECHANGED:
        OnCaptureChanged();
        break;

    case WM_SETCURSOR:
        return true;

    case CM_SETITEMACCTEXT:
    case CM_SETITEMHINT:
        lResult = OnSetItemAccHInt(wParam, lParam, uMsg);
        return true;
        
    case CM_GETITEMACCTEXT:
    case CM_GETITEMHINT:
        lResult = OnGetItemAccHInt(wParam, lParam, uMsg);
        return true;

    default:
        break;
    }

    CHAIN_MSG_MAP(CTuiWgt);

    return 0;
}

TM_REGISTER_CLASS1(CTuiHeader);

TM_END_NAMESPACE

