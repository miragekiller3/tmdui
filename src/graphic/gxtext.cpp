/**
 *  @file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.11.21
 *
 *  Copyright (C) 2018 miragekiller                               
 */

#include "gxtext.h"
#include "../gximage.h"
#include "gxapp.h"


TM_BGN_NAMESPACE

GXText__::GXText__()
    : GXTextBase__(DT_NOPREFIX)
{
}

void GXText__::CalcDeviceSize(CGXAppBase* app, CSizeI& sz, TMBool bCalcEllipsis)
{
    CSizeI szReal(sz);
    app->m_font->FontCalcRect(m_strTxt, szReal, m_flag);
    MakeEllipsisSize(bCalcEllipsis, sz, szReal);
}

// IGXText
void GXText__::SetString(const CGXStrPtr& str)
{
    if (m_strTxt != str)
    {
        m_strTxt = str;
        m_cache.Release();
    }
}

CGXStrPtr GXText__::GetHtml(TMSel0 nBgn, int nSize)
{
    return GetString(nBgn, nSize);
}

void GXText__::SetFormat(TMUInt32 uFormat)
{
    if ((m_flag & DT_TEXTDRAWMASK) != (uFormat & DT_TEXTDRAWMASK))
    {
        m_cache.Release();
        m_flag &= ~DT_TEXTDRAWMASK;
        m_flag |= (uFormat & DT_TEXTDRAWMASK);
    }
}

void GXText__::CalcSize(GXSize& sz, IGXApp* gapp)
{
    // windows GDI not support nCount == 0;
    if (m_strTxt.IsEmpty())
    {
        sz.SetSize(0, 0);
        return;
    }
    
    CGXAppBase* app = static_cast<CGXAppBase*>(gapp);
    CZoom zoom(app->GetDPI());
    CSizeI szi;
    zoom.LPtoDPCeil(sz, szi);
    this->CalcDeviceSize(app, szi, false);
    zoom.DPtoLP(szi, sz);
}

void GXText__::Reclaim()
{
    m_cache.Release();
}

TMBool GXText__::GetSize(GXSize& sz)
{
    if (!m_cache)
        return false;
    DbgAssert(m_font);
    CSize szd(m_cache->GetWidth(), m_cache->GetHeight());
    m_font->GetDPI().DPtoLP(szd, sz);
    return true;
}

void GXText__::Paint(const GXRect& rc, RICHDRAW* td, GXRect* rcReal, IGXApp* gapp)
{
    DbgAssert(!(m_flag & DT_CALCRECT));

    if (m_strTxt.IsEmpty())
    {
        if (rcReal)
        {
            *rcReal = rc;
            gxTextCalcAlignmentRect(*rcReal, 0, 0, m_flag);
        }
       return;
    }

    CGXAppBase* app = static_cast<CGXAppBase*>(gapp);
    bool bRebuild = (!m_cache
        || m_font != app->m_font
        || IsNeedRTL(app) != IsFlagR2L()
        || !m_cache->CacheTestApp(app)
        || this->TrySetMaxSize_Text(rc.Size()));
    if (bRebuild)
    {
        m_font = app->m_font;
        SettingFlagR2L(IsNeedRTL(app));
        m_cache.Release();
    }
    
    TMColor clrNow = app->m_clrText;
    m_szMax = rc.Size();
    m_font->FontRenderText(app, m_strTxt, m_flag, clrNow, tm_addressof(rc.TopLeft()), &m_szMax, m_cache, this, rcReal);
}



TM_END_NAMESPACE


