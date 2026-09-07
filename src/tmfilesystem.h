/**
 *  @file
 *  @brief  file system
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.12.8
 *
 *  Copyright (C) 2009 miragekiller
 */

#pragma once

#ifndef TMFILESYSTEM_H
#define TMFILESYSTEM_H

#include "tmio.h"
#include <sys/stat.h>

#ifdef TM_CPP
#   include <string>
#endif

#ifdef TM_WIN

#   include <windows.h>
#   include <tchar.h>

typedef struct TMDir
{
    HANDLE hd;
    WIN32_FIND_DATA fd;
} TMDir;

#else

#   include <dirent.h>   // opendir, readdir, closedir

#   if defined(TM_GTK)

#   include <glib/gstdio.h>

#   define __chdir      g_chdir
#   define __unlink     g_unlink
#   define __mkdir      g_mkdir
#   define __rmdir      g_rmdir
#   define __stat       g_stat
#   define __chmod      g_chmod
#   define __stat_buf   GStatBuf

static inline char* __getcwd(char* buf, size_t size)
{
    if (!buf || size == 0)
        return NULL;

    gchar* cwd = g_get_current_dir();
    if (cwd) {
        g_strlcpy(buf, cwd, size);
        g_free(cwd);
        return buf;
    }
    return NULL;
}

static inline char* __realpath(const char* path, char* resolved_path)
{
    if (!path || !resolved_path)
        return NULL;

    gchar* absolute_path;

    if (g_path_is_absolute(path)) {
        absolute_path = g_strdup(path);
    }
    else {
        gchar* cwd = g_get_current_dir();
        absolute_path = g_build_filename(cwd, path, NULL);
        g_free(cwd);
    }

    gchar* canonical = g_canonicalize_filename(absolute_path, NULL);

    g_strlcpy(resolved_path, canonical, PATH_MAX);

    g_free(canonical);
    g_free(absolute_path);

    return resolved_path;
}

#else

#   include <unistd.h>   // chdir, getcwd, unlink, rmdir

#   define __Dir        DIR
#   define __chdir      chdir
#   define __getcwd     getcwd
#   define __unlink     unlink
#   define __mkdir      mkdir
#   define __rmdir      rmdir
#   define __stat       stat
#   define __chmod      chmod
#   define __realpath   realpath
#   define __stat_buf   struct stat

#endif // TM_GTK

#ifdef TM_APPLE
#   include <copyfile.h>
#else

int copyfile(const char *from, const char *to, int, int flags);
#   define COPYFILE_EXCL 1
#   define _copyfile_DEFINED

#endif // TM_APPLE

typedef struct TMDir
{
    DIR*            dirp;
    struct dirent*  dire;
} TMDir;

#endif // TM_WIN

TM_BGN_EXTERN_C

/**
 *  @defgroup filesystem
 */


/**
 *  @addtogroup filesystem
 *  filesystem "c" function prototype
 *  @{
 */

/** creates a new directory. */
TM_INLINE TMBool FsMkDir(TMCStr p)
{
    TM_IF_WIN (return CreateDirectory(p, 0));
    TM_NO_WIN (return 0 == __mkdir(p, 0777));
}

/** create a new sub directory like aaa/bbb/ccc. */
// aaa/bbb/ccc:     create dir: current dir + "aaa/bbb/";
// aaa/bbb/ccc/:    create dir: current dir + "aaa/bbb/ccc/";
// c:/aaa/bbb:      create dir: "c:/aaa/";
TM_CAPI void FsMkDirs(TMCStr p);

/** Delete a directory */
TM_INLINE TMBool FsRmDir (TMCStr p)
{
    TM_IF_WIN(return RemoveDirectory(p));
    TM_NO_WIN(return 0 == __rmdir(p));
}

/** Delete a directory and all subdir & file. */
TM_CAPI void FsRmDirs(TMCStr p);

/** Change the current working directory. */
TM_INLINE TMBool FsChDir(TMCStr p)
{
#if TM_WIN_DESKTOP
    return SetCurrentDirectory(p);
#elif TM_WIN_METRO
    DbgAssert(0); return false;
#else
    return 0 == __chdir(p);
#endif
}

/**
 * get current directory for the current process.
 * @remark pBuf must not be 0, buffer size must >= PATH_MAX
 */
TM_INLINE TMBool FsCwd(TMStr pBuf)
{
    DbgAssert(pBuf);
#if TM_WIN_DESKTOP
    return GetCurrentDirectory(PATH_MAX, pBuf);
#elif TM_WIN_METRO
    DbgAssert(0); return false;
#else
    return 0 != __getcwd(pBuf, PATH_MAX);
#endif
}

/**
 * get the path of the directory designated for temporary files.
 * @remark pBuf must not be 0, buffer size must >= PATH_MAX
 */
TM_CAPI size_t FsTmpPath(TMStr pBuf, size_t szBuffer);


/**
 * copies an existing file to a new file
 * @param[in] bOver If this parameter is TRUE and the new file already exists,
 *  the function overwrites the existing file and succeeds.
 */
TM_INLINE TMBool FsCopy(TMCStr pFrom, TMCStr pTo, TMBool bOver)
{
    DbgAssert(pFrom);
    DbgAssert(pTo);

#if TM_WIN_DESKTOP
    TM_IF_WIN (return CopyFile(pFrom, pTo, !(bOver)));
#elif TM_WIN_METRO
    COPYFILE2_EXTENDED_PARAMETERS  ceParameters;
    ceParameters.dwSize = sizeof(COPYFILE2_EXTENDED_PARAMETERS);
    ceParameters.dwCopyFlags = COPY_FILE_ALLOW_DECRYPTED_DESTINATION;
    ceParameters.pfCancel = FALSE;
    ceParameters.pProgressRoutine = NULL;
    ceParameters.pvCallbackContext = NULL;
    HRESULT lRet = CopyFile2(pFrom,pTo, &ceParameters);
    return lRet == S_OK ? TRUE :FALSE;
#else
    return 0 == copyfile(pFrom, pTo, 0, (bOver) ? 0 : COPYFILE_EXCL);
#endif //
}


/**
 * Rename a file or directory.
 * @remark The new name must not be the name of an existing file or directory.
 * in unix pFrom will replace pTo if pTo exists.
 */
TM_INLINE TMBool FsMove(TMCStr pFrom, TMCStr pTo)
{
    DbgAssert(pFrom);
    DbgAssert(pTo);

#if TM_WIN_DESKTOP
    return MoveFile(pFrom, pTo);
#elif TM_WIN_METRO
    return MoveFileEx(pFrom,pTo, MOVEFILE_COPY_ALLOWED);
#else
    return 0 == rename(pFrom, pTo);
#endif // TM_WIN_DESKTOP
}
/** deletes an existing file. */
TM_INLINE TMBool FsRemove(TMCStr pName)
{
    DbgAssert(pName);
    TM_IF_WIN (return DeleteFile(pName));
    TM_NO_WIN (return 0 == __unlink(pName));
}

/** get file size. */
TM_INLINE TMUInt64 FsSize(TMCStr pName)
{
#if TM_WIN_DESKTOP
    WIN32_FIND_DATA fd;
    HANDLE hd = FindFirstFile(pName, &fd);
    if (INVALID_HANDLE_VALUE != hd)
    {
        FindClose(hd);
        return (((TMUInt64)(fd.nFileSizeHigh)) << 32) + (TMUInt64)(fd.nFileSizeLow);
    }
    return 0;
#elif TM_WIN_METRO
    struct _stat buf;
    return (0 == _tstat(pName, &buf)) ? buf.st_size : 0;
#else
    __stat_buf buf;
    return (0 == __stat(pName, &buf)) ? buf.st_size : 0;
#endif
}

/**
 * get file mode
 * @return
    - S_IFMT mask bit
    - S_IFDIR directory
    - S_IFCHR character special
    - S_IFREG normal file
    - S_IFBLK block special
    - S_IFFIFO fifo
    - S_IFSOCK socket
 * @par sample
 * @code
    // check the filename is dir:
    TMUInt uMode = FsMode(c:\\aaa);
    TMBool bDir = ((uMode & S_IFMT) == S_IFDIR);
 * @endcode
 */
TM_INLINE TMUInt FsMode(TMCStr pName)
{
    struct TM_SW_WIN(_stat, stat) buf;
    DbgAssert(pName);
    return (0 == TM_SW_WIN(_tstat, stat)(pName, &buf)) ? buf.st_mode : 0;
}

/* set file attribute */
TM_INLINE TMUInt FsSetAttr(TMCStr file, TMUInt uAccessMode)
{
#ifdef TM_WIN
    return SetFileAttributes(file, uAccessMode);
#else
    return __chmod(file, uAccessMode);
#endif
}

TM_INLINE TMBool _tm_is_f_or_d(TMUInt type)
{
    type &= S_IFMT;
    return type == S_IFDIR || type == S_IFREG;
}

TM_INLINE TMBool FsExist(TMCStr pName)
{
#if TM_WIN_METRO
    WIN32_FILE_ATTRIBUTE_DATA  fileinfo = {0};
    GetFileAttributesEx(pName,GetFileExInfoStandard,&fileinfo);
    return 0xffffffff != fileinfo.dwFileAttributes;
#elif TM_WIN_DESKTOP
    return 0xffffffff != GetFileAttributes(pName);
#else
    return _tm_is_f_or_d(FsMode(pName));
#endif
}

/** get the full path and file name of a specified file. */
TM_INLINE TMBool FsFullPath(TMCStr pName, TMStr pBuf)
{
    DbgAssert(pName);
    DbgAssert(pBuf);
#if TM_WIN_DESKTOP
    return GetFullPathName(pName, PATH_MAX, pBuf, 0);
#elif TM_WIN_METRO
    DbgAssert(0); return false;
#else
    return 0 != __realpath(pName, pBuf);
#endif
}

/** init a TMDir struct */
TM_INLINE void FsFindInit(struct TMDir* pDir)
{
    DbgAssert(pDir);
    TM_IF_WIN (pDir->hd = INVALID_HANDLE_VALUE; pDir->fd.cFileName[0] = 0);
    TM_NO_WIN (pDir->dirp = 0; pDir->dire = 0);
}

/** check is valid or end of find */
TM_INLINE TMBool FsFindEOF(const struct TMDir* pDir)
{
    DbgAssert(pDir);
    TM_IF_WIN (return !pDir->fd.cFileName[0]);
    TM_NO_WIN (return pDir->dire == 0);
}

/**
 * Provides information about the first instance of a filename in specified directory.
 * @par sample
 * @code
    // enum file in directory:
    TMDir dir;
    FsFindFirst (&dir, "c:\\*.*"); // in windows, don't use "c:\" or "c:\."
    for (; !FsFindEOF(&dir); FsFindNext(&dir))
    {
        if (FsFindIsDir(&dir))
        {
            if (!FsFindIsDots(&dir))
            {
                // do somthing for dir
            }
        }
        else
        {
            // do somthing for file
        }
    }
 * @endcode
 */
TM_CAPI TMBool FsFindFirst(struct TMDir* pDir, TMCStr pDirName);

/** Find the next file or dir name */
TM_INLINE TMBool FsFindNext(struct TMDir* pDir)
{
    DbgAssert(pDir);

#ifdef TM_WIN
    if (FindNextFile(pDir->hd, &pDir->fd))
        return TMTrue;
    pDir->fd.cFileName[0] = 0;
    return TMFalse;
#else
    pDir->dire = readdir(pDir->dirp);
    return 0 != pDir->dire;
#endif
}

/** Closes the specified search handle and releases associated resources. */
TM_INLINE void FsFindClose(struct TMDir* pDir)
{
    DbgAssert(pDir);
    TM_IF_WIN(FindClose(pDir->hd));
    TM_NO_WIN(if (pDir->dirp) closedir(pDir->dirp));
    FsFindInit(pDir);
}

/**
 * get file or dir name from struct TMDir.
 * returned name exclude path.
 */
TM_INLINE TMCStr FsFindGetName(const struct TMDir* pDir)
{
    DbgAssert(pDir);
    TM_IF_WIN(return pDir->fd.cFileName);
    TM_NO_WIN(return pDir->dire->d_name);
}

/** get file size from struct TMDir*/
TM_INLINE TMUInt64 FsFindGetSize(const struct TMDir* pDir)
{
    DbgAssert(pDir);
#ifdef TM_WIN
    return (((TMUInt64)(pDir->fd.nFileSizeHigh)) << 32) + (TMUInt64)(pDir->fd.nFileSizeLow);
#else
    return FsSize(FsFindGetName(pDir));
#endif
}

/** get is directory from struct TMDir*/
TM_INLINE TMBool FsFindIsDir(const struct TMDir* pDir)
{
    DbgAssert(pDir);
#if TM_WIN
    return pDir->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
#elif defined(DT_DIR)
    return pDir->dire->d_type == DT_DIR;
#else
    TMCStr p = FsFindGetName(pDir);
    return ((FsMode(p) & S_IFMT) == S_IFDIR);
#endif
}

/** get is dot (".") from struct TMDir*/
TM_INLINE TMBool FsFindIsDot(const struct TMDir* pDir)
{
    TMCStr p = FsFindGetName(pDir);
    return (p[0] == '.') && (p[1] == 0);
}

/** get is dotdot ("..") from struct TMDir*/
TM_INLINE TMBool FsFindIsDotDot(const struct TMDir* pDir)
{
    TMCStr p = FsFindGetName(pDir);
    return (p[0] == '.') && (p[1] == '.') && (p[2] == 0);
}

/** get is dot or dotdot (".." or ".") from struct TMDir*/
TM_INLINE TMBool FsFindIsDots(const struct TMDir* pDir)
{
    TMCStr p = FsFindGetName(pDir);
    return (p[0] == '.') && ((p[1] == 0) || ((p[1] == '.') && (p[2] == 0)));
}


typedef void (*TMFSearch)(const struct TMDir* pDir, int nlayer, TMBool* bStop, void* avg);

/** search directory, enum all sub file and directory and callback to pfn*/
TM_CAPI void FsSearch(TMCStr pDir, TMFSearch pfn, void* avg);


/** @} */ // @addtogroup filesystem

TM_END_EXTERN_C


#ifdef TM_CPP


TM_BGN_NAMESPACE

/**
 *  @class CFindFile
 *  @brief packaged file_system function
 *  @ingroup filesystem
 *  @par sample
 *  @code
    for (CFindFile ff("c:\\*.*"); ff; ff.FindNext())
    {
        if (ff.IsDirectory())
        {
            if (!ff.IsDots())
            {
                // do somthing for dir
            }
        }
        else
        {
            // do somthing for file
        }
    }
 *  @endcode
 */

class CFindFile
{
public:
    TMDir m_dir;
    TM_NO_COPYABLE(CFindFile);

public:
    /** */CFindFile() { FsFindInit(&m_dir); }

    /** */CFindFile(TMCStr pFile) { FsFindFirst(&m_dir, pFile); }

    /** */~CFindFile() { FsFindClose(&m_dir); }

    /** */operator TMBool() const { return !FsFindEOF(&m_dir); }

    /** */TMBool operator !() const { return FsFindEOF(&m_dir); }

    /** */TMBool FindFirst(TMCStr pFile) { return FsFindFirst(&m_dir, pFile); }

    /** */TMBool FindNext() { return FsFindNext(&m_dir); }

//     BOOL IsReadOnly() const { return MatchesMask(FILE_ATTRIBUTE_READONLY); }

    /** */TMBool IsDirectory() const { return FsFindIsDir(&m_dir); }

//     BOOL IsCompressed() const { return MatchesMask(FILE_ATTRIBUTE_COMPRESSED); }
//
//     BOOL IsSystem() const { return MatchesMask(FILE_ATTRIBUTE_SYSTEM); }
//
//     BOOL IsHidden() const { return MatchesMask(FILE_ATTRIBUTE_HIDDEN); }
//
//     BOOL IsTemporary() const { return MatchesMask(FILE_ATTRIBUTE_TEMPORARY); }
//
//     BOOL IsNormal() const { return MatchesMask(FILE_ATTRIBUTE_NORMAL); }
//
//     BOOL IsArchived() const { return MatchesMask(FILE_ATTRIBUTE_ARCHIVE); }

    TMBool MatchesMask(unsigned long dwMask) const
    {
        TM_IF_WIN(return m_dir.fd.dwFileAttributes & dwMask);
        TM_NO_WIN(return true);
    }

//     FILETIME GetLastAccessTime () const
//     {
//         return m_fd.ftLastAccessTime;
//     }
//
//     FILETIME GetLastWriteTime () const
//     {
//         return m_fd.ftLastWriteTime;
//     }
//
//     FILETIME GetCreationTime () const
//     {
//         return m_fd.ftCreationTime;
//     }

    /** */TMCStr GetFileName() const { return FsFindGetName(&m_dir); }

    /** */TMUInt64 GetLength() const { return FsFindGetSize(&m_dir); }

    /** */TMBool IsDot() const { return FsFindIsDot(&m_dir); }

    /** */TMBool IsDotDot() const { return FsFindIsDotDot(&m_dir); }

    /** */TMBool IsDots() const { return FsFindIsDots(&m_dir); }
};


////////////////////////////////////////////////////////////////////////////////
// class CSearchDir
// can search Multi-layer sub-directories
// wildcard is the 2nd argument,so wildcard can not be included in pDir argument,pay attention to the difference between CFindFile
class CSearchDir
{
private:
    static void _Search(const struct TMDir* pDir, int nlayer, TMBool* bStop, void* avg)
    {
        CSearchDir* p = (CSearchDir*)avg;
        p->OnSearch(*(const CFindFile*)pDir, nlayer, *bStop);
    }

public:
    void Search(TMCStr pDir)
    {
        FsSearch(pDir, _Search, this);
    }

    virtual void OnSearch(const CFindFile& ff, int nlayer, TMBool& bStop) = 0;
};


TM_END_NAMESPACE

#endif // TM_CPP


#endif // end of #ifndef TMFILESYSTEM_H
