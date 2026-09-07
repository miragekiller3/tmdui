/*
 *  @file
 *  @brief GTK render
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2026.4.13
 *
 *  Copyright (C) 2026 miragekiller
 */

#include "gxapp_gtk.h"

TM_BGN_NAMESPACE

static inline void _CairoSetColor(cairo_t* cr, TMColor clr, TMUInt8 alphaMul = 0xff)
{
    const double r = ColorR(clr) / 255.0;
    const double g = ColorG(clr) / 255.0;
    const double b = ColorB(clr) / 255.0;
    const double a = (ColorA(clr) / 255.0) * (alphaMul / 255.0);
    cairo_set_source_rgba(cr, r, g, b, a);
}

EGXRet CTexture_UpdateLayeredWindowDC::PresentLayeredWin(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx)
{
    tm_unused(hWnd);
    tm_unused(eFlag);
    tm_unused(fx);
    return eGXRetFail;
}

void CTextureImage::OnSetRenderTarget(CGXAppBase* _app)
{
    CGXAppGtk* app = static_cast<CGXAppGtk*>(_app);
    app->m_dcBk = m_img.CreateDC();
}

void CTextureImage::OnResetRenderTarget(CGXAppBase* _app, GXPTexture pNew)
{
    tm_unused(pNew);
    CGXAppGtk* app = static_cast<CGXAppGtk*>(_app);
    if (app->m_dcBk)
    {
        m_img.DeleteDC(app->m_dcBk);
        app->m_dcBk = 0;
    }
}

void CTextureImage::Blt(const GXRect& rcDst, const TMRectD& rcSrc, TMUInt32 uFlag, TMUInt8 alpha)
{
    CRectI rcd;
    CGXAppGtk* app = CGXAppGtk::Instance();
    CZoom dpi = app->GetDPI();
    dpi.LPtoDP(rcDst, rcd);

    if (uFlag & GX_BLT_TILE)
    {
        int sw = GetWidth();
        int sh = GetHeight();
        if (sw <= 0 || sh <= 0)
            return;

        cairo_t* cr = (cairo_t*)app->m_dcBk;
        cairo_save(cr);
        cairo_rectangle(cr, rcd.Left(), rcd.Top(), rcd.Width(), rcd.Height());
        cairo_clip(cr);
        for (int x = rcd.Left(); x < rcd.Right(); x += sw)
        {
            for (int y = rcd.Top(); y < rcd.Bottom(); y += sh)
            {
                ImageStretchBlt(app->m_dcBk, x, y, sw, sh, m_img, 0, 0, sw, sh, uFlag & ~GX_BLT_TILE, alpha);
            }
        }
        cairo_restore(cr);
        return;
    }

    Check_GX_BLT_TEXTMONO(uFlag, alpha, app);
    this->BltToDC(app->m_dcBk, rcd, rcSrc, uFlag, alpha);
}

void CTextureImage::Blts(const GXRect* rcDsts, const TMRectD* rcSrcs, int nCount, TMUInt32 uFlag, TMUInt8 alpha)
{
    CGXAppGtk* app = CGXAppGtk::Instance();
    Check_GX_BLT_TEXTMONO(uFlag, alpha, app);
    CBltsSelect bs(app->m_dcBk, rcDsts + nCount, app->GetDPI(), uFlag, alpha, GetFormat());
    CImageDC dc(m_img);
    for (int i = 0; i < nCount; ++i)
    {
        CRect rcd;
        app->GetDPI().LPtoDP(rcDsts[i], rcd);
        bs.SBlt(rcd, m_img.GetBitmap(), rcSrcs[i]);
    }
}

EGXRet CTextureImage::Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx)
{
    tm_unused(eFlag);
    tm_unused(fx);

    if (gxGetApp() != GetApp())
        return eGXRetDeviceChanged;

    CClientDCGtk dc(hWnd);
    this->BltToDC(dc, CRect(0,0,0,0), CRect(0,0,0,0), GX_BLT_SRCCOPY | GX_BLT_DFTDSTSRCWH);
    return eGXRetSuccess;
}

void CTextureImage::BltText(const GXRect& rcDst, TMUInt32 uFlag, TMColor clr)
{
    CGXAppGtk* app = CGXAppGtk::Instance();
    TMUInt8 alpha = ColorA(clr);
    if (uFlag & GX_BLT_TEXTMONO)
        Check_GX_BLT_TEXTMONO(uFlag, clr);
    CRect rcx;
    app->GetDPI().LPtoDP(rcDst, rcx);
    BltToDC(app->m_dcBk, rcx, CRect(0, 0, GetWidth(), GetHeight()), uFlag, alpha);
}

CGXAppGtk::CGXAppGtk()
    : m_pBk(0)
    , m_dcBk(0)
{
    ResetFontDevice();
}

CGXAppGtk::~CGXAppGtk()
{
    DbgAssert(!m_stk.size());
}

void CGXAppGtk::ClipDevNow()
{
    if (!m_dcBk)
        return;

    cairo_t* cr = (cairo_t*)m_dcBk;
    cairo_reset_clip(cr);
    cairo_rectangle(cr, m_rcClip.Left(), m_rcClip.Top(), m_rcClip.Width(), m_rcClip.Height());
    cairo_clip(cr);
}

void CGXAppGtk::InitDevTransform()
{
    if (!m_dcBk)
        return;

    cairo_t* cr = (cairo_t*)m_dcBk;
    cairo_identity_matrix(cr);
    if (IsRTL() && m_pBk)
    {
        cairo_translate(cr, m_pBk->GetWidth(), 0.0);
        cairo_scale(cr, -1.0, 1.0);
    }
}

EGXRet CGXAppGtk::InitProp(GXPTexture texture, int nDPI, TMUInt32 dwFlag)
{
    return _GXApp_InitProp(this, static_cast<CTextureImage*>(texture), nDPI, dwFlag);
}

void CGXAppGtk::TextureCreateFromImage(IGXTexture** tex, HIMAGE img)
{
    DbgAssert(tex && !*tex);
    if (!img)
        return;
    ImageAddRef(img);
    *tex = CTextureImage::CreateInst(img);
}

void CGXAppGtk::DrawIcon(const GXRect& rc, TMHIcon hIcon, TMBool bR2L)
{
    tm_unused(rc);
    tm_unused(hIcon);
    tm_unused(bR2L);
}

void CGXAppGtk::Clear(TMColor clr)
{
    _gx_check();
    cairo_t* cr = (cairo_t*)m_dcBk;
    _CairoSetColor(cr, clr);
    cairo_rectangle(cr, 0.0, 0.0, m_pBk->GetWidth(), m_pBk->GetHeight());
    cairo_fill(cr);
}

void CGXAppGtk::FillRect(const GXRect& rc, TMColor clr)
{
    _gx_check();
    CRectI rci;
    GetDPI().LPtoDP(rc, rci);
    cairo_t* cr = (cairo_t*)m_dcBk;
    _CairoSetColor(cr, clr);
    cairo_rectangle(cr, rci.Left(), rci.Top(), rci.Width(), rci.Height());
    cairo_fill(cr);
}

void CGXAppGtk::InvertRect(const GXRect& rcx)
{
    _gx_check();
    CRectI rc;
    GetDPI().LPtoDP(rcx, rc);

    cairo_t* cr = (cairo_t*)m_dcBk;
    cairo_save(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_rectangle(cr, rc.Left(), rc.Top(), rc.Width(), rc.Height());
    cairo_fill(cr);
    cairo_restore(cr);
}

void CGXAppGtk::FrameRect(const GXRect& rc, GXDips width, TMColor clr)
{
    _gx_check();

    CRectI rc1;
    GetDPI().LPtoDP(rc, rc1);

    cairo_t* cr = (cairo_t*)m_dcBk;
    _CairoSetColor(cr, clr);
    cairo_set_line_width(cr, tm_max(1.0f, GetDPI().LPtoDP(width)));
    cairo_rectangle(cr, rc1.Left(), rc1.Top(), rc1.Width(), rc1.Height());
    cairo_stroke(cr);
}

void CGXAppGtk::DrawFocusRect(const GXRect& rc)
{
    _gx_check();

    CRectI rcx;
    GetDPI().LPtoDP(rc, rcx);

    cairo_t* cr = (cairo_t*)m_dcBk;
    _CairoSetColor(cr, TMRGBA(255, 255, 255, 255));
    cairo_set_line_width(cr, 1.0);
    const double dashes[] = { 2.0, 2.0 };
    cairo_set_dash(cr, dashes, 2, 0.0);
    cairo_rectangle(cr, rcx.Left() + 0.5, rcx.Top() + 0.5, rcx.Width() - 1.0, rcx.Height() - 1.0);
    cairo_stroke(cr);
    cairo_set_dash(cr, 0, 0, 0.0);
}

CGXAppGtk* CVertexGtk::GetApp()
{
    return TM_OTHIS(CGXAppGtk, m_vertex);
}

void CVertexGtk::Render(const GXIndices* pt, TMUInt nt)
{
    CGXAppGtk* app = GetApp();
    if (!app || !app->m_dcBk)
        return;

    cairo_t* cr = (cairo_t*)app->m_dcBk;
    for (TMUInt i = 0; i + 2 < nt; i += 3)
    {
        TMUInt ia = pt[i];
        TMUInt ib = pt[i + 1];
        TMUInt ic = pt[i + 2];
        if (ia >= (TMUInt)m_aTv.size() || ib >= (TMUInt)m_aTv.size() || ic >= (TMUInt)m_aTv.size())
            continue;

        const CVertex& a = m_aTv[ia];
        const CVertex& b = m_aTv[ib];
        const CVertex& c = m_aTv[ic];
        TMColor clr = TMRGBA(
            (ColorR(a.clr) + ColorR(b.clr) + ColorR(c.clr)) / 3,
            (ColorG(a.clr) + ColorG(b.clr) + ColorG(c.clr)) / 3,
            (ColorB(a.clr) + ColorB(b.clr) + ColorB(c.clr)) / 3,
            (ColorA(a.clr) + ColorA(b.clr) + ColorA(c.clr)) / 3);

        _CairoSetColor(cr, clr);
        cairo_move_to(cr, a.x, a.y);
        cairo_line_to(cr, b.x, b.y);
        cairo_line_to(cr, c.x, c.y);
        cairo_close_path(cr);
        cairo_fill(cr);
    }
}

void CGXAppGtk::VertexBegin()
{
    m_vertex.Clear();
}

void CGXAppGtk::VertexAddColor(GXDips x, GXDips y, TMColor clr)
{
    CPointI pt;
    GetDPI().LPtoDP(tm_addressof(GXPoint(x, y)), 1, &pt);
    m_vertex.Add(pt.X(), pt.Y(), clr);
}

void CGXAppGtk::VertexFill(const GXIndices* pt, TMUInt nt)
{
    m_vertex.Render(pt, nt);
}

TM_END_NAMESPACE

void gxDrawLine(const GXPoint& from, const GXPoint& to, int width, TMColor clr)
{
    TM::CGXAppGtk* app = TM::CGXAppGtk::Instance();
    app->_gx_check();

    cairo_t* cr = (cairo_t*)app->m_dcBk;
    TM::CPointI p1;
    TM::CPointI p2;
    app->GetDPI().LPtoDP(&from, 1, &p1);
    app->GetDPI().LPtoDP(&to, 1, &p2);

    cairo_set_line_width(cr, width > 0 ? width : 1);
    cairo_set_source_rgba(cr, ColorR(clr) / 255.0, ColorG(clr) / 255.0, ColorB(clr) / 255.0, ColorA(clr) / 255.0);
    cairo_move_to(cr, p1.X(), p1.Y());
    cairo_line_to(cr, p2.X(), p2.Y());
    cairo_stroke(cr);
}

void gxDrawPolyline(const GXPoint* pt, int n, int width, TMColor clr)
{
    if (!pt || n < 2)
        return;

    TM::CGXAppGtk* app = TM::CGXAppGtk::Instance();
    app->_gx_check();

    cairo_t* cr = (cairo_t*)app->m_dcBk;
    cairo_set_line_width(cr, width > 0 ? width : 1);
    cairo_set_source_rgba(cr, ColorR(clr) / 255.0, ColorG(clr) / 255.0, ColorB(clr) / 255.0, ColorA(clr) / 255.0);

    TM::CPointI p;
    app->GetDPI().LPtoDP(pt, 1, &p);
    cairo_move_to(cr, p.X(), p.Y());
    for (int i = 1; i < n; ++i)
    {
        app->GetDPI().LPtoDP(pt + i, 1, &p);
        cairo_line_to(cr, p.X(), p.Y());
    }
    cairo_stroke(cr);
}

