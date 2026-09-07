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
#include "accSpin.h"

using namespace TM;


///////////////////////////////////////////////////////////////////////////////
// CAccSpin
CAccSpin g_accSpin;


long CAccSpin::_get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet)
{
    if (nIndex == eIndexUpLeft || nIndex == eIndexDownRight)
        return nIndex;
    return -1;
};

HRESULT CAccSpin::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    switch(nIndex)
    {
    case eIndexSelf: CAccessibleObj::GetAccNameOrValue(sender, name, eGetPrevStatic | eGetAccText | eGetHint); break;
    case eIndexUpLeft: name = "More"; break;
    case eIndexDownRight: name = "Less"; break;
    default: return S_FALSE;
    }
    return S_OK;
}

HRESULT CAccSpin::_get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val)
{
    if (nIndex != eIndexSelf)
        return DISP_E_MEMBERNOTFOUND;

    int n = CTuiUpDown(sender).GetPos();
    return(VarBstrFromI4(n, 0, 0, &val));
}

long CAccSpin::_get_accRole(HTUI hWgt, long nIndex)
{
    return nIndex ? ROLE_SYSTEM_PUSHBUTTON : ROLE_SYSTEM_SPINBUTTON;
}

HRESULT CAccSpin::_accItemHitTest(CAccessibleWnd& sender, TM::CTuiPoint& pt, long& child)
{
    TM::TUIHITINFO ht = {0};
    ht.pt.x = pt.x;
    ht.pt.y = pt.y;
    sender.HitTest(ht);
    if (ht.hWnd == sender)
    {
        EHitID hid = ht.hid;
        return TUIHITGETSUBSB(hid) == SB_PAGEUP ? eIndexUpLeft : eIndexDownRight;
    }
    return S_FALSE;
}



