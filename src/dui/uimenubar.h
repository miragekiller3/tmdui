/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2008.08.07
 *
 *  Copyright (C) 2008 miragekiller
 */

#pragma once

#ifndef UIMENUBAR_H
#define UIMENUBAR_H

#include "uiwidget.h"
#include "../tmhook.h"



TM_BGN_NAMESPACE



/////////////////////////////////////////////////////////////////////////////////
//CTuiMenuBarImpl

// MS_PRI_INALT: m_nPop is current hot item
// MS_PRI_INPOP: m_nPop is current pop item, m_nHot is next pop item.

// mgr::MenuBarWindowProc: all mode
// CHookGetMessageEx::PreTranslateMessage: on normal mode and alt mode when dialog activate.
// CHookMsgFilter::OnMsgFilter: on pop mode.


class TM_NO_VTABLE CTuiMenuBarImpl
    : public CForePicWin<CTuiWgt>
    , public CHookGetMessageExImpl<CTuiMenuBarImpl>
{
public:
    struct ITEM
    {
        GXDips      nRight;
        GX::CText   gxText;
    };

    typedef CVector<ITEM>           CWidthList;
    typedef CWidthList::recorder    CWidthRec;
    friend struct _TraceMenu;

#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        return CForePicWin<CTuiWgt>::IC_GetDebugInfo(cls, pbuf) + IC_Size(m_aRight);
    }
#endif // TM_DBG

protected:
    CPropMenu       m_menu;
    CWidthList      m_aRight;
    CPropColor      m_clrNormal;
    CPropColor      m_clrHilight;
    int             m_nPop;
    BORDER          m_bdItem;

    static TMBool MenuBarWindowProc(
        CTuiMenuBarImpl* pBar, TMHWnd hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID);

public:
    enum
    {
        MS_AUTOHIDE     = 0x00001000,
    };

    TM_BGN_MASK_SYMBOL2 (CSymbol_MStyle, CSymbol_WsStyle)
        TM_MASK_SYMBOL  (MS_AUTOHIDE)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    TM_DECLARE_FLAG     (PriInPop,      m_dwPriStyle,   MB_PRI_INPOP);
    TM_DECLARE_FLAG     (PriInAlt,      m_dwPriStyle,   MB_PRI_INALT);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_CLASS  ("Menu",                        m_menu)
        OBJ_PROP_CLASS  ("TextColor",                   m_clrNormal)
        OBJ_PROP_CLASS  ("HighlightTextColor",          m_clrHilight)
        OBJ_PROP_MASK   ("Style",                        m_dwStyle, CSymbol_MStyle);
        OBJ_PROP_CLASS  ("ItemPadding",                 m_bdItem)
        CHAIN_OBJ_PROP  (CForePicWin<CTuiWgt>)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiMenuBarImpl)
        MESSAGE_HANDLER (WM_NCCREATE,                   OnCreate)
        MESSAGE_HANDLER (WM_DESTROY,                    OnDestroy)
        MESSAGE_HANDLER (WM_SHOWWINDOW,                 OnEnable)
        MESSAGE_HANDLER (CM_SHOWWINDOW_P,               OnEnable)
        MESSAGE_HANDLER (WM_ENABLE,                     OnEnable)
        MESSAGE_HANDLER (CM_ENABLE_P,                   OnEnable)
        MESSAGE_HANDLER (CM_HITTEST,                    OnHitTest)
        MESSAGE_HANDLER (WM_MOUSELEAVE,                 OnMouseLeave)
        MESSAGE_HANDLER (CM_MOUSEENTRY,                 OnMouseEntry)
        MESSAGE_HANDLER (WM_LBUTTONDOWN,                OnLButtonDown)
        MESSAGE_HANDLER (WM_LBUTTONUP,                  OnLButtonUp)
        MESSAGE_HANDLER (CM_LANGUAGECHANGED,            OnLanguageChanged)
        MESSAGE_HANDLER (MB_GETCURSEL,                  OnGetCurSel)
        MESSAGE_HANDLER (MB_GETMENU,                    OnGetMenu)
        MESSAGE_HANDLER (MB_SETMENU,                    OnSetMenu)
        MESSAGE_HANDLER (MB_SETHMENU,                   OnSetHMenu)
        MESSAGE_HANDLER (MB_GETMENUINFO,                OnGetMenuInfo)
        MESSAGE_HANDLER (MB_GETITEMCOUNT,               OnGetMenuItemCount)
        MESSAGE_HANDLER (MB_SETALTMODE,                 OnSetAltMode)
        
        CHAIN_MSG_MAP   (CForePicWin<CTuiWgt>)
    END_TUIMSG_MAP()

    TMResult OnCreate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnEnable(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnHitTest(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnLButtonDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnLButtonUp(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseEntry(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseLeave(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetCurSel(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return (TMResult)m_nPop; }
    TMResult OnGetMenu(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return (TMResult)m_menu.hMenu; }
    TMResult OnSetMenu(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetHMenu(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetMenuInfo(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetMenuItemCount(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return m_aRight.size(); }
    TMResult OnSetAltMode(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnLanguageChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

public:
    CTuiMenuBarImpl();
    void FinalConstruct(CTuiObject* pp)
    {
        m_pFore = GetPrePic(MENU);// &GetModule()->m_aPrePic.m_picDftListItem;
        CForePicWin<CTuiWgt>::FinalConstruct(pp);
    }

    int HitTest(const TUIPoint& pt, TMBool& bHandled);
    CPoint GetTrackMenuPos(int nHot);
    void OnLoadProp (IDomElement* pXml)
    {
        m_aRight.clear();
        CTuiWgt::OnLoadProp(pXml);
    }

protected:
    TMBool OnSysChar(TMCharT c);
    void ShowMenuItem(int nPos);
    TMBool EntryAltMode();
    void LeaveAltMode();
    void InternalSetPop(int nPos);

public:
    // CHookGetMessageEx
    BOOL PreTranslateMessage(MSG* pMsg);

    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override;
    virtual void Reclaim() tm_override;
};


TM_END_NAMESPACE

#endif // UIMENUBAR_H
