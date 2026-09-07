#include "StdAfx.h"
#include "Panel.h"
//#include "editstream.h"
#include "FactoryData.h"
#include "WndPad.h"



TM_BGN_NAMESPACE

//////////////////////////////////////////////////////////////////////////////
// CEditStream
CPanelApp* CEditStream::_GetApp()
{
    return TM_OTHIS(CPanelApp, m_editStream);
}

void CEditStream::_AddProp(const CWeakStr& pName, IPropBox* pBox)
{
    _GetApp()->GetProjMgr()->PropAdd(pName, pBox);
}

CEditStream::~CEditStream()
{
    for (; m_aProp.size();)
    {
        CPropList::iterator iter = m_aProp.begin();
        iter->second->Release();
        m_aProp.erase(iter);
    }
}

void CEditStream::SetCurrentObj(IObjEdit* p, TMBool bAddMode)
{
    m_aString.clear();
    m_pCurrent = p;
    if (bAddMode)
        return;
    
    for (CPropRec rec(m_aProp); rec; ++rec)
        rec.get_data()->RemoveOwner(p);
}


TM_END_NAMESPACE


void CObjData::EditProp(TMBool bAddMode)
{
    TM::CTuiObject* pObj = GetProp();
    DbgAssert(pObj->IC_IsValidInstance(true));
    DbgTrace("EditProp:%d(%x), %d",pObj->GetObjectID(), pObj, bAddMode);
    CPanelApp* app = GetApp();
    TM::CEditStream& es = app->m_editStream;
    //es.SetAddMode(bAddMode ? this : 0);
    //pObj->EditProp(es);
    es.SetCurrentObj(this, bAddMode);
    if (bAddMode)
        pObj->EditProp(es);
//     Sleep(30);
}


