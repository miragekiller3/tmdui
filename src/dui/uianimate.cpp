/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.02.09
 *
 *  Copyright (C) 2009 miragekiller
 */

#include "uianimate.h"



TM_BGN_NAMESPACE

CTuiAnimateImpl::CTuiAnimateImpl ()
    : m_nRepeat (-1)
    , m_nStart(0)
    , m_nEnd(-1)
{
}

// void CTuiAnimateImpl::FinalConstruct(CTuiObject* pParent)
// {
//     this->SetAni(50);
//     this->Pause();
//     CTuiWgt::FinalConstruct(pParent);
// }

TMResult CTuiAnimateImpl::OnCreate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Set_WS_PRI_DIRTY();
//     CheckFrame();
    if (!Edit_IsObjectInDocMode(this) && IsStyleAutoPlay())
        OnPlay(0, 0, 0, bHandled);
    Invalidate();
    bHandled = IsLifeDestroyed();
    return 0;
}

// TMResult CTuiAnimateImpl::OnShowWindow(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     if (m_pBk)
//         ResetDelay();
//     return 0;
// }

TMResult CTuiAnimateImpl::OnPlay(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (m_pBk)
    {
        int nFrames = m_pBk->GetFrames();
        int nDelay = m_pBk->GetDelay();
        if (nFrames && nDelay)
        {
//             if (m_tmID)
//             {
//                 OptimizeTryStart();
//                 m_ani.Resume();
//             }
//             else
                m_ani.Play(this, nDelay);
//             this->Resume();
            Notify(ACN_START);
        }
    }
    return 0;
}

// int CTuiAnimateImpl::GetCurrentFrame()
// {
//     if (!m_pBk)
//         return 0;
//     int nDelay = m_pBk->GetDelay();
//     if (!nDelay)
//         return 0;
//     int n = m_ani.GetCurrentFrame();
//     int nDelayReal = tm_max(nDelay, eMaxInterval);
//     n = n * nDelayReal / nDelay;
//     return n;
// }

// void CTuiAnimateImpl::CheckFrame()
// {
//     int n = this->GetCurrentFrame();
//     if (n < m_nStart)
//         Seek(m_nStart, true);
// }

void CTuiAnimateImpl::Stop()
{
    if (m_ani.IsStop())
        return;
    m_ani.KillAni();
    Notify(ACN_STOP);
}

int CTuiAnimateImpl::Seek(int nPos, TMBool bNotify, TMClock tmRestart)
{
    if (m_pBk)
    {
        m_ani.SetCurrentFrame(nPos, tmRestart);
        SetDirtyAndInvalidate();
        if (bNotify && IsStyleNotify())
            this->Notify(CTuiAnimate::CACN_SEEKING);
    }
    return nPos;
}

void CTuiAnimateImpl::OptimizeTryPause()
{
    if (-1 == m_nRepeat && !IsStyleNoOptimize() && m_ani.IsRun() && m_pBk && m_pBk->GetDelay())
        m_ani.Pause();
}

void CTuiAnimateImpl::OptimizeTryStart()
{
    m_ani.Resume(this);
}


// int CTuiAnimateImpl::ResetDelay()
// {
//     DbgAssert(m_pBk);
//     int nDelay = m_pBk->GetDelay();
//     if (!nDelay)
//         return 0;
//     tm_setmax(nDelay, eMaxInterval);
//     if (IsStyleNoSlowOnHide() || IsWindowVisible())
//         this->SetDelay(nDelay);
//     else
//         this->SetDelay(nDelay * 100);
//     return nDelay;
// }

TMResult CTuiAnimateImpl::OnGetRange(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (wParam)
        *(int*)wParam = m_nStart;
    if (lParam)
        *(int*)lParam = m_nEnd;
    return 0;
}

TMResult CTuiAnimateImpl::OnSetRange(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_nStart = wParam;
    m_nEnd = lParam;
    return 0;
}

TMResult CTuiAnimateImpl::OnSetRepeat(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_nRepeat = wParam;
    return 0;
}

void CTuiAnimateImpl::OnAni(TMHVni hVni, TMClock tmNow, TMClock tmVBlank)
{
    if (m_pBk)
    {
        // check frame
        int nDelay = m_pBk->GetDelay();
        int nOld = GetCurrentFrame();
        int n = m_ani.CaclFrame(tmNow + 2 * tmVBlank);
        if (nOld == n)
            return;
        int nFrames = m_pBk->GetFrames();
        if (m_nEnd >= 0 && m_nEnd < nFrames)
            nFrames = m_nEnd + 1;
        if (n < m_nStart)
            Seek(m_nStart, true, tmNow);
        else if (n >= nFrames) // check end
        {
            if (m_nRepeat)
                n = m_nStart;
            else
                n = nFrames - 1; // if repeat == 0; this ANI not reset frame to start.
            Seek(n, true, tmNow);

            // check repeat
            if (m_nRepeat == 0 || m_nRepeat == 1)
                Stop();
            else if (m_nRepeat != -1)
                --m_nRepeat;
        }
        else
            Seek(n, false);
    }
    else
        m_ani.KillAni();
}

// CTuiWgt
void CTuiAnimateImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    if (!m_pBk)
        return;

    OptimizeTryStart();

    // remove dirty
    if (Is_WS_PRI_DIRTY())
        Reset_WS_PRI_DIRTY();
        
    // repaint
    CTuiRect rc;
    GetPaddingRect(&rc);
    int nCur = GetCurrentFrame();
    m_pBk->SetCurFrame(nCur);
    m_pBk->Render(0, 0, rc, 0, gapp);
}

void CTuiAnimateImpl::OnSetLife(TMUInt32 life)
{
    if (life == WS_PRI_LIFE_DESTROYING)
        m_ani.KillAni();
    CTuiWgt::OnSetLife(life);
}

#ifdef TUI_EDIT_MODE

// void CTuiAnimateImpl::OnLoadProp(IDomElement* pXml)
// {
//     if (Edit_IsObjectInSpyMode(this) && (GetLife() == WS_PRI_LIFE_CREATE))
//     {
//         this->KillAni();
//         TMBool bHandled;
//         OnCreate(0,0,0, bHandled);
//     }
//     CTuiWgt::OnLoadProp(pXml);
// }

CString CTuiAnimateImpl::GetDebugInfo()
{
    TM::CString str;
    if (m_pBk)
        str.Format(L"\r\n\r\nframes:%d; current frame:%d", m_pBk->GetFrames(), this->GetCurrentFrame());
    return CTuiWgt::GetDebugInfo() + str;
}

#endif // TUI_EDIT_MODE

TM_REGISTER_CLASS1 (CTuiAnimate);

TM_END_NAMESPACE
