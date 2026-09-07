/*
 *  @file
 *  @brief  progress
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.11.26
 *
 *  Copyright (C) 2019 miragekiller
 */

#include "uiprogress.h"


TM_BGN_NAMESPACE


// ITuiPic
void CTuiProgressImpl::CSBPic::RenderSB(const SCROLLSTATE& state, const CTuiRect& rc, WPRENDERFX_SB* fx, IGXApp* gapp)
{
    if (!m_pBtn)
        return;

    CTuiProgressImpl* p = TM_OTHIS(CTuiProgressImpl, m_sbPic);
    CTuiRect rcTmp(rc);
    if (p->IsStyleVertical())
        rcTmp.Rotate();
    p->RenderFore(gapp, state.nThumbTop, rcTmp);
}


////////////////////////////////////////////////////////////////////////////////
// CTuiProgressImpl
TM_REGISTER_CLASS1(CTuiProgress);

CTuiProgressImpl::CTuiProgressImpl()
{
    m_sb.m_pBk = &m_sbPic;
    m_sb.m_nButtonHeight = 0;
    m_sb.m_nThumbMin = 0;
}

CTuiProgressImpl::~CTuiProgressImpl ()
{
}

void CTuiProgressImpl::Init()
{
    CTuiRect rc;
    GetPaddingRect(&rc);
    if (IsStyleVertical())
        m_sb.m_nBarWidth = rc.Width();
    else
        m_sb.m_nBarWidth = rc.Height();
}

TMResult CTuiProgressImpl::OnInit(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Init();
    bHandled = false;
    return 0;
}

TMResult CTuiProgressImpl::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_sbPic.m_pBtn.Release();
    bHandled = false;
    return 0;
}

TMResult CTuiProgressImpl::OnZoomed(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_sbPic.m_pBtn.ChangeDPI(wParam);
    bHandled = false;
    return 0;
}

// CTuiWgt
void CTuiProgressImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    CTuiRect rc;
    GetClientRect(&rc);
    
    CWinState ws(this);
    ws.RenderBk(gapp, rc);
    
    ClientRectToPaddingRect(rc);
    m_sb.Paint(GetFS(), gapp, rc, true);
}


void CTuiProgressImpl::RenderFore(IGXApp* gapp, GXDips nLen, CTuiRect& rc)
{
//     GXDips nLen = MulDiv (r - l, m_nPos - m_nMin, m_nMax - m_nMin);
    GXDips oldr = rc.Right();
    if ((!(m_dwStyle & PBS_SMOOTH)) && (m_nStep > 0))
        nLen = (nLen / m_nStep) * m_nStep;
    rc.SetWidth(nLen);
    if (gapp->ClipAndPush(rc, true, eGXPropClip))
    {
        if (m_dwStyle & PBS_SLIDE)
            rc.KeepRSetW(oldr);// l += (r - oldr);
        else if (!(m_dwStyle & PBS_SCALE))
            rc.SetRight(oldr);// r = oldr;
        if (IsStyleVertical())
            rc.Rotate();
        m_sbPic.m_pBtn->Render(0, 0, rc, 0, gapp);//GetModule()->GetVisual());
        gapp->Pop();
    }
}


TM_END_NAMESPACE
