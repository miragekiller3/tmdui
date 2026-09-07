/**
 *  @file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.11.10
 *
 *  Copyright (C) 2015 miragekiller                               
 */

#pragma once

#ifndef GXFONT_WIN_H
#define GXFONT_WIN_H

#include "gxfont.h"
#include <usp10.h>

#pragma comment(lib, "usp10.lib")


TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CGXFontFactoryWinT
template <class T>
struct CGXFontFactoryWinT
    : public CStaticBaseRefT<CGXFontFactoryWinT<T>, TM::CSet<T> >
{
    CDC m_dcTxt;
    
    CGXFontFactoryWinT()
    {
        m_dcTxt.CreateCompatibleDC(0);
        m_dcTxt.SetBkMode(TRANSPARENT);
        m_dcTxt.SetStretchBltMode(COLORONCOLOR);
        m_dcTxt.SetTextColor(RGB(255,255,255));
        m_dcTxt.SetBkColor(RGB(0,0,0));
    }
    
    ~CGXFontFactoryWinT()
    {
        DbgAssert(GetObjectType(m_dcTxt) == OBJ_MEMDC);
    }

    tm_constexpr11 TMBool IsValid() const { return true; }
};

///////////////////////////////////////////////////////////////////////////////
// CGXFontWin
struct CGXFontWin : public CGXFontImplT<CGXFontWin>
{
public:
    typedef CGXFontImplT<CGXFontWin>            base_class;
    typedef CGXFontFactoryWinT<CGXFontWin>      CGXFontFactory;
    typedef CTextCacheTexT<IGXTextureImpl>      CTextCacheTex;

    struct CAutoFont
    {
        HGDIOBJ hOldFont;

        CAutoFont(CGXFontWin* _ft)
        {
            GF_TRACE(L"CAutoFont::CAutoFont(name:%s, height:%d, dpi%d, TMHFont:%x)", &_ft->m_ft.name, _ft->m_ft.height, _ft->m_dpi, _ft->m_font);
            DbgAssert(OBJ_FONT == GetObjectType(_ft->m_font));
            CGXFontFactory* p = CGXFontWin::GetFontList();
            hOldFont = (TMHFont)::SelectObject(p->m_dcTxt, _ft->m_font);
            DbgAssert(p->m_dcTxt.GetCurrentFont() == _ft->m_font);
        }

        ~CAutoFont()
        {
            DbgVerify(::SelectObject(CGXFontWin::GetFontList()->m_dcTxt, hOldFont));
        }
    };

    TMHFont m_font;
    TMMetric m_tm;

    // no copyable
    CGXFontWin(const CGXFontWin& rhs)
        : base_class(rhs)
        , m_font(0)
        , m_tm(rhs.m_tm)
    {
        DbgAssert(!rhs.m_font);
    }
    void operator = (const CGXFontWin&);

    CGXFontWin()
        : m_font(0)
    {
        m_tm.tmHeight = 0;
    }

    ~CGXFontWin()
    {
        if (m_font)
            DbgVerify(::DeleteObject(m_font));
        DbgAssert(m_nRef == 0);
    }

    void MakeFont()
    {
        DbgAssert(!m_font);
        m_font = m_ft.CreateHFont(m_dpi);
    }

    static CGXFontFactory* GetFontList() { return CGXFontFactory::CheckGetInstanceNoAddRef(); }

    void _FontCalcRect(const CGXStrView& str, TMSizeI& sz, TMUInt flag)
    {
        CGXFontFactory* p = GetFontList();
        DbgAssert(p->m_dcTxt.GetCurrentFont() == m_font);

        CRect rc(0, 0, sz.cx, sz.cy);
        
        const TMUInt uMask = DT_WORDBREAK | DT_SINGLELINE | DT_EXTERNALLEADING | DT_NOPREFIX | DT_INTERNAL | DT_EDITCONTROL | DT_RTLREADING;
        p->m_dcTxt.DrawText(str.data(), str.GetLength(), &rc, (flag & uMask) | DT_CALCRECT);
        GF_TRACE(L"CGXFontWin::_CalcRect(txt:%s, len:%d, sz:(%d,%d->%d,%d), flag:%x)",
            str, str.GetLength(), SizeWidth(sz), SizeHeight(sz), rc.Width(), rc.Height(), flag);
        
        DbgAssert(rc.left == 0);
        DbgAssert(rc.top == 0);
        sz.cx = rc.Right();
        sz.cy = rc.Bottom();
    }

    void _MakeTextImage(CGXAppBase* app, const CGXStrView& str, TMUInt fmt, HIMAGE img, TM::CSize sz)
    {
        CDCHandle dc(CGXFontWin::GetFontList()->m_dcTxt);
        DbgAssert(dc.GetCurrentFont() == m_font);

        HBITMAP hImg = ImageGetBitmap(img);
        HBITMAP hOldbmp = dc.SelectBitmap(hImg);
        CRect rcy(0, 0, sz.Width(), sz.Height());
        CLocal& local = app->m_local;
        if (!local.IsDBCS())
            fmt |= DT_NOFULLWIDTHCHARBREAK;
        if (CGXTextFmt(fmt).IsNeedRTL(app))
        {
            fmt |= DT_RTLREADING;
            if (fmt & DT_RIGHT)
                fmt &= ~DT_RIGHT;
            else
                fmt |= DT_RIGHT;
        }
        
        DbgVerify(dc.DrawText(str.data(), str.GetLength(), &rcy, fmt & 0xfffff));
        dc.SelectBitmap(hOldbmp);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // CGXFont
    virtual EGXDevice FontGetDeviceType() tm_override
    {
        return eGXTxtDevGDI;
    }

    virtual TMMetric& FontGetMetrics() tm_override
    {
        if (!m_tm.tmHeight)
        {
            CAutoFont ftBak(this);
            CGXFontFactory* p = GetFontList();
            TEXTMETRIC tm;
            DbgVerify(::GetTextMetrics(p->m_dcTxt, &tm));
            m_tm.tmAveCharWidth = tm.tmAveCharWidth;
            m_tm.tmHeight = tm.tmHeight;
        }
        return m_tm;
    }

    virtual void FontCalcRect(const CGXStrView& str, TMSizeI& sz, TMUInt flag) tm_override
    {
        CAutoFont ftBak(this);
        _FontCalcRect(str, sz, flag);
    }

    virtual void FontGetExtentPoint(const CGXStrView& str, int nMaxExtent, int* lpnFit, int* lpnDx, TMSize& sz) tm_override
    {
        CAutoFont ftBak(this);
        DbgVerify(::GetTextExtentExPoint(GetFontList()->m_dcTxt, str.data(), str.GetLength(), nMaxExtent, lpnFit, lpnDx, &sz));
        GF_TRACE(L"CGXFontWin::FontGetExtentPoint(str:%s, n:%d, nMaxExtent:%d, sz:(%d,%d))",
            str, str.GetLength(), nMaxExtent, SizeWidth(sz), SizeHeight(sz));
    }

    virtual void FontRenderText(CGXAppBase* app, const CGXStrView& str, TMUInt fmt, TMColor clr, const GXPoint* ptOrg, const GXSize* szMax, CTextCache& cache, CGXTextFmt* fmtMdf, GXRect* rcReal) tm_override
    {
        TM::CZoom zoom(GetDPI());
        CSize szCache;
        if (!cache)
        {
            DbgAssert(szMax);
            
            CAutoFont ftBak(this);
            
            // calc size
            DbgAssert(szMax);
            zoom.LPtoDPCeil(*szMax, szCache);
            CSize szCalc(szCache);
            _FontCalcRect(str, szCalc, fmt);
//             tm_setmin(szCache.cx, szMax->cx);
//             tm_setmin(szCache.cy, szMax->cy);
//            MakeEllipsisSize(false, sz, szReal);
            if (fmtMdf)
                fmtMdf->MakeEllipsisSize(true, szCache, szCalc); // szCache.cx maybe < 0;
            else
                szCache = szCalc;
//             DbgAssert(szCache.Width() > 0);
//             DbgAssert(szCache.Height() > 0);
            
            // create text image
            CImage img;
            img.CreateIndirect(szCache.Width(), szCache.Height(), eGXFormat32X);
            if (!img)
                return;
            
            // draw bk image
            _MakeTextImage(app, str, fmt, img, szCache);
            
            // color
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
            // convert rc from logic device to pixel device
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

            //     if (clrNow != m_color)
            //         m_engine.MakeTextColor(m_color = clrNow);
            
            //     CRect rc;
            //     zoom.LPtoDP(rcx, rc);
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
    NONCLIENTMETRICS info = { sizeof(info), 0 };
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
    ft.FromLOGFONT(info.lfMessageFont);
    ft.height = 16;//dpi.LPtoDP(-15); // info.lfStatusFont.lfHeight);
}

CRefPtr<CGXFont> CGXFont::FontCreateDefaultFromSys()
{
    GXFont ft;
    FontGetDefault(ft);
    return CGXFontWin::CreateFont(ft, TM_DPI_SYSTEM_DEFAULT);
}

///////////////////////////////////////////////////////////////////////////////
// CTextScript
// class CTextScript
// {
// private:
//     SCRIPT_CONTROL          m_control;
//     SCRIPT_STATE            m_state;
//     CLocal                  m_local;
// 
//     void RawGetExtentPoint(LPCTSTR str, int n, int nMaxExtent, LPINT lpnFit, LPINT lpnDx, TMSize* lpSize)
//     {
//         DbgVerify(::GetTextExtentExPoint(CGXFontWin::CheckGetInstance()->m_dcTxt, str, n, nMaxExtent, lpnFit, lpnDx, lpSize));
//     }
// 
// public:
//     void ResetLocal(CLocal local)
//     {
//         m_local = local;
//         
//         // Script Init
//         SCRIPT_DIGITSUBSTITUTE sd;
//         SCRIPT_CONTROL control = { LANG_USER_DEFAULT };
//         SCRIPT_STATE state = {0};
//         ScriptRecordDigitSubstitution(m_local, &sd);
//         ScriptApplyDigitSubstitution(&sd, &control, &m_state);
//         m_state.uBidiLevel = local.IsRTL();
//     }
//     
//     int Itemize(CVector<SCRIPT_ITEM>& aItems, const WCHAR *pwcInChars, int nMaxLen)
//     {
//         int nItems;
//         HRESULT hr;
//         for (;;)
//         {
//             hr = ::ScriptItemize(pwcInChars, nMaxLen, aItems.size(), &m_control, &m_state, &aItems[0], &nItems);
//             if (hr != E_OUTOFMEMORY)
//                 break;
//             aItems.resize(aItems.size() * 2);
//         }
//         //     aItems[nItems].iCharPos = nMaxLen;
//         
//         DbgAssert(aItems[nItems].iCharPos == nMaxLen);
//         DbgAssert(SUCCEEDED(hr));
//         if (FAILED(hr))
//             return 0;
//         
// #ifdef TM_DBG_RICH
//         tm_for (int i = 0; i <= nItems; ++i)
//         {
//             SCRIPT_ITEM& itm = aItems[i];
//             DbgTrace(L"CGXRich::MergeParagraph run%d; charpos:%d, analysis:%d, eScript:%d, bidiLevel:%d, RTL:%d, LayoutRTL:%d",
//                 i, itm.iCharPos, *(int*)&itm.a, itm.a.eScript, itm.a.s.uBidiLevel, itm.a.fRTL, itm.a.fLayoutRTL);
//         }
// #endif
//         
//         // merge style runs
//         {
// //             CAutoRichAnalysis _ama3("CGXRich::MergeParagraph.MergeStyleRuns");
//             
//             int i = 0, j = 0, k = 1;
//             tm_for (; j < nItems; ++i)
//             {
//                 SCRIPT_ITEM a0 = aItems[j];
//                 for (; ++j < nItems;)
//                 {
//                     SCRIPT_ANALYSIS a1 = aItems[j].a;
//                     if (a0.a.fLayoutRTL != a1.fLayoutRTL || a0.a.fRTL != a1.fRTL || a0.a.s.uBidiLevel != a1.s.uBidiLevel)
//                         break;
// //                     rich_trace(L"CGXRich::MergeParagraph merget run:%d", j);
//                     a0.a.eScript = SCRIPT_UNDEFINED;
//                 }
//                 aItems[i] = a0;
//             }
//             aItems[i] = aItems[nItems];
//             nItems = i;
//         }
//         DbgAssert(aItems[nItems].iCharPos == nMaxLen);
//         return nItems;
//     }
// 
//     int MakeSize(SCRIPT_ANALYSIS analysis, LPCTSTR pStr, int nLen, int nMaxSize, TMBool bFirstRun, int* piDx, TMSize& size)
//     {
// //         CAutoRichAnalysis _ama("CRichRunTxtBase::MakeSize");
//         DbgAssert(nLen > 0);
//         
//         int nCount = 0;
//         RawGetExtentPoint(pStr, nLen, nMaxSize, &nCount, piDx, &size);
//         //        DbgTrace("GetTextExtentExPoint:len:%d, maxsize:%d, ncount:%d", nLen, nMaxSize, nCount);
//         if (nCount == nLen)
//             return nLen; // needn't re-warp
//         
//         int nPosSpace = 0;
//         if (nCount)
//         {
// //             CAutoRichAnalysis _ama("CRichRunTxtBase::MakeSize.ScriptBreak");
//             // need word warp.
//             // search white space.
//             tm_alloca(SCRIPT_LOGATTR, logattrs, nCount);
//             ScriptBreak(pStr, nCount, &analysis, logattrs);
//             
//             for (int i = 0; i < nCount; ++i)
//             {
//                 if (/*logattrs[i].fWhiteSpace ||*/ logattrs[i].fSoftBreak)
//                     nPosSpace = i;
//             }
//         }
//         
//         if (bFirstRun)
//         {
//             if (!nPosSpace)
//                 nPosSpace = nCount ? nCount : 1;
//         }
//         else if (!nPosSpace)
//             return 0;
//         
// //         CAutoRichAnalysis _ama2("CRichRunTxtBase::MakeSize.GetTextExtentExPoint");
//         DbgAssert(nPosSpace);
//         RawGetExtentPoint(pStr, nPosSpace, INT_MAX, &nCount, piDx, &size);
//         DbgAssert(nCount == nPosSpace);
//         return nPosSpace;
//     }
//     
// };




TM_END_NAMESPACE

#endif // GXFONT_H