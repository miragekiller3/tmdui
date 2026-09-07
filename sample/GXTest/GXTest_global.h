#ifndef GXTEST_GLOBAL_H
#define GXTEST_GLOBAL_H

#include "tmdef.h"


#ifdef TM_QT
#   include <QtCore/qglobal.h>
#   define _GXTEST_DECL_EXPORT Q_DECL_EXPORT
#   define _GXTEST_DECL_IMPORT Q_DECL_IMPORT
#else
#   define _GXTEST_DECL_EXPORT __declspec(dllexport)
#   define _GXTEST_DECL_IMPORT
#endif // TM_QT


#if defined(GXTEST_EXPORT)
#  define GXTEST_API _GXTEST_DECL_EXPORT
#else
#  define GXTEST_API _GXTEST_DECL_IMPORT
#endif


#endif // GXTEST_GLOBAL_H
