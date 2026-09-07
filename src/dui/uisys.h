/*
 *  @file
 *  @brief  non client object
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.01.09
 *
 *  Copyright (C) 2019 miragekiller
 */

#pragma once

#ifndef UISYS_H
#define UISYS_H

#include "uibutton.h"
#include "frameform.h"


TM_BGN_NAMESPACE


#define SBS_HTML             0x0001
#define SBS_SWAPUPDOWNKEY    0x0002


///////////////////////////////////////////////////////////////////////////////
// CFrameSourceSysT
template <class T>
class TM_NO_VTABLE CFrameSourceSysT : public IFrameSource
{
public:
    IFrameSource* GetFS() { return this; }

    // IFrameSource
    virtual HTUI FSGetWin() tm_override { return static_cast<T*>(this); }
//     virtual TMUInt32 FSGetStyle() tm_override { return FSGetWin()->m_dwStyle; }
//     virtual TMUInt32 FSGetExStyle() tm_override { return FSGetWin()->m_dwAdvStyle; }
//     virtual TMUInt32 FSGetLayout() tm_override { return FSGetWin()->m_dwLayout; }
//     virtual TMBool FSIsActive() tm_override { return FSGetWin()->IsActive(); }
    virtual void FSGetRect(TUIRect& rc) tm_override { FSGetWin()->GetPaddingRect(&rc); }
//     virtual void FSGetHScrollInfo(TUISCROLLINFO& si) tm_override { FSGetWin()->GetHScrollInfo(si); }
//     virtual void FSGetVScrollInfo(TUISCROLLINFO& si) tm_override { FSGetWin()->GetVScrollInfo(si); }
//     virtual void FSSendMessage(TMUInt uMsg, TMWParam wp, TMLParam lp) tm_override { FSGetWin()->SendMessage(uMsg, wp, lp); }
//     virtual void FSInvalidateRect(const TUIRect& rc) tm_override { FSGetWin()->InvalidateRect(rc); }
    virtual void FSGetHScrolBarRect(TUIRect& rc, int nBarHeight) tm_override { FSGetRect(rc); }
    virtual void FSGetVScrolBarRect(TUIRect& rc, int nBarWidth) tm_override { FSGetRect(rc); }
};


///////////////////////////////////////////////////////////////////////////////
// CTuiScrollObj
class TM_NO_VTABLE CTuiScrollObj
    : public CTuiWgt
    , public CFrameSourceSysT<CTuiScrollObj>
{
protected:
    CScrollCtrl     m_sb;
    int             m_nMin;
    int             m_nMax;
    int             m_nPos;
    int             m_nStep;
    TMUInt32        m_dwBuddyID;
    CAutoTuiWidget  m_wndBuddy;
    CString         m_strFmt;

    TM_DECLARE_FLAG_GET (StyleDrawFocusRect,    m_dwStyle, BS_DRAWFOCURECT);
    TM_DECLARE_FLAG     (StyleHTML,             m_dwStyle, SBS_HTML);
    TM_DECLARE_FLAG_GET (StyleSwapUpDownKey,    m_dwStyle, SBS_SWAPUPDOWNKEY);
    TM_DECLARE_FLAG_GET (StyleVertical,         m_dwStyle, PBS_VERTICAL);   // trackbar, progress
    TM_DECLARE_FLAG_GET (StyleHorz,             m_dwStyle, UDS_HORZ);       // updown
    TM_DECLARE_FLAG_GET (StyleAutoBuddy,        m_dwStyle, UDS_AUTOBUDDY);  // updown

    
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_INT    ("Min", m_nMin)
        OBJ_PROP_INT    ("Max", m_nMax)
        OBJ_PROP_INT    ("Pos", m_nPos)
        OBJ_PROP_INT    ("Step", m_nStep)
        OBJ_PROP_WIN    ("BuddyID", m_dwBuddyID)
        OBJ_PROP_TEXT   ("Format", m_strFmt)
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiScrollObj)
        MESSAGE_HANDLER (CM_GETPOS,             OnGetPos)
        MESSAGE_HANDLER (CM_SETPOS,             OnSetPos)
        MESSAGE_HANDLER (CM_DELTAPOS,           OnDeltaPos)
        MESSAGE_HANDLER (CM_GETRANGE,           OnGetRange)
        MESSAGE_HANDLER (CM_SETRANGE,           OnSetRange)
        MESSAGE_HANDLER (CM_GETSTEP,            OnGetStep)
        MESSAGE_HANDLER (CM_SETSTEP,            OnSetStep)
        MESSAGE_HANDLER (CM_STEPIT,             OnStepIt)

        MESSAGE_HANDLER (CM_GETBUDDY,           OnGetBuddy)
        MESSAGE_HANDLER (CM_SETBUDDY,           OnSetBuddy)
        MESSAGE_HANDLER (CM_GETBUDDYID,         OnGetBuddyID)
        MESSAGE_HANDLER (CM_SETBUDDYID,         OnSetBuddyID)
        MESSAGE_HANDLER (CM_SETFORMAT,          OnSetFormat)

        MESSAGE_HANDLER (WM_KEYDOWN,            OnKeyDown)
        MESSAGE_HANDLER (WM_SETFOCUS,           OnInvalidate)
        MESSAGE_HANDLER (WM_KILLFOCUS,          OnInvalidate)
        MESSAGE_HANDLER (CM_MOUSESWITCH,        OnInvalidate)

        if (m_sb.ProcessFrameMessage(GetFS(), uMsg, wParam, lParam, lResult))
            return true;
        CHAIN_MSG_MAP   (CTuiWgt)
    END_TUIMSG_MAP      ()

    TMResult OnGetPos   (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return SyncNewPos(0); }
    TMResult OnSetPos   (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDeltaPos (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetRange (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetRange (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetStep  (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return m_nStep; }
    TMResult OnSetStep  (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnStepIt   (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    TMResult OnSetBuddy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetBuddy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return (TMResult)m_wndBuddy.m_hWnd; }
    TMResult OnSetBuddyID (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetBuddyID (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return m_dwBuddyID; }
    TMResult OnSetFormat(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    TMResult OnKeyDown  (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnLButtonDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    CTuiScrollObj()
        : m_nMin (0)
        , m_nMax (0)
        , m_nPos (0)
        , m_nStep(1)
        , m_dwBuddyID(0)
    {
        m_dwStyle |= WS_TABSTOP;
    }

    int CoercePos(int pos);
    int SetPos(int nNewPos, TMBool bRedraw, TMBool bSendChanging, TMUInt16 nCode);

    // self
    virtual HTUI GetTarget();
    virtual int SyncNewPos(int step) { return m_nPos + step; }

    // IFrameSource
    virtual TMUInt32 FSGetStyle() tm_override { return WS_VISIBLE; }
    virtual TMUInt32 FSGetExStyle() tm_override { return 0; }
    virtual void FSGetHScrollInfo(TUISCROLLINFO& si) tm_override { si.nProject = m_nMax - m_nMin; si.nView = 0; si.nOrg = m_nPos - m_nMin; }
    virtual void FSGetVScrollInfo(TUISCROLLINFO& si) tm_override { FSGetHScrollInfo(si); }
    virtual void FSSendMessage(TMUInt uMsg, TMWParam wp, TMLParam lp) tm_override;
    
    // CTuiObject
#ifdef TUI_EDIT_MODE
    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_dwBuddyID)
            return true;
        return CTuiWgt::EditorMatchID(n);
    }
#endif // TUI_EDIT_MODE
};



///////////////////////////////////////////////////////////////////////////////
// CTuiSysImpl
class TM_NO_VTABLE CTuiSysImpl
    : public CTuiPushButtonImpl
{
private:
    EHitID      m_nType;
    TMUInt32    m_dwTargetID;

    TM_BGN_ENUM_SYMBOL  (CSymbol_Bind)
        TM_ENUM_SYMBOL  (eHitIDCaption)
        TM_ENUM_SYMBOL  (eHitIDMin)
        TM_ENUM_SYMBOL  (eHitIDMax)
        TM_ENUM_SYMBOL  (eHitIDClose)
        TM_ENUM_SYMBOL  (eHitIDSys)
//         TM_ENUM_SYMBOL  (eHitIDHelp)
        TM_ENUM_SYMBOL  (eHitIDLeft)
        TM_ENUM_SYMBOL  (eHitIDTop)
        TM_ENUM_SYMBOL  (eHitIDRight)
        TM_ENUM_SYMBOL  (eHitIDBottom)
        TM_ENUM_SYMBOL  (eHitIDTopLeft)
        TM_ENUM_SYMBOL  (eHitIDTopRight)
        TM_ENUM_SYMBOL  (eHitIDBottomLeft)
        TM_ENUM_SYMBOL  (eHitIDBottomRight)
    TM_END_ENUM_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_WIN    ("Target", m_dwTargetID)
        OBJ_PROP_ENUM   ("type", m_nType, CSymbol_Bind)
        CHAIN_OBJ_PROP  (CTuiPushButtonImpl)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiSysImpl)
        MESSAGE_HANDLER (CM_PRECOMMAND,         OnPreCommand)
        MESSAGE_HANDLER (WM_SETCURSOR,          OnSetCursor)
        CHAIN_MSG_MAP   (CTuiPushButtonImpl)
    END_TUIMSG_MAP()

    TMResult OnPreCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetCursor(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

private:
    // CTuiButtonImpl
    virtual ITuiPic* GetDftPic() tm_override;

    // CTuiObject
#ifdef TUI_EDIT_MODE
    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_dwTargetID)
            return true;
        return CTuiPushButtonImpl::EditorMatchID(n);
    }
#endif // TUI_EDIT_MODE

public:
    CTuiSysImpl();
    HTUI GetTarget();
};


TM_END_NAMESPACE

#endif // UISYS_H
