/**
 *  @file
 *  @brief  string function
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.06.25
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef TMTCHAR_H
#define TMTCHAR_H

#include <malloc.h>     // alloca
#include <stdio.h>      // snprintf
#include <wchar.h>      // snwprintf
#include <wctype.h>     // towupper
#include <ctype.h>      // toupper
#include "tmdebug.h"
#include "tmio.h"

#ifdef TM_WIN
#   include <windows.h>
#   include <wtypes.h>
#   include <oleauto.h>
#   include <shlwapi.h>
#endif

#ifdef __cplusplus
#   include <algorithm>
#   include <string>
#endif


#define _FIMP5(fn,P1,P2,P3,P4,P5) \
    (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) { return fn(p1, p2, p3, p4, p5); }
#define _FIMP6(fn,P1,P2,P3,P4,P5,P6) \
    (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) { return fn(p1, p2, p3, p4, p5, p6); }

#define _FP3(P1,P2,P3)          P1 p1, P2 p2, P3 p3
#define _FP4(P1,P2,P3,P4)       P1 p1, P2 p2, P3 p3, P4 p4
#define _FP5(P1,P2,P3,P4,P5)    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5
#define _FP6(P1,P2,P3,P4,P5,P6) P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6
#define _FI3(fn)                { return fn(p1, p2, p3); }
#define _FI4(fn)                { return fn(p1, p2, p3, p4); }
#define _FI5(fn)                { return fn(p1, p2, p3, p4, p5); }
#define _FI6(fn)                { return fn(p1, p2, p3, p4, p5, p6); }


/**
 *  @defgroup string
 */



/**
 *  @addtogroup string
 *  string "c" function prototype
 *  @{
 */




/**
 * @param[in]   character to classify 
 * @return      true if the character is a space letter
 */
tm_constexpr(TM_INLINE) bool StrIsSpace(int ch) tm_noexcept
{
    return ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t';
}


/**
 * trim string left.
 * @param[in] str   source buffer. can't be 0.
 * @param[in] size  source buffer size. max: -1.
 * @return valid string position;
 */
template <class T>
tm_constexpr17 inline size_t StrSkipSpace(const T* str, size_t size) tm_noexcept
{
    DbgAssert(str);
    size_t n = 0;
    for (; n < size; ++n)
    {
        T c = str[n];
        if (!StrIsSpace(c))
            break;
    }
    return n;
}

/*!
  @function  StrChr
  @param[in] str    pointer to string to be searched, must not 0
  @param[in] size   string buffer size. max: -1
  @param[in] ch     string to search for, must not 0;
  @result
    if not find return 0;
*/
template <class T>
tm_constexpr17 inline const T* StrChr(const T* str, T ch, size_t size = -1) tm_noexcept
{
    TM_IF_CPP17(DbgAssert(str));
    TM_IF_CPP17(ch);
    return std::char_traits<T>::find(str, size, ch);
}


/**
 * safe string length.
 * @param[in] str source buffer. can be 0.
 * @param[in] maxsize source buffer size. max: -1.
 * @return string length. not including the end 0. always < maxsize;
 * @par Generic-Text Routine Mappings:
   - multi byte: StrLenA
   - unicode:    StrLenW
   - c++:        StrLen
 */
TM_CAPI size_t StrLenA(TMCStrA str, size_t maxsize = -1);
TM_CAPI size_t StrLenW(TMCStrW str, size_t maxsize = -1);

TM_INLINE size_t StrLen(TMCStrA str, size_t maxsize = -1) { return StrLenA(str, maxsize); }
TM_INLINE size_t StrLen(TMCStrW str, size_t maxsize = -1) { return StrLenW(str, maxsize); }


/**
 * safe string copy.
 * @param[in] dest destination buffer. pd must valid if ps && ns && nd valid.
 * @param[in] dsize destination buffer size. max: -1.
 * @param[in] src source buffer.
 * @param[in] ssize source buffer size. max: -1.
 * @return new string length. not including the end 0.
 * @remark at the end of pd will auto append 0, if dsize > src length.
 * @par Generic-Text Routine Mappings:
   - multi byte: StrCopyA
   - unicode:    StrCopyW
   - c++:        StrCopy
 */
TM_CAPI size_t StrCopyA(TMStrA dest, size_t dsize, TMCStrA src, size_t ssize = -1);
TM_CAPI size_t StrCopyW(TMStrW dest, size_t dsize, TMCStrW src, size_t ssize = -1);

TM_INLINE size_t StrCopy(_FP4(TMStrA, size_t, TMCStrA, size_t)) _FI4(StrCopyA);
TM_INLINE size_t StrCopy(_FP4(TMStrW, size_t, TMCStrW, size_t)) _FI4(StrCopyW);
TM_INLINE size_t StrCopy(_FP3(TMStrA, size_t, TMCStrA))         _FI3(StrCopyA);
TM_INLINE size_t StrCopy(_FP3(TMStrW, size_t, TMCStrW))         _FI3(StrCopyW);

/**
 * Compare strings.
 * @param[in]   lhs strings1 to compare, can be 0.
 * @param[in]   lsize strings1 length, can be -1.
 * @param[in]   rhs strings2 to compare, must not 0.
 * @param[in]   rsize strings2 length, can be -1.
 * @remark      0 == ""; "" == "".
 * @return
    - < 0 string1 less than string2
    - ==0 string1 identical to string2
    - > 0 string1 greater than string2
 * @par Generic-Text Routine Mappings:
   - multi byte: StrCompareA
   - unicode:    StrCompareW
   - c++:        StrCompare
 */
TM_CAPI int StrCompareA(TMCStrA lhs, size_t lsize, TMCStrA rhs, size_t rsize);
TM_CAPI int StrCompareW(TMCStrW lhs, size_t lsize, TMCStrW rhs, size_t rsize);

TM_INLINE int StrCompare(_FP4(TMCStrA, size_t, TMCStrA, size_t)) _FI4(StrCompareA);
TM_INLINE int StrCompare(_FP4(TMCStrW, size_t, TMCStrW, size_t)) _FI4(StrCompareW);
TM_INLINE int StrCompare(TMCStrA lhs, TMCStrA rhs, size_t size) { DbgAssert(size <= INT_MAX); return strncmp(lhs, rhs, size); }
TM_INLINE int StrCompare(TMCStrW lhs, TMCStrW rhs, size_t size) { DbgAssert(size <= INT_MAX); return wcsncmp(lhs, rhs, size); }
TM_INLINE int StrCompare(TMCStrA lhs, TMCStrA rhs)              { return strcmp(lhs, rhs); }
TM_INLINE int StrCompare(TMCStrW lhs, TMCStrW rhs)              { return wcscmp(lhs, rhs); }


/**
 * Compare characters of two strings without regard to case.
 * @param[in] p1 strings1 to compare, must not 0.
 * @param[in] p2 strings2 to compare, must not 0.
 * @param[in] n Number of characters to compare.
 * @return
    - < 0 string1 less than string2
    - ==0 string1 identical to string2
    - > 0 string1 greater than string2
 * @par Generic-Text Routine Mappings:
   - multi byte: StrCompareIA
   - unicode:    StrCompareIW
   - c++:        StrCompareI
 */
TM_CAPI int StrCompareIA(TMCStrA lhs, size_t lsize, TMCStrA rhs, size_t rsize);
TM_CAPI int StrCompareIW(TMCStrW lhs, size_t lsize, TMCStrW rhs, size_t rsize);

// fix MSVC warning C4996: 'strnicmp': The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name: _strnicmp. See online help for details.
#ifdef TM_WIN
#   define __stricmp  _stricmp
#   define __wcsicmp  _wcsicmp
#   define __strnicmp _strnicmp
#   define __wcsnicmp _wcsnicmp
#elif defined(TM_GNUC) || defined(TM_CLANG)
#   define __stricmp  strcasecmp
#   define __wcsicmp  wcscasecmp
#   define __strnicmp strncasecmp
#   define __wcsnicmp wcsncasecmp
#else
#   define __stricmp  stricmp
#   define __wcsicmp  wcsicmp
#   define __strnicmp strnicmp
#   define __wcsnicmp wcsnicmp
#endif 

TM_INLINE int StrCompareI(_FP4(TMCStrA, size_t, TMCStrA, size_t)) _FI4(StrCompareIA);
TM_INLINE int StrCompareI(_FP4(TMCStrW, size_t, TMCStrW, size_t)) _FI4(StrCompareIW);
TM_INLINE int StrCompareI(TMCStrA lhs, TMCStrA rhs, size_t size) { DbgAssert(size <= INT_MAX); return __strnicmp(lhs, rhs, size); }
TM_INLINE int StrCompareI(TMCStrW lhs, TMCStrW rhs, size_t size) { DbgAssert(size <= INT_MAX); return __wcsnicmp(lhs, rhs, size); }
TM_INLINE int StrCompareI(TMCStrA lhs, TMCStrA rhs)              { return __stricmp(lhs, rhs); }
TM_INLINE int StrCompareI(TMCStrW lhs, TMCStrW rhs)              { return __wcsicmp(lhs, rhs); }


/*!
 * @function        StrUpper strLower
 * @param[in] dest  destination buffer
 * @param[in] src   source buffer.
 * @param[in] ssize source buffer size. max: -1.
 * @return          number of characters converted.
 * @remark          don't auto append 0.
 */
#ifdef TM_WIN_DESKTOP
    TM_INLINE int StrUpperA(TMStrA dest, TMCStrA src, size_t ssize = -1) { return LCMapStringA(LOCALE_USER_DEFAULT, LCMAP_UPPERCASE, src, ssize, dest, INT_MAX); }
    TM_INLINE int StrUpperW(TMStrW dest, TMCStrW src, size_t ssize = -1) { return LCMapStringW(LOCALE_USER_DEFAULT, LCMAP_UPPERCASE, src, ssize, dest, INT_MAX); }
    TM_INLINE int strLowerA(TMStrA dest, TMCStrA src, size_t ssize = -1) { return LCMapStringA(LOCALE_USER_DEFAULT, LCMAP_LOWERCASE, src, ssize, dest, INT_MAX); }
    TM_INLINE int strLowerW(TMStrW dest, TMCStrW src, size_t ssize = -1) { return LCMapStringW(LOCALE_USER_DEFAULT, LCMAP_LOWERCASE, src, ssize, dest, INT_MAX); }
#else
    TM_CAPI int StrUpperA(TMStrA dest, TMCStrA src, size_t ssize = -1);
    TM_CAPI int StrUpperW(TMStrW dest, TMCStrW src, size_t ssize = -1);
    TM_CAPI int strLowerA(TMStrA dest, TMCStrA src, size_t ssize = -1);
    TM_CAPI int strLowerW(TMStrW dest, TMCStrW src, size_t ssize = -1);
#endif // TM_WIN

TM_INLINE int StrUpper(TMStrA dest, TMCStrA src, size_t ssize = -1) { return StrUpperA(dest, src, ssize); }
TM_INLINE int StrUpper(TMStrW dest, TMCStrW src, size_t ssize = -1) { return StrUpperW(dest, src, ssize); }
TM_INLINE int strLower(TMStrA dest, TMCStrA src, size_t ssize = -1) { return strLowerA(dest, src, ssize); }
TM_INLINE int strLower(TMStrW dest, TMCStrW src, size_t ssize = -1) { return strLowerW(dest, src, ssize); }
TM_INLINE int StrUpper(TMStrA str, size_t size) { return StrUpperA(str, str, size); }
TM_INLINE int StrUpper(TMStrW str, size_t size) { return StrUpperW(str, str, size); }
TM_INLINE int strLower(TMStrA str, size_t size) { return strLowerA(str, str, size); }
TM_INLINE int strLower(TMStrW str, size_t size) { return strLowerW(str, str, size); }


/*!
 * @function        StrToUpper StrToLower
 * @param[in] dest  destination buffer
 * @param[in] src   source buffer.
 * @param[in] ssize source buffer size. max: -1.
 * @return          number of characters converted.
 * @remark          don't auto append 0.
 */
#define StrToUpperA                 toupper
#define StrToUpperW                 towupper
#define StrToLowerA                 tolower
#define StrToLowerW                 towlower

TM_INLINE int StrToUpper(TMCharA ch) { return StrToUpperA(ch); }
TM_INLINE int StrToUpper(TMCharW ch) { return StrToUpperW(ch); }
TM_INLINE int StrToLower(TMCharA ch) { return StrToLowerA(ch); }
TM_INLINE int StrToLower(TMCharW ch) { return StrToLowerW(ch); }


/**
 *  convert string range to long
 *  @param[in]  string to be scan, can be 0.
 *  @param[in]  string size, can be -1.
 *  @param[out] ret the converted value, if error do nothing.
 *  @param[in]  base Number base to use, set 0 to auto check:
        -  8: 0123
        - 16: 0x123
        - 10: other
 *  @result     position to character that stops scan. if error will trim string left
 *  @remark     this function will skip space before string.
 *  @par sample:
 *  @code
     const char* p = "0x123H";
     size_t pp;
     long l;
     pp = StrScanLong(p, 0, l, 0);      // pp:"H";     l = 0x123;
     pp = StrScanLong(p, 0, l, 10);     // pp:"x123H"; l = 0;
     pp = StrScanLong(p, p + 4, l, 16); // pp:"3H";    l = 0x12;
 *  @endcode
 */
TM_CAPI size_t StrScanLongA(TMCStrA str, size_t size, TMLongPtr& ret, int base);
TM_CAPI size_t StrScanLongW(TMCStrW str, size_t size, TMLongPtr& ret, int base);

TM_INLINE size_t StrScanLong(_FP4(TMCStrA, size_t, TMLongPtr&, int)) _FI4(StrScanLongA);
TM_INLINE size_t StrScanLong(_FP4(TMCStrW, size_t, TMLongPtr&, int)) _FI4(StrScanLongW);
#ifdef TM_PLATFORM64
TM_INLINE size_t StrScanLong(_FP4(TMCStrA, size_t, long&, int)) { TMLongPtr x = p3; size_t r = StrScanLongA(p1,p2,x,p4); p3 = x; return r; }
TM_INLINE size_t StrScanLong(_FP4(TMCStrW, size_t, long&, int)) { TMLongPtr x = p3; size_t r = StrScanLongW(p1,p2,x,p4); p3 = x; return r; }
#endif
TM_INLINE TMLongPtr StrScanLong(TMCStrA pb, int nBase) { TMLongPtr x = 0; StrScanLongA(pb, -1, x, nBase); return x; }
TM_INLINE TMLongPtr StrScanLong(TMCStrW pb, int nBase) { TMLongPtr x = 0; StrScanLongW(pb, -1, x, nBase); return x; }


/**
 *  scan double from string view
 *  @param[in]  str     string to be scan, can be 0.
 *  @param[in]  size    string size, can be -1.
 *  @param[out] ret     the converted value, if error do nothing.
 *  @result     position to character that stops scan. if error will trim string left
 *  @remark     this function will skip space before string.
 *  @par sample:
 *  @code
     const char* p = " -315.5e5BN";
     double l = HUGE_VAL;
     size_t next;
     next = StrScanDouble(p, 0, l);      // next:"BN"; l = -315.5e5;
     next = StrScanDouble(p, 6, l);      // next:"5e5BN"; l = -315;
     next = StrScanDouble(p, 2, l);      // next:"-315.5e5BN"; l: HUGE_VAL;
 *  @endcode
 */
TM_CAPI size_t StrScanDoubleA(TMCStrA str, size_t size, double& ret);
TM_CAPI size_t StrScanDoubleW(TMCStrW str, size_t size, double& ret);

TM_INLINE size_t StrScanDouble(_FP3(TMCStrA, size_t, double&)) _FI3(StrScanDoubleA);
TM_INLINE size_t StrScanDouble(_FP3(TMCStrW, size_t, double&)) _FI3(StrScanDoubleW);


/**
 *  check it it matches some wildcard characters 
     may check the following wildcard character:'*', '?'; sample:"*.exe", "a?b.doc"...
     cSplit is for separation between wildcard characters  c,like "*.exe;*.doc",cSplit is ';' here 
     this function does not check the validity of the input strings 
 *  @param[in]  str     the source buffer to check
 *  @param[in]  pKey    the key string to match
 *  @param[in]  cSplit  key string split
 *  @param[in]  bCase   case sensitive if bCase is true
 *  @result     return true if there is string matched, else return false
 */
TM_CAPI TMBool StrMatchA(TMCStrA str, TMCStrA pKey, TMCharA cSplit, TMBool bCase);
TM_CAPI TMBool StrMatchW(TMCStrW str, TMCStrW pKey, TMCharW cSplit, TMBool bCase);

TM_INLINE TMBool StrMatch(_FP4(TMCStrA, TMCStrA, TMCharA, TMBool)) _FI4(StrMatchA);
TM_INLINE TMBool StrMatch(_FP4(TMCStrW, TMCStrW, TMCharW, TMBool)) _FI4(StrMatchW);
// FsRtlIsNameInExpression


/**
 *  @function StrConvert StrA2W StrW2A StrA2A StrAuto2W
 *  @param ndcp   dest code page, cn:936; big5:950; 932:jpn; 933:kor; 0:local;
 *  @param nscp   src code page;
 *  @param pd     pointer to dest buffer;
 *  @paran nd     dest buffer size; -1 not detect buffer;
 *  @param ps     pointer to src string;
 *  @param ns     src string length; -1 stop on '\0';
 *  @result       return convert dest string length, not include '\0'; if no enough buffer return 0;
 *  @remark       this func will not auto appand '\0';
*/
#ifndef CP_UTF8
#   define CP_UTF8      65001
#endif // #ifndef CP_UTF8

#define CP_DEFAULT   0 // 1252    // windows default CP

TM_CAPI size_t StrA2W(int ndcp, TMStrW pd, size_t nd, TMCStrA ps, size_t ns);
TM_CAPI size_t StrW2A(int nscp, TMStrA pd, size_t nd, TMCStrW ps, size_t ns);
TM_CAPI size_t StrA2A(int ndcp, TMStrA pd, size_t nd, int nscp, TMCStrA ps, size_t ns);

// try to convert use utf8 first, if error use ANSI.
// the function don't append 0 after end of string.
TM_CAPI size_t StrAuto2W(TMStrW pd, size_t nd, TMCStrA ps, size_t ns, TMBool* bUtf);


TM_INLINE size_t StrConvert   _FIMP5(StrA2W, int, TMStrW, size_t, TMCStrA, size_t);
TM_INLINE size_t StrConvert   _FIMP5(StrW2A, int, TMStrA, size_t, TMCStrW, size_t);
TM_INLINE size_t StrConvert   _FIMP6(StrA2A, int, TMStrA, size_t, int, TMCStrA, size_t);


/**
 *  @function     StrUrlEnc[Size] StrUrlDec[Size]
 *  @param pd     pointer to dest buffer;
 *  @param ps     pointer to src string;
 *  @param ns     src string length; -1 stop on '\0';
 *  @result       return convert dest string length, not include '\0';
 *  @remark       this func will not auto appand '\0';
*/
#define StrUrlEncSize(x)               (3 * (x))
#define StrUrlDecSize(x)               (x)

TM_CAPI int StrUrlEnc(const char* ps, char* pd, int ns);
TM_CAPI int StrUrlDec(const char* ps, char* pd, int ns);


/**
 *  @function           StrSplitIf StrSplitLine StrSplitChr StrSplitChrI StrSplitStr StrSplitStrI StrSplitOne
 *  @param str[in]      string to search for, can be 0.
 *  @param size[in]     string length, can be -1.
 *  @param cur[in,out]  position at which to start the search. return next position to start search
 *  @param f[in]        search function
 *  @param key[in]      search key, must be valid.
 *  @param ksize[in]    key length, can be -1.
 *  @result             Pointer to the found in string, or 0 if not found
 *  @par sample:
 *  @code
    size_t head = 3;
    size_t tail = StrSplitXXX("abcdefg", 7, head, ...);
    if (head) // find
    {
      prev = [3, tail);
      key  = [tail, head);
      next = [head, 7);
    }
    // line split: win:\r\n; Linux/Unix:\n; Mac OS:\r; other:\n\r;
    // split line result:
    // "aaa":      -> "aaa"
    // "aaa\r\n":  -> "aaa"
    // "aaa\rb":   -> "aaa" + "b"
    // "\r\n":     -> ""
    //  "":        -> n/a
 *  @endcode
*/
typedef size_t (*PfnSpliteA)(TMCStrA str, size_t& cur, size_t size); // cur:set keyword pos; return: next token. not match: return 0;
typedef size_t (*PfnSpliteW)(TMCStrW str, size_t& cur, size_t size);
TM_CAPI size_t StrSplitIfA(TMCStrA str, size_t size, size_t& cur, PfnSpliteA f);
TM_CAPI size_t StrSplitIfW(TMCStrW str, size_t size, size_t& cur, PfnSpliteW f);

TM_CAPI size_t StrSplitLineA(TMCStrA str, size_t size, size_t& cur);
TM_CAPI size_t StrSplitLineW(TMCStrW str, size_t size, size_t& cur);

TM_CAPI size_t StrSplitChrA(TMCStrA str, size_t size, size_t& cur, TMCharA key);
TM_CAPI size_t StrSplitChrW(TMCStrW str, size_t size, size_t& cur, TMCharW key);

TM_CAPI size_t StrSplitChrIA(TMCStrA str, size_t size, size_t& cur, TMCharA key);
TM_CAPI size_t StrSplitChrIW(TMCStrW str, size_t size, size_t& cur, TMCharW key);

TM_CAPI size_t StrSplitStrA(TMCStrA str, size_t size, size_t& cur, TMCStrA key, size_t ksize);
TM_CAPI size_t StrSplitStrW(TMCStrW str, size_t size, size_t& cur, TMCStrW key, size_t ksize);

TM_CAPI size_t StrSplitStrIA(TMCStrA str, size_t size, size_t& cur, TMCStrA key, size_t ksize);
TM_CAPI size_t StrSplitStrIW(TMCStrW str, size_t size, size_t& cur, TMCStrW key, size_t ksize);

TM_CAPI size_t StrSplitOneA(TMCStrA str, size_t size, size_t& cur, TMCStrA key, size_t ksize);
TM_CAPI size_t StrSplitOneW(TMCStrW str, size_t size, size_t& cur, TMCStrW key, size_t ksize);

TM_INLINE size_t StrSplitIf  (_FP4(TMCStrA, size_t, size_t&, PfnSpliteA))       _FI4(StrSplitIfA);
TM_INLINE size_t StrSplitIf  (_FP4(TMCStrW, size_t, size_t&, PfnSpliteW))       _FI4(StrSplitIfW);
TM_INLINE size_t StrSplitLine(_FP3(TMCStrA, size_t, size_t&))                   _FI3(StrSplitLineA);
TM_INLINE size_t StrSplitLine(_FP3(TMCStrW, size_t, size_t&))                   _FI3(StrSplitLineW);
TM_INLINE size_t StrSplitChr (_FP4(TMCStrA, size_t, size_t&, TMCharA))          _FI4(StrSplitChrA);
TM_INLINE size_t StrSplitChr (_FP4(TMCStrW, size_t, size_t&, TMCharW))          _FI4(StrSplitChrW);
TM_INLINE size_t StrSplitChrI(_FP4(TMCStrA, size_t, size_t&, TMCharA))          _FI4(StrSplitChrIA);
TM_INLINE size_t StrSplitChrI(_FP4(TMCStrW, size_t, size_t&, TMCharW))          _FI4(StrSplitChrIW);
TM_INLINE size_t StrSplitStr (_FP5(TMCStrA, size_t, size_t&, TMCStrA, size_t))  _FI5(StrSplitStrA);
TM_INLINE size_t StrSplitStr (_FP5(TMCStrW, size_t, size_t&, TMCStrW, size_t))  _FI5(StrSplitStrW);
TM_INLINE size_t StrSplitStrI(_FP5(TMCStrA, size_t, size_t&, TMCStrA, size_t))  _FI5(StrSplitStrIA);
TM_INLINE size_t StrSplitStrI(_FP5(TMCStrW, size_t, size_t&, TMCStrW, size_t))  _FI5(StrSplitStrIW);
TM_INLINE size_t StrSplitOne (_FP5(TMCStrA, size_t, size_t&, TMCStrA, size_t))  _FI5(StrSplitOneA);
TM_INLINE size_t StrSplitOne (_FP5(TMCStrW, size_t, size_t&, TMCStrW, size_t))  _FI5(StrSplitOneW);


/**
 *  @function           StrVPrintf StrPrintf
 *  @param buf[in,out]  string result.
 *  @param size[in]     string buffer size
 *  @param fmt[in]      format control
 *  @result             string result length.
 */
#ifdef TM_MSC
#   pragma warning (push)
#   pragma warning (disable : 4995) // '_snprintf': name was marked as #pragma deprecated
#   pragma warning (disable : 4996) // '_vsnprintf': This function or variable may be unsafe. Consider using _vsnprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#endif // #ifdef TM_MSC

TM_INLINE int StrVPrintf(_FP4(TMStrA, int, TMCStrA, va_list))  _FI4(StrVPrintfA);
TM_INLINE int StrVPrintf(_FP4(TMStrW, int, TMCStrW, va_list))  _FI4(StrVPrintfW);

TM_INLINE int StrPrintf(TMStrA buf, int size, TMCStrA fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int n = StrVPrintf(buf, size, fmt, va);
    va_end(va);
    return n;
}
TM_INLINE int StrPrintf(TMStrW buf, int size, TMCStrW fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int n = StrVPrintf(buf, size, fmt, va);
    va_end(va);
    return n;
}

#ifdef TM_MSC
#   pragma warning (pop)
#endif // #ifdef TM_MSC



/**
 *  @function           StrFromL StrFromH StrFromF
 *  @param d[in]        number to be converted.
 *  @param p[in,out]    string result.
 *  @result             string result length.
 */
TM_INLINE int StrFromL(long d, TMCharA* p) { return StrPrintf(p, 32, "%ld", d); }
TM_INLINE int StrFromL(long d, TMCharW* p) { return StrPrintf(p, 32, L"%ld", d); }
TM_INLINE int StrFromH(TMLParam d, TMCharA* p) { return StrPrintf(p, 32, TM_SW_PLATFORM64("%llx", "%x"), d); }
TM_INLINE int StrFromH(TMLParam d, TMCharW* p) { return StrPrintf(p, 32, TM_SW_PLATFORM64(L"%llx", L"%x"), d); }
TM_INLINE int StrFromF(double d, TMCharA* p, int nPrecision = FLT_DIG) { return StrPrintf(p, 32, "%.*g", nPrecision, d); }
TM_INLINE int StrFromF(double d, TMCharW* p, int nPrecision = FLT_DIG) { return StrPrintf(p, 32, L"%.*g", nPrecision, d); }


/** @} */ // @addtogroup string




///////////////////////////////////////////////////////////////////////////////
// c++ function
#ifdef __cplusplus


/**
 *  @function StrFill
 *  fill dest string by special string or char. not auto append '\0';
 *  @param pd     pointer to dest string; must valid;
 *  @param ps     pointer to src string; must valid;
 *  @param ns     src string length; must valid;
 *  @result       return length that filled;
*/
TM_INLINE size_t StrFill(TMStrA pd, TMCStrA ps, size_t ns) { DbgAssert(!ns || (pd && ps)); std::copy(ps, ps + ns, pd); return ns; }
TM_INLINE size_t StrFill(TMStrW pd, TMCStrW ps, size_t ns) { DbgAssert(!ns || (pd && ps)); std::copy(ps, ps + ns, pd); return ns; }
TM_INLINE size_t StrFill(TMStrA pd, TMCStrW ps, size_t ns) { DbgAssert(!ns || (pd && ps)); return StrW2A(CP_DEFAULT, pd, -1, ps, ns); }
TM_INLINE size_t StrFill(TMStrW pd, TMCStrA ps, size_t ns) { DbgAssert(!ns || (pd && ps)); return StrA2W(CP_DEFAULT, pd, -1, ps, ns); }
TM_INLINE size_t StrFill(TMStrA pd, TMCharA  ch, size_t nc) { DbgAssert(!nc || pd); std::fill_n(pd, nc, ch); return nc; }
TM_INLINE size_t StrFill(TMStrW pd, TMCharW  ch, size_t nc) { DbgAssert(!nc || pd); std::fill_n(pd, nc, ch); return nc; }


/**
 *  @function StrAssign
 *  fill dest string by special string or char. auto append '\0';
 *  @param pd     pointer to dest string; must valid;
 *  @param nd     dest buffer size; must valid. can be -1;
 *  @param ps     pointer to src string; must valid;
 *  @param ns     src string length; must valid;
 *  @result       return length that filled;
*/
template <class T1, class T2>
TM_INLINE size_t _StrAssign(T1 pd, size_t nd, T2 ps, size_t ns)
{
    if (!nd)
        return 0;
    size_t n = StrFill(pd, ps, tm_min(nd-1, ns));
    DbgAssert(pd);
    pd[n] = 0;
    return n;
}

TM_INLINE size_t StrAssign(TMStrA pd, size_t nd, TMCStrW ps, size_t ns)
{
    if (!nd)
        return 0;
    DbgAssert(!ns || (pd && ps));
    size_t n = StrW2A(CP_DEFAULT, pd, nd, ps, ns);
    DbgAssert(pd);
    pd[n] = 0;
    return n;
}

TM_INLINE size_t StrAssign(TMStrW pd, size_t nd, TMCStrA ps, size_t ns)
{
    if (!nd)
        return 0;
    DbgAssert(!ns || (pd && ps));
    size_t n = StrA2W(CP_DEFAULT, pd, nd, ps, ns);
    DbgAssert(pd);
    pd[n] = 0;
    return n;
}

TM_INLINE size_t StrAssign(TMStrA pd, size_t nd, TMCStrA ps, size_t ns) { return _StrAssign(pd, nd, ps, ns); }
TM_INLINE size_t StrAssign(TMStrW pd, size_t nd, TMCStrW ps, size_t ns) { return _StrAssign(pd, nd, ps, ns); }
TM_INLINE size_t StrAssign(TMStrA pd, size_t nd, TMCharA  ch, size_t nc) { return _StrAssign(pd, nd, ch, nc); }
TM_INLINE size_t StrAssign(TMStrW pd, size_t nd, TMCharW  ch, size_t nc) { return _StrAssign(pd, nd, ch, nc); }


// Str2BSTR
#ifdef TM_WIN

TM_INLINE BSTR Str2BSTR(TMCStrW str, UINT size)
{
    return SysAllocStringLen(str, size);
}

TM_INLINE BSTR Str2BSTR(TMCStrA str, UINT size)
{
    if (size == 0 || str == 0)
        return Str2BSTR((TMCStrW)0, 0);
    LPWSTR p = (LPWSTR)alloca((size + 1) * sizeof(TMCharW));
    int n = StrFill(p, str, size);
    return Str2BSTR(p, n);
}

#endif // TM_WIN


#undef _FIMP5
#undef _FIMP6


#undef _FP3
#undef _FP4
#undef _FP5
#undef _FP6
#undef _FI3
#undef _FI4
#undef _FI5
#undef _FI6


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// less function, can use in std::map, set
struct LessString
{
    template<class T> // T is char or wchar_T
    bool operator()(const T* __x, const T* __y) const
    {
        return StrCompare(__x, -1, __y, -1) < 0;
    }
};




TM_END_NAMESPACE



#endif // __cplusplus

#endif // end of TMTCHAR_H

