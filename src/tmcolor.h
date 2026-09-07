/*
 *  @file
 *  @brief  color support
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.23
 *
 *  Copyright (C) 2011 miragekiller
 */

#pragma once

#ifndef TMCOLOR_H
#define TMCOLOR_H

#include "tmdef.h"

#ifdef TM_QT
#   include <QApplication>
#   include <QPalette>
#   include <QToolTip>
#endif

// #ifndef TM_WIN
// typedef TMUInt32   COLORREF;
// #   define RGB(r,g,b)          ((COLORREF)(((TMUInt8)(r)|((TMUInt16)((TMUInt8)(g))<<8))|(((TMUInt32)(TMUInt8)(b))<<16)))
// #   define GetRValue(rgb)      ((TMUInt8)(rgb))
// #   define GetGValue(rgb)      ((TMUInt8)(((TMUInt16)(rgb)) >> 8))
// #   define GetBValue(rgb)      ((TMUInt8)((rgb)>>16))
// #endif // TM_WIN


// TMColor don't same as COLORREF!!!
// in windows platform, TMcolor is premultiplied BGRA, COLORREF is RGBX.
// in QT platform, TMColor same as QRgb.
typedef TMUInt32 TMColor;

const TMColor cColorShiftAlpha  = 24;
const TMColor cColorShiftRed    = 16;
const TMColor cColorShiftGreen  = 8;
const TMColor cColorShiftBlue   = 0;

const TMColor cColorMaskAlpha   = ((TMColor)0xff) << cColorShiftAlpha;
const TMColor cColorMaskRed     = ((TMColor)0xff) << cColorShiftRed;
const TMColor cColorMaskGreen   = ((TMColor)0xff) << cColorShiftGreen;
const TMColor cColorMaskBlue    = ((TMColor)0xff) << cColorShiftBlue;

enum EColorIndex
{                                                                                   // test on windows
    eColorIndexScrollBar = 0,               // COLOR_SCROLLBAR                      gray(200)
    eColorIndexBackground,                  // COLOR_BACKGROUND                     black
    eColorIndexActiveCaption,               // COLOR_ACTIVECAPTION              2   blue(153,180,209)
    eColorIndexInactiveCaption,             // COLOR_INACTIVECAPTION            3   blue(191,205,219)
    eColorIndexMenu,                        // COLOR_MENU                       4   gray(240)
    eColorIndexWindow,                      // COLOR_WINDOW                     5   white
    eColorIndexWindowFrame,                 // COLOR_WINDOWFRAME                6   black
    eColorIndexMenuText,                    // COLOR_MENUTEXT                   7   black
    eColorIndexWindowText,                  // COLOR_WINDOWTEXT                 8   black
    eColorIndexCaptionText,                 // COLOR_CAPTIONTEXT                9   black
    eColorIndexActiveBorder,                // COLOR_ACTIVEBORDER               10  gray(180)
    eColorIndexInactiveBorder,              // COLOR_INACTIVEBORDER             11  blue(244,247,252)
    eColorIndexAppWorkspace,                // COLOR_APPWORKSPACE               12  gray(171)
    eColorIndexHighlight,                   // COLOR_HIGHLIGHT                  13  blue(0,120,215)
    eColorIndexHighlightText,               // COLOR_HIGHLIGHTTEXT              14  white
    eColorIndexBtnFace,                     // COLOR_BTNFACE                    15  gray(240)
    eColorIndexBtnShadow,                   // COLOR_BTNSHADOW                  16  gray(160)
    eColorIndexGrayText,                    // COLOR_GRAYTEXT                   17  gray(109)
    eColorIndexBtnText,                     // COLOR_BTNTEXT                    18  black
    eColorIndexInactiveCationText,          // COLOR_INACTIVECAPTIONTEXT        19  black
    eColorIndexBtnHightlight,               // COLOR_BTNHIGHLIGHT               20  white
    eColorIndex3DDKShadow,                  // COLOR_3DDKSHADOW                 21  gray(105)
    eColorIndex3DLight,                     // COLOR_3DLIGHT                    22  gray(227)
    eColorIndexInfoText,                    // COLOR_INFOTEXT                   23  black
    eColorIndexInfoBK,                      // COLOR_INFOBK                     24  yellow(255,255,225)
    eColorIndexHotLight = 26,               // COLOR_HOTLIGHT                   26  blue(0,102,204)
    eColorIndexGradientActiveCaption,       // COLOR_GRADIENTACTIVECAPTION      27  blue(185,209,234)
    eColorIndexGradientInactiveCaption,     // COLOR_GRADIENTINACTIVECAPTION    28  blue(215,228,242)
    eColorIndexMenuLight,                   // COLOR_MENUHILIGHT                29
    eColorIndexMenuBar,                     // COLOR_MENUBAR                    30

    // https://www.w3.org/TR/css-color-4/#css-system-colors
    // W3C <system-color>                                                      edge                    dark edge
    eColorWebAccentColor                = eColorIndexWindow,                    // n/a                  n/a
    eColorWebAccentColorText            = eColorIndexWindowText,                // n/a                  n/a
    eColorWebActiveText                 = eColorIndexHotLight,                  // blue(38,103,200)     red(243,15,21)
    eColorWebButtonBorder               = eColorIndexWindowFrame,               // black                gray(157)
    eColorWebButtonFace                 = eColorIndexBtnFace,                   // white gray(240)      gray(157)
    eColorWebButtonText                 = eColorIndexBtnText,                   // black                white
    eColorWebCanvas                     = eColorIndexWindow,                    // white                white
    eColorWebCanvasText                 = eColorIndexWindowText,                // black                white
    eColorWebField                      = eColorIndexWindow,                    // white                gray(213)
    eColorWebFieldText                  = eColorIndexWindowText,                // black                white
    eColorWebGrayText                   = eColorIndexGrayText,                  // black gray(109)      gray(135)
    eColorWebHighlight                  = eColorIndexHighlightText,             // white                blue(144,147,205)
    eColorWebHighlightText              = eColorIndexHighlight,                 // blue(44,121,211)     white
    eColorWebLinkText                   = eColorWebActiveText,                  // blue(38,103,200)     pink(224,162,251)
    eColorWebMark                       = eColorIndexInfoBK,                    // yellow(254,254,65)   yellow(254,254,65)
    eColorWebMarkText                   = eColorIndexInfoText,                  // black                white
    eColorWebSelectedItem               = eColorIndexHotLight,                  // yellow(47,104,206)   blue(160.200.252)
    eColorWebSelectedItemText           = eColorIndexWindow,                    // white                gray(213)
    eColorWebVisitedText                = eColorWebActiveText,                  // blue(38,103,200)     pink(249,184,252)
};


// ((TMUInt32)(((TMUInt8)(b)|((TMUInt16)((TMUInt8)(g))<<8))|(((TMUInt32)(TMUInt8)(r))<<16))|((((TMUInt32)(TMUInt8)(a))<<24)))
#   define TMRGBA(r,g,b,a)      ((TMUInt32)(\
                                (((TMUInt8)(r)) << cColorShiftRed) |\
                                (((TMUInt8)(g)) << cColorShiftGreen) |\
                                (((TMUInt8)(b)) << cColorShiftBlue) |\
                                (((TMUInt8)(a)) << cColorShiftAlpha)))
#   define TMRGB(r,g,b)         TMRGBA(r,g,b,0xff)


const TMColor cColorInvalid     =  0;//TMRGB(0,0,0);
// const TMColor cColorDefault     =  TMRGBA(255,255,255,1);


inline TMUInt8 ColorR(TMColor clr) { return (TMUInt8)(clr >> cColorShiftRed); }
inline TMUInt8 ColorG(TMColor clr) { return (TMUInt8)(clr >> cColorShiftGreen); }
inline TMUInt8 ColorB(TMColor clr) { return (TMUInt8)(clr >> cColorShiftBlue); }
inline TMUInt8 ColorA(TMColor clr) { return (TMUInt8)(clr >> cColorShiftAlpha); }

inline float ColorRF(TMColor clr) { return (float)ColorR(clr) / 255.0f; }
inline float ColorGF(TMColor clr) { return (float)ColorG(clr) / 255.0f; }
inline float ColorBF(TMColor clr) { return (float)ColorB(clr) / 255.0f; }
inline float ColorAF(TMColor clr) { return (float)ColorA(clr) / 255.0f; }

inline void ColorSetR(TMColor& clr, TMUInt8 x) { clr &= ~cColorMaskRed; clr |= (((TMColor)x) << cColorShiftRed); }
inline void ColorSetG(TMColor& clr, TMUInt8 x) { clr &= ~cColorMaskGreen; clr |= (((TMColor)x) << cColorShiftGreen); }
inline void ColorSetB(TMColor& clr, TMUInt8 x) { clr &= ~cColorMaskBlue; clr |= (((TMColor)x) << cColorShiftBlue); }
inline void ColorSetA(TMColor& clr, TMUInt8 x) { clr &= ~cColorMaskAlpha; clr |= (((TMColor)x) << cColorShiftAlpha); }

inline TMColor ColorFromRGBA(TMUInt8 r, TMUInt8 g, TMUInt8 b, TMUInt8 a) { return TMRGBA(r, g, b, a); }
inline TMColor ColorFromRGB(TMUInt8 r, TMUInt8 g, TMUInt8 b) { return ColorFromRGBA(r, g, b, 0xff); }
inline TMColor ColorFromRGB_A(TMUInt8 r, TMUInt8 g, TMUInt8 b, TMUInt8 a) { return TMRGBA(r * a / 255, g * a / 255, b * a / 255, a); }
inline TMColor ColorFromRGB_A(TMColor clr) { return ColorFromRGB_A(ColorR(clr), ColorG(clr), ColorB(clr), ColorA(clr)); }

inline TMColor ColorRemoveA(TMColor x)
{
    TMUInt8 r = ColorR(x);
    TMUInt8 g = ColorG(x);
    TMUInt8 b = ColorB(x);
    return ColorFromRGB(r, g, b);
}

inline TMColor ColorToRGB_A(TMColor x)
{
    TMUInt8 a = ColorA(x);
    TMUInt8 r = ColorR(x);
    TMUInt8 g = ColorG(x);
    TMUInt8 b = ColorB(x);
    if (a == 0)
        return x;
    return ColorFromRGBA(r * 255 / a, g * 255 / a, b * 255 / a, a);
}

inline TMColor ColorFromSys(int nIndex);

struct CColorMatrix
{
    TMUInt8 m[3][3];

    CColorMatrix()
    {
        m[0][0] = m[1][1] = m[2][2] = 0xff;
        m[0][1] = m[0][2] = m[1][0] = m[1][2] = m[2][0] = m[2][1] = 0;
    }

    TMColor ConvertColor(TMColor clr)
    {
        TMUInt8 a = ColorA(clr);
        TMUInt8 r = ColorR(clr);
        TMUInt8 g = ColorG(clr);
        TMUInt8 b = ColorB(clr);
        int rr = (r * m[0][2] + g * m[1][2] + b * m[2][2]) >> 8;
        int gg = (r * m[0][1] + g * m[1][1] + b * m[2][1]) >> 8;
        int bb = (r * m[0][0] + g * m[1][0] + b * m[2][0]) >> 8;
        return TMRGBA(rr,gg,bb,a);
    }
};


TM_BGN_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// IColorTransform
class IColorTransform
{
public:
    virtual TMColor ConvertColor(TMLParam context, TMColor clr) = 0;
};

// support dark interface style
class CColorTransformDark : public IColorTransform
{
public:
    static TMColor ToDark(TMColor clr)
    {
        TMUInt8 r = ColorR(clr);
        TMUInt8 g = ColorG(clr);
        TMUInt8 b = ColorB(clr);
        TMUInt8 a = ColorA(clr);
        return ColorFromRGBA(a-r, a-g, a-b, a);
    }

    virtual TMColor ConvertColor(TMLParam context, TMColor clr) tm_override
    {
        return ToDark(clr);
    }
}; 

class CColorTransformPtr
{
public:
    IColorTransform* m_pct;
    
    CColorTransformPtr(IColorTransform* pct = 0) : m_pct(pct) {}
    
    TMColor Convert(TMLParam context, TMColor clr) const
    {
        if (m_pct)
            return m_pct->ConvertColor(context, clr);
        return clr;
    }
};


///////////////////////////////////////////////////////////////////////////////
// color for application
enum EColorVisual
{
    eColorVisualNormal              = 0x0,
    eColorVisualHighContrast        = 0x1,
    eColorVisualTransform           = 0x4,
};
TM_ENUM_2_FLAG(EColorVisual);

class CAppColor
{
public:
    TM_IF_WIN(DbgAssertS(eColorIndexGradientInactiveCaption == COLOR_GRADIENTINACTIVECAPTION));

    static TMColor ColorFromIndex(int index)
    {
        return ColorFromSys(index);
    }

    static int GetFrom(const int (&aList)[5], int state)
    {
        int nIndex = state % 5;
        return aList[nIndex];
    }
    
    static int GetFrom(const int (&aList)[10], int state)
    {
        int nIndex = state % 10;// > 10 ? 5 + state % 5 : state;
        return aList[nIndex];
    }

public:
    typedef TMColor (*PFNGetColor)(int state);

    static TMColor GetColorWindowBk(int state) // edit, listbox, tree
    {
        return ColorFromIndex(eColorIndexWindow);
    }

    static TMColor GetColorMenuBk(int state) // menu bar
    {
        static const int cg_clrMenuBk[] =
        {
            eColorIndexMenu, eColorIndexMenu, eColorIndexHighlight, eColorIndexMenu, eColorIndexHighlight,
        };
        return ColorFromIndex(GetFrom(cg_clrMenuBk, state));
    }

    static TMColor GetColorScrollBk(int state) // scroll bar
    {
        return ColorFromIndex(eColorIndexScrollBar);
    }

    static TMColor GetColorButtonBk(int state) // button, dialog
    {
        static const int cg_clrHCButtonBk[] =
        {
            eColorIndexBtnFace, eColorIndexBtnFace, eColorIndexBtnFace, eColorIndexBtnFace, eColorIndexBtnFace,
            eColorIndexBtnFace, eColorIndexBtnText, eColorIndexBtnFace, eColorIndexBtnFace, eColorIndexBtnFace
        };
        TMColor clr = ColorFromIndex(GetFrom(cg_clrHCButtonBk, state));
        return clr;
    }

    //Normal, Press, Hover, Disable, Focus
    static TMColor GetColorButtonText(int state)
    {
        static const int cg_clrHCButtonText[] = 
        {
            eColorIndexBtnText, eColorIndexBtnText, eColorIndexBtnText, eColorIndexGrayText, eColorIndexBtnText,
            eColorIndexBtnText, eColorIndexBtnText, eColorIndexBtnText, eColorIndexGrayText, eColorIndexBtnText,
        };
        return ColorFromIndex(GetFrom(cg_clrHCButtonText, state));
    }

    static TMColor GetColorCheckRadioText(int state)
    {
        static const int cg_clrHCCheckBoxText[] =
        {
            eColorIndexWindowText, eColorIndexWindowText, eColorIndexWindowText, eColorIndexGrayText, eColorIndexWindowText,
        };
        return ColorFromIndex(GetFrom(cg_clrHCCheckBoxText, state));
    }

    //normal, active
    static TMColor GetColorCaptionText(int state)
    {
        DbgAssert(state == 0 || state == 1);
        static const int cg_clrHCCaptionText[] = 
        {
            eColorIndexInactiveCationText, eColorIndexCaptionText,
        };
        return ColorFromIndex(cg_clrHCCaptionText[state]);
    }

    static TMColor GetColorCaptionButtonText(int state)
    {
        static const int cg_clrHCaptionButtonText[] =
        {
            eColorIndexBtnText, eColorIndexBtnFace, eColorIndexBtnFace, eColorIndexGrayText, eColorIndexBtnFace,
        };
        return ColorFromIndex(GetFrom(cg_clrHCaptionButtonText, state));
    }

    static TMColor GetColorStaticText(int state)
    {
        static const int cg_clrHCStaticText[] = 
        {
            eColorIndexWindowText, eColorIndexWindowText, eColorIndexWindowText, eColorIndexGrayText, eColorIndexWindowText,
        };
        return ColorFromIndex(GetFrom(cg_clrHCStaticText, state));
    }

    static TMColor GetColorHyperlinkText(int state)
    {
        static const int cg_clrHCHyperlinkText[] = 
        {
            eColorIndexHotLight, eColorIndexHotLight, eColorIndexHotLight, eColorIndexGrayText, eColorIndexHotLight,
        };
        return ColorFromIndex(GetFrom(cg_clrHCHyperlinkText, state));
    }

    static TMColor GetColorEditText(int state)
    {
        static const int cg_clrHCEditText[] = // Normal, Press, Hover, Disable, Focus
        {
            eColorIndexWindowText, eColorIndexWindowText, eColorIndexWindowText, eColorIndexGrayText, eColorIndexWindowText
        };
        return ColorFromIndex(GetFrom(cg_clrHCEditText, state));
    }

    static TMColor GetColorListTreeText(int state)
    {
        static const int cg_clrHCListText[] =
        {
            eColorIndexWindowText, eColorIndexHighlightText, eColorIndexWindowText, eColorIndexGrayText, eColorIndexHighlightText,
        };
        return ColorFromIndex(GetFrom(cg_clrHCListText, state));
    }

    static TMColor GetColorListTreeBk(int state)
    {
        static const int cg_clrHCListBk[] =
        {
            eColorIndexBtnFace, eColorIndexHighlight, eColorIndexBtnFace, eColorIndexBtnFace, eColorIndexHighlight,
        };
        return ColorFromIndex(GetFrom(cg_clrHCListBk, state));
    }

    static TMColor GetColorMenuItemText(int state)
    {
        static const int cg_clrHCMenuItemText[] =
        {
            eColorIndexBtnText, eColorIndexHighlightText, eColorIndexHighlightText, eColorIndexGrayText, eColorIndexHighlightText,
        };
        return ColorFromIndex(GetFrom(cg_clrHCMenuItemText, state));
    }

    static TMColor GetColorMenuItemBk(int state)
    {
        static const int cg_clrHCMenuItemBk[] =
        {
            eColorIndexMenu, eColorIndexHighlight, eColorIndexHighlight, eColorIndexMenu, eColorIndexHighlight,
        };
        return ColorFromIndex(GetFrom(cg_clrHCMenuItemBk, state));
    }
};


////////////////////////////////////////////////////////////////////////////////
//pixel class

// DibPixelT
template<class T> struct DibPixelT;

// _ColorComponentT
template<class T> struct _ColorComponentT;
typedef _ColorComponentT<TMUInt8>   _ColorComponent8;
typedef _ColorComponentT<float>     _ColorComponentF;

TM_TEMPLATE_NULL struct _ColorComponentT<TMUInt8>
{
    typedef TMUInt8 value_type;
    inline static TMUInt8 From(TMUInt8 c) { return c; }
    inline static TMUInt8 From(float c) { return c * 255.0f; }
    inline static TMUInt8 PreMultiplied(TMUInt8 c, TMUInt8 a) { return c * a / 255; }
    inline static TMUInt8 MaxValue() { return 255; }
};

TM_TEMPLATE_NULL struct _ColorComponentT<float>
{
    typedef float value_type;
    inline static float From(TMUInt8 c) { return (float)c / 255.0f; }
    inline static float From(float c) { return c; }
    inline static float PreMultiplied(float c, float a) { return c * a; }
    inline static float MaxValue() { return 1.0f; }
};

// _CLR_RGBI
struct _CLR_RGBI
{
    typedef int                                         value_type;
    typedef _CLR_RGBI                                   meta_type;

    int r,g,b;
    enum { a = 255 };
    enum { eBitR = 32, eBitG = 32, eBitB = 32, eBitA = 0 };
    enum { eBitCount = 24 };

    //DECLARE_PIXEL_FUNC();
    void SetA (int) {};

    TMUInt8 SafeGetR () const { return r < 0 ? 0 : r > 255 ? 255 : r; }
    TMUInt8 SafeGetG () const { return g < 0 ? 0 : g > 255 ? 255 : g; }
    TMUInt8 SafeGetB () const { return b < 0 ? 0 : b > 255 ? 255 : b; }
    TMUInt8 SafeGetA () const { return a; }

    template <class _Tp>
    void SafeToPixel (DibPixelT<_Tp>& pixel)
    {
        pixel.SetRGB (SafeGetR(), SafeGetG(), SafeGetB());
    }
};

struct _CLR_RGBAF
{
    typedef float                                       value_type;
    typedef _CLR_RGBAF                                  meta_type;
    typedef _ColorComponentF                            color_component;
    
    float r,g,b,a;
    enum { eBitR = 32, eBitG = 32, eBitB = 32, eBitA = 32 };
    enum { eBitCount = 32 };
    
    float SafeGetR () const { return r < 0 ? 0 : r > 1.0f ? 1.0f : r; }
    float SafeGetG () const { return g < 0 ? 0 : g > 1.0f ? 1.0f : g; }
    float SafeGetB () const { return b < 0 ? 0 : b > 1.0f ? 1.0f : b; }
    float SafeGetA () const { return a < 0 ? 0 : a > 1.0f ? 1.0f : a; }
    
    template <class _Tp>
        void SafeToPixel (DibPixelT<_Tp>& pixel)
    {
        pixel.SetRGBA(SafeGetR(), SafeGetG(), SafeGetB(), SafeGetA());
    }

    void SetA(TMUInt8 x) { a = color_component::From(x); }
    void SetA(float x) { a = color_component::From(x); }
};

struct _CLR_RGBAI : public _CLR_RGBI
{
    typedef int                                         value_type;
    typedef _CLR_RGBAI                                  meta_type;
    enum { eBitR = 32, eBitG = 32, eBitB = 32, eBitA = 32 };
    enum { eBitCount = 128 };

    using _CLR_RGBI::r;
    using _CLR_RGBI::g;
    using _CLR_RGBI::b;
    int a;

    void SetA (int aa) { a = aa; }

    using _CLR_RGBI::SafeGetR;
    using _CLR_RGBI::SafeGetG;
    using _CLR_RGBI::SafeGetB;
    TMUInt8 SafeGetA () const { return a < 0 ? 0 : a > 255 ? 255 : a; }

    template <class _Tp>
    void SafeToPixel (DibPixelT<_Tp>& pixel)
    {
        pixel.SetRGBA (SafeGetR(), SafeGetG(), SafeGetB(), SafeGetA());
    }
};



// struct _RGB555
// {
//     typedef TMUInt8                                     value_type;
//     typedef _CLR_RGBI                                       meta_type;
//     enum { eBitR = 5, eBitG = 5, eBitB = 5, eBitA = 0 };
// 
//     TMUInt8 b    :    5;
//     TMUInt8 g    :    5;
//     TMUInt8 r    :    5;
//     enum { a = 255 };
// 
//     void SetA (TMUInt8 x) {}
// };
// 
// struct _RGB565
// {
//     typedef TMUInt8                                     value_type;
//     typedef _CLR_RGBI                                       meta_type;
//     enum { eBitR = 5, eBitG = 6, eBitB = 5, eBitA = 0 };
// 
//     TMUInt8 b    :    5;
//     TMUInt8 g    :    6;
//     TMUInt8 r    :    5;
//     enum { a = 255 };
// 
//     void SetA (TMUInt8 x) {}
// };

struct _CLR_BGR
{
    typedef TMUInt8                                     value_type;
    typedef _CLR_BGR                                    meta_type;
    typedef _ColorComponent8                            color_component;
    enum { eBitR = 8, eBitG = 8, eBitB = 8, eBitA = 0 };
    enum { eBitCount = 24 };

    TMUInt8 r;
    TMUInt8 g;
    TMUInt8 b;
    enum { a = 255 };

    void SetA (TMUInt8) {}
};

struct _CLR_ARGB : public _ColorComponent8
{
    typedef TMUInt8                                     value_type;
    typedef _CLR_RGBAI                                  meta_type;
    typedef _ColorComponent8                            color_component;
    enum { eBitR = 8, eBitG = 8, eBitB = 8, eBitA = 8 };
    enum { eBitCount = 32 };

    union
    {
        TMUInt32 argb;
        struct
        {
            TMUInt8 b;
            TMUInt8 g;
            TMUInt8 r;
            TMUInt8 a;
        };
    };

    void SetA (TMUInt8 x) { a = x; }
    TMUInt32 GetNativeColor() const { return argb; }
    void SetNativeColor(TMUInt32 clr) { argb = clr; }
};

struct _CLR_RGB
{
    typedef TMUInt8                                     value_type;
    typedef _CLR_RGBI                                   meta_type;
    typedef _ColorComponent8                            color_component;
    enum { eBitR = 8, eBitG = 8, eBitB = 8, eBitA = 0 };
    enum { eBitCount = 24 };

    TMUInt8 b;
    TMUInt8 g;
    TMUInt8 r;
    enum { a = 255 };

    void SetA (TMUInt8) {}
};

struct _CLR_GRAY
{
    typedef TMUInt8                                     value_type;
    typedef _CLR_RGBI                                   meta_type;
    typedef _ColorComponent8                            color_component;
    enum { eBitR = 8, eBitG = 0, eBitB = 0, eBitA = 0 };
    enum { eBitCount = 8 };

    union
    {
        TMUInt8 b;
        TMUInt8 g;
        TMUInt8 r;
    };
    enum { a = 255 };
    
    void SetA(TMUInt8) {}
    void SetG(TMUInt8) {}
    void SetB(TMUInt8) {}
};

struct _CLR_A
{
    typedef TMUInt8                                     value_type;
    typedef _CLR_RGBI                                   meta_type;
    typedef _ColorComponent8                            color_component;
    enum { eBitR = 0, eBitG = 0, eBitB = 0, eBitA = 8 };
    enum { eBitCount = 8 };
    
    union
    {
        TMUInt8 b;
        TMUInt8 g;
        TMUInt8 r;
        TMUInt8 a;
    };
    
    void SetR(TMUInt8) {}
    void SetG(TMUInt8) {}
    void SetB(TMUInt8) {}
};

template <class TX1, class TX2> struct MetaGet
{
    typedef _CLR_RGBAI meta_type;
};

TM_TEMPLATE_NULL struct MetaGet<_CLR_RGBI, _CLR_RGBI>
{
    typedef _CLR_RGBI meta_type;
};

template <class _Tp> struct ValGet
{
    inline static TMUInt8 get(_Tp v) { return v < 0 ? 0 : v > 255 ? 255 : v; }
};

TM_TEMPLATE_NULL struct ValGet<TMUInt8>
{
    inline static TMUInt8 get(TMUInt8 v) { return v; }
};


// T: _CLR_ARGB, _CLR_BGR, _CLR_GRAY, _CLR_A, _CLR_RGBI, _CLR_RGBAI, _CLR_RGBAF
template<class T> struct DibPixelT : public T
{
    typedef T                                           color_struct;
    typedef typename T::value_type                      value_type;
    typedef typename T::meta_type                       meta_type;
    typedef typename T::color_component                 color_component;
    typedef DibPixelT<meta_type>                        meta_pixel;

    using T::r;
    using T::g;
    using T::b;
    using T::a;
    using T::eBitA;

    DibPixelT() {}

//     DibPixelT(TMColor clr) {}

    DibPixelT(const T& rhs)
        : T(rhs)
    {
    }

    DibPixelT(value_type r, value_type g, value_type b)
    { SetRGBA(r, g, b, color_component::MaxValue()); }

    DibPixelT(value_type r, value_type g, value_type b, value_type a)
    { SetRGBA(r, g, b, a); }

    template<class _Tp>
    DibPixelT(const DibPixelT<_Tp>& rhs)
    {
        if (eBitA && rhs.eBitA)
            SetRGBA(rhs.GetR(), rhs.GetG(), rhs.GetB(), rhs.GetA());
        else
            SetRGB(rhs.GetR(), rhs.GetG(), rhs.GetB()), this->SetA(color_component::MaxValue());
    };

    template<class _Tp>
    DibPixelT& operator = (const DibPixelT<_Tp>& rhs)
    { Set(rhs); return *this; };

//     DibPixelT& operator = (const T& rhs)
//     {
//         *((T*)this) = rhs;
//         return *this;
//     }

    template<class _Tp> void Set(const DibPixelT<_Tp>& rhs)
    {
        if (eBitA && rhs.eBitA)
            SetRGBA(rhs.GetR(), rhs.GetG(), rhs.GetB(), rhs.GetA());
        else
            SetRGB(rhs.GetR(), rhs.GetG(), rhs.GetB());
    }

//     void Set (const T& rhs, ...)
//     {
//         *((T*)this) = rhs;
//     }

    void SetRGB(float _r, float _g, float _b) { SetR(_r); SetG(_g); SetB(_b); }
    void SetRGB(TMUInt8 _r, TMUInt8 _g, TMUInt8 _b) { SetR(_r); SetG(_g); SetB(_b); }
    void SetRGBA(float _r, float _g, float _b, float _a) { SetR(_r); SetG(_g); SetB(_b); T::SetA(_a); }
    void SetRGBA(TMUInt8 _r, TMUInt8 _g, TMUInt8 _b, TMUInt8 _a) { SetR(_r); SetG(_g); SetB(_b); T::SetA(_a); }

    value_type GetR() const { return r; }
    value_type GetG() const { return g; }
    value_type GetB() const { return b; }
    value_type GetA() const { return a; }

    TMUInt8 Get8R() const { return _ColorComponent8::From(r); }
    TMUInt8 Get8G() const { return _ColorComponent8::From(g); }
    TMUInt8 Get8B() const { return _ColorComponent8::From(b); }
    TMUInt8 Get8A() const { return _ColorComponent8::From(a); }

    float GetFR() const { return _ColorComponentF::From(r); }
    float GetFG() const { return _ColorComponentF::From(g); }
    float GetFB() const { return _ColorComponentF::From(b); }
    float GetFA() const { return _ColorComponentF::From(a); }

    void SetR(TMUInt8 x) { r = color_component::From(x); }
    void SetG(TMUInt8 x) { g = color_component::From(x); }
    void SetB(TMUInt8 x) { b = color_component::From(x); }
    void SetR(float x) { r = color_component::From(x); }
    void SetG(float x) { g = color_component::From(x); }
    void SetB(float x) { b = color_component::From(x); }

    void PreMultiplied(TMUInt8 alpha)
    {
        DbgAssertS(sizeof(alpha) == sizeof(value_type));
        this->SetR(color_component::PreMultiplied(this->GetR(), alpha));
        this->SetG(color_component::PreMultiplied(this->GetG(), alpha));
        this->SetB(color_component::PreMultiplied(this->GetB(), alpha));
        this->SetA(color_component::PreMultiplied(this->GetA(), alpha));
    }

    void PreMultiplied(float alpha)
    {
        DbgAssertS(sizeof(alpha) == sizeof(value_type));
        this->SetR(color_component::PreMultiplied(this->GetR(), alpha));
        this->SetG(color_component::PreMultiplied(this->GetG(), alpha));
        this->SetB(color_component::PreMultiplied(this->GetB(), alpha));
        this->SetA(color_component::PreMultiplied(this->GetA(), alpha));
    }

    void PreMultiplied()
    {
        value_type alpha = GetA();
        this->SetR(this->GetR() * alpha);
        this->SetG(this->GetG() * alpha);
        this->SetB(this->GetB() * alpha);
    }

    template <class _Tp> bool operator != (const DibPixelT<_Tp>& rhs) const { return !operator == (rhs); }
    template <class _Tp> bool operator == (const DibPixelT<_Tp>& rhs) const
    {
        return GetR() == _ColorComponentF::From(rhs.GetR())
            && GetG() == _ColorComponentF::From(rhs.GetG())
            && GetB() == _ColorComponentF::From(rhs.GetB());
    }

//     TMColor GetClr () const { return TMRGB(GetR(), GetG(), GetB()); }
    void SetColor(TMColor rgba) { SetRGBA(ColorR(rgba), ColorG(rgba), ColorB(rgba), ColorA(rgba)); }
    TMColor ToColor() const { return TMRGBA(Get8R(), Get8G(), Get8B(), Get8A()); }
    void SetColorAsRGB_A(TMColor rgba) // convert rgba to rgb_a
    {
        TMUInt8 a = ColorA(rgba);
        TMUInt8 r = ColorR(rgba);
        TMUInt8 g = ColorG(rgba);
        TMUInt8 b = ColorB(rgba);
        if (a == 0)
            SetRGBA(r,g,b,a);
        else
            SetRGBA(r * 255 / a, g * 255 / a, b * 255 / a, a);
    }

#ifdef TM_WIN
    void SetFromCOLORREF(COLORREF rgb) { SetRGBA(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb), 255); }
    COLORREF ToCOLORREF() const { return RGB(Get8R(), Get8G(), Get8B()); }
#endif

}; // DibPixelT

typedef DibPixelT<_CLR_RGB>                                 DibRGB;
typedef DibPixelT<_CLR_ARGB>                                DibARGB; // window && GX default.
typedef DibPixelT<_CLR_BGR>                                 DibBGR;
typedef DibPixelT<_CLR_GRAY>                                DibGRAY;
typedef DibPixelT<_CLR_A>                                   DibA;

typedef DibPixelT<_CLR_RGBI>                                DibRGBI;
typedef DibPixelT<_CLR_RGBAI>                               DibRGBAI;
typedef DibPixelT<_CLR_RGBAF>                               DibRGBAF;


// fast pixel
template <> struct DibPixelT<TMUInt64>
{
    enum { MA = 0, MR = 16, MG = 32, MB = 48 };
    enum EColor { M255 = 0xff };

    TMUInt64 rgba;

    inline static TMUInt64 EColor64(TMUInt64 clr) { return (clr << MR) + (clr << MB) + (clr << MG) + (clr << MA); }

    TMUInt8 GetR() const { return (TMUInt8)(rgba >> MR); }
    TMUInt8 GetG() const { return (TMUInt8)(rgba >> MG); }
    TMUInt8 GetB() const { return (TMUInt8)(rgba >> MB); }
    TMUInt8 GetA() const { return (TMUInt8)(rgba >> MA); }

    template <class T>
        DibPixelT(const DibPixelT<T>& rhs)
    {
        rgba = (((TMUInt64)rhs.GetR()) << MR) + (((TMUInt64)rhs.GetG()) << MG) + (((TMUInt64)rhs.GetB()) << MB) + (((TMUInt64)rhs.GetA()) << MA);
    }

    DibPixelT(const DibPixelT& rhs)
        : rgba (rhs.rgba)
    {
    }

    DibPixelT(TMUInt64 rhs)
        : rgba (rhs)
    {
    }

    DibPixelT(TMUInt8 clr)
        : rgba (EColor64(clr))
    {
    }


    DibPixelT operator + (EColor clr) const
    {
        return rgba + EColor64(clr);
    }

    DibPixelT operator + (DibPixelT clr) const
    {
        return rgba + clr.rgba;
    }

    DibPixelT operator - (EColor clr) const
    {
        return rgba - EColor64(clr);
    }

    DibPixelT operator - (DibPixelT clr) const
    {
        return rgba - clr.rgba;
    }

    DibPixelT operator * (TMUInt8 clr) const
    {
        return rgba * clr;
    }

    DibPixelT operator / (TMUInt8 clr) const
    {
        return rgba / clr;
    }

    DibPixelT operator << (TMUInt8 clr) const
    {
        return rgba << clr;
    }

    DibPixelT operator >> (TMUInt8 clr) const
    {
        return rgba >> clr;
    }

    DibPixelT Div255() const
    {
        const TMUInt64 nMask = ~EColor64(0xff);
        return (rgba & nMask) >> 8;
    }

};

typedef DibPixelT<TMUInt64> DibFastARGB;

// struct GDI2GXColor : public TM::DibARGB
// {
//     GDI2GXColor(COLORREF clr)
//     {
//         SetFromCOLORREF(clr);
//     }
// 
//     operator TMColor() const { return argb; }
// };
// 
// struct CSysColor : public DibARGB
// {
//     CSysColor(int nIndex)
//     {
//         SetFromCOLORREF(GetSysColor(nIndex));
//     }
//     operator TMColor() const { return argb; }
// };


TM_END_NAMESPACE


#ifdef TM_WIN

inline TMColor ColorFromCOLORREF(COLORREF x) { return ColorFromRGBA(GetRValue(x), GetGValue(x), GetBValue(x), 0xff); }
inline COLORREF ColorToCOLORREF(TMColor x) { return RGB(ColorR(x), ColorG(x), ColorB(x)); }

inline TMColor ColorFromCOLORREF_A(COLORREF x) { return ColorFromRGBA(GetRValue(x), GetGValue(x), GetBValue(x), ColorA(x)); }
inline COLORREF ColorToCOLORREF_A(TMColor x) { return x; }

inline TMColor ColorFromSys(int nIndex) { return ColorFromCOLORREF(GetSysColor(nIndex)); }

#elif defined(TM_QT)

inline TMColor ColorFromQColor(QColor clr) { return clr.rgba(); }
inline QColor ColorToQColor(TMColor x) { return QColor(x); }

inline QColor QColorFromSys(int nIndex)
{
    switch (nIndex)
    {
    case eColorIndexScrollBar:                  return QApplication::palette().color(QPalette::Mid);
    case eColorIndexBackground:                 return QApplication::palette().color(QPalette::Window);
    case eColorIndexActiveCaption:              return QApplication::palette().color(QPalette::Active, QPalette::Highlight);
    case eColorIndexInactiveCaption:            return QApplication::palette().color(QPalette::Inactive, QPalette::Highlight);
    case eColorIndexMenu:                       return QApplication::palette("QMenu").color(QPalette::Window);      // QPalette::Button
    case eColorIndexWindow:                     return QApplication::palette().color(QPalette::Base);
    case eColorIndexWindowFrame:                return QApplication::palette().color(QPalette::Shadow);
    case eColorIndexMenuText:                   return QApplication::palette("QMenu").color(QPalette::WindowText);  // QPalette::Text
    case eColorIndexWindowText:                 return QApplication::palette().color(QPalette::WindowText);
    case eColorIndexCaptionText:                return QApplication::palette().color(QPalette::Active, QPalette::HighlightedText);
    case eColorIndexActiveBorder:               return QApplication::palette().color(QPalette::Active, QPalette::Dark);
    case eColorIndexInactiveBorder:             return QApplication::palette().color(QPalette::Inactive, QPalette::Mid);
    case eColorIndexAppWorkspace:               return QApplication::palette().color(QPalette::AlternateBase);
    case eColorIndexHighlight:                  return QApplication::palette().color(QPalette::Highlight);
    case eColorIndexHighlightText:              return QApplication::palette().color(QPalette::HighlightedText);
    case eColorIndexBtnFace:                    return QApplication::palette().color(QPalette::Button);
    case eColorIndexBtnShadow:                  return QApplication::palette().color(QPalette::Dark);
    case eColorIndexGrayText:                   return QApplication::palette().color(QPalette::Disabled, QPalette::WindowText); //  QPalette::Text
    case eColorIndexBtnText:                    return QApplication::palette().color(QPalette::ButtonText);
    case eColorIndexInactiveCationText:         return QApplication::palette().color(QPalette::Inactive, QPalette::WindowText);
    case eColorIndexBtnHightlight:              return QApplication::palette().color(QPalette::Light);      // QPalette::BrightText
    case eColorIndex3DDKShadow:                 return QApplication::palette().color(QPalette::Shadow);
    case eColorIndex3DLight:                    return QApplication::palette().color(QPalette::Midlight);
    case eColorIndexInfoText:                   return QToolTip::palette().color(QPalette::ToolTipText);    // QPalette::Window
    case eColorIndexInfoBK:                     return QToolTip::palette().color(QPalette::ToolTipBase);    // QPalette::Text
    case eColorIndexHotLight:                   return QApplication::palette().color(QPalette::Link);
    case eColorIndexGradientActiveCaption:      return QApplication::palette().color(QPalette::Active, QPalette::Highlight).lighter(120);
    case eColorIndexGradientInactiveCaption:    return QApplication::palette().color(QPalette::Inactive, QPalette::Highlight).lighter(140);
    case eColorIndexMenuLight:                  return QApplication::palette("QMenu").color(QPalette::Light);
    case eColorIndexMenuBar:                    return QApplication::palette("QMenuBar").color(QPalette::Window);
//     case eColorWebAccentColor:                  return QApplication::palette().color(QPalette::Accent);
//     case eColorWebAccentColorText:              return QApplication::palette().color(QPalette::BrightText);
//     case eColorWebActiveText:                   return QApplication::palette().color(QPalette::Link);
//     case eColorWebButtonBorder:                 return QApplication::palette().color(QPalette::Dark);
//     case eColorWebVisitedText:                  return QApplication::palette().color(QPalette::LinkVisited);
    default:
        return 0;
    }
}
inline TMColor ColorFromSys(int nIndex) 
{
    return ColorFromQColor(QColorFromSys(nIndex));
}

#elif defined(TM_GTK4)
#   include <gtk/gtk.h>

inline TMColor ColorFromGdkColor(GdkRGBA clr) { return ((TM::DibRGBAF*)(&clr))->ToColor(); }
inline GdkRGBA ColorToGdkColor(TMColor x) { TM::DibRGBAF rgba; rgba.SetColor(x); return *(GdkRGBA*)&rgba; }

TM_CAPI GdkRGBA GdkColorFromSys(int nIndex);

inline TMColor ColorFromSys(int nIndex) 
{
    GdkRGBA rgba = GdkColorFromSys(nIndex);
    return ColorFromGdkColor(rgba);
}
#else
inline TMColor ColorFromSys(int nIndex) { return 0; }
#endif



#endif // TMCOLOR_H
