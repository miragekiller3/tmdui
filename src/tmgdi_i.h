/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.8.22
 *
 *  Copyright (C) 2025 miragekiller
 */

#if !defined(t_int) || !defined(t_rc) || !defined(t_pt) || !defined(t_sz)
#   error the file must include from "tmgdi.h"
#endif

#ifdef QT_DX
#   undef QT_DX
#   define QT_DX 1
#else
#   define QT_DX 0
#endif // QT_DX

#ifdef bLTWH
#   undef bLTWH
#   define bLTWH 1
#else
#   define bLTWH 0
#endif

// rect
TM_INLINE t_int& GeometryNative0(t_rc& rc)          { return ((t_int*)&rc)[0]; }
TM_INLINE t_int& GeometryNative1(t_rc& rc)          { return ((t_int*)&rc)[1]; }
TM_INLINE t_int  GeometryNative0c(const t_rc& rc)   { return ((t_int*)&rc)[0]; }
TM_INLINE t_int  GeometryNative1c(const t_rc& rc)   { return ((t_int*)&rc)[1]; }
TM_INLINE t_int& RectNative2(t_rc& rc)              { return ((t_int*)&rc)[2]; }
TM_INLINE t_int& RectNative3(t_rc& rc)              { return ((t_int*)&rc)[3]; }
TM_INLINE t_int  RectNative2c(const t_rc& rc)       { return ((t_int*)&rc)[2]; }
TM_INLINE t_int  RectNative3c(const t_rc& rc)       { return ((t_int*)&rc)[3]; }

TM_INLINE void   RectSetLeft(t_rc& rc, t_int l)     { GeometryNative0(rc) = l; }
TM_INLINE void   RectSetTop(t_rc& rc, t_int t)      { GeometryNative1(rc) = t; }
TM_INLINE t_int  RectLeft(const t_rc& rc)           { return GeometryNative0c(rc); }
TM_INLINE t_int  RectTop(const t_rc& rc)            { return GeometryNative1c(rc); }

// QRect: width=right-left+1; RECT: width=right-left;
#if bLTWH
TM_INLINE void   RectSetRight(t_rc& rc, t_int r)    { RectNative2(rc) = r - GeometryNative0c(rc); }
TM_INLINE void   RectSetBottom(t_rc& rc, t_int b)   { RectNative3(rc) = b - GeometryNative1c(rc); }
TM_INLINE void   RectSetWidth(t_rc& rc, t_int w)    { RectNative2(rc) = w; }
TM_INLINE void   RectSetHeight(t_rc& rc, t_int h)   { RectNative3(rc) = h; }
TM_INLINE t_int  RectRight(const t_rc& rc)          { return GeometryNative0c(rc) + RectNative2c(rc); }
TM_INLINE t_int  RectBottom(const t_rc& rc)         { return GeometryNative1c(rc) + RectNative3c(rc); }
TM_INLINE t_int  RectWidth(const t_rc& rc)          { return RectNative2c(rc); }
TM_INLINE t_int  RectHeight(const t_rc& rc)         { return RectNative3c(rc); }
#else
TM_INLINE void   RectSetRight(t_rc& rc, t_int r)    { RectNative2(rc) = r - QT_DX; }
TM_INLINE void   RectSetBottom(t_rc& rc, t_int b)   { RectNative3(rc) = b - QT_DX; }
TM_INLINE void   RectSetWidth(t_rc& rc, t_int w)    { RectNative2(rc) = GeometryNative0c(rc) + w - QT_DX; }
TM_INLINE void   RectSetHeight(t_rc& rc, t_int h)   { RectNative3(rc) = GeometryNative1c(rc) + h - QT_DX; }
TM_INLINE t_int  RectRight(const t_rc& rc)          { return RectNative2c(rc) + QT_DX; }
TM_INLINE t_int  RectBottom(const t_rc& rc)         { return RectNative3c(rc) + QT_DX; }
TM_INLINE t_int  RectWidth(const t_rc& rc)          { return RectRight(rc) - RectLeft(rc); }
TM_INLINE t_int  RectHeight(const t_rc& rc)         { return RectBottom(rc) - RectTop(rc); }
#endif // bLTWH

// pointer
TM_INLINE t_int& GeometryNative0(t_pt& pt)          { return ((t_int*)&pt)[0]; }
TM_INLINE t_int& GeometryNative1(t_pt& pt)          { return ((t_int*)&pt)[1]; }
TM_INLINE t_int  GeometryNative0c(const t_pt& pt)   { return ((t_int*)&pt)[0]; }
TM_INLINE t_int  GeometryNative1c(const t_pt& pt)   { return ((t_int*)&pt)[1]; }
TM_INLINE void   PointSetX(t_pt& pt, t_int x)       { GeometryNative0(pt) = x; }
TM_INLINE void   PointSetY(t_pt& pt, t_int y)       { GeometryNative1(pt) = y; }
TM_INLINE t_int  PointX(const t_pt& pt)             { return GeometryNative0c(pt); }
TM_INLINE t_int  PointY(const t_pt& pt)             { return GeometryNative1c(pt); }

// size
TM_INLINE t_int& GeometryNative0(t_sz& sz)          { return ((t_int*)&sz)[0]; }
TM_INLINE t_int& GeometryNative1(t_sz& sz)          { return ((t_int*)&sz)[1]; }
TM_INLINE t_int  GeometryNative0c(const t_sz& sz)   { return ((t_int*)&sz)[0]; }
TM_INLINE t_int  GeometryNative1c(const t_sz& sz)   { return ((t_int*)&sz)[1]; }
TM_INLINE void   SizeSetWidth(t_sz& sz, t_int w)    { GeometryNative0(sz) = w; }
TM_INLINE void   SizeSetHeight(t_sz& sz, t_int h)   { GeometryNative1(sz) = h; }
TM_INLINE t_int  SizeWidth(const t_sz& sz)          { return GeometryNative0c(sz); }
TM_INLINE t_int  SizeHeight(const t_sz& sz)         { return GeometryNative1c(sz); }


// traits
TM_BGN_NAMESPACE
TM_TEMPLATE_NULL struct t_value_type<t_rc> : public _rect_traits<t_int, t_rc, t_pt, t_sz, !bLTWH> {};
TM_TEMPLATE_NULL struct t_value_type<t_pt> : public _rect_traits<t_int, t_rc, t_pt, t_sz, !bLTWH> {};
TM_TEMPLATE_NULL struct t_value_type<t_sz> : public _rect_traits<t_int, t_rc, t_pt, t_sz, !bLTWH> {};
TM_END_NAMESPACE


// undefine
#undef t_int
#undef t_rc
#undef t_pt
#undef t_sz
#undef bLTWH
#undef QT_DX

