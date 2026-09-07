/**
 *  @file
 *  @brief  xml
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2004.8.1
 *
 *  Copyright (C) 2004 miragekiller
 */

#pragma once

#ifndef TMXML_H
#define TMXML_H

#include "tmstring.h"
#include "tmcolor.h"    // XmlParseColor


TM_BGN_NAMESPACE
typedef CStrView CXmlTag;
TM_END_NAMESPACE


/**
 *  @addtogroup xml
 *  @{
 */

///////////////////////////////////////////////////////////////////////////////
// IXmlParser && XmlParse
// <aaa  x=bbb c=cccc>abcdfdfdfd<bbb>ddd</bbb>cba</aaa>
// |         |      | |        |                |     |
// 1         2      2 3        4                5     6
// 1: AddNode
// 2: AddProperty
// 3: AddValue
// 4: AddText
// 5: AddText,EndValue
// 6: EndNode
typedef TMRangeT<size_t> XmlRange;
class TM_NO_VTABLE IXmlParser
{
public:
    virtual void AddNode(XmlRange tag) {}
    virtual void EndNode(XmlRange tag) {}
    virtual void AddProperty(XmlRange name, XmlRange val) {}
    virtual void EndPropertys(XmlRange tag, XmlRange props) {}
    virtual void AddText(size_t endPrevTag, size_t beforeText, size_t endText, size_t beforeNextTag) {}
    virtual void AddProcessingInstruction(XmlRange target, XmlRange data) {} // <?xxx ?>
    virtual void AddComment(XmlRange commnet) {} // <!-- xxx -->
    virtual void EndDoc() {}
};

/**
 *  parse XML string
 *  @par sample
 *  @code
    CParserProp ps(pName);
    XmlParse("<a>bb<cc>d</cc>b</a>", &ps);
 *  @endcode
 */
TM_CAPI void XmlParse(TM::CXmlTag xml, IXmlParser* pCallback);



///////////////////////////////////////////////////////////////////////////////
// _CParserProp
struct _CParserProp : public IXmlParser
{
    TM::CXmlTag _tag, _name, _val;
    TMBool _b;

    _CParserProp(TM::CXmlTag tag, TM::CXmlTag name)
        : _tag(tag)
        , _name(name)
        , _b(false)
    {
        XmlParse(tag, this);
    }

    virtual void AddProperty(XmlRange name, XmlRange val) tm_final
    {
        TM::CXmlTag _v(_tag.Mid(val.m_data, val.m_size));
        if (_name == _v)
        {
            _b = true;
            _val = _v;
        }
    }
};

/**
 *  parse XML property
 *  @par sample
 *  @code
    CStringA ps;
    XmlGetProperty(L"<a aa="1" bb="3" ></a>", L"bb", ps); // return: true; ps="3";
 *  @endcode
 */
template <class T>
TM_INLINE TMBool XmlGetProperty(TM::CXmlTag xml, TM::CXmlTag name, T& value)
{
    _CParserProp ps(xml, name);
    if (ps._b)
        return value = ps._val, true;
    return false;
}


/**
 *  encode xml text: " \r\n\t <>&\'\""
 *  @code
    CWeakStrW str(L"\r  abc&");
    XmlTextEncode(str, true);  // return str: "&#13;  abc&amp;"
    XmlTextEncode(str, false); // return str: "<br/>  &nbsp;abc&amp;"
 *  @endcode
 */
TM_CAPI void XmlTextEncode(TM::CWeakStr& str, TMBool bProperty);

/**
*  @code
    CWeakStrW str(L"&#13;\r  abc&amp;");
 *  XmlTextDecode(str, false): // return str: "\r abc&"
 *  XmlTextDecode(str, true):  // return str: "\r\r  abc&"
 *  @see XmlTextDecode
 */
TM_CAPI void XmlTextDecode(TM::CWeakStr& str, TMBool bProperty); // decode "pre" tag can set bProperty = 1;

/**
 *  parse text to color
 *  @code
    XmlParseColor("0xfab");            // return 0xffffaabb
    XmlParseColor("0xff000000");       // return 0xff000000
    XmlParseColor("rgb(100%,0%,20%)"); // return 0xffff0033
    XmlParseColor("rgb(255,0,33)");    // return 0xffff0033
    XmlParseColor("red");              // return 0xffff0000
 *  @endcode
 */
TM_CAPI TMColor XmlParseColor(TM::CXmlTag str);


/** @} */


#endif // TMXML_H
