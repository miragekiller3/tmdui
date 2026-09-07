/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.20
 *
 *  Copyright (C) 2017 miragekiller
 */

#include "uibutton.h"
#include "pic.h"
#include "uiwindow.h"
#include "view.h"
#include "menupop.h"



TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
//CButtonLayout
struct CButtonLayout : public ITextPicLayout
{
    CTuiButtonImpl* but;

    // only for LayoutPT
    virtual TMBool IsAutoWidth() const { return but->IsAutoWidth(); }
    virtual TMBool IsAutoHeight() const { return but->IsAutoHeight(); }
    virtual GXDips GetMaxWidth() { return (but->m_nMaxWidth >= 0) ? but->m_nMaxWidth - but->GetViewLeft() - but->GetViewRight() : SHRT_MAX; }
    
    // for render/LayoutPT
    virtual EAlignMode GetAlignMode() const { return but->m_eMode; }
    virtual TMUInt GetTextFormat() const { return but->m_uFormat; }
    virtual GXDips GetPicMargin() { return but->m_nPicMargin; }
    
    // only for render
    virtual CModule* GetModule() { return but->GetModule(); }
    virtual ITuiPic* GetHCForePic()
    {
        return ((but->m_dwStyle & BS_WINDOWFOREONHC) && but->IsHighContrast()) ? but->GetDftForePic() : 0;
    }
    virtual void AniRender(IGXApp* gapp, ITuiPic* pFore, int nFrame, EButStates ebsBk, const CTuiRect& rcFore)
    {
        but->m_aniList.AniRender(gapp, 0, pFore, nFrame, ebsBk, rcFore);
    }
};


////////////////////////////////////////////////////////////////////////////////
//CTuiButtonImpl
CTuiButtonImpl::CTuiButtonImpl (int nNumStates, int nDownOffset)
    : m_nNumChecks (nNumStates)
    , m_nCurChecks (0)
//     , m_dwState (0)
    , m_nDownOffsetX (nDownOffset)
    , m_nDownOffsetY (nDownOffset)
{
    m_bdPadding.left = m_bdPadding.right = m_bdPadding.bottom = m_bdPadding.top = 4;
    m_dwStyle |= (WS_TABSTOP | BS_AUTOSTATE);
    m_uFormat |= (DT_VCENTER | DT_SINGLELINE);
    m_clr[e_bsNormal]   = TMRGB(0,0,0);
    m_clr[e_bsHigh]     = TMRGB(0,0,0);
    m_clr[e_bsDown]     = TMRGB(0,0,0);
//    m_clr[e_bsDisable]  = TMRGB(128, 128, 128);
    m_clr[e_bsFocus]    = TMRGB(0,0,0);
}

CTuiButtonImpl::~CTuiButtonImpl()
{
}

// CTuiWgt
// EButState CTuiButtonImpl::DoGetState(HTUI hFocus, HTUI hOver, TMBool bMouseDown)
// {
//     EButState state = CTuiWgt::DoGetState(hFocus, hOver, bMouseDown);
//     if (m_dwPriStyle & BS_PRI_KEYDOWN)
//         return e_bsDown | e_bstDown | (state & e_bstMask);
//     return state;
// }

EButStates CTuiButtonImpl::DoGetStates(HTUI hFocus, HTUI hOver, TMBool bMouseDown)
{
    EButStates state = CTuiWgt::DoGetStates(hFocus, hOver, bMouseDown);
    state |= (EButStates)m_nCurChecks;
    if (m_dwPriStyle & BS_PRI_KEYDOWN)
        return e_bssDown | state;
    if (state & e_bssFocus)
        state |= e_bssDefault;
    return state;
}


TMResult CTuiButtonImpl::OnLButtonDown(TMUInt uMsg,TMWParam wParam,TMLParam lParam,TMBool& bHandled)
{
    if (!(m_dwStyle & BS_NOCLICKFOCUS))
        SetFocus(eFocusReasonMouse);
    DISABLE_SPY();
//     CTuiWidget(this).SetState(m_dwState | e_bstDown);
    Invalidate();
    if (IsClickOnPress())
        AutoCheck();
    UpdateWindow();
    if (m_dwStyle & BS_NOTIFY)
        Notify(uMsg == WM_LBUTTONDOWN ? BN_PUSHED : BN_DBLCLK);
    if (IsClickOnPress())
        Notify(BN_CLICKED);
    return 0;
}

void CTuiButtonImpl::AutoCheck()
{
    if ((m_nNumChecks > 0) && (m_dwStyle & BS_AUTOSTATE))
    {
        int nCurChecks = CheckChecks(m_nCurChecks + 1);
        SendCheckMessage(nCurChecks);
    }
}

void CTuiButtonImpl::SendCheckMessage(int nCurChecks)
{
    if (nCurChecks != m_nCurChecks)
        SendMessage(CBM_SETCHECK, nCurChecks, 0);
}

void CTuiButtonImpl::AutoCheckAndNotify()
{
    AutoCheck();
    UpdateWindow();
    Notify(BN_CLICKED);
}

void CTuiButtonImpl::OffsetDownRect(EButStates ebs, CTuiRect& rc)
{
    if (!(e_bssDown & ebs))
        return;

    CTuiZoom zoom(GetDPI());
    int x, y;
    if (GetView()->IsHighContrast())
        x = y = 1;
    else
        x = m_nDownOffsetX, y = m_nDownOffsetY;
    rc.OffsetRect(x, y);
}

TMResult CTuiButtonImpl::OnLButtonUp(TMUInt uMsg,TMWParam wParam,TMLParam lParam,TMBool& bHandled)
{
    DISABLE_SPY();
//     CTuiWidget(this).SetState(m_dwState & ~e_bstDown);
    Invalidate();
    TMBool bValide = (GetView()->GetDown() == this);
    if (bValide && !IsClickOnPress())
        AutoCheckAndNotify();
    return 0;
}

void CTuiButtonImpl::SetButTextColor(IGXApp* gapp, EButState ebs, CAppColor::PFNGetColor pfnText, int nFrame)
{
    CPropColor clr1[] = { CPropColor::eClrDefault, CPropColor::eClrDefault, CPropColor::eClrDefault, GetView()->m_clrDisableText, CPropColor::eClrDefault };
    CPropColor* clrss[] = { m_clr, clr1, 0 };
    RenderSetTextColor(gapp, ebs, clrss, pfnText, nFrame);
}

void CTuiButtonImpl::PaintBk(IGXApp* gapp, int nFrame, EButStates states)
{
    if (!m_pBk)
        return;

    if (GetView()->IsHighContrast())
    {
        ITuiPic* p = GetDftPic();
        if (p)
            p->Render(nFrame, states, m_rcClient, gapp);
    }
    else
        m_aniList.AniRender(gapp, 0, m_pBk, nFrame, states, m_rcClient);
}

TMResult CTuiButtonImpl::Paint(IGXApp* gapp, TMLParam lParam, CAppColor::PFNGetColor pfnText)
{
    CTuiRect rc;
    GetPaddingRect(&rc);
    CButtonLayout bl;
    bl.but = this;

    if (Is_WS_PRI_DIRTY())
    {
        Reset_WS_PRI_DIRTY(); // m_dwPriStyle &= ~WS_PRI_DIRTY;

        GXDips w, h;
        bl.LayoutPT(gapp, m_gxText, m_pFore, m_nPicWidth, m_nPicHeight, rc, w, h);
        //DbgTrace("layout(%d): %g, %g", GetObjectID(), w, h);
        SendScrollRangeMessage(&w, &h);
    }

    EButStates ebsAll = GetStates();
    EButStates ebsBk = (EButStates)(ebsAll & tm_not(e_bssHideFocus));
    EButState ebs = ButStates2ButFrame(ebsBk);// ebs;//((ebs == e_bsFocus) && IsHideFocus()) ? e_bsNormal : ebs;
    int nFrame = ebs + m_nCurChecks * e_bsEnd;
    SetButTextColor(gapp, ebs, pfnText, nFrame);

    // render bk;
    PaintBk(gapp, nFrame, ebsBk);

    // get fore pic & text rect
    OffsetDownRect(ebsAll, rc);
    CTuiRect rcc(rc);
    gxTextCalcAlignmentRectEllipsis(rc, m_nScrollW, m_nScrollH, m_uFormat);
    
    // render fore pic & txt
    CTuiRect rcFore;
    bl.Render(gapp, m_gxText, m_pFore, m_nPicWidth, m_nPicHeight, nFrame, ebsBk, rcc, rc, rcFore);

    /*
    // render fore pic
    CTuiRect rcFore(rc);
    TMUInt fmt = m_uFormat;
    if (m_pFore)
    {
        TMUInt32 uFmt = 0;
        EAlignMode eMode = m_eMode;
        if ((eMode & e_amL2RReading) && GetModule()->GetLocal().IsRTL())
        {
            switch (eMode & e_amMaskLR)
            {
            case e_amLeft: eMode = (EAlignMode)((eMode & ~e_amMaskLR) | e_amRight); break;
            case e_amRight: eMode = (EAlignMode)((eMode & ~e_amMaskLR) | e_amLeft); break;
            }
        }
        switch (eMode & e_amMaskAlignMode)
        {
        case e_amTop: uFmt = DT_TOP | DT_CENTER; break;
        case e_amBottom: uFmt = DT_BOTTOM | DT_CENTER; break;
        case e_amRight: uFmt = DT_RIGHT | DT_VCENTER; break;
        case e_amLeftTop: case e_amTopLeft: uFmt = DT_LEFT | DT_TOP; break;
        case e_amRightTop: case e_amTopRight: uFmt = DT_RIGHT | DT_TOP; break;
        case e_amLeftBottom: case e_amBottomLeft: uFmt = DT_LEFT | DT_BOTTOM; break;
        case e_amRightBottom: case e_amBottomRight: uFmt = DT_RIGHT | DT_BOTTOM; break;
        default: uFmt = DT_LEFT | DT_VCENTER; break;
        }

        gxTextCalcAlignmentRect(rcFore, m_nPicWidth, m_nPicHeight, uFmt);

        if (eMode & e_amFrame)
        {
            switch (eMode & e_amMaskAlign)
            {
            case e_amTop: rcFore.KeepHSetT(rcc.Top()); rc.KeepHSetB(rcc.Bottom()); break;
            case e_amBottom: rcFore.KeepHSetB(rcc.Bottom()); rc.KeepHSetT(rcc.Top()); break;
            case e_amRight: rcFore.KeepWSetR(rcc.Right()); rc.SetLeftRight(rcc.Left(), rcFore.Left() - m_nPicMargin); fmt &= ~(DT_RIGHT | DT_CENTER); fmt |= DT_LEFT; break;
            default: rcFore.KeepWSetL(rcc.Left()); rc.SetLeftRight(rcFore.Right() + m_nPicMargin, rcc.Right()); fmt &= ~(DT_LEFT | DT_CENTER); fmt |= DT_RIGHT; break;
            }
        }
        else
        {
            switch (eMode & e_amMaskAlign)
            {
            case e_amTop: rc.SetTop(rcFore.Bottom() + m_nPicMargin); break;
            case e_amBottom: rc.SetBottom(rcFore.Top() - m_nPicMargin); break;
            case e_amRight: rc.SetRight(rcFore.Left() - m_nPicMargin); break;
            default: rc.SetLeft(rcFore.Right() + m_nPicMargin); break;
            }
        }
        
        ITuiPic* pFore;
        if ((m_dwStyle & BS_WINDOWFOREONHC) && IsHighContrast() && (pFore = GetDftForePic()))
        { ; }
        else
            pFore = m_pFore;
        m_aniList.AniRender(gapp, 0, pFore, nFrame, ebsBk, rcFore);
    }

    // render text
    if (m_gxText)
    {
//         m_gxText->SetFormat(fmt);
        m_gxText->Paint(rc, 0, gapp);
        rc.InflateRect(1, 1); // for draw focus rect
    }
*/
    // render focus rect;
    if ((m_dwStyle & BS_DRAWFOCURECT) && ((ebsAll & (e_bssFocus | e_bssHideFocus)) == e_bssFocus))// IsFocus() && !IsHideFocus())
    {
        if (GetDftPic() == &GetModule()->m_aPrePic.m_picDft_BUTTON) // push button
            GetWindowRect(&rc);// GetPaddingRect(&rc);
        else if (rc.IsRectEmpty())
            rc = rcFore;
        if (!rc.IsRectEmpty())
        {
            ITuiPic* pFocus = IsHighContrast() ? &GetModule()->m_aPrePic.m_picDftFocus : (ITuiPic*)GetModule()->m_ptrFocus;
            if (pFocus)
                pFocus->Render(nFrame, ebsAll, rc, gapp);
        }
    }

    return 0;
}

TMResult CTuiButtonImpl::OnSetCheck (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (m_nCurChecks != wParam)
    {
        TMInt nOld = m_nCurChecks;
        m_nCurChecks = wParam;
        Invalidate();
        SendMessage(CBM_STATECHANGED, nOld, m_nCurChecks);
    }
    return true;
}

TMResult CTuiButtonImpl::OnKeyDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (wParam == VK_SPACE || wParam == VK_EXECUTE || wParam == VK_RETURN)
    {
        SetPriKeyDown();
        Invalidate();
        if (wParam == VK_EXECUTE || wParam == VK_RETURN)
        {
            m_dwPriStyle &= (~BS_PRI_KEYDOWN);
            AutoCheckAndNotify();
        }
        return true;
    }
//     else if (g_input.m_key.IsKey(VK_SPACE))
//         return true;

    return bHandled = false;
}

TMResult CTuiButtonImpl::OnKeyUp(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (wParam == VK_SPACE /*|| wParam == VK_EXECUTE || wParam == VK_RETURN*/)
    {
        Invalidate();
        if (m_dwPriStyle & BS_PRI_KEYDOWN)
        {
            m_dwPriStyle &= (~BS_PRI_KEYDOWN);
            AutoCheckAndNotify();
        }
        return true;
    }
    else
    {
        m_dwPriStyle &= (~BS_PRI_KEYDOWN);
        Invalidate();
        return true;
    }

    return false;
}

TMResult CTuiButtonImpl::OnSetTextColor (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    switch (wParam)
    {
    case e_bsNormal:    m_clr[e_bsNormal] = lParam; break;
    case e_bsHigh:      m_clr[e_bsHigh] = (lParam); break;
    case e_bsDisable:   m_clr[e_bsDisable] = (lParam); break;
    case e_bsFocus:     m_clr[e_bsFocus] = (lParam); break;
    case e_bsDown:      m_clr[e_bsDown] = (lParam); break;
    default:
        return 0;
    }

    Invalidate();
    return 0;
}

// TMResult CTuiButtonImpl::OnGetButtonState(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     TM::EButState ebs = (m_dwPriStyle & BS_PRI_KEYDOWN) ? e_bsDown : GetButtonState();
//     int nFrame = ebs + m_nCurChecks * e_bsEnd;
//     return (TMResult)nFrame;
// }

TMResult CTuiButtonImpl::OnMouseEntry(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DISABLE_SPY();
//     CTuiWidget(this).SetState(m_dwState | e_bstHigh);
    Invalidate();
    m_aniList.AddButtonAni(*(TUIHITINFO*)lParam, (TUIHITINFO*)wParam, (TUIHITINFO*)lParam, IsPriKeyDown(), m_nCurChecks);
    return 0;
}

TMResult CTuiButtonImpl::OnMouseLeave(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DISABLE_SPY();
//     CTuiWidget(this).SetState(m_dwState & ~(e_bstHigh | e_bstDown));
    Invalidate();
    m_aniList.AddButtonAni(*(TUIHITINFO*)wParam, (TUIHITINFO*)wParam, (TUIHITINFO*)lParam, IsPriKeyDown(), m_nCurChecks);
    return 0;
}

TMResult CTuiButtonImpl::OnSetFocus(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DISABLE_SPY();
    Invalidate();
    HTUI hDft = GetOwner()->GetDefaultItem();
    if (hDft)
        hDft->Invalidate();
//         CTuiWidget(this).SetState(m_dwState | e_bstFocus);
    if (m_dwStyle & BS_NOTIFY)
        Notify(BN_SETFOCUS);
    return 0;
}

TMResult CTuiButtonImpl::OnKillFocus(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DISABLE_SPY();
    Invalidate();
    HTUI hDft = GetOwner()->GetDefaultItem();
    if (hDft)
        hDft->Invalidate();
//         CTuiWidget(this).SetState(m_dwState & ~e_bstFocus);
    if (m_dwStyle & BS_NOTIFY)
        Notify(BN_KILLFOCUS);
    return 0;
}


#ifdef TUI_EDIT_MODE

CString CTuiButtonImpl::GetDebugInfo()
{
    CString str;
    str.Format(L"\r\n\r\nm_nCurChecks:%d, state:%d;", m_nCurChecks, GetButtonState());
    return PhotoText::GetDebugInfo() + str;
}

#endif // TUI_EDIT_MODE

////////////////////////////////////////////////////////////////////////////////
//CTuiPushButtonImpl
CTuiPushButtonImpl::CTuiPushButtonImpl()
    : CTuiButtonImpl(0, 1)
{
    m_uFormat |= DT_CENTER;
//     m_pBk = GetDftPic(); // .GetPre (IDP_PRE_PIC_BUTTON);
}

CTuiPushButtonImpl::~CTuiPushButtonImpl()
{
}

// EButState CTuiPushButtonImpl::DoGetState(HTUI hFocus, HTUI hOver, TMBool bMouseDown)
// {
//     EButState state = CTuiButtonImpl::DoGetState(hFocus, hOver, bMouseDown);
//     if (state & e_bstFocus)
//         state |= e_bstDefault;
//     return state;
// }

EButStates CTuiPushButtonImpl::DoGetStates(HTUI hFocus, HTUI hOver, TMBool bMouseDown)
{
    EButStates state = CTuiButtonImpl::DoGetStates(hFocus, hOver, bMouseDown);
    if (state & e_bssFocus)
        state |= e_bssDefault;
    return state;
}

TM_REGISTER_CLASS1 (CTuiPushButton);



//////////////////////////////////////////////////////////////////////////////////////
// class CTuiCheckBoxImpl
CTuiCheckBoxImpl::CTuiCheckBoxImpl ()
    : CTuiButtonImpl(2, 0)
{
    m_dwStyle |= BS_DRAWFOCURECT;
//     m_pFore.GetPre(IDP_PRE_PIC_CHECK);
}

TM_REGISTER_CLASS1 (CTuiCheckBox);



//////////////////////////////////////////////////////////////////////////////////////
// class CTuiRadioButtonImpl
CTuiRadioButtonImpl::CTuiRadioButtonImpl()
    : CTuiButtonImpl(2, 0)
{
    Set_WS_PRI_IS_RADIO();
    m_dwStyle |= BS_DRAWFOCURECT;
//     m_pFore.GetPre (uPicFore);
}

HTUI CTuiRadioButtonImpl::GetChecked ()
{
    CItem itm (this);
    do
    {
        if (itm->IsRadio())
        {
            if ((static_cast<CTuiRadioButtonImpl*>(itm.m_pItem))->m_nCurChecks)
                return *itm; 
        }
        itm.CycleNext();
    }
    while (*itm != this);

    return 0;
}

HTUI CTuiRadioButtonImpl::GetNextVisible()
{
    CItem itm (this);
    for (; itm.CycleNext(), *itm != this;)
    {
        if (itm->IsRadio() && itm->CanInput())
            return *itm;
    }
    return this;
}

HTUI CTuiRadioButtonImpl::GetPrevVisible()
{
    CItem itm (this);
    for (; itm.CyclePrev(), *itm != this;)
    {
        if (itm->IsRadio() && itm->CanInput())
            return *itm;
    }
    return this;
}

static void _Radio_ProcessKeyDown (HTUI hButton)
{
    DISABLE_SPY();
    CTuiWidget(hButton).SetFocus(eFocusReasonTab);
    DISABLE_SPY();
    if (CTuiRadioButton(hButton).GetCheck() == 0) //BST_UNCHECKED)
    {
        DISABLE_SPY();
        CTuiRadioButton(hButton).SetCheck(1); //BST_CHECKED);
        hButton->Notify(BN_CLICKED);
    }
}

TMResult CTuiRadioButtonImpl::OnKeyDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    wParam = GetModule()->GetLocal().ConvertKey(wParam);
    if (wParam == VK_UP || wParam == VK_LEFT)
        _Radio_ProcessKeyDown(GetPrevVisible());
    else if (wParam == VK_DOWN || wParam == VK_RIGHT)
        _Radio_ProcessKeyDown(GetNextVisible());
    else if (wParam == VK_SPACE || wParam == VK_EXECUTE || wParam == VK_RETURN)
        _Radio_ProcessKeyDown(this);
    else
        return bHandled = false;

    return true;
}

// TMResult CTuiRadioButtonImpl::OnLButtonUp (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     if (m_nCurChecks)
//     {
//         GetView()->ReleaseCapture ();
//         RedrawWindow ();
//     }
//     else
//         bHandled = false;
//     return 0;
// }

// static void _Radio_CheckAutoTabStop(HTUI hButton)
// {
//     if (hButton->GetStyle() & RS_AUTOTABSTOP)
//     {
//         CTuiWgt::CItem itm (hButton);
//         for (; itm.CycleNext(), *itm != hButton;)
//         {
//             if (itm->IsRadio())
//             {
//                 if (itm->GetStyle() & RS_AUTOTABSTOP)
//                     itm->Set_WS_PRI_NOTABSTOP();
//             }
//         }
//         hButton->Reset_WS_PRI_NOTABSTOP();
//     }
// }


TMResult CTuiRadioButtonImpl::OnSetFocus (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    CTuiButtonImpl::OnSetFocus(uMsg, wParam, lParam, bHandled);
//     _Radio_CheckAutoTabStop(this);
    return 0;
}

// CCycleNext
struct CCycleNext : public Ref
{
    CCycleNext(HTUI hWgt, HTUI hStop)
        : Ref(hWgt->GetSelf().GetCycleNext(hStop))
    {}
    
    CCycleNext(HTUI hWgt)
        : Ref(hWgt)
    {}
    
    void Next(HTUI hStop)
    {
        DbgAssert(p);
        this->Assign(p->GetSelf().GetCycleNext(hStop));
    }
};

TMResult CTuiRadioButtonImpl::OnSetCheck (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (m_nCurChecks != wParam)
    {
        TMInt nOld = m_nCurChecks;
        m_nCurChecks = wParam;

        if (m_nCurChecks)
        {
            TMUInt32 nType = (m_dwPriStyle & WS_PRI_TYPE_MASK);
            for (CCycleNext ref(this, this); ref; ref.Next(this))
            {
                if ((ref->m_dwPriStyle & WS_PRI_TYPE_MASK) == nType)
                    (static_cast<CTuiButtonImpl*>(ref.p))->SendCheckMessage(0);
            }
        }

//        _Radio_CheckAutoTabStop(this);
//        SetFocus(eFocusReasonUnknown, false);
        Invalidate();
        SendMessage(CBM_STATECHANGED, nOld, m_nCurChecks);
    }
    return 0;
}

TM_REGISTER_CLASS1 (CTuiRadioButton);



////////////////////////////////////////////////////////////////////////////////
// class CTuiTabButtonImpl
CTuiTabButtonImpl::CTuiTabButtonImpl ()
    : CTuiRadioButtonImpl()
    , m_dwTargetID (0)
{
    m_dwStyle |= BS_PRESSCLICK;
    m_uFormat |= DT_CENTER;
}

static void _CTuiTabButtonImpl_SetHotKey(CTuiTabButtonImpl* hWnd, int nCheck)
{
    g_input.RemoveHotkey(CPropHotKey(VK_TAB, MOD_CONTROL), hWnd);
    g_input.RemoveHotkey(CPropHotKey(VK_PRIOR, MOD_CONTROL), hWnd);
    g_input.RemoveHotkey(CPropHotKey(VK_NEXT, MOD_CONTROL), hWnd);
    
    if (nCheck && !hWnd->IsNoTabCtrl())
    {
        g_input.AddHotkey(CPropHotKey(VK_TAB, MOD_CONTROL), hWnd, hWnd->eCommandNext);
        g_input.AddHotkey(CPropHotKey(VK_PRIOR, MOD_CONTROL), hWnd, hWnd->eCommandPrev);
        g_input.AddHotkey(CPropHotKey(VK_NEXT, MOD_CONTROL), hWnd, hWnd->eCommandNext);
    }
}

void CTuiTabButtonImpl::SetCheckImpl(int nCheck)
{
    _CTuiTabButtonImpl_SetHotKey(this, nCheck);
    if (nCheck)
        Set_WS_PRI_IS_CURRENTTAB();
    else
        Set_WS_PRI_IS_RADIO();

    HTUI pWin = m_dwTargetID ? SearchNearestItemInDlg(m_dwTargetID) : 0; // GetTabTarget(m_dwTargetID);
    DISABLE_SPY();
    if (pWin)
        CTuiWidget(pWin).ShowWindow(nCheck ? SW_SHOW : SW_HIDE);
}

TMResult CTuiTabButtonImpl::OnCreate (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    SetCheckImpl(m_nCurChecks);
    bHandled = false;
    return 0;
}

TMResult CTuiTabButtonImpl::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    _CTuiTabButtonImpl_SetHotKey(this, 0);
    bHandled = false;
    return 0;
}

TMResult CTuiTabButtonImpl::OnSetFocus(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    _CTuiTabButtonImpl_SetHotKey(this, m_nCurChecks);
    return 0;
}

TMResult CTuiTabButtonImpl::OnSetCheck (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (!m_nCurChecks != !wParam)
        SetCheckImpl(wParam);

    bHandled = false;
    return 0;
}

TMResult CTuiTabButtonImpl::OnPreCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    switch (LOWORD(wParam))
    {
    case eCommandPrev: _Radio_ProcessKeyDown(GetPrevVisible()); return 0;
    case eCommandNext: _Radio_ProcessKeyDown(GetNextVisible()); return 0;
    }
    bHandled = false;
    return 0;
}


// TMBool CTuiTabButtonImpl::PreTranslateMessage(TUIMSG* pMsg, TMResult& lResult)
// {
//     if ((pMsg->message == CM_APPMESSAGE)
//         && (((TUIMSG*)pMsg->lParam)->message == WM_KEYDOWN)
//         && (((TUIMSG*)pMsg->lParam)->wParam == VK_TAB)
//         && CKeyboard::IsKeyDownControl()
//         && (pMsg->hWnd == GetOwner()))
//     {
//         _Radio_ProcessKeyDown(GetNextVisible());
//         return true;
//     }
//     return false;
// }

TM_REGISTER_CLASS1 (CTuiTabButton);



//////////////////////////////////////////////////////////////////////////////////////
CTuiMenuItemImpl::CTuiMenuItemImpl()
    : CTuiButtonImpl (0, 0)
    , m_nSubMenu (0)
    , m_szCheck (0, 16)
    , m_szArrow (16, 16)
{
//     m_dwStyle &= ~(WS_TABSTOP | BS_NOCLICKFOCUS);
    m_dwLayout = ALIGN_TOP;
//    m_nDownOffsetX = m_nDownOffsetY = 0;
//    m_nNumChecks = 1;
//    m_pBk.GetPre (IDP_PRE_PIC_LISTBOX);
//     m_pCheck = g_picDftCheck();// .GetPre (IDP_PRE_PIC_CHECK);
    m_rcWnd.SetHeight(22);
}

TMResult CTuiMenuItemImpl::OnZoomed(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_pCheck.ChangeDPI(wParam);
    m_pArrow.ChangeDPI(wParam);
    bHandled = false;
    return 0;
}

static void _RenderMenuItemPic(IGXApp* gapp, HTUI hWgt, CTuiRect& rc, TSIZE<TMInt16> sz, CPicPtr& ptr, GXDips nPicMargin, int nFrame)
{
    if (sz.cx != 0)
    {
        rc.OffsetLeft(nPicMargin);
        if (ptr)
        {
            CTuiRect rcx(rc);
            MakeSubYCenter(rcx, sz.cy);
            rcx.SetWidth(sz.cx);
            ptr->Render(nFrame, 0, rcx, 0, gapp);
        }
        rc.OffsetLeft(sz.cx);
    }
}

TMResult CTuiMenuItemImpl::OnMouseEntry(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;

//     DbgTrace("OnMouseEntry:%x", this);

//  SetFocus();

    DISABLE_SPY();
    CTuiWidget self(this);
    if (self.GetMenuPop())
        return 0;
    DISABLE_SPY();
    CTuiWidget(GetOwner()).EndOwnedMenuPop(IDCANCEL);
    SetFocus(eFocusReasonMenuBar);

    PopupSubMenu();

    return 0;
}

TMResult CTuiMenuItemImpl::OnLButtonUp(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (!m_nSubMenu)
    {
        CTuiButtonImpl::OnLButtonUp(uMsg, wParam, lParam, bHandled);
        if (!(m_dwStyle & MS_KEEPPOPUP) && !IsLifeDestroyed()) // (m_dwPriStyle & WS_PRI_DESTROYED)) // maybe destoryed
            g_menuPop.Clear(true, tm_addressof(CTuiMsg(this, uMsg, wParam, lParam)));
    }
    return 0;
}

// TMResult CTuiMenuItemImpl::OnSetFocus(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
// //     TUIHITINFO hi = {this};
// //     GetView()->SetHot(hi);
//     return 0;
// }

TMResult CTuiMenuItemImpl::OnKeyDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    wParam = GetModule()->GetLocal().ConvertKey(wParam);
    switch (wParam)
    {
    case VK_LEFT:
        {
            CView* pView = GetView();
            int nLevel = pView->GetMenuPopLevel(this, false);
            if (nLevel > 0)
                pView->EndMenuPopLevel(nLevel, IDCANCEL, true, tm_addressof(CTuiMsg(this, uMsg, wParam, lParam)));
        }
        return true;

    case VK_RIGHT:
        PopupSubMenu();
        return true;

    case VK_EXECUTE:
    case VK_RETURN:
        if (!m_nSubMenu)
        {
            AutoCheckAndNotify();
            if (!(m_dwStyle & MS_KEEPPOPUP) && !IsLifeDestroyed()) // (m_dwPriStyle & WS_PRI_DESTROYED))
                g_menuPop.Clear(true, tm_addressof(CTuiMsg(this, uMsg, wParam, lParam)));
        }
        return true;

    default:
        break;
    }

    return bHandled = false;
}


// TMResult CTuiMenuItemImpl::OnKillFocus(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//
// }

// TMResult CTuiMenuItemImpl::OnInitMenuPop(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
//     Invalidate();
//     return 0;
// }
// 
// TMResult CTuiMenuItemImpl::OnUnInitMenuPop(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
//     Invalidate();
//     return 0;
// }

TMResult CTuiMenuItemImpl::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    m_pCheck.Release();
    m_pArrow.Release();
    return 0;
}

static void _ShowMenu(CTuiWidget owner, CWindowAT wnd, int nMargin)
{
//     HMONITOR hm = MonitorFromWindow(wnd, MONITOR_DEFAULTTOPRIMARY);
//     MONITORINFO mi = { sizeof(mi) };
//     GetMonitorInfo(hm, &mi);
// 
//     CTuiRect rcPop;
//     wnd.GetWindowRect(&rcPop);
// 
//     CTuiRect rc;
//     owner.GetWindowRect(&rc);
//     owner.ClientToScreen(&rc);
//
//     int x = rc.Right() - nMargin - owner.m_hWnd->GetPadding().right;
//     if (x < mi.rcWork.left)
//         x = mi.rcWork.left;
//     else if (x + rcPop.Width() > mi.rcWork.right)
//         x = rc.Left() - owner.m_hWnd->GetPadding().left - rcPop.Width();
//
//     int y = rc.Top();
//     if (y < mi.rcWork.top)
//         y = mi.rcWork.top;
//     else if (y + rcPop.Height() > mi.rcWork.bottom)
//         y = mi.rcWork.bottom - rcPop.Height();

    GXRect rcTemp;
    owner.GetWindowRect(&rcTemp);
    rcTemp.OffsetLeftRight(nMargin, owner.m_hWnd->GetPadding().right - nMargin); //   .left += nMargin; rcTemp.right -= (nMargin - owner.m_hWnd->GetPadding().right);

    CRectI rcOwner;
    owner.ClientToScreen(rcTemp, rcOwner);

    CRectI rcPop;
    wnd.GetWindowRect(&rcPop);
    rcPop.MoveToXY(rcOwner.Right(), rcOwner.Top());

    CMonitor::HierarchyFromRect(rcPop, rcOwner, rcOwner);

    wnd.SetWindowPos(0, rcPop.Left(), rcPop.Top(),
        0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void CTuiMenuItemImpl::PopupSubMenu()
{
    if (!m_nSubMenu)
        return;

    CTuiWidget wndPop = GetView()->CreateDlg(GetModule()->m_hRes, m_nSubMenu, GetHWND());
    if (!wndPop)
        return;

    _ShowMenu(this, wndPop.GetHWND(), m_nPicMargin);
    wndPop.DoMenuPop(this);
}

// CTuiWgt
EButStates CTuiMenuItemImpl::DoGetStates(HTUI hFocus, HTUI hOver, TMBool bMouseDown)
{
    EButStates s = CTuiButtonImpl::DoGetStates(hFocus, hOver, bMouseDown);
    DISABLE_SPY();
    if (CTuiWidget(this).GetMenuPop())
        s |= e_bssHigh;
    return s;
}

void CTuiMenuItemImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    EButStates ebss = GetStates();
    EButState  ebs = ButStates2ButFrame(ebss);
    int nFrame = m_nCurChecks * e_bsEnd + ebs;
    
    // text color
    SetButTextColor(gapp, ebs, CAppColor::GetColorMenuItemText, nFrame);

    // render bk;
    PaintBk(gapp, nFrame, ebss);
    
    CTuiRect rc;
    GetPaddingRect(&rc);
    OffsetDownRect(ebss, rc);
    GXDips ww = rc.Width();
    GXDips hh = m_szCheck.cy;

    // fore-pic
    {
    tm_setmax(hh, m_szArrow.cy);
    tm_setmax(hh, m_nPicHeight);
    _RenderMenuItemPic(gapp, this, rc, m_szCheck, m_pCheck, m_nPicMargin, nFrame);

    TSIZE<TMInt16> szfor (m_nPicWidth, m_nPicHeight);
    _RenderMenuItemPic(gapp, this, rc, szfor, m_pFore, m_nPicMargin, nFrame);
    
    rc.OffsetRight(-m_nPicMargin - m_szArrow.cx);
    if (m_nSubMenu)
    {
        if (m_pArrow)
        {
            CTuiRect rcx(rc);
            MakeSubYCenter(rcx, m_szArrow.cy);
            rcx.SetLeft(rcx.Right());
            rcx.SetWidth(m_szArrow.cx);
            m_pArrow->Render(nFrame, 0, rcx, 0, gapp);
        }
    }
    }

    if (m_gxText)
    {
        rc.OffsetLeft(m_nPicMargin);
        rc.OffsetRight(-m_nPicMargin);
        
        if (Is_WS_PRI_DIRTY() && (IsAutoWidth() || IsAutoHeight()))
        {
            GXDips w = rc.Width();
            GXDips h = rc.Height();
            CalcTextRect(gapp, w, h, GetMax(ww - w));
            SendScrollRangeMessage(&w, &h);
            GXDips dx = w - rc.Width();
            ww += dx;
            tm_setmax(hh, h);
        }
        
        m_gxText->SetFormat(m_uFormat);
        m_gxText->Paint(rc, 0, 0, gapp);
    }
    
    if (Is_WS_PRI_DIRTY())
    {
        Reset_WS_PRI_DIRTY();
        SendScrollRangeMessage(&ww, &hh);
    }
}


TM_REGISTER_CLASS1(CTuiMenuItem);


TM_END_NAMESPACE

