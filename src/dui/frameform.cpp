/*
 *  @file
 *  @brief  frame: form 
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2012.02.08
 *
 *  Copyright (C) 2012 miragekiller
 */

#include "frameform.h"
#include "uiwidget.h"
#include "view.h"
#include "tmtimer.h"


TM_BGN_NAMESPACE



////////////////////////////////////////////////////////////////////////////////
// class CFormCtrl

void CFormCtrl::Align (TMUInt32 dwStyle, CTuiRect& rc1, CTuiRect& rcNext, GXDips n)
{
    GXDips x;
    switch (dwStyle & ALIGN_MASK)
    {
    case ALIGN_TOP:
        x = rcNext.Top() + n;
        rcNext.MoveTopTo(x);
        rc1.SetBottom(x);
        break;
        
    case ALIGN_RIGHT:
        x = rcNext.Right() - n;
        rcNext.SetRight(x);
        rc1.MoveLeftTo(x);
        break;
        
    case ALIGN_BOTTOM:
        x = rcNext.Bottom() - n;
        rcNext.SetBottom(x);
        rc1.MoveTopTo(x);
        break;
        
    default:
        x = rcNext.Left() + n;
        rcNext.MoveLeftTo(x);
        rc1.SetRight(x);
        break;
    }
}


////////////////////////////////////////////////////////////////////////////////
// CFormSys
TMBool CFormSys::IsVisible(IFrameSource* pSender)
{
    TMUInt32 dwStyle = pSender->FSGetStyle();
    return (dwStyle & WS_CAPTION) && (dwStyle & WS_SYSMENU) ;
}

TMBool CFormSys::OnLButtonDown(IFrameSource* pSender, TMLParam lParam, EHitID hid)
{
    return false;
}

TMBool CFormSys::OnLButtonDblClk(IFrameSource* pSender, TMLParam lParam, EHitID hid)
{
//     PostMessage (pSender->GetHWND(), WM_SYSCOMMAND, SC_CLOSE, 0);
    return false;
}

void CFormSys::Paint(IFrameSource* pSender, IGXApp* gapp, CTuiRect& rc)
{
    if (m_pBk)
    {
        int nFrame = pSender->FSIsActive() ? 1 : 0;
        m_pBk->Render(nFrame, 0, rc, 0, gapp);
    }
    else if (pSender->FSGetWin()->Is_WS_PRI_IS_WINDOW())
    {
        CIconHandle hIcon(CWindowAT(pSender->FSGetWin()->GetHWND()).GetIcon(ICON_SMALL));
        if (hIcon)
            gapp->DrawIcon(rc, hIcon, pSender->FSGetWin()->GetModule()->GetLocal().IsRTL());
    }
}



////////////////////////////////////////////////////////////////////////////////
// CFormOuterBorder
CFormOuterBorder::CFormOuterBorder ()
    : CFormBorderBase(eDftOutterBorderWidth)
    , m_nCorner(0)
{
}

void CFormOuterBorder::FinalConstruct(CModule& mdl)
{
    m_pBk = GetPrePic2(mdl, OUTERBORDER);
}

TMBool CFormOuterBorder::IsVisible(IFrameSource* pSender)
{
    return IsVisible(pSender->FSGetStyle());
}

TMBool CFormOuterBorder::IsVisible(TMUInt32 dwStyle)
{
    return (!(dwStyle & WS_MAXIMIZE)) && (dwStyle & (WS_DLGFRAME | WS_BORDER));
}

void CFormOuterBorder::NextRect(IFrameSource* pSender, CTuiRect& rc)
{
    BorderNextRect(rc);
    pSender->FSCalcShadow(rc);
}

CTuiRect CFormOuterBorder::GetBorder(int nEXt)
{
    int l = nEXt + m_nLeft;
    int t = nEXt + m_nTop;
    int b = nEXt + m_nBottom;
    int r = nEXt + m_nRight;
    tm_setmax(l, 4);
    tm_setmax(t, 4);
    tm_setmax(b, 4);
    tm_setmax(r, 4);
    return CTuiRect(l,t,b,r);
}

struct COBorderCalc
{
    CTuiRect m_rcCorner;
    CTuiRect m_rcBorder;

    void GetBorder(CTuiRect& rc, const CFormOuterBorder& border, int nEXt)
    {
        int l = nEXt + border.m_nLeft;
        int t = nEXt + border.m_nTop;
        int b = nEXt + border.m_nBottom;
        int r = nEXt + border.m_nRight;
        tm_setmax(l, 4);
        tm_setmax(t, 4);
        tm_setmax(b, 4);
        tm_setmax(r, 4);
        rc.SetRect(l,t,b,r);
    }

    COBorderCalc(CFormOuterBorder& border)
    {
        GetBorder(m_rcCorner, border, border.m_nCorner);
        GetBorder(m_rcBorder, border, 0);
    }

    TMBool PtNotInOBorder(const CTuiRect& rc, const CTuiPoint& ptx)
    {
        CTuiRect rc1(rc.Left() + m_rcBorder.Left(), rc.Top() + m_rcCorner.Top(), rc.Right() - m_rcBorder.Right(), rc.Bottom() - m_rcCorner.Bottom());
        CTuiRect rc2(rc.Left() + m_rcCorner.Left(), rc.Top() + m_rcBorder.Top(), rc.Right() - m_rcCorner.Right(), rc.Bottom() - m_rcBorder.Bottom());
        return rc1.PtInRect(ptx) || rc2.PtInRect(ptx);
    }
};

EHitID CFormOuterBorder::HitTestO(IFrameSource* pSender, CTuiRect& rc, const TUIHITINFO& ht)
{
    if (!rc.PtInRect(ht.pt))
        return eHitIDOut;

    if (!IsVisible(pSender))
        return eHitIDClient;

    CTuiRect rcSave(rc);
    NextRect(pSender, rc);

    COBorderCalc calc(*this);
    if (calc.PtNotInOBorder(rcSave, ht.pt))
        return eHitIDClient;

    CTuiPoint pt(ht.pt);
    pt.Offset(-rcSave.Left(), -rcSave.Top());
    GXDips w = rcSave.Width();
    GXDips h = rcSave.Height();
    
    enum { eBorderExt = 16 };
    GXDips nL = m_nLeft + eBorderExt; //tm_max(eBorderMax, m_nLeft);
    GXDips nT = m_nTop + eBorderExt; //tm_max(eBorderMax, m_nTop);
    GXDips nR = m_nRight + eBorderExt; //tm_max(eBorderMax, m_nRight);
    GXDips nB = m_nBottom + eBorderExt; //tm_max(eBorderMax, m_nBottom);

    EHitSubObj nbPos;
    
    if (pt.Y() < nT)
    {
        if (pt.X() < nL)
            nbPos = e_objSZTopLeft;
        else if (pt.X() > (w - nR))
            nbPos = e_objSZTopRight;
        else
            nbPos = e_objSZTop;
    }
    else if (pt.Y() > h - nB)
    {
        if (pt.X() < nL)
            nbPos = e_objSZBottomLeft;
        else if (pt.X() > (w - nR))
            nbPos = e_objSZBottomRight;
        else
            nbPos = e_objSZBottom;
    }
    else if (pt.X() < nL)
        nbPos = e_objSZLeft;
    else
        nbPos = e_objSZRight;

    return TUIHITMAKE(e_objOBorder, nbPos);
}

TMBool CFormOuterBorder::OnLButtonDown(IFrameSource* pSender, TMLParam lParam, EHitID hid)
{
    return false;
}

void CFormOuterBorder::Paint(IFrameSource* pSender, IGXApp* gapp, TMUInt32 dwFStyle, CTuiRect& rc)
{
    TMUInt32 dwStyle = pSender->FSGetStyle();
    if (!((dwStyle & WS_DLGFRAME) || (dwStyle & WS_BORDER)))
        return;

    if (dwStyle & WS_MAXIMIZE)
        rc.InflateRect(m_nLeft, m_nTop, m_nRight, m_nBottom);

    /*/Evan zhu add the codes for set the dialog background /*/
    int nFrame = pSender->FSIsActive() ? 1 : 0;
    if (m_pBk)
    {
        ITuiPic* pic = pSender->FSGetWin()->IsHighContrast() ? GetPrePicFS(pSender, OUTERBORDER) : m_pBk;
        pic->Render(nFrame, 0, rc, 0, gapp); //  pSender->FSGetWin()->GetModule()->m_aPrePic.m_picDftOBorder.Render(nFrame, rc, eVisual);
    }
    NextRect(pSender, rc);
}


////////////////////////////////////////////////////////////////////////////////
// CFormInnerBorder
CFormInnerBorder::CFormInnerBorder()
    : CFormBorderBase (eDftInnerBorderWidth)
{
}

void CFormInnerBorder::FinalConstruct(CModule& mdl)
{
    m_pBk = GetPrePic2(mdl, INNERBORDER);
}

TMBool CFormInnerBorder::IsVisible(IFrameSource* pSender)
{
    return IsVisible(pSender->FSGetExStyle());
}

TMBool CFormInnerBorder::IsVisible(TMULong dwExtStyle)
{
    return (0 != (dwExtStyle & WS_ADV_CLIENTEDGE));
}

void CFormInnerBorder::NextRect(CTuiRect& rc)
{
    BorderNextRect(rc);
}

void CFormInnerBorder::HitTestI(IFrameSource* pSender, const TUIPoint& pt, CTuiRect& rc, EHitID& hid)
{
    if (IsVisible(pSender))
    {
        NextRect(rc);
        if (!rc.PtInRect(pt) && !(pSender->FSGetStyle() & (WS_MAXIMIZE | WS_MINIMIZE)))
            hid = eHitIDIBorder;
    }
}

void CFormInnerBorder::Paint(IFrameSource* pSender, IGXApp* gapp, TMUInt32 dwFStyle, CTuiRect& rc)
{
    int nFrame = pSender->FSGetWin()->GetButtonState();
    if (m_pBk)
    {
        if (pSender->FSGetWin()->IsHighContrast())
            GetPrePicFS(pSender, INNERBORDER)->Render(nFrame, 0, rc, 0, gapp);// pSender->FSGetWin()->GetModule()->m_aPrePic.m_picDftIBorder.Render(nFrame, rc, eVisual);
        else
            m_pBk->Render(nFrame, 0, rc, 0, gapp);
    }
    NextRect(rc);
}


////////////////////////////////////////////////////////////////////////////////
// CFormCaption
CFormCaption::CFormCaption ()
    : m_uFormat (DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_WORD_ELLIPSIS | DT_END_ELLIPSIS)
//     , m_clrFont (TMRGB(255,255,255))
//     , m_clrFontActive (TMRGB(255,255,255))
    , m_nHeight (eDftCaptionHeight)
    , m_nMarginLeft (20)
    , m_nMarginTop (0)
    , m_nMarginRight (60)
    , m_nMarginBottom (0)
    , m_ctrlSys (0)
    , m_ctrlClose (-20)
    , m_ctrlMax (-40)
    , m_ctrlMin (-60)
    , m_ctrlHelp (-80)
{
//     m_pBk.GetPre (IDP_PRE_PIC_CAPTION);
    m_font.height = 0;
    m_font.styleMask = 0;
//     m_clrFont[0] = cColorInvalid;
//     m_clrFont[1] = cColorInvalid;
}

CFormCaption::~CFormCaption ()
{
//     if (m_pFont)
//         m_pFont->Release();
}

void CFormCaption::FinalConstruct(CModule& mdl)
{
    m_pBk = GetPrePic2(mdl, CAPTION);
    m_ctrlClose.FinalConstruct(mdl);
    m_ctrlMin.FinalConstruct(mdl);
    m_ctrlMax.FinalConstruct(mdl);
    m_ctrlHelp.FinalConstruct(mdl);
    m_ctrlSys.FinalConstruct(mdl);
}

TMBool CFormCaption::IsVisible(IFrameSource* pSender)
{
    return IsVisible(pSender->FSGetStyle());
}

TMBool CFormCaption::IsVisible(TMUInt32 dwStyle)
{
    return (dwStyle & WS_CAPTION) == WS_CAPTION;
}

void CFormCaption::NextRect(TMUInt32 dwFStyle, CTuiRect& rc)
{
    CTuiRect rc2;
    Align (dwFStyle, rc2, rc, m_nHeight);
}

EHitID CFormCaption::HitTestC(IFrameSource* pSender, TMUInt32 dwFStyle, CTuiRect& rc, TUIHITINFO& ht)
{
    if (IsVisible(pSender))
    {
        CTuiRect rc2 = rc;
        Align(dwFStyle, rc2, rc, m_nHeight);

        if (rc2.PtInRect (ht.pt))
        {
            if (m_ctrlSys.HitTest(pSender, rc2, ht))
                return eHitIDSys;
            else  if (m_ctrlClose.HitTest(pSender, rc2, ht))
                return eHitIDClose;
            else if (m_ctrlMax.HitTest(pSender, rc2, ht))
                return eHitIDMax;
            else if (m_ctrlMin.HitTest(pSender, rc2, ht))
                return eHitIDMin;
            else if (m_ctrlHelp.HitTest(pSender, rc2, ht))
                return eHitIDHelp;
            else
                return eHitIDCaption;
        }
    }
    return eHitIDClient;
}

TMBool CFormCaption::OnLButtonDown(IFrameSource* pSender, TMLParam lParam, EHitID hid)
{
//         if (!(pSender->GetStyle() & WS_MAXIMIZE))
//             pSender->SendMessage(WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);

    return false;
}

TMBool CFormCaption::OnLButtonDblClk(IFrameSource* pSender, TMLParam lParam, EHitID hid)
{
//     TMUInt32 dwStyle = pSender->GetStyle ();
//     if (dwStyle & WS_MAXIMIZEBOX)
//     {
//         TMUInt uID = (dwStyle & (WS_MINIMIZE | WS_MAXIMIZE))
//             ? SC_RESTORE : SC_MAXIMIZE;
//         pSender->SendMessage(WM_SYSCOMMAND, uID, 0);
//         pSender->Invalidate();
//     }
    return false;
}

void CFormCaption::Paint(IFrameSource* pSender, IGXApp* gapp, TMUInt32 dwFStyle, CTuiRect& rc)
{
    CTuiRect rc2 = rc;
    Align (dwFStyle, rc2, rc, m_nHeight);

    if (!gapp->ClipAndPush(rc2, true, eGXPropClip | eGXPropFont))
        return;
    
    int nFrame = pSender->FSIsActive() ? 1 : 0;
    HTUI hWnd = pSender->FSGetWin();

    // text color
    CPropColor* clrss[] = { m_clrFont, 0 };
    hWnd->RenderSetTextColor(gapp, nFrame, clrss, CAppColor::GetColorCaptionText);

    // render bk
    TMBool bHC = hWnd->IsHighContrast();
    if (bHC)
        GetPrePicFS(pSender, CAPTION)->Render(nFrame, 0, rc2, 0, gapp); //  pSender->FSGetWin()->GetModule()->m_aPrePic.m_picDftCaption.Render(nFrame, rc2, eVisual);
    else if (m_pBk)
        m_pBk->Render(nFrame, 0, rc2, 0, gapp);

    // text
    gapp->FontSetCurrent(m_font);
    GX::CText& text = hWnd->m_gxText;
    if (text)
    {
        CTuiRect rc3(rc2);
        rc3.InflateRect(-m_nMarginLeft, -m_nMarginTop, -m_nMarginRight, -m_nMarginBottom);
        text->SetFormat(m_uFormat);
        text->Paint(rc3, 0, 0, gapp);
    }

    // button
    m_ctrlSys.Paint(pSender, gapp, rc2);
    m_ctrlClose.Paint(pSender, gapp, rc2);
    m_ctrlMax.Paint(pSender, gapp, rc2);
    m_ctrlMin.Paint(pSender, gapp, rc2);
    m_ctrlHelp.Paint(pSender, gapp, rc2);

    gapp->Pop();
}


////////////////////////////////////////////////////////////////////////////////
// CFormScroll
struct SPos
{
    GXDips nButton;
    GXDips nPageUp;
    GXDips nThumb;
    GXDips nPageDown;
};

struct CThumb
{
    IFrameSource*   m_pFrmScrollWnd;
    CFormScroll*  m_pScroll;
    EHitID          m_hid;
    CTimerID1       m_tm1;
    CTimerID2       m_tm2;

    GXDips          m_nMousePos;
    GXDips          m_nNewPageUp;
    SPos            m_sp;
    TUISCROLLINFO   m_si;

    // CTimerID1
    void OnTimer(const CTimerID1&, ...)
    {
        m_tm2.SetTimer(100, this);
    }

    // CTimerID2
    void OnTimer(const CTimerID2&, ...)
    {
        DbgAssert(m_pFrmScrollWnd);
        DbgAssert(m_pScroll);
        CView* pm = m_pFrmScrollWnd->FSGetWin()->GetView();
        EHitID hit (pm->GetHitInfo().hid);
        if (!DoLButtonDown(hit))
            Clear(); // error
    }

    void Clear()
    {
        m_pFrmScrollWnd = 0;
        m_pScroll = 0;
        m_tm1.KillTimer();
        m_tm2.KillTimer();
    }

    TMBool DoLButtonDown(EHitID hit)
    {
        DbgAssert(m_pFrmScrollWnd);
        DbgAssert(m_pScroll);
        
        if (TUIHITGETOBJ(hit) == e_objHScroll) // .nsBar == SB_HORZ)
            m_pFrmScrollWnd->FSSendMessage(CM_HSCROLL, TUIHITGETSUBSB(hit));
        else if (TUIHITGETOBJ(hit) == e_objVScroll)
            m_pFrmScrollWnd->FSSendMessage(CM_VSCROLL, TUIHITGETSUBSB(hit));
        else
            return false;
        return true;
    }

    
    TMBool IsTracking(IFrameSource* pSender, CFormScroll* pScroll, EHitObj nObjID)
    {
        return pSender == m_pFrmScrollWnd && pScroll == m_pScroll && m_hid == TUIHITMAKE(nObjID, e_objSBThumb);
    }

    void OnLButtonDown(IFrameSource* pSender, CFormScroll* pScroll, TMLParam lParam, EHitID hit)
    {
        DbgAssert(pSender);
        DbgAssert(pScroll);
        DbgAssert(TUIHITGETOBJ(hit) == e_objVScroll || TUIHITGETOBJ(hit) == e_objHScroll);

        m_pFrmScrollWnd = pSender;
        m_pScroll = pScroll;
        m_hid = hit;
        m_tm1.KillTimer();
        m_tm2.KillTimer();
        pSender->FSGetWin()->SetCapture();
        if (!m_pFrmScrollWnd)
            return; // window maybe destroy or lost capture.
        
        if (TUIHITGETSUBSB(hit) == SB_THUMBPOSITION)
        {
            if (TUIHITGETOBJ(hit) == e_objHScroll)// SB_HORZ)
            {
                m_nMousePos = (short)LOWORD (lParam);
                pScroll->InvalidHScroll(pSender);
            }
            else
            {
                DbgAssert(TUIHITGETOBJ(hit) == e_objVScroll);
                m_nMousePos = (short)HIWORD (lParam);
                pScroll->InvalidVScroll(pSender);
            }
            m_nNewPageUp = m_sp.nPageUp;
        }
        else
        {
            DoLButtonDown(hit);
            m_tm1.SetTimer(300, this);
        }
    }

    void OnCaptureChanged()
    {
        if (!m_pFrmScrollWnd)
        {
            DbgAssert(!m_tm1);
            DbgAssert(!m_tm2);
            DbgAssert(!m_pScroll);
            return;
        }

        IFrameSource* pw = m_pFrmScrollWnd;
        CFormScroll* ps = m_pScroll;
        Clear();
        
        if (TUIHITGETOBJ(m_hid) == e_objHScroll) // .nsBar == SB_HORZ)
        {
            pw->FSSendMessage(CM_HSCROLL, SB_ENDSCROLL, 0);
            ps->InvalidHScroll(pw);
        }
        else
        {
            DbgAssert(TUIHITGETOBJ(m_hid) == e_objVScroll);
            pw->FSSendMessage(CM_VSCROLL, SB_ENDSCROLL, 0);
            ps->InvalidVScroll(pw);
        }
    }
    
    void OnMouseMove(IFrameSource* pSender, CFormScroll* pScroll, TMUInt uMsg, GXDips nNewMousePos)
    {
        m_nNewPageUp = m_sp.nPageUp + nNewMousePos - m_nMousePos;
        GXDips nPos = (m_sp.nPageUp + nNewMousePos - m_nMousePos) *
            (m_si.nProject - m_si.nView) / (m_sp.nPageUp + m_sp.nPageDown);
        if (nPos < 0)
            nPos = 0;
        if (m_nNewPageUp < 0)
            m_nNewPageUp = 0;
        if (m_nNewPageUp > m_sp.nPageDown + m_sp.nPageUp)
            m_nNewPageUp = m_sp.nPageDown + m_sp.nPageUp;
//        DbgTrace("OnMouseMove %d : %d : %d : %d", nNewMousePos, m_nMousePos, nPos, m_sp.nPageUp);
        pSender->FSSendMessage(uMsg, SB_THUMBTRACK, nPos + 0.5f);
    }

    void GetTrackPos(IFrameSource* pSender, CFormScroll* pScroll, SPos& sp, EHitObj nObjID)
    {
        if (IsTracking (pSender, pScroll, nObjID))
        {
            sp.nPageUp = m_nNewPageUp;
            sp.nPageDown = m_sp.nPageDown + m_sp.nPageUp - m_nNewPageUp;
        }
    }

    EHitSubObj _GetHitCode(GXDips nMin, GXDips nMax, GXDips nCur)
    {
        if (nCur < nMin + m_sp.nButton)
            return e_objSBLineUp;
        if (nCur < nMin + m_sp.nButton + m_sp.nPageUp)
            return e_objSBPageUp;
        if (nCur < nMin + m_sp.nButton + m_sp.nPageUp + m_sp.nThumb)
            return e_objSBThumb;
        if (nCur < nMin + m_sp.nButton + m_sp.nPageUp + m_sp.nThumb + m_sp.nPageDown)
            return e_objSBPageDown;
        return e_objSBLineDown;
    }

    EHitSubObj HitTestH (IFrameSource* pSender, CFormScroll* pSB, GXDips nMin, GXDips nMax, GXDips nCur)
    {
//         m_si.nProject = m_si.nOrg = m_si.nView = 0;
//         pSender->SendMessage(CM_GETSCROLLINFO, (TMLParam)&m_si, 0);
        pSender->FSGetHScrollInfo(m_si);
        pSB->GetPos(m_sp, m_si, nMax - nMin);
        return _GetHitCode(nMin, nMax, nCur);
    }

    EHitSubObj HitTestV (IFrameSource* pSender, CFormScroll* pSB, GXDips nMin, GXDips nMax, GXDips nCur)
    {
//         m_si.nProject = m_si.nOrg = m_si.nView = 0;
//         pSender->SendMessage(CM_GETSCROLLINFO, 0, (TMLParam)&m_si);
        pSender->FSGetVScrollInfo(m_si);
        pSB->GetPos(m_sp, m_si, nMax - nMin);
        return _GetHitCode(nMin, nMax, nCur);
    }
};

static CThumb   s_thumbFrmScroll;



void CFormScroll::PrintScroll(IGXApp* gapp, IFrameSource* pSender, EHitObj nObjID,
    const TUISCROLLINFO& info, CTuiRect& rc, GXDips nHeight, TMBool bVisible, TMBool bForeObj)
{
    DbgAssert(pSender);
    if (rc.IsRectEmpty())
        return;

    ITuiPic* p = m_pBk;
    HTUI pWnd = pSender->FSGetWin();
    if (p && !bForeObj && pWnd->GetView()->IsHighContrast())
        p = GetPrePic2(*pWnd->GetModule(), SCROLLBAR); // &pSender->FSGetWin()->GetModule()->m_aPrePic.m_picDftScroll;//  (IDP_PRE_PIC_SCROLLBAR);

    SPos sp;
    GetPos (sp, info, nHeight);
    s_thumbFrmScroll.GetTrackPos(pSender, this, sp, nObjID);

    SCROLLSTATE st;
    st.obj = nObjID;
    st.nThumbTop = sp.nButton + sp.nPageUp;// sp.nThumb;
    st.nThumbBottom = st.nThumbTop + sp.nThumb;
    st.nButtonHeight = sp.nButton;

    if (!bVisible)
    {
        if (!pWnd->m_aniList.IsInAnimation(TUIHITMAKE(nObjID, 0)))
            return;
        st.eState[0] = st.eState[1] = st.eState[2] = st.eState[3] = st.eState[4] = e_bssHide;
    }
    else if ((info.nProject <= info.nView) || !pWnd->IsWindowEnabled())
        st.eState[0] = st.eState[1] = st.eState[2] = st.eState[3] = st.eState[4] = e_bssDisable;
    else
    {
        CView* pm = pWnd->GetView();
        const TUIHITINFO& hi = pm->GetHitInfo();
        EHitID hid = hi.hid;
        TMBool bOver = ((pm->GetOver() == pWnd) && (nObjID == TUIHITGETOBJ(hid)) && hi.pTaget == this); //hi.bNC);
        TMBool bDown = pm->IsMouseDown();
        st.eState[0] = st.eState[1] = st.eState[2] = st.eState[3] = st.eState[4] = bOver ? e_bssHigh : e_bssNormal;
        if (bOver)
        {
            DbgAssert(TUIHITGETSUBSB(hid) >= SB_LINEUP && TUIHITGETSUBSB(hid) <= SB_THUMBPOSITION);
            st.eState[TUIHITGETSUBSB(hid)] = bDown ? e_bssDown : e_bssFocus;
        }
    }

    pWnd->m_aniList.AniRenderScroll(gapp, p, st, rc);
}

enum { eDftFrmScrollWdith = 20 };

CFormScroll::CFormScroll ()
    : m_nBarWidth (eDftFrmScrollWdith)
    , m_nButtonHeight (eDftFrmScrollWdith)
    , m_nThumbMin (eDftFrmScrollWdith)
{
//     m_pBk.GetPre(IDP_PRE_PIC_SCROLLBAR);
//     m_pSizeBox.GetPre(IDP_PRE_PIC_SIZEBOX);
}

CFormScroll::~CFormScroll ()
{
}

void CFormScroll::FinalConstruct(CModule& mdl)
{
    m_pBk = GetPrePic2(mdl, SCROLLBAR);
    m_pSizeBox = GetPrePic2(mdl, SIZEBOX);
}


// TMBool CFormScroll::IsVisible(IFrameSource* pSender)
// {
// //    TMUInt32 dwStyle = pSender->GetStyle();
//     return m_nBarWidth >= 0 && (pSender->IsLayoutHScroll() || pSender->IsLayoutVScroll()); // (dwStyle & (WS_VSCROLL | WS_HSCROLL)) ;
// }

void CFormScroll::NextRect(IFrameSource* pSender, CTuiRect& rc)
{
    if (pSender->IsLayoutVScroll())
        rc.OffsetRight(-m_nBarWidth);
    if (pSender->IsLayoutHScroll())
        rc.OffsetBottom(-m_nBarWidth);
}

TMBool CFormScroll::IsScrollBarShown(IFrameSource* pSender)
{
    if (pSender->IsLayoutHotScroll())
        return pSender->FSGetWin()->GetView()->IsHoting(pSender->FSGetWin());
    return true;
}

void CFormScroll::GetPos (SPos& sp, const TUISCROLLINFO& info, GXDips nLen)
{
    if (nLen < m_nButtonHeight * 2)
    {
        sp.nButton = nLen / 2;
        sp.nPageDown = sp.nPageUp = sp.nThumb = 0;
        return;
    }
    
    sp.nButton = m_nButtonHeight;
    nLen -= m_nButtonHeight * 2;
    
    if (info.nProject <= info.nView)
    {
        sp.nThumb = 0;
        sp.nPageUp = 0;
        sp.nPageDown = nLen;
        return;
    }
    
    sp.nThumb = info.nView * nLen / info.nProject;
    tm_setmax(sp.nThumb, m_nThumbMin);
//     if (sp.nThumb < eDftFrmScrollWdith)
//         sp.nThumb = eDftFrmScrollWdith;
    if (sp.nThumb >= nLen)
    {
        sp.nThumb = nLen;
        sp.nPageUp = sp.nPageDown = 0;
        return;
    }

    GXDips nTop = info.nOrg;
    if (nTop < 0)
        nTop = 0;
    if (nTop + info.nView >= info.nProject)
        nTop = info.nProject - info.nView;
    
    sp.nPageUp = nTop * (nLen - sp.nThumb) / (info.nProject - info.nView);
    sp.nPageDown = nLen - sp.nPageUp - sp.nThumb;
}

TMBool CFormScroll::HitTestS(IFrameSource* pSender, CTuiRect& rcx, TUIHITINFO& ht, EHitID& hid)
{
//     TMUInt32 dwStyle = pSender->GetStyle ();
    CTuiRect::value_type r = rcx.Right(), l = r, b = rcx.Bottom();
    
    if (pSender->IsLayoutVScroll())
    {
        l -= m_nBarWidth;
        CTuiRect::value_type t = b;
        if (pSender->IsLayoutHScroll()) // dwStyle & WS_HSCROLL)
        {
            t -= m_nBarWidth;
            if (((CTuiPoint&)ht.pt).PtInRect (l, t, r, b))
            {
                hid = eHitIDBottomRight;
                return true;
            }
        }
        
        if (((CTuiPoint&)ht.pt).PtInRect (l, rcx.Top(), r, t))
        {
            EHitSubObj nsPos = s_thumbFrmScroll.HitTestV (pSender, this, rcx.Top(), t, PointY(ht.pt));
            hid = TUIHITMAKE(e_objVScroll, nsPos);
            ht.pTaget = this;
            return true;
        }
    }
    
    if (pSender->IsLayoutHScroll()) // dwStyle & WS_HSCROLL)
    {
        if (((CTuiPoint&)ht.pt).PtInRect(rcx.Left(), b - m_nBarWidth, l, b))
        {
            EHitSubObj nsPos = s_thumbFrmScroll.HitTestH (pSender, this, rcx.Left(), l, PointX(ht.pt));
            hid = TUIHITMAKE(e_objHScroll, nsPos);
            ht.pTaget = this;
            return true;
        }
    }

    return false;
}

void CFormScroll::Paint(IFrameSource* pSender, IGXApp* gapp, const CTuiRect& rcx, TMBool bForeObj)
{
    if (!m_pBk)
        return;

//     DbgAssert(pSender->IsLayoutHScroll() || pSender->IsLayoutVScroll()); // >GetStyle() & (WS_VSCROLL | WS_HSCROLL));

    CTuiRect::value_type r = rcx.Right(), l = r, b = rcx.Bottom();
    TMBool bShowScrollBar = IsScrollBarShown(pSender);
    if (pSender->IsLayoutVScroll())
    {
        TUISCROLLINFO siv = { 0 };
        pSender->FSGetVScrollInfo(siv);
        l -= m_nBarWidth;
        CTuiRect::value_type t = b;
        if (pSender->IsLayoutHScroll() && bShowScrollBar)
        {
            t -= m_nBarWidth;
            CTuiRect rc2(l, t, r, b);
            if (m_pSizeBox)
                m_pSizeBox->Render(0, 0, rc2, 0, gapp);
        }

        CTuiRect rc2(l, rcx.Top(), r, t);
        PrintScroll(gapp, pSender, e_objVScroll, siv, rc2, rc2.Height(), bShowScrollBar, bForeObj);
    }
    
    if (pSender->IsLayoutHScroll())
    {
        TUISCROLLINFO sih = { 0 };
        pSender->FSGetHScrollInfo(sih);
        CTuiRect rc2(rcx.Left(), b - m_nBarWidth, l, b);
        PrintScroll(gapp, pSender, e_objHScroll, sih, rc2, rc2.Width(), bShowScrollBar, bForeObj);
    }
}

void CFormScroll::InvalidHScroll (IFrameSource* pSender)
{
    CTuiRect rc;
    pSender->FSGetHScrolBarRect(rc, m_nBarWidth);
    pSender->FSGetWin()->InvalidateRect(rc);
}

void CFormScroll::InvalidVScroll (IFrameSource* pSender)
{
    CTuiRect rc;
    pSender->FSGetVScrolBarRect(rc, m_nBarWidth);
    pSender->FSGetWin()->InvalidateRect(rc);
}

TMBool CFormScroll::OnMouseLeave(IFrameSource* pSender, TMWParam wParam, TMLParam lParam)
{
    TMBool bShowScrollBar = IsScrollBarShown(pSender);
    HTUI hWnd = pSender->FSGetWin();
    hWnd->m_aniList.AddScrollAni(*(TUIHITINFO*)wParam, (TUIHITINFO*)wParam, (TUIHITINFO*)lParam, bShowScrollBar);
    hWnd->Invalidate();
    return false;
}

TMBool CFormScroll::OnMouseEntry(IFrameSource* pSender, TMWParam wParam, TMLParam lParam)
{
    TMBool bShowScrollBar = IsScrollBarShown(pSender);
    HTUI hWnd = pSender->FSGetWin();
    hWnd->m_aniList.AddScrollAni(*(TUIHITINFO*)lParam, (TUIHITINFO*)wParam, (TUIHITINFO*)lParam, bShowScrollBar);
    hWnd->Invalidate();
    return false;
}

TMBool CFormScroll::OnMouseMove(IFrameSource* pSender, TMLParam lParam, EHitID hid)
{
    EHitObj obj = TUIHITGETOBJ(hid);
    if (!s_thumbFrmScroll.IsTracking(pSender, this, obj))
        return false;
    
    if (obj == e_objHScroll)// .nsBar == SB_HORZ)
    {
        s_thumbFrmScroll.OnMouseMove(pSender, this, CM_HSCROLL, (short)LOWORD(lParam));
        InvalidHScroll(pSender);
    }
    else
    {
        DbgAssert(TUIHITGETOBJ(hid) == e_objVScroll);
        s_thumbFrmScroll.OnMouseMove(pSender, this, CM_VSCROLL, (short)HIWORD(lParam));
        InvalidVScroll(pSender);
    }
    return false;
}

TMBool CFormScroll::OnLButtonDown(IFrameSource* pSender, TMLParam lParam, EHitID hit)
{
    s_thumbFrmScroll.OnLButtonDown(pSender, this, lParam, hit);
    return 0;
}

TMBool CFormScroll::OnLButtonUp(IFrameSource* pSender, TMLParam lParam, EHitID hid)
{
    CView* pm = pSender->FSGetWin()->GetView();
    pm->ReleaseCapture();
    return 0;
}

TMBool CFormScroll::OnCaptureChanged()
{
    s_thumbFrmScroll.OnCaptureChanged();
    return 0;
}

TMBool CFormScroll::OnLButtonDblClk(IFrameSource* pSender, TMLParam lParam, EHitID hid)
{
    return OnLButtonDown(pSender, lParam, hid);
}

////////////////////////////////////////////////////////////////////////////////
// CScrollCtrl
TMBool CScrollCtrl::ProcessFrameMessage(
    IFrameSource* pSender, TMUInt uMsg, TMWParam wParam,
    TMLParam lParam, TMResult& lResult)
{
    switch (uMsg)
    {
    case CM_HITTEST:
        {
            CTuiRect rc;
            pSender->FSGetRect(rc);
            TUIHITINFO& ht = *(TUIHITINFO*)lParam;
            return HitTestS(pSender, rc, ht, ht.hid);
        }

    case CM_MOUSEENTRY:
    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
        return DoMouse(*this, pSender, pSender->FSGetWin()->GetView()->GetHitInfo().hid, uMsg, wParam, lParam);

    case WM_CAPTURECHANGED:
        OnCaptureChanged();
        return 0;

    case WM_MOUSELEAVE:
        return DoMouse(*this, pSender, ((TUIHITINFO*)wParam)->hid, uMsg, wParam, lParam);
    }

    return false;
}




////////////////////////////////////////////////////////////////////////////////
// CFormManage
TMBool CFormManage::OnMouse(IFrameSource* pSender, EHitID hid, TMUInt uMsg, TMWParam wParam, TMLParam lParam)
{
#define HANDLEMOUSE(id,ctrl) \
    case id: return CFormCtrl::DoMouse(ctrl, pSender, hid, uMsg, wParam, lParam);

    switch (TUIHITGETOBJ(hid))
    {
        HANDLEMOUSE (e_objOBorder,     m_ctrlOBorder);
        HANDLEMOUSE (e_objCaption,     m_ctrlCaption);
        HANDLEMOUSE (e_objVScroll,     m_ctrlScroll);
        HANDLEMOUSE (e_objHScroll,     m_ctrlScroll);
        HANDLEMOUSE (e_objMin,         m_ctrlCaption.m_ctrlMin);
        HANDLEMOUSE (e_objMax,         m_ctrlCaption.m_ctrlMax);
        HANDLEMOUSE (e_objHelp,        m_ctrlCaption.m_ctrlHelp);
        HANDLEMOUSE (e_objClose,       m_ctrlCaption.m_ctrlClose);
        HANDLEMOUSE (e_objSys,         m_ctrlCaption.m_ctrlSys);
    }
    return false;

#undef HANDLEMOUSE
}

void CFormManage::HitTestAll(IFrameSource* pSender, TUIHITINFO& ht)
{
    CTuiRect rc;
    pSender->FSGetRect(rc);
    CTuiPoint pt(ht.pt);

    EHitID hid = m_ctrlOBorder.HitTestO(pSender, rc, ht);
    
    if (hid == eHitIDClient)
        hid = m_ctrlCaption.HitTestC(pSender, m_dwFrameStyle, rc, ht);
    
    if (hid == eHitIDClient)
        m_ctrlIBorder.HitTestI(pSender, ht.pt, rc, hid);
    
    if (hid == eHitIDClient)
        m_ctrlScroll.HitTestS(pSender, rc, ht, hid);

    if (hid != eHitIDClient)
    {
        ht.bNC = true;
        ht.hid = hid;
    }
    else
    {
        DbgAssert(!ht.bNC);
        DbgAssert(ht.hid == 0);
    }


//     DbgTrace(L"pNew: %x, hid:%d", pSender, hid.nObjID);
}

void CFormManage::Invalidate (IFrameSource* pSender)
{
    pSender->FSGetWin()->Invalidate();
}

TMBool CFormManage::ProcessFrameMessage (
    IFrameSource* pSender, TMUInt uMsg, TMWParam wParam,
    TMLParam lParam, TMResult& lResult)
{
    switch (uMsg)
    {
    case CM_HITTEST:
        HitTestAll(pSender, *(TUIHITINFO*)lParam);
        return true;

    case WM_NCACTIVATE:
//        case CM_GROUPACTIVATE:
    case WM_SETTEXT:
    case CM_SETTEXT:
        Invalidate (pSender);
        return false;
        
    case CM_NCMOUSEENTRY:
    case WM_NCLBUTTONDOWN:
    case WM_NCMOUSEMOVE:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
        return OnMouse(pSender, pSender->FSGetWin()->GetView()->GetHitInfo().hid, uMsg, wParam, lParam);

    case CM_NCMOUSELEAVE:
        return OnMouse(pSender, ((TUIHITINFO*)wParam)->hid, uMsg, wParam, lParam);

    case WM_CAPTURECHANGED:
        {
            TUIHITINFO& hi = pSender->FSGetWin()->GetView()->GetHitInfo();
            if (hi.bNC)
                return OnMouse(pSender, hi.hid, uMsg, wParam, lParam);
        }
        return false;
        
    case CM_PRINTCLIENT:
        if (lParam & PRF_NONCLIENT)
        {
            CTuiRect rc;
            pSender->FSGetRect(rc);
            TMUInt32 dwFrameStyle = m_dwFrameStyle;
            IGXApp* gapp = (IGXApp*)wParam;

            m_ctrlOBorder.Paint(pSender, gapp, dwFrameStyle, rc);
            //Paint(m_ctrlCaption, pSender, hDC, rc);
            if (m_ctrlCaption.IsVisible(pSender))
                m_ctrlCaption.Paint(pSender, gapp, dwFrameStyle, rc);

            if (m_ctrlIBorder.IsVisible(pSender))
                m_ctrlIBorder.Paint(pSender, gapp, dwFrameStyle, rc);

            if (!pSender->IsLayoutFloatScroll())
                m_ctrlScroll.Paint(pSender, gapp, rc);
        }
        else if (lParam & PRF_FLOATSCROLLBAR)
        {
            if (pSender->IsLayoutFloatScroll())
            {
                CTuiRect rc;
                pSender->FSGetWin()->GetClientRect(&rc);
                IGXApp* gapp = (IGXApp*)wParam;
                m_ctrlScroll.Paint(pSender, gapp, rc);
            }
        }
        return true;

    case CM_GETSCROLLBARINFO:
        if (pSender->IsLayoutHScroll())
        {
            TUISCROLLBARINFO* p = (TUISCROLLBARINFO*)wParam;
            pSender->FSGetHScrolBarRect(p->rc, m_ctrlScroll.m_nBarWidth);
            SPos sp;
            TUISCROLLINFO si;
            pSender->FSGetHScrollInfo(si);
            m_ctrlScroll.GetPos(sp, si, RectWidth(p->rc));
            p->nButtonHeight = sp.nButton;
            p->nThumbTop = sp.nThumb - RectLeft(p->rc);
            p->nThumbBottom = sp.nPageDown - RectLeft(p->rc);
        }
        if (pSender->IsLayoutVScroll())
        {
            TUISCROLLBARINFO* p = (TUISCROLLBARINFO*)lParam;
            pSender->FSGetVScrolBarRect(p->rc, m_ctrlScroll.m_nBarWidth);
            SPos sp;
            TUISCROLLINFO si;
            pSender->FSGetVScrollInfo(si);
            m_ctrlScroll.GetPos(sp, si, RectWidth(p->rc));
            p->nButtonHeight = sp.nButton;
            p->nThumbTop = sp.nThumb - RectLeft(p->rc);
            p->nThumbBottom = sp.nPageDown - RectLeft(p->rc);
        }
        return true;

    case CM_SETOUCAPTIONBKPIC:
        {    
            CModule* pm = NULL;
            if (lParam)
            {
                pm = pSender->FSGetWin()->GetView()->GetModule((TMHModule)lParam);
            }
            else
            {
                pm = GetModule();
                DbgAssert(pm);
            }

            if (pm)
            {
                m_ctrlCaption.m_pBk.Create(*pm, wParam, GetDPI());
                lResult = true;
                Invalidate (pSender);
            }
        }
        return true;

    case CM_SETFRAMEPIC:
        switch (wParam)
        {
        case e_objCaption:  m_ctrlCaption.m_pBk = (ITuiPic*)lParam; break;
        case e_objMin:      m_ctrlCaption.m_ctrlMin.m_pBk = (ITuiPic*)lParam; break;
        case e_objMax:      m_ctrlCaption.m_ctrlMax.m_pBk = (ITuiPic*)lParam; break;
        case e_objClose:    m_ctrlCaption.m_ctrlClose.m_pBk = (ITuiPic*)lParam; break;
        case e_objSys:      m_ctrlCaption.m_ctrlSys.m_pBk = (ITuiPic*)lParam; break;
        case e_objHelp:     m_ctrlCaption.m_ctrlHelp.m_pBk = (ITuiPic*)lParam; break;
        case e_objOBorder:  m_ctrlOBorder.m_pBk = (ITuiPic*)lParam; break;
        case e_objIBorder:  m_ctrlIBorder.m_pBk = (ITuiPic*)lParam; break;
        default: return false;
        }
        return lResult = true;

    case CM_COLORVISUAL:
        if (wParam & eColorVisualHighContrast)
            pSender->FSGetWin()->ChangeFrame(false);
        return false;

    default:
        return false;
    }
}

void CFormManage::GetFrameInfo(HTUI hWnd, FRAMEINFO& fi, TMUInt32 dwStyle, TMUInt32 dwExStyle)
{
    CTuiRect rc(0,0,0,0);
    if (m_ctrlOBorder.IsVisible(dwStyle))
        m_ctrlOBorder.NextRect(hWnd, rc);
    if (m_ctrlCaption.IsVisible(dwStyle))
        m_ctrlCaption.NextRect(m_dwFrameStyle, rc);
    if (m_ctrlIBorder.IsVisible(dwExStyle))
        m_ctrlIBorder.NextRect(rc);
    fi.m_border.SetRect(rc.Left(), rc.Top(), -rc.Right(), -rc.Bottom());
    fi.m_nSBHeight = m_ctrlScroll.m_nBarWidth;
    fi.m_nSBWidth = m_ctrlScroll.m_nBarWidth;
}

TMBool CFormManage::PtInOBorder(const CTuiRect& rc, const CTuiPoint& pt)
{
    COBorderCalc calc(m_ctrlOBorder);
    return !calc.PtNotInOBorder(rc, pt);
}


#ifdef TUI_EDIT_MODE

int CFormManage::EditorRender(IGXApp* gapp, int nState, const CTuiRect& rc)
{
    struct Win : public CTuiWgt
    {
        Win()
        {
            m_dwStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
            m_dwAdvStyle = WS_ADV_CONTEXTHELP | WS_EX_CLIENTEDGE;
            m_dwLayout = LAYOUT_HSCROLL | LAYOUT_VSCROLL;
            m_nScrollW = 800;
            m_nScrollH = 600;
            m_gxText.ReCreate(L"Sample windows text", false);
        }

        ~Win()
        {
            m_nRef = 1;
        }
    };

    static Win _win;
    TMResult lr;
    _win.m_rcWnd = _win.m_rcClient = rc;
    _win.m_pFactoryInfo = m_pFactoryInfo;
    ProcessFrameMessage(&_win, CM_PRINTCLIENT, (TMWParam)gapp, PRF_NONCLIENT, lr);

    CTuiRect rcDraw(rc);
    m_ctrlOBorder.NextRect(&_win, rcDraw);
    gapp->DrawFocusRect(rcDraw);
    m_ctrlCaption.NextRect(m_dwFrameStyle, rcDraw);
    m_ctrlIBorder.NextRect(rcDraw);
    gapp->FrameRect(rcDraw, 1, TMRGB(255,255,0));
    m_ctrlScroll.NextRect(&_win, rcDraw);
    gapp->DrawFocusRect(rcDraw);
    return 0;
}

TMBool CFormManage::EditorMatchID(TMUInt32 n)
{
    return n == m_ctrlOBorder.m_pBk.GetObjectID()
        || n == m_ctrlCaption.m_pBk.GetObjectID()
        || n == m_ctrlCaption.m_ctrlSys.m_pBk.GetObjectID()
        || n == m_ctrlCaption.m_ctrlClose.m_pBk.GetObjectID()
        || n == m_ctrlCaption.m_ctrlMax.m_pBk.GetObjectID()
        || n == m_ctrlCaption.m_ctrlMin.m_pBk.GetObjectID()
        || n == m_ctrlCaption.m_ctrlHelp.m_pBk.GetObjectID()
        || n == m_ctrlIBorder.m_pBk.GetObjectID()
        || n == m_ctrlScroll.m_pBk.GetObjectID()
        || n == m_ctrlScroll.m_pSizeBox.GetObjectID();
}


#endif // TUI_EDIT_MODE



TM_REGISTER_CLASS4(CFormManage, TM_CS(TUI_CLASS_FRM_FORM), true, true);


TM_END_NAMESPACE
