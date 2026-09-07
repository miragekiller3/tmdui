/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.01.04
 *
 *  Copyright (C) 2015 miragekiller
 */

#include "StdAfx.h"
#include "Panel.h"
#include "PropItem.h"
#include "WndPad.h"
#include "ResFile.h"


TM_BGN_NAMESPACE


#define PROP_TEXT_INDENT 2


////////////////////////////////////////////////////////////////////////////////
// CPropBoxBase
CPropBoxBase::CPropBoxBase ()
{
    m_txtName.Create();
    m_txtValue.Create();
}

// CHookKeyboardLLImpl
TMBool CPropBoxBase::OnKeyboardLL(UINT uMsg, KBDLLHOOKSTRUCT* ps)
{
    if (!GetProjMgr()->PropIsFocus())
        return false;
    
    if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)
    {
        DWORD vkCode = ps->vkCode;
        switch (vkCode)
        {
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
        case VK_LWIN:
        case VK_RWIN:
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_APPS:
            return true;
        }
        
        CString str;
        CKeyboardDebugInfo::VK2String(vkCode, str);
        if (!str.IsEmpty())
            GetProjMgr()->PropSetCurrentText(str);
        else
        {
            TCHAR c[2] = { vkCode, 0 };
            GetProjMgr()->PropSetCurrentText(c);
        }
    }
    return true;
}

// IPropBox
void CPropBoxBase::DrawName(const CRectF& rc, TMBool bSelected)
{
    CRectF rcItem = rc;
    rcItem.left += 2; // Indent text
    m_txtName.ReCreate(m_strName, false);
    m_txtName->SetFormat(DT_LEFT | DT_SINGLELINE | DT_EDITCONTROL | DT_NOPREFIX | DT_VCENTER);
    m_txtName->Paint(rcItem);
}

void CPropBoxBase::Draw2 (TMCStr pText, const CRectF& rc, TMBool bSelected)
{
    CRectF rcItem = rc;
    rcItem.left += 2; // Indent text
    m_txtValue.ReCreate(pText, false);
    m_txtValue->SetFormat(DT_LEFT | DT_SINGLELINE | DT_EDITCONTROL | DT_NOPREFIX | DT_VCENTER);
    m_txtValue->Paint(rcItem);
}

TM::CString CPropBoxBase::ID2Text(TMUInt32 dwID)
{
    return m_pApp->m_idMgr.ID2Text(dwID);
}

TMUInt32 CPropBoxBase::Text2ID(const TM::CString& str, EDIT_MODE eAutoCreateFlag)
{
    return m_pApp->m_idMgr.Text2ID(str, eAutoCreateFlag);
}

void CPropBoxBase::InitPop(EDIT_MODE nType)
{
    CPopIDList aid;
    m_pApp->m_idMgr.GetIDs(aid, nType);
    m_pApp->GetProjMgr()->PropInitPopupID(aid);
}

CStringW CPropBoxBase::BeginProp(IObjEdit* p)
{
    return m_pApp->GetProjMgr()->PropBegin(p);
}

void CPropBoxBase::EndProp(IObjEdit* p, const CStringW& key)
{
    GetProjMgr()->PropEnd(p, key);
}

IProjMgr* CPropBoxBase::GetProjMgr()
{
    return m_pApp->GetProjMgr();
}

////////////////////////////////////////////////////////////////////////////////
// CPropBox


CModule& CPropBox2::GetModule() const { return *m_pApp->GetEditorModule(); }


TM_END_NAMESPACE

