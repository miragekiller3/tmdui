#include "StdAfx.h"
#include "IdStream.h"
#include "WndPad.h"
#include "ResourceID.h"

//////////////////////////////////////////////////////////////////////////
//
TM_BGN_NAMESPACE

CIdResetStream::CIdResetStream(const CResetIDMap& aID)
    : m_idResetMap(aID)
{

}

CIdResetStream::~CIdResetStream()
{

}

void CIdResetStream::BeginSerialize(CTuiObject* p)
{
    CModule* pm = p->GetModule();
    pm->EraseObj(p);
    ResetPropID(p->m_dwObjID);
    pm->m_aObjs.insert(p);
}

TMBool CIDPreSearchStream::ExecWin(CStrView pName, TMUInt32& __val)
{
    m_aCID.erase(__val);
    if (__val < IDP_PRE_OBJECT_MAX)
        return false;

    // skip same parent object
    if (m_pCur->GetType() == e_otWgt)
    {
        TMUInt32 nOwnerID = static_cast<HTUI>(m_pCur)->GetOwner()->GetObjectID();
        tm_for (CIDList::recorder rec(m_aParent[__val]); rec; ++rec)
        {
            if (nOwnerID == rec.get_data())
                return false;
        }
    }

    tm_for (CIDList::recorder rec(m_aParent[__val]); rec; ++rec)
        m_aCID.erase(rec.get_data());
    return false;
}

TMBool CIDPreSearchStream::ExecPic(CStrView pName, struct CPicPtr& __val)
{
    m_aCID.erase(__val.GetObjectID());
    return false;
}

TMBool CIDPreSearchStream::ExecFrame(CStrView pName, CFrmPtr& __val)
{
    if (__val)
        m_aCID.erase(__val->GetObjectID());
    return false;
}

void CIDPreSearchStream::Get(CPanelApp* pApp)
{
    TM::CModule* pm = pApp->GetEditorModule();
    
    // do pre-search logic 1, 2
    tm_for (CModule::CObjList::recorder rec(pm->m_aObjs); rec; ++rec)
    {
        TM::CTuiObject* p = static_cast<TM::CTuiObject*>(rec.get_data());
        TMUInt32 oid = p->GetObjectID();
        if (oid < IDP_PRE_OBJECT_MAX)
            continue;
        m_aParent[oid];
        switch (p->GetType())
        {
        case e_otPic:
        case e_otFrm:
            m_aCID.insert(oid);
            break;
            
        case e_otWnd:
            m_aCID.insert(oid);
            Insert(static_cast<CTuiWgt*>(p)->GetDlgCtrlID());
            break;
            
        case e_otWgt:
            {
                HTUI hWin = static_cast<CTuiWgt*>(p);
                if (!hWin->Is_WS_VISIBLE())
                    m_aCID.insert(oid);

                for (HTUI hp = hWin; hp = hp->GetParent();)
                {
                    m_aParent[oid].insert(hp->GetObjectID());
                    if (hp->GetDlgCtrlID() > IDP_PRE_OBJECT_MAX)
                        m_aParent[oid].insert(hp->GetDlgCtrlID());
                }
                
                Insert(hWin->GetDlgCtrlID());
            }
            break;
        }
    }
    
    // do pre-search logic 3
    tm_for (CModule::CObjList::recorder rec(pm->m_aObjs); rec; ++rec)
    {
        TM::CTuiObject* p = static_cast<TM::CTuiObject*>(rec.get_data());
        TMUInt32 oid = p->GetObjectID();
        if (oid < IDP_PRE_OBJECT_MAX)
            continue;
        p->ResetIDProp(*this);
    }
}

TM_END_NAMESPACE


void CPanelApp::ResetUserIds(const TM::CStrView& strUser, int idStart)
{
    CResetIDMap aid;
    m_idMgr.ResetUserIds(aid, idStart, strUser);

    TM::CIdResetStream stm(aid);
    TM::CModule::CObjList aObjs(GetEditorModule()->m_aObjs);
    for (TM::CModule::CObjList::recorder rec(aObjs); rec; ++rec)
    {
        TM::CTuiObject* p = static_cast<TM::CTuiObject*>(rec.get_data());
        if (p->GetObjectID() != 0) // m_dwObjID==0 is temp object for editer
            p->ResetIDProp(stm);
    }
}

int CPanelApp::GetUsers(CPopIDList& aMacro)
{
    m_idMgr.GetUsers(aMacro);
    return m_idMgr.GetUserStepID();
}

void CPanelApp::MergeUsers(CPopIDList& aMacro)
{
    m_idMgr.MergeUsers(aMacro);
}

TM::CString CPanelApp::GetPreSearchID(CPreSearchID& aList)
{
    TM::CIDPreSearchStream stream;
    stream.Get(this);

    TM::CStrPtr str;
    tm_for (TM::CIDPreSearchStream::CParentList::recorder rec(stream.m_aParent); rec; ++rec)
    {
        // make str
        TMUInt32 dwID = rec.get_key();
        TM::CString strM(m_idMgr.ID2Text(dwID));
        str += strM;
        str += L"\r\n";

        // make aList.aDel
        if (stream.m_aCID.find(dwID) == stream.m_aCID.end())
            aList.aDel.insert(strM);

        // make aList.aID
        tm_for (TM::CIDPreSearchStream::CIDList::recorder rec2(rec.get_data()); rec2; ++rec2)
        {
            TMUInt32 dwIDP = rec2.get_data();
            TM::CString strP(m_idMgr.ID2Text(dwIDP));
            aList.aID[strM].insert(strP);
        }
    }

    return std::move(str);
}



