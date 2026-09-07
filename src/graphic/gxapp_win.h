/*
 *  @file
 *  @brief  GDI render
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.13
 *
 *  Copyright (C) 2011 miragekiller
 */

#pragma once

#include "gxapp.h"

// #ifdef TM_RENDER_GDI

#ifndef GXAPP_WIN_H
#define GXAPP_WIN_H


TM_BGN_NAMESPACE


struct CGXAppWin;


////////////////////////////////////////////////////////////////////////////////
// CGXAppWin
class CVertexWin
{
public:
    typedef TM::CVector<TRIVERTEX> CTVList;
    CTVList m_aTv;
    
public:
    CGXAppWin* GetApp();

    void Clear()
    {
        m_aTv.clear();
    }
    
    void Add(int x, int y, TMUInt16 r, TMUInt16 g, TMUInt16 b, TMUInt16 a)
    {
        TRIVERTEX f = {x, y, (COLOR16)(r << 8), (COLOR16)(g << 8), (COLOR16)(b << 8), (COLOR16)(a << 8)};
        m_aTv.push_back(f);
    }
    
    void Render(const GXIndices* pt, TMUInt nt);
};

struct CGXAppWin
    : public CStaticBaseRefT2<CGXAppWin, true, CGXAppBase>
{
#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        return IC_Size(m_vertex.m_aTv);
    }
#endif // TM_DBG

    typedef CGXAppWin    GXApp;
    CTextureImage*       m_pBk;
    CDCHandle            m_dcBk;
    CVertexWin           m_vertex;

    CGXAppWin();
    ~CGXAppWin();

    void _gx_check(TMBool bThread = true, TMBool bBk = true)
    {
        if (bBk)
            DbgAssert(m_pBk);
        CGXAppBase::_gx_check(bThread, bBk);
//        DbgAssert(GetObjectType(m_dcBk) == OBJ_MEMDC);
//        //     DbgAssert(GetObjectType(p->m_dcTxt) == OBJ_MEMDC);
//        DbgAssert(m_font);
//        DbgAssert(GXApp::SGetThis()->IsCreatorThread());
////         DbgAssert(!GetLayout(m_dcBk) == !m_local.IsRTL());
    }

    void ClipDevNow();
    void InitDevTransform();
//     static CGXAppWin* CheckGetInstance() { return GXApp::CheckGetInstanceNoAddRef(); }

    // CGXAppBase
    virtual EGXRet InitProp(GXPTexture texture, int nDPI, TMUInt32 dwFlag) tm_override;

    /////////////////////////////////////////////////////////////////////////
    // IGXApp
    virtual EGXDevice GetDeviceType(GXADAPTERDESC* desc) tm_override { return eGXGraDevGDI | m_font->FontGetDeviceType(); }

//     virtual EGXRet SetRenderTarget(GXPTexture texture);
    virtual GXPTexture GetRenderTarget() tm_override { return m_pBk; }

    virtual void DrawIcon(const GXRect& rc, HICON hIcon, TMBool bR2L) tm_override;
    //virtual void DrawDC(const GXRect& rcDst, HDC hSrc, const TMRectI& rcSrc, TMUInt32 dwRop);

    // Transform
//     virtual void GetTransform(CAffine& aff) tm_override { aff = m_aff; }
//     virtual void SetTransform(const CAffine& aff, ETransformSet mode = eTransformSet) tm_override;
//     virtual void SetLanguageId(ATLocal lcId) tm_override;
    
    // clip
    virtual TMBool ClipAndPush(const GXRect& rc, TMBool bIntersect, EGXProp dwPush) tm_override { return DoClip(rc, bIntersect, dwPush); }
    virtual void GetClip(GXRect& rc) tm_override { DoGetClip(rc); }
//     virtual TMBool TryIntersectClip(const GXRect& rc, GXRect& rcOld) tm_override;
//     virtual int ExcludeClipRect(GXDips left, GXDips top, GXDips right, GXDips bottom) tm_override;
//     virtual void ClearExcludeClipRect() tm_override;
    
    // font and text
//     virtual void FontSetCurrent(const GXFont& ft);
//     virtual void FontGetCurrent(GXFont& ft);
//     virtual void FontGetDefault(GXFont& ft); // get system default font
//     virtual GXDips FontGetHeight();
//     virtual void PushFont();
//     virtual void PopFont();
//     virtual TMColor SetTextColor(TMColor clr);
//     virtual TMColor GetTextColor() { return m_clrText; }
    
    // texture
//     virtual void TextureCreateIndirect(IGXTexture** tex, int w, int h, EGXFormat format) tm_override;
    virtual void TextureCreateFromImage(IGXTexture** tex, HIMAGE img) tm_override;

    // shapes
    virtual void Clear(TMColor clr) tm_override;
    virtual void FillRect(const GXRect& rc, TMColor clr) tm_override;
    virtual void FrameRect(const GXRect& rc, GXDips width, TMColor clr) tm_override;
    virtual void InvertRect(const GXRect& rc);
    // GX_API void gxXorRect(const GXRect& rc, TMColor clr = TMRGB(255,255,255));
//     virtual void DrawLine(const GXPoint& from, const GXPoint& to, int width, TMColor clr);
//     virtual void DrawPolyline(const GXPoint* pt, int n, int width, TMColor clr);
    
    virtual void DrawFocusRect(const GXRect& rc) tm_override;
    
    virtual void VertexBegin() tm_override;
    virtual void VertexAddColor(GXDips x, GXDips y, TMColor clr) tm_override;
    virtual void VertexFill(const GXIndices* pt, TMUInt nt) tm_override;

//     void VertexAddRGBA(GXDips x, GXDips y, TMUInt8 r, TMUInt8 g, TMUInt8 b, TMUInt8 a);
};

typedef CGXAppWin CGXAppPlateform;


TM_END_NAMESPACE

#endif // GXAPP_WIN_H

// #endif  // TM_RENDER_GDI

