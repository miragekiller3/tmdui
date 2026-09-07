/*
 *  @file
 *  @brief CMonitor
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.8.12
 *
 *  Copyright (C) 2020 miragekiller
 */

#pragma once

#ifndef TMMONITOR_H
#define TMMONITOR_H

#include "tmgdi.h"
#include "tmdll.h"

#ifdef TM_QT
#   include <QApplication>
#endif


TM_BGN_NAMESPACE


enum EMonitorDefault
{
    eMonitorDefaultNull     = 0, // MONITOR_DEFAULTTONULL
    eMonitorDefaultPrimary  = 1, // MONITOR_DEFAULTTOPRIMARY
    eMonitorDefaultNearest  = 2, // MONITOR_DEFAULTTONEAREST
};


/**
 * calculate menu popped position from rect.
 * @param[in, out] rc, the popped menu rect. after calculate, the function will fill new pos in rc.left and rc.top.
 * @param[in] rcOwner, the param used to get current monitor.
 * @param[in] rcExclude, the new position will avoid the rect.
 */
template <class T, class T_rect>
static void _Monitor_HierarchyFromRect(const T& monitor, T_rect& rcNew, const T_rect& rcExclude)
{
    T_rect rcWork;
    monitor.GetRect(0, &rcWork);
    
    // make rect in monitor
    int x = rcNew.Left();
    if (x < rcWork.Left())
        x = rcWork.Left();
    else if (rcNew.Right() > rcWork.Right())
        x -= rcNew.Right() - rcWork.Right();
    
    int y = rcNew.Top();
    if (y < rcWork.Top())
        y = rcWork.Top();
    else if (rcNew.Bottom() > rcWork.Bottom())
        y -= rcNew.Bottom() - rcWork.Bottom();
    
    // make rect exclude rcExclude
    int w = rcNew.Right() - rcNew.Left();
    //    int r = x + w;
    if (x > rcExclude.Left() - w && x < rcExclude.Right())
    {
        int xx = rcExclude.Left() - w;
        if (xx > rcWork.Left())
            x = xx;
    }
    
    rcNew.KeepRSetL(x);
    rcNew.KeepBSetT(y);
}

// center window logic:
// child: center by parent, don't check screen area
// owner: center by owner, check screen area
// screen: center by screen, don't check screen area
// nType:0, DS_CENTEROWNER, DS_CENTERSCREEN
// T_Window: CWindow, CWidget, CWindowNative...
template <class T, class M>
void _Monitor_CenterWindow(M* monitor, T& win, TMBool bCenterScreen, TMBool bCenterOwner)
{
    if (!bCenterScreen || !bCenterOwner)
        return;
    
    DbgAssert(win.IsWindow());
    typedef typename M::rect_type rect_type;
    rect_type rcDlg, rcCenter, rcScreen;
    win.GetWindowRect(&rcDlg);
    
    // determine owner window to center against
    T wndParent(win.GetParent());
    if (wndParent)
        wndParent.GetClientRect(&rcCenter);
    else
    {
        T wndOwner(win.GetWindow(GW_OWNER));
        M mTemp(M::FromWindow(wndOwner ? wndOwner : win));
        mTemp.GetRect(0, &rcScreen);
        if (bCenterOwner && wndOwner)
            wndParent.GetWindowRect(&rcCenter);
        else
            rcCenter = rcScreen;
    }
    
    int DlgWidth = rcDlg.Width();
    int DlgHeight = rcDlg.Height();
    
    // find dialog's upper left based on rcCenter
    int xLeft = (rcCenter.Left() + rcCenter.Right()) / 2 - DlgWidth / 2;
    int yTop = (rcCenter.Top() + rcCenter.Bottom()) / 2 - DlgHeight / 2;
    
    // if the dialog is outside the screen, move it inside
    if (!wndParent && bCenterOwner)
    {
        if (xLeft < rcScreen.Left())
            xLeft = rcScreen.Left();
        else if (xLeft + DlgWidth > rcScreen.Right())
            xLeft = rcScreen.Right() - DlgWidth;
        
        if (yTop < rcScreen.Top())
            yTop = rcScreen.Top();
        else if (yTop + DlgHeight > rcScreen.Bottom())
            yTop = rcScreen.Bottom() - DlgHeight;
    }
    
    // map screen coordinates to child coordinates
    win.SetWindowPos(NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

template <class T_rect, class T_monitor>
static void _Monitor_CalcMaxRect(T_monitor& monitor, T_rect& rc, TMBool bFullScreen, TMBool bMaximize)
{
    if (bFullScreen)
        monitor.GetRect(&rc, 0);
    else if (bMaximize)
        monitor.GetRect(0, &rc);
}

template <class T_rect, class T_monitor>
static void _Monitor_CalcValideRect(T_monitor& monitor, T_rect& rc, TMBool bFullScreen, TMBool bMaximize)
{
    monitor = T_monitor::FromRect(rc, eMonitorDefaultNull);
    if (monitor)
    {
        _Monitor_CalcMaxRect(monitor, rc, bFullScreen, bMaximize);
        return;
    }
    monitor = T_monitor::FromRect(rc, eMonitorDefaultNearest);
    if (bFullScreen || bMaximize)
    {
        _Monitor_CalcMaxRect(monitor, rc, bFullScreen, bMaximize);
        return;
    }
    T_rect rcScreen;
    monitor.GetRect(0, &rcScreen);
    if (rc.Width() > rcScreen.Width())  // width > screen.width
        rc.SetLeftRight(rcScreen.Left(), rcScreen.Right());
    else if (rc.Right() > rcScreen.Right()) // window on screen right
        rc.KeepWSetR(rcScreen.Right());
    else if (rc.Left() < rcScreen.Left()) // window on screen left
        rc.KeepWSetL(rcScreen.Left());

    if (rc.Height() > rcScreen.Height())
        rc.SetTopBottom(rcScreen.Top(), rcScreen.Bottom());
    else if (rc.Bottom() > rcScreen.Bottom()) // window on screen bottom
        rc.KeepHSetB(rcScreen.Bottom());
    else if (rc.Top() < rcScreen.Top()) // window on screen top
        rc.KeepHSetT(rcScreen.Top());
}


///////////////////////////////////////////////////////////////////////////////
// monitor for window
#ifdef TM_WIN

class CMonitorWin
{
public:
    typedef CRectNative rect_type;

    HMONITOR m_hMonitor;
    
private:
    CMonitorWin(HMONITOR hm) : m_hMonitor(hm) {}
    
public:
    static CMonitorWin Primary()
    {
        return MonitorFromWindow(0, MONITOR_DEFAULTTOPRIMARY);
    }
    
    static CMonitorWin FromWindow(HWND hWnd, EMonitorDefault eDefault = eMonitorDefaultPrimary)
    {
        return MonitorFromWindow(hWnd, eDefault);
    }
    
    static CMonitorWin FromRect(const TMRectNative& rc, EMonitorDefault eDefault = eMonitorDefaultNearest)
    {
        return MonitorFromRect(&rc, eDefault);
    }
    
    static CMonitorWin FromPoint(const TMPointNative& pt, EMonitorDefault eDefault = eMonitorDefaultNearest)
    {
        return MonitorFromPoint(pt, eDefault);
    }

    operator HMONITOR() const { return m_hMonitor; }

    void GetRect(TMRectNative* rcMonitor, TMRectNative* rcWork) const
    {
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfo(m_hMonitor, &mi);
        if (rcMonitor)
            *rcMonitor = mi.rcMonitor;
        if (rcWork)
            *rcWork = mi.rcWork;
    }

#ifndef DPI_ENUMS_DECLARED
    typedef enum _MONITOR_DPI_TYPE { 
        MDT_EFFECTIVE_DPI  = 0,
            MDT_ANGULAR_DPI    = 1,
            MDT_RAW_DPI        = 2,
            MDT_DEFAULT        = MDT_EFFECTIVE_DPI
    } MONITOR_DPI_TYPE;
#endif // DPI_ENUMS_DECLARED
    
    DECLARE_DLL_FUNC3(dll_shcore, HRESULT, WINAPI, GetDpiForMonitor,
        (HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY));

    inline int GetDPI() const
    {
        UINT x = TM_DPI_SYSTEM_DEFAULT, y = 0;
        if (TM_IMPORT(GetDpiForMonitor))
            TM_IMPORT(GetDpiForMonitor)(m_hMonitor, MDT_EFFECTIVE_DPI, &x, &y);
        return x;
    }

    static void HierarchyFromRect(CRectNative& rcNew, const TMRectNative& rcOwner, const CRectNative& rcExclude)
    {
        _Monitor_HierarchyFromRect(FromRect(rcOwner), rcNew, rcExclude);
    }

    static CMonitorWin CalcValideRect(TMRectNative& rc, TMBool bFullScreen, TMBool bMaximize)
    {
        CMonitorWin monitor(0);
        _Monitor_CalcValideRect(monitor, (CRectNative&)rc, bFullScreen, bMaximize);
        return monitor;
    }
};

#endif // TM_WIN


///////////////////////////////////////////////////////////////////////////////
// monitor for QT
#ifdef TM_QT

class CMonitorQt
{
public:
    typedef CRect rect_type;

    TMHMonitor m_hMonitor;

private:
    CMonitorQt(TMHMonitor hm) : m_hMonitor(hm) {}

public:
    static inline CMonitorQt Primary()
    {
        return QGuiApplication::primaryScreen();
    }

    static inline CMonitorQt FromWindow(TMHWnd hWnd, EMonitorDefault eDefault = eMonitorDefaultPrimary)
    {
        if (hWnd)
            return hWnd->windowHandle()->screen();
        return eDefault == eMonitorDefaultPrimary ? Primary() : 0;
    }

    static inline CMonitorQt FromRect(const TMRect& rc, EMonitorDefault eDefault = eMonitorDefaultNearest)
    {
        return FromPoint(((CRect&)rc).CenterPoint(), eDefault);
    }

    static inline CMonitorQt FromPoint(const TMPoint& pt, EMonitorDefault eDefault = eMonitorDefaultNearest)
    {
        QScreen* qNearest = 0;
        int nLen = INT_MAX;
        QList<QScreen *> ql(QGuiApplication::screens());
        for (int i = 0; i < ql.size(); ++i)
        {
            QScreen* qs = ql.at(i);
            CRect rc(qs->geometry());
            if (rc.PtInRect(pt))
                return qs;
            CPoint pt2(rc.CenterPoint());
            int nNow = abs(PointX(pt) - PointX(pt2));
            tm_setmin(nNow, abs(PointY(pt) - PointY(pt2)));
            if (nNow < nLen)
            {
                nLen = nNow;
                qNearest = qs;
            }
        }
        return (eDefault == eMonitorDefaultNearest) ? qNearest : 0;
    }

    operator TMHMonitor() const { return m_hMonitor; }

    int GetDPI() const
    {
        return m_hMonitor->logicalDotsPerInch();
    }

    void GetRect(TMRect* rcMonitor, TMRect* rcWork) const
    {
        if (rcMonitor)
            *rcMonitor = m_hMonitor->geometry();
        if (rcWork)
            *rcWork = m_hMonitor->availableGeometry();
    }

    static void HierarchyFromRect(CRect& rcNew, const TMRect& rcOwner, const CRect& rcExclude)
    {
        _Monitor_HierarchyFromRect(FromRect(rcOwner), rcNew, rcExclude);
    }

    static CMonitorQt CalcValideRect(TMRect& rc, TMBool bFullScreen, TMBool bMaximize)
    {
        CMonitorQt monitor(0);
        _Monitor_CalcValideRect(monitor, (CRect&)rc, bFullScreen, bMaximize);
        return monitor;
    }
};

#endif


///////////////////////////////////////////////////////////////////////////////
// monitor for GTK
#ifdef TM_GTK

class CMonitorGtk
{
public:
    typedef CRect rect_type;

    GdkMonitor* m_hMonitor;

private:
    CMonitorGtk(TMHMonitor hm) : m_hMonitor(hm) {}

public:
    static inline CMonitorGtk Primary()
    {
        GdkDisplay* dpy = gdk_display_get_default();
        if (!dpy)
            return 0;
        GdkMonitor* m = gdk_display_get_primary_monitor(dpy);
        if (!m && gdk_display_get_n_monitors(dpy) > 0)
            m = gdk_display_get_monitor(dpy, 0);
        return m;
    }

    static inline CMonitorGtk FromWindow(TMHWnd hWnd, EMonitorDefault eDefault = eMonitorDefaultPrimary)
    {
        GdkDisplay* dpy = hWnd ? gtk_widget_get_display(GTK_WIDGET(hWnd)) : gdk_display_get_default();
        if (!dpy)
            return 0;

        if (hWnd)
        {
            GdkWindow* gw = gtk_widget_get_window(GTK_WIDGET(hWnd));
            if (gw)
            {
                GdkMonitor* m = gdk_display_get_monitor_at_window(dpy, gw);
                if (m)
                    return m;
            }
        }

        if (eDefault == eMonitorDefaultPrimary)
            return Primary();
        if (eDefault == eMonitorDefaultNearest && gdk_display_get_n_monitors(dpy) > 0)
            return gdk_display_get_monitor(dpy, 0);
        return 0;
    }

    static inline CMonitorGtk FromRect(const TMRect& rc, EMonitorDefault eDefault = eMonitorDefaultNearest)
    {
        return FromPoint(((CRect&)rc).CenterPoint(), eDefault);
    }

    static inline CMonitorGtk FromPoint(const TMPoint& pt, EMonitorDefault eDefault = eMonitorDefaultNearest)
    {
        GdkDisplay* dpy = gdk_display_get_default();
        if (!dpy)
            return 0;

        int n = gdk_display_get_n_monitors(dpy);
        if (n <= 0)
            return 0;

        const int x = (int)PointX(pt);
        const int y = (int)PointY(pt);
        GdkMonitor* nearest = gdk_display_get_monitor_at_point(dpy, x, y);

        for (int i = 0; i < n; ++i)
        {
            GdkMonitor* m = gdk_display_get_monitor(dpy, i);
            GdkRectangle rcMon;
            gdk_monitor_get_geometry(m, &rcMon);
            if (x >= rcMon.x && x < rcMon.x + rcMon.width && y >= rcMon.y && y < rcMon.y + rcMon.height)
                return m;
        }

        if (eDefault == eMonitorDefaultNearest)
            return nearest;
        if (eDefault == eMonitorDefaultPrimary)
            return Primary();
        return 0;
    }

    operator GdkMonitor*() const { return m_hMonitor; }

    int GetDPI() const
    {
        if (!m_hMonitor)
            return TM_DPI_SYSTEM_DEFAULT;
        int scale = gdk_monitor_get_scale_factor(m_hMonitor);
        if (scale <= 0)
            scale = 1;
        return TM_DPI_SYSTEM_DEFAULT * scale;
    }

    void GetRect(TMRect* rcMonitor, TMRect* rcWork) const
    {
        if (!m_hMonitor)
            return;

        GdkRectangle rm;
        gdk_monitor_get_geometry(m_hMonitor, &rm);

        if (rcMonitor)
            *rcMonitor = CRect(rm.x, rm.y, rm.x + rm.width, rm.y + rm.height);

        if (rcWork)
        {
#if GTK_CHECK_VERSION(3,22,0)
            GdkRectangle rw;
            gdk_monitor_get_workarea(m_hMonitor, &rw);
            *rcWork = CRect(rw.x, rw.y, rw.x + rw.width, rw.y + rw.height);
#else
            *rcWork = CRect(rm.x, rm.y, rm.x + rm.width, rm.y + rm.height);
#endif
        }
    }

    static void HierarchyFromRect(CRect& rcNew, const TMRect& rcOwner, const CRect& rcExclude)
    {
        _Monitor_HierarchyFromRect(FromRect(rcOwner), rcNew, rcExclude);
    }

    static CMonitorGtk CalcValideRect(TMRect& rc, TMBool bFullScreen, TMBool bMaximize)
    {
        CMonitorGtk monitor(0);
        _Monitor_CalcValideRect(monitor, (CRect&)rc, bFullScreen, bMaximize);
        return monitor;
    }
};

#endif


#ifdef TM_QT
#   define CMonitor CMonitorQt
#elif defined(TM_GTK)
#   define CMonitor CMonitorGtk
#elif defined(TM_WIN)
#   define CMonitor CMonitorWin
#else
#   error not support
#endif


TM_END_NAMESPACE


#endif //TMMONITOR_H
