/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.09.26
 *
 *  Copyright (C) 2019 miragekiller
 */

#pragma once

#ifndef GXANISRC_H
#define GXANISRC_H

#include "../tmdib.h"


class ILockData
{
public:
    virtual void* LockData() = 0;
    virtual void UnlockData(void* token) = 0;
};

class IAniSource
{
protected:
    ~IAniSource() {}

public:
    virtual void Release() = 0;
    virtual int GetDelay() = 0; // ms
    virtual void GetInfo(int& w, int& h, int& nformat, int& frames) = 0;
    virtual void GetFrame(int nFrame, TM::CDibHandle32& ld) = 0;
};

#endif // GXANISRC_H
