/*
 *  @file
 *  @brief  register for windows
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2005.04.02
 *
 *  Copyright (C) 2005 miragekiller
 */

#pragma once

#ifndef TMREGKEY_WIN_H
#define TMREGKEY_WIN_H

#include <windows.h>
#include <Shlwapi.h>
#include "tmarchive.h"

typedef HKEY        TMHReg;

#pragma comment(lib, "Shlwapi")


TM_BGN_NAMESPACE


class CRegHandle
{
public:
    typedef CRegHandle                              class_type;
    typedef TMHReg                                  handle_type;

    typedef struct CRegArchiveO                     CArchiveO;
    typedef struct CRegArchiveI                     CArchiveI;

    enum ERegType
    {
        e_rtBinary          = REG_BINARY,
        e_rtNum             = REG_DWORD,
        e_rtStr             = REG_SZ,
        e_rtNone            = REG_NONE
    };

    enum ETypeMask
    {
        e_tmBinary          = 1 << e_rtBinary,
        e_tmNum             = 1 << e_rtNum,
        e_tmStr             = 1 << e_rtStr,
        e_tmAll             = -1,
    };

    enum EKeyAccess
    {
        e_kaRead            = KEY_READ,
        e_kaWrite           = KEY_WRITE,
        e_kaReadWrite       = KEY_ALL_ACCESS,

        e_kaCreateLink      = KEY_CREATE_LINK,
        e_kaCreateDir       = KEY_CREATE_SUB_KEY,
        e_kaEnumDir         = KEY_ENUMERATE_SUB_KEYS,
        e_kaExecute         = KEY_EXECUTE,
        e_kaNotify          = KEY_NOTIFY,
        e_kaQueryValue      = KEY_QUERY_VALUE,
        e_kaSetValue        = KEY_SET_VALUE,
    };

protected:
    handle_type    m_hDir;

public:
    static CRegHandle HKCR () { return HKEY_CLASSES_ROOT; }
    static CRegHandle HKCU () { return HKEY_CURRENT_USER; }
    static CRegHandle HKLM () { return HKEY_LOCAL_MACHINE; }
    static CRegHandle HKUS () { return HKEY_USERS; }
    static CRegHandle HKPD () { return HKEY_PERFORMANCE_DATA; }
    static CRegHandle HKCC () { return HKEY_CURRENT_CONFIG; }
    static CRegHandle HKDD () { return HKEY_DYN_DATA; }

private:
    inline void Report (DWORD dwMsg)
    {
#ifdef TM_DBG
        TCHAR c[MAX_PATH] = { 0 };
        if (ERROR_SUCCESS != dwMsg)
        {
            FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,
                0, dwMsg, 0, c, MAX_PATH, 0);
        }
#endif // TM_DBG
    }

    TMBool _SetValue (LPCTSTR lpKey, ERegType eType, const void *lpData, DWORD cbData)
    {
        DbgAssert(m_hDir && lpData);
        return ERROR_SUCCESS == ::RegSetValueEx (m_hDir, lpKey, 0, eType,
            (const BYTE*)lpData, cbData);
    }

public:
    CRegHandle (handle_type hKey = 0)
        : m_hDir (hKey)
    {
    }

    operator handle_type () { return m_hDir; }

    class_type& operator = (handle_type hKey)
    {
        Attach (hKey);
        return *this;
    }

    void Swap (CRegHandle& rhs)
    {
        tm_swap (m_hDir, rhs.m_hDir);
    }

    void Attach (handle_type hKey)
    {
        DbgAssert(!m_hDir);
        m_hDir = hKey;
    }

    TMHReg Detach ()
    {
        TMHReg hKey = m_hDir;
        m_hDir = 0;
        return hKey;
    }

    // The function fails if the specified key already exists.
    TMBool CreateNew (handle_type hParent, const TMCharT* lpDir,
        EKeyAccess eAccess = e_kaReadWrite)
    {
        DbgAssert(!m_hDir && lpDir && hParent);

        if (CRegHandle().OpenExisting (hParent, lpDir, e_kaRead))
            return false;

        return OpenAlways (hParent, lpDir, eAccess);
    }

    // If the key exists, the function clear the key
    TMBool CreateAlways (handle_type hParent, const TMCharT* lpDir,
        EKeyAccess eAccess = e_kaReadWrite)
    {
        DbgAssert(!m_hDir && lpDir && hParent);

        CRegHandle(hParent).DeleteDir (lpDir);
        return OpenAlways (hParent, lpDir, eAccess);
    }

    // Opens the key. The function fails if the key does not exist.
    TMBool OpenExisting (handle_type hParent, const TMCharT* lpDir,
        EKeyAccess eAccess = e_kaReadWrite)
    {
        DbgAssert(!m_hDir && lpDir && hParent);

        LONG lMsg = RegOpenKeyEx (hParent, lpDir, 0, eAccess, &m_hDir);
        Report (lMsg);
        return ERROR_SUCCESS == lMsg;
    }

    // Opens the key, if it exists.
    // If the key does not exist, the function creates the key
    TMBool OpenAlways (handle_type hParent, const TMCharT* lpDir,
        EKeyAccess eAccess = e_kaReadWrite)
    {
        DbgAssert(!m_hDir && lpDir && hParent);

        LONG lMsg = RegCreateKeyEx (hParent, lpDir, 0, 0,
            REG_OPTION_NON_VOLATILE, eAccess, 0, &m_hDir, 0);
        Report (lMsg);
        return ERROR_SUCCESS == lMsg;
    }

    // Opens the key and clear it
    // The function fails if the key does not exist
    TMBool TruncateExisting (handle_type hParent, const TMCharT* lpDir,
        EKeyAccess eAccess = e_kaReadWrite)
    {
        if (!CRegHandle().OpenExisting (hParent, lpDir, e_kaRead))
            return false;

        return CreateAlways (hParent, lpDir, eAccess);
    }

    handle_type GetChildByIndex (int nDir, EKeyAccess eAccess = e_kaReadWrite)
    {
        TCHAR c[1024];
        DWORD cbSize = 1024;
        HRESULT hr = ::RegEnumKeyEx (m_hDir, nDir, c, &cbSize, 0, 0, 0, 0);
        if (hr == ERROR_SUCCESS)
        {
            CRegHandle hReg;
            hReg.OpenExisting (m_hDir, c, eAccess);
            return hReg.m_hDir;
        }
        return 0;
    }

    void Close ()
    {
        if (m_hDir)
        {
            RegCloseKey (m_hDir);
            m_hDir = 0;
        }
    }

    // delete sub key and all sub sub key
    TMBool DeleteDir (const TMCharT* lpDir)
    {
        DbgAssert(m_hDir && lpDir);
        DWORD dwMsg = SHDeleteKey (m_hDir, lpDir);
        Report (dwMsg);
        return (ERROR_SUCCESS == dwMsg);
    }

    TMBool DeleteValue (TMCStr lpDir, TMCStr lpKey)
    {
        DbgAssert(m_hDir && lpDir);
        return ERROR_SUCCESS == SHDeleteValue (m_hDir, lpDir, lpKey);
    }

    //if lpValue==""or==0£¬indicates delete the default value of that item
    TMBool DeleteValue (const TMCharT* lpValue)
    {
        DbgAssert(m_hDir);
        return ERROR_SUCCESS == ::RegDeleteValue (m_hDir, lpValue);
    }

    TMBool EnumKey (DWORD dwIndex, LPTSTR lpKey, DWORD cbName)
    {
        DbgAssert(m_hDir && lpKey);
        return ERROR_SUCCESS == ::RegEnumKey (m_hDir, dwIndex, lpKey, cbName);
    }

    void Flush ()
    {
        DbgAssert(m_hDir);
        ::RegFlushKey (m_hDir);
    }

    ////////////////////////////////////////////////////////////////////////
    // set value
    TMBool SetValue (LPCTSTR lpKey, LPCTSTR pStr)
    {
        if (pStr)
        {
            int n = (1 + lstrlen(pStr)) * sizeof(TCHAR);
            return _SetValue (lpKey, e_rtStr, pStr, n);
        }
        else
        {
            return _SetValue (lpKey, e_rtStr, TM_T(""), sizeof(TCHAR));
        }
    }

    TMBool SetValue (LPCTSTR lpKey, DWORD dwVal)
    {
        return _SetValue (lpKey, e_rtNum, &dwVal, sizeof(DWORD));
    }

    TMBool SetValue (LPCTSTR lpKey, const BYTE* lpData, size_t dwSize)
    {
        return _SetValue (lpKey, e_rtBinary, lpData, dwSize);
    }

    ////////////////////////////////////////////////////////////////////////
    // query value
    TMBool QueryProp (LPCTSTR lpKey, DWORD* pType, DWORD* pdwSize)
    {
        DbgAssert(lpKey);
        return ERROR_SUCCESS == ::RegQueryValueEx (m_hDir, lpKey, 0, pType,
            0, pdwSize);
    }

    DWORD QuerySize (LPCTSTR lpKey)
    {
        DbgAssert(lpKey);
        DWORD cbData = 0;
        ::RegQueryValueEx (m_hDir, lpKey, 0, 0, 0, &cbData);
        return cbData;
    }

    DWORD QueryValue (LPCTSTR lpKey, LPTSTR lpResult, DWORD dwLen)
    {
        DbgAssert(m_hDir && lpKey);

        DWORD dwType = 0;
        LONG lRes = ::RegQueryValueEx (m_hDir, lpKey, 0, &dwType,
            (BYTE*)lpResult, &dwLen);

        DbgAssert((lRes != ERROR_SUCCESS) || (dwType == REG_SZ) ||
            (dwType == REG_MULTI_SZ) || (dwType == REG_EXPAND_SZ));

        return ERROR_SUCCESS == lRes ? dwLen : 0;
    }

#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
    void QueryValue (LPCTSTR lpKey, _CSTRING_NS::CString& str)
    {
        int n = QuerySize (lpKey);
        LPTSTR p = (LPTSTR) _alloca (n);
        p[0] = 0;
        QueryValue (lpKey, p, n);
        str = p;
    }
#endif

    TMBool QueryValue (LPCTSTR lpKey, CString& str)
    {
        int n = QuerySize (lpKey);
        if (n > sizeof(TCHAR))
        {
            CString strx;
            QueryValue (lpKey, strx.GetBuffer(n), n);
            strx.SetLength (n / sizeof(TCHAR) - 1);
            str.swap (strx);
            return true;
        }
        else
        {
            return false;
        }
    }

#ifdef __ATLBASE_H__
    void QueryValue (LPCTSTR lpKey, CComBSTR& str)
    {
        int n = QuerySize (lpKey);
        LPTSTR p = (LPTSTR) _alloca (n);
        p[0] = 0;
        QueryValue (lpKey, p, n);
        str = p;
    }
#endif

    TMBool QueryValue (LPCTSTR lpKey, DWORD* dwVal)
    {
        DWORD dwType = 0;
        DWORD dwCount = sizeof(DWORD);

        LONG lRes = RegQueryValueEx (m_hDir, lpKey, NULL, &dwType,
            (LPBYTE)dwVal, &dwCount);

        DbgAssert((lRes != ERROR_SUCCESS) || (dwType == REG_DWORD));
        DbgAssert((lRes != ERROR_SUCCESS) || (dwCount == sizeof(DWORD)));

        return ERROR_SUCCESS == lRes;
    }

    DWORD QueryBinary (LPCTSTR lpKey, BYTE* lpData, DWORD dwLen, DWORD* pType = 0)
    {
        LONG lRes = RegQueryValueEx (m_hDir, lpKey, NULL, pType, lpData, &dwLen);
        return ERROR_SUCCESS == lRes ? dwLen : 0;
    }

    TMBool QueryBinary (LPCTSTR lpKey, CStringA& str, DWORD dwMask = e_tmAll)
    {
        DWORD dwSize;
        DWORD eType;
        if (QueryProp (lpKey, &eType, &dwSize) && (eType & dwMask))
        {
            CStringA strx;
            QueryBinary (lpKey, (BYTE*)strx.GetBuffer(dwSize), dwSize);
            str.swap (strx);
            return true;
        }
        else
        {
            return false;
        }
    }

    DWORD QueryNum (const TMCharT* lpKey, DWORD dwDefault = 0)
    {
        QueryValue (lpKey, &dwDefault);
        return dwDefault;
    }
};

class CRegKey : public CRegHandle
{
public:
    typedef CRegKey                                 class_type;

public:
    CRegKey (handle_type hKey = 0)
        : CRegHandle (hKey)
    {
    }

    ~CRegKey ()
    {
        if (m_hDir)
            ::RegCloseKey (m_hDir);
    }
};

struct CRegArciveFileBase
{
    typedef CRegHandle section_type;

    CRegHandle m_file;
    CString m_strKey;

    CRegArciveFileBase(CRegHandle f) : m_file(f) {}

    void DoEntrySection(section_type& storage, const CString& str)
    {
        storage.Swap(m_file);
        m_file.OpenAlways(storage, str, m_file.e_kaReadWrite);
    }

    void DoLeaveSection(section_type& storage)
    {
        m_file.Close();
        m_file.Swap(storage);
    }

    void DoSetKey(const CStringW& str) { m_strKey = str; }
};

struct CRegArchiveO
    : public CArchiveTxtOImplT<CRegArchiveO, TMCharT>
    , public CRegArciveFileBase
{
public:
    CRegArchiveO(CRegHandle f) : CRegArciveFileBase(f) {}

    void DoSerializeString(TMCStr str)
    {
        m_file.SetValue(m_strKey, str);
    }

    void DoSerializeString(const TM::CString& str)
    {
        m_file.SetValue(m_strKey, str);
    }

    void DoSerializeInt(long __x)
    {
        m_file.SetValue(m_strKey, __x);
    }
};

struct CRegArchiveI
    : public CArchiveTxtIImplT<CRegArchiveI, TMCharT>
    , public CRegArciveFileBase
{
public:
    CRegArchiveI(CRegHandle f) : CRegArciveFileBase(f) {}

    using CArchiveTxtIImplT<CRegArchiveI, TMCharT>::DoSerializeString;

    template <class T>
    void DoSerializeInt(T& __x)
    {
        DWORD y = __x;
        TMBool b = m_file.QueryValue(m_strKey, &y);
        if (b)
            __x = (T)y;
        else
            this->SetStat(b);
    }

    int DoSerializeString(TMStr str)
    {
        int n = m_file.QueryValue(m_strKey, str, eDefaultStringBufferSize);
        return this->SetStat(n), n;
    }

    void DoSerializeString(CString& str)
    {
        this->SetStat(m_file.QueryValue(m_strKey, str));
    }
};


TM_END_NAMESPACE


#endif // TMREGKEY_WIN_H
