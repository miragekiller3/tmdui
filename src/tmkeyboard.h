/*
 *  @file
 *  @brief
 *    CKeyboardNative:      Keyboard for native
 *    CKeyboard:            Keyboard for framework
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.8.13
 *
 *  Copyright (C) 2020 miragekiller
 */

#pragma once

#ifndef TMKEYBOARD_H
#define TMKEYBOARD_H

#include "tmgdi.h"
#include "tmnative.h"

#ifdef TM_X11
#   include <X11/Xlib.h>
#   include <X11/keysym.h>
#endif

#ifdef TM_MAC
#endif 

#ifdef TM_QT
#   include <QApplication>
#endif

#ifdef TM_GTK
#   include <gdk/gdk.h>

#   if TM_GTK < 400
#       define TM_GTK_ALT_MASK GDK_MOD1_MASK
#   else
#       define TM_GTK_ALT_MASK GDK_ALT_MASK
#   endif
#endif



#ifndef VK_LBUTTON
/*
 * Virtual Keys, Standard Set
 */
#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */

#define VK_BACK           0x08
#define VK_TAB            0x09

#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D

#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14

#define VK_KANA           0x15
#define VK_HANGEUL        0x15  /* old name - should be here for compatibility */
#define VK_HANGUL         0x15
#define VK_JUNJA          0x17
#define VK_FINAL          0x18
#define VK_HANJA          0x19
#define VK_KANJI          0x19

#define VK_ESCAPE         0x1B

#define VK_CONVERT        0x1C
#define VK_NONCONVERT     0x1D
#define VK_ACCEPT         0x1E
#define VK_MODECHANGE     0x1F

#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F

/* VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
/* VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */

#define VK_LWIN           0x5B
#define VK_RWIN           0x5C
#define VK_APPS           0x5D

#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B
#define VK_F13            0x7C
#define VK_F14            0x7D
#define VK_F15            0x7E
#define VK_F16            0x7F
#define VK_F17            0x80
#define VK_F18            0x81
#define VK_F19            0x82
#define VK_F20            0x83
#define VK_F21            0x84
#define VK_F22            0x85
#define VK_F23            0x86
#define VK_F24            0x87

#define VK_NUMLOCK        0x90
#define VK_SCROLL         0x91

/*
 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5

#endif // VK_LBUTTON

/*
 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5

#ifndef VK_BROWSER_BACK
#   define VK_BROWSER_BACK                      0xA6
#   define VK_BROWSER_FORWARD                   0xA7
#   define VK_BROWSER_REFRESH                   0xA8
#   define VK_BROWSER_STOP                      0xA9
#   define VK_BROWSER_SEARCH                    0xAA
#   define VK_BROWSER_FAVORITES                 0xAB
#   define VK_BROWSER_HOME                      0xAC
#   define VK_VOLUME_MUTE                       0xAD
#   define VK_VOLUME_DOWN                       0xAE
#   define VK_VOLUME_UP                         0xAF
#   define VK_MEDIA_NEXT_TRACK                  0xB0
#   define VK_MEDIA_PREV_TRACK                  0xB1
#   define VK_MEDIA_STOP                        0xB2
#   define VK_MEDIA_PLAY_PAUSE                  0xB3
#   define VK_LAUNCH_MAIL                       0xB4
#   define VK_LAUNCH_MEDIA_SELECT               0xB5
#   define VK_LAUNCH_APP1                       0xB6
#   define VK_LAUNCH_APP2                       0xB7
#endif // VK_BROWSER_BACK

#ifndef VK_OEM_1
#   define VK_OEM_1                             0xBA   // ';:' for US
#   define VK_OEM_PLUS                          0xBB   // '+' any country
#   define VK_OEM_COMMA                         0xBC   // ',' any country
#   define VK_OEM_MINUS                         0xBD   // '-' any country
#   define VK_OEM_PERIOD                        0xBE   // '.' any country
#   define VK_OEM_2                             0xBF   // '/?' for US
#   define VK_OEM_3                             0xC0   // '`~' for US
#endif // VK_OEM_1

// 0xC3 - 0xDA : Gamepad input
#ifndef VK_GAMEPAD_A
#   define VK_GAMEPAD_A                         0xC3 // reserved
#   define VK_GAMEPAD_B                         0xC4 // reserved
#   define VK_GAMEPAD_X                         0xC5 // reserved
#   define VK_GAMEPAD_Y                         0xC6 // reserved
#   define VK_GAMEPAD_RIGHT_SHOULDER            0xC7 // reserved
#   define VK_GAMEPAD_LEFT_SHOULDER             0xC8 // reserved
#   define VK_GAMEPAD_LEFT_TRIGGER              0xC9 // reserved
#   define VK_GAMEPAD_RIGHT_TRIGGER             0xCA // reserved
#   define VK_GAMEPAD_DPAD_UP                   0xCB // reserved
#   define VK_GAMEPAD_DPAD_DOWN                 0xCC // reserved
#   define VK_GAMEPAD_DPAD_LEFT                 0xCD // reserved
#   define VK_GAMEPAD_DPAD_RIGHT                0xCE // reserved
#   define VK_GAMEPAD_MENU                      0xCF // reserved
#   define VK_GAMEPAD_VIEW                      0xD0 // reserved
#   define VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON    0xD1 // reserved
#   define VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON   0xD2 // reserved
#   define VK_GAMEPAD_LEFT_THUMBSTICK_UP        0xD3 // reserved
#   define VK_GAMEPAD_LEFT_THUMBSTICK_DOWN      0xD4 // reserved
#   define VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT     0xD5 // reserved
#   define VK_GAMEPAD_LEFT_THUMBSTICK_LEFT      0xD6 // reserved
#   define VK_GAMEPAD_RIGHT_THUMBSTICK_UP       0xD7 // reserved
#   define VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN     0xD8 // reserved
#   define VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT    0xD9 // reserved
#   define VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT     0xDA // reserved
#endif // VK_GAMEPAD_A

#ifndef VK_OEM_4
#   define VK_OEM_4                             0xDB  //  '[{' for US
#   define VK_OEM_5                             0xDC  //  '\|' for US
#   define VK_OEM_6                             0xDD  //  ']}' for US
#   define VK_OEM_7                             0xDE  //  ''"' for US
#   define VK_OEM_8                             0xDF
#endif // VK_OEM_4

// Various extended or enhanced keyboards
#ifndef VK_OEM_AX
#   define VK_OEM_AX                            0xE1  //  'AX' key on Japanese AX kbd
#   define VK_OEM_102                           0xE2  //  "<>" or "\|" on RT 102-key kbd.
#   define VK_ICO_HELP                          0xE3  //  Help key on ICO
#   define VK_ICO_00                            0xE4  //  00 key on ICO
#endif // VK_OEM_AX

#ifndef VK_PROCESSKEY
#   define VK_PROCESSKEY                        0xE5
#endif // VK_PROCESSKEY

#ifndef VK_ICO_CLEAR
#   define VK_ICO_CLEAR                         0xE6
#endif // VK_ICO_CLEAR

#ifndef VK_PACKET
#   define VK_PACKET                            0xE7
#endif //VK_PACKET

// Nokia/Ericsson definitions
#ifndef VK_OEM_RESET
#   define VK_OEM_RESET                         0xE9
#   define VK_OEM_JUMP                          0xEA
#   define VK_OEM_PA1                           0xEB
#   define VK_OEM_PA2                           0xEC
#   define VK_OEM_PA3                           0xED
#   define VK_OEM_WSCTRL                        0xEE
#   define VK_OEM_CUSEL                         0xEF
#   define VK_OEM_ATTN                          0xF0
#   define VK_OEM_FINISH                        0xF1
#   define VK_OEM_COPY                          0xF2
#   define VK_OEM_AUTO                          0xF3
#   define VK_OEM_ENLW                          0xF4
#   define VK_OEM_BACKTAB                       0xF5
#   define VK_ATTN                              0xF6
#   define VK_CRSEL                             0xF7
#   define VK_EXSEL                             0xF8
#   define VK_EREOF                             0xF9
#   define VK_PLAY                              0xFA
#   define VK_ZOOM                              0xFB
#   define VK_NONAME                            0xFC
#   define VK_PA1                               0xFD
#   define VK_OEM_CLEAR                         0xFE
#endif // VK_OEM_RESET

#ifndef MOD_ALT
#   define MOD_ALT                              0x0001
#   define MOD_CONTROL                          0x0002
#   define MOD_SHIFT                            0x0004
#   define MOD_WIN                              0x0008
#   define MOD_LEFT                             0x8000
#   define MOD_RIGHT                            0x4000
#   define MOD_ON_KEYUP                         0x0800
#   define MOD_IGNORE_ALL_MODIFIER              0x0400
#endif
#define MOD_NATIVEKEYMASK  (MOD_ALT | MOD_SHIFT | MOD_CONTROL | MOD_WIN)


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// keyboard for windows
#ifdef TM_WIN

struct CKeyInputWin
{
    int nCode;
    WPARAM lParam;

private:
    static bool IsKeyDown(int nVirtKey)
    {
#ifdef TM_WIN
        return 0 != (GetKeyState(nVirtKey) & 0x8000);
#elif defined(TM_X11)
        Display* dpy = XOpenDisplay(NULL);
        char keys_return[32];
        XQueryKeymap(dpy, keys_return);
        KeyCode kc2 = XKeysymToKeycode(dpy, nVirtKey); //XK_Shift_L);
        bool Pressed = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
        XCloseDisplay(dpy);
        return Pressed;
#elif defined(TM_MAC)
        //     CGKeyCode cgcode = wxCharCodeWXToOSX((wxKeyCode)key);
        return CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, nVirtKey);
#else
#       error not support
#endif 
    }

public:
    CKeyInputWin() : nCode(0), lParam(0) {}
    CKeyInputWin(WPARAM wp, LPARAM lp) : nCode(wp), lParam(lp) {}

    void Init(WPARAM wp, LPARAM lp)   { nCode = wp; lParam = lp; }
    void Reset()                      { Init(0, 0); }

    bool IsKey(int vk)          const { return nCode == vk; }
    int  IsKeyTab()             const { return VK_TAB != nCode ? 0 : (IsModifiersShift() ? -1 : 1); } // 0:not tab; 1:tab; -1:shift+tab;

    bool IsModifiersAlt()       const { return IsKeyDown(VK_MENU); }
    bool IsModifiersShift()     const { return IsKeyDown(VK_SHIFT); }
    bool IsModifiersControl()   const { return IsKeyDown(VK_CONTROL); }
    
    bool IsModifiersLWin()      const { return IsKeyDown(VK_LWIN); }
    bool IsModifiersRWin()      const { return IsKeyDown(VK_RWIN); }
    bool IsModifiersWin()       const { return IsKeyDown(VK_LWIN) || IsKeyDown(VK_RWIN); }
    
    TMUInt16 GetModifiers()     const
    {
        TMUInt16 mdf = 0;
        if (IsKeyDown(VK_LSHIFT))    mdf |= (MOD_SHIFT | MOD_LEFT);
        if (IsKeyDown(VK_RSHIFT))    mdf |= (MOD_SHIFT | MOD_RIGHT);
        if (IsKeyDown(VK_LCONTROL))  mdf |= (MOD_CONTROL | MOD_LEFT);
        if (IsKeyDown(VK_RCONTROL))  mdf |= (MOD_CONTROL | MOD_RIGHT);
        if (IsKeyDown(VK_LMENU))     mdf |= (MOD_ALT | MOD_LEFT);
        if (IsKeyDown(VK_RMENU))     mdf |= (MOD_ALT | MOD_RIGHT);
        if (IsKeyDown(VK_LWIN))      mdf |= (MOD_WIN | MOD_LEFT);
        if (IsKeyDown(VK_RWIN))      mdf |= (MOD_WIN | MOD_RIGHT);
        return mdf;
    }
    
    TMUInt16 GetModifiersFast() const
    {
        TMUInt16 mdf = 0;
        if (IsKeyDown(VK_SHIFT))    mdf |= MOD_SHIFT;
        if (IsKeyDown(VK_CONTROL))  mdf |= MOD_CONTROL;
        if (IsKeyDown(VK_MENU))     mdf |= MOD_ALT;
        if (IsModifiersWin())       mdf |= MOD_WIN;
        return mdf;
    }
};

class CKeyboardWin
{
public:
    static bool IsKeyDown(int nVirtKey)
    {
#ifdef TM_WIN
        return 0 != (GetKeyState(nVirtKey) & 0x8000);
#elif defined(TM_X11)
        Display* dpy = XOpenDisplay(NULL);
        char keys_return[32];
        XQueryKeymap(dpy, keys_return);
        KeyCode kc2 = XKeysymToKeycode(dpy, nVirtKey); //XK_Shift_L);
        bool Pressed = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
        XCloseDisplay(dpy);
        return Pressed;
#elif defined(TM_MAC)
        //     CGKeyCode cgcode = wxCharCodeWXToOSX((wxKeyCode)key);
        return CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, nVirtKey);
#else
#       error not support
#endif 
    }

public:
    static bool IsModifiersAlt() { return IsKeyDown(VK_MENU); }
    static bool IsModifiersShift() { return IsKeyDown(VK_SHIFT); }
    static bool IsModifiersControl() { return IsKeyDown(VK_CONTROL); }
    
    static bool IsModifiersLWin() { return IsKeyDown(VK_LWIN); }
    static bool IsModifiersRWin() { return IsKeyDown(VK_RWIN); }
    static bool IsModifiersWin() { return IsKeyDown(VK_LWIN) || IsKeyDown(VK_RWIN); }

    static TMUInt16 GetModifiers()
    {
        TMUInt16 mdf = 0;
        if (IsKeyDown(VK_LSHIFT))    mdf |= (MOD_SHIFT | MOD_LEFT);
        if (IsKeyDown(VK_RSHIFT))    mdf |= (MOD_SHIFT | MOD_RIGHT);
        if (IsKeyDown(VK_LCONTROL))  mdf |= (MOD_CONTROL | MOD_LEFT);
        if (IsKeyDown(VK_RCONTROL))  mdf |= (MOD_CONTROL | MOD_RIGHT);
        if (IsKeyDown(VK_LMENU))     mdf |= (MOD_ALT | MOD_LEFT);
        if (IsKeyDown(VK_RMENU))     mdf |= (MOD_ALT | MOD_RIGHT);
        if (IsKeyDown(VK_LWIN))      mdf |= (MOD_WIN | MOD_LEFT);
        if (IsKeyDown(VK_RWIN))      mdf |= (MOD_WIN | MOD_RIGHT);
        return mdf;
    }
    
    static TMUInt16 GetSimpleModifiers()
    {
        TMUInt16 mdf = 0;
        if (IsKeyDown(VK_SHIFT))    mdf |= MOD_SHIFT;
        if (IsKeyDown(VK_CONTROL))  mdf |= MOD_CONTROL;
        if (IsKeyDown(VK_MENU))     mdf |= MOD_ALT;
        if (IsModifiersWin())       mdf |= MOD_WIN;
        return mdf;
    }
};

#endif // TM_WIN


///////////////////////////////////////////////////////////////////////////////
// keyboard for QT
#ifdef TM_QT

class CKeyboardQt
{
public:

    static bool IsKeyDownAlt()
    {
        return 0 != (QApplication::keyboardModifiers() & Qt::AltModifier);
    }

    static bool IsKeyDownShift()
    {
        return 0 != (QApplication::keyboardModifiers() & Qt::ShiftModifier);
    }

    static bool IsKeyDownControl()
    {
        return 0 != (QApplication::keyboardModifiers() & Qt::ControlModifier);
    }

    static TMUInt16 GetModifiers()
    {
        TMUInt16 mdf = 0;
        Qt::KeyboardModifiers qmdf = QApplication::keyboardModifiers();
        if (qmdf & Qt::AltModifier)     mdf |= (MOD_ALT | MOD_LEFT | MOD_RIGHT);
        if (qmdf & Qt::ShiftModifier)   mdf |= (MOD_CONTROL | MOD_LEFT | MOD_RIGHT);
        if (qmdf & Qt::ControlModifier) mdf |= (MOD_SHIFT | MOD_LEFT | MOD_RIGHT);
        if (qmdf & Qt::MetaModifier)    mdf |= (MOD_WIN | MOD_LEFT | MOD_RIGHT);
        return mdf;
    }
    
    static TMUInt16 GetSimpleModifiers()
    {
        TMUInt16 mdf = 0;
        Qt::KeyboardModifiers qmdf = QApplication::keyboardModifiers();
        if (qmdf & Qt::AltModifier)     mdf |= MOD_ALT;
        if (qmdf & Qt::ShiftModifier)   mdf |= MOD_CONTROL;
        if (qmdf & Qt::ControlModifier) mdf |= MOD_SHIFT;
        if (qmdf & Qt::MetaModifier)    mdf |= MOD_WIN;
        return mdf;
    }
};


///////////////////////////////////////////////////////////////////////////////
// key input event for QT (compatible interface with CKeyInputWin)
struct CKeyInputQt
{
    int                     nCode;    // Qt::Key value
    Qt::KeyboardModifiers   nState; // Qt modifier flags

    CKeyInputQt() : nCode(0), nState(Qt::NoModifier) {}
    CKeyInputQt(int key, Qt::KeyboardModifiers mods) : nCode(key), nState(mods) {}

    void Init(int key, Qt::KeyboardModifiers mods) { nCode = key; nState = mods; }
    void Reset() { Init(0, Qt::NoModifier); }

    bool IsKey(int vk)  const { return nCode == vk; }
    int  IsKeyTab()     const
    {
        if (nCode != Qt::Key_Tab && nCode != Qt::Key_Backtab)
            return 0;
        return (nState & Qt::ShiftModifier) ? -1 : 1;
    }

    bool IsModifiersAlt()     const { return 0 != (nState & Qt::AltModifier); }
    bool IsModifiersShift()   const { return 0 != (nState & Qt::ShiftModifier); }
    bool IsModifiersControl() const { return 0 != (nState & Qt::ControlModifier); }

    bool IsModifiersLWin()    const { return 0 != (nState & Qt::MetaModifier); }
    bool IsModifiersRWin()    const { return false; }
    bool IsModifiersWin()     const { return 0 != (nState & Qt::MetaModifier); }

    TMUInt16 GetModifiers() const
    {
        TMUInt16 mdf = 0;
        if (nState & Qt::ShiftModifier)   mdf |= (MOD_SHIFT   | MOD_LEFT | MOD_RIGHT);
        if (nState & Qt::ControlModifier) mdf |= (MOD_CONTROL | MOD_LEFT | MOD_RIGHT);
        if (nState & Qt::AltModifier)     mdf |= (MOD_ALT     | MOD_LEFT | MOD_RIGHT);
        if (nState & Qt::MetaModifier)    mdf |= (MOD_WIN     | MOD_LEFT | MOD_RIGHT);
        return mdf;
    }

    TMUInt16 GetModifiersFast() const { return GetModifiers(); }
};

#endif // TM_QT


///////////////////////////////////////////////////////////////////////////////
// keyboard for GTK
#ifdef TM_GTK

struct CKeyInputGtk
{
    guint           nCode;   // GDK keyval
    GdkModifierType nState;  // GDK modifier state

    CKeyInputGtk() : nCode(0), nState((GdkModifierType)0) {}
    CKeyInputGtk(guint keyval, GdkModifierType state) : nCode(keyval), nState(state) {}

    void Init(guint keyval, GdkModifierType state) { nCode = keyval; nState = state; }
    void Reset() { Init(0, (GdkModifierType)0); }

    bool IsKey(int vk)  const { return nCode == vk; }
    int  IsKeyTab()     const
    {
        if (nCode != GDK_KEY_Tab && nCode != GDK_KEY_ISO_Left_Tab)
            return 0;
        return (nState & GDK_SHIFT_MASK) ? -1 : 1;
    }

    bool IsModifiersAlt()     const { return 0 != (nState & TM_GTK_ALT_MASK); }
    bool IsModifiersShift()   const { return 0 != (nState & GDK_SHIFT_MASK); }
    bool IsModifiersControl() const { return 0 != (nState & GDK_CONTROL_MASK); }

    bool IsModifiersLWin()    const { return 0 != (nState & GDK_SUPER_MASK); }
    bool IsModifiersRWin()    const { return 0 != (nState & GDK_HYPER_MASK); }
    bool IsModifiersWin()     const { return 0 != (nState & (GDK_SUPER_MASK | GDK_HYPER_MASK)); }

    TMUInt16 GetModifiers() const
    {
        TMUInt16 mdf = 0;
        if (nState & GDK_SHIFT_MASK)                     mdf |= (MOD_SHIFT   | MOD_LEFT | MOD_RIGHT);
        if (nState & GDK_CONTROL_MASK)                   mdf |= (MOD_CONTROL | MOD_LEFT | MOD_RIGHT);
        if (nState & TM_GTK_ALT_MASK)                    mdf |= (MOD_ALT     | MOD_LEFT | MOD_RIGHT);
        if (nState & (GDK_SUPER_MASK | GDK_HYPER_MASK))  mdf |= (MOD_WIN     | MOD_LEFT | MOD_RIGHT);
        return mdf;
    }

    TMUInt16 GetModifiersFast() const { return GetModifiers(); }
};

#endif // GTK


TM_END_NAMESPACE


#ifdef TM_WIN
#   define CKeyboardNative  CKeyboardWin
#   define CKeyInput        CKeyInputWin   
#elif defined(TM_QT)
#   define CKeyboardNative  CKeyboardQt
#   define CKeyInput        CKeyInputQt   
#elif defined(TM_GTK)
#   define CKeyInput        CKeyInputGtk
#else
#   error "not support"
#endif

#define CKeyboard           CKeyboardNative

#endif // ATINPUT_H
