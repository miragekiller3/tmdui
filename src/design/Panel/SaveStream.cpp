#include "StdAfx.h"
#include "Panel.h"
#include "FactoryData.h"
#include "WndPad.h"

using namespace TM;

TM_BGN_NAMESPACE


class CPreDom
{
private:
    typedef TM::CMap<TM::CStrPtr, IDomElement*> CDomList;
    TM::CDomDocument    m_doc;
    CDomList            m_aDom;

public:
    CPreDom()
    {
    }

    IDomElement* GetPreElement(CTuiObject* pObj)
    {
        DbgAssert(pObj);
        IDomElement*& p = m_aDom[pObj->GetClassName()];
        if (!p)
        {
            // create default prop list
            CFactoryInfo* pf = pObj->m_pFactoryInfo;
            CTuiObject* p2 = pf->m_pFactory->FCreate(0, 0, pf, TUI_DPI_SYSTEM_DEFAULT);
            DbgAssert(p2);
            CSaveParam sp(SS_ALL & ~SS_CLASSNAME, 0, 0);
            CSaveStream stmDft(m_doc, p2, sp);
            p2->SaveProp (stmDft);
//             --p2->m_nRef;
            pf->m_pFactory->FDestroy (p2);
            p = stmDft;
        }
        return p;
    }

    TMBool IsSameAttrib(CTuiObject* pObj, const TM::CWeakStr& key, const TM::CWeakStr& val)
    {
        IDomElement* p = GetPreElement(pObj);
        CDomStrPtr strVal;
        if (p->getAttribute(key, strVal))
            return strVal == val;
        else
            return false;
    }
};

static CPreDom& GetPreDom()
{
    static CPreDom s_preDom;
    return s_preDom;
}


static TM::CString SSGetTagName(CTuiObject* p)
{
    int nid = p->GetObjectID();
    return std::move(VA2T(TM_T("L%d"), nid));
}

CSaveStream::CSaveStream(IDomNode* pParent, CTuiObject* pCur, CSaveParam& sp)
    : m_sp (sp)
    , m_pObj (pCur)
{
    m_sp.RegistID(pCur->GetObjectID());

    DbgAssert(pCur);
    DbgAssert(pParent);
    base_class& ptr = *this;
    ptr = pParent->get_ownerDocument()->appendChildElement(pParent, SSGetTagName(pCur));
    if (m_sp.m_nFilter & SS_CLASSNAME)
        (*this)->setAttribute(TM_CL("class"), pCur->GetClassName());
}

CSaveStream::~CSaveStream()
{
    // remove empty node
    if ((m_sp.m_nFilter & SS_SKINMODE) && !(*this)->hasAttributes())
        (*this)->removeChild((*this));
}

void CSaveStream::ExecChilds(CTuiWgt* ph)
{
//     if (p->GetObjectID() == 7113)
//     {
//         int x  = 0;
//     }

    for (CTuiWgt::CWndRec rec(ph->GetChilds()); rec; ++rec)
    {
        HTUI hc = *rec;
        IDomNode* pNode = (*this);
        if (m_sp.m_nFilter & SS_SKINMODE)
            pNode = pNode->get_parentNode();
        CSaveStream ss(pNode, hc, m_sp);
        hc->SaveProp(ss);
    }
}

bool _CSaveStream_IsSameProp(IDomElement* p, const CWeakStr& name, const void* pMem, IPropItemSaver* samer)
{
    CDomStrPtr strVal;
    if (p->getAttribute(name, strVal))
    {
        if (samer->EqualString(pMem, strVal))
            return true;
    }
    return false;
}

void CSaveStream::SaveProp(const CWeakStr& name, const void* pToken, const void* pMem,  IPropItemSaver* saver, int nType)
{
    CAutoSaveAnalysis _ama("CSaveStream::_SaveString");
    
    // check remove duplicate prop
    CStrPtr& str = m_aToken[pToken];
    if (!str.IsEmpty())
        (*this)->removeAttribute(str);
    
    str = name;
    
    if (!(m_sp.m_nFilter & nType))
        return;
    
    // check remove default prop
    CAutoSaveAnalysis _ama1("CSaveStream::_SaveString 1");
    if ((m_sp.m_nFilter & SS_REMOVESAME))
    {
        IDomElement* p = GetPreDom().GetPreElement(m_pObj);
        if (p && _CSaveStream_IsSameProp(p, name, pMem, saver))
        {
            (*this)->removeAttribute(name);
            return;
        }
    }
    
    // check is same prop
    CAutoSaveAnalysis _ama2("CSaveStream::_SaveString setAttribute");
    CDomStrView strVal;
    if (_CSaveStream_IsSameProp(*this, name, pMem, saver))
        return;
    
    // save prop
    (*this)->setAttribute(name, saver->T2Str(pMem));
}

TMBool CSaveStream::_SavePropID(const CWeakStr& name, const void* pToken, TMUInt32 __val, int nType)
{
    m_sp.RegistID(__val);
//    return SaveProp(name, pToken, &__val, CProp_long<TMUInt32>::Inst(), nType), false;
    return _SavePropLong(name, pToken, __val, nType);
}

TMBool CSaveStream::ExecFile(const CWeakStr& name, CString& __val)
{
    if (m_sp.m_pSaveFile)
        m_sp.m_pSaveFile->RegisterFile(__val);
    return _SaveString(name, &__val, __val, SS_FILE);
}

TMBool CSaveStream::ExecPic(const CWeakStr& name, CPicPtr& pic)
{
    if (!pic)
        return _SaveString(name, tm_addressof(pic), TM_CT("null"), SS_PIC);
    else
        return _SavePropID(name, tm_addressof(pic), pic->GetObjectID(), SS_PIC);
}

TMBool CSaveStream::ExecFrame(const CWeakStr& name, CFrmPtr& frm)
{
    if (!frm)
        return _SaveString(name, tm_addressof(frm), TM_CT(""), SS_FRAME);
    else
        return _SavePropID(name, tm_addressof(frm), frm->GetObjectID(), SS_FRAME);
}


TM_END_NAMESPACE


static void Editor_SaveFile(TM::CModule* mdl, EObjType eType, IDomNode* pParent, CSaveParam& sp)
{
    CAutoSaveAnalysis _ama("Editor_SaveFile");

    for (CModule::CObjList::recorder rec(mdl->m_aObjs); rec; ++rec)
    {
        CTuiObject* p = static_cast<CTuiObject*>(rec.get_data());
        
        // m_dwObjID==0 is temp object for editer
        if (p->GetObjectID() != 0 && p->GetType() == eType)
        {
            CSaveStream stream(pParent, p, sp);
            p->SaveProp(stream);
            //CDomElement::CPtr nodec(node.appendChildElement())

        //    SaveAll (p, node, dwFlag);
        }
    }
}

static void Editor_SaveFile_Sel(TM::CModule* mdl, EObjType eType, IDomNode* pParent, CSaveParam& sp)
{
    for (CModule::CObjList::recorder rec(mdl->m_aObjs); rec; ++rec)
    {
        CTuiObject* p = static_cast<CTuiObject*>(rec.get_data());
        TMUInt32 oid = p->GetObjectID();
        // m_dwObjID==0 is temp object for editer
        if (oid == 0)
            continue;
        EObjType eCurType = p->GetType();
        if (eCurType == e_otWgt)
        {
            if (sp.m_pSaveList->find(oid) != sp.m_pSaveList->end())
                sp.RegistID((TMUInt32)p->m_pEditData->GetAncestorItem()->GetId());
            continue;
        }
        if (eCurType != eType)
            continue;
        if (sp.m_pSaveList->find(oid) != sp.m_pSaveList->end()) // || p->m_pEditData->GetApp()->GetObjMgr()->GetItemState(p->m_pEditData) & TVIS_SELECTED)
        {
            CSaveStream stream(pParent, p, sp);
            p->SaveProp(stream);
        }
    }
}

void CPanelApp::SaveSkin(LPCTSTR pFileName, TMUInt uMode, IDomDocument** xmlOut)
{
    TMBool bLang = (eSaveModeLang == (uMode & eSaveModeMask));
    TMULong dwFlag = SS_SKINMODE | SS_REMOVESAME;
    if (bLang)
        dwFlag |= (SS_TEXT);
    
    TM::CModule* mdl = GetEditorModule();
    
    CDomDocument doc;
    CDomElementPtr ptrNode = doc->appendChildElement(doc, TUI_XML_NODE0_TMDUI);
    ptrNode->setAttribute(TUI_XML_PROP_VERSIOM, TUI_FILE_VER_STRING);
    ptrNode->setAttribute(TUI_XML_PROP_LCID, L"1033");
    
    IDomNode* ptrRes = doc->appendChildElement(ptrNode, TUI_XML_NODE1_RESTUI);
    CSaveParam sp(dwFlag, 0, 0);
    Editor_SaveFile (mdl, e_otPic, ptrRes, sp);
    Editor_SaveFile (mdl, e_otFrm, ptrRes, sp);
    Editor_SaveFile (mdl, e_otWnd, ptrRes, sp);

    if (bLang)
    {
        CDomElementPtr ptrRes2 = doc->appendChildElement(ptrNode, TUI_XML_NODE1_RESSYS);
//        SaveStringXml (mdl, ptrRes);
        CRcStrList* aStr = (uMode & eSaveLangStringTable) ? &mdl->m_aMessageLang : 0;
        CRcStrList* aMenuItem = (uMode & eSaveLangMenu) ? &mdl->m_aMenuItemLang : 0;
        CRcPopList* aMenuPop = (uMode & eSaveLangMenu) ? &mdl->m_aMenuPopLang : 0;

        m_fileRC.SaveStringXml(m_idMgr, aStr, aMenuItem, aMenuPop, ptrRes2);
    }

    CStringW str;
    doc->transformNode(str, true, 2);

//    CDomDocumentImpl::SaveXmlFile(pFileName, str);
    if (pFileName)
        DomSave(pFileName, str);

    if (xmlOut)
    {
        *xmlOut = doc;
        (*xmlOut)->AddRef();
    }


//     TM::CFile file;
//     file.CreateAlways (pFileName, file.e_faWrite);
//     TM::CStringA strUtf8 (TM_T2UTF(str));
//     file.Write (strUtf8, strUtf8.GetLength());
}

template <class F>
static TMBool _CPanelApp_SaveXml(CDomDocument& doc, IDomElement* ptrNode, TM::CModule* mdl, F __f, CSaveParam& sp)
{
    CAutoSaveAnalysis _ama("_CPanelApp_SaveXml");
    __f (mdl, e_otPic, doc->appendChildElement(ptrNode, TM_CS(TUI_XML_NODE1_PIC)), sp);
    __f (mdl, e_otFrm, doc->appendChildElement(ptrNode, TM_CS(TUI_XML_NODE1_FRM)), sp);
    __f (mdl, e_otWnd, doc->appendChildElement(ptrNode, TM_CS(TUI_XML_NODE1_WND)), sp);
    return true;
}


static void _CPanelApp_AddSel(TM::CModule* mdl, CIDUsedList& id)
{
    for (CModule::CObjList::recorder rec(mdl->m_aObjs); rec; ++rec)
    {
        CTuiObject* p = static_cast<CTuiObject*>(rec.get_data());
        CObjData* pObj = p->m_pEditData;
        if (pObj && (pObj->GetApp()->GetProjMgr()->ItemGetState(pObj) & TVIS_SELECTED))
            ++id[pObj->GetAncestorItem()->GetId()];
    }
}

TMBool CPanelApp::SaveXml(const TM::CString& strPath, TM::CStringW& str, CIDUsedList* aID, ESaveMode eMode)
{
    TM::CModule* mdl = GetEditorModule();

    CDomDocument doc;
    CDomElementPtr ptrNode = doc->appendChildElement(doc, TUI_XML_NODE0_TMDUI);
    ptrNode->setAttribute(TUI_XML_PROP_VERSIOM, TUI_FILE_VER_STRING);
    ptrNode->setAttribute(TUI_XML_PROP_ENCODING, L"utf-8");

    TMUInt32 SS_DEFAULT = (SS_ALL | SS_REMOVESAME);

    if (eMode == eSaveModeNormal)
    {
        CSaveFileRes sf(m_fileRC, m_fileQRC, strPath);
        CSaveParam sp(SS_DEFAULT, aID, &sf);
        _CPanelApp_SaveXml(doc, ptrNode, mdl, Editor_SaveFile, sp);
    }
//     else if (eMode == eSaveModeTest)
//         _CPanelApp_SaveXml(doc, ptrNode, mdl, Editor_SaveFile, 0);
    else
    {
        DbgAssert(eMode == eSaveModeSel);
//         CIDUsedList aIDSave;
        _CPanelApp_AddSel(mdl, *aID);
        CSaveParam sp(SS_DEFAULT, aID, 0);
        for (int n = -1; ; n = aID->size())
        {
            _CPanelApp_SaveXml(doc, ptrNode, mdl, Editor_SaveFile_Sel, sp);
            if (n == aID->size())
                break;
            ptrNode->removeAllChild();
        }
    }

    CAutoSaveAnalysis _ama("transformNode");
    doc->transformNode(str, true, 2);
    
    return true;
}


