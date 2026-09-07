/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.12.15
 *
 *  Copyright (C) 2016 miragekiller
 */

#include "picbmp.h"
#include "view.h"


TM_BGN_NAMESPACE


inline void Line(IGXApp* gapp, int x1, int y1, int x2, int y2, TMColor clr)
{
    GXRect rc(x1-1, y1-1, x2 + 1, y2 + 1);
    gapp->FillRect(rc, clr);
}

////////////////////////////////////////////////////////////////////////////////
//CPicBmpBase
//ITuiPic
CComPtr<ITuiPic> CPicBmpBase::GetSubPic(int nDpi)
{
    if (nDpi == GetDPI())
        return this;

    TMBool bForceUnshared = m_ptrStream ? true : false; // custom pic is unshared object.
    CPicBmpBase* p = static_cast<CPicBmpBase*>(CPicPtr::CreateNative(GetModule(), 0, GetObjectID(), nDpi, bForceUnshared));
    CComPtr<ITuiPic> ptr;
    ptr.Attach(p);
    if (p != this && p)
    {
        if (m_ptrStream)
            p->SetStreamSource(m_ptrStream);
        int nCurFrame = this->GetCurrentFrame();
        p->SetCurFrame(nCurFrame);
    }
    return ptr;//m_ptrOwner->GetSubPic(nDpi);
}

void CPicBmpBase::SetAlpha(TMUInt8 nAlpha) 
{
    m_alpha = nAlpha; 
}

TMBool CPicBmpBase::IsLoaded(IGXTexture* texture) const
{
    return m_bLoaded && (texture ? texture->GetApp() == gxGetApp() : true);
}

void CPicBmpBase::LoadImage(CImage& pic, TMBool bClrKey)
{
    DbgAssert(GetModule());
    CAutoMsgAnalysis awm("CPicBmpBase::LoadImage", GetObjectID());

    m_bLoaded = true;

    CComPtr<IStream> stream;
    LARGE_INTEGER ui = {0};
    if (m_ptrStream)
    {
        stream = m_ptrStream;
        stream->Seek(ui, STREAM_SEEK_CUR, (ULARGE_INTEGER*)&ui);
    }
    else
        GetView()->GetResourceStream(GetModule()->m_hRes, m_strFile, TUI_RES_PIC_TYPE, &stream);

    m_bMultiDPI = false;
    pic.Release();
    IColorTransform* pCT = GetColorTransform(); //IsFlagSupportDark() ? GetModule()->m_pColorTransform : 0;
    if (bClrKey)
    {
        TMColor clr = TMRGB(255, 0, 255);
        pic.CreateFromStream(stream, this, &clr, pCT, GetObjectID());
    }
    else
        pic.CreateFromStream(stream, this, 0, pCT, GetObjectID());

    // only RGBA32 support mono
    if (IsFlagMonoOnHC() && (GetModule()->GetVisual() & eColorVisualHighContrast))
        ImageConvertToText(pic, TMRGBA(255,255,255,255));

        //pic.ConvertBitCount(eGXFormat32A);

    if (!pic)
        DbgTrace(TM_T("CPicBmpBase::LoadImage(%s) fail"), m_strFile.c_str());

    if (m_ptrStream)
        m_ptrStream->Seek(ui, STREAM_SEEK_SET, 0);

//     if (!m_bMultiDPI)
//     {
//         m_orgSize.cx = pic.GetWidth();
//         m_orgSize.Height() = pic.GetHeight();
//     }
}

void CPicBmpBase::LoadTexture(CTexture& pic, TMBool bClrKey)
{
    CImage img;
    LoadImage(img, bClrKey);
    pic.Release();
    pic.CreateFromImage(img);
}

GXDips CPicBmpBase::DoDecideSize(CSize& sz, int nSplite)
{
    m_orgSize = sz;
    m_bMultiDPI = true;
    CTuiZoom zoom(GetDPI());
    int w = zoom.LPtoDPCeil((GXDips)sz.Width() / (GXDips)nSplite) * nSplite;
    int h = CMath::MulDiv(sz.Height(), w, sz.Width());
    GXDips s = (GXDips)w / (GXDips)sz.Width();
    sz.SetSize(w, h);
    return s;
}

void CPicBmpBase::OnLoad()
{
    m_bLoaded = false;
}

// CTuiObject
TMResult CPicBmpBase::Dispatch(TMUInt msg, TMWParam wp, TMLParam lp)
{
    if (msg == CM_COLORVISUAL)
    {
        if (IsFlagSupportDark() && (wp & eColorVisualTransform))
            m_bLoaded = false;
        else if (IsFlagMonoOnHC() && (wp & eColorVisualHighContrast))
            m_bLoaded = false;
    }
    else if (msg == CM_RENDERCHANGED)
    {
        m_bLoaded = false;
        m_txt.Release();
    }
    return CPic::Dispatch(msg, wp, lp);
}


////////////////////////////////////////////////////////////////////////////////
//CPicBmp
CPicBmp::CPicBmp()
    : m_nStates(5)
    , m_clrHC(0)
{
    m_nFlag = CBltSharp::e_all;
}

void CPicBmp::Reload()
{
    if (IsLoaded(m_txt))
        return;

    LoadImage(m_image, IsFlagColorKey());
    m_txt.Release();
    m_txt.CreateFromImage(m_image);
    if (m_image.GetFrameCount() <= 1 || !m_txt)
        m_image.Release();
    m_clrHC = 0;
}

void CPicBmp::GetSize(TMSizeI& sz)
{
    Reload();
    if(m_txt)
    {
        SizeSetWidth(sz, m_txt->GetWidth() / m_nStates);
        SizeSetHeight(sz, m_txt->GetHeight());
    }
    else
    {
        SizeSetWidth(sz, 0);
        SizeSetHeight(sz, 0);
    }
}

TMUInt CPicBmp::GetFrames()
{
    Reload();
    return m_image ? ImageGetFrameCount(m_image) : 1;
}

int CPicBmp::GetDelay()
{
    Reload();
    if (m_image)
        return ImageGetDelay(m_image);
    else
        return 1;
}

TMUInt CPicBmp::GetCurrentFrame()
{
    if (m_image)
        return ImageGetCurrentFrame(m_image);
    else
        return 0;
}

void CPicBmp::SetCurFrame (TMUInt n)
{
    Reload();
    if (m_image)
    {
        if (ImageSetCurrentFrame(m_image, n))
        {
            m_txt->ResetAsImage(m_image);
            m_clrHC = 0;
        }
    }
}

void CPicBmp::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    Reload();
    if (m_txt)
        RenderNative(gapp, nState, m_txt, rc, fx);
}

GXDips CPicBmp::OnImageDecideSize(CSize& sz)
{
    return DoDecideSize(sz, m_nStates);
}

void CPicBmp::OnLoadProp (IDomElement* pXml)
{
    if (m_nStates <= 0)
        m_nStates = 1;
    
    OnLoad();
}

void CPicBmp::RenderNative(IGXApp* gapp, int nFrame, CTexture& txt, const CTuiRect& rcx, EGXBlt eBlt, int w, int h, int wSrc, int nAlpha)
{
    TM::CRect rcs;
    rcs.SetLTWH(w * nFrame, 0, w, h);

    // cacl dest rect
    GXDips dl1 = rcx.Left();
    GXDips dw1 = m_dra.left;
    GXDips dw3 = m_dra.right;
    GXDips dl3 = rcx.Right() - m_dra.right;
    GXDips dl2 = rcx.Left() + m_dra.left;
    GXDips dw2 = dl3 - dl2;

    GXDips dt1 = rcx.Top();
    GXDips dh1 = m_dra.top;
    GXDips dh3 = m_dra.bottom; 
    GXDips dt3 = rcx.Bottom() - m_dra.bottom;
    GXDips dt2 = rcx.Top() + m_dra.top;
    GXDips dh2 = dt3 - dt2;

    // calc source rect
    int sw1 = m_dra.left;
    int sh1 = m_dra.top;
    int sw3 = m_dra.right;
    int sh3 = m_dra.bottom;
    if (m_orgSize.Width())
    {
        sw1 = (float)sw1 * (float)wSrc / (float)m_orgSize.Width();
        sw3 = (float)sw3 * (float)wSrc / (float)m_orgSize.Width();
    }
    if (m_orgSize.Height())
    {
        sh1 = (float)sh1 * (float)h / (float)m_orgSize.Height();
        sh3 = (float)sh3 * (float)h / (float)m_orgSize.Height();
    }
    int sl1 = rcs.Left();
    int sl3 = rcs.Right() - sw3;
    int sl2 = rcs.Left() + sw1;
    int sw2 = sl3 - sl2;

    int st1 = rcs.Top();
    int st3 = rcs.Bottom() - sh3;
    int st2 = rcs.Top() + sh1;
    int sh2 = st3 - st2;

    
    CRectL rcDest[10];
    CRectD rcSrc[10];
    int n = 0;
#   define _SBLT(mask, dx, dy, dw, dh, sx, sy, sw, sh) \
if ((m_nFlag & mask) && dw && dh) rcDest[n].SetLTWH(dx,dy,dw,dh), rcSrc[n].SetLTWH(sx,sy,sw,sh), ++n


    _SBLT(CBltSharp::e_lt, dl1, dt1, dw1, dh1,  sl1, st1, sw1, sh1); // left-top
    _SBLT(CBltSharp::e_lb, dl1, dt3, dw1, dh3,  sl1, st3, sw1, sh3); // left-bottom
    _SBLT(CBltSharp::e_rt, dl3, dt1, dw3, dh1,  sl3, st1, sw3, sh1); // right-top
    _SBLT(CBltSharp::e_rb, dl3, dt3, dw3, dh3,  sl3, st3, sw3, sh3); // right-bottom
    
    _SBLT(CBltSharp::e_wt, dl2, dt1, dw2, dh1,  sl2, st1, sw2, sh1); // top line
    _SBLT(CBltSharp::e_wb, dl2, dt3, dw2, dh3,  sl2, st3, sw2, sh3); // bottom line
    
    _SBLT(CBltSharp::e_lh, dl1, dt2, dw1, dh2,  sl1, st2, sw1, sh2); // left line
    _SBLT(CBltSharp::e_rh, dl3, dt2, dw3, dh2,  sl3, st2, sw3, sh2); // right line
    
    _SBLT(CBltSharp::e_wh, dl2, dt2, dw2, dh2,  sl2, st2, sw2, sh2); // center

#   undef _SBLT

    EGXBlt flg = eBlt | GetFlag();

    rcDest[n] = rcx;
    txt->Blts(rcDest, rcSrc, n, flg, nAlpha);
}

int CPicBmp::EditorRender(IGXApp* gapp, int nState, const CTuiRect& rc)
{
    Reload();
    if (!m_txt)
        return 0;

    if (e_bsEditor == nState)
    {
        int w = m_txt->GetWidth();
        int h = m_txt->GetHeight();
        m_txt->Blt(rc, CRect(0,0,w,h), GetFlag(), m_alpha);
        
        int hh = rc.Width();
        for (int i = 1; i < m_nStates; i++)
        {
            int n = rc.Left() + CMath::MulDiv (hh,  i, m_nStates);
            Line(gapp, n, rc.Top(), n, rc.Bottom(), TMRGB(255,255,255));
        }
        
        int n = rc.Left() + CMath::MulDiv (m_dra.left, hh, w);
        Line(gapp, n, rc.Top(), n, rc.Bottom(), TMRGB(255,255,0));
        
        n = rc.Left() + hh / m_nStates - CMath::MulDiv (m_dra.right, hh, w);
        Line(gapp, n, rc.Top(), n, rc.Bottom(), TMRGB(255,255,0));
        
        int r = rc.Right();//rc.Left() + CMath::MulDiv (m_dra.GetWidth(0), hh, w);
        hh = rc.Bottom() - rc.Top();
        n = rc.Top() + CMath::MulDiv (m_dra.top, hh, h);
        Line(gapp, rc.Left(), n, r, n, TMRGB(255,255,0));
        
        n = rc.Bottom() - CMath::MulDiv (m_dra.bottom, hh, h);
        Line(gapp, rc.Left(), n, r, n, TMRGB(255,255,0));

        return GetCurrentFrame();
    }
    else
    {
        SetCurFrame(nState);
        RenderNative(gapp, nState / 10, m_txt, rc, 0);
        int n = GetCurrentFrame();
        SetCurFrame(0);
        return n;
    }

}

// struct TextDrawMakeMono
// {
//     DibARGB argb;
//     
//     TextDrawMakeMono(TMColor clr) { argb.SetColor(clr); }
//     
//     void operator() (DibARGB& pixel) const
//     {
//         TMUInt8 a = pixel.a;
//         pixel.g = argb.g * a / 255;
//         pixel.b = argb.b * a / 255;
//         pixel.r = argb.r * a / 255;
//     }
// };

void CPicBmp::RenderNative(IGXApp* gapp, int nFrame, CTexture& tex, const CTuiRect& rc, WPRENDERFX* fx)
{
    DbgAssert(tex);

    if (nFrame == e_bsEditor && Edit_IsObjectInSpyOrDocMode(this))
    {
        int w = tex->GetWidth();
        int h = tex->GetHeight();
        tex->Blt(rc, CRect(0,0,w,h), GetFlag(), m_alpha);

        int hh = rc.Right() - rc.Left();
        for (int i = 1; i < m_nStates; i++)
        {
            int n = rc.Left() + CMath::MulDiv (hh,  i, m_nStates);
            Line(gapp, n, rc.Top(), n, rc.Bottom(), TMRGB(255,255,255));
        }

        int n = rc.Left() + CMath::MulDiv (m_dra.left, hh, w);
        Line(gapp, n, rc.Top(), n, rc.Bottom(), TMRGB(255,255,0));

        n = rc.Left() + hh / m_nStates - CMath::MulDiv (m_dra.right, hh, w);
        Line(gapp, n, rc.Top(), n, rc.Bottom(), TMRGB(255,255,0));

        int r = rc.Right();//rc.Left() + CMath::MulDiv (m_dra.GetWidth(0), hh, w);
        hh = rc.Bottom() - rc.Top();
        n = rc.Top() + CMath::MulDiv (m_dra.top, hh, h);
        Line(gapp, rc.Left(), n, r, n, TMRGB(255,255,0));

        n = rc.Bottom() - CMath::MulDiv (m_dra.bottom, hh, h);
        Line(gapp, rc.Left(), n, r, n, TMRGB(255,255,0));
    }
    else
    {
        int nOldFrame = nFrame;
        if (IsFlagSimpleFrame())
            nFrame /= e_bsEnd;
        else if (nFrame >= m_nStates)
            nFrame %= m_nStates;

        int wSrc = tex->GetWidth();
        int w = wSrc / m_nStates;
        int h = tex->GetHeight();
        if (fx)
        {
            int ns = ButStates2ButFrames(fx->nOldState);
            if (IsFlagSimpleFrame())
                ns /= e_bsEnd;
            else if (ns >= m_nStates)
                ns %= m_nStates;
            if (ns != nFrame)
            {
                DbgAssert(tex->GetFormat() == eGXFormat32A);
                CTexture texTmp;
                texTmp.CreateRenderTarget(w, h, eGXFormat32A, gapp);
                if (!texTmp)
                    return;
                {
                    CAutoTextureRender render(texTmp, TM_DPI_SYSTEM_DEFAULT, 0, gapp);
                    gapp->Clear(TMRGBA(0,0,0,0));
                    GXRect rcD(0,0,w,h);
                    gapp->ClipAndPush(rcD);
                    CRect rc1, rc2;
                    rc1.SetLTWH(w * ns, 0, w, h);
                    rc2.SetLTWH(w * nFrame, 0, w, h);
                    tex->Blt(rcD, rc1, GX_BLT_NONE, fx->nMixAlpha);
                    tex->Blt(rcD, rc2, GX_BLT_NONE, 255 - fx->nMixAlpha);
                }
                RenderNative(gapp, 0, texTmp, rc, GX_BLT_NONE, w, h, wSrc, m_alpha);
                return;
            }
        }

        EGXBlt eBlt = GX_BLT_NONE;
        if (IsFlagMonoOnHC() && (GetModule()->GetVisual() & eColorVisualHighContrast))
            eBlt = GX_BLT_TEXTMONO;

        RenderNative(gapp, nFrame, tex, rc, eBlt, w, h, wSrc, m_alpha);
    }
}

// void CPicBmp::ChangeUserInterface(EUserInterface eOld, EUserInterface eNew)
// {
//     if (IsFlagSupportDark() && (eOld == eUserInterfaceColorTransform || eNew == eUserInterfaceColorTransform))
//     {
//         m_txt.Release();
//         m_image.Release();
//     }
// }


TM_REGISTER_CLASS4(CPicBmp, TM_CS(TUI_CLASS_PIC_BMP), true, false);


////////////////////////////////////////////////////////////////////////////////
//CPicAni
TMUInt CPicAni::GetFrames()
{
    DbgAssert(m_nStates >= 0);
    return m_nStates;
}

int CPicAni::GetDelay()
{
    DbgAssert(m_nDelay > 0);
    return m_nDelay;
}

TMUInt CPicAni::GetCurrentFrame()
{
    return m_nCur;
}

void CPicAni::SetCurFrame(TMUInt n)
{
    m_nCur = (n % GetFrames());
}

void CPicAni::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    nState = nState < 0 ? nState : m_nCur;
    CPicBmp::Render(nState, 0, rc, 0, gapp);
}

TM_REGISTER_CLASS4(CPicAni, TM_CS(TUI_CLASS_PIC_ANI), true, false);


////////////////////////////////////////////////////////////////////////////////
//CPicTile

// IImageDpiDecide
GXDips CPicTile::OnImageDecideSize(CSize& sz)
{
    return DoDecideSize(sz, 1);
}

void CPicTile::Reload()
{
    if (IsLoaded(m_txt))
        return;
    
    LoadTexture(m_txt, IsFlagColorKey());
    if (m_txt)
        m_txt->OptimizeForTile();
}

void CPicTile::OnLoadProp (IDomElement* pXml)
{
    OnLoad();
}

void CPicTile::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    Reload();
    if (!m_txt)
        return;
    m_txt->Blt(rc, CRect(0,0,0,0), GetFlag() | GX_BLT_TILE, m_alpha);
}

TM_REGISTER_CLASS4(CPicTile, TM_CS(TUI_CLASS_PIC_TILE), true, false);


////////////////////////////////////////////////////////////////////////////////
//CPicScroll
CPicScroll::CPicScroll ()
    : m_nButtonHeight (20)
    , m_nLineHeight (1)
    , m_nStates (4)
    , m_nThumbTop (5)
    , m_nThumbBottom (5)
    , m_nThumbCore (0)
{
}

void CPicScroll::Reload()
{
    if (IsLoaded(m_txt))
        return;

//        DbgAssert(m_pModule);
    LoadTexture(m_txt, 0);
    if (!m_txt)
        return;
}

void CPicScroll::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp)
{
    Reload();
    if (!m_txt)
        return;

//     const CTuiRect& rc = (const CTuiRect&)rcx;
//     CScrollFrame sf(nFrame);
// 
//     if (sf.nsState == e_bsFocus && m_nStates <= e_bsFocus)
//         sf.nsState = e_bsNormal;

    if (nState == e_bsEditor && Edit_IsObjectInSpyOrDocMode(this))
        RenderEditor(gapp, m_txt, rc);
//     else
//     if (sf.nsBar == SB_HORZ)
//         RenderH(sf, m_txth, rc);
//     else
//         RenderV(sf, m_txt, rc);
}

// IImageDpiDecide
GXDips CPicScroll::OnImageDecideSize(CSize& sz)
{
    return DoDecideSize(sz, m_nStates);
}

void CPicScroll::OnLoadProp (IDomElement* pXml)
{
    if (m_nStates <= 0)
        m_nStates = 1;
    OnLoad();
}

void CPicScroll::RenderEditor(IGXApp* gapp, GX::CTexture& image, const CTuiRect& rc)
{
    DbgAssert(image);
    if (Edit_IsObjectInSpyOrDocMode(this))
    {
        int w = image->GetWidth();
        int h = image->GetHeight();
        image->Blt(rc, CRect(0,0,w,h), 0, m_alpha);

        struct SLine
        {
            int ll, tt;
            int w1, w2;
            int h1, h2;
            
            void HLine(IGXApp* gapp, int t, TMColor clr)
            {
                t = tt + CMath::MulDiv(h1, t, h2);
                Line(gapp, ll, t, ll + w1, t, clr);
            }
            
            void VLine(IGXApp* gapp, int l, TMColor clr)
            {
                l = ll + CMath::MulDiv(w1, l, w2);
                Line(gapp, l, tt, l, tt + h1, clr);
            }
        };
        SLine sl = { (int)rc.Left(), (int)rc.Top(), (int)rc.Width(), w, (int)rc.Height(), h };

        int n;
        n = m_nButtonHeight + m_nLineHeight;
        sl.HLine(gapp, n, TMRGB(255,255,0));
        
        n = m_nButtonHeight + m_nLineHeight + m_nThumbTop;
        sl.HLine(gapp, n, TMRGB(255,255,0));
        
        n = h - m_nButtonHeight - m_nThumbBottom;
        sl.HLine(gapp, n, TMRGB(255,255,0));

        if (m_nThumbCore)
        {
            n = h - m_nButtonHeight - m_nThumbBottom - m_nThumbCore;
            sl.HLine(gapp, n, TMRGB(0,255,255));
        }

        n = m_nButtonHeight;
        sl.HLine(gapp, n, TMRGB(255,255,255));

        n = h - m_nButtonHeight;
        sl.HLine(gapp, n, TMRGB(255,255,255));
        
        for (int i = 1; i < m_nStates; i++)
        {
            int n2 = CMath::MulDiv (w,  i, m_nStates);
            sl.VLine(gapp, n2, TMRGB(255,255,255));
        }
    }
}

void CPicScroll::RenderNative(CTexture& tex, const SCROLLSTATE& state, CTuiRect rc, int nAlpha, int* ss, int wFrm)
{
    // source left position
    // n:normal; d:down;
//     int sn1 = st[0];
//     int sn2 = st[1];
//     int sd1 = st[2];
//     int sd2 = st[3];
    
    // source top position
    int st0 = 0,        sb0 = ss[0];    // up button
    int st1 = ss[0],    sb1 = ss[1];    // up scroll line
    int st2 = ss[1],    sb2 = ss[2];    // thumb top
    int st3 = ss[2],    sb3 = ss[3];    // thumb top line
    int st4 = ss[3],    sb4 = ss[4];    // thumb core
    int st5 = st3,      sb5 = sb3;      // thumb bottom line
    int st6 = ss[4],    sb6 = ss[5];    // thumb bottom
    int st7 = st1,      sb7 = sb1;      // down scroll line
    int st8 = ss[5],    sb8 = ss[6];    // down button

    // rotate
    TMUInt32 flg = GetFlag();
    CRectL rcDest[10];
    CRectD rcSrc[10];
    if (state.obj != e_objVScroll)
    {
        rc.Rotate();
        flg |= GX_BLT_X2Y;
    }
    
    // dest position
    GXDips sc = m_nThumbCore + m_nThumbBottom + m_nThumbTop;
    GXDips dt[10];
    dt[0] = 0;                                                          // up button
    dt[1] = state.nButtonHeight;                                        // up scroll line
    dt[2] = state.nThumbTop;                                            // thumb top
    dt[3] = dt[2] + m_nThumbTop;                                        // thumb top line
    dt[4] = dt[3] + (state.nThumbBottom - state.nThumbTop - sc) / 2;    // thumb core
    dt[5] = dt[4] + m_nThumbCore;                                       // thumb bottom line
    dt[6] = state.nThumbBottom - m_nThumbBottom;                        // thumb bottom
    dt[7] = state.nThumbBottom;                                         // down scroll line
    dt[9] = rc.Height();// : rc.Width();
    dt[8] = dt[9] - state.nButtonHeight;                                // down button
    
    
// #   define _SBLT_V(n,c) \
//     rcDest[n].SetRect(rc.Left(), rc.Top() + dt[n], rc.Right(), rc.Top() + dt[n + 1]); \
//     if (c != hid.nsPos) rcSrc[n].SetRect(sn1, st##n, sn2, sb##n); else rcSrc[n].SetRect(sd1, st##n, sd2, sb##n)
    int nIndex = 0;
#   define _SBLT_V(n,c) \
    if (!(state.eState[c] & e_bssHide)) \
    { \
        rcDest[nIndex].SetRect(rc.Left(), rc.Top() + dt[n], rc.Right(), rc.Top() + dt[n + 1]); \
        int nn = ButStates2ButFrame(state.eState[c]); \
        rcSrc[nIndex].SetRect(nn * wFrm, st##n, nn * wFrm + wFrm, sb##n); \
        ++nIndex; \
    }

    _SBLT_V(0, SB_LINEUP);
    _SBLT_V(1, SB_PAGEUP);
    _SBLT_V(2, SB_THUMBPOSITION);
    _SBLT_V(3, SB_THUMBPOSITION);
    _SBLT_V(4, SB_THUMBPOSITION);
    _SBLT_V(5, SB_THUMBPOSITION);
    _SBLT_V(6, SB_THUMBPOSITION);
    _SBLT_V(7, SB_PAGEDOWN);
    _SBLT_V(8, SB_LINEDOWN);
    rcDest[nIndex] = rc;
    tex->Blts(rcDest, rcSrc, nIndex, flg, nAlpha);
    
#undef _SBLT_V
#undef _SBLT_H
}

void CPicScroll::RenderSB(const SCROLLSTATE& state, const CTuiRect& rcx, WPRENDERFX_SB* fx, IGXApp* gapp)
{
    Reload();
    if (!m_txt)
        return;


//    DbgTrace("CPicScroll::Render(state:%d, hitobj:%d, hitpos:%d)", state.eState, state.hid.nObjID, state.hid.nsPos);

    int wSrc = m_txt->GetWidth();
    int hSrc = m_txt->GetHeight();
    int wFrm = wSrc / m_nStates;

    // source position (pixel)
#define SS0     (m_nButtonHeight)                                       // top of up scroll line 
#define SS1     (m_nButtonHeight + m_nLineHeight)                       // top of thumb top
#define SS2     (SS1 + m_nThumbTop)                                     // top of thumb line
#define SS3(x)  (x - m_nButtonHeight - m_nThumbCore - m_nThumbBottom)   // top of thumb core
#define SS4(x)  (SS3(x) + m_nThumbCore)                                 // top of thumb bottom
#define SS5(x)  (SS4(x) + m_nThumbBottom)                               // top of down button
#define SS6     hSrc                                                    // height of SB

    int ss[7];
    if (m_orgSize.Height()) // SVG file
    {
        ss[0] = (float)SS0 * (float)hSrc / (float)m_orgSize.Height() + 0.5f;
        ss[1] = (float)SS1 * (float)hSrc / (float)m_orgSize.Height() + 0.5f;
        ss[2] = (float)SS2 * (float)hSrc / (float)m_orgSize.Height() + 0.5f;
        ss[3] = (float)SS3(m_orgSize.Height()) * (float)hSrc / (float)m_orgSize.Height() + 0.5f;
        ss[4] = (float)SS4(m_orgSize.Height()) * (float)hSrc / (float)m_orgSize.Height() + 0.5f;
        ss[5] = (float)SS5(m_orgSize.Height()) * (float)hSrc / (float)m_orgSize.Height() + 0.5f;
        ss[6] = SS6;
    }
    else // PNG file
    {
        ss[0] = SS0;
        ss[1] = SS1;
        ss[2] = SS2;
        ss[3] = SS3(hSrc);
        ss[4] = SS4(hSrc);
        ss[5] = SS5(hSrc);
        ss[6] = SS6;
    }
#undef SS0
#undef SS1
#undef SS2
#undef SS3
#undef SS4
#undef SS5
#undef SS6

    // render
    int nAlpha = m_alpha;
    if (fx)
    {
        SCROLLSTATE stt(state);
        CTexture tex;
        tex.CreateRenderTarget(wFrm, hSrc, eGXFormat32A, gapp);
        if (!tex)
            return;
        {
            CAutoTextureRender render(tex, TM_DPI_SYSTEM_DEFAULT, 0, gapp);//, wFrm, hSrc);
            GXRect rcD(0,0,wFrm,hSrc);
            gapp->Clear(TMRGBA(0,0,0,0));
            gapp->ClipAndPush(rcD);

            // mix ani
            // SB_LINEUP / SB_LINEDOWN / SB_PAGEUP / SB_PAGEDOWN / SB_THUMBPOSITION
            int t[] = { 0, ss[5], ss[0], ss[0], ss[1] };
            int h[] = { ss[0], ss[6] - ss[5], ss[1] - ss[0], ss[1] - ss[0], ss[5] - ss[1]}; // (pixel)
            MemZeroT(stt.eState);
            for (int i = 0; i < 5; ++i)
            {
                rcD.SetLTWH(0, t[i], wFrm, h[i]);
                if (state.eState[i] & e_bssHide) // show -> hide
                {
                    if (fx->eState[i] & e_bssHide)
                        stt.eState[i] = e_bssHide;
                    else
                    {
                        CRect rc1;
                        rc1.SetLTWH(ButStates2ButFrame(fx->eState[i]) * wFrm, t[i], wFrm, h[i]);
                        m_txt->Blt(rcD, rc1, 0, fx->nAlpha[i]);
                    }
                }
                else if (fx->eState[i] & e_bssHide) // hide->show
                {
                    CRect rc1;
                    rc1.SetLTWH(ButStates2ButFrame(state.eState[i]) * wFrm, t[i], wFrm, h[i]);
                    m_txt->Blt(rcD, rc1, 0, 255 - fx->nAlpha[i]);
                }
                else
                {
                    DbgAssert(!(state.eState[i] & e_bssHide) && !(fx->eState[i] & e_bssHide));
                    CRect rc1, rc2;
                    rc1.SetLTWH(ButStates2ButFrame(fx->eState[i]) * wFrm, t[i], wFrm, h[i]);
                    rc2.SetLTWH(ButStates2ButFrame(state.eState[i]) * wFrm, t[i], wFrm, h[i]);
                    m_txt->Blt(rcD, rc1, 0, fx->nAlpha[i]);
                    m_txt->Blt(rcD, rc2, 0, 255 - fx->nAlpha[i]);
                }
            }
        }

        // render
        RenderNative(tex, stt, rcx, nAlpha, ss, wFrm);
        return;
    }

    RenderNative(m_txt, state, rcx, nAlpha, ss, wFrm);
}


TM_REGISTER_CLASS4(CPicScroll, TM_CS(TUI_CLASS_PIC_SCROLLBAR), true, false);



TM_END_NAMESPACE
