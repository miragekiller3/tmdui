/*
 *  @file
 *  @brief
 *  gdix unit support
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.4.23
 *
 *  compatibility: c++, windows, mac os X, unix, c++
 *
 *  Copyright (C) 2010 miragekiller
 *  All rights reserved.
 */

#include "stdafx.h"

// #include "tmdef.h"
// #include "tmgdix.h"

#include "gximage.cpp"
#include "graphic/gxapp.cpp"


#ifdef TM_RENDER_GDI
#elif defined(TM_APPLE)
// #   include "gdix/gximage_mac.cpp"
#elif TM_WIN_METRO

#   define TM_RENDER_D2D 1

#   include "tmgdi.h"
//#   include "gdix/gximage_metro.cpp"
#   include "graphic/gximage_metro.cpp"
#   include "graphic/canvas_d2d.cpp"
#endif

//#ifndef TM_QT
//#   include "svg/gxsvg.cpp"
//#endif // TM_QT


//#include "gdix/gxlines.cpp"
//#include "GdiX/canvas.cpp"
//#include "GdiX/canvas_gdip.cpp"
//#include "GdiX/canvas_gl.cpp"

