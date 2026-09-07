/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.02.18
 *
 *  Copyright (C) 2009 miragekiller
 */

#pragma once

#ifndef DRAG_H
#define DRAG_H

#include "../tmdui.h"


TM_BGN_NAMESPACE

class CTuiWnd;


#if defined(TM_WIN_DESKTOP) && !defined(TM_QT)


///////////////////////////////////////////////////////////////////////////////
// CDropMgr
class CDropMgr
{
private:
    TUIHITINFO      m_hitOld;
    TMBool          m_bEntry; // is target receive drag message

public:
    CDropMgr();

    void DoDragLeave(CTuiWnd* p);
    TMResult DoDragEntry();

    TMResult DoEvent(CTuiWnd* p, TMUInt uMsg, TMWParam wParam, TMLParam lParam);
    TMResult DoDragEvent(IDataObject* pDataObj, CTuiWnd* p, TMUInt uMsg, TMULong grfKeyState, POINTL ptx,
        TMULong* dwEffect);

    HTUI GetDragTaget() { return m_hitOld.hWnd; }
}; // CDropMgr


///////////////////////////////////////////////////////////////////////////////
// CDropTargetMgr
class CDropTargetMgr : public IDropTarget
{
public:
    TM_UNUSED_ADDREF_RELEASE();
    
    TM_BGN_INTERFACE_MAP        (IDropTarget)
        TM_INTERFACE_HANDLER    (IDropTarget)
    TM_END_INTERFACE_MAP        ()
        
    IDataObject* m_pDataObj;
    
    STDMETHODIMP DragEnter(IDataObject* pDataObj, TMULong grfKeyState, POINTL pt, TMULong* pdwEffect);
    STDMETHODIMP DragOver(TMULong grfKeyState, POINTL pt, TMULong* pdwEffect);
    STDMETHODIMP DragLeave();
    STDMETHODIMP Drop(IDataObject* pDataObj, TMULong grfKeyState, POINTL pt, TMULong* pdwEffect);
};


#else

class CDropMgr
{
public:
    CDropMgr () {}
    
    void DoDragLeave (CTuiWnd* p) {}
    TMResult DoDragEntry () { return 0; }
    TMResult DoEvent (CTuiWnd* p, TMUInt uMsg, TMWParam wParam, TMLParam lParam)  { return 0; }
    TMResult DoDragEvent (struct IDataObject* pDataObj, CTuiWnd* p, TMUInt uMsg, TMULong grfKeyState, CPoint ptx,
        TMULong* dwEffect) { return 0; }
    HTUI GetDragTaget()  { return 0; }
};

#endif // TM_WIN

TM_END_NAMESPACE

#endif // DRAG_H
