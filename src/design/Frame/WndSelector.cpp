/**
 *    @file
 *    @brief
 *    CWndSelector
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2019-5-29
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2019 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "WndSelector.h"
#include "MainWnd.h"
#include "ProjMgr.h"
#include "../Design.h"

using namespace TM;

#define INDICATE_W      140
#define INDICATE_H      20

CWndSelector::CWndSelector()
{
}

CWndSelector::~CWndSelector()
{
}

CWndPanel& CWndSelector::GetPanel()
{
    return TM_OTHIS(CMainWnd, m_wndSelector)->GetPanel();
}

TMResult CWndSelector::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CWndSelector::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CWndSelector::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CWndSelector::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CWndSelector::OnChildLButtonDown(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiWidget wnd((TM::HTUI)hWndCtl);
    GetPanel().Goto(wnd.GetTag());
    return 0;
}

// rc.left,top: current pos; rc.right,bottom: max right, bottom
void CWndSelector::EnumProc(IProjMgr* mgr, IObjEdit* obj, TM::CTuiWidget wndParent, const TM::CTuiPoint& pt, TMBool bRoot, TM::CTuiRect& rc)
{
    float fTop = rc.top;
    for (; obj; obj = mgr->ItemGetNext(obj))
    {
        CTuiRect rcw;
        if (obj->GetObjRect(rcw) && rcw.PtInRect(pt))
        {
            CTuiWidget w;
            w.Create(wndParent, IDC_RADIOBUTTON_30332);
            TM::CString strMacro(obj->ID2Text());
            CString strClass;
            TMUInt64 uid = obj->GetId();
#ifdef TM_QTSPY
            strClass.Format(L"%s", obj->GetObjClassName().c_str());
#else
            strClass.Format(L"%s(%lld)", obj->GetObjClassName().c_str(), uid);
#endif
            w.SetWindowText(strClass);
            w.SetHint(strMacro);
            w.SetTag((TMLParam)uid);
            if (!(obj->GetObjStyle() & eObjStyleVisible))
                w.SetTextColor(e_bsNormal, TMRGB(128,128,128));
            float fLeft = rc.left;
            rc.top += INDICATE_H;
            EnumProc(mgr, mgr->ItemGetChild(obj), wndParent, pt, false, rc);
            if (rc.left == fLeft)
                rc.left += INDICATE_W;
            w.MoveWindow(fLeft, fTop, rc.left - fLeft, INDICATE_H);
            tm_setmax(rc.right, rc.left);
            tm_setmax(rc.bottom, rc.top);
            rc.top = fTop;
        }
        else
            EnumProc(mgr, mgr->ItemGetChild(obj), wndParent, pt, false, rc);

        if (bRoot)
            break;
    }
}

void CWndSelector::Selector(const TM::CTuiPoint& pt, IProjMgr* mgr)
{
    tm_for (CTuiWidget wnd; wnd = GetWindow(GW_CHILD);)
        wnd.DestroyWindow();

    CTuiRect rc;
    GetClientRect(&rc);
    rc.SetWidth(0);
    rc.SetHeight(0);

    float fLeft = rc.left;
    float fTop = rc.top;
    EnumProc(mgr, mgr->ItemGetSelectedData(true), *this, pt, true, rc);
    SetWindowPos(0, 0, 0, rc.right - fLeft, rc.bottom - fTop, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
//     wnd = GetWindow(GW_CHILD);
//     wnd.SetLayOut(LAYOUT_AUTOWIDTH);
//     if (nMaxLevel)
//     {
//         wnd.SetWindowPos(0, 0, 0, INDICATE_W, nMaxLevel * INDICATE_H,
//             SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
// //        wnd.ChangeFrame();
//     }
    return;
}


//CTuiMsgFilter
// BOOL CWndSelector::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
