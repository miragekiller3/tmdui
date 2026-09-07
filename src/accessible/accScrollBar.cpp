/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.08.30
 *
 *  Copyright (C) 2009-2018 miragekiller
 */


#include "stdafx.h"
#include "accScrollBar.h"

using namespace TM;
//using namespace TM::PRI;

///////////////////////////////////////////////////////////////////////////////
// CAccScrollBar
CAccScrollBar g_accScrollBarV;
CAccScrollBar g_accScrollBarH;

TMBool CAccScrollBar::IsVScroll() const
{
    return this == &g_accScrollBarV;
}

long CAccScrollBar::_get_accItemCount(CAccessibleWnd& sender)
{
    return eCount;
}

HRESULT CAccScrollBar::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    static LPCWSTR p[] = {
        L"Vertical",
        L"Line up",
        L"Page up",
        L"Position",
        L"Page down",
        L"Line down",
        L"Horizontal",
        L"Column left",
        L"Page left",
        L"Position",
        L"Page right",
        L"Column right",
    };
    if (nIndex > eCount || nIndex < 0)
        return DISP_E_MEMBERNOTFOUND;

    name = p[nIndex + IsVScroll() ? (eCount + 1) : 0 ];
    return S_OK;
}

HRESULT CAccScrollBar::_get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val)
{
    if (nIndex != 0)
        return S_FALSE;

    TUISCROLLINFO info = {0};
    if (IsVScroll())
        sender.GetScrollInfo(0, &info);
    else
        sender.GetScrollInfo(&info, 0);
    CString str;
    str.Format(L"%d", info.nOrg);
    val = str;
    return S_OK;
}

long CAccScrollBar::_get_accRole(HTUI hWgt, long nIndex)
{
    switch (nIndex)
    {
    case eIndexSelf:
        return ROLE_SYSTEM_SCROLLBAR;
        
    case eIndexUp:
    case eIndexDown:
    case eIndexPageUp:
    case eIndexPageDown:
        return ROLE_SYSTEM_PUSHBUTTON;
        
    case eIndexThumb:
        return  ROLE_SYSTEM_INDICATOR;
    }

    return S_FALSE;
}

HRESULT CAccScrollBar::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    TMUInt32 dwLayout = sender.GetLayOut();
    TMUInt32 nMask = IsVScroll() ? LAYOUT_VSCROLL : LAYOUT_HSCROLL;
    if (!(dwLayout & nMask) || !sender.IsWindowVisible())
    {
        state = STATE_SYSTEM_INVISIBLE;
        return S_OK;
    }

    state = 0;
    if ((nIndex == eIndexThumb) && (sender.IsFocus()))
        state = STATE_SYSTEM_FOCUSED;

    TUIHITINFO ht = {0};
    sender.GetAPP()->GetMouseLDownState(ht);
    if (ht.hWnd == sender)
    {
        TMUInt8 oid = IsVScroll() ? e_objVScroll : e_objHScroll;
        EHitID hid(ht.hid);
        if (TUIHITGETOBJ(hid) == oid && TUIHITGETSUBSB(hid) < eCount)
        {
            long c[eCount] = { eIndexUp, eIndexDown, eIndexPageUp, eIndexPageDown, eIndexThumb };
            if (c[TUIHITGETSUBSB(hid)] == nIndex)
                state |= STATE_SYSTEM_PRESSED;
        }
    }

    return S_OK;
}

HRESULT CAccScrollBar::_get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction)
{
    switch (nIndex)
    {
    case eIndexUp:
    case eIndexPageUp:
    case eIndexPageDown:
    case eIndexDown:
        szDefaultAction = L"Press";
        return S_OK;
    }
    return S_FALSE;
}

HRESULT CAccScrollBar::_accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc)
{
    if (!nIndex)
        return S_OK;

    TUISCROLLBARINFO wb = {0};
    GXDips* l, *r;
    if (IsVScroll())
    {
        sender.GetScrollBarInfo(0, &wb);
        l = &wb.rc.top; r = &wb.rc.bottom;
    }
    else
    {
        sender.GetScrollBarInfo(&wb, 0);
        l = &wb.rc.left; r = &wb.rc.right;
    }
    if (((CTuiRect&)(wb.rc)).IsRectEmpty())
        return S_FALSE;
    
    switch (nIndex)
    {
    case eIndexSelf: rc = wb.rc; return S_OK;
    case eIndexUp: *r = *l + wb.nButtonHeight; return S_OK;
    case eIndexDown: *l = *r - wb.nButtonHeight; return S_OK;
    case eIndexPageUp: *r = *l + wb.nThumbTop; *l += wb.nButtonHeight; return S_OK;
    case eIndexPageDown: *l += wb.nThumbBottom; *r -= wb.nButtonHeight; return S_OK;
    case eIndexThumb: *r = *l + wb.nThumbBottom; *l += wb.nThumbTop; return S_OK;
    }
    return S_FALSE;
}

HRESULT CAccScrollBar::_accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt)
{
    return S_FALSE;
//     long    lEndUp = 0;
//     SCROLLBARINFO sbi;
//     
//     InitPvar(pvarEnd);
//     
//     //
//     // Validate params
//     //
//     if (! ValidateChild(&varStart)   ||
//         ! ValidateNavDir(dwNavDir, varStart.lVal))
//         return(E_INVALIDARG);
//     
//     if (! MyGetScrollBarInfo(m_hwnd, (m_fVertical ? OBJID_VSCROLL : OBJID_HSCROLL),
//         &sbi))
//     {
//         return(S_FALSE);
//     }
//     
//     if (dwNavDir == NAVDIR_FIRSTCHILD)
//     {
//         dwNavDir = NAVDIR_NEXT;
//     }
//     else if (dwNavDir == NAVDIR_LASTCHILD)
//     {
//         dwNavDir = NAVDIR_PREVIOUS;
//         varStart.lVal = m_cChildren + 1;
//     }
//     else if (varStart.lVal == INDEX_SCROLLBAR_SELF)
//         return(GetParentToNavigate((m_fVertical ? OBJID_VSCROLL : OBJID_HSCROLL),
//         m_hwnd, OBJID_WINDOW, dwNavDir, pvarEnd));
//     
//     FixUpScrollBarInfo(&sbi);
//     
//     switch (dwNavDir)
//     {
//     case NAVDIR_NEXT:
// FindNext:
//         lEndUp = varStart.lVal;
//         
//         while (++lEndUp <= INDEX_SCROLLBAR_MAC)
//         {
//             if (!(sbi.rgstate[lEndUp] & STATE_SYSTEM_INVISIBLE))
//                 break;
//         }
//         
//         if (lEndUp > INDEX_SCROLLBAR_MAC)
//             lEndUp = 0;
//         break;
//         
//     case NAVDIR_PREVIOUS:
// FindPrevious:
//         lEndUp = varStart.lVal;
//         
//         while (--lEndUp >= INDEX_SCROLLBAR_MIC)
//         {
//             if (!(sbi.rgstate[lEndUp] & STATE_SYSTEM_INVISIBLE))
//                 break;
//         }
//         
//         if (lEndUp < INDEX_SCROLLBAR_MIC)
//             lEndUp = 0;
//         break;
//         
//     case NAVDIR_UP:
//         lEndUp = 0;
//         if (m_fVertical)
//             goto FindPrevious;
//         break;
//         
//     case NAVDIR_LEFT:
//         lEndUp = 0;
//         if (!m_fVertical)
//             goto FindPrevious;
//         break;
//         
//     case NAVDIR_DOWN:
//         lEndUp = 0;
//         if (m_fVertical)
//             goto FindNext;
//         break;
//         
//     case NAVDIR_RIGHT:
//         lEndUp = 0;
//         if (!m_fVertical)
//             goto FindNext;
//         break;
//         
//     default:
//         AssertStr("Invalid NavDir");
//     }
//     
//     if (lEndUp != INDEX_SCROLLBAR_SELF)
//     {
//         pvarEnd->vt = VT_I4;
//         pvarEnd->lVal = lEndUp;
//         return(S_OK);
//     }
//     else
//         return(S_FALSE);
}


HRESULT CAccScrollBar::_accItemHitTest(CAccessibleWnd& sender, TM::CTuiPoint& pt, long& child)
{
    TM::TUIHITINFO ht = {0};
    ht.pt.x = pt.x;
    ht.pt.y = pt.y;
    sender.HitTest(ht);
    if (ht.hWnd == sender && ht.bNC)
    {
        EHitID hid = ht.hid;
        if (((TUIHITGETOBJ(hid) == e_objHScroll) && !IsVScroll() || (TUIHITGETOBJ(hid) == e_objVScroll) && IsVScroll()) && (TUIHITGETSUBSB(hid) <= SB_THUMBPOSITION))
        {
            long c[eCount] = { eIndexUp, eIndexDown, eIndexPageUp, eIndexPageDown, eIndexThumb };
            child = c[TUIHITGETSUBSB(hid)];
            return S_OK;
        }
    }
    return S_FALSE;
}



