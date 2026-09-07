/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.03.18
 *
 *  Copyright (C) 2016 miragekiller
 */

#pragma once

#ifndef GXTEXTURE_META_H
#define GXTEXTURE_META_H

#include "../tmcom.h"
#include "../tmtxtfile.h"
#include "../tmdib.h"
#include "gxgif.h"
#include "svg/gxsvg.h"
#include "tmfilename.h"
#include <locale.h>


TM_BGN_NAMESPACE

// void NSVG_SetCT(TM::IColorTransform* pct, int nid);

struct CTextureCreater
{
public:
    // for bmp
    virtual TMBool CreateIndirect(int w, int h, EGXFormat format) = 0;
    virtual void LockRect(TMDibBuff& dib, const TMRect* rc) = 0; // -1: lock from mem first
    virtual void Unlock() = 0;

    // for ani
    virtual void AddAni(IAniSource* source) = 0;

protected:
    struct ICONDIR { TMUInt16 idReserved, idType, idCount; };
    struct ICONDIRENTRY { BYTE bWidth; BYTE bHeight; BYTE bColorCount; BYTE bReserved; WORD wPlanes; WORD wBitCount; DWORD dwBytesInRes; DWORD dwImageOffset; };
    void FromIcon(EImageFormat fmt, IStream* stream, IImageSizeDecide* pDecide, TM::IColorTransform* pct, TMLParam context)
    {
        CSize sz(32, 32);
        if (pDecide)
            pDecide->OnImageDecideSize(sz);
        int w = sz.cx; //dpi * 32.0f / (float)TM_DPI_SYSTEM_DEFAULT;
        int h = sz.cy; //w;
        ICONDIR idir = {0};
        TM::LLINT lint(0);
        TM::ULLINT nStart = 0;
        stream->Seek(lint.m, SEEK_CUR, &nStart);
        ULONG lRead;
        stream->Read(&idir, sizeof(ICONDIR), &lRead);
        if (idir.idCount > 100)
            return;
        
        typedef TM::CMultiMap<int, ICONDIRENTRY> CBInfoList;
        CBInfoList aInfo;
        int dsize = INT_MAX;
        tm_for (int i = 0; i < idir.idCount; ++i)
        {
            ICONDIRENTRY entry = {0};
            stream->Read(&entry, sizeof(entry), &lRead);
            int ww = entry.bWidth;
            int hh = entry.bHeight;
            if (!ww || !hh)
            {
                TMUInt32 c[6];
                int n = entry.dwImageOffset;
                lint = n;
                stream->Seek(lint + nStart, SEEK_SET, 0);
                stream->Read(c, sizeof(c), &lRead);
                if (c[0] == 0x28) // bitmap
                {
                    BITMAPINFOHEADER* p = (BITMAPINFOHEADER*)&c[0];
                    ww = p->biWidth;
                    hh = p->biHeight;
                    entry.wBitCount = p->biBitCount;
                }
                else //png
                {
                    DbgAssert(c[3] == 'RDHI');
                    ww = EndianN2H32(c[4]);
                    hh = EndianN2H32(c[5]);
                    if (!entry.wBitCount)
                        entry.wBitCount = 32;
                }
                lint = 0;
                stream->Seek(lint + sizeof(ICONDIR) + (i + 1) * sizeof(entry) + nStart, SEEK_SET, 0);
            }
            int ddx = ww - w;
            int ddy = ww - h;
            if (ddx < 0)
                ddx = -10 * ddx;
            if (ddy < 0)
                ddy = -10 * ddy;
            int ddsize = ddx + ddy;
            if (ddsize == dsize)
                aInfo.insert2(-entry.wBitCount, entry);
            else if (ddsize < dsize)
            {
                aInfo.clear();
                aInfo.insert2(-entry.wBitCount, entry);
                dsize = ddsize;
            }
        }

        DbgAssert(aInfo.size());
        ICONDIRENTRY bi = aInfo.begin()->second;
        CComPtr<IStream> ptrSub;
        void* pMem = 0;
        stream->Seek(TM::LLINT(bi.dwImageOffset) + nStart, SEEK_SET, 0);
        if (S_OK != StreamCreateReference(stream, bi.dwBytesInRes, &ptrSub, &pMem))
            return;

//         CIcon icon;
//         icon.CreateIconFromResourceEx((PBYTE)pMem, bi.dwBytesInRes);
        HICON icon = ::CreateIconFromResourceEx((PBYTE)pMem, bi.dwBytesInRes, TRUE, 0x00030000, 0, 0, LR_DEFAULTCOLOR);
        if (icon)
        {
            this->FromHICON(icon);
            ::DestroyIcon(icon);
            return;
        }
        
        CreateTextureFromStream(ptrSub, pDecide, pct, context);
    }

//     class CSvgCommonLocale
//     {
//     public:
//         CSvgCommonLocale()
//         {
//             m_strLocale = setlocale(LC_NUMERIC, 0);
//             setlocale(LC_NUMERIC, "C");
//         }
// 
//         ~CSvgCommonLocale()
//         {
//             setlocale(LC_NUMERIC, m_strLocale.c_str());
//         }
// 
//     private:
//         std::string m_strLocale;
//     };

    void FromSVG(EImageFormat fmt, IStream* stream, IImageSizeDecide* pDecide, TM::IColorTransform* pct, TMLParam context)
    {
        TM::CStringA svgXml;
        if (!TxtFileLoadFromStream(svgXml, stream))
            return;

        IAniSource* source = svgLoad(fmt, svgXml.data(), svgXml.size(), pct, context, pDecide);
        if (!source)
            return;
        
        AddAni(source);
        return;
/*

//         NSVG_SetCT(pct, context);

        CSvgCommonLocale _local;
        CSvgImage* pSvg = nsvgParse((char*)svgXml.c_str(), NSVG_UNITS_PX, TM_DPI_SYSTEM_DEFAULT, pct, context);
        if (!pSvg)
            return;

        NSVGrasterizer* pRast = nsvgCreateRasterizer();
        if (!pRast)
        {
            nsvgDelete(pSvg);
            return;
        }
        
        int ww, hh;
        nsvgGetImageSize(pSvg, ww, hh);
        CSize sz(ww, hh);
        GXDips scale = 1.0;
        if (pDecide)
            scale = pDecide->OnImageDecideSize(sz);
        int w = sz.Width();// scale * (float)ww + 0.5f;
        int h = sz.Height();// scale * (float)hh + 0.5f;
        TMBool b = this->CreateIndirect(w, h, eGXFormat32A);
        if (!b)
            return;

//        int stride = 0;
        CDibHandle32 dib;
        this->LockRect(dib, CRect(0,0,w,h));
//        unsigned char* bits = (unsigned char*)this->LockRect(stride, CRect(0,0,0,0));
        nsvgRasterize(pRast, pSvg, 0, 0, scale, dib.GetData(), w, h, dib.GetStride());
        DibPreBlend(dib);
        
        this->Unlock();
        //hTexture = 
        
        nsvgDeleteRasterizer(pRast);
        nsvgDelete(pSvg);
        */
    }

//     TMBool FromSkia(EImageFormat fmt, IStream* stream, IImageSizeDecide* pDecide)
//     {
// #ifdef _USRDLL
// //        static CDll dllRef(TM_SW_WIN64(TM_T("SkiaImage64.dll"), TM_T("SkiaImage.dll")), LOAD_LIBRARY_SEARCH_APPLICATION_DIR);
//         static CDll dllRef(TM_T("SkiaImage.dll"), LOAD_LIBRARY_SEARCH_APPLICATION_DIR);
//         if (!dllRef)
//             return false;
//         
//         PFNSkiaImageLoad fnLoad = (PFNSkiaImageLoad)dllRef.GetProcAddress(TM_NUMBER_OF(SkiaImageLoad));
//         if (!fnLoad)
//             return false;
//         
//         TM::CStringA txt;
//         if (!TxtFileLoadFromStream(txt, stream))
//             return false;
// 
//         IAniSource* source = fnLoad(fmt, txt.data(), txt.size(), pDecide);
//         if (!source)
//             return false;
// 
//         AddAni(source);
//         return true;
// #else
//         return false;
// #endif // _USRDLL
//     }

    void FromGIF(IStream* stream, TM::IColorTransform* pct, TMLParam context)
    {
        GIF::CDecode gd;
        gd.Parse(this, stream, pct, context);
        gd.Done(this);
//         EndAni();
    }

public:
    void FromHICON(HICON hIcon);

    virtual EImageFormat CreateTextureFromStream(IStream* stream, IImageSizeDecide* pDecide, TM::IColorTransform* pct, TMLParam context)
    {
        EImageFormat fmt = DibFormatFromStream(stream);
        switch (fmt)
        {
        case eImageFormatUnknown:
            return fmt;

        case eImageFormatGif89:
        case eImageFormatGif87:
            FromGIF(stream, pct, context);
            return fmt;

        case eImageFormatIco:
        case eImageFormatCursor:
            FromIcon(fmt, stream, pDecide, pct, context);
            return fmt;

        case eImageFormatSVG:
//             {
//                 TMUInt64 uCur = StreamTell(stream);
//                 if (!FromSkia(fmt, stream, pDecide))
//                 {
//                     StreamSeekSet(stream, uCur);
                    FromSVG(fmt, stream, pDecide, pct, context);
//                 }
//             }
            return fmt;

//         case eImageFormatJason:
//             FromSkia(fmt, stream, pDecide);
//             return fmt;
        }

        return eImageFormatUnknown;
    }
};


TM_END_NAMESPACE

#endif // GXTEXTURE_META_H
