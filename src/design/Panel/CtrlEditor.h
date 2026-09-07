//-----------------------------------------------------------
//PanelDlg.h

//  Copyright (C) 2004 miragekiller                               
//  All rights reserved                                                     
//
//
//  Author: 
//      miragekiller    <3916345933@qq.com>
//
//  Histroy :
//      9/29/2006       create
//-------------------------------------------------------------

#pragma  once 

#include "FactoryData.h"


TM_BGN_NAMESPACE

struct CModule;
TM_END_NAMESPACE

// class CPanelApp;


//////////////////////////////////////////////////////////////////////////
// CSharedObjData: for pic and frame
class CWinObj
    : public CObjData
    , public TM::CTimerImpl<CWinObj>
{
protected:
    ~CWinObj()
    {
        if (!m_bSpyMode && m_hWnd)
            m_hWnd.DestroyWindow();
    }

public:
    // IObjEdit
    virtual void DeleteAfter5s() tm_override;
    virtual TM::CString GetAccInfo() tm_override;
    virtual bool IsSameAcc(IAccessible* acc) tm_override;

    // CTimerImpl
    void OnTimer(...);
};

class CSharedEditorObj : public CObjData
{
public:
    TM::CRefPtr<TM::CTuiObject> m_ptrObj;
    TM::CTuiRect m_rcWnd;

    CSharedEditorObj() : m_rcWnd(0,0,500,300) {}
    TM::CTuiObject* GetProp() { return m_ptrObj; }
};

//////////////////////////////////////////////////////////////////////////
// CFrameEditor
class CFrameEditor : public CSharedEditorObj
{
public:
    enum { eObjType = TM::e_otFrm };
    CFrameEditor() { m_eType = TM::e_otFrm; }
    void Create (TM::CTuiWidget wndParent, TM::CTuiObject* pObj);
//     virtual TMBool RendThumbnail(IGXApp* gapp, const TM::CTuiRect& rc, EThumb eThumb) tm_override;
    virtual int RendSample(IGXApp* gapp, int nState, const TM::CTuiRect& rc, ERender eRender) tm_override;
};

class CPicEditor : public CSharedEditorObj
{
public:
    enum { eObjType = TM::e_otPic };
    CPicEditor() { m_eType = TM::e_otPic; }
    void Create (TM::CTuiWidget wndParent, TM::CTuiObject* pObj);
//     virtual TMBool RendThumbnail(IGXApp* gapp, const TM::CTuiRect& rc, EThumb eThumb) tm_override;
    virtual int RendSample(IGXApp* gapp, int nState, const TM::CTuiRect& rcx, ERender eRender) tm_override;
};

class CManagerEditor
    : public CWinObj
{
public:
    enum { eObjType = TM::e_otWnd };

    CManagerEditor();
    ~CManagerEditor();

    void Create (TM::CTuiWidget wndParent, TM::CTuiObject* pManger);
    void EnumCreateChildWindow(TM::CTuiWgt* pWinParent, IObjEdit* pItemParent);

protected:
    virtual TM::CTuiObject* GetProp() tm_final;
    virtual TM::CString ID2Text(TM::CString* pStrUser) tm_final;
};

