/**
 *    @file
 *    @brief
 *    CWndTVSearch
 *
 *
 *    @author 
 *    @date    2014-5-1
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2014 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "WndTVSearch.h"
#include "../Design.h"


CWndTVSearch::CWndTVSearch()
//     : m_nTxtLen(0)
{
}

CWndTVSearch::~CWndTVSearch()
{
}

// TMResult CWndTVSearch::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     return 0;
// }
// 
// TMResult CWndTVSearch::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
// //    EndDialog(wID);
//     DestroyWindow();
//     return 0;
// }
// 
// TMResult CWndTVSearch::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
// //    EndDialog(wID);
//     DestroyWindow();
//     return 0;
// }

TMResult CWndTVSearch::OnEnChange(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    DbgAssert(m_wndTree);
    TM::CTuiWidget win ((TM::HTUI)hWndCtl);
    m_txtKey = win.GetWindowText();
//    str.SetLength (m_nTxtLen);
//    return 0;
    TM::CTuiTree tree(m_wndTree);
    for (TM::CTVItem rec(tree.GetRootItem()); rec; rec = rec.GetGlobalNext())
    {
        ITreeItem* prop = (ITreeItem*)rec.GetData();
        if (prop)
//            prop->SetVisible(rec, str);
        {
            TM::CStrView strTxt(rec.GetText());
            int nRootLevel = 1;
            TMBool b = prop->Match(strTxt, m_txtKey, nRootLevel);

            int nLevel = rec.GetLevel();
            if (nLevel == nRootLevel) // is dilaog, pic, frame
            {
                rec.ShowItem (b, false);
            }
            else if (b && (nLevel > nRootLevel)) // if is window, show dialog
            {
                TM::CTVItem tv = rec;
                for (; nLevel > nRootLevel; --nLevel)
                {
                    tv = tv.GetParent();
                }
                tv.ShowItem (b, false);
            }
        }
    }
   
    return 0;
}

TMResult CWndTVSearch::OnEditClear(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTuiWidget win (TM::CTuiWidget((TM::HTUI)hWndCtl).GetParent());
    win.SetWindowText((TMCStr)0);
    return 0;
}

TMResult CWndTVSearch::OnGotoSel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTVItem itm(m_wndTree.GetSelectedItem());
    
    if (m_txtKey.IsEmpty())
    {
        if (itm)
            itm.EnsureVisible(); // not filter, ensure select item
    }
    else
    {
        if (itm)
            itm = itm.GetGlobalNext();
        if (!itm)
            itm = m_wndTree.GetRootItem();
        for (; itm; itm = itm.GetGlobalNext())
        {
            IObjEdit* prop = (IObjEdit*)itm.GetData();
            if (prop && prop->IsFilter())
            {
                itm.SelectItem();
                break;
            }
        }
    }

    return 0;
}
