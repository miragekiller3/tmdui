/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2007.01.31
 *
 *  Copyright (C) 2007 miragekiller
 */

#include "uitree.h"
#include <functional>
#include "view.h"
#include "tmkeyboard.h"
#include "uiwindow.h"
#include "tooltip.h"
#include "helper.h"


TM_BGN_NAMESPACE


//#define TREE_TRACE DbgTrace
#define TREE_TRACE DbgTraceNull

// struct FNTVEnum
// {
//     inline static TMBool IsNormal(const TREEITEM* p) { return ((p->m_state & (TVIS_HIDE | TVIS_EXPANDED)) == TVIS_EXPANDED); }
//     inline static TMBool IsHide(const TREEITEM* p) { return (p->m_state & TVIS_HIDE); }
// };
// typedef CTreeItemT<TREEITEM*, CTreeDataSourceContainerF<FNTVEnum> > CVisbleTVRec;
typedef CTreeItemT<TREEITEM*> CTVRec;

struct FWalkStopTVItem
{
    TMBool operator () (TREEITEM* itm) const
    {
        return 0 != itm->_parent;  // skip root item
    }
};

struct FWalkStopTVItemVisible
{
    TMBool operator () (TREEITEM* itm) const
    {
        if (!itm->_parent) // skip root item
            return false;
        if (itm->m_state & TVIS_HIDE)
            return false;
        for (; itm = itm->_parent;)
        {
            if ((itm->m_state & (TVIS_HIDE | TVIS_EXPANDED)) != TVIS_EXPANDED)
                return false;
        }
        return true;
    }
};




////////////////////////////////////////////////////////////////////////////////
// TREEITEM
TREEITEM::TREEITEM(CTuiTreeImpl* p, const CStrPtr& str, TMUInt uState)
    : m_pOwner   (p)
    , m_state    (uState)
    , m_statePri (0)
    , m_lParam   (0)
    , m_nWidth   (0)
    , m_nHeight  (-1)
    , m_nTop     (-1)
    , m_hBind (0)
{
    if (str)
        m_gxText.ReCreate(str, uState & TVIS_HTML);
}

TREEITEM::~TREEITEM()
{
    if (_parent) // not root
        DbgAssert(IsDestroyed());
//     DbgAssert(!m_hBind);
//     if (_parent)
//         DbgAssert(!IsValid());
}

static int _TREEITEM_GetLevel(TREEITEM* p)
{
    return CTVRec(p).GetLevel();
}

struct _TREEITEM_SYNC_BIND
{
private:
    TREEITEM*   m_itm;
    TMBool      m_bCheck;
    TMBool      m_bExpend;
    TMBool      m_bIndent;
    TMBool      m_bText;
    TMBool      m_bSelected;

    static TMBool CALLBACK EnumProc(HTUI hwnd, TMLParam lParam)
    {
        _TREEITEM_SYNC_BIND* p = (_TREEITEM_SYNC_BIND*)lParam;
        p->_Done(hwnd, hwnd->m_dwObjID);
        return true;
    }

    void _Done(CTuiButton but, TMUInt32 id)
    {
        switch (id)
        {
        case IDP_PRE_BIND_CHECK:
            if (m_bCheck)
                but.SetCheck((m_itm->m_state & TVIS_CHECKED) ? 1 : 0);
            break;

        case IDP_PRE_BIND_EXPAND:
            if (m_bExpend)
                but.SetCheck((m_itm->m_state & TVIS_EXPANDED) ? 1 : 0);
            break;

        case IDP_PRE_BIND_SELECTED:
            if (m_bSelected)
                but.SetCheck((m_itm->m_state & TVIS_SELECTED) ? 1 : 0);
            break;

        case IDP_PRE_BIND_INDENT:
            if (m_bIndent)
            {
                CTuiRect rc;
                but.GetWindowRect(&rc);
                rc.SetWidth(m_itm->m_pOwner->m_uIndent * (_TREEITEM_GetLevel(m_itm) - 1));
                but.MoveWindow(rc);
            }
            break;

        case IDP_PRE_BIND_LABLE:
            if (m_bText)
                but.SetWindowText(m_itm->m_gxText.GetString().c_str(), m_itm->m_state & TVIS_HTML);
            break;
        }
    }

public:
    friend void _TREEITEM_SYNC_ITEM_BIND(TREEITEM* p, TMBool bCheck, TMBool bExpend, TMBool bIndent, TMBool bText, TMBool bSelected)
    {
        HTUI hWnd = p->m_hBind;
        if (hWnd)
        {
            DISABLE_SPY();
            _TREEITEM_SYNC_BIND tsb;
            tsb.m_itm = p;
            tsb.m_bCheck = bCheck;
            tsb.m_bExpend = bExpend;
            tsb.m_bIndent = bIndent;
            tsb.m_bText = bText;
            tsb.m_bSelected = bSelected;
            hWnd->EnumAllChildWindows(EnumProc, (TMLParam)&tsb, true, -1);
        }
    }
};
void _TREEITEM_SYNC_ITEM_BIND(TREEITEM* p, TMBool bCheck, TMBool bExpend, TMBool bIndent, TMBool bText, TMBool bSelected);


// CDomNodeHelperT
void TREEITEM::OnInsertItem(TREEITEM* pInsert)
{
    DISABLE_SPY();
    DbgAssert(pInsert->get_nodeName() == get_nodeName());
    m_pOwner->InvalidateItemHeightChanged(pInsert->GetParent());
    _TREEITEM_SYNC_ITEM_BIND(pInsert, false, false, true, false, false);
}

static GXDips _TREEITEM_GetHeight(TREEITEM* p)
{
    GXDips h = p->m_pOwner ? (p->m_nHeight < 0 ? p->m_pOwner->m_uItemHeight : p->m_nHeight) : 0;
    return h;
}

static void _TREEITEM_GetRectLR(CTuiTreeImpl* p, TUIRect* lpRect)
{
    RectSetLeft(*lpRect, p->m_rcClient.Left() + p->m_bdPadding.left - p->GetScrollLeft());
    int r = RectLeft(*lpRect) + p->GetScrollWidth();
    tm_setmax(r, p->m_rcClient.Right() - p->m_bdPadding.right);
    RectSetRight(*lpRect,r);
}

static TMBool _TREEITEM_GetRect(TREEITEM* p, TUIRect* lpRect, TMBool bForInvalidate = false)
{
    CTuiTreeImpl* hWnd = p->m_pOwner;
    CTuiRect rc;
    hWnd->GetPaddingRect(&rc);
    GXDips t = p->m_nTop + rc.Top() - hWnd->GetScrollTop();
    GXDips b = t + _TREEITEM_GetHeight(p);
    RectSetTop(*lpRect, t);
    RectSetBottom(*lpRect, b);
    if (!bForInvalidate)
        _TREEITEM_GetRectLR(hWnd, lpRect);
    return true;
}

TMBool TREEITEM::GetRect(TUIRect* lpRect, TMBool bForInvalidate)
{
    TMBool b = _TREEITEM_GetRect(this, lpRect, bForInvalidate);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetRect() %g,%g,%g,%g",
        this, RectLeft(*lpRect), RectTop(*lpRect),
        RectRight(*lpRect), RectBottom(*lpRect));
    return b;
}

// IDomNode
CDomStrPtr TREEITEM::get_nodeName()
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::get_nodeName()", this);
    return CDomStrPtr(TM_CS(TUI_CLASS_TREENODE));
}

CDomStrPtr TREEITEM::get_nodeValue()
{
    CDomStrPtr str(m_gxText.GetString());
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::get_nodeValue() %s", this, str.c_str());
    return str;
}

TMBool _TREEITEM_IsVisible(TREEITEM* p)
{
    return p->m_nTop != -1;
}

void _TREEITEM_Invalidate(TREEITEM* p)
{
    if (_TREEITEM_IsVisible(p))
        p->m_pOwner->Invalidate();
}

void TREEITEM::put_nodeValue(const CDomStrPtr& val)
{
    DbgAssert(!IsDestroyed());
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::put_nodeValue(%s)", this, val.c_str());
    m_gxText.ReCreate(val, m_state & TVIS_HTML);
    _TREEITEM_SYNC_ITEM_BIND(this, false, false, false, true, false);
    _TREEITEM_Invalidate(this);
}

void TREEITEM::removeChild(IDomNode* node)
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::removeChild(%p)", this, node);
    DISABLE_SPY();
    DbgAssert(node);
    DbgAssert(node->get_nodeName() == TM_T2W(TUI_CLASS_TREENODE));
    TREEITEM* p = static_cast<TREEITEM*>(node);
    m_pOwner->RemoveItem(p);
}

// ITVItem
HTUI TREEITEM::GetWindow()
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetWindow()", this);
    return m_pOwner;
}

GXDips TREEITEM::GetHeight()
{
    GXDips h = _TREEITEM_GetHeight(this);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetHeight() %g", this, h);
    return h;
}

void TREEITEM::SetWidth(GXDips _nWidth)
{
    if (m_nWidth == _nWidth)
        return;
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::SetWidth(%g)", this, _nWidth);
    m_nWidth = _nWidth;
    if (CTVItem(this).IsVisible() && m_pOwner->m_nScrollW != -1)
    {
        m_pOwner->m_nScrollW = -1;
        m_pOwner->ChangeFrame(false);
    }
}

static void _TREEITEM_SetHeight(TREEITEM* p, GXDips _nHeight)
{
    if (p->m_nHeight == _nHeight)
        return;
    p->m_nHeight = _nHeight;
    p->m_pOwner->InvalidateItemHeightChanged(p);
}

void TREEITEM::SetHeight(GXDips _nHeight)
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::SetHeight(%g)", this, _nHeight);
    _TREEITEM_SetHeight(this, _nHeight);
}

int TREEITEM::GetLevel()
{
    int n = _TREEITEM_GetLevel(this);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetLevel() %d", this, n);
    return n;
}

void TREEITEM::SetData(void* dwData, TMBool bRedraw)
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::SetData(%p)", this, dwData);
    m_lParam = dwData;
    if (bRedraw)
        _TREEITEM_Invalidate(this);
}

void TREEITEM::SetHint(const TUISETTEXT& wst)
{
    m_pOwner->StringMessageToString((TMLParam)&wst, m_strHint);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::SetHint() %s", this, m_strHint.c_str());
}

void TREEITEM::SetText(const TUISETTEXT& wst)
{
    CString strx;
    m_pOwner->StringMessageToString((TMLParam)&wst, strx);
    put_nodeValue(strx);
}

TMUInt TREEITEM::GetChildCount()
{
    TMUInt n = CTVRec(this).GetChildCount();
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetChildCount() %d", this, n);
    return n;
}

static TMUInt _TREEITEM_GetAllChildCount(TREEITEM* p)
{
    return CTVRec(p).GetAllChildCount();
}

TMUInt TREEITEM::GetAllChildCount()
{
    TMUInt n = _TREEITEM_GetAllChildCount(this);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetAllChildCount() %d", this, n);
    return n;
}

TMBool TREEITEM::IsChild(ITVItem* hChild)
{
    TMBool b = CTVRec(this).IsChild(static_cast<TREEITEM*>(hChild));
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::IsChild(%p) %d", this, hChild, b);
    return b;
}

ITVItem* TREEITEM::GetRoot()
{
//     DbgAssert(IsValid());
    TREEITEM* p = &(m_pOwner->m_tiRoot);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetRoot() %d", this, p);
    return p;
}

ITVItem* TREEITEM::GetChildAt(int nIndex)
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetChildAt(%d)", this, nIndex);
    for (PTVITEM itm = GetChild(); itm; itm = itm->GetNext(), --nIndex)
    {
        if (nIndex <= 0)
            return itm;
    }
    return 0;
}

ITVItem* TREEITEM::GetPrevVisible()
{
    TREEITEM* p = m_pOwner->GetPrevVisible(this);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetPrevVisible() %p", this, p);
    return p;
}

ITVItem* TREEITEM::GetNextVisible()
{
    TREEITEM* p = m_pOwner->GetNextVisible(this);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetNextVisible() %p", this, p);
    return p;
}

ITVItem* TREEITEM::GetGlobalPrev(ITVItem* hParent)
{
    CTVRec itm(this);
    itm.GlobalPrev(static_cast<TREEITEM*>(hParent), FWalkStopTVItem());
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetGlobalPrev(%p) %p", this, hParent, *itm);
    return *itm;
}

ITVItem* TREEITEM::GetGlobalNext(ITVItem* hParent)
{
    CTVRec itm(this);
    itm.GlobalNext(static_cast<TREEITEM*>(hParent), FWalkStopTVItem());
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetGlobalNext(%p) %p", this, hParent, *itm);
    return *itm;
}

static void _CTVItem_SyncHeight(TREEITEM* pItem, TMUInt uFlag = SWP_NOREDRAW | SWP_NOLAYOUTSIBLING)
{
    DbgAssert(pItem->m_hBind);
    if (pItem->IsAutoHeight() || pItem->m_hBind->IsAutoHeight())
        _TREEITEM_SetHeight(pItem, pItem->m_hBind->GetMarginHeight());
    TUIRect rc;
    if (_TREEITEM_GetRect(pItem, &rc))
        pItem->m_hBind->ResetBindItemPosition(rc, uFlag);
}

TMBool _TREEITEM_SetState(TREEITEM& itm, TMUInt uState, TMUInt uMask)
{
    uState &= uMask;
    TMUInt uXor = (itm.m_state & uMask) ^ uState;
    if (!uXor)
        return false;
    
    DbgAssert(itm.IsValidChild());

    CRefPtr<TREEITEM> ptr1(&itm);
    uState = (itm.m_state & ~uMask) | uState;
    TMUInt uOld = itm.m_state;
    itm.m_state = uState;
    
    // bind widget
    if (itm.m_hBind)
    {
        if (uXor & TVIS_HIDE)
        {
            DISABLE_SPY();
            CTuiWidget(itm.m_hBind).ShowWindow(!(uState & TVIS_HIDE));
            if (!itm._parent) // maybe delete on ShowWindow
                return true;
        }
        if (uXor & TVIS_DISABLE)
        {
            itm.m_hBind->EnableWindow(!(uState & TVIS_DISABLE));
            if (!itm._parent) // maybe delete on EnableWindow
                return true;
        }
        
        _CTVItem_SyncHeight(&itm);
    }
    
    if (uXor & TVIS_HTML)
    {
        CString str(itm.m_gxText.GetString());
        itm.m_gxText.ReCreate(str, itm.m_state & TVIS_HTML);
    }

    // sync
    _TREEITEM_SYNC_ITEM_BIND(&itm, uXor & TVIS_CHECKED, uXor & TVIS_EXPANDED, false, uXor & TVIS_HTML, uXor & TVIS_SELECTED);

    // invalidate
    if (uXor & TVIS_HIDE)
        itm.m_pOwner->InvalidateItemHeightChanged(itm.GetParent());
    else if ((uXor & TVIS_EXPANDED) && itm.hasChildNodes())
        itm.m_pOwner->InvalidateItemHeightChanged(&itm);
    
    // sink
    if (itm.m_pOwner->m_pSink)
    {
        itm.m_pOwner->m_pSink->OnStateChanged(&itm, uOld, uState);
        if (!itm._parent) // maybe delete on OnStateChanged
            return true;
    }

    // sink
    itm.m_pOwner->Notify(TVN_STATECHANGE);
    return true;
}

TMBool TREEITEM::SetState(TMUInt uState, TMUInt uMask)
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::SetState(%x, %x)", this, uState, uMask);
    if (IsValidChild())
        return _TREEITEM_SetState(*this, uState, uMask);
    return false;
}

static void _TREEITEM_ExpandAll(TREEITEM* p)
{
    struct EXP
    {
        static void exec(PTVITEM h)
        {
            if (!h->IsRoot())
                _TREEITEM_SetState(*h, TVIS_EXPANDED, TVIS_EXPANDED);
        }
    };
    CTVRec(p).Enum(EXP::exec);
}

void TREEITEM::ExpandAll()
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::ExpandAll()", this);
    _TREEITEM_ExpandAll(this);
}

static void _TREEITEM_ShowItem(TREEITEM* p, TMBool bShow, TMBool bEnsure)
{
    if (p->IsRoot())
        return;
    
    if (bShow && bEnsure && p->_parent && p->_parent->_parent) // skip root item
        _TREEITEM_ShowItem(p->_parent, true, true);
    _TREEITEM_SetState(*p, bShow ? 0 : TVIS_HIDE, TVIS_HIDE);
}

void TREEITEM::ShowItem(TMBool bShow, TMBool bEnsure)
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::ShowItem(%d, %d)", this, bShow, bEnsure);
    _TREEITEM_ShowItem(this, bShow, bEnsure);
}

TMBool TREEITEM::IsVisible()
{
    TMBool b = _TREEITEM_IsVisible(this);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::IsVisible() %d", this, b);
    return b;
}

static TMBool _TREEITEM_IsMouseOver(TREEITEM* p, TMUInt32 dwType)
{
    if (dwType)
    {
        TUIHITINFO& hi = p->m_pOwner->GetView()->GetHitInfo();
        if ((hi.hWnd == p->m_pOwner) && (hi.pTaget == p) && !hi.bNC && (dwType & hi.dwCode))
            return true;
    }
    return false;
}

TMBool TREEITEM::IsMouseOver(TMUInt32 dwType)
{
    TMBool b = _TREEITEM_IsMouseOver(this, dwType);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::IsMouseOver() %d", this, b);
    return b;
}

static void _TREEITEM_ScrollToVisible(TREEITEM* p, TMBool bDelay = false);

struct CDelayScrollToVisible : public CTimerImpl<CDelayScrollToVisible>
{
    CRefPtr<TREEITEM> m_item;
    
    void OnTimer(...)
    {
        DbgAssert(m_item);
        if (m_item->IsDestroyed())
            return;
        _TREEITEM_ScrollToVisible(m_item, false);
    }
};

void _TREEITEM_ScrollToVisible(TREEITEM* p, TMBool bDelay)
{
    DbgAssert(p->IsValidChild());
    if (p == p->m_pOwner->m_pEnsure)
        return;
    static CDelayScrollToVisible s_ds;
    if (bDelay)
    {
        s_ds.m_item = p;
        s_ds.SetTimer(1000);
        return;
    }
    s_ds.KillTimer();
    s_ds.m_item = 0;
    p->m_pOwner->m_pEnsure = p;
    p->m_pOwner->InvalidateItems(p);
}

static TMBool _TREEITEM_EnsureVisible(TREEITEM* p, TMBool bDelay = false)
{
    if (p->IsRoot())
        return true;
    
    for (PTVITEM hp = p; hp = hp->GetParent();)
    {
        if (hp->m_state & TVIS_HIDE)
            return false;
        CTVItem(hp).Expand();
    }
    
    _TREEITEM_ScrollToVisible(p, bDelay);
    return true;
}

TMBool TREEITEM::EnsureVisible()     
{
    TMBool b = _TREEITEM_EnsureVisible(this);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::EnsureVisible()", this);
    return b;
}

void TREEITEM::ScrollToVisible()
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::ScrollToVisible()", this);
    _TREEITEM_ScrollToVisible(this);
}

EButState TREEITEM::GetButtonState(TMUInt32 dwType)
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::GetButtonState(%d)", this, dwType);
    if (!m_pOwner->IsWindowEnabled())
        return e_bsDisable;
    if (m_state & TVIS_SELECTED)
        return e_bsDown;
    return _TREEITEM_IsMouseOver(this, dwType) ? e_bsHigh : e_bsNormal;
}

void TREEITEM::SelectItem(TMUInt32 dwOption)
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::SelectItem(%d)", this, dwOption);
    if (IsValidChild())
        m_pOwner->SelectItem(this, dwOption);
}

TMBool TREEITEM::Sort(PFNTVCOMPARE pfn, TMLParam lParam, int nLevel, TMBool bReversed)
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::Sort(%p, %p, %d, %d)", this, pfn, lParam, nLevel, bReversed);
    CTVRec rec(this);
    if (pfn)
        rec.SortChildEx(pfn, lParam, nLevel, bReversed);
    else
    {
        struct CMP
        {
            static bool compare(PTVITEM h1, PTVITEM h2)
            {
                return h1->m_gxText.GetString() < h2->m_gxText.GetString();
            }
        };
        rec.SortChildEx (CMP::compare, nLevel, bReversed);
    }
    
    //    m_nScrollH = -1;
    m_pOwner->InvalidateItemHeightChanged(this);
    return true;
}

static TMBool _TREEITEM_SetZOrder(TREEITEM* p, ITVItem* hIterator, EZOrder eZOrderOrIndex)
{
    if (!hIterator)
    {
        if (eZOrderOrIndex == eZOrderUp)
            eZOrderOrIndex = eZOrderChildFirst;
        else if (eZOrderOrIndex == eZOrderDown)
            eZOrderOrIndex = eZOrderChildLast;
        hIterator = p->GetParent();
        DbgAssert(hIterator);
    }
    
    TREEITEM* pi = static_cast<TREEITEM*>(hIterator);
    switch (eZOrderOrIndex)
    {
    case eZOrderUp: if (!pi->GetParent()) return false; pi->InsertBefore(p); break;
    case eZOrderDown: if (!pi->GetParent()) return false; pi->InsertAfter(p); break;
    default: if (pi == p) return false; pi->InsertChildAt(p, eZOrderOrIndex); break;
    }
    
    // need reset m_nTop;
    p->m_pOwner->InvalidateItemHeightChanged(p->GetParent());
    return true;
}

TMBool TREEITEM::SetZOrder(ITVItem* hIterator, EZOrder eZOrderOrIndex)
{
    DbgAssert(!IsDestroyed());
    if (hIterator)
        DbgAssert(!((TREEITEM*)hIterator)->IsDestroyed());

    TMBool b = _TREEITEM_SetZOrder(this, hIterator, eZOrderOrIndex);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::Sort(%p, %d)", this, hIterator, eZOrderOrIndex);
    return b;
}

ITVItem* TREEITEM::InsertNew(const CStrPtr& str, EZOrder eZOrderOrPos, TMUInt uState)
{
    DbgAssert(!IsDestroyed());

    PTVITEM h = new TREEITEM(m_pOwner, str, uState);
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::InsertNew(%s, %d, %x) %p", this, str.c_str(), eZOrderOrPos, uState, h);
    //InsertChildAt(h, nZOrderOrPos);
    if (_TREEITEM_SetZOrder(h, this, eZOrderOrPos))
    {
        m_pOwner->InvalidateItemHeightChanged(this);
        return h;
    }
    h->Release();
    return 0;
}

void TREEITEM::Invalidate()
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::Invalidate()", this);
    _TREEITEM_Invalidate(this);
}

static TMBool _TREEITEM_BindProc(
    TREEITEM* p,
    HTUI hWnd, TMUInt uMsg, TMWParam wParam,
    TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    if (uMsg == WM_NCDESTROY)
    {
        if (hWnd == p->m_hBind)
            p->m_hBind = 0;
    }
    else if (uMsg == WM_COMMAND)
    {
        switch (LOWORD(wParam))
        {
        case IDP_PRE_BIND_CHECK:
            p->SetState(CTuiButton((HTUI)lParam).GetCheck() ? TVIS_CHECKED : 0, TVIS_CHECKED);
            break;

        case IDP_PRE_BIND_EXPAND:
            p->SetState(CTuiButton((HTUI)lParam).GetCheck() ? TVIS_EXPANDED : 0, TVIS_EXPANDED);
            break;

        case IDP_PRE_BIND_LABLE:
            if (EN_CHANGEEX == HIWORD(wParam))
                p->m_gxText.ReCreate(((HTUI)lParam)->m_gxText->GetHtml(), p->m_state & TVIS_HTML);
            break;
        }
    }
    return false;
}

static void _TREEITEM_BindItemWindow(TREEITEM* p, HTUI hWnd)
{
    DbgAssert(p->IsValidChild());
    if (p->m_hBind)
        p->m_hBind->DestroyWindow();
    p->m_hBind = hWnd;
    if (hWnd)
    {
        DbgAssert(!hWnd->Is_WS_PRI_BINDMODE());
        hWnd->Bind_Set(p->m_pOwner, p);
        _CTVItem_SyncHeight(p, 0);
        DISABLE_SPY();
        CTuiWidget(hWnd)._Debug_WidgetMustBeNotMgr();
        _TREEITEM_SYNC_ITEM_BIND(p, true, true, true, true, true);
        hWnd->GetProcs().Subclass(hWnd, CWidgetMsgDelegate(p, &_TREEITEM_BindProc), hWnd);
    }
}

void TREEITEM::BindItemWindow(HTUI hWnd)
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::BindItemWindow(%p)", this, hWnd);
    if (m_hBind == hWnd)
        return;
    _TREEITEM_BindItemWindow(CRefPtr<TREEITEM>(this), hWnd);
}

HTUI TREEITEM::CreateBindItemWindow(TMUInt nObjID)
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::CreateBindItemWindow(%d)", this, nObjID);
    DbgAssert(IsValidChild());
    CRefPtr<TREEITEM> ptr(this);
    CTuiWidget wnd;
    wnd.Create(m_pOwner, nObjID);
    if (!wnd)
        return 0;
    Ref ref(wnd);
    _TREEITEM_BindItemWindow(this, wnd);
    if (ref->IsLifeInvalid())
        return 0;
    if (ptr->IsDestroyed())
        return (ref->DestroyWindow(), (HTUI)0);
    return wnd;
}

HTUI TREEITEM::DetachBindItemWindow(HTUI hNewParent)
{
    LOG_FUNC_FOR_SPY(m_pOwner, L"TREEITEM(%p)::DetachBindItemWindow(%p)", this, hNewParent);
    HTUI hWnd = m_hBind;
    if (!hWnd)
        return 0;
    hWnd->GetProcs().Unsubclass(hWnd, CWidgetMsgDelegate(this, &_TREEITEM_BindProc));
    m_hBind = 0;
    hWnd->Bind_Reset(hNewParent ? hNewParent : m_pOwner);
    return hWnd;
}


////////////////////////////////////////////////////////////////////////////////
// CTuiTreeImpl

TM_REGISTER_CLASS1 (CTuiTree);

CTuiTreeImpl::CTuiTreeImpl ()
    : m_pSel (0)
    , m_tiRoot (this, CStrPtr(), TVIS_EXPANDED)
//     , m_pSource (0)
    , m_pSink (0)
    , m_pLdownSel (0)
    , m_pEnsure (0)
    , m_pFirstVisible (0)
    , m_pLastVisible (0)
    , m_uItemHeight (16)
    , m_uIndent (0)
    , m_pfnDelete (0)
//     , m_nScrollW (-1)
//     , m_nScrollH (-1)
{
    m_dwStyle |= WS_TABSTOP;
    m_tiRoot.m_nHeight = 0;
    m_tiRoot.m_nTop = 0;
    SetAutoHScroll();
    SetAutoVScroll();
    Set_WS_PRI_DIRTY();
//     m_pFore.GetPre(IDP_PRE_PIC_LISTBOX);
//    m_dwAdvStyle |= (WS_ADV_AUTOVSCROLLBAR | WS_ADV_AUTOHSCROLLBAR);
}

CTuiTreeImpl::~CTuiTreeImpl ()
{
//     DbgAssert(!m_tiRoot.IsValid());
    DbgAssert(!m_tiRoot._child);
    DbgAssert(!m_tiRoot._next);
}

void CTuiTreeImpl::FreeMem(PTVITEM hItem, TMBool bPrev)
{
    for (; hItem = bPrev ? GetPrevVisible(hItem) : GetNextVisible(hItem);)
    {
        if (!hItem->m_gxText)
            return;
        GXSize sz;
        if (!hItem->m_gxText->GetSize(sz))
            return;
        hItem->m_gxText.Reclaim();
    }
}

TMBool CTuiTreeImpl::RebuildEnsure()
{
    DbgAssert(m_pEnsure);
    PTVITEM hEnsure = m_pEnsure;
    if (!_TREEITEM_IsVisible(hEnsure)) // un-visible
        return false;
    
    CTuiRect rc;
    _TREEITEM_GetRect(hEnsure, rc);
    if (ScrollToVisible(rc, false, true))
    {
        Invalidate();
        m_pFirstVisible = 0;
        return true;
    }
    
    return false;
}

void CTuiTreeImpl::RebuildFirst()
{
    PTVITEM itmPrev = 0;
    GXDips nTop = GetScrollTop() - m_bdPadding.top;
    for (PTVITEM ht = &m_tiRoot; ht = GetNextVisible(ht);)
    {
        DbgAssert(_TREEITEM_IsVisible(ht));
        if (ht->m_nTop < nTop)
            itmPrev = ht;
        else
        {
            m_pFirstVisible = itmPrev ? itmPrev : ht;
            return;
        }
    }

    m_pFirstVisible = itmPrev; //RebuildFirstProc (m_tiRoot._child, GetScrollTop() - m_bdPadding.top, m_uItemHeight);
}

void CTuiTreeImpl::RebuildItemProc (PTVITEM p, TMBool bVisible, GXDips& cx, GXDips& cy)
{
    for (; p; p = p->GetNext())
    {
        TMBool bv = bVisible && !(p->m_state & TVIS_HIDE);
        
        if (bv)
        {
            GXDips nHeight = _TREEITEM_GetHeight(p); //(m_nHeight < 0 ? m_uItemHeight : p->m_nHeight);
            p->m_nTop = cy;
            cy += nHeight;
            tm_setmax(cx, p->m_nWidth);
//            cx = tm_max (sz.cx, p->m_nWidth);
            
            if (!m_pFirstVisible && (cy + m_bdPadding.top - GetScrollTop()) >= 0)
                m_pFirstVisible = p;

            m_pLastVisible = p;
        }
        else
        {
            if (_TREEITEM_IsVisible(p) && p->m_gxText)
                p->m_gxText.Reclaim();
            p->m_nTop = -1;
        }
        
        TMBool bvChild = bv && (p->m_state & TVIS_EXPANDED);
        RebuildItemProc (p->GetChild(), bvChild, cx, cy);
    }
}

// if m_pFirstVisible is 0,position needs to be recalculated 
PTVITEM CTuiTreeImpl::GetFirst ()
{
    if (Is_WS_PRI_DIRTY()) // m_nScrollH == -1 || m_nScrollW == -1)
    {
        Reset_WS_PRI_DIRTY();
//        TREE_TRACE("CTuiTreeImpl::GetFirst (%d)", GetTickCount());

        m_pFirstVisible = 0; //m_tiRoot._child;

        GXDips cx = m_tiRoot.m_nWidth, cy = 0;
        RebuildItemProc (m_tiRoot.GetChild(), true, cx, cy);
        SendScrollRangeMessage(&cx, &cy);
    }

    if (m_pEnsure)
        RebuildEnsure();
    
    if (!m_pFirstVisible)
        RebuildFirst();

    return m_pFirstVisible;
}

void CTuiTreeImpl::InvalidateItems ()
{
    m_pFirstVisible = 0;
    SetDirtyAndInvalidate();
    //ChangeFrame(false);
}

void CTuiTreeImpl::InvalidateItems (PTVITEM hItem)
{
    if (_TREEITEM_IsVisible(hItem))
        InvalidateItems();
}

void CTuiTreeImpl::InvalidateItemHeightChanged(PTVITEM hItem)
{
    if ((hItem && _TREEITEM_IsVisible(hItem)) && !Is_WS_PRI_DIRTY())// m_nScrollH != -1)
    {
        m_pFirstVisible = 0;
        SetDirtyAndInvalidate();// m_nScrollH = -1;
//        ChangeFrame(false);
    }
}

GXDips CTuiTreeImpl::GetScrollWidth ()
{
    if (Is_WS_PRI_DIRTY()) // m_nScrollW == -1)
        GetFirst();
    return m_nScrollW;
}

GXDips CTuiTreeImpl::GetScrollHeight ()
{
    if (Is_WS_PRI_DIRTY()) // m_nScrollH == -1)
        GetFirst();
    return m_nScrollH;
}

void CTuiTreeImpl::AfterScroll ()
{
    //InvalidateItems(); 
    if (m_pFirstVisible)
    {
        m_pFirstVisible = 0;
        Invalidate();
    }
}

void CTuiTreeImpl::RemoveItem(PTVITEM hItem)
{
    if (hItem->IsDestroyed())
    {
        if (hItem->_parent)
            DbgAssert(hItem->_parent->_child != hItem);
        return;
    }

    if (!hItem->IsRoot())
        hItem->SetDestroyed();
    InvalidateItemHeightChanged(hItem);
    hItem->DetachSimple(); // keep parent
    hItem->_next = 0;
    hItem->_prev = 0;

    for (; hItem->_child;)
    {
        PTVITEM hc = hItem->GetChild();
        DbgAssert(!hc->IsDestroyed());
        RemoveItem(hc);
    }

    if (hItem->IsRoot())
        return;

    hItem->m_pOwner->GetView()->ClearChildHitInfo(hItem->m_pOwner, this);

    if (hItem->m_lParam && hItem->m_pOwner->m_pfnDelete)
    {
        TMLParam lp = (TMLParam)hItem->m_lParam;
        hItem->m_lParam = 0;
        hItem->m_pOwner->m_pfnDelete(hItem->m_pOwner, (TMLParam)hItem, lp);
        DbgAssert(!hItem->m_lParam);
    }
//     hItem->m_pOwner = 0;

    PTVITEM pSel = m_pSel;
    if (m_pLdownSel == hItem)
        m_pLdownSel = NULL;
    if (m_pSel == hItem)
        m_pSel = NULL;
    if (m_pEnsure == hItem)
        m_pEnsure = 0;
    if (m_pLastVisible == hItem)
        m_pLastVisible = 0;

    if (hItem->m_hBind)
    {
        Ref h = hItem->m_hBind;
        hItem->m_hBind = 0;
        h->DestroyWindow();
    }

    if (hItem == pSel)
    {
        if (m_pSink)
            m_pSink->OnSelChanged(hItem, 0);
        Notify(TVN_SELCHANGE);
    }

    if (m_pSink)
        m_pSink->OnDeleteItem(hItem);

    hItem->Release();
}


void CTuiTreeImpl::Reset ()
{
//     SetSel (0);
//     SetHot (0);
//     SetFocu (0);
    CTVItem(&m_tiRoot).RemoveChild();
}

void CTuiTreeImpl::AutoScroll ()
{
    Invalidate();
}

PTVITEM CTuiTreeImpl::GetPrevVisible(PTVITEM hItem)
{
    DbgAssert(hItem);
    CTVRec rec(hItem);
    rec.GlobalCyclePrev(0, FWalkStopTVItemVisible());
    return *rec;
//    return (*rec == &m_tiRoot) ? 0 : *rec;

//     if (hItem->_prev)
//     {
//         // skip unvisible
//         PTVITEM hi = hItem->_prev;
//         for (; hi->m_state & TVIS_HIDE; hi = hi->_prev)
//         {
//         }
// 
//         if (hi)
//         {
//             if (hi->m_state & TVIS_EXPANDED)
//             {
//                 PTVITEM hc = GetLastVisible (hi->_child);
//                 if (hc)
//                 {
//                     return hc;
//                 }
//             }
// 
//             return hi;
//         }
//     }
// 
//     return hItem->_parent;
}

PTVITEM CTuiTreeImpl::GetNextVisible(PTVITEM hItem)
{
    DbgAssert(hItem);

    CTVRec rec(hItem);
    rec.GlobalCycleNext(0, FWalkStopTVItemVisible());
    return *rec;
//     if (rec == &m_tiRoot)
//         rec.GlobalNext(0);
//     return *rec;
}

PTVITEM CTuiTreeImpl::GetSelfOrNextEnableVisible(PTVITEM hItem)
{
    for (; hItem; hItem = GetNextVisible(hItem))
    {
        if (!hItem->IsDisable())
            return hItem;
    }
    return hItem;
}

PTVITEM CTuiTreeImpl::GetSelfOrPrevEnableVisible(PTVITEM hItem)
{
    for (; hItem; hItem = GetPrevVisible(hItem))
    {
        if (!hItem->IsDisable())
            return hItem;
    }
    return hItem;
}

PTVITEM CTuiTreeImpl::GetLastVisible()
{
    CTVRec rec(&m_tiRoot);
    rec.GlobalPrev(0, FWalkStopTVItemVisible());
    return *rec;
// 
//     CVisbleTVRec rec(&m_tiRoot);
//     rec.GlobalLast();
//     return *rec;

//     if (!hItem)
//     {
//         return 0;
//     }
//     
//     PTVITEM hi = GetLastVisible (hItem->_next);
//     if (hi)
//     {
//         return hi;
//     }
//     
//     if (hItem->m_state & TVIS_HIDE)
//     {
//         return 0;
//     }
//     
//     if (!(hItem->m_state & TVIS_EXPANDED))
//     {
//         return hItem;
//     }
//     
//     hi = GetLastVisible (hItem->_child);
//     if (hi)
//     {
//         return hi;
//     }
//     
//     return hItem;
}

TMBool CTuiTreeImpl::HitTestTree(TUIHITINFO* pht, TMUInt uMsg)
{
    PTVITEM hi = GetFirst();
    if (!hi || !m_rcClient.PtInRect(pht->pt))
        return false;
    
    GXDips y = PointY(pht->pt) - (m_rcClient.Top() - GetScrollTop() + m_bdPadding.top);
    PTVITEM hOld = hi;
    for (;;)
    {
        DbgAssert(_TREEITEM_IsVisible(hi));
        if (y < hi->m_nTop)
            break;
        hOld = hi;
        hi = GetNextVisible(hi);
        if (!hi)
        {
            if (y > hOld->m_nTop + _TREEITEM_GetHeight(hOld))
            {
                pht->pTaget = 0;
                return true;
            }
            break;
        }
    }

    GXDips x = PointX(pht->pt) - (m_rcClient.Left() - GetScrollLeft() + m_bdPadding.left);
    pht->dwCode = x > m_uIndent * (_TREEITEM_GetLevel(hOld) - 1) ? TVHT_ONITEMINDENT : TVHT_ONITEMLABEL;
    pht->pTaget = hOld;
    return true;
}

TMBool CTuiTreeImpl::HitTest(TUIHITINFO* pht, TMUInt uMsg)
{
    if (!HitTestTree(pht, uMsg))
        return false;
    if (pht->pTaget && ((PTVITEM)(pht->pTaget))->m_hBind)
    {
        TUIHITINFO htNew(*pht);
        ((PTVITEM)(pht->pTaget))->m_hBind->GetAndHitMouseTarget(htNew, uMsg, 0);
        if (htNew.hWnd)
            *pht = htNew;
    }
    return true;
}

void CTuiTreeImpl::DoSelChange(PTVITEM hOld, PTVITEM hNew, TMUInt nKey)
{
    DbgAssert(hOld != hNew);
    //HTUI hFocusOld = _tuiApp.GetFocus();
    if (hOld && hOld->m_hBind)
        hOld->m_hBind->Bind_Unselect();
    if (hNew && hNew->m_hBind)
        hNew->m_hBind->Bind_Select();
    if (m_pSink)
        m_pSink->OnSelChanged(hOld, hNew, nKey);
    Notify(TVN_SELCHANGE);
    //HTUI hFocusNew = _tuiApp.GetFocus();
    //if (hFocusNew == this)
    //    return;
    //if (hNew && CTVItem(hNew).IsBindChild(hFocusNew))
    //    return;
    //if (hFocusOld && hOld && CTVItem(hOld).IsBindChild(hFocusOld)) // old focus on self
    //    this->SetFocus(eFocusReasonUnknown);
}


bool CTuiTreeImpl::SelectItem(PTVITEM hItem, TMULong dwOption, TMUInt nKey /*= 0*/)
{
    if (hItem)
        DbgAssert(hItem->IsValidChild());

    if (hItem && hItem->IsDisable())
        return false;

    PTVITEM hOld = m_pSel;

    if (GetStyle() & TVS_MULTIPLESEL)
    {
        if (hItem && ((dwOption & CTVItem::e_soSelMask) == CTVItem::e_soDeselect))
        {
            if (hItem->m_state & TVIS_SELECTED)
                dwOption = ((dwOption & ~(CTVItem::e_soSelMask | CTVItem::e_soCheckMask)) | CTVItem::e_soCtrl);
            else
                return false;
        }

        TMBool bControl = (((dwOption & CTVItem::e_soSelMask) == CTVItem::e_soCtrl) ||
            ((dwOption & CTVItem::e_soAuto) && g_input.m_key.IsModifiersControl()));
        TMBool bShift   = (((dwOption & CTVItem::e_soSelMask) == CTVItem::e_soShift) ||
            ((dwOption & CTVItem::e_soAuto) && g_input.m_key.IsModifiersShift()));

        if (hItem && bControl)
        {
            m_pSel = hItem;
            _TREEITEM_SetState(*hItem, ~hItem->GetState(), TVIS_SELECTED);
            if (hItem->IsSelected())
                m_pLdownSel = hItem;
        }
        else if (hItem && m_pLdownSel && bShift)
        {
            m_pSel = hItem;
            CTVRec rec(&m_tiRoot);
            void* hEnd = 0;

            for (; ; rec.GlobalNext(0, FWalkStopTVItem()))
            {
                if (!rec)
                {
                    goto BREAK;
                }

                PTVITEM h1 = *rec;
                if (h1 == hItem)
                {
                    hEnd = m_pLdownSel;
                    break;
                }
                else if (h1 == m_pLdownSel)
                {
                    hEnd = hItem;
                    break;
                }

                _TREEITEM_SetState(*h1, 0, TVIS_SELECTED);
            }

            for (; ; rec.GlobalNext(0, FWalkStopTVItem()))
            {
                if (!rec)
                {
                    goto BREAK;
                }

                PTVITEM h1 = *rec;
                _TREEITEM_SetState(*h1, TVIS_SELECTED, TVIS_SELECTED);

                if (h1 == hEnd)
                {
                    break;
                }
            }

            for (; rec.GlobalNext(0, FWalkStopTVItem()), rec;)
            {
                PTVITEM h1 = *rec;
                _TREEITEM_SetState(*h1, 0, TVIS_SELECTED);
            }
BREAK:
            ;
        }
        else
        {
           // TREE_TRACE("select item-- %d", hItem->m_state);
            //Evan Zhu added codes to reslove the mutilselect item by move the left mouse button
            if (hItem)
            {
                if (((hItem->m_state & TVIS_SELECTED) == TVIS_SELECTED)  && ((hItem->m_state & TVIS_CUT) != TVIS_CUT))
                    return false;
                else
                    hItem->m_state = hItem->m_state & ~TVIS_CUT;
            }
            m_pSel = hItem;
        
           // TREE_TRACE("select item-- %d", hItem->m_state);
            for (CTVRec rec(m_tiRoot.GetChild()); rec; rec.GlobalNext(0, FWalkStopTVItem()))
            {
                PTVITEM hi = *rec;
                DbgAssert(hi);
//                 TMUInt uOld = hi->m_state;
//                 TMUInt uNew = (hItem == hi ? (uOld | TVIS_SELECTED) : (uOld & ~TVIS_SELECTED));
                _TREEITEM_SetState(*hi, hItem == hi ? TVIS_SELECTED : 0 , TVIS_SELECTED);
            }
            m_pLdownSel = hItem;
        }

        if (hOld != hItem)
            DoSelChange(hOld, hItem, nKey);
    }
    else
    {
        if (((dwOption & CTVItem::e_soSelMask) == CTVItem::e_soDeselect) && (m_pSel == hItem))
            hItem = 0;
        
        m_pLdownSel = hItem;

        if (m_pSel != hItem)
        {
            if (m_pSel)
                _TREEITEM_SetState(*m_pSel, 0, TVIS_SELECTED);

            m_pSel = hItem;
            
            if (hItem)
                _TREEITEM_SetState(*hItem, TVIS_SELECTED, TVIS_SELECTED);

            DoSelChange(hOld, hItem, nKey);
        }
        else
            return false;
    }

    if (hItem && (dwOption & CTVItem::e_soEnsureVisible))
        _TREEITEM_EnsureVisible(hItem);//, true);

    AutoScroll();
    return hOld != m_pSel;
}

TMBool CTuiTreeImpl::OnKeyDown(TMWParam wParam, TMWParam lParam)
{
    PTVITEM hNew = 0;
    wParam = GetModule()->GetLocal().ConvertKey(wParam);

    switch (wParam)
    {
    case VK_UP:
        if (!m_pSel)
            hNew = GetLastVisible();
        else
            hNew = GetPrevVisible(m_pSel);
        hNew = GetSelfOrPrevEnableVisible(hNew);
        break;

    case VK_LEFT:
        if (!m_pSel)
            return false;
        if (m_pSel->hasChildNodes() && (m_pSel->m_state & (TVIS_EXPANDED | TVIS_DISABLE | TVIS_HIDE)) == TVIS_EXPANDED)
        {
            CTVItem(m_pSel).Collapse();
            return true;
        }
        else
        {
            hNew = m_pSel->GetParent();
            if (hNew == &m_tiRoot)
                return false;
        }
        break;

    case VK_DOWN:
        if (!m_pSel)
            hNew = GetNextVisible(&m_tiRoot);
        else
            hNew = GetNextVisible(m_pSel);
        hNew = GetSelfOrNextEnableVisible(hNew);
        break;

    case VK_RIGHT:
        if (!m_pSel)
            return false;
        if (m_pSel->hasChildNodes() && (m_pSel->m_state & (TVIS_EXPANDED | TVIS_DISABLE | TVIS_HIDE)) != TVIS_EXPANDED)
        {
            CTVItem(m_pSel).Expand();
            return true;
        }
        else
        {
            hNew = GetSelfOrNextEnableVisible(m_pSel->GetChild());
            if (hNew && !m_pSel->IsChild(hNew))
                hNew = 0;
        }
        break;

    case VK_HOME:
        hNew = GetSelfOrNextEnableVisible(GetNextVisible(&m_tiRoot)); // m_tiRoot._child;
        break;

    case VK_END:
        hNew = GetSelfOrPrevEnableVisible(GetLastVisible());
        break;

    case VK_MULTIPLY:
        if (!m_pSel || m_pSel->IsDisable())
            return false;
        _TREEITEM_ExpandAll(m_pSel);
        return true;

    case VK_ADD:
        if (!m_pSel || m_pSel->IsDisable())
            return false;
        CTVItem(m_pSel).Expand();
        return true;

    case VK_SUBTRACT:
        if (!m_pSel || m_pSel->IsDisable())
            return false;
        CTVItem(m_pSel).Collapse();
        return true;

    case VK_PRIOR:
        if (!m_pSel)
            return false;
        else
        {
            GXDips h1 = m_pSel->m_nTop + _TREEITEM_GetHeight(m_pSel);
            GXDips h2 = h1 - m_rcClient.Height();
            PTVITEM it1(m_pSel);
            for (; ;)
            {
                PTVITEM it2 = GetSelfOrPrevEnableVisible(GetPrevVisible(it1));
                if (!it2 || it2->m_nTop < h2)
                    break;
                it1 = it2;
            }
            hNew = it1;
        }
        break;

    case VK_NEXT:
        if (!m_pSel)
            return false;
        else
        {
            GXDips h1 = m_pSel->m_nTop;
            GXDips h2 = h1 + m_rcClient.Height();
            PTVITEM it1(m_pSel);
            for (; ;)
            {
                PTVITEM it2 = GetSelfOrNextEnableVisible(GetNextVisible(it1));
                if (!it2 || (it2->m_nTop > h2))
                    break;
                it1 = it2;
            }
            hNew = it1;
       }
        break;

    case VK_BACK:
        if (!m_pSel)
            return false;
        hNew = m_pSel->GetParent();
        if (hNew == &m_tiRoot)
            return false;
        break;

    case VK_SPACE:
        if (!m_pSel || m_pSel->IsDisable())
            return false;
        if (m_dwStyle & TVS_CHECKBOXES)
        {
            CTVItem ti = CTVItem(m_pSel);
            ti.SetCheck(!ti.IsChecked());
            return true;
        }
        return false;

    default:
        return false;
    }

    if (hNew == 0)
        return (m_dwStyle & TVS_MENUITEM) ? false : true; // false;
    else if (hNew == &m_tiRoot)
        hNew = m_tiRoot.GetChild();

    bool b = SelectItem(hNew, CTVItem::e_soAuto | CTVItem::e_soEnsureVisible, wParam);
    if (b)
        g_mgrTooltip.SetTabToolTip(this);

    CWindowAT(GetHWND()).ResetHideFocus();
    return true;
}

void CTuiTreeImpl::LButtonUp(TMLParam lParam)
{
    CView* ps = GetView();
    TMBool bControl = g_input.m_key.IsKey(VK_CONTROL);
    TMBool bShift   = g_input.m_key.IsKey(VK_SHIFT);

    if (bControl || bShift)
        return ;

     const TUIHITINFO& hi = ps->GetHitInfo();
     PTVITEM ti((PTVITEM)hi.pTaget);
     if (ti && ti->IsSelected())
     { 
         int nState = ti->GetState();
//          _TREEITEM_SetState(*ti, nState| TVIS_CUT, TVIS_MASK);
         SelectItem(ti, CTVItem::e_soEnsureVisible);
     }
}

void CTuiTreeImpl::LButtonDown(TMLParam lParam)
{
    SetFocus(eFocusReasonMouse);

//    TMUInt uFlag = 0;
    const TUIHITINFO& hi = GetView()->GetHitInfo();
    DbgAssert(hi.hWnd == this || IsLifeInvalid());
    PTVITEM ti((PTVITEM)hi.pTaget);
    if (ti)
    {
        SelectItem(ti, CTVItem::e_soAuto | CTVItem::e_soEnsureVisible);
        if (hi.dwCode & TVHT_ONITEMBUTTON)
            _TREEITEM_SetState(*ti, ~ti->GetState(), TVIS_EXPANDED);
        if (hi.dwCode & TVHT_ONCHECKBOX)
            _TREEITEM_SetState(*ti, ~ti->GetState(), TVIS_CHECKED);
    }
}

void CTuiTreeImpl::OnBindModeLButtonDown(TMWParam wParam, TMLParam lParam)
{
    TUIHITINFO hi = GetView()->GetHitInfo();
//     hi.hWnd->SetCapture();
    if (!HitTestTree(&hi, WM_LBUTTONDOWN))
        return;
    SetFocus(eFocusReasonMouse);
    PTVITEM ti((PTVITEM)hi.pTaget);
    if (ti)
        SelectItem(ti, CTVItem::e_soAuto | CTVItem::e_soEnsureVisible);
}

// void CTuiTreeImpl::OnBindModeLButtonUp(TMWParam wParam, TMLParam lParam)
// {
// //     GetView()->ReleaseCapture();
// //     TUIHITINFO hi = GetView()->GetHitInfo();
// //     if (!HitTestTree(&hi, WM_LBUTTONUP))
// //         return;
// //     CTVItem ti((PTVITEM)hi.pTaget);
// //     if (ti)
// //         SelectItem (ti, CTVItem::e_soAuto | CTVItem::e_soEnsureVisible);
// }

void CTuiTreeImpl::RButtonDown(TMLParam lParam)
{
    SetFocus (eFocusReasonMouse);
//    TMUInt uFlag = 0;
    const TUIHITINFO& hi = GetView()->GetHitInfo();
    PTVITEM ti((PTVITEM)hi.pTaget);
    if (ti)
    {
        SelectItem(ti, CTVItem::e_soAuto | CTVItem::e_soEnsureVisible);
//        ti.SelectItem (TVGN_CARET);
    }
}

void CTuiTreeImpl::OnMouseSwitch(TUIHITINFO* pOld, TUIHITINFO* pNew)
{
    if (IsTrackMouse())
        Invalidate();

    // set hint
    if (pNew->hWnd == this) // && (m_dwStyle & TVS_INFOTIP))
    {
        if (g_mgrTooltip.IsTooltipTarget(this))
            g_mgrTooltip.SetToolTip(this, eTTActiveNormal);
    }
    
    if (pNew->hWnd == this && !pNew->bNC)
    {
        // track select
        if (IsMenuItem() || Is_TVS_TRACKSEL())
        {
            PTVITEM hi = (PTVITEM)pNew->pTaget;
            if (hi)
                SelectItem(hi, 0, 0);
        }

        // menu item
        if (IsMenuItem())
            SetFocus(eFocusReasonMenuBar);
    }
}

static void _CTuiTreeImpl_SendDrawItemMessage(TUIDRAWITEM& wdi)
{
    wdi.hWnd->SendMessage(CM_DRAWITEM, 0, (TMLParam)&wdi);
}

// CTuiWgt
TMBool CTuiTreeImpl::NativeInvalidateRect(SPY_INVALIDATE_PARAM(const TUIRect& rc, TM::HTUI hSender))
{
    if (!IsLifeInvalid() && GetOwner()->IsPainting())
        Set_WS_PRI_INVALIDATE();
    return scroll_win::NativeInvalidateRect(SPY_INVALIDATE_PARAM(rc, hSender));
}

void CTuiTreeImpl::NativePrintClient(IGXApp* gapp, TMLParam lParam)
{
    CWinState ws(this);

    TUIDRAWITEM wdi;
    wdi.CtlID = m_dwID;
    wdi.hItem = 0;
    wdi.hWnd = this;
    wdi.hWndBind = 0;
    wdi.tmpData = 0;
    wdi.gxText = 0;
    wdi.gxApp = gapp;
    wdi.pColumn = 0;
    wdi.proxy = 0;

    wdi.dwDrawStage = CDDS_PREPAINT;
    wdi.itemState = ws.m_nFrame;
    GXDips nTemp = m_rcClient.Top() + m_bdPadding.top - GetScrollTop();
    RectSetTop(wdi.rcItem, nTemp);
    nTemp += GetScrollHeight();
    tm_setmax(nTemp, m_rcClient.Bottom());
    RectSetBottom(wdi.rcItem, nTemp);
    _TREEITEM_GetRectLR(this, &wdi.rcItem);
//    wdi.rcItem = m_rcClient;
    wdi.itemData = 0;
    _CTuiTreeImpl_SendDrawItemMessage(wdi);

    CRefPtr<TREEITEM> p(GetFirst());
    if (p)
    {
        TMBool bDirty = Is_WS_PRI_DIRTY();
        DbgAssert(!bDirty);
        GXDips nTopScroll = m_rcClient.Top() + m_bdPadding.top - GetScrollTop();
        GXDips nTop = p->m_nTop;
        FreeMem(m_pFirstVisible, true);
        Reset_WS_PRI_DIRTY();
        Reset_WS_PRI_INVALIDATE();
//        wdi.rcItem.top = hi->m_nTop + m_rcClient.Top() + m_bdPadding.top - GetScrollTop();
        TMBool bToEnd = m_pEnsure ? true : false;
        for (; p && (RectTop(wdi.rcItem) < m_rcClient.Bottom() || bToEnd); p = GetNextVisible(p))
        {
            if (p == m_pEnsure)
                bToEnd = false;
            if (nTop != p->m_nTop || bDirty)
                bToEnd = true;

            GXDips nTempTop = nTop + nTopScroll;
            RectSetTop(wdi.rcItem, nTempTop);
            RectSetHeight(wdi.rcItem, _TREEITEM_GetHeight(p));
            TMBool bInClient = nTempTop < p->m_pOwner->m_rcClient.Bottom();

            wdi.hWndBind = p->m_hBind;
            wdi.hItem = p;
            wdi.itemData = (TMLParam)p->m_lParam;
            wdi.gxText = &(p->m_gxText.p);
            wdi.dwDrawStage = CDDS_ITEMPREPAINT;
            wdi.itemState = 0;
            if (p->m_state & TVIS_SELECTED)
                wdi.itemState |= ODS_SELECTED;
            if (p->m_state & TVIS_DISABLE)
                wdi.itemState |= ODS_DISABLED;

            if (gapp->ClipAndPush(wdi.rcItem, true, eGXPropClip))
            {
                // remove auto-sized bind widget
                if (p->m_hBind && (bInClient || p->m_pOwner->m_pEnsure == p))
                {
                    p->m_hBind->ResetBindItemPosition(wdi.rcItem, SWP_NOREDRAW | SWP_NOLAYOUTSIBLING);
                    if (p->IsAutoHeight() || p->m_hBind->IsAutoHeight())
                    {
                        for (; p->m_hBind->Is_WS_PRI_FRAMECHANGED();)
                        {
                            RectSetHeight(wdi.rcItem, p->m_hBind->GetMarginHeight());
                            p->m_hBind->ResetBindItemPosition(wdi.rcItem, SWP_NOREDRAW | SWP_NOLAYOUTSIBLING, true);
                        }
                        GXDips h = p->m_hBind->GetMarginHeight();
                        if (h != p->m_nHeight)
                        {
                            TREE_TRACE(TM_T("CTuiTreeImpl::set itm height: p:%x; height:%g; txt:%s"), p.p, h, p->m_gxText.GetString().c_str());
                            p->m_nHeight = h;
                            RectSetHeight(wdi.rcItem, p->m_nHeight);
                            p->m_pOwner->Set_WS_PRI_DIRTY();
                        }
                    }
                }

                _CTuiTreeImpl_SendDrawItemMessage(wdi);
                wdi.dwDrawStage = CDDS_ITEMPOSTPAINT;
                _CTuiTreeImpl_SendDrawItemMessage(wdi);
                if (p->m_hBind)
                    CWidgetHelper::DoPrintSelf(gapp, p->m_hBind, c_lpPrint | PRF_BIND);

                gapp->Pop();
            }

            nTop += _TREEITEM_GetHeight(p);
//             wdi.rcItem.top = wdi.rcItem.bottom;
        }
        if (p)
            FreeMem(p, false);
    
        wdi.hWndBind = 0;
        wdi.hItem = 0;
        wdi.itemData = 0;
        wdi.gxText = 0;
        wdi.itemData = 0;
    }

    wdi.dwDrawStage = CDDS_POSTPAINT;
    wdi.itemState = ws.m_nFrame;
    wdi.rcItem = m_rcClient;
    _CTuiTreeImpl_SendDrawItemMessage(wdi);



    if (m_pEnsure)
    {
        GetFirst();
    //         RebuildEnsure();
    //         if (!RebuildEnsure())
    // //         if (!m_pEnsure->IsAutoHeight() || !m_pEnsure->m_hBind || !m_pEnsure->m_hBind->Is_WS_PRI_FRAMECHANGED())
    //             m_pEnsure = 0;
    }

    if (!Is_WS_PRI_INVALIDATE())
        m_pEnsure = 0;
}

void CTuiTreeImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    if (gapp->ClipAndPush(m_rcClient, true, eGXPropClip | eGXPropTextColor))
    {
        TMColor clr = ColorFromSys(COLOR_WINDOWTEXT);
        if (!IsHighContrast())
            clr = GetModule()->ConvertColor(GetObjectID(), clr);
        gapp->SetTextColor(clr);

        NativePrintClient(gapp, lParam);

        //TREE_TRACE ("CTuiTreeImpl::Paint; this:%x; tick:%x", this, GetTickCount());
        gapp->Pop();
    }
}

void CTuiTreeImpl::DrawItem(TUIDRAWITEM* p)
{
    if (p->dwDrawStage == CDDS_PREPAINT)
        RenderSimpleBk(p->gxApp, 0, CAppColor::GetColorWindowBk);
    else if (p->dwDrawStage == CDDS_ITEMPREPAINT)
    {
        PTVITEM hItem = (PTVITEM)(HTVITEM)p->hItem;
        CTVItem itm(hItem);
        int indent = (itm.GetState() & TVIS_NODRAWTEXT) ? 65535 : m_uIndent * (_TREEITEM_GetLevel(hItem) - 1);
//        TMColor clr[] = { TMRGB(0,0,0), ColorFromSys(COLOR_HIGHLIGHTTEXT), TMRGB(0,0,0), TMRGB(128,128,128), TMRGB(0,0,0) };
        RenderListItem(p, m_pFore, indent, 0);//clr);
    }
}

bool CTuiTreeImpl::GetWindowHint(ETTActive ett, CString* str)
{
//     if (!(m_dwStyle & TVS_INFOTIP))
//         return CTuiWgt::GetWindowHint(ett, str);

    CView* view = GetView();
    PTVITEM itm;
    if (ett == eTTActvieTab)
        itm = m_pSel;
    else
    {
        TUIHITINFO& hi = view->GetHitInfo();
        itm = (hi.bNC || hi.hWnd != this) ? 0 : (PTVITEM)hi.pTaget;
    }

    if (itm)
    {
        if (_MakeWindowHInt(str, itm->m_strHint, itm->m_gxText))
            return true;

//         if (itm->m_strHint)
//         {
//             if (str)
//                 *str = itm->m_strHint;
//             return true;
//         }
//         
//         if (Text2Hint(str, view, itm->m_gxText))
//             return true;
    }

    return CTuiWgt::GetWindowHint(ett, str);
}

void CTuiTreeImpl::Reclaim()
{
    struct EXP
    {
        static void exec(PTVITEM h)
        {
            h->m_gxText.Reclaim();
        }
    };
    CTVRec(&m_tiRoot).Enum(EXP::exec);
    scroll_win::Reclaim();
}

TMBool CTuiTreeImpl::ProcessWindowMessage(CTuiWgt* hWnd, TMUInt uMsg,
                                          TMWParam wParam, TMLParam lParam,
                                          TMResult& lResult, TMULong dwMsgMapID)
{
    TMBool bHandle = true;

    switch (uMsg)
    {
    case TVM_REGISTEREVENT:
        m_pSink = (ITVEvent*)lParam;
        break;

//     case TVM_SETPROVIDER:
//         m_pSource = (ITVProvider*)lParam;
//         break;
// 
//     case WM_CREATE:
//         {
//             CTuiTree wt(this);
//             CTVItem ti (wt.GetRootItem());
//             ti.InsertChild (_T("aaa"), TVIS_VISIBLE, 0);
//             ti.InsertChild (_T("bbb"), TVIS_VISIBLE, 0);
//             ti.InsertChild (_T("ccc"), TVIS_VISIBLE, 0);
//         }
// 
// //        TREEVIEW_Create(hwnd, (LPCREATESTRUCTW)lParam);
//         break;

//     case TVM_CREATEDRAGIMAGE:
// //  return TREEVIEW_CreateDragImage(infoPtr, wParam, lParam);
//         break;

    case TVM_RESETCONTENT:
        Reset();
        break;

//     case TVM_EDITLABELA:
//  return (TMResult)TREEVIEW_EditLabel(infoPtr, (HTREEITEM)lParam);
// 
//     case TVM_EDITLABELW:
//  return (TMResult)TREEVIEW_EditLabel(infoPtr, (HTREEITEM)lParam);
// 
//     case TVM_ENDEDITLABELNOW:
//  return TREEVIEW_EndEditLabelNow(infoPtr, (TMBool)wParam);
// 
//     case TVM_ENSUREVISIBLE:
//         EnsureVisible ((PTVITEM)lParam);
//         break;
// 
//      case TVM_EXPAND:
//          lResult = ExpandMsg (wParam, (PTVITEM)lParam);
//          break;

//     case TVM_GETBKCOLOR:
//  return TREEVIEW_GetBkColor(infoPtr);

    case TVM_GETCOUNT:
        lResult = _TREEITEM_GetAllChildCount(&m_tiRoot);
        break;

//     case TVM_GETEDITCONTROL:
//  return TREEVIEW_GetEditControl(infoPtr);

//     case TVM_GETIMAGELIST:
//  return TREEVIEW_GetImageList(infoPtr, wParam);

    case TVM_GETINDENT:
        lResult = m_uIndent;
        break;

//     case TVM_GETINSERTMARKCOLOR:
//  return TREEVIEW_GetInsertMarkColor(infoPtr);

//     case TVM_GETISEARCHSTRINGA:
//  FIXME("Unimplemented msg TVM_GETISEARCHSTRINGA\n");
//  return 0;
// 
//     case TVM_GETISEARCHSTRINGW:
//  FIXME("Unimplemented msg TVM_GETISEARCHSTRINGW\n");
//  return 0;
// 
//     case TVM_GETITEMA:
//  return TREEVIEW_GetItemT(infoPtr, (LPTVITEMEXW)lParam, TMFalse);
// 
//     case TVM_GETITEMW:
//  return TREEVIEW_GetItemT(infoPtr, (LPTVITEMEXW)lParam, TMTrue);
// 
    case TVM_GETITEMHEIGHT:
        lResult = m_uItemHeight;
        break;

//     case TVM_GETITEMRECT:
//  return TREEVIEW_GetItemRect(infoPtr, (TMBool)wParam, (TUIRect*)lParam);
// 
//     case TVM_GETITEMSTATE:
//  return TREEVIEW_GetItemState(infoPtr, (HTREEITEM)wParam, (TMUInt)lParam);
// 
//     case TVM_GETLINECOLOR:
//  return TREEVIEW_GetLineColor(infoPtr);

    case TVM_GETNEXTITEM:
        {
            PTVITEM p = lParam ? (PTVITEM)lParam : &m_tiRoot;

            switch (wParam)
            {
            case TVGN_ROOT: p = &m_tiRoot; break;
            case TVGN_CARET: p = m_pSel; break;
            case TVGN_FIRSTVISIBLE: p = GetFirst(); break;
//            case TVGN_DROPHILITE: p = m_pDrop; break;
            case TVGN_LASTVISIBLE: /*GetFirst();*/ p = GetLastVisible(); break;
            case TVGN_NEXT: p = p->GetNext(); break;
            case TVGN_PREVIOUS: p = p->GetPrev(); break;
            case TVGN_PARENT: p = p->GetParent(); break;
            case TVGN_CHILD: p = p->GetChild(); break;
            case TVGN_NEXTVISIBLE: p = GetNextVisible(p); break;
            case TVGN_PREVIOUSVISIBLE: p = GetPrevVisible(p); break;
//             case TVGN_PREVIOUSEL: p = m_pLdownSel; break;
            default: p = 0; break;
            }

            lResult = (TMResult) p;
        }
        break;

//     case TVM_GETSCROLLTIME:
// //         lResult = m_uScrollTime;
//         break;

//     case TVM_GETTEXTCOLOR:
//         lResult = m_clrText;
//         break;
// 
//     case TVM_GETTOOLTIPS:
//  return TREEVIEW_GetToolTips(infoPtr);
// 
//     case TVM_GETUNICODEFORMAT:
//         return TREEVIEW_GetUnicodeFormat(infoPtr);

    case TVM_GETVISIBLECOUNT:
        lResult = GetHeight() / m_uItemHeight;
        break;

    case CM_HITTEST:
        CTuiWgt::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);
        if (!((TUIHITINFO*)lParam)->bNC)
            HitTest((TUIHITINFO*)lParam, wParam);
        break;

    case TVM_SELECTITEM:
        if ((PTVITEM)lParam != &m_tiRoot)
            SelectItem((PTVITEM)lParam, wParam);
        break;
 
//     case TVM_HITTEST:
//         lResult = (TMResult) HitTest(lParam, (TMUInt*)wParam);
//         break;

//     case TVM_INSERTITEMA:
//  return TREEVIEW_InsertItemT(infoPtr, (LPTVINSERTSTRUCTW)lParam, TMFalse);
// 
//     case TVM_INSERTITEMW:
//  return TREEVIEW_InsertItemT(infoPtr, (LPTVINSERTSTRUCTW)lParam, TMTrue);
// 
//     case TVM_SETBKCOLOR:
//  return TREEVIEW_SetBkColor(infoPtr, (TMColor)lParam);
// 
//     case TVM_SETIMAGELIST:
//  return TREEVIEW_SetImageList(infoPtr, wParam, (HIMAGELIST)lParam);

    case TVM_SETINDENT:
        if (m_uIndent != wParam)
        {
            m_uIndent = wParam;
            Invalidate();
//             TREEVIEW_UpdateSubTree(infoPtr, infoPtr->root);
//             TREEVIEW_UpdateScrollBars(infoPtr);
//             TREEVIEW_Invalidate(infoPtr, NULL);
        }
        break;

//  not support
//     case TVM_SETINSERTMARK:
//     case TVM_SETINSERTMARKCOLOR:
// 
//     case TVM_SETITEMA:
//  return TREEVIEW_SetItemT(infoPtr, (LPTVITEMEXW)lParam, TMFalse);
// 
//     case TVM_SETITEMW:
//         return TREEVIEW_SetItemT(infoPtr, (LPTVITEMEXW)lParam, TMTrue);

//     case TVM_SETLINECOLOR:
//         lResult = m_clrLine;
//         m_clrLine = lParam;
//         break;

    case TVM_SETITEMHEIGHT:
        lResult = m_uItemHeight;
        if (m_uItemHeight != wParam)
        {
            m_uItemHeight = wParam;
//             TREEVIEW_RecalculateVisibleOrder(infoPtr, NULL);
//             TREEVIEW_UpdateScrollBars(infoPtr);
//             TREEVIEW_Invalidate(infoPtr, NULL);
        }
        break;

//  not support
//     case TVM_SETSCROLLTIME:

//     case TVM_SETTEXTCOLOR:
//         lResult = m_clrText;
//         if (m_clrText != lParam)
//         {
//             m_clrText = lParam;
//             Invalidate ();
//         }
//         break;

//     case TVM_SETTOOLTIPS:
//  return TREEVIEW_SetToolTips(infoPtr, (TMHWnd)wParam);
// 
//     case TVM_SETUNICODEFORMAT:
//         return TREEVIEW_SetUnicodeFormat(infoPtr, (TMBool)wParam);

// impl by CTVItem
//     case TVM_SORTCHILDREN:
//     case TVM_SORTCHILDRENCB:

//     case WM_CHAR:
//         return TREEVIEW_ProcessLetterKeys(hwnd, wParam, lParam);
// 
//     case WM_COMMAND: // fot edit control
//  return TREEVIEW_Command(infoPtr, wParam, lParam);
// 
//     case WM_DESTROY:
//  return TREEVIEW_Destroy(infoPtr);

    /* WM_ENABLE */

//     case WM_ERASEBKGND:
//  return TREEVIEW_EraseBackground(infoPtr, (TMHDC)wParam);

    case WM_DESTROY:
        Reset();
        m_tiRoot.m_pOwner = 0;
        bHandle = false;
        break;


//     case WM_GETDLGCODE:
//     return DLGC_WANTARROWS | DLGC_WANTCHARS;

//     case WM_GETFONT:
//  return TREEVIEW_GetFont(infoPtr);
// 
//     case WM_HSCROLL:
//  return TREEVIEW_HScroll(infoPtr, wParam);

    case WM_KEYDOWN:
        bHandle = lResult = OnKeyDown(wParam, lParam);
        break;

    case WM_SETFOCUS:
        if (IsMenuItem() && !m_pSel)
        {
            TMBool b = g_input.m_key.IsKey(VK_UP) || (g_input.m_key.IsKeyTab() < 0); //CKeyboard::IsKeyDownShift() || g_input.IsKeyDown(VK_UP);
            PTVITEM itm = b ? GetLastVisible() : GetNextVisible(&m_tiRoot);
            SelectItem(itm, CTVItem::e_soEnsureVisible, 0);
        }
        bHandle = false;
        break;

    case WM_KILLFOCUS:
        if (IsMenuItem())
            SelectItem(0, 0, 0);
//         TREEVIEW_Invalidate(infoPtr, infoPtr->selectedItem);
//         UpdateWindow(infoPtr->hwnd);
//         TREEVIEW_SendSimpleNotify(infoPtr, NM_KILLFOCUS);
        bHandle = false;
        break;

//     case WM_LBUTTONDBLCLK:
//  return TREEVIEW_LButtonDoubleClick(infoPtr, lParam);

    case WM_LBUTTONDOWN:
        LButtonDown(lParam);
        break;

    case WM_LBUTTONUP:
        LButtonUp(lParam);
        break;

    case CM_MOUSESWITCH:
        OnMouseSwitch((TUIHITINFO*)wParam, (TUIHITINFO*)lParam);
        break;

    case WM_RBUTTONDOWN:
        RButtonDown(lParam);
        bHandle = false;
        break;

//     case CM_MOUSESWITCH:
//         {
//             TUIHITINFO* po = (TUIHITINFO*)wParam;
//             if (po->hWnd == this && po->pTaget)
//                 ((PTVITEM)(po->pTaget))->m_state &= ~TVIS_MOUSEOVER;
//             TUIHITINFO* pn = (TUIHITINFO*)lParam;
//             if (pn->hWnd == this && pn->pTaget)
//                 ((PTVITEM)(pn->pTaget))->m_state |= TVIS_MOUSEOVER;
//         }
//         bHandle = false;
//         break;

    /* WM_MBUTTONDOWN */

//     case WM_MOUSELEAVE:
//  return TREEVIEW_MouseLeave(infoPtr);
// 
//     case WM_MOUSEMOVE:
//         if (infoPtr->dwStyle & TVS_TRACKSELECT)
//             return TREEVIEW_MouseMove(infoPtr, wParam, lParam);
//         else
//             return 0;
// 
//     case WM_NCLBUTTONDOWN:
//         if (infoPtr->hwndEdit)
//             SetFocus(infoPtr->hwnd);
//         goto def;
// 
//     case WM_NCPAINT:
//         if (nc_paint (infoPtr, (HRGN)wParam))
//             return 0;
//         goto def;
// 
//     case WM_NOTIFY:
//  return TREEVIEW_Notify(infoPtr, wParam, lParam);
// 
//     case WM_NOTIFYFORMAT:
//  return TREEVIEW_NotifyFormat(infoPtr, (TMHWnd)wParam, (TMUInt)lParam);

    case CM_DRAWITEM:
        DrawItem((TUIDRAWITEM*)lParam);
        break;

    case CM_SETDELETEDATA:
        m_pfnDelete = (PFNDELETEPROC)lParam;
        break;

    case CM_MSGROUTE:
        {
            TUIMSG* pMsg = (TUIMSG*)lParam;
            bHandle = CWidgetHelper::OnBindModeRoute(this, pMsg, lResult);
            if (!bHandle && pMsg->message == CM_WINDOWPOSCHANGED && ((CTuiWidgetPos*)pMsg->lParam)->IsBindOnSize() && pMsg->hWnd->IsBindOrSub(this) == eBindStateSelf)
            {
                TREEITEM* pItem = pMsg->hWnd->m_pBind->GetOwner(this).m_pTVItem;
                DbgAssert(pItem->m_hBind == pMsg->hWnd);
                _CTVItem_SyncHeight(pItem);
            }
        }
        break;

    case CM_MSGBUBBLE:
        bHandle = CWidgetHelper::OnBindModeBubble(this, (TUIMSG*)(lParam), lResult);
        break;

    case CM_SHOWWINDOW_P:
    case CM_ENABLE_P:
    case CM_SETDPI:
    case CM_LANGUAGECHANGED:
    case CM_RENDERCHANGED:
        {
            struct EXP
            {
                static void exec (PTVITEM h, TMMSG& m)
                {
                    if (h->m_hBind)
                        h->m_hBind->SendBroadcastMessage(m.message, m.wParam, m.lParam);
                }
            };
            TMMSG msg = { 0, uMsg, wParam, lParam };
            CTVRec(&m_tiRoot).Enum2(EXP::exec, msg);
        }
        bHandle = false;
        break;


//     case WM_WINDOWPOSCHANGED:
//         OnWindowPosChanged (uMsg, wParam, lParam, bHandle);
//         break;



//     case WM_SETCURSOR:
//     return TREEVIEW_SetCursor(infoPtr, wParam, lParam);
// 
//     case WM_SETFOCUS:
//     return TREEVIEW_SetFocus(infoPtr);
// 
//     case CM_SETFONT:
//     return TREEVIEW_SetFont(infoPtr, (TMHFont)wParam, (TMBool)lParam);
// 
//     case WM_SETREDRAW:
//         return TREEVIEW_SetRedraw(infoPtr, wParam, lParam);
// 
//     case WM_SIZE:
//     return TREEVIEW_Size(infoPtr, wParam, lParam);
// 
//     case WM_STYLECHANGED:
//     return TREEVIEW_StyleChanged(infoPtr, wParam, lParam);
// 
//     /* WM_SYSCOLORCHANGE */
// 
//     /* WM_SYSKEYDOWN */
// 
//     case WM_TIMER:
//     return TREEVIEW_HandleTimer(infoPtr, wParam);
// 
//     case WM_THEMECHANGED:
//         return theme_changed (infoPtr);
// 
//     case WM_VSCROLL:
//     return TREEVIEW_VScroll(infoPtr, wParam);
// 
//     /* WM_WININICHANGE */
// 
//     case WM_MOUSEWHEEL:
//     if (wParam & (MK_SHIFT | MK_CONTROL))
//         goto def;
//     return TREEVIEW_MouseWheel(infoPtr, wParam);
// 
//     case WM_DRAWITEM:
//     TRACE("drawItem\n");
//     goto def;
// 
//     default:
//     /* This mostly catches MFC and Delphi messages. :(*/
//     if ((uMsg >= WM_USER) && (uMsg < WM_APP))
//         TRACE("Unknown msg %04x wp=%08lx lp=%08lx\n", uMsg, wParam, lParam);
// def:
//     return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    default:
        bHandle = false;
        break;
    }

//     if (!bHandle)
//     {
//         bHandle = scroll_win::ProcessWindowMessage (hWnd, uMsg,
//             wParam, lParam, lResult, dwMsgMapID);
//     }
    if (bHandle)
        return TMTrue;

    CHAIN_MSG_MAP(scroll_win);

    return false;
}

#ifdef TUI_EDIT_MODE

CString CTuiTreeImpl::GetDebugInfo()
{
    CString str;
    str.Format(L"\r\nitem count: %d", m_tiRoot.GetAllChildCount());

    str += L"\r\nitems:";
    CTVItem itm(&m_tiRoot);
    for (int i = 0; (i < 10) && (itm = itm.GetGlobalNext()); ++i)
    {
        str += L"\r\n";
        str.append(itm.GetLevel() * 2, L' ');
        str += "  text:\"";
        str += itm.GetText();
        str += L"\" hint:\"";
        str += itm.GetHint();
        str += L"\"";
    }
    return scroll_win::GetDebugInfo() + str;
}

#endif // TUI_EDIT_MODE

TM_END_NAMESPACE
