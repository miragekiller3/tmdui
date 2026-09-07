/**
 *  @file
 *  @brief  xml
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2004.8.1
 *
 *  Copyright (C) 2004 miragekiller
 */

#include "tmxml.h"
#include "tmlist.h"


enum ETokenType
{
    e_ttEof         = 0,
    e_ttLeft        = '<',
    e_ttRight       = '>',
    e_ttEnd         = '/',      // "</xxx"
    e_ttEqual       = '=',
//     e_ttComment     = '!',      // <!-- comment -->
//     e_ttQuestion    = '?',      // <?xml

    // complex type
    e_ttStr         = ' ',
    e_ttREnd        = ('/>'),   // "/>"
};



TM_BGN_NAMESPACE


template<class T> class CXmlTokenT : public XmlRange // : public TMWStrRange // CXmlTag // TMWStrRange
{
public:
    typedef const T* iterator;

    const T*    m_src;

    CXmlTokenT() {}

    CXmlTokenT(const CXmlTag& xml)
        : m_src(xml.data())
    {
        m_data = 0;
        m_size = xml.size();
    }

//     XmlRange& ToTag() { return *this; }
    XmlRange  ToTag(size_t b, size_t sz) { XmlRange rg = { b, sz }; return rg; }

private:
    inline static bool IsInvalidChar(T c)
    {
        return (c > 0 && c <= 0x20) || (c == 0x7f);
    }

    bool IsEOF()
    {
        return (m_size <= m_data) || !m_src[m_data];
    }

    bool IsLeftAfter(T c)
    {
        return c == '/' || c == '!' || c == '?' || iswalpha(c);
    }

    // sample: aaaa<b x="aa">ccc</b><dd/> xxx
    // return           val     m_data
    // e_ttEof          NL      NL
    // e_ttLeft='<'     b       x="aa">ccc</b><dd/>
    // e_ttRight='>'    NL      ccc</b><dd/>
    // e_ttEnd='/'      NL      <dd/>
    // e_ttEqual='='    aa      >ccc</b><dd/>
    // e_ttComment='!'  ?       ?
    // e_ttStr=' '      ccc     </b><dd/>
    // e_ttREnd='/>'    NL       xxx
    ETokenType Next(XmlRange& val)
    {
        // skip space && control character
        for (; ; ++m_data)
        {
            if (IsEOF())
                return e_ttEof;

            if (!IsInvalidChar(m_src[m_data]))
                break;
        }

        val.m_data = m_data;

        T tmp = m_src[m_data];
        switch (tmp)
        {
        case '>':
            ++m_data;
            return e_ttRight;

        case '/':
            {
                ++m_data;
                ETokenType t = Next(val);
                if (t == e_ttStr)
                    return e_ttEnd;
                else if (t == e_ttRight)
                    return e_ttREnd;
                else
                    return t;
            }

        case '=':
            {
                ++m_data;
                ETokenType t = Next(val);
                if (t == e_ttStr)
                    return e_ttEqual;
                else
                    return t;
            }

        case '\'':
        case '\"':
            ++m_data;
            val.m_data = m_data;
            for (; !IsEOF(); ++m_data)
            {
                if (tmp == m_src[m_data])
                {
                    val.m_size = m_data - val.m_data;
                    ++m_data;
                    return e_ttStr;
                }
            }
            return e_ttEof;

        default:
            val.m_data = m_data;
            for (; !IsEOF(); ++m_data)
            {
                T ch = m_src[m_data];
                if (IsInvalidChar(ch) || (ch == '=') || (ch == '>') || (ch == '<') || (ch == '/'))
                {
                    val.m_size = m_data- val.m_data;
                    return e_ttStr;
                }
            }
            return e_ttEof;
        }
    }

public:
    void Parser(IXmlParser* pCallback)
    {
        int nSkipErrorTag = 0;
        for (;;)
        {
            // find text
            {
                size_t endPrevTag = m_data + nSkipErrorTag;
                // skip space && control character
                for (; IsInvalidChar(m_src[m_data]); ++m_data)
                {
                    if (IsEOF())
                        return;
                }
                size_t beforeText = m_data + nSkipErrorTag;
                nSkipErrorTag = 0;
                size_t endText = m_data;
                for (; ; ++m_data)
                {
                    if (IsEOF())
                        break;
                    if ('<' == m_src[m_data] && IsLeftAfter(m_src[m_data + 1]))
                        break;
                    if (!IsInvalidChar(m_src[m_data]))
                        endText = m_data + 1;
                }
                size_t beforeNextTag = m_data;
                if (beforeNextTag != endPrevTag)
                    pCallback->AddText(endPrevTag, beforeText, endText, beforeNextTag);
//                 DbgTrace("AddText");
//                 DbgTrace(endPrevTag);
                if (IsEOF())
                    return;
            }

            DbgAssert(m_src[m_data] == '<');
            if (m_src[m_data + 1] == '/')
            {
                m_data += 2;
                size_t tagName = m_data;
                for (; '>' != m_src[m_data]; ++m_data)
                {
                    if (IsEOF())
                        return;
                }
                pCallback->EndNode(ToTag(tagName, m_data - tagName));
                ++m_data;
                continue;
            }
            else if (m_src[m_data + 1] == '?') // skip <?xml version="1.0" encoding="utf-8"?>
            {
                size_t x = CXmlTag(m_src + m_data, m_size).find(L' ');
                size_t y = CXmlTag(m_src + m_data, m_size).find(TM_T("?>"), 2);
                if (x != CXmlTag::npos && y != CXmlTag::npos && x < y)
                {
                    pCallback->AddProcessingInstruction(ToTag(m_data + 2, x - 2), ToTag(m_data + x + 1, y - x - 1));
                    m_data += (y + 2);
                }
                else
                {
                    m_data += 2;
                    nSkipErrorTag = -2;
                }
                continue;
            }
            else if (m_src[m_data + 1] == '!' && m_src[m_data + 2] == '-' && m_src[m_data + 3] == '-')
            {
                size_t y = CXmlTag(m_src + m_data, m_size).find(TM_T("-->"), 3);
                if (y != CXmlTag::npos)
                {
                    pCallback->AddComment(ToTag(m_data + 4, y - 4));
                    m_data += (y + 3);
                }
                else
                {
                    m_data += 3;
                    nSkipErrorTag = -2;
                }
                continue;
            }

            DbgAssert(m_src[m_data] == '<');
            ++m_data;
            XmlRange tagName;
            ETokenType tp = Next(tagName);
            if (tp != e_ttStr)
            {
                DbgTrace("CXmlTokenT::Parser error: haven't tag name");
                return; // error
            }
            pCallback->AddNode(tagName);

            // get property
            XmlRange xt;
            for (; ;)
            {
                XmlRange xtTmp;
                tp = Next(xtTmp);

                if (tp == e_ttStr)
                {
                    if (!xtTmp.m_size)
                    {
                        DbgTrace(L"parse XML/HTML format error: %s", CStringW(m_src + tagName.m_data, 128).c_str());
                        break;
                    }
                    xt = xtTmp;
                }
                else if (tp == e_ttEqual)
                {
                    pCallback->AddProperty(xt, xtTmp);
//                     DbgTrace("add prop");
//                     DbgTrace(xt.begin());
                }
                else if (tp == e_ttRight)
                {
                    pCallback->EndPropertys(tagName, ToTag(tagName.m_data + tagName.m_size, m_data - 1 - (tagName.m_data + tagName.m_size)));
                    break;
                }
                else if (tp == e_ttREnd)
                {
                    pCallback->EndPropertys(tagName, ToTag(tagName.m_data + tagName.m_size, m_data - 2 - (tagName.m_data + tagName.m_size)));
                    pCallback->EndNode(tagName);
                    break;
                }
                else if (tp == e_ttEof)
                    return;
            }
        }
    }
};

TM_END_NAMESPACE

void XmlParse(TM::CXmlTag xml, IXmlParser* pCallback)
{
    TM::CXmlTokenT<TMCharT>(xml).Parser(pCallback);
    pCallback->EndDoc();
}

template <class T>
struct CXmlPreT
{
    TMCStr str; TMUChar size; T val; 

    tm_constexpr11 bool operator < (const CXmlPreT& v) const { return TM::CStrView(str, size) < TM::CStrView(v.str, v.size); }
    tm_constexpr11 bool operator < (const TM::CStrView& v) const { return TM::CStrView(str, size) < v; }
    tm_constexpr20 friend bool operator < (const TM::CStrView& v, const CXmlPreT& r) { return v < TM::CStrView(r.str, r.size); }
    tm_constexpr11 T get_data() const { return val; }
};


template <class T> struct XMLCHAR_T
{
    T val[4];
    T size;
    void CopyTo(XMLCHAR_T& dest) const { dest = *this; };
};

struct XMLCHAR_W
{
    TMCharW val;
    enum { size = 1 };
    void CopyTo(XMLCHAR_T<TMCharW>& dest) const { dest.size = 1; dest.val[0] = val; };
};

#ifdef TM_UNICODE
#   define XMLPRE_INIT(x,u16,u8) {TM_T(#x), tm_countof(TM_T(#x)) - 1, {u16} }
    typedef CXmlPreT<XMLCHAR_W>                     CXmlPreText;     
    typedef TM::CStaticSetRecorderT<CXmlPreText>    CPreTextRec;
#else
#   define XMLPRE_INIT(x,u16,u8) {TM_T(#x), tm_countof(TM_T(#x)) - 1, {u8, tm_countof(u8) - 1} }
    typedef CXmlPreT<XMLCHAR_T<TMCharA> >           CXmlPreText;     
    typedef TM::CStaticSetRecorderT<CXmlPreText>    CPreTextRec;
#endif

///////////////////////////////////////////////////////////////////////////////
// pro defined text table
// https://dev.w3.org/html5/html-author/charref
static const CXmlPreText cs_aPreText[] =
{
    XMLPRE_INIT(AMP,        0x0026,     "\x26"), // & = amp
    XMLPRE_INIT(APOS,       0x0027,     "\x27"), // ' = apos
    XMLPRE_INIT(Aacute,     0x00C1,     "\xC3\x81"), // ��
    XMLPRE_INIT(Acirc,      0x00C2,     "\xC3\x82"),
    XMLPRE_INIT(Agrave,     0x00C0,     "\xC3\x80"),
    XMLPRE_INIT(Atilde,     0x00C3,     "\xC3\x83"),
    XMLPRE_INIT(Auml,       0x00C4,     "\xC3\x84"),
    XMLPRE_INIT(Beta,       0x0392,     "\xCE\x92"),
    XMLPRE_INIT(Ccedil,     0x00C7,     "\xC3\x87"),
    XMLPRE_INIT(Chi,        0x03A7,     "\xCE\xA7"),
    XMLPRE_INIT(Dagger,     0x2021,     "\xE2\x80\xA1"),
    XMLPRE_INIT(Delta,      0x0394,     "\xCE\x94"),
    XMLPRE_INIT(Eacute,     0x00C9,     "\xC3\x89"), // ��
    XMLPRE_INIT(Ecirc,      0x00CA,     "\xC3\x8A"),
    XMLPRE_INIT(Egrave,     0x00C8,     "\xC3\x88"),
    XMLPRE_INIT(Epsilon,    0x0395,     "\xCE\x95"),
    XMLPRE_INIT(Eta,        0x0397,     "\xCE\x97"),
    XMLPRE_INIT(Euml,       0x00CB,     "\xC3\x8B"),
    XMLPRE_INIT(GT,         0x003E,     "\x3E"), // > = gt
    XMLPRE_INIT(Gamma,      0x0393,     "\xCE\x93"),
    XMLPRE_INIT(Iacute,     0x00CD,     "\xC3\x8D"), // ��
    XMLPRE_INIT(Icirc,      0x00CE,     "\xC3\x8E"),
    XMLPRE_INIT(Igrave,     0x00CC,     "\xC3\x8C"),
    XMLPRE_INIT(Iota,       0x0399,     "\xCE\x99"),
    XMLPRE_INIT(Iuml,       0x00CF,     "\xC3\x8F"),
    XMLPRE_INIT(Kappa,      0x039A,     "\xCE\x9A"),
    XMLPRE_INIT(LT,         0x003C,     "\x3C"), // < = lt
    XMLPRE_INIT(Lambda,     0x039B,     "\xCE\x9B"),
    XMLPRE_INIT(Mu,         0x039C,     "\xCE\x9C"),
    XMLPRE_INIT(Ntilde,     0x00D1,     "\xC3\x91"),
    XMLPRE_INIT(Nu,         0x039D,     "\xCE\x9D"),
    XMLPRE_INIT(Oacute,     0x00D3,     "\xC3\x93"), // ��
    XMLPRE_INIT(Ocirc,      0x00D4,     "\xC3\x94"),
    XMLPRE_INIT(Ograve,     0x00D2,     "\xC3\x92"),
    XMLPRE_INIT(Omega,      0x03A9,     "\xCE\xA9"),
    XMLPRE_INIT(Omicron,    0x039F,     "\xCE\x9F"),
    XMLPRE_INIT(Otilde,     0x00D5,     "\xC3\x95"),
    XMLPRE_INIT(Ouml,       0x00D6,     "\xC3\x96"),
    XMLPRE_INIT(Phi,        0x03A6,     "\xCE\xA6"),
    XMLPRE_INIT(Pi,         0x03A0,     "\xCE\xA0"),
    XMLPRE_INIT(Prime,      0x2033,     "\xE2\x80\xB3"), // ��
    XMLPRE_INIT(Psi,        0x03A8,     "\xCE\xA8"),
    XMLPRE_INIT(QUOT,       0x0022,     "\x22"), // " = quot
    XMLPRE_INIT(Rho,        0x03A1,     "\xCE\xA1"),
    XMLPRE_INIT(Sigma,      0x03A3,     "\xCE\xA3"),
    XMLPRE_INIT(Tau,        0x03A4,     "\xCE\xA4"),
    XMLPRE_INIT(Theta,      0x0398,     "\xCE\x98"),
    XMLPRE_INIT(Uacute,     0x00DA,     "\xC3\x9A"), // ��
    XMLPRE_INIT(Ucirc,      0x00DB,     "\xC3\x9B"),
    XMLPRE_INIT(Ugrave,     0x00D9,     "\xC3\x99"),
    XMLPRE_INIT(Upsilon,    0x03A5,     "\xCE\xA5"),
    XMLPRE_INIT(Uuml,       0x00DC,     "\xC3\x9C"),
    XMLPRE_INIT(Xi,         0x039E,     "\xCE\x9E"),
    XMLPRE_INIT(Yacute,     0x00DD,     "\xC3\x9D"),
    XMLPRE_INIT(Zeta,       0x0396,     "\xCE\x96"),
    
    XMLPRE_INIT(aacute,     0x00E1,     "\xC3\xA1"), // ��
    XMLPRE_INIT(acirc,      0x00E2,     "\xC3\xA2"),
    XMLPRE_INIT(acute,      0x00B4,     "\xC2\xB4"), // �� (Acute)
    XMLPRE_INIT(agrave,     0x00E0,     "\xC3\xA0"),
    XMLPRE_INIT(amp,        0x0026,     "\x26"), // &
    XMLPRE_INIT(apos,       0x0027,     "\x27"), // '
    XMLPRE_INIT(atilde,     0x00E3,     "\xC3\xA3"),
    XMLPRE_INIT(auml,       0x00E4,     "\xC3\xA4"),
    XMLPRE_INIT(beta,       0x03B2,     "\xCE\xB2"),
    XMLPRE_INIT(brvbar,     0x00A6,     "\xC2\xA6"), // |
    XMLPRE_INIT(bull,       0x2022,     "\xE2\x80\xA2"), // Bullet/Beep
    XMLPRE_INIT(ccedil,     0x00E7,     "\xC3\xA7"),
    XMLPRE_INIT(cedil,      0x00B8,     "\xC2\xB8"), // Cedilla
    XMLPRE_INIT(cent,       0x00A2,     "\xC2\xA2"), // cent: ��
    XMLPRE_INIT(chi,        0x03C7,     "\xCF\x87"),
    XMLPRE_INIT(circ,       0x005E,     "\x5E"), // ^ ? (Circumflex)
    XMLPRE_INIT(copy,       0x00A9,     "\xC2\xA9"), // copy right
    XMLPRE_INIT(curren,     0x00A4,     "\xC2\xA4"), // Gen currency: ��
    XMLPRE_INIT(dagger,     0x2020,     "\xE2\x80\xA0"),
    XMLPRE_INIT(darr,       0x2193,     "\xE2\x86\x93"), // ��
    XMLPRE_INIT(deg,        0x00B0,     "\xC2\xB0"), // ��
    XMLPRE_INIT(delta,      0x03B4,     "\xCE\xB4"),
    XMLPRE_INIT(eacute,     0x00E9,     "\xC3\xA9"), // ��
    XMLPRE_INIT(ecirc,      0x00EA,     "\xC3\xAA"),
    XMLPRE_INIT(egrave,     0x00E8,     "\xC3\xA8"),
    XMLPRE_INIT(epsilon,    0x03B5,     "\xCE\xB5"),
    XMLPRE_INIT(equiv,      0x003D,     "\x3D"), // =
    XMLPRE_INIT(eta,        0x03B7,     "\xCE\xB7"),
    XMLPRE_INIT(euml,       0x00EB,     "\xC3\xAB"),
    XMLPRE_INIT(euro,       0x20AC,     "\xE2\x82\xAC"), // Euro/EU
    XMLPRE_INIT(frac12,     0x00BD,     "\xC2\xBD"),
    XMLPRE_INIT(frac14,     0x00BC,     "\xC2\xBC"),
    XMLPRE_INIT(frac34,     0x00BE,     "\xC2\xBE"),
    XMLPRE_INIT(gamma,      0x03B3,     "\xCE\xB3"),
    XMLPRE_INIT(ge,         0x2265,     "\xE2\x89\xA5"), // ��
    XMLPRE_INIT(gt,         0x003E,     "\x3E"), // >
    XMLPRE_INIT(hellip,     0x2026,     "\xE2\x80\xA6"), // ��
    XMLPRE_INIT(iacute,     0x00ED,     "\xC3\xAD"), // ��
    XMLPRE_INIT(icirc,      0x00EE,     "\xC3\xAE"),
    XMLPRE_INIT(iexcl,      0x00A1,     "\xC2\xA1"), // Inverted Exclamation
    XMLPRE_INIT(igrave,     0x00EC,     "\xC3\xAC"),
    XMLPRE_INIT(iota,       0x03B9,     "\xCE\xB9"),
    XMLPRE_INIT(iquest,     0x00BF,     "\xC2\xBF"), // Inverted question mark
    XMLPRE_INIT(iuml,       0x00EF,     "\xC3\xAF"),
    XMLPRE_INIT(kappa,      0x03F0,     "\xCF\xB0"),
    XMLPRE_INIT(lambda,     0x03BB,     "\xCE\xBB"),
    XMLPRE_INIT(laquo,      0x00AB,     "\xC2\xAB"),
    XMLPRE_INIT(larr,       0x2190,     "\xE2\x86\x90"), // ��
    XMLPRE_INIT(ldquo,      0x201C,     "\xE2\x80\x9C"), // ��
    XMLPRE_INIT(le,         0x2264,     "\xE2\x89\xA4"), // ��
    XMLPRE_INIT(lsaquo,     0x3008,     "\xE3\x80\x88"),
    XMLPRE_INIT(lsquo,      0x2018,     "\xE2\x80\x98"), // Left quote: ��
    XMLPRE_INIT(lt,         0x003C,     "\x3C"), // <
    XMLPRE_INIT(macr,       0x00AF,     "\xC2\xAF"), // Macron accent
    XMLPRE_INIT(mdash,      0x2014,     "\xE2\x80\x94"), // mdash: ��
    XMLPRE_INIT(micro,      0x00B5,     "\xC2\xB5"), // Micro: ��
    XMLPRE_INIT(middot,     0x00B7,     "\xC2\xB7"), // middle dot: ��
    XMLPRE_INIT(mu,         0x03BC,     "\xCE\xBC"),
    XMLPRE_INIT(nbsp,       0x00A0,     "\xC2\xA0"), // non breaking Space
    XMLPRE_INIT(ndash,      0x2013,     "\xE2\x80\x93"), // ndash: �C
    XMLPRE_INIT(not,        0x00AC,     "\xC2\xAC"), // NOT sign
    XMLPRE_INIT(ntilde,     0x00F1,     "\xC3\xB1"),
    XMLPRE_INIT(nu,         0x03BD,     "\xCE\xBD"),
    XMLPRE_INIT(oacute,     0x00F3,     "\xC3\xB3"), // ��
    XMLPRE_INIT(ocirc,      0x00F4,     "\xC3\xB4"),
    XMLPRE_INIT(ograve,     0x00F2,     "\xC3\xB2"),
    XMLPRE_INIT(omega,      0x03C9,     "\xCF\x89"),
    XMLPRE_INIT(omicron,    0x03BF,     "\xCE\xBF"),
    XMLPRE_INIT(ordf,       0x00AA,     "\xC2\xAA"), // feminine ordinal
    XMLPRE_INIT(ordm,       0x00BA,     "\xC2\xBA"), // Masculine Ordinal
    XMLPRE_INIT(otilde,     0x00F5,     "\xC3\xB5"),
    XMLPRE_INIT(ouml,       0x00F6,     "\xC3\xB6"),
    XMLPRE_INIT(para,       0x00B6,     "\xC2\xB6"), // Paragraph
    XMLPRE_INIT(phi,        0x03C6,     "\xCF\x86"),
    XMLPRE_INIT(pi,         0x03C0,     "\xCF\x80"),
    XMLPRE_INIT(plusmn,     0x00B1,     "\xC2\xB1"), // ��
    XMLPRE_INIT(pound,      0x00A3,     "\xC2\xA3"), // Pound Sterling: ��
    XMLPRE_INIT(prime,      0x2032,     "\xE2\x80\xB2"), // Prime: ��
    XMLPRE_INIT(psi,        0x03C8,     "\xCF\x88"),
    XMLPRE_INIT(quot,       0x0022,     "\x22"), // "
    XMLPRE_INIT(raquo,      0x00BB,     "\xC2\xBB"),
    XMLPRE_INIT(rarr,       0x2192,     "\xE2\x86\x92"), // ��
    XMLPRE_INIT(rdquo,      0x201D,     "\xE2\x80\x9D"), // ��
    XMLPRE_INIT(reg,        0x00AE,     "\xC2\xAE"), // R
    XMLPRE_INIT(rho,        0x03C1,     "\xCF\x81"),
    XMLPRE_INIT(rsaquo,     0x3009,     "\xE3\x80\x89"),
    XMLPRE_INIT(rsquo,      0x2019,     "\xE2\x80\x99"), // Apostrophe: ��
    XMLPRE_INIT(sect,       0x00A7,     "\xC2\xA7"), // �� (Section)
    XMLPRE_INIT(shy,        0x00AD,     "\xC2\xAD"), // soft hyphen (SHY)
    XMLPRE_INIT(sigma,      0x03C3,     "\xCF\x83"),
    XMLPRE_INIT(sim,        0x007E,     "\x7E"),
    XMLPRE_INIT(sup1,       0x00B9,     "\xC2\xB9"),
    XMLPRE_INIT(sup2,       0x00B2,     "\xC2\xB2"),
    XMLPRE_INIT(sup3,       0x00B3,     "\xC2\xB3"),
    XMLPRE_INIT(szlig,      0x00DF,     "\xC3\x9F"), // Eszett
    XMLPRE_INIT(tau,        0x03C4,     "\xCF\x84"),
    XMLPRE_INIT(theta,      0x03B8,     "\xCE\xB8"),
    XMLPRE_INIT(tilde,      0x02DC,     "\xCB\x9C"), // ? �� (Tilde)
    XMLPRE_INIT(trade,      0x2122,     "\xE2\x84\xA2"), // TM
    XMLPRE_INIT(uacute,     0x00FA,     "\xC3\xBA"), // ��
    XMLPRE_INIT(uarr,       0x2191,     "\xE2\x86\x91"), // ��
    XMLPRE_INIT(ucirc,      0x00FB,     "\xC3\xBB"),
    XMLPRE_INIT(ugrave,     0x00F9,     "\xC3\xB9"),
    XMLPRE_INIT(uml,        0x00A8,     "\xC2\xA8"), // �� (Umlaut/Dieresis)
    XMLPRE_INIT(upsilon,    0x03C5,     "\xCF\x85"),
    XMLPRE_INIT(uuml,       0x00FC,     "\xC3\xBC"),
    XMLPRE_INIT(xi,         0x03BE,     "\xCE\xBE"),
    XMLPRE_INIT(yacute,     0x00FD,     "\xC3\xBD"), //
    XMLPRE_INIT(yen,        0x00A5,     "\xC2\xA5"), // Yen: ��
    XMLPRE_INIT(zeta,       0x03B6,     "\xCE\xB6"),
};
TM_STATIC_SET_CHECK(cs_aPreText);

//#define INSERT(x,y) this->insert(TM_CL(#x), y);

#undef XMLPRE_INIT
#define XMLPRE_INIT(x,y) {TM_T(#x), tm_countof(TM_T(#x)) - 1, y }

static const CXmlPreT<TMColor> cs_aPreColor[] =
{
    XMLPRE_INIT(aliceblue,              TMRGB(240, 248, 255)),
    XMLPRE_INIT(antiquewhite,           TMRGB(250, 235, 215)),
    XMLPRE_INIT(aqua,                   TMRGB(  0, 255, 255)),
    XMLPRE_INIT(aquamarine,             TMRGB(127, 255, 212)),
    XMLPRE_INIT(azure,                  TMRGB(240, 255, 255)),
    XMLPRE_INIT(beige,                  TMRGB(245, 245, 220)),
    XMLPRE_INIT(bisque,                 TMRGB(255, 228, 196)),
    XMLPRE_INIT(black,                  TMRGB(  0,   0,   0)),
    XMLPRE_INIT(blanchedalmond,         TMRGB(255, 235, 205)),
    XMLPRE_INIT(blue,                   TMRGB(  0,   0, 255)),
    XMLPRE_INIT(blueviolet,             TMRGB(138,  43, 226)),
    XMLPRE_INIT(brown,                  TMRGB(165,  42,  42)),
    XMLPRE_INIT(burlywood,              TMRGB(222, 184, 135)),
    XMLPRE_INIT(cadetblue,              TMRGB( 95, 158, 160)),
    XMLPRE_INIT(chartreuse,             TMRGB(127, 255,   0)),
    XMLPRE_INIT(chocolate,              TMRGB(210, 105,  30)),
    XMLPRE_INIT(coral,                  TMRGB(255, 127,  80)),
    XMLPRE_INIT(cornflowerblue,         TMRGB(100, 149, 237)),
    XMLPRE_INIT(cornsilk,               TMRGB(255, 248, 220)),
    XMLPRE_INIT(crimson,                TMRGB(220,  20,  60)),
    XMLPRE_INIT(cyan,                   TMRGB(  0, 255, 255)),
    XMLPRE_INIT(darkblue,               TMRGB(  0,   0, 139)),
    XMLPRE_INIT(darkcyan,               TMRGB(  0, 139, 139)),
    XMLPRE_INIT(darkgoldenrod,          TMRGB(184, 134,  11)),
    XMLPRE_INIT(darkgray,               TMRGB(169, 169, 169)),
    XMLPRE_INIT(darkgreen,              TMRGB(  0, 100,   0)),
    XMLPRE_INIT(darkgrey,               TMRGB(169, 169, 169)),
    XMLPRE_INIT(darkkhaki,              TMRGB(189, 183, 107)),
    XMLPRE_INIT(darkmagenta,            TMRGB(139,   0, 139)),
    XMLPRE_INIT(darkolivegreen,         TMRGB( 85, 107,  47)),
    XMLPRE_INIT(darkorange,             TMRGB(255, 140,   0)),
    XMLPRE_INIT(darkorchid,             TMRGB(153,  50, 204)),
    XMLPRE_INIT(darkred,                TMRGB(139,   0,   0)),
    XMLPRE_INIT(darksalmon,             TMRGB(233, 150, 122)),
    XMLPRE_INIT(darkseagreen,           TMRGB(143, 188, 143)),
    XMLPRE_INIT(darkslateblue,          TMRGB( 72,  61, 139)),
    XMLPRE_INIT(darkslategray,          TMRGB( 47,  79,  79)),
    XMLPRE_INIT(darkslategrey,          TMRGB( 47,  79,  79)),
    XMLPRE_INIT(darkturquoise,          TMRGB(  0, 206, 209)),
    XMLPRE_INIT(darkviolet,             TMRGB(148,   0, 211)),
    XMLPRE_INIT(deeppink,               TMRGB(255,  20, 147)),
    XMLPRE_INIT(deepskyblue,            TMRGB(  0, 191, 255)),
    XMLPRE_INIT(dimgray,                TMRGB(105, 105, 105)),
    XMLPRE_INIT(dimgrey,                TMRGB(105, 105, 105)),
    XMLPRE_INIT(dodgerblue,             TMRGB( 30, 144, 255)),
    XMLPRE_INIT(firebrick,              TMRGB(178,  34,  34)),
    XMLPRE_INIT(floralwhite,            TMRGB(255, 250, 240)),
    XMLPRE_INIT(forestgreen,            TMRGB( 34, 139,  34)),
    XMLPRE_INIT(fuchsia,                TMRGB(255,   0, 255)),
    XMLPRE_INIT(gainsboro,              TMRGB(220, 220, 220)),
    XMLPRE_INIT(ghostwhite,             TMRGB(248, 248, 255)),
    XMLPRE_INIT(gold,                   TMRGB(255, 215,   0)),
    XMLPRE_INIT(goldenrod,              TMRGB(218, 165,  32)),
    XMLPRE_INIT(gray,                   TMRGB(128, 128, 128)),
    XMLPRE_INIT(green,                  TMRGB(  0, 128,   0)),
    XMLPRE_INIT(greenyellow,            TMRGB(173, 255,  47)),
    XMLPRE_INIT(grey,                   TMRGB(128, 128, 128)),
    XMLPRE_INIT(honeydew,               TMRGB(240, 255, 240)),
    XMLPRE_INIT(hotpink,                TMRGB(255, 105, 180)),
    XMLPRE_INIT(indianred,              TMRGB(205,  92,  92)),
    XMLPRE_INIT(indigo,                 TMRGB( 75,   0, 130)),
    XMLPRE_INIT(ivory,                  TMRGB(255, 255, 240)),
    XMLPRE_INIT(khaki,                  TMRGB(240, 230, 140)),
    XMLPRE_INIT(lavender,               TMRGB(230, 230, 250)),
    XMLPRE_INIT(lavenderblush,          TMRGB(255, 240, 245)),
    XMLPRE_INIT(lawngreen,              TMRGB(124, 252,   0)),
    XMLPRE_INIT(lemonchiffon,           TMRGB(255, 250, 205)),
    XMLPRE_INIT(lightblue,              TMRGB(173, 216, 230)),
    XMLPRE_INIT(lightcoral,             TMRGB(240, 128, 128)),
    XMLPRE_INIT(lightcyan,              TMRGB(224, 255, 255)),
    XMLPRE_INIT(lightgoldenrodyellow,   TMRGB(250, 250, 210)),
    XMLPRE_INIT(lightgray,              TMRGB(211, 211, 211)),
    XMLPRE_INIT(lightgreen,             TMRGB(144, 238, 144)),
    XMLPRE_INIT(lightgrey,              TMRGB(211, 211, 211)),
    XMLPRE_INIT(lightpink,              TMRGB(255, 182, 193)),
    XMLPRE_INIT(lightsalmon,            TMRGB(255, 160, 122)),
    XMLPRE_INIT(lightseagreen,          TMRGB( 32, 178, 170)),
    XMLPRE_INIT(lightskyblue,           TMRGB(135, 206, 250)),
    XMLPRE_INIT(lightslategray,         TMRGB(119, 136, 153)),
    XMLPRE_INIT(lightslategrey,         TMRGB(119, 136, 153)),
    XMLPRE_INIT(lightsteelblue,         TMRGB(176, 196, 222)),
    XMLPRE_INIT(lightyellow,            TMRGB(255, 255, 224)),
    XMLPRE_INIT(lime,                   TMRGB(  0, 255,   0)),
    XMLPRE_INIT(limegreen,              TMRGB( 50, 205,  50)),
    XMLPRE_INIT(linen,                  TMRGB(250, 240, 230)),
    XMLPRE_INIT(magenta,                TMRGB(255,   0, 255)),
    XMLPRE_INIT(maroon,                 TMRGB(128,   0,   0)),
    XMLPRE_INIT(mediumaquamarine,       TMRGB(102, 205, 170)),
    XMLPRE_INIT(mediumblue,             TMRGB(  0,   0, 205)),
    XMLPRE_INIT(mediumorchid,           TMRGB(186,  85, 211)),
    XMLPRE_INIT(mediumpurple,           TMRGB(147, 112, 219)),
    XMLPRE_INIT(mediumseagreen,         TMRGB( 60, 179, 113)),
    XMLPRE_INIT(mediumslateblue,        TMRGB(123, 104, 238)),
    XMLPRE_INIT(mediumspringgreen,      TMRGB(  0, 250, 154)),
    XMLPRE_INIT(mediumturquoise,        TMRGB( 72, 209, 204)),
    XMLPRE_INIT(mediumvioletred,        TMRGB(199,  21, 133)),
    XMLPRE_INIT(midnightblue,           TMRGB( 25,  25, 112)),
    XMLPRE_INIT(mintcream,              TMRGB(245, 255, 250)),
    XMLPRE_INIT(mistyrose,              TMRGB(255, 228, 225)),
    XMLPRE_INIT(moccasin,               TMRGB(255, 228, 181)),
    XMLPRE_INIT(navajowhite,            TMRGB(255, 222, 173)),
    XMLPRE_INIT(navy,                   TMRGB(  0,   0, 128)),
    XMLPRE_INIT(oldlace,                TMRGB(253, 245, 230)),
    XMLPRE_INIT(olive,                  TMRGB(128, 128,   0)),
    XMLPRE_INIT(olivedrab,              TMRGB(107, 142,  35)),
    XMLPRE_INIT(orange,                 TMRGB(255, 165,   0)),
    XMLPRE_INIT(orangered,              TMRGB(255,  69,   0)),
    XMLPRE_INIT(orchid,                 TMRGB(218, 112, 214)),
    XMLPRE_INIT(palegoldenrod,          TMRGB(238, 232, 170)),
    XMLPRE_INIT(palegreen,              TMRGB(152, 251, 152)),
    XMLPRE_INIT(paleturquoise,          TMRGB(175, 238, 238)),
    XMLPRE_INIT(palevioletred,          TMRGB(219, 112, 147)),
    XMLPRE_INIT(papayawhip,             TMRGB(255, 239, 213)),
    XMLPRE_INIT(peachpuff,              TMRGB(255, 218, 185)),
    XMLPRE_INIT(peru,                   TMRGB(205, 133,  63)),
    XMLPRE_INIT(pink,                   TMRGB(255, 192, 203)),
    XMLPRE_INIT(plum,                   TMRGB(221, 160, 221)),
    XMLPRE_INIT(powderblue,             TMRGB(176, 224, 230)),
    XMLPRE_INIT(purple,                 TMRGB(128,   0, 128)),
    XMLPRE_INIT(red,                    TMRGB(255,   0,   0)),
    XMLPRE_INIT(rosybrown,              TMRGB(188, 143, 143)),
    XMLPRE_INIT(royalblue,              TMRGB( 65, 105, 225)),
    XMLPRE_INIT(saddlebrown,            TMRGB(139,  69,  19)),
    XMLPRE_INIT(salmon,                 TMRGB(250, 128, 114)),
    XMLPRE_INIT(sandybrown,             TMRGB(244, 164,  96)),
    XMLPRE_INIT(seagreen,               TMRGB( 46, 139,  87)),
    XMLPRE_INIT(seashell,               TMRGB(255, 245, 238)),
    XMLPRE_INIT(sienna,                 TMRGB(160,  82,  45)),
    XMLPRE_INIT(silver,                 TMRGB(192, 192, 192)),
    XMLPRE_INIT(skyblue,                TMRGB(135, 206, 235)),
    XMLPRE_INIT(slateblue,              TMRGB(106,  90, 205)),
    XMLPRE_INIT(slategray,              TMRGB(112, 128, 144)),
    XMLPRE_INIT(slategrey,              TMRGB(112, 128, 144)),
    XMLPRE_INIT(snow,                   TMRGB(255, 250, 250)),
    XMLPRE_INIT(springgreen,            TMRGB(  0, 255, 127)),
    XMLPRE_INIT(steelblue,              TMRGB( 70, 130, 180)),
    XMLPRE_INIT(tan,                    TMRGB(210, 180, 140)),
    XMLPRE_INIT(teal,                   TMRGB(  0, 128, 128)),
    XMLPRE_INIT(thistle,                TMRGB(216, 191, 216)),
    XMLPRE_INIT(tomato,                 TMRGB(255,  99,  71)),
    XMLPRE_INIT(turquoise,              TMRGB( 64, 224, 208)),
    XMLPRE_INIT(violet,                 TMRGB(238, 130, 238)),
    XMLPRE_INIT(wheat,                  TMRGB(245, 222, 179)),
    XMLPRE_INIT(white,                  TMRGB(255, 255, 255)),
    XMLPRE_INIT(whitesmoke,             TMRGB(245, 245, 245)),
    XMLPRE_INIT(yellow,                 TMRGB(255, 255,   0)),
    XMLPRE_INIT(yellowgreen,            TMRGB(154, 205,  50)),
};
typedef TM::CStaticSetRecorderT<CXmlPreT<TMColor> > CPreColorRec;
TM_STATIC_SET_CHECK(cs_aPreColor);


static const CXmlPreT<EColorIndex> cs_aPreSysColor[] =
{
    XMLPRE_INIT(accentcolor,            eColorWebAccentColor),
    XMLPRE_INIT(accentcolortext,        eColorWebAccentColorText),
    XMLPRE_INIT(activetext,             eColorWebActiveText),
    XMLPRE_INIT(buttonborder,           eColorWebButtonBorder),
    XMLPRE_INIT(buttonface,             eColorWebButtonFace),
    XMLPRE_INIT(buttontext,             eColorWebButtonText),
    XMLPRE_INIT(canvas,                 eColorWebCanvas),
    XMLPRE_INIT(canvastext,             eColorWebCanvasText),
    XMLPRE_INIT(field,                  eColorWebField),
    XMLPRE_INIT(fieldtext,              eColorWebFieldText),
    XMLPRE_INIT(graytext,               eColorWebGrayText),
    XMLPRE_INIT(highlight,              eColorWebHighlight),
    XMLPRE_INIT(highlighttext,          eColorWebHighlightText),
    XMLPRE_INIT(linktext,               eColorWebLinkText),
    XMLPRE_INIT(mark,                   eColorWebMark),
    XMLPRE_INIT(marktext,               eColorWebMarkText),
    XMLPRE_INIT(selecteditem,           eColorWebSelectedItem),
    XMLPRE_INIT(selecteditemtext,       eColorWebSelectedItemText),
    XMLPRE_INIT(visitedtext,            eColorWebVisitedText),
};
typedef TM::CStaticSetRecorderT<CXmlPreT<EColorIndex> > CPreSysColorRec; 
TM_STATIC_SET_CHECK(cs_aPreSysColor);


// #   undef INSERT
#   undef XMLPRE_INIT



// decode:
// 1. left trim;
// 2. if can right trim then right trim, and set flag "ext space"
// 3. for each
//    1. if find \r\n then erase
//    2. merge space
//    3. decode HTML symbol
// 4. if sizeof string == 0 then return;
// 5. if have ext space then append space;
// 6. return.

// tmio.cpp
int __u32to8 (TMStr8 pd, size_t nBuffer, TMUInt32 u32);
int __u32to16(TMStrW pd, TMUInt32 u32);

static int __xml_u32_to_ut(TMCStrW str, int nBase, TMCharW* p)
{
    TMUInt32 u32 = StrScanLong(str, nBase);
    return __u32to16(p, u32);
}

static int __xml_u32_to_ut(TMCStrA str, int nBase, TMCharA* p)
{
    TMUInt32 u32 = StrScanLong(str, nBase);
    return __u32to8((TMStr8)p, 4, u32);
}

void XmlTextDecode(TM::CWeakStr& str, TMBool bProperty)
{
    TM::CAutoAnalysisTuiMsg _ama("XmlTextDecode");

//     str.TrimLeft();
    int nLen = str.GetLength();
    if (!bProperty)
    {
        str = str.TrimRight();
        if (str.IsEmpty())
        {
            if (nLen)
                str = TM_CT(" ");
            return;
        }
        if (str.GetLength() != nLen)
            str.append(TM_CT(" "));
    }

    int n1 = -1; // the '&' position
    TMBool bPrevSpace = false;
    for (int i = 0; i < nLen;)
    {
        TMCharT c = str[i];
        if (!c)
            break;
        if (!bProperty)
        {
            if (c == '\r' || c == '\n' || (StrIsSpace(str[i]) && StrIsSpace(str[i + 1])))
            {
                if (bPrevSpace)
                    str.erase(i, 1);
                else
                    str[i++] = ' ';
                bPrevSpace = true;
                continue;
            }
            else if (c == '\t')
            {
                bPrevSpace = true;
                str[i] = ' ';
                str.insert(i, 3, ' ');
                i += 3;
                continue;
            }
        }
        
        bPrevSpace = false;
        if (c == '&')
            n1 = i;
        else if (c == ';' && n1 >= 0)
        {
            XMLCHAR_T<TMCharT> cReplace;
            if (str[n1 + 1] == '#')
            {
                if (str[n1 + 2] == '0')
                    cReplace.size = 1, cReplace.val[0] = 0;
                else if (str[n1 + 2] == 'x' || str[n1 + 2] == 'X')
                    cReplace.size = __xml_u32_to_ut(str.data() + n1 + 3, 16, cReplace.val);
                else
                    cReplace.size = __xml_u32_to_ut(str.data() + n1 + 2, 10, cReplace.val);
            }
            else
            {
                CPreTextRec rec(cs_aPreText);
                rec.find(str.substr(n1 + 1, i - n1 - 1));
                if (rec)
                    rec->val.CopyTo(cReplace);
                else
                    cReplace.size = 0;
            }

            if (cReplace.size)
            {
                str.replace(n1, i + 1 - n1, cReplace.val, cReplace.size);
                i = n1 + 1;
//                 DbgAssert(str[i - 1] == cReplace);
            }
            else
                ++i;
            n1 = -1;
            continue;
        }
        ++i;
    }
}


void XmlTextEncode(TM::CWeakStr& str, TMBool bProperty)
{
    TM::CAutoAnalysisTuiMsg _ama("XmlTextEncode");

#define XML_TEXT_REPLACE(n,s) \
    do { \
        str.replace(i, n, s, tm_countof(s) - 1); \
        i += tm_countof(s) - 1; \
        DbgAssert(str[i - 1] == s[tm_countof(s) - 2]); \
    } while(0)

    TMBool bFirstSpace = true;
    for (int i = 0; i < str.GetLength();)
    {
        TMCharT c = str[i];
        if (!bProperty)
        {
            if (c == ' ')
            {
                if (bFirstSpace)
                {
                    bFirstSpace = false;
                    ++i;
                }
                else
                    XML_TEXT_REPLACE(1, TM_T("&nbsp;"));
                continue;
            }
            if (c == 0xa0) // NBSP
            {
                bFirstSpace = false;
                XML_TEXT_REPLACE(1, TM_T("&nbsp;"));
                continue;
            }
            if ((c == '\r' && str[i + 1] == '\n') || (c == '\n' && str[i + 1] == '\r'))
            {
                XML_TEXT_REPLACE(2, TM_T("<br/>"));
                bFirstSpace = true;
                continue;
            }
            else if (c == '\r' || c == '\n')
            {
                XML_TEXT_REPLACE(1, TM_T("<br/>"));
                bFirstSpace = true;
                continue;
            }
        }
        else
        {
            if (c == '\r')
            {
                XML_TEXT_REPLACE(1, TM_T("&#13;"));
                continue;
            }
            else if (c == '\n')
            {
                XML_TEXT_REPLACE(1, TM_T("&#10;"));
                continue;
            }
        }

        switch (c)
        {
        case '\0': XML_TEXT_REPLACE(1, TM_T("&#0;"));   break;
        case '\"': XML_TEXT_REPLACE(1, TM_T("&quot;")); break;
//         case '\'': XML_TEXT_REPLACE(1, TM_T("&apos;")); break;
        case '&':  XML_TEXT_REPLACE(1, TM_T("&amp;"));  break;
        case '<':  XML_TEXT_REPLACE(1, TM_T("&lt;"));   break;
        case '>':  XML_TEXT_REPLACE(1, TM_T("&gt;"));   break;
        default: ++i;
        }

        bFirstSpace = true;
    }

#undef XML_TEXT_REPLACE
}

TMColor XmlParseColor(TM::CXmlTag str)
{
    TM::CAutoAnalysisTuiMsg _ama("XmlParseColor");

    if (str[0] == '#')
    {
        long l = 0;
        size_t pos = StrScanLong(str.data() + 1, str.length() - 1, l, 16);
        TMColor clrx;
        if (pos == 3) // 3 byte color type
        {
            TMUInt8 r = (l >> 8) & 0x0f;
            TMUInt8 g = (l >> 4) & 0x0f;
            TMUInt8 b = (l)      & 0x0f;
            clrx = TMRGB(r,g,b);
            clrx |=  (clrx<<4);
        }
        else
        {
            clrx = l;
            if (pos < 8)
                ColorSetA(clrx, 0xff); // clrx = 0xff000000 & clrx; // ColorFromCOLORREF(clrx);
        }
        return clrx;
    }
    else if (str[0] == 'r' && str[1] == 'g' && str[2] == 'b' && str[3] == '(')
    {
        TM::CStrView strClrR, strClrG, strClrB;
        int n;
        n = str.Split(4, ',', strClrR);
        n = str.Split(n, ',', strClrG);
            str.Split(n, ',', strClrB);

        if (strClrR.Find('%') > 0) // rgb(100%,0%,20%)
        {
            double r = 0, g = 0, b = 0;
            strClrR.ScanDouble(r);
            strClrG.ScanDouble(g);
            strClrB.ScanDouble(b);
            return TMRGB(r * 2.55, g * 2.55,b * 2.55);
        }
        else // rgb(100,0,20)
        {
            long r = 0, g = 0, b = 0;
            strClrR.ScanLong(r);
            strClrG.ScanLong(g);
            strClrB.ScanLong(b);
            return TMRGB(r,g,b);
        }
    }
    else
    {
        TM::CStdStr strx(str);
        strx.MakeLower();

        {
            CPreColorRec rec(cs_aPreColor);
            rec.find(strx);
            if (rec)
                return rec->val;// .get_data();
        }

        {
            CPreSysColorRec rec(cs_aPreSysColor);
            rec.find(strx);
            if (rec)
                return ColorFromSys(rec->val);
        }
        
        return TMRGB(0,0,0);
    }
}

