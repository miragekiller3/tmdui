/*
 *  @file
 *  @brief  define basic GDI types and operations
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2004.12.08
 *
 *  Copyright (C) 2004 miragekiller
 */

#pragma once

#ifndef TMGDI_H
#define TMGDI_H

#include "tmdebug.h"
#include "tmmath.h"

#ifdef TM_QT
#   include <QRect>
#   include <QMatrix>
#endif

///////////////////////////////////////////////////////////////////////////////
// types
typedef struct HIMAGE__*                        HIMAGE;
typedef struct IGXTexture*                      GXPTexture;
typedef struct IGXText*                         GXPText;
struct IGXApp;

///////////////////////////////////////////////////////////////////////////////
// for ImageXXXBlt, IGXTexture::BltXXX;
// GX_BLT_SRCCOPY:          dst = src.rgba;
// GX_BLT_SRCCOPY + alpha:  dst = dst.rgba * (1 - alpha) + src.rgba * alpha.
// 0:                       dst = dst.rgba * (1 - src.a) + src.rgba
//
// warning: GX_BLT_X2Y need swap x,y
// sample:
//   CRect rcDst;
//   if (uFlag & GX_BLT_X2Y) rcDst.Rotate();
//   m_txt->Blt(rcDst, CRect(0,0,0,0), GX_BLT_DFTSRCWH | uFlag);
enum EGXBlt
{
    GX_BLT_NONE                     = 0x00000000,   // default: AlphaBlend(dest, src * alpha)
    GX_BLT_SRCCOPY                  = 0x80000000,   // dest = src * alpha
    GX_BLT_LINEAR                   = 0x40000000,   // bilinear. not support GDI.
    GX_BLT_DFTDSTWIDTH              = 0x08000000,   // dst use source width, the flag will ignore dpi.
    GX_BLT_DFTDSTHEIGHT             = 0x04000000,   // dst use source height, the flag will ignore dpi.
    GX_BLT_DFTSRCWIDTH              = 0x02000000,   // source use source width
    GX_BLT_DFTSRCHEIGHT             = 0x01000000,   // source use source height
    GX_BLT_DFTDSTWH                 = GX_BLT_DFTDSTWIDTH | GX_BLT_DFTDSTHEIGHT,
    GX_BLT_DFTSRCWH                 = GX_BLT_DFTSRCWIDTH | GX_BLT_DFTSRCHEIGHT,
    GX_BLT_DFTDSTSRCWH              = GX_BLT_DFTDSTWH    | GX_BLT_DFTSRCWH,
    GX_BLT_TILE                     = 0x00100000,   // tile(brush) mode, the flag ignore R2L/X2Y/DFTDSTSRCWH. not support blts. call OptimizeForTile() first;
    GX_BLT_TEXTMONO                 = 0x00200000,   // dst.rgba = src.a * textcolor. only support GXTexture, not support HIMAGE.

    GX_BLT_R2L                      = 0x00000010,   // swap destination coordinate right and left.
    GX_BLT_B2T                      = 0x00000020,   // swap destination coordinate top and bottom.
    GX_BLT_X2Y                      = 0x00000040,   // swap destination coordinate x and y.

    GX_BLT_ROTATE_0                 = 0x0,
    GX_BLT_ROTATE_0R                = GX_BLT_R2L,
    GX_BLT_ROTATE_90                = GX_BLT_X2Y | GX_BLT_B2T,
    GX_BLT_ROTATE_90R               = GX_BLT_X2Y | GX_BLT_B2T | GX_BLT_R2L,
    GX_BLT_ROTATE_180               = GX_BLT_B2T | GX_BLT_R2L,
    GX_BLT_ROTATE_180R              = GX_BLT_B2T,
    GX_BLT_ROTATE_270               = GX_BLT_X2Y | GX_BLT_R2L,
    GX_BLT_ROTATE_270R              = GX_BLT_X2Y,
    GX_BLT_ROTATE_MASK              = 0x00000070,
};
TM_ENUM_2_FLAG(EGXBlt);

// unused macro
#define TB_SRCCOPY                  GX_BLT_SRCCOPY
#define TB_DFTSRCWIDTH              GX_BLT_DFTSRCWIDTH
#define TB_DFTSRCHEIGHT             GX_BLT_DFTSRCHEIGHT
#define TB_DFTSRCWH                 GX_BLT_DFTSRCWH
#define IBB_SRCCOPY                 GX_BLT_SRCCOPY


///////////////////////////////////////////////////////////////////////////////
// for ImageLockRect, IGXTexture::LockRect;
enum EGXLockRect
{
    GX_LOCK_RECT_READ               = 0x01,
    GX_LOCK_RECT_WRITE              = 0x02,
    GX_LOCK_RECT_READWRITE          = 0x03,
};


///////////////////////////////////////////////////////////////////////////////
// format for image,texture

// sample: DWORD fmt = gxMakeFourCC('I', '4', '2', '0'),
#define gxMakeFourCC(ch0, ch1, ch2, ch3)                            \
    ((TMUInt32)(TMUInt8)(ch0)               |                       \
    ((TMUInt32)(TMUInt8)(ch1) << 8)         |                       \
    ((TMUInt32)(TMUInt8)(ch2) << 16)        |                       \
    ((TMUInt32)(TMUInt8)(ch3) << 24))

// sample: DWORD fmt = gxMakeFourCC1('I420');
#define gxMakeFourCC1(ch)                                           \
    (((((TMUInt32)ch) & 0x000000ff) << 24)  |                       \
    ((((TMUInt32)ch) & 0x00ff0000) >> 8)    |                       \
    ((((TMUInt32)ch) & 0x0000ff00) << 8)    |                       \
    ((((TMUInt32)ch) & 0xff000000) >> 24))


enum EGXFormat
{
    eGXFormatUnknow         = 0,
    eGXFormatNormalMAX      = '0000',

    eGXFormatPAL2           = 2,                                    // GDI
    eGXFormatPAL4           = 4,                                    // GDI
    eGXFormatPAL8           = 8,                                    // GDI
    eGXFormat16             = 16,                                   // GDI, != D3DFMT_X1R5G5B5
    eGXFormat24             = 24,                                   // GDI, != D3DFMT_R8G8B8
    eGXFormat32A            = 32,                                   // GDI:A8B8G8R8; D3D:A8R8G8B8;
    eGXFormat32X            = 33,                                   // GDI:X8B8G8R8; D3D:X8R8G8B8

    eGXFormatA8B8G8R8       = 18,                                   // != D3DFMT_A8B8G8R8
    eGXFormatX8B8G8R8       = 19,                                   // != D3DFMT_X8B8G8R8

    eGXFormatR8G8B8         = 20,                                   // D3DFMT_R8G8B8
    eGXFormatA8R8G8B8       = 21,                                   // D3DFMT_A8R8G8B8
    eGXFormatX8R8G8B8       = 22,                                   // D3DFMT_X8R8G8B8
    eGXFormatR5G6B5         = 23,                                   // D3DFMT_R5G6B5
    eGXFormatA1R5G5B5       = 25,                                   // D3DFMT_A1R5G5B5
    eGXFormatA4R4G4B4       = 26,                                   // D3DFMT_A4R4G4B4
    eGXFormatR3G3B2         = 27,                                   // D3DFMT_R3G3B2
    eGXFormatA8             = 28,                                   // D3DFMT_A8
    eGXFormatA8R3G3B2       = 29,                                   // D3DFMT_A8R3G3B2
    eGXFormatX4R4G4B4       = 30,                                   // D3DFMT_X4R4G4B4
    eGXFormatA2B10G10R10    = 31,                                   // D3DFMT_A2B10G10R10
//     eGXFormatA8B8G8R8       = 32,                                   // D3DFMT_A8B8G8R8, GDI
//     eGXFormatX8B8G8R8       = 33,                                   // D3DFMT_X8B8G8R8
    eGXFormatG16R16         = 34,                                   // D3DFMT_G16R16
    eGXFormatA2R10G10B10    = 35,                                   // D3DFMT_A2R10G10B10
    eGXFormatA16B16G16R16   = 36,                                   // D3DFMT_A16B16G16R16

    eGXFormatA8P8           = 40,                                   // D3DFMT_A8P8
    eGXFormatP8             = 41,                                   // D3DFMT_P8
    eGXFormatL8             = 50,                                   // D3DFMT_L8
    eGXFormatA8L8           = 51,                                   // D3DFMT_A8L8
    eGXFormatA4L4           = 52,                                   // D3DFMT_A4L4
    eGXFormatV8U8           = 60,                                   // D3DFMT_V8U8
    eGXFormatL6V5U5         = 61,                                   // D3DFMT_L6V5U5
    eGXFormatX8L8V8U8       = 62,                                   // D3DFMT_X8L8V8U8
    eGXFormatQ8W8V8U8       = 63,                                   // D3DFMT_Q8W8V8U8
    eGXFormatV16U16         = 64,                                   // D3DFMT_V16U16
    eGXFormatA2W10V10U10    = 67,                                   // D3DFMT_A2W10V10U10

    eGXFormatD16Lockable    = 70,                                   // D3DFMT_D16_LOCKABLE
    eGXFormatD32            = 71,                                   // D3DFMT_D32
    eGXFormatD15S1          = 73,                                   // D3DFMT_D15S1
    eGXFormatD24S8          = 75,                                   // D3DFMT_D24S8
    eGXFormatD24X8          = 77,                                   // D3DFMT_D24X8
    eGXFormatD24X4S4        = 79,                                   // D3DFMT_D24X4S4
    eGXFormatD16            = 80,                                   // D3DFMT_D16
    
    eGXFormatD32FLockable   = 82,                                   // D3DFMT_D32F_LOCKABLE
    eGXFormatD24FS8         = 83,                                   // D3DFMT_D24FS8
    
    
    eGXFormatL16            = 81,                                   // D3DFMT_L16
    
    eGXFormatVertexData     = 100,                                  // D3DFMT_VERTEXDATA
    eGXFormatIndex16        = 101,                                  // D3DFMT_INDEX16
    eGXFormatIndex32        = 102,                                  // D3DFMT_INDEX32
    
    eGXFormatQ16W16V16U16   = 110,                                  // D3DFMT_Q16W16V16U16
    
    // Floating point surface formats
    
    // s10e5 formats (16-bits per channel)
    eGXFormatR16F           = 111,                                  // D3DFMT_R16F
    eGXFormatG16R16F        = 112,                                  // D3DFMT_G16R16F
    eGXFormatA16B16G16R16F  = 113,                                  // D3DFMT_A16B16G16R16F
    
    // IEEE s23e8 formats (32-bits per channel)
    eGXFormatR32F           = 114,                                  // D3DFMT_R32F
    eGXFormatG32R32F        = 115,                                  // D3DFMT_G32R32F
    eGXFormatA32B32G32R32F  = 116,                                  // D3DFMT_A32B32G32R32F
    
    eGXFormatCxV8U8         = 117,                                  // D3DFMT_CxV8U8


    // 422
//     eGXFormatUYVY           = gxMakeFourCC1('UYVY'),
//     eGXFormatYVYU           = gxMakeFourCC1('YVYU'),
//     eGXFormatYUY2           = gxMakeFourCC1('YUY2'),                // YUV422I

    // 411/420; always support on D3D.
    eGXFormatNV12           = gxMakeFourCC1('NV12'),                // [yyyy][yyyy][uvuv]
//     eGXFormatNV21           = gxMakeFourCC1('NV21'),                // [yyyy][yyyy][vuvu] YUV420SP
//     eGXFormatYV12           = gxMakeFourCC1('YV12'),                // [yyyy][yyyy][vv][uu] D3DFMT_YUV420P_YV12
//     eGXFormatYU12           = gxMakeFourCC1('YU12'),                // [yyyy][yyyy][uu][vv] D3DFMT_YUV420P_YU12
//     eGXFormatI420           = gxMakeFourCC1('I420'),                // = YU12
//     eGXFormatIYUV           = gxMakeFourCC1('IYUV'),                // = YU12

    // rmvb
//     eGXFormatRV40           = gxMakeFourCC1('RV40'),

    eGXFormatDXVA_MP2A      = 0x1B81BE0A,                           // DXVA_ModeMPEG2_A
    eGXFormatDXVA_MP2C      = 0x1B81BE0C,                           // DXVA_ModeMPEG2_C
    eGXFormatDXVA_WMVA      = 0x1B81BE90,                           // DXVA_ModeWMV9_A
    eGXFormatDXVA_WMVB      = 0x1B81BE91,                           // DXVA_ModeWMV9_B
    eGXFormatDXVA_WMVC      = 0x1B81BE94,                           // DXVA_ModeWMV9_C
    eGXFormatDXVA_VC1A      = 0x1B81BEA0,                           // DXVA_ModeVC1_A(DXVA 2.0)
    eGXFormatDXVA_VC1B      = 0x1B81BEA1,                           // DXVA_ModeVC1_B(DXVA 2.0)
    eGXFormatDXVA_VC1C      = 0x1B81BEA2,                           // DXVA_ModeVC1_C(DXVA 2.0)
};

tm_constexpr14 inline int gxFormat2BPP(EGXFormat fmt) tm_noexcept
{
    switch (fmt)
    {
    case eGXFormatPAL2:
    case eGXFormatPAL4:
    case eGXFormatPAL8:
    case eGXFormat16:
    case eGXFormat24:
    case eGXFormat32A:
        return fmt;

    case eGXFormat32X:
    case eGXFormatA8B8G8R8:
    case eGXFormatX8B8G8R8:
        return 32;
    }
    return 0;
}

tm_constexpr11 inline bool gxFormatIsFourCC(EGXFormat fmt) tm_noexcept
{
    return fmt > '0000';
}

#ifdef TM_QT
tm_constexpr14 inline EGXFormat gxFormatFromQFormat(QImage::Format fmt) tm_noexcept
{
    switch (fmt)
    {
    case QImage::Format_Indexed8:   return eGXFormatPAL8;
    case QImage::Format_RGB555:     return eGXFormat16;
    case QImage::Format_RGB888:     return eGXFormat24;
    case QImage::Format_RGB32:      return eGXFormat32X;
    case QImage::Format_ARGB32:     return eGXFormat32A;
    case QImage::Format_RGBX8888:   return eGXFormat32X;
    case QImage::Format_RGBA8888:   return eGXFormat32A;
    }
    return eGXFormatUnknow;
}

tm_constexpr14 static inline QImage::Format gxBppFromQFormat(int nbpp) tm_noexcept
{
    switch (nbpp)
    {
    case 8: return QImage::Format_Indexed8;
    case 16: return QImage::Format_RGB16;
    case 24: return QImage::Format_RGB888;
    case 32: return QImage::Format_ARGB32;
    default: DbgAssert(0); return QImage::Format_Invalid;
    }
}

#endif // TM_QT


tm_constexpr14 inline static bool gxFormatIsContainAlpha(EGXFormat fmt) tm_noexcept
{
    switch (fmt)
    {
    case eGXFormat32A:
    case eGXFormat32X:
    case eGXFormatA8B8G8R8:
    case eGXFormatA8R8G8B8:
    case eGXFormatA1R5G5B5:
    case eGXFormatA4R4G4B4:
    case eGXFormatA8R3G3B2:
    case eGXFormatA2B10G10R10:
    case eGXFormatA2R10G10B10:
    case eGXFormatA16B16G16R16:
    case eGXFormatA2W10V10U10:
        return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// rect, point, size
TM_BGN_NAMESPACE
template <class T> struct t_value_type;

template <class T, class T_traits = t_value_type<T> > class CRectT;
template <class T, class T_traits = t_value_type<T> > class CSizeT;
template <class T, class T_traits = t_value_type<T> > class CPointT;

template <class t_int, class t_rc, class t_pt, class t_sz, bool bLTRB>
struct _rect_traits
{
    enum { e_bLTRBMode  = bLTRB };

    typedef _rect_traits                                _traits;
    typedef t_int                                       value_type;
    typedef t_rc                                        rect_type;
    typedef t_pt                                        point_type;
    typedef t_sz                                        size_type;

    DbgAssertS1(sizeof(t_rc) == 4 * sizeof(t_int));
    DbgAssertS2(sizeof(t_pt) == 2 * sizeof(t_int));
    DbgAssertS3(sizeof(t_sz) == 2 * sizeof(t_int));
};
TM_END_NAMESPACE


#if TM_WIN

#   include <windows.h>

    typedef long                GXIntNative;
    typedef float               GXFloatNative;
    typedef RECT                TMRectINative;
    typedef POINT               TMPointINative;
    typedef SIZE                TMSizeINative;
    struct  TMPointFNative      { GXFloatNative x, y; };
    struct  TMSizeFNative       { GXFloatNative cx, cy; };
    struct  TMRectFNative       { GXFloatNative left, top, right, bottom; };
    typedef TMRectINative       TMRectNative;
    typedef TMPointINative      TMPointNative;
    typedef TMSizeINative       TMSizeNative;

#elif TM_APPLE // #if TM_WIN

#   include <CoreGraphics/CoreGraphics.h>

    typedef int                 GXIntNative;
    typedef CGFloat             GXFloatNative;
    struct TMPointINative       { int x, y; };
    struct TMSizeINative        { int cx, cy; };
    struct TMRectINative        { int left, top, right, bottom; };
    typedef CGRect              TMRectFNative;
    typedef CGPoint             TMPointFNative;
    typedef CGSize              TMSizeFNative;
    typedef TMRectFNative       TMRectNative;
    typedef TMPointFNative      TMPointNative;
    typedef TMSizeFNative       TMSizeNative;

#   define bLTWH

#elif TM_ANDROID // #if TM_WIN #elif TM_APPLE

#   if TM_ANDROID >= 9
#       include "android/native_activity.h"
#   else
        typedef struct ARect { int32_t left,top,right,bottom; } ARect;
#   endif

    typedef int32_t             GXIntNative;
    typedef float               GXFloatNativeGXFloat;
    struct TMPoint              { int32_t x, y; };
    struct TMSize               { int32_t cx, cy; };
    typedef ARect               TMRect;

#elif TM_GTK // #if TM_WIN

#   include <gtk/gtktypes.h>

    typedef int                 GXIntNative;
    typedef float               GXFloatNative;
    typedef GdkRectangle        TMRectINative;
    struct TMPointINative       { int x, y; };
    struct TMSizeINative        { int cx, cy; };
#   if TM_GTK >= 400
    typedef graphene_rect_t     TMRectFNative;
    typedef graphene_point_t    TMPointFNative;
    typedef graphene_size_t     TMSizeFNative;
#   else
    struct TMRectFNative        { GXFloatNative x, y, width, height; };
    struct TMPointFNative       { GXFloatNative x, y; };
    struct TMSizeFNative        { GXFloatNative cx, cy; };
#   endif
    typedef TMRectINative       TMRectNative;
    typedef TMPointINative      TMPointNative;
    typedef TMSizeINative       TMSizeNative;

#   define bLTWH
#else // #if TM_WIN #elif TM_APPLE #elif TM_ANDROID

    typedef int                 GXIntNative;
    typedef float               GXFloatNative;
    struct  TMPointI            { int x, y; };
    struct  TMSizeI             { int cx, cy; };
    struct  TMRectI             { int left, top, right, bottom; };
    struct  TMPointF            { float x, y; };
    struct  TMSizeF             { float cx, cy; };
    struct  TMRectF             { float left, top, right, bottom; };
    typedef TMRectI             TMRect;
    typedef TMPointI            TMPoint;
    typedef TMSizeI             TMSize;

#endif // // #if TM_WIN #elif TM_APPLE #elif TM_ANDROID #else


#ifdef TM_QT
    typedef int                 GXInt;
    typedef qreal               GXFloat;
    typedef QRect               TMRectI;
    typedef QPoint              TMPointI;
    typedef QSize               TMSizeI;
    typedef QRectF              TMRectF;
    typedef QPointF             TMPointF;
    typedef QSizeF              TMSizeF;
    typedef QRect               TMRect;
    typedef QPoint              TMPoint;
    typedef QSize               TMSize;

#   define t_int                int
#   define t_rc                 QRect
#   define t_pt                 QPoint
#   define t_sz                 QSize
#   define QT_DX
#   include "atgdi_i.h"
    
#   define t_int                qreal
#   define t_rc                 QRectF
#   define t_pt                 QPointF
#   define t_sz                 QSizeF
#   define bLTWH
#   include "atgdi_i.h"

#else // #ifdef TM_QT
    typedef GXIntNative         GXInt;
    typedef GXFloatNative       GXFloat;
    typedef TMRectINative       TMRectI;
    typedef TMPointINative      TMPointI;
    typedef TMSizeINative       TMSizeI;
    typedef TMRectFNative       TMRectF;
    typedef TMPointFNative      TMPointF;
    typedef TMSizeFNative       TMSizeF;
    typedef TMRectNative        TMRect;
    typedef TMPointNative       TMPoint;
    typedef TMSizeNative        TMSize;

#endif // #ifdef TM_QT #else


#define t_int                   GXIntNative
#define t_rc                    TMRectINative
#define t_pt                    TMPointINative
#define t_sz                    TMSizeINative
#include "tmgdi_i.h"
    
#define t_int                   GXFloatNative
#define t_rc                    TMRectFNative
#define t_pt                    TMPointFNative
#define t_sz                    TMSizeFNative
#include "tmgdi_i.h"


///////////////////////////////////////////////////////////////////////////////
//  CRectT, CSizeT, CPointT
TM_BGN_NAMESPACE

template <class T, class T_traits>
class CRectT : public T
{
public:
    typedef T                                           base_class;
    typedef CRectT                                      class_type;
    typedef T_traits                                    traits;
    typedef typename T_traits::value_type               value_type;
    typedef value_type                                  difference_type;
    typedef typename T_traits::rect_type                rect_type;
    typedef typename T_traits::point_type               point_type;
    typedef typename T_traits::size_type                size_type;
    typedef CPointT<point_type>                         cpoint_type;
    typedef CSizeT<size_type>                           csize_type;

    enum { e_bLTRBMode = T_traits::e_bLTRBMode };

    value_type Left() const         { return RectLeft(*this); }
    value_type Top() const          { return RectTop(*this); }
    value_type Right() const        { return RectRight(*this); }
    value_type Bottom() const       { return RectBottom(*this); }
    value_type Width() const        { return RectWidth(*this); }
    value_type Height() const       { return RectHeight(*this); }
    void SetLeft(value_type l)      { RectSetLeft(*this, l); }
    void SetTop(value_type t)       { RectSetTop(*this, t); }
    void SetRight(value_type r)     { RectSetRight(*this, r); }
    void SetBottom(value_type b)    { RectSetBottom(*this, b); }
    void SetWidth(value_type w)     { RectSetWidth(*this, w); }
    void SetHeight(value_type h)    { RectSetHeight(*this, h); }

    CRectT() {}

    CRectT(value_type l, value_type t, value_type r, value_type b)
    { SetRect(l, t, r, b); }

    CRectT(const TMRectI& rhs) { CopyFrom(rhs); }
    CRectT(const TMRectF& rhs) { CopyFrom(rhs); }
#ifdef TM_QT
    CRectT(const TMRectINative& rhs) { CopyFrom(rhs); }
    CRectT(const TMRectFNative& rhs) { CopyFrom(rhs); }
#endif

    CRectT(const point_type& point, const size_type& size)
    { SetRect(point, size); }

    CRectT(const point_type& topLeft, const point_type& bottomRight)
    { SetRect(topLeft, bottomRight); }

    template <class P> void CopyFrom(const P& rhs)
    {
        if (e_bLTRBMode)
            SetRect(RectLeft(rhs), RectTop(rhs), RectRight(rhs), RectBottom(rhs));
        else
            SetLTWH(RectLeft(rhs), RectTop(rhs), RectWidth(rhs), RectHeight(rhs));
    }

    template <class P> void CopyTo(P& rhs)
    {
        RectSetLeft(rhs, Left());
        RectSetTop(rhs, Top());
        if (e_bLTRBMode)
        {
            RectSetRight(rhs, Right());
            RectSetBottom(rhs, Bottom());
        }
        else
        {
            RectSetWidth(rhs, Width());
            RectSetHeight(rhs, Height());
        }
    }

    csize_type Size() const
    { return csize_type(Width(), Height()); }

    cpoint_type TopLeft() const
    { return cpoint_type(Left(), Top()); }

    cpoint_type BottomRight() const
    { return cpoint_type(Right(), Bottom()); }

    value_type  CenterPointX() const { return (Left() + Right()) / 2; }
    value_type  CenterPointY() const { return (Top() + Bottom()) / 2; }
    cpoint_type CenterPoint()  const { return cpoint_type(CenterPointX(), CenterPointY()); }

    bool IsRectEmpty() const
    { return CMath::LesserEqual(Width(), (value_type)0) || CMath::LesserEqual(Height(), (value_type)0); }

    bool IsRectNull() const
    {
        return CMath::Equal0(GeometryNative0c(*this))
            && CMath::Equal0(GeometryNative1c(*this))
            && CMath::Equal0(RectNative2c(*this))
            && CMath::Equal0(RectNative3c(*this));
    }

    void Rotate()
    {
        value_type tmp = GeometryNative0(*this);
        GeometryNative0(*this) = GeometryNative1(*this);
        GeometryNative1(*this) = tmp;
        tmp = RectNative2(*this);
        RectNative2(*this) = RectNative3(*this);
        RectNative3(*this) = tmp;
    }

    bool PtInRect(value_type x, value_type y) const
    {
        return CMath::GreaterEqual(x, Left())
            && CMath::GreaterEqual(y, Top())
            && CMath::Lesser(x, Right())
            && CMath::Lesser(y, Bottom());
    }

    bool PtInRect(const cpoint_type& point) const
    {
        return PtInRect(PointX(point), PointY(point));
    }

    void SetRect(value_type l, value_type t, value_type r, value_type b)
    { SetLeft(l); SetTop(t); SetRight(r); SetBottom(b); }

    void SetLTWH(value_type l, value_type t, value_type w, value_type h)
    { SetLeft(l); SetTop(t); SetWidth(w); SetHeight(h); }

    void SetRect(const point_type& topLeft, const point_type& bottomRight)
    { SetRect(PointX(topLeft), PointY(topLeft), PointX(bottomRight), PointY(bottomRight)); }

    void SetRect(const point_type& point, const size_type& size)
    { SetLTWH(PointX(point), PointY(point), SizeWidth(size), SizeHeight(size)); }

    void SetRectEmpty()
    {
        GeometryNative0(*this) = 0;
        GeometryNative1(*this) = 0;
        if (e_bLTRBMode)
        {
            RectSetRight(*this, 0);
            RectSetBottom(*this, 0);
        }
        else
        {
            RectSetWidth(*this, 0);
            RectSetHeight(*this, 0);
        }
    }

    void CopyRect(const rect_type& rhs)
    { *(base_class*)this = rhs; }

    bool EqualRect(const rect_type& rhs) const
    {
        return CMath::Equal(GeometryNative0c(*this), GeometryNative0c(rhs))
            && CMath::Equal(GeometryNative1c(*this), GeometryNative1c(rhs))
            && CMath::Equal(RectNative2c(*this), RectNative2c(rhs))
            && CMath::Equal(RectNative3c(*this), RectNative3c(rhs));
    }

    bool EqualRect(const rect_type& rhs, value_type eps) const
    {
        return CMath::Equal(GeometryNative0c(*this), GeometryNative0c(rhs), eps)
            && CMath::Equal(GeometryNative1c(*this), GeometryNative1c(rhs), eps)
            && CMath::Equal(RectNative2c(*this), RectNative2c(rhs), eps)
            && CMath::Equal(RectNative3c(*this), RectNative3c(rhs), eps);
    }

    // left -= dw; top -= dh; right += dw; bottom += dh;
    void InflateRect(value_type dw, value_type dh)
    { this->InflateRect(dw, dh, dw, dh); }

    // left -= dl; top -= dt; right += dr; bottom += db;
    void InflateRect(value_type dl, value_type dt, value_type dr, value_type db)
    {
        SetLeft(Left() - dl);
        SetTop(Top() - dt);
        if (e_bLTRBMode)
        {
            SetRight(Right() + dr);
            SetBottom(Bottom() + db);
        }
        else
        {
            SetWidth(Width() + dl + dr);
            SetHeight(Height() + dt + db);
        }
    }

    // left -= x; top -= y; right += x; bottom += y;
    void InflateRect(const size_type& dwh)
    { this->InflateRect(SizeWidth(dwh), SizeHeight(dwh)); }

    // left -= l; top -= t; right += r; bottom += b;
    void InflateRect(const rect_type& dltwh)
    { this->InflateRect(RectLeft(dltwh), RectTop(dltwh), RectRight(dltwh), RectBottom(dltwh)); }

    void SetLeftRight(value_type l, value_type r)
    { SetLeft(l); SetRight(r); }

    void OffsetLeftRight(value_type dl, value_type dr)
    { SetLeftRight(Left() + dl, Right() + dr); }

    void SetTopBottom(value_type t, value_type b)
    { SetTop(t); SetBottom(b); }

    void OffsetTopBottom(value_type dt, value_type db)
    { SetTopBottom(Top() + dt, Bottom() + db); }

    void SetRightWidth(value_type r, value_type width)
    {
        SetLeft(r - width);
        if (e_bLTRBMode)
            SetRight(r);
        else
            SetWidth(width);
    }

    void SetBottomHeight(value_type b, value_type height)
    {
        SetTop(b - height);
        if (e_bLTRBMode)
            SetBottom(b);
        else
            SetHeight(height);
    }

    // left -= l; top -= t; right -= r; bottom -= b;
    void DistanceRect(const rect_type& rhs)
    { this->InflateRect(RectLeft(rhs), RectTop(rhs), -RectRight(rhs), -RectBottom(rhs)); }

    // left += dx; right += dx; top += dy; bottom += dy;
    void OffsetRect(value_type dx, value_type dy)
    { this->OffsetRectX(dx); OffsetRectY(dy); }

    // left += dx; right += dx;
    void OffsetRectX(value_type dx)
    {
        SetLeft(Left() + dx);
        if (e_bLTRBMode)
            SetRight(Right() + dx);
    }

    // top += dy; bottom += dy;
    void OffsetRectY(value_type dy)
    {
        SetTop(Top() + dy);
        if (e_bLTRBMode)
            SetBottom(Bottom() + dy);
    }

    void OffsetRect(const size_type& dxy)
    { this->OffsetRect(SizeWidth(dxy), SizeHeight(dxy)); }

    // left += l; right += r; top += t; bottom += b;
    void OffsetRect (const rect_type& drc)
    { this->InflateRect(-RectLeft(drc), -RectTop(drc), RectRight(drc), RectBottom(drc)); }

    void NormalizeRect()
    {
        if (e_bLTRBMode)
        {
            if (Left() > Right())
            {
                value_type tmp = Left();
                SetLeft(Right());
                SetRight(tmp);
            }
            if (Top() > Bottom())
            {
                value_type tmp = Top();
                SetTop(Bottom());
                SetBottom(tmp);
            }
        }
        else
        {
            if (Width() < 0)
            {
                SetLeft(Right());
                SetWidth(-Width());
            }
            if (Height() < 0)
            {
                SetTop(Right());
                SetHeight(-Width());
            }
        }
    }

    // new interface function
    // Keep?Set? and Keep?Offset?
    void KeepLSetW(value_type w)
    {
        this->SetWidth(w);
    }

    void KeepLOffsetW(difference_type dw)
    {
        this->SetWidth(Width() + dw);
    }

    void KeepLSetR(value_type r)
    {
        this->SetRight(r);
    }

    void KeepLOffsetR(difference_type dr)
    {
        if (e_bLTRBMode)
            this->SetRight(Right() + dr);
        else
            this->SetWidth(Width() + dr);
    }

    void KeepWSetL(value_type l)
    {
        this->KeepWOffset(l - Left());
    }

    // KeepWOffsetR == KeepWOffsetL
    void KeepWOffset(difference_type dx)
    {
        this->SetLeft(Left() + dx);
        if (e_bLTRBMode)
            this->SetRight(Right() + dx);
    }

    void KeepWSetR(value_type r)
    {
        this->KeepWOffset(r - Right());
    }

    void KeepRSetL(value_type l)
    {
        if (!e_bLTRBMode)
            this->SetWidth(Right() - l);
        this->SetLeft(l);
    }

    void KeepROffsetL(difference_type dl)
    {
        if (!e_bLTRBMode)
            this->SetWidth(Width() - dl);
        this->SetLeft(Left() + dl);
    }

    void KeepRSetW(value_type w)
    {
        this->SetLeft(Right() - w);
        if (!e_bLTRBMode)
            this->SetWidth(w);
    }

    void KeepROffsetW(difference_type dw)
    {
        this->SetLeft(Left() - dw);
        if (!e_bLTRBMode)
            this->SetWidth(Width() + dw);
    }

    void KeepTSetH(value_type h)
    {
        this->SetHeight(h);
    }

    void KeepTOffsetH(difference_type dh)
    {
        this->SetHeight(this->Height() + dh);
    }

    void KeepTSetB(value_type b)
    {
        this->SetBottom(b);
    }

    void KeepTOffsetB(difference_type db)
    {
        if (e_bLTRBMode)
            this->SetBottom(this->Bottom() + db);
        else
            this->SetHeight(this->Height() + db);
    }

    void KeepHSetT(value_type t)
    {
        this->KeepHOffset(t - this->Top());
    }

    // KeepHOffsetB == KeepHOffsetT
    void KeepHOffset(difference_type dy)
    {
        this->SetTop(this->Top() + dy);
        if (e_bLTRBMode)
            this->SetBottom(this->Bottom() + dy);
    }

    void KeepHSetB(value_type b)
    {
        this->KeepHOffset(b - this->Bottom());
    }

    void KeepBSetT(value_type t)
    {
        if (!e_bLTRBMode)
            this->SetHeight(this->Bottom() - t);
        this->SetTop(t);
    }

    void KeepBOffsetT(difference_type dt)
    {
        if (!e_bLTRBMode)
            this->SetHeight(this->Height() - dt);
        this->SetTop(this->Top() + dt);
    }

    void KeepBSetH(value_type h)
    {
        this->SetTop(this->Bottom() - h);
        if (!e_bLTRBMode)
            this->SetHeight(h);
    }

    void KeepBOffsetH(difference_type dh)
    {
        this->SetTop(this->Top() - dh);
        if (!e_bLTRBMode)
            this->SetHeight(this->Height() + dh);
    }

    void KeepCSetW(value_type w) // keep center set width
    {
        value_type l = Left();
        l = l + Width() / 2 - w / 2;
        KeepWSetL(l);
        KeepLSetW(w);
    }

    void KeepCSetH(value_type h) // keep center set height
    {
        value_type l = Top();
        l = l + Height() / (value_type)2 - h / (value_type)2;
        KeepHSetT(l);
        KeepTSetH(h);
    }

    // keep width/height
    void MoveToX(value_type x)
    { this->OffsetRectX(x - Left()); }

    void MoveToY(value_type y)
    { this->OffsetRectY(y - Top()); }

    void MoveToXY(value_type x, value_type y)
    { MoveToX(x); MoveToY(y); }

    void MoveToXY(point_type pt)
    { MoveToX(PointX(pt)); MoveToY(PointY(pt)); }

    // keep right
    void MoveLeftTo(value_type l)
    {
        if (!e_bLTRBMode)
            SetWidth(Width() + Left() - l);
        SetLeft(l);
    }

    // keep right
    void OffsetLeft(value_type dl)
    {
        if (!e_bLTRBMode)
            SetWidth(Width() - dl);
        SetLeft(Left() + dl);
    }

    // keep left
    void MoveRightTo(value_type r)
    { SetRight(r); }

    // keep left
    void OffsetRight(value_type dr)
    {
        if (e_bLTRBMode)
            SetRight(Right() + dr);
        else
            SetWidth(Width() + dr);
    }

    // keep bottom
    void MoveTopTo(value_type t)
    {
        if (!e_bLTRBMode)
            SetHeight(Height() + Top() - t);
        SetTop(t);
    }

    // keep bottom
    void OffsetTop(value_type dt)
    {
        if (!e_bLTRBMode)
            SetHeight(Height() - dt);
        SetTop(Top() + dt);
    }

    // keep top
    void MoveBottomTo(value_type b)
    { SetBottom(b); }

    // keep top
    void OffsetBottom(value_type db)
    {
        if (e_bLTRBMode)
            SetBottom(Bottom() + db);
        else
            SetHeight(Height() + db);
    }

    //  SubtractRect(1,2) => 1 - 2
   //   --------                 -------
   //  |1  ---  |            ---+---   2 |    ---
   //  |  | 2 | | = 2;      | 3 | 1 |    | = | 1 |;
   //  |   ---  |            ---+---     |    ---
   //   --------                 -------
    void IntersectRect(const rect_type& rect1, const rect_type& rect2)
    {
        value_type l1 = RectLeft(rect1), l2 = RectLeft(rect2);
        value_type t1 = RectTop(rect1), t2 = RectTop(rect2);
        value_type r1 = RectRight(rect1), r2 = RectRight(rect2);
        value_type b1 = RectBottom(rect1), b2 = RectBottom(rect2);
        SetRect(tm_max(l1, l2), tm_max(t1, t2), tm_min(r1, r2), tm_min(b1, b2));
    }

    //  false        true            true
    //  -          --------          -------
    // | |  -     |   ---  |     ---+--     |
    //  -  | |    |  |   | |    |      |    |
    //     | |    |   ---  |     ---+--     |
    //      -      --------          -------
    bool IsIntersect(const rect_type& rect) const
    {
        return CMath::LesserEqual(Left(), RectRight(rect))
            && CMath::LesserEqual(RectLeft(rect), Right())
            && CMath::LesserEqual(Top(), RectBottom(rect))
            && CMath::LesserEqual(RectTop(rect), Bottom());
    }

    //  -               -----
    // | |  -          |     |
    //  -  | |     =>  |     |
    //     | |         |     |
    //      -           -----
    void UnionRect(const rect_type& rect1, const rect_type& rect2)
    {
        if (((CRectT&)rect2).IsRectEmpty())
            return;
        if (IsRectEmpty())
            *this = rect2;
        else
        {
            value_type l1 = RectLeft(rect1), l2 = RectLeft(rect2);
            value_type t1 = RectTop(rect1), t2 = RectTop(rect2);
            value_type r1 = RectRight(rect1), r2 = RectRight(rect2);
            value_type b1 = RectBottom(rect1), b2 = RectBottom(rect2);
            SetRect(tm_min(l1, l2), tm_min(t1, t2), tm_max(r1, r2), tm_max(b1, b2));
        }
    }

    //  SubtractRect(1,2) => 1 - 2
    //   --------                 -------
    //  |1  ---  |            ---+--   2 |    ---
    //  |  | 2 | | = 1;      | 1 |  |    | = | 1 |;
    //  |   ---  |            ---+--     |    ---
    //   --------                 -------
    bool SubtractRect (const rect_type& rect1, const rect_type& rect2)
    {
        *this = rect1;
        CRectT tmp;
        tmp.IntersectRect(rect1, rect2);
        if (!tmp.IsRectEmpty())
        {
            if (tmp.EqualRect(*this))
            {
                SetRectEmpty();
                return false;
            }
            if (CMath::Equal(tmp.Top(), Top()) && CMath::Equal(tmp.Bottom(), Bottom()))
            {
                if (CMath::Equal(tmp.Left(), Left()))
                    MoveLeftTo(tmp.Right());
                else if (CMath::Equal(tmp.Right(), Right()))
                    MoveRightTo(tmp.Left());
            }
            else if (CMath::Equal(tmp.Left(), Left()) && CMath::Equal(tmp.Right(), Right()))
            {
                if (CMath::Equal(tmp.Top(), Top()))
                    MoveTopTo(tmp.Bottom());
                else if (CMath::Equal(tmp.Bottom(), Bottom()))
                    MoveBottomTo(tmp.Top());
            }
        }
        return true;
    }

    void ScaleRect(value_type sx, value_type sy)
    {
        GeometryNative0(*this) *= sx;
        GeometryNative1(*this) *= sy;
        if (e_bLTRBMode)
        {
            RectSetRight(*this, Right() * sx);
            RectSetBottom(*this, Bottom() * sy);
        }
        else
        {
            RectSetWidth(*this, Width() * sx);
            RectSetHeight(*this, Height() * sy);
        }
    }

    operator rect_type*()
    { return this; }

    operator const rect_type*() const
    { return this; }

    class_type& operator = (const rect_type& rhs)
    { this->CopyRect (rhs); return *this; }

    bool operator == (const rect_type& rhs) const
    { return this->EqualRect (rhs); }

    bool operator != (const rect_type& rhs) const
    { return !this->EqualRect (rhs); }

    void operator += (const size_type& size)
    { this->OffsetRect (SizeWidth(size), SizeHeight(size)); }

    void operator += (const rect_type& rhs)
    { this->InflateRect(rhs); }

    void operator -= (const size_type& size)
    { this->OffsetRect(-SizeWidth(size), -SizeHeight(size)); }

    void operator -= (const rect_type& rhs)
    { this->InflateRect(-RectLeft(rhs), -RectTop(rhs), -RectRight(rhs), -RectBottom(rhs)); }

    void operator &= (const rect_type& rhs)
    { this->IntersectRect (*this, rhs); }

    void operator |= (const rect_type& rhs)
    { this->UnionRect(*this, rhs); }

    class_type operator + (const size_type& size) const
    { class_type tmp(*this); tmp += size; return tmp; }

    class_type operator - (const size_type& size) const
    { class_type tmp(*this); tmp -= size; return tmp; }

    class_type operator + (const rect_type& rhs) const
    { class_type tmp(*this); tmp += rhs; return tmp; }

    class_type operator - (const rect_type& rhs) const
    { class_type tmp(*this); tmp -= rhs; return tmp; }

    class_type operator & (const rect_type& rhs) const
    { class_type tmp(*this); tmp &= rhs; return tmp; }

    class_type operator | (const rect_type& rhs) const
    { class_type tmp(*this); tmp |= rhs; return tmp; }

    bool Include (const rect_type& rhs) const
    {
        const CRectT& rh = rhs;
        return CMath::LesserEqual(Left(), rh.Left())
            && CMath::LesserEqual(Top(), rh.Top())
            && CMath::GreaterEqual(Right(), rh.Right())
            && CMath::GreaterEqual(Bottom(), rh.Bottom());
    }
/*
    UINT GetRelative(const rect_type& rhs)
    {
        UINT uf = 0;
        value_type l1, l2;
        l1 = RectLeft(*this); l2 = RectLeft(rhs);
        uf |= (l1 == l2 ? e_rrLeftEqual : (l1 < l2 ? e_rrLeftLess : e_rrLeftThan));
        l1 = RectTop(*this); l2 = RectTop(rhs);
        uf |= (l1 == l2 ? e_rrTopEqual : (l1 < l2 ? e_rrTopLess : e_rrTopThan));
        l1 = RectRight(*this), l2 = RectRight(rhs);
        uf |= (l1 == l2 ? e_rrRightEqual : (l1 < l2 ? e_rrRightLess : e_rrRightThan));
        l1 = RectBottom(*this), l2 = RectBottom(rhs);
        uf |= (l1 == l2 ? e_rrBottomEqual : (l1 < l2 ? e_rrBottomLess : e_rrBottomThan));
        return uf;
    }
*/

    bool _CalcUpDwonBestTopBottom(value_type te, value_type be)
    {
        if (be - te < Height())
            return false;
        if (Top() < te)
            MoveToY(te);
        else if (Bottom() > be)
            KeepHSetB(be);
        return true;
    }

    // for function: track pop menu
    // bUpDownFirst: first test up down space, then test left right space
    void CalcUpDownBestPos(const rect_type& rcExclude, const rect_type& rcMonitor)
    {
        value_type lm = RectLeft(rcMonitor);
        value_type rm = RectRight(rcMonitor);
        if (Left() < lm)
            MoveToX(lm);
        else if (Right() > rm)
            KeepWSetR(rm);

        value_type t = RectTop(*this);
        value_type b = RectBottom(*this);
        value_type te = RectTop(rcExclude);
        value_type be = RectBottom(rcExclude);
        value_type tm = RectTop(rcMonitor);
        value_type bm = RectBottom(rcMonitor);
        value_type center = t + b;
        value_type centerE = te + be;
        if (center < centerE) // up of center
        {
            if (_CalcUpDwonBestTopBottom(tm, te))
                return;
            if (_CalcUpDwonBestTopBottom(be, bm))
                return;
        }
        else
        {
            if (_CalcUpDwonBestTopBottom(be, bm))
                return;
            if (_CalcUpDwonBestTopBottom(tm, te))
                return;
        }

        _CalcUpDwonBestTopBottom(tm, bm);
    }

    template <class _RC> void Round(_RC& rhs)
    {
        RectSetLeft(rhs, CMath::Round(Left()));
        RectSetTop(rhs, CMath::Round(Top()));
        RectSetRight(rhs, CMath::Round(Right()));
        RectSetBottom(rhs, CMath::Round(Bottom()));
    }

    template <class _RC> void Floor(_RC& rhs)
    {
        RectSetLeft(rhs, CMath::Floor(Left()));
        RectSetTop(rhs, CMath::Floor(Top()));
        RectSetRight(rhs, CMath::Floor(Right()));
        RectSetBottom(rhs, CMath::Floor(Bottom()));
    }

    template <class _RC> void Ceil(_RC& rhs)
    {
        RectSetLeft(rhs, CMath::Ceiling(Left()));
        RectSetTop(rhs, CMath::Ceiling(Top()));
        RectSetRight(rhs, CMath::Ceiling(Right()));
        RectSetBottom(rhs, CMath::Ceiling(Bottom()));
    }

    void ScaleDownTo(float fAspectRatio)
    {
        float w = Width();
        float h = Height();
        float w2 = fAspectRatio * h;
        if (w2 <= w)
            SetWidth(w2);
        else
            SetHeight(w / fAspectRatio);
    }

    void ScaleUpTo(float fAspectRatio)
    {
        float w = Width();
        float h = Height();
        float w2 = fAspectRatio * h;
        if (w2 >= w)
            SetWidth(w2);
        else
            SetHeight(w / fAspectRatio);
    }

}; // class CRectT<_gdi_rc>


template <class T, class T_traits>
class CSizeT : public T
{
public:
    typedef T                                           base_class;
    typedef CSizeT                                      class_type;
    typedef typename T_traits::value_type               value_type;
    typedef value_type                                  difference_type;
    typedef typename T_traits::rect_type                rect_type;
    typedef typename T_traits::point_type               point_type;
    typedef typename T_traits::size_type                size_type;
    typedef CPointT<point_type>                         cpoint_type;

public:
    CSizeT() {}

    CSizeT(value_type initCX, value_type initCY)
    { SetWidth(initCX); SetHeight(initCY); }

//     CSizeT (const base_class& rhs) : base_class(rhs) {}
    CSizeT(const TMSizeI& rhs) { CopyFrom(rhs); }
    CSizeT(const TMSizeF& rhs) { CopyFrom(rhs); }
#ifdef TM_QT
    CSizeT(const TMRectINative& rhs) { CopyFrom(rhs); }
    CSizeT(const TMRectFNative& rhs) { CopyFrom(rhs); }
#endif

    CSizeT (TMLParam dwSize) { SetWidth((short)TM_LOWORD(dwSize)); SetHeight((short)TM_HIWORD(dwSize)); }

    TMLParam ToLPARAM() const { return TM_MAKELPARAM(Width(), Height()); }

    template <class P> void CopyTo(P& rhs)
    {
        SizeSetWidth(rhs, Width());
        SizeSetHeight(rhs, Height());
    }

    template <class P> void CopyFrom(P& rhs)
    {
        SetSize(SizeWidth(rhs), SizeHeight(rhs));
    }

    value_type Width() const { return SizeWidth(*this); }
    value_type Height() const { return SizeHeight(*this); }

    void SetWidth(value_type w) { SizeSetWidth(*this, w); }
    void SetHeight(value_type h) { SizeSetHeight(*this, h); }

    void OffsetWidth(value_type dw) { SetWidth(Width() + dw); }
    void OffsetHeight(value_type dh) { SetHeight(Height() + dh); }

    void Rotate()
    {
        value_type tmp = GeometryNative0(*this);
        GeometryNative0(*this) = GeometryNative1(*this);
        GeometryNative1(*this) = tmp;
    }

    bool operator == (const base_class& rhs) const
    { return CMath::Equal(Width(), SizeWidth(rhs)) && CMath::Equal(Height(), SizeHeight(rhs)); }

    bool operator != (const base_class& rhs) const
    { return ! operator == (rhs); }

    void operator += (const base_class& size)
    { SetWidth(Width() + SizeWidth(size)); SetHeight(Height() + SizeHeight(size)); }

    void operator -= (const base_class& size)
    { SetWidth(Width() - SizeWidth(size)); SetHeight(Height() - SizeHeight(size)); }

    template <class _Tp> void operator *= (_Tp scale)
    { GeometryNative0(*this) *= scale; GeometryNative1(*this) *= scale; }

    template <class _Tp> void operator /= (_Tp scale)
    { GeometryNative0(*this) /= scale; GeometryNative1(*this) /= scale; }

    void SetSize(value_type CX, value_type CY)
    { SetWidth(CX); SetHeight(CY); }

    class_type operator + (const base_class& size) const
    { class_type tmp(*this); tmp += size; return tmp; }

    class_type operator - (const base_class& size) const
    { class_type tmp(*this); tmp -= size; return tmp; }

    class_type operator - () const
    { return class_type(-Width(), -Height()); }

    cpoint_type& ToPoint()
    {
        DbgAssertS(sizeof(point_type) == sizeof(class_type));
        return *(point_type*)this;
    }

    const cpoint_type& ToPoint() const
    {
        DbgAssertS(sizeof(point_type) == sizeof(class_type));
        return *(const point_type*)this;
    }

    cpoint_type operator + (const point_type& point) const
    { point_type pt = {Width() + PointX(point), Height() + PointY(point)}; return pt; }

    cpoint_type operator - (const point_type& point) const
    { point_type pt = {Width() - PointX(point), Height() - PointY(point)}; return pt; }

    // zoom in/out self to fit parent; return: scale rate.
    float FitTo(const size_type& szParent)
    {
        value_type pw = SizeWidth(szParent);
        value_type ph = SizeHeight(szParent);
        
        float scalex = (float)pw / (float)Width();
        float scaley = (float)ph / (float)Height();
        if (scalex > scaley)
        {
            SetWidth(scaley * Width());
            SetHeight(ph);
            return scaley;
        }
        else
        {
            SetWidth(pw);
            SetHeight(scalex * Height());
            return scalex;
        }
    }

    // zoom in self to fit parent;
    // if self less than parent do nothing.
    float FitTo_ZoomIn(const size_type& szParent)
    {
        value_type w = SizeWidth(szParent);
        value_type h = SizeHeight(szParent);
        if (w > Width() || h > Height())
            return FitTo(szParent);
        return 1;
    }

    // zoom out self to fit parent;
    // if self greater than parent do nothing.
    float FitTo_ZoomOut(const size_type& szParent)
    {
        value_type w = SizeWidth(szParent);
        value_type h = SizeHeight(szParent);
        if (Width() < w || Height() < h)
            return FitTo(szParent);
        return 1;
    }


//     CRectT<_gdi_rc> operator + (const _gdi_rc& rect) const
//     { return *(CRectT<_gdi_rc>*)&(rect) + *this; }
//
//     CRectT<_gdi_rc> operator - (const _gdi_rc& rect) const
//     { return *(CRectT<_gdi_rc>*)&(rect)- *this; }

}; // class CSizeT<_gdi_sz>



template <class T, class T_traits>
class CPointT : public T
{
public:
    typedef T                                           base_class;
    typedef CPointT                                     class_type;
    typedef typename T_traits::value_type               value_type;
    typedef value_type                                  difference_type;
    typedef typename T_traits::rect_type                rect_type;
    typedef typename T_traits::point_type               point_type;
    typedef typename T_traits::size_type                size_type;
    typedef CSizeT<size_type>                           csize_type;

public:
    CPointT () { /* random filled */ }

    CPointT(TMLParam lParam)
    { SetX((short)TM_LOWORD(lParam)); SetY((short)TM_HIWORD(lParam)); }

    CPointT(value_type initX, value_type initY)
    { SetX(initX); SetY(initY); }

    CPointT(const TMPointI& rhs) { CopyFrom(rhs); }
    CPointT(const TMPointF& rhs) { CopyFrom(rhs); }
#ifdef TM_QT
    CPointT(const TMPointINative& rhs) { CopyFrom(rhs); }
    CPointT(const TMPointFNative& rhs) { CopyFrom(rhs); }
#endif

    CPointT(const size_type& initSize) : base_class(*(const base_class*)&initSize)
    { DbgAssertS(sizeof(size_type) == sizeof(point_type)); }

    template <class P> void CopyTo(P& rhs)
    {
        PointSetX(rhs, X());
        PointSetY(rhs, Y());
    }

    template <class P> void CopyFrom(P& rhs)
    {
        SetPoint(PointX(rhs), PointY(rhs));
    }

    TMLParam ToLPARAM() const { return TM_MAKELPARAM(X(), Y()); }

    value_type X() const { return PointX(*this); }
    value_type Y() const { return PointY(*this); }

    void SetX(value_type _x) { PointSetX(*this, _x); }
    void SetY(value_type _y) { PointSetY(*this, _y); }

    void OffsetX(value_type dw) { SetX(X() + dw); }
    void OffsetY(value_type dh) { SetY(Y() + dh); }

//     TMBool PtInRect(const rect_type& rc) const
//     { return ((CRectT<rect_type>*)&rc)->PtInRect(*this); }

    bool PtInRect(value_type l, value_type t, value_type r, value_type b) const
    { return X() >= l && X() < r && Y() >= t && Y() < b; }

    void Offset(value_type xOffset, value_type yOffset)
    { SetX(X() + xOffset); SetY(Y() + yOffset); }

    void Offset(const size_type& size)
    { Offset(SizeWidth(size), SizeHeight(size)); }

    void Offset(const class_type& pt)
    { Offset(PointX(pt), PointY(pt)); }

    void Scale(value_type sx, value_type sy)
    { SetPoint(X() * sx, Y() * sy); }

    bool operator == (const point_type& point) const
    { return CMath::Equal(X(), PointX(point)) && CMath::Equal(Y(), PointY(point)); }

    bool operator != (const point_type& point) const
    { return ! operator == (point); }

    void operator += (const size_type& size)
    { Offset(size); }

    void operator += (const class_type& pt)
    { Offset(pt); }

    void operator -= (const size_type& size)
    { Offset(-SizeWidth(size), -SizeHeight(size)); }

    class_type& operator *= (value_type f) { SetPoint(X() * f, Y() * f); return *this; }
    class_type operator * (value_type f) const { return class_type(X() * f, Y()*f); }

    void SetPoint (value_type X, value_type Y)
    { SetX(X); SetY(Y); }

    class_type operator + (const size_type& size) const
    { return class_type(X() + SizeWidth(size), Y() + SizeHeight(size)); }

    class_type operator - (const size_type& size) const
    { return class_type(X() - SizeWidth(size), Y() - SizeHeight(size)); }

    class_type operator - () const
    { return class_type(-X(), -Y()); }

    csize_type& ToSize()
    {
        DbgAssertS(sizeof(size_type) == sizeof(class_type));
        DbgAssert(csize_type(X(), Y()) == *(csize_type*)this);
        return *(csize_type*)this;
    }

    const csize_type& ToSize () const
    {
        DbgAssertS(sizeof(size_type) == sizeof(class_type));
        DbgAssert(csize_type(X(), Y()) == *(csize_type*)this);
        return *(csize_type*)this;
    }

    csize_type operator - (const point_type& point) const
    { size_type sz = {X() - PointX(point), Y() - PointY(point)}; return sz; }

    value_type Distance(const point_type& point) const
    {
        value_type dx = X() - PointX(point);
        value_type dy = Y() - PointY(point);
        return sqrt(dx * dx  + dy * dy);
    }

}; // class CPointT<_gdi_pt>

typedef CRectT<TMRect>          CRect;
typedef CSizeT<TMSize>          CSize;
typedef CPointT<TMPoint>        CPoint;

typedef CRectT<TMRectI>         CRectI;
typedef CSizeT<TMSizeI>         CSizeI;
typedef CPointT<TMPointI>       CPointI;

typedef CRectT<TMRectF>         CRectF;
typedef CSizeT<TMSizeF>         CSizeF;
typedef CPointT<TMPointF>       CPointF;

typedef CRectT<TMRectNative>    CRectNative;
typedef CSizeT<TMSizeNative>    CSizeNative;
typedef CPointT<TMPointNative>  CPointNative;

typedef CRectT<TMRectINative>   CRectINative;
typedef CSizeT<TMSizeINative>   CSizeINative;
typedef CPointT<TMPointINative> CPointNativeI;

typedef CRectT<TMRectFNative>   CRectFNative;
typedef CSizeT<TMSizeFNative>   CSizeFNative;
typedef CPointT<TMPointFNative> CPointNativeF;

TM_END_NAMESPACE


// xxxD: device; xxxL:logic
typedef TMRect                      TMRectD;
typedef TMPoint                     TMPointD;
typedef TMSize                      TMSizeD;
typedef TMRectF                     TMRectL;
typedef TMPointF                    TMPointL;
typedef TMSizeF                     TMSizeL;

TM_BGN_NAMESPACE
    typedef CRect                   CRectD;
    typedef CPoint                  CPointD;
    typedef CSize                   CSizeD;
    typedef CRectF                  CRectL;
    typedef CPointF                 CPointL;
    typedef CSizeF                  CSizeL;
TM_END_NAMESPACE

typedef TM::CRectL                  GXRect;
typedef TM::CSizeL                  GXSize;
typedef TM::CPointL                 GXPoint;
typedef GXRect::value_type          GXDips; // device-independent pixels
#define GXDIPS_MAX                  FLT_MAX
#define GXDIPS_MIN                  FLT_MIN





///////////////////////////////////////////////////////////////////////////////
// DPI support
typedef TMInt GXDpi;
#define TM_DPI_SYSTEM_DEFAULT      96
// const GXDips GXDIPS_EPSILON = (GXDips)0.5 / (GXDips)TM_DPI_SYSTEM_DEFAULT;

template <class T> TM_INLINE GXDips DpiDP2LP(T n, T dpi)
{
    return (GXDips)n * (GXDips)TM_DPI_SYSTEM_DEFAULT / (GXDips)dpi;
}

TM_INLINE GXInt DpiLP2DP(GXDips n, GXInt dpi)
{
    const GXDips df = (GXDips)(TM_DPI_SYSTEM_DEFAULT - 1) / (GXDips)TM_DPI_SYSTEM_DEFAULT - GXDips(0.5);
    GXDips dff = (n >= 0 ? df : -df);
    return (GXInt)(n * (GXDips)dpi / (GXDips)TM_DPI_SYSTEM_DEFAULT + dff);
}

template <class T> TM_INLINE void DpiLP2DP(T n, T dpi, float& out)
{
    DbgAssert(typeid(T) != typeid(GXInt));
    out = n * dpi / (T)TM_DPI_SYSTEM_DEFAULT;
}

template <class T> TM_INLINE void DpiLP2DP(T n, T dpi, double& out)
{
    DbgAssert(typeid(T) != typeid(GXInt));
    out = n * dpi / (T)TM_DPI_SYSTEM_DEFAULT;
}

template <class T> TM_INLINE void DpiLP2DP(T n, T dpi, GXInt& out)
{
    DbgAssert(typeid(T) != typeid(GXInt));
    double df = (T)(TM_DPI_SYSTEM_DEFAULT - 1) / (T)TM_DPI_SYSTEM_DEFAULT - T(0.5);
    double dff = (n >= 0 ? df : -df);
    out = n * (T)dpi / (T)TM_DPI_SYSTEM_DEFAULT + dff;
}

TM_INLINE int DpiLP2DPCeil(GXDips x, int dpi) { return (int)((x * dpi + TM_DPI_SYSTEM_DEFAULT - 1) / TM_DPI_SYSTEM_DEFAULT); }


TM_BGN_NAMESPACE


/////////////////////////////////////////////////////////////////////////////
// dpi

class CZoom
{
public:
    GXInt m_nDPI;

    CZoom(int n) : m_nDPI(n ? n : TM_DPI_SYSTEM_DEFAULT) {}
    CZoom() : m_nDPI(TM_DPI_SYSTEM_DEFAULT) {}

    operator GXInt () const { return m_nDPI; }
//    CZoom& operator = (int n) { DbgAssert(n); m_nDPI = n; }

    GXDips DPtoLP(GXInt n) const { return DpiDP2LP(n, m_nDPI); }
    TMInt LPtoDP(GXDips n) const { return DpiLP2DP(n, m_nDPI); }
    int LPtoDPCeil(GXDips n) const { return DpiLP2DPCeil(n, m_nDPI); }

    void DPtoLP2(GXFloat n, GXInt& out) const      { out = DpiDP2LP((GXFloat)n, (GXFloat)m_nDPI) ; }
    void DPtoLP2(GXFloat n, GXFloat& out) const    { out = DpiDP2LP((GXFloat)n, (GXFloat)m_nDPI) ; }
    void LPtoDP2(GXFloat n, GXInt& out) const      { DpiLP2DP(n, (GXFloat)m_nDPI, out) ; }
    void LPtoDP2(GXFloat n, GXFloat& out) const    { DpiLP2DP(n, (GXFloat)m_nDPI, out) ; }

    template <class T, class P> void DPtoLPPoint(const T& pt, P& ret) const
    {
        DPtoLP2(GeometryNative0c(pt), GeometryNative0(ret));
        DPtoLP2(GeometryNative1c(pt), GeometryNative1(ret));
    }

    template <class T, class P> void LPtoDPPoint(const T& pt, P& ret) const
    {
        LPtoDP2(GeometryNative0c(pt), GeometryNative0(ret));
        LPtoDP2(GeometryNative1c(pt), GeometryNative1(ret));
    }

    template <class T, class P> void DPtoLPSize(const T& sz, P& ret) const
    {
        DPtoLP2(GeometryNative0c(sz), GeometryNative0(ret));
        DPtoLP2(GeometryNative1c(sz), GeometryNative1(ret));
    }

    template <class T, class P> void LPtoDPSize(const T& sz, P& ret) const
    {
        LPtoDP2(GeometryNative0c(sz), GeometryNative0(ret));
        LPtoDP2(GeometryNative1c(sz), GeometryNative1(ret));
    }

    template <class T, class P> void DPtoLPPoint(const T* pt, int nPt, P* ret) const
    {
        for (int i = 0; i < nPt; ++i)
            DPtoLPPoint(pt[i], ret[i]);
    }

    template <class T, class P> void LPtoDPPoint(const T* pt, int nPt, P* ret) const
    {
        for (int i = 0; i < nPt; ++i)
            LPtoDPPoint(pt[i], ret[i]);
    }

    template <class T, class P> void DPtoLPRect(const T& rc, P& ret) const
    {
        RectSetLeft  (ret, DPtoLP(RectLeft(rc)));
        RectSetTop   (ret, DPtoLP(RectTop(rc)));
        RectSetRight (ret, DPtoLP(RectRight(rc)));
        RectSetBottom(ret, DPtoLP(RectBottom(rc)));
    }

    template <class T, class P> void LPtoDPRect(const T& rc, P& ret) const
    {
        RectSetLeft  (ret, LPtoDP(RectLeft(rc)));
        RectSetTop   (ret, LPtoDP(RectTop(rc)));
        RectSetRight (ret, LPtoDP(RectRight(rc)));
        RectSetBottom(ret, LPtoDP(RectBottom(rc)));
    }

    void LPtoDPCeil(const GXPoint& pt, TMPoint& pt2) const
    {
        PointSetX(pt2, LPtoDPCeil(PointX(pt)));
        PointSetY(pt2, LPtoDPCeil(PointY(pt)));
    }

    void LPtoDPCeil(const GXSize& sz, TMSize& sz2) const
    {
        SizeSetWidth(sz2, LPtoDPCeil(SizeWidth(sz)));
        SizeSetHeight(sz2, LPtoDPCeil(SizeHeight(sz)));
    }

    void LPtoDPCeil(const GXRect& rc, TMRect& rc2) const
    {
        RectSetLeft  (rc2, LPtoDP(RectLeft(rc)));
        RectSetTop   (rc2, LPtoDP(RectTop(rc)));
        RectSetRight (rc2, LPtoDPCeil(RectRight(rc)));
        RectSetBottom(rc2, LPtoDPCeil(RectBottom(rc)));
    }

    void DPtoLP(TMPointI& pt) const { DPtoLPPoint(pt, pt); }
    void DPtoLP(TMPointF& pt) const { DPtoLPPoint(pt, pt); }
    void LPtoDP(TMPointI& pt) const { LPtoDPPoint(pt, pt); }
    void LPtoDP(TMPointF& pt) const { LPtoDPPoint(pt, pt); }

    void DPtoLP(TMPointI* pt, int nPt) const { DPtoLPPoint(pt, nPt, pt); }
    void DPtoLP(TMPointF* pt, int nPt) const { DPtoLPPoint(pt, nPt, pt); }
    void LPtoDP(TMPointI* pt, int nPt) const { LPtoDPPoint(pt, nPt, pt); }
    void LPtoDP(TMPointF* pt, int nPt) const { LPtoDPPoint(pt, nPt, pt); }

    void DPtoLP(TMSizeI& sz) const { DPtoLPSize(sz, sz); }
    void DPtoLP(TMSizeF& sz) const { DPtoLPSize(sz, sz); }
    void LPtoDP(TMSizeI& sz) const { LPtoDPSize(sz, sz); }
    void LPtoDP(TMSizeF& sz) const { LPtoDPSize(sz, sz); }

    void DPtoLP(TMRectI& rc) const { DPtoLPRect(rc, rc); }
    void DPtoLP(TMRectF& rc) const { DPtoLPRect(rc, rc); }
    void LPtoDP(TMRectI& rc) const { LPtoDPRect(rc, rc); }
    void LPtoDP(TMRectF& rc) const { LPtoDPRect(rc, rc); }

    void DPtoLP(const TMPointI& pt, TMPointF& ret) const            { DPtoLPPoint(pt, ret); }
    void DPtoLP(const TMSizeI& sz, TMSizeF& ret) const              { DPtoLPSize(sz, ret); }
    void DPtoLP(const TMRectI& rc, TMRectF& ret) const              { DPtoLPRect(rc, ret); }
    void DPtoLP(const TMPointI* pt, int nPt, TMPointF* ret) const   { DPtoLPPoint(pt, nPt, ret); }
    void LPtoDP(const TMPointF& pt, TMPointI& ret) const            { LPtoDPPoint(pt, ret); }
    void LPtoDP(const TMSizeF& sz, TMSizeI& ret) const              { LPtoDPSize(sz, ret); }
    void LPtoDP(const TMRectF& rc, TMRectI& ret) const              { LPtoDPRect(rc, ret); }
    void LPtoDP(const TMPointF* pt, int nPt, TMPointI* ret) const   { LPtoDPPoint(pt, nPt, ret); }

    void DPtoLP(const TMPointF& pt, TMPointF& ret) const            { DPtoLPPoint(pt, ret); }
    void DPtoLP(const TMSizeF& sz, TMSizeF& ret) const              { DPtoLPSize(sz, ret); }
    void DPtoLP(const TMRectF& rc, TMRectF& ret) const              { DPtoLPRect(rc, ret); }
    void DPtoLP(const TMPointF* pt, int nPt, TMPointF* ret) const   { DPtoLPPoint(pt, nPt, ret); }
    void LPtoDP(const TMPointF& pt, TMPointF& ret) const            { LPtoDPPoint(pt, ret); }
    void LPtoDP(const TMSizeF& sz, TMSizeF& ret) const              { LPtoDPSize(sz, ret); }
    void LPtoDP(const TMRectF& rc, TMRectF& ret) const              { LPtoDPRect(rc, ret); }
    void LPtoDP(const TMPointF* pt, int nPt, TMPointF* ret) const   { LPtoDPPoint(pt, nPt, ret); }
};


/////////////////////////////////////////////////////////////////////////////
// IImageDpiDecide
//  use to image that support dpi

class IImageSizeDecide
{
public:
    virtual GXDips OnImageDecideSize(CSize& sz) = 0;
};

struct CImageDpiDecide : public IImageSizeDecide
{
    CZoom m_zoom;
    CImageDpiDecide(int dpi) : m_zoom(dpi) {}
    GXDips OnImageDecideSize(CSize& sz) tm_override
    {
        m_zoom.LPtoDP(sz);
        return (GXDips)m_zoom.m_nDPI / (GXDips)TM_DPI_SYSTEM_DEFAULT;
    }
};

struct CImagePSizeDecide : public IImageSizeDecide
{
    const TMSizeI* m_sz;
    CImagePSizeDecide(const TMSizeI* sz) : m_sz(sz) {}
    virtual GXDips OnImageDecideSize(CSize& sz) tm_override
    {
        if (m_sz)
        {
            GXDips s1 = (GXDips)SizeWidth(*m_sz) / (GXDips)sz.Width();
            GXDips s2 = (GXDips)SizeHeight(*m_sz) / (GXDips)sz.Height();
            GXDips scale = tm_max(s1, s2);
            sz *= scale;
            return scale;
        }
        return 1;
    }
};

struct CImageSizeDecide : public IImageSizeDecide
{
    CSizeI m_sz;
    CImageSizeDecide(const TMSizeI& sz) : m_sz(sz) {}
    CImageSizeDecide(int w, int h) : m_sz(w, h) {}
    virtual GXDips OnImageDecideSize(CSize& sz) tm_override
    {
        GXDips s1 = (GXDips)SizeWidth(m_sz) / (GXDips)sz.Width();
        GXDips s2 = (GXDips)SizeHeight(m_sz) / (GXDips)sz.Height();
        sz = m_sz;
        return tm_max(s1, s2);
    }
};


/////////////////////////////////////////////////////////////////////////////
// transformation
#define DECALRE_AFFINE_FUNC_GET(classtype,valuetype,m0,m1,m2,m3,dx,dy) \
    inline valuetype AffineM11c(const classtype& aff) { return aff.m0; } \
    inline valuetype AffineM12c(const classtype& aff) { return aff.m1; } \
    inline valuetype AffineM21c(const classtype& aff) { return aff.m2; } \
    inline valuetype AffineM22c(const classtype& aff) { return aff.m3; } \
    inline valuetype AffineDxc (const classtype& aff) { return aff.dx; } \
    inline valuetype AffineDyc (const classtype& aff) { return aff.dy; }

#define DECALRE_AFFINE_FUNC_PUT(classtype,classreal,valuetype,m0,m1,m2,m3,dx,dy) \
    inline valuetype& AffineM11(classtype& aff) { return ((classreal&)aff).m0; } \
    inline valuetype& AffineM12(classtype& aff) { return ((classreal&)aff).m1; } \
    inline valuetype& AffineM21(classtype& aff) { return ((classreal&)aff).m2; } \
    inline valuetype& AffineM22(classtype& aff) { return ((classreal&)aff).m3; } \
    inline valuetype& AffineDx (classtype& aff) { return ((classreal&)aff).dx; } \
    inline valuetype& AffineDy (classtype& aff) { return ((classreal&)aff).dy; }

#define DECALRE_AFFINE_FUNC_GET_PUT(classtype,valuetype,m0,m1,m2,m3,dx,dy) \
    DECALRE_AFFINE_FUNC_GET(classtype,valuetype,m0,m1,m2,m3,dx,dy) \
    DECALRE_AFFINE_FUNC_PUT(classtype,classtype,valuetype,m0,m1,m2,m3,dx,dy)


#ifdef TM_QT

    struct _QAffine { qreal _m11, _m12, _m21, _m22, _dx, _dy; };
    typedef QMatrix TMAffine;
    TM_TEMPLATE_NULL struct t_value_type<QMatrix>
    { typedef qreal value_type; DbgAssertS(sizeof(_QAffine) == sizeof(QMatrix)); };
    DECALRE_AFFINE_FUNC_GET(QMatrix, qreal, m11(), m12(), m21(), m22(), dx(), dy())
    DECALRE_AFFINE_FUNC_PUT(QMatrix, _QAffine, qreal, _m11, _m12, _m21, _m22, _dx, _dy)

#elif TM_APPLE

    typedef CGAffineTransform TMAffine;
    TM_TEMPLATE_NULL struct t_value_type<CGAffineTransform> { typedef CGFloat value_type; };
    DECALRE_AFFINE_FUNC_GET_PUT(CGAffineTransform, CGFloat, a, b, c, d, tx, ty)

#elif TM_GTK3
        
    typedef cairo_matrix_t TMAffine;
    TM_TEMPLATE_NULL struct t_value_type<cairo_matrix_t> { typedef double value_type; };
    DECALRE_AFFINE_FUNC_GET_PUT(cairo_matrix_t, double, xx, yx, xy, yy, x0, y0)

#else

#   ifndef TM_WIN
        typedef struct tagXFORM { float eM11, eM12, eM21, eM22, eDx, eDy; } XFORM;
#   endif
    typedef XFORM TMAffine;
    TM_TEMPLATE_NULL struct t_value_type<XFORM> { typedef float value_type; };
    DECALRE_AFFINE_FUNC_GET_PUT(XFORM, float, eM11, eM12, eM21, eM22, eDx, eDy)

#endif // TM_QT

#undef DECALRE_AFFINE_FUNC_GET
#undef DECALRE_AFFINE_FUNC_PUT
#undef DECALRE_AFFINE_FUNC_GET_PUT


template <class T_Base>
struct CAffineT : public T_Base
{
public:
    typedef t_value_type<T_Base>            traits;
    typedef T_Base                          base_class;
    typedef typename traits::value_type     value_type;
    typedef base_class                      class_type;

    value_type& M11()       { return AffineM11(*this); }
    value_type M11() const  { return AffineM11c(*this); }
    value_type& M12()       { return AffineM12(*this); }
    value_type M12() const  { return AffineM12c(*this); }
    value_type& M21()       { return AffineM21(*this); }
    value_type M21() const  { return AffineM21c(*this); }
    value_type& M22()       { return AffineM22(*this); }
    value_type M22() const  { return AffineM22c(*this); }
    value_type& Dx()        { return AffineDx(*this); }
    value_type Dx() const   { return AffineDxc(*this); }
    value_type& Dy()        { return AffineDy(*this); }
    value_type Dy() const   { return AffineDyc(*this); }

private:
    inline static void _SetPoint(TMPointF& pt, float x, float y)
    {
        PointSetX(pt, x);
        PointSetY(pt, y);
    }

    inline static void _SetPoint(TMPointF& pt, int x, int y)
    {
        PointSetX(pt, x);
        PointSetY(pt, y);
    }

    inline static void _SetPoint(TMPointI& pt, float x, float y)
    {
        PointSetX(pt, x > 0 ? x + 0.5f : x - 0.5f);
        PointSetY(pt, y > 0 ? y + 0.5f : y - 0.5f);
    }

    inline static void _SetPoint(TMPointI& pt, int x, int y)
    {
        PointSetX(pt, x);
        PointSetY(pt, y);
    }

    template <class pt_1, class pt_2>
    void _DPtoLP(const pt_1* pt, int nPt, pt_2* ret) const
    {
        CAffineT aff(*this);
        aff.Invert();
        aff._LPtoDP(pt, nPt, ret);
    }

    template <class pt_1, class pt_2>
    void _LPtoDP_Pt(const pt_1& pt, pt_2& ret) const
    {
        GXDips x = PointX(pt);
        GXDips y = PointY(pt);
        GXDips xx = MapX(x, y);// x * M11() + y * M21() + Dx();
        GXDips yy = MapY(x, y);// x * M12() + y * M22() + Dy();
        _SetPoint(ret, xx, yy);
    }

    template <class pt_1, class pt_2>
    void _LPtoDP(const pt_1* pt, int nPt, pt_2* ret) const
    {
        for (int i = 0; i < nPt; ++i)
            _LPtoDP_Pt(pt[i], ret[i]);
    }

    template<class sz_1, class sz_2> void _MapSize(const sz_1& sz, sz_2& ret) const
    {
        value_type n1 = SizeWidth(sz);
        value_type n2 = SizeHeight(sz);
        value_type m1 = MapW(n1, n2);
        value_type m2 = MapH(n1, n2);
        SizeSetWidth(ret, m1);
        SizeSetHeight(ret, m2);
    }

public:
    CAffineT(value_type m1, value_type m2, value_type m3, value_type m4, value_type dx, value_type dy)
    {
        M11() = m1;
        M12() = m2;
        M21() = m3;
        M22() = m4;
        Dx()  = dx;
        Dy()  = dy;
        Check();
    }

    CAffineT(const class_type& v)
    {
        *this = v;
        Check();
    }

    CAffineT()
    {
//         Reset();
    }

    void Check() const
    {
//         DbgAssert(M11() * M22() != M12() * M21());
    }

    // init function
    void Reset()
    {
        M11() = 1;
        M12() = 0;
        M21() = 0;
        M22() = 1;
        Dx()  = 0;
        Dy()  = 0;
    }

    void Set(value_type m1, value_type m2, value_type m3, value_type m4, value_type dx, value_type dy)
    {
        M11() = m1;
        M12() = m2;
        M21() = m3;
        M22() = m4;
        Dx()  = dx;
        Dy()  = dy;
        Check();
    }

    void SetTranslate(value_type tx, value_type ty)
    {
        Set(1.0f, 0.0f, 0.0f, 1.0f, tx, ty);
    }

    void SetScale(value_type sx, value_type sy)
    {
        Set(sx, 0.0f, 0.0f, sy, 0.0f, 0.0f);
    }

    void SetSkewXR(value_type fRadian)
    {
        Set(1.0f, 0.0f, tan(fRadian), 1.0f, 0.0f, 0.0f);
    }

    void SetSkewYR(value_type fRadian)
    {
        Set(1.0f, tan(fRadian), 0.0f, 1.0f, 0.0f, 0.0f);
    }

    void SetSkewX(value_type fAngle)
    {
        SetSkewXR(fAngle * CDouble::PI() / 180.);
    }

    void SetSkewY(value_type fAngle)
    {
        SetSkewYR(fAngle * CDouble::PI() / 180.);
    }

    void SetRotateR(value_type fRadian)
    {
        value_type cs = cos(fRadian), sn = sin(fRadian);
        Set(cs, sn, -sn, cs, 0.0f, 0.0f);
    }

    void SetRotate(value_type fAngle)
    {
        SetRotateR(fAngle * CDouble::PI() / 180.);
    }

    //   *---------------*                  *---------------*
    //   |         2(1,1)|      map        /        (x2,y2)/
    //   |               |     ----->     /               /
    //   |0(0,0)   1(0,1)|               /(x0,y0) (x1,y1)/
    //   *---------------*              *---------------*
    void SetMapTri(value_type x0, value_type y0,
        value_type x1, value_type y1, value_type x2, value_type y2)
    {
        Set(x1 - x0, y1 - y0, x2 - x0, y2 - y0, x0, y0);
    }

    // don't use left,bottom to x0,y0 
    //    (x0,y0) \             map           (x2,y2)  \
    //      /      (x2,y2)     ----->            \      (x1,y1)
    //  (x1,y1)   /                             (x0,y0)   /
    void SetMapTri(
        value_type px0, value_type py0,
        value_type qx0, value_type qy0,
        value_type rx0, value_type ry0,
        value_type px1, value_type py1,
        value_type qx1, value_type qy1,
        value_type rx1, value_type ry1)
    {
        SetMapTri(px0, py0, qx0, qy0, rx0, ry0);
        Invert();        // transform p0, q0, and r0 back to (0,0),(1,0),(0,1)

        CAffineT _tmp;
        _tmp.SetMapTri(px1, py1, qx1, qy1, rx1, ry1);

        Combine(_tmp);
    }

    //                               *---------------*
    //   *----------*      map       |               |
    //   |   rc0    |     ----->     |      rc1      |
    //   *----------*                |               |
    //                               *---------------*
    void SetMapRect(const TMRectF& rc0, const TMRectF& rc1)
    {
        SetMapTri(
            RectLeft(rc0), RectTop(rc0),
            RectLeft(rc0), RectBottom(rc0),
            RectRight(rc0), RectTop(rc0),
            RectLeft(rc1), RectTop(rc1),
            RectLeft(rc1), RectBottom(rc1),
            RectRight(rc1), RectTop(rc1));
    }

    bool IsEmpty() const
    {
        return M11() == 1 && M12() == 0 && M21() == 0 &&
            M22() == 1 && Dx() == 0 && Dy() == 0;
    }

    CAffineT& operator = (const class_type& v)
    {
        ((class_type&)*this) = v;
        return *this;
    }

    CAffineT& operator *= (const base_class& rhs)
    {
        Combine (rhs);
        return *this;
    }

    CAffineT operator * (const base_class& rhs) const
    {
        return CAffineT(*this).operator *= (rhs);
    }

    value_type MapW(value_type x, value_type y) const
    {
        return M11() * x + M21() * y;
    }

    value_type MapH(value_type x, value_type y) const
    {
        return M12() * x + M22() * y;
    }

    value_type MapX(value_type x, value_type y) const
    {
        return MapW(x, y) + Dx();
    }

    value_type MapY(value_type x, value_type y) const
    {
        return MapH(x, y) + Dy();
    }

    value_type MapRad(value_type rad)
    {
        value_type x = cos(rad);
        value_type y = sin(rad);
        value_type newX = MapW(x, y);
        value_type newY = MapH(x, y);
        value_type newRad = atan2(newY, newX);
        return newRad;
    }

    // this function like LP2DP
    // T: TMRectI or TMRectF
    template<class T> void MapRect(T& rc)
    {
        value_type n1 = RectLeft(rc);
        value_type n2 = RectTop(rc);
        value_type n3 = RectRight(rc);
        value_type n4 = RectBottom(rc);
        RectSetLeft  (rc, MapX(n1, n2));
        RectSetTop   (rc, MapY(n1, n2));
        RectSetRight (rc, MapX(n3, n4));
        RectSetBottom(rc, MapY(n3, n4));
    }

    // {old0,y,old1,y} -> {new0,y,new1,y}; newW / oldW = scale
    void MapH(value_type old0, value_type new0, value_type scale)
    {
        TranslateX(-new0);
        ScaleX(scale);
        TranslateX(old0);
    }

    // {x, old0,x,old1} -> {x,new0,x, new1}; newH / oldH = scale
    void MapV(value_type old0, value_type new0, value_type scale)
    {
        TranslateY(-new0);
        ScaleY(scale);
        TranslateY(old0);
    }

    // this function like LP2DP
    // T: TMPointI or TMPointF
    template<class T> void MapPoint(T& pt)
    {
        value_type n1 = PointX(pt);
        value_type n2 = PointY(pt);
        PointSetX(pt, MapX(n1, n2));
        PointSetY(pt, MapY(n1, n2));
    }

    // this function like LP2DP
    // T: TMSizeI or TMSizeF
    template<class T> void MapSize(T& sz)
    {
        _MapSize(sz, sz);
    }

    void MapSize(const TMSizeF& sz, TMSizeF& ret)
    {
        _MapSize(sz, ret);
    }

    void MapSize(const TMSizeF& sz, TMSizeI& ret)
    {
        _MapSize(sz, ret);
    }

    // *this *= rhs
    void Combine(const CAffineT& rhs)
    {
        rhs.Check();

        value_type _t1 = M11();
        M11() = _t1 * rhs.M11() + M12() * rhs.M21();
        M12() = _t1 * rhs.M12() + M12() * rhs.M22();

        value_type _t2 = M21();
        M21() = _t2 * rhs.M11() + M22() * rhs.M21();
        M22() = _t2 * rhs.M12() + M22() * rhs.M22();

        value_type _t3 = Dx();
        Dx()  = _t3 * rhs.M11() + Dy() * rhs.M21() + rhs.Dx();
        Dy()  = _t3 * rhs.M12() + Dy() * rhs.M22() + rhs.Dy();
    }

    void PreCombine(CAffineT rhs)
    {
        rhs.Check();
        rhs.Combine(*this);
        *this = rhs;
    }

    // *this = 1 / *this
    void Invert()
    {
        value_type det = M11() * M22() - M21() * M12();
        if (det > -1e-6 && det < 1e-6) {
            Reset();;
            return;
        }

        value_type _tmp;

        _tmp = M11();
        M11() = M22() / det;
        M22() = _tmp / det;
        M12() = - M12() / det;
        M21() = - M21() / det;

        _tmp = Dx();
        Dx()  = - (M11() * _tmp + M21() * Dy());
        Dy()  = - (M12() * _tmp + M22() * Dy());
    }

    void TranslateX(value_type dx)
    {
        Dx() += dx;
    }

    void TranslateY(value_type dy)
    {
        Dy() += dy;
    }

    void Translate(value_type dx, value_type dy)
    {
        Dx() += dx;
        Dy() += dy;
    }

    void TranslateToX(value_type x)
    {
        Dx() = x;
    }

    void TranslateToY(value_type y)
    {
        Dy() = y;
    }

    void TranslateTo(value_type x, value_type y)
    {
        Dx() = x;
        Dy() = y;
    }

    void ScaleX(value_type sx)
    {
        M11() *= sx;
        M12() *= sx;
        Dx()  *= sx;
    }

    void ScaleY(value_type sy)
    {
        M21() *= sy;
        M22() *= sy;
        Dy()  *= sy;
    }

    void Scale(value_type sx, value_type sy)
    {
        ScaleX(sx);
        ScaleY(sy);
    }

    value_type GetScale() const
    {
        value_type x = 0.707106781 * M11()  + 0.707106781 * M21();
        value_type y = 0.707106781 * M12() + 0.707106781 * M22();
        return sqrt(x*x + y*y);
    }

    value_type GetAverageScale()
    {
        float sx = sqrt(M11()*M11() + M21()*M21());
        float sy = sqrt(M12()*M12() + M22()*M22());
        return (sx + sy) * 0.5;
    }

    void RotateR(value_type fRadian)
    {
        CAffineT xm;
        xm.M11() =   (value_type)cos(fRadian);
        xm.M12() =   (value_type)sin(fRadian);
        xm.M21() = - xm.M12();
        xm.M22() =   xm.M11();
        xm.Dx()  = 0;
        xm.Dy()  = 0;

        Combine(xm);
    }

    value_type GetRotateR()
    {
        return atan2(M12(), M11());
    }

    void Rotate(value_type fAngle)
    {
        RotateR(fAngle * CDouble::PI() / 180.);
    }

    // fx, fy: origin
    void RotateR(value_type fRadian, value_type fx, value_type fy)
    {
        Translate(-fx, -fy);
        RotateR(fRadian);
        Translate(fx, fy);
    }

    // fx, fy: origin
    void Rotate(value_type fAngle, value_type fx, value_type fy)
    {
        Translate(-fx, -fy);
        Rotate(fAngle);
        Translate(fx, fy);
    }

    //(fX, 0) <-> (0, 0); fX = OriginX * 2;
    void FlipX(value_type fX)
    {
        M11() = -M11();
        M12() = -M12();
        Dx()  = fX - Dx();
    }

    //(0, fY) <-> (0, 0); fY = OriginY * 2;
    void FlipY(value_type fY)
    {
        M21() = -M21();
        M22() = -M22();
        Dy()  = fY - Dy();
    }

    value_type Determinant() const
    {
        return M11() * M22() - M12() * M21();
    }

    void DPtoLP(const TMPointI* pt, int nPt, TMPointF* ret) const
    {
        _DPtoLP(pt, nPt, ret);
    }

    void DPtoLP(const TMPointF* pt, int nPt, TMPointF* ret) const
    {
        _DPtoLP(pt, nPt, ret);
    }

    void LPtoDP(const TMPointF* pt, int nPt, TMPointI* ret) const
    {
        _LPtoDP(pt, nPt, ret);
    }

    void LPtoDP(const TMPointF* pt, int nPt, TMPointF* ret) const
    {
        _LPtoDP(pt, nPt, ret);
    }

    void DPtoLP(const TMRectI& rc, TMRectF& ret) const  { DPtoLP((TMPointI*)&rc, 2, (TMPointF*)&ret); }
    void DPtoLP(const TMRectF& rc, TMRectF& ret) const  { DPtoLP((TMPointF*)&rc, 2, (TMPointF*)&ret); }
    void LPtoDP(const TMRectF& rc, TMRectI& ret) const  { LPtoDP((TMPointF*)&rc, 2, (TMPointI*)&ret); }
    void LPtoDP(const TMRectF& rc, TMRectF& ret) const  { LPtoDP((TMPointF*)&rc, 2, (TMPointF*)&ret); }

    void LPtoDP(const TMPointF& pt, TMPointI& ret) const  { _LPtoDP_Pt(pt, ret); }
    void LPtoDP(const TMPointF& pt, TMPointF& ret) const  { _LPtoDP_Pt(pt, ret); }
};

typedef CAffineT<TMAffine> CAffine;


/////////////////////////////////////////////////////////////////////////////
// CBorder
struct CBorder
{
    typedef TMInt16 value_type;
    TMInt16 left;   // left side width
    TMInt16 top;    // top width
    TMInt16 right;  // right side width
    TMInt16 bottom; // bottom width

    CBorder (TMInt16 l = 0, TMInt16 t = 0, TMInt16 r = 0, TMInt16 b = 0)
        : left (l)
        , top (t)
        , right (r)
        , bottom (b)
    {
    }

    bool operator == (const CBorder& rhs) const
    {
        return left == rhs.left && right == rhs.right && top == rhs.top && bottom == rhs.bottom;
    }

    void Assign(const CRectF& rc)
    {
        left = rc.Left();
        right = rc.Right();
        top = rc.Top();
        bottom = rc.Bottom();
    }

    bool SafeAssign(const CRectF* prc)
    {
        if (!prc)
            return false;
        CBorder bdTemp;
        bdTemp.Assign(*prc);
        if (bdTemp == *this)
            return false;
        *this = bdTemp;
        return true;
    }

    void CopyTo(CRectF* prc)
    {
        if (prc)
            prc->SetRect(left, top, right, bottom);
    }
};


/////////////////////////////////////////////////////////////////////////////
// CBltSharp
struct CBltSharp : public CBorder
{
public:
    enum EValid
    {
        e_lt = 0x01,
        e_wt = 0x01 << 1,
        e_rt = 0x01 << 2,
        e_lh = 0x01 << 3,
        e_lb = 0x01 << 4,
        e_wb = 0x01 << 5,
        e_rh = 0x01 << 6,
        e_rb = 0x01 << 7,
        e_wh = 0x01 << 8,

//         e_tp = 0x01 << 10, // is transparent
//
        e_l  = e_lt | e_lh | e_lb,
        e_w  = e_wt | e_wh | e_wb,
        e_r  = e_rt | e_rh | e_rb,
        e_t  = e_lt | e_wt | e_rt,
        e_h  = e_lh | e_wh | e_rh,
        e_b  = e_lb | e_wb | e_rb,

        e_all = e_l | e_w | e_r,
    };

public:
    TMUInt32    m_mask;

public:
    CBltSharp(TMUInt32 mask = e_all)
        : m_mask(mask)
    {}

    CBltSharp(const CBorder& rhs, TMUInt32 mask)
        : CBorder(rhs)
        , m_mask(mask)
    {
    }

    // dsts: CRectL[10]*; srcs: CRectD[9]*;
    int MakeBltsParams(CRectL* dsts, CRectD* srcs, const CRectL& rcDst, const CRectD& rcSrc, float scaleSW = 1.0f, float scaleSH = 1.0f)
    {
        // cacl dest rect
        GXDips dl1 = rcDst.Left();
        GXDips dw1 = this->left;
        GXDips dw3 = this->right;
        GXDips dl3 = rcDst.Right() - this->right;
        GXDips dl2 = rcDst.Left() + this->left;
        GXDips dw2 = dl3 - dl2;
        
        GXDips dt1 = rcDst.Top();
        GXDips dh1 = this->top;
        GXDips dh3 = this->bottom; 
        GXDips dt3 = rcDst.Bottom() - this->bottom;
        GXDips dt2 = rcDst.Top() + this->top;
        GXDips dh2 = dt3 - dt2;

        // calc source rect
        int sw1 = (float)this->left * scaleSW;
        int sh1 = (float)this->top * scaleSH;
        int sw3 = (float)this->right * scaleSW;
        int sh3 = (float)this->bottom * scaleSH;
        int sl1 = rcSrc.Left();
        int sl3 = rcSrc.Right() - sw3;
        int sl2 = rcSrc.Left() + sw1;
        int sw2 = sl3 - sl2;
        
        int st1 = rcSrc.Top();
        int st3 = rcSrc.Bottom() - sh3;
        int st2 = rcSrc.Top() + sh1;
        int sh2 = st3 - st2;

        int n = 0;
#   define _SBLT(mask, dx, dy, dw, dh, sx, sy, sw, sh) \
        if ((m_mask & mask) && dw && dh) dsts[n].SetLTWH(dx,dy,dw,dh), srcs[n].SetLTWH(sx,sy,sw,sh), ++n

        _SBLT(e_lt, dl1, dt1, dw1, dh1,  sl1, st1, sw1, sh1); // left-top
        _SBLT(e_lb, dl1, dt3, dw1, dh3,  sl1, st3, sw1, sh3); // left-bottom
        _SBLT(e_rt, dl3, dt1, dw3, dh1,  sl3, st1, sw3, sh1); // right-top
        _SBLT(e_rb, dl3, dt3, dw3, dh3,  sl3, st3, sw3, sh3); // right-bottom
        
        _SBLT(e_wt, dl2, dt1, dw2, dh1,  sl2, st1, sw2, sh1); // top line
        _SBLT(e_wb, dl2, dt3, dw2, dh3,  sl2, st3, sw2, sh3); // bottom line
        
        _SBLT(e_lh, dl1, dt2, dw1, dh2,  sl1, st2, sw1, sh2); // left line
        _SBLT(e_rh, dl3, dt2, dw3, dh2,  sl3, st2, sw3, sh2); // right line
        
        _SBLT(e_wh, dl2, dt2, dw2, dh2,  sl2, st2, sw2, sh2); // center
#   undef _SBLT

        return n;
    }
};


TM_END_NAMESPACE


#define TM_FMT_RCI_STR      "%d,%d,%d,%d(%d,%d)"
#define TM_FMT_RCI_VAL(x)   RectLeft(x), RectTop(x), RectRight(x), RectBottom(x), RectWidth(x), RectHeight(x)
#define TM_FMT_RCF_STR      "%g,%g,%g,%g(%g,%g)"
#define TM_FMT_RCF_VAL(x)   TM_FMT_RCI_VAL(x)




#endif // #ifdef TMGDI_H

