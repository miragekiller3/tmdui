/**
 *  @file
 *  @brief  default pic object
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2012.7.13
 *
 *  Copyright (C) 2012 miragekiller                               
 */

#include "tmgdix.h"
#include "module.h"


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// static helper
static void _CPrePic_DrawRectEdge(IGXApp* gapp, CModule& mdl, TMUInt32 oid, const CTuiRect& rc, TMUInt uEdge, TMUInt flags, CTuiRect* rcAdjust) // VERTEX_FILL* pf, 
{
    int c0,c1,c2,c3; // left top outer, left top inner, right bottom inner, right bottom outer
    int cc;  // center color

    if (flags & BF_MONO)
    {
        c0 = c3 = COLOR_WINDOWFRAME;
        c1 = c2 = COLOR_WINDOW;
        cc = COLOR_WINDOW;
    }
    else if (flags & BF_FLAT)
    {
        c0 = c3 = COLOR_3DSHADOW;
        c1 = c2 = COLOR_3DFACE;
        cc = COLOR_3DFACE;
    }
    else
    {
        if (uEdge & BDR_RAISEDOUTER)
        {
            c0 = ((flags & BF_SOFT) ? COLOR_3DHILIGHT : COLOR_3DLIGHT);
            c3 = COLOR_3DDKSHADOW;
        }
        else if (uEdge & BDR_SUNKENOUTER)
        {
            c0 = ((flags & BF_SOFT) ? COLOR_3DDKSHADOW : COLOR_3DSHADOW);
            c3 = COLOR_3DHILIGHT;
        }
        
        if (uEdge & BDR_RAISEDINNER)
        {
            c1 = ((flags & BF_SOFT) ? COLOR_3DLIGHT : COLOR_3DHILIGHT);
            c2 = COLOR_3DSHADOW;
        }
        else if (uEdge & BDR_SUNKENINNER)
        {
            c1 = ((flags & BF_SOFT) ? COLOR_3DSHADOW : COLOR_3DDKSHADOW);
            c2 = COLOR_3DLIGHT;
        }
        cc = COLOR_3DFACE;
    }

    int nBorder = 2;
    if (!(uEdge & BDR_OUTER))
    {
        --nBorder;
        c0 = c1; c3 = c2;
    }
    if (!(uEdge & BDR_INNER))
    {
        --nBorder;
        c1 = c0; c2 = c3;
    }

    GXDips nh = nBorder;
    GXDips nv = nBorder;
    if (nBorder * 2 > rc.Width())
        nh = rc.Width() / 2.0f;
    if (nBorder * 2 > rc.Height())
        nv = rc.Height() / 2.0f;
    if (nh > 0 && nv > 0)
    {
        TMColor clr[] =
        {
            mdl.ConvertColor(oid, ColorFromSys(c0)),
            mdl.ConvertColor(oid, ColorFromSys(c1)),
            mdl.ConvertColor(oid, ColorFromSys(c2)),
            mdl.ConvertColor(oid, ColorFromSys(c3))
        };
        gapp->DrawRectEdge(rc, nh, nv, clr);
    }
    if (rcAdjust)
        rcAdjust->SetRect(rc.Left() + nh, rc.Top() + nv, rc.Right() - nh, rc.Bottom() - nv);

    if (flags & BF_MIDDLE)
        gapp->FillRect(CTuiRect(rc.Left() + nh, rc.Top() + nv, rc.Right() - nh, rc.Bottom() - nv), mdl.ConvertColor(oid, ColorFromSys(cc)));
}

static int _CPrePic_Frame2State(int nFrame)
{
    int nState = nFrame % e_bsEnd;
    int nRet = 0;
    
    switch (nState)
    {
    case e_bsDisable:
        nRet = DFCS_INACTIVE | DFCS_HOT;
        break;
        
    case e_bsDown:
        nRet = DFCS_PUSHED | DFCS_HOT;
        break;
        
    default: //e_bsHigh:
        nRet = DFCS_HOT;
        break;
    }

    if (nFrame >= e_bsEnd)
        nRet |= DFCS_CHECKED;
    return nRet;
}

static int _CPrePic_DrawPushButton(IGXApp* gapp, CModule& mdl, TMUInt32 oid, int nFrame, const CTuiRect& rc, CTuiRect* rcAdjust = 0)
{
    int uFlags = _CPrePic_Frame2State(nFrame);
    
    TMUInt edge;
    if (uFlags & (DFCS_PUSHED | DFCS_CHECKED | DFCS_FLAT))
        edge = EDGE_SUNKEN;
    else
        edge = EDGE_RAISED;
    
    // BF_MONO, BF_FLAT
    DbgAssertS(DFCS_FLAT == BF_FLAT && DFCS_MONO == BF_MONO && DFCS_HOT == BF_SOFT);
    TMUInt flg = (uFlags & (DFCS_FLAT | DFCS_MONO | DFCS_HOT));// | BF_SOFT; // BF_RECT | 
    
    CTuiRect rcX;
    if (!rcAdjust)
        rcAdjust = &rcX;
    _CPrePic_DrawRectEdge(gapp, mdl, oid, rc, edge, flg, rcAdjust); //BF_MONO | BF_RECT| BF_ADJUST);
    
    TMColor clr = CAppColor::GetColorButtonBk(nFrame);
    if (nFrame >= e_bsEnd)
    {
        nFrame /= e_bsEnd;
        TMColor clr2 = ColorFromSys(eColorIndexBtnHightlight);
        int r = (ColorR(clr) + ColorR(clr2) * nFrame) / (nFrame + 1);
        int g = (ColorR(clr) + ColorG(clr2) * nFrame) / (nFrame + 1);
        int b = (ColorR(clr) + ColorB(clr2) * nFrame) / (nFrame + 1);
        clr = TMRGB(r,g,b);
    }
    gapp->FillRect(*rcAdjust, mdl.ConvertColor(oid, clr)); //  HC_FillRect(g_clrHCButtonBk, *rcAdjust, nFrame);
    
    return uFlags;
}

void _CPrePic_RenderTxt(IGXApp* gapp, CModule& mdl, TMUInt32 oid, const CTuiRect& rc, CText& text, TMColor clr)
{
    gapp->Push(eGXPropFont | eGXPropTextColor);
    gapp->FontSetCurrent(mdl.m_aPrePic.font);
    gapp->SetTextColor(mdl.ConvertColor(oid, clr));
    text->Paint(rc, 0, 0, gapp);
    gapp->Pop();
}

// nFlag :  DFCS_XXX
void _CPrePic_RenderCaptionButFore(IGXApp* gapp, CModule& mdl, TMUInt32 oid, CTuiRect& rc, int flag, int nFrame)
{
    DbgAssert((flag & 0x0f) < tm_countof(mdl.m_aPrePic.txtCaption));
    nFrame %= e_bsEnd;
    if (nFrame == e_bsDown)
        rc.OffsetRect(1, 1);
    TMColor clr = CAppColor::GetColorButtonText(nFrame);
    _CPrePic_RenderTxt(gapp, mdl, oid, rc, mdl.m_aPrePic.txtCaption[flag & 0x0f], clr);
}

void __CPrePic_RenderTxt2(IGXApp* gapp, CModule& mdl, TMUInt32 oid, CTuiRect rc, CText* pText, int flag)
{
    if (DFCS_PUSHED & flag)
    {
        GXDips dx = 1;
        rc.OffsetRect(dx, dx);
    }
    
    int colorIdx = (flag & DFCS_INACTIVE) ? COLOR_BTNSHADOW : COLOR_BTNTEXT;
    TMColor clr = ColorFromSys(colorIdx);
    _CPrePic_RenderTxt(gapp, mdl, oid, rc, pText[flag & 0x0f], clr);
}

void _CPrePic_RenderScroll(IGXApp* gapp, CModule& mdl, TMUInt32 oid, const CTuiRect& rc, int flag)
{
    DbgAssert((flag & 0x0f) < tm_countof(mdl.m_aPrePic.txtScroll));
    __CPrePic_RenderTxt2(gapp, mdl, oid, rc, mdl.m_aPrePic.txtScroll, flag);
}

void _CPrePic_RenderCheckRadio(IGXApp* gapp, CModule& mdl, TMUInt32 oid, const CTuiRect& rc, CText* pText, int flag)
{
    gapp->Push(eGXPropFont | eGXPropTextColor);
    gapp->FontSetCurrent(mdl.m_aPrePic.font);
    int colorIdx = !(flag & (DFCS_INACTIVE | DFCS_PUSHED)) ? COLOR_WINDOW : COLOR_BTNFACE;
    gapp->SetTextColor(mdl.ConvertColor(oid, ColorFromSys(colorIdx)));
    
    pText[4]->Paint(rc, 0, 0, gapp);        // center
    
    if (flag & (DFCS_FLAT | DFCS_MONO))
    {
        gapp->SetTextColor(mdl.ConvertColor(oid, ColorFromSys(COLOR_WINDOWFRAME)));
        pText[0]->Paint(rc, 0, 0, gapp);    // Outer right
        pText[1]->Paint(rc, 0, 0, gapp);    // Outer left
        pText[2]->Paint(rc, 0, 0, gapp);    // inner left
        pText[3]->Paint(rc, 0, 0, gapp);    // inner right
    }
    else
    {
        gapp->SetTextColor(mdl.ConvertColor(oid, ColorFromSys(COLOR_BTNSHADOW)));
        pText[0]->Paint(rc, 0, 0, gapp);    // Outer right
        gapp->SetTextColor(mdl.ConvertColor(oid, ColorFromSys(COLOR_BTNHIGHLIGHT)));
        pText[1]->Paint(rc, 0, 0, gapp);    // Outer left
        gapp->SetTextColor(mdl.ConvertColor(oid, ColorFromSys(COLOR_3DDKSHADOW)));
        pText[2]->Paint(rc, 0, 0, gapp);    // inner left
        gapp->SetTextColor(mdl.ConvertColor(oid, ColorFromSys(COLOR_3DLIGHT)));
        pText[3]->Paint(rc, 0, 0, gapp);    // inner right
    }
    
    if (flag & DFCS_CHECKED)
    {
        gapp->SetTextColor(mdl.ConvertColor(oid, ColorFromSys(COLOR_WINDOWTEXT)));
        pText[5]->Paint(rc, 0, 0, gapp);    // checked
    }
    
    gapp->Pop();
}

static void _CPrePic_Init(CText* pText, int n)
{
    for (int i = 0; i < n; ++i)
        pText[i]->SetFormat(DT_VCENTER | DT_CENTER | DT_SINGLELINE);// | DT_AUTOFIT);
}


///////////////////////////////////////////////////////////////////////////////
// CPrePicList
void CPrePicList::InitPreRes()
{
    font.SetName(TM_T("marlett"));
    font.height = 14;
    font.style = 0;
    font.styleMask = GX_FONT_MASK_STYLE;
    
    txtCaption[DFCS_CAPTIONCLOSE].Create(TM_CT("r"));     // DFCS_CAPTIONCLOSE
    txtCaption[DFCS_CAPTIONHELP].Create(TM_CT("s"));      // DFCS_CAPTIONHELP
    txtCaption[DFCS_CAPTIONMIN].Create(TM_CT("0"));       // DFCS_CAPTIONMIN
    txtCaption[DFCS_CAPTIONMAX].Create(TM_CT("1"));       // DFCS_CAPTIONMAX
    txtCaption[DFCS_CAPTIONRESTORE].Create(TM_CT("2"));   // DFCS_CAPTIONRESTORE
    
    txtScroll[DFCS_SCROLLUP].Create(TM_CT("5"));          // DFCS_SCROLLUP
    txtScroll[DFCS_SCROLLDOWN].Create(TM_CT("6"));        // DFCS_SCROLLDOWN
    txtScroll[DFCS_SCROLLLEFT].Create(TM_CT("3"));        // DFCS_SCROLLLEFT
    txtScroll[DFCS_SCROLLRIGHT].Create(TM_CT("4"));       // DFCS_SCROLLRIGHT
    
    txtRadio[0].Create(TM_CT("j"));                       // Outer right
    txtRadio[1].Create(TM_CT("k"));                       // Outer left
    txtRadio[2].Create(TM_CT("l"));                       // inner left
    txtRadio[3].Create(TM_CT("m"));                       // inner right
    txtRadio[4].Create(TM_CT("n"));                       // center
    txtRadio[5].Create(TM_CT("i"));                       // checked
    
    txtCheck[0].Create(TM_CT("e"));                       // Outer right
    txtCheck[1].Create(TM_CT("d"));                       // Outer left
    txtCheck[2].Create(TM_CT("e"));                       // inner left
    txtCheck[3].Create(TM_CT("f"));                       // inner right
    txtCheck[4].Create(TM_CT("g"));                       // center
    txtCheck[5].Create(TM_CT("b"));                       // checked
    
    txtCombo.Create(TM_CT("9"));                          // combo
    
    _CPrePic_Init(txtCaption,   tm_countof(txtCaption));
    _CPrePic_Init(txtScroll,    tm_countof(txtScroll));
    _CPrePic_Init(txtRadio,     tm_countof(txtRadio));
    _CPrePic_Init(txtCheck,     tm_countof(txtCheck));
    _CPrePic_Init(txtCombo.GetThis(), 1);
}


///////////////////////////////////////////////////////////////////////////////
// CPrePicProxyT<T>
template <int nID> void CPrePicProxyT<nID>::GetSize(TMSize& sz)
{
    if (nID == IDP_PRE_PIC_CHECK ||
        nID == IDP_PRE_PIC_RADIO ||
        nID == IDP_PRE_PIC_COMB ||
        nID == IDP_PRE_PIC_HEADER_UP ||
        nID == IDP_PRE_PIC_HEADER_DOWN ||
        nID == IDP_PRE_PIC_UP ||
        nID == IDP_PRE_PIC_DOWN)
    {
        SizeSetWidth(sz, GetSystemMetrics(SM_CXMENUCHECK));
        SizeSetHeight(sz, GetSystemMetrics(SM_CYMENUCHECK));
    }
}

void CPrePicProxyT<IDP_PRE_PIC_NULL>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
}

void CPrePicProxyT<IDP_PRE_PIC_BUTTON>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    if (fx && (fx->nNewState & e_bssFocus))
    {
        CTuiRect rcx(rc);
        TMColor clr = m_mdl.ConvertColor(IDP_PRE_PIC_BUTTON, ColorFromSys(eColorIndexWindowFrame));
        gapp->FrameRect(rcx, 1, clr);
        rcx.InflateRect(-1, -1);
        _CPrePic_DrawPushButton(gapp, m_mdl, IDP_PRE_PIC_BUTTON, nState, rcx);
    }
    else
        _CPrePic_DrawPushButton(gapp, m_mdl, IDP_PRE_PIC_BUTTON, nState, rc);
}

static void _frm_btn_render(IGXApp* gapp, TMUInt nFrmType, CModule& mdl, TMUInt32 oid, int nFrame, const CTuiRect& rcx)
{
    int n = (nFrame / e_bsEnd) % 2;
    int nState = _CPrePic_DrawPushButton(gapp, mdl, oid, (nFrame % e_bsEnd) % 3, rcx);
    
    CTuiRect rc(rcx);
    int nFlag = (nFrmType == DFCS_CAPTIONMAX && n) ? DFCS_CAPTIONRESTORE : nFrmType;
    _CPrePic_RenderCaptionButFore(gapp, mdl, oid, rc, nFlag | nState, nFrame);
}

void CPrePicProxyT<IDP_PRE_PIC_CLOSE>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    _frm_btn_render(gapp, DFCS_CAPTIONCLOSE, m_mdl, IDP_PRE_PIC_CLOSE, nState, rc);
}

void CPrePicProxyT<IDP_PRE_PIC_MIN>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    _frm_btn_render(gapp, DFCS_CAPTIONMIN, m_mdl, IDP_PRE_PIC_MIN, nState, rc);
}

void CPrePicProxyT<IDP_PRE_PIC_MAX>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    _frm_btn_render(gapp, DFCS_CAPTIONMAX, m_mdl, IDP_PRE_PIC_MAX, nState, rc);
}

void CPrePicProxyT<IDP_PRE_PIC_HELP>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    _frm_btn_render(gapp, DFCS_CAPTIONHELP, m_mdl, IDP_PRE_PIC_HELP, nState, rc);
}

void CPrePicProxyT<IDP_PRE_PIC_CHECK>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    int nRet = _CPrePic_Frame2State(nState);
    _CPrePic_RenderCheckRadio(gapp, m_mdl, IDP_PRE_PIC_CHECK, rc, m_mdl.m_aPrePic.txtCheck, nRet);
}

void CPrePicProxyT<IDP_PRE_PIC_RADIO>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    int nRet = _CPrePic_Frame2State(nState);
    _CPrePic_RenderCheckRadio(gapp, m_mdl, IDP_PRE_PIC_RADIO, rc, m_mdl.m_aPrePic.txtRadio, nRet);
}

void CPrePicProxyT<IDP_PRE_PIC_COMB>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    TMColor clr = CAppColor::GetColorButtonText(nState);// TColorTraits::get(nState);
    _CPrePic_RenderTxt(gapp, m_mdl, IDP_PRE_PIC_COMB, rc, m_mdl.m_aPrePic.txtCombo, clr);
}

template <class F>
static void _ListItem_Render(IGXApp* gapp, CModule& mdl, TMUInt32 oid, int nFrame, const CTuiRect& rc, F _f)
{
    if (nFrame == e_bsNormal)
        return;
    int nState = nFrame % e_bsEnd;
    TMColor nColor = mdl.ConvertColor(oid, _f(nState));// pColor[nState];
    gapp->FillRect(rc, nColor);
    if (nState == e_bsFocus)
        gapp->DrawFocusRect(rc);
}

void CPrePicProxyT<IDP_PRE_PIC_LIST_ITEM>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    _ListItem_Render(gapp, m_mdl, IDP_PRE_PIC_LIST_ITEM, nState, rc, CAppColor::GetColorListTreeBk);
}

void CPrePicProxyT<IDP_PRE_PIC_MENU>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    // border
    int nRet = 0;
    CTuiRect rcAdjust;
    switch (nState)
    {
    case e_bsHigh: nRet = BDR_RAISEDINNER; break;
    case e_bsDown: nRet = BDR_SUNKENINNER; break;
    default: return;
    }
    _CPrePic_DrawRectEdge(gapp, m_mdl, IDP_PRE_PIC_MENU, rc, nRet, 0, &rcAdjust);
    
    // bk
    gapp->FillRect(rcAdjust, m_mdl.ConvertColor(IDP_PRE_PIC_MENU, CAppColor::GetColorMenuItemBk(nState)));
}

void CPrePicProxyT<IDP_PRE_PIC_MENU_POP>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    _ListItem_Render(gapp, m_mdl, IDP_PRE_PIC_MENU_POP, nState, rc, CAppColor::GetColorMenuItemBk);
}

void CPrePicProxyT<IDP_PRE_PIC_HEADER_UP>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    TMColor clr = CAppColor::GetColorButtonText(e_bsDisable);
    _CPrePic_RenderTxt(gapp, m_mdl, IDP_PRE_PIC_HEADER_UP, rc, m_mdl.m_aPrePic.txtScroll[DFCS_SCROLLUP], clr);
}

void CPrePicProxyT<IDP_PRE_PIC_HEADER_DOWN>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    TMColor clr = CAppColor::GetColorButtonText(e_bsDisable);
    _CPrePic_RenderTxt(gapp, m_mdl, IDP_PRE_PIC_HEADER_DOWN, rc, m_mdl.m_aPrePic.txtScroll[DFCS_SCROLLDOWN], clr);
}

void CPrePicProxyT<IDP_PRE_PIC_UP>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    _CPrePic_DrawPushButton(gapp, m_mdl, IDP_PRE_PIC_BUTTON, nState, rc);
    _CPrePic_RenderScroll(gapp, m_mdl, IDP_PRE_PIC_DOWN, rc, nState == e_bsDown ? (DFCS_PUSHED | DFCS_SCROLLUP) : DFCS_SCROLLUP);
}

void CPrePicProxyT<IDP_PRE_PIC_DOWN>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    _CPrePic_DrawPushButton(gapp, m_mdl, IDP_PRE_PIC_BUTTON, nState, rc);
    _CPrePic_RenderScroll(gapp, m_mdl, IDP_PRE_PIC_DOWN, rc, nState == e_bsDown ? (DFCS_PUSHED | DFCS_SCROLLDOWN) : DFCS_SCROLLDOWN);
}

void CPrePicProxyT<IDP_PRE_PIC_SCROLLBAR>::RenderSB(const SCROLLSTATE& state, const CTuiRect& rc, WPRENDERFX_SB* fx, IGXApp* gapp)
{
    EHitObj hid = state.obj;
    GXDips dt[6];
    dt[0] = 0;                                                          // up button
    dt[1] = state.nButtonHeight;                                        // up scroll line
    dt[2] = state.nThumbTop;                                            // thumb top
    dt[3] = state.nThumbBottom;                                         // down scroll line
    dt[5] = hid == e_objVScroll ? rc.Height() : rc.Width();
    dt[4] = dt[5] - state.nButtonHeight;                                // down button

    int nState[5] = { e_bsNormal, e_bsDown, e_bsNormal, e_bsDisable, e_bsHigh };
    int nFlag[5] = { 0, DFCS_PUSHED, 0, DFCS_INACTIVE, DFCS_HOT };

#   define _SBLT_V(n,c) \
    rcDest[n].SetRect(rc.Left(), rc.Top() + dt[n], rc.Right(), rc.Top() + dt[n + 1]);
#   define _SBLT_H(n,c) \
    rcDest[n].SetRect(rc.Left() + dt[n], rc.Top(), rc.Left() + dt[n + 1], rc.Bottom());

    CRectL rcDest[5];
    int n1;
    int n2;
    if (hid == e_objVScroll)
    {
        n1 = DFCS_SCROLLUP;
        n2 = DFCS_SCROLLDOWN;
        _SBLT_V(0, SB_LINEUP);
        _SBLT_V(1, SB_PAGEUP);
        _SBLT_V(2, SB_THUMBPOSITION);
        _SBLT_V(3, SB_PAGEDOWN);
        _SBLT_V(4, SB_LINEDOWN);
    }
    else
    {
        n1 = DFCS_SCROLLLEFT;
        n2 = DFCS_SCROLLRIGHT;
        _SBLT_H(0, SB_LINEUP);
        _SBLT_H(1, SB_PAGEUP);
        _SBLT_H(2, SB_THUMBPOSITION);
        _SBLT_H(3, SB_PAGEDOWN);
        _SBLT_H(4, SB_LINEDOWN);
    }
#undef _SBLT_V
#undef _SBLT_H

    CTuiRect rcCalc;

    // SB_LINEUP
    if (!(state.eState[SB_LINEUP] & e_bssHide))
    {
        int nn = ButStates2ButFrame(state.eState[SB_LINEUP]);
        _CPrePic_DrawPushButton(gapp, m_mdl, IDP_PRE_PIC_SCROLLBAR, nState[nn], rcDest[0], &rcCalc);
        _CPrePic_RenderScroll(gapp, m_mdl, IDP_PRE_PIC_SCROLLBAR, rcCalc, nFlag[nn] | n1);
    }
    
    // SB_PAGEUP:
    if (!(state.eState[SB_PAGEUP] & e_bssHide))
        gapp->FillRect(rcDest[1], m_mdl.ConvertColor(IDP_PRE_PIC_SCROLLBAR, CAppColor::GetColorScrollBk(0)));// HC_RenderBk(rcDest[1], COLOR_SCROLLBAR);

    // SB_THUMBPOSITION:
    if (!(state.eState[SB_THUMBPOSITION] & e_bssHide) && !rcDest[2].IsRectEmpty())
        _CPrePic_DrawPushButton(gapp, m_mdl, IDP_PRE_PIC_SCROLLBAR, e_bsNormal, rcDest[2]);

    // SB_PAGEDOWN:
    if (!(state.eState[SB_PAGEDOWN] & e_bssHide))
        gapp->FillRect(rcDest[3], m_mdl.ConvertColor(IDP_PRE_PIC_SCROLLBAR, CAppColor::GetColorScrollBk(0))); //HC_RenderBk(rcDest[3], COLOR_SCROLLBAR);

    // SB_LINEDOWN:
    if (!(state.eState[SB_LINEDOWN] & e_bssHide))
    {
        int nn = ButStates2ButFrame(state.eState[SB_LINEDOWN]);
        _CPrePic_DrawPushButton(gapp, m_mdl, IDP_PRE_PIC_SCROLLBAR, nState[nn], rcDest[4], &rcCalc);
        _CPrePic_RenderScroll(gapp, m_mdl, IDP_PRE_PIC_SCROLLBAR, rcCalc, nFlag[nn] | n2);
    }
}

void CPrePicProxyT<IDP_PRE_PIC_CAPTION>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    int nFrom, nTo;
    if (nState)
    {
        nFrom   = COLOR_ACTIVECAPTION;
        nTo     = COLOR_GRADIENTACTIVECAPTION;
    }
    else
    {
        nFrom   = COLOR_INACTIVECAPTION;
        nTo     = COLOR_GRADIENTINACTIVECAPTION;
    }
    TMColor clrfrom = m_mdl.ConvertColor(IDP_PRE_PIC_CAPTION, ColorFromSys(nFrom));
    TMColor clrTo   = m_mdl.ConvertColor(IDP_PRE_PIC_CAPTION, ColorFromSys(nTo));
    
    gapp->GrandientFill(rc, clrfrom, clrTo, e_gfNormal0);
}

void CPrePicProxyT<IDP_PRE_PIC_INNERBORDER>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    _CPrePic_DrawRectEdge(gapp, m_mdl, IDP_PRE_PIC_INNERBORDER, rc, EDGE_SUNKEN, 0, 0);
    if (nState == e_bsFocus)
        gapp->FrameRect(rc, 1, m_mdl.ConvertColor(0, ColorFromSys(COLOR_HOTLIGHT)));
}

void CPrePicProxyT<IDP_PRE_PIC_OUTERBORDER>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    CTuiRect rc2;
    _CPrePic_DrawRectEdge(gapp, m_mdl, IDP_PRE_PIC_OUTERBORDER, rc, EDGE_RAISED, BF_RECT, &rc2);
    gapp->FillRect(rc2, m_mdl.ConvertColor(IDP_PRE_PIC_OUTERBORDER, CAppColor::GetColorButtonBk(0)));
}

void CPrePicProxyT<IDP_PRE_PIC_SIZEBOX>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    gapp->FillRect(rc, m_mdl.ConvertColor(IDP_PRE_PIC_SIZEBOX, CAppColor::GetColorButtonBk(0)));
}

void CPrePicProxyT<IDP_PRE_PIC_GROUP>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    TMUInt uFlag = nState >= e_bsEnd ? BF_RECT | BF_FLAT : BF_RECT;
    _CPrePic_DrawRectEdge(gapp, m_mdl, IDP_PRE_PIC_GROUP, rc, EDGE_ETCHED, uFlag, 0); //BF_MONO | BF_RECT| BF_ADJUST);
}

void CPrePicProxyT<IDP_PRE_PIC_FOCUS>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    gapp->DrawFocusRect(rc);
}


TM_END_NAMESPACE


