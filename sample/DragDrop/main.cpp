#include "stdafx.h"
#include "main.h"
#include "tmgdix.h"
#include "gximage.h"
#include "tmdrag.h"


using namespace TM;

static CDataFormat cfTUI (_T("tui object id"));


// static CMain::Register<0> s_callback_CMain;

///////////////////////////////////////////////////////////////////////////////
// GetDataObjectInfo
static void _GetDInfo (CString& str, TMULong dwDirection, IDataObject* pObj)
{
    static TMCStr pObjName[] =
    {
        0,
        TM_T("CF_TEXT"),
        TM_T("CF_BITMAP"),
        TM_T("CF_METAFILEPICT"),
        TM_T("CF_SYLK"),
        TM_T("CF_DIF"),
        TM_T("CF_TIFF"),
        TM_T("CF_OEMTEXT"),
        TM_T("CF_DIB"),
        TM_T("CF_PALETTE"),
        TM_T("CF_PENDATA"),
        TM_T("CF_RIFF"),
        TM_T("CF_WAVE"),
        TM_T("CF_UNICODETEXT"),
        TM_T("CF_ENHMETAFILE"),
        TM_T("CF_HDROP"),
        TM_T("CF_LOCALE"),
        TM_T("CF_MAX"),
    };
    
    CComPtr<IEnumFORMATETC> ptrEnum;
    pObj->EnumFormatEtc (dwDirection, &ptrEnum);
    if (ptrEnum)
    {
        FORMATETC fmt;
        ULONG l;
        for (ptrEnum->Reset(); 0 == ptrEnum->Next(1, &fmt, &l);)
        {
            TMCharT c[1024] = {0};
            TMCStr pName;
            if (fmt.cfFormat < CF_MAX)
            {
                pName = pObjName[fmt.cfFormat];
            }
            else
            {
                GetClipboardFormatName (fmt.cfFormat, c, 1024);
                pName = c;
            }
            
            CString strx;
            strx.Format (TM_T("0x%04x - %d - %d - %d - \"%s\"\r\n"),
                fmt.cfFormat, fmt.dwAspect, fmt.lindex, fmt.tymed, pName);
            str += strx;
        }
    }
}

CString GetDataObjectInfo (IDataObject* pObj)
{
    CString str;
    str += "DATADIR_GET";
    str += "\r\n";
    _GetDInfo (str, DATADIR_GET, pObj);
    str += "\r\n";
    
    str += "DATADIR_SET";
    str += "\r\n";
    _GetDInfo (str, DATADIR_SET, pObj);
    
    return str;
}

// TMResult CMain::OnGetDragImage(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandle)
// {
//     DRAGIMAGE* p = (DRAGIMAGE*)pMsg->lParam;
// 
//     CTuiWidget win(pMsg->hWnd);
// 
//     RECT rc;
//     win.GetWindowRect (&rc);
//     int w = rc.right - rc.left;
//     int h = rc.bottom - rc.top;
//     
//     {
//     CGraphDC gdc;
//     gdc.Create (0, w, h);
//     gdc.FillSolidRect (&CRect(0, 0, w, h), RGB(255, 128, 64));
//     gdc.TextOut (0, 0, _T("abcdefghijkl,mop"));
// 
// //     TMHDC hDC = GetWindowDC();
// //     {
// //         CCompatibleDC dc(dib.GetBitmap(), 0);
// //         //dc.BitBlt (0, 0, w, h, hDC, rc.left, rc.top, SRCCOPY);
// //     }
// //     ReleaseDC (hDC);
// 
//     p->hbmpDragImage = gdc.Detach();
//     p->sizeDragImage.cx = w;
//     p->sizeDragImage.cy = h;
//     p->crColorKey = RGB(255, 0, 0);
//     p->ptOffset.x = 0;
//     p->ptOffset.y = 0;
//     }
// 
// //    CWindowDC(0).BitBlt (0, 0, 200, 200, CCompatibleDC(p->hbmpDragImage), 0, 0, SRCCOPY);
// 
//     DbgAssert(::GetObjectType(p->hbmpDragImage) == OBJ_BITMAP);
// 
//     return true;
// }

TMResult CMain::OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    EndDialog(wID);
    return 0;
}

TMResult CMain::OnMouseMove(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    CTuiWidget win(pMsg->hWnd);
    TUIHITINFO hi;

    if (win.GetMouseDownHitInfo(hi))
    {
        CDragData dg;
        dg.AddMem(&pMsg->uObjectID, sizeof(pMsg->uObjectID), cfTUI);
        POINT pt = {0, 0};
        win.SetDragImage(dg, pt, 0, 128, true);
        win.DoDrag(dg, eDragEffMove);
    }

    bHandled = false;
    return 0;
}

TMResult CMain::OnDragEntry(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    DROPEVENT* pEvent = (DROPEVENT*)pMsg->lParam;
    
    CTuiWidget win(GetDlgItem(IDC_ACCEPT1));
    win.SetWindowText (GetDataObjectInfo(pEvent->pData->GetIData()));

    // becasuce it will update the window if calling settext
    // so need to redraw the imaglist at instance after update the window to avoid refreshment issue.
    UpdateWindow();
    ImageList_DragMove (pEvent->ptScreen.x, pEvent->ptScreen.y);

    if (pEvent->pData->Query(cfTUI))
    {
        return DROPEVENT_KEY2EFF(pEvent) | eDragEffMove;
    }

    if (pEvent->pData->QueryAUString())
    {
        return DROPEVENT_KEY2EFF(pEvent) | eDragEffMove;
    }

    return 0;
}

TMResult CMain::OnDragOver(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    DROPEVENT* pEvent = (DROPEVENT*)pMsg->lParam;
    return DROPEVENT_KEY2EFF(pEvent) | eDragEffMove;
}

TMResult CMain::OnDragDrop(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    DROPEVENT* pEvent = (DROPEVENT*)pMsg->lParam;
    CDropData* pData = pEvent->pData;
    DbgAssert(pData);

    CGlobalMem gm;
    if (pData->GetGlobal (gm, cfTUI))
    {
        int idCtrl = 0;
        if (gm.CopyToT(idCtrl) == sizeof(int))
        {
            CTuiWidget win(GetDlgItem(idCtrl));
            if (win)
            {
                win.SetParent (pMsg->hWnd);
                win.SetWindowPos (0, pEvent->ptWindow.x, pEvent->ptWindow.y,
                    0, 0, SWP_NOSIZE | SWP_NOZORDER);
                return eDragEffMove;
            }
        }
    }

    TM::CString str;
    if (pEvent->pData->GetAUString(str))
    {
        CTuiWidget(pMsg->hWnd).SetWindowText (str);
        return eDragEffCopy;
    }
    
    return 0;
}

