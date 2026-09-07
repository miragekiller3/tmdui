/**
 *  @file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2022.8.4
 *
 *  Copyright (C) 2022 miragekiller                               
 */

#pragma once

#ifndef GXFONT_DWRITE_H
#define GXFONT_DWRITE_H

#include <D3Dcommon.h>
#include <dwrite.h>
#include <d2d1_1.h>
#include "gxfont.h"
#include "gxapp_d3d11.h"


TM_BGN_NAMESPACE

struct dll_dwrite   { DECLARE_DLL_LOAD("dwrite.dll", LOAD_LIBRARY_SEARCH_SYSTEM32); };
struct dll_d2d1     { DECLARE_DLL_LOAD("d2d1.dll",   LOAD_LIBRARY_SEARCH_SYSTEM32); };

///////////////////////////////////////////////////////////////////////////////
// CGXFontFactoryDWriteT
template <class T>
struct CGXFontFactoryDWriteT
    : public CStaticBaseRefT<CGXFontFactoryDWriteT<T>, TM::CSet<T> >
{
    CComPtr<ID2D1Factory>               m_ptrD2D1Factory;
    CComPtr<IDWriteFactory>             m_ptrWriteFactory;
//     CComPtr<ID2D1SolidColorBrush>        m_ptrTextColorBrush;

//     T*                                  m_pCurrFont;
//     CComPtr<ID2D1RenderTarget>          m_ptrCurrRenderTarget;
    
    CGXFontFactoryDWriteT()
    {
        // device
        DECLARE_DLL_FUNC(dll_dwrite, HRESULT, WINAPI, DWriteCreateFactory,
            (DWRITE_FACTORY_TYPE factoryType, REFIID iid, IUnknown **factory));
        DECLARE_DLL_FUNC(dll_d2d1, HRESULT, WINAPI, D2D1CreateFactory,
            (D2D1_FACTORY_TYPE factoryType, REFIID iid, CONST D2D1_FACTORY_OPTIONS *pFactoryOptions, void **ppIFactory));

        if (!_D2D1CreateFactory || !_DWriteCreateFactory)
            return;

        D2D1_FACTORY_OPTIONS options = { D2D1_DEBUG_LEVEL_NONE };
#if defined(_DEBUG)
        // If the project is in a debug build, enable Direct2D debugging via SDK Layers.
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
        
        DbgVerifyHResult(_D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory1),
            &options,
            (void**)&m_ptrD2D1Factory));
        DbgVerifyHResult(_DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&m_ptrWriteFactory)
           ));
    }
    
    ~CGXFontFactoryDWriteT()
    {
    }

    TMBool IsValid() const
    {
        return m_ptrWriteFactory != 0;
    }
};

///////////////////////////////////////////////////////////////////////////////
// CGXFontDWrite
struct CGXFontDWrite : public CGXFontImplT<CGXFontDWrite>
{
public:
    typedef CGXFontImplT<CGXFontDWrite>             base_class;
    typedef CGXFontFactoryDWriteT<CGXFontDWrite>    CGXFontFactory;
    typedef CTextCacheTexT<IGXTextureImpl>          CTextCacheTex;

    TMMetric m_tm;
    CComPtr<IDWriteTextFormat> m_ptrTextFormat;

    // no copyable
    CGXFontDWrite(const CGXFontDWrite& rhs)
        : base_class(rhs)
        , m_tm(rhs.m_tm)
    {
        DbgAssert(!rhs.m_ptrTextFormat);
    }
    void operator = (const CGXFontDWrite&);

    CGXFontDWrite()
    {
        m_tm.tmHeight = 0;
    }

    ~CGXFontDWrite()
    {
        DbgAssert(m_nRef == 0);
    }

    void MakeFont()
    {
        DbgAssert(!m_ptrTextFormat);
        TMInt height = GetDPI().LPtoDP(m_ft.height);
        DbgVerifyHResult(GetFontList()->m_ptrWriteFactory->CreateTextFormat(
            m_ft.name,
            NULL,
            (m_ft.style & GX_FONT_WEIGHT) ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
            (m_ft.style & GX_FONT_ITALIC) ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            (FLOAT)height,
            L"en-US", //locale
            &m_ptrTextFormat
           ));
    }

    static CGXFontFactory* GetFontList()
    {
        return CGXFontFactory::CheckGetInstanceNoAddRef();
    }

    void _CreateTextLayout(IDWriteTextLayout*& textLayout, CGXStrPtr str, float w, float h, TMUInt uFormat)
    {
        CGXFontFactory* pl = GetFontList();

        // ellipsis
        if (uFormat & (DT_END_ELLIPSIS | DT_PATH_ELLIPSIS | DT_WORD_ELLIPSIS))    // "..."
        {
            CComPtr<IDWriteInlineObject> inlineObject;
            DbgVerifyHResult(pl->m_ptrWriteFactory->CreateEllipsisTrimmingSign(
                m_ptrTextFormat,
                &inlineObject
               ));
            if (!inlineObject)
                return;
        
            DWRITE_TRIMMING_GRANULARITY granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
            if (uFormat & DT_WORD_ELLIPSIS)
                granularity = DWRITE_TRIMMING_GRANULARITY_WORD;
        
            UINT32 delimiter = 0;
            UINT32 delimiterCount = 0;
            if (uFormat & DT_PATH_ELLIPSIS)
            {
                delimiter = '\\';
                delimiterCount = 1;
            }
        
            DWRITE_TRIMMING trimming = { granularity, delimiter, delimiterCount };
            DbgVerifyHResult(m_ptrTextFormat->SetTrimming(&trimming, inlineObject));
        }
        else
        {
            DWRITE_TRIMMING trimming = { DWRITE_TRIMMING_GRANULARITY_NONE, 0, 0 };
            DbgVerifyHResult(m_ptrTextFormat->SetTrimming(&trimming, 0));
        }

        // single line
        if (uFormat & DT_SINGLELINE)
        {
            size_t tail = 0, next = 0;
            for (; ; )
            {
                tail = StrSplitLine(str, str.GetLength(), next);
                int dt = next - tail;
                if (dt <= 0)
                    break;
                DbgAssert(dt <= 2);
                str.replace(tail, dt, L"  ", 2);
                next += (2 - dt);
            }
            DbgAssert(str.Find('\r') < 0);
            DbgAssert(str.Find('\n') < 0);
        }

        // string prefix
        CVector<int> aPrefix;
        if (!(uFormat & DT_NOPREFIX))
        {
            TMCharW ch = str[0];
            for (int i = 0; ;)
            {
                if (!ch)
                    break;
                if (ch == '&')
                {
                    str.erase(i, 1);
                    ch = str[i];
                    if (ch != '&')  // '&x'
                    {
                        aPrefix.push_back(i);
                        continue;
                    }
                    // '&&'; through 
                }
                ch = str[++i];
            }
        }

        // CreateTextLayout
        DbgVerifyHResult(pl->m_ptrWriteFactory->CreateTextLayout(
            str,
            str.GetLength(),
            m_ptrTextFormat,
            w,
            h,
            &textLayout
           ));
        if (!textLayout)
            return;
    
        // format
        DWRITE_TEXT_RANGE textRange = { 0, UINT_MAX };
    
        if (m_ft.style & GX_FONT_STRIKE)
            DbgVerifyHResult(textLayout->SetStrikethrough(true, textRange));
        if (m_ft.style & GX_FONT_UNDERLINE)
            DbgVerifyHResult(textLayout->SetUnderline(true, textRange));

        int nPrefix = aPrefix.size();
        tm_for (int i = 0; i < nPrefix; ++i)
        {
            DWRITE_TEXT_RANGE textRange = { aPrefix[i], 1 };
            DbgVerifyHResult(textLayout->SetUnderline(true, textRange));
        }
    
        if (!(uFormat & DT_WORDBREAK))
            DbgVerifyHResult(textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP));

        if (uFormat & DT_CENTER)
            DbgVerifyHResult(textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
        else if (uFormat & DT_RIGHT)
            DbgVerifyHResult(textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING));
    
        if (uFormat & DT_VCENTER)
            DbgVerifyHResult(textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
        else if (uFormat & DT_BOTTOM)
            DbgVerifyHResult(textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR));
    
        if (uFormat & DT_RTLREADING)
            DbgVerifyHResult(textLayout->SetReadingDirection(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT));
    }

    ///////////////////////////////////////////////////////////////////////////////
    // CGXFont
    virtual EGXDevice FontGetDeviceType() tm_override
    {
        return eGXTxtDevDWrite;
    }

    virtual TMMetric& FontGetMetrics() tm_override
    {
        if (!m_tm.tmHeight)
        {
            TMSizeI sz = { 0 };
            CStrView strTest(TM_CT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));
            FontCalcRect(strTest, sz, DT_SINGLELINE);
            m_tm.tmAveCharWidth = (sz.cx + 26) / 52;
            m_tm.tmHeight = sz.cy;
        }
        return m_tm;
    }

    void _FontCalcRect(const CGXStrView& str, int nMaxExtent, LPINT lpnFit, LPINT lpnDx, TMSize& sz, TMUInt flag)
    {
        CGXFontFactory* p = GetFontList();
        
        CComPtr<IDWriteTextLayout> textLayout;
        _CreateTextLayout(*&textLayout, str, nMaxExtent, (FLOAT)INT_MAX, flag);
        if (!textLayout)
            return;
        
        DWRITE_TEXT_METRICS metrics = { 0 };
        DbgVerifyHResult(textLayout->GetMetrics(&metrics));
        sz.cx = ceil(metrics.width);// IncludingTrailingWhitespace);
        sz.cy = ceil(metrics.height);
        
        if (lpnDx)
        {
            tm_alloca(DWRITE_LINE_METRICS, dlm, metrics.lineCount);
            UINT32 actualLineCount = 0;
            DbgVerifyHResult(textLayout->GetLineMetrics(dlm, metrics.lineCount, &actualLineCount));
            DbgAssert(lpnFit);
            *lpnFit = dlm[0].length;
            for (int i = 0; i < dlm[0].length; ++i)
            {
                DWRITE_HIT_TEST_METRICS dhtm;
                FLOAT x, y;
                DbgVerifyHResult(textLayout->HitTestTextPosition(i, true, &x, &y, &dhtm));
                lpnDx[i] = ceil(x);
            }
        }
    }

    virtual void FontCalcRect(const CGXStrView& str, TMSizeI& sz, TMUInt flag) tm_override
    {
        int maxWidth = (flag & DT_SINGLELINE) ? INT_MAX : sz.cx;
        const TMUInt uMask = DT_WORDBREAK | DT_SINGLELINE | DT_EXTERNALLEADING | DT_NOPREFIX | DT_INTERNAL | DT_EDITCONTROL | DT_RTLREADING;
        _FontCalcRect(str, maxWidth, 0, 0, sz, flag & uMask);
    }

    virtual void FontGetExtentPoint(const CGXStrView& str, int nMaxExtent, LPINT lpnFit, LPINT lpnDx, TMSize& sz) tm_override
    {
        _FontCalcRect(str, nMaxExtent, lpnFit, lpnDx, sz, DT_LEFT | DT_TOP | DT_NOPREFIX | DT_WORDBREAK);
    }

    void _TextDraw(ID2D1RenderTarget* rd, ID2D1SolidColorBrush* brush, const CGXStrView& str, const TMRect& rc, TMUInt uFormat)
    {
        DbgAssert(rd);

        CComPtr<IDWriteTextLayout> textLayout;
        _CreateTextLayout(*&textLayout, str, RectWidth(rc), RectHeight(rc), uFormat);
        if (!textLayout)
            return;
    
        D2D_POINT_2F pt = { rc.left, rc.top };
//         m_ptrCurrRenderTarget->DrawText(
//             str,
//             nLen,
//             m_pCurrFont->m_ptrTextFormat
//             , &D2D1::RectF(0, 0, oTargetSize.width, oTargetSize.height)
//             , pBrush
//             , (D2D1_DRAW_TEXT_OPTIONS)D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
//                        )
    
        rd->DrawTextLayout(
            pt,
            textLayout,
            brush,
            D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
           );
    }

    void _RenderToTarget(ID2D1RenderTarget* rt, const CGXStrView& str, TMUInt fmt, TMColor clr, TM::CSize sz)
    {
        // text color
        CComPtr<ID2D1SolidColorBrush> ptrTextColorBrush;
        D2D1_COLOR_F clrTxt = { ColorRF(clr), ColorGF(clr), ColorBF(clr), ColorAF(clr) };
        DbgVerifyHResult(rt->CreateSolidColorBrush(
            clrTxt,
            &ptrTextColorBrush
            ));
        
        // begin draw
        D2D1_COLOR_F clrBk = { 0, 0, 0, 0 };
        rt->BeginDraw();
        rt->Clear(clrBk);
        
        // draw
        //         renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        _TextDraw(rt, ptrTextColorBrush, str, CRect(0,0,sz.cx, sz.cy), fmt);
        
        // end draw
        DbgVerifyHResult(rt->EndDraw());
    }
    
    TMBool _CreateTextBuffer(
        CGXAppBase* app, const CGXStrView& str,
        TMUInt fmt, TMColor clr, TM::CSize sz, CTextCache& cache)
    {
        // cache texture
        IGXTexture* p = 0;
        app->TextureCreateRenderTarget(&p, sz.cx, sz.cy, eGXFormat32A);
        if (!p)
            return false;
    
        D2D1_RENDER_TARGET_PROPERTIES targetProperties =
        {
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED },
            96, 96,
            D2D1_RENDER_TARGET_USAGE_NONE,
            //D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
        };
        if ((app->GetDeviceType() & eGXGraDevMask) == eGXGraDevD3D11)
        {
            // DXGI interface
            CComQIPtr<IDXGISurface1> ptrDXGI((IUnknown*)p->GetNativeHandle());
            DbgAssert(ptrDXGI);
            
            // DXGI interface -> render target
            CComPtr<ID2D1RenderTarget> ptrRenderTarget;
            CGXFontFactory* pl = GetFontList();
            DbgVerifyHResult(pl->m_ptrD2D1Factory->CreateDxgiSurfaceRenderTarget(ptrDXGI, &targetProperties, &ptrRenderTarget));
            DbgAssert(ptrRenderTarget);
            
            // render
            _RenderToTarget(ptrRenderTarget, str, fmt, clr, sz);
        }
        else
        {
            CAutoTextureDC dc(p);

            // HDC -> render target
            CComPtr<ID2D1DCRenderTarget> ptrRenderTarget;
            CGXFontFactory* pl = GetFontList();
            DbgVerifyHResult(pl->m_ptrD2D1Factory->CreateDCRenderTarget(&targetProperties, &ptrRenderTarget));
            DbgAssert(ptrRenderTarget);

            // render
            RECT rc = {0, 0, sz.cx, sz.cy};
            ptrRenderTarget->BindDC(dc, &rc);
            _RenderToTarget(ptrRenderTarget, str, fmt, clr, sz);
        }
        
        // create object
        CTextCacheTex* pct = new CTextCacheTex();
        pct->m_imgText.Attach(static_cast<IGXTextureImpl*>(p));
        cache.Attach(pct);
        return true;
    }

    virtual void FontRenderText(
        CGXAppBase* app, const CGXStrView& str,
        TMUInt fmt, TMColor clr, const GXPoint* ptOrg,
        const GXSize* szMax, CTextCache& cache, CGXTextFmt* fmtMdf, GXRect* rcReal) tm_override
    {
        TM::CZoom zoom(GetDPI());
        CSize szCache;
        if (!cache)
        {
            DbgAssert(szMax);

            // calc size
            DbgAssert(szMax);
            zoom.LPtoDPCeil(*szMax, szCache);
            CSize szCalc(szCache);
            FontCalcRect(str, szCache, fmt);
            tm_setmin(szCache.cx, szCalc.cx);
            tm_setmin(szCache.cy, szCalc.cy);
            
            DbgAssert(szCache.Width() > 0);
            DbgAssert(szCache.Height() > 0);
            GF_TRACE("CGXFontWin::FontRenderText()::CacheSize(%d, %d)", szCache.cx, szCache.cy);

            _CreateTextBuffer(app, str, fmt | DT_VCENTER, clr, szCache, cache);
        }
        else
            szCache.SetSize(cache->GetWidth(), cache->GetHeight());

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
            
            EGXBlt uFlag = (fmt & DT_RTLREADING) ? (GX_BLT_R2L) : GX_BLT_NONE;
            CTextCacheTex* pct = static_cast<CTextCacheTex*>(cache.p);
            pct->m_imgText->BltText(rcx, uFlag, 0xffffffff);
        }
    }
};

///////////////////////////////////////////////////////////////////////////////
// CGXFont
CRefPtr<CGXFont> CGXFont::FontCreateDefaultFromDWrite()
{
    GXFont ft;
    FontGetDefault(ft);
    return CGXFontDWrite::CreateFont(ft, TM_DPI_SYSTEM_DEFAULT);
}

TM_END_NAMESPACE

#endif // GXFONT_DWRITE_H



