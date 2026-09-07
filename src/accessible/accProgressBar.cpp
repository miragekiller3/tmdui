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
#include "accProgressBar.h"

CAccTrackBar g_accTrackBar;
CAccProgress g_accProgress;

using namespace TM;


///////////////////////////////////////////////////////////////////////////////
// CAccTrackBar
HRESULT CAccTPBase::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    return  CAccessibleObj::GetAccNameOrValue(sender, name, eGetCustom | eGetPrevStatic | eGetAccText | eGetHint);
}

HRESULT CAccTPBase::_get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val)
{
    CTuiProgress wnd(sender);
    int nPos = CAccessibleObj::GetPercentageValue(wnd);
    CStringW str;
    str.Format(L"%d", nPos);
    val = str;
    return S_OK;
}

int CAccTPBase::_get_acc2States(CAccessibleWnd& sender)
{
    return (sender.GetStyle() & PBS_VERTICAL) ? IA2_STATE_VERTICAL : IA2_STATE_HORIZONTAL;
}

///////////////////////////////////////////////////////////////////////////////
// CAccTrackBar
//HRESULT CAccTrackBar::_get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val)
//{
//    CTuiTrackBar _trackbar(sender);
//    int nPos = CAccessibleObj::GetPercentageValue(_trackbar);
//    //if (Style & TBS_REVERSE) nPos = 100 - nPos;
//    return VarBstrFromI4(nPos, 0, 0, &val);
//    //CStringW str;
//    //str.Format(L"%d%%", nPos);
//    //val = str;
//    //return S_OK;
//}

HRESULT CAccTrackBar::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    HRESULT hr = CAccessibleStyle::_get_accState(sender, nIndex, state);
    if (FAILED(hr))
        return hr;
    
    EButStates nState = sender.GetState();
    TMBool bPressed = (nState & e_bssDown);
    if (bPressed)
        state |= STATE_SYSTEM_PRESSED;
    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// CAccProgress

