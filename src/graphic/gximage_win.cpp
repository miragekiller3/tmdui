/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.4.18
 *
 *  Copyright (C) 2010 miragekiller
 */

#include "../gximage.h"
#include "../tmdib.h"
#include "../tmgdi.h"
#include "../tmfile.h"
#include "../tmmemfile.h"
#include "../tmplatform_win.h"
#include "gxgif.h"
#include "gxtexture_blt.h"
#include "gximage_win_wic.h"


#pragma comment(lib, "Msimg32.lib")

inline bool _gi_isBitmap(HBITMAP hBmp)
{
    DWORD x = GetObjectType(hBmp);
    return x == OBJ_BITMAP;
}

inline bool _gi_isMemDC(HDC hDC)
{
    DWORD x = GetObjectType(hDC);
    return x == OBJ_MEMDC;
}

bool DIB::Create(int w, int h, EGXFormat format, RGBQUAD* ppal, int npal)
{
    DbgAssert(format != eGXFormatA8B8G8R8);
    DbgAssert(format != eGXFormatX8B8G8R8);

    int bpp = gxFormat2BPP(format);
    if (!bpp)
        return false;
    struct
    {
        BITMAPINFO bi;
        RGBQUAD rgb[256];
    } head = { sizeof(BITMAPINFOHEADER), w, h, 1, (WORD)bpp };
    head.bi.bmiHeader.biClrUsed = npal;
    if (ppal && bpp <= 8)
        MemCpy (head.bi.bmiColors, ppal, 256 * sizeof(RGBQUAD));
    void* pData = 0;
    HBITMAP hBmp;
    DbgVerify(hBmp = ::CreateDIBSection(0, &head.bi, DIB_RGB_COLORS, &pData, 0, 0))("width:%d, height:%d, bpp:%d", w, h, bpp);
    if (!hBmp)
        return false;

    this->Set(pData, w, h, bpp);
    this->m_bmp = hBmp;
    this->m_format = format;
    return true;
}


TMUInt8* DIB::PixelPos(int x, int y)
{
    DbgAssert(x < m_width && y < m_height);
    return (TMUInt8*)m_bits + (m_height - y - 1) * m_stride + x * GetBpp() / 8;
}


////////////////////////////////////////////////////////////////////////////////
// HIMAGE__
struct HIMAGE__ : public TM::CBaseRefT<HIMAGE__>
{
    DIB dib;
    int frames;
    int cur;
    class IAniSource* pAni;
    
#ifdef TM_DBG
    typedef TM::CBaseRefT<HIMAGE__> base_ref;
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        HIMAGE__* p = (HIMAGE__*)cls->m_pClass;
        if (pbuf)
        {
            TM::CStringA str;
            str.Format("%d * %d * %d", p->dib.m_width, p->dib.GetHeight(), gxFormat2BPP(p->dib.m_format));
            str.CopyTo(pbuf);
        }
        return p->dib.GetHeight() * p->dib.GetWidthByte();
    }
#endif
    
    HIMAGE__()
        : frames(1)
        , cur(0)
        , pAni(0)
    {
    }
    
    ~HIMAGE__()
    {
        Clean();
    }

    void Clean()
    {
        if (pAni)
        {
            pAni->Release();
            pAni = 0;
        }
        DbgVerify(::DeleteObject(dib.m_bmp));
        dib.m_bmp = 0;
    }
};


////////////////////////////////////////////////////////////////////////////////
// ImageXXX
HIMAGE ImageCreateIndirect(int w, int h, EGXFormat format, RGBQUAD* pal, int nPalSize, const void* pData)
{
    DIB dib;
    if (!dib.Create(w, h, format, pal, nPalSize))
        return 0;

    if (pData)
        MemCpy(dib.m_bits, pData, dib.m_stride * dib.m_height);

    HIMAGE__* p = new HIMAGE__();
    p->dib = dib;
    return p;
}

TMBool ImageReCreate(HIMAGE hImg, int w, int h, int bpp, RGBQUAD* pal, int nPalSize, TMBool bCopy)
{
    if (!hImg)
        return false;

    DIB dib;
    if (!dib.Create(w, h, (EGXFormat)bpp, pal, nPalSize))
        return false;

    if (bCopy)
    {
        TM::CCompatibleDC dc1(hImg->dib.m_bmp, 0);
        TM::CCompatibleDC dc2(dib.m_bmp);
        DbgVerify(::BitBlt(dc2.m_hDC, 0, 0, w, h, dc1.m_hDC, 0, 0, SRCCOPY));
    }

    hImg->Clean();
    hImg->dib = dib;
    hImg->frames = 1;
    hImg->cur = 0;

    return true;
}

HIMAGE ImageCreateFromMem(const void* p, int nSize)
{
    TM::CStreamT<TM::CMemBuf> mb;
    mb.Assign((const TMUInt8*)p, nSize);
    return ImageCreateFromStream(&mb);
}


HIMAGE ImageCreateFromFile(TMCStr pFile)
{
    TM::CStreamT<TM::CFile> f;
    f.OpenExisting (pFile, TM::CFile::e_faRead);
    if (f)
        return ImageCreateFromStream(&f);
    return 0;
}

struct CImgCreateHelper
    : public TM::CTextureCreater
{
    HIMAGE m_img;

    CImgCreateHelper() : m_img(0) {}

    static void _SetCurrentFrame(HIMAGE hImg, int nFrame, IAniSource* pAni)
    {
        TM::CDibHandle32 d32;
        ImageLockRect(hImg, d32, 0, GX_LOCK_RECT_WRITE);
        TM_TRY_ALL(pAni->GetFrame(nFrame, d32));
        hImg->cur = nFrame;
    }

    virtual TMBool CreateIndirect(int w, int h, EGXFormat format)
    {
        if (!m_img)
            m_img = ImageCreateIndirect(w, h, format, 0, 0, 0);
        return m_img != 0;
    }

    virtual void LockRect(TMDibBuff& dib, const TMRect* rc)
    {
        ImageLockRect(m_img, dib, rc);
    }

    virtual void Unlock() {}

    virtual void AddAni(IAniSource* source)
    {
        int w, h, nFrams;
        int bpp;
        source->GetInfo(w, h, bpp, nFrams);
        HIMAGE__* p = ImageCreateIndirect(w, h, (EGXFormat)bpp, 0, 0, 0); // new HIMAGE__();
        if (p)
        {
            p->frames = nFrams;//m_pAni->m_aFrame.size();
            _SetCurrentFrame(p, 0, source);
            m_img = p;
            if (nFrams > 1)
                p->pAni = source;
            else
                source->Release();
        }
        else
            source->Release();
    }

    virtual EImageFormat CreateTextureFromStream(IStream* stream, TM::IImageSizeDecide* pDecide, TM::IColorTransform* pct, TMLParam context)
    {
        EImageFormat b = CTextureCreater::CreateTextureFromStream(stream, pDecide, pct, context);
        if (!b)
            b = _ImageCreatePngFromStream(this, stream);
        return b;
    }

};

static void _ImageCreateFromHICON(TM::CTextureCreater* creater, CDecoder& decoder, HICON hIcon)
{
    TM_TRY_ALL(decoder.CreateFromIcon(creater, hIcon));
}

void TM::CTextureCreater::FromHICON(HICON hIcon)
{
    CDecoder decoder;
    _ImageCreateFromHICON(this, decoder, hIcon);
}


HIMAGE ImageCreateFromStream(IStream* stream, TM::IImageSizeDecide* pDecide, const TMColor* clrKey, TM::IColorTransform* pct, TMLParam context)
{
    CImgCreateHelper helper;
    EImageFormat b;
    tm_try
    {
        b = helper.CreateTextureFromStream(stream, pDecide, pct, context);
    }
    tm_catch_all()
    {
        return 0;
    }
    if (!helper.m_img)
        return 0;
    if (pct && b != eImageFormatSVG && b != eImageFormatGif87 && b != eImageFormatGif89)
        ImageConvertColor(helper.m_img, pct, context);
    if (clrKey)
    {
        if (pct)
            ImageSetColorKey(helper.m_img, pct->ConvertColor(context, *clrKey));
        else
            ImageSetColorKey(helper.m_img, *clrKey);
    }
    return helper.m_img;

//     // not create from Mime, create from IPicture
//     CComPtr<IPicture> pict;
//     HRESULT hr = OleLoadPicture (stream, 0, FALSE, IID_IPicture, (void**)&pict);
//     return ImageCreateFromIPicture(pict);
}

HIMAGE ImageCreateFromResource(HINSTANCE hInst, TMCStr pName, TMCStr pType)
{
    CComPtr<IStream> stream;
    StreamCreateFromResource(hInst, pName, pType, &stream);
    return ImageCreateFromStream(stream);
}

TMBool ImageSaveFile(HIMAGE hImg, TMCStr filename, TMBool bHaveAlpha)
{
    DbgAssert(hImg);
    return _ImageSavePngFromHBITMAP(hImg->dib.m_bmp, filename, bHaveAlpha);
}


void ImageFill(HIMAGE hImg, TMColor clr, const TMRect* rc)
{
    switch (hImg->dib.m_format)
    {
    case eGXFormat32A:
    case eGXFormat32X:
        {
            TM::CDibHandle32 d32(hImg->dib.m_width, -hImg->dib.m_height, hImg->dib.m_bits, hImg->dib.m_stride);
            TM::_ColorFill(d32, clr, rc);
        }
        break;

    default:
        {
            TM::CImageDC dc(hImg);
            TM::CRect rcx;
            if (!rc)
            {
                rcx.SetRect(0, 0, hImg->dib.m_width, hImg->dib.GetHeight());
                rc = &rcx;
            }
            dc.FillSolidRect(rc, ColorToCOLORREF(clr));
        }
    }
}

TM_STATIC_STRUCT _ConvertIcon
{
    void operator () (TM::CDibHandle32::value_type& __d, const TM::CDibHandle32::value_type& __s) const
    {
        if (__s.GetR())
        {
            DbgAssert(__s.GetR() == __s.GetG());
            DbgAssert(__s.GetG() == __s.GetB());
            DbgAssert(__s.GetR() == 0xff);
             __d.SetRGBA((TMUInt8)0, (TMUInt8)0, (TMUInt8)0, (TMUInt8)0);
        }
        else
            __d.SetA(0xff);
    }
};

TM_STATIC_STRUCT _SEARCH
{
    TMBool operator () (TM::CDibHandle32::const_reference pixel) const
    {
        return pixel.GetA();
    }
};

static void _ImageCreateFromHBmp(CImgCreateHelper& creater, CDecoder& decoder, TMHBmp hObj)
{
    TM_TRY_ALL(decoder.CreateFromHBMP(&creater, hObj));
}

HIMAGE ImageCreateFromHBmp(TMHBmp hObj)
{
    CImgCreateHelper creater;
    CDecoder decoder;
    _ImageCreateFromHBmp(creater, decoder, hObj);
    return creater.m_img;
}

HIMAGE ImageCreateFromHIcon(TMHIcon hObj)
{
    CImgCreateHelper creater;
    creater.FromHICON(hObj);
    return creater.m_img;
}

HIMAGE ImageCreateThumbnail(HIMAGE hImg, int w, int h)
{
    if (!hImg || w <= 0 || h <= 0)
        return 0;

    if (!gxFormatIsContainAlpha(hImg->dib.m_format))
    {
        // StretchBlt will lose alpha.
        HIMAGE img = ImageCreateIndirect(w, h, eGXFormat24, 0, 0);
        TM::CImageDC dcd(img);
        TM::CImageDC dcs(hImg);
        DbgAssert(dcd);
        DbgAssert(dcs);
        DbgVerify(::SetStretchBltMode(dcd, HALFTONE));
        DbgVerify(::StretchBlt(dcd, 0, 0, w, h, dcs, 0, 0, hImg->dib.m_width, hImg->dib.m_height, SRCCOPY));
        return img;
    }
    else
    {
        CImgCreateHelper helper;
        try
        {
            CDecoder().ThumbnailBitmap(&helper, hImg->dib.m_bmp, w, h);
        }
        catch(...)
        {
        }
        return helper.m_img;
    }
}

int ImageAddRef(HIMAGE hImg)
{
    if (!hImg)
        return 0;
    return hImg->AddRef();
}

int ImageRelease(HIMAGE hImg)
{
    if (!hImg)
        return 0;
    return hImg->Release();
}

size_t ImageGetWidth(HIMAGE hImg)
{
    return hImg ? hImg->dib.m_width : 0;
}

size_t ImageGetHeight(HIMAGE hImg)
{
    return hImg ? hImg->dib.m_height : 0;
}

EGXFormat ImageGetFormat(HIMAGE hImg)
{
    return hImg ? hImg->dib.m_format : eGXFormatUnknow;
}

size_t ImageGetFrameCount(HIMAGE hImg)
{
    return hImg ? hImg->frames : 1;
}

size_t ImageGetDelay(HIMAGE hImg)
{
    if (!hImg || !hImg->pAni)
        return 0;
    return hImg->pAni->GetDelay();
}


TMBool ImageSetCurrentFrame(HIMAGE hImg, TMUInt nFrame)
{
    if (!hImg)
        return false;
    nFrame %= hImg->frames;
    if (nFrame != hImg->cur)
    {
        CImgCreateHelper::_SetCurrentFrame(hImg, nFrame, hImg->pAni);
        return true;
    }
    return false;
}

TMUInt ImageGetCurrentFrame(HIMAGE hImg)
{
    if (!hImg)
        return 0;
    return hImg->cur;
}


TM_STATIC_STRUCT _FillAlpha
{
    void operator () (TM::CDibHandle32::value_type& clr)
    {
        clr.SetA(0xff);
    }
};

TM_STATIC_STRUCT _FillAlpha2
{
    TMUInt8 r;
    TMUInt8 g;
    TMUInt8 b;

    _FillAlpha2(TMColor clr)
        : r (ColorR(clr))
        , g (ColorG(clr))
        , b (ColorB(clr))
    {
    }

    void operator () (TM::CDibHandle32::value_type& clr)
    {
        if (clr.GetR() == r && clr.GetG() == g && clr.GetB() == b)
            clr.SetRGBA((TMUInt8)0, (TMUInt8)0, (TMUInt8)0, (TMUInt8)0);
        else
            clr.SetA(0xff);
    }
};

void _ToDib(HIMAGE hImg, EGXFormat nBpp, TMColor clrKey = cColorInvalid)
{
    if (nBpp == hImg->dib.m_format && (clrKey == cColorInvalid))
        return;

    // create new dib
    DIB& dibs = hImg->dib;
    DIB dib;
    int w = dibs.m_width;
    int h = dibs.m_height;
    dib.Create(w, h, nBpp, 0, 0);
    DbgAssert(dib.m_bmp);

    // copy src dib to dest dib
    {
        TM::CCompatibleDC dc(dib.m_bmp);
        TM::CCompatibleDC dcs(dibs.m_bmp);
        DbgVerify(::BitBlt(dc, 0, 0, w, h, dcs, 0, 0, SRCCOPY));
    }

    // bitblt will set alpha channel to 0, so fix it
    if (nBpp == eGXFormat32A)
    {
        TM::CDibHandle32 d32(dib.m_width, dib.m_height, dib.m_bits, dib.m_stride);
        if (clrKey == cColorInvalid)
            DibFastEnumPixel(d32, _FillAlpha());
        else
            DibFastEnumPixel(d32, _FillAlpha2(clrKey));
    }

    tm_swap(dib, dibs);
    DbgVerify(::DeleteObject(dib.m_bmp));

    // write back to animate buffer
//     if (hImg->pAni)
//         hImg->pAni->m_aFrame[hImg->cur].dib = dibs;
}

void ImageSetColorKey(HIMAGE hImg, TMColor clr)
{
    if (hImg && (clr != cColorInvalid))
        _ToDib(hImg, eGXFormat32A, clr);
}

TM_STATIC_STRUCT _FillCT
{
    TM::IColorTransform* ct;
    TMLParam oid;
    _FillCT(TM::IColorTransform* _ct, TMLParam _oid) : ct(_ct), oid(_oid) {}
    void operator () (TM::CDibHandle32::value_type& clr)
    {
        clr.SetColor(ct->ConvertColor(oid, clr.ToColor()));
    }
};

void ImageConvertColor(HIMAGE hImg, TM::IColorTransform* pct, TMLParam context)
{
    if (!pct || !hImg)
        return;
    ImageConvertBitCount(hImg, 32);
    TM::CDibHandle32 d32(hImg->dib);
    DibFastEnumPixel(d32, _FillCT(pct, context));
}

TMBool ImageConvertBitCount(HIMAGE hImg, int nBpp)
{
    if (!hImg)
        return false;
    if (nBpp != hImg->dib.m_format)
        _ToDib(hImg, (EGXFormat)nBpp);
    return true;
}

struct _Image_TextDrawMakeRGB
{
    TM::DibRGB rgb;

    _Image_TextDrawMakeRGB(TMColor clr) { rgb.SetColorAsRGB_A(clr); }

    void operator() (TM::DibARGB& pixel) const
    {
        TMUInt8 a = pixel.a;
        pixel.g = rgb.g * a / 255;
        pixel.b = rgb.b * a / 255;
        pixel.r = rgb.r * a / 255;
    }
};

struct _Image_TextDrawMakeRGBA
{
    TM::DibRGB rgb;

    _Image_TextDrawMakeRGBA(TMColor clr) { rgb.SetColor(clr); }

    void operator() (TM::DibARGB& pixel) const
    {
        TMUInt8 a = (pixel.r + pixel.g + pixel.b) / 3;
        pixel.a = a;
        pixel.g = rgb.g * a / 255;
        pixel.b = rgb.b * a / 255;
        pixel.r = rgb.r * a / 255;
    }
};

TMBool ImageConvertToText(HIMAGE hImg, TMColor clr)
{
    if (!hImg)
        return false;
    switch (hImg->dib.m_format)
    {
    case eGXFormat32A:
        {
            TM::CAutoImageLockBits32 dib(hImg);
            DibFastEnumPixel(dib, _Image_TextDrawMakeRGB(clr));
        }
        return true;

    case eGXFormat32X:
        {
            hImg->dib.m_format = eGXFormat32A;
            TM::CAutoImageLockBits32 dib(hImg);
            DibFastEnumPixel(dib, _Image_TextDrawMakeRGBA(clr));
        }
        return true;

    default:
        return false;
    }
}

HBITMAP ImageGetBitmap(HIMAGE hImg)
{
    return hImg ? hImg->dib.m_bmp : 0;
}

void* ImageLockBits(HIMAGE hImg, TMBool bForce)
{
    if (!hImg)
        return 0;
    if (bForce && !hImg->dib.m_bits)
        _ToDib(hImg, hImg->dib.m_format);
    return hImg->dib.m_bits;
}

void ImageLockRect(HIMAGE hImg, TMDibBuff& dib, const TMRect* rc, EGXLockRect lock)
{
    if (!hImg)
        return;
    if (!rc)
    {
        dib = hImg->dib;
        dib.m_height = -dib.m_height;
    }
    else
    {
        dib.m_bits = hImg->dib.PixelPos(rc->left, rc->top);
        dib.m_stride = -hImg->dib.m_stride;
        dib.m_height = rc ? rc->bottom - rc->top : hImg->dib.m_height;
    }
}

HDC ImageCreateDC(HIMAGE hImg, HDC hDC)
{
    if (!hImg)
        return 0;
    HDC hdc = CreateCompatibleDC(hDC);
    DbgAssert(hdc);
    DbgAssert(hImg->dib.m_bmp);
    hImg->dib.m_bmp = (HBITMAP)SelectObject(hdc, hImg->dib.m_bmp);
    DbgAssert(hImg->dib.m_bmp);
    DbgAssert(_gi_isMemDC(hdc));
    return hdc;
}

void ImageDeleteDC(HIMAGE hImg, HDC hdc)
{
    if (!hImg)
        return;
    DbgAssert(_gi_isBitmap(hImg->dib.m_bmp));
    DbgAssert(_gi_isMemDC(hdc));
    HBITMAP hBmp = (HBITMAP)SelectObject(hdc, hImg->dib.m_bmp);
    DbgAssert(_gi_isBitmap(hBmp));
    hImg->dib.m_bmp = hBmp;
    DeleteDC(hdc);
}

void ImageResetAsDib(HIMAGE hImg, GXDibData& dibData)
{
    DbgAssert(hImg);
    DbgAssert(_gi_isBitmap(hImg->dib.m_bmp));
    DibResetAsDib(hImg->dib, hImg->dib.m_format, dibData);
}

void ImageBitBlt(HDC hdc, IMGInt xDst, IMGInt yDst,
                 HIMAGE hImg, IMGInt xSrc, IMGInt ySrc, IMGInt wSrc, IMGInt hSrc,
                 TMUInt32 uFlag, TMUInt8 alpha)
{
    if (!hdc || !hImg)
        return;
    IMGInt w = hImg->dib.m_width;
    IMGInt h = hImg->dib.m_height;
    if (wSrc > w)
        wSrc = w;
    if (hSrc > h)
        hSrc = h;
    ImageStretchBlt(hdc, xDst, yDst, wSrc, hSrc, hImg, xSrc, ySrc, wSrc, hSrc, uFlag, alpha);
}

struct CBltSelectImg : public TM::CBltSelect
{
    TM::CCompatibleDC dcSrc;

    CBltSelectImg(HDC hdc, const TMRect& rcDst, HIMAGE hImg, const TMRect& rcSrc, TMUInt32 uFlag, TMUInt8 _alpha)
        : TM::CBltSelect(hdc, rcDst, rcSrc, uFlag, _alpha, hImg->dib.m_width, hImg->dib.m_height, hImg->dib.m_format)
    {
        dcSrc.SelectIn(hImg->dib.m_bmp);
    }
};

void ImageStretchBlt(TMHDC hdc, const TMRect& rcDst, HIMAGE hImg, const TMRect& rcSrc,
                     TMUInt32 uFlag, TMUInt8 alpha)
{
    if (!hdc || !hImg || alpha == 0)
        return;

    CBltSelectImg bs(hdc, rcDst, hImg, rcSrc, uFlag, alpha);
    bs.Blt(bs.dcSrc);
}

