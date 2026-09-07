/*
 *  @file
 *  @brief  track bar
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.11.22
 *
 *  Copyright (C) 2019 miragekiller
 */

#include "uitrackbar.h"


TM_BGN_NAMESPACE


// ITuiPic
void CTuiTrackBarImpl::CSBPic::RenderSB(const SCROLLSTATE& state, const CTuiRect& rc, WPRENDERFX_SB* fx, IGXApp* gapp)
{
    CTuiTrackBarImpl* p = TM_OTHIS(CTuiTrackBarImpl, m_sbPic);
    CTuiRect rcTmp1(rc), rcTmp2(rc), rcTmp3(rc);
    
    if (p->IsStyleVertical())
    {
        rcTmp2.OffsetTop(state.nThumbTop);
        rcTmp2.KeepTSetH(state.nThumbBottom - state.nThumbTop);
        GXDips x = (rcTmp2.Top() + rcTmp2.Bottom()) / 2.0f;
        rcTmp1.KeepTSetB(x);
        rcTmp3.KeepBSetT(x);
    }
    else
    {
        rcTmp2.OffsetLeft(state.nThumbTop);
        rcTmp2.KeepLSetW(state.nThumbBottom - state.nThumbTop);
        GXDips x = (rcTmp2.Left() + rcTmp2.Right()) / 2.0f;
        rcTmp1.KeepLSetR(x);
        rcTmp3.KeepRSetL(x);
    }
    
    if (m_pLeft)
        m_pLeft->Render(SCROLLSTATE::SS2BS(state.eState[SB_PAGEUP]), 0, rcTmp1, 0, gapp);
    if (m_pRight)
        m_pRight->Render(SCROLLSTATE::SS2BS(state.eState[SB_PAGEDOWN]), 0, rcTmp3, 0, gapp);
    if (m_pBtn)
        m_pBtn->Render(SCROLLSTATE::SS2BS(state.eState[SB_THUMBPOSITION]), 0, rcTmp2, 0, gapp);
}


////////////////////////////////////////////////////////////////////////////////
// CTuiTrackBarImpl
TM_REGISTER_CLASS1(CTuiTrackBar);

CTuiTrackBarImpl::CTuiTrackBarImpl ()
    : m_nBtnPicWidth(0)
{
    m_sb.m_pBk = &m_sbPic;
    m_sb.m_nButtonHeight = 0;
}

CTuiTrackBarImpl::~CTuiTrackBarImpl ()
{
}

void CTuiTrackBarImpl::Init()
{
    CTuiSize sz;
    if (!m_nBtnPicWidth && m_sbPic.m_pBtn)
    {
        CSizeD szd(0, 0);
        m_sbPic.m_pBtn->GetSize(szd);
        CZoom(GetDPI()).DPtoLP(szd, sz);
    }
    else
        sz.SetSize(m_nBtnPicWidth, m_nBtnPicWidth);

    CTuiRect rc;
    GetPaddingRect(&rc);
    if (IsStyleVertical())
       m_sb.m_nBarWidth = rc.Width(), m_sb.m_nThumbMin = sz.Height();
//         m_frm.SetScrollProp(rc.Width(), 0, sz.Height(), &m_sbPic);
    else
       m_sb.m_nBarWidth = rc.Height(), m_sb.m_nThumbMin = sz.Width();
//         m_frm.SetScrollProp(rc.Height(), 0, sz.Width(), &m_sbPic);
}

TMResult CTuiTrackBarImpl::OnInit(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Init();
    bHandled = false;
    return 0;
}

TMResult CTuiTrackBarImpl::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_sbPic.m_pLeft.Release();
    m_sbPic.m_pRight.Release();
    m_sbPic.m_pBtn.Release();
    bHandled = false;
    return 0;
}

TMResult CTuiTrackBarImpl::OnZoomed(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_sbPic.m_pLeft.ChangeDPI(wParam);
    m_sbPic.m_pRight.ChangeDPI(wParam);
    m_sbPic.m_pBtn.ChangeDPI(wParam);
    bHandled = false;
    return 0;
}

TMResult CTuiTrackBarImpl::OnSetLeftPic(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    LoadPicAndInvalidate(m_sbPic.m_pLeft, wParam, lParam);
    return S_OK;
}

TMResult CTuiTrackBarImpl::OnSetRightPic(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    LoadPicAndInvalidate(m_sbPic.m_pRight, wParam, lParam);
    return S_OK;
}

TMResult CTuiTrackBarImpl::OnSetTrackBtnPic(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    LoadPicAndInvalidate(m_sbPic.m_pBtn, wParam, lParam);
    Init();
    return S_OK;
}

// CTuiWgt
void CTuiTrackBarImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    CTuiRect rc;
    GetClientRect(&rc);

    CWinState ws(this);
    ws.RenderBk(gapp, rc);
    
    ClientRectToPaddingRect(rc);
    m_sb.Paint(GetFS(), gapp, rc, true);
    
    if (IsRenderFocusRect())
        RenderFocusRect(gapp, m_rcWnd);
}

TM_END_NAMESPACE
