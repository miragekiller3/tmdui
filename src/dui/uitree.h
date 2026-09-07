/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2007.01.31
 *
 *  Copyright (C) 2007 miragekiller
 */

#pragma once

#ifndef UITREE_H
#define UITREE_H

#include "uiwidget.h"


TM_BGN_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
//                  TREEITEM::nTop dirty changeframe m_pFirstVisible m_pLastVisible m_pEnsure
// 
// CTVItem::move      v              v     v           v
// CTVItem::remove    v              v     v           v


////////////////////////////////////////////////////////////////////////////////
// class CTuiTreeImpl
class CTuiTreeImpl;

#define TVIP_DESTROYED           0xF000

struct TREEITEM
    : public CBaseRefT<TREEITEM, CDomNodeHelperT<TREEITEM, ITVItem> >
    , public TM::CRefObjectRoot<1>
    , public CTreeNodeT<TREEITEM*>
    , public IBindOwner
{
public:
    TM_SELF_ALLOC_F(TREEITEM);
#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        if (pbuf)
        {
            TREEITEM* p = (TREEITEM*)cls->m_pClass;
            p->m_gxText.GetString().CopyTo(pbuf, 255);
        }
        return 0;
    }
#endif // TM_DBG


    CTuiTreeImpl*   m_pOwner; // if begin destroy item, m_pOwner set to 0;
    TMUInt16        m_state;
    TMUInt16        m_statePri;
    GXDips          m_nWidth;
    GXDips          m_nHeight;
    GXDips          m_nTop;
    GX::CText       m_gxText;
    CString         m_strHint;
    void*           m_lParam;
    Ref             m_hBind;

    TM_DECLARE_FLAG(Destroyed,  m_statePri, TVIP_DESTROYED);
    TM_DECLARE_FLAG(Disable,    m_state,    TVIS_DISABLE);
    TM_DECLARE_FLAG(Selected,   m_state,    TVIS_SELECTED);
    TM_DECLARE_FLAG(Check,      m_state,    TVIS_CHECKED);
    TM_DECLARE_FLAG(AutoHeight, m_state,    TVIS_AUTOHEIGHT);

    TREEITEM(CTuiTreeImpl* p, const CStrPtr& str, TMUInt uState);
    ~TREEITEM();

//     TM_UNUSED_ADDREF_RELEASE();
    TM_BGN_INTERFACE_MAP(TREEITEM)
    TM_END_INTERFACE_MAP()

    TMBool IsRoot() const { return m_pOwner && !_parent; }
    TMBool IsValidChild() const { return _parent && !IsDestroyed(); }

    // CDomNodeHelperT
    void OnInsertItem(TREEITEM* pInsert);
    TMBool GetRect(TUIRect* lpRect, TMBool bForInvalidate);

    // IDomNode
    virtual CDomStrPtr get_nodeName() tm_override;
    virtual DOMNodeType get_nodeType() tm_override { return NODE_ELEMENT; }
    virtual CDomStrPtr get_nodeValue() tm_override;
    virtual void put_nodeValue(const CDomStrPtr& val) tm_override;
//    virtual TMBool hasAttributes(const value_type& name) tm_override { return false; }
    virtual void removeChild(IDomNode* node) tm_override;

    // ITVItem
    virtual HTUI GetWindow() tm_override;
    virtual GXDips GetHeight() tm_override;
    virtual void SetWidth(GXDips m_nWidth) tm_override;
    virtual void SetHeight(GXDips m_nHeight) tm_override;
    virtual TMBool GetRect(TUIRect* lpRect) tm_final { return GetRect(lpRect, true); }
    virtual TMUInt GetState() tm_override { return m_state; }
    virtual int GetLevel() tm_override;
    virtual void* GetData() tm_override { return m_lParam; }
    virtual void SetData(void* dwData, TMBool bRedraw = true) tm_override;
    virtual void SetHint(const TUISETTEXT& wst) tm_override;
    virtual CStrPtr GetHint() tm_override { return m_strHint; }
    virtual void SetText(const TUISETTEXT& wst) tm_override;

    virtual TMUInt GetChildCount() tm_override;
    virtual TMUInt GetAllChildCount() tm_override;
    virtual TMBool IsChild(ITVItem* hChild)tm_override;
    virtual ITVItem* GetRoot() tm_override;
    virtual ITVItem* GetChildAt(int nIndex) tm_override;
//     virtual ITVItem* GetPrevAt(int nSkip) tm_override;
//     virtual ITVItem* GetNextAt(int nSkip) tm_override;
    virtual ITVItem* GetPrevVisible() tm_override;
    virtual ITVItem* GetNextVisible() tm_override;
    virtual ITVItem* GetGlobalPrev(ITVItem* hParent = 0) tm_override;
    virtual ITVItem* GetGlobalNext(ITVItem* hParent = 0) tm_override;
    virtual ITVItem* InsertNew(const CStrPtr& str, EZOrder eZOrderOrPos, TMUInt uState) tm_override;

    virtual TMBool SetState(TMUInt uState, TMUInt uMask) tm_override;
//     virtual void Expand() tm_override;
    virtual void ExpandAll() tm_override;
//     virtual void Collapse() tm_override;
//     virtual void Toggle() tm_override;
//     virtual void SetCheck(TMBool bCheck) tm_override;
//     virtual void Enable() tm_override;
//     virtual void Disable() tm_override;

    virtual void ShowItem(TMBool bShow, TMBool bEnsure) tm_override;
    virtual TMBool IsVisible() tm_override;

    virtual TMBool IsMouseOver(TMUInt32 dwType = -1) tm_override;

    virtual TMBool EnsureVisible() tm_override;      
    virtual void ScrollToVisible() tm_override;

    virtual EButState GetButtonState(TMUInt32 dwType = -1) tm_override;

    virtual void SelectItem(TMUInt32 dwOption) tm_override;
    virtual TMBool Sort(PFNTVCOMPARE pfn, TMLParam lParam, int nLevel, TMBool bReversed) tm_override;
    virtual TMBool SetZOrder(ITVItem* hIterator, EZOrder eZOrderOrIndex) tm_override;
//     virtual void MoveUp(ITVItem* hBefore) tm_override;
//     virtual void MoveDown(ITVItem* hAfter) tm_override;
//     virtual void SetParent(ITVItem* hParent) tm_override;
    virtual void Invalidate() tm_override;
    virtual void BindItemWindow(HTUI hWnd) tm_override;
    virtual HTUI CreateBindItemWindow(TMUInt nObjID) tm_override;
    virtual HTUI DetachBindItemWindow(HTUI hNewParent) tm_override;
    virtual HTUI GetBindItemWindow() tm_override { return m_hBind; }

    // IBindOwner
    virtual BINDOWNER GetOwner(HTUI hSelf) { return this; }
    virtual TMBool GetRectForInvalidate(HTUI hSelf, CTuiRect& rect) { return GetRect(&rect, true); }

};

// typedef TREEITEM* PTVITEM;
typedef TREEITEM* PTVITEM;


#define TVS_MULTIPLESEL     0x0001
#define TVS_MENUITEM        0x0002
#define TVS_TRACKSEL        0x0004
#define TVS_TRACKMOUSE      0x0010

class TM_NO_VTABLE CTuiTreeImpl
    : public CScrollObject<CTuiTreeImpl, CForePicWin<CTuiWgt> >
{
protected:
    typedef CScrollObject<CTuiTreeImpl, CForePicWin<CTuiWgt> >         scroll_win;
    typedef TMBool (*PFNRebuildProc)(PTVITEM p, TUISize& sz, void* param);
    friend struct TREEITEM;

public:
    TREEITEM        m_tiRoot;
    PFNDELETEPROC   m_pfnDelete;
    ITVEvent*       m_pSink;
//     ITVProvider*    m_pSource;

    PTVITEM         m_pSel;     // handle to selected item or 0 if none
    void*           m_pLdownSel;
    PTVITEM         m_pEnsure;
    PTVITEM         m_pFirstVisible;    // handle to first visible item
    PTVITEM         m_pLastVisible;     // last expanded sibling or child child item of a tree node

    TMUInt16        m_uItemHeight;    /* item height */
    TMUInt16        m_uIndent;

    friend class CTVItem;
    
public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_Style, CSymbol_WsStyle)
        TM_MASK_SYMBOL  (TVS_CHECKBOXES)
        TM_MASK_SYMBOL  (TVS_DISABLEDRAGDROP)
        TM_MASK_SYMBOL  (TVS_EDITLABELS)
        TM_MASK_SYMBOL  (TVS_FULLROWSELECT)
//         TM_MASK_SYMBOL  (TVS_INFOTIP)
        TM_MASK_SYMBOL  (TVS_MULTIPLESEL)
        TM_MASK_SYMBOL  (TVS_MENUITEM)
        TM_MASK_SYMBOL  (TVS_TRACKMOUSE)
        TM_MASK_SYMBOL  (TVS_TRACKSELECT)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    TM_DECLARE_FLAG     (_TVS_TRACKSEL, m_dwStyle, TVS_TRACKSEL);
    TM_DECLARE_FLAG     (_WS_PRI_INVALIDATE, m_dwPriStyle, WS_PRI_INVALIDATE);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_Style)
        OBJ_PROP_INT    ("ItemHeight", m_uItemHeight)
        OBJ_PROP_INT    ("Indent", m_uIndent)
        CHAIN_OBJ_PROP  (scroll_win)
    END_OBJ_PROP_MAP    ()

private:
    TMBool IsMenuItem() const { return m_dwStyle & TVS_MENUITEM; }
    TMBool IsTrackMouse() const { return m_dwStyle & (TVS_MENUITEM | TVS_TRACKMOUSE); }

public:
    GXDips GetScrollWidth();
    GXDips GetScrollHeight();
    void AfterScroll();

public:
    void FreeMem(PTVITEM hItem, TMBool bPrev);
    TMBool RebuildEnsure();
    TMBool RebuildItem(PTVITEM p, TMBool bVisible, TUISize& sz, PFNRebuildProc pfn, void* param);
    void RebuildItemProc(TREEITEM* p, TMBool bVisible, GXDips& cx, GXDips& cy); // return:scroll width
    void RebuildFirst();
    PTVITEM GetFirst();

    void InvalidateItems();
    void InvalidateItems(PTVITEM hItem);
    void InvalidateItemHeightChanged(PTVITEM hItem);
//     void SetHot(PTVITEM hItem);
    void RemoveItem(PTVITEM hItem);
    void Reset();
    void AutoScroll();
    TMBool ExpandMsg(TMUInt flag, PTVITEM hItem);
    PTVITEM GetPrevVisible(PTVITEM hItem);
    PTVITEM GetNextVisible(PTVITEM hItem);
    PTVITEM GetSelfOrNextEnableVisible(PTVITEM hItem);
    PTVITEM GetSelfOrPrevEnableVisible(PTVITEM hItem);
    PTVITEM GetLastVisible();
    TMBool HitTestTree(TUIHITINFO* pht, TMUInt uMsg);
    TMBool HitTest(TUIHITINFO* pht, TMUInt uMsg);
    void DoSelChange(PTVITEM hOld, PTVITEM hNew, TMUInt nKey);
    bool SelectItem(PTVITEM hItem, TMULong dwOption, TMUInt nKey = 0);
    TMBool OnKeyDown(TMWParam wParam, TMWParam lParam);
    void LButtonDown(TMLParam lParam);
    void RButtonDown(TMLParam lParam);
    void LButtonUp(TMLParam lParam);
    void OnMouseSwitch(TUIHITINFO* pOld, TUIHITINFO* pNew);
    void DrawItem(TUIDRAWITEM* p);

    void OnBindModeLButtonDown(TMWParam wParam, TMLParam lParam);
//     void OnBindModeLButtonUp(TMWParam wParam, TMLParam lParam);
    inline void OnBindModeRButtonDown(TMWParam wParam, TMLParam lParam) { OnBindModeLButtonDown(wParam, lParam); }
    inline TMBool OnBindModeKeyDown(int key, TMLParam lParam) { return OnKeyDown(key, lParam); }
    void NativePrintClient(IGXApp* gapp, TMLParam lParam);

public:
    CTuiTreeImpl ();
    ~CTuiTreeImpl ();
    void FinalConstruct(CTuiObject* pp)
    {
        m_pFore = GetPrePic(LISTBOX); //&GetModule()->m_aPrePic.m_picDftListItem;
        scroll_win::FinalConstruct(pp);
    }

    // CTuiWgt
    TMBool ProcessWindowMessage(CTuiWgt* hWnd, TMUInt uMsg, TMWParam wParam, 
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) tm_override;
    virtual TMBool NativeInvalidateRect(SPY_INVALIDATE_PARAM(const TUIRect& rc, TM::HTUI hSender)) tm_override;
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override;
    virtual bool GetWindowHint(enum ETTActive ett, CString* str) tm_override;
    virtual void Reclaim() tm_override;

    // CTuiObject
#ifdef TUI_EDIT_MODE
    CString GetDebugInfo() tm_override;
#endif // TUI_EDIT_MODE

};



TM_END_NAMESPACE


#endif // UITREE_H
