/**
 *  @file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2026.4.24
 *
 *  Copyright (C) 2026 miragekiller
 */

#pragma once

#ifndef GXFONT_GTK_H
#define GXFONT_GTK_H

#include "gxfont.h"
#include <gtk/gtk.h>
#include <pango/pangocairo.h>
#include <string>
#include <type_traits>


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// CGXFontFactoryGtkT
template <class T>
struct CGXFontFactoryGtkT
    : public CStaticBaseRefT<CGXFontFactoryGtkT<T>, TM::CSet<T> >
{
    tm_constexpr11 TMBool IsValid() const { return true; }
};

///////////////////////////////////////////////////////////////////////////////
// CGXFontGtk
struct CGXFontGtk : public CGXFontImplT<CGXFontGtk>
{
public:
    typedef CGXFontImplT<CGXFontGtk>            base_class;
    typedef CGXFontFactoryGtkT<CGXFontGtk>      CGXFontFactory;
    typedef CTextCacheTexT<IGXTextureImpl>      CTextCacheTex;

    PangoFontDescription* m_font;
    TMMetric m_tm;

    CGXFontGtk(const CGXFontGtk& rhs)
        : base_class(rhs)
        , m_font(0)
        , m_tm(rhs.m_tm)
    {
    }
    void operator = (const CGXFontGtk&);

    CGXFontGtk()
        : m_font(0)
    {
        m_tm.tmHeight = 0;
        m_tm.tmAveCharWidth = 0;
    }

    ~CGXFontGtk()
    {
        if (m_font)
            pango_font_description_free(m_font);
        DbgAssert(m_nRef == 0);
    }

    static std::string _toUtf8(const TMCharW* ws)
    {
        if (!ws || !ws[0])
            return std::string();

        gchar* s = g_ucs4_to_utf8((const gunichar*)ws, -1, 0, 0, 0);
        if (!s)
            return std::string();

        std::string out(s);
        g_free(s);
        return out;
    }

    static std::string _toUtf8(const CGXStrView& str)
    {
        typedef typename std::remove_cv<typename std::remove_pointer<decltype(str.data())>::type>::type CharT;
        if (sizeof(CharT) == sizeof(char))
            return std::string((const char*)str.data(), str.GetLength());

        if (!str.data() || str.GetLength() <= 0)
            return std::string();

        gchar* s = g_ucs4_to_utf8((const gunichar*)str.data(), str.GetLength(), 0, 0, 0);
        if (!s)
            return std::string();

        std::string out(s);
        g_free(s);
        return out;
    }

    void MakeFont()
    {
        DbgAssert(!m_font);
        m_font = pango_font_description_new();

        std::string family = _toUtf8(m_ft.name);
        if (family.empty())
            family = "Sans";

        pango_font_description_set_family(m_font, family.c_str());
        pango_font_description_set_weight(m_font, (m_ft.style & GX_FONT_WEIGHT) ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
        pango_font_description_set_style(m_font, (m_ft.style & GX_FONT_ITALIC) ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL);

        int px = TM::CZoom(m_dpi).LPtoDP(m_ft.height ? m_ft.height : 16);
        if (px <= 0)
            px = 16;
        pango_font_description_set_absolute_size(m_font, px * PANGO_SCALE);
    }

    static CGXFontFactory* GetFontList() { return CGXFontFactory::CheckGetInstanceNoAddRef(); }

    PangoLayout* _CreateLayout(cairo_t* cr, const CGXStrView& str, TMUInt fmt, const TM::CSize* szLimit)
    {
        PangoLayout* layout = pango_cairo_create_layout(cr);
        pango_layout_set_font_description(layout, m_font);

        std::string txt = _toUtf8(str);
        pango_layout_set_text(layout, txt.c_str(), (int)txt.size());

        if (fmt & DT_SINGLELINE)
            pango_layout_set_single_paragraph_mode(layout, TRUE);

        if (fmt & DT_WORDBREAK)
            pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);

        if (fmt & (DT_WORD_ELLIPSIS | DT_PATH_ELLIPSIS | DT_END_ELLIPSIS))
            pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);

        if (szLimit && szLimit->Width() > 0 && (fmt & (DT_WORDBREAK | DT_WORD_ELLIPSIS | DT_PATH_ELLIPSIS | DT_END_ELLIPSIS)))
            pango_layout_set_width(layout, szLimit->Width() * PANGO_SCALE);
        else
            pango_layout_set_width(layout, -1);

        if (fmt & DT_RIGHT)
            pango_layout_set_alignment(layout, PANGO_ALIGN_RIGHT);
        else if (fmt & DT_CENTER)
            pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
        else
            pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);

        return layout;
    }

    void _FontCalcRect(const CGXStrView& str, TMSizeI& sz, TMUInt flag)
    {
        cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);
        cairo_t* cr = cairo_create(surface);
        TM::CSize szLimit(sz.cx, sz.cy);
        PangoLayout* layout = _CreateLayout(cr, str, flag, &szLimit);

        int w = 0;
        int h = 0;
        pango_layout_get_pixel_size(layout, &w, &h);
        sz.cx = w;
        sz.cy = h;

        g_object_unref(layout);
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
    }

    void _FontCalcRect(const CGXStrView& str, TM::CSize& sz, TMUInt flag)
    {
        TMSizeI tmp;
        tmp.cx = (int)sz.Width();
        tmp.cy = (int)sz.Height();
        _FontCalcRect(str, tmp, flag);
        sz.SetSize(tmp.cx, tmp.cy);
    }

    void _MakeTextImage(CGXAppBase* app, const CGXStrView& str, TMUInt fmt, HIMAGE img, TM::CSize sz)
    {
        cairo_t* cr = (cairo_t*)CImageHandle(img).CreateDC();
        if (!cr)
            return;

        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
        cairo_paint(cr);

        PangoLayout* layout = _CreateLayout(cr, str, fmt, &sz);

        if (CGXTextFmt(fmt).IsNeedRTL(app))
        {
            PangoContext* ctx = pango_layout_get_context(layout);
            pango_context_set_base_dir(ctx, PANGO_DIRECTION_RTL);
        }

        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        pango_cairo_update_layout(cr, layout);
        pango_cairo_show_layout(cr, layout);

        g_object_unref(layout);
        CImageHandle(img).DeleteDC((TMHDC)cr);
    }

    virtual EGXDevice FontGetDeviceType() tm_override
    {
        return eGXTxtDevGDI;
    }

    virtual TMMetric& FontGetMetrics() tm_override
    {
        if (!m_tm.tmHeight)
        {
            PangoFontMap* fm = pango_cairo_font_map_get_default();
            PangoContext* ctx = pango_font_map_create_context(fm);
            PangoFontMetrics* mt = pango_context_get_metrics(ctx, m_font, pango_language_get_default());
            m_tm.tmAveCharWidth = PANGO_PIXELS(pango_font_metrics_get_approximate_char_width(mt));
            m_tm.tmHeight = PANGO_PIXELS(pango_font_metrics_get_ascent(mt) + pango_font_metrics_get_descent(mt));
            pango_font_metrics_unref(mt);
            g_object_unref(ctx);
        }
        return m_tm;
    }

    virtual void FontCalcRect(const CGXStrView& str, TMSizeI& sz, TMUInt flag) tm_override
    {
        _FontCalcRect(str, sz, flag);
    }

    virtual void FontGetExtentPoint(const CGXStrView& str, int nMaxExtent, int* lpnFit, int* lpnDx, TMSize& sz) tm_override
    {
        tm_unused(nMaxExtent);

        TMSizeI out;
        out.cx = 0;
        out.cy = 0;
        _FontCalcRect(str, out, DT_SINGLELINE);

        sz.cx = out.cx;
        sz.cy = out.cy;

        if (lpnFit)
            *lpnFit = str.GetLength();

        if (lpnDx)
        {
            int n = str.GetLength();
            int step = (n > 0) ? (out.cx / n) : 0;
            int x = 0;
            for (int i = 0; i < n; ++i)
            {
                x += step;
                lpnDx[i] = x;
            }
        }
    }

    virtual void FontRenderText(CGXAppBase* app, const CGXStrView& str, TMUInt fmt, TMColor clr, const GXPoint* ptOrg, const GXSize* szMax, CTextCache& cache, CGXTextFmt* fmtMdf, GXRect* rcReal) tm_override
    {
        TM::CZoom zoom(GetDPI());
        CSize szCache;
        if (!cache)
        {
            DbgAssert(szMax);

            zoom.LPtoDPCeil(*szMax, szCache);
            CSize szCalc(szCache);
            _FontCalcRect(str, szCalc, fmt);
            if (fmtMdf)
                fmtMdf->MakeEllipsisSize(true, szCache, szCalc);
            else
                szCache = szCalc;

            CImage img;
            img.CreateIndirect(szCache.Width(), szCache.Height(), eGXFormat32X);
            if (!img)
                return;

            _MakeTextImage(app, str, fmt, img, szCache);
            ImageConvertToText(img, TMRGBA(255,255,255,255));

            IGXTexture* p = 0;
            app->TextureCreateFromImage(&p, img);
            if (!p)
                return;
            CTextCacheTex* pct = new CTextCacheTex();
            pct->m_imgText.Attach(static_cast<IGXTextureImpl*>(p));
            cache.Attach(pct);
        }
        else
            szCache.SetSize(cache->GetWidth(), cache->GetHeight());

        DbgAssert(cache);

        if (ptOrg)
        {
            GXSize szg;
            zoom.DPtoLP(szCache, szg);

            GXRect rcx;
            if (szMax)
            {
                rcx = GXRect(*ptOrg, *szMax);
                gxTextCalcAlignmentRect(rcx, szg, fmt);
            }
            else
                rcx = GXRect(*ptOrg, szg);
            if (rcReal)
                *rcReal = rcx;

            EGXBlt uFlag = app->IsRTL() ? (GX_BLT_R2L | GX_BLT_TEXTMONO) : GX_BLT_TEXTMONO;
            CTextCacheTex* pct = static_cast<CTextCacheTex*>(cache.p);
            pct->m_imgText->BltText(rcx, uFlag, clr);
        }
    }
};

///////////////////////////////////////////////////////////////////////////////
// CGXFont
void CGXFont::FontGetDefault(GXFont& ft)
{
    ft.Reset();
    ft.SetName(L"Sans");
    ft.height = 16;
    ft.styleMask = GX_FONT_MASK_STYLE;
}

CRefPtr<CGXFont> CGXFont::FontCreateDefaultFromSys()
{
    GXFont ft;
    FontGetDefault(ft);
    return CGXFontGtk::CreateFont(ft, TM_DPI_SYSTEM_DEFAULT);
}

TM_END_NAMESPACE

#endif // GXFONT_GTK_H