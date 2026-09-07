/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.02.18
 *
 *  Copyright (C) 2009 miragekiller
 */

#include "drag.h"
#include "../tmcom.h"
#include "../tmdll.h"
#include "../tmdrag.h"
#include "uiwindow.h"
#include "uiwindow_win.h"

#if defined(TM_WIN_DESKTOP) && !defined(TM_QT)

TM_BGN_NAMESPACE


CDropMgr::CDropMgr()
    : m_bEntry(false)
{
    MemZeroT(m_hitOld);
}

void CDropMgr::DoDragLeave(CTuiWnd* p)
{
    if (m_hitOld.hWnd && m_bEntry)
        m_hitOld.hWnd->SendMessage(CM_DRAGLEAVE, 0, 0);

    m_hitOld.hWnd = 0;
    m_bEntry = false;
}

TMResult CDropMgr::DoEvent(CTuiWnd* p, TMUInt uMsg, TMWParam wParam, TMLParam lParam)
{
    DbgAssert(p);
    DbgAssert(uMsg != CM_DRAGENTRY);
    DbgAssert(uMsg != CM_DRAGLEAVE);

    // init
    DROPEVENT& de = *(DROPEVENT*)lParam;
    p->ScreenToClient(&de.ptScreen, 1, &de.ptWindow);
//     de.ptWindow = de.ptScreen;
//     ::ScreenToClient(p->GetHWND(), &de.ptWindow);
    
    // get mouse target
    HTUI pc;
    DISABLE_SPY();
    if (Edit_IsObjectInDocMode(p))
        pc = CTuiWidget(p).ChildWindowFromPoint(de.ptWindow, CWP_ALL);
    else
        pc = CTuiWidget(p).ChildWindowFromPoint(de.ptWindow, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT);
    if (!pc)
        return 0;

    TUIHITINFO ht = {0};
    ht.pt = de.ptWindow;
    ht.hWnd = pc;
    DISABLE_SPY();
    CTuiWidget(pc).HitTest(ht, uMsg);
    pc = ht.hWnd;

    // check mouse entry and leave
    TMBool bSame = (pc == m_hitOld.hWnd && m_hitOld.bNC == ht.bNC && m_hitOld.dwCode == ht.dwCode && m_hitOld.pTaget == ht.pTaget);
    if (!bSame)
    {
        if (m_hitOld.hWnd && m_bEntry)
            m_hitOld.hWnd->SendMessage(CM_DRAGLEAVE, (TMWParam)&ht, 0);

        m_hitOld = ht;
        if (pc)
            m_bEntry = pc->SendMessage(CM_DRAGENTRY, (TMWParam)&ht, (TMLParam)&de);
    }

    TMResult lr = 0;
    if (pc && m_bEntry)// && (CM_DRAGENTRY != uMsg))
        lr = pc->SendMessage(uMsg, (TMWParam)&ht, (TMLParam)&de);

    if (CM_DRAGDROP == uMsg)
    {
        m_hitOld.hWnd = 0;
        m_bEntry = false;
    }

    return lr;
}

TMResult CDropMgr::DoDragEntry()
{
    m_bEntry = false;
    m_hitOld.hWnd = 0;
    return S_OK;
}


TMResult CDropMgr::DoDragEvent(IDataObject* pDataObj, CTuiWnd* p, TMUInt uMsg, TMULong grfKeyState,
                                   POINTL ptx, TMULong* dwEffect)
{
    DbgAssert(p);

    CDropData dd(pDataObj);
    DROPEVENT de = { &dd, *dwEffect, grfKeyState };
    PointSetX(de.ptScreen, ptx.x);
    PointSetY(de.ptScreen, ptx.y);

    *dwEffect &= DoEvent(p, uMsg, 0, (TMLParam)&de);
    return *dwEffect ? S_OK : E_FAIL;
}


///////////////////////////////////////////////////////////////////////////////
// CDropTargetMgr
STDMETHODIMP CDropTargetMgr::DragEnter(IDataObject* pDataObj, TMULong grfKeyState, POINTL pt, TMULong* pdwEffect)
{
    m_pDataObj = pDataObj;
    return TM_OTHIS(CTuiWnd_Platform, m_drop)->GetView()->GetDragMgr().DoDragEntry();
}

STDMETHODIMP CDropTargetMgr::DragOver(TMULong grfKeyState, POINTL pt, TMULong* pdwEffect)
{
    CPoint ptx(pt.x, pt.y);
    return TM_OTHIS(CTuiWnd_Platform, m_drop)->GetView()->GetDragMgr().DoDragEvent(m_pDataObj, TM_OTHIS(CTuiWnd_Platform, m_drop), CM_DRAGOVER, grfKeyState, pt, pdwEffect);
}

STDMETHODIMP CDropTargetMgr::DragLeave()
{
    m_pDataObj = 0;
    TM_OTHIS(CTuiWnd_Platform, m_drop)->GetView()->GetDragMgr().DoDragLeave(TM_OTHIS(CTuiWnd_Platform, m_drop));
    return S_OK;
}

STDMETHODIMP CDropTargetMgr::Drop(IDataObject* pDataObj, TMULong grfKeyState, POINTL pt, TMULong* pdwEffect)
{
    return TM_OTHIS(CTuiWnd_Platform, m_drop)->GetView()->GetDragMgr().DoDragEvent(m_pDataObj, TM_OTHIS(CTuiWnd_Platform, m_drop), CM_DRAGDROP, grfKeyState, pt, pdwEffect);
}

TM_END_NAMESPACE

#endif // #if defined(TM_WIN_DESKTOP) && !defined(TM_QT)
