//-----------------------------------------------------------
//PanelDlg.cpp

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

#include "StdAfx.h"
#include "Panel.h"
#include "ctrleditor.h"
#include "tmduiacc.h"


//#include "DlgZoom.h"
#include "WndPad.h"
#include "ResourceID.h"
#include "WndSpy.h"

#ifndef DEFINE_ENUM_FLAG_OPERATORS
#   define DEFINE_ENUM_FLAG_OPERATORS(x) TM_ENUM_2_FLAG(x)
#endif

//#include "_sal.h"
//#include <UIAutomationClient.h>


using namespace TM;


static TMBool _CObjData_MatchID(CPanelApp* app, int nid, int nStrID, const TM::CString& str)
{
    if (!nid)
        return false;
    if (nStrID == nid)
        return true;
    CString strUser;
    TM::CString strMacro(app->m_idMgr.ID2Text(nid, &strUser));
    return strMacro.FindI(str) >= 0 || strUser.FindI(str) >= 0;
}

TM::CString CObjData::ID2Text(TM::CString* pStrUser)
{
    if (m_bSpyMode)
    {
        TM::CString str;
        TM::CTuiObject* pObj = GetProp();
        str.Format(L"%d (%p)", pObj->GetObjectID(), pObj);
        return std::move(str);
    }
    return GetApp()->m_idMgr.ID2Text((TMUInt32)GetId(), pStrUser);
}

TM::CString CManagerEditor::ID2Text(TM::CString* pStrUser)
{
    if (m_bSpyMode)
    {
        TM::CString str;
        TM::CTuiObject* pObj = GetProp();
        str.Format(L"%d HTUI(%p) HWND(%p)", pObj->GetObjectID(), pObj, (static_cast<HTUI>(pObj))->GetHWND());
        return std::move(str);
    }
    return CObjData::ID2Text(pStrUser);
}

TMBool CObjData::_Match(const TM::CStrView& strItemTxt, const TM::CStrView& strFilter, int& nRootLevel)
{
    int nid = strFilter.ToLong();
    int id2 = 0;
    TM::EObjType eType = GetType();
    if (eType == e_otWnd || eType == e_otWgt)
        id2 = (static_cast<HTUI>(GetProp()))->GetDlgCtrlID();
    CPanelApp* app = GetApp();
    TMBool b = strFilter.IsEmpty()
        || -1 != strItemTxt.FindI(strFilter)
        || _CObjData_MatchID(app, GetId(), nid, strFilter)
        || _CObjData_MatchID(app, id2, nid, strFilter);
    if (!b && !nid)
    {
        nid = app->m_idMgr.Text2ID(strFilter); // strFilter maybe pic, frame, win, dlg id.
        if (nid)
            b = GetProp()->EditorMatchID(nid);
        else // strFilter may be style
        {

        }
    }
    m_bFilter = b;
    nRootLevel = 2;
    return b;
}


////////////////////////////////////////////////////////////////////////////////
// CWinObj
void CWinObj::DeleteAfter5s()
{
    KillTimer();
    SetTimer(1000);
}

static void _CWinObj_GetAcc(HTUI hWnd, CComPtr<IAccessible>& ptr)
{
    struct dll_proxy   { DECLARE_DLL_LOAD("TMDuiAcc.dll", LOAD_LIBRARY_SEARCH_APPLICATION_DIR); };
    DECLARE_DLL_FUNC(dll_proxy, TMBool, DFTCALL, GetAccObject, (TM::HTUI hWgt, TMWParam wParam, TMLParam lParam, TMResult& lr));
    if (!_GetAccObject)
        return;
    TMResult lr = 0;
    _GetAccObject(hWnd, 0, OBJID_CLIENT, lr);
    if (!lr)
        return;
    DbgVerifyHResult(lr = ObjectFromLresult(lr, IID_IAccessible, 0, (void**)&ptr));
}

TM::CString CWinObj::GetAccInfo()
{
    CComPtr<IAccessible> ptr;
    _CWinObj_GetAcc(m_hWnd, ptr);
    if (!ptr)
        return TM::CString();
    return std::move(CPanelApp::FormatAcc(ptr, 0));
}

bool CWinObj::IsSameAcc(IAccessible* acc)
{
    CComBSTR bstr;
    acc->get_accDescription(CComVariant(CHILDID_GETHTUI), &bstr);
    TMLongPtr l;
    StrScanLong(bstr, -1, l, 16);
    return (l == (TMLongPtr)m_hWnd.m_hWnd);
}

void CWinObj::OnTimer(...)
{
    KillTimer();
    HWND hWnd = CTuiDummy(m_hWnd).GetWindow();
    if (hWnd)
        ::DestroyWindow(hWnd);
    else
        m_hWnd.DestroyWindow();
};


////////////////////////////////////////////////////////////////////////////////
// CEditorObject

template <class T> class CEditorObject : public T
{
public:
    enum { eEditMode = 1 << T::eObjType };
    TM_SELF_ALLOC(CEditorObject);
    
    inline static TM::EObjType GetObjType() { return (TM::EObjType)T::eObjType; }
    inline static EDIT_MODE GetEditMode() { return (EDIT_MODE)eEditMode; }
    
    virtual TM::EObjType GetType() { return GetObjType(); }
    
    CEditorObject(CPanelApp& view, IObjEdit* pParent, CTuiObject* p)
    {
        this->m_app = &view;
        p->m_pEditData = this;
        DbgAssert(p->GetType() == this->eObjType);
        this->Create(0, p);
    }
};



////////////////////////////////////////////////////////////////////////////////
// CFrameEditor
void CFrameEditor::Create(TM::CTuiWidget wndParent, TM::CTuiObject* pObj)
{
    DbgAssert(pObj);
    DbgAssert(pObj->GetType() == e_otFrm);

    m_ptrObj = pObj;
}

// TMBool CFrameEditor::RendThumbnail(IGXApp* gapp, const TM::CTuiRect& rc, EThumb eThumb)
// {
//     if (eThumb != this->eThumbAll)
//         return false;
//     m_ptrObj->EditorRender(gapp, 0, rc);
//     return true;
// }

int CFrameEditor::RendSample(IGXApp* gapp, int nState, const CTuiRect& rc, ERender eRender)
{
    if (eRender == eRenderForTree || eRender == eRenderThumbNoChild)
        return -1;
    return m_ptrObj->EditorRender(gapp, nState, rc);
}

////////////////////////////////////////////////////////////////////////////////
// CPicEditor

void CPicEditor::Create (TM::CTuiWidget wndParent, TM::CTuiObject* pObj)
{
    DbgAssert(pObj);
    DbgAssert(pObj->GetType() == e_otPic);

    m_ptrObj = pObj;
}

// TMBool CPicEditor::RendThumbnail(IGXApp* gapp, const TM::CTuiRect& rc, EThumb eThumb)
// {
//     if (eThumb == this->eThumbNoChild)
//         return false;
// 
//     if (gapp->ClipAndPush(rc, true, eGXPropClip))
//     {
//         m_ptrObj->EditorRender(gapp, 0, rc);
//         gapp->Pop();
//     }
//     return true;
// }

int CPicEditor::RendSample(IGXApp* gapp, int nState, const CTuiRect& rc, ERender eRender)
{
    if (eRender == this->eRenderThumbNoChild)
        return -1;

    return m_ptrObj->EditorRender(gapp, nState, rc);
}

////////////////////////////////////////////////////////////////////////////////
// CWinData
class CWinData : public CWinObj
{
public:
    enum { eObjType = TM::e_otWgt };

    CWinData ()
    {
        m_eType = TM::e_otWgt;
    }

    void Create(TM::CTuiWidget wndParent, TM::CTuiObject* pT)
    {
        m_hWnd = static_cast<CTuiWgt*>(pT);
    }

    virtual TM::CTuiObject* GetProp() { return m_hWnd; }

    virtual IObjEdit* GetAncestorItem() { return m_hWnd.m_hWnd->GetOwner()->m_pEditData; }
    
    virtual CObjData* GetOwner() { return this; }
};

////////////////////////////////////////////////////////////////////////////////
// CManagerEditor

//TM_TEMPLATE_NULL
CManagerEditor::CManagerEditor()
//     : m_bDown(false)
{
    m_eType = TM::e_otWnd;
}

CManagerEditor::~CManagerEditor()
{
}

void CManagerEditor::Create (TM::CTuiWidget wndParent, TM::CTuiObject* pManger)
{
    TM::CTuiWnd* p = static_cast<TM::CTuiWnd*>(pManger);
    m_hWnd = p;
}

TM::CTuiObject* CManagerEditor::GetProp()
{
    return m_hWnd;
}

////////////////////////////////////////////////////////////////////////////////
// CObjData
void CObjData::Release()
{
    TM::CTuiObject* p = GetProp();
    if (p)
        p->m_pEditData = 0;
    delete this;
}

int CObjData::GetRef()
{
    return GetProp()->m_nRef - 1;
}

static void _CObjData_RendSample(IGXApp* gapp, CTexture& tex, const TM::CTuiRect* rcDst, CTuiWidget wnd, TMBool bRenderChild)
{
    TM::CTuiRect rc;
    wnd.GetWindowRect(&rc);
    if (!rcDst)
        rcDst = &rc;
    CSizeF szf(rcDst->Size());
    CSizeI szi;
    wnd.GetDPI().LPtoDPCeil(szf, szi);
    
    tex.CreateRenderTarget(szi.cx, szi.cy);
    if (!tex)
        return;
    CRect rcx(0, 0, szi.Width(), szi.Height());
    {
        CAutoTextureRender render;
        render.SetMapping(tex, rc.TopLeft(), wnd.GetDPI());
        render->Clear(TMRGB(255,0,255));
        render->ClipAndPush(rcx);
        TMUInt uFlag = PRF_NONCLIENT | PRF_ERASEBKGND | PRF_CLIENT | PRF_BIND | PRF_FLOAT;
        if (bRenderChild)
            uFlag |= PRF_CHILDREN;
        if (!Edit_IsShowHide(true))
            uFlag |= PRF_CHECKVISIBLE;
        wnd.CopyWindowImageEx(uFlag);
    }
}

// TMBool CObjData::RendThumbnail(IGXApp* gapp, const TM::CTuiRect& rc, EThumb eThumb)
// {
//     if (eThumb == IObjEdit::eThumbForTree)
//         return false;
// 
//     CTexture tex;
//     _CObjData_RendSample(gapp, tex, 0, m_hWnd, eThumb != this->eRenderNoChild);
//     if (tex)
//     {
//         float w = tex->GetWidth();
//         float h = tex->GetHeight();
//         float rt1 = w / h;
//         CTuiRect rcx(rc);
//         rcx.ScaleDownTo(rt1);
//         unsigned int uFlag = 0;//(m_hWnd.GetExStyle() & WS_ADV_LAYERED) ? 0 : GX_BLT_SRCCOPY;
//         tex->Blt(rcx, CRect(0,0,0,0), uFlag | GX_BLT_DFTSRCWH, 255);
//     }
//     return false;
// }

int CObjData::RendSample(IGXApp* gapp, int nState, const CTuiRect& rcDest, ERender eRender)
{
    if (eRender == IObjEdit::eRenderForTree)
        return -1;

    DbgAssert(m_hWnd);
    CTexture tex;
    if (eRender == eRenderForView)
    {
        _CObjData_RendSample(gapp, tex, &rcDest, m_hWnd, true);
        if (tex)
        {
            unsigned int uFlag = (m_hWnd.GetExStyle() & WS_ADV_LAYERED) ? 0 : GX_BLT_SRCCOPY;
            tex->Blt(rcDest, CRect(0,0,0,0), uFlag | GX_BLT_DFTSRCWH, 255);
            return 0;
        }
    }
    else
    {
        _CObjData_RendSample(gapp, tex, 0, m_hWnd, eRender != this->eRenderThumbNoChild);
        if (tex)
        {
            float w = tex->GetWidth();
            float h = tex->GetHeight();
            float rt1 = w / h;
            CTuiRect rcx(rcDest);
            rcx.ScaleDownTo(rt1);
            unsigned int uFlag = 0;//(m_hWnd.GetExStyle() & WS_ADV_LAYERED) ? 0 : GX_BLT_SRCCOPY;
            tex->Blt(rcx, CRect(0,0,0,0), uFlag | GX_BLT_DFTSRCWH, 255);
            return 0;
        }
    }
    return -1;
}

// int CObjData::RendSample(IGXApp* gapp, int nState, const CTuiRect& rcDest)
// {
//     DbgAssert(m_hWnd);
//     CTuiWidget wnd(m_hWnd);
// 
//     TM::CTuiRect rc;
//     wnd.GetWindowRect(&rc);
//     if (rc.Left() != 0 || rc.Top() != 0)
//     {
//         wnd.SetWindowPos(0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
//         wnd.GetWindowRect(&rc);
//     }
// 
//     CSizeF szf(rcDest.Width(), rcDest.Height());
//     CSizeI szi;
//     wnd.GetDPI().LPtoDPCeil(szf, szi);
// 
//     CTexture texture;
//     texture.CreateRenderTarget(szi.cx, szi.cy);
// //    texture.CreateIndirect(szi.cx, szi.Height(), 32);
//     if (texture)
//     {
//         CRect rcx(0, 0, szi.Width(), szi.Height());
//         {
//             CAutoTextureRender render;
//             render.SetMapping(texture, rc.TopLeft(), wnd.GetDPI());
//             //gxMapLogicRect(rcx, rc);
// //            gxSetMapping(GXPoint(0,0), rc.TopLeft(), wnd.GetDPI());
//             wnd.CopyWindowImage(!Edit_IsShowHide(true), PRF_BIND | PRF_FLOAT);
//         }
// //         gxTextureBltToDC(CWindowDC(0), CRect(0, 0, 0, 0), texture, rcx, GX_BLT_SRCCOPY | GX_BLT_DFTDSTSRCWH, 255);
//         
//         unsigned int uFlag = (wnd.GetExStyle() & WS_ADV_LAYERED) ? 0 : GX_BLT_SRCCOPY;
//         texture->Blt(rcDest, rcx, uFlag | GX_BLT_DFTSRCWH, 255);
//     }
// 
//     return 0;
// }

CString CObjData::GetObjClassName()
{
    return GetProp()->GetClassName();
}

void CObjData::InvalidateObj()
{
    if (!m_hWnd)
        return;
    m_hWnd.ChangeFrame(); // .Invalidate();
    m_hWnd.Invalidate();
    CTuiWidget wndParent(m_hWnd.GetParent());
    if (wndParent)
        wndParent.ChangeFrame();
}

TMBool CObjData::SetObjRect(const CTuiRect& rc)
{
    if (m_hWnd)
    {
        m_hWnd.m_hWnd->Set_WS_PRI_DIRTY();
        m_hWnd.m_hWnd->MoveWindow(&rc);
        return true;
    }
    return false;
}


TMUInt64 CObjData::GetId()
{
    DbgAssert(GetProp());
    return GetProp()->GetObjectID();
}

TMBool CObjData::MoveItem(IObjEdit* pPos, EMoveParam mp)
{
    if (!pPos)
        return false;

    EObjType eType = GetType();
    if (eType != TM::e_otWgt)
        return false;

    TM::CTuiWgt* pw1 = (TM::CTuiWgt*)GetProp();
    TM::CTuiWgt* pw2 = (TM::CTuiWgt*)(static_cast<CObjData*>(pPos))->GetProp();

    eType = pPos->GetType();
    if (!CTuiWidget(pw1).SetWindowZOrder(pw2, mp))
        return false;

    CPanelApp* pView = GetApp();
    pView->GetProjMgr()->ItemMoveOrAdd(pPos, mp, this);
    return true;
}

TM::IDomElement* CObjData::SaveTo(TM::IDomNode* pParent)
{
    TM::CTuiObject* p = GetProp();
    if (!p)
        return 0;
    CSaveParam sp(SS_ALL, 0, 0);
    CSaveStream st(pParent, p, sp);
    p->SaveProp(st);
    return st;
}

void CObjData::OnLoad(TM::IDomElement* pOld, TM::IDomElement* pNew)
{
    // compare property, delete unused attrib and same attrib
    CComPtr<IDomNodeList> ptrList(pOld->get_attributes());
    for (int i = 0; ; ++i)
    {
        CComQIPtr<IDomAttr> p = ptrList->item(i);
        if (!p)
            break;
        CDomStrView strName(p->get_nodeName());
        CDomStrPtr strVal;
        if (!pNew->getAttribute(strName, strVal))
            pOld->removeAttributeNode(p);
        else if (strVal == p->get_nodeValue())
            pOld->removeAttributeNode(p);
    }

    GetProp()->OnLoadProp (pOld);
}


struct CSyncAncestor
{
    typedef TM::CSet<IObjEdit*> CIDList;
    
    static void InitID(CPanelApp& view, CIDList& aid, IDomNode* ptr, TMBool bRoot)
    {
        if (!ptr)
            return;
        int id = GetDomObjectID(ptr);
        IObjEdit* p = view.GetDataByObjID(id);
        if (p)
            aid.insert(p);
        InitID(view, aid, ptr->get_firstChild(), false);
        if (!bRoot)
            InitID(view, aid, ptr->get_nextSibling(), false);
    }
    
    static void RemoveUnusedItem(CPanelApp& view, CIDList& aid, IObjEdit* p, TMBool bRoot)
    {
        if (!p)
            return;
        RemoveUnusedItem(view, aid, view.GetProjMgr()->ItemGetChild(p), false);
        if (!bRoot)
            RemoveUnusedItem(view, aid, view.GetProjMgr()->ItemGetNext(p), false);
        CIDList::recorder rec(aid);
        rec.find(p);
        if (rec)
        {
            view.GetProjMgr()->ItemRemove(p);
            rec.erase();
        }
    }

    enum ECreateMode
    {
        e_cmCopy, // copy all item
        e_cmSync, // sync all item, only this mode can have param "paID"
        e_cmLoad  // if have same item then copy item, else sync item
    };
    static IObjEdit* CreateSimpleObjectByDom(CPanelApp& view, IObjEdit* pParent, TM::IDomNode* pXml, ECreateMode eMode, CIDList* paID)
    {
        CAutoMsgAnalysis ama1("CreateSimpleObjectByDom::1");
        CDomElementPtr ptr(pXml);
        CDomStrPtr strClass;
        if (!ptr->getAttribute(L"class", strClass)) // delete object
            return 0;

        CAutoMsgAnalysis ama2("CreateSimpleObjectByDom::2");
        int id = GetDomObjectID(pXml);
        IObjEdit* pData = 0;

//         if (id == 56508)
//         {
//             int x = 0;
//         }
        CAutoMsgAnalysis ama3("CreateSimpleObjectByDom::3");
        ECreateMode eOldMode = eMode;
        if (eMode != e_cmCopy)
        {
            pData = view.GetDataByObjID(id);
            
            if (eMode == e_cmLoad)
                eMode = pData ? e_cmCopy : e_cmSync;
        }

        CAutoMsgAnalysis ama4("CreateSimpleObjectByDom::4");
        if (eMode == e_cmCopy)
        {
            CStringW strMicro;
            int idNew = view.m_idMgr.CreateStepObjectID(EM_NULL, strClass.c_str(), strMicro);
            if (eOldMode == e_cmLoad)
            {
                CString strOldMicro(view.m_idMgr.ID2Text(id));
                view.GetProjMgr()->Alert(TM_T("Convert macro %s to %s\r\n"), strOldMicro.c_str(), strMicro.c_str());
            }
            id = idNew;
        }
        else if (pData)
        {
            if (pData->GetObjClassName() != strClass)
                eMode = e_cmCopy; // need recreate object
            else if (paID)
                paID->erase(pData);
        }

        CAutoMsgAnalysis ama5("CreateSimpleObjectByDom::5");
        if (!pData || eMode == e_cmCopy)
        {
            DbgAssert(ptr);

            CModule* pModule = view.GetEditorModule();
            CTuiObject* pWgtParent = (pParent > (void*)10) ? ((CObjData*)pParent)->GetProp() : 0;
            
            CAutoMsgAnalysis ama6("CreateSimpleObjectByDom::6");
            DbgAssert(id);
//             DbgAssert(!view.GetDataByObjID(id)); // copy mode can true
            CTuiObject* pM = pModule->CreateByClass(e_otUnknown, pWgtParent, strClass, id);
            if (!pM)
            {
                IProjMgr* pLog = view.GetProjMgr();
                pLog->Alert(L"can't create class %s(%d);\r\n", strClass.c_str(), id);
                return 0;
            }
            
            CAutoMsgAnalysis ama7("CreateSimpleObjectByDom::7");
            EObjType eType = pM->GetType();
            view.m_idMgr.AutoCreateMacroByID(id, strClass.c_str(), (EDIT_MODE)(1 << eType));

            // check command id
            long cid;
            if (ptr->getAttributeLong(L"id", cid))
            {
                if (cid > 65535)
                {
                    IProjMgr* pLog = view.GetProjMgr();
                    CString strU;
                    TM::CString strM(view.m_idMgr.ID2Text(cid, &strU));
                    pLog->Alert(L"warn: command id greater than 65535: %d(%s); %s\r\n", cid, strM.c_str(), strU.c_str());
                }
            }

            // load xml
            CAutoMsgAnalysis ama75("CreateSimpleObjectByDom::LoadAll");
            pM->LoadAll(ptr, 0);

            CAutoMsgAnalysis ama8("CreateSimpleObjectByDom::8");
            switch (eType)
            {
            case e_otWnd:
                {
                    CAutoMsgAnalysis ama81("CreateSimpleObjectByDom::81");
                    pData = new CEditorObject<CManagerEditor>(view, pParent, pM);
                    (static_cast<CTuiWnd*>(pM))->InitManager(0, 0);
                }
                break;
                
            case e_otPic:
                {
                    CAutoMsgAnalysis ama82("CreateSimpleObjectByDom::82");
                    pData = new CEditorObject<CPicEditor>(view, pParent, pM);
                    pM->Release();
                }
                break;
                
            case e_otFrm:
                {
                    CAutoMsgAnalysis ama83("CreateSimpleObjectByDom::83");
                    pData = new CEditorObject<CFrameEditor>(view, pParent, pM);
                    pM->Release();
                }
                break;
                
            case e_otWgt:
                {
                    CAutoMsgAnalysis ama84("CreateSimpleObjectByDom::84");
                    pData = new CEditorObject<CWinData>(view, pParent, pM);
                    (static_cast<CTuiWgt*>(pM))->InitConstruct(0, 0);
                }
                break;
            }

            CAutoMsgAnalysis ama9("CreateSimpleObjectByDom::9");
            view.GetProjMgr()->ItemAdd(pParent, eZOrderChildLast, pData, eType);
            DbgAssert(pData);
            return pData;
        }

        CAutoMsgAnalysis ama10("CreateSimpleObjectByDom::10");
        CObjData* pObj = static_cast<CObjData*>(pData);
        pObj->MoveItem(pParent, (EMoveParam)eZOrderChildLast);
        pObj->GetProp()->LoadAll(ptr, 0);
        return pData;
    }

    // root item: itmParent == 0 && itmPrev == 0;
    static IObjEdit* SyncItem(CPanelApp& view, IObjEdit* itmParent, IDomNode* ptr, ECreateMode eMode, TMBool bRoot, CIDList* paID)
    {
        if (!ptr)
            return 0;
        
        // sync first child
        IObjEdit* pData;
        {
            CAutoMsgAnalysis ama("SyncItem::CreateSimpleObjectByDom");
            pData = CreateSimpleObjectByDom(view, itmParent, ptr, eMode, paID);
        }
        if (pData)
        {
//             {
//                 CAutoMsgAnalysis ama2("SyncItem::AfterInit");
//                 AfterInit((static_cast<CObjData*>(pData)), CDomElementPtr(ptr));
//             }
            
            // sync child and next
            SyncItem(view, pData, ptr->get_firstChild(), eMode, false, paID);
        }
//         else
//             pData = itmPrev; // delete current node;
        
        if (!bRoot) // root item don't sync next item.
            SyncItem(view, itmParent, ptr->get_nextSibling(), eMode, false, paID);
        return pData;
    }

    static void LoadObject(CPanelApp& view)
    {
        TM::CModule* pModule = view.GetModule(HEDITOR);
        if (!pModule->m_aXmls.size())
            return;
        IDomDocument* doc = pModule->m_aXmls.begin()->second->get_ownerDocument();

        for (CDomElementPtr ptr(doc->get_firstChild()); ptr; ptr = ptr->get_nextSibling())
        {
            if (ptr->get_tagName() == TUI_XML_NODE0_TMDUI)
            {
                for (CDomElementPtr ptrc(ptr->get_firstChild()); ptrc; ptrc = ptrc->get_nextSibling())
                {
                    SyncItem(view, 0, ptrc->get_firstChild(), CSyncAncestor::e_cmLoad, false, 0);
                }
            }
        }
    }
};

// ptr haven't "class" means remove it.
IObjEdit* CPanelApp::SyncAncestor(TM::IDomNode* ptrOld, TM::IDomNode* ptrNew)
{
    CDomElementPtr ptrRoot(ptrNew);
    if (!ptrRoot)
        return 0;
    
    int id = GetDomObjectID(ptrNew);
    IObjEdit* itmObj = GetDataByObjID(id);

    CSyncAncestor::CIDList aID;
    if (itmObj && ptrOld)
        CSyncAncestor::InitID(*this, aID, ptrOld, true);
    IObjEdit* p = CSyncAncestor::SyncItem(*this, 0, ptrNew, CSyncAncestor::e_cmSync, true, &aID);
    for (; aID.size();)
        CSyncAncestor::RemoveUnusedItem(*this, aID, *aID.begin(), true);

    return p;
}

IObjEdit* CPanelApp::PasteTo(IObjEdit* pParent, TM::IDomNode* ptr)
{
    return CSyncAncestor::SyncItem(*this, pParent, ptr, CSyncAncestor::e_cmCopy, true, 0);
}

TMBool CPanelApp::LoadXmlFile(const TM::CString& strXml, TMBool bMerge)
{
    //     LoadExistentResource();
    if (bMerge)
    {
        DbgAssert(!"current version not support merge");
//         TM::CModule* mdl = pSession->GetModule(HEDITOR);
//         static TM::CStringW str;
//         str.Swap(mdl->m_strXml);
    }
    else
    {
        UnLoadXml(HEDITOR);
        CreateEditorModule();
    }

    {
        CAutoMsgAnalysis ama("CPanelApp::LoadXmlFile::LoadXml");
        LoadXml(HEDITOR, strXml, 0);
    }

//    pSession->Editor_LoadXml(HEDITOR, strXml);
    
    CSyncAncestor::LoadObject(*this);

    return TMTrue;
}

void CPanelApp::LoadLanguage(LPCTSTR pFileName)
{
        // backup base object
//     TM::CSaveStream::SaveBaseObject();
    CView::LoadXml(HEDITOR, 0, pFileName);
}



IObjEdit* CPanelSpy::SyncSpyItem(TM::CTuiObject* parent, TM::CTuiObject* pM)
{
    if (!pM)
        return 0;
    
    int id = pM->GetObjectID();
    DbgAssert(id);
    
    EObjType eType = pM->GetType();
    CObjData* pData = 0;
    IObjEdit* pParent = 0;
    HTUI hWin = 0;
    switch (eType)
    {
    case e_otWnd:
        pData = new CEditorObject<CManagerEditor>(*this, pParent, pM);
        hWin = static_cast<HTUI>(pM);
        break;
        
    case e_otPic:
        pData = new CEditorObject<CPicEditor>(*this, pParent, pM);
        break;
        
    case e_otFrm:
        pData = new CEditorObject<CFrameEditor>(*this, pParent, pM);
        break;
        
    case e_otWgt:
        hWin = static_cast<HTUI>(pM);
        pData = new CEditorObject<CWinData>(*this, pParent = parent->m_pEditData, pM);
        break;
    }
    
    DbgAssert(pData);
    pData->m_bSpyMode = true;
    this->GetProjMgr()->ItemAdd(pParent, eZOrderChildLast, pData, eType);

    if (hWin)
    {
        SyncSpyItem(hWin, hWin->GetChild());
        SyncSpyItem(parent, hWin->GetNext());
    }

    return pData;
}






