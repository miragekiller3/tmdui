/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.29
 *
 *  Copyright (C) 2006 miragekiller
 */

#include "pic.h"
#include "../tmdib.h"
#include "property.h"


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// CPic


////////////////////////////////////////////////////////////////////////////////
// filled with color 
class CPicColor : public CPic
{
private:
    CPropColor    m_clr;
    
    BGN_OBJ_PROP_MAP     ()
        OBJ_PROP_CLASS   ("Color", m_clr)
        CHAIN_OBJ_PROP   (CPic)
    END_OBJ_PROP_MAP     ()
        
public:
    CPicColor ()
        : CPic(ColorTransform)
        , m_clr(TMRGB(192,192,192))
    {
    }
    
    virtual void Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp) tm_override
    {
        TMColor clr = m_clr.ToColor();
        if (IsFlagSupportDark())
            gapp->FillRect(rc, GetModule()->ConvertColor(GetObjectID(), clr));
        else
            gapp->FillRect(rc, clr);
    }
};

TM_REGISTER_CLASS4(CPicColor, TM_CS(TUI_CLASS_PIC_CLOR), true, false);


class CPicGradient : public CPic
{
private:
    CPropColor    m_clrFrom;
    CPropColor    m_clrTo;
    EGrandientFill m_eFill;

    TM_BGN_ENUM_SYMBOL  (CSymbol_GrandientFill)
        TM_ENUM_SYMBOL  (e_gfNormal0)
        TM_ENUM_SYMBOL  (e_gfNormal45)
        TM_ENUM_SYMBOL  (e_gfNormal90)
        TM_ENUM_SYMBOL  (e_gfNormal135)
        TM_ENUM_SYMBOL  (e_gfSym0)
        TM_ENUM_SYMBOL  (e_gfSym45)
        TM_ENUM_SYMBOL  (e_gfSym90)
        TM_ENUM_SYMBOL  (e_gfSym135)
        TM_ENUM_SYMBOL  (e_gfCorner45)
        TM_ENUM_SYMBOL  (e_gfCorner135)
        TM_ENUM_SYMBOL  (e_gfCenter)
    TM_END_ENUM_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_CLASS  ("ColorFrom", m_clrFrom)
        OBJ_PROP_CLASS  ("ColorTo", m_clrTo)
        OBJ_PROP_ENUM   ("FillMode", m_eFill, CSymbol_GrandientFill)
        CHAIN_OBJ_PROP  (CPic)
    END_OBJ_PROP_MAP    ()

public:
    CPicGradient ()
        : CPic(ColorTransform)
        , m_clrFrom(TMRGB(255,255,255))
        , m_clrTo(TMRGB(128,128,128))
        , m_eFill(e_gfNormal0)
    {
    }

//     void OnLoadProp ()
//     {
//         if (GetAValue(m_clrFrom) == 0)
//             SetAValue(m_clrFrom, 255);
//         if (GetAValue(m_clrTo) == 0)
//             SetAValue(m_clrTo, 255);
//     }

    inline TMUInt8 Clr2(TMUInt8 c0, TMUInt8 c1) { return (c0 + c1) / 2; }

    virtual void Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp) tm_override
    {
        TMColor clrT = m_clrTo.ToColor();
        TMColor clrF = m_clrFrom.ToColor();
        if (IsFlagSupportDark())
        {
            CModule* pv = GetModule();
            gapp->GrandientFill(rc, pv->ConvertColor(GetObjectID(), clrF), pv->ConvertColor(GetObjectID(), clrT), m_eFill);
        }
        else
            gapp->GrandientFill(rc, clrF, clrT, m_eFill);
    }
};

TM_REGISTER_CLASS4(CPicGradient, TM_CS(TUI_CLASS_PIC_GRADIENT), true, false);


TM_END_NAMESPACE
