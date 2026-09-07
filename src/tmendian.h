/**
 *  @file
 *  @brief  declare based, public function
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2005.11.11
 *
 *  Copyright (C) 2005 miragekiller
 */

#pragma once

#ifndef TMENDIAN_H
#define TMENDIAN_H

#include <ios>
#include "tmdebug.h"

#if TM_CPP >= TM_CPP20
#   include <bit>
#endif


/*/////////////////////////////////////////////////////////////////////////////
// check the current system's byte order
// network standard is Big-Endian
// intel cpu is Little-Endian, mac is Big-Endian
/////////////////////////////////////////////////////////////////////////////*/
enum EEndian
{
#ifdef __cpp_lib_endian
    eEndianLittle   = static_cast<int>(std::endian::little),
    eEndianBig      = static_cast<int>(std::endian::big),
    eEndianNative   = static_cast<int>(std::endian::native),
#else
    eEndianLittle   = 0,
    eEndianBig      = 1,

#   if defined(TM_CPU_LE)
    eEndianNative   = eEndianLittle,
#   elif defined(TM_CPU_BE)
    eEndianNative   = eEndianBig,
#   else
#       error need define endian
#   endif // TM_CPU_LE
#endif // __cpp_lib_endian

    eEndianNet = eEndianBig,
    eEndianHost = eEndianNative,
};

#define EndianIsBig()           (eEndianNative == eEndianBig)
#define EndianIsLittle()        (eEndianNative == eEndianLittle)
#define EndianIsNet()           EndianIsBig()


/**
 * swap byte
 * @param[in] value   source value
 * @return value swaped
 */
inline TMUInt16 EndianByteSwap16(TMUInt16 value)
{
#ifdef __cpp_lib_byteswap
    return std::byteswap(value);
#elif defined(TM_MSC) && (TM_MSC > 1400)
    return _byteswap_ushort(value);
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap16(value);
#else
    return (value >> 8) | (value << 8);
#endif
}

inline TMUInt32 EndianByteSwap32(TMUInt32 value)
{
#ifdef __cpp_lib_byteswap
    return std::byteswap(value);
#elif defined(TM_MSC) && (TM_MSC > 1400)
    return _byteswap_ulong(value);
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap32(value);
#else
    return
        ((value & 0x000000FF) << 24) |
        ((value & 0x0000FF00) <<  8) |
        ((value & 0x00FF0000) >>  8) |
        ((value & 0xFF000000) >> 24);
#endif
}

inline TMUInt64 EndianByteSwap64(TMUInt64 value)
{
#ifdef __cpp_lib_byteswap
    return std::byteswap(value);
#elif defined(TM_MSC) && (TM_MSC > 1400)
    return _byteswap_uint64(value);
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap64(value);
#else
    return
        ((value & 0x00000000000000FFui64) << 56) |
        ((value & 0x000000000000FF00ui64) << 40) |
        ((value & 0x0000000000FF0000ui64) << 24) |
        ((value & 0x00000000FF000000ui64) <<  8) |
        ((value & 0x000000FF00000000ui64) >>  8) |
        ((value & 0x0000FF0000000000ui64) >> 24) |
        ((value & 0x00FF000000000000ui64) >> 40) |
        ((value & 0xFF00000000000000ui64) >> 56);
#endif
}

TM_INLINE void EndianByteSwapStr(TMStrW pDst, TMCStrW pSrc, int n)
{
    for (int i = 0; i < n; ++i)
        pDst[i] = EndianByteSwap16(pSrc[i]);
}

/*/////////////////////////////////////////////////////////////////////////////
// exchange the machine byte order
// h: host;
// n: net;
// h2n*: convert the current machine byte order to network byte order (Big-Endian)
// n2h*: convert network byte order(Big-Endian) to current machine byte order
/////////////////////////////////////////////////////////////////////////////*/

TM_INLINE TMUInt16 EndianH2N16(TMUInt16 n)
{
    if tm_constexpr20 (EndianIsNet())
        return n;
    else
        return EndianByteSwap16(n);
}

TM_INLINE TMUInt32 EndianH2N32(TMUInt32 n)
{
    if tm_constexpr20 (EndianIsNet())
        return n;
    else
        return EndianByteSwap32(n);
}

TM_INLINE TMUInt64 EndianH2N64(TMUInt64 n)
{
    if tm_constexpr20 (EndianIsNet())
        return n;
    else
        return EndianByteSwap64(n);
}

#define EndianN2H16 EndianH2N16
#define EndianN2H32 EndianH2N32
#define EndianN2H64 EndianH2N64


TM_INLINE TMUInt16 EndianH2L16(TMUInt16 n)
{
    if tm_constexpr20 (EndianIsLittle())
        return n;
    else
        return EndianByteSwap16(n);
}

TM_INLINE TMUInt32 EndianH2L32(TMUInt32 n)
{
    if tm_constexpr20 (EndianIsLittle())
        return n;
    else
        return EndianByteSwap32(n);
}

TM_INLINE TMUInt64 EndianH2L64(TMUInt64 n)
{
    if tm_constexpr20 (EndianIsLittle())
        return n;
    else
        return EndianByteSwap64(n);
}

#define EndianL2H16 EndianH2L16
#define EndianL2H32 EndianH2L32
#define EndianL2H64 EndianH2L32


/**
 * exchange the string byte order
 * @param[in]       pSrc  source buffer
 * @param[in]       n     source buffer size
 * @param[out]      pDst  dest buffer
 * @param[in,out]   pSrcDst  src/dest buffer
 */
TM_INLINE void EndianStrL2H(TMStrW pDst, TMCStrW pSrc, int n)
{
    DbgAssert(pSrc && pDst);
    if tm_constexpr20 (EndianIsLittle())
        memcpy(pDst, pSrc, n * sizeof(*pDst));
    else
        EndianByteSwapStr(pDst, pSrc, n);
}

TM_INLINE void EndianStrB2H(TMStrW pDst, TMCStrW pSrc, int n)
{
    DbgAssert(pSrc && pDst);
    if tm_constexpr20 (EndianIsBig())
        memcpy(pDst, pSrc, n * sizeof(*pDst));
    else
        EndianByteSwapStr(pDst, pSrc, n);
}

TM_INLINE void EndianStrL2H(TMStrW pSrcDst, int n)
{
    if tm_constexpr20 (EndianIsBig())
        EndianStrL2H(pSrcDst, pSrcDst, n);
}

TM_INLINE void EndianStrB2H(TMStrW pSrcDst, int n)
{
    if tm_constexpr20 (EndianIsLittle())
        EndianStrB2H(pSrcDst, pSrcDst, n);
}


#ifdef TM_CPP

inline TMInt16  EndianH2N(TMInt8 n)     { return n; }
inline TMUInt16 EndianH2N(TMUInt8 n)    { return n; }
inline TMInt16  EndianH2N(TMInt16 n)    { return EndianH2N16(n); }
inline TMUInt16 EndianH2N(TMUInt16 n)   { return EndianH2N16(n); }
inline TMInt32  EndianH2N(TMInt32 n)    { return EndianH2N32(n); }
inline TMUInt32 EndianH2N(TMUInt32 n)   { return EndianH2N32(n); }
inline TMInt64  EndianH2N(TMInt64 n)    { return EndianH2N64(n); }
inline TMUInt64 EndianH2N(TMUInt64 n)   { return EndianH2N64(n); }

inline TMInt8   EndianH2L(TMInt8 n)     { return n; }
inline TMUInt8  EndianH2L(TMUInt8 n)    { return n; }
inline TMInt16  EndianH2L(TMInt16 n)    { return EndianH2L16(n); }
inline TMUInt16 EndianH2L(TMUInt16 n)   { return EndianH2L16(n); }
inline TMInt32  EndianH2L(TMInt32 n)    { return EndianH2L32(n); }
inline TMUInt32 EndianH2L(TMUInt32 n)   { return EndianH2L32(n); }
inline TMInt64  EndianH2L(TMInt64 n)    { return EndianH2L64(n); }
inline TMUInt64 EndianH2L(TMUInt64 n)   { return EndianH2L64(n); }

#define EndianN2H   EndianH2N
#define EndianL2H   EndianH2L


TM_BGN_NAMESPACE

template<int eType> struct CEndianT;

TM_TEMPLATE_NULL struct CEndianT<eEndianBig>
{
    template<class T> T inline static ToHost(T n)      { return EndianN2H(n); }
    template<class T> T inline static FromHost(T n)    { return EndianH2N(n); }
};

TM_TEMPLATE_NULL struct CEndianT<eEndianLittle>
{
    template<class T> inline static T ToHost(T n)      { return EndianL2H(n); }
    template<class T> inline static T FromHost(T n)    { return EndianH2L(n); }
};

typedef CEndianT<eEndianNet>    CEndianNet;
typedef CEndianT<eEndianLittle> CEndianLittle;
typedef CEndianT<eEndianHost>   CEndianHost;

TM_END_NAMESPACE

#endif // TM_CPP




#endif /* end of #ifndef TMENDIAN_H */
