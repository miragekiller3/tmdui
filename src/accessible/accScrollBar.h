/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.08.30
 *
 *  Copyright (C) 2009-2018 miragekiller
 */

#pragma once

#ifndef ACCSCROLLBAR_H
#define ACCSCROLLBAR_H

#include "Proxy.h"


///////////////////////////////////////////////////////////////////////////////
// CAccScrollBar
class CAccScrollBar : public CAccessibleStyleButtonBase
{
public:
    enum { eIndexSelf = 0, eIndexUp, eIndexPageUp, eIndexThumb, eIndexPageDown, eIndexDown, eCount = 5 };

private:
    TMBool IsVScroll() const;

public:
    virtual long _get_accCtrlCount(CAccessibleWnd& sender) tm_override { return 0; }
    virtual long _get_accItemCount(CAccessibleWnd& sender) tm_override;
//     virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet);  // return -1: succeeded; else return _get_accItemCount();
    
//     virtual TM::HTUI _get_accParent(CAccessibleWnd& sender);
    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
    virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val) tm_override;
//     virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description);
    virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override;
    virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state) tm_override;
//     virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut);
//     virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild);
//     virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children);
    virtual HRESULT _get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction) tm_override;
    
//     virtual HRESULT _accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect);
    virtual HRESULT _accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc) tm_override;
    virtual HRESULT _accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt) tm_override;
    virtual HRESULT _accItemHitTest(CAccessibleWnd& sender, TM::CTuiPoint& pt, long& child) tm_override;
//     virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex);
};

extern CAccScrollBar g_accScrollBarV; // SB_VERT, SB_HORZ
extern CAccScrollBar g_accScrollBarH; // SB_VERT, SB_HORZ

#endif // ACCSCROLLBAR_H