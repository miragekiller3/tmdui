/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.14
 *
 *  Copyright (C) 2006 miragekiller
 */

#include "../tmlist.h"
#include "../tmgdix.h"

#include "object.h"
#include "loader.h"
#include "module.h"



TM_BGN_NAMESPACE


CTuiObject::CTuiObject()
    : m_dwTag(0)
    , m_nRef(1)
    , m_pFactoryInfo(0)
{
#ifdef TUI_EDIT_MODE
    m_pEditData = 0;
#endif
}

CTuiObject::~CTuiObject()
{
}

CModule* CTuiObject::GetModule() const
{
    DbgAssert(m_pFactoryInfo);
    return m_pFactoryInfo->m_pModule;
}

CView* CTuiObject::GetView() const
{
    return GetModule()->m_pOwner;
}


CStrPtr CTuiObject::GetClassName()
{
    DbgAssert(m_pFactoryInfo);
    return CStrPtr(m_pFactoryInfo->m_pFactory->strClass);
}

EObjType CTuiObject::GetType() const
{
    DbgAssert(m_pFactoryInfo);
    return m_pFactoryInfo->m_pFactory->eType;
}

TM_REFAPI CTuiObject::AddRef()
{
//     if (m_dwObjID == 1004)
//     {
//         int x = 0;
//     }
    return AtomicInc(&m_nRef);
}

static void _CTuiObject_Destroy(void* lp)
{
    CTuiObject* p = (CTuiObject*)lp;
    CFactoryInfo* pf = p->m_pFactoryInfo;
    pf->m_pModule->EraseObj(p);
    DbgTrace(TM_T("object destroy:%x(objid:%d; class:%s)"), p, p->m_dwObjID, pf->m_pFactory->strClass.c_str());
    pf->m_pFactory->FDestroy(p);
    pf->m_pModule->Release();
}

// the function maybe call in other thread
TM_REFAPI CTuiObject::Release()
{
    int n = AtomicDec(&m_nRef);

    // system predefined object: m_pFactoryInfo == 0 && m_pModule == 0;
    if (n <= 0 && m_pFactoryInfo)
    {
        DbgAssert(IC_IsCreatorThread());
        _CTuiObject_Destroy(this);
    }
    
    return n;
}

void CTuiObject::LoadAll(IDomElement* xml, TMUInt32 dwFlag)
{
    CLoadStream wx(*GetModule(), xml, dwFlag, m_nDPI);//, m_pModule->GetSkinXml(m_dwObjID));
//    wx.ReadFromFile (xml, m_pModule->GetSkinXml(m_dwObjID));
    m_pFactoryInfo->m_pFactory->FLoadProp(this, wx);
}

TMHModule CTuiObject::GetModuleHandle() const
{
    return GetModule()->m_hRes;
}

TMBool CTuiObject::IsHighContrast() const
{
    return GetView()->m_bHighContrast;
}


#ifdef TUI_EDIT_MODE

CString CTuiObject::GetDebugInfo()
{
    const TMDbgClassInfo* di = TMDbgClassInfo_Check(this, false);
    CString str(_Debug_MakeDbgInfo(di));
    CString strTmp;
    strTmp.Format(L"\r\n  dpi:%d;", GetDPI());
    str += strTmp;
    return str;
}

#endif



TM_END_NAMESPACE
