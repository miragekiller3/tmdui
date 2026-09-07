/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.10.24
 *
 *  Copyright (C) 2025 miragekiller
 */

// #define TYPEMODE_HLSL_VS     // the file include by vs.hlsl
// #define TYPEMODE_HLSL_PS     // the file include by ps.hlsl
// #define TYPEMODE_HLSL4_VS    // the file include by 4.0 vs.hlsl
// #define TYPEMODE_HLSL4_PS    // the file include by 4.0 ps.hlsl
// #define TYPEMODE_C_VERTEX    // the file include by h/cpp, defined vertex
// #define TYPEMODE_C_DECL9     // the file include by h/cpp, defined D3DVERTEXELEMENTX, for CreateVertexDeclaration
// #define TYPEMODE_C_DECL11    // the file include by h/cpp, defined D3D11_INPUT_ELEMENT_DESC, for CreateInputLayout
// sample:
// a.hlsl:
//  #define TYPEMODE_HLSL_VS
//  #include "atgdixsl.h"
//  #include "gxvs_i.h"
// b.cpp:
//  #define TYPEMODE_C_VERTEX
//  #include "atgdixsl.h"
//  #include "gxvs_i.h"
//  #undef  TYPEMODE_C_VERTEX

#if !defined(TYPEMODE_HLSL_VS)      && \
    !defined(TYPEMODE_HLSL_PS)      && \
    !defined(TYPEMODE_HLSL4_VS)     && \
    !defined(TYPEMODE_HLSL4_PS)     && \
    !defined(TYPEMODE_C_VERTEX)     && \
    !defined(TYPEMODE_C_DECL9)      && \
    !defined(TYPEMODE_C_DECL11)
#   error "must define TYPEMODE_XXX"
#endif


///////////////////////////////////////////////////////////////////////////////
// types
#ifdef TYPEMODE_C_VERTEX
#ifndef VERTEX_HELPER
#define VERTEX_HELPER

struct float4 { float x, y, z, w; };
struct float3 { float x, y, z; };
struct float2 { float x, y; };
struct short2 { short x, y; };
struct short4 { short x, y, z, w; };
struct int2   { int x, y; };
struct int4   { int x, y, z, w; };
typedef short half;
struct half4  { half x, y, z, w; };

#ifdef D3DMATRIX_DEFINED
    typedef D3DMATRIX float4x4;
#else
    struct float4x4
    {
        union {
            struct {
                float        _11, _12, _13, _14;
                float        _21, _22, _23, _24;
                float        _31, _32, _33, _34;
                float        _41, _42, _43, _44;
            
            };
            float m[4][4];
        };
    };
#endif
typedef float4x4 matrix;

inline tm_constexpr11 float2 _float2(float x, float y)
{
    float2 tmp = { x, y };
    return tmp;
}

inline float2 normalize(const float2& v)
{
    float len = sqrtf(v.x * v.x + v.y * v.y);
    float2 ret;
    if (len > 0.0001f)
    {
        ret.x = v.x / len;
        ret.y = v.y / len;
    }
    else
    {
        ret.x = 1;
        ret.y = 0;
    }
    return ret;
}

inline tm_constexpr11 float dot(const float2& a, const float2& b)
{
    return a.x * b.x + a.y * b.y;
}

inline tm_constexpr11 float2 perpendicular(const float2& v)
{
    float2 ret = { -v.y, v.x };
    return ret;
}

tm_constexpr20 inline float length(const float2& v)
{
    return sqrtf(dot(v, v));
}

inline tm_constexpr11 bool operator == (const float2& a, const float2& b)
{
    return a.x == b.x && a.y == b.y;
}

inline tm_constexpr11 bool operator != (const float2& a, const float2& b)
{
    return a.x != b.x || a.y != b.y;
}

inline tm_constexpr11 float2 operator - (const float2& a, const float2& b)
{
    float2 ret = { a.x - b.x, a.y - b.y };
    return ret;
}

inline tm_constexpr11 float2 operator + (const float2& a, const float2& b)
{
    float2 ret = { a.x + b.x, a.y + b.y };
    return ret;
}

inline tm_constexpr11 float2 operator * (float s, const float2& a)
{
    float2 ret = { s * a.x, s * a.y };
    return ret;
}

inline tm_constexpr11 float2 operator * (const float2& a, float s)
{
    float2 ret = { s * a.x, s * a.y };
    return ret;
}

tm_constexpr20 inline float distance(const float2& a, const float2& b)
{
    return length(b - a);
}

template <class T> inline tm_constexpr11 T lerp(T a, T b, float t)
{
    return a + t * (b - a);
}

inline float frac(float a) // 3.24259 -> 0.24259
{
    double n = 0;
    modf(a, &n);
    return n;
}

inline tm_constexpr11 float2 frac(float2 a) // 3.24259 -> 0.24259
{
    return _float2(frac(a.x), frac(a.y));
}

#define fract frac // GLSL

#if TM_CPP < TM_CPP11
inline float trunc(float a) // 3.24259 -> 3; -2.3 -> -2;
{
    return (int)a;
}
#endif // TM_CPP < TM_CPP11

inline tm_constexpr11 float clamp(float a, float _min, float _max)
{
    if (a < _min)
        a = _min;
    else if (a > _max)
        a = _max;
    return a;
}

inline tm_constexpr11 float saturate(float a)
{
    return clamp(a, 0.0f, 1.0f);
}

inline tm_constexpr11 float rcp(float a)
{
    return 1.0f / a;
}

inline tm_constexpr20 float asfloat(TMUInt32 a)
{
    union S
    {
        float f;
        TMUInt32 i;
    };
    S x;
    x.i = a;
    return x.f;
}

inline tm_constexpr20 int asint(float a)
{
    union S
    {
        float f;
        int i;
    };
    S x;
    x.f = a;
    return x.i;
}

inline tm_constexpr20 unsigned int asuint(float a)
{
    union S
    {
        float f;
        unsigned int i;
    };
    S x;
    x.f = a;
    return x.i;
}

#endif // VERTEX_HELPER
#endif // TYPEMODE_C_VERTEX


///////////////////////////////////////////////////////////////////////////////
// vertex decl helper
#ifdef TYPEMODE_C_DECL9

#ifndef TYPEMODE_C_DECL9_HELPER_
#define TYPEMODE_C_DECL9_HELPER_

template<class T> struct T2DECLTYPE;
template<> struct T2DECLTYPE<float2> { enum { n = D3DDECLTYPE_FLOAT2 }; };
template<> struct T2DECLTYPE<float3> { enum { n = D3DDECLTYPE_FLOAT3 }; };
template<> struct T2DECLTYPE<float4> { enum { n = D3DDECLTYPE_FLOAT4 }; };
template<> struct T2DECLTYPE<short2> { enum { n = D3DDECLTYPE_SHORT2 }; };
template<> struct T2DECLTYPE<short4> { enum { n = D3DDECLTYPE_SHORT4 }; };

#endif // TYPEMODE_C_DECL9_HELPER_
#endif // TYPEMODE_C_DECL9

#ifdef TYPEMODE_C_DECL11
#ifndef TYPEMODE_C_DECL11_HELPER_
#define TYPEMODE_C_DECL11_HELPER_

template<class T> struct T2DXGIFMT;
template<> struct T2DXGIFMT<float2> { enum { n = DXGI_FORMAT_R32G32_FLOAT }; };
template<> struct T2DXGIFMT<float3> { enum { n = DXGI_FORMAT_R32G32B32_FLOAT }; };
template<> struct T2DXGIFMT<float4> { enum { n = DXGI_FORMAT_R32G32B32A32_FLOAT }; };

#endif // TYPEMODE_C_DECL11_HELPER_
#endif // TYPEMODE_C_DECL11



///////////////////////////////////////////////////////////////////////////////
// VS_INPUT_XXX
// sample: 
// gxvs_i.h:
//   VS_INPUT_BGN              (VS_INPUT1)
//     VS_INPUT_ITEM           (float3, Pos,   POSITION, 0)
//     VS_INPUT_ITEM           (float4, Dif,   COLOR,    0)
//     VS_INPUT_ITEM           (float2, Tex,   TEXCOORD, 1)
//   VS_INPUT_END 
// => vertex shader:
//   struct VS_INPUT1 { float3 Pos : POSITION0; float4 Dif : COLOR0; float2 Tex : TEXCOORD0; }
// => c++:
//   struct VS_INPUT1 { float3 Pos; float4 Dif; float2 Tex; }
// => input layout d3d11:
//   const D3D11_INPUT_ELEMENT_DESC layout_VS_INPUT1::p[] = {
//     { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
//     { "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//     { "TEXCOORD",  1, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//   };
// => input layout d3d9:
//   const D3DVERTEXELEMENT9 layout_VS_INPUT1::p[] = {
//     {0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
//     {0, 12, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0},
//     {0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
//     D3DDECL_END()
//   };

#undef VS_INPUT_BGN
#undef VS_INPUT_ITEM
#undef VS_INPUT_END


#if defined(TYPEMODE_HLSL_VS) || defined(TYPEMODE_HLSL4_VS)
#   define VS_INPUT_BGN(cls)                    struct cls {
#   define VS_INPUT_ITEM(type,name,sem,ind)     type name : sem##ind;
#   define VS_INPUT_END                         };
#elif defined(TYPEMODE_C_DECL9)
#   define VS_INPUT_BGN(clsx)                   struct decl_##clsx { typedef clsx decl_cls; static const D3DVERTEXELEMENT9 p[]; }; const D3DVERTEXELEMENT9 decl_##clsx::p[] = { 
#   define VS_INPUT_ITEM(type,name,sem,ind)     {0, offsetof(decl_cls,name), T2DECLTYPE<type>::n, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_##sem, ind},
#   define VS_INPUT_END                         D3DDECL_END() };

// #   define VS_INPUT_BGN(clsx)                   typedef clsx decl_cls; static const D3DVERTEXELEMENT9 decl_##clsx[] = { 
// #   define VS_INPUT_ITEM(type,name,sem,ind)     {0, offsetof(decl_cls,name), T2DECLTYPE<type>::n, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_##sem, ind},
// #   define VS_INPUT_END                         D3DDECL_END() };
#elif defined(TYPEMODE_C_DECL11)
#   define VS_INPUT_BGN(clsx)                   struct layout_##clsx { typedef clsx layout_cls; static const D3D11_INPUT_ELEMENT_DESC p[]; }; const D3D11_INPUT_ELEMENT_DESC layout_##clsx::p[] = { 
#   define VS_INPUT_ITEM(type,name,sem,ind)     {#sem, ind, (DXGI_FORMAT)T2DXGIFMT<type>::n, 0, offsetof(layout_cls,name), D3D11_INPUT_PER_VERTEX_DATA, 0},
#   define VS_INPUT_END                         };
// #   define VS_INPUT_BGN(clsx)                   typedef clsx layout_cls; static const D3D11_INPUT_ELEMENT_DESC layout_##clsx[] = { 
// #   define VS_INPUT_ITEM(type,name,sem,ind)     {#sem, ind, (DXGI_FORMAT)T2DXGIFMT<type>::n, 0, offsetof(layout_cls,name), D3D11_INPUT_PER_VERTEX_DATA, 0},
// #   define VS_INPUT_END                         };
#elif defined(TYPEMODE_C_VERTEX)
#   define VS_INPUT_BGN(cls)                    struct cls {
#   define VS_INPUT_ITEM(type,name,sem,ind)     type name;
#   define VS_INPUT_END                         };
#else
#   define VS_INPUT_BGN(cls)
#   define VS_INPUT_ITEM(type,name,sem,ind)
#   define VS_INPUT_END
#endif



///////////////////////////////////////////////////////////////////////////////
// PS_CONSTANT_XXX
// sample: 
// gxvs_i.h:
//   PS_CONSTANT_BGN    (PS_CONSTANT)
//     PS_CONSTANT_ITEM (float4, Set0, c0)
//     PS_CONSTANT_ITEM (float4, Tex0, c1)
//   PS_CONSTANT_END

#undef PS_CONSTANT_BGN
#undef PS_CONSTANT_ITEM
#undef PS_CONSTANT_END


#ifdef TYPEMODE_HLSL_PS
#   define PS_CONSTANT_BGN(cls)                
#   define PS_CONSTANT_ITEM(type,name,x)        type name : register(x);
#   define PS_CONSTANT_END                  
#elif defined(TYPEMODE_HLSL4_PS)
#   define PS_CONSTANT_BGN(cls)                 cbuffer cls : register(b0) {
#   define PS_CONSTANT_ITEM(type,name,x)        type name;
#   define PS_CONSTANT_END                      };
#elif defined(TYPEMODE_C_VERTEX)
#   define PS_CONSTANT_BGN(cls)                 struct cls {
#   define PS_CONSTANT_ITEM(type,name,x)        type name;
#   define PS_CONSTANT_END                      };
#else
#   define PS_CONSTANT_BGN(cls)
#   define PS_CONSTANT_ITEM(type,name,x)
#   define PS_CONSTANT_END
#endif

#define VS_CONSTANT_BGN(x)  PS_CONSTANT_BGN(x)
#define VS_CONSTANT_ITEM    PS_CONSTANT_ITEM
#define VS_CONSTANT_END     PS_CONSTANT_END


#undef TYPEMODE_C_VERTEX
#undef TYPEMODE_C_DECL9
#undef TYPEMODE_C_DECL11


