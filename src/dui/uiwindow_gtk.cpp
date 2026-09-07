/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.27
 *
 *  Copyright (C) 2011 miragekiller
 */

#include "uiwindow_gtk.h"

#if defined(TM_GTK)
#include <gtk/gtk.h>

TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// CTuiWnd_Gtk

CTuiWnd_Gtk::~CTuiWnd_Gtk()
{
    if (m_hWnd)
    {
        // Detach signal handlers so callbacks don't fire after destruction
        g_signal_handlers_disconnect_by_data(G_OBJECT(m_hWnd), this);
        m_hWnd = nullptr;
    }
}

// GTK equivalent of CenterDialog()
static void _CenterDialogGtk(GtkWindow* hWnd, TMUInt32 style)
{
    if (!(style & (DS_CENTEROWNER | DS_CENTERSCREEN)))
        return;

    GtkWindow* owner = nullptr;
    if (style & DS_CENTEROWNER)
        owner = gtk_window_get_transient_for(hWnd);

    if (owner && (style & DS_CENTEROWNER))
        gtk_window_set_position(hWnd, GTK_WIN_POS_CENTER_ON_PARENT);
    else
        gtk_window_set_position(hWnd, GTK_WIN_POS_CENTER);
}

HTUI CTuiWnd_Gtk::ManagerInit(TUIFACTORY* pFactory, TMHModule hInst, TMHWnd hWnd, void* param, TMBool bShowWindow, const CRect* prc)
{
    DbgAssert(m_nRef > 0);
    DbgAssert(hWnd);   // window must already be created by the caller (CViewGtk::CreateDlgEx)

    // ── 1. Attach to the already-created native GTK window ──
    m_hWnd = hWnd;
    m_dwStyle |= WS_VISIBLE;

    // ── 2. Apply initial size from prc if provided ──
    if (prc && prc->Width() > 0 && prc->Height() > 0)
        gtk_window_resize(m_hWnd, prc->Width(), prc->Height());

    // ── 3. Centering (�?CenterDialog) ──
    _CenterDialogGtk(m_hWnd, m_dwStyle);

    // ── 4. Register GTK signal handlers (�?installing WNDPROC / Subclass_wself) ──
    GtkWidget* widget = GTK_WIDGET(m_hWnd);

    gtk_widget_add_events(widget,
        GDK_STRUCTURE_MASK          |
        GDK_BUTTON_PRESS_MASK       |
        GDK_BUTTON_RELEASE_MASK     |
        GDK_POINTER_MOTION_MASK     |
        GDK_KEY_PRESS_MASK          |
        GDK_KEY_RELEASE_MASK        |
        GDK_SCROLL_MASK             |
        GDK_FOCUS_CHANGE_MASK       |
        GDK_LEAVE_NOTIFY_MASK);

    g_signal_connect(widget, "configure-event",     G_CALLBACK(_on_configure_event), this);
    g_signal_connect(widget, "draw",                G_CALLBACK(_on_draw),            this);
    g_signal_connect(widget, "delete-event",        G_CALLBACK(_on_delete_event),    this);
    g_signal_connect(widget, "focus-in-event",      G_CALLBACK(_on_focus_in),        this);
    g_signal_connect(widget, "focus-out-event",     G_CALLBACK(_on_focus_out),       this);
    g_signal_connect(widget, "button-press-event",  G_CALLBACK(_on_button_press),    this);
    g_signal_connect(widget, "button-release-event",G_CALLBACK(_on_button_release),  this);
    g_signal_connect(widget, "motion-notify-event", G_CALLBACK(_on_motion_notify),   this);
    g_signal_connect(widget, "key-press-event",     G_CALLBACK(_on_key_press),       this);
    g_signal_connect(widget, "key-release-event",   G_CALLBACK(_on_key_release),     this);
    g_signal_connect(widget, "scroll-event",        G_CALLBACK(_on_scroll),          this);

    // ── 5. InitManager �?builds the TUI object tree (�?CTuiWnd::InitManager) ──
    if (!CTuiWnd::InitManager(pFactory, param))
        return nullptr;

    if (IsLifeDestroyed())
        return nullptr;

    // ── 6. Show window ──
    if (bShowWindow)
        gtk_widget_show_all(widget);

    return this;
}

// ── GTK signal callback implementations ──

gboolean CTuiWnd_Gtk::_on_configure_event(GtkWidget* widget, GdkEventConfigure* event, gpointer user_data)
{
    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(user_data);
    Ref ref(p);
    if (p->IsLifeInvalid())
        return FALSE;

    // Push new pixel size into the TUI layout system
    CZoom zoom(p->m_nDPI);
    GXDips w = zoom.DPtoLP(event->width);
    GXDips h = zoom.DPtoLP(event->height);
    CTuiWgt::SetWindowPos(nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

    // Notify TUI children of show/hide if visibility changed
    return FALSE;
}

gboolean CTuiWnd_Gtk::_on_draw(GtkWidget* widget, cairo_t* cr, gpointer user_data)
{
    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(user_data);
    Ref ref(p);
    if (p->IsLifeInvalid())
        return FALSE;

    // Ask to render the scene; the result is blitted via Cairo
    ERenderBK eRet = p->RenderScene(_gxApp);
    if (eRet == eRenderBkOk && p->m_txBuffer)
    {
        // Composite the Cairo surface stored in m_txBuffer onto the window
        cairo_surface_t* surf = (cairo_surface_t*)p->m_txBuffer->GetNativeHandle();
        if (surf)
        {
            cairo_set_source_surface(cr, surf, 0, 0);
            cairo_paint(cr);
        }
    }
    else if (eRet == eRenderBkResize)
    {
        p->RealChangeFrame(true);
    }
    return TRUE;
}

gboolean CTuiWnd_Gtk::_on_delete_event(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(user_data);
    Ref ref(p);
    // Dispatch WM_CLOSE equivalent so TUI can confirm or cancel
    TMResult lr = 0;
    p->ProcessWindowMessage(p, WM_CLOSE, 0, 0, lr);
    // Returning TRUE prevents default destroy �?TUI will call DestroyWindow when ready
    return TRUE;
}

gboolean CTuiWnd_Gtk::_on_focus_in(GtkWidget* widget, GdkEventFocus* event, gpointer user_data)
{
    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(user_data);
    Ref ref(p);
    if (!p->IsLifeInvalid())
    {
        g_input.FocusOnSet(*p);
        p->SetDefaultFocus(SWP_SHOWWINDOW);
    }
    return FALSE;
}

gboolean CTuiWnd_Gtk::_on_focus_out(GtkWidget* widget, GdkEventFocus* event, gpointer user_data)
{
    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(user_data);
    Ref ref(p);
    if (!p->IsLifeInvalid())
    {
        g_input.FocusOnKill(*p);
        p->SetDefaultFocus(SWP_HIDEWINDOW);
    }
    return FALSE;
}

gboolean CTuiWnd_Gtk::_on_button_press(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(user_data);
    Ref ref(p);
    if (p->IsLifeInvalid())
        return FALSE;
    TMUInt uMsg = (event->type == GDK_2BUTTON_PRESS)   ? WM_LBUTTONDBLCLK :
                  (event->type == GDK_3BUTTON_PRESS)   ? WM_LBUTTONDBLCLK :
                  (event->button == GDK_BUTTON_PRIMARY) ? WM_LBUTTONDOWN :
                  (event->button == GDK_BUTTON_MIDDLE)  ? WM_MBUTTONDOWN  : WM_RBUTTONDOWN;
    TMWParam wp = 0;
    CPointI pt((int)event->x, (int)event->y);
    TMResult lr = 0;
    p->GetView()->DispatchAppMessage(p, p->m_hWnd, uMsg, wp, pt.ToLPARAM(), lr);
    return FALSE;
}

gboolean CTuiWnd_Gtk::_on_button_release(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(user_data);
    Ref ref(p);
    if (p->IsLifeInvalid())
        return FALSE;
    TMUInt uMsg = (event->button == GDK_BUTTON_PRIMARY) ? WM_LBUTTONUP :
                  (event->button == GDK_BUTTON_MIDDLE)  ? WM_MBUTTONUP  : WM_RBUTTONUP;
    CPointI pt((int)event->x, (int)event->y);
    TMResult lr = 0;
    p->GetView()->DispatchAppMessage(p, p->m_hWnd, uMsg, 0, pt.ToLPARAM(), lr);
    return FALSE;
}

gboolean CTuiWnd_Gtk::_on_motion_notify(GtkWidget* widget, GdkEventMotion* event, gpointer user_data)
{
    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(user_data);
    Ref ref(p);
    if (p->IsLifeInvalid())
        return FALSE;
    CPointI pt((int)event->x, (int)event->y);
    TMResult lr = 0;
    p->GetView()->DispatchAppMessage(p, p->m_hWnd, WM_MOUSEMOVE, 0, pt.ToLPARAM(), lr);
    return FALSE;
}

gboolean CTuiWnd_Gtk::_on_key_press(GtkWidget* widget, GdkEventKey* event, gpointer user_data)
{
    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(user_data);
    Ref ref(p);
    if (p->IsLifeInvalid())
        return FALSE;
    TMResult lr = 0;
    p->GetView()->DispatchAppMessage(p, p->m_hWnd, WM_KEYDOWN, event->keyval, (TMLParam)event->hardware_keycode, lr);
    return (TMBool)lr ? TRUE : FALSE;
}

gboolean CTuiWnd_Gtk::_on_key_release(GtkWidget* widget, GdkEventKey* event, gpointer user_data)
{
    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(user_data);
    Ref ref(p);
    if (p->IsLifeInvalid())
        return FALSE;
    TMResult lr = 0;
    p->GetView()->DispatchAppMessage(p, p->m_hWnd, WM_KEYUP, event->keyval, (TMLParam)event->hardware_keycode, lr);
    return FALSE;
}

gboolean CTuiWnd_Gtk::_on_scroll(GtkWidget* widget, GdkEventScroll* event, gpointer user_data)
{
    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(user_data);
    Ref ref(p);
    if (p->IsLifeInvalid())
        return FALSE;
    int delta = 0;
    if      (event->direction == GDK_SCROLL_UP)   delta =  WHEEL_DELTA;
    else if (event->direction == GDK_SCROLL_DOWN)  delta = -WHEEL_DELTA;
    else if (event->direction == GDK_SCROLL_SMOOTH) delta = (int)(-event->delta_y * WHEEL_DELTA);
    if (delta)
    {
        CPointI pt((int)event->x, (int)event->y);
        TMResult lr = 0;
        p->GetView()->DispatchAppMessage(p, p->m_hWnd, WM_MOUSEWHEEL,
            MAKEWPARAM(0, (short)delta), pt.ToLPARAM(), lr);
    }
    return FALSE;
}

void CTuiWnd_Gtk::GetWindowRectNow(CRect& rc)
{
    int x = 0, y = 0, w = 0, h = 0;
    gtk_window_get_position(m_hWnd, &x, &y);
    gtk_window_get_size(m_hWnd, &w, &h);
    rc.SetRect(x, y, x + w, y + h);
}

TMResult CTuiWnd_Gtk::Dispatch(TMUInt msg, TMWParam wp, TMLParam lp)
{
    TMResult lr = 0;
    ProcessWindowMessage(this, msg, wp, lp, lr);
    return lr;
}

TMBool CTuiWnd_Gtk::ProcessWindowMessage(CTuiWgt* hWnd, TMUInt uMsg, TMWParam wParam,
    TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    return CTuiWnd::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
}

TMBool CTuiWnd_Gtk::SetWindowPos(CTuiWgt* pInsertAfter, GXDips x, GXDips y, GXDips cx, GXDips cy, TMUInt nFlags)
{
    if (!m_hWnd)
        return false;
    if (!(nFlags & SWP_NOMOVE))
        gtk_window_move(m_hWnd, (gint)x, (gint)y);
    if (!(nFlags & SWP_NOSIZE))
        gtk_window_resize(m_hWnd, (gint)cx, (gint)cy);
    return true;
}

void CTuiWnd_Gtk::CreateBkBuffer(IGXApp* gapp, const CSize& sz)
{
    // On GTK the backing surface is managed by Cairo/GDK; nothing to allocate here.
}

TMBool CTuiWnd_Gtk::MgrIsAllStyle(TMUInt32 dwStyle, TMUInt32 dwMask)
{
    return (m_dwStyle & dwMask) == (dwStyle & dwMask);
}

// IRealWindow
TMBool CTuiWnd_Gtk::IsAllStyle(TMUInt32 dwStyle, TMUInt32 dwMask)
{
    return MgrIsAllStyle(dwStyle, dwMask);
}

CFrame* CTuiWnd_Gtk::GetFrame()
{
    return CTuiLayerWnd::GetFirst<CFrame>(0);
}

void CTuiWnd_Gtk::RealUpdateAlpha(TMBool bUpdateNow)
{
    if (m_hWnd)
        gtk_widget_queue_draw(GTK_WIDGET(m_hWnd));
}

void CTuiWnd_Gtk::RealChangeFrame(TMBool bPost)
{
    if (m_hWnd)
        gtk_widget_queue_draw(GTK_WIDGET(m_hWnd));
}

void CTuiWnd_Gtk::RealInvalidate(TMBool bDrawHide, TMBool bUpdate)
{
    if (!m_hWnd)
        return;
    if (!bDrawHide && !gtk_widget_get_visible(GTK_WIDGET(m_hWnd)))
        return;
    if (bUpdate)
        gdk_window_process_updates(gtk_widget_get_window(GTK_WIDGET(m_hWnd)), TRUE);
    else
        gtk_widget_queue_draw(GTK_WIDGET(m_hWnd));
}

///////////////////////////////////////////////////////////////////////////////
// CViewGtk

CViewGtk::CViewGtk() {}
CViewGtk::~CViewGtk() {}

void CViewGtk::InternalSetCapture(const CTuiWgt* hWgt)
{
    if (hWgt && hWgt->GetHWND())
        gtk_grab_add(GTK_WIDGET(hWgt->GetHWND()));
}

void CViewGtk::InternalReleaseCapture()
{
    CAppAT::ReleaseCapture();
}

TMBool CViewGtk::InternalIsCapture(const CTuiWgt* hWgt)
{
    return hWgt && hWgt->GetHWND() && CAppAT::GetCapture() == GTK_WIDGET(hWgt->GetHWND());
}

void CViewGtk::InternalSetFocus(const CTuiWgt* hWgt)
{
    if (!hWgt || !hWgt->GetHWND())
        return;
    gtk_window_present(hWgt->GetHWND());
    CWindowAT(hWgt->GetHWND()).SetFocus();
}

TMBool CViewGtk::InternalIsWindowActive(const CTuiWgt* hWgt)
{
    return hWgt && hWgt->GetHWND() && gtk_window_is_active(hWgt->GetHWND());
}

CPoint CViewGtk::InternalGetScreenOrg(HTUI hWgt)
{
    int x = 0;
    int y = 0;
    if (hWgt && hWgt->GetHWND())
        gtk_window_get_position(hWgt->GetHWND(), &x, &y);
    return CPoint(x, y);
}

HTUI CViewGtk::CreateDlgEx(TMHModule hInst, int nID, TMHWnd hWndParent, TUIFACTORY* pFactory, void* param)
{
    // ── 1. Load XML and create the CTuiWnd object tree ──
    CTuiWnd* pMgr = CreateMgr(hInst, nID, TUI_DPI_SYSTEM_DEFAULT);
    if (!pMgr)
        return nullptr;

    // ── 2. Create native GTK window ──
    TMUInt32 dwStyle = pMgr->GetStyle();
    TMBool bVisible  = (dwStyle & WS_VISIBLE) != 0;

    GtkWindowType wtype = (dwStyle & WS_POPUP) ? GTK_WINDOW_POPUP : GTK_WINDOW_TOPLEVEL;
    GtkWindow* hWnd = GTK_WINDOW(gtk_window_new(wtype));
    if (!hWnd)
    {
        pMgr->UnInitOnManagerFail();
        return nullptr;
    }

    // Compute initial pixel size from TUI bound rect
    CRect rc;
    CTuiZoom zoom(pMgr->GetDPI());
    zoom.LPtoDP(pMgr->GetBoundRect(), rc);
    if (rc.Width() > 0 && rc.Height() > 0)
        gtk_window_set_default_size(hWnd, rc.Width(), rc.Height());

    // Window title (TUI uses wide strings; GTK needs UTF-8)
    {
        CStrPtr title = pMgr->GetWindowText();
        if (title && *title)
        {
            gchar* utf8 = g_utf16_to_utf8((const gunichar2*)title.c_str(), -1, NULL, NULL, NULL);
            if (utf8) { gtk_window_set_title(hWnd, utf8); g_free(utf8); }
        }
    }

    // Set transient parent
    if (hWndParent)
        gtk_window_set_transient_for(hWnd, hWndParent);

    // ── 3. Let ManagerInit wire up signals and init the TUI tree ──
    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(pMgr);
    Ref ref(pMgr);
    if (!p->ManagerInit(pFactory, hInst, hWnd, param, bVisible, &rc))
        return nullptr;

    return p;
}

HTUI CViewGtk::CreateDlgByHWND(TMHModule hInst, int nID, TMHWnd hWnd, TUIFACTORY* pFactory, void* param)
{
    // Caller supplies an already-existing GtkWindow �?just attach to it.
    CTuiWnd* pMgr = CreateMgr(hInst, nID, TUI_DPI_SYSTEM_DEFAULT);
    if (!pMgr)
        return nullptr;

    CTuiWnd_Gtk* p = static_cast<CTuiWnd_Gtk*>(pMgr);
    Ref ref(pMgr);
    if (!p->ManagerInit(pFactory, hInst, hWnd, param, false, nullptr))
        return nullptr;

    return p;
}

TM_END_NAMESPACE

TM_BGN_PRIVATE_NAMESPACE

static const char* kDialogEndProp = "TUI_ENDDIALOG_PROP";

#ifndef TUI_EDIT_MODE
TM::CTuiApp* tuiGetAPP(TM::HTUI hWnd)
{
    if (hWnd)
    {
        ASSERT_CLASS(hWnd);
        return hWnd->GetView();
    }

    static TM::CViewGtk s_view;
    return &s_view;
}
#endif

int tuiDoDialogBox(TM::HTUI hMgr, TMHWnd hOwner)
{
    ASSERT_CLASS(hMgr);

    GtkWidget* dlg = GTK_WIDGET(hMgr->GetHWND());
    if (!dlg)
        return -1;

    GtkWidget* owner = hOwner ? GTK_WIDGET(hOwner) : NULL;
    if (owner)
        gtk_widget_set_sensitive(owner, FALSE);

    g_object_set_data(G_OBJECT(dlg), kDialogEndProp, NULL);
    gtk_widget_show(dlg);

    int result = 0;
    while (GTK_IS_WIDGET(dlg) && gtk_widget_get_visible(dlg))
    {
        gpointer done = g_object_get_data(G_OBJECT(dlg), kDialogEndProp);
        if (done)
        {
            result = GPOINTER_TO_INT(done) - 1;
            break;
        }
        g_main_context_iteration(NULL, TRUE);
    }

    if (owner)
        gtk_widget_set_sensitive(owner, TRUE);

    return result;
}

TMBool tuiEndDialog(TM::HTUI hMgr, int nCode)
{
    ASSERT_CLASS(hMgr);

    GtkWidget* dlg = GTK_WIDGET(hMgr->GetHWND());
    if (!dlg)
        return false;

    g_object_set_data(G_OBJECT(dlg), kDialogEndProp, GINT_TO_POINTER(nCode + 1));
    gtk_widget_hide(dlg);
    return true;
}

TM_END_PRIVATE_NAMESPACE

#endif // TM_GTK
