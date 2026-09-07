/**
 *  @file
 *  @brief  CString
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2002.07.23
 *
 *  Copyright (C) 2002 miragekiller
 *
 *  notice :
 *      in gcc, you must add #define _GNU_SOURCE 1 into file
 *      or g++ -D_GNU_SOURCE to support isoc99
 */

///////////////////////////////////////////////////////////////////////////////
// attention:
// all param use "LPCSTR" not support string like "a\0b"
// all param use "CString" or "CStrView" support string like "a\0b"
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// vc6 std::string have bug:
// std::string str1("axd\0c", 5);
// std::string str2(str1);
// str2.resize(5); // after resize, the str2[4] lost;

///////////////////////////////////////////////////////////////////////////////
//         core                      assign(const String&)  assign(other string)
// String  { str* + ref + str_view } shared memory          alloc memory
// WeakStr { str* + ref + str_view } shared memory          str_view
// StdStr  : public std::string{}    alloc memory           alloc memory
// StrView : public std::str_view{}  str_view               str_view


#pragma once

#ifndef TMSTRING_H
#define TMSTRING_H

#ifdef __cplusplus

#include "tmstrptr.h"
#include <algorithm>

#ifdef TM_WIN
#   include <windows.h>
// #   include <shlwapi.h>
#   include <objidl.h>
#elif defined (TM_APPLE)
#   include <CoreFoundation/CFString.h>
#endif


#ifdef TM_APPLE

#include <CoreFoundation/CFString.h>

//TM_Interface NSString;

#endif // TM_APPLE


TM_BGN_NAMESPACE


template < class T > struct string_traits;


TM_TEMPLATE_NULL struct string_traits<TMCharA>
{
    typedef TMCharA                                 value_type;
    typedef TMCharW                                 other_char;
    typedef std::string                             std_string;
};

TM_TEMPLATE_NULL struct string_traits<TMChar16>
{
    typedef TMChar16                                value_type;
    typedef TMCharA                                 other_char;
    typedef std::wstring                            std_string;
};

TM_TEMPLATE_NULL struct string_traits<TMChar32>
{
    typedef TMChar32                                value_type;
    typedef TMCharA                                 other_char;
    typedef std::wstring                            std_string;
};


////////////////////////////////////////////////////////////////////////////////
// _cstring_mode_XXX
template <class CharT>
struct _cstring_mode_strong
{
    typedef CStrPtrBaseT<CharT>                         native_string;
    typedef CStrPtrBaseT<CharT>                         _base_class;
    typedef _CString_Funcs<native_string>               _funcs;
    tm_tpl_cexpr11 static inline void alloc(P& s) { s.alloc(); }
};

template <class CharT>
struct _cstring_mode_weak
{
    typedef CStrPtrBaseT<CharT>                         native_string;
    typedef CStrPtrBaseT<CharT>                         _base_class;
    typedef _CString_Funcs<native_string>               _funcs;
    tm_tpl_cexpr11 static inline void alloc(P& s) tm_noexcept {}
};

template <class CharT>
struct _cstring_mode_view : public std::_tm_string_view<CharT>
{
    typedef std::_tm_string_view<CharT>                 native_string;
    typedef _cstring_mode_view                          _base_class;
    typedef _cstring_mode_view                          class_type;
    typedef std::basic_string<CharT>                    std_string;
    typedef native_string                               std_view;
    typedef _CString_Funcs<class_type>                  _funcs;

    // construct
    tm_constexpr11 _cstring_mode_view()                             {}
    tm_constexpr17 _cstring_mode_view(const CharT* p)               : std_view(p ? p : (const CharT*)L"\0" ) {}
    tm_constexpr11 _cstring_mode_view(size_t n, const CharT* p)     : std_view(p, n) {}
    tm_constexpr11 _cstring_mode_view(const std_string& r)          : std_view(r) {}
    tm_constexpr11 _cstring_mode_view(const std_view& r)            : std_view(r) {}
    tm_tpl_cexpr11 _cstring_mode_view(string_constant<P> a)         : std_view(a.data(), a.size()) {}

    // helper function
    tm_tpl_cexpr11 static inline void alloc(P& s) tm_noexcept {}

    // stand function
    tm_constexpr11 const CharT& operator[](int pos) const { return this->at(pos); }
    tm_constexpr11 operator bool() const tm_noexcept { return !this->empty(); }
    tm_constexpr11 bool operator !() const tm_noexcept { return this->empty(); }
    void resize(size_t count) { DbgAssert(count < this->length()); assign(this->data(), count); }
    void clear() { resize(0); }

    // assign
    tm_constexpr11 void assign(const std_view& x) { *this = x; }
    tm_constexpr11 void assign(const std_string& rhs) { assign(std_view(rhs.data(), rhs.length())); }
    tm_constexpr11 void assign(const CharT* p, size_t _count) { assign(StrMakeView(p, _count)); }
    tm_constexpr17 void assign(const CharT* p) { assign(StrMakeView(p)); }
    tm_constexpr11 void assign(const CharT* _first, const CharT* _last) { assign(_first, _last - _first); }
};

template <class CharT>
struct _cstring_mode_std : public std::basic_string<CharT>
{
public:
    typedef std::basic_string<CharT>                    native_string;
    typedef std::basic_string<CharT>                    std_string;
    typedef CStrPtrBaseT<CharT>                         ptr_string;
    typedef std::_tm_string_view<CharT>                 std_view;
    typedef _cstring_mode_std                           _base_class;
    typedef _CString_Funcs<native_string>               _funcs;

    typedef typename std_string::size_type              size_type;
    typedef typename std_string::iterator               iterator;
    typedef typename std_string::const_iterator         const_iterator;

    using   std_string::npos;

    // construct
    tm_constexpr11 _cstring_mode_std()                              {}
    tm_constexpr20 _cstring_mode_std(const CharT* p)                : std_string(p ? p : (const CharT*)L"\0" ) {}
    tm_constexpr20 _cstring_mode_std(size_t n, const CharT* p)      : std_string(p, n) {}
    tm_constexpr20 _cstring_mode_std(size_t n, CharT c)             : std_string(n, c) {}
    tm_constexpr20 _cstring_mode_std(const std_string& r)           : std_string(r) {}
    tm_constexpr20 _cstring_mode_std(const std_view& v)             : std_string(v.data(), v.length()) {}
    tm_tpl_cexpr20 _cstring_mode_std(string_constant<P> a)          : std_string(a.data(), a.size()) {}
    tm_tpl_cexpr20 _cstring_mode_std(P b, P e, ...)                 : std_string(b, e) { DbgAssertS(sizeof(*b) ==sizeof(*e)); }

    tm_tpl_cexpr20 _cstring_mode_std(const P* p, ...)               { _funcs::__assign(*this, StrMakeView(p)); }
    tm_tpl_cexpr20 _cstring_mode_std(size_t n, const P* p, ...)     { _funcs::__assign(*this, p, n); }
    tm_tpl_cexpr20 _cstring_mode_std(const std::basic_string<P>& r, ...)    { _funcs::__assign(*this, r); }
    tm_tpl_cexpr20 _cstring_mode_std(const std::_tm_string_view<P>& v, ...) { _funcs::__assign(*this, v); }

    // right value reference
#if TM_CPP >= TM_CPP11
    tm_constexpr11 _cstring_mode_std(std_string&& r)    tm_noexcept : std_string(std::move(r)) {}
    tm_constexpr11 _cstring_mode_std(ptr_string&& r)    tm_noexcept { __assign_r(r); }
    tm_constexpr11 void __assign_r(std_string& r)       tm_noexcept { std_string::assign(std::move(r)); }
#else
    tm_constexpr11 void __assign_r(std_string& r)       tm_noexcept { this->swap(r); }
#endif // TM_CPP >= TM_CPP11
    tm_constexpr11 void __assign_r(ptr_string& r)       tm_noexcept { std::swap(r, *this); }
    tm_tpl_cexpr20 void __assign_r(const P& r, ...)     tm_noexcept { _funcs::__assign(*this, r); }

    // const_expr string
    tm_constexpr11 void __assign_c(const CharT* p, size_t n) tm_noexcept { this->assign(p, n); }

    // helper function
    template <class T> tm_constexpr11 static inline void alloc(T& s) {}

    template <class T_custom> // CCoSTR; CComBSTR; vector<Char>
    void MoveFromCustomString(T_custom& str)
    {
        this->assign(std::data(str), std::size(str));
        T_custom _temp;
        std::swap(_temp, str);
    }

    // stand function
    void freeze() { this->begin(); }
    tm_constexpr11 const CharT& operator[](int pos) const { return this->at(pos); }
    CharT& operator[](int pos) { return this->begin()[pos]; }
    operator const CharT*() const tm_noexcept { return this->empty() ? 0 : this->data(); }

#if TM_CPP < TM_CPP11
    void clear()            { erase(); }
    const_iterator cbegin() const { return this->begin(); }
    const_iterator cend()   const { return this->end(); }
    using std_string::insert;
    void insert(const_iterator it, const_iterator first, const_iterator last) { std_string::insert(const_cast<iterator>(it), first, last); }
    void insert(const iterator it, size_type n, CharT c) { std_string::insert(const_cast<iterator>(it), n, c); }
    iterator insert(const_iterator it, CharT c) { return std_string::insert(const_cast<iterator>(it), c); }
#endif

#if TM_CPP < TM_CPP17
    using std_string::replace;
    std_string& replace(size_type pos, size_type count, const std_view& t) { return std_string::replace(pos, count, t.data(), t.size()); }
    std_string& replace(const_iterator first, const_iterator last, const std_view& t) { return std_string::replace(const_cast<iterator>(first), const_cast<iterator>(last), t.data(), t.size()); }
    std_string& replace(size_type pos, size_type count, const std_view& t, size_type pos2, size_type count2 = npos) { return std_string::replace(pos, count, t.data(), t.size(), pos2, count2); }
#endif

};

TM_END_NAMESPACE

#define _CSTRING_MODE_VIEW      0
#define _CSTRING_MODE_STD       1
#define _CSTRING_MODE_STRONG    2
#define _CSTRING_MODE_WEAK      3

# define CharT TMCharA
# define _CSTRING_MODE _CSTRING_MODE_VIEW
#include "tmstring_i.h"

# define CharT TMCharA
# define _CSTRING_MODE _CSTRING_MODE_WEAK
#include "tmstring_i.h"

# define CharT TMCharA
# define _CSTRING_MODE _CSTRING_MODE_STRONG
#include "tmstring_i.h"

# define CharT TMCharA
# define _CSTRING_MODE _CSTRING_MODE_STD
#include "tmstring_i.h"

# define CharT TMCharW
# define _CSTRING_MODE _CSTRING_MODE_VIEW
#include "tmstring_i.h"

# define CharT TMCharW
# define _CSTRING_MODE _CSTRING_MODE_WEAK
#include "tmstring_i.h"

# define CharT TMCharW
# define _CSTRING_MODE _CSTRING_MODE_STRONG
#include "tmstring_i.h"

# define CharT TMCharW
# define _CSTRING_MODE _CSTRING_MODE_STD
#include "tmstring_i.h"


#undef _CSTRING_MODE_VIEW
#undef _CSTRING_MODE_STD
#undef _CSTRING_MODE_STRONG
#undef _CSTRING_MODE_WEAK


TM_BGN_NAMESPACE



template <int nscp, int ndcp>
class A2AT : public CStdStrA
{
private:
    void _convert(TMCStrA p, int nLen)
    {
        DbgAssert(nLen ? 0 != p : 1);
        int nBuf = nLen * 4;
        int n = StrConvert(nscp, this->GetBuffer(nBuf+1), nBuf, ndcp, p, nLen);
        this->SetLength (n);
    }

public:
    A2AT(TMCStrA p)
    {
        if (p)
            _convert(p, StrLen(p));
    }

    A2AT(const std::basic_string<TMStrA>& str)
    {
        _convert(str.data(), str.length());
    }

//    A2AT (CRangeT<TMStrA> str)
//    {
//        int n = str.GetSize();
//        _convert (str.GetHead(), n);
//    }

    A2AT(const std::_tm_string_view<TMStrA>& str)
    {
        int n = str.length();
        _convert(str.data(), n);
    }
};

template <int ncp, class T1, class T2>
class A2BT : public CStringT<T2, _cstring_mode_std<T2> >
{
public:
    typedef CStringT<T2, _cstring_mode_std<T2> > base_class;
    using base_class::GetBuffer;
    using base_class::SetLength;

private:
    void _convert(const std::_tm_string_view<T1>& v)
    {
        int nLen = v.length();
        int nBuf = sizeof(T2) > sizeof(T1) ? nLen : nLen * 4;
        int n = StrConvert(ncp, GetBuffer(nBuf+1), nBuf, v.data(), nLen);
        SetLength(n);
    }

public:
    A2BT(const T1* p)
    {
        if (p)
            _convert(StrMakeView(p));
    }

    A2BT(const T1* p, int nLen)
    {
        if (p)
            _convert(std::_tm_string_view<T1>(p, nLen));
    }

    A2BT(const std::basic_string<T1>& str)
    {
        _convert(str);
    }

    A2BT(const std::_tm_string_view<T1>& str)
    {
        _convert(str);
    }

//     A2BT (CRangeT<const T1*> str)
//     {
//         int n = str.GetSize();
//         _convert (str.GetHead(), n);
//     }
};

class UTFOrA2W : public CStdStrW
{
public:
    TMBool m_bUtf;

private:
    void _convert(const std::string_view& view)
    {
        int nLen = view.size();
        int nBuf = nLen * 4;
        TMStrW pBuf = this->GetBuffer(nBuf + 1);
        int n = StrAuto2W(pBuf, nBuf, view.data(), nLen, &m_bUtf);
        this->SetLength(n);
    }

public:
    UTFOrA2W(TMCStrA p)
    {
        if (p)
            _convert(StrMakeView(p));
    }

    UTFOrA2W(TMCStrA p, int nLen)
    {
        _convert(std::string_view(p, nLen));
    }

    UTFOrA2W(const std::string& str)
    {
        _convert(str);
    }

    UTFOrA2W(const std::string_view& str)
    {
        _convert(str);
    }

//     UTFOrA2W (CRangeT<TMCharA> str)
//     {
//         _convert(str);
//     }

    TMBool IsUtf() const { return m_bUtf; }
};


struct URL2A : public CStdStrA
{
    URL2A(TMCStrA p)
    {
        if (p)
        {
            int n = StrLen(p);
            n = StrUrlDec(p, this->GetBuffer(StrUrlDecSize(n)), n);
            resize(n);
        }
    }
};

struct A2URL : public CStdStrA
{
    A2URL(TMCStrA p)
    {
        if (p)
        {
            int n = StrLen(p);
            n = StrUrlEnc(p, this->GetBuffer(StrUrlEncSize(n)), n);
            resize(n);
        }
    }
};

template <class T>
struct L2TT : public CStringT<T, _cstring_mode_std<T> >
{
    L2TT(long n, TMBool haveCommas = false)
    {
        StrFromL(n, this->GetBuffer(64));
        this->ReleaseBuffer();
        if (haveCommas)
        {
            for (int i = this->length() - 3; i > 0; i -= 3)
                this->insert(i, 1, ',');
        }
    }
};

template <class T>
struct Hex2TT : public CStringT<T, _cstring_mode_std<T> >
{
    Hex2TT(TMLParam n)
    {
        StrFromH(n, this->GetBuffer(64));
        this->ReleaseBuffer();
    }
};

template <class T>
struct F2TT : public CStringT<T, _cstring_mode_std<T> >
{
    F2TT(double n)
    {
        int sz = StrFromF(n, this->GetBuffer(64));
        this->SetLength(sz);
    }
};

template <class T>
struct VL2TT : public CStringT<T, _cstring_mode_std<T> >
{
    VL2TT(const T* fmt, va_list vl)
    {
        this->FormatV(fmt, vl);
    }
};

template <class T>
struct VA2TT : public CStringT<T, _cstring_mode_std<T> >
{
    VA2TT (const T* fmt, ...)
    {
        va_list va;
        va_start(va, fmt);
        this->FormatV(fmt, va);
        va_end(va);
    }
};


#ifdef TM_APPLE

class CFStr
{
protected:
    CFStringRef m_pStr;

public:
    CFStr (CFStringRef str)
        : m_pStr (str)
    {
        if (str)
            CFRetain (str);
    }

//    CFStr (NSString* str)
//        : m_pStr ((CFStringRef)str)
//    {
//        if (str)
//            CFRetain (str);
//    }

    CFStr (TMCStrA p, CFStringBuiltInEncodings eEnc = kCFStringEncodingUTF8)
        : m_pStr (CFStringCreateWithCString (0, p, eEnc))
    {
    }

    CFStr (TMCStrW p)
        :  m_pStr (CFStringCreateWithCString (0, CStringA(p), kCFStringEncodingUTF8))
    {
    }

    ~CFStr ()
    {
        if (m_pStr)
            CFRelease (m_pStr);
    }

    TMBool IsEmpty() const { return !m_pStr; }
};

class T2FS
{
public:
    CFStringRef m_pStr;

public:
    T2FS(TMCStrA str)
        : m_pStr(CFStringCreateWithCStringNoCopy(0, str, kCFStringEncodingUTF8, 0))
    {
    }

    ~T2FS()
    {
        if (m_pStr)
            CFRelease(m_pStr);
    }

    operator CFStringRef () const { return m_pStr; }
};


#endif // TM_APPLE

// template < class _S >
//     _S& operator << (_S& __stream, const CStringA& __str)
// {
//     if (__str.IsEmpty ())
//         __stream << (TMUInt32) 0;
//     else
//     {
//         TMUInt32 dwLen = __str.GetLength() + 1;
//         __stream << dwLen;
//         __stream.Write (__str.data (), dwLen);
//     }
//     return __stream;
// }
//
//
//
// template < class _S >
//     _S& operator << (_S& __stream, const CStringW& __str)
// {
//     if (__str.IsEmpty ())
//         __stream << (TMUInt32) 0;
//     else
//     { // on some platforms unicode is 4 bytes,here  all are transformed to 2 bytes
//         TMUInt32 dwLen = __str.GetLength();
//         __stream << dwLen * 2 + 2;
//         for (TMUInt32 i = 0; i < dwLen; i++)
//             __stream << (TMUInt16) __str.GetAt(i);
//         __stream << (TMUInt16) 0;
//     }
//     return __stream;
// }
//
// template < class _S >
//     _S& operator >> (_S& __stream, CStringA& __str)
// {
//     TMUInt32 dwLen = 0;
//     __stream >> dwLen;
//     __stream.Read (__str.GetBuffer(dwLen), dwLen);
//     return __stream;
// }
//
// template < class _S >
//     _S& operator >> (_S& __stream, CStringW& __str)
// {
//     TMUInt32 dwLen = 0;
//     __stream >> dwLen;
//     dwLen = (dwLen / 2);
//     __str.SetLength (dwLen);
//     for (TMUInt32 i = 0; i < dwLen; i++)
//     {
//         TMUInt16 __tmp;
//         __stream >> __tmp;
//         __str.SetAt (i, __tmp);
//     }
//     return __stream;
// }


TM_END_NAMESPACE


TM_BGN_NAMESPACE

// W:   const wchar*
// A:   const char*
// UTF: utf8*
// L:   long
// H:   long format as hex string
// F:   double
// VL:  variable-argument lists;    like vprintf(fmt, va_list);
// VA:  variable-argument;          like printf(fmt, ...);
// Q:   QT::QString
typedef CStringA                                        W2A;
typedef CStringW                                        A2W;
typedef A2BT<CP_UTF8, TMCharW, TMCharA>                 W2UTF;
typedef A2BT<CP_UTF8, TMCharA, TMCharW>                 UTF2W;
typedef A2AT<CP_DEFAULT, CP_UTF8>                       A2UTF;
typedef A2AT<CP_UTF8, CP_DEFAULT>                       UTF2A;
typedef URL2A                                           URL2A;
typedef A2URL                                           A2URL;
typedef L2TT<TMCharT>                                   L2T;
typedef L2TT<TMCharA>                                   L2A;
typedef L2TT<TMCharW>                                   L2W;
typedef Hex2TT<TMCharT>                                 H2T;
typedef Hex2TT<TMCharA>                                 H2A;
typedef Hex2TT<TMCharW>                                 H2W;
typedef F2TT<TMCharT>                                   F2T;
typedef F2TT<TMCharA>                                   F2A;
typedef F2TT<TMCharW>                                   F2W;
typedef VL2TT<TMCharT>                                  VL2T;
typedef VL2TT<TMCharA>                                  VL2A;
typedef VL2TT<TMCharW>                                  VL2W;
typedef VA2TT<TMCharT>                                  VA2T;
typedef VA2TT<TMCharA>                                  VA2A;
typedef VA2TT<TMCharW>                                  VA2W;

#ifdef TM_UNICODE
    typedef A2W                                         A2T;
    typedef W2A                                         T2A;
    typedef W2UTF                                       T2UTF;
    typedef UTF2W                                       UTF2T;
    template <class T> const T& T2W(const T& v)         { return v; }
    template <class T> const T& W2T(const T& v)         { return v; }
#else
    typedef A2W                                         T2W;
    typedef W2A                                         W2T;
    typedef A2UTF                                       T2UTF;
    typedef UTF2A                                       UTF2T;
    
    template <class T> const T& T2A(const T& v)         { return v; }
    template <class T> const T& A2T(const T& v)         { return v; }
#endif // #ifdef TM_UNICODE




TM_END_NAMESPACE


#define TM_W2A(x)                                       TM::CStringA(x).c_str()
#define TM_A2W(x)                                       TM::CStringW(x).c_str()
#define TM_W2UTF(x)                                     TM::W2UTF(x).c_str()
#define TM_A2UTF(x)                                     TM::A2UTF(x).c_str()
#define TM_UTF2W(x)                                     TM::UTF2W(x).c_str()
#define TM_UTF2A(x)                                     TM::UTF2A(x).c_str()
#define TM_URL2A(x)                                     TM::URL2A(x).c_str()
#define TM_A2URL(x)                                     TM::A2URL(x).c_str()
#define TM_L2T(x)                                       TM::L2TT<TMCharT>(x).c_str()
#define TM_L2A(x)                                       TM::L2TT<TMCharA>(x).c_str()
#define TM_L2W(x)                                       TM::L2TT<TMCharW>(x).c_str()
#define TM_F2T(x)                                       TM::F2TT<TMCharT>(x).c_str()
#define TM_F2A(x)                                       TM::F2TT<TMCharA>(x).c_str()
#define TM_F2W(x)                                       TM::F2TT<TMCharW>(x).c_str()
#define TM_H2T(x)                                       TM::Hex2TT<TMCharT>(x).c_str()
#define TM_H2A(x)                                       TM::Hex2TT<TMCharA>(x).c_str()
#define TM_H2W(x)                                       TM::Hex2TT<TMCharW>(x).c_str()
#define TM_T2FS                                         TM::T2FS
#define TM_A2Q                                          QString::fromUtf8
//#define TM_W2Q(x)                                       QString::fromUtf16((const char16_t*)x)
#define TM_W2Q                                          QString::fromWCharArray
#define TM_W2Q2(x,n)                                    QString::fromWCharArray(x,n)
#define TM_Q2W(x)                                       (reinterpret_cast<const wchar_t *>(x.utf16()))

#ifdef TM_UNICODE
#   define TM_A2T(x)                                    TM_A2W(x)
#   define TM_T2A(x)                                    TM_W2A(x)
#   define TM_T2W(x)                                    (x)
#   define TM_W2T(x)                                    (x)
#   define TM_T2UTF(x)                                  TM_W2UTF(x)
#   define TM_UTF2T(x)                                  TM_UTF2W(x)
#   define TM_T2Q                                       TM_W2Q
#   define TM_T2Q2                                      TM_W2Q2
#   define TM_Q2T                                       TM_Q2W
#else
#   define TM_T2W(x)                                    TM_A2W(x)
#   define TM_W2T(x)                                    TM_W2A(x)
#   define TM_T2A(x)                                    (x)
#   define TM_A2T(x)                                    (x)
#   define TM_T2UTF(x)                                  TM_A2UTF(x)
#   define TM_UTF2T(x)                                  TM_UTF2A(x)
#   define TM_T2Q                                       TM_A2Q
#endif // #ifdef TM_UNICODE



#endif // #ifdef __cplusplus




// #ifdef TM_APPLE
// #   include "mtstring.h"
// #else
// #   define CText CString
// #endif // #ifdef TM_APPLE

#endif // end of TMSTRING_H

