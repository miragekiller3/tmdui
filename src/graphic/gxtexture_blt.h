/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.8.2
 *
 *  Copyright (C) 2020 miragekiller
 */

#pragma once

#ifndef GXTEXTURE_BLT_H
#define GXTEXTURE_BLT_H

#include "../tmatl.h"


TM_BGN_NAMESPACE

#ifdef TM_WIN
struct CBltDeviceWin : public CDCHandleWin
{
protected:
    typedef HDC             src_dc_type;
    enum EFunction { eSrc32Copy, eAlphaBlend, eSrc24Copy };

    TMUInt8     m_nFunction; // 0:__BBlt(SRCCOPY); 1: GDIAlphaBlend; 2: __BBlt
    TMUInt8     m_AlphaFormat;
    TMUInt8     m_alpha;
    bool        m_bClear;
    bool        m_bR2L;

    TMUInt32    m_uFlag;
    CAffine     m_affOld;

    CBltDeviceWin(TMHDC hDC, TMUInt32 uFlag)
        : CDCHandleAT(hDC)
        , m_uFlag(uFlag)
    {
        DbgAssert(hDC);
        DbgVerify(this->GetWorldTransform(m_affOld));
        m_bR2L = m_affOld.M11() < 0;
    }

    ~CBltDeviceWin()
    {
        if (m_uFlag & (GX_BLT_R2L | GX_BLT_X2Y | GX_BLT_B2T))
            DbgVerify(this->SetWorldTransform(m_affOld));
    }

public:
    void InitFunc(TMUInt32 uFlag, TMUInt8 _alpha, EGXFormat format)
    {
        if (((uFlag & GX_BLT_SRCCOPY) || (format < eGXFormat32A)) && _alpha == 255)
        {
            DbgAssert(_alpha == 255);
            m_nFunction = (format == eGXFormat32A) ? eSrc32Copy : eSrc24Copy;
        }
        else
        {
            m_alpha = _alpha;
            m_nFunction = eAlphaBlend;
            m_AlphaFormat = (!(uFlag & GX_BLT_SRCCOPY) && (format == eGXFormatA8B8G8R8 || format == eGXFormat32A)) ? AC_SRC_ALPHA : 0;
        }
        m_bClear = ((uFlag & GX_BLT_SRCCOPY) && (_alpha != 255));
    }

    void _done(int _xDst, int _yDst, int _wDst, int _hDst,
        TMHDC hSrcDC, int _xSrc, int _ySrc, int _wSrc, int _hSrc)
    {
        // GDI have bug: RTL + Blt will offset 1 pixel.
        if (m_bR2L)
            ++_xDst;

#ifdef TM_DBG
        //         CAffine aff;
        //         DbgVerify(::GetWorldTransform(hSrcDC, &aff));
        //         DbgAssert(aff.IsEmpty());
#endif

        if (m_nFunction == eSrc32Copy)
            this->StretchBlt(_xDst, _yDst, _wDst, _hDst, hSrcDC, _xSrc, _ySrc, _wSrc, _hSrc, SRCCOPY);
        else if (m_nFunction == eAlphaBlend)
            this->AlphaBlend(_xDst, _yDst, _wDst, _hDst, hSrcDC, _xSrc, _ySrc, _wSrc, _hSrc, m_AlphaFormat, m_alpha);
        else if (m_nFunction == eSrc24Copy)
            this->Blt24To32(_xDst, _yDst, _wDst, _hDst, hSrcDC, _xSrc, _ySrc, _wSrc, _hSrc, SRCCOPY);
    }

    void _clear(int _xDst, int _yDst, int _wDst, int _hDst)
    {
        if (m_bClear)
            this->PatBlt(_xDst, _yDst, _wDst, _hDst, BLACKNESS);
    }
};
typedef CBltDeviceWin CBltDeviceGDI;
#elif defined(TM_GTK)
struct CBltDeviceGtk : public CDCHandleGtk
{
protected:
    typedef GdkPixbuf*          src_dc_type;

    bool        m_bSrcCopy;
    TMUInt8     m_alpha;
    TMUInt32    m_uFlag;

    CBltDeviceGtk(TMHDC hDC, TMUInt32 uFlag) : CDCHandleGtk(hDC), m_uFlag(uFlag) { cairo_save(hDC); }
    ~CBltDeviceGtk() { cairo_restore(m_hDC); }
    
public:
    void InitFunc(TMUInt32 uFlag, TMUInt8 _alpha, EGXFormat format)
    {
        m_bSrcCopy = (uFlag & GX_BLT_SRCCOPY) ? true : false;
        m_alpha    = _alpha;
    }
    
    void _done(double _xDst, double _yDst, double _wDst, double _hDst,
        GdkPixbuf* pb, double _xSrc, double _ySrc, double _wSrc, double _hSrc)
    {
        cairo_rectangle(m_hDC, _xDst, _yDst, _wDst, _hDst);
        cairo_clip(m_hDC);
        cairo_translate(m_hDC, _xDst, _yDst);
        cairo_scale(m_hDC, _wDst / _wSrc, _hDst / _hSrc);
        if (m_bSrcCopy)
            cairo_set_operator(m_hDC, CAIRO_OPERATOR_SOURCE);
        gdk_cairo_set_source_pixbuf(m_hDC, pb, -_xSrc, -_ySrc);
        cairo_paint_with_alpha(m_hDC, (double)m_alpha / 255.0);
    }
    
    void _clear(int _xDst, int _yDst, int _wDst, int _hDst) {}
};
typedef CBltDeviceGtk CBltDeviceGDI;
#else
#   error not support
#endif // TM_WIN

struct CBltR2L : public CBltDeviceGDI
{
protected:
    CBltR2L(TMHDC hdc, TMUInt32 uFlag)
        : CBltDeviceGDI(hdc, uFlag)
    {
    }

    void Done(const CRect& rcDst)
    {
        DbgAssert(this->IsGraphicsModeAdv());
        CAffine aff;
        aff.Reset();
        if (m_uFlag & GX_BLT_X2Y)
        {
            aff.Rotate(-90);
            aff.FlipX(0);
            if (m_uFlag & GX_BLT_R2L)
            {
                CAffine aff2;
                aff2.Reset();
                aff2.FlipY(rcDst.Left() + rcDst.Right());
                aff *= aff2;
            }
            if (m_uFlag & GX_BLT_B2T)
            {
                CAffine aff2;
                aff2.Reset();
                aff2.FlipX(rcDst.Top() + rcDst.Bottom());
                aff *= aff2;
            }
        }
        else if (m_uFlag & GX_BLT_R2L)
        {
            aff.FlipX(rcDst.Left() + rcDst.Right());
            TM_IF_WIN(m_bR2L = !m_bR2L);
        }
        else if (m_uFlag & GX_BLT_B2T)
        {
            aff.FlipY(rcDst.Top() + rcDst.Bottom());
        }
        DbgVerify(this->LeftMultiplyWorldTransform(&aff));
    }

public:
    void Init(const CRect& rcDst)
    {
        if (m_uFlag & (GX_BLT_R2L | GX_BLT_X2Y | GX_BLT_B2T))
            Done(rcDst);
    }

    void Init(const CRectF* rcDest, CZoom dpi)
    {
        if (m_uFlag & (GX_BLT_R2L | GX_BLT_X2Y | GX_BLT_B2T))
        {
            TMRect rc;
            dpi.LPtoDP(*rcDest, rc);
            Done(rc);
        }
    }
};

#define BLT

struct CBltRect
{
    int lDst;
    int tDst;
    int wDst;
    int hDst;
    int lSrc;
    int tSrc;
    int wSrc;
    int hSrc;

    void InitRect(const CRect& rcDst, const CRect& rcSrc, TMUInt32 uFlag, int w, int h)
    {
        lDst = rcDst.Left();
        tDst = rcDst.Top();
        wDst = (uFlag & GX_BLT_DFTDSTWIDTH) ? w : rcDst.Width();
        hDst = (uFlag & GX_BLT_DFTDSTHEIGHT) ? h : rcDst.Height();
        lSrc = rcSrc.Left();
        tSrc = rcSrc.Top();
        wSrc = (uFlag & GX_BLT_DFTSRCWIDTH) ? w : rcSrc.Width();
        hSrc = (uFlag & GX_BLT_DFTSRCHEIGHT) ? h : rcSrc.Height();
    }
};



struct CBltSelect
    : public CBltR2L
    , public CBltRect
{
    // for blt
    CBltSelect(TMHDC hdc, const TMRect& rcDst, const TMRect& rcSrc, TMUInt32 uFlag, TMUInt8 _alpha, int w, int h, EGXFormat format)
        : CBltR2L(hdc, uFlag)
    {
        CBltRect::InitRect(rcDst, rcSrc, uFlag, w, h);
        m_alpha = _alpha;
        CBltR2L::Init(CRect(lDst, tDst, lDst + wDst, tDst + hDst));
        InitFunc(uFlag, _alpha, format);
    }

public:
    void Blt(src_dc_type hSrcDC)
    {
        _clear(lDst, tDst, wDst, hDst);
        _done(lDst, tDst, wDst, hDst, hSrcDC, lSrc, tSrc, wSrc, hSrc);
    }
};


struct CBltsSelect
    : public CBltR2L
{
public:
    // for blts, tile
    CBltsSelect(TMHDC hdc, const CRectF* rcDest, CZoom dpi, TMUInt32 uFlag, TMUInt8 _alpha, EGXFormat format)
        : CBltR2L(hdc, uFlag)
    {
        CBltR2L::Init(rcDest, dpi);
        InitFunc(uFlag, _alpha, format);
    }

    void Tile(const CRect& rcd, src_dc_type hSrcDC, CZoom dpi, int sw, int sh)
    {
        CPointI pt;
        GXPoint ptSrc(sw, sh);
        dpi.LPtoDP(&ptSrc, 1, &pt);
        int w  = pt.X();
        int h  = pt.Y();

        int l  = rcd.Left();
        int t  = rcd.Top();
        int rr = rcd.Right();
        int bb = rcd.Bottom();

        int n = this->SaveDC();
        this->IntersectClipRect(l, t, rr, bb);
        _clear(rcd.Left(), rcd.Top(), rcd.Width(), rcd.Height());
        for (int r = l; r < rr; r += w)
        {
            for (int b = t; b < bb; b += h)
                _done(r, b, w, h, hSrcDC, 0, 0, sw, sh);
        }
        this->RestoreDC(n);
    }

    void SBlt(const CRect& rcd, src_dc_type hSrcDC, const CRect& rcs)
    {
        _clear(rcd.Left(), rcd.Top(), rcd.Width(), rcd.Height());
        _done(rcd.Left(), rcd.Top(), rcd.Width(), rcd.Height(),
            hSrcDC, rcs.Left(), rcs.Top(), rcs.Width(), rcs.Height());
    }
};



void _ColorFill(CDibHandle32 d32, TMColor clr, const TMRect* rc)
{
    CDibHandle32::value_type c;
    c.SetColor(clr);
    if (!rc)
        DibFastFill(d32, c);
    else
    {
        CClipDib<CDibHandle32> dib(d32);
        dib.SetClip(*rc);
        dib.CheckClip();
        DibFill(dib, c);
    }
}


TM_END_NAMESPACE

#endif // GXTEXTURE_BLT_H
