/**
 *  @file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.7.12
 *
 *  Copyright (C) 2019 miragekiller                               
 */

#pragma once

#ifndef GXFONT_QT_H
#define GXFONT_QT_H

#include "gxfont.h"
#include <QPainter>
#include <qtextlayout.h>


// #define GF_TRACE DbgTrace
#define GF_TRACE DbgTraceNull

TM_BGN_NAMESPACE


struct CGXFontListQT;
#define CGXFontQT CGXFont
typedef CTextEngine_Native CTextEngine;

///////////////////////////////////////////////////////////////////////////////
// CGXFontQT
struct CGXFontQT : public CGXFontBaseT<CGXFontQT>
{
    typedef CGXFontListQT CGXFontList;

    TMHFont m_hFont;
    TMMetric m_tm;

    CGXFontQT()
    {
        m_tm.tmHeight = 0;
    }
    
    CGXFontQT(const CGXFontQT& rhs)
        : CGXFontBaseT<CGXFontQT>(rhs)
    {
        m_tm.tmHeight = 0;
    }

    ~CGXFontQT()
    {
        DbgAssert(m_nRef == 0);
    }

    void MakeFont()
    {
        m_hFont = m_ft.CreateHFont(m_dpi);
    }

    virtual TMMetric& GetMetrics() tm_override;

    virtual TMHFont MakeCurrent(void* app) tm_override;

    static void MakeSysFont(GXFont& ft)
    {
        ft.FromHFont(QApplication::font());
    }

    static CGXFontListQT* CheckGetInstance();
};


///////////////////////////////////////////////////////////////////////////////
// CGXFontListQT
struct CGXFontListQT : public CStaticBaseRefT<CGXFontListQT, TM::CSet<CGXFontQT> >
{
    CGXFontListQT()
    {
    }
    
    ~CGXFontListQT()
    {
    }
};


///////////////////////////////////////////////////////////////////////////////
// CGXFontBaseT<CGXFontQT>
void CTextEngine::DrawToDevice(CGXAppBase* app, const GXRect& rcDst, EGXBlt uFlag, TMColor clr, PFNRawDrawText proc, void* param)
{
    CGXAppQT* app = CGXAppQT::CheckGetInstance();
    CAutoPen apc(app->m_dcBk, clr);
    proc(rcDst, param);
}

void CTextEngine::CaclRect(const CGXStrView& str, TMSizeI& sz, TMUInt fmt)
{
    CGXAppQT* app = CGXAppQT::CheckGetInstance();
    CRect rc(0, 0, sz.width(), sz.height());
    app->m_dcBk.DrawText(str, str.GetLength(), rc, fmt | DT_CALCRECT);
    sz.setWidth(rc.Width());
    sz.setHeight(rc.Height());
}

void CTextEngine::RawDraw(TMCStr str, int n, const TMRect& rc, TMUInt fmt)
{
    CGXAppQT* app = CGXAppQT::CheckGetInstance();
    app->m_dcBk.DrawText(str, n, (TMRect*)&rc, fmt);
}

void CTextEngine::RawDraw(TMCStr str, int n, int left, int top, TMBool bRTL)
{
   //UINT fuOptions = (bRTL ? ETO_RTLREADING : 0); // | ETO_IGNORELANGUAGE
//     DbgVerify(::ExtTextOut(CGXFontWin::CheckGetInstance()->m_dcTxt, left, top, fuOptions, 0, str, n, 0));
}

void CTextEngine::RawGetExtentPoint(TMCStr str, int n, int nMaxExtent, int* lpnFit, int* lpnDx, TMSize* lpSize)
{
//     DbgVerify(::GetTextExtentExPoint(CGXFontWin::CheckGetInstance()->m_dcTxt, str, n, nMaxExtent, lpnFit, lpnDx, lpSize));
}

///////////////////////////////////////////////////////////////////////////////
// CGXFontQT
TMMetric& CGXFontQT::GetMetrics()
{
    if (!m_tm.tmHeight)
    {
        QFontMetrics metr(m_hFont);
        m_tm.tmAveCharWidth = metr.averageCharWidth();
        m_tm.tmHeight = metr.xHeight();
    }
    return m_tm;
}

TMHFont CGXFontQT::MakeCurrent(void* app)
{
    GF_TRACE(L"CGXFontQT::MakeCurrent(name:%s, height:%d, dpi%d, TMHFont:%x)", m_ft.name.c_str(), m_ft.height, m_dpi, &m_hFont);
    CGXAppQT* p = CGXAppQT::CheckGetInstance();
    TMHFont ftOld = p->m_dcBk.m_hDC->font();
    p->m_dcBk.m_hDC->setFont(m_hFont);
    return ftOld;
}

CGXFontListQT* CGXFontQT::CheckGetInstance()
{
    return CGXFontListQT::CheckGetInstanceNoAddRef();
}


///////////////////////////////////////////////////////////////////////////////
// CAutoDCFont
struct CAutoDCFont
{
    QFont ftOld;
    CGXFont* font;

    template <class T> CAutoDCFont(T* _text)
    {
        font = _text->GetFont();
        ftOld = font->MakeCurrent(0);
    }

    ~CAutoDCFont()
    {
        CGXAppQT* app = CGXAppQT::CheckGetInstance();
        app->m_dcBk.m_hDC->setFont(ftOld);
    }
};


TM_END_NAMESPACE

#endif // GXFONT_H
