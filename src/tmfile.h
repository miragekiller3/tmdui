/**
 *  @file
 *  @brief  File I/O, support Large File.
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2002.2.25
 *
 *  to Support Large File in gcc,
 *  you need compile programs with "gcc -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64".
 *
 *  Copyright (C) 2002 miragekiller
 */

#pragma once

#ifndef TMFILE_H
#define TMFILE_H

#include "tmdef.h"
#include <fcntl.h>
#include "tmio.h"
#include "tmendian.h"
#include "tmdebug.h"

#ifdef TM_QT
#   include <QFile>
#endif

#ifdef TM_WIN

#   include <windows.h>

#else

#   define _LARGEFILE_SOURCE
#   define _LARGEFILE64_SOURCE
#   define _FILE_OFFSET_BITS 64

#   include <fcntl.h>
#   include <sys/stat.h>
#   include <unistd.h>

#   ifdef O_LARGEFILE
#       define TM_O_LARGEFILE   O_LARGEFILE
#       define __ftruncate      ftruncate64
#   else
#       define TM_O_LARGEFILE   0
#       define __ftruncate      ftruncate
#   endif // O_LARGEFILE

#   ifdef TM_GTK
#       include <glib/gstdio.h>
#       define __open           g_open
#       define __close(x)       g_close(x,0)
#       define __fsync          g_fsync
#   else
#       define __open           open
#       define __close          close
#       define __fsync          fsync
#   endif // TM_GTK

#endif // TM_WIN


TM_BGN_EXTERN_C


/**
 *  @defgroup filesystem
 *
 *  @defgroup fileIO
 *  @ingroup filesystem
 */


/**
 *  @addtogroup fileIO
 *  file "c" function prototype
 *  @{
 */

/** file access */
#define TM_FILE_READ                TM_SW_WIN(GENERIC_READ, O_RDONLY)
#define TM_FILE_WRITE               TM_SW_WIN(GENERIC_WRITE, O_WRONLY)
#define TM_FILE_READWRITE           TM_SW_WIN(GENERIC_READ | GENERIC_WRITE, O_RDWR)

/** file disposition */
#define TM_FILE_CREATENEW           TM_SW_WIN(CREATE_NEW, O_CREAT | O_EXCL)
#define TM_FILE_CREATEALWAYS        TM_SW_WIN(CREATE_ALWAYS, O_CREAT | O_TRUNC)
#define TM_FILE_OPENEXISTING        TM_SW_WIN(OPEN_EXISTING, O_EXCL)
#define TM_FILE_OPENALWAYS          TM_SW_WIN(OPEN_ALWAYS, O_CREAT)
#define TM_FILE_TRUNCATEEXISTING    TM_SW_WIN(TRUNCATE_EXISTING, O_CREAT | O_EXCL | O_TRUNC)

/** file shared */
#define TM_FILE_SHARED_READ         TM_SW_WIN(FILE_SHARE_READ, 0x00000001)  // default
#define TM_FILE_SHARED_NONE         0

/** file seek */
#define TM_FILE_SEEKSET             TM_SW_WIN(FILE_BEGIN, SEEK_SET)
#define TM_FILE_SEEKCUR             TM_SW_WIN(FILE_CURRENT, SEEK_CUR)
#define TM_FILE_SEEKEND             TM_SW_WIN(FILE_END, SEEK_END)

/** file attributes */
#define TM_FILE_ATTRIBUTE_NORMAL    TM_SW_WIN(FILE_ATTRIBUTE_NORMAL, S_IFREG)
#define TM_FILE_ATTRIBUTE_READONLY  TM_SW_WIN(FILE_ATTRIBUTE_READONLY, S_IREAD)
#define TM_FILE_ATTRIBUTE_RW        TM_SW_WIN(FILE_ATTRIBUTE_SYSTEM, S_IREAD | S_IWRITE)


#define TMHFile                     TM_SW_WIN(HANDLE, int)
#define g_hFileError                TM_SW_WIN(INVALID_HANDLE_VALUE, -1)

/**
 *  creates or opens the file and returns a handle
 *  @param[in] file
 *  Pointer to a null-terminated string that specifies the name of the file
 *  @param[in] uAccess
 *  - TM_FILE_READ Specifies read access to the file.
 *  - TM_FILE_WRITE Specifies write access to the file.
 *  - TM_FILE_READWRITE Specifies read, write access to the file.
 *  @param[in] uDisposition
 *  - TM_FILE_CREATENEW
 *      Creates a new file,
 *      The function fails if the specified file already exists.
 *  - TM_FILE_CREATEALWAYS
 *      Creates a new file.
 *      If the file exists, the function overwrites the file and clears the existing attributes.
 *  - TM_FILE_OPENEXISTING
 *      Opens the file. The function fails if the file does not exist.
 *  - TM_FILE_OPENALWAYS
 *      Opens the file, if it exists.
 *      If the file does not exist, the function creates the file.
 *  - TM_FILE_TRUNCATEEXISTING
 *      Opens the file. Once opened, the function set file size to 0.
 *      The calling process must open the file with at least write access.
 *      The function fails if the file does not exist
 *  @return
 *      if error return g_hFileError
 */
TM_INLINE TMHFile FileOpen(TMCStr file, TMUInt uAccess, TMUInt uDisposition, TMUInt uShared = TM_FILE_SHARED_READ)
{
#if TM_WIN_METRO
    return CreateFile2(file, uAccess, uShared, uDisposition, NULL);
#elif TM_WIN_DESKTOP
    return CreateFile(file, uAccess, uShared, NULL, uDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
#else
    TMHFile hFile = __open(file, uAccess | uDisposition | TM_O_LARGEFILE, S_IRWXU);
    //chmod(file, S_IRWXU);
    return hFile;
#endif
}

/**
 *  read byte from file
 *  @param[in] hFile
 *  Handle to the file to be read,
 *  The file handle must have been created with read access.
 *  @param[out] pBuf
 *  Pointer to the buffer that receives the data read from the file.
 *  @param[in] uCount
 *  Specifies the number of bytes to be read from the file
 *  @return
 *  the number of bytes requested has been read.
 *  if error it will assert and return 0, not (-1).
 */
TM_INLINE size_t FileRead(TMHFile hFile, void* pBuf, size_t uCount)
{
#ifdef TM_WIN
    DWORD dw = 0; BOOL b;
    DbgAssert(hFile != g_hFileError);
    DbgAssert(uCount ? pBuf : (void*)1);
    b = ReadFile(hFile, pBuf, uCount, &dw, 0);
    if (uCount)
        DbgVerify(b);
    return dw;
#else
    int n;
    DbgAssert(hFile != g_hFileError);
    DbgAssert(!pBuf == !uCount);
    n = read(hFile, pBuf, uCount);
    DbgVerify(!uCount || n >= 0);
    return n < 0 ? 0 : n;
#endif
}

/**
 *  write byte to file
 *  @param[in] hFile
 *  Handle to the file to be write,
 *  The file handle must have been created with write access.
 *  @param[in] pBuf
 *  Pointer to the buffer that write to the file.
 *  @param[in] uCount
 *  Specifies the number of bytes to be write to the file
 *  @return
 *  the number of bytes requested has been write.
 *  if error it will assert and return 0, not (-1).
 */
TM_INLINE size_t FileWrite(TMHFile hFile, const void* pBuf, size_t uCount)
{
#ifdef TM_WIN
    BOOL b; DWORD dw = 0;
    DbgAssert(hFile != g_hFileError);
    DbgAssert(uCount ? pBuf : (void*)1);
    b = WriteFile(hFile, pBuf, uCount, &dw, 0);
    DbgAssert(b);
    return dw;
#else
    int n;
    DbgAssert(hFile != g_hFileError);
    DbgAssert(!pBuf == !uCount);
    n = write(hFile, pBuf, uCount);
    DbgAssert(n >= 0);
    return n < 0 ? 0 : n;
#endif
}

/**
 *  moves the file pointer of an open file
 *  @param[in] hFile
 *  Handle to the file whose file pointer is to be moved.
 *  @param[in] lOffset
 *  specifies the number of bytes to move the file pointer.
 *  @param[in] uOrigin
 *  - TM_FILE_SEEKSET The starting point is zero or the beginning of the file.
 *  - TM_FILE_SEEKCUR The starting point is the current value of the file pointer.
 *  - TM_FILE_SEEKEND The starting point is the current end-of-file position.
 *  @return the new file pointer, if error will assert
 */
TM_INLINE TMUInt64 FileSeek(TMHFile hFile, TMInt64 lOffset, TMUInt uOrigin)
{
#ifdef TM_WIN
    LARGE_INTEGER li;
    DbgAssert(hFile != g_hFileError);
    DbgAssert(uOrigin == TM_FILE_SEEKCUR || uOrigin == TM_FILE_SEEKSET || uOrigin == TM_FILE_SEEKEND);
    li.QuadPart = lOffset;
#   if TM_WIN_METRO
    LARGE_INTEGER li1;
    TMBool lRet = SetFilePointerEx(hFile, li, &li1, uOrigin); //Evan modified for Metro style Apps
    DbgAssert(lRet);
    return lRet == 0 ? 0 : li.QuadPart;
#   else
    li.LowPart = SetFilePointer(hFile, li.LowPart, &li.HighPart, uOrigin);
    DbgVerify (li.LowPart != (DWORD)-1);
    return li.LowPart == (DWORD)-1 ? 0 : li.QuadPart;
#   endif // TM_WIN_METRO
#else
    TMUInt64 n;
    DbgAssert(hFile != g_hFileError);
    DbgAssert(uOrigin == TM_FILE_SEEKCUR || uOrigin == TM_FILE_SEEKSET || uOrigin == TM_FILE_SEEKEND);
    n = lseek(hFile, lOffset, uOrigin);
    DbgAssert(n != (TMUInt64)-1);
    return n;
#endif // TM_WIN
}

#ifdef TM_ANDROID
#   define fsync(x)
#endif //TM_ANDROID

/** flushes the buffers of the specified file and causes all buffered data to be written to the file */
TM_INLINE void FileFlush(TMHFile hFile)
{
    DbgAssert(hFile != g_hFileError);
    TM_IF_WIN(FlushFileBuffers(hFile));
    TM_NO_WIN(__fsync(hFile));
}

/** close the opened file handle */
TM_INLINE void FileClose(TMHFile hFile)
{
    DbgAssert(hFile != g_hFileError);
    TM_SW_WIN(CloseHandle, __close)(hFile);
}

/**
 *  get file size
 *  @return if errror return 0, not -1.
 */
TM_INLINE TMUInt64 FileSize(TMHFile hFile)
{
#ifdef TM_WIN
    ULARGE_INTEGER ui = {0};
    DbgAssert(hFile != g_hFileError);
#   if TM_WIN_METRO
    FILE_STANDARD_INFO  fileinfo= {0};
    TMBool bRet = GetFileInformationByHandleEx(hFile, FileStandardInfo, &fileinfo, sizeof(FILE_STANDARD_INFO));                    
    return fileinfo.EndOfFile.QuadPart == INVALID_FILE_SIZE ? 0 : fileinfo.EndOfFile.QuadPart;
#   else
    ui.LowPart = GetFileSize(hFile, &ui.HighPart);
    return ui.QuadPart == INVALID_FILE_SIZE ? 0 : ui.QuadPart;
#   endif // TM_WIN_METRO
#else
    TMUInt64 old, size;
    DbgAssert(hFile != g_hFileError);
    old = FileSeek(hFile, 0, TM_FILE_SEEKCUR);
    size = FileSeek(hFile, 0, TM_FILE_SEEKEND);
    FileSeek(hFile, old, TM_FILE_SEEKSET);
    return size;
#endif // TM_WIN
}

/** set file size */
TM_INLINE void FileSetSize(TMHFile hFile, TMUInt64 dwSize)
{
#ifdef TM_ANDROID
#elif defined (TM_WIN)
    TMUInt64 s;
    DbgAssert(hFile != g_hFileError);
    s = FileSeek(hFile, 0, TM_FILE_SEEKCUR);
    FileSeek(hFile, dwSize, TM_FILE_SEEKSET);
    SetEndOfFile(hFile);
    FileSeek(hFile, s, TM_FILE_SEEKSET);
#else
    DbgAssert(hFile != g_hFileError);
    __ftruncate(hFile, dwSize);
#endif
}

/**
 *  moves the end-of-file (EOF) position for the specified file
 *  to the current position of the file pointer
 */
TM_INLINE void FileSetEOF(TMHFile hFile)
{
    DbgAssert(hFile != g_hFileError);
#ifdef TM_ANDROID
#elif defined TM_WIN
    SetEndOfFile(hFile);
#else
    __ftruncate(hFile, FileSeek(hFile, 0, TM_FILE_SEEKCUR));
#endif
}


/** @} */ // @addtogroup fileIO


TM_END_EXTERN_C


#ifdef __cplusplus


TM_BGN_NAMESPACE

template <class T, class Endian> struct CArchiveBinaryIT;
template <class T, class Endian> struct CArchiveBinaryOT;


/**
 *  @class CFileNative
 *  @brief packaged file function
 *  @ingroup fileIO
 *  @par sample
 *  @code
     CFileNative f;
     f.OpenExisting ("c:/1.txt", f.e_faRead);
     char c[256];
     for (; f.ReadLine(c, 256) != -1;)
     {
        todo (c); ... // c is line of file text
     }
 *  @endcode
 */
class CFileNative
{
public:
    typedef CFileNative                                 class_type;
    typedef TMHFile                                     handle_type;

    typedef void*                                       pointer;
    typedef const void*                                 const_pointer;
    typedef size_t                                      size_type;
    typedef ptrdiff_t                                   difference_type;

    typedef CArchiveBinaryIT<CFileNative, CEndianHost>  CArchiveI;
    typedef CArchiveBinaryOT<CFileNative, CEndianHost>  CArchiveO;

    TM_UNUSED_IC_GetDebugInfo();

    enum EAccess
    {
        e_faRead                = TM_FILE_READ,
        e_faWrite               = TM_FILE_WRITE,
        e_faReadWrite           = TM_FILE_READWRITE,
    };

    enum EDisposition
    {
        e_fdCreateNew           = TM_FILE_CREATENEW,
        e_fdCreateAlways        = TM_FILE_CREATEALWAYS,
        e_fdOpenExisting        = TM_FILE_OPENEXISTING,
        e_fdOpenAlways          = TM_FILE_OPENALWAYS,
        e_fdTruncateExisting    = TM_FILE_TRUNCATEEXISTING,
    };

    DbgAssertS (
        e_stSet == TM_FILE_SEEKSET &&
        e_stCur == TM_FILE_SEEKCUR &&
        e_stEnd == TM_FILE_SEEKEND);

protected:
    handle_type m_hFile;

    TM_NO_COPYABLE(CFileNative);

public:
    /** */
    explicit CFileNative(handle_type hFile = g_hFileError)
        : m_hFile   (hFile)
    {
    }

    /** */ ~CFileNative() { Close(); }

    /** */ operator TMHFile() const { return g_hFileError == m_hFile ? 0 : m_hFile; }

    /** */
    void Attach(handle_type hFile)
    {
        DbgAssert(!*this);
        m_hFile = hFile;
    }

    /** */
    handle_type Detach()
    {
        handle_type hFile = m_hFile;
        m_hFile = g_hFileError;
        return hFile;
    }

    /** */ void Swap(CFileNative& rhs) { tm_swap(rhs.m_hFile, m_hFile); }

    void NativeOpen(TMCStr name, EAccess eAccess, EDisposition disposition)
    {
        DbgAssert(!*this);
        m_hFile = FileOpen(name, eAccess, disposition);
    }

    /**
     *  @copydoc FileOpen
     *  @see FileOpen
     *   The function fails if the specified file already exists.
     */
    void CreateNew(TMCStr name, EAccess eAccess, TMUInt uShared = TM_FILE_SHARED_READ)
    {
        DbgAssert(!*this);
        m_hFile = FileOpen(name, eAccess, TM_FILE_CREATENEW, uShared);
    }

    /**
     *  @copydoc FileOpen
     *  @see FileOpen
     *   If the file exists, the function overwrites the file and clears the existing attributes.
     */
    void CreateAlways(TMCStr name, EAccess eAccess, TMUInt uShared = TM_FILE_SHARED_READ)
    {
        DbgAssert(!*this);
        m_hFile = FileOpen(name, eAccess, TM_FILE_CREATEALWAYS, uShared);
    }

    /**
     *  @copydoc FileOpen
     *  @see FileOpen
     */
    void OpenExisting(TMCStr name, EAccess eAccess, TMUInt uShared = TM_FILE_SHARED_READ)
    {
        DbgAssert(!*this);
        m_hFile = FileOpen(name, eAccess, TM_FILE_OPENEXISTING, uShared);
    }

    /**
     *  @copydoc FileOpen
     *  @see FileOpen
     */
    void OpenAlways(TMCStr name, EAccess eAccess, TMUInt uShared = TM_FILE_SHARED_READ)
    {
        DbgAssert(!*this);
        m_hFile = FileOpen(name, eAccess, TM_FILE_OPENALWAYS, uShared);
    }

    /**
     *  @copydoc FileOpen
     *  @see FileOpen
     */
    void TruncateExisting(TMCStr name, EAccess eAccess, TMUInt uShared = TM_FILE_SHARED_READ)
    {
        DbgAssert(!*this);
        m_hFile = FileOpen(name, eAccess, TM_FILE_TRUNCATEEXISTING, uShared);
    }

    /**
     *  @copydoc FileRead
     *  @see FileRead
     */
    size_type Read (pointer lpBuffer, size_type lBytes) const
    {
        DbgAssert(*this);
        return FileRead(m_hFile, lpBuffer, lBytes);
    }

    /**
     *  @copydoc FileWrite
     *  @see FileWrite
     */
    size_type Write(const_pointer lpBuffer, size_type lBytes)
    {
        DbgAssert(*this);
        return FileWrite(m_hFile, lpBuffer, lBytes);
    }

    /**
     *  @copydoc FileSeek
     *  @see FileSeek, Seek
     */
    TMUInt64 SeekSet(TMInt64 lOffset)
    {
        DbgAssert(*this);
        return Seek(lOffset, e_stSet);
    }

    /**
     *  @copydoc FileSeek
     *  @see FileSeek, Seek
     */
    TMUInt64 SeekCur(TMInt64 lOffset)
    {
        DbgAssert(*this);
        return Seek(lOffset, e_stCur);
    }

    /**
     *  @copydoc FileSeek
     *  @see FileSeek, Seek
     */
    TMUInt64 SeekEnd(TMInt64 lOffset)
    {
        DbgAssert(*this);
        return Seek(lOffset, e_stEnd);
    }

    /**
     *  @copydoc FileSeek
     *  @see FileSeek
     */
    TMUInt64 Seek(TMInt64 lOffset, ESeekType eSeekType)
    {
        DbgAssert(*this);
        return FileSeek(m_hFile, lOffset, eSeekType);
    }

    /**
     *  @copydoc FileFlush
     *  @see FileFlush
     */
    void Flush()
    {
        DbgAssert(*this);
        FileFlush(m_hFile);
    }

    /**
     *  @copydoc FileClose
     *  @see FileClose
     */
    void Close()
    {
        if (*this)
        {
            FileClose(m_hFile);
            m_hFile = g_hFileError;
        }
    }

    /**
     *  @copydoc FileSize
     *  @see FileSize
     */
    TMUInt64 GetSize() const
    {
        DbgAssert(*this);
        return FileSize(m_hFile);
    }

    /**
     *  get current file pointer
     *  @see Seek, FileSeek
     */
    TMUInt64 Tell() const
    {
        DbgAssert(*this);
        return FileSeek(m_hFile, 0, e_stCur);
    }

    /**
     *  @copydoc FileSetSize
     *  @see FileSetSize, FileSetEOF
     */
    void SetSize(TMUInt64 dwSize)
    {
        DbgAssert(*this);
        FileSetSize(m_hFile, dwSize);
    }

    /**
     *  @copydoc FileSetEOF
     *  @see FileSetEOF, FileSetSize
     */
    void SetEof()
    {
        DbgAssert(*this);
        FileSetEOF(m_hFile);
    }
};


#ifdef TM_QT

class CFileQt : public QFile
{
public:
    typedef CFileQt                                 class_type;
    
    typedef void*                                   pointer;
    typedef const void*                             const_pointer;
    typedef qint64                                  size_type;
    typedef qint64                                  difference_type;
    
    typedef CArchiveBinaryIT<CFileNative, CEndianHost>    CArchiveI;
    typedef CArchiveBinaryOT<CFileNative, CEndianHost>    CArchiveO;

    enum EAccess
    {
        e_faRead                = QIODevice::ReadOnly,
        e_faWrite               = QIODevice::WriteOnly,
        e_faReadWrite           = QIODevice::ReadWrite,
    };
    
    enum EDisposition
    {
        e_fdCreateNew           = TM_FILE_CREATENEW,
        e_fdCreateAlways        = TM_FILE_CREATEALWAYS,
        e_fdOpenExisting        = TM_FILE_OPENEXISTING,
        e_fdOpenAlways          = TM_FILE_OPENALWAYS,
        e_fdTruncateExisting    = TM_FILE_TRUNCATEEXISTING,
    };

public:
    operator bool () const { return this->isOpen(); }

//     void Swap (CFileQT& rhs)
//     {
//         QSwap(*this, rhs);
//     }

    void NativeOpen(TMCStr name, EAccess eAccess, EDisposition disposition)
    {
        DbgAssert(!*this);
        switch (disposition)
        {
        case e_fdCreateNew: CreateNew(name, eAccess); break;
        case e_fdCreateAlways: CreateAlways(name, eAccess); break;
        case e_fdOpenExisting: OpenExisting(name, eAccess); break;
        case e_fdOpenAlways: OpenAlways(name, eAccess); break;
        case e_fdTruncateExisting: TruncateExisting(name, eAccess); break;
        }
    }

    void CreateNew(TMCStr name, EAccess eAccess)
    {
        DbgAssert(!*this);
        this->setFileName(TM_T2Q(name));
        if (this->exists())
            return;
        this->open((OpenMode)(eAccess | Truncate));
    }

    void CreateAlways(TMCStr name, EAccess eAccess)
    {
        DbgAssert(!*this);
        this->setFileName(TM_T2Q(name));
        this->open((OpenMode)(eAccess | Truncate));
    }

    void OpenExisting(TMCStr name, EAccess eAccess)
    {
        DbgAssert(!*this);
        this->setFileName(TM_T2Q(name));
        if (!this->exists())
            return;
        this->open((OpenMode)(eAccess));
    }

    void OpenAlways(TMCStr name, EAccess eAccess)
    {
        DbgAssert(!*this);
        this->setFileName(TM_T2Q(name));
        this->open((OpenMode)(eAccess));
    }

    void TruncateExisting(TMCStr name, EAccess eAccess)
    {
        DbgAssert(!*this);
        this->setFileName(TM_T2Q(name));
        if (!this->exists())
            return;
        this->open((OpenMode)(eAccess) | Truncate);
    }

    size_type Read(pointer lpBuffer, size_type lBytes) const
    {
        DbgAssert(*this);
        return const_cast<CFileQt*>(this)->read((char*)lpBuffer, lBytes);
    }

    size_type Write(const_pointer lpBuffer, size_type lBytes)
    {
        DbgAssert(*this);
        return this->write((const char*)lpBuffer, lBytes);
    }

//     long ReadLine(pointer lpBuffer, size_t lBufSize)
//     {
//         DbgAssert(*this);
//         return this->readLine((char*)lpBuffer, lBufSize);
//     }

    TMUInt64 SeekSet(TMInt64 lOffset)
    {
        DbgAssert(*this);
        this->seek(lOffset);
        return this->pos();
    }

    TMUInt64 SeekCur(TMInt64 lOffset)
    {
        DbgAssert(*this);
        return SeekSet(this->pos() + lOffset);
    }

    TMUInt64 SeekEnd(TMInt64 lOffset)
    {
        DbgAssert(*this);
        return SeekSet(this->size() + lOffset);
    }

    TMUInt64 Seek(TMInt64 lOffset, ESeekType eSeekType)
    {
        DbgAssert(*this);
        switch (eSeekType)
        {
        case e_stSet: return SeekSet(lOffset);
        case e_stCur: return SeekCur(lOffset);
        case e_stEnd: return SeekEnd(lOffset);
        }
        return this->pos();
    }

    void Flush()
    {
        DbgAssert(*this);
        this->flush();
    }

    void Close()
    {
        this->close();
    }

    TMUInt64 GetSize() const
    {
        DbgAssert(*this);
        return this->size();
    }

    TMUInt64 Tell() const
    {
        DbgAssert(*this);
        return this->pos();
    }

    void SetSize(TMUInt64 dwSize)
    {
        DbgAssert(*this);
        this->resize(dwSize);
    }

    void SetEof()
    {
        DbgAssert(*this);
        SetSize(this->pos());
    }
};



#endif // #ifdef TM_QT



#ifdef TM_QT
#   define CFile CFileQt
#else
#   define CFile CFileNative
#endif



TM_END_NAMESPACE


#endif // end of #ifdef __cplusplus


#endif // end of TMFILE_H


