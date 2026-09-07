/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2003.04.09
 *
 *  Copyright (C) 2003 miragekiller
 */

#include "tmio.h"           // MemXXX
#include "tmtchar.h"        // StrXXX
#include "tmfilesystem.h"   // copyfile;FsXXX
#include "tmfilename.h"     // DllLoad
#include "tmdll.h"          // DllLoad
#include "tmfile.h"         // StreamXXX
#include "tmmemfile.h"      // StreamXXX
#include "tmcom.h"          // StreamXXX
#include "tmcolor.h"        // GdkColorFromSys
#include "tmdebugext.h"
#include <locale.h>


static TMCharA* s_unused = setlocale(LC_ALL, "C");


//////////////////////////////////////////////////////////////////////////////
// MemXXX
#ifdef TM_DBG

#define __mem_tail TMDbgClassInfo_Tail

void __mem_check(const TMDbgClassInfo* info)
{
    DbgAssert(info->m_checkByte == TMDbgClassInfo::eCheckValue);
    DbgAssert(*(TMDbgClassInfo::ECheckValue*)__mem_tail(info) == TMDbgClassInfo::eCheckValue);
}

void __mem_add(TMDbgClassInfo* p, size_t dwSize)
{
    const TMDbgClassInfo* info = DbgLeakOpt(0, eDbgLeakOptTypeDef);
    if (info)
    {
        *p = *info;
        p->m_pClass = (const char*)(p + 1);
//         StrPrintf(p->m_cInfo, tm_countof(p->m_cInfo), "%p %s", p->m_pClass, info->m_pTypeName);
    }
    else
    {
        p->m_pClass = (const char*)(p + 1);
        p->m_pTypeName = "unknown";
        p->m_sizeCallStack = 0;
//         StrPrintf(p->m_cInfo, tm_countof(p->m_cInfo), "%p", p->m_pClass);
    }
    p->m_eType     = p->eTypeMemTM;
    p->m_bStackObj = false;
    p->m_checkByte = TMDbgClassInfo::eCheckValue;
    p->m_szClass   = dwSize;
    p->m_szDebug   = sizeof(TMDbgClassInfo) + sizeof(TMDbgClassInfo::eCheckValue);
    *((TMDbgClassInfo::ECheckValue*)__mem_tail(p)) = TMDbgClassInfo::eCheckValue; 
    DbgLeakOpt(p, eDbgLeakOptTypeAdd);
}

void __mem_remove(TMDbgClassInfo* p)
{
    DbgAssert(p->m_checkByte == TMDbgClassInfo::eCheckValue);
    DbgAssert(*((TMDbgClassInfo::ECheckValue*)__mem_tail(p)) == TMDbgClassInfo::eCheckValue);
    DbgAssert(DbgLeakOpt(p, eDbgLeakOptTypeRemove)); // destroy error
//     DbgAssert(DbgLeakOpt(p, eDbgLeakOptTypeRemove)); // destroy error
}

#endif // TM_DBG

void* MemAlloc1(size_t dwSize)
{
#ifdef TM_DBG
    TM::CAutoAnalysisTuiMsg awm("MemAlloc1");

    TMDbgClassInfo* p = (TMDbgClassInfo*)malloc(sizeof(TMDbgClassInfo) + dwSize + sizeof(TMDbgClassInfo::ECheckValue));
    p->m_pTypeName = "MemAlloc";
    __mem_add(p, dwSize);
    return p + 1;
#else
    return malloc(dwSize);
#endif
}

void* MemAllocBy1(size_t dwSize, void* pInit)
{
    void* p = MemAlloc1(dwSize);
    if (pInit && p)
        MemCpy(p, pInit, dwSize);
    return p;
}


void* MemRealloc1(void* p, size_t dwSize)
{
#ifdef TM_DBG
    TM::CAutoAnalysisTuiMsg awm("MemRealloc1");

    if (!p)
        return MemAlloc1(dwSize);

    TMDbgClassInfo* pm = (TMDbgClassInfo*)p - 1;
    __mem_check(pm);
    __mem_remove(pm);

    pm = (TMDbgClassInfo*)realloc(pm, sizeof(TMDbgClassInfo) + dwSize + sizeof(TMDbgClassInfo::ECheckValue));
    __mem_add(pm, dwSize);
    return pm + 1;
#else
    return realloc(p, dwSize);
#endif
}

void MemFree(void* p)
{
    if (!p)
        return;
#ifdef TM_DBG
    TMDbgClassInfo* pm = (TMDbgClassInfo*)p - 1;
    __mem_check(pm);
    __mem_remove(pm);
    free(pm);
#else
    free(p);
#endif
}


//////////////////////////////////////////////////////////////////////////////
// file system
void FsMkDirs(TMCStr p)
{
    TMCharT szBuf[512];
    StrCopy (szBuf, 512, p);
    for (int i = 0; ++i;)
    {
        TMCharT c = szBuf[i];
        if (c == 0)
            break;
        if (c == '\\' || c == '/')
        {
            szBuf[i] = 0;
            FsMkDir (szBuf);
            szBuf[i] = c;
        }
    }
}

void FsRmDirs(TMCStr p)
{
    TM::CPathStore pts;

    if (!FsChDir(p))
        return;

    for (TM::CFindFile f(TM_T(".")); f; f.FindNext())
    {
        if (f.IsDirectory())
        {
            if (!f.IsDots())
                FsRmDirs(f.GetFileName());
        }
        else
            FsRemove(f.GetFileName());
    }
    FsRmDir(p);
}

size_t FsTmpPath(TMStr pBuf, size_t szBuffer)
{
#ifdef TM_WIN_DESKTOP
    return GetTempPath(szBuffer, pBuf);
#elif TM_WIN_METRO
    DbgAssert(0);
    return 0;
#else
    const char* c = getenv("TMP");
    if (!c)
    {
        c = getenv("TEMP");
        if (!c)
            c = "/tmp";
    }
    return StrAssign(pBuf, szBuffer, c, -1);
#endif
}


TMBool FsFindFirst(struct TMDir* pDir, TMCStr pName)
{
    DbgAssert(pDir);
    DbgAssert(pName);

#ifdef TM_WIN_DESKTOP

    pDir->hd = FindFirstFile(pName, &pDir->fd);
    if (pDir->hd != INVALID_HANDLE_VALUE)
        return true;
    pDir->fd.cFileName[0] = 0;
    return false;

#elif TM_WIN_METRO

    DbgAssert(0);
    return 0;

#else

    pDir->dirp = opendir(pName);
    if (0 != pDir->dirp)
    {
        pDir->dire = readdir(pDir->dirp);
        return true;
    }
    pDir->dire = 0;
    return false;

#endif
}

static void _SearchDir(TMCStr pDir, int nlayer, TMBool& bStop, TMFSearch pfn, void* avg)
{
    TM::CPathStore pts;

    if (!FsChDir(pDir))
        return;

    TM_IF_DBG(TM::CPathStore pts2);

    for (TM::CFindFile f(TM_T("*.*")); f; f.FindNext())
    {
        pfn(&f.m_dir, nlayer, &bStop, avg);
        if (bStop)
            return;
        if (f.IsDirectory() && !f.IsDots())
            _SearchDir(f.GetFileName(), nlayer + 1, bStop, pfn, avg);
    }
}

void FsSearch(TMCStr pDir, TMFSearch pfn, void* avg)
{
    TMBool bStop = false;
    _SearchDir(pDir, 0, bStop, pfn, avg);
}


TMHModule DllLoad(TMCStr pDllName, TMUInt32 dwFlag)
{
#if TM_WIN_DESKTOP
    TMHModule hm = LoadLibraryEx(pDllName, 0, dwFlag);
    // some system not support LOAD_LIBRARY_SEARCH_XXX
    if (hm == 0 && (dwFlag & LOAD_LIBRARY_SEARCH_MASK) && GetLastError() == ERROR_INVALID_PARAMETER)
    {
        if (dwFlag & (LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS))
        {
            TM::CFileName fn;
            fn.GetSiblingModuleFileName(pDllName);
            return LoadLibrary(fn.c_str());
        }
        if (dwFlag & (LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS))
        {
            TM::CFileName fn;
            fn.GetSystemDirectory();
            fn += '\\';
            fn += pDllName;
            return LoadLibrary(fn.c_str());
        }
        DbgAssert(0);
    }
    return hm;
#elif TM_WIN_METRO
    return LoadPackagedLibrary(pDllName, 0);
#elif defined (TM_APPLE)
    if (!pDllName)
        return CFBundleGetMainBundle();
    TMHModule hm;
    CFStringRef path;
    CFURLRef url;
    path = CFStringCreateWithCString(0, pDllName, kCFStringEncodingUTF8);
    url = CFURLCreateWithFileSystemPath(0, path, kCFURLPOSIXPathStyle, 1);
    hm = CFBundleCreate(0, url);
    CFRelease(url);
    CFRelease(path);
    return hm;
#elif defined(TM_GTK)
    GModule* p = g_module_open(pDllName, G_MODULE_BIND_LAZY);
    return p;
#else
    return dlopen(pDllName, RTLD_NOW);
#endif
}

TMHModule DllLoadFrom(TMCStr pDllName, TMHModule hDir)
{
    TM::CFileName fn;
    fn.GetSiblingModuleFileName(pDllName, hDir);
    return DllLoad(fn.c_str(), 0);
}


#ifdef _copyfile_DEFINED

#include <errno.h>

int copyfile(const char *source, const char *target, int, int flags)
{
    struct stat src_stat;

    if (lstat(source, &src_stat) == -1)
        return -1;

    if (S_ISLNK(src_stat.st_mode))
    {
        char lnk[1024];
        int lnk_size;
        if ((lnk_size = readlink(source, lnk, 1023)) == -1)
            return -2;
        lnk[lnk_size] = '\0';
        if (symlink(lnk, target) == -1)
            return -3;
    }
    else if (S_ISREG(src_stat.st_mode))
    {
        int src, dst;
        int rsize;
        char buf[1024];
        if ((src = open(source, O_RDONLY)) == -1)
            return -4;

        if ((dst = creat(target, src_stat.st_mode)) == -1)
            return -5;

        while ((rsize = read(src, buf, 1024))) {
            if (rsize == -1 && errno == EINTR)
                continue ;
            if (rsize == -1) {
                close(src);
                close(dst);
                return -6;
            }
            while (write(dst, buf, rsize) == -1)
                if (errno != EINTR) {
                    close(src);
                    close(dst);
                    return -7;
                }
        }
        close(src);
        close(dst);
    }
    else
        return -8;

    return 0;
}
#endif // _copyfile_DEFINED






///////////////////////////////////////////////////////////////////////////////
// struppr,lwr
template <class T>
static int __str_upr_lwr(T* dest, const T* src, size_t nSrc, T chA, int dChar)
{
    if (nSrc)
        DbgAssert(src && dest);
    size_t nLen = nSrc;
    size_t i = 0;
    enum { a_z = 'z' - 'a' };
    for (; i < nLen; ++i)
    {
        T ch = src[i];
        if (ch >= chA && ch <= chA + a_z)
            dest[i] = ch + dChar;
        else
            dest[i] = ch;
        if (!ch)
            break;
    }
    return i;
}

#ifndef TM_WIN_DESKTOP
int StrUpperA(TMStrA dest, TMCStrA src, size_t ssize)
{
    return __str_upr_lwr(dest, src, ssize, 'a', 'A' - 'a');
}

int StrUpperW(TMStrW dest, TMCStrW src, size_t ssize)
{
    return __str_upr_lwr(dest, src, ssize, L'a', L'A' - L'a');
}

int strLowerA(TMStrA dest, TMCStrA src, size_t ssize)
{
    return __str_upr_lwr(dest, src, ssize, 'A', 'a' - 'A');
}

int strLowerW(TMStrW dest, TMCStrW src, size_t ssize)
{
    return __str_upr_lwr(dest, src, ssize, L'A', L'a' - L'A');
}

#endif // TM_WIN_DESKTOP



///////////////////////////////////////////////////////////////////////////////
// return: dest size; error return 0;
// the function used in tmxml.cpp
// utf-32 : utf-8
// U-00000000 - U-0000007F: 0xxxxxxx
// U-00000080 - U-000007FF: 110xxxxx 10xxxxxx
// U-00000800 - U-0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
//
// U-00010000 - U-001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
// U-00200000 - U-03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
// U-04000000 - U-7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
int __u32to8(TMStr8 pd, size_t nBuffer, TMUInt32 n)
{
    if (nBuffer == 0)
        return 0;

    if (n <= 0x7f)
    {
        pd[0] = (TMCharA)n;
        return 1;
    }

    if (n <= 0x7ff)
    {
        if (nBuffer >= 2)
        {
            pd[0] = (TMCharA)(0xc0 | (n >> 6));
            pd[1] = (TMCharA)(0x80 | (n & 0x3f));
            return 2;
        }
        return 0;
    }

    // Reject invalid Unicode scalar values and keep parser behavior predictable.
    if (n > 0x10ffff || (n >= 0xd800 && n <= 0xdfff))
    {
        pd[0] = '?';
        return 1;
    }

    if (n <= 0xffff)
    {
        if (nBuffer >= 3)
        {
            pd[0] = (TMCharA)(0xe0 | (n >> 12));
            pd[1] = (TMCharA)(0x80 | ((n >> 6) & 0x3f));
            pd[2] = (TMCharA)(0x80 | (n & 0x3f));
            return 3;
        }
        return 0;
    }

    if (nBuffer >= 4)
    {
        pd[0] = (TMCharA)(0xf0 | (n >> 18));
        pd[1] = (TMCharA)(0x80 | ((n >> 12) & 0x3f));
        pd[2] = (TMCharA)(0x80 | ((n >> 6) & 0x3f));
        pd[3] = (TMCharA)(0x80 | (n & 0x3f));
        return 4;
    }
    return 0;
}

// return 0:EOF; error;-1;
static int __u8tow(TMUInt32& utf32, TMCStr8 utf8, size_t nBuffer)
{
#define IS10(x) ((p[x] & 0xC0) == 0x80)
    
    TMUInt8* p = (TMUInt8*)utf8;
    
    // 00000000 - 0000007F     0xxxxxxx
    if ((p[0] & 0x80) == 0)
    {
        if (nBuffer >= 1)
        {
            utf32 = p[0];
            return 1;
        }
        return 0;
    }

    // 00000080 - 000007FF     110xxxxx 10xxxxxx
    if ((p[0] & 0xE0) == 0xC0)
    {
        if (nBuffer >= 2)
        {
            if (IS10(1))
            {
                utf32  = (((TMUInt32)(p[0] & ~0xE0)) << 6);
                utf32 |= (((TMUInt32)(p[1] & ~0x80)));
                return 2;
            }
            return -1;
        }
        return 0;
    }

    // 00000800 - 0000FFFF     1110xxxx 10xxxxxx 10xxxxxx 
    if ((p[0] & 0xF0) == 0xE0)
    {
        if (nBuffer >= 3)
        {
            if (IS10(1) && IS10(2))
            {
                utf32  = (((TMUInt32)(p[0] & ~0xE0)) << 12);
                utf32 |= (((TMUInt32)(p[1] & ~0x80)) << 6);
                utf32 |= (((TMUInt32)(p[2] & ~0x80)));
                return 3;
            }
            return -1;
        }
        return 0;
    }

    // 00010000 - 001FFFFF     11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    if ((p[0] & 0xF8) == 0xF0)
    {
        if (nBuffer >= 4)
        {
            if (IS10(1) && IS10(2) && IS10(3))
            {
                utf32  = (((TMUInt32)(p[0] & ~0xF0)) << 18);
                utf32 |= (((TMUInt32)(p[1] & ~0x80)) << 12);
                utf32 |= (((TMUInt32)(p[2] & ~0x80)) << 6);
                utf32 |= (((TMUInt32)(p[3] & ~0x80)));
                return 4;
            }
            return -1;
        }
    }

    // 00200000 - 03FFFFFF      111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    if ((p[0] & 0xFC) == 0xF8)
    {
        if (nBuffer >= 5)
        {
            if (IS10(1) && IS10(2) && IS10(3) && IS10(4))
            {
                utf32  = (((TMUInt32)(p[0] & ~0xF0)) << 24);
                utf32 |= (((TMUInt32)(p[1] & ~0x80)) << 18);
                utf32 |= (((TMUInt32)(p[2] & ~0x80)) << 12);
                utf32 |= (((TMUInt32)(p[3] & ~0x80)) << 6);
                utf32 |= (((TMUInt32)(p[4] & ~0x80)));
                return  5;
            }
            return -1;
        }
        return 0;
    }

    // 04000000 - 7FFFFFFF     1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    if ((p[0] & 0xFE) == 0xFC)
    {
        if (nBuffer >= 5)
        {
            if (IS10(1) && IS10(2) && IS10(3) && IS10(4) && IS10(5))
            {
                utf32 =  (p[0] & ~0xF0) << 30;
                utf32 |= (p[1] & ~0x80) << 24;
                utf32 |= (p[2] & ~0x80) << 18;
                utf32 |= (p[3] & ~0x80) << 12;
                utf32 |= (p[4] & ~0x80) << 6;
                utf32 |= (p[5] & ~0x80);
                return 6;
            }
            return -1;
        }
        return 0;
    }

    return (nBuffer >= 1) ? -1 : 0;

#undef IS10
}

int __u32to16(TMStrW pd, TMUInt32 u32)
{
    if (u32 > 0xffff)
    {
        pd[0] = (TMUInt16)((u32 >>   10) + 0xd7c0);
        pd[1] = (TMUInt16)((u32 & 0x3ff) + 0xdc00);
        return 2;
    }
    pd[0] = (TMUInt16)u32;
    return 1;
}

#ifndef TM_WIN
size_t __str_convert(TMStr8 pd, size_t nd, TMCStrW ps, size_t ns)
{
    size_t i = 0, j = 0;
    for (; j < ns; ++j)
    {
        if (i >= nd)
            return 0; // no enough buffer

        if ((ns == size_t(-1)) && (ps[j] == 0))
            break;
        
        int n = __u32to8(pd + i, nd - i, ps[j]);
        if (n <= 0)
            break;
        
        i += n;
    }
    
    return i;
}

size_t __str_convert(TMStrW pd, size_t nd, TMCStr8 ps, size_t ns)
{
    size_t i = 0, j = 0;
    for (; j < ns; )
    {
        if (i >= nd)
            return 0; // no enough buffer

        if ((ns == size_t(-1)) && (ps[j] == 0))
            break;
        
        TMUInt32 u32;
        int n = __u8tow(u32, ps + j, ns - j);
        if (n <= 0)
            break;
        
        i += __u32to16(pd + i, u32);
        j += n;
    }
    
    return i;
}
#endif // #ifndef TM_WIN

size_t StrA2W(int ncp, TMStrW pd, size_t nd, TMCStrA ps, size_t ns)
{
    DbgAssert(ns ? 0 != ps : 1);
    DbgAssert(nd ? 0 != pd : 1);

#ifdef TM_WIN

    if (nd == 0)
    {
        return 0;
    }
    else
    {
        const size_t nmax = INT_MAX / sizeof(TMCharW);
        tm_setmin(nd, nmax);
        int n = MultiByteToWideChar(ncp, 0, ps, ns, pd, nd);
        if (n == 0)
            return 0;
//         {
//             if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
//             {
//                 MemSet (pd, 0, nd);
//                 MultiByteToWideChar (ncp, 0, ps, ns, pd, nd);
//                 return StrLen (pd);
//             }
//             return 0;
//         }
        return pd[n - 1] == 0 ? n - 1 : n;
    }

#else

    return __str_convert(pd, nd, (TMCStr8)ps, ns);

#endif //#ifdef TM_WIN
}

size_t StrW2A(int ncp, TMStrA pd, size_t nd, TMCStrW ps, size_t ns)
{
    DbgAssert(ns ? 0 != ps : 1);
    DbgAssert(nd ? 0 != pd : 1);

#ifdef TM_WIN

    if (nd == 0)
    {
        return 0;
    }
    else
    {
        nd = tm_min(nd, INT_MAX);
        int n = WideCharToMultiByte(ncp, 0, ps, ns, pd, nd, 0, 0);
        if (n == 0)
            return 0;
        return pd[n - 1] == 0 ? n - 1 : n;
    }

#else

    return __str_convert((TMStr8)pd, nd, ps, ns);

#endif //#ifdef TM_WIN

}

size_t StrAuto2W(TMStrW pd, size_t nd, TMCStrA ps, size_t ns, TMBool* bUtf)
{
    size_t i = 0, j = 0;
    for (; i < nd && j < ns; )
    {
        if ((ns == size_t(-1)) && (ps[j] == 0))
            break;
        
        TMUInt32 u32;
        int n = __u8tow(u32, (TMCStr8)ps + j, ns - j);
        if (n == 0)
            break;
        else if (n < 0)
        {
            int x = StrConvert(CP_DEFAULT, pd + i, nd - i, ps + j, ns - j);
            *bUtf = false;
            return x + j;
        }

        i += __u32to16(pd + i, u32);
        j += n;
    }
    
    *bUtf = true;
    return i;
}


size_t StrA2A(int ndcp, TMStrA pd, size_t nd, int nscp, TMCStrA ps, size_t ns)
{
    TM::CStringW str;
    int n = StrConvert(nscp, str.GetBuffer(ns), ns, ps, ns);
    return StrConvert(ndcp, pd, nd, str, n);
}

template<class T>
inline size_t __StrLenT(const T* str, size_t maxsize)
{
    if (!str)
        return 0;
    size_t n = 0;  
    for (; n < maxsize && str[n]; ++n)  
        ;  
    return n;  
}

size_t StrLenA(TMCStrA str, size_t maxsize)
{
    return __StrLenT(str, maxsize);
}

size_t StrLenW(TMCStrW str, size_t maxsize)
{
    return __StrLenT(str, maxsize);
}


///////////////////////////////////////////////////////////////////////////////
// StrScanLong, StrScanDouble
template <class T>
struct _StrV
{
    const T* m_str;
    size_t m_size;
    size_t m_pos;
    bool m_bPositive;
    size_t m_posTemp;

    TMBool Init(const T* str, size_t size) // return: is IsEmpty
    {
        if (!str)
            return m_pos = 0, true;
        m_pos = StrSkipSpace(str, size);
        m_str = str;
        m_size = size;

        if (IsEmpty())
            return true;

        m_posTemp = m_pos;
        return SkipSign();
    }

    static inline bool IisDigit(int c)
    {
        return (c >= '0') && (c <= '9');
    }

    TMBool IsEmpty()
    {
        return m_pos >= m_size;
    }

    size_t ValidSize()
    {
        return m_size - m_posTemp;
    }

    TMBool SkipSign() // return: is IsEmpty
    {
        DbgAssert(!IsEmpty());
        T ch = m_str[m_posTemp];
        if (!ch)
            return true;
        if (ch == '-')
            return (++m_posTemp), (m_bPositive = false), IsEmpty();
        if (ch == '+')
            return (++m_posTemp), (m_bPositive = true), IsEmpty();
        m_bPositive = true;
        return false;
    }

    void ScanLong(TMLongPtr& lRet, unsigned int nBase)
    {
        DbgAssert(!IsEmpty());

        // check param err
        if (nBase == 1 || nBase > 36)
            return;
        
        size_t vSize = ValidSize();
        if (nBase == 0) // auto
        {
            // determine base free-lance, based on first two chars of string
            // sample:
            // 10: 123456
            // 16: 0x67f8
            //  8: 0345
            if (m_str[m_posTemp] != '0')
                nBase = 10;
            else if (vSize >= 2 && (m_str[m_posTemp + 1] == 'x' || m_str[m_posTemp + 1] == 'X'))
                (nBase = 16), (m_posTemp += 2);
            else
                (nBase = 8);
        }
        
        TMLongPtr ret = 0;
        size_t tmp = m_posTemp;
        for (; tmp < m_size; ++tmp)
        {
            unsigned int c = m_str[tmp];
            if (c >= '0' && c <= '9')
                c = c - '0';
            else if (c >= 'a')
                c = c - 'a' + 10;
            else if (c >= 'A')
                c = c - 'A' + 10;
            else
                break;
            
            if (c >= nBase)
                break;
            
            ret = ret * nBase + c;
        }
        
        if (tmp != m_posTemp)
        {
            m_pos = tmp;
            lRet = m_bPositive ? ret : -ret;
        }
    }

    void ScanDouble(double& dRet)
    {
        DbgAssert(!IsEmpty());

        double dbl = 0;
        int nDigit = 0;
        int nDot = 0;
        int nExp = 0;
        T ch;
        for (; m_posTemp < m_size; ++m_posTemp)
        {
            ch = m_str[m_posTemp];
            if (IisDigit(ch))
            {
                dbl = (dbl * 10) + (ch - '0');
                nExp -= nDot;
                ++nDigit;
            }
            else if (ch == '.' && nDot == 0)
                nDot = 1;
            else
                break;
        }
        
        if (0 == nDigit)
            return; // error
        
        m_pos = m_posTemp;
        if ((ch == 'e' || ch == 'E') && !IsEmpty())
        {
            bool bNegative = false;
            switch (ch = m_str[++m_posTemp])
            {
            case '-': bNegative = true; tm_fallthrough;
            case '+': ++m_posTemp;
            }
            
            // Process string of digits
            int n = 0;
            size_t nPos1 = m_posTemp;
            for (; nPos1 < m_size && IisDigit(ch = m_str[nPos1]); ++nPos1)
                n = n * 10 + (ch - '0');
            
            if (nPos1 != m_posTemp)
                m_pos = nPos1;
            nExp = bNegative ? nExp - n : nExp + n;
        }
        
        if (nExp < DBL_MIN_EXP  || nExp > DBL_MAX_EXP)
        {
            dbl = HUGE_VAL;
            goto _STRTODRET;
        }
        
        if (!m_bPositive)
            dbl = -dbl;
        if (nExp < 0)
        {
            nExp = -nExp;
            // this algorithm fast than "for (; nExp<0; ++nExp) { dbl /= 10.; }"
            for (double p10 = 10.; nExp; nExp >>= 1, p10 *= p10)
            {
                if (nExp & 1)
                    dbl /= p10;
            }
        }
        else
        {
            for (double p10 = 10.; nExp; nExp >>= 1, p10 *= p10)
            {
                if (nExp & 1)
                    dbl *= p10;
            }
        }
        
_STRTODRET:
        dRet = dbl;
    }
};


size_t StrScanLongA(TMCStrA str, size_t size, TMLongPtr& ret, int base)
{
    _StrV<TMCharA> sv;
    if (!sv.Init(str, size))
        sv.ScanLong(ret, base);
    return sv.m_pos;
}

size_t StrScanLongW(TMCStrW str, size_t size, TMLongPtr& ret, int base)
{
    _StrV<TMCharW> sv;
    if (!sv.Init(str, size))
        sv.ScanLong(ret, base);
    return sv.m_pos;
}

size_t StrScanDoubleA(TMCStrA str, size_t size, double& ret)
{
    _StrV<TMCharA> sv;
    if (!sv.Init(str, size))
        sv.ScanDouble(ret);
    return sv.m_pos;
}

size_t StrScanDoubleW(TMCStrW str, size_t size, double& ret)
{
    _StrV<TMCharW> sv;
    if (!sv.Init(str, size))
        sv.ScanDouble(ret);
    return sv.m_pos;
}


///////////////////////////////////////////////////////////////////////////////
// StrCompareA, StrCompareW
// here is known compiler issue:
//  A: VC2019::strnicmp("a", "A", -1) will assert if size > INT_MAX.
//  B: WIN::CompareString("\0zva", 2, "\0vza", 2) return 1.
//  C: WIN::CompareString("a", "A") = -1; but std::strcmp("a", "A") = 1;
template <class T, class F>
int _StrCompareT(F _f, const T* lhs, size_t lsize, const T* rhs, size_t rsize)
{
    // fix issue A.
    tm_setmin(lsize, INT_MAX);
    tm_setmin(rsize, INT_MAX);

    if (lhs == rhs && lsize == rsize)
        return 0;
    if (lhs == 0 || lsize == 0)
        return (rsize == 0 || rhs == 0 || rhs[0] == 0) ? 0 : -1;
    if (rhs == 0 || rsize == 0)
        return (lhs[0] == 0) ? 0 : 1;

    // if no strlen, compare("\0zva", 2, "\0vza", 3) will err.
    size_t sz;
    if (lsize < rsize)
    {
        lsize = StrLen(lhs, lsize);
        sz = lsize;
    }
    else if (rsize < lsize)
    {
        rsize = StrLen(rhs, rsize);
        sz = rsize;
    }
    else
        sz = lsize;
    
    int n = (*_f)(lhs, rhs, sz);
    if (n < 0)
        return -1;
    if (n > 0)
        return 1;
    if (lsize == rsize)
        return 0;
    if (lsize == sz)
        return rhs[sz] == 0 ? 0 : -1;

    return lhs[sz] == 0 ? 0 : 1;
}


int StrCompareA(TMCStrA lhs, size_t lsize, TMCStrA rhs, size_t rsize)
{
    return _StrCompareT(&strncmp, lhs, lsize, rhs, rsize);
}

int StrCompareW(TMCStrW lhs, size_t lsize, TMCStrW rhs, size_t rsize)
{
    return _StrCompareT(&wcsncmp, lhs, lsize, rhs, rsize);
}

int StrCompareIA(TMCStrA lhs, size_t lsize, TMCStrA rhs, size_t rsize)
{
    return _StrCompareT(&__strnicmp, lhs, lsize, rhs, rsize); // strcasecmp
}

int StrCompareIW(TMCStrW lhs, size_t lsize, TMCStrW rhs, size_t rsize)
{
    return _StrCompareT(&__wcsnicmp, lhs, lsize, rhs, rsize);
}



///////////////////////////////////////////////////////////////////////////////
// split function

// F: size_t f(const T* p, size_t& cur, size_t end) const;
//    if stop seach then set start = keyword pos; and return next token;
//    else return 0;
template <class T, class F>
inline size_t _str_split_if(const T* p1, size_t size, size_t& start, F _f)
{
    size_t i = start;
    if (!p1 || start >= size || !p1[start])
        return (start = 0), i;
    
    for (; ;)
    {
        int n = _f(p1, i, size);
        if (n)
            return static_cast<void>(start = n), i;

        ++i;
        
        if (!p1[i] || i >= size)
            return start = i;
    }
}

////////////////////////////////////////////////////////////////////////////////
// StrSplitIf
size_t StrSplitIfA(TMCStrA ps, size_t ns, size_t& cur, PfnSpliteA f)
{
    return _str_split_if(ps, ns, cur, f);
}

size_t StrSplitIfW(TMCStrW ps, size_t ns, size_t& cur, PfnSpliteW f)
{
    return _str_split_if(ps, ns, cur, f);
}

////////////////////////////////////////////////////////////////////////////////
// StrSplitLine
struct _str_split_if_line
{
    template <class T> size_t operator()(const T* p, size_t cur, size_t nLen) const
    {
        T c = p[cur];
        if (c == '\r' || c == '\n')
            return cur + (((nLen - cur) >= 2 && p[cur + 1] == '\r' + '\n' - c) ? 2 : 1);
        return 0;
    }
};

size_t StrSplitLineA(TMCStrA ps, size_t ns, size_t& cur)
{
    return _str_split_if(ps, ns, cur, _str_split_if_line());
//     return _str_split_line(ps, ns, cur);
}

size_t StrSplitLineW(TMCStrW ps, size_t ns, size_t& cur)
{
    return _str_split_if(ps, ns, cur, _str_split_if_line());
}


////////////////////////////////////////////////////////////////////////////////
// StrSplitChr
template <class T> struct _str_split_if_chr
{
    T vol;
    _str_split_if_chr(T v) : vol(v) {}
    size_t operator() (const T* p, size_t cur, size_t) const { return (p[cur] == vol) ? cur + 1 : 0; }
};

size_t StrSplitChrA(TMCStrA ps, size_t ns, size_t& cur, TMCharA ch)
{
    return _str_split_if(ps, ns, cur, _str_split_if_chr<TMCharA>(ch));
}

size_t StrSplitChrW(TMCStrW ps, size_t ns, size_t& cur, TMCharW ch)
{
    return _str_split_if(ps, ns, cur, _str_split_if_chr<TMCharW>(ch));
}


////////////////////////////////////////////////////////////////////////////////
// StrSplitChrI
template <class T> struct _str_split_if_chri
{
    T vol;
    _str_split_if_chri(T v) : vol(StrToLower(v)) {}
    size_t operator() (const T* p, size_t cur, size_t) const { return (StrToLower(p[cur]) == vol) ? cur + 1 : 0; }
};

size_t StrSplitChrIA(TMCStrA ps, size_t ns, size_t& cur, TMCharA ch)
{
    return _str_split_if(ps, ns, cur, _str_split_if_chri<TMCharA>(ch));
}

size_t StrSplitChrIW(TMCStrW ps, size_t ns, size_t& cur, TMCharW ch)
{
    return _str_split_if(ps, ns, cur, _str_split_if_chri<TMCharW>(ch));
}


////////////////////////////////////////////////////////////////////////////////
// StrSplitStr
template <class T> struct _str_split_if_str
{
    const T* m_p;
    size_t m_size;
    _str_split_if_str(const T* pk, size_t nk) : m_p(pk), m_size(nk) {}
    size_t operator() (const T* p, size_t& cur, size_t nLen) const
    {
        size_t nRemain = nLen - cur;
        if (nRemain < m_size)
            return cur = nLen;
        if (p[cur] != m_p[0])
            return 0;
        if (m_size == 1)
            return cur + 1;
        return StrCompare(p + cur + 1, m_size - 1, m_p + 1, m_size - 1) ? 0 : cur + m_size;
    }
};

size_t StrSplitStrA(TMCStrA ps, size_t ns, size_t& cur, TMCStrA pk, size_t nk)
{
    DbgAssert(pk);
    DbgAssert(nk);
    return _str_split_if(ps, ns, cur, _str_split_if_str<TMCharA>(pk, nk));
}

size_t StrSplitStrW(TMCStrW ps, size_t ns, size_t& cur, TMCStrW pk, size_t nk)
{
    DbgAssert(pk);
    DbgAssert(nk);
    return _str_split_if(ps, ns, cur, _str_split_if_str<TMCharW>(pk, nk));
}

////////////////////////////////////////////////////////////////////////////////
// StrSplitOne
template <class T> struct _str_split_one_of
{
//    TM::CStrViewT<T> val;
    std::_tm_string_view<T> val;
    _str_split_one_of(const T* v, int sz) : val(v, sz) {}
    size_t operator() (const T* p, size_t cur, size_t) const { return (val.find(p[cur]) == val.npos) ? 0 : cur + 1; }
};

size_t StrSplitOneA(TMCStrA ps, size_t ns, size_t& cur, TMCStrA pk, size_t nk)
{
    DbgAssert(pk);
    DbgAssert(nk);
    return _str_split_if(ps, ns, cur, _str_split_one_of<TMCharA>(pk, nk));
}

size_t StrSplitOneA(TMCStrW ps, size_t ns, size_t& cur, TMCStrW pk, size_t nk)
{
    DbgAssert(pk);
    DbgAssert(nk);
    return _str_split_if(ps, ns, cur, _str_split_one_of<TMCharW>(pk, nk));
}

////////////////////////////////////////////////////////////////////////////////
// StrSplitStrI
template <class T> struct _str_split_if_stri
{
    const T* m_p;
    size_t m_size;
    _str_split_if_stri(const T* pk, size_t nk) : m_p(pk), m_size(nk) {}
    size_t operator() (const T* p, size_t& cur, size_t nLen) const
    {
        size_t nRemain = nLen - cur;
        if (nRemain < m_size)
            return cur = nLen;
        return StrCompareI(p + cur, m_size, m_p, m_size) ? 0 : cur + m_size;
    }
};

size_t StrSplitStrIA(TMCStrA ps, size_t ns, size_t& cur, TMCStrA pk, size_t nk)
{
    DbgAssert(pk);
    DbgAssert(nk);
    return _str_split_if(ps, ns, cur, _str_split_if_stri<TMCharA>(pk, nk));
}

size_t StrSplitStrIW(TMCStrW ps, size_t ns, size_t& cur, TMCStrW pk, size_t nk)
{
    DbgAssert(pk);
    DbgAssert(nk);
    return _str_split_if(ps, ns, cur, _str_split_if_stri<TMCharW>(pk, nk));
}


///////////////////////////////////////////////////////////////////////////////
// url encode, decode
int StrUrlEnc(const char* pSouce, char* pDest, int n)
{
    const char* h = "0123456789abcdef";

    DbgAssert(pSouce && pDest);

    int j = 0;
    for (int i = 0; i < n; i++)
    {
        const char c = *pSouce++;

        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')
            || ('0' <= c && c <= '9') || (c == '-')
            || (c == '_') || (c == '.'))
            pDest[j++] = c;
        else if (c == ' ')
            pDest[j++] = '+';
        else if (!c)
            break;
        else
        {
            pDest[j + 0] = '%';
            pDest[j + 1] = h [((TMUInt8)c) >> 4];
            pDest[j + 2] = h [c & 0x0f ];
            j += 3;
        }
    }

    pDest[j] = 0;
    return j;
}

int StrUrlDec(const char* pSouce, char* pDest, int ns)
{
    DbgAssert(pSouce && pDest);

    int n = 0;
    for (int i = 0; i < ns; ++i)
    {
        char c = *pSouce++;

        if (!c) // eof
            break;

        if ('%' == c)
        {
            char c1 = tolower(pSouce[0]);
            char c2 = tolower(pSouce[1]);
            pSouce += 2;
            if (!c1 || !c2)
                break;

            if (c1 <= '9')
                c1 = c1 - '0';
            else if (c1 >= 'a' && c1 <= 'f')
                c1 = c1 - 'a' + 10;
            else
                continue;

            if (c2 <= '9')
                c2 = c2 - '0';
            else if (c2 >= 'a' && c2 <= 'f')
                c2 = c2 - 'a' + 10;
            else
                continue;

            pDest[n++] = ((c1 << 4) + c2);
        }
        else if (c == '+')
            pDest[n++] = ' ';
        else
            pDest[n++] = c;
    }

    pDest[n] = 0;
    return n;
}


/*
// Strip off any extension (dot something) from end of file,
// IF one exists. Inserts zero into buffer.

void wxStripExtension(char *buffer)
{
    int len, i;

    len = strlen(buffer);
    i = len-1;
    while (i > 0)
    {
        if (buffer[i] == '.')
        {
            buffer[i] = 0;
            break;
        }
        i --;
    }
}

// Return just the filename, not the path
// (basename)
char *
wxFileNameFromPath (char *path)
{
    if (path) {
        char *tcp = path;
        int tcpd;

        tcpd = strlen(path);
        while (--tcpd >= 0) {
            if ((tcp[tcpd] == '/') || (tcp[tcpd] == '\\'))
                return copystring(tcp, tcpd + 1);
        }
        if (isalpha(path[0]) && path[1] == ':')
            return copystring(path, 2);
    }

    return path;
}

// Return just the directory, or NULL if no directory
char *
wxPathOnly(char *path)
{
    if (path && *path) {
        char *buf;
        int i, l;
        Bool done = FALSE;

        buf = copystring(path);

        l = strlen(path);

        i = l - 1;

        // Search backward for a backward or forward slash
        while (!done && i > -1) {
            if (path[i] == '/' || path[i] == '\\') {
                done = TRUE;
                buf[i] = 0;
                return buf;
            } else
                i --;
        }

        // Try Drive specifier
        if (isalpha (buf[0]) && buf[1] == ':') {
            // A:junk --> A:. (since A:.\junk Not A:\junk)
            buf[2] = '.';
            buf[3] = '\0';
            return buf;
        }
    }

    return NULL;
}



  char *wxGetTempFileName (const char *prefix, char *dest)
  {
  static char *temp_folder;
  static int temp_len;
  static short last_temp = 0;   // cache last to speed things a bit
  // At most 1000 temp files to a process! We use a ring count.
  char *buf;

    if (!temp_folder) {
    FSSpec spec;
    SInt16 vRefNum;
    SInt32 dirID;
    const Str255 fileName = "\p";

      if (FindFolder(kOnSystemDisk, 'temp', kCreateFolder, &vRefNum, &dirID) == noErr) {
      wxREGGLOB(temp_folder);
      FSMakeFSSpec(vRefNum,dirID,fileName,&spec);
      temp_folder = scheme_mac_spec_to_path(&spec);
      }
      else
      temp_folder = "";
      temp_len = strlen(temp_folder);
      }

        if (!prefix)
        prefix = "";
        else {
        int i;
        for (i = 0; prefix[i]; i++) {
        if (prefix[i] == ':') {
        prefix = "";
        break;
        }
        }
        if (i > 15)
        prefix = "";
        }

          buf = new char[temp_len + strlen(prefix) + 20];

            for (short suffix = last_temp + 1; suffix != last_temp; ++suffix %= 1000) {
            #ifdef OS_X
            struct stat stbuf;
            #endif
            sprintf (buf, "%s_%s%d", temp_folder, prefix, (int) suffix);
            if (stat ((char *)buf, &stbuf) != 0) {
            // Touch the file to create it (reserve name)
            FILE *fd;
            fd = fopen (buf, "w");
            if (fd) {
            fclose (fd);
            last_temp = suffix;
            if (dest)
            strcpy(dest, buf);
            else
            dest = copystring(buf);
            return dest;
            }
            }
            }
            if (dest) dest[0] = 0;
            return dest;
            }



  Bool wxDirExists(char *dir)
  {
  struct stat sbuf;
  return ((stat(dir, &sbuf) != -1) && S_ISDIR(sbuf.st_mode) ? TRUE : FALSE);
  }

    Bool wxFileExists(char *filename)
    {
    struct stat stbuf;

      return (filename && !stat((char *)filename, &stbuf));
      }

        Bool wxRemoveFile(char *file)
        {
        return !unlink(file);
        }


 */


////////////////////////////////////////////////////////////////////////////////
// strncpy
template <class T>
static size_t _safstr_ncpyT(T* pd, size_t nd, const T* ps, size_t ns)
{
    if (!ps)
        ns = 0;

    size_t i = 0;
    for (; ; ++i)
    {
        if (i == ns || i == nd)
        {
            if (pd[i]) // pd[i] may be can't access to write
                pd[i] = 0;
            break;
        }
        else if (0 == (pd[i] = ps[i]))
        {
            break;
        }
    }
    return i;
}

template < class T >
inline static size_t str_ncpyT(T* pd, size_t nd, const T* ps, size_t ns)
{
    DbgAssert(nd ? 0 != pd : 1);
    size_t i = nd > 0 ? _safstr_ncpyT(pd, nd - 1, ps, ns) : 0;
    return i;
}

size_t StrCopyA(TMStrA pd, size_t nd, TMCStrA ps, size_t ns)
{
    return str_ncpyT(pd, nd, ps, ns);
}

size_t StrCopyW(TMStrW pd, size_t nd, TMCStrW ps, size_t ns)
{
    return str_ncpyT(pd, nd, ps, ns);
}


////////////////////////////////////////////////////////////////////////////////
// strmatch
template <class T>
static bool WildcardCompare(const T* p, const T* pKey, T cSplit)
{
    T ckey = pKey[0];
    T cp = p[0];

    if (cp == 0 && (ckey == 0 || ckey == cSplit))
    {
        return true;
    }

    if (ckey == '*')
    {
        do
        {
            bool b = WildcardCompare(p, pKey + 1, cSplit);
            if (b)
            {
                return true;
            }
        }
        while (*p++);

        return false;
    }

    if (cp == 0 || ckey == 0 || ckey == cSplit || (cp != ckey && ckey != '?'))
    {
        return false;
    }

    return WildcardCompare(p+1, pKey+1, cSplit);
}

template <class T>
static bool WildcardCompares(const T* p, const T* pKey, T cSplit)
{
    for (;;)
    {
        if (WildcardCompare(p, pKey, cSplit))
        {
            return true;
        }

        for (;;)
        {
            T c = *pKey++;
            if (c == 0)
            {
                return false;
            }
            if (c == cSplit)
            {
                break;
            }
        }
    }
}

template <class T>
static bool str_match(const T* pSrc,
                      const T* pKey, T cSplit, TMBool bCase)
{
    DbgAssert(pSrc && pKey);

    if (bCase)
    {
        return WildcardCompares(pSrc, pKey, cSplit);
    }
    else
    {
        int s1 = StrLen(pSrc) + 1;
        int s2 = StrLen(pKey) + 1;
        int n1 = s1 * sizeof(T);
        int n2 = s2 * sizeof(T);
        T* p1 = (T*)alloca(n1);
        T* p2 = (T*)alloca(n2);
        strLower(p1, pSrc, s1);
        strLower(p2, pKey, s2);
        return WildcardCompares(p1, p2, cSplit);
    }
}

TMBool StrMatchA(TMCStrA pSrc, TMCStrA pKey, TMCharA cSplit, TMBool bCase)
{
    return str_match(pSrc, pKey, cSplit, bCase);
}

TMBool StrMatchW(TMCStrW pSrc, TMCStrW pKey, TMCharW cSplit, TMBool bCase)
{
    return str_match(pSrc, pKey, cSplit, bCase);
}


////////////////////////////////////////////////////////////////////////////////
// stream function
TMResult StreamCreateFromMem(const void* pMem, size_t size, IStream **ppstm)
{
    TM::CStreamT<TM::CMemFile>* p = new TM::CRefObjectT<TM::CStreamT<TM::CMemFile> >(1);
    p->Assign((const TMUInt8*)pMem, size);
    *ppstm = p;
    return S_OK;
}

TMResult StreamCreateFromStaticMem(const void* pMem, size_t size, IStream **ppstm)
{
    TM::CStreamT<TM::CMemBuf>* p = new TM::CRefObjectT<TM::CStreamT<TM::CMemBuf> >(1);
    p->Assign((const TMUInt8*)pMem, size);
    *ppstm = p;
    return S_OK;
}

TMResult StreamCreateFromResource(TMHModule hModule, TMCStr pName, TMCStr pType, IStream **ppstm)
{
    TM::CResource res;
    res.Load(hModule, pName, pType);
    if (!res)
        return E_FAIL;
    return StreamCreateFromStaticMem(res.GetData(), res.GetSize(), ppstm);
}

TMResult StreamCreateFromFile(TMCStr pFileName, TMUInt access, IStream **ppstm)
{
    TM::CStreamT<TM::CFile>* p = new TM::CRefObjectT<TM::CStreamT<TM::CFile> >(1);
    p->OpenExisting(pFileName, (TM::CFile::EAccess)access);
    if (!*p)
    {
        p->Release();
        return E_FAIL;
    }
    *ppstm = p;
    return S_OK;
}

class CSubStream
    : public TM::IStreamImpl
{
public:
    TM::LLINT m_nStart;
    int m_nSize;
    TM::CRefPtr<IStream> m_p;

    TM_UNUSED_IC_GetDebugInfo();

    void Construct(IStream* p, int nSize)
    {
        ULARGE_INTEGER l;
        p->Seek(TM::LLINT(0), SEEK_CUR, &l);
        m_nStart = l;
        m_nSize = nSize;
        m_p = p;
    }
    
    virtual ComResult STDMETHODCALLTYPE Read(void *pv, TMULong cb, TMULong *pcbRead)
    {
        return m_p->Read(pv, cb, pcbRead);
    }
    
    virtual ComResult STDMETHODCALLTYPE Write(const void *pv, TMULong cb, TMULong *pcbWritten)
    {
        return m_p->Write(pv, cb, pcbWritten);
    }
    
    // STREAM_SEEK_SET, STREAM_SEEK_CUR, STREAM_SEEK_END
    virtual ComResult STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove,
        COMDword dwOrigin, ULARGE_INTEGER *plibNewPosition)
    {
        ComResult hr;
        switch (dwOrigin)
        {
        case SEEK_SET:
            hr = m_p->Seek(m_nStart + dlibMove, dwOrigin, plibNewPosition); break;
        case SEEK_CUR:
            hr = m_p->Seek(m_nStart + dlibMove, dwOrigin, plibNewPosition); break;
        case SEEK_END:
            hr = m_p->Seek(m_nStart + m_nSize + dwOrigin, SEEK_SET, plibNewPosition); break;
        default:
            return E_INVALIDARG;
        }
        if (plibNewPosition && hr == S_OK)
            plibNewPosition->QuadPart -= m_nStart.m.QuadPart;
        return hr;
    }
    
    virtual ComResult STDMETHODCALLTYPE Stat(STATSTG *pstatstg,
        COMDword grfStatFlag)
    {
        if (pstatstg)
        {
            MemZeroT(*pstatstg);
            pstatstg->type = STGTY_STREAM;
            pstatstg->cbSize.QuadPart = m_nSize;
            //pstatstg->mtime = fstatus.m_mtime;
            //pstatstg->ctime = fstatus.m_ctime;
            //pstatstg->atime = fstatus.m_atime;
            //pstatstg->grfMode = ;
            //             pstatstg->grfLocksSupported = 0;     // no locking supported
            return S_OK;
        }
        return E_FAIL;
    }
};

TMResult StreamCreateCopy(IStream* p, size_t size, IStream **ppstm)
{
    if (!p)
        return E_FAIL;
    CSubStream* pstream = new TM::CRefObjectT<CSubStream>(1);
    pstream->Construct(p, size);
    *ppstm = pstream;
    return S_OK;
}

TM_API TMResult StreamCreateReference(IStream* p, size_t size, IStream **ppstm, void** pMem)
{
    DbgAssert(ppstm);
    DbgAssert(pMem);
    typedef TM::CStreamT<TM::CMemFile> mem_file;
    mem_file* pstream = new TM::CRefObjectT<mem_file>(1);
    if (!pstream->ReadFromStream(p, size))
    {
        pstream->Release();
        return E_FAIL;
    }
    *pMem = pstream->GetHead();
    *ppstm = pstream;
    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// once function
#ifndef TM_MSC
TM::CCritical& _GetOnceCrical()
{
    static TM::CCritical s_cri;
    return s_cri;
}
#endif // TM_MSC

void OnceLock()
{
#ifdef TM_MSC
    HeapLock(GetProcessHeap());
#else
    _GetOnceCrical().Lock();
#endif
}

void OnceUnlock()
{
#ifdef TM_MSC
    HeapUnlock(GetProcessHeap());
#else
    _GetOnceCrical().Unlock();
#endif
}



///////////////////////////////////////////////////////////////////////////////
// ColorFromSys
#ifdef TM_GTK

// s_clrTheme[0].alpha: is cache inited;
// s_clrTheme[1].alpha: is listener install;
static GdkRGBA s_clrTheme[31];

static void _OnThemeChanged(GtkSettings* settings, GParamSpec* pspec, gpointer data)
{
    s_clrTheme[0].alpha = 0;
}

static void _ensure_theme_listener()
{
    if (s_clrTheme[1].alpha != 0)
        return;

    GtkSettings* settings = gtk_settings_get_default();
    if (!settings)
        return;

    gulong _themeSignal = g_signal_connect(settings,
        "notify::gtk-theme-name",
        G_CALLBACK(_OnThemeChanged),
        NULL);
    tm_unused(_themeSignal);

//     gulong _darkSignal = g_signal_connect(settings,
//         "notify::gtk-application-prefer-dark-theme",
//         G_CALLBACK(_OnThemeChanged),
//         NULL);
}

// static GdkRGBA _clr_theme(const char* spec)
// {
//     GdkRGBA color;
//     TMBool b = gdk_rgba_parse(&color, spec);
//     DbgAssert(b);
//     return color;
// }

static GdkRGBA _clr_alpha(GdkRGBA color, float alpha)
{
    color.alpha = alpha;
    return color;
}

static GdkRGBA _clr_shade(GdkRGBA color, float factor)
{
    color.red   *= factor;
    color.green *= factor;
    color.blue  *= factor;
    tm_setmin(color.red,    1.0f);
    tm_setmin(color.green,  1.0f);
    tm_setmin(color.blue,   1.0f);
    return color;
}

struct CThemeDef
{
    GdkRGBA normal_bg;
    GdkRGBA normal_fg;
    GdkRGBA selected_bg;
    GdkRGBA selected_fg;
    GdkRGBA insensitive_fg;
    GdkRGBA borders;
    GdkRGBA link;
    GdkRGBA warning_bg;
    GdkRGBA warning_fg;
    GdkRGBA accent_bg;
    GdkRGBA headerbar_bg;
    GdkRGBA insensitive_bg;

    CThemeDef()
    {
#if TM_GTK >= 400
        DbgVerify(gdk_rgba_parse(&normal_bg,        "@theme_bg_color"));
        DbgVerify(gdk_rgba_parse(&normal_fg,        "@theme_fg_color"));
        DbgVerify(gdk_rgba_parse(&selected_bg,      "@theme_selected_bg_color"));
        DbgVerify(gdk_rgba_parse(&selected_fg,      "@theme_selected_fg_color"));
        DbgVerify(gdk_rgba_parse(&insensitive_fg,   "@insensitive_fg_color"));
        DbgVerify(gdk_rgba_parse(&borders,          "@borders"));
        DbgVerify(gdk_rgba_parse(&link,             "@link_color"));
        DbgVerify(gdk_rgba_parse(&warning_bg,       "@warning_bg_color"));
        DbgVerify(gdk_rgba_parse(&warning_fg,       "@warning_fg_color"));
        DbgVerify(gdk_rgba_parse(&accent_bg,        "@accent_bg_color"));
        DbgVerify(gdk_rgba_parse(&headerbar_bg,     "@headerbar_bg_color"));
        DbgVerify(gdk_rgba_parse(&insensitive_bg,   "@insensitive_bg_color"));
#else
        // GTK3: use a temporary widget to obtain a valid style context with theme data
        GtkWidget* dummy = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkStyleContext* context = gtk_widget_get_style_context(dummy);
        gtk_style_context_get_background_color      (context, GTK_STATE_FLAG_NORMAL,      &normal_bg);
        gtk_style_context_get_color                 (context, GTK_STATE_FLAG_NORMAL,      &normal_fg);
        gtk_style_context_get_background_color      (context, GTK_STATE_FLAG_SELECTED,    &selected_bg);
        gtk_style_context_get_color                 (context, GTK_STATE_FLAG_SELECTED,    &selected_fg);
        gtk_style_context_get_color                 (context, GTK_STATE_FLAG_INSENSITIVE, &insensitive_fg);
        gtk_widget_destroy(dummy);

        borders                                     = _clr_shade(normal_bg, 0.5f);
        link                                        = GdkRGBA{0.0f, 0.0f, 1.0f, 1.0f};
        warning_bg                                  = _clr_shade(normal_bg, 1.1f);
        warning_fg                                  = normal_fg;
        accent_bg                                   = selected_bg;
        headerbar_bg                                = normal_bg;
        insensitive_bg                              = _clr_shade(normal_bg, 0.95f);
#endif
    }
};

GdkRGBA GdkColorFromSys(int nIndex)
{
    DbgAssert(nIndex < tm_countof(s_clrTheme));

    if (s_clrTheme[0].alpha == 0) tm_unlikely
    {
        _ensure_theme_listener();

        // ========== Base colors (direct GTK CSS variable mapping) ==========
        CThemeDef def;

        // Background colors
        s_clrTheme[eColorIndexWindow]                   = def.normal_bg;        // window background
        s_clrTheme[eColorIndexMenu]                     = def.normal_bg;        // menu background
        s_clrTheme[eColorIndexBtnFace]                  = def.normal_bg;        // button face
        s_clrTheme[eColorIndexBackground]               = def.normal_bg;        // desktop background
        s_clrTheme[eColorIndexAppWorkspace]             = def.normal_bg;        // workspace background
        s_clrTheme[eColorIndexScrollBar]                = def.normal_bg;        // scrollbar background

        // Text colors
        s_clrTheme[eColorIndexWindowText]               = def.normal_fg;        // window text
        s_clrTheme[eColorIndexMenuText]                 = def.normal_fg;        // menu text
        s_clrTheme[eColorIndexBtnText]                  = def.normal_fg;        // button text
        s_clrTheme[eColorIndexCaptionText]              = def.normal_fg;        // caption text

        // Highlight/selection colors
        s_clrTheme[eColorIndexHighlight]                = def.selected_bg;      // highlight background
        s_clrTheme[eColorIndexActiveCaption]            = def.selected_bg;      // active caption
        s_clrTheme[eColorIndexHighlightText]            = def.selected_fg;      // highlight text

        // Disabled/grayed text
        s_clrTheme[eColorIndexGrayText]                 = def.insensitive_fg;   // disabled text
        s_clrTheme[eColorIndexInactiveCationText]       = def.insensitive_fg;   // inactive caption text

        // Border colors
        s_clrTheme[eColorIndexWindowFrame]              = def.borders;          // window frame
        s_clrTheme[eColorIndex3DDKShadow]               = def.borders;          // 3D dark shadow

        // Link color
        s_clrTheme[eColorIndexHotLight]                 = def.link;             // hot link

        // Tooltip colors
        s_clrTheme[eColorIndexInfoBK]                   = def.warning_bg;       // info background
        s_clrTheme[eColorIndexInfoText]                 = def.warning_fg;       // info text

        // Accent color
        s_clrTheme[eColorIndexActiveBorder]             = def.accent_bg;        // active border
//         s_clrTheme[eColorIndexActiveBorder]         = clr_bg_selected;

        // Menu bar
        s_clrTheme[eColorIndexMenuBar]                  = def.headerbar_bg;     // menu bar background

        // ========== Derived colors (brightness/alpha adjustments) ==========

        // Shadow effects (darken background by 30%)
        s_clrTheme[eColorIndexBtnShadow]                = _clr_shade(def.normal_bg, 0.7f);      // button shadow

        // Highlight effects (brighten background by 30%)
        s_clrTheme[eColorIndexBtnHightlight]            = _clr_shade(def.normal_bg, 1.3f);      // button highlight
        s_clrTheme[eColorIndex3DLight]                  = _clr_shade(def.normal_bg, 1.3f);      // 3D light
        s_clrTheme[eColorIndexMenuLight]                = _clr_shade(def.normal_bg, 1.2f);      // menu highlight

        // Inactive states
        s_clrTheme[eColorIndexInactiveBorder]           = def.insensitive_bg;                   // inactive border
        s_clrTheme[eColorIndexInactiveCaption]          = def.insensitive_bg;                   // inactive caption

        // Gradient effects (with transparency)
        s_clrTheme[eColorIndexGradientActiveCaption]    = _clr_alpha(def.selected_bg, 0.8f);    // gradient active caption
        s_clrTheme[eColorIndexGradientInactiveCaption]  = _clr_alpha(def.insensitive_bg, 0.6f); // gradient inactive caption
    }

    return s_clrTheme[nIndex];
}

#endif // TM_GTK

