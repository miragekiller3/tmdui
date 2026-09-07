/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2012.02.23
 *
 *  Copyright (C) 2012 miragekiller
 */

#pragma once

#ifndef SCROLL_H 
#define SCROLL_H


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// class CScrollImpl
template <class T>
class TM_NO_VTABLE CScrollImpl
{
protected:
    GXDips   m_nScrollL;
    GXDips   m_nScrollT;
    GXDips   m_nScrollW;
    GXDips   m_nScrollH;

    T* GetT() const { return (T*)static_cast<const T*>(this); }

public:
    CScrollImpl()
        : m_nScrollL (0)
        , m_nScrollT (0)
        , m_nScrollW (0)
        , m_nScrollH (0)
    {
    }

private:
    GXDips ValidScrollTop(GXDips n) const
    {
        if (n < 0)
            return 0;
        GXDips x = GetScrollMaxTop();
        return tm_min (x, n);
    }
    
    GXDips ValidScrollLeft(GXDips n) const
    {
        if (n < 0)
            return 0;
        GXDips x = GetScrollMaxLeft();
        return tm_min (x, n);
    }

public:
    TMBool IsNoThumbTracking() const
    {
        return false;
    }

    GXDips GetViewWidth() const
    {
        T* p = GetT();
        return p->m_rcClient.Width() - p->m_bdPadding.left - p->m_bdPadding.right;
    }
    
    GXDips GetViewHeight() const
    {
        T* p = GetT();
        return p->m_rcClient.Height() - p->m_bdPadding.top - p->m_bdPadding.bottom;
    }

    GXDips GetScrollWidth() const
    {
        return m_nScrollW;
    }

    GXDips GetSafeScrollWidth() const
    {
        GXDips w = GetViewWidth();
        tm_setmax(w, m_nScrollW);
        return w;
    }

//     GXDips ScrollWidthToWndWidth() const
//     {
//         T* p = GetT();
//         return m_nScrollW + p->m_rcWnd.Width() - p->m_rcClient.Width() + p->m_bdPadding.left + p->m_bdPadding.right;
//     }

    GXDips GetScrollHeight() const
    {
        return m_nScrollH;
    }

    GXDips GetScrollLeft() const { return m_nScrollL; }

    GXDips GetScrollLeftByES(int nFalg) const
    {
        GXDips nView = GetViewWidth();
        if (m_nScrollW < nView)
        {
            if (nFalg & ES_CENTER)
                return (m_nScrollW - nView) / 2.0f;
            else if (nFalg & ES_RIGHT)
                return m_nScrollW - nView;
        }
        return m_nScrollL;
    }
    
    GXDips GetScrollTop() const { return m_nScrollT; }
    
    void SetScrollLeft(GXDips n)
    {
        GXDips nNew = ValidScrollLeft(n);
        if (nNew != m_nScrollL && HaveChildAlignScroll(GetT()))
            GetT()->ChangeFrame(false);
        m_nScrollL = nNew;
    }
    
    void SetScrollTop(GXDips n)
    {
        m_nScrollT = ValidScrollTop(n);
    }

    GXDips GetOrgX() const
    {
        T* p = GetT();
        return p->m_rcClient.Left() + p->m_bdPadding.left - GetScrollLeft();
    }

    GXDips GetOrgY() const
    {
        T* p = GetT();
        return p->m_rcClient.Top() + p->m_bdPadding.top - GetScrollTop();
    }
    
    void ResetScroll()
    {
        m_nScrollL = ValidScrollLeft(m_nScrollL);
        m_nScrollT = ValidScrollTop(m_nScrollT);
    }

    void GetProjectRect(TUIRect& rc) const
    {
        DbgAssert(m_nScrollL >= 0);
        DbgAssert(m_nScrollT >= 0);

        CTuiRect rcx;
        GetT()->GetPaddingRect(rcx);
        GXDips w = rcx.Width();
        GXDips h = rcx.Height();
        tm_setmax(w, m_nScrollW);
        tm_setmax(h, m_nScrollH);
        ((CTuiRect*)&rc)->SetLTWH(rcx.Left() - m_nScrollL, rcx.Top() - m_nScrollT, w, h);
//         DbgTrace ("GetProjectRect:%d, %d, %d, %d", rc);
    }

    virtual GXDips GetVScrollLine() const
    {
        return 20;
    }

    virtual GXDips GetHScrollLine() const
    {
        return 20;
    }

    virtual GXDips GetVScrollPage() const
    {
        return GetViewHeight();
    }

    virtual GXDips GetHScrollPage() const
    {
        return GetViewWidth();
    }

    GXDips GetHScrollWheel() const
    {
        return GetHScrollLine() * 3;
    }

    GXDips GetVScrollWheel() const
    {
        GXDips vPage = GetVScrollPage();
        GXDips vLine = GetVScrollLine();
        if (vLine * 3 < vPage)
            return vLine * 3;
        else
        {
            GXDips n = vPage / vLine;
            return n < 2.0f ? vLine : vLine * 2.0f;
        }
    }

    void GetHScrollInfo(TUISCROLLINFO& si)
    {
        si.nView = GetViewWidth();
        si.nProject = m_nScrollW;
        si.nOrg = m_nScrollL;
    }
    void GetVScrollInfo(TUISCROLLINFO& si)
    {
        si.nView = GetViewHeight();
        si.nProject = m_nScrollH;
        si.nOrg = m_nScrollT;
    }

    void AfterScroll()
    {
        // don't call invalidate in this func;
        // because invalidate already called;
//        (static_cast<T*>(this))->Invalidate();
        T* p = GetT();
        p->Invalidate();
//         if (p->_child)
//             LayoutScrollChilds(p);
    }

    TMBool IsVScrollable() const
    {
        T* hWnd = GetT();
        GXDips nView = hWnd->GetViewHeight();
        GXDips nProject = hWnd->GetScrollHeight();
        return CMath::Greater(nProject, nView, hWnd->GetEps());
    }

    TMBool IsHScrollable() const
    {
        T* hWnd = GetT();
        GXDips nView = hWnd->GetViewWidth();
        GXDips nProject = hWnd->GetScrollWidth();
        return CMath::Greater(nProject, nView, hWnd->GetEps());
    }

    TMBool DoWheel(GXDips zDelta)
    {
        T* pT = static_cast<T*>(this);
        GXDips nWheel;
        GXDips nPos;
//        TMUInt32 dwStyle = pT->GetStyle();
        if (pT->HasVScroll()) //  dwStyle & WS_VSCROLL)
        {
            nWheel = zDelta * GetVScrollWheel ();
            nPos = GetScrollTop ();
        }
        else
        {
            nWheel = zDelta * GetHScrollWheel ();
            nPos = GetScrollLeft ();
        }

        nPos -= nWheel;
        
        TMBool bHandle;
        DISABLE_SPY();
        if (pT->HasVScroll()) //  dwStyle & WS_VSCROLL)
//            SetScrollTop (nPos);
            bHandle = CTuiWidget(GetT()).ScrollTo(0, &nPos);
        else
            bHandle = CTuiWidget(GetT()).ScrollTo(&nPos, 0);
//            SetScrollLeft (nPos);
//        pT->Invalidate ();
//        pT->AfterScroll();
        return bHandle;
    }

    GXDips GetScrollMaxLeft() const
    {
        GXDips nView = GetViewWidth();
        GXDips n = GetScrollWidth() - nView;
        return (n < 0) ? 0 : n;
    }

    void DoScrollH(int nReq, GXDips nThumb)
    {
        GXDips nPos = GetScrollLeft();
        GXDips nNew = nPos;
        
        switch (nReq)
        {
        case SB_TOP:        // top or all left
            nNew = 0;
            break;
            
        case SB_BOTTOM:        // bottom or all right
            nNew = GetScrollMaxLeft();
            break;
            
        case SB_LINEUP:        // line up or line left
            nNew -= GetHScrollLine();
            break;
            
        case SB_LINEDOWN:    // line down or line right
            nNew += GetHScrollLine();
            break;
            
        case SB_PAGEUP:        // page up or page left
            nNew -= GetHScrollPage();
            break;
            
        case SB_PAGEDOWN:    // page down or page right
            nNew += GetHScrollPage();
            break;
            
        case SB_THUMBTRACK:
            if (IsNoThumbTracking())
                break;
            tm_fallthrough;
        case SB_THUMBPOSITION:
            nNew = nThumb;
            break;
            
        case SB_ENDSCROLL:
        default:
            return;
        }

        if (nNew != nPos)
        {
            DISABLE_SPY();
            CTuiWidget(GetT()).ScrollTo(&nNew, 0);
//             T* pT = static_cast<T*>(this);
//             SetScrollLeft (nNew);
//             pT->Invalidate ();
//             pT->AfterScroll();
        }
    }

    GXDips GetScrollMaxTop() const
    {
        GXDips nView = GetViewHeight();
        GXDips n = GetScrollHeight() - nView;
        return (n < 0) ? 0 : n;
    }
    
    void DoScrollV(int nReq, GXDips nThumb)
    {
        GXDips nPos = GetScrollTop();
        GXDips nNew = nPos;
        
        switch (nReq)
        {
        case SB_TOP:        // top or all left
            nNew = 0;
            break;
            
        case SB_BOTTOM:        // bottom or all right
            nNew = GetScrollMaxTop();
            break;
            
        case SB_LINEUP:        // line up or line left
            nNew -= GetVScrollLine ();
            break;
            
        case SB_LINEDOWN:    // line down or line right
            nNew += GetVScrollLine ();
            break;
            
        case SB_PAGEUP:        // page up or page left
            nNew -= GetVScrollPage ();
            break;
            
        case SB_PAGEDOWN:    // page down or page right
            nNew += GetVScrollPage ();
            break;
            
        case SB_THUMBTRACK:
            if (IsNoThumbTracking())
                break;
            tm_fallthrough;
        case SB_THUMBPOSITION:
//            DbgTrace("SB_THUMBPOSITION %d", nThumb);
            nNew = nThumb;
            break;
            
        case SB_ENDSCROLL:
        default:
            return;
        }

        if (nNew != nPos)
        {
            DISABLE_SPY();
            CTuiWidget(GetT()).ScrollTo(0, &nNew);

//             T* pT = static_cast<T*>(this);
//             SetScrollTop (nNew);
//             pT->Invalidate ();
//             pT->AfterScroll();
        }
    }

public:
    BEGIN_TUIMSG_MAP    (CScrollImpl)
        MESSAGE_HANDLER (CM_VSCROLL, OnVScroll)
        MESSAGE_HANDLER (CM_HSCROLL, OnHScroll)
        MESSAGE_HANDLER (WM_MOUSEWHEEL, OnMouseWheel)
        MESSAGE_HANDLER (CM_GETSCROLLINFO, OnGetScrollInfo)
        MESSAGE_HANDLER (CM_SCROLLBY, OnScrollBy)
        MESSAGE_HANDLER (CM_SCROLLTO, OnScrollTo)
        MESSAGE_HANDLER (CM_SETSCROLLRANGE, OnSetScrollRange)
    END_MSG_MAP         ()

//     TMResult OnWindowPosChanged (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
//     {
// //        T* pT = static_cast<T*>(this);
// //        DbgAssert(pT->IsWindow());
// 
//         TUIWINDOWPOS* pwp = (TUIWINDOWPOS*) lParam;
//         if (pwp->flags & SWP_FRAMECHANGED)
//         {
//             SetScrollLeft (GetScrollLeft());
//             SetScrollTop (GetScrollTop());
//             AfterScroll ();
// 
//             // after send WM_WINDOWPOSCHANGED, winmgr will auto invalidate,
//             // so needn't call invalidate
//         }
//         bHandled = false;
//         return true;
//     }
    
    TMResult OnVScroll(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
//        T* pT = static_cast<T*>(this);
//        DbgAssert(pT->IsWindow());
        DoScrollV (wParam, lParam);
        return 0;
    }
    
    TMResult OnHScroll(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
//        T* pT = static_cast<T*>(this);
//        DbgAssert(pT->IsWindow());
        DoScrollH (wParam, lParam);
        return 0;
    }

    TMResult OnMouseWheel(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
//        T* pT = static_cast<T*>(this);
//        DbgAssert(pT->IsWindow());
        // need support: ctrl + wheel = zoom in / out
        int zDelta = ((short)HIWORD(wParam));
        if (zDelta == 0)
            return bHandled = false, 0;
        bHandled = DoWheel (zDelta > 0 ? 1 : -1);
        return 0;
    }

//     TMResult OnGestureConfig(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
//     {
//         if (((TUIHITINFO*)lParam)->bNC)
//             return 0;
//         if (GetScrollMaxLeft() || GetScrollMaxTop())
//             return GESTURE_MASK_PAN;
//         return 0;
//     }

//     TMResult OnGestureMove(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
//     {
//         static CTuiPoint ptOld;
//         CTuiPoint ptNew(lParam);
//         if (wParam == GESTURE_FLAG_BEGIN)
//             ptOld = ptNew;
//         else
//         {
//             CTuiWidget wnd(static_cast<T*>(this));
//             wnd.ScrollBy(ptOld.x - ptNew.x, ptOld.y - ptNew.y);
//             ptOld = ptNew;
//         }
//         return 0;
//     }

    TMResult OnGetScrollInfo(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
//        T* pT = static_cast<T*>(this);
//        DbgAssert(pT->IsWindow());

        TUISCROLLINFO* ph = (TUISCROLLINFO*) wParam;
        TUISCROLLINFO* pv = (TUISCROLLINFO*) lParam;
        if (ph)
        {
            ph->nView = GetViewWidth ();
            ph->nProject = GetScrollWidth ();
            ph->nOrg = GetScrollLeft ();
        }
        if (pv)
        {
            pv->nView = GetViewHeight ();
            pv->nProject = GetScrollHeight ();
            pv->nOrg = GetScrollTop ();
        }

        return 0;
    }

    TMResult OnScrollBy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        T* pT = static_cast<T*>(this);
        DbgAssert(pT->IsWindow());
        if (wParam)
        {
            GXDips* p = (GXDips*)wParam;
            *p += GetScrollLeft();
        }
        if (lParam)
        {
            GXDips* p = (GXDips*)lParam;
            *p += GetScrollTop();
        }
        return GetT()->SendMessage(CM_SCROLLTO, wParam, lParam);
    }

    TMResult OnScrollTo(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        T* pT = static_cast<T*>(this);
        DbgAssert(pT->IsWindow());
        GXDips nl = m_nScrollL;
        GXDips nt = m_nScrollT;
        if (wParam)
        {
            GXDips* p = (GXDips*)wParam;
            SetScrollLeft(*p);
            *p = GetScrollLeft();
        }
        if (lParam)
        {
            GXDips* p = (GXDips*)lParam;
            SetScrollTop(*p);
            *p = GetScrollTop();
        }

        TMResult b = (nl != m_nScrollL) || (nt != m_nScrollT);
        if (b)
            pT->AfterScroll();
        return b;
    }

    TMResult OnSetScrollRange(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        TMResult lResult = 0;
        if (wParam && (*(GXDips*)wParam) != m_nScrollW)
        {
            m_nScrollW = *(GXDips*)wParam;
            lResult = true;
        }
        if (lParam && (*(GXDips*)lParam) != m_nScrollH)
        {
            m_nScrollH = *(GXDips*)lParam;
            lResult = true;
        }
        
        if (lResult)
        {
            T* pT = GetT();
//             if (((pT->m_dwLayout & TABLE_MASK) == TABLE_ROW) && pT->_parent)
//                 pT->GetParent()->ChangeFrame(false);
            pT->ChangeFrame(false);
        }
        return lResult;
    }

    TMBool SendScrollRangeMessage(const GXDips* pw, const GXDips* ph)
    {
        T* pT = GetT();
        GXDips eps = pT->GetEps();
        if (pw && CMath::Equal(*pw, GetScrollWidth(), eps))
            pw = 0;
        if (ph && CMath::Equal(*ph, GetScrollHeight(), eps))
            ph = 0;
        if (pw || ph)
        {
            DISABLE_SPY();
            return CTuiWidget(pT).SetScrollRange(pw, ph);
        }
        return false;
    }

    TMBool SendScrollToMessageX(GXDips pl)
    {
        return SendScrollToMessage(&pl, 0);
    }

    TMBool SendScrollToMessageY(GXDips pt)
    {
        return SendScrollToMessage(0, &pt);
    }

    TMBool SendScrollToMessage(GXDips* pl, GXDips* pt)
    {
        T* pT = GetT();
        GXDips eps = pT->GetEps();
        if (pl && CMath::Equal(ValidScrollLeft(*pl), GetScrollLeft(), eps))
            pl = 0;
        if (pt && CMath::Equal(ValidScrollTop(*pt), GetScrollTop(), eps))
            pt = 0;
        if (pl || pt)
        {
            DISABLE_SPY();
            return CTuiWidget(pT).ScrollTo(pl, pt);
        }
        return false;
    }

    void FixScrollOrg()
    {
        GXDips* pl = CMath::Equal(m_nScrollL, ValidScrollLeft(m_nScrollL)) ? 0 : &m_nScrollL;
        GXDips* pt = CMath::Equal(m_nScrollT, ValidScrollTop(m_nScrollT)) ? 0 : &m_nScrollT;
        DISABLE_SPY();
        if (pl || pt)
            CTuiWidget(GetT()).ScrollTo(pl, pt);
    }

};

template <class T, class W>
class TM_NO_VTABLE CScrollObject : public W
{
public:
    T* GetT() { return static_cast<T*>(this); }

    void DoScrollH (TMWParam& nReq, TMLParam& nThumb)
    {
        switch (nReq)
        {
        case SB_LINEUP:        // line up or line left
            nThumb = this->m_nScrollL - GetT()->GetHScrollLine();
            break;
            
        case SB_LINEDOWN:    // line down or line right
            nThumb = this->m_nScrollL + GetT()->GetHScrollLine ();
            break;
            
        case SB_PAGEUP:        // page up or page left
            nThumb = this->m_nScrollL - GetT()->GetHScrollPage ();
            break;
            
        case SB_PAGEDOWN:    // page down or page right
            nThumb = this->m_nScrollL + GetT()->GetHScrollPage ();
            break;

        default:
            return;
        }

        nReq = SB_THUMBPOSITION;
    }

   
    void DoScrollV (int nReq, int nThumb)
    {
        switch (nReq)
        {
        case SB_LINEUP:        // line up or line left
            nThumb = this->m_nScrollT - GetT()->GetVScrollLine();
            break;
            
        case SB_LINEDOWN:    // line down or line right
            nThumb = this->m_nScrollT + GetT()->GetVScrollLine ();
            break;
            
        case SB_PAGEUP:        // page up or page left
            nThumb = this->m_nScrollT - GetT()->GetVScrollPage ();
            break;
            
        case SB_PAGEDOWN:    // page down or page right
            nThumb = this->m_nScrollT + GetT()->GetVScrollPage ();
            break;
            
        default:
            return;
        }
        
        nReq = SB_THUMBPOSITION;
    }

    TMBool ProcessWindowMessage (CTuiWgt* p, TMUInt uMsg,
                                 TMWParam wParam, TMLParam lParam,
                                 TMResult& lResult, TMULong dwMsgMapID = 0)
    {
        if (uMsg == CM_VSCROLL)
            DoScrollV(wParam, lParam);
        else if (uMsg == CM_HSCROLL)
            DoScrollH(wParam, lParam);

        TMBool b = W::ProcessWindowMessage(p, uMsg, wParam, lParam, lResult, dwMsgMapID);
        if (uMsg == CM_SCROLLTO && b)
            GetT()->AfterScroll();
        return b;
    }
};


TM_END_NAMESPACE

#endif // SCROLL_H
