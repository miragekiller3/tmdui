/*
 *  @file
 *  @brief  frame: form 
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2012.02.08
 *
 *  Copyright (C) 2012 miragekiller
 */

#pragma once

#ifndef FRAMEFORM_H 
#define FRAMEFORM_H

#include "frame.h"
#include "pic.h"
#include "txtpic.h"


TM_BGN_NAMESPACE


enum { eDftInnerBorderWidth = 2 };
enum { eDftOutterBorderWidth = 4 };


////////////////////////////////////////////////////////////////////////////////
// class CFormCtrl
struct CFormCtrl
{
public:
    TMBool IsDisable() { return false; }

    void Align(TMUInt32 dwStyle, CTuiRect& rc1, CTuiRect& rcNext, GXDips n);

    template <class T>
    static TMBool DoMouse(T& ctrl, IFrameSource* pSender, EHitID hid, TMUInt uMsg, TMWParam wParam, TMLParam lParam)
    {
        TMBool b = false;
        switch (uMsg)
        {
        case CM_MOUSEENTRY:
        case CM_NCMOUSEENTRY:
            b = ctrl.OnMouseEntry(pSender, wParam, lParam);
            break;
            
        case WM_MOUSELEAVE:
        case CM_NCMOUSELEAVE:
            b = ctrl.OnMouseLeave(pSender, wParam, lParam);
            break;
            
        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
            b = ctrl.OnMouseMove(pSender, lParam, hid);
            break;
            
        case WM_LBUTTONDOWN:
        case WM_NCLBUTTONDOWN:
            b = ctrl.OnLButtonDown(pSender, lParam, hid);
            break;
            
        case WM_LBUTTONUP:
        case WM_NCLBUTTONUP:
            b = ctrl.OnLButtonUp(pSender, lParam, hid);
            break;
            
        case WM_LBUTTONDBLCLK:
        case WM_NCLBUTTONDBLCLK:
            b = ctrl.OnLButtonDblClk(pSender, lParam, hid);
            break;

        case WM_CAPTURECHANGED:
            b = ctrl.OnCaptureChanged();
            break;
        }

        if (b)
            pSender->Update();
        return b;
    }

    TMBool OnMouseEntry(IFrameSource* pSender, TMWParam wParam, TMLParam lParam) { return false; }
    TMBool OnMouseLeave(IFrameSource* pSender, TMWParam wParam, TMLParam lParam) { return false; }
    TMBool OnMouseMove(IFrameSource* pSender, TMLParam lParam, EHitID hid) { return false; }
    TMBool OnLButtonDown(IFrameSource* pSender, TMLParam lParam, EHitID hid) { return false; }
    TMBool OnLButtonUp(IFrameSource* pSender, TMLParam lParam, EHitID hid) { return false; }
    TMBool OnLButtonDblClk(IFrameSource* pSender, TMLParam lParam, EHitID hid) { return false; }
    TMBool OnCaptureChanged() { return false; }
};


////////////////////////////////////////////////////////////////////////////////
// class CFormButtonT
template <EHitID nHitID, TMUInt32 dwStyle, int nSCID>
struct CFormButtonT : public CFormCtrl
{
public:
    CPicPtr     m_pBk;
    typedef CFormCtrl base_class;

public:
    CFormButtonT()
    {
    }

    void FinalConstruct(CModule& mdl)
    {
        m_pBk = GetDftPic(mdl);
    }

    ITuiPic* GetDftPic(CModule& mdl)
    {
        if (nSCID == SC_CLOSE)
            return GetPrePic2(mdl, CLOSE);
        else if (nSCID == SC_MINIMIZE)
            return GetPrePic2(mdl, MIN);
        else if (nSCID == SC_MAXIMIZE)
            return GetPrePic2(mdl, MAX);
        else if (nSCID == SC_CONTEXTHELP)
            return GetPrePic2(mdl, HELP);
        DbgAssert(0);
        return 0;
    }

    TMBool IsVisible (IFrameSource* pSender)
    {
        TMULong ds = pSender->FSGetStyle();
        if (nHitID == eHitIDHelp)
            return (ds & WS_CAPTION) && (pSender->FSGetExStyle() & dwStyle) ;
        else
            return (ds & WS_CAPTION) && (ds & dwStyle) ;
    }

    TMBool OnMouseEntry(IFrameSource* pSender, TMWParam wParam, TMLParam lParam)
    {
//        pSender->m_aniList.AddEntryMixAni(lParam, 0, GetCheck(pSender), false, false);
        pSender->FSGetWin()->m_aniList.AddSysButAni(*(TUIHITINFO*)lParam, e_bssNormal, e_bssHigh, GetCheck(pSender));
        pSender->FSGetWin()->InvalidateRect(((TUIHITINFO*)lParam)->rcObj);
        return false;
    }
    
    TMBool OnMouseLeave(IFrameSource* pSender, TMWParam wParam, TMLParam lParam)
    {
//         pSender->m_aniList.AddLeaveMixAni(wParam, 0, GetCheck(pSender), false, false);
        pSender->FSGetWin()->m_aniList.AddSysButAni(*(TUIHITINFO*)wParam, e_bssHigh, e_bssNormal, GetCheck(pSender));
        pSender->FSGetWin()->Invalidate();//Rect(((TUIHITINFO*)wParam)->rcObj);
        return false;
    }

    TMBool OnLButtonDown(IFrameSource* pSender, TMLParam lParam, EHitID hid)
    {
        return true;
    }
    
    TMBool OnLButtonUp(IFrameSource* pSender, TMLParam lParam, EHitID hid)
    {
        return false;
    }

    EButStates GetButtonStates(IFrameSource* pSender)
    {
        EButStates buts = (EButStates)GetCheck(pSender);
        if (IsDisable())
            return buts | e_bssDisable;
        
        HTUI hWnd = pSender->FSGetWin();
        CView* ps = hWnd->GetView();
        if (ps->GetOver() != hWnd || ps->GetHitInfo().hid != nHitID)
            return buts | e_bssNormal;
        
        return buts | (ps->IsMouseDown() ? e_bssDown : e_bssHigh);
    }

    int GetCheck(IFrameSource* pSender)
    {
        int ebs = 0;
        int nAdd = 1;
        
        if (nHitID == eHitIDMax)
        {
            ++nAdd;
            if (pSender->IsCanRestore())
                ++ebs;
        }
        if (pSender->FSIsActive())
            ebs += nAdd;
        return ebs;
    }

    void Paint(IFrameSource* pSender, IGXApp* gapp, CTuiRect& rc)
    {
        if (rc.Width() <= 0 || rc.Height() <= 0)
            return;

        if (m_pBk)
        {
            EButStates ebss = GetButtonStates(pSender);
            int        ebs = ButStates2ButFrames(ebss);
            HTUI hWnd = pSender->FSGetWin();
            CModule* pm = hWnd->GetModule();
            if (hWnd->GetView()->IsHighContrast())
                GetDftPic(*pm)->Render(ebs, ebss, rc, gapp);
            else
                hWnd->m_aniList.AniRender(gapp, nHitID, m_pBk, ebs, ebss, rc);// m_pBk.Render (pSender, ebs, hDC, rc);
        }
    }
};

typedef CFormButtonT<eHitIDClose, WS_SYSMENU, SC_CLOSE>                CFormClose;
typedef CFormButtonT<eHitIDMin,   WS_MINIMIZEBOX, SC_MINIMIZE>         CFormMin;
typedef CFormButtonT<eHitIDMax,   WS_MAXIMIZEBOX, SC_MAXIMIZE>         CFormMax;
typedef CFormButtonT<eHitIDHelp,  WS_ADV_CONTEXTHELP, SC_CONTEXTHELP>  CFormHelp;


////////////////////////////////////////////////////////////////////////////////
// CFormSys
class CFormSys : public CFormCtrl
{
public:
    CPicPtr    m_pBk;
    
public:
    void FinalConstruct(CModule& mdl) {}
    TMBool IsVisible (IFrameSource* pSender);
    TMBool OnLButtonDown(IFrameSource* pSender, TMLParam lParam, EHitID hid);
    TMBool OnLButtonDblClk(IFrameSource* pSender, TMLParam lParam, EHitID hid);
    void Paint(IFrameSource* pSender, IGXApp* gapp, CTuiRect& rc);
};

////////////////////////////////////////////////////////////////////////////////
// CFormBorderBase
struct CFormBorderBase : public CFormCtrl
{
public:
    CPicPtr m_pBk;
    TMInt16 m_nLeft;
    TMInt16 m_nTop;
    TMInt16 m_nRight;
    TMInt16 m_nBottom;

    CFormBorderBase(int n)
        : m_nLeft(n)
        , m_nTop(n)
        , m_nRight(n)
        , m_nBottom(n)
    {
    }

    void BorderNextRect(CTuiRect& rc)
    {
        rc.InflateRect(-m_nLeft, -m_nTop, -m_nRight, -m_nBottom);
    }
};


////////////////////////////////////////////////////////////////////////////////
// CFormOuterBorder
struct CFormOuterBorder : public CFormBorderBase
{
public:
    int m_nCorner;

public:
    CFormOuterBorder();
    void FinalConstruct(CModule& mdl);
    TMBool IsVisible(IFrameSource* pSender);
    TMBool IsVisible(TMUInt32 dwStyle);
    void NextRect(IFrameSource* pSender, CTuiRect& rc);
    EHitID HitTestO(IFrameSource* pSender, CTuiRect& rc, const TUIHITINFO& ht);
    TMBool OnLButtonDown(IFrameSource* pSender, TMLParam lParam, EHitID hid);
    CTuiRect GetBorder(int nEXt);
    void Paint(IFrameSource* pSender, IGXApp* gapp, TMUInt32 dwFStyle, CTuiRect& rc);
};


////////////////////////////////////////////////////////////////////////////////
// CFormInnerBorder
struct CFormInnerBorder : public CFormBorderBase
{
public:
    CFormInnerBorder();
    void FinalConstruct(CModule& mdl);
    TMBool IsVisible(IFrameSource* pSender);
    TMBool IsVisible(TMULong dwExtStyle);
    void NextRect(CTuiRect& rc);
    void HitTestI(IFrameSource* pSender, const TUIPoint& pt, CTuiRect& rc, EHitID& hid);
    void Paint(IFrameSource* pSender, IGXApp* gapp, TMUInt32 dwFStyle, CTuiRect& rc);
};


////////////////////////////////////////////////////////////////////////////////
// ButT
template <class T>
struct ButT : public T
{
public:
    enum { eDftFrmButWdith = 19 };
    enum { eDftFrmButHeight = 17 };
    int     m_nLeft;
    int     m_nTop;
    short   m_nWidth;
    short   m_nHeight;
    
    ButT(int l)
        : m_nLeft(l)
        , m_nTop(1)
        , m_nWidth(eDftFrmButWdith)
        , m_nHeight(eDftFrmButHeight)
    {
    }
    
    void GetRect(const CTuiRect& rc, CTuiRect& rc2)
    {
        if (m_nLeft < 0)
            rc2.SetLeft(rc.Right() + m_nLeft);
        else
            rc2.SetLeft(rc.Left() + m_nLeft);
        
        if (m_nTop < 0)
            rc2.SetTop(rc.Bottom() + m_nTop);
        else
            rc2.SetTop(rc.Top() + m_nTop);
        
        rc2.SetWidth(m_nWidth);
        rc2.SetHeight(m_nHeight);
    }
    
    void Paint(IFrameSource* pSender, IGXApp* gapp, const CTuiRect& rcCaption)
    {
        if (T::IsVisible(pSender) && m_nWidth > 0 && m_nHeight > 0)
        {
            CTuiRect rc2;
            GetRect(rcCaption, rc2);
            T::Paint(pSender, gapp, rc2);
        }
    }
    
    TMBool HitTest(IFrameSource* pSender, const CTuiRect& rcCaption, TUIHITINFO& ht)
    {
        if (T::IsVisible(pSender))
        {
            CTuiRect rc2;
            GetRect(rcCaption, rc2);
            if (rc2.PtInRect(ht.pt))
            {
                ht.rcObj = rc2;
                return true;
            }
        }
        return false;
    }
};

////////////////////////////////////////////////////////////////////////////////
// CFormCaption
class CFormCaption : public CFormCtrl
{
public:
    enum { eDftCaptionHeight = 20 };
    CPicPtr             m_pBk;
    TMUInt              m_uFormat;
    GXFont              m_font;
    CPropColor          m_clrFont[2];
    TMInt16             m_nHeight;
    TMInt16             m_nMarginLeft;
    TMInt16             m_nMarginTop;
    TMInt16             m_nMarginRight;
    TMInt16             m_nMarginBottom;

    ButT<CFormClose>  m_ctrlClose;
    ButT<CFormMin>    m_ctrlMin;
    ButT<CFormMax>    m_ctrlMax;
    ButT<CFormHelp>   m_ctrlHelp;
    ButT<CFormSys>    m_ctrlSys;

public:
    CFormCaption();
    ~CFormCaption();
    void FinalConstruct(CModule& mdl);
    TMBool IsVisible(IFrameSource* pSender);
    TMBool IsVisible(TMUInt32 dwStyle);
    void NextRect(TMUInt32 dwFStyle, CTuiRect& rc);
    EHitID HitTestC(IFrameSource* pSender, TMUInt32 dwFStyle, CTuiRect& rc, TUIHITINFO& ht);
    TMBool OnLButtonDown(IFrameSource* pSender, TMLParam lParam, EHitID hid);
    TMBool OnLButtonDblClk(IFrameSource* pSender, TMLParam lParam, EHitID hid);
    void Paint(IFrameSource* pSender, IGXApp* gapp, TMUInt32 dwFStyle, CTuiRect& rc);

};


////////////////////////////////////////////////////////////////////////////////
// CFormScroll
struct CFormScroll
    : public CFormCtrl
{
public:
    int         m_nBarWidth;
    int         m_nButtonHeight;
    int         m_nThumbMin;
    CPicPtr     m_pSizeBox;
    CPicPtr     m_pBk;

private:
    void PrintScroll(IGXApp* gapp, IFrameSource* pWnd, EHitObj nsBar,
        const TUISCROLLINFO& info, CTuiRect& rc, GXDips nHeight, TMBool bVisible, TMBool bForeObj);

public:
    CFormScroll();
    ~CFormScroll();
    void FinalConstruct(CModule& mdl);
    void NextRect(IFrameSource* pSender, CTuiRect& rc);
    TMBool IsScrollBarShown(IFrameSource* pSender);
    void GetPos(struct SPos& sp, const TUISCROLLINFO& info, GXDips nLen);
    TMBool HitTestS(IFrameSource* pSender, CTuiRect& rcx, TUIHITINFO& ht, EHitID& hid);
    void Paint(IFrameSource* pSender, IGXApp* gapp, const CTuiRect& rcx, TMBool bForeObj = false);
    void InvalidHScroll(IFrameSource* pWnd);
    void InvalidVScroll(IFrameSource* pWnd);
    TMBool OnMouseLeave(IFrameSource* pSender, TMWParam wParam, TMLParam lParam);
    TMBool OnMouseEntry(IFrameSource* pSender, TMWParam wParam, TMLParam lParam);
    TMBool OnMouseMove(IFrameSource* pSender, TMLParam lParam, EHitID hid);
    TMBool OnLButtonDown(IFrameSource* pSender, TMLParam lParam, EHitID hit);
    TMBool OnLButtonUp(IFrameSource* pSender, TMLParam lParam, EHitID hid);
    TMBool OnLButtonDblClk(IFrameSource* pSender, TMLParam lParam, EHitID hid);
    TMBool OnCaptureChanged();
};

////////////////////////////////////////////////////////////////////////////////
// CScrollCtrl; not for frame, only  for CTuiScrollObj.
class CScrollCtrl : public CFormScroll
{
public:
    TMBool ProcessFrameMessage(
        IFrameSource* pSender, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult);
};


////////////////////////////////////////////////////////////////////////////////
// CFormManage
class CFormManage
    : public CFrame
{
protected:
    TMUInt32        m_dwFrameStyle;
    CFormOuterBorder m_ctrlOBorder;
    CFormCaption     m_ctrlCaption;
    CFormInnerBorder m_ctrlIBorder;
    CFormScroll      m_ctrlScroll;

    friend class CTuiTrackBarImpl;
    
public:
    enum { FS_ALIGN_LEFT            = ALIGN_LEFT };
    enum { FS_ALIGN_RIGHT           = ALIGN_RIGHT };
    enum { FS_ALIGN_TOP             = ALIGN_TOP };
    enum { FS_ALIGN_BOTTOM          = ALIGN_BOTTOM };

    TM_BGN_MASK_SYMBOL  (CSymbol_Style)
        TM_MASK_SYMBOL2 (FS_ALIGN_LEFT, ALIGN_MASK)
        TM_MASK_SYMBOL2 (FS_ALIGN_RIGHT, ALIGN_MASK)
        TM_MASK_SYMBOL2 (FS_ALIGN_TOP, ALIGN_MASK)
        TM_MASK_SYMBOL2 (FS_ALIGN_BOTTOM, ALIGN_MASK)
    TM_END_ENUM_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwFrameStyle, CSymbol_Style)
        //OBJ_PROP_ENUM   ("CaptionAlign", m_ctrlCaption.m_dwLayout, CSymbol_Align)

        OBJ_PROP_PIC    ("OutterBk", m_ctrlOBorder.m_pBk)
        OBJ_PROP_INT    ("OutterLeft", m_ctrlOBorder.m_nLeft)
        OBJ_PROP_INT    ("OutterRight", m_ctrlOBorder.m_nRight)
        OBJ_PROP_INT    ("OutterTop", m_ctrlOBorder.m_nTop)
        OBJ_PROP_INT    ("OutterBottom", m_ctrlOBorder.m_nBottom)
        OBJ_PROP_INT    ("OutterExtCorner", m_ctrlOBorder.m_nCorner)

        OBJ_PROP_PIC    ("CaptionBk", m_ctrlCaption.m_pBk);
        OBJ_PROP_INT    ("CaptionHeight", m_ctrlCaption.m_nHeight);
        OBJ_PROP_MASK   ("CaptionTextFormat", m_ctrlCaption.m_uFormat, CTxtObject::CSymbol_Format)
        OBJ_PROP_CLASS  ("CaptionFont", m_ctrlCaption.m_font)
        OBJ_PROP_CLASS  ("CaptionTextColor", m_ctrlCaption.m_clrFont[0])
        OBJ_PROP_CLASS  ("CaptionActiveTextColor", m_ctrlCaption.m_clrFont[1])
        OBJ_PROP_INT    ("CaptionLeftMargin",     m_ctrlCaption.m_nMarginLeft)
        OBJ_PROP_INT    ("CaptionTopMargin",      m_ctrlCaption.m_nMarginTop)
        OBJ_PROP_INT    ("CaptionRightMargin",    m_ctrlCaption.m_nMarginRight)
        OBJ_PROP_INT    ("CaptionBottomMargin",   m_ctrlCaption.m_nMarginBottom)

        OBJ_PROP_PIC    ("SysBk", m_ctrlCaption.m_ctrlSys.m_pBk);
        OBJ_PROP_INT    ("SysLeft",   m_ctrlCaption.m_ctrlSys.m_nLeft)
        OBJ_PROP_INT    ("SysTop",   m_ctrlCaption.m_ctrlSys.m_nTop)
        OBJ_PROP_INT    ("SysWidth",   m_ctrlCaption.m_ctrlSys.m_nWidth)
        OBJ_PROP_INT    ("SysHeight",   m_ctrlCaption.m_ctrlSys.m_nHeight)

        OBJ_PROP_PIC    ("CloseBk", m_ctrlCaption.m_ctrlClose.m_pBk);
        OBJ_PROP_INT    ("CloseLeft",   m_ctrlCaption.m_ctrlClose.m_nLeft)
        OBJ_PROP_INT    ("CloseTop",   m_ctrlCaption.m_ctrlClose.m_nTop)
        OBJ_PROP_INT    ("CloseWidth",   m_ctrlCaption.m_ctrlClose.m_nWidth)
        OBJ_PROP_INT    ("CloseHeight",   m_ctrlCaption.m_ctrlClose.m_nHeight)

        OBJ_PROP_PIC    ("MaxBk", m_ctrlCaption.m_ctrlMax.m_pBk);
        OBJ_PROP_INT    ("MaxLeft",   m_ctrlCaption.m_ctrlMax.m_nLeft)
        OBJ_PROP_INT    ("MaxTop",   m_ctrlCaption.m_ctrlMax.m_nTop)
        OBJ_PROP_INT    ("MaxWidth",   m_ctrlCaption.m_ctrlMax.m_nWidth)
        OBJ_PROP_INT    ("MaxHeight",   m_ctrlCaption.m_ctrlMax.m_nHeight)

        OBJ_PROP_PIC    ("MinBk", m_ctrlCaption.m_ctrlMin.m_pBk);
        OBJ_PROP_INT    ("MinLeft",   m_ctrlCaption.m_ctrlMin.m_nLeft)
        OBJ_PROP_INT    ("MinTop",   m_ctrlCaption.m_ctrlMin.m_nTop)
        OBJ_PROP_INT    ("MinWidth",   m_ctrlCaption.m_ctrlMin.m_nWidth)
        OBJ_PROP_INT    ("MinHeight",   m_ctrlCaption.m_ctrlMin.m_nHeight)

        OBJ_PROP_PIC    ("HelpBk", m_ctrlCaption.m_ctrlHelp.m_pBk);
        OBJ_PROP_INT    ("HelpLeft",   m_ctrlCaption.m_ctrlHelp.m_nLeft)
        OBJ_PROP_INT    ("HelpTop",   m_ctrlCaption.m_ctrlHelp.m_nTop)
        OBJ_PROP_INT    ("HelpWidth",   m_ctrlCaption.m_ctrlHelp.m_nWidth)
        OBJ_PROP_INT    ("HelpHeight",   m_ctrlCaption.m_ctrlHelp.m_nHeight)

        OBJ_PROP_PIC    ("InnerBk", m_ctrlIBorder.m_pBk)
        OBJ_PROP_INT    ("InnerLeft", m_ctrlIBorder.m_nLeft)
        OBJ_PROP_INT    ("InnerRight", m_ctrlIBorder.m_nRight)
        OBJ_PROP_INT    ("InnerTop", m_ctrlIBorder.m_nTop)
        OBJ_PROP_INT    ("InnerBottom", m_ctrlIBorder.m_nBottom)

        OBJ_PROP_INT    ("SBWidth", m_ctrlScroll.m_nBarWidth)
        OBJ_PROP_INT    ("SBButtonHeight", m_ctrlScroll.m_nButtonHeight)
        OBJ_PROP_INT    ("SBThumbMin", m_ctrlScroll.m_nThumbMin)
        OBJ_PROP_PIC    ("SBBk", m_ctrlScroll.m_pBk)
        OBJ_PROP_PIC    ("SizeBoxBk", m_ctrlScroll.m_pSizeBox)

        CHAIN_OBJ_PROP  (CFrame)
    END_OBJ_PROP_MAP    ()

private:
    TMBool OnMouse(IFrameSource* pSender, EHitID hid, TMUInt uMsg, TMWParam wParam, TMLParam lParam);
    void HitTestAll(IFrameSource* pSender, TUIHITINFO& ht);
    void Invalidate(IFrameSource* pSender);

public:
    CFormManage() : m_dwFrameStyle(ALIGN_TOP) {}
    ~CFormManage() {}
    
    void FinalConstruct(CTuiObject* pp)
    {
        CModule& mdl = *GetModule();
        m_ctrlOBorder.FinalConstruct(mdl);
        m_ctrlCaption.FinalConstruct(mdl);;
        m_ctrlIBorder.FinalConstruct(mdl);;
        m_ctrlScroll.FinalConstruct(mdl);;
    }

    // for CTuiScrollObj
    void SetScrollProp(int nBarWidth, int nButtonHeight, int nThumb, ITuiPic* pBk)
    {
        m_ctrlScroll.m_nBarWidth = nBarWidth;
        m_ctrlScroll.m_nButtonHeight = nButtonHeight;
        m_ctrlScroll.m_nThumbMin = nThumb;
        m_ctrlScroll.m_pBk = pBk;
    }

    // CFrame
    virtual TMBool ProcessFrameMessage(
        IFrameSource* pSender, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult) tm_override;
    virtual void GetFrameInfo(HTUI hWnd, FRAMEINFO& fi, TMUInt32 dwStyle, TMUInt32 dwExStyle) tm_override;
    virtual TMBool PtInOBorder(const CTuiRect& rc, const CTuiPoint& pt) tm_override;


    // CTuiObject
#ifdef TUI_EDIT_MODE
    virtual int EditorRender(IGXApp* gapp, int nState, const CTuiRect& rc) tm_override;
    virtual TMBool EditorMatchID(TMUInt32 n) tm_override;
#endif // TUI_EDIT_MODE

};

TM_END_NAMESPACE



#endif // FRAMEFORM_H 
