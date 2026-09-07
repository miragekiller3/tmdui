#include "../tmduiatl.h"
#include "../tmgdix.h"
#include "../tmdrag.h"
#include "view.h"
#include "uiwidget.h"
#include "helper.h"


TM_BGN_NAMESPACE



TM_BGN_PRIVATE_NAMESPACE

TMBool tuiDebugIsWindowValid(HTUI hWnd, TMBool bSameThread)
{
    if (!hWnd)
        return 0;
    return hWnd->IC_IsValidInstance(bSameThread);
}

HTUI tuiCreateByObjectID(HTUI*, HTUI hWndParent, TMHModule hRes, TMUInt uObjID, TUIFACTORY* pFactory, void* param)
{
    ASSERT_CLASS(hWndParent);

    LOG_FUNC_FOR_SPY(hWndParent, L"tuiCreateByObjectID(hParent:0x%p, hRes:0x%p, oid:%d, pFactory:0x%p, param:0x%p)",
        hWndParent, hRes, uObjID, pFactory, param);

    CModule* p;
    if (hRes)
    {
        p = hWndParent->GetView()->GetModule(hRes);
        if (!p)
            return 0;
    }
    else
        p = hWndParent->GetModule();
    return hWndParent->CreateByObjectID(p, uObjID, pFactory, param);
}

TMBool tuiDestroyWindow(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiDestroyWindow()");
    return hWnd->DestroyWindow();
}

TMBool tuiSubclassWindow(HTUI hWnd, const CWidgetMsgDelegate& func)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiSubclassWindow(func:0x%p)", &func);
    if (hWnd->IsLifeInvalid())
        return false;
    hWnd->Subclass(func);
    hWnd->EnumResetRoutePrintClient();
    return true;
}

TMBool tuiUnsubclassWindow(HTUI hWnd, const CWidgetMsgDelegate& func)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiUnsubclassWindow(func:0x%p)", &func);
    hWnd->EnumResetRoutePrintClient();
    return hWnd->GetProcs().Unsubclass(hWnd, func);
}

TMResult tuiDefSubclassProc(HTUI hWnd, TMUInt uMsg, TMWParam wp, TMLParam lp)
{
    ASSERT_CLASS(hWnd);
//    LOG_FUNC_FOR_SPY(hWnd, L"tuiDefSubclassProc(uMsg:%d, wParam:0X%X, lParam:0x%X)", uMsg, wp, lp);
    TMResult lr = 0;
    hWnd->GetProcs().CallNext(hWnd, uMsg, wp, lp, lr);
    return lr;
}

TMResult tuiDefRouteProc(TUIMSG* pMsg)
{
//     pMsg->Reserved = true;
    ASSERT_CLASS(pMsg->hWnd);
    LOG_FUNC_FOR_SPY(pMsg->hWnd, L"tuiDefRouteProc(pMsg:0x%p)", pMsg);
    return pMsg->hWnd->GetProcs().SendProcMessage(pMsg->hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
}

TMHWnd tuiGetHWND(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetHWND()");
    return hWnd->GetHWND();
}

TMHModule tuiGetModule(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetModule()");
    return hWnd->GetModule()->m_hRes;
}

TMUInt32 tuiGetStyle(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetStyle()");
    return hWnd->GetStyle();
}

TMUInt32 tuiGetExStyle(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetExStyle()");
    return hWnd->GetExStyle();
}

TMUInt32 tuiSetStyle(HTUI hWnd, TMUInt32 dwStyle)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiSetStyle(dwStyle:0x%X)", dwStyle);
    return hWnd->SetStyle(dwStyle);
}

TMUInt32 tuiSetExStyle(HTUI hWnd, TMUInt32 dwExStyle)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiSetExStyle(dwExStyle:0x%X)", dwExStyle);
    return hWnd->SetExStyle(dwExStyle);
}

TMUInt32 tuiGetLayOut(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetLayOut()");
    return hWnd->m_dwLayout;
}

void tuiSetLayOut(HTUI hWnd, TMUInt32 dwLayout)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiSetLayOut(dwLayout:0x%X)", dwLayout);
    if (hWnd->m_dwLayout == dwLayout)
        return;
    hWnd->m_dwLayout = dwLayout;
    hWnd->CheckParentFrameChange(0);
    hWnd->ChangeFrame(false);
}

TMLParam tuiSetTag(HTUI hWnd, TMLParam dwTag)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiSetTag(dwTag:0x%X)", dwTag);
    TMLong dwOld = hWnd->m_dwTag;
    hWnd->m_dwTag = dwTag;
    return dwOld;
}

TMLParam tuiGetTag(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetTag()");
    return hWnd->m_dwTag;
}

TMUInt32 tuiGetRole(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetRole()");
    return hWnd->m_dwRole;
}

void tuiSetRole(HTUI hWnd, TMUInt32 role)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiSetRole()");
    hWnd->m_dwRole = role;
}

EBindState tuiGetBindState(HTUI hWnd, HTUI hParentStop)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetBindState(hParentStop:0x%p)", hParentStop);
    return hWnd->IsBindOrSub(hParentStop);
}

TMResult tuiSendMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiSendMessage(uMsg:0x%X, wParam:0x%X, lParam:0x%X)", uMsg, wParam, lParam);
    return hWnd->SendMessage(uMsg, wParam, lParam);
}

TMBool tuiPostMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam)
{
//     LOG_FUNC_FOR_SPY(hWnd, L"tuiPostMessage(uMsg:%d, wParam:0X%X, lParam:0x%X)", uMsg, wParam, lParam);
    return hWnd->PostMessage(uMsg, wParam, lParam);
}

TMResult tuiSendBroadcastMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool bSendSelf)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiSendBroadcastMessage(uMsg:%d, wParam:0X%X, lParam:0x%X, bSendSelf:%d)", uMsg, wParam, lParam, bSendSelf);
    return hWnd->SendBroadcastMessage(uMsg, wParam, lParam, bSendSelf);
}

void tuiGetWindowText(HTUI hWnd, CStrPtr& str)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetWindowText()");
    str = hWnd->GetWindowText();
}

void tuiGetClassName(HTUI hWnd, CStrPtr& str)
{
    ASSERT_CLASS(hWnd);
//    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetClassName(hWnd:0x%p)", hWnd);
    str = hWnd->GetClassName();
}

void tuiGetHint(HTUI hWnd, CStrPtr& str)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetHint()");
    str = hWnd->m_strHint;
}

void tuiGetAccText(HTUI hWnd, CStrPtr& str)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetAccText()");
    str = hWnd->m_strAcc;
}

TMBool tuiSetWindowPos(HTUI hWnd, HTUI hWndInsertAfter, GXDips x, GXDips y, GXDips cx, GXDips cy, TMUInt nFlags)
{
    ASSERT_CLASS(hWnd);
//     DbgAssert(!hWnd->IsManager())(" oid:%d", hWnd->GetObjectID());
    DbgAssert(((nFlags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE))
        || ((hWnd->m_dwLayout & ALIGN_MASK) != ALIGN_CLIENT))(" oid:%d", hWnd->GetObjectID()); // don't move or resize ALIGN_CLIENT window 
//     DbgAssert(((nFlags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE))
//         || (!hWnd->IsManager()))(" oid:%d", hWnd->GetObjectID()); // don't move or resize mgr 

    LOG_FUNC_FOR_SPY(hWnd, L"tuiSetWindowPos(hWndInsertAfter:0x%p, x:%g, y:%g, cx:%g, cy:%g, nFlags:px%X)",
        hWndInsertAfter, x, y, cx, cy, nFlags);
    return hWnd->SetWindowPos(hWndInsertAfter, x, y, cx, cy, nFlags);
}

TMBool tuiDoMenuPop(HTUI hWnd, HTUI hOwner, HTUI hNewFocus)
{
    ASSERT_CLASS(hWnd);
    ASSERT_CLASS(hOwner);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiDoMenuPop(hOwner:0x%p, hNewFocus:0x%p)",
        hOwner, hNewFocus);
    return hWnd->GetView()->DoMenuPop(hOwner, hWnd, hNewFocus);
}

HTUI tuiGetMenuPop(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetMenuPop()");
    return hWnd->GetView()->GetMenuPop(hWnd);
}

TMBool tuiSortChild(HTUI hWnd, PFNWWCOMPARE pfn, TMLParam lParam, int nLevel, TMBool bReversed)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiSortChild(PFNWWCOMPARE:0x%p, lParam:0x%X, nLevel:%d, bReversed:%d)",
        pfn, lParam, nLevel, bReversed);
    return hWnd->SortChild(pfn, lParam, nLevel, bReversed);
}

TMBool tuiChangeFrame(HTUI hWnd, TMBool bUpdate)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiChangeFrame(bUpdate:%d)", bUpdate);
    return hWnd->ChangeFrame(bUpdate);
}

void tuiGetWindowRect(HTUI hWnd, TUIRect* lpRect)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetWindowRect()");
    hWnd->GetWindowRect(lpRect);
}

void tuiGetClientRect(HTUI hWnd, TUIRect* lpRect)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetClientRect()");
    hWnd->GetClientRect(lpRect);
}

void tuiGetPaddingRect(HTUI hWnd, TUIRect* lpRect)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetPaddingRect()");
    hWnd->GetPaddingRect(lpRect);
}

void tuiGetProjectRect(HTUI hWnd, TUIRect* lpRect)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetProjectRect()");
    hWnd->GetProjectRect(*lpRect);
}

TMBool tuiUpdateWindow(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiUpdateWindow()");
    return hWnd->UpdateWindow();
}

TMBool tuiInvalidateRect(HTUI hWnd, const TUIRect* lpRect, TMBool bErase)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiInvalidateRect(lpRect:0x%p, bErase:%d)", lpRect, bErase);

    if (lpRect)
        return hWnd->InvalidateRect(*lpRect);
    else
        return hWnd->InvalidateRect(hWnd->m_rcWnd);
}

TMBool tuiEnableWindow(HTUI hWnd, TMBool bEnable)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiEnableWindow(bEnable:%d)", bEnable);
    return hWnd->EnableWindow(bEnable);
}

TMBool tuiEnableNotify(HTUI hWnd, TMBool bEnable)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiEnableNotify(bEnable:%d)", bEnable);
    bool b = hWnd->IsFlagEnableNotify();
    hWnd->SettingFlagEnableNotify(bEnable);
    return b;
}

TMBool tuiIsEnableNotify(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiIsEnableNotify()");
    return hWnd->IsFlagEnableNotify();
}

HTUI tuiChildWindowFromPoint(HTUI hWnd, TUIPoint point, TMUInt uFlags)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiChildWindowFromPoint(point:%g,%g, uFlags:0x%X)", PointX(point), PointY(point), uFlags);
    HTUI h1 = 0, h2 = 0;
    hWnd->GetMouseTarget(h1, h2, point, uFlags); // ChildWindowFromPoint(point, uFlags);
    return h1;
}

HTUI tuiGetWindow(HTUI hWnd, TMUInt nCmd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetWindow(nCmd:0x%X)", nCmd);
    return hWnd->GetWindow(nCmd);
}

TMBool tuiIsChild(HTUI hWnd, HTUI hChild)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiIsChild(hChild:0x%p)", hChild);
    return hWnd->IsChild(hChild);
}

HTUI tuiGetParent(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetParent()");
    return hWnd->GetParent();
}

HTUI tuiSetParent(HTUI hWnd, HTUI hWndNewParent)
{
    ASSERT_CLASS(hWnd);
    ASSERT_CLASS(hWndNewParent);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiSetParent(hWndNewParent:0x%p)", hWndNewParent);
    return hWnd->SetParent(hWndNewParent);
}

int tuiGetDlgCtrlID(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetDlgCtrlID()");
    return hWnd->GetDlgCtrlID();
}

int tuiSetDlgCtrlID(HTUI hWnd, int nID)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiSetDlgCtrlID(nID:%d)", nID);
    return hWnd->SetDlgCtrlID(nID);
}

int tuiGetDlgCtrlObjectID(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetDlgCtrlObjectID()");
    return hWnd->m_dwObjID;
}

HTUI tuiGetDlgItem(HTUI hWnd, int nID)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetDlgItem(nID:%d)", nID);
    return hWnd->GetDlgItem(nID);
}

GXPTexture tuiMakeImage(HTUI hWnd, const TUIRect* prc, TMColor clrBk, TMUInt uFlag)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiMakeImage()");
    CTuiRect rc;
    if (!prc)
    {
        hWnd->GetWindowRect(&rc);
        prc = &rc;
    }
    CTuiZoom zoom(hWnd->GetDPI());
    int w = zoom.LPtoDP(RectWidth(*prc));
    int h = zoom.LPtoDP(RectHeight(*prc));
    
    CTextureHandle gdc;
    gdc.CreateRenderTarget(w, h);
    if (!gdc)
        return 0;
    
    {
        CAutoTextureRender ar;
        ar.SetMapping(gdc, ((CTuiRect*)prc)->TopLeft(), hWnd->GetDPI(), hWnd->GetModule()->GetLocal());
        ar->Clear(clrBk);
        DISABLE_SPY();
        CTuiWidget(hWnd).CopyWindowImageEx(uFlag);//gdc, 0, 0, true);
    }

    return gdc;
}

void tuiSetDragImage(HTUI hWnd, CDragData& obj, TMPointI ptOffset, const CTuiRect* prc, TMUInt8 alpha, TMBool bSrcAlpha, TMColor clrKey)
{
    GXPTexture tex = tuiMakeImage(hWnd, prc);
    if (tex)
    {
        CAutoTextureDC dc(tex);
        obj.SetDragImage(ptOffset, dc, CRectI(0,0,tex->GetWidth(),tex->GetHeight()), alpha, bSrcAlpha, clrKey);
        tex->Release();
    }
}

// 
// void tuiSetDragImage(HTUI hWnd, CDragData& obj, TMPointI ptOffset, const CTuiRect* prc, TMUInt8 alpha, TMBool bSrcAlpha, TMColor clrKey)
// {
//     ASSERT_CLASS(hWnd);
//     LOG_FUNC_FOR_SPY(hWnd, L"tuiSetDragImage()");
//     CTuiRect rc;
//     if (!prc)
//     {
//         hWnd->GetWindowRect(&rc);
//         prc = &rc;
//     }
//     CTuiZoom zoom(hWnd->GetDPI());
//     int w = zoom.LPtoDP(prc->Width());
//     int h = zoom.LPtoDP(prc->Height());
//     
//     CTexture gdc;
//     gdc.CreateRenderTarget(w, h);
//     if (!gdc)
//         return;
// 
// //    gdc.ColorFill(TMRGBA(0,0,0,0));
//     
//     {
//         CAutoTextureRender ar;
//         ar.SetMapping(gdc, prc->TopLeft(), hWnd->GetDPI(), hWnd->GetModule()->GetLocal());
// //        gxSetMapping(CTuiPoint(0,0), prc->TopLeft(), hWnd->GetDPI());
//         DISABLE_SPY();
//         ar->FillRect(rc, TMRGBA(0,0,0,0));
//         CTuiWidget(hWnd).CopyWindowImage(true);//gdc, 0, 0, true);
//     }
//    
//     CAutoTextureDC dc(gdc);
//     obj.SetDragImage(ptOffset, dc, CRectI(0,0,w,h), alpha, bSrcAlpha, clrKey);
// }

TMUInt32 tuiDoDrag(HTUI hWnd, CDragData& obj, EDragEff dwEff)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiDoDrag(obj:%P, dwEff:0x%X)", &obj, dwEff);
    return obj.DoDrag(hWnd, dwEff);
}

TMBool tuiEnumAllChildWindows(HTUI hWnd, PFNWWENUMPROC lpEnumFunc, TMLParam lParam, TMBool bEnumSelf, size_t nLevel)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiEnumAllChildWindows(lpEnumFunc:0x%p, lParam:0x%X, nLevel:%d, bEnumSelf:%d)",
        lpEnumFunc, lParam, nLevel, bEnumSelf);
    return hWnd->EnumAllChildWindows(lpEnumFunc, lParam, bEnumSelf, nLevel);
}

void tuiClientToScreen(HTUI hWnd, const TUIPoint* lpClient, int nPt, TMPoint* lpScreen)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiClientToScreen(nPt:%d)", nPt);
    hWnd->ClientToScreen(lpClient, nPt, lpScreen);
}

void tuiScreenToClient(HTUI hWnd, const TMPoint* lpScreen, int nPt, TUIPoint* lpClient)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiScreenToClient(nPt:%d)", nPt);
    hWnd->ScreenToClient(lpScreen, nPt, lpClient);
}

int tuiGetDPI(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetDPI()");
    return hWnd->m_nDPI;
}

IColorTransform* tuiGetColorTransform(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetColorTransform()");
    return hWnd->GetModule()->m_pColorTransform;
}

TMBool tuiAnimateWindow(HTUI hWnd, TMUInt32 dwTime, TMUInt32 dwFlags, const CTuiRect* rcFrom, const CTuiRect* rcTo, IEase* pEase)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiAnimateWindow()");
    return hWnd->m_aniList.AnimateWindow(hWnd, dwTime, dwFlags, rcFrom, rcTo, pEase);
}

struct _CTuiWinPic
    : public CBaseRefT<_CTuiWinPic, ITuiPic>
    , public TM::CRefObjectRoot<1>
    , public CAutoTuiWidget
{
public:
    using CAutoTuiWidget::m_hWnd;

    TM_BGN_INTERFACE_MAP(_CTuiWinPic)
        TM_INTERFACE_HANDLER(ITuiPic);
    TM_END_INTERFACE_MAP()

    _CTuiWinPic(HTUI hWnd)
        : CAutoTuiWidget(hWnd)
    {
        DbgAssert(!hWnd->Is_WS_PRI_BINDMODE());
        hWnd->Set_WS_PRI_BINDMODE();
        hWnd->m_dwLayout &= ~(ANCHOR_HMASK | ANCHOR_VMASK);
        hWnd->m_dwLayout |= (ANCHOR_NONE | ANCHOR_VNONE);
    }

    ~_CTuiWinPic()
    {
        if (m_hWnd)
            m_hWnd->Reset_WS_PRI_BINDMODE();
    }

    virtual TMUInt32 GetObjectID () const tm_override { return 0; }
    virtual void Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp) tm_override
    {
        if (!m_hWnd)
            return;

        if (gapp->ClipAndPush(rc, true, eGXPropClip))
        {
            m_hWnd->ResetBindItemPosition(rc, SWP_NOREDRAW | SWP_NOLAYOUTSIBLING, false);
            if (m_hWnd)
                CWidgetHelper::DoPrintSelf(gapp, (*this), c_lpPrint | PRF_BIND);
            gapp->Pop();
        }
    }
//    virtual void RenderSB(const SCROLLSTATE& state, const CTuiRect& rc, WPRENDERFX_SB* fx = 0) tm_override {}
    virtual TMUInt GetFrames() tm_override { return 1; }
    virtual int GetDelay() tm_override { return 0; }
    virtual TMUInt GetCurrentFrame() tm_override { return 0; }
    virtual void GetSize(TMSize& sz) tm_override { SizeSetWidth(sz, m_hWnd->GetWidth()); SizeSetHeight(sz, m_hWnd->GetHeight()); }
    virtual void SetCurFrame(TMUInt n) tm_override {}
    virtual void SetAlpha(TMUInt8 nAlpha) tm_override {}
    virtual CComPtr<ITuiPic> GetSubPic(int dpi) tm_override { return this; }
};

void tuiConvertToPic(HTUI hWnd, ITuiPic** ppic)
{
    ASSERT_CLASS(hWnd);
    DbgAssert(ppic);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiConvertToPic()");
    _CTuiWinPic* p = new _CTuiWinPic(hWnd);
    *ppic = p;
}

TMBool tuiIsValid(HTUI hWnd)
{
    ASSERT_CLASS(hWnd);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiIsValid()");
    return !hWnd->IsLifeInvalid();
}

TM_END_PRIVATE_NAMESPACE

TM_DECLARE_UUID_NS(TM, CRichWidgetObject,      557C2DEC, 72DA, 4e7e, B7, DE, FE, FF, 68, 85, 03, 01);
struct CRichWidgetObject
    : public IRichInlineObject
{
public:
    TM_UNUSED_IC_GetDebugInfo();

    TM_BGN_INTERFACE_MAP(IRichInlineObject)
    TM_END_INTERFACE_MAP()

    CTuiWidget wndParent;
    HTUI m_hWnd;
    static TMBool _Proc(CRichWidgetObject* pObj, HTUI hWnd,
        TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
    {
        if (uMsg == CM_HITTEST)
        {
            TUIHITINFO* pht = (TUIHITINFO*)lParam;
            TM::CTuiRect rc(pObj->m_hWnd->GetBoundRect());
            if (rc.PtInRect(pht->pt))
            {
                pObj->m_hWnd->GetAndHitMouseTarget(*pht, wParam, 0);
                return true;
            }
        }
        else if (uMsg == WM_NCDESTROY)
        {
            pObj->Destroy();
        }
        return false;
    }

    CRichWidgetObject() : m_hWnd(0) {}
    
    CWidgetMsgDelegate GetDelegate()
    {
        return CWidgetMsgDelegate(this, &_Proc);
    }

    void Init(HTUI hParent, HTUI hWnd)
    {
        wndParent = hParent;
        hWnd->Bind_Set(wndParent, 0);
        //SubclassWindow(hWnd);
        m_hWnd = hWnd;
//        _Debug_WindowMustBeNotMgr();
        wndParent.SubclassWindow(GetDelegate());
    }

    virtual void Destroy() tm_override
    {
        if (m_hWnd)
        {
            m_hWnd->DestroyWindow();
            m_hWnd = 0;
        }
        if (wndParent)
        {
            wndParent.UnsubclassWindow(GetDelegate());
            wndParent = 0;
        }
    }

    virtual void SetFont(const GXFont& ft) tm_override {}
    virtual void SetColor(TMColor clr) tm_override {}
    
    virtual void GetMetrics(GXSize& sz) tm_override
    {
        if (!m_hWnd)
            return;
        sz.SetSize(m_hWnd->GetMarginWidth(), m_hWnd->GetMarginHeight());
    }

    virtual void Render(IGXApp* gapp, GXRect& rc, const RICHDRAW& td) tm_override
    {
        if (!m_hWnd)
            return;
        m_hWnd->ResetBindItemPosition(rc, SWP_NOREDRAW);
        if (m_hWnd)
            CWidgetHelper::DoPrintSelf(gapp, m_hWnd, c_lpPrint | PRF_BIND);
    }
};

TM_DECLARE_UUID_NS(TM, CRichPicObject,      557C2DEC, 72DA, 4e7e, B7, DE, FE, FF, 68, 85, 03, 02);
struct CRichPicObject
    : public IRichInlineObject
{
public:
    TM_BGN_INTERFACE_MAP(IRichInlineObject)
    TM_END_INTERFACE_MAP()

    TM_UNUSED_IC_GetDebugInfo();
    
    HTUI m_hParent;
    CPicPtr m_ptrPic;
    
    void Init(HTUI hParent, ITuiPic* pic)
    {
        m_ptrPic.Attach(pic);
        m_hParent = hParent;
    }
    
    virtual void Destroy() tm_override {}
    virtual void SetFont(const GXFont& ft) tm_override {};
    virtual void SetColor(TMColor clr) tm_override {}
    
    virtual void GetMetrics(GXSize& sz) tm_override
    {
        if (!m_ptrPic)
            return;
        CZoom dpi(m_hParent->GetDPI());
        m_ptrPic.ChangeDPI(dpi);
        CSize szi;
        m_ptrPic->GetSize(szi);
        dpi.DPtoLP(szi, sz);
    }
    
    virtual void Render(IGXApp* gapp, GXRect& rc, const RICHDRAW& td) tm_override
    {
        if (!m_ptrPic)
            return;
        m_ptrPic->Render(0, 0, rc, 0, gapp);
    }
};

class CRichCustomCreator : public IRichContext
{
public:
    HTUI m_hWnd;
    
public:
    TM_UNUSED_IC_GetDebugInfo();

    TM_BGN_INTERFACE_MAP(IRichContext)
    TM_END_INTERFACE_MAP()
    
    virtual void CreateCustomObject(IRichInlinecontainer* pCont, IRichInlineObject** ppObj) tm_override
    {
        DbgAssert(ppObj);
        DbgAssert(!*ppObj);
        CGXStrPtr str;
        pCont->GetAttribute(L"src", str);
        int id = str.ToLong(0);
        if (!id)
            return;
        CTuiObject* pObj = m_hWnd->GetModule()->InternalCreateObject(e_otUnknown, m_hWnd, id, m_hWnd->GetDPI(), false);
        if (!pObj)
            return;
        switch (pObj->GetType())
        {
        case e_otPic:
            {
                CRichPicObject* p = new CRefObjectT<CRichPicObject>(1);
                p->Init(m_hWnd, static_cast<CPic*>(pObj));
                *ppObj = p;
            }
            return;

        case e_otWgt:
            {
                CRichWidgetObject* p = new CRefObjectT<CRichWidgetObject>(1);
                HTUI h = static_cast<HTUI>(pObj);
                h->InitConstruct(0, 0);
                p->Init(m_hWnd, h);
                *ppObj = p;
            }
            return;
        }

        pObj->Release();
        return;
    }
};

TM_BGN_PRIVATE_NAMESPACE

void tuiGetRichContext(HTUI hWnd, IRichContext** ppCreator)
{
    DbgAssert(ppCreator);
    DbgAssert(!*ppCreator);
    LOG_FUNC_FOR_SPY(hWnd, L"tuiGetRichContext(ppCreator0x%p)", ppCreator);
#ifdef TUI_EDIT_MODE
    if (hWnd->GetModuleHandle() == HEDITOR)
        return;
#endif
    CRichCustomCreator* p = new CRefObjectT<CRichCustomCreator>(1);
    p->m_hWnd = hWnd;
    *ppCreator = p;
}




TM_END_PRIVATE_NAMESPACE


#ifdef TM_WIN_DESKTOP


#endif


TM_END_NAMESPACE


