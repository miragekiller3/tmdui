/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.11.10
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef FRAME_H 
#define FRAME_H

#include "module.h"


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// FRAMEINFO
struct FRAMEINFO
{
    CTuiRect    m_border;
    int         m_nSBWidth;
    int         m_nSBHeight;

    FRAMEINFO()
        : m_border(0,0,0,0)
        , m_nSBWidth(0)
        , m_nSBHeight(0)
    {
    }

    void AdjustBorder(CTuiRect& rc)
    {
        rc.SetRect(
            rc.Left() + m_border.Left(),
            rc.Top() + m_border.Top(),
            rc.Right() - m_border.Right(),
            rc.Bottom() - m_border.Bottom());
    }

    void AdjustHScrollBar(TMUInt32 dwLayout, CTuiRect& rc)
    {
        if (LAYOUT_HSCROLL == (dwLayout & (LAYOUT_HSCROLL | LAYOUT_FLOATSCROLL)))
            rc.OffsetBottom(-m_nSBHeight);
    }

    void AdjustVScrollBar(TMUInt32 dwLayout, CTuiRect& rc)
    {
        if (LAYOUT_VSCROLL == (dwLayout & (LAYOUT_VSCROLL | LAYOUT_FLOATSCROLL)))
            rc.OffsetRight(-m_nSBWidth);
    }

    void AdjustScrollBar(TMUInt32 dwLayout, CTuiRect& rc)
    {
        AdjustHScrollBar(dwLayout, rc);
        AdjustVScrollBar(dwLayout, rc);
    }

    void AdjustClientRect(TMUInt32 dwLayout, CTuiRect& rc)
    {
        AdjustBorder(rc);
        AdjustScrollBar(dwLayout, rc);
    }

    void ResetScrollFlag(HTUI hWnd);

    static void InvalidateHotScrollbar(HTUI hOld, HTUI hNew);

    inline void _Dbg_CheckFrame(const CTuiRect& rcW, const CTuiRect& rcC)
    {
        DbgAssert(abs(rcW.Left() - rcC.Left() + m_border.Left()) < 0.5f
            || abs(rcW.Right() - rcC.Right() - m_border.Right() - m_nSBWidth) < 0.5f);
        DbgAssert(abs(rcW.Top() - rcC.Top() + m_border.Top()) < 0.5f
            || abs(rcW.Right() - rcC.Right() - m_border.Right() - m_nSBHeight) < 0.5f);
        DbgAssert(abs(rcW.Bottom() - rcC.Bottom() - m_border.Bottom()) < 0.5f
            || abs(rcW.Bottom() - rcC.Bottom() - m_border.Right() - m_nSBHeight) < 0.5f);
        DbgAssert(abs(rcW.Right() - rcC.Right() - m_border.Right()) < 0.5f
            || abs(rcW.Right() - rcC.Right() - m_border.Right() - m_nSBHeight) < 0.5f);
    }
};



///////////////////////////////////////////////////////////////////////////////
// IFrameSource
class IFrameSource
{
public:
    virtual HTUI FSGetWin() = 0;
    virtual TMUInt32 FSGetStyle() = 0;
    virtual TMUInt32 FSGetExStyle() = 0;
    virtual TMUInt32 FSGetLayout() = 0; // get scroll bar type: LAYOUT_H(V)SCROLL, LAYOUT_HOTSCROLL, LAYOUT_FLOATSCROLL
    virtual TMBool FSIsActive() { return false; }
//     virtual TMHIcon FSGetIcon() { return TMHIcon(); }
    virtual void FSGetRect(TUIRect& rc) = 0;
    virtual void FSGetHScrollInfo(TUISCROLLINFO& si) {}
    virtual void FSGetVScrollInfo(TUISCROLLINFO& si) {}
    virtual void FSGetHScrolBarRect(TUIRect& rc, int nBarHeight) = 0;
    virtual void FSGetVScrolBarRect(TUIRect& rc, int nBarWidth) = 0;
    virtual void FSSendMessage(TMUInt uMsg, TMWParam wp = 0, TMLParam lp = 0) = 0;
    virtual void FSCalcShadow(CTuiRect& rc) {}

    void Update();
    TM_DECLARE_FLAG_GET (LayoutHScroll, const_cast<IFrameSource*>(this)->FSGetLayout(), LAYOUT_HSCROLL);
    TM_DECLARE_FLAG_GET (LayoutVScroll, const_cast<IFrameSource*>(this)->FSGetLayout(), LAYOUT_VSCROLL);
    TM_DECLARE_FLAG2_GET(LayoutHotScroll, const_cast<IFrameSource*>(this)->FSGetLayout(), LAYOUT_HOTSCROLL, LAYOUT_HOTSCROLL);
    TM_DECLARE_FLAG_GET (LayoutFloatScroll, const_cast<IFrameSource*>(this)->FSGetLayout(), LAYOUT_FLOATSCROLL);
    TM_DECLARE_FLAG_GET (CanRestore, const_cast<IFrameSource*>(this)->FSGetStyle(), WS_MAXIMIZE);
};


///////////////////////////////////////////////////////////////////////////////
// CFrame
class TM_NO_VTABLE CFrame : public CTuiObject
{
public:
    enum { eObjType = e_otFrm };

protected:
    CFrame() {}
    ~CFrame() {};

public:
    // wParam: wParam of WM_SYSCOMMAND
    static TMBool DragMoveSize(HTUI hWgt, TMWParam wParam);

public:
    virtual TMBool ProcessFrameMessage(IFrameSource* pSender, TMUInt uMsg,
        TMWParam wParam, TMLParam lParam, TMResult& lResult) = 0;
    virtual void GetFrameInfo(HTUI hWnd, FRAMEINFO& fi, TMUInt32 dwStyle, TMUInt32 dwExStyle) {}
    virtual TMBool PtInOBorder(const CTuiRect& rc, const CTuiPoint& pt) { return false; }
};

struct CFrmPtr : public CObjPtr<CFrame>
{
};

TM_END_NAMESPACE


#endif // FRAME_H 
