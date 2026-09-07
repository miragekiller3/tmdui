/**
 *  @file
 *  @brief  Device-Independent Bitmaps
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2005.03.09
 *
 *  Copyright (C) 2005 miragekiller
 */

#pragma once

#ifndef TMDIB_H
#define TMDIB_H

#include "tmgdi.h"
#include "tmcolor.h"


#   define DWORD_ALIGNED(x)         ((((x)+3) >> 2) << 2)
#   define DWORD_WBYTES(x)          ((((x)+31)>>5)<<2)
#   define IMAGECOLORS(x,y)         (1<<((x)*(y)))

// TMDibBits : bits + stride;
// TMDibBuff : TMDibBits + height;
// TMDibView : TMDibBuff + width;
// TMDib     : TMDibView + format;


// this is a dib
// a-> 000000001111111111100000000
//     000000010000000000010000000
// b-> 000000001111111111100000000
//
// m_stride m_height    m_bits  dib type                        platform
// >0       <0          a       bottom-up DIB(windows dib)      Window GDI
// >0       >0          a       Top-down DIB                    D3D,OpenGL,Apple
// <0       >0          b       bottom-up DIB(windows dib)      some of "LockRect" function
// <0       <0                  error  

// init:
//  bit bottom-up DIB(windows dib):
//      m_height<0; m_stride>0; m_bits:pointer to lefttop(pixel(0, height - 1));
//  Top-down DIB:
//      m_height>0; m_stride>0; m_bits:pointer to lefttop(pixel(0, 0));
//
// after LockRect(x,y):
//  bottom-up DIB(windows dib):
//      m_height>0; m_stride<0; m_bits:pointer to pixel(x, height - y - 1);
//  Top-down DIB:
//      m_height>0; m_stride>0; m_bits:pointer to pixel(x, y);

///////////////////////////////////////////////////////////////////////////////
// TMDibBits
struct TMDibBits
{
    void*   m_bits;
    int     m_stride;           //can less than 0.

    void _Check() const
    {
        if(m_bits)
            DbgAssert(m_stride);
        else
            DbgAssert(!m_stride);
    }

    TMDibBits() : m_bits(0) , m_stride(0) {}

    TMDibBits(void* p, int wb) : m_bits(p) , m_stride(wb)
    {
        _Check();
    }

    operator bool() const { return m_bits != 0; }
    
    void Set(void* p = 0, int wb = 0)
    {
        m_bits = p;
        m_stride = wb;
        _Check();
    }
    
    void Set(void* p, int w, int bpp)
    {
        DbgAssert(w >= 0);
        m_bits = p;
        m_stride = DWORD_WBYTES(w * bpp);
        _Check();
    }
    
    int GetStride() const { _Check(); return m_stride; }
    unsigned int GetWidthByte() const { _Check(); return m_stride > 0 ? m_stride : -m_stride; }
    TMUInt8* GetData() const { _Check(); return (TMUInt8*)m_bits; }
    TMUInt8* GetLineData(int nLine) const { return GetData() + nLine * GetStride(); }
    TMUInt8* GetPixelData(int x, int y, int nBpp) { return GetLineData(y) + x * nBpp / 8; }
};

///////////////////////////////////////////////////////////////////////////////
// TMDibBuff = TMDibBits + height
struct TMDibBuff : public TMDibBits
{
    int     m_height;

    void _Check() const
    {
        if(m_bits)
            DbgAssert(m_height && m_stride);
        else
            DbgAssert(!m_height && !m_stride);
    }

    TMDibBuff() : m_height(0) {}
    
    TMDibBuff(void* p, int wb, int h) : TMDibBits(p, wb) , m_height(h)
    {
        _Check();
    }

    void Set(void* p = 0, int wb = 0, int h = 0)
    {
        TMDibBits::Set(p, wb);
        m_height = h;
        _Check();
    }

    void Set(void* p, int w, int h, int bpp)
    {
        DbgAssert(w >= 0);
        Set(p, DWORD_WBYTES(w * bpp), h);
    }

    int GetHeight() const { return m_height > 0 ? m_height : -m_height; }

    unsigned long GetSizeImage() const { return GetWidthByte() * GetHeight(); }
    
    TMUInt8* GetNativeMemBuffer() const
    {
        if (m_stride >= 0 || m_height == 1)
            return GetData();
        else
            return GetData() + m_stride * (GetHeight() - 1);
    }

    TMUInt8* GetLineData(int nLine) const
    {
        if (m_height < 0)
            return GetData() + (-m_height - nLine - 1) * GetWidthByte();
        else
            return GetData() + nLine * GetStride();
    }
};

///////////////////////////////////////////////////////////////////////////////
// TMDibView = TMDibBuff + width
struct TMDibView : public TMDibBuff
{
    int     m_width;

    void _Check() const
    {
        if(m_bits)
            DbgAssert(m_height && m_stride && m_width > 0);
        else
            DbgAssert(!m_height && !m_stride && !m_width);
    }


    TMDibView() : m_width(0) {}

    TMDibView(int w, int h, void* pBits, int wb)
        : TMDibBuff(pBits, wb, h)
        , m_width(w)
    {
        _Check();
    }

    TMDibView(const TMDibView& dib)
        : TMDibBuff(dib)
        , m_width(dib.m_width)
    {
        _Check();
    }

    TMDibView(int w, const TMDibBuff& dib)
        : TMDibBuff(dib)
        , m_width(w)
    {
        _Check();
    }

    void Set(void* p, int w, int h, int bpp)
    {
        Set(w, h, p, DWORD_WBYTES(w * bpp));
    }

    void Set(int w, int h, void* p, int wb)
    {
        m_width = w;
        TMDibBuff::Set(p, wb, h);
        _Check();
    }

    int GetWidth() const { return m_width; }

    unsigned long GetPixelCount() const { return GetWidth() * GetHeight(); }
};


///////////////////////////////////////////////////////////////////////////////
// dib functions
enum EImageFormat
{
    eImageFormatUnknown = 0,
    eImageFormatBmp,    //  = 1,
    eImageFormatGif89,  //  = 2,
    eImageFormatGif87,  //  = 3,
    eImageFormatIco,    //  = 4,
    eImageFormatCursor, //  = 5,
    eImageFormatSVG,    //  = 6,
    eImageFormatPng,    //  = 7,
    eImageFormatJpg,    //  = 8,
    eImageFormatWmf,    //  = 9,
    eImageFormatJason,  //  = 10,
};

TM_CAPI EImageFormat DibFormatFromMem(const void* pData);
TM_CAPI EImageFormat DibFormatFromStream(struct IStream* stream);

void DibBltRef(TMDibBuff& __d, const TMDibBuff& __s);

template <class DIBn, class F>
void DibEnumPixel(DIBn& dib, F __f);

template <class DIBn, class F>
void DibFastEnumPixel(DIBn& dib, F __f);

template <class DIBn, class T_Color>
void DibFill(DIBn& dib, T_Color clr);

template <class DIBn, class T_Color>
void DibFastFill(DIBn& dib, T_Color clr);

template <class DIBn, class F>
TMBool DibFastSearch(const DIBn& dib, F __f);

template <class D, class S>
void DibBlt(D& __d, int l, int t, const S& __s);

template <class D, class S>
void DibStretch(D& __d, const S& __s);

template <class D, class S, class F>
void DibBltF(D& __d, int l, int t, const S& __s, const F& __f);

template <class D, class S>
TMBool DibFind(const D& __d, const S& __s, TMPoint& pt);

template <class D>
TMUInt32 DibHash(const D& __d);

template <class D, class S1, class S2>
void DibMix(D& __d, const S1& __s1, const S2& __s2, TMUInt8 alpha);

template <class D, class S>
void DibMix(D& __d, const S& __s, TMUInt8 alpha);

TMBool DibResetAsDib(TMDibBuff& dest, EGXFormat fmtDst, const struct GXDibData& dibData);

// multiply src alpha to dest.
// D: CDibXX; S:CDibXX
// dest.r = (dest.r * (255 - src.alpha) + src.r * src.alpha) / 255;
// dest.g = (dest.g * (255 - src.alpha) + src.g * src.alpha) / 255;
// dest.b = (dest.b * (255 - src.alpha) + src.b * src.alpha) / 255;
// dest.a = (dest.a * (255 - src.alpha) + src.a * src.alpha) / 255;
template <class D, class S>
void DibReplaceAlpha(D& __dst, const S& __src);

//TM_INLINE void DibPreBlend(TM::CDibHandle32& dib);


///////////////////////////////////////////////////////////////////////////////
// CDibIterHelper<n>
// pixel interator, n indicates bits of color
TM_BGN_NAMESPACE

#define DECLARE_DIBITERATOR_OPERATOR(S,c) \
    TMBool operator == (const S& rhs) const { return rhs.m_pData == m_pData; } \
    TMBool operator != (const S& rhs) const { return rhs.m_pData != m_pData; } \
    S& operator ++ () { m_pData += c; return *this; } \
    S& operator += (int n) { m_pData += n * c; return *this; } \
    S  operator ++ (int)   const { S tmp(*this); ++tmp; return tmp; } \
    S  operator +  (int n) const { S tmp(*this); tmp+=n; return tmp; } \
    S& operator -- () { m_pData -= c; return *this; } \
    S& operator -= (int n) { m_pData -= n * c; return *this; } \
    S  operator -- (int)   const { S tmp(*this); --tmp; return tmp; } \
    S  operator -  (int n) const { S tmp(*this); tmp-=n; return tmp; }

///////////////////////////////////////////////////////////////////////////
// 256 color bitmap
struct CDibIterHelper8
{
#ifndef TM_WIN
    struct RGBQUAD { TMUInt8 rgbBlue, rgbGreen, rgbRed, rgbReserved; };
#endif

    template<class Pal, class PData, class Ref, class Index>
        class _iterator
    {
    public:
        Pal     m_pPal;
        PData   m_pData;

    public:
        _iterator(Pal pPal, PData pData)
            : m_pPal(pPal)
            , m_pData(pData)
        {}

        _iterator(const _iterator<RGBQUAD*, TMUInt8*, DibBGR&, TMUInt8&>& rhs)
            : m_pPal(rhs.m_pPal)
            , m_pData(rhs.m_pData)
        {}

        _iterator& operator = (const _iterator<RGBQUAD*, TMUInt8*, DibBGR&, TMUInt8&>& rhs)
        {
            m_pPal = rhs.m_pPal;
            m_pData = rhs.m_pData;
        }

        Ref operator * () const
        {
            return (Ref) (m_pPal[m_pData[0]]);
        }

        Ref operator [] (int n) const
        {
            return (Ref)(m_pPal[m_pData[n]]);
        }

        Index GetIndex() const { return *m_pData; }
        Index GetIndex(int n) const { return m_pData[n]; }

        DECLARE_DIBITERATOR_OPERATOR(_iterator, 1);
    };
    // _iterator

    typedef DibBGR                                  value_type;
    typedef _iterator < RGBQUAD*,
        TMUInt8*, value_type&, TMUInt8& >           iterator;
    typedef _iterator < const RGBQUAD*, const TMUInt8*,
        const value_type&, const TMUInt8& >         const_iterator;

    enum { eBitCount = 8 };
};
// CDibIterHelper8


///////////////////////////////////////////////////////////////////////////
// CDibIterHelperT
// T: DibRGB, DibARGB, DibGRAY, ...
template <class T>
struct CDibIterHelperT
{
    template <class PData, class Ref>
    class _iterator
    {
    public:
        enum { eBitByte = T::eBitCount / 8 };
        PData m_pData;
    
    public:
        _iterator(PData pData)
            : m_pData(pData)
        {}
    
        template <class _PData, class _Ref>
        _iterator(const _iterator<_PData, _Ref>& rhs)
            : m_pData(rhs.m_pData)
        {}
    
        template <class _PData, class _Ref>
        _iterator& operator = (const _iterator<_PData, _Ref>& rhs)
        {
            m_pData = rhs.m_pData;
        }
    
        Ref operator * () const
        {
            return *(T*)(m_pData);
        }
    
        Ref operator [] (int n) const
        {
            return *(T*)(m_pData + n * eBitByte);
        }
    
        DECLARE_DIBITERATOR_OPERATOR(_iterator, eBitByte);
    };
    // _iterator
    
    typedef T                                           value_type;
    typedef _iterator<TMUInt8*, T&>                     iterator;
    typedef _iterator<const TMUInt8*, const T&>         const_iterator;
    
    enum { eBitCount = T::eBitCount };
    
    template <class _DIB>
    static inline iterator Get(_DIB& dib, int h) { return dib.GetLineData(h); }
    template <class _DIB>
    static inline const_iterator GetC(const _DIB& dib, int h) { return dib.GetLineData(h); }
};
// CDibIterHelper24

struct CDibIterHelper24
{
    template < class PData, class Ref >
        class _iterator
    {
    public:
        PData   m_pData;

    public:
        _iterator(PData pData)
            : m_pData(pData)
        {}

        _iterator(const _iterator<TMUInt8*, DibRGB&>& rhs)
            : m_pData(rhs.m_pData)
        {}

        _iterator& operator = (const _iterator<TMUInt8*, DibRGB&>& rhs)
        {
            m_pData = rhs.m_pData;
        }

        Ref operator * () const
        {
            return * (DibRGB*)(m_pData);
        }

        Ref operator [] (int n) const
        {
            return * (DibRGB*)(m_pData + n * 3);
        }

        DECLARE_DIBITERATOR_OPERATOR(_iterator, 3);
    };
    // _iterator

    typedef DibRGB                                  value_type;
    typedef _iterator < TMUInt8*, value_type& >     iterator;
    typedef _iterator < const TMUInt8*,
        const value_type& >                         const_iterator;

    enum { eBitCount = 24 };

    template <class _DIB>
    static inline iterator Get(_DIB& dib, int h) {
        return dib.GetLineData(h); }
    template <class _DIB>
    static inline const_iterator GetC(const _DIB& dib, int h) {
        return dib.GetLineData(h); }
};
// CDibIterHelper24


///////////////////////////////////////////////////////////////////////////
// >= 32bit  bitmap
template <class T>
struct CDibIterHelperG32
{
    typedef T                                       value_type;
    typedef value_type*                             iterator;
    typedef const value_type*                       const_iterator;

    enum { eBitCount = sizeof(T) * 8 };

    template <class _DIB>
    static inline iterator Get(_DIB& dib, int h) {
        return (iterator) dib.GetLineData(h); }
    template <class _DIB>
    static inline const_iterator GetC(const _DIB& dib, int h) {
        return (const_iterator)dib.GetLineData(h); }
};
typedef CDibIterHelperG32<DibARGB>                  CDibIterHelper32;

// CDibIterHelper32


#undef DECLARE_DIBITERATOR_OPERATOR




TM_END_NAMESPACE




///////////////////////////////////////////////////////////////////////////////
// CDibT<n>
// specialize classes for different bit count
TM_BGN_NAMESPACE


// T is CDibIterHelperX
template <class T>
class CDibT : public TMDibView
{
public:
    typedef CDibT                                   dib_type;
    typedef typename T::iterator                    iterator;
    typedef typename T::const_iterator              const_iterator;
    typedef typename T::value_type                  value_type;
    typedef value_type&                             reference;
    typedef const value_type&                       const_reference;
    typedef value_type*                             pointer;
    typedef const value_type*                       const_pointer;

protected:
    struct _Fill : public value_type
    {
        void operator()(reference pixel) const
        {
            pixel = *this;
        }
    };

public:
    enum { eBitCount            = T::eBitCount };
    enum { e_nByteCount         = eBitCount / 8 };
    enum { e_bRegular           = true };       // GetPixel() always return valid pixel
    enum { e_bSuppotFastEnum    = true };       // support DibFastXXX

public:
    CDibT() {}

    CDibT(int w, int h, void* pBits)
        : TMDibView(w, h, pBits, DWORD_WBYTES(w * eBitCount))
    {
    }

    CDibT(int w, int h, void* pBits, int wb)
        : TMDibView(w, h, pBits, wb)
    {
    }

    CDibT(const CDibT& dib)
        : TMDibView(dib)
    {
    }

    CDibT(const TMDibBuff& dib)
        : TMDibView(dib.GetWidthByte() / e_nByteCount, dib)
    {
    }

    CDibT(int w, const TMDibBuff& dib)
        : TMDibView(w, dib)
    {
    }

    void Set(void* p, int w, int h) // w >= 0; h can < 0;
    {
        TMDibView::Set(p, w, h, eBitCount);
    }

    int GetBitCount() const { return eBitCount; }

    iterator GetLine(int y) { return T::Get(*this, y); }
    const_iterator GetLine(int y) const { return T::GetC(*this, y); }

    reference GetPixel(int x, int y)
    {
        DbgAssert(x >= 0 && x < GetWidth());
        DbgAssert(y >= 0 && y < GetHeight());
        return GetLine(y)[x];
    }

    const_pointer GetPixel(int x, int y) const
    {
        DbgAssert(x >= 0 && x < GetWidth());
        DbgAssert(y >= 0 && y < GetHeight());
        return &(GetLine(y)[x]);
    }

    void LockRect(CDibT& that, int x, int y)
    {
        that.m_bits = &GetPixel(x, y);
        if(m_height < 0)
        {
            that.m_stride = -m_stride;
            that.m_height = -m_height - y;
        }
        else
        {
            that.m_stride = m_stride;
            that.m_height = m_height - y;
        }
    }

    template <class S>
    void BltFrom(int l, int t, const S& __s)
    {
        DbgAssert(*this);
        DibBlt(*this, l, t, __s);
    }

    template <class D, class F>
        void BltFromEx(int l, int t, const D& __d, const F& __f)
    {
        DbgAssert(*this);

        int w = __d.GetWidth();
        int h = __d.GetHeight();
        for(int j = 0; j < h; j++)
        {
            for(int i = 0; i < w; i++)
            {
                typename D::const_pointer p = __d.GetPixel(i, j);
                if(D::e_bRegular || p)
                {
                    __f(GetPixel(l + i, t + j), *p);
                }
            }
        }
    }

    void Fill(TMColor clr)
    {
        _Fill v;
        v.SetColor(clr);
        DibFastEnumPixel(*this, v);
    }

    void DebugOut(int l = 0, int t = 0)
    {
#ifdef TM_WIN
        BITMAPINFO info = { sizeof(BITMAPINFOHEADER), GetWidth(), GetHeight(), 1, GetBitCount() };
        info.bmiHeader.biSizeImage = GetSizeImage();
        
        int h = abs(GetHeight());
        
        int l2 = 0;
        int t2 = 0;
        int w2 = GetWidth();
        int h2 = GetHeight();
        
        HDC hdc = GetDC(0);
        RECT rc = {l, t, l + w2, t + h2};
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(DKGRAY_BRUSH));
        SetDIBitsToDevice(hdc,
            l, t, w2, h2, l2, t2, h-t2-h2, h2, GetData(), &info, DIB_RGB_COLORS);
        ReleaseDC(0, hdc);
#endif
    }
};
// CDibT


//typedef CDibT<CDibIterHelper8>                  CDib8;  // 256color
typedef CDibT<CDibIterHelper24>                  CDibHandle24;
//typedef CDibT<CDibIterHelper32 >                 CDibHandle32;
typedef CDibT<CDibIterHelperT<DibARGB> >         CDibHandle32;
typedef CDibT<CDibIterHelperT<DibGRAY> >         CDibHandleGray;
typedef CDibT<CDibIterHelperT<DibA> >            CDibHandleA;


TM_END_NAMESPACE



////////////////////////////////////////////////////////////////////////////////
//effect class
TM_BGN_NAMESPACE


#define DECLARE_DIB_TYPE(c,x) \
    typedef c                                           class_type; \
    typedef x                                           base_class; \
    typedef typename x::dib_type                        dib_type; \
    typedef typename x::iterator                        iterator; \
    typedef typename x::const_iterator                  const_iterator; \
    typedef typename x::value_type                      value_type; \
    typedef typename x::reference                       reference; \
    typedef typename x::const_reference                 const_reference; \
    typedef typename x::pointer                         pointer; \
    typedef typename x::const_pointer                   const_pointer;



////////////////////////////////////////////////////////////////////////////////
// True Dib
// the 538 width dib's WidthByte == 540 width dib's WidthByte, so make True Dib
// T: CDibHandleXXX
template <class T>
class CTrueDib : public T
{
public:
    DECLARE_DIB_TYPE(CTrueDib, T)
    
private:
    int m_w;
    
public:
    CTrueDib(const dib_type& dib)
        : T(dib)
        , m_w(base_class::GetWidth())
    {
    }
    
    void SetWidth(int w)
    {
        m_w = w;
    }
    
    int GetWidth() const { return m_w; }
};


////////////////////////////////////////////////////////////////////////////////
// Sub Dib
template < class T >
class CClipDib : public T
{
public:
    DECLARE_DIB_TYPE(CClipDib, T)
    enum { e_bSuppotFastEnum = false };  // support DibFastXXX

private:
    int m_l, m_t, m_w, m_h;

public:
    CClipDib(const dib_type& dib)
        : T(dib)
        , m_l(0)
        , m_t(0)
        , m_w(base_class::GetWidth())
        , m_h(base_class::GetHeight())
    {
    }

    void SetClip(const TMRect& rc)
    {
        SetClip(RectLeft(rc),  RectTop(rc), RectWidth(rc), RectHeight(rc));
    }

    void SetClip(int l, int t, int w, int h)
    {
        m_l = l; m_t = t; m_w = w; m_h = h;
    }

    CRect GetClip() const { CRect rc; rc.SetLTWH(m_l, m_t, m_w, m_h); return rc; }

    void CheckClip()
    {
        int w = base_class::GetWidth();
        int h = base_class::GetHeight();

        if (m_l < 0) m_l = 0;
        if (m_t < 0) m_t = 0;
        if (m_w > w) m_w = w;
        if (m_h > h) m_h = h;
    }

    int GetWidth() const { return m_w; }
    int GetHeight() const { return m_h; }

    const_iterator GetLine(int y) const
    { return T::GetLine(y + m_t) + m_l; }

    reference GetPixel(int x, int y)
    { return T::GetPixel(x + m_l, y + m_t); }

    const_pointer GetPixel(int x, int y) const
    { return T::GetPixel(x + m_l, y + m_t); }
};

////////////////////////////////////////////////////////////////////////////////
// rotate Dib
// anti-clockwise turn by n degree, n can be set 0,90,180,270,360,...
template < class T, int n >
class CFixedRotateDib : public T
{
public:
    DECLARE_DIB_TYPE(CFixedRotateDib, T);
    DbgAssertS((n % 90) == 0);

    enum { e_bHorizontal = ((n % 180) == 0) };
    enum { e_nAngle = n % 360 };

public:
    CFixedRotateDib(const dib_type& dib) : T(dib) {}

    int GetWidth() const
    {
        if(e_bHorizontal)
            return T::GetWidth();
        else
            return T::GetHeight();
    }

    int GetHeight() const
    {
        if (e_bHorizontal)
            return T::GetHeight();
        else
            return T::GetWidth();
    }

    // override  GetLine in base class for not support this function
    void GetLine() const {};

    const_pointer GetPixel(int x, int y) const
    {
        if (e_nAngle == 270)
            return T::GetPixel(y, GetWidth() - x - 1);
        else if (e_nAngle == 180)
            return T::GetPixel(GetWidth() - x - 1, GetHeight() - y - 1);
        else if (e_nAngle == 90)
            return T::GetPixel(GetHeight() - y - 1, x);
        else
            return T::GetPixel(x, y);
    }
};
// CFixedRotateDib


////////////////////////////////////////////////////////////////////////////////
// CRotateDib
// rotate a dib
template <class T>
class CRotateDib : public T
{
private:
    int m_sn;
    int m_cs;
    int m_nNewW;
    int m_nNewH;
    int m_nOldW;
    int m_nOldH;
    int m_nDW;
    int m_nDH;

    int _Round(double x) const
    {
        return (int)(x > 0 ? x + 0.5 : x - 0.5);
    }

    inline int _GetY(int nx, int ny) const
    {
        return ((-nx * m_sn + ny * m_cs) >> 10) + m_nDH;
    }

    inline int _GetX(int nx, int ny) const
    {
        return ((nx * m_cs + ny * m_sn) >> 10) + m_nDW;
    }

public:
    DECLARE_DIB_TYPE(CRotateDib, T);
    enum { e_bRegular = false };  // GetPixel() maybe return 0;

public:
    CRotateDib(const dib_type& dib)
        : T(dib)
    {}

    void SetRadian(double x)
    {
        m_sn = _Round(sin(x) * 1024);
        m_cs = _Round(cos(x) * 1024);
        m_nOldW = T::GetWidth();
        m_nOldH = T::GetHeight();
        m_nNewH = (abs(m_nOldW * m_sn) + abs(m_nOldH * m_cs)) >> 10;
        m_nNewW = (abs(m_nOldW * m_cs) + abs(m_nOldH * m_sn)) >> 10;
        m_nDW = (((-m_nNewW * m_cs - m_nNewH * m_sn) >> 10) + m_nOldW) / 2 ;
        m_nDH = (((m_nNewW * m_sn - m_nNewH * m_cs) >> 10) + m_nOldH) / 2 ;
        m_nNewW = abs(m_nNewW);
        m_nNewH = abs(m_nNewH);
    }

    void SetAngle(int nAngle)
    {
        SetRadian((double)nAngle * 3.14159f / 180.0f);
    }

    int GetWidth() const
    {
        return m_nNewW;
    }

    int GetHeight() const
    {
        return m_nNewH;
    }

    // override  GetLine in base class for not support this function
    void GetLine() const {};

    const_pointer GetPixel(int x, int y) const
    {
        int xx = _GetX(x, y);
        if (xx >= 0 && xx < m_nOldW)
        {
            int yy = _GetY(x, y);
            if (yy >= 0 && yy < m_nOldH)
            {
                return T::GetPixel(xx, yy);
            }
        }
        return 0;
    }

}; // CRotateDib


////////////////////////////////////////////////////////////////////////////////
// CAffineDib
template <class T>
class CAffineDib : public T
{
private:
    int m_nNewW;
    int m_nNewH;
    int m_nOldW;
    int m_nOldH;

    int eM11;
    int eM12;
    int eM21;
    int eM22;
    int eDx;
    int eDy;

//     const XFORM* p;

    enum { eMoveBit = 10 };
    enum { eScale = 1 << eMoveBit };

    int _Round(float x) const
    {
        x *= eScale;
        return (int)(x > 0 ? x + 0.5 : x - 0.5);
    }

    int _GetX(int x, int y) const
    {
        return (eM11 * x + eM21 * y + eDx) >> eMoveBit;
    }

    int _GetY(int x, int y) const
    {
        return (eM12 * x + eM22 * y + eDy) >> eMoveBit;
    }

public:
    DECLARE_DIB_TYPE(CAffineDib, T);
    enum { e_bRegular = false };  // GetPixel() maybe return 0;

public:
    CAffineDib(const dib_type& dib)
        : T(dib)
    {}

    template <class V>
    void SetAffine(const CAffineT<V>& xForm)
    {
        eM11 = _Round(xForm.eM11);
        eM12 = _Round(xForm.eM12);
        eM21 = _Round(xForm.eM21);
        eM22 = _Round(xForm.eM22);
        eDx  = _Round(xForm.eDx);
        eDy  = _Round(xForm.eDy);

//         p = &xForm;

        m_nOldW = T::GetWidth();
        m_nOldH = T::GetHeight();

        int x1 = _GetX(m_nOldW, m_nOldH);
        int y1 = _GetY(m_nOldW, m_nOldH);

        int x2 = _GetX(0, 0);
        int y2 = _GetY(0, 0);

        int x3 = _GetX(0, m_nOldH);
        int y3 = _GetY(0, m_nOldH);

        int x4 = _GetX(m_nOldW, 0);
        int y4 = _GetY(m_nOldW, 0);

        m_nNewW = tm_max(x1, x2);
        m_nNewW = tm_max(m_nNewW, x3);
        m_nNewW = tm_max(m_nNewW, x4);

        m_nNewH = tm_max(y1, y2);
        m_nNewH = tm_max(m_nNewH, y3);
        m_nNewH = tm_max(m_nNewH, y4);
    }

    int GetWidth() const
    {
        return m_nNewW;
    }

    int GetHeight() const
    {
        return m_nNewH;
    }

    // override  GetLine in base class for not support this function
    void GetLine() const {};

    const_pointer GetPixel(int x, int y) const
    {
        int xx = _GetX(x, y);
        if (xx >= 0 && xx < m_nOldW)
        {
            int yy = _GetY(x, y);
            if (yy >= 0 && yy < m_nOldH)
            {
                return T::GetPixel(xx, yy);
            }
        }
        return 0;
    }

}; // CRotateDib


////////////////////////////////////////////////////////////////////////////////
// flip Dib
// take the n degree axis to make mirror image, n can be set 0,45,90,135,180,...
template < class T, int n >
class CFlipDib : public T
{
public:
    DECLARE_DIB_TYPE(CFlipDib, T);
    DbgAssertS((n % 45) == 0);

    enum { e_bHorizontal = ((n % 90) == 0) };
    enum { e_nAngle = n % 180 };

public:
    CFlipDib(const dib_type& dib) : T(dib) { }

    int GetWidth() const
    {
        if (e_bHorizontal)
            return T::GetWidth();
        else
            return T::GetHeight();
    }

    int GetHeight() const
    {
        if (e_bHorizontal)
            return T::GetHeight();
        else
            return T::GetWidth();
    }

    // override  GetLine in base class for not support this function
    void GetLine() const {};

    const_pointer GetPixel(int x, int y) const
    {
        if (e_nAngle == 0)
            return T::GetPixel(x, T::GetHeight() - y - 1);
        else if (e_nAngle == 90)
            return T::GetPixel(T::GetWidth() - x - 1, y);
        else if (e_nAngle == 45)
            return T::GetPixel(T::GetWidth() - y - 1, T::GetHeight() - x - 1);
        else
            return T::GetPixel(y, x);
    }
};
// CFlipDib


////////////////////////////////////////////////////////////////////////////////
// Scale Dib
// Zoom to a new length and width
struct CScaleIndex
{
    int     m_nLengh;
    int*    m_pIndex;

    CScaleIndex() : m_nLengh(0), m_pIndex(0) {}

    ~CScaleIndex() { delete []m_pIndex; }

    void Build(int nOld, int nNew)
    {
        --nOld;
        DbgAssert(nOld >= 0);

        m_nLengh = nNew;
        m_pIndex = new int[nNew];
        for (int i = 0; i < nNew; i++)
            m_pIndex[i] = nOld * i / nNew;
    }

    void BuildShape(int nOld, int nNew, int nLeft, int nRight)
    {
        --nOld;
        DbgAssert(nOld >= 0);
        DbgAssert(nLeft >= 0 && nRight >= 0);
        DbgAssert(nLeft + nRight <= nOld);
        DbgAssert(nLeft + nRight <= nNew);

        m_nLengh = nNew;
        m_pIndex = new int[nNew];

        for (int i = 0; i < nLeft; ++i)
            m_pIndex[i] = i;
        for (int j = nNew - nRight; j < nNew; ++j)
            m_pIndex[j] = j - nNew + nOld;

        nOld -= (nLeft + nRight);
        nNew -= (nLeft + nRight);
        for (int k = 0; k < nNew; ++k)
            m_pIndex[k+nLeft] = nLeft + nOld * k / nNew;
    }

    int operator[] (int n) const
    { DbgAssert(n>=0 && n < m_nLengh); return m_pIndex[n]; }
};

template < class T >
class CFastXScaleDib : public T
{
public:
    DECLARE_DIB_TYPE(CFastXScaleDib, T);

private:
    CScaleIndex m_siX;

public:
    CFastXScaleDib(const dib_type& dib) : T(dib) {}

    void SetScaleWidth(int nNewWidth)
    { m_siX.Build(base_class::GetWidth(), nNewWidth); }

    void SetShapeScaleWidth(int nNewWidth, int nLeft, int nRight)
    { m_siX.BuildShape(base_class::GetWidth(), nNewWidth, nLeft, nRight); }

    int GetWidth() const { return m_siX.m_nLengh; }

    //  override  GetLine in base class for not support this function
    void GetLine() const {};

    const_pointer GetPixel(int x, int y) const
    { return base_class::GetPixel(m_siX[x], y); }
};

template < class T >
class CFastYScaleDib : public T
{
public:
    DECLARE_DIB_TYPE(CFastYScaleDib, T);

private:
    CScaleIndex m_siY;

public:
    CFastYScaleDib(const dib_type& dib) : T(dib) {}

    void SetScaleHeight(int nNewHeight)
    { m_siY.Build(base_class::GetHeight(), nNewHeight); }

    void SetShapeScaleHeight(int nNewHeight, int nTop, int nBottom)
    { m_siY.BuildShape(base_class::GetHeight(), nNewHeight, nTop, nBottom); }

    long GetHeight() const { return m_siY.m_nLengh; }

    const_iterator GetLine(int y) const
    { return T::GetLine(m_siY[y]); }

    const_pointer GetPixel(int x, int y) const
    { return base_class::GetPixel(x, m_siY[y]); }
};


TM_END_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// pixel process functions

////////////////////////////////////////////////////////////////////////////////
// CAlphaBlend
// (src * a + dest * (256 - a)) / 256
// = (src * a - dest * a) / 256 + dest
// = (src - dest + 256) * a) / 256 - a + dest
// (src - dest) may be < 0; so add 256;
class CAlphaBlend
{
public:
    template<class T1, class T2>
        void operator() (TM::DibPixelT<T1>& dst, const TM::DibPixelT<T2>& src) const
    {
        DbgAssertS(sizeof(T2::value_type) == sizeof(T1::value_type) == sizeof(TMUInt8));

        // compose three 8-bit to one 64-bit,
        // three 8-bit processing can be replaced by one 64-bit multiplication
        // 0000000000000000rrrrrrrr000000000000gggggggg000000000000bbbbbbbb
        enum { M1 = 40, M2 = 20 };
        const TMUInt64 m = (((TMUInt64)0x100) << M1) + (((TMUInt64)0x100) << M2) + 0x100;

        TMUInt64 _a = src.GetA();
        TMUInt64 a = (_a << M1) + (_a << M2) + _a;
        TMUInt64 d = (((TMUInt64)dst.GetR()) << M1) + (((TMUInt64)dst.GetG()) << M2) + dst.GetB();
        TMUInt64 s = (((TMUInt64)src.GetR()) << M1) + (((TMUInt64)src.GetG()) << M2) + src.GetB();

        TMUInt64 r = (((s + m - d) * _a) >> 8) - a + d;

        TM::DibRGB rgb((TMUInt8)(r >> M1), (TMUInt8)(r >> M2), (TMUInt8)r, 0);
        dst.Set(rgb);
    }
};

struct CGray
{
    template <class T>
    void operator() (TM::DibPixelT<T>& dst) const
    {
        // Gray = R*0.299 + G*0.587 + B*0.114
        TMUInt x = (dst.GetR()*19595 + dst.GetG()*38469 + dst.GetB()*7472) >> 16;
        dst.SetR(x);
        dst.SetG(x);
        dst.SetB(x);
    }

    template <class T>
        void operator() (TM::DibPixelT<T>& dst, const TM::DibPixelT<T>& src) const
    {
        // Gray = R*0.299 + G*0.587 + B*0.114
        TMUInt x = (src.GetR()*19595 + src.GetG()*38469 + src.GetB()*7472) >> 16;
        dst.SetR(x);
        dst.SetG(x);
        dst.SetB(x);
    }
};

template <class DIBn, class F>
void DibEnumPixel(DIBn& dib, F __f)
{
    int x = dib.GetWidth();
    int y = dib.GetHeight();
    for (int i = 0; i < x; ++i)
    {
        for (int j = 0; j < y; ++j)
        {
            __f(dib.GetPixel(i, j));
        }
    }
}

template <class DIBn, class T_Color>
void DibFill(DIBn& dib, T_Color clr)
{
    typename DIBn::value_type __color = clr;
    int x = dib.GetWidth();
    int y = dib.GetHeight();
    for (int i = 0; i < x; ++i)
    {
        for (int j = 0; j < y; ++j)
            dib.GetPixel(i, j) = __color;
    }
}

template <class DIBn, class T_Color>
void DibFastFill(DIBn& dib, T_Color clr)
{
    DbgAssertS(DIBn::e_bSuppotFastEnum);
    typedef typename DIBn::value_type value_type;
    typename DIBn::value_type __color = clr;
    TMUInt8* p1 = dib.GetNativeMemBuffer();
    TMUInt8* p2 = p1 + dib.GetSizeImage();
    for (; p1 < p2; p1 += DIBn::e_nByteCount)
        *(value_type*)p1 = __color;
}

template <class DIBn, class F>
void DibFastEnumPixel(DIBn& dib, F __f)
{
    DbgAssertS(DIBn::e_bSuppotFastEnum);
    typedef typename DIBn::value_type value_type;
    TMUInt8* p1 = dib.GetNativeMemBuffer();
    TMUInt8* p2 = p1 + dib.GetSizeImage();
    for (; p1 < p2; p1 += DIBn::e_nByteCount)
        __f(*(value_type*)p1);
}

template <class DIBn, class F>
TMBool DibFastSearch(const DIBn& dib, F __f)
{
    typedef typename DIBn::value_type value_type;
    TMUInt8* p1 = dib.GetNativeMemBuffer();
    TMUInt8* p2 = p1 + dib.GetSizeImage();
    for (; p1 < p2; p1 += DIBn::e_nByteCount)
    {
        if (__f(*(value_type*)p1))
            return true;
    }
    return false;
}

template <class D, class S>
void DibBlt(D& __d, int l, int t, const S& __s)
{
    DbgAssert(__d);

    int w = __s.GetWidth();
    int h = __s.GetHeight();
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            typename S::const_pointer p = __s.GetPixel(i, j);
            if (S::e_bRegular || p)
                __d.GetPixel(l + i, t + j) = *p;
        }
    }
}

template <class D, class S>
void DibStretch(D& __d, const S& __s)
{
    int w1 = __d.GetWidth();
    int w2 = __d.GetWidth();
    int h1 = __d.GetHeight();
    int h2 = __s.GetHeight();
    if (w1 == w2 && h1 == h2)
    {
        if (__d.eBitCount == __s.eBitCount && __d.e_bSuppotFastEnum && __s.e_bSuppotFastEnum)
            DibBltRef(__d, __s);
        else
            DibBlt(__d, 0, 0, __s);
    }
    else if (w1 != w2 && h1 == h2)
    {
        typedef TM::CFastXScaleDib<D>       Scale;
        Scale ss(__s);
        ss.SetScaleWidth(w1);
        DibBlt(__d, 0, 0, ss);
    }
    else if (w1 == w2 && h1 != h2)
    {
        typedef TM::CFastYScaleDib<D>       Scale;
        Scale ss(__s);
        ss.SetScaleHeight(h1);
        DibBlt(__d, 0, 0, ss);
    }
    else
    {
        typedef TM::CFastXScaleDib<D>       ScaleX;
        typedef TM::CFastYScaleDib<ScaleX>  Scale;
        Scale ss(__s);
        ss.SetScaleWidth(w1);
        ss.SetScaleHeight(h1);
        DibBlt(__d, 0, 0, ss);
    }
}


template <class D, class S, class F>
void DibBltF(D& __d, int l, int t, const S& __s, const F& __f)
{
    DbgAssert(__d);

    int w = __s.GetWidth();
    int h = __s.GetHeight();
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            typename S::const_pointer p = __s.GetPixel(i, j);
            if (S::e_bRegular || p)
                __f(__d.GetPixel(l + i, t + j), *p);
        }
    }
}

template <class D, class S>
TMBool DibFind(const D& __d, const S& __s, TMPoint& pt)
{
    int wd = __d.GetWidth();
    int hd = __d.GetHeight();
    int ws = __s.GetWidth();
    int hs = __s.GetHeight();
    typedef typename D::value_type          value_type;
    value_type clr0 = *__s.GetPixel(0, 0);
    for (int y = 0; y < hd - hs; ++y)
    {
        for (int x = 0; x < wd - ws; ++x)
        {
            const value_type* pcd = __d.GetPixel(x, y);
            value_type cd = *pcd;//__d.GetPixel(x, y);
            if (cd == clr0)
            {
                for (int j = 0; j < hs; ++j)
                {
                    for (int i = 0; i < ws; ++i)
                    {
                        if (*__d.GetPixel(x + i, y + j) != *__s.GetPixel(i, j))
                            goto TM_DIB_FIND_NOT_SUB_DIB;
                    }
                }
                PointSetX(pt, x);
                PointSetY(pt, y);
                return true;
            }
TM_DIB_FIND_NOT_SUB_DIB:
            ;
        }
    }
    return false;
}

template <class D>
TMUInt32 DibHash(const D& __d)
{
    int w = __d.GetWidth();
    int h = __d.GetHeight();
    typedef typename D::value_type          value_type;
    TMUInt32 hash = 1315423911;
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            const value_type* pcd = __d.GetPixel(x, y);
            hash  ^=  ((hash << 5) + pcd->GetClr() + (hash >> 2)); ;
        }
    }
    return hash;
}

// dest = (s1 * alpha + s2 * (255 - alpha)) / 255;
// alpha: 0-255;
template <class D, class S1, class S2>
void DibMix(D& __d, const S1& __s1, const S2& __s2, TMUInt8 alpha)
{
    int w = __d.GetWidth();
    int h = __d.GetHeight();
    typedef typename D::reference          reference;
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            reference pd = __d.GetPixel(x,y);
            typename S1::const_reference s1(*__s1.GetPixel(x,y));
            typename S2::const_reference s2(*__s2.GetPixel(x,y));
            DbgAssertS1(sizeof(pd.r) == sizeof(TMUInt8));
            DbgAssertS2(sizeof(s1.r) == sizeof(TMUInt8));
            DbgAssertS3(sizeof(s2.r) == sizeof(TMUInt8));
            pd.SetR((TMUInt8)((s1.GetR()- s2.GetR()) * alpha / 255 + s2.GetR()));
            pd.SetG((TMUInt8)((s1.GetG()- s2.GetG()) * alpha / 255 + s2.GetG()));
            pd.SetB((TMUInt8)((s1.GetB()- s2.GetB()) * alpha / 255 + s2.GetB()));
            pd.SetA((TMUInt8)((s1.GetA()- s2.GetA()) * alpha / 255 + s2.GetA()));
        }
    }
}


template <class D, class S>
void DibMix(D& __d, const S& __s, TMUInt8 alpha)
{
    int w = __d.GetWidth();
    int h = __d.GetHeight();
    //    typedef typename D::reference          reference;
    //TM::DibRGBAI alpha64(alpha, alpha, alpha, alpha);
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            typename S::value_type s1(*__s.GetPixel(x,y));
            s1.PreMultiplied(alpha);
//            TM::DibRGBAI sd = (s1 * alpha) / 255;
            __d.GetPixel(x, y) = s1;
        }
    }
}

// dest format      DibData::bits   DibData::alpha,flag
//   32a            24              (alpha, eNone), (0, eKeepAlpha)
//   32a            0               (alpha, eNone)
//   YUV            YUV             (0, eNone), (alpha, eNone), (0, eKeepAlpha)
//   YUV            0               (0, eNone), (alpha, eNone)
//   other          Image format    must(0, eNone)
struct GXDibData
{
    enum EFlag
    {
        eNone      = 0x00,
        eBottomUp  = 0x01,      // is windows DIB.
        eKeepAlpha = 0x02,      // keep old alpha layer.
    };
    
    void*       bits;           // can be 0.
    int         wb;             // 0: use dest wb;
    union{
    TMUInt32    flag;           // mask of EFlag.
    bool bBottomUpDib;
    };
    TM::CDibHandleGray  alpha;
};
TMBool DibResetAsDib(TMDibBuff& dest, EGXFormat fmtDst, const GXDibData& dibData);


template <class D, class S>
TM_INLINE void DibReplaceAlpha(D& __dst, const S& __src)
{
    typedef typename D::value_type value_type;
    typedef typename S::value_type type_src;
    int w = __dst.GetWidth();
    int h = __dst.GetHeight();
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            value_type& d = __dst.GetPixel(x,y);
            const type_src*  s = __src.GetPixel(x,y);
            TMUInt8 alpha = s->GetA();
            d.PreMultiplied(alpha);
        }
    }
}

struct CPreBlend
{
    template <class T> void operator() (TM::DibPixelT<T>& dst) const
    {
        TMUInt8 a = dst.GetA();
        TMUInt8 r = dst.GetR();
        TMUInt8 g = dst.GetG();
        TMUInt8 b = dst.GetB();
        dst.SetR((TMUInt8)(r * a / 255));
        dst.SetG((TMUInt8)(g * a / 255));
        dst.SetB((TMUInt8)(b * a / 255));
    }
};

// make RGBA to pre_blend RGBA;
// win32 GDI use pre_blend RGBA;
// convert color RGBA(200,100,50,128)->RGBA(100,50,25,128);
TM_INLINE void DibPreBlend(TM::CDibHandle32& dib)
{
    DibFastEnumPixel(dib, CPreBlend());
}

#endif // end of MYBMP_H






