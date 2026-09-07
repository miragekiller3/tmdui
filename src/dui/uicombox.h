/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2007.01.26
 *
 *  Copyright (C) 2007 miragekiller
 */

#pragma once

#ifndef UICOMBOX_H
#define UICOMBOX_H

#include "uibutton.h"


TM_BGN_NAMESPACE 

struct ICbList;

class TM_NO_VTABLE CTuiComboButtonImpl
    : public CTuiPushButtonImpl
{
private:
    CString             m_strItems;
    TMUInt32            m_nDlgID;   // CID<EM_DIALOG>
    TMUInt32            m_nWndID;   // CID<EM_WGT>
    TMUInt32            m_nReferID; // CID<EM_WGT>
    Ref                 m_wndEdit;
    Ref                 m_wndList;
    Ref                 m_wndPop;
    ICbList*            m_iList;    //none; listbox; treeview

    enum EPop { ePopNull, ePopColl, ePopUp };

public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_CBStyle, CSymbol_BStyle)
        TM_MASK_SYMBOL  (CBS_SIMPLE) // don't close listbox
        TM_MASK_SYMBOL  (CBS_CLICKTOPOP)
        TM_MASK_SYMBOL  (CBS_ARROWTOPOP)
        TM_MASK_SYMBOL  (CBS_SPACETOPOP)
        TM_MASK_SYMBOL  (CBS_FILTER)
        TM_MASK_SYMBOL  (CBS_NOIMEDIALTY)
        TM_MASK_SYMBOL  (CBS_NOEDIT)
        TM_MASK_SYMBOL  (CBS_NOSYNCHINT)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_CBStyle)
        OBJ_PROP_DLG    ("PopDlg", m_nDlgID)
        OBJ_PROP_WIN    ("PopList", m_nWndID)
        OBJ_PROP_WIN    ("Refer", m_nReferID)
        OBJ_PROP_TEXT   ("ListItems", m_strItems)
        CHAIN_OBJ_PROP  (CTuiPushButtonImpl)
    END_OBJ_PROP_MAP    ()

    TM_DECLARE_FLAG     (_Style_CBS_FILTER,         m_dwStyle, CBS_FILTER);
    TM_DECLARE_FLAG     (_Style_CBS_SPACETOPOP,     m_dwStyle, CBS_SPACETOPOP);
    TM_DECLARE_FLAG     (_Style_CBS_NOIMEDIALTY,    m_dwStyle, CBS_NOIMEDIALTY);
    TM_DECLARE_FLAG     (_Style_CBS_NOEDIT,         m_dwStyle, CBS_NOEDIT);
    TM_DECLARE_FLAG     (_Style_CBS_NOSYNCHINT,     m_dwStyle, CBS_NOSYNCHINT);
    TM_DECLARE_FLAG     (UpDownMode,                m_dwPriStyle, CB_PRI_UPDOWNMODE);

public: // edit, list, pop message handle.
    BEGIN_TUIMSG_MAP_H  (m_wndEdit, eTuiProcRef)
        return ProcessEditMessage(hWnd, uMsg, wParam, lParam, lResult);
    END_TUIMSG_MAP()
    TMBool ProcessEditMessage(HTUI hEdit, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult);

    BEGIN_TUIMSG_MAP_H  (m_wndList, eTuiProcRef)
        return ProcessListMessage(hWnd, uMsg, wParam, lParam, lResult);
    END_TUIMSG_MAP()
    TMBool ProcessListMessage(HTUI hList, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult);

    BEGIN_TUIMSG_MAP_H  (m_wndPop, eTuiProcRef)
        return ProcessPopMessage(hWnd, uMsg, wParam, lParam, lResult);
    END_TUIMSG_MAP()
    TMBool ProcessPopMessage(HTUI hPop, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult);

protected:
    CTuiComboButtonImpl();
    ~CTuiComboButtonImpl();

    ICbList* GetICB() { return m_iList; }
    HTUI GetRefer();
    TMBool ProcessKeyDown(HTUI hWgt, TMUInt uMsg, TMWParam wp, TMLParam lp, TMResult& ret);
    void OnCreate();
    TMBool SubmitText(bool bNotifyOk);
    TMBool DoMousePop();
    void InitListOnPop();
    void PopAsDialog(int x, int y, int cx, int cy, TMUInt nFlags);
    void Pop(TMBool bMousePop);
    EPop IsPop();
    void Filter();

    // CTuiButtonImpl
    virtual ITuiPic* GetDftPic() tm_override { return &GetModule()->m_aPrePic.m_picDft_BUTTON; }
    virtual ITuiPic* GetDftForePic() tm_override { return &GetModule()->m_aPrePic.m_picDftComb; }
    
    // CTuiWgt
    TMBool ProcessWindowMessage (CTuiWgt* hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) tm_override;

    // CTuiObject
#ifdef TUI_EDIT_MODE
    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_nReferID || n == m_nWndID || n == m_nDlgID)
            return true;
        return CTuiPushButtonImpl::EditorMatchID(n);
    }
#endif // TUI_EDIT_MODE
};


TM_END_NAMESPACE

#endif // UICOMBOX_H

