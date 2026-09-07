/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.14
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef UISTATIC_H 
#define UISTATIC_H

#include "txtpic.h"


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// class CTuiStaticImpl
class TM_NO_VTABLE CTuiStaticImpl
//     : public CTuiWgt
    : public CAutoSizedTxtObject
{
protected:
    CPropColor  m_clrNormal;
    TMInt       m_nCurChecks;

public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_Style,     CSymbol_WsStyle)
        TM_MASK_SYMBOL2 (SS_WINDOWBKONHC,   SS_BKONHCMASK)
        TM_MASK_SYMBOL2 (SS_DLGBKONHC,      SS_BKONHCMASK)
        TM_MASK_SYMBOL2 (SS_CLEARBKONHC,    SS_BKONHCMASK)
        TM_MASK_SYMBOL2 (SS_NULLBKONHC,     SS_BKONHCMASK)
        TM_MASK_SYMBOL  (SS_NULLBKONNORMAL)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    TM_DECLARE_FLAG     (FlagWindowBkOnHC, m_dwStyle, SS_WINDOWBKONHC);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_Style)
        OBJ_PROP_CLASS  ("TextColor",  m_clrNormal)
        OBJ_PROP_INT    ("State",      m_nCurChecks)
        CHAIN_OBJ_PROP  (CAutoSizedTxtObject)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiStatic)
        MESSAGE_HANDLER (CM_SETTEXTCOLOR,       OnSetTextColor)
        MESSAGE_HANDLER (CBM_GETCHECK,          OnGetCheck)
        MESSAGE_HANDLER (CBM_SETCHECK,          OnSetCheck)
//         MESSAGE_HANDLER (CBM_GETBUTTONSTATE,    OnGetButtonState)
        CHAIN_MSG_MAP   (CAutoSizedTxtObject)
    END_TUIMSG_MAP()

//     TMResult OnCreate (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetTextColor(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetCheck(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return GetCheck(); }
    TMResult OnSetCheck(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnGetButtonState(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return GetCheck(); }

// private:
//     void CalcDelta (TMInt16& dx, TMInt16& dy);
    
public:
    CTuiStaticImpl()
        : m_nCurChecks(0)
    {
    }

    int GetCheck() const;

    // CTuiWgt
    virtual EButStates DoGetStates(HTUI hFocus, HTUI hOver, TMBool bMouseDown) tm_override;
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override;
};


////////////////////////////////////////////////////////////////////////////////
// class CTuiGroupBoxImpl
class TM_NO_VTABLE CTuiGroupBoxImpl :  public CTuiWgt
{
protected:
    CPropColor m_clrNormal;
    
public:
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_CLASS  ("TextColor",  m_clrNormal)
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()
    
    BEGIN_TUIMSG_MAP    (CTuiGroupBoxImpl)
        MESSAGE_HANDLER (CM_SETTEXTCOLOR,           OnSetTextColor)
        MESSAGE_HANDLER (WM_GETDLGCODE,             OnGetDlgCode)
        CHAIN_MSG_MAP   (CTuiWgt)
    END_TUIMSG_MAP      ()
    
    TMResult OnSetTextColor(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        Invalidate();
        m_clrNormal = lParam;
        return 0;
    }
    
    TMResult OnGetDlgCode (TMUInt uMsg,TMWParam wParam,TMLParam lParam,TMBool& bHandled)
    {
        return DLGC_BUTTON | DLGC_STATIC;
    }
    
public:
    CTuiGroupBoxImpl();
    ~CTuiGroupBoxImpl();

    void FinalConstruct(CTuiObject* pp)
    {
        m_pBk   = &GetModule()->m_aPrePic.m_picDftGroup;
        CTuiWgt::FinalConstruct(pp);
    }
    
    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam);
};

TM_END_NAMESPACE

#endif // UISTATIC_H
