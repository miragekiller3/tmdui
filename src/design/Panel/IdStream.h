//#include "../../tmlist.h"

#ifndef NULLSTREAM_H
#   include "NullStream.h"
#endif

#pragma  once 



typedef TM::CMap<TMUInt32, TMUInt32> CResetIDMap;
class CPanelApp;


TM_BGN_NAMESPACE



class CIdResetStream : public CIdStream
{
public:
    const CResetIDMap& m_idResetMap;

public:
    CIdResetStream(const CResetIDMap& aID);
    ~CIdResetStream(void);

    template<class T> TMUInt32 ResetPropID(T& __val)
    {
        CResetIDMap::const_iterator iter = m_idResetMap.find(__val);
        if (iter != m_idResetMap.end())
            __val = iter->second;
        return false;
    }

    virtual void BeginSerialize(CTuiObject* p) tm_override;

    virtual TMBool ExecID(CStrView pName, TMUInt32& __val) tm_override
    {
        return ResetPropID(__val);
    }

    virtual TMBool ExecDlg(CStrView pName, TMUInt32& __val) tm_override
    {
        return ResetPropID(__val);
    }

    virtual TMBool ExecWin(CStrView pName, TMUInt32& __val) tm_override
    {
        return ResetPropID(__val);
    }
};


// pre-search logic:
// 1. get all dlg, frame, pic object id;
// 2. get all invisible's widget object id;
// 3. if some object use object2, delete object2's id; delete object2's dlg id;
class CIDPreSearchStream : public CIdStream
{
public:
    typedef CSet<TMUInt32> CIDList;
    typedef CMap<TMUInt32, CIDList> CParentList; // if win id used, remove it all parent id.
    CTuiObject* m_pCur;
    CIDList m_aCID; // oid+cid;
    CParentList m_aParent;

    virtual void BeginSerialize (CTuiObject* p) { m_pCur = p; }
    virtual TMBool ExecID       (CStrView pName, TMUInt32& __val) tm_override { return false; }
    virtual TMBool ExecDlg      (CStrView pName, TMUInt32& __val) tm_override { m_aCID.erase(__val); return false; }
    virtual TMBool ExecWin      (CStrView pName, TMUInt32& __val) tm_override;
    virtual TMBool ExecPic      (CStrView pName, struct CPicPtr& __val) tm_override;
    virtual TMBool ExecFrame    (CStrView pName, CFrmPtr& __val) tm_override;

    void Insert(TMUInt nid)
    {
        if (nid > IDP_PRE_OBJECT_MAX)
            m_aCID.insert(nid);
    }

    void Get(CPanelApp* pApp);
};


TM_END_NAMESPACE
