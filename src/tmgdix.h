/*
 *  @file
 *  @brief  graphic X
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.13
 *
 *  Copyright (C) 2011 miragekiller
 */

#pragma once

#ifndef TMGDIX_H
#define TMGDIX_H

#include "tmdib.h"
#include "tmnative.h"
#include "tmcolor.h"
#include "tmstrptr.h"
#include "gximage.h"


// next version
// ...
#define GX_API              TM_CAPI
#define gxGetApp            gxGetApp20221010

struct GXFont;
struct IGXApp;
struct IGXText; // needn't re-create when device changed.

typedef TM::CAffine         GXAffine;
typedef TM::CStrViewW       CGXStrView;
typedef TM::CStrPtrW        CGXStrPtr;

enum EGXTransform
{
    eGXTransformSet,
    eGXTransformIdentity,
    eGXTransformMultiply,
    eGXTransformLeftMultiply,
};

enum EGXDevice
{
    eGXGraDevGDI            = 0x01,
    eGXGraDevD3D9           = 0x02,
    eGXGraDevD3D11          = 0x03,
    eGXGraDevEnd            = 0x04,
    eGXGraDevGL             = 0x04,
    eGXGraDevMask           = 0x0f,

    eGXTxtDevGDI            = 0x10,
    eGXTxtDevDWrite         = 0x20,
    eGXTxtDevMask           = 0xf0,
};
TM_ENUM_2_FLAG(EGXDevice);


enum EGXRet
{
    eGXRetSuccess           = 0,
    eGXRetFail              = 1,
    eGXRetDeviceChanged     = 2, // need re-create all texture.
};

// for IGXApp::Push
enum EGXProp
{
    eGXPropRenderTarget     = 0x01, // change RenderTarget will auto reset clip
    eGXPropAffine           = 0x02,
    eGXPropDPI              = 0x04,
    eGXPropLanguage         = 0x08,
    eGXPropTransform        = eGXPropAffine | eGXPropDPI | eGXPropLanguage,
    eGXPropFont             = 0x10,
    eGXPropTextColor        = 0x20,
    eGXPropClip             = 0x40,

    eGXPropNone             = 0,
    eGXPropAll              = 0xff,
    eGXPropAllNot           = ~eGXPropAll,
};
TM_ENUM_2_FLAG(EGXProp);


// for IGXTexture::ResetAsObject
enum EGXReset
{
    _eGXResetCopy           = 0x00, // not replace texture format.
    _eGXResetMove           = 0x10, // replace texture format, if create success, the source object will auto free.

//     _eGXResetTypeHDC        = 0x01, // HDC
    _eGXResetTypeDib        = 0x01, // GXDibData*
    _eGXResetTypeHIMAGE     = 0x02, // HIMAGE
    _eGXResetTypeIUnknown   = 0x03, // IDirect3DSurface9*
    _eGXResetTypeGLTexture  = 0x04, // GL texture ID

//    eGXResetCopyHDC         = _eGXResetTypeHDC,
    eGXResetCopyDib         = _eGXResetTypeDib,
    eGXResetCopyHIMAGE      = _eGXResetTypeHIMAGE,
    eGXResetCopyIUnknown    = _eGXResetTypeIUnknown,
    eGXResetCopyGLTexture   = _eGXResetTypeGLTexture,

    eGXResetMoveHIMAGE      = _eGXResetMove | _eGXResetTypeHIMAGE,
    eGXResetMoveIUnknown    = _eGXResetMove | _eGXResetTypeIUnknown,
    eGXResetMoveGLTexture   = _eGXResetMove | _eGXResetTypeGLTexture,
};

enum EGrandientFill
{
    e_gfNormal0,
    e_gfNormal45,
    e_gfNormal90,
    e_gfNormal135,

    e_gfSym0,
    e_gfSym45,
    e_gfSym90,
    e_gfSym135,

    e_gfCorner45,
    e_gfCorner135,

    e_gfCenter,
};


enum EGXComposite
{
	eGXCompositeSourceOver,
	eGXCompositeSourceIn,
	eGXCompositeSourceOut,
	eGXCompositeAtop,
	eGXCompositeDestinationOver,
	eGXCompositeDestinationIn,
	eGXCompositeDestinationOut,
	eGXCompositeDestinationAtop,
	eGXCompositeLighter,
	eGXCompositeCopy,
	eGXCompositeXor,
};

enum EGXCapJoin
{
    eGXCapButt,   //  [    ]
	eGXCapSquare, // [      ]
	eGXCapRound,  // (      )
	eGXJoinMiter, // \/
	eGXJoinBevel, // \_/
	eGXJoinRound, // (
// 	eGXJoinLast,  // use last pointer
};

struct CGXStyle
{
    enum { eMaxStop = 128 };
    
	struct ColorStop
	{
		float stop;
		TMColor color;
	};
	
	struct LinearGradient
	{
        int nStop;
        const ColorStop* cs;
		float x0, y0, x1, y1;
	};
	
	struct RadialGradient
	{
        int nStop;
        const ColorStop* cs;
		float x0, y0, x1, y1, r0, r1;
	};

	enum ERepeat
    {
        eRepeatNon                  = 0,
		eRepeatX                    = 0x01,
        eRepeatY                    = 0x02,
        eRepeatXY                   = 0x03,
    };

    enum EType
    {
        eTypeColor,
        eTypeLinearGradient,
        eTypeRadialGradient,
        eTypePattern,
    };

	struct Pattern
	{
		IGXTexture*             image;
		ERepeat                 repeat;
	};

	union
	{
		TMColor                 clr;
		const LinearGradient*   lg;
		const RadialGradient*   rg;
		const Pattern*          pattern;
	};
    EType                       type;

    CGXStyle() {}
    CGXStyle(TMColor c) : clr(c), type(eTypeColor) {}
    CGXStyle(const LinearGradient& p) : lg(&p), type(eTypeLinearGradient) {}
    CGXStyle(const RadialGradient& p) : rg(&p), type(eTypeRadialGradient) {}
    CGXStyle(const Pattern& p) : pattern(&p), type(eTypePattern) {}
    CGXStyle& operator = (TMColor c) { clr = c; type = eTypeColor; return *this; }
    CGXStyle& operator = (const LinearGradient& p) { lg = &p; type = eTypeLinearGradient; return *this; }
    CGXStyle& operator = (const RadialGradient& p) { rg = &p; type = eTypeRadialGradient; return *this; }
    CGXStyle& operator = (const Pattern& p) { pattern = &p; type = eTypePattern; return *this; }
};


struct IGXAppNotify
{
public:
    virtual void OnGXDeviceError(TMResult hr) {}
    virtual void OnGXDeviceChanged(IGXApp* pNew) = 0; // pNew == 0 means termed
};


GX_API void     gxInit();
GX_API void     gxTerm();
GX_API IGXApp*  gxGetApp();
GX_API void     gxSetDefaultGraDevices(TMBool bUpdate, EGXDevice d1, EGXDevice d2 = eGXGraDevGDI, EGXDevice d3 = eGXGraDevGDI, EGXDevice d4 = eGXGraDevGDI);
GX_API void     gxSetDefaultTxtDevices(TMBool bUpdate, EGXDevice d1);
GX_API void     gxTextCreate(IGXText** ppText, TMBool bRich = false);
GX_API void     gxSetNotify(IGXAppNotify* sink, TMBool bAdd);
GX_API void     gxFontGetDefault(GXFont& ft);
GX_API int CALLBACK gxTextWordBreakProc(TMCStr s, int index, int end, int action);


////////////////////////////////////////////////////////////////////////////////
// IGXTexture
// LockRect && GetDC maybe failed if device lost.
struct IGXSurface : public IUnknown
{
public:
    virtual void    LockRect(TMDibBuff& dib, const TMRect* rc, EGXLockRect flag) = 0;
    virtual void    UnlockRect() = 0;
    virtual TMHDC   GetDC(TMBool bDiscard) = 0; // bDiscard: is copy texture to DC. maybe return 0 if device lost.
    virtual void    ReleaseDC(TMHDC hdc) = 0;
};


////////////////////////////////////////////////////////////////////////////////
// IGXTexture
enum EGXPresent
{
    eGXPresentDCPaint   = 0x0000,
    eGXPresentDCClient  = 0x0001,
    eGXPresentLayered   = 0x0002, // layered window
    eGXPresentDCMask    = 0x0003,
};

// layered window: all member valid; other: only rcDirty valid.
struct GXPRESENTFX
{
    TMUInt8             alpha;      
    const TMPointI*     ptDstScreen;    // new window top left
    const TMSizeI*      szDst;          // new window size
    TMPointI            ptSrc;          // x > 0 && y > 0:  drag left to right or drag top to bottom, x < 0 && y < 0:  drag left to left or drag top to top
    const TMRect*       rcDirty;
};


//                  Indirect(FromImage)     RenderTarget    SwapChain
// Blt/Blts         yes                     yes             yes
// Present          no                      no              yes
// SetRenderTarget  no                      yes             yes
// Lock(r)          yes                     yes             yes
// Lock(w)          yes                     yes             yes
// Lock(rw)         yes                     yes             yes
// GetDC(true)      yes                     yes             yes
// GetDC(false)     yes                     yes             yes
struct IGXTexture : public IUnknown
{
public:
    virtual IGXApp* GetApp() = 0;
    virtual void*   GetNativeHandle() = 0; // GDI:HBITMAP; d3d:IUnknown; gl texture:tid; gl rendertarget:HGLRC
    virtual void    GetSurface(IGXSurface** ppSurface) tm_thread_safe = 0;

    virtual TMBool  Resize(int w, int h) = 0;
    virtual TMBool  ResetAsObject(const void* pData, EGXReset eReset) tm_thread_safe = 0;
    virtual void    OptimizeForTile() = 0; // for GX_BLT_TILE
    virtual TMBool  SaveFile(TMCStr pFile, TMBool bHaveAlpha) = 0;

    virtual int     GetWidth() = 0;
    virtual int     GetHeight() = 0;
    virtual EGXFormat   GetFormat() = 0;

    virtual void    Blt(const GXRect& rcDst, const TMRectD& rcSrc, TMUInt32 uFlag, TMUInt8 alpha = 0xff) = 0;
    virtual void    Blts(const GXRect* rcDsts, const TMRectD* rcSrcs, int nCount, TMUInt32 uFlag, TMUInt8 alpha = 0xff) = 0; // rcDsts[nCount] = bound rect. for RTL.

    // only for swap chain. return: is device lost.
    virtual EGXRet  Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx) = 0;

    // inline function
    inline TMBool   ResetAsImage(HIMAGE hImage) tm_thread_safe { return ResetAsObject(hImage, eGXResetMoveHIMAGE); }
    inline TMBool   ResetAsDib(const GXDibData& data) tm_thread_safe { return ResetAsObject(&data, eGXResetCopyDib); }
};


////////////////////////////////////////////////////////////////////////////////
// IGXPath2D
struct IGXPath2D : public IUnknown
{
public:
    virtual void AddPath(IGXPath2D* path) = 0;
    virtual void ClosePath() = 0;
    virtual void MoveTo(float x, float y) = 0;
    virtual void LineTo(float x, float y) = 0;
	virtual void QuadraticCurveTo(float cpx, float cpy, float x, float y) {}
	virtual void BezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y) {}
    virtual void Arc(float cx, float cy, float r, float sAngle, float eAngle, TMBool counterclockwise = false) {}
// 	void arcTo() {}
//     virtual void ellipse() = 0;
//     virtual void rect() = 0;
//     virtual void roundRect() = 0;
};


////////////////////////////////////////////////////////////////////////////////
// IGXCanvas
struct IGXCanvas : public IUnknown
{
public:
	// path2d
	virtual void CreatePath2D(IGXPath2D* pppath) {}

	// composite
	virtual void SetGlobalCompositeOperation(EGXComposite op) {}
	virtual void SetGlobalAlpha(float alpha) {} // 0-1.0f
	virtual EGXComposite GetGlobalCompositeOperation() { return eGXCompositeSourceOver; }
	virtual float GetGlobalAlpha() { return 1; }
	
	// other
	virtual void Save() {}
	virtual void Restore() {}
	
	// style
// 	CGXStyle CreateLinearGradient(float x0, float y0, float x1, float y1) {}
// 	CGXStyle CreateRadialGradient(float x0, float y0, float r0, float x1, float y1, float r1) {}
// 	CGXStyle AddColorStop(CGXStyle& style, float stop, TMColor color) {}
// 	CGXStyle CreatePattern(int image, CGXStyle::EFlags flag) {}

	virtual void SetFillStyle(CGXStyle style) {}
	CGXStyle GetFillStyle() {}
	virtual void SetStrokeStyle(CGXStyle style) {}
	CGXStyle GetStrokeStyle() {}
	
	virtual void SetLineCap(EGXCapJoin cap) {}
	virtual void SetLineJoin(EGXCapJoin join) {}
	virtual void SetLineWidth(float w) {} // not support transform
	virtual void SetMiterLimit(float l) {}
	
	// path
    virtual void Rect(float x, float y, float w, float h) {}
    virtual void FillRect(float x, float y, float w, float h) {}
    virtual void StrokeRect(float x, float y, float w, float h) {}
    void clearRect() {}
	virtual void Fill() {}
	virtual void Stroke() {}
	virtual void BeginPath() {}
	virtual void MoveTo(float x, float y) {}
	virtual void ClosePath() {}
	virtual void LineTo(float x, float y) {}
	void clip() {}
	virtual void QuadraticCurveTo(float cpx, float cpy, float x, float y) {}
	virtual void BezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y) {}
    virtual void Arc(float cx, float cy, float r, float sAngle = 0, float eAngle = CFloat::PI2(), TMBool counterclockwise = false) {}
	void arcTo(float x1, float y1, float x2, float y2, float radius) {}
	void isPointInPath() {}
	
	// transform
	virtual void Scale(float x, float y) {}
	virtual void Rotate(float angle) {} // degrees*Math.PI/180
	virtual void Translate(float dx, float dy) {}
	virtual void Transform(float a, float b, float c, float d, float e, float f) {}
    virtual void SetTransform(float a, float b, float c, float d, float e, float f) {} // 1,0,0,1,0,0

    // text
    virtual void SetFont(const GXFont& font) {}
    virtual void FillText(IGXText* text, float x, float y, float nMaxWidth = -1) {}
 
//     // image
//     drawImage()
//     width	���� ImageData ����Ŀ��ȡ�?
//     height	���� ImageData ����ĸ߶ȡ�?
//     data
//     createImageData()	�����µġ��հ׵� ImageData ����
//     getImageData()	���� ImageData ���󣬸ö���Ϊ������ָ���ľ��θ����������ݡ�
//     putImageData()

    // extern
    void SetTransform(const GXAffine& aff) { SetTransform((const float*)&aff); }
    void SetTransform(const float* aff) { SetTransform(aff[0], aff[1], aff[2], aff[3], aff[4], aff[5]); }
    virtual void RoundRect(float x, float y, float w, float h, float r) {}
    virtual void Ellipse(float cx, float cy, float rx, float ry) {}
};


////////////////////////////////////////////////////////////////////////////////
// IGXApp
struct GXADAPTERDESC
{
    TMCharW     Description [128];
    TMUInt      VendorId;
    TMUInt      DeviceId;
    TMUInt      SubSysId;
    TMUInt      Revision;
    size_t      DedicatedVideoMemory;
    size_t      DedicatedSystemMemory;
    size_t      SharedSystemMemory;
    GUID        AdapterLuid;
    TMUInt64    DriverVersion;
};

const TMUInt32 nVendorID_Microsoft = 0x00001414; // L"Microsoft Basic Render Driver"
const TMUInt32 nVendorID_Intel     = 0x00008086;
const TMUInt32 nVendorID_AMD       = 0x00001002;
const TMUInt32 nVendorID_ATI       = nVendorID_AMD;
const TMUInt32 nVendorID_NVIDIA    = 0x000010DE;
const TMUInt32 nVendorID_VIA       = 0x00001106;
const TMUInt32 nVendorID_S3        = 0x00005333;

struct IGXApp : public IUnknown
{
public:
    virtual EGXDevice GetDeviceType(GXADAPTERDESC* desc = 0) = 0;
    virtual void*     GetNativeHandle() = 0; // GDI:0; d3d:IUnknown

    // clip and push.
    // clip intersect sample:
    // if (gapp->ClipAndPush(m_rcWnd, true, eGXPropClip))
    // {
    //   ... render
    //   gapp->Pop(); // restore old clip
    // }
    // or
    // if (gapp->ClipAndPush(m_rcWnd, true, eGXPropNone))
    // {
    //   ... render
    // }
    virtual TMBool ClipAndPush(const GXRect& rc, TMBool bIntersect = false, EGXProp dwPush = eGXPropNone) = 0; // dwPush : mask of EGXProp
    virtual void GetClip(GXRect& rc) = 0;
    virtual void Push(EGXProp dwFlag) = 0; // mask of EGXProp
    virtual EGXProp Pop() = 0;

    // render target
    virtual EGXRet SetRenderTarget(GXPTexture texture) = 0; // the function will reset clip.
    virtual GXPTexture GetRenderTarget() = 0;

    //  font size = f(dpi);
    //  clip rect = f(dpi, aff.dx, aff.dy, locol);
    //  other transform = f(dpi, aff, local);
    virtual void GetTransform(TM::CAffine* aff, int* dpi = 0, ATLocal* lcId = 0) = 0;
    virtual void SetTransform(const TM::CAffine* aff, const int* dpi = 0, const ATLocal* lcId = 0, EGXTransform eMode = eGXTransformSet) = 0;

    // font and text
    virtual void FontSetCurrent(const GXFont& ft) = 0;
    virtual void FontGetCurrent(GXFont& ft) = 0;
    virtual GXDips FontGetHeight() = 0;
    virtual TMColor SetTextColor(TMColor clr) = 0; // support alpha channel.
    virtual TMColor GetTextColor() = 0;

    // texture, tex:in; *tex must be 0
    virtual void TextureCreateIndirect(IGXTexture** tex, int w, int h, EGXFormat format = eGXFormat32A) = 0;
    virtual void TextureCreateRenderTarget(IGXTexture** tex, int w, int h, EGXFormat format = eGXFormat32A) = 0;
    virtual void TextureCreateSwapChain(IGXTexture** tex, TMHWnd hWnd, int w, int h, EGXFormat format = eGXFormat32A) = 0;
    virtual void TextureCreateFromImage(IGXTexture** tex, HIMAGE img) = 0;

	// canvas
	virtual void CanvasCreate(IGXCanvas** canvas) {}

    // shapes
	virtual EGXRet PresentCanvas(IGXCanvas* canvas, const TM::CAffine* aff) { return eGXRetFail; }
    virtual void Clear(TMColor clr) = 0;
    virtual void FillRect(const GXRect& rc, TMColor clr) = 0;
    virtual void FrameRect(const GXRect& rc, GXDips width, TMColor clr) = 0;
    virtual void InvertRect(const GXRect& rc) = 0;
    virtual void DrawFocusRect(const GXRect& rc) = 0;
    virtual void DrawRectEdge(const GXRect& rc, GXDips wBorderH, GXDips wBorderV, TMColor clr[4]) = 0; // clr:
    virtual void GrandientFill(const GXRect& rc, TMColor clrFrom, TMColor clrTo, EGrandientFill eFill) = 0;

//     virtual void DrawLine(const GXPoint& from, const GXPoint& to, int width, TMColor clr) = 0;
//     virtual void DrawPolyline(const GXPoint* pt, int n, int width, TMColor clr) = 0;
    virtual void DrawIcon(const GXRect& rc, TMHIcon hIcon, TMBool bR2L) = 0;
//     virtual void DrawDC(const GXRect& rcDst, TMHDC hSrc, const TMRectI& rcSrc, TMUInt32 dwRop) = 0; // dwRop: CAPTUREBLT, NOMIRRORBITMAP

	/*
	///////////////////////////////////////////////////////////////////////////
	// 2D
	// path2d
	virtual void CreatePath2D(IGXPath2D* pppath) {}

	// composite
	virtual void SetGlobalCompositeOperation(EGXComposite op) {}
	virtual void SetGlobalAlpha(float alpha) {} // 0-1.0f
	virtual EGXComposite GetGlobalCompositeOperation() { return eGXCompositeSourceOver; }
	virtual float GetGlobalAlpha() { return 1; }
	
	// other
	virtual void Save() {}
	virtual void Restore() {}
	
	// style
	CGXStyle CreateLinearGradient(float x0, float y0, float x1, float y1) {}
	CGXStyle CreateRadialGradient(float x0, float y0, float r0, float x1, float y1, float r1) {}
	CGXStyle AddColorStop(CGXStyle& style, float stop, TMColor color) {}
	CGXStyle CreatePattern(int image, CGXStyle::EFlags flag) {}
	
	void SetFillStyle(CGXStyle style) {}
	CGXStyle GetFillStyle() {}
	void SetStrokeStyle(CGXStyle style) {}
	CGXStyle GetStrokeStyle() {}
	
	virtual void SetLineCap(EGXCapJoin cap) {}
	virtual void SetLineJoin(EGXCapJoin join) {}
	virtual void SetLineWidth(float w) {} // not support transform
	virtual void SetMiterLimit(float l) {}
	
	// path
    virtual void Rect(float x, float y, float w, float h) {}
    virtual void FillRect(float x, float y, float w, float h) {}
    virtual void StrokeRect(float x, float y, float w, float h) {}
    void clearRect() {}
	virtual void Fill() {}
	virtual void Stroke() {}
	virtual void BeginPath() {}
	virtual void MoveTo(float x, float y) {}
	virtual void ClosePath() {}
	virtual void LineTo(float x, float y) {}
	void clip() {}
	virtual void QuadraticCurveTo(float cpx, float cpy, float x, float y) {}
	virtual void BezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y) {}
    virtual void Arc(float cx, float cy, float r, float sAngle, float eAngle, TMBool counterclockwise = false) {}
	void arcTo(float x1, float y1, float x2, float y2, float radius) {}
	void isPointInPath() {}
	
	// transform
	virtual void Scale(float x, float y) {}
	virtual void Rotate(float angle) {} // degrees*Math.PI/180
	virtual void Translate(float dx, float dy) {}
	virtual void Transform(float a, float b, float c, float d, float e, float f) {}
	virtual void SetTransform(float a, float b, float c, float d, float e, float f) {}

    virtual void Flush() {}
	*/

	// text
	// 	font
	// 	textAlign
	// 	textBaseline
	// 	fillText()
	// 	strokeText()
	// 	measureText()
	
	// image
	// drawImage()
	// createImageData()
	// getImageData()
	// putImageData()
};


////////////////////////////////////////////////////////////////////////////////
// rich : "gxRich" function
// rich support HTML tags:
//   a, font, big, small, em, strong, b, u, strike, del, i, /* p, div */, h, img, br, widget
typedef GXPText                        GXPRich;

const TMUInt DT_L2RREADING          = 0x00100000;   // the text always show left to right.
//const TMUInt DT_AUTOFIT             = 0x00200000;   // if text rect greater than draw rect, it auto zoom out.
const TMUInt DT_NOLINKUNDERLINE     = 0x00800000;

// state, read only
const TMUInt DT_RICH                = 0x01000000;   // the text is a rich text
const TMUInt DT_STATE_ELLIPSIS      = 0x02000000;   // the text rect greater than draw rect, need show as ellipsis.

// mask for text && rich
const TMUInt DT_TEXTDRAWMASK        = 0x003FFFFF;
const TMUInt DT_RICHDRAWMASK        = DT_L2RREADING | DT_SINGLELINE | DT_RIGHT | DT_CENTER | DT_RICH | DT_NOLINKUNDERLINE | DT_WORDBREAK | DT_WORD_ELLIPSIS;

#define GX_CARET_WIDTH              2
#define GX_MAX_LINE_WIDTH           SHRT_MAX


struct RICHSEL
{
    TMSel0 nSelBegin;
    TMSel0 nSelEnd;
    TMColor clrSelText;
    TMColor clrSelBackground;
};

struct RICHDRAW
{
    RICHSEL* pSel;
    int nCurrentTab;
    TMBool bDisable;            // EButState
    TM::EColorVisual eVisual;
    TM::IColorTransform* pct;
    TMLParam lct;               // param for pct
    GXDips nWidth;              // only for DT_WORD_ELLIPSIS; default:0;
};

struct IRichInlineObject : public IUnknown
{
public:
    virtual void Destroy() = 0;
    virtual void SetFont(const GXFont& ft) = 0;
    virtual void SetColor(TMColor clr) = 0;
    virtual void GetMetrics(GXSize& sz) = 0;
//     virtual void IsLink() = 0; // is tab stop
    virtual void Render(IGXApp* gapp, GXRect& rc, const RICHDRAW& td) = 0;
};

struct IRichInlinecontainer
{
public:
    virtual TMBool GetAttribute(const CGXStrView& key, CGXStrPtr& val) = 0;
};

struct IRichContext : public IUnknown
{
public:
    virtual void CreateCustomObject(IRichInlinecontainer* pCont, IRichInlineObject** ppObj) = 0;
};

struct IRichLink
{
public:
    virtual void GetStringRef(TMStr pBuffer) = 0;
};


#define RCFLAG_LINEINFO         0x001
#define RCFLAG_LINELEFT         0x002
#define RCFLAG_RUNINFO          0x010
//#define RCFLAG_CHARWIDTH        0x040
//#define RCFLAG_CHARPOS      0x100
#define RCFLAG_ALL              0x0FF
#define RCFLAG_PRI_AFTERWARP    0x100 // only for TMDui

struct RICHCURSOR
{
    TMUInt32 nFlag;

    // RCFLAG_LINEINFO
    int nLineIndex;         // line index
    TMSel0 nLineCharStart;   // the first char index in all string
    int nLineCharLength;
    GXDips nLineWidth;
    GXDips nLineTop;
    GXDips nLineHeight;

    // RCFLAG_LINEOFFSET
    GXDips nLineLeft;

    // RCFLAG_RUNINFO
    int nRunIndex;          // relative to line
    int nRunCharStart;      // start character, relative to line
    int nRunCharLength;
    GXDips nRunLeft;
    GXDips nRunWidth;
    GXDips nRunHeight;
    IRichInlineObject* pObj;
    IRichLink* pLink;

    int nCharOfRun;         // cursor position, relative to run

    // RCFLAG_CHARPOS
    GXPoint* ptLeftTop; // left top of char. in/out; if false will set the volume to 0
};

struct IGXText : public IUnknown
{
public:
    // public function
    virtual void SetString(const CGXStrPtr& str) = 0; // text: str is text; rich: str is html.
    virtual CGXStrPtr GetString(TMSel0 nBgn = 0, int nSize = -1) = 0;
    virtual CGXStrPtr GetHtml(TMSel0 nBgn = 0, int nSize = -1) = 0;
    virtual int GetLength() = 0;
    virtual void SetFormat(TMUInt32 uFormat) = 0;
    virtual TMUInt32 GetFormat() = 0;
    virtual TMBool GetSize(GXSize& sz) = 0; // if dirty return false;
    virtual void CalcSize(GXSize& sz, IGXApp* gapp = gxGetApp()) = 0; //sz: in/out; 
    virtual void Reclaim() = 0; // call when device changed.
    virtual void Paint(const GXRect& rc, RICHDRAW* td = 0, GXRect* rcReal = 0, IGXApp* gapp = gxGetApp()) = 0;

    // only for rich
    virtual void SetMaxLength(int n) { DbgAssert(0); }
    virtual int GetMaxLength() { DbgAssert(0); return -1; }
    virtual void SetPasswordChar(wchar_t chPassword) { DbgAssert(0); }
    virtual wchar_t GetPasswordChar() { DbgAssert(0); return 0; }
    virtual void SetContext(IRichContext* p) { DbgAssert(0); }
    virtual int GetLineCount() { DbgAssert(0); return 0; }
    virtual int GetLineRange(int nLine, TMSel0& nStart) { DbgAssert(0); return 0; } // return: length of line
    virtual void GetInfoFromChar(TMSel0 nIndex, RICHCURSOR& info, TMBool bAfterWarp = true) { DbgAssert(0); }
    virtual TMBool GetInfoFromPos(GXDips x, GXDips y, RICHCURSOR& info) { DbgAssert(0); return false; }
    virtual void ReplaceHtml(TMSel0 nBgn, int nSize, const CGXStrView& str) { DbgAssert(0); } // nSize: can be -1;
    virtual void ReplaceTxt(TMSel0 nBgn, int nSize, const CGXStrView& str) { DbgAssert(0); }  // nSize: can be -1;
    virtual int GetLinks(int n, wchar_t* p) { DbgAssert(0); return 0; } // return: links count
};


///////////////////////////////////////////////////////////////////////////////
// helper function
template <bool t_bManaged>
class CGXAppT : public TM::CRefPtrBaseT<IGXApp, t_bManaged>
{
public:
    typedef TM::CRefPtrBaseT<IGXApp, t_bManaged> base_class;

    CGXAppT(IGXApp* app) : base_class (app) {}
    CGXAppT() : base_class (gxGetApp()) {}

    void SetMapping(GXPoint ptOld, GXPoint ptNew, int dpi)
    {
        DbgAssert(*this);
        TM::CAffine aff;
        aff.Reset();
        aff.MapH(ptOld.X(), ptNew.X(), 1);
        aff.MapV(ptOld.Y(), ptNew.Y(), 1);
        this->p->SetTransform(&aff, &dpi, 0, eGXTransformSet);
    }

    void SetMapping(GXPoint ptOld, GXPoint ptNew, int dpi, ATLocal lc)
    {
        DbgAssert(*this);
        TM::CAffine aff;
        aff.Reset();
        //         float sc = dpi / (float)TM_DPI_SYSTEM_DEFAULT;
        aff.MapH(ptOld.X(), ptNew.X(), 1);
        aff.MapV(ptOld.Y(), ptNew.Y(), 1);
        this->p->SetTransform(&aff, &dpi, &lc, eGXTransformSet);
    }

    void SetMapping(GXPoint ptOrg, int dpi, ATLocal lc)
    {
        this->SetMapping(GXPoint(0,0), ptOrg, dpi, lc);
//         TM::CAffine aff;
//         aff.Reset();
//         //         float sc = dpi / (float)TM_DPI_SYSTEM_DEFAULT;
//         aff.MapH(0, ptOrg.X(), 1);
//         aff.MapV(0, ptOrg.Y(), 1);
//         m_pApp->SetTransform(&aff, &dpi, &lc, eGXTransformSet);
    }

    void SetMapping(int dpi = TM_DPI_SYSTEM_DEFAULT, ATLocal lc = 0)
    {
        DbgAssert(*this);
        TM::CAffine aff;
        aff.Reset();
        this->p->SetTransform(&aff, &dpi, &lc, eGXTransformSet);
    }

    ATLocal GetLocal()
    {
        ATLocal lc;
        this->p->GetTransform(0, 0, &lc);
        return lc;
    }
};

typedef CGXAppT<true>   CGXApp;
typedef CGXAppT<false>  CGXAppHandle;
#define _gxApp          CGXAppHandle()


////////////////////////////////////////////////////////////////////////////////
// font
const int GX_FONT_WEIGHT            = 0x01;
const int GX_FONT_ITALIC            = 0x02;
const int GX_FONT_UNDERLINE         = 0x04;
const int GX_FONT_STRIKE            = 0x08;
const int GX_FONT_FIXED             = 0x10;
const int GX_FONT_MASK_WEIGHT       = GX_FONT_WEIGHT;
const int GX_FONT_MASK_ITALIC       = GX_FONT_ITALIC;
const int GX_FONT_MASK_UNDERLINE    = GX_FONT_UNDERLINE;
const int GX_FONT_MASK_STRIKE       = GX_FONT_STRIKE;
const int GX_FONT_MASK_FIXED        = GX_FONT_FIXED;
const int GX_FONT_MASK_STYLE        = 0x1f;

const int GX_FONT_FACE              = 0x0100;
const int GX_FONT_HEIGHT            = 0x0200;

// set name to "sys" to set system default font
//#define GX_FONT_NAME_SYS                 TM_T("sys")

struct GXFont
{
//    typedef TM::CStrArrayT<TMCharT, 32> CFontString;

    TMCharW name[32];   // set empty to skip name
    union
    {
        struct
        {
            TMInt16 height; // set to 0 to skip height, the height is font line height
            TMUInt8 styleMask;
            TMUInt8 style;  // weight, italic, underline, strike
        };
        TMUInt32    u;
    };

    int  _namecomp(TMCStrW str) const { return StrCompare(name, str, tm_countof(name)); }
    bool _namempty() const { return !name[0]; }

    GXFont()
    {
        Reset();
    }

    void Reset()
    {
        name[0] = 0;
        u = 0;
    }

    void SetName(TMCStrW str, size_t sz = -1)
    {
        StrCopy(name, tm_countof(name), str, sz);
    }

    void Init(const CGXStrView& n, TMInt16 h, TMBool weight = 0, TMBool italic = 0, TMBool underline = 0, TMBool strike = 0)
    {
        SetName(n.data(), n.length());
        height = h;
        styleMask = GX_FONT_MASK_STYLE;
        if (weight)
            style |= GX_FONT_WEIGHT;
        if (italic)
            style |= GX_FONT_ITALIC;
        if (underline)
            style |= GX_FONT_UNDERLINE;
        if (strike)
            style |= GX_FONT_STRIKE;
    }

    TMBool IsInited() const
    {
        return !_namempty() && height && styleMask == GX_FONT_MASK_STYLE;
    }

    void SetStyle(TMBool bSet, TMUInt8 _style)
    {
        if (bSet)
            style |= _style;
        else
            style &= ~_style;
        styleMask |= _style;
    }

    void RemoveStyle(TMUInt8 _style)
    {
        style &= ~_style;
        styleMask &= ~_style;
    }

    void SetUnderline(TMBool bSet) { SetStyle(bSet, GX_FONT_UNDERLINE); }
    void RemoveUnderline() { RemoveStyle(GX_FONT_UNDERLINE); }
    void SetItalic(TMBool bSet) { SetStyle(bSet, GX_FONT_ITALIC); }
    void RemoveItalic() { RemoveStyle(GX_FONT_ITALIC); }
    void SetWeight(TMBool bSet) { SetStyle(bSet, GX_FONT_WEIGHT); }
    void RemoveWeight() { RemoveStyle(GX_FONT_WEIGHT); }
    void SetStrike(TMBool bSet) { SetStyle(bSet, GX_FONT_STRIKE); }
    void RemoveStrike() { RemoveStyle(GX_FONT_STRIKE); }

    void SupplementFrom(const GXFont& rhs)
    {
        if (_namempty())
            SetName(rhs.name);
        if (!height)
            height = rhs.height;
        style = (rhs.style & ~styleMask & rhs.styleMask) | (style & styleMask);
        styleMask |= rhs.styleMask;
    }

    void ReplaceFrom(const GXFont& rhs)
    {
        if (rhs.name)
            SetName(rhs.name);
        if (rhs.height)
            height = rhs.height;
        style = (style & styleMask & ~rhs.styleMask) | (rhs.style & rhs.styleMask);
        styleMask |= rhs.styleMask;
    }

    TMUInt32 ReplaceFromEx(const GXFont& rhs)
    {
        TMUInt32 uModify = (this->style & rhs.styleMask) ^ (rhs.style & rhs.styleMask);

        if (uModify)
        {
            this->style &= ~rhs.styleMask;
            this->style |= (rhs.style & rhs.styleMask);
        }

        if (!rhs._namempty() && _namecomp(rhs.name))
        {
            SetName(rhs.name);
            uModify |= GX_FONT_FACE;
        }

        if (rhs.height != 0 && rhs.height != this->height)
        {
            this->height = rhs.height;
            uModify |= GX_FONT_HEIGHT;
        }

        return uModify;
    }

    bool operator == (const GXFont& rhs) const
    {
        return u == rhs.u && !_namecomp(rhs.name);
    }

    bool operator != (const GXFont& rhs) const
    {
        return u != rhs.u || _namecomp(rhs.name);
    }

#ifdef TM_QT
    inline TMHFont CreateHFont(int nDpi) const
    {
        QFont ft(TM_T2Q(name), -1,
            (style & GX_FONT_WEIGHT) ? 700 : 400,
            style & GX_FONT_ITALIC);
        if (style & GX_FONT_UNDERLINE)
            ft.setUnderline(true);
        if (style & GX_FONT_STRIKE)
            ft.setStrikeOut(true);
        ft.setPixelSize(TM::CZoom(nDpi).LPtoDP(height));
        return ft;
    }

    inline void FromHFont(TMHFont ft)
    {
        Init(TM_Q2W(ft.family()), ft.pointSize(),
            ft.weight() > FW_NORMAL, ft.italic(), ft.underline(), ft.strikeOut());
    }

#elif defined(TM_WIN)
    inline void ToLOGFONT(LOGFONTW& lf) const
    {
        MemZeroT(lf);
        StrCopy(lf.lfFaceName, LF_FACESIZE, name);
        lf.lfWeight = (style & GX_FONT_WEIGHT) ? FW_BOLD : FW_NORMAL;
        lf.lfItalic = style & GX_FONT_ITALIC;
        lf.lfUnderline = style & GX_FONT_UNDERLINE;
        lf.lfStrikeOut = style & GX_FONT_STRIKE;
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfPitchAndFamily = DEFAULT_PITCH | FF_MODERN; // FIXED_PITCH 
//         lf.lfQuality = ANTIALIASED_QUALITY;
        lf.lfHeight = -height;
    }

    inline void FromLOGFONT(const LOGFONTW& lf)
    {
        Init(lf.lfFaceName, (TMInt16)(lf.lfHeight > 0 ? lf.lfHeight : -lf.lfHeight),
            lf.lfWeight > FW_NORMAL, lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut);
    }

    inline HFONT CreateHFont() const
    {
        LOGFONTW lf;
        ToLOGFONT(lf);
        HFONT hFont;
        DbgVerify(hFont = CreateFontIndirectW(&lf));
        return hFont;
    }

    inline HFONT CreateHFont(int nDpi) const
    {
        LOGFONTW lf;
        ToLOGFONT(lf);
        lf.lfHeight = TM::CZoom(nDpi).LPtoDP(lf.lfHeight);
        HFONT hFont;
        DbgVerify(hFont = CreateFontIndirectW(&lf));

        GXFont ftmp;
        ftmp.FromHFont(hFont);

        return hFont;
    }

    inline void FromHFont(TMHFont hFont)
    {
        LOGFONTW lf = {0};
        DbgVerify(GetObject(hFont, sizeof(LOGFONT), &lf));
        Init(lf.lfFaceName, lf.lfHeight,
            lf.lfWeight > FW_NORMAL, lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut);
    }
    
#elif defined(TM_APPLE)
    inline TMHFont CreateHFont() const
    {
        return CGFontCreateWithFontName(TM_T2FS(name));
    }
#endif

    // editor function
    bool CompareForEditor(const GXFont& rhs) const
    {
        return height == rhs.height && style == rhs.style && !_namecomp(rhs.name);
    }

    void FromText(const TM::CStrViewW& xml)
    {
        TM::CStrViewW _name;
        TM::CStrViewW strEnd = xml.ScanByChar(':', _name);
        this->SetName(_name.data(), _name.length());

        {
            TM::CStrViewW strSize;
            strEnd = strEnd.ScanByChar(':', strSize);
            int h = strSize.ToLong(0, 10);
            if (h < 0)
                h = -h;
            this->height = h;
        }
        
        this->styleMask = GX_FONT_MASK_STYLE;
        this->style = 0;
        DbgAssert(strEnd.size() <= 4);
        if (strEnd.find('i') != strEnd.npos)
            this->style |= GX_FONT_ITALIC;
        if (strEnd.find('u') != strEnd.npos)
            this->style |= GX_FONT_UNDERLINE;
        if (strEnd.find('s') != strEnd.npos)
            this->style |= GX_FONT_STRIKE;
        if (strEnd.find('b') != strEnd.npos)
            this->style |= GX_FONT_WEIGHT;
    }

    TM::CString ToText() const
    {
        TM::CString str(this->name);
        
        TMCharT c[32];
        int h =  this->height; //MulDiv (font.height, 72, 96);
        if (h == 0)
            c[0] = 0;
        else
        {
            if (h < 0)
                h = -h;
            StrFromL(h, c);
        }
        
        TMUInt8 _style = this->style & this->styleMask;
        if (style)
        {
            str += TM_T(':');
            str += c;
            
            str += TM_T(':');
            if (_style & GX_FONT_ITALIC)
                str += TM_T('i');
            if (_style & GX_FONT_UNDERLINE)
                str += TM_T('u');
            if (_style & GX_FONT_STRIKE)
                str += TM_T('s');
            if (_style & GX_FONT_WEIGHT)
                str += TM_T('b');
        }
        else if (h)
        {
            str += TM_T(':');
            str += c;
        }
        return std::move(str);
    }
};


////////////////////////////////////////////////////////////////////////////////
// text

template <class T, class V>
inline void _gxTextCalcAlignmentRect(T& rc, V w, V h, TMUInt32 uFormat)
{
    if ((uFormat & (DT_TOP | DT_BOTTOM | DT_VCENTER)) == DT_BOTTOM)
        rc.KeepBSetH(h);
    else if ((uFormat & (DT_TOP | DT_BOTTOM | DT_VCENTER)) == DT_VCENTER)
        rc.SetBottomHeight((rc.Bottom() + rc.Top() + h) / V(2), h);
    else
        rc.KeepTSetH(h);

    if ((uFormat & (DT_LEFT | DT_RIGHT | DT_CENTER)) == DT_RIGHT)
        rc.KeepRSetW(w);
    else if ((uFormat & (DT_LEFT | DT_RIGHT | DT_CENTER)) == DT_CENTER)
        rc.SetRightWidth((rc.Right() + rc.Left() + w) / (V)2, w);
    else
        rc.KeepLSetW(w);
}

inline void gxTextCalcAlignmentRect(GXRect& rc, GXDips w, GXDips h, TMUInt32 uFormat)
{ _gxTextCalcAlignmentRect(rc, w, h, uFormat); }

inline void gxTextCalcAlignmentRect(TM::CRect& rc, int w, int h, TMUInt32 uFormat)
{ _gxTextCalcAlignmentRect(rc, w, h, uFormat); }


inline void gxTextCalcAlignmentRect(TM::CRectF& rc, const GXSize& sz, TMUInt32 uFormat)
{ _gxTextCalcAlignmentRect(rc, sz.Width(), sz.Height(), uFormat); }

inline void gxTextCalcAlignmentRect(TM::CRectI& rc, const TM::CSize& sz, TMUInt32 uFormat)
{ _gxTextCalcAlignmentRect(rc, sz.Width(), sz.Height(), uFormat); }

inline void gxTextCalcAlignmentRectEllipsis(TM::CRectF& rc, GXDips w, GXDips h, TMUInt32 uFormat)
{
    GXDips w1 = rc.Width(), h1 = rc.Height();
    if (w1 < w && (uFormat & (DT_WORDBREAK | DT_WORD_ELLIPSIS | DT_PATH_ELLIPSIS | DT_END_ELLIPSIS)))
        w = w1;
    if (h1 < h && (uFormat & (DT_END_ELLIPSIS)))
        h = h1;
    _gxTextCalcAlignmentRect(rc, w, h, uFormat);
}



// inline int gxTextureGetWidth(GXPTexture x)  { return x ? x->GetWidth() : 0; }
// inline int gxTextureGetHeight(GXPTexture x) { return x ? x->GetHeight() : 0; }



////////////////////////////////////////////////////////////////////////////////
// sharp
GX_API void gxDrawLine(const GXPoint& from, const GXPoint& to, int width, TMColor clr);
GX_API void gxDrawPolyline(const GXPoint* pt, int n, int width, TMColor clr);



// filter
// GX_API void gxFilterBlur(const GXRect& rc, int nRadius);
// copy bk buffer to txtBack, then use txtBack blur to bk buffer.
// GX_API void gxFilterBlur(GXPTexture txtBack, const TM::CRectI& rcx, int nRadius, int nScale);
// GX_API void gxFilterGray(const GXRect& rc, int nMix);
// GX_API void gxFilterSkew(const GXRect& rc, HIMAGE img, TMUInt8 scale, TMBool bRepeat);




TM_BGN_NAMESPACE

template <bool t_bManaged>
class CTextureT : public CRefPtrBaseT<IGXTexture, t_bManaged>
{
public:
    typedef CRefPtrBaseT<IGXTexture, t_bManaged> base_class;
    DECALRE_REFPTR_IMPL(CTextureT, IGXTexture);

    TMBool ReCreateIndirect(int w, int h, EGXFormat format = eGXFormat32A, IGXApp* gapp = gxGetApp())
    {
        if ((*this) && (w != (*this)->GetWidth() || h != (*this)->GetHeight()))
            this->Release();
        if (!*this)
        {
            CreateIndirect(w, h, format, gapp);
            DbgAssert(*this);
            return true;
        }
        return false;
    }

    void CreateSwapChain(TMHWnd hWnd, int w, int h, EGXFormat format = eGXFormat32A, IGXApp* gapp = gxGetApp())
    {
        DbgAssert(!*this);
        gapp->TextureCreateSwapChain(&*this, hWnd, w, h, format);
    }

    void CreateRenderTarget(int w, int h, EGXFormat format = eGXFormat32A, IGXApp* gapp = gxGetApp())
    {
        DbgAssert(!*this);
        gapp->TextureCreateRenderTarget(&*this, w, h, format);
    }

    void CreateIndirect(int w, int h, EGXFormat format = eGXFormat32A, IGXApp* gapp = gxGetApp())
    {
        DbgAssert(!*this);
        gapp->TextureCreateIndirect(&*this, w, h, format);
    }

    void CreateFromImage(HIMAGE img, IGXApp* gapp = gxGetApp())
    {
        DbgAssert(!*this);
        gapp->TextureCreateFromImage(&*this, img);
    }

    void CreateFromFile(TMCStr pFile, IGXApp* gapp = gxGetApp())
    {
        DbgAssert(!*this);
        CImage img;
        img.CreateFromFile(pFile);
        CreateFromImage(img, gapp);
    }

    void CreateFromResource(TMHModule hInst, TMCStr pName, TMCStr pType, IGXApp* gapp = gxGetApp())
    {
        DbgAssert(!*this);
        CImage img;
        img.CreateFromResource(hInst, pName, pType);
        CreateFromImage(img, gapp);
    }

    void CreateFromStream(IStream* stream, IImageSizeDecide* pDecide = 0, IGXApp* gapp = gxGetApp())
    {
        DbgAssert(!*this);
        CImage img;
        img.CreateFromStream(stream, pDecide);
        CreateFromImage(img, gapp);
    }

    void CreateFromStream(IStream* stream, TMSizeI* sz, IGXApp* gapp = gxGetApp())
    {
        DbgAssert(!*this);
        if (sz)
        {
            CImagePSizeDecide sd(sz);
            sd.m_sz = sz;
            CreateFromStream(stream, &sd, gapp);
        }
        else
            CreateFromStream(stream, (IImageSizeDecide*)0, gapp);
    }

    void CreateFromGDIObject(TMHBmp hObj, IGXApp* gapp = gxGetApp())
    {
        DbgAssert(!*this);
        CImage img;
        img.CreateFromGDIObject(hObj);
        CreateFromImage(img, gapp);
    }

    #ifndef TM_GTK
    void CreateFromGDIObject(TMHIcon hObj, IGXApp* gapp = gxGetApp())
    {
        DbgAssert(!*this);
        CImage img;
        img.CreateFromGDIObject(hObj);
        CreateFromImage(img, gapp);
    }
    #endif // TM_GTK

    void Debug_BltToScreen(int w = INT_MAX, int h = INT_MAX)
    {
#if defined(TM_WIN) && defined(TM_DBG)
        HDC hdc = this->p->CreateDC();
        HDC hdcW = GetDC(0);
        ::BitBlt(hdcW, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
        ReleaseDC(0, hdcW);
        this->p->DeleteDC(hdc);
#endif
    }
};

typedef CTextureT<true>      CTexture;
typedef CTextureT<false>     CTextureHandle;



template <bool t_bManaged>
class CTextT : public CRefPtrBaseT<IGXText, t_bManaged>
{
public:
    typedef CRefPtrBaseT<IGXText, t_bManaged> base_class;
    DECALRE_REFPTR_IMPL(CTextT, IGXText);

    CTextT* GetThis() { return this; }

    void Create(TMBool bRich = false)
    {
        DbgAssert(!*this);
        gxTextCreate(&*this, bRich);
    }

    void Create(const CGXStrPtr& str, TMBool bRich = false)
    {
        DbgAssert(!*this);
        gxTextCreate(&*this, bRich);
        (*this)->SetString(str);
    }

    void ReCreate(const CGXStrPtr& str, TMBool bRich)
    {
        if (*this && (!((*this)->GetFormat() & DT_RICH) != !bRich))
            this->Release();
        if (!*this)
            Create(bRich);
        (*this)->SetString(str);
    }

    void GetString(CGXStrPtr& str) const
    {
        if (*this)
            str = (*this)->GetString();
    }

    CGXStrPtr GetString() const
    {
        if (*this)
            return (*this)->GetString();
        else
            return CGXStrPtr();
    }

    void Reclaim()
    {
        if (*this)
            (*this)->Reclaim();
    }
};

typedef CTextT<true>      CText;
typedef CTextT<false>     CTextHandle;


template <class T_Rect>
class CDirtyRectT
{
public:
    T_Rect m_rcDirty;

public:
    CDirtyRectT()
        : m_rcDirty(0,0,0,0)//SHRT_MIN,SHRT_MIN,SHRT_MAX,SHRT_MAX)
    {}

    void Validate()
    {
        m_rcDirty.SetRectEmpty();
    }

    // return: is the new state different as old state.
    TMBool Invalidate(const T_Rect& rc)
    {
        if (rc.IsRectEmpty())
            return false;
        if (m_rcDirty.IsRectEmpty())
        {
            m_rcDirty = rc;
            return true;
        }
        else
        {
            T_Rect rcx(m_rcDirty);
            m_rcDirty |= rc;
            return rcx != m_rcDirty;
        }
    }

    void Invalidate()
    {
        m_rcDirty.SetRect(SHRT_MIN,SHRT_MIN,SHRT_MAX,SHRT_MAX);
    }

    // if dirty all return 0; if validate return empty rect; else return rect;
    // the return value can use to call "::InvalidateRect(HWND)"
    const T_Rect& GetDirty()
    {
        return m_rcDirty;
    }

    TMBool IsDirty() const
    {
        return !m_rcDirty.IsRectEmpty();
    }
};


class CAutoTextureRender
{
private:
    CRefPtr<IGXApp>     m_app;
    CRefPtr<IGXTexture> m_tex;

public:
    CAutoTextureRender(IGXApp* gapp = gxGetApp())
        : m_app(gapp)
    {
    }

    CAutoTextureRender(GXPTexture texture, IGXApp* gapp = gxGetApp())
        : m_app(gapp)
        , m_tex(texture)
    {
        m_app->Push(eGXPropAll);
        m_app->SetRenderTarget(texture);
    }

    CAutoTextureRender(GXPTexture texture, int dpi, ATLocal lc = 0, IGXApp* gapp = gxGetApp())
        : m_app(gapp)
        , m_tex(texture)
    {
        m_app->Push(eGXPropAll);
        m_app->SetRenderTarget(texture);
        CGXApp(m_app).SetMapping(dpi, lc);
    }

    EGXRet SetMapping(GXPTexture texture, const GXPoint& ptOrg, int dpi, ATLocal lc = 0)
    {
        DbgAssert(m_app);
        DbgAssert(!m_tex);
        m_tex = texture;
        m_app->Push(eGXPropAll);
        EGXRet ret = m_app->SetRenderTarget(texture);
        if (eGXRetSuccess == ret)
            CGXApp(m_app).SetMapping(ptOrg, dpi, lc);
        return ret;
    }

    ~CAutoTextureRender()
    {
        m_app->Pop();
    }

    IGXApp* operator -> () const { return m_app; }

    IGXApp* GetApp() { return m_app; }
};


struct CAutoTextureDC : public _cdc_base
{
    TM_NO_COPYABLE(CAutoTextureDC)

public:
    CComPtr<IGXSurface> m_surface;

    CAutoTextureDC(GXPTexture tex, TMBool bDiscard = false)
    {
        tex->GetSurface(&m_surface);
        DbgAssert(m_surface);
        m_hDC = m_surface->GetDC(bDiscard); // maybe lost device.
    }

    ~CAutoTextureDC()
    {
        if (m_hDC)
            m_surface->ReleaseDC(m_hDC);
    }
};

// T: CDibHandle32, CDibHandle24;
template <class T>
struct CAutoTextureLockBitsT : public T
{
    CComPtr<IGXSurface> m_surface;

    CAutoTextureLockBitsT(GXPTexture tex, const TMRect* rc = 0, EGXLockRect flag = GX_LOCK_RECT_READ)
    {
        tex->GetSurface(&m_surface);
        DbgAssert(m_surface);
        m_surface->LockRect(*this, rc, flag);
        this->m_width = tex->GetWidth();
    }

    ~CAutoTextureLockBitsT()
    {
        if (*this)
            m_surface->UnlockRect();
    }
};
typedef CAutoTextureLockBitsT<CDibHandle32> CAutoTextureLockBits32;
typedef CAutoTextureLockBitsT<CDibHandle24> CAutoTextureLockBits24;


TM_END_NAMESPACE



#endif // TMGDIX_H
