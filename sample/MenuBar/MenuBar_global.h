#ifndef MENUBAR_GLOBAL_H
#define MENUBAR_GLOBAL_H

#include "tmdef.h"


#ifdef TM_QT
#   include <QtCore/qglobal.h>
#   define _MENUBAR_DECL_EXPORT Q_DECL_EXPORT
#   define _MENUBAR_DECL_IMPORT Q_DECL_IMPORT
#else
#   define _MENUBAR_DECL_EXPORT __declspec(dllexport)
#   define _MENUBAR_DECL_IMPORT
#endif // TM_QT


#if defined(MENUBAR_EXPORT)
#  define MENUBAR_API _MENUBAR_DECL_EXPORT
#else
#  define MENUBAR_API _MENUBAR_DECL_IMPORT
#endif


#endif // MENUBAR_GLOBAL_H
