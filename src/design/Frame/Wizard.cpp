//////////////////////////////////////////////////////////////////////////
///objectIdDlg

#include "StdAfx.h"
#include "Wizard.h"
// #include "resource.h"
// #include "ResourceID.h"
#include "MainWnd.h"
#include "cfg.h"
// #include <atldlgs.h>

using namespace TM;

TM::CString MacroNameToName(const TM::CString& strMacro, TM::CString& pre)
{
    TM::CString strRet;

    size_t n = strMacro.find('_');
    if (n == strMacro.npos)
        return strRet;

    n = strMacro.find('_', n + 1);
    if (n == strMacro.npos)
        return strRet;

    pre.assign(strMacro, 0, n + 1);

    int nLenght = strMacro.GetLength();
    strRet.SetLength(nLenght);
    int i = 0;
    for (; n < nLenght; ++n, ++i)
    {
        TMCharT c = strMacro[(int)n];
        if (L'_' == c)
        {
            ++n;
            strRet[i] = strMacro[(int)n];
        }
        else
            strRet[i] = _istupper(c) ? c - ('A' - 'a') : c;
    }
    strRet.SetLength(i);
    return strRet;
}

CWizardDlg ::CWizardDlg(CObjItem ti)
    : m_ti(ti)
{

}

CWizardDlg::~CWizardDlg()
{

}

TMResult CWizardDlg::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_strPre = "IDD_DLG_";
    TM::CString str;
    m_ti.GetText(str);
    Item(IDE_MARCONAME).SetWindowText(str);

    TM::CFileName fn(m_pMainWnd->GetRCName());
    TM::CString strPrj(fn.GetBody());
    Item(IDE_PRJNAME).SetWindowText(strPrj);

    return 0;
}

TMResult CWizardDlg::OnOk(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CString strMacro;
    Item(IDE_MARCONAME).GetWindowText(strMacro);
    TM::CString strti;
    m_ti.GetText(strti);
    strMacro = strMacro.Trim();
    if (strMacro.IsEmpty() ||
        strti == strMacro ||
        strMacro.FindOneOf(L" ;,./~!@#$%^&*()+|{}[]`-=\\:\"\'<>?") >= 0)
        return 0;

    TM::CString strMsg = m_pMainWnd->GetProj()->SetObjectIDMacro(strti, strMacro);
    if (strMsg.IsEmpty())
    {
        m_ti.SetText(strMacro);
        EndDialog(0);
    }
    else
        MessageBox(strMsg, _T("error"), MB_OK);
    return 0;
}

TMResult CWizardDlg::OnClose(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//     if (wID == IDOK)
//         Item(IDE_MARCONAME).GetWindowText(m_strObjectID);
    EndDialog(wID);
    return 0;
}

TMResult CWizardDlg::OnChange(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CString strMacro;
    CTuiEdit wndEdit(Item(IDE_MARCONAME));
     wndEdit.GetWindowText(strMacro);

    m_strCDlg = TM_T("CDlg");
    m_strDlg = TM_T("Dlg");
    if (m_ti.GetLevel() > 2)
    {
        m_strCDlg = TM_T("CWgt");
        m_strDlg = TM_T("Wgt");
    }

    m_strName = MacroNameToName(strMacro, m_strPre);
    Item(IDE_CLASSNAME).SetWindowText(m_strCDlg + m_strName);
    Item(IDE_FILENAME).SetWindowText(m_strDlg + m_strName + ".h(cpp)");

    TMColor clr = strMacro == TM_T(TM_STRING_OF(IDP_PRE_BASED_OBJECT)) ? TMRGB(255,0,0) : TMRGB(0,0,0);
    wndEdit.SetTextColor(e_bsNormal, clr);
    wndEdit.SetTextColor(e_bsFocus, clr);
    
    return 0;
}

struct CReplace
{
    TM::CStringA strClass;
    TM::CStringA strUserName;
    TM::CStringA strMacroH;
    TM::CStringA strID;
    TM::CStringA strNameH;
    TM::CStringA strNameC;
    TM::CStringA strFuncName;
    TM::CStringA strPrj;
    TM::CStringA strPrjUpper;
    
    TM::CStringA strData;
    char szYear[32];

    CReplace(CWizardDlg& dlg, const TM::CStringA& _strID, const TM::CStringA& _prj)
    {
        strPrj = _prj;
        strPrjUpper = _prj;
        strPrjUpper.MakeUpper();

        strFuncName = dlg.m_strName;
        strClass = dlg.m_strCDlg + dlg.m_strName;
        strUserName = dlg.m_pMainWnd->GetUser();
        if (strUserName.IsEmpty())
            strUserName = TM_T("YourUserName <name@Email.com>\r\n */#error please add user name and Email first in h/cpp file, you can add user on TMDui editor./*");
        strID = _strID;

        strNameH = dlg.m_strDlg + dlg.m_strName + TM_T(".h");
        strNameC = dlg.m_strDlg + dlg.m_strName + TM_T(".cpp");

        strMacroH = dlg.m_strDlg + dlg.m_strName + "_H";
        strMacroH.MakeUpper();
        
        TM::CTime tm = TM::CTime::GetCurrentTime();
        strData.Format("%d-%d-%d", tm.GetYear(), tm.GetMonth(), tm.GetDay());
        tm.Format(szYear, 31, "%Y");
    }

    TM::CStringA LoadFile(TMCStr name)
    {
        TM::CStringA str;

        TM::CFile f;
        TM::CString strPath(GetModulePath());
        f.OpenExisting(strPath + TM_T("\\") + name, f.e_faRead);
        if (f)
        {
            int n = f.GetSize();
            f.Read(str.GetBuffer(n), n);
        }
        else
        {
            TM::CResource res;
            res.Load(_Module.GetModuleInstance(), name, RT_HTML);
            str.Assign((TMCStrA)res.GetData(), res.GetSize());
        }
        
        
        str.Replace("WIZARD_FILEMACRO_H", strMacroH);
//         str.Replace("WIZARD_FUNCNAME", strFuncName);
        str.Replace("WIZARD_CLASSNAME", strClass);
        str.Replace("WIZARD_MACRONAME", strID);
        str.Replace("WIZARD_USERNAME", strUserName);
        str.Replace("WIZARD_DATA", strData);
        str.Replace("WIZARD_YEAR", szYear);
        str.Replace("WIZARD_FILE_H", strNameH);
        str.Replace("WIZARD_RES_XML_NAME", TM_T2A(TUI_RES_XML_NAME));
        str.Replace("WIZARD_RES_XML_FILE", TM_T2A(TUI_RES_XML_FILE));
        str.Replace("WIZARD_RES_XML_HFILE", TM_T2A(TM::CStrView(TUI_RES_XML_FILE, tm_countof(TUI_RES_XML_FILE) - 5))); // aaa.xml->aaa.h

        str.Replace("WIZARD_PROJECTNAME", strPrj);
        str.Replace("WIZARD_PROJECTUPPERNAME", strPrjUpper);
        
        return str;
    }

//     TM::CStringA LoadPrjFile(TMCStr name, const TM::CStringA& strPrj)
//     {
//         TM::CStringA str(LoadFile(name));
//         str.Replace("PROJECTNAME", strPrj);
//         TM::CStringA strPrjUpper(strPrj);
//         strPrjUpper.MakeUpper();
//         str.Replace("PROJECTUPPERNAME", strPrjUpper);
//         return str;
//     }

};


TMResult CWizardDlg::OnSaveCode(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CString strMacro;
    Item(IDE_MARCONAME).GetWindowText(strMacro);

    CReplace rp(*this, strMacro, L"");

    TM::CFile f1, f2;
    TM::CString strNameH(rp.strNameH), strNameC(rp.strNameC);
    f1.CreateNew(strNameH, f1.e_faWrite);
    f2.CreateNew(strNameC, f2.e_faWrite);
    if (!f1 || !f2)
    {
        if (IDOK == MessageBox(TM_T("the file have exist, overwrite?"), TM_T("error"), MB_OKCANCEL))
        {
            if (!f1)
                f1.CreateAlways(strNameH, f1.e_faWrite);
            if (!f2)
                f2.CreateAlways(strNameC, f2.e_faWrite);
        }
    }


    if (f2)
    {
        TM::CStringA str(rp.LoadFile(TM_T("Templet_Dlg.cpp")));
        f2.Write (str, str.GetLength());
    }

    if (f1)
    {
        TM::CStringA str(rp.LoadFile(TM_T("Templet_Dlg.h")));
        f1.Write (str, str.GetLength());
    }

    return 0;
}

struct CReplacePrj
{
    TM::CStringA strPrjC;
    TM::CStringA strPrjDsp;

    CReplacePrj(const TM::CStringA& prj, WORD wID)
    {
        strPrjC = prj + ".cpp";
        strPrjDsp = prj + ".dsp";
    }
};

TMResult CWizardDlg::OnPrjChange(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CString strPrj;
    Item(IDE_PRJNAME).GetWindowText(strPrj);
    TM::CString str;
    str.Format(TM_T("stdafx.h(cpp);%s.cpp;%s.dsw;%s.pro"), strPrj.c_str(), strPrj.c_str(), strPrj.c_str());
    Item(IDE_PRJSAVENAME).SetWindowText(str);
    return 0;
}

TMResult CWizardDlg::OnCreateProject(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CString strPrj;
    Item(IDE_PRJNAME).GetWindowText(strPrj);
    strPrj = strPrj.Trim();
    if (strPrj.IsEmpty())
    {
        MessageBox(TM_T("the project name is empty"), TM_T("error"), MB_OK);
        return 0;
    }

    TM::CFile fStdafxH, fStdafxC, fPrjC, fPrjDsp, fPrjPro, fGlobalH; // fPrjQrc,
    LPCTSTR pStdafxH        = TM_T("stdafx.h");
    LPCTSTR pStdafxC        = TM_T("stdafx.cpp");
    TM::CString fnPrjC      = strPrj + TM_T(".cpp");
    TM::CString fnOrjDsp    = strPrj + TM_T(".dsp");
    TM::CString fnOrjPro    = strPrj + TM_T(".pro");
    TM::CString fnOrjQrc    = strPrj + TM_T(".qrc");
    TM::CString fnGlobalH   = strPrj + TM_T("_global.h");
    fStdafxH.CreateNew(pStdafxH, TM::CFile::e_faWrite);
    fStdafxC.CreateNew(pStdafxC, TM::CFile::e_faWrite);
    fPrjC.CreateNew(fnPrjC, TM::CFile::e_faWrite);
    fPrjDsp.CreateNew(fnOrjDsp, TM::CFile::e_faWrite);
    fPrjPro.CreateNew(fnOrjPro, TM::CFile::e_faWrite);
//     fPrjQrc.CreateNew(fnOrjQrc, TM::CFile::e_faWrite);
    fGlobalH.CreateNew(fnGlobalH, TM::CFile::e_faWrite);

    if (!fStdafxH || !fStdafxC || !fPrjC || !fPrjDsp || !fPrjPro || !fGlobalH)// || !fPrjQrc)
    {
        if (IDOK == MessageBox(TM_T("the file have exist, overwrite?"), TM_T("error"), MB_OKCANCEL))
        {
            if (!fStdafxH)
                fStdafxH.CreateAlways(pStdafxH, TM::CFile::e_faWrite);
            if (!fStdafxC)
                fStdafxC.CreateAlways(pStdafxC, TM::CFile::e_faWrite);
            if (!fPrjC)
                fPrjC.CreateAlways(fnPrjC, TM::CFile::e_faWrite);
            if (!fPrjDsp)
                fPrjDsp.CreateAlways(fnOrjDsp, TM::CFile::e_faWrite);
            if (!fPrjPro)
                fPrjPro.CreateAlways(fnOrjPro, TM::CFile::e_faWrite);
//             if (!fPrjQrc)
//                 fPrjQrc.CreateAlways(fnOrjQrc, TM::CFile::e_faWrite);
            if (!fGlobalH)
                fGlobalH.CreateAlways(fnGlobalH, TM::CFile::e_faWrite);

        }
    }

    TM::CString strMacro;
    Item(IDE_MARCONAME).GetWindowText(strMacro);
    CReplace rp(*this, strMacro, strPrj);

    if (fStdafxH)
    {
        TM::CStringA str(rp.LoadFile(TM_T("Templet_StdAfx.h")));
        fStdafxH.Write (str, str.GetLength());
    }

    if (fStdafxC)
    {
        TM::CStringA str(rp.LoadFile(TM_T("Templet_StdAfx.cpp")));
        fStdafxC.Write (str, str.GetLength());
    }

    if (fPrjC)
    {
        TM::CStringA str(rp.LoadFile(wID == IDC_CREATEPRJDLL ? TM_T("Templet_prjdll.cpp") : TM_T("Templet_prjexe.cpp")));
        fPrjC.Write (str, str.GetLength());
    }

    if (fPrjDsp)
    {
        TM::CStringA str(rp.LoadFile(wID == IDC_CREATEPRJDLL ? TM_T("Templet_prjdll.dsp") : TM_T("Templet_prjexe.dsp")));
        fPrjDsp.Write (str, str.GetLength());
    }

    if (fPrjPro)
    {
        TM::CStringA str(rp.LoadFile(wID == IDC_CREATEPRJDLL ? TM_T("Templet_prjdll.pro") : TM_T("Templet_prjexe.pro")));
        fPrjPro.Write (str, str.GetLength());
    }

//     if (fPrjQrc)
//     {
//         TM::CStringA str(rp.LoadFile(TM_T("TEMPLET_RC.QRC")));
//         fPrjQrc.Write (str, str.GetLength());
//     }

    if (fGlobalH)
    {
        TM::CStringA str(rp.LoadFile(TM_T("TEMPLET_GLOBAL.H")));
        fGlobalH.Write (str, str.GetLength());
    }

    return 0;
}

// TMResult CWizardDlg::OnSetPre(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
//     Item(IDE_MARCONAME).SetWindowText(TM_T(TM_STRING_OF(IDP_PRE_BASED_OBJECT)));
//     return 0;
// }
