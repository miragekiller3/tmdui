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
#include "accCheckBox.h"


///////////////////////////////////////////////////////////////////////////////
// CAccCheckBox
CAccCheckBox g_accCheckBox;

HRESULT CAccCheckBox::_get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction)
{
    if (nIndex != CHILDID_SELF)
        return S_FALSE;

    TM::CTuiCheckBox winCkBtn(sender);
    TMBool bChecked = winCkBtn.GetCheck();
    szDefaultAction = bChecked ? L"UnCheck" : L"Check";
    return S_OK;
}

