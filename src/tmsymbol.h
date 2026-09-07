/*
 *  @file
 *  @brief  output the enumeration and mask value in string
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.10.08
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef TMSYMBOL_H 
#define TMSYMBOL_H

#include "tmtchar.h"


#ifdef TM_CPP

TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// handle enumeration objects 
struct CSymbolEnum { TMCStr pName; unsigned long nVal; };

template<class T> struct CEnumSymbolT
{
    //struct CMask { TMCStr pName; unsigned long nVal; };
    typedef CSymbolEnum CMask;

    inline static TMCStr GetNameByIndex(int nIndex)
    {
        return T::Get(nIndex).pName;
    }

    inline static unsigned long GetValByIndex(int nIndex)
    {
        return T::Get(nIndex).nVal;
    }

    inline static unsigned long GetMaskByIndex(int nIndex)
    {
        return T::e_nMask;
    }

    inline static bool CheckVal(int nIndex, unsigned long dwVal)
    {
        CMask& m = T::Get(nIndex);
        return (dwVal & T::e_nMask) == (m.nVal & T::e_nMask);
    }
    
    inline static TMCStr GetName(int nVal)
    {
        nVal &= GetMask();
        for (int i = 0; ; ++i)
        {
            CMask& m = T::Get(i);
            if ((m.nVal & GetMask()) == nVal || 0 == m.pName)
                return m.pName;
        }
    }

    inline static TMBool GetValByName(TMCStr pName, unsigned long& val)
    {
        if (!pName)
            return false;

        for (int i = 0; ; ++i)
        {
            CMask& m = T::Get(i);
            if (0 == m.pName)
                return false;
            if (StrCompareI(pName, m.pName) == 0)
            {
                val = m.nVal & GetMask();
                return true;
            }
        }
    }

    inline static unsigned long GetMask()
    {
        return T::e_nMask;
    }
};

#define TM_BGN_ENUM_SYMBOL2(classname,nMask) \
    struct classname { \
        enum { e_nMask = nMask }; \
        static CSymbolEnum& Get (int n) { \
            static CSymbolEnum p[] = { 

#define TM_BGN_ENUM_SYMBOL(classname)   TM_BGN_ENUM_SYMBOL2(classname, -1)
#define TM_ENUM_SYMBOL(nVal)            { TM_T(#nVal), nVal },
#define TM_ENUM_SYMBOL2(str,nVal)       { TM_T(#str), nVal },
#define TM_ENUM_SYMBOL_NS(ns,nVal)      { TM_T(#nVal), ns::nVal },
#define TM_END_ENUM_SYMBOL()            {0,0}, }; return p[n]; } };



////////////////////////////////////////////////////////////////////////////////
// handle mask objects
struct CSymbolMask
{
    TMCStr pName;
    unsigned long nMask;
    unsigned long nVal;
};

struct CSymbolMaskNull
{
    static CSymbolMask& Get(int) { static CSymbolMask p = {0}; return p; }
};

template < class T > struct CMaskSymbolT
{
    //struct CMask { TMCStr pName; unsigned long nMask; unsigned long nVal; };
    typedef CSymbolMask CMask;

    inline static TMCStr GetNameByIndex(int nIndex)
    {
        return T::Get(nIndex).pName;
    }

    inline static unsigned int GetValByIndex(int nIndex)
    {
        return T::Get(nIndex).nVal;
    }
    
    inline static unsigned int GetMaskByIndex(int nIndex)
    {
        return T::Get(nIndex).nMask;
    }

    inline static bool CheckVal(int nIndex, unsigned long dwVal)
    {
        CMask& m = T::Get (nIndex);
        return (dwVal & m.nMask & GetMask()) == m.nVal;
    }
    
    inline static unsigned long _getmask()
    {
        unsigned long m = 0;
        for (int i = 0; ; ++i)
        {
            CMask& msk = T::Get(i);
            unsigned long n = msk.nMask;
            if (n == 0 && !msk.pName)
                return m;
            m |= n;
        }
    }

    inline static unsigned long GetMask()
    {
        static unsigned long m = _getmask();
        return m;
    }
};

#define TM_BGN_MASK_SYMBOL2(classname,base) \
    struct classname { \
        typedef base base_class; \
        static CSymbolMask& Get (int n) { \
        static CSymbolMask p[] = { 

#define TM_BGN_MASK_SYMBOL(classname) TM_BGN_MASK_SYMBOL2(classname,CSymbolMaskNull)


#define TM_MASK_SYMBOL(nVal)                { TM_T(#nVal), nVal, nVal },
#define TM_MASK_SYMBOL2(nVal, nMask)        { TM_T(#nVal), nMask, nVal },
// #define TM_MASK_SYMBOL3(name, nVal, nMask)  { name, nMask, nVal },
#define TM_MASK_BREAK()                     { TM_T(""), 0, 0 },

#define TM_END_MASK_SYMBOL() \
    }; \
    enum { eSize = sizeof(p) / sizeof(CSymbolMask) }; \
    return n < eSize ? p[n] : base_class::Get(n - eSize); } };


TM_END_NAMESPACE

#else

#   define TM_BGN_ENUM_SYMBOL(class)
#   define TM_BGN_ENUM_SYMBOL2(class,base)
#   define TM_ENUM_SYMBOL(x)
#   define TM_END_ENUM_SYMBOL()

#   define TM_BGN_MASK_SYMBOL(class)
//#   define TM_BGN_MASK_SYMBOL2(class,n)
#   define TM_MASK_SYMBOL(x)
#   define TM_MASK_SYMBOL2(x,y)
#   define TM_END_MASK_SYMBOL()

#endif // #ifdef TM_CPP


#endif // TMSYMBOL_H
