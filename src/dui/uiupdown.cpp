/*
 *  @file
 *  @brief updown
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.11.26
 *
 *  Copyright (C) 2019 miragekiller
 */

#include "uiupdown.h"

TM_BGN_NAMESPACE

// ITuiPic
void CTuiUpDownImpl::CSBPic::RenderSB(const SCROLLSTATE& state, const CTuiRect& rc, WPRENDERFX_SB* fx, IGXApp* gapp)
{
    CTuiUpDownImpl* p = TM_OTHIS(CTuiUpDownImpl, m_sbPic);
    CTuiRect rcTmp1(rc), rcTmp2(rc);
    
    if (p->IsStyleHorz())
    {
        GXDips x = rc.Width() / 2.0f;
        rcTmp1.KeepLSetW(x);
        rcTmp2.KeepRSetW(x);

//         _gxApp->Push(eGXPropAffine);
//         CAffine aff;
//         aff.RotateR(CFloat::PI() / 2, rcTmp1.CenterPointX(), rcTmp1.CenterPointY());
//         _gxApp->SetTransform(&aff, 0, 0, eGXTransformLeftMultiply);
    }
    else
    {
        GXDips x = rc.Height() / 2.0f;
        rcTmp1.KeepTSetH(x);
        rcTmp2.KeepBSetH(x);
    }
    
    if (m_pUp)
        m_pUp->Render(SCROLLSTATE::SS2BS(state.eState[SB_LINEUP]), 0, rcTmp1, 0, gapp);
    if (m_pDown)
        m_pDown->Render(SCROLLSTATE::SS2BS(state.eState[SB_LINEDOWN]), 0, rcTmp2, 0, gapp);

//     if (p->IsStyleHorz())
//         _gxApp->Pop();
}

///////////////////////////////////////////////////////////////////////////////
// CTuiUpDownCtrlImpl

TM_REGISTER_CLASS1(CTuiUpDown);

CTuiUpDownImpl::CTuiUpDownImpl()
{
    m_sb.m_pBk = &m_sbPic;
    m_sb.m_nThumbMin = 0;
}

CTuiUpDownImpl::~CTuiUpDownImpl ()
{
}

void CTuiUpDownImpl::Init()
{
    CTuiSize sz;
    CTuiRect rc;
    GetPaddingRect(&rc);
    if (IsStyleHorz())
        m_sb.m_nBarWidth = rc.Height(), m_sb.m_nButtonHeight = rc.Width() / 2.0f;
    else
        m_sb.m_nBarWidth = rc.Width(), m_sb.m_nButtonHeight = rc.Height() / 2.0f;
}

TMResult CTuiUpDownImpl::OnInit(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Init();
    bHandled = false;
    return 0;
}

TMResult CTuiUpDownImpl::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_sbPic.m_pUp.Release();
    m_sbPic.m_pDown.Release();
    bHandled = false;
    return 0;
}

TMResult CTuiUpDownImpl::OnZoomed(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_sbPic.m_pUp.ChangeDPI(wParam);
    m_sbPic.m_pDown.ChangeDPI(wParam);
    bHandled = false;
    return 0;
}

// CTuiScrollObj
HTUI CTuiUpDownImpl::GetTarget()
{
    HTUI hWnd = CTuiScrollObj::GetTarget();
    if (hWnd)
        return hWnd;
    return IsStyleAutoBuddy() ? GetWindow(GW_HWNDPREV) : 0;
}

int CTuiUpDownImpl::SyncNewPos(int step)
{
    HTUI hWnd = GetTarget();
    if (hWnd)
    {
        CString str;
        CTuiWidget(hWnd).GetWindowText(str);
        m_nPos = str.ToLong(m_nPos);
    }
    return m_nPos - step;
}


// CTuiWgt
void CTuiUpDownImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    CTuiRect rc;
    GetClientRect(&rc);

    CWinState ws(this);
    ws.RenderBk(gapp, rc);
    
    ClientRectToPaddingRect(rc);
    m_sb.Paint(GetFS(), gapp, rc, false);
    
    if (IsRenderFocusRect())
        RenderFocusRect(gapp, m_rcWnd);
}

TM_END_NAMESPACE
