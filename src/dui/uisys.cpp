/*
 *  @file
 *  @brief  non client object
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.01.09
 *
 *  Copyright (C) 2019 miragekiller
 */

#include "uisys.h"


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// CTuiScrollObj
int CTuiScrollObj::CoercePos(int pos)
{
    if (m_nMin < m_nMax)
    {
        if (pos < m_nMin)
            pos = m_nMin;
        else if (pos > m_nMax)
            pos = m_nMax;
    }
    else
    {
        if (pos < m_nMax)
            pos = m_nMax;
        else if (pos > m_nMin)
            pos = m_nMin;
    }
    return pos;
}

int CTuiScrollObj::SetPos(int nNewPos, TMBool bRedraw, TMBool bSendChanging, TMUInt16 nCode)
{
    int nPos = m_nPos;
    nNewPos = CoercePos(nNewPos);

    if (bSendChanging && nPos != nNewPos)
    {
        SendMessage(CM_POSCHANGING, nPos, (TMLParam)&nNewPos);
        nNewPos = CoercePos(nNewPos);
    }

    if (nPos != nNewPos)//  && m_nMin <= nNewPos && nNewPos <= m_nMax)
    {
        m_nPos = nNewPos;
        if (bRedraw)
            Invalidate();

        CTuiWidget hWnd(GetTarget());
        if (hWnd)
        {
            CString strText;
            if (m_strFmt.IsEmpty())
                strText.Format(TM_T("%d"), m_nPos);
            else
                strText.Format(m_strFmt, m_nPos);
            hWnd.SetWindowText(strText, IsStyleHTML());
        }

        if (nCode)
            Notify(nCode);
        Notify(CN_VALUECHANGE);
    }

    return nPos;
}

HTUI CTuiScrollObj::GetTarget()
{
    CTuiWidget hWnd(m_wndBuddy);
    if (!m_wndBuddy && m_dwBuddyID)
        return SearchNearestItemInDlg(m_dwBuddyID);
    return m_wndBuddy;
}

TMResult CTuiScrollObj::OnSetPos(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    return SetPos(lParam, wParam, false, 0);
}

TMResult CTuiScrollObj::OnDeltaPos(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DISABLE_SPY();
    return CTuiProgress(this).SetPos(SyncNewPos((int)lParam), wParam); // m_nPos + (int)lParam, wParam);
}

TMResult CTuiScrollObj::OnGetRange(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (wParam)
        *(int*)wParam = m_nMin;
    if (lParam)
        *(int*)lParam = m_nMax;
    return true;
}

TMResult CTuiScrollObj::OnSetRange(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bool bChange = false;
    if (wParam && m_nMin != *(int*)wParam)
    {
        m_nMin = *(int*)wParam;
        bChange = true;
    }
    if (lParam && m_nMax != *(int*)lParam)
    {
        m_nMax = *(int*)lParam;
        bChange = true;
    }
    if (bChange)
        Invalidate();
    return true;
}

TMResult CTuiScrollObj::OnSetStep(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    int nOld = m_nStep;
    m_nStep = (int)wParam;
    return nOld;
}

TMResult CTuiScrollObj::OnStepIt(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DISABLE_SPY();
    return CTuiProgress(this).SetPos(SyncNewPos(m_nStep), wParam); // m_nPos + m_nStep, wParam);
}

TMResult CTuiScrollObj::OnSetBuddy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_wndBuddy.SubclassWindow(HTUI(wParam));
    return true;
}

TMResult CTuiScrollObj::OnSetBuddyID(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_dwBuddyID = wParam;
    return true;
}

TMResult CTuiScrollObj::OnSetFormat(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    SettingStyleHTML(((TUISETTEXT*)lParam)->bHtml);
    return StringMessageToString(wParam, m_strFmt);
}

TMResult CTuiScrollObj::OnKeyDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    wParam = GetModule()->GetLocal().ConvertKey(wParam);

    if (wParam == VK_UP)
        wParam = IsStyleSwapUpDownKey() ? VK_RIGHT : VK_LEFT;
    else if (wParam == VK_DOWN)
        wParam = IsStyleSwapUpDownKey() ? VK_LEFT : VK_RIGHT;

    if (VK_LEFT == wParam)
        SetPos(SyncNewPos(m_nStep), true, true, SB_ENDSCROLL); // m_nPos - m_nStep, true, true, SB_ENDSCROLL);
    else if (VK_RIGHT == wParam)
        SetPos(SyncNewPos(m_nStep), true, true, SB_ENDSCROLL); // m_nPos + m_nStep, true, true, SB_ENDSCROLL);
    else
        return bHandled = false;
   
    return 0;
}

TMResult CTuiScrollObj::OnLButtonDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    SetFocus(eFocusReasonMouse);
    return 0;
}


// IFrameSource
void CTuiScrollObj::FSSendMessage(TMUInt uMsg, TMWParam wp, TMLParam lp)
{
    DbgAssert(uMsg == CM_HSCROLL || uMsg == CM_VSCROLL);
    switch (wp)
    {
    case SB_LINEUP:
    case SB_PAGEUP: SetPos(SyncNewPos(-m_nStep), true, true, wp); break; // m_nPos - m_nStep, true, true, wp); break;
    case SB_LINEDOWN:
    case SB_PAGEDOWN: SetPos(SyncNewPos(m_nStep), true, true, wp); break; // m_nPos + m_nStep, true, true, wp); break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK: SetPos(m_nMin + lp, true, true, wp); break;
    default: Notify(wp); break;
    }
}



///////////////////////////////////////////////////////////////////////////////
// CTuiSysImpl
TM_REGISTER_CLASS1(CTuiSys);

CTuiSysImpl::CTuiSysImpl()
    : CTuiPushButtonImpl()
    , m_nType (eHitIDCaption)
    , m_dwTargetID (0)
{
//     m_pBk.Release();
}

HTUI CTuiSysImpl::GetTarget()
{
    HTUI hWnd = m_dwTargetID ? SearchNearestItemInDlg(m_dwTargetID) : GetParent();
    DbgAssert(hWnd);
    return hWnd;
}

ITuiPic* CTuiSysImpl::GetDftPic()
{
    switch (m_nType)
    {
    case eHitIDCaption: return GetPrePic(CAPTION);  //&GetModule()->m_aPrePic.m_picDftCaption;
    case eHitIDClose:   return GetPrePic(CLOSE);    //&GetModule()->m_aPrePic.m_picDftClose;
    case eHitIDMin:     return GetPrePic(MIN);      //&GetModule()->m_aPrePic.m_picDftMin;
    case eHitIDMax:     return GetPrePic(MAX);      //&GetModule()->m_aPrePic.m_picDftMax;
    }
    return 0;
}

TMResult CTuiSysImpl::OnPreCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (HIWORD(wParam) == BN_CLICKED)
    {
        switch (TUIHITGETOBJ(m_nType))
        {
        case e_objSys:      wParam = SC_MOUSEMENU; break;
        case e_objCaption:  wParam = SC_MOVE | HTCAPTION; break;
        case e_objClose:    wParam = SC_CLOSE; break;
        case e_objMin:      wParam = SC_MINIMIZE; break;
        case e_objMax:      wParam = GetTarget()->CanRestore() ? SC_RESTORE : SC_MAXIMIZE; break;
        case e_objOBorder:  wParam = SC_SIZE | TUIHITGETSUBSZ(m_nType); break;
        default: return 0;
        }
    }
    else if (HIWORD(wParam) == BN_DBLCLK)
    {
        switch (m_nType)
        {
        case eHitIDSys:      wParam = SC_CLOSE; break;
        case eHitIDCaption:  wParam = GetTarget()->CanRestore() ? SC_RESTORE : SC_MAXIMIZE; break;
        default: return 0;
        }
    }
    
    GetTarget()->SendMessage (WM_SYSCOMMAND, wParam, 0);
    return 0;
}

TMResult CTuiSysImpl::OnSetCursor(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (SetNcCursor(m_nType))
        return true;
    return 0;
}


TM_END_NAMESPACE


