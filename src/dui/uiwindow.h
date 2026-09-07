/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.14
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef UIWINDOW_H
#define UIWINDOW_H

#include "uilayerwnd.h"
#include "input.h"


TM_BGN_NAMESPACE


class CTuiMenuBarImpl;
class CTuiWnd_Win;


class TM_NO_VTABLE CTuiWnd
    : public CTuiLayerWnd
{
    TM_NO_COPYABLE(CTuiWnd)

public:
    friend class CWidgetHelper;
    friend class CTuiWgt;
    friend class CTuiWidget;
    friend class CTuiDummyBase;
    friend class CTuiMenuBarImpl;
    typedef CSubClassMgr<TMHWnd>      CWndProcMgr;

#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        return m_aHotKeyMgr.IC_GetDebugInfo(cls, pbuf)
            + CTuiLayerWnd::IC_GetDebugInfo(cls, pbuf)
            + IC_Size(m_mgrWndProc.m_aPorc);
    }
#endif // TM_DBG

public:
    CAutoTuiWidget  m_wndFocusBackup;
    CHotKeyMap      m_aHotKeyMgr;
    TMUInt32        m_nDefaultItem;
//     CTuiWnd_Win* m_pReal;

public:
    CWndProcMgr     m_mgrWndProc;

    enum { eObjType = e_otWnd };

public:
    TM_BGN_MASK_SYMBOL  (CSymbol_WsStyle)
        TM_MASK_SYMBOL  (WS_CHILD)
        TM_MASK_SYMBOL  (WS_TABSTOP)
        TM_MASK_SYMBOL  (WS_CAPTION)
        TM_MASK_SYMBOL  (WS_BORDER)
        TM_MASK_SYMBOL  (WS_DLGFRAME)
        TM_MASK_SYMBOL  (WS_MINIMIZEBOX)
        TM_MASK_SYMBOL  (WS_MAXIMIZEBOX)
        TM_MASK_SYMBOL  (WS_SYSMENU)
        TM_MASK_SYMBOL  (WS_SIZEBOX)

        TM_MASK_SYMBOL  (WS_DISABLED)
        TM_MASK_SYMBOL  (WS_VISIBLE)

        TM_MASK_BREAK   ()
        TM_MASK_SYMBOL  (DS_FULLSCREEN)
        TM_MASK_SYMBOL  (DS_NODWMSHADOW)
        TM_MASK_SYMBOL2 (DS_CENTERSCREEN,   DS_CENTEROWNER | DS_CENTERSCREEN)
        TM_MASK_SYMBOL2 (DS_CENTEROWNER,    DS_CENTEROWNER | DS_CENTERSCREEN)
        TM_MASK_SYMBOL  (DS_DWMBLUR)
        TM_MASK_SYMBOL  (DS_NODWMTRANSPARENT)
        

        TM_MASK_BREAK   ()
        TM_MASK_SYMBOL2 (SS_WINDOWBKONHC,   SS_BKONHCMASK)
        TM_MASK_SYMBOL2 (SS_DLGBKONHC,      SS_BKONHCMASK)
        TM_MASK_SYMBOL2 (SS_CLEARBKONHC,    SS_BKONHCMASK)
        TM_MASK_SYMBOL2 (SS_NULLBKONHC,     SS_BKONHCMASK)
        TM_MASK_SYMBOL  (SS_NULLBKONNORMAL)
    TM_END_MASK_SYMBOL  ()

    TM_BGN_MASK_SYMBOL  (CSymbol_AdvStyle) //, WS_ADV_NORMASTYLEMASK)
        TM_MASK_SYMBOL  (WS_ADV_L2RHINT)
        TM_MASK_SYMBOL  (WS_ADV_CLIENTEDGE)
        TM_MASK_SYMBOL  (WS_ADV_PENETRATE)
        TM_MASK_SYMBOL  (WS_ADV_CONTEXTHELP)
        TM_MASK_SYMBOL  (WS_ADV_TOOLWINDOW)
        TM_MASK_SYMBOL  (WS_ADV_TOPMOST)
        TM_MASK_SYMBOL  (WS_ADV_ARROWGROUP)
        TM_MASK_SYMBOL  (WS_ADV_TABGROUP)
        TM_MASK_SYMBOL  (WS_ADV_LAYERED)
        TM_MASK_SYMBOL  (WS_ADV_NOACTIVATE)
    TM_END_MASK_SYMBOL  ()

    TM_DECLARE_FLAG     (_WS_ADV_LAYERED,       m_dwAdvStyle,   WS_ADV_LAYERED);

    TM_DECLARE_FLAG     (_DS_DWMBLUR,           m_dwStyle,      DS_DWMBLUR);
    TM_DECLARE_NOFLAG   (StyleDWMTransparent,   m_dwStyle,      DS_NODWMTRANSPARENT);
    TM_DECLARE_FLAG     (_DS_FULLSCREEN,        m_dwStyle,      DS_FULLSCREEN);
    TM_DECLARE_FLAG     (_DS_CENTEROWNER,       m_dwStyle,      DS_CENTEROWNER);
    TM_DECLARE_FLAG     (_DS_CENTERSCREEN,      m_dwStyle,      DS_CENTERSCREEN);

    TM_DECLARE_FLAG     (_MS_PRI_HIDEFOCUS,      m_dwPriStyle,  MS_PRI_HIDEFOCUS);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_WsStyle)
        OBJ_PROP_MASK   ("AdvStyle", m_dwAdvStyle, CSymbol_AdvStyle)
        OBJ_PROP_WIN    ("DefaultItem", m_nDefaultItem)
        CHAIN_OBJ_PROP  (CTuiLayerWnd)
    END_OBJ_PROP_MAP    ()

public:
    CTuiWnd();
    ~CTuiWnd();

public:
    TMBool TranslateAcceleratorKey(TMMSG* pMsg);
    TMBool InitManager(TUIFACTORY* pFactory, void* param);
    void SubclassMgr(const CWindowMsgDelegate& func) { m_mgrWndProc.Subclass(MgrGetHWND(), func, this); }
    bool UnsubclassMgr(const CWindowMsgDelegate& func) { return m_mgrWndProc.Unsubclass(MgrGetHWND(), func); }
    void SetDefaultFocus(TMUInt uShowFlag); // SWP_SHOWWINDOW, SWP_HIDEWINDOW, SWP_NOACTIVATE
    TMUInt32 GetDefualtItemID() const { return m_nDefaultItem; }
    HTUI GetDefaultItem() { return GetDlgItem(m_nDefaultItem); }
    void InitWndFocusBackup();

    // CTuiWgt
    virtual TMBool ProcessWindowMessage(CTuiWgt* hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) tm_override;
    virtual CFrame* GetFrame() tm_override { return GetReal()->GetFrame(); }
    virtual TMBool IsHideFocus() const tm_override { return Is_MS_PRI_HIDEFOCUS(); } // m_dwUiState & UISF_HIDEFOCUS; }

    // MgrXXX
    virtual TMHWnd MgrGetHWND () const = 0;
    virtual void MgrSetHideFocus() {}
    virtual TMBool MgrIsAllStyle(TMUInt32 dwStyle, TMUInt32 dwMask) { return (dwMask & WS_VISIBLE); }

protected:
    // CTuiObject
#ifdef TUI_EDIT_MODE
    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_nDefaultItem)
            return true;
        return CTuiLayerWnd::EditorMatchID(n);
    }

    CString GetDebugInfo() tm_override;
#endif // TUI_EDIT_MODE

}; // CTuiWnd


typedef CObjPtr<CTuiWgt>        CWinPtr;
typedef CObjPtr<CTuiWnd>        CMgrPtr;



TM_END_NAMESPACE

#endif // UIWINDOW_H
