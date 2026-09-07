/*
 *  @file
 *  @brief  layout
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2012.02.29
 *
 *  Copyright (C) 2012 miragekiller                               
 */

#include "layout.h"


TM_BGN_NAMESPACE


const GXDips CLayoutHelper_mx       = FLT_MAX;
const GXDips CLayoutHelper_mn       = FLT_MIN;
const TMUInt32 _LAYOUT_AUTOWIDTH_C   = 0x10000000;
const TMUInt32 _LAYOUT_AUTOHEIGHT_C  = 0x20000000;


struct CLayoutHelper
{
    void SetPos(HTUI hWgt, GXDips l, GXDips t, GXDips w, GXDips h)
    {
        enum { eLay = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOLAYOUTSIBLING }; // | SWP_NOREDRAW
        hWgt->SetWindowPos(0, l, t, w, h, eLay);
    }

    void SetRect(HTUI hWgt, GXDips l, GXDips t, GXDips r, GXDips b)
    {
        SetPos(hWgt, l, t, r - l, b - t);
    }

    static void SetAlignFlagH(HTUI p, TMUInt32& dwScrollFlag, GXDips& nAuto)
    {
#ifndef TUI_TEST_MODE
        DbgAssert(p->IsAlignH());
#endif
        dwScrollFlag |= LAYOUT_AUTOWIDTH;
        if (p->IsAutoHeight())
        {
            tm_setmax(nAuto, p->GetMarginHeight());
            dwScrollFlag |= _LAYOUT_AUTOHEIGHT_C;
        }
    }

    static void SetAlignFlagV(HTUI p, TMUInt32& dwScrollFlag, GXDips& nAuto)
    {
#ifndef TUI_TEST_MODE
        DbgAssert(p->IsAlignV());
#endif
        dwScrollFlag |= LAYOUT_AUTOHEIGHT;
        if (p->IsAutoWidth())
        {
            tm_setmax(nAuto, p->GetMarginWidth());
            dwScrollFlag |= _LAYOUT_AUTOWIDTH_C;
        }
    }

    void LayoutAlign(HTUI p, CTuiRect& rc, TMUInt32& dwScrollFlag, GXDips& wAuto, GXDips& hAuto)
    {
        if (!p)
            return;

        Ref ref(p);
        TMUInt32 dwStyle = p->GetStyle();
        TMULong dwLayout = p->m_dwLayout;
        if ((dwStyle & WS_VISIBLE) && ((dwLayout & LAYOUT_MODE_MASK) == LAYOUT_ALIGN) && !p->Is_WS_PRI_BINDMODE())
        {
            GXDips n;
            switch (dwLayout & ALIGN_MASK)
            {
            case ALIGN_LEFT:
                n = p->GetWidth();
                SetPos(p, rc.Left() + p->m_bdMargin.left,
                    rc.Top() + p->m_bdMargin.top, n,
                    rc.Height() - p->m_bdMargin.top - p->m_bdMargin.bottom);
                rc.SetLeft(p->m_rcWnd.Right() + p->m_bdMargin.right);
                SetAlignFlagH(p, dwScrollFlag, hAuto);
                break;

            case ALIGN_LEFT_RIGHT:
                LayoutAlign(p->GetNext(), rc, dwScrollFlag, wAuto, hAuto);
                n = p->GetWidth();
                SetPos(p, rc.Left() + p->m_bdMargin.left,
                    rc.Top() + p->m_bdMargin.top, n,
                    rc.Height() - p->m_bdMargin.top - p->m_bdMargin.bottom);
                rc.SetLeft(p->m_rcWnd.Right() + p->m_bdMargin.right);
                SetAlignFlagH(p, dwScrollFlag, hAuto);
                return;

            case ALIGN_TOP:
                n = p->GetHeight();
                SetPos(p, rc.Left() + p->m_bdMargin.left,
                    rc.Top() + p->m_bdMargin.top,
                    rc.Width() - p->m_bdMargin.left - p->m_bdMargin.right, n);
                rc.SetTop(p->m_rcWnd.Bottom() + p->m_bdMargin.bottom);
                SetAlignFlagV(p, dwScrollFlag, wAuto);
                break;

            case ALIGN_HEADER:
                n = p->GetHeight();
                {
                    HTUI parent = p->_parent;
                    GXDips sl = -parent->GetScrollLeft();
                    SetPos(p, sl + rc.Left() + p->m_bdMargin.left,
                        rc.Top() + p->m_bdMargin.top,
                        parent->GetSafeScrollWidth() /*- p->m_bdMargin.left - p->m_bdMargin.right*/, n);
                }
                rc.SetTop(p->m_rcWnd.Bottom() + p->m_bdMargin.bottom);
                SetAlignFlagV(p, dwScrollFlag, wAuto);
                break;

            case ALIGN_TOP_BOTTOM:
                LayoutAlign(p->GetNext(), rc, dwScrollFlag, wAuto, hAuto);
                n = p->GetHeight();
                SetPos(p, rc.Left() + p->m_bdMargin.left,
                    rc.Top() + p->m_bdMargin.top,
                    rc.Width() - p->m_bdMargin.left - p->m_bdMargin.right, n);
                rc.SetTop(p->m_rcWnd.Bottom() + p->m_bdMargin.bottom);
                SetAlignFlagV(p, dwScrollFlag, wAuto);
                return;

            case ALIGN_RIGHT:
                LayoutAlign(p->GetNext(), rc, dwScrollFlag, wAuto, hAuto);
                n = p->GetWidth();
                SetPos(
                    p, rc.Right() - n /*- p->m_bdMargin.left*/ - p->m_bdMargin.right,
                    rc.Top() + p->m_bdMargin.top, n,
                    rc.Height() - p->m_bdMargin.top - p->m_bdMargin.bottom);
                rc.SetRight(p->m_rcWnd.Left() - p->m_bdMargin.left);
                SetAlignFlagH(p, dwScrollFlag, hAuto);
                return;

            case ALIGN_RIGHT_RIGHT:
                n = p->GetWidth();
                SetPos(p, rc.Right() - n /*- p->m_bdMargin.left*/ - p->m_bdMargin.right,
                    rc.Top() + p->m_bdMargin.top, n,
                    rc.Height() - p->m_bdMargin.top - p->m_bdMargin.bottom);
                rc.SetRight(p->m_rcWnd.Left() - p->m_bdMargin.left);
                SetAlignFlagH(p, dwScrollFlag, hAuto);
                break;

            case ALIGN_BOTTOM:
                LayoutAlign(p->GetNext(), rc, dwScrollFlag, wAuto, hAuto);
                n = p->GetHeight();
                SetPos(
                    p, rc.Left() + p->m_bdMargin.left,
                    rc.Bottom() - n - p->m_bdMargin.bottom, //- p->m_bdMargin.top, 
                    rc.Width() - p->m_bdMargin.left - p->m_bdMargin.right, n);
                rc.SetBottom(p->m_rcWnd.Top() - p->m_bdMargin.top);
                SetAlignFlagV(p, dwScrollFlag, wAuto);
                return;

            case ALIGN_BOTTOM_BOTTOM:
                n = p->GetHeight();
                SetPos(p, rc.Left() + p->m_bdMargin.left,
                    rc.Bottom() - n - p->m_bdMargin.bottom, //- p->m_bdMargin.top, 
                    rc.Width() - p->m_bdMargin.left - p->m_bdMargin.right, n);
                rc.SetBottom(p->m_rcWnd.Top() - p->m_bdMargin.top);
                SetAlignFlagV(p, dwScrollFlag, wAuto);
                break;
                
            case ALIGN_CLIENT:
                dwScrollFlag |= ALIGN_CLIENT;
                break;
            }
        }

        LayoutAlign(p->GetNext(), rc, dwScrollFlag, wAuto, hAuto);
    }

    void LayoutAlignClient(HTUI p, CTuiRect& rc)
    {
        if (!p)
            return;
        
        Ref ref(p);
        TMUInt32 dwStyle = p->GetStyle();
        TMULong dwLayout = p->m_dwLayout;
        if ((dwStyle & WS_VISIBLE) && ((dwLayout & ALIGN_MASK) == ALIGN_CLIENT) && !p->Is_WS_PRI_BINDMODE())
        {
            SetRect(p,
                rc.Left() + p->m_bdMargin.left,
                rc.Top() + p->m_bdMargin.top,
                rc.Right() - p->m_bdMargin.right,
                rc.Bottom() - p->m_bdMargin.bottom);
        }
        
        LayoutAlignClient(p->GetNext(), rc);
    }

    // left, right: in: child new position; out:child new position
    // dLeft, dRight: parent's new position - parent's old postion
    void LayoutAnchorHV(TMULong dwLayout, GXDips& left, GXDips& right, GXDips newL, GXDips oldL, GXDips newR, GXDips oldR)
    {
#define CALC_Fix(nOld)      nOld
#define CALC_Left(nOld)     nOld - oldL + newL
#define CALC_Right(nOld)    nOld + newR - oldR
#define CALC_Center(nOld)   nOld + (newL + newR) / 2 - (oldL + oldR)/2;
        
        switch (dwLayout & LAYOUT_HMASK)
        {
        case LAYOUT_LEFT:
            left = CALC_Left(left);
            right = CALC_Left(right);
            break;
            
        case LAYOUT_RIGHT:
            left = CALC_Right(left);
            right = CALC_Right(right);
            break;
            
        case LAYOUT_CENTER:
            left = CALC_Center(left);
            right = CALC_Center(right);
            break;
            
        case LAYOUT_FRAME:
            left = CALC_Left(left);
            right = CALC_Right(right);
            break;
            
        case LAYOUT_LCENTER:
            left = CALC_Left(left);
            right = CALC_Center(right);
            break;
            
        case LAYOUT_RCENTER:
            left = CALC_Center(left);
            right = CALC_Right(right);
            break;
            
        case LAYOUT_NONE:
        default:
            left = CALC_Fix(left);
            right = CALC_Fix(right);
            break;
        }
#undef CALC_Fix
#undef CALC_Left
#undef CALC_Right
#undef CALC_Center
    }

    void LayoutAnchor(HTUI p, const CTuiRect& rcOldP, const CTuiRect& rcNewP)
    {
        if (!p)
            return;

        Ref ref(p);
        TMUInt32 dwLayout = p->m_dwLayout;
        if (((dwLayout & LAYOUT_MODE_MASK) == LAYOUT_ANCHOR) && !p->Is_WS_PRI_BINDMODE())
        {
            GXDips l = p->GetLeft();
            GXDips r = p->GetRight();
            LayoutAnchorHV(dwLayout, l, r, rcNewP.Left(), rcOldP.Left(), rcNewP.Right(), rcOldP.Right());
            GXDips t = p->GetTop();
            GXDips b = p->GetBottom();
            LayoutAnchorHV(dwLayout >> 4, t, b, rcNewP.Top(), rcOldP.Top(), rcNewP.Bottom(), rcOldP.Bottom());
            SetRect(p, l, t, r, b);
        }
        LayoutAnchor(p->GetNext(), rcOldP, rcNewP);
    }

    HTUI LayOutFlowLine(HTUI p, CTuiRect& rc, CTuiRect::value_type l, CTuiRect::value_type& h, CTuiRect::value_type& b)
    {
        if (!p)
            return 0;

        TMULong dwLayout = p->m_dwLayout;
        if ((dwLayout & LAYOUT_MODE_MASK) != LAYOUT_FLOW)
            return p;

        if (!p->Is_WS_VISIBLE() || p->Is_WS_PRI_BINDMODE())
            return LayOutFlowLine(p->GetChild(), rc, l, h, b);

        GXDips w = p->GetWidth();
        GXDips ll = l + w + p->m_bdMargin.left + p->m_bdMargin.right;
        if (ll > rc.Right() && l > rc.Left())
        {
            rc.SetTop(rc.Top() + h);
            CTuiRect::value_type hh = 0;
            return LayOutFlowLine (p, rc, rc.Left(), hh, b);
        }
        
        GXDips hh = p->GetHeight() + p->m_bdMargin.top + p->m_bdMargin.bottom;
        h = tm_max(h, hh);
        GXDips nTop = rc.Top();

        HTUI h1 = LayOutFlowLine(p->GetNext(), rc, ll, h, b); 
        
        switch ((dwLayout & FLOW_MASK) & ~LAYOUT_SCROLL)
        {
        case FLOW_TOP: nTop += p->m_bdMargin.top; break;
        case FLOW_MIDDLE: nTop += (h - p->GetHeight()) / (GXDips)2; break;
        case FLOW_BOTTOM: nTop += (h - hh - p->m_bdMargin.bottom); break;
        }
        SetPos(p, l + p->m_bdMargin.left, nTop, w, p->GetHeight());
        tm_setmax(b, p->GetBottom() + p->m_bdMargin.bottom);
        return h1;
    }

//     static HTUI LayOutFlowRow(HTUI p, const CTuiRect& rcBound, const CTuiRect& rcOldP, const CTuiRect& rcNewP, CTuiRect& rcClient)
//     {
//         if (!p)
//             return 0;
// 
//         TMULong dwLayout = p->m_dwLayout;
//         HTUI hWgt = p;
//         if ((dwLayout & LAYOUT_MODE_MASK) == LAYOUT_ROW)
//         {
//             CTuiRect rc;
//             LayoutAnchorV(p, rc, rcOldP, rcNewP);
//         
//             TMULong dwFlag = dwLayout & LAYOUT_HMASK;
//             if (LAYOUT_LEFT == dwFlag)
//             {
//                 rc.SetLeft(rcClient.Left() + p->m_bdMargin.left);
//                 rc.SetWidth(p->m_rcWnd.Width());
//                 if (p->IsWindowVisible())
//                     rcClient.SetLeft (rc.Right() + p->m_bdMargin.right);
//             }
//             else if (LAYOUT_FRAME == dwFlag)
//                 rc.SetLeft(rcClient.Left() + p->m_bdMargin.left);
// 
//             hWgt = LayOutFlowRow (p->m_pNext, rcBound, rcOldP, rcNewP, rcClient);
//         
//             if (LAYOUT_RIGHT == dwFlag)
//                 rc.SetRightWidth(rcClient.Right() - p->m_bdMargin.right, p->m_rcWnd.Width());
//             else if (LAYOUT_FRAME == dwFlag)
//                 rc.SetRight(rcClient.Right() - p->m_bdMargin.right);
// 
//             p->SetWindowPos(0, &rc, eLay);
//         }
//         if (p->IsWindowVisible())
//             rcClient.SetRight(p->m_rcWnd.Left() - p->m_bdMargin.left);
//         return hWgt;
//     }
// 
//     static HTUI LayOutFlowColumn(HTUI p, const CTuiRect& rcBound, const CTuiRect& rcOldP, const CTuiRect& rcNewP, CTuiRect& rcClient)
//     {
//         if (!p)
//             return 0;
//         
//         TMULong dwLayout = p->m_dwLayout;
//         HTUI hWgt = p;
//         if ((dwLayout & LAYOUT_MODE_MASK) == LAYOUT_COLUMN)
//         {
//             CTuiRect rc;
//             LayoutAnchorH(p, rc, rcOldP, rcNewP);
//             
//             TMULong dwFlag = dwLayout & LAYOUT_VMASK;
//             if (LAYOUT_TOP == dwFlag)
//             {
//                 rc.SetTop(rcClient.Top() + p->m_bdMargin.top);
//                 rc.SetHeight(p->m_rcWnd.Height());
//                 if (p->IsWindowVisible())
//                     rcClient.SetTop (rc.Bottom() + p->m_bdMargin.bottom);
//             }
//             else if (LAYOUT_VFRAME == dwFlag)
//                 rc.SetTop(rcClient.Top() + p->m_bdMargin.top);
//             
//             hWgt = LayOutFlowColumn (p->m_pNext, rcBound, rcOldP, rcNewP, rcClient);
//             
//             if (LAYOUT_BOTTOM == dwFlag)
//                 rc.SetBottomHeight(rcClient.Bottom() - p->m_bdMargin.bottom, p->m_rcWnd.Height());
//             else if (LAYOUT_VFRAME == dwFlag)
//                 rc.SetBottom(rcClient.Bottom() - p->m_bdMargin.bottom);
//             
//             p->SetWindowPos(0, &rc, eLay);
//         }
//         if (p->IsWindowVisible())
//             rcClient.SetBottom(p->m_rcWnd.Top() - p->m_bdMargin.top);
//         return hWgt;
//     }


    void LayOutFlow(HTUI hWgt, const CTuiRect& rcBound, CTuiRect::value_type& b)
    {
        for (HTUI hOld = 0; hWgt;)
        {
            if (!hWgt->Is_WS_VISIBLE())
            {
                hWgt = hWgt->GetNext();
                continue;
            }

            if ((hWgt->m_dwLayout & LAYOUT_MODE_MASK) == LAYOUT_FLOW)
            {
                CTuiRect::value_type l, h;
                CTuiRect rcFlow(rcBound);
                if (hOld)
                {
                    rcFlow.SetTop(hOld->m_rcWnd.Top() - hOld->m_bdMargin.top);
                    l = hOld->m_rcWnd.Right() + hOld->m_bdMargin.right;
                    h = hOld->GetHeight() + hOld->m_bdMargin.top + hOld->m_bdMargin.bottom;
                }
                else
                {
                    l = rcBound.Left();
                    h = 0;
                }
                hOld = hWgt;
                hWgt = LayOutFlowLine (hWgt, rcFlow, l, h, b);
            }
            else
            {
                hOld = hWgt;
                hWgt = hWgt->GetNext();
            }

//             switch (hWgt->m_dwLayout & LAYOUT_MODE_MASK)
//             {
//             case LAYOUT_FLOW:
// 
//                 break;
// 
// //             case LAYOUT_ROW:
// //                 {
// //                     CTuiRect rc = rcBound;
// //                     if (hOld)
// //                         rc.SetLeft(hOld->m_rcWnd.Right() + hOld->m_bdMargin.right);
// //                     hOld = hWgt;
// //                     hWgt = LayOutFlowRow(hWgt, rcBound, rcDx, rc);
// //                 }
// //                 break;
// // 
// //             case LAYOUT_COLUMN:
// //                 {
// //                     CTuiRect rc = rcBound;
// //                     if (hOld)
// //                         rc.SetTop(hOld->m_rcWnd.Bottom() + hOld->m_bdMargin.bottom);
// //                     hOld = hWgt;
// //                     hWgt = LayOutFlowColumn(hWgt, rcBound, rcDx, rc);
// //                 }
// //                 break;
// 
//             default:
//                 hOld = hWgt;
//                 hWgt = hWgt->m_pNext;
//                 break;
//             }
        }
    }

//     void LayOutScrollTable(HTUI pParent)
//     {
//         DbgAssert(pParent);
//         
//         TM::CTuiRect rc;
//         pParent->GetProjectRect(rc);
//         GXDips ww = rc.Width();
//         GXDips tt = rc.Top();
//         for (HTUI p = pParent->GetChild(); p; p = p->GetNext())
//         {
//             if (!p->Is_WS_VISIBLE())
//                 continue;
//             if ((p->m_dwLayout & TABLE_MASK) == TABLE_ROW)
//             {
//                 GXDips hh = p->GetHeight() + p->m_bdMargin.top + p->m_bdMargin.bottom;
//                 SetPos(p, rc.Left() + p->m_bdMargin.left, tt + p->m_bdMargin.top,
//                     ww - p->m_bdMargin.left - p->m_bdMargin.right, p->GetHeight());
//                 tt += hh;
//             }
//         }
//     }
// 
//     TMBool LayOutTable(HTUI pParent)
//     {
//         DbgAssert(pParent);
// 
//         GXDips wScroll = 0, wWnd = 0; GXDips h = 0;
//         for (HTUI p = pParent->GetChild(); p; p = p->GetNext())
//         {
//             if (!p->Is_WS_VISIBLE())
//                 continue;
//             if ((p->m_dwLayout & TABLE_MASK) == TABLE_ROW)
//             {
//                 GXDips hh = p->GetHeight() + p->m_bdMargin.top + p->m_bdMargin.bottom;
//                 h += hh;
// 
//                 if (p->GetScrollWidth())
//                 {
//                     GXDips ww = p->ScrollWidthToWndWidth() + p->m_bdMargin.left + p->m_bdMargin.right;
//                     wScroll = tm_max(wScroll, ww);
//                 }
//                 else
//                 {
//                     GXDips ww = p->m_rcWnd.Width() + p->m_bdMargin.left + p->m_bdMargin.right;
//                     wWnd = tm_max(wWnd, ww);
//                 }
//             }
//         }
//         GXDips w = wScroll ? wScroll : wWnd;
//         if (!w && !h)
//             return false;
// 
// //         if (CTuiWidget(pParent).SetScrollRange(w, h))
// //             return true;
// 
//         LayOutScrollTable(pParent);
// 
//         return true;
//     }

}; // CLayoutHelper


void LayoutChilds(HTUI hParent, const CTuiRect& rcOldP, const CTuiRect& rcNewP)
{
    // anchor
    CLayoutHelper helper;
    helper.LayoutAnchor(hParent->GetChild(), rcOldP, rcNewP);

    // optimize
    if (!hParent->Is_WS_VISIBLE())
        return;

    // align mode
    TMUInt32 maskAlign = 0;
    CTuiRect rcAlign(rcNewP);
    GXDips wAuto = 0, hAuto = 0;
    helper.LayoutAlign(hParent->GetChild(), rcAlign, maskAlign, wAuto, hAuto);
    if ((maskAlign & ALIGN_CLIENT) == ALIGN_CLIENT)
        helper.LayoutAlignClient(hParent->GetChild(), rcAlign);
    
    // flow mode
    GXDips nFlowBottom = -GXDIPS_MAX;
    helper.LayOutFlow(hParent->GetChild(), rcNewP, nFlowBottom);

    // table mode
//     helper.LayOutTable(hParent);

    // auto size
    TMUInt32 dwScrollMask = hParent->m_dwLayout;
    if (hParent->Is_WS_PRI_ALIGNAUTOSIZE())
    {
        GXDips* pw = 0, *ph = 0;
        if (maskAlign & LAYOUT_AUTOWIDTH)
            wAuto = 0;
        if (maskAlign & LAYOUT_AUTOHEIGHT)
            hAuto = 0;
        if ((dwScrollMask & LAYOUT_AUTOWIDTH) && (maskAlign & (LAYOUT_AUTOWIDTH | _LAYOUT_AUTOWIDTH_C)))
        {
            GXDips left = rcAlign.Left() - rcNewP.Left();
            GXDips right = rcNewP.Right() - rcAlign.Right();
            tm_setmax(left, right);
            left += wAuto;
            pw = &left;
        }
        if ((dwScrollMask & LAYOUT_AUTOHEIGHT) && ((maskAlign & (LAYOUT_AUTOHEIGHT | _LAYOUT_AUTOHEIGHT_C)) || nFlowBottom != -GXDIPS_MAX))
        {
            if (nFlowBottom != -GXDIPS_MAX && rcAlign.Top() < nFlowBottom)
                rcAlign.KeepBSetT(nFlowBottom);
            GXDips top = rcAlign.Top() - rcNewP.Top();
            GXDips bottom = rcNewP.Bottom() - rcAlign.Bottom();
            tm_setmax(top, bottom);
            top += hAuto;
            ph = &top;
        }
        
        hParent->SendScrollRangeMessage(pw, ph);
    }
}

bool HaveChildAlignScroll(HTUI hParent)
{
    for (HTUI hc = hParent->GetChild(); hc; hc = hc->GetNext())
    {
        if ((hc->m_dwLayout & (ALIGN_SCROLL | LAYOUT_MODE_MASK)) == (ALIGN_SCROLL | LAYOUT_ALIGN))
            return true;
    }
    return false;
}


// static void _DeltaSizeAnchor(TMULong dwLayOut, CTuiRect& rc, GXDips dx, GXDips dy)
// {
//     GXDips n;
//     switch (dwLayOut & LAYOUT_HMASK)
//     {
//     case LAYOUT_RCENTER:
//     case LAYOUT_RIGHT:
//         rc.MoveLeftTo(rc.Left() - dx); break;
//         
//     case LAYOUT_CENTER:
//     case LAYOUT_FRAME:
//         n = dx / (GXDips)2;
//         rc.SetLeftRight(rc.Left() - n, rc.Right() + dx - n);
//         break;
//         
//     default:
//         rc.OffsetRight(dx);
//         break;
//     }
//     
//     switch (dwLayOut & LAYOUT_VMASK)
//     {
//     case LAYOUT_BCENTER:
//     case LAYOUT_BOTTOM:
//         rc.MoveTopTo(rc.Top() - dy); break;
//         
//     case LAYOUT_VCENTER:
//     case LAYOUT_VFRAME:
//         n = dy / (GXDips)2;
//         rc.SetTopBottom(rc.Top() - n, rc.Bottom() + dy - n);
//         break;
//         
//     default:
//         rc.OffsetBottom(dy);
//         break;
//     }
// }
// 
// void LayoutDeltaSize(TMULong dwLayOut, CTuiRect& rc, GXDips dx, GXDips dy)
// {
//     switch (dwLayOut & LAYOUT_MODE_MASK)
//     {
//     case LAYOUT_ANCHOR:
//         //     case LAYOUT_ROW:
//         //     case LAYOUT_COLUMN:
//         _DeltaSizeAnchor(dwLayOut, rc, dx, dy);
//         break;
//         
//     case LAYOUT_ALIGN:
//     case LAYOUT_TABLE:
//         switch (dwLayOut & ALIGN_TYPE_MASK)
//         {
//         case ALIGN_LEFT & ALIGN_TYPE_MASK: rc.OffsetRight(dx); break;
//         case ALIGN_RIGHT & ALIGN_TYPE_MASK: rc.OffsetLeft(-dx); break;
//         case ALIGN_TOP & ALIGN_TYPE_MASK: rc.OffsetBottom(dy); break;
//         case ALIGN_BOTTOM & ALIGN_TYPE_MASK: rc.OffsetTop(-dy); break;
//         default: return;
//         }
//         break;
// 
//     case LAYOUT_FLOW:
//          rc.OffsetRight(dx);
//          rc.OffsetBottom(dy);
//          break;
// 
//     default:
//         DbgAssert(0);
// //         rc.OffsetRight(dx);
// //         rc.OffsetBottom(dy);
//         break;
//     }
// }


TM_END_NAMESPACE
