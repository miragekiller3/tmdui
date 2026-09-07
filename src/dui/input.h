/*
 *  @file
 *  @brief  input: focus, caret, gesture
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.7.4
 *
 *  Copyright (C) 2019 miragekiller
 */

#pragma once

#ifndef INPUT_H
#define INPUT_H

#include "../tmtimer.h"
#include "../tmkeyboard.h"
#include "../tmthread.h"
#include "../tmdui.h"
#include "../tmhook.h"
#include "uiwidget.h"


TM_BGN_NAMESPACE

class CTuiWnd_Win;


///////////////////////////////////////////////////////////////////////////////
// CGesture
class CGesture
{
private:
    enum { eGIDBgn = GID_ZOOM };
    TUIHITINFO  m_htGesture;
    HTUI        m_hGesture[GESTURE_ID_COUNT];

    void OnWMGestureNotify(HTUI hMgr, TMLParam lParam);
    TMBool OnWMGesture(HTUI hMgr, TMLParam lParam);
    
public:
    CGesture();
    TMBool ProcessMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam);
    void Clear(HTUI hWnd);
    void ClearItem(HTUI hWnd, void* pItem);
    TMBool GetState(TUIHITINFO& ht, int nID);

    static TMResult OnCMGestureNotify(HTUI hWnd);
    static TMBool OnCMGesture(HTUI hWnd, TMWParam wParam);
};


///////////////////////////////////////////////////////////////////////////////
// HotKey
class CHotKeyMap
{
public:
#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        return IC_Size(m_aHotKey);
    }
#endif // TM_DBG

protected:
    struct CHotKeyStore : public CPropHotKeyID
    {
    public:
        HTUI m_hOwner;
        
        CHotKeyStore(TMUInt32 key = 0)
            : CPropHotKeyID(key,0)
            , m_hOwner(0)
        {}
        
        CHotKeyStore(const CPropHotKey& key, HTUI hOwner, TMUInt32 id)
            : CPropHotKeyID(key, id)
            , m_hOwner(hOwner)
        {
        }
    };

    typedef CVector<CHotKeyStore> CHotKeyList;
    CHotKeyList m_aHotKey;
    friend struct CInputHotKey;
    friend class CKeyboardDebugInfo;

    void _Done(const CHotKeyStore* p);
    const CHotKeyStore* _TranslateHotKey(TMUInt uMsg, TMWParam wparam, TMLParam lparam);
    TMBool _Check(CPropHotKey hotkey, HTUI hWnd);
    int Add(const CPropHotKey& hotkey, HTUI hWnd, TMUInt cmd);
    int AddMT(const CPropHotKey& hotkey, HTUI hWnd, TMUInt cmd);
    int Erase(const CPropHotKey& hotkey, HTUI hWnd);
    int EraseMT(const CPropHotKey& hotkey, HTUI hWnd);
    int size() const { return m_aHotKey.size(); }

public:
    ~CHotKeyMap() { DbgAssert(size() == 0); }
    TMBool TranslateHotKeyMT(TMUInt uMsg, TMWParam wparam, TMLParam lparam);
};

struct CInputHotKey
{
private:
#ifdef TM_WIN
    struct _HotKeySys
        : public CHotKeyMap
        , public CHookThreadKeyboardLLImpl<_HotKeySys>
    {
        TMBool OnKeyboardLL(UINT uMsg, KBDLLHOOKSTRUCT* ps)
        {
//              if (ps->flags & LLKHF_INJECTED) 
//                  return false;
            return this->TranslateHotKeyMT(uMsg, ps->vkCode, 0);
        }
    };
#else
    struct _HotKeySys : public CHotKeyMap
    {
        void SetHook() {}
        void RemoveHook() {}
    };
#endif // TM_WIN

    typedef CAutoLockT<CCritical> CLock;
    friend CHotKeyMap;

    CCritical           m_criticalHotKey;
    _HotKeySys          m_hotKeyAppMT;
    _HotKeySys          m_hotKeySysMT;
    bool                m_bActivateApp;

public:
    CInputHotKey();
    void AddHotkey(CPropHotKey hk, HTUI hWnd, TMUInt nCmd);
    void RemoveHotkey(CPropHotKey hk, HTUI hWnd);
    void OnActivateApp(TMBool bActivate);
};

///////////////////////////////////////////////////////////////////////////////
// FocusChain
class CFocusChain
{
private:
    typedef TM::CMap<HTUI, HTUI> CFCList;
    CFCList m_aChains[2]; // next, prev

    TMBool RemoveFC(CTuiWidget wnd, int index);

public:
    void RemoveFocusChain(HTUI hWnd);
    void SetFocusChain(HTUI hFirst, HTUI hSecond);
};


///////////////////////////////////////////////////////////////////////////////
// CInput
struct CInput
    : public CTimerImpl<CInput>
    , public CGesture
    , public CInputHotKey
    , public CFocusChain
{
public:
    CTuiWnd*            m_hFocusMgr;
    HTUI                m_hCaret;
    CTuiRect            m_rcCaret;
    bool                m_bShowCaret;
    bool                m_bKorea;
    CKeyInput           m_key;

public:
    CInput();

    CGesture& Gesture() { return *this; }
    void Clear(HTUI hWnd);

    // CTimer
    void OnTimer(TMHTimer hTimer, TMClock tmNow);

    // caret
    void CaretCreate(HTUI pWin);
    void CaretDestroy(HTUI pWin);
    void CaretReset(HTUI pWin);
    void CaretDraw(IGXApp* gapp, HTUI pWin, const CTuiRect& rc);

    // IME
    void ImeOnStartCompostion();
    void ImeEnable(HTUI hWgt, TMBool bEnable);

    // focus
    void FocusOnSet(CTuiWnd& mgr);
    void FocusOnKill(CTuiWnd& mgr);

    // state
    EButStates GetScrollStates(const TUIHITINFO& cur, const TUIHITINFO* pHot, TMBool bHotSB);

};


///////////////////////////////////////////////////////////////////////////////
// CFocus
class CFocus
{
private:
    bool            m_bFoucing;
    Ref             m_pFocus;

private:
    TMBool OnTabKey(CTuiWnd* hMgr, CKeyInput key);
    HTUI SetNextTab(CTuiWnd* hMgr, TMBool bReverse);
    HTUI SetNextTab(CTuiWnd* hMgr, HTUI hNow, TMBool bReverse, TMBool bTest);
    TMBool SetNextArrow(TMBool bReverse);

public:
    CFocus() : m_bFoucing (0), m_pFocus(0) {}
    TMBool IsFocuing() { return m_bFoucing; }
    HTUI GetFocus(); // manager always can't focus.
    HTUI GetNextTab(HTUI hNow, TMBool bReverse) { return SetNextTab(hNow->GetOwner(), hNow, bReverse, true); }
    TMBool CheckSetFocus(CTuiWgt* pWin, TMBool bCheck, TMBool bActive, EFocusReason reason);
    void CleanFocus(HTUI hWgt);
    TMBool IsArrowGroupMessage(CTuiWnd* hMgr, TUIMSG* pMsg); // return true to stop message
    TMBool IsDialogMessage(CTuiWnd* hMgr, TUIMSG* pMsg); // return true to stop message
    TMBool AppOnKey (HTUI pMgr, TMUInt uMsg, TMWParam wParam, TMLParam lParam); // return: bHandled
    void AppOnSetFocus(HTUI hMgr, TMHWnd hWnd); // call when mgr::HWND receive WM_SET/KILLFOCUS.
//     void AppOnKillFocus(HTUI hMgr, TMHWnd hWnd);
};

extern CInput g_input;


class CSubFocus
{
private:
    int m_nCurrentTab;
    
public:
    CSubFocus() : m_nCurrentTab(0) {}
    
    int GetCurrentTab() const { return m_nCurrentTab; }
    
    TMBool OnTabKey(HTUI hWnd, int nTabsCount, CKeyInput key)
    {
        if (nTabsCount == 0)
            return false;
        
        TMBool bReverse = key.IsModifiersShift();
        if (!bReverse)
        {
            if (m_nCurrentTab == nTabsCount - 1)
            {
                m_nCurrentTab = 0;
                hWnd->Invalidate();
                return false;
            }
            ++m_nCurrentTab;
        }
        else
        {
            if (m_nCurrentTab == 0)
            {
                m_nCurrentTab = nTabsCount - 1;
                hWnd->Invalidate();
                return false;
            }
            --m_nCurrentTab;
        }
        hWnd->Invalidate();
        return true;
    }
    
};


// T_Hotkey: CPropHotKey or CPropHotKeyID
template <class T_This, class T_Hotkey, int n>
class TM_NO_VTABLE CHotKeyWinT
{
protected:
    typedef CHotKeyWinT CHotKeyWin;
    enum { eCountHotKey = n };
    
    T_Hotkey     m_Hotkey[eCountHotKey];
    
private:
    T_This* This() { return static_cast<T_This*>(this); }

    void ClearHotKey(int nStart = 0, int nEnd = eCountHotKey)
    {
        for (int i = nStart; i < nEnd; ++i)
            g_input.RemoveHotkey(m_Hotkey[i], This());
    }
    
    void ResetHotKeys(int nStart, int nEnd, const TUIHOTKEY* whk)
    {
        DbgAssert(nStart >= 0);
        DbgAssert(nEnd <= eCountHotKey);

        // clear
        ClearHotKey(nStart, nEnd);

        // reset hot key value
        if (whk)
        {
            tm_for (int i = nStart; i < nEnd; ++i)
                m_Hotkey[i].Set(whk[i - nStart]);

        }

        // add hot key
        if (!Edit_IsObjectInDocMode(This()) && This()->IsAllEnable(true))
        {
            tm_for (int i = nStart; i < nEnd; ++i)
                g_input.AddHotkey(m_Hotkey[i], This(), m_Hotkey[i].GetID());
        }
    }
    
    void GetHotkey(const T_Hotkey& hk, TUIHOTKEY& whk)
    {
        whk.modifiers    = hk.m_modifiers;
        whk.vkey         = hk.m_vk;
        whk.id           = hk.GetID();
    }

    inline void InitHotKey()
    {
        ResetHotKeys(0, eCountHotKey, 0);
    }
    
protected:
    // CTuiWgt
    TMBool ProcessWindowMessage(TM::HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0)
    {
        if (uMsg == WM_NCDESTROY)
            ClearHotKey();
        else if (uMsg == WM_ENABLE || uMsg == CM_ENABLE_P || uMsg == WM_NCCREATE || uMsg == CM_SETPARENT_P)
            InitHotKey();
        else if (uMsg == CM_SETHOTKEY)
        {
            DbgAssert(lParam);
            const TUIHOTKEY* p = (const TUIHOTKEY*)lParam;
            if (eCountHotKey == 1)
            {
                ResetHotKeys(0, 1, p);
                lResult = true;
            }
            else if (wParam == (TMWParam)-1)
            {
                ResetHotKeys(0, eCountHotKey, p);
                lResult = true;
            }
            else if (wParam >= 0 && wParam < eCountHotKey)
            {
                ResetHotKeys(wParam, wParam + 1, p);
                lResult = true;
            }
            return true;
        }
        else if (uMsg == CM_GETHOTKEY)
        {
            DbgAssert(lParam);
            TUIHOTKEY* p = (TUIHOTKEY*)lParam;
            if (eCountHotKey == 1)
            {
                GetHotkey(m_Hotkey[0], p[0]);
                lResult = true;
            }
            else if (wParam == (TMWParam)-1)
            {
                for (int i = 0; i < eCountHotKey; ++i)
                    GetHotkey(m_Hotkey[i], p[i]);
                lResult = true;
            }
            else if (wParam >= 0 && wParam < eCountHotKey)
            {
                GetHotkey(m_Hotkey[wParam], p[0]);
                lResult = true;
            }
            return true;
        }
        
        return false;
    }
};


class CKeyboardDebugInfo
{
private:
    typedef CStaticMapPair<int, TMCStr> KeyMap[149];
    static KeyMap& GetPair()
    {
#define _HK(x)  x, TM_T(#x)
        static CStaticMapPair<int, TMCStr> s_map_CPropBoxHotKey[] =
        {
            _HK(VK_BACK), 
            _HK(VK_TAB), 
            
            _HK(VK_CLEAR), 
            _HK(VK_RETURN), 
            
            _HK(VK_SHIFT), 
            _HK(VK_CONTROL), 
            _HK(VK_MENU), 
            _HK(VK_PAUSE), 
            _HK(VK_CAPITAL), 
            
            _HK(VK_KANA), 
            _HK(VK_JUNJA), 
            _HK(VK_FINAL), 
            _HK(VK_HANJA), 
            
            _HK(VK_ESCAPE), 
            
            _HK(VK_CONVERT), 
            _HK(VK_NONCONVERT), 
            _HK(VK_ACCEPT), 
            _HK(VK_MODECHANGE), 
            
            _HK(VK_SPACE), 
            _HK(VK_PRIOR), 
            _HK(VK_NEXT), 
            _HK(VK_END), 
            _HK(VK_HOME), 
            _HK(VK_LEFT), 
            _HK(VK_UP), 
            _HK(VK_RIGHT), 
            _HK(VK_DOWN), 
            _HK(VK_SELECT), 
            _HK(VK_PRINT), 
            _HK(VK_EXECUTE), 
            _HK(VK_SNAPSHOT), 
            _HK(VK_INSERT), 
            _HK(VK_DELETE), 
            _HK(VK_HELP), 
            
            // _HK(VK_LWIN           0x5B
            // _HK(VK_RWIN           0x5C
            //     _HK(VK_APPS), 
            
            //     _HK(VK_NUMPAD0), 
            //     _HK(VK_NUMPAD1), 
            //     _HK(VK_NUMPAD2), 
            //     _HK(VK_NUMPAD3), 
            //     _HK(VK_NUMPAD4), 
            //     _HK(VK_NUMPAD5), 
            //     _HK(VK_NUMPAD6), 
            //     _HK(VK_NUMPAD7), 
            //     _HK(VK_NUMPAD8), 
            //     _HK(VK_NUMPAD9), 
            _HK(VK_MULTIPLY), 
            _HK(VK_ADD), 
            _HK(VK_SEPARATOR), 
            _HK(VK_SUBTRACT), 
            _HK(VK_DECIMAL), 
            _HK(VK_DIVIDE), 
            _HK(VK_F1), 
            _HK(VK_F2), 
            _HK(VK_F3), 
            _HK(VK_F4), 
            _HK(VK_F5), 
            _HK(VK_F6), 
            _HK(VK_F7), 
            _HK(VK_F8), 
            _HK(VK_F9), 
            _HK(VK_F10), 
            _HK(VK_F11), 
            _HK(VK_F12), 
            _HK(VK_F13), 
            _HK(VK_F14), 
            _HK(VK_F15), 
            _HK(VK_F16), 
            _HK(VK_F17), 
            _HK(VK_F18), 
            _HK(VK_F19), 
            _HK(VK_F20), 
            _HK(VK_F21), 
            _HK(VK_F22), 
            _HK(VK_F23), 
            _HK(VK_F24), 
            
            _HK(VK_NUMLOCK), 
            _HK(VK_SCROLL), 

            _HK(VK_BROWSER_BACK), 
            _HK(VK_BROWSER_FORWARD), 
            _HK(VK_BROWSER_REFRESH), 
            _HK(VK_BROWSER_STOP), //        0xA9
            _HK(VK_BROWSER_SEARCH), //      0xAA
            _HK(VK_BROWSER_FAVORITES), //   0xAB
            _HK(VK_BROWSER_HOME), //        0xAC
            _HK(VK_VOLUME_MUTE), //         0xAD
            _HK(VK_VOLUME_DOWN), //         0xAE
            _HK(VK_VOLUME_UP), //           0xAF
            _HK(VK_MEDIA_NEXT_TRACK), //    0xB0
            _HK(VK_MEDIA_PREV_TRACK), //    0xB1
            _HK(VK_MEDIA_STOP), //          0xB2
            _HK(VK_MEDIA_PLAY_PAUSE), //    0xB3
            _HK(VK_LAUNCH_MAIL), //         0xB4
            _HK(VK_LAUNCH_MEDIA_SELECT), // 0xB5
            _HK(VK_LAUNCH_APP1), //         0xB6
            _HK(VK_LAUNCH_APP2), //         0xB7
            _HK(VK_OEM_1), //          0xBA   // ';:' for US
            _HK(VK_OEM_PLUS), //       0xBB   // '+' any country
            _HK(VK_OEM_COMMA), //      0xBC   // ',' any country
            _HK(VK_OEM_MINUS), //      0xBD   // '-' any country
            _HK(VK_OEM_PERIOD), //     0xBE   // '.' any country
            _HK(VK_OEM_2), //          0xBF   // '/?' for US
            _HK(VK_OEM_3), //          0xC0   // '`~' for US
            _HK(VK_GAMEPAD_A), //                         0xC3 // reserved
            _HK(VK_GAMEPAD_B), //                         0xC4 // reserved
            _HK(VK_GAMEPAD_X), //                         0xC5 // reserved
            _HK(VK_GAMEPAD_Y), //                         0xC6 // reserved
            _HK(VK_GAMEPAD_RIGHT_SHOULDER), //            0xC7 // reserved
            _HK(VK_GAMEPAD_LEFT_SHOULDER), //             0xC8 // reserved
            _HK(VK_GAMEPAD_LEFT_TRIGGER), //              0xC9 // reserved
            _HK(VK_GAMEPAD_RIGHT_TRIGGER), //             0xCA // reserved
            _HK(VK_GAMEPAD_DPAD_UP), //                   0xCB // reserved
            _HK(VK_GAMEPAD_DPAD_DOWN), //                 0xCC // reserved
            _HK(VK_GAMEPAD_DPAD_LEFT), //                 0xCD // reserved
            _HK(VK_GAMEPAD_DPAD_RIGHT), //                0xCE // reserved
            _HK(VK_GAMEPAD_MENU), //                      0xCF // reserved
            _HK(VK_GAMEPAD_VIEW), //                      0xD0 // reserved
            _HK(VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON), //    0xD1 // reserved
            _HK(VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON), //   0xD2 // reserved
            _HK(VK_GAMEPAD_LEFT_THUMBSTICK_UP), //        0xD3 // reserved
            _HK(VK_GAMEPAD_LEFT_THUMBSTICK_DOWN), //      0xD4 // reserved
            _HK(VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT), //     0xD5 // reserved
            _HK(VK_GAMEPAD_LEFT_THUMBSTICK_LEFT), //      0xD6 // reserved
            _HK(VK_GAMEPAD_RIGHT_THUMBSTICK_UP), //       0xD7 // reserved
            _HK(VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN), //     0xD8 // reserved
            _HK(VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT), //    0xD9 // reserved
            _HK(VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT), //     0xDA // reserved
            _HK(VK_OEM_4), //          0xDB  //  '[{' for US
            _HK(VK_OEM_5), //          0xDC  //  '\|' for US
            _HK(VK_OEM_6), //          0xDD  //  ']}' for US
            _HK(VK_OEM_7), //          0xDE  //  ''"' for US
            _HK(VK_OEM_8), //          0xDF
            _HK(VK_OEM_AX), //         0xE1  //  'AX' key on Japanese AX kbd
            _HK(VK_OEM_102), //        0xE2  //  "<>" or "\|" on RT 102-key kbd.
            _HK(VK_ICO_HELP), //       0xE3  //  Help key on ICO
            _HK(VK_ICO_00), //         0xE4  //  00 key on ICO
            _HK(VK_PROCESSKEY), //     0xE5
            _HK(VK_ICO_CLEAR), //      0xE6
            _HK(VK_PACKET), //         0xE7
            _HK(VK_OEM_RESET), //      0xE9
            _HK(VK_OEM_JUMP), //       0xEA
            _HK(VK_OEM_PA1), //        0xEB
            _HK(VK_OEM_PA2), //        0xEC
            _HK(VK_OEM_PA3), //        0xED
            _HK(VK_OEM_WSCTRL), //     0xEE
            _HK(VK_OEM_CUSEL), //      0xEF
            _HK(VK_OEM_ATTN), //       0xF0
            _HK(VK_OEM_FINISH), //     0xF1
            _HK(VK_OEM_COPY), //       0xF2
            _HK(VK_OEM_AUTO), //       0xF3
            _HK(VK_OEM_ENLW), //       0xF4
            _HK(VK_OEM_BACKTAB), //    0xF5
            _HK(VK_ATTN), //           0xF6
            _HK(VK_CRSEL), //          0xF7
            _HK(VK_EXSEL), //          0xF8
            _HK(VK_EREOF), //          0xF9
            _HK(VK_PLAY), //           0xFA
            _HK(VK_ZOOM), //           0xFB
            _HK(VK_NONAME), //         0xFC
            _HK(VK_PA1), //            0xFD
            _HK(VK_OEM_CLEAR), //      0xFE

        };
    #undef _HK
        return s_map_CPropBoxHotKey;
    }

public:
    static void VK2String(TMUInt16 vk, TM::CString& str)
    {
        StaticMapFind(GetPair(), vk, str);
    }

    static void String2VK(TMCStr str, TMUInt16& vk)
    {
        StaticMapFindByStrI(GetPair(), str, vk);
    }

    static TM::CString Mod2String(TMUInt16 mod)
    {
        CString str;
#define _FMT(x) if (mod & x) str += L###x L" | "
        _FMT(MOD_APP);
        _FMT(MOD_SYS);
        _FMT(MOD_DISABLE);
        _FMT(MOD_THROUGH);
        _FMT(MOD_ALT);
        _FMT(MOD_CONTROL);
        _FMT(MOD_SHIFT);
        _FMT(MOD_WIN);
        _FMT(MOD_LEFT);
        _FMT(MOD_RIGHT);
        _FMT(MOD_ON_KEYUP);
        _FMT(MOD_IGNORE_ALL_MODIFIER);
#undef _FMT
        return str;
    }

    static TM::CString HotKeys2String(CHotKeyMap& aKey, TMCStr pPre)
    {
        typedef CHotKeyMap::CHotKeyList     CHotKeyList;
        typedef CHotKeyMap::CHotKeyStore    CHotKeyStore;

        CString str;
        for (CHotKeyList::recorder rec(aKey.m_aHotKey); rec; ++rec)
        {
            const CHotKeyStore& hks = rec.get_data();
            CString strTmp;
            CString strVk;
            VK2String(hks.m_vk, strVk);
            strTmp.Format(TM_T("%d(%s) - %d - "), hks.m_hOwner->m_dwObjID, hks.m_hOwner->GetClassName().c_str(), hks.m_id);
            str += pPre;
            str += strTmp;
            str += strVk;
            str += L" - ";
            str += Mod2String(hks.m_modifiers);
        }
        return str;
    }
};


TM_END_NAMESPACE




#endif // INPUT_H
