//-----------------------------------------------------------
//main.cpp
//
//  Copyright (C) 2009 miragekiller                               
//  All rights reserved                                                     
//
//
//  Author: 
//      miragekiller    <3916345933@qq.com>
//
//    Histroy :
//      2009.02.04      create
//-------------------------------------------------------------

#include "stdafx.h"
#include "main.h"
#include "tmgdi.h"
//#include "gx.cpp"

using namespace TM;

// template <class T, int nID>
// class CHookBase
// {
// protected:
//     CThisThunk  m_thunk;
//     HHOOK       m_hHook;
//     
// protected:
//     CHookBase ()
//         : m_hHook (0)
//     {
//     }
//     
// public:
//     void SetHook ()
//     {
//         if (!m_hHook)
//         {
//             m_thunk.SetCallBack (static_cast<T*>(this), &T::_HookProc);
//             m_hHook =  SetWindowsHookEx(
//                 nID, 
//                 (HOOKPROC)m_thunk.GetProcAddres(),
//                 NULL,
//                 GetCurrentThreadId());
//         }
//     }
//     
//     void RemoveHook ()
//     {
//         if (m_hHook)
//         {
//             UnhookWindowsHookEx (m_hHook);
//             m_hHook = 0;
//         }
//     }
//     
//     static TMBool IsValidWindow (TMHWnd hWnd, TMHWnd hMsgWnd)
//     {
//         return hWnd == hMsgWnd || IsChild (hWnd, hMsgWnd);
//     }
//     
// };
// 
// class CCallWndHook : public CHookBase <CCallWndHook, WH_CALLWNDPROC>
// {
// public:
//     static CCallWndHook s_a[50];
// 
// public:
//     CCallWndHook() { SetHook(); }
// 
//     TMResult _HookProc (int nCode, TMWParam wParam, TMLParam lParam)
//     {
//         DbgTrace ("CCallWndHook::_HookProc:nCode:%d; pos:%d", nCode, this - &(s_a[0]));
//         return CallNextHookEx (m_hHook, nCode, wParam, lParam);
//     }
//     
// public:
//     
// };
// 
// CCallWndHook CCallWndHook::s_a[];

//static CCallWndHook& x = CCallWndHook::s_a[0];

class CTVParam
//     : public CAnimation
{
private:
    CTVItem     m_itm;

    ~CTVParam () {}

public:
    CTVParam ()
//         : CAnimation (30, 10)
    {
    }

    void Release (CTVCtrl& tv)
    {
//         Stop ();
        delete this;
    }

    void CreateAni (HTVITEM itm, TMBool bForword)
    {
//         m_itm = itm;
//         RECT rc;
//         m_itm.GetRect(&rc);
//         SetCurrFrame (rc.bottom - rc.top - 14);
//         SetDirection (bForword);
//         Play ();
    }

//     virtual void OnAniStop ()
//     {
//         m_itm.SetHeight (IsForward() ? 40 : -1);
// 
//     }
// 
//     virtual void OnAniStep (int nPos)
//     {
//         int n = GetCurrFrame() + 14;
//         m_itm.SetHeight (n);
//     }
};

typedef CTVItemT<CTVParam*> CItemEX;


TMResult CMain::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_tv.SubclassWindow (Item(IDT_TV));
    m_tv.RegisterEventSink (this);
    m_tvLog = Item (IDT_LOG);
    return 0;
}

TMResult CMain::OnDelItem(...)
{
    CTVItem itm(m_tv.GetRootItem());
    CTVItem itmNew;

    for (; itmNew = itm.GetGlobalNext();)
    {
        if (itmNew.GetState() & TVIS_SELECTED)
        {
            itmNew.Remove ();
        }
        else
        {
            itm = itmNew;
        }
    }
    return 0;
}


void SetHeight (CTVItem itm, int n)
{
    for (; itm = itm.GetGlobalNext();)
    {
        if (itm.GetState() & TVIS_SELECTED)
        {
            int nh = itm.GetHeight();
            itm.SetHeight (nh + n);
        }
    }
}

TMResult CMain::OnDec(...)
{
    int n = Item(IDE_ADDTEXT).GetWindowTextLength();
//    m_tuiWindow.SetDlgItemText (IDE_ADDTEXT, _T("bbb"));
    SetHeight (m_tv.GetRootItem(), -1);
    return 0;
}

TMResult CMain::OnAdd(...)
{
//    m_tuiWindow.SetDlgItemText (IDE_ADDTEXT, _T("aaaaa"));
    SetHeight (m_tv.GetRootItem(), 1);
    return 0;
}


TMResult CMain::OnChange(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTuiWidget win ((TM::HTUI)hWndCtl);
    
    TM::CString str;
    int n = win.GetWindowText(str.GetBuffer(1024), 1023);
    str.SetLength (n);
    
    for (CTVItem itm(m_tv.GetRootItem()); itm = itm.GetGlobalNext();)
    {
        TM::CString strTxt;
        itm.GetText(strTxt);
        TMBool b = str.IsEmpty() || -1 != strTxt.FindI(str);
        itm.ShowItem (b, true);
    }

    return 0;
}

void CMain::AddItem(TMBool bHTML)
{
    int n = m_tuiWindow.GetDlgItemInt (IDE_NUM);
    TMCharT c[1024];
    m_tuiWindow.GetDlgItemText (IDE_ADDTEXT, c, 1023);
    
    CTVItem itm (m_tv.GetSelectedItem());
    if (!itm)
    {
        itm = m_tv.GetRootItem();
    }
    int nLevel = itm.GetLevel();
    
    m_tv.SetRedraw (false);
    for (int i = 0; i < n; ++i)
    {
        static int s_index = 0;
        CString str;
        str.Format(L"<b>item</b>(%d - %d)", nLevel, s_index++);
        CTVParam* p = new CTVParam();
        CItemEX itmNew = itm.InsertChild (str);
        if (bHTML)
            itmNew.SetState(TVIS_HTML, TVIS_HTML);
        itmNew.SetData(p, false);
        itmNew.SetHint(str);
    }
    m_tv.SetRedraw (true);
    m_tv.Invalidate ();
}


void CMain::AddLog (TMCStr p)
{
//    m_tvLog.AppendText(p);
//    m_tvLog.AppendText(TM_T("\r\n"));
}

void CMain::OnDeleteItem (HTVITEM hItem)
{
    CItemEX itm(hItem);
    itm.GetData()->Release (m_tv);

    CString str;
    str.Format (_T("OnDeleteItem - itm: %x;"), hItem);
    AddLog (str);
}

void CMain::OnStateChanged (HTVITEM hItem, TMUInt uOld, TMUInt uNew)
{
    CString str;
    str.Format (_T("OnStateChanged - itm: %x; old: %x; new: %x;"), hItem, uOld, uNew);
    AddLog (str);
}

void CMain::OnSelChanged (HTVITEM hOld, HTVITEM hNew,TMUInt uKey)
{
    if (hNew)
    {
        CItemEX(hNew).GetData()->CreateAni (hNew, true);
    }
    if (hOld)
    {
//        CItemEX(hOld).SetHeight (-1);

        CItemEX(hOld).GetData()->CreateAni (hOld, false);
    }

    CString str;
    str.Format (_T("OnSelChanged - hOld: %x; hNew: %x;"), hOld, hNew);
    AddLog (str);
}
