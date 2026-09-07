/*
 *  @file
 *  @brief gif
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.04.19
 *
 *  Copyright (C) 2010 miragekiller
 */

#pragma once

#ifndef GXGIF_H
#define GXGIF_H

#include <math.h>
#include "../tmlist.h"
#include "../tmio.h"
#include "../tmdib.h"
#include "../tmlzw.h"
#include "../tmmemfile.h"
#include "gxanisrc.h"


// #define GIF_TRACE DbgTrace
#define GIF_TRACE DbgTraceNull

struct DIB : public TMDibView
{
    EGXFormat m_format;          // BitsPixel
    TMHBmp m_bmp;
    //    HANDLE m_hFileMapping;
    
    int GetBpp() { return gxFormat2BPP(m_format); }
    TMUInt8* PixelPos(int x, int y);
    bool Create(int w, int h, EGXFormat format, RGBQUAD* ppal, int npal);
};

namespace GIF {

class CGifStream : public TM::IStreamImpl
{
protected:
    enum { e_size = 257 };
    IStream* m_pStream;
    WORD navail_bytes;              /* # bytes left in block */
    TMUInt8 byte_buff[e_size];               /* Current block */
    TMUInt8 *pbytes;                      /* Pointer to next byte in block */

public:
    CGifStream(IStream* pStream)
        : m_pStream (pStream)
        , pbytes (byte_buff)
    {
        pStream->Read(byte_buff, 1, 0);
        navail_bytes = 1;
    }

    ~CGifStream()
    {
        if (navail_bytes < 0)
            return;

        TMUInt8 b;
        while (b = 0, m_pStream->Read(&b, 1, 0), b)
            m_pStream->Seek(TM::LLINT(b).m, STREAM_SEEK_CUR, 0);
    }

    TMUInt8 get_next_code()
    {
        if (navail_bytes < 0)
            return 0;
        else if (navail_bytes == 0)
        {
            /* Out of bytes in current block, so read next block
             */
            TMUInt8 b = 0;
            m_pStream->Read(&b, 1, 0);
            if (b == 0)
            {
                navail_bytes = -1;
                return 0;
            }

            m_pStream->Read(byte_buff, b, 0);
            navail_bytes = b;
            pbytes = byte_buff;
        }
        --navail_bytes;
        return *pbytes++;
    }

    virtual HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead)
    {
        DbgAssert(cb == 1);
        *(TMUInt8*)pv = get_next_code();
        return S_OK;
    }
};

class CDibStream : public TM::IStreamImpl
{
private:
    int m_AlignedWidth;
    int m_Width; int m_Height; int m_Interlace; int m_clrIndex;
    TMUInt32* m_bufOut;
    TMUInt32* m_pPal;
    int RowOffset;
    int col;
    int row;

public:
    CDibStream(TMUInt32* bufOut, TMUInt32* pPal, int AlignedWidth, int Width, int Height, const int Interlace, int clrIndex)
        : m_AlignedWidth (AlignedWidth)
        , m_Width (Width)
        , m_Height (Height)
        , m_Interlace (Interlace)
        , m_clrIndex (clrIndex)
        , m_bufOut (bufOut)
        , m_pPal (pPal)
        , col (0)
        , row (0)
        , RowOffset (0)
    {
    }

    virtual HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten)
    {
        for (ULONG i = 0; i < cb; ++i)
        {
            if (col == m_Width)      // Check if new row.
            {
                if (m_Interlace)
                {
                    // If interlaced::
                    if ((row&7) == 0) {row += 8; if (row >= m_Height) row = 4;}
                    else if ((row&3) == 0) {row += 8; if (row >= m_Height) row = 2;}
                    else if ((row&1) == 0) {row += 4; if (row >= m_Height) row = 1;}
                    else row += 2;
                }
                else       // If not interlaced:
                    row++;
                
                RowOffset = row * m_AlignedWidth;  // Set new row offset
                col = 0;
            }
            
            TMUInt8 b = ((TMUInt8*)(pv))[i];
//            int z = RowOffset + col;
//             DbgTrace("Write RowOffset + col : %d", z);
            if (m_clrIndex != b) // (m_clrIndex == -1 || m_clrIndex != b) // 
                m_bufOut[RowOffset + col] = m_pPal[b]; // Write output
            col++; 
        }
        return S_OK;
    }
};

int LZWDecoder3(IStream* bufIn, TMUInt32 * bufOut, TMUInt32* pPal, int AlignedWidth,
                int Width, int Height, const int Interlace, int clrKey)
{
    //     bufOut += AlignedWidth * Height;
    CGifStream src(bufIn);
//     TM::CStreamT<TM::CMemBuf> dst;
//     dst.Assign((TMUInt8*)bufOut, Width * Height);
    CDibStream dst(bufOut, pPal, AlignedWidth, Width, Height, Interlace, clrKey);
    TMBool b = LzwDecode(&src, &dst);
    DbgAssert(b);
    return 0;
}

struct FRAME
{
    DIB dib;
    WORD nDelay;
};
typedef TM::CVector<FRAME> CFrameList;


class CAniSourceGif : public IAniSource
{
public:
    TM_SELF_ALLOC_F(CAniSourceGif);
#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        CAniSourceGif* p = (CAniSourceGif*)cls->m_pClass;
        return IC_Size(p->m_aFrame);
    }
#endif

public:
    int m_nDelay;
    CFrameList m_aFrame;
    
private:
    ~CAniSourceGif ()
    {
        for (CFrameList::recorder rec(m_aFrame); rec; ++rec)
            DbgVerify(DeleteObject(rec->dib.m_bmp));
    }

public:
    CAniSourceGif ()
        : m_nDelay(INT_MAX)
    {
    }
    
    virtual void Release() tm_override { delete this; }

    virtual int GetDelay() tm_override { return m_nDelay; }

    virtual void GetInfo(int& w, int& h, int& bpp, int& frames) tm_override
    {
        DIB& dib = m_aFrame[0].dib;
        w = dib.m_width;
        h = dib.m_height;
        bpp = dib.m_format;
        frames = m_aFrame.size();
    }
    
//     void Init ()
//     {
//         for (CFrameList::recorder rec(m_aFrame); rec; ++rec)
//         {
//             int d = rec->nDelay;
//             m_nDelay = min(m_nDelay, d);
//             //             m_nAllTime += d;
//         }
//     }
    
    // CAniSource
    virtual void GetFrame(int nFrame, TM::CDibHandle32& ld) tm_override
    {
        DIB& dib = m_aFrame[nFrame].dib; 
        memcpy(ld.GetData(), dib.m_bits, abs(dib.m_stride * dib.m_height));
    }
};




//using namespace TM;


#pragma pack(push, 1)   // turn byte alignment on

struct Header       // GIF header  
{
    char m_cSignature[3];   // 'GIF'
    char m_cVersion[3];        // "87a" or "89a"
};

struct LSDescriptor // Logical Screen Descriptor
{
    WORD m_wWidth;    // 2 bytes. Logical screen width
    WORD m_wHeight; // 2 bytes. Logical screen height
    unsigned char m_cPacked;      // packed field    
    unsigned char m_cBkIndex;     // 1 byte. Background color index
    unsigned char m_cPixelAspect; // 1 byte. Pixel aspect ratio
    
    TMBool IsLocalPal() { return m_cPacked & 0x80; }
    UINT GetBpp() { return (m_cPacked & 0x07) + 1; }
};

struct ImageDescriptor // image descriptor block
{
    //    unsigned char m_cImageSeparator; // image separator byte (0x2C)
    WORD m_wLeftPos; // image left position
    WORD m_wTopPos;  // image top position
    WORD m_wWidth;   // image width
    WORD m_wHeight;  // image height
    unsigned char m_cPacked; // packed field
    
    TMBool IsLocalPal() { return m_cPacked & 0x80; }
    TMBool IsInterlaced() { return m_cPacked & 0x40; }
    UINT GetBpp() { return (m_cPacked & 0x07) + 1; }
};

struct PlainTextExt // plain text extension block
{
    unsigned char m_cExtIntroducer;  // extension introducer (0x21)
    unsigned char m_cPlainTextLabel; // text extension label (0x01)
    unsigned char m_cBlockSize; // fixed value of 12
    WORD m_wLeftPos;    // text grid left position
    WORD m_wTopPos;     // text grid top position
    WORD m_wGridWidth;  // text grid width
    WORD m_wGridHeight; // text grid height
    unsigned char m_cCellWidth;  // character cell width
    unsigned char m_cCellHeight; // character cell height
    unsigned char m_cFgColor; // text foreground color index
    unsigned char m_cBkColor; // text background color index
};

struct ControlExt // graphic control extension block
{
    //      unsigned char m_cExtIntroducer; // extension introducer (0x21)
    //      unsigned char m_cControlLabel;  // control extension label (0xF9)
    unsigned char m_cBlockSize; // fixed value of 4
    BYTE m_transpcolflag:1;
    BYTE m_userinputflag:1;
    BYTE m_dispmeth:3;
    BYTE m_reserve:3;
    //    unsigned char m_cPacked;    // packed field
    WORD m_wDelayTime;    // delay time
    unsigned char m_cTColorIndex; // transparent color index
    unsigned char m_cBlockTerm;   // block terminator (0x00)
};

struct CommentExt  // comment extension block
{
    unsigned char m_cExtIntroducer; // extension introducer (0x21)
    unsigned char m_cCommentLabel;  // comment extension label (0xFE)
};

struct AppExtension // application extension block
{
    unsigned char m_cExtIntroducer; // extension introducer (0x21)
    unsigned char m_cExtLabel; // app. extension label (0xFF)
    unsigned char m_cBlockSize; // fixed value of 11
    char m_cAppIdentifier[8];   // application identifier
    char m_cAppAuth[3];  // application authentication code
};

#pragma pack(pop)


class CDecode
{
    DbgAssertS1(sizeof(LSDescriptor)       ==  7);
//    DbgAssertS2(sizeof(ImageDescriptor)    == 10);
    DbgAssertS3(sizeof(AppExtension)       == 14);
//     DbgAssertS4(sizeof(PlainTextExt)       == 15);
//      DbgAssertS5(sizeof(ControlExt)         ==  8);
    DbgAssertS6(sizeof(Header)             ==  6);

    enum
    {
        gifNoneSpecified =  0,              // no disposal method specified
        gifNoDispose =      1,              // do not dispose, leave the bits there
        gifRestoreBkgnd =   2,              // replace the image with the background color
        gifRestorePrev =    3               // replace the image with the previous pixels
    };

public:
//     CFrameList m_aFram;
    int m_nStart;
    int m_nWidth;
    int m_nHeight;
    TM::DibARGB m_clrTable[256];
    int m_nBk;
    int m_nBpp;
    void* m_pCache;
    CAniSourceGif* m_pSource;

    CDecode()
        : m_nBpp(8)
        , m_pCache(0)
    {
        m_pSource = new CAniSourceGif();
    }

    ~CDecode()
    {
    }
    
    void SkipDataBlok(IStream* stream)
    {
        TMUInt8 b;
        while (b = 0, stream->Read(&b, 1, 0), b)
        {
            stream->Seek(TM::LLINT(b).m, STREAM_SEEK_CUR, 0);
        }
    }
    
    void ReadPal(IStream* stream, int nBpp, TM::DibARGB* rgb, TM::IColorTransform* pct, TMLParam context)
    {
        TMUInt8 c[256 * 3];
        int nPalSize = 1 << nBpp;
        stream->Read(c, nPalSize * 3, 0);
        if (pct)
        {
            for (int i = 0; i < nPalSize; ++i)
            {
                TMColor clr = TMRGB(c[i * 3], c[i * 3 + 1], c[i * 3 + 2]);
                rgb[i].SetColor(pct->ConvertColor(context, clr));
                GIF_TRACE("ReadPal(index:%d, rgb:0X%X)", i, rgb[i].GetNativeColor());
            }
        }
        else
        {
            for (int i = 0; i < nPalSize; ++i)
            {
                rgb[i].SetR(c[i * 3]);
                rgb[i].SetG(c[i * 3 + 1]);
                rgb[i].SetB(c[i * 3 + 2]);
                rgb[i].SetA(0xff);
                GIF_TRACE("ReadPal(index:%d, rgb:0X%X)", i, rgb[i].GetNativeColor());
            }
        }
    }
    
    void ReadLS(IStream* stream, TM::IColorTransform* pct, TMLParam context)
    {
        LSDescriptor lsd = {0};
        stream->Read(&lsd, sizeof(lsd), 0);
        m_nWidth = lsd.m_wWidth;
        m_nHeight = lsd.m_wHeight;
        m_nBk = lsd.m_cBkIndex;
        if (lsd.IsLocalPal())
        {
            m_nBpp = lsd.GetBpp();
            ReadPal(stream, m_nBpp, m_clrTable, pct, context);
        }
        else
        { // default Palette
            GIF_TRACE("ReadLS::Set Default Palette");
            if (pct)
            {
                for (int i = 0; i < 256; ++i)
                    m_clrTable[i].SetColor(pct->ConvertColor(context, TMRGB(i,i,i)));
            }
            else
            {
                for (int i = 0; i < 256; ++i)
                    m_clrTable[i].SetRGBA((TMUInt8)i, (TMUInt8)i, (TMUInt8)i, 255);
            }
        }
    }
    
    template <class T>
    void Parse(T* creater, IStream* stream, TM::IColorTransform* pct, TMLParam context)
    {
        // skip file head
        stream->Seek(TM::LLINT(sizeof(Header)).m, STREAM_SEEK_CUR, 0);
        
        // read Logical Screen Descriptor
        ReadLS(stream, pct, context);
        
        ControlExt ceNow = {0};
        ControlExt cePre = {0};
        ImageDescriptor idPre = {0};

        for (; ;)
        {
            TMUInt8 indx = 0x3B;
            stream->Read(&indx, sizeof(indx), 0);
            switch (indx)
            {
            case '!': //extension block
                stream->Read(&indx, sizeof(indx), 0);
                switch (indx)
                {
                case 0xF9: // graphic control extension
                    stream->Read(&ceNow, sizeof(ceNow), 0);
                    GIF_TRACE("Read extension(cTColorIndex:%d, dispmeth:%d, transpcolflag:%d, wDelayTime:%d, BlockTerm:%d)",
                        ceNow.m_cTColorIndex, ceNow.m_dispmeth, ceNow.m_transpcolflag, ceNow.m_wDelayTime, ceNow.m_cBlockTerm);
                    continue;
                    
                case 0xFE:  // comment extension
                case 0x01:  // plain text extension
                case 0xFF:  // application extension
                default:    // Unknown extension
                    GIF_TRACE("Read SkipDataBlok(0X%X)", indx);
                    SkipDataBlok(stream);
                    continue;
                };
                break;
                
                case ',': // image data
                    {
                        ImageDescriptor id;
                        stream->Read(&id, sizeof(id), 0);
                        
                        // read pal
                        int nBpp = m_nBpp;
                        TM::DibARGB* pPal = m_clrTable;
                        TM::DibARGB clrTable[256];
                        int LocalColorMap = id.IsLocalPal();
                        if (LocalColorMap)
                        {
                            nBpp = id.GetBpp();
                            ReadPal(stream, nBpp, clrTable, pct, context);
                            pPal = clrTable;
                        }

                        // create dib
                        TM::CDibHandle32 d32;
//                        creater->AddFrame(m_nWidth, m_nHeight, 32, ceNow.m_wDelayTime * 10, d32);
                        {
                            FRAME frm;
                            MemZeroT(frm);
                            frm.dib.Create(m_nWidth, m_nHeight, eGXFormat32A, 0, 0);
                            int nDelay = ceNow.m_wDelayTime * 10;
                            tm_setmin(m_pSource->m_nDelay, nDelay);
                            frm.nDelay = nDelay;
                            m_pSource->m_aFrame.push_back(frm);
                            d32 = frm.dib;
                            d32.m_height = -d32.m_height;
                        }
                        
                        if (!d32)
                            return;
                        int stride = d32.m_height < 0 ? -d32.m_stride : d32.m_stride;

                        int nClrIndex = ceNow.m_transpcolflag ? ceNow.m_cTColorIndex : -1; //ce.m_cTColorIndex;
                        GIF_TRACE("Read image data(dispmethPrev:%d, Interlaced:%d, LocalPal:%d, bpp:%d, cPacked:%d, left:%d, top:%d, width:%d, height:%d)",
                            cePre.m_dispmeth, id.IsInterlaced(), id.IsLocalPal(), id.GetBpp(), id.m_cPacked, id.m_wLeftPos, id.m_wTopPos, id.m_wWidth, id.m_wHeight);

                        if (m_pCache)
                            MemCpy(d32.GetNativeMemBuffer(), m_pCache, d32.GetSizeImage());

                        if (cePre.m_dispmeth == gifRestoreBkgnd)
                        {
                            TM::DibARGB rgba = ceNow.m_transpcolflag ? TM::DibARGB(0,0,0,0) : pPal[m_nBk];
                            TM::CClipDib<TM::CDibHandle32> dibc(d32);
                            dibc.SetClip(idPre.m_wLeftPos, idPre.m_wTopPos, idPre.m_wWidth, idPre.m_wHeight);
                            DibFill(dibc, rgba); 
                        }

                        cePre = ceNow;
                        idPre = id;
                        if (ceNow.m_dispmeth <= gifNoDispose)
                            m_pCache = d32.GetNativeMemBuffer();
                        
                        LZWDecoder3(
                            stream,
                            (TMUInt32*) &d32.GetPixel(id.m_wLeftPos, id.m_wTopPos),
                            (TMUInt32*) pPal,
                            stride / sizeof(TM::DibARGB), // gf.dib.wb / sizeof(DibARGB),
                            id.m_wWidth, id.m_wHeight,
                            id.IsInterlaced(),
                            nClrIndex
                           );

//                        d32.DebugOut(0,0);
                    }
                    continue;
                    
                case ';': //trailer
                default:
                    break;
            }
            return;
        }
    }

    template <class T>
    void Done(T* creater)
    {
        creater->AddAni(m_pSource);
    }
    
}; // CGifDecode

} // namespace GIF

#endif // GXGIF_H