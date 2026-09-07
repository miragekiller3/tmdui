#include "StdAfx.h"
#include "FileOpt.h"
#include "ResFile.h"
#include "ResourceID.h"
#include "WndPad.h"
//#include "FactoryData.h"
//#include "version.h"


using namespace TM;



CString CPanelApp::LoadFromFile(LPCTSTR pFileName, TMBool bMerge)
{
    TM::CFileName strFullName(pFileName);
    TM::CString str(strFullName.GetPath(true));
    TM::CFileName fnXml;
    TMCStr ext = strFullName.GetExt(false);

    if (!bMerge)
    {
        m_idMgr.Reset();
        m_fileRC.LoadDftResFile(m_idMgr, m_hResWin);
        m_fileQRC.LoadDftResFile(m_idMgr);
        m_strXmlFileName = TM_CS(TUI_RES_XML_FILE);
    }

    if (0 == StrCompareI(ext, TM_T("rc")))
    {
        // load windows h file
        if (bMerge)
            return CString();

        m_idMgr.LoadHFile(str + _T("resource.h"), TM::EM_RC, GetProjMgr()); // load windows h file first
        
        // load rc file
        m_fileRC.LoadResFile(m_idMgr, strFullName, m_strXmlFileName, GetProjMgr());

        // load rc h file
        fnXml = str + m_strXmlFileName;//m_fileRC.GetXmlFileName();
        TM::CFileName fnH(fnXml.GetPathBody(false));
        fnH += _T(".h");
        m_idMgr.LoadHFile(fnH, TM::EM_CONTROL, GetProjMgr());
        
        // load xml
        this->LoadXmlFile(fnXml, bMerge);
    }
    else if (0 == StrCompareI(ext, TM_T("qrc")))
    {
        // load windows h file
        if (bMerge)
            return CString();
        
        // load rc file
        m_fileQRC.LoadResFile(m_idMgr, strFullName, m_strXmlFileName);
        
        // load rc h file
        fnXml = str + m_strXmlFileName;//m_fileQRC.GetXmlFileName();
        TM::CFileName fnH(fnXml.GetPathBody(false));
        fnH += _T(".h");
        m_idMgr.LoadHFile(fnH, TM::EM_CONTROL, GetProjMgr());
        
        // load xml
        this->LoadXmlFile(fnXml, bMerge);
    }
    else // load xml file
    {
        fnXml = strFullName;
        
        // load h file
        TM::CFileName fnH(fnXml.GetPathBody(false));
        fnH += _T(".h");
        m_idMgr.LoadHFile(fnH, TM::EM_CONTROL, GetProjMgr());

        this->LoadXmlFile(fnXml, bMerge);
    }

    return fnXml;
}






