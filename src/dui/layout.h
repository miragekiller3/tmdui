/**
 *  @file
 *  @brief  layout
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2012.02.29
 *
 *  Copyright (C) 2012 miragekiller                               
 */

#pragma once

#ifndef LAYOUT_H
#define LAYOUT_H

#include "uiwidget.h"


TM_BGN_NAMESPACE

void LayoutChilds(HTUI hParent, const CTuiRect& rcOldP, const CTuiRect& rcNewP);

// is childs have ALIGN_HEADER;
bool HaveChildAlignScroll(HTUI hParent);

// void LayoutDeltaSize(TMULong dwLayOut, CTuiRect& rc, GXDips dx, GXDips dy);


TM_END_NAMESPACE

#endif // LAYOUT_H
