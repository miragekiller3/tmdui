
//-----------------------------------------------------------
//CtrlPanel.cpp

//  Copyright (C) 2004 miragekiller
//  All rights reserved      
//
//
//  Author: 
//      miragekiller    <3916345933@qq.com>
//
//  Histroy :
//      9/30/2006       create
//-------------------------------------------------------------

#include "StdAfx.h"
#include "ObjectTreeView.h"
#include "MainWnd.h"
#include "Tools.h"
#include "tmtxtsearch.h"

using namespace TM;



void CSearchResult::Insert(const SIMPLERST& sr)
{
    aResult.insert(sr);
}

struct LessS
{
    bool operator () (const CSearchResult::SIMPLERST* p1, const CSearchResult::SIMPLERST* p2) const
    {
        return *p1 < *p2;
    }
};

void CSearchResult::MakeRef()
{
    typedef CSet<const SIMPLERST*, LessS> CSRPList;
    typedef CMap<CString, int> CBodyList;
    CSRPList aSL;
    CBodyList aBody;
    tm_for (CSRList::recorder rec(aResult); rec; ++rec)
    {
        const SIMPLERST& sl = rec.get_data();
        if (aSL.insert(&sl).second)
        {
            CFnView fn(sl.filename);
            ++aBody[fn.GetBody()];
        }
    }
    nRefByFile = aSL.size();

    nRefByBody = 9999;
    tm_for (CBodyList::recorder rec(aBody); rec; ++rec)
    {
        int n = rec.get_data();
        tm_setmin(nRefByBody, n);
    }
}


static void OutputDot()
{
    static int m_nProgress = 0;
    OutputDebugString (_T("."));
    if (++m_nProgress >= 80)
    {
        m_nProgress = 0;
        OutputDebugString (_T("\r\n"));
    }
}

void CKeyWord::InsertKey(TM::CStringA str, const TM::CStringA& strFullKey)
{
    str = str.Trim();
    if (str.IsEmpty())
        return;
    TM::CStringA& fk = m_aKeys[str];
    fk += strFullKey;
}

// void CKeyWord::InsertFuncLine(const TM::CStringA& strLine, const TM::CStringA& strFullLine)
// {
//     int n = strLine.Find("::");
//     if (n >= 0)
//         InsertFuncLine(strLine.substr(n + 2), strFullLine);
//     else
//         InsertKey(strLine, strFullLine);
// }

void CKeyWord::InsertSelfLine(const TM::CStringA& strLine, const TM::CStringA& strFullLine, TMCStrA pKey, TMCStrA pKeyPre, TMCStrA pKeyEnd, TMBool bSplitKey)
{
    int n1 = strLine.Find(pKey);
    if (n1 < 0)
        return;

    if (n1 > 0 && pKeyPre)
    {
    }

    if (bSplitKey)
    {
        n1 += (StrLen(pKey) - 1);
        n1 = strLine.find_first_not_of(" \t\r\n", n1);
    }
    if (n1 >= 0) // line str maybe "class \r\n"
    {
        for (int n2 = n1; ; ++n2)
        {
            char c = strLine[n2];
            if (!c || ((c != '_') && iswctype(c, _SPACE | _PUNCT | _CONTROL | _BLANK)))
            {
                if (c && pKeyEnd && !StrChr(pKeyEnd, c))
                    break;
                TM::CStringA str2(strLine.substr(n1, n2 - n1));
                InsertKey(str2, strFullLine);
                return;
            }
        }
// 
//         int n2 = strLine.find_first_of(pKeyEnd, n1);
//         if (n2 <= 0)
//             n2 = strLine.GetLength();
//         TM::CStringA str2(strLine.substr(n1, n2 - n1));
//         InsertKey(str2, strFullLine);
//         return;
    }
//     OutputDebugStringA(strLine.c_str());
//     OutputDebugStringA("\r\n");
}

TMBool IsFuncLetter(char c)
{
    return c == '_' || iswctype(c, _DIGIT) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

void CKeyWord::InsertSelfFunction(const TM::CStringA& strLine, const TM::CStringA& strFullLine)
{
    int n1 = strLine.Find('(');
    if (n1 < 2)
        return;
    int n2 = n1;
    if (iswctype(strLine[n2 - 1], _BLANK | _SPACE)) // for "void foo ()"
        n2 = --n1;
    for (; --n2 >= 0;)
    {
        char c = strLine[n2];
        if (!IsFuncLetter(c))
        {
            if (iswctype(c, _BLANK | _SPACE) && n1 - n2 > 2)
            {
                TM::CStringA str2(strLine.substr(n2, n1 - n2));
                InsertKey(str2, strFullLine);
            }
            return;
        }
    }
    return;
}

TMBool IsSymChar(char c)
{
    return (c != '_') && c != '\"' && c != '\'' && iswctype(c, _SPACE | _PUNCT | _CONTROL | _BLANK);
}

static void PreProcessLinePreStr(CStringA& strLine, const char* pkey, int nkey)
{
    int n = strLine.Find(pkey);
    if (n <= 0)
        return;

    for (int m = n - 1; m >= 0; --m)
    {
        if (IsSymChar(strLine[m]))
        {
            strLine.erase(m, n + nkey - m);
            break;
        }
    }
}


static void PreProcessLine(CStringA& strLine)
{
    if (strLine.IsEmpty())
        return;
    char cPrev = strLine[0];
    for (int i = 1; ;)
    {
        char c = strLine[i];
        if (!c)
            break;
        if (cPrev == '\"') // "aaa" -> ""
        {
            if (c == '\"')
            {
                cPrev = 0;
                ++i;
                continue;
            }
            strLine.erase(i, 1);
            continue;
        }
        if (IsSymChar(c) && iswctype(cPrev, _BLANK | _SPACE))
        {
            strLine[i - 1] = cPrev = c;
            strLine.erase(i, 1);
            continue;
        }
        if (IsSymChar(cPrev) && iswctype(c, _BLANK | _SPACE))
        {
            strLine.erase(i, 1);
            continue;
        }
        if (c == '/' && cPrev == '/')
        {
            strLine.erase(i - 1);
            break;
        }
        cPrev = c;
        ++i;
    }

    int n = strLine.Find("__declspec");
    if (n >= 0)
    {
        int nPair = 0;
        int m = n + 1;
        for (; m < strLine.GetLength(); ++m)
        {
            char c = strLine[m];
            switch (c)
            {
            case '(': ++nPair; continue;
            case ')': --nPair; if (nPair == 0) break;
            default: continue;
            }
            break;
        }
        strLine.erase(n, m - n + 1);
    }

    PreProcessLinePreStr(strLine, "_VTABLE", 7);
    PreProcessLinePreStr(strLine, "_EXPORT", 7);
}

void CKeyWord::InsertKeyLine(const TM::CStringA& strLine, EKeyType nType)
{
    TM::CStringA strKey(strLine);
//    TM::CStringA strKey("        InsertSelfLine(strKey, strLine, \"#define \", \" \\t\", \" \\t\\r\\n(\", true);");
    PreProcessLine(strKey);
//     OutputDebugStringA(strLine.c_str());
//     OutputDebugStringA("\r\n");
//     OutputDebugStringA(strKey.c_str());
//     OutputDebugStringA("\r\n");

    switch (nType)
    {
    case eKeyTypeNormal: // normal
        InsertKey(strKey, strLine);
        break;
        
//     case eKeyTypeFunction: // function
//         InsertFuncLine(strKey, strLine);
//         break;
        
    case eKeyTypeMacro: // macro
        InsertSelfLine(strKey, strLine, "#define ", " \t", 0, true);
//         {
//             int n1 = strKey.Find ("#define ");
//             if (n1 >= 0)
//             {
//                 n1 = strKey.find_first_not_of(" \t\r\n", n1 + 7);
//                 if (n1 > 0) // line str maybe "xxx #define "
//                 {
//                     int n2 = strKey.find_first_of(" \t\r\n(", n1);
//                     if (n2 <= 0)
//                         n2 = strKey.GetLength();
//                     TM::CStringA str2(strKey.substr(n1, n2 - n1));
//                     InsertKey(str2, strLine);
//                 }
//             }
//         }
        break;

    case eKeyTypeClass:
        InsertSelfLine(strKey, strLine, "class ", " \t", ";{: \t\r\n", true);
        InsertSelfLine(strKey, strLine, "struct ", " \t", ";{: \t\r\n", true);
        InsertSelfLine(strKey, strLine, "using ", " \t", ";{: \t\r\n", true);
//         InsertSelfLine(strKey, strLine, "union ", " \t", ";{: \t\r\n", true);
//         InsertSelfLine(strKey, strLine, "enum ", " \t", ";{: \t\r\n", true);
        break;

    case eKeyTypeMember:
        InsertSelfLine(strKey, strLine, "m", " \t*", ":; \t\r\n[", false);
        break;

    case eKeyTypeFunction:
        InsertSelfFunction(strKey, strLine);
        break;
    }
}

void CKeyWord::ParseFile(const TM::CString& strTxt, EKeyType nType)
{
    TM::CStringA strLine;
    for (int nPos = 0; nPos = strTxt.SplitLine(nPos, strLine);)
        InsertKeyLine(strLine, nType);
}

void CKeyWord::SearchFile(const TM::CString& strPath, EKeyType nType)
{
    struct CSearchK : public TM::CSearchDir
    {
        CKeyWord& m_kw;
        EKeyType m_nType;
        CSearchK(CKeyWord& kw, EKeyType type) : m_kw (kw), m_nType(type) {}
        virtual void OnSearch (const TM::CFindFile& ff, int nlayer, TMBool& bStopSearch)
        {
            CString strFn(ff.GetFileName());
            if (strFn.Match(_T("*.c,*.cpp,*.h,*.inl,*.hpp,*.cxx,*.rc,*.def"), _T(','), false))
            {
                CString strTxt;
                TxtFileLoadAs(strFn, strTxt, false);
                m_kw.ParseFile(strTxt, m_nType);
                OutputDot();
            }
        }
    };

    TM::CString strP;
    CSearchK sk(*this, nType);
    for (int i = 0; i = strPath.Split(i, ';', strP);)
    {
        strP = strP.Trim();
        if (!strP.IsEmpty())
        {
            OutputDebugString(TM::CString(L"\r\nPreSeach ") + strP + _T(":\r\n"));
            int n = m_aKeys.size();
            sk.Search(strP);
            n = m_aKeys.size() - n;
            OutputDebugString(TM::CString(L"\r\nPreSeach ") + strP + _T(" result: ") + TM_L2T(n) + _T("\r\n"));
        }
    }
}



class CSearch
    : public TM::CSearchDir
    , public TM::CCppFileSeachSinkImpl
{
public:
    typedef TM::CSet<TM::CString>  CMacroSet;

    CLineMap    m_aPath;

    CKeyWord&  m_kw;
    TM::CTxtSearch  m_search;

    TM::CString m_strFn;

    
    CSearch (CKeyWord& kw)
        : m_kw(kw)
    {
        for (CKeyWord::CKeyList::recorder rec(kw.m_aKeys); rec; ++rec)
            m_search.AddKeyword(rec.get_key());
    }

    virtual void OnSearch (const TM::CFindFile& ff, int nlayer, TMBool& bStopSearch)
    {
        Init();
        m_strFn = ff.GetFileName();
        if (m_strFn.Match(_T("*.c,*.cpp,*.h,*.inl,*.hpp,*.cxx,*.rc,*.def"), _T(','), false))
        {
            TM::CString strF;
            if (TxtFileLoadAs(m_strFn, strF, false))
            {
                m_search.Search(strF, this, 0);
                OutputDot();
            }
        }
    }

    virtual void OnFind(const TM::CStringA& text, int nStart, const CStringA& key, const void* param)
    {
        int n = nStart + key.GetLength();
        char cnext = text[n];
        if (_isctype(cnext, _UPPER | _LOWER | _DIGIT) || cnext == '_')
            return;
        char cprev = text[nStart - 1];
        if (_isctype(cprev, _UPPER | _LOWER | _DIGIT) || cprev == '_')
            return;
        TMCharT c[1024];
        GetFullPathName (m_strFn, 1023, c, 0);

        CSearchResult& sr = m_aPath[key];
        sr.key = key;
        sr.keyContext = m_kw.m_aKeys[key];
        CSearchResult::SIMPLERST srs;
        srs.nLine = m_nLine;
        srs.filename = m_strFn;
        srs.filename.MakeLower();
        srs.pathname = c;
        for (; ; ++n)
        {
            cnext = text[n];
            if (!cnext || cnext == '\r' || cnext == '\n')
                break;
        }
        srs.line = text.substr(m_nLineIndex, n - m_nLineIndex);
        sr.Insert(srs);
    }

    void ToLineMap(CLineMap& aLine)
    {
        aLine.swap(m_aPath);
    }
};



void CheckKeyword(const TM::CString& strPath, CKeyWord& kw, CLineMap& mp)
{
    CSearch sh(kw);

    TM::CString strP;
    for (int i = 0; i = strPath.Split(i, ';', strP);)
    {
        strP = strP.Trim();
        if (!strP.IsEmpty())
        {
            OutputDebugString(TM::CString(L"\r\nSeach ") + strP + _T(":\r\n"));
            int n = sh.m_aPath.size();
            sh.Search (strP);
            n = sh.m_aPath.size() - n;
            OutputDebugString(TM::CString(L"\r\nSeach ") + strP + _T(" result: ") + TM_L2T(n) + _T("\r\n"));
        }
    }

    sh.ToLineMap(mp);

    for (CLineMap::recorder rec(mp); rec; ++rec)
    {
        CSearchResult& srx = rec.get_data();
        srx.MakeRef();
    }
}

