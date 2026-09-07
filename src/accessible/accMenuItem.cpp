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
#include "accMenuItem.h"


///////////////////////////////////////////////////////////////////////////////
// CAccMenuItem
CAccMenuItem g_accMenuItem;


// HRESULT CAccMenuItem::_get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val)
// {
//     return sender.GetAccNameOrValue(val, eGetWindowText);
// }

HRESULT CAccMenuItem::_get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction)
{
    szDefaultAction = L"Press";
    return S_OK;
}


