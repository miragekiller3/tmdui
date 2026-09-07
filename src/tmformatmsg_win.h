/*
 *  @file
 *  @brief  debug function for message
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.03.26
 *
 *  Copyright (C) 2015 miragekiller
 */

#pragma once

#ifndef TMFORMATMSG_WIN_H
#define TMFORMATMSG_WIN_H

#ifndef TM_WIN
#   error only support windows
#endif

#include "tmnative.h"
#include "tmatl.h"
#include "tmstring.h"
#include "tmgdi.h"
#include "tmplatform_win.h"


typedef struct __objid* _objid;
TM_INLINE LPCTSTR DbgFormatObjId(TMWParam dwObjID)
{
#   define FORAMT_CASE(m) case (LONG)(m): return TM_T(#m);

    switch ((LONG)dwObjID)
    {
        FORAMT_CASE(OBJID_WINDOW);
        FORAMT_CASE(OBJID_SYSMENU);
        FORAMT_CASE(OBJID_TITLEBAR);
        FORAMT_CASE(OBJID_MENU);
        FORAMT_CASE(OBJID_CLIENT);
        FORAMT_CASE(OBJID_VSCROLL);
        FORAMT_CASE(OBJID_HSCROLL);
        FORAMT_CASE(OBJID_SIZEGRIP);
        FORAMT_CASE(OBJID_CARET);
        FORAMT_CASE(OBJID_CURSOR);
        FORAMT_CASE(OBJID_ALERT);
        FORAMT_CASE(OBJID_SOUND);
        FORAMT_CASE(OBJID_QUERYCLASSNAMEIDX);
        FORAMT_CASE(OBJID_NATIVEOM);
        FORAMT_CASE(OBJID_UiaRootObjectId);
    }
#undef FORAMT_CASE

    static TM::CStdStr s_str;
    s_str.Format(TM_T("%X"), dwObjID);
    return s_str.c_str();
}


template <TMBool bFormatParam>
struct CFmtMessageImpl
{
    typedef struct _nulls*  _n;
    typedef struct _hex*    _h;
    typedef struct _int*    _i;
    typedef struct _comb*   _c; // use LOWORD & HIWORD
    typedef struct _ucomb*   _uc; // use unsigned LOWORD & HIWORD

    inline static void FormatParam(TM::CStdStr& str, _n p, TMCStr ps) {}
    inline static void FormatParam(TM::CStdStr& str, _h p, TMCStr ps) { str.Format(TM_T(" %s:0x%X"), ps, p); }
    inline static void FormatParam(TM::CStdStr& str, _i p, TMCStr ps) { str.Format(TM_T(" %s:%d"), ps, p); }
    inline static void FormatParam(TM::CStdStr& str, _c p, TMCStr ps) { str.Format(TM_T(" %s:%d,%d"), ps, (short)LOWORD(p), (short)HIWORD(p)); }
    inline static void FormatParam(TM::CStdStr& str, _uc p, TMCStr ps) { str.Format(TM_T(" %s:%d,%d"), ps, LOWORD(p), HIWORD(p)); }
    inline static void FormatParam(TM::CStdStr& str, TMCStr p, TMCStr ps) { str.Format(TM_T(" %s:%s"), ps, p); }
    inline static void FormatParam(TM::CStdStr& str, TMRectI* p, TMCStr ps) { if (p) str.Format(TM_T(" %s:") TM_T(TM_FMT_RCI_STR) TM_T(")"), ps, TM_FMT_RCI_VAL(*p)); else str.Format(TM_T(" %s:null"), ps); }
    inline static void FormatParam(TM::CStdStr& str, float* p, TMCStr ps) { if (p) str.Format(TM_T(" %s:%g"), ps, *p); else str.Format(TM_T(" %s:null"), ps); }
    inline static void FormatParam(TM::CStdStr& str, double* p, TMCStr ps) { if (p) str.Format(TM_T(" %s:%g"), ps, *p); else str.Format(TM_T(" %s:null"), ps); }
    inline static void FormatParam(TM::CStdStr& str, TM::CBorder* p, TMCStr ps) { if (p) str.Format(TM_T(" %s:%d,%d,%d,%d"), ps, (int)p->left, (int)p->top, (int)p->right, (int)p->bottom); else str.Format(TM_T(" %s:null"), ps); }
    inline static void FormatParam(TM::CStdStr& str, _objid p, TMCStr ps) { str.Format(TM_T(" %s:%s"), ps, DbgFormatObjId((TMWParam)p)); }

#ifdef TM_WIN
    inline static void FormatParam(TM::CStdStr& str, MINMAXINFO* p, TMCStr)
    {
        str.Format(TM_T(" ptReserved:%d,%d ptMaxSize:%d,%d ptMaxPosition:%d,%d ptMinTrackSize:%d,%d, ptMaxTrackSize:%d,%d"),
            p->ptReserved, p->ptMaxSize, p->ptMaxPosition, p->ptMinTrackSize, p->ptMaxTrackSize);
    }
    inline static void FormatParam(TM::CStdStr& str, LPDRAWITEMSTRUCT p, TMCStr)
    {
        str.Format(TM_T(" CtlType:0x%X CtlID:%d itemID:%d itemAction:0x%X itemState:0x%X hwndItem:0x%X hDC:0x%X rcItem:%d,%d,%d,%d itemData:0x%X"),
            p->CtlType, p->CtlID, p->itemID, p->itemAction, p->itemState, p->hwndItem, p->hDC, p->rcItem, p->itemData);
    }
    inline static void FormatParam(TM::CStdStr& str, LPMEASUREITEMSTRUCT p, TMCStr)
    {
        str.Format(TM_T(" CtlType:0x%X CtlID:%d itemID:%d itemWidth:%d itemHeight:%d itemData:0x%X"),
            p->CtlType, p->CtlID, p->itemID, p->itemWidth, p->itemHeight, p->itemData);
    }
    inline static void FormatParam(TM::CStdStr& str, LPDELETEITEMSTRUCT p, TMCStr)
    {
        str.Format(TM_T(" CtlType:0x%X CtlID:%d itemID:%d hwndItem:0x%X itemData:0x%X"),
            p->CtlType, p->CtlID, p->itemID, p->hwndItem, p->itemData);
    }
#endif // TM_WIN

    inline static void FormatParam(TM::CStdStr& str, TMSTYLESTRUCT* p, TMCStr)
    {
        str.Format(TM_T(" old:0x%X new:0x%X"), p->styleOld, p->styleNew);
    }

#ifdef CM_HITTEST
    inline static void FormatParam(TM::CStdStr& str, TM::TUIHITINFO* p, TMCStr ps)
    {
        str.Format(TM_T(" %s(hWnd:%X hHint:%X pt:%g,%g bNC:%d dwCode:0x%X pTaget:%X)"),
            ps, p->hWnd, p->hHint, PointX(p->pt), PointY(p->pt), p->bNC, p->dwCode, p->pTaget);
    }
    inline static void FormatParam(TM::CStdStr& str, TM::TUIDRAWITEM* p, TMCStr ps)
    {
        str.Format(TM_T(" CtlID:%d dwDrawStage:%X itemState:%X hWnd:%X hWndBind:%X hItem:%X rcItem:%g,%g,%g,%g itemData:%X"),
            p->CtlID, p->dwDrawStage, p->itemState, p->hWnd, p->hWndBind, p->hItem,
            RectLeft(p->rcItem), RectTop(p->rcItem),
            RectRight(p->rcItem), RectBottom(p->rcItem),
            p->itemData);
    }
    inline static void FormatParam(TM::CStdStr& str, TM::TUISETTEXT* p, TMCStr ps)
    {
        str.Format(TM_T(" bHtml:%d hModule:%X nIDStr:%d pStr:%s"),
            p->bHtml, p->hModule, p->nIDStr, p->str ? p->str->c_str() : TM_T(""));
    }

    inline static void FormatParam(TM::CStdStr& str, TM::TUIMSG* p, TMCStr ps)
    {
        if (p)
        {
            str.Format(TM_T(" hWnd:%X uObjectID:%d uID:%d uTag:%d "),
                p->hWnd, p->uObjectID, p->uID, p->uTag);
            str += done(p->message, p->wParam, p->lParam, 0);
        }
        else
            str.Format(TM_T(" %s:0"), ps);
    }

    template <class TWin>
    inline static TMCStr _visible_state(TWin win, TMUInt32 dwFlag)
    {
        static TM::CStdStr str;
        DWORD dwStyle = win.GetStyle();
        TMBool bVisible = win.IsWindow() ? (dwStyle & WS_VISIBLE) : false;
        if (bVisible && (dwFlag & SWP_HIDEWINDOW))
            str = TM_T("to_hide");
        else if (!bVisible && (dwFlag & SWP_SHOWWINDOW))
            str = TM_T("to_show");
        else if (bVisible)
            str = TM_T("visible");
        else
            str = TM_T("hide");
        if (dwStyle & WS_MAXIMIZE)
            str += TM_T(", maximize");
        else if (dwStyle & WS_MINIMIZE)
            str += TM_T(", minimize");
        return str;
    }

    inline static void FormatParam(TM::CStdStr& str, TMWINDOWPOS* p, TMCStr)
    {
        if (!p)
            return;
        str.Format(TM_T(" hwnd:0x%X hwndInsertAfter:0x%X x:%d y:%d cx:%d cy:%d flags:0x%X %s"),
            p->hwnd, p->hwndInsertAfter, p->x, p->y, p->cx, p->cy, p->flags, _visible_state(TM::CWindowAT(p->hwnd), p->flags));
    }

    inline static void FormatParam(TM::CStdStr& str, TM::TUIWINDOWPOS* p, TMCStr)
    {
        if (!p)
            return;
        TM::TUIWINDOWPOS* p2 = (TM::TUIWINDOWPOS*)p;
        str.Format(TM_T(" pWnd:0x%X pWndInsertAfter:0x%X x:%g y:%g cx:%g cy:%g flags:0x%X, %s"),
            p2->pWnd, p2->pWndInsertAfter, p2->x, p2->y, p2->cx, p2->cy, p2->flags, _visible_state(TM::CTuiWidget(p2->pWnd), p->flags));
    }
#else
    inline static void FormatParam(TM::CStdStr& str, TMWINDOWPOS* p, TMCStr)
    {
        str.Format(TM_T(" hwnd:0x%X hwndInsertAfter:0x%X x:%d y:%d cx:%d cy:%d flags:0x%X"),
            p->hwnd, p->hwndInsertAfter, p->x, p->y, p->cx, p->cy, p->flags);
    }
#endif



    template <class T> inline static TM::CStdStr PrintParam(T p, TMCStr ps)
    {
        TM::CStdStr str;
        FormatParam(str, p, ps);
        return str;
    }

    inline static TM::CStdStr PrintRet(TMResult* p)
    {
        TM::CStdStr str;
        if (p)
            str.Format(TM_T(" result:%d(0x%X)"), *p, *p);
        return str;
    }

    static TM::CStdStr done(TMUInt uMsg, TMWParam wp, TMLParam lp, TMResult* ret)
    {
        return done(0, uMsg, wp, lp, ret);
    }


    // #   define FORAMT_CASE4(m,w,ws,l,ls,r) case m: c = TM_T(#m); if (bFormatParam) c += PrintParam((w)wp, TM_T(ws)) + PrintParam((l)lp, TM_T(ls)); return c + PrintRet((r*)ret);
#  define FORAMT_CASE3(m,w,ws,l,ls) \
    case m: \
    { DbgAssertS(m < FORMAT_MAX_VAL && m >= FORMAT_MIN_VAL); } \
        c = TM_T(#m); \
        if (bFormatParam) \
            c += PrintParam((w)wp, TM_T(ws)) + PrintParam((l)lp, TM_T(ls)); \
        return c + PrintRet(ret);
#   define FORAMT_CASE(m) \
    case m: \
        {DbgAssertS(m < FORMAT_MAX_VAL && m >= FORMAT_MIN_VAL); } \
        c = TM_T(#m); \
        return c + PrintRet(ret);
#   ifdef TM_WIN
#       define FORAMT_CASEW                     FORAMT_CASE
#       define FORAMT_CASEW3                    FORAMT_CASE3
    // #       define FORAMT_CASEW4                    FORAMT_CASE4
#else
#       define FORAMT_CASEW(m)
#       define FORAMT_CASEW3(m,w,ws,l,ls)
    // #       define FORAMT_CASEW4(m,w,ws,l,ls,r)
#endif

    static TM::CStdStr doneNormal(HWND hWnd, TMUInt uMsg, TMWParam wp, TMLParam lp, TMResult* ret)
    {
        enum { FORMAT_MIN_VAL  = 0 };
        enum { FORMAT_MAX_VAL = WM_USER };

        TM::CStdStr c;

        switch (uMsg)
        {
            /*
             * Window Messages
             */
            FORAMT_CASE     (WM_NULL);
            FORAMT_CASE3    (WM_CREATE, _n, "", _h, "data");
            FORAMT_CASE     (WM_DESTROY);
            FORAMT_CASE3    (WM_MOVE, _n, "", _c, "pos");
            FORAMT_CASE3    (WM_SIZE, _n, "", _c, "size");

            FORAMT_CASE3    (WM_ACTIVATE, _h, "state", _h, "hWnd");

            FORAMT_CASE3    (WM_SETFOCUS, _h, "hLost", _n, "");
            FORAMT_CASE3    (WM_KILLFOCUS, _h, "hGet", _n, "");
            FORAMT_CASE3    (WM_ENABLE, _i, "state", _n, "");
            FORAMT_CASE3    (WM_SETREDRAW, _i, "redraw", _n, "");
            FORAMT_CASE3    (WM_SETTEXT, _n, "", TMCStr, "text");
            FORAMT_CASE3    (WM_GETTEXT, _i, "size", _h, "buffer");
            FORAMT_CASE     (WM_GETTEXTLENGTH);
            //FORAMT_CASE     (WM_PAINT);
        case WM_PAINT:
            {
                c = TM_T("WM_PAINT");
                if (hWnd)
                {
#ifdef TM_WIN_NO_QT
                    RECT rc;
                    GetUpdateRect(hWnd, &rc, false);
                    if (bFormatParam)
                        c += PrintParam(&rc, TM_T("rect"));
#endif
                }
            }
            return c + PrintRet(ret);

            FORAMT_CASE     (WM_CLOSE);
            FORAMT_CASE3    (WM_QUERYENDSESSION, _n, "", _i, "option");
            FORAMT_CASE     (WM_QUIT);
            FORAMT_CASE     (WM_QUERYOPEN);
            FORAMT_CASE3    (WM_ERASEBKGND, _h, "hDC", _n, "");
            FORAMT_CASE     (WM_SYSCOLORCHANGE);
            FORAMT_CASE3    (WM_ENDSESSION, _i, "EndOption", _i, "LogoffOption");
            FORAMT_CASE3    (WM_SHOWWINDOW, _i, "flag", _i, "status");
            FORAMT_CASE3    (WM_WININICHANGE, _n, "", TMCStr, "area");
//             #if (WINVER >= 0x0400)
//             FORAMT_CASE(WM_SETTINGCHANGE); == WM_WININICHANGE
//             #endif /* WINVER >= 0x0400 */


            FORAMT_CASE3    (WM_DEVMODECHANGE, _n, "", TMCStr, "device");
            FORAMT_CASE3    (WM_ACTIVATEAPP, _i, "flag", _i, "thread");
            FORAMT_CASE     (WM_FONTCHANGE);
            FORAMT_CASE     (WM_TIMECHANGE);

#ifdef CM_HITTEST
            FORAMT_CASE3    (WM_CANCELMODE, _i, "id", TM::TUIMSG*, "message");
#else
            FORAMT_CASE     (WM_CANCELMODE);
#endif
            FORAMT_CASE3    (WM_SETCURSOR, _h, "hWnd", _c, "hitcode&message");
            FORAMT_CASE3    (WM_MOUSEACTIVATE, _h, "hWnd", _i, "hitcode&message");
            FORAMT_CASE     (WM_CHILDACTIVATE);
            FORAMT_CASE     (WM_QUEUESYNC);

            FORAMT_CASEW3   (WM_GETMINMAXINFO, _n, "", MINMAXINFO*, "");
            FORAMT_CASE     (WM_PAINTICON);
            FORAMT_CASE3    (WM_ICONERASEBKGND, _h, "hDC", _n, "");
            FORAMT_CASE3    (WM_NEXTDLGCTL, _i, "id", _i, "how");
            FORAMT_CASE3    (WM_SPOOLERSTATUS, _i, "status", _i, "number");
            FORAMT_CASEW3   (WM_DRAWITEM, _i, "id", LPDRAWITEMSTRUCT, "");
            FORAMT_CASEW3   (WM_MEASUREITEM, _i, "id", LPMEASUREITEMSTRUCT, "");
            FORAMT_CASEW3   (WM_DELETEITEM, _i, "id", LPDELETEITEMSTRUCT, "");
            FORAMT_CASE3    (WM_VKEYTOITEM, _c, "key&caret", _h, "hWnd");
            FORAMT_CASE3    (WM_CHARTOITEM, _c, "key&caret", _h, "hWnd");
            FORAMT_CASE3    (WM_SETFONT, _h, "font", _i, "redraw");
            FORAMT_CASE     (WM_GETFONT);
            FORAMT_CASE3    (WM_SETHOTKEY, _c, "key&modifiers", _n, "");
            FORAMT_CASE     (WM_GETHOTKEY);
            FORAMT_CASE     (WM_QUERYDRAGICON);
            FORAMT_CASE(WM_COMPAREITEM);
            #if (WINVER >= 0x0500)
            FORAMT_CASE3    (WM_GETOBJECT, _h, "dwFlags", _objid, "dwObjId");
            #endif /* WINVER >= 0x0500 */
            FORAMT_CASE(WM_COMPACTING);
            FORAMT_CASE(WM_COMMNOTIFY);
            FORAMT_CASE3    (WM_WINDOWPOSCHANGING, _n, "", TMWINDOWPOS*, "");
#ifdef CM_WINDOWPOSCHANGED
            FORAMT_CASE3    (WM_WINDOWPOSCHANGED, TM::TUIWINDOWPOS*, "", TMWINDOWPOS*, "");
#else
            FORAMT_CASE3    (WM_WINDOWPOSCHANGED, _n, "", TMWINDOWPOS*, "");
#endif

            FORAMT_CASE(WM_POWER);

            FORAMT_CASE(WM_COPYDATA);
            FORAMT_CASE(WM_CANCELJOURNAL);


            FORAMT_CASE(WM_NOTIFY);
            FORAMT_CASE(WM_INPUTLANGCHANGEREQUEST);
            FORAMT_CASE(WM_INPUTLANGCHANGE);
            FORAMT_CASE(WM_TCARD);
            FORAMT_CASE(WM_HELP);
//             FORAMT_CASE(WM_USERCHANGED);
            FORAMT_CASE(WM_NOTIFYFORMAT);

            FORAMT_CASE(WM_CONTEXTMENU);
            FORAMT_CASE3    (WM_STYLECHANGING, _i, "type", TMSTYLESTRUCT*, "");
            FORAMT_CASE3    (WM_STYLECHANGED, _i, "type", TMSTYLESTRUCT*, "");
            FORAMT_CASE3    (WM_DISPLAYCHANGE, _i, "depth", _c, "size");
            FORAMT_CASE(WM_GETICON);
            FORAMT_CASE(WM_SETICON);

            FORAMT_CASE3    (WM_NCCREATE, _n, "", _h, "data");
            FORAMT_CASE     (WM_NCDESTROY);
            FORAMT_CASE3    (WM_NCCALCSIZE, _i, "flag", TMRectI*, "rcClient");
            FORAMT_CASE(WM_NCHITTEST);
            FORAMT_CASE3    (WM_NCPAINT, _h, "rgn", _n, "");
            FORAMT_CASE3    (WM_NCACTIVATE, _h, "state", _n, "");
            FORAMT_CASE(WM_GETDLGCODE);
            FORAMT_CASE     (WM_SYNCPAINT);
            FORAMT_CASE3    (WM_NCMOUSEMOVE, _h, "hit", _c, "pos");
            FORAMT_CASE3    (WM_NCLBUTTONDOWN, _h, "hit", _c, "pos");
            FORAMT_CASE3    (WM_NCLBUTTONUP, _h, "hit", _c, "pos");
            FORAMT_CASE3    (WM_NCLBUTTONDBLCLK, _h, "hit", _c, "pos");
            FORAMT_CASE3    (WM_NCRBUTTONDOWN, _h, "hit", _c, "pos");
            FORAMT_CASE3    (WM_NCRBUTTONUP, _h, "hit", _c, "pos");
            FORAMT_CASE3    (WM_NCRBUTTONDBLCLK, _h, "hit", _c, "pos");
            FORAMT_CASE3    (WM_NCMBUTTONDOWN, _h, "hit", _c, "pos");
            FORAMT_CASE3    (WM_NCMBUTTONUP, _h, "hit", _c, "pos");
            FORAMT_CASE3    (WM_NCMBUTTONDBLCLK, _h, "hit", _c, "pos");
            FORAMT_CASE(WM_NCMOUSELEAVE);

            FORAMT_CASEW(WM_INPUT);


    //        FORAMT_CASE(WM_KEYFIRST);
            FORAMT_CASE3    (WM_KEYDOWN, _h, "key", _h, "flag");
            FORAMT_CASE3    (WM_KEYUP, _h, "key", _h, "flag");
            FORAMT_CASE(WM_CHAR);
            FORAMT_CASE(WM_DEADCHAR);
            FORAMT_CASE3    (WM_SYSKEYDOWN, _h, "key", _h, "flag");
            FORAMT_CASE3    (WM_SYSKEYUP, _h, "key", _h, "flag");
            FORAMT_CASE(WM_SYSCHAR);
            FORAMT_CASE(WM_SYSDEADCHAR);
            FORAMT_CASE(WM_KEYLAST);

            #if (WINVER >= 0x0400)
            FORAMT_CASE3    (WM_IME_STARTCOMPOSITION, _n, "", _n, "");
            FORAMT_CASE3    (WM_IME_ENDCOMPOSITION, _n, "", _n, "");
            FORAMT_CASE3    (WM_IME_COMPOSITION, _h, "character", _h, "indicator");
    //        FORAMT_CASE(WM_IME_KEYLAST);
            #endif /* WINVER >= 0x0400 */

            FORAMT_CASE(WM_INITDIALOG);
            FORAMT_CASE3    (WM_COMMAND, _uc, "identifier&code", _h, "HWND");
            FORAMT_CASE3    (WM_SYSCOMMAND, _h, "code", _c, "pos");
            FORAMT_CASE(WM_TIMER);
            FORAMT_CASE(WM_SYSTIMER);
            FORAMT_CASE3    (WM_HSCROLL, _c, "pos&code", _h, "HWND");
            FORAMT_CASE3    (WM_VSCROLL, _c, "pos&code", _h, "HWND");
            FORAMT_CASE(WM_INITMENU);
            FORAMT_CASE(WM_INITMENUPOPUP);
            FORAMT_CASE(WM_MENUSELECT);
            FORAMT_CASE(WM_MENUCHAR);
            FORAMT_CASE(WM_ENTERIDLE);
            #if (WINVER >= 0x0500)
            FORAMT_CASE(WM_MENURBUTTONUP);
            FORAMT_CASE(WM_MENUDRAG);
            FORAMT_CASE(WM_MENUGETOBJECT);
            FORAMT_CASE(WM_UNINITMENUPOPUP);
            FORAMT_CASE(WM_MENUCOMMAND);
            #endif /* WINVER >= 0x0500 */

            FORAMT_CASE(WM_CHANGEUISTATE);
            FORAMT_CASE(WM_UPDATEUISTATE);
            FORAMT_CASE(WM_QUERYUISTATE);

            FORAMT_CASE(WM_CTLCOLORMSGBOX);
            FORAMT_CASE(WM_CTLCOLOREDIT);
            FORAMT_CASE(WM_CTLCOLORLISTBOX);
            FORAMT_CASE(WM_CTLCOLORBTN);
            FORAMT_CASE(WM_CTLCOLORDLG);
            FORAMT_CASE(WM_CTLCOLORSCROLLBAR);
            FORAMT_CASE(WM_CTLCOLORSTATIC);

            FORAMT_CASE(LB_ADDSTRING);
            FORAMT_CASE(LB_INSERTSTRING);
            FORAMT_CASE(LB_DELETESTRING);
            FORAMT_CASE(LB_SELITEMRANGEEX);
            FORAMT_CASE(LB_RESETCONTENT);
            FORAMT_CASE(LB_SETSEL);
            FORAMT_CASE(LB_SETCURSEL);
            FORAMT_CASE(LB_GETSEL);
            FORAMT_CASE(LB_GETCURSEL);
            FORAMT_CASE(LB_GETTEXT);
            FORAMT_CASE(LB_GETTEXTLEN);
            FORAMT_CASE(LB_GETCOUNT);
            FORAMT_CASE(LB_SELECTSTRING);
            FORAMT_CASE(LB_DIR);
            FORAMT_CASE(LB_GETTOPINDEX);
            FORAMT_CASE(LB_FINDSTRING);
            FORAMT_CASE(LB_GETSELCOUNT);
            FORAMT_CASE(LB_GETSELITEMS);
            FORAMT_CASE(LB_SETTABSTOPS);
            FORAMT_CASE(LB_GETHORIZONTALEXTENT);
            FORAMT_CASE(LB_SETHORIZONTALEXTENT);
            FORAMT_CASE(LB_SETCOLUMNWIDTH);
            FORAMT_CASE(LB_ADDFILE);
            FORAMT_CASE(LB_SETTOPINDEX);
            FORAMT_CASE(LB_GETITEMRECT);
            FORAMT_CASE(LB_GETITEMDATA);
            FORAMT_CASE(LB_SETITEMDATA);
            FORAMT_CASE(LB_SELITEMRANGE);
            FORAMT_CASE(LB_SETANCHORINDEX);
            FORAMT_CASE(LB_GETANCHORINDEX);
            FORAMT_CASE(LB_SETCARETINDEX);
            FORAMT_CASE(LB_GETCARETINDEX);
            FORAMT_CASE(LB_SETITEMHEIGHT);
            FORAMT_CASE(LB_GETITEMHEIGHT);
            FORAMT_CASE(LB_FINDSTRINGEXACT);
            FORAMT_CASE(LB_SETLOCALE);
            FORAMT_CASE(LB_GETLOCALE);
            FORAMT_CASE(LB_SETCOUNT);
            FORAMT_CASE(LB_INITSTORAGE);
            FORAMT_CASE(LB_ITEMFROMPOINT);
            FORAMT_CASE(LB_MSGMAX);


    //        FORAMT_CASE(WM_MOUSEFIRST);
            FORAMT_CASE3    (WM_MOUSEMOVE, _h, "keyindicators", _c, "pos");
            FORAMT_CASE3    (WM_LBUTTONDOWN, _h, "keyindicators", _c, "pos");
            FORAMT_CASE3    (WM_LBUTTONUP, _h, "keyindicators", _c, "pos");
            FORAMT_CASE3    (WM_LBUTTONDBLCLK, _h, "keyindicators", _c, "pos");
            FORAMT_CASE3    (WM_RBUTTONDOWN, _h, "keyindicators", _c, "pos");
            FORAMT_CASE3    (WM_RBUTTONUP, _h, "keyindicators", _c, "pos");
            FORAMT_CASE3    (WM_RBUTTONDBLCLK, _h, "keyindicators", _c, "pos");
            FORAMT_CASE3    (WM_MBUTTONDOWN, _h, "keyindicators", _c, "pos");
            FORAMT_CASE3    (WM_MBUTTONUP, _h, "keyindicators", _c, "pos");
            FORAMT_CASE3    (WM_MBUTTONDBLCLK, _h, "keyindicators", _c, "pos");

            FORAMT_CASE3    (WM_MOUSEWHEEL, _c, "keyindicators&wheel", _c, "pos");
    //        FORAMT_CASE(WM_MOUSELAST);

            FORAMT_CASE(WM_PARENTNOTIFY);
            FORAMT_CASE(WM_ENTERMENULOOP);
            FORAMT_CASE(WM_EXITMENULOOP);

            FORAMT_CASE(WM_NEXTMENU);
            FORAMT_CASE3    (WM_SIZING, _i, "edge", TMRectI*, "pos");
            FORAMT_CASE3    (WM_CAPTURECHANGED, _n, "", _h, "hCapture");
            FORAMT_CASE3    (WM_MOVING, _i, "wparam", TMRectI*, "pos");
            FORAMT_CASE(WM_POWERBROADCAST);
            FORAMT_CASE3    (WM_DEVICECHANGE, _h, "event", _h, "data");

            FORAMT_CASE(WM_MDICREATE);
            FORAMT_CASE(WM_MDIDESTROY);
            FORAMT_CASE(WM_MDIACTIVATE);
            FORAMT_CASE(WM_MDIRESTORE);
            FORAMT_CASE(WM_MDINEXT);
            FORAMT_CASE(WM_MDIMAXIMIZE);
            FORAMT_CASE(WM_MDITILE);
            FORAMT_CASE(WM_MDICASCADE);
            FORAMT_CASE(WM_MDIICONARRANGE);
            FORAMT_CASE(WM_MDIGETACTIVE);


            FORAMT_CASE(WM_MDISETMENU);
            FORAMT_CASE(WM_ENTERSIZEMOVE);
            FORAMT_CASE(WM_EXITSIZEMOVE);
            FORAMT_CASE(WM_DROPFILES);
            FORAMT_CASE(WM_MDIREFRESHMENU);
#ifdef TM_WIN
//             #if (WINVER >= 0x0602)
            FORAMT_CASE(WM_POINTERDEVICECHANGE);
            FORAMT_CASE(WM_POINTERDEVICEINRANGE);
            FORAMT_CASE(WM_POINTERDEVICEOUTOFRANGE);
//             #endif /* WINVER >= 0x0602 */


//             #if (WINVER >= 0x0601)
            FORAMT_CASE3    (WM_TOUCH, _i, "num", _h, "hTouch");
            FORAMT_CASE3    (WM_GESTURE, _i, "ullArguments", _h, "hGesture");
            FORAMT_CASE3    (WM_GESTURENOTIFY, _n, "", _h, "GESTURENOTIFYSTRUCT*");
//             #endif /* WINVER >= 0x0601 */

//             #if (WINVER >= 0x0602)
            FORAMT_CASE3     (WM_NCPOINTERUPDATE, _h, "info", _c, "pos");
            FORAMT_CASE3     (WM_NCPOINTERDOWN, _h, "info", _c, "pos");
            FORAMT_CASE3     (WM_NCPOINTERUP, _h, "info", _c, "pos");
            FORAMT_CASE3     (WM_POINTERUPDATE, _h, "info", _c, "pos");
            FORAMT_CASE3     (WM_POINTERDOWN, _h, "info", _c, "pos");
            FORAMT_CASE3     (WM_POINTERUP, _h, "info", _c, "pos");
            FORAMT_CASE3     (WM_POINTERENTER, _h, "info", _c, "pos");
            FORAMT_CASE3     (WM_POINTERLEAVE, _h, "info", _c, "pos");
            FORAMT_CASE3     (WM_POINTERACTIVATE, _h, "info", _h, "hActive");
            FORAMT_CASE3     (WM_POINTERCAPTURECHANGED, _h, "info", _h, "hCapture");
            FORAMT_CASE3     (WM_TOUCHHITTESTING, _h, "wp", _h, "lp");
            FORAMT_CASE3     (WM_POINTERWHEEL, _h, "info", _c, "pos");
            FORAMT_CASE3     (WM_POINTERHWHEEL, _h, "info", _c, "pos");
            FORAMT_CASE      (DM_POINTERHITTEST);
//             #endif /* WINVER >= 0x0602 */

            #if (WINVER >= 0x0400)
            FORAMT_CASE3    (WM_IME_SETCONTEXT, _i, "bActive", _h, "options");
            FORAMT_CASE3    (WM_IME_NOTIFY, _h, "command", _h, "data");
            FORAMT_CASE3    (WM_IME_CONTROL, _h, "command", _h, "data");
            FORAMT_CASE3    (WM_IME_COMPOSITIONFULL, _n, "", _n, "");
            FORAMT_CASE3    (WM_IME_SELECT, _i, "bSelected", _h, "identifier");
            FORAMT_CASE3    (WM_IME_CHAR, _h, "character", _h, "indicators");
            #endif /* WINVER >= 0x0400 */
            #if (WINVER >= 0x0500)
            FORAMT_CASE3    (WM_IME_REQUEST, _h, "command", _h, "data");
            #endif /* WINVER >= 0x0500 */
            #if (WINVER >= 0x0400)
            FORAMT_CASE(WM_IME_KEYDOWN);
            FORAMT_CASE(WM_IME_KEYUP);
            #endif /* WINVER >= 0x0400 */
#endif

            FORAMT_CASE(WM_MOUSEHOVER);
            FORAMT_CASE(WM_MOUSELEAVE);


#ifdef WM_DPICHANGED
            FORAMT_CASE3(WM_DPICHANGED, _c, "xdpi&ydpi", TMRectI*, "rect");
#endif

#ifdef WM_GETDPISCALEDSIZE
            FORAMT_CASE3(WM_GETDPISCALEDSIZE, _i, "dpi", _c, "newsize");
#endif


            FORAMT_CASE(WM_CUT);
            FORAMT_CASE(WM_COPY);
            FORAMT_CASE(WM_PASTE);
            FORAMT_CASE(WM_CLEAR);
            FORAMT_CASE(WM_UNDO);
            FORAMT_CASE(WM_RENDERFORMAT);
            FORAMT_CASE(WM_RENDERALLFORMATS);
            FORAMT_CASE(WM_DESTROYCLIPBOARD);
            FORAMT_CASE(WM_DRAWCLIPBOARD);
            FORAMT_CASE(WM_PAINTCLIPBOARD);
            FORAMT_CASE(WM_VSCROLLCLIPBOARD);
            FORAMT_CASE(WM_SIZECLIPBOARD);
            FORAMT_CASE(WM_ASKCBFORMATNAME);
            FORAMT_CASE(WM_CHANGECBCHAIN);
            FORAMT_CASE(WM_HSCROLLCLIPBOARD);
            FORAMT_CASE(WM_QUERYNEWPALETTE);
            FORAMT_CASE(WM_PALETTEISCHANGING);
            FORAMT_CASE(WM_PALETTECHANGED);
            FORAMT_CASE(WM_HOTKEY);
            FORAMT_CASE(WM_POPUPSYSTEMMENU);

            FORAMT_CASE3    (WM_PRINT, _h, "hDC", _h, "options");
            FORAMT_CASE3    (WM_PRINTCLIENT, _h, "hDC", _h, "options");

            FORAMT_CASE(WM_DWMCOMPOSITIONCHANGED);
            FORAMT_CASEW(WM_DWMNCRENDERINGCHANGED);
            FORAMT_CASEW(WM_DWMCOLORIZATIONCOLORCHANGED);
            FORAMT_CASEW(WM_DWMWINDOWMAXIMIZEDCHANGE);

//             FORAMT_CASE(WM_HANDHELDFIRST);
//             FORAMT_CASE(WM_HANDHELDLAST);
//
//             FORAMT_CASE(WM_AFXFIRST);
//             FORAMT_CASE(WM_AFXLAST);
//             //#endif /* WINVER >= 0x0400 */
//
//             FORAMT_CASE(WM_PENWINFIRST);
//             FORAMT_CASE(WM_PENWINLAST);


//             FORAMT_CASE(WM_USER);
//             FORAMT_CASE(WM_APP);

//
            // button
//            FORAMT_CASE3    (BM_SETSTATE, _i, "state", _n, "");
            FORAMT_CASE(BM_GETCHECK);
            FORAMT_CASE3    (BM_SETCHECK, _i, "state", _n, "");
            FORAMT_CASE(BM_CLICK);

            // edit,memo,rich
            FORAMT_CASE(EM_GETSEL);
            FORAMT_CASE(EM_SETSEL);
            FORAMT_CASE(EM_GETMODIFY);
            FORAMT_CASE(EM_SETMODIFY);
            FORAMT_CASE(EM_GETLINECOUNT);
            FORAMT_CASE(EM_LINEINDEX);
            FORAMT_CASE(EM_LINELENGTH);
            FORAMT_CASE(EM_REPLACESEL);
            FORAMT_CASE(EM_GETLINE);
            FORAMT_CASE(EM_LIMITTEXT);
            FORAMT_CASE(EM_CANUNDO);
            FORAMT_CASE(EM_UNDO);
            FORAMT_CASE(EM_LINEFROMCHAR);
            FORAMT_CASE(EM_SETTABSTOPS);
            FORAMT_CASE(EM_SETPASSWORDCHAR);
            FORAMT_CASE(EM_EMPTYUNDOBUFFER);
            FORAMT_CASE(EM_GETFIRSTVISIBLELINE);
            FORAMT_CASE(EM_SETREADONLY);
            FORAMT_CASE(EM_GETPASSWORDCHAR);
            FORAMT_CASE(EM_GETLIMITTEXT);
            FORAMT_CASE(EM_POSFROMCHAR);
            FORAMT_CASE(EM_CHARFROMPOS);
            FORAMT_CASE(EM_SETIMESTATUS);
            FORAMT_CASE(EM_GETIMESTATUS);

            default:
                return c;
            };
    }

    static TM::CStdStr doneTMDui(HWND hWnd, TMUInt uMsg, TMWParam wp, TMLParam lp, TMResult* ret)
    {
        TM::CStdStr c;

#ifdef CM_BASE
        enum { FORMAT_MIN_VAL = CM_BASE };
        enum { FORMAT_MAX_VAL = CM_END };

        switch (uMsg)
        {
            FORAMT_CASE3    (CM_HITTEST, _i, "message", TM::TUIHITINFO*, "");

            FORAMT_CASE3    (CM_MOUSEENTRY, TM::TUIHITINFO*, "old", TM::TUIHITINFO*, "new");
            FORAMT_CASE3    (CM_NCMOUSEENTRY, TM::TUIHITINFO*, "old", TM::TUIHITINFO*, "new");
            FORAMT_CASE3    (CM_NCMOUSELEAVE, TM::TUIHITINFO*, "old", TM::TUIHITINFO*, "new");
            //FORAMT_CASE(CM_MOUSEDRAG            0x02A7 //after call StartDrag,system use WM_MOUSEDRAG instead of WM_MOUSEMOVE.
            FORAMT_CASE3    (CM_MOUSESWITCH, TM::TUIHITINFO*, "old", TM::TUIHITINFO*, "new");

            FORAMT_CASE(CM_NCSETCURSOR);

            FORAMT_CASE(CM_GESTURENOTIFY);
            FORAMT_CASE(CM_GESTURE);

            FORAMT_CASE3    (CM_SETBKPIC, _n, "", _h, "pic");
            FORAMT_CASE3    (CM_SETBKPIC2, _i, "id", _h, "module");
            FORAMT_CASE     (CM_GETBKPIC);
            FORAMT_CASE     (CM_SETFOREPIC);
            FORAMT_CASE     (CM_GETFOREPIC);

            FORAMT_CASE3    (CM_SETTEXT, _n, "",  TM::TUISETTEXT*, "text");
            FORAMT_CASE3    (CM_SETACCTEXT, _n, "",  TM::TUISETTEXT*, "text");
            FORAMT_CASE3    (CM_SETHINT, _n, "", TM::TUISETTEXT*, "text");
            FORAMT_CASE3    (CM_ACTIVEHINT, _n, "", _i, "active");
            FORAMT_CASE3    (CM_SETTEXTCOLOR, _i, "state", _h, "color");
            FORAMT_CASE3    (CM_SETITEMHINT, _i, "index", TM::TUISETTEXT*, "text");
            FORAMT_CASE3    (CM_GETITEMHINT, _i, "index", _h, "text");
            FORAMT_CASE3    (CM_SETITEMACCTEXT, _i, "index", TM::TUISETTEXT*, "text");
            FORAMT_CASE3    (CM_GETITEMACCTEXT, _i, "index", _n, "");
            FORAMT_CASE(CM_SETOUCAPTIONBKPIC);
            FORAMT_CASE(CM_SETFRAMEPIC);
    //         FORAMT_CASE(CM_SETSYSICONDISABLED);

            FORAMT_CASE     (CM_GETSTATE);
            FORAMT_CASE     (CM_SETGROUPDFTTABSTOP);
            //             FORAMT_CASE3    (CM_SETSTATE, _i, "state", _n, "");
            FORAMT_CASE3    (CM_GETSCROLLINFO, _h, "h_info", _h, "v_info");
            FORAMT_CASE(CM_GETSCROLLBARINFO);
            FORAMT_CASE3    (CM_HSCROLL, _i, "scroll", _i, "thumb");
            FORAMT_CASE3    (CM_VSCROLL, _i, "scroll", _i, "thumb");
            FORAMT_CASE3    (CM_SCROLLBY, GXDips*, "x", GXDips*, "y");
            FORAMT_CASE3    (CM_SCROLLTO, GXDips*, "x", GXDips*, "y");
            FORAMT_CASE3    (CM_SETSCROLLRANGE, GXDips*, "w", GXDips*, "h");
            FORAMT_CASE(CM_LANGUAGECHANGED);
            FORAMT_CASE(CM_COLORVISUAL);

            FORAMT_CASE3    (CM_SETDPI, _i, "new", _n, "");
            FORAMT_CASE3    (CM_ZOOMED, _i, "new", _i, "old");

            FORAMT_CASE(CM_RENDERCHANGED);

//            FORAMT_CASE(   CM_MSGROUTE);
            FORAMT_CASE3    (CM_MSGROUTE, _n, "", TM::TUIMSG*, "message");
            FORAMT_CASE (CM_APPMESSAGE);
            FORAMT_CASE3    (CM_MSGBUBBLE, _n, "", TM::TUIMSG*, "message");

            FORAMT_CASE3    (CM_DRAWITEM, _n, "", TM::TUIDRAWITEM*, "");
            FORAMT_CASE3    (CM_SETDELETEDATA, _n, "", _h, "proc");
            FORAMT_CASE3    (CM_SETPFNBINDBACKUP, _n, "", _h, "proc");

//             FORAMT_CASE(CM_DRAWPIC);

            FORAMT_CASE(CM_DRAGENTRY);
            FORAMT_CASE(CM_DRAGOVER);
            FORAMT_CASE(CM_DRAGDROP);
            FORAMT_CASE(CM_DRAGLEAVE);

            FORAMT_CASE3    (CM_SHOWWINDOW_P, _i, "flag", _n, "");
            FORAMT_CASE3    (CM_ENABLE_P, _i, "state", _n, "");
            FORAMT_CASE3    (CM_SETPARENT_P, _h, "oldparent", _h, "hWgt");

            FORAMT_CASE     (CM_INVALIDATE);

    //        FORAMT_CASE(CM_POSTMESSAGE);
    //        FORAMT_CASE(CM_POSTMESSAGENOPARAM);

            FORAMT_CASE     (CM_GET_PADING);
            FORAMT_CASE     (CM_GET_MARGIN);
            FORAMT_CASE3    (CM_SET_PADING, TM::CBorder*, "pading", _n, "");
            FORAMT_CASE3    (CM_SET_MARGIN, TM::CBorder*, "margin", _n, "");
            FORAMT_CASE(CM_SETMINMAXINFO);
            FORAMT_CASE(CM_GETMINMAXINFO);

            FORAMT_CASE(CM_INITMENUPOP);
            FORAMT_CASE(CM_UNINITMENUPOP);
            FORAMT_CASE(CM_BEGINMENUPOP);
            FORAMT_CASE(CM_ENDMENUPOP);

            FORAMT_CASE(CM_SETFONT);
            FORAMT_CASE(CM_GETFONT);

            FORAMT_CASE3    (CM_PRECOMMAND, _uc, "identifier&code", _h, "HWND");


            FORAMT_CASE3    (CM_PRINTCLIENT, _h, "gxapp", _h, "options");


//            FORAMT_CASE(CM_ACC_POST_NOTIFY);
            FORAMT_CASE(CM_ACC_POST_KEYDOWN);
            FORAMT_CASE(CM_ACC_ALERT_EVENT);
//             FORAMT_CASE(CM_ACC_POST_READ_TEXT);
//             FORAMT_CASE(CM_ACC_POST_FORCE_NOTIFY);
            FORAMT_CASE(CM_ACC_GET_DESCRIPTION);
//            FORAMT_CASE(CM_ACC_GET_OBJECT);

//             FORAMT_CASE(CM_HINT_INIT);
//             FORAMT_CASE(CM_ISBINDWINDOW);

            // progress, up-down, track-bar, scroll, video, animate;
            FORAMT_CASE(CM_GETPOS);
            FORAMT_CASE(CM_SETPOS);
            FORAMT_CASE(CM_DELTAPOS);
            FORAMT_CASE(CM_GETRANGE);
            FORAMT_CASE(CM_SETRANGE);
            FORAMT_CASE(CM_GETSTEP);
            FORAMT_CASE(CM_SETSTEP);
            FORAMT_CASE(CM_STEPIT);

            // progress, up-down, track-bar, arrow, tab-button;
            FORAMT_CASE(CM_GETBUDDY);
            FORAMT_CASE(CM_SETBUDDY);

            // progress, up-down, track-bar;
            FORAMT_CASE(CM_GETBUDDYID);
            FORAMT_CASE(CM_SETBUDDYID);
            FORAMT_CASE(CM_SETFORMAT);

            // track-bar
            FORAMT_CASE(TBM_SETLEFTPIC)
            FORAMT_CASE(TBM_SETRIGHTPIC)
            FORAMT_CASE(TBM_SETTRCKBTNPIC)

            // dummy, float, activex, web-view
            FORAMT_CASE(CM_GETWINDOW);

            // dummy
            FORAMT_CASE(CDM_SETWINDOW);
            FORAMT_CASE(CDM_GETDUMMYINFO);

            // button, hotkey
            FORAMT_CASE3    (CM_SETHOTKEY, _i, "index", _n, "");
            FORAMT_CASE3    (CM_GETHOTKEY, _i, "index", _n, "");

            // radio,tabbutton
            FORAMT_CASE     (CBM_GETCHECKEDBUTTON)
            FORAMT_CASE     (CBM_GETNEXTBUTTON)
            FORAMT_CASE     (CBM_GETPREVBUTTON)
            FORAMT_CASE(CBM_SETFOREPICWIDTH);
            FORAMT_CASE(CBM_GETBUTTONSTATE);
            FORAMT_CASE(CBM_GETBUTTONSTATECOUNT);
            FORAMT_CASE(CBM_GETALIGNMODE);
            FORAMT_CASE(CBM_SETALIGNMODE);
            FORAMT_CASE(CBM_STATECHANGED);

            // combo
            FORAMT_CASE     (CCB_GETBUTTON)
            FORAMT_CASE     (CCB_GETEDIT)
            FORAMT_CASE     (CCB_GETPOPDLG)
            FORAMT_CASE     (CCB_GETPOPLIST)
            FORAMT_CASE     (CCB_FILTER)
            FORAMT_CASE     (CCB_ISDROPDOWN)
            FORAMT_CASE     (CCB_SUBMIT)
            FORAMT_CASE     (CCB_SETREFERID)

            // list box
            FORAMT_CASE3    (CLB_SETTEXT, _i, "index", TM::TUISETTEXT*, "text");
            FORAMT_CASE3    (CLB_GETTEXT, _i, "index", _n, "");
            FORAMT_CASE3    (CLB_ISITEMENABLE, _i, "index", _n, "");
            FORAMT_CASE3    (CLB_ENABLEITEM, _i, "index", _i, "bEnable");
            FORAMT_CASE3    (CLB_BINDWIN, _i, "index", _h, "hWgt");
            FORAMT_CASE3    (CLB_CREATEBINDWIN, _i, "index", _i, "oid");
            FORAMT_CASE3    (CLB_DETACHBINDWIN, _i, "index", _h, "hParent");
            FORAMT_CASE3    (CLB_GETBINDWIN, _i, "index", _n, "");
            FORAMT_CASE3    (CLB_SORT, _h, "pfn", _h, "key");
            FORAMT_CASE3    (CLB_DOWNCOLOR, _h, "color", _n, "");

            // edit,memo,rich
            FORAMT_CASE(CEM_REPLACESELHTML);
            FORAMT_CASE(CEM_GETSELHTML);
            FORAMT_CASE(CEM_SETPROMPT);
            FORAMT_CASE(CEM_GETCHARINFO);
            FORAMT_CASE(CEM_GETCHARUNIT);


            // activeX
            FORAMT_CASE     (CAX_GETHOST);
            FORAMT_CASE     (CAX_GETCONTROL);
            FORAMT_CASE3    (CAX_SETEXT, _h, "extUnknown", _n, "");
            FORAMT_CASE3    (CAX_BINDOBJECT, _n, "", _n, "");
            FORAMT_CASE3    (CAX_ONOBJINIT, _n, "", _n, "");

            // webview
            FORAMT_CASE3    (CWV_SETZOOM            , _n, "", double*, "zoom");
            FORAMT_CASE3    (CWV_GETZOOM            , _n, "", _h, "zoom*");
            FORAMT_CASE3    (CWV_GETSOUCE           , _n, "", _h, "CStrPtr*");
            FORAMT_CASE3    (CWV_NAVIGATE           , TMCStrW, "url", TMCStrW, "head");
            FORAMT_CASE3    (CWV_NAVIGATETOSTRING   , _n, "", TMCStrW, "html");
            FORAMT_CASE3    (CWV_ADDDOCSCRIPT       , _n, "", TMCStrW, "js");
            FORAMT_CASE3    (CWV_EXECSCRIPT         , _n, "", TMCStrW, "js");
            FORAMT_CASE     (CWV_RELOAD);
            FORAMT_CASE     (CWV_GETPID);
            FORAMT_CASE     (CWV_CANGOBACK);
            FORAMT_CASE     (CWV_CANGOFORWARD);
            FORAMT_CASE     (CWV_GOBACK);
            FORAMT_CASE     (CWV_GOFORWARD);
            FORAMT_CASE     (CWV_STOP);
            FORAMT_CASE3    (CWV_GETTITLE           , _n, "", _h, "CStrPtr*");

            FORAMT_CASE3    (CWV_ONWEBMESSAGE       , _n, "", TMCStrW, "message");
            FORAMT_CASE3    (CWV_ONNAVIGATESTART    , _n, "", TMCStrW, "url");
            FORAMT_CASE3    (CWV_ONTITLECHANGE      , _n, "", TMCStrW, "title");
            FORAMT_CASE3    (CWV_ONCOMMANDSTATE     , _i, "command", _i, "bEnable");
            FORAMT_CASE     (CWV_ONNAVIGATECOMPLETE);
            FORAMT_CASE3    (CWV_ONNAVIGATEERROR    , _n, "", _i, "errCode");
            FORAMT_CASE3    (CWV_ONFULLSCREEN       , _n, "", _i, "bFullScreen");
            FORAMT_CASE3    (CWV_ONNEWWINDOW        , TMCStrW, "", _h, "HTUI**");
//             FORAMT_CASE     (CWV_ONQUIT);
            FORAMT_CASE3    (CWV_ONWINDOWCLOSING    , _n, "", _n, "");

            FORAMT_CASE3    (CWV_ONCRGETFOLDER      , _h, "browserExecutableFolder", _h, "userDataFolder");

            FORAMT_CASE3    (CWV_ONIEEXEC           , _i, "CmdID", _h, "document");
            FORAMT_CASE3    (CWV_ONIEDLCONTROL      , _n, "", _h, "flag*");
            FORAMT_CASE3    (CWV_ONIEDOCCOMPLETE    , _n, "", _h, "document");
            FORAMT_CASE     (CWV_ONIEDOWNBEGIN);
            FORAMT_CASE     (CWV_ONIEDOWNCOMPLETE);
            FORAMT_CASE     (CWV_ONIEFILEDOWNLOAD);
            FORAMT_CASE3    (CWV_ONIESTATUSBAR      , _n, "", _i, "bShow");
            FORAMT_CASE3    (CWV_ONIETHEATERMODE    , _n, "", _i, "bTheaterMode");
            FORAMT_CASE3    (CWV_ONIEPROGRESSCHANGE , _i, "Progress", _i, "ProgressMax");
            FORAMT_CASE3    (CWV_ONIEPROPCHANGE     , _n, "", TMCStrW, "Property");
            FORAMT_CASE3    (CWV_ONIESTATUETEXT     , _n, "", TMCStrW, "text");

            // private
            FORAMT_CASE     (CM_PRI_DIRTY);
            FORAMT_CASE     (CM_PRI_POSCHANGING);

            // mgr, float
            FORAMT_CASE3    (CM_SETALPHA, _n, "", _i, "alpha");
            FORAMT_CASE3    (CM_SETINTERVAL, _n, "", _i, "interval");
            FORAMT_CASE     (CM_GET_BK_TEX);

            // mgr
            FORAMT_CASE(CM_SET_THEME_TYPE);
            FORAMT_CASE(CM_GET_MENUBAR);
            FORAMT_CASE(CM_GET_WIDGET);
            FORAMT_CASE3    (CM_SETDEFAULTITEM, _n, "", _i, "id");
            FORAMT_CASE3    (CM_SETDEFAULTFOCUS, _n, "", _h, "HTUI");

            // menu bar
            FORAMT_CASE(MB_GETMENU);
            FORAMT_CASE3    (MB_SETMENU, _h, "HODULE", _h, "pResID");
            FORAMT_CASE3    (MB_SETHMENU, _n, "", _h, "hmenu");
//            FORAMT_CASE(MB_GETCURSEL);
            FORAMT_CASE3    (MB_GETMENUINFO, _i, "nid", _h, "info");
            FORAMT_CASE(MB_GETITEMCOUNT);
            FORAMT_CASE3    (MB_SETALTMODE, _i, "bAlt", _n, "");

            // animate
            FORAMT_CASE(CACM_PLAY);
            FORAMT_CASE(CACM_STOP);
            FORAMT_CASE(CACM_SEEK);
            FORAMT_CASE(CACM_SEEKCUR);
            FORAMT_CASE(CACM_GETPOS);
            FORAMT_CASE(CACM_GETRANGE);
            FORAMT_CASE(CACM_SETRANGE);
            FORAMT_CASE(CACM_SETREPEAT);
            FORAMT_CASE(CACM_GETREPEAT);


            default:
                {
                    if (uMsg >= WM_USER && uMsg < WM_APP)
                        c.Format(TM_T("WM_USER + %d"), uMsg - WM_USER);
#ifdef CM_HITTEST
                    else if (uMsg >= CM_PRI_BASE && uMsg < CM_PRI_END)
                        c.Format(TM_T("CM_PRI_BASE + 0x%X"), uMsg - CM_PRI_BASE);
#endif
                    else if (uMsg >= 0xC000 && uMsg <= 0xFFFF)
                    {
                        TMCharT d[1024] = {0};
                        GetClipboardFormatName(uMsg, d, tm_countof(d));
                        c.Format(TM_T("RegisterWindowMessage: 0x%X(%s)"), uMsg, d);
                    }
                    else if (uMsg > WM_APP)
                        c.Format(TM_T("WM_APP + 0x%X"), uMsg - WM_APP);
                    else
                        c.Format(TM_T("unkown: 0x%X"), uMsg);

                    if (bFormatParam)
                    {
                        TM::CStdStr str;
                        str.Format(TM_T(", wParam:0x%X, lParam:x%X"), wp, lp);
                        c += str;
                    }

                    return c + PrintRet(ret);
                }
                break;
            };

#else   // #ifdef CM_BASE
        return c;
#endif  // #ifdef CM_BASE
    }

    static TM::CStdStr done(HWND hWnd, TMUInt uMsg, TMWParam wp, TMLParam lp, TMResult* ret)
    {
        TM::CStdStr c;

        if (uMsg == WM_WINDOWPOSCHANGED && wp)
            lp = 0;

        if (uMsg < WM_USER)
        {
            c = doneNormal(hWnd, uMsg, wp, lp, ret);
            if (uMsg == WM_CREATE || uMsg == WM_NCCREATE)
            {
                c += L" ";
                c += DbgFormatWindow(hWnd);
            }
        }
#ifdef CM_END
        else if (uMsg >= CM_BASE && uMsg < CM_END)
            c = doneTMDui(hWnd, uMsg, wp, lp, ret);
#endif
        if (!c.IsEmpty())
            return c;


        if (uMsg >= WM_USER && uMsg < WM_APP)
            c.Format(TM_T("WM_USER + %d"), uMsg - WM_USER);
#ifdef CM_HITTEST
        else if (uMsg >= CM_PRI_BASE && uMsg < CM_PRI_END)
            c.Format(TM_T("CM_PRI_BASE + 0x%X"), uMsg - CM_PRI_BASE);
#endif
        else if (uMsg >= 0xC000 && uMsg <= 0xFFFF)
        {
            TMCharT d[1024] = {0};
            GetClipboardFormatName(uMsg, d, tm_countof(d));
            c.Format(TM_T("RegisterWindowMessage: 0x%X(%s)"), uMsg, d);
        }
        else if (uMsg > WM_APP)
            c.Format(TM_T("WM_APP + 0x%X"), uMsg - WM_APP);
        else
            c.Format(TM_T("unkown: 0x%X"), uMsg);

        if (bFormatParam)
        {
            TM::CStdStr str;
            str.Format(TM_T(", wParam:0x%X, lParam:x%X"), wp, lp);
            c += str;
        }

        return c + PrintRet(ret);
    }

#   undef FORAMT_CASE
#   undef FORAMT_CASE3
// #   undef FORAMT_CASE4
};

inline static TM::CStdStr DbgFormatMessage(TMUInt uMsg)
{
    return CFmtMessageImpl<false>::done(uMsg, 0, 0, 0);

}

inline static TM::CStdStr DbgFormatMessage(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult* ret = 0)
{
    return CFmtMessageImpl<true>::done(uMsg, wParam, lParam, ret);
}

inline static TM::CStdStr DbgFormatMessage(HWND hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult* ret = 0)
{
    return CFmtMessageImpl<true>::done(hWnd, uMsg, wParam, lParam, ret);
}




#endif // TMFORMATMSG_H
