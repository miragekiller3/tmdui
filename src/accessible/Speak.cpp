/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.9.5
 *
 *  Copyright (C) 2018 miragekiller
 */



#include "stdafx.h"
#include "speak.h"
#include "tmdll.h"


// #import "C:\Program Files\Freedom Scientific\JAWS\2020\AccessibleObjectTools.dll"
// //#import "C:\Program Files\Freedom Scientific\JAWS\2021\x86\FsDomSrv.dll"
// #import "C:\Program Files\Freedom Scientific\JAWS\2020\x86\FsDomNodes\FSDomNodeSymphony.dll"
// #import "C:\Program Files (x86)\Freedom Scientific\Shared\FSAPI\1.0\FSAPI.dll"
// #import "C:\ProgramData\Freedom Scientific\JAWS\2020\Locale\enu\Help\HHActiveX.dll"
// #import "C:\Program Files\Freedom Scientific\JAWS\2020\FsDomNodes\FSDomNodeSymphony.dll"
// #import "C:\Program Files\Freedom Scientific\JAWS\2020\AccessibleObjectTools.dll"


// jaws api
#import "win/fsapi.dll" no_implementation

// Microsoft Speech API (SAPI)
#pragma warning (push)
#pragma warning (disable : 4129) //S' : unrecognized character escape sequence
#pragma warning (disable : 4305) //'initializing' : truncation from 'const double' to 'const float'
#pragma warning (disable : 4192) //automatically excluding 'wireHWND' while importing type library 'sapi.dll'
#import "win/sapi.dll" no_implementation
#pragma warning (pop)

// nvda sdk
typedef error_status_t (__stdcall *PFN_nvdaController_testIfRunning)(void);
typedef error_status_t (__stdcall *PFN_nvdaController_speakText)(const wchar_t *text);
typedef error_status_t (__stdcall *PFN_nvdaController_cancelSpeech)(void);
typedef error_status_t (__stdcall *PFN_nvdaController_brailleMessage)(const wchar_t *message);
#define NVDADLL_NAME TM_SW_WIN64(L"nvdaControllerClient64.dll", L"nvdaControllerClient32.dll")



TM_BGN_NAMESPACE

using namespace SpeechLib;


///////////////////////////////////////////////////////////////////////////////
// CAccSpeakDeviceJaws
struct CAccSpeakDeviceJaws : public CAccSpeak::IDevice
{
    TM_SELF_ALLOC(CAccSpeakDeviceJaws);

    CComPtr<FSAPILib::IJawsApi>     m_ptrJaws;

    CAccSpeakDeviceJaws()
    {
        m_ptrJaws.CoCreateInstance(__uuidof(FSAPILib::JawsApi), 0, CLSCTX_INPROC_SERVER);
        DbgAssert(m_ptrJaws);
    }

    virtual TMBool Say(LPCWSTR pStr) tm_final
    {
        if (!IsLoad())
            return false;
        VARIANT_BOOL varret = VARIANT_FALSE;
        DbgVerifyHResult(m_ptrJaws->raw_SayString((BSTR)pStr, VARIANT_TRUE, &varret));
        return varret;
    }

    TMBool IsLoad() const { return m_ptrJaws != 0; }
};

///////////////////////////////////////////////////////////////////////////////
// CAccSpeakDeviceNvda
struct CAccSpeakDeviceNvda : public CAccSpeak::IDevice
{
    TM_SELF_ALLOC(CAccSpeakDeviceNvda);

    PFN_nvdaController_speakText        m_speakText;
    PFN_nvdaController_testIfRunning    m_testIfRunning;

    CAccSpeakDeviceNvda()
    {
        CDllHandle dll;
        dll.LoadLibrary(NVDADLL_NAME);
        if (dll)
        {
            m_speakText = (PFN_nvdaController_speakText)dll.GetProcAddress("nvdaController_speakText");
            m_testIfRunning = (PFN_nvdaController_testIfRunning)dll.GetProcAddress("nvdaController_testIfRunning");
        }
        else
        {
            m_speakText = 0;
            m_testIfRunning = 0;
        }
    }
    
    virtual TMBool Say(LPCWSTR pStr) tm_final
    {
        if (!IsLoad())
            return false;

        DbgAssert(m_speakText);
        HRESULT hr;
        DbgVerifyHResult(hr = m_speakText(pStr));
        if (FAILED(hr))
            DbgVerifyHResult(hr = m_testIfRunning());
        return SUCCEEDED(hr);
    }

    TMBool IsLoad() const { return m_speakText && m_testIfRunning; }
};

///////////////////////////////////////////////////////////////////////////////
// CAccSpeakDeviceSapi
struct CAccSpeakDeviceSapi : public CAccSpeak::IDevice
{
    TM_SELF_ALLOC(CAccSpeakDeviceSapi);

    CComPtr<SpeechLib::ISpVoice>    m_ptrSpVoice;
    
    CAccSpeakDeviceSapi()
    {
        m_ptrSpVoice.CoCreateInstance(__uuidof(SpeechLib::SpVoice), 0, CLSCTX_INPROC_SERVER);
    }
    
    virtual TMBool Say(LPCWSTR pStr) tm_final
    {
        if (!IsLoad())
            return false;
        DbgAssert(m_ptrSpVoice);
        DWORD flags = SVSFlagsAsync | SVSFIsNotXML | SVSFPurgeBeforeSpeak;
        HRESULT hr;
        DbgVerifyHResult(hr = m_ptrSpVoice->raw_Speak((BSTR)pStr, flags, NULL));
        return SUCCEEDED(hr);
    }

    TMBool IsLoad() const { return m_ptrSpVoice != 0; }
};

///////////////////////////////////////////////////////////////////////////////
// CAccSpeak
CAccSpeak::CAccSpeak()
{
}

TMBool CAccSpeak::InitDevice()
{
    if (m_ptrDevice)
    {
        if (::IsWindow(m_hWnd))
            return true;
        else
            m_ptrDevice.reset(0);
    }
    DbgAssert(!m_ptrDevice);

    if (m_hWnd = FindWindow(L"JFWUI2", NULL))
    {
        CAccSpeakDeviceJaws* p = new CAccSpeakDeviceJaws();
        m_ptrDevice.reset(p);
        m_dev = eSpeakDevJAWS;
        return true;
    }

    if (m_hWnd = FindWindow(L"wxWindowClassNR", L"NVDA"))
    {
        CAccSpeakDeviceNvda* p = new CAccSpeakDeviceNvda();
        m_ptrDevice.reset(p);
        if (COSVersion::GetVersionDetail() >= COSVersion::MakeDetail(COSVersion::eVersionWin10, 18000))
            m_dev = eSpeakDevNVDAThan1800;
        else
            m_dev = eSpeakDevNVDALess1800;
        return true;
    }

    if (m_hWnd = FindWindow(L"NarratorUIClass", L"Narrator Settings"))
    {
        CAccSpeakDeviceSapi* p = new CAccSpeakDeviceSapi();
        m_ptrDevice.reset(p);
        m_dev = eSpeakNarrator;
        return true;
    }

    return false;
}

TMBool CAccSpeak::SayString(LPCWSTR pStr, ESpeakDev flag)
{
    if (!pStr || !pStr[0] || !InitDevice())
        return false;
    if (flag & m_dev)
        return m_ptrDevice->Say(pStr);
    return true;
}


TM_END_NAMESPACE


