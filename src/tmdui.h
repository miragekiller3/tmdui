/**
 *  @file
 *  @brief  Template Direct UI Library
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.14
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef TMDUI_H
#define TMDUI_H

#include "tmnative.h"
#include "tmdom.h"
#include "tmgdix.h"
#include "tmcolor.h"
#include "tmstrptr.h"
#include "tmkeyboard.h"

#ifdef TM_WIN_DESKTOP
#   include <mshtml.h>
#   include <mshtmhst.h>
#   include <commctrl.h>
#elif defined(TM_APPLE)
#endif // TM_WIN_DESKTOP


struct GXFont;
struct IRichContext;
struct IEase;
struct RICHCURSOR;

// next version:
// 1. app::team change to function?
#define tuiGetAPP tuiGetApp20220927


#define TUI_FILE_VER_090710     20090710
#define TUI_FILE_VER_100403     20100403
#define TUI_FILE_VER_120208     20120208
#define TUI_FILE_VER_120215     20120215
#define TUI_FILE_VER_120720     20120720
#define TUI_FILE_VER_140213     20140213
#define TUI_FILE_VER_160921     20160921 // color from BGR to RGB, font height from PT to PX
#define TUI_FILE_VER_181218     20181218 // support sub dir res file
#define TUI_FILE_VER_191018     20191018 // URL encode -> HTML encode
#define TUI_FILE_VER_191219     20191219 // modify DS_FULLSCREEN, DS_CENTERSCREEN value.
#define TUI_FILE_VER_210202     20210202 // support color index
#define TUI_FILE_VER_CURRENT    TUI_FILE_VER_210202
#define TUI_FILE_VER_STRING     TM_T(TM_NUMBER_OF(TUI_FILE_VER_CURRENT))



////////////////////////////////////////////////////////////////////////////////
// preprocessing
#define TUI_BGN_NAMESPACE                   namespace TM {
#define TUI_END_NAMESPACE                   }


TUI_BGN_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
// replace TMHWnd with HTUI
class   CTuiWgt;
class   CTuiApp;
class   CTuiWidget;
struct  ITuiPic;
tm_enum (EDragEff);

typedef CTuiWgt*        HTUI;

typedef TMRectF         TUIRect;
typedef TMPointF        TUIPoint;
typedef TMSizeF         TUISize;
typedef TM::CRectF      CTuiRect;
typedef TM::CSizeF      CTuiSize;
typedef TM::CPointF     CTuiPoint;
typedef TM::CZoom       CTuiZoom;

template <class T, class T_hwnd, class T_traits> struct _CTuiWidgetProc;


////////////////////////////////////////////////////////////////////////////////
// TMDui predefined string class string
#define _CS(x)                          TM_T(x) // prefix to class name
#define TUI_CLASS_WIN_MAIN              _CS("TMDui_frame") // windows class
#define TUI_CLASS_WIN_FLOAT             _CS("TMDui_float_window")
#define TUI_CLASS_WIN_CHROME            _CS("TMDui_chrome_window")
#define TUI_CLASS_WIN_CHROME_W          _CS("TMDui_chrome_window_tab_walker")
#define TUI_CLASS_WIN_GHOST             _CS("TMDui_ghost_window")

#define TUI_CLASS_WINDOW                _CS("window")
#define TUI_CLASS_WIDGET                _CS("widget")
#define TUI_CLASS_STATIC                _CS("static")
#define TUI_CLASS_PUSHBUTTON            _CS("pushbutton")
#define TUI_CLASS_CHECKBOX              _CS("checkbox")
#define TUI_CLASS_RADIOBUTTON           _CS("radiobutton")
#define TUI_CLASS_TABBUTTON             _CS("tabbutton")
#define TUI_CLASS_HEADER                _CS("header")
#define TUI_CLASS_COMBOBUTTON           _CS("combobutton")
#define TUI_CLASS_SYS                   _CS("sys")
#define TUI_CLASS_GROUPBOX              _CS("groupbox")
#define TUI_CLASS_MENUITEM              _CS("menuitem")
#define TUI_CLASS_EDIT                  _CS("edit")
#define TUI_CLASS_MEMO                  _CS("memo")
#define TUI_CLASS_RICH                  _CS("rich")
#define TUI_CLASS_PROGRESS              _CS("progress")
#define TUI_CLASS_SCROLLBOX             _CS("scrollbox")
#define TUI_CLASS_MENUBAR               _CS("menubar")
#define TUI_CLASS_ANIMATE               _CS("animate")
#define TUI_CLASS_VIDEO                 _CS("video")
#define TUI_CLASS_TREE                  _CS("tree")
#define TUI_CLASS_LISTBOX               _CS("listbox")
#define TUI_CLASS_LAYER                 _CS("layer")
#define TUI_CLASS_FLOAT                 _CS("float")
#define TUI_CLASS_DUMMY                 _CS("dummy")
#define TUI_CLASS_ARROW                 _CS("arrow")
#define TUI_CLASS_HOTKEY                _CS("hotkey")
#define TUI_CLASS_ACTIVEX               _CS("activex")
#define TUI_CLASS_WEBVIEW               _CS("webview")
#define TUI_CLASS_TRACKBAR              _CS("trackbar")
#define TUI_CLASS_UPDOWN                _CS("updown")
#define TUI_CLASS_SPLITTER              _CS("splitter")
#define TUI_CLASS_REFER                 _CS("refer")

#define TUI_CLASS_FRM_FORM              _CS("form")

#define TUI_CLASS_PIC_CLOR              _CS("clr")
#define TUI_CLASS_PIC_GRADIENT          _CS("gradient")
#define TUI_CLASS_PIC_BMP               _CS("bmp")
#define TUI_CLASS_PIC_ANI               _CS("ani")
#define TUI_CLASS_PIC_TILE              _CS("tile")
#define TUI_CLASS_PIC_SCROLLBAR         _CS("scrollbar")

////////////////////////////////////////////////////////////////////////////////
// TMDui predefined XML string
#define TUI_XML_NODE0_TMDUI             _CS("tmdui")        // main, lang xml
#define TUI_XML_NODE1_WND               _CS("wnd")          // main xml
#define TUI_XML_NODE1_FRM               _CS("frame")        // main xml
#define TUI_XML_NODE1_PIC               _CS("pic")          // main xml

#define TUI_XML_NODE1_RESTUI            _CS("res_tui")      // lang xml
#define TUI_XML_NODE1_RESSYS            _CS("res_sys")      // lang xml
#define TUI_XML_NODE2_STRING            _CS("message")      // lang xml
#define TUI_XML_NODE2_MENU              _CS("menu")         // lang xml
#define TUI_XML_NODE2_MENUPOP           _CS("menupop")      // lang xml

#define TUI_RES_XML_NAME                _CS("tuires.xml")   // default xml file resource name  tuires.xml
#define TUI_RES_XML_FILE                _CS("tuires.xml")   // default xml file name
#define TUI_RES_PIC_TYPE                _CS("TUIPIC")       // default bmp file resource type

// XML property
#define TUI_XML_PROP_LCID               _CS("localid")      // lang xml
#define TUI_XML_PROP_VERSIOM            _CS("version")      // main xml
#define TUI_XML_PROP_ENCODING           _CS("encoding")     // main xml

// for _tuiApp::LoadString
#define TUI_XML_PROP_TEXT               _CS("Text")
#define TUI_XML_PROP_HINT               _CS("Hint")
#define TUI_XML_PROP_LISTITEMS          _CS("ListItems")
#define TUI_XML_PROP_FORMAT             _CS("Format")
#define TUI_XML_PROP_SRC                _CS("src")
#define TUI_XML_PROP_FILENAME           _CS("FileName")

// windows prop
#define WIN_PROP_TYPE_MGR               0x01
#define WIN_PROP_TYPE_FLOAT             0x02
#define WIN_PROP_TYPE_DUMMY             0x04
#define WIN_PROP_TYPE                   _CS("TUI_TYPE")

#ifndef TUI_ACC_DLL64
#   define TUI_ACC_DLL64                _CS("TMDuiAcc.dll")
#endif // #ifndef TUI_ACC_DLL64
#ifndef TUI_ACC_DLL
#   define TUI_ACC_DLL                  _CS("TMDuiAcc.dll")
#endif // #ifndef TUI_ACC_DLL

// tree item
#define TUI_CLASS_TREENODE              _CS("#treeitem")

////////////////////////////////////////////////////////////////////////////////
// predefined object id
#define IDP_PRE_BIND_BASE               20
#define IDP_PRE_BIND_CHECK              20  // check,    widget check <=> ischeck
#define IDP_PRE_BIND_EXPAND             21  // expand,   widget check <=> isexpand
#define IDP_PRE_BIND_INDENT             22  // indent,   widget width <=  indent width
#define IDP_PRE_BIND_LABLE              23  // text,     widget text  <=> text
#define IDP_PRE_BIND_SELECTED           24  // selected, widget check <=  isselect


#define IDP_PRE_OBJECT_BASE             30
#define IDP_PRE_OBJECT_MAX              200

#define IDP_PRE_PIC_NULL                70
#define IDP_PRE_PIC_BUTTON              71  // push button bk
#define IDP_PRE_PIC_CHECK               72  // check box fore
#define IDP_PRE_PIC_RADIO               73  // radio button fore
#define IDP_PRE_PIC_GROUP               74  // group bk
#define IDP_PRE_PIC_COMB                75  // comb button fore
#define IDP_PRE_PIC_LIST_ITEM           76  // list box item bk
#define IDP_PRE_PIC_MENU                77  // menu bar item bk
#define IDP_PRE_PIC_MENU_POP            78  // menu item item bk
#define IDP_PRE_PIC_HEADER_UP           79  // header button fore
#define IDP_PRE_PIC_HEADER_DOWN         80  // header button fore
#define IDP_PRE_PIC_UP                  81  // up/down fore&bk
#define IDP_PRE_PIC_DOWN                82  // up/down fore&bk
#define IDP_PRE_PIC_FOCUS               83  // draw focus rect

#define IDP_PRE_PIC_SCROLLBAR           85
#define IDP_PRE_PIC_SIZEBOX             86
#define IDP_PRE_PIC_OUTERBORDER         87
#define IDP_PRE_PIC_INNERBORDER         88

#define IDP_PRE_PIC_CAPTION             90
#define IDP_PRE_PIC_CLOSE               91
#define IDP_PRE_PIC_MIN                 92
#define IDP_PRE_PIC_MAX                 93
#define IDP_PRE_PIC_HELP                94





////////////////////////////////////////////////////////////////////////////////
// predefined property id
enum EAppProperty                                           // action   type        default
{
    eAppPropTargetDefault               = 0x00000000,
    eAppPropTargetHint                  = 0x01000000,
    eAppPropTargetEdit                  = 0x02000000,
    eAppPropTargetEditPrompt            = 0x03000000,
    eAppPropTargetDisabled              = 0x04000000,

    eAppPropIDColorBk                   = 0x00000001,
    eAppPropIDColorBkSel                = 0x00000002,
    eAppPropIDColorText                 = 0x00000003,
    eAppPropIDColorTextSel              = 0x00000004,

    ///////////////////////////////////////////////////////////////////////////
    // general
    eAppPropAnimationButton             = 0x00000010,   // get/put  TMBool      false
    eAppPropAnimationScroll             = 0x00000011,   // get/put  TMBool      false
    eAppPropAnimationSysBut             = 0x00000012,   // get/put  TMBool      false
    eAppPropHighContrast                = 0x00000013,   // get/put  TMBool      false
    eAppPropUseSystemFrame              = 0x00000014,   // get/put  TMBool      false
    eAppPropShowHintOnEllipsis          = 0x00000016,   // get/put  TMBool      false
    eAppPropShowHintOnTabKey            = 0x00000017,   // get/put  TMBool      false

    // webview
    eAppPropWebview2SSOUsingOSA         = 0x00000018,   // get/put  TMBool      false
    eAppPropWebview2Arguments           = 0x00000019,   // put      TMCStr    ""
    eAppPropWebview2Language            = 0x00000020,   // put      TMCStr    ""
    eAppPropWebview2ExecutableFolder    = 0x00000021,   // put      TMCStr    ""
    eAppPropWebview2UserDataFolder      = 0x00000022,   // put      TMCStr    ""

    // all text
    eAppPropDisabledColorText           = eAppPropTargetDisabled | eAppPropIDColorText,     // get/put  TMColor     TMRGB(128,128,128)

    // tooltip
    eAppPropHintColorBk                 = eAppPropTargetHint | eAppPropIDColorBk,           // get/put  TMColor     cColorInvalid
    eAppPropHintColorText               = eAppPropTargetHint | eAppPropIDColorText,         // get/put  TMColor     cColorInvalid
    eAppPropHintLayered                 = eAppPropTargetHint | 0xff,                        // get/put  TMBool      false

    // edit
    eAppPropEditColorBkSel              = eAppPropTargetEdit | eAppPropIDColorBkSel,        // get/put  TMColor     TMRGB(0x04, 0x9f, 0xd9)
    eAppPropEditColorTextSel            = eAppPropTargetEdit | eAppPropIDColorTextSel,      // get/put  TMColor     TMRGB(255, 255, 255)
    eAppPropEditColorPrompt             = eAppPropTargetEditPrompt | eAppPropIDColorText,   // get/put  TMColor     TMRGB(192,192,192)
};

#define TUI_DPI_SYSTEM_DEFAULT          TM_DPI_SYSTEM_DEFAULT

////////////////////////////////////////////////////////////////////////////////
// new style
// WS_CAPTION
// WS_BORDER
// WS_DLGFRAME
// WS_SYSMENU
// WS_SIZEBOX
// WS_DISABLED
// WS_TABSTOP
// WS_VISIBLE
//const TMUInt32 WS_L2RREADING            = 0x00100000L; // WS_HSCROLL
const TMUInt32 WS_GROUPTABSTOP          = 0x00200000L; // WS_VSCROLL

// static, float, mgr
const TMUInt32 SS_RENDERBKONHC          = 0x0000; // render bk-pic on HC;
const TMUInt32 SS_WINDOWBKONHC          = 0x0001; // HC mode render bk as COLOR_WINDOW
const TMUInt32 SS_DLGBKONHC             = 0x0002; // HC mode render bk as COLOR_BTNFACE
const TMUInt32 SS_CLEARBKONHC           = 0x0003; // HC mode render bk as RGBA(0,0,0,0)
const TMUInt32 SS_NULLBKONHC            = 0x000f; // HC mode don't render bk
const TMUInt32 SS_BKONHCMASK            = 0x000f; // mask of SS_XXXBKONHC,
const TMUInt32 SS_NULLBKONNORMAL        = 0x0010; // don't render bk-pic on normal.

// button
#define BS_AUTOSTATE                    0x0006L     // BS_AUTO3STATE
#define BS_DRAWFOCURECT                 0x0010L
#define BS_WINDOWFOREONHC               0x0020L     // display default fore pic on HC
#define BS_NOCLICKFOCUS                 0x0040L
#define BS_PRESSCLICK                   0x0080L
// #define RS_AUTOTABSTOP                  0x2000L     // for radio,tab,head
#define MS_KEEPPOPUP                    0x8000L     // for menu item
#define TS_NOCTRLTAB                    0x1000L     // for tab button

#if !TM_WIN
#   define BS_NOTIFY                    0x4000L
#endif

// combo button
#define CBS_CLICKTOPOP                  0x0100L         // user click edit then pop list
#define CBS_ARROWTOPOP                  0x0200L         // user input up/down key then pop list
#define CBS_SPACETOPOP                  0x0400L         // user input space key then pop list
#define CBS_FILTER                      0x0800L
#define CBS_NOIMEDIALTY                 0x1000L
#define CBS_NOEDIT                      0x2000L
#define CBS_NOSYNCHINT                  0x8000L

// video
#define VS_PLAYING                      0x0002L
#define VS_MUTED                        0x0004L
#define VS_NOTIFY                       0x4000L
#define VS_KEEPAR                       0x8000L         // keep aspect ratio
#define VS_R2L                          0x0010L         // GX_BLT_R2L
#define VS_B2T                          0x0020L         // GX_BLT_B2T
#define VS_X2Y                          0x0040L         // GX_BLT_X2Y
#define VS_ROTATE_MASK                  0x0070L         // GX_BLT_ROTATE_MASK
#define VS_ROTATE_0                     0x0000L         // GX_BLT_ROTATE_0
#define VS_ROTATE_0R                    (VS_R2L)
#define VS_ROTATE_90                    (VS_X2Y | VS_B2T)
#define VS_ROTATE_90R                   (VS_X2Y | VS_B2T | VS_R2L)
#define VS_ROTATE_180                   (VS_B2T | VS_R2L)
#define VS_ROTATE_180R                  (VS_B2T)
#define VS_ROTATE_270                   (VS_X2Y | VS_R2L)
#define VS_ROTATE_270R                  (VS_X2Y)


// mgr
const TMUInt32 DS_FULLSCREEN            = 0x0100;
const TMUInt32 DS_NODWMSHADOW           = 0x0200;       // only for eThemeTypeDWM, eThemeTypeDWMBlur
const TMUInt32 DS_CENTERSCREEN          = 0x0400;
const TMUInt32 DS_CENTEROWNER           = DS_CENTER;    // 0x0800L
const TMUInt32 DS_DWMBLUR               = 0x1000;       // eThemeTypeDWMBlur
const TMUInt32 DS_NODWMTRANSPARENT      = 0x2000;       // don't use eThemeTypeDWMTransparent to instead eThemeTypeLayered


////////////////////////////////////////////////////////////////////////////////
// new ex-style, replace WindowExStyle,WindowStyle is compatible with the original
const TMUInt32 WS_ADV_L2RHINT           = 0x00000004L;  //WS_EX_NOPARENTNOTIFY
const TMUInt32 WS_ADV_PENETRATE         = 0x00000020;   //WS_EX_TRANSPARENT - mouse penetration
const TMUInt32 WS_ADV_CLIENTEDGE        = 0x00000200;   //WS_EX_CLIENTEDGE;
const TMUInt32 WS_ADV_ARROWGROUP        = 0x00004000;
const TMUInt32 WS_ADV_TABGROUP          = 0x00010000L;  // not WS_EX_CONTROLPARENT


// only for window widget
const TMUInt32 WS_ADV_LAYERED           = 0x00080000;   // WS_EX_LAYERED;
const TMUInt32 WS_ADV_TOPMOST           = 0x00000008;   //WS_EX_TOPMOST;
const TMUInt32 WS_ADV_CONTEXTHELP       = 0x00000400;   //WS_EX_CONTEXTHELP;
const TMUInt32 WS_ADV_TOOLWINDOW        = 0x00000080;   //WS_EX_TOOLWINDOW;
const TMUInt32 WS_ADV_NOACTIVATE        = 0x08000000;   //WS_EX_NOACTIVATE

// only for child widget
const TMUInt32 WS_ADV_ATTACHMENT        = 0x00000010;   // share ButtonState with parent widget, only for sub-window
const TMUInt32 WS_ADV_ROUTER            = 0x00000080;   // the widget can receive route manage


// layout
const TMUInt32 LAYOUT_MODE_MASK         = 0x000f;
const TMUInt32 LAYOUT_HMASK             = 0x00f0;
const TMUInt32 LAYOUT_VMASK             = 0x0f00;
const TMUInt32 LAYOUT_MASK              = 0x0fff;

const TMUInt32 LAYOUT_ANCHOR            = 0;
const TMUInt32 LAYOUT_ALIGN             = 0x0001;
const TMUInt32 LAYOUT_FLOW              = 0x0002;
const TMUInt32 LAYOUT_SCROLL            = 0x0008;

const TMUInt32 LAYOUT_LEFT              = 0;
const TMUInt32 LAYOUT_CENTER            = 0x010;
const TMUInt32 LAYOUT_RIGHT             = 0x020;
const TMUInt32 LAYOUT_FRAME             = 0x030;
const TMUInt32 LAYOUT_NONE              = 0x040;
const TMUInt32 LAYOUT_LCENTER           = 0x050;
const TMUInt32 LAYOUT_RCENTER           = 0x060;

const TMUInt32 LAYOUT_TOP               = 0;
const TMUInt32 LAYOUT_VCENTER           = 0x100;
const TMUInt32 LAYOUT_BOTTOM            = 0x200;
const TMUInt32 LAYOUT_VFRAME            = 0x300;
const TMUInt32 LAYOUT_VNONE             = 0x400;
const TMUInt32 LAYOUT_TCENTER           = 0x500;
const TMUInt32 LAYOUT_BCENTER           = 0x600;

const TMUInt32 LAYOUT_AUTOWIDTH         = 0x00001000;
const TMUInt32 LAYOUT_AUTOHSCROLL       = 0x00002000;
const TMUInt32 LAYOUT_HSCROLL           = 0x00004000;
const TMUInt32 LAYOUT_AUTOHEIGHT        = 0x00010000;
const TMUInt32 LAYOUT_AUTOVSCROLL       = 0x00020000;
const TMUInt32 LAYOUT_VSCROLL           = 0x00040000;
const TMUInt32 LAYOUT_FLOATSCROLL       = 0x00100000;   // client rect include scrollbar
const TMUInt32 LAYOUT_HOTSCROLL         = 0x00300000;   // invisible when mouse out of widget.


// Anchor attributes
// common horizontal arrangement ways: center,left ,both sides, non-alignment,left alignment by default
const TMUInt32 ANCHOR_LEFT              = LAYOUT_ANCHOR | LAYOUT_LEFT   ;
const TMUInt32 ANCHOR_CENTER            = LAYOUT_ANCHOR | LAYOUT_CENTER ;
const TMUInt32 ANCHOR_RIGHT             = LAYOUT_ANCHOR | LAYOUT_RIGHT  ;
const TMUInt32 ANCHOR_FRAME             = LAYOUT_ANCHOR | LAYOUT_FRAME  ;
const TMUInt32 ANCHOR_NONE              = LAYOUT_ANCHOR | LAYOUT_NONE   ;
const TMUInt32 ANCHOR_LCENTER           = LAYOUT_ANCHOR | LAYOUT_LCENTER;
const TMUInt32 ANCHOR_RCENTER           = LAYOUT_ANCHOR | LAYOUT_RCENTER;
const TMUInt32 ANCHOR_HMASK             = LAYOUT_MODE_MASK | LAYOUT_HMASK;

// common vertical arrangement ways: center,left,both sides,non-alignment,up alignment by default
const TMUInt32 ANCHOR_TOP               = LAYOUT_ANCHOR | LAYOUT_TOP    ;
const TMUInt32 ANCHOR_VCENTER           = LAYOUT_ANCHOR | LAYOUT_VCENTER;
const TMUInt32 ANCHOR_BOTTOM            = LAYOUT_ANCHOR | LAYOUT_BOTTOM ;
const TMUInt32 ANCHOR_VFRAME            = LAYOUT_ANCHOR | LAYOUT_VFRAME ;
const TMUInt32 ANCHOR_VNONE             = LAYOUT_ANCHOR | LAYOUT_VNONE  ;
const TMUInt32 ANCHOR_TCENTER           = LAYOUT_ANCHOR | LAYOUT_TCENTER;
const TMUInt32 ANCHOR_BCENTER           = LAYOUT_ANCHOR | LAYOUT_BCENTER;
const TMUInt32 ANCHOR_VMASK             = LAYOUT_MODE_MASK | LAYOUT_VMASK;

// common side align widget:left, right, up, down, all of the client area
// side align widget will change the parent widget's client rect
// after set side widget attributes, Anchor attribute will be ignored
const TMUInt32 ALIGN_LEFT               = 0x001;    // 0000 0001   h
const TMUInt32 ALIGN_RIGHT              = 0x011;    // 0001 0001   h
const TMUInt32 ALIGN_TOP                = 0x021;    // 0010 0001   v
const TMUInt32 ALIGN_BOTTOM             = 0x031;    // 0011 0001   v
const TMUInt32 ALIGN_CLIENT             = 0x041;    // 0100 0001

const TMUInt32 ALIGN_REVERSE            = 0x080;    // 1000 0000
const TMUInt32 ALIGN_LEFT_RIGHT         = ALIGN_REVERSE | ALIGN_LEFT;   // 1000 0001   h
const TMUInt32 ALIGN_RIGHT_RIGHT        = ALIGN_REVERSE | ALIGN_RIGHT;  // 1001 0001   h
const TMUInt32 ALIGN_TOP_BOTTOM         = ALIGN_REVERSE | ALIGN_TOP;    // 1010 0001   v
const TMUInt32 ALIGN_BOTTOM_BOTTOM      = ALIGN_REVERSE | ALIGN_BOTTOM; // 1011 0001   v
const TMUInt32 ALIGN_MASK               = 0x1ff;
const TMUInt32 ALIGN_TYPE_MASK          = 0x0f0;

const TMUInt32 ALIGN_SCROLL             = 0x100;
const TMUInt32 ALIGN_HEADER             = ALIGN_SCROLL | ALIGN_TOP;     // 1 0010 0001   h

const TMUInt32 ALIGN_DIR_MASK           = 0x06f;    // 0110 1111
const TMUInt32 ALIGN_DIR_H              = 0x001;    // 0000 0001
const TMUInt32 ALIGN_DIR_V              = 0x021;    // 0010 0001
#define LAYOUT_IS_ALING_H(x)            ((x & ALIGN_DIR_MASK) == ALIGN_DIR_H)
#define LAYOUT_IS_ALING_V(x)            ((x & ALIGN_DIR_MASK) == ALIGN_DIR_V)


// flow
const TMUInt32 FLOW_TOP                 = 0x002;
const TMUInt32 FLOW_MIDDLE              = 0x012;
const TMUInt32 FLOW_BOTTOM              = 0x022;
const TMUInt32 FLOW_BREAK               = 0x032;
const TMUInt32 FLOW_MASK                = 0x0ff;


enum EObjType
{
    e_otUnknown = -1,
    e_otWnd     = 0,
    e_otFrm     = 1,
    e_otPic     = 2,
    e_otWgt     = 3,

    e_otMin     = 0,
    e_otMax     = 4,
};


enum EIDTYPE
{
    EM_NULL     = 0,

    EM_PIC      = 1 << TM::e_otPic,     // 0x04
    EM_FRAME    = 1 << TM::e_otFrm,     // 0x02
    EM_WGT      = 1 << TM::e_otWgt,     // 0x08
    EM_DIALOG   = 1 << TM::e_otWnd,     // 0x01
    EM_CONTROL  = 1 << 5,               // 0x20

    EM_ALLOBJ   = EM_PIC | EM_FRAME | EM_WGT | EM_DIALOG | EM_CONTROL,

    EM_PRE      = 1 << 8,
    EM_STRING   = 1 << 9,
    EM_MENU     = 1 << 10,
    EM_ACCEL    = 1 << 11,
    EM_RC       = 1 << 12,
    EM_WIN      = EM_PRE | EM_STRING | EM_MENU | EM_ACCEL | EM_RC,

    EM_NEXT     = 1 << 15,
    EM_OWNER    = 1 << 16,

    EM_ALL      = -1,
};
TM_ENUM_2_FLAG(EIDTYPE);

enum EBindState
{
    eBindStateNone      = 0,    // the widget is not bind widget
    eBindStateSelf      = 1,    // the widget is bind widget
    eBindStateParent    = 2,    // the widget is bind widget's child widget
    eBindStateCBList    = 3,    // the widget is combo::listbox[tree]

    eBindStateSelfMask  = 0x01,
};

// ChildWindowFromPoint
#define CWP_SKIPBINDWINDOW  0x0008


////////////////////////////////////////////////////////////////////////////////
// extend GetWindow function
#define GW_GLOBALNEXT           10
#define GW_GLOBALPREV           11
#define GW_TABNEXT              12
#define GW_TABPREV              13

// sort child
// return A < B ? true : false;
typedef int (CALLBACK *PFNWWCOMPARE)(HTUI hWnd1, HTUI hWnd2, TMLParam lParamSort);

// enum childs; return TMTrue To continue enumeration;
typedef TMBool (CALLBACK *PFNWWENUMPROC) (HTUI hwnd, TMLParam lParam);


// return data1 < data2 ? true : false;
typedef bool (CALLBACK *PFNCOMPARE)(TMLParam data1, TMLParam data2, TMLParam lParamSort);


typedef TMBool (*PFNWindowProc)(void* pVoid, TMHWnd hWnd, TMUInt uMsg, TMWParam wParam,
                             TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID);
typedef TMBool (*PFNWidgetProc)(void* pVoid, HTUI hWnd, TMUInt uMsg, TMWParam wParam,
                             TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID);





///////////////////////////////////////////////////////////////////////////////
// TUIFACTORY
struct TUIFACTORY
{
    typedef void (*PFNCreator)(HTUI hWgt, void* param1, void* param2);
    PFNCreator creator;
    void* param1;
    void* param2;
};



///////////////////////////////////////////////////////////////////////////////
// CMsgDelegateT
template <class T> struct CMsgDelegateT;
typedef CMsgDelegateT<TMHWnd> CWindowMsgDelegate;
typedef CMsgDelegateT<HTUI>   CWidgetMsgDelegate;


////////////////////////////////////////////////////////////////////////////////
// state
enum EButState
{
    // button
    e_bsNormal          = 0,
    e_bsDown            = 1,
    e_bsHigh            = 2,
    e_bsDisable         = 3,
    e_bsFocus           = 4,            // focus or default item
    e_bsEnd             = 5,

    e_bsEditor          = 0x20000000,   // for editor render pic

};
// TM_ENUM_2_FLAG(EButState);


enum EButStates
{
    // button
    e_bssNormal         = 0x0000,
    e_bssDown           = 0x0100,
    e_bssHigh           = 0x0200,
    e_bssDisable        = 0x0400,
    e_bssFocus          = 0x0800,

    // scroll bar
    e_sssUnactive       = e_bssNormal,
    e_sssDown           = e_bssDown,
    e_sssActive         = e_bssHigh,    // mouse on scrollbar if LAYOUT_FLOATSCROLL
    e_sssDisable        = e_bssDisable,
    e_sssHover          = e_bssFocus,   // mouse on object.

    e_bssDefault        = 0x1000,       // default item or focus push button.
    e_bssHideFocus      = 0x2000,       // is hide focus.
    e_bssHide           = 0x8000,
    e_bssNumMask        = 0x00ff,
};
TM_ENUM_2_FLAG(EButStates);

// return >= 0;
TM_INLINE int ButStates2ButFrameNum(EButStates ss)
{
    int n = ss & e_bssNumMask;
    return n;
}

TM_INLINE EButState ButStates2ButFrame(EButStates ss)
{
    if (ss & e_bssDisable)
        return e_bsDisable;
    if (ss & e_bssDown)
        return e_bsDown;
    if (ss & e_bssHigh)
        return e_bsHigh;
    if ((ss & e_bssDefault) ||
        (e_bssFocus == (ss & (e_bssFocus | e_bssHideFocus))))
        return e_bsFocus;
    return e_bsNormal;
}

TM_INLINE EButState ButStates2ButFrames(EButStates ss)
{
    EButState bs = ButStates2ButFrame(ss);
    int n = ButStates2ButFrameNum(ss);
    return (EButState)(n * e_bsEnd + bs);
}


// lParam of WM_SETFOCUS and WM_KILLFOCUS
enum EFocusReason
{
    eFocusReasonUnknown,        // Another reason, usually application-specific.
    eFocusReasonMouse,          // A mouse action occurred.
    eFocusReasonTab,            // The Tab, arrow key was pressed.
    eFocusReasonBacktab,        // A Backtab occurred.
    eFocusReasonActiveWindow,   // The window system made this window either active or inactive.
    eFocusReasonPopup,          // The application opened/closed a pop-up that grabbed/released the keyboard focus.
    eFocusReasonShortcut,       // The user typed a label's buddy shortcut
    eFocusReasonMenuBar,        // The menu bar, menu item took focus.
};



////////////////////////////////////////////////////////////////////////////////
// notify code: for WM_COMMAND
#define CN_VALUECHANGE          0x0301   // send when control value changed.



////////////////////////////////////////////////////////////////////////////////
// new added windows messages
// #define CM_LAYOUT               (CM_BASE + 304) // rearrange sub-windows based on Align attribute,lParam:const TMRect*; rect before parent changed
// #define CM_LAYOUTCHILD          (CM_BASE + 305) // enumerate sub-windows and send CM_LAYOUT message,lParam:const TMRect*;rect before parent changed

#define CM_BASE                 WM_APP // 0x8000  32768
#define CM_END                  (CM_BASE + 0x800)

enum EHitObj
{
    e_objOut            = -2,       // HTERROR,
    e_objWindow         = 0,        // HTNOWHERE

    e_objClient         = 1,        // HTCLIENT,
    e_objCaption        = 2,        // HTCAPTION,
    e_objHScroll        = 6,        // HTHSCROLL,
    e_objVScroll        = 7,        // HTVSCROLL,

    e_objMin            = 8,        // HTMINBUTTON,
    e_objMax            = 9,        // HTMAXBUTTON,
    e_objClose          = 20,       // HTCLOSE,
    e_objSys            = 3,        // HTSYSMENU,
    e_objHelp           = 21,       // HTHELP,

    e_objOBorder        = 30,
    e_objIBorder        ,
};

enum EHitSubObj
{
    // scroll bar
    e_objSBLineUp       = 0,        // SB_LINEUP,
    e_objSBLineDown     = 1,        // SB_LINEDOWN,
    e_objSBPageUp       = 2,        // SB_PAGEUP,
    e_objSBPageDown     = 3,        // SB_PAGEDOWN,
    e_objSBThumb        = 4,        // SB_THUMBPOSITION,
    e_objSBFirst        = e_objSBLineUp,
    e_objSBLast         = e_objSBThumb,

    // outer border :: size
    e_objSZLeft         = 1,        // WMSZ_LEFT,
    e_objSZRight        = 2,        // WMSZ_RIGHT,
    e_objSZTop          = 3,        // WMSZ_TOP,
    e_objSZTopLeft      = 4,        // WMSZ_TOPLEFT,
    e_objSZTopRight     = 5,        // WMSZ_TOPRIGHT,
    e_objSZBottom       = 6,        // WMSZ_BOTTOM,
    e_objSZBottomLeft   = 7,        // WMSZ_BOTTOMLEFT,
    e_objSZBottomRight  = 8,        // WMSZ_BOTTOMRIGHT,
    e_objSZFirst        = e_objSZLeft,
    e_objSZLast         = e_objSZBottomRight,
};


#define _TUIHITMAKE(obj,subobj)     (((((TMUInt32)obj) << 8) + (subobj)))
#define TUIHITMAKE(obj,subobj)      ((EHitID)_TUIHITMAKE(obj,subobj))
#define TUIHITGETOBJ(x)             ((EHitObj)(((TMInt32)((TMUInt32)(x))) >> 8))    // EHitID -> e_objHScroll, preserve negative EHitObj values
#define TUIHITGETSUBOBJ(x)          ((EHitSubObj)(x & 0xff))                        // EHitID -> e_objSBLineUp
#define TUIHITGETSUBSB(x)           ((EHitSubObj)(x & 0xff))                        // EHitID -> SB_XXX
#define TUIHITGETSUBSZ(x)           ((EHitSubObj)(x & 0xff))                        // EHitID -> WMSZ_XXX

enum EHitID
{
    eHitIDOut           = _TUIHITMAKE(e_objOut,      0),
    eHitIDWindow        = _TUIHITMAKE(e_objWindow,   0),

    eHitIDClient        = _TUIHITMAKE(e_objClient,   0),
    eHitIDCaption       = _TUIHITMAKE(e_objCaption,  0),

    eHitIDMin           = _TUIHITMAKE(e_objMin,      0),
    eHitIDMax           = _TUIHITMAKE(e_objMax,      0),
    eHitIDClose         = _TUIHITMAKE(e_objClose,    0),
    eHitIDSys           = _TUIHITMAKE(e_objSys,      0),
    eHitIDHelp          = _TUIHITMAKE(e_objHelp,     0),

    eHitIDIBorder       = _TUIHITMAKE(e_objIBorder,  0),

    // O border
    eHitIDOBorder       = _TUIHITMAKE(e_objOBorder,  0),
    eHitIDTopLeft       = _TUIHITMAKE(e_objOBorder,  e_objSZTopLeft),
    eHitIDTopRight      = _TUIHITMAKE(e_objOBorder,  e_objSZTopRight),
    eHitIDTop           = _TUIHITMAKE(e_objOBorder,  e_objSZTop),
    eHitIDBottomLeft    = _TUIHITMAKE(e_objOBorder,  e_objSZBottomLeft),
    eHitIDBottomRight   = _TUIHITMAKE(e_objOBorder,  e_objSZBottomRight),
    eHitIDBottom        = _TUIHITMAKE(e_objOBorder,  e_objSZBottom),
    eHitIDLeft          = _TUIHITMAKE(e_objOBorder,  e_objSZLeft),
    eHitIDRight         = _TUIHITMAKE(e_objOBorder,  e_objSZRight),

    // H scrollbar
    eHitIDHScroll       = _TUIHITMAKE(e_objHScroll,  0),
    eHitIDHLineUp       = _TUIHITMAKE(e_objHScroll,  e_objSBLineUp),
    eHitIDHLineDown     = _TUIHITMAKE(e_objHScroll,  e_objSBLineDown),
    eHitIDHPageUp       = _TUIHITMAKE(e_objHScroll,  e_objSBPageUp),
    eHitIDHPageDown     = _TUIHITMAKE(e_objHScroll,  e_objSBPageDown),
    eHitIDHThumb        = _TUIHITMAKE(e_objHScroll,  e_objSBThumb),

    // V scrollbar
    eHitIDVScroll       = _TUIHITMAKE(e_objVScroll,  0),
    eHitIDVLineUp       = _TUIHITMAKE(e_objVScroll,  e_objSBLineUp),
    eHitIDVLineDown     = _TUIHITMAKE(e_objVScroll,  e_objSBLineDown),
    eHitIDVPageUp       = _TUIHITMAKE(e_objVScroll,  e_objSBPageUp),
    eHitIDVPageDown     = _TUIHITMAKE(e_objVScroll,  e_objSBPageDown),
    eHitIDVThumb        = _TUIHITMAKE(e_objVScroll,  e_objSBThumb),
};

struct TUIHITINFO
{
    DbgAssertS1(TUIHITGETOBJ(TUIHITMAKE(e_objOut,0)) == e_objOut);

    HTUI                hWnd;               // CM_HITTEST:in; CM_GESTUREHITTEST:out;
    HTUI                hHint;              // current hint widget. set to 0 means current widget.
    TMPointL            pt;                 // in, mouse logic coordinate based widget.
    TMUInt8             bNC;                // is non-client object;
    TMUInt8             bDomNode;           // pTarget is IDomNode*;
    union
    {
        TMUInt32        dwCode;             // hit test code, if bNC == true, the dwCode is EHitID.
        EHitID          hid;
    };
    TMUInt32            dwCode2;            // HITCODE_HEADER in listbox, tree.
    void*               pTaget;             // point to sub item, in tree view or list view.
    TMRectL             rcObj;              // logic rect of object.
};


#define CM_SUBCLASS             (CM_BASE + 0x100) // wp:pvoid, lp:HTUI subclass; 0:unsublass

#define CM_HITTEST              (CM_BASE + 0x101) // WP:current message; LP:TUIHITINFO*; return:no;

// WM_MOUSELEAVE: wp:TUIHITINFO* pold; lp:TUIHITINFO* pNew;
#define CM_MOUSEENTRY           (CM_BASE + 0x102) // wp:old TUIHITINFO*; lp:new TUIHITINFO*;
#define CM_NCMOUSEENTRY         (CM_BASE + 0x103) // wp:old TUIHITINFO*; lp:new TUIHITINFO*;
#define CM_NCMOUSELEAVE         (CM_BASE + 0x104) // wp:old TUIHITINFO*; lp:new TUIHITINFO*;
//#define CM_MOUSEDRAG            0x02A7 //after call StartDrag,system use WM_MOUSEDRAG instead of WM_MOUSEMOVE.
#define CM_MOUSESWITCH          (CM_BASE + 0x105) // wp:old TUIHITINFO*; lp:new TUIHITINFO*;

#define CM_NCSETCURSOR          (CM_BASE + 0x106)

// gesture & touch
#define GESTURE_FLAG_BEGIN      0x01
#define GESTURE_FLAG_INERTIA    0x02
#define GESTURE_FLAG_END        0x04

#define GESTURE_ID_ZOOM                             0x00
#define GESTURE_ID_PAN                              0x01 // move
#define GESTURE_ID_ROTATE                           0x02
#define GESTURE_ID_TWOFINGERTAP                     0x03
#define GESTURE_ID_PRESSANDTAP                      0x04
#define GESTURE_ID_COUNT                            0x05

// #define GESTURE_ID_PAN_SINGLE_FINGER_VERTICALLY     0x10
// #define GESTURE_ID_PAN_SINGLE_FINGER_HORIZONTALLY   0x11
// #define GESTURE_ID_PAN_GUTTER                       0x12
// #define GESTURE_ID_PAN_INERTIA                      0x13

// #define GESTURE_ZOOM           (0x01 << GESTURE_ID_ZOOM)
// #define GESTURE_MASK_PAN            (0x01 << GESTURE_ID_PAN) // move
// #define GESTURE_MASK_ROTATE         (0x01 << GESTURE_ID_ROTATE)
// #define GESTURE_MASK_TWOFINGERTAP   (0x01 << GESTURE_ID_TWOFINGERTAP)
// #define GESTURE_MASK_PRESSANDTAP    (0x01 << GESTURE_ID_PRESSANDTAP)

#define GESTURE_ZOOM                                (0x01 << GESTURE_ID_ZOOM                         )
#define GESTURE_PAN                                 (0x01 << GESTURE_ID_PAN                          )
#define GESTURE_ROTATE                              (0x01 << GESTURE_ID_ROTATE                       )
#define GESTURE_TWOFINGERTAP                        (0x01 << GESTURE_ID_TWOFINGERTAP                 )
#define GESTURE_PRESSANDTAP                         (0x01 << GESTURE_ID_PRESSANDTAP                  )
#define GESTURE_DETAIL_SHIFT                        16

#define GESTURE_PAN_SINGLE_FINGER_VERTICALLY        (GESTURE_PAN | (0x00000002 << GESTURE_DETAIL_SHIFT))
#define GESTURE_PAN_SINGLE_FINGER_HORIZONTALLY      (GESTURE_PAN | (0x00000004 << GESTURE_DETAIL_SHIFT))
#define GESTURE_PAN_GUTTER                          (GESTURE_PAN | (0x00000008 << GESTURE_DETAIL_SHIFT))
#define GESTURE_PAN_INERTIA                         (GESTURE_PAN | (0x00000010 << GESTURE_DETAIL_SHIFT))


// this is a bubble message
#define CM_GESTURENOTIFY        (CM_BASE + 0x10a) // wp: n/a; lp: TUIHITINFO*; return: mask of GESTURE_XXX
#define CM_GESTURE              (CM_BASE + 0x10b) // wp: GESTUREINFO*; lp:TUIHITINFO*;


///////////////////////////////////////////////////////////////////////////////
//  sample:
//  xml in module 1:     <L1000 color="0x00" />
//  xml in module cur:   <L1000 color="0x10" />
//  string table in module 1:   1000 "aaa" 
//  string table in module cur: 1000 "aa1" 
//
//  hModule  +   nIDStr  +    str   ->      text
//  0            0            "bbb"         "bbb"
//  1            1000         0             "aaa"
//  0            1000         0             "aa1"
//  1            1000         "color"       "0x00"
//  0            1000         "color"       "0x10"
//  other:                                  false
struct TUISETTEXT
{
    enum { eDefault, eHTML, eText };
    int             bHtml;
    TMHModule       hModule;
    int             nIDStr;
    const CStrPtr*  str;                          // nIDStr == 0: str; other: strXmlProperty in hModule, sample: settext(hModule, 1000, "color") = "0x00"; <L1000 color="0x00" />
};

#define CM_SETTEXT              (CM_BASE + 0x110) // WP: n/a; LP:TUISETTEXT*
#define CM_SETTEXTFORMAT        (CM_BASE + 0x111) // wp: new format;
#define CM_SETACCTEXT           (CM_BASE + 0x112) // WP: n/a; LP:TUISETTEXT*
#define CM_SETHINT              (CM_BASE + 0x113) // WP: n/a; LP:TUISETTEXT*
#define CM_ACTIVEHINT           (CM_BASE + 0x114) // WP: n/a; LP:bool
#define CM_HINTONADDTOOL        (CM_BASE + 0x115) // WP: HTUI hTool; LP:TOOLINFO*;  call when custom hint add tool
#define CM_HINTONSHOW           (CM_BASE + 0x116) // WP: HTUI hTool; LP:WINDOWPOS*; call when custom hint before show
#define CM_SETITEMHINT          (CM_BASE + 0x117) // WP: index; LP:TUISETTEXT*
#define CM_GETITEMHINT          (CM_BASE + 0x118) // WP: index; LP:CStrPtr*
#define CM_SETITEMACCTEXT       (CM_BASE + 0x119) // wp: index; lp:TUISETTEXT*
#define CM_GETITEMACCTEXT       (CM_BASE + 0x11a) // wp: index; lp:CStrPtr*


#define CM_SETBKPIC             (CM_BASE + 0x120) // WP: n/a; LP:ITuiPic*
#define CM_SETBKPIC2            (CM_BASE + 0x121) // WP: pic id; lParam:hModule
#define CM_GETBKPIC             (CM_BASE + 0x122)
#define CM_SETFOREPIC           (CM_BASE + 0x123) // WP: pic id; LP:hModule or WP:0; LP:ITuiPic*
#define CM_GETFOREPIC           (CM_BASE + 0x124)
#define CM_SETOUCAPTIONBKPIC    (CM_BASE + 0x125)
#define CM_SETFRAMEPIC          (CM_BASE + 0x126) // wp: hid; lp:


#define CM_SETTEXTCOLOR         (CM_BASE + 0x130) // WP:EButState; LP:: COLOREF
#define CM_SETFRAME             (CM_BASE + 0x131) // WP: pic id; lParam:hModule; id can be eAppPropUseSystemFrame to set mgr frame.
#define CM_GETSTATE             (CM_BASE + 0x132) // wp:0; lp:0;  return: EButStates
#define CM_SETGROUPDFTTABSTOP   (CM_BASE + 0x133) // return: bool

// #define CM_ISBINDWINDOW         (CM_BASE + 0x133)


//      --------           -----      ----
//      |      |             |        nOrg
//    --|------|--  ---   nProject    ----
//    | |      | | nView     |
//    --|------|--  ---      |
//      --------           -----
struct TUISCROLLINFO
{
    GXDips nView;
    GXDips nProject;
    GXDips nOrg;
};

struct TUISCROLLBARINFO
{
    TUIRect rc;
    GXDips nButtonHeight;
    GXDips nThumbTop;
    GXDips nThumbBottom;
};

#define CM_GETSCROLLINFO        (CM_BASE + 0x140) // WP:H TUISCROLLINFO*; LP:V TUISCROLLINFO*;
#define CM_GETSCROLLBARINFO     (CM_BASE + 0x141) // wp:H TUISCROLLBARINFO*,  lp:V TUISCROLLBARINFO*

// WP:request; LP:(wp == SB_THUMBPOSITION) ? ThumbPosition : 0;
#define CM_HSCROLL              (CM_BASE + 0x142)
#define CM_VSCROLL              (CM_BASE + 0x143)
#define CM_SCROLLBY             (CM_BASE + 0x144) //wp:(in/out); GXDips* x; lp:(in/out); GXDips* y;  out:new scroll pos;
#define CM_SCROLLTO             (CM_BASE + 0x145) //wp:(in/out); GXDips* x; lp:(in/out); GXDips* y;  out:new scroll pos;
#define CM_SETSCROLLRANGE       (CM_BASE + 0x146) //wp:GXDips* w; lp:GXDips* w; return: is set range.

// switch languages , WP:TMCStr pLangfilename body; LP:local id
// language packet header sample:<widget version="1.2007.11.22" localid="2052">
// this message send to all HWND, HTUI
#define CM_LANGUAGECHANGED      (CM_BASE + 0x150)
#define CM_GETLOCALE            (CM_BASE + 0x151)

#define CM_COLORVISUAL          (CM_BASE + 0x153) // wp: changed EColorVisual, lp:n/a
#define CM_USERINTERFACE        CM_COLORVISUAL
#define CM_GETCOLORVISUAL       (CM_BASE + 0x154) // return: mask of color visual

// set dpi logic:
// windows receive WM_DPICHANGED
// manager dispatch CM_ZOOMED
// border cast CM_SETDPI on CM_ZOOMED
// all widget set dpi and if old != new then send CM_ZOOMED.
#define CM_SETDPI               (CM_BASE + 0x157) // wp: new dpi; return:old dpi
#define CM_ZOOMED               (CM_BASE + 0x156) // HTUI broadcast,including sub-controls
// #define CM_ZOOM                 CM_ZOOMED

#define CM_RENDERCHANGED        (CM_BASE + 0x158) // wp:n/a; lp:n/a; broadcast to all widget. swich render to GDI/D3D/GL


// all of the sub-control messages are transmitted through main widget
// the message will route to parent, stop on window.
// use can modify message and param in TUIMSG*.
// child set bHandled == true to stop route.
// these window can receive bubble:
//  1. manager
//  2. have style: WS_ADV_ROUTE;
//  3. bind widget;
//  4. parent of bind widget.
//  5. HWND
struct TUIMSG
{
    TMHWnd   hWin; // if TUIMSG is windows message;
    TMUInt   message;
    TMWParam wParam;
    TMLParam lParam;

    HTUI     hWnd;
    TMUInt   uObjectID; // object id, 0 means windows message.
    TMUInt   uID;
    TMLParam uTag;
};
#define CM_MSGROUTE             (CM_BASE + 0x160)   // WP:BOOL* bHandled; LP: TUIMSG*

// all system message
// system send message to window; window will convert the message to CM_APPMESSAGE, and process to CTuiMsgFilter.
// after window process the message, windows will send CM_APPMESSAGE to root widget.
#define CM_APPMESSAGE           (CM_BASE + 0x162)   // WP:hMananger(HTUI); LP:TUIMSG*;

// if focus widget don't process keyboard message:
//  WM_CHAR,WM_SYSCHAR,WM_KEYDOWN,WM_KEYUP,WM_DEADCHAR,
//  WM_SYSKEYDOWN,WM_SYSKEYUP,WM_SYSDEADCHAR,WM_KEYLAST,WM_CANCELMODE
// the focus widget will call parent's widget to CM_MSGBUBBLE.
// these widget can receive bubble:
//  1. root widget
//  2. have style: WS_ADV_ARROWGROUP;
//  3. bind widget;
//  4. parent of bind widget.
#define CM_MSGBUBBLE            (CM_BASE + 0x163)   // wp: n/a; lp: TUIMSG*

typedef void (*PFNPostCallback)(HTUI hWnd, TMLParam lp);
#define CM_POSTCALLBACK         (CM_BASE + 0x164)   // wp: PFNPostCallback; lp: use data

// drag drop
class CDragData;
class CDropData;

struct DROPEVENT
{
    CDropData* pData;
    TMUInt32 dwEffect;  // CDropData::EEff
    TMUInt32 dwKeyState;
    TMPointD ptScreen;
    TMPointL ptWindow; // mouse logic coordinate based window
};

#define DROPEVENT_KEY2EFF(pEvent) \
    (DROP_KEY2EFF((pEvent)->dwKeyState) & (pEvent)->dwEffect)

// WP:TUIHITINFO*; LP:DRAGEVENT*; return:dwEffect
#define CM_DRAGENTRY            (CM_BASE + 0x170)
#define CM_DRAGOVER             (CM_BASE + 0x171)
#define CM_DRAGDROP             (CM_BASE + 0x172)
#define CM_DRAGLEAVE            (CM_BASE + 0x173)     // WP:0; LP:0; return:0


// the message broadcast when parent widget show/hide
#define CM_SHOWWINDOW_P         (CM_BASE + 0x180)   // WP:bShow;
// the message broadcast when parent widget enable/disable
#define CM_ENABLE_P             (CM_BASE + 0x181)   // WP:bEnable;
// the message broadcast when self or parent call "SetParent"
#define CM_SETPARENT_P          (CM_BASE + 0x182)   // WP:HOldParent; LP:HSet;

#define CM_INVALIDATE           (CM_BASE + 0x185)   // WP:n/a; LP:n/a; let HTUI invalidate.


#define CM_SET_PADING           (CM_BASE + 0x191)     // wp:TMRectL*; lp:n/a;
#define CM_GET_PADING           (CM_BASE + 0x192)     // wp:n/a; lp:TMRectL*;
#define CM_SET_MARGIN           (CM_BASE + 0x193)     // wp:TMRectL*; lp:n/a;
#define CM_GET_MARGIN           (CM_BASE + 0x194)     // wp:n/a; lp:TMRectL*;

#define SWMMI_MINWIDTH          0x01
#define SWMMI_MAXWIDTH          0x02
#define SWMMI_MINHEIGHT         0x04
#define SWMMI_MAXHEIGHT         0x08
struct TUIMINMAXINFO
{
    int nMinWidth;
    int nMaxWidth;
    int nMinHeight;
    int nMaxHeight;
};
#define CM_SETMINMAXINFO        (CM_BASE + 0x195)     // wp: mask of SWMMI_XXX; lp:TUIMINMAXINFO*
#define CM_GETMINMAXINFO        (CM_BASE + 0x196)     // lp:TUIMINMAXINFO*

// used to SetWindowPos and WM_WINDOWPOSCHANGED
#define SWP_NOLAYOUT                    0x100000    // do not layout child window
#define SWP_NOLAYOUTSIBLING             0x200000    // do not layout Sibling window

#define HTUI_TOP                        ((TM::HTUI)0) // HWND_TOP
#define HTUI_BOTTOM                     ((TM::HTUI)1) // HWND_BOTTOM

struct TUIWINDOWPOS {
    HTUI pWnd;
    HTUI pWndInsertAfter;
    GXDips x;
    GXDips y;
    GXDips cx;
    GXDips cy;
    TMUInt flags;
};
#define CM_WINDOWPOSCHANGED     WM_WINDOWPOSCHANGED // wp:TUIWINDOWPOS*; lp:TUIWINDOWPOS*

// close menu pop logic:
//  1. send WM_CANCELMODE to widget;
//  2. widget send WM_CANCELMODE to next pop's owner.
//  3. the next pop's owner send WM_CANCELMODE to next pop. and goto 2.
//  4. user can receive WM_CANCELMODE and block it.
//  5. the last pop's owner receive WM_CANCELMODE.
//  6. last pop's owner send WM_ENDMENUPOP to last pop.
//  7.   use can block WM_ENDMENUPOP to hide, show, destroy.
//  8.   if use not block, the last pop widget destroy self.
//  9. after send WM_ENDMENUPOP, remove last pop from pop list.
//  a. if close all pop, send root owner CM_UNINITMENUPOP.
//
// don't place dummy on pop window.
//
// lp:hPop;
#define CM_INITMENUPOP          (CM_BASE + 0x1a0) // the owner widget receive when pop.
#define CM_UNINITMENUPOP        (CM_BASE + 0x1a1) // pop owner widget receive when end pop.
#define CM_BEGINMENUPOP         (CM_BASE + 0x1a2) // the pop widget receive when pop.

// wp:IDOK or IDCANCEL; lp:TUIMSG*, the last message before end menupop
#define CM_ENDMENUPOP           (CM_BASE + 0x1a3) // the pop widget receive when end pop.

// the message only used in app::EndMenuPopLevel
#define CM_ENDMENUPOPSHOW       (CM_BASE + 0x1a5) // wp: true keep menupop widget show, false hide menupop widget.


#define CM_SETFONT              (CM_BASE + 0x1b0) // wp: const GXFont*; lp: redraw option, see MSDN(WM_SETFONT)
#define CM_GETFONT              (CM_BASE + 0x1b1) // wp: GXFont*; lp: is only get control font, if set false and if havn't font info will get parent font


// the progress of control send command && notify message:
// 1. if (IsEnableNotify()) self->SendMessage(CM_PRECOMMAND, id);
// 2. self::OnSelfCommand: GetPrent()->SendMessage(WM_COMMAND, id);
// 3: parent::OnCommand: GetPrent()->SendMessage(WM_COMMAND, id);
#define CM_PRECOMMAND           (CM_BASE + 0x1c0)

// instead WM_NOTIFY
struct NMBASE
{
    HTUI      hwndFrom;
    TMUInt    oidFrom;
    TMUInt    idFrom;
    TMUInt    code;
    TMLParam  lpItemData;
    TMLParam  lp1;
    TMLParam  lp2;
};
#define CM_PRENOTIFY            (CM_BASE + 0x1c1) // wp:id; lp:NMBASE*; send to self 
#define CM_NOTIFY               (CM_BASE + 0x1c2) // wp:id; lp:NMBASE*; send to parent->parent...


// WM_PRINT/WM_PRINTCLENT
// #define PRF_CHECKVISIBLE    0x00000001L
// #define PRF_NONCLIENT       0x00000002L
// #define PRF_CLIENT          0x00000004L
// #define PRF_ERASEBKGND      0x00000008L
// #define PRF_CHILDREN        0x00000010L
// #define PRF_OWNED           0x00000020L
const TMUInt32 PRF_BIND             = 0x00000100L; // render bind window
const TMUInt32 PRF_FLOATSCROLLBAR   = 0x00000200L;
const TMUInt32 PRF_FLOAT            = 0x00000400L; // render float control
#define CM_PRINTCLIENT          (CM_BASE + 0x1d0)  // WP:IGXApp*; LP:drawing options




// Microsoft Active Accessibility (MSAA)
#define ROLE_DEFAULT            0

#define CM_ACC_POST_KEYDOWN     (CM_BASE + 0x1e0)
#define CM_ACC_ALERT_EVENT      (CM_BASE + 0x1e1)
#define CM_ACC_GET_DESCRIPTION  (CM_BASE + 0x1e2)   // wp: childid; lp: std::wstring*; return:true/false


///////////////////////////////////////////////////////////////////////////////
// controls message

// list,tree

struct IColumnProxy
{
public:
    virtual void GetRenderText(TMLParam nTargetItemData, TMLParam nHeaderItemData, CText& text) = 0;
    virtual CString GetHInt(TMLParam nTargetItemData, TMLParam nHeaderItemData) = 0;
    virtual bool Sort(TMLParam nTargetItemData1, TMLParam nTargetItemData2, TMLParam nHeaderItemData, TMBool bSortUp) = 0;    // lpHeaderItem == -1: no sort; return: lpTarget1 < lpTarget2
    virtual void Reclaim(TMLParam nTargetItemData) {}
};

struct IColumn
{
    virtual int GetColumnCount() = 0;
    virtual TMLParam GetColumnData(int nCol) = 0;
    virtual GXDips GetColumnWidth(int nCol) = 0;
    virtual int GetColumnFormat(int nCol) = 0;        // mask of HDF_XXX;
};

// owner draw for listbox and tree
// WP:no; LP:TUIDRAWITEM*;
// most of the member reference DRAWITEMSTRUCT except for
//   1. dwDrawStage: refer to  NMCUSTOMDRAW::dwDrawStage
//   2. tmpData: user can write a self-managed data to this member in  CDDS_PREXXX and CDDS_ITEMPREXXX
//      and delete this data in CDDS_POSTXXX and CDDS_ITEMPOSTXXX itself
struct TUIDRAWITEM
{
    TMUInt              CtlID;
    TMUInt32            dwDrawStage;
    TMUInt              itemState;
    HTUI                hWnd;
    HTUI                hWndBind;
    union
    {
        TMUInt          itemID; // list box
        void*           hItem;  // tree
    };
    TUIRect             rcItem;     // in/out: user set left/right can change hscrollbar
    TMLParam            itemData;
    TMLong              tmpData;    // in/out
    GXPText*            gxText;     // in/out, if replace it, need old->release(); new->addref();
    IGXApp*             gxApp;
    IColumn*            pColumn;
    IColumnProxy*       proxy;
};

#define CM_DRAWITEM             (CM_BASE + 0x200)

// auto delete item data, for listbox and tree.
typedef void (CALLBACK *PFNDELETEPROC) (HTUI hwnd, TMLParam lIDorHITEM, TMLParam lData);
template <class T> struct CTuiProcDelete
{
    static void CALLBACK done(HTUI hwnd, TMLParam lIDorHITEM, TMLParam lData) { delete (T*)lData; }
};
template <class T> struct CTuiProcRelease
{
    static void CALLBACK done(HTUI hwnd, TMLParam lIDorHITEM, TMLParam lData) { ((T*)lData)->Relase(); }
};
#define CM_SETDELETEDATA        (CM_BASE + 0x201) // lp:PFNDELETEPROC

// backup or restore bind widget, for bind widget if need optimize memory
// if bBackUp and return false then stop optimize
// bind widget visible->invisible: if (!PFN_FREE_RESTORE(hParent, hBind, true, ...)) DestroyWindow(hBind);
// bind widget invisible->visible: CreateWindow(hBind); PFN_FREE_RESTORE(hParent, hBind, false, ...);
typedef TMBool (CALLBACK *PFNBINDBACKUP)(HTUI hWnd, HTUI hBind, TMBool bBackUp, TMLParam lIDorHITEM, TMLParam lData);
#define CM_SETPFNBINDBACKUP     (CM_BASE + 0x202) // lp:PFNBINDBACKUP


///////////////////////////////////////////////////////////////////////////////
// progress, up-down, track-bar, scroll, video, animate;
#define CM_GETPOS               (CM_BASE + 0x210) // return pos;
#define CM_SETPOS               (CM_BASE + 0x211) // wp:bRedraw; lp:pos;  return:old pos;
#define CM_DELTAPOS             (CM_BASE + 0x212) // wp:bRedraw; lp:dpos; return:old pos;
#define CM_GETRANGE             (CM_BASE + 0x213) // wp:int*; lp:int*;
#define CM_SETRANGE             (CM_BASE + 0x214) // wp:int*; lp:int*; return:bool;
#define CM_GETSTEP              (CM_BASE + 0x215) //
#define CM_SETSTEP              (CM_BASE + 0x216) // wp:step; return:old step
#define CM_STEPIT               (CM_BASE + 0x217) // wp:bRedraw; return:old pos;

// progress, up-down, track-bar, arrow, tab-button;
#define CM_GETBUDDY             (CM_BASE + 0x218) //
#define CM_SETBUDDY             (CM_BASE + 0x219) // wp:HTUI, return:bool

// progress, up-down, track-bar;
#define CM_GETBUDDYID           (CM_BASE + 0x21a) //
#define CM_SETBUDDYID           (CM_BASE + 0x21b) // wp:id
#define CM_SETFORMAT            (CM_BASE + 0x21c) // wp:n/a; LP:TUISETTEXT*
#define CM_POSCHANGING          (CM_BASE + 0x21d) // wp:old pos; lp:int* new pos; user can modify new pos; only send when user input.

// track-bar
#define TBM_SETLEFTPIC          (CM_BASE + 0x220) // wp: pic id; LP:hModule or WP:0; LP:ITuiPic*
#define TBM_SETRIGHTPIC         (CM_BASE + 0x221) // wp: pic id; LP:hModule or WP:0; LP:ITuiPic*
#define TBM_SETTRCKBTNPIC       (CM_BASE + 0x222) // wp: pic id; LP:hModule or WP:0; LP:ITuiPic*

// dummy, float, activex, web-view
#define CM_GETWINDOW            (CM_BASE + 0x225) //

// dummy
#define CDM_SETWINDOW           (CM_BASE + 0x226) // wp:TMHWnd
#define CDM_GETDUMMYINFO        (CM_BASE + 0x227)

// button, hotkey
#define MOD_APP                 0x0010  // app hotkey
#define MOD_SYS                 0x0020  // global hotkey
#define MOD_DISABLE             0x0040  // disable
#define MOD_THROUGH             0x0080  // don't eat key message
// #define MOD_ALT                         0x0001
// #define MOD_CONTROL                     0x0002
// #define MOD_SHIFT                       0x0004
// #define MOD_WIN                         0x0008
// #define MOD_LEFT                        0x8000
// #define MOD_RIGHT                       0x4000
// #define MOD_ON_KEYUP                    0x0800
// #define MOD_IGNORE_ALL_MODIFIER         0x0400
struct TUIHOTKEY
{
    TMUInt16    modifiers;      // MOD_XXX
    TMUInt16    vkey;           // virtual-key
    TMUInt      id;             // only for hotkey
};
#define CM_SETHOTKEY            (CM_BASE + 0x231) // wp:index(only for CTuiHotKey); lp:const TUIHOTKEY* or const TUIHOTKEY[10]*(index == -1);
#define CM_GETHOTKEY            (CM_BASE + 0x232) // wp:index(only for CTuiHotKey); lp:TUIHOTKEY* or TUIHOTKEY[10]*(index == -1);

// button, static
#define CBM_GETCHECK            0x00F0   //BM_GETCHECK,
#define CBM_SETCHECK            0x00F1   //BM_SETCHECK,
#define CBM_STATECHANGED        (CM_BASE + 0x23a) // wp: old check; lp:new check;

// button
#define CBM_CLICK               0x00F5   //BM_CLICK,
#define CBM_SETFOREPICWIDTH     (CM_BASE + 0x23b) // wp: width; lp:height;
#define CBM_GETBUTTONSTATE      (CM_BASE + 0x23c)
#define CBM_GETBUTTONSTATECOUNT (CM_BASE + 0x23d)
#define CBM_GETALIGNMODE        (CM_BASE + 0x23e)
#define CBM_SETALIGNMODE        (CM_BASE + 0x23f)
#define CBM_SETFOREPIC          CM_SETFOREPIC
#define CBM_GETFOREPIC          CM_GETFOREPIC

// radio,tabbutton
#define CBM_GETCHECKEDBUTTON    (CM_BASE + 0x240)
#define CBM_GETNEXTBUTTON       (CM_BASE + 0x241)
#define CBM_GETPREVBUTTON       (CM_BASE + 0x242)

// combo
#define CCB_GETBUTTON           (CM_BASE + 0x250)
#define CCB_GETEDIT             (CM_BASE + 0x251)
#define CCB_GETPOPDLG           (CM_BASE + 0x252)
#define CCB_GETPOPLIST          (CM_BASE + 0x253)
#define CCB_FILTER              (CM_BASE + 0x254) // send self when user input char to edit, if have CBS_FILTER style. wp:null, lp:null
#define CCB_ISDROPDOWN          (CM_BASE + 0x255)
#define CCB_SUBMIT              (CM_BASE + 0x256) // if pop then submit, else fail.
#define CCB_SETLISTITEMS        (CM_BASE + 0x257) // lp:LPCTSTR
#define CCB_SETREFERID          (CM_BASE + 0x258) // lp:new refer id
#define CCB_SHOWDROPDOWN        CB_SHOWDROPDOWN
#define CCB_SETEDITSEL          CB_SETEDITSEL

// edit,memo,rich
#define CEM_GETSEL              0x00B0  //EM_GETSEL
#define CEM_SETSEL              0x00B1  //EM_SETSEL
#define CEM_GETMODIFY           0x00B8  //EM_GETMODIFY
#define CEM_SETMODIFY           0x00B9  //EM_SETMODIFY
#define CEM_GETLINECOUNT        0x00BA  //EM_GETLINECOUNT
#define CEM_LINEINDEX           0x00BB  //EM_LINEINDEX
#define CEM_LINELENGTH          0x00C1  //EM_LINELENGTH
#define CEM_REPLACESEL          0x00C2  //EM_REPLACESEL
#define CEM_GETLINE             0x00C4  //EM_GETLINE
#define CEM_LIMITTEXT           0x00C5  //EM_LIMITTEXT
#define CEM_CANUNDO             0x00C6  //EM_CANUNDO
#define CEM_UNDO                0x00C7  //EM_UNDO
#define CEM_LINEFROMCHAR        0x00C9  //EM_LINEFROMCHAR
#define CEM_SETTABSTOPS         0x00CB  //EM_SETTABSTOPS
#define CEM_SETPASSWORDCHAR     0x00CC  //EM_SETPASSWORDCHAR
#define CEM_EMPTYUNDOBUFFER     0x00CD  //EM_EMPTYUNDOBUFFER
#define CEM_GETFIRSTVISIBLELINE 0x00CE  //EM_GETFIRSTVISIBLELINE
#define CEM_SETREADONLY         0x00CF  //EM_SETREADONLY
#define CEM_GETPASSWORDCHAR     0x00D2  //EM_GETPASSWORDCHAR
#define CEM_GETLIMITTEXT        0x00D5  //EM_GETLIMITTEXT
#define CEM_POSFROMCHAR         0x00D6  //EM_POSFROMCHAR
#define CEM_CHARFROMPOS         0x00D7  //EM_CHARFROMPOS
#define CEM_SETIMESTATUS        0x00D8  //EM_SETIMESTATUS
#define CEM_GETIMESTATUS        0x00D9  //EM_GETIMESTATUS
#define CEM_REPLACESELHTML      (CM_BASE + 0x270)
#define CEM_GETSELHTML          (CM_BASE + 0x271)
#define CEM_SETPROMPT           (CM_BASE + 0x272) // wp: String id; LP:va list    format text per id and valist
#define CEM_GETCHARINFO         (CM_BASE + 0x273) // wp: int nIndex, lp:RICHCURSOR*;
#define CEM_GETCHARUNIT         (CM_BASE + 0x274) // wp: ETextUnit unit, lp:TMSelRange*;

// activeX Notification Codes & Messages
#define XN_INIT                 10                  // call after object create, before init, the flash OCX need the command

#define CAX_BASE                (CM_BASE + 0x280)
#define CAX_END                 (CM_BASE + 0x289)
#define CAX_GETHOST             (CM_BASE + 0x280)
#define CAX_GETCONTROL          (CM_BASE + 0x281)
#define CAX_SETEXT              (CM_BASE + 0x282) // wp:extend IUnknown interface
#define CAX_BINDOBJECT          (CM_BASE + 0x283) // bind a existed object; wp:IInknown*; return:HRESULT
#define CAX_ONOBJINIT           (CM_BASE + 0x284) // wp:n/a; lp:n/a; call after object init.

// webview
#define CWV_BASE                (CM_BASE + 0x280)
#define CWV_END                 (CM_BASE + 0x2bf)
#define CWV_GETCONTROL          CAX_GETCONTROL
#define CWV_SETEXT              CAX_SETEXT
#define CWV_ONOBJINIT           CAX_ONOBJINIT   // wp:n/a; lp:n/a; call after object create.

#define CWV_SETZOOM             (CM_BASE + 0x290) // lp:zoom(const double*);
#define CWV_GETZOOM             (CM_BASE + 0x291) // lp:zoom(double*);
#define CWV_GETSOUCE            (CM_BASE + 0x292) // lp:CStrPtr*
#define CWV_NAVIGATE            (CM_BASE + 0x293) // wp:url; lp:head
#define CWV_NAVIGATETOSTRING    (CM_BASE + 0x294) // lp:html
#define CWV_ADDDOCSCRIPT        (CM_BASE + 0x295) // lp:script text
#define CWV_EXECSCRIPT          (CM_BASE + 0x296) // lp:script text
#define CWV_RELOAD              (CM_BASE + 0x297) // lp:bReloadFromServer
#define CWV_GETPID              (CM_BASE + 0x298)
#define CWV_CANGOBACK           (CM_BASE + 0x299)
#define CWV_CANGOFORWARD        (CM_BASE + 0x29a)
#define CWV_GOBACK              (CM_BASE + 0x29b)
#define CWV_GOFORWARD           (CM_BASE + 0x29c)
#define CWV_STOP                (CM_BASE + 0x29d)
#define CWV_GETTITLE            (CM_BASE + 0x29e) // lp:CStrPtr*

// extend mode notify for IE and chrome
#define CWV_ONWEBMESSAGE        (CM_BASE + 0x2a0) // lp:LPCWSTR;
#define CWV_ONNAVIGATESTART     (CM_BASE + 0x2a1) // lp:LPCWSTR url; {return:is cancel load; default:false};
#define CWV_ONTITLECHANGE       (CM_BASE + 0x2a2) // lp:LPCWSTR Text;
#define CWV_ONCOMMANDSTATE      (CM_BASE + 0x2a3) // wp:Long command(CSC_NAVIGATEBACK,...); lp:bool bEnable;
#define CWV_ONNAVIGATECOMPLETE  (CM_BASE + 0x2a4) //
#define CWV_ONNAVIGATEERROR     (CM_BASE + 0x2a5) // lp:error code; maybe 404,...;
#define CWV_ONFULLSCREEN        (CM_BASE + 0x2a6) // lp:bool bFullScreen;
#define CWV_ONNEWWINDOW         (CM_BASE + 0x2a7) // wp:LPCWSTR url; lp:HTUI*; { return:Is Cancel the navigation; default: false; }
#define CWV_ONWINDOWCLOSING     (CM_BASE + 0x2a8) // { return:Is close; default: false; }

// extend mode notify for chrome
struct BROWSERFOLDERS
{
    TMCharW szFolder[2][2048]; // L"" means default folder
};
#define CWV_ONCRGETFOLDER       (CM_BASE + 0x2b0) // wp:BROWSERFOLDERS* executable, lp:BROWSERFOLDERS* userData;
#define CWV_ONPROCESSFAILED     (CM_BASE + 0x2b1) // lp:COREWEBVIEW2_PROCESS_FAILED_KIND;

// extend mode notify for IE
#define CWV_ONIEEXEC            (CM_BASE + 0x2b5) // wp:CmdID(OLECMDID_SHOWSCRIPTERROR, ...); lp:UInknown* pDoc; { return:Is stop page; default:false; }
#define CWV_ONIEDLCONTROL       (CM_BASE + 0x2b6) // lp:long*; default: DLCTL_DLIMAGES | DLCTL_VIDEOS | DLCTL_BGSOUNDS
#define CWV_ONIEDOCCOMPLETE     (CM_BASE + 0x2b7) // lp:IDispatch* doc;
#define CWV_ONIEDOWNBEGIN       (CM_BASE + 0x2b8) //
#define CWV_ONIEDOWNCOMPLETE    (CM_BASE + 0x2b9) //
#define CWV_ONIEFILEDOWNLOAD    (CM_BASE + 0x2ba) // return: is Cancel the download process; default:false;
#define CWV_ONIESTATUSBAR       (CM_BASE + 0x2bb) // lp:bool bShow;
#define CWV_ONIETHEATERMODE     (CM_BASE + 0x2bc) // lp:bool bTheaterMode;
#define CWV_ONIEPROGRESSCHANGE  (CM_BASE + 0x2bd) // wp:long Progress; lp:long ProgressMax;
#define CWV_ONIEPROPCHANGE      (CM_BASE + 0x2be) // lp:BSTR szProperty;
#define CWV_ONIESTATUETEXT      (CM_BASE + 0x2bf) // lp:BOOL bShow;

// mgr, float
// LP: 0-255 alpha
#define CM_SETALPHA             (CM_BASE + 0x2e0)
#define CM_SETINTERVAL          (CM_BASE + 0x2e1)
#define CM_GET_BK_TEX           (CM_BASE + 0x2e2) // return GXPTexture

// user need process WM_DWMCOMPOSITIONCHANGED for system setting changed.
enum EThemeType
{
    eThemeTypeSystem,           // use system frame
    eThemeTypeXP,               // child window or no shadow window.
    eThemeTypeLayered,          // not support SetThemeType
    eThemeTypeDWM,              // have a shadow after win7.
    eThemeTypeDWMBlur,          // not support SetThemeType
    eThemeTypeDWMTransparent,   // like layered window
    eThemeTypeAuto,
};
#define CM_GET_THEME_TYPE       (CM_BASE + 0x2e3) // return: EThemeType

// mgr
#define CM_SET_THEME_TYPE       (CM_BASE + 0x2e4) // return: true/false; wp:EThemeType
#define CM_GET_MENUBAR          (CM_BASE + 0x2e5) // return:HTUI
#define CM_GET_WIDGET           (CM_BASE + 0x2e6) // send to HWND; Get HTUI.
#define CM_SETDEFAULTITEM       (CM_BASE + 0x2e7) // lp:int id or oid.
#define CM_SETDEFAULTFOCUS      (CM_BASE + 0x2e8) // lp:HTUI. if HWND activate, the "default focus" will auto focus.

// menu bar
#define MB_GETMENU              (CM_BASE + 0x2f0)
#define MB_SETMENU              (CM_BASE + 0x2f1)
#define MB_SETHMENU             (CM_BASE + 0x2f2)
#define MB_GETCURSEL            0x0188            // LB_GETCURSEL
#define MB_GETMENUINFO          (CM_BASE + 0x2f3) // wp:id; lp:MENUINFO*; id==0 get full info; id==1 get sub item 0, ...
#define MB_GETITEMCOUNT         (CM_BASE + 0x2f4)
#define MB_SETALTMODE           (CM_BASE + 0x2f5) // wp:bool bAlt;

// animate
#define CACM_PLAY               (CM_BASE + 0x300)
#define CACM_STOP               (CM_BASE + 0x301)
#define CACM_SEEK               (CM_BASE + 0x302)
#define CACM_SEEKCUR            (CM_BASE + 0x303)
#define CACM_GETPOS             (CM_BASE + 0x304)
#define CACM_GETRANGE           (CM_BASE + 0x305)
#define CACM_SETRANGE           (CM_BASE + 0x306)
#define CACM_SETREPEAT          (CM_BASE + 0x307)
#define CACM_GETREPEAT          (CM_BASE + 0x308)

// video
//#define VM_GETRENDER            (CM_BASE + 0x310)
#define VM_SEEK                 (CM_BASE + 0x311)
#define VM_SEEKCUR              (CM_BASE + 0x312)
#define VM_GETPOS               (CM_BASE + 0x313)
#define VM_GETRANGE             (CM_BASE + 0x314)
#define VM_SETRANGE             (CM_BASE + 0x315)
#define VM_SETREPEAT            (CM_BASE + 0x316)
#define VM_GETREPEAT            (CM_BASE + 0x317)
#define VM_PLAY                 (CM_BASE + 0x320)// ACM_PLAY
#define VM_PAUSE                (CM_BASE + 0x321)// ACM_STOP
#define VM_SETSRC               (CM_BASE + 0x322)// lp: file name
#define VM_SETSRCM              (CM_BASE + 0x323)// lp: IMoniker*
#define VM_LOAD                 (CM_BASE + 0x324)
#define VM_SETTIME              (CM_BASE + 0x325)// lp: time
#define VM_GETTIME              (CM_BASE + 0x326)
#define VM_GETDURATION          (CM_BASE + 0x327)
#define VM_GETENDED             (CM_BASE + 0x328)
#define VM_SETVOLUME            (CM_BASE + 0x329)
#define VM_GETVOLUME            (CM_BASE + 0x32a)
#define VM_SETMUTE              (CM_BASE + 0x32b)
#define VM_GETMUTE              (CM_BASE + 0x32c)
#define VM_GETSRC               (CM_BASE + 0x32d)  // lp: CStrPtr*
#define VM_GETVIDEOSIZE         (CM_BASE + 0x32e)  // wp: iscalc VS_X2Y;  lp: TMSize*


///////////////////////////////////////////////////////////////////////////////
// private message. don't define self message in this range. don't use this message.
#define CM_PRI_BASE             (CM_BASE + 0x350)
#define CM_PRI_END              (CM_BASE + 0x3ff)

#define CM_PRI_DIRTY            (CM_PRI_BASE + 2)   // WP:n/a; LP:n/a
#define CM_PRI_POSCHANGING      (CM_PRI_BASE + 3)   // WP:n/a; LP:TUIWINDOWPOS*; WND: WP:CTuiRect*; LP:WINDOWPOS*;

#define CM_USER                 (CM_BASE + 0x400)



////////////////////////////////////////////////////////////////////////////////
// ITuiPic
struct WPRENDERFX
{
    EButStates      nOldState;      // old state for animate
    EButStates      nNewState;      // new state for animate
    int             nMixAlpha;      // mix alpha for old state
};

struct SCROLLSTATE
{
    EHitObj         obj;            // e_objHScroll or e_objVScroll;
    GXDips          nButtonHeight;
    GXDips          nThumbTop;
    GXDips          nThumbBottom;
    EButStates      eState[5];      // current state

    static EButState SS2BS(EButStates state)
    {
        EButState s = ButStates2ButFrame(state);
        return (s == e_bsHigh) ? e_bsNormal : (s == e_bsFocus ? e_bsHigh : s);
    }
};

struct WPRENDERFX_SB
{
    EButStates      eState[5];      // old state for animate
    int             nAlpha[5];      // mix alpha for animate
};


TM_DECLARE_UUID_NS(TM, ITuiPic, 7B2DB675, 3B6E, 49d4, 93, F7, 81, 26, 9D, 81, 1D, 00);
struct ITuiPic : public IUnknown
{
public:
    virtual TMUInt32 GetObjectID () const = 0;
    virtual void Render(int nState, TMHDC unused, const CTuiRect& rc, WPRENDERFX* fx = 0, IGXApp* gapp = gxGetApp()) = 0;
    virtual void RenderSB(const SCROLLSTATE& state, const CTuiRect& rc, WPRENDERFX_SB* fx = 0, IGXApp* gapp = gxGetApp()) {}
    virtual TMUInt GetFrames() { return 1; }
    virtual int GetDelay() { return SHRT_MAX; }
    virtual TMUInt GetCurrentFrame() { return 0; }
    virtual void GetSize(TMSize& sz) {}
    virtual void SetCurFrame(TMUInt n) {}
    virtual void SetAlpha(TMUInt8 nAlpha) {}
    virtual CComPtr<ITuiPic> GetSubPic(int dpi) { return this; }

    virtual void SetColorTransform(IColorTransform* ctf) {} // the ctf not care property "ColorTransform".

    void Render(int nState, EButStates nStates, const CTuiRect& rc, IGXApp* gapp)
    {
        WPRENDERFX fx = { nStates, nStates, 255 };
        Render(nState, 0, rc, &fx, gapp);
    }
};


////////////////////////////////////////////////////////////////////////////////
// CTuiMsgFilter
class CTuiMsgFilter
{
public:
    typedef TMBool (*PFNFilterProc) (void* pVoid, TUIMSG* pMsg, TMResult& lResult);

    // CM_APPMESSAGE to receive windows message
    // return true to stop dispatch.
    // use can modify message and param in pMsg.
    virtual TMBool PreTranslateMessage(TUIMSG* pMsg, TMResult& lResult)
    {
        return PreTranslateMessage(pMsg->hWnd, pMsg->message, pMsg->wParam, pMsg->lParam, lResult);
    }

    // unused function
    virtual TMBool PreTranslateMessage(HTUI hWnd, TMUInt msg, TMWParam wParam, TMLParam lParam, TMResult& lResult) { return false; }
};



////////////////////////////////////////////////////////////////////////////////
// core functions

// all of function have no protect, so don't call function direct.
TM_BGN_PRIVATE_NAMESPACE

// debug function
TM_CAPI TMBool tuiDebugIsWindowValid(HTUI hWnd, TMBool bSameThread);

// widget function
TM_CAPI CTuiApp* tuiGetAPP(HTUI hWnd);
TM_CAPI HTUI tuiCreateByObjectID(HTUI* unused, HTUI hWndParent, TMHModule hRes, TMUInt uObjID, TUIFACTORY* pFactory, void* param);
TM_CAPI TMBool tuiDestroyWindow(HTUI hWnd);
TM_CAPI TMBool tuiSubclassWindow(HTUI hWnd, const CWidgetMsgDelegate& func);
TM_CAPI TMBool tuiUnsubclassWindow(HTUI hWnd, const CWidgetMsgDelegate& func);
TM_CAPI TMResult tuiDefSubclassProc(HTUI hWnd, TMUInt uMsg, TMWParam wp, TMLParam lp);
TM_CAPI TMResult tuiDefRouteProc(TUIMSG* pMsg);

TM_CAPI TMHWnd tuiGetHWND(HTUI hWnd);
TM_CAPI TMHModule tuiGetModule(HTUI hWnd);
TM_CAPI TMUInt32 tuiGetStyle(HTUI hWnd);
TM_CAPI TMUInt32 tuiGetExStyle(HTUI hWnd);
TM_CAPI TMUInt32 tuiSetStyle(HTUI hWnd, TMUInt32 dwStyle);
TM_CAPI TMUInt32 tuiSetExStyle(HTUI hWnd, TMUInt32 dwExStyle);
TM_CAPI TMUInt32 tuiGetLayOut(HTUI hWnd);
TM_CAPI void tuiSetLayOut(HTUI hWnd, TMUInt32 dwLayout);
TM_CAPI TMLParam tuiSetTag(HTUI hWnd, TMLParam dwTag);
TM_CAPI TMLParam tuiGetTag(HTUI hWnd);
TM_CAPI TMUInt32 tuiGetRole(HTUI hWnd);
TM_CAPI void tuiSetRole(HTUI hWnd, TMUInt32 dwRole);
TM_CAPI EBindState tuiGetBindState(HTUI hWnd, HTUI hParentStop);
TM_CAPI TMResult tuiSendMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam);
TM_CAPI TMBool tuiPostMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam) tm_thread_safe;
TM_CAPI TMResult tuiSendBroadcastMessage (HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool bSendSelf);
TM_CAPI void tuiGetWindowText(HTUI hWnd, CStrPtr& str);
TM_CAPI void tuiGetClassName(HTUI hWnd, CStrPtr& str);
TM_CAPI void tuiGetHint(HTUI hWnd, CStrPtr& str);
TM_CAPI void tuiGetAccText(HTUI hWnd, CStrPtr& str);
TM_CAPI TMBool tuiSetWindowPos(HTUI hWnd, HTUI hWndInsertAfter, GXDips x, GXDips y, GXDips cx, GXDips cy, TMUInt nFlags);
TM_CAPI TMBool tuiDoMenuPop(HTUI hWnd, HTUI hOwner, HTUI hNewFocus);
TM_CAPI HTUI tuiGetMenuPop(HTUI hWnd);
TM_CAPI TMBool tuiSortChild(HTUI hWnd, PFNWWCOMPARE pfn, TMLParam lParam, int nLevel, TMBool bReversed);
TM_CAPI TMBool tuiChangeFrame(HTUI hWnd, TMBool bUpdate);
TM_CAPI void tuiGetWindowRect(HTUI hWnd, TUIRect* lpRect);
TM_CAPI void tuiGetClientRect(HTUI hWnd, TUIRect* lpRect);
TM_CAPI void tuiGetPaddingRect(HTUI hWnd, TUIRect* lpRect);
TM_CAPI void tuiGetProjectRect(HTUI hWnd, TUIRect* lpRect);
TM_CAPI TMBool tuiUpdateWindow(HTUI hWnd);
TM_CAPI TMBool tuiInvalidateRect(HTUI hWnd, const TUIRect* lpRect, TMBool bErase);
TM_CAPI TMBool tuiEnableWindow(HTUI hWnd, TMBool bEnable);
TM_CAPI TMBool tuiEnableNotify(HTUI hWnd, TMBool bEnable);
TM_CAPI TMBool tuiIsEnableNotify(HTUI hWnd);
TM_CAPI HTUI tuiChildWindowFromPoint(HTUI hWnd, TMPointL point, TMUInt uFlags);
TM_CAPI HTUI tuiGetWindow(HTUI hWnd, TMUInt nCmd);
TM_CAPI TMBool tuiIsChild(HTUI hWnd, HTUI hChild);
TM_CAPI HTUI tuiGetParent(HTUI hWnd);
TM_CAPI HTUI tuiSetParent(HTUI hWnd, HTUI hWndNewParent);
TM_CAPI int tuiGetDlgCtrlID(HTUI hWnd);
TM_CAPI int tuiSetDlgCtrlID(HTUI hWnd, int nID);
TM_CAPI int tuiGetDlgCtrlObjectID(HTUI hWnd);
TM_CAPI HTUI tuiGetDlgItem(HTUI hWnd, int nID);
TM_CAPI GXPTexture tuiMakeImage(HTUI hWnd, const TUIRect* prc, TMColor clrBk = TMRGBA(0,0,0,0), TMUInt uFlag = PRF_NONCLIENT | PRF_ERASEBKGND | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE | PRF_BIND | PRF_FLOAT);
TM_CAPI void tuiSetDragImage(HTUI hWnd, CDragData& obj, TMPointD ptOffset, const CTuiRect* prc, TMUInt8 alpha, TMBool bSrcAlpha, TMColor clrKey);
TM_CAPI TMUInt32 tuiDoDrag(HTUI hWnd, CDragData& obj, TM::EDragEff dwEff);
TM_CAPI TMBool tuiEnumAllChildWindows(HTUI hWnd, PFNWWENUMPROC lpEnumFunc, TMLParam lParam, TMBool bEnumSelf, size_t nLevel);
TM_CAPI void tuiClientToScreen(HTUI hWnd, const TMPointL* lpClient, int nPt, TMPointD* lpScreen);
TM_CAPI void tuiScreenToClient(HTUI hWnd, const TMPointD* lpScreen, int nPt, TMPointL* lpClient);
TM_CAPI int tuiGetDPI(HTUI hWnd);
TM_CAPI IColorTransform* tuiGetColorTransform(HTUI hWnd);
TM_CAPI void tuiGetRichContext(HTUI hWnd, IRichContext** ppCreator);
TM_CAPI TMBool tuiAnimateWindow(HTUI hWnd, TMUInt32 dwTime, TMUInt32 dwFlags, const CTuiRect* rcFrom, const CTuiRect* rcTo, IEase* pEase);
TM_CAPI HTUI tuiCaretGetInfo(TUIRect& rc);
TM_CAPI void tuiConvertToPic(HTUI hWnd, ITuiPic** ppic);

// for Accessible
TM_CAPI TMBool tuiIsValid(HTUI hWnd);


// dialog function
TM_CAPI int tuiDoDialogBox(HTUI hMgr, TMHWnd hOwner);
TM_CAPI TMBool tuiEndDialog(HTUI hMgr, int nCode);
TM_CAPI TMBool tuiSubclassMgr(HTUI hMgr, const CWindowMsgDelegate& func);
TM_CAPI bool tuiUnsubclassMgr(HTUI hMgr, const CWindowMsgDelegate& func);
TM_CAPI TMResult tuiDefSubclassMgrProc(HTUI hMgr, TMUInt uMsg, TMWParam wp, TMLParam lp);

TM_END_PRIVATE_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CMsgDelegateT
template <class T> struct CMsgDelegateT
{
    typedef T   value_type;

    typedef TMBool(*_PFNProc)(
        void* pVoid, value_type hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID);

    void*       pVoid;
    _PFNProc    pfn;

    CMsgDelegateT(_PFNProc proc)
        : pfn(proc)
        , pVoid(0)
    {
    }

    template <class V, class F>
    CMsgDelegateT(V* pvoid, F proc)
        : pfn((_PFNProc)proc)
        , pVoid(pvoid)
    {
        DbgAssert(pVoid);
        typedef TMBool(*_proc)(V*, value_type, TMUInt, TMWParam, TMLParam, TMResult&, TMULong);
        _proc x = proc;
        tm_unused(x);
    }

    bool operator == (const CMsgDelegateT& rhs) const { return rhs.pfn == pfn && rhs.pVoid == pVoid; }

    TUIFACTORY ToFactory() const
    {
        TUIFACTORY f = { _CreateByPFN, reinterpret_cast<void*>(pfn), pVoid };
        return f;
    }

private:
    static void _SubClass(HTUI hWgt, PFNWidgetProc proc, void* param)
    {
        PRI::tuiSubclassWindow(hWgt, CWidgetMsgDelegate(param, proc));
//        CTuiWidget(hWgt).SubclassWindow(CWidgetMsgDelegate(param, proc));
    }

    static void _SubClass(HTUI hWgt, PFNWindowProc proc, void* param)
    {
        PRI::tuiSubclassMgr(hWgt, CWindowMsgDelegate(param, proc));
        //CTuiWindowT<CTuiWidget>(hWgt).SubclassMgr(CWindowMsgDelegate(param, proc));
    }

    static void _CreateByPFN(HTUI hWgt, void* param1, void* param2)
    {
        if (param1)
        {
            _PFNProc pm = (_PFNProc)param1;
            _SubClass(hWgt, pm, param2);
        }
    }
};

TM_CAPI void tuiPlugInCreate(TMBool bSpyMode);
typedef void (*PFNTuiPlugInCreate)(TM::CTuiApp& app, TMBool bDoModal);


////////////////////////////////////////////////////////////////////////////////
// class CTuiApp
template <class T, class T_Base, class T_Map> struct CTuiWidgetBaseT;

class CTuiApp
{
public:
#ifdef TM_WIN_METRO
    static CTuiApp* Init(Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ panel);
#else
    static CTuiApp* Init(void*) { return 0; }
#endif

    // unload all XML; call Term() when end process.
    // if only have 1 resource, Term() same as UnLoadXml();
    virtual void Term() = 0;

    static CTuiApp& GetCurrentSession() { return *PRI::tuiGetAPP(0); }

    ///////////////////////////////////////////////////////////////////////////
    // init
    // QT: pBase must include full resource path name such as ":/widget/chat/tuires.xml"
    // MAKEINTRESOURCE, those 2 values could be file name, or resource ID, but do not be duplicated
    // a module only can unloadXml once.
    // this is error code: the res.dll call "UnLoadXml" twice.
    // a.dll : TMHModule h1 = LoadLibray("res.dll"); _tuiApp.LoadXml(h1, ..); ...; _tuiApp.UnLoadXml(h1);
    // b.dll : TMHModule h2 = LoadLibray("res.dll"); _tuiApp.LoadXml(h2, ..); ...; _tuiApp.UnLoadXml(h2);
    virtual TMBool LoadXml(TMHModule hRes, TMCStr pBase, TMCStr pLang = 0) = 0;
    virtual void UnLoadXml(TMHModule hRes) = 0;

    virtual void SetColorTransform(TMHModule hRes, IColorTransform* pct) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // property
    virtual TMBool SetProperty(EAppProperty idProp, const void* pVol, int nVolSize) = 0;
    virtual TMBool GetProperty(EAppProperty idProp, void* pVol, int nVolSize) = 0;

    template <class T> inline TMBool SetPropertyT(EAppProperty idProp, const T& _v) { return SetProperty(idProp, &_v, sizeof(T)); }
    template <class T> inline TMBool GetPropertyT(EAppProperty idProp, T& _v) { return GetProperty(idProp, &_v, sizeof(T)); }

    inline void SetAnimationButton(TMBool b)            { SetPropertyT(eAppPropAnimationButton,     b); }
    inline void SetAnimationScroll(TMBool b)            { SetPropertyT(eAppPropAnimationScroll,     b); }
    inline void SetAnimationSysBut(TMBool b)            { SetPropertyT(eAppPropAnimationSysBut,     b); }
    inline void SetHighContrast(TMBool b)               { SetPropertyT(eAppPropHighContrast,        b); }
    inline void SetUseSystemFrame(TMBool b)             { SetPropertyT(eAppPropUseSystemFrame,      b); }
    inline void SetWebview2SSOUsingOSA(TMBool b)        { SetPropertyT(eAppPropWebview2SSOUsingOSA, b); }
    inline void SetWebview2Arguments(TMCStr b)          { SetPropertyT(eAppPropWebview2Arguments,   b); }
    inline void SetWebview2Language(TMCStr b)           { SetPropertyT(eAppPropWebview2Language,    b); }
    inline void SetWebview2ExecutableFolder(TMCStr b)   { SetPropertyT(eAppPropWebview2ExecutableFolder,        b); }
    inline void SetWebview2UserDataFolder(TMCStr b)     { SetPropertyT(eAppPropWebview2UserDataFolder,          b); }
    inline void SetShowHintOnEllipsis(TMBool b)         { SetPropertyT(eAppPropShowHintOnEllipsis,  b); }
    inline void SetShowHintOnTabKey(TMBool b)           { SetPropertyT(eAppPropShowHintOnTabKey,    b); }

    inline void SetToolTipLayered(TMBool b)             { SetPropertyT(eAppPropHintLayered,         b); }
    inline void SetDisabledColorText(TMColor c)         { SetPropertyT(eAppPropDisabledColorText,   c); }
    inline void SetHintColorBk(TMColor c)               { SetPropertyT(eAppPropHintColorBk,         c); }
    inline void SetHintColorText(TMColor c)             { SetPropertyT(eAppPropHintColorText,       c); }
    inline void SetColorEditSelBackground(TMColor c)    { SetPropertyT(eAppPropEditColorBkSel,      c); }
    inline void SetColorEditSelText(TMColor c)          { SetPropertyT(eAppPropEditColorTextSel,    c); }
    inline void SetColorEditPrompt(TMColor c)           { SetPropertyT(eAppPropEditColorPrompt,     c); }

    inline TMBool IsAnimationButton()                   { TMBool b; GetPropertyT(eAppPropAnimationButton,       b); return b; }
    inline TMBool IsAnimationScroll()                   { TMBool b; GetPropertyT(eAppPropAnimationScroll,       b); return b; }
    inline TMBool IsAnimationSysBut()                   { TMBool b; GetPropertyT(eAppPropAnimationSysBut,       b); return b; }
    inline TMBool IsHighContrast()                      { TMBool b; GetPropertyT(eAppPropHighContrast,          b); return b; }
    inline TMBool IsUseSystemFrame()                    { TMBool b; GetPropertyT(eAppPropUseSystemFrame,        b); return b; }
    inline TMBool IsWebview2SSOUsingOSA()               { TMBool b; GetPropertyT(eAppPropWebview2SSOUsingOSA,   b); return b; }
    inline TMBool IsShowHintOnEllipsis()                { TMBool b; GetPropertyT(eAppPropShowHintOnEllipsis,    b); return b; }
    inline TMBool IsShowHintOnTabKey()                  { TMBool b; GetPropertyT(eAppPropShowHintOnTabKey,      b); return b; }

    inline TMBool  IsToolTipLayered()                   { TMBool b; GetPropertyT(eAppPropHintLayered,           b); return b; }
    inline TMColor GetDisabledColorText()               { TMColor c; GetPropertyT(eAppPropDisabledColorText,    c); return c; }
    inline TMColor GetHintColorBk()                     { TMColor c; GetPropertyT(eAppPropHintColorBk,          c); return c; }
    inline TMColor GetHintColorText()                   { TMColor c; GetPropertyT(eAppPropHintColorText,        c); return c; }
    inline TMColor GetColorEditSelBackground()          { TMColor c; GetPropertyT(eAppPropEditColorBkSel,       c); return c; }
    inline TMColor GetColorEditSelText()                { TMColor c; GetPropertyT(eAppPropEditColorTextSel,     c); return c; }
    inline TMColor GetColorEditPrompt()                 { TMColor c; GetPropertyT(eAppPropEditColorPrompt,      c); return c; }


    ///////////////////////////////////////////////////////////////////////////
    // resource function
    virtual CStrPtr LoadString(TMHModule hRes, int nID, const CStrView* strXmlProperty = 0) = 0; // strXmlProperty: TUI_PROPERTY_XXX
    virtual TMHMenu LoadMenu(TMHModule hRes, TMCStr pMenu) = 0;
    virtual TMResult GetResourceStream(TMHModule hRes, TMCStr lpszName, TMCStr uType, struct IStream** ppStream) = 0;
    virtual CComPtr<ITuiPic> GetPic(TMHModule hRes, int nIDObj, int dpi = TUI_DPI_SYSTEM_DEFAULT) = 0;
    // use self file to replace pic's source file.
    // sample:
    // CComPtr<IStream> ptrStream;
    // StreamCreateFromFile(lpszName, TM::CFile::e_faRead, ptrStream);
    // CComPtr<ITuiPic> pic(_tuiApp.CreateCustomPic(_ModuleATL.GetModuleInstance(), IDC_BMP_10023, ptrStream));
    virtual CComPtr<ITuiPic> CreateCustomPic(TMHModule hRes, int nIDObj, IStream* stream) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // register a factory function to subclass widget
    // when create a new widget,
    // the widget will create a new class to subclass self use nObjectID.
    virtual void RegisterCallbackFactory(TMHModule hRes, int nObjectID, const TUIFACTORY& factory) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // message filter
    // return: bHandle
    virtual TMBool AddMessageFilter(CTuiMsgFilter* pMessageFilter) = 0;
    virtual TMBool RemoveMessageFilter(CTuiMsgFilter* pMessageFilter) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // state
    virtual void GetMouseHotState(TUIHITINFO& ht) = 0;
    // get last button down state.
    // return: is the button down now.
    virtual TMBool GetMouseLDownState(TUIHITINFO& ht) = 0;
    virtual TMBool GetMouseRDownState(TUIHITINFO& ht) = 0;
    virtual TMBool GetMouseMDownState(TUIHITINFO& ht) = 0;
    virtual TMBool GetGestureState(TUIHITINFO& ht, int nID) = 0;
    virtual CKeyInput GetKeyInput() = 0;

    ///////////////////////////////////////////////////////////////////////////
    // other function
    virtual TMBool SetCapture(HTUI hWnd) = 0;
    virtual HTUI GetCapture() = 0;
//    virtual void ReleaseCapture() = 0;
    virtual TMBool SetFocus(HTUI hWnd, EFocusReason reason = eFocusReasonUnknown) = 0;
    virtual HTUI GetFocus() = 0;
    virtual void KillFocus() = 0;
//    virtual void SetFocusChain(HTUI hFirst, HTUI hSecond) = 0; // hSecond == 0: remove tab chain.

    ///////////////////////////////////////////////////////////////////////////
    // menu pop
    // hWnd not in pop: return -1; in first pop: return 0; ...
    // bOwner:
    //   true: hWnd is pop owner or hWnd is child of pop owner.
    //   false: hWnd is pop or hWnd is parent of pop.
    virtual int GetMenuPopLevel(HTUI hWnd, TMBool bOwner) = 0;
    virtual TMBool EndMenuPopLevel(int nLevel, int nResult, TMBool bRestoreFocus = true, TUIMSG* p = 0) = 0;
    virtual TMBool GetMenuPopInfo(int nLevel, HTUI* pOwner, HTUI* pPop) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // dialog
    virtual HTUI CreateDlgEx(TMHModule hInst, int nID, TMHWnd hWndParent, TUIFACTORY* pFactory, void* param = 0) = 0;
    virtual HTUI CreateDlgByHWND(TMHModule hInst, int nID, TMHWnd hWnd, TUIFACTORY* pFactory, void* param = 0) = 0;
    virtual void EnumDialogs(PFNWWENUMPROC lpEnumFunc, TMLParam lParam) = 0;

    HTUI CreateDlg(TMHModule hInst, int nID, TMHWnd hWndParent, int unused = 0, void* param = 0)
    {
        return CreateDlgEx(hInst, nID, hWndParent, 0, param);
    }

    int DlgBox(TMHModule hInst, int nID, TMHWnd hWndParent, int unused = 0, void* param = 0)
    {
        HTUI hWgt = CreateDlgEx(hInst, nID, hWndParent, 0, param);
        return hWgt ? PRI::tuiDoDialogBox(hWgt, hWndParent) : -1;
    }
};

#define _tuiApp (TM::CTuiApp::GetCurrentSession())




/**
 * this class same as ATL::CWindow, see ATL::CWindow in MSDN
 */
class CTuiWidget
{
public:
    HTUI m_hWnd;

public:
    void _Debug_WidgetMustValidAndSameThread() const
    {
#ifdef TM_DBG
        DbgAssert(m_hWnd);
        DbgAssert(PRI::tuiDebugIsWindowValid(m_hWnd, true));
#endif
    }

    void _Debug_WidgetMustBeMgr() const
    {
#ifdef TM_DBG
        if (!m_hWnd)
            return;
        CString str;
        GetClassName(str);
        DbgAssert(str == TUI_CLASS_WINDOW)("oid:%d", GetDlgCtrlObjectID());
#endif
    }

    void _Debug_WidgetMustBeNotMgr() const
    {
#ifdef TM_DBG
        if (!m_hWnd)
            return;
        CString str;
        GetClassName(str);
        DbgAssert(str != TUI_CLASS_WINDOW)("oid:%d", GetDlgCtrlObjectID());
#endif
    }

public:
    TMBool IsWindow() const
    {
        if (!m_hWnd)
            return false;
#ifdef TM_DBG
        return PRI::tuiDebugIsWindowValid(m_hWnd, false);
#else
        return true;
#endif
    }

    CTuiWidget (HTUI hWnd = NULL)
        : m_hWnd (hWnd)
    {
    }

    CTuiWidget& operator= (HTUI hWnd)
    {
        m_hWnd = hWnd;
        return *this;
    }

    static TMCStr GetWndClassName()
    {
        return NULL;
    }

    void Attach (HTUI hWndNew)
    {
        m_hWnd = hWndNew;
    }

    HTUI Detach()
    {
        HTUI hWnd = m_hWnd;
        m_hWnd = NULL;
        return hWnd;
    }

    CTuiApp* GetAPP()
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetAPP(m_hWnd);
    }

    /** this function will create a child widget use XML res template */
    // hRes: 0: use hWndParent's resource module.
    // the new widget is last child of parent
    HTUI Create(HTUI hWndParent, TMUInt uObjID, TMHModule hRes = 0, TUIFACTORY* pFactory = 0, void* param = 0)
    {
        DbgProtect(!hWndParent, 0);
        DbgAssert(!m_hWnd)("oid:%d", uObjID);
        DbgAssert(uObjID);
        CTuiWidget(hWndParent)._Debug_WidgetMustValidAndSameThread();
        m_hWnd = PRI::tuiCreateByObjectID(0, hWndParent, hRes, uObjID, pFactory, param);
        _Debug_WidgetMustBeNotMgr();
        return m_hWnd;
    }

    TMBool DestroyWindow()
    {
        DbgProtect(!m_hWnd, true);
        _Debug_WidgetMustValidAndSameThread();
        if (!PRI::tuiDestroyWindow(m_hWnd))
            return false;
        m_hWnd = NULL;
        return true;
    }

    // widget will auto unsubclass func when WM_NCDESTROY.
    TMBool SubclassWindow(const CWidgetMsgDelegate& func)
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
        DbgAssert(!UnsubclassWindow(func)); // don't subclass a window twice.
        return PRI::tuiSubclassWindow(m_hWnd, func);
    }

    TMBool UnsubclassWindow(const CWidgetMsgDelegate& func)
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiUnsubclassWindow(m_hWnd, func);
    }

    template <class T_this, class T_tratis>
    TMBool SubclassWindow(T_this* pthis, T_tratis)
    {
        CWidgetMsgDelegate dg(pthis, _CTuiWidgetProc<T_this, HTUI, T_tratis>::_WindowProc);
        return SubclassWindow(dg);
    }

    template <class T_this, class T_tratis>
    TMBool UnsubclassWindow(T_this* pthis, T_tratis)
    {
        CWidgetMsgDelegate dg(pthis, _CTuiWidgetProc<T_this, HTUI, T_tratis>::_WindowProc);
        return UnsubclassWindow(dg);
    }

    TMResult DefWindowProc(TMUInt uMsg, TMWParam wp, TMLParam lp)
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiDefSubclassProc(m_hWnd, uMsg, wp, lp);
    }

    static TMResult DefRouteProc(TUIMSG* pMsg)
    {
        DbgAssert(pMsg);
        CTuiWidget(pMsg->hWnd)._Debug_WidgetMustValidAndSameThread();
        return PRI::tuiDefRouteProc(pMsg);
    }

//     GXPTexture GetBkBuffer ();

    void CopyWindowImageEx(TMUInt uFlag)
    {
        SendMessage(WM_PRINT, 0, uFlag);
    }

    void CopyWindowImage(bool bCheckVisible, TMUInt ext = PRF_BIND | PRF_FLOAT)
    {
        TMUInt uFlag = PRF_NONCLIENT | PRF_ERASEBKGND | PRF_CLIENT | PRF_CHILDREN | ext;
        if (bCheckVisible)
            uFlag |= PRF_CHECKVISIBLE;
        CopyWindowImageEx(uFlag);
    }

// Attributes
    operator HTUI() const { return m_hWnd; }

    TMHWnd GetHWND () const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetHWND(m_hWnd);
    }

    TMHModule GetModule() const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetModule(m_hWnd);
    }

    TMUInt32 GetStyle() const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetStyle(m_hWnd);
    }

    TMUInt32 GetExStyle() const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetExStyle(m_hWnd);
    }

    TMUInt32 SetStyle(TMUInt32 dwStyle)
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiSetStyle(m_hWnd, dwStyle);
    }

    TMUInt32 SetExStyle(TMUInt32 dwExStyle)
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiSetExStyle(m_hWnd, dwExStyle);
    }

    TMUInt32 GetLayOut() const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetLayOut(m_hWnd);
    }

    void SetLayOut(TMUInt32 dwLayout)
    {
        _Debug_WidgetMustValidAndSameThread();
        DbgProtectVoid(!m_hWnd);
        PRI::tuiSetLayOut(m_hWnd, dwLayout);
    }

    TMLParam GetTag() const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetTag(m_hWnd);
    }

    TMLParam SetTag(TMLParam dwTag)
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiSetTag(m_hWnd, dwTag);
    }

    EBindState GetBindState(HTUI hParentStop = 0) const
    {
        DbgProtect(!m_hWnd, eBindStateNone);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetBindState(m_hWnd, hParentStop);
    }

    CTuiZoom GetDPI() const
    {
        DbgProtect(!m_hWnd, TM_DPI_SYSTEM_DEFAULT);
        DbgAssert(IsWindow());
        return PRI::tuiGetDPI(m_hWnd);
    }

    ATLocal GetLocale() const
    {
        return (ATLocal)this->SendMessage(CM_GETLOCALE, 0, 0L);
    }

    CColorTransformPtr GetColorTransform() const
    {
        DbgProtect(!m_hWnd, 0);
        DbgAssert(IsWindow());
        return PRI::tuiGetColorTransform(m_hWnd);
    }

    EColorVisual GetColorVisual() const
    {
        return (EColorVisual)this->SendMessage(CM_GETCOLORVISUAL, 0, 0);
    }


// do not meet object encapsulation principle delete it
//  TMLLong GetWindowLong(int nIndex) const;
//  TMLLong SetWindowLong(int nIndex, TMLLong dwNewLong);
//  WORD GetWindowWord(int nIndex) const
//  WORD SetWindowWord(int nIndex, WORD wNewWord)

// extension
    TMBool GetPrevHitInfo(TUIHITINFO& info)
    {
        DbgProtect(!m_hWnd, false);
        GetAPP()->GetMouseHotState(info);
        return info.hWnd == m_hWnd;
    }

    TMBool GetMouseDownHitInfo(TUIHITINFO& info)
    {
        DbgProtect(!m_hWnd, false);
        return GetAPP()->GetMouseLDownState(info) && (info.hWnd == m_hWnd);
    }

    // window: screen device rect;
    // widget: logic rect;
    // see MSDN:AnimateWindow and tmani.h:IEase
    TMBool AnimateWindow(TMUInt32 dwTime, TMUInt32 dwFlags, const TUIRect* rcFrom = 0, const TUIRect* rcTo = 0, IEase* pEase = 0)
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(IsWindow());
        return PRI::tuiAnimateWindow(m_hWnd, dwTime, dwFlags, static_cast<const CTuiRect*>(rcFrom), static_cast<const CTuiRect*>(rcTo), pEase);
    }

    TMBool StopAnimateWindow()
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(IsWindow());
        return PRI::tuiAnimateWindow(m_hWnd, 0, 0, 0, 0, 0);
    }

// Message Functions
    TMResult SendMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0) const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiSendMessage(m_hWnd, message, wParam, lParam);
    }

    TMBool PostMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0) const tm_thread_safe
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(IsWindow());
        return PRI::tuiPostMessage(m_hWnd, message, wParam, lParam);
    }

    TMBool PostMessageCallback(PFNPostCallback pfn, TMLParam lParam = 0)
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(IsWindow());
        return PRI::tuiPostMessage(m_hWnd, CM_POSTCALLBACK, (TMWParam)pfn, lParam);
    }

    TMResult SendBroadcastMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0, TMBool bSendSelf = true) const
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiSendBroadcastMessage(m_hWnd, message, wParam, lParam, bSendSelf);
    }

    TMResult SendNotifyMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(message == WM_COMMAND);
        return this->SendMessage(CM_PRECOMMAND, wParam, lParam);
    }

//     TMResult SendCommandMessage(TMUInt16 code);

    TMBool HitTest(TUIHITINFO& hi, TMUInt uMsg = 0)
    {
        DbgProtect(!m_hWnd, false);
        SendMessage(CM_HITTEST, uMsg, (TMLParam)&hi);
        return !(hi.bNC && (hi.dwCode == (TMUInt)-2));
    }

// windows prop function
    TMBool SetFrame(int nID, TMHModule hModule = 0)
    {
        return SendMessage(CM_SETFRAME, nID, (TMLParam)hModule);
    }

    TMBool IsBindWindow()
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(IsWindow());
        return eBindStateNone != PRI::tuiGetBindState(m_hWnd, 0);
    }

    TMBool SetBkPic(int nID, TMHModule hModule = 0)
    {
        return SendMessage(CM_SETBKPIC2, nID, (TMLParam)hModule);
    }

    TMBool SetBkPic(ITuiPic* pPic)
    {
        return SendMessage(CM_SETBKPIC, 0, (TMLParam)pPic);
    }

    ITuiPic* GetBkPic()
    {
        return (ITuiPic*)SendMessage(CM_GETBKPIC, 0, 0);
    }

    TMBool SetFrameCaptionBKPic(int nID, TMHModule hModule = 0)
    {
        return SendMessage(CM_SETOUCAPTIONBKPIC, nID, (TMLParam)hModule);
    }

    TMBool SetFramePic(EHitObj obj, ITuiPic* pic)
    {
        return SendMessage(CM_SETFRAMEPIC, (TMWParam)obj, (TMLParam)pic);
    }

    // the function will reset layout to (ANCHOR_NONE | ANCHOR_VNONE).
    CComPtr<ITuiPic> ConvertToPic()
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        CComPtr<ITuiPic> ptr;
        PRI::tuiConvertToPic(m_hWnd, &ptr);
        return ptr;
    }

    //System Menu  wId: SC_CLOSE, SC_MINIMIZE, SC_MAXIMIZE
//     TMBool SetFrameSysIconDisabled(TMUInt wID, TMBool isDisabled)
//     {
//         return SendMessage(CM_SETSYSICONDISABLED, TMWParam(wID), TMLParam(isDisabled));
//     }
// Window Text Functions

    TMBool _SendSetTextMessage(TMUInt uMsg, TMWParam wParam, int bHtml, const CStrPtr* str, int nid = 0, TMHModule hModule = 0)
    {
        TUISETTEXT cst = { bHtml, hModule, nid, str };
        return this->SendMessage(uMsg, wParam, (TMLParam)&cst);
    }

    TMBool _SendSetTextMessage(TMUInt uMsg, TMWParam wParam, int bHtml, TMHModule hModule, int nid)
    {
        TUISETTEXT cst = { bHtml, hModule, nid, 0 };
        return this->SendMessage(uMsg, wParam, (TMLParam)&cst);
    }

    TMBool SetWindowText(const CStrPtr& strOrKey, int bHtml = TUISETTEXT::eDefault, int idRes = 0, TMHModule hModule = 0)
    {
        return this->_SendSetTextMessage(CM_SETTEXT, 0, bHtml, &strOrKey, idRes, hModule);
    }

//     TMBool FormatWindowText(int nID, ...)
//     {
//         va_list va;
//         va_start (va, nID);
//         TMBool b = _SendSetTextMessage(CM_SETTEXT, 0, false, 0, (TMCStr)nID, &va);
//         va_end (va);
//         return b;
//     }
//
//     TMBool FormatWindowTextEx(TMBool bHtml, TMHModule hModule, int nID, ...)
//     {
//         va_list va;
//         va_start (va, nID);
//         TMBool b = _SendSetTextMessage(CM_SETTEXT, bHtml, hModule, (TMCStr)nID, &va);
//         va_end (va);
//         return b;
//     }

    // use TMRGB or TMRGBA
    TMBool SetTextColor(EButState eBut, TMColor clr)
    {
        return SendMessage(CM_SETTEXTCOLOR, eBut, clr);
    }

    void SetTextFormat(TMUInt uFmt)
    {
        SendMessage(CM_SETTEXTFORMAT, uFmt, 0);
    }

    int GetWindowText(TMStr lpszStringBuf, int nMaxCount) const
    {
        return SendMessage(WM_GETTEXT, nMaxCount, (TMLParam)lpszStringBuf);
    }

    int GetWindowTextLength() const
    {
        return SendMessage(WM_GETTEXTLENGTH);
    }

    bool GetWindowText(CStrPtr& str) const
    {
        _Debug_WidgetMustValidAndSameThread();
        DbgProtect(!m_hWnd, false);
        PRI::tuiGetWindowText(m_hWnd, str);
        return true;
    }
    
    void GetWindowText(std::tstring& str) const
    {
        GetWindowText().swap(str);
    }

    CStrPtr GetWindowText() const
    {
        _Debug_WidgetMustValidAndSameThread();
        CStrPtr ptr;
        DbgProtect(!m_hWnd, ptr);
        PRI::tuiGetWindowText(m_hWnd, ptr);
        return std::move(ptr);
    }

    bool GetClassName(CStrPtr& str) const
    {
        _Debug_WidgetMustValidAndSameThread();
        DbgProtect(!m_hWnd, false);
        PRI::tuiGetClassName(m_hWnd, str);
        return true;
    }

    CStrPtr GetClassName() const
    {
        _Debug_WidgetMustValidAndSameThread();
        CStrPtr ptr;
        DbgProtect(!m_hWnd, ptr);
        PRI::tuiGetClassName(m_hWnd, ptr);
        return std::move(ptr);
    }

    TMBool SetHint(const CStrPtr& strOrKey, int idRes = 0, TMHModule hModule = 0)
    {
        return _SendSetTextMessage(CM_SETHINT, 0, false, &strOrKey, idRes, hModule);
    }

//     TMBool SetHint(int nID, TMHModule hModule = 0)
//     {
//         return _SendSetTextMessage(CM_SETHINT, 0, false, hModule, nID);
//     }
// 
//     TMBool FormatHint(int nID, ...)
//     {
//         va_list va;
//         va_start (va, nID);
//         TMBool b = _SendSetTextMessage(CM_SETHINT, 0, 0, (TMCStr)nID, &va);
//         va_end (va);
//         return b;
//     }
//
//     TMBool FormatHintEx(TMBool bHtml, TMHModule hModule, int nID, ...)
//     {
//         va_list va;
//         va_start (va, nID);
//         TMBool b = _SendSetTextMessage(CM_SETHINT, bHtml, hModule, (TMCStr)nID, &va);
//         va_end (va);
//         return b;
//     }

    bool GetHint(CStrPtr& str)
    {
        _Debug_WidgetMustValidAndSameThread();
        DbgProtect(!m_hWnd, false);
        PRI::tuiGetHint(m_hWnd, str);
        return true;
    }

    CStrPtr GetHint()
    {
        _Debug_WidgetMustValidAndSameThread();
        CStrPtr ptr;
        DbgProtect(!m_hWnd, ptr);
        PRI::tuiGetHint(m_hWnd, ptr);
        return ptr;
    }

    TMBool ActiveHint(TMBool bShow)
    {
        return SendMessage(CM_ACTIVEHINT, 0, bShow);
    }

    TMBool SetAccText(const CStrPtr& strOrKey, int idRes = 0, TMHModule hModule = 0)
    {
        return _SendSetTextMessage(CM_SETACCTEXT, 0, false, &strOrKey, idRes, hModule);
    }

    TMBool SetAccText(int nID, TMHModule hModule = 0)
    {
        return _SendSetTextMessage(CM_SETACCTEXT, 0, false, hModule, nID);
    }

    void AccSpeak()
    {
        PostMessage(CM_ACC_ALERT_EVENT, 0, 0L);
    }

    long AccGetRole()
    {
        _Debug_WidgetMustValidAndSameThread();
        DbgProtect(!m_hWnd, 0);
        return PRI::tuiGetRole(m_hWnd);
    }

    void AccSetRole(long role)
    {
        _Debug_WidgetMustValidAndSameThread();
        DbgProtectVoid(!m_hWnd);
        PRI::tuiSetRole(m_hWnd, role);
    }

//     TMBool FormatAccText(int nID, ...)
//     {
//         DbgAssert(IsWindow());
//         va_list va;
//         va_start (va, nID);
//         TMBool b = _SendSetTextMessage(CM_SETACCTEXT, 0, 0, (TMCStr)nID, &va);
//         va_end (va);
//         return b;
//     }
//
//     TMBool FormatAccTextEx(TMBool bHtml, TMHModule hModule, int nID, ...)
//     {
//         va_list va;
//         va_start (va, nID);
//         TMBool b = _SendSetTextMessage(CM_SETACCTEXT, bHtml, hModule, (TMCStr)nID, &va);
//         va_end (va);
//         return b;
//     }

    bool GetAccText(CStrPtr& str)
    {
        _Debug_WidgetMustValidAndSameThread();
        DbgProtect(!m_hWnd, false);
        PRI::tuiGetAccText(m_hWnd, str);
        return true;
    }

    CString GetAccText()
    {
        _Debug_WidgetMustValidAndSameThread();
        CString ptr;
        DbgProtect(!m_hWnd, ptr);
        PRI::tuiGetAccText(m_hWnd, ptr);
        return std::move(ptr);
    }


    // return e_bss*** | e_bss*** | ...
    EButStates GetState() const
    {
        return (EButStates)SendMessage(CM_GETSTATE, 0, 0L);
    }

//     void SetState(TMUInt uState)
//     {
//         SendMessage(CM_SETSTATE, uState, 0L);
//     }

// Font Functions

    void SetFont(const GXFont* font, TMBool bRedraw = TMTrue)
    {
        SendMessage(CM_SETFONT, (TMWParam)font, MAKELPARAM(bRedraw, 0));
    }

    void GetFont(GXFont* font, TMBool bOnlyGetControlFont = false) const
    {
        SendMessage(CM_GETFONT, (TMWParam)font, bOnlyGetControlFont);
    }

// Menu Functions (non-child windows only)

//     TMHMenu GetMenu() const { return 0; }
//     TMBool SetMenu(TMHMenu hMenu) { return false; }
//     TMBool DrawMenuBar() { return false; }
//     TMHMenu GetSystemMenu(TMBool bRevert) const { return 0; }
//     TMBool HiliteMenuItem(TMHMenu hMenu, TMUInt uItemHilite, TMUInt uHilite) { return false; }

// Window Size and Position Functions

    TMBool IsIconic() const { return GetStyle() & WS_MINIMIZE; }

    TMBool IsZoomed() const { return GetStyle() & WS_MAXIMIZE; }

    TMBool MoveWindow(GXDips x, GXDips y, GXDips nWidth, GXDips nHeight, TMBool bRepaint = TMTrue)
    {
        const TMUInt FLAGS_REDRAW  = (SWP_NOZORDER | SWP_NOACTIVATE);
        const TMUInt FLAGS_NOREDRAW = (FLAGS_REDRAW | SWP_NOREDRAW);
        return SetWindowPos(NULL, x, y, nWidth, nHeight, (bRepaint ? FLAGS_REDRAW : FLAGS_NOREDRAW));
    }

    TMBool MoveWindow(const TMRectF* lpRect, TMBool bRepaint = TMTrue)
    {
        return MoveWindow (RectLeft(*lpRect), RectTop(*lpRect),
            RectWidth(*lpRect), RectHeight(*lpRect), bRepaint);
    }

    TMBool SetWindowPos(HTUI hWndInsertAfter, GXDips x, GXDips y, GXDips cx, GXDips cy, TMUInt nFlags)
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
//         _Debug_WidgetMustBeNotMgr(); // mgr window must use windows function to move/show/hide
//         DbgAssert(((nFlags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE))
//             || ((GetLayOut() & ALIGN_MASK) != ALIGN_CLIENT))("oid:%d", GetDlgCtrlObjectID()); // don't move or resize ALIGN_CLIENT window
        return PRI::tuiSetWindowPos(m_hWnd, hWndInsertAfter, x, y, cx, cy, nFlags);
    }

    TMBool SetWindowPos(HTUI hWndInsertAfter, const TMRectF* lpRect, TMUInt nFlags)
    {
        return SetWindowPos(hWndInsertAfter, RectLeft(*lpRect), RectTop(*lpRect),
            RectWidth(*lpRect), RectHeight(*lpRect), nFlags);
    }

    TMBool SetWindowZOrder(HTUI hIterator, EZOrder eZOrder)
    {
        switch (eZOrder)
        {
        case eZOrderChildFirst:
            return 0 != SetParent(hIterator);

        case eZOrderChildLast:
            if (SetParent(hIterator))
            {
                SendWindowToBack();
                return true;
            }
            return false;

        case eZOrderUp:
            if (SetWindowPos(hIterator, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE))
            {
                CTuiWidget(hIterator).SetWindowPos(*this, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
                return true;
            }
            return false;

        case eZOrderDown:
            return SetWindowPos(hIterator, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

        default:
            return false;
        }
    }

    // see CM_BEGINMENUPOP, CM_ENDMENUPOP
    // hOwner:
    // if hOwner is a pop window(include child), the system will close all hOwner's pop window.
    // if hOwner not a pop window(include child), the system will close all system pop window.
    // if hOwner == self, the system will close all self's pop window
    // when mouse click hOwner, the system will not close owner's pop window, other wise will close all owner's pop window.
    // this function will send CM_BEGINMENUPOP to self.
    // when menu pop end, default will restore focus to hOwner.
    // sample1:
    //  OnPop()
    //  {
    //     TM::CTuiWidget wnd;
    //     wnd.Create(GetWindow(GW_OWNER), IDW_WINDOW_POP);
    //     TM::CRect rc;
    //     GetWindowRect(&rc);
    //     rc.OffsetRect(40, 40);
    //     wnd.SetWindowPos(HTUI_TOP, &rc, 0);
    //     wnd.DoMenuPop();
    //   }
    // sample2:
    //  OnDlgPop()
    //  {
    //      TM::CTuiWidget widget(_tuiApp.CreateDlg(_ModuleATL.GetResourceInstance(), IDD_MANAGE_20010, GetHWND()));
    //      CWindow wnd(widget.GetHWND());
    //      TM::CRect rc;
    //      wnd.GetWindowRect(&rc);
    //      TM::CPoint pt;
    //      GetCursorPos(&pt);
    //      rc.MoveToXY(pt.x, pt.y);
    //      wnd.SetWindowPos(HWND_TOP, &rc, SWP_NOACTIVATE);
    //      widget.DoMenuPop((TM::HTUI)hWndCtl);
    //  }
    TMBool DoMenuPop(HTUI hOwner, HTUI hNewFocus)
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
        DbgAssert(!IsChild(hOwner));
        DbgAssert(hOwner);
        DbgAssert(hOwner != m_hWnd);
        return PRI::tuiDoMenuPop(m_hWnd, hOwner, hNewFocus);
    }

    TMBool DoMenuPop(HTUI hOwner) { return DoMenuPop(hOwner, 0); }

    // hides all pop-up windows owned by current window
    // this function don't close self
    // this function will send CM_ENDMENUPOP to poped window
    // default will destroy window on this message.
    // sample:
    // OnEndMenuPop() { DestroyWindow(); or HideWindow(); }
    inline TMBool EndOwnedMenuPop(int nResult = IDCANCEL, TMBool bRestoreFocus = true, TUIMSG* p = 0)
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(m_hWnd);
        HTUI hWnd = GetMenuPop();
        if (!hWnd)
            return false;
        CTuiApp* pApp = GetAPP();
        int nLevel = pApp->GetMenuPopLevel(m_hWnd, true);
        if (nLevel < 0)
            return false;
        return pApp->EndMenuPopLevel(nLevel, nResult, bRestoreFocus, p);
    }

    // this function will close self
    inline TMBool EndMenuPop(int nResult = IDCANCEL, TMBool bRestoreFocus = true, TUIMSG* p = 0)
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(m_hWnd);
        CTuiApp* pApp = GetAPP();
        int n = pApp->GetMenuPopLevel(m_hWnd, false);
        if (n < 0)
            return false;
        return pApp->EndMenuPopLevel(n, nResult, bRestoreFocus, p);
    }

    inline TMBool EndMenuPopNoDestroy(int nResult = IDCANCEL, TMBool bRestoreFocus = true, TMBool bShow = false)
    {
        TUIMSG msg = { 0, CM_ENDMENUPOPSHOW, (TMWParam)bShow };
        return EndMenuPop(nResult, bRestoreFocus, &msg);
    }

    HTUI GetMenuPopOwner()
    {
        DbgProtect(!m_hWnd, 0);
        DbgAssert(m_hWnd);
        CTuiApp* pApp = GetAPP();
        int n = pApp->GetMenuPopLevel(m_hWnd, false);
        HTUI h = 0;
        pApp->GetMenuPopInfo(n, &h, 0);
        return h;
    }

    HTUI GetMenuPop()
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetMenuPop(m_hWnd);
    }

//     int GetMenuPopLevel();
//
//     void EndMenuPopLevel(int nLevel);



    /**
     *  sort child widget use special function or widget text
     *  @param[in] pfn sort child use function, or set 0 to sort use widget text.
     *  @param[in] lParam specifies a parameter value passed to the compare function.
     *  @param[in] nLevel  child widget layers sorted down. or:
            - 0: only sort current level child widget;
            - -1: sort child widgets of all levels.
     *  @param[in] bReversed forward / reverse sort
     */
    TMBool SortChild(PFNWWCOMPARE pfn, TMLParam lParam, int nLevel, TMBool bReversed)
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiSortChild(m_hWnd, pfn, lParam, nLevel, bReversed);
    }

    /**
     *  let user rebuild client rect and layout all child widget.
     *  @param[in] bUpdate
        - true: layout now
        - false: layout on next message loop
     *  @remark
     *  The flow of ChangeFrame(false):<br>
     *  check window private style (WS_PRI_FRAMECHANGED)<br>
     *  -> yes? return<br>
     *  -> no? Set style (WS_PRI_FRAMECHANGED) && Invalidate<br>
     *  -> ... (MessageLoop)<br>
     *  -> OnPrint<br>
     *  -> ChangeFrame(true)
     */
    TMBool ChangeFrame(TMBool bUnused = false)
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiChangeFrame(m_hWnd, false);
    }

//     TMUInt ArrangeIconicWindows() { return 0; }

    TMBool BringWindowToTop()
    {
        return SetWindowPos(HTUI_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    }

    TMBool SendWindowToBack ()
    {
        return SetWindowPos(HTUI_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    }

    void GetWindowRect(TUIRect* lpRect) const
    {
        DbgProtectVoid(!m_hWnd);
        _Debug_WidgetMustValidAndSameThread();
        DbgAssert(lpRect);
        PRI::tuiGetWindowRect(m_hWnd, lpRect);
    }

    void GetClientRect(TUIRect* lpRect) const
    {
        DbgProtectVoid(!m_hWnd);
        _Debug_WidgetMustValidAndSameThread();
        DbgAssert(lpRect);
        PRI::tuiGetClientRect(m_hWnd, lpRect);
    }

    void GetPaddingRect(TUIRect* lpRect) const
    {
        DbgProtectVoid(!m_hWnd);
        _Debug_WidgetMustValidAndSameThread();
        DbgAssert(lpRect);
        PRI::tuiGetPaddingRect(m_hWnd, lpRect);
    }

    void GetProjectRect(TUIRect* lpRect) const
    {
        DbgProtectVoid(!m_hWnd);
        _Debug_WidgetMustValidAndSameThread();
        DbgAssert(lpRect);
        PRI::tuiGetProjectRect(m_hWnd, lpRect);
    }

//     TMBool GetWindowPlacement(WINDOWPLACEMENT FAR* lpwndpl) const
//     {
//         return false;
//     }
//
//     TMBool SetWindowPlacement(const WINDOWPLACEMENT FAR* lpwndpl)
//     {
//         return false;
//     }

// Coordinate Mapping Functions
    void ClientToScreen(const TMPointL* lpPoint, int nPt, TMPointD* pOut) const
    {
        _Debug_WidgetMustValidAndSameThread();
        DbgProtectVoid(!m_hWnd);
        PRI::tuiClientToScreen(m_hWnd, lpPoint, nPt, pOut);
    }

    void ScreenToClient(const TMPointD* lpPoint, int nPt, TMPointL* pOut) const
    {
        _Debug_WidgetMustValidAndSameThread();
        DbgProtectVoid(!m_hWnd);
        PRI::tuiScreenToClient(m_hWnd, lpPoint, nPt, pOut);
    }

    void ClientToScreen(const TMRectL& rect, TMRectD& out) const
    {
        ClientToScreen((TMPointL*)&rect, 2, (TMPointD*)&out);
    }

    void ScreenToClient(const TMRectD& rect, TMRectL& out) const
    {
        ScreenToClient((TMPointD*)&rect, 2, (TMPointL*)&out);
    }


//  int MapWindowPoints(HTUI hWndTo, LPPOINT lpPoint, TMUInt nCount) const;
//  int MapWindowPoints(HTUI hWndTo, TMRect* lpRect) const;

// Update and Painting Functions

//  TMHDC BeginPaint(LPPAINTSTRUCT lpPaint);
//  void EndPaint(LPPAINTSTRUCT lpPaint);

//     TMHDC GetDC(TMBool bClip = false);

//     TMHDC GetWindowDC();

//     int ReleaseDC(TMHDC hDC);

    void Print(TMHDC unused, TMUInt32 dwFlags) const
    {
        SendMessage(WM_PRINT, 0, dwFlags);
    }

    void PrintClient(TMHDC unused, TMUInt32 dwFlags) const
    {
        SendMessage(WM_PRINTCLIENT, 0, dwFlags);
    }

    TMBool UpdateWindow()
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiUpdateWindow(m_hWnd);
    }

    void SetRedraw(TMBool bRedraw = TMTrue)
    {
        SendMessage(WM_SETREDRAW, (TMWParam)bRedraw, 0);
    }

//     TMBool GetUpdateRect(TMRect* lpRect, TMBool bErase = TMFalse);

//    int GetUpdateRgn(HRGN hRgn, TMBool bErase = TMFalse);

    TMBool Invalidate(TMBool bErase = TMTrue)
    {
        return InvalidateRect(0, bErase);
    }

    TMBool PostInvalidate() tm_thread_safe
    {
        return PostMessage(CM_INVALIDATE, 0, 0);
    }

    TMBool InvalidateRect(const TUIRect* lpRect, TMBool bErase = TMTrue)
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiInvalidateRect(m_hWnd, 0, bErase);
    }

//     TMBool ValidateRect(const TMRect* lpRect);

//     void InvalidateRgn(HRGN hRgn, TMBool bErase = TMTrue);
//
//     TMBool ValidateRgn(HRGN hRgn);

    TMBool ShowWindow(int nCmdShow)
    {
        const TMUInt us = SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW;
        const TMUInt uh = SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_HIDEWINDOW;
        return SetWindowPos(0, 0, 0, 0, 0, nCmdShow ? us : uh);
    }

    bool IsWindowVisible() const
    {
        return 0 != (GetStyle() & WS_VISIBLE);
    }

    TMBool ShowOwnedPopups(TMBool bShow = TMTrue) { return false; }

//     TMHDC GetDCEx(HRGN hRgnClip, TMUInt flags)
//     {
//         return GetDC ();
//     }
//
//     TMBool LockWindowUpdate(TMBool bLock = TMTrue);
//
//     TMBool RedrawWindow(const TMRect* lpRectUpdate = NULL, HRGN hRgnUpdate = NULL, TMUInt flags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

// Timer Functions

//  TMUInt SetTimer(TMUInt nIDEvent, TMUInt nElapse);
//  TMBool KillTimer(TMUInt nIDEvent);

// Window State Functions

    TMBool IsWindowEnabled() const
    {
        return !(GetStyle() & WS_DISABLED);
    }

    TMBool EnableWindow(TMBool bEnable = TMTrue)
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiEnableWindow(m_hWnd, bEnable);
    }

    TMBool EnableNotify(TMBool bEnable = TMTrue) // enable control send WM_COMMAND message
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiEnableNotify(m_hWnd, bEnable);
    }

    TMBool IsEnableNotify()
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiIsEnableNotify(m_hWnd);
    }

//  HTUI SetActiveWindow();

    // window mode:  capture mouse to window. window dispatch to widget from pointer.
    // other wise:   capture mouse to current widget.
    TMBool SetCapture ()
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(IsWindow());
        return GetAPP()->SetCapture(m_hWnd);
    }

    TMBool IsCapture()
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(IsWindow());
        return m_hWnd == GetAPP()->GetCapture();
    }

    TMBool ReleaseCapture ()
    {
        DbgProtect(!m_hWnd, false);
        TMBool b = IsCapture();
        if (b)
            GetAPP()->SetCapture(0);
        return b;
    }

    inline TMBool SetFocus(EFocusReason reason = eFocusReasonUnknown)
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(IsWindow());
        return GetAPP()->SetFocus(m_hWnd, reason);
    }

    inline TMBool SetGroupDefaultTabStop()
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(IsWindow());
        return SendMessage(CM_SETGROUPDFTTABSTOP, 0, 0);
    }

    inline TMBool IsFocus()
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(IsWindow());
        return m_hWnd == GetAPP()->GetFocus();
    }

    HTUI GetNextDlgTabItem(TMBool bPrevious)
    {
        return GetWindow(bPrevious ? GW_TABPREV : GW_TABNEXT); // (HTUI)SendMessage(bPrevious ? CM_GETPREVTAB : CM_GETNEXTTAB);
    }

//     HTUI GetFocus();
// Dialog-Box Item Functions

    TMBool CheckDlgButton(int nIDButton, TMUInt nCheck)
    {
        return SendDlgItemMessage(nIDButton, BM_SETCHECK, nCheck);
    }

//  TMBool CheckRadioButton(int nIDFirstButton, int nIDLastButton, int nIDCheckButton);
//  int DlgDirList(TMStr lpPathSpec, int nIDListBox, int nIDStaticPath, TMUInt nFileType);
//  int DlgDirListComboBox(TMStr lpPathSpec, int nIDComboBox, int nIDStaticPath, TMUInt nFileType);
//  TMBool DlgDirSelect(TMStr lpString, int nCount, int nIDListBox);
//  TMBool DlgDirSelectComboBox(TMStr lpString, int nCount, int nIDComboBox);

    TMUInt GetDlgItemInt(int nID, TMBool* lpTrans = NULL, TMBool bSigned = TMTrue) const
    {
        TMCharT c[64];
        TMUInt n = GetDlgItemText(nID, c, 64);
        long x = 0;
        StrScanLong(c, n, x, 0);
        return x;
    }

    double GetDlgItemDouble(int nID, double dft = 0) const
    {
        TMCharT c[64];
        TMUInt n = GetDlgItemText(nID, c, 64);
        double x = dft;
        StrScanDouble(c, n, x);
        return x;
    }

    TMUInt GetDlgItemText(int nID, TMStr lpStr, int nMaxCount) const
    {
        return SendDlgItemMessage(nID, WM_GETTEXT, nMaxCount, (TMLParam)lpStr);
    }

//    TMBool GetDlgItemText(int nID, BSTR& bstrText) const;
//    HTUI GetNextDlgGroupItem(HTUI hWndCtl, TMBool bPrevious = TMFalse) const;
//    HTUI GetNextDlgTabItem(HTUI hWndCtl, TMBool bPrevious = TMFalse) const;

    TMUInt IsDlgButtonChecked(int nIDButton) const
    {
        return SendDlgItemMessage(nIDButton, BM_GETCHECK);
    }

    TMResult SendDlgItemMessage(int nID, TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0) const
    {
        _Debug_WidgetMustValidAndSameThread();
        CTuiWidget win(GetDlgItem(nID));
        if (!win)
            return 0;
        return win.SendMessage(message, wParam, lParam);
    }

    TMBool SetDlgItemInt(int nID, TMUInt nValue, TMBool bSigned = TMTrue)
    {
        CTuiWidget wnd(GetDlgItem(nID));
        if (!wnd)
            return false;
        TMCharT c[32];
        if (bSigned)
            StrPrintf(c, 32, TM_T("%u"), nValue);
        else
            StrPrintf(c, 32, TM_T("%d"), (int)nValue);
        return wnd.SetWindowText(c);
    }

    TMBool SetDlgItemText(int nID, TMCStr lpszString)
    {
        return SendDlgItemMessage(nID, WM_SETTEXT, 0, (TMLParam)lpszString);
    }

// Scrolling Functions

    void GetScrollInfo(TUISCROLLINFO* lpHInfo, TUISCROLLINFO* lpVInfo) const
    {
        SendMessage(CM_GETSCROLLINFO, (TMWParam)lpHInfo, (TMLParam)lpVInfo);
    }

    void GetScrollBarInfo(TUISCROLLBARINFO* lpHInfo, TUISCROLLBARINFO* lpVInfo) const
    {
        SendMessage(CM_GETSCROLLBARINFO, (TMWParam)lpHInfo, (TMLParam)lpVInfo);
    }

//     void SetScrollInfo(TUISCROLLINFO* lpHInfo, TUISCROLLINFO* lpVInfo) const
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CM_SETSCROLLINFO, (TMWParam)lpHInfo, (TMLParam)lpVInfo);
//     }

    // set TUISCROLLINFO::nProject
    TMBool SetScrollRange(GXDips x, GXDips y) const
    {
        return SetScrollRange(&x, &y);
    }

    // 0: skip
    TMBool SetScrollRange(const GXDips* x, const GXDips* y) const
    {
        return SendMessage(CM_SETSCROLLRANGE, (TMWParam)x, (TMLParam)y);
    }

    // offset TUISCROLLINFO::nOrg
    void ScrollBy(GXDips* pdx, GXDips* pdy)// pdx and pdy will return the current pos
    {
        this->SendMessage(CM_SCROLLBY, (TMWParam)pdx, (TMLParam)pdy);
    }

    void ScrollBy(GXDips dx, GXDips dy)
    {
        ScrollBy(&dx, &dy);
    }

    int ScrollByX(GXDips dx)
    {
        ScrollBy(&dx, 0);
        return dx;
    }
    
    int ScrollByY(GXDips dy)
    {
        ScrollBy(0, &dy);
        return dy;
    }

    // set TUISCROLLINFO::nOrg
    TMBool ScrollTo(GXDips* px, GXDips* py) // pdx and pdy will return the current pos
    {
        return this->SendMessage(CM_SCROLLTO, (TMWParam)px, (TMLParam)py);
    }

    TMBool ScrollTo(GXDips x, GXDips y)
    {
        return ScrollTo(&x, &y);
    }

    TMBool ScrollToX(GXDips x)
    {
        return ScrollTo(&x, 0);
    }

    TMBool ScrollToY(GXDips y)
    {
        return ScrollTo(0, &y);
    }

//     TMBool GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo) const;
//
//     TMBool SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, TMBool bRedraw = TMTrue);
//
//     int GetScrollPos(int nBar) const;
//
//     TMBool GetScrollRange(int nBar, TMInt* lpMinPos, TMInt* lpMaxPos) const;

//  TMBool ScrollWindow(int xAmount, int yAmount, const TMRect* lpRect = NULL, const TMRect* lpClipRect = NULL);
//
//  int ScrollWindowEx(int dx, int dy, const TMRect* lpRectScroll, const TMRect* lpRectClip, HRGN hRgnUpdate, TMRect* lpRectUpdate, TMUInt uFlags);
//
//  int ScrollWindowEx(int dx, int dy, TMUInt uFlags, const TMRect* lpRectScroll = NULL, const TMRect* lpRectClip = NULL, HRGN hRgnUpdate = NULL, TMRect* lpRectUpdate = NULL);

//     int SetScrollPos(int nBar, int nPos, TMBool bRedraw = TMTrue);
//
//     TMBool SetScrollRange(int nBar, int nMinPos, int nMaxPos, TMBool bRedraw = TMTrue);
//
//     TMBool ShowScrollBar(TMUInt nBar, TMBool bShow = TMTrue);

//  TMBool EnableScrollBar(TMUInt uSBFlags, TMUInt uArrowFlags = ESB_ENABLE_BOTH);

// Window Access Functions

    // CWP_SKIPBINDWINDOW
    // this function may be return self.
    HTUI ChildWindowFromPoint(const CTuiPoint& point, TMUInt uFlags = CWP_SKIPINVISIBLE | CWP_SKIPBINDWINDOW | CWP_SKIPTRANSPARENT) const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiChildWindowFromPoint(m_hWnd, point, uFlags);
    }

//     HTUI ChildWindowFromPointEx(TMPointL point, TMUInt uFlags) const;

    HTUI GetTopWindow() const
    {
        return GetWindow(GW_CHILD);
    }

    HTUI GetWindow(TMUInt nCmd) const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetWindow(m_hWnd, nCmd);
    }

//  HTUI GetLastActivePopup() const;

    // hChild can be 0;
    TMBool IsChild(HTUI hChild) const
    {
        DbgProtect(!m_hWnd, false);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiIsChild(m_hWnd, hChild);
    }

    TMBool IsSelfOrChild(HTUI hChild) const
    {
        return hChild == m_hWnd || IsChild(hChild);
    }

    HTUI GetParent() const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetParent(m_hWnd);
    }

    // set to hWndNewParent's first child.
    HTUI SetParent(HTUI hWndNewParent)
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiSetParent(m_hWnd, hWndNewParent);
    }

// Window Tree Access

    int GetDlgCtrlID() const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetDlgCtrlID(m_hWnd);
    }

    int SetDlgCtrlID(int nID)
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiSetDlgCtrlID(m_hWnd, nID);
    }

    int GetDlgCtrlObjectID() const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetDlgCtrlObjectID(m_hWnd);
    }

    HTUI GetDlgItem(int nID) const
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiGetDlgItem(m_hWnd, nID);
    }

    CTuiWidget Item(int nID) const { return GetDlgItem(nID); }



// Alert Functions

//  TMBool FlashWindow(TMBool bInvert);
//
//  int MessageBox(TMCStr lpszText, TMCStr lpszCaption = _T(""), TMUInt nType = MB_OK);

// Clipboard Functions

//  TMBool ChangeClipboardChain(HTUI hWndNewNext);
//
//  HTUI SetClipboardViewer()
//
//  TMBool OpenClipboard()

// Caret Functions

//  TMBool CreateCaret(HBITMAP hBitmap);
//
//  TMBool CreateSolidCaret(int nWidth, int nHeight)
//
//  TMBool CreateGrayCaret(int nWidth, int nHeight)
//
//  TMBool HideCaret()
//
//  TMBool ShowCaret()
//
// #ifdef _INC_SHELLAPI
// // Drag-Drop Functions
//  void DragAcceptFiles(TMBool bAccept = TMTrue)
// #endif

// Icon Functions

 /*   HICON SetIcon(HICON hIcon, TMBool bBigIcon = TMTrue) { return 0; }

    HICON GetIcon(TMBool bBigIcon = TMTrue) const { return 0; }*/

// Help Functions

//  TMBool WinHelp(TMCStr lpszHelp, TMUInt nCmd = HELP_CONTEXT, TMUInt dwData = 0);
//
//  TMBool SetWindowContextHelpId(TMUInt dwContextHelpId);
//
//  TMUInt GetWindowContextHelpId() const;

// Hot Key Functions

//  int SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);
//
//  TMUInt GetHotKey() const;

    GXPTexture MakeImage(const CTuiRect* prc = 0)
    {
        _Debug_WidgetMustValidAndSameThread();
        DbgProtect(!m_hWnd, 0);
        return PRI::tuiMakeImage(m_hWnd, prc);
    }

    void SetDragImage(CDragData& obj, TMPointD ptOffset, const CTuiRect* prc = 0, TMUInt8 alpha = 0xff, TMBool bSrcAlpha = true, TMColor clrKey = cColorInvalid)
    {
        _Debug_WidgetMustValidAndSameThread();
        DbgProtectVoid(!m_hWnd);
//         GXPTexture tex = MakeImage(prc);
//         if (tex)
//         {
//             CAutoTextureDC dc(tex);
//             obj.SetDragImage(ptOffset, dc, CRectI(0,0,tex->GetWidth(),tex->GetHeight()), alpha, bSrcAlpha, clrKey);
//             tex->Release();
//         }
        PRI::tuiSetDragImage(m_hWnd, obj, ptOffset, prc, alpha, bSrcAlpha, clrKey);
    }

    TMUInt32 DoDrag(CDragData& obj, EDragEff dwEff)
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiDoDrag(m_hWnd, obj, dwEff);
    }

    TMBool EnumAllChildWindows(PFNWWENUMPROC lpEnumFunc, TMLParam lParam, TMBool bEnumSelf = true, size_t nLevel = -1)
    {
        DbgProtect(!m_hWnd, 0);
        _Debug_WidgetMustValidAndSameThread();
        return PRI::tuiEnumAllChildWindows(m_hWnd, lpEnumFunc, lParam, bEnumSelf, nLevel);
    }

    TMBool EnumChildWindows(PFNWWENUMPROC lpEnumFunc, TMLParam lParam)
    {
        return EnumAllChildWindows(lpEnumFunc, lParam, false, 1);
    }

    void SetPading(int nLeft, int nTop,int nRight, int nBottom)
    {
        TMRectL rc = {(GXDips)nLeft, (GXDips)nTop, (GXDips)nRight, (GXDips)nBottom};
        this->SendMessage(CM_SET_PADING, (TMWParam)&rc, 0);
    }

    void GetPading(TMRectL& rc)
    {
        this->SendMessage(CM_GET_PADING, 0, (TMLParam)&rc);
    }

    void SetMargin(int nLeft, int nTop,int nRight, int nBottom)
    {
        TMRectL rc = { (GXDips)nLeft, (GXDips)nTop, (GXDips)nRight, (GXDips)nBottom};
        this->SendMessage(CM_SET_MARGIN, (TMWParam)&rc, 0);
    }

    void GetMargin(TMRectL& rc)
    {
        this->SendMessage(CM_GET_MARGIN, 0, (TMLParam)&rc);
    }

    void SetMaxWidth(int nWidth)
    {
        TUIMINMAXINFO wmmi = { -1, nWidth };
        this->SetMinMaxInfo(SWMMI_MAXWIDTH, wmmi);
    }

    void SetMinWidth(int nWidth)
    {
        TUIMINMAXINFO wmmi = { nWidth };
        this->SetMinMaxInfo(SWMMI_MINWIDTH, wmmi);
    }

    void SetMaxHeight(int nHeight)
    {
        TUIMINMAXINFO wmmi = { -1, -1, -1, nHeight };
        this->SetMinMaxInfo(SWMMI_MAXHEIGHT, wmmi);
    }

    void SetMinHeight(int nHeight)
    {
        TUIMINMAXINFO wmmi = { -1, -1, nHeight };
        this->SetMinMaxInfo(SWMMI_MINHEIGHT, wmmi);
    }

    void SetMinMaxInfo(TMUInt32 nFlag, const TUIMINMAXINFO& info)
    {
        this->SendMessage(CM_SETMINMAXINFO, nFlag, (TMLParam)&info);
    }

    void GetMinMaxInfo(TUIMINMAXINFO& info)
    {
        this->SendMessage(CM_GETMINMAXINFO, 0, (TMLParam)&info);
    }

// Misc. Operations

//N new
    /*
    TMBool IsDialogMessage(LPMSG lpMsg)
    {
        DbgAssert(IsWindow(m_hWnd));
        return ::IsDialogMessage(m_hWnd, lpMsg);
    }*/

    void NextDlgCtrl() const
    {
        SendMessage(WM_NEXTDLGCTL, 0, 0L);
    }

    void PrevDlgCtrl() const
    {
        SendMessage(WM_NEXTDLGCTL, 1, 0L);
    }

    void GotoDlgCtrl(HTUI hWndCtrl) const
    {
        SendMessage(WM_NEXTDLGCTL, (TMWParam)hWndCtrl, 1L);
    }

/*
    TMBool ResizeClient(int nWidth, int nHeight, TMBool bRedraw = TMTrue)
    {
        DbgAssert(IsWindow(m_hWnd));

        TMRect rcWnd;
        if (!GetClientRect(&rcWnd))
            return TMFalse;

        if (nWidth != -1)
            rcWnd.right = nWidth;
        if (nHeight != -1)
            rcWnd.bottom = nHeight;

        if (!::AdjustWindowRectEx(&rcWnd, GetStyle(), (!(GetStyle() & WS_CHILD) && (GetMenu() != NULL)), GetExStyle()))
            return TMFalse;

        TMUInt uFlags = SWP_NOZORDER | SWP_NOMOVE;
        if (!bRedraw)
            uFlags |= SWP_NOREDRAW;

        return SetWindowPos(NULL, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, uFlags);
    }

    int GetWindowRgn(HRGN hRgn)
    {
        DbgAssert(IsWindow(m_hWnd));
        return ::GetWindowRgn(m_hWnd, hRgn);
    }
    int SetWindowRgn(HRGN hRgn, TMBool bRedraw = TMFalse)
    {
        DbgAssert(IsWindow(m_hWnd));
        return ::SetWindowRgn(m_hWnd, hRgn, bRedraw);
    }
    HDWP DeferWindowPos(HDWP hWinPosInfo, HTUI hWndInsertAfter, int x, int y, int cx, int cy, TMUInt uFlags)
    {
        DbgAssert(IsWindow(m_hWnd));
        return ::DeferWindowPos(hWinPosInfo, m_hWnd, hWndInsertAfter, x, y, cx, cy, uFlags);
    }
    TMUInt GetWindowThreadID()
    {
        DbgAssert(IsWindow(m_hWnd));
        return ::GetWindowThreadProcessId(m_hWnd, NULL);
    }
    TMUInt GetWindowProcessID()
    {
        DbgAssert(IsWindow(m_hWnd));
        TMUInt dwProcessID;
        ::GetWindowThreadProcessId(m_hWnd, &dwProcessID);
        return dwProcessID;
    }
    TMBool IsWindow()
    {
        return IsWindow(m_hWnd);
    }
    TMBool IsWindowUnicode()
    {
        DbgAssert(IsWindow(m_hWnd));
        return ::IsWindowUnicode(m_hWnd);
    }
    TMBool IsParentDialog()
    {
        DbgAssert(IsWindow(m_hWnd));
        TMCharT szBuf[8]; // "#32770" + NUL character
        GetClassName(GetParent(), szBuf, sizeof(szBuf)/sizeof(TMCharT));
        return lstrcmp(szBuf, _T("#32770")) == 0;
    }
    TMBool ShowWindowAsync(int nCmdShow)
    {
        DbgAssert(IsWindow(m_hWnd));
        return ::ShowWindowAsync(m_hWnd, nCmdShow);
    }

    HTUI GetDescendantWindow(int nID) const
    {
        DbgAssert(IsWindow(m_hWnd));

        // GetDlgItem recursive (return first found)
        // breadth-first for 1 level, then depth-first for next level

        // use GetDlgItem since it is a fast USER function
        HTUI hWndChild, hWndTmp;
        CWindow wnd;
        if ((hWndChild = ::GetDlgItem(m_hWnd, nID)) != NULL)
        {
            if (::GetTopWindow(hWndChild) != NULL)
            {
                // children with the same ID as their parent have priority
                wnd.Attach(hWndChild);
                hWndTmp = wnd.GetDescendantWindow(nID);
                if (hWndTmp != NULL)
                    return hWndTmp;
            }
            return hWndChild;
        }

        // walk each child
        for (hWndChild = ::GetTopWindow(m_hWnd); hWndChild != NULL;
            hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
        {
            wnd.Attach(hWndChild);
            hWndTmp = wnd.GetDescendantWindow(nID);
            if (hWndTmp != NULL)
                return hWndTmp;
        }

        return NULL;    // not found
    }

    void SendMessageToDescendants(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0, TMBool bDeep = TMTrue)
    {
        CWindow wnd;
        for (HTUI hWndChild = ::GetTopWindow(m_hWnd); hWndChild != NULL;
            hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
        {
            ::SendMessage(hWndChild, message, wParam, lParam);

            if (bDeep && ::GetTopWindow(hWndChild) != NULL)
            {
                // send to child windows after parent
                wnd.Attach(hWndChild);
                wnd.SendMessageToDescendants(message, wParam, lParam, bDeep);
            }
        }
    }

    TMBool CenterWindow(HTUI hWndCenter = NULL)
    {
        DbgAssert(IsWindow(m_hWnd));

        // determine owner window to center against
        TMUInt dwStyle = GetStyle();
        if (hWndCenter == NULL)
        {
            if (dwStyle & WS_CHILD)
                hWndCenter = ::GetParent(m_hWnd);
            else
                hWndCenter = ::GetWindow(m_hWnd, GW_OWNER);
        }

        // get coordinates of the window relative to its parent
        TMRect rcDlg;
        ::GetWindowRect(m_hWnd, &rcDlg);
        TMRect rcArea;
        TMRect rcCenter;
        HTUI hWndParent;
        if (!(dwStyle & WS_CHILD))
        {
            // don't center against invisible or minimized windows
            if (hWndCenter != NULL)
            {
                TMUInt dwStyle = ::GetWindowLong(hWndCenter, GWL_STYLE);
                if (!(dwStyle & WS_VISIBLE) || (dwStyle & WS_MINIMIZE))
                    hWndCenter = NULL;
            }

            // center within screen coordinates
            ::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
            if (hWndCenter == NULL)
                rcCenter = rcArea;
            else
                ::GetWindowRect(hWndCenter, &rcCenter);
        }
        else
        {
            // center within parent client coordinates
            hWndParent = ::GetParent(m_hWnd);
            DbgAssert(IsWindow(hWndParent));

            ::GetClientRect(hWndParent, &rcArea);
            DbgAssert(IsWindow(hWndCenter));
            ::GetClientRect(hWndCenter, &rcCenter);
            ::MapWindowPoints(hWndCenter, hWndParent, (TMPointL*)&rcCenter, 2);
        }

        int DlgWidth = rcDlg.right - rcDlg.left;
        int DlgHeight = rcDlg.bottom - rcDlg.top;

        // find dialog's upper left based on rcCenter
        int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
        int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

        // if the dialog is outside the screen, move it inside
        if (xLeft < rcArea.left)
            xLeft = rcArea.left;
        else if (xLeft + DlgWidth > rcArea.right)
            xLeft = rcArea.right - DlgWidth;

        if (yTop < rcArea.top)
            yTop = rcArea.top;
        else if (yTop + DlgHeight > rcArea.bottom)
            yTop = rcArea.bottom - DlgHeight;

        // map screen coordinates to child coordinates
        return ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
*/
    TMBool ModifyStyle(TMUInt dwRemove, TMUInt dwAdd)
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(IsWindow());
        TMUInt32 dwStyle = GetStyle();
        TMUInt32 dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
        if (dwStyle == dwNewStyle)
            return TMFalse;
        SetStyle(dwNewStyle);
        return TMTrue;
    }

    TMBool ModifyStyleEx(TMUInt dwRemove, TMUInt dwAdd)
    {
        DbgProtect(!m_hWnd, false);
        DbgAssert(IsWindow());
        TMUInt32 dwStyle = GetExStyle();
        TMUInt32 dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
        if (dwStyle == dwNewStyle)
            return TMFalse;
        SetExStyle(dwNewStyle);
        return TMTrue;
    }

    void GetRichContext(IRichContext** ppObj)
    {
        DbgProtectVoid(!m_hWnd);
        DbgAssert(IsWindow());
        DbgAssert(ppObj);
        DbgAssert(!*ppObj);
        PRI::tuiGetRichContext(m_hWnd, ppObj);
    }

/*
    TMBool GetWindowText(BSTR* pbstrText)
    {
        return GetWindowText(*pbstrText);
    }
    TMBool GetWindowText(BSTR& bstrText)
    {
        USES_CONVERSION;
        DbgAssert(IsWindow(m_hWnd));
        if (bstrText != NULL)
        {
            SysFreeString(bstrText);
            bstrText = NULL;
        }

        int nLen = ::GetWindowTextLength(m_hWnd);
        if (nLen == 0)
        {
            bstrText = ::SysAllocString(OLESTR(""));
            return (bstrText != NULL) ? TMTrue : TMFalse;
        }

        TMStr lpszText = (TMStr)_alloca((nLen+1)*sizeof(TMCharT));

        if (!::GetWindowText(m_hWnd, lpszText, nLen+1))
            return TMFalse;

        bstrText = ::SysAllocString(T2OLE(lpszText));
        return (bstrText != NULL) ? TMTrue : TMFalse;
    }
    HTUI GetTopLevelParent() const
    {
        DbgAssert(IsWindow(m_hWnd));

        HTUI hWndParent = m_hWnd;
        HTUI hWndTmp;
        while((hWndTmp = ::GetParent(hWndParent)) != NULL)
            hWndParent = hWndTmp;

        return hWndParent;
    }

    HTUI GetTopLevelWindow() const
    {
        DbgAssert(IsWindow(m_hWnd));

        HTUI hWndParent;
        HTUI hWndTmp = m_hWnd;

        do
        {
            hWndParent = hWndTmp;
            hWndTmp = (::GetWindowLong(hWndParent, GWL_STYLE) & WS_CHILD) ? ::GetParent(hWndParent) : ::GetWindow(hWndParent, GW_OWNER);
        }
        while(hWndTmp != NULL);

        return hWndParent;
    }*/

    // serialize

//     CSerializeWindowTextT<CTuiWidget> SerializeText() const
//     {
//         return CSerializeWindowTextT<CTuiWidget>(*this);
//     }
};


////////////////////////////////////////////////////////////////////////////////
// class CAutoTuiWidget
static TMBool _CAutoTuiWidget_DetachProc(
    CTuiWidget* pWnd,
    HTUI hWnd, TMUInt uMsg, TMWParam wParam,
    TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    if (uMsg == WM_NCDESTROY)
        *pWnd = 0;
    return false;
}

template <class _TBase>
struct CAutoTuiWidgetT : public _TBase
{
private:
    // hide base class function.
    TM_NO_COPYABLE(CAutoTuiWidgetT);
    void Attach();
    void Detach();
    void SubclassWindow();
    void UnsubclassWindow();
    void Create();
    CWidgetMsgDelegate GetDelegate() const
    {
        return CWidgetMsgDelegate((CTuiWidget*)this, &_CAutoTuiWidget_DetachProc);
    }

    void _SubClass(HTUI hWgt)
    {
        if (hWgt && CTuiWidget(hWgt).SubclassWindow(GetDelegate()))
            this->m_hWnd = hWgt;
    }

    void _Unsubclass()
    {
        if (this->m_hWnd)
        {
            TMBool b = CTuiWidget::UnsubclassWindow(GetDelegate());
            DbgAssert(b);
            this->m_hWnd = 0;
        }
    }

public:
    CAutoTuiWidgetT(HTUI hWnd = NULL)
        : _TBase(0)
    {
        _SubClass(hWnd);
    }

    ~CAutoTuiWidgetT()
    {
        _Unsubclass();
    }

    void SubclassWindow(HTUI hWgt)
    {
        if (hWgt == this->m_hWnd)
            return;
        _Unsubclass();
        _SubClass(hWgt);
    }

    void operator = (HTUI hWgt)
    {
        this->SubclassWindow(hWgt);
    }

    void DestroyWindow()
    {
        DbgAssert(this->m_hWnd);
        CTuiWidget(this->m_hWnd).DestroyWindow();
        DbgAssert(!this->m_hWnd);
    }
};

typedef CAutoTuiWidgetT<CTuiWidget>                      CAutoTuiWidget;






#define CLASS_IMPL(class_name,class_str) \
    class_name(HTUI hWnd = NULL) : TBase(hWnd) {} \
    class_name & operator =(HTUI hWnd) { this->m_hWnd = hWnd; return *this; } \
    static CStrPtr GetWndClassName() { return TM_CS(class_str); }




////////////////////////////////////////////////////////////////////////////////
// class CTuiWindow
template <class TBase>
class CTuiWindowT : public TBase
{
public:
    CLASS_IMPL(CTuiWindowT, TUI_CLASS_WINDOW);

    TMBool SubclassMgr(const CWindowMsgDelegate& func)
    {
        DbgProtect(!this->m_hWnd, false);
        this->_Debug_WidgetMustValidAndSameThread();
        this->_Debug_WidgetMustBeMgr();
        return PRI::tuiSubclassMgr(this->m_hWnd, func);
    }

    bool UnsubclassMgr(const CWindowMsgDelegate& func)
    {
        DbgProtect(!this->m_hWnd, false);
        this->_Debug_WidgetMustValidAndSameThread();
        this->_Debug_WidgetMustBeMgr();
        return PRI::tuiUnsubclassMgr(this->m_hWnd, func);
    }

    TMResult DefSubclassMgrProc(TMUInt uMsg, TMWParam wp, TMLParam lp)
    {
        DbgProtect(!this->m_hWnd, false);
        this->_Debug_WidgetMustValidAndSameThread();
        this->_Debug_WidgetMustBeMgr();
        return PRI::tuiDefSubclassMgrProc(this->m_hWnd, uMsg, wp, lp);
    }

    void SetAlpha(TMUInt8 alpha)
    {
        this->_Debug_WidgetMustBeMgr();
        this->SendMessage(CM_SETALPHA, 0, alpha);
    }

    void SetInterval(int interval)
    {
        this->_Debug_WidgetMustBeMgr();
        this->SendMessage(CM_SETINTERVAL, 0, interval);
    }

    GXPTexture GetBkTexture()
    {
        this->_Debug_WidgetMustBeMgr();
        return (GXPTexture)this->SendMessage(CM_GET_BK_TEX, 0, 0);
    }

    HTUI GetMenuBar()
    {
        this->_Debug_WidgetMustBeMgr();
        return (HTUI)this->SendMessage(CM_GET_MENUBAR, 0, 0);
    }

    TMBool SetDefaultItem(int id)
    {
        this->_Debug_WidgetMustBeMgr();
        return (TMBool)this->SendMessage(CM_SETDEFAULTITEM, 0, id);
    }

    TMBool SetDefaultFocus(HTUI hWnd)
    {
        this->_Debug_WidgetMustBeMgr();
        return (TMBool)this->SendMessage(CM_SETDEFAULTFOCUS, 0, (TMLParam)hWnd);
    }

    EThemeType GetThemeType()
    {
        this->_Debug_WidgetMustBeMgr();
        return (EThemeType)this->SendMessage(CM_GET_THEME_TYPE, 0, 0);
    }

    TMBool SetThemeType(EThemeType eType)
    {
        this->_Debug_WidgetMustBeMgr();
        return this->SendMessage(CM_SET_THEME_TYPE, eType, 0);
    }

     int DoDialogBox(TMHWnd hOwner)
     {
         DbgProtect(!this->m_hWnd, false);
         this->_Debug_WidgetMustValidAndSameThread();
         this->_Debug_WidgetMustBeMgr();
         return PRI::tuiDoDialogBox(this->m_hWnd, hOwner);
     }

    TMBool EndDialog(int nCode)
    {
        DbgProtect(!this->m_hWnd, false);
        this->_Debug_WidgetMustValidAndSameThread();
        this->_Debug_WidgetMustBeMgr();
        return PRI::tuiEndDialog(this->m_hWnd, nCode);
    }

    void SetDPI(int nDpi)
    {
        this->_Debug_WidgetMustBeMgr();
        this->SendMessage(WM_DPICHANGED, MAKEWPARAM(nDpi, nDpi), 0);
    }

//     CSerializeWindowPlacement SerializeWindowPlacement() { return CSerializeWindowPlacement(this->GetHWND()); }
#if TM_WIN_DESKTOP

#endif // TM_WIN_DESKTOP
};

typedef CTuiWindowT<CTuiWidget>                      CTuiWindow;



////////////////////////////////////////////////////////////////////////////////
// class CTuiStatic
template < class TBase >
class CTuiStaticT : public TBase
{
public:
    CLASS_IMPL(CTuiStaticT, TUI_CLASS_STATIC);

    int GetCheck() const
    {
        return (int)TBase::SendMessage(CBM_GETCHECK, 0, 0L);
    }

    void SetCheck (int nCheck)
    {
        TBase::SendMessage(CBM_SETCHECK, nCheck, 0L);
    }
};

typedef CTuiStaticT < CTuiWidget >                      CTuiStatic;


////////////////////////////////////////////////////////////////////////////////
// class CTuiButton
enum EAlignMode
{
    e_amMaskDir         = 0x01,
    e_amMaskDirH        = 0x00,
    e_amMaskDirV        = 0x01,

    e_amMaskMode        = 0xf0,
    e_amMode1           = 0x00, // center
    e_amMode2           = 0x10, // left or top
    e_amMode3           = 0x20, // right or bottom

    e_amMaskAlign       = 0x0f,
    e_amLeft            = 0x00 | e_amMaskDirH,  // 0
    e_amTop             = 0x00 | e_amMaskDirV,  // 1
    e_amRight           = 0x02 | e_amMaskDirH,  // 2
    e_amBottom          = 0x02 | e_amMaskDirV,  // 3

    e_amMaskLR          = e_amLeft  | e_amRight,
    e_amMaskBT          = e_amTop   | e_amBottom,

    e_amLeftTop         = e_amMode2 | e_amLeft,
    e_amLeftBottom      = e_amMode3 | e_amLeft,
    e_amRightTop        = e_amMode2 | e_amRight,
    e_amRightBottom     = e_amMode3 | e_amRight,
    e_amTopLeft         = e_amMode2 | e_amTop,
    e_amTopRight        = e_amMode3 | e_amTop,
    e_amBottomLeft      = e_amMode2 | e_amBottom,
    e_amBottomRight     = e_amMode3 | e_amBottom,

    e_amL2RReading      = 0x100,
    e_amFrame           = 0x200,
    e_amMaskAlignMode   = e_amMaskMode | e_amMaskAlign,
};

template < class T >
class CTuiButtonT : public T
{
public:
    typedef CTuiButtonT                                 TBase;

public:
    CTuiButtonT(HTUI hWnd = 0) : T(hWnd) {}

    int GetCheck() const
    {
        return (int)this->SendMessage(CBM_GETCHECK, 0, 0L);
    }

    void SetCheck(int nCheck)
    {
        this->SendMessage(CBM_SETCHECK, nCheck, 0L);
    }

    int GetChecks()
    {
        return this->SendMessage(CBM_GETBUTTONSTATECOUNT, 0, 0);
    }

    void SetAlignMode(EAlignMode eMode)
    {
        this->SendMessage(CBM_SETALIGNMODE, eMode, 0L);
    }

    EAlignMode GetAlignMode()
    {
        return (EAlignMode)this->SendMessage(CBM_GETALIGNMODE, 0, 0);
    }

    int GetButtonStateNumber()
    {
        return GetChecks();
    }

    EButState GetButtonState()
    {
        EButStates states = this->GetState();
        return ButStates2ButFrames(states);
//         return (EButState)this->SendMessage(CBM_GETBUTTONSTATE, 0, 0);
    }


    void Click()
    {
        this->SendMessage(CBM_CLICK, 0, 0L);
    }

    TMBool SetForePic(int nID, TMHModule hModule = 0)
    {
        return this->SendMessage(CBM_SETFOREPIC, nID, (TMLParam)hModule);
    }

    TMBool SetForePic(ITuiPic* pPic)
    {
        return this->SendMessage(CBM_SETFOREPIC, 0, (TMLParam)pPic);
    }

    ITuiPic* GetForePic()
    {
        return (ITuiPic*)this->SendMessage(CBM_GETFOREPIC, 0, 0);
    }

    void SetForePicSize(int nWidth, int nHeight)
    {
        this->SendMessage(CBM_SETFOREPICWIDTH, nWidth, nHeight);
    }

    TMBool SetHotKey(const TUIHOTKEY& hotkey)
    {
        return this->SendMessage(CM_SETHOTKEY, 0, (TMLParam)&hotkey);
    }

    TMBool GetHotKey(TUIHOTKEY& hotkey)
    {
        return this->SendMessage(CM_GETHOTKEY, 0, (TMLParam)&hotkey);
    }
};

typedef CTuiButtonT < CTuiWidget >                      CTuiButton;


////////////////////////////////////////////////////////////////////////////////
// class CTuiPushButton
template < class T >
class CTuiPushButtonT : public CTuiButtonT < T >
{
public:
    typedef CTuiButtonT<T>                              TBase;
    CLASS_IMPL(CTuiPushButtonT, TUI_CLASS_PUSHBUTTON);
};

typedef CTuiPushButtonT < CTuiWidget >                  CTuiPushButton;


////////////////////////////////////////////////////////////////////////////////
// class CTuiCheckBox
template < class T >
class CTuiCheckBoxT : public CTuiButtonT < T >
{
public:
    typedef CTuiButtonT<T>                              TBase;
    CLASS_IMPL(CTuiCheckBoxT, TUI_CLASS_CHECKBOX);
};

typedef CTuiCheckBoxT < CTuiWidget >                    CTuiCheckBox;


////////////////////////////////////////////////////////////////////////////////
// class CTuiRadioButton
template < class T >
class CTuiRadioButtonT : public CTuiButtonT < T >
{
public:
    typedef CTuiButtonT<T>                              TBase;
    CLASS_IMPL(CTuiRadioButtonT, TUI_CLASS_RADIOBUTTON);

    CTuiRadioButtonT GetCheckedButton()
    {
        return (HTUI)this->SendMessage(CBM_GETCHECKEDBUTTON, 0, 0L);
    }

    CTuiRadioButtonT GetNextButton()
    {
        return (HTUI)this->SendMessage(CBM_GETNEXTBUTTON, 0, 0L);
    }

    CTuiRadioButtonT GetPrevButton()
    {
        return (HTUI)this->SendMessage(CBM_GETPREVBUTTON, 0, 0L);
    }
};

typedef CTuiRadioButtonT < CTuiWidget >                 CTuiRadioButton;


////////////////////////////////////////////////////////////////////////////////
// class CTuiTabButton
template < class T >
class CTuiTabButtonT : public CTuiRadioButtonT < T >
{
public:
    typedef CTuiRadioButtonT<T>                         TBase;
    CLASS_IMPL(CTuiTabButtonT, TUI_CLASS_TABBUTTON);

public:
    HTUI GetTarget ()
    {
        return (HTUI)this->SendMessage(CM_GETBUDDY, 0, 0L);
    }
};

typedef CTuiTabButtonT < CTuiWidget >                   CTuiTabButton;


//////////////////////////////////////////////////////////////////////////
//class CTuiHeader

// #define LVS_ICON                0x0000
// #define LVS_REPORT              0x0001
// #define LVS_SMALLICON           0x0002
// #define LVS_LIST                0x0003
// #define LVS_TYPEMASK            0x0003
// #define LVS_SINGLESEL           0x0004
// #define LVS_SHOWSELALWAYS       0x0008
// #define LVS_SORTASCENDING       0x0010
// #define LVS_SORTDESCENDING      0x0020
// #define LVS_SHAREIMAGELISTS     0x0040
// #define LVS_NOLABELWRAP         0x0080
// #define LVS_AUTOARRANGE         0x0100
// #define LVS_EDITLABELS          0x0200
// #if (_WIN32_IE >= 0x0300)
// #define LVS_OWNERDATA           0x1000
// #endif
// #define LVS_NOSCROLL            0x2000
// 
// #define LVS_TYPESTYLEMASK       0xfc00
// 
// #define LVS_ALIGNTOP            0x0000
// #define LVS_ALIGNLEFT           0x0800
// #define LVS_ALIGNMASK           0x0c00
// 
// #define LVS_OWNERDRAWFIXED      0x0400
// #define LVS_NOCOLUMNHEADER      0x4000
// #define LVS_NOSORTHEADER        0x8000

 
// #define LVS_EX_GRIDLINES        0x00000001
// #define LVS_EX_SUBITEMIMAGES    0x00000002
// #define LVS_EX_CHECKBOXES       0x00000004
// #define LVS_EX_TRACKSELECT      0x00000008
// #define LVS_EX_HEADERDRAGDROP   0x00000010
// #define LVS_EX_FULLROWSELECT    0x00000020 // applies to report mode only
// #define LVS_EX_ONECLICKACTIVATE 0x00000040
// #define LVS_EX_TWOCLICKACTIVATE 0x00000080
// #if (_WIN32_IE >= 0x0400)
// #define LVS_EX_FLATSB           0x00000100
// #define LVS_EX_REGIONAL         0x00000200
// #define LVS_EX_INFOTIP          0x00000400 // listview does InfoTips for you
// #define LVS_EX_UNDERLINEHOT     0x00000800
// #define LVS_EX_UNDERLINECOLD    0x00001000
// #define LVS_EX_MULTIWORKAREAS   0x00002000
// #endif



// style
#ifndef HDS_BUTTONS

    struct HDITEM
    {
        TMUInt      mask;
        int         cxy;
        TMCStr      pszText;
        void*       hbm;
        int         cchTextMax;
        int         fmt;
        TMLParam    lParam;
        int         iImage;        // index of bitmap in ImageList
        int         iOrder;        // where to draw this item
    };

// #   define HDS_HORZ                     0x0000
#   define HDS_BUTTONS                  0x0002
// #   define HDS_HOTTRACK                 0x0004
// #   define HDS_HIDDEN                   0x0008

#   define HDS_DRAGDROP                 0x0040
// #   define HDS_FULLDRAG                 0x0080
// #   define HDS_FILTERBAR                0x0100
#endif
// #ifndef HDS_FLAT
// #   define HDS_FLAT                     0x0200
// #endif
#ifndef HDS_NOSIZING
// #   define HDS_CHECKBOXES               0x0400
#   define HDS_NOSIZING                 0x0800
// #   define HDS_OVERFLOW                 0x1000
#endif

// mask
#ifndef HDI_WIDTH
#   define HDI_WIDTH                    0x0001
//#   define HDI_HEIGHT              HDI_WIDTH
#   define HDI_TEXT                     0x0002
#   define HDI_FORMAT                   0x0004
#   define HDI_LPARAM                   0x0008
// #define HDI_BITMAP              0x0010
// #define HDI_IMAGE               0x0020
// #define HDI_DI_SETITEM          0x0040
// #define HDI_ORDER               0x0080
#endif

// format
#ifndef HDF_LEFT
#   define HDF_LEFT                     0x0000 // Same as LVCFMT_LEFT
#   define HDF_RIGHT                    0x0001 // Same as LVCFMT_RIGHT
#   define HDF_CENTER                   0x0002 // Same as LVCFMT_CENTER
#   define HDF_JUSTIFYMASK              0x0003 // Same as LVCFMT_JUSTIFYMASK
// #   define HDF_RTLREADING               0x0004 // Same as LVCFMT_LEFT
// #   define HDF_BITMAP                   0x2000
// #   define HDF_STRING                   0x4000
// #   define HDF_OWNERDRAW                0x8000 // Same as LVCFMT_COL_HAS_IMAGES
// #   define HDF_IMAGE                    0x0800 // Same as LVCFMT_IMAGE
#   define HDF_BITMAP_ON_RIGHT          0x1000 // Same as LVCFMT_BITMAP_ON_RIGHT
#endif
#ifndef HDF_SORTUP
#   define HDF_SORTUP                   0x0400
#   define HDF_SORTDOWN                 0x0200
#endif
#ifndef HDF_FIXEDWIDTH
// #   define HDF_CHECKBOX                 0x0040
// #   define HDF_CHECKED                  0x0080
#   define HDF_FIXEDWIDTH               0x0100 // Can't resize the column; same as LVCFMT_FIXED_WIDTH
// #   define HDF_SPLITBUTTON              0x1000000 // Column is a split button; same as LVCFMT_SPLITBUTTON
#endif
#   define HDF_AUTOSORTUP               0x0010
#   define HDF_AUTOSORTDOWN             0x0020
#   define HDF_CURRENTSEL               0x0040


// hit test
#ifndef HHT_NOWHERE
#   define HHT_NOWHERE                  0x0001
#   define HHT_ONHEADER                 0x0002
#   define HHT_ONDIVIDER                0x0004
#   define HHT_ONDIVOPEN                0x0008
#   define HHT_ABOVE                    0x0100
#   define HHT_BELOW                    0x0200
#   define HHT_TORIGHT                  0x0400
#   define HHT_TOLEFT                   0x0800
#endif

// message
#ifndef HDM_FIRST
#   define HDM_FIRST                    0x1200      // Header messages
#   define HDM_GETITEMCOUNT             (HDM_FIRST + 0)
#   define HDM_DELETEITEM               (HDM_FIRST + 2)
#   define HDM_GETITEMRECT              (HDM_FIRST + 7)
#   define HDM_INSERTITEM               (HDM_FIRST + 10)
#   define HDM_GETITEM                  (HDM_FIRST + 11)
#   define HDM_SETITEM                  (HDM_FIRST + 12)
#endif
#ifndef HDM_SETBITMAPMARGIN
#   define HDM_SETBITMAPMARGIN          (HDM_FIRST + 20)
#   define HDM_GETBITMAPMARGIN          (HDM_FIRST + 21)
// #   define HDM_SETFILTERCHANGETIMEOUT   (HDM_FIRST + 22)
// #   define HDM_EDITFILTER               (HDM_FIRST + 23)
#endif

// notify in CM_NOTIFY
#ifndef HDN_FIRST
#   define HDN_FIRST                    (0U-300U)       // header
#   define HDN_LAST                     (0U-399U)
#   define HDN_ITEMCHANGING             (HDN_FIRST-20)  // lp1:iItem; lp2:HDITEM*; return ture: stop change;
#   define HDN_ITEMCHANGED              (HDN_FIRST-21)  // lp1:iItem; lp2:HDITEM*;
#   define HDN_ITEMCLICK                (HDN_FIRST-22)  // lp1:iItem; lp2:new fmt if HDF_AUTOSORTXXX; return ture: stop sort;
#   define HDN_ITEMDBLCLICK             (HDN_FIRST-23)  // lp1:iItem;
#   define HDN_DIVIDERDBLCLICK          (HDN_FIRST-25)  // lp1:iItem;
#   define HDN_BEGINTRACK               (HDN_FIRST-26)  // lp1:iItem; lp2:width; return ture: stop track;
#   define HDN_ENDTRACK                 (HDN_FIRST-27)  // lp1:iItem; lp2:width;
#   define HDN_TRACK                    (HDN_FIRST-28)  // lp1:iItem; lp2:width; return ture: stop track;
#   define HDN_BEGINDRAG                (HDN_FIRST-10)  // lp1:iItem; return ture: stop drag;
#   define HDN_ENDDRAG                  (HDN_FIRST-11)  // lp1:iItem; lp2:iDivider; return ture: stop move item;
#endif // HDN_FIRST

// TUIHITINFO_HEADER
struct HITCODE_HEADER
{
    union
    {
        struct
        {
            TMUInt16    flags;
            TMInt16     iItem;
        };
        TMUInt32        dwCode;
    };
    
    HITCODE_HEADER(TMUInt32 u) : dwCode(u) {}
    operator TMUInt32() const { return dwCode; }
};


template <class TBase>
class CTuiHeaderT : public TBase
{
public:
    CLASS_IMPL(CTuiHeaderT, TUI_CLASS_HEADER);

    int GetItemCount() const
    {
        return (int)this->SendMessage(HDM_GETITEMCOUNT, 0, 0L);
    }

    TMBool GetItem(int nIndex, HDITEM* pHeaderItem) const
    {
        return (TMBool)this->SendMessage(HDM_GETITEM, nIndex, (TMLParam)pHeaderItem);
    }

    TMLParam GetItemData(int nIndex) const
    {
        HDITEM hi = { HDI_LPARAM };
        GetItem(nIndex, &hi);
        return hi.lParam;
    }

    TMBool SetItem(int nIndex, const HDITEM* pHeaderItem)
    {
        return (TMBool)this->SendMessage(HDM_SETITEM, nIndex, (TMLParam)pHeaderItem);
    }

    TMBool SetItemText(int nIndex, TMCStr text) const
    {
        HDITEM hi = { HDI_TEXT };
        hi.pszText = text;
        return SetItem(nIndex, &hi);
    }

//  CImageList GetImageList() const
//  {
//      DbgAssert(IsWindow());
//      return CImageList((HIMAGELIST)::SendMessage(m_hWnd, HDM_GETIMAGELIST, 0, 0L));
//  }
//
//  CImageList SetImageList(HIMAGELIST hImageList)
//  {
//      DbgAssert(IsWindow());
//      return CImageList((HIMAGELIST)::SendMessage(m_hWnd, HDM_SETIMAGELIST, 0, (TMLParam)hImageList));
//  }
//
//     TMBool GetOrderArray(int nSize, int* lpnArray) const
//     {
//         return (TMBool)::SendMessage(m_hWnd, HDM_GETORDERARRAY, nSize, (TMLParam)lpnArray);
//     }
//
//     TMBool SetOrderArray(int nSize, int* lpnArray)
//     {
//         DbgAssert(IsWindow());
//         return (TMBool)::SendMessage(m_hWnd, HDM_SETORDERARRAY, nSize, (TMLParam)lpnArray);
//     }

    TMBool GetItemRect(int nIndex, TUIRect* lpItemRect) const
    {
        return (TMBool)this->SendMessage(HDM_GETITEMRECT, nIndex, (TMLParam)lpItemRect);
    }

//     int SetHotDivider(TMBool bPos, TMUInt32 dwInputValue)
//     {
//         DbgAssert(IsWindow());
//         return (int)::SendMessage(m_hWnd, HDM_SETHOTDIVIDER, bPos, dwInputValue);
//     }
//
//     TMBool GetUnicodeFormat() const
//     {
//         DbgAssert(IsWindow());
//         return (TMBool)::SendMessage(m_hWnd, HDM_GETUNICODEFORMAT, 0, 0L);
//     }
//
//     TMBool SetUnicodeFormat(TMBool bUnicode = TRUE)
//     {
//         DbgAssert(IsWindow());
//         return (TMBool)::SendMessage(m_hWnd, HDM_SETUNICODEFORMAT, bUnicode, 0L);
//     }

    int GetBitmapMargin() const
    {
        return (int)this->SendMessage(HDM_GETBITMAPMARGIN, 0, 0L);
    }

    int SetBitmapMargin(int nWidth)
    {
        return (int)this->SendMessage(HDM_SETBITMAPMARGIN, nWidth, 0L);
    }

//     int SetFilterChangeTimeout(TMUInt32 dwTimeOut)
//     {
//         DbgAssert(IsWindow());
//         return (int)::SendMessage(m_hWnd, HDM_SETFILTERCHANGETIMEOUT, 0, dwTimeOut);
//     }
//
// // Operations
    int InsertItem(int nIndex, const HDITEM* phdi)
    {
        return (int)this->SendMessage(HDM_INSERTITEM, nIndex, (TMLParam)phdi);
    }

    TMBool DeleteItem(int nIndex)
    {
        return (TMBool)this->SendMessage(HDM_DELETEITEM, nIndex, 0L);
    }

//     TMBool Layout(HD_LAYOUT* pHeaderLayout)
//     {
//         DbgAssert(IsWindow());
//         return (TMBool)::SendMessage(m_hWnd, HDM_LAYOUT, 0, (TMLParam)pHeaderLayout);
//     }
//
//     int HitTest(LPHDHITTESTINFO lpHitTestInfo) const
//     {
//         DbgAssert(IsWindow());
//         return (int)::SendMessage(m_hWnd, HDM_HITTEST, 0, (TMLParam)lpHitTestInfo);
//     }
//
//     int OrderToIndex(int nOrder)
//     {
//         DbgAssert(IsWindow());
//         return (int)::SendMessage(m_hWnd, HDM_ORDERTOINDEX, nOrder, 0L);
//     }
//
//     int EditFilter(int nColumn, TMBool bDiscardChanges)
//     {
//         DbgAssert(IsWindow());
//         return (int)::SendMessage(m_hWnd, HDM_EDITFILTER, nColumn, MAKELPARAM(bDiscardChanges, 0));
//     }
//
//     int ClearFilter(int nColumn)
//     {
//         DbgAssert(IsWindow());
//         return (int)::SendMessage(m_hWnd, HDM_CLEARFILTER, nColumn, 0L);
//     }
//
//     int ClearAllFilters()
//     {
//         DbgAssert(IsWindow());
//         return (int)::SendMessage(m_hWnd, HDM_CLEARFILTER, (TMWParam)-1, 0L);
//     }

    int SetItemAccText(int nIndex, const CStrPtr& strOrKey, int idRes = 0, TMHModule hModule = 0)
    {
        DbgProtect(!this->m_hWnd, false);
        return this->_SendSetTextMessage(CM_SETITEMACCTEXT, nIndex, false, &strOrKey, idRes, hModule);
    }
    
    CStrPtr GetItemAccText(int nIndex)
    {
        CStrPtr ptr;
        DbgProtect(!this->m_hWnd, ptr);
        this->SendMessage(CM_GETITEMACCTEXT, nIndex, (TMLParam)&ptr);
        return std::move(ptr);
    }
    
    int SetItemHint(int nIndex, const CStrPtr& strOrKey, int idRes = 0, TMHModule hModule = 0)
    {
        return this->_SendSetTextMessage(CM_SETITEMHINT, nIndex, false, &strOrKey, idRes, hModule);
    }
    
    int GetItemHint(int nIndex, CStrPtr& v) const
    {
        return this->SendMessage(CM_GETITEMHINT, nIndex, (TMLParam)&v);
    }
    
    CStrPtr GetItemHint(int nIndex)
    {
        CStrPtr ptr;
        DbgProtect(!this->m_hWnd, ptr);
        GetItemHint(nIndex, ptr);
        return std::move(ptr);
    }
};

typedef CTuiHeaderT<CTuiWidget>                   CTuiHeader;



////////////////////////////////////////////////////////////////////////////////
// ComboBox::CTuiComboUnit
// this class can use any combo unit window: listbox, tree, combo button, edit, static.
template < class T >
class CTuiComboUnitT : public T
{
public:
    typedef T   TBase;

    CTuiComboUnitT (HTUI hWnd = NULL)
        : TBase (hWnd)
    {
    }

    HTUI GetPopList() const
    {
        return (HTUI)this->SendMessage(CCB_GETPOPLIST, 0, 0L);
    }

    HTUI GetButton() const
    {
        return (HTUI)this->SendMessage(CCB_GETBUTTON, 0, 0L);
    }

    HTUI GetEdit() const
    {
        return (HTUI)this->SendMessage(CCB_GETEDIT, 0, 0L);
    }

    HTUI GetPopDlg() const
    {
        return (HTUI)this->SendMessage(CCB_GETPOPDLG, 0, 0L);
    }

    TMBool IsDropDown()
    {
        return (TMBool)this->SendMessage(CCB_ISDROPDOWN, 0, 0L);
    }
};

typedef CTuiComboUnitT<CTuiWidget> CTuiComboUnit;


////////////////////////////////////////////////////////////////////////////////
// ComboBox::CTuiComboButtonT
// notify messages:
// when before show drop down list:
//  CBN_DROPDOWN
// when user click list or press return key:
//  BN_CLICKED -> CBN_SELCHANGE-> CBN_SELENDOK-> CBN_CLOSEUP;
// when user cancel list:
//  CBN_SELENDCANCEL -> CBN_CLOSEUP;
// list not pop: when user press up/down key, or mouse wheel on edit
//  CBN_SELCHANGE -> CBN_SELENDOK
// list pop: when user press up/down key
//  CBN_SELCHANGE
template < class T >
class CTuiComboButtonT : public CTuiPushButtonT<T>
{
public:
    typedef CTuiPushButtonT<T>                         TBase;

    CLASS_IMPL(CTuiComboButtonT, TUI_CLASS_COMBOBUTTON);

    // for drop-down combo boxes
    void ShowDropDown(TMBool bShow = TMTrue) // show or hide.
    {
        this->SendMessage(CCB_SHOWDROPDOWN, bShow);
    }

    TMBool Submit()     // submit if pop
    {
        return this->SendMessage(CCB_SUBMIT);
    }

    TMBool SetEditSel() // submit not send CBN_SELENDOK, need set listbox sel first.
    {
        return this->SendMessage(CCB_SETEDITSEL);
    }

    TMBool SetListItems(TMCStr pStrs)
    {
        return this->SendMessage(CCB_SETLISTITEMS, 0, (TMLParam)pStrs);
    }

    TMBool SetReferID(int rid)
    {
        return this->SendMessage(CCB_SETREFERID, 0, rid);
    }
};

typedef CTuiComboButtonT<CTuiComboUnit>           CTuiComboButton;


////////////////////////////////////////////////////////////////////////////////
// class CTuiSys
template < class T >
class CTuiSysT : public CTuiButtonT < T >
{
public:
    typedef CTuiButtonT<T>                              TBase;
    CLASS_IMPL(CTuiSysT, TUI_CLASS_SYS);
};

typedef CTuiSysT<CTuiWidget>                            CTuiSys;

////////////////////////////////////////////////////////////////////////////////
// class CTuiGroupBox
template < class TBase >
class CTuiGroupBoxT : public TBase
{
public:
    CLASS_IMPL(CTuiGroupBoxT, TUI_CLASS_GROUPBOX);
};

typedef CTuiGroupBoxT < CTuiWidget >                    CTuiGroupBox;


//////////////////////////////////////////////////////////////////////////
//class CTuiMenuItem
template < class T >
class CTuiMenuItemT : public CTuiButtonT < T >
{
public:
    typedef CTuiButtonT<T>                         TBase;
    CLASS_IMPL(CTuiMenuItemT, TUI_CLASS_MENUITEM);
};

typedef CTuiMenuItemT<CTuiWidget>                           CTuiMenuItem;



////////////////////////////////////////////////////////////////////////////////
// class CTuiEdit
//   notify message: EN_SELCHANGE, EN_CHANGE, EN_CHANGEEX, BN_CLICKED(link)
//
// COMMAND_HANDLER     (IDC_XXX, EN_CHANGE,  OnChange)
//                     memo                edit
// SetWindowText        n/a             EN_CHANGE
// input char       EN_CHANGE/EX        EN_CHANGE/EX
// paste/cut/undo   EN_CHANGE/EX        EN_CHANGE/EX
// wnd.XXX          EN_CHANGE/EX        EN_CHANGE/EX
#define EN_CHANGEEX           CN_VALUECHANGE


enum { ES_WORD_ELLIPSIS     = 0x0004L }; // ES_MULTILINE, don't set LAYOUT_HSCROLL and LAYOUT_AUTOHSCROLL
enum { ES_NOCARET           = 0x0200L };
enum { ES_NOPOPUPMENU       = 0x4000L };
enum { ES_NOAUTOSELECT      = 0x8000L };
enum { ES_WANTTAB           = 0x0040L }; // ES_AUTOVSCROLL
enum { ES_FOCUSPROMPT       = 0x0080L }; // ES_AUTOHSCROLL
enum { ES_NOLINKUNDERLINE   = 0x0020L }; // ES_PASSWORD
enum { ES_L2RREADING        = 0x0400L }; // ES_OEMCONVERT
enum { ES_HYPERLINK         = ES_READONLY | ES_NOAUTOSELECT | ES_NOCARET }; // ES_WANTTAB | ES_WANTRETURN |

enum ETextUnit
{
    eTextUnitChar           = 0x0000,
    eTextUnitFormat         = 0x0001,
    eTextUnitWord           = 0x0002,
    eTextUnitSentence       = 0x0003,
    eTextUnitLine           = 0x0004,
    eTextUnitParagraph      = 0x0005,
    eTextUnitPage           = 0x0006,
    eTextUnitDocument       = 0x0007,
    eTextUnitMaskType       = 0x000f,

    eTextUnitLeft           = 0x1000,   // enable TMSelRange::left
    eTextUnitRight          = 0x2000,   // enable TMSelRange::right
};
TM_ENUM_2_FLAG(ETextUnit);


template <class TBase>
class CTuiEditBaseT : public TBase
{
public:
// Constructors
    CTuiEditBaseT(HTUI hWnd = NULL) : TBase(hWnd)
    {
    }

    TMBool CanUndo() const
    {
        return (TMBool)this->SendMessage(CEM_CANUNDO, 0, 0L);
    }

    int GetLineCount() const
    {
        return (int)this->SendMessage(CEM_GETLINECOUNT, 0, 0L);
    }

    TMBool GetModify() const
    {
        return (TMBool)this->SendMessage(CEM_GETMODIFY, 0, 0L);
    }

    void SetModify(TMBool bModified = TMTrue)
    {
        this->SendMessage(CEM_SETMODIFY, bModified, 0L);
    }

//     void GetRect(TMRect* lpRect) const
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CEM_GETRECT, 0, (TMLParam)lpRect);
//     }

    TMULong GetSel() const
    {
        return (TMULong)this->SendMessage(CEM_GETSEL, 0, 0L);
    }

    void GetSel(TMSel0& nStartChar, TMSel0& nEndChar) const
    {
        this->SendMessage(CEM_GETSEL, (TMWParam)&nStartChar, (TMLParam)&nEndChar);
    }

//     TMUInt GetMargins() const
//     {
//         DbgAssert(IsWindow());
//         return (TMUInt)SendMessage(CEM_GETMARGINS, 0, 0L);
//     }
//
//     void SetMargins(TMUInt nLeft, TMUInt nRight)
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CEM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, MAKELONG(nLeft, nRight));
//     }

    TMUInt GetLimitText() const
    {
        return (TMUInt)this->SendMessage(CEM_GETLIMITTEXT, 0, 0L);
    }

    void SetLimitText(TMUInt nMax)
    {
        this->SendMessage(CEM_LIMITTEXT, nMax, 0L);
    }

    TMPointL PosFromChar(TMSel2 nChar) const
    {
        DbgAssert(SelIs2(nChar));
        TMPointL pt = {0, 0};
        this->SendMessage(CEM_POSFROMCHAR, (TMWParam)&pt, nChar);
        return pt;
    }

    TMSel0 CharFromPos(TMPointL pt, int* pLine = NULL) const
    {
        TMUInt dwRet = (TMUInt)this->SendMessage(CEM_CHARFROMPOS, 0, MAKELPARAM(PointX(pt), PointY(pt)));
        if (pLine != NULL)
            *pLine = (int)(short)HIWORD(dwRet);
        return (TMSel0)(short)LOWORD(dwRet);
    }

    int GetLine(int nIndex, TMStr lpszBuffer, TMUInt16 nMaxLength) const
    {
        DbgAssert(SelIs2(nIndex));
        DbgAssert(lpszBuffer);
        *(TMUInt16*)lpszBuffer = nMaxLength;
        return (int)this->SendMessage(CEM_GETLINE, nIndex, (TMLParam)lpszBuffer);
    }

    TMCharT GetPasswordChar() const
    {
        return (TMCharT)this->SendMessage(CEM_GETPASSWORDCHAR, 0, 0L);
    }

    void SetPasswordChar(TMCharT ch)
    {
        this->SendMessage(CEM_SETPASSWORDCHAR, ch, 0L);
    }

    int GetFirstVisibleLine() const
    {
        return (int)this->SendMessage(CEM_GETFIRSTVISIBLELINE, 0, 0L);
    }

    TMBool SetReadOnly(TMBool bReadOnly = TMTrue)
    {
        return (TMBool)this->SendMessage(CEM_SETREADONLY, bReadOnly, 0L);
    }

// Operations
    void EmptyUndoBuffer()
    {
        this->SendMessage(CEM_EMPTYUNDOBUFFER, 0, 0L);
    }

//     TMBool FmtLines(TMBool bAddEOL)
//     {
//         DbgAssert(IsWindow());
//         return (TMBool)SendMessage(CEM_FMTLINES, bAddEOL, 0L);
//     }

    void LimitText(int nChars = 0)
    {
        this->SendMessage(CEM_LIMITTEXT, nChars, 0L);
    }

    int LineFromChar(TMSel2 nIndex = eSelEnd) const
    {
        DbgAssert(SelIs2(nIndex));
        return (int)this->SendMessage(CEM_LINEFROMCHAR, nIndex, 0L);
    }

    TMSel0 LineIndex(int nLine = -1) const
    {
        return (TMSel0)this->SendMessage(CEM_LINEINDEX, nLine, 0L);
    }

    int LineLength(int nLine = -1) const
    {
        return (int)this->SendMessage(CEM_LINELENGTH, nLine, 0L);
    }

//     void LineScroll(int nLines, int nChars = 0)
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CEM_LINESCROLL, nChars, nLines);
//     }

    void ReplaceSel(TMCStr lpszNewText, TMBool bCanUndo = TMFalse)
    {
        this->SendMessage(CEM_REPLACESEL, (TMWParam)bCanUndo, (TMLParam)lpszNewText);
    }

//     void SetRect(const TMRect* lpRect)
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CEM_SETRECT, 0, (TMLParam)lpRect);
//     }

//     void SetRectNP(const TMRect* lpRect)
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CEM_SETRECTNP, 0, (TMLParam)lpRect);
//     }

//     void SetSel(TMUInt dwSelection, TMBool bNoScroll = TMFalse)
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CEM_SETSEL, LOWORD(dwSelection), HIWORD(dwSelection));
// //      if (!bNoScroll)
// //          SendMessage(EM_SCROLLCARET, 0, 0L);
//     }

    void SetSel(TMSel2 nStartChar, TMSel2 nEndChar, TMBool bNoScroll = TMFalse)
    {
        DbgAssert(SelIs2(nStartChar));
        DbgAssert(SelIs2(nEndChar));
        this->SendMessage(CEM_SETSEL, nStartChar, nEndChar);
//      if (!bNoScroll)
//          SendMessage(EM_SCROLLCARET, 0, 0L);
    }

    void SetSelAll(TMBool bNoScroll = TMFalse)
    {
        SetSel(0, eSelEnd, bNoScroll);
    }

    void SetSelNone(TMBool bNoScroll = TMFalse)
    {
        SetSel(eSelEnd, 0, bNoScroll);
    }

    TMBool SetTabStops(int nTabStops, TMInt* rgTabStops)
    {
        return (TMBool)this->SendMessage(CEM_SETTABSTOPS, nTabStops, (TMLParam)rgTabStops);
    }

    TMBool SetTabStops()
    {
        return (TMBool)this->SendMessage(CEM_SETTABSTOPS, 0, 0L);
    }

    TMBool SetTabStops(const int& cxEachStop)    // takes an 'int'
    {
        return (TMBool)this->SendMessage(CEM_SETTABSTOPS, 1, (TMLParam)(TMInt*)&cxEachStop);
    }

//     void ScrollCaret()
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CEM_SCROLLCARET, 0, 0L);
//     }
//
//     int Scroll(int nScrollAction)
//     {
//         DbgAssert(IsWindow());
//         DbgAssert((GetStyle() & ES_MULTILINE) != 0);
//         TMResult lRet = SendMessage(CEM_SCROLL, nScrollAction, 0L);
//         if (!(TMBool)HIWORD(lRet))
//             return -1;   // failed
//         return (int)(short)LOWORD(lRet);
//
//     }

    void InsertText(TMSel2 nInsertAfterChar, TMCStr lpstrText, TMBool bNoScroll = TMFalse, TMBool bCanUndo = TMFalse)
    {
        DbgAssert(SelIs2(nInsertAfterChar));
        SetSel(nInsertAfterChar, nInsertAfterChar, bNoScroll);
        ReplaceSel(lpstrText, bCanUndo);
    }

    void AppendText(TMCStr lpstrText, TMBool bNoScroll = TMFalse, TMBool bCanUndo = TMFalse)
    {
        InsertText(this->GetWindowTextLength(), lpstrText, bNoScroll, bCanUndo);
    }

    void AppendFormat(TMCStr fmt, ...)
    {
        CString str;
        va_list vl;
        va_start(vl,fmt);
        str.FormatV(fmt, vl);
        va_end(vl);
        AppendText(str);
    }

    TMBool SetPrompt(const CStrPtr& strOrKey, int bHtml = TUISETTEXT::eDefault, int idRes = 0, TMHModule hModule = 0)
    {
        return this->_SendSetTextMessage(CEM_SETPROMPT, 0, bHtml, &strOrKey, idRes, hModule);
    }

//     TMBool FormatPrompt(int nID, ...)
//     {
//         va_list va;
//         va_start (va, nID);
//         TMBool b = _SendSetTextMessage(CM_SETPROMPT, false, 0, (TMCStr)nID, &va);
//         va_end (va);
//         return b;
//     }
//
//     TMBool FormatPromptEx(TMBool bHtml, TMHModule hModule, int nID, ...)
//     {
//         va_list va;
//         va_start (va, nID);
//         TMBool b = _SendSetTextMessage(CM_SETPROMPT, bHtml, hModule, (TMCStr)nID, &va);
//         va_end (va);
//         return b;
//     }
//

    // Clipboard operations
    TMBool Undo()
    {
        return (TMBool)this->SendMessage(CEM_UNDO, 0, 0L);
    }

    void Clear()
    {
        this->SendMessage(WM_CLEAR, 0, 0L);
    }

    void Copy()
    {
        this->SendMessage(WM_COPY, 0, 0L);
    }

    void Cut()
    {
        this->SendMessage(WM_CUT, 0, 0L);
    }

    void Paste()
    {
        this->SendMessage(WM_PASTE, 0, 0L);
    }

        // use TMRGB or TMRGBA
//     void SetSelectedTextColor(TMColor clr)
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CM_SETTEXTCOLOR, -1, clr);
//     }
//
//     void SetSelectedTextFont(const GXFont* font)
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CEM_SETSELFONT, (TMWParam)font);
//     }

    void ReplaceSelHtml(TMCStr lpszHTML, TMBool bCanUndo = TMFalse)
    {
        this->SendMessage(CEM_REPLACESELHTML, (TMWParam)bCanUndo, (TMLParam)lpszHTML);
    }

    void GetCharInfo(TMSel2 nIndex, RICHCURSOR& cursor)
    {
        DbgAssert(SelIs2(nIndex));
        this->SendMessage(CEM_GETCHARINFO, (TMWParam)nIndex, (TMLParam)&cursor);
    }

    void GetCharUnit(ETextUnit eUnit, TMSelRange& rg) //rg:in/out
    {
        this->SendMessage(CEM_GETCHARUNIT, (TMWParam)eUnit, (TMLParam)&rg);
    }
};


template < class T >
class CTuiEditT : public CTuiEditBaseT<T>
{
public:
    typedef CTuiEditBaseT<T> TBase;
    CLASS_IMPL(CTuiEditT, TUI_CLASS_EDIT);

};
typedef CTuiEditT<CTuiWidget>                           CTuiEdit;


template < class T >
class CTuiMemoT : public CTuiEditBaseT<T>
{
public:
    typedef CTuiEditBaseT<T> TBase;
    CLASS_IMPL(CTuiMemoT, TUI_CLASS_MEMO);

};
typedef CTuiMemoT<CTuiWidget>                           CTuiMemo;


// not support CSS.
// support tag: a, font, mark, big, small, em, strong, b, u, strike, s, del, i, h, img, br, span, widget, p, div, pre;
// support css: background-color;
// extend support:
//    click <a id="123456">: send command id(123456) + BN_CLICKED
//    click <a>: send command id(rich command id) + BN_CLICKED
//    click <a href="www.aaa.com">: open www.aaa.com in system default web browser.
//    <widget src="objectid">; create a widget in rich.
template < class T >
class CTuiRichT : public CTuiEditBaseT<T>
{
public:
    typedef CTuiEditBaseT<T> TBase;
    CLASS_IMPL(CTuiRichT, TUI_CLASS_RICH);

    // use TMRGB or TMRGBA
//     void SetSelectedTextColor(TMColor clr)
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CM_SETTEXTCOLOR, -1, clr);
//     }
//
//     void SetSelectedTextFont(const GXFont* font)
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CEM_SETSELFONT, (TMWParam)font);
//     }

    void ReplaceSelHtml(TMCStr lpszHTML, TMBool bCanUndo = TMFalse)
    {
        this->SendMessage(CEM_REPLACESELHTML, (TMWParam)bCanUndo, (TMLParam)lpszHTML);
    }

//     void GetSelHtml(CString& str, const TMRANGE* range)
//     {
//         this->SendMessage(CEM_GETSELHTML, (TMWParam)&str, (TMLParam)range);
//     }

    void GetSelHtml(CString& str, TMSel2 nStartChar, TMSel2 nEndChar)
    {
        str = std::move(GetSelHtml(nStartChar, nEndChar));
    }

    CStrPtr GetSelHtml(TMSel2 nStartChar, TMSel2 nEndChar)
    {
        DbgAssert(SelIs2(nStartChar));
        DbgAssert(SelIs2(nEndChar));
        TMSelRange range = { nStartChar, nEndChar };
        return std::move(GetSelHtml(&range));
    }

    void GetSelHtml(CString& str)
    {
        str = std::move(GetSelHtml(0));
    }

    CStrPtr GetSelHtml(TMSelRange* range = 0)
    {
        CStrPtr ptr;
        this->SendMessage(CEM_GETSELHTML, (TMWParam)&ptr, (TMLParam)range);
        return std::move(ptr);
    }

    void AppendHtml(TMCStr lpstrText, TMBool bNoScroll = TMFalse, TMBool bCanUndo = TMFalse)
    {
        int n = this->GetWindowTextLength();
        this->SetSel(n, n, bNoScroll);
        ReplaceSelHtml(lpstrText, bCanUndo);
    }
    
    void AppendHtmlFormat(TMCStr fmt, ...)
    {
        CString str;
        va_list vl;
        va_start(vl,fmt);
        str.FormatV(fmt, vl);
        va_end(vl);
        AppendHtml(str);
    }
};
typedef CTuiRichT<CTuiWidget>                           CTuiRich;


////////////////////////////////////////////////////////////////////////////////
// class CTuiProgressT
template < class TBase >
class CTuiProgressT : public TBase
{
public:
    CLASS_IMPL(CTuiProgressT, TUI_CLASS_PROGRESS);

    CTuiWidget GetBuddy() const
    {
        return (HTUI)this->SendMessage(CM_GETBUDDY, 0, 0L); // discard UDM_GETBUDDY
    }

    TMBool SetBuddy(HTUI hBuddy)
    {
        return this->SendMessage(CM_SETBUDDY, (TMWParam)hBuddy, 0L); // discard UDM_SETBUDDY
    }

    int GetBuddyID() const
    {
        return this->SendMessage(CM_GETBUDDYID, 0, 0L);
    }

    void SetBuddyID(int nID)
    {
        this->SendMessage(CM_SETBUDDYID, nID, 0L);
    }

    TMBool SetForePic (int nID, TMHModule hModule = 0)
    {
        return this->SendMessage(CBM_SETFOREPIC, nID, (TMLParam)hModule);
    }

    TMBool SetForePic (ITuiPic* pPic)
    {
        return this->SendMessage(CBM_SETFOREPIC, 0, (TMLParam)pPic);
    }

    ITuiPic* GetForePic()
    {
        return (ITuiPic*)this->SendMessage(CBM_GETFOREPIC, 0, 0);
    }

    TMBool SetRange(int nLower, int nUpper)
    {
        return this->SendMessage(CM_SETRANGE, (TMWParam)&nLower, (TMLParam)&nUpper); // discard PBM_SETRANGE
    }

    int SetPos(int nPos, TMBool bRedraw = true)
    {
        DbgAssert(this->IsWindow());
        return this->SendMessage(CM_SETPOS, bRedraw, nPos); // discard PBM_SETPOS
    }

    int OffsetPos(int nPos, TMBool bRedraw = true)
    {
        return (int)this->SendMessage(CM_DELTAPOS, bRedraw, nPos); // discard PBM_DELTAPOS
    }

    int SetStep(int nStep)
    {
        return this->SendMessage(CM_SETSTEP, nStep, 0L); // discard PBM_SETSTEP
    }

    int GetPos() const
    {
        return this->SendMessage(CM_GETPOS, 0, 0L); // discard PBM_GETPOS
    }

    void GetRange(int& nMin, int& nMax) const
    {
        this->SendMessage(CM_GETRANGE, (TMWParam)&nMin, (TMLParam)&nMax); // discard TBM_GETRANGEXXX
    }

    void GetRange(TMRANGE* pPBRange) const
    {
        DbgAssert(pPBRange != NULL);
        GetRange(pPBRange->iLow, pPBRange->iHigh); // discard PBM_GETRANGE
    }

    int GetRangeLimit(TMBool bLimit) const
    {
        TMRANGE rg;
        this->GetRange(&rg);
        return bLimit ? rg.iHigh : rg.iLow;
    }

    TMBool SetRange32(int nMin, int nMax)
    {
        return this->SendMessage(CM_SETRANGE, (TMWParam)&nMin, (TMLParam)&nMax); // discard PBM_SETRANGE32
    }

    // Operations
    int StepIt(TMBool bRedraw = true)
    {
        return this->SendMessage(CM_STEPIT, bRedraw, 0L); // discard PBM_STEPIT
    }
};

typedef CTuiProgressT < CTuiWidget >                    CTuiProgress;



///////////////////////////////////////////////////////////////////////////////
// CTuiTrackBarT
// #define  WM_TRACKBAR_CHANGE      WM_USER + 90

template <class TBase>
class CTuiTrackBarT : public TBase
{
public:
    CLASS_IMPL(CTuiTrackBarT, TUI_CLASS_TRACKBAR);

    int GetLineSize() const
    {
        return (int)this->SendMessage(CM_GETSTEP, 0, 0L); // discard TBM_GETLINESIZE
    }

    int SetLineSize(int nSize)
    {
        return (int)this->SendMessage(CM_SETSTEP, 0, nSize); // discard TBM_SETLINESIZE
    }

    int GetRangeMin() const
    {
        int n = 0;
        this->SendMessage(CM_GETRANGE, (TMWParam)&n, 0L); // discard TBM_GETRANGEMIN
        return n;
    }

    void SetRangeMin(int nMin, TMBool bRedraw = TMFalse)
    {
        this->SendMessage(CM_SETRANGE, (TMWParam)&nMin, 0); // discard TBM_SETRANGE
    }

    int GetRangeMax() const
    {
        int n = 0;
        this->SendMessage(CM_GETRANGE, 0, (TMLParam)&n); // discard TBM_GETRANGEMAX
        return n;
    }

    void SetRangeMax(int nMax, TMBool bRedraw = TMFalse)
    {
        this->SendMessage(CM_SETRANGE, 0, (TMLParam)&nMax); // discard TBM_SETRANGEMAX
    }

    void GetRange(int& nMin, int& nMax) const
    {
        this->SendMessage(CM_GETRANGE, (TMWParam)&nMin, (TMLParam)&nMax); // discard TBM_GETRANGEXXX
    }

    void SetRange(int nMin, int nMax, TMBool bRedraw = TMTrue)
    {
        this->SendMessage(CM_SETRANGE, (TMWParam)&nMin, (TMLParam)&nMax); // discard TBM_SETRANGE
    }

    int GetPos() const
    {
        return (int)this->SendMessage(CM_GETPOS, 0, 0L); // discard TBM_GETPOS
    }

    void SetPos(int nPos, TMBool bRedraw = true)
    {
        this->SendMessage(CM_SETPOS, bRedraw, nPos); // discard TBM_SETPOS
    }

    int OffsetPos(int nPos, TMBool bRedraw = true)
    {
        return (int)this->SendMessage(CM_DELTAPOS, bRedraw, nPos); // discard PBM_DELTAPOS
    }

    TMBool SetLeftPic(int nID, TMHModule hModule = 0)
    {
        return this->SendMessage(TBM_SETLEFTPIC, nID, (TMLParam)hModule);
    }

    TMBool SetLeftPic(ITuiPic* pic)
    {
        return this->SendMessage(TBM_SETLEFTPIC, 0, (TMLParam)pic);
    }

    TMBool SetRightPic(int nID, TMHModule hModule = 0)
    {
        return this->SendMessage(TBM_SETRIGHTPIC, nID, (TMLParam)hModule);
    }

    TMBool SetRightPic(ITuiPic* pic)
    {
        return this->SendMessage(TBM_SETRIGHTPIC, 0, (TMLParam)pic);
    }

    TMBool SetTrackBtnPic(int nID, TMHModule hModule = 0)
    {
        return this->SendMessage(TBM_SETTRCKBTNPIC, nID, (TMLParam)hModule);
    }

    TMBool SetTrackBtnPic(ITuiPic* pic)
    {
        return this->SendMessage(TBM_SETTRCKBTNPIC, 0, (TMLParam)pic);
    }

    CTuiWidget GetBuddy() const
    {
        return (HTUI)this->SendMessage(CM_GETBUDDY, 0, 0L);
    }

    void SetBuddy(HTUI hBuddy)
    {
        this->SendMessage(CM_SETBUDDY, (TMWParam)hBuddy, 0L);
    }

    int GetBuddyID() const
    {
        return this->SendMessage(CM_GETBUDDYID, 0, 0L);
    }

    void SetBuddyID(int nID)
    {
        this->SendMessage(CM_SETBUDDYID, nID, 0L);
    }

    TMBool SetFormat(const CStrPtr& lpszString, int bHtml = TUISETTEXT::eDefault)
    {
        return this->_SendSetTextMessage(CM_SETFORMAT, 0, bHtml, &lpszString);
    }

    TMBool SetFormat(int nID, int bHtml = TUISETTEXT::eDefault, TMHModule hModule = 0)
    {
        return this->_SendSetTextMessage(CM_SETFORMAT, 0, bHtml, hModule, nID);
    }

    TMBool SetFormat(int nID, TMHModule hModule)
    {
        return this->_SendSetTextMessage(CM_SETFORMAT, 0, TUISETTEXT::eDefault, hModule, nID);
    }
};

typedef CTuiTrackBarT<CTuiWidget>   CTuiTrackBar;


///////////////////////////////////////////////////////////////////////////////
// CTuiUpDownT
template <class TBase>
class CTuiUpDownT : public TBase
{
public:
     CLASS_IMPL(CTuiUpDownT, TUI_CLASS_UPDOWN);
    // Attributes
//     TMUInt GetAccel(int nAccel, UDACCEL* pAccel) const
//     {
//         DbgAssert(IsWindow());
//         return (TMUInt)LOWORD(SendMessage(UDM_GETACCEL, nAccel, (TMLParam)pAccel));
//     }
//
//     TMBool SetAccel(int nAccel, UDACCEL* pAccel)
//     {
//         DbgAssert(IsWindow());
//         return (TMBool)LOWORD(SendMessage(UDM_SETACCEL, nAccel, (TMLParam)pAccel));
//     }
//
//     TMUInt GetBase() const
//     {
//         DbgAssert(IsWindow());
//         return (TMUInt)LOWORD(SendMessage(UDM_GETBASE, 0, 0L));
//     }
//
//     TMUInt SetBase(TMUInt nBase)
//     {
//         DbgAssert(IsWindow());
//         return (TMUInt)SendMessage(UDM_SETBASE, nBase, 0L);
//     }

    CTuiWidget GetBuddy() const
    {
        return (HTUI)this->SendMessage(CM_GETBUDDY, 0, 0L); // discard UDM_GETBUDDY
    }

    TMBool SetBuddy(HTUI hBuddy)
    {
        return this->SendMessage(CM_SETBUDDY, (TMWParam)hBuddy, 0L); // discard UDM_SETBUDDY
    }

    int GetBuddyID() const
    {
        return this->SendMessage(CM_GETBUDDYID, 0, 0L);
    }

    void SetBuddyID(int nID)
    {
        this->SendMessage(CM_SETBUDDYID, nID, 0L);
    }

    int GetPos(TMBool* lpbError = NULL) const
    {
        return (int)this->SendMessage(CM_GETPOS, 0, (TMLParam)lpbError); // discard UDM_GETPOS
    }

    int SetPos(int nPos, TMBool bRedraw = true)
    {
        return this->SendMessage(CM_SETPOS, bRedraw, nPos);  // discard UDM_SETPOS
    }

    int OffsetPos(int nPos, TMBool bRedraw = true)
    {
        return (int)this->SendMessage(CM_DELTAPOS, bRedraw, nPos); // discard PBM_DELTAPOS
    }

    void SetRange(int nLower, int nUpper)
    {
        this->SendMessage(CM_SETRANGE, (TMWParam)&nLower, (TMLParam)&nUpper);  // discard UDM_SETRANGE
    }

    void GetRange(int& nLower, int& nUpper) const
    {
        this->SendMessage(CM_GETRANGE, (TMWParam)&nLower, (TMLParam)&nUpper); // discard UDM_GETRANGE
    }

};

typedef CTuiUpDownT<CTuiWidget>   CTuiUpDown;


template <class TBase>
class CTuiSplitterT : public TBase
{
public:
    CLASS_IMPL(CTuiSplitterT, TUI_CLASS_SPLITTER);
    // Attributes

    void MoveTo(int nPos) const
    {
        this->SendMessage(CM_SETPOS, 0,  TMLParam(nPos));
    }

    void SetRange(int nMin, int nMax) const
    {
        this->SendMessage(CM_SETRANGE, 0, MAKELPARAM(nMin, nMax));
    }
};

typedef CTuiSplitterT<CTuiWidget>   CTuiSplitter;



////////////////////////////////////////////////////////////////////////////////
// class CTuiScrollBoxT
template < class TBase >
class CTuiScrollBoxT : public TBase
{
public:
    CLASS_IMPL(CTuiScrollBoxT, TUI_CLASS_SCROLLBOX);


};

typedef CTuiScrollBoxT < CTuiWidget >                   CTuiScrollBox;


////////////////////////////////////////////////////////////////////////////////
// class CTuiMenuBarT
template <class TBase>
class CTuiMenuBarT : public TBase
{
public:
    struct ITEMINFO
    {
        TUIRect rc;
        TMCharT text[PATH_MAX];
    };

public:
    CLASS_IMPL(CTuiMenuBarT, TUI_CLASS_MENUBAR);

    TMHMenu GetMenu()
    {
        return (TMHMenu)this->SendMessage(MB_GETMENU, 0, 0);
    }

    void SetMenu(TMHModule hRes, TMCStr pResID)
    {
        this->SendMessage(MB_SETMENU, (TMWParam)hRes, (TMLParam)pResID);
    }

    void SetMenu(TMHModule hRes, int nResID)
    {
        this->SendMessage(MB_SETMENU, (TMWParam)hRes, (TMLParam)nResID);
    }

    void SetMenu(TMHMenu hMenu)
    {
        this->SendMessage(MB_SETHMENU, 0, (TMLParam)hMenu);
    }

    int GetCurSel() const
    {
        return (int)this->SendMessage(MB_GETCURSEL, 0, 0L);
    }

    TMBool GetMenuBarInfo(int nid, ITEMINFO& info) const
    {
        return (TMBool)this->SendMessage(MB_GETMENUINFO, nid, (TMLParam)&info);
    }

    int GetMenuItemCount() const
    {
        return (int)this->SendMessage(MB_GETITEMCOUNT);
    }

    // show menubar, set to ATL mode, focus item on ch; if ch == 0 then focus item to 0;
    TMBool SetAltMode(TMBool bAlt) const
    {
        return (TMBool)this->SendMessage(MB_SETALTMODE, bAlt);
    }
};

typedef CTuiMenuBarT<CTuiWidget>                        CTuiMenuBar;



////////////////////////////////////////////////////////////////////////////////
// class CTuiAnimateT
template <class TBase>
class CTuiAnimateT : public TBase
{
public:
    CLASS_IMPL(CTuiAnimateT, TUI_CLASS_ANIMATE);

    enum ENotifyCode
    {
        CACN_SEEKING    = 0x102,                // set style CACS_NOTIFY
        CACN_START      = ACN_START,
        CACN_STOP       = ACN_STOP,
    };

    // nRepeat:
    //   0: if play end, the frame will not reset to start.
    //  -1: loop play.
    //  other(1, 2, ...): if play end, the frame will reset to start. and repeat property reset to 1.
    void SetRepeat(int nRepeat)
    {
        this->SendMessage(CACM_SETREPEAT, nRepeat, 0);
    }

    TMUInt GetRepeat()
    {
        return this->SendMessage(CACM_GETREPEAT, 0, 0);
    }

    void Play()
    {
        this->SendMessage(CACM_PLAY, 0, 0);
    }

    void Stop()
    {
        this->SendMessage(CACM_STOP, 0, 0);
    }

    int Seek(int nPos)
    {
        return this->SendMessage(CACM_SEEK, nPos, 0L);
    }

    int SeekCur(int ndPos)
    {
        return this->SendMessage(CACM_SEEKCUR, ndPos, 0L);
    }

    TMUInt GetPos() const
    {
        return (TMUInt)this->SendMessage(CACM_GETPOS, 0, 0L);
    }

    void GetRange(int* pBgn, int* pEnd) const
    {
        this->SendMessage(CACM_GETRANGE, (TMWParam)pBgn, (TMLParam)pEnd);
    }

    TMUInt SetRange(int nMin, int nMax)
    {
        return (TMUInt)this->SendMessage(CACM_SETRANGE, nMin, nMax);
    }
};

typedef CTuiAnimateT<CTuiWidget>                        CTuiAnimate;



///////////////////////////////////////////////////////////////////////////////
// CTuiListBox

// styles
#ifndef LBS_SORT
#   define LBS_SORT             0x0002L
#   define LBS_MULTIPLESEL      0x0008L         // use mouse toggle switch multi item
#   define LBS_EXTENDEDSEL      0x0800L         // use shift + mouse select multi item
#   define LBS_NOSEL            0x4000L
// #   define LBS_NOTIFY            0x0001L
// #   define LBS_NOREDRAW          0x0004L
// #   define LBS_OWNERDRAWFIXED    0x0010L
// #   define LBS_OWNERDRAWVARIABLE 0x0020L
// #   define LBS_HASSTRINGS        0x0040L
// #   define LBS_USETABSTOPS       0x0080L
// #   define LBS_NOINTEGRALHEIGHT  0x0100L
// #   define LBS_MULTICOLUMN       0x0200L
// #   define LBS_WANTKEYBOARDINPUT 0x0400L
// #   define LBS_DISABLENOSCROLL   0x1000L
// #   define LBS_NODATA            0x2000L
#endif // #ifndef LBS_SORT

#define LBS_TRACKMOUSE          0x8000L         // show item mouse hot state.
#define LBS_TRACKSEL            0x0004L         // single sel, use in combox.
#define LBS_MENUITEM            0x0010L         // single sel, use in menu.
#define LBS_COLUMNSEL           0x0020L         // send LBN_SELCHANGE when change column. show sel on column, not on line.
#define LBS_SEL_MASK            (LBS_MULTIPLESEL | LBS_EXTENDEDSEL | LBS_NOSEL)

// messages
#ifndef LB_ADDSTRING
#   define LB_ADDSTRING         0x0180
#   define LB_INSERTSTRING      0x0181
#   define LB_DELETESTRING      0x0182
#   define LB_SELITEMRANGEEX    0x0183
#   define LB_RESETCONTENT      0x0184
#   define LB_SETSEL            0x0185
#   define LB_SETCURSEL         0x0186
#   define LB_GETSEL            0x0187
#   define LB_GETCURSEL         0x0188
#   define LB_GETTEXT           0x0189
#   define LB_GETTEXTLEN        0x018A
#   define LB_GETCOUNT          0x018B
#   define LB_SELECTSTRING      0x018C
#   define LB_DIR               0x018D
#   define LB_GETTOPINDEX       0x018E
#   define LB_FINDSTRING        0x018F
#   define LB_GETSELCOUNT       0x0190
#   define LB_GETSELITEMS       0x0191
// #   define LB_SETTABSTOPS          0x0192
// #   define LB_GETHORIZONTALEXTENT  0x0193
// #   define LB_SETHORIZONTALEXTENT  0x0194
// #   define LB_SETCOLUMNWIDTH       0x0195
#   define LB_ADDFILE           0x0196
#   define LB_SETTOPINDEX       0x0197
#   define LB_GETITEMRECT       0x0198
#   define LB_GETITEMDATA       0x0199
#   define LB_SETITEMDATA       0x019A
#   define LB_SELITEMRANGE      0x019B
#   define LB_SETANCHORINDEX    0x019C
#   define LB_GETANCHORINDEX    0x019D
#   define LB_SETCARETINDEX     0x019E
#   define LB_GETCARETINDEX     0x019F
#   define LB_SETITEMHEIGHT     0x01A0
#   define LB_GETITEMHEIGHT     0x01A1
#   define LB_FINDSTRINGEXACT   0x01A2
// #   define LB_SETLOCALE            0x01A5
// #   define LB_GETLOCALE            0x01A6
#   define LB_SETCOUNT          0x01A7
// #   define LB_INITSTORAGE          0x01A8
// #   define LB_ITEMFROMPOINT        0x01A9
#   define LB_MSGMAX            0x01B0
#endif // #ifndef LB_ADDSTRING

#define CLB_SETTEXT             (CM_BASE + 0x260) // wp: index;   lp:TUISETTEXT*
#define CLB_GETTEXT             (CM_BASE + 0x261) // wp: index;   lp:CStrPtr*
#define CLB_ISITEMENABLE        (CM_BASE + 0x262) // wp: index;
#define CLB_ENABLEITEM          (CM_BASE + 0x263) // wp: index;   lp:bShow
#define CLB_BINDWIN             (CM_BASE + 0x264) // wp: index;   lp:hWgt
#define CLB_CREATEBINDWIN       (CM_BASE + 0x265) // wp: index;   lp:hWgt object id
#define CLB_DETACHBINDWIN       (CM_BASE + 0x266) // wp: index;   lp:new parent
#define CLB_GETBINDWIN          (CM_BASE + 0x267) // wp: index;
#define CLB_SORT                (CM_BASE + 0x268) // wp: pfn;     lp:key
#define CLB_ENUM                (CM_BASE + 0x269) // wp: pfn;     lp:LBENUM*; return: item count for enum;
#define CLB_DOWNCOLOR           (CM_BASE + 0x26a) // wp: TMColor;
#define CLB_SETHEADERPROXY      (CM_BASE + 0x26f) // wp: HTUI Header, 0: auto serach first child;   lp:IColumnProxy*
#define CLB_EXPROTCSV           (CM_BASE + 0x270) // wp: CStringA*; lp: IStream*; save const char* to stream;
#define CLB_GETFOREPIC          CBM_GETFOREPIC
#define CLB_SETFOREPIC          CBM_SETFOREPIC


// notify
// COMMAND_CODE_HANDLER    (LBN_SELCHANGE,             OnSelChange)
#ifndef LBN_SELCHANGE
// #   define LBN_ERRSPACE                 (-2)
#   define LBN_SELCHANGE        1
#   define LBN_DBLCLK           2
// #   define LBN_SELCANCEL                3
#   define LBN_SETFOCUS         4
#   define LBN_KILLFOCUS        5
#endif
#   define LBN_SORTED           6

// return values
#ifndef LB_ERR
#   define LB_ERR              (-1)
#endif


// owner draw
// ROUTE_OID_HANDLER       (IDC_LISTBOX_1057, CM_DRAWITEM,  OnDrawItem)
// TMResult CDlgXXX::OnDrawItem(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
// {
//     TUIDRAWITEM* pDrawS = (TUIDRAWITEM*)pMsg->lParam;
//     ...


// return data1 < data2 ? true : false;
typedef bool (CALLBACK *PFNLBCOMPARE)(TMLParam data1, TMLParam data2, TMLParam lParamSort);

struct LBENUM
{
    TMLParam dataUser;
    TMUInt nBgn;
    TMUInt nEnd;
};
typedef bool (CALLBACK *PFNLBENUM)(int nIndex, TMLParam dataItem, TMLParam dataUser); // return true: end enum;


template <class TBase>
class CTuiListBoxT : public TBase
{
public:
    CLASS_IMPL(CTuiListBoxT, TUI_CLASS_LISTBOX);

    void SetPfnAutoDeleteData(PFNDELETEPROC proc)
    {
        this->SendMessage(CM_SETDELETEDATA, 0, (TMLParam)proc);
    }

    void SetHeaderProxy(HTUI header, IColumnProxy* proxy)
    {
        this->SendMessage(CLB_SETHEADERPROXY, (TMWParam)header, (TMLParam)proxy);
    }

    TMBool BindItemWindow(int nIndex, HTUI hWnd)
    {
       DbgAssert(this->IsWindow());
       return this->SendMessage(CLB_BINDWIN, nIndex, (TMLParam)hWnd);
    }

    HTUI CreateBindItemWindow(int nIndex, TMUInt nObjID)
    {
        HTUI hWnd = (HTUI)this->SendMessage(CLB_CREATEBINDWIN, nIndex, nObjID);
        CTuiWidget(hWnd)._Debug_WidgetMustBeNotMgr();
        return hWnd;
    }

    void SetPfnBindBackupRestore(PFNBINDBACKUP proc)
    {
        this->SendMessage(CM_SETPFNBINDBACKUP, 0, (TMLParam)proc);
    }

    HTUI DetachBindItemWindow(int nIndex, HTUI hNewParent)
    {
       DbgAssert(this->IsWindow());
       return (HTUI)this->SendMessage(CLB_DETACHBINDWIN, nIndex, (TMLParam)hNewParent);
    }

    HTUI GetBindItemWindow(int nIndex)
    {
        return (HTUI)this->SendMessage(CLB_GETBINDWIN, nIndex);
    }

    EBindState IsBindChild(int nIndex, HTUI hChild)
    {
        DbgAssert(*this);
        if (!hChild)
            return eBindStateNone;
        HTUI hBind = GetBindItemWindow(nIndex);
        if (!hBind)
            return eBindStateNone;
        if (hBind == hChild)
            return eBindStateSelf;
        return CTuiWidget(hBind).IsChild(hChild) ? eBindStateParent : eBindStateNone;
    }

    // pfn == 0  sort by character
    void Sort(PFNLBCOMPARE pfn, TMLParam lParamSort)
    {
        this->SendMessage(CLB_SORT, (TMWParam)pfn, lParamSort);
    }

    int Enum(PFNLBENUM pfn, TMLParam dataUser, int nBgn = 0, int nEnd = -1)
    {
        LBENUM lbe = { dataUser, nBgn, nEnd };
        return this->SendMessage(CLB_ENUM, (TMWParam)pfn,(TMLParam)&lbe);
    }

    void FreezePanes(int nTop = 0, int nLeft = 0);

    TMBool ExportCSV(IStream* stream, const CStringA& StrSplit = TM_CS(","))
    {
        return this->SendMessage(CLB_EXPROTCSV, (TMWParam)&StrSplit, (TMLParam)stream);
    }

    // for entire listbox
    int GetCount() const
    {
        return (int)this->SendMessage(LB_GETCOUNT, 0, 0L);
    }

    int SetCount(TMUInt cItems)
    {
        return (int)this->SendMessage(LB_SETCOUNT, cItems, 0L);
    }

    int EnableItem(int nIndex, TMBool bEnabled = TMTrue)
    {
        return (int)this->SendMessage(CLB_ENABLEITEM, nIndex, bEnabled);
    }

    TMBool IsItemEnabled(int nIndex)
    {
        return (TMBool)this->SendMessage(CLB_ISITEMENABLE, nIndex);
    }

//     TMBool ShowItem(int nIndex, TMBool bShow)
//     {
//         DbgAssert(IsWindow());
//         return LB_ERR != SendMessage(LB_SHOWITEM, nIndex, bShow);
//     }
//
//     TMBool IsItemVisible(int nIndex)
//     {
//         DbgAssert(IsWindow());
//         return (int)SendMessage(LB_ISITEMVISIBLE, nIndex, 0);
//     }


//     int GetHorizontalExtent() const
//     {
//         DbgAssert(IsWindow());
//         return (int)SendMessage(LB_GETHORIZONTALEXTENT, 0, 0L);
//     }
//
//     void SetHorizontalExtent(int cxExtent)
//     {
//         DbgAssert(IsWindow());
//         SendMessage(LB_SETHORIZONTALEXTENT, cxExtent, 0L);
//     }
    int SetDownColor(TMColor clrDown)
    {
        return (int)this->SendMessage(CLB_DOWNCOLOR, clrDown, 0);
    }

    int GetTopIndex() const
    {
        return (int)this->SendMessage(LB_GETTOPINDEX, 0, 0L);
    }

    int SetTopIndex(int nIndex)
    {
        return (int)this->SendMessage(LB_SETTOPINDEX, nIndex, 0L);
    }

//     ATLocal GetLocale() const
//     {
//         return (ATLocal)this->SendMessage(LB_GETLOCALE, 0, 0L);
//     }
//
//     ATLocal SetLocale(ATLocal nNewLocale)
//     {
//         return (ATLocal)this->SendMessage(LB_SETLOCALE, (TMWParam)nNewLocale, 0L);
//     }
//
// #if (WINVER >= 0x0500) && !defined(_WIN32_WCE)
//  TMUInt GetListBoxInfo() const
//  {
//      DbgAssert(IsWindow());
// #if (_WIN32_WINNT >= 0x0501)
//      return (TMUInt)SendMessage(LB_GETLISTBOXINFO, 0, 0L);
// #else // !(_WIN32_WINNT >= 0x0501)
//      return ::GetListBoxInfo(m_hWnd);
// #endif // !(_WIN32_WINNT >= 0x0501)
//  }
// #endif // (WINVER >= 0x0500) && !defined(_WIN32_WCE)

    // for single-selection listboxes
    int GetCurSel(TMBool bCheckDisable = false) const
    {
//        DbgAssert((GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) == 0);
        return (int)this->SendMessage(LB_GETCURSEL, 0, bCheckDisable);
    }

    int SetCurSel(int nSelect, TMBool bEnsureVisible = true)
    {
//        DbgAssert((GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) == 0);
        return (int)this->SendMessage(LB_SETCURSEL, nSelect, bEnsureVisible);
    }

    // for multiple-selection listboxes
    TMBool GetSel(int nIndex, TMBool bCheckDisable = false) const           // also works for single-selection
    {
        return (TMBool)this->SendMessage(LB_GETSEL, nIndex, 0L);
    }

    int SetSel(int nIndex, TMBool bSelect = TMTrue)
    {
        DbgProtect(!this->m_hWnd, -1);
        DbgAssert((this->GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) != 0);
        return (int)this->SendMessage(LB_SETSEL, bSelect, nIndex);
    }

    int GetSelCount() const
    {
        DbgProtect(!this->m_hWnd, -1);
        DbgAssert((this->GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) != 0);
        return (int)this->SendMessage(LB_GETSELCOUNT, 0, 0L);
    }

    int GetSelItems(int nMaxItems, TMInt* rgIndex) const
    {
        DbgProtect(!this->m_hWnd, -1);
        DbgAssert((this->GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) != 0);
        return (int)this->SendMessage(LB_GETSELITEMS, nMaxItems, (TMLParam)rgIndex);
    }

    int GetAnchorIndex() const
    {
        DbgProtect(!this->m_hWnd, -1);
        DbgAssert((this->GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) != 0);
        return (int)this->SendMessage(LB_GETANCHORINDEX, 0, 0L);
    }

    void SetAnchorIndex(int nIndex)
    {
        DbgProtectVoid(!this->m_hWnd);
        DbgAssert((this->GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) != 0);
        this->SendMessage(LB_SETANCHORINDEX, nIndex, 0L);
    }

    int GetCaretIndex() const
    {
        return (int)this->SendMessage(LB_GETCARETINDEX, 0, 0);
    }

    int SetCaretIndex(int nIndex, TMBool bScroll = TMTrue)
    {
        return (int)this->SendMessage(LB_SETCARETINDEX, nIndex, MAKELONG(bScroll, 0));
    }

    // for listbox items
    TMLParam GetItemData(int nIndex) const
    {
        return (TMLParam)this->SendMessage(LB_GETITEMDATA, nIndex, 0L);
    }

    int SetItemData(int nIndex, TMLParam dwItemData)
    {
        return (int)this->SendMessage(LB_SETITEMDATA, nIndex, (TMLParam)dwItemData);
    }

    void* GetItemDataPtr(int nIndex) const
    {
        return (void*)this->SendMessage(LB_GETITEMDATA, nIndex, 0L);
    }

    int SetItemDataPtr(int nIndex, void* pData)
    {
        return this->SetItemData(nIndex, (TMLParam)pData);
    }

    int GetItemRect(int nIndex, TUIRect* lpRect) const
    {
        return (int)this->SendMessage(LB_GETITEMRECT, nIndex, (TMLParam)lpRect);
    }

    int SetText(int nIndex, const CStrPtr& strOrKey, int bHtml = TUISETTEXT::eDefault, int idRes = 0, TMHModule hModule = 0)
    {
        return (int)this->_SendSetTextMessage(CLB_SETTEXT, nIndex, bHtml, &strOrKey, idRes, hModule);
    }

    int GetText(int nIndex, TMStr lpszBuffer) const
    {
        CStrPtr v;
        GetText(nIndex, v);
        return v.CopyTo(lpszBuffer);
    }

    int GetText(int nIndex, CStrPtr& v) const
    {
        return this->SendMessage(CLB_GETTEXT, nIndex, (TMLParam)&v);
    }

//     int GetText(int nIndex, TM::CString& strText) const
//     {
//         CStrPtr v;
//         int n = GetText(nIndex, v);
//         strText = std::move(v);
//         return n;
//     }


// #ifndef _ATL_NO_COM
// #if defined(_OLEAUTO_H_) && defined(USES_CONVERSION)
//     TMBool GetTextBSTR(int nIndex, BSTR& bstrText) const
//     {
//         USES_CONVERSION;
//         DbgAssert(this->IsWindow());
//         DbgAssert(bstrText == NULL);
// 
//         int nLen = GetTextLen(nIndex);
//         if (nLen == LB_ERR)
//             return TMFalse;
// 
//         TMStr lpszText = (TMStr)_alloca((nLen + 1) * sizeof(TMCharT));
// 
//         if (GetText(nIndex, lpszText) == LB_ERR)
//             return TMFalse;
// 
//         bstrText = ::SysAllocString(T2OLE(lpszText));
//         return (bstrText != NULL) ? TMTrue : TMFalse;
//     }
// #endif // _OLEAUTO_H_
// #endif // !_ATL_NO_COM
// 
// #if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
//     int GetText(int nIndex, _CSTRING_NS::CString& strText) const
//     {
//         DbgProtect(!m_hWnd, 0);
//         DbgAssert(IsWindow());
//         int cchLen = GetTextLen(nIndex);
//         if (cchLen == LB_ERR)
//             return LB_ERR;
//         int nRet = LB_ERR;
//         TMStr lpstr = strText.GetBufferSetLength(cchLen);
//         if (lpstr != NULL)
//         {
//             nRet = GetText(nIndex, lpstr);
//             strText.ReleaseBuffer();
//         }
//         return nRet;
//     }
// #endif // defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)

    int SetItemAccText(int nIndex, const CStrPtr& strOrKey, int idRes = 0, TMHModule hModule = 0)
    {
        DbgProtect(!this->m_hWnd, false);
        return this->_SendSetTextMessage(CM_SETITEMACCTEXT, nIndex, false, &strOrKey, idRes, hModule);
    }

    CStrPtr GetItemAccText(int nIndex)
    {
        CStrPtr ptr;
        DbgProtect(!this->m_hWnd, ptr);
        this->SendMessage(CM_GETITEMACCTEXT, nIndex, (TMLParam)&ptr);
        return std::move(ptr);
    }

    int SetItemHint(int nIndex, const CStrPtr& strOrKey, int idRes = 0, TMHModule hModule = 0)
    {
        return this->_SendSetTextMessage(CM_SETITEMHINT, nIndex, false, &strOrKey, idRes, hModule);
    }
    
    int GetItemHint(int nIndex, CStrPtr& v) const
    {
        return this->SendMessage(CM_GETITEMHINT, nIndex, (TMLParam)&v);
    }

    CStrPtr GetItemHint(int nIndex)
    {
        CStrPtr ptr;
        DbgProtect(!this->m_hWnd, ptr);
        GetItemHint(nIndex, ptr);
        return std::move(ptr);
    }
    
    int GetTextLen(int nIndex) const
    {
        return (int)this->SendMessage(LB_GETTEXTLEN, nIndex, 0L);
    }

    int GetItemHeight(int nIndex) const
    {
        return (int)this->SendMessage(LB_GETITEMHEIGHT, nIndex, 0L);
    }

    int SetItemHeight(int nIndex, TMUInt cyItemHeight)
    {
        return (int)this->SendMessage(LB_SETITEMHEIGHT, nIndex, MAKELONG(cyItemHeight, 0));
    }

    // Settable only attributes
//  void SetColumnWidth(int cxWidth)
//  {
//      SendMessage(LB_SETCOLUMNWIDTH, cxWidth, 0L);
//  }
//
//  TMBool SetTabStops(int nTabStops, TMInt* rgTabStops)
//  {
//      DbgAssert((GetStyle() & LBS_USETABSTOPS) != 0);
//      return (TMBool)SendMessage(LB_SETTABSTOPS, nTabStops, (TMLParam)rgTabStops);
//  }
//
//  TMBool SetTabStops()
//  {
//      DbgAssert((GetStyle() & LBS_USETABSTOPS) != 0);
//      return (TMBool)SendMessage(LB_SETTABSTOPS, 0, 0L);
//  }
//
//  TMBool SetTabStops(const int& cxEachStop)    // takes an 'int'
//  {
//      DbgAssert((GetStyle() & LBS_USETABSTOPS) != 0);
//      return (TMBool)SendMessage(LB_SETTABSTOPS, 1, (TMLParam)(TMInt*)&cxEachStop);
//  }

// Operations
//     int InitStorage(int nItems, TMUInt nBytes)
//     {
//         return (int)SendMessage(LB_INITSTORAGE, (TMWParam)nItems, nBytes);
//     }

    void ResetContent()
    {
        this->SendMessage(LB_RESETCONTENT, 0, 0L);
    }

//     TMUInt ItemFromPoint(TMPointL pt, TMBool& bOutside) const
//     {
//         TMUInt dw = (TMUInt)SendMessage(LB_ITEMFROMPOINT, 0, MAKELPARAM(pt.x, pt.y));
//         bOutside = (TMBool)HIWORD(dw);
//         return (TMUInt)LOWORD(dw);
//     }

    // manipulating listbox items
    int AddString(const CStrPtr& lpszItem, int bHtml = TUISETTEXT::eDefault)
    {
        return (int)this->_SendSetTextMessage(LB_ADDSTRING, 0, bHtml, &lpszItem);
    }

    int AddString(int nIDStr, int bHtml = TUISETTEXT::eDefault, TMHModule hModule = 0)
    {
        return (int)this->_SendSetTextMessage(LB_ADDSTRING, 0, bHtml, hModule, nIDStr);
    }

    int AddString(int nIDStr, TMHModule hModule)
    {
        return (int)this->_SendSetTextMessage(LB_ADDSTRING, 0, TUISETTEXT::eDefault, hModule, nIDStr);
    }

    int InsertString(int nIndex, const CStrPtr& lpszItem, int bHtml = TUISETTEXT::eDefault)
    {
        return (int)this->_SendSetTextMessage(LB_INSERTSTRING, nIndex, bHtml, &lpszItem);
    }

    int InsertString(int nIndex, int nIDStr, int bHtml = TUISETTEXT::eDefault, TMHModule hModule = 0)
    {
        return (int)this->_SendSetTextMessage(LB_INSERTSTRING, nIndex, bHtml, hModule, nIDStr);
    }

    int InsertString(int nIndex, int nIDStr, TMHModule hModule)
    {
        return (int)this->_SendSetTextMessage(LB_INSERTSTRING, nIndex, TUISETTEXT::eDefault, hModule, nIDStr);
    }

    int DeleteString(TMUInt nIndex)
    {
        return (int)this->SendMessage(LB_DELETESTRING, nIndex, 0L);
    }

    // attr: FILE_ATTRIBUTE_NORMAL, FILE_ATTRIBUTE_DIRECTORY, ....
    int Dir(TMUInt attr, TMCStr lpszWildCard)
    {
        return (int)this->SendMessage(LB_DIR, attr, (TMLParam)lpszWildCard);
    }

    int AddFile(TMCStr lpstrFileName)
    {
        return (int)this->SendMessage(LB_ADDFILE, 0, (TMLParam)lpstrFileName);
    }

    // selection helpers
    // err: return LB_ERR
    int FindString(int nStartAfter, TMCStr lpszItem) const
    {
        return (int)this->SendMessage(LB_FINDSTRING, nStartAfter, (TMLParam)lpszItem);
    }

    int FindStringExact(int nIndexStart, TMCStr lpszFind) const
    {
        return (int)this->SendMessage(LB_FINDSTRINGEXACT, nIndexStart, (TMLParam)lpszFind);
    }

    int SelectString(int nStartAfter, TMCStr lpszItem)
    {
        return (int)this->SendMessage(LB_SELECTSTRING, nStartAfter, (TMLParam)lpszItem);
    }

    int SelItemRange(TMBool bSelect, int nFirstItem, int nLastItem)
    {
        DbgProtect(!this->m_hWnd, -1);
        DbgAssert((this->GetStyle() & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) != 0);
        DbgAssert(nFirstItem <= nLastItem);
        return bSelect ? (int)this->SendMessage(LB_SELITEMRANGEEX, nFirstItem, nLastItem) : (int)this->SendMessage(LB_SELITEMRANGEEX, nLastItem, nFirstItem);
    }

// #ifdef WIN32_PLATFORM_WFSP   // SmartPhone only messages
//  TMUInt GetInputMode(TMBool bCurrentMode = TMTrue)
//  {
//      return SendMessage(LB_GETINPUTMODE, 0, (TMLParam)bCurrentMode);
//  }
//
//  TMBool SetInputMode(TMUInt dwMode)
//  {
//      return SendMessage(LB_SETINPUTMODE, 0, (TMLParam)dwMode);
//  }
// #endif // WIN32_PLATFORM_WFSP

    TMBool SetForePic(int nID, TMHModule hModule = 0)
    {
        return this->SendMessage(CLB_SETFOREPIC, nID, (TMLParam)hModule);
    }

    TMBool SetForePic(ITuiPic* pPic)
    {
        return this->SendMessage(CLB_SETFOREPIC, 0, (TMLParam)pPic);
    }

    ITuiPic* GetForePic()
    {
        return (ITuiPic*)this->SendMessage(CLB_GETFOREPIC, 0, 0);
    }
};

typedef CTuiListBoxT<CTuiWidget>   CTuiListBox;



////////////////////////////////////////////////////////////////////////////////
// class CTuiTreeViewT
// notify codes: TVN_STATECHANGE, TVN_SELCHANGE
//
// INotifyPropertyChanged, INotifyPropertyChanging 13:49
// http://www.eggheadcafe.com/tutorials/aspnet/fc841e66-3aac-4bd9-baba-bf26b9783a9b/wpf-treeview-control-with.aspx  13:50
// INotifyCollectionChanged

typedef class ITVItem* HTVITEM;


#ifndef TV_FIRST
#   define TV_FIRST                0x1100      // TreeView messages
// #   define TVM_INSERTITEM          (TV_FIRST + 0)
// #   define TVM_DELETEITEM          (TV_FIRST + 1)
// #   define TVM_EXPAND              (TV_FIRST + 2)
// #   define TVM_GETITEMRECT         (TV_FIRST + 4)
#   define TVM_GETCOUNT            (TV_FIRST + 5)
// #   define TVM_GETINDENT           (TV_FIRST + 6)
// #   define TVM_SETINDENT           (TV_FIRST + 7)
// #   define TVM_GETIMAGELIST        (TV_FIRST + 8)
// #   define TVM_SETIMAGELIST        (TV_FIRST + 9)
#   define TVM_GETNEXTITEM         (TV_FIRST + 10)
// #   define TVM_SELECTITEM          (TV_FIRST + 11)
// #   define TVM_GETITEM             (TV_FIRST + 12)
// #   define TVM_SETITEM             (TV_FIRST + 13)
// #   define TVM_EDITLABEL           (TV_FIRST + 14)
// #   define TVM_GETEDITCONTROL      (TV_FIRST + 15)
#   define TVM_GETVISIBLECOUNT     (TV_FIRST + 16)
// #   define TVM_HITTEST             (TV_FIRST + 17)
// #   define TVM_CREATEDRAGIMAGE     (TV_FIRST + 18)
// #   define TVM_SORTCHILDREN        (TV_FIRST + 19)
// #   define TVM_ENSUREVISIBLE       (TV_FIRST + 20)
// #   define TVM_SORTCHILDRENCB      (TV_FIRST + 21)
// #   define TVM_ENDEDITLABELNOW     (TV_FIRST + 22)
// #   define TVM_GETISEARCHSTRING    (TV_FIRST + 23)
// #   define TVM_SETTOOLTIPS         (TV_FIRST + 24)
// #   define TVM_GETTOOLTIPS         (TV_FIRST + 25)
// #   define TVM_SETINSERTMARK       (TV_FIRST + 26)
#   define TVM_SETITEMHEIGHT       (TV_FIRST + 27)
#   define TVM_GETITEMHEIGHT       (TV_FIRST + 28)
// #   define TVM_SETBKCOLOR          (TV_FIRST + 29)
// #   define TVM_SETTEXTCOLOR        (TV_FIRST + 30)
// #   define TVM_GETBKCOLOR          (TV_FIRST + 31)
// #   define TVM_GETTEXTCOLOR        (TV_FIRST + 32)
// #   define TVM_SETSCROLLTIME       (TV_FIRST + 33)
// #   define TVM_GETSCROLLTIME       (TV_FIRST + 34)
// #   define TVM_SETINSERTMARKCOLOR  (TV_FIRST + 37)
// #   define TVM_GETINSERTMARKCOLOR  (TV_FIRST + 38)
#endif

const int TVM_REGISTEREVENT         = TV_FIRST + 50;
const int TVM_RESETCONTENT          = LB_RESETCONTENT;
const int TVM_GETFOREPIC            = CBM_GETFOREPIC;
const int TVM_SETFOREPIC            = CBM_SETFOREPIC;

#define TVN_SELCHANGE               LBN_SELCHANGE
#define TVN_STATECHANGE             2

#define TVHT_ONCHECKBOX             0x1000

#define TVIS_HIDE                   0x8000
#define TVIS_CHECKED                0x4000
#define TVIS_DISABLE                0x2000
#define TVIS_AUTOHEIGHT             0x1000 // only for bind window, let item height = bind window height.
#define TVIS_NODRAWTEXT             0x0800
#define TVIS_HTML                   0x0400
#define TVIS_MASK                   0xffff
// #define TVIS_SELECTED           0x0002
// #define TVIS_CUT                0x0004
// #define TVIS_DROPHILITED        0x0008
// #define TVIS_BOLD               0x0010
// #define TVIS_EXPANDED           0x0020
// #define TVIS_EXPANDEDONCE       0x0040
// #define TVIS_EXPANDPARTIAL      0x0080
// #define TVIS_OVERLAYMASK        0x0F00
// #define TVIS_STATEIMAGEMASK     0xF000
// #define TVIS_USERMASK           0xF000

// #define TVIS_MOUSEOVER   0x4000000
// #define TVGN_PREVIOUSEL  0x000B


// return hItem1 < hItem2 ? true : false;
typedef TMBool (CALLBACK *PFNTVCOMPARE)(HTVITEM hItem1, HTVITEM hItem2, TMLParam lParamSort);

class TM_NO_VTABLE ITVItem : public IDomNode
{
public:
    virtual HTUI GetWindow() = 0;

    virtual GXDips GetHeight() = 0;
    virtual void SetWidth(GXDips nWidth) = 0;
    virtual void SetHeight(GXDips nHeight) = 0;
    virtual TMBool GetRect(TUIRect* lpRect) = 0;
    virtual TMUInt GetState() = 0;
    virtual int GetLevel() = 0;
    virtual void* GetData() = 0;
    virtual void SetData(void* dwData, TMBool bRedraw = true) = 0;
    virtual void SetHint(const TUISETTEXT& wst) = 0;
    virtual CStrPtr GetHint() = 0;
    virtual void SetText(const TUISETTEXT& wst) = 0;
    virtual TMBool SetState(TMUInt uState, TMUInt uMask) = 0;
    virtual void ExpandAll() = 0;

    virtual ITVItem* InsertNew(const CStrPtr& str, EZOrder nZOrderOrPos, TMUInt uState = 0) = 0;
    virtual TMBool SetZOrder(ITVItem* hIterator, EZOrder eZOrderOrIndex) = 0; // eZOrderUp, eZOrderDown will failed when hIterator == root item
    virtual TMUInt GetChildCount() = 0;
    virtual TMUInt GetAllChildCount() = 0;
    virtual TMBool IsChild(ITVItem* hChild) = 0;
    virtual ITVItem* GetChildAt(int nIndex) = 0;
    virtual ITVItem* GetRoot() = 0;
    virtual ITVItem* GetPrevVisible() = 0;
    virtual ITVItem* GetNextVisible() = 0;
    virtual ITVItem* GetGlobalPrev(ITVItem* hParent = 0) = 0;
    virtual ITVItem* GetGlobalNext(ITVItem* hParent = 0) = 0;

    // show or hide Item;
    // (bShow && bEnsure) will set all of the parent window to show.
    // but it not expand parent, not EnsureVisible.
    virtual void ShowItem(TMBool bShow, TMBool bEnsure) = 0;
    virtual TMBool IsVisible() = 0;

    //TVHT_ONITEMICON, TVHT_ONITEMLABEL, TVHT_ONITEM, TVHT_ONITEMINDENT, TVHT_ONITEMBUTTON, TVHT_ONITEMRIGHT, TVHT_ONITEMSTATEICON
    virtual TMBool IsMouseOver(TMUInt32 dwType = -1) = 0;

    // it will expand all parent.
    // return false: item or parent is TVIS_HIDE.
    virtual TMBool EnsureVisible() = 0;
    virtual void ScrollToVisible() = 0;

    //TVHT_ONITEMICON, TVHT_ONITEMLABEL, TVHT_ONITEM, TVHT_ONITEMINDENT, TVHT_ONITEMBUTTON, TVHT_ONITEMRIGHT, TVHT_ONITEMSTATEICON
    //TVHT_ONCHECKBOX
    //selected: return down;
    virtual EButState GetButtonState(TMUInt32 dwType = -1) = 0;

    virtual void SelectItem(TMUInt32 dwOption) = 0;

    // pfn == 0  sort by character
    virtual TMBool Sort(TM::PFNTVCOMPARE pfn, TMLParam lParam, int nLevel, TMBool bReversed) = 0;

    virtual void Invalidate() = 0;

    // bind window
    virtual void BindItemWindow(HTUI hWnd) = 0;
    virtual HTUI CreateBindItemWindow(TMUInt nObjID) = 0;
    virtual HTUI DetachBindItemWindow(HTUI hNewParent) = 0;
    virtual HTUI GetBindItemWindow() = 0;

};


class ITVEvent
{
public:
    //virtual void OnCreateItem (HTVITEM hItem) {}

    virtual void OnDeleteItem (HTVITEM hItem) {}

    virtual void OnStateChanged (HTVITEM hItem, TMUInt uOld, TMUInt uNew) {}

    // single selection tree: generated when single selection made
    // multiple selection tree: generated when a series of selections made
    virtual void OnSelChanged (HTVITEM hOld, HTVITEM hNew, TMUInt uKey = 0) {}

//    virtual void OnBeginDrag (HTVITEM hItem) {}
//    virtual void OnBeginRDrag (HTVITEM hItem) {}
//    virtual void OnGetInfotip (HTVITEM hItem) {}
//    virtual TMBool OnSelChanging (HTVITEM hOld, HTVITEM hNew) { return true; }
};

class CTVItem
{
public:
    enum ESelOption
    {
        e_soNull            = 0x000,
        e_soCtrl            = 0x001,    // simulate ctrl key be hit down
        e_soShift           = 0x002,    // simulate shift key be hit down
        e_soAll             = 0x003,    // simulate shift & ctrl key be hit down
        e_soDeselect        = 0x004,    // unselect the item
        e_soCheckShirt      = 0x010,    // check shift key
        e_soCheckCtrl       = 0x020,    // check ctrl key
        e_soAuto            = 0x030,    // check current shift & ctrl key
        e_soEnsureVisible   = 0x100,    // Ensure Visible

        e_soSelMask         = 0x00f,
        e_soCheckMask       = 0x0f0,
    };

public:
    HTVITEM m_hItem;

    CTVItem (HTVITEM hItem = 0)
        : m_hItem (hItem)
    {
    }

    operator HTVITEM () const { return m_hItem; }

    HTUI GetWindow()
    {
        DbgAssert(*this);
        return m_hItem->GetWindow();
    }

    void BindItemWindow(HTUI hWnd)
    {
        DbgAssert(*this);
        DbgAssert(m_hItem->get_parentNode()); // root item can't BindItemWindow
        DbgAssert(hWnd != GetWindow());
        if (hWnd)
            DbgAssert(!CTuiWidget(hWnd).IsChild(GetWindow()));
        m_hItem->BindItemWindow(hWnd);
    }

    HTUI CreateBindItemWindow(TMUInt nObjID)
    {
        DbgAssert(*this);
        DbgAssert(m_hItem->get_parentNode()); // root item can't CreateBindItemWindow
        return m_hItem->CreateBindItemWindow(nObjID);
    }

    HTUI DetachBindItemWindow(HTUI hNewParent)
    {
        DbgAssert(*this);
        return m_hItem->DetachBindItemWindow(hNewParent);
    }

    HTUI GetBindItemWindow()
    {
        DbgAssert(*this);
        return m_hItem->GetBindItemWindow();
    }

    EBindState IsBindChild(HTUI hChild)
    {
        DbgAssert(*this);
        if (!hChild)
            return eBindStateNone;
        HTUI hBind = GetBindItemWindow();
        if (!hBind)
            return eBindStateNone;
        if (hBind == hChild)
            return eBindStateSelf;
        return CTuiWidget(hBind).IsChild(hChild) ? eBindStateParent : eBindStateNone;
    }

//     int GetDefaultHeight();

    int GetHeight()
    {
        DbgAssert(*this);
        return m_hItem->GetHeight();
    }

    void SetWidth(int nWidth)
    {
        DbgAssert(*this);
        m_hItem->SetWidth(nWidth);
    }

    void SetHeight(int nHeight)
    {
        DbgAssert(*this);
        m_hItem->SetHeight(nHeight);
    }

    TMBool GetRect(TUIRect* lpRect)
    {
        DbgAssert(*this);
        return m_hItem->GetRect(lpRect);
    }

    // show or hide Item;
    // (bShow && bEnsure) will set all of the parent window to show.
    // but it not expand parent, not EnsureVisible.
    void ShowItem(TMBool bShow, TMBool bEnsure)
    {
        DbgAssert(*this);
        m_hItem->ShowItem(bShow, bEnsure);
    }

    TMUInt GetState()
    {
        DbgAssert(*this);
        return m_hItem->GetState();
    }

    //TVHT_ONITEMICON, TVHT_ONITEMLABEL, TVHT_ONITEM, TVHT_ONITEMINDENT, TVHT_ONITEMBUTTON, TVHT_ONITEMRIGHT, TVHT_ONITEMSTATEICON
    //TVHT_ONCHECKBOX
    //selected: return down;
    EButState GetButtonState(TMUInt32 dwType = -1)
    {
        DbgAssert(*this);
        return m_hItem->GetButtonState(dwType);
    }

    TMUInt GetChildCount()
    {
        DbgAssert(*this);
        return m_hItem->GetChildCount();
    }

    TMUInt GetAllChildCount()
    {
        DbgAssert(*this);
        return m_hItem->GetAllChildCount();
    }

//     void SetState (TMUInt nState, TMUInt nStateMask);

    CStrPtr GetText()
    {
        DbgAssert(*this);
        return m_hItem->get_nodeValue();
    }

    void GetText(CString& str)
    {
        str = std::move(GetText());
    }

    void GetText(std::tstring& str)
    {
        GetText().swap(str);
    }

    void GetText(TMStrW pStr, int nBuffer)
    {
        GetText().CopyTo(pStr, nBuffer);
    }

    void SetText(const CStrPtr& lpszTxt)
    {
        DbgAssert(*this);
        m_hItem->put_nodeValue(lpszTxt);
    }

    void* GetData() const
    {
        DbgAssert(*this);
        return (const_cast<HTVITEM>(m_hItem))->GetData();
    }

    void SetData(void* dwData, TMBool bRedraw = true)
    {
        DbgAssert(*this);
        m_hItem->SetData(dwData, bRedraw);
    }

    void SetHint(const CStrPtr& strOrID, int idRes = 0, TMHModule hModule = 0)
    {
        DbgAssert(*this);
        TUISETTEXT wst = { false, hModule, idRes, &strOrID };
        (const_cast<HTVITEM>(m_hItem))->SetHint(wst);
    }

//     void SetHint(int idText, TMHModule hResText = 0)
//     {
//         DbgAssert(*this);
//         TUISETTEXT wst = { false, hResText, idText, 0 };
//         (const_cast<HTVITEM>(m_hItem))->SetHint(wst);
//     }

    CStrPtr GetHint()
    {
        DbgAssert(*this);
        return std::move((const_cast<HTVITEM>(m_hItem))->GetHint());
    }

    void GetHint(std::tstring& str)
    {
        GetHint().swap(str);
    }

    void GetHint(CString& str)
    {
        str = std::move(GetHint());
    }


//     HIMAGELIST CreateDragImage ();

    int GetLevel()
    {
        DbgAssert(*this);
        return m_hItem->GetLevel();
    }

//     TMBool EditLabel ();

    // it will expand all parent.
    // return false: item or parent is TVIS_HIDE.
    TMBool EnsureVisible()
    {
        DbgAssert(*this);
        DbgAssert(m_hItem->get_parentNode()); // root item can't ScrollToVisible
        return m_hItem->EnsureVisible();
    }

    void ScrollToVisible()
    {
        DbgAssert(*this);
        DbgAssert(m_hItem->get_parentNode()); // root item can't ScrollToVisible
        m_hItem->ScrollToVisible();
    }

    CTVItem GetChild(int nIndex)
    {
        DbgAssert(*this);
        return m_hItem->GetChildAt(nIndex);
    }

    CTVItem GetChild()
    {
        DbgAssert(*this);
        return static_cast<HTVITEM>(m_hItem->get_firstChild());
    }

    // 0: ->prev; 1: ->prev->prev; ...
//     CTVItem GetPrev (int nSkip)
//     {
//         DbgAssert(*this);
//         return m_hItem->GetPrevAt(nSkip);
//     }

    CTVItem GetPrev ()
    {
        DbgAssert(*this);
        return static_cast<HTVITEM>(m_hItem->get_previousSibling());
    }

    // 0: ->next; 1: ->next->next; ...
//     CTVItem GetNext (int nSkip)
//     {
//         DbgAssert(*this);
//         return m_hItem->GetNextAt(nSkip);
//     }

    CTVItem GetNext ()
    {
        DbgAssert(*this);
        return static_cast<HTVITEM>(m_hItem->get_nextSibling());
    }

    CTVItem GetParent()
    {
        DbgAssert(*this);
        return static_cast<HTVITEM>(m_hItem->get_parentNode());
    }

    CTVItem GetRoot()
    {
        DbgAssert(*this);
        return m_hItem->GetRoot();
    }

    CTVItem GetPrevVisible()
    {
        DbgAssert(*this);
        return m_hItem->GetPrevVisible();
    }

    CTVItem GetNextVisible()
    {
        DbgAssert(*this);
        return m_hItem->GetNextVisible();
    }

    // enumerate entire tree structure
    CTVItem GetGlobalPrev(HTVITEM hParent = 0)
    {
        DbgAssert(*this);
        return m_hItem->GetGlobalPrev(hParent);
    }

    CTVItem GetGlobalNext(HTVITEM hParent = 0)
    {
        DbgAssert(*this);
        return m_hItem->GetGlobalNext(hParent);
    }

//     CTVItem SortInsertChild (TMCStr pStr, PFNTVCOMPARE pfn, TMLParam lParam,TMBool bReversed);

    CTVItem InsertChild(const CStrPtr& str, size_t nPos = 0)
    {
        DbgAssert(*this);
//        return m_hItem->InsertChild(pStr, nPos);
        DbgAssert(nPos != (size_t)eZOrderDown);
        DbgAssert(nPos != (size_t)eZOrderUp);
        return m_hItem->InsertNew(str, (EZOrder)nPos);
    }

    inline CTVItem AppendChild(const CStrPtr& str) { return InsertChild(str, -1); }

//    CTVItem InsertSort (TMCStr pStr) const;

    CTVItem InsertAfter(const CStrPtr& str)
    {
        DbgAssert(*this);
        DbgAssert(m_hItem->get_parentNode()); // root item can't insert after
        return m_hItem->InsertNew(str, eZOrderDown);
    }

    CTVItem InsertBefore(const CStrPtr& str)
    {
        DbgAssert(*this);
        DbgAssert(m_hItem->get_parentNode()); // root item can't insert before
        return m_hItem->InsertNew(str, eZOrderUp);
    }

    TMBool HasChildren() { return GetChild() ? true : false; }

    void Remove()
    {
        DbgAssert(*this);
        m_hItem->removeChild(m_hItem);
    }

    void RemoveChild() //not include itself
    {
        DbgAssert(*this);
        m_hItem->removeAllChild();
    }

    void Expand()
    {
        DbgAssert(*this);
        m_hItem->SetState(TVIS_EXPANDED, TVIS_EXPANDED);
    }

    void ExpandAll()
    {
        DbgAssert(*this);
        m_hItem->ExpandAll();
    }

    void Collapse()
    {
        DbgAssert(*this);
        m_hItem->SetState(0, TVIS_EXPANDED);
//         m_hItem->Collapse();
    }

    void Toggle()
    {
        DbgAssert(*this);
//         m_hItem->Toggle();
        m_hItem->SetState(~GetState(), TVIS_EXPANDED);
    }

    void SetCheck(TMBool bCheck)
    {
        DbgAssert(*this);
//         m_hItem->SetCheck(bCheck);
        m_hItem->SetState(bCheck ? TVIS_CHECKED : 0, TVIS_CHECKED);
    }

    void Enable()
    {
        DbgAssert(*this);
//         m_hItem->Enable();
        m_hItem->SetState(0, TVIS_DISABLE);
    }

    void Disable()
    {
        DbgAssert(*this);
//         m_hItem->Disable();
        m_hItem->SetState(TVIS_DISABLE, TVIS_DISABLE);
    }

    TMBool IsVisible() { return !(GetState() & TVIS_HIDE); }

    TMBool IsDisable() {return GetState() & TVIS_DISABLE; }

    TMBool IsExpand() { return GetState() & TVIS_EXPANDED; }

    TMBool IsSelected() { return GetState() & TVIS_SELECTED; }

    TMBool IsChecked() { return GetState() & TVIS_CHECKED; }

    //TVHT_ONITEMICON, TVHT_ONITEMLABEL, TVHT_ONITEM, TVHT_ONITEMINDENT, TVHT_ONITEMBUTTON, TVHT_ONITEMRIGHT, TVHT_ONITEMSTATEICON
    TMBool IsMouseOver(TMUInt32 dwType = -1)
    {
        DbgAssert(*this);
        return m_hItem->IsMouseOver(dwType);
    }

    TMBool IsChild(HTVITEM hChild)
    {
        DbgAssert(*this);
        return m_hItem->IsChild(hChild);
    }

    TMBool FindChild(HTVITEM hChild)
    {
        DbgAssert(*this);
        return DomFindChild(m_hItem, hChild);
    }

    void SelectItem(TMUInt32 dwOption = e_soEnsureVisible)
    {
        DbgAssert(*this);
        m_hItem->SelectItem(dwOption);
    }

    // pfn == 0  sort by character
    TMBool Sort(TM::PFNTVCOMPARE pfn, TMLParam lParam, int nLevel, TMBool bReversed)
    {
        DbgAssert(*this);
        return m_hItem->Sort(pfn, lParam, nLevel, bReversed);
    }

    // eZOrderUp, eZOrderDown will failed when hIterator == root item
    TMBool SetZOrder(HTVITEM hIterator, EZOrder eZOrder)
    {
        DbgAssert(*this);
        return m_hItem->SetZOrder(hIterator, eZOrder);
    }

    // move item before hBefore;
    // move item to top when hAfter == 0;
    TMBool MoveUp(HTVITEM hBefore)
    {
        DbgAssert(*this);
        //m_hItem->MoveUp(hBefore);
        return SetZOrder(hBefore, eZOrderUp);
    }

    // move item after hAfter;
    // move item to bottom when hBefore == 0;
    TMBool MoveDown(HTVITEM hAfter)
    {
        DbgAssert(*this);
//         m_hItem->MoveDown(hAfter);
        return SetZOrder(hAfter, eZOrderDown);
    }

    // move item to parent's first child;
    void SetParent(HTVITEM hParent)
    {
        DbgAssert(*this);
//         m_hItem->SetParent(hParent);
        SetZOrder(hParent, eZOrderChildFirst);
    }

    TMBool SetState(TMUInt uState, TMUInt uMask = TVIS_MASK)
    {
        DbgAssert(*this);
        // use selectItem to set select flag.
        DbgAssert((uState & TVIS_SELECTED & TVIS_MASK) == (GetState() & TVIS_SELECTED & TVIS_MASK));
        return m_hItem->SetState(uState, uMask);
    }

    void Invalidate()
    {
        DbgAssert(*this);
        m_hItem->Invalidate();
    }

    void DeleteItemsByState(TMUInt uState, TMUInt uMask)
    {
        for (CTVItem itm(*this); itm;)
        {
            CTVItem itmTmp;
            for (; itmTmp = itm.GetGlobalNext(*this);)
            {
                if ((itmTmp.GetState() & uMask) == uState)
                    itmTmp.Remove();
                else
                    break;
            }
            itm = itmTmp;
        }
    }

    void DeleteSelectedItems()
    {
        DeleteItemsByState(TVIS_SELECTED, TVIS_SELECTED);
    }
};

template <class T>
class CTVItemT : public CTVItem
{
public:
    CTVItemT (HTVITEM hItem = 0)
        : CTVItem (hItem)
    {
    }

    CTVItemT (const CTVItem& rhs)
        : CTVItem (rhs)
    {
    }

    CTVItemT& operator = (HTVITEM hItem)
    {
        m_hItem = hItem;
        return *this;
    }

    CTVItemT& operator = (const CTVItem& rhs)
    {
        m_hItem = rhs.m_hItem;
        return *this;
    }

    T GetData ()
    {
        return (T) CTVItem::GetData();
    }

    void SetData (T tData, TMBool bRedraw = true)
    {
        CTVItem::SetData ((void*)tData, bRedraw);
    }
};

template < class TBase >
class CTuiTreeT : public TBase
{
public:
    CLASS_IMPL(CTuiTreeT, TUI_CLASS_TREE);

//     void SetProvider(ITVProvider* pSource)
//     {
//         this->SendMessage(TVM_SETPROVIDER, 0, (TMLParam)pSource);
//     }

    // sample:    m_wndTree.SetAutoDeleteData(CTuiProcDelete<TreeItem*>::done);
    void SetPfnAutoDeleteData(PFNDELETEPROC proc)
    {
        this->SendMessage(CM_SETDELETEDATA, 0, (TMLParam)proc);
    }

    void RegisterEventSink (ITVEvent* pSink)
    {
        this->SendMessage(TVM_REGISTEREVENT, 0, (TMLParam)pSink);
    }

    void ResetContent()
    {
        this->SendMessage(TVM_RESETCONTENT, 0, 0);
    }

    int GetCount() const
    {
        return this->SendMessage(TVM_GETCOUNT, 0, 0L);
    }

//     CTVItem GetDropHilight () const
//     {
//         return (HTVITEM)this->SendMessage(TVM_GETNEXTITEM, TVGN_DROPHILITE, 0L);
//     }

    // certainly in window
    CTVItem GetFirstVisible() const
    {
        return (HTVITEM)this->SendMessage(TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0L);
    }

    // different from FirstVisible concept,could be out of widget.
    CTVItem GetLastVisible() const
    {
        return (HTVITEM)this->SendMessage(TVM_GETNEXTITEM, TVGN_LASTVISIBLE, 0L);
    }

    // can set hItem = 0;
    void SelectItem(HTVITEM hItem, TMUInt dwOption = CTVItem::e_soEnsureVisible)
    {
        this->SendMessage(TVM_SELECTITEM, dwOption, (TMLParam)hItem);
    }

    // only for single selected item
    CTVItem GetSelectedItem() const
    {
        return (HTVITEM)this->SendMessage(TVM_GETNEXTITEM, TVGN_CARET, 0L);
    }

//     CTVItem GetPreSelectedItem() const
//     {
//         return (HTVITEM) SendMessage(TVM_GETNEXTITEM, TVGN_PREVIOUSEL, 0L);
//     }
    CTVItem GetRootItem() const
    {
        HTVITEM hItem = (HTVITEM)this->SendMessage(TVM_GETNEXTITEM, TVGN_ROOT, 0L);
        return hItem;
    }

    void SetItemWidth(int nWidth)
    {
        GetRootItem().SetWidth (nWidth);
    }

    TMBool SetItemHeight(int nHeight)
    {
        return this->SendMessage(TVM_SETITEMHEIGHT, nHeight, 0L);
    }

    int GetItemHeight() const
    {
        return this->SendMessage(TVM_GETITEMHEIGHT, 0, 0L);
    }

//     TMColor GetTextColor() const
//     {
//         return (TMColor)this->SendMessage(TVM_GETTEXTCOLOR, 0, 0L);
//     }
// 
//     TMColor SetTextColor(TMColor clr)
//     {
//         return (TMColor)this->SendMessage(TVM_SETTEXTCOLOR, 0, (TMLParam)clr);
//     }

    int GetVisibleCount() const
    {
        return this->SendMessage(TVM_GETVISIBLECOUNT, 0, 0L);
    }

//     TMBool Expand(HTVITEM hItem, TMUInt uFlag)
//     {
//         return this->SendMessage(TVM_EXPAND, uFlag, (TMLParam)(hItem));
//     }

    TMBool SetForePic(int nID, TMHModule hModule = 0)
    {
        return this->SendMessage(TVM_SETFOREPIC, nID, (TMLParam)hModule);
    }

    TMBool SetForePic(ITuiPic* pPic)
    {
        return this->SendMessage(TVM_SETFOREPIC, 0, (TMLParam)pPic);
    }

    ITuiPic* GetForePic()
    {
        return (ITuiPic*)this->SendMessage(TVM_GETFOREPIC, 0, 0);
    }
};

typedef CTuiTreeT<CTuiWidget>                           CTuiTree;


////////////////////////////////////////////////////////////////////////////////
// class CTuiHotKey
template <class TBase>
class CTuiHotKeyT : public TBase
{
public:
    CLASS_IMPL(CTuiHotKeyT, TUI_CLASS_HOTKEY);
    enum { eCountHotKey = 10 };

    TMBool SetHotKey(int nIndex, const TUIHOTKEY& hotkey)
    {
        return this->SendMessage(CM_SETHOTKEY, nIndex, (TMLParam)&hotkey);
    }

    TMBool GetHotKey(int nIndex, TUIHOTKEY& hotkey)
    {
        DbgAssert(nIndex != -1);
        return this->SendMessage(CM_GETHOTKEY, nIndex, (TMLParam)&hotkey);
    }

    TMBool SetHotKeys(const TUIHOTKEY* hotkey) // hotkey: const CLHOTKEY[eCountHotKey]*
    {
        return this->SendMessage(CM_SETHOTKEY, -1, (TMLParam)hotkey);
    }

    TMBool GetHotKeys(TUIHOTKEY* hotkey) // hotkey: TUIHOTKEY[eCountHotKey]*
    {
        return this->SendMessage(CM_GETHOTKEY, -1, (TMLParam)hotkey);
    }
};

typedef CTuiHotKeyT<CTuiWidget>                         CTuiHotKey;




#if TM_WIN_DESKTOP


////////////////////////////////////////////////////////////////////////////////
// class CTuiVideoT
template <class TBase>
class CTuiVideoT : public TBase
{
public:
    CLASS_IMPL(CTuiVideoT, TUI_CLASS_VIDEO);

    enum ENotifyCode
    {
        VN_COMPLETE     = 0x01,                 // EC_COMPLETE
        VN_USERABORT    = 0x02,                 // EC_USERABORT
        VN_ERRORABORT   = 0x03,                 // EC_ERRORABORT
        VN_CLOCKCHANGED = 0x0D,                 // EC_CLOCK_CHANGED
        VN_PAUSED       = 0x0E,                 // EC_PAUSED
        VN_LOAD         = 0x100,
        VN_PLAY         = 0x101,
        VN_SEEKING      = 0x102,                // set style VS_NOTIFY, use EC_STEP_COMPLETE?
    };

//     IBaseFilter* GetRender()
//     {
//         return (IBaseFilter*)this->SendMessage(VM_GETRENDER, 0, 0);
//     }

    void AddTextTrack()
    {
    }

    TMBool CanPlayType() const
    {
    }

    TMBool Load()
    {
        return this->SendMessage(VM_LOAD, 0, 0);
    }

    TMBool Play()
    {
        return this->SendMessage(VM_PLAY, 0, 0);
    }

    TMBool Pause()
    {
        return this->SendMessage(VM_PAUSE, 0, 0);
    }

    void GetAudioTracks()
    {
    }

    void SetAutoplay() {}
    void GetAutoplay() {}

    void GetBuffered() {}
    void GetController() {}

    void SetControls() {}
    void GetControls() {}

    void SetCrossOrigin() {}
    void GetCrossOrigin() {}

    void GetCurrentSrc() {}

    TMUInt64 GetDuration() // return: nanoSeconds;
    {
        TMUInt64 n = 0;
        this->SendMessage(VM_GETDURATION, 0, (TMLParam)&n);
        return n;
    }

    void SetCurrentTime(TMUInt64 nNanoSeconds)
    {
        this->SendMessage(VM_SETTIME, 0,  (TMLParam)&nNanoSeconds);
    }

    TMUInt64 GetCurrentTime() // return: nanoSeconds;
    {
        TMUInt64 n = 0;
        this->SendMessage(VM_GETTIME, 0, (TMLParam)&n);
        return n;
    }

    void SetDefaultMuted() {}
    void GetDefaultMuted() {}

    void SetDefaultPlaybackRate() {}
    void GetDefaultPlaybackRate() {}


    TMBool GetEnded() // return: is end of video/audio
    {
        return this->SendMessage(VM_GETENDED, 0, 0);
    }

    void GetError() {}

    void SetLoop() {}
    void GetLoop() {}

    void SetMediaGroup() {}
    void GetMediaGroup() {}

    void GetNetworkState() {}

    void SetPaused() {}
    void GetPaused() {}

    void SetPlaybackRate() {}
    void GetPlaybackRate() {}

    void GetPlayed() {}

    void SetPreload() {}
    void GetPreLoad() {}

    void GetReadyState() {}
    void GetSeekable() {}
    void GetSeeking() {}

    void SetSrc(TMCStr pUrl)
    {
        this->SendMessage(VM_SETSRC, 0, (TMLParam)pUrl);
    }

    void SetSrcMoniker(IMoniker* pMoniker) // for cam
    {
        this->SendMessage(VM_SETSRCM, 0, (TMLParam)pMoniker);
    }

    void GetSrc(std::tstring& str)
    {
        CStrPtr ptr;
        this->SendMessage(VM_GETSRC, 0, (TMLParam)&ptr);
        ptr.swap(str);
    }

    void GetStartDate() {}
    void GetTextTracks() {}
    void GetVideoTracks() {}
    TMBool GetVideoSize(TMSize& sz, TMBool IsCalc_VS_X2Y) { this->SendMessage(VM_GETVIDEOSIZE, IsCalc_VS_X2Y, (TMLParam)&sz);}

    void SetMuted(TMBool bMute)
    {
        this->SendMessage(VM_SETMUTE, 0, bMute);
    }

    TMBool GetMuted()
    {
        return this->SendMessage(VM_GETMUTE, 0, 0);
    }

    float GetVolume() // 0 - 1.0f
    {
        float v;
        this->SendMessage(VM_GETVOLUME, 0, (TMLParam)&v);
        return v;
    }

    void SetVolume(float v) // 0 - 1.0f
    {
        this->SendMessage(VM_SETVOLUME, 0, (TMLParam)&v);
    }

//     void SetRepeat(int nRepeat)
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CACM_SETREPEAT, nRepeat, 0);
//     }
//
//     TMUInt GetRepeat()
//     {
//         DbgAssert(IsWindow());
//         return SendMessage(CACM_GETREPEAT, 0, 0);
//     }
//
//     void Play ()
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CACM_PLAY, 0, 0);
//     }
//
//     void Stop ()
//     {
//         DbgAssert(IsWindow());
//         SendMessage(CACM_STOP, 0, 0);
//     }
//
//     int Seek(int nPos)
//     {
//         DbgAssert(IsWindow());
//         return SendMessage(CACM_SEEK, nPos, 0L);
//     }
//
//     int SeekCur(int ndPos)
//     {
//         DbgAssert(IsWindow());
//         return SendMessage(CACM_SEEKCUR, nPos, 0L);
//     }
//
//     TMUInt GetPos() const
//     {
//         DbgAssert(IsWindow());
//         return (TMUInt)SendMessage(CACM_GETPOS, 0, 0L);
//     }
//
//     void GetRange(int* pBgn, int* pEnd) const
//     {
//         DbgAssert(IsWindow());
//         DbgAssert(pPBRange != NULL);
//         SendMessage(CACM_GETRANGE, (TMWParam)pBgn, (TMLParam)pEnd);
//     }
//
//     TMUInt SetRange(int nMin, int nMax)
//     {
//         DbgAssert(IsWindow());
//         return (TMUInt)SendMessage(CACM_SETRANGE, nMin, nMax);
//     }
};

typedef CTuiVideoT<CTuiWidget>                        CTuiVideo;


////////////////////////////////////////////////////////////////////////////////
// class CTuiDummy
// bind a TMHWnd to let it have the Layout feature
struct DUMMYINFO
{
    enum
    {
        MODE_CHILD      = 0x01,
        MODE_POP        = 0x02,
        FLAG_MASK       = 0x0f,
    };

    TMHWnd hWnd; // TMHWnd bind
    TMHWnd hOverlay;
    TMUInt32 nMode;
};

template < class TBase >
class CTuiDummyT : public TBase
{
public:
    CLASS_IMPL(CTuiDummyT, TUI_CLASS_DUMMY);

    void SetWindow(TMHWnd hWnd)
    {
        this->SendMessage(CDM_SETWINDOW, (TMWParam)hWnd);
    }

    TMHWnd GetWindow() const
    {
        return (TMHWnd)this->SendMessage(CM_GETWINDOW);
    }

    void GetInfo(DUMMYINFO& info) const
    {
        this->SendMessage(CDM_GETDUMMYINFO, 0, (TMLParam)&info);
    }
};

typedef CTuiDummyT < CTuiWidget >                       CTuiDummy;


////////////////////////////////////////////////////////////////////////////////
// class CTuiFloat
template < class TBase >
class CTuiFloatT : public TBase
{
public:
    CLASS_IMPL(CTuiFloatT, TUI_CLASS_FLOAT);

    TMHWnd GetWindow() const
    {
        return (TMHWnd)this->SendMessage(CM_GETWINDOW);
    }

    void SetAlpha(TMUInt8 alpha)
    {
        this->SendMessage(CM_SETALPHA, 0, alpha);
    }

    void SetInterval(int interval)
    {
        this->SendMessage(CM_SETINTERVAL, 0, interval);
    }

    GXPTexture GetBkTexture()
    {
        return (GXPTexture)this->SendMessage(CM_GET_BK_TEX, 0, 0);
    }
};

typedef CTuiFloatT < CTuiWidget >                       CTuiFloat;


////////////////////////////////////////////////////////////////////////////////
// class CTuiLayer
template <class TBase>
class CTuiLayerT : public TBase
{
public:
    CLASS_IMPL(CTuiLayerT, TUI_CLASS_LAYER);

    void SetAlpha(TMUInt8 alpha)
    {
        this->SendMessage(CM_SETALPHA, 0, alpha);
    }

    GXPTexture GetBkTexture()
    {
        return (GXPTexture)this->SendMessage(CM_GET_BK_TEX, 0, 0);
    }
};

typedef CTuiLayerT<CTuiWidget>                           CTuiLayer;


////////////////////////////////////////////////////////////////////////////////
// class CTuiArrow
template < class TBase >
class CTuiArrowT : public TBase
{
public:
    CLASS_IMPL(CTuiArrowT, TUI_CLASS_ARROW);

    void SetWindow(HTUI hWnd)
    {
        this->SendMessage(CM_SETBUDDY, (TMWParam)hWnd);
    }

    HTUI GetWindow() const
    {
        return (HTUI)this->SendMessage(CM_GETBUDDY);
    }
};

typedef CTuiArrowT < CTuiWidget >                       CTuiArrow;


#ifdef TM_WIN

////////////////////////////////////////////////////////////////////////////////
// class CTuiActiveX
// ActiveX widget
// webrowser: {8856F961-340A-11D0-A96B-00C04FD705A2}
template < class TBase >
class CTuiActiveXT : public TBase
{
public:
    CLASS_IMPL(CTuiActiveXT, TUI_CLASS_ACTIVEX);

    TMHWnd GetWindow() const
    {
        return (TMHWnd)this->SendMessage(CM_GETWINDOW);
    }

    TMBool BindObject(IUnknown* pObj)
    {
        return this->SendMessage(CAX_BINDOBJECT, (TMWParam)pObj, 0);
    }

    //add new interface support to host
    //when control query host's interface it calls QueryInterface of this interface directly
    TMBool SetExtUnkown(IUnknown* pUnk)
    {
        return this->SendMessage(CAX_SETEXT, (TMWParam)pUnk, 0);
    }

    IUnknown* GetHost() const
    {
        return (IUnknown*)this->SendMessage(CAX_GETHOST);
    }

    HRESULT QueryHost(REFIID iid, void** ppUnk)
    {
        DbgAssert(ppUnk != NULL);
        HRESULT hr = E_NOTIMPL;
        IUnknown* spUnk = GetHost();
        if (spUnk)
            hr = spUnk->QueryInterface(iid, ppUnk);
        return hr;
    }

    template <class Q>
        HRESULT QueryHost(Q** ppUnk)
    {
        return QueryHost(__uuidof(Q), (void**)ppUnk);
    }

    IUnknown* GetControl() const
    {
        return (IUnknown*)this->SendMessage(CAX_GETCONTROL);
    }

    HRESULT QueryControl(REFIID iid, void** ppUnk)
    {
        DbgAssert(ppUnk != NULL);
        HRESULT hr = E_NOTIMPL;
        *ppUnk = NULL;
        IUnknown* spUnk = GetControl();
        if (spUnk)
            hr = spUnk->QueryInterface(iid, ppUnk);
        return hr;
    }

    template <class Q>
        HRESULT QueryControl(Q** ppUnk)
    {
        return QueryControl(__uuidof(Q), (void**)ppUnk);
    }
};

typedef CTuiActiveXT < CTuiWidget >                     CTuiActiveX;


////////////////////////////////////////////////////////////////////////////////
// class CTuiWebView
// help: https://docs.microsoft.com/en-us/dotnet/api/microsoft.web.webview2.winforms.webview2?view=WebView2-dotnet-1.0.674-prerelease#executescriptasync

// style
#define WVS_NOCHROME                            0x0002
#define WVS_NOIE                                0x0004

#define WVS_EXTNOSCRIPT                         0x0008
#define WVS_EXTENDMODE                          0x0010
#define WVS_EXTNOCONTEXTMENU                    0x0020 // the flag will send WM_CONTEXTMENU message;

#define WVS_PRIVATEMODE                         0x0100 // chrome: enable private mode


template <class TBase>
class CTuiWebViewT : public TBase
{
public:
    CLASS_IMPL(CTuiWebViewT, TUI_CLASS_WEBVIEW);

//     void Recreate()
//     {
//         this->SendMessage(CWV_RECREATE);
//     }

    TMHWnd GetWindow() const
    {
        return (TMHWnd)this->SendMessage(CM_GETWINDOW);
    }

    IUnknown* GetControl() const
    {
        return (IUnknown*)this->SendMessage(CWV_GETCONTROL);
    }

    HRESULT QueryControl(REFIID iid, void** ppUnk)
    {
        DbgAssert(ppUnk != NULL);
        HRESULT hr = E_NOTIMPL;
        *ppUnk = NULL;
        IUnknown* spUnk = GetControl();
        if (spUnk)
            hr = spUnk->QueryInterface(iid, ppUnk);
        return hr;
    }

    // Q:
    // ICoreWebView2Environment
    // ICoreWebView2Controller,
    // ICoreWebView2
    // IWebBrowser2
    // query another interface like ICoreWebView2_4:
    //   QueryControl(ICoreWebView2) first, then call ICoreWebView2::QueryIntface(ICoreWebView2_4);
    template <class Q>
        HRESULT QueryControl(Q** ppUnk)
    {
        return QueryControl(__uuidof(Q), (void**)ppUnk);
    }

    //add new interface support to host
    // when control query host's interface it calls QueryInterface of this interface directly
    // pUnk: IDocHostUIHandler, IDocHostShowUI
    TMBool SetExtUnkown(IUnknown* pUnk)
    {
        return this->SendMessage(CWV_SETEXT, (TMWParam)pUnk, 0);
    }

    // 1 means 100%
    TMBool SetZoomFactor(double nScale)
    {
        return this->SendMessage(CWV_SETZOOM, 0, (TMLParam)&nScale);
    }

    double GetZoomFactor()
    {
        double x = 1.0;
        this->SendMessage(CWV_GETZOOM, 0, (TMLParam)&x);
        return x;
    }


    //         virtual HRESULT __stdcall get_Settings (
    //         struct ICoreWebView2Settings * * Settings) = 0;
    TMBool GetSource(std::tstring& str)
    {
        CStrPtr ptr;
        TMBool b = this->SendMessage(CWV_GETSOUCE, 0, (TMLParam)&ptr);
        ptr.swap(str);
        return b;
    }

    TMBool GetSource(CString& str)
    {
        CStrPtr ptr;
        TMBool b = this->SendMessage(CWV_GETSOUCE, 0, (TMLParam)&ptr);
        str = std::move(ptr);
        return b;
    }

    TMBool Navigate(LPCWSTR uri, LPCWSTR pHead = 0)
    {
        return this->SendMessage(CWV_NAVIGATE, (TMWParam)uri, (TMLParam)pHead);
    }

    TMBool NavigateToString(LPCWSTR htmlContent, int nLen = -1)
    {
        return this->SendMessage(CWV_NAVIGATETOSTRING, 0, (TMLParam)htmlContent);
    }


//     virtual HRESULT __stdcall raw_add_NavigationStarting (
//         struct ICoreWebView2NavigationStartingEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_NavigationStarting (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall raw_add_ContentLoading (
//         struct ICoreWebView2ContentLoadingEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_ContentLoading (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall raw_add_SourceChanged (
//         struct ICoreWebView2SourceChangedEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_SourceChanged (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall raw_add_HistoryChanged (
//         struct ICoreWebView2HistoryChangedEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_HistoryChanged (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall raw_add_NavigationCompleted (
//         struct ICoreWebView2NavigationCompletedEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_NavigationCompleted (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall raw_add_FrameNavigationStarting (
//         struct ICoreWebView2NavigationStartingEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_FrameNavigationStarting (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall raw_add_FrameNavigationCompleted (
//         struct ICoreWebView2NavigationCompletedEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_FrameNavigationCompleted (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall raw_add_ScriptDialogOpening (
//         struct ICoreWebView2ScriptDialogOpeningEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_ScriptDialogOpening (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall raw_add_PermissionRequested (
//         struct ICoreWebView2PermissionRequestedEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_PermissionRequested (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall raw_add_ProcessFailed (
//         struct ICoreWebView2ProcessFailedEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_ProcessFailed (
//         struct EventRegistrationToken token) = 0;

    // only support extend mode
    TMBool AddScriptToExecuteOnDocumentCreated(LPWSTR javaScript)
    {
        DbgAssert(this->GetStyle() & WVS_EXTENDMODE);
        return this->SendMessage(CWV_ADDDOCSCRIPT, 0, (TMLParam)javaScript);
    }

//     virtual HRESULT __stdcall raw_AddScriptToExecuteOnDocumentCreated (
//         LPWSTR javaScript,
//         struct ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler * handler) = 0;
//     virtual HRESULT __stdcall raw_RemoveScriptToExecuteOnDocumentCreated (
//         LPWSTR id) = 0;

     TMBool ExecuteScript(LPCWSTR javaScript)
     {
         return this->SendMessage(CWV_EXECSCRIPT, 0, (TMLParam)javaScript);
     }

//     virtual HRESULT __stdcall raw_CapturePreview (
//         enum COREWEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT imageFormat,
//         struct IStream * imageStream,
//         struct ICoreWebView2CapturePreviewCompletedHandler * handler) = 0;

     TMBool Reload()
     {
         return this->SendMessage(CWV_RELOAD, 0, 0);
     }

//     virtual HRESULT __stdcall raw_PostWebMessageAsJson (
//         LPWSTR webMessageAsJson) = 0;
//     virtual HRESULT __stdcall raw_PostWebMessageAsString (
//         LPWSTR webMessageAsString) = 0;
//     virtual HRESULT __stdcall raw_add_WebMessageReceived (
//         struct ICoreWebView2WebMessageReceivedEventHandler * handler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_WebMessageReceived (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall raw_CallDevToolsProtocolMethod (
//         LPWSTR methodName,
//         LPWSTR parametersAsJson,
//         struct ICoreWebView2CallDevToolsProtocolMethodCompletedHandler * handler) = 0;

     int GetBrowserProcessId()
     {
         return this->SendMessage(CWV_GETPID, 0, 0);
     }

     int CanGoBack()
     {
         return this->SendMessage(CWV_CANGOBACK, 0, 0);
     }

     int CanGoForward()
     {
         return this->SendMessage(CWV_CANGOFORWARD, 0, 0);
     }

     TMBool GoBack()
     {
         return this->SendMessage(CWV_GOBACK, 0, 0);
     }

     TMBool GoForward()
     {
         return this->SendMessage(CWV_GOFORWARD, 0, 0);
     }

//     virtual HRESULT __stdcall raw_GetDevToolsProtocolEventReceiver (
//         LPWSTR eventName,
//         struct ICoreWebView2DevToolsProtocolEventReceiver * * receiver) = 0;

     void Stop()
     {
         this->SendMessage(CWV_STOP, 0, 0);
     }

//     virtual HRESULT __stdcall raw_add_NewWindowRequested (
//         struct ICoreWebView2NewWindowRequestedEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_NewWindowRequested (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall raw_add_DocumentTitleChanged (
//         struct ICoreWebView2DocumentTitleChangedEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_DocumentTitleChanged (
//         struct EventRegistrationToken token) = 0;

     TMBool GetDocumentTitle(std::tstring& str)
     {
         CStrPtr ptr;
         TMBool b = this->SendMessage(CWV_GETTITLE, 0, (TMLParam)&ptr);
         ptr.swap(str);
         return b;
     }

     TMBool GetDocumentTitle(CString& str)
     {
         CStrPtr ptr;
         TMBool b = this->SendMessage(CWV_GETTITLE, 0, (TMLParam)&ptr);
         str = std::move(ptr);
         return b;
     }

//     virtual HRESULT __stdcall raw_AddHostObjectToScript (
//         LPWSTR name,
//         VARIANT * object) = 0;
//     virtual HRESULT __stdcall raw_RemoveHostObjectFromScript (
//         LPWSTR name) = 0;
//     virtual HRESULT __stdcall raw_OpenDevToolsWindow () = 0;
//     virtual HRESULT __stdcall raw_add_ContainsFullScreenElementChanged (
//         struct ICoreWebView2ContainsFullScreenElementChangedEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_ContainsFullScreenElementChanged (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall get_ContainsFullScreenElement (
//         long * ContainsFullScreenElement) = 0;
//     virtual HRESULT __stdcall raw_add_WebResourceRequested (
//         struct ICoreWebView2WebResourceRequestedEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_WebResourceRequested (
//         struct EventRegistrationToken token) = 0;
//     virtual HRESULT __stdcall raw_AddWebResourceRequestedFilter (
//         LPWSTR uri,
//         enum COREWEBVIEW2_WEB_RESOURCE_CONTEXT ResourceContext) = 0;
//     virtual HRESULT __stdcall raw_RemoveWebResourceRequestedFilter (
//         LPWSTR uri,
//         enum COREWEBVIEW2_WEB_RESOURCE_CONTEXT ResourceContext) = 0;
//     virtual HRESULT __stdcall raw_add_WindowCloseRequested (
//         struct ICoreWebView2WindowCloseRequestedEventHandler * eventHandler,
//         struct EventRegistrationToken * token) = 0;
//     virtual HRESULT __stdcall raw_remove_WindowCloseRequested (
//         struct EventRegistrationToken token) = 0;

     void EnableScript(TMBool bEnable)
     {
         if (bEnable)
            this->ModifyStyle(WVS_EXTNOSCRIPT, 0);
         else
            this->ModifyStyle(0, WVS_EXTNOSCRIPT);
     }


};

typedef CTuiWebViewT<CTuiWidget>                        CTuiWebView;




#endif // TM_WIN


#endif // TM_WIN_DESKTOP


TUI_END_NAMESPACE


#endif // TMDUI_H
