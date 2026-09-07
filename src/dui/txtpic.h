/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.11.09
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef TXTPIC_H
#define TXTPIC_H

#include "tmstring.h"
#include "uiwidget.h"
#include "pic.h"


TM_BGN_NAMESPACE

inline static void MakeSubYCenter(TM::CTuiRect& rc, GXDips h)
{
    rc.SetTop((rc.Top() + rc.Bottom() - h) / 2);
    rc.SetHeight(h);
}

class TM_NO_VTABLE CTxtObject : public CTuiWgt
{
public:
    TMUInt m_uFormat;
    
public:
    CTxtObject () : m_uFormat (DT_NOPREFIX) {}

    TM_BGN_MASK_SYMBOL  (CSymbol_Format)
        TM_MASK_SYMBOL2 (DT_CENTER, DT_CENTER | DT_RIGHT)
        TM_MASK_SYMBOL2 (DT_RIGHT, DT_CENTER | DT_RIGHT)
        TM_MASK_SYMBOL2 (DT_VCENTER, DT_VCENTER | DT_BOTTOM)
        TM_MASK_SYMBOL2 (DT_BOTTOM, DT_VCENTER | DT_BOTTOM)
        TM_MASK_SYMBOL  (DT_WORDBREAK)
        TM_MASK_SYMBOL  (DT_SINGLELINE)
        TM_MASK_SYMBOL  (DT_EXPANDTABS)
        TM_MASK_SYMBOL  (DT_TABSTOP)
        TM_MASK_SYMBOL  (DT_NOCLIP)
        TM_MASK_SYMBOL  (DT_EXTERNALLEADING)
        TM_MASK_SYMBOL  (DT_NOPREFIX)
        TM_MASK_SYMBOL  (DT_INTERNAL)
        TM_MASK_SYMBOL  (DT_EDITCONTROL)
        TM_MASK_SYMBOL  (DT_PATH_ELLIPSIS)
        TM_MASK_SYMBOL  (DT_END_ELLIPSIS)
        TM_MASK_SYMBOL  (DT_WORD_ELLIPSIS)
        TM_MASK_SYMBOL  (DT_L2RREADING)
    TM_END_MASK_SYMBOL  ()
        
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("TextFormat", m_uFormat, CSymbol_Format);
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()
    
    BEGIN_TUIMSG_MAP    (CTxtObject)
        MESSAGE_HANDLER (CM_SETTEXTFORMAT, OnSetFormat)
        CHAIN_MSG_MAP   (CTuiWgt)
    END_TUIMSG_MAP()
    
    TMResult OnSetFormat (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        m_uFormat = wParam;
        return 0;
    }
    
    TMResult OnGetFormat (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        return m_uFormat;
    }
    
    TMBool IsSingleLine () { return (m_uFormat & DT_SINGLELINE) || !(m_uFormat & DT_WORDBREAK); }
    
    TMBool IsFixedWidth ()
    {
        return !IsSingleLine() || 
            (m_uFormat & (DT_WORD_ELLIPSIS | DT_PATH_ELLIPSIS | DT_END_ELLIPSIS));
    }

    void CalcTextRect(IGXApp* gapp, GXDips& w, GXDips& h, GXDips nMax)
    {
//         const TMUInt uMask = DT_CENTER | DT_RIGHT | DT_BOTTOM | DT_VCENTER;
//        TMUInt fmt = (m_uFormat | DT_CALCRECT) & ~uMask;
        
        if (IsAutoWidth())
            w = (GXDips)SHRT_MAX;
        if (nMax >= 0 && w > nMax)
            w = nMax;
        
        GXSize sz(w, h);
        if (m_gxText)
        {
            m_gxText->SetFormat(m_uFormat);
            m_gxText->CalcSize(sz, gapp);
            w = sz.Width();
            h = sz.Height();
        }
        else
            w = h = 0;
    }

//     void CalcForamtRect(TM::CTuiRect& rc, int w, int h, TMUInt uFormat)
//     {
//         if (uFormat & DT_RIGHT)
//             rc.SetLeft(rc.Right() - w);
//         else if (uFormat & DT_CENTER)
//             rc.SetLeft((rc.Left() + rc.Right() - w) / 2);
//         rc.SetWidth(w);
// 
//         if (uFormat & DT_BOTTOM)
//             rc.SetTop(rc.Bottom() - h);
//         else if (uFormat & DT_VCENTER)
//             rc.SetTop((rc.Top() + rc.Bottom() - h) / 2);
//         rc.SetHeight(h);
//     }
};


class TM_NO_VTABLE CAutoSizedTxtObject : public CTxtObject
{
// protected:
//     int         m_nMinWidth;
//     int         m_nMaxWidth;
    
public:
//     BGN_OBJ_PROP_MAP    ()
//         OBJ_PROP_INT    ("MinWidth",            m_nMinWidth)
//         OBJ_PROP_INT    ("MaxWidth",            m_nMaxWidth)
//         CHAIN_OBJ_PROP  (CTxtObject)
//     END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CAutoSizedTxtObject)
        MESSAGE_HANDLER (CM_SETTEXT,            OnPreDirty)
        MESSAGE_HANDLER (WM_SETTEXT,            OnPreDirty)
        MESSAGE_HANDLER (CM_SETTEXTFORMAT,      OnPreDirty)
        MESSAGE_HANDLER (CM_LANGUAGECHANGED,    OnPreDirty)
        MESSAGE_HANDLER (CM_COLORVISUAL,        OnPreDirty)
        MESSAGE_HANDLER (WM_SIZE,               OnDirty)
        MESSAGE_HANDLER (CM_ZOOMED,             OnDirty)
        MESSAGE_HANDLER (CM_SETFONT,            OnDirty)
        CHAIN_MSG_MAP   (CTxtObject)
    END_TUIMSG_MAP()


    CAutoSizedTxtObject()
//         : m_nMinWidth (-1)
//         , m_nMaxWidth (-1)
    {
        Set_WS_PRI_DIRTY();
    }

    TMResult OnPreDirty(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        TMResult rl;
        CTxtObject::ProcessWindowMessage(this, uMsg, wParam, lParam, rl, 0);
        if (!GetWindowText().IsEmpty())
        {
            GXDips x = 1;
            if (IsAutoHeight() && GetHeight() <= 0)
                SendScrollRangeMessage(0, &x);
            else if (IsAutoWidth() && GetWidth() <= 0)
                SendScrollRangeMessage(&x, 0);
        }
        SetDirtyAndInvalidate();
        return 0;
    }

    GXDips GetMax(GXDips nExclude)
    {
        if (m_nMaxWidth >= 0)
            return m_nMaxWidth - GetViewLeft() - GetViewRight() - nExclude;
        else
            return -1;
    }
};

struct TM_NO_VTABLE PhotoText
    : public CForePicWin<CAutoSizedTxtObject>
{
    EAlignMode  m_eMode;
    int         m_nPicWidth;
    int         m_nPicHeight;
    int         m_nPicMargin;

    TM_BGN_MASK_SYMBOL  (CSymbol_Mode)
        TM_MASK_SYMBOL2 (e_amLeft,          e_amMaskAlignMode)
        TM_MASK_SYMBOL2 (e_amTop,           e_amMaskAlignMode)
        TM_MASK_SYMBOL2 (e_amRight,         e_amMaskAlignMode)
        TM_MASK_SYMBOL2 (e_amBottom,        e_amMaskAlignMode)
        TM_MASK_SYMBOL2 (e_amLeftTop,       e_amMaskAlignMode)
        TM_MASK_SYMBOL2 (e_amLeftBottom,    e_amMaskAlignMode)
        TM_MASK_SYMBOL2 (e_amRightTop,      e_amMaskAlignMode)
        TM_MASK_SYMBOL2 (e_amRightBottom,   e_amMaskAlignMode)
        TM_MASK_SYMBOL2 (e_amTopLeft,       e_amMaskAlignMode)
        TM_MASK_SYMBOL2 (e_amTopRight,      e_amMaskAlignMode)
        TM_MASK_SYMBOL2 (e_amBottomLeft,    e_amMaskAlignMode)
        TM_MASK_SYMBOL2 (e_amBottomRight,   e_amMaskAlignMode)
        TM_MASK_SYMBOL  (e_amL2RReading)
        TM_MASK_SYMBOL  (e_amFrame)
    TM_END_MASK_SYMBOL  ()

    TM_DECLARE_FLAG_GET (FlagDirVert, m_eMode, e_amMaskDirV);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Alignment",   m_eMode, CSymbol_Mode)
//         OBJ_PROP_PIC    ("ForePic",     m_pFore)
        OBJ_PROP_INT    ("PicWidth",    m_nPicWidth)
        OBJ_PROP_INT    ("PicHeight",   m_nPicHeight)
        OBJ_PROP_INT    ("PicMargin",   m_nPicMargin)
        CHAIN_OBJ_PROP  (CForePicWin<CAutoSizedTxtObject>)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (PhotoText)
        MESSAGE_HANDLER (CBM_GETALIGNMODE,      OnGetAlignMode)
        MESSAGE_HANDLER (CBM_SETALIGNMODE,      OnSetAlignMode)
        MESSAGE_HANDLER (CBM_SETFOREPIC,        OnDirty)
        MESSAGE_HANDLER (CBM_SETFOREPICWIDTH,   OnSetForePicWidth)
        CHAIN_MSG_MAP   (CForePicWin<CAutoSizedTxtObject>)
    END_TUIMSG_MAP()

    PhotoText()
        : m_eMode       (e_amLeft)
        , m_nPicWidth   (16)
        , m_nPicHeight  (16)
        , m_nPicMargin  (2)
    {
    }

    TMResult OnGetAlignMode(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        return m_eMode;
    }
    
    TMResult OnSetAlignMode(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        if (m_eMode != wParam)
        {
            m_eMode = (EAlignMode)wParam;
            OnPreDirty(uMsg, wParam, lParam, bHandled);
        }
        return true;
    }

    TMResult OnSetForePicWidth(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        m_nPicWidth = wParam;
        m_nPicHeight = lParam;
        if (m_pFore)
            OnPreDirty(uMsg, wParam, lParam, bHandled);
        return 0;
    }
};


struct ITextPicLayout
{
    // only for LayoutPT
    virtual TMBool IsAutoWidth() const = 0;
    virtual TMBool IsAutoHeight() const = 0;
    virtual GXDips GetMaxWidth() = 0;       // { return (m_nMaxWidth >= 0) ? m_nMaxWidth - GetViewLeft() - GetViewRight() : SHRT_MAX; }

    // for render/LayoutPT
    virtual EAlignMode GetAlignMode() const = 0;
    virtual TMUInt GetTextFormat() const = 0;
    virtual GXDips GetPicMargin() = 0;

    // only for render
    virtual CModule* GetModule() = 0;
    virtual ITuiPic* GetHCForePic() = 0;
    virtual void AniRender(IGXApp* gapp, ITuiPic* pFore, int nFrame, EButStates ebsBk, const CTuiRect& rcFore) = 0;

    
private:
    void CalcPicSize(ITuiPic* pFore, GXDips& nPicWidth, GXDips& nPicHeight)
    {
        if (nPicWidth > 0 && nPicHeight > 0)
            return;
        CSize sz(0,0);
        if (pFore)
            pFore->GetSize(sz);
        if (nPicWidth < 0)
            nPicWidth = sz.Width();
        if (nPicHeight < 0)
            nPicHeight = sz.Height();
    }
    
public:
    void LayoutPT(IGXApp* gapp, IGXText* gxText, ITuiPic* pFore, GXDips nPicWidth, GXDips nPicHeight,
        const CTuiRect& rcPadding, GXDips& w, GXDips& h)
    {
        if (gxText && gxText->GetLength())
        {
            GXDips nPicMargin = GetPicMargin();
            CalcPicSize(pFore, nPicWidth, nPicHeight);
            
            if (IsAutoWidth())
                w = GetMaxWidth();
            else
                w = rcPadding.Width();
            
            if (IsAutoHeight())
                h = SHRT_MAX;
            else
                h = rcPadding.Height();
            
            GXDips dh = 0, dw = 0;
            if (pFore)
            {
                if (GetAlignMode() & e_amMaskDirV)
                    dh = (nPicMargin + nPicHeight);
                else
                    dw = (nPicMargin + nPicWidth);
            }
            
            // get the width and height of text
            GXSize szTest(w - dw, h - dh);
            gxText->SetFormat(GetTextFormat());
            gxText->CalcSize(szTest, gapp);
            
            dw += tm_max(szTest.Width(), 1);
            dh += tm_max(szTest.Height(), 1);
            w = tm_max(dw, nPicWidth);
            h = tm_max(dh, nPicHeight);
        }
        else if (pFore)
        {
            CalcPicSize(pFore, nPicWidth, nPicHeight);
            w = nPicWidth;
            h = nPicHeight;
        }
        else
        {
            w = 0;
            h = 0;
        }
    }

    // |--------------------|
    // |   rcPadding        |
    // |     ---------------------
    // |     |  rcView           |
    // |     ---------------------
    // |--------------------|
    void Render(
        IGXApp* gapp, IGXText* gxText, ITuiPic* pFore, GXDips nPicWidth, GXDips nPicHeight,
        int nFrame, EButStates ebsBk,
        const CTuiRect& rcPadding,
        CTuiRect& rcView, // in/out
        CTuiRect& rcFore) // out
    {
        rcFore = rcView;
        TMUInt fmt = GetTextFormat();
        if (pFore)
        {
            TMUInt32 uFmt = 0;
            EAlignMode eMode = GetAlignMode();
            if ((eMode & e_amL2RReading) && GetModule()->GetLocal().IsRTL())
            {
                switch (eMode & e_amMaskLR)
                {
                case e_amLeft: eMode = (EAlignMode)((eMode & ~e_amMaskLR) | e_amRight); break;
                case e_amRight: eMode = (EAlignMode)((eMode & ~e_amMaskLR) | e_amLeft); break;
                }
            }
            switch (eMode & e_amMaskAlignMode)
            {
            case e_amTop: uFmt = DT_TOP | DT_CENTER; break;
            case e_amBottom: uFmt = DT_BOTTOM | DT_CENTER; break;
            case e_amRight: uFmt = DT_RIGHT | DT_VCENTER; break;
            case e_amLeftTop: case e_amTopLeft: uFmt = DT_LEFT | DT_TOP; break;
            case e_amRightTop: case e_amTopRight: uFmt = DT_RIGHT | DT_TOP; break;
            case e_amLeftBottom: case e_amBottomLeft: uFmt = DT_LEFT | DT_BOTTOM; break;
            case e_amRightBottom: case e_amBottomRight: uFmt = DT_RIGHT | DT_BOTTOM; break;
            default: uFmt = DT_LEFT | DT_VCENTER; break;
            }
            
            CalcPicSize(pFore, nPicWidth, nPicHeight);
            gxTextCalcAlignmentRect(rcFore, nPicWidth, nPicHeight, uFmt);
            
            if (eMode & e_amFrame)
            {
                switch (eMode & e_amMaskAlign)
                {
                case e_amTop: rcFore.KeepHSetT(rcPadding.Top()); rcView.KeepHSetB(rcPadding.Bottom()); break;
                case e_amBottom: rcFore.KeepHSetB(rcPadding.Bottom()); rcView.KeepHSetT(rcPadding.Top()); break;
                case e_amRight: rcFore.KeepWSetR(rcPadding.Right()); rcView.SetLeftRight(rcPadding.Left(), rcFore.Left() - GetPicMargin()); fmt &= ~(DT_RIGHT | DT_CENTER); fmt |= DT_LEFT; break;
                default: rcFore.KeepWSetL(rcPadding.Left()); rcView.SetLeftRight(rcFore.Right() + GetPicMargin(), rcPadding.Right()); fmt &= ~(DT_LEFT | DT_CENTER); fmt |= DT_RIGHT; break;
                }
            }
            else
            {
                switch (eMode & e_amMaskAlign)
                {
                case e_amTop: rcView.SetTop(rcFore.Bottom() + GetPicMargin()); break;
                case e_amBottom: rcView.SetBottom(rcFore.Top() - GetPicMargin()); break;
                case e_amRight: rcView.SetRight(rcFore.Left() - GetPicMargin()); break;
                default: rcView.SetLeft(rcFore.Right() + GetPicMargin()); break;
                }
            }
            
            ITuiPic* pf = GetHCForePic();
            if (!pf)
                pf = pFore;
            AniRender(gapp, pFore, nFrame, ebsBk, rcFore);
        }
        
        // render text
        if (gxText)
        {
//             m_gxText->SetFormat(fmt);
            gxText->Paint(rcView, 0, 0, gapp);
            rcView.InflateRect(1, 1); // for draw focus rect
        }
    }

//     void Render2(
//         IGXApp* gapp, IGXText* gxText, ITuiPic* pFore, GXDips nPicWidth, GXDips nPicHeight,
//         int nFrame, EButStates ebsBk,
//         const CTuiRect& rcPadding,
//         CTuiRect& rcView, // in/out
//         CTuiRect& rcFore) // out
//     {
//         rcFore = rcView;
//         TMUInt fmt = GetTextFormat();
//         GXDips w = rcView.Width();
//         GXDips h = rcView.Height();
//         if (pFore)
//         {
//             CalcPicSize(pFore, nPicWidth, nPicHeight);
//             EAlignMode eMode = GetAlignMode();
//             GXDips mg = GetPicMargin();
//             if (eMode & & e_amMaskDirV)
//                 h -= (nPicHeight + mg);
//             else
//                 w -= (nPicHeight + mg);
//         }
// 
//         if (gxText)
//         {
//             gxText->Paint(rcView, 0, gapp);
// 
// 
// 
//         if (pFore)
//         {
//             TMUInt32 uFmt = 0;
//             EAlignMode eMode = GetAlignMode();
//             if ((eMode & e_amL2RReading) && GetModule()->GetLocal().IsRTL())
//             {
//                 switch (eMode & e_amMaskLR)
//                 {
//                 case e_amLeft: eMode = (EAlignMode)((eMode & ~e_amMaskLR) | e_amRight); break;
//                 case e_amRight: eMode = (EAlignMode)((eMode & ~e_amMaskLR) | e_amLeft); break;
//                 }
//             }
//             switch (eMode & e_amMaskAlignMode)
//             {
//             case e_amTop: uFmt = DT_TOP | DT_CENTER; break;
//             case e_amBottom: uFmt = DT_BOTTOM | DT_CENTER; break;
//             case e_amRight: uFmt = DT_RIGHT | DT_VCENTER; break;
//             case e_amLeftTop: case e_amTopLeft: uFmt = DT_LEFT | DT_TOP; break;
//             case e_amRightTop: case e_amTopRight: uFmt = DT_RIGHT | DT_TOP; break;
//             case e_amLeftBottom: case e_amBottomLeft: uFmt = DT_LEFT | DT_BOTTOM; break;
//             case e_amRightBottom: case e_amBottomRight: uFmt = DT_RIGHT | DT_BOTTOM; break;
//             default: uFmt = DT_LEFT | DT_VCENTER; break;
//             }
//             
//             CalcPicSize(pFore, nPicWidth, nPicHeight);
//             gxTextCalcAlignmentRect(rcFore, nPicWidth, nPicHeight, uFmt);
//             
//             if (eMode & e_amFrame)
//             {
//                 switch (eMode & e_amMaskAlign)
//                 {
//                 case e_amTop: rcFore.KeepHSetT(rcPadding.Top()); rcView.KeepHSetB(rcPadding.Bottom()); break;
//                 case e_amBottom: rcFore.KeepHSetB(rcPadding.Bottom()); rcView.KeepHSetT(rcPadding.Top()); break;
//                 case e_amRight: rcFore.KeepWSetR(rcPadding.Right()); rcView.SetLeftRight(rcPadding.Left(), rcFore.Left() - GetPicMargin()); fmt &= ~(DT_RIGHT | DT_CENTER); fmt |= DT_LEFT; break;
//                 default: rcFore.KeepWSetL(rcPadding.Left()); rcView.SetLeftRight(rcFore.Right() + GetPicMargin(), rcPadding.Right()); fmt &= ~(DT_LEFT | DT_CENTER); fmt |= DT_RIGHT; break;
//                 }
//             }
//             else
//             {
//                 switch (eMode & e_amMaskAlign)
//                 {
//                 case e_amTop: rcView.SetTop(rcFore.Bottom() + GetPicMargin()); break;
//                 case e_amBottom: rcView.SetBottom(rcFore.Top() - GetPicMargin()); break;
//                 case e_amRight: rcView.SetRight(rcFore.Left() - GetPicMargin()); break;
//                 default: rcView.SetLeft(rcFore.Right() + GetPicMargin()); break;
//                 }
//             }
//             
//             ITuiPic* pf = GetHCForePic();
//             if (!pf)
//                 pf = pFore;
//             AniRender(gapp, pFore, nFrame, ebsBk, rcFore);
//         }
//         
//         // render text
//         if (gxText)
//         {
//             //             m_gxText->SetFormat(fmt);
//             gxText->Paint(rcView, 0, gapp);
//             rcView.InflateRect(1, 1); // for draw focus rect
//         }
//     }

};


TM_END_NAMESPACE


#endif // TXTPIC_H
