/**
 *  @file
 *  @brief
 *  CDlgPlayHz
 *
 *  @author YourUserName <name@Email.com>
 *  @date   2026-4-3
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2026 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgPlayHz.h"
#include "tmlist.h"
#include "tmtime.h"

#include <mmsystem.h>
// #include <cmath>
// #include <iostream>
// #include <vector>
// #include <string>
// #include <sstream>
// #include <thread>
// #include <atomic>
// #include <conio.h>

#pragma comment(lib, "winmm.lib")

using namespace TM;


const double PI = 3.14159265358979323846;
const int SAMPLE_RATE = 44100;
const int BITS_PER_SAMPLE = 16;
const int CHANNELS = 1;
const int BUFFER_SIZE = 4096;

bool g_isPlaying = false;
bool g_stopRequested = false;
double g_currentFrequency = 440.0;

class CWaveOut16
{
public:
    HWAVEOUT hWaveOut;
    WAVEHDR waveHdr;
    CVector<short> audio_data;
    bool bPlay;

public:
    CWaveOut16() : hWaveOut(0), bPlay(false) {}
    ~CWaveOut16()
    {
        waveOutClose(hWaveOut);
    }

    void generateSineWaveBlock(double frequency, int num_samples)
    {
        num_samples = SAMPLE_RATE * 20 / frequency;
        audio_data.resize(num_samples);
    
        static double phase = 0.0;
        double amplitude = 0.7; // 0-1
        double phase_increment = 2.0 * PI * frequency / SAMPLE_RATE;
    
        for (int i = 0; i < num_samples; i++)
        {
            double sample = amplitude * sin(phase);
            phase += phase_increment;
        
            if (phase >= 2.0 * PI)
                phase -= 2.0 * PI;
        
            short pcm_sample = static_cast<short>(sample * (double)SHRT_MAX);
        
            audio_data[i] = pcm_sample;
        }
    }

    void Play(double frequency)
    {
        if (!hWaveOut)
        {
            WAVEFORMATEX wfx = {0};
            wfx.wFormatTag = WAVE_FORMAT_PCM;
            wfx.nChannels = CHANNELS;
            wfx.nSamplesPerSec = SAMPLE_RATE;
            wfx.nAvgBytesPerSec = SAMPLE_RATE * CHANNELS * (BITS_PER_SAMPLE / 8);
            wfx.nBlockAlign = CHANNELS * (BITS_PER_SAMPLE / 8);
            wfx.wBitsPerSample = BITS_PER_SAMPLE;
            wfx.cbSize = 0;
            
            
            MMRESULT result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
            if (result != MMSYSERR_NOERROR)
                return;
        }
    
        generateSineWaveBlock(frequency, BUFFER_SIZE);
        
        MemZeroT(waveHdr);
        waveHdr.lpData = (LPSTR)audio_data.data();
        waveHdr.dwBufferLength = audio_data.size() * sizeof(short);
        waveHdr.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;   // 标记为循环块
        waveHdr.dwLoops = -1;                       // 循环次数（0=无限）
    
        MMRESULT result = waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(WAVEHDR));
        if (result != MMSYSERR_NOERROR)
            return;
    
        result = waveOutWrite(hWaveOut, &waveHdr, sizeof(WAVEHDR));
        bPlay = true;

        return;
    }

    void Stop()
    {
        bPlay = false;
        if (!hWaveOut)
            return;
        waveOutReset(hWaveOut);
        waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(WAVEHDR));
    }
};

static CWaveOut16 s_waveOut;



CDlgPlayHz::CDlgPlayHz()
{
}

CDlgPlayHz::~CDlgPlayHz()
{
}

TMResult CDlgPlayHz::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgPlayHz::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgPlayHz::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgPlayHz::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgPlayHz::OnPlay(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    s_waveOut.Stop();
    s_waveOut.Play(GetDlgItemInt(IDC_EDIT_FREQ));
    if (!m_ani1)
    {
        m_ani1.SetTimer(1000, this);
        m_tick = GetTickCount();
    }
    return 0;
}

TMResult CDlgPlayHz::OnStop(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    s_waveOut.Stop();
    m_ani1.KillTimer();
    return 0;
}

TMResult CDlgPlayHz::OnEnChange(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    if (s_waveOut.bPlay)
        OnPlay(0, 0, 0, bHandled);
    return 0;
}


//CTuiMsgFilter
// BOOL CDlgPlayHz::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }

void CDlgPlayHz::OnTimer(TMHTimer hTimer, TMClock tmNow)
{
    CSeconds tm((tmNow - m_tick) / 1000);
    CString str;
    str.Format(L"%02d:%02d:%02d", tm.GetHours(), tm.GetMinuteOfHour(), tm.GetSecondOfMinute());
    Item(IDC_EDIT_1016).SetWindowText(str);
}


