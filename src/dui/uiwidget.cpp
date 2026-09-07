/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.14
 *
 *  Copyright (C) 2006 miragekiller
 */

#include "uiwidget.h"
#include "pic.h"
#include "frame.h"
#include "layout.h"
#include "view.h"
#include "uiwindow.h"
#include "helper.h"
#include "uiscrollbox.h"
#include "menupop.h"
#include "loader.h"
#include "tooltip.h"



TM_BGN_NAMESPACE



////////////////////////////////////////////////////////////////////////////////
//CFrameSourceT<CTuiWgt>
void CFrameSourceT<CTuiWgt>::FSGetHScrolBarRect(TUIRect& rcx, int nBarHeight)
{
    CTuiRect& rc = (CTuiRect&)rcx;
    HTUI hWnd = FSGetWin();
    hWnd->GetClientRect(rc);
    if (hWnd->IsLayoutFloatScroll())
        rc.KeepBSetH(nBarHeight);
    else
    {
        rc.SetTop(rc.Bottom());
        rc.SetHeight(nBarHeight);
    }
}

void CFrameSourceT<CTuiWgt>::FSGetVScrolBarRect(TUIRect& rcx, int nBarWidth)
{
    CTuiRect& rc = (CTuiRect&)rcx;
    HTUI hWnd = FSGetWin();
    hWnd->GetClientRect(rc);
    if (hWnd->IsLayoutFloatScroll())
        rc.KeepRSetW(nBarWidth);
    else
    {
        rc.SetLeft(rc.Right());
        rc.SetWidth(nBarWidth);
    }
}

void CFrameSourceT<CTuiWgt>::FSCalcShadow(CTuiRect& rc)
{
    HTUI hWnd = FSGetWin();
    if (hWnd->IsHighContrast())
        rc.InflateRect(
        -hWnd->m_bdMargin.left,
        -hWnd->m_bdMargin.top,
        -hWnd->m_bdMargin.right,
        -hWnd->m_bdMargin.bottom);
}


////////////////////////////////////////////////////////////////////////////////
//CTuiWgt
class CTuiWgt_ : public CTuiWgt
{
public:
    CTuiWgt_()
    {
        Set_WS_PRI_ALIGNAUTOSIZE();
    }
};

TM_REGISTER_CLASS2(CTuiWgt_, TM_CS(TUI_CLASS_WIDGET));


CTuiWgt::CTuiWgt()
    : m_dwID(0)
    , m_dwStyle(WS_VISIBLE)
    , m_dwAdvStyle(0)
    , m_dwLayout(0)
    , m_rcWnd(0, 0, 0, 0)
    , m_rcClient(0, 0, 0, 0)
    , m_nMinWidth(-1)
    , m_nMaxWidth(-1)
    , m_nMinHeight(-1)
    , m_nMaxHeight(-1)
    , m_nIDHint(0)
    , m_dwPriStyle(0)
    , m_hRoutePrintClient(0)
    , m_dwRole(ROLE_DEFAULT)
    , m_pBind(0)
{
    m_font.height = 0;
    m_font.styleMask = 0;
    DbgAssert(m_pBk == 0);
}

CTuiWgt::~CTuiWgt()
{
    _parent.Release();
    DbgAssert(!_child);
    DbgAssert(!_next);
    DbgAssert(!_prev);
}

TMBool CTuiWgt::NodeInsertAfter(CTuiWgt* p)
{
    if (p == this || IsLifeDestroyed())
        return false;
    
    if (p == HTUI_TOP)
    {
        p = GetParent();
        if (p && (p->_child != this))
        {
//             NodeDetach();
            p->InsertChildFirst(this);
        }
        else
            return false;
    }
    else if (p == HTUI_BOTTOM)
    {
        p = GetParent();
        if (p)
        {
//             NodeDetach();
            p->InsertChildAt(this, -1);
        }
        else
            return false;
    }
    else if (this != p->_next)
    {
//         NodeDetach();
        p->InsertAfter(this);
    }
    else
        return false;

    return true;
}

TMBool CTuiWgt::StringMessageToString(TMLParam lParam, CString& str)
{
    TUISETTEXT* p = (TUISETTEXT*)lParam;
    DbgAssert(p);
    if (p->nIDStr)
    {
        CModule* pModule;
        if (p->hModule)
        {
            pModule = GetView()->GetModule(p->hModule);
            if (!pModule)
                return false;
        }
        else
            pModule = GetModule();
        if (p->str)
            str = pModule->LoadString(p->nIDStr, tm_addressof(CStrView(*p->str)));
        else
            str = pModule->LoadString(p->nIDStr, 0);
    }
    else if (p->str)
        str = std::move(*p->str);
    else
        str.clear();

    return true;
}

TMBool CTuiWgt::LoadPicAndInvalidate(CPicPtr& ptr, int nObjID, TMLParam lParam)
{
    if (nObjID)
    {
        CModule* pm;
        if (lParam)
        {
            pm = GetView()->GetModule((TMHModule)lParam);
            if (!pm)
                return false;
        }
        else
            pm = GetModule();
        CPicPtr ptrPic;
        ptrPic.Create (*pm, nObjID, GetDPI());
        if (ptrPic.p == ptr.p)
            return false;
        ptrPic.Swap(ptr);
    }
    else
    {
        if (ptr.p == (ITuiPic*)lParam)
            return false;
        ptr = (ITuiPic*)lParam;
    }
    Invalidate();
    return true;
}

// TMBool CTuiWgt::SetSubFocus(void* pHit, TMUInt32 nFlag)
// {
//     return false;
// }

void CTuiWgt::RenderSetTextColor(IGXApp* gapp, int nFrame, CPropColor** clrs, CAppColor::PFNGetColor pfnText, int frmPfn)
{
    DbgAssert(pfnText);

    CPropColor clr;
    if (!IsHighContrast())
    {
        for (; ; ++clrs)
        {
            if (clrs[0])
            {
                if (clrs[0][nFrame].IsDefault())
                    continue;
                if (clrs[0][nFrame].IsParent())
                    return;
                clr = clrs[0][nFrame];
            }
            break;
        }
        DbgAssert(!clr.IsParent());
    }

    TMColor clr2;
    if (clr.IsDefault())
        clr2 = pfnText(frmPfn);
    else
        clr2 = clr.ToColor();
    clr2 = GetModule()->ConvertColor(GetObjectID(), clr2);

    gapp->SetTextColor(clr2);
}

void CTuiWgt::RenderListItem(TUIDRAWITEM* pDrawS, ITuiPic* pFore, int uIndent, CPropColor* clrTxt)
{
    DbgAssert(pDrawS);
    DbgAssert(pDrawS->dwDrawStage == CDDS_ITEMPREPAINT);
    IGXApp* gapp = pDrawS->gxApp;

    EButState nFrame = e_bsNormal;// itm.bSel ? e_bsDown : ((nHot == n1) ? e_bsHigh : e_bsNormal);
    if (pDrawS->itemState & ODS_DISABLED)
        nFrame = e_bsDisable;
    else if (pDrawS->itemState & ODS_SELECTED)
    {
        nFrame = e_bsDown;
        if (pDrawS->itemState & ODS_FOCUS)
            nFrame = e_bsFocus;
    }
    else if (pDrawS->itemState & ODS_HOTLIGHT)
        nFrame = e_bsHigh;

    DbgAssert(nFrame >= 0 && nFrame < 5);

    // text color
    gapp->Push(eGXPropTextColor);
    CPropColor* clrss[] = { clrTxt, 0 };
    RenderSetTextColor(gapp, nFrame, clrss, CAppColor::GetColorListTreeText);
//     TMColor clr;
//     if ((nColorVisual & eColorVisualHighContrast) || !clrTxt[nFrame])
//         clr = CAppColor::GetColorListTreeText(nColorVisual, nFrame);// HC_GetColorT(g_clrHCListText, nFrame);// GetView()->GetHCListColor(nFrame);
//     if (!(nColorVisual & eColorVisualHighContrast))
//         clr = GetModule()->ConvertColor(GetObjectID(), clr);
//     gxSetTextColor(clr);
    
    if (pFore)
    {
        if (IsHighContrast())
            GetPrePic(LISTBOX)->Render(nFrame, 0, pDrawS->rcItem, 0, gapp); // GetModule()->m_aPrePic.m_picDftListItem.Render(nFrame, pDrawS->rcItem, nColorVisual);
        else
            pFore->Render(nFrame, 0, pDrawS->rcItem, 0, gapp);
    }

    if (pDrawS->pColumn)
    {
        CTuiRect rcTmp(pDrawS->rcItem);
        int nColumn = pDrawS->pColumn->GetColumnCount();
        for (int i = 0; i < nColumn; ++i, rcTmp.KeepRSetL(rcTmp.Right()))
        {
            rcTmp.SetWidth(pDrawS->pColumn->GetColumnWidth(i));
            if ((pDrawS->itemState & ODS_SELECTED) && (pDrawS->pColumn->GetColumnFormat(i) & HDF_CURRENTSEL))
                gapp->DrawFocusRect(rcTmp);
            CText text;
            pDrawS->proxy->GetRenderText(pDrawS->itemData, pDrawS->pColumn->GetColumnData(i), text);
            if (text)
                text->Paint(rcTmp, 0, 0, gapp);
        }
    }
    else if (pDrawS->gxText && *pDrawS->gxText)
    {
        CTuiRect rcTmp(pDrawS->rcItem);
        rcTmp.KeepROffsetL(uIndent);
        (*pDrawS->gxText)->SetFormat(DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_WORD_ELLIPSIS);
        (*pDrawS->gxText)->Paint(rcTmp, 0, 0, gapp);
    }

    gapp->Pop();
}

void CTuiWgt::RenderFocusRect(IGXApp* gapp, const CTuiRect& rc)
{
    CView* pView = GetView();
    ITuiPic* pFocus = IsHighContrast() ? &GetModule()->m_aPrePic.m_picDftFocus : (ITuiPic*)GetModule()->m_ptrFocus;
    if (pFocus)
        pFocus->Render(e_bsNormal, 0, rc, 0, gapp);
}

void CTuiWgt::RenderSimpleBk(IGXApp* gapp, int nState, CAppColor::PFNGetColor pfn)
{
    if (m_pBk)
    {
        if (IsHighContrast())
            gapp->FillRect(m_rcClient, pfn(nState));   //HC_RenderBk(m_rcClient, nIndex);
        else
            m_pBk->Render(nState, 0, m_rcClient, 0, gapp);
    }
}

void CTuiWgt::RenderStaticBk(IGXApp* gapp, int nState, EButStates* states)
{
    if (!m_pBk)
        return;
    if (IsHighContrast())
    {
        switch (m_dwStyle & SS_BKONHCMASK)
        {
        case SS_RENDERBKONHC:   break;
        case SS_WINDOWBKONHC:   gapp->FillRect(m_rcClient, CAppColor::GetColorWindowBk(0)); return; //  HC_RenderBk(m_rcClient, COLOR_WINDOW); return;
        case SS_DLGBKONHC:      gapp->FillRect(m_rcClient, CAppColor::GetColorButtonBk(0)); return; //HC_RenderBk(m_rcClient, COLOR_3DFACE); return;
        case SS_CLEARBKONHC:    gapp->FillRect(m_rcClient, 0); return;
        default:                return;
        }
    }
    else
    {
        if (m_dwStyle & SS_NULLBKONNORMAL)
            return;
    }

    if (states)
        m_pBk->Render(nState, *states, m_rcClient, gapp);
    else
        m_pBk->Render(nState, 0, m_rcClient, 0, gapp);
}

void CTuiWgt::OnNcLButtonDown (TMLParam lParam)
{
    CView* p = GetView();
    EHitID hidNew = p->GetHitInfo().hid;

    if (hidNew == eHitIDSys)
    {
//         TM::CPointI pt;
//         DbgVerify(::GetCursorPos(&pt));
//        ::SendMessage(GetHWND(), WM_SYSCOMMAND, SC_MOUSEMENU, pt.ToLPARAM());
        SendMessage(WM_SYSCOMMAND, SC_MOUSEMENU, lParam);
    }
    else if (TUIHITGETOBJ(hidNew) == e_objOBorder)
    {
        if ((m_dwStyle & WS_SIZEBOX) && !(m_dwStyle & (WS_MINIMIZE | WS_MAXIMIZE)))
            SendMessage(WM_SYSCOMMAND, SC_SIZE | TUIHITGETSUBSZ(hidNew), 0);
    }
    else if (hidNew == eHitIDCaption)
    {
        if (!(m_dwStyle & WS_MAXIMIZE))
            SendMessage(WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
    }
}

void CTuiWgt::OnNcLButtonUp (TMLParam lParam)
{
    CView* p = GetView();
    EHitID hidOld = p->GetDownInfo().hid;
    EHitID hidNew = p->GetHitInfo().hid;
    if (this == p->GetDown() && hidOld == hidNew)
    {
        TMWParam wp;
        switch (hidNew)
        {
        case eHitIDMax:     wp = CanRestore() ? SC_RESTORE : SC_MAXIMIZE; break;
        case eHitIDMin:     wp = SC_MINIMIZE; break;
        case eHitIDClose:   wp = SC_CLOSE; break;
        case eHitIDHelp:    wp = SC_CONTEXTHELP; break;
        default:
            return;
        }
        SendMessage(WM_SYSCOMMAND, wp, 0);
        Invalidate();
        UpdateWindow();
    }
}

void CTuiWgt::OnNcLButtonDblClk (TMLParam lParam)
{
    CView* p = GetView();
    EHitID hidNew = p->GetHitInfo().hid;
    /*
    if (hidNew == eHitIDSys)
        DbgVerify(::PostMessage (GetHWND(), WM_SYSCOMMAND, SC_CLOSE, 0));
    else*/ if (hidNew == eHitIDCaption)
    {
        if (m_dwStyle & WS_MAXIMIZEBOX)
        {
            TMUInt uID = (m_dwStyle & (WS_MINIMIZE | WS_MAXIMIZE)) ? SC_RESTORE : SC_MAXIMIZE;
            SendMessage (WM_SYSCOMMAND, uID, 0);
        }
    }
}

// print self and all sibling and all child
struct _CTuiWgt_PrintSiblings
{
    Ref pTab;
    TMLParam lParam;
    IGXApp* gapp;

    void __fastcall done(CTuiWgt* p)
    {
        if (!p)
            return;
        
        Ref ref(p);
        done(p->GetNext());
        if (p->Is_WS_PRI_IS_CURRENTTAB())
        {
            DbgAssert(!pTab);
            pTab = p;
        }
        else
            CWidgetHelper::DoPrintSelf(gapp, p, lParam);
    }
};

// the last print message maybe have been deleted.
static void _CTuiWgt_RenderFrame(IGXApp* gxApp, HTUI hWnd, TMLParam lParam, TMLParam lPRender)
{
    if (!(lParam & PRF_NONCLIENT))
        return;
    
    CFrame* pFrame = hWnd->GetFrame();
    if (!pFrame)
        return;
    
    TMResult lr;
    pFrame->ProcessFrameMessage(hWnd, CM_PRINTCLIENT, (TMWParam)gxApp, lPRender, lr);
}

void CTuiWgt::DoPrint(IGXApp* gxApp, TMLParam lParam)
{
//     CAutoMsgAnalysis awm1("CTuiWgt::DoPrint1", GetObjectID());
    gxApp->FontSetCurrent(m_font);
    
//     CAutoMsgAnalysis awm2("CTuiWgt::DoPrint2", GetObjectID());
    if (m_dwPriStyle & WS_PRI_FRAMECHANGED)
        SetWindowPos(0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);// | SWP_NOREDRAW);
    
    TM_ANALYSIS_UNUSED_BEGIN();
    TUI_IF_EDIT(m_fpsRend.Reset());
    TUI_IF_EDIT(m_fpsRendSelf.Reset());
    TUI_IF_EDIT(TMInt64 nTmChild = 0);
    TM_ANALYSIS_UNUSED_END();

//     CAutoMsgAnalysis awm3("CTuiWgt::DoPrint3", GetObjectID());
    _CTuiWgt_RenderFrame(gxApp, this, lParam, lParam);
    
//     CAutoMsgAnalysis awm4("CTuiWgt::DoPrint4", GetObjectID());
    TMBool bDrawClient = true;
    TMBool bClipChild = IsClipChild();
    if (bClipChild)
        bClipChild = bDrawClient = gxApp->ClipAndPush(m_rcClient, true, eGXPropClip);

//     CAutoMsgAnalysis awm5("CTuiWgt::DoPrint5", GetObjectID());
    if (bDrawClient)
    {
        if (lParam & PRF_CLIENT)
            SendPrintClientMessage(lParam);
        
//         CAutoMsgAnalysis awm6("CTuiWgt::DoPrint6", GetObjectID());
        if (lParam & PRF_CHILDREN)
        {
            _CTuiWgt_PrintSiblings ps;
            ps.lParam = (lParam & ~PRF_BIND);
            ps.gapp = gxApp;

            TUI_IF_EDIT(CCycle tmChild);
            ps.done(GetChild());
            TUI_IF_EDIT(nTmChild = tmChild.StepUp());

            if (ps.pTab)
                CWidgetHelper::DoPrintSelf(gxApp, ps.pTab, ps.lParam);
        }
    }
    
//     CAutoMsgAnalysis awm7("CTuiWgt::DoPrint7", GetObjectID());
    if (bClipChild)
        gxApp->Pop();

//     CAutoMsgAnalysis awm8("CTuiWgt::DoPrint8", GetObjectID());
    _CTuiWgt_RenderFrame(gxApp, this, lParam, PRF_FLOATSCROLLBAR);

    TM_ANALYSIS_UNUSED_BEGIN();
    TUI_IF_EDIT(m_fpsRend.StepUp());
    TUI_IF_EDIT(m_fpsRendSelf.StepUp(-nTmChild));
    TM_ANALYSIS_UNUSED_END();
}

TMResult CTuiWgt::Dispatch(TMUInt msg, TMWParam wp, TMLParam lp)
{
    switch (msg)
    {
    case CM_LANGUAGECHANGED:
    case CM_RENDERCHANGED:
        SendMessage(msg, (TMWParam)wp, lp);
        break;
        
    case CM_COLORVISUAL:
        SendMessage(CM_COLORVISUAL, wp, lp);
        break;
    }
    return 0;
}

TMBool CTuiWgt::ProcessWindowMessage(CTuiWgt*, TMUInt uMsg,
                                     TMWParam wParam, TMLParam lParam,
                                     TMResult& lResult, TMULong dwMsgMapID)
{
    if (uMsg == WM_PRINT)
    {
        CWidgetHelper::DoPrintSelf(_gxApp, this, lParam);
        return true;
    }
    
    {
        CFrame* pFrame = GetFrame();
        if (pFrame)
        {
            TMBool bHandled = pFrame->ProcessFrameMessage(this, uMsg, wParam, lParam, lResult);
            if (bHandled || IsLifeDestroyed())
                return true;
        }
    }

    typedef CScrollImpl<CTuiWgt> scroll_impl;
    if (scroll_impl::ProcessWindowMessage(this, uMsg, wParam, lParam, lResult, dwMsgMapID))
        return true;

    switch (uMsg)
    {
    case WM_NCCREATE:
         ChangeFrame (false);
        break;

    case WM_INITDIALOG:
        lResult = true;
        return true;

//     case CM_BEGINMENUPOP:

    case CM_ENDMENUPOP:
        if (lParam)
        {
            TUIMSG* pMsg = (TUIMSG*)lParam;
            if (pMsg->message == CM_ENDMENUPOPSHOW)
            {
                if (!pMsg->wParam)
                    SetWindowPos(0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
                return true;
            }
        }
        DestroyWindow();
        return true;

    case WM_PRINTCLIENT:
        DoPrintClient(_gxApp, lParam); //(TMHDC)wParam, COLOR_WINDOW);
        break;

    case CM_SETFRAME:
        if (!m_pFrame.IsSameObject(m_pFrame, (TMHModule)lParam, wParam))
        {
            CModule* pm;
            if (lParam)
            {
                pm = GetView()->GetModule((TMHModule)lParam);
                if (!pm)
                    break;
            }
            else
                pm = GetModule();
            m_pFrame.Create(*pm, wParam, GetDPI());
        }
        
        ChangeFrame(false);
        return lResult = true;

    case CM_SETBKPIC:
        if (m_pBk != (ITuiPic*)(lParam))
        {
            m_pBk = (ITuiPic*)(lParam);
            m_pBk.ChangeDPI(GetDPI());
            Invalidate();
        }
        return lResult = true;

    case CM_SETBKPIC2:
        {
            CModule* pm;
            if (lParam)
            {
                pm = GetView()->GetModule((TMHModule)lParam);
                if (!pm)
                    break;
            }
            else
                pm = GetModule();
            CPicPtr ptr;
            ptr.Create(*pm, wParam, GetDPI());
            if (m_pBk != ptr.p)
            {
                m_pBk = ptr.p;
                Invalidate();
            }
        }

        return lResult = true;

    case CM_GETBKPIC:
       lResult = (TMResult)static_cast<ITuiPic*>(m_pBk.p);
       return true;
 
    case CM_SETHINT:
        if (StringMessageToString(lParam, m_strHint) && g_mgrTooltip.IsTooltipTarget(this))
            g_mgrTooltip.ReactiveToolTip(this);
        lResult = true;
        break;

    case CM_ACTIVEHINT:
        g_mgrTooltip.SetToolTip(lParam ? this : 0, lParam ? eTTActiveNow : eTTActiveNormal);
        lResult = true;
        break;

    case CM_SETACCTEXT:
        StringMessageToString(lParam, m_strAcc);
        break;

    // can pop drop list
//     case CM_POPDROPLIST:
//         lResult = true;
//         break;

//     case CM_MSGROUTE:
//         CWidgetHelper::RouteMessage(lParam);
//         lResult = true;
//         break;
    
    case WM_SETTEXT:
        DoSetText((TMCStr)lParam, wParam);
        lResult = true;
        Invalidate();
        break;

    case CM_SETTEXT:
        {
            CString str;
            lResult = StringMessageToString(lParam, str);
            if (lResult)
            {
                DoSetText(str, ((TUISETTEXT*)lParam)->bHtml);
                Invalidate();
            }
        }
        break;

    case WM_GETTEXT:
        lResult = m_gxText.GetString().CopyTo((TMStr)lParam, wParam);
        break;

    case WM_GETTEXTLENGTH:
        lResult = m_gxText ? m_gxText->GetLength() : 0;
        break;

    case CM_GETSTATE:
        lResult = GetStates();
        break;

    case CM_SETGROUPDFTTABSTOP:
        lResult = SetGroupDefaultTabStop();
        break;

//     case CM_SETSTATE:
//         if ((m_dwAdvStyle & WS_ADV_ATTACHMENT) && _parent)
//             GetParent()->SendMessage(CM_SETSTATE, wParam, lParam);
//         break;

    case CM_SETFONT:
        m_font = *(GXFont*)wParam;
        if (lParam)
            Invalidate();
        lResult = true;
        break;

//     case CM_GETCONTROLFONT:
//         *(GXFont*)wParam = m_font;
//         lResult = true;
//         break;

    case CM_GETFONT:
        if (lParam)
            *(GXFont*)wParam = m_font;
        else
            GetRealFont(*(GXFont*)wParam);
        lResult = true;
        break;

    case WM_QUERYOPEN:
    case WM_QUERYENDSESSION:
    case WM_DEVICECHANGE:
    case WM_POWERBROADCAST:
        lResult = TMTrue;
        break;

//     case CM_GESTUREMOVE:
    case CM_PRENOTIFY: uMsg = CM_NOTIFY; goto _process_bubble;
    case CM_PRECOMMAND: uMsg = WM_COMMAND;  tm_fallthrough;
    case WM_MOUSEWHEEL:     tm_fallthrough;
    case WM_CONTEXTMENU:    tm_fallthrough;
    case WM_COMMAND:        tm_fallthrough;
    case WM_PARENTNOTIFY:
_process_bubble:
        {
            CTuiWgt* p = GetParent();
            if (p)
                lResult = p->SendMessage(uMsg, wParam, lParam, eSendMsgProc);
        }
        break;

    case CM_MSGBUBBLE:
        if (IsArrowGroup() && GetView()->IsArrowGroupMessage(GetOwner(), (TUIMSG*)lParam))
            return lResult = true;
        goto _process_bubble;

    case CM_POSTCALLBACK:
        lResult = PostCallback((PFNPostCallback)wParam, lParam);
        break;

    case WM_CHARTOITEM:
    case WM_VKEYTOITEM:
        lResult = -1L;
        break;

    case WM_ACTIVATE:
        break;

    case WM_SETREDRAW:
        SettingFlagRedraw(wParam);
        if (wParam)
            Invalidate();
//         if (wParam)
//             m_dwPriStyle &= (~WS_PRI_DISABLEREDRAW);
//         else
//             m_dwPriStyle |= WS_PRI_DISABLEREDRAW;
        break;

    case CM_WINDOWPOSCHANGED:
        {
            TUIWINDOWPOS* p = (TUIWINDOWPOS*)lParam;
            if (!(p->flags & SWP_NOMOVE))
                SendMessage(WM_MOVE, 0, MAKELONG(p->x, p->y));
            if (!(p->flags & SWP_NOSIZE))
                SendMessage(WM_SIZE, 0, MAKELONG(p->cx, p->cy));
            if ((p->flags & SWP_SHOWWINDOW) || (p->flags & SWP_HIDEWINDOW))
                SendBroadcastMessage(CM_SHOWWINDOW_P, 0 != (m_dwStyle & WS_VISIBLE), 0, false);

//             if ((p->flags & (SWP_NOMOVE | SWP_NOSIZE)) != (SWP_NOMOVE | SWP_NOSIZE))
//                 LayoutAutoParent(this, _parent);
        }
        break;

    case CM_SHOWWINDOW_P:
    case WM_SHOWWINDOW:
        if (!wParam)
            Reclaim();
    // through
    case CM_ENABLE_P:
    case WM_ENABLE:
        if (!wParam)
            GetView()->ClearChildInfo(this, uMsg);
        break;

    case CM_GESTURENOTIFY:
        lResult = CGesture::OnCMGestureNotify(this);
        return true;

    case CM_GESTURE:
        lResult = CGesture::OnCMGesture(this, wParam);
        return true;

    case WM_SYSCOMMAND:
        if (wParam == SC_CLOSE)
            SendMessage(WM_CLOSE, 0, 0);
        else if ((wParam & 0xFFF0) == SC_SIZE || (wParam & 0xFFF0) == SC_MOVE)
            CFrame::DragMoveSize(this, wParam);
//         else if (wParam & (SC_SIZE | SC_MOVE))
//             SetCapture();
        break;

    case WM_CLOSE:
        DestroyWindow();
        break;

    case CM_SET_PADING:
        if (m_bdPadding.SafeAssign((CTuiRect*)wParam))
            ChangeFrame(false);
        break;

    case CM_GET_PADING:
        m_bdPadding.CopyTo((CTuiRect*)lParam);
        break;

    case CM_SET_MARGIN:
        if (m_bdMargin.SafeAssign((CTuiRect*)wParam))
            CheckParentFrameChange(0);
        break;

    case CM_GET_MARGIN:
        m_bdMargin.CopyTo((CTuiRect*)lParam);
        break;

    case CM_GETMINMAXINFO:
        {
            TUIMINMAXINFO* p = (TUIMINMAXINFO*)lParam;
            p->nMaxHeight = m_nMaxHeight;
            p->nMinHeight = m_nMinHeight;
            p->nMaxWidth  = m_nMaxWidth;
            p->nMinWidth  = m_nMinWidth;
        }
        break;

    case CM_SETMINMAXINFO:
        {
            TUIMINMAXINFO* p = (TUIMINMAXINFO*)lParam;
            TMBool b = false;
            if ((wParam & SWMMI_MAXHEIGHT) && (m_nMaxHeight != p->nMaxHeight)) m_nMaxHeight = p->nMaxHeight, b = true;
            if ((wParam & SWMMI_MINHEIGHT) && (m_nMinHeight != p->nMinHeight)) m_nMinHeight = p->nMinHeight, b = true;
            if ((wParam & SWMMI_MAXWIDTH) && (m_nMaxWidth  != p->nMaxWidth)) m_nMaxWidth = p->nMaxWidth, b = true;
            if ((wParam & SWMMI_MINWIDTH) && (m_nMinWidth  != p->nMinWidth)) m_nMinWidth = p->nMinWidth, b = true;
            if (b)
            {
                ChangeFrame(false);
                CheckParentFrameChange(0);
            }
        }
        break;

    case CM_NCSETCURSOR:
//         DbgTrace("CM_NCSETCURSOR");
        if (m_dwStyle & WS_SIZEBOX)
        {
            CView* pView = GetView();
            const TUIHITINFO& ht = pView->GetHitInfo();
            if (SetNcCursor(ht.hid))
                return lResult = true;
        } // through
    case WM_SETCURSOR:
//         DbgTrace("WM_SETCURSOR");
        //GetView()->InternalSetCursor(IDC_ARROW);
        OSSetCursor(CCursorAT::cursor_ARROW());
        lResult = true;
        break;

    case WM_NEXTDLGCTL:
        if (!lParam)
            SetNextFocus(wParam, eFocusReasonUnknown);
        else if (wParam)
            ((HTUI)wParam)->SetFocus(eFocusReasonUnknown);
        break;

    case WM_NCLBUTTONDOWN:
        OnNcLButtonDown(lParam);
        break;

    case WM_NCLBUTTONUP:
        OnNcLButtonUp(lParam);
        break;

    case WM_NCLBUTTONDBLCLK:
        OnNcLButtonDblClk(lParam);
        break;

    case CM_GETLOCALE:
        lResult = GetModule()->GetLocal();
        return true;

    case CM_GETCOLORVISUAL:
        lResult = GetModule()->GetVisual();
        return true;

    case CM_SETDPI:
        if (wParam != m_nDPI)
        {
            int n = m_nDPI;
            m_nDPI = wParam;
            Reclaim();
            ChangeFrame(false);
            SendMessage(CM_ZOOMED, wParam, n);
            SendBroadcastMessage(CM_SETDPI, wParam, lParam, false, 1);
        }
        return true;

//     case CM_GETDPI:
//         lResult = m_nDPI;
//         return true;

    case CM_ZOOMED:
        m_pFrame.ChangeDPI(wParam);
        m_pBk.ChangeDPI(wParam);
//         m_dpi = wParam;
        break;

    case WM_DESTROY:
        m_pBk.Release();
        m_pFrame.Release();
        TM_IF_DBG(*(TMBool*)lParam = true);
        break;

    case WM_NCDESTROY:
        TM_IF_DBG(*(TMBool*)lParam = true);
        break;

//     case WM_NCMOUSEMOVE:
//         OnNcMouse (pSender, pSender->GetView()->GetHitInfo().dwCode, uMsg, lParam);
//         break:

    case WM_CHAR:           tm_fallthrough;
    case WM_SYSCHAR:        tm_fallthrough;
    case WM_KEYDOWN:        tm_fallthrough;
    case WM_KEYUP:          tm_fallthrough;
    case WM_DEADCHAR:       tm_fallthrough;
    case WM_SYSKEYDOWN:     tm_fallthrough;
    case WM_SYSKEYUP:       tm_fallthrough;
    case WM_SYSDEADCHAR:    tm_fallthrough;
    case WM_KEYLAST:        tm_fallthrough;
    case WM_CANCELMODE:
        lResult = SendBubbleMessage(uMsg, wParam, lParam);
        break;

    case CM_INVALIDATE:
        Invalidate();
        return true;

    case CM_LANGUAGECHANGED:
    case CM_RENDERCHANGED:
        if (!Is_WS_PRI_ALIGNAUTOSIZE())
            Set_WS_PRI_DIRTY();
        Reclaim();
        break;

    default:
        break;
    }

    return true;
}

TMResult CTuiWgt::SendBubbleMessage(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
{
    CTuiMsg msg(this, uMsg, wParam, lParam);
    return SendMessage(CM_MSGBUBBLE, 0, (TMLParam)&msg, eSendMsgProc);
}


void CTuiWgt::OnLoadProp (IDomElement* pXml)
{
    m_rcClient = m_rcWnd;
    AdjustClientRect(m_rcClient);
//     if (IsAutoWidth())
//         m_nScrollW = m_rcClient.Width() - m_bdPadding.left - m_bdPadding.right;
//     if (IsAutoHeight())
//         m_nScrollH = m_rcClient.Height() - m_bdPadding.top - m_bdPadding.bottom;
//    ChangeFrame (true);
}


void CTuiWgt::GetMouseTarget(HTUI& p, HTUI& pHint, const CTuiPoint& pt, TMUInt uFlags)
{
    if ((uFlags & CWP_SKIPINVISIBLE) && !IsWindowVisible())
        return;
    if ((uFlags & CWP_SKIPBINDWINDOW) && Is_WS_PRI_BINDMODE())
        return;
    if (!m_rcWnd.PtInRect(pt))
        return;

    DbgAssert(!p || !pHint);

    if ((uFlags & CWP_SKIPDISABLED) && (m_dwStyle & WS_DISABLED))
    {
        if (!p)
            p = this; // find hit, but need find hint in childs.
        if (pHint) 
            return;
    }

    DbgAssert(!p || !pHint);

    // check size box
    TMBool bCheckSizeBox = false;
    if (WS_SIZEBOX == (GetStyle() & (WS_SIZEBOX | WS_MAXIMIZE)))
    {
        CFrame* frm = GetFrame();
        if (frm)
            bCheckSizeBox = frm->PtInOBorder(m_rcWnd, pt);
//         {
//             CTuiRect rc(m_rcWnd);
//             rc.InflateRect(-4, -4);
//             bCheckSizeBox = !rc.PtInRect(pt);
//         }
    }

    // check clip child
    if (!bCheckSizeBox)
    {
        TMBool bCheckChild = !IsClipChild();
        if (!bCheckChild)
        {
            if (IsLayoutFloatScroll())
            {
                CTuiRect rc(m_rcWnd);
                AdjustClientRect(rc, m_dwLayout & ~LAYOUT_FLOATSCROLL);
                bCheckChild = rc.PtInRect(pt);
            }
            else
                bCheckChild = m_rcClient.PtInRect(pt);
        }

        if (bCheckChild)
        {
            for (CTuiWgt::CWndRec rec(GetChilds()); rec; ++rec)
            {
                CTuiWgt* p2 = *rec;
                p2->GetMouseTarget(p, pHint, pt, uFlags);
                if (p && pHint)
                    return;
            }
        }
    }

    DbgAssert(!p || !pHint);
    if ((GetExStyle() & WS_ADV_PENETRATE) && (uFlags & CWP_SKIPTRANSPARENT))
    {
        if (!pHint && GetWindowHint(eTTActiveNormal, 0))
            pHint = this;
    }
    else if (!p)
    {
        p = this;
        if (!pHint)
            pHint = this;
    }
    else if (!pHint)
        pHint = this;
    
    return;
}

TMBool CTuiWgt::GetAndHitMouseTarget(TUIHITINFO& ht, TMUInt uMsg, TMUInt uFlagsExt)
{
    DbgAssert(!IsLifeInvalid());

    HTUI pNew = 0;
    HTUI pHint = 0;
    GetMouseTarget(pNew, pHint, ht.pt, uFlagsExt | CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);

    if (pHint == pNew)
        pHint = 0;
    
    // if float set WS_ADV_PENETRATE and hint. it maybe pNew==0, and pHint != 0;
    ht.hWnd = pNew;
    ht.hHint = pHint;
    ht.pTaget = 0;
    ht.bNC = false;
    ht.dwCode = 0;

    if (pNew)
    {
        DbgAssert(!pNew->IsLifeInvalid());
        Ref ref1(pNew);
        Ref ref2(pHint);
        pNew->SendMessage(CM_HITTEST, uMsg, (TMLParam)&ht);
        if (ht.hWnd && ht.hWnd->IsLifeInvalid())
            ht.hWnd = 0;
        if (ht.hHint && ht.hHint->IsLifeInvalid())
            ht.hHint = 0;
    }
    return true;
}


void CTuiWgt::FinalConstruct(CTuiObject* pParent)
{
    if (pParent)
        ((CTuiWgt*)pParent)->InsertChildAt (this, -1);
}

void CTuiWgt::FinalRelease()
{
    m_aniList.ClearAni();
}

static GXDips _CTuiWgt_CalcMinMax(GXDips n, GXDips nMin, GXDips nMax)
{
    if (nMin >= 0 && n < nMin)
        n = nMin;
    else if (nMax >= 0 && n > nMax)
        n = nMax;
    return n;
}

TMBool CTuiWgt::PreCalcWindowPos(CTuiWidgetPos& wp, const FRAMEINFO& fi, const CTuiRect& rcMinMax)
{
    TMBool bAutoWidth = IsAutoWidth() && (m_nScrollW > 0);
    TMBool bAutoHeight = IsAutoHeight() && (m_nScrollH > 0);
//     if (!bAutoWidth && !bAutoHeight)
//         return false;
    
    if (_parent && _parent->Is_WS_PRI_IS_SCROLLBOX() && _parent->GetChild() == this)
    {
        wp.x = _parent->m_rcClient.Left() + _parent->m_bdPadding.left + m_bdMargin.left - _parent->GetScrollLeft();
        wp.y = _parent->m_rcClient.Top() + _parent->m_bdPadding.top + m_bdMargin.top - _parent->GetScrollTop();
    }
    
    GXDips nNewW = wp.cx;
    if (bAutoWidth)
        nNewW = m_nScrollW + m_bdPadding.left + m_bdPadding.right + fi.m_border.Left() + fi.m_border.Right();
    nNewW = _CTuiWgt_CalcMinMax(nNewW, rcMinMax.Left(), rcMinMax.Right());

    GXDips nNewH = wp.cy;
    if (bAutoHeight)
        nNewH = m_nScrollH + m_bdPadding.top + m_bdPadding.bottom + fi.m_border.Top() + fi.m_border.Bottom();
    nNewH = _CTuiWgt_CalcMinMax(nNewH, rcMinMax.Top(), rcMinMax.Bottom());

    GXDips dx = nNewW - wp.cx;
    GXDips dy = nNewH - wp.cy;
    GXDips eps = GetEps();
    if (!CMath::Equal0(dx, eps) || !CMath::Equal0(dy, eps))
        wp.DeltaSize(m_dwLayout, dx, dy); //DeltaSize(dx, dy, rc);

//     wp.x = rc.left;
//     wp.y = rc.top;
//     wp.cx = rc.Width();
//     wp.cy = rc.Height();

    return true;
}

CFrame* CTuiWgt::GetFrame()
{
    return m_pFrame.p;
}

static void _CTuiWgt_Detach(HTUI hWnd)
{
    if (hWnd->Is_WS_VISIBLE())
    {
        hWnd->CheckParentFrameChange(SWP_HIDEWINDOW);
        hWnd->Invalidate();
    }
    hWnd->DetachSimple();
    hWnd->_next = 0;
    hWnd->_prev = 0;
}

bool CTuiWgt::DestroyWindow()
{
    DbgAssert(IC_IsValidInstance(true));
    
    Ref ref(this);

#ifdef TM_DBG
    int oid = GetObjectID();
#endif

    if (!IsLifeInvalid())
    {
        SetLife(WS_PRI_LIFE_DESTROYING);
#ifdef TM_DBG
        TMBool p = 0;
        SendMessage(WM_DESTROY, 0, (TMLParam)&p);
#   ifndef TUI_TEST_MODE
        DbgAssert(p)("please set bhandled = false on OnDestroy(%d)", GetObjectID()); 
#   endif
#else
        SendMessage(WM_DESTROY, 0, 0);
#endif
    }

    if (GetLife() == WS_PRI_LIFE_DESTROYING)
    {
        SetLife(WS_PRI_LIFE_DESTROY_CLD);
        tm_for (CTuiWgt* p; p = GetChild();)// p->GetNext())
        {
            DbgAssert(!p->IsLifeDestroyed());
            p->DestroyWindow();// AddRef();
            DbgAssert(p != GetChild());
        }
    }

    if (GetLife() == WS_PRI_LIFE_DESTROY_CLD)
    {
        SetLife(WS_PRI_LIFE_DETACH);
        _CTuiWgt_Detach(this);
        CView* pView = GetView();
        pView->ClearChildInfo(this, WM_DESTROY);
    }

    if (GetLife() == WS_PRI_LIFE_DETACH)
    {
        SetLife(WS_PRI_LIFE_NCDESTROY);
#ifdef TM_DBG
        TMBool p = 0;
        SendMessage(WM_NCDESTROY, 0, (TMLParam)&p);
#   ifndef TUI_TEST_MODE
        DbgAssert(p)("please set bhandled = false on OnNCDestroy(%d)", GetObjectID());
#   endif
#else
        SendMessage(WM_NCDESTROY, 0, 0);
#endif
        SetLife(WS_PRI_LIFE_DESTROYED);
//        DbgAssert(!m_procMgrWgt.Unsubclass(this, this));
        DbgAssert(!Unsubclass_self());
        m_aniList.PreDestroy();
        CView* pView = GetView();
        DbgAssert(pView->GetFocus() != this);
        Release();
    }
    
    return true;
}

void CTuiWgt::InitConstruct(TUIFACTORY* pFactory, void* param)
{
    DbgAssert(IsLifeNew());

    // subclass self and default factory create
    CWidgetHelper::BroadcastSubclass(this);
    
    // factory create 
    if (pFactory)
        pFactory->creator(this, pFactory->param1, pFactory->param2);
    
    // send WM_NCCREATE && WM_CREATE message
    CWidgetHelper::BroadcastInit(this, param);
    DbgAssert(!IsLifeNew()); // maybe destroyed.
}

void CTuiWgt::UnInitOnManagerFail()
{
    for (CTuiWgt* p = GetChild(); p; p = GetChild())
        p->UnInitOnManagerFail ();
//    m_procMgrWgt.Unsubclass(this, this);
    Unsubclass_self();
    NodeDetach();
    Release();
}

EBindState CTuiWgt::IsBindOrSub(HTUI hParent)
{
    if (Is_WS_PRI_BINDCBLIST())
        return eBindStateCBList;
    if (Is_WS_PRI_BINDMODE())
        return eBindStateSelf;
    if (this == hParent || _parent == 0)
        return eBindStateNone;
    return _parent->IsBindOrSub(hParent) ? eBindStateParent : eBindStateNone;
}

TMBool CTuiWgt::IsBindSelected()
{
    DbgAssert(Is_WS_PRI_BINDMODE());
    DbgAssert(_parent);
    return _next || _prev || _parent->_child == this;
}

void CTuiWgt::ResetBindItemPosition(CTuiRect rcWnd, TMUInt32 uRedrawFlag, TMBool bForce)
{
    ShrinkMargin(rcWnd);
    if (bForce || !rcWnd.EqualRect(GetBoundRect(), GetEps()))
        SetWindowPos(0, &rcWnd, SWP_NOZORDER | SWP_NOOWNERZORDER | uRedrawFlag);
}

void CTuiWgt::Bind_Set(HTUI hParent, IBindOwner* pOwner, TMBool bSelect)
{
    DbgAssert(!IsChild(hParent));
    DbgAssert(!IsLifeInvalid());
    DbgAssert(!hParent->IsLifeInvalid());
    DbgAssert(hParent != this);

    Set_WS_PRI_BINDMODE();
    NodeDetach();
    if (bSelect)
    {
        DbgAssert(!IsRadio());
        hParent->InsertChildFirst(this);
    }
    else
        _parent = hParent;
    m_pBind = pOwner;
}

void CTuiWgt::Bind_Reset(HTUI hParent)
{
    Reset_WS_PRI_BINDMODE();
//    NodeDetach();
    SetParent(hParent);
    DbgAssert(hParent == _parent);
    m_pBind = 0;
}

TMBool CTuiWgt::SetGroupDefaultTabStop()
{
    if (!(GetStyle() & WS_GROUPTABSTOP))
        return false;
    CTuiWgt::CItem itm(this);
    for (; itm.CycleNext(), *itm != this;)
    {
        if (itm->GetStyle() & WS_GROUPTABSTOP)
            itm->Set_WS_PRI_NOTABSTOP();
    }
    Reset_WS_PRI_NOTABSTOP();
    return true;
}

TMBool CTuiWgt::IsAutoWidth() const
{
    return (m_dwLayout & LAYOUT_AUTOWIDTH);
//         && !IsAlignV()
//         && !IsAlignClient()
//         && !IsAnchorFrame();
}

TMBool CTuiWgt::IsAutoHeight() const
{
    return (m_dwLayout & LAYOUT_AUTOHEIGHT);
//         && !IsAlignH()
//         && !IsAlignClient()
//         && !IsAnchorVFrame();
}

bool CTuiWgt::_MakeWindowHInt(CString* str, const GX::CText& txt)
{
    if (txt && GetView()->m_bShowHintOnEllipsis)
    {
        TMUInt32 fmt = txt->GetFormat();
        if (fmt & DT_STATE_ELLIPSIS)
        {
            if (str)
                *str = txt->GetString();
            return true;
        }
    }
    return false;
}

bool CTuiWgt::_MakeWindowHInt(CString* str, const CString& strHint, const GX::CText& txt)
{
    if (strHint)
    {
        if (str)
            *str = strHint;
        return true;
    }
    return _MakeWindowHInt(str, txt);
}

bool CTuiWgt::_MakeWindowHInt(CString* str, IColumnProxy* proxy, TMLParam lPItem, TMLParam lPHead)
{
    CString strHint(proxy->GetHInt(lPItem, lPHead));
    if (strHint)
    {
        if (str)
            *str = strHint;
        return true;
    }
    
    GX::CText txt;
    proxy->GetRenderText(lPItem, lPHead, txt);
    return _MakeWindowHInt(str, txt);
}

bool CTuiWgt::GetWindowHint(ETTActive ett, CString* str)
{
    return _MakeWindowHInt(str, m_strHint, m_gxText);
}

TMUInt32 CTuiWgt::SetStyle(TMUInt32 dwStyle)
{
    if (dwStyle != m_dwStyle)
    {
        Ref ref(this);
        TMSTYLESTRUCT ss = { m_dwStyle, dwStyle };
        SendMessage(WM_STYLECHANGING, GWL_STYLE, (TMLParam)&ss);
        if (m_dwStyle != ss.styleNew)
        {
            m_dwStyle = ss.styleNew;
            SendMessage(WM_STYLECHANGED, GWL_STYLE, (TMLParam)&ss);
        }
        return ss.styleOld;
    }
    return dwStyle;
}

TMUInt32 CTuiWgt::SetExStyle (TMUInt32 dwStyle)
{
    if (dwStyle != m_dwAdvStyle)
    {
        TMSTYLESTRUCT ss = { m_dwAdvStyle, dwStyle };
        SendMessage (WM_STYLECHANGING, GWL_EXSTYLE, (TMLParam)&ss);
        m_dwAdvStyle = dwStyle;
        SendMessage (WM_STYLECHANGED, GWL_EXSTYLE, (TMLParam)&ss);
    }
    return dwStyle;
}

EButState CTuiWgt::GetButtonState () const
{
    if (!IsWindowEnabled())
        return e_bsDisable;

    CView* pm = GetView();
    const CTuiWgt* pthis = this;
    const CTuiWgt* pover = pm->GetOver();


    if (pm->IsMouseDown() && pthis == pover)
        return e_bsDown;

    if (pthis == pover)
        return e_bsHigh;
//     for (; pover;)
//     {
// 
// //         if (pover->GetExStyle() & WS_ADV_ATTACHMENT)
// //             pover = pover->m_pParent;
// //         else
//             break;
//     }

//    const CTuiWnd* pMgr = GetOwner();
    if (this == pm->GetFocus()) // || (/*pm->InternalIsFocus(this) ||*/ (m_dwPriStyle & MGR_PRI_GROUPACTIVATE)))
    {
        if (!IsHideFocus())
            return e_bsFocus;
    }
    return e_bsNormal;
}

static TMBool _CTuiWgt_IsDefaultItem(HTUI p)
{
    CTuiWnd* pm = p->GetOwner();
    if (!pm)
        return false;
    TMUInt32 id = pm->GetDefualtItemID();
    return id && (id == p->GetObjectID() || id == p->m_dwID);
}

// EButState CTuiWgt::GetState()
// {
//     CView* pm = GetView();
//     HTUI hFocus = pm->GetFocus();
//     HTUI hOver = pm->GetClientOver();
//     TMBool bMouseDown = pm->IsMouseDown();
//     return DoGetState(hFocus, hOver, bMouseDown);
// }

// EButState CTuiWgt::DoGetState(HTUI hFocus, HTUI hOver, TMBool bMouseDown)
// {
//     if (!IsWindowEnabled())
//         return e_bsDisable | e_bstDisable;
// 
//     EButState eState;
//     const CTuiWgt* pthis = this;
//     if (bMouseDown && pthis == hOver)
//         eState = e_bsDown | e_bstDown | e_bstHigh;
//     else if (pthis == hOver)
//         eState = e_bsHigh | e_bstHigh;
//     else if (this == hFocus)
//     {
//         if (_CTuiWgt_IsDefaultItem(this))
//             eState = e_bsFocus | e_bstDefault;
//         else
//             eState = e_bsNormal;
//     }
//     else
//     {
//         DbgAssert(hFocus != this);
//         if (hFocus && (hFocus->DoGetState(hFocus, hOver, bMouseDown) & e_bstDefault))
//             eState = e_bsNormal | e_bstNormal;
//         else if (_CTuiWgt_IsDefaultItem(this))
//             eState = e_bsNormal | e_bstDefault;
//         else
//             eState = e_bsNormal | e_bstNormal;
//     }
// 
//     if (this == hFocus)
//     {
//         eState |= e_bstFocus;
//         if (IsHideFocus())
//             eState |= e_bstHideFous;
//     }
// 
//     if (((eState & (e_bstDefault | e_bsMask)) == e_bstDefault) ||
//         ((eState & (e_bstFocus | e_bstHideFous)) == e_bstFocus))
//     {
//         eState &= tm_not(e_bsMask);
//         eState |= e_bsFocus;
//     }
// 
//     return eState;
// }

EButStates CTuiWgt::DoGetStates(HTUI hFocus, HTUI hOver, TMBool bMouseDown)
{
    if (!IsWindowEnabled())
        return e_bssDisable;
    
    EButStates eState;
    const CTuiWgt* pthis = this;
    if (bMouseDown && pthis == hOver)
        eState = e_bssDown | e_bssHigh;
    else if (pthis == hOver)
        eState = e_bssHigh;
    else
        eState = e_bssNormal;
    if (this == hFocus)
    {
        eState |= e_bssFocus;
        if (_CTuiWgt_IsDefaultItem(this))
            eState |= e_bssDefault;
        if (IsHideFocus())
            eState |= e_bssHideFocus;
    }
    else
    {
        DbgAssert(hFocus != this);
        if (hFocus && (hFocus->DoGetStates(hFocus, hOver, bMouseDown) & e_bssDefault))
            ; // taken away by other focus.
        else if (_CTuiWgt_IsDefaultItem(this))
            eState |= e_bssDefault;
    }

    return eState;
}

EButStates CTuiWgt::GetStates()
{
    CView* pm = GetView();
    HTUI hFocus = pm->GetFocus();
    HTUI hOver = pm->GetClientOver();
    TMBool bMouseDown = pm->IsMouseDown();
    return DoGetStates(hFocus, hOver, bMouseDown);
}

template <TMBool bPrintClient>
struct _CTuiWgt_SendRouteMessage
{
    static TMResult done(HTUI p, CTuiMsg& msg, TMBool bBindWnd, TMBool& bHandled)
    {
        return p->m_procMgrWgt.SendProcMessage(
            p, CM_MSGROUTE, (TMWParam)&bHandled, (TMLParam)&msg,
            (p->Is_WS_PRI_IS_WINDOW() || p->Is_WS_PRI_IS_SCROLLBOX() || bBindWnd) ? 0 : 1); // normal window proc needn't CM_MSGROUTE
    }

    static TMBool done(CTuiMsg& msg, TMResult& lr) // return: bHandled
    {
        TMBool bBindWnd = false;
        for (CTuiWgt* p = msg.hWnd; p;)
        {
            if (p->Is_WS_PRI_IS_WINDOW() || p->Is_WS_PRI_IS_SCROLLBOX() || (p->m_dwAdvStyle & WS_ADV_ROUTER) || bBindWnd)
            {
                CAutoMsgAnalysis ama("route", msg.hWnd->GetObjectID(), msg.message, msg.wParam);
                Ref ref2(p);
                TMBool bHandled = false;
//                 TMResult hr = p->m_procMgrWgt.SendProcMessage(
//                     p, CM_MSGROUTE, (TMWParam)&bHandled, (TMLParam)&msg,
//                     (p->Is_WS_PRI_IS_WINDOW() || p->Is_WS_PRI_IS_SCROLLBOX() || bBindWnd) ? 0 : 1); // normal window proc needn't CM_MSGROUTE
                TMResult hr = done(p, msg, bBindWnd, bHandled);
                if (bPrintClient && (msg.message == WM_PRINTCLIENT) && (hr || bHandled))
                {
                    msg.hWnd->m_hRoutePrintClient = p;
                    return lr = hr, bHandled;
                }
                if (bHandled)
                    return lr = hr, true;

                bBindWnd = p->Is_WS_PRI_BINDMODE();
                p = p->GetParent();
            }
            else
            {
                bBindWnd = p->Is_WS_PRI_BINDMODE();
                p = p->GetParent();
            }
            
        }
        return false;
    }
};

// Message Functions
TMResult CTuiWgt::SendMessage(TMUInt uMsg, TMWParam wParam, TMLParam lParam, CTuiWgt::ESendMsg eFlag)
{
    DbgAssert(IC_IsValidInstance(true));

    if ((uMsg == CM_ENDMENUPOP) && Edit_IsKeepMenuPop(Edit_IsObjectInSpyMode(this)))
        return 0;

    if (IsLifeDestroyed())
    {
//         DbgAssert(!_parent);
        DbgAssert(!_next);
        DbgAssert(!_prev);
        DbgAssert(!_prev);
        DbgAssert(!m_procMgrWgt.GetSize());
        if (uMsg == WM_DESTROY || uMsg == WM_NCDESTROY)
        {
            TM_IF_DBG(*(TMBool*)lParam = true);
        }
        return 0;
    }

    Ref ref(this);
    LOG_MSG_FOR_SPY(0, this, uMsg, wParam, lParam);

    g_menuPop.ProcessMessageWgt(this, uMsg, wParam, lParam);

    CTuiMsg wm(this, uMsg, wParam, lParam);
    if (eFlag & eSendMsgPre)
    {
        CAutoMsgAnalysis ama("premsg", GetObjectID(), uMsg, wParam);
        if (GetView()->DoPreTranslateMessage(&wm, __msgLog.lr))
            return __msgLog.lr;
    }

    if (eFlag & eSendMsgRoute)
    {
        if (_CTuiWgt_SendRouteMessage<0>::done(wm, __msgLog.lr))
            return __msgLog.lr;
    }

    {
        CAutoMsgAnalysis ama("send", GetObjectID(), uMsg, wParam);
        if (IsLifeDestroyed())
            DbgAssert(m_procMgrWgt.GetSize() == 0);
        return __msgLog.lr = m_procMgrWgt.SendProcMessage(this, wm.message, wm.wParam, wm.lParam);
    }
}

TMResult CTuiWgt::SendPrintClientMessage(TMLParam lParam)
{
#   define HTUI_ROUTE_PRINTCLIENT_NONE     ((HTUI)-1)

    IC_CheckInstance();

    LOG_MSG_FOR_SPY(0, this, WM_PRINTCLIENT, 0, lParam);

    if (!m_hRoutePrintClient)
    {
        m_hRoutePrintClient = HTUI_ROUTE_PRINTCLIENT_NONE;
        CAutoMsgAnalysis ama("route", GetObjectID(), WM_PRINTCLIENT, 0);
        TMResult lr = 0;
        CTuiMsg wm(this, WM_PRINTCLIENT, 0, lParam);
        _CTuiWgt_SendRouteMessage<WM_PRINTCLIENT>::done(wm, lr);
        if (lr)
            return 0;
    }
    else if (m_hRoutePrintClient != HTUI_ROUTE_PRINTCLIENT_NONE)
    {
        CAutoMsgAnalysis ama("route", GetObjectID(), WM_PRINTCLIENT, 0);
        CTuiMsg wm(this, WM_PRINTCLIENT, 0, lParam);
        TMBool bHandled;
        TMResult lr = _CTuiWgt_SendRouteMessage<0>::done(m_hRoutePrintClient, wm, false, bHandled);
        if (lr)
            return 0;
    }

    {
        CAutoMsgAnalysis ama("send", GetObjectID(), WM_PRINTCLIENT, 0);
        return m_procMgrWgt.SendProcMessage(this, WM_PRINTCLIENT, 0, lParam);
    }
}

struct MSG_POST_CALLBACK
{
    HTUI h; PFNPostCallback wp; TMLParam lp;
    
    MSG_POST_CALLBACK(HTUI _h, PFNPostCallback _w, TMLParam _l) : h(_h), wp(_w), lp(_l) {}
    void AddRef() { h->AddRef(); }
    void Release() { h->Release(); delete this; }
    
    static void Done(MSG_POST_CALLBACK* pMsg)
    {
        DbgAssert(pMsg);
        pMsg->wp(pMsg->h, pMsg->lp);
    }
};

TMBool CTuiWgt::PostCallback(PFNPostCallback wParam, TMLParam lParam)
{
    MSG_POST_CALLBACK* pMSg = new MSG_POST_CALLBACK(this, wParam, lParam);
    return CWidgetCallT<MSG_POST_CALLBACK, MSG_POST_CALLBACK*>::Call(pMSg);
}

struct MSG_POST
{
    HTUI h; TMUInt msg; TMWParam wp; TMLParam lp;

    MSG_POST(HTUI _h, TMUInt _m, TMWParam _w, TMLParam _l) : h(_h), msg(_m), wp(_w), lp(_l) {}
    void AddRef() { h->AddRef(); }
    void Release() { h->Release(); delete this; }

    static void Done(MSG_POST* pMsg)
    {
        DbgAssert(pMsg);
        pMsg->h->SendMessage(pMsg->msg, pMsg->wp, pMsg->lp);
    }
};

TMBool CTuiWgt::PostMessage(TMUInt message, TMWParam wParam, TMLParam lParam)
{
    MSG_POST* pMSg = new MSG_POST(this, message, wParam, lParam);
    return CWidgetCallT<MSG_POST, MSG_POST*>::Call(pMSg);
}

TMResult CTuiWgt::Notify(int id, TMUInt16 uCode, TMBool bPost, HTUI hSender)
{
    if (!(m_dwPriStyle & (WS_PRI_NONOTIFY))) // | WS_PRI_DESTROYED)))
    {
        TMWParam wp = MAKEWPARAM(id, uCode);
        if (bPost)
            return PostMessage(CM_PRECOMMAND, wp, (TMLParam)hSender);
        else
            return SendMessage(CM_PRECOMMAND, wp, (TMLParam)hSender);
    }
    return 0;
}


// TMBool CTuiWgt::EnableNotify(TMBool bEnable)
// {
//     TMBool b = !(m_dwAdvStyle & WS_PRI_NONOTIFY);
//     if (bEnable)
//         m_dwAdvStyle &= ~WS_PRI_NONOTIFY;
//     else
//         m_dwAdvStyle |= WS_PRI_NONOTIFY;
//     return b;
// }

TMBool CALLBACK _CTuiWgt_SendMessage(HTUI hWgt, TMLParam lParam)
{
    TMMSG* p = (TMMSG*)lParam;
    hWgt->SendMessage(p->message, p->wParam, p->lParam);
    return true;
}

TMBool CTuiWgt::SendBroadcastMessage(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool bSendSelf, size_t nLevel)
{
    TMMSG msg;
    msg.message = uMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;
    return EnumAllChildWindows(_CTuiWgt_SendMessage, (TMLParam)&msg, bSendSelf, nLevel);
}

void CTuiWgt::CheckParentFrameChange(TMUInt uFlag)
{
    // flow mode and border mode need change parent frame
    struct CheckFrame
    {
        inline static TMBool Next(TMULong dwLayout)
        {
            return (dwLayout & LAYOUT_MODE_MASK) == LAYOUT_FLOW;
    //                || (dwLayout & ROW_HMASK) == ROW_LEFT
    //                || (dwLayout & ROW_HMASK) == ROW_FRAME
    //                || (dwLayout & COL_VMASK) == COL_TOP
    //                || (dwLayout & COL_VMASK) == COL_VFRAME;
        }

        inline static TMBool Prev(TMULong dwLayout)
        {
            return false;
    //             return (dwLayout & ROW_HMASK) == ROW_RIGHT
    //                 || (dwLayout & ROW_HMASK) == ROW_FRAME
    //                 || (dwLayout & COL_VMASK) == COL_BOTTOM
    //                 || (dwLayout & COL_VMASK) == COL_VFRAME;
        }
    };

    if (_parent && _parent->Is_WS_PRI_IS_SCROLLBOX() && (_parent->GetChild() == this) && !(uFlag & SWP_NOSIZE))
    {
        (static_cast<CTuiScrollBoxImpl*>(GetParent()))->Update();
    }
    else if (!Is_WS_PRI_BINDMODE() && _parent
        && !(uFlag & SWP_NOLAYOUTSIBLING)
        && (((m_dwLayout & LAYOUT_MODE_MASK) == LAYOUT_ALIGN)
//             || ((m_dwLayout & TABLE_MASK) == TABLE_ROW)
            || (((m_dwLayout & LAYOUT_MODE_MASK) == LAYOUT_FLOW) && !_prev)
//             || (m_dwLayout & (ALIGN_AUTO_PARENT))
//            || (m_dwLayout & (LAYOUT_SCROLLPARENTLEFT | LAYOUT_SCROLLPARENTRIGHT | LAYOUT_SCROLLPARENTTOP | LAYOUT_SCROLLPARENTBOTTOM))
            || (_next && CheckFrame::Next(GetNext()->m_dwLayout))
            || (_prev && CheckFrame::Prev(GetPrev()->m_dwLayout)))
        && (!(uFlag & SWP_NOSIZE) || !(uFlag & SWP_NOMOVE)
            || (SWP_SHOWWINDOW & uFlag) || (SWP_HIDEWINDOW & uFlag)
            || !(SWP_NOZORDER & uFlag)))
    {
//         if (_parent->IsScrollBox() && _parent->GetChild() == this)
//             (static_cast<CTuiScrollBoxImpl*>(_parent))->Update();
//         else
            _parent->ChangeFrame (false);
    }
};

void CTuiWgt::GetRealFont(GXFont& ft)
{
    ft.Reset();
    for (HTUI hWgt = this; ; hWgt = hWgt->GetParent())
    {
        if (hWgt)
        {
            ft.SupplementFrom(hWgt->m_font);
            if (!ft._namempty() && ft.height && ft.styleMask == GX_FONT_MASK_STYLE)
                break;
        }
        else
        {
            GXFont f;
            gxFontGetDefault(f);
            ft.SupplementFrom(f);
            break;
        }
    }
}

void CTuiWgt::DoSetWindowPos(CTuiWidgetPos& wp, FRAMEINFO& fi)
{
    TMBool bLayout = (m_dwPriStyle & WS_PRI_FRAMECHANGED);
    if (bLayout)
        wp.flags |= SWP_FRAMECHANGED;
    m_dwPriStyle &= ~WS_PRI_FRAMECHANGED;

    wp.ResetFlag(m_rcWnd);
    wp.ResetVisible(m_dwStyle);

//     if (m_dwPriStyle & WS_PRI_FRAMECHANGED)
//     {
//         nFlags.SetChangeFrame();
//         m_dwPriStyle &= ~WS_PRI_FRAMECHANGED;
//     }
    
    // disable child call parent "changeframe"
//    m_dwPriStyle |= WS_PRI_FRAMECHANGED;
//    TMBool bInvaliated = (nFlags & SWP_NOREDRAW);
//     TMBool bChangeFrame = (m_dwPriStyle & WS_PRI_FRAMECHANGED) || nFlags.IsChangeFrame();


    CTuiRect rcOldWin = GetBoundRect();
    CTuiRect rcOldP;
    GetPaddingRect(&rcOldP);

    // before processing, invalidate
    if (wp.IsRedraw() && (wp.IsMove() || wp.IsSize() || wp.IsHide() || wp.IsZOrder()))
    {
#ifdef TM_DBG
//         if (!IsLifeDestroyed() && GetOwner()->IsPainting())
//             DbgTrace("CTuiWgt::SetWindowPos(rect:%f,%f,%f,%f; scroll:%f,%f; flag:0X%X; oid:%d) on painting",
//                 wp.x, wp.y, wp.x + wp.cx, wp.y + wp.cy, m_nScrollW, m_nScrollH, wp.flags, GetObjectID());
#endif
        Invalidate();
    }

    // processing
    if (wp.IsZOrder() && !NodeInsertAfter(wp.pWndInsertAfter))
        wp.ResetZOrder();

    if (wp.IsHide())
    {
        m_dwStyle &= (~WS_VISIBLE);
        SendMessage(WM_SHOWWINDOW, false, 0);
    }

    if (wp.IsShow())
    {
        m_dwStyle |= WS_VISIBLE;
        SendMessage(WM_SHOWWINDOW, true, 0);
    }

    // change frame
    if (wp.IsChangeFrame() || wp.IsSize())
    {
        BORDER border;
        GetFrameAndPaddingBorder(border);
        
        m_rcWnd.SetLTWH(wp.x, wp.y, wp.cx, wp.cy);
        wp.SetChangeFrame();
        m_rcClient = m_rcWnd;
        fi.AdjustBorder(m_rcClient);
        fi.ResetScrollFlag(this);
//         CTuiWidget(this).ScrollTo(m_nScrollL, m_nScrollT);
        FixScrollOrg();
        BORDER border2;
        GetFrameAndPaddingBorder(border2);
        if (border == border2 && (!bLayout))
            wp.ResetChangeFrame();
        else
            wp.SetChangeFrame();
    }
    else if (wp.IsMove())
    {
        fi._Dbg_CheckFrame(m_rcWnd, m_rcClient);
        m_rcWnd.SetLTWH(wp.x, wp.y, wp.cx, wp.cy);
        m_rcClient = m_rcWnd;
        fi.AdjustClientRect(m_dwLayout, m_rcClient);
    }

    // after processing, invalidate
    if (wp.IsRedraw() && (wp.IsMove() || wp.IsSize() || wp.IsShow() || wp.IsZOrder()))
        Invalidate();

    // layout
    if (wp.IsLayOut() && (bLayout || wp.IsMove() || wp.IsSize() || wp.IsChangeFrame() || wp.IsShow()))
    {
        wp.SetLayOut();
        if (GetChild())
        {
            CTuiRect rcNewP;
            GetPaddingRect(&rcNewP);
            LayoutChilds(this, rcOldP, rcNewP);
        }
    }
    else
        wp.ResetLayOut();

    // change frame if layout mode needed.
    CheckParentFrameChange(wp.flags);
    
    enum { eFlag1 = SWP_FRAMECHANGED | SWP_HIDEWINDOW | SWP_SHOWWINDOW };
    enum { eFlag2 = SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE };
    if ((eFlag2 != ((eFlag1 | eFlag2) & wp.flags)) || bLayout)
    {
//         TUIWINDOWPOS wp = { this, pInsertAfter,
//             GetLeft(), GetTop(), GetWidth(), GetHeight(), nFlags };
        SendMessage(CM_WINDOWPOSCHANGED, (TMLParam)&wp, (TMWParam)&wp);
    }

}


TMBool CTuiWgt::SetWindowPos(CTuiWgt* pInsertAfter,
                             GXDips x, GXDips y, GXDips cx, GXDips cy, TMUInt _nFlags)
{
    IC_CheckInstance();
    if (IsLifeInvalid())
        return false;

    Ref ref(this);

    // get frame info
    FRAMEINFO fi;
    CFrame* pf = GetFrame();
    if (pf)
        pf->GetFrameInfo(this, fi, m_dwStyle, m_dwAdvStyle);

    // pre-calc for autosize, minmax width,height.
    TUIWINDOWPOS wp = { this, pInsertAfter, x, y, cx, cy, _nFlags };
    CTuiWidgetPos& cwp = static_cast<CTuiWidgetPos&>(wp);
    cwp.ReplaceFromRect(m_rcWnd);
    if (!(_nFlags & SWP_NOSENDCHANGING))
    {
        PreCalcWindowPos(cwp, fi, CTuiRect(m_nMinWidth, m_nMinHeight, m_nMaxWidth, m_nMaxHeight));
        if (Is_WS_PRI_IS_WINDOW())
            cwp.x = cwp.y = 0;
        SendMessage(CM_PRI_POSCHANGING, 0, (TMLParam)&wp);
    }

    TMBool bNoRedraw = !cwp.IsRedraw() && !IsFlagRedraw();
    if (bNoRedraw)
        ResetFlagRedraw();
    DoSetWindowPos(cwp, fi);
    if (bNoRedraw)
        SetFlagRedraw();

    return !IsLifeDestroyed();
} 

// void CTuiWgt::PaddingRectToClientRect(CTuiRect& rc)
// {
//     rc.InflateRect(m_bdPadding.left, m_bdPadding.top, m_bdPadding.right, m_bdPadding.bottom);
// }
// 
// void CTuiWgt::ClientRectToWindowRect(CTuiRect& rc)
// {
//     rc.InflateRect(
//         m_rcClient.Left() - m_rcWnd.Left(),
//         m_rcClient.Top() - m_rcWnd.Top(),
//         m_rcWnd.Right() - m_rcClient.Right(),
//         m_rcWnd.Bottom() - m_rcClient.Bottom());
// }

void CTuiWgt::WindowRectToMarginRect(CTuiRect& rc)
{
    rc.InflateRect(m_bdMargin.left, m_bdMargin.top, m_bdMargin.right, m_bdMargin.bottom);
}

void CTuiWgt::ClientRectToPaddingRect(CTuiRect& rc)
{
    rc.InflateRect(-m_bdPadding.left, -m_bdPadding.top, -m_bdPadding.right, -m_bdPadding.bottom);
}

// void CTuiWgt::WindowRectToClientRect(CTuiRect& rc)
// {
//     rc.InflateRect(
//         m_rcWnd.Left() - m_rcClient.Left(),
//         m_rcWnd.Top() - m_rcClient.Top(),
//         m_rcClient.Right() - m_rcWnd.Right(),
//         m_rcClient.Bottom() - m_rcWnd.Bottom());
// }

void CTuiWgt::MarginRectToWindowRect(CTuiRect& rc)
{
    rc.InflateRect(-m_bdMargin.left, -m_bdMargin.top, -m_bdMargin.right, -m_bdMargin.bottom);
}

// void CTuiWgt::DeltaSize (GXDips dx, GXDips dy, CTuiRect& rc)
// {
//     LayoutDeltaSize(m_dwLayout, rc, dx, dy);
// }

static int CALLBACK _CTuiWgt_PFNWWCOMPARE(HTUI hWnd1, HTUI hWnd2, TMLParam lParamSort)
{
    return hWnd1->GetWindowText() < hWnd2->GetWindowText();
}

TMBool CTuiWgt::SortChild(PFNWWCOMPARE pfn, TMLParam lParam, int nLevel, TMBool bReversed)
{
    if (!pfn)
        pfn = _CTuiWgt_PFNWWCOMPARE;
    CItem(this).SortChildEx(pfn, lParam, nLevel, bReversed);
    ChangeFrame(false);
    return true;
}

TMBool CTuiWgt::ChangeFrame(TMBool bUpdate)
{
    if (bUpdate)
    {
        return SetWindowPos (0, 0, 0, 0, 0,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
    }
    else if (!(m_dwPriStyle & WS_PRI_FRAMECHANGED))
    {
        Invalidate();
        m_dwPriStyle |= WS_PRI_FRAMECHANGED;
    }
    return true;
}

void CTuiWgt::PostDirty()
{
    if (!Is_WS_PRI_DIRTY())
    {
        Set_WS_PRI_DIRTY();
        PostMessage(CM_PRI_DIRTY, 0, 0);
    }
}

TMBool CTuiWgt::SetDirtyAndInvalidate()
{
    //         DbgTrace("CTuiWgt::SetDirty(%d)", GetObjectID());
    if (!Is_WS_PRI_DIRTY())
    {
        Set_WS_PRI_DIRTY();
        return Invalidate();
    }
    return true;
}

TMBool CTuiWgt::GetClientRect(TUIRect* lpRect) const
{
    *lpRect = m_rcClient;
    return true;
}

template <class T, class P>
static void _DPtoLP(int nZoom, const T& pt, P& out, int dxWin, int dyWin)
{
    CTuiZoom zoom(nZoom);
    out.SetPoint(zoom.DPtoLP(pt.X() - dxWin), zoom.DPtoLP(pt.Y() - dyWin));
}

template <class T, class P>
static void _LPtoDP(int nZoom, const T& pt, P& out, int dxWin, int dyWin)
{
    CTuiZoom zoom(nZoom);
    out.SetPoint(zoom.LPtoDP(pt.X()) + dxWin, zoom.LPtoDP(pt.Y()) + dyWin);
}

template <class T, class P>
static void _DPtoLPRTL(int nZoom, const T& pt, P& out, int dxWin, int dyWin)
{
    CTuiZoom zoom(nZoom);
    out.SetPoint(zoom.DPtoLP(dxWin - pt.X()), zoom.DPtoLP(pt.Y() - dyWin));
}

template <class T, class P>
static void _LPtoDPRTL(int nZoom, const T& pt, P& out, int dxWin, int dyWin)
{
    CTuiZoom zoom(nZoom);
    out.SetPoint(dxWin - zoom.LPtoDP(pt.X()), zoom.LPtoDP(pt.Y()) + dyWin);
}

template <class T, class P>
static void _DPtoLP(const CTuiWgt* hWgt, const T* lpPoint, int nPt, P* out, int dx, int dy)
{
    int zoom = hWgt->GetDPI();
    if (hWgt->GetModule()->GetLocal().IsRTL())
    {
        for (int i = 0; i < nPt; ++i)
            _DPtoLPRTL(zoom, lpPoint[i], out[i], dx, dy);
    }
    else
    {
        for (int i = 0; i < nPt; ++i)
            _DPtoLP(zoom, lpPoint[i], out[i], dx, dy);
    }
}

template <class T, class P>
static void _LPtoDP(const CTuiWgt* hWgt, const T* lpPoint, int nPt, P* out, GXDips dx, GXDips dy)
{
    int zoom = hWgt->GetDPI();
    if (hWgt->GetModule()->GetLocal().IsRTL())
    {
        for (int i = 0; i < nPt; ++i)
            _LPtoDPRTL(zoom, lpPoint[i], out[i], dx, dy);
    }
    else
    {
        for (int i = 0; i < nPt; ++i)
            _LPtoDP(zoom, lpPoint[i], out[i], dx, dy);
    }
}

// void CTuiWgt::ClientToScreen(CPointI* lpPoint, int nPt) const
// {
//     CTuiPoint pt = GetView()->InternalGetScreenOrg(const_cast<HTUI>(this));
//     _LPtoDP(this, lpPoint, nPt, lpPoint, pt.X(), pt.Y());
// }
// 
// void CTuiWgt::ScreenToClient(CPointI* lpPoint, int nPt) const
// {
//     CTuiPoint pt = GetView()->InternalGetScreenOrg(const_cast<HTUI>(this));
//     _DPtoLP(this, lpPoint, nPt, lpPoint, pt.X(), pt.Y());
// }

void CTuiWgt::ClientToScreen(const TUIPoint* lpClient, int nPt, TMPointI* lpScreen) const
{
    CPoint pt = GetView()->InternalGetScreenOrg(const_cast<HTUI>(this));
    _LPtoDP(this, static_cast<const CTuiPoint*>(lpClient), nPt, static_cast<CPointI*>(lpScreen), pt.X(), pt.Y());
}

void CTuiWgt::ScreenToClient(const TMPointI* lpScreen, int nPt, TUIPoint* lpClient) const
{
    CPoint pt = GetView()->InternalGetScreenOrg(const_cast<HTUI>(this));
    _DPtoLP(this, static_cast<const CPointI*>(lpScreen), nPt, static_cast<CTuiPoint*>(lpClient), pt.X(), pt.Y());
}


TMBool CTuiWgt::ScrollToVisible(const CTuiRect& rc, TMBool bLeftRight, TMBool bTopBottom)
{
    CTuiRect rcp;
    GetPaddingRect(&rcp);
    GXDips l = GetScrollLeft(), t = GetScrollTop();
    GXDips eps = GetEps();
    
    if (bLeftRight)
    {
        GXDips w = rcp.Width();
        if (rc.Left() < rcp.Left())
            l += rc.Left() - rcp.Left() - w / 3;//, GetEps();
        else if (rc.Right() > rcp.Right())
            l += rc.Left() - rcp.Right() + w / 3;//, GetEps();
    }
    
    if (bTopBottom)
    {
        if (rc.Top() < rcp.Top() || rc.Height() > rcp.Height())
            t += rc.Top() - rcp.Top();
        else if (rc.Bottom() > rcp.Bottom())
            t += rc.Bottom() - rcp.Bottom();
    }

    DISABLE_SPY();
    return this->SendScrollToMessage(&l, &t);
}

TMBool CTuiWgt::UpdateWindow()
{
    CTuiWnd* pMgr = GetOwner();
    if (pMgr)
        pMgr->MgrUpdateWindow();
    return true;
}

TMBool CTuiWgt::IsHideFocus() const
{
    DbgAssert(_parent);
    return _parent->IsHideFocus();
}

TMBool CTuiWgt::NativeInvalidateRect(SPY_INVALIDATE_PARAM(const TUIRect& rc, TM::HTUI hSender))
{
    // Invalidate needn't clip sibling windows, because they maybe transparent.
    // Is_WS_PRI_FRAMECHANGED means have been invalidated.
    if (!IsWindowVisible() || !IsFlagRedraw())// || Is_WS_PRI_FRAMECHANGED())
    {
        //DbgTrace("CTuiWgt(%p-%d)::InvalidateRect() invisible", this, this->GetObjectID());
        return false;
    }

    // clip self rect
    CTuiRect rcCur(m_rcWnd);
    rcCur &= rc;
    if (rcCur.IsRectEmpty())
        return false;

    if (m_pBind)
    {
        CTuiRect rci;
        m_pBind->GetRectForInvalidate(this, rci);
        rcCur &= rci;
        if (rcCur.IsRectEmpty())
            return false;
    }

    if (_parent)
        return _parent->NativeInvalidateRect(SPY_INVALIDATE_PARAM(rcCur, hSender));
    
    return true;
}

TMBool CTuiWgt::EnableWindow (TMBool bEnable)
{
    // check same state
    if ((0 != bEnable) == !(m_dwStyle & WS_DISABLED))
        return false;

    if (bEnable)
        m_dwStyle &= (~WS_DISABLED);
    else
        m_dwStyle |= WS_DISABLED;
    
    Ref ref(this);
    Invalidate();
    SendMessage(WM_ENABLE, bEnable, 0);
    SendBroadcastMessage(CM_ENABLE_P, bEnable, 0, false);

    return true;
}

TMBool CTuiWgt::CanInput() const
{
    for (const CTuiWgt* p = this; p; p = p->GetParent())
    {
        if ((p->m_dwStyle & WS_DISABLED) || !(p->m_dwStyle & WS_VISIBLE) || p->IsLifeInvalid())
        {
            return false;
        }
    }
    return true;
}

TMBool CTuiWgt::IsAllVisible(TMBool bCheckRealVisible, TMBool bCheckRealMinimize)
{
    HTUI p = this;
    for (; ;)
    {
        if (!p->Is_WS_VISIBLE())
            return false;
        if (p->Is_WS_PRI_IS_WINDOW())
            break;
        p = p->GetParent();
        if (!p)
            return true;
    }

    if (bCheckRealVisible || bCheckRealMinimize)
    {
        TMUInt32 dwStyle = 0;
        TMUInt32 dwMask = 0;
        if (bCheckRealVisible)
            dwStyle = dwMask = WS_VISIBLE;
        if (bCheckRealMinimize)
            dwMask |= WS_MINIMIZE;
        CTuiWnd* mgr = static_cast<CTuiWnd*>(p);
        return mgr->MgrIsAllStyle(dwStyle, dwMask);
    }

    return true;
}

TMBool CTuiWgt::IsAllEnable(TMBool bCheckReal) const
{
    HTUI p = const_cast<HTUI>(this);
    for (; ;)
    {
        if (!p->IsWindowEnabled())
            return false;
        if (p->Is_WS_PRI_IS_WINDOW())
            break;
        p = p->GetParent();
        if (!p)
            return true;
    }

    if (bCheckReal)
    {
        CTuiWnd* mgr = static_cast<CTuiWnd*>(p);
        return mgr->MgrIsAllStyle(0, WS_DISABLED);
    }
    return true;
}

TMBool CTuiWgt::SetCapture ()
{
    GetView()->SetCapture(this);
    return true;
//     CTuiWnd* pMgr = GetOwner();
//     return pMgr->SetCapture (this);
}

TMBool CTuiWgt::IsCapture () const
{
    return this == GetView()->GetCapture();
}

TMBool CTuiWgt::SetFocus(EFocusReason reason, TMBool bActive)
{
    if (reason == eFocusReasonMouse)
        GetOwner()->MgrSetHideFocus();
    return GetView()->CheckSetFocus(this, true, bActive, reason);
}

TMBool CTuiWgt::IsFocus () const
{
    return this == GetView()->GetFocus();
}

void CTuiWgt::SetNextFocus(TMBool bReverse, EFocusReason reason, TMBool bActive)
{
//     GetView()->SetNextFocus(bReverse);
    DISABLE_SPY();
    HTUI hWnd = CTuiWidget(this).GetNextDlgTabItem(bReverse);
    if (hWnd)
        hWnd->SetFocus(reason, bActive);
}

TMBool CTuiWgt::IsActive() const
{
    return GetView()->InternalIsWindowActive(this);
}


// CTuiWgt* CTuiWgt::ChildWindowFromPoint (CTuiPoint point, TMUInt32 dwStyle, TMUInt32 dwStyleMask, TMUInt32 dwAdvStyle, TMUInt32 dwAdvMask) const
// {
//     CTuiWgt* p = const_cast<CTuiWgt*>(this);
//     return CWidgetHelper::GetMouseTarget2(p, point, dwStyle, dwStyleMask, dwAdvStyle, dwAdvMask);
// }


// CTuiWgt* CTuiWgt::GetTopWindow()
// {
//     return GetChild();
// }

CTuiWgt* CTuiWgt::GetWindow(TMUInt nCmd)
{
    switch (nCmd)
    {
    case GW_CHILD:
        return GetChild();

    case GW_HWNDFIRST:
        return CItem(this).GetFirst();

    case GW_HWNDLAST:
        return CItem(this).GetLast();

    case GW_HWNDNEXT: // bottom direction 
        return GetNext();

    case GW_HWNDPREV: // top direction
        return GetPrev();

    case GW_OWNER:
        return GetOwner ();

    case GW_GLOBALNEXT:
        { CItem itm(this); itm.GlobalNext(); return itm; }

    case GW_GLOBALPREV:
        { CItem itm(this); itm.GlobalPrev(); return itm; }

    case GW_TABNEXT:
        return GetView()->GetNextTab(this, false);
        
    case GW_TABPREV:
        return GetView()->GetNextTab(this, true);
    }

    return 0;
}

TMBool CTuiWgt::IsChild(const CTuiWgt* pChild) const
{
    while (pChild)
    {
        pChild = pChild->GetParent ();
        if (this == pChild)
            return true;
    }

    return false;
}

TMHWnd CTuiWgt::GetHWND() const
{
    CTuiWnd* p = GetOwner();
    return p ? p->MgrGetHWND() : 0;
}

TMHWnd CTuiWgt::GetOverlayHWND()
{
    if (_parent)
        return _parent->GetOverlayHWND();
    DbgAssert(0);
    return 0;
}

void CTuiWgt::Reclaim()
{
    m_gxText.Reclaim();
}

// void CTuiWgt::GetWalkerItemNext(CWalkerItem& itm)
// {
//     DbgAssert(!itm.m_parent);
//     DbgAssert(!itm.m_hIter);
//     itm.m_hIter = itm.m_hIter->_next;
// }
// 
// void CTuiWgt::GetWalkerItemPrev(CWalkerItem& itm)
// {
//     DbgAssert(!itm.m_parent);
//     DbgAssert(!itm.m_hIter);
//     itm.m_hIter = itm.m_hIter->_prev;
// }

CTuiWnd* CTuiWgt::GetOwner() const
{
    if (_parent)
        return GetParent()->GetOwner();
    if (Is_WS_PRI_IS_WINDOW())
        return static_cast<CTuiWnd*>(cthis());
    return 0;
}

CTuiWgt* CTuiWgt::SetParent(CTuiWgt* pParent)
{
    DbgAssert(!IsChild(pParent));
    DbgAssert(!IsLifeInvalid());
    DbgAssert(!Is_WS_PRI_BINDMODE());

    if (this == pParent || !pParent || IsLifeInvalid() || pParent->IsLifeInvalid() || Is_WS_PRI_BINDMODE())
        return 0;

//    TMBool bChangeFrame = Is_WS_VISIBLE();
//     if (bChangeFrame)
//         CheckParentFrameChange(SWP_HIDEWINDOW);
    CTuiWgt* pOld = GetParent();

    Ref ref1(this);
    Ref ref2(pOld);

    pOld->ChangeFrame(false);
    EnumResetRoutePrintClient();
//     NodeDetach();
    pParent->InsertChildFirst(this);
    EnumResetRoutePrintClient();

    pParent->ChangeFrame(false);

    TMBool bDPI = (m_nDPI != pParent->GetDPI());
    SendBroadcastMessage(CM_SETPARENT_P, (TMWParam)pOld, (TMLParam)this);
    if (bDPI)
        SendBroadcastMessage(CM_SETDPI, pParent->GetDPI());
//     if (bChangeFrame)
//         CheckParentFrameChange(SWP_SHOWWINDOW);

    return pOld;
}

CTuiWgt* CTuiWgt::GetDlgItem(int nID, HTUI hSkip) const
{
    for (CWndRec rec(const_cast<HTUI>(this)->GetChilds()); rec; ++rec)
    {
        HTUI p = *rec;
        DbgAssert(!p->IsLifeDestroyed());
        if (p == hSkip)
            continue;
        if (p->m_dwID == nID || p->m_dwObjID == nID)
            return p;
        p = p->GetDlgItem(nID, hSkip);
        if (p)
            return p;
    }
    return 0;
}

HTUI CTuiWgt::SearchNearestItemInDlg(int nID)
{
    DbgAssert(nID);

    HTUI hSkip = 0;
    HTUI hParent = this;
    for (; hParent; hSkip = hParent, hParent = hParent->GetParent())
    {
        if (hParent->m_dwID == nID || hParent->m_dwObjID == nID)
            return hParent;
        
        HTUI pWin = hParent->GetDlgItem(nID, hSkip);
        if (pWin)
            return pWin;
    }
    return 0;
}

// return true: continue; false:break;
TMBool _CTuiWgt_EnumChildWindows(HTUI hWgt, PFNWWENUMPROC lpEnumFunc, TMLParam lParam, TMBool bEnumSelf, size_t nLevel)
{
    DbgAssert(hWgt->m_nRef > 1);
    if (bEnumSelf && !lpEnumFunc(hWgt, lParam))
        return false;
    if (nLevel == 0)
        return true;
    for (HTUI hChild = hWgt->GetChild(); hChild;)
    {
        Ref ref(hChild);
        if (!_CTuiWgt_EnumChildWindows(hChild, lpEnumFunc, lParam, true, nLevel - 1))
            return false;
        hChild = hChild->GetNext();
    }
    return true;
}

TMBool CTuiWgt::EnumAllChildWindows(PFNWWENUMPROC lpEnumFunc, TMLParam lParam, TMBool bEnumSelf, size_t nLevel)
{
    Ref ref(this);
    _CTuiWgt_EnumChildWindows(this, lpEnumFunc, lParam, bEnumSelf, nLevel);
    return true;
}

TMBool CALLBACK _CTuiWgt_ResetRoutePrintClient(HTUI hwnd, TMLParam lParam)
{
    hwnd->m_hRoutePrintClient = 0;
    return true;
}

void CTuiWgt::EnumResetRoutePrintClient()
{
    EnumAllChildWindows(_CTuiWgt_ResetRoutePrintClient, 0, true, -1);
}

void CTuiWgt::AdjustClientRect(TUIRect& rc, TMUInt32 dwLayout)
{
    if (!m_pFrame)
        return;
    FRAMEINFO fi;
    m_pFrame->GetFrameInfo(this, fi, m_dwStyle, m_dwAdvStyle);
    fi.AdjustClientRect(dwLayout, *(CTuiRect*)&rc);
}

HTUI CTuiWgt::CreateByObjectID(CModule* pModule, TMUInt uObjID, TUIFACTORY* pFactory, void* param)
{
    if (IsLifeInvalid())
        return 0;

    int nDpi = GetOwner()->GetDPI();
    HTUI hWgt = CWinPtr::CreateNative(pModule, this, uObjID, nDpi);//CFWinPtr(pModule).CreateObject(this, uObjID);
    if (hWgt)
    {
        Ref ref(hWgt);
        hWgt->InitConstruct(pFactory, param);
        if (hWgt->IsWindowVisible())
            hWgt->CheckParentFrameChange(SWP_SHOWWINDOW);
        if (!hWgt->IsLifeDestroyed())
            return hWgt;
    }
    return 0;
}

TMBool CTuiWgt::SetNcCursor(EHitID hid)
{
    if (TUIHITGETOBJ(hid) == e_objOBorder)
    {
        CCursorAT::cursor_id nID = CCursorAT::cursor_NULL();
        switch (GetModule()->GetLocal().ConvertSZ(TUIHITGETSUBSZ(hid)))
        {
        case WMSZ_TOPLEFT:      nID = CCursorAT::cursor_SIZENWSE();    break;
        case WMSZ_TOPRIGHT:     nID = CCursorAT::cursor_SIZENESW();    break;
        case WMSZ_TOP:          nID = CCursorAT::cursor_SIZENS();      break;
        case WMSZ_BOTTOMLEFT:   nID = CCursorAT::cursor_SIZENESW();    break;
        case WMSZ_BOTTOMRIGHT:  nID = CCursorAT::cursor_SIZENWSE();    break;
        case WMSZ_BOTTOM:       nID = CCursorAT::cursor_SIZENS();      break;
        case WMSZ_LEFT:         nID = CCursorAT::cursor_SIZEWE();      break;
        case WMSZ_RIGHT:        nID = CCursorAT::cursor_SIZEWE();      break;
        default: return false;
        }
        
        OSSetCursor(nID);
        return true;
    }
    return false;
}


#ifdef TUI_EDIT_MODE

CString _CTuiWgt_format_state(HTUI hWnd)
{
    EButStates state = hWnd->GetStates();
    CString str(L"\r\n  State:");
#define FORAMT_(mask,st) if ((state & mask) == st) str += L###st L"; ";
//     FORAMT_(e_bsMask, e_bsNormal);
//     FORAMT_(e_bsMask, e_bsDown);
//     FORAMT_(e_bsMask, e_bsHigh);
//     FORAMT_(e_bsMask, e_bsDisable);
//     FORAMT_(e_bsMask, e_bsFocus);
//     FORAMT_(e_bstNormal, e_bstNormal);
    FORAMT_(e_bssDown, e_bssDown);
    FORAMT_(e_bssHigh, e_bssHigh);
    FORAMT_(e_bssDisable, e_bssDisable);
    FORAMT_(e_bssFocus, e_bssFocus);
    FORAMT_(e_bssDefault, e_bssDefault);
    FORAMT_(e_bssHideFocus, e_bssHideFocus);
    FORAMT_(e_bssHide, e_bssHide);
//    FORAMT_(e_bsEditor, e_bsEditor);
#undef FORAMT_
    return str;
}

static int _CTuiWgt_GetAllVisibleCount(HTUI h)
{
    if (!h->Is_WS_VISIBLE())
        return 0;
    int nCount = 1;
    for (h = h->GetChild(); h; h = h->GetNext())
        nCount += _CTuiWgt_GetAllVisibleCount(h);
    return nCount;
}

CString CTuiWgt::GetDebugInfo()
{
    CString str(CTuiObject::GetDebugInfo());
    _DInfo_AppendOrderUp(str);

    // rect
    CString strTmp;
    CTuiRect rcl(GetWindowRect());
    CRect rcd;
    ClientToScreen(rcl, rcd);
    strTmp.Format(L"\r\n  logic rect:" TM_L(TM_FMT_RCF_STR) L";", TM_FMT_RCF_VAL(rcl));
    str += strTmp;
    strTmp.Format(L" device rect:" TM_L(TM_FMT_RCI_STR) L";", TM_FMT_RCI_VAL(rcd));
    str += strTmp;

    // scroll
    strTmp.Format(L"\r\n  VScroll:%g-%g; HScroll:%g-%g;",
        GetScrollHeight(), GetScrollTop(), GetScrollWidth(), GetScrollLeft());
    str += strTmp;

    // state
    str += _CTuiWgt_format_state(this);

    // text format
    if (m_gxText)
    {
        TMUInt32 fmt = m_gxText->GetFormat();
        strTmp.Format(L"\r\n  text format:%x;", fmt);
        str += strTmp;
    }

    // module
    TCHAR c[1024] = {0};
    GetModuleFileName(GetModule()->m_hRes, c, tm_countof(c));
    str += L"\r\nModule:";
    str += c;

    // language
    CLocal local = GetModule()->GetLocal();
    TCHAR localBuf[256] = {0};
    local.ToString(localBuf);
    strTmp.Format(L"\r\nLanguage:%d(%s)", local, localBuf);
    str += strTmp;
    
    // tab
    {
        HTUI h1 = GetWindow(GW_TABNEXT);
        HTUI h2 = GetWindow(GW_TABPREV);
        strTmp.Format(L"\r\nNextTab:%p(%d); PrevTab:%p(%d)",
            h1, h1 ? h1->GetObjectID() : 0,
            h2, h2 ? h2->GetObjectID() : 0);
        str += strTmp;
    }

    // child
    strTmp.Format(L"\r\nchild count:%d; visible child count:%d;",
        GetSelf().GetAllChildCount(), _CTuiWgt_GetAllVisibleCount(this) - 1);
    str += strTmp;

    // real HWND
    strTmp.Format(L"\r\nHWND:0x%X; overlay HWND:0x%X", GetHWND(), GetOverlayHWND());
    str += strTmp;
    
    // subclass
    str += L"\r\nsub class functions:";
    str += _Debug_MakeDelegatesInfo(m_procMgrWgt);

    // render
    float d1 = m_fpsRend.GetAvgDTime().ToMillionSecondF();
    float d2 = m_fpsRendSelf.GetAvgDTime().ToMillionSecondF();
    strTmp.Format(L"\r\navg render self time:%gms; avg render self and child time:%gms;",
        d2, d1);
    str += strTmp;

    return str;
}

TMBool CTuiWgt::EditorMatchID(TMUInt32 n)
{
    if (n == m_pFrame.GetObjectID() || n == m_pBk.GetObjectID() || n == m_nIDHint)
        return true;
    return false;
}


#endif // TUI_EDIT_MODE

TM_END_NAMESPACE
