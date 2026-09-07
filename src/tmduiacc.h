/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.08.25
 *
 *  Copyright (C) 2018 miragekiller
 */

#pragma once

#ifndef TMDUIACC_H
#define TMDUIACC_H

#include "tmdui.h"
#include <oleacc.h>


///////////////////////////////////////////////////////////////////////////////
// ROLE_SYSTEM_XXX
#ifndef ROLE_SYSTEM_TITLEBAR
#   define ROLE_SYSTEM_TITLEBAR            0x00000001
#   define ROLE_SYSTEM_MENUBAR             0x00000002
#   define ROLE_SYSTEM_SCROLLBAR           0x00000003
#   define ROLE_SYSTEM_GRIP                0x00000004
#   define ROLE_SYSTEM_SOUND               0x00000005
#   define ROLE_SYSTEM_CURSOR              0x00000006
#   define ROLE_SYSTEM_CARET               0x00000007
#   define ROLE_SYSTEM_ALERT               0x00000008
#   define ROLE_SYSTEM_WINDOW              0x00000009
#   define ROLE_SYSTEM_CLIENT              0x0000000A
#   define ROLE_SYSTEM_MENUPOPUP           0x0000000B
#   define ROLE_SYSTEM_MENUITEM            0x0000000C
#   define ROLE_SYSTEM_TOOLTIP             0x0000000D
#   define ROLE_SYSTEM_APPLICATION         0x0000000E
#   define ROLE_SYSTEM_DOCUMENT            0x0000000F
#   define ROLE_SYSTEM_PANE                0x00000010
#   define ROLE_SYSTEM_CHART               0x00000011
#   define ROLE_SYSTEM_DIALOG              0x00000012
#   define ROLE_SYSTEM_BORDER              0x00000013
#   define ROLE_SYSTEM_GROUPING            0x00000014
#   define ROLE_SYSTEM_SEPARATOR           0x00000015
#   define ROLE_SYSTEM_TOOLBAR             0x00000016
#   define ROLE_SYSTEM_STATUSBAR           0x00000017
#   define ROLE_SYSTEM_TABLE               0x00000018
#   define ROLE_SYSTEM_COLUMNHEADER        0x00000019
#   define ROLE_SYSTEM_ROWHEADER           0x0000001A
#   define ROLE_SYSTEM_COLUMN              0x0000001B
#   define ROLE_SYSTEM_ROW                 0x0000001C
#   define ROLE_SYSTEM_CELL                0x0000001D
#   define ROLE_SYSTEM_LINK                0x0000001E
#   define ROLE_SYSTEM_HELPBALLOON         0x0000001F
#   define ROLE_SYSTEM_CHARACTER           0x00000020
#   define ROLE_SYSTEM_LIST                0x00000021
#   define ROLE_SYSTEM_LISTITEM            0x00000022
#   define ROLE_SYSTEM_OUTLINE             0x00000023
#   define ROLE_SYSTEM_OUTLINEITEM         0x00000024
#   define ROLE_SYSTEM_PAGETAB             0x00000025
#   define ROLE_SYSTEM_PROPERTYPAGE        0x00000026
#   define ROLE_SYSTEM_INDICATOR           0x00000027
#   define ROLE_SYSTEM_GRAPHIC             0x00000028
#   define ROLE_SYSTEM_STATICTEXT          0x00000029
#   define ROLE_SYSTEM_TEXT                0x0000002A  // Editable, selectable, etc.
#   define ROLE_SYSTEM_PUSHBUTTON          0x0000002B
#   define ROLE_SYSTEM_CHECKBUTTON         0x0000002C
#   define ROLE_SYSTEM_RADIOBUTTON         0x0000002D
#   define ROLE_SYSTEM_COMBOBOX            0x0000002E
#   define ROLE_SYSTEM_DROPLIST            0x0000002F
#   define ROLE_SYSTEM_PROGRESSBAR         0x00000030
#   define ROLE_SYSTEM_DIAL                0x00000031
#   define ROLE_SYSTEM_HOTKEYFIELD         0x00000032
#   define ROLE_SYSTEM_SLIDER              0x00000033
#   define ROLE_SYSTEM_SPINBUTTON          0x00000034
#   define ROLE_SYSTEM_DIAGRAM             0x00000035
#   define ROLE_SYSTEM_ANIMATION           0x00000036
#   define ROLE_SYSTEM_EQUATION            0x00000037
#   define ROLE_SYSTEM_BUTTONDROPDOWN      0x00000038
#   define ROLE_SYSTEM_BUTTONMENU          0x00000039
#   define ROLE_SYSTEM_BUTTONDROPDOWNGRID  0x0000003A
#   define ROLE_SYSTEM_WHITESPACE          0x0000003B
#   define ROLE_SYSTEM_PAGETABLIST         0x0000003C
#   define ROLE_SYSTEM_CLOCK               0x0000003D
#endif // ROLE_SYSTEM_TITLEBAR



// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the TMDuiAcc_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// TUIPROXY_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef TMDuiAcc_EXPORTS
#define TUIPROXY_API __declspec(dllexport)
#else
#define TUIPROXY_API //__declspec(dllimport)
#endif


#if defined (TUIPROXY_LIB) || defined(TMDuiAcc_EXPORTS)
#   define LibGetAccObject           GetAccObject
#   define LibGetAccObjectQT         GetAccObjectQT
#else
#   define LibGetAccObject(a,b,c,d)  false
#   define LibGetAccObjectQT(a)      0
#endif

#define CHILDID_GETOBJECTID             -1 // get_accDescription(CHILDID_GETOBJECTID) return oid/cid
#define CHILDID_GETOID                  -2 // get_accDescription(CHILDID_GETOBJECTID) return oid
#define CHILDID_GETCID                  -3 // get_accDescription(CHILDID_GETOBJECTID) return cid
#define CHILDID_GETHTUI                 -4 // get_accDescription(CHILDID_GETOBJECTID) return cid



///////////////////////////////////////////////////////////////////////////////
// export function
extern "C"
{
// This class is exported from the TMDuiAcc.dll

#ifdef TM_QT

    class QAccessibleInterface;
    typedef QAccessibleInterface* (*fnGetAccObjectQT)(TM::HTUI hWgt);
    TUIPROXY_API QAccessibleInterface* GetAccObjectQT(TM::HTUI hWgt);

#endif

    typedef TMBool(*fnGetAccObject)(TM::HTUI hWgt, TMWParam wParam, TMLParam lParam, TMResult& lr);
    TUIPROXY_API TMBool GetAccObject(TM::HTUI hWgt, TMWParam wParam, TMLParam lParam, TMResult& lr);
}

#endif // ACC
