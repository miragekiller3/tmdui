#ifndef HEADER_GLOBAL_H
#define HEADER_GLOBAL_H

#include "tmdef.h"


#ifdef TM_QT
#   include <QtCore/qglobal.h>
#   define _HEADER_DECL_EXPORT Q_DECL_EXPORT
#   define _HEADER_DECL_IMPORT Q_DECL_IMPORT
#else
#   define _HEADER_DECL_EXPORT __declspec(dllexport)
#   define _HEADER_DECL_IMPORT
#endif // TM_QT


#if defined(HEADER_EXPORT)
#  define HEADER_API _HEADER_DECL_EXPORT
#else
#  define HEADER_API _HEADER_DECL_IMPORT
#endif


#endif // HEADER_GLOBAL_H
