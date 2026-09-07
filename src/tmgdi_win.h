#pragma once

#ifndef WTGDI_H
#define WTGDI_H

#include "tmplatform_win.h"
#include "tmcolor.h"


#if TM_WIN_DESKTOP

#   include "tmatl.h"
//#   include <windows.h>

#pragma warning (push)

// class 'xxx' needs to have dll-interface to be used by clients of class 'yyy'
#pragma warning (disable : 4251)
// non dll-interface class 'xxx' used as base for dll-interface class 'yyy'
#pragma warning (disable : 4275)


TM_BGN_NAMESPACE


const TMUInt16 ropPattern   = 0xF0;
const TMUInt16 ropSource    = 0xCC;
const TMUInt16 ropDest      = 0xAA;
#define MAKEROP(x) ((TMUInt32)(x) << 16)


#ifdef CDCHandleAT
    typedef CDCHandleAT _inherit_cdc;
#else
        
    struct _inherit_cdc
    {
    public:
        HDC     m_hDC;
        _inherit_cdc(HDC hdc = 0) : m_hDC(hdc) {}
        operator HDC() const { return m_hDC; }
    };
#endif // CDCHandleAT

    
///////////////////////////////////////////////////////////////////////////////
// class CCompatibleDC
class CCompatibleDC : public _inherit_cdc
{
protected:
    HGDIOBJ m_hOldBmp;
    
public:
    CCompatibleDC(HBITMAP hBmp, HDC hdc = 0)
        : _inherit_cdc (_NS_SDK::CreateCompatibleDC(hdc))
        , m_hOldBmp (_NS_SDK::SelectObject(m_hDC, hBmp))
    {
        DbgAssert(hBmp && _NS_SDK::GetObjectType(hBmp) == OBJ_BITMAP);
        DbgAssert(m_hDC);
        DbgAssert(m_hOldBmp);
    }

    CCompatibleDC()
        : m_hOldBmp (0)
    {
    }

    void SelectIn(HBITMAP hBmp, HDC hdc = 0)
    {
        DbgAssert(!m_hDC);
        DbgAssert(!m_hOldBmp);
        DbgAssert(hBmp && _NS_SDK::GetObjectType(hBmp) == OBJ_BITMAP);

        m_hDC = _NS_SDK::CreateCompatibleDC(hdc);
        m_hOldBmp = _NS_SDK::SelectObject(m_hDC, hBmp);

        DbgAssert(m_hDC);
        DbgAssert(m_hOldBmp);
    }

    ~CCompatibleDC()
    {
        DbgAssert(m_hDC);
        DbgAssert(m_hOldBmp);
        _NS_SDK::SelectObject(m_hDC, m_hOldBmp);
        _NS_SDK::DeleteDC(m_hDC);
    }
};

///////////////////////////////////////////////////////////////////////////////
// class CGraphDC
class CGraphDC : public _inherit_cdc
{
protected:
    HGDIOBJ     m_hOldBmp;
    int         m_nWidth;
    int         m_nHeight;
    
public:
    CGraphDC()
        : m_hOldBmp(0)
        , m_nWidth(0)
        , m_nHeight(0)
    {
    }

    CGraphDC(HDC hdc, int w, int h)
        : m_hOldBmp(0)
    {
        Create(hdc, w, h);
    }

    CGraphDC(HDC hdc, int w, int h, int bpp, const void *lpvBits = 0, UINT cPlanes = 1)
        : m_hOldBmp (0)
    {
        Create(hdc, w, h, bpp, lpvBits, cPlanes);
    }

    ~CGraphDC()
    {
        Delete();
    }

    HBITMAP _CreateCompatibleBitmap(HDC hdc, int w, int h)
    {
        if (hdc)
            return _NS_SDK::CreateCompatibleBitmap(hdc, w, h);

        HDC hWin = ::GetDC(0);
        HBITMAP hRet = _NS_SDK::CreateCompatibleBitmap(hWin, w, h);
        ::ReleaseDC(0, hWin);
        return hRet;
    }

    void Delete()
    {
        if (m_hDC)
        {
            _NS_SDK::DeleteObject(_NS_SDK::SelectObject (m_hDC, m_hOldBmp));
            _NS_SDK::DeleteDC(m_hDC);
        }
        m_hDC = 0;
        m_hOldBmp = 0;
        m_nWidth = m_nHeight = 0;
    }

    HBITMAP Detach()
    {
        if (m_hDC)
        {
            HBITMAP hBmp = (HBITMAP)_NS_SDK::SelectObject(m_hDC, m_hOldBmp);
            _NS_SDK::DeleteDC (m_hDC);
            m_hDC = 0;
            m_hOldBmp = 0;
            return hBmp;
        }

        return 0;
    }

    HBITMAP GetBitmap()
    {
        return m_hDC ? (HBITMAP)_NS_SDK::GetCurrentObject(m_hDC, OBJ_BITMAP) : 0;
    }

    void Create(HDC hdc, int w, int h)
    {
        DbgAssert(!m_hDC);
        DbgAssert(!m_hOldBmp);
        
        HDC hDC = hdc;
        if (!hdc)
            hDC = _NS_SDK::GetDC(0);
        
        m_hDC = _NS_SDK::CreateCompatibleDC(hDC);
        DbgAssert(m_hDC);
        
        HBITMAP hBmp = _CreateCompatibleBitmap(hDC, w, h);
        DbgAssert(hBmp);

        m_hOldBmp = _NS_SDK::SelectObject(m_hDC,  hBmp);
        DbgAssert(m_hOldBmp);
        
        if (!hdc)
            _NS_SDK::ReleaseDC(0, hDC);

        m_nWidth = w;
        m_nHeight = h;
    }

    void ReCreate(HDC hdc, int w, int h)
    {
        if (m_hDC)
        {
            if (w != m_nWidth || h != m_nHeight)
            {
                HBITMAP hNew = _CreateCompatibleBitmap(hdc, w, h);
                DbgAssert(hNew);

                HGDIOBJ hOld = _NS_SDK::SelectObject(m_hDC, hNew);
                DbgAssert(hOld);
                _NS_SDK::DeleteObject(hOld);
                m_nWidth = w;
                m_nHeight = h;
            }
        }
        else
            Create(hdc, w, h);
    }

    // cBitsPerPel: 1, 2, 4, 8, 16, 24, 32
    void Create(HDC hdc, int w, int h, UINT cBitsPerPel, const void *lpvBits = 0, UINT cPlanes = 1)
    {
        DbgAssert(!m_hDC);
        DbgAssert(!m_hOldBmp);
        
        m_hDC = _NS_SDK::CreateCompatibleDC(hdc);
        m_hOldBmp = _NS_SDK::SelectObject(m_hDC, _NS_SDK::CreateBitmap (w, h, cPlanes, cBitsPerPel, lpvBits));
        
        DbgAssert(m_hDC);
        DbgAssert(m_hOldBmp);
    }

    int GetWidth() const { return m_nWidth; }
    int GetHeight() const { return m_nHeight; }
};

TM_END_NAMESPACE


// AlphaFormat: 0 or AC_SRC_ALPHA;
// alpha:       0-255;
inline void GDIAlphaBlend(HDC hdc,
    int xDst, int yDst, int wDst, int hDst, HDC hImg, 
    int xSrc, int ySrc, int wSrc, int hSrc, BYTE AlphaFormat, TMUInt8 alpha)
{
    DbgAssert(hImg);
    DbgAssert(hdc);
    DbgAssert(GetObjectType(hdc) == OBJ_DC || GetObjectType(hdc) == OBJ_MEMDC);
    DbgAssert(GetObjectType(hImg) == OBJ_DC || GetObjectType(hImg) == OBJ_MEMDC);

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = alpha;
    bf.AlphaFormat = AlphaFormat; //(hImg->dib.bpp == 32) ? 1 : 0;// AC_SRC_NO_PREMULT_ALPHA;
    
    DbgVerify(AlphaBlend(hdc, xDst, yDst, wDst, hDst, hImg, xSrc, ySrc, wSrc, hSrc, bf))
        ("(%x, %d, %d, %d, %d, %x, %d, %d, %d, %d)", hdc, xDst, yDst, wDst, hDst, hImg, xSrc, ySrc, wSrc, hSrc);
}

inline void GDIBlt(HDC hdc, 
    int xDst, int yDst, int wDst, int hDst, HDC hImg, 
    int xSrc, int ySrc, int wSrc, int hSrc, DWORD dwRop)
{
    DbgAssert(hImg);
    DbgAssert(hdc);
    //    dwRop |= NOMIRRORBITMAP;
    if (wDst == wSrc && hDst == hSrc)
        DbgVerify(::BitBlt(hdc, xDst, yDst, wDst, hDst, hImg, xSrc, ySrc, dwRop));
    else
        DbgVerify(::StretchBlt(hdc, xDst, yDst, wDst, hDst, hImg, xSrc, ySrc, wSrc, hSrc, dwRop));
}

// support RGBA
inline void GDIFillRect(TMHDC hdc, const RECT& rc, TMColor clr)
{
    COLOR16 rr = ColorR(clr) << 8;
    COLOR16 gg = ColorG(clr) << 8;
    COLOR16 bb = ColorB(clr) << 8;
    COLOR16 aa = ColorA(clr) << 8;
    
    TRIVERTEX vet[2] = { rc.left, rc.top, rr, gg, bb, aa, rc.right, rc.bottom, rr, gg, bb, aa };
    GRADIENT_RECT Index[] = { 0, 1 };
    DbgVerify(GradientFill(hdc, vet, 2, Index, 1, GRADIENT_FILL_RECT_H));
}

inline void GDIInvertRect(TMHDC hdc, const RECT& rc)
{
    int l = rc.left;
    int t = rc.top;
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;

    CDCHandle dc(hdc);
    static HBRUSH hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
    HBRUSH hOld = dc.SelectBrush(hbr);
    DbgVerify(dc.PatBlt(l, t, w, h, PATINVERT));
    DbgVerify(dc.SelectBrush(hOld));
}

inline void GDIFrameRect(TMHDC hdc, RECT& rcOutter, const RECT& rcInner, TMColor clr)
{
    int l = rcOutter.left;
    int t = rcOutter.top;
    int r = rcOutter.right;
    int b = rcOutter.bottom;

    int lll =  rcInner.left;
    int ttt =  rcInner.top;
    int r_rr = rcInner.right;
    int b_bb = rcInner.bottom;

    COLOR16 rrr = ColorR(clr) << 8;
    COLOR16 ggg = ColorG(clr) << 8;
    COLOR16 bbb = ColorB(clr) << 8;
    COLOR16 aaa = ColorA(clr) << 8;
    
    TRIVERTEX v[6] =
    {                                            //        0----------------
        l, t, rrr, ggg, bbb, aaa,                //        |               |
        r, ttt, rrr, ggg, bbb, aaa,              //        |----4----------1
        l, b_bb, rrr, ggg, bbb, aaa,             //        |    |     |    |
        r, b, rrr, ggg, bbb, aaa,                //        2----------5----|
        lll, ttt, rrr, ggg, bbb, aaa,            //        |               |
        r_rr, b_bb, rrr, ggg, bbb, aaa,          //        ----------------3
    };
    
    // GradientFill can't support index > 3.
    GRADIENT_RECT Index1[] = { 0, 1, 2, 3 };
    DbgVerify(GradientFill(hdc, v, 6, Index1, 2, GRADIENT_FILL_RECT_H));
    GRADIENT_RECT Index2[] = { 2, 4, 5, 1 };
    DbgVerify(GradientFill(hdc, v, 6, Index2, 2, GRADIENT_FILL_RECT_H));
}


// the function maybe send message "WM_DPICHANGED"
static inline TMBool UpdateLayeredWin(HWND hWnd, const POINT* ptDstScreen, const SIZE* szDst, HDC hdc, const POINT& ptOrg, TMUInt8 alpha, const RECT* rcDirty = 0)
{
    DbgAssert(::IsWindow(hWnd));
    DbgAssert(::GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_LAYERED);
    DbgAssert(!(::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD));
    DbgAssert(OBJ_MEMDC == GetObjectType(hdc));

// #ifdef TM_DBG
//     struct CPresentProcTest
//     {
//     private:
//         HWND m_hWnd;
//         LONG_PTR m_proc;
//         
//         static LRESULT CALLBACK _NullWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//         {
//             DbgAssert(0);
//             return 0;
//         }
//         
//     public:
//         CPresentProcTest(HWND hWnd)
//             : m_hWnd(hWnd)
//         {
//             m_proc = ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&_NullWndProc);
//         }
//         
//         ~CPresentProcTest()
//         {
//             LONG_PTR procold = ::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, m_proc);
//             DbgAssert(procold == (LONG_PTR)&_NullWndProc);
//         }
//     };
//     CPresentProcTest _test(hWnd);
// #endif // TM_DBG
//     

//     if (rcDirty)
//         DbgTrace(L"UpdateLayeredWindow rcDirty:%d,%d,%d,%d", *rcDirty);

    BLENDFUNCTION blendFunc = { 0 };
    blendFunc.AlphaFormat = AC_SRC_ALPHA ;
    blendFunc.BlendOp = AC_SRC_OVER;
    blendFunc.SourceConstantAlpha = alpha;
    TMBool b;
    TM::CGraphDC dcTmp;

    if (szDst && (ptOrg.x < 0 || ptOrg.y < 0))
    {
        dcTmp.Create(0, szDst->cx, szDst->cy, 32);
        ::BitBlt(dcTmp, -ptOrg.x, -ptOrg.y, szDst->cx, szDst->cy, hdc, 0, 0, SRCCOPY);
        hdc = dcTmp;
        ptDstScreen = 0;
        szDst = 0;
    }

    if (_UpdateLayeredWindowIndirect)
    {
        _UPDATELAYEREDWINDOWINFO ui = {
            sizeof(_UPDATELAYEREDWINDOWINFO),
            0,
            ptDstScreen,
            szDst,
            hdc,
            &ptOrg,
            0,
            &blendFunc,
            ULW_ALPHA,
            rcDirty
        };
        b = _UpdateLayeredWindowIndirect(hWnd, &ui);
    }
    else
        b = UpdateLayeredWindow(hWnd, 0, tm_const_cast(ptDstScreen), tm_const_cast(szDst), hdc, tm_const_cast(&ptOrg), 0, &blendFunc, ULW_ALPHA);

    DbgVerify(b)
        ("UpdateLayeredWindow failed: hwnd:%x, pt:%d,%d, sz:%d,%d, hdc:%x, org:%d,%d, alpha:%d, visible:%d",
        hWnd,
        ptDstScreen ? ptDstScreen->x : 0,
        ptDstScreen ? ptDstScreen->y : 0,
        szDst ? szDst->cx : 0,
        szDst ? szDst->cy : 0,
        hdc, ptOrg, alpha, ::IsWindowVisible(hWnd));
    return b;
}


#pragma warning (pop)

#endif // TM_WIN_DESKTOP

#endif // #ifndef WTGDI_H
