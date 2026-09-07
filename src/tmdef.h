/**
 *  @file
 *  @brief  declare base type 
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2002-04-09
 *
 *  Copyright (C) 2002 miragekiller                             
 */

// 64-bit and Data Size Neutrality
// http://www.unix.org/whitepapers/64bit.html;
// https://en.wikipedia.org/wiki/64-bit_computing
// Size Specification of printf: https://msdn.microsoft.com/en-us/library/tcxf1dw6.aspx
//
// LLP64:  windows, MinGW
// LP64:   Solaris, Linux, BSD, macOS. Windows when using Cygwin; z/OS, Most Unix.
// ILP64:  HAL Computer Systems port of Solaris to the SPARC64
// SILP64: Classic UNICOS[41] (versus UNICOS/mp, etc.)
//
// Data Type        LP32    ILP32   ILP64   LLP64   LP64   printf      *
// char             8       8       8       8       8
// short            16      16      16      16      16
// int32                            32
// int              16      32      64      32      32
// long             32      32      64      32      64
// long long        64      64      64      64      64
// double                   64              64
// long double              128             128
// pointer          32      32      64      64      64     "%p"
// wchar_t                  16*             32                          some unix use 32.
// size_t           32      32      64      64      64     c99:"%z?"; vc:"%I?";
// ptrdiff_t        32      32      64      64      64

#pragma once

#ifndef TMDEF_H
#define TMDEF_H

#include <limits.h>
#include <stddef.h> /* the head file decared size_t and wchar_t && ptrdiff_t*/
#include <float.h>



/*****************************************************************************/
#define TM_COMMA ,



/*/////////////////////////////////////////////////////////////////////////////
// TM_STRING_OF(x) turns macro x to string
// TM_NUMBER_OF(x) turns macro x to digit string
// sample:
//    #define XXX 0x15
//    char* c = TM_STRING_OF(XXX); //c="XXX"
//    char* d = TM_NUMBER_OF(XXX); //c="0x15"
/////////////////////////////////////////////////////////////////////////////*/
#define TM_STRING_OF(x)     #x
#define TM_NUMBER_OF(x)     TM_STRING_OF(x)

/*/////////////////////////////////////////////////////////////////////////////
// TM_LINK_OF(x,y)
// sample:
//    #define XXX TM_LINK_OF(xxx_,__LINE__) //XXX=xxx_72
/////////////////////////////////////////////////////////////////////////////*/
#define _TM_LINK_OF(x,y)    x##y
#define TM_LINK_OF(x,y)     _TM_LINK_OF(x,y)


/*****************************************************************************
  select compiler:
    TM_INTEL; TM_GNUC; TM_BORLAND; TM_MWERKS; TM_MSC

  sample:
    #if TM_INTEL
    #if TM_MSC > 1200
    #if TM_GNUC && TM_GNUC < 400
*****************************************************************************/
#if defined(__clang__)
#   define TM_CLANG                         __clang__
#endif

#if defined(__GNUC__)
#   define TM_GNUC                          (__GNUC__ * 100 + __GNUC_MINOR__) /* 305, 403 etc. */
#endif

#if defined(__INTEL_COMPILER)
#   define TM_INTEL                         __INTEL_COMPILER
#elif defined(__ICL)
#   define TM_INTEL                         __ICL
#elif defined(__ICC)
#   define TM_INTEL                         __ICC
#elif defined(__ECC)
#   define TM_INTEL                         __ECC
#elif defined(__BORLANDC__)
#   define TM_BORLAND                       __BORLANDC__    /* 0x0550 == Borland C++Builder 5, command-line compiler 5.5; Borland C++ Builder 2008 and below: __BORLANDC__ >= 0x599 */
#elif defined(__MWERKS__)
#   define TM_MWERKS                        __MWERKS__ /* 0x2301 == 5.3; 0x2401 == 6.2; 0x2407 == 7.x; 0x3003 == 8.x */
#elif defined(_MSC_VER)
    // 1200 == VC++6.0,         1200-1202 == VC++6SP
    // 1300 == VC++7.0(2003),   1310 == VC++7.1(2003)
    // 1400 == VC++8.0(2005)
    // 1500 == VC++9.0(2008)
    // 1600 == VC++10.0(2010)
    // 1700 == VC++11.0(2012)
    // 1800 == VC++12.0(2013)
    // 1900 == VC++14.0(2015)
    // 1910 == VC++15.0(2017)
    // 1928 == VC++16.0(2019)
#   define TM_MSC                           _MSC_VER
#endif


/*****************************************************************************
  select object CPU types:
    TM_CPU_LE   // LITTLE_ENDIAN    1234
    TM_CPU_BE   // BIG_ENDIAN        4321
    TM_CPU_PE   // PDP_ENDIAN        3412
    TM_CPU_64
    TM_ARM; TM_X64; TM_X86; TM_ALPHA; TM_PPC; TM_PPC64; TM_MIPS; TM_68K; TM_MPPC; TM_SH

  sample:
    #if TM_X86 || TM_X64
*****************************************************************************/
#if defined(__arm__) || defined(__ARM_EABI__) || defined(__ARM__) || defined(_ARM_)
#   define TM_ARM                           1
#   ifdef __ARMEB__
#       define TM_CPU_BE                    1
#   else
#       define TM_CPU_LE                    1
#   endif
#elif defined(_M_IA64) || defined(__x86_64) || defined(_M_X64)
#   define TM_X64                           1
#   define TM_CPU_64                        1
#   define TM_CPU_LE                        1
#elif defined(_M_IX86) || defined(__i386__) || defined(__X86__)
#   define TM_X86                           1
#   define TM_CPU_LE                        1
#elif defined(_M_ALPHA)
#   define TM_ALPHA                         1
#   define TM_CPU_BE                        1
#elif defined(_M_PPC) || defined(__PPC__) || defined(__ppc__) // IBM PowerPC
#   define TM_PPC                           1
#   define TM_CPU_BE                        1
#elif defined(__ppc64__) // IBM PowerPC
#   define TM_PPC64
#   define TM_CPU_BE                        1
#   define TM_CPU_64                        1
#elif defined(_M_MRX000) || defined(__MIPS__)
#   define TM_MIPS                          1
#elif defined(_M_M68K) || defined(__mc68000__)
#   define TM_68K                           1
#   define TM_CPU_BE                        1
#elif defined(_M_MPPC)
#   define TM_MPPC                          1
#elif defined(__SH__)  // SuperH
#   define TM_SH
#endif

#if defined(__BIGENDIAN__) || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
#   undef TM_CPU_LE
#   undef TM_CPU_BE
#   define TM_CPU_BE                        1
#endif

#if defined(__LITTLEENDIAN__) || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
#   undef TM_CPU_LE
#   undef TM_CPU_BE
#   define TM_CPU_LE                        1
#endif

/*****************************************************************************
  select platform:
    TM_GNU(TM_ANDROID, TM_LINUX, TM_BSD, TM_SUN, TM_CYGWIN);
    TM_WIN(TM_WIN32, TM_WIN64);
    TM_APPLE(TM_MAC(TM_MAC32, TM_MAC64), TM_IOS);

  sample:
    #if TM_WIN32
    #if TM_MAC32 || TM_IOS
*****************************************************************************/
#if defined(__QNX__)
#   define TM_QNX                           1
#   define TM_GNU                           1
#elif defined(ANDROID)
#   ifndef __ANDROID_API__
#       include <android/api-level.h>
#   endif
#   define TM_ANDROID                       __ANDROID_API__ // 3:1.5; 8:2.2; 11:3.0;
#   define TM_GNU                           1
#elif defined(linux) || defined(__linux) || defined(__linux__)
#   define TM_GNU                           1
#   define TM_LINUX                         1
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#   define TM_GNU                           1
#   define TM_BSD                           1
#elif defined(sun) || defined(__sun)
#   define TM_GNU                           1
#   define TM_SUN                           1
#elif defined(__CYGWIN__)
#   define TM_GNU                           1
#   define TM_CYGWIN                        1
#elif defined(WINAPI_FAMILY) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(__WIN64__) || defined(WIN64)

#   define TM_WIN                           1

// when create a metro app, VC: #define WINAPI_FAMILY WINAPI_FAMILY_APP
// when create a desktop app, VC: not define WINAPI_FAMILY
#   if defined(WINAPI_FAMILY)
#       include <winapifamily.h>
#       if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) //WINAPI_FAMILY_ONE_PARTITION(WINAPI_FAMILY, WINAPI_PARTITION_APP)
#           define TM_WIN_DESKTOP           1
#       elif (WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE_APP))
#           define TM_WIN_PHONE             1
#       else
#           define TM_WIN_METRO             1
#       endif
#   else
#       define TM_WIN_DESKTOP               1
#   endif // WINAPI_FAMILY

#   if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#       define TM_WIN32                     1
#   endif

#   if defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
#       define TM_WIN64                     1
#       define TM_PLATFORM64                1
#   endif

#elif defined(__APPLE__) || defined(__APPLE_CC__) || defined(__APPLE_CPP__)
#   define TM_APPLE                         1
#   ifdef macintosh
#       define TM_MAC                       9
#   elif defined(__MACH__)
#       define TM_MAC                       10
#   else
#       define TM_IOS                       1
#   endif
#endif

#if defined(TM_WIN64) || defined(__LP64__) || defined(__64BIT__) || defined(_LP64) || (__WORDSIZE == 64)
#   define TM_PLATFORM64                    1
#   ifdef TM_MAC
#       define TM_MAC64                     1
#   endif
#endif

#ifdef QT_GUI_LIB
#   define TM_QT                            QT_VERSION
#elif defined(GTK_MAJOR_VERSION)
#   define TM_GTK                           (GTK_MAJOR_VERSION * 100 + GTK_MINOR_VERSION)
// #undef TM_WIN
// #undef TM_WIN32
// #undef TM_WIN64
// #undef TM_WIN_DESKTOP
#   if GTK_MAJOR_VERSION == 3
#       define TM_GTK3                      TM_GTK  // 300
#   elif GTK_MAJOR_VERSION == 4
#       define TM_GTK4                      TM_GTK  // 400
#   elif GTK_MAJOR_VERSION >= 5
#       define TM_GTK5                      TM_GTK  // 500
#   endif // GTK_MAJOR_VERSION
#elif defined(TM_WIN)
#   define TM_WIN_NO_QT                     TM_WIN
#endif // QT_GUI_LIB

// code blocks 17.12: 302000
// code blocks 20.04: 600000
#ifdef __MINGW64_VERSION_MAJOR
#   define TM_MINGW                         (__MINGW64_VERSION_MAJOR * 10000 + __MINGW64_VERSION_MINOR * 100 + __MINGW64_VERSION_BUGFIX)
#elif defined(__MINGW32_VERSION)
#   define TM_MINGW                         (__MINGW32_MAJOR_VERSION * 10000 + __MINGW32_MINOR_VERSION * 100 + __MINGW32_MINOR_VERSION)
#endif

#ifdef __CYGWIN__
#   define TM_CYGWIN                        1
#endif 

#if !defined(TM_USE_MS_ATL)
#   if defined(TM_WIN) && !defined(TM_QT) && !defined(TM_GTK) && defined(TM_MSC)
#       define TM_USE_MS_ATL                (1)
#   else
#       define TM_USE_MS_ATL                (0)
#   endif
#endif // TM_USE_MS_ATL


/*//////////////////////////////////////////////////////////////////////////////
// complie
//////////////////////////////////////////////////////////////////////////////*/
#ifdef TM_MSC
// #   pragma comment(linker, "/align:32")
// #   pragma comment(linker, "/merge:.data=.text")
// #   pragma comment(linker, "/merge:.rdata=.text")
// #   pragma comment(linker, "/OPT:NOWIN98")
#endif

/*//////////////////////////////////////////////////////////////////////////////
// remove warnings
//////////////////////////////////////////////////////////////////////////////*/
#ifdef TM_MSC
#   pragma warning (disable : 4018)     //warning C4018 : '>=' : signed / unsigned mismatch
#   pragma warning (disable : 4042)     //warning C4042: 'nDftRef' : has bad storage class
#   pragma warning (disable : 4100)     //warning C4100: 'tableName' : unreferenced formal parameter
#   pragma warning (disable : 4189)     //local variable is initialized but not referenced
#   pragma warning (disable : 4244)     //warning C4244: 'initializing' : conversion from X to Y, possible loss of data
#   pragma warning (disable : 4245)     //conversion from 'int' to 'unsigned int', signed / unsigned mismatch
#   pragma warning (disable : 4389)     //'==' : signed / unsigned mismatch
#   pragma warning (disable : 4503)     //decorated name length exceeded, name was truncated
//#   pragma warning (error : 4457) // declaration of 'identifier' hides function parameter
//#   pragma warning (error : 4458) // declaration of 'identifier' hides class member
//#   pragma warning (disable : 4616) //warning C4616: #pragma warning : warning number '6255' out of range, must be between '4001' and '4999'
//#   pragma warning (disable : 4706) //assignment within conditional expression
#   pragma warning (disable : 4786)     // identifier was truncated to '255' characters in the browser information
#   pragma warning (disable : 4838)     // warning C4838: conversion from 'double' to 'float' requires a narrowing conversion
#   pragma warning (disable : 4927)     //warning C4927 : illegal conversion; more than one user - defined conversion has been implicitly applied
//#   pragma warning (disable : 4996) //The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: _xxx.
//#   pragma warning (disable : 6102) //warning C6102 : Using 'b' from failed function call at line '41'
//#   pragma warning (disable : 6255) //warning C6255 : _alloca indicates failure by raising a stack overflow exception.Consider using _malloca instead
//#   pragma warning (disable : 6308) //warning C6308 : 'realloc' might return null pointer
//#   pragma warning (disable : 6326) //warning C6326 : Constant constant comparison    Potential comparison of a constant with another constant. sample: if (enum 1 > enum 2) ...
//#   pragma warning (disable : 28204) //warning C28204 : 'CopyTo' : Only one of this override and the one at c : \program files(x86)\windows kits\8.1\include\um\objidlbase.h(2267) are annotated for _Param_(1) : both or neither must be annotated.
#   ifdef TM_WIN64
#       pragma warning (disable : 4267) // warning C4267 'argument': conversion from 'size_t' to 'DWORD', possible loss of data
//#       pragma warning (disable : 4302) // warning C4302: 'type cast': truncation from 'int *' to 'DWORD'
#       pragma warning (disable : 4311) // warning C4311: 'type cast' : pointer truncation from 'LPVOID' to 'DWORD'
#       pragma warning (disable : 4312) // warning C4312: 'type cast' : conversion from 'OLE_HANDLE' to 'HICON' of greater size
#   endif //
#elif defined(TM_GNUC)
#   pragma GCC diagnostic ignored   "-Wsign-compare"                // comparison between signed and unsigned integer expressions
#   pragma GCC diagnostic ignored   "-Wcomment"                     // multi-line comment
#   pragma GCC diagnostic ignored   "-Wmultichar"                   // multi-character character constant [];  int x = '/>'
#   pragma GCC diagnostic ignored   "-Winvalid-offsetof"            // offset of on non-standard-layout type '...'
#   pragma GCC diagnostic ignored   "-Wdelete-non-virtual-dtor"     // deleting object of polymorphic class type 'xxx' which has non-virtual destructor might cause undefined behaviour
#   pragma GCC diagnostic ignored   "-Wunused-parameter"            // unused parameter 'comment'
#   pragma GCC diagnostic ignored   "-Wswitch"                      // xxx enumeration values not handled in switch:
#   pragma GCC diagnostic ignored   "-Wmissing-field-initializers"  // missing initializer for member 'tagSTATSTG::type' STATSTG stg = { 0 };

//#   pragma GCC diagnostic ignored   "-Wunused-value"              // left operand of comma operator has no effect
//#   pragma GCC diagnostic ignored   "-Wunused-variable"           // unused variable 'p'
//#   pragma GCC diagnostic ignored   "-Wunused-local-typedefs"     // typedef 'XXX' locally defined but not used
//#   pragma GCC diagnostic ignored   "-Wunused-but-set-variable"   // variable 'p1' set but not used
//#   pragma GCC diagnostic ignored   "-Wunused-function"           // function defined but not used


#   ifdef TM_CLANG
#   pragma clang diagnostic ignored "-Wshorten-64-to-32"            // implicit conversion loses integer precision: 'size_t' (aka 'unsigned long') to 'int'
#   pragma clang diagnostic ignored "-Wparentheses"                 // Using the result of an assignment as a condition without parentheses
#   pragma clang diagnostic ignored "-Wunused-variable"             // unused variable '_ama'
//#   pragma clang diagnostic ignored "-Wunused-value"                // expression result unused
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"     // 'CGTextEncoding' is deprecated:
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"  // missing field 'tm_gmtoff' initializer
#   pragma clang diagnostic ignored "-Wreorder"                     // field 'm_nSubMenu' will be initialized after field 'm_szCheck'
#   pragma clang diagnostic ignored "-Wundefined-bool-conversion"   // warning: 'this' pointer cannot be null in well-defined C++ code

#   endif

#endif // #ifdef TM_MSC



/*****************************************************************************/
/* function declare */
#ifdef TM_EXPORTS
#   define TM_API                           __declspec(dllexport)
#else
#   define TM_API                           //__declspec(dllimport)
#endif

#ifdef TM_MSC
#   define TM_NO_VTABLE                     __declspec(novtable)
#else
#   define TM_NO_VTABLE
#endif

#define TM_CPP98                            1L
#define TM_CPP03                            199711L
#define TM_CPP11                            201103L
#define TM_CPP14                            201402L
#define TM_CPP17                            201703L
#define TM_CPP20                            202002L
#define TM_CPP23                            202302L //if consteval; std::expected; std::flat_map; deducing this; constexpr std::unique_ptr
#define TM_CPP26                            202612L

#ifdef __cplusplus
#   ifdef _MSVC_LANG
#       define TM_CPP                       _MSVC_LANG
#   else
#       define TM_CPP                       __cplusplus
#   endif
#   define TM_C                             extern "C"
#   define TM_BGN_EXTERN_C                  extern "C" {
#   define TM_END_EXTERN_C                  }
#   define TM_INLINE                        inline
#else
#   define TM_C
#   define TM_BGN_EXTERN_C
#   define TM_END_EXTERN_C
#   if defined(TM_GNU)
#       define TM_INLINE                    inline
#   elif defined(TM_MSC)
#       define TM_INLINE                    __inline
#   else
#       define TM_INLINE                    static
#   endif
#endif /* end of #ifdef __cplusplus */

#ifndef __COUNTER__
#   define __COUNTER__                      __LINE__
#endif

// VC will remove try{}catch{} if no throw in try{}.
#ifdef TM_MSC
#   define tm_try                           __try
#   define tm_catch_all()                   __except(EXCEPTION_EXECUTE_HANDLER)
#else
#   define tm_try                           try
#   define tm_catch_all()                   catch(...)
#endif

#define TM_BEGIN_TRY_CATCH                  tm_try {
#define TM_END_TRY_CATCH                    } tm_catch_all() {}
#define TM_TRY_ALL(x)                       tm_try { x; } tm_catch_all() {}

#if (TM_CPP >= TM_CPP11) || (defined(TM_MSC) && TM_MSC >= 1300)
#   define tm_for                           for
#else
#   define tm_for                           if (false) {} else for
#endif

#if (TM_CPP >= TM_CPP11) || (defined(TM_MSC) && TM_MSC > 1500)
#   define tm_override                      override
#   define tm_final                         final
#   define tm_constexpr(x)                  constexpr
#   define tm_noexcept                      noexcept
#else
#   define tm_override
#   define tm_final
#   define tm_constexpr(x)                  x
#   define tm_noexcept                      throw()
#endif

#define tm_constexpr11                      TM_IF_CPP11(constexpr)
#define tm_constexpr14                      TM_IF_CPP14(constexpr)
#define tm_constexpr17                      TM_IF_CPP17(constexpr)
#define tm_constexpr20                      TM_IF_CPP20(constexpr)
#define tm_tpl_cexpr11                      template<class P> tm_constexpr11
#define tm_tpl_cexpr14                      template<class P> tm_constexpr14
#define tm_tpl_cexpr17                      template<class P> tm_constexpr17
#define tm_tpl_cexpr20                      template<class P> tm_constexpr20

#if (TM_CPP >= TM_CPP14)
#   define tm_deprecated(x)                 [[deprecated(x)]]
#elif (defined TM_MSC) && (TM_MSC > 1400)
#   define tm_deprecated(x)                 __declspec(deprecated(x))
#elif defined(TM_GNUC)
#   define tm_deprecated(x)                 __attribute__((deprecated(x)))
#else
#   define tm_deprecated(x)
#endif

#define tm_noreturn                         TM_IF_CPP11([[noreturn]])
#define tm_fallthrough                      TM_IF_CPP17([[fallthrough]])
#define tm_nodiscard                        TM_IF_CPP17([[nodiscard]])
#define tm_maybe_unused                     TM_IF_CPP17([[maybe_unused]])
#define tm_no_unique_addr                   TM_IF_CPP20([[no_unique_address]])
#define tm_likely                           TM_IF_CPP20([[likely]])
#define tm_unlikely                         TM_IF_CPP20([[unlikely]])

#ifdef TM_WIN
#   define tm_unused(x)                     UNREFERENCED_PARAMETER(x)
#elif defined(TM_GNUC) || defined(TM_CLANG)
#   define tm_unused(x)                     (void)x
#else
#   define tm_unused(p)                     (void)p
#endif

#if TM_CPP >= TM_CPP23
#   define tm_assume(expr)                  [[assume(expr)]]
#elif defined(_MSC_VER)
#   define tm_assume(expr)                  __assume(expr)
#elif defined(TM_GNUC)
#   define tm_assume(expr)                  __attribute__(assume(expr))
#elif defined(__clang__)
#   define tm_assume(expr)                  __builtin_assume(expr)
#else
#   define tm_assume(expr)
#endif


// #if tm_has_include(<filesystem>) XXX
#ifdef __has_include
#   define tm_has_include                   __has_include
#else
#   define tm_has_include(x)                0
#endif

#if (TM_CPP >= TM_CPP11)
#   define  tm_thread_local                 thread_local
#elif defined(TM_MSC)
#   define  tm_thread_local                 __declspec(thread)
#else
#   define  tm_thread_local                 __thread
#endif

#define tm_thread_safe  // mean the function support call on thread.

#define tm_enum(x)                          enum x TM_IF_CPP11(: int)

#define TM_CAPI                             TM_C TM_API

#ifdef TM_CPP
#   define TMClass                          class
#else
#   define TMClass                          struct
#endif // ifdef __OBJC__

// for objective c/c++
// in h file used c/cpp, the class declare use sample: class C; C* p = x;
// but if C is a objective's object ,this decare can't compile,
// now can use: TMClass C;  C* p = x;
#ifdef __OBJC__
#   define TM_Interface                     @class
#   define TM_OBJC
#else
#   define TM_Interface                     TMClass
#endif // __OBJC__


#if (defined(TM_MSC) && (TM_MSC <= 1300)) || defined(TM_BORLAND)
#   define TM_TYPENAME
#else
#   define TM_TYPENAME                      typename
#endif

#ifdef TM_MWERKS
#   define TM_TEMPLATE_NULL
#else
#   define TM_TEMPLATE_NULL                 template <>
#endif

#define TM_BGN_NAMESPACE                    namespace TM {
#define TM_END_NAMESPACE                    }

#define GX_BGN_NAMESPACE                    namespace TM {
#define GX_END_NAMESPACE                    }

#define TM_BGN_PRIVATE_NAMESPACE            namespace PRI {
#define TM_END_PRIVATE_NAMESPACE            }

#define GX TM

#define TM_BGN_NAMESPACE_X(x)               namespace x {
#define TM_END_NAMESPACE_X(x)               } // x


/*****************************************************************************/
/* rebuild size of std type */

#   define TM_UCHAR_MAX                     UCHAR_MAX
#   define TM_USHRT_MAX                     USHRT_MAX
#   define TM_UINT_MAX                      UINT_MAX
#   define TM_ULONG_MAX                     ULONG_MAX

/* long long, set TM_LLONG macro when support long long, else not */
#ifdef ULLONG_MAX /* iso c99 supported */
#   define TM_LLONG                         long long
#   define TM_ULLONG_MAX                    ULLONG_MAX
#elif defined(ULONG_LONG_MAX)
#   define TM_LLONG                         long long
#   define TM_ULLONG_MAX                    ULONG_LONG_MAX
#elif defined _UI64_MAX /* vc and borland c */
#   define TM_LLONG                         __int64
#   define TM_ULLONG_MAX                    _UI64_MAX
#else
#   define TM_LLONG                         long long
#   define TM_ULLONG_MAX                    0xffffffffffffffffULL
#endif /* end of #if ULLONG_MAX == ... */


/* wchar_t, set TM_WCHAR macro if wchar_t is stand type, else not */
#if TM_CPP >= TM_CPP11
#   define  TM_CHAR16                       char16_t
#   define  TM_CHAR32                       char32_t
#elif defined(TM_WIN)
#   define  TM_CHAR32                       TMUInt32
#   define  TM_CHAR16                       TMUInt16
#endif // TM_CPP >= TM_CPP11

#ifdef __MWERKS__
#   if defined(__cplusplus) && __option(wchar_type)
#       define TM_WCHAR                     wchar_t
/*in code warror, sizeof(wchar_t) == 2 */
#       define TM_WCHAR_MAX                 0xffff
#       undef TM_CHAR16
#       define TM_CHAR16                    wchar_t
#   endif
/* See http://aspn.activestate.com/ASPN/Mail/Message/boost/1614864 */
/* _WCHAR_T_DEFINED is the Win32 spelling */
/* _WCHAR_T is the Linux spelling */
#elif defined(_MSC_VER)
#   define TM_WCHAR                         wchar_t
#   define TM_WCHAR_MAX                     0xffff
#   undef TM_CHAR16
#   define TM_CHAR16                        wchar_t
#elif defined(__GNUC__)
/* in linux , sizeof(wchar_T) == 4 */
#   define TM_WCHAR                         wchar_t
#   define TM_WCHAR_MAX                     0xffffffff
#   undef TM_CHAR32
#   define TM_CHAR32                        wchar_t
#else
#   error std wchar_t check only support codewarrior, vc, gcc
#endif /* end of #ifdef __MWERKS__ */


/*****************************************************************************/
/* base type */
#define TMChar                              char
#define TMUChar                             unsigned char
#define TMInt                               int
#define TMUInt                              unsigned int
#define TMShort                             short
#define TMUShort                            unsigned short
#define TMLong                              long
#define TMULong                             unsigned long
// #define TMLong                              TM_LLONG
// #define TMULLong                            unsigned TM_LLONG


/*///////////////////////////////////////////////////////////////////////////*/
/* pick-up types with size */

//  8-bit types */
#if TM_UCHAR_MAX == 0xff
#   define TM_INT8                          char
#   define TM_UINT8_MAX                     TM_UCHAR_MAX
#else
#   error defaults int8 size not correct;
#endif

/*  8-bit types */
#if TM_USHRT_MAX == 0xffff
#   define TM_INT16                         short
#   define TM_UINT16_MAX                    TM_USHRT_MAX
#elif TM_UINT_MAX == 0xffff
#   define TM_INT16                         int
#   define TM_UINT16_MAX                    TM_UINT_MAX
#else
#   error defaults int16 size not correct;
#endif

/*  32-bit types */
#if TM_UINT_MAX == 0xffffffff
#   define TM_INT32                         int
#   define TM_UINT32_MAX                    TM_UINT_MAX
#elif TM_ULONG_MAX == 0xffffffff
#   define TM_INT32                         long
#   define TM_UINT32_MAX                    TM_ULONG_MAX
#else
#   error defaults int32 size not correct;
#endif

/*  64-bit types */
#if TM_ULLONG_MAX == 0xffffffffffffffff
#   define TM_INT64                         TM_LLONG
#   define TM_UINT64_MAX                    TM_ULLONG_MAX
#elif ULONG_MAX == 0xffffffffffffffff
#   define TM_INT64                         long
#   define TM_UINT64_MAX                    TM_ULONG_MAX
#   define TM_ULONG_MAX                     TM_ULONG_MAX
#endif



/*****************************************************************************/
/* uintx typedefs */
typedef TM_INT8                             TMInt8;
typedef unsigned TM_INT8                    TMUInt8;

typedef TM_INT16                            TMInt16;
typedef unsigned TM_INT16                   TMUInt16;

typedef TM_INT32                            TMInt32;
typedef unsigned TM_INT32                   TMUInt32;

#ifdef TM_INT64
    typedef TM_INT64                        TMInt64;
    typedef unsigned TM_INT64               TMUInt64;
#endif

/*****************************************************************************/
/* XXXptr */
#ifdef TM_PLATFORM64
#   define TMIntPtr                         TMInt64
#   define TMUIntPtr                        TMUInt64
#   define TMLongPtr                        TMInt64
#   define TMULongPtr                       TMUInt64
#else
#   define TMIntPtr                         TMInt
#   define TMUIntPtr                        TMUInt
#   define TMLongPtr                        TMLong
#   define TMULongPtr                       TMULong
#endif // TM_IF_CPU64

#   define TMWParam                         TMUIntPtr   // WPARAM
#   define TMLParam                         TMLongPtr   // LPARAM
#   define TMResult                         TMLongPtr   // LRESULT
#   define TM_LOWORD(l)                     ((TMUShort)(((TMULongPtr)(l)) & 0xffff))
#   define TM_HIWORD(l)                     ((TMUShort)((((TMULongPtr)(l)) >> 16) & 0xffff))
#   define TM_MAKELPARAM(a, b)              ((TMLParam)(((TMUShort)(a)) | ((TMULong)((TMUShort)(b))) << 16))

#ifndef RSIZE_MAX
#   define TM_STD_UNSAFEC
#   define RSIZE_MAX                        (ULONG_MAX >> 1)
#endif

/*/////////////////////////////////////////////////////////////////////////////
//  clock_t */
#define TMClock                             TMUInt32    // MS


/*/////////////////////////////////////////////////////////////////////////////
//  bool */
typedef int                                 TMBool;
#define TMTrue                              1
#define TMFalse                             0


/*****************************************************************************/
/* file name support for macintosh */
#ifdef macintosh
    struct FSSpec;
#   define TMFilename                       FSSpec
#else
#   define TMFilename                       TMCharT
#endif  // end of #ifdef macintosh

#   define TMFilenameT                      TMCharT
#   define TMFilenameA                      TMCharA
#   define TMFilenameW                      TMCharW


#ifndef PATH_MAX
#   define PATH_MAX MAX_PATH //MAXPATHLEN, FILENAME_MAX
#endif



/*****************************************************************************/
//typedef void*                               TMHandle;



/*///////////////////////////////////////////////////////////////////////////*/
/* change object number to memory byte
   sample : SizeByte (TMUInt16*, 3) ==  3* sizeof (TMUInt16) == 6;       */
#define SizeByte(_Type,__dwSize)    ((size_t)((_Type)0 + (__dwSize)))

/* change memory byte to object number
// sample : SizeObj (TMUInt16*, 6) ==  6 / sizeof (TMUInt16) == 3;        */
#define SizeObj(_Type,__dwSize)     ((_Type)(__dwSize) - (_Type)0)



/*****************************************************************************/
/* TM_NO_OPT macro, use to destroy warn by define func */
#define TM_NO_OPT       0



/*****************************************************************************/
/* TM_STATIC_CLASS && TM_STATIC_STRUCT */
/* instead keywords class and struct, */
/* let class or struct not display on info list */
#define TM_STATIC_CLASS                     class
#define TM_STATIC_STRUCT                    struct



/*****************************************************************************/
/* switch macro */

/* switch int 64 supported */
#ifdef TM_INT64
#   define TM_IF_I64(x)                     x
#   define TM_NO_I64(x)
#   define TM_SW_I64(x,y)                   x
#else
#   define TM_IF_I64(x)
#   define TM_NO_I64(x)                     x
#   define TM_SW_I64(x,y)                   y
#endif /* end of #ifdef TM_NO_INT64 */

#ifdef TM_GNU
#   define TM_IF_GNU(x)                     x
#   define TM_NO_GNU(x)
#   define TM_SW_GNU(x,y)                   x
#else
#   define TM_IF_GNU(x)
#   define TM_NO_GNU(x)                     x
#   define TM_SW_GNU(x,y)                   y
#endif /* end of #ifdef __GNUC__ */

#ifdef TM_MSC
#   define TM_IF_MSC(x)                     x
#   define TM_NO_MSC(x)
#   define TM_SW_MSC(x,y)                   x
#else
#   define TM_IF_MSC(x)
#   define TM_NO_MSC(x)                     x
#   define TM_SW_MSC(x,y)                   y
#endif /* end of #ifdef _MSC_VER */

#ifdef TM_MSL  // codewarrior
#   define TM_IF_MSL(x)                     x
#   define TM_NO_MSL(x)
#   define TM_SW_MSL(x,y)                   x
#else
#   define TM_IF_MSL(x)
#   define TM_NO_MSL(x)                     x
#   define TM_SW_MSL(x,y)                   y
#endif /* end of #ifdef __MSL__ */

#ifdef TM_APPLE
#   define TM_IF_MAC(x)                     x
#   define TM_NO_MAC(x)
#   define TM_SW_MAC(x,y)                   x
#else
#   define TM_IF_MAC(x)
#   define TM_NO_MAC(x)                     x
#   define TM_SW_MAC(x,y)                   y
#endif /* end of #ifdef macintosh */

#ifdef TM_QT
#   define TM_IF_QT(x)                      x
#   define TM_NO_QT(x)
#   define TM_SW_QT(x,y)                    x
#else
#   define TM_IF_QT(x)
#   define TM_NO_QT(x)                      x
#   define TM_SW_QT(x,y)                    y
#endif /* end of #ifdef TM_QT */

#ifdef TM_GTK
#   define TM_IF_GTK(x)                     x
#   define TM_NO_GTK(x)
#   define TM_SW_GTK(x,y)                   x
#else
#   define TM_IF_GTK(x)
#   define TM_NO_GTK(x)                     x
#   define TM_SW_GTK(x,y)                   y
#endif /* end of #ifdef TM_GTK */

#ifdef TM_GTK3
#   define TM_IF_GTK3(x)                    x
#   define TM_NO_GTK3(x)
#   define TM_SW_GTK3(x,y)                  x
#else
#   define TM_IF_GTK3(x)
#   define TM_NO_GTK3(x)                    x
#   define TM_SW_GTK3(x,y)                  y
#endif /* end of #ifdef TM_GTK3 */

#ifdef TM_GTK4
#   define TM_IF_GTK4(x)                    x
#   define TM_NO_GTK4(x)
#   define TM_SW_GTK4(x,y)                  x
#else
#   define TM_IF_GTK4(x)
#   define TM_NO_GTK4(x)                    x
#   define TM_SW_GTK4(x,y)                  y
#endif /* end of #ifdef TM_GTK4 */

#ifdef TM_WIN32
#   define TM_IF_WIN32(x)                   x
#   define TM_NO_WIN32(x)
#   define TM_SW_WIN32(x,y)                 x
#else
#   define TM_IF_WIN32(x)
#   define TM_NO_WIN32(x)                   x
#   define TM_SW_WIN32(x,y)                 y
#endif /* end of #ifdef WIN32 */

#ifdef TM_WIN64
#   define TM_IF_WIN64(x)                   x
#   define TM_NO_WIN64(x)
#   define TM_SW_WIN64(x,y)                 x
#else
#   define TM_IF_WIN64(x)
#   define TM_NO_WIN64(x)                   x
#   define TM_SW_WIN64(x,y)                 y
#endif /* end of #ifdef WIN64 */

#ifdef TM_PLATFORM64
#   define TM_IF_PLATFORM64(x)              x
#   define TM_NO_PLATFORM64(x)
#   define TM_SW_PLATFORM64(x,y)            x
#else
#   define TM_IF_PLATFORM64(x)
#   define TM_NO_PLATFORM64(x)              x
#   define TM_SW_PLATFORM64(x,y)            y
#endif /* end of #ifdef TM_PLATFORM64 */

#if defined TM_WIN
#   define TM_IF_WIN(x)                     x
#   define TM_NO_WIN(x)
#   define TM_SW_WIN(x,y)                   x
#else
#   define TM_IF_WIN(x)
#   define TM_NO_WIN(x)                     x
#   define TM_SW_WIN(x,y)                   y
#endif /* end of #ifdef TM_WIN */

#if defined TM_MINGW
#   define TM_IF_MINGW(x)                   x
#   define TM_NO_MINGW(x)
#   define TM_SW_MINGW(x,y)                 x
#else
#   define TM_IF_MINGW(x)
#   define TM_NO_MINGW(x)                   x
#   define TM_SW_MINGW(x,y)                 y
#endif /* end of #ifdef TM_WIN */

/*  multi byte string (mbs) && wide char string (wcs, unicode) */
#if defined(_UNICODE) || defined(UNICODE)
#   define TM_UNICODE
#   define TM_IF_UNICODE(x)                 x
#   define TM_NO_UNICODE(x)
#   define TM_SW_UNICODE(x,y)               x
#else
#   define TM_IF_UNICODE(x)
#   define TM_NO_UNICODE(x)                 x
#   define TM_SW_UNICODE(x,y)               y
#endif /* end of #ifndef _UNICODE */

#if defined(NDEBUG)
// #   define TM_DBG                           1   // enable debug always
#   define TM_PROTECTED                         // protected mode: TMDui will auto return when m_hWnd == 0;
#else
#   define TM_DBG                           1
#endif /* end of #ifndef NDEBUG */

#ifdef TM_DBG
#   define TM_IF_DBG(x)                     x
#   define TM_NO_DBG(x)
#   define TM_SW_DBG(x,y)                   x
#else
#   define TM_IF_DBG(x)
#   define TM_NO_DBG(x)                     x
#   define TM_SW_DBG(x,y)                   y
#endif // #ifdef TM_DBG

#ifdef TM_PROTECTED
#   define TM_PROTECTED
#   define TM_IF_PROTECTED(x)               x
#   define TM_NO_PROTECTED(x)
#   define TM_SW_PROTECTED(x)               x
#   else
#   define TM_IF_PROTECTED(x)
#   define TM_NO_PROTECTED(x)               x
#   define TM_SW_PROTECTED(x)
#endif

#ifdef TM_CPP
#   define TM_IF_CPP(x)                     x
#   define TM_NO_CPP(x)
#   define TM_SW_CPP(x,y)                   x
#else
#   define TM_IF_CPP(x)
#   define TM_NO_CPP(x)                     x
#   define TM_SW_CPP(x,y)                   y
#endif /* end of #ifndef TM_CPP */

#if defined(TM_CPP) && (TM_CPP >= TM_CPP03)
#   define TM_IF_CPP03(x)                   x
#   define TM_NO_CPP03(x)
#   define TM_SW_CPP03(x,y)                 x
#else
#   define TM_IF_CPP03(x)
#   define TM_NO_CPP03(x)                   x
#   define TM_SW_CPP03(x,y)                 y
#endif // defined(TM_CPP) && (TM_CPP > TM_CPP03)

#if defined(TM_CPP) && (TM_CPP >= TM_CPP11)
#   define TM_IF_CPP11(x)                   x
#   define TM_NO_CPP11(x)
#   define TM_SW_CPP11(x,y)                 x
#else
#   define TM_IF_CPP11(x)
#   define TM_NO_CPP11(x)                   x
#   define TM_SW_CPP11(x,y)                 y
#endif // defined(TM_CPP) && (TM_CPP >= TM_CPP11)

#if defined(TM_CPP) && (TM_CPP >= TM_CPP14)
#   define TM_IF_CPP14(x)                   x
#   define TM_NO_CPP14(x)
#   define TM_SW_CPP14(x,y)                 x
#else
#   define TM_IF_CPP14(x)
#   define TM_NO_CPP14(x)                   x
#   define TM_SW_CPP14(x,y)                 y
#endif // defined(TM_CPP) && (TM_CPP > TM_CPP14)

#if defined(TM_CPP) && (TM_CPP >= TM_CPP17)
#   define TM_IF_CPP17(x)                   x
#   define TM_NO_CPP17(x)
#   define TM_SW_CPP17(x,y)                 x
#else
#   define TM_IF_CPP17(x)
#   define TM_NO_CPP17(x)                   x
#   define TM_SW_CPP17(x,y)                 y
#endif // defined(TM_CPP) && (TM_CPP > TM_CPP17)

#if defined(TM_CPP) && (TM_CPP >= TM_CPP20)
#   define TM_IF_CPP20(x)                   x
#   define TM_NO_CPP20(x)
#   define TM_SW_CPP20(x,y)                 x
#else
#   define TM_IF_CPP20(x)
#   define TM_NO_CPP20(x)                   x
#   define TM_SW_CPP20(x,y)                 y
#endif // defined(TM_CPP) && (TM_CPP > TM_CPP20)

#if defined(TM_CPP) && (TM_CPP >= TM_CPP23)
#   define TM_IF_CPP23(x)                   x
#   define TM_NO_CPP23(x)
#   define TM_SW_CPP23(x,y)                 x
#else
#   define TM_IF_CPP23(x)
#   define TM_NO_CPP23(x)                   x
#   define TM_SW_CPP23(x,y)                 y
#endif // defined(TM_CPP) && (TM_CPP >= TM_CPP23)

#if defined(TM_CPP) && (TM_CPP >= TM_CPP26)
#   define TM_IF_CPP26(x)                   x
#   define TM_NO_CPP26(x)
#   define TM_SW_CPP26(x,y)                 x
#else
#   define TM_IF_CPP26(x)
#   define TM_NO_CPP26(x)                   x
#   define TM_SW_CPP26(x,y)                 y
#endif // defined(TM_CPP) && (TM_CPP >= TM_CPP26)

#ifdef TM_OBJC
#   define TM_IF_OBJC(x)                    x
#   define TM_NO_OBJC(x)
#   define TM_SW_OBJC(x,y)                  x
#else
#   define TM_IF_OBJC(x)
#   define TM_NO_OBJC(x)                    x
#   define TM_SW_OBJC(x,y)                  y
#endif /* end of #ifndef TM_OBJC */

/* check std wchar_t supported */
#ifdef TM_WCHAR
#   define TM_IF_WCHAR(x)                   x
#   define TM_IF_NWCHAR(x)
#   define TM_SW_WCHAR(x,y)                 x
#else
#   define TM_IF_WCHAR(x)
#   define TM_IF_NWCHAR(x)                  x
#   define TM_SW_WCHAR(x,y)                 y
#endif /* end of #ifdef TM_WCHAR */



/*////////////////////////////////////////////////////////////////////////////
//  multi byte string (mbs, char*) && wide char string (wcs, wchar_t*) */

/* declare CharA && CharW */
#if TM_CPP >= TM_CPP20
#   define TM_CHAR8                         char8_t
#else
#   define TM_CHAR8                         TMUInt8
#endif // TM_CPP >= TM_CPP11

typedef wchar_t                             TMCharW;
typedef char                                TMCharA;
typedef TM_CHAR8                            TMChar8;
typedef TM_CHAR16                           TMChar16;
typedef TM_CHAR32                           TMChar32;


/* declare CharT */
#undef tstring
#undef tstring_view
#undef tstringstream
#ifdef  TM_UNICODE
    typedef TMCharW                         TMCharT;
#   define __TM_T(c)                        L##c
#   define tstring                          wstring
#   define tstring_view                     wstring_view
#   define tstringstream                    wstringstream
#else
    typedef TMCharA                         TMCharT;
#   define __TM_T(c)                        c
#   define tstring                          string
#   define tstring_view                     string_view
#   define tstringstream                    stringstream
#endif /* #ifdef TM_UNICODE */

typedef TMCharT*                            TMStr;
typedef const TMCharT*                      TMCStr;
typedef TMCharA*                            TMStrA;
typedef const TMCharA*                      TMCStrA;
typedef TMCharW*                            TMStrW;
typedef const TMCharW*                      TMCStrW;
typedef TMChar8*                            TMStr8;
typedef const TMChar8*                      TMCStr8;
typedef TMChar16*                           TMStr16;
typedef const TMChar16*                     TMCStr16;
typedef TMChar32*                           TMStr32;
typedef const TMChar32*                     TMCStr32;


// string, constexpr string
#define TM_T(c)                             __TM_T(c)
#define __TM_L(c)                           L##c
#define TM_L(c)                             __TM_L(c)
#define TM_CS(c)                            StrMakeConstant(c)
#define TM_CT(c)                            StrMakeConstant(TM_T(c))
#define TM_CL(c)                            StrMakeConstant(TM_L(c))


// right value reference
#if defined(TM_CPP)
    template<class T> struct rref_obj
    {
        T* obj;
        explicit rref_obj(T* o) : obj(o) {}
        template <class P> rref_obj(const P& r) : obj(r.obj) {}
        T& operator *() const { return *obj; }
    };
#   if TM_CPP >= TM_CPP11
#       define TM_RRef(x)                   x&&
#       define TM_RRefGet(x)                (*x.obj)
#   else
#       define TM_RRef(x)                   rref_obj<x>
#       define TM_RRefGet(x)                (*x.obj)
        TM_BGN_NAMESPACE_X(std)
        template <class T> const T* move(const T* p) { return p; }
        template <class T> rref_obj<T> move(const T& p) { return rref_obj<T>(const_cast<T*>(&p)); }
        template <class T> const rref_obj<T>& move(const rref_obj<T>& p, ...) { return p; }
        TM_END_NAMESPACE_X(std)
#       endif  // TM_CPP >= TM_CPP11
#   endif // #if defined(TM_CPP)


// com
#define COMDword                            TM_SW_WIN(DWORD,TMUInt32)
#define COMPointL                           TM_SW_WIN(POINTL,TMPointI)
#define ComResult                           TM_SW_WIN(HRESULT,TMLong)


// __WCHAR_MAX: gcc

// a wide string convert a MB string need buffer in byte;
// the UTF8 need max 6 BYTE;
#define TM_MB_MAX                           6


/* define operator */
#define TM_DECLARE_CLASS_OPERATOR_EQU(S) \
    TMBool operator == (const S& rhs) const { return _chk (rhs); } \
    TMBool operator != (const S& rhs) const { return !_chk (rhs); }

#define TM_DECLARE_CLASS_OPERATOR_INC(S) \
    S& operator ++ () { _inc (); return *this; } \
    S& operator += (int n) { _inc (n); return *this; } \
    S  operator ++ (int)   const { S tmp(*this); ++tmp; return tmp; } \
    S  operator +  (int n) const { return (S (*this) += n); }

#define TM_DECLARE_CLASS_OPERATOR_DEC(S) \
    S& operator -- () { _dec (); return *this; } \
    S& operator -= (int n) { _dec (n); return *this; } \
    S  operator -- (int)   const {  return (-- S (*this)); } \
    S  operator -  (int n) const { return (S (*this) -= n); }

// this function need decalra S& operator += (int n) first
#define TM_DECLARE_CLASS_OPERATOR_EX_SELF_INC_DEC(S) \
    S& operator ++ () { return operator += (1); } \
    S  operator ++ (int) const { S tmp(*this); ++tmp; return tmp; } \
    S& operator -- () { return operator += (-1); } \
    S  operator -- (int) const {  return (--S(*this)); } \


// this function need decalra S& operator += (int n) first
#define TM_DECLARE_CLASS_OPERATOR_EX_INC(S,T) \
    S  operator +  (T n) const { return (S(*this) += n); }

// this function need decalra S& operator -= (int n) first
#define TM_DECLARE_CLASS_OPERATOR_EX_DEC(S,T) \
    S  operator -  (T n) const { return (S(*this) -= n); }

// this function need decalra S& operator += (int n) first
#define TM_DECLARE_CLASS_OPERATOR_EX_INC_DEC(S,T) \
    TM_DECLARE_CLASS_OPERATOR_EX_INC(S,T) \
    TM_DECLARE_CLASS_OPERATOR_EX_DEC(S,T) \


// this function need decalra bool operator == (const S& rhs) first
#define TM_DECLARE_CLASS_OPERATOR_EX_EQU(S) \
    bool operator != (const S& rhs) const { return !operator == (rhs); }

// this function need decalra S& operator *= (int n) first
#define TM_DECLARE_CLASS_OPERATOR_EX_MUL(S) \
    S  operator *  (int n) const { return (S(*this) *= n); }

// this function need decalra S& operator /= (int n) first
#define TM_DECLARE_CLASS_OPERATOR_EX_DIV(S) \
    S  operator /  (int n) const { return (S(*this) /= n); }

// this function need decalra S& operator <<= (int n) first
#define TM_DECLARE_CLASS_OPERATOR_EX_LSHIFT(S) \
    S  operator << (int n) const { return (S(*this) <<= n); }

// this function need decalra S& operator >>= (int n) first
#define TM_DECLARE_CLASS_OPERATOR_EX_RSHIFT(S) \
    S  operator >> (int n) const { return (S(*this) >>= n); }



/* ref object class implement */
#define DECALRE_REFOBJ_CLASS_IMPL(CLS,OBJ,memb,FAdd,FRel) \
    CLS(OBJ __obj = 0) : memb(__obj) {} \
    ~CLS() { if (t_bManaged && memb != NULL) FRel; } \
    operator OBJ() const { return memb; } \
    bool IsNull() const { return 0 != memb; } \
    void Swap(CLS& rhs) { tm_swap(memb, rhs.memb); } \
    void Release() { if (memb) { FRel; memb = 0; } } \
    OBJ Detach() { OBJ __obj = memb; memb = NULL; return __obj; } \
    void Attach(OBJ __obj) { if (t_bManaged && memb && memb != __obj) FRel; memb = __obj; } \
    CLS& operator = (OBJ __obj) { Attach(__obj); if (t_bManaged && memb != NULL) FAdd; return *this; }


/* result */
#define TM_RESULT_FAILED(x)             ((x)<0)
#define TM_RESULT_SUCCEEDED(x)          ((x)>=0)
#define TM_RESULT_VALUE(x)              ((x)<0?0:(x))


/* disable default constructor and operator= function */
#define TM_NO_COPYABLE(classname) \
    private: \
        classname (const classname&); \
        classname& operator = (const classname&);

#ifdef TM_CPP
TM_BGN_NAMESPACE
    class CNoCopyable
    {
    protected:
        CNoCopyable() {}

    private:
        CNoCopyable (const CNoCopyable&);
        const CNoCopyable& operator= (const CNoCopyable&);
    };
TM_END_NAMESPACE
#endif // TM_CPP



/* get owner this pointer from member */
/* "this-&(((ClassName*)0)->Member),": check logic error */
#if 1 //TM_DBG && TM_GNUC
/* avoid warning: invalid access to non-static data member 'XXX::YYY' of NULL object (perhaps the 'offsetof' macro was used incorrectly) */
#   define TM_OTHIS_P(This,ClassName, Member) \
    ((ClassName*)(((char*)This) - (offsetof(ClassName, Member)))) //((ClassName*)(((char*)this) - ((size_t)&(((ClassName *)1)->Member))) - 1)
#else
#   define TM_OTHIS_P(This,ClassName, Member) \
    (This-&(((ClassName*)0)->Member), ((ClassName*)(((char*)this) - (offsetof(ClassName, Member)))))
#endif
#   define TM_OTHIS(ClassName, Member) TM_OTHIS_P(this,ClassName,Member)

// #define container_of(ptr, type, member) ({ \
//     const typeof(((type *)0)->member) *__mptr = (ptr); \
//                 (type *)((char *)__mptr - offsetof(type,member));})

/* get parent this pointer from child class */
#define TM_PTHIS(ClassName) \
    (static_cast<ClassName*>(this))


/*/////////////////////////////////////////////////////////////////////////////
// sample:
// enum _EType { eTypeA, eTypeB };
// TM_ENUM_2_FLAG(EType,_EType);
// EType x = eTypeA | eTypeB;
//
/////////////////////////////////////////////////////////////////////////////*/
#ifdef TM_CPP
#   define TM_ENUM_2_FLAG(T) \
    extern "C++" {\
    tm_constexpr(TM_INLINE) T operator | (T x1, T x2) { return (T)((unsigned int)x1 | (unsigned int)x2); } \
    tm_constexpr(TM_INLINE) T operator & (T x1, T x2) { return (T)((unsigned int)x1 & (unsigned int)x2); } \
    tm_constexpr(TM_INLINE) T operator ^ (T x1, T x2) { return (T)((unsigned int)x1 & (unsigned int)x2); } \
    tm_constexpr(TM_INLINE) T operator ~ (T x1) { return (T)~(unsigned int)x1; } \
    tm_constexpr(TM_INLINE) T tm_not(T x1, ...) { return operator ~(x1); } /* vc6 not support global operator ~ enum. */ \
    TM_INLINE T& operator |= (T& x1, T x2) { return (T&)((unsigned int&)x1 |= (unsigned int)x2); } \
    TM_INLINE T& operator &= (T& x1, T x2) { return (T&)((unsigned int&)x1 &= (unsigned int)x2); } \
    TM_INLINE T& operator ^= (T& x1, T x2) { return (T&)((unsigned int&)x1 ^= (unsigned int)x2); } \
    }
#else
#   define TM_ENUM_2_FLAG(T)
#endif // TM_CPP


/*/////////////////////////////////////////////////////////////////////////////
// sample:
// struct
// {
//   DWORD m_style;
//   TM_DECALRE_FLAG(Visible, m_style, WS_VISIBLE);
// } xx;
// if (!xx().IsVisble()) xx().SetVisible();
/////////////////////////////////////////////////////////////////////////////*/
#if defined(TM_CPP) && defined(TM_MSC) && (TM_MSC < 1300)
    template <class T> inline T __TM_NOT(T x)  { return (T)~((TMUInt)(x)); }
#else
#   define __TM_NOT(x)  (~(x))
#endif // defined(TM_CPP) && defined(TM_MSC) && (TM_MSC < 1300)

#define TM_DECLARE_FLAG(name,member,flag) \
    bool Is##name() const   { return 0 != (member & (flag)); } \
    void Set##name()        { member |= (flag); } \
    void Reset##name()      { member &= __TM_NOT(flag); } \
    void Setting##name(TMBool bSet) { if (bSet) Set##name(); else Reset##name(); }

#define TM_DECLARE_NOFLAG(name,member,flag) \
    bool Is##name() const   { return !(member & (flag)); } \
    void Set##name()        { member &= __TM_NOT(flag); } \
    void Reset##name()      { member |= (flag); } \
    void Setting##name(TMBool bSet) { if (bSet) Set##name(); else Reset##name(); }

#define TM_DECLARE_FLAG_GET(name,member,flag) \
    bool Is##name() const   { return 0 != (member & (flag)); }

#define TM_DECLARE_NOFLAG_GET(name,member,flag) \
    bool Is##name() const   { return !(member & (flag)); }

#define TM_DECLARE_FLAG2(name,member,flag,mask) \
    bool Is##name() const   { return (flag) == (member & (mask)); } \
    void Set##name()        { member &= __TM_NOT(mask); member |= (flag); } \
    void Reset##name()      { member &= __TM_NOT(mask); } \
    void Setting##name(TMBool bSet) { if (bSet) Set##name(); else Reset##name(); }

#define TM_DECLARE_FLAG2_GET(name,member,flag,mask) \
    bool Is##name() const   { return (flag) == (member & (mask)); }


/*/////////////////////////////////////////////////////////////////////////////
// TM_STATIC_INIT
//  int aaa()
//  {
//      TM_STATIC_INIT(int,val,-1,GetAAA());
//      return val;
//  } 
//  this function used memory less than "int aaa() { static int val = GetAAA(); return val; }"
/////////////////////////////////////////////////////////////////////////////*/
#define TM_STATIC_INIT(type,name,dft,fnInit) \
    static type name = (dft); \
    if (name == (dft)) { name = fnInit; }


#if (TM_CPP >= TM_CPP11) && !defined(TM_MSC) // vc not support VLA
#   define tm_alloca(type,c,size) type c[size]
#else
#   define tm_alloca(type,c,size) type* c = (type*)alloca(sizeof(type)*size)
#endif


/* support standard c for all platforms */
#if defined(TM_MSC) || defined(TM_BORLAND) || defined(TM_WIN)
// #   define WCSTOK                       wcstok
#   define StrPrintfA               _snprintf
#   define StrVPrintfA              _vsnprintf
#   define StrPrintfW               _snwprintf
#   define StrVPrintfW              _vsnwprintf
#else
// #   define WCSTOK(x,y)                  wcstok(x,y,0)
#   define StrPrintfA               snprintf
#   define StrVPrintfA              vsnprintf
#   define StrPrintfW               snwprintf // swprintf
#   if defined(TM_GNU) || defined(TM_MWERKS) || defined(TM_APPLE)
#       define StrVPrintfW          vswprintf
#   else
#       define StrVPrintfW          vsnwprintf
#   endif
#endif


#ifdef TM_APPLE
// #   include "mtdef.h"
#   define _NS_SDK  GX
#   define _NS_WTL  GX
#   define _NS_ATL  GX
#else
#   define _NS_SDK
#   define _NS_WTL  WTL
#   define _NS_ATL  ATL
#endif


#ifdef TM_WIN
#   define TMHModule                HMODULE
#elif defined(TM_APPLE)
#   define TMHModule                CFBundleRef
#elif defined(TM_GTK)
#   define TMHModule                GModule*
#else
#   define TMHModule                void*
#endif // TM_WIN



enum EZOrder
{
    eZOrderChildFirst = 0,      // set current object = iterator's first child;
    eZOrderChildLast = -1,      // set current object = iterator's first child;
    eZOrderUp = -2,             // set current object = before iterator;
    eZOrderDown = -3,           // set current object = after iterator;
};

#define e_zoChildFirst          eZOrderChildFirst
#define e_zoChildLast           eZOrderChildLast
#define e_zoUp                  eZOrderUp
#define e_zoDown                eZOrderDown


#ifdef TM_CPP

// for edit, rich
typedef int TMSel0;             // 0-length
typedef int TMSel1;             // 0-length or eSelEnd
typedef int TMSel2;             // 0-length or eSelEnd or eSelCaret
enum { eSelEnd                  = -1 };
enum { eSelCaret                = -2 };
inline bool SelIs0(TMSel0 x) { return x >= 0; }
inline bool SelIs1(TMSel1 x) { return x >= eSelEnd; }
inline bool SelIs2(TMSel2 x) { return x >= eSelCaret; }
struct TMSelRange
{
    TMSel2 left;
    TMSel2 right;
};


///////////////////////////////////////////////////////////////////////////////
// custom allocator
// sample:
//  template<class T> struct my_alloc { TM_SELF_ALLOCATOR(my_alloc,(MemAlloc(n, T)), MemFree(p)) };
//  typedef std::set<int, std::less<int>, my_alloc<int>> myset;
#define TM_SELF_ALLOCATOR(name,fn_alloc,fn_free)                            \
    typedef T value_type;                                                   \
    typedef T& reference;                                                   \
    typedef const T& const_reference;                                       \
    typedef T* pointer;                                                     \
    typedef const T* const_pointer;                                         \
    typedef size_t size_type;                                               \
    typedef ptrdiff_t difference_type;                                      \
    name() {}                                                               \
    template<class P> name(const name<P>&) {}                               \
    template<class P> static void destroy(P* p) { p->~P(); }                \
    static char* _Charalloc(size_t n) { return (char*)fn_alloc; }           \
    static T* allocate(size_t n) { return (T*)_Charalloc(n * sizeof(T)); }  \
    static void deallocate(void *p, size_t) { fn_free; }


TM_BGN_NAMESPACE

using ::EZOrder;
using ::eZOrderDown;
using ::eZOrderUp;
using ::eZOrderChildFirst;
using ::eZOrderChildLast;


///////////////////////////////////////////////////////////////////////////////
// CStdStr:             WTL CString + std::string interface, std::string core.
// CString(CStrPtr):    WTL CString + std::string interface, std::string core, copy on write.
// CStrView:            WTL CString + std::string_view interface, std::string_view core. read only.
template <class CharT> struct _cstring_mode_strong;
template <class CharT> struct _cstring_mode_std;
template <class CharT> struct _cstring_mode_view;
template <class CharT> struct _cstring_mode_weak;


template <class T, int n>                                   class CStrArrayT;
template <class CharT,
          class T_mode = _cstring_mode_strong<CharT> >      class CStringT;

typedef CStringT<TMCharA>                                   CStrPtrA;
typedef CStringT<TMCharW>                                   CStrPtrW;
typedef CStringT<TMCharT>                                   CStrPtr;

typedef CStringT<TMCharA, _cstring_mode_weak<TMCharA> >     CWeakStrA;
typedef CStringT<TMCharW, _cstring_mode_weak<TMCharW> >     CWeakStrW;
typedef CStringT<TMCharT, _cstring_mode_weak<TMCharT> >     CWeakStr;


typedef CStringT<TMCharA, _cstring_mode_std<TMCharA> >      CStdStrA;
typedef CStringT<TMCharW, _cstring_mode_std<TMCharW> >      CStdStrW;
typedef CStringT<TMCharT, _cstring_mode_std<TMCharT> >      CStdStr;

typedef CStringT<TMCharA, _cstring_mode_view<TMCharA> >     CStrViewA;
typedef CStringT<TMCharW, _cstring_mode_view<TMCharW> >     CStrViewW;
typedef CStringT<TMCharT, _cstring_mode_view<TMCharT> >     CStrView;

typedef CStringT<TMCharA>                                   CStringA;
typedef CStringT<TMCharW>                                   CStringW;
typedef CStringT<TMCharT>                                   CString;



TM_END_NAMESPACE


#endif // TM_CPP



/* define public function */
#define tm_max(a,b) (((a) > (b)) ? (a) : (b))
#define tm_min(a,b) (((a) < (b)) ? (a) : (b))

#define tm_countof(array) (sizeof(array)/sizeof(array[0]))

#ifdef TM_CPP

    // for CComPtr or QT build error: taking address of temporary 'xxx'
    // sample:
    //   void aaa(const CRect*){}
    //   aaa(&CRect()); // QT will build error
    //   aaa(std::addressof(CRect())); // c++ >11 will build error
    template<class T> tm_constexpr(inline) T* tm_addressof(const T& __r) {
        return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(__r))); }


    template<class T, class P> tm_constexpr(inline) void tm_setmax(T& __x, const P& __y) { if (__x < __y) __x = __y; }
    template<class T, class P> tm_constexpr(inline) void tm_setmin(T& __x, const P& __y) { if (__x > __y) __x = __y; }

    template<class T> tm_constexpr(inline) void tm_swap(T& __x, T& __y) {
        const T __tmp(__x); __x = __y; __y = __tmp; }

    template<class T> tm_constexpr(inline) T* tm_const_cast(const T* p) {
        return const_cast < T* > (p); }

    template<class T> tm_constexpr(inline) T& tm_const_cast_ref(const T& p) {
        return const_cast <T&> (p); }

    template<class T> struct constexpr_obj
    {
        const T* data() const { return (const T*)this; }
    };
    template<class T> tm_constexpr(inline) const constexpr_obj<T>* tm_constexpr_cast(const T* p) { return (const constexpr_obj<T>*)p; }


    ////////////////////////////////////////////////////////////////////////////
    // caution, the function is unsecure, sample:
    // #define STRLEN(x)  StrSwitch(x,strlen,wcslen)(x);
    // STRLEN(pc++) // pc++ runned 2 times
    template<class F1, class F2>
        tm_constexpr(inline) F1 StrSwitch(TMCStrA, F1 f1, F2) { return f1; }
    template<class F1, class F2>
        tm_constexpr(inline) F2 StrSwitch(TMCStrW, F1, F2 f2) { return f2; }

    template<class T> typename T::const_iterator tm_cbegin(const T& __x) { return __x.begin(); }
    template<class T> typename T::const_iterator tm_cend(const T& __x) { return __x.end(); }
    template<class T> typename T::const_pointer  tm_cend_data(const T& __x) { return __x.data() + __x.size(); }


#endif // TM_CPP



#endif /* end of #ifndef _ATDEF_H */

