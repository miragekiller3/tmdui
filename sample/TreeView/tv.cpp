//-----------------------------------------------------------
//  tv.cpp
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

#include "StdAfx.h"
#include "tv.h"
#include "tmdrag.h"
#include "tmgdix.h"


using namespace TM;


#define INDENT 14
#define DRAG_TV_ITEM        _T("drag tv item")





TMResult CTVCtrl::OnHitTest (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DefWindowProc (uMsg, wParam, lParam);
    
    TM::TUIHITINFO* phi = (TM::TUIHITINFO*)lParam;
    if (phi->bNC)
        return 0;
    
    CTVItem ti ((TM::HTVITEM)phi->pTaget);
    if (ti)
    {
        int nLevel = ti.GetLevel();
        TM::CTuiRect rc;
        GetClientRect(&rc);
        int x = phi->pt.x - rc.left;
        if (x < nLevel * INDENT)
        {
            phi->dwCode = TVHT_ONITEMINDENT;
        }
        else if (x < (nLevel + 1) * INDENT)
        {
            phi->dwCode = TVHT_ONITEMBUTTON;
        }
        else
        {
            phi->dwCode = TVHT_ONITEMLABEL;
        }
    }
    
    return 0;
}

TMResult CTVCtrl::OnDrawItem (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TM::TUIDRAWITEM* pdi = (TM::TUIDRAWITEM*)lParam;
    
    if (pdi->dwDrawStage == CDDS_ITEMPREPAINT)
    {
        pdi->gxApp->SetTextColor(TMRGB(0, 0, 0));
        CTVItem ti ((TM::HTVITEM)pdi->hItem);
        int nLevel = ti.GetLevel();
        TM::CText text;
        if (ti.GetChild())
            text.Create(ti.IsExpand() ? _T("-") : _T("+"));
        else
            text.Create(_T("."));
        TUIRect rc = pdi->rcItem;
        rc.left += nLevel * INDENT;
        text->SetFormat(DT_SINGLELINE | DT_VCENTER);
        text->Paint(rc);
        //DrawText (pdi->hDC, &c, 1, &rc, DT_SINGLELINE | DT_VCENTER);
        rc.left += INDENT;
        CString strText;
        ti.GetText(strText);
        text.ReCreate(strText, false);
        text->Paint(rc);
        //DrawText (pdi->hDC, strText, -1, &rc, DT_SINGLELINE | DT_VCENTER);
        if (ti.IsSelected())
        {
//            DrawText (pdi->hDC, ti.GetText(), -1, &rc, DT_SINGLELINE | DT_VCENTER | DT_CALCRECT);
            rc = pdi->rcItem;
            pdi->gxApp->InvertRect(rc);
        }
    }
    else
    {
        if (pdi->dwDrawStage == CDDS_PREPAINT)
        {
            //SetBkMode (pdi->hDC, TRANSPARENT);
        }
        
        bHandled = false;
    }
    
    return 0;
}

// start to drag-and-drop 
TMResult CTVCtrl::OnMouseMove (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TM::TUIHITINFO hi;
    if (GetMouseDownHitInfo(hi))
    {
        if (hi.pTaget)
        {
            TM::HTVITEM hItem = ((TM::HTVITEM)hi.pTaget);
            if (hItem)
            {
                TM::CDragData dg;
                dg.AddMem (0, 0, DRAG_TV_ITEM);
                DoDrag (dg, eDragEffMove);
            }
        }
    }
    return 0;
}

TMResult CTVCtrl::OnDragEntry (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TM::DROPEVENT* pEvent = (TM::DROPEVENT*)lParam;
    if (pEvent->pData->Query(DRAG_TV_ITEM))
    {
        return eDragEffMove;
    }
    else if (pEvent->pData->QueryAUString() || pEvent->pData->Query(TM::CDropData::e_cfHDROP))
    {
        return eDragEffCopy;
    }

    return 0;
}

TMResult CTVCtrl::OnDragOver (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TM::DROPEVENT* pEvent = (TM::DROPEVENT*)lParam;
    if (pEvent->pData->Query(DRAG_TV_ITEM))
    {
        TM::TUIHITINFO hi = {0};
        hi.hWnd = m_hWnd;
        hi.pt = pEvent->ptWindow;
        SendMessage (CM_HITTEST, 0, (TMLParam)&hi);
        CTVItem itm (((TM::HTVITEM)hi.pTaget));
        CTVItem itmSel = GetSelectedItem ();
        
        if (itmSel && itm && itm != itmSel && !itmSel.IsChild(itm))
        {
            return eDragEffMove;
        }

        return 0;
    }

    return eDragEffCopy;

}

TMResult CTVCtrl::OnDragDrop (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TM::DROPEVENT* pEvent = (TM::DROPEVENT*)lParam;

    TM::TUIHITINFO hi = {0};
    hi.hWnd = m_hWnd;
    hi.pt = pEvent->ptWindow;
    SendMessage (CM_HITTEST, 0, (TMLParam)&hi);
    CTVItem itm (((TM::HTVITEM)hi.pTaget));
    if (!itm)
    {
        return 0;
    }

    if (pEvent->pData->Query(DRAG_TV_ITEM))
    {
        CTVItem itmSel = GetSelectedItem ();

        if (itmSel && itm != itmSel && !itmSel.IsChild(itm))
        {
            itmSel.MoveDown (itm.GetChild());//, itm);
            return eDragEffMove;
        }
    }
    else if (pEvent->pData->QueryAUString())
    {
        CString str;
        if (pEvent->pData->GetAUString(str) && !str.IsEmpty())
        {
            itm.InsertChild (str);
        }
        return eDragEffCopy;
    }
    else if (pEvent->pData->Query(TM::CDropData::e_cfHDROP))
    {
        CDrop drop;
        pEvent->pData->GetDrop (drop);
        if (drop)
        {
            for (int i = 0; ; ++i)
            {
                TMCharT c[1024];
                if (drop.DragQueryFile(i, c, 1023) <= 0)
                {
                    break;
                }
                itm.InsertChild (c);
            }
            return eDragEffCopy;
        }
    }

    return 0;
}


TMResult CTVCtrl::OnMouseLeave (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    TM::TUIHITINFO* phiOld = (TM::TUIHITINFO*)wParam;
    TM::TUIHITINFO* phiNew = (TM::TUIHITINFO*)lParam;
    if (!phiNew->bNC && (phiOld->pTaget != phiNew->pTaget) && (phiNew->hWnd == m_hWnd) && phiNew->pTaget)
    {
        CTVItem itm ((HTVITEM)phiNew->pTaget);
        CString str;
        
        int n = 0;
        for (CTVItem itc(itm.GetChild()); itc; itc = itc.GetNext(), ++n)
        {
        }

//         str.Format (_T("level:%d; childs:%d; %s"), itm.GetLevel(), n, itm.GetText());
//         SetHint (str);
    }

    return 0;
}