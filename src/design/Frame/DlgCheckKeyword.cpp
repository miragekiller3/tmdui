/**
 *    @file
 *    @brief
 *    CDlgCheckKeyword
 *
 *
 *    @author   miragekiller <3916345933@qq.com>
 *    @date     2017-6-13
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgCheckKeyword.h"
#include "Tools.h"
#include "projmgr.h"
#include "tmfilename.h"
#include "tmwnd.h"

using namespace TM;

///////////////////////////////////////////////////////////////////////////////
// CDlgCheckKeyword
CDlgCheckKeyword::CDlgCheckKeyword()
{
}

CDlgCheckKeyword::~CDlgCheckKeyword()
{
}

TMResult CDlgCheckKeyword::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//    m_objCur = (HTVITEM)lParam;
    TM::CFileName fn;
    fn.GetCurrentDirectory();
    Item(IDC_EDIT_30253).SetWindowText(fn);
    return 0;
}

TMResult CDlgCheckKeyword::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgCheckKeyword::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgCheckKeyword::OnImport (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    m_aListParent.aID.clear();
    TM::CTuiMemo memo(Item(IDC_MEMO_30264));
    
    CTuiListBox lb(Item(IDC_LISTBOX_31090));
    int n = lb.GetCurSel();
    if (n <= 0)
    {
        CString strKey = m_panel->GetProj()->GetPreSearchID(m_aListParent);
        memo.AppendText(strKey);

//         CObjItem itm(m_objCur);
//         for (itm = itm.GetChild(); itm; itm = itm.GetNext())
//         {
//             IObjEdit* pData = itm.GetData();
//             TM::CString str2;
//             pData->ID2Text(str2);
//             str2 += L"\r\n";
//             memo.AppendText(str2);
//         }
    }
    else
    {
        TM::CString strPath;
        Item(IDC_EDIT_30253).GetWindowText(strPath);
        CKeyWord kw;
        kw.SearchFile(strPath, (EKeyType)(n - 1 + eKeyTypeMacro));
        
        CString str;
        for (CKeyWord::CKeyList::recorder rec(kw.m_aKeys); rec; ++rec)
        {
            str += rec.get_key();
            str += "\r\n";
        }
        Item(IDC_MEMO_30264).SetWindowText(str);
    }

    return 0;
}

TMResult CDlgCheckKeyword::OnSelDir (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CPathDialog dlg;
    dlg.DoModal (GetHWND(), _T("Select Source File root directory\r\nSee report on DebugView or VC Debug Window"));
    TM::CString str;
    if (dlg.GetPath(str.GetBuffer(MAX_PATH)))
        Item(IDC_EDIT_30253).SetWindowText(str);
    return 0;
}

TMResult CDlgCheckKeyword::OnStart (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    // init
    int nType = TM::CTuiRadioButton(Item(IDC_RADIOBUTTON_30258)).GetCheckedButton().GetDlgCtrlObjectID() - IDC_RADIOBUTTON_30258;
    TM::CString str;
    Item(IDC_MEMO_30264).GetWindowText(str);
    
    TM::CString strPath;
    Item(IDC_EDIT_30253).GetWindowText(strPath);

    // search
    CKeyWord kw;
    kw.ParseFile(str, (EKeyType)nType);
    CheckKeyword(strPath, kw, m_aLines);

    // process parent list
    tm_for (CPreSearchID::CParentList::recorder rec(m_aListParent.aID); rec; ++rec)
    {
        const TM::CStringA& str = rec.get_key();
        CLineMap::recorder reclinenow(m_aLines);
        reclinenow.find(str);
        if (!reclinenow)
            continue;
        if (reclinenow.get_data().aResult.size() <= 1) // skip unused macro.
            continue;

        tm_for (CPreSearchID::CMacoList::recorder recp(rec.get_data()); recp; ++recp)
        {
            CLineMap::recorder reclinep(m_aLines);
            reclinep.find(recp.get_key());
            if (reclinep)
                reclinep.get_data().aChilds.insert(str);
        }
    }

    // remove unused id
    tm_for (CPreSearchID::CMacoList::recorder rec(m_aListParent.aDel); rec; ++rec)
        m_aLines.erase(rec.get_key());

    // add result
    OnSort();

    TM::CTuiTabButton(Item(IDC_TABBUTTON_30266)).SetCheck(1);
    return 0;
}

void AddList(TM::CTuiTree lb, const CSearchResult& sr)
{
    TM::CString strn;
    strn.Format(L"%d-%d-%d-%d ", sr.aResult.size(), sr.aChilds.size(), sr.nRefByFile, sr.nRefByBody);
    strn += sr.keyContext;
    strn += (*sr.aResult.begin()).pathname;
    CTVItem itm = lb.GetRootItem().AppendChild(strn);
    itm.SetData((void*)&sr);
}

struct LBST
{
    CTuiTree lb;
    TMBool bByRef;
};

//bool CALLBACK lbsort(TMLParam data1, TMLParam data2, TMLParam lParamSort)
TMBool CALLBACK lbsort(HTVITEM hItem1, HTVITEM hItem2, TMLParam lParamSort)
{
    LBST* pst = (LBST*)lParamSort;
    CSearchResult* s1 = (CSearchResult*)CTVItem(hItem1).GetData();;
    CSearchResult* s2 = (CSearchResult*)CTVItem(hItem2).GetData();;
    if (pst->bByRef)
    {
        int n1 = s1->aResult.size();
        int n2 = s2->aResult.size();
        if (n1 != n2)
            return n1 < n2;
    }
    return (*s1->aResult.begin()).pathname < (*s2->aResult.begin()).pathname;
}

TMResult CDlgCheckKeyword::OnSort (...)
{
//    TMBool bMergeLine = CTuiRadioButton(Item(IDC_CHECKBOX_31085)).GetCheck();

    TM::CTuiTree lb(Item(IDC_LISTBOX_30252));
    lb.ResetContent();

    for (CLineMap::recorder rec(m_aLines); rec; ++rec)
    {
        const CSearchResult& srx = rec.get_data();
        AddList(lb, srx);
    }

    LBST lbst;
    lbst.lb = lb;
    lbst.bByRef = !CTuiRadioButton(Item(IDC_RADIOBUTTON_31083)).GetCheck();
    lb.GetRootItem().Sort(lbsort, (TMLParam)&lbst, 999, false);

    return 0;
}


TMResult CDlgCheckKeyword::OnSelChange (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTuiTree lb(Item(IDC_LISTBOX_30252));
    CTVItem itm = lb.GetSelectedItem();
    if (!itm)
        return 0;
    CSearchResult* s1 = (CSearchResult*)itm.GetData();
    
    CString str;
    str.Format(L"%d ", s1->aResult.size());
    str += s1->key;
    str += L": ";
    str += s1->keyContext;
    str += L"\r\n";
    OutputDebugString(str);
    m_panel->SelectByString(TM_A2T(s1->key));

    tm_for (CSearchResult::CSRList::recorder rec(s1->aResult); rec; ++rec)
    {
        const CSearchResult::SIMPLERST& sr = rec.get_data();
        str = sr.pathname;
        str += L"(";
        str += TM_L2T(sr.nLine);
        str += L"): ";
        str += sr.line;
        str += L"\r\n";
        OutputDebugString(str);
    }

    if (s1->aChilds.size())
    {
        OutputDebugStringA("childs: ");
        tm_for (CPreSearchID::CMacoList::recorder rec(s1->aChilds); rec; ++rec)
        {
            OutputDebugStringA(rec.get_key());
            OutputDebugStringA(", ");
        }
        OutputDebugString(L"\r\n");
    }

    OutputDebugString(L"\r\n");
    return 0;
}

TMResult CDlgCheckKeyword::OnEnChange (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTuiTree tv(Item(IDC_LISTBOX_30252));
    CString strBefore;
    CString strAfter;
    Item(IDC_MEMO_30269).GetWindowText(strBefore);
    Item(IDC_MEMO_30270).GetWindowText(strAfter);
    for (CTVItem itm(tv.GetRootItem().GetChild()); itm; itm = itm.GetNext())
    {
        TMBool bShow = true;
        CSearchResult* s1 = (CSearchResult*)itm.GetData();
        for (CSearchResult::CSRList::recorder rec(s1->aResult); rec; ++rec)
        {
            const CSearchResult::SIMPLERST& sr = rec.get_data();
            int n = sr.line.Find(s1->key);
            DbgAssert(n >= 0);
            CStrViewA str1(sr.line.data(), n);
            CStrViewA str2(sr.line.data() + n + s1->key.GetLength());// , &*sr.line.end());
            CStringA rgkey;
            tm_for (int n2 = 0; n2 = strBefore.SplitLine(n2, rgkey);)
            {
                if (str1.Find(rgkey) >= 0)
                {
                    bShow = false;
                    break;
                }
            }
            if (!bShow)
                break;
            tm_for (int n2 = 0; n2 = strAfter.SplitLine(n2, rgkey);)
            {
                if (str2.Find(rgkey) >= 0)
                {
                    bShow = false;
                    break;
                }
            }
            if (!bShow)
                break;
        }
        itm.ShowItem(bShow, false);
    }
    return 0;
}
