/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.06.29
 *
 *  Copyright (C) 2025 miragekiller
 */

#pragma once

#ifndef UIHEADER_H
#define UIHEADER_H

#include "uiwidget.h"


TM_BGN_NAMESPACE


class TM_NO_VTABLE CTuiHeaderImpl
    : public CTuiWgt
{
public:
    struct ITEM
    {
        TMLParam            lParam;
        GXDips              nWidth;
        int                 nFmt;   // mask of HDF_XXX;
        GX::CText           gxText;
        CString             strAcc;
        CString             strHint;

        ITEM()
        {
            nWidth = 0;
            lParam = 0;
            nFmt = 0;
        }
    };
    typedef CVector<ITEM>           CItemList;

    int                 m_nPicMargin;
    CItemList           m_aItem;
    CPicPtr             m_pArray[2]; // 0:up; 1:down
    CPicPtr             m_pItem;
    CPropColor          m_clrNormal;
    CPropColor          m_clrHilight;
    CTexture            m_texDrag;
    int                 m_nCurSel;

    enum
    {
        HDS_BEGINTRACK_ = 0x4000, // have send HDN_BEGINTRACK
        HDS_HAVEMOVE_   = 0x8000, // have move mouse
    };
    
public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_HStyle, CSymbol_WsStyle)
        TM_MASK_SYMBOL  (HDS_BUTTONS)
//         TM_MASK_SYMBOL  (HDS_HOTTRACK)
        TM_MASK_SYMBOL  (HDS_DRAGDROP)
        TM_MASK_SYMBOL  (HDS_NOSIZING)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    TM_DECLARE_FLAG     (_HDS_BUTTONS,          m_dwStyle,   HDS_BUTTONS);
    TM_DECLARE_FLAG     (_HDS_DRAGDROP,         m_dwStyle,   HDS_DRAGDROP);
    TM_DECLARE_FLAG     (_HDS_NOSIZING,         m_dwStyle,   HDS_NOSIZING);
    TM_DECLARE_FLAG     (_HDS_BEGINTRACK_,      m_dwStyle,   HDS_BEGINTRACK_);
    TM_DECLARE_FLAG     (_HDS_HAVEMOVE_,        m_dwStyle,   HDS_HAVEMOVE_);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle,    CSymbol_HStyle)
        OBJ_PROP_CLASS  ("NormalTextColor",     m_clrNormal)
        OBJ_PROP_CLASS  ("HighlightTextColor",  m_clrHilight)
        OBJ_PROP_PIC    ("PicUp",               m_pArray[0])
        OBJ_PROP_PIC    ("PicDown",             m_pArray[1])
        OBJ_PROP_PIC    ("PicBut",              m_pItem)
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()
    
public:
    inline void _Check(int n) { DbgAssert(n >= 0 && n < m_aItem.size()); }

public:
    CTuiHeaderImpl() {}

    void FinalConstruct(CTuiObject* pp);
    ITuiPic* GetDftForePic(int fmt);
    ITuiPic* GetForePic(int fmt);

    int GetItemCount() { return m_aItem.size(); }
    bool IsItemFixed(const ITEM& item) const;
    void OnPrintClient(HDC hdc);
    GXPTexture CreateDragImage(int iItem);
    TMResult NotifyEx_Safe(UINT code, TMLParam lItm, TMLParam l1, TMLParam l2);
    LRESULT DeleteItem(int iItem);
    LRESULT OnGetItem(int nItem, HDITEM* phdi);
    LRESULT GetItemRect(int iItem, TUIRect* lpRect);
    int GetDragRect(TUIRect& rcDrag); // return: current selected item
    int GetDividerRect(int nSel, const TUIRect& rcDrag, TUIRect& rc); // return: current divider item.
    void CopyFmt(int nItem, ITEM& itm, int fmt);
    void CopyItem(int nItemCheck, ITEM& itm, const HDITEM *phdi);
    void CheckItem(const ITEM& itm, HDITEM *phdi);
    LRESULT OnHitTest(TUIHITINFO& hti);
    LRESULT InsertItem(int nItem, const HDITEM* phdi);
    LRESULT SetItem(int nItem, const HDITEM* phdi);
    LRESULT OnSetItemAccHInt(TMWParam wp, TMLParam lp, TMUInt msg);
    LRESULT OnGetItemAccHInt(TMWParam wp, TMLParam lp, TMUInt msg);
    LRESULT SetBitmapMargin(int m_nPicMargin);
    LRESULT OnCreate();
    LRESULT OnLButtonDown(LPARAM lParam);
    LRESULT OnLButtonUp(int x, int y);
    LRESULT OnMouseMove(LPARAM lParam);
    LRESULT OnLButtonDblClk();
    void OnMouseSwitch(const TUIHITINFO& hti);
    void OnCaptureChanged();

    // CTuiWgt
    virtual TMBool ProcessWindowMessage(CTuiWgt* hWnd, TMUInt uMsg,
        TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID) tm_final;
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_final;
    virtual bool GetWindowHint(ETTActive ett, CString* str) tm_final;
};

TM_END_NAMESPACE

#endif // UIHEADER_H