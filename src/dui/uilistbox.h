/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2008.08.21
 *
 *  Copyright (C) 2008 miragekiller
 */

#pragma once

#ifndef UILISTBOX_H
#define UILISTBOX_H

#include "uiwidget.h"


TM_BGN_NAMESPACE

class CTuiListBoxImpl;
class CTuiHeaderImpl;


// CColumnImpl
class TM_NO_VTABLE CColumnImpl : public IColumn
{
protected:
    CTuiHeaderImpl* m_pHeader;
    IColumnProxy*   m_proxy;
    int             m_nCurColumn;   // -1: no header or no LBS_COLUMNSEL;

    CColumnImpl() : m_pHeader(0), m_proxy(0), m_nCurColumn(-1) {}

    virtual int GetColumnCount() tm_override;
    virtual TMLParam GetColumnData(int nCol) tm_override;
    virtual GXDips GetColumnWidth(int nCol) tm_override;
    virtual int GetColumnFormat(int nCol) tm_override;
};


////////////////////////////////////////////////////////////////////////////
// LBITEM
struct TM_NO_VTABLE LBITEM_Base
    : public IBindOwner
    , public CBindWidgetImpl
{
    TMLParam        lParam;
    CText           gxText;
    CString         strAcc;
    CString         strHint;
    bool            bSel;
    bool            bEnable;

    LBITEM_Base()
        : lParam(0)
        , bSel(false)
        , bEnable(true)
    {
    }

    void ResetBind()
    {
        if (!m_hBind)
            return;
        DbgAssert(m_hBind->m_pBind);
        m_hBind->m_pBind = this;
    }
};

struct LBITEM : public LBITEM_Base
{
public:
    LBITEM() {}
    LBITEM (const CString& str) { gxText.ReCreate(str, false); }

    LBITEM(const LBITEM& rhs) : LBITEM_Base(rhs) { ResetBind(); }
    LBITEM& operator = (const LBITEM& rhs)
    {
        LBITEM_Base::operator = (rhs);
        ResetBind();
        return *this;
    }

    void InitString(HTUI hWnd, TMLParam lp)
    {
        CString str;
        hWnd->StringMessageToString(lp, str);
        gxText.ReCreate(str, ((TUISETTEXT*)lp)->bHtml);
    }

    CStrPtr strText() const { return gxText.GetString(); }

    bool operator < (const LBITEM& rhs) const { return strText().CompareNoCase(rhs.strText()) < 0; }

    // IBindOwner
    virtual BINDOWNER GetOwner(HTUI hSelf) tm_final { return BINDOWNER(this); }
    virtual TMBool GetRectForInvalidate(HTUI hSelf, CTuiRect& rect) tm_final;
};


////////////////////////////////////////////////////////////////////////////
// CTuiListBoxImpl
class TM_NO_VTABLE CTuiListBoxImpl
    : public CForePicWin<CTuiWgt>
    , public CColumnImpl
{
public:
#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        return CForePicWin<CTuiWgt>::IC_GetDebugInfo(cls, pbuf) + IC_Size(m_aItem);
    }
#endif // TM_DBG

private:
    typedef CVector<LBITEM>             CItemList;
    typedef CItemList::recorder         CItemRec;
    friend CWidgetHelper;
    friend LBITEM;
    friend struct _CTuiListBoxImpl;

    PFNDELETEPROC   m_pfnDelete;
    PFNBINDBACKUP   m_pfnBindBackup;
    CPropColor      m_clrNormal;
    CPropColor      m_clrSelect;
    CPropColor      m_clrHilight;
    CPropColor      m_clrFocus;
    CItemList       m_aItem;
    int             m_LdownSel;
    int             m_nCurSel;
    int             m_nFirstVisible;
    int             m_nEndVisible;
    TMInt16         m_nItemHeight;
    TMUInt16        m_uIndent;

public: 
    TM_BGN_MASK_SYMBOL2 (CSymbol_Style, CSymbol_WsStyle)
        TM_MASK_SYMBOL2 (LBS_EXTENDEDSEL,   LBS_SEL_MASK | LBS_TRACKSEL | LBS_MENUITEM)
        TM_MASK_SYMBOL2 (LBS_MULTIPLESEL,   LBS_SEL_MASK | LBS_TRACKSEL | LBS_MENUITEM)
        TM_MASK_SYMBOL2 (LBS_NOSEL,         LBS_SEL_MASK | LBS_TRACKSEL | LBS_MENUITEM)
        TM_MASK_SYMBOL2 (LBS_MENUITEM,      LBS_SEL_MASK | LBS_MENUITEM)
        TM_MASK_SYMBOL2 (LBS_TRACKSEL,      LBS_SEL_MASK | LBS_TRACKSEL)
        TM_MASK_SYMBOL  (LBS_SORT)
        TM_MASK_SYMBOL  (LBS_COLUMNSEL)
//         TM_MASK_SYMBOL  (LBS_HASSTRINGS)
//         TM_MASK_SYMBOL  (LBS_NOTIFY)
//         TM_MASK_SYMBOL  (LBS_INFOTIP)
//         TM_MASK_SYMBOL  (LBS_DISABLENOSCROLL)
//         TM_MASK_SYMBOL  (LBS_WANTKEYBOARDINPUT)
        TM_MASK_SYMBOL  (LBS_TRACKMOUSE)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    TM_DECLARE_FLAG     (_LBS_TRACKMOUSE,   m_dwStyle, LBS_TRACKMOUSE);
//     TM_DECLARE_FLAG     (Sort,          m_dwStyle, LBS_SORT);
//     TM_DECLARE_FLAG     (InfoTip,       m_dwStyle, LBS_INFOTIP);
    TM_DECLARE_FLAG     (_LBS_TRACKSEL,     m_dwStyle, LBS_TRACKSEL);
    TM_DECLARE_FLAG     (_LBS_MENUITEM,     m_dwStyle, LBS_MENUITEM);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style",               m_dwStyle, CSymbol_Style)
        OBJ_PROP_CLASS  ("NormalTextColor",     m_clrNormal)
        OBJ_PROP_CLASS  ("SelectTextColor",     m_clrSelect)
        OBJ_PROP_CLASS  ("HighlightTextColor",  m_clrHilight)
        OBJ_PROP_CLASS  ("FocusTextColor",      m_clrFocus)
        OBJ_PROP_INT    ("ItemHeight",          m_nItemHeight)
        OBJ_PROP_INT    ("Indent",              m_uIndent)
        CHAIN_OBJ_PROP  (CForePicWin<CTuiWgt>)
    END_OBJ_PROP_MAP    ()
    
public:
    CTuiListBoxImpl();
    ~CTuiListBoxImpl();
    void FinalConstruct(CTuiObject* pp)
    {
        m_pFore = GetPrePic(LISTBOX); //&GetModule()->m_aPrePic.m_picDftMenuBarItem;
        CForePicWin<CTuiWgt>::FinalConstruct(pp);
    }

public:
    GXDips GetVScrollLine() const { return m_nItemHeight; }

protected:
    // CTuiWgt
    TMBool ProcessWindowMessage(CTuiWgt* hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) tm_override;
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override;
    virtual bool GetWindowHint(ETTActive ett, CString* str) tm_override;
    virtual void Reclaim() tm_override;

    // CTuiObject
#ifdef TUI_EDIT_MODE
    CString GetDebugInfo() tm_override;
#endif // TUI_EDIT_MODE

//     void DoBindWin(int nItem, HTUI hWnd, HTUI hParent);
    HTUI CreateBindWin(TMWParam nIndex, TMLParam dwObjID);
    TMBool BindWin(TMWParam nIndex, HTUI hNewParent);
    HTUI DetachBindWin(TMWParam nIndex, HTUI hWnd);
    void OnHitTest(TMWParam wParam, TUIHITINFO& hit);
    void AutoScroll();
    TMResult SelItem(int nItem, int nColumn, TMUInt32 dwStyle, TMUInt16 nModifier, TMBool bEnsureVisible);
    void OnLButtonDown(TMWParam wParam, TMLParam lParam);
    void OnMouseSwitch(TMWParam wParam, TMLParam lParam);
//     void OnPaint(TMHDC hDC);
    TMBool EnableItem(int nIndex, TMBool bEnable = TMTrue);
    void NativePrintClient(IGXApp* gapp, TMLParam lParam);
    void OnDrawItem(TMWParam wParam, TMLParam lParam);
//     void OnMouseMove(TMWParam wParam,TMLParam lParam); 
//     TMResult OnDrag(TMUInt uMsg, TMWParam wParam, TMLParam lParam);
    TMBool OnKeyDown(int key, TMLParam lParam);
    int AddString(const LBITEM& itm);
    int InsertString(int nIndex, TMLParam lp);
    int DeleteString(int nIndex);
    void ModifyString(int nIndex, const CString& str);
    void ModifyString(int nIndex, TMLParam lp);
    int FindString(int nStart, TMCStr lStr, TMBool bSel, TMBool bExact);
    TMResult GetItemRect(TMULong n, CTuiRect* prc, TMBool bForInvalidate = false);
    void InvalidateHT (TUIHITINFO* p);

    int SelItemRange(bool bSelect, int nFirst, int nLast); 
    
    int GetCount() const { return m_aItem.size(); }
    void OnSort(PFNLBCOMPARE pfn, TMLParam lp);
    int  OnEnum(PFNLBENUM pfn, TMLParam lp);
    void ReclaimItems(TMUInt32 nFirst, TMUInt32 nLast);
    void CleanItem(LBITEM& itm, int idx) { CleanItem(itm.lParam, itm.m_hBind, idx); }
    void CleanItem(TMLParam lParam, HTUI hBind, int idx);
    bool ExportToCSV(IStream* stream, const CStringA& StrSplit);

    inline void OnBindModeLButtonDown(TMWParam wParam, TMLParam lParam) { OnLButtonDown(wParam, lParam); }
    inline void OnBindModeRButtonDown(TMWParam wParam, TMLParam lParam) {}
    inline void OnBindModeMouseSwitch(TMWParam wParam, TMLParam lParam) {} // OnMouseSwitch(wParam, lParam); } 
    inline TMBool OnBindModeKeyDown(int key, TMLParam lParam) { return OnKeyDown(key, lParam); }
    
//     int GetItem(GXDips nPosY);
};


    
TM_END_NAMESPACE

#endif // UILISTBOX_H
    
