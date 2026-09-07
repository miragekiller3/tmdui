/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.08.25
 *
 *  Copyright (C) 2009-2018 miragekiller
 */

#ifndef ACCTREEVIEW_H
#define ACCTREEVIEW_H

#include "Proxy.h"



///////////////////////////////////////////////////////////////////////////////
// CAccTreeView
class CAccTreeView : public CAccessibleStyle
{
public:
    static HRESULT GetGroupPosition(CAccessibleWnd& wnd, long* groupLevel, long* similarItemsInGroup, long* positionInGroup);
    static UINT GetIndexByItem(TM::CTVItem item);
    static TM::CTVItem GetItemByIndex(CAccessibleWnd& sender, UINT uIndex);

public:
    // self
    virtual bool _tree_isFocus(HTUI hWnd) { return 0 != TM::CTuiWidget(hWnd).IsSelfOrChild(_tuiApp.GetFocus()); }
    
    // IAccessibleStyle
    virtual long _get_accItemCount(CAccessibleWnd& sender) tm_override;
    virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet) tm_override;  // return -1: succeeded; else return _get_accItemCount();
    
//     virtual TM::HTUI _get_accParent(CAccessibleWnd& sender);
    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
    virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val) tm_override;
//     virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description) tm_override;
    virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override;
    virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state) tm_override;
//     virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut);
    virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild) tm_override;
    virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children) tm_override;
    virtual HRESULT _get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction) tm_override;
    
    virtual HRESULT _accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect) tm_override;
    virtual HRESULT _accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc) tm_override;
    virtual HRESULT _accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt) tm_override;
    virtual HRESULT _accItemHitTest(CAccessibleWnd& sender, TM::CTuiPoint& pt, long& child) tm_override;
    virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex) tm_override;

//     virtual HRESULT _get_accEnum(CAccessibleWnd& sender, IEnumVARIANT** ppEnum) tm_override;
//     virtual TMULong CopyTo(CAccessibleWnd& sender, int nCurPos, VARIANT* pVars, TMULong cVar) tm_override;
};


///////////////////////////////////////////////////////////////////////////////
// CAccTreeItem
class CAccTreeItem : public CAccessibleStyleItemBase
{
public:
    HRESULT GetGroupPosition(CAccessibleWnd& wnd, long* groupLevel, long* similarItemsInGroup, long* positionInGroup);

public:
    static UINT GetIndexByItem(TM::CTVItem item);
    static TM::CTVItem GetItemByIndex(CAccessibleWnd& sender, UINT uIndex);

public:
    //virtual long _get_accIndex() tm_override { return m_sel; }
    //virtual long _get_accItemCount(CAccessibleWnd& sender) tm_override;
    //virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet) tm_override;  // return -1: succeeded; else return _get_accItemCount();

    //virtual HTUI    _get_accParent(CAccessibleWnd& sender) tm_override { return sender; }
    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
    virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val) tm_override;
    //     virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description) tm_override;
    virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override;
    virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state) tm_override;
    //     virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut);
    //virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild) tm_override;
    //virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children) tm_override;
    virtual HRESULT _get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction) tm_override;

    virtual HRESULT _accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect) tm_override;
    virtual HRESULT _accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc) tm_override;
    virtual HRESULT _accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt) tm_override;
    virtual HRESULT _accItemHitTest(CAccessibleWnd& sender, TM::CTuiPoint& pt, long& child) tm_override;
    virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex) tm_override;

    //     virtual HRESULT _get_accEnum(CAccessibleWnd& sender, IEnumVARIANT** ppEnum) tm_override;
    //     virtual TMULong CopyTo(CAccessibleWnd& sender, int nCurPos, VARIANT* pVars, TMULong cVar) tm_override;
};


///////////////////////////////////////////////////////////////////////////////
// CAccCombTree
class CAccCombTree : public CAccTreeView
{
public:
    // self
    virtual bool _tree_isFocus(HTUI hWnd) tm_final { return CAccessibleObj::CombUtilIsFocus(hWnd, eCombList, _tuiApp.GetFocus()); }
    
    // IAccessibleStyle
    //virtual long _get_accItemCount(CAccessibleWnd& sender) tm_override;
    //virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet) tm_override;  // return -1: succeeded; else return _get_accItemCount();

    virtual TM::HTUI _get_accParent(CAccessibleWnd& sender);
    //virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
    //virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val) tm_override;
    //     virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description) tm_override;
    //virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override;
    //virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state) tm_override;
    //     virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut);
    //virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild) tm_override;
    //virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children) tm_override;
    //virtual HRESULT _get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction) tm_override;

    //virtual HRESULT _accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect) tm_override;
    //virtual HRESULT _accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc) tm_override;
    //virtual HRESULT _accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt) tm_override;
    //virtual HRESULT _accItemHitTest(CAccessibleWnd& sender, TM::CTuiPoint& pt, long& child) tm_override;
    //virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex) tm_override;

    //     virtual HRESULT _get_accEnum(CAccessibleWnd& sender, IEnumVARIANT** ppEnum) tm_override;
    //     virtual TMULong CopyTo(CAccessibleWnd& sender, int nCurPos, VARIANT* pVars, TMULong cVar) tm_override;
};



extern CAccTreeView g_accTreeView;
extern CAccTreeItem g_accTreeSelItem;
extern CAccCombTree g_accCombTreeView;


#endif