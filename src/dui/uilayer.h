/*
 *  @file
 *  @brief layer
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.8.25
 *
 *  Copyright (C) 2021 miragekiller
 */

#pragma once

#ifndef UILAYER_H
#define UILAYER_H

#include "uiwidget.h"
#include "../tmgdix.h"


TM_BGN_NAMESPACE


enum ERenderBK
{
    eRenderBkOk = 0,    // succeeded
    eRenderBkResize,    // size modified
    eRenderBkAbort,     // the window have been destroy, or alloc memory fail, or is painting
};

template <class T>
inline TMBool _InvalidateDirtyRect(T* p, const CRectF& rc)
{
    TMBool b = p->m_rcDirty.Invalidate(rc);
    if (b)
        Edit_SpyInvalidate(0, p, 0, &p->m_rcDirty.GetDirty());
    return b;
}

///////////////////////////////////////////////////////////////////////////////
// CTuiLayerBase
class TM_NO_VTABLE CTuiLayerBase
    : public CTuiWgt
{
public:
    typedef CDirtyRectT<CTuiRect>   CRectDirty;

    CTexture        m_txBuffer;
    CRectDirty      m_rcDirty;  // for texture, left,top from mgr
    TMUInt8         m_alpha;

    TM_IF_DBG(int   m_nCountRepaint);

    TUI_IF_EDIT(TM::CFps m_fpsFlip);
    TUI_IF_EDIT(TM::CFps m_fpsReal);

public:
    TM_DECLARE_FLAG     (Painting, m_dwPriStyle,    LAYER_PRI_PAINTING);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_INT    ("Alpha", m_alpha)
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiLayerBase)
        MESSAGE_HANDLER (CM_COLORVISUAL,            OnInvalidate)
        MESSAGE_HANDLER (CM_SETALPHA,               OnSetAlpha)
        MESSAGE_HANDLER (CM_GET_BK_TEX,             OnGetBkTexture)
        CHAIN_MSG_MAP   (CTuiWgt)
    END_TUIMSG_MAP()

    TMResult OnSetAlpha(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetBkTexture(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return (TMResult)(GXPTexture)m_txBuffer; }

public:
    CTuiLayerBase();
    TMUInt8 GetAlpha() const { return m_alpha; }
    CTexture& GetTarget() { return m_txBuffer; }
    TMBool IsRegionDirty() const { return m_rcDirty.IsDirty(); }
//     void GetDeviceDirtyRect(CRectI& rc);

private:
    ERenderBK RenderBkBuffer(IGXApp* gapp);

public:
    ERenderBK RenderScene(IGXApp* gapp);
    TMBool CheckInvalidate(const TUIRect& rc);

public:
    // CTuiLayerBase
    virtual void CreateBkBuffer(IGXApp* gapp, const CSize& sz);

    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) { RenderStaticBk(gapp, 0); }
    virtual void Reclaim();

public:
    // CTuiObject
#ifdef TUI_EDIT_MODE
    CString GetDebugInfo() tm_override;
#endif // TUI_EDIT_MODE

};


///////////////////////////////////////////////////////////////////////////////
// CTuiLayerImpl
class TM_NO_VTABLE CTuiLayerImpl : public CTuiLayerBase
{
public:
    // CTuiWgt
    virtual TMBool NativeInvalidateRect(SPY_INVALIDATE_PARAM(const TUIRect& rc, TM::HTUI hSender));
    virtual void DoPrint(IGXApp* gapp, TMLParam lParam) tm_override;
};



TM_END_NAMESPACE



#endif //UILAYER_H
