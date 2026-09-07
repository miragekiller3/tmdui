/**
 *  @file
 *  @brief  dynamic-link library support
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2005.4.16
 *
 *  Copyright (C) 2005 miragekiller
 */

#pragma once

#ifndef TMDLL_H
#define TMDLL_H

#include "tmdebug.h"


#ifdef TM_WIN

#   include <windows.h>

    typedef struct { void* p; int m; }  TMResData;

#elif defined (TM_APPLE)

#   include "mtcore.h"
//#include <ApplicationServices/ApplicationServices.h>
#   include <CoreGraphics/CGDataProvider.h>
    typedef CFDataRef                   TMResData;

#else

#   if defined(TM_GTK)

        typedef struct { GBytes* p; }  TMResData;

#       include <gmodule.h> 

#       define __dlopen                 g_module_open
#       define __dlsym                  g_module_symbol
#       define __dlclose                g_module_close

        inline static void* __dlsym(GModule* module, const gchar* symbol_name)
        {
            gpointer symbol;
            if (g_module_symbol(module, symbol_name, &symbol)) tm_likely
                return symbol;
            return 0;
        }

#   else

#       include <unistd.h>
#       include <dlfcn.h>

    typedef struct { void* p; int m; }  TMResData;

#       define __dlopen                 dlopen
#       define __dlsym                  dlsym
#       define __dlclose                dlclose

#   endif // TM_GTK

#endif // #ifdef TM_WIN


/**
 *  @defgroup DLL
 */


/**
 *  @addtogroup DLL
 *  dll "c" function prototype
 *  @{
 */


/**
 *  maps the specified executable module into the address space of the calling process.
 *  see LoadLibrary in MSDN.
 */
#ifndef LOAD_LIBRARY_SEARCH_APPLICATION_DIR
// #   define LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR     0x00000100   // add dll to search path, must full path
#   define LOAD_LIBRARY_SEARCH_APPLICATION_DIR  0x00000200   // c:\1.exe -> c:\1\1.dll can only can load c:\1.dll
//#   define LOAD_LIBRARY_SEARCH_USER_DIRS        0x00000400   // only load path: AddDllDirectory or SetDllDirectory
#   define LOAD_LIBRARY_SEARCH_SYSTEM32         0x00000800   // only load path system32
#   define LOAD_LIBRARY_SEARCH_DEFAULT_DIRS     0x00001000   // only load APPLICATION_DIR, USER_DIRS, SYSTEM32
#endif // LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
#define LOAD_LIBRARY_SEARCH_MASK                0x00001f00

TM_CAPI TMHModule DllLoad(TMCStr pDllName, TMUInt32 dwFlag);


/**
 *  load a module from path by hDir.
 */
TM_CAPI TMHModule DllLoadFrom(TMCStr pDllName, TMHModule hDir);


/**
 *  retrieves the address of an exported function or variable from the specified dynamic-link library (DLL).
 *  see GetProcAddress in MSDN.
 */
TM_INLINE void* DllSym(TMHModule hModule, TMCStrA pFuncName)
{
//     MessageBoxA(0, pFuncName, "", MB_OK);
#ifdef TM_WIN
    return (void*)GetProcAddress(hModule, pFuncName);
#elif defined (TM_APPLE)
    CFStringRef str;
    void* p;
    str = CFStringCreateWithCString(0, pFuncName, kCFStringEncodingUTF8);
    p = CFBundleGetFunctionPointerForName(hModule, str);
    CFRelease(str);
    return p;
#else
    return __dlsym (hModule, pFuncName);
#endif
}

TM_INLINE void* _DllSymSafe(TMHModule hModule, TMCStrA pFuncName)
{
    return hModule ? DllSym(hModule, pFuncName) : 0;
}


/**
 *  the module is unmapped from the address space of the calling process and the handle is no longer valid.
 *  see FreeLibrary in MSDN.
 */
TM_INLINE TMBool DllFree(TMHModule hModule)
{
#ifdef TM_WIN
    return FreeLibrary(hModule);
#elif defined (TM_APPLE)
    CFBundleUnloadExecutable(hModule);
    CFRelease(hModule);
    return true;
#else
    return __dlclose (hModule);
#endif
}


/**
 * the full path and file name for the file containing the specified module.
 * @remark pBuf must not be 0, buffer size must >= PATH_MAX.
 */
TM_INLINE TMBool DllName(TMHModule hModule, TMStr pBuf)
{
#if TM_WIN_DESKTOP
    return GetModuleFileName(hModule, pBuf, PATH_MAX);
#elif TM_WIN_METRO
    DbgAssert(0);
    return false;
#elif defined (TM_APPLE)
    TMBool n = 0; TMHModule hMain = 0; CFURLRef url = 0;
    if (!hModule)
        hModule = hMain = CFBundleGetMainBundle();
    url = CFBundleCopyBundleURL(hModule);
    n = CFURLGetFileSystemRepresentation(url, true, (UInt8*)pBuf, PATH_MAX);
    CFRelease(url);
    if (hMain)
        CFRelease(hMain);
    return n;
/*
#elif 1
    int mib[4];
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;
    char buf[1024];
    size_t size = sizeof(buf);
    sysctl(mib, 4, buf, &size, NULL, 0);
    if (size == 0 || size == sizeof(buf)) return 0;
    return size;

    char* p = getenv("_");
    return 0;*/
#else
    int n;
// Linux:
//     /proc/<pid>/exe
// Solaris:
//     /proc/<pid>/object/a.out (filename only)
//         /proc/<pid>/path/a.out (complete pathname)
// *BSD (and maybe Darwing too):
//     /proc/<pid>/file
    if (!hModule)
    {
#ifdef TM_GTK
        gchar* exe_path = g_file_read_link("/proc/self/exe", NULL);
        if (exe_path)
        {
            n = g_strlcpy(pBuf, exe_path, PATH_MAX);
            g_free(exe_path);
        }
        else
            n = 0;
#else
        n = readlink("/proc/self/exe", pBuf, PATH_MAX);
#endif
    }
    else
    {
        DbgAssert(0);
        return 0;
//         sprintf(c, "/proc/%d/fd/%d", getpid(), fileno(hModule));
//         n = readlink(c, pBuf, PATH_MAX);
    }
    if (n < 0 || n > PATH_MAX)
        n = 0;
    pBuf[n] = 0;
    return n;
#endif
}

/**
 * get the module handle for the specified module if the file has been mapped into the address space of the calling process.
 */
TM_INLINE TMHModule DllHandle(TMCStr pName)
{
#ifdef TM_WIN_DESKTOP
    return GetModuleHandle(pName);
#elif TM_WIN_METRO
    DbgAssert(0);
    return 0;
#else
    TMHModule hm = DllLoad(pName, 0);
    if (hm)
    {
        TM_IF_MAC (DbgAssert(CFGetRetainCount(hm) > 1));
        DllFree(hm);
    }
    return hm;
#endif
}

/*
//DllString
//test code in windows
char strBuf[MAX_PATH] = {0};
DllString(DllLoad(("E:\\Projects\\Connect\\US_Main_2\\bin\\client\\Debug\\apExtRes.dll")),
1, strBuf, MAX_PATH);
//test code in mac
 ///////////////////////
 // Cliff
 char strBuf[1000] = {0};
 DllString(DllLoad("/Volumes/dada/TestCode/TBundle/build/Debug/TBundle.bundle"),
 "FirstMacString", strBuf, 1000, "InfoPlist");
 ///////////////////////

*/
TM_INLINE TMInt DllString(TMHModule hMod, TMUIntPtr uID, TMStr pBuf, TMInt nBufferMax, TMStr tableName = NULL)
{
#ifdef TM_WIN_DESKTOP

    return LoadString(hMod, uID, pBuf, nBufferMax);

#elif TM_WIN_METRO

    DbgAssert(0);
    return 0;

#elif defined(TM_APPLE)
    DbgAssert(0);
    TMCStrA key = (TMCStrA)uID;
    CFStringRef strkey = CFStringCreateWithCString(0, key, kCFStringEncodingUTF8);
    CFStringRef strtableName = (tableName == NULL) ? NULL : CFStringCreateWithCString(0, tableName, kCFStringEncodingUTF8);

    CFStringRef strresult = CFBundleCopyLocalizedString(hMod, strkey, CFSTR(""), strtableName);
    CFRelease(strkey);
    if (strtableName)
        CFRelease(strtableName);

    TMBool bRet = CFStringGetCString(strresult, pBuf, nBufferMax, kCFStringEncodingASCII);
    int retLen = 0;
    if (bRet)
        retLen = CFStringGetLength(strresult);
    CFRelease(strresult);
    return retLen;
#else
    return 0;
#endif
}


/**
 * resource function
 */
TM_INLINE void ResInit(TMResData* data)
{
    DbgAssert(data);
#if defined(TM_APPLE)
    *data = 0;
#elif defined(TM_GTK)
    data->p = 0;
#else
    data->m = 0;
    data->p = 0;
#endif
}

TM_INLINE TMBool ResValid(const TMResData* data)
{
#ifdef TM_APPLE
    return *data != 0;
#elif defined(TM_GTK)
    return data->p != 0;
#else
    return data->m != 0;
#endif
}

TM_INLINE TMBool ResLoad(TMHModule hMod, TMCStr pResName, TMCStr pTypeName, TMResData* data)
{
#ifdef TM_WIN_DESKTOP
    HGLOBAL hGbl; HRSRC hRes;
    DbgAssert(data);

    hRes = FindResource(hMod, pResName, pTypeName);
    if (hRes)
    {
        hGbl = LoadResource(hMod, hRes);
        if (hGbl)
        {
            data->p = LockResource(hGbl);
            data->m = SizeofResource(hMod, hRes);
            return true;
        }
    }
    return false;

#elif TM_WIN_METRO

    DbgAssert(0);
    return false;

#elif defined TM_APPLE
    CFStringRef name = NULL;
    CFURLRef url = NULL;
    CGDataProviderRef provider = NULL;
    TMBool ret = false;
    DbgAssert(data);

    name = CFStringCreateWithCStringNoCopy(NULL, pResName, kCFStringEncodingUTF8, kCFAllocatorNull);
    url = CFBundleCopyResourceURL(hMod, name, NULL, NULL);
    if (url)
    {
        provider = CGDataProviderCreateWithURL(url);
        *data = CGDataProviderCopyData(provider);
        CGDataProviderRelease(provider);
        CFRelease(url);
        ret = true;
    }
    CFRelease(name);
    return ret;

#elif defined(TM_GTK)
    DbgAssert(data);
    // pResName£º "/org/example/myres.bin"
    data->p = g_resources_lookup_data(pResName, G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
        return 0 != data->p;
#else
    return false;
#endif
}

TM_INLINE void ResFree(TMResData* data)
{
    DbgAssert(data);
#ifdef TM_APPLE
    if (!*data)
        return;
    CFRelease(*data);
    *data = 0;
#elif defined(TM_GTK)
    if (!data->p)
        return;
    g_bytes_unref(data->p);
    data->p = 0;
#endif
}

TM_INLINE size_t ResSize(const TMResData* data)
{
#ifdef TM_APPLE
    return CFDataGetLength(*data);
#elif defined(TM_GTK)
    return g_bytes_get_size(data->p);
#else
    return data->m;
#endif
}

TM_INLINE const TMUInt8* ResData(const TMResData* data)
{
#ifdef TM_APPLE
    return CFDataGetBytePtr(*data);
#elif defined(TM_GTK)
    return (const TMUInt8*)g_bytes_get_data(data->p, NULL);
#else
    return (const TMUInt8*)data->p;
#endif
}


/** @} */ // @addtogroup DLL


#ifdef TM_CPP

TM_BGN_NAMESPACE


/**
 *  @class CDll
 *  @brief packaged dll function
 *  @ingroup DLL
 */
template <bool t_bManaged>
class CDllT
{
protected:
    TMHModule m_hModule;
    TM_NO_COPYABLE (CDllT);

public:
    /** */ CDllT() : m_hModule (0) {}

    /** */
    CDllT(TMCStr pDllName, TMUInt32 dwFlag)
        : m_hModule (DllLoad(pDllName, dwFlag))
    {
    }

    /** */
    ~CDllT()
    {
        if (m_hModule && t_bManaged)
            DllFree(m_hModule);
    }

    /** */ operator TMHModule() const { return m_hModule; }

    /** */
//     void Swap (CDllT& rhs)
//     {
//         tm_swap (m_hModule, rhs.m_hModule);
//     }

    /** */
    TMHModule LoadLibrary(TMCStr pDllName, TMUInt32 dwFlag = 0)
    {
        DbgAssert(!m_hModule);
        DbgAssert(pDllName);
        return m_hModule = DllLoad(pDllName, dwFlag);
    }

    TMHModule LoadLibraryFrom(TMCStr pDllName, TMHModule hDir)
    {
        return m_hModule = DllLoadFrom(pDllName, hDir);
    }

    TMHModule LoadLibraryFromIf(TMCStr pDllName, TMHModule hDir)
    {
        this->FindLibrary(pDllName);
        if (!*this)
            LoadLibraryFrom(pDllName, hDir);
        return m_hModule;
    }

    TMHModule FindLibrary(TMCStr pDllName)
    {
        DbgAssert(!m_hModule);
        DbgAssert(pDllName);
        return m_hModule = DllHandle(pDllName);
    }

    /** */
    void FreeLibrary()
    {
        if (m_hModule)
        {
            DllFree(m_hModule);
            m_hModule = 0;
        }
    }

    /** */
    void Attach(TMHModule hModule)
    {
        DbgAssert(!m_hModule);
        m_hModule = hModule;
    }

    /** */
    TMHModule Detach()
    {
        TMHModule hModule = m_hModule;
        m_hModule = 0;
        return hModule;
    }

    /** */
    void* GetProcAddress(TMCStrA lpProcName)
    {
        DbgAssert(m_hModule);
        return DllSym(m_hModule, lpProcName);
    }
};

typedef CDllT<true>      CDll;
typedef CDllT<false>     CDllHandle;



///////////////////////////////////////////////////////////////////////////////
// sample:
//  struct CDllShcore { DECLARE_DLL_LOAD("shcore.dll", LOAD_LIBRARY_SEARCH_SYSTEM32); }
//  struct CDllUser32 { DECLARE_DLL_GET("user32.dll"; }
#define DECLARE_DLL_LOAD(DllName,dwFlag) \
    static TMHModule Load() \
    { \
        TM_STATIC_INIT(TMHModule, s_hModule, (TMHModule)-1, (DllLoad(TM_T(DllName), dwFlag))); \
        return s_hModule; \
    }

#define DECLARE_DLL_LOAD_FROM(DllName,hInst) \
    static TMHModule Load() \
    { \
        TM_STATIC_INIT(TMHModule, s_hModule, (TMHModule)-1, (DllLoadFrom(TM_T(DllName), hInst))); \
        return s_hModule; \
    }

#define DECLARE_DLL_GET(DllName) \
    static TMHModule Load() \
    { \
        TM_STATIC_INIT(TMHModule, s_hModule, (TMHModule)-1, (DllHandle(TM_T(DllName)))); \
        return s_hModule; \
    }
    

///////////////////////////////////////////////////////////////////////////////
// DECLARE_DLL_FUNC, DECLARE_DLL_FUNC_INST
// sample:
//  DECLARE_DLL_FUNC(dll_user32, HDC, WINAPI, GetDC, (HWND hWnd))
//  if (_GetDC) HDC hdc = _GetDC(hwnd);
#ifndef DFTCALL
#   define DFTCALL
#endif

//if no CallMode,Please use DFTCALL
#define DECLARE_DLL_FUNC(DllClassName,ReturnType,CallMode,FuncName,ParamList) \
    typedef ReturnType (CallMode * PFN##FuncName) ParamList; \
    static PFN##FuncName _##FuncName = (PFN##FuncName)_DllSymSafe(DllClassName::Load(), #FuncName)

#define DECLARE_DLL_FUNC3(DllClassName,ReturnType,CallMode,FuncName,ParamList) \
    struct CImport_##FuncName \
    { \
        typedef ReturnType (TM_IF_WIN(CallMode) * PFN) ParamList; \
        inline static PFN get() \
        { \
            TM_STATIC_INIT(PFN, s_f, (PFN)-1, ((PFN)_DllSymSafe(DllClassName::Load(), #FuncName))); \
            return s_f; \
        } \
    }; \
    typedef CImport_##FuncName::PFN PFN##FuncName; \

#define TM_IMPORT(FuncName) (CImport_##FuncName::get())


// #define DECLARE_DLL_FUNC_INST(DllClassName,ReturnType,CallMode,FuncName,ParamList) \
//     typedef ReturnType (TM_IF_WIN(CallMode) * PFN##FuncName) ParamList; \
//     template <class T> struct CF_##FuncName { static PFN##FuncName _##FuncName; }; \
//     template <class T> PFN##FuncName CF_##FuncName<T>::_##FuncName = (PFN##FuncName)_DllSymSafe(DllClassName::Load(), #FuncName); \
//     inline PFN##FuncName Inst_##FuncName() { return CF_##FuncName<void>::_##FuncName; }

// #define DECLARE_DLL_FUNC_INST(DllClassName,ReturnType,CallMode,FuncName,ParamList) \
//     typedef ReturnType (TM_IF_WIN(CallMode) * PFN##FuncName) ParamList; \
//     static PFN##FuncName _##FuncName = (PFN##FuncName)_DllSymSafe(DllClassName::Load(), #FuncName); \
//     inline PFN##FuncName Inst_##FuncName() { return _##FuncName; }


class CResource
{
private:
    TMResData m_data;

public:
    CResource()
    {
        ResInit(&m_data);
    }

    ~CResource()
    {
        ResFree(&m_data);
    }

    operator TMBool() const { return ResValid(&m_data); }

    TMBool Load(TMHModule hMod, TMCStr pResName, TMCStr pTypeName)
    {
        DbgAssert(!*this);
        return ResLoad(hMod, pResName, pTypeName, &m_data);
    }

    const TMUInt8* GetData() const
    {
        DbgAssert(*this);
        return ResData(&m_data);
    }

    size_t GetSize() const
    {
        DbgAssert(*this);
        return ResSize(&m_data);
    }

    void Free()
    {
        DbgAssert(*this);
        ResFree(&m_data);
        ResInit(&m_data);
    }
};

TM_END_NAMESPACE


#ifdef TM_WIN
    struct dll_null     {};
    struct dll_user32   { DECLARE_DLL_GET  ("user32.dll"); };
    struct dll_comctl32 { DECLARE_DLL_LOAD ("comctl32.dll", LOAD_LIBRARY_SEARCH_SYSTEM32); };
    struct dll_gdi32    { DECLARE_DLL_GET  ("gdi32.dll"); };
    struct dll_ntdll    { DECLARE_DLL_GET  ("ntdll.dll"); };
    struct dll_kernel32 { DECLARE_DLL_GET  ("kernel32.dll"); };
    struct dll_psapi    { DECLARE_DLL_LOAD ("psapi.dll", LOAD_LIBRARY_SEARCH_SYSTEM32); };
    struct dll_dwmapi   { DECLARE_DLL_LOAD ("dwmapi.dll", LOAD_LIBRARY_SEARCH_SYSTEM32); };
    struct dll_shcore   { DECLARE_DLL_LOAD ("shcore.dll", LOAD_LIBRARY_SEARCH_SYSTEM32); };
    struct dll_dbghelp  { DECLARE_DLL_LOAD ("dbghelp.dll", 0); };
    struct dll_Uiautomationcore   { DECLARE_DLL_LOAD("Uiautomationcore.dll", LOAD_LIBRARY_SEARCH_SYSTEM32); };
#endif


#endif // #ifdef TM_CPP

#endif // end of TMDLL_H
