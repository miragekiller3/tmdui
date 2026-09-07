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
#include "accRadioButton.h"


///////////////////////////////////////////////////////////////////////////////
// CAccRadioButton
CAccRadioButton g_accRadioButton;

HRESULT CAccRadioButton::_get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction)
{
    szDefaultAction = L"Check";
    return S_OK;
}
