/**
 *  @file
 *  @brief
 *  file mapping provides two major advantages: 
 *  - Shared memory 
 *  - Faster and easier file access 
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.11.28
 *
 *  Copyright (C) 2009 miragekiller
 *  All rights reserved.
 */

#ifndef TMFMAP_H
#define TMFMAP_H

#ifndef TMFILE_H
#   include "tmfile.h"
#endif

#ifndef TM_WIN
#   include <sys/mman.h>
#endif


/**
 *  @defgroup filesystem
 *  @defgroup process
 *  @defgroup synchronization
 *
 *  @defgroup mapping
 *  @ingroup filesystem process synchronization
 */


/**
 *  @addtogroup mapping
 *  file map "c" function prototype
 *  @{
 */

/** access */
#define TM_FMAP_READ                TM_SW_WIN (FILE_MAP_READ, PROT_READ)
#define TM_FMAP_WRITE               TM_SW_WIN (FILE_MAP_WRITE, PROT_WRITE)

/**
 * @struct TMFMapping
 * please use this function to init: TMFMapping x = { 0 };
 */
typedef struct
{
    void* pBuf;
    size_t dwSize;
    TM_IF_WIN(HANDLE hMap);
} TMFMapping;

/** 
 *  init TMFMapping struct
 *  @param[in] hFile Handle to the file from which to create a mapping object.
 */
TM_INLINE void FMapInit(TMFMapping* pMap)
{
    MemZeroT(*pMap);
}

/** 
 *  maps a view of a file into the address space of the calling process.
 *  @param[out] pMap receive mapping value
 *  @param[in] hFile Handle to the file from which to create a mapping object.
 *  @param[in] dwPos file offset where mapping is to begin.
 *  @param[in] dwSize number of bytes of the file to map. if 0 then the entire file is mapped.
 *  @param[in] uFlag the type of access to the file mapping.
 *  - TM_FMAP_READ Read-only access.
 *  - TM_FMAP_WRITE Read/write access.
 */
TM_INLINE void FMapMap(TMFMapping* pMap, TMHFile hFile, TMUInt64 dwPos, 
                            size_t dwSize, TMUInt uFlag)
{
#ifdef TM_WIN
    HANDLE hr; void* p; ULARGE_INTEGER ui;
    TMUInt ua = (uFlag & TM_FMAP_WRITE) ? PAGE_READWRITE
        : ((uFlag & TM_FMAP_READ) ? PAGE_READONLY : 0);
    DbgAssert(pMap);
#if TM_WIN_METRO
    hr = CreateFileMappingFromApp(hFile, NULL, ua, 0, 0);
#else
    hr = CreateFileMapping(hFile, NULL, ua, 0, 0, NULL);
#endif
    if (hr)
    {
        ui.QuadPart = dwPos;
#if TM_WIN_METRO
        p = MapViewOfFileFromApp(hr, uFlag, ui.QuadPart, dwSize);
#else
        p = MapViewOfFile(hr, uFlag, ui.HighPart, ui.LowPart, dwSize);
#endif
        if (p)
        {
            pMap->hMap = hr;
            pMap->pBuf = p;
            pMap->dwSize = FileSize(hFile);
        }
        else
            CloseHandle(hr);
    }
#else
    void* p;
    DbgAssert(pMap);
    p = mmap(NULL, dwSize, uFlag, MAP_SHARED, hFile, dwPos);
    if (p != (void *)-1)
    {
        pMap->pBuf = p;
        pMap->dwSize = dwSize;
    }
#endif
}

/** 
 *  unmaps a file mapping
 */
TM_INLINE void FMapUnmap(TMFMapping* pMap)
{
    DbgAssert(pMap);
#ifdef TM_WIN
    if (pMap->pBuf)
    {
        UnmapViewOfFile(pMap->pBuf);
        pMap->pBuf = 0;
    }
    if (pMap->hMap)
    {
        CloseHandle(pMap->hMap);
        pMap->hMap = 0;
    }
#else
    if (pMap->pBuf)
    {
        munmap(pMap->pBuf, pMap->dwSize);
        pMap->pBuf = 0;
        pMap->dwSize = 0;
    }
#endif
} 

/** @} */ // @addtogroup fileIO


#ifdef __cplusplus


TM_BGN_NAMESPACE


/**
 *  @class CFileMapping
 *  @brief packaged file_map function
 *  @ingroup mapping
 *  @par sample
 *  @code
    CFile f;
    f.OpenExisting("c:/1.txt", f.e_faRead);
    CFileMapping fm;
    fm.Map(f, fm.e_faRead);
    CAXmlTag xml(fm, f.GetSize());
    todo(xml); ... 
 *  @endcode
 */
class CFileMapping
{
public:
    typedef CFileMapping                            class_type;

    typedef void*                                   pointer;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;
    
    enum EAccess 
    { 
        e_fmRead        = TM_FMAP_READ,
        e_fmWrite       = TM_FMAP_WRITE,
        e_fmReadWrite   = TM_FMAP_READ | TM_FMAP_WRITE,
    };

protected:
    TMFMapping m_mp;

    TM_NO_COPYABLE(CFileMapping);

public:
    /** */ 
    CFileMapping()
    {
        FMapInit(&m_mp);
    }

    /** */ ~CFileMapping() { if (*this) Unmap(); }

    /** */ operator TMStrA() const { return (TMStrA)m_mp.pBuf; }

    /**
     *  @copydoc FMapMap
     *  @see FMapMap
     */
    void Map(TMHFile hFile, EAccess eAccess = e_fmRead, TMUInt64 dwPos = 0, size_t dwSize = 0)
    {
        DbgAssert(!*this);
        FMapMap(&m_mp, hFile, dwPos, dwSize, eAccess);
    }

    /**
     *  @copydoc FMapUnmap
     *  @see FMapUnmap
     */
    void Unmap()
    {
        DbgAssert(*this);
        FMapUnmap(&m_mp);
    }

    size_t GetSize() const
    {
        return m_mp.dwSize;
    }

    TMStrA Begin() { return (TMStrA)m_mp.pBuf; }
    TMStrA End() { return Begin() + m_mp.dwSize; }
};


TM_END_NAMESPACE


#endif // end of #ifdef __cplusplus
   

#endif // end of TMFMAP_H


