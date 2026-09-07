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
#include "accPushButton.h"

using namespace TM;

CAccPushButton g_accPushButton;
CAccCombButton g_accCombButton;

///////////////////////////////////////////////////////////////////////////////
// CAccPushButton

// HRESULT CAccPushButton::_get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val)
// {
//     return sender.GetAccNameOrValue(val, eGetWindowText);
// }

HRESULT CAccPushButton::_get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction)
{
    szDefaultAction = L"Press";
    return S_OK;
}



long   CAccPushButton::_get_accRole(HTUI hWgt, long nIndex) 
{ 
#if SUPPORT_CHANGEROLE_TO_CHECK_BOX
    TM::CTuiPushButton button(sender);
    if (button.GetButtonStateNumber() > 1)
    {
        return ROLE_SYSTEM_CHECKBUTTON;
    }
#endif
    return ROLE_SYSTEM_PUSHBUTTON;
}

HRESULT CAccPushButton::_get_accHelp(CAccessibleWnd& sender, long nIndex, CComBSTR& description)
{
    return E_NOTIMPL;
    HRESULT retCode = S_OK;
    if (nIndex == CHILDID_SELF)
    {
        UINT nID = sender.GetDlgCtrlObjectID();
        TM::CString strID;
        strID.Format(_T("%d"), nID);
        description = strID;

    }
    else
    {
        retCode = S_FALSE;
    }
    return S_OK;
}

HRESULT CAccPushButton::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    HRESULT hr = CAccessibleStyleButtonBase::_get_accState(sender, nIndex, state);
    if (FAILED(hr))
        return hr;

    //state |= STATE_SYSTEM_DEFAULT;
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// CAccCombButton
HRESULT CAccCombButton::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    HRESULT hr = CAccPushButton::_get_accState(sender, nIndex, state);
    if (FAILED(hr))
        return hr;

    if (!(state & STATE_SYSTEM_FOCUSED))
    {
        if (CAccessibleObj::CombUtilIsFocus(sender, eCombButton, _tuiApp.GetFocus()))
            state |= STATE_SYSTEM_FOCUSED;
    }

    //state &= ~STATE_SYSTEM_CHECKED;

    //TMBool bExpand = CTuiComboUnit(sender).IsDropDown();
    //if (bExpand)
    //    state |= STATE_SYSTEM_EXPANDED;
    //else
    //    state |= STATE_SYSTEM_COLLAPSED;

    //state |= STATE_SYSTEM_DEFAULT;
    return S_OK;
}