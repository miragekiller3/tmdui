/**
 *  @file
 *  @brief  dom.
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2014.2.13
 *
 *  Copyright (C) 2014 miragekiller                               
 */

/*
 *  serialize sample:
 *    SERIALIZE_BGN_MAP   (CDlgMain)
 *      SERIALIZE_ENTRY   (TM_T("earnings"))
 *      SERIALIZE_VAR     (TM_T("stocks"), Item(IDC_MEMO_2002).SerializeText())
 *      SERIALIZE_LEAVE   ()
 *    SERIALIZE_END_MAP   ()
 *    OnInitDialog()
 *    {
 *      m_doc->LoadXml(L"set.cfg");
 *      DoSerialize(CDomDocument::CArchiveI(m_doc));
 *    }
 *    OnOk()
 *    {
 *      DoSerialize(CDomDocument::CArchiveO(m_doc));
 *    }
 */

#pragma once

#ifndef TMDOM_H
#define TMDOM_H

#include "tmxml.h"
#include "tmstrptr.h"
#include "tmcom.h"
#include "tmarchive.h"
#include "tmtxtfile.h"


TM_BGN_NAMESPACE
struct IDomNode;
struct IDomText;
struct IDomElement;
struct IDomProcessingInstruction;
struct IDomDocument;
TM_END_NAMESPACE


#define DOM_API  TM_CAPI
typedef TM::CStrView            CDomStrView;
typedef TM::CStrPtr             CDomStrPtr;
typedef void (*PFNDOMENUM)(TM::IDomNode* pNode, TMLParam lParam);


class IDomParser
{
public:
    virtual void AddNode(const CDomStrPtr& tag) = 0;
    virtual void EndNode() = 0;
    virtual void AddProperty(const CDomStrPtr& name, const CDomStrPtr& val) = 0;
    virtual void EndPropertys() = 0;
    virtual void AddText(const CDomStrPtr& text) = 0;
    virtual void AddProcessingInstruction(const CDomStrPtr& target, const CDomStrPtr& data) {}
};

enum EXmlLoadFlag
{
    eXmlLoadFlagSpacePreserve   = 0x01, // keep all space
    eXmlLoadFlagSpaceXML        = 0x02, // remove space before and after tag
    eXmlLoadFlagHTML            = 0x03, // merge space before and after tag, auto close <img>, <H>, ...
    eXmlLoadFlagText            = 0x04, 
};

DOM_API void    DomParser(TM::CXmlTag xml, IDomParser* pCallback, EXmlLoadFlag flag);
DOM_API void    DomCreate(TM::IDomDocument** ppDoc);
DOM_API TMBool  DomSave  (TMCStr fnXml, const TM::CString& xml);
DOM_API void    DomEnum  (TM::IDomNode* pNode, PFNDOMENUM proc, TMLParam lParam);
DOM_API void    DomRemoveChild(TM::IDomNode* pNode); // don't remove slef
DOM_API TMBool  DomIsChild(TM::IDomNode* pNode, TM::IDomNode* pChild);
DOM_API TMBool  DomFindChild(TM::IDomNode* pNode, TM::IDomNode* pChild); // pChild can be a invalid pointer
DOM_API int     DomLevel(TM::IDomNode* pNode);
DOM_API int     DomChildCount(TM::IDomNode* pNode);
DOM_API int     DomChildAllCount(TM::IDomNode* pNode);
DOM_API TM::IDomNode* DomRoot(TM::IDomNode* pNode);
DOM_API void    DomSetZorder(TM::IDomNode* pNode, TM::IDomNode* hIterator, EZOrder eZOrderOrIndex);

// xmlSpacePreserve



TM_BGN_NAMESPACE


struct IDomNodeCreator : public IUnknown // IDomNodeExtend
{
public:
    virtual IDomElement* createElement(const CDomStrPtr& tag);
    virtual IDomText* createTextNode(const CDomStrPtr& txt);
    virtual IDomProcessingInstruction* createProcessingInstruction(const CDomStrPtr& target, const CDomStrPtr& data);
};

enum DOMNodeType 
{
    NODE_INVALID = 0, 
    NODE_ELEMENT = NODE_INVALID + 1, 
    NODE_ATTRIBUTE = NODE_ELEMENT + 1, 
    NODE_TEXT = NODE_ATTRIBUTE + 1, 
    NODE_CDATA_SECTION = NODE_TEXT + 1, 
    NODE_ENTITY_REFERENCE = NODE_CDATA_SECTION + 1, 
    NODE_ENTITY = NODE_ENTITY_REFERENCE + 1, 
    NODE_PROCESSING_INSTRUCTION = NODE_ENTITY + 1, 
    NODE_COMMENT = NODE_PROCESSING_INSTRUCTION + 1, 
    NODE_DOCUMENT = NODE_COMMENT + 1, 
    NODE_DOCUMENT_TYPE = NODE_DOCUMENT + 1, 
    NODE_DOCUMENT_FRAGMENT = NODE_DOCUMENT_TYPE + 1, 
    NODE_NOTATION = NODE_DOCUMENT_FRAGMENT + 1 
}; 

TM_DECLARE_UUID_NS(TM, IDomNode,                    51F3E618, D50A, 4cd2, 8D, 9E, C5, 07, 3B, BD, 09, 00);
TM_DECLARE_UUID_NS(TM, IDomNodeList,                51F3E618, D50A, 4cd2, 8D, 9E, C5, 07, 3B, BD, 09, 01);
TM_DECLARE_UUID_NS(TM, IDomAttr,                    51F3E618, D50A, 4cd2, 8D, 9E, C5, 07, 3B, BD, 09, 02);
TM_DECLARE_UUID_NS(TM, IDomText,                    51F3E618, D50A, 4cd2, 8D, 9E, C5, 07, 3B, BD, 09, 03);
TM_DECLARE_UUID_NS(TM, IDomElement,                 51F3E618, D50A, 4cd2, 8D, 9E, C5, 07, 3B, BD, 09, 04);
TM_DECLARE_UUID_NS(TM, IDomProcessingInstruction,   51F3E618, D50A, 4cd2, 8D, 9E, C5, 07, 3B, BD, 09, 05);
TM_DECLARE_UUID_NS(TM, IDomDocument,                51F3E618, D50A, 4cd2, 8D, 9E, C5, 07, 3B, BD, 09, 09);

TM_DECLARE_UUID_NS(TM, IDomNodeCreator,             51F3E618, D50A, 4cd2, 8D, 9E, C5, 07, 3B, BD, 09, 10);

// TM_DECLARE_UUID_NS(IDomExtend,    51F3E618, D50A, 4cd2, 8D, 9E, C5, 07, 3B, BD, 09, 06);
// 
// 
// struct IDomExtend : public IUnknown
// {
// public:
//     virtual void OnExtendDetach(IDomNode* pOwner, void* pData) = 0;
// };

struct IDomNode : public IUnknown
{
public:
    // w3c, not support
    // value_type get_baseURI()
    virtual CComPtr<IDomNodeList> get_childNodes();
//     {
//         return IDomNodeList(*this);
//     }
//     virtual void AddExtend(IDomNodeExtend* extend) = 0;
//     virtual void RemoveExtend(IDomNodeExtend* extend) = 0;

//     virtual void SetExtend(const GUID& guid, TMLParam val, PFNEXTENDDETACH proc = 0) = 0;
//     virtual void RemoveExtend(const GUID& guid) = 0;
//     virtual TMBool GetExtend(const GUID& guid, TMLParam* val) = 0;

//     virtual void setUserData(void* p) = 0;
//     
//     virtual void* getUserData() = 0;

    virtual IDomNode* get_firstChild() = 0;
    virtual IDomNode* get_lastChild() = 0;

    // w3c, not support
    // localName
    //  namespaceURI 

    virtual IDomNode* get_nextSibling() = 0;

    // NODE_ATTRIBUTE Contains the name of the attribute. 
    // NODE_CDATA_SECTION Contains the literal string "#cdata-section". 
    // NODE_COMMENT Contains the literal string "#comment". 
    // NODE_DOCUMENT Contains the literal string "#document". 
    // NODE_DOCUMENT_TYPE Contains the name of the document type; for example, xxx in <!DOCTYPE xxx ...>. 
    // NODE_DOCUMENT_FRAGMENT Contains the literal string "#document-fragment". 
    // NODE_ELEMENT Contains the name of the XML tag, with any namespace prefix included if present. 
    // NODE_ENTITY Contains the name of the entity. 
    // NODE_ENTITY_REFERENCE Contains the name of the entity referenced. Note that the name does not include the leading ampersand or the trailing semicolon. The name includes the namespace if one is present. 
    // NODE_NOTATION Contains the name of the notation. 
    // NODE_PROCESSING_INSTRUCTION Contains the target; the first token following the <? characters. 
    // NODE_TEXT Contains the literal string "#text". 
    virtual CDomStrPtr get_nodeName() = 0;

    virtual DOMNodeType get_nodeType() = 0;

    // NODE_ATTRIBUTE Contains a string representing the value of the attribute. For attributes with subnodes, this is the concatenated text of all subnodes with entities expanded. Setting this value deletes all children of the node and replaces them with a single text node containing the value written. 
    // NODE_CDATA_SECTION Contains a string representing the text stored in the CDATA section. 
    // NODE_COMMENT Contains the content of the comment, exclusive of the comment's start and end sequence. 
    // NODE_DOCUMENT, NODE_DOCUMENT_TYPE, NODE_DOCUMENT_FRAGMENT, NODE_ELEMENT, NODE_ENTITY, NODE_ENTITY_REFERENCE, NODE_NOTATION Contains Null. Note that attempting to set the value of nodes of these types generates an error. 
    // NODE_PROCESSING_INSTRUCTION Contains the content of the processing instruction, excluding the target. (The target appears in the nodeName property.) 
    // NODE_TEXT Contains a string representing the text stored in the text node. 
    virtual CDomStrPtr get_nodeValue() = 0;
    virtual void put_nodeValue(const CDomStrPtr& val) = 0;
    
    virtual IDomDocument* get_ownerDocument() = 0;

    virtual IDomNode* get_parentNode() = 0;

    //W3C 
    // prefix

    virtual IDomNode* get_previousSibling() = 0;

    //W3C 
    //textContent

    // IE
    //value_type get_text() const { return m_strText; }
    //xml

    ///////////////////////////////////////////////////////////////////////
    // W3C method
    virtual IDomNode* appendChild(IDomNode* child) = 0;

    // W3C
    CComPtr<IDomNode> cloneNode(TMBool clone_all_child) { DbgAssert(0); return 0; }
    // compareDocumentPosition()

    virtual TMBool hasAttributes() { return false; }

    virtual TMBool hasChildNodes() = 0;

    //Address of the reference node; newChild is inserted to the left of refChild. If Null, newChild is inserted at the end of the child list.
    virtual void insertBefore(IDomNode* newItem, int nPos) = 0;
    
    virtual void insertBefore(IDomNode* newItem, IDomNode* nPos) = 0;

    // W3C
    // isDefaultNamespace(URI)
    //isEqualNode()
    //isSameNode()
    //isSupported() 
    // lookupNamespaceURI()
    // lookupPrefix()
    // void normalize() {}
    
    virtual void removeChild(IDomNode* node) = 0;

    //W3C
    //void replaceChild(iterator newChild, iterator oldChild);

    //IE
    IDomNodeList* selectNodes(CDomStrView expression);
    virtual IDomNode* selectSingleNode(CDomStrView queryString) = 0;

    //W3C
    // transformNode()
    // transformNodeToObject()
    // setUserData(key,data,handler)

    // IE

    ///////////////////////////////////////////////////////////////////////
    // extend
    virtual int transformNode(CString& str, TMBool bTransformChilds = true, int nTab = 2); // return: number of child element nodes.

//     IDomElement* appendChildElement(const value_type& name);

    void removeAllChild()
    {
        DomRemoveChild(this);
    }

    void insertAfter(IDomNode* newItem)
    {
        IDomNode* ptr = get_nextSibling();
        if (ptr)
            insertBefore(newItem, ptr);
        else
            get_parentNode()->appendChild(newItem);
    }
    
//     CDomElement* insertBeforeElement(const value_type& name, int nPos)
//     {
//         CDomElement* p = CDomElement::CreateInstance(name);
//         (*this)->insertBefore(p, nPos);
//         return p;
//     }
//     
//     CDomText* appendChildTextNode(const value_type& txt)
//     {
//         CDomText* p = CDomText::CreateInstance(txt);
//         (*this)->appendChild(p);
//         return p;
//     }
//     
//     CDomText* insertBeforeTextNode(const value_type& txt, int nPos)
//     {
//         CDomText* p = CDomText::CreateInstance(txt);
//         (*this)->insertBefore(p, nPos);
//         return p;
//     }
};

struct IDomNodeList : public IUnknown
{
public:
    virtual IDomNode* item(int n) = 0;
    virtual long get_length() = 0;
};

struct IDomText : public IDomNode
{
public:
    enum { node_type = NODE_TEXT };
    
public:
    ///////////////////////////////////////////////////////////////////////
    // W3C property
    virtual CDomStrPtr get_data() = 0;
    virtual void put_data(const CDomStrPtr& txt) = 0;
    
    //W3C
    //    isElementContentWhitespace
    
    virtual long get_length() = 0;
    
    //W3C
    //wholeText
    
    ///////////////////////////////////////////////////////////////////////
    // W3C method
    
    // W3C
    virtual void appendData(const CDomStrView& txt) = 0;
    
    virtual void deleteData(long offset, long count) = 0;
    
    virtual void insertData(long offset, const CDomStrView& txt) = 0;
    
    virtual void replaceData(long offset, long count, const CDomStrView& txt) = 0;
    
    // W3C not support
    // replaceWholeText()
    // splitText()
    // substringData()
};

struct IDomAttr : public IDomNode
{
public:
    virtual CDomStrPtr get_name() = 0;
    virtual IDomElement* get_ownerElement() = 0;
//    schemaTypeInfo
//        specified
//        textContent
    virtual CDomStrPtr get_text() = 0;
    virtual CDomStrPtr get_value() = 0;
    virtual void put_value(const CDomStrPtr& val) = 0;
//        xml
};


struct IDomElement : public IDomNode
{
public:
    enum { node_type = NODE_ELEMENT };

public:
    ///////////////////////////////////////////////////////////////////////
    // W3C property
    virtual CComPtr<IDomNodeList> get_attributes() = 0;

    //W3C 
    // schemaTypeInfo
    
    virtual CDomStrPtr get_tagName() = 0;
    virtual void put_tagName(const CDomStrPtr& name) = 0;

    ///////////////////////////////////////////////////////////////////////
    // W3C method

    // W3C
    // iterator cloneNode(TMBool clone_all_child);
    // compareDocumentPosition()
    // dispatchEvent()

    virtual TMBool getAttribute(const CDomStrView& pname, CDomStrPtr& retStr) = 0;
    CDomStrPtr getAttribute(const CDomStrView& pname) { CDomStrPtr v; getAttribute(pname, v); return v; }

    virtual IDomAttr* getAttributeNode(const CDomStrView& name) = 0;
    
    virtual CComPtr<IDomNodeList> getElementsByTagName(const CDomStrView& name) = 0;
    
    // W3C
    //     getFeature(feature,version)
    //     getUserData(key)

//    virtual TMBool hasAttributes(); // { return 0; }

    virtual TMBool hasAttribute(const CDomStrView& name) = 0;
    
    virtual void removeAttribute(const CDomStrView& pname) = 0;
    
    virtual void removeAttributeNode(IDomNode* attr) = 0;

    virtual IDomAttr* setAttribute(const CDomStrPtr& name, const CDomStrPtr& pvalue) = 0;
    
    virtual void setAttributeNode(IDomNode* attr) = 0;
    
    //W3C
    // setIdAttribute(name,isId)
    // setIdAttributeNS(uri,name,isId)
    // setIdAttributeNode(idAttr,isId)

    // ext
    TMBool getAttributeLong(const CDomStrView& pname, long& ret)
    {
        CDomStrPtr v;
        if (getAttribute(pname, v))
        {
            ret = v.ToLong(ret);
            return true;
        }
        return false;
    }

    TMBool getAttributeFloat(const CDomStrView& pname, float& ret)
    {
        CDomStrPtr v;
        if (getAttribute(pname, v))
        {
            ret = (float)v.ToDouble(ret);
            return true;
        }
        return false;
    }

    IDomAttr* setAttributeLong(const CDomStrPtr& name, long x) { return setAttribute(name, TM::L2W(x)); }
    IDomAttr* setAttributeHex(const CDomStrPtr& name, long x) { return setAttribute(name, TM::Hex2TT<TMCharW>(x)); }
    IDomAttr* setAttributeFloat(const CDomStrPtr& name, float x) { return setAttribute(name, TM::F2W(x)); }
};

struct IDomProcessingInstruction : public IDomNode
{
public:
    enum { node_type = NODE_PROCESSING_INSTRUCTION };

public:
    virtual CDomStrPtr get_data() = 0;
    virtual void put_data(const CDomStrPtr& txt) = 0;
    virtual CDomStrPtr get_target() = 0;
};

struct IDomDocument : public IDomNode
{
public:
    ///////////////////////////////////////////////////////////////////////
    // W3C property
    
    // W3C not support
    // async
    
    // W3C not support
    //    doctype
    
    virtual CDomStrPtr get_documentURI() = 0;
    virtual void put_documentURI(const CDomStrPtr& str) = 0;
    
    // W3C not support
    // domConfig
    
    // W3C not support
    // implementation
    // inputEncoding
    
    // W3C not support
    // strictErrorChecking
    // xmlEncoding
    // xmlStandalone
    // xmlVersion
    
    ///////////////////////////////////////////////////////////////////////
    // W3C method
    
    // W3C not support
    // adoptNode(sourcenode)
    
    // W3C not support
    // createAttribute(name)
    // createAttributeNS(uri,name)
    // createCDATASection()
    // createComment()
    // createDocumentFragment()
    
    virtual IDomElement* createElement(const CDomStrPtr& name) = 0; // { return 0; }
    
    // W3C not support
    // createElementNS()
    // createEvent()
    // createEntityReference(name)
    // createExpression()
    virtual IDomProcessingInstruction* createProcessingInstruction(const CDomStrPtr& target, const CDomStrPtr& data) = 0;
    // createRange()
    // evaluate()
    
    virtual IDomText* createTextNode(const CDomStrPtr& txt) = 0; // { return 0; }
    
    // W3C not support
    // getElementById()
    // getElementsByTagName()
    // getElementsByTagNameNS()
    //    importNode()
    
    virtual void LoadXml(CXmlTag tag, IDomNode* pParent = 0, EXmlLoadFlag flag = eXmlLoadFlagSpaceXML) = 0;
    
    // W3C not support
    //    normalizeDocument()
    //    renameNode()

    // extend
    IDomElement* appendChildElement(IDomNode* pNode, const CDomStrPtr& name)
    {
        CComPtr<IDomElement> ptr(createElement(name));
        pNode->appendChild(ptr);
        return ptr;
    }

    IDomText* appendTextElement(IDomNode* pNode, const CDomStrPtr& text)
    {
        CComPtr<IDomText> ptr(createTextNode(text));
        pNode->appendChild(ptr);
        return ptr;
    }
};

typedef CComQIPtr<IDomNode>         CDomNodePtr;
typedef CComQIPtr<IDomText>         CDomTextPtr;
typedef CComQIPtr<IDomElement>      CDomElementPtr;
typedef CComQIPtr<IDomDocument>     CDomDocumentPtr;
typedef CComQIPtr<IDomAttr>         CDomAttributePtr;
typedef CComQIPtr<IDomNodeList>     CDomAttributeListPtr;


TM_END_NAMESPACE


TM_BGN_NAMESPACE


// struct CDomElementDataSourceContainer
// {
//     inline static IDomElement* GetParent(IDomNode* p)
//     {
//         DbgAssert(p);
//         CDomElementPtr ptr(p->get_parentNode());
//         return ptr;
//     }
// 
//     inline static IDomElement* GetChild(IDomNode* p)
//     {
//         DbgAssert(p);
//         p = p->get_firstChild();
//         if (!p)
//             return 0;
//         CDomElementPtr ptr(p);
//         if (ptr)
//             return ptr;
//         return GetNext(p);
//     }
// 
//     inline static IDomElement* GetNext(IDomNode* p)
//     {
//         DbgAssert(p);
//         for (; p = p->get_nextSibling();)
//         {
//             CDomElementPtr pe(p);
//             if (pe)
//                 return pe;
//         }
//         return 0;
//     }
// 
//     inline static IDomElement* GetPrev(IDomNode* p)
//     {
//         DbgAssert(p);
//         for (; p = p->get_previousSibling();)
//         {
//             CDomElementPtr pe(p);
//             if (pe)
//                 return pe;
//         }
//         return 0;
//     }
// };
// 
// typedef CTreeItemT<IDomElement*, CDomElementDataSourceContainer>    CDomElementRec;

class CDomDocument : public CDomDocumentPtr
{
    TM_NO_COPYABLE(CDomDocument);

public:
    typedef struct CDomArchiveO CArchiveO;
    typedef struct CDomArchiveI CArchiveI;
    using CDomDocumentPtr::p;
    using CDomDocumentPtr::Release;

    CDomDocument()
    {
        DomCreate(&p);
    }

    void clear()
    {
        Release();
        DomCreate(&p);
    }

    TMBool LoadFromFile(TMCStr pFile, TMBool bCheckUtf8 = true)
    {
        TM::CString strXml;
        ETXTFILESTYLE tfs = TxtFileLoadAs(pFile, strXml, bCheckUtf8);
        if (tfs == e_TxtFileStyleUnknown)
            return false;
        p->LoadXml(strXml);
        return true;
    }

    TMBool SaveToFile(TMCStr pFile, int nTab = 2)
    {
        TM::CString str;
        p->transformNode(str, true, nTab);
        return DomSave(pFile, str);
    }
};

struct CDomArciveFileBase
{
    typedef IDomNode* section_type;
    CDomNodePtr m_file;
    CString m_strKey;
    CDomArciveFileBase(IDomNode* f) : m_file(f) {}
    
    void DoEntrySection(section_type& storage, const CString& str)
    {
        storage = m_file;
        IDomNode* p = m_file->selectSingleNode(str);
        if (!p)
            p = m_file->get_ownerDocument()->appendChildElement(m_file, str);
        m_file = p;
    }

    void DoLeaveSection(section_type& storage)
    {
        m_file = storage;
    }

    void DoSetKey(const CStringW& str) { m_strKey = str; }

    IDomElement* Element()
    {
        DbgAssert(m_file->get_nodeType() == NODE_ELEMENT);
        return static_cast<IDomElement*>(m_file.p);
    }
};

struct CDomArchiveO
    : public CArchiveTxtOImplT<CDomArchiveO, TMCharW>
    , public CDomArciveFileBase
{
public:
    CDomArchiveO(IDomNode* f) : CDomArciveFileBase(f) {}
    
    void DoSerializeString(const CStringW& str)
    {
        this->Element()->setAttribute(m_strKey, str);
    }
};

struct CDomArchiveI
    : public CArchiveTxtIImplT<CDomArchiveI, TMCharW>
    , public CDomArciveFileBase
{
public:
    CDomArchiveI(IDomNode* f) : CDomArciveFileBase(f) {}
    
    using CArchiveTxtIImplT<CDomArchiveI, TMCharW>::DoSerializeString;
    
    void DoSerializeString(CDomStrPtr& str)
    {
        this->SetStat(this->Element()->getAttribute(m_strKey, str));
    }

//     void DoSerializeString(CStringW& str)
//     {
//         CDomStrPtr ptr;
//         TMBool b = this->Element()->getAttribute(m_strKey, ptr);
//         this->SetStat(b);
//         if (b)
//             str = ptr;
//     }
};


// T: must inherit form CTreeNodeT;  T_Base: IDomXXX;
template <class T, class T_Base>
class TM_NO_VTABLE CDomNodeHelperT : public T_Base
{
public:
    T* GetThis() { return static_cast<T*>(this); }

    // IDomNode
    virtual CComPtr<IDomNodeList> get_childNodes() tm_override { return 0; }
    virtual IDomNode* get_firstChild() tm_override { return GetThis()->GetChild(); }
    virtual IDomNode* get_lastChild() tm_override { return GetThis()->GetSelf().GetChildAt(-1); }
    virtual IDomNode* get_nextSibling() tm_override { return GetThis()->GetNext(); }
    virtual IDomDocument* get_ownerDocument() tm_override { return 0; }
    virtual IDomNode* get_parentNode() tm_override { return GetThis()->GetParent(); }
    virtual IDomNode* get_previousSibling() tm_override { return GetThis()->GetPrev(); }
    virtual TMBool hasChildNodes() tm_override { return 0 != GetThis()->GetChild(); }
    virtual IDomNode* selectSingleNode(CDomStrView queryString) tm_override { return 0; }
    virtual int transformNode(CString& str, TMBool bTransformChilds = true, int nTab = 2) tm_override { return 0; }

    virtual IDomNode* appendChild(IDomNode* child) tm_override
    {
        DbgAssert(child);
        T* p = static_cast<T*>(child);
        GetThis()->InsertChildAt(p, -1);
        GetThis()->OnInsertItem(p);
        return p;
    }

    void insertBefore(IDomNode* newItem, int nPos) tm_override
    {
        DbgAssert(newItem);
        T* p = static_cast<T*>(newItem);
        GetThis()->InsertChildAt(p, nPos);
        GetThis()->OnInsertItem(p);
    }

    void insertBefore(IDomNode* newItem, IDomNode* nPos) tm_override
    {
        DbgAssert(newItem);
        DbgAssert(nPos);
        
        T* p = static_cast<T*>(newItem);
        T* pPos = static_cast<T*>(nPos);
        pPos->InsertBefore(p);
        GetThis()->OnInsertItem(p);
    }

    // IDomElement
//     CComPtr<IDomNodeList> get_attributes() { return 0; }
//     CStringW get_tagName()  { DbgAssert(0); return CStringW(); }
//     void put_tagName(const CStringW& name) { DbgAssert(0); }
//     TMBool getAttribute(const CStringW& pname, CStringW& retStr) { DbgAssert(0); return false; }
//     TMBool getAttribute(const CStringW& pname, CStringW& retStr) { DbgAssert(0); return false; }
//     IDomAttr* getAttributeNode(const CStringW& name) { DbgAssert(0); return 0; }
//     CComPtr<IDomNodeList> getElementsByTagName(const CStringW& name) { DbgAssert(0); return 0; }
//     TMBool hasAttributes() { DbgAssert(0); return false; }
//     TMBool hasAttribute(const value_type& name) { DbgAssert(0); return false; }
//     void removeAttribute(const value_type& pname) { DbgAssert(0); }
//     void removeAttributeNode(IDomNode* attr) { DbgAssert(0); }
//     IDomAttr* setAttribute(const value_type& name, const value_type& pvalue) { DbgAssert(0); return 0; }
//     void setAttributeNode(IDomNode* attr) { DbgAssert(0); }

    // helper function
    void OnInsertItem(IDomNode* pInsert) {}

};

// template <class T>
// class CDomPtrT
// {
// public:
//     T* m_p;
// 
// public:
//     CDomPtrT(T* p = 0)
//         : m_p(p)
//     {
//     }
// 
//     operator T* () const { return m_p; }
// 
//     T* operator->() const { return m_p; }
// 
//     int GetLevel()
//     {
//         DbgAssert(*this);
//         return DomLevel(m_p);
//     }
// };


TM_END_NAMESPACE


#endif // end of #ifndef TMDOM_H
