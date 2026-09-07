
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

#pragma  once 

#include "../Design.h"

enum EKeyType
{
    eKeyTypeNormal = 0,
    eKeyTypeMacro = 1,
    eKeyTypeClass = 2,
    eKeyTypeMember = 3, // m_XXX
    eKeyTypeFunction = 4, // void foo()
};


struct CSearchResult
{
    struct SIMPLERST
    {
        TM::CString filename;
        TM::CString pathname;
        TM::CStringA line;
        int nLine;
        bool operator < (const SIMPLERST& rhs) const 
        {
            if (filename != rhs.filename)
                return filename < rhs.filename;
            return nLine < rhs.nLine;
        }
    };
    typedef TM::CMultiSet<SIMPLERST> CSRList;

    TM::CStringA key;
    TM::CStringA keyContext;
    CSRList     aResult;
    CPreSearchID::CMacoList aChilds;
    int nRefByFile;
    int nRefByBody;

    bool operator < (const CSearchResult& rhs) const
    {
        return key < rhs.key;
    }

    void Insert(const SIMPLERST& sr);

    void MakeRef();
};

typedef TM::CMap<TM::CStringA, CSearchResult> CLineMap; // keyword, result



struct CKeyWord
{
    typedef TM::CMap<TM::CStringA, TM::CStringA> CKeyList; // str, full str
    CKeyList        m_aKeys;

private:
    void InsertKey(TM::CStringA str, const TM::CStringA& strFullKey);
//     void InsertFuncLine(const TM::CStringA& strLine, const TM::CStringA& strFullLine);
    void InsertKeyLine(const TM::CStringA& strLine, EKeyType nType);
    void InsertSelfLine(const TM::CStringA& strLine, const TM::CStringA& strFullLine, TMCStrA pKey, TMCStrA pKeyPre, TMCStrA pKeyEnd, TMBool bSplitKey);
    void InsertSelfFunction(const TM::CStringA& strLine, const TM::CStringA& strFullLine);

public:
    void ParseFile(const TM::CString& strTxt, EKeyType nType);
    void SearchFile(const TM::CString& strPath, EKeyType nType);
};


void CheckKeyword(const TM::CString& strPath, CKeyWord& kw, CLineMap& mp);


// void ResetControlId(int nId);
