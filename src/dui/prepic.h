/**
 *  @file
 *  @brief default pic object
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2012.7.13
 *
 *  Copyright (C) 2012 miragekiller                               
 */


#pragma once

#ifndef PREPIC_H
#define PREPIC_H

#include "../tmgdix.h"


TM_BGN_NAMESPACE

struct CPrePicProxy
    : public CTuiObject
    , public ITuiPic
{
    CModule& m_mdl;

    TM_BGN_INTERFACE_MAP(ITuiPic)
    TM_END_INTERFACE_MAP()
    TM_UNUSED_ADDREF_RELEASE();

    CPrePicProxy(CModule& mdl, TMUInt32 oid) : m_mdl(mdl) { m_dwObjID = oid; }
    bool operator < (const CPrePicProxy& rhs) const { return GetObjectID() < rhs.GetObjectID(); }

    // ITuiPic
    virtual TMUInt32 GetObjectID () const tm_override  { return m_dwObjID; }
    virtual void Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx = 0, IGXApp* gapp = 0) tm_override {}

    // unused interface for CTuiObject
    void OnLoadProp (IDomElement* pXml) {}
    void SaveProp(class CSaveStream& __stm) {}
    void EditProp(class CEditStream& __stm) {}
    void ResetIDProp(class CIdStream& __stm) {}
    CString GetDebugInfo() { return CString(); }
    int EditorRender(IGXApp* gapp, int nState, const CTuiRect& rc) { return 0; }
    TMBool EditorMatchID(TMUInt32 n) { return false; }
};


template <int nID> class CPrePicProxyT : public CPrePicProxy
{
public:
    CPrePicProxyT(CModule& mdl) : CPrePicProxy(mdl, 0) {}
    virtual TMUInt32 GetObjectID() const tm_override { return nID; }
    virtual void GetSize(TMSize& sz) tm_override;
    virtual void Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx = 0, IGXApp* gapp = 0) tm_override;
    virtual void RenderSB(const SCROLLSTATE& state, const CTuiRect& rc, WPRENDERFX_SB* fx = 0, IGXApp* gapp = 0) tm_override;
};

template <int nID> void CPrePicProxyT<nID>::Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp) {}
template <int nID> void CPrePicProxyT<nID>::RenderSB(const SCROLLSTATE& state, const CTuiRect& rc, WPRENDERFX_SB* fx, IGXApp* gapp) {}


struct CPrePicList
{
    CText txtCaption[5];
    CText txtScroll[4];
    CText txtRadio[6];
    CText txtCheck[6];
    CText txtCombo;
    GXFont font;

    CPrePicProxyT<IDP_PRE_PIC_NULL>         m_picDft_NULL;
    CPrePicProxyT<IDP_PRE_PIC_BUTTON>       m_picDft_BUTTON;
    CPrePicProxyT<IDP_PRE_PIC_CHECK>        m_picDftCheck;
    CPrePicProxyT<IDP_PRE_PIC_RADIO>        m_picDftRadio;
    CPrePicProxyT<IDP_PRE_PIC_GROUP>        m_picDftGroup;
    CPrePicProxyT<IDP_PRE_PIC_COMB>         m_picDftComb;
    CPrePicProxyT<IDP_PRE_PIC_LIST_ITEM>      m_picDft_LISTBOX;
    CPrePicProxyT<IDP_PRE_PIC_MENU>         m_picDft_MENU;
    CPrePicProxyT<IDP_PRE_PIC_MENU_POP>     m_picDftMenuPopItem;
    CPrePicProxyT<IDP_PRE_PIC_HEADER_UP>    m_picDft_HEADER_UP;
    CPrePicProxyT<IDP_PRE_PIC_HEADER_DOWN>  m_picDft_HEADER_DOWN;
    CPrePicProxyT<IDP_PRE_PIC_UP>           m_picDft_UP;
    CPrePicProxyT<IDP_PRE_PIC_DOWN>         m_picDft_DOWN;
    CPrePicProxyT<IDP_PRE_PIC_FOCUS>        m_picDftFocus;
    CPrePicProxyT<IDP_PRE_PIC_SCROLLBAR>    m_picDft_SCROLLBAR;
    CPrePicProxyT<IDP_PRE_PIC_SIZEBOX>      m_picDft_SIZEBOX;
    CPrePicProxyT<IDP_PRE_PIC_OUTERBORDER>  m_picDft_OUTERBORDER;
    CPrePicProxyT<IDP_PRE_PIC_INNERBORDER>  m_picDft_INNERBORDER;
    CPrePicProxyT<IDP_PRE_PIC_CAPTION>      m_picDft_CAPTION;
    CPrePicProxyT<IDP_PRE_PIC_CLOSE>        m_picDft_CLOSE;
    CPrePicProxyT<IDP_PRE_PIC_MIN>          m_picDft_MIN;
    CPrePicProxyT<IDP_PRE_PIC_MAX>          m_picDft_MAX;
    CPrePicProxyT<IDP_PRE_PIC_HELP>         m_picDft_HELP;

    CPrePicProxy* begin() { return &m_picDft_NULL; }
    CPrePicProxy* end()   { return 1 + &m_picDft_HELP; }

    CPrePicList(CModule& mdl)
        : m_picDft_NULL         (mdl)
        , m_picDft_BUTTON       (mdl)
        , m_picDftCheck         (mdl)
        , m_picDftRadio         (mdl)
        , m_picDftGroup         (mdl)
        , m_picDftComb          (mdl)
        , m_picDft_LISTBOX      (mdl)
        , m_picDft_MENU         (mdl)
        , m_picDftMenuPopItem   (mdl)
        , m_picDft_HEADER_UP    (mdl)
        , m_picDft_HEADER_DOWN  (mdl)
        , m_picDft_UP           (mdl)
        , m_picDft_DOWN         (mdl)
        , m_picDftFocus         (mdl)
        , m_picDft_SCROLLBAR    (mdl)
        , m_picDft_SIZEBOX      (mdl)
        , m_picDft_OUTERBORDER  (mdl)
        , m_picDft_INNERBORDER  (mdl)
        , m_picDft_CAPTION      (mdl)
        , m_picDft_CLOSE        (mdl)
        , m_picDft_MIN          (mdl)
        , m_picDft_MAX          (mdl)
        , m_picDft_HELP         (mdl)
    {
        InitPreRes();
    }

    void InitPreRes();

    void Reclaim()
    {
        tm_for (int i = 0; i < tm_countof(txtCaption); ++i)
            txtCaption[i].Reclaim();
        tm_for (int i = 0; i < tm_countof(txtScroll); ++i)
            txtScroll[i].Reclaim();
        tm_for (int i = 0; i < tm_countof(txtRadio); ++i)
            txtRadio[i].Reclaim();
        tm_for (int i = 0; i < tm_countof(txtCheck); ++i)
            txtCheck[i].Reclaim();
        txtCombo.Reclaim();
    }

    CPrePicProxy* find(TMUInt32 dwObjID)
    {
        CPrePicProxy _temp(*(CModule*)0, dwObjID);
        CPrePicProxy* iter = std::lower_bound(begin(), end(), _temp);
        if (iter != end())
            return (iter->GetObjectID() == dwObjID) ? iter : 0;
        DbgAssert(dwObjID > IDP_PRE_PIC_HELP);
        return 0;
    }
};



TM_END_NAMESPACE


#endif // PREPIC_H
