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

#ifndef UIWIDGET_H
#define UIWIDGET_H

#include "../tmdui.h"
#include "../tmlist.h"
#include "../tmstring.h"
#include "../tmsymbol.h"
#include "../tmautoptr.h"
#include "../tmtree.h"
#include "../tmgdix.h"
#include "../tmduiacc.h"
#include "object.h"
#include "module.h"
#include "pic.h"
#include "frame.h"
// #include "menu.h"
#include "subclass.h"
#include "scroll.h"
#include "property.h"
#include "anilist.h"


// debug view see instance:
//  CInstanceCountT<TM::CTuiWnd_Win>::s_nCount

#define KEYBOARD_MENU           ((TMLParam)-1)  //  Keyboard generated menu on WM_CONTEXTMENU

#define ASSERT_CLASS(x)         DbgAssert(x && x->IC_IsValidInstance(true))

#define ENTRY_LIFE_MAP(x)       if (!x->IsLifeInvalid()) {
#define LEAVE_LIFE_MAP()        }


#define DECALRE_MSG_MAP_WGT(cls,name) \
public: \
    void Subclass_##name(HTUI h) { h->Subclass(CWidgetMsgDelegate(this, _Proc_##name)); } \
    TMBool Unsubclass_##name(HTUI h) { return h->GetProcs().Unsubclass(this, CWidgetMsgDelegate(this, _Proc_##name)); } \
    static TMBool _Proc_##name(cls* p, HTUI h, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)

#define DECALRE_MSG_MAP_WIN(cls,name) \
public: \
    void Subclass_##name(HTUI h) { CTuiWindow(h).SubclassMgr(CWindowMsgDelegate(this, _Proc_##name)); } \
    TMBool Unsubclass_##name(HTUI h) { return CTuiWindow(h).UnsubclassMgr(CWindowMsgDelegate(this, _Proc_##name)); } \
    static TMBool _Proc_##name(cls* p, HWND h, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)


TM_BGN_NAMESPACE


class CTuiWgt;
class CTuiWnd;
class CWidgetHelper;
struct ITuiPic;
class CFrame;
class CView;
// struct CWalkerItem;
typedef CRefPtr<CTuiWgt> Ref;
template <class T, class _Rc> struct CTuiWidgetPosT;
typedef CTuiWidgetPosT<TUIWINDOWPOS, CTuiRect>  CTuiWidgetPos;
typedef CTuiWidgetPosT<TMWINDOWPOS, CRect>      CWndWindowPos;
template <class T> inline void _CheckRef(T* p) { if (p) DbgAssert(p->m_nRef > 0); }
tm_enum(ETTActive);


///////////////////////////////////////////////////////////////////////////////
// set to m_dwPriStyle
const TMUInt32 WS_PRI_FRAMECHANGED      = 0x10000000;   // frame changed
const TMUInt32 WS_PRI_DIRTY             = 0x20000000;
const TMUInt32 WS_PRI_DISABLEREDRAW     = 0x40000000;   // Do not redraw
const TMUInt32 WS_PRI_NOTABSTOP         = 0x80000000;   // can't tab in widget and child widget.
const TMUInt32 WS_PRI_BINDCBLIST        = 0x01000000;   // bind for combo::listbox
const TMUInt32 WS_PRI_BINDMODE          = 0x02000000;   // bind mode widget
const TMUInt32 WS_PRI_NONOTIFY          = 0x04000000;   // disable send WM_COMMAND
const TMUInt32 WS_PRI_ALIGNAUTOSIZE     = 0x08000000;   // use child align to auto size. edit,static,button use context, so haven't the flag.

// 1. create new object; now is new.
// 2. if (new) { set WS_PRI_LIFE_NCCREATE; Send(WM_NCCREATE); }
// 3. if (WS_PRI_LIFE_NCCREATE) { set WS_PRI_LIFE_NCCREATE; Send(WM_CREATE); }
// 4. if (cur < WS_PRI_LIFE_DESTROYING) { Set WS_PRI_LIFE_DESTROYING; Send(WM_DESTROY); Send(WM_NCDESTROY); Set WS_PRI_LIFE_DESTROYED; }
const TMUInt32 WS_PRI_LIFE_NEW          = 0x00000000;   // new object
const TMUInt32 WS_PRI_LIFE_NCCREATE     = 0x00100000;   // have been send WM_NCCREATE
const TMUInt32 WS_PRI_LIFE_CREATE       = 0x00200000;   // have been send WM_CREATE
const TMUInt32 WS_PRI_LIFE_DESTROYING   = 0x00300000;   // destroying
const TMUInt32 WS_PRI_LIFE_DESTROY_CLD  = 0x00400000;   // destroy child
const TMUInt32 WS_PRI_LIFE_DETACH       = 0x00500000;   // detach tree
const TMUInt32 WS_PRI_LIFE_NCDESTROY    = 0x00600000;   // send WM_NCDESTROY
const TMUInt32 WS_PRI_LIFE_DESTROYED    = 0x00700000;   // clean all, has been destroyed
const TMUInt32 WS_PRI_LIFE_MASK         = 0x00F00000;

const TMUInt32 WS_PRI_IS_WINDOW         = 0x00010000;   // it is root window
const TMUInt32 WS_PRI_IS_SCROLLBOX      = 0x00020000;   // the widget is scroll box
const TMUInt32 WS_PRI_IS_CURRENTTAB     = 0x00030000;   // current checked tab button
const TMUInt32 WS_PRI_IS_RADIO          = 0x00040000;   // is radio button
const TMUInt32 WS_PRI_TYPE_MASK         = 0x000f0000;

// button
const TMUInt32 BS_PRI_KEYDOWN           = 0x00000001;

// combo button
const TMUInt32 CB_PRI_UPDOWNMODE        = 0x00000002;            // the user last press up/down key

// tree;
const TMUInt32 WS_PRI_INVALIDATE        = 0x00000001;

// dummy, float
const TMUInt32 DM_PRI_USEWINDOWRECT     = 0x00000010;   // bind HWND to dummy window rect, else bind to padding rect
#define DM_PRI_MODE_CHILD               DUMMYINFO::MODE_CHILD
#define DM_PRI_MODE_POP                 DUMMYINFO::MODE_POP
#define DM_PRI_FLAG_MASK                DUMMYINFO::FLAG_MASK

// webview : dummy,
const TMUInt32 WV_PRI_ZOOMED            = 0x000000020; 
const TMUInt32 WV_PRI_LOADED            = 0x000000040; 

// menu bar
const TMUInt32 MB_PRI_INPOP             = 0x000000001;
const TMUInt32 MB_PRI_INALT             = 0x000000002;

// rich
const TMUInt32 RICH_PRI_MODIFY          = 0x000000010;   // is the control have been modified
const TMUInt32 RICH_PRI_ENSUREVISIBLE   = 0x000000020;   // is the control need ensure visible
const TMUInt32 RICH_PRI_AFTER_WRAP      = 0x000000040;   // the caret is displayed in wraped new line
const TMUInt32 RICH_PRI_UPDATE          = 0x000000080;   // need send EN_UPDATE message
const TMUInt32 RICH_PRI_TYPE_HTML       = 0x000000002;

// layer
const TMUInt32 LAYER_PRI_PAINTING       = 0x000000004;   // the widget is processing WM_PRINT

// mgr
const TMUInt32 MS_PRI_IS_DESTROYWIN     = 0x000000002;   // the widget is destroy
const TMUInt32 MS_PRI_HIDEFOCUS         = 0x000000010;   // hide focus
const TMUInt32 MS_PRI_NEEDERASEBKGND    = 0x000000020;   // need erase back ground
const TMUInt32 MS_PRI_NEEDRESETSIZEOBX  = 0x000000040;   // need add size box when paint


const TMLParam c_lpPrint = PRF_NONCLIENT | PRF_ERASEBKGND | PRF_CLIENT | PRF_CHECKVISIBLE | PRF_CHILDREN;


template <class T> class TM_NO_VTABLE CFrameSourceT : public IFrameSource
{
public:
    virtual HTUI FSGetWin() tm_override { return static_cast<T*>(this); }
    virtual TMUInt32 FSGetStyle() tm_override { return FSGetWin()->m_dwStyle; }
    virtual TMUInt32 FSGetExStyle() tm_override { return FSGetWin()->m_dwAdvStyle; }
    virtual TMUInt32 FSGetLayout() tm_override { return FSGetWin()->m_dwLayout; }
    virtual TMBool FSIsActive() tm_override { return FSGetWin()->IsActive(); }
    virtual void FSGetRect(TUIRect& rc) tm_override { rc = FSGetWin()->m_rcWnd; }
    virtual void FSGetHScrolBarRect(TUIRect& rc, int nBarHeight) tm_override;
    virtual void FSGetVScrolBarRect(TUIRect& rc, int nBarWidth) tm_override;
    virtual void FSGetHScrollInfo(TUISCROLLINFO& si) tm_override { FSGetWin()->GetHScrollInfo(si); }
    virtual void FSGetVScrollInfo(TUISCROLLINFO& si) tm_override { FSGetWin()->GetVScrollInfo(si); }
    virtual void FSSendMessage(TMUInt uMsg, TMWParam wp, TMLParam lp) tm_override { FSGetWin()->SendMessage(uMsg, wp, lp); }
    virtual void FSCalcShadow(CTuiRect& rc) tm_override;
};

////////////////////////////////////////////////////////////////////////////////
// CTreeBaseT
template <class T> struct CTreeBaseT
{
    CRefPtr<T> _parent;
    T* _child;
    T* _next;
    T* _prev;

    CTreeBaseT() : _parent(0), _child(0), _next(0), _prev(0) {}
};

////////////////////////////////////////////////////////////////////////////////
// Bind Window
union BINDOWNER
{
    struct TREEITEM*    m_pTVItem; // tree item.
    struct LBITEM*      m_pLBItem;

    BINDOWNER(TREEITEM* p = 0) : m_pTVItem(p) {}
    explicit BINDOWNER(LBITEM* p) : m_pLBItem(p) {}
};

struct TM_NO_VTABLE IBindOwner
{
public:
    virtual BINDOWNER GetOwner(HTUI hSelf) = 0;
    virtual TMBool GetRectForInvalidate(HTUI hSelf, CTuiRect& rect) = 0;
};


////////////////////////////////////////////////////////////////////////////////
// CTuiWgt
class TM_NO_VTABLE CTuiWgt
    : public CTuiObject
    , public CScrollImpl<CTuiWgt>
    , public CTreeNodeT<HTUI, CTreeBaseT<CTuiWgt> >
    , public CFrameSourceT<CTuiWgt>
//     , public IWalkerItem
{
public:
    enum { eObjType = e_otWgt };

    typedef CTreeItemT<HTUI>                        CItem;
    typedef CSubClassMgr<HTUI>                      CWgtProcMgr;

    class CWndRec
    {
        CTuiWgt* m_p;

    public:
        CWndRec (CTuiWgt* p) : m_p (p) {}
        CWndRec& operator ++ () { m_p = m_p->GetNext(); return *this; }
        operator bool () const { return m_p != 0; }
        bool operator ! () const { return m_p == 0; }
        CTuiWgt* operator * () { return m_p; }
    };

#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        return IC_Size(m_procMgrWgt.m_aPorc);
    }
#endif // TM_DBG

public:
    CWgtProcMgr         m_procMgrWgt;

    TMUInt32            m_dwPriStyle; // private style
    CTuiRect            m_rcWnd;
    CTuiRect            m_rcClient;
    BORDER              m_bdMargin;
    BORDER              m_bdPadding;
    int                 m_nMinWidth;
    int                 m_nMaxWidth;
    int                 m_nMinHeight;
    int                 m_nMaxHeight;

    TMUInt32            m_dwID;
    TMUInt32            m_dwStyle;
    TMUInt32            m_dwAdvStyle;
    TMUInt32            m_dwLayout;
    TMUInt32            m_dwRole;   // ROLE_SYSTEM_XXX
    TMUInt32            m_nIDHint;  // CID<EM_DIALOG>
    CString             m_strHint;
    CString             m_strAcc;
    GXFont              m_font;
    CFrmPtr             m_pFrame;
    CPicPtr             m_pBk;
    CPropText           m_gxText;
    CWidgetAniList      m_aniList;

    IBindOwner*         m_pBind; // tree item.

    HTUI                m_hRoutePrintClient; // current route print client proc. 0:not inited; -1:no route;

    TUI_IF_EDIT(TM::CFps m_fpsRend);
    TUI_IF_EDIT(TM::CFps m_fpsRendSelf);

    friend class  CWidgetHelper;
    friend struct CLayoutHelper;
    friend class  CTuiWnd;
    friend class  CWndRec;
    friend class  CView;
    friend struct CTreeItemT<HTUI>;
    friend struct CTreeDataSourceContainer;
    friend CEditStream;
    friend CScrollImpl<CTuiWgt>;
    
public:
    TM_BGN_MASK_SYMBOL  (CSymbol_WsStyle)
        TM_MASK_SYMBOL  (WS_CAPTION)
        TM_MASK_SYMBOL  (WS_BORDER)
        TM_MASK_SYMBOL  (WS_DLGFRAME)
        TM_MASK_SYMBOL  (WS_SYSMENU)
        TM_MASK_SYMBOL  (WS_SIZEBOX)
        TM_MASK_SYMBOL  (WS_DISABLED)
        TM_MASK_SYMBOL  (WS_TABSTOP)
        TM_MASK_SYMBOL  (WS_VISIBLE)
        TM_MASK_SYMBOL  (WS_GROUPTABSTOP)
    TM_END_MASK_SYMBOL  ()

    TM_BGN_MASK_SYMBOL  (CSymbol_Layout) //, LAYOUT_MASK)
        TM_MASK_SYMBOL  (LAYOUT_AUTOWIDTH)
        TM_MASK_SYMBOL  (LAYOUT_AUTOHSCROLL)
        TM_MASK_SYMBOL  (LAYOUT_HSCROLL)
        TM_MASK_SYMBOL  (LAYOUT_AUTOHEIGHT)
        TM_MASK_SYMBOL  (LAYOUT_AUTOVSCROLL)
        TM_MASK_SYMBOL  (LAYOUT_VSCROLL)
        TM_MASK_SYMBOL  (LAYOUT_FLOATSCROLL)
        TM_MASK_SYMBOL  (LAYOUT_HOTSCROLL)
        
        TM_MASK_BREAK   ()
        TM_MASK_SYMBOL2 (ANCHOR_TOP, ANCHOR_VMASK)
        TM_MASK_SYMBOL2 (ANCHOR_TCENTER, ANCHOR_VMASK)
        TM_MASK_SYMBOL2 (ANCHOR_VCENTER, ANCHOR_VMASK)
        TM_MASK_SYMBOL2 (ANCHOR_BCENTER, ANCHOR_VMASK)
        TM_MASK_SYMBOL2 (ANCHOR_BOTTOM, ANCHOR_VMASK)
        TM_MASK_SYMBOL2 (ANCHOR_VFRAME, ANCHOR_VMASK)
        TM_MASK_SYMBOL2 (ANCHOR_VNONE, ANCHOR_VMASK)
        TM_MASK_SYMBOL2 (ANCHOR_LEFT, ANCHOR_HMASK)
        TM_MASK_SYMBOL2 (ANCHOR_LCENTER, ANCHOR_HMASK)
        TM_MASK_SYMBOL2 (ANCHOR_CENTER, ANCHOR_HMASK)
        TM_MASK_SYMBOL2 (ANCHOR_RCENTER, ANCHOR_HMASK)
        TM_MASK_SYMBOL2 (ANCHOR_RIGHT, ANCHOR_HMASK)
        TM_MASK_SYMBOL2 (ANCHOR_FRAME, ANCHOR_HMASK)
        TM_MASK_SYMBOL2 (ANCHOR_NONE, ANCHOR_HMASK)
        TM_MASK_BREAK   ()
        TM_MASK_SYMBOL2 (ALIGN_LEFT, LAYOUT_MASK)
        TM_MASK_SYMBOL2 (ALIGN_RIGHT, LAYOUT_MASK)
        TM_MASK_SYMBOL2 (ALIGN_TOP, LAYOUT_MASK)
        TM_MASK_SYMBOL2 (ALIGN_BOTTOM, LAYOUT_MASK)
        TM_MASK_SYMBOL2 (ALIGN_CLIENT, LAYOUT_MASK)
        TM_MASK_SYMBOL2 (ALIGN_LEFT_RIGHT, LAYOUT_MASK)
        TM_MASK_SYMBOL2 (ALIGN_RIGHT_RIGHT, LAYOUT_MASK)
        TM_MASK_SYMBOL2 (ALIGN_TOP_BOTTOM, LAYOUT_MASK)
        TM_MASK_SYMBOL2 (ALIGN_BOTTOM_BOTTOM, LAYOUT_MASK)
        TM_MASK_SYMBOL2 (ALIGN_HEADER, LAYOUT_MASK)
        
        TM_MASK_BREAK   ()
        TM_MASK_SYMBOL2 (FLOW_TOP, LAYOUT_MASK)
        TM_MASK_SYMBOL2 (FLOW_MIDDLE, LAYOUT_MASK)
        TM_MASK_SYMBOL2 (FLOW_BOTTOM, LAYOUT_MASK)
        TM_MASK_SYMBOL2 (FLOW_BREAK, LAYOUT_MASK)
//         TM_MASK_BREAK   ()
//         TM_MASK_SYMBOL2 (TABLE_ROW, TABLE_MASK)
    TM_END_MASK_SYMBOL  ()

    TM_BGN_MASK_SYMBOL  (CSymbol_AdvStyle) //, WS_ADV_NORMASTYLEMASK)
        TM_MASK_SYMBOL  (WS_ADV_L2RHINT)
        TM_MASK_SYMBOL  (WS_ADV_CLIENTEDGE)
        TM_MASK_SYMBOL  (WS_ADV_PENETRATE)
        TM_MASK_SYMBOL  (WS_ADV_ATTACHMENT)
        TM_MASK_SYMBOL  (WS_ADV_ROUTER)
        TM_MASK_SYMBOL  (WS_ADV_TABGROUP)
        TM_MASK_SYMBOL  (WS_ADV_ARROWGROUP)
    TM_END_MASK_SYMBOL  ()

    TM_BGN_ENUM_SYMBOL  (CSymbol_Role)
//        TM_ENUM_SYMBOL  (ROLE_CUSTOM)
        TM_ENUM_SYMBOL  (ROLE_DEFAULT)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_TITLEBAR)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_MENUBAR)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_SCROLLBAR)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_GRIP)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_SOUND)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_CURSOR)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_CARET)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_ALERT)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_WINDOW)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_CLIENT)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_MENUPOPUP)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_MENUITEM)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_TOOLTIP)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_APPLICATION)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_DOCUMENT)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_PANE)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_CHART)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_DIALOG)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_BORDER)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_GROUPING)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_SEPARATOR)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_TOOLBAR)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_STATUSBAR)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_TABLE)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_COLUMNHEADER)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_ROWHEADER)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_COLUMN)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_ROW)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_CELL)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_LINK)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_HELPBALLOON)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_CHARACTER)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_LIST)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_LISTITEM)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_OUTLINE)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_OUTLINEITEM)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_PAGETAB)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_PROPERTYPAGE)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_INDICATOR)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_GRAPHIC)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_STATICTEXT)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_TEXT)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_PUSHBUTTON)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_CHECKBUTTON)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_RADIOBUTTON)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_COMBOBOX)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_DROPLIST)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_PROGRESSBAR)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_DIAL)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_HOTKEYFIELD)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_SLIDER)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_SPINBUTTON)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_DIAGRAM)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_ANIMATION)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_EQUATION)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_BUTTONDROPDOWN)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_BUTTONMENU)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_BUTTONDROPDOWNGRID)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_WHITESPACE)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_PAGETABLIST)
        TM_ENUM_SYMBOL  (ROLE_SYSTEM_CLOCK)
    TM_END_ENUM_SYMBOL  ()

    TM_DECLARE_FLAG     (_WS_VISIBLE,           m_dwStyle,    WS_VISIBLE);

    TM_DECLARE_FLAG     (_WS_PRI_BINDCBLIST,    m_dwPriStyle, WS_PRI_BINDCBLIST);
    TM_DECLARE_FLAG     (_WS_PRI_BINDMODE,      m_dwPriStyle, WS_PRI_BINDMODE);
    TM_DECLARE_FLAG     (_WS_PRI_NOTABSTOP,     m_dwPriStyle, WS_PRI_NOTABSTOP); // for WS_AUTOTABSTOP
    TM_DECLARE_FLAG     (_WS_PRI_FRAMECHANGED,  m_dwPriStyle, WS_PRI_FRAMECHANGED);
    TM_DECLARE_NOFLAG   (FlagEnableNotify,      m_dwPriStyle, WS_PRI_NONOTIFY);
    TM_DECLARE_NOFLAG   (FlagRedraw,            m_dwPriStyle, WS_PRI_DISABLEREDRAW);
    TM_DECLARE_FLAG     (_WS_PRI_DIRTY,         m_dwPriStyle, WS_PRI_DIRTY);
    TM_DECLARE_FLAG     (_WS_PRI_ALIGNAUTOSIZE, m_dwPriStyle, WS_PRI_ALIGNAUTOSIZE);

    TM_DECLARE_FLAG2    (_WS_PRI_IS_WINDOW,     m_dwPriStyle, WS_PRI_IS_WINDOW,     WS_PRI_TYPE_MASK);
    TM_DECLARE_FLAG2    (_WS_PRI_IS_SCROLLBOX,  m_dwPriStyle, WS_PRI_IS_SCROLLBOX,  WS_PRI_TYPE_MASK);
    TM_DECLARE_FLAG2    (_WS_PRI_IS_CURRENTTAB, m_dwPriStyle, WS_PRI_IS_CURRENTTAB, WS_PRI_TYPE_MASK);
    TM_DECLARE_FLAG2    (_WS_PRI_IS_RADIO,      m_dwPriStyle, WS_PRI_IS_RADIO,      WS_PRI_TYPE_MASK);
    TMBool IsRadio() const { return Is_WS_PRI_IS_RADIO() || Is_WS_PRI_IS_CURRENTTAB(); }

    TM_DECLARE_FLAG_GET (_WS_ADV_L2RHINT,      m_dwAdvStyle, WS_ADV_L2RHINT);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_ID     ("ID",          m_dwID)
        OBJ_PROP_MASK   ("Style",       m_dwStyle,      CSymbol_WsStyle)
        OBJ_PROP_MASK   ("Layout",      m_dwLayout,     CSymbol_Layout)
        OBJ_PROP_MASK   ("AdvStyle",    m_dwAdvStyle,   CSymbol_AdvStyle)
        OBJ_PROP_ENUM   ("role",        m_dwRole,       CSymbol_Role)
        OBJ_PROP_CLASS  ("Rect",        m_rcWnd)
        OBJ_PROP_INT    ("MinWidth",    m_nMinWidth)
        OBJ_PROP_INT    ("MaxWidth",    m_nMaxWidth)
        OBJ_PROP_INT    ("MinHeight",   m_nMinHeight)
        OBJ_PROP_INT    ("MaxHeight",   m_nMaxHeight)
        OBJ_PROP_CLASS  ("Text",        m_gxText)
        OBJ_PROP_DLG    ("HintDlg",     m_nIDHint)
        OBJ_PROP_TEXT   ("Hint",        m_strHint)
        OBJ_PROP_TEXT   ("AccText",     m_strAcc)
        OBJ_PROP_PIC    ("BkPic",       m_pBk)
        OBJ_PROP_CLASS  ("Font",        m_font)
        OBJ_PROP_FRAME  ("Frame",       m_pFrame)
        OBJ_PROP_CLASS  ("margin",      m_bdMargin)
        OBJ_PROP_CLASS  ("padding",     m_bdPadding)
        CHAIN_OBJ_PROP  (CTuiObject)
        OBJ_PROP_CHILDS ()
    END_OBJ_PROP_MAP    ()

protected:
    CTuiWgt();
    ~CTuiWgt();
    CTuiWgt* cthis () const { return const_cast<CTuiWgt*> (this); }

    void NodeDetach () { this->Detach (); }

    TMBool NodeInsertAfter (CTuiWgt* p);

    TMBool LoadPicAndInvalidate(CPicPtr& ptr, int nObjID, TMLParam lParam);
    bool _MakeWindowHInt(CString* str, const GX::CText& txt);
    bool _MakeWindowHInt(CString* str, const CString& strHint, const GX::CText& txt);
    bool _MakeWindowHInt(CString* str, IColumnProxy* proxy, TMLParam lPItem, TMLParam lPHead);

private:
    // wndproc, subclass, unsubclass
    static TMBool _Proc(HTUI p, HTUI h, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
    {
        return p->ProcessWindowMessage(h, uMsg, wParam, lParam, lResult, dwMsgMapID);
    }
    void Subclass_self() { Subclass(CWidgetMsgDelegate(this, _Proc)); }
    TMBool Unsubclass_self() { return GetProcs().Unsubclass(this, CWidgetMsgDelegate(this, _Proc)); }

public:
    void OSSetCursor(CCursorAT::cursor_id nID);
    
    TMBool StringMessageToString(TMLParam lParam, CString& str);
    void RenderListItem(TUIDRAWITEM* pDrawS, ITuiPic* pFore, int uIndent, CPropColor* clrTxt);
    void RenderFocusRect(IGXApp* gapp, const CTuiRect& rc);
    TMBool IsRenderFocusRect() { return (m_dwStyle & BS_DRAWFOCURECT) && IsFocus() && !IsHideFocus(); }

    void RenderSetTextColor(IGXApp* gapp, int nFrame, CPropColor** clrs, CAppColor::PFNGetColor pfnText, int frmPfn);
    void RenderSetTextColor(IGXApp* gapp, int nFrame, CPropColor** clrs, CAppColor::PFNGetColor pfnText) { RenderSetTextColor(gapp, nFrame, clrs, pfnText, nFrame); }
    void RenderSimpleBk(IGXApp* gapp, int nState, CAppColor::PFNGetColor pfn); // nState: GetButtonState()
    void RenderStaticBk(IGXApp* gapp, int nState, EButStates* states = 0);
    void DoSetWindowPos(CTuiWidgetPos& wp, FRAMEINFO& fi);

    void OnNcLButtonDown(TMLParam lParam);
    void OnNcLButtonUp(TMLParam lParam);
    void OnNcLButtonDblClk(TMLParam lParam);

    void CheckParentFrameChange(TMUInt uFlag);

public:
    // CTuiObject
    void FinalConstruct(CTuiObject* pParent);
    void FinalRelease();
    void OnLoadProp(IDomElement* pXml);
    TMResult Dispatch(TMUInt msg, TMWParam wp, TMLParam lp);
#ifdef TUI_EDIT_MODE
    virtual CString GetDebugInfo() tm_override;
    virtual TMBool EditorMatchID(TMUInt32 n) tm_override;
#endif

    // self
    virtual TMBool ProcessWindowMessage(CTuiWgt* pSender, TMUInt uMsg,
        TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0);
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) { RenderSimpleBk(gapp, 0, CAppColor::GetColorButtonBk); }
    virtual void DoPrint(IGXApp* gapp, TMLParam lParam);
    virtual void DoSetText(const CString& str, int bRich) { m_gxText.ReCreate(str, bRich == TUISETTEXT::eHTML); }
    virtual CFrame* GetFrame();
    virtual void UpdateOnArrowDirty() { ChangeFrame(true); }
    virtual bool GetWindowHint(ETTActive ett, CString* str); // str can be 0:
    virtual EButStates DoGetStates(HTUI hFocus, HTUI hOver, TMBool bMouseDown);
    virtual TMBool SetWindowPos(CTuiWgt* pInsertAfter, GXDips x, GXDips y, GXDips cx, GXDips cy, TMUInt nFlags);
    virtual TMBool IsHideFocus() const;
    virtual TMBool NativeInvalidateRect(SPY_INVALIDATE_PARAM(const TUIRect& rc, TM::HTUI hSender));
    virtual TMHWnd GetOverlayHWND(); // float will pop a overlay window.
    virtual void Reclaim(); // call when hide; dpi, language, device changed.
//     virtual void GetWalkerItemNext(CWalkerItem& itm);
//     virtual void GetWalkerItemPrev(CWalkerItem& itm);
    virtual void OnSetLife(TMUInt32 life) {}

public:
    TMResult SendBubbleMessage(TMUInt uMsg, TMWParam wParam, TMLParam lParam);
    
    TMResult OnInvalidate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        bHandled = false;
        Invalidate();
        return 0;
    }

    TMResult OnDirty(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        bHandled = false;
        SetDirtyAndInvalidate();
        return 0;
    }

    void GetMouseTarget(HTUI& p, HTUI& pHint, const CTuiPoint& pt, TMUInt uFlags);
    TMBool GetAndHitMouseTarget(TUIHITINFO& ht, TMUInt uMsg, TMUInt uFlagsExt);

    TMBool PreCalcWindowPos(CTuiWidgetPos& wp, const FRAMEINFO& fi, const CTuiRect& rcMinMax);

public:
    CWndRec GetChilds () { return GetChild(); }

    bool DestroyWindow();
    
    void InitConstruct(TUIFACTORY* pFactory, void* param);
    void UnInitOnManagerFail();

    TMBool HaveParent() const { return _parent != 0; }

    void SetLife(TMUInt32 life) { DbgAssert((m_dwPriStyle & life) != life); m_dwPriStyle &= ~WS_PRI_LIFE_MASK; m_dwPriStyle |= life; OnSetLife(life); }
    TMUInt32 GetLife() const { return m_dwPriStyle & WS_PRI_LIFE_MASK; }
    TMBool IsLifeNew() const { return GetLife() == WS_PRI_LIFE_NEW; }
    TMBool IsLifeDestroyed() const { return GetLife() == WS_PRI_LIFE_DESTROYED; }
    TMBool IsLifeInvalid() const { return GetLife() >= WS_PRI_LIFE_DESTROYING; }
    TMBool IsBindWindow() const { return m_dwPriStyle & WS_PRI_BINDMODE; }

    EBindState IsBindOrSub(HTUI hParent); // return is current widget is bind mode, or child of bind widget, stop on hParent.
    TMBool IsBindSelected();
    void ResetBindItemPosition(CTuiRect rcWnd, TMUInt32 uRedrawFlag, TMBool bForce = false);
    void Bind_Set(HTUI hParent, IBindOwner* pOwner, TMBool bSelect = false);    // set bind mode
    void Bind_Reset(HTUI hParent);                                              // set normal mode
    void Bind_Select() { Bind_Set(_parent, m_pBind, true); }                    // add bind widget to global tab list
    void Bind_Unselect() { Bind_Set(_parent, m_pBind, false); }                 // remove bind widget from global tab list

    // Attributes    
    TMBool IsClipChild() const { return ((m_dwLayout & (LAYOUT_HSCROLL | LAYOUT_VSCROLL)) || (m_dwAdvStyle & WS_ADV_CLIENTEDGE)); }

    TMBool IsTabGroup() const { return m_dwAdvStyle & WS_ADV_TABGROUP; }
    TMBool IsArrowGroup() const { return m_dwAdvStyle & WS_ADV_ARROWGROUP; }
    TMBool IsTabStop() const { return !Is_WS_PRI_IS_WINDOW() && ((m_dwStyle & WS_TABSTOP) == WS_TABSTOP); }
    TMBool IsGroup() const { return m_dwStyle & WS_GROUP; }
    TMBool SetGroupDefaultTabStop();

    TMBool IsWindowVisible() const { return Edit_IsShowHide(Edit_IsObjectInDocMode(this)) ? true : (WS_VISIBLE & m_dwStyle); }
    TMBool IsWindowEnabled() const { return ! (WS_DISABLED & m_dwStyle); }
    TMBool CanRestore () { return (m_dwStyle & WS_MAXIMIZE); }

    TMBool IsAnchorFrame() const { return (m_dwLayout & (LAYOUT_MODE_MASK | LAYOUT_HMASK)) == LAYOUT_FRAME; }
    TMBool IsAnchorVFrame() const { return (m_dwLayout & (LAYOUT_MODE_MASK | LAYOUT_VMASK)) == LAYOUT_VFRAME; }
    TMBool IsAnchorVNone() const { return (m_dwLayout & (LAYOUT_MODE_MASK | LAYOUT_VMASK)) == LAYOUT_VNONE; }
    TMBool IsAlignH() const { return LAYOUT_IS_ALING_H(m_dwLayout); }
    TMBool IsAlignV() const { return LAYOUT_IS_ALING_V(m_dwLayout); }
    TMBool IsAlignClient() const { return (m_dwLayout & ALIGN_MASK) == ALIGN_CLIENT; }

    TMBool IsAutoWidth() const;
    TMBool IsAutoHeight() const;
    TMBool IsAutoHScroll() const { return m_dwLayout & LAYOUT_AUTOHSCROLL; }
    TMBool IsAutoVScroll() const { return m_dwLayout & LAYOUT_AUTOVSCROLL; }
    void SetAutoHScroll() { m_dwLayout |= LAYOUT_AUTOHSCROLL; }
    void SetAutoVScroll() { m_dwLayout |= LAYOUT_AUTOVSCROLL; }
    TMBool HasHScroll() const { return m_dwLayout & LAYOUT_HSCROLL; }
    TMBool HasVScroll() const { return m_dwLayout & LAYOUT_VSCROLL; }
    void SetHScroll() { m_dwLayout |= LAYOUT_HSCROLL; }
    void RemoveHScroll() { m_dwLayout &= ~LAYOUT_HSCROLL; }
    void SetVScroll() { m_dwLayout |= LAYOUT_VSCROLL; }
    void RemoveVScroll() { m_dwLayout &= ~LAYOUT_VSCROLL; }
    TM_DECLARE_FLAG_GET(LayoutFloatScroll, m_dwLayout, LAYOUT_FLOATSCROLL);
    TM_DECLARE_FLAG2_GET(LayoutHotScroll, m_dwLayout, LAYOUT_HOTSCROLL, LAYOUT_HOTSCROLL);

    TMBool IsVert() const { return ((m_dwLayout & LAYOUT_MODE_MASK) != LAYOUT_FLOW) && !IsAlignH(); }

    TMUInt32 GetStyle() const { return m_dwStyle; }

    TMUInt32 GetExStyle() const { return m_dwAdvStyle; }

    TMUInt32 GetPriStyle() const { return m_dwPriStyle;}

//     TMULong GetState() const;

    ITuiPic* GetBkPic() { return m_pBk; }

    TMUInt32 GetIDHint() const { return m_nIDHint; }
//     static bool Text2Hint(CString* str, CView* view, const GX::CText& txt);
    void SetWindowHint (TMCStr p) { m_strHint = p; }

//     void SetWindowBk (ITuiPic* pPic);

//     ITuiPic* GetWindowBk () { return m_pBk; }

//    TMUInt32 SetWindowLong (int nIndex, TMUInt32 dwNewLong);

    TMUInt32 SetStyle (TMUInt32 dwStyle); // { return SetWindowLong (GWL_STYLE, dwStyle); }

    TMUInt32 SetExStyle (TMUInt32 dwStyle); // { return SetWindowLong (GWL_EXSTYLE, dwStyle); }

//     TMLLong SetWindowID (TMLLong dwID) //  { return SetWindowLong (GWL_ID, dwID); }
//     {
//         TMLLong x = m_dwID;
//         m_dwID = dwID;
//         return x;
//     }

    void CalcProjRect(CTuiRect& rc, TMUInt fmtTxt, int nExclude = 0)
    {
        DbgAssert(Is_WS_PRI_DIRTY());

        GetPaddingRect(&rc);
        GXDips w = rc.Width();
        if (IsAutoWidth())
            w = (GXDips)SHRT_MAX;
        if (m_nMaxWidth >= 0)
        {
            GXDips ww = m_nMaxWidth - GetViewLeft() - GetViewRight() - nExclude;
            if (ww > 0 && w > ww)
                w = ww;
        }
        rc.SetWidth(w);
        rc.SetBottom((GXDips)SHRT_MAX);
    }

    EButState GetButtonState() const;
    EButStates GetStates();

    // Subclass
    CWgtProcMgr& GetProcs()
    {
        return m_procMgrWgt;
    }

    void Subclass(const CWidgetMsgDelegate& pCallback)
    {
        DbgAssert(!IsLifeInvalid());
        m_procMgrWgt.Subclass(this, pCallback, this);
    }

    // Message Functions
    enum ESendMsg { eSendMsgProc = 0, eSendMsgPre = 1, eSendMsgRoute = 2, eSendMsgAll = 3 };
    TMResult SendMessage(TMUInt uMsg, TMWParam wParam = 0, TMLParam lParam = 0, ESendMsg eFlag = eSendMsgAll);
    TMResult SendPrintClientMessage(TMLParam lParam);

    TMBool PostCallback(PFNPostCallback wParam, TMLParam lParam);
    TMBool PostMessage(TMUInt uMsg, TMWParam wParam = 0, TMLParam lParam = 0);

//     TMResult SendNotifyMessage(TMUInt uMsg, TMWParam wParam = 0, TMLParam lParam = 0);

    // send command message
    TMResult Notify(int id, TMUInt16 uCode, TMBool bPost, HTUI hSender);
    TMResult Notify(int id, TMUInt16 uCode, TMBool bPost) { return Notify(id, uCode, bPost, this); }
    TMResult Notify(TMUInt16 uCode, TMBool bPost = false) { return Notify(m_dwID, uCode, bPost); }
//     TMBool EnableNotify(TMBool bEnable);
    TMResult NotifyEx(int id, NMBASE* nm) { return SendMessage(CM_PRENOTIFY, id, (TMLParam)nm); }
    TMResult NotifyEx(int id, TMUInt code, TMLParam lItm, TMLParam l1, TMLParam l2) { NMBASE cb = { this, m_dwObjID, m_dwID, code, lItm, l1, l2 }; return NotifyEx(id, &cb); }
    TMResult NotifyEx(TMUInt code, TMLParam lItm, TMLParam l1, TMLParam l2) { return NotifyEx(m_dwID, code, lItm, l1, l2); }

    TMBool SendBroadcastMessage(TMUInt uMsg, TMWParam wParam = 0, TMLParam lParam = 0, TMBool bSendSelf = true, size_t nLevel = -1);

    // Window Text Functions
//     TMBool SetWindowText(TMCStr lpszString)
//     {
//         return SendMessage (WM_SETTEXT, 0, (TMLParam)lpszString);
//     }
// 
//     int GetWindowText(TMStrW lpszStringBuf, int nMaxCount) const
//     {
//         return cthis()->SendMessage (WM_GETTEXT, nMaxCount, (TMLParam)lpszStringBuf);
//     }
// 
//     int GetWindowTextLength() const
//     {
//         return cthis()->SendMessage (WM_GETTEXTLENGTH, 0, 0);
//     }

    CGXStrPtr GetWindowText() const
    {
        return m_gxText.GetString();
    }

    // Font Functions
    void GetRealFont(GXFont& ft);
//     void SetFont(const GXFont* font, TMBool bRedraw = TMTrue);
// 
// 
//     void GetFont(GXFont* font) const
//     {
//         cthis()->SendMessage(CM_GETFONT, (TMWParam)font, 0);
//     }

    // Menu Functions (non-child windows only)
//     TMHMenu GetMenu () const
//     {
//         return ::GetMenu (GetHWND());
//     }

    // Window Size and Position Functions
    TMBool MoveWindow(GXDips x, GXDips y, GXDips nWidth, GXDips nHeight, TMBool bRepaint = TMTrue)
    {
        const TMUInt FLAGS_REDRAW  = (SWP_NOZORDER | SWP_NOACTIVATE);
        const TMUInt FLAGS_NOREDRAW = (FLAGS_REDRAW | SWP_NOREDRAW);

        return SetWindowPos(NULL, x, y, nWidth, nHeight,
            (bRepaint ? FLAGS_REDRAW : FLAGS_NOREDRAW));
    }

    TMBool MoveWindow(const TUIRect* lpRect, TMBool bRepaint = TMTrue)
    {
        return this->MoveWindow(RectLeft(*lpRect), RectTop(*lpRect),
            RectWidth(*lpRect), RectHeight(*lpRect), bRepaint);
    }

    TMBool SetWindowPos(CTuiWgt* pInsertAfter, const TUIRect* lpRect, TMUInt nFlags)
    {
        return this->SetWindowPos(pInsertAfter,
            RectLeft(*lpRect), RectTop(*lpRect),
            RectWidth(*lpRect), RectHeight(*lpRect), nFlags);
    }

//     void PaddingRectToClientRect(CTuiRect& rc);
//     void ClientRectToWindowRect(CTuiRect& rc);
    void WindowRectToMarginRect(CTuiRect& rc);

    void ClientRectToPaddingRect(CTuiRect& rc);
//     void WindowRectToClientRect(CTuiRect& rc);
    void MarginRectToWindowRect(CTuiRect& rc);

    TMBool SortChild(PFNWWCOMPARE pfn, TMLParam lParam, int nLevel, TMBool bReversed);

    TMBool ChangeFrame(TMBool bUpdate);

    // post dirty: can work if hide, out of parent.
    // invalidate: only word on can paint.
    void PostDirty();

    TMBool SetDirtyAndInvalidate();

    TMBool GetWindowRect(TUIRect* lpRect) const
    {
        *lpRect = m_rcWnd;
        return true;
    }

    const CTuiRect& GetWindowRect() const { return m_rcWnd; }

    TMBool GetClientRect(TUIRect* lpRect) const;

    const CTuiRect& GetClientRect() const { return m_rcClient; }

    void GetPaddingRect(TUIRect* rc) const
    {
        CalcPaddingRect(m_rcClient, rc);
    }

    void GetFrameAndPaddingBorder(BORDER& border)
    {
        border.left = m_bdPadding.left + m_rcClient.Left() - m_rcWnd.Left();
        border.top = m_bdPadding.top + m_rcClient.Top() - m_rcWnd.Top();
        border.right = m_bdPadding.right + m_rcWnd.Right() - m_rcClient.Right();
        border.bottom = m_bdPadding.bottom + m_rcWnd.Bottom() - m_rcClient.Bottom();
    }

    void CalcPaddingRect (const CTuiRect& rcS, TUIRect* rc) const
    {
        RectSetLeft(*rc, rcS.Left() + m_bdPadding.left);
        RectSetTop(*rc, rcS.Top() + m_bdPadding.top);
        RectSetRight(*rc, rcS.Right() - m_bdPadding.right);
        RectSetBottom(*rc, rcS.Bottom() - m_bdPadding.bottom);
    }

    GXDips GetPaddingWidth() const { return m_rcClient.Width() - m_bdPadding.left - m_bdPadding.right; }
    GXDips GetPaddingHeight() const { return m_rcClient.Height() - m_bdPadding.top - m_bdPadding.bottom; }

    const BORDER& GetPadding() const { return m_bdPadding; }
    const BORDER& GetMargin() const { return m_bdMargin; }

    GXDips GetViewLeft() const { return m_rcClient.Left() - m_rcWnd.Left() + m_bdPadding.left; }
    GXDips GetViewRight() const { return m_rcWnd.Right() - m_rcClient.Right() + m_bdPadding.right; }
    GXDips GetViewTop() const { return m_rcClient.Top() - m_rcWnd.Top() + m_bdPadding.top; }
    GXDips GetViewBottom() const { return m_rcWnd.Bottom() - m_rcClient.Bottom() + m_bdPadding.bottom; }

    GXDips GetLeft () const { return m_rcWnd.Left(); }
    GXDips GetRight () const { return m_rcWnd.Right(); }
    GXDips GetTop () const { return m_rcWnd.Top(); }
    GXDips GetBottom () const { return m_rcWnd.Bottom(); }
    GXDips GetWidth () const { return m_rcWnd.Width(); }
    GXDips GetHeight () const { return m_rcWnd.Height(); }
    GXDips GetMarginWidth() const { return m_rcWnd.Width() + m_bdMargin.left + m_bdMargin.right; }
    GXDips GetMarginHeight() const { return m_rcWnd.Height() + m_bdMargin.top + m_bdMargin.bottom; }

    const CTuiRect& GetBoundRect () const { return m_rcWnd; }

    void ShrinkMargin(TUIRect& rc) { (static_cast<CTuiRect&>(rc)).InflateRect(-m_bdMargin.left, -m_bdMargin.top, -m_bdMargin.right, -m_bdMargin.bottom); }

//     void GetAnchorMinSize(CTuiSize& sz);

//     void ClientToScreen(CPointI* lpPoint, int nPt = 1) const;
//     void ScreenToClient(CPointI* lpPoint, int nPt = 1) const;

    void ClientToScreen(const TUIPoint* lpClient, int nPt, TMPointI* lpScreen) const;
    void ScreenToClient(const TMPointI* lpScreen, int nPt, TUIPoint* lpClient) const;

    void ClientToScreen(const TUIRect& rc, TMRect& out) const 
    {
        ClientToScreen((CTuiPoint*)(&rc), 2, (CPointI*)&out);
//         if (GetModule()->GetLocal().IsRTL())
//             tm_swap(out.left, out.right);
    }
    
    void ScreenToClient(const TMRect& rc, TUIRect& out) const
    {
        ScreenToClient((CPointI*)(&rc), 2, (CTuiPoint*)&out);
//         if (GetModule()->GetLocal().IsRTL())
//             tm_swap(out.left, out.right);
    }

    // convert window position to padding-scroll position
//     GXDips MapScrollLeft(GXDips x)
//     {
//         return x - m_rcClient.Left() - m_bdPadding.left + GetScrollLeft();
//     }
// 
//     GXDips MapScrollTop(GXDips y)
//     {
//         return y - m_rcClient.Top() - m_bdPadding.top + GetScrollTop();
//     }

    TMBool ScrollToVisible(const CTuiRect& rc, TMBool bLeftRight, TMBool bTopBottom);


//     CAffine& GetMatrix() { return m_matrix; }

    // Coordinate Mapping Functions

    // Update and Painting Functions
//     TMHDC GetDC(TMBool bClip);
// 
//     int ReleaseDC(TMHDC hDC);

//     void InitDC (TMHDC hDC, TMBool bClip);
// 
//     void Print(TMHDC hDC, TMUInt32 dwFlags) const
//     {
//         cthis()->SendMessage(WM_PRINT, (TMWParam)hDC, dwFlags);
//     }
// 
//     void PrintClient(TMHDC hDC, TMUInt32 dwFlags) const
//     {
//         cthis()->SendMessage(WM_PRINTCLIENT, (TMWParam)hDC, dwFlags);
//     }

    TMBool UpdateWindow();

//     void SetRedraw(TMBool bRedraw = TMTrue)
//     {
//         SendMessage(WM_SETREDRAW, (TMWParam)bRedraw, 0);
//     }

//     TMBool GetWindowClipBox(TUIRect& rc);

    TMBool Invalidate()
    {
        return this->InvalidateRect(m_rcWnd);
    }

    TMBool InvalidateRect(const TUIRect& rc)
    {
        return this->NativeInvalidateRect(SPY_INVALIDATE_PARAM(rc, this));
    }

//     TMBool ShowWindow(int nCmdShow)
//     {
//         const TMUInt us = SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW;
//         const TMUInt uh = SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_HIDEWINDOW;
//         return this->SetWindowPos (0, 0, 0, 0, 0, nCmdShow ? us : uh);
//     }
// 
//     TMBool LockWindowUpdate(TMBool bLock = TMTrue)
//     {
//         return SendMessage(WM_SETREDRAW, (TMWParam)!bLock, 0);
//     }
// 
    TMBool RedrawWindow ()
    {
        Invalidate ();
        return UpdateWindow ();
    }

    // Timer Functions

    // Window State Functions
    TMBool CanInput () const;

    TMBool IsAllVisible(TMBool bCheckRealVisible = false, TMBool bCheckRealMinimize = false);

    TMBool IsAllEnable(TMBool bCheckReal = false) const;

    TMBool IsWindow()
    {
        return true; //m_dwPriStyle & WS_PRI_ISWINDOW;
    }

    TMBool EnableWindow(TMBool bEnable = TMTrue);

    TMBool SetCapture ();

//     void ReleaseCapture();
    TMBool IsCapture () const;

    TMBool SetFocus(EFocusReason reason, TMBool bActive = true);
    TMBool IsFocus() const;

    void SetNextFocus(TMBool bReverse, EFocusReason reason, TMBool bActive = true);

    TMBool IsActive() const;

    // Dialog-Box Item Functions

    // Scrolling Functions
//     TMBool GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo) const;
//     int SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, TMBool bRedraw = TMTrue);
//     int GetScrollPos(int nBar) const;
//     TMBool GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos) const;
//     TMBool ScrollWindow(int xAmount, int yAmount, const TUIRect* lpRect = NULL, const TUIRect* lpClipRect = NULL);
//     int ScrollWindowEx(int dx, int dy, const TUIRect* lpRectScroll, const TUIRect* lpRectClip, HRGN hRgnUpdate, TUIRect* lpRectUpdate, TMUInt uFlags);
//     int ScrollWindowEx(int dx, int dy, TMUInt uFlags, const TUIRect* lpRectScroll = NULL, const TUIRect* lpRectClip = NULL, HRGN hRgnUpdate = NULL, TUIRect* lpRectUpdate = NULL);
//     int SetScrollPos(int nBar, int nPos, TMBool bRedraw = TMTrue);
//     TMBool SetScrollRange(int nBar, int nMinPos, int nMaxPos, TMBool bRedraw = TMTrue);
//     TMBool ShowScrollBar(TMUInt nBar, TMBool bShow = TMTrue);
//     TMBool EnableScrollBar(TMUInt uSBFlags, TMUInt uArrowFlags = ESB_ENABLE_BOTH) { return 0; }

    // Window Access Functions
//     CTuiWgt* ChildWindowFromPoint (CTuiPoint point, TMUInt32 dwStyle = WS_VISIBLE, TMUInt32 dwStyleMask = WS_VISIBLE | WS_DISABLED, TMUInt32 dwAdvStyle = 0, TMUInt32 dwAdvMask = WS_ADV_PENETRATE) const;
//     CTuiWgt* ChildWindowFromPoint (CTuiPoint point, TMUInt uFlags, HTUI& hHint) const;

//     CTuiWgt* GetTopWindow();

    CTuiWgt* GetWindow(TMUInt nCmd);

    TMBool IsChild(const CTuiWgt* pChild) const;

    TMHWnd GetHWND () const;

    CTuiWnd* GetOwner() const;

    CTuiWgt* GetParent() const { return _parent; } // CTuiWgt* p = _parent; return /*const_cast<HTUI>*/(p); }

    CTuiWgt* SetParent(CTuiWgt* pParent);

    // Window Tree Access
    int GetDlgCtrlID() const
    {
        return m_dwID;
    }

    int SetDlgCtrlID(int nID)
    {
        int nOldID = m_dwID;
        m_dwID = nID;
        return nOldID;
    }

    CTuiWgt* GetDlgItem(int nID, HTUI hSkip = 0) const;
    HTUI SearchNearestItemInDlg(int nID);

    // IME
    // Alert Functions
    // Clipboard Functions
    // Caret Functions

//     inline void SetExtDraw() { m_dwPriStyle |= WS_PRI_EXTDRAW; }
//     inline TMBool IsExtDraw() { return m_dwPriStyle & WS_PRI_EXTDRAW; }
//     void ExtRenderPic(ITuiPic* pic, int nState, TMHDC hDC, const TUIRect& rc, void* key);

    // Drag-Drop Functions
    // Icon Functions
    // Help Functions
    // Hot Key Functions

//     TMBool StartDrag ();
//     void EndDrag ();

    TMBool EnumAllChildWindows(PFNWWENUMPROC lpEnumFunc, TMLParam lParam, TMBool bEnumSelf, size_t nLevel);
    void EnumResetRoutePrintClient();

    void AdjustClientRect(TUIRect& rc, TMUInt32 dwLayout);
    void AdjustClientRect(TUIRect& rc) { AdjustClientRect(rc, m_dwLayout); }

    HTUI CreateByObjectID(CModule* pModule, TMUInt uObjID, TUIFACTORY* pFactory, void* param = 0);

    TMBool SetNcCursor(EHitID hid);


//     void SetSysButtonDisabled(TMUInt uID, TMBool isDisabled);

}; // class CTuiWgt


// CTuiWidgetPosT
template <class T, class _Rc>
struct CTuiWidgetPosT : public T
{
    typedef typename _Rc::value_type value_type;

    TM_DECLARE_FLAG(ChangeFrame, this->flags, SWP_FRAMECHANGED);
    TM_DECLARE_FLAG(Show, this->flags, SWP_SHOWWINDOW);
    TM_DECLARE_FLAG(Hide, this->flags, SWP_HIDEWINDOW);
    TM_DECLARE_NOFLAG(Size, this->flags, SWP_NOSIZE);
    TM_DECLARE_NOFLAG(Move, this->flags, SWP_NOMOVE);
    TM_DECLARE_NOFLAG(ZOrder, this->flags, SWP_NOZORDER);
    TM_DECLARE_NOFLAG(Redraw, this->flags, SWP_NOREDRAW);
    TM_DECLARE_NOFLAG(LayOut, this->flags, SWP_NOLAYOUT);
    TM_DECLARE_NOFLAG(LayoutSibling, this->flags, SWP_NOLAYOUTSIBLING);

    static bool IsMoveOrSize(TMLParam lp)
    {
        T* p = (T*)lp;
        return ((SWP_NOSIZE | SWP_NOMOVE)) != (p->flags & (SWP_NOSIZE | SWP_NOMOVE));
    }

    TMBool IsBindOnSize() const
    {
        return IsSize() && ((this->flags & (SWP_NOREDRAW | SWP_NOLAYOUTSIBLING)) != (SWP_NOREDRAW | SWP_NOLAYOUTSIBLING));
    }

    void ReplaceFromRect(const _Rc& rcw)
    {
        if (this->flags & SWP_NOMOVE)
            this->x = rcw.Left(), this->y = rcw.Top();
        if (this->flags & SWP_NOSIZE)
            this->cx = rcw.Width(), this->cy = rcw.Height();
    }

    void ReplaceFromWP(const T& rhs)
    {
        if (this->flags & SWP_NOMOVE)
            this->x = rhs.x, this->y = rhs.y;
        if (this->flags & SWP_NOSIZE)
            this->cx = rhs.cx, this->cy = rhs.cy;
        if (!(this->flags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW)))
            this->flags |= (rhs.flags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW));
        if (!(rhs.flags & SWP_NOZORDER))
        {
            this->flags &= ~SWP_NOZORDER;
            this->hwndInsertAfter = rhs.hwndInsertAfter;
        }
        if (!(rhs.flags & SWP_NOOWNERZORDER))
            this->flags &= ~SWP_NOOWNERZORDER;
        if (!(rhs.flags & SWP_NOACTIVATE))
            this->flags &= ~SWP_NOACTIVATE;
        if (!(rhs.flags & SWP_NOCOPYBITS))
            this->flags &= ~SWP_NOCOPYBITS;
        if (!(rhs.flags & SWP_NOREDRAW))
            this->flags &= ~SWP_NOREDRAW;
    }

    void ResetFlag(const _Rc& rcWin)
    {
        if (CNumT<value_type>::Equal(this->x, rcWin.Left()) && CNumT<value_type>::Equal(this->y, rcWin.Top()))
            this->flags |= SWP_NOMOVE;
        else
            this->flags &= ~SWP_NOMOVE;
        if (CNumT<value_type>::Equal(this->cx, rcWin.Width()) && CNumT<value_type>::Equal(this->cy, rcWin.Height()))
            this->flags |= SWP_NOSIZE;
        else
            this->flags &= ~SWP_NOSIZE;
    }

    void ResetVisible(TMUInt32 dwStyle)
    {
        if (IsHide() && !(dwStyle & WS_VISIBLE))
            ResetHide();
        
        if (IsShow() && (dwStyle & WS_VISIBLE))
            ResetShow();
    }
    
    void MergeBound(const _Rc& rcScreen)
    {
        tm_setmax(this->x, rcScreen.Left());
        tm_setmax(this->y, rcScreen.Top());
        tm_setmin(this->cx, rcScreen.Right() - this->x);
        tm_setmin(this->cy, rcScreen.Bottom() - this->y);
    }

    void ResetBound(const _Rc& rcScreen)
    {
        this->x = rcScreen.Left();
        this->y = rcScreen.Top();
        this->cx = rcScreen.Width();
        this->cy = rcScreen.Height();
    }

    void DeltaSize(TMULong dwLayOut, value_type dx, value_type dy)
    {
        switch (dwLayOut & LAYOUT_MODE_MASK)
        {
        case LAYOUT_ANCHOR:
            switch (dwLayOut & LAYOUT_HMASK)
            {
            case LAYOUT_RCENTER:
            case LAYOUT_RIGHT:
                this->x -= dx; this->cx += dx;  // rc.MoveLeftTo(rc.Left() - dx); break;
                break;
                
            case LAYOUT_CENTER:
            case LAYOUT_FRAME:
                this->x = (this->x * (value_type)2 - dx) / (value_type)2;
                this->cx += dx;
//                rc.SetLeftRight(rc.Left() - n, rc.Right() + dx - n);
                break;
                
            default:
                this->cx += dx; //rc.OffsetRight(dx);
                break;
            }
            
            switch (dwLayOut & LAYOUT_VMASK)
            {
            case LAYOUT_BCENTER:
            case LAYOUT_BOTTOM:
                this->y -= dy; this->cy += dy; //rc.MoveTopTo(rc.Top() - dy);
                break;
                
            case LAYOUT_VCENTER:
            case LAYOUT_VFRAME:
                this->y = (this->y * (value_type)2 - dy) / (value_type)2;
                this->cy += dy;
//                 n = dy / (GXDips)2;
//                 rc.SetTopBottom(rc.Top() - n, rc.Bottom() + dy - n);
                break;
                
            default:
                this->cy += dy; //rc.OffsetBottom(dy);
                break;
            }
            break;
            
        case LAYOUT_ALIGN:
//         case LAYOUT_TABLE:
            switch (dwLayOut & ALIGN_TYPE_MASK)
            {
            case ALIGN_LEFT_RIGHT & ALIGN_TYPE_MASK:
            case ALIGN_LEFT & ALIGN_TYPE_MASK: this->cx += dx; this->cy += dy; break;// rc.OffsetRight(dx); break;
            case ALIGN_RIGHT_RIGHT:
            case ALIGN_RIGHT & ALIGN_TYPE_MASK: this->x -= dx; this->cx += dx; this->cy += dy; break; // rc.OffsetLeft(-dx); break;
            case ALIGN_TOP_BOTTOM:
            case ALIGN_TOP & ALIGN_TYPE_MASK: this->cy += dy; this->cx += dx; break; // rc.OffsetBottom(dy); break;
            case ALIGN_BOTTOM_BOTTOM:
            case ALIGN_BOTTOM & ALIGN_TYPE_MASK: this->y -= dy; this->cy += dy; this->cx += dx; break; // rc.OffsetTop(-dy); break;
            default: return;
            }
            break;
            
        case LAYOUT_FLOW:
            this->cx += dx; // rc.OffsetRight(dx);
            this->cy += dy; // rc.OffsetBottom(dy);
            break;
            
        default:
            DbgAssert(0);
            //         rc.OffsetRight(dx);
            //         rc.OffsetBottom(dy);
            break;
        }
    }

    void ResetFromRect(const _Rc& rcw)
    {
        this->x = rcw.Left(), this->y = rcw.Top();
        this->cx = rcw.Width(), this->cy = rcw.Height();
    }
};

// CWidgetCallT
template <class T, class P> struct CWidgetCallT
{
private:
    static void _done(void* lp)
    {
        P h = (P)lp;
        T::Done(h);
        h->Release();
    }
    
public:
    static TMBool Call(P lp)
    {
        lp->AddRef();
        TMBool b = CallCreate(eCallTypeAsync, _done, lp);
        if (!b)
            lp->Release();
        return b;
    }
};


// CForePicWin
template <class TBase>
struct TM_NO_VTABLE CForePicWin : public TBase
{
    CPicPtr      m_pFore;

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_PIC    ("ForePic",     m_pFore)
        CHAIN_OBJ_PROP  (TBase)
    END_OBJ_PROP_MAP    ()

    // CTuiWgt
    TMBool ProcessWindowMessage(TM::HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0)
    {
        if (uMsg == CBM_SETFOREPIC)
        {
            lResult = this->LoadPicAndInvalidate(m_pFore, wParam, lParam);
            return true;
        }
        else if (uMsg == CBM_GETFOREPIC)
        {
            ITuiPic* p = m_pFore;
            lResult = (TMResult)p;
            return true;
        }
        else if (uMsg == CM_ZOOMED)
            m_pFore.ChangeDPI(wParam);
        else if (uMsg == WM_DESTROY)
            m_pFore.Release();

        return TBase::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
    }

    // CTuiObject
#ifdef TUI_EDIT_MODE
    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_pFore.GetObjectID())
            return true;
        return TBase::EditorMatchID(n);
    }
#endif // TUI_EDIT_MODE
};


/////////////////////////////////////////////////////////////////////////////
// CWinState
struct CWinState
{
    HTUI        m_hWidget;
    int         m_nFrame;

    CWinState(HTUI h)
        : m_hWidget(h)
        , m_nFrame(h->GetButtonState())
    {
    }

    CWinState(HTUI h, int nCheck)
        : m_hWidget(h)
        , m_nFrame(h->GetButtonState() * nCheck)
    {
    }

    void Render(IGXApp* gapp, ITuiPic* p, const CTuiRect& rc)
    {
        if (p)
            p->Render(m_nFrame, 0, rc, 0, gapp);
    }

    void RenderBk(IGXApp* gapp, const TUIRect& rc)
    {
        ITuiPic* p = m_hWidget->GetBkPic();
        Render(gapp, p, rc);
    }
};


// CTuiMsg
struct CTuiMsg : public TUIMSG
{
    CTuiMsg(HTUI h, TMUInt m, TMWParam wp, TMLParam lp)
    {
        hWin = 0;
        hWnd = h;
        uObjectID = h->m_dwObjID;
        uID = h->GetDlgCtrlID();
        uTag = h->m_dwTag;
        message = m;
        wParam = wp;
        lParam = lp;
    }

    CTuiMsg(HTUI h, void*) // for spy
    {
        hWin = 0;
        hWnd = h;
        uObjectID = h ? h->m_dwObjID : 0;
        uID = h ? h->GetDlgCtrlID() : 0;
        uTag = h ? h->m_dwTag : 0;
        message = 0;
        wParam = 0;
        lParam = 0;
    }
};

// bind widget
struct CBindWidgetImpl
{
    Ref       m_hBind;
    TMUIntPtr m_dwBindObjID;

private:
    void _BindCreate(HTUI hParent, TMUInt32 dwObjID, IBindOwner* pOwner)
    {
        TMBool bVisible = hParent->Is_WS_VISIBLE();
        hParent->Reset_WS_VISIBLE();
        CTuiWidget wnd;
        wnd.Create(hParent, dwObjID);
        DoBindWin(wnd, hParent, pOwner);
        hParent->Setting_WS_VISIBLE(bVisible);
    }

public:
    CBindWidgetImpl() : m_dwBindObjID(0) {}
    
    void DoBindWin(HTUI hBind, HTUI hParent, IBindOwner* pOwner)
    {
        Ref hOld = m_hBind;
        m_hBind = hBind;
        if (hBind)
        {
            DbgAssert(!hBind->Is_WS_PRI_BINDMODE());
            hBind->Bind_Set(hParent, pOwner);
        }
        if (hOld)
            hOld->DestroyWindow();
    }

    void BindDestroy()
    {
        Ref hOld = m_hBind;
        m_hBind = 0;
        if (hOld)
            hOld->DestroyWindow();
    }

    void Bind_Select() { if (m_hBind) m_hBind->Bind_Select(); }
    void Bind_Unselect() { if (m_hBind) m_hBind->Bind_Unselect(); }

    HTUI BindCreate(PFNBINDBACKUP proc, HTUI hParent, TMUInt32 dwObjID, IBindOwner* pOwner, TMLParam lIDorHITEM, TMLParam lData)
    {
        m_dwBindObjID = dwObjID;
        if (proc)
        {
            BindDestroy();
            return 0;
        }

        _BindCreate(hParent, dwObjID, pOwner);
        return m_hBind;
    }

    void BindReclaim(PFNBINDBACKUP pfn, HTUI hParent, TMLParam lIDorHITEM, TMLParam lData)
    {
        if (pfn && m_hBind && !m_hBind->IsBindSelected())
        {
            if (pfn(hParent, m_hBind, true, lIDorHITEM, lData))
                BindDestroy();
        }
    }

    HTUI BindRestore(PFNBINDBACKUP pfn, HTUI hParent, IBindOwner* pOwner, TMLParam lIDorHITEM, TMLParam lData)
    {
        if (m_hBind)
            return m_hBind;
        if (!m_dwBindObjID || !pfn)
            return 0;
        _BindCreate(hParent, m_dwBindObjID, pOwner);
        pfn(hParent, m_hBind, false, lIDorHITEM, lData);
        return m_hBind;
    }

};

///////////////////////////////////////////////////////////////////////////////
// debug function
#if defined(TUI_EDIT_MODE)

#   define LOG_MSG_FOR_SPY(hw,h,m,w,l)  _Log_Message2 __msgLog(hw,h,m,w,l); if (__msgLog.bBlock) return 0;
#   define LOG_FUNC_FOR_SPY             _Log_Function    _funcLog
#   define LOG_INVALIDATE_FOR_SPY(h)    //Edit_IsObjectInSpyMode(g_hInvalidate) Edit_SpyInvalidate(g_hInvalidate, g_hInvalidate->GetObjectID())

    extern TMBool g_bInSpyMessageOrFunc; // only true can spy trace.

    struct _Log_Base
    {
        TMResult lr;
        int step;
        TMBool bOldSpyMode;
        CCycle cycle;
        _Log_Base()
            : lr(0)
            , bOldSpyMode(g_bInSpyMessageOrFunc)
        {
        }
    };
    
    struct _Log_Base2
    {
        TMResult lr;
        TMBool bOldSpyMode;
        _Log_Base2()
            : lr(0)
            , bOldSpyMode(g_bInSpyMessageOrFunc)
        {
        }
    };

    struct _Log_Message2
        : public CSpyMsg
        , public CTuiMsg
        , public _Log_Base2
    {
        _Log_Message2(HWND hWnd, HTUI h, TMUInt m, TMWParam wp, TMLParam lp)
            : CTuiMsg(h, m, wp, lp)
        {
            DbgAssert(!hWin);
            hWin = hWnd;
            g_bInSpyMessageOrFunc = Edit_IsObjectInSpyMode(h);
            double c = 0;
            if (hWnd && m == CM_MSGROUTE) // skip WIN::CM_MSGROUTE
                message = 0;
            if (g_bInSpyMessageOrFunc && message)
                SpyMsgFunc(*this, eIn, hWin ? eWnd : eWidget, 0);
        }

        ~_Log_Message2()
        {
            if (g_bInSpyMessageOrFunc && message)
            {
                double c = cycle.Elapse().ToMicroSecond();
                SpyMsgFunc(*this, eOut, hWin ? eWnd : eWidget, 0);
            }
            g_bInSpyMessageOrFunc = bOldSpyMode;
        }

        virtual void ToItemString(TM::CString& str, TM::CString& strHint, EMode eMode) tm_override
        {
            TMResult* plr = (eMode == eIn) ? &lr : 0;
            DbgAssert(!bBlock);
            if (hWin)
            {
                str.Format(strFmtAPP, hWin, uObjectID);
                str += DbgFormatMessage(hWin, message, wParam, lParam, plr);
            }
            else
            {
                str.Format(strFmtWGT, hWnd, uObjectID);
                str += DbgFormatMessage(message, wParam, lParam, plr);
            }

            if (eMode == eOut)
                AppendCycle(str);
        }
    };

    struct _Log_Function : public CTuiMsg, public _Log_Base
    {
        CString str;
        TMBool bDisableSpyFunc;

        _Log_Function(HTUI h, LPCTSTR pFmt, ...)
            : CTuiMsg(h, 0)
            , bDisableSpyFunc(CDisableSpyFunc::IsDisable())
        {
            CDisableSpyFunc::IsDisable() = false;
            g_bInSpyMessageOrFunc = h ? Edit_IsObjectInSpyMode(h) : true;
            if (g_bInSpyMessageOrFunc && !bDisableSpyFunc)
            {
                va_list vl;
                va_start(vl,pFmt);
                str.FormatV(SHRT_MAX, 0, pFmt, vl);
                va_end(vl);
                SpyFunction(str, this, 0, step, 0);
            }
        }
        ~_Log_Function()
        {
            if (g_bInSpyMessageOrFunc && !bDisableSpyFunc)
                SpyFunction(str, this, &lr, step, cycle.Elapse().ToMicroSecond());
            g_bInSpyMessageOrFunc = bOldSpyMode;
        }
    };
    
#else

#   define LOG_MSG_FOR_SPY(hw,h,m,w,l)  _Log_Ununsed __msgLog;
#   define LOG_FUNC_FOR_SPY             _DbgTraceUnused

    struct _Log_Ununsed
    {
        TMResult lr;
        _Log_Ununsed()
            : lr(0)
        {
        }
    };
    struct _Log_Function
    {
        _Log_Function(...) {}
    };

    class CDisableSpyFunc {};


#endif // TUI_EDIT_MODE


TM_END_NAMESPACE

#endif // UIWIDGET_H
