/*
 *  @file
 *  @brief  XML config file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.03.18
 *
 *  Copyright (C) 2015 miragekiller
 */

#pragma once

#ifndef TMXMLCFG_H
#define TMXMLCFG_H

#include "tmdom.h"


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// Profile DDX
template <TMBool _bLoad> struct XMLCFG_DDX_NODE
{
    TM::CDomElement::CPtr m_ptr;
    enum { bLoad = true };

    XMLCFG_DDX_NODE(IDomNode* pNode)
        : m_ptr (pNode)
    {
    }

public:
    operator IDomElement* () { return m_ptr; }

    IDomElement* Entry(TMCStrW pSection)
    {
        IDomElement* pOld = m_ptr;
        if (m_ptr)
            m_ptr = m_ptr->selectSingleNode(pSection);
        return pOld;
    }

    TMBool DDX_Text(TMCStr pKey, CString& var)
    {
        if (!*this)
            return false;
        CDomStrView rg;
        if (!m_ptr->getAttribute(pKey, rg))
            return false;
        var = rg;
        return true;
    }

    void DDX_Text(TMCStr pKey, CString& var, DWORD dwLen, TMCStr pDft)
    {
        if (!DDX_Text(pKey, var) && pDft)
            var = pDft;
    }

    void DDX_Text(TMCStr pKey, LPTSTR var, DWORD dwLen, TMCStr pDft)
    {
        CString str;
        if (DDX_Text(pKey, str))
            StrCopy(var, dwLen, str.c_str());
        else if (pDft)
            StrCopy(var, dwLen, pDft);
    }

    template < class T >
    void DDX_Int(TMCStr pKey, T& nVar, int dft)
    {
        CString str;
        if (!DDX_Text(pKey, str))
            nVar = dft;
        else
            nVar = str.ToLong();
    }
    
    void DDX_Float(TMCStr pKey, float& fVar, float dft)
    {
        CString str;
        if (!DDX_Text(pKey, str))
            fVar = dft;
        else
            fVar = str.ToDouble();
    }

//     template < class T >
//     void DDX_Struct (TMCStr pk, T& var)
//     {
//         GetPrivateProfileStruct (ps, pk, &var, sizeof(T), pf);
//     }
};


TM_TEMPLATE_NULL struct XMLCFG_DDX_NODE<false>
{
    TM::CDomElement::CPtr m_ptr;
    enum { bLoad = false };

    XMLCFG_DDX_NODE(IDomNode* pNode)
        : m_ptr(pNode)
    {
    }

//     XMLCFG_DDX_NODE(TM::CDomNode* ptr)
//         : TM::CDomElement::CPtr (ptr)
//     {
//     }
    
public:
    operator IDomElement* () { return m_ptr; }

    // return old
    IDomNode* Entry(TMCStrW pSection)
    {
        IDomNode* pOld = m_ptr;
        IDomNode* p = m_ptr->selectSingleNode(pSection);
        if (!p)
            p = m_ptr->appendChildElement(pSection);
        m_ptr = p;
        return pOld;
    }

    void DDX_Text(TMCStr pKey, const CDomElement::value_type& var)
    {
        DbgAssert(m_ptr);
        m_ptr->setAttribute(pKey, var);
    }

    template <class T>
    void DDX_Text(TMCStr pKey, const T& var, DWORD dwLen, TMCStr dft)
    {
        DDX_Text(pKey, var);
    }
    
    template < class T >
    void DDX_Int(TMCStr pKey, T& nVar, int dft)
    {
        TCHAR c[MAX_PATH];
        _ltot(nVar, c, 10);
        DDX_Text(pKey, c);
    }
    
    void DDX_Float (TMCStr pKey, float& fVar, float dft)
    {
        TCHAR c[MAX_PATH];
        StrFromF(fVar, c);
        DDX_Text(pKey, c);
    }

//     template < class T >
//     void DDX_Struct (TMCStr pk, T& var)
//     {
//         GetPrivateProfileStruct (ps, pk, &var, sizeof(T), pf);
//     }
};

// template <TMBool bLoad> 
// struct XMLCFG_DDX : public CDomDocument
// {
//     typedef XMLCFG_DDX_NODE<bLoad> value_type;
// 
//     XMLCFG_DDX(TMCStr pFile)
//     {
//         LoadXmlFile(pFile);
//     }
//     
//     void Save(TMCStr pName)
//     {
//         if (!bLoad)
//         {
//             CStringW str;
//             transformNode(str);
//             SaveXmlFile(pName, str);
//         }
//     }
// };

typedef XMLCFG_DDX_NODE<true>    XMLCFG_DDX_LOAD;
typedef XMLCFG_DDX_NODE<false>   XMLCFG_DDX_SAVE;

#define BGN_XMLCFG_DDX_MAP_BASE() \
    template <class T > BOOL DoXmlCfgExchange (T& node) { // TM::CDomNode::CPtr parent(&pd);

#define BGN_XMLCFG_DDX_MAP(thisClass) \
    virtual void ProfileExchangeLoad (TM::IDomNode* pNode) { TM::XMLCFG_DDX_LOAD _l(pNode); DoXmlCfgExchange(_l); } \
    virtual void ProfileExchangeSave (TM::IDomNode* pNode) { TM::XMLCFG_DDX_SAVE _s(pNode); DoXmlCfgExchange(_s); } \
    BGN_XMLCFG_DDX_MAP_BASE()    

// section use XPath format
#define XMLCFG_DDX_ENTRY(section)       { TM::IDomNode* pOld = node.Entry(section);
#define XMLCFG_DDX_LEAVE()              node = pOld; }

#define XMLCFG_DDX_TEXT2(key,var,dft)   node.DDX_Text(key, var, sizeof(var), dft);

#define XMLCFG_DDX_INT2(key,var,dft)    node.DDX_Int(key, var, dft);

#define XMLCFG_DDX_FLOAT2(key,var,dft)  node.DDX_Float(key, var, dft);

#define XMLCFG_DDX_STRUCT(key,var)      node.DDX_Struct(key, var);


#define XMLCFG_DDX_TEXT(key,var)        XMLCFG_DDX_TEXT2(key, var, _T(""))
#define XMLCFG_DDX_INT(key,var)         XMLCFG_DDX_INT2(key, var, 0)
#define XMLCFG_DDX_FLOAT(key,var)       XMLCFG_DDX_FLOAT2(key, var, 0)
#define XMLCFG_CHAIN_DDX_MAP(base)      base::DoXmlCfgExchange(node)

#define END_XMLCFG_DDX_MAP()            return TRUE; }


class CXmlCfg
{
protected:
    TM::CDomDocument m_doc;
    TM::CString m_strName;

    CXmlCfg(TMCStr pName)
    {
        Open(pName);
    }

    CXmlCfg()
    {
    }

    void Open(TMCStr pName)
    {
        m_strName = pName;
        m_doc.LoadXmlFile(pName);
    }

    void Save()
    {
        CStringW str;
        m_doc.transformNode(str);
        m_doc.SaveXmlFile(m_strName, str);
    }

    IDomNode* GetSettingNode(TMCStrW pNodeName, TMBool bCreate)
    {
        if (!pNodeName)
            return &m_doc;
        
        TM::CDomNode::CPtr parent(&m_doc);
        TM::IDomNode* p = parent->selectSingleNode(pNodeName);
        if (bCreate && !p)
            p = parent->appendChildElement(pNodeName);
        return p;
    }
};



TM_END_NAMESPACE


#endif // TMXMLCFG_H

