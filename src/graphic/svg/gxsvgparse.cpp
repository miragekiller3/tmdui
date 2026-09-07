/*
 * Copyright (c) 2013-14 Mikko Mononen memon@inside.org
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * The SVG parser is based on Anti-Grain Geometry 2.4 SVG example
 * Copyright (C) 2002-2004 Maxim Shemanarev (McSeem) (http://www.antigrain.com/)
 *
 * Arc calculation code based on canvg (https://code.google.com/p/canvg/)
 *
 * Bounding box calculation based on http://blog.hackers-cafe.net/2009/06/how-to-calculate-bezier-curves-bounding.html
 *
 */

// https://github.com/memononen/nanosvg.git

#include "../../tmxml.h"
#include "../../tmdebugext.h"
#include "gxsvgparse.h"

#ifndef NANOSVG_H
#define NANOSVG_H

TM_BGN_NAMESPACE


#ifndef NSVGLONGLONG
#   define NSVGLONGLONG long long
#endif

#define SVG_BGN_ATTR(attrs) \
    tm_for (int i = 0; i < nAttr; ++i) { \
    const TM::CXmlTag& aval = attrs[i].m_val; \
    const TM::CSvgDomAttrib& attr = attrs[i]; \
    ESvgAttrib atype = attr.m_eName;

#define SVG_END_ATTR()  else if (ParseAttr(attr)) return; }

#define SVG_END_ATTR2()  }

#define SVG_DECALRE_ARRRY(str,array,nMax)     tm_setmin(nMax, str.length() / 2);  CSvgNum* array = (CSvgNum*)alloca(nMax * sizeof(CSvgNum));


#ifndef NANOSVG_CPLUSPLUS
#ifdef __cplusplus
extern "C" {
#endif
#endif

// NanoSVG is a simple stupid single-header-file SVG parse. The output of the parser is a list of cubic bezier shapes.
//
// The library suits well for anything from rendering scalable icons in your editor application to prototyping a game.
//
// NanoSVG supports a wide range of SVG features, but something may be missing, feel free to create a pull request!
//
// The shapes in the SVG images are transformed by the viewBox and converted to specified units.
// That is, you should get the same looking data as your designed in your favorite app.
//
// NanoSVG can return the paths in few different units. For example if you want to render an image, you may choose
// to get the paths in pixels, or if you are feeding the data into a CNC-cutter, you may want to use millimeters.
//
// The units passed to NanoSVG should be one of: 'px', 'pt', 'pc' 'mm', 'cm', or 'in'.
// DPI (dots-per-inch) controls how the unit conversion is done.
//
// If you don't know or care about the units stuff, "px" and 96 should get you going.


/* Example Usage:
    // Load SVG
    NSVGimage* image;
    image = nsvgParseFromFile("test.svg", "px", 96);
    printf("size: %f x %f\n", image->width, image->height);
    // Use...
    for (NSVGshape *shape = image->shapes; shape != NULL; shape = shape->next) {
        for (NSVGpath *path = shape->paths; path != NULL; path = path->next) {
            for (int i = 0; i < path->npts-1; i += 3) {
                float* p = &path->pts[i*2];
                drawCubicBez(p[0],p[1], p[2],p[3], p[4],p[5], p[6],p[7]);
            }
        }
    }
    // Delete
    nsvgDelete(image);
*/
/*
enum NSVGpaintType {
    NSVG_PAINT_UNDEF = -1,
    NSVG_PAINT_NONE = 0,
    NSVG_PAINT_COLOR = 1,
    NSVG_PAINT_LINEAR_GRADIENT = 2,
    NSVG_PAINT_RADIAL_GRADIENT = 3
};

enum NSVGspreadType {
    NSVG_SPREAD_PAD = 0,
    NSVG_SPREAD_REFLECT = 1,
    NSVG_SPREAD_REPEAT = 2
};

enum NSVGlineJoin {
    NSVG_JOIN_MITER = 0,
    NSVG_JOIN_ROUND = 1,
    NSVG_JOIN_BEVEL = 2
};

enum NSVGlineCap {
    NSVG_CAP_BUTT = 0,
    NSVG_CAP_ROUND = 1,
    NSVG_CAP_SQUARE = 2
};

enum NSVGfillRule {
    NSVG_FILLRULE_NONZERO = 0,
    NSVG_FILLRULE_EVENODD = 1
};

enum NSVGflags {
    NSVG_FLAGS_VISIBLE = 0x01
};

typedef struct NSVGgradientStop {
    unsigned int color;
    float offset;
} NSVGgradientStop;

typedef struct NSVGgradient {
    float xform[6];
    char spread;
    float fx, fy;
    int nstops;
    NSVGgradientStop stops[1];
} NSVGgradient;

typedef struct NSVGpaint {
    signed char type;
    union {
        unsigned int color;
        NSVGgradient* gradient;
    };
} NSVGpaint;

typedef struct NSVGpath
{
    float* pts;                 // Cubic bezier points: x0,y0, [cpx1,cpx1,cpx2,cpy2,x1,y1], ...
    int npts;                   // Total number of bezier points.
    char closed;                // Flag indicating if shapes should be treated as closed.
    float bounds[4];            // Tight bounding box of the shape [minx,miny,maxx,maxy].
    struct NSVGpath* next;      // Pointer to next path, or NULL if last element.
} NSVGpath;

typedef struct NSVGshape
{
    char id[64];                // Optional 'id' attr of the shape or its group
    NSVGpaint fill;             // Fill paint
    NSVGpaint stroke;           // Stroke paint
    float opacity;              // Opacity of the shape.
    float strokeWidth;          // Stroke width (scaled).
    float strokeDashOffset;     // Stroke dash offset (scaled).
    float strokeDashArray[8];           // Stroke dash array (scaled).
    char strokeDashCount;               // Number of dash values in dash array.
    char strokeLineJoin;        // Stroke join type.
    char strokeLineCap;         // Stroke cap type.
    float miterLimit;           // Miter limit
    char fillRule;              // Fill rule, see NSVGfillRule.
    unsigned char flags;        // Logical or of NSVG_FLAGS_* flags
    float bounds[4];            // Tight bounding box of the shape [minx,miny,maxx,maxy].
    char fillGradient[64];		// Optional 'id' of fill gradient
    char strokeGradient[64];	// Optional 'id' of stroke gradient
    float xform[6];				// Root transformation for fill/stroke gradient
    NSVGpath* paths;            // Linked list of paths in the image.
    struct NSVGshape* next;     // Pointer to next shape, or NULL if last element.
} NSVGshape;

typedef struct NSVGimage
{
    float width;                // Width of the image.
    float height;               // Height of the image.
    NSVGshape* shapes;          // Linked list of shapes in the image.
} NSVGimage;

// Parses SVG file from a file, returns SVG image as paths.
NSVGimage* nsvgParseFromFile(const char* filename, const char* units, float dpi);

// Parses SVG file from a null terminated string, returns SVG image as paths.
// Important note: changes the string.
NSVGimage* nsvgParse(char* input, const char* units, float dpi);

// Duplicates a path.
NSVGpath* nsvgDuplicatePath(NSVGpath* p);

// Deletes an image.
void nsvgDelete(NSVGimage* image);
*/
#ifndef NANOSVG_CPLUSPLUS
#ifdef __cplusplus
}
#endif
#endif

#endif // NANOSVG_H

#ifdef NANOSVG_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>
#include <math.h>

#define NSVG_PI (3.14159265358979323846264338327f)
#define NSVG_KAPPA90 (0.5522847493f)    // Length proportional to radius of a cubic bezier handle for 90deg arcs.

#define NSVG_ALIGN_MIN 0
#define NSVG_ALIGN_MID 1
#define NSVG_ALIGN_MAX 2
#define NSVG_ALIGN_NONE 0
#define NSVG_ALIGN_MEET 1
#define NSVG_ALIGN_SLICE 2

// #define NSVG_NOTUSED(v) do { (void)(1 ? (void)0 : ((void)(v))); } while(0)
// #define NSVG_RGB(r, g, b) (((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16))

#ifdef _MSC_VER
    #pragma warning (disable: 4996) // Switch off security warnings
    #pragma warning (disable: 4100) // Switch off unreferenced formal parameter warnings
    #ifdef __cplusplus
    #define NSVG_INLINE inline
    #else
    #define NSVG_INLINE
    #endif
#else
    #define NSVG_INLINE inline
#endif


// static int nsvg__isspace(char c)
// {
//  return strchr(" \t\n\v\f\r", c) != 0;
// }
// 
// static int nsvg__isdigit(char c)
// {
//  return c >= '0' && c <= '9';
// }

static NSVG_INLINE float nsvg__minf(float a, float b) { return a < b ? a : b; }
static NSVG_INLINE float nsvg__maxf(float a, float b) { return a > b ? a : b; }

/*
// Simple XML parser

#define NSVG_XML_TAG 1
#define NSVG_XML_CONTENT 2
#define NSVG_XML_MAX_ATTRIBS 256

static void nsvg__parseContent(char* s,
                               void (*contentCb)(void* ud, const char* s),
                               void* ud)
{
    // Trim start white spaces
    while (*s && nsvg__isspace(*s)) s++;
    if (!*s) return;

    if (contentCb)
        (*contentCb)(ud, s);
}

static void nsvg__parseElement(char* s,
                               void (*startelCb)(void* ud, const char* el, const char** attr),
                               void (*endelCb)(void* ud, const char* el),
                               void* ud)
{
    const char* attr[NSVG_XML_MAX_ATTRIBS];
    int nattr = 0;
    char* name;
    int start = 0;
    int end = 0;
    char quote;

    // Skip white space after the '<'
    while (*s && nsvg__isspace(*s)) s++;

    // Check if the tag is end tag
    if (*s == '/') {
        s++;
        end = 1;
    } else {
        start = 1;
    }

    // Skip comments, data and preprocessor stuff.
    if (!*s || *s == '?' || *s == '!')
        return;

    // Get tag name
    name = s;
    while (*s && !nsvg__isspace(*s)) s++;
    if (*s) { *s++ = '\0'; }

    // Get attribs
    while (!end && *s && nattr < NSVG_XML_MAX_ATTRIBS-3) {
        char* name = NULL;
        char* value = NULL;

        // Skip white space before the attrib name
        while (*s && nsvg__isspace(*s)) s++;
        if (!*s) break;
        if (*s == '/') {
            end = 1;
            break;
        }
        name = s;
        // Find end of the attrib name.
        while (*s && !nsvg__isspace(*s) && *s != '=') s++;
        if (*s) { *s++ = '\0'; }
        // Skip until the beginning of the value.
        while (*s && *s != '\"' && *s != '\'') s++;
        if (!*s) break;
        quote = *s;
        s++;
        // Store value and find the end of it.
        value = s;
        while (*s && *s != quote) s++;
        if (*s) { *s++ = '\0'; }

        // Store only well formed attributes
        if (name && value) {
            attr[nattr++] = name;
            attr[nattr++] = value;
        }
    }

    // List terminator
    attr[nattr++] = 0;
    attr[nattr++] = 0;

    // Call callbacks.
    if (start && startelCb)
        (*startelCb)(ud, name, attr);
    if (end && endelCb)
        (*endelCb)(ud, name);
}

int nsvg__parseXML(char* input,
                   void (*startelCb)(void* ud, const char* el, const char** attr),
                   void (*endelCb)(void* ud, const char* el),
                   void (*contentCb)(void* ud, const char* s),
                   void* ud)
{
    char* s = input;
    char* mark = s;
    int state = NSVG_XML_CONTENT;
    while (*s) {
        if (*s == '<' && state == NSVG_XML_CONTENT) {
            // Start of a tag
            *s++ = '\0';
            nsvg__parseContent(mark, contentCb, ud);
            mark = s;
            state = NSVG_XML_TAG;
        } else if (*s == '>' && state == NSVG_XML_TAG) {
            // Start of a content or new tag.
            *s++ = '\0';
            nsvg__parseElement(mark, startelCb, endelCb, ud);
            mark = s;
            state = NSVG_XML_CONTENT;
        } else {
            s++;
        }
    }

    return 1;
}


/* Simple SVG parser. * /

#define NSVG_MAX_ATTR 128

enum NSVGgradientUnits {
    NSVG_USER_SPACE = 0,
    NSVG_OBJECT_SPACE = 1
};

#define NSVG_MAX_DASHES 8

enum NSVGunits {
    NSVG_UNITS_USER,
    NSVG_UNITS_PX,
    NSVG_UNITS_PT,
    NSVG_UNITS_PC,
    NSVG_UNITS_MM,
    NSVG_UNITS_CM,
    NSVG_UNITS_IN,
    NSVG_UNITS_PERCENT,
    NSVG_UNITS_EM,
    NSVG_UNITS_EX
};

typedef struct NSVGcoordinate {
    float value;
    int units;
} NSVGcoordinate;

typedef struct NSVGlinearData {
    NSVGcoordinate x1, y1, x2, y2;
} NSVGlinearData;

typedef struct NSVGradialData {
    NSVGcoordinate cx, cy, r, fx, fy;
} NSVGradialData;

typedef struct NSVGgradientData
{
    char id[64];
    char ref[64];
	signed char type;
    union {
        NSVGlinearData linear;
        NSVGradialData radial;
    };
    char spread;
    char units;
    float xform[6];
    int nstops;
    NSVGgradientStop* stops;
    struct NSVGgradientData* next;
} NSVGgradientData;

typedef struct NSVGattrib
{
    char id[64];
    float xform[6];
    unsigned int fillColor;
    unsigned int strokeColor;
    float opacity;
    float fillOpacity;
    float strokeOpacity;
    char fillGradient[64];
    char strokeGradient[64];
    float strokeWidth;
    float strokeDashOffset;
    float strokeDashArray[NSVG_MAX_DASHES];
    int strokeDashCount;
    char strokeLineJoin;
    char strokeLineCap;
    float miterLimit;
    char fillRule;
    float fontSize;
    unsigned int stopColor;
    float stopOpacity;
    float stopOffset;
    char hasFill;
    char hasStroke;
    char visible;
} NSVGattrib;

typedef struct NSVGparser
{
    NSVGattrib attr[NSVG_MAX_ATTR];
    int attrHead;
    float* pts;
    int npts;
    int cpts;
    NSVGpath* plist;
    NSVGimage* image;
    NSVGgradientData* gradients;
    NSVGshape* shapesTail;
    float viewMinx, viewMiny, viewWidth, viewHeight;
    int alignX, alignY, alignType;
    float dpi;
    char pathFlag;
    char defsFlag;
} NSVGparser;

static void nsvg__xformIdentity(float* t)
{
    t[0] = 1.0f; t[1] = 0.0f;
    t[2] = 0.0f; t[3] = 1.0f;
    t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformSetTranslation(float* t, float tx, float ty)
{
    t[0] = 1.0f; t[1] = 0.0f;
    t[2] = 0.0f; t[3] = 1.0f;
    t[4] = tx; t[5] = ty;
}

static void nsvg__xformSetScale(float* t, float sx, float sy)
{
    t[0] = sx; t[1] = 0.0f;
    t[2] = 0.0f; t[3] = sy;
    t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformSetSkewX(float* t, float a)
{
    t[0] = 1.0f; t[1] = 0.0f;
    t[2] = tanf(a); t[3] = 1.0f;
    t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformSetSkewY(float* t, float a)
{
    t[0] = 1.0f; t[1] = tanf(a);
    t[2] = 0.0f; t[3] = 1.0f;
    t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformSetRotation(float* t, float a)
{
    float cs = cosf(a), sn = sinf(a);
    t[0] = cs; t[1] = sn;
    t[2] = -sn; t[3] = cs;
    t[4] = 0.0f; t[5] = 0.0f;
}

static void nsvg__xformMultiply(float* t, float* s)
{
    float t0 = t[0] * s[0] + t[1] * s[2];
    float t2 = t[2] * s[0] + t[3] * s[2];
    float t4 = t[4] * s[0] + t[5] * s[2] + s[4];
    t[1] = t[0] * s[1] + t[1] * s[3];
    t[3] = t[2] * s[1] + t[3] * s[3];
    t[5] = t[4] * s[1] + t[5] * s[3] + s[5];
    t[0] = t0;
    t[2] = t2;
    t[4] = t4;
}

static void nsvg__xformInverse(float* inv, float* t)
{
    double invdet, det = (double)t[0] * t[3] - (double)t[2] * t[1];
    if (det > -1e-6 && det < 1e-6) {
        nsvg__xformIdentity(t);
        return;
    }
    invdet = 1.0 / det;
    inv[0] = (float)(t[3] * invdet);
    inv[2] = (float)(-t[2] * invdet);
    inv[4] = (float)(((double)t[2] * t[5] - (double)t[3] * t[4]) * invdet);
    inv[1] = (float)(-t[1] * invdet);
    inv[3] = (float)(t[0] * invdet);
    inv[5] = (float)(((double)t[1] * t[4] - (double)t[0] * t[5]) * invdet);
}

static void nsvg__xformPremultiply(float* t, float* s)
{
    float s2[6];
    memcpy(s2, s, sizeof(float)*6);
    nsvg__xformMultiply(s2, t);
    memcpy(t, s2, sizeof(float)*6);
}

static void nsvg__xformPoint(float* dx, float* dy, float x, float y, float* t)
{
    *dx = x*t[0] + y*t[2] + t[4];
    *dy = x*t[1] + y*t[3] + t[5];
}

static void nsvg__xformVec(float* dx, float* dy, float x, float y, float* t)
{
    *dx = x*t[0] + y*t[2];
    *dy = x*t[1] + y*t[3];
}
*/

#define NSVG_EPSILON (1e-12)

static int nsvg__ptInBounds(float* pt, float* bounds)
{
    return pt[0] >= bounds[0] && pt[0] <= bounds[2] && pt[1] >= bounds[1] && pt[1] <= bounds[3];
}


static double nsvg__evalBezier(double t, double p0, double p1, double p2, double p3)
{
    double it = 1.0-t;
    return it*it*it*p0 + 3.0*it*it*t*p1 + 3.0*it*t*t*p2 + t*t*t*p3;
}

static void nsvg__curveBounds(float* bounds, float* curve)
{
    int i, j, count;
    double roots[2], a, b, c, b2ac, t, v;
    float* v0 = &curve[0];
    float* v1 = &curve[2];
    float* v2 = &curve[4];
    float* v3 = &curve[6];

    // Start the bounding box by end points
    bounds[0] = nsvg__minf(v0[0], v3[0]);
    bounds[1] = nsvg__minf(v0[1], v3[1]);
    bounds[2] = nsvg__maxf(v0[0], v3[0]);
    bounds[3] = nsvg__maxf(v0[1], v3[1]);

    // Bezier curve fits inside the convex hull of it's control points.
    // If control points are inside the bounds, we're done.
    if (nsvg__ptInBounds(v1, bounds) && nsvg__ptInBounds(v2, bounds))
        return;

    // Add bezier curve inflection points in X and Y.
    for (i = 0; i < 2; i++) {
        a = -3.0 * v0[i] + 9.0 * v1[i] - 9.0 * v2[i] + 3.0 * v3[i];
        b = 6.0 * v0[i] - 12.0 * v1[i] + 6.0 * v2[i];
        c = 3.0 * v1[i] - 3.0 * v0[i];
        count = 0;
        if (fabs(a) < NSVG_EPSILON) {
            if (fabs(b) > NSVG_EPSILON) {
                t = -c / b;
                if (t > NSVG_EPSILON && t < 1.0-NSVG_EPSILON)
                    roots[count++] = t;
            }
        } else {
            b2ac = b*b - 4.0*c*a;
            if (b2ac > NSVG_EPSILON) {
                t = (-b + sqrt(b2ac)) / (2.0 * a);
                if (t > NSVG_EPSILON && t < 1.0-NSVG_EPSILON)
                    roots[count++] = t;
                t = (-b - sqrt(b2ac)) / (2.0 * a);
                if (t > NSVG_EPSILON && t < 1.0-NSVG_EPSILON)
                    roots[count++] = t;
            }
        }
        for (j = 0; j < count; j++) {
            v = nsvg__evalBezier(roots[j], v0[i], v1[i], v2[i], v3[i]);
            bounds[0+i] = nsvg__minf(bounds[0+i], (float)v);
            bounds[2+i] = nsvg__maxf(bounds[2+i], (float)v);
        }
    }
}

NSVGparser::NSVGparser() // static NSVGparser* nsvg__createParser(void)
    : pts(0)
    , cpts(0)
    , viewMinx(0)
    , viewMiny(0)
    , viewWidth(0)
    , viewHeight(0)
    , alignX(0)
    , alignY(0)
    , alignType(0)
    , dpi(0)
    , m_pct(0)
    , m_stamp(0)
{
    Init();
}

void NSVGparser::Init()
{
    NSVGparser* p = this;
    attrHead = 0;
    npts = 0;
    image = new NSVGimage();
    gradients = 0;
    shapesTail = 0;
    defsFlag = 0;
    plist = 0;
    
    memset(p->image, 0, sizeof(NSVGimage));
    
    // Init style
    MemZeroT(attr);
    p->attr[0].xform.Reset();           // nsvg__xformIdentity(p->attr[0].xform);
	                                    // memset(p->attr[0].id, 0, sizeof p->attr[0].id);
//     p->attr[0].fillColor = NSVG_RGB(0,0,0);
//     p->attr[0].strokeColor = NSVG_RGB(0,0,0);
    p->attr[0].opacity = 1;
    p->attr[0].fillOpacity = 1;
    p->attr[0].strokeOpacity = 1;
    p->attr[0].stopOpacity = 1;
    p->attr[0].strokeWidth = 1;
    p->attr[0].strokeLineJoin = NSVG_JOIN_MITER;
    p->attr[0].strokeLineCap = NSVG_CAP_BUTT;
    p->attr[0].miterLimit = 4;
    p->attr[0].fillRule = NSVG_FILLRULE_NONZERO;
    p->attr[0].m_fill.Init();           //    p->attr[0].hasFill = 1;
    p->attr[0].visible = 1;

// 	return p;
// 
// error:
// 	if (p) {
// 		if (p->image) free(p->image);
// 		free(p);
// 	}
// 	return NULL;
}

static void nsvg__deletePaths(NSVGpath* path)
{
    while (path) {
        NSVGpath *next = path->next;
        if (path->pts != NULL)
            free(path->pts);
        delete path;                    // free(path);
        path = next;
    }
}

static void nsvg__deletePaint(NSVGpaint* paint)
{
    if (paint->type == NSVG_PAINT_LINEAR_GRADIENT || paint->type == NSVG_PAINT_RADIAL_GRADIENT)
        free(paint->gradient);
}

static void nsvg__deleteGradientData(NSVGgradientData* grad)
{
    NSVGgradientData* next;
    while (grad != NULL) {
        next = grad->next;
        free(grad->stops);
        delete grad;                    // free(grad);
        grad = next;
    }
}

NSVGparser::~NSVGparser()               // static void nsvg__deleteParser(NSVGparser* p)
{
    NSVGparser* p = this;
    nsvg__deletePaths(p->plist);
    nsvg__deleteGradientData(p->gradients);
    nsvgDelete(p->image);
    if (pts)
        free(p->pts);
}

/*
static void nsvg__resetPath(NSVGparser* p)
{
	p->npts = 0;
}

static void nsvg__addPoint(NSVGparser* p, float x, float y)
{
	if (p->npts+1 > p->cpts) {
		p->cpts = p->cpts ? p->cpts*2 : 8;
		p->pts = (float*)realloc(p->pts, p->cpts*2*sizeof(float));
		if (!p->pts) return;
	}
	p->pts[p->npts*2+0] = x;
	p->pts[p->npts*2+1] = y;
	p->npts++;
}
*/
void NSVGparser::AddPoint(TM::CPointF pt)
{
    NSVGparser* p = this;
    if (p->npts+1 > p->cpts) {
        p->cpts = p->cpts ? p->cpts*2 : 8;
        p->pts = (TM::CPointF*)realloc(p->pts, p->cpts * sizeof(TM::CPointF));
        if (!p->pts) return;
    }
    p->pts[this->npts] = pt;
    p->npts++;
}
/*

static void nsvg__moveTo(NSVGparser* p, float x, float y)
{
	if (p->npts > 0) {
		p->pts[(p->npts-1)*2+0] = x;
		p->pts[(p->npts-1)*2+1] = y;
	} else {
		nsvg__addPoint(p, x, y);
	}
}
*/
void NSVGparser::MoveTo(TM::CPointF pt)
{
    if (this->npts > 0)
        this->pts[this->npts-1] = pt;
    else
        AddPoint(pt);
}
/*

static void nsvg__lineTo(NSVGparser* p, float x, float y)
{
	float px,py, dx,dy;
	if (p->npts > 0) {
		px = p->pts[(p->npts-1)*2+0];
		py = p->pts[(p->npts-1)*2+1];
		dx = x - px;
		dy = y - py;
		nsvg__addPoint(p, px + dx/3.0f, py + dy/3.0f);
		nsvg__addPoint(p, x - dx/3.0f, y - dy/3.0f);
		nsvg__addPoint(p, x, y);
	}
}
*/
void NSVGparser::LineTo(float x, float y)
{
    if (this->npts > 0)
    {
        float px = this->pts[this->npts-1].X();
        float py = this->pts[this->npts-1].Y();
        float dx = x - px;
        float dy = y - py;
        AddPoint(px + dx/3.0f, py + dy/3.0f);
        AddPoint(x - dx/3.0f, y - dy/3.0f);
        AddPoint(x, y);
    }
}
/*

static void nsvg__cubicBezTo(NSVGparser* p, float cpx1, float cpy1, float cpx2, float cpy2, float x, float y)
{
	if (p->npts > 0) {
		nsvg__addPoint(p, cpx1, cpy1);
		nsvg__addPoint(p, cpx2, cpy2);
		nsvg__addPoint(p, x, y);
	}
}
*/
void NSVGparser::CubicBezTo(float cpx1, float cpy1, float cpx2, float cpy2, float x, float y)
{
    if (this->npts > 0) {
        AddPoint(cpx1, cpy1);
        AddPoint(cpx2, cpy2);
        AddPoint(x, y);
    }
}
/*

static NSVGattrib* nsvg__getAttr(NSVGparser* p)
{
	return &p->attr[p->attrHead];
}
*/
NSVGattrib* NSVGparser::GetAttr()
{
    return &attr[attrHead];
}
/*

static void nsvg__pushAttr(NSVGparser* p)
{
	if (p->attrHead < NSVG_MAX_ATTR-1) {
		p->attrHead++;
		memcpy(&p->attr[p->attrHead], &p->attr[p->attrHead-1], sizeof(NSVGattrib));
	}
}
*/
void NSVGparser::PushAttr()
{
    if (attrHead < NSVG_MAX_ATTR - 1)
    {
        ++attrHead;
        attr[attrHead] = attr[attrHead - 1];
    }
}
/*

static void nsvg__popAttr(NSVGparser* p)
{
	if (p->attrHead > 0)
		p->attrHead--;
}
*/
void NSVGparser::PopAttr()
{
    if (attrHead > 0)
        --attrHead;
}

static float nsvg__actualOrigX(NSVGparser* p)
{
    return p->viewMinx;
}

static float nsvg__actualOrigY(NSVGparser* p)
{
    return p->viewMiny;
}

static float nsvg__actualWidth(NSVGparser* p)
{
    return p->viewWidth;
}

static float nsvg__actualHeight(NSVGparser* p)
{
    return p->viewHeight;
}

static float nsvg__actualLength(NSVGparser* p)
{
    float w = nsvg__actualWidth(p), h = nsvg__actualHeight(p);
    return sqrtf(w*w + h*h) / sqrtf(2.0f);
}

float NSVGparser::ConvertToPixels(NSVGcoordinate c, float orig, float length)
{
    NSVGparser* p = this;
    NSVGattrib* attr = GetAttr();
    switch (c.units) {
    case NSVG_UNITS_USER:       return c.value;
    case NSVG_UNITS_PX:         return c.value;
    case NSVG_UNITS_PT:         return c.value / 72.0f * p->dpi;
    case NSVG_UNITS_PC:         return c.value / 6.0f * p->dpi;
    case NSVG_UNITS_MM:         return c.value / 25.4f * p->dpi;
    case NSVG_UNITS_CM:         return c.value / 2.54f * p->dpi;
    case NSVG_UNITS_IN:         return c.value * p->dpi;
    case NSVG_UNITS_EM:         return c.value * attr->fontSize;
    case NSVG_UNITS_EX:         return c.value * attr->fontSize * 0.52f; // x-height of Helvetica.
    case NSVG_UNITS_PERCENT:    return orig + c.value / 100.0f * length;
    default:                    return c.value;
    }
    return c.value;
}

static NSVGgradientData* nsvg__findGradientData(NSVGparser* p, TM::CXmlTag id)
{
    NSVGgradientData* grad = p->gradients;
    if (id.IsEmpty())                   //  if (id == NULL || *id == '\0')
        return NULL;
    while (grad != NULL) {
        if (grad->id == id)             // if (strcmp(grad->id, id) == 0)
            return grad;
        grad = grad->next;
    }
    return NULL;
}

static NSVGgradient* nsvg__createGradient(NSVGparser* p, TM::CXmlTag id, const float* localBounds, NSVGpaintType* paintType)
{
    CAutoMsgAnalysis ama("nsvg__createGradient()");
    NSVGattrib* attr = p->GetAttr();
    NSVGgradientData* data = NULL;
    NSVGgradientData* ref = NULL;
    NSVGgradientStop* stops = NULL;
    NSVGgradient* grad;
    float ox, oy, sw, sh, sl;
    int nstops = 0;
    int refIter;

    data = nsvg__findGradientData(p, id);
    if (data == NULL) return NULL;

    // TODO: use ref to fill in all unset values too.
    ref = data;
    refIter = 0;
    while (ref != NULL) {
        NSVGgradientData* nextRef = NULL;
        if (stops == NULL && ref->stops != NULL) {
            stops = ref->stops;
            nstops = ref->nstops;
            break;
        }
        nextRef = nsvg__findGradientData(p, ref->ref);
        if (nextRef == ref) break; // prevent infite loops on malformed data
        ref = nextRef;
        refIter++;
        if (refIter > 32) break; // prevent infite loops on malformed data
    }
    if (stops == NULL) return NULL;

    grad = (NSVGgradient*)malloc(sizeof(NSVGgradient) + sizeof(NSVGgradientStop)*(nstops-1));
    if (grad == NULL) return NULL;

    // The shape width and height.
    if (data->units == NSVG_OBJECT_SPACE) {
        ox = localBounds[0];
        oy = localBounds[1];
        sw = localBounds[2] - localBounds[0];
        sh = localBounds[3] - localBounds[1];
    } else {
        ox = nsvg__actualOrigX(p);
        oy = nsvg__actualOrigY(p);
        sw = nsvg__actualWidth(p);
        sh = nsvg__actualHeight(p);
    }
    sl = sqrtf(sw*sw + sh*sh) / sqrtf(2.0f);

    if (data->type == NSVG_PAINT_LINEAR_GRADIENT) {
        float x1, y1, x2, y2, dx, dy;
        x1 = p->ConvertToPixels(data->linear.x1, ox, sw);
        y1 = p->ConvertToPixels(data->linear.y1, oy, sh);
        x2 = p->ConvertToPixels(data->linear.x2, ox, sw);
        y2 = p->ConvertToPixels(data->linear.y2, oy, sh);
        // Calculate transform aligned to the line
        dx = x2 - x1;
        dy = y2 - y1;
        grad->xform.Set(dy, -dx, dx, dy, x1, y1);   // grad->xform[0] = dy; grad->xform[1] = -dx;
                                                    // grad->xform[2] = dx; grad->xform[3] = dy;
                                                    // grad->xform[4] = x1; grad->xform[5] = y1;
    } else {
        float cx, cy, fx, fy, r;
        cx = p->ConvertToPixels(data->radial.cx, ox, sw);
        cy = p->ConvertToPixels(data->radial.cy, oy, sh);
        fx = p->ConvertToPixels(data->radial.fx, ox, sw);
        fy = p->ConvertToPixels(data->radial.fy, oy, sh);
        r  = p->ConvertToPixels(data->radial.r, 0, sl);
        // Calculate transform aligned to the circle
        grad->xform.Set(r, 0, 0, r, cx, cy);         // grad->xform[0] = r; grad->xform[1] = 0;
                                                    // grad->xform[2] = 0; grad->xform[3] = r;
                                                    // grad->xform[4] = cx; grad->xform[5] = cy;
        grad->fx = fx / r;
        grad->fy = fy / r;
    }

    grad->xform.Combine(data->xform);               //  nsvg__xformMultiply(grad->xform, data->xform);
    grad->xform.Combine(attr->xform);               //  nsvg__xformMultiply(grad->xform, attr->xform);

    grad->spread = data->spread;
    memcpy(grad->stops, stops, nstops*sizeof(NSVGgradientStop));
    grad->nstops = nstops;

    *paintType = data->type;

    return grad;
}

/*
static float nsvg__getAverageScale(float* t)
{
	float sx = sqrtf(t[0]*t[0] + t[2]*t[2]);
	float sy = sqrtf(t[1]*t[1] + t[3]*t[3]);
	return (sx + sy) * 0.5f;
}
*/

void NSVGshape::GetLocalBounds(float* bounds, TM::CAffine& xform)
{
    CAutoMsgAnalysis ama("NSVGshape::GetLocalBounds()");
    NSVGpath* path;
    TMPointF curve[4]; float curveBounds[4];
    int i, first = 1;
    for (path = this->paths; path != NULL; path = path->next) {
        xform.LPtoDP(path->pts[0], curve[0]);             //nsvg__xformPoint(&curve[0], &curve[1], path->pts[0], path->pts[1], xform);
        for (i = 0; i < path->npts-1; i += 3) {
            xform.LPtoDP(&path->pts[i + 1], 3, &curve[1]);    // nsvg__xformPoint(&curve[2], &curve[3], path->pts[(i+1)*2], path->pts[(i+1)*2+1], xform);
                                                            // nsvg__xformPoint(&curve[4], &curve[5], path->pts[(i+2)*2], path->pts[(i+2)*2+1], xform);
                                                            // nsvg__xformPoint(&curve[6], &curve[7], path->pts[(i+3)*2], path->pts[(i+3)*2+1], xform);
            nsvg__curveBounds(curveBounds, (float*)curve);
            if (first) {
                bounds[0] = curveBounds[0];
                bounds[1] = curveBounds[1];
                bounds[2] = curveBounds[2];
                bounds[3] = curveBounds[3];
                first = 0;
            } else {
                bounds[0] = nsvg__minf(bounds[0], curveBounds[0]);
                bounds[1] = nsvg__minf(bounds[1], curveBounds[1]);
                bounds[2] = nsvg__maxf(bounds[2], curveBounds[2]);
                bounds[3] = nsvg__maxf(bounds[3], curveBounds[3]);
            }
            curve[0] = curve[3];    // curve[0] = curve[6];
                                    // curve[1] = curve[7];
        }
    }
}


static void _FillToGpaint(CSvgFill& fill, float opacity, NSVGparser* doc, NSVGattrib* attr, NSVGshape* shape, NSVGpaint& paint)
{
    switch (fill.m_type)
    {
    case CSvgFill::eFillNone:
        paint.type = NSVG_PAINT_NONE;
        break;
        
    case CSvgFill::eFillColor:
        paint.type = NSVG_PAINT_COLOR;
        paint.color = fill.m_Color;
        ColorSetA(paint.color, opacity*255);// |= (unsigned int)(attr->fillOpacity*255) << 24;
        break;
        
    case CSvgFill::eFillGradient:
        {
            float localBounds[4];
            TM::CAffine inv(attr->xform);
            inv.Invert();
            shape->GetLocalBounds(localBounds, inv);
            paint.gradient = nsvg__createGradient(doc, fill.m_gradient, localBounds, &paint.type);
            if (paint.gradient == NULL)
                paint.type = NSVG_PAINT_NONE;
        }
    }
}

static void nsvg__addShape(NSVGparser* p)
{
    CAutoMsgAnalysis ama("nsvg__addShape()");

    NSVGattrib* attr = p->GetAttr();
    DbgAssert(attr->IsVisible());
    float scale = 1.0f;
    NSVGshape* shape;
    NSVGpath* path;
    int i;

    if (p->plist == NULL)
        return;

    shape = new NSVGshape();
    if (shape == NULL) goto error;
    memset(shape, 0, sizeof(NSVGshape));

    shape->id = attr->id; // memcpy(shape->id, attr->id, sizeof shape->id);
    scale = attr->xform.GetAverageScale();// nsvg__getAverageScale(attr->xform);
    shape->strokeWidth = attr->strokeWidth * scale;
    shape->strokeDashOffset = attr->strokeDashOffset * scale;
    shape->strokeDashCount = (char)attr->strokeDashCount;
    for (i = 0; i < attr->strokeDashCount; i++)
        shape->strokeDashArray[i] = attr->strokeDashArray[i] * scale;
    shape->strokeLineJoin = attr->strokeLineJoin;
    shape->strokeLineCap = attr->strokeLineCap;
    shape->miterLimit = attr->miterLimit;
    shape->fillRule = attr->fillRule;
    shape->opacity = attr->opacity;

    shape->paths = p->plist;
    p->plist = NULL;

    // Calculate shape bounds
    shape->bounds = shape->paths->bounds;   // shape->bounds[0] = shape->paths->bounds[0];
                                            // shape->bounds[1] = shape->paths->bounds[1];
                                            // shape->bounds[2] = shape->paths->bounds[2];
                                            // shape->bounds[3] = shape->paths->bounds[3];
    for (path = shape->paths->next; path != NULL; path = path->next) {
        shape->bounds |= path->bounds;      // shape->bounds[0] = nsvg__minf(shape->bounds[0], path->bounds[0]);
                                            // shape->bounds[1] = nsvg__minf(shape->bounds[1], path->bounds[1]);
                                            // shape->bounds[2] = nsvg__maxf(shape->bounds[2], path->bounds[2]);
                                            // shape->bounds[3] = nsvg__maxf(shape->bounds[3], path->bounds[3]);
    }

    // Set fill
    _FillToGpaint(attr->m_fill, attr->fillOpacity, p, attr, shape, shape->fill);
    _FillToGpaint(attr->m_stroke, attr->strokeOpacity, p, attr, shape, shape->stroke);
//     if (attr->hasFill == 0) {
//         shape->fill.type = NSVG_PAINT_NONE;
//     } else if (attr->hasFill == 1) {
//         shape->fill.type = NSVG_PAINT_COLOR;
//         shape->fill.color = attr->fillColor;
//         shape->fill.color |= (unsigned int)(attr->fillOpacity*255) << 24;
//     } else if (attr->hasFill == 2) {
//         float inv[6], localBounds[4];
//         nsvg__xformInverse(inv, attr->xform);
//         nsvg__getLocalBounds(localBounds, shape, inv);
//         shape->fill.gradient = nsvg__createGradient(p, attr->fillGradient, localBounds, &shape->fill.type);
//         if (shape->fill.gradient == NULL) {
//             shape->fill.type = NSVG_PAINT_NONE;
//         }
//     }
// 
//     // Set stroke
//     if (attr->hasStroke == 0) {
//         shape->stroke.type = NSVG_PAINT_NONE;
//     } else if (attr->hasStroke == 1) {
//         shape->stroke.type = NSVG_PAINT_COLOR;
//         shape->stroke.color = attr->strokeColor;
//         shape->stroke.color |= (unsigned int)(attr->strokeOpacity*255) << 24;
//     } else if (attr->hasStroke == 2) {
//         float inv[6], localBounds[4];
//         nsvg__xformInverse(inv, attr->xform);
//         nsvg__getLocalBounds(localBounds, shape, inv);
//      shape->stroke.gradient = nsvg__createGradient(p, attr->strokeGradient, localBounds, &shape->stroke.type);
//      if (shape->stroke.gradient == NULL)
//          shape->stroke.type = NSVG_PAINT_NONE;
//  }

    // Set flags
    shape->flags = (attr->visible ? NSVG_FLAGS_VISIBLE : 0x00);

    // Add to tail
    if (p->image->shapes == NULL)
        p->image->shapes = shape;
    else
        p->shapesTail->next = shape;
    p->shapesTail = shape;

    return;

error:
    if (shape) free(shape);
}

void NSVGparser::AddPath(char closed)
{
    CAutoMsgAnalysis ama("NSVGparser::AddPath()");
    NSVGparser* p = this;
    NSVGattrib* attr = p->GetAttr();
    NSVGpath* path = NULL;
    //  float bounds[4];
    float* curve;
    int i;
    
    if (p->npts < 4)
        return;
    
    if (closed)
        LineTo(this->pts[0]); // nsvg__lineTo(p, p->pts[0], p->pts[1]);
    
    // Expect 1 + N*3 points (N = number of cubic bezier segments).
    if ((p->npts % 3) != 1)
        return;

    path = (NSVGpath*)malloc(sizeof(NSVGpath));
    if (path == NULL) goto error;
    memset(path, 0, sizeof(NSVGpath));
    
    path->pts = (TM::CPointF*)malloc(p->npts * sizeof(TM::CPointF));
    if (path->pts == NULL) goto error;
    path->closed = closed;
    path->npts = p->npts;
    
    // Transform path.
//     for (i = 0; i < p->npts; ++i)
//         nsvg__xformPoint(&path->pts[i*2], &path->pts[i*2+1], p->pts[i*2], p->pts[i*2+1], attr->xform);
    attr->xform.LPtoDP(p->pts, p->npts, path->pts);

    // Find bounds
    for (i = 0; i < path->npts-1; i += 3) {
        curve = (float*)&path->pts[i]; //        curve = &path->pts[i*2];
        TM::CRectF bounds;
        nsvg__curveBounds((float*)&bounds, curve);
        if (i == 0) {
            path->bounds = bounds;      // path->bounds[0] = bounds[0];
                                        // path->bounds[1] = bounds[1];
                                        // path->bounds[2] = bounds[2];
                                        // path->bounds[3] = bounds[3];
        } else {
            path->bounds |= bounds;     // path->bounds[0] = nsvg__minf(path->bounds[0], bounds[0]);
                                        // path->bounds[1] = nsvg__minf(path->bounds[1], bounds[1]);
                                        // path->bounds[2] = nsvg__maxf(path->bounds[2], bounds[2]);
                                        // path->bounds[3] = nsvg__maxf(path->bounds[3], bounds[3]);
        }
    }
    
    path->next = p->plist;
    p->plist = path;
    
    return;
    
error:
    if (path != NULL) {
        if (path->pts != NULL) free(path->pts);
        free(path);
    }
}

/*
// We roll our own string to float because the std library one uses locale and messes things up.
static double nsvg__atof(const char* s)
{
    char* cur = (char*)s;
    char* end = NULL;
    double res = 0.0, sign = 1.0;
    long long intPart = 0, fracPart = 0;
    char hasIntPart = 0, hasFracPart = 0;

    // Parse optional sign
    if (*cur == '+') {
        cur++;
    } else if (*cur == '-') {
        sign = -1;
        cur++;
    }

    // Parse integer part
    if (nsvg__isdigit(*cur)) {
        // Parse digit sequence
        intPart = strtoll(cur, &end, 10);
        if (cur != end) {
            res = (double)intPart;
            hasIntPart = 1;
            cur = end;
        }
    }

    // Parse fractional part.
    if (*cur == '.') {
        cur++; // Skip '.'
        if (nsvg__isdigit(*cur)) {
            // Parse digit sequence
            fracPart = strtoll(cur, &end, 10);
            if (cur != end) {
                res += (double)fracPart / pow(10.0, (double)(end - cur));
                hasFracPart = 1;
                cur = end;
            }
        }
    }

    // A valid number should have integer or fractional part.
    if (!hasIntPart && !hasFracPart)
        return 0.0;

    // Parse optional exponent
    if (*cur == 'e' || *cur == 'E') {
        long expPart = 0;
        cur++; // skip 'E'
        expPart = strtol(cur, &end, 10); // Parse digit sequence with sign
        if (cur != end) {
            res *= pow(10.0, (double)expPart);
        }
    }

    return res * sign;
}


static const char* nsvg__parseNumber(const char* s, char* it, const int size)
{
    const int last = size-1;
    int i = 0;

    // sign
    if (*s == '-' || *s == '+') {
        if (i < last) it[i++] = *s;
        s++;
    }
    // integer part
    while (*s && nsvg__isdigit(*s)) {
        if (i < last) it[i++] = *s;
        s++;
    }
    if (*s == '.') {
        // decimal point
        if (i < last) it[i++] = *s;
        s++;
        // fraction part
        while (*s && nsvg__isdigit(*s)) {
            if (i < last) it[i++] = *s;
            s++;
        }
    }
    // exponent
    if ((*s == 'e' || *s == 'E') && (s[1] != 'm' && s[1] != 'x')) {
        if (i < last) it[i++] = *s;
        s++;
        if (*s == '-' || *s == '+') {
            if (i < last) it[i++] = *s;
            s++;
        }
        while (*s && nsvg__isdigit(*s)) {
            if (i < last) it[i++] = *s;
            s++;
        }
    }
    it[i] = '\0';

    return s;
}

static const char* nsvg__getNextPathItem(const char* s, char* it)
{
    it[0] = '\0';
    // Skip white spaces and commas
    while (*s && (nsvg__isspace(*s) || *s == ',')) s++;
    if (!*s) return s;
    if (*s == '-' || *s == '+' || *s == '.' || nsvg__isdigit(*s)) {
        s = nsvg__parseNumber(s, it, 64);
    } else {
        // Parse command
        it[0] = *s++;
        it[1] = '\0';
        return s;
    }

    return s;
}

static unsigned int nsvg__parseColorHex(const char* str)
{
    unsigned int r=0, g=0, b=0;
    if (sscanf(str, "#%2x%2x%2x", &r, &g, &b) == 3)        // 2 digit hex
        return NSVG_RGB(r, g, b);
    if (sscanf(str, "#%1x%1x%1x", &r, &g, &b) == 3)        // 1 digit hex, e.g. #abc -> 0xccbbaa
        return NSVG_RGB(r*17, g*17, b*17);            // same effect as (r<<4|r), (g<<4|g), ..
    return NSVG_RGB(128, 128, 128);
}

static unsigned int nsvg__parseColorRGB(const char* str)
{
    unsigned int r=0, g=0, b=0;
    float rf=0, gf=0, bf=0;
    if (sscanf(str, "rgb(%u, %u, %u)", &r, &g, &b) == 3)        // decimal integers
        return NSVG_RGB(r, g, b);
    if (sscanf(str, "rgb(%f%%, %f%%, %f%%)", &rf, &gf, &bf) == 3)    // decimal integer percentage
        return NSVG_RGB(roundf(rf*2.55f), roundf(gf*2.55f), roundf(bf*2.55f)); // (255 / 100.0f)
    return NSVG_RGB(128, 128, 128);
}

typedef struct NSVGNamedColor {
    const char* name;
    unsigned int color;
} NSVGNamedColor;

NSVGNamedColor nsvg__colors[] = {

    { "red", NSVG_RGB(255, 0, 0) },
    { "green", NSVG_RGB(0, 128, 0) },
    { "blue", NSVG_RGB(0, 0, 255) },
    { "yellow", NSVG_RGB(255, 255, 0) },
    { "cyan", NSVG_RGB(0, 255, 255) },
    { "magenta", NSVG_RGB(255, 0, 255) },
    { "black", NSVG_RGB(0, 0, 0) },
    { "grey", NSVG_RGB(128, 128, 128) },
    { "gray", NSVG_RGB(128, 128, 128) },
    { "white", NSVG_RGB(255, 255, 255) },

#ifdef NANOSVG_ALL_COLOR_KEYWORDS
    { "aliceblue", NSVG_RGB(240, 248, 255) },
    { "antiquewhite", NSVG_RGB(250, 235, 215) },
    { "aqua", NSVG_RGB(0, 255, 255) },
    { "aquamarine", NSVG_RGB(127, 255, 212) },
    { "azure", NSVG_RGB(240, 255, 255) },
    { "beige", NSVG_RGB(245, 245, 220) },
    { "bisque", NSVG_RGB(255, 228, 196) },
    { "blanchedalmond", NSVG_RGB(255, 235, 205) },
    { "blueviolet", NSVG_RGB(138, 43, 226) },
    { "brown", NSVG_RGB(165, 42, 42) },
    { "burlywood", NSVG_RGB(222, 184, 135) },
    { "cadetblue", NSVG_RGB(95, 158, 160) },
    { "chartreuse", NSVG_RGB(127, 255, 0) },
    { "chocolate", NSVG_RGB(210, 105, 30) },
    { "coral", NSVG_RGB(255, 127, 80) },
    { "cornflowerblue", NSVG_RGB(100, 149, 237) },
    { "cornsilk", NSVG_RGB(255, 248, 220) },
    { "crimson", NSVG_RGB(220, 20, 60) },
    { "darkblue", NSVG_RGB(0, 0, 139) },
    { "darkcyan", NSVG_RGB(0, 139, 139) },
    { "darkgoldenrod", NSVG_RGB(184, 134, 11) },
    { "darkgray", NSVG_RGB(169, 169, 169) },
    { "darkgreen", NSVG_RGB(0, 100, 0) },
    { "darkgrey", NSVG_RGB(169, 169, 169) },
    { "darkkhaki", NSVG_RGB(189, 183, 107) },
    { "darkmagenta", NSVG_RGB(139, 0, 139) },
    { "darkolivegreen", NSVG_RGB(85, 107, 47) },
    { "darkorange", NSVG_RGB(255, 140, 0) },
    { "darkorchid", NSVG_RGB(153, 50, 204) },
    { "darkred", NSVG_RGB(139, 0, 0) },
    { "darksalmon", NSVG_RGB(233, 150, 122) },
    { "darkseagreen", NSVG_RGB(143, 188, 143) },
    { "darkslateblue", NSVG_RGB(72, 61, 139) },
    { "darkslategray", NSVG_RGB(47, 79, 79) },
    { "darkslategrey", NSVG_RGB(47, 79, 79) },
    { "darkturquoise", NSVG_RGB(0, 206, 209) },
    { "darkviolet", NSVG_RGB(148, 0, 211) },
    { "deeppink", NSVG_RGB(255, 20, 147) },
    { "deepskyblue", NSVG_RGB(0, 191, 255) },
    { "dimgray", NSVG_RGB(105, 105, 105) },
    { "dimgrey", NSVG_RGB(105, 105, 105) },
    { "dodgerblue", NSVG_RGB(30, 144, 255) },
    { "firebrick", NSVG_RGB(178, 34, 34) },
    { "floralwhite", NSVG_RGB(255, 250, 240) },
    { "forestgreen", NSVG_RGB(34, 139, 34) },
    { "fuchsia", NSVG_RGB(255, 0, 255) },
    { "gainsboro", NSVG_RGB(220, 220, 220) },
    { "ghostwhite", NSVG_RGB(248, 248, 255) },
    { "gold", NSVG_RGB(255, 215, 0) },
    { "goldenrod", NSVG_RGB(218, 165, 32) },
    { "greenyellow", NSVG_RGB(173, 255, 47) },
    { "honeydew", NSVG_RGB(240, 255, 240) },
    { "hotpink", NSVG_RGB(255, 105, 180) },
    { "indianred", NSVG_RGB(205, 92, 92) },
    { "indigo", NSVG_RGB(75, 0, 130) },
    { "ivory", NSVG_RGB(255, 255, 240) },
    { "khaki", NSVG_RGB(240, 230, 140) },
    { "lavender", NSVG_RGB(230, 230, 250) },
    { "lavenderblush", NSVG_RGB(255, 240, 245) },
    { "lawngreen", NSVG_RGB(124, 252, 0) },
    { "lemonchiffon", NSVG_RGB(255, 250, 205) },
    { "lightblue", NSVG_RGB(173, 216, 230) },
    { "lightcoral", NSVG_RGB(240, 128, 128) },
    { "lightcyan", NSVG_RGB(224, 255, 255) },
    { "lightgoldenrodyellow", NSVG_RGB(250, 250, 210) },
    { "lightgray", NSVG_RGB(211, 211, 211) },
    { "lightgreen", NSVG_RGB(144, 238, 144) },
    { "lightgrey", NSVG_RGB(211, 211, 211) },
    { "lightpink", NSVG_RGB(255, 182, 193) },
    { "lightsalmon", NSVG_RGB(255, 160, 122) },
    { "lightseagreen", NSVG_RGB(32, 178, 170) },
    { "lightskyblue", NSVG_RGB(135, 206, 250) },
    { "lightslategray", NSVG_RGB(119, 136, 153) },
    { "lightslategrey", NSVG_RGB(119, 136, 153) },
    { "lightsteelblue", NSVG_RGB(176, 196, 222) },
    { "lightyellow", NSVG_RGB(255, 255, 224) },
    { "lime", NSVG_RGB(0, 255, 0) },
    { "limegreen", NSVG_RGB(50, 205, 50) },
    { "linen", NSVG_RGB(250, 240, 230) },
    { "maroon", NSVG_RGB(128, 0, 0) },
    { "mediumaquamarine", NSVG_RGB(102, 205, 170) },
    { "mediumblue", NSVG_RGB(0, 0, 205) },
    { "mediumorchid", NSVG_RGB(186, 85, 211) },
    { "mediumpurple", NSVG_RGB(147, 112, 219) },
    { "mediumseagreen", NSVG_RGB(60, 179, 113) },
    { "mediumslateblue", NSVG_RGB(123, 104, 238) },
    { "mediumspringgreen", NSVG_RGB(0, 250, 154) },
    { "mediumturquoise", NSVG_RGB(72, 209, 204) },
    { "mediumvioletred", NSVG_RGB(199, 21, 133) },
    { "midnightblue", NSVG_RGB(25, 25, 112) },
    { "mintcream", NSVG_RGB(245, 255, 250) },
    { "mistyrose", NSVG_RGB(255, 228, 225) },
    { "moccasin", NSVG_RGB(255, 228, 181) },
    { "navajowhite", NSVG_RGB(255, 222, 173) },
    { "navy", NSVG_RGB(0, 0, 128) },
    { "oldlace", NSVG_RGB(253, 245, 230) },
    { "olive", NSVG_RGB(128, 128, 0) },
    { "olivedrab", NSVG_RGB(107, 142, 35) },
    { "orange", NSVG_RGB(255, 165, 0) },
    { "orangered", NSVG_RGB(255, 69, 0) },
    { "orchid", NSVG_RGB(218, 112, 214) },
    { "palegoldenrod", NSVG_RGB(238, 232, 170) },
    { "palegreen", NSVG_RGB(152, 251, 152) },
    { "paleturquoise", NSVG_RGB(175, 238, 238) },
    { "palevioletred", NSVG_RGB(219, 112, 147) },
    { "papayawhip", NSVG_RGB(255, 239, 213) },
    { "peachpuff", NSVG_RGB(255, 218, 185) },
    { "peru", NSVG_RGB(205, 133, 63) },
    { "pink", NSVG_RGB(255, 192, 203) },
    { "plum", NSVG_RGB(221, 160, 221) },
    { "powderblue", NSVG_RGB(176, 224, 230) },
    { "purple", NSVG_RGB(128, 0, 128) },
    { "rosybrown", NSVG_RGB(188, 143, 143) },
    { "royalblue", NSVG_RGB(65, 105, 225) },
    { "saddlebrown", NSVG_RGB(139, 69, 19) },
    { "salmon", NSVG_RGB(250, 128, 114) },
    { "sandybrown", NSVG_RGB(244, 164, 96) },
    { "seagreen", NSVG_RGB(46, 139, 87) },
    { "seashell", NSVG_RGB(255, 245, 238) },
    { "sienna", NSVG_RGB(160, 82, 45) },
    { "silver", NSVG_RGB(192, 192, 192) },
    { "skyblue", NSVG_RGB(135, 206, 235) },
    { "slateblue", NSVG_RGB(106, 90, 205) },
    { "slategray", NSVG_RGB(112, 128, 144) },
    { "slategrey", NSVG_RGB(112, 128, 144) },
    { "snow", NSVG_RGB(255, 250, 250) },
    { "springgreen", NSVG_RGB(0, 255, 127) },
    { "steelblue", NSVG_RGB(70, 130, 180) },
    { "tan", NSVG_RGB(210, 180, 140) },
    { "teal", NSVG_RGB(0, 128, 128) },
    { "thistle", NSVG_RGB(216, 191, 216) },
    { "tomato", NSVG_RGB(255, 99, 71) },
    { "turquoise", NSVG_RGB(64, 224, 208) },
    { "violet", NSVG_RGB(238, 130, 238) },
    { "wheat", NSVG_RGB(245, 222, 179) },
    { "whitesmoke", NSVG_RGB(245, 245, 245) },
    { "yellowgreen", NSVG_RGB(154, 205, 50) },
#endif
};

static unsigned int nsvg__parseColorName(const char* str)
{
    int i, ncolors = sizeof(nsvg__colors) / sizeof(NSVGNamedColor);

    for (i = 0; i < ncolors; i++) {
        if (strcmp(nsvg__colors[i].name, str) == 0) {
            return nsvg__colors[i].color;
        }
    }

    return NSVG_RGB(128, 128, 128);
}

static unsigned int nsvg__parseColor(const char* str)
{
    size_t len = 0;
    while(*str == ' ') ++str;
    len = strlen(str);
    if (len >= 1 && *str == '#')
        return nsvg__parseColorHex(str);
    else if (len >= 4 && str[0] == 'r' && str[1] == 'g' && str[2] == 'b' && str[3] == '(')
        return nsvg__parseColorRGB(str);
    return nsvg__parseColorName(str);
}

static float nsvg__parseOpacity(const char* str)
{
    float val = nsvg__atof(str);
    if (val < 0.0f) val = 0.0f;
    if (val > 1.0f) val = 1.0f;
    return val;
}

static float nsvg__parseMiterLimit(const char* str)
{
    float val = nsvg__atof(str);
    if (val < 0.0f) val = 0.0f;
    return val;
}
*/

NSVGunits NSVGparser::ParseUnits(TM::CXmlTag units)
{
    if (!units.length())
        return NSVG_UNITS_USER;
    if (units[0] == 'p' && units[1] == 'x')
        return NSVG_UNITS_PX;
    else if (units[0] == 'p' && units[1] == 't')
        return NSVG_UNITS_PT;
    else if (units[0] == 'p' && units[1] == 'c')
        return NSVG_UNITS_PC;
    else if (units[0] == 'm' && units[1] == 'm')
        return NSVG_UNITS_MM;
    else if (units[0] == 'c' && units[1] == 'm')
        return NSVG_UNITS_CM;
    else if (units[0] == 'i' && units[1] == 'n')
        return NSVG_UNITS_IN;
    else if (units[0] == '%')
        return NSVG_UNITS_PERCENT;
    else if (units[0] == 'e' && units[1] == 'm')
        return NSVG_UNITS_EM;
    else if (units[0] == 'e' && units[1] == 'x')
        return NSVG_UNITS_EX;
    return NSVG_UNITS_USER;
}

/*
static int nsvg__isCoordinate(const char* s)
{
    // optional sign
    if (*s == '-' || *s == '+')
        s++;
    // must have at least one digit, or start by a dot
    return (nsvg__isdigit(*s) || *s == '.');
}
*/

NSVGcoordinate NSVGparser::ParseCoordinateRaw(TM::CXmlTag val)
{
    double x = 0;
    val = val.ScanDouble(x);
    NSVGunits units = ParseUnits(val);
    NSVGcoordinate coord = { x, units };
    return coord;
}

static NSVGcoordinate nsvg__coord(float v, NSVGunits units)
{
    NSVGcoordinate coord = {v, units};
    return coord;
}

/*
static float nsvg__parseCoordinate(NSVGparser* p, const char* str, float orig, float length)
{
	NSVGcoordinate coord = nsvg__parseCoordinateRaw(str);
	return nsvg__convertToPixels(p, coord, orig, length);
}

static int nsvg__parseTransformArgs(const char* str, float* args, int maxNa, int* na)
{
    const char* end;
    const char* ptr;
    char it[64];

    *na = 0;
    ptr = str;
    while (*ptr && *ptr != '(') ++ptr;
    if (*ptr == 0)
        return 1;
    end = ptr;
    while (*end && *end != ')') ++end;
    if (*end == 0)
        return 1;

    while (ptr < end) {
        if (*ptr == '-' || *ptr == '+' || *ptr == '.' || nsvg__isdigit(*ptr)) {
            if (*na >= maxNa) return 0;
            ptr = nsvg__parseNumber(ptr, it, 64);
            args[(*na)++] = (float)nsvg__atof(it);
        } else {
            ++ptr;
        }
    }
    return (int)(end - str);
}


static int nsvg__parseMatrix(float* xform, const char* str)
{
    float t[6];
    int na = 0;
    int len = nsvg__parseTransformArgs(str, t, 6, &na);
    if (na != 6) return len;
    memcpy(xform, t, sizeof(float)*6);
    return len;
}

static int nsvg__parseTranslate(float* xform, const char* str)
{
    float args[2];
    float t[6];
    int na = 0;
    int len = nsvg__parseTransformArgs(str, args, 2, &na);
    if (na == 1) args[1] = 0.0;

    nsvg__xformSetTranslation(t, args[0], args[1]);
    memcpy(xform, t, sizeof(float)*6);
    return len;
}

static int nsvg__parseScale(float* xform, const char* str)
{
    float args[2];
    int na = 0;
    float t[6];
    int len = nsvg__parseTransformArgs(str, args, 2, &na);
    if (na == 1) args[1] = args[0];
    nsvg__xformSetScale(t, args[0], args[1]);
    memcpy(xform, t, sizeof(float)*6);
    return len;
}

static int nsvg__parseSkewX(float* xform, const char* str)
{
    float args[1];
    int na = 0;
    float t[6];
    int len = nsvg__parseTransformArgs(str, args, 1, &na);
    nsvg__xformSetSkewX(t, args[0]/180.0f*NSVG_PI);
    memcpy(xform, t, sizeof(float)*6);
    return len;
}

static int nsvg__parseSkewY(float* xform, const char* str)
{
    float args[1];
    int na = 0;
    float t[6];
    int len = nsvg__parseTransformArgs(str, args, 1, &na);
    nsvg__xformSetSkewY(t, args[0]/180.0f*NSVG_PI);
    memcpy(xform, t, sizeof(float)*6);
    return len;
}

static int nsvg__parseRotate(float* xform, const char* str)
{
    float args[3];
    int na = 0;
    float m[6];
    float t[6];
    int len = nsvg__parseTransformArgs(str, args, 3, &na);
    if (na == 1)
        args[1] = args[2] = 0.0f;
    nsvg__xformIdentity(m);

    if (na > 1) {
        nsvg__xformSetTranslation(t, -args[1], -args[2]);
        nsvg__xformMultiply(m, t);
    }

    nsvg__xformSetRotation(t, args[0]/180.0f*NSVG_PI);
    nsvg__xformMultiply(m, t);

    if (na > 1) {
        nsvg__xformSetTranslation(t, args[1], args[2]);
        nsvg__xformMultiply(m, t);
    }

    memcpy(xform, m, sizeof(float)*6);

    return len;
}

static void nsvg__parseTransform(float* xform, const char* str)
{
    float t[6];
    int len;
    nsvg__xformIdentity(xform);
    while (*str)
    {
        if (strncmp(str, "matrix", 6) == 0)
            len = nsvg__parseMatrix(t, str);
        else if (strncmp(str, "translate", 9) == 0)
            len = nsvg__parseTranslate(t, str);
        else if (strncmp(str, "scale", 5) == 0)
            len = nsvg__parseScale(t, str);
        else if (strncmp(str, "rotate", 6) == 0)
            len = nsvg__parseRotate(t, str);
        else if (strncmp(str, "skewX", 5) == 0)
            len = nsvg__parseSkewX(t, str);
        else if (strncmp(str, "skewY", 5) == 0)
            len = nsvg__parseSkewY(t, str);
        else{
            ++str;
            continue;
        }
        if (len != 0) {
            str += len;
        } else {
            ++str;
            continue;
        }

        nsvg__xformPremultiply(xform, t);
    }
}
*/

float NSVGparser::ParseAniAttrCoordinatePX(const TM::CSvgDomAttrib& attr, float orig, float length)
{
    NSVGcoordinate coord = ParseAniAttrCoordinate(attr);
    return ConvertToPixels(coord, orig, length);
}

NSVGcoordinate NSVGparser::ParseAniAttrCoordinate(const TM::CSvgDomAttrib& attr)
{
    CAutoMsgAnalysis ama("NSVGparser::ParseAniAttrCoordinate()");

    NSVGcoordinate coord;
    if (attr.m_ani)
    {
        CSvgAniItem i1, i2;
        float tmRate = attr.m_ani->GetFrame(m_stamp, i1, i2);
        coord = ParseCoordinateRaw(i1.m_val);
        NSVGcoordinate coord2 = ParseCoordinateRaw(i2.m_val);
        coord.value = i1.CalcFrameFloat(coord.value, coord2.value, tmRate);
    }
    else
        coord = ParseCoordinateRaw(attr.m_val);
    return coord;
}
/*

static void nsvg__parseUrl(char* id, const char* str)
{
	int i = 0;
	str += 4; // "url(";
	if (*str && *str == '#')
		str++;
	while (i < 63 && *str && *str != ')') {
		id[i] = *str++;
		i++;
	}
	id[i] = '\0';
}
*/
// fill: url('#gradient-id');
// fill: url('/#gradient-id');
TM::CXmlTag NSVGparser::ParseAttrUrl(TM::CXmlTag val)
{
    DbgAssert(val.Left(4) == TM_CL("url("));
    if (val[4] == '#')
        val = val.Mid(5);
    else
        val = val.Mid(4);
    val = val.Trim();
    DbgAssert(val.end()[-1] == ')');
    val = val.Left(val.GetLength() - 1);
//    val.CopyTo(id, NSVG_MAX_ID);
    return val;
}
/*

static char nsvg__parseLineCap(const char* str)
{
	if (strcmp(str, "butt") == 0)
		return NSVG_CAP_BUTT;
	else if (strcmp(str, "round") == 0)
		return NSVG_CAP_ROUND;
	else if (strcmp(str, "square") == 0)
		return NSVG_CAP_SQUARE;
	// TODO: handle inherit.
	return NSVG_CAP_BUTT;
}
*/
NSVGlineCap NSVGparser::ParseAttrLineCap(TM::CXmlTag val)
{
    if (val == TM_CL("butt"))
        return NSVG_CAP_BUTT;
    else if (val == TM_CL("round"))
        return NSVG_CAP_ROUND;
    else if (val == TM_CL("square"))
        return NSVG_CAP_SQUARE;
    else
        svg_trace("NSVGparser::ParseAttrLineCap(%s) unknown cap", TM_W2A(val));
    // TODO: handle inherit.
    return NSVG_CAP_BUTT;
}
/*

static char nsvg__parseLineJoin(const char* str)
{
	if (strcmp(str, "miter") == 0)
		return NSVG_JOIN_MITER;
	else if (strcmp(str, "round") == 0)
		return NSVG_JOIN_ROUND;
	else if (strcmp(str, "bevel") == 0)
		return NSVG_JOIN_BEVEL;
	// TODO: handle inherit.
	return NSVG_JOIN_MITER;
}
*/
NSVGlineJoin NSVGparser::ParseAttrLineJoin(TM::CXmlTag val)
{
    if (val == TM_CL("miter"))
        return NSVG_JOIN_MITER;
    else if (val == TM_CL("round"))
        return NSVG_JOIN_ROUND;
    else if (val == TM_CL("bevel"))
        return NSVG_JOIN_BEVEL;
    else
        svg_trace("NSVGparser::ParseAttrLineJoin(%s) unknown jion", TM_W2A(val));
    // TODO: handle inherit.
    return NSVG_JOIN_MITER;
}

NSVGfillRule NSVGparser::ParseAttrFillRule(TM::CXmlTag val)
{
    if (val == TM_CL("nonzero"))
        return NSVG_FILLRULE_NONZERO;
    else if (val == TM_CL("evenodd"))
        return NSVG_FILLRULE_EVENODD;
        svg_trace("NSVGparser::ParseAttrFillRule(%s) unknown rule", TM_W2A(val));
    // TODO: handle inherit.
    return NSVG_FILLRULE_NONZERO;
}

static size_t _CSvgDoc_Splite(TMCStrW p, size_t& cur, size_t nLen)
{
    TMCharW c = p[cur];
    if (c > ' ' && c <= 126 && c != ',')
        return 0;
    size_t n = cur + 1;
    for (; n < nLen; ++n)
    {
        c = p[n];
        if (c > ' ' && c <= 126 && c != ',')
            break;
    }
    return n;
}

// length ::= number (~"em" | ~"ex" | ~"px" | ~"in" | ~"cm" | ~"mm" | ~"pt" | ~"pc")?
// void NSVGparser::ArrayMakePX(CSvgNum* array, int nNum, float orig, float length)
// {
//     for (int i = 0; i < nNum; ++i, ++array)
//     {
//         CXmlTag unit(CXmlTag(array->m_unit).TrimLeft());
//         TMUInt16 x = (unit[0] << 8) | unit[1];
//         switch (x)
//         {
//         case 'px': continue;
//         case 'pt': array->m_val *= (this->dpi / 72.0f); break;
//         case 'pc': array->m_val *= (this->dpi / 6.0f); break;
//         case 'mm': array->m_val *= (this->dpi / 25.4f); break;
//         case 'cm': array->m_val *= (this->dpi / 2.54f); break;
//         case 'in': array->m_val *= this->dpi; break;
//         case 'em': array->m_val *= attr->fontSize; break;
//         case 'ex': array->m_val *= (attr->fontSize * 0.52f); break; // x-height of Helvetica.
//         default:
//             if (unit[0] != '%')
//             {
//                 if (unit.TrimRight().length())
//                     DbgTrace("NSVGparser::ArrayMakePX() unknown unit:%s", TM_W2A(unit));
//                 continue;
//             }
//             array->m_val = orig + array->m_val / 100.0f * length;
//             break;
//         }
//         array->m_unit = L"px";
//     }
// }


TMBool NSVGparser::ArrayMakeAniValue(CSvgAniItem& ani, const CSvgNum* fArray1, const CSvgNum* fArray2, CSvgNum* fOut, int nNum, float tmRate)
{
    for (int i = 0; i < nNum; ++i)
    {
        if (fArray1[i].m_usage != fArray2[i].m_usage)
            return false;
        if (!(fArray1[i].m_usage & eSvgUsage_no_animate))
            fOut[i].m_val = ani.CalcFrameFloat(fArray1[i].m_val, fArray2[i].m_val, tmRate);
    }
    return true;
}

TMBool _CSvgDoc_is_split(wchar_t c)
{
    return (c <= ' ' || c > 126 || c == ',');
}

size_t _CSvgDoc_find_split(TMCStrW bgn, size_t len)
{
    int n = 0;
    for (; n < len; ++n)
    {
        if (_CSvgDoc_is_split(bgn[n]))
            break;
    }
    return n;
}

size_t _CSvgDoc_Skip_split(TMCStrW bgn, size_t start, size_t len)
{
    for (; start < len; ++start)
    {
        if (!_CSvgDoc_is_split(bgn[start]))
            break;
    }
    return start;
}

int NSVGparser::ArrayParse(TM::CXmlTag val, ESvgUsage usage, CSvgNum* array, int nArray)
{
    int i = 0;
    for (; i < nArray && val.length();)
    {
        double x = -DBL_MAX;
        val = val.ScanDouble(x);

        size_t n = _CSvgDoc_find_split(val.data(), val.length());

        if (x != -DBL_MAX)
        {
            array[i].m_usage = usage;
            array[i].m_val = x;
            array[i].m_unit = val.Left(n);
            ++i;
            val = val.Mid(_CSvgDoc_Skip_split(val.data(), n, val.length()));
        }
        else
            break;
    }
    return i;
}

int NSVGparser::ArrayParsePath(TM::CXmlTag val, CSvgNum* array, int nArray)
{
    // A rx ry x-axis-rotation large-arc-flag sweep-flag x y
    // a rx ry x-axis-rotation large-arc-flag sweep-flag dx dy
    // large-arc-flag and sweep-flag are boolean values, so need scan bool
    int nArc = -1; 
    int i = 0;
    int* pCount = 0;
    for (; i < nArray && val.length();)
    {
        double x = -DBL_MAX;
        if (nArc == 3 || nArc == 4) // large-arc-flag, sweep-flag
        {
            val = val.TrimLeft();
            if (val[0] == '0' || val[0] == '1')
            {
                x = val[0] - '0';
                val = val.Mid(1);
            }
        }
        else
            val = val.ScanDouble(x);
        
        if (x != -DBL_MAX)
        {
            DbgAssert(pCount);
            array[i].m_usage = eSvgUsage_user;
            array[i].m_val = x;
            ++*pCount;
            ++i;
            if (nArc >= 0)
            {
                if (++nArc == 8)
                    nArc = 1;
            }
        }
        else
        {
//            val = val.TrimLeft();
            if (!val.length())
                break;
            DbgAssert(val[0] != ' ');
            if (val[0] != ',')
            {
                nArc = (val[0] == 'a' || val[0] == 'A') ? 0 : -1;
                array[i].m_usage = (ESvgUsage)(eSvgUsage_no_animate | val[0]);
                array[i].m_count = 0;
                pCount = &array[i].m_count;
                ++i;
            }
            val = val.Mid(1);
        }
    }
    return i;
}


int NSVGparser::ArrayParseTransform(TM::CXmlTag val, CSvgNum* array)
{
    TM::CXmlTag tag;
    int nSize = 0;
    tm_for (size_t i = 0; i = val.Split(i, ')', tag);)
    {
        tag = tag.Trim();
        int nStep = 0;
        if (tag.Left(6) == TM_CL("matrix"))
        {
            nStep = ArrayParse(tag.Mid(6).Skip('('), eSvgUsage_matrix, array, 6);
            if (nStep != 6)
                nStep = 0;
        }
        else if (tag.Left(9) == TM_CL("translate"))
        {
            if (tag[9] == 'X') // translateX
            {
                nStep = ArrayParse(tag.Mid(10).Skip('('), eSvgUsage_translate, array, 1);
                if (nStep == 1)
                {
                    array[1] = array[0];
                    array[1].m_val = 0;
                    nStep = 2;
                }
            }
            else if (tag[9] == 'Y') // translateY
            {
                nStep = ArrayParse(tag.Mid(10).Skip('('), eSvgUsage_translate, array + 1, 1);
                if (nStep == 1)
                {
                    array[0] = array[1];
                    array[0].m_val = 0;
                    nStep = 2;
                }
            }
            else // translate
            {
                nStep = ArrayParse(tag.Mid(9).Skip('('), eSvgUsage_translate, array, 2);
                if (nStep == 1)
                {
                    array[1] = array[0];
                    nStep = 2;
                }
            }
        }
        else if (tag.Left(5) == TM_CL("scale"))
        {
            if (tag[5] == 'X') // scaleX
            {
                nStep = ArrayParse(tag.Mid(6).Skip('('), eSvgUsage_scale, array, 1);
                if (nStep == 1)
                {
                    array[1] = array[0];
                    array[1].m_val = 1;
                    nStep = 2;
                }
            }
            else if (tag[5] == 'Y') // scaleY
            {
                nStep = ArrayParse(tag.Mid(6).Skip('('), eSvgUsage_scale, array + 1, 1);
                if (nStep == 1)
                {
                    array[0] = array[1];
                    array[0].m_val = 1;
                    nStep = 2;
                }
            }
            else // scale
            {
                nStep = ArrayParse(tag.Mid(5).Skip('('), eSvgUsage_scale, array, 2);
                if (nStep == 1)
                {
                    array[1] = array[0];
                    nStep = 2;
                }
            }
        }
        else if (tag.Left(6) == TM_CL("rotate"))
        {
            nStep = ArrayParse(tag.Mid(6).Skip('('), eSvgUsage_rotate, array, 3);
            if (nStep == 1)
            {
                array[2] = array[1] = array[0];
                array[1].m_val = array[2].m_val = 0;
                nStep = 3;
            }
            else if (nStep != 3)
                nStep = 0;
        }
        else if (tag.Left(5) == TM_CL("skewX"))
        {
            nStep = ArrayParse(tag.Mid(5).Skip('('), eSvgUsage_skewX, array, 1);
        }
        else if (tag.Left(5) == TM_CL("skewY"))
        {
            nStep = ArrayParse(tag.Mid(5).Skip('('), eSvgUsage_skewY, array, 1);
        }
        else
            continue;

        if (nStep == 0)
            continue;
        if (nStep == -1)
            return 0;

        array += nStep;
        nSize += nStep;
    }

    return nSize;
}

void NSVGparser::ParseAniAttrTransform(const TM::CSvgDomAttrib& attr, TM::CAffine& aff)
{
    CAutoMsgAnalysis ama("NSVGparser::ParseAniAttrTransform()");

    CSvgNum array[64];
    int nArray;
    if (attr.m_ani)
    {
        CSvgAniItem i1, i2;
        float tmRate = attr.m_ani->GetFrame(m_stamp, i1, i2);
        nArray = ArrayParseTransform(i1.m_val, array);
        CSvgNum array2[64];
        if (nArray != ArrayParseTransform(i2.m_val, array2))
            return;
        if (!ArrayMakeAniValue(i1, array, array2, array, nArray, tmRate))
            return;
    }
    else
        nArray = ArrayParseTransform(attr.m_val, array);

    CAffine _aff;
    for (int i = 0; i < nArray;)
    {
        switch (array[i].m_usage)
        {
        case eSvgUsage_matrix:
            _aff.Set(
                array[i].m_val, array[i + 1].m_val, array[i + 2].m_val,
                array[i + 3].m_val, array[i + 4].m_val, array[i + 5].m_val);
            i += 6;
            break;
            
        case eSvgUsage_translate:
            _aff.SetTranslate(array[i].m_val, array[i + 1].m_val);
            i+= 2;
            break;
            
        case eSvgUsage_scale:
            _aff.SetScale(array[i].m_val, array[i + 1].m_val);
            i+= 2;
            break;
            
        case eSvgUsage_rotate:
            _aff.Reset();
            _aff.Rotate(array[i].m_val, array[i + 1].m_val, array[i + 2].m_val);
            i+= 3;
            break;
            
        case eSvgUsage_skewX:
            _aff.SetSkewX(array[i].m_val);
            ++i;
            break;
            
        case eSvgUsage_skewY:
            _aff.SetSkewY(array[i].m_val);
            ++i;
            break;
            
        default:
            DbgAssert(0);
            return;
        }
        
        aff.PreCombine(_aff);
    }
}

int NSVGparser::ParseAniAttrArray(const TM::CSvgDomAttrib& attr, CSvgNum* array, int nArray)
{
    CAutoMsgAnalysis ama("NSVGparser::ParseAniAttrArray()");

    int nRet;
    if (attr.m_ani)
    {
        CSvgAniItem i1, i2;
        float tmRate = attr.m_ani->GetFrame(m_stamp, i1, i2);
        nRet = ArrayParse(i1.m_val, eSvgUsage_user, array, nArray);
        CSvgNum* array2 = (CSvgNum*)alloca(nArray * sizeof(CSvgNum));
        if (nRet != ArrayParse(i2.m_val, eSvgUsage_user, array2, nArray))
        {
            svg_trace("NSVGparser::ParseAniAttrArray() ani array data count not match");
            return 0;
        }
        if (!ArrayMakeAniValue(i1, array, array2, array, nRet, tmRate))
        {
            svg_trace("NSVGparser::ParseAniAttrArray() ani array data not match");
            return 0;
        }
    }
    else
        nRet = ArrayParse(attr.m_val, eSvgUsage_user, array, nArray);
    return nRet;
}

int NSVGparser::ParseAniAttrPath(const TM::CSvgDomAttrib& attr, CSvgNum* array, int nArray)
{
    CAutoMsgAnalysis ama("NSVGparser::ParseAniAttrPath()");

    int nRet;
    if (attr.m_ani)
    {
        CSvgAniItem i1, i2;
        float tmRate = attr.m_ani->GetFrame(m_stamp, i1, i2);
        nRet = ArrayParsePath(i1.m_val, array, nArray);
        CSvgNum* array2 = (CSvgNum*)alloca(nArray * sizeof(CSvgNum));
        if (nRet != ArrayParsePath(i2.m_val, array2, nArray))
        {
            svg_trace("NSVGparser::ParseAniAttrArray() ani array data count not match");
            return 0;
        }
        if (!ArrayMakeAniValue(i1, array, array2, array, nRet, tmRate))
        {
            svg_trace("NSVGparser::ParseAniAttrArray() ani array data not match");
            return 0;
        }
    }
    else
        nRet = ArrayParsePath(attr.m_val, array, nArray);
    return nRet;
}


// stroke-dasharray = '20, 10, 5'
// stroke-dasharray = "4 1 2"
int NSVGparser::ParseAniAttrStrokeDashArray(const TM::CSvgDomAttrib& attr, float* strokeDashArray)
{
    if (attr.m_val[0] == 'n')
        return 0;

    CAutoMsgAnalysis ama("NSVGparser::ParseAniAttrStrokeDashArray()");

    CSvgNum array[NSVG_MAX_DASHES];
    int nArray = ParseAniAttrArray(attr, array, NSVG_MAX_DASHES);

    for (int i = 0; i < nArray; ++i)
        strokeDashArray[i] = array[i].m_val;
    return nArray;
}

TMColor NSVGparser::ParseAniAttrColor(const TM::CSvgDomAttrib& attr)
{
    CAutoMsgAnalysis ama("NSVGparser::ParseAniAttrColor()");

    TMColor clr;
    if (attr.m_ani)
    {
        CSvgAniItem i1, i2;
        float tmRate = attr.m_ani->GetFrame(m_stamp, i1, i2);
        TMColor clr1 = XmlParseColor(i1.m_val);
        TMColor clr2 = XmlParseColor(i2.m_val);
        TMUInt8 r1 = ColorR(clr1), r2 = ColorR(clr2);
        TMUInt8 g1 = ColorG(clr1), g2 = ColorG(clr2);
        TMUInt8 b1 = ColorB(clr1), b2 = ColorB(clr2);
        r1 =  (TMUInt8)i1.CalcFrameFloat(r1, r2, tmRate);
        g1 =  (TMUInt8)i1.CalcFrameFloat(g1, g2, tmRate);
        b1 =  (TMUInt8)i1.CalcFrameFloat(b1, b2, tmRate);
        clr = TMRGB(r1,g1,b1);
    }
    else
        clr = XmlParseColor(attr.m_val);

    if (m_pct)
        clr = m_pct->ConvertColor(m_pctID, clr);
    return clr;// ColorToCOLORREF(clr);
}

void NSVGparser::ParseAniAttrFill(const TM::CSvgDomAttrib& attr, CSvgFill& fill)
{
    CAutoMsgAnalysis ama("NSVGparser::ParseAniAttrFill()");

    if (attr.m_val == TM_CL("none"))
        fill.m_type = CSvgFill::eFillNone;
    else if (attr.m_val.Left(4) == TM_CL("url("))
    {
        fill.m_type = CSvgFill::eFillGradient;
        TM::CXmlTag gra = ParseAttrUrl(attr.m_val);
        fill.m_gradient.m_data = gra.data();
        fill.m_gradient.m_size = gra.size();
    }
    else
    {
        fill.m_type = CSvgFill::eFillColor;
        fill.m_Color = ParseAniAttrColor(attr);
    }
}

float NSVGparser::ParseAniAttrfloat(const TM::CSvgDomAttrib& attr)
{
    CAutoMsgAnalysis ama("NSVGparser::ParseAniAttrfloat()");

    CSvgNum num;
    if (1 != ParseAniAttrArray(attr, &num, 1))
    {
        DbgTrace("NSVGparser::ParseAniAttrfloat() error");
        return 1;
    }
    return num.m_val;
}

bool NSVGparser::ParseAttr(const TM::CSvgDomAttrib& attr)
{
    DbgAssert(attr.m_eName != eSvgAttrib_unknown);

    NSVGattrib* statck = GetAttr();
    DbgAssert(statck);

    const TM::CXmlTag& val = attr.m_val;
    
    switch (attr.m_eName)
    {
//     case eSvgAttrib_style:
//         ParseAttrStyle(val);
//         break;

    case eSvgAttrib_display:
        if (val == TM_CL("none"))
            return (statck->visible = 0), true;
        break;

    case eSvgAttrib_fill:
        ParseAniAttrFill(attr, statck->m_fill);
        break;

    case eSvgAttrib_opacity: // <alpha-value>; Default value 1; Animatable Yes;
        statck->opacity *= ParseAniAttrfloat(attr);
        return !statck->IsVisible();

    case eSvgAttrib_fill_opacity:
        statck->fillOpacity = ParseAniAttrfloat(attr);
        break;

    case eSvgAttrib_stroke:
        ParseAniAttrFill(attr, statck->m_stroke);
        break;

    case eSvgAttrib_stroke_width:
        statck->strokeWidth = ParseAniAttrCoordinatePX(attr, 0.0f, GetActualLength());
        break;

    case eSvgAttrib_stroke_dasharray: // none | <dasharray>
        statck->strokeDashCount = ParseAniAttrStrokeDashArray(attr, statck->strokeDashArray);
        break;

    case eSvgAttrib_stroke_dashoffset:
        statck->strokeDashOffset = ParseAniAttrCoordinatePX(attr, 0.0f, GetActualLength());
        break;

    case eSvgAttrib_stroke_opacity:
        statck->strokeOpacity = ParseAniAttrfloat(attr);
        break;

    case eSvgAttrib_stroke_linecap:
        statck->strokeLineCap = ParseAttrLineCap(val);
        break;

    case eSvgAttrib_stroke_linejoin:
        statck->strokeLineJoin = ParseAttrLineJoin(val);
        break;

    case eSvgAttrib_stroke_miterlimit: // <number>; Default value: 4; Animatable: Yes
        statck->miterLimit = ParseAniAttrfloat(attr);
        break;

    case eSvgAttrib_fill_rule:
        statck->fillRule = ParseAttrFillRule(val);
        break;

    case eSvgAttrib_font_size:
        statck->fontSize = ParseAniAttrCoordinatePX(attr, 0.0f, GetActualLength());
        break;

    case eSvgAttrib_stop_color: // currentcolor | <color> <icccolor>; Default value: black; Animatable: Yes
        statck->stopColor = ParseAniAttrColor(attr);
        break;

    case eSvgAttrib_stop_opacity:
        statck->stopOpacity = ParseAniAttrfloat(attr);
        break;

    case eSvgAttrib_offset:
        statck->stopOffset = ParseAniAttrCoordinatePX(attr, 0.0f, 1.0f);
        break;

    case eSvgAttrib_id:
        statck->id = val;   //val.CopyTo(statck->id, NSVG_MAX_ID);
        break;

    case eSvgAttrib_transform:
        ParseAniAttrTransform(attr, statck->xform);
        break;

    default:
        svg_trace("NSVGparser::ParseAttr() unsupport name : %s", _Dbg_ESvgAttrib2Str(attr.m_eName));
        return false;
    }
    return false;
}

void NSVGparser::ParseAttrs(const TM::CSvgDomAttrib* attrs, int nAttr)
{
    SVG_BGN_ATTR(attrs)
        if (0) {}
    SVG_END_ATTR()
}
/*

static int nsvg__getArgsPerElement(char cmd)
{
    switch (cmd) {
        case 'v':
        case 'V':
        case 'h':
        case 'H':
            return 1;
        case 'm':
        case 'M':
        case 'l':
        case 'L':
        case 't':
        case 'T':
            return 2;
        case 'q':
        case 'Q':
        case 's':
        case 'S':
            return 4;
        case 'c':
        case 'C':
            return 6;
        case 'a':
        case 'A':
            return 7;
        case 'z':
        case 'Z':
            return 0;
    }
    return -1;
}
*/

struct CURRENTPOINT
{
    float cx,cy;
    float cx2,cy2;

    void Set(float x, float y)
    {
        cx = cx2 = x;
        cy = cy2 = y;
    }
/*
    
    static void nsvg__pathLineTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
    {
	    if (rel) {
		    *cpx += args[0];
		    *cpy += args[1];
	    } else {
		    *cpx = args[0];
		    *cpy = args[1];
	    }
	    nsvg__lineTo(p, *cpx, *cpy);
    }
*/
    void Set2(CSvgNum* array, bool rel)
    {
        if (rel)
        {
            cx += array[0].m_val;
            cy += array[1].m_val;
        }
        else
        {
            cx = array[0].m_val;
            cy = array[1].m_val;
        }
        cx2 = cx;
        cy2 = cy;
    }
/*
    
    static void nsvg__pathHLineTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
    {
	    if (rel)
		    *cpx += args[0];
	    else
		    *cpx = args[0];
	    nsvg__lineTo(p, *cpx, *cpy);
    }
*/
    void SetX(CSvgNum* array, bool rel)
    {
        if (rel)
            cx += array[0].m_val;
        else
            cx = array[0].m_val;
        cx2 = cx;
    }
/*
    
    static void nsvg__pathVLineTo(NSVGparser* p, float* cpx, float* cpy, float* args, int rel)
    {
	    if (rel)
		    *cpy += args[0];
	    else
		    *cpy = args[0];
	    nsvg__lineTo(p, *cpx, *cpy);
    }
*/
    void SetY(CSvgNum* array, bool rel)
    {
        if (rel)
            cy += array[0].m_val;
        else
            cy = array[0].m_val;
        cy2 = cy;
    }
};

void NSVGparser::PathCubicBezTo(CURRENTPOINT& cp, CSvgNum* array, bool rel)
{
    float x2, y2, cx1, cy1, cx2, cy2;

    if (rel) {
        cx1 = array[0].m_val + cp.cx;   // *cpx + args[0];
        cy1 = array[1].m_val + cp.cy;   // *cpy + args[1];
        cx2 = array[2].m_val + cp.cx;   // *cpx + args[2];
        cy2 = array[3].m_val + cp.cy;   // *cpy + args[3];
        x2  = array[4].m_val + cp.cx;   // *cpx + args[4];
        y2  = array[5].m_val + cp.cy;   // *cpy + args[5];
    } else {
        cx1 = array[0].m_val;
        cy1 = array[1].m_val;
        cx2 = array[2].m_val;
        cy2 = array[3].m_val;
        x2  = array[4].m_val;
        y2  = array[5].m_val;
    }
    
    CubicBezTo(cx1,cy1, cx2,cy2, x2,y2);
    
    cp.cx2 = cx2;
    cp.cy2 = cy2;
    cp.cx = x2;
    cp.cy = y2;
}

void NSVGparser::PathCubicBezShortTo(CURRENTPOINT& cp, CSvgNum* array, bool rel)
{
    float cx2 = array[0].m_val;
    float cy2 = array[1].m_val;
    float x2  = array[2].m_val;
    float y2  = array[3].m_val;
    
    float x1 = cp.cx;
    float y1 = cp.cy;
    if (rel)
    {
        cx2 += cp.cx;
        cy2 += cp.cy;
        x2 += cp.cx;
        y2 += cp.cy;
    }
    
    float cx1 = 2*x1 - cp.cx2;
    float cy1 = 2*y1 - cp.cy2;
    
    CubicBezTo(cx1,cy1, cx2,cy2, x2,y2);
    
    cp.cx2 = cx2;
    cp.cy2 = cy2;
    cp.cx = x2;
    cp.cy = y2;
}

void NSVGparser::PathQuadBezTo(CURRENTPOINT& cp, CSvgNum* array, int rel)
{
    float cx = array[0].m_val;
    float cy = array[1].m_val;
    float x2 = array[2].m_val;
    float y2 = array[3].m_val;
    float x1 = cp.cx;
    float y1 = cp.cy;
    
    if (rel)
    {
        cx += cp.cx;
        cy += cp.cy;
        x2 += cp.cx;
        y2 += cp.cy;
    }
    
    // Convert to cubic bezier
    float cx1 = x1 + 2.0f/3.0f*(cx - x1);
    float cy1 = y1 + 2.0f/3.0f*(cy - y1);
    float cx2 = x2 + 2.0f/3.0f*(cx - x2);
    float cy2 = y2 + 2.0f/3.0f*(cy - y2);
    
    CubicBezTo(cx1,cy1, cx2,cy2, x2,y2);
    
    cp.cx2 = cx;
    cp.cy2 = cy;
    cp.cx = x2;
    cp.cy = y2;
}

void NSVGparser::PathQuadBezShortTo(CURRENTPOINT& cp, CSvgNum* array, int rel)
{
    float x2 = array[0].m_val;
    float y2 = array[1].m_val;
    float x1 = cp.cx;
    float y1 = cp.cy;

    if (rel)
    {
        x2 += cp.cx;
        y2 += cp.cy;
    }
    
    float cx = 2*x1 - cp.cx2;
    float cy = 2*y1 - cp.cy2;
    
    // Convert to cubix bezier
    float cx1 = x1 + 2.0f/3.0f*(cx - x1);
    float cy1 = y1 + 2.0f/3.0f*(cy - y1);
    float cx2 = x2 + 2.0f/3.0f*(cx - x2);
    float cy2 = y2 + 2.0f/3.0f*(cy - y2);
    
    CubicBezTo(cx1,cy1, cx2,cy2, x2,y2);
    
    cp.cx2 = cx;
    cp.cy2 = cy;
    cp.cx = x2;
    cp.cy = y2;
}

static float nsvg__sqr(float x) { return x*x; }
static float nsvg__vmag(float x, float y) { return sqrtf(x*x + y*y); }

static float nsvg__vecrat(float ux, float uy, float vx, float vy)
{
    return (ux*vx + uy*vy) / (nsvg__vmag(ux,uy) * nsvg__vmag(vx,vy));
}

static float nsvg__vecang(float ux, float uy, float vx, float vy)
{
    float r = nsvg__vecrat(ux,uy, vx,vy);
    if (r < -1.0f) r = -1.0f;
    if (r > 1.0f) r = 1.0f;
    return ((ux*vy < uy*vx) ? -1.0f : 1.0f) * acosf(r);
}

void NSVGparser::PathArcTo(CURRENTPOINT& cp, CSvgNum* array, int rel)
{
    CAutoMsgAnalysis ama("NSVGparser::PathArcTo()");

    // Ported from canvg (https://code.google.com/p/canvg/)
    float rx = fabsf(array[0].m_val);                   // y radius
    float ry = fabsf(array[1].m_val);                   // x radius
    float rotx = array[2].m_val / 180.0f * NSVG_PI;     // x rotation angle
    float fa = fabsf(array[3].m_val) > 1e-6 ? 1 : 0;    // Large arc
    float fs = fabsf(array[4].m_val) > 1e-6 ? 1 : 0;    // Sweep direction
    float x2 = array[5].m_val;
    float y2 = array[6].m_val;

//  float x1, y1, x2, y2, cx, cy, dx, dy, d;
//  float x1p, y1p, cxp, cyp, s, sa, sb;
//  float ux, uy, vx, vy, a1, da;
//  float a, px = 0, py = 0, ptanx = 0, ptany = 0;//, t[6];
//  float sinrx, cosrx;
//  int i, ndivs;
//  float hda, kappa;

    float x1 = cp.cx;                           // start point
    float y1 = cp.cy;
    if (rel)    {                           // end point
        x2 += cp.cx;
        y2 += cp.cy;
    }

    float dx = x1 - x2;
    float dy = y1 - y2;
    float d = sqrtf(dx*dx + dy*dy);
    cp.Set(x2, y2);
    if (d < 1e-6f || rx < 1e-6f || ry < 1e-6f) {
        // The arc degenerates to a line
        LineTo(x2, y2);
        return;
    }

    float sinrx = sinf(rotx);
    float cosrx = cosf(rotx);

    // Convert to center point parameterization.
    // http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes
    // 1) Compute x1', y1'
    float x1p = cosrx * dx / 2.0f + sinrx * dy / 2.0f;
    float y1p = -sinrx * dx / 2.0f + cosrx * dy / 2.0f;
    d = nsvg__sqr(x1p)/nsvg__sqr(rx) + nsvg__sqr(y1p)/nsvg__sqr(ry);
    if (d > 1) {
        d = sqrtf(d);
        rx *= d;
        ry *= d;
    }
    // 2) Compute cx', cy'
    float s = 0.0f;
    float sa = nsvg__sqr(rx)*nsvg__sqr(ry) - nsvg__sqr(rx)*nsvg__sqr(y1p) - nsvg__sqr(ry)*nsvg__sqr(x1p);
    float sb = nsvg__sqr(rx)*nsvg__sqr(y1p) + nsvg__sqr(ry)*nsvg__sqr(x1p);
    if (sa < 0.0f) sa = 0.0f;
    if (sb > 0.0f)
        s = sqrtf(sa / sb);
    if (fa == fs)
        s = -s;
    float cxp = s * rx * y1p / ry;
    float cyp = s * -ry * x1p / rx;

    // 3) Compute cx,cy from cx',cy'
    float cx = (x1 + x2)/2.0f + cosrx*cxp - sinrx*cyp;
    float cy = (y1 + y2)/2.0f + sinrx*cxp + cosrx*cyp;

    // 4) Calculate theta1, and delta theta.
    float ux = (x1p - cxp) / rx;
    float uy = (y1p - cyp) / ry;
    float vx = (-x1p - cxp) / rx;
    float vy = (-y1p - cyp) / ry;
    float a1 = nsvg__vecang(1.0f,0.0f, ux,uy);  // Initial angle
    float da = nsvg__vecang(ux,uy, vx,vy);      // Delta angle

//  if (vecrat(ux,uy,vx,vy) <= -1.0f) da = NSVG_PI;
//  if (vecrat(ux,uy,vx,vy) >= 1.0f) da = 0;

    if (fs == 0 && da > 0)
        da -= 2 * NSVG_PI;
    else if (fs == 1 && da < 0)
        da += 2 * NSVG_PI;

    // Approximate the arc using cubic spline segments.
    TM::CAffine t(cosrx, sinrx, -sinrx, cosrx, cx, cy); //  t[0] = cosrx; t[1] = sinrx;
                                                        //  t[2] = -sinrx; t[3] = cosrx;
                                                        //  t[4] = cx; t[5] = cy;

    // Split arc into max 90 degree segments.
    // The loop assumes an iteration per end point (including start and end), this +1.
    float ndivs = (int)(fabsf(da) / (NSVG_PI*0.5f) + 1.0f);
    float hda = (da / (float)ndivs) / 2.0f;
    // Fix for ticket #179: division by 0: avoid cotangens around 0 (infinite)
    if ((hda < 1e-3f) && (hda > -1e-3f))
        hda *= 0.5f;
    else
        hda = (1.0f - cosf(hda)) / sinf(hda);
    float kappa = fabsf(4.0f / 3.0f * hda);
    if (da < 0.0f)
        kappa = -kappa;

    float px = 0, py = 0, ptanx = 0, ptany = 0;
    for (int i = 0; i <= ndivs; i++) {
        float a = a1 + da * ((float)i/(float)ndivs);
        dx = cosf(a);
        dy = sinf(a);
//      nsvg__xformPoint(&x, &y, dx*rx, dy*ry, t); // position
//      nsvg__xformVec(&tanx, &tany, -dy*rx * kappa, dx*ry * kappa, t); // tangent
//      if (i > 0)
//          nsvg__cubicBezTo(px+ptanx,py+ptany, x-tanx, y-tany, x, y);
//      px = x;
//      py = y;
//      ptanx = tanx;
//      ptany = tany;

        TM::CPointF xy;
        TM::CSizeF tanxy;
        t.LPtoDP(TM::CPointF(dx*rx, dy*ry), xy); // position
        t.MapSize(TM::CSizeF(-dy*rx * kappa, dx*ry * kappa), tanxy); // tangent
        if (i > 0)
            CubicBezTo(px+ptanx,py+ptany, xy.X()-tanxy.Width(), xy.Y()-tanxy.Height(), xy.X(), xy.Y());
        px = xy.X();
        py = xy.Y();
        ptanx = tanxy.Width();
        ptany = tanxy.Height();
    }
}

void NSVGparser::ParseNode_path(const TM::CSvgDomAttrib* attrs, int nAttr)
{
    const TM::CSvgDomAttrib* d = 0;

    SVG_BGN_ATTR(attrs)
        if (atype == eSvgAttrib_d) // <string>; Default value: none; Animatable: Yes;
            d = &attr;
    SVG_END_ATTR()

    if (d)
    {
        ResetPath();
        CURRENTPOINT cp = {0};
        bool closedFlag = false;

        int nCount = d->m_val.length();
        CSvgNum* array = (CSvgNum*)alloca(nCount * sizeof(CSvgNum));
        nCount = ParseAniAttrPath(*d, array, nCount);
        
        TMCharW cmd;
        for (int i = 0; i < nCount;)
        {
            TMCharW cmdTemp = array[i].m_usage;
            if (cmdTemp != eSvgUsage_user)
            {
                ++i;
                cmd = cmdTemp & eSvgUsage_path_mask;
            }
            switch (cmd)
            {
            // MoveTo
            case 'm':
            case 'M':
                if (cmdTemp != eSvgUsage_user)
                    DbgAssert(0 == array[i-1].m_count % 2);
                
                // process last path
                if (i > 1)
                    this->AddPath(closedFlag);
                // Start new subpath.
                this->ResetPath();
                closedFlag = 0;

                // begin new path
                cp.Set2(array + i, cmd == 'm');
                this->MoveTo(cp.cx, cp.cy);
                cmd = 'l' + cmd - 'm'; // convert next array to LineTo
                i += 2;
                break;

            // LineTo
            case 'l':
            case 'L':
                if (cmdTemp != eSvgUsage_user)
                    DbgAssert(0 == array[i-1].m_count % 2);
                cp.Set2(array + i, cmd == 'l');
                this->LineTo(cp.cx, cp.cy);
                i += 2;
                break;
            case 'H':
            case 'h':
                cp.SetX(array + i, cmd == 'h');
                this->LineTo(cp.cx, cp.cy);
                i += 1;
                break;
            case 'V':
            case 'v':
                cp.SetY(array + i, cmd == 'v');
                this->LineTo(cp.cx, cp.cy);
                i += 1;
                break;
            
            // Cubic Bezier Curve:
            case 'C':
            case 'c':
                if (cmdTemp != eSvgUsage_user)
                    DbgAssert(0 == array[i-1].m_count % 6);
                PathCubicBezTo(cp, array + i, cmd == 'c');
                i += 6;
                break;
            case 'S':
            case 's':
                if (cmdTemp != eSvgUsage_user)
                    DbgAssert(0 == array[i-1].m_count % 4);
                PathCubicBezShortTo(cp, array + i, cmd == 's');
                i += 4;
                break;

            // Quadratic Bezier Curve
            case 'Q':
            case 'q':
                if (cmdTemp != eSvgUsage_user)
                    DbgAssert(0 == array[i-1].m_count % 4);
                PathQuadBezTo(cp, array + i, cmd == 'q');
                i += 4;
                break;
            case 'T':
            case 't':
                if (cmdTemp != eSvgUsage_user)
                    DbgAssert(0 == array[i-1].m_count % 2);
                PathQuadBezShortTo(cp, array + i, cmd == 't');
                i += 2;
                break;

            // Elliptical Arc Curve
            case 'A':
            case 'a':
                if (cmdTemp != eSvgUsage_user)
                    DbgAssert(0 == array[i-1].m_count % 7);
                PathArcTo(cp, array + i, cmd == 'a');
                i += 7;
                break;

            case 'Z':
            case 'z':
                DbgAssert(array[i-1].m_count == 0);
                closedFlag = true;
                // Commit path.
                if (this->npts > 0)
                {
                    // Move current point to first point
                    //                      cpx = this->pts[0];
                    //                      cpy = this->pts[1];
                    cp.Set(this->pts[0].X(), this->pts[0].Y());
                    AddPath(closedFlag);
                }
                // Start new subpath.
                ResetPath();
                MoveTo(cp.cx, cp.cy);
                closedFlag = 0;
                break;

            default:
                DbgAssert(0);
                break;
            }

//            i += (1 + array[i].m_count);
        }

        // Commit path.
        if (this->npts)
            AddPath(closedFlag);
    }

    nsvg__addShape(this);
}


void NSVGparser::ParseNode_rect(const TM::CSvgDomAttrib* attrs, int nAttr)
{
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
    float rx = -1.0f; // marks not set
    float ry = -1.0f;
    
    SVG_BGN_ATTR(attrs)
        if (atype == eSvgAttrib_x)
            x = ParseAniAttrCoordinatePX(attr, GetActualOrigX(), GetActualWidth());
        else if (atype == eSvgAttrib_y)
            y = ParseAniAttrCoordinatePX(attr, GetActualOrigY(), GetActualHeight());
        else if (atype == eSvgAttrib_width)
            w = ParseAniAttrCoordinatePX(attr, 0.0f, GetActualWidth());
        else if (atype == eSvgAttrib_height)
            h = ParseAniAttrCoordinatePX(attr, 0.0f, GetActualHeight());
        else if (atype == eSvgAttrib_rx)
            rx = fabsf(ParseAniAttrCoordinatePX(attr, 0.0f, GetActualWidth()));
        else if (atype == eSvgAttrib_ry)
            ry = fabsf(ParseAniAttrCoordinatePX(attr, 0.0f, GetActualHeight()));
    SVG_END_ATTR()

    if (rx < 0.0f && ry > 0.0f) rx = ry;
    if (ry < 0.0f && rx > 0.0f) ry = rx;
    if (rx < 0.0f) rx = 0.0f;
    if (ry < 0.0f) ry = 0.0f;
    if (rx > w/2.0f) rx = w/2.0f;
    if (ry > h/2.0f) ry = h/2.0f;
    
    if (w != 0.0f && h != 0.0f)
    {
        ResetPath();
        
        if (rx < 0.00001f || ry < 0.0001f)
        {
            MoveTo(x, y);
            LineTo(x+w, y);
            LineTo(x+w, y+h);
            LineTo(x, y+h);
        }
        else
        {
            // Rounded rectangle
            MoveTo(x+rx, y);
            LineTo(x+w-rx, y);
            CubicBezTo(x+w-rx*(1-NSVG_KAPPA90), y, x+w, y+ry*(1-NSVG_KAPPA90), x+w, y+ry);
            LineTo(x+w, y+h-ry);
            CubicBezTo(x+w, y+h-ry*(1-NSVG_KAPPA90), x+w-rx*(1-NSVG_KAPPA90), y+h, x+w-rx, y+h);
            LineTo(x+rx, y+h);
            CubicBezTo(x+rx*(1-NSVG_KAPPA90), y+h, x, y+h-ry*(1-NSVG_KAPPA90), x, y+h-ry);
            LineTo(x, y+ry);
            CubicBezTo(x, y+ry*(1-NSVG_KAPPA90), x+rx*(1-NSVG_KAPPA90), y, x+rx, y);
        }
        
        AddPath(1);
        
        nsvg__addShape(this);
    }
}

void NSVGparser::ParseNode_circle(const TM::CSvgDomAttrib* attrs, int nAttr)
{
    float cx = 0.0f;
    float cy = 0.0f;
    float r = 0.0f;
    
    SVG_BGN_ATTR(attrs)
        if (atype == eSvgAttrib_cx)
            cx = ParseAniAttrCoordinatePX(attr, GetActualOrigX(), GetActualWidth());
        else if (atype == eSvgAttrib_cy)
            cy = ParseAniAttrCoordinatePX(attr, GetActualOrigY(), GetActualHeight());
        else if (atype == eSvgAttrib_r)
            r = fabsf(ParseAniAttrCoordinatePX(attr, 0.0f, GetActualLength()));
    SVG_END_ATTR()

    if (r > 0.0f) {
        ResetPath();
        
        MoveTo(cx+r, cy);
        CubicBezTo(cx+r, cy+r*NSVG_KAPPA90, cx+r*NSVG_KAPPA90, cy+r, cx, cy+r);
        CubicBezTo(cx-r*NSVG_KAPPA90, cy+r, cx-r, cy+r*NSVG_KAPPA90, cx-r, cy);
        CubicBezTo(cx-r, cy-r*NSVG_KAPPA90, cx-r*NSVG_KAPPA90, cy-r, cx, cy-r);
        CubicBezTo(cx+r*NSVG_KAPPA90, cy-r, cx+r, cy-r*NSVG_KAPPA90, cx+r, cy);
        
        AddPath(1);
        
        nsvg__addShape(this);
    }
}

void NSVGparser::ParseNode_ellipse(const TM::CSvgDomAttrib* attrs, int nAttr)
{
    float cx = 0.0f;
    float cy = 0.0f;
    float rx = 0.0f;
    float ry = 0.0f;


    SVG_BGN_ATTR(attrs)
        if (atype == eSvgAttrib_cx)
            cx = ParseAniAttrCoordinatePX(attr, GetActualOrigX(), GetActualWidth());
        else if (atype == eSvgAttrib_cy)
            cy = ParseAniAttrCoordinatePX(attr, GetActualOrigY(), GetActualHeight());
        else if (atype == eSvgAttrib_rx)
            rx = fabsf(ParseAniAttrCoordinatePX(attr, 0.0f, GetActualWidth()));
        else if (atype == eSvgAttrib_ry)
            ry = fabsf(ParseAniAttrCoordinatePX(attr, 0.0f, GetActualHeight()));
    SVG_END_ATTR()

    if (rx > 0.0f && ry > 0.0f) {
        
        ResetPath();
        
        MoveTo(cx+rx, cy);
        CubicBezTo(cx+rx, cy+ry*NSVG_KAPPA90, cx+rx*NSVG_KAPPA90, cy+ry, cx, cy+ry);
        CubicBezTo(cx-rx*NSVG_KAPPA90, cy+ry, cx-rx, cy+ry*NSVG_KAPPA90, cx-rx, cy);
        CubicBezTo(cx-rx, cy-ry*NSVG_KAPPA90, cx-rx*NSVG_KAPPA90, cy-ry, cx, cy-ry);
        CubicBezTo(cx+rx*NSVG_KAPPA90, cy-ry, cx+rx, cy-ry*NSVG_KAPPA90, cx+rx, cy);
        
        AddPath(1);
        
        nsvg__addShape(this);
    }
}

void NSVGparser::ParseNode_line(const TM::CSvgDomAttrib* attrs, int nAttr)
{
    float x1 = 0.0;
    float y1 = 0.0;
    float x2 = 0.0;
    float y2 = 0.0;

    SVG_BGN_ATTR(attrs)
        if (atype == eSvgAttrib_x1)
            x1 = ParseAniAttrCoordinatePX(attr, GetActualOrigX(), GetActualWidth());
        else if (atype == eSvgAttrib_y1)
            y1 = ParseAniAttrCoordinatePX(attr, GetActualOrigY(), GetActualHeight());
        else if (atype == eSvgAttrib_x2)
            x2 = ParseAniAttrCoordinatePX(attr, GetActualOrigX(), GetActualWidth());
        else if (atype == eSvgAttrib_y2)
            y2 = ParseAniAttrCoordinatePX(attr, GetActualOrigY(), GetActualHeight());
    SVG_END_ATTR()
    
    ResetPath();
    
    MoveTo(x1, y1);
    LineTo(x2, y2);
    
    AddPath(0);
    
    nsvg__addShape(this);
}

void NSVGparser::ParseNode_poly(const TM::CSvgDomAttrib* attrs, int nAttr, bool closeFlag)
{
    ResetPath();
    
    SVG_BGN_ATTR(attrs)
        if (atype == eSvgAttrib_points) // [ <number>+ ]#
        {
            int nArray = INT_MAX;
            SVG_DECALRE_ARRRY(aval, array, nArray);
            nArray = ParseAniAttrArray(attr, array, nArray);
            if (nArray > 3)
            {
                MoveTo(array[0].m_val, array[1].m_val);
                for (int i = 2; i < nArray; i += 2)
                    LineTo(array[i].m_val, array[i + 1].m_val);
            }
        }
    SVG_END_ATTR()
    
    AddPath((char)closeFlag);
    
    nsvg__addShape(this);
}

void NSVGparser::ParseNode_use(const TM::CSvgDomAttrib* attrs, int nAttr)
{
    SVG_BGN_ATTR(attrs)
        if (atype == eSvgAttrib_xlink_href || atype == eSvgAttrib_href)
        {
            TM::CSvgDomNode* node = FindNode(aval.Mid(1));
            if (node)
            {
                int nSizeOld = node->m_attribs.size();
                for (int x = 0; x < nAttr; ++x)
                {
                    ESvgAttrib _eName = attrs[x].m_eName;
                    if (_eName != eSvgAttrib_xlink_href && _eName != eSvgAttrib_href && _eName != eSvgAttrib_id)
                        node->m_attribs.push_back(attrs[x]);
                }
                ParseNodes(node);
                node->m_attribs.resize(nSizeOld);
            }
            return;
        }
    SVG_END_ATTR2()
}

void NSVGparser::ParseNode_svg(const TM::CSvgDomAttrib* attrs, int nAttr)
{
    SVG_BGN_ATTR(attrs)
        if (atype == eSvgAttrib_width)
            this->image->width = ParseAniAttrCoordinatePX(attr, 0.0f, 0.0f); // auto | <length> | <percentage>, auto (treated as 100%)
        else if (atype == eSvgAttrib_height)
            this->image->height = ParseAniAttrCoordinatePX(attr, 0.0f, 0.0f);
        else if (atype == eSvgAttrib_viewBox) // <number>?, <number>?, <number>?, <number>
        {
            CSvgNum f[4];
            int n = ArrayParse(aval, eSvgUsage_user, f, 4); // ParseCoordArray
            if (n > 0)
                this->viewMinx = f[0].m_val;
            if (n > 1)
                this->viewMiny = f[1].m_val;
            if (n > 2)
                this->viewWidth = f[2].m_val;
            if (n > 3)
                this->viewHeight = f[3].m_val;
        }
        else if (atype == eSvgAttrib_preserveAspectRatio)
        {
            if (aval.Find(L"none") >= 0)
            {
                // No uniform scaling
                this->alignType = NSVG_ALIGN_NONE;
            }
            else
            {
                // Parse X align
                if (aval.Find(L"xMin") >= 0)
                    this->alignX = NSVG_ALIGN_MIN;
                else if (aval.Find(L"xMid") >= 0)
                    this->alignX = NSVG_ALIGN_MID;
                else if (aval.Find(L"xMax") >= 0)
                    this->alignX = NSVG_ALIGN_MAX;

                // Parse X align
                if (aval.Find(L"yMin") >= 0)
                    this->alignY = NSVG_ALIGN_MIN;
                else if (aval.Find(L"yMid") >= 0)
                    this->alignY = NSVG_ALIGN_MID;
                else if (aval.Find(L"yMax") >= 0)
                    this->alignY = NSVG_ALIGN_MAX;

                // Parse meet/slice
                this->alignType = NSVG_ALIGN_MEET;
                if (aval.Find(L"slice") >= 0)
                    this->alignType = NSVG_ALIGN_SLICE;
            }
        }
    SVG_END_ATTR()
}

NSVGgradientData::NSVGgradientData(NSVGpaintType eType)
    : units(NSVG_OBJECT_SPACE)
    , type(eType)
    , spread(NSVG_SPREAD_PAD)
    , nstops(0)
    , stops(0)
    , next(0)
{
//     this->id[0] = 0;
//     this->ref[0] = 0;
    this->xform.Reset();// nsvg__xformIdentity(this->xform);

    if (eType == NSVG_PAINT_LINEAR_GRADIENT) {
        this->linear.x1 = nsvg__coord(0.0f, NSVG_UNITS_PERCENT);
        this->linear.y1 = nsvg__coord(0.0f, NSVG_UNITS_PERCENT);
        this->linear.x2 = nsvg__coord(100.0f, NSVG_UNITS_PERCENT);
        this->linear.y2 = nsvg__coord(0.0f, NSVG_UNITS_PERCENT);
    }
    else if (eType == NSVG_PAINT_RADIAL_GRADIENT) {
        this->radial.cx = nsvg__coord(50.0f, NSVG_UNITS_PERCENT);
        this->radial.cy = nsvg__coord(50.0f, NSVG_UNITS_PERCENT);
        this->radial.r = nsvg__coord(50.0f, NSVG_UNITS_PERCENT);
    }
    else
        DbgAssert(0);
}


void NSVGparser::ParseNode_gradient(const TM::CSvgDomAttrib* attrs, int nAttr, NSVGpaintType type)
{
    NSVGgradientData* grad = new NSVGgradientData(type);

    SVG_BGN_ATTR(attrs)
        switch (atype)
        {
        case eSvgAttrib_id:
            grad->id = aval;    // aval.CopyTo(grad->id, NSVG_MAX_ID);
            break;

        case eSvgAttrib_gradientUnits:
            grad->units = (aval == TM_CL("objectBoundingBox")) ? NSVG_OBJECT_SPACE : NSVG_USER_SPACE;
            break;

        case eSvgAttrib_cx:
            grad->radial.cx = ParseAniAttrCoordinate(attr);
            break;

        case eSvgAttrib_cy:
            grad->radial.cy = ParseAniAttrCoordinate(attr);
            break;

        case eSvgAttrib_r:
            grad->radial.r = ParseAniAttrCoordinate(attr);
            break;

        case eSvgAttrib_fx:
            grad->radial.fx = ParseAniAttrCoordinate(attr);
            break;

        case eSvgAttrib_fy:
            grad->radial.fy = ParseAniAttrCoordinate(attr);
            break;

        case eSvgAttrib_x1:
            grad->linear.x1 = ParseAniAttrCoordinate(attr);
            break;

        case eSvgAttrib_y1:
            grad->linear.y1 = ParseAniAttrCoordinate(attr);
            break;

        case eSvgAttrib_x2:
            grad->linear.x2 = ParseAniAttrCoordinate(attr);
            break;

        case eSvgAttrib_y2:
            grad->linear.y2 = ParseAniAttrCoordinate(attr);
            break;

        case eSvgAttrib_spreadMethod:
            {
                if (aval == TM_CL("pad"))
                    grad->spread = NSVG_SPREAD_PAD;
                else if (aval == TM_CL("reflect"))
                    grad->spread = NSVG_SPREAD_REFLECT;
                else if (aval == TM_CL("repeat"))
                    grad->spread = NSVG_SPREAD_REPEAT;
            }
        break;

        case eSvgAttrib_xlink_href:
        case eSvgAttrib_href:
            grad->ref = aval.Mid(1);    // .CopyTo(grad->ref, NSVG_MAX_ID); // "<use xlink:href="#heart"> "
            break;

        case eSvgAttrib_gradientTransform:
            grad->xform.Reset();
            ParseAniAttrTransform(attr, grad->xform);
            break;

        default:
            svg_trace("NSVGparser::ParseNode_gradient() unsupport attr : %s", _Dbg_ESvgAttrib2Str(atype));
            break;
        }
    SVG_END_ATTR2()
    
    grad->next = this->gradients;
    this->gradients = grad;
}

void NSVGparser::ParseNode_gradientStop(const TM::CSvgDomAttrib* attrs, int nAttr)
{
    NSVGattrib* curAttr = GetAttr();
    NSVGgradientData* grad;
    NSVGgradientStop* stop;
    int i, idx;
    
    curAttr->stopOffset = 0;
    curAttr->stopColor = 0;
    curAttr->stopOpacity = 1.0f;
    
    ParseAttrs(attrs, nAttr);
    
    // Add stop to the last gradient.
    grad = this->gradients;
    if (grad == NULL) return;
    
    grad->nstops++;
    grad->stops = (NSVGgradientStop*)realloc(grad->stops, sizeof(NSVGgradientStop)*grad->nstops);
    if (grad->stops == NULL) return;
    
    // Insert
    idx = grad->nstops-1;
    for (i = 0; i < grad->nstops-1; i++) {
        if (curAttr->stopOffset < grad->stops[i].offset) {
            idx = i;
            break;
        }
    }
    if (idx != grad->nstops-1) {
        for (i = grad->nstops-1; i > idx; i--)
            grad->stops[i] = grad->stops[i-1];
    }
    
    stop = &grad->stops[idx];
    stop->color = curAttr->stopColor;
    ColorSetA(stop->color, curAttr->stopOpacity*255);// stop->color |= (unsigned int)(curAttr->stopOpacity*255) << 24;
    stop->offset = curAttr->stopOffset;
}

TMBool NSVGparser::ParseNode(ESvgTag tag, const TM::CSvgDomAttrib* attrs, int nAttr)
{
    if (tag == eSvgTag_linearGradient)
        return ParseNode_gradient(attrs, nAttr, NSVG_PAINT_LINEAR_GRADIENT), false;
    else if (tag == eSvgTag_radialGradient)
        return ParseNode_gradient(attrs, nAttr, NSVG_PAINT_RADIAL_GRADIENT), false;
    else if (tag == eSvgTag_stop)
        return ParseNode_gradientStop(attrs, nAttr), false;
    else if (this->defsFlag)
    {
        svg_trace("NSVGparser::ParseNode() unsupport tag : defs::%s", _Dbg_ESvgTag2Str(tag));
        return false;
    }

    switch (tag)
    {
    case eSvgTag_g:
        PushAttr();
        ParseAttrs(attrs, nAttr);
        return true;

    case eSvgTag_path:
        PushAttr();
        ParseNode_path(attrs, nAttr);
        return true;

    case eSvgTag_rect:
        PushAttr();
        ParseNode_rect(attrs, nAttr);
        return true;

    case eSvgTag_circle:
        PushAttr();
        ParseNode_circle(attrs, nAttr);
        return true;

    case eSvgTag_ellipse:
        PushAttr();
        ParseNode_ellipse(attrs, nAttr);
        return true;

    case eSvgTag_line:
        PushAttr();
        ParseNode_line(attrs, nAttr);
        return true;

    case eSvgTag_polyline:
        PushAttr();
        ParseNode_poly(attrs, nAttr, false);
        return true;

    case eSvgTag_polygon:
        PushAttr();
        ParseNode_poly(attrs, nAttr, true);
        return true;

//     case eSvgTag_linearGradient:
//         ParseNode_gradient(attrs, nAttr, NSVG_PAINT_LINEAR_GRADIENT);
//         return false;
// 
//     case eSvgTag_radialGradient:
//         ParseNode_gradient(attrs, nAttr, NSVG_PAINT_RADIAL_GRADIENT);
//         return false;
// 
//     case eSvgTag_stop:
//         ParseNode_gradientStop(attrs, nAttr);
//         return false;

    case eSvgTag_defs:
        this->defsFlag = 1;
        return false;

//     case eSvgTag_symbol: // need't parse
//         return false;

    case eSvgTag_use:
        ParseNode_use(attrs, nAttr);
        return false;

    case eSvgTag_svg:
        ParseNode_svg(attrs, nAttr);
        return false;

    default:
        svg_trace("NSVGparser::ParseNode() unsupport tag : %s", _Dbg_ESvgTag2Str(tag));
        return false;
    }
}

static void nsvg__imageBounds(NSVGparser* p, TM::CRectF& bounds)
{
    NSVGshape* shape;
    shape = p->image->shapes;
    if (shape == NULL) {
        bounds.SetRectEmpty();// bounds[0] = bounds[1] = bounds[2] = bounds[3] = 0.0;
        return;
    }
    bounds = shape->bounds;         // bounds[0] = shape->bounds[0];
                                    // bounds[1] = shape->bounds[1];
                                    // bounds[2] = shape->bounds[2];
                                    // bounds[3] = shape->bounds[3];
    
    for (shape = shape->next; shape != NULL; shape = shape->next) {
        bounds |= shape->bounds;    // bounds[0] = nsvg__minf(bounds[0], shape->bounds[0]);
                                    // bounds[1] = nsvg__minf(bounds[1], shape->bounds[1]);
                                    // bounds[2] = nsvg__maxf(bounds[2], shape->bounds[2]);
                                    // bounds[3] = nsvg__maxf(bounds[3], shape->bounds[3]);
    }
}

static float nsvg__viewAlign(float content, float container, int type)
{
    if (type == NSVG_ALIGN_MIN)
        return 0;
    else if (type == NSVG_ALIGN_MAX)
        return container - content;
    // mid
    return (container - content) * 0.5f;
}

void NSVGgradient::Scale(float tx, float ty, float sx, float sy)
{
    TM::CAffine aff;
    aff.SetTranslate(tx, ty);
    this->xform.Combine(aff);
    aff.SetScale(sx, sy);
    this->xform.Combine(aff);
}

void NSVGparser::ScaleToViewbox(NSVGunits units)
{
    NSVGshape* shape;
    NSVGpath* path;
    float tx, ty, sx, sy, us, avgs;
    int i;
    NSVGparser* p = this;
    
    // Guess image size if not set completely.
    TM::CRectF bounds;
    nsvg__imageBounds(p, bounds);
    
    if (p->viewWidth == 0) {
        if (p->image->width > 0) {
            p->viewWidth = p->image->width;
        } else {
            p->viewMinx = bounds.Left();        // bounds[0];
            p->viewWidth = bounds.Width();      // bounds[2] - bounds[0];
        }
    }
    if (p->viewHeight == 0) {
        if (p->image->height > 0) {
            p->viewHeight = p->image->height;
        } else {
            p->viewMiny = bounds.Top();         // bounds[1];
            p->viewHeight = bounds.Height();    // bounds[3] - bounds[1];
        }
    }
    if (p->image->width == 0)
        p->image->width = p->viewWidth;
    if (p->image->height == 0)
        p->image->height = p->viewHeight;
    
    tx = -p->viewMinx;
    ty = -p->viewMiny;
    sx = p->viewWidth > 0 ? p->image->width / p->viewWidth : 0;
    sy = p->viewHeight > 0 ? p->image->height / p->viewHeight : 0;
    // Unit scaling
    us = 1.0f / ConvertToPixels(nsvg__coord(1.0f, units), 0.0f, 1.0f); // nsvg__convertToPixels(p, nsvg__coord(1.0f, nsvg__parseUnits(units)), 0.0f, 1.0f);
    
    // Fix aspect ratio
    if (p->alignType == NSVG_ALIGN_MEET) {
        // fit whole image into viewbox
        sx = sy = nsvg__minf(sx, sy);
        tx += nsvg__viewAlign(p->viewWidth*sx, p->image->width, p->alignX) / sx;
        ty += nsvg__viewAlign(p->viewHeight*sy, p->image->height, p->alignY) / sy;
    } else if (p->alignType == NSVG_ALIGN_SLICE) {
        // fill whole viewbox with image
        sx = sy = nsvg__maxf(sx, sy);
        tx += nsvg__viewAlign(p->viewWidth*sx, p->image->width, p->alignX) / sx;
        ty += nsvg__viewAlign(p->viewHeight*sy, p->image->height, p->alignY) / sy;
    }
    
    // Transform
    sx *= us;
    sy *= us;
    avgs = (sx+sy) / 2.0f;
    for (shape = p->image->shapes; shape != NULL; shape = shape->next) {
        shape->bounds.OffsetRect(tx, ty);       // shape->bounds[0] = (shape->bounds[0] + tx) * sx;
        shape->bounds.ScaleRect(sx, sy);        // shape->bounds[1] = (shape->bounds[1] + ty) * sy;
                                                // shape->bounds[2] = (shape->bounds[2] + tx) * sx;
                                                // shape->bounds[3] = (shape->bounds[3] + ty) * sy;
        for (path = shape->paths; path != NULL; path = path->next) {
            path->bounds.OffsetRect(tx, ty);    // path->bounds[0] = (path->bounds[0] + tx) * sx;
            path->bounds.ScaleRect(sx, sy);     // path->bounds[1] = (path->bounds[1] + ty) * sy;
                                                // path->bounds[2] = (path->bounds[2] + tx) * sx;
                                                // path->bounds[3] = (path->bounds[3] + ty) * sy;
            for (i = 0; i < path->npts; i++) {
                path->pts[i].Offset(tx, ty);  // float* pt;
                path->pts[i].Scale(sx, sy);   // pt = &path->pts[i*2];
                                                // pt[0] = (pt[0] + tx) * sx;
                                                // pt[1] = (pt[1] + ty) * sy;
            }
        }
        
        if (shape->fill.type == NSVG_PAINT_LINEAR_GRADIENT || shape->fill.type == NSVG_PAINT_RADIAL_GRADIENT)
        {
            shape->fill.gradient->Scale(tx,ty, sx,sy);
//             memcpy(t, shape->fill.gradient->xform, sizeof(float)*6);
            shape->fill.gradient->xform.Invert();
        }
        if (shape->stroke.type == NSVG_PAINT_LINEAR_GRADIENT || shape->stroke.type == NSVG_PAINT_RADIAL_GRADIENT)
        {
            shape->stroke.gradient->Scale(tx,ty, sx,sy);
//             memcpy(t, shape->stroke.gradient->xform, sizeof(float)*6);
            shape->stroke.gradient->xform.Invert();
        }
        
        shape->strokeWidth *= avgs;
        shape->strokeDashOffset *= avgs;
        for (i = 0; i < shape->strokeDashCount; i++)
            shape->strokeDashArray[i] *= avgs;
    }
}

void NSVGparser::Reset()
{
    nsvg__deletePaths(this->plist);
    nsvg__deleteGradientData(this->gradients);
    nsvgDelete(this->image);
    //free(this->pts);

    Init();

}

void NSVGparser::Parse(float time)
{
    CAutoMsgAnalysis ama("NSVGparser::Parse()");

    Reset();
    m_stamp = time;
    ParseNodes(&m_root);
    CAutoMsgAnalysis ama2("NSVGparser::Parse::ScaleToViewbox()");
    ScaleToViewbox(NSVG_UNITS_PX);
}

void NSVGparser::ParseNodes(TM::CSvgDomNode* node)
{
//     if (!bInUse && node->m_tag == eSvgTag_symbol)
//         return;

    TMBool bPush = ParseNode(node->m_tag, node->m_attribs.data(), node->m_attribs.size());
    if (GetAttr()->IsVisible())
    {
        for (TM::CSvgDomNode::CChildList::recorder rec(node->m_childs); rec; ++rec)
            ParseNodes(rec.get_data());
    }

    if (bPush)
        PopAttr();

    switch (node->m_tag)
    {
    case eSvgTag_defs: this->defsFlag = 0; break;
    }
}

// void NSVGparser::Parse(const char* input, NSVGunits units, float dpi)
// {
//     this->dpi = dpi;
//     
//     //ParseXML(input);
//     LoadXml(input);
//     ParseNodes(&m_root);
//     
//     // Scale to viewBox
//     ScaleToViewbox(units);
// }
// 
// NSVGimage* nsvgParse(char* input, NSVGunits units, float dpi, TM::IColorTransform* pct, TMLParam context)
// {
//     NSVGparser doc;
//     doc.m_pct = pct;
//     doc.m_pctID = context;
//     doc.Parse(input, units, dpi);
//     NSVGimage* ret = doc.image;
//     doc.image = NULL;
//     return ret;
// }


// NSVGpath* nsvgDuplicatePath(NSVGpath* p)
// {
//     NSVGpath* res = NULL;
// 
//     if (p == NULL)
//         return NULL;
// 
//     res = new NSVGpath();
//     if (res == NULL) goto error;
//     memset(res, 0, sizeof(NSVGpath));
// 
//     res->pts = (TM::CPointF*)malloc(p->npts * sizeof(TM::CPointF), CPointF);
//     if (res->pts == NULL) goto error;
//     memcpy(res->pts, p->pts, p->npts * sizeof(TM::CPointF));
//     res->npts = p->npts;
// 
//     memcpy(res->bounds, p->bounds, sizeof(p->bounds));
// 
//     res->closed = p->closed;
// 
//     return res;
// 
// error:
//     if (res != NULL) {
//         free(res->pts);
//         free(res);
//     }
//     return NULL;
// }

void nsvgDelete(NSVGimage* image)
{
    NSVGshape *snext, *shape;
    if (image == NULL) return;
    shape = image->shapes;
    while (shape != NULL) {
        snext = shape->next;
        nsvg__deletePaths(shape->paths);
        nsvg__deletePaint(&shape->fill);
        nsvg__deletePaint(&shape->stroke);
        delete shape;
        shape = snext;
    }
    delete image;
}


struct CAniSourceSVG : public IAniSource
{
public:
    TM_SELF_ALLOC_F(CAniSourceSVG);
#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        return IC_Size(m_doc.m_aNodes) + m_doc.m_root.IC_GetDebugInfo(cls, pbuf);
    }
#endif // TM_DBG

    NSVGparser m_doc;
    TM::CSize m_sz;
    GXDips m_scale;

public:
    static CAniSourceSVG* Make(const char* data, int nSize, TM::IColorTransform* pct, TMLParam context, TM::IImageSizeDecide* isd)
    {
        // load xml
        CAniSourceSVG* p = new CAniSourceSVG();
        p->m_doc.m_pct = pct;
        p->m_doc.m_pctID = context;
        p->m_doc.dpi = TM_DPI_SYSTEM_DEFAULT;
        
        {
            CAutoMsgAnalysis ama("NSVGparser::LoadXml()");
            CStringW strf;
            TxtFileConvert(data, nSize, strf, true);
            p->m_doc.LoadXml(strf);
        }

        // parse
        p->m_doc.Parse(0);

        int ww = p->m_doc.image->width;
        int hh = p->m_doc.image->height;
//         nsvgGetImageSize(p->m_doc.image, ww, hh);
        p->m_sz.SetSize(ww, hh);
        if (isd)
            p->m_scale = isd->OnImageDecideSize(p->m_sz);
        else
            p->m_scale = 1.0;

        return p;
    }
    
    virtual void Release() tm_override { delete this; }
    
    virtual int GetDelay() tm_override { return SVG_FRAME_INTERVALS; }
    
    virtual void GetInfo(int& w, int& h, int& bpp, int& frames) tm_override
    {
        w = m_sz.cx;
        h = m_sz.cy;
        bpp = eGXFormat32A;
        frames = (int)(m_doc.m_dur / SVG_FRAME_INTERVALS);
    }
    
    virtual void GetFrame(int nFrame, TM::CDibHandle32& ld) tm_override
    {
        if (nFrame != 0 || !m_doc.image)
            m_doc.Parse((float)nFrame * SVG_FRAME_INTERVALS);

        CAutoMsgAnalysis ama("NSVGparser::nsvgRasterize()");

        NSVGrasterizer* pRast = nsvgCreateRasterizer();
        DbgAssert(pRast);

        nsvgRasterize(pRast, m_doc.image, 0, 0, m_scale, ld.GetLineData(0), m_sz.Width(), m_sz.Height(), -ld.GetStride());
//         DibPreBlend(ld);
        
        nsvgDeleteRasterizer(pRast);

        nsvgDelete(m_doc.image);
        m_doc.image = 0;
    }
};

IAniSource* svgLoad(EImageFormat fmt, const char* data, int nSize, TM::IColorTransform* pct, TMLParam context, TM::IImageSizeDecide* isd)
{
    return CAniSourceSVG::Make(data, nSize, pct, context, isd);
}



TM_END_NAMESPACE


#endif
