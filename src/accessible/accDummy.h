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

#ifndef ACCDUMMY_H
#define ACCDUMMY_H

#include "Proxy.h"


///////////////////////////////////////////////////////////////////////////////
// CAccDummy
class CAccDummy : public CAccessibleStyle
{
public:
    virtual long _get_accItemCount(CAccessibleWnd& sender) tm_override;
    virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet) tm_override;  // return -1: succeeded; else return _get_accItemCount();
    
//     virtual TM::HTUI _get_accParent(TM::HTUI hWnd);
//     virtual HRESULT _get_accName(TM::HTUI hWnd, long nIndex, CComBSTR& name);
//     virtual HRESULT _get_accValue(TM::HTUI hWnd, long nIndex, CComBSTR& val);
//     virtual HRESULT _get_accDescription(TM::HTUI hWnd, long nIndex, CComBSTR& description);
//     virtual long    _get_accRole(TM::HTUI hWnd, long nIndex);
//     virtual HRESULT _get_accState(TM::HTUI hWnd, long nIndex, long& state);
//     virtual HRESULT _get_accKeyboardShortcut(TM::HTUI hWnd, long nIndex, CComBSTR& szKeyboardShortcut);
//     virtual HRESULT _get_accFocus(TM::HTUI hWnd, VARIANT* pvarFocusChild);
//     virtual HRESULT _get_accSelection(TM::HTUI hWnd, long& children) { return E_NOTIMPL; }
//     virtual HRESULT _get_accDefaultAction(TM::HTUI hWnd, long nIndex, CComBSTR& szDefaultAction);

//     virtual HRESULT _accSelect(TM::HTUI hWnd, long nIndex, long flagsSelect);
//     virtual HRESULT _accItemLocation(TM::HTUI hWnd, long nIndex, TM::CTuiRect& rc) { return E_NOTIMPL; }
//     virtual HRESULT _accNavigate(TM::HTUI hWnd, long navDir, long varStart, VARIANT* pvarEndUpAt);
//     virtual void    _accItemHitTest(TM::CTuiPoint& pt, long& child) {}
//     virtual HRESULT _accDoDefaultAction(TM::HTUI hWnd, long nIndex);
};

extern CAccDummy g_accDummy;


#endif