/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.02.09
 *
 *  Copyright (C) 2009 miragekiller
 */

#pragma once

#ifndef UIANIMATETE_H
#define UIANIMATETE_H

#include "uiwidget.h"
#include "../tmani.h"


TM_BGN_NAMESPACE

#define animate_trace DbgTrace


#define hVniStop  ((TMHVni)0)
class CAniEx
{
protected:
    CVniID  m_tmID;         // 0:stop    | 0:pause      | n:run
    TMClock m_clock;        // 0         | start time   | start time
    TMInt   m_nCurFrame;    // cur frm   | cur frm      | cur frm
    TMClock m_delay;

    template <class T_this>
    void _Resume(T_this* pthis)
    {
        DbgAssert(IsPause());
        m_tmID.m_tmID = 0;
        m_tmID.SetAni(pthis);
        animate_trace("CAniEx::Resume() : %d", m_tmID.m_tmID);
    }

public:
    CAniEx()
        : m_clock(0)
        , m_delay(100)
        , m_nCurFrame(0)
    {
    }
    
    TMBool IsStop() const { return hVniStop == m_tmID && !m_clock; }
    TMBool IsPause() const { return hVniStop == m_tmID && m_clock; }
    TMBool IsRun() const { return hVniStop != m_tmID; }
    
    void Pause()
    {
        if (!IsRun())
            return;
        
        animate_trace("CAniEx::Pause(%d)", m_tmID.m_tmID);
        m_tmID.KillAni();
    }

    template <class T_this>
    TMBool Play(T_this* pthis, TMUInt interval)
    {
        if (IsPause()) // pause -> play
            _Resume(pthis);
        else if (IsStop()) // stopped -> play
        {
            m_tmID.SetAni(pthis);
            m_delay = interval;
            m_clock = TimeClock();
            animate_trace("CAniEx::Play() : %d", m_tmID.m_tmID);
            return true;
        }
        return false;
    }
    
    template <class T_this>
    TMBool Resume(T_this* pthis)
    {
        if (!IsPause()) // pause -> play
            return false;
        _Resume(pthis);
        return true;
    }
    
    int GetCurrentFrame()
    {
        return m_nCurFrame;
    }

    int CaclFrame(TMClock tm)
    {
        DbgAssert(!IsStop());
        return (tm - m_clock) / m_delay;
    }
    
    void SetCurrentFrame(int nPos, TMClock tmRestart)
    {
        m_nCurFrame = nPos;
        if (tmRestart)
            m_clock = tmRestart - (nPos * m_delay);
    }
    
    void KillAni()
    {
        animate_trace("CAniEx::KillAni(%d)", m_tmID.m_tmID);
        if (IsPause())
            m_tmID.m_tmID = 0;
        else
            m_tmID.KillAni();
        m_clock = 0;
    }
};

class TM_NO_VTABLE CTuiAnimateImpl
    : public CTuiWgt
{
private:
    int m_nRepeat;
    int m_nStart;
    int m_nEnd;
    CAniEx m_ani;

    typedef CAniEx ani_class;

public:
    enum { eMaxInterval = 33 };
    enum 
    {
        CACS_AUTOPLAY       = 0x0004, // ACS_AUTOPLAY
        CACS_NOTIFY         = 0x4000,
        CACS_NOOPTIMIZE     = 0x8000,
    };

public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_AStyle, CSymbol_WsStyle)
        TM_MASK_SYMBOL  (CACS_AUTOPLAY)
        TM_MASK_SYMBOL  (CACS_NOTIFY)
        TM_MASK_SYMBOL  (CACS_NOOPTIMIZE)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    TM_DECLARE_FLAG     (StyleAutoPlay,     m_dwStyle,  CACS_AUTOPLAY);
    TM_DECLARE_FLAG     (StyleNotify,       m_dwStyle,  CACS_NOTIFY);
    TM_DECLARE_FLAG     (StyleNoOptimize,   m_dwStyle,  CACS_NOOPTIMIZE);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_INT    ("Repeat",          m_nRepeat)
        OBJ_PROP_MASK   ("Style",           m_dwStyle,  CSymbol_AStyle)
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()
    
    BEGIN_TUIMSG_MAP    (CTuiAnimateImpl)
        MESSAGE_HANDLER (WM_NCCREATE,       OnCreate)
//         MESSAGE_HANDLER (WM_SHOWWINDOW,     OnShowWindow)
//         MESSAGE_HANDLER (CM_SHOWWINDOW_P,   OnShowWindow)

        ENTRY_LIFE_MAP  (this)
        MESSAGE_HANDLER (CACM_PLAY,         OnPlay)
        MESSAGE_HANDLER (CACM_STOP,         OnStop)
        MESSAGE_HANDLER (CACM_SEEK,         OnSeek)
        MESSAGE_HANDLER (CACM_SEEKCUR,      OnSeekCur)
        LEAVE_LIFE_MAP  ()

        MESSAGE_HANDLER (CACM_GETPOS,       OnGetPos)
        MESSAGE_HANDLER (CACM_GETRANGE,     OnGetRange)
        MESSAGE_HANDLER (CACM_SETRANGE,     OnSetRange)
        MESSAGE_HANDLER (CACM_SETREPEAT,    OnSetRepeat)
        MESSAGE_HANDLER (CACM_GETREPEAT,    OnGetRepeat)
        CHAIN_MSG_MAP   (CTuiWgt)
    END_TUIMSG_MAP      ()

    TMResult OnCreate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnShowWindow(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnPlay(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnStop(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { Stop(); return 0; }
    TMResult OnSeek(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return Seek(wParam); }
    TMResult OnSeekCur(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return Seek(this->GetCurrentFrame() + wParam);  }
    TMResult OnGetPos(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return this->GetCurrentFrame(); }
    TMResult OnGetRange(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetRange(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetRepeat(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetRepeat(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return m_nRepeat; }

    CTuiAnimateImpl();
//     void FinalConstruct(CTuiObject* pParent);
    int GetCurrentFrame() { return m_ani.GetCurrentFrame(); }
//     void CheckFrame();
    void Stop();
    int Seek(int nPos, TMBool bNotify = false, TMClock tmRestart = 0);
//     int ResetDelay();
    void OptimizeTryPause();
    void OptimizeTryStart();
    void SetDirtyAndInvalidate()
    {
        if (!Invalidate())
            OptimizeTryPause();
    }


    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override;
    virtual void OnSetLife(TMUInt32 life) tm_override;

    //  AniProc
    void OnAni(TMHVni hVni, TMClock tmNow, TMClock tmVBlank);

        // CTuiObject
#ifdef TUI_EDIT_MODE
//     virtual void OnLoadProp(IDomElement* pXml) tm_override;
    CString GetDebugInfo() tm_override;
#endif // TUI_EDIT_MODE

};

TM_END_NAMESPACE

#endif // UIANIMATETE_H