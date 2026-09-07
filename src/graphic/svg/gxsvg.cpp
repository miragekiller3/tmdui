/*
 *  @file
 *  @brief
 *      html5 canvas2d interface
 *      https://dev.w3.org/html5/2dcontext-LC/
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.11.04
 *
 *  Copyright (C) 2019 miragekiller
 */

#include "gxsvg.h"
#include "../../tmcolor.h"



#if TM_MSC < 1300
#   define strtoll strtol
#endif
#define NSVGLONGLONG TMInt64


#include "gxsvgdom.cpp"

#define malloc(x)           MemAlloc(x,void)
#define realloc(x,y)        MemRealloc(x,y,void)
#define free                MemFree


#define NANOSVG_IMPLEMENTATION
#define NANOSVG_ALL_COLOR_KEYWORDS
#include "gxsvgparse.cpp"


#define NANOSVGRAST_IMPLEMENTATION
#include "gxsvgrast.cpp"

#undef NANOSVG_IMPLEMENTATION
#undef NANOSVG_ALL_COLOR_KEYWORDS
#undef NANOSVGRAST_IMPLEMENTATION
#undef free
#undef realloc
#undef malloc






