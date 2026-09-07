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


/*
 *  @file
 *  @brief  nano svg support
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.11.06
 *
 *  Copyright (C) 2019 miragekiller
 */

#pragma once

#ifndef GXNANOSVG_H
#define GXNANOSVG_H

#include "gxsvgdom.h"


TM_BGN_NAMESPACE

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

enum NSVGpaintType {
//     NSVG_PAINT_UNDEF = -1,
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
    TMColor color;                      // unsigned int color;
    float offset;
} NSVGgradientStop;

typedef struct NSVGgradient {
    TM::CAffine xform;                  // float xform[6];
    NSVGspreadType spread;              // char spread;
    float fx, fy;
    int nstops;
    NSVGgradientStop stops[1];
    void Scale(float tx, float ty, float sx, float sy);
} NSVGgradient;

typedef struct NSVGpaint {
    NSVGpaintType type;                 // signed char type;
    union {
        TMColor color;                  // unsigned int color;
        NSVGgradient* gradient;
        TMWStrRange   gradient_id;
    };
} NSVGpaint;

typedef struct NSVGpath
{
    TM_NO_COPYABLE(NSVGpath);
public:
    TM_SELF_ALLOC(NSVGpath);
    NSVGpath() {}
    TM::CPointF* pts;                   // float* pts;          // Cubic bezier points: x0,y0, [cpx1,cpx1,cpx2,cpy2,x1,y1], ...
    int npts;                                               // Total number of bezier points.
    char closed;                                            // Flag indicating if shapes should be treated as closed.
    TM::CRectF bounds;                  // float bounds[4];     // Tight bounding box of the shape [minx,miny,maxx,maxy].
    struct NSVGpath* next;                                  // Pointer to next path, or NULL if last element.
} NSVGpath;

typedef struct NSVGshape
{
    TM_SELF_ALLOC(NSVGshape);
    TM::CXmlTag id;                     // char id[64];         // Optional 'id' attr of the shape or its group
    NSVGpaint fill;                                             // Fill paint
    NSVGpaint stroke;                                           // Stroke paint
    float opacity;                                              // Opacity of the shape.
    float strokeWidth;                                          // Stroke width (scaled).
    float strokeDashOffset;                                     // Stroke dash offset (scaled).
    float strokeDashArray[8];                                   // Stroke dash array (scaled).
    char strokeDashCount;                                       // Number of dash values in dash array.
    NSVGlineJoin strokeLineJoin : 8;    // char strokeLineJoin; // Stroke join type.
    NSVGlineCap strokeLineCap : 8;      // char strokeLineCap;  // Stroke cap type.
    float miterLimit;                                           // Miter limit
    NSVGfillRule fillRule : 8;          // char fillRule;       // Fill rule, see NSVGfillRule.
    unsigned char flags;                                        // Logical or of NSVG_FLAGS_* flags
    TM::CRectF bounds;                  // float bounds[4];     // Tight bounding box of the shape [minx,miny,maxx,maxy].
//    char fillGradient[64];		                                // Optional 'id' of fill gradient
//    char strokeGradient[64];	                                // Optional 'id' of stroke gradient
//     TM::CAffine xform;                  // float xform[6];		// Root transformation for fill/stroke gradient
    NSVGpath* paths;                                            // Linked list of paths in the image.
    struct NSVGshape* next;                                     // Pointer to next shape, or NULL if last element.
    void GetLocalBounds(float* bounds, TM::CAffine& xform);
} NSVGshape;

typedef struct NSVGimage
{
    TM_SELF_ALLOC(NSVGimage);
    float width;                    // Width of the image.
    float height;                    // Height of the image.
    NSVGshape* shapes;                // Linked list of shapes in the image.
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

#ifndef NANOSVG_CPLUSPLUS
#ifdef __cplusplus
}
#endif
#endif

#ifdef NANOSVG_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define NSVG_PI (3.14159265358979323846264338327f)
#define NSVG_KAPPA90 (0.5522847493f)	// Length proportional to radius of a cubic bezier handle for 90deg arcs.

#define NSVG_ALIGN_MIN 0
#define NSVG_ALIGN_MID 1
#define NSVG_ALIGN_MAX 2
#define NSVG_ALIGN_NONE 0
#define NSVG_ALIGN_MEET 1
#define NSVG_ALIGN_SLICE 2

#define NSVG_NOTUSED(v) do { (void)(1 ? (void)0 : ( (void)(v) ) ); } while(0)
#define NSVG_RGB(r, g, b) (((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16))

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


static int nsvg__isspace(char c)
{
	return strchr(" \t\n\v\f\r", c) != 0;
}

static int nsvg__isdigit(char c)
{
	return c >= '0' && c <= '9';
}

// static NSVG_INLINE float nsvg__minf(float a, float b) { return a < b ? a : b; }
// static NSVG_INLINE float nsvg__maxf(float a, float b) { return a > b ? a : b; }


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

/*int nsvg__parseXML(char* input,
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
}*/


/* Simple SVG parser. */

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
    NSVGunits units;                // int units;
} NSVGcoordinate;

typedef struct NSVGlinearData {
    NSVGcoordinate x1, y1, x2, y2;
} NSVGlinearData;

typedef struct NSVGradialData {
    NSVGcoordinate cx, cy, r, fx, fy;
} NSVGradialData;

typedef struct NSVGgradientData
{
    TM::CXmlTag id;                 // char id[64]; 
    TM::CXmlTag ref;                // char ref[64];
    NSVGpaintType type;             // signed char type;
    union {
        NSVGlinearData linear;
        NSVGradialData radial;
    };
    NSVGspreadType spread : 8;      // char spread;
    NSVGgradientUnits units : 8;    // char units;
    TM::CAffine xform;              // float xform[6];
    int nstops;
    NSVGgradientStop* stops;
    struct NSVGgradientData* next;
    NSVGgradientData(NSVGpaintType eType);
} NSVGgradientData;

struct CSvgFill
{
    enum EFill { eFillNone, eFillColor, eFillGradient } m_type;
    union { TMColor m_Color; TMWStrRange m_gradient; };
    void Init() { m_type = eFillColor; m_Color = TMRGB(0, 0, 0); }
    void Set(TM::CXmlTag val);
};

typedef struct NSVGattrib
{
    TM::CXmlTag id;                     // char id[64];
    TM::CAffine xform;                  // float xform[6];
    CSvgFill m_fill;                    // unsigned int fillColor;
    CSvgFill m_stroke;                  // unsigned int strokeColor;
    float opacity;
    float fillOpacity;
    float strokeOpacity;
//     char fillGradient[64];
//     char strokeGradient[64];
    float strokeWidth;
    float strokeDashOffset;
    float strokeDashArray[NSVG_MAX_DASHES];
    int strokeDashCount;
    NSVGlineJoin strokeLineJoin : 8;    // char strokeLineJoin;
    NSVGlineCap strokeLineCap : 8;      // char strokeLineCap;
    float miterLimit;
    NSVGfillRule fillRule;              // char fillRule;
    float fontSize;
    TMColor stopColor;                  // unsigned int stopColor;
    float stopOpacity;
    float stopOffset;
//     char hasFill;
//     char hasStroke;
    char visible;
    bool IsVisible() const { return visible && (opacity > (0.5f / 255.0f)); }
} NSVGattrib;

struct CURRENTPOINT;
struct NSVGparser : public TM::CSvgDomDoc
{
    TM_SELF_ALLOC(NSVGparser);
    friend CSvgFill;
    friend struct CSvgAni;

    NSVGattrib attr[NSVG_MAX_ATTR];
    int attrHead;
    TM::CPointF* pts;                   // float* pts;
    int npts;
    int cpts;
    NSVGpath* plist;
    NSVGimage* image;
    NSVGgradientData* gradients;
    NSVGshape* shapesTail;
    float viewMinx, viewMiny, viewWidth, viewHeight;
    int alignX, alignY, alignType;
    float dpi;
//     char pathFlag;                   // never used
    char defsFlag;
    
    // color transform
    TM::IColorTransform* m_pct;
    TMLParam m_pctID;

    // animate
    float m_stamp; // ms

private:
    static TM::CXmlTag ParseAttrUrl(TM::CXmlTag val);
    static NSVGunits ParseUnits(TM::CXmlTag val);
    static NSVGcoordinate ParseCoordinateRaw(TM::CXmlTag val);
    static NSVGlineCap ParseAttrLineCap(TM::CXmlTag val);
    static NSVGlineJoin ParseAttrLineJoin(TM::CXmlTag val);
    static NSVGfillRule ParseAttrFillRule(TM::CXmlTag val);

private:
    // parse
    void ParseNodes(TM::CSvgDomNode* node);
    TMBool ParseNode(ESvgTag tag, const TM::CSvgDomAttrib* attrs, int nAttr); // return: bPopAttr
    void ParseNode_gradient(const TM::CSvgDomAttrib* attrs, int nAttr, NSVGpaintType type);
    void ParseNode_gradientStop(const TM::CSvgDomAttrib* attrs, int nAttr);
    void ParseNode_use(const TM::CSvgDomAttrib* attrs, int nAttr);
    void ParseNode_svg(const TM::CSvgDomAttrib* attrs, int nAttr);
    void ParseNode_path(const TM::CSvgDomAttrib* attrs, int nAttr);
    void ParseNode_rect(const TM::CSvgDomAttrib* attrs, int nAttr);
    void ParseNode_circle(const TM::CSvgDomAttrib* attrs, int nAttr);
    void ParseNode_ellipse(const TM::CSvgDomAttrib* attrs, int nAttr);
    void ParseNode_line(const TM::CSvgDomAttrib* attrs, int nAttr);
    void ParseNode_poly(const TM::CSvgDomAttrib* attrs, int nAttr, bool closeFlag);

    void ParseAttrs(const TM::CSvgDomAttrib* attrs, int nAttr);
    bool ParseAttr(const TM::CSvgDomAttrib& attr); // return: is stop parse

    int ArrayParse(TM::CXmlTag val, ESvgUsage usage, CSvgNum* array, int nArray); // 1 2 4 or 1, 2, 3
    int ArrayParsePath(TM::CXmlTag val, CSvgNum* array, int nArray); // M 10,10 h 10m0,10 h 10
    int ArrayParseTransform(TM::CXmlTag val, CSvgNum* array);
//     void ArrayMakePX(CSvgNum* array, int nNum, float orig, float length);
    TMBool ArrayMakeAniValue(CSvgAniItem& ani, const CSvgNum* array, const CSvgNum* array2, CSvgNum* out, int nArray, float tmRate); // tmRate: 0 - 1;

    NSVGcoordinate ParseAniAttrCoordinate(const TM::CSvgDomAttrib& attr);
    float ParseAniAttrCoordinatePX(const TM::CSvgDomAttrib& attr, float orig, float length);
    void ParseAniAttrTransform(const TM::CSvgDomAttrib& attr, TM::CAffine& aff);
    int ParseAniAttrArray(const TM::CSvgDomAttrib& attr, CSvgNum* array, int nArray);
    int ParseAniAttrPath(const TM::CSvgDomAttrib& attr, CSvgNum* array, int nArray);
    int ParseAniAttrStrokeDashArray(const TM::CSvgDomAttrib& attr, float* strokeDashArray);
    TMColor ParseAniAttrColor(const TM::CSvgDomAttrib& attr);
    void ParseAniAttrFill(const TM::CSvgDomAttrib& attr, CSvgFill& fill);
    float ParseAniAttrfloat(const TM::CSvgDomAttrib& attr);

    // coordinate
    float GetActualOrigX() const { return viewMinx; }
    float GetActualOrigY() const { return viewMiny; }
    float GetActualWidth() const { return viewWidth; }
    float GetActualHeight() const { return viewHeight; }
    float GetActualLength() const { return sqrtf(viewWidth * viewWidth + viewHeight * viewHeight) / sqrtf(2.0f); }
public:
    float ConvertToPixels(NSVGcoordinate c, float orig, float length);
private:
    // path
    void ResetPath() { this->npts = 0; }
    void AddPoint(float x, float y) { AddPoint(TM::CPointF(x, y)); }
    void AddPoint(TM::CPointF pt);
    void MoveTo(float x, float y) { MoveTo(TM::CPointF(x, y)); }
    void MoveTo(TM::CPointF pt);
    void LineTo(float x, float y);
    void LineTo(TM::CPointF pt) { LineTo(pt.X(), pt.Y()); }
    void CubicBezTo(float cpx1, float cpy1, float cpx2, float cpy2, float x, float y);

    void PathCubicBezTo(CURRENTPOINT& cp, CSvgNum* array, bool rel);
    void PathCubicBezShortTo(CURRENTPOINT& cp, CSvgNum* array, bool rel);
    void PathQuadBezTo(CURRENTPOINT& cp, CSvgNum* array, int rel);
    void PathQuadBezShortTo(CURRENTPOINT& cp, CSvgNum* array, int rel);
    void PathArcTo(CURRENTPOINT& cp, CSvgNum* array, int rel);

    // other
    void AddPath(char closed);



    void ScaleToViewbox(NSVGunits units);

public:
    NSVGattrib* GetAttr();
    void PushAttr();
    void PopAttr();
    void Parse(float time); // 0 ~ 1

public:
    NSVGparser();
    ~NSVGparser();
    void Reset();
    void Init();
//     void Parse(const char* input, NSVGunits units, float dpi);
};


#ifdef __cplusplus
extern "C" {
#endif

// NSVGimage* nsvgParse(char* input, NSVGunits units, float dpi, TM::IColorTransform* pct, TMLParam context);
struct NSVGrasterizer* nsvgCreateRasterizer();
// void nsvgDelete(NSVGimage* image);
void nsvgRasterize(NSVGrasterizer* r,
    NSVGimage* image, float tx, float ty, float scale,
    unsigned char* dst, int w, int h, int stride);
void nsvgDeleteRasterizer(NSVGrasterizer* r);
// void nsvgGetImageSize(NSVGimage* image, int& width, int& height);

#ifdef __cplusplus
}
#endif





inline CPointF BezierPoint(float t, CPointF p0, CPointF p1, CPointF p2, CPointF p3)
{
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    
    p0 *= uuu;// uuu * p0;
    p0 += p1 * (3 * uu * t);
    p0 += p2 * (3 * u * tt);
    p0 += p3 * ttt;
    
    return p0;
}

// inline float BezierLength(CPointF p0, CPointF p1, CPointF p2, CPointF p3, int pointCount = 30)
// {
//     DbgAssert(pointCount >= 2);
//     
//     float length = 0.0f;
//     CPointF lastPoint = BezierPoint(0.0f, p0, p1, p2, p3);
//     for (int i = 1; i <= pointCount; i++)
//     {
//         CPointF point = BezierPoint((float)i/(float)pointCount, p0, p1, p2, p3);
//         length += point.Distance(lastPoint);
//         lastPoint = point;
//     }
//     return length;
// }
// 
// inline CPointF BezierTangent(float t, CPointF p0, CPointF p1, CPointF p2, CPointF p3)
// {
//     float u = 1 - t;
//     float uu = u * u;
//     float tu = t * u;
//     float tt = t * t;
//     
//     p0 *= (3 * uu * (-1.0f));
//     p0 += p1 * (3 * (uu - 2 * tu));
//     p0 += p2 * (3 * (2 * tu - tt));
//     p0 += p3 * (3 * tt);
//     
//     return p0;
//     
//     //返回单位向量
//     //    return P.normalized;
// }


#endif // NANOSVG_IMPLEMENTATION


TM_END_NAMESPACE

#endif // GXNANOSVG_H
