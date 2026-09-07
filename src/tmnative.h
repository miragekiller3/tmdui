/*
 *  @file
 *  @brief  native window support
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.23
 *
 *  Copyright (C) 2011 miragekiller
 */

#pragma once

#ifndef TMNATIVE_H
#define TMNATIVE_H

#include "tmdef.h"

#if TM_WIN_DESKTOP

#   include <windows.h>
#   include <richedit.h>

// #   define TMHModule                    HMODULE
#   define TMHandle                     HANDLE
#   define TMHWnd                       HWND
#   define TMHDC                        HDC
#   define TMHBmp                       HBITMAP
#   define TMHBrush                     HBRUSH
#   define TMHMenu                      HMENU  
#   define TMHGDIObj                    HGDIOBJ
#   define TMHFont                      HFONT
#   define TMHIcon                      HICON
#   define TMHMonitor                   HMONITOR

#   define TMHWndNative                 HWND   
#   define TMHDCNative                  HDC


#elif TM_WIN_METRO

#   include <windows.h>
#   include <richedit.h>

// #   define TMHModule                    HMODULE
#   define TMHandle                     HANDLE
//    typedef Windows::UI::Core::CoreWindow^  TMHWnd;
#   define TMHWnd                       HWND   
#   define TMHDC                        HDC    
#   define TMHBmp                       HBITMAP
#   define TMHBrush                     HBRUSH
#   define TMHGDIObj                    HGDIOBJ
#   define TMHFont                      HFONT
#   define TMHIcon                      HICON
#   define TMHMonitor                   HMONITOR

#   define TMHWndNative                 HWND   
#   define TMHDCNative                  HDC

#elif TM_APPLE

#   include <CoreGraphics/CGContext.h>
#   include <CoreFoundation/CFBundle.h>
#   include "mtcore.h"

// #   define TMHModule                    CFBundleRef
#   define TMHDC                        CGContextRef    
#   define TMHBmp                       CGImageRef
#   define TMHFont                      CGFontRef
#   define TMHIcon                      CGImageRef
#   define TMHGDIObj                    CGImageRef
#   define TMHWndNative                 void*
#   define TMHDCNative                  CGContextRef

#elif TM_ANDROID

//#   include <android/native_window.h>
#   define TMHWnd                       struct ANativeWindow*   
#   define TMHWndNative                 struct ANativeWindow*  

#elif defined(__WINSCW__) || defined(__SYMBIAN32__)  /* Symbian */

#   define TMHDC                        int

#elif defined(__QNXNTO__)

#   include <screen/screen.h>

#   define TMHWnd                       screen_window_t   
#   define TMHDC                        int    
#   define TMHWndNative                 screen_window_t  

#elif defined(TM_GTK)

#   include <gtk/gtk.h>

#   define TMHWnd                       GtkWindow*   
#   define TMHDC                        cairo_t*    
#   define TMHBmp                       GdkPixbuf*
#   define TMHBrush                     GdkPixbuf*
#   define TMHIcon                      GdkPixbuf*    
#   define TMHCursor                    GdkCursor*    
#   define TMHWndNative                 GtkWindow*
#   define TMHMenu                      GtkMenu*  
#   define TMHMonitor                   GdkMonitor*

#elif defined(__unix__)

#   include <X11/Xlib.h>
#   include <X11/Xutil.h>

#   define TMHWnd                       Window   
#   define TMHDC                        Display*    
#   define TMHBmp                       Pixmap
#   define TMHWndNative                 Window  

#endif

#ifndef TMHWnd
#   define TMHWnd                       void*
#endif

#ifndef TMHandle
#   define TMHandle                     void*
#endif

// #ifndef TMHModule
// #   define TMHModule                    void*
// #endif

#ifndef TMHMenu
#   define TMHMenu                      void*
#endif

#ifndef TMHDC
#   define TMHDC                        void*
#endif

#ifndef TMHBmp
#   define TMHBmp                       void*
#endif

#ifndef TMHBrush
#   define TMHBrush                     void*
#endif

#ifndef TMHGDIObj
#   define TMHGDIObj                    void*
#endif

#ifndef TMHFont
#   define TMHFont                      void*
#endif

#ifndef TMHIcon
#   define TMHIcon                      void*
#endif

#ifdef TM_QT

#   include <QWidget>
#   include <QWindow>
#   include <QDialog>
#   include <QScreen>
#   include <QGuiApplication>

#   undef TMHWnd
#   undef TMHDC
#   undef TMHBmp
#   undef TMHBrush
#   undef TMHIcon
#   undef TMHFont
#   undef TMHMonitor
#   define TMHWnd                       QDialog*
//#   define TMHWnd                       QWindow*
#   define TMHDC                        QPainter*
#   define TMHBmp                       QImage*
#   define TMHBrush                     QBrush*
#   define TMHIcon                      QIcon
#   define TMHFont                      QFont
#   define TMHMonitor                   QScreen*

/*
 * Standard Cursor IDs
 */
#   define TMCursorID                   Qt::CursorShape
#   define TMCursor_ARROW               Qt::ArrowCursor
#   define TMCursor_IBEAM               Qt::IBeamCursor
#   define TMCursor_WAIT                Qt::WaitCursor
#   define TMCursor_CROSS               Qt::CrossCursor
#   define TMCursor_UPARROW             Qt::UpArrowCursor
#   define TMCursor_SIZENWSE            Qt::SizeFDiagCursor
#   define TMCursor_SIZENESW            Qt::SizeBDiagCursor
#   define TMCursor_SIZEWE              Qt::SizeHorCursor
#   define TMCursor_SIZENS              Qt::SizeVerCursor
#   define TMCursor_SIZEALL             Qt::SizeAllCursor
#   define TMCursor_NO                  Qt::ForbiddenCursor
#   define TMCursor_HAND                Qt::PointingHandCursor
#   define TMCursor_APPSTARTING         Qt::WaitCursor
#   define TMCursor_HELP                Qt::WhatsThisCursor
#   define TMCursor_SPLITV              Qt::SplitVCursor
#   define TMCursor_SPLITH              Qt::SplitHCursor

#endif



// typedef TM_SW_WIN(WPARAM, TMULongPtr)  TMWParam;   // WPARAM
// typedef TM_SW_WIN(LPARAM, TMULongPtr)  TMLParam;   // LPARAM
// typedef TM_SW_WIN(LRESULT, TMLLongPtr)  TMResult;   // LRESULT

#ifndef MAKELONG
#   define MAKELONG(a, b)   ((TMLong)(((TMUShort)(a)) | ((TMULong)((TMUShort)(b))) << 16))
#   define MAKEWPARAM(l, h)      (TMWParam)MAKELONG(l, h)
#   define MAKELPARAM(l, h)      (TMLParam)MAKELONG(l, h)
#endif

#ifndef LOWORD
#define LOWORD(l)           ((TMUShort)((TMLParam)(l)) & 0xFFFF)
#endif

#ifndef HIWORD
#define HIWORD(l)           ((TMUShort)(((TMLParam)(l) >> 16) & 0xFFFF))
#endif

#ifndef CALLBACK
#   define CALLBACK
#endif

typedef TMUInt32 ATLocal; // LCID


#ifndef TM_WIN


///////////////////////////////////////////////////////////////////////////////
// com

// typedef struct _GUID
// {
//     TMUInt32   Data1;
//     unsigned short Data2;
//     unsigned short Data3;
//     unsigned char Data4[8];
// } GUID;
// 
// typedef GUID IID;
// typedef GUID CLSID;
// #define REFIID                      const IID &

// typedef union _LARGE_INTEGER
// {
//     struct {
//         TMUInt32 LowPart;
//         TMInt32 HighPart;
//     };
//     struct {
//         TMUInt32 LowPart;
//         TMInt32 HighPart;
//     } u;
//     TMInt64 QuadPart;
// } LARGE_INTEGER;
// 
// typedef LARGE_INTEGER *PLARGE_INTEGER;
// 
// 
// typedef union _ULARGE_INTEGER
// {
//     struct {
//         TMUInt32 LowPart;
//         TMUInt32 HighPart;
//     };
//     struct {
//         TMUInt32 LowPart;
//         TMUInt32 HighPart;
//     } u;
//     TMUInt64 QuadPart;
// } ULARGE_INTEGER;
// 
// typedef ULARGE_INTEGER *PULARGE_INTEGER;
// 
// typedef struct  tagSTATSTG
// {
//     TMULong type;
//     ULARGE_INTEGER cbSize;
// } STATSTG;
// 
// typedef enum tagSTGTY
// {
//     STGTY_STORAGE    = 1,
//     STGTY_STREAM    = 2,
//     STGTY_LOCKBYTES    = 3,
//     STGTY_PROPERTY    = 4
// }    STGTY;
// 
// typedef enum tagSTREAM_SEEK
// {
//     STREAM_SEEK_SET    = 0,
//     STREAM_SEEK_CUR    = 1,
//     STREAM_SEEK_END    = 2
// }    STREAM_SEEK;
// 

typedef struct tagRGBQUAD
{
    TMUInt8 rgbBlue;
    TMUInt8 rgbGreen;
    TMUInt8 rgbRed;
    TMUInt8 rgbReserved;
} RGBQUAD, *LPRGBQUAD;

typedef struct tagBITMAPINFOHEADER{
    TMUInt32      biSize;
    TMInt32       biWidth;
    TMInt32       biHeight;
    TMUInt16      biPlanes;
    TMUInt16      biBitCount;
    TMUInt32      biCompression;
    TMUInt32      biSizeImage;
    TMInt32       biXPelsPerMeter;
    TMInt32       biYPelsPerMeter;
    TMUInt32      biClrUsed;
    TMUInt32      biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER;

typedef struct tagBITMAPINFO
{
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO;

#define DIB_RGB_COLORS      0 /* color table in RGBs */
#define SRCCOPY             0x00CC0020 /* dest = source                   */



#define MAKEINTRESOURCE(x)  ((TMCStr)x)

// class CWindow
// {
// public:
//     TMHWnd m_hWnd;
// 
//     CWindow(TMHWnd hWnd = 0) : m_hWnd(hWnd) {}
// };
// 
// TM_INLINE TMBool IsWindow(TMHWnd hWnd) { return hWnd != 0; }




//typedef struct tagMINMAXINFO {
//    TMPoint ptReserved;
//    TMPoint ptMaxSize;
//    TMPoint ptMaxPosition;
//    TMPoint ptMinTrackSize;
//    TMPoint ptMaxTrackSize;
//} MINMAXINFO, *PMINMAXINFO, *LPMINMAXINFO;


/*
 * MEASUREITEMSTRUCT for ownerdraw
 */
//typedef struct tagMEASUREITEMSTRUCT {
//    TMUInt       CtlType;
//    TMUInt       CtlID;
//    TMUInt       itemID;
//    TMUInt       itemWidth;
//    TMUInt       itemHeight;
//    TMULong      itemData;
//} MEASUREITEMSTRUCT, *LPMEASUREITEMSTRUCT;

/*
 * DRAWITEMSTRUCT for ownerdraw
 */
//typedef struct tagDRAWITEMSTRUCT {
//    TMUInt        CtlType;
//    TMUInt        CtlID;
//    TMUInt        itemID;
//    TMUInt        itemAction;
//    TMUInt        itemState;
//    TMHWnd        hwndItem;
//    TMHDC         hDC;
//    TMRect        rcItem;
//    TMULLong      itemData;
//} DRAWITEMSTRUCT, *LPDRAWITEMSTRUCT;

/*
 * DELETEITEMSTRUCT for ownerdraw
 */
//typedef struct tagDELETEITEMSTRUCT {
//    TMUInt       CtlType;
//    TMUInt       CtlID;
//    TMUInt       itemID;
//    TMHWnd       hwndItem;
//    TMUInt       itemData;
//} DELETEITEMSTRUCT, *LPDELETEITEMSTRUCT;


/*
 * Owner draw control types
 */
#define ODT_MENU        1
#define ODT_LISTBOX     2
#define ODT_COMBOBOX    3
#define ODT_BUTTON      4
#define ODT_STATIC      5

/*
 * Owner draw state
 */
#define ODS_SELECTED        0x0001
#define ODS_GRAYED          0x0002
#define ODS_DISABLED        0x0004
#define ODS_CHECKED         0x0008
#define ODS_FOCUS           0x0010
#define ODS_DEFAULT         0x0020
#define ODS_COMBOBOXEDIT    0x1000
#define ODS_HOTLIGHT        0x0040
#define ODS_INACTIVE        0x0080

/*
 * Owner draw actions
 */
#define ODA_DRAWENTIRE  0x0001
#define ODA_SELECT      0x0002
#define ODA_FOCUS       0x0004


#define WS_OVERLAPPED       0x00000000L
#define WS_POPUP            0x80000000L
#define WS_CHILD            0x40000000L
#define WS_MINIMIZE         0x20000000L
#define WS_VISIBLE          0x10000000L
#define WS_DISABLED         0x08000000L
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L
#define WS_MAXIMIZE         0x01000000L
#define WS_CAPTION          0x00C00000L     /* WS_BORDER | WS_DLGFRAME  */
#define WS_BORDER           0x00800000L
#define WS_DLGFRAME         0x00400000L
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_THICKFRAME       0x00040000L
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L
#define WS_MINIMIZEBOX      0x00020000L
#define WS_MAXIMIZEBOX      0x00010000L
#define WS_TILED            WS_OVERLAPPED
#define WS_ICONIC           WS_MINIMIZE
#define WS_SIZEBOX          WS_THICKFRAME
#define WS_TILEDWINDOW      WS_OVERLAPPEDWINDOW
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_POPUPWINDOW      (WS_POPUP | WS_BORDER | WS_SYSMENU)
#define WS_CHILDWINDOW      (WS_CHILD)

#define DS_CENTER           0x0800L


/*
 * Extended Window Styles
 */
#define WS_EX_DLGMODALFRAME     0x00000001L
#define WS_EX_NOPARENTNOTIFY    0x00000004L
#define WS_EX_TOPMOST           0x00000008L
#define WS_EX_ACCEPTFILES       0x00000010L
#define WS_EX_TRANSPARENT       0x00000020L
#define WS_EX_MDICHILD          0x00000040L
#define WS_EX_TOOLWINDOW        0x00000080L
#define WS_EX_WINDOWEDGE        0x00000100L
#define WS_EX_CLIENTEDGE        0x00000200L
#define WS_EX_CONTEXTHELP       0x00000400L
#define WS_EX_RIGHT             0x00001000L
#define WS_EX_LEFT              0x00000000L
#define WS_EX_RTLREADING        0x00002000L
#define WS_EX_LTRREADING        0x00000000L
#define WS_EX_LEFTSCROLLBAR     0x00004000L
#define WS_EX_RIGHTSCROLLBAR    0x00000000L
#define WS_EX_CONTROLPARENT     0x00010000L
#define WS_EX_STATICEDGE        0x00020000L
#define WS_EX_APPWINDOW         0x00040000L
#define WS_EX_NOINHERITLAYOUT   0x00100000L // Disable inheritence of mirroring by children
#define WS_EX_LAYOUTRTL         0x00400000L // Right to left mirroring
#define WS_EX_COMPOSITED        0x02000000L
#define WS_EX_NOACTIVATE        0x08000000L


#define WS_EX_OVERLAPPEDWINDOW  (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
#define WS_EX_PALETTEWINDOW     (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)




/*
 * Virtual Keys, Standard Set
 */
// #define VK_LBUTTON        0x01
// #define VK_RBUTTON        0x02
// #define VK_CANCEL         0x03
// #define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */
// 
// #define VK_BACK           0x08
// #define VK_TAB            0x09
// 
// #define VK_CLEAR          0x0C
// #define VK_RETURN         0x0D
// 
// #define VK_SHIFT          0x10
// #define VK_CONTROL        0x11
// #define VK_MENU           0x12
// #define VK_PAUSE          0x13
// #define VK_CAPITAL        0x14
// 
// #define VK_KANA           0x15
// #define VK_HANGEUL        0x15  /* old name - should be here for compatibility */
// #define VK_HANGUL         0x15
// #define VK_JUNJA          0x17
// #define VK_FINAL          0x18
// #define VK_HANJA          0x19
// #define VK_KANJI          0x19
// 
// #define VK_ESCAPE         0x1B
// 
// #define VK_CONVERT        0x1C
// #define VK_NONCONVERT     0x1D
// #define VK_ACCEPT         0x1E
// #define VK_MODECHANGE     0x1F
// 
// #define VK_SPACE          0x20
// #define VK_PRIOR          0x21
// #define VK_NEXT           0x22
// #define VK_END            0x23
// #define VK_HOME           0x24
// #define VK_LEFT           0x25
// #define VK_UP             0x26
// #define VK_RIGHT          0x27
// #define VK_DOWN           0x28
// #define VK_SELECT         0x29
// #define VK_PRINT          0x2A
// #define VK_EXECUTE        0x2B
// #define VK_SNAPSHOT       0x2C
// #define VK_INSERT         0x2D
// #define VK_DELETE         0x2E
// #define VK_HELP           0x2F
// 
// /* VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
// /* VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */
// 
// #define VK_LWIN           0x5B
// #define VK_RWIN           0x5C
// #define VK_APPS           0x5D
// 
// #define VK_NUMPAD0        0x60
// #define VK_NUMPAD1        0x61
// #define VK_NUMPAD2        0x62
// #define VK_NUMPAD3        0x63
// #define VK_NUMPAD4        0x64
// #define VK_NUMPAD5        0x65
// #define VK_NUMPAD6        0x66
// #define VK_NUMPAD7        0x67
// #define VK_NUMPAD8        0x68
// #define VK_NUMPAD9        0x69
// #define VK_MULTIPLY       0x6A
// #define VK_ADD            0x6B
// #define VK_SEPARATOR      0x6C
// #define VK_SUBTRACT       0x6D
// #define VK_DECIMAL        0x6E
// #define VK_DIVIDE         0x6F
// #define VK_F1             0x70
// #define VK_F2             0x71
// #define VK_F3             0x72
// #define VK_F4             0x73
// #define VK_F5             0x74
// #define VK_F6             0x75
// #define VK_F7             0x76
// #define VK_F8             0x77
// #define VK_F9             0x78
// #define VK_F10            0x79
// #define VK_F11            0x7A
// #define VK_F12            0x7B
// #define VK_F13            0x7C
// #define VK_F14            0x7D
// #define VK_F15            0x7E
// #define VK_F16            0x7F
// #define VK_F17            0x80
// #define VK_F18            0x81
// #define VK_F19            0x82
// #define VK_F20            0x83
// #define VK_F21            0x84
// #define VK_F22            0x85
// #define VK_F23            0x86
// #define VK_F24            0x87
// 
// #define VK_NUMLOCK        0x90
// #define VK_SCROLL         0x91

/*
 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
// #define VK_LSHIFT         0xA0
// #define VK_RSHIFT         0xA1
// #define VK_LCONTROL       0xA2
// #define VK_RCONTROL       0xA3
// #define VK_LMENU          0xA4
// #define VK_RMENU          0xA5

/*
 * Dialog Box Command IDs
 */
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#define IDCLOSE             8
#define IDHELP              9

/*
 * MessageBox() Flags
 */
#define MB_OK               0x00000000L
#define MB_OKCANCEL         0x00000001L
#define MB_ABORTRETRYIGNORE 0x00000002L
#define MB_YESNOCANCEL      0x00000003L
#define MB_YESNO            0x00000004L
#define MB_RETRYCANCEL      0x00000005L

// * Scroll Bar Constants
#define SB_HORZ             0
#define SB_VERT             1
#define SB_CTL              2
#define SB_BOTH             3

// Scroll Bar Commands
#define SB_LINEUP           0
#define SB_LINELEFT         0
#define SB_LINEDOWN         1
#define SB_LINERIGHT        1
#define SB_PAGEUP           2
#define SB_PAGELEFT         2
#define SB_PAGEDOWN         3
#define SB_PAGERIGHT        3
#define SB_THUMBPOSITION    4
#define SB_THUMBTRACK       5
#define SB_TOP              6
#define SB_LEFT             6
#define SB_BOTTOM           7
#define SB_RIGHT            7
#define SB_ENDSCROLL        8


/*
 * SetWindowPos Flags
 */
#define SWP_NOSIZE          0x0001
#define SWP_NOMOVE          0x0002
#define SWP_NOZORDER        0x0004
#define SWP_NOREDRAW        0x0008
#define SWP_NOACTIVATE      0x0010
#define SWP_FRAMECHANGED    0x0020  /* The frame changed: send WM_NCCALCSIZE */
#define SWP_SHOWWINDOW      0x0040
#define SWP_HIDEWINDOW      0x0080
#define SWP_NOCOPYBITS      0x0100
#define SWP_NOOWNERZORDER   0x0200  /* Don't do owner Z ordering */
#define SWP_NOSENDCHANGING  0x0400  /* Don't send WM_WINDOWPOSCHANGING */
#define SWP_DRAWFRAME       SWP_FRAMECHANGED
#define SWP_NOREPOSITION    SWP_NOOWNERZORDER
#define SWP_DEFERERASE      0x2000
#define SWP_ASYNCWINDOWPOS  0x4000

/*
 * Key State Masks for Mouse Messages
 */
#define MK_LBUTTON          0x0001
#define MK_RBUTTON          0x0002
#define MK_SHIFT            0x0004
#define MK_CONTROL          0x0008
#define MK_MBUTTON          0x0010


/*
 * GetWindow() Constants
 */
#define GW_HWNDFIRST        0
#define GW_HWNDLAST         1
#define GW_HWNDNEXT         2
#define GW_HWNDPREV         3
#define GW_OWNER            4
#define GW_CHILD            5
#define GW_ENABLEDPOPUP     6
#define GW_MAX              6

/*
 * System Menu Command Values
 */
#define SC_SIZE         0xF000
#define SC_MOVE         0xF010
#define SC_MINIMIZE     0xF020
#define SC_MAXIMIZE     0xF030
#define SC_NEXTWINDOW   0xF040
#define SC_PREVWINDOW   0xF050
#define SC_CLOSE        0xF060
#define SC_VSCROLL      0xF070
#define SC_HSCROLL      0xF080
#define SC_MOUSEMENU    0xF090
#define SC_KEYMENU      0xF100
#define SC_ARRANGE      0xF110
#define SC_RESTORE      0xF120
#define SC_TASKLIST     0xF130
#define SC_SCREENSAVE   0xF140
#define SC_HOTKEY       0xF150
#define SC_DEFAULT      0xF160
#define SC_MONITORPOWER 0xF170
#define SC_CONTEXTHELP  0xF180
#define SC_SEPARATOR    0xF00F


/*
 * WM_SIZE message wParam values
 */
#define SIZE_RESTORED       0
#define SIZE_MINIMIZED      1
#define SIZE_MAXIMIZED      2
#define SIZE_MAXSHOW        3
#define SIZE_MAXHIDE        4


/*  wParam for WM_SIZING message  */
#define WMSZ_LEFT           1
#define WMSZ_RIGHT          2
#define WMSZ_TOP            3
#define WMSZ_TOPLEFT        4
#define WMSZ_TOPRIGHT       5
#define WMSZ_BOTTOM         6
#define WMSZ_BOTTOMLEFT     7
#define WMSZ_BOTTOMRIGHT    8

/*  ChildWindowFromPointEx  */
#define CWP_ALL             0x0000
#define CWP_SKIPINVISIBLE   0x0001
#define CWP_SKIPDISABLED    0x0002
#define CWP_SKIPTRANSPARENT 0x0004


/* WM_PRINT flags */
#define PRF_CHECKVISIBLE    0x00000001L
#define PRF_NONCLIENT       0x00000002L
#define PRF_CLIENT          0x00000004L
#define PRF_ERASEBKGND      0x00000008L
#define PRF_CHILDREN        0x00000010L
#define PRF_OWNED           0x00000020L

/* 3D border styles */
#define BDR_RAISEDOUTER 0x0001
#define BDR_SUNKENOUTER 0x0002
#define BDR_RAISEDINNER 0x0004
#define BDR_SUNKENINNER 0x0008

#define BDR_OUTER       0x0003
#define BDR_INNER       0x000c

#define EDGE_RAISED     (BDR_RAISEDOUTER | BDR_RAISEDINNER)
#define EDGE_SUNKEN     (BDR_SUNKENOUTER | BDR_SUNKENINNER)
#define EDGE_ETCHED     (BDR_SUNKENOUTER | BDR_RAISEDINNER)
#define EDGE_BUMP       (BDR_RAISEDOUTER | BDR_SUNKENINNER)

/* Border flags */
#define BF_LEFT         0x0001
#define BF_TOP          0x0002
#define BF_RIGHT        0x0004
#define BF_BOTTOM       0x0008

#define BF_TOPLEFT      (BF_TOP | BF_LEFT)
#define BF_TOPRIGHT     (BF_TOP | BF_RIGHT)
#define BF_BOTTOMLEFT   (BF_BOTTOM | BF_LEFT)
#define BF_BOTTOMRIGHT  (BF_BOTTOM | BF_RIGHT)
#define BF_RECT         (BF_LEFT | BF_TOP | BF_RIGHT | BF_BOTTOM)

#define BF_DIAGONAL     0x0010

// For diagonal lines, the BF_RECT flags specify the end point of the
// vector bounded by the rectangle parameter.
#define BF_DIAGONAL_ENDTOPRIGHT     (BF_DIAGONAL | BF_TOP | BF_RIGHT)
#define BF_DIAGONAL_ENDTOPLEFT      (BF_DIAGONAL | BF_TOP | BF_LEFT)
#define BF_DIAGONAL_ENDBOTTOMLEFT   (BF_DIAGONAL | BF_BOTTOM | BF_LEFT)
#define BF_DIAGONAL_ENDBOTTOMRIGHT  (BF_DIAGONAL | BF_BOTTOM | BF_RIGHT)


#define BF_MIDDLE       0x0800  /* Fill in the middle */
#define BF_SOFT         0x1000  /* For softer buttons */
#define BF_ADJUST       0x2000  /* Calculate the space left over */
#define BF_FLAT         0x4000  /* For flat rather than 3D borders */
#define BF_MONO         0x8000  /* For monochrome borders */


/* flags for DrawFrameControl */

#define DFC_CAPTION             1
#define DFC_MENU                2
#define DFC_SCROLL              3
#define DFC_BUTTON              4
#define DFC_POPUPMENU           5

#define DFCS_CAPTIONCLOSE       0x0000
#define DFCS_CAPTIONMIN         0x0001
#define DFCS_CAPTIONMAX         0x0002
#define DFCS_CAPTIONRESTORE     0x0003
#define DFCS_CAPTIONHELP        0x0004

#define DFCS_MENUARROW          0x0000
#define DFCS_MENUCHECK          0x0001
#define DFCS_MENUBULLET         0x0002
#define DFCS_MENUARROWRIGHT     0x0004
#define DFCS_SCROLLUP           0x0000
#define DFCS_SCROLLDOWN         0x0001
#define DFCS_SCROLLLEFT         0x0002
#define DFCS_SCROLLRIGHT        0x0003
#define DFCS_SCROLLCOMBOBOX     0x0005
#define DFCS_SCROLLSIZEGRIP     0x0008
#define DFCS_SCROLLSIZEGRIPRIGHT 0x0010

#define DFCS_BUTTONCHECK        0x0000
#define DFCS_BUTTONRADIOIMAGE   0x0001
#define DFCS_BUTTONRADIOMASK    0x0002
#define DFCS_BUTTONRADIO        0x0004
#define DFCS_BUTTON3STATE       0x0008
#define DFCS_BUTTONPUSH         0x0010

#define DFCS_INACTIVE           0x0100
#define DFCS_PUSHED             0x0200
#define DFCS_CHECKED            0x0400

#define DFCS_TRANSPARENT        0x0800
#define DFCS_HOT                0x1000

#define DFCS_ADJUSTRECT         0x2000
#define DFCS_FLAT               0x4000
#define DFCS_MONO               0x8000


/*
 * DrawText() Format Flags
 */
#define DT_TOP              0x00000000
#define DT_LEFT             0x00000000
#define DT_CENTER           0x00000001
#define DT_RIGHT            0x00000002
#define DT_VCENTER          0x00000004
#define DT_BOTTOM           0x00000008
#define DT_WORDBREAK        0x00000010
#define DT_SINGLELINE       0x00000020
#define DT_EXPANDTABS       0x00000040
#define DT_TABSTOP          0x00000080
#define DT_NOCLIP           0x00000100
#define DT_EXTERNALLEADING  0x00000200
#define DT_CALCRECT         0x00000400
#define DT_NOPREFIX         0x00000800
#define DT_INTERNAL         0x00001000

#define DT_EDITCONTROL      0x00002000
#define DT_PATH_ELLIPSIS    0x00004000
#define DT_END_ELLIPSIS     0x00008000
#define DT_MODIFYSTRING     0x00010000
#define DT_RTLREADING       0x00020000
#define DT_WORD_ELLIPSIS    0x00040000

#define DT_NOFULLWIDTHCHARBREAK     0x00080000
#define DT_HIDEPREFIX               0x00100000
#define DT_PREFIXONLY               0x00200000


/*
 * Dialog Codes
 */
#define DLGC_WANTARROWS     0x0001      /* Control wants arrow keys         */
#define DLGC_WANTTAB        0x0002      /* Control wants tab keys           */
#define DLGC_WANTALLKEYS    0x0004      /* Control wants all keys           */
#define DLGC_WANTMESSAGE    0x0004      /* Pass message to control          */
#define DLGC_HASSETSEL      0x0008      /* Understands EM_SETSEL message    */
#define DLGC_DEFPUSHBUTTON  0x0010      /* Default pushbutton               */
#define DLGC_UNDEFPUSHBUTTON 0x0020     /* Non-default pushbutton           */
#define DLGC_RADIOBUTTON    0x0040      /* Radio button                     */
#define DLGC_WANTCHARS      0x0080      /* Want WM_CHAR messages            */
#define DLGC_STATIC         0x0100      /* Static item: don't include       */
#define DLGC_BUTTON         0x2000      /* Button item: can be checked      */


/*
 * WM_STYLECHANGING, WM_STYLECHANGED
 */
#define GWL_STYLE           (-16)
#define GWL_EXSTYLE         (-20)


/*
 * ShowWindow() Commands
 */
#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_NORMAL           1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE         3
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_SHOWMINNOACTIVE  7
#define SW_SHOWNA           8
#define SW_RESTORE          9
#define SW_SHOWDEFAULT      10
#define SW_FORCEMINIMIZE    11
#define SW_MAX              11


/*
 * WM_MOUSEACTIVATE Return Codes
 */
#define MA_ACTIVATE         1
#define MA_ACTIVATEANDEAT   2
#define MA_NOACTIVATE       3
#define MA_NOACTIVATEANDEAT 4


/*
 * WM_NCHITTEST and MOUSEHOOKSTRUCT Mouse Position Codes
 */
#define HTERROR             (-2)
#define HTTRANSPARENT       (-1)
#define HTNOWHERE           0
#define HTCLIENT            1
#define HTCAPTION           2
#define HTSYSMENU           3
#define HTGROWBOX           4
#define HTSIZE              HTGROWBOX
#define HTMENU              5
#define HTHSCROLL           6
#define HTVSCROLL           7
#define HTMINBUTTON         8
#define HTMAXBUTTON         9
#define HTLEFT              10
#define HTRIGHT             11
#define HTTOP               12
#define HTTOPLEFT           13
#define HTTOPRIGHT          14
#define HTBOTTOM            15
#define HTBOTTOMLEFT        16
#define HTBOTTOMRIGHT       17
#define HTBORDER            18
#define HTREDUCE            HTMINBUTTON
#define HTZOOM              HTMAXBUTTON
#define HTSIZEFIRST         HTLEFT
#define HTSIZELAST          HTBOTTOMRIGHT
#define HTOBJECT            19
#define HTCLOSE             20
#define HTHELP              21



/*
 * Color Types
 */
#define CTLCOLOR_MSGBOX         0
#define CTLCOLOR_EDIT           1
#define CTLCOLOR_LISTBOX        2
#define CTLCOLOR_BTN            3
#define CTLCOLOR_DLG            4
#define CTLCOLOR_SCROLLBAR      5
#define CTLCOLOR_STATIC         6
#define CTLCOLOR_MAX            7

#define COLOR_SCROLLBAR         0
#define COLOR_BACKGROUND        1
#define COLOR_ACTIVECAPTION     2
#define COLOR_INACTIVECAPTION   3
#define COLOR_MENU              4
#define COLOR_WINDOW            5
#define COLOR_WINDOWFRAME       6
#define COLOR_MENUTEXT          7
#define COLOR_WINDOWTEXT        8
#define COLOR_CAPTIONTEXT       9
#define COLOR_ACTIVEBORDER      10
#define COLOR_INACTIVEBORDER    11
#define COLOR_APPWORKSPACE      12
#define COLOR_HIGHLIGHT         13
#define COLOR_HIGHLIGHTTEXT     14
#define COLOR_BTNFACE           15
#define COLOR_BTNSHADOW         16
#define COLOR_GRAYTEXT          17
#define COLOR_BTNTEXT           18
#define COLOR_INACTIVECAPTIONTEXT 19
#define COLOR_BTNHIGHLIGHT      20
#define COLOR_3DDKSHADOW        21
#define COLOR_3DLIGHT           22
#define COLOR_INFOTEXT          23
#define COLOR_INFOBK            24
#define COLOR_HOTLIGHT                  26
#define COLOR_GRADIENTACTIVECAPTION     27
#define COLOR_GRADIENTINACTIVECAPTION   28
#define COLOR_DESKTOP           COLOR_BACKGROUND
#define COLOR_3DFACE            COLOR_BTNFACE
#define COLOR_3DSHADOW          COLOR_BTNSHADOW
#define COLOR_3DHIGHLIGHT       COLOR_BTNHIGHLIGHT
#define COLOR_3DHILIGHT         COLOR_BTNHIGHLIGHT
#define COLOR_BTNHILIGHT        COLOR_BTNHIGHLIGHT
#define COLOR_MENUHILIGHT       29
#define COLOR_MENUBAR           30

/* Gesture IDs */
#   define GID_BEGIN                       1
#   define GID_END                         2
#   define GID_ZOOM                        3
#   define GID_PAN                         4
#   define GID_ROTATE                      5
#   define GID_TWOFINGERTAP                6
#   define GID_PRESSANDTAP                 7
#   define GID_ROLLOVER                    GID_PRESSANDTAP


/*
 * User Button Notification Codes
 */
#define BN_CLICKED          0
#define BN_PAINT            1
#define BN_HILITE           2
#define BN_UNHILITE         3
#define BN_DISABLE          4
#define BN_DOUBLECLICKED    5
#define BN_PUSHED           BN_HILITE
#define BN_UNPUSHED         BN_UNHILITE
#define BN_DBLCLK           BN_DOUBLECLICKED
#define BN_SETFOCUS         6
#define BN_KILLFOCUS        7


/*
 * Button Control Messages
 */
#define BM_GETCHECK        0x00F0
#define BM_SETCHECK        0x00F1
#define BM_GETSTATE        0x00F2
#define BM_SETSTATE        0x00F3
#define BM_SETSTYLE        0x00F4
#if (WINVER >= 0x0400)
#define BM_CLICK           0x00F5
#define BM_GETIMAGE        0x00F6
#define BM_SETIMAGE        0x00F7

#define BST_UNCHECKED      0x0000
#define BST_CHECKED        0x0001
#define BST_INDETERMINATE  0x0002
#define BST_PUSHED         0x0004
#define BST_FOCUS          0x0008
#endif /* WINVER >= 0x0400 */


// drawstage flags
// values under 0x00010000 are reserved for global custom draw values.
// above that are for specific controls
#define CDDS_PREPAINT           0x00000001
#define CDDS_POSTPAINT          0x00000002
#define CDDS_PREERASE           0x00000003
#define CDDS_POSTERASE          0x00000004
// the 0x000010000 bit means it's individual item specific
#define CDDS_ITEM               0x00010000
#define CDDS_ITEMPREPAINT       (CDDS_ITEM | CDDS_PREPAINT)
#define CDDS_ITEMPOSTPAINT      (CDDS_ITEM | CDDS_POSTPAINT)
#define CDDS_ITEMPREERASE       (CDDS_ITEM | CDDS_PREERASE)
#define CDDS_ITEMPOSTERASE      (CDDS_ITEM | CDDS_POSTERASE)
#define CDDS_SUBITEM            0x00020000



// tree view
#define TVS_HASBUTTONS          0x0001
#define TVS_HASLINES            0x0002
#define TVS_LINESATROOT         0x0004
#define TVS_EDITLABELS          0x0008
#define TVS_DISABLEDRAGDROP     0x0010
#define TVS_SHOWSELALWAYS       0x0020
#define TVS_RTLREADING          0x0040
#define TVS_NOTOOLTIPS          0x0080
#define TVS_CHECKBOXES          0x0100
#define TVS_TRACKSELECT         0x0200
#define TVS_SINGLEEXPAND        0x0400
#define TVS_INFOTIP             0x0800
#define TVS_FULLROWSELECT       0x1000
#define TVS_NOSCROLL            0x2000
#define TVS_NONEVENHEIGHT       0x4000

#define TVIS_SELECTED           0x0002
#define TVIS_CUT                0x0004
#define TVIS_DROPHILITED        0x0008
#define TVIS_BOLD               0x0010
#define TVIS_EXPANDED           0x0020
#define TVIS_EXPANDEDONCE       0x0040
#define TVIS_EXPANDPARTIAL      0x0080
#define TVIS_OVERLAYMASK        0x0F00
#define TVIS_STATEIMAGEMASK     0xF000
#define TVIS_USERMASK           0xF000

#define TVE_COLLAPSE            0x0001
#define TVE_EXPAND              0x0002
#define TVE_TOGGLE              0x0003
#define TVE_EXPANDPARTIAL       0x4000
#define TVE_COLLAPSERESET       0x8000

#define TVGN_ROOT               0x0000
#define TVGN_NEXT               0x0001
#define TVGN_PREVIOUS           0x0002
#define TVGN_PARENT             0x0003
#define TVGN_CHILD              0x0004
#define TVGN_FIRSTVISIBLE       0x0005
#define TVGN_NEXTVISIBLE        0x0006
#define TVGN_PREVIOUSVISIBLE    0x0007
#define TVGN_DROPHILITE         0x0008
#define TVGN_CARET              0x0009
#define TVGN_LASTVISIBLE        0x000A


#define TVHT_NOWHERE            0x0001
#define TVHT_ONITEMICON         0x0002
#define TVHT_ONITEMLABEL        0x0004
#define TVHT_ONITEM             (TVHT_ONITEMICON | TVHT_ONITEMLABEL | TVHT_ONITEMSTATEICON)
#define TVHT_ONITEMINDENT       0x0008
#define TVHT_ONITEMBUTTON       0x0010
#define TVHT_ONITEMRIGHT        0x0020
#define TVHT_ONITEMSTATEICON    0x0040
#define TVHT_ABOVE              0x0100
#define TVHT_BELOW              0x0200
#define TVHT_TORIGHT            0x0400
#define TVHT_TOLEFT             0x0800


// #define TV_FIRST                0x1100      // TreeView messages
// #define TVM_INSERTITEM          (TV_FIRST + 0)
// #define TVM_DELETEITEM          (TV_FIRST + 1)
// #define TVM_EXPAND              (TV_FIRST + 2)
// #define TVM_GETITEMRECT         (TV_FIRST + 4)
// #define TVM_GETCOUNT            (TV_FIRST + 5)
// #define TVM_GETINDENT           (TV_FIRST + 6)
// #define TVM_SETINDENT           (TV_FIRST + 7)
// #define TVM_GETIMAGELIST        (TV_FIRST + 8)
// #define TVM_SETIMAGELIST        (TV_FIRST + 9)
// #define TVM_GETNEXTITEM         (TV_FIRST + 10)
// #define TVM_SELECTITEM          (TV_FIRST + 11)
// #define TVM_GETITEM             (TV_FIRST + 12)
// #define TVM_SETITEM             (TV_FIRST + 13)
// #define TVM_EDITLABEL           (TV_FIRST + 14)
// #define TVM_GETEDITCONTROL      (TV_FIRST + 15)
// #define TVM_GETVISIBLECOUNT     (TV_FIRST + 16)
// #define TVM_HITTEST             (TV_FIRST + 17)
// #define TVM_CREATEDRAGIMAGE     (TV_FIRST + 18)
// #define TVM_SORTCHILDREN        (TV_FIRST + 19)
// #define TVM_ENSUREVISIBLE       (TV_FIRST + 20)
// #define TVM_SORTCHILDRENCB      (TV_FIRST + 21)
// #define TVM_ENDEDITLABELNOW     (TV_FIRST + 22)
// #define TVM_GETISEARCHSTRING    (TV_FIRST + 23)
// #define TVM_SETTOOLTIPS         (TV_FIRST + 24)
// #define TVM_GETTOOLTIPS         (TV_FIRST + 25)
// #define TVM_SETINSERTMARK       (TV_FIRST + 26)
// #define TVM_SETITEMHEIGHT       (TV_FIRST + 27)
// #define TVM_GETITEMHEIGHT       (TV_FIRST + 28)
// #define TVM_SETBKCOLOR          (TV_FIRST + 29)
// #define TVM_SETTEXTCOLOR        (TV_FIRST + 30)
// #define TVM_GETBKCOLOR          (TV_FIRST + 31)
// #define TVM_GETTEXTCOLOR        (TV_FIRST + 32)
// #define TVM_SETSCROLLTIME       (TV_FIRST + 33)
// #define TVM_GETSCROLLTIME       (TV_FIRST + 34)
// #define TVM_SETINSERTMARKCOLOR  (TV_FIRST + 37)
// #define TVM_GETINSERTMARKCOLOR  (TV_FIRST + 38)

// 
// // list box
// /*
//  * Listbox Styles
//  */
// #define LBS_NOTIFY            0x0001L
// #define LBS_SORT              0x0002L
// #define LBS_NOREDRAW          0x0004L
// #define LBS_MULTIPLESEL       0x0008L
// #define LBS_OWNERDRAWFIXED    0x0010L
// #define LBS_OWNERDRAWVARIABLE 0x0020L
// #define LBS_HASSTRINGS        0x0040L
// #define LBS_USETABSTOPS       0x0080L
// #define LBS_NOINTEGRALHEIGHT  0x0100L
// #define LBS_MULTICOLUMN       0x0200L
// #define LBS_WANTKEYBOARDINPUT 0x0400L
// #define LBS_EXTENDEDSEL       0x0800L
// #define LBS_DISABLENOSCROLL   0x1000L
// #define LBS_NODATA            0x2000L
// #define LBS_NOSEL             0x4000L
// #define LBS_STANDARD          (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)

// 
// /*
//  * Listbox messages
//  */
// #define LB_ADDSTRING            0x0180
// #define LB_INSERTSTRING         0x0181
// #define LB_DELETESTRING         0x0182
// #define LB_SELITEMRANGEEX       0x0183
// #define LB_RESETCONTENT         0x0184
// #define LB_SETSEL               0x0185
// #define LB_SETCURSEL            0x0186
// #define LB_GETSEL               0x0187
// #define LB_GETCURSEL            0x0188
// #define LB_GETTEXT              0x0189
// #define LB_GETTEXTLEN           0x018A
// #define LB_GETCOUNT             0x018B
// #define LB_SELECTSTRING         0x018C
// #define LB_DIR                  0x018D
// #define LB_GETTOPINDEX          0x018E
// #define LB_FINDSTRING           0x018F
// #define LB_GETSELCOUNT          0x0190
// #define LB_GETSELITEMS          0x0191
// #define LB_SETTABSTOPS          0x0192
// #define LB_GETHORIZONTALEXTENT  0x0193
// #define LB_SETHORIZONTALEXTENT  0x0194
// #define LB_SETCOLUMNWIDTH       0x0195
// #define LB_ADDFILE              0x0196
// #define LB_SETTOPINDEX          0x0197
// #define LB_GETITEMRECT          0x0198
// #define LB_GETITEMDATA          0x0199
// #define LB_SETITEMDATA          0x019A
// #define LB_SELITEMRANGE         0x019B
// #define LB_SETANCHORINDEX       0x019C
// #define LB_GETANCHORINDEX       0x019D
// #define LB_SETCARETINDEX        0x019E
// #define LB_GETCARETINDEX        0x019F
// #define LB_SETITEMHEIGHT        0x01A0
// #define LB_GETITEMHEIGHT        0x01A1
// #define LB_FINDSTRINGEXACT      0x01A2
// #define LB_SETLOCALE            0x01A5
// #define LB_GETLOCALE            0x01A6
// #define LB_SETCOUNT             0x01A7
// #define LB_INITSTORAGE          0x01A8
// #define LB_ITEMFROMPOINT        0x01A9
// #define LB_MSGMAX               0x01B0
// 
// /*
//  * Listbox Notification Codes
//  */
// #define LBN_ERRSPACE        (-2)
// #define LBN_SELCHANGE       1
// #define LBN_DBLCLK          2
// #define LBN_SELCANCEL       3
// #define LBN_SETFOCUS        4
// #define LBN_KILLFOCUS       5
// 
// /*
//  * Listbox Return Values
//  */
// #define LB_OKAY             0
// #define LB_ERR              (-1)
// #define LB_ERRSPACE         (-2)


//====== PROGRESS CONTROL =====================================================

#ifndef NOPROGRESS

#define PBS_SMOOTH              0x01
#define PBS_VERTICAL            0x04

#define PBM_SETRANGE            (WM_USER+1)
#define PBM_SETPOS              (WM_USER+2)
#define PBM_DELTAPOS            (WM_USER+3)
#define PBM_SETSTEP             (WM_USER+4)
#define PBM_STEPIT              (WM_USER+5)
#define PBM_SETRANGE32          (WM_USER+6)  // lParam = high, wParam = low

#define PBM_GETRANGE            (WM_USER+7)  // wParam = return (TRUE ? low : high). lParam = PPBRANGE or NULL
#define PBM_GETPOS              (WM_USER+8)
#define PBM_SETBARCOLOR         (WM_USER+9)        // lParam = bar color
#define PBM_SETBKCOLOR          CCM_SETBKCOLOR  // lParam = bkColor

#endif  // NOPROGRESS


//====== HEADER CONTROL =======================================================

// #ifndef NOHEADER
// 
// #ifdef _WIN32
// #define WC_HEADERA              "SysHeader32"
// #define WC_HEADERW              L"SysHeader32"
// 
// #ifdef UNICODE
// #define WC_HEADER               WC_HEADERW
// #else
// #define WC_HEADER               WC_HEADERA
// #endif
// 
// #else
// #define WC_HEADER               "SysHeader"
// #endif
// 
// // begin_r_commctrl
// 
// #define HDS_HORZ                0x0000
// #define HDS_BUTTONS             0x0002
// #define HDS_HOTTRACK            0x0004
// #define HDS_HIDDEN              0x0008
// 
// #define HDS_DRAGDROP            0x0040
// #define HDS_FULLDRAG            0x0080
// #define HDS_FILTERBAR           0x0100
// 
// #ifndef HDS_FLAT
// #   define HDS_FLAT             0x0200
// #endif
// 
// #ifndef HDS_CHECKBOXES
// #   define HDS_CHECKBOXES       0x0400
// #   define HDS_NOSIZING         0x0800
// #   define HDS_OVERFLOW         0x1000
// #endif
// 
// // end_r_commctrl
// 
// #if (_WIN32_IE >= 0x0300)
// #define HD_ITEMA HDITEMA
// #define HD_ITEMW HDITEMW
// #else
// #define HDITEMW  HD_ITEMW
// #define HDITEMA  HD_ITEMA
// #endif
// #define HD_ITEM HDITEM
// 
// struct HDITEM
// {
//     TMUInt      mask;
//     int         cxy;
//     TMCStr    pszText;
//     void*     hbm;
//     int         cchTextMax;
//     int         fmt;
//     TMLParam    lParam;
//     int     iImage;        // index of bitmap in ImageList
//     int     iOrder;        // where to draw this item
// };
// 
// 
// #define HDI_WIDTH               0x0001
// #define HDI_HEIGHT              HDI_WIDTH
// #define HDI_TEXT                0x0002
// #define HDI_FORMAT              0x0004
// #define HDI_LPARAM              0x0008
// #define HDI_BITMAP              0x0010
// #define HDI_IMAGE               0x0020
// #define HDI_DI_SETITEM          0x0040
// #define HDI_ORDER               0x0080
// 
// #define HDF_LEFT                0x0000 // Same as LVCFMT_LEFT
// #define HDF_RIGHT               0x0001 // Same as LVCFMT_RIGHT
// #define HDF_CENTER              0x0002 // Same as LVCFMT_CENTER
// #define HDF_JUSTIFYMASK         0x0003 // Same as LVCFMT_JUSTIFYMASK
// #define HDF_RTLREADING          0x0004 // Same as LVCFMT_LEFT
// 
// #define HDF_BITMAP              0x2000
// #define HDF_STRING              0x4000
// #define HDF_OWNERDRAW           0x8000 // Same as LVCFMT_COL_HAS_IMAGES
// #define HDF_IMAGE               0x0800 // Same as LVCFMT_IMAGE
// #define HDF_BITMAP_ON_RIGHT     0x1000 // Same as LVCFMT_BITMAP_ON_RIGHT
// 
// #if (NTDDI_VERSION >= NTDDI_WINXP)
// #define HDF_SORTUP              0x0400
// #define HDF_SORTDOWN            0x0200
// #endif
// 
// #if (NTDDI_VERSION >= NTDDI_VISTA)
// #define HDF_CHECKBOX            0x0040
// #define HDF_CHECKED             0x0080
// #define HDF_FIXEDWIDTH          0x0100 // Can't resize the column; same as LVCFMT_FIXED_WIDTH
// #define HDF_SPLITBUTTON      0x1000000 // Column is a split button; same as LVCFMT_SPLITBUTTON
// #endif
// 
// #define HDM_FIRST               0x1200      // Header messages
// 
// #define HDM_GETITEMCOUNT        (HDM_FIRST + 0)
// 
// #define HDM_INSERTITEMA         (HDM_FIRST + 1)
// #define HDM_INSERTITEMW         (HDM_FIRST + 10)
// 
// #ifdef UNICODE
// #define HDM_INSERTITEM          HDM_INSERTITEMW
// #else
// #define HDM_INSERTITEM          HDM_INSERTITEMA
// #endif
// 
// 
// #define HDM_GETITEMA            (HDM_FIRST + 3)
// #define HDM_GETITEMW            (HDM_FIRST + 11)
// 
// #ifdef UNICODE
// #define HDM_GETITEM             HDM_GETITEMW
// #else
// #define HDM_GETITEM             HDM_GETITEMA
// #endif
// 
// #define HDM_SETITEMA            (HDM_FIRST + 4)
// #define HDM_SETITEMW            (HDM_FIRST + 12)
// 
// #ifdef UNICODE
// #define HDM_SETITEM             HDM_SETITEMW
// #else
// #define HDM_SETITEM             HDM_SETITEMA
// #endif
// 
// // #if (_WIN32_IE >= 0x0300)
// // #define HD_LAYOUT  HDLAYOUT
// // #else
// // #define HDLAYOUT   HD_LAYOUT
// // #endif
// // 
// // typedef struct _HD_LAYOUT
// // {
// //     RECT FAR* prc;
// //     WINDOWPOS FAR* pwpos;
// // } HDLAYOUT, FAR *LPHDLAYOUT;
// // 
// // 
// // #define HDM_LAYOUT              (HDM_FIRST + 5)
// // #define Header_Layout(hwndHD, playout) \
// //     (BOOL)SNDMSG((hwndHD), HDM_LAYOUT, 0, (LPARAM)(HD_LAYOUT FAR*)(playout))
// 
// 
// #define HHT_NOWHERE             0x0001
// #define HHT_ONHEADER            0x0002
// #define HHT_ONDIVIDER           0x0004
// #define HHT_ONDIVOPEN           0x0008
// #define HHT_ABOVE               0x0100
// #define HHT_BELOW               0x0200
// #define HHT_TORIGHT             0x0400
// #define HHT_TOLEFT              0x0800
// 
// // #if (_WIN32_IE >= 0x0300)
// // #define HD_HITTESTINFO HDHITTESTINFO
// // #else
// // #define HDHITTESTINFO  HD_HITTESTINFO
// // #endif
// // 
// // typedef struct _HD_HITTESTINFO
// // {
// //     POINT pt;
// //     UINT flags;
// //     int iItem;
// // } HDHITTESTINFO, FAR *LPHDHITTESTINFO;
// // 
// // 
// // #define HDM_HITTEST             (HDM_FIRST + 6)
// // 
// // #if (_WIN32_IE >= 0x0300)
// // 
// // #define HDM_GETITEMRECT         (HDM_FIRST + 7)
// // #define Header_GetItemRect(hwnd, iItem, lprc) \
// //         (BOOL)SNDMSG((hwnd), HDM_GETITEMRECT, (WPARAM)iItem, (LPARAM)lprc)
// // 
// // #define HDM_SETIMAGELIST        (HDM_FIRST + 8)
// // #define Header_SetImageList(hwnd, himl) \
// //         (HIMAGELIST)SNDMSG((hwnd), HDM_SETIMAGELIST, 0, (LPARAM)himl)
// // 
// // #define HDM_GETIMAGELIST        (HDM_FIRST + 9)
// // #define Header_GetImageList(hwnd) \
// //         (HIMAGELIST)SNDMSG((hwnd), HDM_GETIMAGELIST, 0, 0)
// // 
// // 
// // #define HDM_ORDERTOINDEX        (HDM_FIRST + 15)
// // #define Header_OrderToIndex(hwnd, i) \
// //         (int)SNDMSG((hwnd), HDM_ORDERTOINDEX, (WPARAM)i, 0)
// // 
// // #define HDM_CREATEDRAGIMAGE     (HDM_FIRST + 16)  // wparam = which item (by index)
// // #define Header_CreateDragImage(hwnd, i) \
// //         (HIMAGELIST)SNDMSG((hwnd), HDM_CREATEDRAGIMAGE, (WPARAM)i, 0)
// // 
// // #define HDM_GETORDERARRAY       (HDM_FIRST + 17)
// // #define Header_GetOrderArray(hwnd, iCount, lpi) \
// //         (BOOL)SNDMSG((hwnd), HDM_GETORDERARRAY, (WPARAM)iCount, (LPARAM)lpi)
// // 
// // #define HDM_SETORDERARRAY       (HDM_FIRST + 18)
// // #define Header_SetOrderArray(hwnd, iCount, lpi) \
// //         (BOOL)SNDMSG((hwnd), HDM_SETORDERARRAY, (WPARAM)iCount, (LPARAM)lpi)
// // // lparam = int array of size HDM_GETITEMCOUNT
// // // the array specifies the order that all items should be displayed.
// // // e.g.  { 2, 0, 1}
// // // says the index 2 item should be shown in the 0ths position
// // //      index 0 should be shown in the 1st position
// // //      index 1 should be shown in the 2nd position
// // 
// // 
// // #define HDM_SETHOTDIVIDER          (HDM_FIRST + 19)
// // #define Header_SetHotDivider(hwnd, fPos, dw) \
// //         (int)SNDMSG((hwnd), HDM_SETHOTDIVIDER, (WPARAM)fPos, (LPARAM)dw)
// // // convenience message for external dragdrop
// // // wParam = BOOL  specifying whether the lParam is a dwPos of the cursor
// // //              position or the index of which divider to hotlight
// // // lParam = depends on wParam  (-1 and wParm = FALSE turns off hotlight)
// // #endif      // _WIN32_IE >= 0x0300
// // 
// // #if (_WIN32_IE >= 0x0400)
// // #define HDM_SETUNICODEFORMAT   CCM_SETUNICODEFORMAT
// // #define Header_SetUnicodeFormat(hwnd, fUnicode)  \
// //     (BOOL)SNDMSG((hwnd), HDM_SETUNICODEFORMAT, (WPARAM)(fUnicode), 0)
// // 
// // #define HDM_GETUNICODEFORMAT   CCM_GETUNICODEFORMAT
// // #define Header_GetUnicodeFormat(hwnd)  \
// //     (BOOL)SNDMSG((hwnd), HDM_GETUNICODEFORMAT, 0, 0)
// // #endif
// // 
// // #define HDN_ITEMCHANGINGA       (HDN_FIRST-0)
// // #define HDN_ITEMCHANGINGW       (HDN_FIRST-20)
// // #define HDN_ITEMCHANGEDA        (HDN_FIRST-1)
// // #define HDN_ITEMCHANGEDW        (HDN_FIRST-21)
// // #define HDN_ITEMCLICKA          (HDN_FIRST-2)
// // #define HDN_ITEMCLICKW          (HDN_FIRST-22)
// // #define HDN_ITEMDBLCLICKA       (HDN_FIRST-3)
// // #define HDN_ITEMDBLCLICKW       (HDN_FIRST-23)
// // #define HDN_DIVIDERDBLCLICKA    (HDN_FIRST-5)
// // #define HDN_DIVIDERDBLCLICKW    (HDN_FIRST-25)
// // #define HDN_BEGINTRACKA         (HDN_FIRST-6)
// // #define HDN_BEGINTRACKW         (HDN_FIRST-26)
// // #define HDN_ENDTRACKA           (HDN_FIRST-7)
// // #define HDN_ENDTRACKW           (HDN_FIRST-27)
// // #define HDN_TRACKA              (HDN_FIRST-8)
// // #define HDN_TRACKW              (HDN_FIRST-28)
// // #if (_WIN32_IE >= 0x0300)
// // #define HDN_GETDISPINFOA        (HDN_FIRST-9)
// // #define HDN_GETDISPINFOW        (HDN_FIRST-29)
// // #define HDN_BEGINDRAG           (HDN_FIRST-10)
// // #define HDN_ENDDRAG             (HDN_FIRST-11)
// // #endif
// // 
// // #ifdef UNICODE
// // #define HDN_ITEMCHANGING         HDN_ITEMCHANGINGW
// // #define HDN_ITEMCHANGED          HDN_ITEMCHANGEDW
// // #define HDN_ITEMCLICK            HDN_ITEMCLICKW
// // #define HDN_ITEMDBLCLICK         HDN_ITEMDBLCLICKW
// // #define HDN_DIVIDERDBLCLICK      HDN_DIVIDERDBLCLICKW
// // #define HDN_BEGINTRACK           HDN_BEGINTRACKW
// // #define HDN_ENDTRACK             HDN_ENDTRACKW
// // #define HDN_TRACK                HDN_TRACKW
// // #if (_WIN32_IE >= 0x0300)
// // #define HDN_GETDISPINFO          HDN_GETDISPINFOW
// // #endif
// // #else
// // #define HDN_ITEMCHANGING         HDN_ITEMCHANGINGA
// // #define HDN_ITEMCHANGED          HDN_ITEMCHANGEDA
// // #define HDN_ITEMCLICK            HDN_ITEMCLICKA
// // #define HDN_ITEMDBLCLICK         HDN_ITEMDBLCLICKA
// // #define HDN_DIVIDERDBLCLICK      HDN_DIVIDERDBLCLICKA
// // #define HDN_BEGINTRACK           HDN_BEGINTRACKA
// // #define HDN_ENDTRACK             HDN_ENDTRACKA
// // #define HDN_TRACK                HDN_TRACKA
// // #if (_WIN32_IE >= 0x0300)
// // #define HDN_GETDISPINFO          HDN_GETDISPINFOA
// // #endif
// // #endif
// // 
// // 
// // 
// // #if (_WIN32_IE >= 0x0300)
// // #define HD_NOTIFYA              NMHEADERA
// // #define HD_NOTIFYW              NMHEADERW
// // #else
// // #define tagNMHEADERA            _HD_NOTIFY
// // #define NMHEADERA               HD_NOTIFYA
// // #define tagHMHEADERW            _HD_NOTIFYW
// // #define NMHEADERW               HD_NOTIFYW
// // #endif
// // #define HD_NOTIFY               NMHEADER
// // 
// // typedef struct tagNMHEADERA
// // {
// //     NMHDR   hdr;
// //     int     iItem;
// //     int     iButton;
// //     HDITEMA FAR* pitem;
// // }  NMHEADERA, FAR* LPNMHEADERA;
// // 
// // 
// // typedef struct tagNMHEADERW
// // {
// //     NMHDR   hdr;
// //     int     iItem;
// //     int     iButton;
// //     HDITEMW FAR* pitem;
// // } NMHEADERW, FAR* LPNMHEADERW;
// // 
// // #ifdef UNICODE
// // #define NMHEADER                NMHEADERW
// // #define LPNMHEADER              LPNMHEADERW
// // #else
// // #define NMHEADER                NMHEADERA
// // #define LPNMHEADER              LPNMHEADERA
// // #endif
// // 
// // typedef struct tagNMHDDISPINFOW
// // {
// //     NMHDR   hdr;
// //     int     iItem;
// //     UINT    mask;
// //     LPWSTR  pszText;
// //     int     cchTextMax;
// //     int     iImage;
// //     LPARAM  lParam;
// // } NMHDDISPINFOW, FAR* LPNMHDDISPINFOW;
// // 
// // typedef struct tagNMHDDISPINFOA
// // {
// //     NMHDR   hdr;
// //     int     iItem;
// //     UINT    mask;
// //     LPSTR   pszText;
// //     int     cchTextMax;
// //     int     iImage;
// //     LPARAM  lParam;
// // } NMHDDISPINFOA, FAR* LPNMHDDISPINFOA;
// // 
// // 
// // #ifdef UNICODE
// // #define NMHDDISPINFO            NMHDDISPINFOW
// // #define LPNMHDDISPINFO          LPNMHDDISPINFOW
// // #else
// // #define NMHDDISPINFO            NMHDDISPINFOA
// // #define LPNMHDDISPINFO          LPNMHDDISPINFOA
// // #endif
// 
// #endif      // NOHEADER


/*
 * Combo Box messages
 */
#ifndef NOWINMESSAGES
#define CB_GETEDITSEL               0x0140
#define CB_LIMITTEXT                0x0141
#define CB_SETEDITSEL               0x0142
#define CB_ADDSTRING                0x0143
#define CB_DELETESTRING             0x0144
#define CB_DIR                      0x0145
#define CB_GETCOUNT                 0x0146
#define CB_GETCURSEL                0x0147
#define CB_GETLBTEXT                0x0148
#define CB_GETLBTEXTLEN             0x0149
#define CB_INSERTSTRING             0x014A
#define CB_RESETCONTENT             0x014B
#define CB_FINDSTRING               0x014C
#define CB_SELECTSTRING             0x014D
#define CB_SETCURSEL                0x014E
#define CB_SHOWDROPDOWN             0x014F
#define CB_GETITEMDATA              0x0150
#define CB_SETITEMDATA              0x0151
#define CB_GETDROPPEDCONTROLRECT    0x0152
#define CB_SETITEMHEIGHT            0x0153
#define CB_GETITEMHEIGHT            0x0154
#define CB_SETEXTENDEDUI            0x0155
#define CB_GETEXTENDEDUI            0x0156
#define CB_GETDROPPEDSTATE          0x0157
#define CB_FINDSTRINGEXACT          0x0158
#define CB_SETLOCALE                0x0159
#define CB_GETLOCALE                0x015A
#if (WINVER >= 0x0400)
#define CB_GETTOPINDEX              0x015b
#define CB_SETTOPINDEX              0x015c
#define CB_GETHORIZONTALEXTENT      0x015d
#define CB_SETHORIZONTALEXTENT      0x015e
#define CB_GETDROPPEDWIDTH          0x015f
#define CB_SETDROPPEDWIDTH          0x0160
#define CB_INITSTORAGE              0x0161
#if (_WIN32_WCE >= 0x0400)
#define CB_MULTIPLEADDSTRING        0x0163
#endif
#endif /* WINVER >= 0x0400 */

#if (_WIN32_WINNT >= 0x0501)
#define CB_GETCOMBOBOXINFO          0x0164
#endif /* _WIN32_WINNT >= 0x0501 */

#if (_WIN32_WINNT >= 0x0501)
#define CB_MSGMAX                   0x0165
#elif (_WIN32_WCE >= 0x0400)
#define CB_MSGMAX                   0x0163
#elif (WINVER >= 0x0400)
#define CB_MSGMAX                   0x0162
#else
#define CB_MSGMAX                   0x015B
#endif
#endif  /* !NOWINMESSAGES */



//====== ANIMATE CONTROL ======================================================

#ifndef NOANIMATE

// #define ANIMATE_CLASSW          L"SysAnimate32"
// #define ANIMATE_CLASSA          "SysAnimate32"
// 
// #ifdef UNICODE
// #define ANIMATE_CLASS           ANIMATE_CLASSW
// #else
// #define ANIMATE_CLASS           ANIMATE_CLASSA
// #endif

// begin_r_commctrl

#define ACS_CENTER              0x0001
#define ACS_TRANSPARENT         0x0002
#define ACS_AUTOPLAY            0x0004
#if (_WIN32_IE >= 0x0300)
#define ACS_TIMER               0x0008  // don't use threads... use timers
#endif

// end_r_commctrl

#define ACM_OPENA               (WM_USER+100)
#define ACM_OPENW               (WM_USER+103)

#ifdef UNICODE
#define ACM_OPEN                ACM_OPENW
#else
#define ACM_OPEN                ACM_OPENA
#endif

#define ACM_PLAY                (WM_USER+101)
#define ACM_STOP                (WM_USER+102)


#define ACN_START               1
#define ACN_STOP                2


// #define Animate_Create(hwndP, id, dwStyle, hInstance)   \
//     CreateWindow(ANIMATE_CLASS, NULL,           \
// dwStyle, 0, 0, 0, 0, hwndP, (HMENU)(id), hInstance, NULL)
// 
// #define Animate_Open(hwnd, szName)          (BOOL)SNDMSG(hwnd, ACM_OPEN, 0, (LPARAM)(LPTSTR)(szName))
// #define Animate_OpenEx(hwnd, hInst, szName) (BOOL)SNDMSG(hwnd, ACM_OPEN, (WPARAM)hInst, (LPARAM)(LPTSTR)(szName))
// #define Animate_Play(hwnd, from, to, rep)   (BOOL)SNDMSG(hwnd, ACM_PLAY, (WPARAM)(UINT)(rep), (LPARAM)MAKELONG(from, to))
// #define Animate_Stop(hwnd)                  (BOOL)SNDMSG(hwnd, ACM_STOP, 0, 0)
// #define Animate_Close(hwnd)                 Animate_Open(hwnd, NULL)
// #define Animate_Seek(hwnd, frame)           Animate_Play(hwnd, frame, frame, 1)

#endif      // NOANIMATE


//====== TOOLTIPS CONTROL =====================================================

#ifndef NOTOOLTIPS


#define TTTOOLINFO_V1_SIZE  sizeof(TMTOOLINFO) //     CCSIZEOF_STRUCT(TTTOOLINFOA, lpszText)


#define TTS_ALWAYSTIP           0x01
#define TTS_NOPREFIX            0x02

#define TTF_IDISHWND            0x0001

// Use this to center around trackpoint in trackmode
// -OR- to center around tool in normal mode.
// Use TTF_ABSOLUTE to place the tip exactly at the track coords when
// in tracking mode.  TTF_ABSOLUTE can be used in conjunction with TTF_CENTERTIP
// to center the tip absolutely about the track point.

#define TTF_CENTERTIP           0x0002
#define TTF_RTLREADING          0x0004
#define TTF_SUBCLASS            0x0010
#define TTF_TRACK               0x0020
#define TTF_ABSOLUTE            0x0080
#define TTF_TRANSPARENT         0x0100
#define TTF_DI_SETITEM          0x8000       // valid only on the TTN_NEEDTEXT callback

#define TTDT_AUTOMATIC          0
#define TTDT_RESHOW             1
#define TTDT_AUTOPOP            2
#define TTDT_INITIAL            3

#define TTM_ACTIVATE            (WM_USER + 1)
#define TTM_SETDELAYTIME        (WM_USER + 3)
#define TTM_ADDTOOLA            (WM_USER + 4)
#define TTM_ADDTOOLW            (WM_USER + 50)
#define TTM_DELTOOLA            (WM_USER + 5)
#define TTM_DELTOOLW            (WM_USER + 51)
#define TTM_NEWTOOLRECTA        (WM_USER + 6)
#define TTM_NEWTOOLRECTW        (WM_USER + 52)
#define TTM_RELAYEVENT          (WM_USER + 7)

#define TTM_GETTOOLINFOA        (WM_USER + 8)
#define TTM_GETTOOLINFOW        (WM_USER + 53)

#define TTM_SETTOOLINFOA        (WM_USER + 9)
#define TTM_SETTOOLINFOW        (WM_USER + 54)

#define TTM_HITTESTA            (WM_USER +10)
#define TTM_HITTESTW            (WM_USER +55)
#define TTM_GETTEXTA            (WM_USER +11)
#define TTM_GETTEXTW            (WM_USER +56)
#define TTM_UPDATETIPTEXTA      (WM_USER +12)
#define TTM_UPDATETIPTEXTW      (WM_USER +57)
#define TTM_GETTOOLCOUNT        (WM_USER +13)
#define TTM_ENUMTOOLSA          (WM_USER +14)
#define TTM_ENUMTOOLSW          (WM_USER +58)
#define TTM_GETCURRENTTOOLA     (WM_USER + 15)
#define TTM_GETCURRENTTOOLW     (WM_USER + 59)
#define TTM_WINDOWFROMPOINT     (WM_USER + 16)
#define TTM_TRACKACTIVATE       (WM_USER + 17)  // wParam = TRUE/FALSE start end  lparam = LPTOOLINFO
#define TTM_TRACKPOSITION       (WM_USER + 18)  // lParam = dwPos
#define TTM_SETTIPBKCOLOR       (WM_USER + 19)
#define TTM_SETTIPTEXTCOLOR     (WM_USER + 20)
#define TTM_GETDELAYTIME        (WM_USER + 21)
#define TTM_GETTIPBKCOLOR       (WM_USER + 22)
#define TTM_GETTIPTEXTCOLOR     (WM_USER + 23)
#define TTM_SETMAXTIPWIDTH      (WM_USER + 24)
#define TTM_GETMAXTIPWIDTH      (WM_USER + 25)
#define TTM_SETMARGIN           (WM_USER + 26)  // lParam = lprc
#define TTM_GETMARGIN           (WM_USER + 27)  // lParam = lprc
#define TTM_POP                 (WM_USER + 28)
#define TTM_UPDATE              (WM_USER + 29)


#ifdef UNICODE
#define TTM_ADDTOOL             TTM_ADDTOOLW
#define TTM_DELTOOL             TTM_DELTOOLW
#define TTM_NEWTOOLRECT         TTM_NEWTOOLRECTW
#define TTM_GETTOOLINFO         TTM_GETTOOLINFOW
#define TTM_SETTOOLINFO         TTM_SETTOOLINFOW
#define TTM_HITTEST             TTM_HITTESTW
#define TTM_GETTEXT             TTM_GETTEXTW
#define TTM_UPDATETIPTEXT       TTM_UPDATETIPTEXTW
#define TTM_ENUMTOOLS           TTM_ENUMTOOLSW
#define TTM_GETCURRENTTOOL      TTM_GETCURRENTTOOLW
#else
#define TTM_ADDTOOL             TTM_ADDTOOLA
#define TTM_DELTOOL             TTM_DELTOOLA
#define TTM_NEWTOOLRECT         TTM_NEWTOOLRECTA
#define TTM_GETTOOLINFO         TTM_GETTOOLINFOA
#define TTM_SETTOOLINFO         TTM_SETTOOLINFOA
#define TTM_HITTEST             TTM_HITTESTA
#define TTM_GETTEXT             TTM_GETTEXTA
#define TTM_UPDATETIPTEXT       TTM_UPDATETIPTEXTA
#define TTM_ENUMTOOLS           TTM_ENUMTOOLSA
#define TTM_GETCURRENTTOOL      TTM_GETCURRENTTOOLA
#endif


#endif      // NOTOOLTIPS


// begin_r_commctrl

#define UDS_WRAP                0x0001
#define UDS_SETBUDDYINT         0x0002
#define UDS_ALIGNRIGHT          0x0004
#define UDS_ALIGNLEFT           0x0008
#define UDS_AUTOBUDDY           0x0010
#define UDS_ARROWKEYS           0x0020
#define UDS_HORZ                0x0040
#define UDS_NOTHOUSANDS         0x0080
#if (_WIN32_IE >= 0x0300)
#define UDS_HOTTRACK            0x0100
#endif
// end_r_commctrl

/*
 * Edit Control Styles
 */
#define ES_LEFT             0x0000L
#define ES_CENTER           0x0001L
#define ES_RIGHT            0x0002L
#define ES_MULTILINE        0x0004L
#define ES_UPPERCASE        0x0008L
#define ES_LOWERCASE        0x0010L
#define ES_PASSWORD         0x0020L
#define ES_AUTOVSCROLL      0x0040L
#define ES_AUTOHSCROLL      0x0080L
#define ES_NOHIDESEL        0x0100L
#define ES_OEMCONVERT       0x0400L
#define ES_READONLY         0x0800L
#define ES_WANTRETURN       0x1000L
#define ES_NUMBER           0x2000L

/*
 * Edit Control Messages
 */
#define EM_GETSEL               0x00B0
#define EM_SETSEL               0x00B1
#define EM_GETRECT              0x00B2
#define EM_SETRECT              0x00B3
#define EM_SETRECTNP            0x00B4
#define EM_SCROLL               0x00B5
#define EM_LINESCROLL           0x00B6
#define EM_SCROLLCARET          0x00B7
#define EM_GETMODIFY            0x00B8
#define EM_SETMODIFY            0x00B9
#define EM_GETLINECOUNT         0x00BA
#define EM_LINEINDEX            0x00BB
#define EM_SETHANDLE            0x00BC
#define EM_GETHANDLE            0x00BD
#define EM_GETTHUMB             0x00BE
#define EM_LINELENGTH           0x00C1
#define EM_REPLACESEL           0x00C2
#define EM_GETLINE              0x00C4
#define EM_LIMITTEXT            0x00C5
#define EM_CANUNDO              0x00C6
#define EM_UNDO                 0x00C7
#define EM_FMTLINES             0x00C8
#define EM_LINEFROMCHAR         0x00C9
#define EM_SETTABSTOPS          0x00CB
#define EM_SETPASSWORDCHAR      0x00CC
#define EM_EMPTYUNDOBUFFER      0x00CD
#define EM_GETFIRSTVISIBLELINE  0x00CE
#define EM_SETREADONLY          0x00CF
#define EM_SETWORDBREAKPROC     0x00D0
#define EM_GETWORDBREAKPROC     0x00D1
#define EM_GETPASSWORDCHAR      0x00D2
#define EM_SETMARGINS           0x00D3
#define EM_GETMARGINS           0x00D4
#define EM_SETLIMITTEXT         EM_LIMITTEXT   /* ;win40 Name change */
#define EM_GETLIMITTEXT         0x00D5
#define EM_POSFROMCHAR          0x00D6
#define EM_CHARFROMPOS          0x00D7
#define EM_SETIMESTATUS         0x00D8
#define EM_GETIMESTATUS         0x00D9

/*
 * Edit Control Notification Codes
 */
#define EN_SETFOCUS         0x0100
#define EN_KILLFOCUS        0x0200
#define EN_CHANGE           0x0300
#define EN_UPDATE           0x0400
#define EN_ERRSPACE         0x0500
#define EN_MAXTEXT          0x0501
#define EN_HSCROLL          0x0601
#define EN_VSCROLL          0x0602

/* New notifications */
#define EN_MSGFILTER            0x0700
#define EN_REQUESTRESIZE        0x0701
#define EN_SELCHANGE            0x0702
#define EN_DROPFILES            0x0703
#define EN_PROTECTED            0x0704
#define EN_CORRECTTEXT            0x0705            /* PenWin specific */
#define EN_STOPNOUNDO            0x0706
#define EN_IMECHANGE            0x0707            /* Far East specific */
#define EN_SAVECLIPBOARD        0x0708
#define EN_OLEOPFAILED            0x0709
#define EN_OBJECTPOSITIONS        0x070a
#define EN_LINK                    0x070b
#define EN_DRAGDROPDONE            0x070c

/*
 * EDITWORDBREAKPROC code values
 */
#define WB_LEFT            0
#define WB_RIGHT           1
#define WB_ISDELIMITER     2

/* new word break function actions */
#define WB_CLASSIFY            3
#define WB_MOVEWORDLEFT        4
#define WB_MOVEWORDRIGHT    5
#define WB_LEFTBREAK        6
#define WB_RIGHTBREAK        7

/* Far East specific flags */
#define WB_MOVEWORDPREV        4
#define WB_MOVEWORDNEXT        5
#define WB_PREVBREAK        6
#define WB_NEXTBREAK        7



/*
 * Combo Box return Values
 */
#define CB_OKAY             0
#define CB_ERR              (-1)
#define CB_ERRSPACE         (-2)


/*
 * Combo Box Notification Codes
 */
#define CBN_ERRSPACE        (-1)
#define CBN_SELCHANGE       1
#define CBN_DBLCLK          2
#define CBN_SETFOCUS        3
#define CBN_KILLFOCUS       4
#define CBN_EDITCHANGE      5
#define CBN_EDITUPDATE      6
#define CBN_DROPDOWN        7
#define CBN_CLOSEUP         8
#define CBN_SELENDOK        9
#define CBN_SELENDCANCEL    10


#ifndef NOWINSTYLES
// begin_r_winuser

/*
 * Combo Box styles
 */
#define CBS_SIMPLE            0x0001L
#define CBS_DROPDOWN          0x0002L
#define CBS_DROPDOWNLIST      0x0003L
#define CBS_OWNERDRAWFIXED    0x0010L
#define CBS_OWNERDRAWVARIABLE 0x0020L
#define CBS_AUTOHSCROLL       0x0040L
#define CBS_OEMCONVERT        0x0080L
#define CBS_SORT              0x0100L
#define CBS_HASSTRINGS        0x0200L
#define CBS_NOINTEGRALHEIGHT  0x0400L
#define CBS_DISABLENOSCROLL   0x0800L
#if (WINVER >= 0x0400)
#define CBS_UPPERCASE           0x2000L
#define CBS_LOWERCASE           0x4000L
#endif /* WINVER >= 0x0400 */

// end_r_winuser
#endif  /* !NOWINSTYLES */


/*
 * Combo Box messages
 */
#ifndef NOWINMESSAGES
#define CB_GETEDITSEL               0x0140
#define CB_LIMITTEXT                0x0141
#define CB_SETEDITSEL               0x0142
#define CB_ADDSTRING                0x0143
#define CB_DELETESTRING             0x0144
#define CB_DIR                      0x0145
#define CB_GETCOUNT                 0x0146
#define CB_GETCURSEL                0x0147
#define CB_GETLBTEXT                0x0148
#define CB_GETLBTEXTLEN             0x0149
#define CB_INSERTSTRING             0x014A
#define CB_RESETCONTENT             0x014B
#define CB_FINDSTRING               0x014C
#define CB_SELECTSTRING             0x014D
#define CB_SETCURSEL                0x014E
#define CB_SHOWDROPDOWN             0x014F
#define CB_GETITEMDATA              0x0150
#define CB_SETITEMDATA              0x0151
#define CB_GETDROPPEDCONTROLRECT    0x0152
#define CB_SETITEMHEIGHT            0x0153
#define CB_GETITEMHEIGHT            0x0154
#define CB_SETEXTENDEDUI            0x0155
#define CB_GETEXTENDEDUI            0x0156
#define CB_GETDROPPEDSTATE          0x0157
#define CB_FINDSTRINGEXACT          0x0158
#define CB_SETLOCALE                0x0159
#define CB_GETLOCALE                0x015A
#define CB_GETTOPINDEX              0x015b
#define CB_SETTOPINDEX              0x015c
#define CB_GETHORIZONTALEXTENT      0x015d
#define CB_SETHORIZONTALEXTENT      0x015e
#define CB_GETDROPPEDWIDTH          0x015f
#define CB_SETDROPPEDWIDTH          0x0160
#define CB_INITSTORAGE              0x0161
#if (WINVER >= 0x0400)
#define CB_MSGMAX                   0x0162
#else
#define CB_MSGMAX                   0x015B
#endif
#endif  /* !NOWINMESSAGES */



#ifndef NOWINSTYLES

// begin_r_winuser

/*
 * Scroll Bar Styles
 */
#define SBS_HORZ                    0x0000L
#define SBS_VERT                    0x0001L
#define SBS_TOPALIGN                0x0002L
#define SBS_LEFTALIGN               0x0002L
#define SBS_BOTTOMALIGN             0x0004L
#define SBS_RIGHTALIGN              0x0004L
#define SBS_SIZEBOXTOPLEFTALIGN     0x0002L
#define SBS_SIZEBOXBOTTOMRIGHTALIGN 0x0004L
#define SBS_SIZEBOX                 0x0008L
#define SBS_SIZEGRIP                0x0010L

// end_r_winuser

#endif /* !NOWINSTYLES */



/*
 * Window Messages
 */
#define WM_NULL                         0x0000
#define WM_CREATE                       0x0001
#define WM_DESTROY                      0x0002
#define WM_MOVE                         0x0003
#define WM_SIZE                         0x0005

#define WM_ACTIVATE                     0x0006
/*
 * WM_ACTIVATE state values
 */
#define     WA_INACTIVE     0
#define     WA_ACTIVE       1
#define     WA_CLICKACTIVE  2

#define WM_SETFOCUS                     0x0007
#define WM_KILLFOCUS                    0x0008
#define WM_ENABLE                       0x000A
#define WM_SETREDRAW                    0x000B
#define WM_SETTEXT                      0x000C
#define WM_GETTEXT                      0x000D
#define WM_GETTEXTLENGTH                0x000E
#define WM_PAINT                        0x000F
#define WM_CLOSE                        0x0010
#define WM_QUERYENDSESSION              0x0011
#define WM_QUIT                         0x0012
#define WM_QUERYOPEN                    0x0013
#define WM_ERASEBKGND                   0x0014
#define WM_SYSCOLORCHANGE               0x0015
#define WM_ENDSESSION                   0x0016
#define WM_SHOWWINDOW                   0x0018
#define WM_WININICHANGE                 0x001A
#define WM_SETTINGCHANGE                WM_WININICHANGE


#define WM_DEVMODECHANGE                0x001B
#define WM_ACTIVATEAPP                  0x001C
#define WM_FONTCHANGE                   0x001D
#define WM_TIMECHANGE                   0x001E
#define WM_CANCELMODE                   0x001F
#define WM_SETCURSOR                    0x0020
#define WM_MOUSEACTIVATE                0x0021
#define WM_CHILDACTIVATE                0x0022
#define WM_QUEUESYNC                    0x0023

#define WM_GETMINMAXINFO                0x0024
#define WM_PAINTICON                    0x0026
#define WM_ICONERASEBKGND               0x0027
#define WM_NEXTDLGCTL                   0x0028
#define WM_SPOOLERSTATUS                0x002A
#define WM_DRAWITEM                     0x002B
#define WM_MEASUREITEM                  0x002C
#define WM_DELETEITEM                   0x002D
#define WM_VKEYTOITEM                   0x002E
#define WM_CHARTOITEM                   0x002F
#define WM_SETFONT                      0x0030
#define WM_GETFONT                      0x0031
#define WM_SETHOTKEY                    0x0032
#define WM_GETHOTKEY                    0x0033
#define WM_QUERYDRAGICON                0x0037
#define WM_COMPAREITEM                  0x0039
#if (WINVER >= 0x0500)
#define WM_GETOBJECT                    0x003D
#endif /* WINVER >= 0x0500 */
#define WM_COMPACTING                   0x0041
#define WM_COMMNOTIFY                   0x0044  /* no longer suported */
#define WM_WINDOWPOSCHANGING            0x0046
#define WM_WINDOWPOSCHANGED             0x0047

#define WM_POWER                        0x0048
/*
 * wParam for WM_POWER window message and DRV_POWER driver notification
 */
#define PWR_OK              1
#define PWR_FAIL            (-1)
#define PWR_SUSPENDREQUEST  1
#define PWR_SUSPENDRESUME   2
#define PWR_CRITICALRESUME  3

#define WM_COPYDATA                     0x004A
#define WM_CANCELJOURNAL                0x004B


#define WM_NOTIFY                       0x004E
#define WM_INPUTLANGCHANGEREQUEST       0x0050
#define WM_INPUTLANGCHANGE              0x0051
#define WM_TCARD                        0x0052
#define WM_HELP                         0x0053
#define WM_USERCHANGED                  0x0054
#define WM_NOTIFYFORMAT                 0x0055

#define NFR_ANSI                             1
#define NFR_UNICODE                          2
#define NF_QUERY                             3
#define NF_REQUERY                           4

#define WM_CONTEXTMENU                  0x007B
#define WM_STYLECHANGING                0x007C
#define WM_STYLECHANGED                 0x007D
#define WM_DISPLAYCHANGE                0x007E
#define WM_GETICON                      0x007F
#define WM_SETICON                      0x0080
/*
 * WM_SETICON / WM_GETICON Type Codes
 */
#define ICON_SMALL          0
#define ICON_BIG            1


#define WM_NCCREATE                     0x0081
#define WM_NCDESTROY                    0x0082
#define WM_NCCALCSIZE                   0x0083
#define WM_NCHITTEST                    0x0084
#define WM_NCPAINT                      0x0085
#define WM_NCACTIVATE                   0x0086
#define WM_GETDLGCODE                   0x0087
#define WM_SYNCPAINT                    0x0088
#define WM_NCMOUSEMOVE                  0x00A0
#define WM_NCLBUTTONDOWN                0x00A1
#define WM_NCLBUTTONUP                  0x00A2
#define WM_NCLBUTTONDBLCLK              0x00A3
#define WM_NCRBUTTONDOWN                0x00A4
#define WM_NCRBUTTONUP                  0x00A5
#define WM_NCRBUTTONDBLCLK              0x00A6
#define WM_NCMBUTTONDOWN                0x00A7
#define WM_NCMBUTTONUP                  0x00A8
#define WM_NCMBUTTONDBLCLK              0x00A9

#define WM_KEYFIRST                     0x0100
#define WM_KEYDOWN                      0x0100
#define WM_KEYUP                        0x0101
#define WM_CHAR                         0x0102
#define WM_DEADCHAR                     0x0103
#define WM_SYSKEYDOWN                   0x0104
#define WM_SYSKEYUP                     0x0105
#define WM_SYSCHAR                      0x0106
#define WM_SYSDEADCHAR                  0x0107
#define WM_KEYLAST                      0x0108

#if (WINVER >= 0x0400)
#define WM_IME_STARTCOMPOSITION         0x010D
#define WM_IME_ENDCOMPOSITION           0x010E
#define WM_IME_COMPOSITION              0x010F
#define WM_IME_KEYLAST                  0x010F
#endif /* WINVER >= 0x0400 */

#define WM_INITDIALOG                   0x0110
#define WM_COMMAND                      0x0111
#define WM_SYSCOMMAND                   0x0112
#define WM_TIMER                        0x0113
#define WM_HSCROLL                      0x0114
#define WM_VSCROLL                      0x0115
#define WM_INITMENU                     0x0116
#define WM_INITMENUPOPUP                0x0117
#define WM_MENUSELECT                   0x011F
#define WM_MENUCHAR                     0x0120
#define WM_ENTERIDLE                    0x0121
#define WM_MENURBUTTONUP                0x0122
#define WM_MENUDRAG                     0x0123
#define WM_MENUGETOBJECT                0x0124
#define WM_UNINITMENUPOPUP              0x0125
#define WM_MENUCOMMAND                  0x0126


#define WM_CTLCOLORMSGBOX               0x0132
#define WM_CTLCOLOREDIT                 0x0133
#define WM_CTLCOLORLISTBOX              0x0134
#define WM_CTLCOLORBTN                  0x0135
#define WM_CTLCOLORDLG                  0x0136
#define WM_CTLCOLORSCROLLBAR            0x0137
#define WM_CTLCOLORSTATIC               0x0138


#define WM_MOUSEFIRST                   0x0200
#define WM_MOUSEMOVE                    0x0200
#define WM_LBUTTONDOWN                  0x0201
#define WM_LBUTTONUP                    0x0202
#define WM_LBUTTONDBLCLK                0x0203
#define WM_RBUTTONDOWN                  0x0204
#define WM_RBUTTONUP                    0x0205
#define WM_RBUTTONDBLCLK                0x0206
#define WM_MBUTTONDOWN                  0x0207
#define WM_MBUTTONUP                    0x0208
#define WM_MBUTTONDBLCLK                0x0209

#define WM_MOUSEWHEEL                   0x020A
#define WM_MOUSELAST                    0x020A

#define WHEEL_DELTA                     120     /* Value for rolling one detent */
#define WHEEL_PAGESCROLL                (UINT_MAX) /* Scroll one page */

#define WM_PARENTNOTIFY                 0x0210
#define WM_ENTERMENULOOP                0x0211
#define WM_EXITMENULOOP                 0x0212

#define WM_NEXTMENU                     0x0213
#define WM_SIZING                       0x0214
#define WM_CAPTURECHANGED               0x0215
#define WM_MOVING                       0x0216
#define WM_POWERBROADCAST               0x0218
#define WM_DEVICECHANGE                 0x0219

#define WM_MDICREATE                    0x0220
#define WM_MDIDESTROY                   0x0221
#define WM_MDIACTIVATE                  0x0222
#define WM_MDIRESTORE                   0x0223
#define WM_MDINEXT                      0x0224
#define WM_MDIMAXIMIZE                  0x0225
#define WM_MDITILE                      0x0226
#define WM_MDICASCADE                   0x0227
#define WM_MDIICONARRANGE               0x0228
#define WM_MDIGETACTIVE                 0x0229


#define WM_MDISETMENU                   0x0230
#define WM_ENTERSIZEMOVE                0x0231
#define WM_EXITSIZEMOVE                 0x0232
#define WM_DROPFILES                    0x0233
#define WM_MDIREFRESHMENU               0x0234


#if (WINVER >= 0x0602)
#define WM_POINTERDEVICECHANGE          0x238
#define WM_POINTERDEVICEINRANGE         0x239
#define WM_POINTERDEVICEOUTOFRANGE      0x23A
#endif /* WINVER >= 0x0602 */


#if (WINVER >= 0x0601)
#define WM_TOUCH                        0x0240
#endif /* WINVER >= 0x0601 */

#define WM_NCPOINTERUPDATE              0x0241
#define WM_NCPOINTERDOWN                0x0242
#define WM_NCPOINTERUP                  0x0243
#define WM_POINTERUPDATE                0x0245
#define WM_POINTERDOWN                  0x0246
#define WM_POINTERUP                    0x0247
#define WM_POINTERENTER                 0x0249
#define WM_POINTERLEAVE                 0x024A
#define WM_POINTERACTIVATE              0x024B
#define WM_POINTERCAPTURECHANGED        0x024C
#define WM_TOUCHHITTESTING              0x024D
#define WM_POINTERWHEEL                 0x024E
#define WM_POINTERHWHEEL                0x024F
#define DM_POINTERHITTEST               0x0250

//
// #if (WINVER >= 0x0400)
// #define WM_IME_SETCONTEXT               0x0281
// #define WM_IME_NOTIFY                   0x0282
// #define WM_IME_CONTROL                  0x0283
// #define WM_IME_COMPOSITIONFULL          0x0284
// #define WM_IME_SELECT                   0x0285
// #define WM_IME_CHAR                     0x0286
// #endif /* WINVER >= 0x0400 */
// #if (WINVER >= 0x0500)
// #define WM_IME_REQUEST                  0x0288
// #endif /* WINVER >= 0x0500 */
// #if (WINVER >= 0x0400)
// #define WM_IME_KEYDOWN                  0x0290
// #define WM_IME_KEYUP                    0x0291
// #endif /* WINVER >= 0x0400 */


#define WM_MOUSEHOVER                   0x02A1
#define WM_MOUSELEAVE                   0x02A3

#define WM_CUT                          0x0300
#define WM_COPY                         0x0301
#define WM_PASTE                        0x0302
#define WM_CLEAR                        0x0303
#define WM_UNDO                         0x0304
#define WM_RENDERFORMAT                 0x0305
#define WM_RENDERALLFORMATS             0x0306
#define WM_DESTROYCLIPBOARD             0x0307
#define WM_DRAWCLIPBOARD                0x0308
#define WM_PAINTCLIPBOARD               0x0309
#define WM_VSCROLLCLIPBOARD             0x030A
#define WM_SIZECLIPBOARD                0x030B
#define WM_ASKCBFORMATNAME              0x030C
#define WM_CHANGECBCHAIN                0x030D
#define WM_HSCROLLCLIPBOARD             0x030E
#define WM_QUERYNEWPALETTE              0x030F
#define WM_PALETTEISCHANGING            0x0310
#define WM_PALETTECHANGED               0x0311
#define WM_HOTKEY                       0x0312

#define WM_PRINT                        0x0317
#define WM_PRINTCLIENT                  0x0318

#define WM_DWMCOMPOSITIONCHANGED        0x031E

#define WM_HANDHELDFIRST                0x0358
#define WM_HANDHELDLAST                 0x035F

#define WM_AFXFIRST                     0x0360
#define WM_AFXLAST                      0x037F
//#endif /* WINVER >= 0x0400 */

#define WM_PENWINFIRST                  0x0380
#define WM_PENWINLAST                   0x038F




#define WM_USER                         0x0400
#define WM_APP                          0x8000


/* Graphics Modes */
#define GM_COMPATIBLE       1
#define GM_ADVANCED         2
#define GM_LAST             2

/* xform stuff */
#define MWT_IDENTITY        1
#define MWT_LEFTMULTIPLY    2
#define MWT_RIGHTMULTIPLY   3
#define MWT_MIN             MWT_IDENTITY
#define MWT_MAX             MWT_RIGHTMULTIPLY

/* Icon/Cursor header */
#define DI_MASK         0x0001
#define DI_IMAGE        0x0002
#define DI_NORMAL       0x0003
#define DI_COMPAT       0x0004
#define DI_DEFAULTSIZE  0x0008

/* AlphaBlend */
#define AC_SRC_ALPHA     0x01

/* Font Weights */
#define FW_DONTCARE         0
#define FW_THIN             100
#define FW_EXTRALIGHT       200
#define FW_LIGHT            300
#define FW_NORMAL           400
#define FW_MEDIUM           500
#define FW_SEMIBOLD         600
#define FW_BOLD             700
#define FW_EXTRABOLD        800
#define FW_HEAVY            900

// the modifiers of hot key
// #define MOD_ALT                         0x0001
// #define MOD_CONTROL                     0x0002
// #define MOD_SHIFT                       0x0004
// #define MOD_WIN                         0x0008
// #define MOD_LEFT                        0x8000
// #define MOD_RIGHT                       0x4000
// #define MOD_ON_KEYUP                    0x0800
// #define MOD_IGNORE_ALL_MODIFIER         0x0400


/*
 * AnimateWindow() Commands
 */
#define AW_HOR_POSITIVE             0x00000001
#define AW_HOR_NEGATIVE             0x00000002
#define AW_VER_POSITIVE             0x00000004
#define AW_VER_NEGATIVE             0x00000008
#define AW_CENTER                   0x00000010
#define AW_HIDE                     0x00010000
#define AW_ACTIVATE                 0x00020000
#define AW_SLIDE                    0x00040000
#define AW_BLEND                    0x00080000


#define IS_INTRESOURCE(_r) (((TMUIntPtr)(_r) >> 16) == 0)


#endif




#if !defined(TM_WIN) || defined (TM_QT)

#   include "tmgdi.h"


// windows
struct TMMSG {
    TMHWnd      hwnd;
    TMUInt      message;
    TMWParam    wParam;
    TMLParam    lParam;
};

struct TMSTYLESTRUCT {
    TMUInt32 styleOld;
    TMUInt32 styleNew;
};

// SetWindowPos Flags
#define TMHWND_TOP        ((TMHWnd)0)
#define TMHWND_BOTTOM     ((TMHWnd)1)
#define TMHWND_TOPMOST    ((TMHWnd)-1)
#define TMHWND_NOTOPMOST  ((TMHWnd)-2)

struct TMWINDOWPOS
{
    TMHWnd  hwnd;
    TMHWnd  hwndInsertAfter;
    int     x;
    int     y;
    int     cx;
    int     cy;
    TMUInt  flags;
};

// tool tip
struct TMTOOLINFO {
    TMUInt cbSize;
    TMUInt uFlags;
    TMHWnd hwnd;
    TMUInt uId;
    TMRect rect;
    TMHModule hinst;
    TMCStr lpszText;
    TMLParam lParam;
};

// other
struct TMRANGE
{
    int iLow;
    int iHigh;
};

#else // #if !defined(TM_WIN) || defined (TM_QT)

// windows
#   define TMMSG                        MSG
#   define TMSTYLESTRUCT                STYLESTRUCT

// SetWindowPos Flags
#   define TMHWND_TOP                   HWND_TOP       
#   define TMHWND_BOTTOM                HWND_BOTTOM    
#   define TMHWND_TOPMOST               HWND_TOPMOST   
#   define TMHWND_NOTOPMOST             HWND_NOTOPMOST 
#   define TMWINDOWPOS                  WINDOWPOS

// tool tip
#   define TMTOOLINFO                   TOOLINFO

// other
#   define TMRANGE                      PBRANGE

#ifndef TMCursorID
#   define TMCursorID                   LPCTSTR
#   define TMCursor_ARROW               IDC_ARROW
#   define TMCursor_IBEAM               IDC_IBEAM
#   define TMCursor_WAIT                IDC_WAIT
#   define TMCursor_CROSS               IDC_CROSS
#   define TMCursor_UPARROW             IDC_UPARROW
#   define TMCursor_SIZENWSE            IDC_SIZENWSE
#   define TMCursor_SIZENESW            IDC_SIZENESW
#   define TMCursor_SIZEWE              IDC_SIZEWE
#   define TMCursor_SIZENS              IDC_SIZENS
#   define TMCursor_SIZEALL             IDC_SIZEALL
#   define TMCursor_NO                  IDC_NO
#   define TMCursor_HAND                IDC_HAND
#   define TMCursor_APPSTARTING         IDC_APPSTARTING
#   define TMCursor_HELP                IDC_HELP
#   define TMCursor_SPLITV              IDC_SIZEWE
#   define TMCursor_SPLITH              IDC_SIZENS
#endif // #ifndef TMCursorID


#endif // #if !defined(TM_WIN) || defined (TM_QT) else



#ifndef WS_EX_LAYERED
#   define WS_EX_LAYERED                    0x00080000
#endif

#ifndef WS_EX_NOREDIRECTIONBITMAP
#   define WS_EX_NOREDIRECTIONBITMAP        0x00200000
#endif

#ifndef WM_NCMOUSELEAVE
#   define WM_NCMOUSELEAVE                  0x02A2
#endif

#ifndef WM_DPICHANGED
#   define WM_DPICHANGED                    0x02E0
#endif

#ifndef WM_GETDPISCALEDSIZE
#   define WM_GETDPISCALEDSIZE              0x02E4
#endif

#ifndef WM_SYSTIMER
#   define WM_SYSTIMER                        0x0118
#endif

#ifndef WM_POPUPSYSTEMMENU
#   define WM_POPUPSYSTEMMENU               0x0313
#endif

#ifndef WM_CHANGEUISTATE

#   define WM_CHANGEUISTATE                 0x0127
#   define WM_UPDATEUISTATE                 0x0128
#   define WM_QUERYUISTATE                  0x0129

// LOWORD(wParam) values in WM_*UISTATE*
#   define UIS_SET                          1
#   define UIS_CLEAR                        2
#   define UIS_INITIALIZE                   3

// HIWORD(wParam) values in WM_*UISTATE*
#   define UISF_HIDEFOCUS                   0x1
#   define UISF_HIDEACCEL                   0x2
#   ifndef UISF_ACTIVE
#       define UISF_ACTIVE                  0x4
#   endif /* UISF_ACTIVE */

#endif //WM_CHANGEUISTATE


#endif // TMNATIVE_H
