/*
 *  @file
 *  @brief  platform sdk support
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.09.19
 *
 *  Copyright (C) 2010 miragekiller
 */

#pragma once

#ifndef TMPLATFORM_WIN_H
#define TMPLATFORM_WIN_H

#include "tmdll.h"

#ifndef TM_WIN
#   error "the file only support windows"
#endif

#include <windows.h>

#if tm_has_include(<uxtheme.h>) || TM_MSC >= 1300
#   include <uxtheme.h>
#   pragma comment(lib, "uxtheme.lib")
#endif

#if tm_has_include(<Dwmapi.h>) || defined(_INC_WINAPIFAMILY)
#   include <Dwmapi.h>
// #   pragma comment(lib, "Dwmapi.lib")
#endif

// shellscalingapi.h not support win7
// so need loadlibray and call fucntion
// #if (defined(__has_include) && __has_include(<shellscalingapi.h>)) || defined(_INC_WINAPIFAMILY)
// #   include <shellscalingapi.h>
// #   pragma comment(lib, "Shcore.lib")
// #endif


#ifndef HWND_MESSAGE
#   define HWND_MESSAGE     ((HWND)-3)
#endif

#if (_WIN32_WINNT < 0x0501)

    typedef LRESULT (CALLBACK *SUBCLASSPROC)(HWND hWnd, UINT uMsg, WPARAM wParam,
        LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

    DECLARE_DLL_FUNC3(dll_comctl32, BOOL, WINAPI, SetWindowSubclass, (HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData));
    inline BOOL SetWindowSubclass(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
    {
        return TM_IMPORT(SetWindowSubclass)(hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    DECLARE_DLL_FUNC3(dll_comctl32, BOOL, WINAPI, RemoveWindowSubclass, (HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass));
    inline BOOL RemoveWindowSubclass(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass)
    {
        return TM_IMPORT(RemoveWindowSubclass)(hWnd, pfnSubclass, uIdSubclass);
    }

    DECLARE_DLL_FUNC3(dll_comctl32, LRESULT, WINAPI, DefSubclassProc, (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam));
    inline LRESULT DefSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return TM_IMPORT(DefSubclassProc)(hWnd, uMsg, wParam, lParam);
    }

//     DECLARE_DLL_FUNC(dll_comctl32, BOOL, WINAPI, SetWindowSubclass, (HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData));
//     DECLARE_DLL_FUNC(dll_comctl32, BOOL, WINAPI, RemoveWindowSubclass, (HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass));
//     DECLARE_DLL_FUNC(dll_comctl32, LRESULT, WINAPI, DefSubclassProc, (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam));
// 
// #   define SetWindowSubclass    _SetWindowSubclass
// #   define RemoveWindowSubclass _RemoveWindowSubclass
// #   define DefSubclassProc      _DefSubclassProc

#endif // #if (_WIN32_WINNT < 0x0501)




#ifndef WM_NCUAHDRAWCAPTION
#   define WM_NCUAHDRAWCAPTION          0x00AE
#   define WM_NCUAHDRAWFRAME            0x00AF
#endif

///////////////////////////////////////////////////////////////////////////////
// raw function
#ifndef RID_INPUT
#   define WM_INPUT                     0x00FF

#   define RID_INPUT                    0x10000003
#   define RID_HEADER                   0x10000005

#   define RIDEV_INPUTSINK              0x00000100
#   define RIDEV_REMOVE                 0x00000001

#   define RIM_TYPEMOUSE                0
#   define RIM_TYPEKEYBOARD             1
#   define RIM_TYPEHID                  2

#   define RI_MOUSE_LEFT_BUTTON_DOWN    0x0001  // Left Button changed to down.
#   define RI_MOUSE_LEFT_BUTTON_UP      0x0002  // Left Button changed to up.
#   define RI_MOUSE_RIGHT_BUTTON_DOWN   0x0004  // Right Button changed to down.
#   define RI_MOUSE_RIGHT_BUTTON_UP     0x0008  // Right Button changed to up.
#   define RI_MOUSE_MIDDLE_BUTTON_DOWN  0x0010  // Middle Button changed to down.
#   define RI_MOUSE_MIDDLE_BUTTON_UP    0x0020  // Middle Button changed to up.

    DECLARE_HANDLE(HRAWINPUT);

    typedef struct tagRAWINPUTHEADER {
        DWORD dwType;
        DWORD dwSize;
        HANDLE hDevice;
        WPARAM wParam;
    } RAWINPUTHEADER, *PRAWINPUTHEADER, *LPRAWINPUTHEADER;

    typedef struct tagRAWMOUSE {
        USHORT usFlags;
        union {
            ULONG ulButtons;
            struct  {
                USHORT  usButtonFlags;
                USHORT  usButtonData;
            };
        };
        ULONG ulRawButtons;
        LONG lLastX;
        LONG lLastY;
        ULONG ulExtraInformation;
    } RAWMOUSE, *PRAWMOUSE, *LPRAWMOUSE;

    typedef struct tagRAWKEYBOARD {
        USHORT MakeCode;
        USHORT Flags;
        USHORT Reserved;
        USHORT VKey;
        UINT   Message;
        ULONG ExtraInformation;
    } RAWKEYBOARD, *PRAWKEYBOARD, *LPRAWKEYBOARD;

    typedef struct tagRAWHID {
        DWORD dwSizeHid;    // byte size of each report
        DWORD dwCount;      // number of input packed
        BYTE bRawData[1];
    } RAWHID, *PRAWHID, *LPRAWHID;

    typedef struct tagRAWINPUT {
        RAWINPUTHEADER header;
        union {
            RAWMOUSE    mouse;
            RAWKEYBOARD keyboard;
            RAWHID      hid;
        } data;
    } RAWINPUT, *PRAWINPUT, *LPRAWINPUT;

    typedef struct tagRAWINPUTDEVICE {
        USHORT usUsagePage; // Toplevel collection UsagePage
        USHORT usUsage;     // Toplevel collection Usage
        DWORD dwFlags;
        HWND hwndTarget;    // Target hwnd. NULL = follows keyboard focus
    } RAWINPUTDEVICE, *PRAWINPUTDEVICE, *LPRAWINPUTDEVICE;
    typedef CONST RAWINPUTDEVICE* PCRAWINPUTDEVICE;


    DECLARE_DLL_FUNC3(dll_user32, UINT, WINAPI, GetRawInputData, (HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader));
    DECLARE_DLL_FUNC3(dll_user32, UINT, WINAPI, RegisterRawInputDevices, (PCRAWINPUTDEVICE pRawInputDevices, UINT uiNumDevices, UINT cbSize));

#   define GetRawInputData          TM_IMPORT(GetRawInputData)
#   define RegisterRawInputDevices  TM_IMPORT(RegisterRawInputDevices)

#endif // WM_INPUT



///////////////////////////////////////////////////////////////////////////////
// new user 32 function
//  layered window, layout, MSAA, ...

#ifndef LWA_ALPHA

#   define LWA_ALPHA        0x2
#   define AC_SRC_ALPHA     0x01

#   define ULW_COLORKEY     0x1
#   define ULW_ALPHA        0x2
#   define ULW_OPAQUE       0x4


    DECLARE_DLL_FUNC3(dll_user32, BOOL, WINAPI, UpdateLayeredWindow,
        (HWND hwnd,            
        HDC hdcDst,            
        POINT *pptDst,         
        SIZE *psize,           
        HDC hdcSrc,            
        POINT *pptSrc,         
        COLORREF crKey,        
        BLENDFUNCTION *pblend, 
        DWORD dwFlags          
       ));
    DECLARE_DLL_FUNC3(dll_user32, BOOL, WINAPI, SetLayeredWindowAttributes,
        (HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags));

#   define UpdateLayeredWindow          TM_IMPORT(UpdateLayeredWindow)
#   define SetLayeredWindowAttributes   TM_IMPORT(SetLayeredWindowAttributes)

#endif // LWA_ALPHA

    typedef struct _tagUPDATELAYEREDWINDOWINFO {
        DWORD               cbSize;
        HDC                 hdcDst;
        const POINT         *pptDst;
        const SIZE          *psize;
        HDC                 hdcSrc;
        const POINT         *pptSrc;
        COLORREF            crKey;
        const BLENDFUNCTION *pblend;
        DWORD               dwFlags;
        const RECT          *prcDirty;
    } _UPDATELAYEREDWINDOWINFO;
    
    DECLARE_DLL_FUNC3(dll_user32, BOOL, WINAPI, UpdateLayeredWindowIndirect,
        (HWND hwnd, const _UPDATELAYEREDWINDOWINFO *pULWInfo));
#   define _UpdateLayeredWindowIndirect TM_IMPORT(UpdateLayeredWindowIndirect)


    DECLARE_DLL_FUNC3(dll_user32, int, WINAPI, InternalGetWindowText,
        (HWND hWnd, LPWSTR pString, int cchMaxCount));
#   define _InternalGetWindowText TM_IMPORT(InternalGetWindowText)

    TM_INLINE LPCWSTR DbgFormatWindow(HWND hWnd)
    {
        WCHAR c[1024] = {0}, n[1024] = {0};
        ::GetClassNameW(hWnd, c, 1024);
        _InternalGetWindowText(hWnd, n, 1024);
        static WCHAR d[1024];
        wsprintfW(d, L"%p,%s,%s", hWnd, c, n);
//        static TM::CString s_str;
//        s_str.Format(L"%p,%s,%s", hWnd, c, n);
        return d;
    }


#ifndef WS_EX_LAYERED
#   define WS_EX_LAYERED 0x00080000
#endif

#ifndef WS_EX_NOACTIVATE
#   define WS_EX_NOACTIVATE 0x08000000
#endif

#ifndef LAYOUT_BITMAPORIENTATIONPRESERVED
#   define LAYOUT_BITMAPORIENTATIONPRESERVED    0x00000008
#   define LAYOUT_RTL                           0x00000001
    DECLARE_DLL_FUNC3(dll_gdi32, DWORD, WINAPI, SetLayout, (HDC, DWORD));
    DECLARE_DLL_FUNC3(dll_gdi32, DWORD, WINAPI, GetLayout, (HDC));
#   define SetLayout TM_IMPORT(SetLayout)
#   define GetLayout TM_IMPORT(GetLayout)

#   define WS_EX_LAYOUTRTL                      0x00400000L
#   define WS_EX_NOINHERITLAYOUT                0x00100000L
#   define NOMIRRORBITMAP                       0x80000000  // use in BitBlt
    
#endif

#ifndef DT_NOFULLWIDTHCHARBREAK
#   define DT_NOFULLWIDTHCHARBREAK  0x00080000
#endif

#ifndef CAPTUREBLT
#   define CAPTUREBLT               0x40000000 
#endif

#ifndef OBJID_WINDOW
#   define OBJID_WINDOW             0x00000000
#   define OBJID_SYSMENU            0xFFFFFFFF
#   define OBJID_TITLEBAR           0xFFFFFFFE
#   define OBJID_MENU               0xFFFFFFFD
#   define OBJID_CLIENT             0xFFFFFFFC
#   define OBJID_VSCROLL            0xFFFFFFFB
#   define OBJID_HSCROLL            0xFFFFFFFA
#   define OBJID_SIZEGRIP           0xFFFFFFF9
#   define OBJID_CARET              0xFFFFFFF8
#   define OBJID_CURSOR             0xFFFFFFF7
#   define OBJID_ALERT              0xFFFFFFF6
#   define OBJID_SOUND              0xFFFFFFF5
#endif 

#ifndef OBJID_QUERYCLASSNAMEIDX
#   define OBJID_QUERYCLASSNAMEIDX  0xFFFFFFF4
#   define OBJID_NATIVEOM           0xFFFFFFF0
#endif 

#define OBJID_UiaRootObjectId       0xffffffE7 // -25


#ifndef EVENT_OBJECT_INVOKED
#   define EVENT_OBJECT_INVOKED                0x8013  // hwnd + ID + idChild is item invoked
#   define EVENT_OBJECT_TEXTSELECTIONCHANGED   0x8014  // hwnd + ID + idChild is item w? test selection change
#   define EVENT_OBJECT_CONTENTSCROLLED        0x8015
#endif

#ifndef EVENT_SYSTEM_ARRANGMENTPREVIEW
#   define EVENT_SYSTEM_ARRANGMENTPREVIEW      0x8016
#endif

#ifndef EVENT_OBJECT_CLOAKED
#   define EVENT_OBJECT_CLOAKED                0x8017
#   define EVENT_OBJECT_UNCLOAKED              0x8018
#   define EVENT_OBJECT_LIVEREGIONCHANGED      0x8019
#   define EVENT_OBJECT_HOSTEDOBJECTSINVALIDATED 0x8020
#   define EVENT_OBJECT_DRAGSTART              0x8021
#   define EVENT_OBJECT_DRAGCANCEL             0x8022
#   define EVENT_OBJECT_DRAGCOMPLETE           0x8023
#   define EVENT_OBJECT_DRAGENTER              0x8024
#   define EVENT_OBJECT_DRAGLEAVE              0x8025
#   define EVENT_OBJECT_DRAGDROPPED            0x8026
#   define EVENT_OBJECT_IME_SHOW               0x8027
#   define EVENT_OBJECT_IME_HIDE               0x8028
#   define EVENT_OBJECT_IME_CHANGE             0x8029
#   define EVENT_OBJECT_TEXTEDIT_CONVERSIONTARGETCHANGED 0x8030
#endif


// DECLARE_DLL_FUNC("ole32.dll", void, WINAPI, CoFreeUnusedLibrariesEx, (DWORD, DWORD));
// #   define CoFreeUnusedLibrariesEx    CDllFunc_CoFreeUnusedLibrariesEx::Inst()


#ifndef COLOR_HOTLIGHT
#   define COLOR_HOTLIGHT                   26
#   define COLOR_GRADIENTACTIVECAPTION      27
#   define COLOR_GRADIENTINACTIVECAPTION    28
#endif

#ifndef DFCS_TRANSPARENT
#   define DFCS_TRANSPARENT                 0x0800 // this flag only used to DFC_MENU and DFC_POPUPMENU
#   define DFCS_HOT                         0x1000
#endif

#ifndef ABM_GETAUTOHIDEBAREX
#   define ABM_GETAUTOHIDEBAREX             0x0000000B
#endif



///////////////////////////////////////////////////////////////////////////////
// DPI API

// #ifndef DPI_ENUMS_DECLARED
// #define DPI_ENUMS_DECLARED
//     
//     typedef enum _PROCESS_DPI_AWARENESS { 
//         PROCESS_DPI_UNAWARE                 = 0,
//         PROCESS_SYSTEM_DPI_AWARE        = 1,
//         PROCESS_PER_MONITOR_DPI_AWARE   = 2
//     } PROCESS_DPI_AWARENESS;
// 
//     typedef enum _MONITOR_DPI_TYPE { 
//         MDT_EFFECTIVE_DPI  = 0,
//         MDT_ANGULAR_DPI    = 1,
//         MDT_RAW_DPI        = 2,
//         MDT_DEFAULT        = MDT_EFFECTIVE_DPI
//     } MONITOR_DPI_TYPE;
// 
// #endif // DPI_ENUMS_DECLARED

//     DECLARE_DLL_FUNC ("Shcore.dll", HRESULT, WINAPI, SetProcessDpiAwareness,
//         (PROCESS_DPI_AWARENESS value));
// #   define SetProcessDpiAwareness CDllFunc_SetProcessDpiAwareness::Inst()
// 
//     DECLARE_DLL_FUNC ("Shcore.dll", HRESULT, WINAPI, GetDpiForMonitor,
//         (HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY));
// #   define GetDpiForMonitor CDllFunc_GetDpiForMonitor::Inst()
// 
//     DECLARE_DLL_FUNC ("dll_user32.dll", BOOL, WINAPI, IsProcessDPIAware,
//         ());
// #   define IsProcessDPIAware CDllFunc_IsProcessDPIAware::Inst()



///////////////////////////////////////////////////////////////////////////////
// theme
#ifndef _UXTHEME_H_
    typedef struct _MARGINS {
        int cxLeftWidth;
        int cxRightWidth;
        int cyTopHeight;
        int cyBottomHeight;
    } MARGINS, *PMARGINS;
#endif

//     DECLARE_DLL_FUNC ("UxTheme.dll", HRESULT, WINAPI, SetWindowTheme,
//         (HWND hWnd, LPCTSTR pszSubAppName, LPCTSTR pszSubIDList));
// #   define SetWindowTheme CDllFunc_SetWindowTheme::Inst()



///////////////////////////////////////////////////////////////////////////////
// DWM API

#ifndef WM_DWMCOMPOSITIONCHANGED
#   define WM_DWMCOMPOSITIONCHANGED         0x031E
#   define WM_DWMNCRENDERINGCHANGED         0x031F
#   define WM_DWMCOLORIZATIONCOLORCHANGED   0x0320
#   define WM_DWMWINDOWMAXIMIZEDCHANGE      0x0321
#endif // WM_DWMCOMPOSITIONCHANGED


enum __DWMWINDOWATTRIBUTE { 
    _DWMWA_NCRENDERING_ENABLED          = 1,
    _DWMWA_NCRENDERING_POLICY,
    _DWMWA_TRANSITIONS_FORCEDISABLED,
    _DWMWA_ALLOW_NCPAINT,
    _DWMWA_CAPTION_BUTTON_BOUNDS,
    _DWMWA_NONCLIENT_RTL_LAYOUT,
    _DWMWA_FORCE_ICONIC_REPRESENTATION,
    _DWMWA_FLIP3D_POLICY,
    _DWMWA_EXTENDED_FRAME_BOUNDS,

    // > Windows Vista
    _DWMWA_HAS_ICONIC_BITMAP,
    _DWMWA_DISALLOW_PEEK,
    _DWMWA_EXCLUDED_FROM_PEEK,

    // > Windows 7
    _DWMWA_CLOAK,
    _DWMWA_CLOAKED,
    _DWMWA_FREEZE_REPRESENTATION,
    _DWMWA_PASSIVE_UPDATE_MODE,

    // >= Windows 11.22000
    _DWMWA_USE_HOSTBACKDROPBRUSH,
    _DWMWA_USE_IMMERSIVE_DARK_MODE = 20,
    _DWMWA_WINDOW_CORNER_PREFERENCE = 33,
    _DWMWA_BORDER_COLOR,
    _DWMWA_CAPTION_COLOR,
    _DWMWA_TEXT_COLOR,
    _DWMWA_VISIBLE_FRAME_BORDER_THICKNESS,

    // >= Windows 11.22621
    _DWMWA_SYSTEMBACKDROP_TYPE,
    _DWMWA_LAST
};

enum __DWMNCRENDERINGPOLICY {
    _DWMNCRP_USEWINDOWSTYLE,    // The non-client rendering area is rendered based on the window style.
    _DWMNCRP_DISABLED,            // The non-client area rendering is disabled; the window style is ignored.
    _DWMNCRP_ENABLED,            // The non-client area rendering is enabled; the window style is ignored.
    _DWMNCRP_LAST
};

enum __DWM_SYSTEMBACKDROP_TYPE {
    _DWMSBT_AUTO,
    _DWMSBT_NONE,                // default
    _DWMSBT_MAINWINDOW,            // mica
    _DWMSBT_TRANSIENTWINDOW,    // acrylic
    _DWMSBT_TABBEDWINDOW        // tabbed
};

#ifndef DWM_BB_ENABLE
#   define DWM_BB_ENABLE                 0x00000001
#   define DWM_BB_BLURREGION             0x00000002
#   define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004
    typedef struct _DWM_BLURBEHIND {
      DWORD dwFlags;
      BOOL  fEnable;
      HRGN  hRgnBlur;
      BOOL  fTransitionOnMaximized;
    } DWM_BLURBEHIND, *PDWM_BLURBEHIND;
    
#endif // DWM_BB_ENABLE

// _Dwmapi_DwmXXXX
DECLARE_DLL_FUNC3(dll_dwmapi, HRESULT, WINAPI, DwmExtendFrameIntoClientArea, (HWND hwnd, const MARGINS *pMarInse));
DECLARE_DLL_FUNC3(dll_dwmapi, BOOL,    WINAPI, DwmDefWindowProc, (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult));
DECLARE_DLL_FUNC3(dll_dwmapi, HRESULT, WINAPI, DwmGetWindowAttribute, (HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute));
DECLARE_DLL_FUNC3(dll_dwmapi, HRESULT, WINAPI, DwmSetWindowAttribute, (HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute));
DECLARE_DLL_FUNC3(dll_dwmapi, HRESULT, WINAPI, DwmIsCompositionEnabled, (BOOL *pfEnabled));
DECLARE_DLL_FUNC3(dll_dwmapi, HRESULT, WINAPI, DwmEnableBlurBehindWindow, (HWND hWnd, const DWM_BLURBEHIND *pBlurBehind));
DECLARE_DLL_FUNC3(dll_dwmapi, HRESULT, WINAPI, DwmFlush, (void));

#define _DwmExtendFrameIntoClientArea   TM_IMPORT(DwmExtendFrameIntoClientArea)
#define _DwmDefWindowProc               TM_IMPORT(DwmDefWindowProc)
#define _DwmGetWindowAttribute          TM_IMPORT(DwmGetWindowAttribute)
#define _DwmSetWindowAttribute          TM_IMPORT(DwmSetWindowAttribute)
#define _DwmIsCompositionEnabled        TM_IMPORT(DwmIsCompositionEnabled)
#define _DwmEnableBlurBehindWindow      TM_IMPORT(DwmEnableBlurBehindWindow)
#define _DwmFlush                       TM_IMPORT(DwmFlush)





///////////////////////////////////////////////////////////////////////////////
// undocument function

// SetWindowCompositionAttribute
typedef enum _WINDOWCOMPOSITIONATTRIB  
{  
    WCA_UNDEFINED = 0,  
    WCA_NCRENDERING_ENABLED = 1,  
    WCA_NCRENDERING_POLICY = 2,  
    WCA_TRANSITIONS_FORCEDISABLED = 3,  
    WCA_ALLOW_NCPAINT = 4,  
    WCA_CAPTION_BUTTON_BOUNDS = 5,  
    WCA_NONCLIENT_RTL_LAYOUT = 6,  
    WCA_FORCE_ICONIC_REPRESENTATION = 7,  
    WCA_EXTENDED_FRAME_BOUNDS = 8,  
    WCA_HAS_ICONIC_BITMAP = 9,  
    WCA_THEME_ATTRIBUTES = 10,  
    WCA_NCRENDERING_EXILED = 11,  
    WCA_NCADORNMENTINFO = 12,  
    WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,  
    WCA_VIDEO_OVERLAY_ACTIVE = 14,  
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,  
    WCA_DISALLOW_PEEK = 16,  
    WCA_CLOAK = 17,  
    WCA_CLOAKED = 18,  
    WCA_ACCENT_POLICY = 19,  
    WCA_FREEZE_REPRESENTATION = 20,  
    WCA_EVER_UNCLOAKED = 21,  
    WCA_VISUAL_OWNER = 22,  
    WCA_LAST = 23  
} WINDOWCOMPOSITIONATTRIB;  

typedef struct _WINDOWCOMPOSITIONATTRIBDATA  
{  
    WINDOWCOMPOSITIONATTRIB dwAttrib;  
    PVOID pvData;  
    SIZE_T cbData;  
} WINDOWCOMPOSITIONATTRIBDATA;  

typedef enum _ACCENT_STATE  
{  
    ACCENT_DISABLED = 0,  
    ACCENT_ENABLE_GRADIENT = 1,  
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,  
    ACCENT_ENABLE_BLURBEHIND = 3,  
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,    // RS4 1803
    ACCENT_ENABLE_HOSTBACKDROP = 5,         // RS5 1809
    ACCENT_INVALID_STATE = 6,
} ACCENT_STATE;  

typedef struct _ACCENT_POLICY  
{  
    ACCENT_STATE AccentState;  
    DWORD AccentFlags;  
    DWORD GradientColor;  
    DWORD AnimationId;  
} ACCENT_POLICY;  

DECLARE_DLL_FUNC3(dll_user32, BOOL, WINAPI, SetWindowCompositionAttribute,
    (HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA* pAttrData));
#define _SetWindowCompositionAttribute TM_IMPORT(SetWindowCompositionAttribute)


///////////////////////////////////////////////////////////////////////////////
// touch pad    

#ifndef WM_POINTERDEVICECHANGE //(WINVER >= 0x0600)
#   define WM_POINTERDEVICECHANGE          0x238
#   define WM_POINTERDEVICEINRANGE         0x239
#   define WM_POINTERDEVICEOUTOFRANGE      0x23A
#endif /* WM_POINTERDEVICECHANGE */
    
#ifndef WM_TOUCH //(WINVER >= 0x0601)
#   define WM_TOUCH                        0x0240
#   define WM_GESTURE                      0x0119
#   define WM_GESTURENOTIFY                0x011A

/* Gesture flags - GESTUREINFO.dwFlags */
#   define GF_BEGIN                        0x00000001
#   define GF_INERTIA                      0x00000002
#   define GF_END                          0x00000004
    
/* Gesture IDs */
#   define GID_BEGIN                       1
#   define GID_END                         2
#   define GID_ZOOM                        3
#   define GID_PAN                         4
#   define GID_ROTATE                      5
#   define GID_TWOFINGERTAP                6
#   define GID_PRESSANDTAP                 7
#   define GID_ROLLOVER                    GID_PRESSANDTAP

    DECLARE_HANDLE(HGESTUREINFO);

    typedef struct tagGESTURENOTIFYSTRUCT {
        UINT   cbSize;
        DWORD  dwFlags;
        HWND   hwndTarget;
        POINTS ptsLocation;
        DWORD  dwInstanceID;
    } GESTURENOTIFYSTRUCT, *PGESTURENOTIFYSTRUCT;

    typedef struct tagGESTURECONFIG {
        DWORD dwID;                     // gesture ID
        DWORD dwWant;                   // settings related to gesture ID that are to be turned on
        DWORD dwBlock;                  // settings related to gesture ID that are to be turned off
    } GESTURECONFIG, *PGESTURECONFIG;

    typedef struct tagGESTUREINFO {
        UINT cbSize;                    // size, in bytes, of this structure (including variable length Args field)
        DWORD dwFlags;                  // see GF_* flags
        DWORD dwID;                     // gesture ID, see GID_* defines
        HWND hwndTarget;                // handle to window targeted by this gesture
        POINTS ptsLocation;             // current location of this gesture
        DWORD dwInstanceID;             // internally used
        DWORD dwSequenceID;             // internally used
        ULONGLONG ullArguments;         // arguments for gestures whose arguments fit in 8 BYTES
        UINT cbExtraArgs;               // size, in bytes, of extra arguments, if any, that accompany this gesture
    } GESTUREINFO, *PGESTUREINFO;

/* Common gesture configuration flags - set GESTURECONFIG.dwID to zero */
#   define GC_ALLGESTURES                              0x00000001

/* Zoom gesture configuration flags - set GESTURECONFIG.dwID to GID_ZOOM */
#   define GC_ZOOM                                     0x00000001 

// Pan gesture configuration flags - set GESTURECONFIG.dwID to GID_PAN
#   define GC_PAN                                      0x00000001
#   define GC_PAN_WITH_SINGLE_FINGER_VERTICALLY        0x00000002
#   define GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY      0x00000004
#   define GC_PAN_WITH_GUTTER                          0x00000008
#   define GC_PAN_WITH_INERTIA                         0x00000010

// Rotate gesture configuration flags - set GESTURECONFIG.dwID to GID_ROTATE
#   define GC_ROTATE                                   0x00000001

// Two finger tap gesture configuration flags - set GESTURECONFIG.dwID to GID_TWOFINGERTAP
#   define GC_TWOFINGERTAP                             0x00000001

// PressAndTap gesture configuration flags - set GESTURECONFIG.dwID to GID_PRESSANDTAP
#   define GC_PRESSANDTAP                              0x00000001
#   define GC_ROLLOVER                                 GC_PRESSANDTAP


#endif // WM_TOUCH
    
#ifndef WM_NCPOINTERUPDATE // (WINVER >= 0x0602)
#   define WM_NCPOINTERUPDATE              0x0241
#   define WM_NCPOINTERDOWN                0x0242
#   define WM_NCPOINTERUP                  0x0243
#   define WM_POINTERUPDATE                0x0245
#   define WM_POINTERDOWN                  0x0246
#   define WM_POINTERUP                    0x0247
#   define WM_POINTERENTER                 0x0249
#   define WM_POINTERLEAVE                 0x024A
#   define WM_POINTERACTIVATE              0x024B
#   define WM_POINTERCAPTURECHANGED        0x024C
#   define WM_TOUCHHITTESTING              0x024D
#   define WM_POINTERWHEEL                 0x024E
#   define WM_POINTERHWHEEL                0x024F
#   define DM_POINTERHITTEST               0x0250
#endif /* WINVER >= 0x0602 */

DECLARE_DLL_FUNC3(dll_user32, BOOL, WINAPI, SetGestureConfig, (HWND hWnd, DWORD dwReserved, UINT cIDs, PGESTURECONFIG pGestureConfig, UINT cbSize));
DECLARE_DLL_FUNC3(dll_user32, BOOL, WINAPI, GetGestureConfig, (HWND hwnd, DWORD dwReserved, DWORD dwFlags, PUINT pcIDs, PGESTURECONFIG pGestureConfig, UINT cbSize));
DECLARE_DLL_FUNC3(dll_user32, BOOL, WINAPI, GetGestureInfo, (HGESTUREINFO hGestureInfo, PGESTUREINFO pGestureInfo));
DECLARE_DLL_FUNC3(dll_user32, BOOL, WINAPI, CloseGestureInfoHandle, (HGESTUREINFO hGestureInfo));
#define _SetGestureConfig       TM_IMPORT(SetGestureConfig)
#define _GetGestureConfig       TM_IMPORT(GetGestureConfig)
#define _GetGestureInfo         TM_IMPORT(GetGestureInfo)
#define _CloseGestureInfoHandle TM_IMPORT(CloseGestureInfoHandle)


// DXGI error code
#ifndef DXGI_ERROR_DEVICE_REMOVED
#   define DXGI_ERROR_DEVICE_REMOVED        _HRESULT_TYPEDEF_(0x887A0005L)
#   define DXGI_ERROR_DEVICE_HUNG           _HRESULT_TYPEDEF_(0x887A0006L)
#   define DXGI_ERROR_DEVICE_RESET          _HRESULT_TYPEDEF_(0x887A0007L)
#endif



#ifdef TM_CPP

class COSVersion
{
public:
    enum EVersion
    {
        eVersionUnkown  =      0,
        eVersionPrev    =  0x500,
        eVersionXp      =  0x501,
        eVersionVista   =  0x600,
        eVersionWin7    =  0x601,
        eVersionWin8    =  0x602,
        eVersionWin10   = 0x1000,
        eVersionWin11   = 0x1100,
    };

    typedef TMUInt64 DETAIL;

    struct _OSVersion
    {
        DWORD major;
        DWORD minor;
        DWORD build;

        _OSVersion()
        {
            DECLARE_DLL_FUNC(dll_ntdll, void, WINAPI, RtlGetNtVersionNumbers, (LPDWORD major, LPDWORD minor, LPDWORD build));
            _RtlGetNtVersionNumbers(&major, &minor, &build);
        }

        EVersion GetVersion()
        {
            if (minor == 5 && minor > 0) return eVersionXp;
            if (major < 6) return eVersionPrev;
            if (major == 6) return (EVersion)((major << 16) + minor);
            if (major == 10)
            {
                if (TM_LOWORD(build) < 22000) return eVersionWin10; // 22000: 0x55f0
                return eVersionWin11;
            }
            return eVersionUnkown;
        }

        DETAIL GetVersionDetail()
        {
            if (minor == 5 && minor > 0) return MakeDetail(eVersionXp, build);
            if (major < 6) return               MakeDetail(eVersionPrev, build);
            if (major == 6) return              MakeDetail(((major << 16) + minor), build);
            if (major == 10)
            {
                if (TM_LOWORD(build) < 22000)   // 22000: 0x55f0
                    return                      MakeDetail(eVersionWin10, build);
                return                          MakeDetail(eVersionWin11, build);
            }
            return eVersionUnkown;
        }
    };

    static _OSVersion& GetVerStruct()
    {
        static _OSVersion s_ov;
        return s_ov;
    }

    static tm_constexpr11 DETAIL MakeDetail(DWORD ver, DWORD build)
    {
        return build | (((DETAIL)ver) << 32);
    }

    static EVersion GetVersion() { return GetVerStruct().GetVersion(); }
    static DETAIL GetVersionDetail() { return GetVerStruct().GetVersionDetail(); }

    static DWORD GetMajor() { return GetVerStruct().major; }
    static DWORD GetMinor() { return GetVerStruct().minor; }
    static DWORD GetBuild() { return GetVerStruct().build; }

};


#endif


///////////////////////////////////////////////////////////////////////////////
// HWND hWnd = CreateWindowT<__line__>(L"aaa");
/*
struct CCreateWindowParam
{
public:
    HWND m_hWndParent;
    DWORD m_dwExStyle;
    DWORD m_dwStyle;
    int m_X;
    int m_Y;
    int m_nWidth;
    int m_nHeight;
    HMENU m_hMenu;
    HINSTANCE m_hInstance;

    CCreateWindowParam(
        HWND hWndParent = 0,
        DWORD dwExStyle = 0,
        DWORD dwStyle = 0,
        int X = 0,
        int Y = 0,
        int nWidth = 0,
        int nHeight = 0,
        HMENU hMenu = 0,
        HINSTANCE hInstance = 0)
        : m_hWndParent(hWndParent)
        , m_dwExStyle(dwExStyle)
        , m_dwStyle(dwStyle)
        , m_X(X)
        , m_Y(Y)
        , m_nWidth(nWidth)
        , m_nHeight(nHeight)
        , m_hMenu(hMenu)
        , m_hInstance(hInstance)
    {
    }

    ATOM Register(LPCTSTR lpClassName)
    {
        WNDCLASSEX cls = {
            sizeof(WNDCLASSEX),
            CS_DBLCLKS | CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW,
            ::DefWindowProc,
            0,
            0,
            0,
            0,
            ::LoadCursor(0, IDC_ARROW),
            0,
            0,
            lpClassName,
            0,
        };
        ATOM atom1 = RegisterClassEx(&cls);
        return atom1;
    }

    HWND Create(
        LPCTSTR lpClassName,
        LPCTSTR lpWindowName = 0)
    {
        static ATOM atom1 = Register(lpClassName);
        return ::CreateWindowEx(m_dwExStyle, lpClassName, lpWindowName, m_dwStyle, m_X, m_Y, m_nWidth, m_nHeight, m_hWndParent, m_hMenu, m_hInstance, 0);
    }

    HWND Create(
        LPCTSTR lpClassName,
        LPCTSTR lpWindowName,
        WNDPROC proc)
    {
        HWND hWnd = Create(lpClassName, lpWindowName);
        ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (ULONG_PTR)proc);
        return hWnd;
    }

    HWND Create(
        LPCTSTR lpClassName,
        LPCTSTR lpWindowName,
        WNDPROC proc,
        void* userdata)
    {
        HWND hWnd = Create(lpClassName, lpWindowName, proc);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (ULONG_PTR)userdata);
        return hWnd;
    }

    static void* GetUserData(HWND hWnd)
    {
        return (void*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }
};

template <int n>
struct CreateWindowT
{
    HWND m_hWnd;

    static ATOM Register(LPCTSTR lpClassName)
    {
        WNDCLASSEX  wc = 
        {
            sizeof (WNDCLASSEX),
                CS_DBLCLKS | CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW,// | CS_OWNDC,
                ::DefWindowProc, 0, 0, 0, 0,
                ::LoadCursor(0, IDC_ARROW),
                0, //(HBRUSH)GetStockObject(NULL_BRUSH),
                0, lpClassName, 0
        };
        return RegisterClassEx(&wc); 
    }

    CreateWindowT(
        LPCTSTR lpClassName,
        HWND hWndParent = 0,
        DWORD dwExStyle = 0,
        LPCTSTR lpWindowName = 0,
        DWORD dwStyle = 0,
        int X = 0,
        int Y = 0,
        int nWidth = 0,
        int nHeight = 0,
        HMENU hMenu = 0,
        HINSTANCE hInstance = 0)
    {
        static ATOM atom1 = Register(lpClassName);
        m_hWnd = ::CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, 0);
    }

    CreateWindowT(
        LPCTSTR lpClassName,
        HWND hWndParent,
        DWORD dwExStyle,
        LPCTSTR lpWindowName,
        DWORD dwStyle,
        int X,
        int Y,
        int nWidth,
        int nHeight,
        HMENU hMenu,
        HINSTANCE hInstance,
        WNDPROC proc)
    {
        static ATOM atom1 = Register(lpClassName);
        m_hWnd = ::CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, 0);
        ::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (ULONG_PTR)proc);
    }

    operator HWND () const { return m_hWnd; }
};
*/


#endif // TMPLATFORM_WIN_H
