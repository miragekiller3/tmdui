/**
 *  @file
 *  @brief  filename
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2002.07.23
 *
 *  Copyright (C) 2002 miragekiller
 */

#pragma once

#ifndef TMFILENAME_H
#define TMFILENAME_H

#include "tmdef.h"
#include "tmio.h"
#include "tmstring.h"
#include "tmdll.h"
#include "tmfilesystem.h"


#ifdef TM_WIN
#   include <Shlwapi.h>
#   pragma comment(lib, "Shlwapi")
#endif

/**
 *  @defgroup filesystem
 *
 *  @defgroup filename
 *  @ingroup filesystem
 *  technical specification(sample):<br>
 *  c:/window/info/KB932716-v2.log
 *  - path: c:/window/info
 *  - body: KB932716-v2
 *  - ext:  log
 *  - name: KB932716-v2.log
 */


TM_BGN_NAMESPACE


/**
 *  @class CFnViewT
 *  @ingroup filename
 *
 *  @brief
 *  use 2 const_iterator point to a memory stored a filename string;
 *  the string must not cend by 0;
 */


template <class T>
//class CFnViewT : public std::_tm_string_view<T> // CStringT<T, cstring_traits_mode_weak > // CStrViewT<T>
class CFnViewT : public CStringT<T, _cstring_mode_view<T> > // CStrViewT<T>
{
public:
//    typedef CStrViewT<T>                            base_class;
//     typedef CStringT<T, cstring_traits_mode_weak >  base_class;
//     typedef CStrPtrBaseBaseT<T>                     sub_class;
//    typedef std::_tm_string_view<T>                 base_class;
    typedef CStringT<T, _cstring_mode_view<T> >     base_class;
    typedef base_class                              sub_class;
    typedef typename base_class::const_iterator     const_iterator;
    typedef typename base_class::value_type         value_type;
//     typedef typename base_class::std_string         std_string;

    using base_class::cbegin;
    using base_class::cend;
    using base_class::__make_view;

public:
    /** constructor. */
    CFnViewT() {}

    CFnViewT(const base_class& rhs)
        : base_class(rhs)
    {
    }

    /**
     *  @param[in] bKeepDot
     *  - false "c:\a\b.ext"-> "ext"
     *  - true  "c:\a\b.ext"-> ".ext"
     */
    CFnViewT GetExt(TMBool bKeepDot) const
    {
        for (const_iterator iter = cend(); iter != cbegin();)
        {
            value_type v = *--iter;
            if (v == '.')
                return __make_view(iter + !bKeepDot, cend());
            if (v == '\\' || v == '/')
                break;
        }

        return CFnViewT();
    }

    /** "c:\a\b.ext"->"b.ext" */
    CFnViewT GetName() const
    {
        for (size_t n = this->length(); n > 0;)
        {
            value_type v = this->data()[--n];
            if (v == '\\' || v == '/')
                return this->Mid(n + 1);
        }
        return *this;
    }

    /**
     *  @param[in] bKeepSlash
     *  - false "c:\a\b.ext"->"c:\a"
     *  - true  "c:\a\b.ext"->"c:\a\"
     *  "a.ext"             -> ""
     */
    CFnViewT GetPath(TMBool bKeepSlash) const
    {
        for (const_iterator iter = cend(); iter != cbegin();)
        {
            value_type v = *--iter;
            if (v == '\\' || v == '/')
                return __make_view(cbegin(), iter + (false != bKeepSlash));
        }
        return CFnViewT();
    }

    /**
     *  @param[in] bKeepSlash
     *  - false "c:\a\b.ext"->"c:"
     *  - true  "c:\a\b.ext"->"c:\"
     *  "a.ext"             -> ""
     */
    CFnViewT GetDrive(TMBool bKeepSlash) const
    {
        if (cbegin()[1] != ':')
            return CFnViewT();
        return CFnViewT(cbegin(), bKeepSlash ? 3 : 2);
    }

    /**
     *  @param[in] bKeepDot
     *  - false "c:\a\b.ext"-> "c:\a\b"
     *  - true  "c:\a\b.ext"-> "c:\a\b."
     */
    CFnViewT GetPathBody(TMBool bKeepDot) const
    {
        CFnViewT ext(GetExt(!bKeepDot));
        if (ext)
            return __make_view(this->data(), ext.data());
        return *this;
    }

    /** "c:\a\b.ext"->"b" */
    CFnViewT GetBody() const
    {
        const_iterator itend = cend();
        tm_for (const_iterator iter = itend; iter != cbegin();)
        {
            value_type v = *--iter;

            if (v == '.')
            {
                itend = iter;
                break;
            }

            if (v == '\\' || v == '/')
                return __make_view(iter + 1, itend);
        }

        tm_for (const_iterator iter = itend; iter != cbegin();)
        {
            value_type v = *--iter;

            if (v == '\\' || v == '/')
                return __make_view(iter + 1, itend);
        }

        return __make_view(cbegin(), itend);
    }

    TMBool IsRelativePath() const
    {
        return this->data()[0] != '\\'
            && this->data()[0] != '/'
            && (this->length() < 3 || this->data()[1] != ':');
    }
}; // CFnViewT


typedef CFnViewT<TMCharA>         CFnViewA;
typedef CFnViewT<TMCharW>         CFnViewW;
typedef CFnViewT<TMCharT>         CFnView;



/**
 *  @class CFileName
 *  @ingroup filename
 *
 *  @see CFnViewT
 */
class CFileName : public CString
{
public:
    typedef CString                                 base_class;
    typedef base_class                              cont_type;
    typedef cont_type::base_class                   std_string;
    typedef CFnView                                 fn_view;
    typedef base_class::view_type                   view_type;

    using base_class::value_type;
    using base_class::pointer;
    using base_class::const_pointer;
    using base_class::c_str;

public:
    explicit CFileName() {}

    template < class _Tp >
    explicit CFileName (const _Tp& name)
        : base_class (name)
    {
    }

    template < class U >
    CFileName& operator = (const U& __val)
    {
        base_class::operator = (__val);
        return *this;
    }

    /**
     *  @param[in] bKeepDot
     *  - false "c:\a\b.ext"-> "ext"
     *  - true  "c:\a\b.ext"-> ".ext"
     */
    const_pointer GetExt(TMBool bKeepDot) const
    {
        return fn_view(*this).GetExt(bKeepDot).data();
    }

    /** "c:\a\b.ext"->"b.ext" */
    const_pointer GetName() const
    {
        return fn_view(*this).GetName().data();
    }

    /**
     *  @param[in] bKeepSlash
     *  - false "c:\a\b.ext"->"c:\a"
     *  - true  "c:\a\b.ext"->"c:\a\"
     */
    fn_view GetPath(TMBool bKeepSlash) const
    {
        return fn_view(*this).GetPath(bKeepSlash);
    }

    /**
     *  @param[in] bKeepDot
     *  - false "c:\a\b.ext"-> "c:\a\b"
     *  - true  "c:\a\b.ext"-> "c:\a\b."
     */
    fn_view GetPathBody(TMBool bKeepDot) const
    {
        return fn_view(*this).GetPathBody(bKeepDot);
    }

    /** "c:\a\b.ext"->"b" */
    fn_view GetBody() const
    {
        return fn_view(*this).GetBody();
    }

    void ReplaceFileName(TMCStr pfn)
    {
        fn_view fn = GetPath(true);
        this->replace(fn.size(), this->npos, pfn);
    }

    void ReplaceFileExt(view_type ext)
    {
        fn_view fn = GetPathBody(true);
        this->replace(fn.size(), this->npos, ext);
    }

    TMBool IsRelativePath() const
    {
        return fn_view(*this).IsRelativePath();
    }

    /** get curret temp path */
    void GetTempPath()
    {
        size_t n = FsTmpPath(GetBuffer(PATH_MAX+1), PATH_MAX);
        SetLength(n);
    }

#ifdef TM_WIN
    /**
     *  make a temp file name as "pDir \ lpPrefix + uUnique + .tmp"
     *  @param[in] lpPrefix
     *      first three characters as the prefix of the file name
     *  @param[in] pDir
     *      directory path that create file, 0 auto get system temp path
     *  @remarks
     *      this func will test the tmp file is exists
     */
    void MakeTemp(TMCStr lpPrefix, TMCStr pDir = 0)
    {
        if (!pDir)
        {
            TMCharT c[PATH_MAX];
            ::GetTempPath(PATH_MAX, c);
            ::GetTempFileName(c, lpPrefix, 0, GetBuffer(PATH_MAX + 1));
        }
        else
        {
            ::GetTempFileName(0, lpPrefix, 0, GetBuffer(PATH_MAX + 1));
        }

        ReleaseBuffer();
    }

    /**
     *  @remarks the return path not include Slash
     */
    void GetSystemDirectory()
    {
        DWORD n = ::GetSystemDirectory(GetBuffer(PATH_MAX + 1), PATH_MAX);
        SetLength(n);
    }

    /** @remarks the return path not include Slash */
    void GetWindowsDirectory()
    {
        DWORD n = ::GetWindowsDirectory(GetBuffer(PATH_MAX + 1), PATH_MAX);
        SetLength(n);
    }
#endif // TM_WIN

    /** @remarks the return path not include Slash */
    void GetCurrentDirectory()
    {
        int n = FsCwd(GetBuffer(PATH_MAX));
        TM_IF_WIN(SetLength(n));
        TM_NO_WIN(if (n) ReleaseBuffer(); else Empty());
    }

    /** set curret dir */
    TMBool SetCurrentDirectory() const
    {
        return FsChDir(c_str());
    }

    /**
     *  retrieves the full path and file name for specified module
     *  @param[in] hModule
     *      0 returns the path for current process
     *  @remarks this func will test the tmp file is exists
     */
    void GetModuleFileName(TMHModule hModule = 0)
    {
        DllName(hModule, GetBuffer(PATH_MAX + 1));
        ReleaseBuffer();
    }

    /**
     *  retrieves the full path for specified module
     *  @see GetModuleFileName
     */
    void GetModuleFilePath(TMHModule hModule = 0, TMBool bKeepSlash = false)
    {
        GetModuleFileName (hModule);
        SetLength(GetPath(bKeepSlash).length());
    }

    /**
     *  retrieves the full module path + pNewFileNme
     *  @see GetModuleFileName GetModuleFilePath
     */
    void GetSiblingModuleFileName(TMCStr pNewName, TMHModule hModule = 0)
    {
        GetModuleFileName(hModule);
        SetLength(GetPath(true).length());
        append(pNewName);
    }

    void GetFullPathName(TMCStr pName)
    {
#ifdef TM_WIN_DESKTOP
        DWORD n = ::GetFullPathName(pName, PATH_MAX, GetBuffer(PATH_MAX + 1), 0);
        SetLength(n);
#elif defined(TM_APPLE)
        if (pName)
        {
            CFURLRef url = CFURLCreateAbsoluteURLWithBytes(0, (const UInt8*)pName, StrLen(pName), kCFStringEncodingUTF8, 0, false);
            CFURLGetFileSystemRepresentation(url, true, (UInt8*)GetBuffer(PATH_MAX + 1), PATH_MAX);
            CFRelease(url);
            ReleaseBuffer();
        }
        else
            Empty();
#else
        FsFullPath(pName, GetBuffer(PATH_MAX + 1));
        ReleaseBuffer();
#endif
    }

    //  *this                   pName           output
    //  c:\a\x\y\file           path            c:\a\x\y\file\path
    //  c:\a\x\y\file\          path            c:\a\x\y\file\path
    //  c:\a\x\y\file\          \path           c:\path
    //  n/a                     \path           [current dir]:\path
    void MergeFullPathName(view_type path)
    {
        if (path[0] == '\\' || path[0] == '/')
        {
            if (GetLength() >= 2 && cbegin()[1] == ':')
                replace(2, npos, path.data(), path.length());
            else
                Assign(path);
        }
        else if (path[1] == ':')
            Assign(path);
        else
        {
            MakeSlash();
            Append(path);
        }

        CFileName fn;
        fn.swap(*this);
        GetFullPathName(fn);
    }

//     void GetProcessFileNameW(DWORD pid)
//     {
//         HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
//         wchar_t tsPath[MAX_PATH + 1] = { 0 };
//
//         DECLARE_DLL_FUNC("Kernel32.dll", BOOL, WINAPI, QueryFullProcessImageNameW,
//             (HANDLE hProcess, DWORD  dwFlags, LPSTR  lpExeName, PDWORD lpdwSize));
// #   define GetProcessImageFileNameW CDllFunc_QueryFullProcessImageNameW::LoadDirSystem32()
//         if (GetProcessImageFileNameW)
//             GetProcessImageFileNameW(hProc, 0, tsPath, MAX_PATH + 1);
//         else
//         {
//             DECLARE_DLL_FUNC("psapi.dll", DWORD, WINAPI, GetProcessImageFileNameW,
//                 (HANDLE hProcess, LPWSTR  lpImageFileName, DWORD  nSize));
// #       define GetProcessImageFileNameW CDllFunc_GetProcessImageFileNameW::LoadDirSystem32()
//             GetProcessImageFileNameW(hProc, tsPath, MAX_PATH + 1);
//
//
//     }


//  pFromPath: "c:\\a\\b\\path";
//  *this = "c:\\a\\x\\y\\file";        return: bGetChildDir ? false : ..\..\x\y\file;
//  *this = "c:\\a\\b\\path\\aa\\file"; return: "aa\\file";
    TMBool MakeRelativePath(TMCStr pFromPath, TMBool bGetChildDir = false)
    {
#ifdef TM_WIN
        TMCharT c[2048];
        if (PathRelativePathTo(c, pFromPath, FILE_ATTRIBUTE_DIRECTORY, this->c_str(), FILE_ATTRIBUTE_NORMAL))
        {
            if (c[0] == '\\')                       // xp return \\aa\\file
                Assign(&c[1]);
            else if (c[0] == '.' && c[1] == '\\')   // win10 return .\\aa\\file
                Assign(&c[2]);
            else if (bGetChildDir)
                return false;
            else
                Assign(&c[0]);
            return true;
        }
#else
        DbgAssert(0);
#endif // TM_WIN
        return false;
    }

    /** append '\\' or '/' on cend of path string if no slash */
    void MakeSlash()
    {
        int n = GetLength();
        if (n > 0)
        {
            value_type v = GetAt(n - 1);
            if (v != '\\' && v != '/')
                insert(cend(), TM_SW_WIN('\\', '/'));
        }
    }

    TMBool IsExists() const
    {
        return FsExist(c_str());
    }
};


/**
 *  @class CPathStore
 *  @ingroup filesystem
 *
 *  @brief save current path, then user can reset path,
 *  when exit curent func, it will auto restor saved path
 *
 *  @par sample:
 *  @code
    void func()
    {
        CPathStore ps; // now ps get current dir is "d:\\";
        SetCurrentDirectory ("c:\\");
    }
    // when exit the func, the path restore to "d:\\"
 *  @endcode
 *
 */
class CPathStore
{
private:
    CFileName m_path;

public:
    CPathStore ()
    {
        m_path.GetCurrentDirectory();
    }

    const CFileName& GetPath() const { return m_path; }

    ~CPathStore ()
    {
        m_path.SetCurrentDirectory ();
    }
};


TM_END_NAMESPACE

// #undef _HMODULE
//
//
// #ifdef TM_WIN_DESKTOP
//
// #   include "wtfilename.h"
//
// #endif // cend of #ifdef TM_APPLE


#endif // cend of #ifndef TMFILENAME_H
