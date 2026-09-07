/**
 *  @file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.11.10
 *
 *  Copyright (C) 2015 miragekiller                               
 */

#pragma once

#ifndef GXFONT_MAC_H
#define GXFONT_MAC_H

#ifndef GXFONT_H
#   error "include gxfont.h, don't incude this file"
#endif


// #define GF_TRACE DbgTrace
#define GF_TRACE DbgTraceNull

TM_BGN_NAMESPACE

struct TMMetric
{
    int tmAveCharWidth;
    int tmHeight;
};

template <class T>
struct CGXFontListMac : public TM::CSet<T>
{
    CGXFontListMac()
    {
    }
    
    ~CGXFontListMac()
    {
    }
};

struct CGXFontMac : public CGXFontImplT<CGXFontMac>
{
    typedef CGXFontListMac<CGXFontMac>      _CGXFontList;
    typedef CStaticRefObjectT<_CGXFontList> CGXFontList;

    GFontHandle m_font;
    TMMetric m_tm;

    CGXFontMac()
    {
        m_tm.tmHeight = 0;
    }
    
    CGXFontMac(const CGXFontMac& rhs)
        : CGXFontImplT<CGXFontMac>(rhs)
    {
        m_tm.tmHeight = 0;
    }

    ~CGXFontMac()
    {
        m_font.Release();
        DbgAssert(m_nRef == 0);
    }

    void MakeFont()
    {
        m_font = m_ft.CreateHFont();
    }

    TMMetric& FontGetMetrics();

    TMHFont MakeCurrent(CGXAppBase* app);

    void FontCalcRect(const CGXStrView& str, TMSizeI& sz, TMUInt fmt);

    static void FontGetDefault(GXFont& ft)
    {
//         NONCLIENTMETRICS info = { sizeof(info), 0 };
//         SystemParametersInfo (SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
//         ft.FromLOGFONT(info.lfMessageFont);
//         ft.height = 16;//dpi.LPtoDP(-15); // info.lfStatusFont.lfHeight);
    }
};

TMMetric& CGXFontMac::FontGetMetrics()
{
    if (!m_tm.tmHeight)
    {
        m_tm.tmAveCharWidth = m_font.GetFontBBox().size.width;
        m_tm.tmHeight = m_font.GetXHeight();
    }
    return m_tm;
}

TMHFont CGXFontMac::MakeCurrent(CGXAppBase* app)
{
    GF_TRACE(L"CGXFontMac::MakeCurrent(name:%s, height:%d, dpi%d, TMHFont:%x)", m_ft.name.c_str(), m_ft.height, m_dpi, m_font.m_hFont);
    return 0;
}

void CGXFontMac::FontCalcRect(const CGXStrView& str, TMSizeI& sz, TMUInt flag)
{
    return;
}


typedef CGXFontMac CGXFont;


struct CAutoDCFont
{
 //   HGDIOBJ hOldFont;
    CGXFont* font;
    template <class T> CAutoDCFont(T* _text)
    {
//         CGContextSaveGState();
        font = _text->GetFont();
        //hOldFont = font->MakeCurrent();
    }
    ~CAutoDCFont()
    {
//         CGContextRestoreGState();
    }
};


TM_END_NAMESPACE

#endif // GXFONT_H
