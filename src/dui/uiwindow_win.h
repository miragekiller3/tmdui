/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.4.13
 *
 *  Copyright (C) 2016 miragekiller
 */

#pragma once

#ifndef UIWINDOW_WIN_H
#define UIWINDOW_WIN_H

#include "../tmthunk.h"
#include "view.h"
#include "uiwindow.h"
#include "platform.h"


#define CTuiWnd_Platform CTuiWnd_Win

TM_BGN_NAMESPACE


class CViewWin;
class ITheme;


struct CPosing 
{
    CWndWindowPos* m_pos; 

    CPosing()
        : m_pos(0)
    {
    }

    void Release()
    {
        m_pos = 0;
    }

    operator CWndWindowPos* () const { return m_pos; }

    void Init(HTUI hMgr, CWndWindowPos* pos)
    {
        if (!m_pos)
        {
            m_pos = pos;
            CWindowAT wnd(hMgr->GetHWND());
            CRect rc;
            wnd.GetWindowRectToParent(rc);
            pos->ReplaceFromRect(rc);
        }
        else
        {
            pos->ReplaceFromWP(*m_pos);
            m_pos = pos;
        }
    }
};


///////////////////////////////////////////////////////////////////////////////
// _CRealWindow
class CRealWindow
    : public IRealWindow
{
public:
    typedef CDirtyRectT<CRect>      CRectUpdate;
    HWND            m_hWnd;
    CAniID          m_ani; // 0 if window hide or !Interval
    ITheme*         m_pTheme;
    CRectUpdate     m_rcUpdate; // left,top from layer. validate when flip.
    
protected:
    CRealWindow() : m_hWnd(0), m_pTheme(0) {}
    ~CRealWindow();

protected:
    void AddFlipChain();
    void MergeNativeInvalidateRect(const TMRect& rc);
    void MergeNativeInvalidateRect();
    void UpdateWindow();

public:
    bool PaintToScreen();
    bool SetTheme(EThemeType eType, TMBool bForce = false);

public:
    // IRealWindow
    virtual TMHWnd GetReal() const tm_final { return m_hWnd; }
    virtual TMBool IsAllStyle(TMUInt32 dwStyle, TMUInt32 dwMask) tm_final { return WIN::IsAllStyle(m_hWnd, dwStyle, dwMask); }
    virtual CFrame* GetFrame() tm_final;
    
    virtual void RealUpdateAlpha(TMBool bUpdateNow) tm_final;
    virtual void RealChangeFrame(TMBool bPost) tm_final;
    virtual void RealInvalidate(TMBool bDrawHide, TMBool bUpdate = false) tm_final;

    //  CAniProc
    void OnAni(TMHAni, ...);
};


///////////////////////////////////////////////////////////////////////////////
// CTuiWnd_Win
class CTuiWnd_Win
    : public CTuiWnd
    , public CRealWindow
{
private:
    TM_CLASS_INSTANCE_COUNT(CTuiWnd_Win);

public:
    CDropTargetMgr      m_drop;
    CAtlThunk           m_thunk;
    WNDPROC             m_procOld;
    HWND                m_hWndFocus;
    CPosing             m_posing;
    static CAtlThunk    s_thunkCache;

    CTuiWnd_Win()
        : m_procOld(0)
        , m_hWndFocus(0)
    {
    }
    
    ~CTuiWnd_Win();

    TMHWndNative GetNativeHWND() { return m_hWnd; }

    static LRESULT CALLBACK _WNDPROC_Before(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK _WNDPROC(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK _WNDPROC_Null(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return 0; }

    void TrackSysMenu(TMLParam lParam, TMPoint* pt = 0);
    TMResult CallWindowMessageChain(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void CalcWindowPos(CWndWindowPos& wp, TMUInt32 dwStyle);
    void GetWindowRectNow(CRect& rc);
    HTUI ManagerInit(TUIFACTORY* pFactory, TMHModule hInst, TMHWnd hWnd, void* param, TMBool bShowWindow, const CRect* prc);

private:
    // wndproc, subclass, unsubclass
    static TMBool _WndProc(CTuiWnd_Win* p, HWND h, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
    {
        return p->ProcessWindowMessage(h, uMsg, wParam, lParam, lResult, dwMsgMapID);
    }
    void Subclass_wself() { m_mgrWndProc.Subclass(m_hWnd, CWindowMsgDelegate(this, _WndProc), this); }
    TMBool Unsubclass_wself() { return m_mgrWndProc.Unsubclass(m_hWnd, CWindowMsgDelegate(this, _WndProc)); }
    TMBool ProcessWindowMessage(TMHWnd hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0);

public:
    // CTuiObject
    TMResult Dispatch(TMUInt msg, TMWParam wp, TMLParam lp);

    // CTuiWgt
    virtual TMBool ProcessWindowMessage(CTuiWgt* hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) tm_override;
    virtual TMHWnd GetOverlayHWND() tm_override { return m_hWnd; }
    virtual void UpdateOnArrowDirty() { GetReal()->RealChangeFrame(false); } 
    virtual TMBool SetWindowPos(CTuiWgt* pInsertAfter, GXDips x, GXDips y, GXDips cx, GXDips cy, TMUInt nFlags);

    // CTuiLayerBase
    virtual void CreateBkBuffer(IGXApp* gapp, const CSize& sz) tm_override;

    //CTuiLayerWnd
    virtual IRealWindow* GetReal() tm_final { return this; }

    // CTuiWnd
    virtual TMHWnd MgrGetHWND() const tm_override { return m_hWnd; }
    virtual TMBool MgrIsAllStyle(TMUInt32 dwStyle, TMUInt32 dwMask) tm_override { return WIN::IsAllStyle(m_hWnd, dwStyle, dwMask); }

    // IRealWindow
    CTuiLayerWnd* GetLayer() tm_final { return this; }

public:
    // CTuiObject
#ifdef TUI_EDIT_MODE
    virtual CString GetDebugInfo() tm_override;
#endif // TUI_EDIT_MODE
};


///////////////////////////////////////////////////////////////////////////////
// CViewWin
class CViewWin
    : public CView
{
public:
    TM_CLASS_INSTANCE_CHECK(CViewWin);

    CViewWin();
    ~CViewWin();
    virtual void InternalSetCapture(const CTuiWgt* hWgt);
    virtual void InternalReleaseCapture();
    virtual TMBool InternalIsCapture(const CTuiWgt* hWgt);
    
    virtual void InternalSetFocus(const CTuiWgt* hWgt);
    
    virtual TMBool InternalIsWindowActive(const CTuiWgt* hWgt);
    virtual CPoint InternalGetScreenOrg(HTUI hWgt);
    
    HTUI CreateDlgEx(TMHModule hInst, int nID, TMHWnd hWndParent, TUIFACTORY* pFactory, void* param);
    HTUI CreateDlgByHWND(TMHModule hInst, int nID, TMHWnd hWnd, TUIFACTORY* pFactory, void* param);
};


TM_END_NAMESPACE


#endif //UIWINDOW_WIN_H
