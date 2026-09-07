/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.3.30
 *
 *  Copyright (C) 2016 miragekiller
 */

#pragma once

#ifndef TMXMLSTRING_H
#define TMXMLSTRING_H

#include "tmxml.h"


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// XmlStrToLong, XmlStrFromLong                       "123456789" <-> long
// XmlStrToHex = XmlStrToLong, XmlStrFromHex          "#abcd0123" <-> long
inline void XmlStrToLong(CStrView str, long& x)
{
    if (str.IsEmpty())
        return;
    if (str[0] == '#')
        StrScanLong(str.data() + 1, str.length() - 1, x, 16);
    else
        StrScanLong(str.data(), str.length(), x, 10);
}

inline void XmlStrToLong(CStrView str, bool& x)
{
    long y = x;
    XmlStrToLong(str, y);
    x = (0 != y);
}

template <class T>
inline void XmlStrToLong(CStrView str, T& x, ...)
{
    long y = x;
    XmlStrToLong(str, y);
    x = static_cast<T>(y);
}

inline CString XmlStrFromHex(long x)
{
    TM::CString str;
    str.Format(TM_T("#%X"), x);
    return std::move(str);
}

inline CString XmlStrFromLong(long x)
{
    return std::move(TM::L2TT<TMCharT>(x));
}


///////////////////////////////////////////////////////////////////////////////
// "*" <-> 42; "" <-> 0
inline void XmlStrToChar(CStrView str, wchar_t& x)
{
    x = str.IsEmpty() ? 0 : str[0];
}

inline CString XmlStrFromChar(wchar_t x)
{
    return CString(&x, 1);
}


///////////////////////////////////////////////////////////////////////////////
// RGB(a0,b0,c0) <-> #a0b0c0
// RGB(rr,gg,bb) <-  #rgb
// RGB(rr,gg,bb) <-  rgb(rr,gg,bb)
// RGB(255,0,51) <-  rgb(100%,0%,20%)
// RGB(255,0,0)  <-  red
inline void XmlStrToColor(CStrView str, TMColor& clrx)
{
    clrx = XmlParseColor(str);
}

inline CString XmlStrFromColor(TMColor clr)
{
    CString str;
    if ((clr & 0xff000000) == 0xff000000)
        str.Format(TM_T("#%06X"), 0x00ffffff & clr);
    else
        str.Format(TM_T("#%08X"), clr);
    return std::move(str);
}


///////////////////////////////////////////////////////////////////////////////
// 1, 2 <-> "1,2"
template <class T> inline void XmlStrToInts(CStrView str, T* p, int n)
{
    for (int i = 0; i < n; ++i)
    {
        T x = 0;
        str = str.ScanLong(x).Mid(1);
        *p++ = x;
    }
}

template <class T> inline CString XmlStrFromInts(const T* p, int n)
{
    DbgAssert(n > 0);
    CString str;
    str.Format(TM_T("%d"), (int)p[0]);
    for (int i = 1; i < n; ++i)
        str.AppendFormat(TM_T(",%d"), (int)p[i]);
    return std::move(str);
}

template <class T> inline static CString XmlStrFromInts(const T& obj)
{
    typedef typename T::value_type value_type;
    return std::move(XmlStrFromInts((const value_type*)&obj, sizeof(T) / sizeof(value_type)));
}

template <class T> inline static void XmlStrToInts(CStrView str, T& obj)
{
    typedef typename T::value_type value_type;
    XmlStrToInts(str, (value_type*)&obj, sizeof(T) / sizeof(value_type));
}

TM_END_NAMESPACE


#endif //TMXMLSTRING_H
