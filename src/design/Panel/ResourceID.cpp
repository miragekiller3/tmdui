#include "StdAfx.h"
#include "ResourceID.h"
#include "tmfilename.h"
#include "tmdebugext.h"


using namespace TM;

#define CAutoHFileAnalysis CAutoAnalysisNull
//#define CAutoHFileAnalysis CAutoAnalysisTuiUser


#define NEXT_CONTROL_VALUE  L"_TUI_NEXT_CONTROL_VALUE"

//#define FIRST_CONTROL_VALUE 10001
#define USER_STEP           200
#define USER_FIRST          1000


TM::CString RIDInfo::GetDisplayText()
{
    if (!str.IsEmpty())
        return str;
    CString str2;
    _itot(id, str2.GetBuffer(32), 10);
    str2.ReleaseBuffer();
    return str2;
}

void RIDInfo::CreateDisplayText(TMCStr pClass)
{
    TMCStr pHead = _T("IDC");
    switch (mode)
    {
    case EM_DIALOG: pHead = _T("IDD"); break;
    case EM_WGT: pHead = _T("IDW"); break;
    case EM_FRAME: pHead = _T("IDF"); break;
    case EM_PIC: pHead = _T("IDP"); break;
    case EM_CONTROL: pHead = _T("IDC"); break;
    }

    str.Format(_T("%s_%s_%d"), pHead, pClass, id);
    str.MakeUpper();
    str.Replace('.', '_');
}

// auto created id always have EM_OWNER.
CIDMgr::RIDInfoX* CIDMgr::CRIDInfoList::insert(iterator __it, const RIDInfo& __x, IProjMgr* pLog)
{
    TMBool bInsertIDTable  = (__x.id > 0) && (__x.mode & (TM::EM_ALLOBJ | TM::EM_NEXT | TM::EM_OWNER));
    TMBool bInsertStrTable = (__x.id > 0);

    if (bInsertIDTable)
    {
        RIDInfoX* rec = FindByID(__x.id);
        if (rec)
        {
            if (pLog)
                pLog->Alert(L"remove duplicate id: (%d,%s,%s) <- (%d,%s,%s);\r\n",
                __x.id, __x.str.c_str(), __x.strFn.c_str(),
                rec->id, rec->str.c_str(), rec->strFn.c_str());
            return 0;
        }
    }

    if (bInsertStrTable)
    {
        RIDInfoX* rec = FindByStr(__x.str);
        if (rec)
        {
            if (pLog)
                pLog->Alert(L"remove duplicate macro: (%d,%s,%s) <- (%d,%s,%s);\r\n",
                __x.id, __x.str.c_str(), __x.strFn.c_str(),
                rec->id, rec->str.c_str(), rec->strFn.c_str());
            bInsertStrTable = false;
        }
    }

    RIDInfoX* p = new RIDInfoX(__x);
    iterator iter = base_class::insert(__it, p);
    p->iter = iter;
    if (bInsertIDTable)
        p->iterID = m_aIndexID.insert2(p->id, p).first;
    else
        p->iterID = m_aIndexID.end();
    if (bInsertStrTable)
        p->iterStr = m_aIndexStr.insert2(p->str, p).first;
    else
        p->iterStr = m_aIndexStr.end();

    return p;
}

void CIDMgr::CRIDInfoList::ResetID(const CIDMgr::RIDInfoX* p, TMUInt32 id)
{
    DbgAssert(m_aIndexID.find(id) == m_aIndexID.end());
    DbgAssert(m_aIndexID.find(p->id) != m_aIndexID.end());
    m_aIndexID.erase(p->id);
    CIDMgr::RIDInfoX* pp = const_cast<CIDMgr::RIDInfoX*>(p);
    pp->id = id;
    pp->iterID = m_aIndexID.insert2(id, pp).first;
}

void CIDMgr::CRIDInfoList::ResetStr(const CIDMgr::RIDInfoX* p, const CString& str)
{
    DbgAssert(m_aIndexStr.find(str) == m_aIndexStr.end());
    DbgAssert(m_aIndexStr.find(p->str) != m_aIndexStr.end());
    m_aIndexStr.erase(p->str);
    CIDMgr::RIDInfoX* pp = const_cast<CIDMgr::RIDInfoX*>(p);
    pp->str = str;
    pp->iterStr = m_aIndexStr.insert2(str, pp).first;
}

void CIDMgr::CRIDInfoList::Erase(const RIDInfoX* p)
{
    m_aIndexStr.erase(p->iterStr);
    m_aIndexID.erase(p->iterID);
    this->erase(p->iter);
    delete p;
}

int CIDMgr::CRIDInfoList::GetUserStepID(int start)
{
    for (;;)
    {
        CRIDByIDList::const_iterator iter = m_aIndexID.lower_bound(start);
        if (iter == m_aIndexID.end())
            return start;
        CIDMgr::RIDInfoX* p = iter->second;
        if (p->id > start + USER_STEP)
            return start;
        start += USER_STEP;
    }
}


////////////////////////////////////////////////////////////////////////////////
// struct RIDInfo
CIDMgr::CIDMgr ()
    : m_nSizeFile(4096)
{
    m_strUser = NEXT_CONTROL_VALUE;
    Reset();
}

void CIDMgr::Reset()
{
    m_aRID.clear();
    m_aHFiles.clear();
    LoadSysID();
}

void CIDMgr::LoadSysID()
{
#define INSERT_DEFAULT_ID(tp,x) \
    { \
    RIDInfo info; \
    info.str = TM_CT(#x); \
    info.id = x; \
    info.mode = EM_PRE | tp; \
    m_aRID.push_back(info, 0); \
    } \
    
    INSERT_DEFAULT_ID(EM_CONTROL, IDOK);
    INSERT_DEFAULT_ID(EM_CONTROL, IDCANCEL);
    INSERT_DEFAULT_ID(EM_CONTROL, IDABORT);
    INSERT_DEFAULT_ID(EM_CONTROL, IDRETRY);
    INSERT_DEFAULT_ID(EM_CONTROL, IDIGNORE);
    INSERT_DEFAULT_ID(EM_CONTROL, IDYES);
    INSERT_DEFAULT_ID(EM_CONTROL, IDNO);
    INSERT_DEFAULT_ID(EM_CONTROL, IDCLOSE);
    INSERT_DEFAULT_ID(EM_CONTROL, IDHELP);
    
//     INSERT_DEFAULT_ID(EM_PRE, IDP_PRE_OBJECT_BASE);

    INSERT_DEFAULT_ID(EM_CONTROL, IDP_PRE_BIND_CHECK);
    INSERT_DEFAULT_ID(EM_CONTROL, IDP_PRE_BIND_EXPAND);
    INSERT_DEFAULT_ID(EM_CONTROL, IDP_PRE_BIND_INDENT);
    INSERT_DEFAULT_ID(EM_CONTROL, IDP_PRE_BIND_LABLE);
    INSERT_DEFAULT_ID(EM_CONTROL, IDP_PRE_BIND_SELECTED);

    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_NULL);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_BUTTON);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_CHECK);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_RADIO);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_GROUP);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_COMB);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_LIST_ITEM);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_MENU);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_MENU_POP);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_HEADER_UP);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_HEADER_DOWN);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_UP);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_DOWN);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_FOCUS);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_SCROLLBAR);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_SIZEBOX);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_OUTERBORDER);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_INNERBORDER);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_CAPTION);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_CLOSE);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_MIN);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_MAX);
    INSERT_DEFAULT_ID(EM_PIC, IDP_PRE_PIC_HELP);

#undef INSERT_DEFAULT_ID
}


static CString _CIDMgr_MakeUserMacro(const CString& _strUser)
{
    CString strMacro(TM_CS(NEXT_CONTROL_VALUE));
    
    CString strUser = _strUser.Trim();
    if (!strUser.IsEmpty())
    {
        strUser.MakeUpper();
        strUser.Replace(L"  ", L" ");
        strUser.Replace(L"  ", L" ");
        strUser.Replace(L"  ", L" ");
        strUser.Replace(TM_T(' '), TM_T('_'));
        strUser.Replace(TM_T('@'), TM_T('_'));
        strUser.Replace(TM_T('.'), TM_T('_'));
        strUser.Replace(TM_T('<'), TM_T('_'));
        strUser.Replace(TM_T('>'), TM_T('_'));
        strMacro += '_';
        strMacro += strUser;
    }
    return std::move(strMacro);
}

void CIDMgr::SetUser(const TM::CStrView& strUser)
{
    m_strUserSave = strUser;
    m_strUser = _CIDMgr_MakeUserMacro(strUser);
}

const RIDInfo* CIDMgr::Next(EDIT_MODE eMode, TMCStr pClass, const TM::CString* strMacro)
{
    RIDInfo ridNew;

    const RIDInfoX* rec = m_aRID.FindByStr(m_strUser);
    if (!rec)
    {
        // the user first create object.
        int n = GetUserStepID();
        ridNew.id = n;

        RIDInfo rid;
        rid.id = n + 1;
        rid.mode = EM_NEXT;
        rid.str = m_strUser;
        rec = m_aRID.push_back(rid, 0);
    }
    else
    {
        TMInt32 idNext = rec->id;
        ridNew.id = idNext;

        // make sure next id not used
        if (m_aRID.FindByID(++idNext))
            idNext = GetUserStepID();
        m_aRID.ResetID(rec, idNext);

        // add blank line
        CRIDInfoList::iterator iter = rec->iter;
        ++iter;
        if (iter == m_aRID.end() || !(*iter)->str.IsEmpty())
        {
            RIDInfo rid;
            rid.id = -1;
            rid.mode = EM_CONTROL;
            m_aRID.insert(iter, rid);
        }
    }

    ridNew.mode = eMode | TM::EM_OWNER;
    if (strMacro)
        ridNew.str = *strMacro;
    else
        ridNew.CreateDisplayText(pClass);
    return m_aRID.insert(rec->iter, ridNew);
}



////////////////////////////////////////////////////////////////////////////////
// class CIDMgr

// h file definition£º#define IDC_XXX n1   //{index} n2

void CIDMgr::LoadHFile(TMCStr pFileName, EDIT_MODE em, IProjMgr* pLog)
{
    // don't load same file 2nd.
    if (!m_aHFiles.insert(pFileName).second)
        return;

    CStringW strFile;
    if (TxtFileLoadAs(pFileName, strFile, false) == e_TxtFileStyleUnknown)
        return;
    tm_setmax(m_nSizeFile, strFile.size());

    CFileName fn(CFnView(pFileName).GetName());
    
    CStringW str;
    for (int nStart = 0; nStart = strFile.SplitLine(nStart, str);)
    {
        str = str.Trim();

        RIDInfo info;
        info.strFn = fn;
        info.mode = em;

        CStringW strx[4];
        CStringW strXml;
        ParseLine2(str, strx, tm_countof(strx), &strXml);
        int nid = strx[2].ToLong();

        if (nid != 0 && !strx[1].IsEmpty() && strx[0] == L"#define" && !strx[2].IsEmpty())
        {
            info.id = nid;
            info.strXml = strXml;

            if (strx[1].Find(NEXT_CONTROL_VALUE) >= 0)
                info.mode = EM_NEXT;
            
            info.str = strx[1];
        }
        else
        {
            info.id = -1;
            info.str = str;
        }
        info.str.freeze();

        m_aRID.push_back(info, pLog);
    }
}

static CStringA MakeXmlString(const CString& xml, int nRef, TMBool bOwner, const CString& strUserSave)
{
    if (!nRef)
        return CStringA();

    CDomDocument doc;
    doc->LoadXml(xml);
    CDomElementPtr ptr(doc->selectSingleNode(L"tui"));
    if (!ptr)
        ptr = doc->appendChildElement(doc, L"tui");

    ptr->removeAttribute(L"removed");
    if (bOwner)
        ptr->setAttribute(L"user", strUserSave);
    if (nRef > 1)
        ptr->setAttributeLong(L"ref", nRef - 1);
    else
        ptr->removeAttribute(L"ref");

    CStringW str(L"//");
    ptr->transformNode(str, true, 0);
    str.Remove(L'\r');
    str.Remove(L'\n');
    str.Replace(L"<tui user=\"\"", L"<tui");
    str.Replace(L"<tui></tui>", L"<tui />");
    str.Replace(L"//<tui removed=\"1\"></tui>", L"");
    return str;
}

// nRef == 0: unused;
static void WriteLine(CStringA& f, const RIDInfo& inf, int nRef, const CString& strUserSave)
{
#define LINE_FMT            "#define %-43s %-7d %s"

    if (inf.id == -1)
    {
        f += inf.str;
    }
    else if (inf.IsFlagOwner() && !nRef)
        return;
    else
    {
        CStringA strXml;
        if (inf.mode != EM_NEXT)
        {
            strXml = MakeXmlString(inf.strXml, nRef, inf.IsFlagOwner(), strUserSave);
            if (strXml.IsEmpty())
                return;
        }
        CStringA str;
        str.Format(LINE_FMT,TM_T2A(inf.str), inf.id, strXml.c_str());
        f += str;
    }

    f.append("\r\n", 2);
}

static void _CIDMgr_WriteID(CIDMgr::CRIDInfoList& aInfo, CStringA& f, CIDUsedList& aID, const CString& strUserSave)
{
    for (CIDMgr::CRIDInfoRec rec(aInfo); rec; ++rec)
    {
        RIDInfo& inf = **rec;
        if ((inf.id < 0 || !inf.str.empty()) && !(inf.mode & EM_WIN))
        {
            CIDUsedList::recorder rec(aID);
            rec.find(inf.id);
            int nRef = rec ? rec.get_data() : 0;
            WriteLine(f, inf, nRef, strUserSave);
        }
    }
}

void CIDMgr::SaveHFile(TMCStr pFileName, CIDUsedList& aID)
{
    CAutoHFileAnalysis _ama("CIDMgr::SaveHFile");
    CStringA str;
    str.reserve(m_nSizeFile);
    _CIDMgr_WriteID(m_aRID, str, aID, m_strUserSave);
//     WriteLine (str, NEXT_CONTROL_VALUE, s_idCtrl.Get());
    //WriteLine (str, NEXT_OBJECT_VALUE, s_idObj.Get());

    CAutoHFileAnalysis _ama2("CIDMgr::SaveHFile::check");
    //return if the file is not changed  
    {
        CFile f;
        f.OpenExisting(pFileName, f.e_faRead);
        if (f)
        {
            CStringA str2;
            int n = f.GetSize();
            f.Read(str2.GetBuffer(n), n);
            if (str2 == str)
                return;
        }
    }

    CAutoHFileAnalysis _ama3("CIDMgr::SaveHFile::dave");
    {
        CFile f;
        f.CreateAlways (pFileName, f.e_faWrite);
        if (!f)
            return;
        f.Write(str, str.GetLength());
    }
}

void CIDMgr::GetIDs(CPopIDList& aID, EDIT_MODE dwMask)
{
    for (CRIDInfoRec rec(m_aRID); rec; ++rec)
    {
        RIDInfo& inf = **rec;
        if (((inf.mode & dwMask) == dwMask) && (inf.id != -1))
        {
            aID.push_back(CPopIDList::value_type(inf.GetDisplayText(), inf.id));
// 
//             TM::CTVItem itmc = itm.AppendChild(inf.GetDisplayText());
//             itmc.SetData((void*)inf.id, false);
        }
    }
}



TM::CString CIDMgr::ID2Text(TMUInt32 dwID, TM::CString* pStrUser)
{
    if (dwID == (TMUInt32)-1) // -1 is not used line
        return TM::CStrView();

    TM::CStrPtr str;
    const RIDInfoX* ri = m_aRID.FindByID(dwID);
    if (ri)
    {
       if (ri->str.empty())
            dwID = ri->id;
        else
        {
            str = ri->str;
            if (pStrUser)
            {
                *pStrUser = ri->strXml;
                if (ri->IsFlagOwner())
                    *pStrUser += m_strUserSave;
            }
            return str;
        }
    }

    str.Format(L"%d", dwID);
    return str;
}

TMUInt32 CIDMgr::CreateStepObjectID(EDIT_MODE eMode, TMCStr pClass, TM::CString& strMacro)
{
    const RIDInfo* rid = Next(eMode, pClass, 0);
    strMacro = rid->str;
    return rid->id;
}

TMUInt32 CIDMgr::Text2ID(const TM::CString& strMacro, EDIT_MODE em)
{
    TM::CString str(strMacro);
    str = str.Trim();
    TMUInt32 n = str.ToLong();
    if (n != 0 || str.IsEmpty() || str[0] == '0' || str == TM_T("null"))
        return n;

    const RIDInfoX* rec = m_aRID.FindByStr(str);
    if (rec)
        return rec->id;

    if (em)
        return Next(em, 0, &strMacro)->id;

    return 0;
}

void CIDMgr::AutoCreateMacroByID(TMUInt32 dwID, TMCStr pClass, EDIT_MODE em)
{
    if (dwID <= 0)
        return;

    RIDInfoX* p = m_aRID.FindByID(dwID);
    if (p)
    {
        p->mode |= em;
        return; // need't create
    }

    // if h file have not macro, but xml file have object.
    RIDInfo ri;
    ri.id = dwID;
    ri.mode = em;
    ri.CreateDisplayText(pClass);
    m_aRID.push_back(ri, 0);
}

TM::CString CIDMgr::SetObjectIDMacro(const TM::CStrView& pOld, const TM::CString& strNew)
{
    DbgAssert(strNew != pOld);
    DbgAssert(!strNew.IsEmpty());

    TM::CString strRet;

    // return false if the pNew already exists
    const RIDInfoX* rec1 = m_aRID.FindByStr(strNew);
    if (rec1)
    {
        strRet.Format(TM_T("There alreay have ID named %s or the macro is illegal."), strNew.c_str());
        return std::move(strRet);
    }

    const RIDInfoX* rec = m_aRID.FindByStr(pOld);
    if (rec)
    {
        m_aRID.ResetStr(rec, strNew);
        return std::move(strRet);
    }

    // old not find
    strRet.Format(TM_T("The ID %s not find"), pOld);
    return std::move(strRet);
}

void CIDMgr::SetType (const TM::CString& strMacro, EDIT_MODE eMode)
{
    RIDInfoX* rec = m_aRID.FindByStr(strMacro);
    if (rec)
        rec->mode = eMode;
}

void CIDMgr::ResetUserIds(CResetIDMap& stm, int nId, const TM::CStrView& strUser)
{
    for (CRIDInfoRec rec(m_aRID); rec; ++rec)
    {
        RIDInfoX* inf = *rec;
        if ((inf->id > 0) && (inf->mode & EM_ALLOBJ)) // blank line id == -1;
        {
            if (inf->GetUser() == strUser)
            {
                int idOld = inf->id; 
                nId = m_aRID.GetUserStepID(nId);
                m_aRID.ResetID(inf, nId);
                stm.insert2(idOld, nId);
                ++nId;
            }
        }
    }
    CString strMacro = _CIDMgr_MakeUserMacro(strUser);
    RIDInfoX* pNext = m_aRID.FindByStr(strMacro);
    if (pNext)
    {
        nId = m_aRID.GetUserStepID(nId);
        m_aRID.ResetID(pNext, nId);
    }
}

int CIDMgr::GetUserStepID()
{
    return m_aRID.GetUserStepID(USER_FIRST);
}

void CIDMgr::GetUsers(CPopIDList& aMacro)
{
    CSet<CXmlTag> aUsers;
    CRIDByIDList::recorder rec(m_aRID.m_aIndexID);
    for (; rec; ++rec)
    {
        RIDInfo* p = rec.get_data();
        if (p->mode & (EM_WIN | EM_NEXT))
            continue;
        CXmlTag tagUser = p->GetUser();
        if (aUsers.insert(tagUser).second) // first insert.
            aMacro.insert(CPopIDList::value_type(tagUser, p->id));
    }
}

void CIDMgr::MergeUsers(CPopIDList& aMacro)
{
    if (aMacro.size() <= 1)
        return;

    CPopIDList::recorder rec(aMacro);
    CString str = rec.get_data().first;
    for (; ++rec;)
    {
        CXmlTag strReplace = rec.get_data().first;
        CString strNext = _CIDMgr_MakeUserMacro(strReplace);

        for (CRIDInfoRec rec(m_aRID); rec; ++rec)
        {
            RIDInfoX* p = *rec;
            if (p->mode & EM_WIN)
                continue;
            if (p->mode == EM_NEXT) // delete next macro
            {
                if (p->str == strNext)
                {
                    --rec;
                    m_aRID.Erase(p);
                    continue;
                }
            }
            CXmlTag tag = p->GetUser();
            if (strReplace == tag)
                p->strXml = MakeXmlString(p->strXml, 1, true, str);
        }
    }
}

