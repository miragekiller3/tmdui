/**
 *    @file
 *    @brief
 *    CDlgControls
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2012-10-2
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2012 miragekiller                               
 *    All rights reserved  
 */

#ifndef WNDTV_H
#define WNDTV_H

#include "tuires.h"

class IPropBox;

class CTVCtrlRender : public TM::CTuiTree
{
public:
    enum { IDD = IDW_TREE_RES_VIEW };

public:
    TM::CMap<TM::CString, int>  m_aClassPic;
    TM::CTuiPicPtr              m_picIcon;
    GX::CText                   m_txtDot;
    GX::CText                   m_txtAdd;
    GX::CText                   m_txtDec;
    GX::CText                   m_txtNum[10];
    TM::HTVITEM                 m_itmSelParent;
    TM::HTVITEM                 m_itmSelPrev;
    TM::HTVITEM                 m_itmSelNext;

    CTVCtrlRender();
    ~CTVCtrlRender();

    BEGIN_TUIMSG_MAP(CTVCtrlRender)
        MESSAGE_HANDLER     (CM_HITTEST,        OnHitTest)
        MESSAGE_HANDLER     (CM_DRAWITEM,       OnDrawItem)
        MESSAGE_HANDLER     (CM_MOUSESWITCH,    OnMouseSwitch)
    END_TUIMSG_MAP()

    TMResult OnHitTest (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDrawItem (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseSwitch (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
};


#endif