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
#include "accStatic.h"


///////////////////////////////////////////////////////////////////////////////
// CAccStatic
CAccStatic g_accStatic;
CAccGroup  g_accGroup;

HRESULT CAccStaticBase::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    state = STATE_SYSTEM_READONLY | CAccessibleObj::GetState(sender);
    return S_OK;
}
