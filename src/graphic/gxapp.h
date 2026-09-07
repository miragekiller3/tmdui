/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.13
 *
 *  Copyright (C) 2011 miragekiller
 */

#pragma once

#ifndef GXAPP_H
#define GXAPP_H

#include "../tmgdix.h"
#include "../tmfile.h"
#include "../tmlocal.h"
#include "../tmlist.h"
#include "../tmautoptr.h"
#include "../tmani.h"
#include "../tmthread.h"
#include "../gximage.h"
#include "gxfont.h"


TM_BGN_NAMESPACE

#ifndef GX_TRACE
// #    define GX_TRACE DbgTrace
#   define GX_TRACE DbgTraceNull
#endif


struct CGXFont;
class CGXAppBase;
typedef TMUInt32            GXIndices;
typedef TM::CStrPtrW        CGXStrPtr;


///////////////////////////////////////////////////////////////////////////////
// CGXDeviceLock
class CGXDeviceLockNull
{
public:
    void LockDevice() {}
    void UnlockDevice() {}
};

//class CGXDeviceLock
//{
//private:
//    TM::CCritical                   m_critical;
//
//public:
//    void LockDevice() { m_critical.Lock(); }
//    void UnlockDevice() { m_critical.Unlock(); }
//
//    void _gx_check_lock_tid()
//    {
//        DbgAssert(ThreadSelfId() == (TMThreadID)(((TMCritical*)&m_critical)->OwningThread));
//    }
//};

template <class T>
class CGXDeviceAutoLockT : public CNoCopyable
{
protected:
    T* m_pObj;

public:
    CGXDeviceAutoLockT(T* p) : m_pObj(p) { m_pObj->LockDevice(); }
    ~CGXDeviceAutoLockT() { m_pObj->UnlockDevice(); }
};


///////////////////////////////////////////////////////////////////////////////
// IGXSurfaceImpl
class TM_NO_VTABLE IGXSurfaceImpl : public IGXSurface
{
public:
    TM_BGN_INTERFACE_MAP(IGXSurface)
    TM_END_INTERFACE_MAP()
};

///////////////////////////////////////////////////////////////////////////////
// IGXTextureImpl
#define DBG_FILL_COLOR TMRGB(0xff, 0, 0xff)
#ifdef TM_DBG
inline void DBG_INIT(HIMAGE img)  { if (img) ImageFill(img, TMRGB(0xff, 0, 0xff), 0); }
template <class T> inline IGXTexture*  DBG_INIT(T* texture, ...) { if (texture) texture->ColorFill(TMRGB(0xff, 0, 0xff)); return texture; }
#else
#   define DBG_INIT(texture)  texture
#endif

enum ETextureType { eTextureTypeTexture, eTextureTypeRenderTarget, eTextureTypeSwapChain, eTextureTypeOffscreen };

class TM_NO_VTABLE IGXTextureImpl : public IGXTexture
{
public:
    TM_BGN_INTERFACE_MAP(IGXTexture)
    TM_END_INTERFACE_MAP()

//     TMBool ResetAsDC(TMHDC hdc);
    TMBool ResetAsDib(const GXDibData& data);
    TMBool ReplaceByImage(HIMAGE hImage);
    EGXRet PresentLayeredWin(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx);
    TMColor Check_GX_BLT_TEXTMONO(TMUInt32 uFlag, TMUInt8 alpha, CGXAppBase* app);
    void    NativeColorFill(TMColor clr);
    
    // IGXTexture
    virtual void    OptimizeForTile() tm_override {}
    virtual TMBool  SaveFile(TMCStr pFile, TMBool bHaveAlpha) tm_override;
    virtual int     GetWidth() tm_override { return 0; }
    virtual int     GetHeight() tm_override { return 0; }
    virtual EGXFormat     GetFormat() tm_override { return eGXFormat32A; }
    virtual void    Blt(const GXRect& rcDst, const TMRectD& rcSrc, unsigned int uFlag, TMUInt8 alpha = 0xff) tm_override { DbgAssert(0); }
    virtual void    Blts(const GXRect* rcDsts, const TMRectD* rcSrcs, int nCount, unsigned int uFlag, TMUInt8 alpha = 0xff) tm_override { DbgAssert(0); }
    virtual EGXRet  Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx) tm_override { DbgAssert(0); return eGXRetFail; }

    // self
//     virtual TMAtmInt GetRef() const = 0;
    virtual void    OnSetRenderTarget(CGXAppBase* _app) {}
    virtual void    OnResetRenderTarget(CGXAppBase* _app, GXPTexture pNew) {}
    virtual void    BltText(const GXRect& rcDst, unsigned int uFlag, TMColor clr) { DbgAssert(0); } // = 0;
};

#define  CTexture_UpdateLayeredWindowDC     IGXTextureImpl
#define CTexture_UpdateLayeredWindowBuffer  IGXTextureImpl


////////////////////////////////////////////////////////////////////////////////
// CTexture_UpdateLayeredWindowDC && CTexture_UpdateLayeredWindowBuffer
// struct TM_NO_VTABLE CTexture_UpdateLayeredWindowDC : public IGXTextureImpl
// {
//     EGXRet PresentLayeredWin(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx);
// };
// #define CTexture_UpdateLayeredWindowBuffer CTexture_UpdateLayeredWindowDC

// struct TM_NO_VTABLE CTexture_UpdateLayeredWindowBuffer : public IGXTextureImpl
// {
//     EGXRet PresentLayeredWin(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx);
// };

////////////////////////////////////////////////////////////////////////////////
// CTextureImage
class TM_NO_VTABLE CTextureImage
    : public CTexture_UpdateLayeredWindowDC
    , public IGXSurface
{
public:
    CImage m_img;
    TMColor m_clrText; // for text render.

    TM_UNUSED_IC_GetDebugInfo();

public:
    TM_BGN_INTERFACE_MAP(IGXTexture)
    TM_END_INTERFACE_MAP()

    CTextureImage();
    ~CTextureImage();
    
    static GXPTexture CreateInst(HIMAGE img);
//     TMBool ReplaceByImage(HIMAGE hImage);
    void Check_GX_BLT_TEXTMONO(TMUInt32 uFlag, TMUInt8& alpha, CGXAppBase* app);
    void Check_GX_BLT_TEXTMONO(TMUInt32 uFlag, TMColor clr);
    void BltToDC(TMHDC hdc, const TMRectD& rcDst, const TMRectD& rcSrc, TMUInt32 uFlag, TMUInt8 alpha = 0xff);

    // IGXTexture
    virtual IGXApp* GetApp() tm_override;
    virtual void*   GetNativeHandle() tm_override { return m_img.GetBitmap(); }
    virtual void    GetSurface(IGXSurface** ppSurface) tm_override;

    virtual TMBool  Resize(int w, int h);
    virtual TMBool  ResetAsObject(const void* pData, EGXReset eReset) tm_override;
    virtual void    OptimizeForTile() tm_override;
    virtual TMBool  SaveFile(TMCStr pFile, TMBool bHaveAlpha) tm_override;
    
    virtual int     GetWidth() tm_override;
    virtual int     GetHeight() tm_override;
    virtual EGXFormat GetFormat() tm_override;
    virtual void    Blt(const GXRect& rcDst, const TMRectD& rcSrc, TMUInt32 uFlag, TMUInt8 alpha) tm_override;
    virtual void    Blts(const GXRect* rcDsts, const TMRectD* rcSrcs, int nCount, TMUInt32 uFlag, TMUInt8 alpha = 0xff) tm_override;
    virtual EGXRet  Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx) tm_override;

    // IGXSurface
    virtual void    LockRect(TMDibBuff& dib, const TMRect* rc, EGXLockRect flag) tm_override;
    virtual void    UnlockRect() tm_override {};
    virtual TMHDC   GetDC(TMBool bDiscard) tm_override;
    virtual void    ReleaseDC(TMHDC hdc) tm_override;
//     virtual void    ToTexture(IGXTexture** ppTex) tm_override;

    // IGXTextureImpl
    virtual void    OnSetRenderTarget(CGXAppBase* _app) tm_override;
    virtual void    OnResetRenderTarget(CGXAppBase* _app, GXPTexture pNew) tm_override;
    virtual void    BltText(const GXRect& rcDst, TMUInt32 uFlag, TMColor clr) tm_override;
};



///////////////////////////////////////////////////////////////////////////////
// CGXAppBase
class TM_NO_VTABLE CGXAppBase : public IGXApp
{
public:
    TM::CAnyStack                   m_stk;
    TM::CRefPtr<CGXFont>            m_font;
    TMColor                         m_clrText;
    CAffine                         m_aff;
    CLocal                          m_local;
    CRect                           m_rcClip; // device rect.

#ifdef TM_DBG
    struct CTID { TMThreadID tid; CTID() : tid(ThreadSelfId()){} } m_tid;

public:
    virtual void _gx_check(TMBool bThread = true, TMBool bBk = true)
    {
        DbgAssert(m_font);
        if (bThread)
            DbgAssert(ThreadSelfId() == m_tid.tid);
    }
#else
    void _gx_check(TMBool bThread = true, TMBool bBk = true) {}
#endif

public:
    CGXAppBase();
    ~CGXAppBase();

    TM_BGN_INTERFACE_MAP(IGXApp)
    TM_END_INTERFACE_MAP()

    TMBool IsPainting() { return 0 != GetRenderTarget(); }
    void ClipDevInit() { m_rcClip.SetRect(-SHRT_MAX, -SHRT_MAX, SHRT_MAX, SHRT_MAX); }
    TMBool IsRTL() const { return m_local.IsRTL(); }
    void SelectFont(const GXFont& fontkey, int nDPI);
    CZoom GetDPI();
    void PushFont();
    TMBool PopFont();
    TMBool DoClip(const GXRect& rc, TMBool bIntersect, EGXProp dwPush);
    void DoGetClip(GXRect& rc) { GetDPI().DPtoLP(m_rcClip, rc); }
    TMBool ResetFontDevice();

    // self
    virtual EGXRet InitProp(GXPTexture texture, int nDPI, TMUInt32 dwFlag) = 0;
    virtual void VertexBegin() = 0;
    virtual void VertexAddColor(GXDips x, GXDips y, TMColor clr) = 0;
    virtual void VertexFill(const GXIndices* pt, TMUInt nt) = 0;

    /////////////////////////////////////////////////////////////////////////
    // IGXApp
    virtual void* GetNativeHandle() tm_override { return 0; }

    virtual void Push(EGXProp dwFlag) tm_override;
    virtual EGXProp Pop() tm_override;
    
    virtual EGXRet SetRenderTarget(GXPTexture texture) tm_override { return InitProp(texture, 0, eGXPropRenderTarget); }

    // Transform
    virtual void GetTransform(TM::CAffine* aff = 0, int* nDPI = 0, ATLocal* lcId = 0) tm_override;
    virtual void SetTransform(const TM::CAffine* aff, const int* nDPI = 0, const ATLocal* lcId = 0, EGXTransform eMode = eGXTransformSet) tm_override;

    // clip
    virtual TMBool ClipAndPush(const GXRect& rc, TMBool bIntersect, EGXProp dwPush) tm_override;
    virtual void GetClip(GXRect& rc) tm_override;

    // font and text
    virtual void FontSetCurrent(const GXFont& ft) tm_override;
    virtual void FontGetCurrent(GXFont& ft) tm_override;
    virtual GXDips FontGetHeight() tm_override;
    virtual TMColor SetTextColor(TMColor clr) tm_override;
    virtual TMColor GetTextColor() tm_override { return m_clrText; }

    // texture
    virtual void TextureCreateIndirect(IGXTexture** tex, int w, int h, EGXFormat format) tm_override;
    virtual void TextureCreateRenderTarget(IGXTexture** tex, int w, int h, EGXFormat format) tm_override;
    virtual void TextureCreateSwapChain(IGXTexture** tex, TMHWnd hWnd, int w, int h, EGXFormat format) tm_override;
    virtual void TextureCreateFromImage(IGXTexture** tex, HIMAGE img) tm_override;

	// canvas
	virtual void CanvasCreate(IGXCanvas** canvas) tm_override;

    // shapes
    virtual void DrawRectEdge(const GXRect& rc, GXDips wBorderH, GXDips wBorderV, TMColor clr[4]) tm_override;
    virtual void GrandientFill(const GXRect& rc, TMColor clrFrom, TMColor clrTo, EGrandientFill eFill) tm_override;
    virtual void DrawIcon(const GXRect& rc, TMHIcon hIcon, TMBool bR2L) tm_override;
};


///////////////////////////////////////////////////////////////////////////////
// CGXDevMgr
enum EDeviceReset
{
    eDeviceResetGDI = 0,
    eDeviceReset1ms = 1,
    eDeviceReset3s  = 3000,
};

struct CGXDevMgr
    : public CStaticBaseRefT2<CGXDevMgr, true, TM::CAniImpl<CGXDevMgr> > // don't use TM::CTimer.
{
    typedef TM::CSet<IGXAppNotify*> CGXSinkList;
    typedef CGXSinkList::iterator   CGXSinkIter;
    CGXSinkList m_aSinks;
    CGXSinkIter m_iterCur;

    EGXDevice   m_dftGraDev[5];
    EGXDevice   m_dftTxtDev;
    CGXAppBase* m_gxAppBase;

    CGXDevMgr();
    ~CGXDevMgr();

    // IGXAppNotify
    void NotifyDispatch(CGXAppBase* pApp, TMResult hr);
    void NotifyDispatchError(TMResult hr) { NotifyDispatch(0, hr); }
    void NotifyAdd(IGXAppNotify* p);
    void NotifyErase(IGXAppNotify* p);

    // device
    void DoChangeDevice(CGXAppBase* pApp);
    void ChangeDevice();
    TMBool IsDeviceLost() const { return 0 != m_tmID; }
    TMBool IsCurrentDevice(CGXAppBase* pApp) const { return pApp == m_gxAppBase; }

    void SetDefaultGraDevices(const EGXDevice* devs, int nDevs, TMBool bUpdate);
    void SetDefaultTxtDevices(EGXDevice dev, TMBool bUpdate);
    void SetDeviceLost(EDeviceReset eReset);

    //  CAniProc
    void OnAni(TMHAni tm, ...);
};


///////////////////////////////////////////////////////////////////////////////
// public function
template <class APP, class TEX>
static EGXRet _GXApp_InitProp(APP* app, TEX texture, int nDPI, TMUInt32 dwFlag)
{
    while (dwFlag & eGXPropRenderTarget)
    {
        if (app->m_pBk == texture)
        {
            dwFlag &= ::tm_not(eGXPropRenderTarget);
            break;
        }
        
        if (texture)
        {
            if (texture->GetApp() != app)
                return eGXRetDeviceChanged;
            if (app->m_pBk)
                app->m_pBk->OnResetRenderTarget(app, texture);
            texture->OnSetRenderTarget(app);
        }
        else if (app->m_pBk)
                app->m_pBk->OnResetRenderTarget(app, 0);
        app->m_pBk = texture;
        break;
    }
    
    if (dwFlag & eGXPropDPI)
    {
        GX_TRACE("  _GXApp_InitProp::SetDPI(%d)", nDPI);
        app->SelectFont(app->m_font->GetKey(), nDPI);
    }
    else if (dwFlag & eGXPropFont)
    {
        GX_TRACE("  _GXApp_InitProp::MakeCurrentFont()");
    }
    
    if (app->m_pBk)
    {
        if (dwFlag & eGXPropClip)
            app->ClipDevNow();
        else if (dwFlag & eGXPropRenderTarget)
            app->ClipDevInit();
    }
    
    if ((dwFlag & eGXPropTransform) && app->m_pBk)
        app->InitDevTransform();
    
    //     DbgAssert(GetDPI() == GetAffDPI());
    return eGXRetSuccess;
}

TM_END_NAMESPACE


#endif  // TM_RENDER_GDI

