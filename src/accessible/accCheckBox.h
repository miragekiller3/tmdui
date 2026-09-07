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

#ifndef ACCCHECKBOX_H
#define ACCCHECKBOX_H

#include "Proxy.h"


///////////////////////////////////////////////////////////////////////////////
// CAccCheckBox
class CAccCheckBox : public CAccessibleStyleButtonBase
{
public:
//     virtual long _get_accItemCount(CAccessibleWnd& sender) { return 0; };  // child item count; list, tree
//     virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet) { return 0; };  // return -1: succeeded; else return _get_accItemCount();
//     
//     virtual TM::HTUI _get_accParent(CAccessibleWnd& sender) { return TM::CTuiWidget(hWnd).GetParent(); }
//     virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name);
//     virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val) { return S_OK; }
//     virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description) { return E_NOTIMPL; }
    virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override { return ROLE_SYSTEM_CHECKBUTTON; }
//     virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state);
//     virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut);
//     virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild);
//     virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children) { return E_NOTIMPL; }
    virtual HRESULT _get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction) tm_override;
    
//     virtual HRESULT _accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect);
//     virtual HRESULT _accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc) { return E_NOTIMPL; }
//     virtual HRESULT _accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt);
//     virtual void    _accItemHitTest(TM::CTuiPoint& pt, long& child) {}
//     virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex);
};

extern CAccCheckBox g_accCheckBox;

#endif