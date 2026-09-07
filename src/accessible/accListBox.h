/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.08.25
 *
 *  Copyright (C) 2009-2018 miragekiller
 */

#pragma once

#ifndef ACCLISTBOX_H
#define ACCLISTBOX_H

#include "Proxy.h"


///////////////////////////////////////////////////////////////////////////////
// CAccListBox
class CAccListBox : public CAccessibleStyle
{
public:
    static HRESULT GetGroupPosition(CAccessibleWnd& wnd, long* groupLevel, long* similarItemsInGroup, long* positionInGroup);

    // self
    virtual bool _list_isFocus(HTUI hWnd) { return 0 != TM::CTuiWidget(hWnd).IsSelfOrChild(_tuiApp.GetFocus()); }

    // IAccessibleStyle
    virtual long _get_accItemCount(CAccessibleWnd& sender) tm_override;
    virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet) tm_override;  // return -1: succeeded; else return _get_accItemCount();

//     virtual TM::HTUI _get_accParent(CAccessibleWnd& sender);
    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
//     virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val);
//     virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description);
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
};


///////////////////////////////////////////////////////////////////////////////
// CAccListItem
class CAccListItem : public CAccessibleStyleItemBase
{
public:
    HRESULT GetGroupPosition(CAccessibleWnd& wnd, long* groupLevel, long* similarItemsInGroup, long* positionInGroup);

public:
    //virtual long _get_accIndex() tm_override { return m_sel; }
    //virtual long _get_accItemCount(CAccessibleWnd& sender) tm_override;
    //virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet) tm_override;  // return -1: succeeded; else return _get_accItemCount();

    //virtual HTUI    _get_accParent(CAccessibleWnd& sender) tm_override { return sender; }
    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
    //     virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val);
    //     virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description);
    virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override { return ROLE_SYSTEM_LISTITEM; }
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
};

///////////////////////////////////////////////////////////////////////////////
// CAccCombList
class CAccCombList : public CAccListBox
{
public:
    // self
    virtual bool _list_isFocus(HTUI hWnd) tm_final { return CAccessibleObj::CombUtilIsFocus(hWnd, eCombList, _tuiApp.GetFocus()); }

    // IAccessibleStyle
    virtual TM::HTUI _get_accParent(CAccessibleWnd& sender);
    //virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
    //     virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val);
    //     virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description);
    //virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override;
    //virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state) tm_override;
    ////     virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut);
    //virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild) tm_override;
    //virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children) tm_override;
    //virtual HRESULT _get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction) tm_override;

    //virtual HRESULT _accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect) tm_override;
    //virtual HRESULT _accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc) tm_override;
    //virtual HRESULT _accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt) tm_override;
    //virtual HRESULT _accItemHitTest(CAccessibleWnd& sender, TM::CTuiPoint& pt, long& child) tm_override;
    //virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex) tm_override;
};


extern CAccListBox   g_accListBox;
extern CAccListItem  g_accListSelItem;
extern CAccCombList   g_accCombListBox;


#endif