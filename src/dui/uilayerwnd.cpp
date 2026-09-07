/*
 *  @file
 *  @brief  window layer base class.
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2017.11.16
 *
 *  Copyright (C) 2017 miragekiller
 */

#include "uilayerwnd.h"
#include "view.h"


TM_BGN_NAMESPACE


CTuiLayerWnd::~CTuiLayerWnd()
{
}

TMResult CTuiLayerWnd::OnSetAlpha(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (m_alpha != (TMUInt8)lParam)
    {
        m_alpha = (TMUInt8)lParam;
        GetReal()->RealUpdateAlpha(true);
    }
    return 0;
}

TMBool CTuiLayerWnd::NativeInvalidateRect(SPY_INVALIDATE_PARAM(const TUIRect& rc, TM::HTUI hSender))
{
    TMBool b = CheckInvalidate(rc);
    if (!b)
        return b;
    GetReal()->RealInvalidate(false, false);
    return true;
}


TM_END_NAMESPACE



