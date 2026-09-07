/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.14
 *
 *  Copyright (C) 2006 miragekiller
 */

#include "uistatic.h"
#include "view.h"


TM_BGN_NAMESPACE



////////////////////////////////////////////////////////////////////////////////
// class CTuiStaticImpl
EButStates CTuiStaticImpl::DoGetStates(HTUI hFocus, HTUI hOver, TMBool bMouseDown)
{
    if (m_nCurChecks < 0)
        return GetParent()->DoGetStates(hFocus, hOver, bMouseDown);

    EButStates state = CTuiWgt::DoGetStates(hFocus, hOver, bMouseDown);
    state |= (EButStates)m_nCurChecks;

    return state;
}

void CTuiStaticImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    CTuiRect rect;
    GetPaddingRect(&rect);

    if (Is_WS_PRI_DIRTY() && (IsAutoWidth() || IsAutoHeight()))
    {
        Reset_WS_PRI_DIRTY();
        GXDips w = rect.Width();
        GXDips h = rect.Height();
        CalcTextRect(gapp, w, h, GetMax(0));
        SendScrollRangeMessage(&w, &h);
    }

    //set text color
    {
        CPropColor clrs[] = { m_clrNormal, m_clrNormal, m_clrNormal, GetView()->m_clrDisableText, m_clrNormal };
        CPropColor* clrss[] = { clrs, 0 };
        RenderSetTextColor(gapp, GetButtonState(), clrss, CAppColor::GetColorStaticText);
    }

    if (m_nCurChecks > 0)
        RenderStaticBk(gapp, m_nCurChecks);
    else
    {
        EButStates ss = GetStates();
        RenderStaticBk(gapp, ButStates2ButFrames(ss), &ss);
    }

    if (m_gxText)
    {
        m_gxText->SetFormat(m_uFormat);
        m_gxText->Paint(rect, 0, 0, gapp);
    }
}

// CTuiWgt
TMResult CTuiStaticImpl::OnSetTextColor (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Invalidate();
    m_clrNormal = lParam;
    return 0;
}

TMResult CTuiStaticImpl::OnSetCheck(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
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

int CTuiStaticImpl::GetCheck() const
{
    return m_nCurChecks < 0 ? GetParent()->GetButtonState() : m_nCurChecks;
}


TM_REGISTER_CLASS1 (CTuiStatic);


//////////////////////////////////////////////////////////////////////////////////////
// CTuiGroupBoxImpl
CTuiGroupBoxImpl::CTuiGroupBoxImpl()
{
    Set_WS_PRI_ALIGNAUTOSIZE();
}

CTuiGroupBoxImpl::~CTuiGroupBoxImpl()
{
}

void CTuiGroupBoxImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    // text color
    {
        EButState ebs = IsWindowEnabled() ? e_bsNormal : e_bsDisable;
        CPropColor clrs[] = { m_clrNormal, m_clrNormal, m_clrNormal, GetView()->m_clrDisableText, m_clrNormal };
        CPropColor* clrss[] = { clrs, 0 };
        RenderSetTextColor(gapp, ebs, clrss, CAppColor::GetColorStaticText);
    }
    
    // text
    CTuiRect rc;
    GetClientRect(&rc);
    CTuiRect rc2(rc);
    rc2.OffsetLeft(7);
    rc2.OffsetRight(-7);
    rc2.OffsetTop(1);
    int nLen = 0;
    if (m_gxText)
    {
        nLen = m_gxText->GetLength();
        m_gxText->SetFormat(DT_SINGLELINE | DT_NOPREFIX | DT_WORD_ELLIPSIS);
        m_gxText->Paint(rc2, 0, &rc2, gapp);
    }
    else
        rc2.SetHeight(0);
    
    // draw the box
    if (m_pBk)
    {
        rc.SetTop((rc2.Top() + rc2.Bottom()) / 2);
        
        if (nLen)
        {
            CTuiRect rcc[3]; //left, right, bottom
            rcc[0].SetRect(rc.Left(), rc.Top(), rc2.Left() - 1, rc.Bottom());
            rcc[1].SetRect(rc2.Right() + 1, rc.Top(), rc.Right(), rc.Bottom());
            rcc[2].SetRect(rc2.Left() - 1, rc2.Bottom(), rc2.Right() + 1, rc.Bottom());
            for (int i = 0; i < 3; ++i)
            {
                if (gapp->ClipAndPush(rcc[i], true, eGXPropClip))
                {
                    m_pBk->Render(GetButtonState(), 0, rc, 0, gapp);
                    gapp->Pop();
                }
            }
        }
        else
            m_pBk->Render(GetButtonState(), 0, rc, 0, gapp);
    }
}

TM_REGISTER_CLASS1 (CTuiGroupBox);


TM_END_NAMESPACE
