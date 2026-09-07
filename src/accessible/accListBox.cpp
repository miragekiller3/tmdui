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
#include "accListBox.h"
#include "Hash.h"

using namespace TM;

CAccListBox  g_accListBox;
CAccListItem g_accListSelItem;
CAccCombList g_accCombListBox;

///////////////////////////////////////////////////////////////////////////////
// CAccListBox
HRESULT CAccListBox::GetGroupPosition(CAccessibleWnd& wnd, long* groupLevel, long* similarItemsInGroup, long* positionInGroup)
{
    CTuiListBox lb(wnd);
    *positionInGroup = lb.GetCurSel() + 1;
    *similarItemsInGroup = lb.GetCount();
    *groupLevel = 0;
    return S_OK;
}

long CAccListBox::_get_accItemCount(CAccessibleWnd& sender)
{
    return CTuiListBox(sender).GetCount();
}

long CAccListBox::_get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet)
{
    CTuiListBox wnd(sender);
    int n = wnd.GetCount();
    if (nIndex > n)
        return n;

    if (_list_isFocus(sender) && (nIndex - 1) == CTuiListBox(sender).GetCurSel())
        *ppRet = CHash::MakeFocusItem(this, sender, nIndex, g_accListSelItem);

    //HTUI hBind = 0;// wnd.GetBindItemWindow(nIndex - 1);
    //if (hBind)
    //    *ppRet = CAccessibleObj::MakeAccClient(hBind);
    return -1;
}

HRESULT CAccListBox::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    if (nIndex == CHILDID_SELF)
    {
        CString str;
        CAccessibleObj::GetAccNameOrValue(sender, str, eGetCustom | eGetPrevStatic | eGetAccText | eGetWindowText);
        name = str;
        return S_OK;
    }

    --nIndex;

    CTuiListBox wnd(sender);
    int count = wnd.GetCount();
    if (nIndex > count)
    {
//        DbgAssert(0);
        return E_INVALIDARG;
    }

    wnd.GetItemAccText(nIndex).CopyToBSTR(&name);
//     wnd.GetTextBSTR(nIndex - 1, *&name);
    return S_OK;
}

long CAccListBox::_get_accRole(HTUI hWgt, long nIndex)
{
    if (nIndex == CHILDID_SELF)
        return ROLE_SYSTEM_LIST;

    CTuiWidget sender(hWgt);
    long role = sender.AccGetRole();
    switch (role)
    {
    case ROLE_SYSTEM_GROUPING:
    case ROLE_SYSTEM_WINDOW:        return ROLE_SYSTEM_STATICTEXT;
    case ROLE_SYSTEM_PAGETABLIST:   return ROLE_SYSTEM_PAGETAB;
    case ROLE_SYSTEM_MENUBAR:
    case ROLE_SYSTEM_MENUPOPUP:     return ROLE_SYSTEM_MENUITEM;
    default:                        return ROLE_SYSTEM_LISTITEM;
    }
}

HRESULT CAccListBox::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    if (nIndex == CHILDID_SELF)
    {
        HRESULT hr = CAccessibleStyle::_get_accState(sender, nIndex, state);
        if (this == &g_accCombListBox)
            state |= STATE_SYSTEM_FLOATING;
        return hr;
    }

    if (sender.IsFocus() || sender.IsChild(_tuiApp.GetFocus()))
        state |= STATE_SYSTEM_FOCUSABLE;

    CTuiListBox wnd(sender);
    TMBool bMultiSel = ((wnd.GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) != 0);
    if (bMultiSel)
        state |= STATE_SYSTEM_MULTISELECTABLE;
    else
        state |= STATE_SYSTEM_SELECTABLE;
    long index = nIndex - 1;
    if (index == wnd.GetCurSel())
    {
        state |= STATE_SYSTEM_SELECTED;
        if (state & STATE_SYSTEM_FOCUSABLE)
            state |= STATE_SYSTEM_FOCUSED;
    }

//     CTuiRect rcList, rcItem;
//     wnd.GetWindowRect(&rcList);
//     wnd.GetItemRect(index, &rcItem);
//     rcList &= rcItem;
//     if (rcList.IsRectEmpty())
//         state |= STATE_SYSTEM_OFFSCREEN;
    return S_OK;
}

HRESULT CAccListBox::_get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild)
{
    CTuiListBox lb(sender);
    TMBool bFocus = lb.IsFocus();
    if (bFocus)
    {
        pvarFocusChild->lVal = lb.GetCurSel() + 1;
        pvarFocusChild->vt = VT_I4;
        return S_OK;
    }
    return S_FALSE;
}

HRESULT CAccListBox::_get_accSelection(CAccessibleWnd& sender, long& children)
{
    CTuiListBox lb(sender);
    children = lb.GetCurSel() + 1;
    return children ? S_OK : S_FALSE;
}

HRESULT CAccListBox::_get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction)
{
    if (nIndex == CHILDID_SELF)
        return DISP_E_MEMBERNOTFOUND;
    szDefaultAction = L"Double-click";
    return S_OK;
}

HRESULT CAccListBox::_accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect)
{
   // Move the focus to the list box.
    CTuiListBox lb(sender);
    lb.SetFocus();
    
    // Move the selection if called on to do so.
    if (((flagsSelect & (SELFLAG_TAKESELECTION | SELFLAG_TAKEFOCUS)) != 0) && (nIndex != CHILDID_SELF))
    {
        int selection = nIndex - 1;
        lb.SetSel(selection);
    }
    return S_OK;
}

HRESULT CAccListBox::_accItemLocation(CAccessibleWnd& sender, long nIndex, CTuiRect& rc)
{
    if (!nIndex)
        return S_OK;
    CTuiListBox lb(sender);
    int n = lb.GetItemRect(nIndex - 1, &rc);
    return n == LB_ERR ? DISP_E_MEMBERNOTFOUND : S_OK;
}

HRESULT CAccListBox::_accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt)
{
    long ret;
    int nCount = _get_accItemCount(sender);
    switch (navDir)
    {
    case NAVDIR_FIRSTCHILD:
        if ((varStart == CHILDID_SELF) && (nCount > 0))
            ret = 1;
        else 
            return S_FALSE;
        break;
        
    case NAVDIR_LASTCHILD:
        if ((varStart == CHILDID_SELF) && (nCount > 0))
            ret = nCount;
        else
            return S_FALSE;
        break;
        
    case NAVDIR_NEXT:
    case NAVDIR_DOWN:
        if (varStart != CHILDID_SELF)
        {
            ret = varStart + 1;
            if (ret > nCount) // Out of range.
                return S_FALSE;
        }
        else  // Call through to method on standard container.
            return CAccessibleStyle::_accNavigate(sender, navDir, varStart, pvarEndUpAt);
        break;
        
    case NAVDIR_PREVIOUS:
    case NAVDIR_UP:
        if (varStart != CHILDID_SELF)
        {
            ret = varStart - 1;
            if (pvarEndUpAt->lVal < 1) // Out of range.
                return S_FALSE;
        }
        else  // Call through to method on standard container.
            return CAccessibleStyle::_accNavigate(sender, navDir, varStart, pvarEndUpAt);
        break;
        
        // Unsupported directions.
    case NAVDIR_LEFT:
    case NAVDIR_RIGHT:
        if (varStart == CHILDID_SELF)
            return CAccessibleStyle::_accNavigate(sender, navDir, varStart, pvarEndUpAt);
        return S_FALSE;

    default:
        return false;
    }

    pvarEndUpAt->vt = VT_I4;
    pvarEndUpAt->lVal = ret;
    return S_OK;
}

HRESULT CAccListBox::_accItemHitTest(CAccessibleWnd& sender, CTuiPoint& pt, long& child)
{
    TUIHITINFO ht = {0};
    ht.pt.x = pt.x;
    ht.pt.y = pt.y;
    sender.HitTest(ht);
    if (ht.hWnd == sender)
        child = ht.dwCode + 1;
    return S_OK;
}

HRESULT CAccListBox::_accDoDefaultAction(CAccessibleWnd& sender, long nIndex)
{
    CTuiListBox lb(sender);
    CTuiRect rcTmp;
    int n = lb.GetItemRect(nIndex - 1, &rcTmp);
    if (n == LB_ERR)
        return E_INVALIDARG;

    CTuiPoint pt = rcTmp.CenterPoint();
    CAccessibleObj::VirtualMouseClick(sender, pt, true);
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// CAccListItem
HRESULT CAccListItem::GetGroupPosition(CAccessibleWnd& wnd, long* groupLevel, long* similarItemsInGroup, long* positionInGroup)
{
    *positionInGroup = m_sel;
    *similarItemsInGroup = CTuiListBox(wnd).GetCount();
    *groupLevel = 0;
    return S_OK;
}

HRESULT CAccListItem::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    if (nIndex == 0)
        return g_accListBox._get_accName(sender, m_sel, name);
    return S_FALSE;
}

HRESULT CAccListItem::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    if (nIndex == 0)
    {
        HRESULT hr = g_accListBox._get_accState(sender, m_sel, state);
        state |= (STATE_SYSTEM_SELECTED | STATE_SYSTEM_FOCUSABLE | STATE_SYSTEM_FOCUSED);
        return hr;
    }
    return S_FALSE;
}

//HRESULT CAccListItem::_get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild)
//{
//    return S_FALSE;
//}
//
//HRESULT CAccListItem::_get_accSelection(CAccessibleWnd& sender, long& children)
//{
//    return S_FALSE;
//}

HRESULT CAccListItem::_get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction)
{
    if (nIndex == 0)
        return g_accListBox._get_accDefaultAction(sender, m_sel, szDefaultAction);
    return S_FALSE;
}

HRESULT CAccListItem::_accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect)
{
    if (nIndex == 0)
        return g_accListBox._accSelect(sender, m_sel, flagsSelect);
    return S_FALSE;
}

HRESULT CAccListItem::_accItemLocation(CAccessibleWnd& sender, long nIndex, CTuiRect& rc)
{
    if (nIndex == 0)
        return g_accListBox._accItemLocation(sender, m_sel, rc);
    return S_FALSE;
}

HRESULT CAccListItem::_accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt)
{
    if (varStart == 0)
        return g_accListBox._accNavigate(sender, navDir, m_sel, pvarEndUpAt);
    return S_FALSE;
}

HRESULT CAccListItem::_accItemHitTest(CAccessibleWnd& sender, CTuiPoint& pt, long& child)
{
    return S_FALSE;
}

HRESULT CAccListItem::_accDoDefaultAction(CAccessibleWnd& sender, long nIndex)
{
    if (nIndex == 0)
        return g_accListBox._accDoDefaultAction(sender, m_sel);
    return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// CAccCombList
HTUI CAccCombList::_get_accParent(CAccessibleWnd& sender)
{
    return CTuiComboUnit(sender).GetEdit();
}
