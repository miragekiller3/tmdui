#ifndef RESOURCEID_H
#define RESOURCEID_H


#include "IdStream.h"
#include "../Design.h"
#include "tmautoptr.h"


inline bool _isspace (wchar_t ch)
{
    return ch==L' ' || ch==L'\r' || ch==L'\n' || ch==L'\t';
}

// template <class T>
// LPCWSTR ParseLine(LPCWSTR p2, T* strx, int nSize)
// {
//     if (!p2)
//         return 0;
// 
//     LPCWSTR p1;
//     
//     for (int i = 0; i < nSize; ++i)
//     {
//         for (; p2[0] && (_isspace(p2[0]) || p2[0]==','); ++p2);
//         if (!p2[0])
//             return 0;
//         
//         p1 = p2;
//         if (p2[0] == '\"')
//         {
//             for (++p2; p2[0]; ++p2)
//             {
//                 if (p2[0] == '\"')
//                 {
//                     if (p2[1] == '\"')
//                         ++p2;
//                     else
//                         break;
//                 }
//             }
//             strx[i].Assign (p1+1, p2-p1-1);
//             ++p2;
//         }
//         else
//         {
//             for (; p2[0] && !_isspace(p2[0]) && p2[0]!=','; ++p2);
//             strx[i].Assign (p1, p2-p1);
//             int n = strx[i].Find(L"//");
//             if (n >= 0)
//             {
//                 strx[i].SetLength (n);
//                 return p1 + n;
//             }
//         }
//     }
//     return 0;
// }

inline void ParseLine2(const TM::CStringW& str, TM::CStringW* strx, int nSize, TM::CStringW* strXml)
{
    if (!str)
        return;
    
    LPCWSTR p0 = str;
    LPCWSTR p2 = p0;
    LPCWSTR p1;
    
    for (int i = 0; i < nSize; ++i)
    {
        for (; p2[0] && (_isspace(p2[0]) || p2[0]==','); ++p2);
        if (!p2[0])
            return;
        
        p1 = p2;
        if (p2[0] == '\"')
        {
            for (++p2; p2[0]; ++p2)
            {
                if (p2[0] == '\"')
                {
                    if (p2[1] == '\"')
                        ++p2;
                    else
                        break;
                }
            }
            strx[i] = str.Mid(p1 + 1 - p0, p2-p1-1);
            ++p2;
        }
        else
        {
            for (; p2[0] && !_isspace(p2[0]) && p2[0]!=','; ++p2);
            strx[i] = str.Mid(p1 - p0, p2 - p1);
            int n = strx[i].Find(L"//");
            if (n >= 0)
            {
                strx[i].SetLength(n);
                if (strXml)
                    *strXml = str.Mid(p1 + n - p0);
                return;
            }
        }
    }
    return;
}



struct RIDInfo
{
    TM::CString     str;        // macro string, if not macro, it's line text
    TM::CString     strXml;     // line info
    TM::CString     strFn;      // file name
    TMInt32         id;
    TM::EIDTYPE     mode;       // union of EDIT_MODE
    
    TM_DECLARE_FLAG_GET(FlagOwner, mode, TM::EM_OWNER);
    
    TM::CString GetDisplayText();
    void CreateDisplayText(TMCStr pClass);
    TM::CStrView GetUser()
    {
//         CDomStrViewPtr propUser;
//         TM::CDomDocument doc;
//         doc->LoadXml(strXml);
//         TM::CDomElementPtr ptr(doc->get_firstChild());
//         if (ptr)
//             ptr->getAttribute(L"user", propUser);
//         return propUser;

        TM::CXmlTag tag(strXml);
        TM::CXmlTag propUser;
        XmlGetProperty(tag, L"user", propUser);
        return propUser;
    }

};


class CIDMgr
{
public:
    struct RIDInfoX;
    typedef TM::CList<RIDInfoX*>                            _CRIDInfoList;
    typedef TM::CMap<TMInt32, RIDInfoX*>                    CRIDByIDList;
    typedef TM::CMap<TM::CStrView, RIDInfoX*>               CRIDByStrList;
    struct RIDInfoX : public RIDInfo
    {
        RIDInfoX() {}
        RIDInfoX(const RIDInfo& ri) : RIDInfo(ri) {}
        
        _CRIDInfoList::iterator iter;
        CRIDByIDList::iterator iterID;
        CRIDByStrList::iterator iterStr;
    };

    struct CRIDInfoList : public _CRIDInfoList
    {
        typedef _CRIDInfoList                           base_class;
        typedef base_class::iterator                    iterator;
        typedef base_class::recorder                    recorder;

        CRIDByIDList    m_aIndexID;     // only support TMDui id.
        CRIDByStrList   m_aIndexStr;    // support TMDui id and RC id.

        ~CRIDInfoList()
        {
            clear();
        }

        RIDInfoX* FindByID(TMUInt32 id)
        {
            CRIDByIDList::iterator iter = m_aIndexID.find(id);
            if (iter == m_aIndexID.end())
                return 0;
            return iter->second;
        }

        RIDInfoX* FindByStr(const TM::CStrView& str)
        {
            CRIDByStrList::iterator iter = m_aIndexStr.find(str);
            if (iter == m_aIndexStr.end())
                return 0;
            return iter->second;
        }

        void clear() { m_aIndexID.clear(); m_aIndexStr.clear(); recorder::list_clear(*this, TM::CA_Delete()); }
        
        RIDInfoX* push_back(const RIDInfo& __x, IProjMgr* pLog) { return insert(end(), __x, pLog); }

        RIDInfoX* insert(iterator __it, const RIDInfo& __x, IProjMgr* pLog = 0);

        void ResetID(const RIDInfoX* p, TMUInt32 id);
        void ResetStr(const RIDInfoX* p, const TM::CString& str);
        void Erase(const RIDInfoX* p);

        int GetUserStepID(int start);
    };

    typedef CRIDInfoList::recorder  CRIDInfoRec;

private:
    TM::CSet<TM::CString> m_aHFiles;
    TM::CString m_strUser;
    int m_nSizeFile;
    TM::CString m_strUserSave;
    CRIDInfoList m_aRID;
    
    const RIDInfo* Next(EDIT_MODE eMode, TMCStr pClass, const TM::CString* strMacro);
    void LoadSysID();

public:
    CIDMgr ();
    void Reset();
    void LoadHFile(TMCStr pFileName, EDIT_MODE em, IProjMgr* pLog);
    void SaveHFile(TMCStr pFileName, CIDUsedList& aID);
    TMUInt32 CreateStepObjectID (EDIT_MODE eMode, TMCStr pClass, TM::CString& strMacro);
    void AutoCreateMacroByID(TMUInt32 dwID, TMCStr pClass, EDIT_MODE em);
    void GetIDs(CPopIDList& aID, EDIT_MODE nType);
    
    // replace macro;
    // return 0: old err or new have exist;
    // return 1: success;
    // return 2: the old == new;
    TM::CString SetObjectIDMacro(const TM::CStrView& pOld, const TM::CString& strNew);
    void SetType (const TM::CString& strMacro, EDIT_MODE eMode);

    void ResetUserIds(CResetIDMap& stm, int nId, const TM::CStrView& strUser);

    void SetUser(const TM::CStrView& strUser);

    TMUInt32 Text2ID(const TM::CString& strMacro, EDIT_MODE eCreateFlag = TM::EM_NULL);
    TM::CString ID2Text(TMUInt32 dwID, TM::CString* pStrUser = 0);

    int GetUserStepID();
    void GetUsers(CPopIDList& aMacro);
    void MergeUsers(CPopIDList& aMacro);

};

#endif // RESOURCEID_H