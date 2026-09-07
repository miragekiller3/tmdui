/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.12.1
 *
 *  Copyright (C) 2015 miragekiller
 */

#pragma once

#ifndef GXIMAGE_WIN_WIC_H
#define GXIMAGE_WIN_WIC_H


//#if TM_MSC >= 1300
#define __IWICJpegFrameDecode_INTERFACE_DEFINED__
#define __IWICJpegFrameEncode_INTERFACE_DEFINED__
#include <wincodec.h>
// #else
// #   include "wincodec6.h"
// #endif

#include "tmdib.h"
#include "gxtexture_meta.h"



// const GUID CLSID_WICGifDecoderX = {0x381dda3c, 0x9ce9, 0x4834, 0xa2, 0x3e, 0x1f, 0x98, 0xf8, 0xfc, 0x52, 0xbe};
// const GUID CLSID_WICPngDecoderX = {0x389ea17b, 0x5078, 0x4cde, 0xb6, 0xef, 0x25, 0xc1, 0x51, 0x75, 0xc7, 0x51};
const GUID CLSID_WICImagingFactoryX = {0xcacaf262, 0x9370, 0x4615, 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0xa};
const GUID GUID_ContainerFormatPngX = {0x1b7cfaf4, 0x713f, 0x473c, 0xbb, 0xcd, 0x61, 0x37, 0x42, 0x5f, 0xae, 0xaf};
const GUID GUID_WICPixelFormatDontCareX = {0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x00};
//const GUID GUID_WICPixelFormat32bppBGRAX = {0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0f};
const GUID GUID_WICPixelFormat32bppPBGRAX = {0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x10};
DECLARE_GUID(GUID_WICPixelFormat32bppBGRX, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0e);


class CDecoder
{
public:
    CComPtr<IWICImagingFactory> factory;

public:
    CDecoder() throw()
    {
        DbgVerifyHResult(factory.CoCreateInstance(CLSID_WICImagingFactoryX));
    }

    void CopyOut32(TM::CTextureCreater* creater, IWICBitmapSource* obj, EGXFormat fmt)
    {
        UINT width, height;
        DbgVerifyHResult(obj->GetSize(&width, &height));
        
        WICRect rect;
        rect.X = 0;
        rect.Width = width;
        rect.Height = 1;
        creater->CreateIndirect(width, height, fmt);
        TM::CDibHandle32 d32;
        creater->LockRect(d32, 0);
        if (!d32)
            return;
        int wb = d32.GetWidthByte();
        for (int i = 0; i < height; ++i)
        {
            rect.Y = i;
            BYTE* p = d32.GetLineData(i);
            DbgVerifyHResult(obj->CopyPixels(&rect, wb, wb, p));
        }
        creater->Unlock();
    }

    void CopyOut(TM::CTextureCreater* creater, IWICBitmapSource* obj)
    {
        WICPixelFormatGUID fmtGuid;
        obj->GetPixelFormat(&fmtGuid);
        if (fmtGuid == GUID_WICPixelFormat32bppPBGRAX)
            CopyOut32(creater, obj, eGXFormat32A);
//         if (fmtGuid == GUID_WICPixelFormat32bppBGRX)
//             CopyOut32(creater, obj, eGXFormat32X);
        else
        {
            CComPtr<IWICFormatConverter> convert;
            DbgVerifyHResult(factory->CreateFormatConverter(&convert));
            DbgVerifyHResult(convert->Initialize(obj, GUID_WICPixelFormat32bppPBGRAX, WICBitmapDitherTypeNone, 0, 0, WICBitmapPaletteTypeCustom));
            CopyOut32(creater, convert, eGXFormat32A);
        }
    }

    void GetFrame(TM::CTextureCreater* creater, IWICBitmapDecoder* decoder, int nFrame)
    {
        CComPtr<IWICBitmapFrameDecode> frame;
        DbgVerifyHResult(decoder->GetFrame(nFrame, &frame));
        DbgAssert(frame);
        CopyOut(creater, frame);
    }

    EImageFormat CreateFromStream(TM::CTextureCreater* creater, IStream* stream)
    {
        CComPtr<IWICStream> stm;
        DbgVerifyHResult(factory->CreateStream(&stm));
        DbgVerifyHResult(stm->InitializeFromIStream(stream));

        CComPtr<IWICBitmapDecoder> decoder;
        DbgVerifyHResult(factory->CreateDecoderFromStream(stm, 0, WICDecodeMetadataCacheOnLoad, &decoder));
        if (!decoder)
            return eImageFormatUnknown;
        
        UINT nFrames = 0;
        DbgVerifyHResult(decoder->GetFrameCount(&nFrames));
        if (nFrames == 0)
            return eImageFormatUnknown;
        
        GetFrame(creater, decoder, 0);
        return eImageFormatPng;
    }

    void CreateFromIcon(TM::CTextureCreater* creater, HICON hIcon)
    {
        CComPtr<IWICBitmap> bmp;
        DbgVerifyHResult(factory->CreateBitmapFromHICON(hIcon, &bmp));
        if (bmp)
            CopyOut(creater, bmp);
    }

    void CreateFromHBMP(TM::CTextureCreater* creater, HBITMAP hBmp)
    {
        CComPtr<IWICBitmap> bmp(BitmapFromHBMP(hBmp, false));
        if (bmp)
            CopyOut(creater, bmp);
    }

    TMBool SaveFile(TMCStr strFile, IWICBitmap* spWicBitmap)
    {
        if (!spWicBitmap)
            return false;
        CComPtr<IWICStream> spStream;  
        DbgVerifyHResult(factory->CreateStream(&spStream));  
        HRESULT hr = spStream->InitializeFromFilename(TM_T2W(strFile), GENERIC_WRITE);
        if (hr != S_OK)
            return false;
        CComPtr<IWICBitmapEncoder> spBitmapEncoder;  
        CComPtr<IWICBitmapFrameEncode> spFrameEncode;  
        DbgVerifyHResult(factory->CreateEncoder(GUID_ContainerFormatPngX, NULL, &spBitmapEncoder));  
        DbgVerifyHResult(spBitmapEncoder->Initialize(spStream, WICBitmapEncoderNoCache));  
        DbgVerifyHResult(spBitmapEncoder->CreateNewFrame(&spFrameEncode, NULL));  
        DbgVerifyHResult(spFrameEncode->Initialize(NULL));
        UINT width = 0, height = 0;
        DbgVerifyHResult(spWicBitmap->GetSize(&width, &height));
        DbgVerifyHResult(spFrameEncode->SetSize(width,height));  
        WICPixelFormatGUID format = GUID_WICPixelFormatDontCareX;  
        DbgVerifyHResult(spFrameEncode->SetPixelFormat(&format));  
        DbgVerifyHResult(spFrameEncode->WriteSource(spWicBitmap, NULL));  
        DbgVerifyHResult(spFrameEncode->Commit());  
        DbgVerifyHResult(spBitmapEncoder->Commit());
        return true;
    }

    CComPtr<IWICBitmap> BitmapFromHBMP(HBITMAP hBmp, TMBool bHaveAlpha)
    {
        CComPtr<IWICBitmap> bmp;
        DbgVerifyHResult(factory->CreateBitmapFromHBITMAP(hBmp, NULL, bHaveAlpha ? WICBitmapUseAlpha : WICBitmapIgnoreAlpha, &bmp));
        return bmp;
    }

    void ThumbnailBitmap(TM::CTextureCreater* creater, HBITMAP hBmp, int uWidth, int uHeight)
    {
        // Create the scaler.
        CComPtr<IWICBitmapScaler> spBitmapScaler;
        DbgVerifyHResult(factory->CreateBitmapScaler(&spBitmapScaler));
        if (!spBitmapScaler)
            return;

        // Initialize the scaler to half the size of the original source.
        CComPtr<IWICBitmap> spBitmapSrc(BitmapFromHBMP(hBmp, true));
        DbgVerifyHResult(spBitmapScaler->Initialize(spBitmapSrc, uWidth, uHeight, WICBitmapInterpolationModeFant));
        
        // create thumbnail bitmap
        CComPtr<IWICBitmap> spBitmapDes;
        DbgVerifyHResult(factory->CreateBitmapFromSource(spBitmapScaler,  WICBitmapCacheOnLoad, &spBitmapDes));
        if (!spBitmapDes)
            return;

        // create texture
        CopyOut(creater, spBitmapDes);
    }
};

EImageFormat _ImageCreatePngFromStream(TM::CTextureCreater* creater, IStream* stream)
{
    return CDecoder().CreateFromStream(creater, stream);
};

TMBool _ImageSavePngFromHBITMAP(HBITMAP hBmp, TMCStr strFile, TMBool bHaveAlpha)
{
    CDecoder rec;
    return rec.SaveFile(strFile, rec.BitmapFromHBMP(hBmp, bHaveAlpha));
}


#endif // GXIMAGE_WIN_WIC_H