/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.01.04
 *
 *  Copyright (C) 2018 miragekiller
 */

#include "anilist.h"
#include "tmani.h"
#include "pic.h"
#include "uiwidget.h"
#include "view.h"


TM_BGN_NAMESPACE

//#define ANI_TRACE DbgTrace
#define ANI_TRACE DbgTraceNull

// ani
class _CAniImpl
{
protected:
    CVniID          m_tmID;
    TMClock         m_tmStart;
    HTUI            m_hTarget;
    CRefPtr<IEase>  m_ptrEase;

protected:
    _CAniImpl(HTUI hWnd)
        : m_hTarget(hWnd)
    {
        ANI_TRACE("_CAniImpl::_CAniImpl() this:%p", this);
    }
    
    void Start()
    {
        m_tmStart = TimeClock();
        m_tmID.SetAni(this);
    }

    ~_CAniImpl()
    {
        ANI_TRACE("_CAniImpl::~_CAniImpl() this:%p", this);
    }

public:
    static void EraseIter(CWidgetAniList::CAniList::recorder rec)
    {
        _CAniImpl* p = rec.get_data();
        rec.erase();
        p->Delete(true);
    }

    struct CDestroy
    {
        template <class T> inline void operator() (T& p) const { p.second->Delete(false); }
    };
    
#ifdef TM_DBG
    virtual void Check(HTUI hWnd, EHitID nHitID) = 0;
#else
    void Check(HTUI hWnd, EHitID nHitID) {}
#endif

    virtual void Delete(TMBool bReset) = 0;
    virtual TMBool Render(IGXApp* gapp, ITuiPic* pPic, int nState, EButStates states, const CTuiRect& rc) { return false; } // if return false then remove ani.
    virtual TMBool Render(IGXApp* gapp, ITuiPic* pPic, const SCROLLSTATE& state, const CTuiRect& rc) { return false; }

    virtual void OnAni(TMHVni hVni, TMClock tmNow, TMClock tmVBlank) = 0;

    // call on OnAni
    void EraseSelf(HTUI hWgt)
    {
        DbgAssert(hWgt);
        DbgAssert(hWgt->m_aniList.m_aAni);
        for (CWidgetAniList::CAniList::recorder rec(*hWgt->m_aniList.m_aAni); rec; ++rec)
        {
            if (this == rec.get_data())
            {
                EraseIter(rec);
                hWgt->m_aniList.CheckErase();
                return;
            }
        }
        DbgAssert(0);
    }

    TMUInt GetPassed(TMClock tmNow)
    {
        return m_tmID ? (tmNow - m_tmStart) : 0;
    }

    void Pause()
    {
        m_tmID.KillAni();
    }
};

struct CMixAni : public _CAniImpl
{
public:
    TM_SELF_ALLOC(CMixAni);

public:
    EButStates  m_stateOld;
    EButStates  m_stateNew;
    TMUInt8     m_alpha;

    void Check(HTUI hWnd, EHitID nHitID)
    {
        DbgAssert(hWnd == m_hTarget);
        DbgAssert(nHitID != eHitIDOut);
    }

public:
    CMixAni(HTUI hWnd)
        : _CAniImpl(hWnd)
        , m_alpha(255)
    {
    }

    void ResetAs(EButStates so, EButStates sn)
    {
        m_stateOld = so;
        m_stateNew = sn;
        ANI_TRACE("CMixAni:CMixAni(state:%x->%x; idObj:%d)",m_stateOld, m_stateNew, m_hTarget->GetObjectID());
        Start();
    }

    virtual void Delete(TMBool bReset) tm_override
    {
        delete this;
    }

    TMUInt8 CalcAlpha(TMClock tmNow) // int nPassedBase = 0)
    {
        TMUInt nPass = GetPassed(tmNow);
        int nalpha = 255.0f * (float)nPass / 300.0f;// powf(0.7f, (nPass - nPassedBase) / 100.0f);
//         DbgTrace("CMixAni::CalcAlpha() nPass:%d; alpha:%d", nPass, nalpha);
        tm_setmin(nalpha, 255);
        TMUInt8 alpha = 255 - nalpha;
        ANI_TRACE("_CAniImpl::CalcAlpha(passed:%d, alpha:%d)", nPass, alpha);
//        m_alpha = alpha;
        return (TMUInt8)alpha;
    }

    virtual TMBool Render(IGXApp* gapp, ITuiPic* pPic, int nState, EButStates states, const CTuiRect& rc) tm_override
    {
        if (states != m_stateNew)
        {
            ANI_TRACE("CMixAni:Render(idObj:%d) states(%x) != m_stateNew(%x)",
                m_hTarget->GetObjectID(), states, m_stateNew);
            return false;
        }
        if (!m_tmID)
            return false;
        int nAlpha = m_alpha; //CalcAlpha();
        ANI_TRACE("CMixAni:Render(target:%p, state:%d->%d; alpha:%d, idObj:%d)",
            m_hTarget, m_stateOld, m_stateNew, nAlpha, m_hTarget->GetObjectID());
        if (nAlpha <= 0)
        {
            ANI_TRACE("CMixAni:Render(idObj:%d) finish", m_hTarget->GetObjectID());
            return false;
        }
        WPRENDERFX wpf;
        wpf.nMixAlpha = nAlpha;
        wpf.nOldState = m_stateOld;
        wpf.nNewState = states;
        pPic->Render(nState, 0, rc, &wpf, gapp);
        return true;
    }

    //  CAniProc
    virtual void OnAni(TMHVni hVni, TMClock tmNow, TMClock tmVBlank) tm_override
    {
        Ref ref(m_hTarget);
        TMUInt8 alpha = m_alpha;
        TMUInt8 aNew = CalcAlpha(tmNow + 2 * tmVBlank);
        if (aNew != alpha)
        {
            m_alpha = aNew;
            m_hTarget->Invalidate();
        }
        if (aNew == 0)
            EraseSelf(m_hTarget);
    }
};

struct CWindowAni
    : public _CAniImpl
    , public CBaseRefT<CWindowAni, TM::CRefObjectRoot<1> >
{
public:
    TM_SELF_ALLOC(CWindowAni);

private:
    TMULong m_dwFlag;
    TMULong m_dwTime;
    CTuiRect m_rcFrom;
    CTuiRect m_rcTo;

public:
    CWindowAni(HTUI hWnd)
        : _CAniImpl(hWnd)
    {
    }

    void Check(HTUI hWnd, EHitID nHitID)
    {
        DbgAssert(hWnd == m_hTarget);
        DbgAssert(nHitID == eHitIDOut);
    }

    void ResetAs(TMULong dwTime, TMULong dwFlags, const CTuiRect* rcFrom, const CTuiRect* rcTo, IEase* pEase)
    {
        ANI_TRACE("CWindowAni:CWindowAni(idObj:%d, dwTime:%d, rcFrom(%g,%g,%g,%g), rcTo(%g,%g,%g,%g)", m_hTarget->GetObjectID(),
            rcFrom ? rcFrom->Left() : 0,
            rcFrom ? rcFrom->Top() : 0,
            rcFrom ? rcFrom->Right() : 0,
            rcFrom ? rcFrom->Bottom() : 0,
            rcTo ? rcTo->Left() : 0,
            rcTo ? rcTo->Top() : 0,
            rcTo ? rcTo->Right() : 0,
            rcTo ? rcTo->Bottom() : 0
           );

        // init
        Start();
        m_dwFlag = dwFlags;
        m_dwTime = dwTime;
        m_ptrEase = pEase;
        if (!pEase)
            EaseCreatePower(&m_ptrEase, 9.0);
        
        CTuiRect rc1, rc2;
        if (m_hTarget->Is_WS_PRI_IS_WINDOW())
        {
            CRect rcx;
            CWindowAT(m_hTarget->GetHWND()).GetWindowRect(&rcx);
            rc1.CopyFrom(rcx);
        }
        else
            m_hTarget->GetWindowRect(&rc1);
        if (dwFlags & AW_CENTER)
        {
            CTuiPoint pt = rc1.CenterPoint();
            rc2.SetRect(pt, pt);
        }
        else
        {
            enum
            {
                HP = AW_HOR_POSITIVE | AW_HIDE,
                HN = AW_HOR_NEGATIVE | AW_HIDE,
                VP = AW_VER_POSITIVE | AW_HIDE,
                VN = AW_VER_NEGATIVE | AW_HIDE,
            };
            rc2 = rc1;
            if ((dwFlags & HP) == AW_HOR_POSITIVE || (dwFlags & HN) == HN)
                rc2.KeepLSetR(rc1.Left());
            else if ((dwFlags & HP) == HP || (dwFlags & HN) == AW_HOR_NEGATIVE)
                rc2.SetLeft(rc1.Right());
            if ((dwFlags & VP) == AW_VER_POSITIVE || (dwFlags & VN) == VN)
                rc2.KeepTSetB(rc1.Top());
            else if ((dwFlags & VP) == VP || (dwFlags & VN) == AW_VER_NEGATIVE)
                rc2.SetTop(rc1.Bottom());
        }
        
        if (dwFlags & AW_HIDE)
        {
            m_rcFrom = rcFrom ? *rcFrom : rc1; m_rcTo = rcTo ? *rcTo : rc2;
            SetPos(rc1, SWP_SHOWWINDOW | SWP_NOACTIVATE);
        }
        else
        {
            m_rcFrom = rcFrom ? *rcFrom : rc2;
            m_rcTo = rcTo ? *rcTo : rc1;
        }
    }

    void Reset()
    {
        ANI_TRACE("CWindowAni:Reset(idObj:%d)", m_hTarget->GetObjectID());
        if (m_dwFlag & AW_HIDE)
            SetPos(m_rcFrom, SWP_HIDEWINDOW | SWP_NOACTIVATE);  // restore old position;
        else
            SetPos(m_rcTo, SWP_SHOWWINDOW | SWP_NOACTIVATE);
        if (m_dwFlag & AW_BLEND)
            m_hTarget->SendMessage(CM_SETALPHA, 0, 255);
    }

    virtual void Delete(TMBool bReset) tm_override
    {
        this->Pause();
        if (bReset)
            Reset();
        Release();
    }

    float CalcPoint(TMClock tmNow, float x1, float x2)
    {
        return EaseCalcProgress(m_ptrEase, m_dwFlag, x1, x2, m_dwTime, GetPassed(tmNow));
    }

    void SetPos(CTuiRect& rc, TMUInt nFlags)
    {
        if (m_hTarget->Is_WS_PRI_IS_WINDOW())
            CWindowAT(m_hTarget->GetHWND()).SetWindowPos(0, rc.Left(), rc.Top(), rc.Width(), rc.Height(), nFlags); // maybe call GetMessage.
        else
            m_hTarget->SetWindowPos(0, &rc, nFlags | SWP_NOZORDER);
    }

    //  CAniProc
    virtual void OnAni(TMHVni hVni, TMClock tmNow, TMClock tmVBlank) tm_override
    {
        AddRef();
        Ref ref(m_hTarget);
        tmNow += 2 * tmVBlank;
        CTuiRect rc(
            CalcPoint(tmNow, m_rcFrom.Left(), m_rcTo.Left()),
            CalcPoint(tmNow, m_rcFrom.Top(), m_rcTo.Top()),
            CalcPoint(tmNow, m_rcFrom.Right(), m_rcTo.Right()),
            CalcPoint(tmNow, m_rcFrom.Bottom(), m_rcTo.Bottom()));
        SetPos(rc, SWP_SHOWWINDOW | SWP_NOACTIVATE);

        ANI_TRACE("CWindowAni:OnAni(idObj:%d) progress:%d-%d",
            m_hTarget->GetObjectID(), m_dwTime, GetPassed(tmNow));
        if (m_dwFlag & AW_BLEND)
        {
            TMUInt8 alpha = CalcPoint(tmNow, 255, 0);
            TMUInt8 a = m_dwFlag & AW_HIDE ? alpha : 255 - alpha;
            m_hTarget->SendMessage(CM_SETALPHA, 0, a);
        }

        if (m_tmID && GetPassed(tmNow) >= m_dwTime)
        {
            Pause();
            EraseSelf(m_hTarget);
        }
        Release();
    }
};


CWidgetAniList::CAniList::~CAniList()
{
     recorder::list_clear(*this, _CAniImpl::CDestroy());
}

void CWidgetAniList::ClearAni()
{
    if (m_aAni)
    {
        CAniList* p = m_aAni;
        m_aAni = 0;
        delete p;
    }
}

void CWidgetAniList::PreDestroy()
{
    if (m_aAni)
    {
        for (CAniList::recorder rec(*m_aAni); rec; ++rec)
            rec.get_data()->Pause();
    }
}

_CAniImpl*& CWidgetAniList::AddAni(EHitID nHitID)
{
    if (!m_aAni)
        m_aAni = new CAniList();
    return (*m_aAni)[nHitID];
}

void CWidgetAniList::AddMixAni(HTUI hWnd, EHitID nHitID, EButStates nOldState, EButStates nNewState)
{
    DbgAssert(!hWnd->IsLifeInvalid());

    // create ani list
    _CAniImpl*& ani = AddAni(nHitID);
    if (!ani)
        ani = new CMixAni(hWnd);
    ani->Check(hWnd, nHitID);
    (static_cast<CMixAni*>(ani))->ResetAs(nOldState, nNewState);
    return;
}

void CWidgetAniList::CheckErase()
{
    DbgAssert(m_aAni);
    if (!m_aAni->size())
    {
        ANI_TRACE("CScrollAni:CheckErase() Clean Ani");
        delete m_aAni;
        m_aAni = 0;
    }
}

void CWidgetAniList::AniRender(IGXApp* gapp, int nHitID, ITuiPic* pPic, int nState, EButStates states, const CTuiRect& rc)
{
    if (m_aAni)
    {
        CAniList::recorder rec(*m_aAni);
        rec.find(nHitID);
        if (rec)
        {
            _CAniImpl* p = rec.get_data();
            if (p->Render(gapp, pPic, nState, states, rc))
                return;
            _CAniImpl::EraseIter(rec); // rec.erase(_CAniImpl::CDelete());
            CheckErase();
//             return;
        }
    }
    pPic->Render(nState, states, rc, gapp);
}

void CWidgetAniList::AniRenderScroll(IGXApp* gapp, ITuiPic* pPic, const SCROLLSTATE& state, const CTuiRect& rc)
{
    if (m_aAni)
    {
        WPRENDERFX_SB fx = { state.eState[0], state.eState[1], state.eState[2], state.eState[3], state.eState[4] };

        int n = 0;
        CAniList::iterator iter = m_aAni->lower_bound(TUIHITMAKE(state.obj, 0));
        for (; iter != m_aAni->end();)
        {
            EHitID hid = (EHitID)iter->first;
            if (TUIHITGETOBJ(hid) != state.obj)
                break;
            CMixAni* ani = static_cast<CMixAni*>(iter->second);
            TMUInt8 alpha = ani->m_alpha;
            if (alpha == 0)
            {
                iter = m_aAni->erase(iter);
                ani->Delete(true);
            }
            else
            {
                ++n;
                fx.eState[TUIHITGETSUBSB(hid)] = ani->m_stateOld;
                fx.nAlpha[TUIHITGETSUBSB(hid)] = alpha;
                ++iter;
            }
        }

        if (n)
        {
            ANI_TRACE("CWidgetAniList:AniRenderScroll(nHitID:%d, newstates:%X,%X,%X,%X,%X, oldstates:%X,%X,%X,%X,%X, alphas:%d,%d,%d,%d,%d)",
                state.obj, state.eState[0], state.eState[1], state.eState[2], state.eState[3], state.eState[4],
                fx.eState[0], fx.eState[1], fx.eState[2], fx.eState[3], fx.eState[4],
                fx.nAlpha[0], fx.nAlpha[1], fx.nAlpha[2], fx.nAlpha[3], fx.nAlpha[4]);
            pPic->RenderSB(state, rc, &fx, gapp);
            CheckErase();
            return;
        }
    }
    pPic->RenderSB(state, rc, 0, gapp);
}

void CWidgetAniList::AddButtonAni(const TUIHITINFO& self, const TUIHITINFO* pold, const TUIHITINFO* pnew, TMBool bExtMouseDown, int nCurCheck)
{
    if (!self.hWnd || self.hWnd->IsLifeInvalid()
        || (!self.bNC && !self.hWnd->GetView()->m_bAnimationButton))
        return;
    CView* pm = self.hWnd->GetView();
    bExtMouseDown = bExtMouseDown || pm->IsMouseDown();
    HTUI hFocus = pm->GetFocus();
    EButStates nOld = (EButStates)(nCurCheck | self.hWnd->DoGetStates(hFocus, pold->hWnd, bExtMouseDown));
    EButStates nNew = (EButStates)(nCurCheck | self.hWnd->DoGetStates(hFocus, pnew->hWnd, bExtMouseDown));
    AddMixAni(self.hWnd, self.hid, nOld, nNew);
}

void CWidgetAniList::AddSysButAni(const TUIHITINFO& self, EButStates nOldState, EButStates nNewState, int nCurCheck)
{
    DbgAssert(self.hWnd);
    if (self.hWnd->IsLifeInvalid() || !self.hWnd->GetView()->m_bAnimationSysBut)
        return;
    EButStates nOld = nOldState | (EButStates)nCurCheck;
    EButStates nNew = nNewState | (EButStates)nCurCheck;
    AddMixAni(self.hWnd, self.hid, nOld, nNew);
}

void CWidgetAniList::AddScrollAni(const TUIHITINFO& s, const TUIHITINFO* pold, const TUIHITINFO* pnew, TMBool bHotSB)
{
    DbgAssert(s.hWnd);
    if (s.hWnd->IsLifeInvalid() || !s.hWnd->GetView()->m_bAnimationScroll)
        return;
    TUIHITINFO self(s);
    EHitObj obj = TUIHITGETOBJ(self.hid);
    for (int i = e_objSBFirst; i <= e_objSBLast; ++i)
    {
        self.hid = TUIHITMAKE(obj, i);
        EButStates nOld = g_input.GetScrollStates(self, pold, bHotSB);
        EButStates nNew = g_input.GetScrollStates(self, pnew, bHotSB);
        AddMixAni(self.hWnd, (EHitID)self.dwCode, nOld, nNew);
    }
}

void CWidgetAniList::AddScrollShowHideAni(EHitObj obj, HTUI hWnd, TMBool bShow)
{
    DbgAssert(hWnd);
    if (hWnd->IsLifeInvalid() || !hWnd->GetView()->m_bAnimationScroll)
        return;
    for (int i = e_objSBFirst; i <= e_objSBLast; ++i)
    {
        EHitID nObjID = TUIHITMAKE(obj, i);
        if (bShow)
            AddMixAni(hWnd, nObjID, e_bssHide, e_bssNormal);
        else
            AddMixAni(hWnd, nObjID, e_bssNormal, e_bssHide);
    }
}

TMBool CWidgetAniList::AnimateWindow(HTUI hWnd, TMULong dwTime, TMULong dwFlags, const CTuiRect* rcFrom, const CTuiRect* rcTo, IEase* pEase)
{
    if (!dwTime)
    {
        if (m_aAni)
        {
            CAniList::recorder rec(*m_aAni);
            rec.find(eHitIDOut);
            if (!rec)
                return false;
            _CAniImpl::EraseIter(rec); // rec.erase(_CAniImpl::CDelete());
        }
        return true;
    }

    if (hWnd->IsLifeInvalid())
        return false;

    Ref ref(hWnd);
    
    _CAniImpl*& pp = AddAni(eHitIDOut);
    CWindowAni* pwa;
    if (!pp)
    {
        pp = pwa = new CWindowAni(hWnd);
        pwa->AddRef();
    }
    else
    {
        pwa = static_cast<CWindowAni*>(pp);
        pwa->AddRef();
        pwa->Reset();
        if (hWnd->IsLifeDestroyed())
            return false;
    }
    pwa->Check(hWnd, eHitIDOut);
    pwa->ResetAs(dwTime, dwFlags, rcFrom, rcTo, pEase);
    pwa->Release();
    return true;
}

TMBool CWidgetAniList::IsInAnimation(EHitID nObjID)
{
    if (!m_aAni)
        return false;
    CAniList::const_iterator iter = m_aAni->lower_bound(nObjID);
    if (iter == m_aAni->end())
        return false;
    return TUIHITGETOBJ(iter->first) == TUIHITGETOBJ(nObjID);
}


TM_END_NAMESPACE

