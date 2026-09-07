/**
 *    @file
 *    @brief
 *    CDlgSpyFocus
 *
 *  @author miragekiller <3916345933@qq.com>
 *    @date    2017-11-12
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGSPYFOCUS_H
#define DLGSPYFOCUS_H

#include "tuires.h"
#include "tmtimer.h"

class CDlgSpyFocus
    : public TM::CTuiWidgetImpl<CDlgSpyFocus> //, TM::CTuiWindow>
{
public:
    enum { IDD = IDC_MANAGE_TARGET_SELECT };
    
public:
    static void OnTrackFocus(HWND hParent, void* pNewObj, const TM::CRect& rc);
};


#endif // DLGSPYFOCUS_H