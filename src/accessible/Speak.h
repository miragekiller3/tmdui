/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.9.5
 *
 *  Copyright (C) 2018 miragekiller
 */

#ifndef ACCSPEAK_H
#define ACCSPEAK_H

#ifndef TMAUTOPTR_H
#   include "tmautoptr.h"
#endif

TM_BGN_NAMESPACE

enum ESpeakDev
{
    eSpeakDevJAWS           = 0x01,
    eSpeakDevNVDALess1800   = 0x02,
    eSpeakDevNVDAThan1800   = 0x04, // NVDA will speak anytime after Win10.1800
    eSpeakDevNVDA           = eSpeakDevNVDALess1800 | eSpeakDevNVDAThan1800,
    eSpeakNarrator          = 0x08,
    eSpeakAll               = eSpeakDevJAWS | eSpeakDevNVDA | eSpeakNarrator,
    eSpeakAllOld            = eSpeakDevJAWS | eSpeakDevNVDALess1800 | eSpeakNarrator,
};
TM_ENUM_2_FLAG(ESpeakDev);


struct CAccSpeak
{
public:
    struct IDevice
    {
        virtual TMBool Say(LPCWSTR pStr) = 0;
        virtual ~IDevice() {}
    };

private:
    CAutoPtr<IDevice> m_ptrDevice;
    HWND m_hWnd;
    ESpeakDev m_dev;

    TMBool InitDevice();

public:
    CAccSpeak();
    TMBool SayString(LPCWSTR str, ESpeakDev flag = eSpeakAll);
};


TM_END_NAMESPACE



#endif //ACCSPEAK_H
