/*
 *  @file
 *  @brief  layer
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.8.25
 *
 *  Copyright (C) 2021 miragekiller
 */

#include "uilayer.h"


TM_BGN_NAMESPACE

CTuiLayerBase::CTuiLayerBase()
    : m_alpha(255)
{
    TM_IF_DBG(m_nCountRepaint = 0);
    Set_WS_PRI_ALIGNAUTOSIZE();
}

// void CTuiLayerBase::GetDeviceDirtyRect(CRectI& rc)
// {
//     CZoom wz(GetDPI());
//     CTuiRect rcDirty(m_rcDirty.GetDirty());
//     rcDirty.OffsetRect(-GetLeft(), -GetTop());
//     wz.LPtoDPCeil(rcDirty, rc);
// }


TMResult CTuiLayerBase::OnSetAlpha(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (m_alpha != (TMUInt8)lParam)
        m_alpha = (TMUInt8)lParam;
    return 0;
}

ERenderBK CTuiLayerBase::RenderBkBuffer(IGXApp* gapp)
{
    CRect rc;
    CTuiZoom dpi(GetDPI());
    CTuiRect rcOld(m_rcWnd);
    dpi.LPtoDP(rcOld, rc);
    CSize szd(rc.Width(), rc.Height());
    
    if (m_txBuffer)
    {
        if (szd.Width() != m_txBuffer->GetWidth() || szd.Height() != m_txBuffer->GetHeight())
        {
            if (!m_txBuffer->Resize(szd.Width(), szd.Height()))
                return m_txBuffer.Release(), eRenderBkAbort;
        }
    }
    else if (szd.Width() <= 0 || szd.Height() <= 0)
        return eRenderBkOk;
    else
    {
        CreateBkBuffer(gapp, szd);
        if (!m_txBuffer)
            return eRenderBkAbort;
    }

    {
    CAutoTextureRender render(gapp); //(m_txBuffer);
    {
        CPoint ptWinD;
        CTuiPoint ptWinL;
        dpi.LPtoDP(tm_addressof(m_rcWnd.TopLeft()), 1, &ptWinD);
        dpi.DPtoLP(&ptWinD, 1, &ptWinL);
        CLocal lc = GetModule()->GetLocal();
        if (eGXRetSuccess != render.SetMapping(m_txBuffer, ptWinL, GetDPI(), lc))
            return eRenderBkAbort;

        GXFont ft;
        GetRealFont(ft);
        render->FontSetCurrent(ft);
    }
//      gxSetMapping(GXPoint(0,0), m_rcWnd.TopLeft(), GetDPI(), lc);
    
    TMLParam lp = c_lpPrint;
    int i = 0;
    for (; ; ++i)
    {
        TM_IF_DBG(m_nCountRepaint = i);

        //             CCycle cy;
        //         DbgTrace ("bp.DrawBk(%d)-%d: owner:%x, WH:%d,%d", i, GetTickCount(), this, w, h);
        render->ClipAndPush(m_rcDirty.GetDirty());
        m_rcDirty.Validate(); // _ValidateDirtyRect(this); //m_rcDirty.Validate();

        // if pop this assert; set break on WIN::InvalidateRect.
        DbgAssert(i < 100);

        render->Push(eGXPropFont | eGXPropTextColor);
        {
            CAutoMsgAnalysis awm("CTuiLayerBase::CTuiWgt::_DoPrint", GetObjectID());
            CTuiWgt::DoPrint(render.GetApp(), lp);
        }
        render->Pop();

        if (IsLifeDestroyed())
            return eRenderBkAbort;
        if (!IsRegionDirty())
            break;
        else if (GetDPI() != dpi //|| Is_WS_PRI_DIRTY()
            || dpi.LPtoDP(m_rcWnd.Width()) != szd.Width()
            || dpi.LPtoDP(m_rcWnd.Height()) != szd.Height()) // change size or dpi when render.
        {
            DbgTrace ("abort render: oid(%d), dpi(%d,%d), old size(%g,%g) new size(%g,%g)",
                GetObjectID(), dpi, GetDPI(), rcOld.Width(), rcOld.Height(), GetWidth(), GetHeight());
            return eRenderBkResize;
        }
    }
    
    DbgVerify(i <= 1)(" - %d - %d", i, GetObjectID());
    }
    TM_IF_DBG(m_nCountRepaint = 0);

    // debug out
    Edit_SaveTexture(Edit_IsObjectInSpyMode(this), this, m_txBuffer, GetAlpha());
    
    return eRenderBkOk;
}


ERenderBK CTuiLayerBase::RenderScene(IGXApp* gapp)
{
    if (!IsRegionDirty() && m_txBuffer)
        return eRenderBkOk;

    if (IsPainting())
    {
        DbgTrace("CTuiLayerBase::RenderScene() failed: widget id(%d) WM_PAINT reentry", GetObjectID());
        return eRenderBkAbort;
    }

//     TUI_IF_EDIT(CAutoFps _autofps(m_fpsRend));
    TUI_IF_EDIT(m_fpsReal.StepUp());

    struct CPriPainting
    {
        CTuiLayerBase* hWgt;
        CPriPainting(CTuiLayerBase* p) : hWgt(p) { hWgt->SetPainting(); }
        ~CPriPainting() { hWgt->ResetPainting(); }
    };
    CPriPainting pp(this);

    return RenderBkBuffer(gapp);
}

TMBool CTuiLayerBase::CheckInvalidate(const TUIRect& rc)
{
    TM_IF_DBG(DbgAssert(m_nCountRepaint < 90));
    return _InvalidateDirtyRect(this, static_cast<const CTuiRect&>(rc)); // TMBool b = m_rcDirty.Invalidate(static_cast<const CTuiRect&>(rc));
}

void CTuiLayerBase::CreateBkBuffer(IGXApp* gapp, const CSize& sz)
{
    m_txBuffer.CreateRenderTarget(sz.Width(), sz.Height(), eGXFormat32A, gapp);
}

void CTuiLayerBase::Reclaim()
{
    m_txBuffer.Release();
    Invalidate();
    CTuiWgt::Reclaim();
}

#ifdef TUI_EDIT_MODE

CString CTuiLayerBase::GetDebugInfo()
{
    float d1 = m_fpsRend.GetAvgDTime().ToMillionSecondF();
    float d2 = m_fpsFlip.GetAvgDTime().ToMillionSecondF();
    float d3 = m_fpsReal.GetAvgDTime().ToMillionSecondF();
    TM::CString str;
    str.Format(L"\r\n\r\navg flip time:%gms; max FPS:%g; real FPS:%g",
        d2, 1000.0f / (d1 + d2), 1000.0f / d3);
    
    return CTuiWgt::GetDebugInfo() + str;
}

#endif // TUI_EDIT_MODE



///////////////////////////////////////////////////////////////////////////////
// CTuiLayerImpl

TM_REGISTER_CLASS1(CTuiLayer);

TMBool CTuiLayerImpl::NativeInvalidateRect(SPY_INVALIDATE_PARAM(const TUIRect& rc, TM::HTUI hSender))
{
    CheckInvalidate(rc);
    return CTuiWgt::NativeInvalidateRect(SPY_INVALIDATE_PARAM(m_rcWnd, hSender));
}

void CTuiLayerImpl::DoPrint(IGXApp* gapp, TMLParam lParam)
{
    if (m_alpha == 255)
    {
        CTuiWgt::DoPrint(gapp, lParam);
        return;
    }
    
    if (eRenderBkOk == this->RenderScene(gapp))
        m_txBuffer->Blt(m_rcWnd, CRect(0,0,0,0), GX_BLT_DFTSRCWH, m_alpha);
}


TM_END_NAMESPACE



