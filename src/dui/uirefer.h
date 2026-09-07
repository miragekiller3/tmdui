/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2017.10.20
 *
 *  Copyright (C) 2017 miragekiller
 */

#pragma once

#ifndef UIREFER_H
#define UIREFER_H

#include "uiwidget.h"


TM_BGN_NAMESPACE


class TM_NO_VTABLE CTuiReferImpl
    : public CTuiWgt
{
private:
    TMUInt32 m_dwTargetID;

public:
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_CLASS  ("Rect", m_rcWnd)
        OBJ_PROP_WIN    ("Target", m_dwTargetID)
    END_OBJ_PROP_MAP    ()
    
    BEGIN_TUIMSG_MAP    (CTuiReferImpl)
        CHAIN_MSG_MAP   (CTuiWgt)
    END_TUIMSG_MAP      ()

//     TMResult OnPrintClient(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    CTuiReferImpl();

    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam);

    // CTuiObject
#ifdef TUI_EDIT_MODE
    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_dwTargetID)
            return true;
        return CTuiWgt::EditorMatchID(n);
    }
#endif // TUI_EDIT_MODE
};

TM_END_NAMESPACE



#endif //UIREFER_H
