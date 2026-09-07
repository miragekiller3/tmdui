/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.10.28
 *
 *  Copyright (C) 2009 miragekiller
 */

#pragma once

#ifndef TMREGKEY_H
#define TMREGKEY_H

#include "tmdef.h"

#ifdef TM_WIN
#   include "tmregkey_win.h"
#elif defined (TM_APPLE)
#   include "mtregkey.h"
#else

#endif //#ifdef TM_WIN


TM_BGN_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
// RegKey DDX
struct REG_DDX_LOAD
{
    CRegKey m_key;

    ~REG_DDX_LOAD () { m_key.Close(); }

    void Init (TMHReg hRoot, TMCStr pSection)
    {
        m_key.Close();
        m_key.OpenExisting (hRoot, pSection, m_key.e_kaRead);
    }

private:
    void GetStr (TMCStr pKey, TMStr pVar, TMULong dwLen)
    {
        m_key.QueryValue (pKey, pVar, dwLen);
    }
    
#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
    void GetStr (TMCStr pKey, _CSTRING_NS::CString& str, TMULong)
    {
        m_key.QueryValue (pKey, str);
    }
#endif

    void GetStr (TMCStr pKey, CString& str, TMULong)
    {
        m_key.QueryValue (pKey, str);
    }

#ifdef __ATLBASE_H__
    void GetStr (TMCStr pKey, CComBSTR& str, TMULong dwLen)
    {
        m_key.QueryValue (pKey, str);
    }
#endif

public:
    template < class T >
    void DDX_Text (TMCStr pKey, T& var, TMULong dwLen)
    {
        if (m_key)
            GetStr (pKey, var, dwLen);
    }
    
    template < class T >
    void DDX_Int (TMCStr pKey, T& nVar)
    {
        if (m_key)
        {
            TMULong dwRet = nVar;
            m_key.QueryValue (pKey, &dwRet);
            nVar = (T)dwRet;
        }
    }
    
    template < class T >
    void DDX_Struct (TMCStr pKey, T& var)
    {
        if (m_key)
            m_key.QueryBinary (pKey, (TMUInt8*)&var, sizeof(T));
    }

    void DDX_Float (TMCStr pKey, float& fVar)
    {
        DDX_Struct (pKey, fVar);
    }

    void DDX_Double (TMCStr pKey, double& fVar)
    {
        DDX_Struct (pKey, fVar);
    }
};


struct REG_DDX_SAVE
{
    CRegKey m_key;

    ~REG_DDX_SAVE () { m_key.Close(); }

    void Init (TMHReg hRoot, TMCStr pSection)
    {
        m_key.Close();
        m_key.OpenAlways (hRoot, pSection, m_key.e_kaReadWrite);
    }

private:
    void PutStr (TMCStr pKey, TMCStr pVar)
    {
        m_key.SetValue (pKey, pVar);
    }

    void PutStr (TMCStr pKey, const CString& str)
    {
        PutStr (pKey, str.c_str());
    }

public:
    template < class T >
    void DDX_Text (TMCStr pKey, T& var, TMULong dwLen)
    {
        if (m_key)
            PutStr (pKey, var);
    }
    
    void DDX_Int (TMCStr pKey, TMULong dwVar)
    {
        if (m_key)
        {
            m_key.SetValue (pKey, dwVar);
        }
    }
    
    template < class T >
    void DDX_Struct (TMCStr pKey, T& var)
    {
        if (m_key)
            m_key.SetValue (pKey, (const TMUInt8*)&var, sizeof(T));
    }

    void DDX_Float (TMCStr pKey, float& fVar)
    {
        DDX_Struct (pKey, fVar);
    }

    void DDX_Double (TMCStr pKey, double& fVar)
    {
        DDX_Struct (pKey, fVar);
    }
};


TM_END_NAMESPACE


#define BGN_REG_DDX_MAP(thisClass) \
    TMBool RegExchangeLoad () { TM::REG_DDX_LOAD _l; return DoRegExchange(_l); } \
    TMBool RegExchangeSave () { TM::REG_DDX_SAVE _s; return DoRegExchange(_s); } \
    template < class T > TMBool DoRegExchange (T& rd) {

#define REG_DDX_SECTION(root,section)   rd.Init (root, section);

#define REG_DDX_TEXT(key,var)           rd.DDX_Text (key, var, sizeof(var));

#define REG_DDX_INT(key,var)            rd.DDX_Int (key, var);

#define REG_DDX_FLOAT(key,var)          rd.DDX_Float (key, var);

#define REG_DDX_DOUBLE(key,var)         rd.DDX_Double (key, var);

#define REG_DDX_STRUCT(key,var)         rd.DDX_Struct (key, var);

#define END_REG_DDX_MAP()               return TRUE; }



#endif // #ifndef MTREGKEY_H
