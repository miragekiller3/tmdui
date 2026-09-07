#pragma once

#ifndef TMINET_H 
#define TMINET_H

#include "tmstring.h"
#include "tmthread.h"
#include "tmplatform_win.h"
#include "tmlist.h"

#include <Wininet.h>


#pragma comment(lib, "Wininet.lib")



TM_BGN_NAMESPACE


struct URLInfo
{
    CString strUrl;
    CString strHost;
    CString strPath;
    INTERNET_PORT nPort;
    INTERNET_SCHEME nScheme;
};

inline BOOL ParseUrl (LPCTSTR pURL, URLInfo& uInfo)
{
    // 标准化URL
    DWORD dwSize = INTERNET_MAX_URL_LENGTH;
    BOOL b = InternetCanonicalizeUrl (pURL,
        uInfo.strUrl.GetBuffer (INTERNET_MAX_URL_LENGTH), &dwSize, ICU_BROWSER_MODE);
    if (!b)
    {
        if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
            return FALSE;
        b = InternetCanonicalizeUrl (pURL,
            uInfo.strUrl.GetBuffer (INTERNET_MAX_URL_LENGTH), &dwSize, ICU_BROWSER_MODE);
        if (!b)
            return false;
    }
    uInfo.strUrl.SetLength (dwSize);
    
    // 解析URL
    URL_COMPONENTS uc = { sizeof(URL_COMPONENTS) };
    uc.dwHostNameLength = INTERNET_MAX_URL_LENGTH;
    uc.lpszHostName = uInfo.strHost.GetBuffer(INTERNET_MAX_URL_LENGTH);
    uc.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
    uc.lpszUrlPath = uInfo.strPath.GetBuffer(INTERNET_MAX_URL_LENGTH);
    b = InternetCrackUrl (uInfo.strUrl, 0, 0, &uc);
    if (!b)
    {
        uInfo.nScheme = INTERNET_SCHEME_UNKNOWN;
        return false;
    }
    else
    {
        uInfo.nScheme = uc.nScheme;
        uInfo.nPort = uc.nPort;
        uInfo.strHost.SetLength (uc.dwHostNameLength);
        uInfo.strPath.SetLength (uc.dwUrlPathLength);
        return true;
    }
}

class CINet
{
public:
    HINTERNET m_hSession;

    TM_NO_COPYABLE(CINet);

public:
    CINet (
//        LPCTSTR pAgent = TM_T("Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0)"),
        LPCTSTR pAgent = TM_T("Mozilla/5.0 (Windows NT 5.1)   Chrome/49.0.2623.112"),
        DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG,
        LPCTSTR pProxyName = 0,
        LPCTSTR pProxyPass = 0)
    {
        m_hSession = InternetOpen (pAgent, dwAccessType, pProxyName, pProxyPass, 0);
    }

    ~CINet()
    {
        if (m_hSession)
            InternetCloseHandle (m_hSession);
    }

    operator HINTERNET() const { return m_hSession; }
};

class CINetFile
{
protected:
    HINTERNET m_hConnect;

    TM_NO_COPYABLE(CINetFile);

    enum EFlag
    {
        eFlag1 = INTERNET_FLAG_NO_CACHE_WRITE,
        eFlag2 = INTERNET_FLAG_EXISTING_CONNECT |
//                  INTERNET_FLAG_NO_AUTO_REDIRECT |
                 INTERNET_FLAG_SECURE |
                 INTERNET_FLAG_IGNORE_CERT_CN_INVALID |
                 INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
//                 INTERNET_FLAG_NO_COOKIES |
                 INTERNET_FLAG_RELOAD |
                 INTERNET_FLAG_NEED_FILE,
    };

public:
    CINetFile ()
        : m_hConnect (0)
    {
    }

    ~CINetFile () { Close (); }

    void Close ()
    {
        if (m_hConnect)
        {
            InternetCloseHandle (m_hConnect);
            m_hConnect = 0;
        }
    }

    operator BOOL () const { return m_hConnect != 0; }

    BOOL Open (HINTERNET hSession, LPCWSTR pUrl)
    {
        DbgAssert(hSession);
        DbgAssert(!m_hConnect);
        m_hConnect = InternetOpenUrlW(hSession, pUrl, 0, 0, eFlag1, 0);
        return m_hConnect != 0;
    }

    BOOL Open (HINTERNET hSession, LPCSTR pUrl)
    {
        DbgAssert(hSession);
        DbgAssert(!m_hConnect);
        m_hConnect = InternetOpenUrlA(hSession, pUrl, 0, 0, eFlag1, 0);
        return m_hConnect != 0;
    }

    int GetLength ()
    {
        DbgAssert(*this);
        
        DWORD dwByteToRead = 0;
        DWORD dwSizeOfRq = 4;
        HttpQueryInfo (m_hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
            (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL);
        
        return dwByteToRead;
    }

    long Read (LPSTR lpBuffer, long lBytes)
    { 
        DbgAssert(*this);

        DWORD dw = 0;
        BOOL b = InternetReadFile (m_hConnect, lpBuffer, lBytes, &dw);
        return b ? dw : -1;
    }
};


inline int GetUrlFile(CINetFile& f, CStringA& str, int nLen = 0)
{
    if (f)
    {
        int nPos = str.GetLength();
        if (nLen)
        {
            int n = f.Read (str.GetBuffer(nLen + nPos) + nPos, nLen);
            str.SetLength (nPos + (n >= 0 ? n : 0));
        }
        else
        {
            for (; ;)
            {
                const int nBuf = 2048*16;
                int n = f.Read (str.GetBuffer(nPos + nBuf) + nPos, nBuf);
                n = n < 0 ? 0 : n;
                nPos += n;
                if (n == 0)
                    break;
            }
            str.SetLength (nPos);
        }
    }
    
    return str.GetLength();
}

inline int GetUrlFile (HINTERNET hSession, TMCStrW pUrl, CStringA& str, int nLen = 0)
{
    CINetFile f;
    f.Open (hSession, pUrl);
    return GetUrlFile(f, str, nLen);
}

inline int GetUrlFile (HINTERNET hSession, TMCStrA pUrl, CStringA& str, int nLen = 0)
{
    CINetFile f;
    f.Open (hSession, pUrl);
    return GetUrlFile(f, str, nLen);
}

inline int GetUrlFile (TMCStrW pUrl, CStringA& str, int nLen = 0)
{
    CINet inet;
    return GetUrlFile(inet, pUrl, str, nLen);
}

inline int GetUrlFile (TMCStrA pUrl, CStringA& str, int nLen = 0)
{
    CINet inet;
    return GetUrlFile(inet, pUrl, str, nLen);
}


class IDownloadUrlFileSink
{
public:
    virtual void OnDownLoad(void* pKey, const CString& url, CStringA& strRet) = 0;
//     virtual void OnDoanloadError(void* pKey, const CString& url) = 0;
};

class CAsyncUrlFile
    : public CThreadID
    , public IThread
{
protected:
    struct TASK
    {
        void*   pKey;
        CString strUrl;
        IDownloadUrlFileSink* pSink;
    };
    typedef CVector<TASK> CTaskList;

    CCritical       m_mutex;
    CTaskList       m_aTask;
    TMBool          m_bClose;

    enum { WM_ONDOWNLOAD = WM_USER + 300 };

#define s_hMessage GetMessageHWND()
    static HWND& GetMessageHWND() { static HWND s_hWnd; return s_hWnd; }

    virtual unsigned ThreadProc ()
    {
        CINet inet;

        for (;;)
        {
            // get task
            TMBool bSleep = false;
            TMBool bStop = false;
            TASK task;
            {
                CAutoLockT<CCritical> lock(m_mutex);
                bSleep = !m_aTask.size();
                bStop = m_bClose;
                if (!bSleep)
                {
                    task = m_aTask[0];
                    m_aTask.erase(m_aTask.begin());
                    DbgAssert(task.pSink);
                }
            }
            if (bStop)
                return 0;
            else if (bSleep)
            {
                Suspend();
                continue;
            }

            CStringA str;
            int n = GetUrlFile(inet, task.strUrl, str);
            SendMessage(s_hMessage, WM_ONDOWNLOAD, (WPARAM)&task, (LPARAM)&str);
        }
    }

    static LRESULT CALLBACK MsgWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_ONDOWNLOAD)
        {
            TASK* pTask = (TASK*)wParam;
            CStringA* pString = (CStringA*)lParam;
            pTask->pSink->OnDownLoad(pTask->pKey, pTask->strUrl, *pString);
            return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

public:
    CAsyncUrlFile()
        : m_bClose (false)
    {
    }

    ~CAsyncUrlFile()
    {
        Close();
    }

    void DownLoad(void* pKey, const CString& strUrl, IDownloadUrlFileSink* pSink)
    {
        DbgAssert(pSink);

        if (!s_hMessage)
        {
            WNDCLASSEX  wc = 
            {
                sizeof (WNDCLASSEX), CS_GLOBALCLASS,
                    MsgWindowProc, 0, 0, 0, 0, 
                    0, 0, 0, TM_T("TM_INET_DOWNLOAD_MSG"), 0
            };
            RegisterClassEx (&wc); 
            s_hMessage = ::CreateWindowEx(0, TM_T("TM_INET_DOWNLOAD_MSG"), 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
        }

        {
            CAutoLockT<CCritical> lock(m_mutex);
            TASK task;
            task.pKey = pKey;
            task.strUrl = strUrl;
            task.pSink = pSink;
            m_aTask.push_back(task);
        }

        if (!IsValid())
            Create(this);
        else
            Resume();
    }

    void UnDownLoad(void* pKey, IDownloadUrlFileSink* pSink)
    {
        CAutoLockT<CCritical> lock(m_mutex);
        for (CTaskList::recorder rec(m_aTask); rec;)
        {
            TASK& task = rec.get_data();
            if ((task.pKey == pKey) && (!pSink || pSink == task.pSink))
                rec.erase();
            else
                ++rec;
        }
    }

    void Close()
    {
        {
            CAutoLockT<CCritical> lock(m_mutex);
            m_aTask.clear();
            m_bClose = true;
        }

        if (IsValid())
        {
            Resume();
            Join();
        }
    }

};


// class CAsyncUrlFileTask
//     : public CThread
//     , public IThread
// {
// protected:
//     struct TASK
//     {
//         CString strUrl;
//         TM::CString strRet;
//         int nState;
//     };
// 
//     CCritical       m_mutex;
//     CVector<TASK>   m_aTask;
//     TMBool          m_bClose;
// 
//     enum { WM_ONDOWNLOAD = WM_USER + 300 };
// 
// #define s_hMessage GetMessageHWND()
//     static HWND& GetMessageHWND() { static HWND s_hWnd; return s_hWnd; }
// 
//     virtual unsigned ThreadProc ()
//     {
//         CINet inet;
// 
//         for (;;)
//         {
//             // get task
//             TMBool bSleep = false;
//             TMBool bStop = false;
//             TASK task;
//             {
//                 CAutoLockT<CCritical> lock(m_mutex);
//                 bSleep = !m_aTask.size();
//                 bStop = m_bClose;
//                 if (!bSleep)
//                 {
//                     task = m_aTask[0];
//                     m_aTask.erase(m_aTask.begin());
//                     DbgAssert(task.pSink);
//                 }
//             }
//             if (bStop)
//                 return 0;
//             else if (bSleep)
//             {
//                 Suspend();
//                 continue;
//             }
// 
//             CStringA str;
//             int n = GetUrlFile(inet, task.strUrl, str);
//             SendMessage(s_hMessage, WM_ONDOWNLOAD, (WPARAM)&task, (LPARAM)&str);
//         }
//     }
// 
//     static LRESULT CALLBACK MsgWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//     {
//         if (uMsg == WM_ONDOWNLOAD)
//         {
//             TASK* pTask = (TASK*)wParam;
//             CStringA* pString = (CStringA*)lParam;
//             pTask->pSink->OnDownLoad(pTask->pKey, pTask->strUrl, *pString);
//             return 0;
//         }
//         return DefWindowProc(hwnd, uMsg, wParam, lParam);
//     }
// 
// public:
//     CAsyncUrlFile()
//         : m_bClose (false)
//     {
//     }
// 
//     ~CAsyncUrlFile()
//     {
//         Close();
//     }
// 
//     void DownLoad(void* pKey, const CString& strUrl, IDownloadUrlFileSink* pSink)
//     {
//         DbgAssert(pSink);
// 
//         if (!s_hMessage)
//         {
//             WNDCLASSEX  wc = 
//             {
//                 sizeof (WNDCLASSEX), CS_GLOBALCLASS,
//                     MsgWindowProc, 0, 0, 0, 0, 
//                     0, 0, 0, TM_T("TM_INET_DOWNLOAD_MSG"), 0
//             };
//             RegisterClassEx (&wc); 
//             s_hMessage = ::CreateWindowEx(0, TM_T("TM_INET_DOWNLOAD_MSG"), 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
//         }
// 
//         {
//             CAutoLockT<CCritical> lock(m_mutex);
//             TASK task;
//             task.pKey = pKey;
//             task.strUrl = strUrl;
//             task.pSink = pSink;
//             m_aTask.push_back(task);
//         }
// 
//         if (!IsValid())
//             Create(this);
//         else
//             Resume();
//     }
// 
//     void Close()
//     {
//         {
//             CAutoLockT<CCritical> lock(m_mutex);
//             m_aTask.clear();
//             m_bClose = true;
//         }
// 
//         if (IsValid())
//         {
//             Resume();
//             Join();
//         }
//     }
// 
// };


TM_END_NAMESPACE



#endif // TMINET_H 
