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

#ifndef ACCPROGRESSBAR_H
#define ACCPROGRESSBAR_H

#include "Proxy.h"

 ///////////////////////////////////////////////////////////////////////////////
 // CAccTPBase
class TM_NO_VTABLE CAccTPBase : public CAccessibleStyle
{
public:
    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
    virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val) tm_override;
    virtual int _get_acc2States(CAccessibleWnd& sender) tm_final;
};

///////////////////////////////////////////////////////////////////////////////
// CAccTrackBar
class CAccTrackBar : public CAccTPBase
{
public:
//     virtual long _get_accItemCount(CAccessibleWnd& sender);
//     virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet);  // return -1: succeeded; else return _get_accItemCount();
    
//     virtual TM::HTUI _get_accParent(CAccessibleWnd& sender);
//     virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
//    virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val) tm_override;
//     virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description);
    virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override { return ROLE_SYSTEM_SLIDER;/* ROLE_SYSTEM_PROGRESSBAR;*/ }
    virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state) tm_override;
//     virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut);
//     virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild);
//     virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children);
//     virtual HRESULT _get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction);
    
//     virtual HRESULT _accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect);
//     virtual HRESULT _accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc);
//     virtual HRESULT _accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt);
//     virtual void    _accItemHitTest(CAccessibleWnd& sender, TM::CTuiPoint& pt, long& child);
//     virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex);

//     virtual HRESULT _QueryService(CAccessibleWnd& sender, REFIID riid, void** ppvObject);
};

///////////////////////////////////////////////////////////////////////////////
// CAccProgress
class CAccProgress : public CAccTPBase
{
//    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
//    virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val) tm_override;
    virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override { return ROLE_SYSTEM_PROGRESSBAR; }
};


extern CAccTrackBar g_accTrackBar;
extern CAccProgress g_accProgress;


#endif