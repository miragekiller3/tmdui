/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.29
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef PIC_H
#define PIC_H

#include "object.h"
#include "module.h"


TM_BGN_NAMESPACE


class CPic;

enum
{
    LeftTop         = CBltSharp::e_lt,
    CenterTop       = CBltSharp::e_wt,
    RightTop        = CBltSharp::e_rt,
    LeftCenter      = CBltSharp::e_lh,
    LeftBottom      = CBltSharp::e_lb,
    CenterBottom    = CBltSharp::e_wb,
    RightCenter     = CBltSharp::e_rh,
    CenterCenter    = CBltSharp::e_wh,
    RightBottom     = CBltSharp::e_rb,
    ColorKey        = 0x01 << 10, // is transparent
    SourceCopy      = 0x01 << 11,
    L2RReading      = 0x01 << 12,
    SimpleFrame     = 0x01 << 14,
    ColorTransform  = 0x01 << 15,
    MonochromeOnHC  = 0x01 << 16,
    OverlyingHigh   = 0x01 << 17,
    OverlyingFocus  = 0x01 << 18,
};

////////////////////////////////////////////////////////////////////////////////
// CPic
class TM_NO_VTABLE CPic
    : public CTuiObject
    , public ITuiPic
{
public:
    TMUInt32 m_nFlag;

    TM_BGN_MASK_SYMBOL  (CSymbol_FlagBase)
        TM_MASK_SYMBOL  (ColorTransform)
    TM_END_MASK_SYMBOL  ()

    TM_DECLARE_FLAG_GET (FlagColorKey,      m_nFlag, ColorKey);
    TM_DECLARE_FLAG_GET (FlagSimpleFrame,   m_nFlag, SimpleFrame);
    TM_DECLARE_FLAG     (FlagSupportDark,   m_nFlag, ColorTransform);
    TM_DECLARE_FLAG     (FlagMonoOnHC,      m_nFlag, MonochromeOnHC);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Valid", m_nFlag, CSymbol_FlagBase)
        CHAIN_OBJ_PROP  (CTuiObject)
    END_OBJ_PROP_MAP    ()

protected:
    CPic(TMUInt32 nFlag = 0) : m_nFlag(nFlag) {}
    ~CPic() {}

public:
    enum { eObjType = e_otPic };

public:
    TM_BGN_INTERFACE_MAP(CPic)
        TM_INTERFACE_HANDLER(ITuiPic);
    TM_END_INTERFACE_MAP()
    
    virtual void SetStreamSource(IStream* stream) {}

    // ITuiPic
    TM_REFAPI AddRef () tm_override { return CTuiObject::AddRef(); }
    TM_REFAPI Release() tm_override { return CTuiObject::Release(); }
    virtual TMUInt GetSupportStates() { return 1 << e_bsNormal; }
    virtual TMUInt32 GetObjectID () const tm_override { return m_dwObjID; }
    virtual CComPtr<ITuiPic> GetSubPic(int dpi) tm_override { return this; }

    // CTuiObject
    int EditorRender(IGXApp* gapp, int nState, const CTuiRect& rc)
    {
        this->Render(nState, 0, rc, 0, gapp);
        return 0;
    }
};

class TM_NO_VTABLE CPicTranform : public CPic
{
private:
    IColorTransform*    m_ctf;

    // ITuiPic
    virtual void SetColorTransform(IColorTransform* ctf)
    {
        if (m_ctf == ctf)
            return;
        m_ctf = ctf;
        m_pFactoryInfo->m_pFactory->FDispatch(this, CM_COLORVISUAL, eColorVisualTransform, 0);
    }

protected:
    CPicTranform() : m_ctf(0) {}

    IColorTransform* GetColorTransform()
    {
        if (m_ctf)
            return m_ctf;
        return IsFlagSupportDark() ? GetModule()->m_pColorTransform : 0;
    }
};

struct CPicPtr : public CObjPtr<ITuiPic, CPic>
{
    typedef CObjPtr<ITuiPic, CPic> base_class;

    ITuiPic* operator = (ITuiPic* lp)
    {
        base_class::Assign(lp);
        return p;
    }
    
    void ChangeDPI(int dpi)
    {
        if (p)
            base_class::Assign(p->GetSubPic(dpi));
    }
};

void DefPicTerm();


TM_END_NAMESPACE

#endif // PIC_H
