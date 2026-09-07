/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.11.10
 *
 *  Copyright (C) 2006 miragekiller
 */

#include "frame.h"
#include "uiwidget.h"
#include "view.h"
#include "tmmouse.h"


TM_BGN_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
// class FRAMEINFO
void FRAMEINFO::ResetScrollFlag(HTUI hWnd)
{
    if (hWnd->IsAutoVScroll())
    {
//         DbgAssert(!hWnd->IsAutoHeight());

        if (hWnd->IsVScrollable())
        {
            hWnd->SetVScroll();
            AdjustVScrollBar(hWnd->m_dwLayout, hWnd->m_rcClient);
        }
        else
            hWnd->RemoveVScroll();
    }
    else
        AdjustVScrollBar(hWnd->m_dwLayout, hWnd->m_rcClient);

    if (hWnd->IsAutoHScroll())
    {
//         DbgAssert(!hWnd->IsAutoWidth());
        if (hWnd->IsHScrollable())
        {
            hWnd->SetHScroll();
            AdjustHScrollBar(hWnd->m_dwLayout, hWnd->m_rcClient);
            if (hWnd->IsAutoVScroll() && !hWnd->HasVScroll() && hWnd->IsVScrollable())
            {
                hWnd->SetVScroll();
                AdjustVScrollBar(hWnd->m_dwLayout, hWnd->m_rcClient);
            }
        }
        else
            hWnd->RemoveHScroll();
    }
    else
        AdjustHScrollBar(hWnd->m_dwLayout, hWnd->m_rcClient);
}

void FRAMEINFO::InvalidateHotScrollbar(HTUI hOld, HTUI hNew)
{
    CSet<HTUI> aAdd;
    CSet<HTUI> aDel;

    for (; hNew; hNew = hNew->GetParent())
    {
        if (hNew->IsLayoutHotScroll() && (hNew->HasVScroll() || hNew->HasHScroll()))
            aAdd.insert(hNew), hNew->Invalidate();
    }
    for (; hOld; hOld = hOld->GetParent())
    {
        if (hOld->IsLayoutHotScroll() && (hOld->HasVScroll() || hOld->HasHScroll()))
        {
            if (aAdd.erase(hOld) == 0)
                aDel.insert(hOld), hOld->Invalidate();
        }
    }

    // ani add
    tm_for (CSet<HTUI>::recorder rec(aAdd); rec; ++rec)
    {
        HTUI hWgt = rec.get_data();
        if (hWgt->IsLayoutHotScroll())
        {
            if (hWgt->HasVScroll())
                hWgt->m_aniList.AddScrollShowHideAni(e_objVScroll, hWgt, true);
            if (hWgt->HasHScroll())
                hWgt->m_aniList.AddScrollShowHideAni(e_objHScroll, hWgt, true);
        }
    }
    // ani remove
    tm_for (CSet<HTUI>::recorder rec(aDel); rec; ++rec)
    {
        HTUI hWgt = rec.get_data();
        if (hWgt->IsLayoutHotScroll())
        {
            if (hWgt->HasVScroll())
                hWgt->m_aniList.AddScrollShowHideAni(e_objVScroll, hWgt, false);
            if (hWgt->HasHScroll())
                hWgt->m_aniList.AddScrollShowHideAni(e_objHScroll, hWgt, false);
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
// IFrameSource
void IFrameSource::Update()
{
    HTUI hWnd = FSGetWin();
    hWnd->Invalidate();
    hWnd->UpdateWindow();
}



///////////////////////////////////////////////////////////////////////////
// used to track move window
class _MoveSizeFunction
    : public CTuiMsgFilter //CMessageFilter
{
protected:
    TMWParam m_nHid;
    CPointI m_ptDown;
    CTuiRect m_rcOld; // HWND: device rect; HTUI: logic rect;
    CTuiRect m_rcOldDummy;
    HTUI m_hWidget;
//    TMBool m_bRTL;

    CRect OnMouseMove()
    {
        CPointI ptNew = CMouse::GetCursorPos();
        int dx = ptNew.X() - m_ptDown.X();
        int dy = ptNew.Y() - m_ptDown.Y();
        TMBool bRTL = m_hWidget->GetModule()->GetLocal().IsRTL();
        if (bRTL)
            dx = -dx;
        
        CRect rc(0,0,0,0);
        if ((m_nHid & 0xfff0) == SC_MOVE) // caption
            rc.OffsetRect(dx, dy);
        else
        {
            int nHid = (m_nHid & 0x000f);
            if (nHid == WMSZ_LEFT || nHid == WMSZ_TOPLEFT || nHid == WMSZ_BOTTOMLEFT)
                rc.OffsetLeft(dx);
            else if (nHid == WMSZ_RIGHT || nHid == WMSZ_TOPRIGHT || nHid == WMSZ_BOTTOMRIGHT)
                rc.OffsetRight(dx);
            
            if (nHid == WMSZ_TOP || nHid == WMSZ_TOPLEFT || nHid == WMSZ_TOPRIGHT)
                rc.OffsetTop(dy);
            else if (nHid == WMSZ_BOTTOM || nHid == WMSZ_BOTTOMLEFT || nHid == WMSZ_BOTTOMRIGHT)
                rc.OffsetBottom(dy);
        }
        return rc;
    }

    void GetWindowRect(CTuiRect& rc)
    {
        if (m_hWidget->Is_WS_PRI_IS_WINDOW())
        {
            CWindowAT wnd(m_hWidget->GetHWND());
            CRect rcw;
            wnd.GetWindowRect(&rcw);
            rcw.CopyTo(rc);
        }
        else
            m_hWidget->GetWindowRect(&rc);
    }
    
    void MoveWindowD(const CRect* drc) // drc == 0: restore to down rect
    {
        if (m_hWidget->Is_WS_PRI_IS_WINDOW())
        {
            CWindowAT wnd(m_hWidget->GetHWND());
            if (drc)
            {
                CRect rcw(m_rcOldDummy);
                rcw.OffsetRect(*drc);
                wnd.MoveWindow(&rcw);
                CRect rcNew;
                wnd.GetWindowRect(&rcNew);
                if (rcNew != rcw) // for drag cross screen;
                {
                    rcw.DistanceRect(rcNew);
                    m_rcOldDummy.DistanceRect(CTuiRect(rcw));
                }
            }
            else
            {
                CRect rcw(m_rcOld);
                wnd.MoveWindow(&rcw);
                wnd.MoveWindow(&rcw); // for windows cross move window bug
            }
        }
        else
        {
            if (drc)
            {
                CTuiRect rc;
                m_hWidget->GetDPI().DPtoLP(*drc, rc);
                rc.OffsetRect(m_rcOld);
                m_hWidget->MoveWindow(rc);
            }
            else
                m_hWidget->MoveWindow(m_rcOld);
        }
    }

    void Free()
    {
        m_hWidget->GetView()->RemoveMessageFilter(this);
        CAppAT::ReleaseCapture();
    }

    void DoMouse()
    {
        CRect rc(OnMouseMove());
        MoveWindowD(&rc);
    }

    virtual TMBool PreTranslateMessage(TUIMSG* pMsg, TMResult& lResult)
    {
        if (pMsg->message == CM_APPMESSAGE)
        {
            TUIMSG* pAppMsg = (TUIMSG*)pMsg->lParam;
    //         DbgTrace (L"_MoveSizeFunction::PreTranslateMessage(%s)", DbgFormatMessage(pAppMsg->message));
            switch (pAppMsg->message)
            {
            case WM_MOUSEMOVE:
                DoMouse();
                return true;

            case WM_NCMOUSEMOVE:
                DbgAssert(0);
                break;

            case WM_LBUTTONUP:
            case WM_LBUTTONDOWN:
            case WM_NCLBUTTONUP:
            case WM_NCLBUTTONDOWN:
                Free();
                return true;

            case WM_ACTIVATE:
            case WM_ACTIVATEAPP:
            case WM_CAPTURECHANGED:
                Free();
                return false;

            case WM_KEYDOWN:
                if (pAppMsg->wParam == VK_ESCAPE)
                {
                    Free();
                    MoveWindowD(0);
                }
                else if (pAppMsg->wParam == VK_RETURN)
                    Free();
                return true;
            }
        }
        else if (pMsg->hWnd == m_hWidget && WM_DESTROY == pMsg->message)
        {
            Free();
        }
        
        return false;
    }

public:
    TMBool BeginDrag(HTUI hWnd, TMWParam wParam)
    {
        m_hWidget = hWnd;
        m_nHid = wParam;
        GetWindowRect(m_rcOld);
        m_rcOldDummy = m_rcOld;
        m_ptDown = CMouse::GetCursorPos();
        m_hWidget->GetView()->AddMessageFilter(this);
        m_hWidget->GetView()->InternalSetCapture(hWnd);
        return true;
    }
};

TMBool CFrame::DragMoveSize(HTUI hWgt, TMWParam wParam)
{
    static _MoveSizeFunction s_funcMoveSize;
    return s_funcMoveSize.BeginDrag(hWgt, wParam);
}


TM_END_NAMESPACE
