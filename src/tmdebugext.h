/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.3.26
 *
 *  Copyright (C) 2019 miragekiller
 */

#pragma once

#ifndef TMDEBUGEXT_H
#define TMDEBUGEXT_H

#include "tmformatmsg.h"
#include "tmtime.h"
#include "tmfile.h"
#include "tmlist.h"
#include "tmthread.h"
#include "tmkeyboard.h"


#ifdef TM_DBG

inline TM::CString MEM2T(TMUInt64 byte)
{
    if (byte < 1024 * 8)
    {
        TM::L2T s(byte, ',');
        s += L'B';
        return s;
    }
    byte /= 1024;
    TM::L2T s(byte, ',');
    s += L'K';
    return s;
}



///////////////////////////////////////////////////////////////////////////////
// memory dump
struct CMemDump
{
    typedef std::pair<TM::CStrViewA, int> key;  // class type, class memory.
    typedef std::pair<int,int> value;           // count, all memory of obj.
    typedef TM::CMap<key, value> CObjMap;
    CObjMap aMap;
    int nAllCount;

    CMemDump() : nAllCount(0) {}

    void Insert(const TMDbgClassInfo* p, int nMem)
    {
        value& v = aMap[key(p->m_pTypeName, p->m_szClass)];
        ++v.first;
        v.second += nMem;
        ++nAllCount;
    }

    void Insert(const TMDbgClassInfo* p)
    {
        Insert(p, TMDbgClassInfo_GetRealSize(p));
    }
    
    int AppendToString(TM::CString& str)
    {
        int nAllMem = 0;
        tm_for(CObjMap::recorder rec(aMap); rec; ++rec)
        {
            const key& k = rec.get_key();
            value v = rec.get_data();
            str.AppendFormat(TM_T("  %5d * %6s + %8s = %10s : "),
                v.first, MEM2T(k.second).c_str(),
                MEM2T(v.second - v.first * k.second).c_str(),
                MEM2T(v.second).c_str());
            str += rec.get_key().first.data();
            str += L"\r\n";
            nAllMem += v.second;
        }
        return nAllMem;
    }

    int AppendToCSV(TM::CString& str)
    {
        int nAllMem = 0;
        str.Append(L"count\tclass size\tall ext size\tall size\t\avg class size\tobject\r\n");
        tm_for(CObjMap::recorder rec(aMap); rec; ++rec)
        {
            const key& k = rec.get_key();
            value v = rec.get_data();
            str.AppendFormat(TM_T("%d\t%d\t%d\t%d\t\""),
                v.first, k.second,
                v.second - v.first * k.second,
                v.second, v.second / v.first);
            str += rec.get_key().first.data();
            str += L"\"\r\n";
            nAllMem += v.second;
        }
        return nAllMem;
    }
};

typedef ADVSET_A(const TMDbgClassInfo*, const char*, ->m_pClass, dbg_allocator<const TMDbgClassInfo*>) _CDebugLeakInfoList;
//typedef ADVSET(const TMDbgClassInfo*, const char*, ->m_pClass) _CDebugLeakInfoList;
struct CDebugLeakInfoList : public _CDebugLeakInfoList { STL_DECLARE_RECORDER(CDebugLeakInfoList, TM::eListTypeMap); };
typedef CDebugLeakInfoList::recorder CDebugLeakInfoRec;
typedef CDebugLeakInfoList::iterator CDebugLeakInfoIter;



struct CCallStatck
{
    int    m_sizeCallStack;
    void*  m_addrCallStack[30];

    CCallStatck() { m_sizeCallStack = _dbgApp.GetCallStack(tm_countof(m_addrCallStack), &m_addrCallStack[0]); }
};

#endif // #ifdef TM_DBG


TM_BGN_NAMESPACE


#define TM_HOTKEY_EXT           VK_F4
#define TM_HOTKEY_EXT_CLASS     TM_T("TMDui_hotkey_ext")

///////////////////////////////////////////////////////////////////////////////
//  CDebugCSVFile
struct CDebugCSVFile
{
    TM::CFile m_file;

    CDebugCSVFile(const TM::CStdStr& fn)
    {
        TM::CTime tm(TM::CTime::GetCurrentTime());
        TMCharT c[256];
        tm.Format(c, 255, fn + L" %c.csv"); // L"c:\\temp\\TMDui Analysis %c.csv");
        std::replace(&(c[20]), &(c[45]), ':', '_');
        std::replace(&(c[20]), &(c[45]), '/', '_');
        m_file.CreateNew(c, m_file.e_faWrite);
    }

    void Append(const std::string_view& str)
    {
        if (m_file)
            m_file.Write(str.data(), str.length());
    }
};


///////////////////////////////////////////////////////////////////////////////
//  CDebugHotKey
struct IDebugHotkeySink
{
    virtual void Flip() = 0; // tm_override
};

template <int vk>
struct CDebugHotKeyT
{
protected:
    typedef TM::CSet<IDebugHotkeySink*> CSinkList;
    CSinkList   m_aSink;
    
#if defined(TM_WIN) && !defined(TM_QT)
    TMHWnd      m_hWnd;

private:
    void Init()
    {
        WNDCLASSEX  wc =
        {
            sizeof(WNDCLASSEX), 0/*CS_GLOBALCLASS*/, HotkeyProc, 0, 0, 0, 0, 0, 0, 0, TM_HOTKEY_EXT_CLASS, 0
        };
        
        DbgVerify(RegisterClassEx(&wc));
        m_hWnd = CreateWindowEx(0, TM_HOTKEY_EXT_CLASS, 0, 0, 0, 0, 0, 0, ((HWND)-3)/*HWND_MESSAGE*/, 0, 0, 0);
        DbgVerify(RegisterHotKey(m_hWnd, vk, 0, vk));
    }

    static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
    {
        KillTimer(0, idEvent);
        Inst().Init();
    }

    static LRESULT CALLBACK HotkeyProc(HWND hWnd, UINT Msg, TMWParam wParam, LPARAM lParam)
    {
        if (Msg == WM_HOTKEY)
        {
            if (HIWORD(lParam) == vk)
                Flip();
        }
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    CDebugHotKeyT()
    {
        // MS spy++ maybe freeze if create window here.
        ::SetTimer(0, 0, 300, TimerProc);
    }
    
public:
    ~CDebugHotKeyT()
    {
        DbgVerify(UnregisterHotKey(m_hWnd, vk));
        DbgVerify(DestroyWindow(m_hWnd));
        DbgVerify(UnregisterClass(TM_HOTKEY_EXT_CLASS, 0));
    }
#endif // TM_WIN

public:
    static void Flip()
    {
        for (CSinkList::recorder rec(Inst().m_aSink); rec; ++rec)
            rec.get_data()->Flip();
    }
    
    static CDebugHotKeyT& Inst()
    {
        static CDebugHotKeyT s_inst;
        return s_inst;
    }

public:
    static void AddSink(IDebugHotkeySink* p) { Inst().m_aSink.insert(p); }
    static void DelSink(IDebugHotkeySink* p) { Inst().m_aSink.erase(p); }
};


///////////////////////////////////////////////////////////////////////////////
// CDebugAnalysis
struct CDebugAnalysis : public IDebugHotkeySink
{
    struct CMsgResult
    {
        int nCount;
        TMInt64 timeJ;      // jin time
        TMInt64 timeM;      // mao time
        TMInt64 timeJMax;   // max jin time

        CMsgResult()
            : nCount(0)
            , timeJ(0)
            , timeM(0)
            , timeJMax(0)
        {
        }

        void SetTimeJ(TMInt64 tm)
        {
            tm_setmax(timeJMax, tm);
            timeJ += tm;
        }
    };

    struct MSGKEY
    {
        TMUInt32 idobj;
        TMUInt32 msg;
        TMUInt32 id;
        TMCStrA type; // 0, "premsg", "route", "send", "window"

        bool operator < (const MSGKEY& rhs) const
        {
            if (idobj != rhs.idobj)
                return idobj < rhs.idobj;
            if (msg != rhs.msg)
                return msg < rhs.msg;
            if (id != rhs.id)
                return id < rhs.id;
            return type < rhs.type;
        }
    };

    struct STACKITEM
    {
        STACKITEM*  old;
        CMsgResult* msg;
        TMInt64     timeJ; // jin time
        bool        bNull;
    };

    typedef TM::CMap<MSGKEY, CMsgResult>    CItemMap;
    typedef CDebugHotKeyT<TM_HOTKEY_EXT>    CDebugHotKey;

    CItemMap        m_aAllMsg;
    CStdStr         m_strFn;
    TM::CCycle      m_cycle;
    TMThreadID      m_idThread;
    STACKITEM*      m_pCurr;

    CDebugAnalysis(const CStdStr& strFn)
        : m_strFn(strFn)
        , m_pCurr(0)
    {
        DbgTrace("CDebugAnalysis::CDebugAnalysis()");
        m_idThread = ThreadSelfId();
        CDebugHotKey::AddSink(this);
    }

    ~CDebugAnalysis()
    {
        CDebugHotKey::DelSink(this);
        Flip();
        DbgTrace("CDebugAnalysis::~CDebugAnalysis()");
    }

private:
    TMUInt32 WParam2ID(TMUInt uMsg, TMWParam wParam)
    {
#ifdef TM_WIN
        switch (uMsg)
        {
        case WM_COMMAND: return LOWORD(wParam);
        case WM_SYSCOMMAND: return wParam;
        }
#endif
        return 0;
    }

public:
    void Push(STACKITEM& sitm, TMCStrA nType, TMUInt idObj, TMUInt uMsg, TMWParam wParam)
    {
        if (m_idThread != ThreadSelfId())
            return;
			
        TMInt64 dtime = m_cycle.StepUp();

        if (m_pCurr)
            m_pCurr->timeJ += dtime;

        MSGKEY itm;
        itm.idobj = idObj;
        itm.msg = uMsg;
        itm.id = WParam2ID(uMsg, wParam);
        itm.type = nType;

        CMsgResult& result = m_aAllMsg[itm];
        ++result.nCount;;
        sitm.msg = &result;
        sitm.timeJ = 0;
        sitm.bNull = !nType;
        sitm.old = m_pCurr;
        m_pCurr = &sitm;
        m_cycle.Reset();
    }
    
    TMInt64 Pop()
    {
        if (m_idThread != ThreadSelfId())
            return 0;

        if (!m_pCurr) // maybe released by hot key
            return 0;
        TMInt64 dtime = m_cycle.StepUp();
        STACKITEM& itm = *m_pCurr;
        dtime += itm.timeJ;                     // make J time
        itm.msg->timeJ += dtime;
        tm_setmax(itm.msg->timeJMax, dtime);

        // add time to all parent node;
        if (itm.bNull)
            itm.msg->timeM += dtime;
        else
            for (STACKITEM* p = &itm; p; p = p->old)
                p->msg->timeM += dtime;

        m_pCurr = itm.old;
        m_cycle.Reset();
        return dtime;
    }

    void PushNull()
    {
        Push(* new STACKITEM(), 0, 0, 0, 0);
    }

    TMInt64 PopNull()
    {
        STACKITEM* p = m_pCurr;
        TMInt64 x = Pop();
        delete p;
        return x;
    }

    virtual void Flip() tm_override
    {
        // create CSV file
        CDebugCSVFile csvfile(m_strFn);
        csvfile.Append("msg,command id,window,type,call count,times(us),AVG time(us),net times(us),AVG net time(us),max net times(us)\r\n");

        // flip all analysis
        int nLine = 2;
        for (CItemMap::recorder rec(m_aAllMsg); rec; ++rec, ++nLine)
        {
            const MSGKEY& mk = rec.get_key();
            CMsgResult& mr = rec.get_data();
            
            CStdStrA str(DbgFormatMessage(mk.msg));
            csvfile.Append(str);

            char str2[2048];
            int fps = (TM::CCycle::Frequency() / 1000000);
            int n = mr.timeM / fps;
            int m = mr.timeJ / fps;
            int k = mr.timeJMax / fps;
//             StrPrintf(str2, tm_countof(str2), ",%d,%d,%s,%d,%d,%d,%d,%d,%d\r\n",
//                 mk.id, mk.idobj, mk.type, mr.nCount, n, n / mr.nCount, m, m / mr.nCount, k);
            StrPrintf(str2, tm_countof(str2), ",%d,%d,%s,%d,%d,=F%d/E%d,%d,=H%d/E%d,%d\r\n",
                mk.id, mk.idobj, mk.type, mr.nCount, n, nLine, nLine, m, nLine, nLine, k);
            csvfile.Append(str2);
        }

        m_aAllMsg.clear();
        for (STACKITEM* p = m_pCurr; p; p = p->old)
            p->old = 0;
        m_pCurr = 0;
    }
};


#define ANALYSIS_TUI_MSG    1
#define ANALYSIS_TUI_USER   2


template <int nType>
struct CAutoAnalysisT : public CDebugAnalysis::STACKITEM
{
    static CDebugAnalysis& Instance()
    {
        static CDebugAnalysis _inst(VA2T(TM_T("c:\\temp\\TMDui Analysis(%d)"), nType));
        return _inst;
    }

    CAutoAnalysisT(TMCStrA pType, TMUInt idObj = 0, TMUInt uMsg = 0, TMWParam wParam = 0)
    {
//         DbgAssert(pType);
        Instance().Push(*this, pType, idObj, uMsg, wParam);
    }
    
    ~CAutoAnalysisT()
    {
        Instance().Pop();
    }
};

struct CAutoAnalysisNull
{
    CAutoAnalysisNull(...) {}
};

typedef CAutoAnalysisT<ANALYSIS_TUI_MSG>    CAutoAnalysisTuiMsg;
typedef CAutoAnalysisT<ANALYSIS_TUI_USER>   CAutoAnalysisTuiUser;


#ifdef TM_DBG
#   define CAutoMsgAnalysis                 CAutoAnalysisTuiMsg
#   define TM_ANALYSIS_UNUSED(x)            TM::CAutoAnalysisTuiMsg x(0)
#   define TM_ANALYSIS_UNUSED_BEGIN()       CAutoAnalysisTuiMsg::Instance().PushNull()
#   define TM_ANALYSIS_UNUSED_END()         CAutoAnalysisTuiMsg::Instance().PopNull()
#else
//#define CAutoMsgAnalysis TM_SW_DBG(CAutoAnalysisTuiMsg, CAutoAnalysisNull)
#   define CAutoMsgAnalysis                 CAutoAnalysisNull
#   define TM_ANALYSIS_UNUSED(x)
#   define TM_ANALYSIS_UNUSED_BEGIN()
#   define TM_ANALYSIS_UNUSED_END()
#endif



TM_END_NAMESPACE



#endif //TMDEBUGEXT_H
