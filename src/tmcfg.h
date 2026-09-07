/*
 *  @file
 *  @brief  windows config file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.01.02
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef TMCFG_H
#define TMCFG_H

#include <windows.h>
#include "tmarchive.h"
#include "tmstring.h"


TM_BGN_NAMESPACE


class CCfgPath
{
private:
    TCHAR m_szPath[MAX_PATH];

public:
    CCfgPath (TMCStr pPath)
    {
        GetFullPathName (pPath,MAX_PATH, m_szPath, 0);
    }

    CCfgPath& operator = (TMCStr pPath)
    {
        GetFullPathName (pPath,MAX_PATH, m_szPath, 0);
        return *this;
    }

    operator TMCStr () const { return m_szPath; }
};


/*
class CCfg;

TM_TEMPLATE_NULL struct VarT<CCfg>
{
    CCfg&   cfg;
    TMCStr key;

    VarT (CCfg& c, TMCStr k)
        : cfg (c)
        , key (k)
    {
    }

    operator int () const { return 0; }
    operator float () const { return 0; }

//    template <int n>
//        operator char[n] () const { char c[n]; return 0; }
};
*/


class CCfg
{
public:
//    typedef VarT<CCfg> TMVarCfg;
    typedef struct CCfgArchiveO CArchiveO;
    typedef struct CCfgArchiveI CArchiveI;

private:
    CString     m_pPath;
    CString     m_pKey;

//    friend TMVarCfg;

public:
    CCfg ()
    {
    }

    CCfg (const CString& pPath)
        : m_pPath (pPath)
    {
    }

    CCfg (const CString& pPath, const CString& pKey)
        : m_pPath (pPath)
        , m_pKey (pKey)
    {
    }

    void Open (const CString& pPath, const CString& pKey)
    {
        m_pPath = pPath;
        m_pKey = pKey;
    }

    void OpenKey (const CString& pKey)
    {
        m_pKey = pKey;
    }

    const CString& GetPath () const { return m_pPath; }

    const CString& GetKey () const { return m_pKey; }

    BOOL DeleteKey ()
    {
        return WritePrivateProfileSection(m_pKey, 0, m_pPath);
    }

/*    TMBool Query (long& l, int nBase = 10)
    {
        TCHAR c[32];
        if (Query (c, 32) == 0)
            return false;
        TCHAR p2;
        long x = _tcstol (c, &p2, nBase);
        if (p2 - c == 0)
            return false;
        else
            return l = x, true;
    }

    TMBool Query (double& d)
    {
        TCHAR c[32];
        if (Query (c, 32) == 0)
            return false;
        TCHAR p2;
        double x = _tcstod (c, &p2);
        if (p2 - c == 0)
            return false;
        else
            return d = x, true;
    }

    TMBool Query (TCHAR* p, size_t n)
    {
        TCHAR c = p[0];
        return GetPrivateProfileString (m_pKey, pSubKey, p, p, n, m_pPath);
    }

    TMVarCfg Query (TMCStr pSubKey)
    {
        return TMVarCfg (*this, pSubKey);
    }
*/
    BOOL QueryValue (TMCStr pSubKey, LPVOID p, size_t n)
    {
        return GetPrivateProfileStruct (m_pKey, pSubKey, p, n, m_pPath);
    }

    UINT QueryValue (TMCStr pSubKey, UINT dft)
    {
        return GetPrivateProfileInt (m_pKey, pSubKey, dft, m_pPath);
    }

    DWORD QueryValue (TMCStr pSubKey, LPTSTR pBuf, size_t n, TMCStr dft)
    {
        return GetPrivateProfileString (m_pKey, pSubKey, dft, pBuf, n, m_pPath);
    }

    BOOL SetValue (TMCStr pSubKey, UINT val)
    {
        TCHAR c[MAX_PATH];
        StrFromL(val, c);
        return SetValue(pSubKey, c);
    }

    BOOL SetValue (TMCStr pSubKey, TMCStr val)
    {
        return WritePrivateProfileString (m_pKey, pSubKey, val, m_pPath);
    }

    BOOL SetValue (TMCStr pSubKey, LPCVOID p, size_t n)
    {
        return WritePrivateProfileStruct (m_pKey, pSubKey, (LPVOID)p, n, m_pPath);
    }
};


////////////////////////////////////////////////////////////////////////////////
// archive
struct CCfgArciveFileBase
{
    typedef void* section_type;
    CCfg& m_file;
    CString m_strKey;
    CCfgArciveFileBase(CCfg& f) : m_file(f) {}
    void DoEntrySection(section_type& storage, const CString& str) { m_file.OpenKey(str); }
    void DoLeaveSection(section_type& storage) {}
    void DoSetKey(const CString& str) { m_strKey = str; }
};

struct CCfgArchiveO
    : public CArchiveTxtOImplT<CCfgArchiveO , TMCharT>
    , public CCfgArciveFileBase
{
public:
    CCfgArchiveO(CCfg& f) : CCfgArciveFileBase(f) {}

    void DoSerializeVoid(const void* p, size_t n)
    {
        m_file.SetValue(m_strKey, p, n);
    }

    void DoSerializeString(const CString& str)
    {
        m_file.SetValue(m_strKey, str);
    }

    void DoSerializeInt(long n)
    {
        m_file.SetValue(m_strKey, n);
    }
};

struct CCfgArchiveI
    : public CArchiveTxtIImplT<CCfgArchiveI, TMCharT>
    , public CCfgArciveFileBase
{
public:
    CCfgArchiveI(CCfg& f) : CCfgArciveFileBase(f) {}

    void DoSerializeVoid(void* p, size_t n)
    {
        this->SetStat(m_file.QueryValue(m_strKey, p, n));
    }

    using CArchiveTxtIImplT<CCfgArchiveI, TMCharT>::DoSerializeString;

    int DoSerializeString(TMCharT* str)
    {
        int n = m_file.QueryValue(m_strKey, str, eDefaultStringBufferSize, 0);
        this->SetStat(n);
        return n;
    }
};


////////////////////////////////////////////////////////////////////////////////
// Profile DDX
struct PROFILE_DDX_LOAD
{
    TMCStr pf; // file
    TMCStr ps; // section

    PROFILE_DDX_LOAD (TMCStr pFile)
        : pf (pFile)
        , ps (0)
    {
    }

    void Init (TMCStr pSection)
    {
        ps = pSection;
    }

private:
    int GetStr (TMCStr pk, LPTSTR pVar, DWORD dwLen, TMCStr pDft)
    {
        return GetPrivateProfileString (ps, pk, pDft, pVar, dwLen, pf);
    }

#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
    void GetStr (TMCStr pk, _CSTRING_NS::CString& str, DWORD dwLen, TMCStr pDft)
    {
        int n = GetStr (pk, str.GetBuffer(1024), 1024, pDft);
        str.ReleaseBuffer (n);
    }
#endif

    void GetStr (TMCStr pk, CString& str, DWORD dwLen, TMCStr pDft)
    {
        TM::CString strTmp;
        int n = GetStr (pk, strTmp.GetBuffer(1024), 1024, pDft);
        strTmp.SetLength (n);
        str.swap (strTmp);
    }

#ifdef __ATLBASE_H__
    void GetStr (TMCStr pk, CComBSTR& str, DWORD dwLen, TMCStr pDft)
    {
        TCHAR c[1024];
        GetStr (pk, c, 1024, pDft);
        str = c;
    }
#endif

public:
    template < class T >
    void DDX_Text (TMCStr pk, T& var, DWORD dwLen, TMCStr dft)
    {
        GetStr (pk, var, dwLen, dft);
    }

    template < class T >
    void DDX_Int (TMCStr pk, T& nVar, int dft)
    {
        nVar = GetPrivateProfileInt (ps, pk, dft, pf);
    }

    void DDX_Float (TMCStr pk, float& fVar, float dft)
    {
        TCHAR c[MAX_PATH];
        GetStr(pk, c, MAX_PATH, TM_T(""));
        double x = dft;
        if (c[0])
            StrScanDouble(c, MAX_PATH, x);
        fVar = x;
    }

    template < class T >
    void DDX_Struct (TMCStr pk, T& var)
    {
        GetPrivateProfileStruct (ps, pk, &var, sizeof(T), pf);
    }
};


struct PROFILE_DDX_SAVE
{
    TMCStr pf; // file
    TMCStr ps; // section

    PROFILE_DDX_SAVE (TMCStr pFile)
        : pf (pFile)
        , ps (0)
    {
    }

    void Init (TMCStr pSection)
    {
        ps = pSection;
    }

private:
    void PutStr (TMCStr pk, LPCSTR pVar)
    {
        WritePrivateProfileString (ps, pk, TM_A2T(pVar), pf);
    }

    void PutStr (TMCStr pk, LPCWSTR pVar)
    {
        WritePrivateProfileString (ps, pk, TM_W2T(pVar), pf);
    }

    void PutStr (TMCStr pk, CString str)
    {
        PutStr (pk, str.c_str());
    }

public:
    template < class T >
    void DDX_Text (TMCStr pk, T& var, DWORD dwLen, TMCStr dft)
    {
        PutStr (pk, var);
    }

    template < class T >
    void DDX_Int (TMCStr pk, T& nVar, int dft)
    {
        TCHAR c[MAX_PATH];
        StrFromL(nVar, c);
        PutStr (pk, c);
    }

    void DDX_Float (TMCStr pk, float& fVar, float dft)
    {
        TCHAR c[MAX_PATH];
        StrFromF(fVar, c);
        PutStr (pk, c);
    }

    template < class T >
    void DDX_Struct (TMCStr pk, T& var)
    {
        GetPrivateProfileStruct (ps, pk, &var, sizeof(T), pf);
    }
};



#define BGN_PROFILE_DDX_MAP(thisClass,filename) \
    BOOL ProfileExchangeLoad () { TM::PROFILE_DDX_LOAD _l(filename); return DoProfileExchange(_l); } \
    BOOL ProfileExchangeSave () { TM::PROFILE_DDX_SAVE _s(filename); return DoProfileExchange(_s); } \
    BOOL ProfileExchangeIsSectionExists(TMCStr pStr) {TCHAR c[5];  return GetPrivateProfileSection(pStr, c, 5, filename); } \
    template <class T > BOOL DoProfileExchange (T& pd) {

#define PROFILE_DDX_SECTION(section)     pd.Init (section);

#define PROFILE_DDX_TEXT2(key,var,dft)   pd.DDX_Text (key, var, sizeof(var), dft);

#define PROFILE_DDX_INT2(key,var,dft)    pd.DDX_Int (key, var, dft);

#define PROFILE_DDX_FLOAT2(key,var,dft)  pd.DDX_Float (key, var, dft);

#define PROFILE_DDX_STRUCT(key,var)      pd.DDX_Struct (key, var);


#define PROFILE_DDX_TEXT(key,var)        PROFILE_DDX_TEXT2 (key, var, TM_T(""))
#define PROFILE_DDX_INT(key,var)         PROFILE_DDX_INT2 (key, var, 0)
#define PROFILE_DDX_FLOAT(key,var)       PROFILE_DDX_FLOAT2 (key, var, 0)

#define END_PROFILE_DDX_MAP()            return TRUE; }



TM_END_NAMESPACE


#endif // TMCFG_H

