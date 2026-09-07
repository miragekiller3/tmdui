/*
 *  @file
 *  @brief  GDI render
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.13
 *
 *  Copyright (C) 2011 miragekiller
 */

#include "gxapp_win.h"
#include "tmgdi.h"
#include "gxtexture_blt.h"


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// CTexture_UpdateLayeredWindowDC && CTexture_UpdateLayeredWindowBuffer
inline static EGXRet _IGXTextureImpl_PresentLayeredWin(TMHWnd hWnd, HDC hSrc, EGXPresent eFlag, GXPRESENTFX* fx)
{
    if (!hSrc)
        return eGXRetFail;

//     if ((eFlag & eGXPresentDCMask) == eGXPresentDCPaint)
//     {
//         CPaintDC dc(hWnd);
//     }
    UpdateLayeredWin(hWnd, fx->ptDstScreen, fx->szDst, hSrc, fx->ptSrc, fx->alpha, fx->rcDirty);
    return eGXRetSuccess;
}

EGXRet CTexture_UpdateLayeredWindowDC::PresentLayeredWin(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx)
{
    CAutoTextureDC dc(this);
    return _IGXTextureImpl_PresentLayeredWin(hWnd, dc, eFlag, fx);
}

// EGXRet CTexture_UpdateLayeredWindowBuffer::PresentLayeredWin(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx)
// {
//     CImage img;
//     {
//         CAutoTextureLockBits32 src(this, 0, GX_LOCK_RECT_READ);
//         img.CreateIndirect(GetWidth(), GetHeight(), 32);
//         if (!src || !img)
//             return eGXRetFail;
//         CAutoImageLockBits32 dst(img, 0, GX_LOCK_RECT_WRITE);
//         DibBlt(dst, 0, 0, src);
//     }
// 
//     CImageDC dc(img);
//     return _IGXTextureImpl_PresentLayeredWin(hWnd, dc, eFlag, fx);
// }


////////////////////////////////////////////////////////////////////////////////
// CTextureImage
void CTextureImage::OnSetRenderTarget(CGXAppBase* _app)
{
    CGXAppWin* app = static_cast<CGXAppWin*>(_app);
    app->m_dcBk = m_img.CreateDC();
    DbgVerify(app->m_dcBk.SetBkMode(TRANSPARENT));
    DbgVerify(app->m_dcBk.SetStretchBltMode(COLORONCOLOR));
    DbgVerify(SetGraphicsMode(app->m_dcBk, GM_ADVANCED));
}

void CTextureImage::OnResetRenderTarget(CGXAppBase* _app, GXPTexture pNew)
{
    CGXAppWin* app = static_cast<CGXAppWin*>(_app);
    DbgAssert(app->m_dcBk);
    m_img.DeleteDC(app->m_dcBk);
    app->m_dcBk = 0;
}

// IGXTexture
void CTextureImage::Blt(const GXRect& rcDst, const TMRectD& rcSrc, TMUInt32 uFlag, TMUInt8 alpha)
{
    CRectI rcd;
    CGXAppWin* app = CGXAppWin::Instance();
    CZoom dpi = app->GetDPI();
    dpi.LPtoDP(rcDst, rcd);
    if (uFlag & GX_BLT_TILE)
    {
        CImageDC dc(m_img);
        CBltsSelect bs(app->m_dcBk, &rcDst, dpi, uFlag, alpha, GetFormat());
        bs.Tile(rcd, dc, dpi, GetWidth(), GetHeight());
    }
    else
    {
        Check_GX_BLT_TEXTMONO(uFlag, alpha, app);
        this->BltToDC(app->m_dcBk, rcd, rcSrc, uFlag, alpha);
    }
}

void CTextureImage::Blts(const GXRect* rcDsts, const TMRectD* rcSrcs, int nCount, TMUInt32 uFlag, TMUInt8 alpha)
{
    CGXAppWin* app = CGXAppWin::Instance();
    Check_GX_BLT_TEXTMONO(uFlag, alpha, app);
    CBltsSelect bs(app->m_dcBk, rcDsts + nCount, app->GetDPI(), uFlag, alpha, GetFormat());
    CImageDC dc(m_img);
    for (int i = 0; i < nCount; ++i)
    {
        CRect rcd;
        app->GetDPI().LPtoDP(rcDsts[i], rcd);
        bs.SBlt(rcd, dc, rcSrcs[i]);
    }
}

static EGXRet _CTextureImage_Present(CTextureImage* tex, HWND hDest, CDCHandle dc, const TMRect* clip)
{
    HRGN hrgn = CreateRectRgn(0,0,0,0);
    for (HWND wnd = ::GetWindow(hDest, GW_CHILD); wnd; wnd = ::GetWindow(wnd, GW_HWNDNEXT))
    {
        if (!(GetWindowLong(wnd, GWL_STYLE) & WS_VISIBLE))
            continue;
        
        CRect rc;
        ::GetWindowRect(wnd, &rc);
        ::ScreenToClient(hDest, (LPPOINT)&rc);
        ::ScreenToClient(hDest, ((LPPOINT)&rc)+1);
        
        int regionType = GetWindowRgn(wnd, hrgn);
        if (regionType != ERROR)
        {
            rc.NormalizeRect();
            DbgVerifyNotEqual(ERROR, OffsetRgn(hrgn, rc.left, rc.top));
            DbgVerifyNotEqual(ERROR, ExtSelectClipRgn(dc, hrgn, RGN_DIFF));
        }
        else
        {
            DbgVerifyNotEqual(ERROR, dc.ExcludeClipRect(&rc));
        }
    }
    DbgVerify(DeleteObject(hrgn)); /* finished with region */
    
    TMUInt32 uFlag = 0;
    if (GetWindowLong(hDest, GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
    {
        SetLayout(dc, 0);
        if (clip)
        {
            int w = tex->GetWidth();
            CRect rc(w - clip->right, clip->top, w - clip->left, clip->bottom);
            tex->BltToDC(dc, rc, rc, GX_BLT_SRCCOPY);
            return eGXRetSuccess;
        }
    }
    
    if (clip)
        tex->BltToDC(dc, *clip, *clip, GX_BLT_SRCCOPY);
    else
        tex->BltToDC(dc, CRect(0,0,0,0), CRect(0,0,0,0), GX_BLT_SRCCOPY | GX_BLT_DFTDSTSRCWH);
    
    return eGXRetSuccess;
}

EGXRet CTextureImage::Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx)
{
    if (gxGetApp() != GetApp())
        return eGXRetDeviceChanged;

    if (eFlag == eGXPresentLayered)
        return this->PresentLayeredWin(hWnd, eFlag, fx);

    if ((eFlag & eGXPresentDCMask) == eGXPresentDCClient)
        return _CTextureImage_Present(this, hWnd, (HDC)CClientDC(hWnd), fx->rcDirty);

    CPaintDC dc(hWnd);
    return _CTextureImage_Present(this, hWnd, (HDC)dc, &dc.m_ps.rcPaint);
}


// IGXTextureImpl
void CTextureImage::BltText(const GXRect& rcDst, TMUInt32 uFlag, TMColor clr)
{
    CGXAppWin* app = CGXAppWin::Instance();
    TMUInt8 alpha = ColorA(clr);
    if (uFlag & GX_BLT_TEXTMONO)
        Check_GX_BLT_TEXTMONO(uFlag, clr);
    CRect rcx;
    app->GetDPI().LPtoDP(rcDst, rcx);
    BltToDC(app->m_dcBk, rcx, CRect(0,0,GetWidth(), GetHeight()), uFlag, alpha);
}

// void CTextureImage::MakeTextColor(TMColor clr)
// {
//     CAutoTextureLockBits32 dib(this);
//     DibEnumPixel(dib, TextDrawMakeRGB(clr));
// }


////////////////////////////////////////////////////////////////////////////////
// CGXAppWin
CGXAppWin::CGXAppWin()
    : m_pBk (0)
{
//    m_font = CGXFont::FontCreateDefaultAuto();
    ResetFontDevice();
}

CGXAppWin::~CGXAppWin()
{
    DbgAssert(!m_stk.size());
}

void CGXAppWin::ClipDevNow()
{
    DbgVerifyEqual(SIMPLEREGION, m_dcBk.SelectClipRgn(0, RGN_COPY));
    DbgVerifyNotEqual(ERROR, m_dcBk.IntersectClipRect(m_rcClip));
    GX_TRACE("CGXAppWin::ClipDevNow(" TM_FMT_RCI_STR ")", TM_FMT_RCI_VAL(m_rcClip));
}

void CGXAppWin::InitDevTransform()
{
    _gx_check();
    float fDPI = GetDPI();
    float f1 = (float)TM_DPI_SYSTEM_DEFAULT / fDPI;
    float f2 = fDPI / (float)TM_DPI_SYSTEM_DEFAULT;
    CAffine aff(f1,0, 0,f1, 0,0);
    aff *= m_aff;
    aff.Scale(f2, f2);
    if (IsRTL())
    {
        CAffine affx;
        affx.Reset();
        affx.FlipX(m_pBk->GetWidth());
        aff *= affx;
    }

//     {
//         GXPoint pt(159,64);
//         aff.MapPoint(pt);
//         int x = 0;
//         DbgTrace("CGXAppWin::InitDevTransform(%d,%d) map 159,64 to (%g,%g)",
//             m_pBk->GetWidth(), m_pBk->GetHeight(), pt.x, pt.y);
//     }

//     if (m_local.IsRTL())
//         aff.Rotate(360); // AlphaBlend not support (LAYOUT_RTL and Identity-Matrix);

//     CAffine aff(m_aff);
//     float f = (float)TM_DPI_SYSTEM_DEFAULT / fDPI;
//    aff.Scale(f, f);
//     DbgVerifyNotEqual(GDI_ERROR, SetLayout(m_dcBk, m_local.IsRTL() ? LAYOUT_RTL : 0));
   ::SetWorldTransform(m_dcBk, &aff);
}


EGXRet CGXAppWin::InitProp(GXPTexture texture, int nDPI, TMUInt32 dwFlag)
{
    return _GXApp_InitProp(this, static_cast<CTextureImage*>(texture), nDPI, dwFlag);
}

// void CGXAppWin::TextureCreateIndirect(IGXTexture** tex, int w, int h, EGXFormat format)
// {
//     DbgAssert(!*tex);
//     HIMAGE hImg = ImageCreateIndirect(w, h, format, 0, 0);
//     *tex = DBG_INIT(CTextureImage::CreateInst(hImg));
// }

void CGXAppWin::TextureCreateFromImage(IGXTexture** tex, HIMAGE img)
{
    DbgAssert(tex && !*tex);
    if (!img)
        return;
    ImageAddRef(img);
//     bool b = CCanRecreate<CRefObjectT<CTextureImage> >::test(*tex);
    *tex = CTextureImage::CreateInst(img);
}

void CGXAppWin::DrawIcon(const GXRect& rc, HICON hIcon, TMBool bR2L)
{
    CImage img;
    img.CreateFromGDIObject(hIcon);
    if (!img)
        return;
    
    CRectI rcd;
    GetDPI().LPtoDP(rc, rcd);
    CImage imgT;
    if (rcd.Width() == img.GetWidth() && rcd.Height() == img.GetHeight())
        imgT = img;
    else
        imgT.CreateThumbnail(img, rcd.Width(), rcd.Height());
    imgT.StretchBlt(m_dcBk, rcd.Left(), rcd.Top(), rcd.Width(), rcd.Height(),
        0, 0, 0, 0, bR2L ? (GX_BLT_DFTSRCWH | GX_BLT_R2L) : GX_BLT_DFTSRCWH);
}

//void CGXAppWin::DrawDC(const GXRect& rcDst, HDC hSrc, const TMRectI& rcSrc, TMUInt32 dwRop)
//{
//    CRectI rcd;
//    GetDPI().LPtoDP(rcDst, rcd);
//    GDIBlt24To32(m_dcBk, rcd.left, rcd.top, rcd.Width(), rcd.Height(),
//        hSrc, rcSrc.left, rcSrc.top, rcSrc.right - rcSrc.left, rcSrc.bottom - rcSrc.top, dwRop); 
//}

void CGXAppWin::Clear(TMColor clr)
{
    _gx_check();
    CRectI rci(0, 0, m_pBk->GetWidth(), m_pBk->GetHeight());
    GDIFillRect(m_dcBk, rci, clr);
}

void CGXAppWin::FillRect(const GXRect& rc, TMColor clr)
{
    _gx_check();
    CRectI rci;
    GetDPI().LPtoDP(rc, rci);
    GDIFillRect(m_dcBk, rci, clr);
}

void CGXAppWin::InvertRect(const GXRect& rcx)
{
    _gx_check();
    
    CRectI rc;
    GetDPI().LPtoDP(rcx, rc);
    GDIInvertRect(m_dcBk, rc);
}

void CGXAppWin::FrameRect(const GXRect& rc, GXDips width, TMColor clr)
{
    _gx_check();

    CRectI rc1, rc2;
    GetDPI().LPtoDP(rc, rc1);

    GXRect rcInner(rc);
    rcInner.InflateRect(-width, -width);
    GetDPI().LPtoDP(rcInner, rc2);

    GDIFrameRect(m_dcBk, rc1, rc2, clr);
}

void CGXAppWin::DrawFocusRect(const GXRect& rc)
{
    _gx_check();
    CRectI rcx;
    GetDPI().LPtoDP(rc, rcx);
    m_dcBk.DrawFocusRect(&rcx);
}

CGXAppWin* CVertexWin::GetApp()
{
    return TM_OTHIS(CGXAppWin, m_vertex);
}

void CVertexWin::Render(const GXIndices* pt, TMUInt nt)
{
    enum { nMax = 24 };
    int i = 0;
    TRIVERTEX* pv = m_aTv.data();
    int nv = m_aTv.size();
    for (; i <= (int)nt - nMax; i += nMax)
        DbgVerify(GradientFill(GetApp()->m_dcBk, pv, nv, (void*)(pt + i), nMax / 3, GRADIENT_FILL_TRIANGLE));
    if (i < nt)
        DbgVerify(GradientFill(GetApp()->m_dcBk, pv, nv, (void*)(pt + i), (nt - i) / 3, GRADIENT_FILL_TRIANGLE));
}

void CGXAppWin::VertexBegin()
{
    m_vertex.Clear();
}

void CGXAppWin::VertexAddColor(GXDips x, GXDips y, TMColor clr)
{
    CPointI pt;
    GetDPI().LPtoDP(tm_addressof(GXPoint(x,y)), 1, &pt);
    m_vertex.Add(pt.X(), pt.Y(), ColorR(clr), ColorG(clr), ColorB(clr), ColorA(clr));
}

void CGXAppWin::VertexFill(const GXIndices* pt, TMUInt nt)
{
    m_vertex.Render(pt, nt);
}


/////////////////////////////////////////////////////////////////////////
// CGXAppBase

TM_END_NAMESPACE


void gxDrawLine(const GXPoint& from, const GXPoint& to, int width, TMColor clr)
{
    TM::CGXAppWin* app = TM::CGXAppWin::Instance();
    app->_gx_check();
    CPen pen;
    pen.CreatePen(PS_SOLID, width, ColorToCOLORREF(clr));
    HPEN hOld = app->m_dcBk.SelectPen(pen);
    GXRect rc(from, to);
    TM::CRectI rci;
    app->GetDPI().LPtoDP(rc, rci);
    //    CZoom zoom(m_dpi);
    //     m_dcBk.MoveTo(zoom.LPtoDP(from.x), zoom.LPtoDP(from.y));
    //     m_dcBk.LineTo(zoom.LPtoDP(to.x), zoom.LPtoDP(to.y));
    app->m_dcBk.MoveTo(rci.Left(), rci.Top());
    app->m_dcBk.LineTo(rci.Right(), rci.Bottom());
    app->m_dcBk.SelectPen(hOld);
}

void gxDrawPolyline(const GXPoint* pt, int n, int width, TMColor clr)
{
    TM::CGXAppWin* app = TM::CGXAppWin::Instance();
    app->_gx_check();
    CPen pen;
    pen.CreatePen(PS_SOLID, width, ColorToCOLORREF(clr));
    HPEN hOld = app->m_dcBk.SelectPen(pen);
    TMPoint* ptx = (TMPoint*)_alloca(sizeof(TMPoint) * n);
    app->GetDPI().LPtoDP(pt, n, ptx);
    app->m_dcBk.Polyline(ptx, n);
    app->m_dcBk.SelectPen(hOld);
}


// #endif  // TM_RENDER_GDI

