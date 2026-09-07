/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.08.25
 *
 *  Copyright (C) 2009-2018 miragekiller
 */


#include "stdafx.h"
#include "accMgr.h"


///////////////////////////////////////////////////////////////////////////////
// CAccMgr
CAccMgr g_accMgr;


HRESULT CAccMgr::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    HRESULT hr = CAccessibleObj::GetAccNameOrValue(sender, name, eGetCustom | eGetAccText | eGetHint | eGetWindowText);
    if (S_OK == hr)
        return hr;

    TCHAR szTitle[MAX_PATH] = { 0 };
    _InternalGetWindowText(sender.GetHWND(), szTitle, MAX_PATH);
    if (!szTitle[0])
        return S_FALSE;

    name = szTitle;
    return S_OK;
}

HRESULT CAccMgr::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    HRESULT hr = CAccessibleStyle::_get_accState(sender, nIndex, state);
    if (FAILED(hr))
        return hr;

    TMHWnd hParent = sender.GetHWND();
    TMULong dwState = 0;
    if (::GetActiveWindow() == hParent)
    {
        TMUInt32 dwStyle  = sender.GetStyle();
        TMBool bVisible  = ::IsWindowVisible(hParent);
        TMBool bEnabled  = ::IsWindowEnabled(hParent);
        TMBool bSizeable = (dwStyle & WS_SIZEBOX);
        if (!bVisible)
            state |= STATE_SYSTEM_INVISIBLE;
        
        if (!bEnabled)
            state |= STATE_SYSTEM_UNAVAILABLE;
        
        state |= STATE_SYSTEM_MOVEABLE;
        state |= STATE_SYSTEM_FOCUSABLE;
        
        if (bSizeable)
            dwState |= STATE_SYSTEM_SIZEABLE;
    }
    return S_OK;
}

HRESULT CAccMgr::_get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild)
{
    TMHWnd hWnd = sender.GetHWND();
    if (hWnd && ::GetActiveWindow() == hWnd)
    {
        TM::HTUI hFocusWnd = sender.GetAPP()->GetFocus();
        if (hFocusWnd)
        {
            pvarFocusChild->vt = VT_DISPATCH;
            pvarFocusChild->pdispVal = CAccessibleObj::MakeAccClient(hFocusWnd);
            return S_OK;
        }
    }
    return S_FALSE;
}

HRESULT CAccMgr::_accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect)
{
    if (flagsSelect != SELFLAG_TAKEFOCUS)
        return S_FALSE;

    ::SetActiveWindow(sender.GetHWND());
    return S_OK;
}
