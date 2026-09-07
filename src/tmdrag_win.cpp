/*
 *  @file
 *  @brief  drag drop
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.02.18
 *
 *  Copyright (C) 2009 miragekiller
 */

#include "tmdrag.h"
#include "tmcom.h"
#include "tmdll.h"
#include "tmgdi.h"
#include "tmplatform_win.h"
#include "tmmouse.h"
#include <shlobj.h>


///////////////////////////////////////////////////////////////////////////////
// TM_DragImpl
class TM_DragImpl
    : public IDataObject
    , public IDropSource
    , public TM::IDragApp
{
public:
    TM::IDragSource*    m_source;
    STGMEDIUM*          m_pStg;
    FORMATETC*          m_pFmt;
    int                 m_nSize;
    POINT               m_ptOffset;
    HWND                m_hDrag;
    const void*         m_pKey;

private:
    int Lookup(const FORMATETC* lpFmt)
    {
        // check for match
        // we threat lindex == 0 and lindex == -1 as equivalent
        for (int i = 0; i < m_nSize; ++i)
        {
            FORMATETC& fmt = m_pFmt[i];

            if ((fmt.cfFormat == lpFmt->cfFormat) &&
                ((fmt.tymed & lpFmt->tymed) != 0) &&
//                 (m_obj.m_pStg[i].tymed == TYMED_NULL ||
//                 fmt.lindex == lpFmt->lindex) &&
                (fmt.dwAspect == lpFmt->dwAspect))
            {
                return i;
            }
        }

        return -1;
    }

    void CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, CLIPFORMAT cfFormat)
    {
        switch (pMedSrc->tymed)
        {
        case TYMED_HGLOBAL:
        case TYMED_GDI:
        case TYMED_MFPICT:
        case TYMED_ENHMF:
        case TYMED_FILE:
            pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal, cfFormat, NULL);
            break;

        case TYMED_ISTREAM:
            pMedDest->pstm = pMedSrc->pstm;
            pMedSrc->pstm->AddRef();
            break;

        case TYMED_ISTORAGE:
            pMedDest->pstg = pMedSrc->pstg;
            pMedSrc->pstg->AddRef();
            break;

        case TYMED_NULL:
        default:
            break;
        }

        pMedDest->tymed = pMedSrc->tymed;
        pMedDest->pUnkForRelease = NULL;
        if (pMedSrc->pUnkForRelease != NULL)
        {
            pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;
            pMedSrc->pUnkForRelease->AddRef();
        }
    }

public:
    TM_UNUSED_ADDREF_RELEASE    ();
    TM_BGN_INTERFACE_MAP        (IDataObject)
        TM_INTERFACE_HANDLER    (IDataObject)
        TM_INTERFACE_HANDLER    (IDropSource)
    TM_END_INTERFACE_MAP        ()

    //////////////////////////////////////////////////////////////////////////
    // IDataObject
    STDMETHODIMP GetData(LPFORMATETC lpFmt, LPSTGMEDIUM lpStgMedium)
    {
        DbgAssert(lpFmt);
        return TryGetData(lpFmt, lpStgMedium) ? S_OK : DATA_E_FORMATETC;
    }

    STDMETHODIMP GetDataHere(LPFORMATETC lpFmt, LPSTGMEDIUM lpStgMedium)
    {
        DbgTrace ("CDragData::GetDataHere");
        return E_NOTIMPL;
    }

    STDMETHODIMP QueryGetData(LPFORMATETC lpFmt)
    {
        DbgAssert(lpFmt);
        return Lookup(lpFmt) >= 0 ? S_OK : DATA_E_FORMATETC;
    }

    STDMETHODIMP GetCanonicalFormatEtc(LPFORMATETC, LPFORMATETC)
    {
        return DATA_S_SAMEFORMATETC;
    }

    STDMETHODIMP SetData(LPFORMATETC lpFmt, LPSTGMEDIUM lpStgMedium, BOOL bRelease)
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP EnumFormatEtc(DWORD dwDirection, LPENUMFORMATETC* ppenumFormatEtc)
    {
        if (DATADIR_SET == dwDirection)
        {
            return E_NOTIMPL;
        }

        typedef CComEnum<IEnumFORMATETC, &IID_IEnumFORMATETC, FORMATETC, _Copy<FORMATETC> > CEnum;

        if (ppenumFormatEtc)
        {
            CComObject<CEnum>* p = 0;
            CComObject<CEnum>::CreateInstance(&p);
            if (p)
            {
                p->Init(m_pFmt, m_pFmt + m_nSize, 0, AtlFlagNoCopy);
                (*ppenumFormatEtc) = p;
                p->AddRef();
            }
            return S_OK;
        }

        return E_FAIL;
    }

    STDMETHODIMP DAdvise(
        FORMATETC* /*pFormatetc*/, DWORD /*advf*/,
        LPADVISESINK /*pAdvSink*/, DWORD* pdwConnection)
    {
        *pdwConnection = 0;
        return OLE_E_ADVISENOTSUPPORTED;
    }

    STDMETHODIMP DUnadvise(DWORD /*dwConnection*/)
    {
        return OLE_E_ADVISENOTSUPPORTED;
    }

    STDMETHODIMP EnumDAdvise(LPENUMSTATDATA* ppenumAdvise)
    {
        *ppenumAdvise = NULL;
        return OLE_E_ADVISENOTSUPPORTED;
    }

    //////////////////////////////////////////////////////////////////////////
    // IDropSource
    STDMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
    {
        //unlock window and allow updates to occur
        //     ImageList_DragLeave(NULL) ;

        if (fEscapePressed || !m_pKey)
        {
            return DRAGDROP_S_CANCEL;
        }

        if (!(grfKeyState & (MK_LBUTTON|MK_RBUTTON)))
        {
            return DRAGDROP_S_DROP;
        }

        TM::CPoint pt(TM::CMouse::GetCursorPos());
        if (m_hDrag)
        {
            SetWindowPos(m_hDrag, NULL,
                pt.x - m_ptOffset.x, pt.y - m_ptOffset.y, 0, 0,
                SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
        }

        return (m_source && m_source->OnDraging(pt, grfKeyState)) ? S_OK : DRAGDROP_S_CANCEL;
    }

    STDMETHODIMP GiveFeedback(DWORD dwEffect)
    {
        if (m_source)
            m_source->OnGiveFeedback((TM::EDragEff)dwEffect);
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }

    ///////////////////////////////////////////////////////////////////////////
    // helper function
    void Append(int n)
    {
        n += m_nSize;
        int nAlloc = ((n / 4) + 1) * 4;
        m_pStg = (STGMEDIUM*)MemRealloc(m_pStg, nAlloc * sizeof(STGMEDIUM), TM_DragImpl);
        m_pFmt = (FORMATETC*)MemRealloc(m_pFmt, nAlloc * sizeof(FORMATETC), TM_DragImpl);
        m_nSize  = n;
    }

    static void SetFmt(FORMATETC& fmt, WORD cFmt, DWORD dwAspect, LONG lindex, DWORD tymed)
    {
        fmt.cfFormat = cFmt;
        fmt.ptd = 0;
        fmt.dwAspect = dwAspect;
        fmt.lindex = lindex;
        fmt.tymed = tymed;
    }

    static void SetStg(STGMEDIUM& stg, HANDLE hMem, DWORD tymed)
    {
        stg.hGlobal = hMem;
        stg.tymed = tymed;
        stg.pUnkForRelease = 0;
    }

    static void MemToGloal(LPCVOID pMem, size_t s, STGMEDIUM& stg)
    {
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, s);
        DbgAssert(hMem);
        LPVOID p = GlobalLock(hMem);
        MemCpy(p, pMem, s);
        GlobalUnlock(hMem);
        SetStg(stg, hMem, TYMED_HGLOBAL);
    }

    void AddObj(TM::CDataFormat fmt, DWORD dwAspect, LONG lindex, HANDLE hObj, DWORD tmd)
    {
        int n = m_nSize;
        Append(1);
        SetFmt(m_pFmt[n], fmt, dwAspect, lindex, tmd);
        SetStg(m_pStg[n], hObj, tmd);
    }

    static HWND CreateDragWindow()
    {
        HWND hWnd = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            TM_T("static"), TM_DRAG_WIN_TEXT, WS_POPUP | WS_MINIMIZE,
            0, 0, 0, 0, NULL, NULL, 0, NULL);
        ShowWindow(hWnd, SW_SHOWNOACTIVATE);
        return hWnd;
    }

    static void UpdateLayedWin(HWND hWnd, HDC hDC, const RECT& rc, COLORREF clrKey, BYTE alpha, BYTE AlphaFormat, DWORD dwFlags)
    {
        BLENDFUNCTION blend;
        blend.BlendOp = AC_SRC_OVER;
        blend.BlendFlags = 0;
        blend.SourceConstantAlpha = alpha /*DRAGDROP_ALPHA*/;
        blend.AlphaFormat = AlphaFormat;

        HDC hdc = GetDC(hWnd);
        POINT ptSrc = {rc.left, rc.top};
        POINT ptDst = { -10000, 0 };
        SIZE szSrc = {rc.right - rc.left, rc.bottom - rc.top};
        UpdateLayeredWindow(hWnd, hdc, &ptDst, &szSrc, hDC, &ptSrc, clrKey, &blend, dwFlags);
        ReleaseDC(hWnd, hdc);
    }

    template <class T>
    static TMBool _CDropData_GetString(IDataObject* data, T& str, TM::CDataFormat fmt,
        DWORD dwAspect, LONG lindex)
    {
        TM::CGlobalMem gm;
        TM::CDropData dd(data);
        if (dd.GetGlobal(gm, fmt, dwAspect, lindex) && gm)
        {
            size_t nSize = gm.GlobalSize();
            if (nSize != 0)
            {
                typedef typename T::value_type value_type;
                TM::CStringT<value_type> cstr;
                nSize -= sizeof(value_type);
                int nCh = nSize / sizeof(value_type);
                gm.CopyTo(cstr.GetBuffer(nCh), nSize);
                str.swap(cstr);
                return true;
            }
        }

        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    // IDragApp
    virtual void Init() tm_override
    {
        DbgAssert(m_pStg == 0);
        DbgAssert(m_pFmt == 0);
        DbgAssert(m_nSize == 0);
    }

    virtual void Close() tm_override
    {
        for (int i = 0; i < m_nSize; ++i)
            ReleaseStgMedium(m_pStg + i);
        
        if (m_hDrag)
            DestroyWindow(m_hDrag);
        
        if (m_pStg)
        {
            MemFree(m_pStg);
            m_pStg = 0;
        }
        if (m_pFmt)
        {
            MemFree(m_pFmt);
            m_pFmt = 0;
        }
        
        m_nSize = 0;
        m_hDrag = 0;
        m_pKey = 0;
    }

    virtual void SetImage(TMPointI ptOffset, TMHDC hDC, const TMRectI& rc, BYTE alpha, TMBool bSrcAlpha, TMColor clrKey) tm_override
    {
        if (!m_hDrag)
            m_hDrag = CreateDragWindow();
        m_ptOffset = ptOffset;
        DWORD dwFlag;
        if (clrKey == cColorInvalid)
            dwFlag = 0;
        else
        {
            dwFlag = ULW_COLORKEY;
            clrKey = ColorToCOLORREF(clrKey);
        }
        UpdateLayedWin(m_hDrag, hDC, rc, clrKey, alpha, bSrcAlpha ? AC_SRC_ALPHA : 0, dwFlag | ULW_ALPHA);
    }
    
    virtual TMBool ShowImage(TMBool bShow) tm_override
    {
        if (!m_hDrag)
            return false;
        return ShowWindow(m_hDrag, bShow ? SW_SHOWNOACTIVATE : SW_HIDE);
    }
    
    virtual DWORD DoDrag(const void* key, TM::IDragSource* source, TM::EDragEff dwEffMask) tm_override
    {
        DbgAssert(!m_pKey);
        m_pKey = key;
        m_source = source;
        DWORD dwEffRet = 0;
        DoDragDrop(this, this, dwEffMask, &dwEffRet);
        return dwEffRet;
    }
    
    virtual TMBool Stop(const void* key) tm_override
    {
        if (key != m_pKey)
            return false;
        m_pKey = 0;
        return true;
    }

    virtual void AddMem(LPCVOID pMem, size_t s, WORD fmt, DWORD dwAspect, LONG lindex) tm_override
    {
        int n = m_nSize;
        Append(1);
        MemToGloal(pMem, s, m_pStg[n]);
        SetFmt(m_pFmt[n], fmt, dwAspect, lindex, TM::eMtHGlobal);
    }

    virtual void AddGlobal(HGLOBAL hGlb, WORD fmt, DWORD dwAspect, LONG lindex) tm_override
    {
        AddObj(fmt, dwAspect, lindex, hGlb, TM::eMtHGlobal);
    }

    virtual void AddTextA(LPCSTR pStr, WORD fmt, DWORD dwAspect, LONG lindex) tm_override
    {
        if (pStr == 0)
            pStr = "";
        int n = StrLen(pStr) + 1;
        AddMem(pStr, n, fmt, dwAspect, lindex);
    }

    virtual void AddTextW(LPCWSTR pStr, WORD fmt, DWORD dwAspect, LONG lindex) tm_override
    {
        if (pStr == 0)
            pStr = L"";
        int n = StrLen(pStr) + 1;
        AddMem(pStr, n * sizeof(*pStr), fmt, dwAspect, lindex);
    }

    virtual void AddTextAW(LPCTSTR pStr, DWORD dwAspect, LONG lindex) tm_override
    {
        AddTextA(TM_T2A(pStr), TM::CDragBase::e_cfText, dwAspect, lindex);
        AddTextW(TM_T2W(pStr), TM::CDragBase::e_cfUnicodeText, dwAspect, lindex);
    }

    virtual void AddMeta(HENHMETAFILE hMeta, WORD fmt, DWORD dwAspect, LONG lindex) tm_override
    {
        AddObj(fmt, dwAspect, lindex, hMeta, TM::eMtEmf);
    }

    virtual void AddStream(IStream* pStream, WORD fmt, DWORD dwAspect, LONG lindex) tm_override
    {
        AddObj(fmt, dwAspect, lindex, pStream, TM::eMtStream);
    }

    virtual void AddFiles(LPCTSTR pFiles, DWORD dwAspect, LONG lindex) tm_override
    {
        DbgAssert(pFiles);
        
        // get file list mem size
        int n = 0;
        for (; pFiles[n] || pFiles[n+1]; ++n)
        {
        }
        n = (2 + n) * sizeof(*pFiles);
        
        // create mem
        TM::CGlobalMem gm;
        gm.GlobalAlloc(sizeof(DROPFILES) + n);
        DROPFILES* p = (DROPFILES*)gm.GlobalLock();
        p->pFiles = sizeof(DROPFILES);
        p->fWide = TM_SW_UNICODE(true, false);
        p->fNC = 0;
        p->pt.x = 0;
        p->pt.y = 0;
        MemCpy(p + 1, pFiles, n);
        gm.GlobalUnlock();
        
        // add object
        AddObj(TM::CDragBase::e_cfHDROP, dwAspect, lindex, gm.Detach(), TM::eMtHGlobal);
    }

    virtual void AddFileArray(LPCTSTR* ppFiles, size_t dwCount, DWORD dwAspect, LONG lindex) tm_override
    {
        // check size
        int* pSize = (int*)_alloca(sizeof(int) * dwCount);
        int nSize = 0;
        size_t dwNum = 0;
        for (; ppFiles[dwNum] && dwNum < dwCount; ++dwNum)
        {
            int x = StrLen(ppFiles[dwNum]) + 1;
            pSize[dwNum] = x;
            nSize += x;
        }
        
        // create mem
        TM::CGlobalMem gm;
        gm.GlobalAlloc (sizeof(DROPFILES) + (nSize + 1) * sizeof(**ppFiles));
        DROPFILES* p = (DROPFILES*) gm.GlobalLock();
        p->pFiles = sizeof(DROPFILES);
        p->fWide = TM_SW_UNICODE(true, false);
        p->fNC = 0;
        p->pt.x = 0;
        p->pt.y = 0;
        
        // copy mem
        LPTSTR pByte = (LPTSTR)(p + 1);
        for (size_t i = 0; i < dwNum; ++i)
        {
            MemCpy(pByte, ppFiles[i], sizeof(*pByte) * pSize[i]);
            pByte += pSize[i];
        }
        pByte[nSize] = 0; // end byte is "\0\0"
        
        gm.GlobalUnlock();
        
        // add object
        AddObj(TM::CDragBase::e_cfHDROP, dwAspect, lindex, gm.Detach(), TM::eMtHGlobal);
    }

    virtual bool TryGetData(const FORMATETC* lpFmt, LPSTGMEDIUM lpStgMedium) tm_final
    {
        int n = Lookup(lpFmt);
        if (n >= 0)
        {
            CopyMedium(lpStgMedium, &m_pStg[n], lpFmt->cfFormat);
            return true;
        }
        return false;
    }

};

static TM_DragImpl s_ddImpl;


///////////////////////////////////////////////////////////////////////////////
// CDragData

TM::IDragApp* DragGetApp()
{
    return &s_ddImpl;
}

TMBool DropGetData(IDataObject* data, void** ppv, WORD fmt, DWORD dwAspect, LONG lindex, DWORD tymed)
{
    DbgAssert(data);
    DbgAssert(ppv);

    FORMATETC ft = {fmt, 0, dwAspect, lindex, tymed};
    STGMEDIUM stg = {0};
    if (S_OK == data->GetData (&ft, &stg))
    {
        // restrict caller to manage object handle so do not need to release stg;
        *ppv = stg.hGlobal;
        return true;
    }

    return false;
}

TMBool DropGetStringA(IDataObject* data, TM::CStringA* str, WORD fmt, DWORD dwAspect, LONG lindex)
{
    return TM_DragImpl::_CDropData_GetString(data, *str, fmt, dwAspect, lindex);
}

TMBool DropGetStringW(IDataObject* data, TM::CStringW* str, WORD fmt, DWORD dwAspect, LONG lindex)
{
    return TM_DragImpl::_CDropData_GetString(data, *str, fmt, dwAspect, lindex);
}

