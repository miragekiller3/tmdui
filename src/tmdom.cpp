/**
 *  @file
 *  @brief  dom.
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.04.16
 *
 *  Copyright (C) 2015 miragekiller
 */

#include "tmdom.h"
#include "tmlist.h"
#include "tmtree.h"
#include "tmatomic.h"
#include "tmtxtfile.h"
#include "tmdebugext.h"
// #include "atmempool.h"

//#define CAutoDomSaveAnalysis CAutoAnalysisNull
//#define CAutoDomSaveAnalysis CAutoAnalysisTuiUser
#define CAutoDomSaveAnalysis CAutoAnalysisTuiMsg


TM_DECLARE_UUID(CDomNode_, 64B35660, A48F, 4640, 80, 04, A5, 07, 18, CE, 9F, BD);

TM_BGN_NAMESPACE


template <class T> struct CDomNodeT;
typedef CDomNodeT<IDomNode> CDomNode;

#define TM_INTERFACE_NODE() \
    if (TM_UUID_OF(CDomNode_) == iid) \
    { \
        CDomNode* p = static_cast<CDomNode*>(GetNode()); \
        *ppvObject = p; \
        AddRef(); \
        return S_OK; \
    }


///////////////////////////////////////////////////////////////////////
// CDomNodeList
class CDomNodeList
    : public IDomNodeList
{
public:
    CVector<CComPtr<IDomNode> > m_aNode;

#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        CDomNodeList* p = (CDomNodeList*)cls->m_pClass;
        return IC_Size(p->m_aNode);
    }
#endif // TM_DBG

public:
    TM_BGN_INTERFACE_MAP        (IDomNodeList)
        TM_INTERFACE_HANDLER    (IDomNodeList);
    TM_END_INTERFACE_MAP        ();

public:
    virtual IDomNode* item(int n) tm_override
    {
        if (n >= m_aNode.size())
            return 0;
        else
            return m_aNode.at(n);
    }

    virtual long get_length() tm_override
    {
        return m_aNode.size();
    }

    void AddElementsByTagName(IDomNode* pNode, const CDomStrView& name)
    {
        if (!pNode || pNode->get_nodeType() != NODE_ELEMENT)
            return;
        if (pNode->get_nodeName().CompareNoCase(name) == 0)
            m_aNode.insert(pNode);
        AddElementsByTagName(pNode->get_nextSibling(), name);
        AddElementsByTagName(pNode->get_firstChild(), name);
    }
};

///////////////////////////////////////////////////////////////////////
// IDomNode
enum EXPathType
{
    e_xptError      = -1,
    e_xptEof        = 0,
    e_xptCurrent    = './',
    e_xptParent     = '../',
    e_xptRoot       = '/',
    e_xptAllLevel   = '//',
    e_xptString     = 'a',
    e_xptAttr       = '@',
    e_xpAll         = '*',
};

template <class T>
struct CDomNode_ParseXPath
{
    T* GetSelf() { return static_cast<T*>(this); }

    void Parse(CDomStrView s)
    {
        CDomStrView::const_pointer str = s.data();
        for (int i = 0; i < s.length(); ++i)
        {
            TMCharW ch;

            int bgn = i;
            tm_for (; ; ++i)
            {
                ch = str[i];
                switch (ch)
                {
                case '.':
                case '/':
                case 0:
                case '*':
                case '@':
                    GetSelf()->ParseString(s.Mid(bgn, i - bgn));
                    if (!ch)
                        return;
                    break;
                    
                default:
                    continue;
                }
                break;
            }

            switch (ch)
            {
            case '@':
                GetSelf()->ParseAttr();
                break;

            case '*':
                GetSelf()->ParseAll();
                break;

            case '.':
                if (str[i + 1] == '/')
                {
                    ++i;
                    GetSelf()->ParseCurrent();
                }
                else if (str[i + 1] == '.')
                {
                    ++i;
                    GetSelf()->ParseParent();
                }
                else
                    GetSelf()->ParseError();
                break;

            case '/':
                if (i == 0)
                    GetSelf()->ParseRoot();
                else
                    GetSelf()->ParseCurrent();
                break;
            }
        }
    }
};

struct CSingleNodeParser : public CDomNode_ParseXPath<CSingleNodeParser>
{
    IDomNode* p;
    TMBool bAttr;

    void ParseError() { p = 0; }
    void ParseAttr() { bAttr = true; }
    void ParseAll() { if (p) p = p->get_firstChild(); }
    void ParseCurrent() {}
    void ParseParent() { if (p) p = p->get_parentNode(); }
    void ParseRoot() { if (p) p = p->get_ownerDocument(); }

    void ParseString(const CDomStrView& str)
    {
        if (!p || str.IsEmpty())
            return;

        if (bAttr)
        {
            CComQIPtr<IDomElement> ptr(p);
            if (ptr)
                p = ptr->getAttributeNode(str);
            else
                p = 0;
        }
        else
        {
            for (p = p->get_firstChild(); p; p = p->get_nextSibling())
            {
                if (str.CompareNoCase(p->get_nodeName()) == 0)
                    return;
            }
            DbgAssert(p == 0);
        }
    }
};

IDomNodeList* IDomNode::selectNodes(CDomStrView expression)
{
    return 0;
}

CComPtr<IDomNodeList> IDomNode::get_childNodes()
{
    CDomNodeList* p = new CRefObjectT<CDomNodeList>(0);
    for (IDomNode* pc = get_firstChild(); pc; pc = pc->get_nextSibling())
        p->m_aNode.insert(pc);
    return p;
}

int IDomNode::transformNode(CString& str, TMBool bTransformChilds, int nTab)
{
    int nCount = 0;
    if (bTransformChilds)
    {
        CAutoDomSaveAnalysis _ama("IDomNode::transformNode::write child");
        IDomNode* pc = get_firstChild();
        for (IDomNode* p = pc; p; p = p->get_nextSibling())
        {
            // write child
            nCount += p->transformNode(str, bTransformChilds, nTab);
        }

        // write tail
//         CAutoDomSaveAnalysis _ama2("IDomNode::transformNode::write tail");
//         if (pc)
//             str.append(IDomNode_Depth(this) * nTab, ' ');
    }
    return nCount;
}


///////////////////////////////////////////////////////////////////////
// CDomNode

// xml node
// http://www.w3school.com.cn/xmldom/dom_node.asp



template <class T>
struct TM_NO_VTABLE CDomNodeT
    : public T
    , public CTreeNodeT<CDomNodeT<IDomNode>* >
{
public:
    typedef CDomNodeT                                   class_type;
    typedef CTreeNodeT<CDomNodeT<IDomNode>* >           base_class;
    typedef base_class::iterator                        iterator;
    typedef TM::CString                                 value_type;
    typedef CComQIPtr<IDomNode>                         CPtr;
    TM_UNUSED_IC_GetDebugInfo();
    TM_NO_COPYABLE(CDomNodeT);

public:
    CDomNodeT()
    {
    }

    virtual ~CDomNodeT()
    {
        DbgAssert(!GetChild());
    }

    CDomNodeT<IDomNode>* GetNode() { return (CDomNodeT<IDomNode>*)this; }

    ///////////////////////////////////////////////////////////////////////
    // W3C property

    // w3c, not support
    // value_type get_baseURI()
//     IDomNodeList get_childNodes()
//     {
//         return IDomNodeList(*this);
//     }

    virtual IDomNode* get_firstChild() tm_override
    {
        return GetSelf()->GetChild();
    }

    virtual IDomNode* get_lastChild() tm_override
    {
        return GetSelf().GetChildAt(-1);
    }

    // w3c, not support
    // localName ���ؽڵ�ı������ơ�?No 1 9 Yes
    //  namespaceURI

    virtual IDomNode* get_nextSibling() tm_override
    {
        return GetSelf()->GetNext();
    }

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
//     virtual value_type get_nodeName() = 0;
//
//     virtual DOMNodeType get_nodeType() = 0;

    // NODE_ATTRIBUTE Contains a string representing the value of the attribute. For attributes with subnodes, this is the concatenated text of all subnodes with entities expanded. Setting this value deletes all children of the node and replaces them with a single text node containing the value written.
    // NODE_CDATA_SECTION Contains a string representing the text stored in the CDATA section.
    // NODE_COMMENT Contains the content of the comment, exclusive of the comment's start and end sequence.
    // NODE_DOCUMENT, NODE_DOCUMENT_TYPE, NODE_DOCUMENT_FRAGMENT, NODE_ELEMENT, NODE_ENTITY, NODE_ENTITY_REFERENCE, NODE_NOTATION Contains Null. Note that attempting to set the value of nodes of these types generates an error.
    // NODE_PROCESSING_INSTRUCTION Contains the content of the processing instruction, excluding the target. (The target appears in the nodeName property.)
    // NODE_TEXT Contains a string representing the text stored in the text node.
    virtual CDomStrPtr get_nodeValue() tm_override { return CDomStrPtr(); }
    virtual void put_nodeValue(const CDomStrPtr& val) tm_override {}

    virtual IDomDocument* get_ownerDocument() tm_override
    {
        IDomNode* p = get_parentNode();
        if (!p)
            return 0;
        return p->get_ownerDocument();
    }

    virtual IDomNode* get_parentNode() tm_override
    {
        return GetSelf()->GetParent();
    }

    //W3C
    // prefix

    virtual IDomNode* get_previousSibling() tm_override
    {
        return GetSelf()->GetPrev();
    }

    //W3C
    //textContent

    // IE
    //value_type get_text() const { return m_strText; }
    //xml

    ///////////////////////////////////////////////////////////////////////
    // W3C method
    virtual IDomNode* appendChild(IDomNode* child) tm_override
    {
        CComQIPtr<CDomNode, &TM_UUID_OF(CDomNode_)> node(child);
        if (node)
        {
            GetSelf()->InsertChildAt(node, -1);
            return child;
        }
        return 0;
    }

    // W3C
    // iterator cloneNode(TMBool clone_all_child);
    // compareDocumentPosition()

//     virtual TMBool hasAttribute(const value_type& name) tm_override { return false; }

    virtual TMBool hasChildNodes() tm_override
    {
        return 0 != GetSelf()->GetChild();
    }

    //Address of the reference node; newChild is inserted to the left of refChild. If Null, newChild is inserted at the end of the child list.
    virtual void insertBefore(IDomNode* newChild, int n) tm_override
    {
        CComQIPtr<CDomNode, &TM_UUID_OF(CDomNode_)> node(newChild);
        if (!node)
            return;
        GetSelf()->InsertChildAt(node, n);
    }

    virtual void insertBefore(IDomNode* newChild, IDomNode* n) tm_override
    {
        CComQIPtr<CDomNode, &TM_UUID_OF(CDomNode_)> node1(newChild);
        CComQIPtr<CDomNode, &TM_UUID_OF(CDomNode_)> node2(n);
        if (!node1 || !node2 || node2 == node1)
            return;
        node2->InsertBefore(node1);
    }

    // W3C
    // isDefaultNamespace(URI)
    //isEqualNode()
    //isSameNode()
    //isSupported()
    // lookupNamespaceURI()
    // lookupPrefix()
    // void normalize() {}

    virtual void removeChild(IDomNode* node) tm_override
    {
        CComQIPtr<CDomNode, &TM_UUID_OF(CDomNode_)> ptr(node);
        if (!ptr)
            return;
        ptr->Detach();
        DomRemoveChild(node);
        node->Release();
    }

    //W3C
    //void replaceChild(iterator newChild, iterator oldChild);
    //selectNodes()

    IDomNode* selectSingleNode(CDomStrView queryString) tm_override  // ���Һ� XPath ��ѯƥ���һ���ڵ�?6
    {
        CSingleNodeParser parser;
        parser.p = this;
        parser.bAttr = false;
        parser.Parse(queryString);
        return parser.p;
    }

    // selectSingleNode() ���Һ� XPath ��ѯƥ���һ���ڵ�?6
    // transformNode() ʹ�� XSLT ��һ���ڵ�ת��Ϊһ���ַ����� 6
    // transformNodeToObject() ʹ�� XSLT ��һ���ڵ�ת��Ϊһ���ĵ��� 6
    // setUserData(key,data,handler) �Ѷ���������ڵ��ϵ�һ�����ϡ�?    No Yes

    template <class F> void Enum(F _func)
    {
        GetSelf().Enum(_func);
    }

    template <class F, class P> void Enum2 (F __f, P param)
    {
        GetSelf().Enum2(__f, param);
    }
};





// xml attribute
// http://www.w3school.com.cn/xmldom/dom_attribute.asp
struct CDomAttr : public CDomNodeT<IDomAttr>
{
public:
    typedef CComQIPtr<IDomAttr>                         CPtr;

public:
    value_type m_strName;
    value_type m_strValue;

public:
    TM_BGN_INTERFACE_MAP        (IDomAttr)
        TM_INTERFACE_HANDLER    (IDomNode);
        TM_INTERFACE_HANDLER    (IDomAttr);
        TM_INTERFACE_NODE       ();
    TM_END_INTERFACE_MAP        ();

public:
    // extend
    CDomAttr() {}

    ///////////////////////////////////////////////////////////////////////
    // W3C property
//     virtual TMBool get_isId() tm_override { return false; } //m_strName.CompareNoCase(L"id") == 0; }
    virtual IDomElement* get_ownerElement() tm_override;
    virtual IDomNode* get_parentNode() tm_override { return 0; }
    virtual IDomDocument* get_ownerDocument() tm_override;
//     virtual value_type get_localName() tm_override { return m_strName; };
    virtual CDomStrPtr get_name() tm_override { return m_strName; };
    virtual CDomStrPtr get_nodeName() tm_override { return m_strName; }
    virtual DOMNodeType get_nodeType() tm_override { return NODE_ATTRIBUTE; }
    virtual CDomStrPtr get_nodeValue() tm_override { return m_strValue; }
    virtual void put_nodeValue(const CDomStrPtr& val) tm_override { m_strValue = val; }
    virtual CDomStrPtr get_text() tm_override { return m_strValue; }
    virtual CDomStrPtr get_value() tm_override { return m_strValue; }
    virtual void put_value(const CDomStrPtr& val) tm_override { m_strValue = val; }
};


// for create element && attrib
struct CDomElementPool : public CDomNodeT<IDomNode>
{
protected:
    CDomNodeT<IDomNode>* m_pOwner;

public:
    TM_UNUSED_ADDREF_RELEASE();

    TM_BGN_INTERFACE_MAP        (IDomNode)
    TM_END_INTERFACE_MAP        ();

    CDomElementPool(CDomNodeT<IDomNode>* pOwner);
    ~CDomElementPool();

public:
    virtual DOMNodeType get_nodeType() tm_override { return NODE_INVALID; }
    virtual CDomStrPtr get_nodeName() tm_override { return CDomStrView(); }
    virtual IDomDocument* get_ownerDocument() tm_override;
};


// xml element
// http://www.w3school.com.cn/xmldom/dom_element.asp
struct CDomElement : public CDomNodeT<IDomElement>
{
public:
    enum { node_type = NODE_ELEMENT };
    typedef CDomElement                                 class_type;
    typedef CDomNodeT<IDomElement>                      base_class;
    typedef base_class::iterator                        iterator;
    typedef base_class::value_type                      value_type;
    typedef CDomAttr::iterator                          CPropRec;
    typedef CComQIPtr<IDomElement>                      CPtr;
    typedef CComQIPtr<IDomAttr>                         CAttributePtr;
    typedef CComQIPtr<IDomNodeList>                     CAttributeListPtr;

public:
    TM_BGN_INTERFACE_MAP        (IDomElement)
        TM_INTERFACE_HANDLER    (IDomNode);
        TM_INTERFACE_HANDLER    (IDomElement);
        TM_INTERFACE_NODE       ();
    TM_END_INTERFACE_MAP        ();

protected:
    value_type  m_strName;
    CDomElementPool m_aAttr;

public:
    CDomElement();
    ~CDomElement();

    ///////////////////////////////////////////////////////////////////////
    // W3C property
    virtual CComPtr<IDomNodeList> get_attributes() tm_override;

    virtual CDomStrPtr get_nodeName() tm_override
    {
        return m_strName;
    }

    virtual DOMNodeType get_nodeType() tm_override
    {
        return NODE_ELEMENT;
    }

    //W3C
    // schemaTypeInfo

    virtual CDomStrPtr get_tagName() tm_override { return m_strName; }
    virtual void put_tagName(const CDomStrPtr& name) tm_override { m_strName = name; }
    
    ///////////////////////////////////////////////////////////////////////
    // W3C method

    // W3C
    // iterator cloneNode(TMBool clone_all_child);
    // compareDocumentPosition()
    // dispatchEvent()

    virtual TMBool getAttribute(const CDomStrView& pname, CDomStrPtr& retStr) tm_override;

    virtual IDomAttr* getAttributeNode(const CDomStrView& name) tm_override;

    virtual CComPtr<IDomNodeList> getElementsByTagName(const CDomStrView& name) tm_override;

    // W3C
    //     getFeature(feature,version) ���� DOM ���󣬴˶����ִ��ӵ��ָ�����ԺͰ汾��ר�ŵ�?API��     No Yes
    //     getUserData(key) ���ع����ڵ��ϼ��Ķ��󡣴˶����������ͨ��ʹ����ͬ�ļ�������?setUserData �����õ��˽ڵ㡣     No Yes

    virtual TMBool hasAttributes() tm_override;

    virtual TMBool hasAttribute(const CDomStrView& name) tm_override;

    virtual void removeAttribute(const CDomStrView& pname) tm_override;

    virtual void removeAttributeNode(IDomNode* attr) tm_override;

    virtual IDomAttr* setAttribute(const CDomStrPtr& name, const CDomStrPtr& pvalue) tm_override;

    virtual void setAttributeNode(IDomNode* attr) tm_override { DbgAssert(0); }

    //W3C
    // setIdAttribute(name,isId) ���?Attribute ���� isId ����Ϊ true����ô�˷������ָ������������Ϊһ���û�ȷ��?ID �����ԣ�user-determined ID attribute����     No Yes
    // setIdAttributeNS(uri,name,isId) ���?Attribute ���� isId ����Ϊ true����ô�˷������ָ������������Ϊһ���û�ȷ��?ID �����ԣ�user-determined ID attribute�������������ռ䣩��     No Yes
    // setIdAttributeNode(idAttr,isId) ���?Attribute ���� isId ����Ϊ true����ô�˷������ָ������������Ϊһ���û�ȷ��?ID �����ԣ�user-determined ID attribute����     No Yes

    ///////////////////////////////////////////////////////////////////////
    // extend
//     CPropRec get_attributes_rec()
//     {
//         return m_aAttr.GetChild();
//     }

    virtual int transformNode(CString& str, TMBool bTransformChilds, int nTab) tm_override;
};


// xml element
// https://www.w3school.com.cn/xmldom/dom_processinginstruction.asp
struct CDomProcessingInstruction : public CDomNodeT<IDomProcessingInstruction>
{
public:
    enum { node_type = NODE_PROCESSING_INSTRUCTION };
    typedef CDomProcessingInstruction                   class_type;
    typedef CDomNode                                    base_class;
    typedef base_class::iterator                        iterator;
    typedef base_class::value_type                      value_type;
    typedef CComQIPtr<IDomProcessingInstruction>        CPtr;

public:
    value_type  m_strText;
    value_type  m_strTarget;

public:
    TM_BGN_INTERFACE_MAP        (IDomProcessingInstruction)
        TM_INTERFACE_HANDLER    (IDomNode);
        TM_INTERFACE_HANDLER    (IDomProcessingInstruction);
        TM_INTERFACE_NODE       ();
    TM_END_INTERFACE_MAP        ();


public:
    ///////////////////////////////////////////////////////////////////////
    // W3C property
    virtual CDomStrPtr get_nodeName() tm_override { return m_strTarget; }
    virtual DOMNodeType get_nodeType() tm_override { return NODE_PROCESSING_INSTRUCTION; }
    virtual CDomStrPtr get_target() tm_override { return m_strTarget; }

    virtual CDomStrPtr get_data() tm_override { return m_strText; }
    virtual void put_data(const CDomStrPtr& txt) tm_override { m_strText = txt; }


    ///////////////////////////////////////////////////////////////////////
    // extend
    virtual int transformNode(CString& str, TMBool bTransformChilds, int nTab) tm_override
    {
        CAutoDomSaveAnalysis _ama("CDomText::transformNode");
        str += L"<?";
        str += m_strTarget;
        str += ' ';
        str += m_strText;
        str += L"?>\r\n";
        return 1;
    }
};


// text element
// http://www.w3school.com.cn/xmldom/dom_text.asp
struct CDomText : public CDomNodeT<IDomText>
{
public:
    enum { node_type = NODE_TEXT };
    typedef CDomText                                    class_type;
    typedef CDomNode                                    base_class;
    typedef base_class::iterator                        iterator;
    typedef base_class::value_type                      value_type;
    typedef CComQIPtr<IDomText>                         CPtr;

protected:
    value_type  m_strText;

public:
    TM_BGN_INTERFACE_MAP        (IDomText)
        TM_INTERFACE_HANDLER    (IDomNode);
        TM_INTERFACE_HANDLER    (IDomText);
        TM_INTERFACE_NODE       ();
    TM_END_INTERFACE_MAP        ();


public:
    ///////////////////////////////////////////////////////////////////////
    // W3C property
    virtual CDomStrPtr get_nodeName() tm_override { return CDomStrView(TM_CT("#text")); }
    virtual DOMNodeType get_nodeType() tm_override { return NODE_TEXT; }
    virtual CDomStrPtr get_data() tm_override { return m_strText; }
    virtual void put_data(const CDomStrPtr& txt) tm_override { m_strText = txt; }

    //W3C
    //    isElementContentWhitespace �ж��ı��ڵ��Ƿ�����հ��ַ����ݡ�?No No No Yes
    virtual long get_length() tm_override { return m_strText.size(); }

    //W3C
    //wholeText ���ĵ��е�˳����˽ڵ㷵�������ı��ڵ�������ı� No No No Yes

    ///////////////////////////////////////////////////////////////////////
    // W3C method

    // W3C
    virtual void appendData(const CDomStrView& txt) tm_override { m_strText += txt; }
    virtual void deleteData(long offset, long count) tm_override { m_strText.erase(offset, count); }
    virtual void insertData(long offset, const CDomStrView& txt) tm_override { m_strText.insert(offset, txt.data(), txt.size()); }
    virtual void replaceData(long offset, long count, const CDomStrView& txt) tm_override { m_strText.replace(offset, count, txt.data(), txt.size()); }

    // W3C not support
    // replaceWholeText() ʹ��ָ���ı����滻�˽ڵ��Լ��������ڵ��ı��ڵ� No No No Yes
    // splitText() ��һ�� Text �ڵ�ָ�������� 6 1 9 Yes
    // substringData() �ӽڵ���ȡ���� 6 1 9 Yes

    ///////////////////////////////////////////////////////////////////////
    // extend
    virtual int transformNode(CString& str, TMBool bTransformChilds, int nTab) tm_override
    {
        CAutoDomSaveAnalysis _ama("CDomText::transformNode");
        CWeakStr strx(m_strText);
        XmlTextEncode(strx, false);
//         str += this->StrEnc(m_strText);
        str += strx;
        return 0;
    }
};


// xml Document
// http://www.w3school.com.cn/xmldom/dom_document.asp
struct CDomDocumentImpl : public CDomNodeT<IDomDocument>
{
public:
    enum { node_type = NODE_DOCUMENT };
    typedef CDomDocumentImpl                            class_type;
    typedef CDomNodeT<IDomDocument>                     base_class;
    typedef base_class::iterator                        iterator;
    typedef base_class::value_type                      value_type;
    typedef CComQIPtr<IDomDocument>                     CPtr;
    friend CDomElementPool;

protected:
//     INotifyDataChanged* m_pSink;
    CDomElementPool m_poor; // used to put new node by createXXXX;
    value_type  m_url;

public:
    TM_UNUSED_ADDREF_RELEASE();

    TM_BGN_INTERFACE_MAP        (IDomDocument)
        TM_INTERFACE_HANDLER    (IDomNode);
        TM_INTERFACE_HANDLER    (IDomDocument);
        TM_INTERFACE_NODE       ();
    TM_END_INTERFACE_MAP        ();

public:
    CDomDocumentImpl();
    ~CDomDocumentImpl();

    ///////////////////////////////////////////////////////////////////////
    // W3C property

    // W3C not support
    // async �涨 XML �ļ��������Ƿ�Ӧ����ͬ�������� 5 1.5 9 No

    // W3C not support
    //    doctype �������ĵ���ص��ĵ���������?(DTD)�� 6 1 9 Yes

    virtual CDomStrPtr get_documentURI() tm_override { return m_url; }
    virtual void put_documentURI(const CDomStrPtr& str) tm_override { m_url = str; }

    // W3C not support
    // domConfig ����normalizeDocument()������ʱ��ʹ�õ�����     No Yes

    // W3C not support
    // implementation ���ش������ĵ��� DOMImplementation ���� No 1 9 Yes
    // inputEncoding ���������ĵ��ı��뷽ʽ���ڽ���ʱ���� No 1 No Yes

    virtual CDomStrPtr get_nodeName() tm_override;

    virtual DOMNodeType get_nodeType() tm_override;

    virtual IDomDocument* get_ownerDocument() tm_override
    {
        return this;
    }

    // W3C not support
    // strictErrorChecking ���û򷵻��Ƿ�ǿ�ƽ��д����顣 No 1 No Yes
    // xmlEncoding �����ĵ��ı��뷽���� No 1 No Yes
    // xmlStandalone ���û򷵻��ĵ��Ƿ�Ϊ standalone�� No 1 No Yes
    // xmlVersion ���û򷵻��ĵ��� XML �汾�� No 1 No Yes

    ///////////////////////////////////////////////////////////////////////
    // W3C method

    // W3C not support
    // adoptNode(sourcenode) ����һ���ĵ����ĵ�ѡ��һ���ڵ㣬Ȼ�󷵻ر�ѡ�ڵ㡣     No Yes

    // W3C not support
    // createAttribute(name) ����ӵ��ָ�����Ƶ����Խڵ㣬�������µ� Attr ���� 6 1 9 Yes
    // createAttributeNS(uri,name) ����ӵ��ָ�����ƺ������ռ�����Խڵ㣬�������µ�?Attr ����     9 Yes
    // createCDATASection() ���� CDATA ���νڵ㡣 5 1 9 Yes
    // createComment() ����ע�ͽڵ㡣 6 1 9 Yes
    // createDocumentFragment() �����յ� DocumentFragment ���󣬲����ش˶��� 5 1 9 Yes

    virtual TM::IDomElement* createElement(const CDomStrPtr& name) tm_override;

    // W3C not support
    // createElementNS() ��������ָ�������ռ��Ԫ�ؽڵ�?No 1 9 Yes
    // createEvent() �����µ� Event ����       Yes
    // createEntityReference(name) ���� EntityReference ���󣬲����ش˶��� 5   No Yes
    // createExpression() ����һ��XPath����ʽ�Թ��Ժ����?      Yes

    virtual IDomProcessingInstruction* createProcessingInstruction(const CDomStrPtr& target, const CDomStrPtr& data) tm_override;
    
    // createRange() ���� Range ���󣬲����ش˶��� No     Yes
    // evaluate() ����һ�� XPath ����ʽ�� No 1 9 Yes

    virtual IDomText* createTextNode(const CDomStrPtr& txt) tm_override;

    // W3C not support
    // getElementById() ���Ҿ���ָ����Ψһ ID ��Ԫ�ء� 5 1 9 Yes
    // getElementsByTagName() �������о���ָ�����Ƶ�Ԫ�ؽڵ㡣 5 1 9 Yes
    // getElementsByTagNameNS() �������о���ָ�����ƺ������ռ��Ԫ�ؽڵ�?No 1 9 Yes
    //    importNode() ��һ���ڵ����һ���ĵ����Ƶ����ĵ��Ա�Ӧ�á�?    9 Yes

    int transformNode(CString& str, TMBool bTransformChilds = true, int nTab = 2) tm_override
    {
        CAutoDomSaveAnalysis _ama("CDomDocumentImpl::transformNode");
        str.reserve(500000);
        return GetNode()->IDomNode::transformNode(str, bTransformChilds, nTab);
    }

    virtual void LoadXml(CXmlTag tag, IDomNode* pParent = 0, EXmlLoadFlag flag = eXmlLoadFlagSpaceXML) tm_override;

    // W3C not support
    //    normalizeDocument()       No Yes
    //    renameNode() ������Ԫ�ػ������Խڵ㡣     No Yes

    // extend
//     TMBool LoadXmlFile(TMCStr fnXml);

    // sample:
    //     CStringW str;
    //     doc.transformNode(str);
    //     doc.SaveXmlFile(pFileName, str);
    static void SaveXmlFile(TMCStr fnXml, const CStringW& xml);

    void clear();

//     void swap(CDomDocumentImpl& ohs)
//     {
//         iterator it1 = GetChild();
//         iterator it2 = ohs.GetChild();
//         GetSelf().InsertChildAt(it2, 0);
//         ohs.GetSelf().InsertChildAt(it1, 0);
//         m_url.Swap(ohs.m_url);
//     }

//     template <class T> void setEventSink(INotifyDataChangedT<T>* p)
//     {
//         m_pSink = (INotifyDataChanged*)p;
//     }
// 
//     virtual INotifyDataChanged* getEventSink() tm_override { return m_pSink; }
};


///////////////////////////////////////////////////////////////////////
// CDomAttr
IDomElement* CDomAttr::get_ownerElement()
{
    if (_parent)
        return CComQIPtr<IDomElement> (GetParent()->GetParent());
    return 0;
}

IDomDocument* CDomAttr::get_ownerDocument()
{
    if (_parent)
        return GetParent()->GetParent()->get_ownerDocument();
    else
        return 0;
}


///////////////////////////////////////////////////////////////////////
// CDomAttrList
// IDomNode* CDomAttrList::item(int n)
// {
//     return GetSelf().GetChildAt(n);
// }
//
// long CDomAttrList::get_length()
// {
//     return GetSelf().GetChildCount();
// }



///////////////////////////////////////////////////////////////////////
// CDomElement
CDomElement::CDomElement()
    : m_aAttr (GetSelf())
{
}

CDomElement::~CDomElement()
{
}


CComPtr<IDomNodeList> CDomElement::get_attributes()
{
    return m_aAttr.get_childNodes();
}

TMBool CDomElement::getAttribute(const CDomStrView& pname, CDomStrPtr& retStr)
{
    IDomAttr* p = getAttributeNode(pname);
    if (!p)
        return false;
    retStr = p->get_value();
    return true;
}

IDomAttr* CDomElement::getAttributeNode(const CDomStrView& name)
{
    for (IDomNode* p = m_aAttr.get_firstChild(); p; p = p->get_nextSibling())
    {
        if (p->get_nodeName().CompareNoCase(name) == 0)
            return static_cast<IDomAttr*>(p);
    }
    return 0;
}

CComPtr<IDomNodeList> CDomElement::getElementsByTagName(const CDomStrView& name)
{
    CDomNodeList* p = new CRefObjectT<CDomNodeList>(0);
    p->AddElementsByTagName(get_firstChild(), name);
    return p;
}

TMBool CDomElement::hasAttributes()
{
    return 0 != m_aAttr.GetChild();
}

TMBool CDomElement::hasAttribute(const CDomStrView& name)
{
    return 0 != getAttributeNode(name);
}

void CDomElement::removeAttribute(const CDomStrView& pname)
{
    IDomAttr* p = getAttributeNode(pname);
    if (p)
        removeAttributeNode(p);
}

void CDomElement::removeAttributeNode(IDomNode* attr)
{
    // check attr is CDomAttr
    DbgAssert(NODE_ATTRIBUTE == attr->get_nodeType());
    CComQIPtr<CDomNode, &TM_UUID_OF(CDomNode_)> ptr(attr);
    if (ptr)
    {
        ptr->Detach();
        attr->Release();
    }
}

IDomAttr* CDomElement::setAttribute(const CDomStrPtr& name, const CDomStrPtr& pvalue)
{
    IDomAttr* p = getAttributeNode(name);
    if (p)
    {
        p->put_nodeValue(pvalue);
        return p;
    }

    CDomAttr* p2 = new CRefObjectT<CDomAttr>(1);
    p2->m_strName = name;
    p2->m_strValue = pvalue;
    m_aAttr.appendChild(p2->GetNode());
    return p2;
}

int CDomElement::transformNode(CString& strx, TMBool bTransformChilds, int nTab)
{
    CAutoDomSaveAnalysis _ama("CDomElement::transformNode");

    int nCount = 1;
    CStdStr str;

    // write tab
    str.append((DomLevel(this) - 1) * nTab, ' ');

    // write name
    str += TM_T('<');
    str += m_strName;

    // write prop
//    CHECKMEMORY();
    CAutoDomSaveAnalysis _ama1("CDomElement::transformNode::write prop");
    for (IDomNode* p = m_aAttr.get_firstChild(); p; p = p->get_nextSibling())
    {
//         CComQIPtr<CDomAttr> ptr(p);

        str += ' ';
        str += p->get_nodeName();
        str.append(TM_T("=\""), 2);
        CAutoDomSaveAnalysis _ama2("CDomElement::transformNode::write prop::StrEnc");
        CWeakStr data(p->get_nodeValue());
        XmlTextEncode(data, true);
        str += data;
        str += '\"';
    }
//     str += L'>';

//     if (hasChildNodes())
//         str.append(L"\r\n", 2);

//     if (bTransformChilds)
    {
        CAutoDomSaveAnalysis _ama3("CDomElement::transformNode::write child");
        CString strc;
        int n = GetNode()->IDomNode::transformNode(strc, bTransformChilds, nTab);
        if (strc.IsEmpty()) // no child
        {
            str.append(TM_T(" />\r\n"), 5);
        }
        else
        {
            str += L'>';
            if (n)
            {
                str.append(TM_T("\r\n"), 2);
                str += strc;
                str.append((DomLevel(this) - 1) * nTab, ' ');
            }
            else // only have text child
                str += strc;
            str.append(TM_T("</"), 2);
            str += m_strName;
            str.append(TM_T(">\r\n"), 3);
        }
        nCount += n;
    }

    strx += str;
//     DbgAssert(strx.GetLength() == StrLen(strx.c_str()));
    return nCount;
}


///////////////////////////////////////////////////////////////////////
// CDomElementPool
CDomElementPool::CDomElementPool(CDomNodeT<IDomNode>* pOwner)
    : m_pOwner (pOwner)
{
}

CDomElementPool::~CDomElementPool()
{
    removeAllChild();
}

IDomDocument* CDomElementPool::get_ownerDocument()
{
    return m_pOwner->get_ownerDocument();
}


///////////////////////////////////////////////////////////////////////
// CDomDocumentImpl
CDomDocumentImpl::CDomDocumentImpl()
//     : m_pSink (0)
    : m_poor (GetSelf())
{
}

CDomDocumentImpl::~CDomDocumentImpl()
{
    clear();
}

CDomStrPtr CDomDocumentImpl::get_nodeName()
{
    return CDomStrPtr(TM_CT("#document"));
}

DOMNodeType CDomDocumentImpl::get_nodeType()
{
    return NODE_DOCUMENT;
}

IDomElement* CDomDocumentImpl::createElement(const CDomStrPtr& name)
{
    IDomElement* p;
    CComQIPtr<IDomNodeCreator> ptr(this);
    if (ptr)
        p = ptr->createElement(name);
    else
        p = ((IDomNodeCreator*)0)->IDomNodeCreator::createElement(name);
//     DbgAssert(!m_poor.get_firstChild());
    m_poor.appendChild(p);
    return p;
}

IDomProcessingInstruction* CDomDocumentImpl::createProcessingInstruction(const CDomStrPtr& target, const CDomStrPtr& data)
{
    IDomProcessingInstruction* p;
    CComQIPtr<IDomNodeCreator> ptr(this);
    if (ptr)
        p = ptr->createProcessingInstruction(target, data);
    else
        p = ((IDomNodeCreator*)0)->IDomNodeCreator::createProcessingInstruction(target, data);
//     DbgAssert(!m_poor.get_firstChild());
    m_poor.appendChild(p);
    return p;
}


IDomText* CDomDocumentImpl::createTextNode(const CDomStrPtr& txt)
{
    IDomText* p;
    CComQIPtr<IDomNodeCreator> ptr(this);
    if (ptr)
        p = ptr->createTextNode(txt);
    else
        p = ((IDomNodeCreator*)0)->IDomNodeCreator::createTextNode(txt);
//     DbgAssert(!m_poor.get_firstChild());
    m_poor.appendChild(p);
    return p;
}


struct CDomDocument_Parse : public IDomParser
{
    IDomDocument* m_pDoc;
    IDomNode* m_pRoot;
    IDomNode* m_pCurrent;
    TMUInt32 m_flag;

    CDomDocument_Parse(IDomDocument* pDoc, IDomNode* pRoot)
        : m_pDoc (pDoc)
        , m_pRoot (pRoot)
        , m_pCurrent (pRoot)
    {
        DbgAssert(pRoot);
    }

    virtual void AddNode(const CDomStrPtr& tag) tm_override
    {
        DbgAssert(m_pCurrent);
        m_pCurrent = m_pDoc->appendChildElement(m_pCurrent, tag);
    }

    virtual void EndNode() tm_override
    {
        DbgAssert(m_pCurrent);
        IDomNode* pp = m_pCurrent->get_parentNode();
        if (pp)
            m_pCurrent = pp;
    }

    virtual void AddProperty(const CDomStrPtr& name, const CDomStrPtr& val) tm_override
    {
        DbgAssert(m_pCurrent);
        CComQIPtr<IDomElement> ptr(m_pCurrent);
        if (ptr)
            ptr->setAttribute(name, val);
        else
            DbgTrace(L"CDomDocument_Parse::AddProperty(%s) - can't add property", CString(name).c_str());
    }

    virtual void EndPropertys() tm_override {}
    
    virtual void AddText(const CDomStrPtr& text) tm_override
    {
        DbgAssert(m_pCurrent);
        if (!text.IsEmpty())
            m_pCurrent->appendChild(m_pDoc->createTextNode(text));
    }

    virtual void AddProcessingInstruction(const CDomStrPtr& target, const CDomStrPtr& data) tm_override
    {
        CComPtr<IDomProcessingInstruction> ptr(m_pDoc->createProcessingInstruction(target, data));
        m_pDoc->appendChild(ptr);
    }
};

void CDomDocumentImpl::LoadXml(CXmlTag tag, IDomNode* pParent, EXmlLoadFlag flag)
{
    CAutoMsgAnalysis ama("CDomDocumentImpl::LoadXml");

    if (!pParent)
        pParent = this;
    pParent->removeAllChild();

    CDomDocument_Parse parse(this, pParent);
    parse.m_flag = flag;
    DomParser(tag, &parse, flag);

// #ifdef TM_DBG
//     CStringW strXml;
//     pParent->transformNode(strXml);
//     DbgTrace(strXml.c_str());
// #endif
}

void CDomDocumentImpl::SaveXmlFile(TMCStr fnXml, const CStringW& xml)
{
    CFile file;
    file.CreateAlways(fnXml, file.e_faWrite);
    TxtFileSaveAs(xml, e_txtFileStyleUtf8, file);
}

void CDomDocumentImpl::clear()
{
    removeAllChild();
    m_poor.removeAllChild();
}

IDomElement* IDomNodeCreator::createElement(const CDomStrPtr& tag)
{
    TM::CDomElement* p = new TM::CRefObjectT<TM::CDomElement>(1);
    p->put_tagName(tag);
    return p;
}

IDomText* IDomNodeCreator::createTextNode(const CDomStrPtr& txt)
{
    TM::CDomText* p = new TM::CRefObjectT<TM::CDomText>(1);
    p->put_data(txt);
    return p;
}

IDomProcessingInstruction* IDomNodeCreator::createProcessingInstruction(const CDomStrPtr& target, const CDomStrPtr& data)
{
    TM::CDomProcessingInstruction* p = new TM::CRefObjectT<TM::CDomProcessingInstruction>(1);
    p->m_strText = data;
    p->m_strTarget = target;
    return p;
}


TM_END_NAMESPACE


struct CDomNodeDataSourceContainer
{
    inline static TM::IDomNode* GetParent(TM::IDomNode* p)
    {
        DbgAssert(p);
        return p->get_parentNode();
    }

    inline static TM::IDomNode* GetChild(TM::IDomNode* p)
    {
        DbgAssert(p);
        return p->get_firstChild();
    }

    inline static TM::IDomNode* GetNext(TM::IDomNode* p)
    {
        DbgAssert(p);
        return p->get_nextSibling();
    }

    inline static TM::IDomNode* GetPrev(TM::IDomNode* p)
    {
        DbgAssert(p);
        return p->get_previousSibling();
    }
};

typedef TM::CTreeItemT<TM::IDomNode*, CDomNodeDataSourceContainer>          CDomNodeRec;

void DomCreate(TM::IDomDocument** ppDoc)
{
    DbgAssert(ppDoc);
    *ppDoc = new TM::CRefObjectT<TM::CDomDocumentImpl>(1);
}

void DomEnum(TM::IDomNode* pNode, PFNDOMENUM proc, TMLParam lParam)
{
    CComQIPtr<TM::CDomNode, &TM_UUID_OF(CDomNode_)> ptr(pNode);
    if (ptr)
        ptr->Enum2(proc, lParam);
}

TMBool DomSave(TMCStr fnXml, const TM::CString& xml)
{
    return TxtFileSaveAs(xml, e_txtFileStyleUtf8NoBOM, fnXml);
}

void DomRemoveChild(TM::IDomNode* pNode)
{
    for (TM::IDomNode* p; 0 != (p = pNode->get_firstChild());)
        pNode->removeChild(p);
}

TMBool DomIsChild(TM::IDomNode* pNode, TM::IDomNode* pChild)
{
    DbgAssert(pChild);
    return CDomNodeRec(pNode).IsChild(pChild);
}

TMBool DomFindChild(TM::IDomNode* pNode, TM::IDomNode* pChild)
{
    DbgAssert(pNode);
    if (pNode == pChild)
        return true;
    for (pNode = pNode->get_firstChild(); pNode; pNode = pNode->get_nextSibling())
    {
        if (DomFindChild(pNode, pChild))
            return true;
    }
    return false;
}

int DomLevel(TM::IDomNode* pNode)
{
    DbgAssert(pNode);
    return CDomNodeRec(pNode).GetLevel();
}

int DomChildCount(TM::IDomNode* pNode)
{
    return CDomNodeRec(pNode).GetChildCount();
}

int DomChildAllCount(TM::IDomNode* pNode)
{
    return CDomNodeRec(pNode).GetAllChildCount();
}

TM::IDomNode* DomRoot(TM::IDomNode* pNode)
{
    return CDomNodeRec(pNode).GetRoot();
}

void DomSetZorder(TM::IDomNode* pNode, TM::IDomNode* hIterator, EZOrder eZOrderOrIndex)
{
    DbgAssert(pNode);

    if (!hIterator)
    {
        if (eZOrderOrIndex == eZOrderUp)
            eZOrderOrIndex = eZOrderChildFirst;
        else if (eZOrderOrIndex == eZOrderDown)
            eZOrderOrIndex = eZOrderChildLast;
        hIterator = pNode->get_parentNode();
        DbgAssert(hIterator);
    }

    switch (eZOrderOrIndex)
    {
    case eZOrderUp:
        hIterator->insertBefore(pNode, hIterator);
        return;

    case eZOrderDown:
        {
            TM::IDomNode* p = hIterator->get_nextSibling();
            if (p)
                p->insertBefore(pNode, p);
            else
                hIterator->get_parentNode()->appendChild(pNode);
        }
        return;

    case eZOrderChildLast:
        hIterator->appendChild(pNode);
        return;

    default:
        break;
    }

    // eZOrderChildFirst or InsertChildN.
    if (!hIterator->get_firstChild())
        hIterator->appendChild(pNode);
    else if (eZOrderOrIndex == 0)
        hIterator->insertBefore(pNode, hIterator->get_firstChild());
    else
    {
        TM::IDomNode* p = hIterator->get_firstChild();
        TM::IDomNode* pn = p->get_nextSibling();
        int nPos = eZOrderOrIndex;
        for (--nPos; pn && nPos > 0; p = pn, pn = pn->get_nextSibling(), --nPos)
        {
        }

        DbgAssert(p);
        if (p != pNode)
            p->insertAfter(pNode);
    }
}


TM_BGN_NAMESPACE

struct CDocument_Parse : public IXmlParser
{
    enum
    {
        eTypeMask       = 0x0f,
        eCloseOnEndProp = 0x10,
        ePrevSpace      = 0x40,
        eInPre          = 0x80
    };

    const TM::CXmlTag&  m_xml;
    TMUInt32            m_flag;
    IDomParser*         m_parser;
    CVector<CStdStr>    m_aNode;
    CVector<TMCharT>    m_strCache;
    CVector<size_t>     m_aStrCache;

    TM_DECLARE_FLAG2(Preserve,          m_flag, eXmlLoadFlagSpacePreserve,  eTypeMask);
    TM_DECLARE_FLAG2(XML,               m_flag, eXmlLoadFlagSpaceXML,       eTypeMask);
    TM_DECLARE_FLAG2(HTML,              m_flag, eXmlLoadFlagHTML,           eTypeMask);

    TM_DECLARE_FLAG (CloseOnEndProp,    m_flag, eCloseOnEndProp);
    TM_DECLARE_FLAG (PrevSpace,         m_flag, ePrevSpace);
    TM_DECLARE_FLAG (InPre,             m_flag, eInPre);

    typedef CString     value_type;

    void PushStr(TMCStr p, size_t n)
    {
        m_aStrCache.push_back(m_strCache.size());
        m_aStrCache.push_back(n);
        m_strCache.insert(m_strCache.end(), p, p + n);
        m_strCache.push_back(0);
    }

    CDocument_Parse(const TM::CXmlTag& xml, IDomParser* parser, EXmlLoadFlag flag)
        : m_xml(xml)
        , m_flag (flag)
        , m_parser(parser)
    {
    }

    void _EndNode()
    {
        m_aNode.pop_back();
        m_parser->EndNode();
    }

    virtual void AddNode(XmlRange tag) tm_final
    {
        CAutoMsgAnalysis ama("CDocument_Parse::AddNode");

        CStdStr str(m_xml.Mid(tag.m_data, tag.m_size));
        if (IsHTML())
        {
            DbgAssert(!IsCloseOnEndProp());
            str.MakeLower();
            if (str == TM_T("img") || str == TM_T("br") || str == TM_T("hr") || str == TM_T("input") || str == TM_T("meta"))
                SetCloseOnEndProp();
            else if (str == TM_T("pre"))
                SetInPre();
        }

        PushStr(str.data(), tag.m_size);
        m_aNode.push_back(str);
    }

    virtual void EndNode(XmlRange name) tm_final
    {
        CAutoMsgAnalysis ama("CDocument_Parse::EndNode");

        CStdStr strName(m_xml.Mid(name.m_data, name.m_size));
        if (IsHTML())
        {
            strName.MakeLower();
            if (strName == TM_T("pre"))
                ResetInPre();
        }
        for (int nSize = m_aNode.size(); nSize-- > 0;)
        {
            if (m_aNode[nSize] == strName)
            {
                for (int n = m_aNode.size(); n-- > nSize;)
                    _EndNode();
                return;
            }
        }
        DbgTrace(L"CDomDocument_Parse::EndNode(%s) - can't find end tag", strName.c_str());
    }

    virtual void AddProperty(XmlRange name, XmlRange value) tm_final
    {
        CAutoMsgAnalysis ama("CDocument_Parse::AddProperty");
//         CStringW data;
//         CDomNode::StrDec(data, value);
        CWeakStr data(m_xml.Mid(value.m_data, value.m_size));
        XmlTextDecode(data, true);

        PushStr(m_xml.data() + name.m_data, name.m_size);
        PushStr(data.data(), data.size());
    }

    virtual void EndPropertys(XmlRange tag, XmlRange props) tm_final
    {
        CAutoMsgAnalysis ama("CDocument_Parse::EndPropertys");
        CDomStrPtr s;
        s.MoveFromCustomString(m_strCache);
        m_parser->AddNode(s.Left(m_aStrCache[1]));
        for (int i = 2; i < m_aStrCache.size(); i += 4)
            m_parser->AddProperty(
                s.Mid(m_aStrCache[i], m_aStrCache[i + 1]),
                s.Mid(m_aStrCache[i + 2], m_aStrCache[i + 3]));
        m_parser->EndPropertys();
        m_aStrCache.clear();
        DbgAssert(m_strCache.empty());

        if (IsCloseOnEndProp())
        {
            _EndNode();
            ResetCloseOnEndProp();
        }
    }

    virtual void AddText(size_t endPrevTag, size_t beforeText, size_t endText, size_t beforeNextTag) tm_final
    {
        CAutoMsgAnalysis ama("CDocument_Parse::AddText");
        CXmlTag xml;
        if (IsXML())
            xml.Assign(m_xml.data() + beforeText, m_xml.data() + endText);
        else
            xml.Assign(m_xml.data() + endPrevTag, m_xml.data() + beforeNextTag);
//         value_type data;
//         CDomNode::StrDec(data, xml);
        CWeakStr data(xml);
        if (IsInPre())
            XmlTextDecode(data, true);
        else
        {
            XmlTextDecode(data, false);
            if (!IsPreserve() && !data.IsEmpty())
            {
                if (endPrevTag != beforeText && !IsPrevSpace()) // if last tag have space then ignore space
                    data.insert(data.begin(), 1, ' ');
                SettingPrevSpace(endText != beforeNextTag);
                if (IsPrevSpace())
                    data.append(1, ' ');
            }
        }

        if (!data.IsEmpty())
            m_parser->AddText(data);
    }

    virtual void AddProcessingInstruction(XmlRange target, XmlRange data) tm_final
    {
        m_parser->AddProcessingInstruction(m_xml.Mid(target.m_data, target.m_size), m_xml.Mid(target.m_data, target.m_size));
    }
};

TM_END_NAMESPACE

void DomParser(TM::CXmlTag xml, IDomParser* pCallback, EXmlLoadFlag flag)
{
    if (xml.empty())
        return;
    if (flag == eXmlLoadFlagText)
        pCallback->AddText(xml);
    else
    {
        TM::CDocument_Parse parser(xml, pCallback, flag);
        XmlParse(xml, &parser);
    }
}

