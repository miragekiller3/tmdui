/**
 *  @file
 *  @brief  Device-Independent Bitmaps
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2005.03.09
 *
 *  Copyright (C) 2005 miragekiller
 */

#include "tmdib.h"
#include "tmfile.h"
#include "tmdebug.h"
#include "tmcom.h"
//#include "wtdib.h"

#include <math.h>


static const TMUInt32 cs_cBitsBmp[] = {0x0000ffff, 'MB'};
static const TMUInt32 cs_cBitsG89[] = {0xffffffff, 0x0000ffff, '8FIG', 'a9'};
static const TMUInt32 cs_cBitsG87[] = {0xffffffff, 0x0000ffff, '8FIG', 'a7'};
static const TMUInt32 cs_cBitsJpg[] = {0x0000ffff, 0x0000d8ff};
static const TMUInt32 cs_cBitsPng[] = {0xffffffff, 0xffffffff, 0x474e5089, 0x0a1a0a0d};
static const TMUInt32 cs_cBitsIco[] = {0xffffffff, 0x00010000};
static const TMUInt32 cs_cBitsCur[] = {0xffffffff, 0x00020000};
static const TMUInt32 cs_cBitsWmf[] = {0xffffffff, 0x9ac6cdd7};
static const TMUInt32 cs_cBitsSvg[] = {0x000000ff, '<'};
static const TMUInt32 cs_cBitsSU8[] = {0xffffffff, 0x3cbfbbef};
static const TMUInt32 cs_cBitsJsn[] = {0x000000ff, '{'};
static const TMUInt32 cs_cBitsJU8[] = {0xffffffff, 0x7bbfbbef};

// case 0x00: {
//     if (data.length >= 12) {
//         //....ftypheic ....ftypheix ....ftyphevc ....ftyphevx
//         NSString *testString = [[NSString alloc] initWithData:[data subdataWithRange:NSMakeRange(4, 8)] encoding:NSASCIIStringEncoding];
//         if ([testString isEqualToString:@"ftypheic"]
//             || [testString isEqualToString:@"ftypheix"]
//             || [testString isEqualToString:@"ftyphevc"]
//             || [testString isEqualToString:@"ftyphevx"]) {
//             return SDImageFormatHEIC;


struct MIMEItem
{
    EImageFormat    m_format;
    TMUInt32        m_dwBitsSize;
    const TMUInt32* m_cBits;
};

static const MIMEItem cs_aMime[] = 
{
    {eImageFormatBmp,       sizeof(cs_cBitsBmp) / sizeof(TMUInt32) / 2, cs_cBitsBmp},
    {eImageFormatGif89,     sizeof(cs_cBitsG89) / sizeof(TMUInt32) / 2, cs_cBitsG89},
    {eImageFormatGif87,     sizeof(cs_cBitsG87) / sizeof(TMUInt32) / 2, cs_cBitsG87},
    {eImageFormatIco,       sizeof(cs_cBitsIco) / sizeof(TMUInt32) / 2, cs_cBitsIco},
    {eImageFormatCursor,    sizeof(cs_cBitsCur) / sizeof(TMUInt32) / 2, cs_cBitsCur},
    {eImageFormatSVG,       sizeof(cs_cBitsSvg) / sizeof(TMUInt32) / 2, cs_cBitsSvg},
    {eImageFormatSVG,       sizeof(cs_cBitsSU8) / sizeof(TMUInt32) / 2, cs_cBitsSU8},
    {eImageFormatPng,       sizeof(cs_cBitsPng) / sizeof(TMUInt32) / 2, cs_cBitsPng},
    {eImageFormatJpg,       sizeof(cs_cBitsJpg) / sizeof(TMUInt32) / 2, cs_cBitsJpg},
    {eImageFormatWmf,       sizeof(cs_cBitsWmf) / sizeof(TMUInt32) / 2, cs_cBitsWmf},
    {eImageFormatJason,     sizeof(cs_cBitsJsn) / sizeof(TMUInt32) / 2, cs_cBitsJsn},
    {eImageFormatJason,     sizeof(cs_cBitsJU8) / sizeof(TMUInt32) / 2, cs_cBitsJU8},
};

EImageFormat DibFormatFromMem(const void* pData)
{
    DbgAssert(pData);
    TMUInt32* pu = (TMUInt32*)pData;
    for (size_t i = 0; i < sizeof(cs_aMime) / sizeof(MIMEItem); ++i)
    {
        const MIMEItem& itm = cs_aMime[i];
        for (size_t j = 0; ; ++j)
        {
            if (j == itm.m_dwBitsSize)
                return itm.m_format;
            if ((pu[j] & itm.m_cBits[j]) != itm.m_cBits[j + itm.m_dwBitsSize])
                break;
        }
    }
    return eImageFormatUnknown;
}

EImageFormat DibFormatFromStream(IStream* stream)
{
    if (!stream)
        return eImageFormatUnknown;
    TMUInt8 c[32] = { 0 };
    stream->Read (c, 32, 0);
    LARGE_INTEGER li;
    li.QuadPart = -32;
    stream->Seek (li, STREAM_SEEK_CUR, 0);
    return DibFormatFromMem(c);
}

void DibBltRef(TMDibBuff& __d, const TMDibBuff& __s)
{
    DbgAssert(__d.GetHeight() == __s.GetHeight());
    DbgAssert(__d.GetWidthByte() >= __s.GetWidthByte());
//    DbgAssert(__d.GetWidthByte() < __s.GetWidthByte() + 128);
    if (__d.m_height == __s.m_height && __d.m_stride == __s.m_stride)
        MemCpy(__d.GetNativeMemBuffer(), __s.GetNativeMemBuffer(), __d.GetSizeImage());
    else
    {
        int y = __d.GetHeight();
        int wb = __d.GetWidthByte();
        tm_setmin(wb, __s.GetWidthByte());
        for (int i = 0; i < y; ++i)
            MemCpy(__d.GetLineData(i), __s.GetLineData(i), wb);
    }
}

template <class A>
void __dib_24_8to32(TM::CDibHandle32 d32, const TM::CDibHandle24& d24, const A& d8)
{
    int w = d32.GetWidth();
    int h = d32.GetHeight();
    for (int j = 0; j < h; ++j)
    {
        TM::CDibHandle32::iterator dst = d32.GetLine(j);
        TM::CDibHandle24::const_iterator src1 = d24.GetLine(j);
        for (int i = 0; i < w; ++i)
        {
            TM::CDibHandle24::value_type s24 = *src1;
            (*dst).SetColor(ColorFromRGB_A(s24.GetR(), s24.GetG(), s24.GetB(), d8.GetPixel(i, j)->GetR()));
            ++dst;
            ++src1;
        }
    }
}

void __dib_mix_24_8_to_32(TM::CDibHandle32 d32, const TM::CDibHandle24& d24, const TM::CDibHandleGray& alpha)
{
    int w = d32.GetWidth();
    int h = d32.GetHeight();
    if (alpha.GetWidth() == w && alpha.GetHeight())
        __dib_24_8to32(d32, d24, alpha);
    else
    {
        typedef TM::CFastXScaleDib<TM::CDibHandleGray>  ScaleGray8X;
        typedef TM::CFastYScaleDib<ScaleGray8X>         ScaleGray8;
        ScaleGray8 ds8(alpha);
        ds8.SetScaleWidth(w);
        ds8.SetScaleHeight(h);
        __dib_24_8to32(d32, d24, ds8);
    }
}

TMDibBuff __dibData2RefDib(TMDibBuff& dest, const GXDibData& dibData)
{
    TMDibBuff dib(
        dibData.bits,
        dibData.wb ? dibData.wb : dest.m_stride,
        (dibData.flag & GXDibData::eBottomUp) ? -dest.GetHeight() : dest.GetHeight());
    DbgAssert(dest.GetHeight() == dib.GetHeight());
    return dib;
}

TMBool DibResetAsDib(TMDibBuff& dest, EGXFormat fmtDst, const GXDibData& dibData)
{
    if (fmtDst == eGXFormat32A)
    {
        TM::CDibHandle32 d32(dest);
        if (dibData.flag & GXDibData::eKeepAlpha) // src must be 24 bit
        {
            TM::CDibHandle24 d24(__dibData2RefDib(dest, dibData));
            DbgAssert(d24.GetWidth() == d32.GetWidth());
            __dib_24_8to32(d32, d24, d32);
            return true;
        }
        else if (dibData.alpha) // src must be [0, 24 bit] + alpha channel
        {
            if (dibData.bits)
            {
                TM::CDibHandle24 d24(__dibData2RefDib(dest, dibData));
                DbgAssert(d24.GetWidth() == d32.GetWidth());
                __dib_mix_24_8_to_32(d32, d24, dibData.alpha);
            }
            else if (dibData.alpha.GetWidth() == d32.GetWidth() && dibData.alpha.GetHeight() == d32.GetHeight())
                DibReplaceAlpha(d32, dibData.alpha);
            else
            {
                typedef TM::CFastXScaleDib<TM::CDibHandleA>     ScaleA8X;
                typedef TM::CFastYScaleDib<ScaleA8X>            ScaleA8;
                ScaleA8 ds8(dibData.alpha);
                ds8.SetScaleWidth(d32.GetWidth());
                ds8.SetScaleHeight(d32.GetHeight());
                DibReplaceAlpha(d32, ds8);
            }
            return true;
        }
    }

    if (!dibData.bits || dibData.alpha)
        return false;
    
    TMDibBuff src(__dibData2RefDib(dest, dibData));
    DibBltRef(dest, src);
    return true;
}
