#ifndef RESFILE_H
#define RESFILE_H

#include "tmdom.h"
#include "tmlist.h"


class CIDMgr;
class IProjMgr;


typedef TM::CMap<int, TM::CString>          CRcStrList;
typedef TM::CMap<TM::CString, TM::CString>  CRcPopList;


class CResFile
{
public:
    struct ITEM
    {
        union
        {
            struct 
            {
                bool bString    : 1;
                bool bStringID  : 1;
                bool bMenu      : 1;
                bool bPop       : 1;
                bool bHtml      : 1;
                bool bPic       : 1;
            };
            TMUInt32 mask;
        };

        TM::CStringW strID;
        TM::CStringW strVol;
        TM::CStringW strLine;
        
        ITEM()
            : mask(0)
        {
        }
    };
    typedef TM::CList<ITEM>             CItemList;
    typedef TM::CList<ITEM>::recorder   CListRec;
    typedef TM::CSet<TM::CString*>      CFileNameList;
    typedef CFileNameList::recorder     CFileNameRec;
    friend class CSaveFileRes;

private:
    CItemList       m_aItem;
    ETXTFILESTYLE   m_eRCFileType;
//    CFileNameList   m_aFn;
    int             m_nSizeFile;
    ITEM*           m_pFileItem;

private:
    void AddFileItem(const ITEM& itm);
    void ParseResFile(CIDMgr& idMgr, TM::CStringW strFile, TMCStr pFile, TM::CString& strXmlFileName, IProjMgr* pLog);
//     void SetPicRes(const TM::CString& StrCopyPath);

public:
    CResFile() : m_eRCFileType(e_TxtFileStyleAnsi), m_nSizeFile(4096) {}
    TMBool LoadResFile(CIDMgr& idMgr, TMCStr pFile, TM::CString& strXmlFileName, IProjMgr* pLog);
    TMBool LoadDftResFile(CIDMgr& idMgr, HINSTANCE hInst);
    TMBool SaveResFile(TMCStr pRC, const TM::CString& strXmlFileName);
    void SaveStringXml(CIDMgr& idMgr, CRcStrList* aStr, CRcStrList* aMenuItem, CRcPopList* aMenuPop, TM::IDomElement* pXml);
//     void RegisterFile(TM::CString& strFN);
};

class CQRcFile
{
private:
    TM::CDomDocument   m_ptrFile;
    TM::CDomElementPtr m_ptrDom;
    friend class CSaveFileRes;

    TM::IDomNode* FindTuiElement(); // return element of <RCC>
    void AddFile(TMCStrW strFile);

public:
    CQRcFile();
    TMBool LoadResFile(CIDMgr& idMgr, TMCStr pFile, TM::CString& strXmlFileName);
    TMBool LoadDftResFile(CIDMgr& idMgr);
    TMBool SaveResFile(TMCStr pRC, const TM::CString& strXmlFileName);
};


class CSaveFileRes
{
private:
    const TM::CString&      m_fnPath;
    CResFile::CItemList&    m_aItem;
    CResFile::CItemList::iterator m_iter;
    std::set<TM::CString>   m_aFile;
    std::set<TM::CString>   m_aDir;
    CQRcFile&               m_fileQrc;

public:
    CSaveFileRes(CResFile& res, CQRcFile& qrc, const TM::CString& path);
    void RegisterFile(TM::CString& strFN);

};

// extern CResFile s_resFile;


#endif