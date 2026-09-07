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
#include "accTreeView.h"
//#include "tmcom.h"

using namespace TM;

CAccTreeView g_accTreeView;
CAccTreeItem g_accTreeSelItem;
CAccCombTree g_accCombTreeView;

///////////////////////////////////////////////////////////////////////////////
// CAccTreeView
static void _CAccTreeItem_GetGroupPosition(CTVItem itm, long* groupLevel, long* similarItemsInGroup, long* positionInGroup)
{
    *groupLevel = itm.GetLevel();

    long pig = 1;
    long sig = 1;
    tm_for(CTVItem w(itm); w = w.GetPrev(); )
    {
        if (w.IsVisible())
        {
            ++pig; ++sig;
        }
    }
    tm_for(CTVItem w(itm); w = w.GetNext(); )
    {
        if (w.IsVisible())
            ++sig;
    }
    *similarItemsInGroup = sig;
    *positionInGroup = pig;
}

HRESULT CAccTreeView::GetGroupPosition(CAccessibleWnd& wnd, long* groupLevel, long* similarItemsInGroup, long* positionInGroup)
{
    CTVItem itm = CTuiTree(wnd).GetSelectedItem();
    if (!itm)
        return S_FALSE;
    _CAccTreeItem_GetGroupPosition(itm, groupLevel, similarItemsInGroup, positionInGroup);
    return S_OK;
}

UINT CAccTreeView::GetIndexByItem(CTVItem item)
{
    int i = 1;
    for (CTVItem _Item_ = item.GetRoot() ; _Item_ = _Item_.GetGlobalNext(); ++i)
    {
        DbgAssert(_Item_);
        if (_Item_ == item.m_hItem)
        {
            ACC_TRACE("CAccTreeView::GetIndexByItem(item:0x%p) return:%d", item.m_hItem, i);
            return i;
        }
    }
    
    ACC_TRACE("CAccTreeView::GetIndexByItem(item:0x%p) return:0");
    return 0;
}

CTVItem CAccTreeView::GetItemByIndex(CAccessibleWnd& sender, UINT uIndex)
{
    DbgAssert(0 != uIndex);
    CTuiTree tree(sender);
    CTVItem tvItem = tree.GetRootItem();
    for (UINT i = 0; (i < uIndex) && tvItem; ++i)
        tvItem = tvItem.GetGlobalNext(); //index start from 1;
    ACC_TRACE("CAccTreeView::GetItemByIndex(item uIndex:%d, item:0x%p)", uIndex, tvItem.m_hItem);
    return tvItem;
}

long CAccTreeView::_get_accItemCount(CAccessibleWnd& sender)
{
    ACC_TRACE("CAccTreeView::_get_accItemCount(this:%p, item count:%d)", this, CTuiTree(sender).GetCount());
    return CTuiTree(sender).GetCount();
}

long CAccTreeView::_get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet)
{
    CTVItem childItem = GetItemByIndex(sender, nIndex);
    if (!childItem)
    {
        ACC_TRACE("CAccTreeView::_get_accItem(this:%p, nIndex:%d)", this, nIndex);
        return _get_accItemCount(sender);
    }

    if (_tree_isFocus(sender) && CTuiTree(sender).GetSelectedItem() == childItem)
        *ppRet = CHash::MakeFocusItem(this, sender, nIndex, g_accTreeSelItem);

    return -1;
}

HRESULT CAccTreeView::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    if (CHILDID_SELF == nIndex)
        return CAccessibleStyle::_get_accName(sender, nIndex, name);

    CTVItem childItem = GetItemByIndex(sender, nIndex);
    if (!childItem)
        return DISP_E_MEMBERNOTFOUND;
    CStringW strAccText;
    HTUI hWgt = childItem.GetBindItemWindow();
    
    if (hWgt)
    {
        CTuiWidget itemWindow(hWgt);
//         itemWindow.SendMessageW(CM_ACC_GET_ITEM_TEXT, (TMWParam)childItem.m_hItem, (TMLParam)&strAccText);
        itemWindow.GetAccText(strAccText);
    }

    if (!strAccText.empty())
    {
        name = strAccText.c_str();
    }
    else
    {
        CString strName;
        childItem.GetHint(strName);
        if (strName.IsEmpty())
            childItem.GetText(strName);
        name = strName;
    }
    return S_OK;
}

HRESULT CAccTreeView::_get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val)
{
    if (CHILDID_SELF == nIndex)
        return CAccessibleStyle::_get_accValue(sender, nIndex, val);

    long role = CAccessibleObj::GetRealRole(sender, this, 0);
    if (role != ROLE_SYSTEM_OUTLINEITEM)
        return DISP_E_MEMBERNOTFOUND;

    CTVItem childItem = GetItemByIndex(sender, nIndex);
    if (!childItem)
        return DISP_E_MEMBERNOTFOUND;
    
    int iLevel = childItem.GetLevel() - 1;
    return VarBstrFromI4(iLevel, 0, 0, &val);
}

long CAccTreeView::_get_accRole(HTUI hWgt, long nIndex)
{
    if (CHILDID_SELF == nIndex)
        return ROLE_SYSTEM_OUTLINE;

    CTuiWidget sender(hWgt);
    if (sender.GetStyle() & TVS_CHECKBOXES)
        return ROLE_SYSTEM_CHECKBUTTON;
    long role = sender.AccGetRole();
    switch (role)
    {
    case ROLE_SYSTEM_LIST:          return ROLE_SYSTEM_LISTITEM;
    case ROLE_SYSTEM_GROUPING:
    case ROLE_SYSTEM_WINDOW:        return ROLE_SYSTEM_STATICTEXT;
    case ROLE_SYSTEM_PAGETABLIST:   return ROLE_SYSTEM_PAGETAB;
    default:                        return ROLE_SYSTEM_OUTLINEITEM;
    }
}

HRESULT CAccTreeView::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    if (CHILDID_SELF == nIndex)
    {
        HRESULT hr = CAccessibleStyle::_get_accState(sender, CHILDID_SELF, state);
        if (this == &g_accCombTreeView)
            state |= STATE_SYSTEM_FLOATING;
        return hr;
    }

    CTVItem tvItem = GetItemByIndex(sender, nIndex);
    if (!tvItem)
        return E_INVALIDARG;

    state = 0;
    
    if (sender.IsFocus() || sender.IsChild(_tuiApp.GetFocus()))
        state |= STATE_SYSTEM_FOCUSABLE;
    
    for (;;)
    {
        if (tvItem.m_hItem->IsVisible())
        {
            CTuiRect rc, rcClient;
            tvItem.GetRect(&rc);
            sender.GetClientRect(&rcClient);
            rcClient &= rc;
            if (!rcClient.IsRectEmpty())
                break;
        }
        state |=  STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN;
        break;
    }
    
    DWORD dwState = tvItem.GetState();
    if (dwState & TVIS_SELECTED)
    {
        state |= STATE_SYSTEM_SELECTED;
        if (state & STATE_SYSTEM_FOCUSABLE)
            state |= STATE_SYSTEM_FOCUSED;
    }
        
    if (!(dwState & TVIS_DISABLE) && !(dwState & TVIS_HIDE))
        state |= STATE_SYSTEM_SELECTABLE;
    
    if (dwState & TVIS_DROPHILITED)
        state |= STATE_SYSTEM_HOTTRACKED;
    
    if (dwState & (TVIS_EXPANDED | TVIS_EXPANDPARTIAL))
        state |= STATE_SYSTEM_EXPANDED;
    else if (tvItem.HasChildren())
        state |= STATE_SYSTEM_COLLAPSED;
  
    if (dwState & TVIS_CHECKED)
        state |= STATE_SYSTEM_CHECKED;
        
    
//     if (tvItem.HasChildren())
//         state |= tvItem.IsExpand() ? STATE_SYSTEM_EXPANDED : STATE_SYSTEM_COLLAPSED;
//     
//     if (tvItem.IsSelected())
//     {
//         state |= STATE_SYSTEM_SELECTED;
// //         state |= (nStateWin & STATE_SYSTEM_FOCUSED);
//     }
//     else
//         state &= (~STATE_SYSTEM_FOCUSED);
//     
//     if (tvItem.IsChecked())
//         state |= STATE_SYSTEM_CHECKED;
//     
//     if (!tvItem.IsVisible())
//         state |= STATE_SYSTEM_INVISIBLE;
// 
//     if (!tvItem.IsDisable())
//         state |= STATE_SYSTEM_SELECTABLE;
        

//     CTuiRect rcTree, rcItem;
//     sender.GetWindowRect(&rcTree);
//     tvItem.GetRect(&rcItem);
//     rcTree &= rcItem;
//     if (rcTree.IsRectEmpty())
//         state |= STATE_SYSTEM_OFFSCREEN;

    return S_OK;
}

HRESULT CAccTreeView::_get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild)
{
    HRESULT hr = CAccessibleStyle::_get_accFocus(sender, pvarFocusChild);
    if (FAILED(hr))
        return hr;

    CTuiTree tree(sender);
    CTVItem selectedItem = tree.GetSelectedItem();
    if (selectedItem)
    {
        UINT nIndex = GetIndexByItem(selectedItem);
        pvarFocusChild->pdispVal = CHash::MakeFocusItem(this, sender, nIndex, g_accTreeSelItem);
        pvarFocusChild->vt = VT_DISPATCH;
    }

    return S_OK;
}

HRESULT CAccTreeView::_get_accSelection(CAccessibleWnd& sender, long& children)
{
    CTuiTree tree(sender);
    CTVItem selItem = tree.GetSelectedItem();
    if (!selItem)
        return S_FALSE;

    children = GetIndexByItem(selItem);
    return S_OK;
}

HRESULT CAccTreeView::_get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction)
{
    if (CHILDID_SELF == nIndex)
        return S_FALSE;

    CTVItem tvItem = GetItemByIndex(sender, nIndex);
    if (!tvItem)
        return DISP_E_MEMBERNOTFOUND;

    if (!tvItem.HasChildren())
        return DISP_E_MEMBERNOTFOUND;

    szDefaultAction = tvItem.IsExpand() ? L"Collapse" : L"Expand";
    return S_OK;
}

HRESULT CAccTreeView::_accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect)
{
    if (nIndex == CHILDID_SELF)
        return CAccessibleStyle::_accSelect(sender, nIndex, flagsSelect);

    if (!(flagsSelect & SELFLAG_TAKEFOCUS))
        return DISP_E_MEMBERNOTFOUND;

    CTVItem tvItem = GetItemByIndex(sender, nIndex);
    if (!tvItem)
        return DISP_E_MEMBERNOTFOUND;

    if ((flagsSelect & SELFLAG_TAKESELECTION))
        tvItem.SelectItem();
    else if (flagsSelect & SELFLAG_REMOVESELECTION)
        tvItem.SelectItem(CTVItem::e_soNull);

    return S_OK;
}

HRESULT CAccTreeView::_accItemLocation(CAccessibleWnd& sender, long nIndex, CTuiRect& rc)
{
    if (!nIndex)
        return S_OK;

    CTVItem tvItem = GetItemByIndex(sender, nIndex);
    if (!tvItem)
        return DISP_E_MEMBERNOTFOUND;

    if (!tvItem.IsVisible())
        return S_FALSE;

    tvItem.GetRect(&rc);
    return S_OK;
}

HRESULT CAccTreeView::_accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt)
{
    CTVItem tvItem;
    if (varStart == CHILDID_SELF)
    {
        switch (navDir)
        {
        case NAVDIR_FIRSTCHILD: tvItem = CTuiTree(sender).GetFirstVisible(); break;
        case NAVDIR_LASTCHILD:  tvItem = CTuiTree(sender).GetLastVisible(); break;
        default:
            return CAccessibleStyle::_accNavigate(sender, navDir, varStart, pvarEndUpAt);
        }

        if (!tvItem)
            return DISP_E_MEMBERNOTFOUND;
    }
    else
    {
        tvItem = (HTVITEM)GetItemByIndex(sender, varStart);
        if (!tvItem)
            return DISP_E_MEMBERNOTFOUND;
    
        switch (navDir)
        {
        case NAVDIR_NEXT:
            tvItem = tvItem.GetGlobalNext();
            break;
            
        case NAVDIR_PREVIOUS:
            tvItem = tvItem.GetGlobalPrev();
            break;
            
        case NAVDIR_UP:
            tvItem = tvItem.GetPrev();
            break;
            
        case NAVDIR_DOWN:
            tvItem = tvItem.GetNext();
            break;
            
        case NAVDIR_LEFT:
            tvItem = tvItem.GetParent();
            break;

        case NAVDIR_RIGHT:
            tvItem = tvItem.GetChild();
            break;

        case NAVDIR_FIRSTCHILD:
            tvItem = tvItem.GetChild();
            break;
            
        case NAVDIR_LASTCHILD:
            tvItem = tvItem.GetChild(-1);
            break;
            
        default:
            return S_FALSE;
        }
    }

    if (!tvItem)
        return DISP_E_MEMBERNOTFOUND;

    pvarEndUpAt->vt = VT_I4;
    pvarEndUpAt->lVal = GetIndexByItem(tvItem);
    return S_OK;
}

HRESULT CAccTreeView::_accItemHitTest(CAccessibleWnd& sender, CTuiPoint& pt, long& child)
{
    TUIHITINFO ht = {0};
    ht.hWnd = sender;
    ht.pt.x = pt.x;
    ht.pt.y = pt.y;
    sender.HitTest(ht);
    if (ht.hWnd == sender && !ht.bNC) // skip bind window
    {
        CTVItem itm = (HTVITEM)ht.pTaget;
        if (itm)
            child = GetIndexByItem(itm);
    }
    return S_OK;
}

HRESULT CAccTreeView::_accDoDefaultAction(CAccessibleWnd& sender, long nIndex)
{
    if (CHILDID_SELF == nIndex)
        return S_FALSE;
    
    CTVItem item = GetItemByIndex(sender, nIndex);
    if (!item)
        return DISP_E_MEMBERNOTFOUND;
    
    item.Toggle();
    item.SelectItem();
    return S_OK;
}


// class CEnumTreeItem : public CEnumImplBaseT<CEnumTreeItem, IEnumVARIANT, VARIANT>
// {
// private:
//     CAccessibleWrapper& m_owner;
// 
//     TM_BGN_INTERFACE_MAP(IEnumVARIANT)
//         TM_INTERFACE_HANDLER(IEnumVARIANT)
//         TM_INTERFACE_HANDLER(IUnknown)
//         TM_INTERFACE_MAP_MEMBER(&m_owner.m_objClient)
//     TM_END_INTERFACE_MAP()
//     
// public:
//     CEnumTreeItem(CAccessibleWrapper& owner)
//         : m_owner(owner)
//     {
//         m_owner.AddRef();
//     }
//     
//     CEnumTreeItem(const CEnumTreeItem& rhs)
//         : m_owner(rhs.m_owner)
//     {
//         m_owner.AddRef();
//     }
// 
//     ~CEnumTreeItem()
//     {
//         m_owner.Release();
//     }
//     
//     TMULong CopyTo(VARIANT* ppPins, TMULong cPins)
//     {
//         int nCount = cPins + this->m_nPos;
//         if (nCount > m_owner.m_objClient.m_ptr->_get_accItemCount(m_owner))
//             return 0;
//         if (ppPins)
//         {
//             for (int i = 0; i < cPins; ++i)
//                 (static_cast<CComVariant*>(ppPins))[i] = this->m_nPos + i + 1;
//         }
//         return cPins;
//     }
// };

// HRESULT CAccTreeView::_get_accEnum(CAccessibleWnd& sender, IEnumVARIANT** ppEnum)
// {
//     ACC_TRACE(L"CAccTreeView::_get_accEnum(%p), %s", this, _DINF(sender));
//     *ppEnum = new CEnumTreeItem(static_cast<CAccessibleWrapper&>(sender));
//     return S_OK;
// }
// 
// static CString trace_vars(VARIANT* ppPins, TMULong cPins)
// {
//     CString str;
//     if (!ppPins)
//         return str;
//     for (int i = 0; i < cPins; ++i)
//     {
//         CString str2;
//         str2.Format(L"%d,", ppPins[i].iVal);
//         str += str2;
//     }
//     return str;
// }
// 
// TMULong CAccTreeView::CopyTo(CAccessibleWnd& sender, int nCurPos, VARIANT* ppPins, TMULong cPins)
// {
//     int nCount = cPins + nCurPos;
//     if (nCount > _get_accItemCount(sender))
//         return 0;
//     if (ppPins)
//     {
//         for (int i = 0; i < cPins; ++i)
//         {
//             (static_cast<CComVariant*>(ppPins))[i] = nCurPos + i + 1;
//             DbgAssert(ppPins[i].vt == VT_I4);
//         }
//     }
//     if (cPins == 1)
//     {
//         int x = 0;
//     }
//     ACC_TRACE(L"CAccTreeView::CopyTo(nCurPos:%d, cvars:%d, vars:%s), %s",
//         nCurPos, cPins, trace_vars(ppPins, cPins).c_str(), _DINF(sender));
//     return cPins;
// }

///////////////////////////////////////////////////////////////////////////////
// CAccTreeItem
HRESULT CAccTreeItem::GetGroupPosition(CAccessibleWnd& wnd, long* groupLevel, long* similarItemsInGroup, long* positionInGroup)
{
    CTVItem itm = CAccTreeView::GetItemByIndex(wnd, m_sel);
    if (!itm)
        return S_FALSE;
    _CAccTreeItem_GetGroupPosition(itm, groupLevel, similarItemsInGroup, positionInGroup);
    return S_OK;
}

HRESULT CAccTreeItem::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    if (nIndex == 0)
        return g_accTreeView._get_accName(sender, m_sel, name);
    return S_FALSE;
}

HRESULT CAccTreeItem::_get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val)
{
    if (nIndex == 0)
        return g_accTreeView._get_accValue(sender, m_sel, val);
    return S_FALSE;
}

long CAccTreeItem::_get_accRole(HTUI hWgt, long nIndex)
{
    if (nIndex == 0)
        return g_accTreeView._get_accRole(hWgt, m_sel);
    return S_FALSE;
}

HRESULT CAccTreeItem::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    if (nIndex == 0)
    {
        HRESULT hr = g_accTreeView._get_accState(sender, m_sel, state);
        state |= (STATE_SYSTEM_SELECTED |  STATE_SYSTEM_FOCUSABLE | STATE_SYSTEM_FOCUSED);
        return hr;
    }
    return S_FALSE;
}

//HRESULT CAccTreeItem::_get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild)
//{
//    pvarFocusChild->v
//    return S_FALSE;
//}
//
//HRESULT CAccTreeItem::_get_accSelection(CAccessibleWnd& sender, long& children)
//{
//    return S_FALSE;
//}

HRESULT CAccTreeItem::_get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction)
{
    if (nIndex == 0)
        return g_accTreeView._get_accDefaultAction(sender, m_sel, szDefaultAction);
    return S_FALSE;
}

HRESULT CAccTreeItem::_accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect)
{
    if (nIndex == 0)
        return g_accTreeView._accSelect(sender, m_sel, flagsSelect);
    return S_FALSE;
}

HRESULT CAccTreeItem::_accItemLocation(CAccessibleWnd& sender, long nIndex, CTuiRect& rc)
{
    if (nIndex == 0)
        return g_accTreeView._accItemLocation(sender, m_sel, rc);
    return S_FALSE;
}

HRESULT CAccTreeItem::_accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt)
{
    if (varStart == 0)
        return g_accTreeView._accNavigate(sender, navDir, m_sel, pvarEndUpAt);
    return S_FALSE;
}

HRESULT CAccTreeItem::_accItemHitTest(CAccessibleWnd& sender, CTuiPoint& pt, long& child)
{
    return g_accTreeView._accItemHitTest(sender, pt, child);
}

HRESULT CAccTreeItem::_accDoDefaultAction(CAccessibleWnd& sender, long nIndex)
{
    if (nIndex == 0)
        return g_accTreeView._accDoDefaultAction(sender, m_sel);
    return S_FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// CAccCombTree
HTUI CAccCombTree::_get_accParent(CAccessibleWnd& sender)
{
    return CTuiComboUnit(sender).GetEdit();
}
