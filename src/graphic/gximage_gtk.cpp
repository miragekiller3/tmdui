/*
 *  @file
 *  @brief image support for GTK
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.4.18
 *
 *  Copyright (C) 2010 miragekiller
 */

#include "../gximage.h"
//#include "../tmfile.h"
//#include "../tmmemfile.h"

#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <cairo.h>
#include "gxtexture_blt.h"


////////////////////////////////////////////////////////////////////////////////
// HIMAGE__
struct HIMAGE__ : public TM::CBaseRefT<HIMAGE__>
{
    GdkPixbuf* m_pb;
    GdkPixbufAnimation* m_anim;
    int m_frames;
    int m_cur;
    int m_delay;

private:
    HIMAGE__(){}

    int _calc_frame_count()
    {
        if (!m_anim)
            return m_frames = 1;
        DbgAssert(!gdk_pixbuf_animation_is_static_image(m_anim));
        
        GTimeVal tv = {0, 0};
        GdkPixbufAnimationIter* iter = gdk_pixbuf_animation_get_iter(m_anim, &tv);
        if (!iter)
            return m_frames = 1;
        
        m_delay = gdk_pixbuf_animation_iter_get_delay_time(iter);
        if (m_delay <= 0)
            m_delay = 1000 / 60;

        GdkPixbuf* first = gdk_pixbuf_animation_iter_get_pixbuf(iter);
        size_t count = 1;
        for (int i = 0; i < 10000; ++i)
        {
            int delay = gdk_pixbuf_animation_iter_get_delay_time(iter);
            if (delay <= 0)
                break;
            g_time_val_add(&tv, (glong)(delay * 1000));
            gdk_pixbuf_animation_iter_advance(iter, &tv);
            GdkPixbuf* cur = gdk_pixbuf_animation_iter_get_pixbuf(iter);
            if (cur == first)
                break;
            ++count;
        }
        g_object_unref(iter);
        return m_frames = count;
    }

public:
    ~HIMAGE__()
    {
        Clean();
    }

    void Clean()
    {
        if (m_pb)
        {
            g_object_unref(m_pb);
            m_pb = 0;
        }
        if (m_anim)
        {
            g_object_unref(m_anim);
            m_anim = 0;
        }
    }

    void SetAndRef(GdkPixbuf* pb)
    {
        if (m_pb)
            g_object_unref(m_pb);
        m_pb = pb;
        g_object_ref(pb);
    }

    static HIMAGE _create(GdkPixbuf* pb, GdkPixbufAnimation* anim)
    {
        if (!pb && !anim)
            return 0;

        if (anim && gdk_pixbuf_animation_is_static_image(anim))
        {
            g_object_unref(anim);
            anim = 0;
        }
        
        HIMAGE__* img   = new HIMAGE__();
        img->m_pb       = pb;
        img->m_anim     = anim;
        img->m_cur      = 0;
        img->_calc_frame_count();
        return img;
    }
};

static inline GdkPixbuf* _get_pixbuf(HIMAGE hImg)
{
    return hImg ? hImg->m_pb : 0;
}

static inline GdkPixbufAnimation* _get_animation(HIMAGE hImg)
{
    return hImg ? hImg->m_anim : 0;
}




// static inline GdkPixbuf* _get_current_pixbuf(HIMAGE hImg)
// {
//     return _get_pixbuf(hImg);
// }

static inline int _clampi(int v, int lo, int hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

static inline const char* _save_type_from_filename(const char* filename)
{
    if (!filename)
        return "png";

    const char* dot = strrchr(filename, '.');
    if (!dot)
        return "png";

    ++dot;
    if (!dot[0])
        return "png";

    if (!strcasecmp(dot, "png")) return "png";
    if (!strcasecmp(dot, "jpg") || !strcasecmp(dot, "jpeg")) return "jpeg";
    if (!strcasecmp(dot, "bmp")) return "bmp";
    if (!strcasecmp(dot, "gif")) return "gif";
    if (!strcasecmp(dot, "ico")) return "ico";
    if (!strcasecmp(dot, "svg")) return "svg";
    return "png";
}

static void _copy_dib_like_to_pixbuf(GdkPixbuf* pb, const void* pData, int srcStride, EGXFormat fmt)
{
    if (!pb || !pData)
        return;

    const int w = gdk_pixbuf_get_width(pb);
    const int h = gdk_pixbuf_get_height(pb);
    const int dstStride = gdk_pixbuf_get_rowstride(pb);
    const int dstChannels = gdk_pixbuf_get_n_channels(pb);
    guchar* dst = gdk_pixbuf_get_pixels(pb);

    if (fmt == eGXFormat24)
    {
        for (int y = 0; y < h; ++y)
        {
            const unsigned char* srcRow = (const unsigned char*)pData + (h - 1 - y) * srcStride;
            guchar* dstRow = dst + y * dstStride;
            for (int x = 0; x < w; ++x)
            {
                const unsigned char* s = srcRow + x * 3;
                guchar* d = dstRow + x * dstChannels;
                d[0] = s[2];
                d[1] = s[1];
                d[2] = s[0];
                if (dstChannels == 4)
                    d[3] = 0xFF;
            }
        }
    }
    else
    {
        for (int y = 0; y < h; ++y)
        {
            const unsigned char* srcRow = (const unsigned char*)pData + (h - 1 - y) * srcStride;
            guchar* dstRow = dst + y * dstStride;
            for (int x = 0; x < w; ++x)
            {
                const unsigned char* s = srcRow + x * 4;
                guchar* d = dstRow + x * dstChannels;
                d[0] = s[2];
                d[1] = s[1];
                d[2] = s[0];
                if (dstChannels == 4)
                    d[3] = s[3];
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// ImageXXX
HIMAGE ImageCreateIndirect(int w, int h, EGXFormat format, RGBQUAD* pal, int nPalSize, const void* pData)
{
    tm_unused(pal);
    tm_unused(nPalSize);

    if (w <= 0 || h <= 0)
        return 0;

    const int bpp = gxFormat2BPP(format);
    const gboolean hasAlpha = (bpp == 32) ? true : false;
    if (!(bpp == 24 || bpp == 32))
        return 0;

    GdkPixbuf* pb = gdk_pixbuf_new(GDK_COLORSPACE_RGB, hasAlpha, 8, w, h);
    if (!pb)
        return 0;

    if (pData)
    {
        const int srcStride = DWORD_WBYTES(w * bpp);
        _copy_dib_like_to_pixbuf(pb, pData, srcStride, format);
    }
    else
    {
        gdk_pixbuf_fill(pb, 0x00000000);
    }

    return HIMAGE__::_create(pb, 0);
}

TMBool ImageReCreate(HIMAGE hImg, int w, int h, int bpp, RGBQUAD* pal, int nPalSize, TMBool bCopy)
{
    tm_unused(pal);
    tm_unused(nPalSize);

    GdkPixbuf* pb = _get_pixbuf(hImg);
    if (!pb)
        return false;

    const int ow = gdk_pixbuf_get_width(pb);
    const int oh = gdk_pixbuf_get_height(pb);
    if (ow != w || oh != h)
        return false;

    const int channels = gdk_pixbuf_get_n_channels(pb);
    if (!((bpp == 24 && channels == 3) || (bpp == 32 && channels == 4)))
        return false;

    if (!bCopy)
        gdk_pixbuf_fill(pb, 0x00000000);

    return true;
}

HIMAGE ImageCreateFromMem(const void* p, int nSize)
{
    if (!p || nSize <= 0)
        return 0;

    GdkPixbufLoader* loader = gdk_pixbuf_loader_new();
    if (!loader)
        return 0;

    GError* err = 0;
    gboolean ok = gdk_pixbuf_loader_write(loader, (const guchar*)p, (gsize)nSize, &err);
    if (ok)
        ok = gdk_pixbuf_loader_close(loader, &err);

    if (!ok)
    {
        if (err)
            g_error_free(err);
        g_object_unref(loader);
        return 0;
    }

    GdkPixbufAnimation* anim = gdk_pixbuf_loader_get_animation(loader);
    if (!anim)
    {
        g_object_unref(loader);
        return 0;
    }

    g_object_ref(anim);
    GdkPixbuf* pb = gdk_pixbuf_animation_get_static_image(anim);
    if (pb)
        g_object_ref(pb);
    g_object_unref(loader);

    return HIMAGE__::_create(pb, anim);
}

HIMAGE ImageCreateFromFile(TMCStr pFile)
{
    if (!pFile)
        return 0;

    GError* err = 0;
    GdkPixbufAnimation* anim = gdk_pixbuf_animation_new_from_file((const char*)pFile, &err);
    if (!anim)
    {
        if (err)
            g_error_free(err);
        return 0;
    }

    GdkPixbuf* pb = gdk_pixbuf_animation_get_static_image(anim);
    if (pb)
        g_object_ref(pb);

    return HIMAGE__::_create(pb, anim);
}

HIMAGE ImageCreateFromStream(IStream* stream, TM::IImageSizeDecide* pDecide, const TMColor* clrKey, TM::IColorTransform* pct, TMLParam context)
{
    tm_unused(pDecide);

    if (!stream)
        return 0;

    const TMUInt64 pos = StreamTell(stream);
    const TMUInt64 end = StreamSeekEnd(stream, 0);
    if (end <= pos)
    {
        StreamSeekSet(stream, (TMInt64)pos);
        return 0;
    }

    const size_t size = (size_t)(end - pos);
    StreamSeekSet(stream, (TMInt64)pos);

    std::vector<unsigned char> buf(size);
    TMULong readed = 0;
    if (S_OK != stream->Read(buf.data(), (TMULong)size, &readed) || readed != (TMULong)size)
    {
        StreamSeekSet(stream, (TMInt64)pos);
        return 0;
    }

    StreamSeekSet(stream, (TMInt64)pos);

    HIMAGE img = ImageCreateFromMem(buf.data(), (int)size);
    if (!img)
        return 0;

    if (pct)
    {
        EImageFormat ef = DibFormatFromMem(buf.data());
        if (ef != eImageFormatSVG && ef != eImageFormatGif87 && ef != eImageFormatGif89)
            ImageConvertColor(img, pct, context);
    }

    if (clrKey)
    {
        TMColor k = pct ? pct->ConvertColor(context, *clrKey) : *clrKey;
        ImageSetColorKey(img, k);
    }

    return img;
}

HIMAGE ImageCreateFromResource(TMHModule hInst, TMCStr pName, TMCStr pType)
{
    tm_unused(hInst);
    tm_unused(pName);
    tm_unused(pType);
    return 0;
}

HIMAGE ImageCreateFromHBmp(TMHBmp hObj)
{
    return HIMAGE__::_create(hObj, 0);
}

HIMAGE ImageCreateThumbnail(HIMAGE hImg, int w, int h)
{
    if (!hImg || w <= 0 || h <= 0 || !hImg->m_pb)
        return 0;

    GdkPixbuf* out = gdk_pixbuf_scale_simple(hImg->m_pb, w, h, GDK_INTERP_BILINEAR);
    if (!out)
        return 0;

    return HIMAGE__::_create(out, 0);
}

TMBool ImageSaveFile(HIMAGE hImg, TMCStr filename, TMBool bHaveAlpha)
{
    tm_unused(bHaveAlpha);

    if (!hImg || !filename || !hImg->m_pb)
        return false;

    gboolean ok = gdk_pixbuf_save(hImg->m_pb, (const char*)filename, _save_type_from_filename(filename), 0, NULL);
    return ok;
}

void ImageFill(HIMAGE hImg, TMColor clr, const TMRect* rc)
{
    GdkPixbuf* pb = hImg->m_pb;
    if (!pb)
        return;

    const int w = gdk_pixbuf_get_width(pb);
    const int h = gdk_pixbuf_get_height(pb);
    const int channels = gdk_pixbuf_get_n_channels(pb);
    const int stride = gdk_pixbuf_get_rowstride(pb);
    guchar* pixels = gdk_pixbuf_get_pixels(pb);

    int l = 0;
    int t = 0;
    int r = w;
    int b = h;
    if (rc)
    {
        l = _clampi((int)RectLeft(*rc), 0, w);
        t = _clampi((int)RectTop(*rc), 0, h);
        r = _clampi((int)RectRight(*rc), 0, w);
        b = _clampi((int)RectBottom(*rc), 0, h);
        if (l >= r || t >= b)
            return;
    }

    const guchar rr = ColorR(clr);
    const guchar gg = ColorG(clr);
    const guchar bb = ColorB(clr);
    const guchar aa = ColorA(clr);
    for (int y = t; y < b; ++y)
    {
        guchar* row = pixels + y * stride;
        for (int x = l; x < r; ++x)
        {
            guchar* p = row + x * channels;
            p[0] = rr;
            p[1] = gg;
            p[2] = bb;
            if (channels == 4)
                p[3] = aa;
        }
    }
}

void ImageSetColorKey(HIMAGE hImg, TMColor clr)
{
    if (!hImg || !hImg->m_pb || clr == cColorInvalid || !gdk_pixbuf_get_has_alpha(hImg->m_pb))
        return;

    const int w = gdk_pixbuf_get_width(hImg->m_pb);
    const int h = gdk_pixbuf_get_height(hImg->m_pb);
    const int stride = gdk_pixbuf_get_rowstride(hImg->m_pb);
    guchar* pixels = gdk_pixbuf_get_pixels(hImg->m_pb);

    const guchar rr = ColorR(clr);
    const guchar gg = ColorG(clr);
    const guchar bb = ColorB(clr);
    for (int y = 0; y < h; ++y)
    {
        guchar* row = pixels + y * stride;
        for (int x = 0; x < w; ++x)
        {
            guchar* p = row + x * 4;
            if (p[0] == rr && p[1] == gg && p[2] == bb)
                p[3] = 0;
        }
    }
}

void ImageConvertColor(HIMAGE hImg, TM::IColorTransform* pct, TMLParam context)
{
    if (!pct || !hImg->m_pb)
        return;

    const int w = gdk_pixbuf_get_width(hImg->m_pb);
    const int h = gdk_pixbuf_get_height(hImg->m_pb);
    const int channels = gdk_pixbuf_get_n_channels(hImg->m_pb);
    const int stride = gdk_pixbuf_get_rowstride(hImg->m_pb);
    guchar* pixels = gdk_pixbuf_get_pixels(hImg->m_pb);

    for (int y = 0; y < h; ++y)
    {
        guchar* row = pixels + y * stride;
        for (int x = 0; x < w; ++x)
        {
            guchar* p = row + x * channels;
            TMColor c = ColorFromRGBA(p[0], p[1], p[2], channels == 4 ? p[3] : 0xFF);
            c = pct->ConvertColor(context, c);
            p[0] = ColorR(c);
            p[1] = ColorG(c);
            p[2] = ColorB(c);
            if (channels == 4)
                p[3] = ColorA(c);
        }
    }
}

TMBool ImageConvertToText(HIMAGE hImg, TMColor clr)
{
    if (!hImg || !hImg->m_pb || !gdk_pixbuf_get_has_alpha(hImg->m_pb))
        return false;

    const int w = gdk_pixbuf_get_width(hImg->m_pb);
    const int h = gdk_pixbuf_get_height(hImg->m_pb);
    const int stride = gdk_pixbuf_get_rowstride(hImg->m_pb);
    guchar* pixels = gdk_pixbuf_get_pixels(hImg->m_pb);

    const guchar rr = ColorR(clr);
    const guchar gg = ColorG(clr);
    const guchar bb = ColorB(clr);
    for (int y = 0; y < h; ++y)
    {
        guchar* row = pixels + y * stride;
        for (int x = 0; x < w; ++x)
        {
            guchar* p = row + x * 4;
            guchar a = (guchar)((p[0] + p[1] + p[2]) / 3);
            p[0] = (guchar)(rr * a / 255);
            p[1] = (guchar)(gg * a / 255);
            p[2] = (guchar)(bb * a / 255);
            p[3] = a;
        }
    }

    return true;
}

int ImageAddRef(HIMAGE hImg)
{
    return hImg ? hImg->AddRef() : 0;
}

int ImageRelease(HIMAGE hImg)
{
    return hImg ? hImg->Release() : 0;
}

size_t ImageGetWidth(HIMAGE hImg)
{
    return (hImg && hImg->m_pb) ? gdk_pixbuf_get_width(hImg->m_pb) : 0;
}

size_t ImageGetHeight(HIMAGE hImg)
{
    return (hImg && hImg->m_pb) ? gdk_pixbuf_get_height(hImg->m_pb) : 0;
}

EGXFormat ImageGetFormat(HIMAGE hImg)
{
    GdkPixbuf* pb = _get_pixbuf(hImg);
    if (!pb)
        return eGXFormatUnknow;
    return gdk_pixbuf_get_has_alpha(pb) ? eGXFormat32A : eGXFormat24;
}

TMBool ImageConvertBitCount(HIMAGE hImg, int nBpp)
{
    GdkPixbuf* pb = _get_pixbuf(hImg);
    if (!pb)
        return false;

    const int channels = gdk_pixbuf_get_n_channels(pb);
    return ((nBpp == 24 && channels == 3) || (nBpp == 32 && channels == 4)) ? true : false;
}

size_t ImageGetFrameCount(HIMAGE hImg)
{
    return hImg ? hImg->m_frames : 1;
}

size_t ImageGetDelay(HIMAGE hImg)
{
    if (!hImg)
        return 0;
    return hImg->m_delay;    
}

TMBool ImageSetCurrentFrame(HIMAGE hImg, TMUInt nFrame)
{
    if (!hImg || !hImg->m_anim)
        return false;
    
    nFrame %= hImg->m_frames;
    if (nFrame == hImg->m_cur)
        return false;

    hImg->m_cur = (int)nFrame;

    glong timeMs = (glong)nFrame * hImg->m_delay;
    GTimeVal tv;
    tv.tv_sec = timeMs / 1000;
    tv.tv_usec = (timeMs % 1000) * 1000;

    GdkPixbufAnimationIter* iter = gdk_pixbuf_animation_get_iter(hImg->m_anim, &tv);
    if (!iter)
        return true;

    gdk_pixbuf_animation_iter_advance(iter, &tv);
    GdkPixbuf* pb = gdk_pixbuf_animation_iter_get_pixbuf(iter);
    if (pb)
        hImg->SetAndRef(pb);
    g_object_unref(iter);
    return true;
}

TMUInt ImageGetCurrentFrame(HIMAGE hImg)
{
    return hImg ? hImg->m_cur : 0;
}

TMHBmp ImageGetBitmap(HIMAGE hImg)
{
    return (TMHBmp)_get_pixbuf(hImg);
}

void* ImageLockBits(HIMAGE hImg, TMBool bForce)
{
    if (!hImg || !hImg->m_pb)
        return 0;
    tm_unused(bForce);
    GdkPixbuf* pb = _get_pixbuf(hImg);
    return pb ? gdk_pixbuf_get_pixels(pb) : 0;
}

void ImageLockRect(HIMAGE hImg, TMDibBuff& dib, const TMRect* rc, EGXLockRect lock)
{
    if (!hImg || !hImg->m_pb)
        return;
    tm_unused(lock);

    GdkPixbuf* pb = _get_pixbuf(hImg);
    if (!pb)
    {
        dib.Set();
        return;
    }

    const int w = gdk_pixbuf_get_width(pb);
    const int h = gdk_pixbuf_get_height(pb);
    const int channels = gdk_pixbuf_get_n_channels(pb);
    const int stride = gdk_pixbuf_get_rowstride(pb);
    guchar* pixels = gdk_pixbuf_get_pixels(pb);

    if (!rc)
    {
        dib.m_bits = pixels;
        dib.m_stride = stride;
        dib.m_height = h;
        return;
    }

    int l = _clampi((int)RectLeft(*rc), 0, w);
    int t = _clampi((int)RectTop(*rc), 0, h);
    int r = _clampi((int)RectRight(*rc), 0, w);
    int b = _clampi((int)RectBottom(*rc), 0, h);
    dib.m_bits = pixels + t * stride + l * channels;
    dib.m_stride = stride;
    dib.m_height = (b > t) ? (b - t) : 0;
}

TMHDC ImageCreateDC(HIMAGE hImg, TMHDC hDC)
{
    if (!hImg || !hImg->m_pb)
        return 0;
    cairo_surface_t* surface = gdk_cairo_surface_create_from_pixbuf(hImg->m_pb, 1, NULL);
    cairo_t* cr = cairo_create(surface);
    cairo_surface_destroy(surface);
    return cr;
}

void ImageDeleteDC(HIMAGE hImg, TMHDC hdc)
{
    cairo_destroy(hdc);
}

void ImageResetAsDib(HIMAGE hImg, GXDibData& dibData)
{
    tm_unused(hImg);
    tm_unused(dibData);
}

void ImageBitBlt(TMHDC hdc, IMGInt xDst, IMGInt yDst,
                 HIMAGE hImg, IMGInt xSrc, IMGInt ySrc, IMGInt wSrc, IMGInt hSrc,
                 TMUInt32 uFlag, TMUInt8 alpha)
{
    if (!hdc || !hImg || !hImg->m_pb || alpha == 0)
        return;

    IMGInt w = gdk_pixbuf_get_width(hImg->m_pb);
    IMGInt h = gdk_pixbuf_get_height(hImg->m_pb);
    if (wSrc > w)
        wSrc = w;
    if (hSrc > h)
        hSrc = h;
    ImageStretchBlt(hdc, xDst, yDst, wSrc, hSrc, hImg, xSrc, ySrc, wSrc, hSrc, uFlag, alpha);    
}

void ImageStretchBlt(TMHDC hdc, const TMRect& rcDst, HIMAGE hImg, const TMRect& rcSrc,
                     TMUInt32 uFlag, TMUInt8 alpha)
{
    if (!hdc || !hImg || !hImg->m_pb || alpha == 0)
        return;

    TM::CBltSelect bs(hdc, rcDst, rcSrc, uFlag, alpha, gdk_pixbuf_get_width(hImg->m_pb), gdk_pixbuf_get_height(hImg->m_pb), eGXFormat32A);
    bs.Blt(hImg->m_pb);
}
