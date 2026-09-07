/**
 *  @file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.11.10
 *
 *  Copyright (C) 2015 miragekiller                               
 */

#pragma once

#ifndef GXFONT_H
#define GXFONT_H

#include "../tmgdix.h"
#include "../tmautoptr.h"
#include "../tmlist.h"


//#define GF_TRACE DbgTrace
#define GF_TRACE DbgTraceNull

struct TMMetric
{
    int tmAveCharWidth;
    int tmHeight;
};


TM_BGN_NAMESPACE

class CGXAppBase;

///////////////////////////////////////////////////////////////////////////////
// CGXTextFmt

// for rich
const TMUInt DT_DIRTY               = 0x10000000;   // need rewrap
const TMUInt DT_DIRTYPART           = 0x20000000;   // some line need rewrap

struct CGXTextFmt 
{
    TMUInt                  m_flag;

    CGXTextFmt(TMUInt flag)
        : m_flag (flag)
    {
    }

    // for text and rich
    TM_DECLARE_FLAG     (FlagR2L,               m_flag, DT_RTLREADING);
    TM_DECLARE_FLAG     (FlagSingleLine,        m_flag, DT_SINGLELINE);
    TM_DECLARE_FLAG     (FlagRight,             m_flag, DT_RIGHT);
    TM_DECLARE_FLAG     (FlagCenter,            m_flag, DT_CENTER);
    TM_DECLARE_FLAG     (FlagWordBreak,         m_flag, DT_WORDBREAK);
    TM_DECLARE_FLAG     (_DT_STATE_ELLIPSIS,    m_flag, DT_STATE_ELLIPSIS);
    TM_DECLARE_FLAG_GET (_DT_RTLREADING,        m_flag, DT_RTLREADING);
    
    // for rich
    TM_DECLARE_FLAG     (FlagDirty,             m_flag, DT_DIRTY);
    TM_DECLARE_FLAG     (FlagDirtyPart,         m_flag, DT_DIRTYPART);
    TM_DECLARE_NOFLAG   (FlagUnderLink,         m_flag, DT_NOLINKUNDERLINE);

    template <class T_App> bool IsNeedRTL(T_App* app) const
    {
        return !(m_flag & DT_L2RREADING) && app->IsRTL();
    }

protected:
    TMBool IsUseMaxWidth() const
    {
        return (m_flag & DT_WORDBREAK) || (((m_flag & (DT_WORD_ELLIPSIS | DT_PATH_ELLIPSIS | DT_END_ELLIPSIS))));
    }
    TMBool IsUseMaxHeight() const
    {
        return (m_flag & (DT_EDITCONTROL | DT_END_ELLIPSIS));
    }

    TMBool IsAutoWrapRich() const
    {
        return DT_WORDBREAK == (m_flag & (DT_WORDBREAK | DT_WORD_ELLIPSIS));
    }

//     TMBool IsWordEllipsis() const
//     {
//         return DT_WORD_ELLIPSIS == (m_flag & (DT_WORDBREAK | DT_WORD_ELLIPSIS));
//     }

public:
    template <class T> void MakeEllipsisSize(TMBool bCalcEllipsis, T& sz, const T& szReal)
    {
        Reset_DT_STATE_ELLIPSIS();
        if (!bCalcEllipsis || !IsUseMaxWidth() || sz.Width() > szReal.Width())
            sz.SetWidth(szReal.Width());
        else if (sz.Width() < szReal.Width())
            Set_DT_STATE_ELLIPSIS();

        if (!bCalcEllipsis || !IsUseMaxHeight() || sz.Height() > szReal.Height())
            sz.SetHeight(szReal.Height());
        else if (IsUseMaxHeight() && sz.Height() < szReal.Height())
            Set_DT_STATE_ELLIPSIS();
    }
};


///////////////////////////////////////////////////////////////////////////////
//ITextCache
class ITextCache : public IUnknown
{
public:
    TM_BGN_INTERFACE_MAP(IUnknown)
    TM_END_INTERFACE_MAP()
    
    virtual int GetWidth() = 0;
    virtual int GetHeight() = 0;
    virtual TMBool CacheTestApp(IGXApp* app) = 0;
};
typedef CRefPtr<ITextCache> CTextCache;


///////////////////////////////////////////////////////////////////////////////
//CGXFont
struct CGXFont
{
public:
    GXFont m_ft;
    int m_dpi;
    TMAtmInt m_nRef;
    
    bool operator < (const CGXFont& rhs) const
    {
        DbgAssert(m_ft.styleMask == GX_FONT_MASK_STYLE);
        DbgAssert(rhs.m_ft.styleMask == GX_FONT_MASK_STYLE);
        if (m_dpi != rhs.m_dpi)
            return m_dpi < rhs.m_dpi;
        if (m_ft.u == rhs.m_ft.u)
            return m_ft._namecomp(rhs.m_ft.name) < 0;
        else
            return m_ft.u < rhs.m_ft.u;
    }
    
    CGXFont()
        : m_nRef(0)
    {
    }
    
    ~CGXFont()
    {
    }
    
    CZoom GetDPI() const { return m_dpi; }
    
    const GXFont& GetKey()
    {
        return m_ft;
    }
    
    TM_REFAPI AddRef()
    {
        DbgAssert(m_nRef >= 0);
        return AtomicInc(&m_nRef);
    }

    virtual TM_REFAPI Release() = 0;

    virtual EGXDevice FontGetDeviceType() = 0;

    virtual TMMetric& FontGetMetrics() = 0;

    virtual CRefPtr<CGXFont> CloneFont(const GXFont& ft, int nDpi) = 0;

    // for text
    virtual void FontCalcRect(const CGXStrView& str, TMSizeI& sz, TMUInt fmt) = 0;

    // for rich
    virtual void FontGetExtentPoint(const CGXStrView& str, int nMaxExtent, int* lpnFit, int* lpnDx, TMSize& sz) = 0;
    
    // if ptOrg then blt to device; else only make cache.
    // if no cache then create cache.
    // if no szMax only blt use cache.
    // if fmtMdf set DT_STATE_ELLIPSIS flag.
    // if rcReal set rcReal = real text paint rect
    virtual void FontRenderText(CGXAppBase* app, const CGXStrView& str, TMUInt fmt, TMColor clr, const GXPoint* ptOrg, const GXSize* szMax, CTextCache& cache, CGXTextFmt* fmtMdf, GXRect* rcReal = 0) = 0;

    static void FontGetDefault(GXFont& ft);
    static CRefPtr<CGXFont> FontCreateDefaultFromSys();
#ifdef TM_WIN
    static CRefPtr<CGXFont> FontCreateDefaultFromDWrite();
#endif
    static CRefPtr<CGXFont> FontCreateDefaultAuto(EGXDevice eDev)
    {
#ifdef TM_WIN
        if (eDev == eGXTxtDevDWrite)
        {
            CRefPtr<CGXFont> ft(FontCreateDefaultFromDWrite());
            if (ft)
                return ft;
        }
#endif
        return FontCreateDefaultFromSys();
    }
};


///////////////////////////////////////////////////////////////////////////////
// helper
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//CTextCacheTexT
template <class T_Tex> // T_Tex: IGXTextureImpl
struct CTextCacheTexT
    : public CBaseRefT<CTextCacheTexT<T_Tex>, ITextCache>
    , public TM::CRefObjectRoot<1>
{
public:
    TM_SELF_ALLOC(CTextCacheTexT);
    CRefPtr<T_Tex>  m_imgText;
    
public:
    TMBool CacheTestApp(IGXApp* app) { return app == m_imgText->GetApp(); }
    int GetWidth() { return m_imgText->GetWidth(); }
    int GetHeight() { return m_imgText->GetHeight(); }
};


///////////////////////////////////////////////////////////////////////////////
// CGXFontImplT
template <class T>
struct TM_NO_VTABLE CGXFontImplT : public CGXFont
{
    TM_CLASS_INSTANCE_CHECK(T);

    TM_REFAPI Release()
    {
        DbgAssert(m_nRef >= 0);
        TMAtmInt x = AtomicDec(&m_nRef);
        if (x <= 0)
        {
            typename T::CGXFontFactory* p = T::CGXFontFactory::GetInstanceNoAddRef();
            DbgAssert(p);
            p->erase(*static_cast<T*>(this));
            p->Release();
        }
        return x;
    }

    virtual CRefPtr<CGXFont> CloneFont(const GXFont& ft, int nDpi) tm_override
    {
        return CreateFont(ft, nDpi);
    }

    static CRefPtr<CGXFont> CreateFont(const GXFont& ft, int nDpi)
    {
        DbgAssert((ft.styleMask & GX_FONT_MASK_STYLE) == GX_FONT_MASK_STYLE);

        typedef typename T::CGXFontFactory list_type;
        TM::CRefPtr<list_type> ptr;
        list_type::CreateInstance(&ptr);
        if (!ptr->IsValid())
            return 0;

        T ftk;
        ftk.m_ft = ft;
        ftk.m_dpi = nDpi;
        typename list_type::recorder rec(*ptr);
        rec.find(ftk);
        if (rec)
            return const_cast<T*>(&rec.get_data());
        
        T& pf = const_cast<T&>(*(ptr->insert(ftk).first));
        DbgAssert(pf.m_nRef == 0);
        { list_type* p = ptr; p->AddRef(); }
        pf.MakeFont();
        return &pf;
    }
};


struct CFontAppBase
{
    TM::CRefPtr<CGXFont>            m_font;

    CZoom GetDPI()
    {
        DbgAssert(m_font);
        return m_font->m_dpi;
    }

    void SelectFont(const GXFont& fontkey, int nDPI)
    {
        TM::CRefPtr<CGXFont> ptr(m_font->CloneFont(fontkey, nDPI));
        if (m_font != ptr)
            m_font = ptr;
    }

    void FontSetCurrent(const GXFont& ft)
    {
        GXFont fontkey = m_font->GetKey();
        TMBool bReleaseFont = fontkey.ReplaceFromEx(ft);
        if (bReleaseFont)
            SelectFont(fontkey, GetDPI());
    }
};


TM_END_NAMESPACE



#endif // GXFONT_H
