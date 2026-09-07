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

#include "Proxy.h"


///////////////////////////////////////////////////////////////////////////////
// CAccComboBox
class TM_NO_VTABLE CAccComboBox : public CAccessibleStyle
{
private:
    TM::CRefPtr<IAccessibleStyle>    m_ptrEdit;

public:
    enum { eIndexSelf = 0, eIndexEdit, eIndexButton, eIndexList, eCount = 3 };

private:
    CAccessibleObj* GetListBoxIAccessible(HTUI hWgt);
    CAccessibleObj* GetButtonIAccessible(HTUI hWgt);

public:
    void Init(TM::CRefPtr<IAccessibleStyle>& pEdit) { m_ptrEdit.Swap(pEdit); }
    virtual long _get_accCtrlCount(CAccessibleWnd& sender) tm_override { return 0; }
    virtual long _get_accItemCount(CAccessibleWnd& sender) tm_override { return eCount; };  //1:edit; 2:button; 3:listbox
    virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet) tm_override;  // return -1: succeeded; else return _get_accItemCount();
    
//     virtual TM::HTUI _get_accParent(CAccessibleWnd& sender);
    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
    virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val) tm_override;
//     virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description) tm_override;
    virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override;
    virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state) tm_override;
    virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut) tm_override;
//     virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild);
//     virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children) { return E_NOTIMPL; }
//     virtual HRESULT _get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction);

//     virtual HRESULT _accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect);
//     virtual HRESULT _accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc) { return E_NOTIMPL; }
//     virtual HRESULT _accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt);
//     virtual void    _accItemHitTest(TM::CTuiPoint& pt, long& child) {}
//     virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex);
};

// extern CAccComboBox g_accComboBox;
