/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2013.12.07
 *
 *  Copyright (C) 2013 miragekiller                               
 */

#pragma once

#ifndef VIEW_H
#define VIEW_H

#include "drag.h"
#include "../tmhook.h"
#include "uiwidget.h"
#include "input.h"

class CPanelApp;


TM_BGN_NAMESPACE


class CTuiWnd;

////////////////////////////////////////////////////////////////////////////////
// CMsgFilterList
class CMsgFilterList : public CCallSetT<CCallProcT<CTuiMsgFilter*> >
{
public:
    ~CMsgFilterList()
    {
        this->Clear();
    }
    
    TMBool PreTranslateMessage(TUIMSG* pMsg, TMResult& lResult)
    {
        DbgAssert(pMsg->hWnd);
        DbgAssert(pMsg->hWnd->m_nRef);

        typedef CCallSetT<CCallProcT<CTuiMsgFilter*> > base_class;
        base_class::CPrepareCallChain prepare(*this);
        CALLSET_BEGIN_CALL(0);
        if (p->PreTranslateMessage(pMsg, lResult))
            return true;
        CALLSET_END_CALL();
        return TMFalse;   // not translated
    }
};

////////////////////////////////////////////////////////////////////////////////
// CView
class CView
    : public CTuiApp
    , public IGXAppNotify
    , public CFocus
{
public:
    typedef CMap<TMHModule, CModule*>   CModuleList;

    TM_IF_DBG(int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf) { return IC_Size(m_aModules) + IC_Size(m_aFilter.m_aPorc); });
    
protected:
    CModuleList         m_aModules;
    CMsgFilterList      m_aFilter;

public:
    bool                m_bSystemFrame;
    bool                m_bShowHintOnEllipsis;
    bool                m_bShowHintOnTabKey;
    bool                m_bHighContrast;
    bool                m_bAnimationButton;
    bool                m_bAnimationScroll;
    bool                m_bAnimationSysBut;
    TMColor             m_clrDisableText;
    TMColor             m_clrEditSelBackground;
    TMColor             m_clrColorEditSelText;
    TMColor             m_clrColorEditPrompt;

    static bool         s_bWebview2AllowSingleSignOnUsingOSPrimaryAccount;
    static CString      s_strWebview2Arguments;
    static CString      s_strWebview2Language;
    static CString      s_strWebview2ExecutableFolder;
    static CString      s_strWebview2UserDataFolder;

public:
    TUIHITINFO          m_htOver;
    TUIHITINFO          m_htPrev;
    TUIHITINFO          m_htDownL;
    TUIHITINFO          m_htDownR;
    TUIHITINFO          m_htDownM;
    HTUI                m_pCapture;
    TMULong             m_dwState;
    
    CDropMgr            m_mgrDrag;
    
public:
    friend struct _CViewProp;
    friend CPanelApp;
    
public:
    CView();
    ~CView();
    
    CModule* GetModule(TMHModule hRes)
    {
        CModuleList::iterator it = m_aModules.find(hRes);
        if (it != m_aModules.end())
        {
            return it->second;
        }
        else
        {
            return 0;
        }
    }

    CModule* CreateModule(TMHModule hRes)
    {
        CModule*& md = m_aModules[hRes];
        if (!md)
            md = new CModule(this, hRes);
        return md;
    }
    
    TMBool IsSysFrame() { return m_bSystemFrame; }
    void OnSettingChanged();
//     void OnDisplayChanged();
//     static TMClock GetNextVBlankTicket();
    static TMBool IsDragDistance(const TUIPoint& pt1, const TUIPoint& pt2);
    void DispatchModule(TMUInt msg, TMWParam wp, TMLParam lp);
    
    // CTuiApp
    virtual void Term() tm_override;
    virtual TMBool LoadXml(TMHModule hRes, TMCStr pBase, TMCStr pLang) tm_override;
    virtual void UnLoadXml(TMHModule hRes) tm_override;
    virtual void SetColorTransform(TMHModule hRes, IColorTransform* pct) tm_override;

    virtual CStrPtr LoadString(TMHModule hRes, int nID, const CStrView* strXmlProperty) tm_override;
    virtual TMHMenu LoadMenu(TMHModule hRes, TMCStr pMenu) tm_override;
    GXPTexture LoadTexture(TMHModule hRes, TMCStr lpszName, TMCStr uType);
    virtual TMResult GetResourceStream(TMHModule hRes, TMCStr lpszName, TMCStr uType, struct IStream** ppStream) tm_override;
    virtual CComPtr<ITuiPic> GetPic(TMHModule hRes, int nID, int dpi) tm_override;
    virtual CComPtr<ITuiPic> CreateCustomPic(TMHModule hRes, int nIDObj, IStream* stream) tm_override;
    virtual void RegisterCallbackFactory(TMHModule hRes, int nObjectID, const TUIFACTORY& factory) tm_override;
    
private:
    void MoveMouse(HTUI hOwner, TUIPoint pt, TMUInt uMsg);

    //WM_SETCURSOR, WM_MOUSEWHEEL, WM_MOUSELEAVE, WM_CAPTURECHANGED
    TMBool CheckCaptureChange(TMResult& lResult);

public:
    // CTuiApp
    virtual TMBool SetProperty(EAppProperty idProp, const void* pVol, int nVolSize) tm_override;
    virtual TMBool GetProperty(EAppProperty idProp, void* pVol, int nVolSize) tm_override;

    virtual TMBool AddMessageFilter(CTuiMsgFilter* pMessageFilter) tm_override;
    virtual TMBool RemoveMessageFilter(CTuiMsgFilter* pMessageFilter) tm_override;
    
    virtual void GetMouseHotState(TUIHITINFO& ht) tm_override;
    
    // return: is the button down
    virtual TMBool GetMouseLDownState(TUIHITINFO& ht) tm_override ;
    virtual TMBool GetMouseRDownState(TUIHITINFO& ht) tm_override;
    virtual TMBool GetMouseMDownState(TUIHITINFO& ht) tm_override;
    virtual TMBool GetGestureState(TUIHITINFO& ht, int nID) tm_override;
    virtual CKeyInput GetKeyInput() tm_override { return g_input.m_key; };

    virtual TMBool SetCapture (CTuiWgt* pWin) tm_override;
    virtual HTUI GetCapture() tm_override;
    virtual HTUI GetFocus() tm_override; // manager always can't focus.

    virtual void KillFocus() tm_override;
    virtual TMBool SetFocus(HTUI hWnd, EFocusReason reason) tm_override { return CheckSetFocus(hWnd, true, true, reason); }
    virtual void SetFocusChain(HTUI hFirst, HTUI hSecond) /*tm_override*/{ g_input.SetFocusChain(hFirst, hSecond); }

    virtual HTUI CreateDlgEx (TMHModule hInst, int nID, TMHWnd hWndParent, TUIFACTORY* pFactory, void* param) tm_override;
    virtual HTUI CreateDlgByHWND(TMHModule hInst, int nID, TMHWnd hWnd, TUIFACTORY* pFactory, void* param) tm_override;
    virtual void EnumDialogs(PFNWWENUMPROC lpEnumFunc, TMLParam lParam) tm_override;

    virtual int GetMenuPopLevel(HTUI hWnd, TMBool bOwner) tm_override;
    virtual TMBool EndMenuPopLevel(int nLevel, int nResult, TMBool bRestoreFocus, TUIMSG* p) tm_override;
    virtual TMBool GetMenuPopInfo(int nLevel, HTUI* pOwner, HTUI* pPop) tm_override;

    void ReleaseCapture() { SetCapture(0); }
    CTuiWnd* CreateMgr(TMHModule hInst, int nID, int dpi);
    TMBool DoMenuPop(HTUI hOwner, HTUI hWnd, HTUI hFocus);
    HTUI GetMenuPop(HTUI hWnd);

private:
    // IGXAppNotify
    virtual void OnGXDeviceChanged(IGXApp* pNew) tm_override;

public:
    CTuiWgt* GetOver() { return m_htOver.hWnd; }
    CTuiWgt* GetClientOver() { return m_htOver.bNC ? 0 : m_htOver.hWnd; }
    TMBool IsHoting(HTUI hWnd) const;
    bool IsMouseDown () { return 0 != (m_dwState & MK_LBUTTON); }
    //    bool IsDragging () const { return 0 != (m_dwPriStyle & MGR_PRI_DRAG); }
    
    TUIHITINFO& GetHitInfo() { return m_htOver; }
    TUIHITINFO& GetDownInfo() { return m_htDownL; }
    
    const CTuiWgt* GetDown () { return m_htDownL.hWnd; }

    TMBool GetPrevHitInfo (HTUI hWgt, TUIHITINFO* pInfo)
    {
        if (GetOver() == hWgt)
        {
            if (pInfo)
                *pInfo = m_htOver;
            return true;
        }
        
        return false;
    }
    
    TMBool GetMouseDownHitInfo (HTUI hWgt, TUIHITINFO* pInfo)
    {
        if (GetDown() == hWgt)
        {
            if (pInfo)
                *pInfo = m_htDownL;
            return m_dwState & MK_LBUTTON;
        }
        
        return false;
    }

    TMBool DoPreTranslateMessage(TUIMSG* pMsg, TMResult& lResult);
    TMBool DoPreTranslateAppMessage(HTUI hMgr, TMHWnd hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult);

    TMBool DispatchAppMessage(HTUI hMgr, TMHWnd hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult);

    TMBool IsBasicMouseMessage(TMUInt uMsg) { return uMsg >= WM_MOUSEFIRST && uMsg <= WM_MBUTTONDBLCLK; }
    void DispatchBasicMouseMessage(HTUI hOwner, TMUInt uMsg, TMWParam wParam, CPointI ptCur, int dx = 0, int dy = 0);
    
//     TUIHITINFO* DispatchGestureHitTest(HTUI hOwner, const TMPointI& ptCur);
//     void DispatchGestureMove(HTUI hOwner, int nType, CPointI ptCur);

    void OnAppSize(HTUI pMgr, long width, long height);
    
    void SetHitHot(const TUIHITINFO& ht);
    void SetHitHotLeave(HTUI hMgr, TMHWnd hWnd);

//     void SwitchMouse();

    void ClearChildInfo(CTuiWgt* p, TMUInt uMsg);
//     void ClearChildInfo (CTuiWgt* p);
    void ClearChildHitInfo (CTuiWgt* p, void* pItem);
    
    CDropMgr& GetDragMgr() { return m_mgrDrag; }

public:
    virtual void InternalSetCapture(const CTuiWgt* hWgt) = 0;
    virtual void InternalReleaseCapture() = 0;
    virtual TMBool InternalIsCapture(const CTuiWgt* hWgt) = 0;
    virtual void InternalSetFocus(const CTuiWgt* hWgt) = 0;
    virtual TMBool InternalIsWindowActive(const CTuiWgt* hWgt) { return 1; }
    virtual CPoint InternalGetScreenOrg(HTUI hWgt) { return CTuiPoint(0,0); }
};


TM_END_NAMESPACE


#endif // VIEW_H
