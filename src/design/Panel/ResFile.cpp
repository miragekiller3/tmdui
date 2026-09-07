#include "stdafx.h"
#include "ResFile.h"
#include "ResourceID.h"
#include "tmdebugext.h"
#include "tmfilename.h"

using namespace TM;

#define CAutoResFileAnalysis CAutoAnalysisNull
//#define CAutoResFileAnalysis CAutoAnalysisTuiUser




///////////////////////////////////////////////////////////////////////////////
// CResFile
// struct FindByID { FIND_OP(strID) };
// struct FindByBHTML { FIND_OP(bHtml) };
struct FindByBPIC { FIND_OP(.bPic) };
struct FindByBMENU { FIND_OP(.bMenu) };
struct FindByBStr { FIND_OP(.bString) };
struct FindByBPop { FIND_OP(.bPop) };


template<class T> CStringW& GetV(T& strx)
{
    if (strx[4].Find ('.') >= 0)
        return strx[4];
    else if (strx[3].Find ('.') >= 0)
        return strx[3];
    else
        return strx[2];
}

// remove "//" and " and '
void RemoveComment(TM::CStringW& str)
{
    for (int i = 0; i < str.GetLength(); ++i)
    {
        char c = str[i];
        if (c == '\"' || c == '\'')
        {
            for (++i; i < str.GetLength() && str[i] != c; ++i)
            {
            }
        }
        else if (c == '/' && str[i + 1] == '/')
        {
            str.SetLength(i);
        }
    }
}

// remove "/* */"
bool RemoveComment2(TM::CStringW& str, bool bComment)
{
    if (!bComment)
    {
        int n = str.Find(L"/*");
        if (n >= 0)
        {
            bComment = true;
            str.erase(n, -1);
        }
    }

    if (bComment)
    {
        int n = str.Find(L"*/");
        if (n >= 0)
        {
            bComment = false;
            str.erase(0, n + 2);
        }
        else
            str.clear();
    }
    return bComment;
}

void CResFile::AddFileItem(const ITEM& itm)
{
    DbgAssert(!m_pFileItem);
    m_aItem.push_back(itm);
    m_pFileItem = &m_aItem.back();
}

void CResFile::ParseResFile(CIDMgr& idMgr, TM::CStringW strFile, TMCStr pFile, TM::CString& strXmlFileName, IProjMgr* pLog)
{
    TM::CStringW str;
    TMBool bString = false;
    int nMenu = 0;
    int nLoop = 0;
    bool bComment = false;
    m_pFileItem = 0;
    for (int nStart = 0; nStart = strFile.SplitLine(nStart, str);)
    {
        ITEM itm;
//        str.freeze();
        itm.strLine = str;

        //str.Replace ("//", "\0");
        RemoveComment(str);
        bComment = RemoveComment2(str, bComment);
        str = str.Trim();
//         str.freeze();

        if (!str.IsEmpty())
        {
            CStringW strx[5];
            ParseLine2(str, strx, tm_countof(strx), 0);

            if (nLoop)
            {
                if (strx[0] == L"END")
                    --nLoop;
            }
            else if (bString)
            {
                if (strx[0] == L"END")
                    bString = false;
                else if (strx[0] == L"BEGIN")
                    ;
                else if (strx[1].IsEmpty()) // string maybe 2 line
                {
                    ITEM& itmx = m_aItem.back();
                    if (itmx.bStringID)
                    {
                        itm.strID = itmx.strID;
                        itm.strVol = strx[0];
                        itm.bString = true;
                    }
                    else if (!strx[0].IsEmpty())
                    {
                        itm.bStringID = true;
                        itm.strID = strx[0];
                    }
                }   
                else
                {
                    itm.strID = strx[0];
                    itm.strVol = strx[1];
                    itm.bString = true;
                }
            }
            else if (nMenu)
            {
                if (strx[0] == L"BEGIN")
                    ++nMenu;
                else if (strx[0] == L"END")
                {
                    if (--nMenu == 1)
                        nMenu = 0;
                }
                else if (strx[1].IsEmpty())
                    ;
                else if (strx[0] == L"POPUP")
                {
                    itm.strID = strx[1];
                    itm.strVol = strx[1];
                    itm.bPop = true;
                }
                else if (strx[0] == L"MENUITEM" && strx[1] != L"SEPARATOR")
                {
                    itm.strID = strx[2];
                    itm.strVol = strx[1];
                    itm.bMenu = true;
                }
            }
            else if (strx[0] == L"BEGIN")
                ++nLoop;
            else if (strx[1] == L"MENU")
            {
                idMgr.SetType(strx[0], EM_RC | EM_MENU);
                ++nMenu;
            }
            else if (strx[1] == L"ACCELERATORS")
            {   
                idMgr.SetType(strx[0], EM_RC | EM_ACCEL);
            }
            else if (strx[0] == L"STRINGTABLE")
                bString = true;
            else if (strx[1] == L"HTML")
            {
                //itm.bDiscardable = strx[2].CompareNoCase("DISCARDABLE") == 0;
                itm.strID = strx[0];
                itm.strVol = GetV(strx);
                itm.bHtml = true;
                if (0 == itm.strID.CompareNoCase(TUI_RES_XML_NAME))
                {
                    strXmlFileName = itm.strVol;
                    AddFileItem(itm);
                    continue;
                }
//                TMBool b = itm.strID == TUI_XML_NAME;
            }
            else if (strx[1] == TUI_RES_PIC_TYPE)
            {
                //itm.bDiscardable = strx[2].CompareNoCase("DISCARDABLE") == 0;
                itm.strID = strx[0];
                itm.strVol = GetV(strx);
                itm.bPic = true;
            }
            else if (pFile && strx[0] == L"#include")
            {
                TM::CFnView fnrc(pFile);
                TM::CFileName fn(fnrc.GetPath(true));
                fn.GetFullPathName(TM_A2T(strx[1]));
                idMgr.LoadHFile(fn, EM_RC, pLog); // load windows h file first
            }
        }

        m_aItem.push_back(itm);
    }

    DbgAssert(nLoop == 0);
    DbgAssert(nMenu == 0);
    DbgAssert(bString == 0);

    // insert XML resource.
    if (!m_pFileItem) //strXmlFileName.IsEmpty())
    {
        ITEM itm;
        itm.bHtml = true;
        itm.strID = TM_CS(TUI_RES_XML_NAME);
        AddFileItem(itm);
    }
}

TMBool CResFile::LoadResFile(CIDMgr& idMgr, TMCStr pFile, CString& strXmlFileName, IProjMgr* pLog)
{
    DbgAssert(pFile);
    m_aItem.clear();

    TM::CStringW strFile;
    m_eRCFileType = TxtFileLoadAs(pFile, strFile, false);
    if (m_eRCFileType == e_TxtFileStyleUnknown)
        return false;

    m_nSizeFile = 4096;
    tm_setmax(m_nSizeFile, strFile.size() * 1.1f);
    ParseResFile(idMgr, strFile, pFile, strXmlFileName, pLog);
    return true;
}

TMBool CResFile::LoadDftResFile(CIDMgr& idMgr, HINSTANCE hInst)
{
    m_aItem.clear();

    TM::CResource res;
    res.Load(hInst, TM_T("TEMPLET_RC.RC"), RT_HTML);
    TM::CStringW strFile;
    strFile.Assign((TMCStrA)res.GetData(), res.GetSize());
    m_eRCFileType = e_TxtFileStyleAnsi;
    
    CString strXml;
    ParseResFile(idMgr, strFile, 0, strXml, 0);
    return true;
}


struct LessItm
{
    bool operator () (const CResFile::ITEM& itm1, const CResFile::ITEM& itm2) const
    {
        return itm1.strID < itm2.strID;
    }
};

TMBool CResFile::SaveResFile(TMCStr pRC, const TM::CString& strXmlFileName)
{
    CAutoResFileAnalysis _ama("CResFile::SaveResFile");

    TM::CFileName strFullFileName (pRC);

    //open the file 
    TM::CFile f;
    f.CreateAlways (pRC, f.e_faReadWrite);
    if (!f)
        return false;

    // make resource XML file name
    DbgAssert(m_pFileItem);
    m_pFileItem->strVol = strXmlFileName;
    m_pFileItem->strLine = TUI_RES_XML_NAME L"                       HTML                 \"";
    m_pFileItem->strLine += strXmlFileName;
    m_pFileItem->strLine += L"\"";

    CAutoResFileAnalysis _ama2("CResFile::SaveResFile::add line");
    CStringW strOut;
    strOut.reserve(m_nSizeFile);
    for (CListRec recx (m_aItem); recx; ++recx)
    {
        CAutoResFileAnalysis _ama00("CResFile::SaveResFile::add line 1");
        strOut += (*recx).strLine;
        CAutoResFileAnalysis _ama01("CResFile::SaveResFile::add line break");
        strOut.append(L"\r\n", 2);
    }

    CAutoResFileAnalysis _ama3("CResFile::SaveResFile::TxtFileSaveAs");
    if (m_eRCFileType == e_TxtFileStyleUnknown)
        m_eRCFileType = e_TxtFileStyleAnsi;
    TxtFileSaveAs(strOut, m_eRCFileType, f);

    return  TMTrue;
}

static void Dec(CStringW& str)
{
    int i = 0;
    for (CStringW::iterator p2 = str.begin(); p2 != str.end(); ++p2, ++i)
    {
        wchar_t c = *p2;
        if (c == '\\')
        {
            ++p2;
            switch (*p2)
            {
            case 'a': c = '\a'; break;
            case 'b': c = '\b'; break;
            case 'f': c = '\f'; break;
            case 'n': c = '\n'; break;
            case 'r': c = '\r'; break;
            case 't': c = '\t'; break;
            case 'v': c = '\v'; break;
            case '\\': c = '\\'; break;
            case '\'': c = '\''; break;
            case '\"': c = '\"'; break;
            case '?': c = '?'; break;
            case '0': c = '\0'; break;
            default: DbgAssert(0); break;
            }
        }
        else if (c == '\"' && p2[1] == '\"')
            ++p2;

        str[i] = c;
    }
    str.SetLength(i);
}



// static int Dec (TMStrW p1, TMCStrW p2)
// {
//     wchar_t c;
//     
//     int i = 0;
//     for (; c = *p2; ++p2, ++p1, ++i)
//     {
//         if (c == '\\')
//         {
//             ++p2;
//             switch (*p2)
//             {
//             case '\\': c = '\\'; break;
//             case 'r': c = '\r'; break;
//             case 't': c = '\t'; break;
//             case 'n': c = '\n'; break;
//             default: DbgAssert(0); break;
//             }
//         }
//         else if (c == '\"' && p2[1] == '\"')
//             ++p2;
// 
//         *p1 = c;
//     }
//     return i;
// }

struct FmtID
{
    template <class T>
        void operator() (CIDMgr& idMgr, T& aList, CResFile::ITEM& itm, TM::IDomElement* pNode) const
    {
        int n = idMgr.Text2ID(itm.strID);
        if (n >= 0)
        {
            CStringW strSrc;

            typename T::recorder rec(aList);
            rec.find (n);
            if (rec)
            {
                strSrc = rec.get_data();
            }
            else
            {
                strSrc = itm.strVol;
                Dec (strSrc);
            }

            TM::CDomDocumentPtr doc(pNode->get_ownerDocument());
            TM::CDomElementPtr pc = pNode->appendChild(doc->createElement(VA2W(L"L%d", n)));
            pc->setAttribute(L"text", strSrc);

//             CStringW strTmp;
//             StrEnc (strTmp, strSrc);
//             str2.Format (L"      <L%d text=\"%s\"></L%d>\r\n", n, strTmp.c_str(), n);
        }
    }
};

struct FmtTxt
{
    template <class T>
    void operator() (CIDMgr& idMgr, T& aList, CResFile::ITEM& itm, TM::IDomElement* pNode) const
    {
        CStringW strSrc;

        typename T::recorder rec(aList);
        rec.find (itm.strVol);

        if (rec)
        {
            strSrc = rec.get_data();
        }
        else
        {
            strSrc = itm.strVol;
            Dec(strSrc);
        }

        CStringW strTmp;
//         StrEnc (strTmp, strSrc);

        TM::CDomDocumentPtr doc(pNode->get_ownerDocument());
        TM::CDomElementPtr pc = pNode->appendChild(doc->createElement(L"L0"));
        pc->setAttribute(L"source", CStringW(itm.strID));
        pc->setAttribute(L"text", strTmp);
    }
};


template <class T, class F, class Fmt >
void _CResFile_SaveTag (CResFile::CItemList& aItem, CIDMgr& idMgr, T& aList, TM::IDomDocument* pDoc, TM::IDomElement* pXml, TMCStr pTag, F f, Fmt fmt)
{
    IDomElement* pNode = pDoc->appendChildElement(pXml, pTag);

    for (CResFile::CListRec rec(aItem); rec; ++rec)
    {
        CResFile::ITEM& itm = *rec;
        if (f(itm, true))
        {
            CStringW str2;
            fmt (idMgr, aList, itm, pNode);
        }
    }
}

void CResFile::SaveStringXml(CIDMgr& idMgr, CRcStrList* aStr, CRcStrList* aMenuItem, CRcPopList* aMenuPop, TM::IDomElement* pXml)
{
    IDomDocument* pDoc = pXml->get_ownerDocument();
    if (aStr)
        _CResFile_SaveTag(m_aItem, idMgr, *aStr, pDoc, pXml, TUI_XML_NODE2_STRING, FindByBStr(), FmtID());
    if (aMenuItem)
        _CResFile_SaveTag(m_aItem, idMgr, *aMenuItem, pDoc, pXml, TUI_XML_NODE2_MENU, FindByBMENU(), FmtID());
    if (aMenuPop)
        _CResFile_SaveTag(m_aItem, idMgr, *aMenuPop, pDoc, pXml, TUI_XML_NODE2_MENUPOP, FindByBPop(), FmtTxt());
}


///////////////////////////////////////////////////////////////////////////////
// CResFile

#define TUI_PREFIX  _CS("/") TUI_XML_NODE0_TMDUI  _CS("/")

CQRcFile::CQRcFile()
{
}

IDomNode* CQRcFile::FindTuiElement()
{
    IDomNode* p = p = m_ptrFile->get_firstChild(); //<RCC>
    if (!p)
        p =  m_ptrFile->appendChildElement(m_ptrFile, TM_CL("RCC"));
    for (IDomNode* pp = p->get_firstChild(); pp; pp = pp->get_nextSibling())
    {
        CDomElementPtr ele(pp);
        if (!ele)
            continue;
        CDomStrPtr str;
        ele->getAttribute(TM_CL("prefix"), str);
        if (str.Left(5) == TUI_PREFIX)
        {
            m_ptrDom = ele;
            break;
        }
    }
    return p;
}

void CQRcFile::AddFile(TMCStrW strFile)
{
    DbgAssert(m_ptrFile);
    DbgAssert(m_ptrDom);
    IDomElement* p = m_ptrFile->appendChildElement(m_ptrDom, L"file");
    m_ptrFile->appendTextElement(p, strFile);
}

TMBool CQRcFile::LoadResFile(CIDMgr& idMgr, TMCStr pFile, CString& strXmlFileName)
{
    if (!m_ptrFile.LoadFromFile(pFile))
        return false;
    strXmlFileName = TM_CS(TUI_RES_XML_FILE);
    return true;
}

TMBool CQRcFile::LoadDftResFile(CIDMgr& idMgr)
{
    m_ptrDom.Release();
    m_ptrFile.clear();
    IDomElement* p = m_ptrFile->appendChildElement(m_ptrFile, TM_CL("RCC"));
    return true;
}

TMBool CQRcFile::SaveResFile(TMCStr pRC, const TM::CString& strXmlFileName)
{
    TM::CFileName strFullFileName(pRC);
//     TM::CString StrCopyPath = strFullFileName.GetPath(true);
    if (m_ptrDom)
    {
        TM::CFnView fn(pRC);
        CStringW str(TUI_PREFIX);
        str += fn.GetBody();
        m_ptrDom->setAttribute(L"prefix", str);
    }

    return m_ptrFile.SaveToFile(strFullFileName);
}



///////////////////////////////////////////////////////////////////////////////
// CSaveFileRes
CSaveFileRes::CSaveFileRes(CResFile& res, CQRcFile& qrc, const TM::CString& path)
    : m_fnPath(path)
    , m_aItem(res.m_aItem)
    , m_iter(res.m_aItem.end())
    , m_fileQrc(qrc)
{
    // qrc file
    IDomNode* pParent = qrc.FindTuiElement();
    if (qrc.m_ptrDom)
        qrc.m_ptrFile->removeChild(qrc.m_ptrDom);
    qrc.m_ptrDom = qrc.m_ptrFile->appendChildElement(pParent, L"qresource");
    qrc.AddFile(TUI_RES_XML_NAME);

    // rc file
    // 1. delete all of the TUIPIC
    // 2. get the position of the last TUIPIC
    for (CResFile::CListRec rec(res.m_aItem); rec.find(FindByBPIC(), true), rec;)
    {
        rec.erase();
        m_iter = rec.get_iter();
    }
}

void CSaveFileRes::RegisterFile(TM::CString& strFN)
{
    if (strFN.IsEmpty())
        return;

    // get the file name ID
    TM::CFileName str(strFN);
    LPCTSTR p = str.GetName();
    CResFile::ITEM itm;
    itm.strID = p;
    itm.strID.MakeUpper();
//     itm.strID.Replace(' ', '_'); // don't convert
    
    // make relative file name
    if (str.IsRelativePath())
        itm.strVol = str;
    else if (str.MakeRelativePath(m_fnPath, true))
        itm.strVol = str;
    else
        itm.strVol = p;
    strFN = itm.strVol;

    // check if there is a duplicated file
    if (!m_aFile.insert(itm.strID).second)
        return;
    
    // make target file name
    TM::CFileName strPath(m_fnPath + itm.strVol);

    // create dir.
    TM::CString fnPath(strPath.GetPath(true));
    if (m_aFile.insert(fnPath).second)
        FsMkDirs(fnPath);

    // copy file
    CopyFile(str, strPath, false);
    
    // write to the qrc item
    m_fileQrc.AddFile(itm.strVol);

    // write to the rc item
    itm.bPic = true;
    TM::CStringW strLine;
    strLine.Format(L"%-32s " TUI_RES_PIC_TYPE L"               \"%s\"", itm.strID.c_str(), itm.strVol.c_str());
    itm.strLine = strLine;
    m_aItem.insert(m_iter, itm);
}
