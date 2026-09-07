/**
 *    @file
 *    @brief
 *    CWndPad
 *
 *
 *  @author miragekiller <3916345933@qq.com>
 *    @date    2017-11-1
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "WndSpy.h"
#include "ProcessEmbed.h"
#include "resource.h"
#include "FactoryData.h"

using namespace TM;

///////////////////////////////////////////////////////////////////////////////
// CTrackFocusSpy
void CTrackFocusSpy::Enable(BOOL bEnable)
{
    if (bEnable)
        AppGetTester()->AddMessageFilter(this);
    else
    {
        AppGetTester()->RemoveMessageFilter(this);
        TrackFocus(0);
    }
}

BOOL CTrackFocusSpy::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
{
    switch (pMsg->message)
    {
    case CM_APPMESSAGE:
        if (((TM::TUIMSG*)pMsg->lParam)->message == WM_WINDOWPOSCHANGED)
            TrackFocus(m_focus);
        break;
        
    case WM_KILLFOCUS:
    case WM_SETFOCUS:
        TrackFocus(AppGetTester()->GetFocus());
        break;
        
    case WM_SIZE:
    case WM_MOVE:
        if (CTuiWidget(pMsg->hWnd).IsFocus())
            TrackFocus(pMsg->hWnd); // _OnSetFocus(m_wndPanel, pMsg->hWnd, GetHWND());
        break;
    }
    return false;
}

void CTrackFocusSpy::TrackFocus(HTUI hWgt)
{
//     if (hWgt == m_focus)
//         return;

    CString strAlert;
    if (hWgt != m_focus)
    {
        strAlert.Format(L"focus change: %p(%5d) -> %p(%5d)\r\n",
            m_focus.m_hWnd, m_focus ? m_focus.m_hWnd->GetObjectID() : 0,
            hWgt,     hWgt ?  hWgt->GetObjectID() : 0);
        m_focus = hWgt;
    }
    
    CRect rc;
    if (hWgt)
    {
        CTuiRect rcw;
        m_focus.GetWindowRect(&rcw);
        rcw.InflateRect(4, 4);
        if (rcw.IsRectEmpty())
        {
            DbgTrace("spy focus error: foucs window rect is empty(oid:%d; rect:%g,%g,%g,%g)",
                m_focus.GetDlgCtrlObjectID(), rcw.Left(), rcw.Top(), rcw.Right(), rcw.Bottom());
            return;
        }
        
        m_focus.ClientToScreen(rcw, rc);
        rc.NormalizeRect();
    }

    CPanelSpy* ps = TM_OTHIS(CPanelSpy, m_ts);
    ps->GetProjMgr()->OnTrackFocus(hWgt, rc, strAlert);
}


///////////////////////////////////////////////////////////////////////////////
// CPanelSpy
CPanelSpy::CPanelSpy(PANELINFO& info)
    : CPanelApp(info)
{
    AppGetTester()->AddMessageFilter(this);
    TrackFocusObject(true);
}

CPanelSpy::~CPanelSpy()
{
    DbgTrace("CPanelSpy::~CPanelSpy");
    AppGetTester()->RemoveMessageFilter(this);
    TrackFocusObject(false);
}


// IProj
TMResult CPanelSpy::TrackMenuPop(HWND hWnd, TMPoint pt, HMENU hMenuRoot, IObjEdit* pSel)
{
    if (pSel < objEditMax)
        return 0;
    
    CMenuHandle muMain = hMenuRoot;
    CMenuHandle muPop = muMain.GetSubMenu(1);
    TM::EObjType eType = pSel->GetType();

    muPop.DeleteMenu(IDM_DIALOG_PROPERTY, MF_BYCOMMAND);
    muPop.DeleteMenu(IDM_ITEM_DELETE, MF_BYCOMMAND);
 
    if (eType != TM::e_otWgt) // EM_WGT)
    {
        muPop.DeleteMenu(IDM_MOVE_UP, MF_BYCOMMAND);
        muPop.DeleteMenu(IDM_MOVE_DOWN, MF_BYCOMMAND);
    }

    if (eType != TM::e_otWnd)
    {
        muPop.DeleteMenu(IDM_P_EMBED_WGT, MF_BYCOMMAND);
        muPop.DeleteMenu(IDM_WGT_EMBED_P, MF_BYCOMMAND);
    }

    if (eType != TM::e_otWgt || eType != TM::e_otWnd)
        muPop.DeleteMenu(IDM_SPY_MSG_OBJ, MF_BYCOMMAND);
    
    //     if (eType != TM::e_otWnd) // EM_DIALOG)
    //         muPop.DeleteMenu(IDM_TOOL_TEST, MF_BYCOMMAND);
    //     
    //     if (eType != TM::e_otWnd && eType != TM::e_otWgt)// EM_DIALOG && eMode != EM_WGT)
    //     {
    //         muPop.DeleteMenu(IDM_TOOL_ADDFILE, MF_BYCOMMAND);
    //         muPop.DeleteMenu(IDM_TOOL_DELFILE, MF_BYCOMMAND);
    //     }
    
    int n = muPop.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, hWnd);
    if (n == IDM_P_EMBED_WGT)
        SpySetEmbed(0);
    else if (n == IDM_WGT_EMBED_P)
        SpySetEmbed(1);
    else
        return n;
    return 0;
} 

CString CPanelSpy::LoadFromFile(LPCTSTR pFileName, TMBool bMerge)
{
    CView& view = AppGetTester()->GetView();
    HMODULE hModule = GetModuleHandle(pFileName);
    CModule* pm = view.GetModule(hModule);
    if (!pm)
    {
        m_pmCurrent = 0;
        return pFileName;
    }
    m_pmCurrent = pm;
    for (CModule::CObjList::recorder rec(pm->m_aObjs); rec; ++rec)
    {
        CTuiObject* pObj = static_cast<CTuiObject*>(rec.get_data());
        if (pObj->GetType() != e_otWgt)
            SyncSpyItem(0, pObj);
    }

    return pFileName;
}

void CPanelSpy::SpySetEmbed(int nMode)
{
    IObjEdit* pd = GetProjMgr()->ItemGetSelectedData(true);
    if (!pd || (pd->GetType() != e_otWgt && pd->GetType() != e_otWnd))
    {
        ::MessageBox(0, L"please select a window in object list", L"error", MB_OK);
        return;
    }
    CObjData* p = static_cast<CObjData*>(pd);
    HTUI hWgt = static_cast<HTUI>(p->GetProp());
    DbgAssert(hWgt);
    if (nMode == 0)
        CProcessEmbed::Embed1(hWgt->GetHWND());
    else
        CProcessEmbed::Embed2(hWgt->GetHWND());
}

// CTuiMsgFilter
BOOL CPanelSpy::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
{
    switch (pMsg->message)
    {
    case WM_NCDESTROY:
        {
            CObjData* p = pMsg->hWnd->m_pEditData;
            if (p)
            {
                DbgAssert(p->m_hWnd == pMsg->hWnd);
                GetProjMgr()->ItemRemove(p);
            }
        }
        break;
    }
    return false;
}



IProj* CreateSpyProj(PANELINFO& info)
{
    CPanelApp* view = new CPanelSpy(info);

//     _Module.m_pIMainWnd = pMainWnd;
//     view->m_hRealWnd = info.hWnd;
//     view->m_pEditor = info.pMainWnd;

    return view;
}