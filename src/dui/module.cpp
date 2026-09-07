/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2008.10.15
 *
 *  Copyright (C) 2008 miragekiller
 */

#include "module.h"
#include "../tmtxtfile.h"
#include "uiwidget.h"
#include "view.h"


TM_BGN_NAMESPACE


//////////////////////////////////////////////////////////////////////////////
//predefined object map
class CPreList : public CModule::CObjList
{
public:
    static CPreList& GetMap()
    {
        static CPreList g_mapPre;
        return g_mapPre;
    }
};


//////////////////////////////////////////////////////////////////////////////
//predefined factory function map
class _CPfnList : public CModule::CFactoryList
{
public:
    TMBool Register(CFactoryInfo::_Class* p)
    {
        DbgAssert(this->find(p->strClass) == this->cend());
        CFactoryInfo f(p);
        return insert(f).second;
    }

    static _CPfnList& GetMap()
    {
        static _CPfnList g_map;
        return g_map;
    }
};

TMBool CPfnMap::RegisterPfn(CFactoryInfo::_Class* p)
{
    return _CPfnList::GetMap().Register(p);
}


////////////////////////////////////////////////////////////////////////////////
// class CModule
CModule::CModule(CView* pOwner, TMHModule hRes)
    : m_hRes(hRes)
    , m_pOwner(pOwner)
    , m_pColorTransform(0)
    , m_aPrePic(*this)
{
    DbgTrace("CModule::CModule(0x%X)", this);

    m_aFactory = _CPfnList::GetMap();

    m_ptrFocus = &m_aPrePic.m_picDftFocus;

    for (CFactoryList::recorder rec(m_aFactory); rec; ++rec)
        (const_cast<CFactoryInfo&>(rec.get_data())).m_pModule = this;
}

CModule::~CModule()
{
    DbgTrace("CModule::~CModule(0x%X)", this);
    //DbgAssert(!m_aObjs.size());
    ClearXmls();
}

template <class T>
TMBool _CModule_Term(T& alist)
{
    for (typename T::recorder rec(alist); rec; ++rec)
    {
        CTuiObject* p = static_cast<CTuiObject*>(rec.get_data());
        if (p->GetType() == e_otWnd)
        {
            HTUI hWnd = static_cast<HTUI>(p);
            if (!hWnd->IsLifeDestroyed())
            {
                hWnd->DestroyWindow();
                return false;
            }
        }
    }
    return true;
}

void CModule::TermAndRelease()
{
    while (!_CModule_Term(m_aObjs))
    {
    }
    Release();
}

CModule::CObjList::recorder CModule::_FindObj(TMUInt32 id, int dpi)
{
    CObjList::recorder rec2(m_aObjs);
    TUIOBJID oid = { id, dpi };
    rec2.find(static_cast<CTuiObject*>(&oid));
    return rec2;
}

CTuiObject* CModule::CreateByClass(EObjType eType, CTuiObject* pParent, const CString& pClass, int nID, int dpi, TMBool bForceMultiDPI, TMBool bForceUnshare)
{
    DbgAssert(pClass);

    CFactoryList::recorder rec(m_aFactory);
    rec.find(pClass);
    if (!rec)
    {
        DbgTrace("CModule::CreateByClass(%s(%d)) failed", TM_T2A(pClass), nID);
        return 0;
    }

    CFactoryInfo* pi = &rec.get_data();
    if (eType != e_otUnknown && eType != pi->m_pFactory->eType)
    {
#ifndef TUI_TEST_MODE
        DbgAssert(0);
#endif
        return 0;
    }
  
    TMBool bMultiDpi = bForceMultiDPI || pi->m_pFactory->bMultiDPI;
    TMBool bShare = pi->m_pFactory->bShare && !bForceUnshare;
    int nDPI = (bMultiDpi || !bShare) ? dpi : 0;
    CTuiObject* p = pi->m_pFactory->FCreate(pParent, nID, pi, nDPI);
    m_aObjs.insert(p);
    pi->m_pModule->AddRef();
    return p;
}

TM_BGN_ENUM_SYMBOL(_EN_EObjType)
    TM_ENUM_SYMBOL(e_otWnd)
    TM_ENUM_SYMBOL(e_otFrm)
    TM_ENUM_SYMBOL(e_otPic)
    TM_ENUM_SYMBOL(e_otWgt)
TM_END_ENUM_SYMBOL()

CTuiObject* CModule::InternalCreateObject(EObjType eType, CTuiObject* pParent, int nIDObj, int dpi, TMBool bForceUnshare)
{
    DbgAssert(dpi);
    DbgTrace(L"CModule::InternalCreateObject(type:%s, pParent:0X%p, idObj:%d, dpi:%d, forceUnshare:%d)",
        CEnumSymbolT<_EN_EObjType>::GetName(eType), pParent, nIDObj, dpi, bForceUnshare);
    
    if (eType == e_otPic || eType == e_otUnknown)
    {
        CTuiObject* pObj = m_aPrePic.find(nIDObj);// CPreMap::GetPreObject(nIDObj);
        if (pObj)
            return pObj; // predefined object needn't ref and dpi.
    }
    
    // if share object use old object, not create
    // CustomObject::nIDObj == 0; so skip it.
    if (!bForceUnshare)
    {
        CObjList::recorder rec(_FindObj(nIDObj, dpi));
        if (rec)
        {
            CTuiObject* p = static_cast<CTuiObject*>(rec.get_data());
            DbgAssert(p && p->m_pFactoryInfo);
            if (p->m_pFactoryInfo->m_pFactory->bShare)
            {
                p->AddRef();
                return p;
            }
        }
    }
    
    // create new object
    CXmlList::recorder rec2(m_aXmls);
    rec2.find(nIDObj);
    if (rec2)
    {
        IDomElement* x1 = rec2.get_data();
        CDomStrPtr str;
        x1->getAttribute(L"class", str);
        if (str == TM_T2W(TUI_CLASS_REFER))
        {
            long nNewID = 0;
            x1->getAttributeLong(L"Target", nNewID);
            return InternalCreateObject(eType, pParent, nNewID, dpi, bForceUnshare);
        }
        
        long bMultiDPI = false;
        x1->getAttributeLong(L"MultiDPI", bMultiDPI);
        CTuiObject* pObj = CreateByClass(eType, pParent, str, nIDObj, dpi, bMultiDPI, bForceUnshare);
        if (pObj)
        {
            pObj->LoadAll(x1, SS_CHILD);
            return pObj;
        }
    }
    
    return 0;
}

void CModule::EraseObj(CTuiObject* obj)
{
    CObjList::recorder rec(_FindObj(obj->GetObjectID(), 0));
    DbgAssert(rec);
    for (; ; ++rec)
    {
        CTuiObject* p = rec.get_data();
        DbgAssert(p->GetObjectID() == obj->GetObjectID());
        if (p == obj)
        {
            rec.erase();
            return;
        }
    }
}

template <class T>
struct _Module_Dispatch
{
    template <class T_List>
    static void Done(T_List& aList, TMUInt _m, TMWParam _w, TMLParam _l)
    {
        int n = aList.size();
        T* p = new T[n];
        int i = 0;
        for (typename T_List::recorder rec(aList); rec; ++rec, ++i)
        {
            p[i] = static_cast<T>(rec.get_data());
            p[i]->AddRef();
        }
        for (i = 0;  i < n; ++i)
        {
            p[i]->m_pFactoryInfo->m_pFactory->FDispatch(p[i], _m, _w, _l);
            p[i]->Release();
        }
        delete []p;
    }
};

// CM_LANGUAGECHANGED, CM_COLORVISUAL
void CModule::DispatchObject(TMUInt msg, TMWParam wp, TMLParam lp)
{
    if (CM_RENDERCHANGED == msg)
        m_aPrePic.Reclaim();

    _Module_Dispatch<CTuiObject*>::Done(m_aObjs, msg, wp, lp);
}

#if !TM_ANDROID
CComPtr<IDomDocument> CModule::XmlToDoc(TMCStr pFile, CString* pfn)
{
    CAutoMsgAnalysis awm("CModule::XmlToDoc");

    CString strXml;
    if (pFile[0] == '<')
    {
        strXml = pFile;
    }
    else
    {
#ifdef TUI_EDIT_MODE
        ETXTFILESTYLE tfs = e_TxtFileStyleUnknown;
        if (!m_hRes)
            tfs = TxtFileLoadAs(pFile, strXml, true);
        if (tfs != e_TxtFileStyleUnknown)
        {
            if (pfn)
            {
                CFnView rg2(pFile);
                *pfn = std::move(rg2.GetPath(true));
            }
        }
        else
#endif
        {
            CResource res;
            res.Load(m_hRes, pFile, (TMCStr)23); // RT_HTML;
            if (res)
                TxtFileConvert((const char*)res.GetData(), res.GetSize(), strXml, true);
        }
    }
    
    CAutoMsgAnalysis awm1("CModule::XmlToDoc::Edit_ConvertXmlVersion");
    Edit_ConvertXmlVersion(*this, strXml);

    CAutoMsgAnalysis awm2("CModule::XmlToDoc::LoadXml");
    CDomDocument doc;
    doc->LoadXml(strXml);
    return doc;
}


#endif


void CModule::_LoadFile(IDomNode* tag)
{
    for (IDomNode* ptr = tag->get_firstChild(); ptr; ptr = ptr->get_nextSibling())
    {
        int nID = GetDomObjectID(ptr);
        if (nID)
        {
            CDomElementPtr _ptr(ptr);
            m_aXmls[nID] = _ptr;
        }
        _LoadFile(ptr);
    }
}

template <class T> 
void _CModule_FillStrList(T& aList, IDomNode* element)
{
    if (!element)
        return;
    for (IDomNode* p = element->get_firstChild(); p; p = p->get_nextSibling())
    {
        CDomElementPtr pe(p);
        if (!pe)
            continue;
        CGXStrPtr str(pe->get_nodeName());
        CDomStrView sr = str.Mid(1);
        int nID = sr.ToLong();
        aList[nID] = pe->getAttribute(L"text");
    }
}

template <class T> 
void _CModule_FillPopList(T& aList, IDomNode* element)
{
    if (!element)
        return;

    for (IDomNode* p = element->get_firstChild(); p; p = p->get_nextSibling())
    {
        CDomElementPtr pe(p);
        if (!pe)
            continue;
        aList[pe->getAttribute(L"source")] = pe->getAttribute(L"text");
    }
}

static void _CModule_CheckVersion(IDomElement* node)
{
    if (Edit_IsPlugInModeEditor())
        return;
    CDomStrPtr strVersion;
    if (node->getAttribute(TUI_XML_PROP_VERSIOM, strVersion))
        DbgAssert(strVersion == TM_T2W(TUI_FILE_VER_STRING));
}

void CModule::LoadBaseXml(TMCStr pBase)
{
    DbgAssert(pBase);

    CAutoMsgAnalysis awm("CModule::LoadBaseXml");

    ClearXmls();

    CComPtr<IDomDocument> doc(XmlToDoc(pBase, &m_strXmlFilePath));
    DbgAssert(doc);

    CDomElementPtr node = doc->get_firstChild();
    if (node)
    {
        _CModule_CheckVersion(node);

        for (IDomNode* p = node->get_firstChild(); p; p = p->get_nextSibling())
        {
            CDomElementPtr ptr(p);
            if (!ptr)
                continue;
            CGXStrPtr strTag = ptr->get_tagName();
            if (strTag == TUI_XML_NODE1_PIC)
                _LoadFile (ptr);
            else if (strTag == TUI_XML_NODE1_FRM)
                _LoadFile (ptr);
            else if (strTag == TUI_XML_NODE1_WND)
                _LoadFile (ptr);
        }
    }

    if (!m_aXmls.empty())
    {
        IDomDocument* pDoc = doc;
        pDoc->AddRef();
    }
}

void CModule::LoadLangXml(TMCStr pLang)
{
    DbgAssert(pLang);
//    DbgAssert(!m_aXmls.empty()); user can only load language xml.

    CComPtr<IDomDocument> doc(XmlToDoc(pLang, 0));

    CDomElementPtr node = doc->get_firstChild();
    if (!node)
        return;
    
    _CModule_CheckVersion(node);

    long lid;
    if (node->getAttributeLong(TUI_XML_PROP_LCID, lid))
        m_local.Set(lid);
    
    CDomElementPtr ptrWinres = node->selectSingleNode(TUI_XML_NODE1_RESSYS);
    if (ptrWinres)
    {
        _CModule_FillStrList(m_aMessageLang, ptrWinres->selectSingleNode(TUI_XML_NODE2_STRING));
        _CModule_FillStrList(m_aMenuItemLang, ptrWinres->selectSingleNode(TUI_XML_NODE2_MENU));
        _CModule_FillPopList(m_aMenuPopLang, ptrWinres->selectSingleNode(TUI_XML_NODE2_MENUPOP));
    }
    
    IDomNode* ptrTuires = node->selectSingleNode(TUI_XML_NODE1_RESTUI);
    if (ptrTuires)
    {
        IDomNode* p = ptrTuires->get_firstChild();

        for (; p; p = p->get_nextSibling())
        {
            CDomElementPtr pe(p);
            if (!pe)
                continue;
            
            int nID = GetDomObjectID(pe);
            DbgAssert(nID > 0);
            CXmlList::recorder rec(m_aXmls);
            rec.find(nID);
            if (rec)
            {
                CDomAttributeListPtr al(pe->get_attributes());
                int x = al->get_length();
                for (int i = 0; i < x; ++i)
                {
                    CDomAttributePtr attrib = al->item(i);
                    rec.get_data()->setAttribute(attrib->get_name(), attrib->get_value());
                }

                CObjList::recorder rec2(_FindObj(nID, 0));
                for (; rec2 && rec2.get_key()->GetObjectID() == nID; ++rec2)
                {
                    CTuiObject* pObj = static_cast<CTuiObject*>(rec2.get_data());
                    pObj->LoadAll(pe, 0);
                }
            }
            else
            {
                DbgTrace("can't load skin object:%d", nID);
            }
        }
    }
    
    DispatchObject(CM_LANGUAGECHANGED, (TMWParam)pLang, m_local.m_lcID);
}

void CModule::LoadXml(TMCStr pBase, TMCStr pLang)
{
    if (pBase)
    {
        DbgAssert(!m_aXmls.size());
        LoadBaseXml(pBase);
    }
    if (pLang)
        LoadLangXml(pLang);
    return;
}

void CModule::ClearXmls()
{
    if (!m_aXmls.empty())
    {
        IDomDocument* p = m_aXmls.begin()->second->get_ownerDocument();
        m_aXmls.clear ();
        p->Release();
    }
}

void CModule::SetColorTransform(IColorTransform* pct)
{
    if (m_pColorTransform == 0 && pct == 0)
        return;
    EColorVisual uModifyed = (pct != m_pColorTransform) ? eColorVisualTransform : eColorVisualNormal;
    m_pColorTransform = pct;
    DispatchObject(CM_COLORVISUAL, uModifyed, 0);
//    DispatchUI(uModifyed);
}

EColorVisual CModule::GetVisual()
{
    if (m_pColorTransform)
        return eColorVisualTransform;
    if (m_pOwner->m_bHighContrast)
        return eColorVisualHighContrast;
    return eColorVisualNormal;
}

TMColor CModule::ConvertColor(int nObjID, TMColor clr)
{
    if (!m_pColorTransform)
        return clr;
    return m_pColorTransform->ConvertColor(nObjID, clr);
}


#ifdef TM_WIN_DESKTOP


template < class T, class K >
static TMBool _CModule_SetMenuItem(T& aList, TMHMenu hMenu, MENUITEMINFO& mi, const K& __key, int nPos)
{
    typedef typename T::recorder recorder;
    recorder rec(aList);
    rec.find(__key);
    if (rec)
    {
        const CString& str = rec.get_data();
        mi.dwTypeData = (TMStr)str.c_str();
        mi.cch = str.GetLength() + 1;
        mi.fMask = MIIM_TYPE;
        SetMenuItemInfo(hMenu, nPos, true, &mi);
        return true;
    }
    return false;
}

void CModule::SetMenuInfo(TMHMenu hMenu, int nPos)
{
    MENUITEMINFO mi = { sizeof(mi), MIIM_ID | MIIM_TYPE | MIIM_SUBMENU };
    CString str;
    mi.cch = 1024;
    mi.dwTypeData = str.GetBuffer(1024);
    if (GetMenuItemInfo(hMenu, nPos, true, &mi))
    {
        if (mi.cch)
        {
            if (!_CModule_SetMenuItem(m_aMenuItemLang, hMenu, mi, mi.wID, nPos))
            {
                str.SetLength(mi.cch);
                _CModule_SetMenuItem(m_aMenuPopLang, hMenu, mi, str, nPos);
            }
        }

        if (mi.hSubMenu)
        {
            SetMenuInfo(mi.hSubMenu, 0);
        }

        SetMenuInfo(hMenu, nPos + 1);
    }
}

#endif

CStrPtr CModule::LoadString(int nID, const CStrView* strXmlProperty)
{
    if (!this)
        return CStrPtr();

    if (strXmlProperty)
    {
        CXmlList::recorder rec2(m_aXmls);
        rec2.find(nID);
        if (!rec2)
            return CStrPtr();
        IDomElement* x1 = rec2.get_data();
        CDomStrPtr str;
        x1->getAttribute(*strXmlProperty, str);
        return str;
    }

    CModule::CStrList::recorder rec(m_aMessageLang);
    rec.find(nID);
    if (rec)
        return CStrPtr(rec.get_data());
    
#if TM_WIN_DESKTOP
    return CString::FastLoadString(m_hRes, nID);
//     CStrPtr str;
//     for (int nLen = 1024; ; nLen *= 2)
//     {
//         int nRes = ::LoadString(m_hRes, nID, str.GetBuffer(nLen), nLen);
//         if (nRes < nLen - 1)
//         {
//             str.SetLength(nRes);
//             return std::move(str);
//         }
//     }
#endif
}



TM_END_NAMESPACE
