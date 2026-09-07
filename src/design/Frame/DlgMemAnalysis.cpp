/**
 *  @file
 *  @brief
 *  CDlgMemAnalysis
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025-6-16
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2025 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgMemAnalysis.h"
#include "tmdebugext.h"
#include "MemHook.h"

using namespace TM;


///////////////////////////////////////////////////////////////////////////////
// CProxy
#define IND_COUNT           0
#define IND_SIZE            1
#define IND_SUM             2
#define IND_DEL             3
#define IND_SIZEDEL         4
#define IND_SUMDEL          5
#define IND_HASH            6

struct CProxy : public IColumnProxy
{
    virtual void GetRenderText(TMLParam lpTarget, TMLParam lpHeaderItem, CText& text) tm_final
    {
        CCallBackMap::value_type* p = (CCallBackMap::value_type*)lpTarget;
        TM::CString str;
        if (lpHeaderItem == IND_COUNT)
            str.Format(L"%d", p->second.m_nCount);
        else if (lpHeaderItem == IND_SIZE)
            str = MEM2T(p->second.m_nSize);
        else if (lpHeaderItem == IND_SUM)
            str = MEM2T(p->second.m_nSum);
        else if (lpHeaderItem == IND_DEL)
            str.Format(L"%d", p->second.m_nDel);
        else if (lpHeaderItem == IND_SIZEDEL)
            str = MEM2T(p->second.m_nSizeDel);
        else if (lpHeaderItem == IND_SUMDEL)
            str = MEM2T(p->second.m_nSumDel);
        else
            str.Format(L"%p", p->first.m_hash);
        text.ReCreate(str, false);
    }
    
    virtual CString GetHInt(TMLParam lpTarget, TMLParam lpHeaderItem) tm_final
    {
        return CString();
    }
    
    virtual bool Sort(TMLParam lpTarget1, TMLParam lpTarget2, TMLParam lpHeaderItem, TMBool bSortUp) tm_final
    {
        CCallBackMap::value_type* p1 = (CCallBackMap::value_type*)lpTarget1;
        CCallBackMap::value_type* p2 = (CCallBackMap::value_type*)lpTarget2;
        if (lpHeaderItem == IND_COUNT)
            return p1->second.m_nCount < p2->second.m_nCount;
        else if (lpHeaderItem == IND_SIZE)
            return p1->second.m_nSize < p2->second.m_nSize;
        else if (lpHeaderItem == IND_DEL)
            return p1->second.m_nDel < p2->second.m_nDel;
        else if (lpHeaderItem == IND_SIZEDEL)
            return p1->second.m_nSizeDel < p2->second.m_nSizeDel;
        return 0;
    }
};
static CProxy s_proxy;

///////////////////////////////////////////////////////////////////////////////
// CDlgMemAnalysis
CDlgMemAnalysis::CDlgMemAnalysis()
{
}

CDlgMemAnalysis::~CDlgMemAnalysis()
{
}

TMResult CDlgMemAnalysis::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    CAutoHookMainThread autoHook;

    Init();

    CTuiHeader header(Item(IDC_HEADER_30401));
    HDITEM itm = { HDI_TEXT | HDI_WIDTH | HDI_FORMAT | HDI_LPARAM, 50, (TMStrW)L"count", 0, 0, HDF_AUTOSORTUP | HDF_AUTOSORTDOWN | HDF_BITMAP_ON_RIGHT };
    itm.lParam = IND_COUNT;
    header.InsertItem(itm.lParam, &itm);

    itm.cxy = 100;
    itm.pszText = (TMStrW)L"size";
    itm.lParam = IND_SIZE;
    header.InsertItem(itm.lParam, &itm);

    itm.pszText = (TMStrW)L"sum";
    itm.lParam = IND_SUM;
    itm.fmt = 0;
    header.InsertItem(itm.lParam, &itm);

    itm.cxy = 50;
    itm.fmt = HDF_AUTOSORTUP | HDF_AUTOSORTDOWN;
    itm.lParam = IND_DEL;
    itm.pszText = (TMStrW)L"dels";
    header.InsertItem(itm.lParam, &itm);
    
    itm.pszText = (TMStrW)L"size del";
    itm.cxy = 100;
    itm.lParam = IND_SIZEDEL;
    header.InsertItem(itm.lParam, &itm);
    
    itm.pszText = (TMStrW)L"sum del";
    itm.lParam = IND_SUMDEL;
    itm.fmt = 0;
    header.InsertItem(itm.lParam, &itm);

    itm.pszText = (TMStrW)L"hash";
    itm.lParam = IND_HASH;
    itm.fmt = 0;
    header.InsertItem(itm.lParam, &itm);


    CTuiListBox lb(Item(IDC_LISTBOX_30395));
    lb.SetHeaderProxy(header, &s_proxy);


//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgMemAnalysis::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgMemAnalysis::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgMemAnalysis::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgMemAnalysis::OnSelChange1(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    CAutoHookMainThread autoHook;

    CTuiListBox lb((HTUI)hWndCtl);
    int n = lb.GetCurSel();
    CallBackInfo* p = (CallBackInfo*)lb.GetItemDataPtr(n);

    CTuiListBox lb2(Item(IDC_LISTBOX_30397));
    lb2.ResetContent();

    for (int i = 0; i < p->first.m_nCallBack; ++i)
    {
        void* f = p->first.m_callback[i];
        int j = lb2.AddString(GetCallBackStr(f));
        lb2.SetItemDataPtr(j, f);
        lb2.SetItemHint(j, GetSourceAndLine(f));
    }

    return 0;
}

TMResult CDlgMemAnalysis::OnSelChange2(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    CAutoHookMainThread autoHook;

    CTuiListBox lb((HTUI)hWndCtl);
    int n = lb.GetCurSel();
    void* p = lb.GetItemDataPtr(n);
    JumpToSource(p);

    return 0;
}

struct ENUMS
{
    int nAllSize;
    int nAllSizeDel;
};
bool CALLBACK FN_Enum(int nIndex, TMLParam dataItem, TMLParam dataUser)
{
    ENUMS* pu = (ENUMS*)dataUser;
    CCallBackMap::value_type* pi = (CCallBackMap::value_type*)dataItem;
    pu->nAllSize += pi->second.m_nSize;
    pi->second.m_nSum = pu->nAllSize;
    pu->nAllSizeDel += pi->second.m_nSizeDel;
    pi->second.m_nSumDel = pu->nAllSizeDel;
    return 0;
}

TMResult CDlgMemAnalysis::OnSorted1(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    ENUMS n = {0};
    CTuiListBox lb((HTUI)hWndCtl);
    lb.Enum(FN_Enum, (TMLParam)&n);
    return 0;
}

TMResult CDlgMemAnalysis::OnHeaderClicked(int idCtrl, NMBASE* pnmh, TMBool& bHandled)
{
    return 0;
}



//CTuiMsgFilter
// BOOL CDlgMemAnalysis::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }

void CDlgMemAnalysis::Init()
{
    CTuiListBox lb(Item(IDC_LISTBOX_30395));

    CCallBackMap& aMap = BeginGetCallBackMap();
    for (CCallBackMap::recorder rec(aMap); rec; ++rec)
    {
        CString str;
        CCallBackMap::value_type* p = &(*rec.get_iter());
        if (p->second.m_nCount)
        {
            str.Format(L"%6d %16s %p", p->second.m_nCount, MEM2T(p->second.m_nSize).c_str(), p->first.m_hash);
            int n = lb.AddString(str);
            lb.SetItemDataPtr(n, p);
        }
    }

    EndGetCallBackMap();

}

