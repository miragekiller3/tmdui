/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.12.15
 *
 *  Copyright (C) 2016 miragekiller
 */

#pragma once

#ifndef PICBMP_H
#define PICBMP_H

#include "pic.h"


TM_BGN_NAMESPACE


class TM_NO_VTABLE CPicBmpBase
    : public CPicTranform
    , public IImageSizeDecide
{
public:
    bool     m_bLoaded;
    bool     m_bMultiDPI;
    CString  m_strFile;
    TMUInt8  m_alpha;
    CSize    m_orgSize;
    CTexture m_txt;
    CComPtr<IStream>  m_ptrStream;

    TM_BGN_MASK_SYMBOL2 (CSymbol_Flag, CSymbol_FlagBase)
        TM_MASK_SYMBOL  (ColorKey)
        TM_MASK_SYMBOL  (SourceCopy)
        TM_MASK_SYMBOL  (L2RReading)
//         TM_MASK_SYMBOL  (MultiDPI)
        TM_MASK_SYMBOL  (SimpleFrame)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_FILE   ("FileName", m_strFile)
        OBJ_PROP_MASK   ("Valid", m_nFlag, CSymbol_Flag)
        OBJ_PROP_INT    ("Alpha", m_alpha)
        OBJ_PROP_INT    ("MultiDPI", m_bMultiDPI)
        CHAIN_OBJ_PROP  (CPic)
    END_OBJ_PROP_MAP    ()

protected:
    CPicBmpBase()
        : m_bLoaded(false)
        , m_bMultiDPI(false)
        , m_alpha(0xff)
        , m_orgSize(0,0)
    {
    }

public:
    void SetAlpha(TMUInt8 nAlpha) tm_override;
    
    ~CPicBmpBase()
    {
    }

    TMBool IsLoaded(IGXTexture* texture) const;
    void LoadImage(CImage& pic, TMBool bClrKey);
    void LoadTexture(CTexture& pic, TMBool bClrKey);
    GXDips DoDecideSize(CSize& sz, int nSplite);

    inline EGXBlt GetFlag()
    {
        EGXBlt uf = GX_BLT_NONE;
        if (m_nFlag & SourceCopy)
            uf |= GX_BLT_SRCCOPY;
        if ((m_nFlag & L2RReading) && GetModule()->GetLocal().IsRTL())
            uf |= GX_BLT_R2L;
        return uf;
    }

    void OnLoad();


    // CPic
    virtual void SetStreamSource(IStream* stream) tm_override { m_ptrStream = stream; }

    // ITuiPic
    virtual CComPtr<ITuiPic> GetSubPic(int nDpi) tm_override;
    virtual void GetSize(TMSizeI& sz) tm_override {}
    virtual TMUInt GetFrames() tm_override { return 1; }
    virtual int GetDelay() tm_override { return 0; }
    virtual TMUInt GetCurrentFrame() tm_override { return 0; }
    virtual void SetCurFrame (TMUInt n) tm_override {}

    //CTuiObject
    TMResult Dispatch(TMUInt msg, TMWParam wp, TMLParam lp);

#ifdef TUI_EDIT_MODE
    CString GetDebugInfo()
    {
        CSize sz(m_orgSize);
        if (m_txt)
        {
            sz.cx = m_txt->GetWidth();
            sz.cy = m_txt->GetHeight();
        }

        GetSize(sz);
        TM::CString str;
        str.Format(L"\r\n\r\nwidth:%d; height:%d; MultiDPI:%d; frames:%d; delay:%d",
            sz.cx, sz.cy, m_bMultiDPI, GetFrames(), GetDelay());
        return CPic::GetDebugInfo() + str;
    }
#endif

};


////////////////////////////////////////////////////////////////////////////////
//CPicBmp
class TM_NO_VTABLE CPicBmp : public CPicBmpBase
{
public:
    typedef CPicBmpBase base_class;

protected:
    CBorder     m_dra;
    TMUInt16    m_nStates;
    CImage      m_image;
    TMColor     m_clrHC;

    TM_BGN_MASK_SYMBOL2 (CSymbol_Valid, CSymbol_Flag)
        TM_MASK_SYMBOL  (LeftTop)
        TM_MASK_SYMBOL  (LeftCenter)
        TM_MASK_SYMBOL  (LeftBottom)
        TM_MASK_SYMBOL  (CenterTop)
        TM_MASK_SYMBOL  (CenterCenter)
        TM_MASK_SYMBOL  (CenterBottom)
        TM_MASK_SYMBOL  (RightTop)
        TM_MASK_SYMBOL  (RightCenter)
        TM_MASK_SYMBOL  (RightBottom)
        TM_MASK_SYMBOL  (MonochromeOnHC)
//         TM_MASK_SYMBOL  (OverlyingHigh)
//         TM_MASK_SYMBOL  (OverlyingFocus)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_INT    ("FrameNum", m_nStates);
//         OBJ_PROP_INT    ("Width", m_width)
//         OBJ_PROP_INT    ("Height", m_height)
        OBJ_PROP_INT    ("Left", m_dra.left)
        OBJ_PROP_INT    ("Top", m_dra.top)
        OBJ_PROP_INT    ("Right", m_dra.right)
        OBJ_PROP_INT    ("Bottom", m_dra.bottom)
        OBJ_PROP_MASK   ("Valid", m_nFlag, CSymbol_Valid)
        CHAIN_OBJ_PROP  (base_class)
    END_OBJ_PROP_MAP    ()

public:
    CPicBmp();

    // ITuiPic
    virtual void GetSize(TMSizeI& sz) tm_override;
    virtual TMUInt GetFrames() tm_override;
    virtual int GetDelay() tm_override;
    virtual TMUInt GetCurrentFrame() tm_override;
    virtual void SetCurFrame (TMUInt n) tm_override;
    virtual void Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp) tm_override;
//     virtual void ChangeUserInterface(EUserInterface eOld, EUserInterface eNew) tm_override;

    // IImageDpiDecide
    virtual GXDips OnImageDecideSize(CSize& sz) tm_override;

    //CTuiObject
    int EditorRender(IGXApp* gapp, int nState, const CTuiRect& rc);

    void Reload();
    void OnLoadProp (IDomElement* pXml);
    void RenderNative(IGXApp* gapp, int nState, CTexture& txt, const CTuiRect& rc, WPRENDERFX* fx);
    void RenderNative(IGXApp* gapp, int nState, CTexture& tex, const CTuiRect& rc, EGXBlt eBlt, int wFrm, int hFrm, int wTex, int nAlpha);
};


////////////////////////////////////////////////////////////////////////////////
//CPicAni
class TM_NO_VTABLE CPicAni : public CPicBmp
{
protected:
    int m_nDelay;
    int m_nCur;
 
public:
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_INT    ("Delay", m_nDelay)
        CHAIN_OBJ_PROP  (CPicBmp)
    END_OBJ_PROP_MAP    ()
        
public:
    CPicAni()
        : m_nDelay (100)
        , m_nCur (0)
    {
        m_nStates = 1;
    }

    // ITuiPic
    virtual TMUInt GetFrames() tm_override;
    virtual int GetDelay() tm_override;
    virtual TMUInt GetCurrentFrame() tm_override;
    virtual void SetCurFrame(TMUInt n) tm_override;
    virtual void Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp) tm_override;
};


////////////////////////////////////////////////////////////////////////////////
//CPicTile
class TM_NO_VTABLE CPicTile : public CPicBmpBase
{
public:
    CPicTile() {}
    ~CPicTile() {}
    void Reload();
    void OnLoadProp (IDomElement* pXml);

    // IImageDpiDecide
    virtual GXDips OnImageDecideSize(CSize& sz) tm_override;

    // ITuiPic
    virtual void Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp) tm_override;
};



////////////////////////////////////////////////////////////////////////////////
//CPicScroll
class TM_NO_VTABLE CPicScroll
    : public CPicBmpBase
{
private:
    typedef CPicBmpBase base_class;

    int             m_nButtonHeight;
    int             m_nLineHeight;
    int             m_nStates;
    int             m_nThumbTop;
    int             m_nThumbBottom;
    int             m_nThumbCore;

    BGN_OBJ_PROP_MAP            ()
        OBJ_PROP_INT            ("FrameNum", m_nStates);
        OBJ_PROP_INT            ("ButtonHeight", m_nButtonHeight);
        OBJ_PROP_INT            ("LineHeight", m_nLineHeight);
        OBJ_PROP_INT            ("ThumbTop", m_nThumbTop);
        OBJ_PROP_INT            ("ThumbBottom", m_nThumbBottom);
        OBJ_PROP_INT            ("ThumbCore", m_nThumbCore);
        CHAIN_OBJ_PROP          (base_class)
    END_OBJ_PROP_MAP            ()

public:
    CPicScroll();
    void RenderEditor(IGXApp* gapp, GX::CTexture& tex, const CTuiRect& rc);
    void RenderNative(CTexture& tex, const SCROLLSTATE& state, CTuiRect rc, int nAlpha, int* ss, int wFrm);

    void OnLoadProp(IDomElement* pXml);
    void Reload();

    // ITuiPic
    virtual void Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp) tm_override;
    virtual void RenderSB(const SCROLLSTATE& state, const CTuiRect& rc, WPRENDERFX_SB* fx, IGXApp* gapp) tm_override;

    // IImageDpiDecide
    virtual GXDips OnImageDecideSize(CSize& sz) tm_override;

    // UInknown
    TM_REFAPI AddRef () tm_override { return CPic::AddRef(); }
    TM_REFAPI Release() tm_override { return CPic::Release(); }
};


TM_END_NAMESPACE

#endif // PICBMP_H

