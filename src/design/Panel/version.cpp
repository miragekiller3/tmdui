/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.04.03
 *
 *  Copyright (C) 2010 miragekiller
 */

#include "StdAfx.h"
#include "Panel.h"
#include "WndPad.h"


int s_nCurrentVersion = TUI_FILE_VER_CURRENT;


TM_BGN_NAMESPACE

static void _Edit_ConvertXmlVersion(IDomNode* _xml, TMLParam lp)
{
    CDomElementPtr xml(_xml);
    if (!xml)
        return;

    IProjMgr* pLog = (IProjMgr*)lp;
}

void Edit_ConvertXmlVersion(CModule& mdl, CStringW& strXml)
{
    if (strXml.IsEmpty())
        return;

    IProjMgr* pLog = mdl.m_hRes ? 0 : (static_cast<CPanelApp*>(mdl.m_pOwner))->GetProjMgr();

    // get version
    {
        TM::CXmlTag xml(strXml);
        TM::CXmlTag tag;
        if (XmlGetProperty(xml, TUI_XML_PROP_VERSIOM, tag))
            tag.ScanLong(s_nCurrentVersion);

        if (s_nCurrentVersion < TUI_FILE_VER_210202)
        {
            DbgAssert("not support version before 210202");
            return;
        }

        if (s_nCurrentVersion > TUI_FILE_VER_CURRENT && pLog)
        {
            pLog->Alert(
                L"The editor version is %d, but the file version is %d.\r\nyou can't save file before upgrade your editor\r\n",
                TUI_FILE_VER_CURRENT, s_nCurrentVersion);
        }
        else if (s_nCurrentVersion < TUI_FILE_VER_CURRENT && pLog)
        {
            pLog->Alert(
                L"Upgrade XML file version from %d to %d\r\n",
                s_nCurrentVersion, TUI_FILE_VER_CURRENT);
        }
    }

    if (s_nCurrentVersion <= TUI_FILE_VER_CURRENT)
    {
        CDomDocument node;
        node->LoadXml(TM::CXmlTag(strXml));
        DomEnum(node, _Edit_ConvertXmlVersion, (TMLParam)pLog);
        strXml.clear();
        node->transformNode(strXml);
        CHECKMEMORY();
    }
}


TM_END_NAMESPACE

TMUInt32 IProj::GetFileVersion()
{
    return s_nCurrentVersion;
}

