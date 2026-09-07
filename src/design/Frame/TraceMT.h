/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.2.17
 *
 *  Copyright (C) 2025 miragekiller
 */

#pragma once

#ifndef TRACEMT_H
#define TRACEMT_H

#include "tmthread.h"


class CTraceMT
    : public ITraceHook
{
private:
    typedef std::pair<CTraceMT*, LPCTSTR> CALLPARAM;

public:
    virtual void DoOnTrace(LPCTSTR pStr) = 0;

    static void _DoTrace(CALLPARAM* p)
    {
        p->first->DoOnTrace(p->second);
    }
    virtual void OnTrace(int tp, LPCTSTR pStr) tm_override
    {
        CALLPARAM cp(this, pStr);
        CallCreate(eCallTypeSync, (PFNCall1)_DoTrace, &cp);
    }
};



#endif //TRACEMT_H
