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
#include "accComboBox.h"

using namespace TM;


//enum ECombMode
//{
//    _eNull = 0x00,
//    _eNoEdit = 0x01,         // CBS_NOEDIT
//    _eNoList = 0x02,         // CBS_NOIMEDIALTY + nopop
//
//    eModeEditBtnList = 0x00,
//    eModeEditBtn = _eNoList,
//    eModeList = _eNoEdit,
//    eModeNone = _eNoList | _eNoEdit,
//
//    eCountEditBtnList = 3,
//    eCountEditBtn = 2,
//    eCountList = 1,
//    eCountNone = 0,
//};
//TM_ENUM_2_FLAG(ECombMode);
//
//class IComboDetail;

//class IComboDetail
//{
//public:
//    virtual long _GetCount() = 0;
//    virtual CAccComboBox::EObj _Index2Obj(long nIndex) = 0;
//};
//
//template <ECombMode mode, ECombMode count>
//class CComboDetail : public IComboDetail
//{
//    virtual long _GetCount() tm_final
//    {
//        return count;
//    }
//
//    virtual CAccComboBox::EObj _Index2Obj(long nIndex) tm_final
//    {
//        DbgAssert(nIndex != 0);
//        if (mode == eModeEditBtnList)
//        {
//            if (nIndex >= 1 && nIndex <= 3)
//                return (CAccComboBox::EObj)nIndex;
//        }
//        else if (mode == eModeEditBtn)
//        {
//            if (nIndex >= 1 && nIndex <= 2)
//                return (CAccComboBox::EObj)nIndex;
//        }
//        else if (mode == eModeList && nIndex == 1)
//            return CAccComboBox::eIndexList;
//        return CAccComboBox::eIndexError;
//    }
//};
//
//static CComboDetail<eModeEditBtnList, eCountEditBtnList>  _s_accCombEditBtnList;
//static CComboDetail<eModeEditBtn, eCountEditBtn>      _s_accCombEditBtn;
//static CComboDetail<eModeList, eCountList>         _s_accCombList;
//static CComboDetail<eModeNone, eCountNone>         _s_accCombNone;


///////////////////////////////////////////////////////////////////////////////
// CAccComboBox
// CAccComboBox g_accComboBox;

// CAccessibleObj* CAccComboBox::GetSubIAccessible(CAccessibleWnd& sender, long nIndex)
// {
//     CTuiComboUnit wnd(sender);
//     HTUI hTarget;
//     
//     switch (nIndex)
//     {
//     case eIndexEdit: hTarget = wnd.GetEdit(); break;
//     case eIndexButton: hTarget = wnd.GetButton(); break;
//     case eIndexList: hTarget = wnd.GetPopList();  break;
//     default: DbgAssert(0); return 0;
//     }
//     return CAccessibleWrapper::GetAccClient(hTarget);
// }

CAccessibleObj* CAccComboBox::GetListBoxIAccessible(HTUI hWgt)
{
    CTuiComboUnit wnd(hWgt);
    HTUI hTarget = wnd.GetPopList();
    if (!hTarget)
        return 0;

    return CAccessibleObj::MakeAccClient(hTarget);
}

CAccessibleObj* CAccComboBox::GetButtonIAccessible(HTUI hWgt)
{
    CTuiComboUnit wnd(hWgt);
    HTUI hTarget = wnd.GetButton();
    if (!hTarget || hTarget == hWgt)
        return 0;

    return CAccessibleObj::MakeAccClient(hTarget);
}

long CAccComboBox::_get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet)
{
    if (nIndex == eIndexList)
        *ppRet = GetListBoxIAccessible(sender);
    else if (nIndex == eIndexButton)
        *ppRet = GetButtonIAccessible(sender);
    else if (nIndex != eIndexEdit)
        return 3;

    return -1;
};

HRESULT CAccComboBox::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    return CAccessibleObj::GetAccNameOrValue(sender, name, eGetPrevStatic | eGetAccText | eGetHint);
}

HRESULT CAccComboBox::_get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val)
{
    if (nIndex != eIndexSelf && nIndex != eIndexEdit)
        return S_FALSE;

    return CAccessibleObj::GetAccNameOrValue(sender, val, eGetWindowText);
}

long CAccComboBox::_get_accRole(HTUI hWgt, long nIndex)
{
    switch (nIndex)
    {
    case CHILDID_SELF: return ROLE_SYSTEM_COMBOBOX;
    case eIndexEdit: return ROLE_SYSTEM_TEXT; //CAccessibleObj::GetRealRole(hWgt, m_ptrEdit, 0);
    case eIndexButton: return ROLE_SYSTEM_PUSHBUTTON;
    case eIndexList: return ROLE_SYSTEM_LIST;
    default:
        return 0;
    }
//     case 2: return ROLE_SYSTEM_PUSHBUTTON;
//     case 3:
//         {
//             IAccessible* pAccListBox = GetListBoxIAccessible(hWnd);
//             if (pAccListBox)
//             {
//                 CComVariant var((long)CHILDID_SELF);
//                 CComVariant varRet((long)0);
//                 pAccListBox->get_accRole(var, &varRet);
//                 pAccListBox->Release();
//                 return varRet.lVal;
//             }
//             break;
//         }
//     }
//     return 0;
}

HRESULT CAccComboBox::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
//     DbgAssert(nIndex == 0 || nIndex == 1);


    HRESULT hr = m_ptrEdit->_get_accState(sender, 0, state);
    if (FAILED(hr))
        return hr;

    if (nIndex == CHILDID_SELF || nIndex == eIndexEdit)
    {
        TMBool bExpand = CTuiComboUnit(sender).IsDropDown();
        if (bExpand)
        {
            state |= STATE_SYSTEM_EXPANDED;
 //           state &= ~STATE_SYSTEM_FOCUSED;
        }
        else
        {
            state |= STATE_SYSTEM_COLLAPSED;
            if (!(state & STATE_SYSTEM_FOCUSED))
            {
                if (CAccessibleObj::CombUtilIsFocus(sender, eCombEdit, _tuiApp.GetFocus()))
                    state |= STATE_SYSTEM_FOCUSED;
            }
        }

        return S_OK;
    }

    return S_OK;
}

HRESULT CAccComboBox::_get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut)
{
    if (nIndex == eIndexButton)
    {
        szKeyboardShortcut = L"Alt+Down Arrow";
        return S_OK;
    }
//     else if (nIndex == CHILDID_SELF)
//     {
//         CTuiWidget hPreWin (CTuiWidget(hWnd).GetWindow(GW_HWNDPREV));
//         if (hPreWin)
//             return __get_accKeyboardShortcut(hWnd, szKeyboardShortcut);
//     }
    return S_FALSE;
}



