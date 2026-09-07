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

#ifndef ACCMGR_H
#define ACCMGR_H

#include "Proxy.h"


///////////////////////////////////////////////////////////////////////////////
// CAccMgr
class CAccMgr : public CAccessibleStyle
{
public:
//     virtual long _get_accItemCount(CAccessibleWnd& sender);
//     virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet);  // return -1: succeeded; else return _get_accItemCount();
    
//     virtual TM::HTUI _get_accParent(CAccessibleWnd& sender);
    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
//     virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val);
//     virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description);
    virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override { return ROLE_SYSTEM_CLIENT; }
    virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state) tm_override;
//     virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut);
    virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild) tm_override;
//     virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children);
//     virtual HRESULT _get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction);
    
    virtual HRESULT _accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect) tm_override;
//     virtual HRESULT _accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc);
//     virtual HRESULT _accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt);
//     virtual void    _accItemHitTest(CAccessibleWnd& sender, TM::CTuiPoint& pt, long& child);
//     virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex);
};

extern CAccMgr g_accMgr;


#endif