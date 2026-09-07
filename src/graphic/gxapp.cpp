/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.9.22
 *
 *  Copyright (C) 2018 miragekiller
 */

#include "gxapp.h"
#include "../tmdebugext.h"

#ifdef TM_QT

#   include "gxapp_qt.h"
#   include "gxfont_qt.h"
//#   include "gximage_qt.cpp"
#   include "gxapp_qt.cpp"
#   include "gxtext.cpp"
#   include "gxrich_qt.cpp"

#elif defined(TM_WIN)

#   include "gxfont_win.h"
#   include "gxfont_dwrite.h"
#   include "gxapp_win.cpp"
#   include "gxtext.cpp"
#   include "gxrich.cpp"
#   include "gxapp_d3d9.cpp"
#   include "gxapp_d3d11.cpp"

//#   ifdef TUI_EDIT_MODE
#       include "gxapp_gl.cpp"
//#   endif

#elif defined(TM_APPLE)

#   include "gxfont_mac.h"
//#   include "gximage_mac.cpp"
#   include "gxapp_mac.cpp"
#   include "gxtext.cpp"
#   include "gxrich.cpp"

#elif defined(TM_GTK)

#   include "gxfont_gtk.h"
#   include "gxapp_gtk.cpp"

#else

#   error not support

#endif




TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// IGXTextureImpl
TMBool IGXTextureImpl::ResetAsDib(const GXDibData& data)
{
    TMDibBuff dSelf(0,0,0);
    CComPtr<IGXSurface> surface;
    GetSurface(&surface);
    DbgAssert(surface);
    surface->LockRect(dSelf, 0, data.bits ? GX_LOCK_RECT_WRITE : GX_LOCK_RECT_READWRITE);
    if (dSelf)
    {
        DibResetAsDib(dSelf, GetFormat(), data);
        surface->UnlockRect();
        return true;
    }
    return false;
}

TMBool IGXTextureImpl::ReplaceByImage(HIMAGE hImage)
{
    GX_TRACE("IGXTextureImpl::ReplaceByImage(%p)", hImage);
    CDibHandle32 s32;
    ImageLockRect(hImage, s32, 0, GX_LOCK_RECT_READ);
    if (!s32)
        return false;
    GXDibData dd;
    dd.bits = s32.m_bits;
    dd.wb = s32.m_stride;
    dd.flag = GXDibData::eBottomUp;
    return ResetAsDib(dd);
}

TMColor IGXTextureImpl::Check_GX_BLT_TEXTMONO(TMUInt32 uFlag, TMUInt8 alpha, CGXAppBase* app)
{
    if (uFlag & GX_BLT_TEXTMONO)
    {
        DibARGB clr;
        clr.SetColor(app->GetTextColor());
        clr.PreMultiplied(alpha);
        return clr.ToColor();
    }
    return TMRGBA(alpha, alpha, alpha, alpha);
}

void IGXTextureImpl::NativeColorFill(TMColor clr)
{
    CRect rc;
    rc.SetLTWH(0, 0, GetWidth(), GetHeight());
    CAutoTextureDC dcTex(this);
    dcTex.ClearRect(rc, clr);
}

TMBool IGXTextureImpl::SaveFile(TMCStr pFile, TMBool bHaveAlpha)
{
    CImage img;
    int w = GetWidth();
    int h = GetHeight();
    img.CreateIndirect(GetWidth(), GetHeight(), eGXFormat32A);
    {
        CImageDC dcImg(img);
        CAutoTextureDC dcTex(this);
        dcImg.BitBlt(0, 0, w, h, dcTex, 0, 0, SRCCOPY);
    }
    return ImageSaveFile(img, pFile, bHaveAlpha);
}


////////////////////////////////////////////////////////////////////////////////
// CTextureImage
CTextureImage::CTextureImage()
    : m_clrText(cColorInvalid)
{
    GetApp()->AddRef();
}

CTextureImage::~CTextureImage()
{
    GetApp()->Release();
}

GXPTexture CTextureImage::CreateInst(HIMAGE img)
{
    if (!img)
        return 0;
    CTextureImage* p = new CRefObjectT<CTextureImage>(1);
    p->m_img.Attach(img);
    return p;
}

void CTextureImage::Check_GX_BLT_TEXTMONO(TMUInt32 uFlag, TMUInt8& alpha, CGXAppBase* app)
{
    if (uFlag & GX_BLT_TEXTMONO)
    {
        TMColor clr = app->GetTextColor();
        if (clr != m_clrText)
        {
            ImageConvertToText(m_img, clr);
            m_clrText = clr;
        }
        alpha = ColorA(clr);
    }
}

void CTextureImage::Check_GX_BLT_TEXTMONO(TMUInt32 uFlag, TMColor clr)
{
    if (uFlag & GX_BLT_TEXTMONO)
    {
        if (clr != m_clrText)
        {
            ImageConvertToText(m_img, clr);
            m_clrText = clr;
        }
    }
}


// IGXTexture
IGXApp* CTextureImage::GetApp()
{
    return CGXAppPlateform::Instance();
}

void CTextureImage::GetSurface(IGXSurface** ppSurface)
{
    DbgAssert(m_img);
    DbgAssert(!*ppSurface);
    IGXSurface* p = this;
    p->AddRef();
    *ppSurface = p;
}

void CTextureImage::OptimizeForTile()
{
    if (!m_img)
        return;
    int w = m_img.GetWidth();
    int h = m_img.GetHeight();
    if (w > 255 && h > 255)
        return;
    int ww = w * (1 + (int)(255 / w));
    int hh = h * (1 + (int)(255 / h));
    CImage img;
    img.CreateIndirect(ww, hh, eGXFormat32A);
    img.Swap(m_img);
    CImageDC dc1(m_img);
    for (int i = 0; i < ww; i += w)
    {
        for (int j = 0; j < hh; j += h)
            img.BitBlt(dc1, i, j, 0, 0, w, h, GX_BLT_SRCCOPY);
    }
}

TMBool CTextureImage::Resize(int w, int h)
{
    if (w == ImageGetWidth(m_img) && h == ImageGetHeight(m_img))
        return true;
    EGXFormat fmt = m_img.GetFormat();
    m_img.Release();
    m_img.CreateIndirect(w, h, fmt);
    DBG_INIT(m_img);
    if (!m_img)
        return false;
    return true;
}

TMBool CTextureImage::ResetAsObject(const void* pData, EGXReset eReset)
{
    DbgAssert(pData);
    CAutoMsgAnalysis awm("CTextureImage::ResetAsObject");

    switch (eReset)
    {
    case eGXResetMoveHIMAGE:
        if ((HIMAGE)pData != m_img)
            m_img = (HIMAGE)pData;
        return true;

    case eGXResetCopyDib:
        return ResetAsDib(*(GXDibData*)pData);

    case eGXResetCopyHIMAGE:
        return CImageDC(m_img).BitBlt(0, 0, GetWidth(), GetHeight(), CImageDC((HIMAGE)pData), 0, 0, SRCCOPY);
    }
    return false;
}

TMBool CTextureImage::SaveFile(TMCStr pFile, TMBool bHaveAlpha)
{
    return ImageSaveFile(m_img, pFile, bHaveAlpha);
}

int CTextureImage::GetWidth()
{
    DbgAssert(m_img);
    return ImageGetWidth(m_img);
}

int CTextureImage::GetHeight()
{
    DbgAssert(m_img);
    return ImageGetHeight(m_img);
}

EGXFormat CTextureImage::GetFormat()
{
    DbgAssert(m_img);
    return ImageGetFormat(m_img);
}

void CTextureImage::BltToDC(TMHDC hdc, const TMRectD& rcDst, const TMRectD& rcSrc, TMUInt32 uFlag, TMUInt8 alpha)
{
    DbgAssert(m_img);
    ImageStretchBlt(hdc, rcDst, m_img, rcSrc, uFlag, alpha);
}

// IGXSurface
void CTextureImage::LockRect(TMDibBuff& dib, const TMRect* rc, EGXLockRect flag)
{
    DbgAssert(m_img);
    ImageLockRect(m_img, dib, rc, flag);
}

TMHDC CTextureImage::GetDC(TMBool bDiscard)
{
    DbgAssert(m_img);
    return ImageCreateDC(m_img);
}

void CTextureImage::ReleaseDC(TMHDC hdc)
{
    DbgAssert(m_img);
    ImageDeleteDC(m_img, hdc);
}


///////////////////////////////////////////////////////////////////////////////
// CGXAppBase
CGXAppBase::CGXAppBase()
    : m_clrText(TMRGB(255,255,255))
{
    ClipDevInit();
}

CGXAppBase::~CGXAppBase()
{
}

void CGXAppBase::Push(EGXProp dwFlag)
{
    DbgAssert(0 == (dwFlag & eGXPropAllNot));
    TMUInt32 dwOldSize = m_stk.size();

    if (dwFlag & eGXPropFont)
        PushFont();
    
    if (dwFlag & eGXPropClip)
        m_stk.push(m_rcClip);
    
    if (dwFlag & eGXPropTextColor)
        m_stk.push(m_clrText);
    
    if (dwFlag & eGXPropTransform)
        m_stk.push(m_aff);

    if (dwFlag & eGXPropDPI)
        m_stk.push(GetDPI());
    
    if (dwFlag & eGXPropLanguage)
        m_stk.push(m_local);
    
    if (dwFlag & eGXPropRenderTarget)
        m_stk.push(GetRenderTarget());
    
    m_stk.push(dwFlag);

    GX_TRACE("CGXAppBase::Push(%x) end; size:%d->%d", dwFlag, dwOldSize, m_stk.size());
}

EGXProp CGXAppBase::Pop()
{
    TMUInt32 dwOldSize = m_stk.size();
    TMUInt32 dwFlag;
    m_stk.pop(dwFlag);
    DbgAssert(0 == (dwFlag & eGXPropAllNot));
    
    GXPTexture texture = 0;
    if (dwFlag & eGXPropRenderTarget)
        m_stk.pop(texture);
    
    if (dwFlag & eGXPropLanguage)
        m_stk.pop(m_local);

    CZoom dpi(GetDPI());
    if (dwFlag & eGXPropDPI)
        m_stk.pop(dpi);
    
    if (dwFlag & eGXPropTransform)
        m_stk.pop(m_aff);
    
    if (dwFlag & eGXPropTextColor)
        m_stk.pop(m_clrText);
    
    if (dwFlag & eGXPropClip)
    {
        m_stk.pop(m_rcClip);
        GX_TRACE("CGXAppBase::PopClip(%d,%d,%d,%d)", m_rcClip);
    }
    
    if (dwFlag & eGXPropFont)
        PopFont();

    InitProp(texture, dpi, dwFlag);

    GX_TRACE("CGXAppBase::Pop(%x) end; size:%d->%d", dwFlag, dwOldSize, m_stk.size());
    return (EGXProp)dwFlag;
}

void CGXAppBase::GetTransform(TM::CAffine* aff, int* nDPI, ATLocal* lcId)
{
    if (aff)
        *aff = m_aff;
    if (nDPI)
        *nDPI = GetDPI();
    if (lcId)
        *lcId = m_local;
}

void CGXAppBase::SetTransform(const TM::CAffine* aff, const int* nDPI, const ATLocal* lcId, EGXTransform eMode)
{
    TMUInt32 flag = 0;
    if (aff)
    {
        flag = eGXPropTransform;
        switch (eMode)
        {
        case eGXTransformSet: m_aff = *aff; break;
        case eGXTransformIdentity: m_aff.Reset(); break;
        case eGXTransformMultiply: m_aff *= *aff; break;
        case eGXTransformLeftMultiply: m_aff.PreCombine(*aff); break;
        }
    }

    if (lcId && m_local != *lcId)
        static_cast<void>(flag |= eGXPropLanguage), m_local = *lcId;
    
    int dpi = GetDPI();
    if (nDPI && dpi != *nDPI)
        static_cast<void>(flag |= eGXPropDPI), dpi = *nDPI;

    InitProp(0, dpi, flag);
}

TMBool CGXAppBase::ClipAndPush(const GXRect& rc, TMBool bIntersect, EGXProp dwPush)
{
    _gx_check();
    GXRect rcx(rc);
    rcx.OffsetRect(m_aff.Dx(), m_aff.Dy());
    return DoClip(rcx, bIntersect, dwPush);
}

void CGXAppBase::GetClip(GXRect& rc)
{
    _gx_check();
    DoGetClip(rc);
    rc.OffsetRect(-m_aff.Dx(), -m_aff.Dy());
}

void CGXAppBase::SelectFont(const GXFont& fontkey, int nDPI) // device font
{
    TM::CRefPtr<CGXFont> ptr(m_font->CloneFont(fontkey, nDPI));
    if (m_font != ptr)
        m_font = ptr;
}

CZoom CGXAppBase::GetDPI()
{
    DbgAssert(m_font);
    return m_font->m_dpi;
}

void CGXAppBase::FontSetCurrent(const GXFont& ft)
{
    _gx_check();
    
    GXFont fontkey = m_font->GetKey();
    TMBool bReleaseFont = fontkey.ReplaceFromEx(ft);
    if (bReleaseFont)
        SelectFont(fontkey, GetDPI());
}

void CGXAppBase::FontGetCurrent(GXFont& ft)
{
    _gx_check();
    ft = m_font->GetKey();
}

GXDips CGXAppBase::FontGetHeight()
{
    _gx_check();
    return GetDPI().DPtoLP(m_font->FontGetMetrics().tmHeight);
}

void CGXAppBase::PushFont()
{
    CGXFont* p = m_font;
    m_stk.push(p);
    if (p)
        p->AddRef();
}

TMBool CGXAppBase::PopFont()
{
    CGXFont* p = 0;
    m_stk.pop(p);
    DbgAssert(p);
    TMBool b = (p != m_font);
    if (b)
        m_font = p;
    p->Release();
    return b;
}

TMBool CGXAppBase::DoClip(const GXRect& rc, TMBool bIntersect, EGXProp dwPush)
{
    _gx_check();
    
    CRect rcx;
    GetDPI().LPtoDP(rc, rcx);
    if (bIntersect)
    {
        rcx.IntersectRect(rcx, m_rcClip);
        if (rcx.IsRectEmpty())
            return false;
    }
    
    if (dwPush)
        Push(dwPush);
    
    m_rcClip = rcx;
    GX_TRACE("CGXAppBase::DoClip(rc:%g,%g,%g,%g, bIntersect:%d, bAutoPush:%d), realclip:%d,%d,%d,%d",
        rc.Left(), rc.Top(), rc.Right(), rc.Bottom(), bIntersect, dwPush, m_rcClip);
    
    InitProp(0, 0, eGXPropClip);
    
    return true;
}

TMBool CGXAppBase::ResetFontDevice()
{
    void* old = m_font;
    m_font = CGXFont::FontCreateDefaultAuto(TM::CGXDevMgr::Instance()->m_dftTxtDev);
    return old != m_font;
}

TMColor CGXAppBase::SetTextColor(TMColor clr)
{
    _gx_check();
    TMColor clrTemp = m_clrText;
    m_clrText = clr;
    return clrTemp;
}

void CGXAppBase::TextureCreateIndirect(IGXTexture** tex, int w, int h, EGXFormat format)
{
    DbgAssert(tex && !*tex);
    CImage img;
    img.CreateIndirect(w, h, format, 0, 0);
    DBG_INIT(img);
    TextureCreateFromImage(tex, img);
}

void CGXAppBase::TextureCreateRenderTarget(IGXTexture** tex, int w, int h, EGXFormat bpp)
{
    DbgAssert(tex && !*tex);
    TextureCreateIndirect(tex, w, h, bpp);
}

void CGXAppBase::TextureCreateSwapChain(IGXTexture** tex, TMHWnd hWnd, int w, int h, EGXFormat bpp)
{
    DbgAssert(tex && !*tex);
    TextureCreateIndirect(tex, w, h, bpp);
}

void CGXAppBase::TextureCreateFromImage(IGXTexture** tex, HIMAGE img)
{
    DbgAssert(tex && !*tex);
    int w = ImageGetWidth(img);
    int h = ImageGetHeight(img);
    if (!w || !h)
        return;
    EGXFormat format = ImageGetFormat(img);
    IGXTexture* pt = 0;
    TextureCreateIndirect(&pt, w, h, format);
    if (pt)
    {
        if (pt->ResetAsImage(img))
            *tex = pt;
        else
            pt->Release();
    }
}

void CGXAppBase::CanvasCreate(IGXCanvas** canvas)
{
#ifdef GXCANVAS_H
	*canvas = new CGXCanvas();
#endif // GXCANVAS_H
}

void CGXAppBase::DrawRectEdge(const GXRect& rc, GXDips nh, GXDips nv, TMColor clr[4])
{
    TMColor clra = clr[0];
    TMColor clrb = clr[1];
    TMColor clrc = clr[2];
    TMColor clrd = clr[3];
	GXDips l = rc.Left(), r = rc.Right(), t = rc.Top(), b = rc.Bottom();

    //        0a------------1a,2d
    //        |               |
    //        |---6b---7b,8c--|
    //        |    |     |    |
    //        |--9b,10c-12c---|
    //        |               |
    //        3a,4d-----------5d
    VertexBegin();
    VertexAddColor(l, t, clra);             // 0
    VertexAddColor(r, t, clra);             // 1
    VertexAddColor(r, t, clrd);             // 2
    VertexAddColor(l, b, clra);             // 3
    VertexAddColor(l, b, clrd);             // 4
    VertexAddColor(r, b, clrd);				// 5
    VertexAddColor(l + nh, t + nv, clrb);   // 6
    VertexAddColor(r - nh, t + nv, clrb);   // 7
    VertexAddColor(r - nh, t + nv, clrc);   // 8
    VertexAddColor(l + nh, b - nv, clrb);   // 9
    VertexAddColor(l + nh, b - nv, clrc);   // 10
    VertexAddColor(r - nh, b - nv, clrc);   // 11
    static const GXIndices ind [] = 
    {
        3, 0, 9,   9, 0, 6,   6, 0, 1,    6, 1, 7,
        8, 2, 5,   5, 11, 8,  4, 10, 11,  5, 4, 11
    };
    VertexFill(ind, 24);
}

void CGXAppBase::GrandientFill(const GXRect& rc, TMColor clrFrom, TMColor clrTo, EGrandientFill eFill)
{
#define x0 rc.Left()
#define y0 rc.Top()
#define x1 rc.Right()
#define y1 rc.Bottom()

    TMColor cf = clrFrom;
    TMColor ct = clrTo;
    TMUInt8 r0 = ColorR(cf);
    TMUInt8 g0 = ColorG(cf);
    TMUInt8 b0 = ColorB(cf);
    TMUInt8 a0 = ColorA(cf);
    TMUInt8 r1 = ColorR(ct);
    TMUInt8 g1 = ColorG(ct);
    TMUInt8 b1 = ColorB(ct);
    TMUInt8 a1 = ColorA(ct);

    VertexBegin();
    switch (eFill)
    {
    case e_gfNormal0:
        {
            VertexAddColor(x0, y0, clrFrom);    //  0      0f--------------1t
            VertexAddColor(x1, y0, clrTo);      //  1      |               |
            VertexAddColor(x0, y1, clrFrom);    //  2      |               |
            VertexAddColor(x1, y1, clrTo);      //  3      2f--------------3t
            static const GXIndices Index[] = { 0, 2, 1, 2, 3, 1 };
            VertexFill(Index, 2 * 3);
        }
        break;

    case e_gfNormal45:
        {
            TMUInt8 r = (r0 + r1) / 2;
            TMUInt8 g = (g0 + g1) / 2;
            TMUInt8 b = (b0 + b1) / 2;
            TMUInt8 a = (a0 + a1) / 2;
            TMColor clr = TMRGBA(r,g,b,a);
            VertexAddColor(x0, y0, clrFrom);
            VertexAddColor(x1, y1, clrTo);
            VertexAddColor(x0, y1, clr);
            VertexAddColor(x1, y0, clr);
            static const GXIndices Index[] = { 0, 1, 2, 0, 3, 1};
            VertexFill(Index, 2 * 3);
        }
        break;

    case e_gfNormal90:
        {
            VertexAddColor(x0, y0, clrFrom);
            VertexAddColor(x1, y0, clrFrom);
            VertexAddColor(x0, y1, clrTo);
            VertexAddColor(x1, y1, clrTo);
            static const GXIndices Index[] = { 0, 2, 1, 2, 3, 1 };
            VertexFill(Index, 2 * 3);
        }
        break;
        
    case e_gfNormal135:
        {
            TMUInt8 r = (r0 + r1) / 2;
            TMUInt8 g = (g0 + g1) / 2;
            TMUInt8 b = (b0 + b1) / 2;
            TMUInt8 a = (a0 + a1) / 2;
            TMColor clr = TMRGBA(r,g,b,a);
            VertexAddColor(x1, y0, clrFrom);
            VertexAddColor(x0, y1, clrTo);
            VertexAddColor(x1, y1, clr);
            VertexAddColor(x0, y0, clr);
            static const GXIndices Index[] = { 0, 1, 2, 0, 1, 3};
            VertexFill(Index, 2 * 3);
        }
        break;

    case e_gfSym0:
        {
            VertexAddColor(x0, y0,             clrFrom);
            VertexAddColor((x0 + x1) / 2, y0,  clrTo);
            VertexAddColor(x1, y0,             clrFrom);
            VertexAddColor(x0, y1,             clrFrom);
            VertexAddColor((x0 + x1) / 2, y1,  clrTo);
            VertexAddColor(x1, y1,             clrFrom);
            static const GXIndices Index[] = { 0, 3, 1, 1, 3, 4, 1, 4, 2, 2, 4, 5 };
            VertexFill(Index, 4 * 3);
        }
        break;

    case e_gfSym45:
        {
            VertexAddColor(x0, y0, clrFrom);
            VertexAddColor(x1, y1, clrFrom);
            VertexAddColor(x1, y0, clrTo);
            VertexAddColor(x0, y1, clrTo);
            static const GXIndices Index[] = { 0, 2, 3, 2, 3, 1 };
            VertexFill(Index, 2 * 3);
        }
        break;

    case e_gfSym90:
        {
            VertexAddColor(x0, y0,             clrFrom);
            VertexAddColor(x1, y0,             clrFrom);
            VertexAddColor(x0, (y0 + y1)/2,    clrTo);
            VertexAddColor(x1, (y0 + y1)/2,    clrTo);
            VertexAddColor(x0, y1,             clrFrom);
            VertexAddColor(x1, y1,             clrFrom);
            static const GXIndices Index[] = { 0, 2, 1, 1, 2, 3, 2, 4, 3, 3, 4, 5 };
            VertexFill(Index, 4 * 3);
        }
        break;

    case e_gfSym135:
        {
            VertexAddColor(x0, y0, clrTo);
            VertexAddColor(x1, y0, clrFrom);
            VertexAddColor(x0, y1, clrFrom);
            VertexAddColor(x1, y1, clrTo);
            static const GXIndices Index[] = { 0, 1, 3, 0, 3, 2 };
            VertexFill(Index, 2 * 3);
        }
        break;

    case e_gfCorner45:
        {
            VertexAddColor(x0, y0, clrFrom);
            VertexAddColor(x1, y0, clrTo);
            VertexAddColor(x1, y1, clrTo);
            VertexAddColor(x0, y1, clrTo);
            static const GXIndices Index[] = { 0, 1, 2, 0, 3, 2 };
            VertexFill(Index, 2 * 3);
        }
        break;

    case e_gfCorner135:
        {
            VertexAddColor(x0, y0, clrTo);
            VertexAddColor(x1, y0, clrFrom);
            VertexAddColor(x1, y1, clrTo);
            VertexAddColor(x0, y1, clrTo);
            static const GXIndices Index[] = { 1, 2, 3, 1, 0, 3 };
            VertexFill(Index, 2 * 3);
        }
        break;

    case e_gfCenter:
        {
            VertexAddColor(x0,          y0,         clrTo);
            VertexAddColor(x1,          y0,         clrTo);
            VertexAddColor(x1,          y1,         clrTo);
            VertexAddColor(x0,          y1,         clrTo);
            VertexAddColor((x0+x1)/2,   (y0+y1)/2,  clrFrom);
            static const GXIndices Index[] = { 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4 };
            VertexFill(Index, 4 * 3);
        }
        break;
    }

#undef x0
#undef y0
#undef x1
#undef y1
}


void CGXAppBase::DrawIcon(const GXRect& rc, TMHIcon hIcon, TMBool bR2L)
{
    CTexture texture;
    texture.CreateFromGDIObject(hIcon);
    if (texture)
        texture->Blt(rc, CRect(0, 0, 0, 0), bR2L ? (GX_BLT_DFTSRCWH | GX_BLT_R2L) : GX_BLT_DFTSRCWH);
}


///////////////////////////////////////////////////////////////////////////////
// CGXDevMgr
CGXDevMgr::CGXDevMgr()
{
    CGXAppPlateform::Instance(&m_gxAppBase);
    m_iterCur = m_aSinks.end();
}

CGXDevMgr::~CGXDevMgr()
{
    m_gxAppBase->Release();
    m_gxAppBase = 0;
}

void CGXDevMgr::NotifyDispatch(CGXAppBase* pApp, TMResult hr)
{
    for (m_iterCur = m_aSinks.begin(); m_iterCur != m_aSinks.end();)
    {
        IGXAppNotify* p = *m_iterCur;
        ++m_iterCur;
        if (pApp)
            p->OnGXDeviceChanged(pApp);
        else
            p->OnGXDeviceError(hr);
    }
}

void CGXDevMgr::NotifyAdd(IGXAppNotify* p)
{
    m_aSinks.insert(p);
}

void CGXDevMgr::NotifyErase(IGXAppNotify* p)
{
    CGXSinkIter iter = m_aSinks.find(p);
    if (iter == m_aSinks.end())
        return; // no find sink
    
    if (iter == m_iterCur)
        ++m_iterCur;
    m_aSinks.erase(iter);
}

void CGXDevMgr::DoChangeDevice(CGXAppBase* pApp)
{
    if (pApp == m_gxAppBase)
        return;
    CGXAppBase* pOld = m_gxAppBase;
    m_gxAppBase = pApp;
    if (pApp)
        pApp->AddRef();
    
    // only have old device need notify.
    if (pOld)
    {
        TM_IF_DBG(int n1 = DbgUnknownGetRef(pOld));
        NotifyDispatch(pApp, 0);
        TM_IF_DBG(int n2 = DbgUnknownGetRef(pOld));
        pOld->Release();
    }
}

static TMBool _CGXDevMgr_Is3DPause()
{
#ifdef TM_WIN
    return (0 == ::GetForegroundWindow()); // lock screen.
#endif
    return false;
}

static TMBool _CGXDevMgr_IsVirtualDisplayCard(CGXAppBase* app)
{
    GXADAPTERDESC desc = { 0 };
    app->GetDeviceType(&desc);
    return desc.VendorId == nVendorID_Microsoft; // not support MS virtual display card.
}

void CGXDevMgr::ChangeDevice()
{
    KillAni();

    TMBool bTest3D = false;
    for (int n = 0; n < tm_countof(m_dftGraDev); ++n)
    {
        EGXDevice dev = m_dftGraDev[n];
#ifdef GXAPP_D3D9_H
        if (dev == eGXGraDevD3D9)
        {
            CGXAppD3D9T* p = new CGXAppD3D9T(0);
            if (p->CreateDevice() && !_CGXDevMgr_IsVirtualDisplayCard(p))
            {
                DbgTrace("CGXDevMgr::ChangeDevice() change device to eGXDeviceD3D9");
                DoChangeDevice(p);
                return;
            }
            bTest3D = true;
            delete p;
        }
        else
#endif
#ifdef GXAPP_D3D11_H
            if (dev == eGXGraDevD3D11)
        {
            CGXAppD3D11T* p = new CGXAppD3D11T(0);
            if (p->CreateDevice() && !_CGXDevMgr_IsVirtualDisplayCard(p))
            {
                DbgTrace("CGXDevMgr::ChangeDevice() change device to eGXDeviceD3D11");
                DoChangeDevice(p);
                return;
            }
            bTest3D = true;
            delete p;
        }
        else
#endif
#ifdef GXAPP_GL_H
            if (dev == eGXGraDevGL)
        {
//             CGXAppGL* p = CGXAppGL::CreateInst();
//             if (p)
            CGXAppGLT* p = new CGXAppGLT(0);
            if (p->CreateDevice() && !_CGXDevMgr_IsVirtualDisplayCard(p))
            {
                DbgTrace("CGXDevMgr::ChangeDevice() changed device to eGXDeviceGL");
                DoChangeDevice(p);
                return;
            }
            bTest3D = true;
        }
        else
#endif
            break;
    }

    CRefPtr<CGXAppPlateform::GXApp> gxApp;
    CGXAppPlateform::Instance(&gxApp);
    DbgTrace("CGXDevMgr::ChangeDevice() change device to eGXDeviceGDI");
    DoChangeDevice(gxApp);

    if (bTest3D && _CGXDevMgr_Is3DPause())
        SetDeviceLost(eDeviceReset3s);
}

void CGXDevMgr::SetDefaultGraDevices(const EGXDevice* devs, int nDevs, TMBool bUpdate)
{
    if (nDevs < 1)
        return;

    tm_setmin(nDevs, tm_countof(m_dftGraDev) - 1);
    std::copy(devs, devs + nDevs, m_dftGraDev);
    m_dftGraDev[nDevs] = eGXGraDevGDI;
    
    if (!bUpdate)
        return;

    if (m_gxAppBase)
    {
        if (!IsDeviceLost())
        {
            EGXDevice eType = m_gxAppBase->GetDeviceType();
            if (eType == devs[0])
                return;
            SetDeviceLost(eDeviceReset1ms);
        }
    }
    else
        ChangeDevice();
}

void CGXDevMgr::SetDefaultTxtDevices(EGXDevice dev, TMBool bUpdate)
{
    if (m_dftTxtDev == dev)
        return;
    m_dftTxtDev = dev;
    
    if (!bUpdate)
        return;

    m_gxAppBase->ResetFontDevice();
    NotifyDispatch(m_gxAppBase, 0);
}

void CGXDevMgr::SetDeviceLost(EDeviceReset eReset)
{
    if (eReset == eDeviceResetGDI)
    {
        m_dftGraDev[0] = eGXGraDevGDI;
        eReset = eDeviceReset1ms;
    }
    if (!m_tmID)
        SetAni(eReset);
    else if (eReset == eDeviceReset1ms)
        this->SetDelay(eDeviceReset1ms);
}

void CGXDevMgr::OnAni(TMHAni tm, ...)
{
    GX_TRACE("CGXDevMgr::OnAni() try change device");

    if (_CGXDevMgr_Is3DPause()) // lock screen.
    {
        this->SetDelay(eDeviceReset3s);
        return;
    }

    CGXAppBase* pOld = m_gxAppBase;
    if (pOld)
    {
        if (pOld->GetRenderTarget()) // is rending?
            return;
    }

    this->ChangeDevice();
}


// CGXDevMgr g_mgrDev;

TM_END_NAMESPACE


void gxSetDefaultGraDevices(TMBool bUpdate, EGXDevice d1, EGXDevice d2, EGXDevice d3, EGXDevice d4)
{
    EGXDevice dd[] = { d1, d2, d3, d4 };
    TM::CGXDevMgr::Instance()->SetDefaultGraDevices(dd, tm_countof(dd), bUpdate);
}

void gxSetDefaultTxtDevices(TMBool bUpdate, EGXDevice d1)
{
    TM::CGXDevMgr::Instance()->SetDefaultTxtDevices(d1, bUpdate);
}

IGXApp* gxGetApp()
{
    TM::CGXDevMgr* p = TM::CGXDevMgr::Instance();
    DbgAssert(p->m_gxAppBase);
    return p->m_gxAppBase;
}

void gxInit()
{
    TM::CGXDevMgr::Instance()->AddRef();
}


void gxTerm()
{
    int n = TM::CGXDevMgr::Instance()->Release();
    GX_TRACE("gxTerm() %d", n);
}

void gxSetNotify(IGXAppNotify* sink, TMBool bAdd)
{
    if (bAdd)
        TM::CGXDevMgr::Instance()->NotifyAdd(sink);
    else
    {
        TM::CGXDevMgr* p = TM::CGXDevMgr::GetInstanceNoAddRef();
        if (p)
            p->NotifyErase(sink);
    }
}

void gxFontGetDefault(GXFont& ft)
{
    TM::CGXFont::FontGetDefault(ft);
}






#ifdef TUI_EDIT_MODE

CComPtr<IGXApp> Edit_CreateD9Device(int nAdapter)
{
#ifdef GXAPP_D3D9_H
    TM::CGXAppD3D9T* p = new TM::CGXAppD3D9T(0);
    if (p->CreateDevice(nAdapter))
        return p;
    delete p;
#endif // GXAPP_D3D9_H
    return 0;
}

CComPtr<IGXApp> Edit_CreateD11Device(int nAdapter)
{
#ifdef GXAPP_D3D11_H
    TM::CGXAppD3D11T* p = new TM::CGXAppD3D11T(0);

    static CComPtr<IDXGIFactory> s_factory;
    if (nAdapter == 0)
    {
        if (p->CreateDevice(0))
        {
            s_factory = p->m_factory;
            return p;
        }
        return 0;
    }
    
    if (!s_factory)
        return 0;
    
    CComPtr<IDXGIAdapter> ptrAdapter;
    s_factory->EnumAdapters(nAdapter, &ptrAdapter);
    if (!ptrAdapter)
        return 0;
    
    if (p->CreateDevice(ptrAdapter))
        return p;

    delete p;
#endif // GXAPP_D3D11_H
    return 0;
}

#endif // TUI_EDIT_MODE