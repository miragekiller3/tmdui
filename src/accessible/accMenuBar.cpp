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
#include "accMenuBar.h"
// #include "MSAAProxy.h"


///////////////////////////////////////////////////////////////////////////////
// CAccMenuBar
CAccMenuBar g_accMenuBar;

long CAccMenuBar::_get_accItemCount(CAccessibleWnd& sender)
{
    return TM::CTuiMenuBar(sender).GetMenuItemCount();
}

HRESULT CAccMenuBar::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    if (nIndex == CHILDID_SELF)
        return CAccessibleObj::GetAccNameOrValue(sender, name, eGetCustom | eGetAccText | eGetWindowText);

    TM::CTuiMenuBar::ITEMINFO minfo = {0};
    if (!TM::CTuiMenuBar(sender).GetMenuBarInfo(nIndex - 1, minfo))
        return DISP_E_MEMBERNOTFOUND;


//    TM::CStrPtr x;
//     int n = x.npos;
//     int n2 =  std::_tm_string_view<char>::npos;
//    int n3 =  TM::CStrPtrBaseT<char>::npos;
    

    TM::CString strText(minfo.text);
    strText.Replace(_T("&"), _T(""));
    name = strText;
    return S_OK;
}

// HRESULT CAccMenuBar::_get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description)
// {
//     description = nIndex == CHILDID_SELF ? L"menu bar" : L"menu item";
//     return S_OK;
// }

long CAccMenuBar::_get_accRole(HTUI hWgt, long nIndex)
{
    return nIndex == CHILDID_SELF ? ROLE_SYSTEM_MENUBAR : ROLE_SYSTEM_MENUITEM;
}

HRESULT CAccMenuBar::_get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut)
{
    if (nIndex == CHILDID_SELF)
        return S_FALSE;

    TM::CTuiMenuBar::ITEMINFO minfo = {0};
    if (!TM::CTuiMenuBar(sender).GetMenuBarInfo(nIndex - 1, minfo))
        return DISP_E_MEMBERNOTFOUND;

    return CAccessibleObj::MakeAccKeyboardShortcut(minfo.text, szKeyboardShortcut);
}

HRESULT CAccMenuBar::_get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction)
{
    if (nIndex == CHILDID_SELF)
        return S_FALSE;

    szDefaultAction = L"Click";
    return S_OK;
}

HRESULT CAccMenuBar::_accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc)
{
    if (!nIndex)
        return S_OK;
    TM::CTuiMenuBar::ITEMINFO minfo = {0};
    if (!TM::CTuiMenuBar(sender).GetMenuBarInfo(nIndex - 1, minfo))
        return DISP_E_MEMBERNOTFOUND;
    rc = minfo.rc;
    return S_OK;
}

HRESULT CAccMenuBar::_accDoDefaultAction(CAccessibleWnd& sender, long nIndex)
{
    TM::CTuiRect rcTmp;
    HRESULT hr = _accItemLocation(sender, nIndex, rcTmp);
    if (FAILED(hr))
        return hr;

    TM::CTuiPoint pt = rcTmp.CenterPoint();
    CAccessibleObj::VirtualMouseClick(sender, pt, false);
    return S_OK;
}

