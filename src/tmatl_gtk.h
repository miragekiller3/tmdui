/*
 *  @file
 *  @brief
 *  ATL for GTK
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.08.03
 *
 *  Copyright (C) 2019 miragekiller
 */

#pragma once

#ifndef TMATL_GTK_H
#define TMATL_GTK_H

#ifndef TMATL_H
#   error the file must include from "tmatl.h"
#endif

#ifndef TM_GTK
#   error "only for GTK platform"
#endif

#include <gtk/gtk.h>
#include <cairo.h>

#include "tmstring.h"
#include "tmcolor.h"
#include "tmmessagequeue.h"
#include "tmmouse.h"


// #define GTK_CLASS_CONSTRUCT(cls_name,handle_type,obj_name) \
//     typedef cls_name class_type; \
//     cls_name (handle_type p = 0) : obj_name(p) {} \
//     operator handle_type () const { return obj_name; } \
//     bool operator ! () const { return !handle_type; } \
//     void Swap(cls_name& rhs) { tm_swap(obj_name, rhs.obj_name); } \
//     void Attach(handle_type p) { obj_name = p; } \
//     handle_type Detach() { handle_type p = obj_name; obj_name = 0; return p; } \
//     cls_name& operator = (handle_type p) { obj_name = p; return this; }


TM_BGN_NAMESPACE

// Custom message structure for GTK idle/event queue
struct _TMCustomMessage
{
    TMHWnd         hWnd;           // Target window
    TMUInt         message;        // Message ID
    TMWParam       wParam;         // First parameter
    TMLParam       lParam;         // Second parameter
    void*          pUser;          // User callback pointer
    gpointer       pCallback;      // Callback function pointer
    TMResult*      pResult;        // Result output (for sync sends)
    GMutex*        sync_mutex;     // Mutex for sync sends
    GCond*         sync_cond;      // Condition var for sync sends
    gboolean       is_sync;        // Is synchronous send?
};

// Idle callback function that processes custom messages
static gboolean _tm_message_handler_idle(gpointer user_data)
{
    _TMCustomMessage* msg = (_TMCustomMessage*)user_data;
    if (!msg || !msg->hWnd) return G_SOURCE_REMOVE;
    
    // Try to get WndProc callback if available
    if (msg->pCallback)
    {
        // Cast to PFNWindowProc and call it
        // typedef TMBool (*PFNWindowProc)(void* pVoid, TMHWnd hWnd, TMUInt uMsg, TMWParam wParam,
        //                              TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID);
        typedef TMBool (*PFNWindowProc)(void*, TMHWnd, TMUInt, TMWParam, TMLParam, TMResult&, TMULong);
        PFNWindowProc pfn = (PFNWindowProc)msg->pCallback;
        TMResult result = 0;
        TMULong dwMsgMapID = 0;
        
        if (pfn)
            pfn(msg->pUser, msg->hWnd, msg->message, msg->wParam, msg->lParam, result, dwMsgMapID);
        
        if (msg->pResult)
            *msg->pResult = result;
    }
    
    // Signal waiting thread if this was a sync send
    if (msg->is_sync && msg->sync_mutex && msg->sync_cond)
    {
        g_mutex_lock(msg->sync_mutex);
        g_cond_signal(msg->sync_cond);
        g_mutex_unlock(msg->sync_mutex);
    }
    
    // Cleanup
    if (msg->pResult == NULL) // Only free if not waiting for result
    {
        if (msg->sync_mutex) g_mutex_free(msg->sync_mutex);
        if (msg->sync_cond) g_cond_free(msg->sync_cond);
        g_free(msg);
    }
    
    return G_SOURCE_REMOVE;
}

static inline const char* _TMUtf8Slice(TMCStr s, int count, gchar** ownedText = NULL)
{
    if (ownedText)
        *ownedText = NULL;
    if (!s)
        return "";
    if (count < 0)
        return s;

    const char* end = g_utf8_offset_to_pointer(s, count);
    if (*end == '\0')
        return s;

    if (!ownedText)
        return s;

    *ownedText = g_strndup(s, (gsize)(end - s));
    return *ownedText ? *ownedText : "";
}

static inline const char* _TMUtf8TabsToSpaces(TMCStr s, int count, gchar** ownedText = NULL)
{
    const char* text = _TMUtf8Slice(s, count, ownedText);
    for (const char* p = text; *p; ++p)
    {
        if (*p == '\t')
        {
            if (ownedText && *ownedText == NULL)
                *ownedText = g_strdup(text);
            if (*ownedText)
            {
                for (char* q = *ownedText; *q; ++q)
                {
                    if (*q == '\t')
                        *q = ' ';
                }
                return *ownedText;
            }
            return text;
        }
    }
    return text;
}


///////////////////////////////////////////////////////////////////////////////
// CWindowGtk
class CWindowGtk
{
public:
    TMHWnd m_hWnd;

 //   GTK_CLASS_CONSTRUCT(CWindowGtk, TMHWnd, m_hWnd);

    CWindowGtk (TMHWnd p = 0) : m_hWnd(p) {}
    operator TMHWnd () const { return m_hWnd; }
    bool operator ! () const { return !m_hWnd; }
    void Swap(CWindowGtk& rhs) { tm_swap(m_hWnd, rhs.m_hWnd); }
    void Attach(TMHWnd p) { m_hWnd = p; }
    TMHWnd Detach() { TMHWnd p = m_hWnd; m_hWnd = 0; return p; }
    CWindowGtk& operator = (TMHWnd p) { m_hWnd = p; return *this; }

    static TMHWnd _AsWindow(GtkWidget* w)
    {
        return (w && GTK_IS_WINDOW(w)) ? GTK_WINDOW(w) : NULL;
    }
    
    // static ATOM NativeRegister(TMCStr p)
    // {
    //     tm_unused(p);
    //     return 1;
    // }
    
    template <class WndProcT>
        static TMHWnd NativeCreate2(TMCStr p, TMHWnd hWndParent, WndProcT wndProc, void* user)
    {
        tm_unused(p);
        tm_unused(hWndParent);
        GtkWidget* w = gtk_window_new(GTK_WINDOW_POPUP);
        
        // Store user data and callback pointer for message routing
        g_object_set_data(G_OBJECT(w), "tm.user", user);
        
        // Store callback function pointer (type-erased)
        // This callback will be invoked by PostMessage/SendMessage handlers
        if (wndProc)
            g_object_set_data(G_OBJECT(w), "tm.callback", (gpointer)wndProc);
        
        return GTK_WINDOW(w);
    }
    
    static TMCStr GetWndClassName()
    {
        return NULL;
    }
    
    TMHWnd Create(TMCStr lpstrWndClass, TMHWnd hWndParent, TMRect& rcPos, TMCStr szWindowName = NULL,
        TMUInt32 dwStyle = 0, TMUInt32 dwExStyle = 0,
        TMUInt nID = 0, void* lpCreateParam = NULL)
    {
        tm_unused(lpstrWndClass);
        tm_unused(dwStyle);
        tm_unused(dwExStyle);
        tm_unused(nID);
        tm_unused(lpCreateParam);
        GtkWidget* w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        if (hWndParent)
            gtk_window_set_transient_for(GTK_WINDOW(w), hWndParent);
        gtk_window_move(GTK_WINDOW(w), RectLeft(rcPos), RectTop(rcPos));
        gtk_widget_set_size_request(w, RectWidth(rcPos), RectHeight(rcPos));
        if (szWindowName)
        {
            gtk_window_set_title(GTK_WINDOW(w), szWindowName);
        }
        m_hWnd = GTK_WINDOW(w);
        return m_hWnd;
    }
    
    TMHWnd Create(TMCStr lpstrWndClass, TMHWnd hWndParent, TMRect* lpRect = NULL, TMCStr szWindowName = NULL,
        TMUInt32 dwStyle = 0, TMUInt32 dwExStyle = 0,
        TMHMenu hMenu = NULL, void* lpCreateParam = NULL)
    {
        tm_unused(hMenu);
        DbgAssert(lpRect);
        m_hWnd = Create(lpstrWndClass, hWndParent, *lpRect, szWindowName, dwStyle, dwExStyle, 0, lpCreateParam);
        return m_hWnd;
    }
    
    TMBool DestroyWindow()
    {
        DbgAssert(IsWindow());
        gtk_widget_destroy(GTK_WIDGET(m_hWnd));
        m_hWnd = NULL;
        return true;
    }
    
    // Attributes
    TMUInt32 GetStyle() const
    {
        DbgAssert(IsWindow());
        TMUInt32 dw = 0;
        if (gtk_widget_get_visible(GTK_WIDGET(m_hWnd)))
            dw |= WS_VISIBLE;
        if (!gtk_widget_get_sensitive(GTK_WIDGET(m_hWnd)))
            dw |= WS_DISABLED;
        if (IsIconic())
            dw |= WS_MINIMIZE;
        else if (IsZoomed())
            dw |= WS_MAXIMIZE;
        return dw;
    }
    
    TMUInt32 GetExStyle() const
    {
        DbgAssert(IsWindow());
        return 0;
    }
    
    void SetStyle(TMUInt32 dwStyle)
    {
        DbgAssert(IsWindow());
        if (dwStyle & WS_VISIBLE)
            gtk_widget_show(GTK_WIDGET(m_hWnd));
        else
            gtk_widget_hide(GTK_WIDGET(m_hWnd));
    }
    
    void SetExStyle(TMUInt32 dwStyle)
    {
        tm_unused(dwStyle);
        DbgAssert(IsWindow());
    }
    
    // LONG GetWindowLong(int nIndex) const
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(nIndex);
    //     return 0;
    // }
    
    // LONG SetWindowLong(int nIndex, LONG dwNewLong)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(nIndex);
    //     tm_unused(dwNewLong);
    //     return 0;
    // }
    
    // WORD GetWindowWord(int nIndex) const
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(nIndex);
    //     return 0;
    // }
    
    // WORD SetWindowWord(int nIndex, WORD wNewWord)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(nIndex);
    //     tm_unused(wNewWord);
    //     return 0;
    // }
    
    // Message Functions
    
    // Message Functions
    // SendMessage: Synchronous message - blocks until message is processed
    // Uses g_main_context_invoke for thread-safe sync dispatch
    
    TMResult SendMessage(TMUInt message, TMWParam wParam, TMLParam lParam)
    {
        DbgAssert(IsWindow());
        
        GObject* pGObj = G_OBJECT(m_hWnd);
        void* pUser = g_object_get_data(pGObj, "tm.user");
        
        // If no callback, try static dispatch through instance method
        if (!pUser || message < WM_USER)
        {
            // Standard window messages - can be handled without callback
            // For now, return 0 (would be handled by subclass virtual method in full implementation)
            return 0;
        }
        
        gpointer pCallback = g_object_get_data(pGObj, "tm.callback");
        
        // Allocate message structure
        _TMCustomMessage* msg = g_new0(_TMCustomMessage, 1);
        msg->hWnd = m_hWnd;
        msg->message = message;
        msg->wParam = wParam;
        msg->lParam = lParam;
        msg->pUser = pUser;
        msg->pCallback = pCallback;
        msg->pResult = g_new0(TMResult, 1);
        msg->is_sync = TRUE;
        msg->sync_mutex = g_mutex_new();
        msg->sync_cond = g_cond_new();
        
        // Send through main context (thread-safe)
        GMainContext* context = g_main_context_default();
        
        g_mutex_lock(msg->sync_mutex);
        {
            // Queue the message via idle handler
            g_main_context_invoke(context, _tm_message_handler_idle, msg);
            
            // Wait for message to be processed
            g_cond_wait(msg->sync_cond, msg->sync_mutex);
        }
        g_mutex_unlock(msg->sync_mutex);
        
        // Get result
        TMResult result = msg->pResult ? *msg->pResult : 0;
        
        // Cleanup
        g_free(msg->pResult);
        g_mutex_free(msg->sync_mutex);
        g_cond_free(msg->sync_cond);
        g_free(msg);
        
        return result;
    }
    
    // PostMessage: Asynchronous message - queues and returns immediately
    // Uses g_idle_add for async dispatch
    
    TMBool PostMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgAssert(IsWindow());
        
        GObject* pGObj = G_OBJECT(m_hWnd);
        void* pUser = g_object_get_data(pGObj, "tm.user");
        gpointer pCallback = g_object_get_data(pGObj, "tm.callback");
        
        // Allocate message structure
        _TMCustomMessage* msg = g_new0(_TMCustomMessage, 1);
        msg->hWnd = m_hWnd;
        msg->message = message;
        msg->wParam = wParam;
        msg->lParam = lParam;
        msg->pUser = pUser;
        msg->pCallback = pCallback;
        msg->pResult = NULL;  // No result for async
        msg->is_sync = FALSE;
        msg->sync_mutex = NULL;
        msg->sync_cond = NULL;
        
        // Queue the message via idle handler (will be processed when main loop is idle)
        g_idle_add(_tm_message_handler_idle, msg);
        
        return TMTrue;
    }

    // SendNotifyMessage: Like PostMessage but waits for processing on main thread
    
    TMBool SendNotifyMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgAssert(IsWindow());
        return PostMessage(message, wParam, lParam);
    }
    
    // Window Text Functions
    
    TMBool SetWindowText(TMCStr lpszString)
    {
        DbgAssert(IsWindow());
        gtk_window_set_title(GTK_WINDOW(m_hWnd), lpszString ? lpszString : "");
        return true;
    }
    
    int GetWindowText(TMStr lpszStringBuf, int nMaxCount) const
    {
        DbgAssert(IsWindow());
        tm_unused(lpszStringBuf);
        tm_unused(nMaxCount);
        return 0;
    }
    
    int GetWindowTextLength() const
    {
        DbgAssert(IsWindow());
        return 0;
    }
    
    // Font Functions
    
    // void SetFont(HFONT hFont, TMBool bRedraw = TMTrue)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(hFont);
    //     tm_unused(bRedraw);
    // }
    
    // HFONT GetFont() const
    // {
    //     DbgAssert(IsWindow());
    //     return 0;
    // }
    
    // Menu Functions (non-child windows only)
    
    // TMHMenu GetMenu() const
    // {
    //     DbgAssert(IsWindow());
    //     return 0;
    // }
    
    // TMBool SetMenu(TMHMenu hMenu)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(hMenu);
    //     return TMFalse;
    // }
    
    // TMBool DrawMenuBar()
    // {
    //     DbgAssert(IsWindow());
    //     return TMFalse;
    // }
    
    // GTK has no system menu handle; stubs for Win32 compat
    TMHMenu GetSystemMenu(TMBool bRevert) const
    {
        DbgAssert(IsWindow());
        tm_unused(bRevert);
        return NULL;
    }

    TMBool HiliteMenuItem(TMHMenu hMenu, TMUInt uItemHilite, TMUInt uHilite)
    {
        DbgAssert(IsWindow());
        tm_unused(hMenu);
        tm_unused(uItemHilite);
        tm_unused(uHilite);
        return TMFalse;
    }

    // Window Size and Position Functions

    TMBool IsIconic() const
    {
        DbgAssert(IsWindow());
        GdkWindow* gw = gtk_widget_get_window(GTK_WIDGET(m_hWnd));
        if (!gw) return TMFalse;
        return (gdk_window_get_state(gw) & GDK_WINDOW_STATE_ICONIFIED) != 0;
    }

    TMBool IsZoomed() const
    {
        DbgAssert(IsWindow());
        GdkWindow* gw = gtk_widget_get_window(GTK_WIDGET(m_hWnd));
        if (!gw) return TMFalse;
        return (gdk_window_get_state(gw) & GDK_WINDOW_STATE_MAXIMIZED) != 0;
    }

    TMBool MoveWindow(int x, int y, int nWidth, int nHeight, TMBool bRepaint = TMTrue)
    {
        DbgAssert(IsWindow());
        gtk_window_move(GTK_WINDOW(m_hWnd), x, y);
        gtk_window_resize(GTK_WINDOW(m_hWnd), nWidth > 0 ? nWidth : 1, nHeight > 0 ? nHeight : 1);
        if (bRepaint)
            gtk_widget_queue_draw(GTK_WIDGET(m_hWnd));
        return TMTrue;
    }

    TMBool MoveWindow(const TMRect* lpRect, TMBool bRepaint = TMTrue)
    {
        DbgAssert(IsWindow());
        if (!lpRect) return TMFalse;
        return MoveWindow(RectLeft(*lpRect), RectTop(*lpRect), RectWidth(*lpRect), RectHeight(*lpRect), bRepaint);
    }

    TMBool SetWindowPos(TMHWnd hWndInsertAfter, int x, int y, int cx, int cy, TMUInt nFlags)
    {
        DbgAssert(IsWindow());
        tm_unused(hWndInsertAfter);
        if (!(nFlags & SWP_NOMOVE))
            gtk_window_move(GTK_WINDOW(m_hWnd), x, y);
        if (!(nFlags & SWP_NOSIZE) && cx > 0 && cy > 0)
            gtk_window_resize(GTK_WINDOW(m_hWnd), cx, cy);
        if (!(nFlags & SWP_NOZORDER))
            gtk_window_present(GTK_WINDOW(m_hWnd));
        if (!(nFlags & SWP_NOREDRAW))
            gtk_widget_queue_draw(GTK_WIDGET(m_hWnd));
        return TMTrue;
    }

    TMBool SetWindowPos(TMHWnd hWndInsertAfter, const TMRect* lpRect, TMUInt nFlags)
    {
        DbgAssert(IsWindow());
        if (!lpRect) return TMFalse;
        return SetWindowPos(hWndInsertAfter,  RectLeft(*lpRect), RectTop(*lpRect),
            RectWidth(*lpRect), RectHeight(*lpRect), nFlags);
    }

    TMUInt ArrangeIconicWindows()
    {
        DbgAssert(IsWindow());
        return 0; // no GTK equivalent
    }

    TMBool BringWindowToTop()
    {
        DbgAssert(IsWindow());
        gtk_window_present(GTK_WINDOW(m_hWnd));
        return TMTrue;
    }

    TMBool GetWindowRect(TMRect* lpRect) const
    {
        DbgAssert(IsWindow());
        if (!lpRect) return TMFalse;
        int x = 0, y = 0, w = 0, h = 0;
        gtk_window_get_position(GTK_WINDOW(m_hWnd), &x, &y);
        gtk_window_get_size(GTK_WINDOW(m_hWnd), &w, &h);
        RectSetLeft(*lpRect, x);
        RectSetTop(*lpRect, y);
        RectSetWidth(*lpRect, w);
        RectSetHeight(*lpRect, h);
        return TMTrue;
    }

    TMBool GetClientRect(TMRect* lpRect) const
    {
        DbgAssert(IsWindow());
        if (!lpRect) return TMFalse;
        GtkWidget* w = GTK_WIDGET(m_hWnd);
        RectSetLeft(*lpRect, 0);
        RectSetTop(*lpRect, 0);
        RectSetWidth(*lpRect, gtk_widget_get_allocated_width(w));
        RectSetHeight(*lpRect, gtk_widget_get_allocated_height(w));
        return TMTrue;
    }

//     TMBool GetWindowPlacement(WINDOWPLACEMENT* lpwndpl) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(lpwndpl);
//         return TMFalse;
//     }
// 
//     TMBool SetWindowPlacement(const WINDOWPLACEMENT* lpwndpl)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(lpwndpl);
//         return TMFalse;
//     }

    // Coordinate Mapping Functions
    
    TMBool ClientToScreen(TMPoint* lpPoint) const
    {
        DbgAssert(IsWindow());
        int x = 0, y = 0;
        gtk_window_get_position(GTK_WINDOW(m_hWnd), &x, &y);
        lpPoint->x += x;
        lpPoint->y += y;
        return true;
    }
    
    TMBool ClientToScreen(TMRect* lpRect) const
    {
        DbgAssert(IsWindow());
        if (!ClientToScreen((TMPoint*)lpRect))
            return TMFalse;
        return ClientToScreen(((TMPoint*)lpRect)+1);
    }
    
    TMBool ScreenToClient(TMPoint* lpPoint) const
    {
        DbgAssert(IsWindow());
        int x = 0, y = 0;
        gtk_window_get_position(GTK_WINDOW(m_hWnd), &x, &y);
        lpPoint->x -= x;
        lpPoint->y -= y;
        return true;
    }
    
    TMBool ScreenToClient(TMRect* lpRect) const
    {
        DbgAssert(IsWindow());
        if (!ScreenToClient((TMPoint*)lpRect))
            return TMFalse;
        return ScreenToClient(((TMPoint*)lpRect)+1);
    }
    
//     int MapWindowPoints(TMHWnd hWndTo, TMPoint* lpPoint, TMUInt nCount) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(hWndTo);
//         tm_unused(lpPoint);
//         tm_unused(nCount);
//         return 0;
//     }
//     
//     int MapWindowPoints(TMHWnd hWndTo, TMRect* lpRect) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(hWndTo);
//         tm_unused(lpRect);
//         return 0;
//     }
    
    // Update and Painting Functions
    
    // TMHDC BeginPaint(LPPAINTSTRUCT lpPaint)
    // {
    //     DbgAssert(IsWindow());
    //     TMHDC hDC = GetDC();
    //     if (lpPaint != NULL)
    //     {
    //         memset(lpPaint, 0, sizeof(*lpPaint));
    //         lpPaint->hdc = hDC;
    //         GetClientRect(&lpPaint->rcPaint);
    //     }
    //     return hDC;
    // }
    
    // void EndPaint(LPPAINTSTRUCT lpPaint)
    // {
    //     DbgAssert(IsWindow());
    //     if (lpPaint != NULL && lpPaint->hdc != NULL)
    //     {
    //         cairo_destroy(lpPaint->hdc);
    //         lpPaint->hdc = NULL;
    //     }
    // }
    
    TMHDC GetDC()
    {
        DbgAssert(IsWindow());
        GtkWidget* widget = GTK_WIDGET(m_hWnd);
        if (!gtk_widget_get_realized(widget))
            gtk_widget_realize(widget);
        GdkWindow* window = gtk_widget_get_window(widget);
        if (window == NULL)
            return NULL;
        return gdk_cairo_create(window);
    }
    
    TMHDC GetWindowDC()
    {
        DbgAssert(IsWindow());
        return GetDC();
    }
    
    int ReleaseDC(TMHDC hDC)
    {
        DbgAssert(IsWindow());
        if (hDC == NULL)
            return 0;
        cairo_destroy(hDC);
        return 1;
    }
    
//     void Print(TMHDC hDC, TMUInt32 dwFlags) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(hDC);
//         tm_unused(dwFlags);
//     }
//     
//     void PrintClient(TMHDC hDC, TMUInt32 dwFlags) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(hDC);
//         tm_unused(dwFlags);
//     }
    
    TMBool UpdateWindow()
    {
        DbgAssert(IsWindow());
        gtk_widget_queue_draw(GTK_WIDGET(m_hWnd));
        return true;
    }
    
//     void SetRedraw(TMBool bRedraw = TMTrue)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(bRedraw);
//     }
//     
//     TMBool GetUpdateRect(TMRect* lpRect, TMBool bErase = TMFalse)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(lpRect);
//         tm_unused(bErase);
//         return TMFalse;
//     }
//     
//     int GetUpdateRgn(HRGN hRgn, TMBool bErase = TMFalse)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(hRgn);
//         tm_unused(bErase);
//         return ERROR;
//     }
    
    TMBool Invalidate(TMBool bErase = TMTrue)
    {
        tm_unused(bErase);
        DbgAssert(IsWindow());
        gtk_widget_queue_draw(GTK_WIDGET(m_hWnd));
        return true;
    }
    
    TMBool InvalidateRect(const TMRect* lpRect, TMBool bErase = TMTrue)
    {
        tm_unused(lpRect);
        tm_unused(bErase);
        DbgAssert(IsWindow());
        gtk_widget_queue_draw(GTK_WIDGET(m_hWnd));
        return true;
    }
    
    // TMBool ValidateRect(const TMRect* lpRect)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(lpRect);
    //     return TMTrue;
    // }
    
    // void InvalidateRgn(HRGN hRgn, TMBool bErase = TMTrue)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(hRgn);
    //     tm_unused(bErase);
    //     gtk_widget_queue_draw(GTK_WIDGET(m_hWnd));
    // }
    
    // TMBool ValidateRgn(HRGN hRgn)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(hRgn);
    //     return TMTrue;
    // }
    
    TMBool ShowWindow(int nCmdShow)
    {
        DbgAssert(IsWindow());
        switch (nCmdShow)
        {
        case SW_HIDE: gtk_widget_hide(GTK_WIDGET(m_hWnd)); return true;
        case SW_SHOWMINIMIZED: ShowMinimized(); return true;
        case SW_SHOWMAXIMIZED: ShowMaximized(); return true;
        case SW_SHOWNORMAL:
        case SW_RESTORE: ShowNormal(); return true;
        default: gtk_widget_show(GTK_WIDGET(m_hWnd)); return true;
        }
    }
    
    // test parent and parent's parent visible state.
    TMBool IsWindowVisible() const
    {
        DbgAssert(IsWindow());
        return gtk_widget_get_visible(GTK_WIDGET(m_hWnd));
    }
    
//     TMBool ShowOwnedPopups(TMBool bShow = TMTrue)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(bShow);
//         return TMFalse;
//     }
//     
//     TMHDC GetDCEx(HRGN hRgnClip, TMUInt32 flags)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(hRgnClip);
//         tm_unused(flags);
//         return 0;
//     }
//     
//     TMBool LockWindowUpdate(TMBool bLock = TMTrue)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(bLock);
//         return TMFalse;
//     }
    
//     TMBool RedrawWindow(const TMRect* lpRectUpdate = NULL, HRGN hRgnUpdate = NULL, TMUInt flags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(lpRectUpdate);
//         tm_unused(hRgnUpdate);
//         tm_unused(flags);
//         gtk_widget_queue_draw(GTK_WIDGET(m_hWnd));
//         return TMTrue;
//     }
    
    // Timer Functions
    
//     TMUInt SetTimer(TMUInt nIDEvent, TMUInt nElapse)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nIDEvent);
//         tm_unused(nElapse);
//         return 0;
//     }
//     
//     TMBool KillTimer(TMUInt nIDEvent)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nIDEvent);
//         return TMFalse;
//     }
    
    // Window State Functions
    
    TMBool IsWindowEnabled() const
    {
        DbgAssert(IsWindow());
        return gtk_widget_get_sensitive(GTK_WIDGET(m_hWnd));
    }
    
    TMBool EnableWindow(TMBool bEnable = TMTrue)
    {
        DbgAssert(IsWindow());
        gtk_widget_set_sensitive(GTK_WIDGET(m_hWnd), bEnable);
        return TMTrue;
    }
    
    TMHWnd SetActiveWindow()
    {
        DbgAssert(IsWindow());
        gtk_window_present(GTK_WINDOW(m_hWnd));
        return m_hWnd;
    }
    
    void SetCapture()
    {
        DbgAssert(IsWindow());
        gtk_grab_add(GTK_WIDGET(m_hWnd));
    }
    
    void SetFocus()
    {
        DbgAssert(IsWindow());
        gtk_widget_grab_focus(GTK_WIDGET(m_hWnd));
    }
    
    // Dialog-Box Item Functions
    
//     TMBool CheckDlgButton(int nIDButton, TMUInt nCheck)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nIDButton);
//         tm_unused(nCheck);
//         return TMFalse;
//     }
//     
//     TMBool CheckRadioButton(int nIDFirstButton, int nIDLastButton, int nIDCheckButton)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nIDFirstButton);
//         tm_unused(nIDLastButton);
//         tm_unused(nIDCheckButton);
//         return TMFalse;
//     }
//     
//     int DlgDirList(TMStr lpPathSpec, int nIDListBox, int nIDStaticPath, TMUInt nFileType)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(lpPathSpec);
//         tm_unused(nIDListBox);
//         tm_unused(nIDStaticPath);
//         tm_unused(nFileType);
//         return 0;
//     }
//     
//     int DlgDirListComboBox(TMStr lpPathSpec, int nIDComboBox, int nIDStaticPath, TMUInt nFileType)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(lpPathSpec);
//         tm_unused(nIDComboBox);
//         tm_unused(nIDStaticPath);
//         tm_unused(nFileType);
//         return 0;
//     }
//     
//     TMBool DlgDirSelect(TMStr lpString, int nCount, int nIDListBox)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(lpString);
//         tm_unused(nCount);
//         tm_unused(nIDListBox);
//         return TMFalse;
//     }
//     
//     TMBool DlgDirSelectComboBox(TMStr lpString, int nCount, int nIDComboBox)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(lpString);
//         tm_unused(nCount);
//         tm_unused(nIDComboBox);
//         return TMFalse;
//     }
//     
//     TMUInt GetDlgItemInt(int nID, TMBool* lpTrans = NULL, TMBool bSigned = TMTrue) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nID);
//         tm_unused(lpTrans);
//         tm_unused(bSigned);
//         return 0;
//     }
//     
//     TMUInt GetDlgItemText(int nID, TMStr lpStr, int nMaxCount) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nID);
//         tm_unused(lpStr);
//         tm_unused(nMaxCount);
//         return 0;
//     }
//     
//     TMBool GetDlgItemText(int nID, BSTR& bstrText) const
//     {
//         DbgAssert(IsWindow());
//         
//         TMHWnd hWndCtl = GetDlgItem(nID);
//         if (hWndCtl == NULL)
//             return TMFalse;
//         
//         return CWindowGtk(hWndCtl).GetWindowText(bstrText);
//     }
//     
//     TMHWnd GetNextDlgGroupItem(TMHWnd hWndCtl, TMBool bPrevious = TMFalse) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(hWndCtl);
//         tm_unused(bPrevious);
//         return 0;
//     }
//     
//     TMHWnd GetNextDlgTabItem(TMHWnd hWndCtl, TMBool bPrevious = TMFalse) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(hWndCtl);
//         tm_unused(bPrevious);
//         return 0;
//     }
//     
//     TMUInt IsDlgButtonChecked(int nIDButton) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nIDButton);
//         return 0;
//     }
//     
//     TMResult SendDlgItemMessage(int nID, TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nID);
//         tm_unused(message);
//         tm_unused(wParam);
//         tm_unused(lParam);
//         return 0;
//     }
//     
//     TMBool SetDlgItemInt(int nID, TMUInt nValue, TMBool bSigned = TMTrue)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nID);
//         tm_unused(nValue);
//         tm_unused(bSigned);
//         return TMFalse;
//     }
//     
//     TMBool SetDlgItemText(int nID, TMCStr lpszString)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nID);
//         tm_unused(lpszString);
//         return TMFalse;
//     }
//     
// #ifndef _ATL_NO_HOSTING
//     TMResult GetDlgControl(int nID, REFIID iid, void** ppUnk)
//     {
//         DbgAssert(IsWindow());
//         DbgAssert(ppUnk != NULL);
//         TMResult hr = E_FAIL;
//         TMHWnd hWndCtrl = GetDlgItem(nID);
//         if (hWndCtrl != NULL)
//         {
//             *ppUnk = NULL;
//             CComPtr<IUnknown> spUnk;
//             hr = AtlAxGetControl(hWndCtrl, &spUnk);
//             if (SUCCEEDED(hr))
//                 hr = spUnk->QueryInterface(iid, ppUnk);
//         }
//         return hr;
//     }
// #endif //!_ATL_NO_HOSTING
    
    // Scrolling Functions
    
//     int GetScrollPos(int nBar) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nBar);
//         return 0;
//     }
//     
//     TMBool GetScrollRange(int nBar, int* lpMinPos, int* lpMaxPos) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nBar);
//         tm_unused(lpMinPos);
//         tm_unused(lpMaxPos);
//         return TMFalse;
//     }
//     
//     TMBool ScrollWindow(int xAmount, int yAmount, const TMRect* lpRect = NULL, const TMRect* lpClipRect = NULL)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(xAmount);
//         tm_unused(yAmount);
//         tm_unused(lpRect);
//         tm_unused(lpClipRect);
//         return TMFalse;
//     }
//     
//     int ScrollWindowEx(int dx, int dy, const TMRect* lpRectScroll, const TMRect* lpRectClip, HRGN hRgnUpdate, TMRect* lpRectUpdate, TMUInt uFlags)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(dx);
//         tm_unused(dy);
//         tm_unused(lpRectScroll);
//         tm_unused(lpRectClip);
//         tm_unused(hRgnUpdate);
//         tm_unused(lpRectUpdate);
//         tm_unused(uFlags);
//         return 0;
//     }
//     
//     int ScrollWindowEx(int dx, int dy, TMUInt uFlags, const TMRect* lpRectScroll = NULL, const TMRect* lpRectClip = NULL, HRGN hRgnUpdate = NULL, TMRect* lpRectUpdate = NULL)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(dx);
//         tm_unused(dy);
//         tm_unused(uFlags);
//         tm_unused(lpRectScroll);
//         tm_unused(lpRectClip);
//         tm_unused(hRgnUpdate);
//         tm_unused(lpRectUpdate);
//         return 0;
//     }
//     
//     int SetScrollPos(int nBar, int nPos, TMBool bRedraw = TMTrue)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nBar);
//         tm_unused(nPos);
//         tm_unused(bRedraw);
//         return 0;
//     }
//     
//     TMBool SetScrollRange(int nBar, int nMinPos, int nMaxPos, TMBool bRedraw = TMTrue)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nBar);
//         tm_unused(nMinPos);
//         tm_unused(nMaxPos);
//         tm_unused(bRedraw);
//         return TMFalse;
//     }
//     
//     TMBool ShowScrollBar(TMUInt nBar, TMBool bShow = TMTrue)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nBar);
//         tm_unused(bShow);
//         return TMFalse;
//     }
//     
//     TMBool EnableScrollBar(TMUInt uSBFlags, TMUInt uArrowFlags = ESB_ENABLE_BOTH)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(uSBFlags);
//         tm_unused(uArrowFlags);
//         return TMFalse;
//     }
    
    // Window Access Functions
    
//     TMHWnd ChildWindowFromPoint(TMPoint point) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(point);
//         return 0;
//     }
//     
//     TMHWnd ChildWindowFromPointEx(TMPoint point, TMUInt uFlags) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(point);
//         tm_unused(uFlags);
//         return 0;
//     }
//     
//     TMHWnd GetTopWindow() const
//     {
//         DbgAssert(IsWindow());
//         return 0;
//     }
    
    TMHWnd GetWindow(TMUInt nCmd) const
    {
        DbgAssert(this->m_hWnd);
        switch (nCmd)
        {
        case GW_CHILD: return 0;
        case GW_OWNER: return _AsWindow(gtk_widget_get_toplevel(GTK_WIDGET(this->m_hWnd)));
        }
        return 0;
    }
    
//     TMHWnd GetLastActivePopup() const
//     {
//         DbgAssert(IsWindow());
//         return m_hWnd;
//     }
    
    TMBool IsChild(TMHWnd hWnd) const
    {
        DbgAssert(IsWindow());
        if (!hWnd)
            return false;
        GtkWindow* p = GTK_WINDOW(this->m_hWnd);
        while (p)
        {
            if (p == hWnd)
                return TMTrue;
            p = gtk_window_get_transient_for(p);
        }
        return TMFalse;
    }
    
    TMHWnd GetParent() const
    {
        DbgAssert(IsWindow());
        return gtk_window_get_transient_for(GTK_WINDOW(m_hWnd));
    }
    
    void SetParent(TMHWnd hWndNewParent)
    {
        DbgAssert(IsWindow());
        gtk_window_set_transient_for(GTK_WINDOW(m_hWnd), hWndNewParent);
    }
    
    // Window Tree Access
    
//     int GetDlgCtrlID() const
//     {
//         DbgAssert(IsWindow());
//         return 0;
//     }
//     
//     int SetDlgCtrlID(int nID)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nID);
//         return 0;
//     }
//     
//     TMHWnd GetDlgItem(int nID) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nID);
//         return 0;
//     }
    
    // Alert Functions
    
//     TMBool FlashWindow(TMBool bInvert)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(bInvert);
//         return TMFalse;
//     }
    
    int MessageBox(TMCStr lpszText, TMCStr lpszCaption = TM_T(""), TMUInt nType = MB_OK)
    {
        DbgAssert(IsWindow());
        tm_unused(nType);
        GtkWidget* dlg = gtk_message_dialog_new(GTK_WINDOW(m_hWnd), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", lpszText ? lpszText : "");
        gtk_window_set_title(GTK_WINDOW(dlg), lpszCaption ? lpszCaption : "");
        int rc = gtk_dialog_run(GTK_DIALOG(dlg));
        gtk_widget_destroy(dlg);
        return rc;
    }
    
    // Clipboard Functions
    
    // TMBool ChangeClipboardChain(TMHWnd hWndNewNext)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(hWndNewNext);
    //     return TMFalse;
    // }
    
    // TMHWnd SetClipboardViewer()
    // {
    //     DbgAssert(IsWindow());
    //     return 0;
    // }
    
    // TMBool OpenClipboard()
    // {
    //     DbgAssert(IsWindow());
    //     return TMFalse;
    // }
    
    // // Caret Functions
    
    // TMBool CreateCaret(HBITMAP hBitmap)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(hBitmap);
    //     return TMTrue;
    // }
    
    // BOOL CreateUnunsedCaret(int nWidth, int nHeight) throw()
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(nWidth);
    //     tm_unused(nHeight);
    //     return TMTrue;
    // }
    
    // TMBool CreateSolidCaret(int nWidth, int nHeight)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(nWidth);
    //     tm_unused(nHeight);
    //     return TMFalse;
    // }
    
    // TMBool CreateGrayCaret(int nWidth, int nHeight)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(nWidth);
    //     tm_unused(nHeight);
    //     return TMFalse;
    // }
    
    // TMBool HideCaret()
    // {
    //     DbgAssert(IsWindow());
    //     return TMTrue;
    // }
    
    // TMBool ShowCaret()
    // {
    //     DbgAssert(IsWindow());
    //     return TMTrue;
    // }
    
    // Drag-Drop Functions
//     void DragAcceptFiles(TMBool bAccept = TMTrue)
//     {
//         DbgAssert(IsWindow()); ::DragAcceptFiles(m_hWnd, bAccept);
//     }
    
    // Icon Functions
    
    TMHIcon SetIcon(TMHIcon hIcon, TMBool bBigIcon = TMTrue)
    {
        DbgAssert(IsWindow());
        tm_unused(hIcon);
        tm_unused(bBigIcon);
        return 0;
    }
    
    TMHIcon GetIcon(TMBool bBigIcon = TMTrue) const
    {
        DbgAssert(IsWindow());
        tm_unused(bBigIcon);
        return 0;
    }
    
    // Help Functions
    
//     TMBool WinHelp(TMCStr lpszHelp, TMUInt nCmd = HELP_CONTEXT, TMUInt32 dwData = 0)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(lpszHelp);
//         tm_unused(nCmd);
//         tm_unused(dwData);
//         return TMFalse;
//     }
//     
//     TMBool SetWindowContextHelpId(TMUInt32 dwContextHelpId)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(dwContextHelpId);
//         return TMFalse;
//     }
//     
//     TMUInt32 GetWindowContextHelpId() const
//     {
//         DbgAssert(IsWindow());
//         return 0;
//     }
    
    // Hot Key Functions
    
//     int SetHotKey(WORD wVirtualKeyCode, WORD wModifiers)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(wVirtualKeyCode);
//         tm_unused(wModifiers);
//         return 0;
//     }
//     
//     TMUInt32 GetHotKey() const
//     {
//         DbgAssert(IsWindow());
//         return 0;
//     }
    
    // Misc. Operations
    
    //N new
    // TMBool GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(nBar);
    //     tm_unused(lpScrollInfo);
    //     return TMFalse;
    // }
    // TMBool SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, TMBool bRedraw = TMTrue)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(nBar);
    //     tm_unused(lpScrollInfo);
    //     tm_unused(bRedraw);
    //     return TMFalse;
    // }

    // TMBool IsDialogMessage(LPMSG lpMsg)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(lpMsg);
    //     return TMFalse;
    // }
    
    void NextDlgCtrl() const
    {
        DbgAssert(IsWindow());
        gtk_widget_child_focus(GTK_WIDGET(m_hWnd), GTK_DIR_TAB_FORWARD);
    }
    
    void PrevDlgCtrl() const
    {
        DbgAssert(IsWindow());
        gtk_widget_child_focus(GTK_WIDGET(m_hWnd), GTK_DIR_TAB_BACKWARD);
    }
    
//     void GotoDlgCtrl(TMHWnd hWndCtrl) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(hWndCtrl);
//     }
    
    TMBool ResizeClient(int nWidth, int nHeight, TMBool bRedraw = TMTrue)
    {
        DbgAssert(IsWindow());
        tm_unused(bRedraw);
        TMRect rcWnd;
        if (!GetClientRect(&rcWnd))
            return TMFalse;
        if (nWidth != -1)
            RectSetWidth(rcWnd, nWidth);
        if (nHeight != -1)
            RectSetHeight(rcWnd, nHeight);
        return SetWindowPos(NULL, 0, 0, RectWidth(rcWnd), RectHeight(rcWnd), SWP_NOZORDER | SWP_NOMOVE);
    }
    
    // int GetWindowRgn(HRGN hRgn)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(hRgn);
    //     return ERROR;
    // }
    // int SetWindowRgn(HRGN hRgn, TMBool bRedraw = TMFalse)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(hRgn);
    //     tm_unused(bRedraw);
    //     return TMFalse;
    // }
    // HDWP DeferWindowPos(HDWP hWinPosInfo, TMHWnd hWndInsertAfter, int x, int y, int cx, int cy, TMUInt uFlags)
    // {
    //     DbgAssert(IsWindow());
    //     tm_unused(hWinPosInfo);
    //     SetWindowPos(hWndInsertAfter, x, y, cx, cy, uFlags);
    //     return 0;
    // }

    // TMUInt32 GetWindowThreadID()
    // {
    //     DbgAssert(IsWindow());
    //     return 0;
    // }
    // TMUInt32 GetWindowProcessID()
    // {
    //     DbgAssert(IsWindow());
    //     return 0;
    // }
    
    TMBool IsWindow() const
    {
        return 0 != m_hWnd;
    }
    
    // TMBool IsWindowUnicode()
    // {
    //     DbgAssert(IsWindow());
    //     return TMTrue;
    // }
    // TMBool IsParentDialog()
    // {
    //     DbgAssert(IsWindow());
    //     return TMFalse;
    // }
    TMBool ShowWindowAsync(int nCmdShow)
    {
        DbgAssert(IsWindow());
        return ShowWindow(nCmdShow);
    }
    
//     TMHWnd GetDescendantWindow(int nID) const
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nID);
//         return 0;
//     }
//     
//     void SendMessageToDescendants(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0, TMBool bDeep = TMTrue)
//     {
//         tm_unused(message);
//         tm_unused(wParam);
//         tm_unused(lParam);
//         tm_unused(bDeep);
//     }
    
    TMBool CenterWindow(TMHWnd hWndCenter = NULL)
    {
        DbgAssert(IsWindow());
        tm_unused(hWndCenter);
        gtk_window_set_position(GTK_WINDOW(m_hWnd), GTK_WIN_POS_CENTER);
        return TMTrue;
    }
    
    TMBool ModifyStyle(TMUInt32 dwRemove, TMUInt32 dwAdd, TMUInt nFlags = 0)
    {
        DbgAssert(IsWindow());
        tm_unused(dwRemove);
        tm_unused(dwAdd);
        tm_unused(nFlags);
        return TMTrue;
    }
    
    TMBool ModifyStyleEx(TMUInt32 dwRemove, TMUInt32 dwAdd, TMUInt nFlags = 0)
    {
        DbgAssert(IsWindow());
        tm_unused(dwRemove);
        tm_unused(dwAdd);
        tm_unused(nFlags);
        return TMTrue;
    }
    
//     TMBool GetWindowText(BSTR* pbstrText)
//     {
//         return GetWindowText(*pbstrText);
//     }
//     TMBool GetWindowText(BSTR& bstrText)
//     {
//         DbgAssert(IsWindow());
//         if (bstrText != NULL)
//         {
//             SysFreeString(bstrText);
//             bstrText = NULL;
//         }
//         
//         bstrText = ::SysAllocString(OLESTR(""));
//         return (bstrText != NULL) ? TMTrue : TMFalse;
//     }

    TMHWnd GetTopLevelParent() const
    {
        DbgAssert(IsWindow());
        return _AsWindow(gtk_widget_get_toplevel(GTK_WIDGET(m_hWnd)));
    }
    
    TMHWnd GetTopLevelWindow() const
    {
        DbgAssert(IsWindow());
        return _AsWindow(gtk_widget_get_toplevel(GTK_WIDGET(m_hWnd)));
    }
    
    
    // ext funciton
    void CenterWindow2(TMBool bCenterScreen, TMBool bCenterOwner)
    {
        _Monitor_CenterWindow(((CMonitor*)0), *this, bCenterScreen, bCenterOwner);
    }

    void Move(int x, int y)
    {
        DbgAssert(IsWindow());
        gtk_window_move(GTK_WINDOW(m_hWnd), x, y);
    }
    
    void Resize(int w, int h)
    {
        DbgAssert(IsWindow());
        gtk_widget_set_size_request(GTK_WIDGET(m_hWnd), w, h);
    }
    
    void ShowMinimized()
    {
        DbgAssert(IsWindow());
        gtk_window_iconify(GTK_WINDOW(m_hWnd));
    }
    
    void ShowMaximized()
    {
        DbgAssert(IsWindow());
        gtk_window_maximize(GTK_WINDOW(m_hWnd));
    }
    
    void ShowNormal()
    {
        DbgAssert(IsWindow());
        gtk_window_deiconify(GTK_WINDOW(m_hWnd));
        gtk_window_unmaximize(GTK_WINDOW(m_hWnd));
        gtk_widget_show(GTK_WIDGET(m_hWnd));
    }
    
    TMBool IsSelfVisible() const
    {
        DbgAssert(IsWindow());
        return gtk_widget_get_visible(GTK_WIDGET(m_hWnd));
    }
    
    TMBool IsActiveWindow() const
    {
        DbgAssert(IsWindow());
        return gtk_window_is_active(GTK_WINDOW(m_hWnd));
    }

    TMBool IsFocus()
    {
        return gtk_widget_has_focus(GTK_WIDGET(m_hWnd));
    }
    

//     void SetHideFocus()
//     {
//     }
//     
//     void ResetHideFocus()
//     {
//     }
//     
//     void ScreenToParent(TMRect& rc)
//     {
//         DbgAssert(IsWindow());
//     }
//     
//     void GetWindowRectToParent(TMRect& rc)
//     {
//         DbgAssert(IsWindow());
//         GetWindowRect(&rc);
//     }
//     
//     void EnableAreo(int nClient)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(nClient);
//     }
//     
//     TMBool EnableBlur(TMBool bEnable)
//     {
//         DbgAssert(IsWindow());
//         tm_unused(bEnable);
//         return false;
//     }
};



///////////////////////////////////////////////////////////////////////////////
// CIcon
template <bool t_bManaged>
class CIconGtkT
{
public:
    TMHIcon m_hIcon;

    //GTK_CLASS_CONSTRUCT(CIconGtkT, TMHIcon, m_hIcon);

    static TMHIcon _LoadPixbufFromBytes(const TMUInt8* bits, TMUInt32 TMSize, int cxDesired = 0, int cyDesired = 0)
    {
        if (!bits || TMSize == 0)
            return NULL;

        GdkPixbufLoader* loader = gdk_pixbuf_loader_new();
        if (!loader)
            return NULL;

        GError* err = NULL;
        gboolean ok = gdk_pixbuf_loader_write(loader, bits, TMSize, &err);
        if (ok)
            ok = gdk_pixbuf_loader_close(loader, &err);
        if (!ok)
        {
            if (err)
                g_error_free(err);
            g_object_unref(loader);
            return NULL;
        }

        GdkPixbuf* pix = gdk_pixbuf_loader_get_pixbuf(loader);
        if (!pix)
        {
            g_object_unref(loader);
            return NULL;
        }
        g_object_ref(pix);
        g_object_unref(loader);

        if (cxDesired > 0 || cyDesired > 0)
        {
            int w = gdk_pixbuf_get_width(pix);
            int h = gdk_pixbuf_get_height(pix);
            int tw = (cxDesired > 0) ? cxDesired : w;
            int th = (cyDesired > 0) ? cyDesired : h;
            GdkPixbuf* scaled = gdk_pixbuf_scale_simple(pix, tw, th, GDK_INTERP_BILINEAR);
            g_object_unref(pix);
            pix = scaled;
        }
        return pix;
    }

    static TMHIcon _LoadThemeIconByName(const char* name, int TMSize)
    {
        if (!name || !*name)
            return NULL;
        GtkIconTheme* theme = gtk_icon_theme_get_default();
        if (!theme)
            return NULL;
        GError* err = NULL;
        GdkPixbuf* pix = gtk_icon_theme_load_icon(theme, name, TMSize, GTK_ICON_LOOKUP_USE_BUILTIN, &err);
        if (err)
            g_error_free(err);
        return pix;
    }

    typedef const char* icon_id;
    static icon_id icon_APPLICATION()   { return "application-x-executable"; }
    static icon_id icon_ASTERISK()      { return "dialog-information"; }
    static icon_id icon_EXCLAMATION()   { return "dialog-warning"; }
    static icon_id icon_HAND()          { return "dialog-error"; }
    static icon_id icon_IDI_QUESTION()  { return "help-browser"; }
    static icon_id icon_WINLOGO()       { return "start-here"; }

    static GdkPixbuf* _LoadPixbufFromPath(TMCStr path, int cxDesired = 0, int cyDesired = 0)
    {
        if (!path)
            return NULL;

        GError* err = NULL;
        GdkPixbuf* p = NULL;
        if (cxDesired > 0 || cyDesired > 0)
            p = gdk_pixbuf_new_from_file_at_scale(path, cxDesired, cyDesired, TMTrue, &err);
        else
            p = gdk_pixbuf_new_from_file(path, &err);
        if (err)
            g_error_free(err);
        return p;
    }

public:
    // Constructor/destructor/operators
    CIconGtkT(TMHIcon hIcon = 0)
        : m_hIcon(hIcon)
    {
        if (m_hIcon && t_bManaged)
            g_object_ref(m_hIcon);
    }

    ~CIconGtkT()
    {
        if (t_bManaged && m_hIcon != NULL)
            DestroyIcon();
    }

    CIconGtkT<t_bManaged>& operator = (TMHIcon hIcon)
    {
        Attach(hIcon);
        return *this;
    }

    void Attach(TMHIcon hIcon)
    {
        TMHIcon p = hIcon;
        if (m_hIcon == p)
            return;

        if (t_bManaged && m_hIcon != NULL)
            g_object_unref(m_hIcon);
        m_hIcon = p;
        if (t_bManaged && m_hIcon != NULL)
            g_object_ref(m_hIcon);
    }

    TMHIcon Detach()
    {
        TMHIcon p = m_hIcon;
        m_hIcon = NULL;
        return p;
    }

    operator TMHIcon() const { return m_hIcon; }

    bool IsNull() const { return (m_hIcon == NULL); }
    operator bool() const { return !IsNull(); }

    // Create/destroy methods
    TMHIcon LoadIcon(TMCStr icon)
    {
        DbgAssert(m_hIcon);
        m_hIcon = _LoadPixbufFromPath(icon);
        return m_hIcon;
    }

    TMHIcon LoadIcon(TMCStr icon, int cxDesired, int cyDesired, TMUInt fuLoad = 0)
    {
        DbgAssert(m_hIcon);
        tm_unused(fuLoad);
        m_hIcon = _LoadPixbufFromPath(icon, cxDesired, cyDesired);
        return m_hIcon;
    }

    TMHIcon LoadOEMIcon(TMCStr name)
    {
        DbgAssert(m_hIcon);
        m_hIcon = _LoadThemeIconByName(name, 32);
        return m_hIcon;
    }

    TMHIcon CreateIcon(int nWidth, int nHeight, TMUInt8 cPlanes, TMUInt8 cBitsPixel, const TMUInt8* lpbANDbits, const TMUInt8 *lpbXORbits)
    {
        DbgAssert(m_hIcon);
        tm_unused(lpbANDbits);
        if (!lpbXORbits || nWidth <= 0 || nHeight <= 0 || cPlanes != 1 || cBitsPixel != 32)
            return NULL;

        m_hIcon = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TMTrue, 8, nWidth, nHeight);
        if (!m_hIcon)
            return NULL;

        int stride = gdk_pixbuf_get_rowstride(m_hIcon);
        TMUInt8* dst = gdk_pixbuf_get_pixels(m_hIcon);
        int srcStride = nWidth * 4;
        for (int y = 0; y < nHeight; ++y)
            memcpy(dst + y * stride, lpbXORbits + y * srcStride, srcStride);
        return m_hIcon;
    }

    TMHIcon CreateIconFromResource(const TMUInt8* pBits, TMUInt32 dwResSize, TMUInt32 dwVersion = 0x00030000)
    {
        DbgAssert(m_hIcon);
        tm_unused(dwVersion);
        m_hIcon = _LoadPixbufFromBytes(pBits, dwResSize);
        return m_hIcon;
    }

//     TMHIcon CreateIconFromResourceEx(const TMUInt8* pbBits, TMUInt32 cbBits, TMUInt32 dwVersion = 0x00030000, int cxDesired = 0, int cyDesired = 0, TMUInt uFlags = LR_DEFAULTCOLOR)
//     {
//         DbgAssert(m_hIcon);
//         tm_unused(dwVersion);
//         tm_unused(uFlags);
//         m_hIcon = _LoadPixbufFromBytes(pbBits, cbBits, cxDesired, cyDesired);
//         return m_hIcon;
//     }
// 
//     TMHIcon CreateIconIndirect(PICONINFO pIconInfo)
//     {
//         tm_unused(pIconInfo);
//         return NULL;
//     }

    TMHIcon ExtractIcon(TMCStr lpszExeFileName, TMUInt nIconIndex)
    {
        tm_unused(nIconIndex);
        DbgAssert(m_hIcon);
        m_hIcon = _LoadPixbufFromPath(lpszExeFileName);
        return m_hIcon;
    }

//     TMHIcon ExtractAssociatedIcon(HINSTANCE hInst, TMStr lpIconPath, LPWORD lpiIcon)
//     {
//         tm_unused(hInst);
//         tm_unused(lpiIcon);
//         DbgAssert(m_hIcon);
//         m_hIcon = _LoadPixbufFromPath(lpIconPath);
//         return m_hIcon;
//     }

    TMBool DestroyIcon()
    {
        if (m_hIcon == NULL)
            return TMFalse;
        g_object_unref(m_hIcon);
        m_hIcon = NULL;
        return TMTrue;
    }

    // Operations
    TMHIcon CopyIcon()
    {
        DbgAssert(m_hIcon);
        TMHIcon copy = gdk_pixbuf_copy(m_hIcon);
        return copy;
    }

    TMHIcon DuplicateIcon()
    {
        return CopyIcon();
    }

    TMBool DrawIcon(TMHDC hDC, int x, int y)
    {
        DbgAssert(m_hIcon);
        if (m_hIcon == NULL || hDC == NULL)
            return TMFalse;
        gdk_cairo_set_source_pixbuf(hDC, m_hIcon, x, y);
        cairo_paint(hDC);
        return TMTrue;
    }

    TMBool DrawIcon(TMHDC hDC, TMPoint pt)
    {
        return DrawIcon(hDC, pt.x, pt.y);
    }

    TMBool DrawIconEx(TMHDC hDC, int x, int y, int cxWidth, int cyWidth, TMUInt uStepIfAniCur = 0, TMHBrush hbrFlickerFreeDraw = NULL, TMUInt uFlags = DI_NORMAL)
    {
        tm_unused(uStepIfAniCur);
        tm_unused(hbrFlickerFreeDraw);
        tm_unused(uFlags);
        DbgAssert(m_hIcon);
        if (m_hIcon == NULL || hDC == NULL)
            return TMFalse;

        GdkPixbuf* draw = m_hIcon;
        if (cxWidth > 0 && cyWidth > 0)
        {
            draw = gdk_pixbuf_scale_simple(m_hIcon, cxWidth, cyWidth, GDK_INTERP_BILINEAR);
            if (draw == NULL)
                return TMFalse;
        }

        gdk_cairo_set_source_pixbuf(hDC, draw, x, y);
        cairo_paint(hDC);

        if (draw != m_hIcon)
            g_object_unref(draw);
        return TMTrue;
    }

    TMBool DrawIconEx(TMHDC hDC, TMPoint pt, TMSize TMSize, TMUInt uStepIfAniCur = 0, TMHBrush hbrFlickerFreeDraw = NULL, TMUInt uFlags = DI_NORMAL)
    {
        return DrawIconEx(hDC, pt.x, pt.y, TMSize.cx, TMSize.cy, uStepIfAniCur, hbrFlickerFreeDraw, uFlags);
    }

//     TMBool GetIconInfo(PICONINFO pIconInfo) const
//     {
//         DbgAssert(pIconInfo != NULL);
//         DbgAssert(m_hIcon);
//         if (!pIconInfo || !m_hIcon)
//             return TMFalse;
//         pIconInfo->fIcon = TMTrue;
//         pIconInfo->xHotspot = 0;
//         pIconInfo->yHotspot = 0;
//         pIconInfo->hbmMask = NULL;
//         pIconInfo->hbmColor = NULL;
//         return TMTrue;
//     }
// 
//     TMBool GetIconInfoEx(PICONINFOEX pIconInfo) const
//     {
//         DbgAssert(pIconInfo != NULL);
//         DbgAssert(m_hIcon);
//         if (!pIconInfo || !m_hIcon)
//             return TMFalse;
//         pIconInfo->fIcon = TMTrue;
//         pIconInfo->xHotspot = 0;
//         pIconInfo->yHotspot = 0;
//         pIconInfo->hbmMask = NULL;
//         pIconInfo->hbmColor = NULL;
//         pIconInfo->wResID = 0;
//         pIconInfo->szModName[0] = 0;
//         pIconInfo->szResName[0] = 0;
//         return TMTrue;
//     }

    TMResult LoadIconMetric(TMCStr icon, int lims)
    {
        if (LoadIcon(icon, lims, lims) == NULL)
            return -1;
        return 0;
    }

    TMResult LoadIconWithScaleDown(TMCStr icon, int cx, int cy)
    {
        if (LoadIcon(icon, cx, cy) == NULL)
            return -1;
        return 0;
    }

    TMResult LoadOEMIconMetric(TMCStr lpstrIconName, int lims)
    {
        if (LoadOEMIcon(lpstrIconName) == NULL)
            return -1;
        if (lims > 0)
        {
            GdkPixbuf* scaled = gdk_pixbuf_scale_simple(m_hIcon, lims, lims, GDK_INTERP_BILINEAR);
            if (!scaled)
                return -1;
            g_object_unref(m_hIcon);
            m_hIcon = scaled;
        }
        return 0;
    }

    TMResult LoadOEMIconWithScaleDown(TMCStr lpstrIconName, int cx, int cy)
    {
        if (LoadOEMIcon(lpstrIconName) == NULL)
            return -1;
        int w = (cx > 0) ? cx : gdk_pixbuf_get_width(m_hIcon);
        int h = (cy > 0) ? cy : gdk_pixbuf_get_height(m_hIcon);
        GdkPixbuf* scaled = gdk_pixbuf_scale_simple(m_hIcon, w, h, GDK_INTERP_BILINEAR);
        if (!scaled)
            return -1;
        g_object_unref(m_hIcon);
        m_hIcon = scaled;
        return 0;
    }

    // Helper
//     static bool IsOEMIcon(TMCStr lpstrIconName)
//     {
//         return _MapOEMIconName(lpstrIconName) != NULL;
//     }
};

typedef CIconGtkT<false>   CIconHandleGtk;
typedef CIconGtkT<true>    CIconGtk;


///////////////////////////////////////////////////////////////////////////////
// CCursor

// protect template member from a winuser.h macro
#ifdef CopyCursor
#undef CopyCursor
#endif

template <bool t_bManaged>
class CCursorGtkT
{
public:
    TMHCursor m_hCursor;

    //GTK_CLASS_CONSTRUCT(CCursorGtkT, TMHCursor, m_hCursor);

    typedef TMCStr cursor_id;
    
    static cursor_id cursorARROW()        { return "default"; }
    static cursor_id cursorIBEAM()        { return "text"; }
    static cursor_id cursorWAIT()         { return "wait"; }
    static cursor_id cursorCROSS()        { return "crosshair"; }
    static cursor_id cursorUPARROW()      { return "n-resize"; }
    static cursor_id cursorSIZENWSE()     { return "nwse-resize"; }
    static cursor_id cursorSIZENESW()     { return "nesw-resize"; }
    static cursor_id cursorSIZEWE()       { return "ew-resize"; }
    static cursor_id cursorSIZENS()       { return "ns-resize"; }
    static cursor_id cursorSIZEALL()      { return "move"; }
    static cursor_id cursorNO()           { return "not-allowed"; }
    static cursor_id cursorHAND()         { return "pointer"; }
    static cursor_id cursorAPPSTARTING()  { return "progress"; }
    static cursor_id cursorHELP()         { return "help"; }
    static cursor_id cursorSPLITV()       { return cursorSIZEWE(); }
    static cursor_id cursorSPLITH()       { return cursorSIZENS(); }

    static GdkPixbuf* _LoadPixbufFromPath(TMCStr path)
    {
        if (!path)
            return NULL;

        GError* err = NULL;
        GdkPixbuf* p = gdk_pixbuf_new_from_file(path, &err);
        if (err)
            g_error_free(err);
        return p;
    }

    static GdkPixbuf* _LoadPixbufFromBytes(const TMUInt8* bits, TMUInt32 TMSize)
    {
        if (!bits || TMSize == 0)
            return NULL;

        GdkPixbufLoader* loader = gdk_pixbuf_loader_new();
        if (!loader)
            return NULL;

        GError* err = NULL;
        gboolean ok = gdk_pixbuf_loader_write(loader, bits, TMSize, &err);
        if (ok)
            ok = gdk_pixbuf_loader_close(loader, &err);
        if (!ok)
        {
            if (err)
                g_error_free(err);
            g_object_unref(loader);
            return NULL;
        }

        GdkPixbuf* pix = gdk_pixbuf_loader_get_pixbuf(loader);
        if (!pix)
        {
            g_object_unref(loader);
            return NULL;
        }
        g_object_ref(pix);
        g_object_unref(loader);
        return pix;
    }

    static TMHCursor _CreateCursorFromPixbuf(GdkPixbuf* pix, int xHotSpot = 0, int yHotSpot = 0)
    {
        if (!pix)
            return NULL;

        GdkDisplay* display = gdk_display_get_default();
        if (!display)
            return NULL;

        return gdk_cursor_new_from_pixbuf(display, pix, xHotSpot, yHotSpot);
    }
    
    // Constructor/destructor/operators
    CCursorGtkT(TMHCursor hCursor = NULL) : m_hCursor(hCursor)
    { }
    
    ~CCursorGtkT()
    {
        if (t_bManaged && m_hCursor != NULL)
            DestroyCursor();
    }
    
    CCursorGtkT<t_bManaged>& operator =(TMHCursor hCursor)
    {
        Attach(hCursor);
        return *this;
    }
    
    void Attach(TMHCursor hCursor)
    {
        if (m_hCursor == hCursor)
            return;

        if (t_bManaged && m_hCursor != NULL)
            g_object_unref(m_hCursor);
        m_hCursor = hCursor;
        if (t_bManaged && m_hCursor != NULL)
            g_object_ref(m_hCursor);
    }
    
    TMHCursor Detach()
    {
        TMHCursor hCursor = m_hCursor;
        m_hCursor = NULL;
        return hCursor;
    }
    
    operator TMHCursor() const { return m_hCursor; }
    
    bool IsNull() const { return m_hCursor == NULL; }
    
    // Create/destroy methods
    // TMHCursor LoadCursor(ATL::_U_STRINGorID cursor)
    // {
    //     DbgAssert(!m_hCursor);
    //     // GTK side: treat resource id/string as system cursor id when possible.
    //     m_hCursor = LoadSysCursor(cursor.m_lpstr);
    //     return m_hCursor;
    // }
    
    TMHCursor LoadSysCursor(cursor_id lpstrCursorName)
    {
        DbgAssert(!m_hCursor);
        GdkDisplay* display = gdk_display_get_default();
        if (!display)
            return NULL;
        m_hCursor = gdk_cursor_new_from_name(display, lpstrCursorName);
        return m_hCursor;
    }
    
    // deprecated
    TMHCursor LoadOEMCursor(TMCStr lpstrCursorName)
    {
        return LoadSysCursor(lpstrCursorName);
    }
    
    TMHCursor LoadCursor(cursor_id cursor, int cxDesired, int cyDesired, TMUInt fuLoad = 0)
    {
        DbgAssert(!m_hCursor);
        tm_unused(fuLoad);
        if (!cursor || !*cursor)
            return NULL;

        GdkPixbuf* pix = NULL;
        GError* err = NULL;
        if (cxDesired > 0 || cyDesired > 0)
            pix = gdk_pixbuf_new_from_file_at_scale(cursor, cxDesired, cyDesired, TMTrue, &err);
        else
            pix = gdk_pixbuf_new_from_file(cursor, &err);
        if (err)
            g_error_free(err);
        if (!pix)
            return NULL;

        m_hCursor = _CreateCursorFromPixbuf(pix, 0, 0);
        g_object_unref(pix);
        return m_hCursor;
    }
    
    TMHCursor LoadCursorFromFile(TMCStr pstrFilename)
    {
        DbgAssert(!m_hCursor);
        GdkPixbuf* pix = _LoadPixbufFromPath(pstrFilename);
        if (!pix)
            return NULL;
        m_hCursor = _CreateCursorFromPixbuf(pix, 0, 0);
        g_object_unref(pix);
        return m_hCursor;
    }

    TMHCursor CreateCursor(int xHotSpot, int yHotSpot, int nWidth, int nHeight, const void *pvANDPlane, const void *pvXORPlane)
    {
        DbgAssert(!m_hCursor);
        tm_unused(pvANDPlane);
        if (!pvXORPlane || nWidth <= 0 || nHeight <= 0)
            return NULL;

        GdkPixbuf* pix = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TMTrue, 8, nWidth, nHeight);
        if (!pix)
            return NULL;

        int stride = gdk_pixbuf_get_rowstride(pix);
        TMUInt8* dst = gdk_pixbuf_get_pixels(pix);
        int srcStride = nWidth * 4;
        const TMUInt8* src = static_cast<const TMUInt8*>(pvXORPlane);
        for (int y = 0; y < nHeight; ++y)
            memcpy(dst + y * stride, src + y * srcStride, srcStride);

        m_hCursor = _CreateCursorFromPixbuf(pix, xHotSpot, yHotSpot);
        g_object_unref(pix);
        return m_hCursor;
    }

    TMHCursor CreateCursorFromResource(const TMUInt8* pBits, TMUInt32 dwResSize, TMUInt32 dwVersion = 0x00030000)
    {
        DbgAssert(!m_hCursor);
        tm_unused(dwVersion);
        GdkPixbuf* pix = _LoadPixbufFromBytes(pBits, dwResSize);
        if (!pix)
            return NULL;
        m_hCursor = _CreateCursorFromPixbuf(pix, 0, 0);
        g_object_unref(pix);
        return m_hCursor;
    }
    
//     TMHCursor CreateCursorFromResourceEx(const TMUInt8* pbBits, TMUInt32 cbBits, TMUInt32 dwVersion = 0x00030000, int cxDesired = 0, int cyDesired = 0, TMUInt uFlags = LR_DEFAULTCOLOR)
//     {
//         DbgAssert(!m_hCursor);
//         tm_unused(dwVersion);
//         tm_unused(uFlags);
//         GdkPixbuf* pix = _LoadPixbufFromBytes(pbBits, cbBits);
//         if (!pix)
//             return NULL;
// 
//         if (cxDesired > 0 || cyDesired > 0)
//         {
//             int tw = (cxDesired > 0) ? cxDesired : gdk_pixbuf_get_width(pix);
//             int th = (cyDesired > 0) ? cyDesired : gdk_pixbuf_get_height(pix);
//             GdkPixbuf* scaled = gdk_pixbuf_scale_simple(pix, tw, th, GDK_INTERP_BILINEAR);
//             g_object_unref(pix);
//             pix = scaled;
//             if (!pix)
//                 return NULL;
//         }
// 
//         m_hCursor = _CreateCursorFromPixbuf(pix, 0, 0);
//         g_object_unref(pix);
//         return m_hCursor;
//     }

    TMBool DestroyCursor()
    {
        if (m_hCursor == NULL)
            return TMFalse;
        g_object_unref(m_hCursor);
        m_hCursor = NULL;
        return TMTrue;
    }
    
    // Operations
    TMHCursor CopyCursor()
    {
        if (!m_hCursor)
            return NULL;
        g_object_ref(m_hCursor);
        return m_hCursor;
    }

//     TMBool GetCursorInfo(LPCURSORINFO pCursorInfo)
//     {
//         DbgAssert(pCursorInfo);
//         if (pCursorInfo->cbSize >= sizeof(CURSORINFO))
//         {
//             pCursorInfo->flags = m_hCursor ? CURSOR_SHOWING : 0;
//             pCursorInfo->hCursor = (HCURSOR)m_hCursor;
//             pCursorInfo->ptScreenPos.x = 0;
//             pCursorInfo->ptScreenPos.y = 0;
//         }
//         return m_hCursor != NULL;
//     }
};

typedef CCursorGtkT<false>   CCursorHandleGtk;
typedef CCursorGtkT<true>    CCursorGtk;



///////////////////////////////////////////////////////////////////////////////
// CDC - The device context class (GTK/Cairo implementation)
// TMHDC = cairo_t*  (see tmnative.h TM_GTK block)
template <bool t_bManaged>
class CDCGtkT
{
public:
    // Data members
    TMHDC m_hDC;
    // TMColor m_clrText;
    // TMColor m_clrBk;
    // TMPoint m_ptCur;
    // TMUInt m_uTextAlign;

    //GTK_CLASS_CONSTRUCT(CDCGtkT, TMHDC, m_hDC);

    // Constructor/destructor/operators
    CDCGtkT(TMHDC hDC = NULL)
        : m_hDC(hDC)
        // , m_clrText(TMRGB(0, 0, 0))
        // , m_clrBk(TMRGB(255, 255, 255))
        // , m_ptCur(0, 0)
        // , m_uTextAlign(0)
    {
    }

    ~CDCGtkT()
    {
        if (t_bManaged && m_hDC != NULL)
            cairo_destroy(m_hDC);
        m_hDC = NULL;
    }

    CDCGtkT<t_bManaged>& operator =(TMHDC hDC)
    {
        Attach(hDC);
        return *this;
    }

    void Attach(TMHDC hDC)
    {
        if (t_bManaged && m_hDC != NULL && m_hDC != hDC)
            cairo_destroy(m_hDC);
        m_hDC = hDC;
    }
    
    TMHDC Detach()
    {
        TMHDC hDC = m_hDC;
        m_hDC = NULL;
        return hDC;
    }
    
    operator TMHDC() const { return m_hDC; }
    
    bool IsNull() const { return (m_hDC == NULL); }
    
// Operations
    /*
    TMHWnd WindowFromDC() const
    {
        DbgAssert(m_hDC);
        return NULL;
    }

      /*
    CPenHandle GetCurrentPen() const
    {
        DbgAssert(m_hDC != NULL);
        return CPenHandle((HPEN)::GetCurrentObject(m_hDC, OBJ_PEN));
    }

    CBrushHandle GetCurrentBrush() const
    {
        DbgAssert(m_hDC != NULL);
        return CBrushHandle((TMHBrush)::GetCurrentObject(m_hDC, OBJ_BRUSH));
    }

    CPaletteHandle GetCurrentPalette() const
    {
        DbgAssert(m_hDC != NULL);
        return CPaletteHandle((HPALETTE)::GetCurrentObject(m_hDC, OBJ_PAL));
    }

    CFontHandle GetCurrentFont() const
    {
        DbgAssert(m_hDC != NULL);
        return CFontHandle((HFONT)::GetCurrentObject(m_hDC, OBJ_FONT));
    }

    CBitmapHandle GetCurrentBitmap() const
    {
        DbgAssert(m_hDC != NULL);
        return CBitmapHandle((HBITMAP)::GetCurrentObject(m_hDC, OBJ_BITMAP));
    }

    TMHDC CreateDC(TMCStr lpszDriverName, TMCStr lpszDeviceName, TMCStr lpszOutput, const DEVMODE* lpInitData)
    {
        DbgAssert(m_hDC == NULL);
        m_hDC = ::CreateDC(lpszDriverName, lpszDeviceName, lpszOutput, lpInitData);
        return m_hDC;
    }

    TMHDC CreateCompatibleDC(TMHDC hDC = NULL)
    {
        DbgAssert(m_hDC == NULL);
        m_hDC = ::CreateCompatibleDC(hDC);
        return m_hDC;
    }*/

    TMBool DeleteDC()
    {
        if (m_hDC == NULL)
            return TMFalse;
        if (t_bManaged)
            cairo_destroy(m_hDC);
        m_hDC = NULL;
        return TMTrue;
    }
    
// Device-Context Functions
    int SaveDC()
    {
        DbgAssert(m_hDC);
        cairo_save(m_hDC);
        return 1;
    }

    TMBool RestoreDC(int nSavedDC)
    {
        tm_unused(nSavedDC);
        DbgAssert(m_hDC);
        cairo_restore(m_hDC);
        return TMTrue;
    }

    /*
    int GetDeviceCaps(int nIndex) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetDeviceCaps(m_hDC, nIndex);
    }
    
    TMUInt SetBoundsRect(LPCRECT lpRectBounds, TMUInt flags)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetBoundsRect(m_hDC, lpRectBounds, flags);
    }
    
    TMUInt GetBoundsRect(LPRECT lpRectBounds, TMUInt flags) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetBoundsRect(m_hDC, lpRectBounds, flags);
    }
    
    TMBool ResetDC(const DEVMODE* lpDevMode)
    {
        DbgAssert(m_hDC != NULL);
        return ::ResetDC(m_hDC, lpDevMode) != NULL;
    }
    
    // Drawing-Tool Functions
    TMBool GetBrushOrg(LPPOINT lpPoint) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetBrushOrgEx(m_hDC, lpPoint);
    }
    
    TMBool SetBrushOrg(int x, int y, LPPOINT lpPoint = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetBrushOrgEx(m_hDC, x, y, lpPoint);
    }
    
    TMBool SetBrushOrg(POINT point, LPPOINT lpPointRet = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetBrushOrgEx(m_hDC, point.x, point.y, lpPointRet);
    }

#ifndef _WIN32_WCE
    int EnumObjects(int nObjectType, int (CALLBACK* lpfn)(void*, TMLParam), TMLParam lpData)
    {
        DbgAssert(m_hDC != NULL);
#ifdef STRICT
        return ::EnumObjects(m_hDC, nObjectType, (GOBJENUMPROC)lpfn, lpData);
#else
        return ::EnumObjects(m_hDC, nObjectType, (GOBJENUMPROC)lpfn, (void*)lpData);
#endif
    }
#endif // !_WIN32_WCE
    
    // Type-safe selection helpers
    HPEN SelectPen(HPEN hPen)
    {
        DbgAssert(m_hDC != NULL);
#ifndef _WIN32_WCE
        DbgAssert(hPen == NULL || ::GetObjectType(hPen) == OBJ_PEN || ::GetObjectType(hPen) == OBJ_EXTPEN);
#else // CE specific
        DbgAssert(hPen == NULL || ::GetObjectType(hPen) == OBJ_PEN);
#endif // _WIN32_WCE
        return (HPEN)::SelectObject(m_hDC, hPen);
    }
    
    TMHBrush SelectBrush(TMHBrush hBrush)
    {
        DbgAssert(m_hDC != NULL);
        DbgAssert(hBrush == NULL || ::GetObjectType(hBrush) == OBJ_BRUSH);
        return (TMHBrush)::SelectObject(m_hDC, hBrush);
    }
    
    HFONT SelectFont(HFONT hFont)
    {
        DbgAssert(m_hDC != NULL);
        DbgAssert(hFont == NULL || ::GetObjectType(hFont) == OBJ_FONT);
        return (HFONT)::SelectObject(m_hDC, hFont);
    }
    
    HBITMAP SelectBitmap(HBITMAP hBitmap)
    {
        DbgAssert(m_hDC != NULL);
        DbgAssert(hBitmap == NULL || ::GetObjectType(hBitmap) == OBJ_BITMAP);
        return (HBITMAP)::SelectObject(m_hDC, hBitmap);
    }
    
    int SelectRgn(HRGN hRgn)       // special return for regions
    {
        DbgAssert(m_hDC != NULL);
        DbgAssert(hRgn == NULL || ::GetObjectType(hRgn) == OBJ_REGION);
        return PtrToInt(::SelectObject(m_hDC, hRgn));
    }
    
    // Type-safe selection helpers for stock objects
    HPEN SelectStockPen(int nPen)
    {
        DbgAssert(m_hDC != NULL);
#if (_WIN32_WINNT >= 0x0500)
        DbgAssert(nPen == WHITE_PEN || nPen == BLACK_PEN || nPen == NULL_PEN || nPen == DC_PEN);
#else
        DbgAssert(nPen == WHITE_PEN || nPen == BLACK_PEN || nPen == NULL_PEN);
#endif // !(_WIN32_WINNT >= 0x0500)
        return SelectPen((HPEN)::GetStockObject(nPen));
    }
    
    TMHBrush SelectStockBrush(int nBrush)
    {
#if (_WIN32_WINNT >= 0x0500)
        DbgAssert((nBrush >= WHITE_BRUSH && nBrush <= HOLLOW_BRUSH) || nBrush == DC_BRUSH);
#else
        DbgAssert(nBrush >= WHITE_BRUSH && nBrush <= HOLLOW_BRUSH);
#endif // !(_WIN32_WINNT >= 0x0500)
        return SelectBrush((TMHBrush)::GetStockObject(nBrush));
    }
    
    HFONT SelectStockFont(int nFont)
    {
#ifndef _WIN32_WCE
        DbgAssert((nFont >= OEM_FIXED_FONT && nFont <= SYSTEM_FIXED_FONT) || nFont == DEFAULT_GUI_FONT);
#else // CE specific
        DbgAssert(nFont == SYSTEM_FONT);
#endif // _WIN32_WCE
        return SelectFont((HFONT)::GetStockObject(nFont));
    }
    
    HPALETTE SelectStockPalette(int nPalette, TMBool bForceBackground)
    {
        DbgAssert(nPalette == DEFAULT_PALETTE); // the only one supported
        return SelectPalette((HPALETTE)::GetStockObject(nPalette), bForceBackground);
    }

    // Color and Color Palette Functions
    COLORREF GetNearestColor(COLORREF crColor) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetNearestColor(m_hDC, crColor);
    }

    HPALETTE SelectPalette(HPALETTE hPalette, TMBool bForceBackground)
    {
        DbgAssert(m_hDC != NULL);
    
        return ::SelectPalette(m_hDC, hPalette, bForceBackground);
    }

    TMUInt RealizePalette()
    {
        DbgAssert(m_hDC != NULL);
        return ::RealizePalette(m_hDC);
    }

    #ifndef _WIN32_WCE
    void UpdateColors()
    {
        DbgAssert(m_hDC != NULL);
        ::UpdateColors(m_hDC);
    }
    #endif // !_WIN32_WCE
*/

// Drawing-Attribute Functions
//     TMColor GetBkColor() const
//     {
//         return m_clrBk;
//     }
// 
//     int GetBkMode() const
//     {
//         return OPAQUE;
//     }

    /*
    int GetPolyFillMode() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetPolyFillMode(m_hDC);
    }
    
    int GetROP2() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetROP2(m_hDC);
    }
    
    int GetStretchBltMode() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetStretchBltMode(m_hDC);
    }
 
    COLORREF GetTextColor() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetTextColor(m_hDC);
    }
    
    COLORREF SetBkColor(COLORREF crColor)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetBkColor(m_hDC, crColor);
    }
    
    int SetBkMode(int nBkMode)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetBkMode(m_hDC, nBkMode);
    }
    
#ifndef _WIN32_WCE
    int SetPolyFillMode(int nPolyFillMode)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetPolyFillMode(m_hDC, nPolyFillMode);
    }
#endif // !_WIN32_WCE
    
    int SetROP2(int nDrawMode)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetROP2(m_hDC, nDrawMode);
    }
    
#ifndef _WIN32_WCE
    int SetStretchBltMode(int nStretchMode)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetStretchBltMode(m_hDC, nStretchMode);
    }
#endif // !_WIN32_WCE
    
    COLORREF SetTextColor(COLORREF crColor)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetTextColor(m_hDC, crColor);
    }
    
#ifndef _WIN32_WCE
    TMBool GetColorAdjustment(LPCOLORADJUSTMENT lpColorAdjust) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetColorAdjustment(m_hDC, lpColorAdjust);
    }
    
    TMBool SetColorAdjustment(const COLORADJUSTMENT* lpColorAdjust)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetColorAdjustment(m_hDC, lpColorAdjust);
    }
    */
    


//     TMBool MoveTo(int x, int y, TMPoint* lpPoint = NULL)
//     {
//         DbgAssert(m_hDC);
//         if (lpPoint != NULL)
//             *lpPoint = m_ptCur;
//         m_ptCur.x = x;
//         m_ptCur.y = y;
//         cairo_move_to(m_hDC, x, y);
//         return TMTrue;
//     }
// 
//     TMBool MoveTo(TMPoint point, TMPoint* lpPointRet = NULL)
//     {
//         return MoveTo(point.x, point.y, lpPointRet);
//     }
// 
//     TMBool LineTo(int x, int y)
//     {
//         DbgAssert(m_hDC);
//         cairo_line_to(m_hDC, x, y);
//         cairo_stroke(m_hDC);
//         m_ptCur.x = x;
//         m_ptCur.y = y;
//         cairo_move_to(m_hDC, x, y);
//         return TMTrue;
//     }
// 
//     TMBool LineTo(TMPoint point)
//     {
//         return LineTo(point.x, point.y);
//     }
// 
//     TMColor SetPixel(int x, int y, TMColor crColor)
//     {
//         DbgAssert(m_hDC);
//         cairo_save(m_hDC);
//         cairo_set_source_rgb(m_hDC, GetRValue(crColor) / 255.0, GetGValue(crColor) / 255.0, GetBValue(crColor) / 255.0);
//         cairo_rectangle(m_hDC, x, y, 1.0, 1.0);
//         cairo_fill(m_hDC);
//         cairo_restore(m_hDC);
//         return crColor;
//     }
// 
//     TMColor SetPixel(TMPoint point, TMColor crColor)
//     {
//         return SetPixel(point.x, point.y, crColor);
//     }
// 
//     TMBool SetPixelV(int x, int y, TMColor crColor)
//     {
//         return SetPixel(x, y, crColor) != CLR_INVALID;
//     }
// 
//     TMBool SetPixelV(TMPoint point, TMColor crColor)
//     {
//         return SetPixelV(point.x, point.y, crColor);
//     }

//     TMBool Polyline(const TMPoint* lpPoints, int nCount)
//     {
//         DbgAssert(m_hDC);
//         if (lpPoints == NULL || nCount <= 0)
//             return TMFalse;
//         cairo_move_to(m_hDC, lpPoints[0].x, lpPoints[0].y);
//         for (int i = 1; i < nCount; ++i)
//             cairo_line_to(m_hDC, lpPoints[i].x, lpPoints[i].y);
//         cairo_stroke(m_hDC);
//         m_ptCur = lpPoints[nCount - 1];
//         return TMTrue;
//     }

    TMBool Rectangle(int x1, int y1, int x2, int y2)
    {
        DbgAssert(m_hDC);
        cairo_rectangle(m_hDC, x1, y1, x2 - x1, y2 - y1);
        cairo_stroke(m_hDC);
        return TMTrue;
    }

    TMBool Rectangle(const TMRect* lpRect)
    {
        if (lpRect == NULL)
            return TMFalse;
        return Rectangle(RectLeft(*lpRect), RectTop(*lpRect), RectRight(*lpRect), RectBottom(*lpRect));
    }

    void FillSolidRect(const TMRect* lpRect, TMColor clr)
    {
        DbgAssert(m_hDC);
        if (lpRect == NULL)
            return;
        cairo_save(m_hDC);
        cairo_set_source_rgb(m_hDC, ColorRF(clr), ColorGF(clr), ColorBF(clr));
        cairo_rectangle(m_hDC, RectLeft(*lpRect), RectTop(*lpRect), RectRight(*lpRect) - RectLeft(*lpRect), RectBottom(*lpRect) - RectTop(*lpRect));
        cairo_fill(m_hDC);
        cairo_restore(m_hDC);
    }

//     TMBool FillRect(const TMRect* lpRect, TMHBrush hBrush)
//     {
//         tm_unused(hBrush);
//         if (lpRect == NULL)
//             return TMFalse;
//         FillSolidRect(lpRect, m_clrBk);
//         return TMTrue;
//     }

    TMBool DrawIcon(int x, int y, TMHIcon hIcon)
    {
        DbgAssert(m_hDC);
        if (hIcon == NULL)
            return TMFalse;
        gdk_cairo_set_source_pixbuf(m_hDC, hIcon, x, y);
        cairo_paint(m_hDC);
        return TMTrue;
    }

    TMBool DrawIcon(TMPoint point, TMHIcon hIcon)
    {
        return DrawIcon(point.x, point.y, hIcon);
    }

    TMBool DrawIconEx(int x, int y, TMHIcon hIcon, int cxWidth, int cyWidth, TMUInt uStepIfAniCur = 0, TMHBrush hbrFlickerFreeDraw = NULL, TMUInt uFlags = DI_NORMAL)
    {
        tm_unused(uStepIfAniCur);
        tm_unused(hbrFlickerFreeDraw);
        tm_unused(uFlags);
        if (hIcon == NULL || m_hDC == NULL)
            return TMFalse;
        GdkPixbuf* draw = hIcon;
        if (cxWidth > 0 && cyWidth > 0)
            draw = gdk_pixbuf_scale_simple(hIcon, cxWidth, cyWidth, GDK_INTERP_BILINEAR);
        if (draw == NULL)
            return TMFalse;
        gdk_cairo_set_source_pixbuf(m_hDC, draw, x, y);
        cairo_paint(m_hDC);
        if (draw != hIcon)
            g_object_unref(draw);
        return TMTrue;
    }

//     TMBool TextOut(int x, int y, TMCStr lpszString, int nCount = -1)
//     {
//         DbgAssert(m_hDC);
//         if (lpszString == NULL)
//             return TMFalse;
//         gchar* ownedText = NULL;
//         const char* text = _TMUtf8Slice(lpszString, nCount, &ownedText);
//         cairo_save(m_hDC);
//         cairo_set_source_rgb(m_hDC, ColorRF(m_clrText), ColorGF(m_clrText), ColorBF(m_clrText));
//         cairo_move_to(m_hDC, x, y);
//         cairo_show_text(m_hDC, text);
//         cairo_restore(m_hDC);
//         g_free(ownedText);
//         return TMTrue;
//     }
// 
//     int DrawText(TMCStr lpstrText, int cchText, TMRect* lpRect, TMUInt uFormat)
//     {
//         tm_unused(uFormat);
//         if (m_hDC == NULL || lpstrText == NULL || lpRect == NULL)
//             return 0;
//         gchar* ownedText = NULL;
//         const char* text = _TMUtf8Slice(lpstrText, cchText, &ownedText);
//         cairo_text_extents_t te;
//         cairo_text_extents(m_hDC, text, &te);
//         if ((uFormat & DT_CALCRECT) != 0)
//         {
//             RectSetRight(*lpRect, RectLeft(*lpRect) + (int)te.width);
//             RectSetBottom(*lpRect, RectTop(*lpRect) + (int)te.height);
//             g_free(ownedText);
//             return (int)te.height;
//         }
//         g_free(ownedText);
//         return TextOut(RectLeft(*lpRect), RectTop(*lpRect) + (int)te.height, lpstrText, cchText) ? (int)te.height : 0;
//     }

    TMBool GetTextExtent(TMCStr lpszString, int nCount, TMSize* lpSize) const
    {
        if (m_hDC == NULL || lpszString == NULL || lpSize == NULL)
            return TMFalse;
        gchar* ownedText = NULL;
        const char* text = _TMUtf8Slice(lpszString, nCount, &ownedText);
        cairo_text_extents_t te;
        cairo_text_extents(m_hDC, text, &te);
        g_free(ownedText);
        lpSize->cx = (long)te.x_advance;
        lpSize->cy = (long)te.height;
        return TMTrue;
    }

//     int SetMapMode(int nMapMode)
//     {
//         tm_unused(nMapMode);
//         return 0;
//     }
// 
//     int GetMapMode() const
//     {
//         return MM_TEXT;
//     }

    TMBool SetViewportOrg(int x, int y, TMPoint* lpPoint = NULL)
    {
        tm_unused(x);
        tm_unused(y);
        tm_unused(lpPoint);
        return TMTrue;
    }

    TMBool SetWindowOrg(int x, int y, TMPoint* lpPoint = NULL)
    {
        tm_unused(x);
        tm_unused(y);
        tm_unused(lpPoint);
        return TMTrue;
    }

    TMBool SetWindowExt(int x, int y, TMSize* lpSize = NULL)
    {
        tm_unused(x);
        tm_unused(y);
        tm_unused(lpSize);
        return TMTrue;
    }

    TMBool SetViewportExt(int x, int y, TMSize* lpSize = NULL)
    {
        tm_unused(x);
        tm_unused(y);
        tm_unused(lpSize);
        return TMTrue;
    }

    TMBool GetCurrentPosition(TMPoint* lpPoint) const
    {
        DbgAssert(m_hDC);
        if (!cairo_has_current_point(m_hDC)) 
            return TMFalse;
        double x, y;
        cairo_get_current_point(m_hDC, &x, &y);
        PointSetX(*lpPoint, (long)x);
        PointSetY(*lpPoint, (long)y);
        return TMTrue;
    }

    TMBool Ellipse(int x1, int y1, int x2, int y2)
    {
        DbgAssert(m_hDC);
        double cx = (x1 + x2) / 2.0;
        double cy = (y1 + y2) / 2.0;
        double rx = abs(x2 - x1) / 2.0;
        double ry = abs(y2 - y1) / 2.0;
        cairo_save(m_hDC);
        cairo_translate(m_hDC, cx, cy);
        cairo_scale(m_hDC, rx > 0.0 ? rx : 1.0, ry > 0.0 ? ry : 1.0);
        cairo_arc(m_hDC, 0.0, 0.0, 1.0, 0.0, 2.0 * 3.14159265358979323846);
        cairo_restore(m_hDC);
        cairo_stroke(m_hDC);
        return TMTrue;
    }

    TMBool Ellipse(const TMRect* lpRect)
    {
        if (lpRect == NULL)
            return TMFalse;
        return Ellipse(RectLeft(*lpRect), RectTop(*lpRect), RectRight(*lpRect), RectBottom(*lpRect));
    }

    TMBool FillRect(const TMRect* lpRect, int nColorIndex)
    {
        FillSolidRect(lpRect, ColorFromSys(nColorIndex));
        return TMTrue;
    }

    void FillSolidRect(int x, int y, int cx, int cy, TMColor clr)
    {
        TMRect rc = {x, y, cx, cy};
        FillSolidRect(&rc, clr);
    }

    TMBool DrawIconEx(const TMPoint& point, TMHIcon hIcon, const TMSize& TMSize, TMUInt uStepIfAniCur = 0, TMHBrush hbrFlickerFreeDraw = NULL, TMUInt uFlags = DI_NORMAL)
    {
        return DrawIconEx(point.x, point.y, hIcon, TMSize.cx, TMSize.cy, uStepIfAniCur, hbrFlickerFreeDraw, uFlags);
    }

//     TMBool DrawState(TMPoint pt, TMSize TMSize, HBITMAP hBitmap, TMUInt nFlags, TMHBrush hBrush = NULL)
//     {
//         tm_unused(hBrush);
//         tm_unused(nFlags);
//         if (hBitmap == NULL || m_hDC == NULL)
//             return TMFalse;
//         // On GTK, HBITMAP maps to TMHBmp = GdkPixbuf* per tmnative.h
//         GdkPixbuf* pix = (GdkPixbuf*)hBitmap;
//         int w = TMSize.cx > 0 ? TMSize.cx : gdk_pixbuf_get_width(pix);
//         int h = TMSize.cy > 0 ? TMSize.cy : gdk_pixbuf_get_height(pix);
//         return DrawIconEx(pt.x, pt.y, pix, w, h, 0, NULL, DI_NORMAL);
//     }
// 
//     TMBool DrawState(TMPoint pt, TMSize TMSize, TMHIcon hIcon, TMUInt nFlags, TMHBrush hBrush = NULL)
//     {
//         tm_unused(nFlags);
//         tm_unused(hBrush);
//         int w = TMSize.cx > 0 ? TMSize.cx : (hIcon != NULL ? gdk_pixbuf_get_width(hIcon) : 0);
//         int h = TMSize.cy > 0 ? TMSize.cy : (hIcon != NULL ? gdk_pixbuf_get_height(hIcon) : 0);
//         return DrawIconEx(pt.x, pt.y, hIcon, w, h, 0, NULL, DI_NORMAL);
//     }
// 
//     TMBool DrawState(TMPoint pt, TMSize TMSize, TMCStr lpszText, TMUInt nFlags, TMBool bPrefixText = TMTrue, int nTextLen = 0, TMHBrush hBrush = NULL)
//     {
//         tm_unused(TMSize);
//         tm_unused(nFlags);
//         tm_unused(bPrefixText);
//         tm_unused(hBrush);
//         return TextOut(pt.x, pt.y, lpszText, nTextLen);
//     }
// 
//     TMBool DrawState(TMPoint pt, TMSize TMSize, DRAWSTATEPROC lpDrawProc, TMLParam lData, TMUInt nFlags, TMHBrush hBrush = NULL)
//     {
//         tm_unused(pt);
//         tm_unused(TMSize);
//         tm_unused(nFlags);
//         tm_unused(hBrush);
//         if (lpDrawProc == NULL || m_hDC == NULL)
//             return TMFalse;
//         return lpDrawProc(m_hDC, lData, 0, TMSize.cx, TMSize.cy) ? TMTrue : TMFalse;
//     }

    TMBool BitBlt(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, int xSrc, int ySrc, TMUInt32 dwRop)
    {
        tm_unused(dwRop);
        if (m_hDC == NULL || hSrcDC == NULL || nWidth <= 0 || nHeight <= 0)
            return TMFalse;

        cairo_surface_t* srcSurface = cairo_get_target(hSrcDC);
        if (srcSurface == NULL)
            return TMFalse;

        cairo_save(m_hDC);
        cairo_rectangle(m_hDC, x, y, nWidth, nHeight);
        cairo_clip(m_hDC);
        cairo_set_source_surface(m_hDC, srcSurface, x - xSrc, y - ySrc);
        cairo_paint(m_hDC);
        cairo_restore(m_hDC);
        return TMTrue;
    }

    TMBool StretchBlt(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, int xSrc, int ySrc, int wSrc, int hSrc, TMUInt32 dwRop)
    {
        tm_unused(dwRop);
        if (m_hDC == NULL || hSrcDC == NULL)
            return TMFalse;
        if (nWidth <= 0 || nHeight <= 0 || wSrc <= 0 || hSrc <= 0)
            return TMFalse;

        cairo_surface_t* srcSurface = cairo_get_target(hSrcDC);
        if (srcSurface == NULL)
            return TMFalse;

        cairo_save(m_hDC);
        cairo_rectangle(m_hDC, x, y, nWidth, nHeight);
        cairo_clip(m_hDC);
        cairo_translate(m_hDC, x, y);
        cairo_scale(m_hDC, (double)nWidth / (double)wSrc, (double)nHeight / (double)hSrc);
        cairo_set_source_surface(m_hDC, srcSurface, -xSrc, -ySrc);
        cairo_pattern_t* pattern = cairo_get_source(m_hDC);
        if (pattern != NULL)
            cairo_pattern_set_filter(pattern, CAIRO_FILTER_BILINEAR);
        cairo_paint(m_hDC);
        cairo_restore(m_hDC);
        return TMTrue;
    }

//     TMBool MaskBlt(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, int xSrc, int ySrc, HBITMAP hMaskBitmap, int xMask, int yMask, TMUInt32 dwRop)
//     {
//         tm_unused(xMask);
//         tm_unused(yMask);
//         tm_unused(hMaskBitmap);
//         // Cairo path currently ignores external bitmap masks and falls back to source copy.
//         return BitBlt(x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, dwRop);
//     }
// 
//     TMBool PlgBlt(TMPoint* lpPoint, TMHDC hSrcDC, int xSrc, int ySrc, int nWidth, int nHeight, HBITMAP hMaskBitmap, int xMask, int yMask)
//     {
//         tm_unused(hMaskBitmap);
//         tm_unused(xMask);
//         tm_unused(yMask);
//         if (lpPoint == NULL)
//             return TMFalse;
//         // Approximate Parallelogram blit with destination top-left anchored blit.
//         return BitBlt(lpPoint[0].x, lpPoint[0].y, nWidth, nHeight, hSrcDC, xSrc, ySrc, SRCCOPY);
//     }
// 
//     TMBool TransparentBlt(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, TMUInt crTransparent)
//     {
//         tm_unused(crTransparent);
//         // Color-key transparency is not mapped yet; keep geometry-compatible blit.
//         return StretchBlt(x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, nSrcWidth, nSrcHeight, SRCCOPY);
//     }

//     TMBool GradientFill(const PTRIVERTEX pVertices, TMUInt32 nVertices, void* pMeshElements, TMUInt32 nMeshElements, TMUInt32 dwMode)
//     {
//         tm_unused(pMeshElements);
//         tm_unused(nMeshElements);
//         tm_unused(dwMode);
//         if (m_hDC == NULL || pVertices == NULL || nVertices < 2)
//             return TMFalse;
// 
//         const TRIVERTEX& v0 = pVertices[0];
//         const TRIVERTEX& v1 = pVertices[1];
//         double x0 = (double)v0.x;
//         double y0 = (double)v0.y;
//         double x1 = (double)v1.x;
//         double y1 = (double)v1.y;
//         double left = x0 < x1 ? x0 : x1;
//         double top = y0 < y1 ? y0 : y1;
//         double right = x0 > x1 ? x0 : x1;
//         double bottom = y0 > y1 ? y0 : y1;
//         double w = right - left;
//         double h = bottom - top;
//         if (w <= 0.0 || h <= 0.0)
//             return TMFalse;
// 
//         cairo_pattern_t* pat = cairo_pattern_create_linear(left, top, right, bottom);
//         if (pat == NULL)
//             return TMFalse;
// 
//         cairo_pattern_add_color_stop_rgba(pat, 0.0,
//             (double)v0.Red / 65535.0,
//             (double)v0.Green / 65535.0,
//             (double)v0.Blue / 65535.0,
//             1.0);
//         cairo_pattern_add_color_stop_rgba(pat, 1.0,
//             (double)v1.Red / 65535.0,
//             (double)v1.Green / 65535.0,
//             (double)v1.Blue / 65535.0,
//             1.0);
// 
//         cairo_save(m_hDC);
//         cairo_rectangle(m_hDC, left, top, w, h);
//         cairo_set_source(m_hDC, pat);
//         cairo_fill(m_hDC);
//         cairo_restore(m_hDC);
//         cairo_pattern_destroy(pat);
//         return TMTrue;
//     }

    TMBool GradientFillRect(TMRect& rect, TMColor clr1, TMColor clr2, bool bHorizontal)
    {
        if (m_hDC == NULL)
            return TMFalse;
        double x0 = (double)RectLeft(rect);
        double y0 = (double)RectTop(rect);
        double x1 = bHorizontal ? (double)RectRight(rect) : (double)RectLeft(rect);
        double y1 = bHorizontal ? (double)RectTop(rect) : (double)RectBottom(rect);
        cairo_pattern_t* pat = cairo_pattern_create_linear(x0, y0, x1, y1);
        if (pat == NULL)
            return TMFalse;

        cairo_pattern_add_color_stop_rgba(pat, 0.0,
            ColorRF(clr1),
            ColorGF(clr1),
            ColorBF(clr1),
            1.0);
        cairo_pattern_add_color_stop_rgba(pat, 1.0,
            ColorRF(clr2),
            ColorGF(clr2),
            ColorBF(clr2),
            1.0);

        cairo_save(m_hDC);
        cairo_rectangle(m_hDC, RectLeft(rect), RectTop(rect), RectRight(rect) - RectLeft(rect), RectBottom(rect) - RectTop(rect));
        cairo_set_source(m_hDC, pat);
        cairo_fill(m_hDC);
        cairo_restore(m_hDC);
        cairo_pattern_destroy(pat);
        return TMTrue;
    }

//     TMBool AlphaBlend(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, BLENDFUNCTION bf)
//     {
//         if (m_hDC == NULL || hSrcDC == NULL)
//             return TMFalse;
//         if (nWidth <= 0 || nHeight <= 0 || nSrcWidth <= 0 || nSrcHeight <= 0)
//             return TMFalse;
// 
//         cairo_surface_t* srcSurface = cairo_get_target(hSrcDC);
//         if (srcSurface == NULL)
//             return TMFalse;
// 
//         double alpha = (double)bf.SourceConstantAlpha / 255.0;
//         cairo_save(m_hDC);
//         cairo_rectangle(m_hDC, x, y, nWidth, nHeight);
//         cairo_clip(m_hDC);
//         cairo_translate(m_hDC, x, y);
//         cairo_scale(m_hDC, (double)nWidth / (double)nSrcWidth, (double)nHeight / (double)nSrcHeight);
//         cairo_set_source_surface(m_hDC, srcSurface, -xSrc, -ySrc);
//         cairo_paint_with_alpha(m_hDC, alpha);
//         cairo_restore(m_hDC);
//         return TMTrue;
//     }
// 
//     TMBool ExtTextOut(int x, int y, TMUInt nOptions, const TMRect* lpRect, TMCStr lpszString, TMUInt nCount = (TMUInt)-1, int* lpDxWidths = NULL)
//     {
//         tm_unused(nOptions);
//         tm_unused(lpDxWidths);
// //         if (lpRect != NULL && (nOptions & ETO_OPAQUE) != 0)
// //             FillSolidRect((const TMRect*)lpRect, m_clrBk);
//         int count = (nCount == (TMUInt)-1) ? -1 : (int)nCount;
//         return TextOut(x, y, lpszString, count);
//     }
// 
//     TMSize TabbedTextOut(int x, int y, TMCStr lpszString, int nCount = -1, int nTabPositions = 0, int* lpnTabStopPositions = NULL, int nTabOrigin = 0)
//     {
//         tm_unused(nTabPositions);
//         tm_unused(lpnTabStopPositions);
//         tm_unused(nTabOrigin);
//         TMSize TMSize = { 0, 0 };
//         if (lpszString == NULL || m_hDC == NULL)
//             return TMSize;
// 
//         gchar* ownedText = NULL;
//         const char* text = _TMUtf8TabsToSpaces(lpszString, nCount, &ownedText);
// 
//         cairo_save(m_hDC);
//         cairo_set_source_rgb(m_hDC, ColorRF(m_clrText), ColorGF(m_clrText), ColorBF(m_clrText));
//         cairo_move_to(m_hDC, x, y);
//         cairo_show_text(m_hDC, text);
//         cairo_restore(m_hDC);
// 
//         cairo_text_extents_t te;
//         cairo_text_extents(m_hDC, text, &te);
//         g_free(ownedText);
//         TMSize.cx = te.x_advance;
//         TMSize.cy = te.height;
//         return TMSize;
//     }
// 
//     int DrawText(TMStr lpstrText, int cchText, TMRect* lpRect, TMUInt uFormat)
//     {
//         return DrawText((TMCStr)lpstrText, cchText, (TMRect*)lpRect, uFormat);
//     }
// 
//     int DrawTextEx(TMStr lpstrText, int cchText, TMRect* lpRect, TMUInt uFormat, LPDRAWTEXTPARAMS lpDTParams = NULL)
//     {
//         tm_unused(lpDTParams);
//         return DrawText((TMCStr)lpstrText, cchText, (TMRect*)lpRect, uFormat);
//     }

    TMBool GetTextExtentExPoint(TMCStr lpszString, int cchString, TMSize* lpSize, int nMaxExtent, int* lpnFit = NULL, int* alpDx = NULL)
    {
        tm_unused(alpDx);
        if (!GetTextExtent(lpszString, cchString, lpSize))
            return TMFalse;
        if (lpnFit != NULL)
        {
            if (lpSize->cx <= 0 || cchString <= 0)
                *lpnFit = 0;
            else if (nMaxExtent <= 0)
                *lpnFit = 0;
            else
                *lpnFit = (lpSize->cx <= nMaxExtent) ? cchString : (cchString * nMaxExtent / lpSize->cx);
        }
        return TMTrue;
    }

    TMUInt32 GetTabbedTextExtent(TMCStr lpszString, int nCount = -1, int nTabPositions = 0, int* lpnTabStopPositions = NULL) const
    {
        tm_unused(nTabPositions);
        tm_unused(lpnTabStopPositions);
        TMSize TMSize = { 0, 0 };
        if (!GetTextExtent(lpszString, nCount, &TMSize))
            return 0;
        return MAKELONG((TMUInt16)TMSize.cx, (TMUInt16)TMSize.cy);
    }

//     TMUInt GetTextAlign() const
//     {
//         return m_uTextAlign;
//     }
// 
//     TMUInt SetTextAlign(TMUInt nFlags)
//     {
//         TMUInt old = m_uTextAlign;
//         m_uTextAlign = nFlags;
//         return old;
//     }

    // -------------------------------------------------------------------------
    // Geometry helpers �?compute the angle from bounding-box centre to a point
    // -------------------------------------------------------------------------
private:
    static double _AngleTo(double cx, double cy, double px, double py)
    {
        return atan2(py - cy, px - cx);
    }

public:
    TMBool Arc(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
    {
        if (m_hDC == NULL) return TMFalse;
        double cx = (x1 + x2) / 2.0, cy = (y1 + y2) / 2.0;
        double rx = abs(x2 - x1) / 2.0, ry = abs(y2 - y1) / 2.0;
        if (rx <= 0.0 || ry <= 0.0) return TMFalse;
        double a1 = _AngleTo(cx, cy, x3, y3);
        double a2 = _AngleTo(cx, cy, x4, y4);
        cairo_save(m_hDC);
        cairo_translate(m_hDC, cx, cy);
        cairo_scale(m_hDC, rx, ry);
        cairo_arc(m_hDC, 0.0, 0.0, 1.0, a1, a2);
        cairo_restore(m_hDC);
        cairo_stroke(m_hDC);
        return TMTrue;
    }

    TMBool Arc(const TMRect* lpRect, TMPoint ptStart, TMPoint ptEnd)
    {
        if (!lpRect) return TMFalse;
        return Arc(RectLeft(*lpRect), RectTop(*lpRect), RectRight(*lpRect), RectBottom(*lpRect),
                   ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
    }

//     TMBool AngleArc(int x, int y, int nRadius, float fStartAngle, float fSweepAngle)
//     {
//         if (m_hDC == NULL || nRadius <= 0) return TMFalse;
//         double startRad = fStartAngle * 3.14159265358979323846 / 180.0;
//         double endRad   = startRad + fSweepAngle * 3.14159265358979323846 / 180.0;
//         cairo_save(m_hDC);
//         cairo_move_to(m_hDC, m_ptCur.x, m_ptCur.y);
//         if (fSweepAngle >= 0.0f)
//             cairo_arc(m_hDC, x, y, nRadius, startRad, endRad);
//         else
//             cairo_arc_negative(m_hDC, x, y, nRadius, startRad, endRad);
//         cairo_restore(m_hDC);
//         cairo_stroke(m_hDC);
//         double ex = x + nRadius * cos(endRad);
//         double ey = y + nRadius * sin(endRad);
//         m_ptCur.x = (int)ex;
//         m_ptCur.y = (int)ey;
//         return TMTrue;
//     }
// 
//     TMBool ArcTo(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
//     {
//         double cx = (x1 + x2) / 2.0, cy = (y1 + y2) / 2.0;
//         double rx = abs(x2 - x1) / 2.0, ry = abs(y2 - y1) / 2.0;
//         if (rx <= 0.0 || ry <= 0.0) return TMFalse;
//         double a1 = _AngleTo(cx, cy, x3, y3);
//         double a2 = _AngleTo(cx, cy, x4, y4);
//         double sx = cx + rx * cos(a1), sy = cy + ry * sin(a1);
//         cairo_save(m_hDC);
//         cairo_move_to(m_hDC, m_ptCur.x, m_ptCur.y);
//         cairo_line_to(m_hDC, sx, sy);
//         cairo_translate(m_hDC, cx, cy);
//         cairo_scale(m_hDC, rx, ry);
//         cairo_arc(m_hDC, 0.0, 0.0, 1.0, a1, a2);
//         cairo_restore(m_hDC);
//         cairo_stroke(m_hDC);
//         double ex = cx + rx * cos(a2), ey = cy + ry * sin(a2);
//         m_ptCur.x = (int)ex;
//         m_ptCur.y = (int)ey;
//         return TMTrue;
//     }
// 
//     TMBool ArcTo(const TMRect* lpRect, TMPoint ptStart, TMPoint ptEnd)
//     {
//         if (!lpRect) return TMFalse;
//         return ArcTo(RectLeft(*lpRect), RectTop(*lpRect), RectRight(*lpRect), RectBottom(*lpRect),
//                      ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
//     }

    TMBool Chord(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
    {
        if (m_hDC == NULL) return TMFalse;
        double cx = (x1 + x2) / 2.0, cy = (y1 + y2) / 2.0;
        double rx = abs(x2 - x1) / 2.0, ry = abs(y2 - y1) / 2.0;
        if (rx <= 0.0 || ry <= 0.0) return TMFalse;
        double a1 = _AngleTo(cx, cy, x3, y3);
        double a2 = _AngleTo(cx, cy, x4, y4);
        cairo_save(m_hDC);
        cairo_translate(m_hDC, cx, cy);
        cairo_scale(m_hDC, rx, ry);
        cairo_arc(m_hDC, 0.0, 0.0, 1.0, a1, a2);
        cairo_close_path(m_hDC);
        cairo_restore(m_hDC);
        cairo_stroke(m_hDC);
        return TMTrue;
    }

    TMBool Chord(const TMRect* lpRect, TMPoint ptStart, TMPoint ptEnd)
    {
        if (!lpRect) return TMFalse;
        return Chord(RectLeft(*lpRect), RectTop(*lpRect), RectRight(*lpRect), RectBottom(*lpRect),
                     ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
    }

    TMBool Pie(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
    {
        if (m_hDC == NULL) return TMFalse;
        double cx = (x1 + x2) / 2.0, cy = (y1 + y2) / 2.0;
        double rx = abs(x2 - x1) / 2.0, ry = abs(y2 - y1) / 2.0;
        if (rx <= 0.0 || ry <= 0.0) return TMFalse;
        double a1 = _AngleTo(cx, cy, x3, y3);
        double a2 = _AngleTo(cx, cy, x4, y4);
        cairo_save(m_hDC);
        cairo_translate(m_hDC, cx, cy);
        cairo_scale(m_hDC, rx, ry);
        cairo_move_to(m_hDC, 0.0, 0.0);
        cairo_arc(m_hDC, 0.0, 0.0, 1.0, a1, a2);
        cairo_close_path(m_hDC);
        cairo_restore(m_hDC);
        cairo_stroke(m_hDC);
        return TMTrue;
    }

    TMBool Pie(const TMRect* lpRect, TMPoint ptStart, TMPoint ptEnd)
    {
        if (!lpRect) return TMFalse;
        return Pie(RectLeft(*lpRect), RectTop(*lpRect), RectRight(*lpRect), RectBottom(*lpRect),
                   ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
    }

    TMBool Polygon(const TMPoint* lpPoints, int nCount)
    {
        if (m_hDC == NULL || !lpPoints || nCount < 2) return TMFalse;
        cairo_move_to(m_hDC, lpPoints[0].x, lpPoints[0].y);
        for (int i = 1; i < nCount; ++i)
            cairo_line_to(m_hDC, lpPoints[i].x, lpPoints[i].y);
        cairo_close_path(m_hDC);
        cairo_stroke(m_hDC);
        return TMTrue;
    }

    TMBool PolyPolygon(const TMPoint* lpPoints, const int* lpPolyCounts, int nCount)
    {
        if (m_hDC == NULL || !lpPoints || !lpPolyCounts || nCount <= 0) return TMFalse;
        int idx = 0;
        for (int p = 0; p < nCount; ++p)
        {
            int cnt = lpPolyCounts[p];
            if (cnt < 2) { idx += cnt; continue; }
            cairo_move_to(m_hDC, lpPoints[idx].x, lpPoints[idx].y);
            for (int i = 1; i < cnt; ++i)
                cairo_line_to(m_hDC, lpPoints[idx + i].x, lpPoints[idx + i].y);
            cairo_close_path(m_hDC);
            idx += cnt;
        }
        cairo_stroke(m_hDC);
        return TMTrue;
    }

//     TMBool PolylineTo(const TMPoint* lpPoints, int nCount)
//     {
//         if (m_hDC == NULL || !lpPoints || nCount <= 0) return TMFalse;
//         cairo_move_to(m_hDC, m_ptCur.x, m_ptCur.y);
//         for (int i = 0; i < nCount; ++i)
//             cairo_line_to(m_hDC, lpPoints[i].x, lpPoints[i].y);
//         cairo_stroke(m_hDC);
//         m_ptCur = lpPoints[nCount - 1];
//         cairo_move_to(m_hDC, m_ptCur.x, m_ptCur.y);
//         return TMTrue;
//     }
// 
//     TMBool PolyPolyline(const TMPoint* lpPoints, const TMUInt32* lpPolyPoints, int nCount)
//     {
//         if (m_hDC == NULL || !lpPoints || !lpPolyPoints || nCount <= 0) return TMFalse;
//         int idx = 0;
//         for (int p = 0; p < nCount; ++p)
//         {
//             int cnt = (int)lpPolyPoints[p];
//             if (cnt < 2) { idx += cnt; continue; }
//             cairo_move_to(m_hDC, lpPoints[idx].x, lpPoints[idx].y);
//             for (int i = 1; i < cnt; ++i)
//                 cairo_line_to(m_hDC, lpPoints[idx + i].x, lpPoints[idx + i].y);
//             cairo_stroke(m_hDC);
//             idx += cnt;
//         }
//         return TMTrue;
//     }

    TMBool PolyBezier(const TMPoint* lpPoints, int nCount)
    {
        if (m_hDC == NULL || !lpPoints || nCount < 4) return TMFalse;
        cairo_move_to(m_hDC, lpPoints[0].x, lpPoints[0].y);
        for (int i = 1; i + 2 < nCount; i += 3)
            cairo_curve_to(m_hDC, lpPoints[i].x, lpPoints[i].y,
                           lpPoints[i+1].x, lpPoints[i+1].y,
                           lpPoints[i+2].x, lpPoints[i+2].y);
        cairo_stroke(m_hDC);
        return TMTrue;
    }

//     TMBool PolyBezierTo(const TMPoint* lpPoints, int nCount)
//     {
//         if (m_hDC == NULL || !lpPoints || nCount < 3) return TMFalse;
//         cairo_move_to(m_hDC, m_ptCur.x, m_ptCur.y);
//         for (int i = 0; i + 2 < nCount; i += 3)
//             cairo_curve_to(m_hDC, lpPoints[i].x, lpPoints[i].y,
//                            lpPoints[i+1].x, lpPoints[i+1].y,
//                            lpPoints[i+2].x, lpPoints[i+2].y);
//         cairo_stroke(m_hDC);
//         m_ptCur = lpPoints[nCount - 1];
//         return TMTrue;
//     }

    TMBool RoundRect(int x1, int y1, int x2, int y2, int x3, int y3)
    {
        if (m_hDC == NULL) return TMFalse;
        double rx = abs(x3) / 2.0, ry = abs(y3) / 2.0;
        double w = x2 - x1, h = y2 - y1;
        if (w <= 0 || h <= 0 || rx <= 0 || ry <= 0)
            return Rectangle(x1, y1, x2, y2);
        cairo_save(m_hDC);
        cairo_translate(m_hDC, x1 + rx, y1 + ry);
        cairo_scale(m_hDC, rx, ry);
        cairo_arc(m_hDC, 0.0, -((h/2 - ry)/ry), 1.0, -3.14159265358979323846, -3.14159265358979323846/2);
        cairo_restore(m_hDC);
        // Simpler implementation: use a properly scaled rounded rectangle path
        cairo_save(m_hDC);
        cairo_new_path(m_hDC);
        cairo_move_to(m_hDC, x1 + rx, y1);
        cairo_line_to(m_hDC, x2 - rx, y1);
        cairo_arc(m_hDC, x2 - rx, y1 + ry, rx < ry ? rx : ry, -3.14159265358979323846/2, 0.0);
        cairo_line_to(m_hDC, x2, y2 - ry);
        cairo_arc(m_hDC, x2 - rx, y2 - ry, rx < ry ? rx : ry, 0.0, 3.14159265358979323846/2);
        cairo_line_to(m_hDC, x1 + rx, y2);
        cairo_arc(m_hDC, x1 + rx, y2 - ry, rx < ry ? rx : ry, 3.14159265358979323846/2, 3.14159265358979323846);
        cairo_line_to(m_hDC, x1, y1 + ry);
        cairo_arc(m_hDC, x1 + rx, y1 + ry, rx < ry ? rx : ry, 3.14159265358979323846, 3.0*3.14159265358979323846/2);
        cairo_close_path(m_hDC);
        cairo_stroke(m_hDC);
        cairo_restore(m_hDC);
        return TMTrue;
    }

    TMBool RoundRect(const TMRect* lpRect, TMPoint point)
    {
        if (!lpRect) return TMFalse;
        return RoundRect(RectLeft(*lpRect), RectTop(*lpRect), RectRight(*lpRect), RectBottom(*lpRect), point.x, point.y);
    }

    TMBool DrawFocusRect(const TMRect* lpRect)
    {
        if (m_hDC == NULL || !lpRect) return TMFalse;
        double saved[4];
        cairo_get_dash(m_hDC, saved, NULL);
        double dashes[] = { 1.0, 1.0 };
        cairo_save(m_hDC);
        cairo_set_dash(m_hDC, dashes, 2, 0.0);
        cairo_set_source_rgb(m_hDC, 0.0, 0.0, 1.0);
        cairo_rectangle(m_hDC, RectLeft(*lpRect), RectTop(*lpRect), RectRight(*lpRect) - RectLeft(*lpRect), RectBottom(*lpRect) - RectTop(*lpRect));
        cairo_stroke(m_hDC);
        cairo_restore(m_hDC);
        return TMTrue;
    }

    TMBool IntersectClipRect(int x1, int y1, int x2, int y2)
    {
        if (m_hDC == NULL) return TMFalse;
        cairo_rectangle(m_hDC, x1, y1, x2 - x1, y2 - y1);
        cairo_clip(m_hDC);
        return TMTrue;
    }

    TMBool IntersectClipRect(const TMRect* lpRect)
    {
        if (!lpRect) return TMFalse;
        return IntersectClipRect(RectLeft(*lpRect), RectTop(*lpRect), RectRight(*lpRect), RectBottom(*lpRect));
    }

    //extern function
    void ClearRect(const TMRect& rc, TMColor clr)
    {
        DbgAssert(this->m_hDC != NULL);
        FillRect(&rc, clr);
    }

    void SetGraphicsModeAdv(TMBool bAdv) const
    {
    }

    bool IsGraphicsModeAdv() const
    {
        return true;
    }

    TMBool GetWorldTransform(TMAffine& xform)
    {
        DbgAssert(this->m_hDC != NULL);
        cairo_get_matrix(this->m_hDC, &xform);
        return true;
    }
    
    TMBool SetWorldTransform(const TMAffine& xform)
    {
        DbgAssert(this->m_hDC != NULL);
        cairo_set_matrix(this->m_hDC, &xform);
        return true;
    }
    
    TMBool LeftMultiplyWorldTransform(const TMAffine* xform)
    {
        DbgAssert(this->m_hDC != NULL);
        cairo_matrix_t cur, result;
        cairo_get_matrix(this->m_hDC, &cur);
        cairo_matrix_multiply(&result, xform, &cur);
        cairo_set_matrix(this->m_hDC, &result);
        return true;
    }
    
    TMBool RightMultiplyWorldTransform(const TMAffine* xform)
    {
        DbgAssert(this->m_hDC != NULL);
        cairo_transform(this->m_hDC, xform);
        return true;
    }
};

typedef CDCGtkT<false>        CDCHandleGtk;
typedef CDCGtkT<true>         CDCGtk;


///////////////////////////////////////////////////////////////////////////////
// CClientDCGtk
class CClientDCGtk : public CDCGtk
{
public:
    // Constructor/destructor
    CClientDCGtk(TMHWnd hWnd)
        : CDCGtk(gdk_cairo_create(gtk_widget_get_window(GTK_WIDGET(hWnd))))
    {
        DbgAssert(hWnd == NULL);
    }
};


///////////////////////////////////////////////////////////////////////////////
// CATScreenDC
struct CScreenDCWriteGtk
    : public CDCGtk
{
    GdkWindow* m_pWindow;
    CRect m_rc;
    
    CScreenDCWriteGtk(const CRect& rc)
        : CDCGtk(NULL)
        , m_pWindow(gdk_get_default_root_window())
        , m_rc(rc)
    {
        if (m_pWindow == NULL)
            return;

        m_hDC = gdk_cairo_create(m_pWindow);
        if (m_hDC == NULL)
            return;

        cairo_rectangle(m_hDC, RectLeft(m_rc), RectTop(m_rc), RectWidth(m_rc), RectHeight(m_rc));
        cairo_clip(m_hDC);
    }
    
    ~CScreenDCWriteGtk()
    {
        if (m_hDC != NULL)
        {
            cairo_surface_t* surface = cairo_get_target(m_hDC);
            if (surface != NULL)
                cairo_surface_flush(surface);
        }

        if (m_pWindow != NULL)
        {
            gdk_window_process_updates(m_pWindow, TMTrue);
            GdkDisplay* display = gdk_window_get_display(m_pWindow);
            if (display != NULL)
                gdk_display_flush(display);
        }
    }
};

///////////////////////////////////////////////////////////////////////////////
// app
class CAppGtk
{
public:
    static inline void SetDPIAware()
    {
        // GTK handles per-monitor scaling through the active backend.
    }
    
    static inline TMBool IsDPIAware()
    {
        return TMTrue;
    }
    
    static inline void Quit(int nExitCode)
    {
        tm_unused(nExitCode);
        if (gtk_main_level() > 0)
            gtk_main_quit();
    }
    
    static void ReleaseCapture()
    {
        GtkWidget* widget = GetCapture();
        if (widget)
            gtk_grab_remove(widget);
    }
    
    static GtkWidget* GetCapture()
    {
        return gtk_grab_get_current();
    }

    static TMBool IsHighContrast()
    {
        GtkSettings* settings = gtk_settings_get_default();
        if (!settings)
            return FALSE;

        gchar* theme_name = NULL;
        g_object_get(settings, "gtk-theme-name", &theme_name, NULL);
        if (!theme_name)
            return FALSE;

        const gboolean is_hc =
            (g_ascii_strcasecmp(theme_name, "HighContrast") == 0) ||
            (g_str_has_prefix(theme_name, "HighContrast")) ||
            (g_str_has_prefix(theme_name, "High-Contrast"));

        g_free(theme_name);
        return is_hc ? TRUE : FALSE;
    }
};


TM_END_NAMESPACE



TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CRefWindowBaseT
template <class T, class TBase = CWindowGtk>
class CRefWindowGtkT
: public TBase
{
public:
    typedef CRefWindowGtkT CRefWindow;
    
    ~CRefWindowGtkT()
    {
        DbgAssert(!this->m_hWnd);
    }
    
    static TMCStr GetWndClassName() { return TM_T("TM window"); }
    
    TMResult WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
    {
        return 0;
    }
    
    void SendPostedEvents()
    {
        DbgAssert(this->m_hWnd);
    }

    void Create(TMHWnd hWndParent, const TMRect& rcPos, TMCStr szWindowName = NULL,
        TMUInt32 dwStyle = 0, TMUInt32 dwExStyle = 0,
        TMUInt nID = 0, void* lpCreateParam = NULL, TMHModule hModule = _ModuleATL.GetModuleInstance())
    {
        tm_unused(nID);
        tm_unused(hModule);
        TMRect rc(rcPos);
        this->TBase::Create(GetWndClassName(), hWndParent, rc, szWindowName, dwStyle, dwExStyle, 0, lpCreateParam);
        if (this->m_hWnd)
            gtk_widget_hide(GTK_WIDGET(this->m_hWnd));
    }
};



// template <class T>
// class CATMessageWindowT : public CRefWindowBaseT<T, CQObjectT<QObject> >
// {
// public:
//     void Create()
//     {
//         typedef typename CRefWindowBaseT<T, CQObjectT<QObject> >::CCustomObject CCustomObject;
//         this->m_hWnd = new CCustomObject(this);
//     }
// 
//     TMBool IsWindow()
//     {
//         return 0 != this->m_hWnd;
//     }
// 
//     void DestroyWindow()
//     {
//         delete this->m_hWnd;
//         this->m_hWnd = 0;
//     }
// };



TM_END_NAMESPACE



#endif //TMATL_GTK_H
