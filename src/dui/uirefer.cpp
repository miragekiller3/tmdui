/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2017.10.20
 *
 *  Copyright (C) 2017 miragekiller
 */

#include "uirefer.h"


TM_BGN_NAMESPACE

CTuiReferImpl::CTuiReferImpl()
    : m_dwTargetID(0)
{
}

// CTuiWgt
void CTuiReferImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    TMUInt8 r = rand() % 255;
    TMUInt8 g = rand() % 255;
    TMUInt8 b = rand() % 255;
    gapp->FrameRect(m_rcClient, 2, TMRGB(r, g, b));
}

#ifdef TUI_EDIT_MODE
TM_REGISTER_CLASS2 (CTuiReferImpl, TM_CS(TUI_CLASS_REFER));
#endif

TM_END_NAMESPACE


