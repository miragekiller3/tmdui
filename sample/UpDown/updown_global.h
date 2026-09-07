#ifndef UPDOWN_GLOBAL_H
#define UPDOWN_GLOBAL_H

#include "tmdef.h"


#ifdef TM_QT
#   include <QtCore/qglobal.h>
#   define _UPDOWN_DECL_EXPORT Q_DECL_EXPORT
#   define _UPDOWN_DECL_IMPORT Q_DECL_IMPORT
#else
#   define _UPDOWN_DECL_EXPORT __declspec(dllexport)
#   define _UPDOWN_DECL_IMPORT
#endif // TM_QT


#if defined(UPDOWN_EXPORT)
#  define UPDOWN_API _UPDOWN_DECL_EXPORT
#else
#  define UPDOWN_API _UPDOWN_DECL_IMPORT
#endif


#endif // UPDOWN_GLOBAL_H
