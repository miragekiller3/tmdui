/*
 *  @file
 *  @brief
 *    CMouseXXX:        mouse for native
 *    CMouse:           mouse for framework
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.8.13
 *
 *  Copyright (C) 2020 miragekiller
 */

#pragma once

#ifndef TMMOUSE_H
#define TMMOUSE_H

#include "tmgdi.h"

#ifdef TM_WIN
#   include <windows.h>
#endif

#ifdef TM_X11
#   include <X11/Xlib.h>
#endif

#ifdef TM_MAC
#endif 

#ifdef TM_QT
#   include <QApplication>
#endif

#ifdef TM_GTK
#   include <gdk/gdk.h>
#endif


TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CMouseWin
#ifdef TM_WIN

class CMouseWin
{
public:
    static CPointNative GetCursorPos()
    {
        TM::CPointNative pt;
        ::GetCursorPos(&pt);
        return pt;
    }
    
    static void SetCursorPos(const CPointNative& pt)
    {
        ::SetCursorPos(pt.X(), pt.Y());
    }
    
    static void VirtualClick(const CPointNative& pt)
    {
        ::SetCursorPos(pt.X(), pt.Y());
        ::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); // MOUSEEVENTF_ABSOLUTE
    }
};

#endif // TM_WIN

///////////////////////////////////////////////////////////////////////////////
// CMouseMac
#ifdef TM_MAC
class CMouseMac
{
public:
    static CPointNative GetCursorPos()
    {
        CGEventRef event = CGEventCreate(NULL);
        CPointNative cursor = CGEventGetLocation(event);
        CFRelease(event);
        return cursor;
    }

    static void SetCursorPos(const CPoint& pt)
    {
        CGWarpMouseCursorPosition(pt);
        CGAssociateMouseAndMouseCursorPosition(true);
    }

    static void VirtualClick(const CPoint& pt)
    {
        CGEventRef click_down = CGEventCreateMouseEvent(
            NULL, kCGEventLeftMouseDown,
            pt,
            kCGMouseButtonLeft
           );
        CGEventRef click_up = CGEventCreateMouseEvent(
            NULL, kCGEventLeftMouseUp,
            pt,
            kCGMouseButtonLeft
           );
        CGEventPost(kCGHIDEventTap, click_down);
        CGEventPost(kCGHIDEventTap, click_up);
        CFRelease(click_up);
        CFRelease(click_down);
    }
};

#endif // TM_MAC


///////////////////////////////////////////////////////////////////////////////
// CMouseGtk
#ifdef TM_GTK
class CMouseGtk
{
public:
    static CPointNative GetCursorPos()
    {
        CPointNative pt;
        GdkDisplay* display = gdk_display_get_default();
        GdkSeat*    seat    = gdk_display_get_default_seat(display);
        GdkDevice*  pointer = gdk_seat_get_pointer(seat);
        gint x = 0, y = 0;
        gdk_device_get_position(pointer, NULL, &x, &y);
        pt.SetPoint(x, y);
        return pt;
    }

#if defined(TM_GTK3)
    static void SetCursorPos(const CPointNative& pt)
    {
        GdkDisplay* display = gdk_display_get_default();
        GdkSeat*    seat    = display ? gdk_display_get_default_seat(display) : NULL;
        GdkDevice*  pointer = seat ? gdk_seat_get_pointer(seat) : NULL;
        if (pointer)
            gdk_device_warp(pointer, gdk_display_get_default_screen(display), pt.X(), pt.Y());
    }

    static void VirtualClick(const CPointNative& pt)
    {
        SetCursorPos(pt);
        GdkWindow* pRootWnd = gdk_get_default_root_window();
        if (pRootWnd)
        {
            gdk_test_simulate_button(pRootWnd, pt.X(), pt.Y(), 1, (GdkModifierType)0, GDK_BUTTON_PRESS);
            gdk_test_simulate_button(pRootWnd, pt.X(), pt.Y(), 1, (GdkModifierType)0, GDK_BUTTON_RELEASE);
        }
    }
#endif
};

#endif // TM_GTK

///////////////////////////////////////////////////////////////////////////////
// CMouseX11
#ifdef TM_X11
class CMouseX11
{
public:
    static CPointNative GetCursorPos()
    {
        Display* dpy = XOpenDisplay(0);
        Window root_window = XRootWindow(dpy, 0);
        Window root_ret, child_ret;
        int root_x, root_y;
        int win_x, win_y;
        unsigned int mask;
        Bool result = XQueryPointer(dpy, root_window,
            &root_ret, &child_ret,
            &root_x, &root_y,
            &win_x, &win_y,
            &mask);
        if (result == True)
            pt.SetPoint(root_x, root_y);
        XCloseDisplay(dpy);
    }

    static void SetCursorPos(const CPointNative& pt)
    {
        Display* dpy = XOpenDisplay(0);
        Window root_window = XRootWindow(dpy, 0);
        XSelectInput(dpy, root_window, KeyReleaseMask);
        XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, pt.X(), pt.Y());
        XFlush(dpy);
        XCloseDisplay(dpy);
    }

//     static void VirtualClick(const CPointNative& pt)
//     {
//     }
};

#endif // TM_X11


///////////////////////////////////////////////////////////////////////////////
// CMouseQt
#ifdef TM_QT
class CMouseQt
{
public:
    typedef Qt::CursorShape cursor_id;
    
    static cursor_id cursorARROW()        { return Qt::ArrowCursor; }
    static cursor_id cursorIBEAM()        { return Qt::IBeamCursor; }
    static cursor_id cursorWAIT()         { return Qt::WaitCursor; }
    static cursor_id cursorCROSS()        { return Qt::CrossCursor; }
    static cursor_id cursorUPARROW()      { return Qt::UpArrowCursor; }
    static cursor_id cursorSIZENWSE()     { return Qt::SizeFDiagCursor; }
    static cursor_id cursorSIZENESW()     { return Qt::SizeBDiagCursor; }
    static cursor_id cursorSIZEWE()       { return Qt::SizeHorCursor; }
    static cursor_id cursorSIZENS()       { return Qt::SizeVerCursor; }
    static cursor_id cursorSIZEALL()      { return Qt::SizeAllCursor; }
    static cursor_id cursorNO()           { return Qt::ForbiddenCursor; }
    static cursor_id cursorHAND()         { return Qt::PointingHandCursor; }
    static cursor_id cursorAPPSTARTING()  { return Qt::WaitCursor; }
    static cursor_id cursorHELP()         { return Qt::WhatsThisCursor; }
    static cursor_id cursorSPLITV()       { return Qt::SplitVCursor; }
    static cursor_id cursorSPLITH()       { return Qt::SplitHCursor; }

    static CPoint GetCursorPos()
    {
        return QCursor::pos();
    }

    static void SetCursorPos(const CPoint& pt)
    {
        QCursor::setPos(pt);
    }
};

#endif // TM_QT

#ifdef TM_QT
#   define CMouse           CMouseQt
#elif defined(TM_GTK)
#   define CMouse           CMouseGtk
#elif defined(TM_MAC)
#   define CMouse           CMouseMac
#elif defined(TM_X11)
#   define CMouse           CMouseX11
#elif defined(TM_WIN)
#   define CMouse           CMouseWin
#else
#   error sot support
#endif // #ifdef TM_QT #else


TM_END_NAMESPACE



#endif // ATINPUT_H
