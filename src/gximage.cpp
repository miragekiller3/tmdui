/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2023.6.1
 *
 *  Copyright (C) 2023 miragekiller
 */

#include "tmdef.h"
#include "tmgdi.h"

#ifdef TM_QT
#   include "graphic/gximage_qt.cpp"
#elif defined(TM_WIN)
#   include "graphic/gximage_win.cpp"
#elif defined(TM_APPLE)
#   include "graphic/gximage_mac.cpp"
#elif defined(TM_GTK)
#   include "graphic/gximage_gtk.cpp"
#endif

#ifdef TM_WIN
#   include "graphic/svg/gxsvg.cpp"
#endif // TM_QT


