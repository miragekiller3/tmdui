/**
 *  @file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.11.10
 *
 *  Copyright (C) 2015 miragekiller                               
 */

#pragma once

#ifndef GXTEXT_H
#define GXTEXT_H

#include "../tmgdix.h"
#include "gxapp.h"
#include "gxfont.h"


TM_BGN_NAMESPACE


struct GXTextBase__
    : public IGXText 
    , public CGXTextFmt
{
    GXSize                  m_szMax;
    TM::CRefPtr<CGXFont>    m_font;
    CGXStrPtr               m_strTxt;

    TM_UNUSED_IC_GetDebugInfo();

    GXTextBase__(TMUInt flag)
        : CGXTextFmt(flag)
        , m_szMax((GXDips)GX_MAX_LINE_WIDTH, (GXDips)GX_MAX_LINE_WIDTH)
    {
    }

protected:
    TMBool TrySetMaxSize_Text(const GXSize& sz) // return: bDirty
    {
        TMBool bRebuild =
            (IsUseMaxWidth() && (sz.Width() != m_szMax.Width())) ||
            (IsUseMaxHeight() && (sz.Height() != m_szMax.Height()));
        return bRebuild;
    }

    template <class T_Rich>
    TMBool TrySetMaxSize_Rich(T_Rich* rich, const GXSize& sz) // return: bDirty
    {
        TMBool bRebuild =
            (IsAutoWrapRich() && (sz.Width() > m_szMax.Width() || sz.Width() < rich->GetWidth())) ||
            (IsUseMaxHeight() && (sz.Height() != m_szMax.Height()));
        m_szMax = sz;
        return bRebuild;
    }

    GXDips GetMaxWidth() const { return (m_flag & DT_WORDBREAK) ? tm_max(m_szMax.Width(), 1) : GX_MAX_LINE_WIDTH; }
//     GXDips GetMaxHeight() const { return IsUseMaxHeight() ? tm_max(m_szMax.Height(), 1) : SHRT_MAX; }

    // IGXText
    virtual CGXStrPtr GetString(TMSel0 nBgn, int nSize) tm_override { return m_strTxt.Mid(nBgn, nSize); }
    virtual int GetLength() tm_override { return m_strTxt.length(); }
    virtual TMUInt32 GetFormat() tm_override { return m_flag; }
};

struct GXText__
    : public GXTextBase__
{
    CTextCache m_cache;

    GXText__();
    ~GXText__() {}

    void CalcDeviceSize(class CGXAppBase* app, CSizeI& sz, TMBool bCalcEllipsis = false);

public:
    // IUnknown
    TM_BGN_INTERFACE_MAP(IUnknown)
    TM_END_INTERFACE_MAP()

    // IGXText
    virtual void SetString(const CGXStrPtr& str) tm_override;
    virtual CGXStrPtr GetHtml(TMSel0 nBgn = 0, int nSize = -1) tm_override;
    virtual void SetFormat(TMUInt32 uFormat) tm_override;
    virtual void CalcSize(GXSize& sz, IGXApp* gapp) tm_override;
//     virtual void Paint(const GXPoint& ptOrg, RICHDRAW* td, IGXApp* gapp) tm_override { Paint(ptOrg, td, gapp, 0);}
//     virtual void Paint(const GXRect& rc, RICHDRAW* td, IGXApp* gapp) tm_override
//     {
//         this->SetMaxSize(rc.Size());
//         this->Paint(rc.TopLeft(), 0, gapp);
//     }

    virtual TMBool GetSize(GXSize& sz) tm_override;
    virtual void Reclaim() tm_override;

    virtual void Paint(const GXRect& rc, RICHDRAW* td, GXRect* rcReal, IGXApp* gapp) tm_override;
};


TM_END_NAMESPACE


#endif // GXTEXT_H
