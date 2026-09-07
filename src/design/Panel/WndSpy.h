/**
 *    @file
 *    @brief
 *    CWndPad
 *
 *
 *  @author miragekiller <3916345933@qq.com>
 *    @date    2017-11-1
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#ifndef WNDSPY_H
#define WNDSPY_H

#include "wndpad.h"

///////////////////////////////////////////////////////////////////////////////
// CTrackFocusSpy
class CTrackFocusSpy : public TM::CTuiMsgFilter
{
public:
    TM::CAutoTuiWidget m_focus;
    void Enable(BOOL bEnable);
    void TrackFocus(TM::HTUI hWgt);

    //CTuiMsgFilter
    BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);
};


///////////////////////////////////////////////////////////////////////////////
// CPanelSpy
class CPanelSpy
    : public CPanelApp
    , public TM::CTuiMsgFilter
{
public:
    TM_SELF_ALLOC(CPanelSpy);
    
public:
    CTrackFocusSpy m_ts;

private:
    IObjEdit* SyncSpyItem(TM::CTuiObject* parent, TM::CTuiObject* pM);
    void SpySetEmbed(int nMode);

public:
    CPanelSpy(PANELINFO& info);
    ~CPanelSpy();

    // IProj
    virtual TMResult TrackMenuPop(HWND hWnd, TMPoint pt, HMENU hMenuRoot, IObjEdit* pSel) tm_final; // return: command need main window process. 
    virtual TM::CString LoadFromFile(LPCTSTR pFileName, TMBool bMerge) tm_final;
    virtual void TrackFocusObject(TMBool bEnalbe) tm_final { m_ts.Enable(bEnalbe); }

    //CTuiMsgFilter
    virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};


#endif // WNDSPY_H
