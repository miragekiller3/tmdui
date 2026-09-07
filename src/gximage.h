/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.4.18
 *
 *  Copyright (C) 2010 miragekiller
 */

#pragma once

#ifndef GXIMAGE_H
#define GXIMAGE_H

#include "tmlist.h"
#include "tmgdi.h"
#include "tmnative.h"
#include "tmdib.h"


struct IPicture;
typedef int IMGInt;


#define IMG_API TM_CAPI


IMG_API HIMAGE ImageCreateIndirect(int w, int h, EGXFormat format, RGBQUAD* pal, int nPalSize, const void* pData = 0);

IMG_API HIMAGE ImageCreateFromMem(const void* p, int nSize);

IMG_API HIMAGE ImageCreateFromFile(TMCStr pFile);

IMG_API HIMAGE ImageCreateFromStream(struct IStream* stream, TM::IImageSizeDecide* pDecide = 0, const TMColor* clrKey = 0, TM::IColorTransform* pct = 0, TMLParam context = 0);

IMG_API HIMAGE ImageCreateFromResource(TMHModule hInst, TMCStr pName, TMCStr pType);

IMG_API HIMAGE ImageCreateFromHBmp(TMHBmp hObj);

#ifdef TM_GTK
#   define ImageCreateFromHIcon(hIcon) ImageCreateFromHBmp(hIcon)
#else
IMG_API HIMAGE ImageCreateFromHIcon(TMHIcon hObj);
#endif

IMG_API HIMAGE ImageCreateThumbnail(HIMAGE hImg, int w, int h);

IMG_API TMBool ImageReCreate(HIMAGE hImg, int w, int h, int bpp, RGBQUAD* pal, int nPalSize, TMBool bCopy);

IMG_API TMBool ImageSaveFile(HIMAGE hImg, TMCStr filename, TMBool bHaveAlpha);

IMG_API void ImageFill(HIMAGE hImg, TMColor clr, const TMRect* rc = 0);

IMG_API void ImageSetColorKey(HIMAGE hImg, TMColor clr);

IMG_API void ImageConvertColor(HIMAGE hImg, TM::IColorTransform* pct, TMLParam context);

// 32a: img.rgb = clr.rgb; img.a = 255;
// 32x: change to 32a; img.rgba = clr.rgba * (img.rgb / 3);
IMG_API TMBool ImageConvertToText(HIMAGE hImg, TMColor clr);

IMG_API int ImageAddRef(HIMAGE hImg);

IMG_API int ImageRelease(HIMAGE hImg);

IMG_API size_t ImageGetWidth(HIMAGE hImg);

IMG_API size_t ImageGetHeight(HIMAGE hImg);

IMG_API EGXFormat ImageGetFormat(HIMAGE hImg);

IMG_API TMBool ImageConvertBitCount(HIMAGE hImg, int nBpp);

// return ms; error return 0;
IMG_API size_t ImageGetFrameCount(HIMAGE hImg);

IMG_API size_t ImageGetDelay(HIMAGE hImg);

// if nFrame == Current Frame or other failed return false;
IMG_API TMBool ImageSetCurrentFrame(HIMAGE hImg, TMUInt nFrame);

IMG_API TMUInt ImageGetCurrentFrame(HIMAGE hImg);

IMG_API TMHBmp ImageGetBitmap(HIMAGE hImg);

IMG_API void* ImageLockBits(HIMAGE hImg, TMBool bForce);

IMG_API void ImageLockRect(HIMAGE hImg, TMDibBuff& dib, const TMRect* rc = 0, EGXLockRect lock = GX_LOCK_RECT_READWRITE); // rc == 0: return start of memory

IMG_API TMHDC ImageCreateDC(HIMAGE hImg, TMHDC hDC = 0);

IMG_API void ImageDeleteDC(HIMAGE hImg, TMHDC hdc);

IMG_API void ImageResetAsDib(HIMAGE hImg, GXDibData& dibData);



// alpha:0-255
IMG_API void ImageBitBlt(
    TMHDC hdc, IMGInt xDst, IMGInt yDst,
    HIMAGE hImg, IMGInt xSrc = 0, IMGInt ySrc = 0, IMGInt wSrc = INT_MAX, IMGInt hSrc = INT_MAX,
    TMUInt32 uFlag = 0, TMUInt8 alpha = 0xff);

IMG_API void ImageStretchBlt(
    TMHDC hdc, const TMRect& rcDst, HIMAGE hImg, const TMRect& rcSrc,
    TMUInt32 uFlag = 0, TMUInt8 alpha = 0xff);

// IMG_API void ImageSharpBlt(
//     TMHDC hdc, const TMRect& rcDst, HIMAGE hImg, const TMRect& rcSrc,
//     const TM::CBltSharp& sharp, TMUInt32 uFlag, TMUInt8 alpha, int dpi);

// rgbaChannel: 0 - use dest color; 1 - use src color;
// TM_API void ImageChannelBlt(
//     TMHDC hdc, const TMRect& rcDst, HIMAGE hImg, const TMRect& rcSrc,
//     TMColor rgbaChannel, TMUInt32 uFlag = 0);

inline void ImageStretchBlt(
    TMHDC hdc, IMGInt xDst, IMGInt yDst, IMGInt wDst, IMGInt hDst,
    HIMAGE hImg, IMGInt xSrc, IMGInt ySrc, IMGInt wSrc, IMGInt hSrc,
    TMUInt32 uFlag = 0, TMUInt8 alpha = 0xff)
{
    ImageStretchBlt(hdc, TM::CRect(xDst, yDst, xDst + wDst, yDst + hDst),
        hImg, TM::CRect(xSrc, ySrc, xSrc + wSrc, ySrc + hSrc), uFlag, alpha);
}


GX_BGN_NAMESPACE


template <bool t_bManaged>
class CImageT
{
private:
    HIMAGE Init(HIMAGE hImage)
    {
        if (t_bManaged && hImage)
            ImageAddRef(hImage);
        return hImage;
    }

public:
    HIMAGE m_hImage;

    CImageT(HIMAGE hImage = 0)
        : m_hImage(Init(hImage))
    {
    }

    CImageT(const CImageT& img)
        : m_hImage(Init(img))
    {
    }

    ~CImageT()
    {
        if (t_bManaged && m_hImage != NULL)
            ImageRelease(m_hImage);
    }

    CImageT& operator = (HIMAGE hImage)
    {
        if (m_hImage != hImage)
        {
            if (t_bManaged && m_hImage != NULL)
                ImageRelease(m_hImage);
            m_hImage = Init(hImage);
        }
        return *this;
    }

    CImageT& operator = (const CImageT& img)
    {
        return operator = (img.m_hImage);
    }

    void Swap(CImageT& rhs)
    {
        tm_swap(m_hImage, rhs.m_hImage);
    }

    void Attach(HIMAGE hImage)
    {
        if (t_bManaged && m_hImage != NULL && m_hImage != hImage)
            ImageRelease(m_hImage);
        m_hImage = hImage;
    }

    HIMAGE Detach()
    {
        HIMAGE hImage = m_hImage;
        m_hImage = NULL;
        return hImage;
    }

    void Release()
    {
        ImageRelease(m_hImage);
        m_hImage = 0;
    }

    operator HIMAGE() const { return m_hImage; }

    bool IsNull() const { return (m_hImage == NULL); }

    HIMAGE CreateIndirect(int w, int h, EGXFormat format, RGBQUAD* pal = 0, int nPalSize = 0, const void* pData = 0)
    {
        DbgAssert(!*this);
        return m_hImage = ImageCreateIndirect(w, h, format, pal, nPalSize, pData);
    }

    HIMAGE CreateIndirect(int w, int h, int bpp)
    {
        DbgAssert(!*this);
        DbgAssert(bpp == 32);
        return m_hImage = ImageCreateIndirect(w, h, eGXFormat32A, 0, 0, 0);
    }

    HIMAGE CreateFromFile(TMCStr pFile)
    {
        DbgAssert(!*this);
        return m_hImage = ImageCreateFromFile(pFile);
    }

    HIMAGE CreateFromStream(IStream* stream, IImageSizeDecide* pDecide = 0, const TMColor* clrKey = 0, TM::IColorTransform* pct = 0, TMLParam context = 0)
    {
        DbgAssert(!*this);
        return m_hImage = ImageCreateFromStream(stream, pDecide, clrKey, pct, context);
    }

    HIMAGE CreateFromStream(IStream* stream, const TMSizeI* sz, TM::IColorTransform* pct = 0, TMLParam context = 0)
    {
        DbgAssert(!*this);
        CImagePSizeDecide psd(sz);
        return m_hImage = ImageCreateFromStream(stream, &psd, 0, pct, context);
    }

    HIMAGE CreateFromResource(TMHModule hInst, TMCStr pName, TMCStr pType)
    {
        DbgAssert(!*this);
        return m_hImage = ImageCreateFromResource(hInst, pName, pType);
    }

    HIMAGE CreateFromGDIObject(TMHBmp hObj)
    {
        DbgAssert(!*this);
        return m_hImage = ImageCreateFromHBmp(hObj);
    }

    #if !defined(TM_GTK)
    HIMAGE CreateFromGDIObject(TMHIcon hObj)
    {
        DbgAssert(!*this);
        return m_hImage = ImageCreateFromHIcon(hObj);
    }
    #endif

//     HIMAGE CreateFromIPicture(IPicture* picture)
//     {
//         DbgAssert(!*this);
//         return m_hImage = ImageCreateFromIPicture(picture);
//     }

    HIMAGE CreateThumbnail(HIMAGE hImg, int w, int h)
    {
        DbgAssert(!*this);
        return m_hImage = ImageCreateThumbnail(hImg, w, h);
    }

    TMBool ReCreate(int w, int h, int bpp, RGBQUAD* pal, int nPalSize, TMBool bCopy)
    {
        DbgAssert(*this);
        return ImageReCreate(m_hImage, w, h, bpp, pal, nPalSize, bCopy);
    }

    TMBool ReCreate(int w, int h, EGXFormat format)
    {
        if (m_hImage)
        {
             if (w == ImageGetWidth(m_hImage) && h == ImageGetHeight(m_hImage) && format == ImageGetFormat(m_hImage))
             {
                 ImageFill(m_hImage, 0);
                 return true;
             }
             ImageRelease(m_hImage);
        }
        m_hImage = ImageCreateIndirect(w, h, format, 0, 0);
        return 0 != m_hImage;
    }

    void SetColorKey(TMColor clr)
    {
        DbgAssert(*this);
        ImageSetColorKey(m_hImage, clr);
    }

//     TMColor ImageGetColorKey()
//     {
//         DbgAssert(*this);
//         return ImageGetColorKey(m_hImage);
//     }
// 
//     TMUInt8 SetAlpha(TMUInt8 alpha)
//     {
//         DbgAssert(*this);
//         return ImageSetAlpha(m_hImage, alpha);
//     }
// 
//     TMUInt8 ImageGetAlpha()
//     {
//         DbgAssert(*this);
//         return ImageGetAlpha(m_hImage);
//     }

    size_t GetWidth() const
    {
        return ImageGetWidth(m_hImage);
    }

    size_t GetHeight() const
    {
        return ImageGetHeight(m_hImage);
    }

    EGXFormat GetFormat() const
    {
        return ImageGetFormat(m_hImage);
    }

    TMBool ConvertBitCount(int nBpp)
    {
        return ImageConvertBitCount(m_hImage, nBpp);
    }

    size_t GetFrameCount()
    {
        return ImageGetFrameCount(m_hImage);
    }

    TMBool SetCurrentFrame(TMUInt nFrame)
    {
        DbgAssert(*this);
        return ImageSetCurrentFrame(m_hImage, nFrame);
    }

    TMUInt GetCurrentFrame()
    {
        return ImageGetCurrentFrame(m_hImage);
    }

    size_t GetDelay()
    {
        return ImageGetDelay(m_hImage);
    }

//     bool Ticket(size_t milliseconds, TMBool bChangeCurFrame)
//     {
//         return ImageTicket(m_hImage, milliseconds, bChangeCurFrame);
//     }

    TMHBmp GetBitmap()
    {
        return ImageGetBitmap(m_hImage);
    }

    void* LockBits(TMBool bForce)
    {
        DbgAssert(*this);
        return ImageLockBits(m_hImage, bForce);
    }

    TMHDC CreateDC(TMHDC hDC = 0)
    {
        DbgAssert(*this);
        return ImageCreateDC(m_hImage, hDC);
    }

    void DeleteDC(TMHDC hdc)
    {
        DbgAssert(*this);
        ImageDeleteDC(m_hImage, hdc);
    }

    void BitBlt(TMHDC hdc,
        IMGInt xDst = 0, IMGInt yDst = 0,
        IMGInt xSrc = 0, IMGInt ySrc = 0, IMGInt wSrc = INT_MAX, IMGInt hSrc = INT_MAX,
        TMUInt32 uFlag = 0, TMUInt8 alpha = 255)
    {
        DbgAssert(*this);
        ImageBitBlt(hdc, xDst, yDst, m_hImage, xSrc, ySrc, wSrc, hSrc, uFlag, alpha);
    }

    void StretchBlt(TMHDC hdc,
        IMGInt xDst, IMGInt yDst, IMGInt wDst, IMGInt hDst,
        IMGInt xSrc, IMGInt ySrc, IMGInt wSrc, IMGInt hSrc,
        TMUInt32 uFlag = 0, TMUInt8 alpha = 255)
    {
        DbgAssert(*this);
        ImageStretchBlt(hdc, xDst, yDst, wDst, hDst, m_hImage, xSrc, ySrc, wSrc, hSrc, uFlag, alpha);
    }
};


typedef CImageT<true>   CImage;
typedef CImageT<false>  CImageHandle;


#ifdef TMATL_H
#   define _cdc_base    CDCHandleAT
#elif defined(__ATLGDI_H__)
#   define _cdc_base    CDCHandle
#elif defined(TM_MAC)
#   define _cdc_base    GContext 
#else

struct _cdc_base
{
public:
    TMHDC     m_hDC;
    _cdc_base (TMHDC hdc = 0) : m_hDC(hdc) {}
    operator TMHDC() const { return m_hDC; }
};

#endif // __ATLGDI_H__


class CImageDC : public _cdc_base
{
public:
    TM_NO_COPYABLE(CImageDC);
    
public:
    HIMAGE m_hImg;

    CImageDC(HIMAGE hImg)
        : _cdc_base(ImageCreateDC(hImg))
        , m_hImg(hImg)
    {
    }

    ~CImageDC()
    {
        ImageDeleteDC(m_hImg, *this);
    }
};


// T: CDibHandle32, CDibHandle24;
template <class T>
struct CAutoImageLockBitsT : public T
{
public:
    TM_NO_COPYABLE(CAutoImageLockBitsT);
    
public:
    CAutoImageLockBitsT(HIMAGE img, const TMRect* rc = 0, EGXLockRect flag = GX_LOCK_RECT_READWRITE)
    {
        DbgAssert(gxFormat2BPP(ImageGetFormat(img)) == T::eBitCount);
//        int strike;
//        m_bits = 
        ImageLockRect(img, *this, rc, flag);
        this->m_width = ImageGetWidth(img);
//        m_height = ImageGetHeight(t);
    }
    
    ~CAutoImageLockBitsT()
    {
    }
};
typedef CAutoImageLockBitsT<CDibHandle32> CAutoImageLockBits32;
typedef CAutoImageLockBitsT<CDibHandle24> CAutoImageLockBits24;


GX_END_NAMESPACE

#endif // GXIMAGE_H




