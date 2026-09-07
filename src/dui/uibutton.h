/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.20
 *
 *  Copyright (C) 2017 miragekiller
 */

#pragma once

#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "txtpic.h"
// #include "menu.h"
#include "input.h"


TM_BGN_NAMESPACE


#define PIC_MARGIN          2


/////////////////////////////////////////////////////////////////////////////////
//CTuiButtonImpl
class TM_NO_VTABLE CTuiButtonImpl
    : public PhotoText
    , public CHotKeyWinT<CTuiButtonImpl, CPropHotKey, 1>
{
protected:
    TMUInt8     m_nNumChecks;
    TMUInt8     m_nCurChecks;
//     TMUInt8     m_dwState;
    TMInt8      m_nDownOffsetX;
    TMInt8      m_nDownOffsetY;
    CPropColor  m_clr[e_bsEnd];

public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_BStyle, CSymbol_WsStyle)
        TM_MASK_SYMBOL  (BS_NOTIFY)
//        TM_MASK_SYMBOL  (BS_DEFPUSHBUTTON)
        TM_MASK_SYMBOL  (BS_AUTOSTATE)
        TM_MASK_SYMBOL  (BS_DRAWFOCURECT)
        TM_MASK_SYMBOL  (BS_WINDOWFOREONHC)
        TM_MASK_SYMBOL  (BS_NOCLICKFOCUS)
        TM_MASK_SYMBOL  (BS_PRESSCLICK)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    TM_DECLARE_FLAG     (PriKeyDown,            m_dwPriStyle,  BS_PRI_KEYDOWN);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style",               m_dwStyle, CSymbol_BStyle)
        OBJ_PROP_INT    ("DownOffsetX",         m_nDownOffsetX)
        OBJ_PROP_INT    ("DownOffsetY",         m_nDownOffsetY)
        OBJ_PROP_CLASS  ("NormalTextColor",     m_clr[e_bsNormal])
        OBJ_PROP_CLASS  ("HighlightTextColor",  m_clr[e_bsHigh])
        OBJ_PROP_CLASS  ("DownTextColor",       m_clr[e_bsDown])
        OBJ_PROP_CLASS  ("DisableTextColor",    m_clr[e_bsDisable])
        OBJ_PROP_CLASS  ("FocusTextColor",      m_clr[e_bsFocus])
        OBJ_PROP_CLASS  ("Hotkey",              m_Hotkey[0])
        CHAIN_OBJ_PROP  (PhotoText)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiButtonImpl)
        MESSAGE_HANDLER (WM_LBUTTONDOWN,        OnLButtonDown)
        MESSAGE_HANDLER (WM_LBUTTONDBLCLK,      OnLButtonDown)
        MESSAGE_HANDLER (WM_LBUTTONUP,          OnLButtonUp)
        MESSAGE_HANDLER (CM_MOUSEENTRY,         OnMouseEntry)
        MESSAGE_HANDLER (WM_MOUSELEAVE,         OnMouseLeave)
        MESSAGE_HANDLER (WM_SETFOCUS,           OnSetFocus)
        MESSAGE_HANDLER (WM_KILLFOCUS,          OnKillFocus)
//         MESSAGE_HANDLER (WM_ENABLE,             OnEnable)
        MESSAGE_HANDLER (WM_KEYDOWN,            OnKeyDown)
        MESSAGE_HANDLER (CM_SETTEXTCOLOR,       OnSetTextColor)

//         MESSAGE_HANDLER (CM_GETSTATE,           OnGetState)
//         MESSAGE_HANDLER (CM_SETSTATE,           OnSetState)
        MESSAGE_HANDLER (CBM_GETCHECK,          OnGetCheck)
        MESSAGE_HANDLER (CBM_SETCHECK,          OnSetCheck)
        MESSAGE_HANDLER (CBM_CLICK,             OnClick)
        MESSAGE_HANDLER (CBM_SETFOREPICWIDTH,   OnSetForePicWidth)
//         MESSAGE_HANDLER (CBM_GETBUTTONSTATE,    OnGetButtonState)
        MESSAGE_HANDLER (CBM_GETBUTTONSTATECOUNT, OnGetStateNumber)
        CHAIN_MSG_MAP   (CHotKeyWin)
        CHAIN_MSG_MAP   (PhotoText)
    END_TUIMSG_MAP()

    TMResult OnLButtonDown      (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnLButtonUp        (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnKeyDown          (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnKeyUp            (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetTextColor     (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnGetButtonState   (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseEntry       (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseLeave       (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetFocus         (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnKillFocus        (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);


//     TMResult OnEnable(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
//     {
//         TMUInt8 n = wParam ? (m_dwState & ~e_bstDisable) : (m_dwState | e_bstDisable);
//         DISABLE_SPY();
//         CTuiWidget(this).SetState(n);
//         return 0;
//     }

    void SetButTextColor(IGXApp* gapp, EButState ebs, CAppColor::PFNGetColor pfnText, int nFrame);
    void PaintBk(IGXApp* gapp, int nFrame, EButStates states);
    TMResult Paint(IGXApp* gapp, TMLParam lParam, CAppColor::PFNGetColor pfnText);

    TMResult OnGetCheck(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        return m_nCurChecks;
    }

    TMResult OnSetCheck(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    TMResult OnClick (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        AutoCheckAndNotify();
//
//         OnLButtonDown (uMsg, wParam, lParam, bHandled);
//         OnLButtonUp (uMsg, wParam, lParam, bHandled);
        return true;
    }

    TMResult OnGetStateNumber(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        return m_nNumChecks;
    }

    void OnLoadProp (IDomElement* pXml)
    {
        TMBool b;
        OnDirty (0, 0, 0, b);
        CTuiWgt::OnLoadProp (pXml);
    }

    void AutoCheck();
//     void NotifyClick() { Notify (BN_CLICKED); }
    void AutoCheckAndNotify();
    void OffsetDownRect(EButStates ebs, CTuiRect& rc);
    void SendCheckMessage(int nCurChecks);
    void CheckSetCheck(int nCheck)
    {
        DISABLE_SPY();
        if (nCheck == m_nCurChecks)
            return;
        CTuiButton(this).SetCheck(nCheck);
    }
//     void SendNotify (TMUInt uCode);

    virtual int CheckChecks(int nCurChecks)
    {
        if (nCurChecks >= m_nNumChecks)
            nCurChecks = 0;
        return nCurChecks;
    }

    EButState GetMenuItemState ()
    {
        return GetButtonState();
    }

    TMBool IsClickOnPress() const { return m_dwStyle & BS_PRESSCLICK; }

//     HTUI GetTabTarget(int nTargetID);


public:
    CTuiButtonImpl (int nNumStates, int nDownOffset);
    ~CTuiButtonImpl();

    void FinalConstruct(CTuiObject* pp)
    {
        m_pBk   = GetDftPic();
        m_pFore = GetDftForePic();
        PhotoText::FinalConstruct(pp);
    }

    // CTuiWgt
//     virtual EButState DoGetState(HTUI hFocus, HTUI hOver, TMBool bMouseDown) tm_override;
    virtual EButStates DoGetStates(HTUI hFocus, HTUI hOver, TMBool bMouseDown) tm_override;

    // CTuiButtonImpl 
    virtual ITuiPic* GetDftPic() { return 0; }
    virtual ITuiPic* GetDftForePic() { return 0; }

protected:
    // CTuiObject
#ifdef TUI_EDIT_MODE
    CString GetDebugInfo() tm_override;
#endif // TUI_EDIT_MODE

};



/////////////////////////////////////////////////////////////////////////////////
//CTuiPushButtonImpl
class TM_NO_VTABLE CTuiPushButtonImpl
    : public CTuiButtonImpl
{
public:
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_INT    ("StateNumber",    m_nNumChecks)
        CHAIN_OBJ_PROP  (CTuiButtonImpl)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiPushButtonImpl)
        MESSAGE_HANDLER (WM_GETDLGCODE,    OnGetDlgCode)
        MESSAGE_HANDLER (WM_KEYUP,         OnKeyUp)
        CHAIN_MSG_MAP   (CTuiButtonImpl)
    END_TUIMSG_MAP      ()

    TMResult OnGetDlgCode (TMUInt uMsg,TMWParam wParam,TMLParam lParam,TMBool& bHandled)
    {
        return DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON;
//            ((m_dwStyle & BS_DEFPUSHBUTTON) ? DLGC_DEFPUSHBUTTON : DLGC_UNDEFPUSHBUTTON);
    }

public:
    CTuiPushButtonImpl ();
    ~CTuiPushButtonImpl();
    
    // CTuiPushButtonImpl
    virtual ITuiPic* GetDftPic() tm_override { return &GetModule()->m_aPrePic.m_picDft_BUTTON; }

    // CTuiWgt
//     virtual EButState DoGetState(HTUI hFocus, HTUI hOver, TMBool bMouseDown) tm_override;
    virtual EButStates DoGetStates(HTUI hFocus, HTUI hOver, TMBool bMouseDown) tm_override;
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override
    {
        Paint(gapp, lParam, CAppColor::GetColorButtonText);
    }
};


////////////////////////////////////////////////////////////////////////////////
// class CTuiCheckBoxImpl
class TM_NO_VTABLE CTuiCheckBoxImpl :  public CTuiButtonImpl
{
protected:
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_INT    ("StateNumber",   m_nNumChecks)
        OBJ_PROP_INT    ("State",         m_nCurChecks)
        CHAIN_OBJ_PROP  (CTuiButtonImpl)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiCheckBoxImpl)
        MESSAGE_HANDLER (WM_KEYUP,         OnKeyUp)
        CHAIN_MSG_MAP   (CTuiButtonImpl)
    END_TUIMSG_MAP      ()

public:
    CTuiCheckBoxImpl ();

    // CTuiPushButtonImpl
//     virtual ITuiPic* GetDftPic() { return 0; }
    virtual ITuiPic* GetDftForePic() tm_override { return &GetModule()->m_aPrePic.m_picDftCheck; }

    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override
    {
        Paint(gapp, lParam, CAppColor::GetColorCheckRadioText);
    }
};



////////////////////////////////////////////////////////////////////////////////
// class CTuiRadioButtonImpl
class TM_NO_VTABLE CTuiRadioButtonImpl :  public CTuiButtonImpl
{
public:
//     TM_BGN_MASK_SYMBOL2 (CSymbol_RStyle, CSymbol_BStyle)
//         TM_MASK_SYMBOL  (RS_AUTOTABSTOP)
//         TM_MASK_BREAK   ()
//     TM_END_MASK_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
//         OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_RStyle)
        OBJ_PROP_INT    ("State", m_nCurChecks)
        CHAIN_OBJ_PROP  (CTuiButtonImpl)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiRadioButtonImpl)
        MESSAGE_HANDLER (WM_GETDLGCODE,         OnGetDlgCode)
        MESSAGE_HANDLER (WM_KEYDOWN,            OnKeyDown)
        MESSAGE_HANDLER (WM_SETFOCUS,           OnSetFocus)
        MESSAGE_HANDLER (CBM_SETCHECK,          OnSetCheck)
        MESSAGE_HANDLER (CBM_GETCHECKEDBUTTON,  OnGetChecked)
        MESSAGE_HANDLER (CBM_GETNEXTBUTTON,     OnGetNext)
        MESSAGE_HANDLER (CBM_GETPREVBUTTON,     OnGetPrev)
        CHAIN_MSG_MAP   (CTuiButtonImpl)
    END_TUIMSG_MAP      ()

    TMResult OnKeyDown (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//    TMResult OnLButtonUp (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetFocus (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetCheck (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    TMResult OnGetDlgCode (TMUInt uMsg,TMWParam wParam,TMLParam lParam,TMBool& bHandled)
    {
        return DLGC_BUTTON | DLGC_RADIOBUTTON;
    }

    TMResult OnGetChecked(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return (TMResult)GetChecked(); }
    TMResult OnGetNext(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return (TMResult)GetNextVisible(); }
    TMResult OnGetPrev(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return (TMResult)GetPrevVisible(); }

protected:
    HTUI GetChecked ();
    HTUI GetNextVisible();
    HTUI GetPrevVisible();

    virtual int CheckChecks(int nCurChecks) tm_override
    {
        if (nCurChecks >= m_nNumChecks)
            nCurChecks = m_nNumChecks - 1;
        if (nCurChecks < 0)
            nCurChecks = 0;
        return nCurChecks;
    }

public:
    CTuiRadioButtonImpl();

    // CTuiPushButtonImpl
//     virtual ITuiPic* GetDftPic() { return 0; }
    virtual ITuiPic* GetDftForePic() tm_override { return &GetModule()->m_aPrePic.m_picDftRadio; }

    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override
    {
        Paint(gapp, lParam, CAppColor::GetColorCheckRadioText);
    }
};



////////////////////////////////////////////////////////////////////////////////
// class CTuiTabButtonImpl
class TM_NO_VTABLE CTuiTabButtonImpl
    : public CTuiRadioButtonImpl
//     , public CTuiMsgFilter
{
protected:
    TMUInt32 m_dwTargetID;

protected:
    TM_BGN_MASK_SYMBOL2 (CSymbol_TStyle, CSymbol_BStyle)
        TM_MASK_SYMBOL  (TS_NOCTRLTAB)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_WIN    ("Target", m_dwTargetID)
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_TStyle)
        CHAIN_OBJ_PROP  (CTuiRadioButtonImpl)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiTabButtonImpl)
        MESSAGE_HANDLER (WM_CREATE,                     OnCreate)
        MESSAGE_HANDLER (WM_DESTROY,                    OnDestroy)
        MESSAGE_HANDLER (WM_SETFOCUS,                   OnSetFocus)
        MESSAGE_HANDLER (CBM_SETCHECK,                  OnSetCheck)
        MESSAGE_HANDLER (CM_GETBUDDY,                   OnGetTarget)
        MESSAGE_HANDLER (CM_PRECOMMAND,                 OnPreCommand)
//         MESSAGE_HANDLER (WM_GETDLGCODE,             OnGetDlgCode)
        CHAIN_MSG_MAP   (CTuiRadioButtonImpl)
    END_TUIMSG_MAP      ()

    TMResult OnCreate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetFocus(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetCheck(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetTarget(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return m_dwTargetID ? (TMResult)SearchNearestItemInDlg(m_dwTargetID) : 0; }
    TMResult OnPreCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnGetDlgCode (TMUInt uMsg,TMWParam wParam,TMLParam lParam,TMBool& bHandled)
//     {
//         return (m_dwStyle & WS_VISIBLE) ? (DLGC_BUTTON | DLGC_RADIOBUTTON) : DLGC_BUTTON;
//     }

public:
    enum { eCommandPrev = SHRT_MAX - 1, eCommandNext };
    TMBool IsNoTabCtrl() const { return m_dwStyle & TS_NOCTRLTAB; }
    CTuiTabButtonImpl ();
    void SetCheckImpl(int nCheck);
    TMUInt32 GetTargetID(){ return m_dwTargetID; }// for proxy

    // CTuiMsgFilter
//     virtual TMBool PreTranslateMessage(TUIMSG* pMsg, TMResult& lResult) tm_override;

    // CTuiPushButtonImpl
    virtual ITuiPic* GetDftPic() tm_override { return &GetModule()->m_aPrePic.m_picDft_BUTTON; }
    virtual ITuiPic* GetDftForePic() tm_override { return 0; }

    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override
    {
        Paint(gapp, lParam, CAppColor::GetColorButtonText);
    }

    // CTuiObject
#ifdef TUI_EDIT_MODE
    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_dwTargetID)
            return true;
        return CTuiRadioButtonImpl::EditorMatchID(n);
    }
#endif // TUI_EDIT_MODE
};


////////////////////////////////////////////////////////////////////////////////
// class CTuiMenuItemImpl
class TM_NO_VTABLE CTuiMenuItemImpl : public CTuiButtonImpl
{
public:
    CPicPtr             m_pCheck;
    TSIZE<TMInt16>      m_szCheck;
    CPicPtr             m_pArrow;
    TSIZE<TMInt16>      m_szArrow;
    TMUInt32            m_nSubMenu; // CID<EM_DIALOG>

public:
    CTuiMenuItemImpl();

public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_MStyle, CSymbol_BStyle)
        TM_MASK_SYMBOL  (MS_KEEPPOPUP)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style",       m_dwStyle, CSymbol_MStyle)
        OBJ_PROP_INT    ("StateNumber", m_nNumChecks)
        OBJ_PROP_DLG    ("submenu",     m_nSubMenu)
        OBJ_PROP_PIC    ("CheckPic",    m_pCheck)
        OBJ_PROP_PIC    ("ArrowPic",    m_pArrow)
        OBJ_PROP_CLASS  ("CheckSize",   m_szCheck)
        OBJ_PROP_CLASS  ("ArrowSize",   m_szArrow)
        CHAIN_OBJ_PROP  (CTuiButtonImpl)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiMenuItemImpl)
        MESSAGE_HANDLER (CM_ZOOMED,             OnZoomed)
        MESSAGE_HANDLER (CM_MOUSEENTRY,         OnMouseEntry)
        MESSAGE_HANDLER (WM_LBUTTONUP,          OnLButtonUp)
//         MESSAGE_HANDLER (WM_SETFOCUS,               OnSetFocus)
//         MESSAGE_HANDLER (WM_GETDLGCODE,             OnGetDlgCode)
//         MESSAGE_HANDLER (WM_KILLFOCUS,              OnKillFocus)
        MESSAGE_HANDLER (WM_KEYDOWN,            OnKeyDown)
        MESSAGE_HANDLER (CM_INITMENUPOP,        OnInvalidate)
        MESSAGE_HANDLER (CM_UNINITMENUPOP,      OnInvalidate)
        MESSAGE_HANDLER (WM_DESTROY,            OnDestroy)
        CHAIN_MSG_MAP   (CTuiButtonImpl)
    END_TUIMSG_MAP      ()

    TMResult OnZoomed(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseEntry(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnLButtonUp(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnSetFocus(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnKeyDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnKillFocus(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnInitMenuPop(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnUnInitMenuPop(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

public:
    void FinalConstruct(CTuiObject* pp)
    {
        m_pCheck = &GetModule()->m_aPrePic.m_picDftCheck;// .GetPre (IDP_PRE_PIC_CHECK);
        CTuiButtonImpl::FinalConstruct(pp);
    }

    void PopupSubMenu();

    // CTuiPushButtonImpl
    virtual ITuiPic* GetDftPic() { return &GetModule()->m_aPrePic.m_picDftMenuPopItem; }

    // CTuiWgt
    virtual EButStates DoGetStates(HTUI hFocus, HTUI hOver, TMBool bMouseDown) tm_override;
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam);

    // CTuiObject
#ifdef TUI_EDIT_MODE
    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_pCheck.GetObjectID() || n == m_pArrow.GetObjectID() || n == m_nSubMenu)
            return true;
        return CTuiButtonImpl::EditorMatchID(n);
    }
#endif // TUI_EDIT_MODE

};


TM_END_NAMESPACE

#endif // UIBUTTON_H
