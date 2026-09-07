/**
 *    @file
 *    @brief
 *    CWndPicEdit
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2019-6-21
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2019 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "WndobjEdit.h"
#include "ProjMgr.h"
#include "DlgSvgEdit.h"
#include "tmfilename.h"
#include "tmdrag.h"
#include "tmkeyboard.h"
#include "mainwnd.h"

using namespace TM;


///////////////////////////////////////////////////////////////////////////////
// CWndPicEdit
CWndPicEdit::CWndPicEdit()
{
    SetAni(300);
}

CWndPicEdit::~CWndPicEdit()
{
}

TMResult CWndPicEdit::OnPrintEdit(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    IProjMgr* pMgr = m_pPanel;//->GetObjMgr();
    IObjEdit* pObj = pMgr->ItemGetSelectedData(true);
    TM::CTuiRect rc;
    TM::CTuiWidget(pMsg->hWnd).GetClientRect(&rc);
    pObj->RendSample(_gxApp, e_bsEditor, rc, pObj->eRenderForView);
    return 0;
}

TMResult CWndPicEdit::OnPrintTest(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    IProjMgr* pMgr = m_pPanel;//->GetObjMgr();
    IObjEdit* pObj = pMgr->ItemGetSelectedData(true);
    TM::CTuiRect rc;
    TM::CTuiWidget(pMsg->hWnd).GetClientRect(&rc);
    int nFrame = this->GetCurrentFrame();
    int nOld = m_nCurrentFrame;
    m_nCurrentFrame = pObj->RendSample(_gxApp, nFrame, rc, pObj->eRenderForView);

    if (nOld != m_nCurrentFrame)
    {
        CString str;
        str.Format(L"%d", m_nCurrentFrame);
        Item(IDC_STATIC_31244).SetWindowText(str);
    }
    return 0;
}

TMResult CWndPicEdit::OnStep (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    int n = this->GetCurrentFrame();
    if (wID == IDC_PUSHBUTTON_31242)
        n -= 10;
    else if (wID == IDC_PUSHBUTTON_31243)
        --n;
    else if (wID == IDC_PUSHBUTTON_30342)
        ++n;
    else
        n += 10;
    tm_setmax(n, 0);
    this->SetCurrentFrame(n);
    Invalidate();
    return 0;
}

TMResult CWndPicEdit::OnPlay (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiButton but((HTUI)hWndCtl);
    if (but.GetCheck())
        this->Pause();
    else
        this->Resume();
    return 0;
}

TMResult CWndPicEdit::OnEdit (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    IProjMgr* pMgr = m_pPanel;//->GetObjMgr();
    IObjEdit* pObj = pMgr->ItemGetSelectedData(true);
    CDomDocument doc;
    TM::IDomElement* p = pObj->SaveTo(doc);
    TM::CFileName fn(p->getAttribute(L"FileName"));
    if (!fn.IsExists())
        return false;
    CDlgSvgEdit::CPtr ptr;
    ptr.CreateInstance(),
        (ptr->m_strFile = fn),
        ptr->Create(*AppGetEditor(), (HWND)0);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// CWndFrmEdit
TMResult CWndFrmEdit::OnPrintClient (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    IProjMgr* pMgr = m_pPanel;//->GetObjMgr();
    IObjEdit* pObj = pMgr->ItemGetSelectedData(true);
    TM::CTuiRect rc;
    GetClientRect(&rc);
    pObj->RendSample(_gxApp, 0, rc, pObj->eRenderForView);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// CWndWinEdit

struct MOVEPARAM;
struct HITTESTPARAM;

enum
{
    e_htOut     = 0,
    e_htLeft    = 0x001,
    e_htRight   = 0x002,
    e_htTop     = 0x004,
    e_htBottom  = 0x008,
    e_htClient  = e_htLeft | e_htRight | e_htTop | e_htBottom,
};

const int DRAG_BLOK_SIZE = 6;


int                     m_nCurrentHT;
HCURSOR                 m_cursor[e_htClient + 1];
TM::CTuiPoint           m_ptDown;
TM::CTuiPoint           m_ptNow;
TMBool                  m_bSelectMode; // ¿òÑ¡Ä£Ê½
TMBool                  s_bPrinting;



struct MOVEPARAM : public TMSizeF
{
    int ht;
};

static void EnumMoveProc(IProjMgr* mgr, IObjEdit* p, MOVEPARAM* mp)
{
    if (!(mgr->ItemGetState(p) & TVIS_SELECTED))
        return;
    
    CTuiRect rc;
    if (!p->GetObjRect(rc))
        return;
    
    if (p->GetType() != TM::e_otWnd)
    {
        if (mp->ht & e_htLeft)
            rc.left += mp->cx;
        if (mp->ht & e_htTop)
            rc.top += mp->cy;
    }
    if (mp->ht & e_htRight)
        rc.right += mp->cx;
    if (mp->ht & e_htBottom)
        rc.bottom += mp->cy;
    p->SetObjRect(rc);
}

struct HITTESTPARAM : public TUIPoint
{
    int ht;
};

static void EnumHitTestProc(IProjMgr* mgr, IObjEdit* p, struct HITTESTPARAM* mp)
{
    if (mp->ht)
        return;
    
    if (!(mgr->ItemGetState(p) & TVIS_SELECTED))
        return;
    
    CTuiRect rc;
    if (!p->GetObjRect(rc))
        return;
    
    if (rc.PtInRect(*mp))
        mp->ht = e_htClient;
    else
    {
        rc.InflateRect(DRAG_BLOK_SIZE, DRAG_BLOK_SIZE, DRAG_BLOK_SIZE, DRAG_BLOK_SIZE);
        if (!rc.PtInRect(*mp))
            return;
        
        if (mp->y < rc.top + DRAG_BLOK_SIZE)
            mp->ht = e_htTop;
        else if (mp->y >= rc.bottom - DRAG_BLOK_SIZE)
            mp->ht = e_htBottom;
        
        if (mp->x < rc.left + DRAG_BLOK_SIZE)
            mp->ht |= e_htLeft;
        else if (mp->x >= rc.right - DRAG_BLOK_SIZE)
            mp->ht |= e_htRight;
    }
}

template <class F, class P>
inline void Pad_EnumItems(CWndPanel* pm, IObjEdit* p, F __func, P __param)
{
    __func(pm, p, __param);
    for (p = pm->ItemGetChild(p); p; p = pm->ItemGetNext(p))
        Pad_EnumItems(pm, p, __func, __param);
}

CWndWinEdit* CWndWinEdit::Init(CWndPanel* p)
{
    m_pPanel = p;
    HINSTANCE hMdl = _Module.GetResourceInstance();
    m_rpic = (ITuiPic*)p->GetAPP()->GetPic(hMdl, IDP_BMP_SELECT, TUI_DPI_SYSTEM_DEFAULT);

    if (!m_cursor[e_htOut])
    {
        m_cursor[e_htOut] = LoadCursor(0, IDC_ARROW);
        m_cursor[e_htClient] = LoadCursor(0, IDC_SIZEALL);
        
        m_cursor[e_htLeft] = m_cursor[e_htRight] = LoadCursor(0, IDC_SIZEWE);
        m_cursor[e_htTop] = m_cursor[e_htBottom] = LoadCursor(0, IDC_SIZENS);
        
        m_cursor[e_htLeft | e_htTop] = m_cursor[e_htRight | e_htBottom] = LoadCursor(0, IDC_SIZENWSE);
        m_cursor[e_htLeft | e_htBottom] = m_cursor[e_htRight | e_htTop] = LoadCursor(0, IDC_SIZENESW);
    }
    return this;
}

TMResult CWndWinEdit::OnDragLeave (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Invalidate();
    return 0;//GetApp()->GetDragMgr().DoDragLeave(m_hWnd);
}

TMResult CWndWinEdit::OnDragEntry (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Invalidate();
    TM::DROPEVENT* pEvent = (TM::DROPEVENT*)lParam;
    
    if (pEvent->pData->Query(DRAG_WND_CLASS))
    {
        return DROPEVENT_KEY2EFF(pEvent) | eDragEffCopy;
    }
    return 0;
}

static int ControlHeight(const TM::CString& strName)
{
    return CTRL_HEIGHT;
}

static IObjEdit* ChildFromParent(IProjMgr* pMgr, IObjEdit* p, IObjEdit* itmStop, CTuiPoint pt)
{
    if (!p)
        return 0;
    
    CTuiRect rc;
    if (!p->GetObjRect(rc))
        return 0;
    
    if (rc.PtInRect(pt))
    {
        for (IObjEdit* pc = pMgr->ItemGetChild(p); pc; pc = pMgr->ItemGetNext(pc))
        {
            IObjEdit* itmFind = ChildFromParent(pMgr, pc, itmStop, pt);
            if (itmFind)
                return itmFind == itmStop ? p : itmFind;
        }
        return p;
    }
    return 0;
}

TMResult CWndWinEdit::OnDragDrop (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TM::DROPEVENT* pEvent = (TM::DROPEVENT*)lParam;
    
    TM::CString strClass;
    if (pEvent->pData->GetString(strClass, DRAG_WND_CLASS) && !strClass.IsEmpty())
    {
        int nHeight = ControlHeight (strClass);
        CPoint ptx(ToPos(pEvent->ptWindow));
        RECT rcTemp = { ptx.x, ptx.y, ptx.x + CTRL_WIDTH, ptx.y + nHeight };
        
        IProjMgr* pMgr = m_pPanel;//->GetProjMgr();
        IObjEdit* itmCur = pMgr->ItemGetSelectedData(false);
        IObjEdit* itmOwner = pMgr->ItemGetSelectedData(true);
        IObjEdit* itmSel = ChildFromParent(pMgr, itmOwner, itmCur, ptx);
        if (!itmSel)
            return 0;
        
//        DbgAssert(hc->m_pEditData);
        m_pPanel->ItemCreate(itmSel, strClass, &rcTemp);
        return pEvent->dwEffect;
    }
    Invalidate();
    return 0;//GetView()->GetDragMgr().DoEvent(m_hWnd, uMsg, wParam, lParam);
}

struct PAINTPARAM : public CTuiRect
{
    IGXApp* gapp;
    CWndWinEdit* edit;
};

static void EnumPaint(IProjMgr* pm, IObjEdit* p, PAINTPARAM* pp)
{
    if (pm->ItemGetState(p) & TVIS_SELECTED)
    {
        CTuiRect rc;
        if (p->GetObjRect(rc))
        {
            rc.OffsetRect(pp->left, pp->top);
            rc.InflateRect(DRAG_BLOK_SIZE, DRAG_BLOK_SIZE, DRAG_BLOK_SIZE, DRAG_BLOK_SIZE);
            pp->edit->m_rpic->Render(0, 0, rc, 0, pp->gapp);
        }
    }
};

TMResult CWndWinEdit::OnPrintClient(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    s_bPrinting = true;

    IProjMgr* pMgr = m_pPanel;//->GetObjMgr();
    IObjEdit* pObj = pMgr->ItemGetSelectedData(true);

    PAINTPARAM pp;
    pObj->GetObjRect(pp);
    SetScrollRange(pp.Width(), pp.Height());
    GetPaddingRect(&pp);

    CTuiWidget wndParent(GetParent());
    CTuiRect rcParent;
    wndParent.GetWindowRect(&rcParent);
    tm_setmin(pp.right, rcParent.right);
    tm_setmin(pp.bottom, rcParent.bottom);

    IGXApp* gapp = _gxApp;
    pp.gapp = gapp;
    pp.edit = this;
    pObj->RendSample(gapp, 0, pp, pObj->eRenderForView);
    Pad_EnumItems(m_pPanel, pObj, EnumPaint, &pp);
    
    if (m_bSelectMode)
    {
        CTuiRect rc(m_ptDown, m_ptNow);
        rc.NormalizeRect();
        if (!rc.IsRectEmpty())
        {
            rc.OffsetRect(pp.Left(), pp.Top());
            gapp->FrameRect(rc, 1, TMRGB(0,255,128));
        }
    }

    s_bPrinting = false;
    return 0;
}

TMResult CWndWinEdit::OnKeyDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    const int nStep = 1;
    int dv = 0;
    int dh = 0;
    
    switch (wParam)
    {
    case VK_DELETE:
        m_pPanel->DeleteSelect();
        //         DeleteCurWindow();
        return 0;
        
    case VK_UP:
        dv -= nStep;
        break;
        
    case VK_DOWN:
        dv += nStep;
        break;
        
    case VK_LEFT:
        dh -= nStep;
        break;
        
    case VK_RIGHT:
        dh += nStep;
        break;
        
    default:
        return true;
    }
    
    MOVEPARAM mp;
    mp.cx = dh;
    mp.cy = dv;
    CKeyInput key(wParam, lParam);
    if (key.IsModifiersShift())
        mp.ht = e_htLeft | e_htTop;
    else if (key.IsModifiersControl())
        mp.ht = e_htRight | e_htBottom;
    else
        mp.ht = e_htClient;
    Pad_EnumItems(m_pPanel, m_pPanel->ItemGetSelectedData(true), EnumMoveProc, &mp);
    
    return true;
}

TMResult CWndWinEdit::OnMouseMove(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    int n = m_nCurrentHT;
    SetCursor(m_cursor[n]);

    IProjMgr* pMgr = m_pPanel;//->GetObjMgr();
    IObjEdit* pCur = pMgr->ItemGetSelectedData(true);

    if (IsCapture())
    {
        CTuiPoint pt(ToPos(lParam));
        if (pt != m_ptNow)
        {
            TM::CTuiRect rc(m_ptDown, pt);
//             CDlgZoom* pZoom = m_pPanel->GetMagnify();
//             pZoom->TryCreate(pCur);
//             pZoom->MoveTo(rc);
            
            if (m_bSelectMode)
            {
//                GetEditor()->Invalidate();
                m_ptNow = pt;
                m_pPanel->Invalidate();
            }
            else
            {
                CTuiSize sz = pt - m_ptNow;
                m_ptNow = pt;
                MOVEPARAM mp;
                TMSizeF& _mp = mp;
                _mp = sz;
                mp.ht = m_nCurrentHT;
                Pad_EnumItems(m_pPanel, pCur, EnumMoveProc, &mp);
            }
        }
    }
    else
    {
        HITTESTPARAM hp;
        CTuiPoint(ToPos(lParam)).CopyTo(hp);
        hp.ht = 0;
        Pad_EnumItems(m_pPanel, pCur, EnumHitTestProc, &hp);
        m_nCurrentHT = hp.ht;
    }
    return 0;
}


TMResult CWndWinEdit::OnLButtonDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_ptDown = ToPos(lParam);
    m_ptNow = ToPos(lParam);

    SetFocus(eFocusReasonMouse);

//     if (m_nCurrentHT == e_htOut)
//         return 0;

    CKeyInput key = _tuiApp.GetKeyInput();
    if (key.IsKey(VK_SHIFT) || key.IsKey(VK_CONTROL) || !m_nCurrentHT)
        m_bSelectMode = true;
    else if (m_nCurrentHT)
        m_bSelectMode = false;
    SetCapture();
    return 0;
}

static void EnumDeselect(CWndPanel* pMgr, IObjEdit* p, int)
{
    pMgr->SelectItem(p, CTVItem::e_soDeselect);
}

static void EnumSelect(CWndPanel* pMgr, IObjEdit* p, CTuiRect* prc)
{
    if (!p)
        return;
    CTuiRect rcw;
    if (p->GetObjRect(rcw) && ((*prc & rcw) == rcw))
        pMgr->SelectItem(p, CTVItem::e_soCtrl);
}

void CWndWinEdit::DoSelect()
{
    IProjMgr* pMgr = m_pPanel;//->GetObjMgr();
    IObjEdit* itmCur = pMgr->ItemGetSelectedData(false);
    IObjEdit* itmOwner = pMgr->ItemGetSelectedData(true);
    IObjEdit* itmSel = ChildFromParent(pMgr, itmOwner, itmCur, m_ptNow);
    if (!itmSel)
        Pad_EnumItems(m_pPanel, itmOwner, EnumDeselect, 0);
    else
        m_pPanel->SelectItem(itmSel, CTVItem::e_soAuto | CTVItem::e_soEnsureVisible);
}

CTuiPoint CWndWinEdit::ToPos(const CTuiPoint& lp)
{
    CTuiPoint ptCur(lp);
    CTuiRect rc;
    GetPaddingRect(&rc);
    ptCur.Offset(-rc.Left(), -rc.Top());
    return ptCur;
}

TMResult CWndWinEdit::OnLButtonUp (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (IsCapture())
    {
        IProjMgr* pMgr = m_pPanel;//->GetObjMgr();
        IObjEdit* itmCur = pMgr->ItemGetSelectedData(false);
//         CDlgZoom* pZoom = m_pPanel->GetMagnify();
//         if (!pZoom->TryDestroy(itmCur))
        DoSelect();
        //m_hWnd.GetAPP()->SetCapture(0);
        ReleaseCapture();

        if (m_bSelectMode)
        {
            CTuiPoint ptd(m_ptDown);
            CTuiPoint ptn(m_ptNow);
            CTuiRect rc(ptd, ptn);
            rc.NormalizeRect();
            m_pPanel->Invalidate();

//            CObjItem p = _Module.GetMainWnd()->GetSelectedItem(true);
            Pad_EnumItems(m_pPanel, pMgr->ItemGetSelectedData(true), EnumSelect, &rc);
//            TM::CTreeItemT<CTuiWgt>(m_hWnd).Enum2(CEnumSelect(), rc);
        }
        m_ptNow = m_ptDown;
        m_pPanel->OnSelector(m_ptNow);
    }

    return 0;
}
