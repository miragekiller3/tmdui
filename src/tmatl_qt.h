/*
 *  @file
 *  @brief
 *  ATL for QT
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.08.03
 *
 *  Copyright (C) 2019 miragekiller
 */

#pragma once

#ifndef TMATL_QT_H
#define TMATL_QT_H

#ifndef TMATL_H
#   error the file must include from "tmatl.h"
#endif

#ifndef TM_QT
#   error "only for QT platform"
#endif

#include <QApplication>
#include <QPainter>
#include <QSemaphore>
#include <QMimeData>
#include <QThread>
#include <QBackingStore>
#include <QDesktopWidget>
#include <cmath>

#include "tmstring.h"
#include "tmcolor.h"
#include "tmmessagequeue.h"


#define QT_CLASS_CONSTRUCT(cls_name,obj_type,obj_name) \
    typedef cls_name class_type; \
    typedef T value_type; \
    typedef obj_type*  pointer; \
    cls_name (pointer p = 0) : obj_name(p) {} \
    operator pointer () const { return obj_name; } \
    bool operator ! () const { return !obj_name; } \
    void Swap(cls_name& rhs) { tm_swap(obj_name, rhs.obj_name); } \
    void Attach(pointer p) { obj_name = p; } \
    pointer Detach() { pointer p = obj_name; obj_name = 0; return p; } \
    cls_name& operator = (pointer* p) { obj_name = p; return this; }

#define QT_CLASS_CONSTRUCT2(cls_name,obj_type,obj_name) \
    using base_class::obj_name; \
    typedef T value_type; \
    typedef cls_name class_type; \
    typedef obj_type*  pointer; \
    cls_name (pointer p = 0) : base_class(p) {} \
    operator pointer () const { return (pointer)(obj_name); } \
    bool operator ! () const { return !obj_name; } \
    void Swap(cls_name& rhs) { tm_swap(obj_name, rhs.obj_name); } \
    void Attach(pointer p) { obj_name = p; } \
    pointer Detach() { return (pointer)base_class::Detach(); } \
    cls_name& operator = (pointer* p) { obj_name = p; return this; }



TM_BGN_NAMESPACE

template <class T>
class CQObjectT
{
public:
    union
    {
        T*      m_obj;
        TMHWnd  m_hWnd;
    };

    QT_CLASS_CONSTRUCT(CQObjectT, T, m_obj);

    TMResult SendMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgAssert(m_obj);
        return CMessageEvent::SendMessage(m_obj, message, wParam, lParam);
    }

    TMBool PostMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgAssert(m_obj);
        return CMessageEvent::PostMessage(m_obj, message, wParam, lParam);
    }

    TMBool SetProp(TMCStr lpString, TMHandle hData)
    {
        DbgAssert(m_obj);
        return m_obj->setProperty(TM_T2A(lpString), QVariant((qulonglong)hData));
    }
    
    TMHandle RemoveProp(TMCStr str)
    {
        DbgAssert(m_obj);
        TMHandle p = GetProp(TM_T2A(str));
        m_obj->setProperty(TM_T2A(str), QVariant());
        return p;
    }

    TMHandle GetProp(TMCStr lpString)
    {
        return (TMHandle)m_obj->property(TM_T2A(lpString)).toULongLong();
    }
};


///////////////////////////////////////////////////////////////////////////////
// CWindowQt

#define CWIDGET(type) template <class T> type CQWidgetT<T>

template <class T>
class CQWidgetT : public CQObjectT<T>
{
public:
    typedef CQObjectT<T> base_class;
    QT_CLASS_CONSTRUCT2(CQWidgetT, T, m_obj);

    static TMCStr GetWndClassName()
    {
        return NULL;
    }

//  TMHWnd Create(TMCStr lpstrWndClass, TMHWnd hWndParent, TMRect& rcPos, TMCStr szWindowName = NULL,
//          TMUInt32 dwStyle = 0, TMUInt32 dwExStyle = 0,
//          TMUInt nID = 0, void* lpCreateParam = NULL)
//  {
//      m_obj = ::CreateWindowEx(dwExStyle, lpstrWndClass, szWindowName,
//          dwStyle, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
//          rcPos.bottom - rcPos.top, hWndParent, (HMENU)nID,
//          _Module.GetModuleInstance(), lpCreateParam);
//      return m_obj;
//  }
// 
//  TMHWnd Create(TMCStr lpstrWndClass, TMHWnd hWndParent, LPRECT lpRect = NULL, TMCStr szWindowName = NULL,
//          TMUInt32 dwStyle = 0, TMUInt32 dwExStyle = 0,
//          HMENU hMenu = NULL, void* lpCreateParam = NULL)
//  {
//      if (lpRect == NULL)
//          lpRect = &rcDefault;
//      m_obj = ::CreateWindowEx(dwExStyle, lpstrWndClass, szWindowName,
//          dwStyle, lpRect->left, lpRect->top, lpRect->right - lpRect->left,
//          lpRect->bottom - lpRect->top, hWndParent, hMenu,
//          _Module.GetModuleInstance(), lpCreateParam);
//      return m_obj;
//  }

    TMBool DestroyWindow()
    {
        DbgAssert(IsWindow());
        m_obj->close();
        m_obj = NULL;
        return true;
    }

// Attributes
    TMUInt32 GetStyle() const
    {
        DbgAssert(IsWindow());
        TMUInt32 dw = 0;
        if (IsSelfVisible())
            dw = WS_VISIBLE;
        if (!IsWindowEnabled())
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
      TMUInt32 dw = 0;
      if (this->m_obj->testAttribute(Qt::WA_TranslucentBackground))
          dw = WS_EX_LAYERED;
      return dw;
  }

  void SetStyle(TMUInt32 dwStyle)
  {
      DbgAssert(IsWindow());
      Qt::WindowFlags flagsOld = this->m_hWnd->windowFlags();
      Qt::WindowFlags flags = flagsOld &
          (~(Qt::MSWindowsFixedSizeDialogHint | Qt::FramelessWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint));

      DbgAssert(!(dwStyle & WS_CHILD));
//       flags |= Qt::CustomizeWindowHint;

      if (dwStyle & WS_POPUP)
          flags |= Qt::CustomizeWindowHint;//FramelessWindowHint;
      else
          flags |= Qt::WindowTitleHint; // WS_OVERLAPPED
      if (dwStyle & WS_DLGFRAME)
          flags |= Qt::MSWindowsFixedSizeDialogHint;
//       if ((dwStyle & WS_CAPTION) != WS_CAPTION)
//           flags |= Qt::FramelessWindowHint;// (Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
      if (dwStyle & WS_MAXIMIZEBOX)
          flags |= Qt::WindowMaximizeButtonHint;
      if (dwStyle & WS_MINIMIZEBOX)
          flags |= Qt::WindowMinimizeButtonHint;
      if (dwStyle & WS_SYSMENU)
          flags |= Qt::WindowSystemMenuHint;
      if (flagsOld != flags)
        this->m_hWnd->setWindowFlags(flags);

#ifdef TM_WIN
      // fix bug: nonframe window no shadow
//       HWND hWnd = (HWND)m_hWnd->winId();
//       TMULong styleOld = ::GetWindowLong(hWnd, GWL_STYLE);
//       ::SetWindowLong(hWnd, GWL_STYLE, styleOld | WS_THICKFRAME);
#endif
  }

  void SetExStyle(TMUInt32 dwStyle)
  {
      Qt::WindowFlags flagsOld = this->m_hWnd->windowFlags();
      Qt::WindowFlags flags = flagsOld & (~(Qt::WindowType_Mask | Qt::WindowTransparentForInput | Qt::WindowContextHelpButtonHint));
      if ((dwStyle & (WS_EX_LAYERED | WS_EX_TRANSPARENT)) == (WS_EX_LAYERED | WS_EX_TRANSPARENT))
          flags |= Qt::WindowTransparentForInput;
       if (dwStyle & WS_EX_TOOLWINDOW)
           flags |= Qt::Tool;
       else
           flags |= Qt::Dialog;
      if (dwStyle & WS_EX_CONTEXTHELP)
          flags |= Qt::WindowContextHelpButtonHint;

      DbgAssert(IsWindow());
      bool bLayered = (0 != (dwStyle & WS_EX_LAYERED));
      if (bLayered != this->m_obj->testAttribute(Qt::WA_TranslucentBackground))
      {
          this->m_obj->setAttribute(Qt::WA_TranslucentBackground, bLayered);
          if (bLayered)
              flags = Qt::FramelessWindowHint; // layed must set Frameless
      }

      if (flags != flagsOld)
          this->m_hWnd->setWindowFlags(flags);
  }
 
//  LONG GetWindowLong(int nIndex) const
//  {
//      DbgAssert(IsWindow());
//      return ::GetWindowLong(m_obj, nIndex);
//  }
// 
//  LONG SetWindowLong(int nIndex, LONG dwNewLong)
//  {
//      DbgAssert(IsWindow());
//      return ::SetWindowLong(m_obj, nIndex, dwNewLong);
//  }
// 
//  WORD GetWindowWord(int nIndex) const
//  {
//      DbgAssert(IsWindow());
//      return ::GetWindowWord(m_obj, nIndex);
//  }
// 
//  WORD SetWindowWord(int nIndex, WORD wNewWord)
//  {
//      DbgAssert(IsWindow());
//      return ::SetWindowWord(m_obj, nIndex, wNewWord);
//  }

    // Message Functions

//  TMBool SendNotifyMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
//  {
//      DbgAssert(IsWindow());
//      return ::SendNotifyMessage(m_obj, message, wParam, lParam);
//  }
// 
//  // support for C style macros
//  static TMResult SendMessage(TMHWnd hWnd, TMUInt message, TMWParam wParam, TMLParam lParam)
//  {
//      DbgAssert(IsWindow());
//      return ::SendMessage(hWnd, message, wParam, lParam);
//  }

// Window Text Functions

    TMBool SetWindowText(TMCStr lpszString)
    {
        DbgAssert(IsWindow());
        m_obj->setWindowTitle(TM_T2Q(lpszString));
        return true;
    }

//  int GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const
//  {
//      DbgAssert(IsWindow());
//      return ::GetWindowText(m_obj, lpszStringBuf, nMaxCount);
//  }
// 
//  int GetWindowTextLength() const
//  {
//      DbgAssert(IsWindow());
//      return ::GetWindowTextLength(m_obj);
//  }
// 
// // Font Functions
// 
//  void SetFont(HFONT hFont, TMBool bRedraw = TRUE)
//  {
//      DbgAssert(IsWindow());
//      ::SendMessage(m_obj, WM_SETFONT, (TMWParam)hFont, MAKELPARAM(bRedraw, 0));
//  }
// 
//  HFONT GetFont() const
//  {
//      DbgAssert(IsWindow());
//      return (HFONT)::SendMessage(m_obj, WM_GETFONT, 0, 0);
//  }
// 
// // Menu Functions (non-child windows only)
// 
//  HMENU GetMenu() const
//  {
//      DbgAssert(IsWindow());
//      return ::GetMenu(m_obj);
//  }
// 
//  TMBool SetMenu(HMENU hMenu)
//  {
//      DbgAssert(IsWindow());
//      return ::SetMenu(m_obj, hMenu);
//  }
// 
//  TMBool DrawMenuBar()
//  {
//      DbgAssert(IsWindow());
//      return ::DrawMenuBar(m_obj);
//  }
// 
//  HMENU GetSystemMenu(TMBool bRevert) const
//  {
//      DbgAssert(IsWindow());
//      return ::GetSystemMenu(m_obj, bRevert);
//  }
// 
//  TMBool HiliteMenuItem(HMENU hMenu, TMUInt uItemHilite, TMUInt uHilite)
//  {
//      DbgAssert(IsWindow());
//      return ::HiliteMenuItem(m_obj, hMenu, uItemHilite, uHilite);
//  }

// Window Size and Position Functions

    TMBool IsIconic() const
    {
        DbgAssert(IsWindow());
        return m_obj->isMinimized();
    }

    TMBool IsZoomed() const
    {
        DbgAssert(IsWindow());
        return m_obj->isMaximized();
    }

    TMBool MoveWindow(int x, int y, int nWidth, int nHeight, TMBool bRepaint = TRUE)
    {
        return MoveWindow(tm_addressof(CRect(x, y, x + nWidth, y + nHeight)));
    }
    
    TMBool MoveWindow(const TMRect* lpRect, TMBool bRepaint = TRUE)
    {
        DbgAssert(IsWindow());
        QWindow* win = m_obj->windowHandle();
        QMargins margin = win->frameMargins();
        CRect rc(*lpRect);
        rc.InflateRect(-margin.left(), -margin.top(), -margin.right(), -margin.bottom());
        win->setGeometry(rc);
        return true;
    }

    void _SetWP(TMHWnd hWndInsertAfter, TMUInt nFlag)
    {
        if (nFlag & SWP_SHOWWINDOW)
            m_obj->show();
        else if (nFlag & SWP_HIDEWINDOW)
            m_obj->hide();
    }
    
    TMBool SetWindowPos(TMHWnd hWndInsertAfter, int x, int y, int cx, int cy, TMUInt nFlags)
    {
        DbgAssert(IsWindow());
        switch (nFlags & (SWP_NOSIZE | SWP_NOMOVE))
        {
        case 0: MoveWindow(x, y, cx, cy); break;
        case SWP_NOSIZE: m_obj->move(x, y); break;
        case SWP_NOMOVE: m_obj->resize(cx, cy); break;
        }
        _SetWP(hWndInsertAfter, nFlags);
        return true;
    }
    
    TMBool SetWindowPos(TMHWnd hWndInsertAfter, TMRect* lpRect, TMUInt nFlags)
    {
        DbgAssert(IsWindow());
        DbgAssert(lpRect);
        switch (nFlags & (SWP_NOSIZE | SWP_NOMOVE))
        {
        case 0: MoveWindow(lpRect); break;
        case SWP_NOSIZE: m_obj->move(RectLeft(*lpRect), RectTop(*lpRect)); break;
        case SWP_NOMOVE: m_obj->resize(RectWidth(*lpRect), RectHeight(*lpRect)); break;
        }
        _SetWP(hWndInsertAfter, nFlags);
        return true;
    }

    void ChangeFrame()
    {
        DbgAssert(IsWindow());
#ifdef TM_WIN
        ::SetWindowPos((HWND)this->m_hWnd->winId(), 0, 0, 0, 0, 0,
            SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER);
#else
        DbgAssert(0);
#endif
    }

//  TMUInt ArrangeIconicWindows()
//  {
//      DbgAssert(IsWindow());
//      return ::ArrangeIconicWindows(m_obj);
//  }
// 
//  TMBool BringWindowToTop()
//  {
//      DbgAssert(IsWindow());
//      return ::BringWindowToTop(m_obj);
//  }

    TMBool GetWindowRect(TMRect* lpRect) const
    {
        DbgAssert(IsWindow());
        *lpRect = m_obj->frameGeometry();
        QWindow* win = m_obj->windowHandle();
        *lpRect = win->frameGeometry();
        return true;
    }

    TMBool GetClientRect(TMRect* lpRect) const
    {
        DbgAssert(IsWindow());
        *lpRect = m_obj->rect();
        return true;
    }

//  TMBool GetWindowPlacement(WINDOWPLACEMENT FAR* lpwndpl) const
//  {
//      DbgAssert(IsWindow());
//      return ::GetWindowPlacement(m_obj, lpwndpl);
//  }
// 
//  TMBool SetWindowPlacement(const WINDOWPLACEMENT FAR* lpwndpl)
//  {
//      DbgAssert(IsWindow());
//      return ::SetWindowPlacement(m_obj, lpwndpl);
//  }

// Coordinate Mapping Functions

    TMBool ClientToScreen(TMPoint* lpPoint) const
    {
        DbgAssert(IsWindow());
        *lpPoint = m_obj->mapToGlobal(*lpPoint);
        return true;
    }

    TMBool ClientToScreen(TMRect* lpRect) const
    {
        DbgAssert(IsWindow());
        if (!ClientToScreen((TMPoint*)lpRect))
            return FALSE;
        return ClientToScreen(((TMPoint*)lpRect)+1);
    }

    TMBool ScreenToClient(TMPoint* lpPoint) const
    {
        DbgAssert(IsWindow());
        *lpPoint = m_obj->mapFromGlobal(*lpPoint);
    }

    TMBool ScreenToClient(TMRect* lpRect) const
    {
        DbgAssert(IsWindow());
        if (!ScreenToClient((TMPoint*)lpRect))
            return FALSE;
        return ScreenToClient(((TMPoint*)lpRect)+1);
    }

//  int MapWindowPoints(TMHWnd hWndTo, LPPOINT lpPoint, TMUInt nCount) const
//  {
//      DbgAssert(IsWindow());
//      return ::MapWindowPoints(m_obj, hWndTo, lpPoint, nCount);
//  }
// 
//  int MapWindowPoints(TMHWnd hWndTo, LPRECT lpRect) const
//  {
//      DbgAssert(IsWindow());
//      return ::MapWindowPoints(m_obj, hWndTo, (LPPOINT)lpRect, 2);
//  }
// 
// // Update and Painting Functions
// 
//  TMHDC BeginPaint(LPPAINTSTRUCT lpPaint)
//  {
//      DbgAssert(IsWindow());
//      return ::BeginPaint(m_obj, lpPaint);
//  }
// 
//  void EndPaint(LPPAINTSTRUCT lpPaint)
//  {
//      DbgAssert(IsWindow());
//      ::EndPaint(m_obj, lpPaint);
//  }
// 
//  TMHDC GetDC()
//  {
//      DbgAssert(IsWindow());
//      return ::GetDC(m_obj);
//  }
// 
//  TMHDC GetWindowDC()
//  {
//      DbgAssert(IsWindow());
//      return ::GetWindowDC(m_obj);
//  }
// 
//  int ReleaseDC(TMHDC hDC)
//  {
//      DbgAssert(IsWindow());
//      return ::ReleaseDC(m_obj, hDC);
//  }
// 
//  void Print(TMHDC hDC, TMUInt32 dwFlags) const
//  {
//      DbgAssert(IsWindow());
//      ::SendMessage(m_obj, WM_PRINT, (TMWParam)hDC, dwFlags);
//  }
// 
//  void PrintClient(TMHDC hDC, TMUInt32 dwFlags) const
//  {
//      DbgAssert(IsWindow());
//      ::SendMessage(m_obj, WM_PRINTCLIENT, (TMWParam)hDC, dwFlags);
//  }

    TMBool UpdateWindow()
    {
        DbgAssert(IsWindow());
        m_obj->repaint(m_obj->contentsRect());
        return true;
    }

//  void SetRedraw(TMBool bRedraw = TRUE)
//  {
//      DbgAssert(IsWindow());
//      ::SendMessage(m_obj, WM_SETREDRAW, (TMWParam)bRedraw, 0);
//  }
// 
//  TMBool GetUpdateRect(LPRECT lpRect, TMBool bErase = FALSE)
//  {
//      DbgAssert(IsWindow());
//      return ::GetUpdateRect(m_obj, lpRect, bErase);
//  }
// 
//  int GetUpdateRgn(HRGN hRgn, TMBool bErase = FALSE)
//  {
//      DbgAssert(IsWindow());
//      return ::GetUpdateRgn(m_obj, hRgn, bErase);
//  }

    TMBool Invalidate(TMBool bErase = TRUE)
    {
        DbgAssert(IsWindow());
        m_obj->update(m_obj->contentsRect());
        return true;
    }
    
    TMBool InvalidateRect(const TMRect* lpRect, TMBool bErase = TRUE)
    {
        DbgAssert(IsWindow());
        if (!lpRect)
            return Invalidate(bErase);
        m_obj->update(*lpRect);
        return true;
    }

//     TMBool ValidateRect(LPCRECT lpRect)
//     {
//         DbgAssert(IsWindow());
//         return ::ValidateRect(m_obj, lpRect);
//     }
// 
//  void InvalidateRgn(HRGN hRgn, TMBool bErase = TRUE)
//  {
//      DbgAssert(IsWindow());
//      ::InvalidateRgn(m_obj, hRgn, bErase);
//  }
// 
//  TMBool ValidateRgn(HRGN hRgn)
//  {
//      DbgAssert(IsWindow());
//      return ::ValidateRgn(m_obj, hRgn);
//  }

    TMBool ShowWindow(int nCmdShow)
    {
        DbgAssert(IsWindow());
        switch (nCmdShow)
        {
        case SW_HIDE: m_obj->hide(); return true;
        case SW_SHOWMINIMIZED: ShowMinimized(); return true;
        case SW_SHOWMAXIMIZED: ShowMaximized(); return true;
        case SW_SHOWNORMAL:
        case SW_RESTORE: ShowNormal(); return true;
        default: m_obj->show(); return true;
        }
    }

    // test parent and parent's parent visible state.
    TMBool IsWindowVisible() const
    {
        DbgAssert(IsWindow());
        return m_obj->isVisible();
    }

//  TMBool ShowOwnedPopups(TMBool bShow = TRUE)
//  {
//      DbgAssert(IsWindow());
//      return ::ShowOwnedPopups(m_obj, bShow);
//  }
// 
//  TMHDC GetDCEx(HRGN hRgnClip, TMUInt32 flags)
//  {
//      DbgAssert(IsWindow());
//      return ::GetDCEx(m_obj, hRgnClip, flags);
//  }
// 
//  TMBool LockWindowUpdate(TMBool bLock = TRUE)
//  {
//      DbgAssert(IsWindow());
//      return ::LockWindowUpdate(bLock ? m_obj : NULL);
//  }
// 
//  TMBool RedrawWindow(LPCRECT lpRectUpdate = NULL, HRGN hRgnUpdate = NULL, TMUInt flags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE)
//  {
//      DbgAssert(IsWindow());
//      return ::RedrawWindow(m_obj, lpRectUpdate, hRgnUpdate, flags);
//  }
// 
// // Timer Functions
// 
//  TMUInt SetTimer(TMUInt nIDEvent, TMUInt nElapse)
//  {
//      DbgAssert(IsWindow());
//      return ::SetTimer(m_obj, nIDEvent, nElapse, NULL);
//  }
// 
//  TMBool KillTimer(TMUInt nIDEvent)
//  {
//      DbgAssert(IsWindow());
//      return ::KillTimer(m_obj, nIDEvent);
//  }

// Window State Functions

    TMBool IsWindowEnabled() const
    {
        DbgAssert(IsWindow());
        return m_obj->isEnabled();
    }

//  TMBool EnableWindow(TMBool bEnable = TRUE)
//  {
//      DbgAssert(IsWindow());
//      return ::EnableWindow(m_obj, bEnable);
//  }
// 
//  TMHWnd SetActiveWindow()
//  {
//      DbgAssert(IsWindow());
//      return ::SetActiveWindow(m_obj);
//  }

    void SetCapture()
    {
        DbgAssert(IsWindow());
        m_obj->grabMouse();
//        m_obj->windowHandle()->setMouseGrabEnabled(true);
    }
    
    void SetFocus()
    {
        DbgAssert(IsWindow());
        m_obj->setFocus();
    }

    TMBool IsFocus()
    {
        return QGuiApplication::focusWindow()->winId() == m_obj->winId();
    }


// Dialog-Box Item Functions

//  TMBool CheckDlgButton(int nIDButton, TMUInt nCheck)
//  {
//      DbgAssert(IsWindow());
//      return ::CheckDlgButton(m_obj, nIDButton, nCheck);
//  }
// 
//  TMBool CheckRadioButton(int nIDFirstButton, int nIDLastButton, int nIDCheckButton)
//  {
//      DbgAssert(IsWindow());
//      return ::CheckRadioButton(m_obj, nIDFirstButton, nIDLastButton, nIDCheckButton);
//  }
// 
//  int DlgDirList(LPTSTR lpPathSpec, int nIDListBox, int nIDStaticPath, TMUInt nFileType)
//  {
//      DbgAssert(IsWindow());
//      return ::DlgDirList(m_obj, lpPathSpec, nIDListBox, nIDStaticPath, nFileType);
//  }
// 
//  int DlgDirListComboBox(LPTSTR lpPathSpec, int nIDComboBox, int nIDStaticPath, TMUInt nFileType)
//  {
//      DbgAssert(IsWindow());
//      return ::DlgDirListComboBox(m_obj, lpPathSpec, nIDComboBox, nIDStaticPath, nFileType);
//  }
// 
//  TMBool DlgDirSelect(LPTSTR lpString, int nCount, int nIDListBox)
//  {
//      DbgAssert(IsWindow());
//      return ::DlgDirSelectEx(m_obj, lpString, nCount, nIDListBox);
//  }
// 
//  TMBool DlgDirSelectComboBox(LPTSTR lpString, int nCount, int nIDComboBox)
//  {
//      DbgAssert(IsWindow());
//      return ::DlgDirSelectComboBoxEx(m_obj, lpString, nCount, nIDComboBox);
//  }
// 
//  TMUInt GetDlgItemInt(int nID, TMBool* lpTrans = NULL, TMBool bSigned = TRUE) const
//  {
//      DbgAssert(IsWindow());
//      return ::GetDlgItemInt(m_obj, nID, lpTrans, bSigned);
//  }
// 
//  TMUInt GetDlgItemText(int nID, LPTSTR lpStr, int nMaxCount) const
//  {
//      DbgAssert(IsWindow());
//      return ::GetDlgItemText(m_obj, nID, lpStr, nMaxCount);
//  }
//  TMBool GetDlgItemText(int nID, BSTR& bstrText) const
//  {
//      DbgAssert(IsWindow());
// 
//      TMHWnd hWndCtl = GetDlgItem(nID);
//      if (hWndCtl == NULL)
//          return FALSE;
// 
//      return CWindowQt(hWndCtl).GetWindowText(bstrText);
//  }
//  TMHWnd GetNextDlgGroupItem(TMHWnd hWndCtl, TMBool bPrevious = FALSE) const
//  {
//      DbgAssert(IsWindow());
//      return ::GetNextDlgGroupItem(m_obj, hWndCtl, bPrevious);
//  }
// 
//  TMHWnd GetNextDlgTabItem(TMHWnd hWndCtl, TMBool bPrevious = FALSE) const
//  {
//      DbgAssert(IsWindow());
//      return ::GetNextDlgTabItem(m_obj, hWndCtl, bPrevious);
//  }
// 
//  TMUInt IsDlgButtonChecked(int nIDButton) const
//  {
//      DbgAssert(IsWindow());
//      return ::IsDlgButtonChecked(m_obj, nIDButton);
//  }
// 
//  TMResult SendDlgItemMessage(int nID, TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
//  {
//      DbgAssert(IsWindow());
//      return ::SendDlgItemMessage(m_obj, nID, message, wParam, lParam);
//  }
// 
//  TMBool SetDlgItemInt(int nID, TMUInt nValue, TMBool bSigned = TRUE)
//  {
//      DbgAssert(IsWindow());
//      return ::SetDlgItemInt(m_obj, nID, nValue, bSigned);
//  }
// 
//  TMBool SetDlgItemText(int nID, TMCStr lpszString)
//  {
//      DbgAssert(IsWindow());
//      return ::SetDlgItemText(m_obj, nID, lpszString);
//  }
// 
// #ifndef _ATL_NO_HOSTING
//  TMResult GetDlgControl(int nID, REFIID iid, void** ppUnk)
//  {
//      DbgAssert(IsWindow());
//      DbgAssert(ppUnk != NULL);
//      TMResult hr = E_FAIL;
//      TMHWnd hWndCtrl = GetDlgItem(nID);
//      if (hWndCtrl != NULL)
//      {
//          *ppUnk = NULL;
//          CComPtr<IUnknown> spUnk;
//          hr = AtlAxGetControl(hWndCtrl, &spUnk);
//          if (SUCCEEDED(hr))
//              hr = spUnk->QueryInterface(iid, ppUnk);
//      }
//      return hr;
//  }
// #endif //!_ATL_NO_HOSTING
// 
// // Scrolling Functions
// 
//  int GetScrollPos(int nBar) const
//  {
//      DbgAssert(IsWindow());
//      return ::GetScrollPos(m_obj, nBar);
//  }
// 
//  TMBool GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos) const
//  {
//      DbgAssert(IsWindow());
//      return ::GetScrollRange(m_obj, nBar, lpMinPos, lpMaxPos);
//  }
// 
//  TMBool ScrollWindow(int xAmount, int yAmount, LPCRECT lpRect = NULL, LPCRECT lpClipRect = NULL)
//  {
//      DbgAssert(IsWindow());
//      return ::ScrollWindow(m_obj, xAmount, yAmount, lpRect, lpClipRect);
//  }
// 
//  int ScrollWindowEx(int dx, int dy, LPCRECT lpRectScroll, LPCRECT lpRectClip, HRGN hRgnUpdate, LPRECT lpRectUpdate, TMUInt uFlags)
//  {
//      DbgAssert(IsWindow());
//      return ::ScrollWindowEx(m_obj, dx, dy, lpRectScroll, lpRectClip, hRgnUpdate, lpRectUpdate, uFlags);
//  }
// 
//  int ScrollWindowEx(int dx, int dy, TMUInt uFlags, LPCRECT lpRectScroll = NULL, LPCRECT lpRectClip = NULL, HRGN hRgnUpdate = NULL, LPRECT lpRectUpdate = NULL)
//  {
//      DbgAssert(IsWindow());
//      return ::ScrollWindowEx(m_obj, dx, dy, lpRectScroll, lpRectClip, hRgnUpdate, lpRectUpdate, uFlags);
//  }
// 
//  int SetScrollPos(int nBar, int nPos, TMBool bRedraw = TRUE)
//  {
//      DbgAssert(IsWindow());
//      return ::SetScrollPos(m_obj, nBar, nPos, bRedraw);
//  }
// 
//  TMBool SetScrollRange(int nBar, int nMinPos, int nMaxPos, TMBool bRedraw = TRUE)
//  {
//      DbgAssert(IsWindow());
//      return ::SetScrollRange(m_obj, nBar, nMinPos, nMaxPos, bRedraw);
//  }
// 
//  TMBool ShowScrollBar(TMUInt nBar, TMBool bShow = TRUE)
//  {
//      DbgAssert(IsWindow());
//      return ::ShowScrollBar(m_obj, nBar, bShow);
//  }
// 
//  TMBool EnableScrollBar(TMUInt uSBFlags, TMUInt uArrowFlags = ESB_ENABLE_BOTH)
//  {
//      DbgAssert(IsWindow());
//      return ::EnableScrollBar(m_obj, uSBFlags, uArrowFlags);
//  }
// 
// // Window Access Functions
// 
//  TMHWnd ChildWindowFromPoint(POINT point) const
//  {
//      DbgAssert(IsWindow());
//      return ::ChildWindowFromPoint(m_obj, point);
//  }
// 
//  TMHWnd ChildWindowFromPointEx(POINT point, TMUInt uFlags) const
//  {
//      DbgAssert(IsWindow());
//      return ::ChildWindowFromPointEx(m_obj, point, uFlags);
//  }
// 
//  TMHWnd GetTopWindow() const
//  {
//      DbgAssert(IsWindow());
//      return ::GetTopWindow(m_obj);
//  }

    TMHWnd GetWindow(TMUInt nCmd) const
    {
        DbgAssert(this->m_hWnd);
        switch (nCmd)
        {
        case GW_CHILD: return 0;
        case GW_OWNER: return static_cast<TMHWnd>(this->m_hWnd->topLevelWidget());
        }
        return 0;
    }

//  TMHWnd GetLastActivePopup() const
//  {
//      DbgAssert(IsWindow());
//      return ::GetLastActivePopup(m_obj);
//  }
 
    TMBool IsChild(TMHWnd hWnd) const
    {
        DbgAssert(IsWindow());
        for (QWidget* pc = hWnd; pc; pc = pc->nativeParentWidget())
        {
            if (pc == this->m_hWnd)
                return true;
        }
        return false; // ::IsChild(m_obj, hWnd);
    }

    TMHWnd GetParent() const
    {
        DbgAssert(IsWindow());
        return static_cast<TMHWnd>(m_obj->nativeParentWidget());
    }

    void SetParent(TMHWnd hWndNewParent)
    {
        DbgAssert(IsWindow());
        m_obj->setParent(hWndNewParent);
    }

// Window Tree Access

//  int GetDlgCtrlID() const
//  {
//      DbgAssert(IsWindow());
//      return ::GetDlgCtrlID(m_obj);
//  }
// 
//  int SetDlgCtrlID(int nID)
//  {
//      DbgAssert(IsWindow());
//      return (int)::SetWindowLong(m_obj, GWL_ID, nID);
//  }
// 
//  TMHWnd GetDlgItem(int nID) const
//  {
//      DbgAssert(IsWindow());
//      return ::GetDlgItem(m_obj, nID);
//  }
// 
// // Alert Functions
// 
//  TMBool FlashWindow(TMBool bInvert)
//  {
//      DbgAssert(IsWindow());
//      return ::FlashWindow(m_obj, bInvert);
//  }
// 
//    int MessageBox(TMCStr lpszText, TMCStr lpszCaption = TM_T(""), TMUInt nType = MB_OK)
//    {
//        DbgAssert(IsWindow());
//        QMessageBox::StandardButton rb = QMessageBox::information(NULL, lpszCaption, lpszText, QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
//        return rb;
//    }

// // Clipboard Functions
// 
//  TMBool ChangeClipboardChain(TMHWnd hWndNewNext)
//  {
//      DbgAssert(IsWindow());
//      return ::ChangeClipboardChain(m_obj, hWndNewNext);
//  }
// 
//  TMHWnd SetClipboardViewer()
//  {
//      DbgAssert(IsWindow());
//      return ::SetClipboardViewer(m_obj);
//  }
// 
//  TMBool OpenClipboard()
//  {
//      DbgAssert(IsWindow());
//      return ::OpenClipboard(m_obj);
//  }
// 
// // Caret Functions
 
  TMBool CreateCaret(HBITMAP hBitmap)
  {
# ifdef TM_WIN
      DbgAssert(IsWindow());
      return ::CreateCaret((HWND)this->m_hWnd->winId(), hBitmap, 0, 0);
#else
      return true;
#endif
  }

  BOOL CreateUnunsedCaret(int nWidth, int nHeight) throw()
  {
# ifdef TM_WIN
      DbgAssert(IsWindow());
      return ::CreateCaret((HWND)this->m_hWnd->winId(), (HBITMAP)2, nWidth, nHeight);
#else
      return true;
#endif
  }
 
//  TMBool CreateSolidCaret(int nWidth, int nHeight)
//  {
//      DbgAssert(IsWindow());
//      return ::CreateCaret(m_obj, (HBITMAP)0, nWidth, nHeight);
//  }
// 
//  TMBool CreateGrayCaret(int nWidth, int nHeight)
//  {
//      DbgAssert(IsWindow());
//      return ::CreateCaret(m_obj, (HBITMAP)1, nWidth, nHeight);
//  }
// 
//  TMBool HideCaret()
//  {
//      DbgAssert(IsWindow());
//      return ::HideCaret(m_obj);
//  }
 
  TMBool ShowCaret()
  {
#ifdef TM_WIN
      DbgAssert(IsWindow());
      return ::ShowCaret((HWND)m_obj->winId());
#else
      return true;
#endif
  }
 
// #ifdef _INC_SHELLAPI
// // Drag-Drop Functions
//  void DragAcceptFiles(TMBool bAccept = TRUE)
//  {
//      DbgAssert(IsWindow()); ::DragAcceptFiles(m_obj, bAccept);
//  }
// #endif

// Icon Functions

//  TMHIcon SetIcon(TMHIcon hIcon, TMBool bBigIcon = TRUE)
//  {
//      DbgAssert(IsWindow());
//      return (TMHIcon)::SendMessage(m_obj, WM_SETICON, bBigIcon, (TMLParam)hIcon);
//  }
 
  TMHIcon GetIcon(TMBool bBigIcon = TRUE) const
  {
      DbgAssert(IsWindow());
      return m_obj->windowIcon();
  }
 
// // Help Functions
// 
//  TMBool WinHelp(TMCStr lpszHelp, TMUInt nCmd = HELP_CONTEXT, TMUInt32 dwData = 0)
//  {
//      DbgAssert(IsWindow());
//      return ::WinHelp(m_obj, lpszHelp, nCmd, dwData);
//  }
// 
//  TMBool SetWindowContextHelpId(TMUInt32 dwContextHelpId)
//  {
//      DbgAssert(IsWindow());
//      return ::SetWindowContextHelpId(m_obj, dwContextHelpId);
//  }
// 
//  TMUInt32 GetWindowContextHelpId() const
//  {
//      DbgAssert(IsWindow());
//      return ::GetWindowContextHelpId(m_obj);
//  }
// 
// // Hot Key Functions
// 
//  int SetHotKey(WORD wVirtualKeyCode, WORD wModifiers)
//  {
//      DbgAssert(IsWindow());
//      return (int)::SendMessage(m_obj, WM_SETHOTKEY, MAKEWORD(wVirtualKeyCode, wModifiers), 0);
//  }
// 
//  TMUInt32 GetHotKey() const
//  {
//      DbgAssert(IsWindow());
//      return ::SendMessage(m_obj, WM_GETHOTKEY, 0, 0);
//  }
// 
// // Misc. Operations
// 
// //N new
//  TMBool GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo)
//  {
//      DbgAssert(IsWindow());
//      return ::GetScrollInfo(m_obj, nBar, lpScrollInfo);
//  }
//  TMBool SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, TMBool bRedraw = TRUE)
//  {
//      DbgAssert(IsWindow());
//      return ::SetScrollInfo(m_obj, nBar, lpScrollInfo, bRedraw);
//  }
//  TMBool IsDialogMessage(LPMSG lpMsg)
//  {
//      DbgAssert(IsWindow());
//      return ::IsDialogMessage(m_obj, lpMsg);
//  }
// 
//  void NextDlgCtrl() const
//         {
//      DbgAssert(IsWindow());
//      ::SendMessage(m_obj, WM_NEXTDLGCTL, 0, 0L);
//  }
//  void PrevDlgCtrl() const
//         {
//      DbgAssert(IsWindow());
//      ::SendMessage(m_obj, WM_NEXTDLGCTL, 1, 0L);
//  }
//  void GotoDlgCtrl(TMHWnd hWndCtrl) const
//         {
//      DbgAssert(IsWindow());
//      ::SendMessage(m_obj, WM_NEXTDLGCTL, (TMWParam)hWndCtrl, 1L);
//  }
// 
//  TMBool ResizeClient(int nWidth, int nHeight, TMBool bRedraw = TRUE)
//  {
//      DbgAssert(IsWindow());
// 
//      TMRect rcWnd;
//      if (!GetClientRect(&rcWnd))
//          return FALSE;
// 
//      if (nWidth != -1)
//          rcWnd.right = nWidth;
//      if (nHeight != -1)
//          rcWnd.bottom = nHeight;
// 
//      if (!::AdjustWindowRectEx(&rcWnd, GetStyle(), (!(GetStyle() & WS_CHILD) && (GetMenu() != NULL)), GetExStyle()))
//          return FALSE;
// 
//      TMUInt uFlags = SWP_NOZORDER | SWP_NOMOVE;
//      if (!bRedraw)
//          uFlags |= SWP_NOREDRAW;
// 
//      return SetWindowPos(NULL, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, uFlags);
//  }
// 
//  int GetWindowRgn(HRGN hRgn)
//  {
//      DbgAssert(IsWindow());
//      return ::GetWindowRgn(m_obj, hRgn);
//  }
//  int SetWindowRgn(HRGN hRgn, TMBool bRedraw = FALSE)
//  {
//      DbgAssert(IsWindow());
//      return ::SetWindowRgn(m_obj, hRgn, bRedraw);
//  }
//  HDWP DeferWindowPos(HDWP hWinPosInfo, TMHWnd hWndInsertAfter, int x, int y, int cx, int cy, TMUInt uFlags)
//  {
//      DbgAssert(IsWindow());
//      return ::DeferWindowPos(hWinPosInfo, m_obj, hWndInsertAfter, x, y, cx, cy, uFlags);
//  }
//  TMUInt32 GetWindowThreadID()
//  {
//      DbgAssert(IsWindow());
//      return ::GetWindowThreadProcessId(m_obj, NULL);
//  }
//  TMUInt32 GetWindowProcessID()
//  {
//      DbgAssert(IsWindow());
//      TMUInt32 dwProcessID;
//      ::GetWindowThreadProcessId(m_obj, &dwProcessID);
//      return dwProcessID;
//  }
    
    TMBool IsWindow() const
    {
        return 0 != m_obj;
    }

//  TMBool IsWindowUnicode()
//  {
//      DbgAssert(IsWindow());
//      return ::IsWindowUnicode(m_obj);
//  }
//  TMBool IsParentDialog()
//  {
//      DbgAssert(IsWindow());
//      TCHAR szBuf[8]; // "#32770" + NUL character
//      GetClassName(GetParent(), szBuf, sizeof(szBuf)/sizeof(TCHAR));
//      return lstrcmp(szBuf, _T("#32770")) == 0;
//  }
//  TMBool ShowWindowAsync(int nCmdShow)
//  {
//      DbgAssert(IsWindow());
//      return ::ShowWindowAsync(m_obj, nCmdShow);
//  }
// 
//  TMHWnd GetDescendantWindow(int nID) const
//  {
//      DbgAssert(IsWindow());
// 
//      // GetDlgItem recursive (return first found)
//      // breadth-first for 1 level, then depth-first for next level
// 
//      // use GetDlgItem since it is a fast USER function
//      TMHWnd hWndChild, hWndTmp;
//      CWindowQt wnd;
//      if ((hWndChild = ::GetDlgItem(m_obj, nID)) != NULL)
//      {
//          if (::GetTopWindow(hWndChild) != NULL)
//          {
//              // children with the same ID as their parent have priority
//              wnd.Attach(hWndChild);
//              hWndTmp = wnd.GetDescendantWindow(nID);
//              if (hWndTmp != NULL)
//                  return hWndTmp;
//          }
//          return hWndChild;
//      }
// 
//      // walk each child
//      for (hWndChild = ::GetTopWindow(m_obj); hWndChild != NULL;
//          hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
//      {
//          wnd.Attach(hWndChild);
//          hWndTmp = wnd.GetDescendantWindow(nID);
//          if (hWndTmp != NULL)
//              return hWndTmp;
//      }
// 
//      return NULL;    // not found
//  }
// 
//  void SendMessageToDescendants(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0, TMBool bDeep = TRUE)
//  {
//      CWindowQt wnd;
//      for (TMHWnd hWndChild = ::GetTopWindow(m_obj); hWndChild != NULL;
//          hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
//      {
//          ::SendMessage(hWndChild, message, wParam, lParam);
// 
//          if (bDeep && ::GetTopWindow(hWndChild) != NULL)
//          {
//              // send to child windows after parent
//              wnd.Attach(hWndChild);
//              wnd.SendMessageToDescendants(message, wParam, lParam, bDeep);
//          }
//      }
//  }
// 
//  TMBool CenterWindow(TMHWnd hWndCenter = NULL)
//  {
//      DbgAssert(IsWindow());
// 
//      // determine owner window to center against
//      TMUInt32 dwStyle = GetStyle();
//      if (hWndCenter == NULL)
//      {
//          if (dwStyle & WS_CHILD)
//              hWndCenter = ::GetParent(m_obj);
//          else
//              hWndCenter = ::GetWindow(m_obj, GW_OWNER);
//      }
// 
//      // get coordinates of the window relative to its parent
//      TMRect rcDlg;
//      ::GetWindowRect(m_obj, &rcDlg);
//      TMRect rcArea;
//      TMRect rcCenter;
//      TMHWnd hWndParent;
//      if (!(dwStyle & WS_CHILD))
//      {
//          // don't center against invisible or minimized windows
//          if (hWndCenter != NULL)
//          {
//              TMUInt32 dwStyle = ::GetWindowLong(hWndCenter, GWL_STYLE);
//              if (!(dwStyle & WS_VISIBLE) || (dwStyle & WS_MINIMIZE))
//                  hWndCenter = NULL;
//          }
// 
//          // center within screen coordinates
//          ::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
//          if (hWndCenter == NULL)
//              rcCenter = rcArea;
//          else
//              ::GetWindowRect(hWndCenter, &rcCenter);
//      }
//      else
//      {
//          // center within parent client coordinates
//          hWndParent = ::GetParent(m_obj);
//          DbgAssert(::IsWindow(hWndParent));
// 
//          ::GetClientRect(hWndParent, &rcArea);
//          DbgAssert(::IsWindow(hWndCenter));
//          ::GetClientRect(hWndCenter, &rcCenter);
//          ::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
//      }
// 
//      int DlgWidth = rcDlg.right - rcDlg.left;
//      int DlgHeight = rcDlg.bottom - rcDlg.top;
// 
//      // find dialog's upper left based on rcCenter
//      int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
//      int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;
// 
//      // if the dialog is outside the screen, move it inside
//      if (xLeft < rcArea.left)
//          xLeft = rcArea.left;
//      else if (xLeft + DlgWidth > rcArea.right)
//          xLeft = rcArea.right - DlgWidth;
// 
//      if (yTop < rcArea.top)
//          yTop = rcArea.top;
//      else if (yTop + DlgHeight > rcArea.bottom)
//          yTop = rcArea.bottom - DlgHeight;
// 
//      // map screen coordinates to child coordinates
//      return ::SetWindowPos(m_obj, NULL, xLeft, yTop, -1, -1,
//          SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
//  }

    void CenterWindow2(TMBool bCenterScreen, TMBool bCenterOwner)
    {
        _Monitor_CenterWindow(((CMonitor*)0), *this, bCenterScreen, bCenterOwner);
    }

// 
//  TMBool ModifyStyle(TMUInt32 dwRemove, TMUInt32 dwAdd, TMUInt nFlags = 0)
//  {
//      DbgAssert(IsWindow());
// 
//      TMUInt32 dwStyle = ::GetWindowLong(m_obj, GWL_STYLE);
//      TMUInt32 dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
//      if (dwStyle == dwNewStyle)
//          return FALSE;
// 
//      ::SetWindowLong(m_obj, GWL_STYLE, dwNewStyle);
//      if (nFlags != 0)
//      {
//          ::SetWindowPos(m_obj, NULL, 0, 0, 0, 0,
//              SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
//      }
// 
//      return TRUE;
//  }
// 
//  TMBool ModifyStyleEx(TMUInt32 dwRemove, TMUInt32 dwAdd, TMUInt nFlags = 0)
//  {
//      DbgAssert(IsWindow());
// 
//      TMUInt32 dwStyle = ::GetWindowLong(m_obj, GWL_EXSTYLE);
//      TMUInt32 dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
//      if (dwStyle == dwNewStyle)
//          return FALSE;
// 
//      ::SetWindowLong(m_obj, GWL_EXSTYLE, dwNewStyle);
//      if (nFlags != 0)
//      {
//          ::SetWindowPos(m_obj, NULL, 0, 0, 0, 0,
//              SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
//      }
// 
//      return TRUE;
//  }
// 
//  TMBool GetWindowText(BSTR* pbstrText)
//  {
//      return GetWindowText(*pbstrText);
//  }
//  TMBool GetWindowText(BSTR& bstrText)
//  {
//      USES_CONVERSION;
//      DbgAssert(IsWindow());
//      if (bstrText != NULL)
//      {
//          SysFreeString(bstrText);
//          bstrText = NULL;
//      }
// 
//      int nLen = ::GetWindowTextLength(m_obj);
//      if (nLen == 0)
//      {
//          bstrText = ::SysAllocString(OLESTR(""));
//          return (bstrText != NULL) ? TRUE : FALSE;
//      }
// 
//      LPTSTR lpszText = (LPTSTR)_alloca((nLen+1)*sizeof(TCHAR));
// 
//      if (!::GetWindowText(m_obj, lpszText, nLen+1))
//          return FALSE;
// 
//      bstrText = ::SysAllocString(T2OLE(lpszText));
//      return (bstrText != NULL) ? TRUE : FALSE;
//  }
//  TMHWnd GetTopLevelParent() const
//  {
//      DbgAssert(IsWindow());
// 
//      TMHWnd hWndParent = m_obj;
//      TMHWnd hWndTmp;
//      while((hWndTmp = ::GetParent(hWndParent)) != NULL)
//          hWndParent = hWndTmp;
// 
//      return hWndParent;
//  }
// 
//  TMHWnd GetTopLevelWindow() const
//  {
//      DbgAssert(IsWindow());
// 
//      TMHWnd hWndParent;
//      TMHWnd hWndTmp = m_obj;
// 
//      do
//      {
//          hWndParent = hWndTmp;
//          hWndTmp = (::GetWindowLong(hWndParent, GWL_STYLE) & WS_CHILD) ? ::GetParent(hWndParent) : ::GetWindow(hWndParent, GW_OWNER);
//      }
//      while(hWndTmp != NULL);
// 
//      return hWndParent;
//  }


    // ext funciton
    void Move(int x, int y)
    {
        DbgAssert(IsWindow());
        m_obj->move(x, y);
    }

    void Resize(int w, int h)
    {
        DbgAssert(IsWindow());
        m_obj->resize(w, h);
    }

    void ShowMinimized()
    {
        DbgAssert(IsWindow());
        m_obj->showMinimized();
    }

    void ShowMaximized()
    {
        DbgAssert(IsWindow());
        m_obj->showMaximized();
    }

    void ShowNormal()
    {
        DbgAssert(IsWindow());
        m_obj->showNormal();
    }

    TMBool IsSelfVisible() const
    {
        DbgAssert(IsWindow());
        return !m_obj->isHidden();
    }
    
    TMBool IsActiveWindow() const
    {
        DbgAssert(IsWindow());
        return m_obj->isActiveWindow();
    }

    void SetHideFocus()
    {
    }
    
    void ResetHideFocus()
    {
    }

    void ScreenToParent(TMRect& rc)
    {
        DbgAssert(IsWindow());
    }

    void GetWindowRectToParent(TMRect& rc)
    {
        DbgAssert(IsWindow());
        rc = m_obj->frameGeometry();
    }

    void EnableAreo(int nClient)
    {
#ifdef TM_WIN
        CWindowNative((HWND)this->m_hWnd->winId()).EnableAreo(nClient);
#endif
    }

    TMBool EnableBlur(TMBool bEnable)
    {
#ifdef TM_WIN
        return CWindowNative((HWND)this->m_hWnd->winId()).EnableBlur(bEnable);
#endif
        return false;
    }
};



///////////////////////////////////////////////////////////////////////////////
// CIcon
template <bool t_bManaged>
class CIconQtT : public QIcon
{
public:
// Constructor/destructor/operators
    CIconQtT(TMHIcon hIcon = TMHIcon())
        : QIcon(hIcon)
    {
    }

    ~CIconQtT()
    {
        if (!t_bManaged)
            this->detach();
    }

    CIconQtT<t_bManaged>& operator = (TMHIcon hIcon)
    {
        QIcon::operator=(hIcon);
        return *this;
    }

    void Attach(TMHIcon hIcon)
    {
        if (!t_bManaged)
            this->detach();
        *this = hIcon;
    }

    TMHIcon Detach()
    {
        TMHIcon hIcon = *this;
        this->detach();
        return hIcon;
    }

//  operator TMHIcon() const { return m_hIcon; }

    bool IsNull() const { return !isDetached(); }
    operator bool() const { return IsNull(); }

// Create/destroy methods
//  TMHIcon LoadIcon(ATL::_U_STRINGorID icon)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      m_hIcon = ::LoadIcon(ModuleHelper::GetResourceInstance(), icon.m_lpstr);
//      return m_hIcon;
//  }
// 
//  TMHIcon LoadIcon(ATL::_U_STRINGorID icon, int cxDesired, int cyDesired, TMUInt fuLoad = 0)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      m_hIcon = (TMHIcon) ::LoadImage(ModuleHelper::GetResourceInstance(), icon.m_lpstr, IMAGE_ICON, cxDesired, cyDesired, fuLoad);
//      return m_hIcon;
//  }
// 
// #ifndef _WIN32_WCE
//  TMHIcon LoadOEMIcon(TMCStr lpstrIconName)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      ATLASSERT(IsOEMIcon(lpstrIconName));
//      m_hIcon = ::LoadIcon(NULL, lpstrIconName);
//      return m_hIcon;
//  }
// 
//  TMHIcon CreateIcon(int nWidth, int nHeight, TMUInt8 cPlanes, TMUInt8 cBitsPixel, const TMUInt8* lpbANDbits, const TMUInt8 *lpbXORbits)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      ATLASSERT(lpbANDbits != NULL);
//      ATLASSERT(lpbXORbits != NULL);
//      m_hIcon = ::CreateIcon(ModuleHelper::GetResourceInstance(), nWidth, nHeight, cPlanes, cBitsPixel, lpbANDbits, lpbXORbits);
//      return m_hIcon;
//  }
// 
//  TMHIcon CreateIconFromResource(PBYTE pBits, TMUInt32 dwResSize, TMUInt32 dwVersion = 0x00030000)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      ATLASSERT(pBits != NULL);
//      m_hIcon = ::CreateIconFromResource(pBits, dwResSize, TRUE, dwVersion);
//      return m_hIcon;
//  }
// 
//  TMHIcon CreateIconFromResourceEx(PBYTE pbBits, TMUInt32 cbBits, TMUInt32 dwVersion = 0x00030000, int cxDesired = 0, int cyDesired = 0, TMUInt uFlags = LR_DEFAULTCOLOR)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      ATLASSERT(pbBits != NULL);
//      ATLASSERT(cbBits > 0);
//      m_hIcon = ::CreateIconFromResourceEx(pbBits, cbBits, TRUE, dwVersion, cxDesired, cyDesired, uFlags);
//      return m_hIcon;
//  }
// #endif // !_WIN32_WCE
// 
//  TMHIcon CreateIconIndirect(PICONINFO pIconInfo)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      ATLASSERT(pIconInfo != NULL);
//      m_hIcon = ::CreateIconIndirect(pIconInfo);
//      return m_hIcon;
//  }
// 
// #ifndef _WIN32_WCE
//  TMHIcon ExtractIcon(TMCStr lpszExeFileName, TMUInt nIconIndex)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      ATLASSERT(lpszExeFileName != NULL);
//      m_hIcon = ::ExtractIcon(ModuleHelper::GetModuleInstance(), lpszExeFileName, nIconIndex);
//      return m_hIcon;
//  }
// 
//  TMHIcon ExtractAssociatedIcon(HINSTANCE hInst, LPTSTR lpIconPath, LPWORD lpiIcon)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      ATLASSERT(lpIconPath != NULL);
//      ATLASSERT(lpiIcon != NULL);
//      m_hIcon = ::ExtractAssociatedIcon(hInst, lpIconPath, lpiIcon);
//      return m_hIcon;
//  }
// #endif // !_WIN32_WCE
// 
//  TMBool DestroyIcon()
//  {
//      ATLASSERT(m_hIcon != NULL);
//      TMBool bRet = ::DestroyIcon(m_hIcon);
//      if (bRet != FALSE)
//          m_hIcon = NULL;
//      return bRet;
//  }
// 
// // Operations
// #ifndef _WIN32_WCE
//  TMHIcon CopyIcon()
//  {
//      ATLASSERT(m_hIcon != NULL);
//      return ::CopyIcon(m_hIcon);
//  }
// 
//  TMHIcon DuplicateIcon()
//  {
//      ATLASSERT(m_hIcon != NULL);
//      return ::DuplicateIcon(NULL, m_hIcon);
//  }
// #endif // !_WIN32_WCE
// 
//  TMBool DrawIcon(HDC hDC, int x, int y)
//  {
//      ATLASSERT(m_hIcon != NULL);
// #ifndef _WIN32_WCE
//      return ::DrawIcon(hDC, x, y, m_hIcon);
// #else // CE specific
//      return ::DrawIconEx(hDC, x, y, m_hIcon, 0, 0, 0, NULL, DI_NORMAL);
// #endif // _WIN32_WCE
//  }
// 
//  TMBool DrawIcon(HDC hDC, POINT pt)
//  {
//      ATLASSERT(m_hIcon != NULL);
// #ifndef _WIN32_WCE
//      return ::DrawIcon(hDC, pt.x, pt.y, m_hIcon);
// #else // CE specific
//      return ::DrawIconEx(hDC, pt.x, pt.y, m_hIcon, 0, 0, 0, NULL, DI_NORMAL);
// #endif // _WIN32_WCE
//  }
// 
//  TMBool DrawIconEx(HDC hDC, int x, int y, int cxWidth, int cyWidth, TMUInt uStepIfAniCur = 0, TMHBrush hbrFlickerFreeDraw = NULL, TMUInt uFlags = DI_NORMAL)
//  {
//      ATLASSERT(m_hIcon != NULL);
//      return ::DrawIconEx(hDC, x, y, m_hIcon, cxWidth, cyWidth, uStepIfAniCur, hbrFlickerFreeDraw, uFlags);
//  }
// 
//  TMBool DrawIconEx(HDC hDC, POINT pt, SIZE size, TMUInt uStepIfAniCur = 0, TMHBrush hbrFlickerFreeDraw = NULL, TMUInt uFlags = DI_NORMAL)
//  {
//      ATLASSERT(m_hIcon != NULL);
//      return ::DrawIconEx(hDC, pt.x, pt.y, m_hIcon, size.cx, size.cy, uStepIfAniCur, hbrFlickerFreeDraw, uFlags);
//  }
// 
// #ifndef _WIN32_WCE
//  TMBool GetIconInfo(PICONINFO pIconInfo) const
//  {
//      ATLASSERT(m_hIcon != NULL);
//      ATLASSERT(pIconInfo != NULL);
//      return ::GetIconInfo(m_hIcon, pIconInfo);
//  }
// 
// #if (_WIN32_WINNT >= 0x0600)
//  TMBool GetIconInfoEx(PICONINFOEX pIconInfo) const
//  {
//      ATLASSERT(m_hIcon != NULL);
//      ATLASSERT(pIconInfo != NULL);
//      return ::GetIconInfoEx(m_hIcon, pIconInfo);
//  }
// #endif // (_WIN32_WINNT >= 0x0600)
// 
// #if defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_LONGHORN)
//  TMResult LoadIconMetric(ATL::_U_STRINGorID icon, int lims)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      USES_CONVERSION;
//      return ::LoadIconMetric(ModuleHelper::GetResourceInstance(), T2CW(icon.m_lpstr), lims, &m_hIcon);
//  }
// 
//  TMResult LoadIconWithScaleDown(ATL::_U_STRINGorID icon, int cx, int cy)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      USES_CONVERSION;
//      return ::LoadIconWithScaleDown(ModuleHelper::GetResourceInstance(), T2CW(icon.m_lpstr), cx, cy, &m_hIcon);
//  }
// 
//  TMResult LoadOEMIconMetric(TMCStr lpstrIconName, int lims)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      ATLASSERT(IsOEMIcon(lpstrIconName));
//      return ::LoadIconMetric(NULL, (LPCWSTR)lpstrIconName, lims, &m_hIcon);
//  }
// 
//  TMResult LoadOEMIconWithScaleDown(TMCStr lpstrIconName, int cx, int cy)
//  {
//      ATLASSERT(m_hIcon == NULL);
//      ATLASSERT(IsOEMIcon(lpstrIconName));
//      USES_CONVERSION;
//      return ::LoadIconWithScaleDown(NULL, (LPCWSTR)lpstrIconName, cx, cy, &m_hIcon);
//  }
// #endif // defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_LONGHORN)
// #endif // !_WIN32_WCE
// 
//  // Helper
// #ifndef _WIN32_WCE
//  static bool IsOEMIcon(TMCStr lpstrIconName)
//  {
// #if (WINVER >= 0x0600)
//      return (lpstrIconName == IDI_APPLICATION || lpstrIconName == IDI_ASTERISK || lpstrIconName == IDI_EXCLAMATION ||
//                lpstrIconName == IDI_HAND || lpstrIconName == IDI_QUESTION || lpstrIconName == IDI_WINLOGO ||
//                lpstrIconName == IDI_SHIELD);
// #else // !(WINVER >= 0x0600)
//      return (lpstrIconName == IDI_APPLICATION || lpstrIconName == IDI_ASTERISK || lpstrIconName == IDI_EXCLAMATION ||
//                lpstrIconName == IDI_HAND || lpstrIconName == IDI_QUESTION || lpstrIconName == IDI_WINLOGO);
// #endif // !(WINVER >= 0x0600)
//  }
// #endif // !_WIN32_WCE
};

typedef CIconQtT<false>   CIconHandleQt;
typedef CIconQtT<true>    CIconQt;


///////////////////////////////////////////////////////////////////////////////
// CCursor

// protect template member from a winuser.h macro
// #ifdef CopyCursor
//   #undef CopyCursor
// #endif
// 
// template <bool t_bManaged>
// class CCursorT
// {
// public:
//  HCURSOR m_hCursor;
// 
// // Constructor/destructor/operators
//  CCursorT(HCURSOR hCursor = NULL) : m_hCursor(hCursor)
//  { }
// 
//  ~CCursorT()
//  {
//      if (t_bManaged && m_hCursor != NULL)
//          DestroyCursor();
//  }
// 
//  CCursorT<t_bManaged>& operator =(HCURSOR hCursor)
//  {
//      Attach(hCursor);
//      return *this;
//  }
// 
//  void Attach(HCURSOR hCursor)
//  {
//      if (t_bManaged && m_hCursor != NULL)
//          DestroyCursor();
//      m_hCursor = hCursor;
//  }
// 
//  HCURSOR Detach()
//  {
//      HCURSOR hCursor = m_hCursor;
//      m_hCursor = NULL;
//      return hCursor;
//  }
// 
//  operator HCURSOR() const { return m_hCursor; }
// 
//  bool IsNull() const { return m_hCursor == NULL; }
// 
// // Create/destroy methods
//  HCURSOR LoadCursor(ATL::_U_STRINGorID cursor)
//  {
//      ATLASSERT(m_hCursor == NULL);
//      m_hCursor = ::LoadCursor(ModuleHelper::GetResourceInstance(), cursor.m_lpstr);
//      return m_hCursor;
//  }
// 
//  HCURSOR LoadSysCursor(TMCStr lpstrCursorName)
//  {
//      ATLASSERT(m_hCursor == NULL);
// #if (WINVER >= 0x0500)
//      ATLASSERT(lpstrCursorName == IDC_ARROW || lpstrCursorName == IDC_IBEAM || lpstrCursorName == IDC_WAIT ||
//          lpstrCursorName == IDC_CROSS || lpstrCursorName == IDC_UPARROW || lpstrCursorName == IDC_SIZE ||
//          lpstrCursorName == IDC_ICON || lpstrCursorName == IDC_SIZENWSE || lpstrCursorName == IDC_SIZENESW ||
//          lpstrCursorName == IDC_SIZEWE || lpstrCursorName == IDC_SIZENS || lpstrCursorName == IDC_SIZEALL ||
//          lpstrCursorName == IDC_NO || lpstrCursorName == IDC_APPSTARTING || lpstrCursorName == IDC_HELP ||
//          lpstrCursorName == IDC_HAND);
// #else // !(WINVER >= 0x0500)
//      ATLASSERT(lpstrCursorName == IDC_ARROW || lpstrCursorName == IDC_IBEAM || lpstrCursorName == IDC_WAIT ||
//          lpstrCursorName == IDC_CROSS || lpstrCursorName == IDC_UPARROW || lpstrCursorName == IDC_SIZE ||
//          lpstrCursorName == IDC_ICON || lpstrCursorName == IDC_SIZENWSE || lpstrCursorName == IDC_SIZENESW ||
//          lpstrCursorName == IDC_SIZEWE || lpstrCursorName == IDC_SIZENS || lpstrCursorName == IDC_SIZEALL ||
//          lpstrCursorName == IDC_NO || lpstrCursorName == IDC_APPSTARTING || lpstrCursorName == IDC_HELP);
// #endif // !(WINVER >= 0x0500)
//      m_hCursor = ::LoadCursor(NULL, lpstrCursorName);
//      return m_hCursor;
//  }
// 
//  // deprecated
//  HCURSOR LoadOEMCursor(TMCStr lpstrCursorName)
//  {
//      return LoadSysCursor(lpstrCursorName);
//  }
// 
//  HCURSOR LoadCursor(ATL::_U_STRINGorID cursor, int cxDesired, int cyDesired, TMUInt fuLoad = 0)
//  {
//      ATLASSERT(m_hCursor == NULL);
//      m_hCursor = (HCURSOR) ::LoadImage(ModuleHelper::GetResourceInstance(), cursor.m_lpstr, IMAGE_CURSOR, cxDesired, cyDesired, fuLoad);
//      return m_hCursor;
//  }
// 
// #ifndef _WIN32_WCE
//  HCURSOR LoadCursorFromFile(TMCStr pstrFilename)
//  {
//      ATLASSERT(m_hCursor == NULL);
//      ATLASSERT(pstrFilename != NULL);
//      m_hCursor = ::LoadCursorFromFile(pstrFilename);
//      return m_hCursor;
//  }
// #endif // !_WIN32_WCE
// 
// #if !defined(_WIN32_WCE) || ((_WIN32_WCE >= 0x400) && !(defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)))
//  HCURSOR CreateCursor(int xHotSpot, int yHotSpot, int nWidth, int nHeight, const void *pvANDPlane, const void *pvXORPlane)
//  {
//      ATLASSERT(m_hCursor == NULL);
//      m_hCursor = ::CreateCursor(ModuleHelper::GetResourceInstance(), xHotSpot, yHotSpot, nWidth, nHeight, pvANDPlane, pvXORPlane);
//      return m_hCursor;
//  }
// #endif // !defined(_WIN32_WCE) || ((_WIN32_WCE >= 0x400) && !(defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)))
// 
// #ifndef _WIN32_WCE
//  HCURSOR CreateCursorFromResource(PBYTE pBits, TMUInt32 dwResSize, TMUInt32 dwVersion = 0x00030000)
//  {
//      ATLASSERT(m_hCursor == NULL);
//      ATLASSERT(pBits != NULL);
//      m_hCursor = (HCURSOR)::CreateIconFromResource(pBits, dwResSize, FALSE, dwVersion);
//      return m_hCursor;
//  }
// 
//  HCURSOR CreateCursorFromResourceEx(PBYTE pbBits, TMUInt32 cbBits, TMUInt32 dwVersion = 0x00030000, int cxDesired = 0, int cyDesired = 0, TMUInt uFlags = LR_DEFAULTCOLOR)
//  {
//      ATLASSERT(m_hCursor == NULL);
//      ATLASSERT(pbBits != NULL);
//      ATLASSERT(cbBits > 0);
//      m_hCursor = (HCURSOR)::CreateIconFromResourceEx(pbBits, cbBits, FALSE, dwVersion, cxDesired, cyDesired, uFlags);
//      return m_hCursor;
//  }
// #endif // !_WIN32_WCE
// 
//  TMBool DestroyCursor()
//  {
//      ATLASSERT(m_hCursor != NULL);
// #if !defined(_WIN32_WCE) || ((_WIN32_WCE >= 0x400) && !(defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)))
//      TMBool bRet = ::DestroyCursor(m_hCursor);
//      if (bRet != FALSE)
//          m_hCursor = NULL;
//      return bRet;
// #else // !(!defined(_WIN32_WCE) || ((_WIN32_WCE >= 0x400) && !(defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP))))
//      ATLTRACE2(atlTraceUI, 0, _T("Warning: This version of Windows CE does not have ::DestroyCursor()\n"));
//      return FALSE;
// #endif // !(!defined(_WIN32_WCE) || ((_WIN32_WCE >= 0x400) && !(defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP))))
//  }
// 
// // Operations
// #ifndef _WIN32_WCE
//  HCURSOR CopyCursor()
//  {
//      ATLASSERT(m_hCursor != NULL);
//      return (HCURSOR)::CopyIcon((TMHIcon)m_hCursor);
//  }
// #endif // !_WIN32_WCE
// 
// #if (WINVER >= 0x0500) && !defined(_WIN32_WCE)
//  TMBool GetCursorInfo(LPCURSORINFO pCursorInfo)
//  {
//      ATLASSERT(m_hCursor != NULL);
//      ATLASSERT(pCursorInfo != NULL);
//      return ::GetCursorInfo(pCursorInfo);
//  }
// #endif // (WINVER >= 0x0500) && !defined(_WIN32_WCE)
// };
// 
// typedef CCursorT<false>   CCursorHandle;
// typedef CCursorT<true>    CCursor;



///////////////////////////////////////////////////////////////////////////////
// auto function
struct CAutoCompositionMode
{
    TMHDC m_hDC;
    QPainter::CompositionMode m_modeOld;

    CAutoCompositionMode(TMHDC _hdc, QPainter::CompositionMode mode)
        : m_hDC(_hdc)
        , m_modeOld(m_hDC->compositionMode())
    {
        m_hDC->setCompositionMode(mode);
    }

    ~CAutoCompositionMode()
    {
        m_hDC->setCompositionMode(m_modeOld);
    }
};

struct CAutoOpacity
{
    TMHDC m_hDC;

    CAutoOpacity(TMHDC _hdc, qreal opacity)
        : m_hDC(_hdc)
    {
        m_hDC->setOpacity(opacity);
    }

    ~CAutoOpacity()
    {
        m_hDC->setOpacity(1);
    }
};

struct CAutoPen
{
    TMHDC m_hDC;
    QPen m_old;

    CAutoPen(TMHDC _hdc)
        : m_hDC(_hdc)
        , m_old(_hdc->pen())
    {
    }

    CAutoPen(TMHDC _hdc, Qt::PenStyle ps)
        : m_hDC(_hdc)
        , m_old(_hdc->pen())
    {
        m_hDC->setPen(QPen(ps));
    }

    CAutoPen(TMHDC _hdc, TMColor clr)
        : m_hDC(_hdc)
        , m_old(_hdc->pen())
    {
        m_hDC->setPen(ColorToQColor(clr));
    }

    CAutoPen(TMHDC _hdc, QColor clr)
        : m_hDC(_hdc)
        , m_old(_hdc->pen())
    {
        m_hDC->setPen(clr);
    }

    CAutoPen(TMHDC _hdc, QPen pen)
        : m_hDC(_hdc)
        , m_old(_hdc->pen())
    {
        m_hDC->setPen(pen);
    }

    ~CAutoPen()
    {
        m_hDC->setPen(m_old);
    }
};

struct CAutoBrush
{
    TMHDC m_hDC;
    QBrush m_old;
    
    CAutoBrush(TMHDC _hdc)
        : m_hDC(_hdc)
        , m_old(_hdc->brush())
    {
    }

    CAutoBrush(TMHDC _hdc, QBrush brush)
        : m_hDC(_hdc)
        , m_old(_hdc->brush())
    {
        m_hDC->setBrush(brush);
    }
    
//     CAutoBrush(TMHDC _hdc, QColor clr)
//         : m_hDC(_hdc)
//     {
//         m_hDC->setBrush(clr);
//     }
//     
//     CAutoBrush(TMHDC _hdc, QPen pen)
//         : m_hDC(_hdc)
//     {
//         m_hDC->setBrush(pen);
//     }
    
    ~CAutoBrush()
    {
        m_hDC->setBrush(m_old);
    }
};


///////////////////////////////////////////////////////////////////////////////
// CDC - The device context class
// class QImage : public QPaintDevice;
// class QPixmap : public QPaintDevice
template <bool t_bManaged>
class CDCQtT
{
public:
// Data members
    QPainter* m_hDC;

// Constructor/destructor/operators
    CDCQtT(TMHDC hDC = NULL) : m_hDC(hDC)
    {
    }

    ~CDCQtT()
    {
        if (t_bManaged && m_hDC != NULL)
            delete m_hDC;
    }

    CDCQtT<t_bManaged>& operator =(TMHDC hDC)
    {
        Attach(hDC);
        return *this;
    }

    void Attach(TMHDC hDC)
    {
        if (t_bManaged && m_hDC != NULL && m_hDC != hDC)
            delete m_hDC;
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
#ifndef _WIN32_WCE
    TMHWnd WindowFromDC() const
    {
        DbgAssert(m_hDC != NULL);
        return ::WindowFromDC(m_hDC);
    }
#endif // !_WIN32_WCE

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
            return FALSE;
//        m_hDC->Release();
        return true;
    }
    
// Device-Context Functions
    int SaveDC()
    {
        DbgAssert(m_hDC != NULL);
        m_hDC->save();
        return -1;
    }

    TMBool RestoreDC(int nSavedDC)
    {
        DbgAssert(m_hDC != NULL);
        m_hDC->restore();
        return true;
    }
    /*
    int GetDeviceCaps(int nIndex) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetDeviceCaps(m_hDC, nIndex);
    }

#ifndef _WIN32_WCE
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
#endif // !_WIN32_WCE

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

// Drawing-Attribute Functions
    COLORREF GetBkColor() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetBkColor(m_hDC);
    }

    int GetBkMode() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetBkMode(m_hDC);
    }

#ifndef _WIN32_WCE
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
#endif // !_WIN32_WCE

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


// Mapping Functions
    void SetGraphicsModeAdv(TMBool bAdv) const
    {
        DbgAssert(m_hDC != NULL);
        m_hDC->setMatrixEnabled(bAdv ? true : false);
    }

    bool IsGraphicsModeAdv() const
    {
        DbgAssert(m_hDC != NULL);
        return m_hDC->matrixEnabled();
    }

    TMBool GetWorldTransform(TMAffine& xform)
    {
        DbgAssert(m_hDC != NULL);
        xform = m_hDC->matrix();
        return true;
    }

    TMBool SetWorldTransform(const TMAffine& xform)
    {
        DbgAssert(m_hDC != NULL);
        m_hDC->setMatrix(xform);
        return true;
    }

    TMBool LeftMultiplyWorldTransform(const TMAffine* xform)
    {
        DbgAssert(this->m_hDC != NULL);
        m_hDC->setMatrix(*xform, true);
        return true;
    }
    
    TMBool RightMultiplyWorldTransform(const TMAffine* xform)
    {
        DbgAssert(this->m_hDC != NULL);
        CAffine aff;
        GetWorldTransform(aff);
        aff.PreCombine(*xform);
        m_hDC->setMatrix(*xform);
        return true;
    }


/*
    int GetMapMode() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetMapMode(m_hDC);
    }

    TMBool GetViewportOrg(LPPOINT lpPoint) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetViewportOrgEx(m_hDC, lpPoint);
    }

    int SetMapMode(int nMapMode)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetMapMode(m_hDC, nMapMode);
    }
#endif // !_WIN32_WCE

    // Viewport Origin
    TMBool SetViewportOrg(int x, int y, LPPOINT lpPoint = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetViewportOrgEx(m_hDC, x, y, lpPoint);
    }

    TMBool SetViewportOrg(POINT point, LPPOINT lpPointRet = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return SetViewportOrg(point.x, point.y, lpPointRet);
    }

#ifndef _WIN32_WCE
    TMBool OffsetViewportOrg(int nWidth, int nHeight, LPPOINT lpPoint = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::OffsetViewportOrgEx(m_hDC, nWidth, nHeight, lpPoint);
    }

    // Viewport Extent
    TMBool GetViewportExt(LPSIZE lpSize) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetViewportExtEx(m_hDC, lpSize);
    }

    TMBool SetViewportExt(int x, int y, LPSIZE lpSize = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetViewportExtEx(m_hDC, x, y, lpSize);
    }

    TMBool SetViewportExt(SIZE size, LPSIZE lpSizeRet = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return SetViewportExt(size.cx, size.cy, lpSizeRet);
    }

    TMBool ScaleViewportExt(int xNum, int xDenom, int yNum, int yDenom, LPSIZE lpSize = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::ScaleViewportExtEx(m_hDC, xNum, xDenom, yNum, yDenom, lpSize);
    }
#endif // !_WIN32_WCE

    // Window Origin
#ifndef _WIN32_WCE
    TMBool GetWindowOrg(LPPOINT lpPoint) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetWindowOrgEx(m_hDC, lpPoint);
    }

    TMBool SetWindowOrg(int x, int y, LPPOINT lpPoint = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetWindowOrgEx(m_hDC, x, y, lpPoint);
    }

    TMBool SetWindowOrg(POINT point, LPPOINT lpPointRet = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return SetWindowOrg(point.x, point.y, lpPointRet);
    }

    TMBool OffsetWindowOrg(int nWidth, int nHeight, LPPOINT lpPoint = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::OffsetWindowOrgEx(m_hDC, nWidth, nHeight, lpPoint);
    }

    // Window extent
    TMBool GetWindowExt(LPSIZE lpSize) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetWindowExtEx(m_hDC, lpSize);
    }

    TMBool SetWindowExt(int x, int y, LPSIZE lpSize = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetWindowExtEx(m_hDC, x, y, lpSize);
    }

    TMBool SetWindowExt(SIZE size, LPSIZE lpSizeRet = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return SetWindowExt(size.cx, size.cy, lpSizeRet);
    }

    TMBool ScaleWindowExt(int xNum, int xDenom, int yNum, int yDenom, LPSIZE lpSize = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::ScaleWindowExtEx(m_hDC, xNum, xDenom, yNum, yDenom, lpSize);
    }

// Coordinate Functions
    TMBool DPtoLP(LPPOINT lpPoints, int nCount = 1) const
    {
        DbgAssert(m_hDC != NULL);
        return ::DPtoLP(m_hDC, lpPoints, nCount);
    }

    TMBool DPtoLP(LPRECT lpRect) const
    {
        DbgAssert(m_hDC != NULL);
        return ::DPtoLP(m_hDC, (LPPOINT)lpRect, 2);
    }

    TMBool DPtoLP(LPSIZE lpSize) const
    {
        SIZE sizeWinExt = { 0, 0 };
        if (!GetWindowExt(&sizeWinExt))
            return FALSE;
        SIZE sizeVpExt = { 0, 0 };
        if (!GetViewportExt(&sizeVpExt))
            return FALSE;
        lpSize->cx = ::MulDiv(lpSize->cx, abs(sizeWinExt.cx), abs(sizeVpExt.cx));
        lpSize->cy = ::MulDiv(lpSize->cy, abs(sizeWinExt.cy), abs(sizeVpExt.cy));
        return TRUE;
    }

    TMBool LPtoDP(LPPOINT lpPoints, int nCount = 1) const
    {
        DbgAssert(m_hDC != NULL);
        return ::LPtoDP(m_hDC, lpPoints, nCount);
    }

    TMBool LPtoDP(LPRECT lpRect) const
    {
        DbgAssert(m_hDC != NULL);
        return ::LPtoDP(m_hDC, (LPPOINT)lpRect, 2);
    }

    TMBool LPtoDP(LPSIZE lpSize) const
    {
        SIZE sizeWinExt = { 0, 0 };
        if (!GetWindowExt(&sizeWinExt))
            return FALSE;
        SIZE sizeVpExt = { 0, 0 };
        if (!GetViewportExt(&sizeVpExt))
            return FALSE;
        lpSize->cx = ::MulDiv(lpSize->cx, abs(sizeVpExt.cx), abs(sizeWinExt.cx));
        lpSize->cy = ::MulDiv(lpSize->cy, abs(sizeVpExt.cy), abs(sizeWinExt.cy));
        return TRUE;
    }

// Special Coordinate Functions (useful for dealing with metafiles and OLE)
    #define HIMETRIC_INCH   2540    // HIMETRIC units per inch

    void DPtoHIMETRIC(LPSIZE lpSize) const
    {
        DbgAssert(m_hDC != NULL);
        int nMapMode;
        if ((nMapMode = GetMapMode()) < MM_ISOTROPIC && nMapMode != MM_TEXT)
        {
            // when using a constrained map mode, map against physical inch
            ((CDCHandle*)this)->SetMapMode(MM_HIMETRIC);
            DPtoLP(lpSize);
            ((CDCHandle*)this)->SetMapMode(nMapMode);
        }
        else
        {
            // map against logical inch for non-constrained mapping modes
            int cxPerInch = GetDeviceCaps(LOGPIXELSX);
            int cyPerInch = GetDeviceCaps(LOGPIXELSY);
            DbgAssert(cxPerInch != 0 && cyPerInch != 0);
            lpSize->cx = ::MulDiv(lpSize->cx, HIMETRIC_INCH, cxPerInch);
            lpSize->cy = ::MulDiv(lpSize->cy, HIMETRIC_INCH, cyPerInch);
        }
    }

    void HIMETRICtoDP(LPSIZE lpSize) const
    {
        DbgAssert(m_hDC != NULL);
        int nMapMode;
        if ((nMapMode = GetMapMode()) < MM_ISOTROPIC && nMapMode != MM_TEXT)
        {
            // when using a constrained map mode, map against physical inch
            ((CDCHandle*)this)->SetMapMode(MM_HIMETRIC);
            LPtoDP(lpSize);
            ((CDCHandle*)this)->SetMapMode(nMapMode);
        }
        else
        {
            // map against logical inch for non-constrained mapping modes
            int cxPerInch = GetDeviceCaps(LOGPIXELSX);
            int cyPerInch = GetDeviceCaps(LOGPIXELSY);
            DbgAssert(cxPerInch != 0 && cyPerInch != 0);
            lpSize->cx = ::MulDiv(lpSize->cx, cxPerInch, HIMETRIC_INCH);
            lpSize->cy = ::MulDiv(lpSize->cy, cyPerInch, HIMETRIC_INCH);
        }
    }

    void LPtoHIMETRIC(LPSIZE lpSize) const
    {
        LPtoDP(lpSize);
        DPtoHIMETRIC(lpSize);
    }

    void HIMETRICtoLP(LPSIZE lpSize) const
    {
        HIMETRICtoDP(lpSize);
        DPtoLP(lpSize);
    }
#endif // !_WIN32_WCE

// Region Functions
    TMBool FillRgn(HRGN hRgn, TMHBrush hBrush)
    {
        DbgAssert(m_hDC != NULL);
        return ::FillRgn(m_hDC, hRgn, hBrush);
    }

#ifndef _WIN32_WCE
    TMBool FrameRgn(HRGN hRgn, TMHBrush hBrush, int nWidth, int nHeight)
    {
        DbgAssert(m_hDC != NULL);
        return ::FrameRgn(m_hDC, hRgn, hBrush, nWidth, nHeight);
    }

    TMBool InvertRgn(HRGN hRgn)
    {
        DbgAssert(m_hDC != NULL);
        return ::InvertRgn(m_hDC, hRgn);
    }

    TMBool PaintRgn(HRGN hRgn)
    {
        DbgAssert(m_hDC != NULL);
        return ::PaintRgn(m_hDC, hRgn);
    }
#endif // !_WIN32_WCE
*/
// Clipping Functions
    void SetClipRect(const TMRect& rc)
    {
        DbgAssert(m_hDC != NULL);
        m_hDC->setClipRect(rc);
    }

/*
    int GetClipBox(LPRECT lpRect) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetClipBox(m_hDC, lpRect);
    }

    int GetClipRgn(CRgn& region) const
    {
        DbgAssert(m_hDC != NULL);
        if (region.IsNull())
            region.CreateRectRgn(0, 0, 0, 0);

        int nRet = ::GetClipRgn(m_hDC, region);
        if (nRet != 1)
            region.DeleteObject();

        return nRet;
    }

#ifndef _WIN32_WCE
    TMBool PtVisible(int x, int y) const
    {
        DbgAssert(m_hDC != NULL);
        return ::PtVisible(m_hDC, x, y);
    }

    TMBool PtVisible(POINT point) const
    {
        DbgAssert(m_hDC != NULL);
        return ::PtVisible(m_hDC, point.x, point.y);
    }
#endif // !_WIN32_WCE

    TMBool RectVisible(LPCRECT lpRect) const
    {
        DbgAssert(m_hDC != NULL);
        return ::RectVisible(m_hDC, lpRect);
    }

    int SelectClipRgn(HRGN hRgn)
    {
        DbgAssert(m_hDC != NULL);
        return ::SelectClipRgn(m_hDC, (HRGN)hRgn);
    }

    int ExcludeClipRect(int x1, int y1, int x2, int y2)
    {
        DbgAssert(m_hDC != NULL);
        return ::ExcludeClipRect(m_hDC, x1, y1, x2, y2);
    }

    int ExcludeClipRect(LPCRECT lpRect)
    {
        DbgAssert(m_hDC != NULL);
        return ::ExcludeClipRect(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
    }

#ifndef _WIN32_WCE
    int ExcludeUpdateRgn(TMHWnd hWnd)
    {
        DbgAssert(m_hDC != NULL);
        return ::ExcludeUpdateRgn(m_hDC, hWnd);
    }
#endif // !_WIN32_WCE
*/
    int IntersectClipRect(int x1, int y1, int x2, int y2)
    {
        DbgAssert(m_hDC != NULL);
        CRect rc(x1, y1, x2, y2);
        return IntersectClipRect(&rc);
    }
    
    int IntersectClipRect(const TMRect* lpRect)
    {
        DbgAssert(m_hDC != NULL);
        DbgAssert(lpRect);
        m_hDC->setClipRect(*lpRect, Qt::IntersectClip);
        return true;
    }
    /*
#ifndef _WIN32_WCE
    int OffsetClipRgn(int x, int y)
    {
        DbgAssert(m_hDC != NULL);
        return ::OffsetClipRgn(m_hDC, x, y);
    }

    int OffsetClipRgn(SIZE size)
    {
        DbgAssert(m_hDC != NULL);
        return ::OffsetClipRgn(m_hDC, size.cx, size.cy);
    }

    int SelectClipRgn(HRGN hRgn, int nMode)
    {
        DbgAssert(m_hDC != NULL);
        return ::ExtSelectClipRgn(m_hDC, hRgn, nMode);
    }
#endif // !_WIN32_WCE

// Line-Output Functions
#if !defined(_WIN32_WCE) || (_WIN32_WCE >= 400)
    TMBool GetCurrentPosition(LPPOINT lpPoint) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetCurrentPositionEx(m_hDC, lpPoint);
    }

    TMBool MoveTo(int x, int y, LPPOINT lpPoint = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::MoveToEx(m_hDC, x, y, lpPoint);
    }

    TMBool MoveTo(POINT point, LPPOINT lpPointRet = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return MoveTo(point.x, point.y, lpPointRet);
    }

    TMBool LineTo(int x, int y)
    {
        DbgAssert(m_hDC != NULL);
        return ::LineTo(m_hDC, x, y);
    }

    TMBool LineTo(POINT point)
    {
        DbgAssert(m_hDC != NULL);
        return LineTo(point.x, point.y);
    }
#endif // !defined(_WIN32_WCE) || (_WIN32_WCE >= 400)

#ifndef _WIN32_WCE
    TMBool Arc(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
    {
        DbgAssert(m_hDC != NULL);
        return ::Arc(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4);
    }

    TMBool Arc(LPCRECT lpRect, POINT ptStart, POINT ptEnd)
    {
        DbgAssert(m_hDC != NULL);
        return ::Arc(m_hDC, lpRect->left, lpRect->top,
            lpRect->right, lpRect->bottom, ptStart.x, ptStart.y,
            ptEnd.x, ptEnd.y);
    }
#endif // !_WIN32_WCE

    TMBool Polyline(const POINT* lpPoints, int nCount)
    {
        DbgAssert(m_hDC != NULL);
        return ::Polyline(m_hDC, lpPoints, nCount);
    }

#ifndef _WIN32_WCE
    TMBool AngleArc(int x, int y, int nRadius, float fStartAngle, float fSweepAngle)
    {
        DbgAssert(m_hDC != NULL);
        return ::AngleArc(m_hDC, x, y, nRadius, fStartAngle, fSweepAngle);
    }

    TMBool ArcTo(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
    {
        DbgAssert(m_hDC != NULL);
        return ::ArcTo(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4);
    }

    TMBool ArcTo(LPCRECT lpRect, POINT ptStart, POINT ptEnd)
    {
        DbgAssert(m_hDC != NULL);
        return ArcTo(lpRect->left, lpRect->top, lpRect->right,
        lpRect->bottom, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
    }

    int GetArcDirection() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetArcDirection(m_hDC);
    }

    int SetArcDirection(int nArcDirection)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetArcDirection(m_hDC, nArcDirection);
    }

    TMBool PolyDraw(const POINT* lpPoints, const TMUInt8* lpTypes, int nCount)
    {
        DbgAssert(m_hDC != NULL);
        return ::PolyDraw(m_hDC, lpPoints, lpTypes, nCount);
    }

    TMBool PolylineTo(const POINT* lpPoints, int nCount)
    {
        DbgAssert(m_hDC != NULL);
        return ::PolylineTo(m_hDC, lpPoints, nCount);
    }

    TMBool PolyPolyline(const POINT* lpPoints,
        const TMUInt32* lpPolyPoints, int nCount)
    {
        DbgAssert(m_hDC != NULL);
        return ::PolyPolyline(m_hDC, lpPoints, lpPolyPoints, nCount);
    }

    TMBool PolyBezier(const POINT* lpPoints, int nCount)
    {
        DbgAssert(m_hDC != NULL);
        return ::PolyBezier(m_hDC, lpPoints, nCount);
    }

    TMBool PolyBezierTo(const POINT* lpPoints, int nCount)
    {
        DbgAssert(m_hDC != NULL);
        return ::PolyBezierTo(m_hDC, lpPoints, nCount);
    }
#endif // !_WIN32_WCE

// Simple Drawing Functions
    TMBool FillRect(LPCRECT lpRect, TMHBrush hBrush)
    {
        DbgAssert(m_hDC != NULL);
        return ::FillRect(m_hDC, lpRect, hBrush);
    }

    TMBool FillRect(LPCRECT lpRect, int nColorIndex)
    {
        DbgAssert(m_hDC != NULL);
#ifndef _WIN32_WCE
        return ::FillRect(m_hDC, lpRect, (TMHBrush)LongToPtr(nColorIndex + 1));
#else // CE specific
        return ::FillRect(m_hDC, lpRect, ::GetSysColorBrush(nColorIndex));
#endif // _WIN32_WCE
    }

#ifndef _WIN32_WCE
    TMBool FrameRect(LPCRECT lpRect, TMHBrush hBrush)
    {
        DbgAssert(m_hDC != NULL);
        return ::FrameRect(m_hDC, lpRect, hBrush);
    }
#endif // !_WIN32_WCE
*/
    TMBool InvertRect(const TMRect* lpRect)
    {
        DbgAssert(this->m_hDC != NULL);
        CAutoCompositionMode mode(this->m_hDC, QPainter::RasterOp_SourceXorDestination);
        FillSolidRect(lpRect, 0);
        return true;
    }

    /*
    TMBool DrawIcon(int x, int y, TMHIcon hIcon)
    {
        DbgAssert(m_hDC != NULL);
#ifndef _WIN32_WCE
        return ::DrawIcon(m_hDC, x, y, hIcon);
#else // CE specific
        return ::DrawIconEx(m_hDC, x, y, hIcon, 0, 0, 0, NULL, DI_NORMAL);
#endif // _WIN32_WCE
    }

    TMBool DrawIcon(POINT point, TMHIcon hIcon)
    {
        DbgAssert(m_hDC != NULL);
#ifndef _WIN32_WCE
        return ::DrawIcon(m_hDC, point.x, point.y, hIcon);
#else // CE specific
        return ::DrawIconEx(m_hDC, point.x, point.y, hIcon, 0, 0, 0, NULL, DI_NORMAL);
#endif // _WIN32_WCE
    }*/

    TMBool DrawIconEx(int x, int y, TMHIcon hIcon, int cxWidth, int cyWidth, TMUInt uStepIfAniCur = 0, TMHBrush hbrFlickerFreeDraw = NULL, TMUInt uFlags = DI_NORMAL)
    {
        DbgAssert(m_hDC != NULL);
        return DrawIconEx(CPoint(x,y), hIcon, CSize(cxWidth, cyWidth), uStepIfAniCur, hbrFlickerFreeDraw, uFlags);
    }

    TMBool DrawIconEx(const TMPoint& point, TMHIcon hIcon, const TMSize& size, TMUInt uStepIfAniCur = 0, TMHBrush hbrFlickerFreeDraw = NULL, TMUInt uFlags = DI_NORMAL)
    {
        DbgAssert(m_hDC != NULL);
        DbgAssert(uStepIfAniCur == 0 && hbrFlickerFreeDraw == 0 && uFlags == DI_NORMAL);
        QPixmap pixmap(hIcon.pixmap(size));
        if (pixmap.isNull())
            return false;
        m_hDC->drawPixmap(point, pixmap);
        return true;
    }
    /*
#ifndef _WIN32_WCE
    TMBool DrawState(POINT pt, SIZE size, HBITMAP hBitmap, TMUInt nFlags, TMHBrush hBrush = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::DrawState(m_hDC, hBrush, NULL, (TMLParam)hBitmap, 0, pt.x, pt.y, size.cx, size.cy, nFlags | DST_BITMAP);
    }

    TMBool DrawState(POINT pt, SIZE size, TMHIcon hIcon, TMUInt nFlags, TMHBrush hBrush = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::DrawState(m_hDC, hBrush, NULL, (TMLParam)hIcon, 0, pt.x, pt.y, size.cx, size.cy, nFlags | DST_ICON);
    }

    TMBool DrawState(POINT pt, SIZE size, TMCStr lpszText, TMUInt nFlags, TMBool bPrefixText = TRUE, int nTextLen = 0, TMHBrush hBrush = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::DrawState(m_hDC, hBrush, NULL, (TMLParam)lpszText, (TMWParam)nTextLen, pt.x, pt.y, size.cx, size.cy, nFlags | (bPrefixText ? DST_PREFIXTEXT : DST_TEXT));
    }

    TMBool DrawState(POINT pt, SIZE size, DRAWSTATEPROC lpDrawProc, TMLParam lData, TMUInt nFlags, TMHBrush hBrush = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::DrawState(m_hDC, hBrush, lpDrawProc, lData, 0, pt.x, pt.y, size.cx, size.cy, nFlags | DST_COMPLEX);
    }
#endif // !_WIN32_WCE

// Ellipse and Polygon Functions
#ifndef _WIN32_WCE
    TMBool Chord(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
    {
        DbgAssert(m_hDC != NULL);
        return ::Chord(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4);
    }

    TMBool Chord(LPCRECT lpRect, POINT ptStart, POINT ptEnd)
    {
        DbgAssert(m_hDC != NULL);
        return ::Chord(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
    }
#endif // !_WIN32_WCE
*/
    void DrawFocusRect(const TMRect* lpRect)
    {
        DbgAssert(m_hDC != NULL);
        QPen pen(Qt::blue, 1, Qt::DotLine);
        QPen penOld(m_hDC->pen());
        m_hDC->setPen(pen);
        m_hDC->drawRect(*lpRect);
        m_hDC->setPen(penOld);
    }

    /*
    TMBool Ellipse(int x1, int y1, int x2, int y2)
    {
        DbgAssert(m_hDC != NULL);
        return ::Ellipse(m_hDC, x1, y1, x2, y2);
    }

    TMBool Ellipse(LPCRECT lpRect)
    {
        DbgAssert(m_hDC != NULL);
        return ::Ellipse(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
    }

#ifndef _WIN32_WCE
    TMBool Pie(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
    {
        DbgAssert(m_hDC != NULL);
        return ::Pie(m_hDC, x1, y1, x2, y2, x3, y3, x4, y4);
    }

    TMBool Pie(LPCRECT lpRect, POINT ptStart, POINT ptEnd)
    {
        DbgAssert(m_hDC != NULL);
        return ::Pie(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
    }
#endif // !_WIN32_WCE

    TMBool Polygon(const POINT* lpPoints, int nCount)
    {
        DbgAssert(m_hDC != NULL);
        return ::Polygon(m_hDC, lpPoints, nCount);
    }

#ifndef _WIN32_WCE
    TMBool PolyPolygon(const POINT* lpPoints, const INT* lpPolyCounts, int nCount)
    {
        DbgAssert(m_hDC != NULL);
        return ::PolyPolygon(m_hDC, lpPoints, lpPolyCounts, nCount);
    }
#endif // !_WIN32_WCE

    TMBool Rectangle(int x1, int y1, int x2, int y2)
    {
        DbgAssert(m_hDC != NULL);
        return ::Rectangle(m_hDC, x1, y1, x2, y2);
    }

    TMBool Rectangle(LPCRECT lpRect)
    {
        DbgAssert(m_hDC != NULL);
        return ::Rectangle(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
    }

    TMBool RoundRect(int x1, int y1, int x2, int y2, int x3, int y3)
    {
        DbgAssert(m_hDC != NULL);
        return ::RoundRect(m_hDC, x1, y1, x2, y2, x3, y3);
    }

    TMBool RoundRect(LPCRECT lpRect, POINT point)
    {
        DbgAssert(m_hDC != NULL);
        return ::RoundRect(m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, point.x, point.y);
    }

// Bitmap Functions
    TMBool PatBlt(int x, int y, int nWidth, int nHeight, TMUInt32 dwRop)
    {
        DbgAssert(m_hDC != NULL);
        return ::PatBlt(m_hDC, x, y, nWidth, nHeight, dwRop);
    }
    */


    TMBool _StretchBlt(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, TMUInt32 dwRop)
    {
        DbgAssert(this->m_hDC != NULL);
        QPaintDevice *pd = hSrcDC->device();
        switch (pd->devType())
        {
        case QInternal::Pixmap:
            m_hDC->drawPixmap(QRect(x, y, nWidth, nHeight), *static_cast<QPixmap *>(pd), QRect(xSrc, ySrc, nSrcWidth, nSrcHeight));
            return true;
        case QInternal::Image:
            m_hDC->drawImage(QRect(x, y, nWidth, nHeight), *static_cast<QImage *>(pd), QRect(xSrc, ySrc, nSrcWidth, nSrcHeight));
            return true;
        default:
            DbgAssert(0);
            break;
        }
        return false;
    }

    TMBool BitBlt(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, int xSrc, int ySrc, TMUInt32 dwRop)
    {
        DbgAssert(this->m_hDC != NULL);
        return StretchBlt(x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, nWidth, nHeight, dwRop);
    }

    TMBool StretchBlt(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, int xSrc, int ySrc, int wSrc, int hSrc, TMUInt32 dwRop)
    {
        DbgAssert(this->m_hDC != NULL);
        CAutoCompositionMode mode(this->m_hDC, QPainter::CompositionMode_Source);
        return _StretchBlt(x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, wSrc, hSrc, dwRop);
    }

    TMBool Blt24To32(
        int xDst, int yDst, int wDst, int hDst, TMHDC hImg,
        int xSrc, int ySrc, int wSrc, int hSrc, DWORD dwRop = 0) // dwRop: CAPTUREBLT, NOMIRRORBITMAP
    {
        return StretchBlt(xDst, yDst, wDst, hDst, hImg, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
    }


    TMBool AlphaBlend(
        int x, int y, int nWidth, int nHeight, TMHDC hImg,
        int xSrc, int ySrc, int wSrc, int hSrc, TMUInt8 AlphaFormat, TMUInt8 alpha)
    {
        DbgAssert(this->m_hDC != NULL);
        DbgAssert(hImg);

        CAutoOpacity opacity(this->m_hDC, (qreal)alpha / 255.0);
        if (AlphaFormat == AC_SRC_ALPHA)
            return _StretchBlt(x, y, nWidth, nHeight, hImg, xSrc, ySrc, wSrc, hSrc, 0);
        return StretchBlt(x, y, nWidth, nHeight, hImg, xSrc, ySrc, wSrc, hSrc, 0);
    }

    /*
    COLORREF GetPixel(int x, int y) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetPixel(m_hDC, x, y);
    }

    COLORREF GetPixel(POINT point) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetPixel(m_hDC, point.x, point.y);
    }

    COLORREF SetPixel(int x, int y, COLORREF crColor)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetPixel(m_hDC, x, y, crColor);
    }

    COLORREF SetPixel(POINT point, COLORREF crColor)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetPixel(m_hDC, point.x, point.y, crColor);
    }

#ifndef _WIN32_WCE
    TMBool FloodFill(int x, int y, COLORREF crColor)
    {
        DbgAssert(m_hDC != NULL);
        return ::FloodFill(m_hDC, x, y, crColor);
    }

    TMBool ExtFloodFill(int x, int y, COLORREF crColor, TMUInt nFillType)
    {
        DbgAssert(m_hDC != NULL);
        return ::ExtFloodFill(m_hDC, x, y, crColor, nFillType);
    }
#endif // !_WIN32_WCE

    TMBool MaskBlt(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, int xSrc, int ySrc, HBITMAP hMaskBitmap, int xMask, int yMask, TMUInt32 dwRop)
    {
        DbgAssert(m_hDC != NULL);
        return ::MaskBlt(m_hDC, x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, hMaskBitmap, xMask, yMask, dwRop);
    }

#ifndef _WIN32_WCE
    TMBool PlgBlt(LPPOINT lpPoint, TMHDC hSrcDC, int xSrc, int ySrc, int nWidth, int nHeight, HBITMAP hMaskBitmap, int xMask, int yMask)
    {
        DbgAssert(m_hDC != NULL);
        return ::PlgBlt(m_hDC, lpPoint, hSrcDC, xSrc, ySrc, nWidth, nHeight, hMaskBitmap, xMask, yMask);
    }

    TMBool SetPixelV(int x, int y, COLORREF crColor)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetPixelV(m_hDC, x, y, crColor);
    }

    TMBool SetPixelV(POINT point, COLORREF crColor)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetPixelV(m_hDC, point.x, point.y, crColor);
    }
#endif // !_WIN32_WCE

#if !defined(_ATL_NO_MSIMG) || defined(_WIN32_WCE)
#ifndef _WIN32_WCE
    TMBool TransparentBlt(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, TMUInt crTransparent)
    {
        DbgAssert(m_hDC != NULL);
        return ::TransparentBlt(m_hDC, x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, nSrcWidth, nSrcHeight, crTransparent);
    }
#else // CE specific
    TMBool TransparentImage(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, TMUInt crTransparent)
    {
        DbgAssert(m_hDC != NULL);
        return ::TransparentImage(m_hDC, x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, nSrcWidth, nSrcHeight, crTransparent);
    }
#endif // _WIN32_WCE

#if (!defined(_WIN32_WCE) || (_WIN32_WCE >= 420))
    TMBool GradientFill(const PTRIVERTEX pVertices, TMUInt32 nVertices, void* pMeshElements, TMUInt32 nMeshElements, TMUInt32 dwMode)
    {
        DbgAssert(m_hDC != NULL);
        return ::GradientFill(m_hDC, pVertices, nVertices, pMeshElements, nMeshElements, dwMode);
    }

    TMBool GradientFillRect(RECT& rect, COLORREF clr1, COLORREF clr2, bool bHorizontal)
    {
        DbgAssert(m_hDC != NULL);

        TRIVERTEX arrTvx[2] = { { 0 }, { 0 } };

        arrTvx[0].x = rect.left;
        arrTvx[0].y = rect.top;
        arrTvx[0].Red = MAKEWORD(0, GetRValue(clr1));
        arrTvx[0].Green = MAKEWORD(0, GetGValue(clr1));
        arrTvx[0].Blue = MAKEWORD(0, GetBValue(clr1));
        arrTvx[0].Alpha = 0;

        arrTvx[1].x = rect.right;
        arrTvx[1].y = rect.bottom;
        arrTvx[1].Red = MAKEWORD(0, GetRValue(clr2));
        arrTvx[1].Green = MAKEWORD(0, GetGValue(clr2));
        arrTvx[1].Blue = MAKEWORD(0, GetBValue(clr2));
        arrTvx[1].Alpha = 0;

        GRADIENT_RECT gr = { 0, 1 };

        return ::GradientFill(m_hDC, arrTvx, 2, &gr, 1, bHorizontal ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V);
    }
#endif // !defined(_WIN32_WCE) || (_WIN32_WCE >= 420)

#if !defined(_WIN32_WCE) || (_WIN32_WCE > 0x500)
    TMBool AlphaBlend(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, BLENDFUNCTION bf)
    {
        DbgAssert(m_hDC != NULL);
        return ::AlphaBlend(m_hDC, x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, nSrcWidth, nSrcHeight, bf);
    }
#endif // !defined(_WIN32_WCE) || (_WIN32_WCE > 0x500)
#endif //  !defined(_ATL_NO_MSIMG) || defined(_WIN32_WCE)

// Extra bitmap functions
    // Helper function for painting a disabled toolbar or menu bitmap
    // This function can take either an HBITMAP (for SS) or a DC with 
    //           the bitmap already painted (for cmdbar)
    TMBool DitherBlt(int x, int y, int nWidth, int nHeight, TMHDC hSrcDC, HBITMAP hBitmap, int xSrc, int ySrc,
            TMHBrush hBrushBackground = ::GetSysColorBrush(COLOR_3DFACE),
            TMHBrush hBrush3DEffect = ::GetSysColorBrush(COLOR_3DHILIGHT),
            TMHBrush hBrushDisabledImage = ::GetSysColorBrush(COLOR_3DSHADOW))
    {
        DbgAssert(m_hDC != NULL || hBitmap != NULL);
        DbgAssert(nWidth > 0 && nHeight > 0);
        
        // Create a generic DC for all BitBlts
        CDCHandle dc = (hSrcDC != NULL) ? hSrcDC : ::CreateCompatibleDC(m_hDC);
        DbgAssert(dc.m_hDC != NULL);
        if (dc.m_hDC == NULL)
            return FALSE;
        
        // Create a DC for the monochrome DIB section
        CDC dcBW = ::CreateCompatibleDC(m_hDC);
        DbgAssert(dcBW.m_hDC != NULL);
        if (dcBW.m_hDC == NULL)
        {
            if (hSrcDC == NULL)
                dc.DeleteDC();
            return FALSE;
        }

        // Create the monochrome DIB section with a black and white palette
        struct RGBBWBITMAPINFO
        {
            BITMAPINFOHEADER bmiHeader; 
            RGBQUAD bmiColors[2]; 
        };

        RGBBWBITMAPINFO rgbBWBitmapInfo = 
        {
            { sizeof(BITMAPINFOHEADER), nWidth, nHeight, 1, 1, BI_RGB, 0, 0, 0, 0, 0 },
            { { 0x00, 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x00 } }
        };

        void* pbitsBW;
        CBitmap bmpBW = ::CreateDIBSection(dcBW, (LPBITMAPINFO)&rgbBWBitmapInfo, DIB_RGB_COLORS, &pbitsBW, NULL, 0);
        DbgAssert(bmpBW.m_hBitmap != NULL);
        if (bmpBW.m_hBitmap == NULL)
        {
            if (hSrcDC == NULL)
                dc.DeleteDC();
            return FALSE;
        }
        
        // Attach the monochrome DIB section and the bitmap to the DCs
        HBITMAP hbmOldBW = dcBW.SelectBitmap(bmpBW);
        HBITMAP hbmOldDC = NULL;
        if (hBitmap != NULL)
            hbmOldDC = dc.SelectBitmap(hBitmap);

        // Block: Dark gray removal: we want (128, 128, 128) pixels to become black and not white
        {
            CDC dcTemp1 = ::CreateCompatibleDC(m_hDC);
            CDC dcTemp2 = ::CreateCompatibleDC(m_hDC);
            CBitmap bmpTemp1;
            bmpTemp1.CreateCompatibleBitmap(dc, nWidth, nHeight);
            CBitmap bmpTemp2;
            bmpTemp2.CreateBitmap(nWidth, nHeight, 1, 1, NULL);
            HBITMAP hOldBmp1 = dcTemp1.SelectBitmap(bmpTemp1);
            HBITMAP hOldBmp2 = dcTemp2.SelectBitmap(bmpTemp2);
            // Let's copy our image, it will be altered
            dcTemp1.BitBlt(0, 0, nWidth, nHeight, dc, xSrc, ySrc, SRCCOPY);

            // All dark gray pixels will become white, the others black
            dcTemp1.SetBkColor(RGB(128, 128, 128));
            dcTemp2.BitBlt(0, 0, nWidth, nHeight, dcTemp1, 0, 0, SRCCOPY);
            // Do an XOR to set to black these white pixels
            dcTemp1.BitBlt(0, 0, nWidth, nHeight, dcTemp2, 0, 0, SRCINVERT);

            // BitBlt the bitmap into the monochrome DIB section
            // The DIB section will do a true monochrome conversion
            // The magenta background being closer to white will become white
            dcBW.BitBlt(0, 0, nWidth, nHeight, dcTemp1, 0, 0, SRCCOPY);

            // Cleanup
            dcTemp1.SelectBitmap(hOldBmp1);
            dcTemp2.SelectBitmap(hOldBmp2);
        }
        
        // Paint the destination rectangle using hBrushBackground
        if (hBrushBackground != NULL)
        {
            RECT rc = { x, y, x + nWidth, y + nHeight };
            FillRect(&rc, hBrushBackground);
        }

        // BitBlt the black bits in the monochrome bitmap into hBrush3DEffect color in the destination DC
        // The magic ROP comes from the Charles Petzold's book
        TMHBrush hOldBrush = SelectBrush(hBrush3DEffect);
        BitBlt(x + 1, y + 1, nWidth, nHeight, dcBW, 0, 0, 0xB8074A);

        // BitBlt the black bits in the monochrome bitmap into hBrushDisabledImage color in the destination DC
        SelectBrush(hBrushDisabledImage);
        BitBlt(x, y, nWidth, nHeight, dcBW, 0, 0, 0xB8074A);

        SelectBrush(hOldBrush);
        dcBW.SelectBitmap(hbmOldBW);
        dc.SelectBitmap(hbmOldDC);

        if (hSrcDC == NULL)
            dc.DeleteDC();

        return TRUE;
    }

// Text Functions
#ifndef _WIN32_WCE
    TMBool TextOut(int x, int y, TMCStr lpszString, int nCount = -1)
    {
        DbgAssert(m_hDC != NULL);
        if (nCount == -1)
            nCount = lstrlen(lpszString);
        return ::TextOut(m_hDC, x, y, lpszString, nCount);
    }
#endif // !_WIN32_WCE

    TMBool ExtTextOut(int x, int y, TMUInt nOptions, LPCRECT lpRect, TMCStr lpszString, TMUInt nCount = -1, LPINT lpDxWidths = NULL)
    {
        DbgAssert(m_hDC != NULL);
        if (nCount == -1)
            nCount = lstrlen(lpszString);
        return ::ExtTextOut(m_hDC, x, y, nOptions, lpRect, lpszString, nCount, lpDxWidths);
    }

#ifndef _WIN32_WCE
    SIZE TabbedTextOut(int x, int y, TMCStr lpszString, int nCount = -1, int nTabPositions = 0, LPINT lpnTabStopPositions = NULL, int nTabOrigin = 0)
    {
        DbgAssert(m_hDC != NULL);
        if (nCount == -1)
            nCount = lstrlen(lpszString);
        LONG lRes = ::TabbedTextOut(m_hDC, x, y, lpszString, nCount, nTabPositions, lpnTabStopPositions, nTabOrigin);
        SIZE size = { GET_X_LPARAM(lRes), GET_Y_LPARAM(lRes) };
        return size;
    }
#endif // !_WIN32_WCE
*/
    int DrawText(TMCStr lpstrText, int cchText, TMRect* lpRect, TMUInt uFormat)
    {
        DbgAssert(m_hDC != NULL);
        DbgAssert((uFormat & DT_MODIFYSTRING) == 0);
        DbgAssert(lpstrText);

        int nFlag = 0;
        if (uFormat & DT_SINGLELINE)
            nFlag |= Qt::TextSingleLine;
        if (uFormat & DT_WORDBREAK)
            nFlag |= Qt::TextWordWrap;
        if (uFormat & DT_RIGHT)
            nFlag |= Qt::TextForceRightToLeft;
        if (!(uFormat & DT_HIDEPREFIX))
            nFlag |= Qt::TextShowMnemonic;

        QString string(TM_T2Q(lpstrText, cchText));
        if (uFormat & DT_CALCRECT)
        {
            QFontMetrics objMetrics(m_hDC->fontMetrics());
            if (nFlag & DT_END_ELLIPSIS)
                string = objMetrics.elidedText(string, Qt::ElideRight, lpRect->width(), nFlag);

            *lpRect = objMetrics.boundingRect(*lpRect, nFlag, string);
            return true;
        }

        if (nFlag & DT_END_ELLIPSIS)
            string = m_hDC->fontMetrics().elidedText(string, Qt::ElideRight, lpRect->width(), nFlag);
        m_hDC->drawText(*lpRect, nFlag, string);
        return true;

        //return ::DrawText(m_hDC, lpstrText, cchText, lpRect, uFormat);
    }
    /*
    int DrawText(LPTSTR lpstrText, int cchText, LPRECT lpRect, TMUInt uFormat)
    {
        DbgAssert(m_hDC != NULL);
        return ::DrawText(m_hDC, lpstrText, cchText, lpRect, uFormat);
    }

#ifndef _WIN32_WCE
    int DrawTextEx(LPTSTR lpstrText, int cchText, LPRECT lpRect, TMUInt uFormat, LPDRAWTEXTPARAMS lpDTParams = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::DrawTextEx(m_hDC, lpstrText, cchText, lpRect, uFormat, lpDTParams);
    }
#endif // !_WIN32_WCE

#if (_WIN32_WINNT >= 0x0501)
    int DrawShadowText(LPCWSTR lpstrText, int cchText, LPRECT lpRect, TMUInt32 dwFlags, COLORREF clrText, COLORREF clrShadow, int xOffset, int yOffset)
    {
        DbgAssert(m_hDC != NULL);
        // This function is present only if comctl32.dll version 6 is loaded;
        // we use LoadLibrary/GetProcAddress to allow apps compiled with
        // _WIN32_WINNT >= 0x0501 to run on older Windows/CommCtrl
        int nRet = 0;
        HMODULE hCommCtrlDLL = ::LoadLibrary(_T("comctl32.dll"));
        DbgAssert(hCommCtrlDLL != NULL);
        if (hCommCtrlDLL != NULL)
        {
            typedef int (WINAPI *PFN_DrawShadowText)(TMHDC hDC, LPCWSTR lpstrText, TMUInt cchText, LPRECT lpRect, TMUInt32 dwFlags, COLORREF clrText, COLORREF clrShadow, int xOffset, int yOffset);
            PFN_DrawShadowText pfnDrawShadowText = (PFN_DrawShadowText)::GetProcAddress(hCommCtrlDLL, "DrawShadowText");
            DbgAssert(pfnDrawShadowText != NULL);   // this function requires CommCtrl6
            if (pfnDrawShadowText != NULL)
                nRet = pfnDrawShadowText(m_hDC, lpstrText, cchText, lpRect, dwFlags, clrText, clrShadow, xOffset, yOffset);
            ::FreeLibrary(hCommCtrlDLL);
        }
        return nRet;
    }
#endif // (_WIN32_WINNT >= 0x0501)

    TMBool GetTextExtent(TMCStr lpszString, int nCount, LPSIZE lpSize) const
    {
        DbgAssert(m_hDC != NULL);
        if (nCount == -1)
            nCount = lstrlen(lpszString);
        return ::GetTextExtentPoint32(m_hDC, lpszString, nCount, lpSize);
    }

    TMBool GetTextExtentExPoint(TMCStr lpszString, int cchString, LPSIZE lpSize, int nMaxExtent, LPINT lpnFit = NULL, LPINT alpDx = NULL)
    {
        DbgAssert(m_hDC != NULL);
        return ::GetTextExtentExPoint(m_hDC, lpszString, cchString, nMaxExtent, lpnFit, alpDx, lpSize);
    }

#ifndef _WIN32_WCE
    TMUInt32 GetTabbedTextExtent(TMCStr lpszString, int nCount = -1, int nTabPositions = 0, LPINT lpnTabStopPositions = NULL) const
    {
        DbgAssert(m_hDC != NULL);
        if (nCount == -1)
            nCount = lstrlen(lpszString);
        return ::GetTabbedTextExtent(m_hDC, lpszString, nCount, nTabPositions, lpnTabStopPositions);
    }

    TMBool GrayString(TMHBrush hBrush, TMBool (CALLBACK* lpfnOutput)(TMHDC, TMLParam, int), TMLParam lpData, int nCount, int x, int y, int nWidth, int nHeight)
    {
        DbgAssert(m_hDC != NULL);
        return ::GrayString(m_hDC, hBrush, (GRAYSTRINGPROC)lpfnOutput, lpData, nCount, x, y, nWidth, nHeight);
    }
#endif // !_WIN32_WCE

#if !defined(_WIN32_WCE) || (_WIN32_WCE >= 400)
    TMUInt GetTextAlign() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetTextAlign(m_hDC);
    }

    TMUInt SetTextAlign(TMUInt nFlags)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetTextAlign(m_hDC, nFlags);
    }
#endif // !defined(_WIN32_WCE) || (_WIN32_WCE >= 400)

    int GetTextFace(LPTSTR lpszFacename, int nCount) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetTextFace(m_hDC, nCount, lpszFacename);
    }

    int GetTextFaceLen() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetTextFace(m_hDC, 0, NULL);
    }

#ifndef _ATL_NO_COM
#ifdef _OLEAUTO_H_
    TMBool GetTextFace(BSTR& bstrFace) const
    {
        USES_CONVERSION;
        DbgAssert(m_hDC != NULL);
        DbgAssert(bstrFace == NULL);

        int nLen = GetTextFaceLen();
        if (nLen == 0)
            return FALSE;

        CTempBuffer<TCHAR, _WTL_STACK_ALLOC_THRESHOLD> buff;
        LPTSTR lpszText = buff.Allocate(nLen);
        if (lpszText == NULL)
            return FALSE;

        if (!GetTextFace(lpszText, nLen))
            return FALSE;

        bstrFace = ::SysAllocString(T2OLE(lpszText));
        return (bstrFace != NULL) ? TRUE : FALSE;
    }
#endif
#endif // !_ATL_NO_COM

#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
    int GetTextFace(_CSTRING_NS::CString& strFace) const
    {
        DbgAssert(m_hDC != NULL);

        int nLen = GetTextFaceLen();
        if (nLen == 0)
            return 0;

        LPTSTR lpstr = strFace.GetBufferSetLength(nLen);
        if (lpstr == NULL)
            return 0;
        int nRet = GetTextFace(lpstr, nLen);
        strFace.ReleaseBuffer();
        return nRet;
    }
#endif // defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)

    TMBool GetTextMetrics(LPTEXTMETRIC lpMetrics) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetTextMetrics(m_hDC, lpMetrics);
    }

#ifndef _WIN32_WCE
    int SetTextJustification(int nBreakExtra, int nBreakCount)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetTextJustification(m_hDC, nBreakExtra, nBreakCount);
    }

    int GetTextCharacterExtra() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetTextCharacterExtra(m_hDC);
    }

    int SetTextCharacterExtra(int nCharExtra)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetTextCharacterExtra(m_hDC, nCharExtra);
    }
#endif // !_WIN32_WCE

// Advanced Drawing
    TMBool DrawEdge(LPRECT lpRect, TMUInt nEdge, TMUInt nFlags)
    {
        DbgAssert(m_hDC != NULL);
        return ::DrawEdge(m_hDC, lpRect, nEdge, nFlags);
    }

    TMBool DrawFrameControl(LPRECT lpRect, TMUInt nType, TMUInt nState)
    {
        DbgAssert(m_hDC != NULL);
        return ::DrawFrameControl(m_hDC, lpRect, nType, nState);
    }

// Scrolling Functions
    TMBool ScrollDC(int dx, int dy, LPCRECT lpRectScroll, LPCRECT lpRectClip, HRGN hRgnUpdate, LPRECT lpRectUpdate)
    {
        DbgAssert(m_hDC != NULL);
        return ::ScrollDC(m_hDC, dx, dy, lpRectScroll, lpRectClip, hRgnUpdate, lpRectUpdate);
    }

// Font Functions
#ifndef _WIN32_WCE
    TMBool GetCharWidth(TMUInt nFirstChar, TMUInt nLastChar, LPINT lpBuffer) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetCharWidth(m_hDC, nFirstChar, nLastChar, lpBuffer);
    }

    // GetCharWidth32 is not supported under Win9x
    TMBool GetCharWidth32(TMUInt nFirstChar, TMUInt nLastChar, LPINT lpBuffer) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetCharWidth32(m_hDC, nFirstChar, nLastChar, lpBuffer);
    }

    TMUInt32 SetMapperFlags(TMUInt32 dwFlag)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetMapperFlags(m_hDC, dwFlag);
    }

    TMBool GetAspectRatioFilter(LPSIZE lpSize) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetAspectRatioFilterEx(m_hDC, lpSize);
    }

    TMBool GetCharABCWidths(TMUInt nFirstChar, TMUInt nLastChar, LPABC lpabc) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetCharABCWidths(m_hDC, nFirstChar, nLastChar, lpabc);
    }

    TMUInt32 GetFontData(TMUInt32 dwTable, TMUInt32 dwOffset, void* lpData, TMUInt32 cbData) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetFontData(m_hDC, dwTable, dwOffset, lpData, cbData);
    }

    int GetKerningPairs(int nPairs, LPKERNINGPAIR lpkrnpair) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetKerningPairs(m_hDC, nPairs, lpkrnpair);
    }

    TMUInt GetOutlineTextMetrics(TMUInt cbData, LPOUTLINETEXTMETRIC lpotm) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetOutlineTextMetrics(m_hDC, cbData, lpotm);
    }

    TMUInt32 GetGlyphOutline(TMUInt nChar, TMUInt nFormat, LPGLYPHMETRICS lpgm, TMUInt32 cbBuffer, void* lpBuffer, const MAT2* lpmat2) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetGlyphOutline(m_hDC, nChar, nFormat, lpgm, cbBuffer, lpBuffer, lpmat2);
    }

    TMBool GetCharABCWidths(TMUInt nFirstChar, TMUInt nLastChar, LPABCFLOAT lpABCF) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetCharABCWidthsFloat(m_hDC, nFirstChar, nLastChar, lpABCF);
    }

    TMBool GetCharWidth(TMUInt nFirstChar, TMUInt nLastChar, float* lpFloatBuffer) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetCharWidthFloat(m_hDC, nFirstChar, nLastChar, lpFloatBuffer);
    }
#endif // !_WIN32_WCE

// Printer/Device Escape Functions
#ifndef _WIN32_WCE
    int Escape(int nEscape, int nCount, LPCSTR lpszInData, void* lpOutData)
    {
        DbgAssert(m_hDC != NULL);
        return ::Escape(m_hDC, nEscape, nCount, lpszInData, lpOutData);
    }
#endif // !_WIN32_WCE

    int Escape(int nEscape, int nInputSize, LPCSTR lpszInputData,
        int nOutputSize, LPSTR lpszOutputData)
    {
        DbgAssert(m_hDC != NULL);
        return ::ExtEscape(m_hDC, nEscape, nInputSize, lpszInputData, nOutputSize, lpszOutputData);
    }

#ifndef _WIN32_WCE
    int DrawEscape(int nEscape, int nInputSize, LPCSTR lpszInputData)
    {
        DbgAssert(m_hDC != NULL);
        return ::DrawEscape(m_hDC, nEscape, nInputSize, lpszInputData);
    }
#endif // !_WIN32_WCE

    // Escape helpers
#if !defined(_WIN32_WCE) || ((_WIN32_WCE >= 200) && defined(StartDoc))
    int StartDoc(TMCStr lpszDocName)  // old Win3.0 version
    {
        DOCINFO di = { 0 };
        di.cbSize = sizeof(DOCINFO);
        di.lpszDocName = lpszDocName;
        return StartDoc(&di);
    }

    int StartDoc(LPDOCINFO lpDocInfo)
    {
        DbgAssert(m_hDC != NULL);
        return ::StartDoc(m_hDC, lpDocInfo);
    }

    int StartPage()
    {
        DbgAssert(m_hDC != NULL);
        return ::StartPage(m_hDC);
    }

    int EndPage()
    {
        DbgAssert(m_hDC != NULL);
        return ::EndPage(m_hDC);
    }

    int SetAbortProc(TMBool (CALLBACK* lpfn)(TMHDC, int))
    {
        DbgAssert(m_hDC != NULL);
        return ::SetAbortProc(m_hDC, (ABORTPROC)lpfn);
    }

    int AbortDoc()
    {
        DbgAssert(m_hDC != NULL);
        return ::AbortDoc(m_hDC);
    }

    int EndDoc()
    {
        DbgAssert(m_hDC != NULL);
        return ::EndDoc(m_hDC);
    }
#endif // !defined(_WIN32_WCE) || ((_WIN32_WCE >= 200) && defined(StartDoc))

// MetaFile Functions
#ifndef _WIN32_WCE
    TMBool PlayMetaFile(HMETAFILE hMF)
    {
        DbgAssert(m_hDC != NULL);
        if (::GetDeviceCaps(m_hDC, TECHNOLOGY) == DT_METAFILE)
        {
            // playing metafile in metafile, just use core windows API
            return ::PlayMetaFile(m_hDC, hMF);
        }

        // for special playback, lParam == pDC
        return ::EnumMetaFile(m_hDC, hMF, EnumMetaFileProc, (TMLParam)this);
    }

    TMBool PlayMetaFile(HENHMETAFILE hEnhMetaFile, LPCRECT lpBounds)
    {
        DbgAssert(m_hDC != NULL);
        return ::PlayEnhMetaFile(m_hDC, hEnhMetaFile, lpBounds);
    }

    TMBool AddMetaFileComment(TMUInt nDataSize, const TMUInt8* pCommentData) // can be used for enhanced metafiles only
    {
        DbgAssert(m_hDC != NULL);
        return ::GdiComment(m_hDC, nDataSize, pCommentData);
    }

    // Special handling for metafile playback
    static int CALLBACK EnumMetaFileProc(TMHDC hDC, HANDLETABLE* pHandleTable, METARECORD* pMetaRec, int nHandles, TMLParam lParam)
    {
        CDCHandle* pDC = (CDCHandle*)lParam;

        switch (pMetaRec->rdFunction)
        {
        case META_SETMAPMODE:
            pDC->SetMapMode((int)(short)pMetaRec->rdParm[0]);
            break;
        case META_SETWINDOWEXT:
            pDC->SetWindowExt((int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
            break;
        case META_SETWINDOWORG:
            pDC->SetWindowOrg((int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
            break;
        case META_SETVIEWPORTEXT:
            pDC->SetViewportExt((int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
            break;
        case META_SETVIEWPORTORG:
            pDC->SetViewportOrg((int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
            break;
        case META_SCALEWINDOWEXT:
            pDC->ScaleWindowExt((int)(short)pMetaRec->rdParm[3], (int)(short)pMetaRec->rdParm[2], 
                (int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
            break;
        case META_SCALEVIEWPORTEXT:
            pDC->ScaleViewportExt((int)(short)pMetaRec->rdParm[3], (int)(short)pMetaRec->rdParm[2],
                (int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
            break;
        case META_OFFSETVIEWPORTORG:
            pDC->OffsetViewportOrg((int)(short)pMetaRec->rdParm[1], (int)(short)pMetaRec->rdParm[0]);
            break;
        case META_SAVEDC:
            pDC->SaveDC();
            break;
        case META_RESTOREDC:
            pDC->RestoreDC((int)(short)pMetaRec->rdParm[0]);
            break;
        case META_SETBKCOLOR:
            pDC->SetBkColor(*(UNALIGNED COLORREF*)&pMetaRec->rdParm[0]);
            break;
        case META_SETTEXTCOLOR:
            pDC->SetTextColor(*(UNALIGNED COLORREF*)&pMetaRec->rdParm[0]);
            break;

        // need to watch out for SelectObject(HFONT), for custom font mapping
        case META_SELECTOBJECT:
            {
                HGDIOBJ hObject = pHandleTable->objectHandle[pMetaRec->rdParm[0]];
                TMUInt nObjType = ::GetObjectType(hObject);
                if (nObjType == 0)
                {
                    // object type is unknown, determine if it is a font
                    HFONT hStockFont = (HFONT)::GetStockObject(SYSTEM_FONT);
                    HFONT hFontOld = (HFONT)::SelectObject(pDC->m_hDC, hStockFont);
                    HGDIOBJ hObjOld = ::SelectObject(pDC->m_hDC, hObject);
                    if (hObjOld == hStockFont)
                    {
                        // got the stock object back, so must be selecting a font
                        pDC->SelectFont((HFONT)hObject);
                        break;  // don't play the default record
                    }
                    else
                    {
                        // didn't get the stock object back, so restore everything
                        ::SelectObject(pDC->m_hDC, hFontOld);
                        ::SelectObject(pDC->m_hDC, hObjOld);
                    }
                    // and fall through to PlayMetaFileRecord...
                }
                else if (nObjType == OBJ_FONT)
                {
                    // play back as CDCHandle::SelectFont(HFONT)
                    pDC->SelectFont((HFONT)hObject);
                    break;  // don't play the default record
                }
            }
            // fall through...

        default:
            ::PlayMetaFileRecord(hDC, pHandleTable, pMetaRec, nHandles);
            break;
        }

        return 1;
    }
#endif // !_WIN32_WCE

// Path Functions
#ifndef _WIN32_WCE
    TMBool AbortPath()
    {
        DbgAssert(m_hDC != NULL);
        return ::AbortPath(m_hDC);
    }

    TMBool BeginPath()
    {
        DbgAssert(m_hDC != NULL);
        return ::BeginPath(m_hDC);
    }

    TMBool CloseFigure()
    {
        DbgAssert(m_hDC != NULL);
        return ::CloseFigure(m_hDC);
    }

    TMBool EndPath()
    {
        DbgAssert(m_hDC != NULL);
        return ::EndPath(m_hDC);
    }

    TMBool FillPath()
    {
        DbgAssert(m_hDC != NULL);
        return ::FillPath(m_hDC);
    }

    TMBool FlattenPath()
    {
        DbgAssert(m_hDC != NULL);
        return ::FlattenPath(m_hDC);
    }

    TMBool StrokeAndFillPath()
    {
        DbgAssert(m_hDC != NULL);
        return ::StrokeAndFillPath(m_hDC);
    }

    TMBool StrokePath()
    {
        DbgAssert(m_hDC != NULL);
        return ::StrokePath(m_hDC);
    }

    TMBool WidenPath()
    {
        DbgAssert(m_hDC != NULL);
        return ::WidenPath(m_hDC);
    }

    TMBool GetMiterLimit(PFLOAT pfMiterLimit) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetMiterLimit(m_hDC, pfMiterLimit);
    }

    TMBool SetMiterLimit(float fMiterLimit)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetMiterLimit(m_hDC, fMiterLimit, NULL);
    }

    int GetPath(LPPOINT lpPoints, LPBYTE lpTypes, int nCount) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetPath(m_hDC, lpPoints, lpTypes, nCount);
    }

    TMBool SelectClipPath(int nMode)
    {
        DbgAssert(m_hDC != NULL);
        return ::SelectClipPath(m_hDC, nMode);
    }
#endif // !_WIN32_WCE

// Misc Helper Functions
    static CBrushHandle PASCAL GetHalftoneBrush()
    {
        TMHBrush halftoneBrush = NULL;
        WORD grayPattern[8] = { 0 };
        for (int i = 0; i < 8; i++)
            grayPattern[i] = (WORD)(0x5555 << (i & 1));
        HBITMAP grayBitmap = CreateBitmap(8, 8, 1, 1, &grayPattern);
        if (grayBitmap != NULL)
        {
            halftoneBrush = ::CreatePatternBrush(grayBitmap);
            DeleteObject(grayBitmap);
        }
        return CBrushHandle(halftoneBrush);
    }

    void DrawDragRect(LPCRECT lpRect, SIZE size, LPCRECT lpRectLast, SIZE sizeLast, TMHBrush hBrush = NULL, TMHBrush hBrushLast = NULL)
    {
        // first, determine the update region and select it
        CRgn rgnOutside;
        rgnOutside.CreateRectRgnIndirect(lpRect);
        RECT rect = *lpRect;
        ::InflateRect(&rect, -size.cx, -size.cy);
        ::IntersectRect(&rect, &rect, lpRect);
        CRgn rgnInside;
        rgnInside.CreateRectRgnIndirect(&rect);
        CRgn rgnNew;
        rgnNew.CreateRectRgn(0, 0, 0, 0);
        rgnNew.CombineRgn(rgnOutside, rgnInside, RGN_XOR);

        TMHBrush hBrushOld = NULL;
        CBrush brushHalftone;
        if (hBrush == NULL)
            brushHalftone = hBrush = CDCHandle::GetHalftoneBrush();
        if (hBrushLast == NULL)
            hBrushLast = hBrush;

        CRgn rgnLast;
        CRgn rgnUpdate;
        if (lpRectLast != NULL)
        {
            // find difference between new region and old region
            rgnLast.CreateRectRgn(0, 0, 0, 0);
            rgnOutside.SetRectRgn(lpRectLast->left, lpRectLast->top, lpRectLast->right, lpRectLast->bottom);
            rect = *lpRectLast;
            ::InflateRect(&rect, -sizeLast.cx, -sizeLast.cy);
            ::IntersectRect(&rect, &rect, lpRectLast);
            rgnInside.SetRectRgn(rect.left, rect.top, rect.right, rect.bottom);
            rgnLast.CombineRgn(rgnOutside, rgnInside, RGN_XOR);

            // only diff them if brushes are the same
            if (hBrush == hBrushLast)
            {
                rgnUpdate.CreateRectRgn(0, 0, 0, 0);
                rgnUpdate.CombineRgn(rgnLast, rgnNew, RGN_XOR);
            }
        }
        if (hBrush != hBrushLast && lpRectLast != NULL)
        {
            // brushes are different -- erase old region first
            SelectClipRgn(rgnLast);
            GetClipBox(&rect);
            hBrushOld = SelectBrush(hBrushLast);
            PatBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);
            SelectBrush(hBrushOld);
            hBrushOld = NULL;
        }

        // draw into the update/new region
        SelectClipRgn(rgnUpdate.IsNull() ? rgnNew : rgnUpdate);
        GetClipBox(&rect);
        hBrushOld = SelectBrush(hBrush);
        PatBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);

        // cleanup DC
        if (hBrushOld != NULL)
            SelectBrush(hBrushOld);
        SelectClipRgn(NULL);
    }
    */
    void FillSolidRect(const TMRect* lpRect, TMColor clr)
    {
        DbgAssert(m_hDC != NULL);
        QColor qc(ColorToQColor(clr));
        m_hDC->fillRect(*lpRect, qc);
    }

    /*
    void FillSolidRect(int x, int y, int cx, int cy, COLORREF clr)
    {
        DbgAssert(m_hDC != NULL);

        RECT rect = { x, y, x + cx, y + cy };
        FillSolidRect(&rect, clr);
    }

    void Draw3dRect(LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
    {
        Draw3dRect(lpRect->left, lpRect->top, lpRect->right - lpRect->left,
            lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
    }

    void Draw3dRect(int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
    {
        FillSolidRect(x, y, cx - 1, 1, clrTopLeft);
        FillSolidRect(x, y, 1, cy - 1, clrTopLeft);
        FillSolidRect(x + cx, y, -1, cy, clrBottomRight);
        FillSolidRect(x, y + cy, cx, -1, clrBottomRight);
    }

// DIB support
#if !defined(_WIN32_WCE) || (_WIN32_WCE >= 410)
    int SetDIBitsToDevice(int x, int y, TMUInt32 dwWidth, TMUInt32 dwHeight, int xSrc, int ySrc, TMUInt uStartScan, TMUInt cScanLines, const void* lpvBits, const BITMAPINFO* lpbmi, TMUInt uColorUse)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetDIBitsToDevice(m_hDC, x, y, dwWidth, dwHeight, xSrc, ySrc, uStartScan, cScanLines, lpvBits, lpbmi, uColorUse);
    }
#endif // !defined(_WIN32_WCE) || (_WIN32_WCE >= 410)
*/
    int StretchDIBits(int x, int y, int nWidth, int nHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, const void* lpvBits, const BITMAPINFO* lpbmi, TMUInt uColorUse, TMUInt32 dwRop)
    {
        DbgAssert(m_hDC != NULL);
        QImage tmp_img((uchar *)lpvBits, lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight, gxBppFromQFormat(lpbmi->bmiHeader.biBitCount));
        m_hDC->drawImage(QRect(x,y,nWidth,nHeight), tmp_img, QRect(xSrc, ySrc, nSrcWidth, nSrcHeight));
        return true;
    }
/*
    TMUInt GetDIBColorTable(TMUInt uStartIndex, TMUInt cEntries, RGBQUAD* pColors) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetDIBColorTable(m_hDC, uStartIndex, cEntries, pColors);
    }

    TMUInt SetDIBColorTable(TMUInt uStartIndex, TMUInt cEntries, const RGBQUAD* pColors)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetDIBColorTable(m_hDC, uStartIndex, cEntries, pColors);
    }

// OpenGL support
#if !defined(_ATL_NO_OPENGL) && !defined(_WIN32_WCE)
    int ChoosePixelFormat(const PIXELFORMATDESCRIPTOR* ppfd)
    {
        DbgAssert(m_hDC != NULL);
        return ::ChoosePixelFormat(m_hDC, ppfd);
    }

    int DescribePixelFormat(int iPixelFormat, TMUInt nBytes, LPPIXELFORMATDESCRIPTOR ppfd)
    {
        DbgAssert(m_hDC != NULL);
        return ::DescribePixelFormat(m_hDC, iPixelFormat, nBytes, ppfd);
    }

    int GetPixelFormat() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetPixelFormat(m_hDC);
    }

    TMBool SetPixelFormat(int iPixelFormat, const PIXELFORMATDESCRIPTOR* ppfd)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetPixelFormat(m_hDC, iPixelFormat, ppfd);
    }

    TMBool SwapBuffers()
    {
        DbgAssert(m_hDC != NULL);
        return ::SwapBuffers(m_hDC);
    }

    HGLRC wglCreateContext()
    {
        DbgAssert(m_hDC != NULL);
        return ::wglCreateContext(m_hDC);
    }

    HGLRC wglCreateLayerContext(int iLayerPlane)
    {
        DbgAssert(m_hDC != NULL);
        return ::wglCreateLayerContext(m_hDC, iLayerPlane);
    }

    TMBool wglMakeCurrent(HGLRC hglrc)
    {
        DbgAssert(m_hDC != NULL);
        return ::wglMakeCurrent(m_hDC, hglrc);
    }

    TMBool wglUseFontBitmaps(TMUInt32 dwFirst, TMUInt32 dwCount, TMUInt32 listBase)
    {
        DbgAssert(m_hDC != NULL);
        return ::wglUseFontBitmaps(m_hDC, dwFirst, dwCount, listBase);
    }

    TMBool wglUseFontOutlines(TMUInt32 dwFirst, TMUInt32 dwCount, TMUInt32 listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf)
    {
        DbgAssert(m_hDC != NULL);
        return ::wglUseFontOutlines(m_hDC, dwFirst, dwCount, listBase, deviation, extrusion, format, lpgmf);
    }

    TMBool wglDescribeLayerPlane(int iPixelFormat, int iLayerPlane, TMUInt nBytes, LPLAYERPLANEDESCRIPTOR plpd)
    {
        DbgAssert(m_hDC != NULL);
        return ::wglDescribeLayerPlane(m_hDC, iPixelFormat, iLayerPlane, nBytes, plpd);
    }

    int wglSetLayerPaletteEntries(int iLayerPlane, int iStart, int cEntries, const COLORREF* pclr)
    {
        DbgAssert(m_hDC != NULL);
        return ::wglSetLayerPaletteEntries(m_hDC, iLayerPlane, iStart, cEntries, pclr);
    }

    int wglGetLayerPaletteEntries(int iLayerPlane, int iStart, int cEntries, COLORREF* pclr)
    {
        DbgAssert(m_hDC != NULL);
        return ::wglGetLayerPaletteEntries(m_hDC, iLayerPlane, iStart, cEntries, pclr);
    }

    TMBool wglRealizeLayerPalette(int iLayerPlane, TMBool bRealize)
    {
        DbgAssert(m_hDC != NULL);
        return ::wglRealizeLayerPalette(m_hDC, iLayerPlane, bRealize);
    }

    TMBool wglSwapLayerBuffers(TMUInt uPlanes)
    {
        DbgAssert(m_hDC != NULL);
        return ::wglSwapLayerBuffers(m_hDC, uPlanes);
    }
#endif // !defined(_ATL_NO_OPENGL) && !defined(_WIN32_WCE)

// New for Windows 2000 only
#if (_WIN32_WINNT >= 0x0500)
    COLORREF GetDCPenColor() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetDCPenColor(m_hDC);
    }

    COLORREF SetDCPenColor(COLORREF clr)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetDCPenColor(m_hDC, clr);
    }

    COLORREF GetDCBrushColor() const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetDCBrushColor(m_hDC);
    }

    COLORREF SetDCBrushColor(COLORREF clr)
    {
        DbgAssert(m_hDC != NULL);
        return ::SetDCBrushColor(m_hDC, clr);
    }

#ifndef _WIN32_WCE
    TMUInt32 GetFontUnicodeRanges(LPGLYPHSET lpgs) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetFontUnicodeRanges(m_hDC, lpgs);
    }
#endif // !_WIN32_WCE

    TMUInt32 GetGlyphIndices(TMCStr lpstr, int cch, LPWORD pgi, TMUInt32 dwFlags) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetGlyphIndices(m_hDC, lpstr, cch, pgi, dwFlags);
    }

    TMBool GetTextExtentPointI(LPWORD pgiIn, int cgi, LPSIZE lpSize) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetTextExtentPointI(m_hDC, pgiIn, cgi, lpSize);
    }

    TMBool GetTextExtentExPointI(LPWORD pgiIn, int cgi, int nMaxExtent, LPINT lpnFit, LPINT alpDx, LPSIZE lpSize) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetTextExtentExPointI(m_hDC, pgiIn, cgi, nMaxExtent, lpnFit, alpDx, lpSize);
    }

    TMBool GetCharWidthI(TMUInt giFirst, TMUInt cgi, LPWORD pgi, LPINT lpBuffer) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetCharWidthI(m_hDC, giFirst, cgi, pgi, lpBuffer);
    }

    TMBool GetCharABCWidthsI(TMUInt giFirst, TMUInt cgi, LPWORD pgi, LPABC lpabc) const
    {
        DbgAssert(m_hDC != NULL);
        return ::GetCharABCWidthsI(m_hDC, giFirst, cgi, pgi, lpabc);
    }
#endif // (_WIN32_WINNT >= 0x0500)

// New for Windows 2000 and Windows 98
#if (WINVER >= 0x0500) && !defined(_WIN32_WCE)
    TMBool ColorCorrectPalette(HPALETTE hPalette, TMUInt32 dwFirstEntry, TMUInt32 dwNumOfEntries)
    {
        DbgAssert(m_hDC != NULL);
        return ::ColorCorrectPalette(m_hDC, hPalette, dwFirstEntry, dwNumOfEntries);
    }
#endif // (WINVER >= 0x0500) && !defined(_WIN32_WCE)

*/
};

typedef CDCQtT<false>        CDCHandleQt;
typedef CDCQtT<true>         CDCQt;


///////////////////////////////////////////////////////////////////////////////
// CATScreenDC
struct CScreenDCWriteQt
    : public CDCQt
{
    struct QWindowHack : public QWindow
    {
    };

    QWindowHack* m_pWin;
    QBackingStore m_backingStore;
    CRect m_rc;

    CScreenDCWriteQt(const CRect& rc)
        : m_pWin(static_cast<QWindowHack*>(QWindow::fromWinId(QApplication::desktop()->winId())))
        , m_backingStore(m_pWin)
        , m_rc(rc)
    {
        m_backingStore.resize(rc.size());
        m_backingStore.beginPaint(rc);
        m_hDC = new QPainter(m_backingStore.paintDevice());
    }
    
    ~CScreenDCWriteQt()
    {
        delete m_hDC;
        m_backingStore.endPaint();
        m_backingStore.flush(m_rc);
        delete m_pWin;
    }
};

///////////////////////////////////////////////////////////////////////////////
// app
class CAppQt
{
public:
    static inline void SetDPIAware()
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5,9,0))
        QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    }

    static inline TMBool IsDPIAware()
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5,9,0))
        return QGuiApplication::testAttribute(Qt::AA_EnableHighDpiScaling);
#else
        return false;
#endif
    }

    static inline void Quit(int nExitCode)
    {
        QApplication::exit(nExitCode);
    }

    static void ReleaseCapture()
    {
        QWidget* widget = GetCapture();
        if (widget)
            widget->releaseMouse();
    }

    static QWidget* GetCapture()
    {
        return QWidget::mouseGrabber();
    }

    static TMBool IsHighContrast()
    {
#ifdef TM_WIN
        HIGHCONTRAST hc = { sizeof(HIGHCONTRAST) };
        if (!SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, FALSE))
            return FALSE;
        return (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
#else
        // Qt does not expose a cross-platform high-contrast switch on all versions.
        // Use an accessibility-oriented fallback: very high text/background contrast.
        const QPalette pal = QApplication::palette();
        const QColor fg = pal.color(QPalette::Active, QPalette::WindowText);
        const QColor bg = pal.color(QPalette::Active, QPalette::Window);

        auto to_linear = [](int c) -> double {
            const double v = c / 255.0;
            return (v <= 0.03928) ? (v / 12.92) : std::pow((v + 0.055) / 1.055, 2.4);
        };

        auto luminance = [&](const QColor& c) -> double {
            return 0.2126 * to_linear(c.red()) +
                0.7152 * to_linear(c.green()) +
                0.0722 * to_linear(c.blue());
        };

        double l1 = luminance(fg);
        double l2 = luminance(bg);
        if (l1 < l2)
            tm_swap(l1, l2);

        const double ratio = (l1 + 0.05) / (l2 + 0.05);
        return ratio >= 7.0;
#endif
    }
};


TM_END_NAMESPACE



TM_BGN_NAMESPACE

typedef CQWidgetT<QDialog>  CWindowQt;



///////////////////////////////////////////////////////////////////////////////
// CRefWindowBaseT
template <class T, class TBase = CWindowQt>
class CRefWindowBaseT
    : public TBase
{
public:
    typedef CRefWindowBaseT CRefWindow;

    struct CCustomObject : public TBase::value_type
    {
        TBase* m_pOwner;
        CCustomObject(TBase* p) : m_pOwner(p) {}

        // return true to stop message.
        bool nativeEvent(const QByteArray &eventType, void *message, long *result)
        {
            TMMSG* msg = (TMMSG*)message;
            (static_cast<T*>(m_pOwner))->WndProc(msg->message, msg->wParam, msg->lParam);
            return false;
        };

        void customEvent(QEvent *event) tm_override
        {
            if (event->type() == CMessageEvent::GetEventType())
            {
                CMessageEvent* pEvent = static_cast<CMessageEvent*>(event);
                TMResult hr = (static_cast<T*>(m_pOwner))->WndProc(pEvent->m_message, pEvent->m_wParam, pEvent->m_lParam);
                if (pEvent->m_lr)
                    *pEvent->m_lr = hr;
                if (pEvent->m_semaphone)
                    pEvent->m_semaphone->release();
            }
        }
    };
    
    ~CRefWindowBaseT()
    {
        DbgAssert(!this->m_obj);
    }
    
    static TMCStr GetWndClassName() { return TM_T("TM window"); }
    
    TMResult WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
    {
        return 0;
    }
    
    void SendPostedEvents()
    {
        DbgAssert(this->m_obj);
        QApplication::sendPostedEvents(this->m_obj, CMessageEvent::GetEventType());
    }
};

template <class T>
class CRefWindowQtT : public CRefWindowBaseT<T, QDialog>
{
public:
    void Create(TMHWnd hWndParent, const TMRect& rcPos, TMCStr szWindowName = NULL,
        TMUInt32 dwStyle = 0, TMUInt32 dwExStyle = 0,
        TMUInt nID = 0, void* lpCreateParam = NULL, TMHModule hModule = _ModuleATL.GetModuleInstance())
    {
        typedef typename CRefWindowBaseT<T, QDialog>::CCustomObject CCustomObject;
        QDialog* p = new CCustomObject(this);
        this->m_obj = p;
        if (szWindowName)
            this->SetWindowText(szWindowName);
        p->hide();
        //        p->exec();
    }
};



// template <class T>
// class CATMessageWindowT : public CRefWindowBaseT<T, CQObjectT<QObject> >
// {
// public:
//     void Create()
//     {
//         typedef typename CRefWindowBaseT<T, CQObjectT<QObject> >::CCustomObject CCustomObject;
//         this->m_obj = new CCustomObject(this);
//     }
// 
//     TMBool IsWindow()
//     {
//         return 0 != this->m_obj;
//     }
// 
//     void DestroyWindow()
//     {
//         delete this->m_obj;
//         this->m_obj = 0;
//     }
// };



TM_END_NAMESPACE



#endif //TMATL_QT_H
