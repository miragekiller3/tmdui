/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.14
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef HELPTER_H
#define HELPTER_H


TM_BGN_NAMESPACE

const TMUInt uPrintMask = PRF_CHECKVISIBLE | PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN;

#ifdef TM_DBG
struct _GXCheckProp
{
    IGXApp* app;
    _GXCheckProp(IGXApp* _app) : app(_app) { app->Push(eGXPropNone); }
    ~_GXCheckProp() { EGXProp prop = app->Pop(); DbgAssert(prop == eGXPropNone); }
};
#define GXCheckProp(x) _GXCheckProp _temp(x);
#else
TM_INLINE void GXCheckProp(IGXApp* _app) {}
#endif


////////////////////////////////////////////////////////////////////////////////
// CWidgetHelper
class CWidgetHelper
{
public:
    static CTuiWgt* FindChild (CTuiWgt* pParent, int nID)
    {
        for (CTuiWgt::CWndRec rec(pParent->GetChilds()); rec; ++rec)
        {
            CTuiWgt* p = *rec;
            if (p->GetObjectID() == nID)
                return p;
        }
        return 0;
    }

    // print self and all child
    static void DoPrintSelf(IGXApp* gapp, CTuiWgt* p, TMLParam lParam)
    {
        if ((p->m_dwPriStyle & WS_PRI_DISABLEREDRAW)
            || ((lParam & PRF_CHECKVISIBLE) && !p->IsWindowVisible())
            || (!(lParam & PRF_BIND) && p->Is_WS_PRI_BINDMODE()))
            return;

        if (gapp->ClipAndPush(p->m_rcWnd, true, eGXPropClip | eGXPropFont | eGXPropTextColor))
        {
            {
                CAutoMsgAnalysis awm("CTuiWgt::_DoPrint", p->GetObjectID());
                GXCheckProp(gapp);
                p->DoPrint(gapp, lParam);
            }
            gapp->Pop();
        }
    }

    static TMBool CALLBACK PfnCreateSubclass(HTUI hWgt, TMLParam lParam)
    {
        if (hWgt->IsLifeNew())
        {
            // subclass widget it self
            hWgt->Subclass_self();

            // create suclassed class
            CModule::CPreCallbackList::recorder rec(hWgt->GetModule()->m_aCallback);
            int nID = hWgt->GetObjectID();
            rec.find(nID);
            for (; rec; ++rec)
            {
                if (rec.get_key() != nID)
                    break;
                CModule::CPreCallbackList::data_reference func = rec.get_data();
                func.creator(hWgt, func.param1, func.param2);
            }
        }
        return true;
    }

    static void BroadcastSubclass(HTUI hWgt)
    {
        hWgt->EnumAllChildWindows(PfnCreateSubclass, 0, true, -1);
    }

    static TMBool CALLBACK PfnSendNCCreateMessage(HTUI hWgt, TMLParam lParam)
    {
        if (hWgt->GetLife() == WS_PRI_LIFE_NEW)
        {
            hWgt->SetLife(WS_PRI_LIFE_NCCREATE);
            hWgt->SendMessage (WM_NCCREATE, 0, lParam);
        }
        return true;
    }

    static TMBool CALLBACK PfnSendCreateMessage(HTUI hWgt, TMLParam lParam)
    {
        if (hWgt->GetLife() == WS_PRI_LIFE_NCCREATE)
        {
            hWgt->SetLife(WS_PRI_LIFE_CREATE);
            hWgt->SendMessage (WM_CREATE, 0, lParam);
        }
        return true;
    }

    static void BroadcastInit(HTUI hWgt, void* param)
    {
        DbgAssert(hWgt->IsLifeNew());
        
        // send WM_NCCREATE
        PfnSendNCCreateMessage(hWgt, (TMLParam)param);
        hWgt->EnumAllChildWindows(PfnSendNCCreateMessage, 0, false, -1);
        
        // send WM_CREATE
        PfnSendCreateMessage(hWgt, (TMLParam)param);
        hWgt->EnumAllChildWindows(PfnSendCreateMessage, 0, false, -1);
        
        DbgAssert(!hWgt->IsLifeNew()); // maybe destroyed.

        // Set InitFlag
//         hWgt->EnumAllChildWindows(PfnSetInitFlag, 0, true, -1);
    }

    template <class T>
    static TMBool OnBindModeRoute(T* pWin, TUIMSG* pMsg, TMResult& lr)
    {
        if (pMsg->message == WM_LBUTTONDOWN)
        {
            if (!pMsg->hWnd->IsBindOrSub(pWin))
                return false;
            //CTuiWidget::DefRouteProc(pMsg);
            pWin->OnBindModeLButtonDown(pMsg->wParam, pMsg->lParam);
            //return true;
        }
        else if (pMsg->message == WM_RBUTTONDOWN)
        {
            if (!pMsg->hWnd->IsBindOrSub(pWin))
                return false;
            //CTuiWidget::DefRouteProc(pMsg);
            pWin->OnBindModeRButtonDown(pMsg->wParam, pMsg->lParam);
            //return true;
        }

        return false;
    }

    template <class T>
    static TMBool OnBindModeBubble(T* pWin, TUIMSG* pMsg, TMResult& lr)
    {
        if (pMsg->message == WM_KEYDOWN)
        {
            if (!pMsg->hWnd->IsBindOrSub(pWin))
                return false;
            return lr = pWin->OnBindModeKeyDown(pMsg->wParam, pMsg->lParam);
        }
        return false;
    }

}; // class CWidgetHelper


TM_END_NAMESPACE


#endif  //HELPTER_H