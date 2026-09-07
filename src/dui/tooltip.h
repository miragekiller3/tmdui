/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.6.16
 *
 *  Copyright (C) 2016 miragekiller
 */

#pragma once

#ifndef TOOLTIP_H
#define TOOLTIP_H

#include "../tmduiatl.h"
#include "../tmani.h"

#ifdef TM_WIN
#   ifndef TTM_ADJUSTRECT
#       define TTM_ADJUSTRECT (WM_USER + 31)
#   endif
#   include <atlctrls.h>
#endif

#ifdef TM_QT
#   include <QToolTip>
#   include <QWidget>
#endif


TM_BGN_NAMESPACE

tm_enum(ETTActive)
{
    eTTActiveNormal,        // track wait time
    eTTActiveNow,           // immediately, track by user message, 
    eTTActvieTab,           // immediately, track by user press tab key
};

struct CTooltipSys
{
#ifdef TM_QT
#elif defined(TM_WIN)
    CToolTipCtrlT<CWindowAT> m_tooltip;
    void DestroyTip();
#endif
    bool SetNativeToolTip(HTUI hWnd, TMCStr pStr, int nTuiID, ETTActive eActive, ETTActive eOld);
    void SetTipBkColor(TMColor clr);
    void SetTipTextColor(TMColor clr);
    void SetToLayered(TMBool bLayed);
};


///////////////////////////////////////////////////////////////////////////////
// class CToolTipTui
class CToolTipTui
    : public CTuiDialogImpl<CToolTipTui>
{
private:
    CAniID0  m_aniMouse;
    CAniID1  m_aniShow;

private:
    void SetTool(TMHWnd hTarget, TMBool bDestroying);
    void TargetProc(TMMSG* msg);
    void TrackActivate();
    
public:
    // CAniProc
    void OnAni(const CAniID0& tm, ...);
    void OnAni(const CAniID1& tm, ...);
    
    // CTuiDialogImpl
    TMBool ProcessWindowMessage(TMHWnd hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0);

public:
    void SetNativeToolTip(HTUI hWnd, TMCStr pStr, int nTuiID, ETTActive eActive);
    TMBool Notify(HTUI hTarget, TMUInt msg, TMLParam lp);
};


///////////////////////////////////////////////////////////////////////////////
// class CTooltipMgr
class CTooltipMgr
{
public:
    friend CToolTipTui;
    CTooltipSys m_wndTool;
    CToolTipTui m_tuiTool;  // the tooltip window
    HTUI        m_hTarget;
    ETTActive   m_mode;
    TMColor     m_clrBk;
    TMColor     m_clrText;
    TMBool      m_bLayedTooltip;

    void UpdateProp(int id);

public:
    CTooltipMgr();
    ~CTooltipMgr() { Clear(); }
    TMBool IsPop();
    void Clear();
    void SetToolTip(HTUI pNew, ETTActive eActive);
    void SetTabToolTip(HTUI pNew);
    void ReactiveToolTip(HTUI pNew) { SetToolTip(pNew, m_mode); }
    TMBool IsTooltipChild(HTUI hWnd);
    TMBool IsTooltipTarget(HTUI hWnd) { return hWnd == m_hTarget; }
    void RelayMouseEvent(HTUI hOwner, TMUInt uMsg, TMWParam wParam, TMLParam lParam);
};

extern CTooltipMgr g_mgrTooltip;



TM_END_NAMESPACE



#endif //TOOLTIP_H
