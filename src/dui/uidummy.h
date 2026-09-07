/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.12.29
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef UIDUMMY_H
#define UIDUMMY_H

#include "uiwindow.h"
#include "txtpic.h"
#include "tmhook.h"
#include "../tmautoptr.h"


enum EUnbindDummy
{
    // call when dummy reset, destroyed.
    // dummy:detach; activex:close obj and detach; webview2: close obj and destroy; float?:destroy;
    eUnbindDummyDefault,
    
    // call when bind HWND destroy.
    eUnbindDummyDetach,
};

TM_BGN_NAMESPACE


enum { DM_MOVEONRENDER  = 0x0001 }; // only support pop window
enum { DM_NODELAYLOAD   = 0x8000 };
// enum { DM_SCREENDPI     = 0x04 };


template <class T>
struct C3rdAutoDestroy
{
    C3rdAutoDestroy(void*){}
};


/////////////////////////////////////////////////////////
// for sync window state
struct IDummy
{
protected:
    ~IDummy() {}

public:
    virtual void DestroyDummy(EUnbindDummy eReason) = 0;
    virtual TMBool SyncIsValid() { return true; }
    virtual TMBool SyncIsUpdateNowMode(HTUI hWgt) { return false; }
    virtual TMBool SyncIsTargetVisible() = 0;
    virtual HWND GetTargetHWND() = 0;

//     virtual void SyncGetSrcScreenRect(HTUI hWgt, CRect& rc);
//     virtual void SyncGetTargetScreenRect(CRect& rc) = 0;
    virtual TMUInt32 SyncGetScreenPos(HTUI hWgt, CRectI& rc) { return 0; }
    virtual void SyncSetTargetScreenPos(CRect* rcSreen, TMUInt32 flag) = 0;
    virtual void SyncSetTargetEnable(TMBool bEnable) = 0;
    
    virtual TMBool ProcessWindowMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0);

    // helper function
    void SyncWin(HTUI hWgt, TMBool bUpdateNow);
    HWND GetTargetFocus();
    static void SyncEnableWindow(HTUI hWgt, HWND hWnd, TMWParam wp, TMBool bFloat);
    static TMBool SyncIsSelfOrOwneredWindow(HTUI hWgt, HWND hOwner);
};

class CDummyPtr
{
private:
    IDummy* p;
    friend class CDummyWindowBase;

public:
    CDummyPtr() : p(0) {}
    ~CDummyPtr() { DbgAssert(!p); } 
    IDummy* operator ->() const { return p; }
    operator IDummy* () const { return p; }
    void DestroyDefault() { if (p) p->DestroyDummy(eUnbindDummyDefault); } // DbgAssert(!p); }
};


/////////////////////////////////////////////////////////
// CDummyWindowBase
class TM_NO_VTABLE CDummyWindowBase
    : public IDummy
{
protected:
    CWindowAT m_wnd;
    CTuiWgt*  m_pWnd;

public:
    TM_UNUSED_IC_GetDebugInfo();

//     TMBool IsDetached();
    void AttachWidget(HTUI hWnd);
    void DetachWidget();
    void OnZoomed(int nDpi);

    // self
    virtual void BindHWND(TMHWnd hWnd);
    virtual TMHWnd UnbindHWND();
    
    // IDummy
    virtual TMBool SyncIsValid() tm_override { return m_wnd != 0; }
    virtual HWND GetTargetHWND() tm_override { return m_wnd; }

    virtual TMBool SyncIsTargetVisible() tm_override;
//     virtual TMUInt32 SyncGetGetScreenPos(HTUI hWgt, CRectI& rc);
//     virtual void SyncGetTargetScreenRect(CRect& rc) tm_override;
    virtual TMUInt32 SyncGetScreenPos(HTUI hWgt, CRectI& rc) tm_override;
    virtual void SyncSetTargetScreenPos(CRect* rcSreen, TMUInt32 flag) tm_override;
    virtual void SyncSetTargetEnable(TMBool bEnable) tm_override;
    virtual TMBool ProcessWindowMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) tm_override;
};



/////////////////////////////////////////////////////////
// CTuiDummyBase
class TM_NO_VTABLE CTuiDummyBase
    : public CTuiWgt
{
public:
    CDummyPtr   m_ptrDummyImpl;

    TM_DECLARE_FLAG             (StyleMoveOnRender, m_dwStyle,      DM_MOVEONRENDER);
    TM_DECLARE_FLAG             (FlagChild,         m_dwPriStyle,   DM_PRI_MODE_CHILD);
    TM_DECLARE_FLAG             (FlagPop,           m_dwPriStyle,   DM_PRI_MODE_POP);

    BEGIN_TUIMSG_MAP            (CTuiDummyBase)
        CHAIN_MSG_MAP_POINTER   (m_ptrDummyImpl);
        MESSAGE_HANDLER         (WM_NCDESTROY, OnDestroy)
        MESSAGE_HANDLER         (CDM_SETWINDOW, OnSetWindow)
        CHAIN_MSG_MAP           (CTuiWgt);
    END_TUIMSG_MAP              ()

    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        m_ptrDummyImpl.DestroyDefault();
        bHandled = false;
        return 0;
    }

    TMResult OnSetWindow (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        InitWin((TMHWnd)wParam);
        return 1;
    }

    ~CTuiDummyBase()
    {
        DbgAssert(!m_ptrDummyImpl);
    }

    HWND GetWnd()
    {
        return m_ptrDummyImpl ? m_ptrDummyImpl->GetTargetHWND() : 0;
    }

    void InitWin(CWindow wnd);


#ifdef TUI_EDIT_MODE
    CString GetDebugInfo() tm_override;
#endif // TUI_EDIT_MODE

};


/////////////////////////////////////////////////////////
// CDummyChild
// child window mode and windowless mode, for dummy, activex, webview.
class CDummyChild
    : public CDummyWindowBase
    , public CHookGetMessageExImpl<CDummyChild>
    , public CHookCallWndImpl<CDummyChild>
{
protected:
    typedef CHookCallWndImpl<CDummyChild>   CHookCallWnd;

public:
    TM_SELF_ALLOC(CDummyChild);

//     CDummyChild();
    ~CDummyChild();
//     void Construct(CTuiDummyBase* hWnd);

    bool IsWindowLess() const { return 0 == (TMHWnd)m_wnd; }
//     TMBool IsValideWindow(HWND hWnd);
    TMBool SetNextFocus(MSG* msg);
    virtual void SetDefaultWndFocus(TMBool bReverse);

    // CDummyWindowBase
    virtual void BindHWND(TMHWnd hWnd) tm_override;
    virtual TMHWnd UnbindHWND() tm_override;

    // CHookCallWnd, CActiveXCont override
    virtual TMBool OnCallWndProc(CWPSTRUCT* ps);

    // CHookGetMessageEx, CActiveXCont override
    virtual TMBool PreTranslateMessage(MSG* msg);

    // IDummy
    virtual void DestroyDummy(EUnbindDummy eReason) tm_override { DetachWidget(); UnbindHWND(); delete this; }
    virtual TMBool ProcessWindowMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) tm_override;
};

/////////////////////////////////////////////////////////
// CDummyPop 
class CDummyPop
    : public CDummyWindowBase
    , public CHookCallWndRetImpl<CDummyPop>
{
protected:
    typedef CHookCallWndRetImpl<CDummyPop> CHookCallWndRet;

public:
    TM_SELF_ALLOC(CDummyPop);

    // CDummyWindowBase
    virtual void BindHWND(TMHWnd hWnd) tm_override;
    virtual TMHWnd UnbindHWND() tm_override;

    // IDummy
    virtual void DestroyDummy(EUnbindDummy eReason) tm_override { DetachWidget(); UnbindHWND(); delete this; }
//     virtual TMBool ProcessWindowMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam,
//         TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) tm_override;
    virtual TMBool SyncIsUpdateNowMode(HTUI hWgt) tm_override
    {
        return !(static_cast<CTuiDummyBase*>(hWgt))->IsStyleMoveOnRender();
    }

    // CHookCallWndRet
    TMBool OnCallWndRetProc(CWPRETSTRUCT* ps);

public:
    ~CDummyPop();
//     void Construct(CTuiDummyBase* hWnd);
};



/////////////////////////////////////////////////////////////////////////////
// class CTuiDummyImpl
class TM_NO_VTABLE CTuiDummyImpl
    : public CTuiDummyBase
{
private:
    TMUInt32 m_nIDDlg;

public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_DStyle, CSymbol_WsStyle)
        TM_MASK_SYMBOL  (DM_MOVEONRENDER)
//         TM_MASK_SYMBOL  (DM_FOLLOWMODE)
//         TM_MASK_SYMBOL  (DM_SCREENDPI)
//         TM_MASK_SYMBOL  (DM_LAYEREDCHILD)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

//     TM_BGN_MASK_SYMBOL  (CSymbol_WsStyle)
//         TM_MASK_SYMBOL  (DM_SETFONT)
//         TM_MASK_SYMBOL  (DM_SETWINDOWTEXT)
//         TM_MASK_SYMBOL  (DM_CLIPPARENT)
//     TM_END_MASK_SYMBOL  ()
    
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_DStyle)
        OBJ_PROP_DLG    ("BindDlg", m_nIDDlg)
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()
    
    BEGIN_TUIMSG_MAP    (CTuiDummyImpl)
        MESSAGE_HANDLER (CDM_GETDUMMYINFO,  OnGetInfo)
        MESSAGE_HANDLER (WM_NCCREATE,       OnCreate)
        CHAIN_MSG_MAP   (CTuiDummyBase)
    END_TUIMSG_MAP      ()

    TMResult OnCreate (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetInfo(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    CTuiDummyImpl()
        : m_nIDDlg (0)
    {
        m_dwStyle |= WS_TABSTOP;
    }

    // CTuiObject
#ifdef TUI_EDIT_MODE
    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_nIDDlg)
            return true;
        return CTuiDummyBase::EditorMatchID(n);
    }
#endif // TUI_EDIT_MODE
};


/////////////////////////////////////////////////////////////////////////////
// class CTuiDelayDummy
class TM_NO_VTABLE CTuiDelayDummy
    : public CTuiDummyBase
{
public:
    TM_DECLARE_FLAG     (PriLoaded,         m_dwPriStyle,   WV_PRI_LOADED);
    TM_DECLARE_NOFLAG   (StyleDelayLoad,    m_dwStyle,      DM_NODELAYLOAD);

    TM_BGN_MASK_SYMBOL2 (CSymbol_DStyle, CSymbol_WsStyle)
        TM_MASK_SYMBOL  (DM_NODELAYLOAD)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()
        
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_DStyle)
        CHAIN_OBJ_PROP  (CTuiDummyBase)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiDelayDummy)
        MESSAGE_HANDLER (WM_NCCREATE,                   OnShow)
        MESSAGE_HANDLER (CM_SHOWWINDOW_P,               OnShow)
        MESSAGE_HANDLER (WM_SHOWWINDOW,                 OnShow)
        CHAIN_MSG_MAP   (CTuiDummyBase)
    END_TUIMSG_MAP()

    TMResult OnShow     (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    virtual void Recreate() = 0;
};

TM_END_NAMESPACE

#endif // UIDUMMY_H
