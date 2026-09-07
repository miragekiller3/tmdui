/*
 *  @file
 *  @brief
 *  GX helper
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.11.30
 *
 *  Copyright (C) 2018 miragekiller
 */

#pragma once

#ifndef GXHELPER_H
#define GXHELPER_H

#include "tmdef.h"

#define TYPEMODE_C_VERTEX
#include "hlsl\gxvs_i.h"


// default 3D setting:
// LIGHTING: close
// ZENABLE: close
// SCISSOR: enable
// Sampler State: POINT
// ALPHABLEND: false
// blend mode: SRC_ONE + DEST_INVSRCALPHA


TM_BGN_NAMESPACE

// #define FAILED_RETURN_HR(x)             DbgVerifyHResult(hr = (x)); if (FAILED(x)) return hr;
// #define FAILED_CHECK_RETURN_HR(app, x)  DbgVerifyHResult(hr = (x)); if (FAILED(x)) return app->CheckError(hr), hr;
#define FAILED_CHECK(app,ret,func)            DbgVerifyHResult(hr = (func)); if (FAILED(hr)) { app->CheckError(hr); return ret; }

enum { ePT_TRIANGLESTRIP, ePT_TRIANGLELIST };
// enum EGDIOpt { eGDIOptFill, eGDIOptInvert, eGDIOptBlt };

typedef CRectI Rect3D_D;
typedef CRectF Rect3D_L;
typedef CRectI Rect3D_Tex;
typedef CPointI Point3D_D;
typedef CPointF Point3D_L;

inline void LPToDP_3D(CZoom dpi, const Rect3D_L& src, Rect3D_D& dst)
{
    dpi.LPtoDP(src, dst);
}

inline void LPToDP_3D(CZoom dpi, const Point3D_L& src, Point3D_D& dst)
{
    dpi.LPtoDP(&src, 1, &dst);
}




template <class T, int n>
struct CVertexStrip
{
    typedef T   value_type;
    enum { ePT = ePT_TRIANGLESTRIP };
    T m_vertex[n];

    const T* buffer() const { return m_vertex; }
    int size() const { return n - 2; }
    int countVertex() const { return n; }
    int sizeBuffer() const { return sizeof(m_vertex); }

    // x0y0 -----> x1y0
    //         /
    //        /
    //       /
    //      /
    //     /
    // x0y2 -----> x1y2
    void InitRect(const Rect3D_D& rc)
    {
        DbgAssert(n == 4);
        m_vertex[0].InitXY(rc.left, rc.top);
        m_vertex[1].InitXY(rc.right, rc.top);
        m_vertex[2].InitXY(rc.left, rc.bottom);
        m_vertex[3].InitXY(rc.right, rc.bottom);
    }

    void InitRectX2Y(const Rect3D_D& rc)
    {
        DbgAssert(n == 4);
        m_vertex[0].InitXY(rc.top, rc.left);
        m_vertex[1].InitXY(rc.top, rc.right);
        m_vertex[2].InitXY(rc.bottom, rc.left);
        m_vertex[3].InitXY(rc.bottom, rc.right);
    }

    void InitRect(CZoom aff, const Rect3D_L& rc)
    {
        DbgAssert(n == 4);
        Rect3D_D dst;
        LPToDP_3D(aff, rc, dst);
        GX_TRACE("  CVertexStrip::InitRect(dpi:%d, rc:(" TM_FMT_RCI_STR "))", aff.m_nDPI, TM_FMT_RCI_VAL(dst));
        InitRect(dst);
    }

    void InitRectUV(float l, float t, float r, float b, float w, float h)
    {
        DbgAssert(n == 4);
        T::CalcUV(l, t, w, h);
        T::CalcUV(r, b, w, h);
        m_vertex[0].InitUV(l, t);
        m_vertex[1].InitUV(r, t);
        m_vertex[2].InitUV(l, b);
        m_vertex[3].InitUV(r, b);
    }

    void InitFrame(CZoom aff, const Rect3D_L& rc, GXDips width)
    {
        DbgAssert(n == 10);
        Rect3D_L rcInner(rc);
        rcInner.InflateRect(-width, -width);
        Rect3D_D rc1, rc2;
        LPToDP_3D(aff, rc, rc1);
        LPToDP_3D(aff, rcInner, rc2);

        m_vertex[0].InitXY(rc1.left, rc1.top);             //0        0---------------2
        m_vertex[1].InitXY(rc2.left, rc2.top);             //1        |               |
        m_vertex[2].InitXY(rc1.right, rc1.top);            //2        |----1-----3----|
        m_vertex[3].InitXY(rc2.right, rc2.top);            //3        |    |     |    |
        m_vertex[4].InitXY(rc1.right, rc1.bottom);         //4        |----7-----5----|
        m_vertex[5].InitXY(rc2.right, rc2.bottom);         //5        |               |
        m_vertex[6].InitXY(rc1.left, rc1.bottom);          //6        6---------------4
        m_vertex[7].InitXY(rc2.left, rc2.bottom);          //7
        m_vertex[8].InitXY(rc1.left, rc1.top);             //8 == 0
        m_vertex[9].InitXY(rc2.left, rc2.top);             //9 == 1
    }

    void InitColor(TMColor clr)
    {
        typename T::color_type c = T::ToColor(clr);
        for (int i = 0; i < n; ++i)
            m_vertex[i].InitClr(c);
    }
};

template <class T>
struct CVertexList
{
    typedef T value_type;
    enum { ePT = ePT_TRIANGLELIST };
    T* m_vertex;
    int m_nCount;

    CVertexList(int n)
    {
        m_nCount = n * 3;
        m_vertex = new T[m_nCount];
    }

    ~CVertexList()
    {
        delete[] m_vertex;
    }

    const T* buffer() const { return m_vertex; }
    void resize(int n) { DbgAssert(n <= m_nCount); m_nCount = n; }
    int size() const { return m_nCount / 3; }
    int countVertex() const { return m_nCount; }
    int sizeBuffer() const { return m_nCount * sizeof(T); }

    // x0y0 -----> x1y0         x1y0
    //         /               /   |
    //        /               /    |
    //       /               /     |
    //      /               /      |
    //     /               /       |
    // x0y2             x0y2      x1y2
    int AddRect(int nStart, const Rect3D_D& rc)
    {
        m_vertex[nStart + 0].InitXY(rc.left, rc.top);
        m_vertex[nStart + 1].InitXY(rc.right, rc.top);
        m_vertex[nStart + 2].InitXY(rc.left, rc.bottom);
        m_vertex[nStart + 3].InitXY(rc.left, rc.bottom);
        m_vertex[nStart + 4].InitXY(rc.right, rc.top);
        m_vertex[nStart + 5].InitXY(rc.right, rc.bottom);
        nStart += 6;
        DbgAssert(nStart <= m_nCount);
        return nStart;
    }

    int AddRectX2Y(int nStart, const Rect3D_D& rc)
    {
        m_vertex[nStart + 0].InitXY(rc.top, rc.left);
        m_vertex[nStart + 1].InitXY(rc.top, rc.right);
        m_vertex[nStart + 2].InitXY(rc.bottom, rc.left);
        m_vertex[nStart + 3].InitXY(rc.bottom, rc.left);
        m_vertex[nStart + 4].InitXY(rc.top, rc.right);
        m_vertex[nStart + 5].InitXY(rc.bottom, rc.right);
        nStart += 6;
        DbgAssert(nStart <= m_nCount);
        return nStart;
    }

    int AddRect(int nStart, CZoom aff, const Rect3D_L& rc)
    {
        Rect3D_D dst;
        LPToDP_3D(aff, rc, dst);
        return AddRect(nStart, dst);
    }

    void AddRectUV(int nStart, const Rect3D_Tex& rc, float w, float h)
    {
        float l = rc.left;
        float t = rc.top;
        float r = rc.right;
        float b = rc.bottom;
        T::CalcUV(l, t, w, h);
        T::CalcUV(r, b, w, h);
        m_vertex[nStart + 0].InitUV(l, t);
        m_vertex[nStart + 1].InitUV(r, t);
        m_vertex[nStart + 2].InitUV(l, b);
        m_vertex[nStart + 3].InitUV(l, b);
        m_vertex[nStart + 4].InitUV(r, t);
        m_vertex[nStart + 5].InitUV(r, b);
    }

    void AddRects(CZoom aff, const Rect3D_L* rcDsts, const Rect3D_Tex* rcSrcs, int nCount, float w, float h)
    {
        int n = 0;
        for (int i = 0; i < nCount; ++i)
        {
            this->AddRectUV(n, rcSrcs[i], w, h);
            n = this->AddRect(n, aff, rcDsts[i]);
        }
    }

    void AddRectsFlag(CZoom aff, const Rect3D_L* rcDsts, const Rect3D_Tex* rcSrcs, int nCount, float w, float h, TMUInt32 uFlag)
    {
        int n = 0;
        GXDips tmpX = rcDsts[nCount].left + rcDsts[nCount].right;
        GXDips tmpY = rcDsts[nCount].top + rcDsts[nCount].bottom;
        for (int i = 0; i < nCount; ++i)
        {
            this->AddRectUV(n, rcSrcs[i], w, h);

            Rect3D_L rcTmp = rcDsts[i];
            if (uFlag & GX_BLT_R2L)
                rcTmp.left = tmpX - rcTmp.left, rcTmp.right = tmpX - rcTmp.right;
            if (uFlag & GX_BLT_B2T)
                rcTmp.top = tmpY - rcTmp.top, rcTmp.bottom = tmpY - rcTmp.bottom;
            Rect3D_D dst;
            LPToDP_3D(aff, rcTmp, dst);
            if (uFlag & GX_BLT_X2Y)
                n = this->AddRectX2Y(n, dst);
            else
                n = this->AddRect(n, dst);
        }
    }

    void InitColor(TMColor clr)
    {
        typename T::color_type c = T::ToColor(clr);
        for (int i = 0; i < m_nCount; ++i)
            m_vertex[i].InitClr(c);
    }
};


template <class T>
class CVertexListForIndT
{
public:
    enum { eSize = sizeof(T) };
    enum { ePT = ePT_TRIANGLELIST };

    typedef T value_type;
    typedef TM::CVector<T> CTVList;
    CTVList m_aTv;

    const T* buffer() const { return m_aTv.data(); }
    int countVertex() const { return m_aTv.size(); }
    int sizeBuffer() const { return countVertex() * sizeof(T); }

public:
    void Clear()
    {
        m_aTv.clear();
    }

    void Add(long x, long y, TMColor clr)
    {
        T f;
        f.InitXY(x, y);
        f.InitClr(T::ToColor(clr));
        m_aTv.push_back(f);
    }

    void Add(CZoom aff, float x, float y, TMColor clr)
    {
        Point3D_D pt;
        LPToDP_3D(aff, CPointF(x, y), pt);
        Add(pt.x, pt.y, clr);
    }
};


template <class T, class T_Vec>
class CTextureBltT
    : public CBaseRefT<CTextureBltT<T, T_Vec>, T>
    , public TM::CRefObjectRoot<1>
{
public:
    typedef typename T::CDrawStrip CDrawStrip;
    TM_CLASS_INSTANCE_COUNT(T);

#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        CTextureBltT* p = (CTextureBltT*)cls->m_pClass;
        if (pbuf)
        {
            CStringA str;
            str.Format("%d * %d * %d", p->GetWidth(), p->GetHeight(), p->GetFormat());
            str.CopyTo(pbuf);
        }
        return p->GetWidth() * p->GetHeight() * gxFormat2BPP(p->GetFormat()) / 8;
    }
#endif

private:
    //     virtual TMAtmInt GetRef() const tm_override
    //     {
    //         return this->m_nRef;
    //     }

    template <class _V>
    void DoBlt1(_V& vertexs, TMUInt32 uFlag, TMColor clr)
    {
        // d3d9: InitColor is unused function.
        // d3d11: InitColor is valid. 
        CDrawStrip ds(vertexs);
        if (uFlag & GX_BLT_SRCCOPY)
        {
//             vertexs.InitColor(0xffffffff);
            vertexs.InitColor(clr);
//             this->m_ptrDevice->BlendSetCopy(this, clr);
            DoBlt2(ds, uFlag);
//             this->m_ptrDevice->BlendResetCopy(this);
        }
        else
        {
            vertexs.InitColor(clr);
            this->m_ptrDevice->BlendSetOver();
            DoBlt2(ds, uFlag);
            this->m_ptrDevice->BlendResetOver();
        }
    }

    void DoBlt2(CDrawStrip& ds, TMUInt32 uFlag)
    {
        if (uFlag & GX_BLT_LINEAR)
        {
            this->m_ptrDevice->SamplersSetLinear();
            this->NativeBlt(ds);
            this->m_ptrDevice->SamplersResetLinear();
        }
        else
            this->NativeBlt(ds);
    }

    void _InitRectDP(const Rect3D_L* rcDst, Rect3D_Tex& rcSrc, TMUInt32 uFlag, Rect3D_D& ret)
    {
        DbgAssert(rcDst);
        LPToDP_3D(this->m_ptrDevice->GetDPI(), *rcDst, ret);
        if (uFlag & GX_BLT_DFTDSTWIDTH)
            ret.SetWidth(this->GetWidth());
        if (uFlag & GX_BLT_DFTDSTHEIGHT)
            ret.SetHeight(this->GetHeight());
        if (uFlag & GX_BLT_DFTSRCWIDTH)
            rcSrc.SetWidth(this->GetWidth());
        if (uFlag & GX_BLT_DFTSRCHEIGHT)
            rcSrc.SetHeight(this->GetHeight());
    }

    void _BltAsDevice(Rect3D_D& dst, const Rect3D_Tex* src, TMUInt32 uFlag, TMColor clr)
    {
        DbgAssert(src);
        CVertexStrip<T_Vec, 4> vertexs;
        if (uFlag & GX_BLT_R2L)
            tm_swap(dst.left, dst.right);
        if (uFlag & GX_BLT_B2T)
            tm_swap(dst.top, dst.bottom);
        if (uFlag & GX_BLT_X2Y)
            vertexs.InitRectX2Y(dst);
        else
            vertexs.InitRect(dst);
        vertexs.InitRectUV(src->Left(), src->Top(), src->Right(), src->Bottom(), this->GetWidth(), this->GetHeight());
        DoBlt1(vertexs, uFlag, clr);
    }

public:
    virtual void Blt(const GXRect& rcDst, const TMRectD& _rcSrc, TMUInt32 uFlag, TMUInt8 alpha) tm_override
    {
        this->m_ptrDevice->LockDevice();
        this->m_ptrDevice->_gx_check();
        TMColor clr = this->Check_GX_BLT_TEXTMONO(uFlag, alpha, this->m_ptrDevice);
        Rect3D_D dst;
        if (uFlag & GX_BLT_TILE)
        {
            LPToDP_3D(this->m_ptrDevice->GetDPI(), rcDst, dst);
            Rect3D_Tex rcSrc(0, 0, rcDst.Width(), rcDst.Height());
            _BltAsDevice(dst, &rcSrc, uFlag, clr);
        }
        else
        {
            Rect3D_Tex rcSrc(_rcSrc);
            _InitRectDP(&rcDst, rcSrc, uFlag, dst);
            _BltAsDevice(dst, &rcSrc, uFlag, clr);
        }
        this->m_ptrDevice->UnlockDevice();
    }

    virtual void Blts(const GXRect* rcDsts, const TMRectD* rcSrcs, int nCount, TMUInt32 uFlag, TMUInt8 alpha) tm_override
    {
        if (nCount <= 0)
            return;

        this->m_ptrDevice->LockDevice();
        this->m_ptrDevice->_gx_check();
        TMColor clr = this->Check_GX_BLT_TEXTMONO(uFlag, alpha, this->m_ptrDevice);
        CVertexList<T_Vec> vertex(nCount * 2);
        float w = this->GetWidth();
        float h = this->GetHeight();
        if (uFlag & (GX_BLT_R2L | GX_BLT_X2Y | GX_BLT_B2T))
            vertex.AddRectsFlag(this->m_ptrDevice->GetDPI(), rcDsts, static_cast<const Rect3D_Tex*>(rcSrcs), nCount, w, h, uFlag);
        else
            vertex.AddRects(this->m_ptrDevice->GetDPI(), rcDsts, static_cast<const Rect3D_Tex*>(rcSrcs), nCount, w, h);
        DoBlt1(vertex, uFlag, clr);
        this->m_ptrDevice->UnlockDevice();
    }

    virtual void BltText(const GXRect& rcDst, TMUInt32 uFlag, TMColor clr) tm_override
    {
        this->m_ptrDevice->LockDevice();
        this->m_ptrDevice->_gx_check();
        Rect3D_D dst;
        Rect3D_Tex rcSrc(0, 0, this->GetWidth(), this->GetHeight());
        _InitRectDP(&rcDst, rcSrc, uFlag, dst);
        _BltAsDevice(dst, &rcSrc, uFlag, clr);
        this->m_ptrDevice->UnlockDevice();
    }

public:
    static T* Inst()
    {
        return new CTextureBltT();
    }
};

template <class T> class CGXApp3DT : public T
{
public:
    template <class T_List> void _DrawStrip(T_List& v)
    {
        typename T::CDrawStrip ds(v);
        this->DrawStrip(ds);
    }

    void _FillRect(const Rect3D_L* rc, TMColor clr)
    {
        DbgAssert(rc);
        this->_gx_check();
        CVertexStrip<T::CVertex_FillRect, 4> vertexs;
        vertexs.InitRect(this->GetDPI(), *rc);
        vertexs.InitColor(clr);
        this->_DrawStrip(vertexs);
    }

public:
    virtual void FillRect(const GXRect& rc, TMColor clr) tm_override
    {
        GX_TRACE("CGXApp3DT::FillRect(%f, %f, %f, %f, 0x%x)", rc.Left(), rc.Top(), rc.Right(), rc.Bottom(), clr);
        this->LockDevice();
//         this->BlendSetCopy();
        _FillRect(&rc, clr);
//         this->BlendResetCopy();
        this->UnlockDevice();
    }

    virtual void InvertRect(const GXRect& rc) tm_override
    {
        GX_TRACE("CGXApp3DT::InvertRect(%f, %f, %f, %f)", rc.Left(), rc.Top(), rc.Right(), rc.Bottom());
        this->LockDevice();
        this->BlendSetInvert();
        _FillRect(&rc, TMRGBA(255, 255, 255, 255));
        this->BlendResetInvert();
        this->UnlockDevice();
    }

    virtual void FrameRect(const GXRect& rc, GXDips width, TMColor clr) tm_override
    {
        GX_TRACE("CGXApp3DT::FrameRect(%f, %f, %f, %f, %f, 0x%x)", rc.Left(), rc.Top(), rc.Right(), rc.Bottom(), width, clr);
        this->_gx_check();
        this->LockDevice();
//         this->BlendSetCopy(clr);

        CVertexStrip<T::CVertex_FrameRect, 10> vertexs;
        vertexs.InitFrame(this->GetDPI(), rc, width);
        vertexs.InitColor(clr);
        this->_DrawStrip(vertexs);

//         this->BlendResetCopy();
        this->UnlockDevice();
    }

    virtual void DrawFocusRect(const GXRect& rc) tm_override
    {
        GX_TRACE("CGXApp3DT::DrawFocusRect(%f, %f, %f, %f)", rc.Left(), rc.Top(), rc.Right(), rc.Bottom());
        this->_gx_check();

        this->LockDevice();
        this->BlendSetInvert();

        CZoom aff = this->GetDPI();
        Rect3D_D dst;
        LPToDP_3D(aff, rc, dst);
        CVertexList<T::CVertex_DrawFocusRect> vertexs(4 * (int)(dst.Height() / 2 + 1) + 4 * (int)(dst.Width() / 2 + 1));
        int n = 0;
        for (Rect3D_D::value_type t = dst.top + 2; t < dst.bottom - 2; t += 2)
        {
            n = vertexs.AddRect(n, Rect3D_D(dst.left, t, dst.left + 1, t + 1));        // left
            n = vertexs.AddRect(n, Rect3D_D(dst.right - 1, t, dst.right, t + 1));      // right
        }
        for (Rect3D_D::value_type l = dst.left; l < dst.right; l += 2)
        {
            n = vertexs.AddRect(n, Rect3D_D(l, dst.top, l + 1, dst.top + 1));          // top
            n = vertexs.AddRect(n, Rect3D_D(l, dst.bottom - 1, l + 1, dst.bottom));    // bottom
        }
        vertexs.resize(n);
        this->_DrawStrip(vertexs);

        this->BlendResetInvert();
        this->UnlockDevice();
    }

    virtual void VertexBegin() tm_override
    {
        this->m_vertex.Clear();
    }

    virtual void VertexAddColor(GXDips x, GXDips y, TMColor clr) tm_override
    {
        this->m_vertex.Add(this->GetDPI(), x, y, clr);
    }
};


struct CNV12CopyBase
{
    virtual TMDibBuff LockRect(void* surface) = 0;
    virtual void UnlockRect(void* surface) = 0;
    virtual void ReleaseA() = 0;
    virtual void* RecreateA(int w, int h) = 0;

    virtual void CopyData(void* pTex, int height, TMUInt8* plane, int stride)
    {
        TMDibBuff dst(LockRect(pTex));
        if (!dst)
            return;
        dst.m_height = height;
        TMDibBuff src(plane, stride, height);
        DibBltRef(dst, src);
        UnlockRect(pTex);
    }

    virtual void CopyDataA(void* pTex, int width, int height, const CDibHandleGray& alpha)
    {
        CTrueDib<CDibHandleGray> dst(LockRect(pTex));
        if (!dst)
            return;
        dst.m_height = height;
        CTrueDib<CDibHandleGray> src(alpha);
        dst.SetWidth(width);
        src.SetWidth(width);
        DibStretch(dst, src);
        UnlockRect(pTex);
    }

    TMBool ResetAsDib(const GXDibData& data, int Width, int Height, void* pY, void* pU)
    {
        GX_TRACE("CNV12CopyBase::ResetAsDib()");

        TMUInt8* plane = (TMUInt8*)data.bits;
        if (plane)
        {
            int wb = data.wb;
            if (!wb)
                wb = Width;
            CopyData(pY, Height, plane, wb);

            TMUInt8* plane1 = plane + Width * Height;
            CopyData(pU, Height / 2, plane1, wb);

            if (data.flag & data.eKeepAlpha)
                return true;
        }

        if (!data.alpha)
        {
            ReleaseA();
            return true;
        }

        Width = data.alpha.GetWidth();
        Height = data.alpha.GetHeight();
        void* pA = RecreateA(Width, Height);
        if (!pA)
            return true;
        CopyDataA(pA, Width, Height, data.alpha);
        return true;
    }
};

// T: CGXAppD3D11, CGXAppD3D9, CGXAppGL
template <class T, class T_D3D> struct CShaderImpl2
{
    typename T_D3D::T_VS    m_vs;
    typename T_D3D::T_PS    m_psNor; // normal
    typename T_D3D::T_PS    m_psTex;
    typename T_D3D::T_PS    m_pShaderPixel_NV12;
    typename T_D3D::T_PS    m_pShaderPixel_NV12A;

    ~CShaderImpl2()
    {
        T* pthis =  static_cast<T*>(this);
        m_vs.Release(pthis);
        m_psNor.Release(pthis);
        m_psTex.Release(pthis);
        m_pShaderPixel_NV12.Release(pthis);
        m_pShaderPixel_NV12A.Release(pthis);
//         pthis->GetVSCBufffer()->Release(pthis);
    }

    T* T_This() { return static_cast<T*>(this); }

    HRESULT ShaderInit(
        typename T_D3D::VS_DESC descVS,
        typename T_D3D::PS_DESC descPS,
        typename T_D3D::PS_DESC descPSTex,
        size_t szCBufferVS)
    {
        HRESULT hr;
        T* pthis =  static_cast<T*>(this);

        typename T_D3D::T_VSCBuffer* cb = pthis->GetVSCBufffer();
        hr |= cb->Create(pthis, szCBufferVS);

        hr =  m_vs   .Create(pthis, descVS);
        hr |= m_psNor.Create(pthis, descPS,    m_vs, descVS);
        hr |= m_psTex.Create(pthis, descPSTex, m_vs, descVS);
        m_vs.SetCurrent(pthis);
        m_psNor.SetCurrent(pthis);
        
        cb->SetCurrent(pthis);
        
//         m_pShaderPixel_NV12.Init();
//         m_pShaderPixel_NV12A.Init();

        pthis->BlendSetCopy();
        
        return hr;
    }
};



struct CD3dVertex : public VS_INPUT1
{
    typedef DibRGBAF color_type;
    void InitXY(float _x, float _y) { Pos.x = _x; Pos.y = _y;  } // Pos.z = 0;
    void InitUV(float u, float v) { Tex.x = u; Tex.y = v; }
    void InitClr(const color_type& clr) { Dif.x = clr.r; Dif.y = clr.g; Dif.z = clr.b; Dif.w = clr.a; }
    static color_type ToColor(TMColor clr) { color_type c; c.SetColor(clr); return c; }

    static void CalcUV(float& x, float& y, float w, float h)
    {
        x /= w;
        y /= h;
    }
};


TM_END_NAMESPACE



#endif //GXHELPER_H
