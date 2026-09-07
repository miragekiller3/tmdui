/*
 *  @file
 *  @brief GTK render
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2026.4.13
 *
 *  Copyright (C) 2026 miragekiller
 */

#pragma once

#ifndef GXAPP_GTK_H
#define GXAPP_GTK_H

#include "gxapp.h"

TM_BGN_NAMESPACE

struct CGXAppGtk;

class CVertexGtk
{
public:
    struct CVertex
    {
        double x;
        double y;
        TMColor clr;
    };

    typedef TM::CVector<CVertex> CTVList;
    CTVList m_aTv;

public:
    CGXAppGtk* GetApp();

    void Clear()
    {
        m_aTv.clear();
    }

    void Add(double x, double y, TMColor clr)
    {
        CVertex v = { x, y, clr };
        m_aTv.push_back(v);
    }

    void Render(const GXIndices* pt, TMUInt nt);
};

struct CGXAppGtk
    : public CStaticBaseRefT2<CGXAppGtk, true, CGXAppBase>
{
#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        return IC_Size(m_vertex.m_aTv);
    }
#endif

    typedef CGXAppGtk GXApp;
    CTextureImage* m_pBk;
    TMHDC m_dcBk;
    CVertexGtk m_vertex;

    CGXAppGtk();
    ~CGXAppGtk();

    void _gx_check(TMBool bThread = true, TMBool bBk = true)
    {
        if (bBk)
            DbgAssert(m_pBk);
        CGXAppBase::_gx_check(bThread, bBk);
    }

    void ClipDevNow();
    void InitDevTransform();

    virtual EGXRet InitProp(GXPTexture texture, int nDPI, TMUInt32 dwFlag) tm_override;
    virtual EGXDevice GetDeviceType(GXADAPTERDESC* desc) tm_override { return eGXGraDevGDI | m_font->FontGetDeviceType(); }
    virtual GXPTexture GetRenderTarget() tm_override { return m_pBk; }

    virtual void DrawIcon(const GXRect& rc, TMHIcon hIcon, TMBool bR2L) tm_override;

    virtual TMBool ClipAndPush(const GXRect& rc, TMBool bIntersect, EGXProp dwPush) tm_override { return DoClip(rc, bIntersect, dwPush); }
    virtual void GetClip(GXRect& rc) tm_override { DoGetClip(rc); }

    virtual void TextureCreateFromImage(IGXTexture** tex, HIMAGE img) tm_override;

    virtual void Clear(TMColor clr) tm_override;
    virtual void FillRect(const GXRect& rc, TMColor clr) tm_override;
    virtual void FrameRect(const GXRect& rc, GXDips width, TMColor clr) tm_override;
    virtual void InvertRect(const GXRect& rc);
    virtual void DrawFocusRect(const GXRect& rc) tm_override;

    virtual void VertexBegin() tm_override;
    virtual void VertexAddColor(GXDips x, GXDips y, TMColor clr) tm_override;
    virtual void VertexFill(const GXIndices* pt, TMUInt nt) tm_override;
};

typedef CGXAppGtk CGXAppPlateform;

TM_END_NAMESPACE

#endif // GXAPP_GTK_H

