/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.4.13
 *
 *  Copyright (C) 2016 miragekiller
 */

#pragma once

#ifndef UIWINDOW_GTK_H
#define UIWINDOW_GTK_H

#include "view.h"
#include "platform.h"


#define CTuiWnd_Platform CTuiWnd_Gtk

TM_BGN_NAMESPACE

class ITheme;

///////////////////////////////////////////////////////////////////////////////
// CTuiWnd_Gtk
class CTuiWnd_Gtk
    : public CTuiWnd
    , public IRealWindow
{
private:
    TM_CLASS_INSTANCE_COUNT(CTuiWnd_Gtk);

public:
    TMHWnd              m_hWnd;     // GtkWindow* â€?the native GTK top-level window

    CTuiWnd_Gtk()
        : m_hWnd(0)
    {
    }

    ~CTuiWnd_Gtk();

    TMHWndNative GetNativeHWND() { return m_hWnd; }

    HTUI ManagerInit(TUIFACTORY* pFactory, TMHModule hInst, TMHWnd hWnd, void* param, TMBool bShowWindow, const CRect* prc);

    void GetWindowRectNow(CRect& rc);

private:
    // GTK signal callbacks â€?analogous to the Win32 WNDPROC
    static gboolean _on_configure_event(GtkWidget* widget, GdkEventConfigure* event, gpointer user_data);
    static gboolean _on_draw           (GtkWidget* widget, cairo_t* cr,              gpointer user_data);
    static gboolean _on_delete_event   (GtkWidget* widget, GdkEvent* event,          gpointer user_data);
    static gboolean _on_focus_in       (GtkWidget* widget, GdkEventFocus* event,     gpointer user_data);
    static gboolean _on_focus_out      (GtkWidget* widget, GdkEventFocus* event,     gpointer user_data);
    static gboolean _on_button_press   (GtkWidget* widget, GdkEventButton* event,    gpointer user_data);
    static gboolean _on_button_release (GtkWidget* widget, GdkEventButton* event,    gpointer user_data);
    static gboolean _on_motion_notify  (GtkWidget* widget, GdkEventMotion* event,    gpointer user_data);
    static gboolean _on_key_press      (GtkWidget* widget, GdkEventKey* event,       gpointer user_data);
    static gboolean _on_key_release    (GtkWidget* widget, GdkEventKey* event,       gpointer user_data);
    static gboolean _on_scroll         (GtkWidget* widget, GdkEventScroll* event,    gpointer user_data);

public:
    // CTuiObject
    TMResult Dispatch(TMUInt msg, TMWParam wp, TMLParam lp);

    // CTuiWgt
    virtual TMBool ProcessWindowMessage(CTuiWgt* hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) tm_override;
    virtual TMHWnd GetOverlayHWND() tm_override { return m_hWnd; }
    virtual void UpdateOnArrowDirty() tm_override { RealChangeFrame(false); }
    virtual TMBool SetWindowPos(CTuiWgt* pInsertAfter, GXDips x, GXDips y, GXDips cx, GXDips cy, TMUInt nFlags);

    // CTuiLayerBase
    virtual void CreateBkBuffer(IGXApp* gapp, const CSize& sz) tm_override;

    // CTuiLayerWnd
    virtual IRealWindow* GetReal() tm_final { return this; }

    // CTuiWnd
    virtual TMHWnd MgrGetHWND() const tm_override { return m_hWnd; }
    virtual TMBool MgrIsAllStyle(TMUInt32 dwStyle, TMUInt32 dwMask) tm_override;

    // IRealWindow
    virtual TMHWnd GetReal() const tm_final { return m_hWnd; }
    virtual CTuiLayerWnd* GetLayer() tm_final { return this; }
    virtual TMBool IsAllStyle(TMUInt32 dwStyle, TMUInt32 dwMask) tm_override;
    virtual CFrame* GetFrame() tm_override;
    virtual void RealUpdateAlpha(TMBool bUpdateNow) tm_override;
    virtual void RealChangeFrame(TMBool bPost) tm_override;
    virtual void RealInvalidate(TMBool bDrawHide = false, TMBool bUpdate = false) tm_override;

public:
#ifdef TUI_EDIT_MODE
    virtual CString GetDebugInfo() tm_override;
#endif // TUI_EDIT_MODE
};

class CViewGtk : public CView
{
public:
    TM_CLASS_INSTANCE_CHECK(CViewGtk);

    CViewGtk();
    ~CViewGtk();

    virtual void InternalSetCapture(const CTuiWgt* hWgt) tm_override;
    virtual void InternalReleaseCapture() tm_override;
    virtual TMBool InternalIsCapture(const CTuiWgt* hWgt) tm_override;
    virtual void InternalSetFocus(const CTuiWgt* hWgt) tm_override;

    virtual TMBool InternalIsWindowActive(const CTuiWgt* hWgt) tm_override;
    virtual CPoint InternalGetScreenOrg(HTUI hWgt) tm_override;

    virtual HTUI CreateDlgEx(TMHModule hInst, int nID, TMHWnd hWndParent, TUIFACTORY* pFactory, void* param) tm_override;
    virtual HTUI CreateDlgByHWND(TMHModule hInst, int nID, TMHWnd hWnd, TUIFACTORY* pFactory, void* param) tm_override;
};

TM_END_NAMESPACE


#endif //UIWINDOW_GTK_H
