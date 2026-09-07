/*
 *  @file
 *  @brief  drag drop
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.02.18
 *
 *  Copyright (C) 2009 miragekiller
 */

#pragma once

#ifndef TMDRAG_H
#define TMDRAG_H

#include "tmdef.h"

// ---------- Platform-specific includes ----------
#if defined(TM_GTK)
#   include "tmstring.h"
#   include "tmcolor.h"
#   include "tmnative.h"
#   include <vector>
#else
#   include <windows.h>
#   include <ShellAPI.h>
#   include "tmatl.h"
#   include "tmstring.h"
#   include "tmcolor.h"
#   include "tmnative.h"
#   define TM_DRAG_WIN_TEXT TM_T("tm_drag")
#endif
// ---------- GTK compat: Win32 constants/types not provided by system headers ----------
#if defined(TM_GTK)
#   ifndef S_OK
#       define S_OK 0
#   endif
#   ifndef DRAGDROP_S_USEDEFAULTCURSORS
#       define DRAGDROP_S_USEDEFAULTCURSORS 0
#   endif
#   ifndef DVASPECT_CONTENT
#       define DVASPECT_CONTENT 1
#   endif
#   ifndef MK_SHIFT
#       define MK_SHIFT 0x0004
#   endif
#   ifndef MK_CONTROL
#       define MK_CONTROL 0x0008
#   endif
#   ifndef CF_TEXT
#       define CF_TEXT          1
#   endif
#   ifndef CF_BITMAP
#       define CF_BITMAP        2
#   endif
#   ifndef CF_METAFILEPICT
#       define CF_METAFILEPICT  3
#   endif
#   ifndef CF_SYLK
#       define CF_SYLK          4
#   endif
#   ifndef CF_DIF
#       define CF_DIF           5
#   endif
#   ifndef CF_TIFF
#       define CF_TIFF          6
#   endif
#   ifndef CF_OEMTEXT
#       define CF_OEMTEXT       7
#   endif
#   ifndef CF_DIB
#       define CF_DIB           8
#   endif
#   ifndef CF_PALETTE
#       define CF_PALETTE       9
#   endif
#   ifndef CF_PENDATA
#       define CF_PENDATA       10
#   endif
#   ifndef CF_RIFF
#       define CF_RIFF          11
#   endif
#   ifndef CF_WAVE
#       define CF_WAVE          12
#   endif
#   ifndef CF_UNICODETEXT
#       define CF_UNICODETEXT   13
#   endif
#   ifndef CF_ENHMETAFILE
#       define CF_ENHMETAFILE   14
#   endif
#   ifndef CF_HDROP
#       define CF_HDROP         15
#   endif
#   ifndef CF_LOCALE
#       define CF_LOCALE        16
#   endif
#   ifndef CF_MAX
#       define CF_MAX           18
#   endif
#   ifndef DROPEFFECT_NONE
#       define DROPEFFECT_NONE   0
#   endif
#   ifndef DROPEFFECT_COPY
#       define DROPEFFECT_COPY   1
#   endif
#   ifndef DROPEFFECT_MOVE
#       define DROPEFFECT_MOVE   2
#   endif
#   ifndef DROPEFFECT_LINK
#       define DROPEFFECT_LINK   4
#   endif
#   ifndef DROPEFFECT_SCROLL
#       define DROPEFFECT_SCROLL 0x80000000u
#   endif
#   ifndef TYMED_NULL
#       define TYMED_NULL     0
#   endif
#   ifndef TYMED_HGLOBAL
#       define TYMED_HGLOBAL  1
#   endif
#   ifndef TYMED_FILE
#       define TYMED_FILE     2
#   endif
#   ifndef TYMED_ISTREAM
#       define TYMED_ISTREAM  4
#   endif
#   ifndef TYMED_ISTORAGE
#       define TYMED_ISTORAGE 8
#   endif
#   ifndef TYMED_GDI
#       define TYMED_GDI      16
#   endif
#   ifndef TYMED_MFPICT
#       define TYMED_MFPICT   32
#   endif
#   ifndef TYMED_ENHMF
#       define TYMED_ENHMF    64
#   endif
typedef TMUInt32 DWORD;
typedef TMUInt16 WORD;
typedef TMLong   LONG;
typedef int      BOOL;
typedef void*    HGLOBAL;
typedef void*    HENHMETAFILE;
typedef WORD     CLIPFORMAT;
struct IDataObject;
struct IDropTarget;
struct IStream;
typedef struct tagFORMATETC
{
    CLIPFORMAT cfFormat;
    void* ptd;
    DWORD dwAspect;
    LONG lindex;
    DWORD tymed;
} FORMATETC, *LPFORMATETC;
typedef struct tagSTGMEDIUM
{
    DWORD tymed;
    union
    {
        HGLOBAL hGlobal;
        void* pstm;
        void* pstg;
    };
    void* pUnkForRelease;
} STGMEDIUM, *LPSTGMEDIUM;
#endif // TM_GTK compat types

TM_BGN_NAMESPACE


#define _TM_W_G(x,y)  TM_SW_GTK(y,x)

tm_enum(EClipboardFmt)
{
    eCfNull         = 0,
    eCfText         = 1,    // CF_TEXT
    eCfBitmap       = 2,    // CF_BITMAP,
    eCfMFPict       = 3,    // CF_METAFILEPICT,
    eCfSYLK         = 4,    // CF_SYLK,
    eCfDif          = 5,    // CF_DIF,
    eCfTiff         = 6,    // CF_TIFF,
    eCfOEMText      = 7,    // CF_OEMTEXT,
    eCfDib          = 8,    // CF_DIB,
    eCfPalette      = 9,    // CF_PALETTE,
    eCfPenData      = 10,   // CF_PENDATA,
    eCfRiff         = 11,   // CF_RIFF,
    eCfWave         = 12,   // CF_WAVE,
    eCfUnicodeText  = 13,   // CF_UNICODETEXT,
    eCfEMF          = 14,   // CF_ENHMETAFILE,
    eCfHDROP        = 15,   // CF_HDROP,
    eCfLocale       = 16,   // CF_LOCALE,

    eCfHtml         = 17,   // (CF_MAX + 1),
    eCfRtf          = 18,   // (CF_MAX + 2),
    eCfPng          = 19,   // (CF_MAX + 3),
    eCfJpeg         = 20,   // (CF_MAX + 4),
};

tm_enum(EDragEff)
{
    eDragEffNone    = 0,  // DROPEFFECT_NONE,
    eDragEffCopy    = _TM_W_G(DROPEFFECT_COPY, GDK_ACTION_COPY),
    eDragEffMove    = _TM_W_G(DROPEFFECT_MOVE, GDK_ACTION_MOVE),
    eDragEffLink    = _TM_W_G(DROPEFFECT_LINK, GDK_ACTION_LINK),
//     eDragEffScroll = DROPEFFECT_SCROLL,
};
TM_ENUM_2_FLAG(EDragEff);

tm_enum(EMedType)
{
    eMtNull         = 0,    // TYMED_NULL,
    eMtHGlobal      = 1,    // TYMED_HGLOBAL,   // HGLOBAL
    eMtFile         = 2,    // TYMED_FILE,      // LPOLESTR
    eMtStream       = 4,    // TYMED_ISTREAM,   // IStream
    eMtStorage      = 8,    // TYMED_ISTORAGE,  // IStorage
    eMtGDI          = 16,   // TYMED_GDI,       // HBITMAP
    eMtMFPict       = 32,   // TYMED_MFPICT,    // HMETAFILEPICT
    eMtEmf          = 64,   // TYMED_ENHMF,     // HENHMETAFILE
};
TM_ENUM_2_FLAG(EMedType);


struct IDragSource
{
public:
    virtual TMBool OnDraging(TMPoint pt, TMUInt32 dwKeyState) // dwKeyState: MK_XX1 | MK_XX2 ...
    {
        tm_unused(pt);
        tm_unused(dwKeyState);
        return true;
    }
    virtual void OnGiveFeedback(EDragEff dwEffect)
    {
        tm_unused(dwEffect);
    }
};

#ifdef TM_WIN
    typedef ::IDropTarget   IDropTarget;
#else
    struct IDropTarget : public IUnknown
    {
    private:
        bool    m_bEntry;
        friend struct IDropTargetImpl;
    public:
        virtual ComResult DragEnter(IDataObject* data, COMDword keyState, COMPointL pt, COMDword* effect) = 0;
        virtual ComResult DragOver(COMDword keyState, COMPointL pt, COMDword* effect) = 0;
        virtual ComResult DragLeave() = 0;
        virtual ComResult Drop(IDataObject* data, COMDword keyState, COMPointL pt, COMDword* effect) = 0;
    };
#endif // TM_WIN

class IDragApp
{
public:
    virtual void Init() = 0;
    virtual void Close() = 0;

    // general
    virtual void SetImage(TMPointI ptOffset, TMHDC hDC, const TMRectI& rc, TMUInt8 alpha, TMBool bSrcAlpha, TMColor clrKey) = 0;
    virtual TMBool ShowImage(TMBool bShow) = 0;
    virtual DWORD DoDrag(const void* key, TM::IDragSource* source, EDragEff dwEffMask) = 0;
    virtual TMBool Stop(const void* key) = 0;

    // add data
    virtual void AddMem(const void* pMem, size_t s, WORD fmt, DWORD dwAspect, LONG lindex) = 0;
    virtual void AddTextA(TMCStrA pStr, WORD fmt, DWORD dwAspect, LONG lindex) = 0;
    virtual void AddTextW(TMCStrW pStr, WORD fmt, DWORD dwAspect, LONG lindex) = 0;
    virtual void AddGlobal(HGLOBAL hGlb, WORD fmt, DWORD dwAspect, LONG lindex) = 0;
    virtual void AddTextAW(TMCStr pStr, DWORD dwAspect, LONG lindex) = 0;
    virtual void AddMeta(HENHMETAFILE hMeta, WORD fmt, DWORD dwAspect, LONG lindex) = 0;
    virtual void AddStream(IStream* pStream, WORD fmt, DWORD dwAspect, LONG lindex) = 0;
    virtual void AddFiles(TMCStr pFiles, DWORD dwAspect, LONG lindex) = 0; // fn1\0fn2\0...\0\0
    virtual void AddFileArray(TMCStr* ppFiles, size_t dwCount, DWORD dwAspect, LONG lindex) = 0;
};

TM_END_NAMESPACE

struct IDataObject;
struct IDropTarget;

TM_CAPI TM::IDragApp* DragGetApp();
TM_CAPI TMBool DropGetData(IDataObject* data, void** ppv, WORD fmt, DWORD dwAspect, LONG lindex, DWORD tymed);
TM_CAPI TMBool DropGetStringA(IDataObject* data, TM::CStringA* str, WORD fmt, DWORD dwAspect, LONG lindex);
TM_CAPI TMBool DropGetStringW(IDataObject* data, TM::CStringW* str, WORD fmt, DWORD dwAspect, LONG lindex);
#if defined(TM_GTK)
TM_CAPI TMBool DragRegisterGtk(TMHWnd hWnd, IDropTarget* dt);
TM_CAPI TMBool DragRevokeGtk(TMHWnd hWnd);
#endif
inline  TMBool DragRegister(TMHWnd hWnd, TM::IDropTarget* dt)
{
#ifdef TM_WIN
    return S_OK == RegisterDragDrop(hWnd, dt);
#elif defined(TM_GTK)
    return DragRegisterGtk(hWnd, dt);
#else
    tm_unused(hWnd);
    tm_unused(dt);
    return false;
#endif
}

inline  TMBool DragRevoke(TMHWnd hWnd)
{
#ifdef TM_WIN
    return S_OK == RevokeDragDrop(hWnd);
#elif defined(TM_GTK)
    return DragRevokeGtk(hWnd);
#else
    tm_unused(hWnd);
    return false;
#endif
}


#if defined(TM_GTK)
// Single object wrappers (one drag-data-received callback)
TM_CAPI void GtkDropDataSet(const void* gtkSelectionData);
TM_CAPI void GtkDropDataClear();
TM_CAPI TMBool DropGetUriList(TM::CStringA* pUriList);
TM_CAPI TMBool DropGetLocalFileListW(TM::CStringW* pFileList);
TM_CAPI TMBool DropGetLocalFileListA(TM::CStringA* pFileList);

// Multi-type bundle (accumulate across multiple drag-data-received callbacks)
TM_CAPI void GtkDropBundleAdd(const void* gtkSelectionData);
TM_CAPI size_t GtkDropBundleGetItemCount();
TM_CAPI const char* GtkDropBundleGetMimeAt(size_t index);
TM_CAPI WORD GtkDropBundleGetFormatAt(size_t index);
TM_CAPI TMBool GtkDropBundleHasFormat(WORD fmt);
TM_CAPI TMBool GtkDropBundleGetString(WORD fmt, TM::CStringW* str);
#endif

#define DROP_KEY2EFF(key) \
    ((key & MK_CONTROL) ? \
    ((key & MK_SHIFT) ? eDragEffLink : eDragEffCopy) : \
    ((key & MK_SHIFT) ? eDragEffMove : eDragEffNone))



// ---------- Win-only drag helper templates (CDropT / CGlobalMemT) ----------
#if !defined(TM_GTK)
TM_BGN_NAMESPACE
template <bool t_bManaged>
class CDropT
{
public:
    HDROP m_hDrop;
    CDropT(HDROP hDrop = NULL) : m_hDrop(hDrop) {}
    ~CDropT()
    {
        if (t_bManaged && m_hDrop != NULL)
            DeleteObject();
    }
    CDropT<t_bManaged>& operator = (HDROP hDrop) { Attach(hDrop); return *this; }
    void Attach(HDROP hDrop)
    {
        if (t_bManaged && m_hDrop != NULL && m_hDrop != hDrop)
            ::DragFinish(m_hDrop);
        m_hDrop = hDrop;
    }
    HDROP Detach() { HDROP h = m_hDrop; m_hDrop = NULL; return h; }
    operator HDROP() const { return m_hDrop; }
    bool IsNull() const { return m_hDrop == NULL; }
    void DeleteObject()
    {
        DbgAssert(m_hDrop != NULL);
        ::DragFinish(m_hDrop);
        m_hDrop = NULL;
    }
    UINT DragQueryFile(UINT iFile, LPTSTR lpszFile, UINT cch)
    {
        DbgAssert(m_hDrop);
        return ::DragQueryFile(m_hDrop, iFile, lpszFile, cch);
    }
    BOOL DragQueryPoint(LPPOINT lppt)
    {
        DbgAssert(m_hDrop);
        return ::DragQueryPoint(m_hDrop, lppt);
    }
};
typedef CDropT<false> CDropHandle;
typedef CDropT<true>  CDrop;
template <bool t_bManaged>
class CGlobalMemT
{
public:
    HGLOBAL m_hGlobal;
    CGlobalMemT(HGLOBAL hGlobal = NULL) : m_hGlobal(hGlobal) {}
    ~CGlobalMemT()
    {
        if (t_bManaged && m_hGlobal != NULL)
            DeleteObject();
    }
    CGlobalMemT<t_bManaged>& operator = (HGLOBAL hGlobal) { Attach(hGlobal); return *this; }
    void Attach(HGLOBAL hGlobal)
    {
        if (t_bManaged && m_hGlobal != NULL && m_hGlobal != hGlobal)
            ::GlobalFree(m_hGlobal);
        m_hGlobal = hGlobal;
    }
    HGLOBAL Detach() { HGLOBAL h = m_hGlobal; m_hGlobal = NULL; return h; }
    operator HGLOBAL() const { return m_hGlobal; }
    bool IsNull() const { return m_hGlobal == NULL; }
    void DeleteObject()
    {
        DbgAssert(m_hGlobal != NULL);
        ::GlobalFree(m_hGlobal);
        m_hGlobal = NULL;
    }
    HGLOBAL GlobalAlloc(DWORD dwBytes, UINT uFlags = GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_SHARE)
    {
        DbgAssert(m_hGlobal == NULL);
        m_hGlobal = ::GlobalAlloc(uFlags, dwBytes);
        return m_hGlobal;
    }
    size_t GlobalSize()
    {
        DbgAssert(m_hGlobal != NULL);
        return ::GlobalSize(m_hGlobal);
    }
    LPVOID GlobalLock()
    {
        DbgAssert(m_hGlobal != NULL);
        return ::GlobalLock(m_hGlobal);
    }
    BOOL GlobalUnlock()
    {
        DbgAssert(m_hGlobal != NULL);
        return ::GlobalUnlock(m_hGlobal);
    }
    HGLOBAL GlobalReAlloc(size_t dwBytes, UINT uFlags = GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_SHARE)
    {
        return m_hGlobal = ::GlobalReAlloc(m_hGlobal, dwBytes, uFlags);
    }
    HGLOBAL Assign(const void* p, size_t dwSize, UINT uFlags = GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_SHARE)
    {
        DbgAssert(m_hGlobal == NULL);
        GlobalAlloc(dwSize, uFlags);
        MemCpy(GlobalLock(), p, dwSize);
        GlobalUnlock();
        return m_hGlobal;
    }
    template <class T>
    HGLOBAL AssignT(const T& __val, UINT uFlags = GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_SHARE)
    {
        return Assign(&__val, sizeof(T), uFlags);
    }
    size_t CopyTo(LPVOID p, size_t dwSize)
    {
        DbgAssert(m_hGlobal);
        DbgAssert(p);
        size_t s = min(GlobalSize(), dwSize);
        MemCpy(p, GlobalLock(), s);
        GlobalUnlock();
        return s;
    }
    template <class T>
    size_t CopyToT(T& __val) { return CopyTo(&__val, sizeof(T)); }
};
typedef CGlobalMemT<false> CGlobalMemHandle;
typedef CGlobalMemT<true>  CGlobalMem;
TM_END_NAMESPACE
#endif // !TM_GTK (Win-only templates)

// ---------- Shared drag interfaces and classes ----------
TM_BGN_NAMESPACE
class CDragBase
{
public:
    enum EFmt
    {
        e_cfNull        = 0,
        e_cfText        = CF_TEXT,
        e_cfBitmap      = CF_BITMAP,
        e_cfMFPict      = CF_METAFILEPICT,
        e_cfSYLK        = CF_SYLK,
        e_cfDif         = CF_DIF,
        e_cfTiff        = CF_TIFF,
        e_cfOEMText     = CF_OEMTEXT,
        e_cfDib         = CF_DIB,
        e_cfPalette     = CF_PALETTE,
        e_cfPenData     = CF_PENDATA,
        e_cfRiff        = CF_RIFF,
        e_cfWave        = CF_WAVE,
        e_cfUnicodeText = CF_UNICODETEXT,
        e_cfEMF         = CF_ENHMETAFILE,
        e_cfHDROP       = CF_HDROP,
        e_cfLocale      = CF_LOCALE,
        e_cfMax         = CF_MAX,
        // Cross-platform extension IDs for richer payloads on GTK.
        e_cfHtml        = (CF_MAX + 1),
        e_cfRtf         = (CF_MAX + 2),
        e_cfPng         = (CF_MAX + 3),
        e_cfJpeg        = (CF_MAX + 4),
    };
//     enum EEff
//     {
//         e_efNone   = DROPEFFECT_NONE,
//         e_efCopy   = DROPEFFECT_COPY,
//         e_efMove   = DROPEFFECT_MOVE,
//         e_efLink   = DROPEFFECT_LINK,
//         e_efScroll = DROPEFFECT_SCROLL,
//     };
//     enum EMedType
//     {
//         eMtNull    = TYMED_NULL,
//         eMtHGlobal = TYMED_HGLOBAL,  // HGLOBAL
//         eMtFile    = TYMED_FILE,      // LPOLESTR
//         eMtStream  = TYMED_ISTREAM,   // IStream
//         eMtStorage = TYMED_ISTORAGE,  // IStorage
//         eMtGDI     = TYMED_GDI,       // HBITMAP
//         eMtMFPict  = TYMED_MFPICT,    // HMETAFILEPICT
//         eMtEmf     = TYMED_ENHMF,     // HENHMETAFILE
//     };
};
class CDataFormat
{
public:
    WORD m_dwFmt;
    CDataFormat(CDragBase::EFmt eFmt) : m_dwFmt((WORD)eFmt) {}
    CDataFormat(WORD dwFmt) : m_dwFmt(dwFmt) {}
    // GTK: map common named clipboard formats to stable custom IDs.
    CDataFormat(TMCStr pFmt) : m_dwFmt(
#if defined(TM_GTK)
        0)
    {
        if (!pFmt)
            return;
        CString fmtName = pFmt;
        if (fmtName.CompareNoCase(TM_T("HTML Format")) == 0 || fmtName.CompareNoCase(TM_T("text/html")) == 0)
            m_dwFmt = eCfHtml;
        else if (fmtName.CompareNoCase(TM_T("Rich Text Format")) == 0 || fmtName.CompareNoCase(TM_T("text/rtf")) == 0)
            m_dwFmt = eCfRtf;
        else if (fmtName.CompareNoCase(TM_T("image/png")) == 0)
            m_dwFmt = eCfPng;
        else if (fmtName.CompareNoCase(TM_T("image/jpeg")) == 0 || fmtName.CompareNoCase(TM_T("image/jpg")) == 0)
            m_dwFmt = eCfJpeg;
    }
#else
        ::RegisterClipboardFormat(pFmt)) {}
#endif
    operator WORD() const { return m_dwFmt; }
    bool operator == (const CDataFormat& rhs) const { return m_dwFmt == rhs.m_dwFmt; }
};
// Win-only forward declarations
#if !defined(TM_GTK)
class CDragImpl;
struct CDOHelper;
#endif
class CDragData
    : public CDragBase
    , public IDragSource
{
private:
    TM_NO_COPYABLE(CDragData);
public:
    CDragData()  { DragGetApp()->Init();  }
    ~CDragData() { DragGetApp()->Close(); }

    void AddMem(const void* pMem, size_t s, CDataFormat fmt, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = 1)
    {
        DragGetApp()->AddMem(pMem, s, fmt, dwAspect, lindex);
    }

    void AddHGlobal(HGLOBAL hGlb, CDataFormat fmt, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = 1)
    {
        DragGetApp()->AddGlobal(hGlb, fmt, dwAspect, lindex);
    }

    void AddText(TMCStrA pStr, CDataFormat fmt = e_cfText, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = 1)
    {
        DragGetApp()->AddTextA(pStr, fmt, dwAspect, lindex);
    }

    void AddText(TMCStrW pStr, CDataFormat fmt = e_cfUnicodeText, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = 1)
    {
        DragGetApp()->AddTextW(pStr, fmt, dwAspect, lindex);
    }

    // add ANSI and Unicode string at the same time
    void AddAUText(TMCStr pStr, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = 1)
    {
        DragGetApp()->AddTextAW(pStr, dwAspect, lindex);
    }

    void AddMetaFile(HENHMETAFILE hMeta, CDataFormat fmt = e_cfEMF, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = 1)
    {
        DragGetApp()->AddMeta(hMeta, fmt, dwAspect, lindex);
    }

    void AddStream(IStream* pStream, CDataFormat fmt, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = 1)
    {
        DragGetApp()->AddStream(pStream, fmt, dwAspect, lindex);
    }

    // double-NUL-terminated filename table
    void AddDragFiles(TMCStr pFiles, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = 1)
    {
        DragGetApp()->AddFiles(pFiles, dwAspect, lindex);
    }

    void AddDragFiles(TMCStr* ppFiles, size_t dwCount, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = 1)
    {
        DragGetApp()->AddFileArray(ppFiles, dwCount, dwAspect, lindex);
    }

    // SrcAlpha does not support clrKey
    void SetDragImage(TMPointI ptOffset, TMHDC hDC, const TMRectI& rc,
        TMUInt8 alpha = 0xff, TMBool bSrcAlpha = false, TMColor clrKey = cColorInvalid)
    {
        DragGetApp()->SetImage(ptOffset, hDC, rc, alpha, bSrcAlpha, clrKey);
    }

    TMBool ShowDragImage(TMBool bShow) { return DragGetApp()->ShowImage(bShow); }
    
    DWORD DoDrag(const void* key, EDragEff dwEffMask) { return DragGetApp()->DoDrag(key, this, dwEffMask); }

    static void StopDrag(const void* key) { DragGetApp()->Stop(key); }
};

class CDropData : public CDragBase
{
private:
    IDataObject* m_pData;
#if !defined(TM_GTK)
    // Win/GTK unified retrieval via DragGetApp helper methods.
    TMBool _GetData(void** ppv, CDataFormat fmt, DWORD dwAspect, LONG lindex, DWORD tymed)
    {
        return DropGetData(m_pData, ppv, fmt, dwAspect, lindex, tymed);
    }
#endif
public:
    CDropData(IDataObject* pData) : m_pData(pData) {}
    ~CDropData() {}
    IDataObject* GetIData() { return m_pData; }
    ///////////////////////////////////////////////////////////////////////////
    // Query functions
#if !defined(TM_GTK)
    // Win: query via FORMATETC
    TMBool Query(const FORMATETC& fmt)
    {
        DbgAssert(m_pData);
        HRESULT hr = m_pData->QueryGetData(const_cast<FORMATETC*>(&fmt));
        return S_OK == hr;
    }

    TMBool Query(CDataFormat fmt, EMedType eType = eMtHGlobal, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        FORMATETC ft = {fmt, 0, dwAspect, lindex, eType};
        return Query(ft);
    }

    TMBool QueryAUString(DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        return Query(e_cfText, eMtHGlobal, dwAspect, lindex)
            || Query(e_cfUnicodeText, eMtHGlobal, dwAspect, lindex);
    }
#else
    // GTK: query from wrapped GtkSelectionData payload
    TMBool Query(CDataFormat fmt, EMedType eType = eMtHGlobal, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        void* payload = NULL;
        return DropGetData(&payload, fmt, dwAspect, lindex, eType);
    }
    TMBool QueryAUString(DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        return Query(e_cfText, eMtHGlobal, dwAspect, lindex)
            || Query(e_cfUnicodeText, eMtHGlobal, dwAspect, lindex)
            || Query(e_cfHtml, eMtHGlobal, dwAspect, lindex)
            || Query(e_cfRtf, eMtHGlobal, dwAspect, lindex);
    }
#endif
    ///////////////////////////////////////////////////////////////////////////
    // Get functions -- callers release any handles returned
#if !defined(TM_GTK)
#   ifdef __ATLGDI_H__
    TMBool GetEMF(CEnhMetaFile& emf, CDataFormat fmt = e_cfEMF, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        DbgAssert(!emf);
        return _GetData((void**)&emf.m_hEMF, fmt, dwAspect, lindex, eMtEmf);
    }
    TMBool GetBitmap(CBitmap& bmp, CDataFormat fmt = e_cfBitmap, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        DbgAssert(!bmp);
        return _GetData((void**)&bmp.m_hBitmap, fmt, dwAspect, lindex, eMtEmf);
    }
#   endif // __ATLGDI_H__
    TMBool GetGlobal(CGlobalMem& gm, CDataFormat fmt, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        return _GetData((void**)&gm.m_hGlobal, fmt, dwAspect, lindex, eMtHGlobal);
    }
    TMBool GetStream(IStream** p, CDataFormat fmt, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        return _GetData((void**)p, fmt, dwAspect, lindex, eMtStream);
    }
    TMBool GetDrop(CDrop& drop, CDataFormat fmt = e_cfHDROP, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        DbgAssert(!drop);
        return _GetData((void**)&drop.m_hDrop, fmt, dwAspect, lindex, eMtHGlobal);
    }
    TMBool GetString(CStringA& str, CDataFormat fmt = e_cfText, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        return DropGetStringA(m_pData, &str, fmt, dwAspect, lindex);
    }
    TMBool GetString(CStringW& str, CDataFormat fmt = e_cfUnicodeText, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        return DropGetStringW(m_pData, &str, fmt, dwAspect, lindex);
    }
    // Returns the first valid string from e_cfUnicodeText or e_cfText
    TMBool GetAUString(CString& str, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        if (GetString(str))
            return true;
        TM_SW_UNICODE(CStringA, CStringW) strx;
        if (GetString(strx))
        {
            CString stry(strx);
            str.swap(stry);
            return true;
        }
        return false;
    }

//     TMBool IsDropTypeEqualTo(const CStrView& strType)
//     {
//         if (strType.IsEmpty())
//             return false;
//         static TMCStr pObjName[] =
//         {
//             0,
//             TM_T("CF_TEXT"),        TM_T("CF_BITMAP"),     TM_T("CF_METAFILEPICT"),
//             TM_T("CF_SYLK"),        TM_T("CF_DIF"),         TM_T("CF_TIFF"),
//             TM_T("CF_OEMTEXT"),     TM_T("CF_DIB"),         TM_T("CF_PALETTE"),
//             TM_T("CF_PENDATA"),     TM_T("CF_RIFF"),        TM_T("CF_WAVE"),
//             TM_T("CF_UNICODETEXT"), TM_T("CF_ENHMETAFILE"), TM_T("CF_HDROP"),
//             TM_T("CF_LOCALE"),      TM_T("CF_MAX"),
//         };
//         CComPtr<IEnumFORMATETC> ptrEnum;
//         m_pData->EnumFormatEtc(DATADIR_GET, &ptrEnum);
//         if (ptrEnum)
//         {
//             FORMATETC fmt;
//             ULONG l;
//             for (ptrEnum->Reset(); 0 == ptrEnum->Next(1, &fmt, &l);)
//             {
//                 TCHAR c[1024] = {0};
//                 if (fmt.cfFormat < CF_MAX)
//                 {
//                     if (strType.CompareNoCase(pObjName[fmt.cfFormat]) == 0)
//                         return true;
//                 }
//                 else
//                 {
//                     GetClipboardFormatName(fmt.cfFormat, c, 1024);
//                     if (strType.CompareNoCase(c) == 0)
//                         return true;
//                 }
//             }
//         }
//         return false;
//     }
#else // TM_GTK: read from internal GTK drop bundle cache
    TMBool GetGlobal(void* gm, CDataFormat fmt, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        tm_unused(gm);
        void* payload = NULL;
        return DropGetData(&payload, fmt, dwAspect, lindex, eMtHGlobal);
    }
    TMBool GetStream(IStream** p, CDataFormat fmt, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        tm_unused(p);
        tm_unused(fmt);
        tm_unused(dwAspect);
        tm_unused(lindex);
        return false;
    }
    TMBool GetDrop(void* drop, CDataFormat fmt = e_cfHDROP, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        tm_unused(drop);
        void* payload = NULL;
        return DropGetData(&payload, fmt, dwAspect, lindex, eMtHGlobal);
    }
    TMBool GetString(CStringA& str, CDataFormat fmt = e_cfText, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        if (DropGetStringA(&str, fmt, dwAspect, lindex))
            return true;
        str.Empty();
        return false;
    }
    TMBool GetString(CStringW& str, CDataFormat fmt = e_cfUnicodeText, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        if (DropGetStringW(&str, fmt, dwAspect, lindex))
            return true;
        str.Empty();
        return false;
    }
    TMBool GetAUString(CString& str, DWORD dwAspect = DVASPECT_CONTENT, LONG lindex = -1)
    {
        if (GetString(str, e_cfUnicodeText, dwAspect, lindex))
            return true;
        TM_SW_UNICODE(CStringA, CStringW) strx;
        if (GetString(strx, e_cfText, dwAspect, lindex)
            || GetString(strx, e_cfHtml, dwAspect, lindex)
            || GetString(strx, e_cfRtf, dwAspect, lindex))
        {
            CString stry(strx);
            str.swap(stry);
            return true;
        }
        str.Empty();
        return false;
    }
//     BOOL IsDropTypeEqualTo(const CStrView& strType)
//     {
//         if (strType.IsEmpty())
//             return FALSE;
//         CDataFormat fmt((TMCStr)strType);
//         if (fmt == 0)
//             return FALSE;
//         return Query(fmt) ? TRUE : FALSE;
//     }
#endif // CDropData platform methods
};
TM_END_NAMESPACE

#endif // TMDRAG_H