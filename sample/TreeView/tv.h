//-----------------------------------------------------------
//  tv.h
//
//  Copyright (C) 2009 miragekiller                               
//  All rights reserved                                                     
//
//
//  Author: 
//      miragekiller    <3916345933@qq.com>
//
//    Histroy :
//      2009.02.04      create
//-------------------------------------------------------------


#ifndef TV_H
#define TV_H


class CTVCtrl : public TM::CTuiWidgetImpl<CTVCtrl, TM::CTuiTree>
{
public:
    BEGIN_TUIMSG_MAP(CTVCtrl)
        MESSAGE_HANDLER     (CM_HITTEST,        OnHitTest)
//         MESSAGE_HANDLER     (CM_DRAWITEM,       OnDrawItem)
        MESSAGE_HANDLER     (WM_MOUSEMOVE,      OnMouseMove)
        MESSAGE_HANDLER     (CM_DRAGENTRY,      OnDragEntry)
        MESSAGE_HANDLER     (CM_DRAGOVER,       OnDragOver)
        MESSAGE_HANDLER     (CM_DRAGDROP,       OnDragDrop)
        MESSAGE_HANDLER     (WM_MOUSELEAVE,     OnMouseLeave)
        CHAIN_MSG_MAP       (root_class)
    END_TUIMSG_MAP()
    
    TMResult OnHitTest (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDrawItem (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseMove (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDragEntry (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDragOver (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDragDrop (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseLeave (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
};


#endif