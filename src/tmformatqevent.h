/*
 *  @file
 *  @brief  qt event -> string
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.3.12
 *
 *  Copyright (C) 2025 miragekiller
 */

#pragma once

#ifndef TMFORMATQEVENT_H
#define TMFORMATQEVENT_H

#include <QtCore/QEvent>
#include <QtWidgets/QGraphicsSceneEvent>
#include "tmstring.h"


inline TM::CString DbgFormatQEvent(QActionEvent* e, LPCTSTR s)
{
     TM::CString str(s);
     return str.AppendFormat(L" action:%p before:%p", e->action(), e->before());
}

inline TM::CString DbgFormatQEvent(QChildEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.AppendFormat(L" child:%p", e->child());
}

inline TM::CString DbgFormatQEvent(QDynamicPropertyChangeEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    str += L" propertyName:";
    str += e->propertyName().data();
    return str;
}

inline TM::CString DbgFormatQEvent(QFileOpenEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    str += L" file:";
    str += (TMCStrW)e->file().utf16();
    str += L" url:";
    str += (TMCStrW)e->url().toString().utf16();
    return str;
}

inline TM::CString DbgFormatQEvent(QFocusEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.AppendFormat(L" reason:%d", e->reason());
}

inline TM::CString DbgFormatQEvent(QGraphicsSceneEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.AppendFormat(L" timestamp:%p widget:%p", e->timestamp(), e->widget());
}

inline TM::CString DbgFormatQEvent(QGraphicsSceneContextMenuEvent* e, LPCTSTR s)
{
    QGraphicsSceneEvent* p = e;
    return DbgFormatQEvent(p, s).
        AppendFormat(L" modifiers:%x pos:%g,%g reason:%d", e->modifiers(), e->pos().x(), e->pos().y()).
        AppendFormat(L" scenePos:%g,%g", e->scenePos().x(), e->scenePos().y()).
        AppendFormat(L" screenPos:%d,%d", e->screenPos().x(), e->screenPos().y());
}

inline TM::CString DbgFormatQEvent(QGraphicsSceneDragDropEvent* e, LPCTSTR s)
{
    QGraphicsSceneEvent* p = e;
    return DbgFormatQEvent(p, s);
}

inline TM::CString DbgFormatQEvent(QGraphicsSceneHelpEvent* e, LPCTSTR s)
{
    QGraphicsSceneEvent* p = e;
    return DbgFormatQEvent(p, s).
        AppendFormat(L" scenePos:%g,%g screenPos:%d,%d",
        e->scenePos().x(), e->scenePos().y(), e->screenPos().x(), e->screenPos().y());
}

inline TM::CString DbgFormatQEvent(QGraphicsSceneHoverEvent* e, LPCTSTR s)
{
    QGraphicsSceneEvent* p = e;
    return DbgFormatQEvent(p, s).
        AppendFormat(L" lastPos:%g,%g", e->lastPos().x(), e->lastPos().y()).
        AppendFormat(L" lastScenePos:%g,%g", e->lastScenePos().x(), e->lastScenePos().y()).
        AppendFormat(L" lastScreenPos:%d,%d", e->lastScreenPos().x(), e->lastScreenPos().y()).
        AppendFormat(L" modifiers:%x", e->modifiers()).
        AppendFormat(L" pos:%g,%g", e->pos().x(), e->pos().y()).
        AppendFormat(L" scenePos:%g,%g", e->scenePos().x(), e->scenePos().y()).
        AppendFormat(L" screenPos:%d,%d", e->screenPos().x(), e->screenPos().y());
}

inline TM::CString DbgFormatQEvent(QGraphicsSceneMouseEvent* e, LPCTSTR s)
{
    QGraphicsSceneEvent* p = e;
    return DbgFormatQEvent(p, s).
        AppendFormat(L" button:%x buttons:%x flags:%x", e->button(), e->buttons(), e->flags()).
        AppendFormat(L" lastPos:%g,%g", e->lastPos().x(), e->lastPos().y()).
        AppendFormat(L" lastScenePos:%g,%g", e->lastScenePos().x(), e->lastScenePos().y()).
        AppendFormat(L" lastScreenPos:%d,%d", e->lastScreenPos().x(), e->lastScreenPos().y()).
        AppendFormat(L" modifiers:%x", e->modifiers()).
        AppendFormat(L" pos:%g,%g", e->pos().x(), e->pos().y()).
        AppendFormat(L" scenePos:%g,%g", e->scenePos().x(), e->scenePos().y()).
        AppendFormat(L" screenPos:%d,%d", e->screenPos().x(), e->screenPos().y()).
        AppendFormat(L" source:%x", e->source());
}

inline TM::CString DbgFormatQEvent(QGraphicsSceneMoveEvent* e, LPCTSTR s)
{
    QGraphicsSceneEvent* p = e;
    return DbgFormatQEvent(p, s).
        AppendFormat(L" newPos:%g,%g", e->newPos().x(), e->newPos().y()).
        AppendFormat(L" oldPos:%g,%g", e->oldPos().x(), e->oldPos().y());
}

inline TM::CString DbgFormatQEvent(QGraphicsSceneResizeEvent* e, LPCTSTR s)
{
    QGraphicsSceneEvent* p = e;
    return DbgFormatQEvent(p, s).
        AppendFormat(L" newSize:%g,%g", e->newSize().width(), e->newSize().height()).
        AppendFormat(L" oldSize:%g,%g", e->oldSize().width(), e->oldSize().height());
}

inline TM::CString DbgFormatQEvent(QGraphicsSceneWheelEvent* e, LPCTSTR s)
{
    QGraphicsSceneEvent* p = e;
    return DbgFormatQEvent(p, s).
        AppendFormat(L" buttons:%x delta:%d isInverted:%d", e->buttons(), e->delta(), e->isInverted()).
        AppendFormat(L" modifiers:%x orientation:%d phase:%d", e->modifiers(), e->orientation(), e->phase()).
        AppendFormat(L" pixelDelta:%d,%d", e->pixelDelta().x(), e->pixelDelta().y()).
        AppendFormat(L" pos:%g,%g", e->pos().x(), e->pos().y()).
        AppendFormat(L" scenePos:%g,%g", e->scenePos().x(), e->scenePos().y()).
        AppendFormat(L" screenPos:%d,%d", e->screenPos().x(), e->screenPos().y());
}

inline TM::CString DbgFormatQEvent(QHelpEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.
        AppendFormat(L" globalPos:%d,%d", e->globalPos().x(), e->globalPos().y()).
        AppendFormat(L" pos:%d,%d", e->pos().x(), e->pos().y());
}
 
inline TM::CString DbgFormatQEvent(QInputEvent* e, LPCTSTR s)
{
    TM::CString str(s);
//     return str.
//         AppendFormat(L" device:%p deviceType:%x", e->device(), e->deviceType()).
//         AppendFormat(L" modifiers:%x timestamp:%p", e->modifiers(), e->timestamp());
    return str.
        AppendFormat(L" modifiers:%x", e->modifiers());
}

inline TM::CString DbgFormatQEvent(QContextMenuEvent* e, LPCTSTR s)
{
    QInputEvent* p = e;
    return DbgFormatQEvent(p, s).
        AppendFormat(L" globalPos:%d,%d", e->globalPos().x(), e->globalPos().y()).
        AppendFormat(L" pos:%d,%d", e->pos().x(), e->pos().y()).
        AppendFormat(L" reason:%d", e->reason());
}
 
inline TM::CString DbgFormatQEvent(QKeyEvent* e, LPCTSTR s)
{
    QInputEvent* p = e;
    return DbgFormatQEvent(p, s).
        AppendFormat(L" count:%d", e->count()).
        AppendFormat(L" isAutoRepeat:%d", e->isAutoRepeat()).
        AppendFormat(L" key:%d", e->key()).
        AppendFormat(L" modifiers:%x", e->modifiers()).
        AppendFormat(L" nativeModifiers:%x", e->nativeModifiers()).
        AppendFormat(L" nativeScanCode:%d", e->nativeScanCode()).
        AppendFormat(L" nativeVirtualKey:%d", e->nativeVirtualKey()).
        AppendFormat(L" text:%s", e->text().utf16());
}

inline TM::CString DbgFormatQEvent(QPointerEvent* e, LPCTSTR s)
{
    QInputEvent* p = e;
//     return DbgFormatQEvent(p, s).
//         AppendFormat(L" pointCount:%d", (int)e->pointCount()).
//         AppendFormat(L" pointerType:%d", e->pointerType()).
//         AppendFormat(L" pointingDevice:%p", e->pointingDevice());
    return DbgFormatQEvent(p, s);
}

inline TM::CString DbgFormatQEvent(QSinglePointEvent* e, LPCTSTR s)
{
    QPointerEvent* p = e;
    return DbgFormatQEvent(p, s).
//         AppendFormat(L" button:%x", e->button()).
//         AppendFormat(L" buttons:%x", e->buttons()).
//         AppendFormat(L" exclusivePointGrabber:%p", e->exclusivePointGrabber()).
        AppendFormat(L" globalPosition:%g,%g", e->globalPosition().x(), e->globalPosition().y()).
        AppendFormat(L" position:%g,%g", e->position().x(), e->position().y()).
        AppendFormat(L" scenePosition:%g,%g", e->scenePosition().x(), e->scenePosition().y());
}

inline TM::CString DbgFormatQEvent(QEnterEvent* e, LPCTSTR s)
{
    QSinglePointEvent* p = e;
    return DbgFormatQEvent(p, s);
}

inline TM::CString DbgFormatQEvent(QHoverEvent* e, LPCTSTR s)
{
    QSinglePointEvent* p = e;
    return DbgFormatQEvent(p, s).
        AppendFormat(L" oldPos:%d,%d", e->oldPos().x(), e->oldPos().y());
}

inline TM::CString DbgFormatQEvent(QMouseEvent* e, LPCTSTR s)
{
    QSinglePointEvent* p = e;
    return DbgFormatQEvent(p, s).
        AppendFormat(L" flags:%x", e->flags());
}

inline TM::CString DbgFormatQEvent(QWheelEvent* e, LPCTSTR s)
{
    QSinglePointEvent* p = e;
    return DbgFormatQEvent(p, s).
        AppendFormat(L" angleDelta:%d,%d", e->angleDelta().x(), e->angleDelta().y()).
        AppendFormat(L" inverted:%d", e->inverted()).
        AppendFormat(L" button:%d", e->phase()).
        AppendFormat(L" pixelDelta:%d,%d", e->pixelDelta().x(), e->pixelDelta().y());
}

inline TM::CString DbgFormatQEvent(QMoveEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.
        AppendFormat(L" oldPos:%d,%d", e->oldPos().x(), e->oldPos().y()).
        AppendFormat(L" pos:%d,%d", e->pos().x(), e->pos().y());
}

inline TM::CString DbgFormatQEvent(QPaintEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.
        AppendFormat(L" rect:%d,%d,%d,%d", e->rect().x(), e->rect().y(), e->rect().right(), e->rect().bottom());
}

inline TM::CString DbgFormatQEvent(QPlatformSurfaceEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.
        AppendFormat(L" surfaceEventType:%d", e->surfaceEventType());
}

inline TM::CString DbgFormatQEvent(QResizeEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.
        AppendFormat(L" oldSize:%d,%d", e->oldSize().width(), e->oldSize().height()).
        AppendFormat(L" size:%d,%d", e->size().width(), e->size().height());
}

inline TM::CString DbgFormatQEvent(QScrollEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.
        AppendFormat(L" contentPos:%g,%g", e->contentPos().x(), e->contentPos().y()).
        AppendFormat(L" overshootDistance:%g,%g", e->overshootDistance().x(), e->overshootDistance().y()).
        AppendFormat(L" scrollState:%d", e->scrollState());
}

inline TM::CString DbgFormatQEvent(QScrollPrepareEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.
        AppendFormat(L" contentPos:%g,%g", e->contentPos().x(), e->contentPos().y()).
        AppendFormat(L" contentPosRange:%g,%g,%g,%g", e->contentPosRange().x(), e->contentPosRange().y(),
            e->contentPosRange().right(), e->contentPosRange().bottom()).
        AppendFormat(L" startPos:%g,%g", e->startPos().x(), e->startPos().y()).
        AppendFormat(L" viewportSize:%g,%g", e->viewportSize().width(), e->viewportSize().height());
}

inline TM::CString DbgFormatQEvent(QShortcutEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.
        AppendFormat(L" isAmbiguous:%d", e->isAmbiguous()).
        AppendFormat(L" key:%s", e->key().toString().utf16());
}

inline TM::CString DbgFormatQEvent(QStatusTipEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.
        AppendFormat(L" tip:%s", e->tip().utf16());
}

inline TM::CString DbgFormatQEvent(QTimerEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.
        AppendFormat(L" timerId:%d", e->timerId());
}

inline TM::CString DbgFormatQEvent(QWhatsThisClickedEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.
        AppendFormat(L" href:%s", e->href().utf16());
}

inline TM::CString DbgFormatQEvent(QWindowStateChangeEvent* e, LPCTSTR s)
{
    TM::CString str(s);
    return str.
        AppendFormat(L" oldState:%x", e->oldState());
}



//QChildWindowEvent // 6.7 //QEvent::ChildWindowAdded or QEvent::ChildWindowRemoved.
//QCloseEvent
// QDragLeaveEvent
// QDropEvent QDragMoveEvent
//QExposeEvent
// QGestureEvent 
// QHideEvent
// QIconDragEvent
// QNativeGestureEvent  x
// QTabletEvent x
// QTouchEvent x
// QInputMethodEvent  x
// QInputMethodQueryEvent x
// QShowEvent
// QStateMachine::SignalEvent x
// QStateMachine::WrappedEvent x

inline TM::CString DbgFormatQEvent(QEvent* e)
{
    static TM::CString s_str;

#define DBG_STR(id)         case QEvent::id: return TM_T(#id);
#define DBG_STR2(id,E)      case QEvent::id: return DbgFormatQEvent((E*)e, TM_T(#id));
    switch(e->type())
    {
        DBG_STR(None); // = 0,                               // invalid event
        DBG_STR2(Timer, QTimerEvent); //  = 1,                              // timer event
        DBG_STR2(MouseButtonPress, QMouseEvent); //  = 2,                   // mouse button pressed
        DBG_STR2(MouseButtonRelease, QMouseEvent); //  = 3,                 // mouse button released
        DBG_STR2(MouseButtonDblClick, QMouseEvent); //  = 4,                // mouse button double click
        DBG_STR2(MouseMove, QMouseEvent); //  = 5,                          // mouse move
        DBG_STR2(KeyPress, QKeyEvent); //  = 6,                           // key pressed
        DBG_STR2(KeyRelease, QKeyEvent); //  = 7,                         // key released
        
        DBG_STR2(FocusIn, QFocusEvent); //  = 8,                            // keyboard focus received
        DBG_STR2(FocusOut, QFocusEvent); //  = 9,                           // keyboard focus lost
        DBG_STR2(FocusAboutToChange, QFocusEvent); //  = 23,                // keyboard focus is about to be lost
        
        DBG_STR2(Enter, QEnterEvent ); //  = 10,                             // mouse enters widget
        DBG_STR(Leave); //  = 11,                             // mouse leaves widget
        DBG_STR2(Paint, QPaintEvent); //  = 12,                             // paint widget
        DBG_STR2(Move, QMoveEvent); //  = 13,                              // move widget
        DBG_STR2(Resize, QResizeEvent); //  = 14,                            // resize widget
        DBG_STR(Create); //  = 15,                            // after widget creation
        DBG_STR(Destroy); //  = 16,                           // during widget destruction
        DBG_STR(Show); //  = 17,                              // widget is shown
        DBG_STR(Hide); //  = 18,                              // widget is hidden
        DBG_STR(Close); //  = 19,                             // request to close widget
        DBG_STR(Quit); //  = 20,                              // request to quit application
        DBG_STR(ParentChange); //  = 21,                      // widget has been reparented
        DBG_STR(ParentAboutToChange); //  = 131,              // sent just before the parent change is done
        DBG_STR(ThreadChange); //  = 22,                      // object has changed threads
        DBG_STR(WindowActivate); //  = 24,                    // window was activated
        DBG_STR(WindowDeactivate); //  = 25,                  // window was deactivated
        DBG_STR(ShowToParent); //  = 26,                      // widget is shown to parent
        DBG_STR(HideToParent); //  = 27,                      // widget is hidden to parent
        DBG_STR2(Wheel, QWheelEvent); //  = 31,                             // wheel event
        DBG_STR(WindowTitleChange); //  = 33,                 // window title changed
        DBG_STR(WindowIconChange); //  = 34,                  // icon changed
        DBG_STR(ApplicationWindowIconChange); //  = 35,       // application icon changed
        DBG_STR(ApplicationFontChange); //  = 36,             // application font changed
        DBG_STR(ApplicationLayoutDirectionChange); //  = 37,  // application layout direction changed
        DBG_STR(ApplicationPaletteChange); //  = 38,          // application palette changed
        DBG_STR(PaletteChange); //  = 39,                     // widget palette changed
        DBG_STR(Clipboard); //  = 40,                         // internal clipboard event
        DBG_STR(Speech); //  = 42,                            // reserved for speech input
        DBG_STR(MetaCall); //  =  43,                         // meta call event
        DBG_STR(SockAct); //  = 50,                           // socket activation
        DBG_STR(WinEventAct); //  = 132,                      // win event activation
        DBG_STR(DeferredDelete); //  = 52,                    // deferred delete event
        DBG_STR(DragEnter); //  = 60,                         // drag moves into widget
        DBG_STR(DragMove); //  = 61,                          // drag moves in widget
        DBG_STR(DragLeave); //  = 62,                         // drag leaves or is cancelled
        DBG_STR(Drop); //  = 63,                              // actual drop
        DBG_STR(DragResponse); //  = 64,                      // drag accepted/rejected

        DBG_STR2(ChildAdded, QChildEvent); //  = 68,                        // new child widget
        DBG_STR2(ChildPolished, QChildEvent); //  = 69,                     // polished child widget
        DBG_STR2(ChildRemoved, QChildEvent); //  = 71,                      // deleted child widget
        
        DBG_STR(ShowWindowRequest); //  = 73,                 // widget's window should be mapped
        DBG_STR(PolishRequest); //  = 74,                     // widget should be polished
        DBG_STR(Polish); //  = 75,                            // widget is polished
        DBG_STR(LayoutRequest); //  = 76,                     // widget should be relayouted
        DBG_STR(UpdateRequest); //  = 77,                     // widget should be repainted
        DBG_STR(UpdateLater); //  = 78,                       // request update() later

        DBG_STR(EmbeddingControl); //  = 79,                  // ActiveX embedding
        DBG_STR(ActivateControl); //  = 80,                   // ActiveX activation
        DBG_STR(DeactivateControl); //  = 81,                 // ActiveX deactivation
        DBG_STR2(ContextMenu, QContextMenuEvent); //  = 82,                       // context popup menu
        DBG_STR(InputMethod); //  = 83,                       // input method
        DBG_STR(TabletMove); //  = 87,                        // Wacom tablet event
        DBG_STR(LocaleChange); //  = 88,                      // the system locale changed
        DBG_STR(LanguageChange); //  = 89,                    // the application language changed
        DBG_STR(LayoutDirectionChange); //  = 90,             // the layout direction changed
        DBG_STR(Style); //  = 91,                             // internal style event
        DBG_STR(TabletPress); //  = 92,                       // tablet press
        DBG_STR(TabletRelease); //  = 93,                     // tablet release
        DBG_STR(OkRequest); //  = 94,                         // CE (Ok) button pressed
        DBG_STR(HelpRequest); //  = 95,                       // CE (?)  button pressed

        DBG_STR(IconDrag); //  = 96,                          // proxy icon dragged

        DBG_STR(FontChange); //  = 97,                        // font has changed
        DBG_STR(EnabledChange); //  = 98,                     // enabled state has changed
        DBG_STR(ActivationChange); //  = 99,                  // window activation has changed
        DBG_STR(StyleChange); //  = 100,                      // style has changed
        DBG_STR(IconTextChange); //  = 101,                   // icon text has changed.  Deprecated.
        DBG_STR(ModifiedChange); //  = 102,                   // modified state has changed
        DBG_STR(MouseTrackingChange); //  = 109,              // mouse tracking state has changed

        DBG_STR(WindowBlocked); //  = 103,                    // window is about to be blocked modally
        DBG_STR(WindowUnblocked); //  = 104,                  // windows modal blocking has ended
        DBG_STR2(WindowStateChange, QWindowStateChangeEvent); //  = 105,

        DBG_STR(ReadOnlyChange); //  = 106,                   // readonly state has changed

        DBG_STR2(ToolTip, QHelpEvent); //  = 110,
        DBG_STR2(WhatsThis, QHelpEvent); //  = 111,
        DBG_STR2(StatusTip, QStatusTipEvent); //  = 112,

        DBG_STR2(ActionChanged, QActionEvent) //  = 113,
        DBG_STR2(ActionAdded, QActionEvent); //  = 114,
        DBG_STR2(ActionRemoved, QActionEvent); //  = 115,

        DBG_STR2(FileOpen, QFileOpenEvent); //  = 116,                         // file open request

        DBG_STR2(Shortcut, QShortcutEvent); //  = 117,                         // shortcut triggered
        DBG_STR2(ShortcutOverride, QKeyEvent); //  = 51,                  // shortcut override request

        DBG_STR2(WhatsThisClicked, QWhatsThisClickedEvent); //  = 118,

        DBG_STR(ToolBarChange); //  = 120,                    // toolbar visibility toggled

//         DBG_STR(DBG_STR(ApplicationActivate); //  = 121,              // deprecated. Use ApplicationStateChange instead.
//         DBG_STR(DBG_STR(ApplicationActivated = ApplicationActivate, // deprecated
//         DBG_STR(DBG_STR(ApplicationDeactivate = 122,            // deprecated. Use ApplicationStateChange instead.
//         DBG_STR(ApplicationDeactivated = ApplicationDeactivate, // deprecated

        DBG_STR2(QueryWhatsThis, QHelpEvent); //  = 123,                   // query what's this widget help
        DBG_STR(EnterWhatsThisMode); //  = 124,
        DBG_STR(LeaveWhatsThisMode); //  = 125,

        DBG_STR(ZOrderChange); //  = 126,                     // child widget has had its z-order changed

        DBG_STR2(HoverEnter, QHoverEvent); //  = 127,                       // mouse cursor enters a hover widget
        DBG_STR2(HoverLeave, QHoverEvent); //  = 128,                       // mouse cursor leaves a hover widget
        DBG_STR2(HoverMove, QHoverEvent); //  = 129,                        // mouse cursor move inside a hover widget

        // last event id used = 132



#ifdef QT_KEYPAD_NAVIGATION
        DBG_STR(EnterEditFocus); //  = 150,                   // enter edit mode in keypad navigation
        DBG_STR(LeaveEditFocus); //  = 151,                   // enter edit mode in keypad navigation
#endif
        DBG_STR(AcceptDropsChange); //  = 152,

        DBG_STR(ZeroTimerEvent); //  = 154,                   // Used for Windows Zero timer events

        DBG_STR2(GraphicsSceneMouseMove, QGraphicsSceneMouseEvent); //  = 155,           // GraphicsView
        DBG_STR2(GraphicsSceneMousePress, QGraphicsSceneMouseEvent); //  = 156,
        DBG_STR2(GraphicsSceneMouseRelease, QGraphicsSceneMouseEvent); //  = 157,
        DBG_STR2(GraphicsSceneMouseDoubleClick, QGraphicsSceneMouseEvent); //  = 158,
        DBG_STR2(GraphicsSceneContextMenu, QGraphicsSceneContextMenuEvent); //  = 159,
        DBG_STR2(GraphicsSceneHoverEnter, QGraphicsSceneHoverEvent); //  = 160,
        DBG_STR2(GraphicsSceneHoverMove, QGraphicsSceneHoverEvent); //  = 161,
        DBG_STR2(GraphicsSceneHoverLeave, QGraphicsSceneHoverEvent); //  = 162,
        DBG_STR2(GraphicsSceneHelp, QGraphicsSceneHelpEvent); //  = 163,
        DBG_STR2(GraphicsSceneDragEnter, QGraphicsSceneDragDropEvent); //  = 164,
        DBG_STR2(GraphicsSceneDragMove, QGraphicsSceneDragDropEvent); //  = 165,
        DBG_STR2(GraphicsSceneDragLeave, QGraphicsSceneDragDropEvent); //  = 166,
        DBG_STR2(GraphicsSceneDrop, QGraphicsSceneDragDropEvent); //  = 167,
        DBG_STR2(GraphicsSceneWheel, QGraphicsSceneWheelEvent); //  = 168,

        DBG_STR(KeyboardLayoutChange); //  = 169,             // keyboard layout changed

        DBG_STR2(DynamicPropertyChange, QDynamicPropertyChangeEvent); //  = 170,            // A dynamic property was changed through setProperty/property

        DBG_STR(TabletEnterProximity); //  = 171,
        DBG_STR(TabletLeaveProximity); //  = 172,

        DBG_STR2(NonClientAreaMouseMove, QMouseEvent); //  = 173,
        DBG_STR2(NonClientAreaMouseButtonPress, QMouseEvent); //  = 174,
        DBG_STR2(NonClientAreaMouseButtonRelease, QMouseEvent); //  = 175,
        DBG_STR2(NonClientAreaMouseButtonDblClick, QMouseEvent); //  = 176,

        DBG_STR(MacSizeChange); //  = 177,                    // when the Qt::WA_Mac{Normal,Small,Mini}Size changes

        DBG_STR(ContentsRectChange); //  = 178,               // sent by QWidget::setContentsMargins (internal)

        DBG_STR(MacGLWindowChange); //  = 179,                // Internal! the window of the GLWidget has changed

        DBG_STR(FutureCallOut); //  = 180,

        DBG_STR2(GraphicsSceneResize, QGraphicsSceneResizeEvent); //   = 181,
        DBG_STR2(GraphicsSceneMove, QGraphicsSceneMoveEvent); //   = 182,

        DBG_STR(CursorChange); //  = 183,
        DBG_STR(ToolTipChange); //  = 184,

        DBG_STR(NetworkReplyUpdated); //  = 185,              // Internal for QNetworkReply

        DBG_STR(GrabMouse); //  = 186,
        DBG_STR(UngrabMouse); //  = 187,
        DBG_STR(GrabKeyboard); //  = 188,
        DBG_STR(UngrabKeyboard); //  = 189,
//        DBG_STR(MacGLClearDrawable); //  = 191,               // Internal Cocoa, the window has changed, so we must clear

        DBG_STR(StateMachineSignal); //  = 192,
        DBG_STR(StateMachineWrapped); //  = 193,

        DBG_STR(TouchBegin); //  = 194,
        DBG_STR(TouchUpdate); //  = 195,
        DBG_STR(TouchEnd); //  = 196,

#ifndef QT_NO_GESTURES
        DBG_STR(NativeGesture); //  = 197,                    // QtGui native gesture
#endif
        DBG_STR(RequestSoftwareInputPanel); //  = 199,
        DBG_STR(CloseSoftwareInputPanel); //  = 200,

        DBG_STR(WinIdChange); //  = 203,
#ifndef QT_NO_GESTURES
        DBG_STR(Gesture); //  = 198,
        DBG_STR(GestureOverride); //  = 202,
#endif
        DBG_STR2(ScrollPrepare, QScrollPrepareEvent); //  = 204,
        DBG_STR2(Scroll, QScrollEvent); //  = 205,

        DBG_STR(Expose); //  = 206,

        DBG_STR(InputMethodQuery); //  = 207,
        DBG_STR(OrientationChange); //  = 208,                // Screen orientation has changed

        DBG_STR(TouchCancel); //  = 209,
        
        DBG_STR(ThemeChange); //  = 210,

        DBG_STR(SockClose); //  = 211,                        // socket closed

        DBG_STR(PlatformPanel); //  = 212,

        DBG_STR(StyleAnimationUpdate); //  = 213,             // style animation target should be updated
        DBG_STR(ApplicationStateChange); //  = 214,

        DBG_STR(WindowChangeInternal); //  = 215,             // internal for QQuickWidget
        DBG_STR(ScreenChangeInternal); //  = 216,

        DBG_STR2(PlatformSurface, QPlatformSurfaceEvent); //  = 217,                  // Platform surface created or about to be destroyed

        DBG_STR(Pointer); //  = 218,                          // QQuickPointerEvent; ### Qt 6: QPointerEvent

        DBG_STR2(GraphicsSceneLeave, QGraphicsSceneWheelEvent); // = 220

        // 512 reserved for Qt Jambi's MetaCall event
        // 513 reserved for Qt Jambi's DeleteOnMainThread event

        DBG_STR(User); //  = 1000,                            // first user event id
        DBG_STR(MaxUser); //  = 65535                         // last user event id
    }

#undef DBG_STR
#undef DBG_STR2

    s_str.Format(L"%d", e->type());
    return s_str;
}



#endif // TMFORMATQEVENT_H
