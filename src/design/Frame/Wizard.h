#ifndef WIZARD_H 
#define WIZARD_H

#include "ObjectTreeView.h"

TM::CString MacroNameToName(const TM::CString& strMacro);

class CMainWnd;

class CWizardDlg : public TM::CTuiDialogImpl<CWizardDlg>
{
public:
    enum{ IDD = IDD_TUI_WIZARD };

public:
    TM::CString        m_strName;
    TM::CString        m_strPre; // "IDD_DLG_"
    TM::CString     m_strCDlg; // "CDlg"
    TM::CString     m_strDlg; // "Dlg"
    CObjItem        m_ti;
    CMainWnd*       m_pMainWnd;

public:
    CWizardDlg(CObjItem ti);
    ~CWizardDlg();

    BEGIN_MSG_MAP(CWizardDlg)
        MESSAGE_HANDLER        (WM_INITDIALOG,                OnInitDialog);
        COMMAND_ID_HANDLER    (IDOK,                        OnOk)
        COMMAND_ID_HANDLER    (IDCANCEL,                    OnClose)
        COMMAND_ID_HANDLER    (IDC_SAVECODE,                OnSaveCode)
        COMMAND_ID_HANDLER    (IDC_CREATEPRJDLL,            OnCreateProject)
        COMMAND_ID_HANDLER    (IDC_CREATEPRJEXE,            OnCreateProject)
//         COMMAND_ID_HANDLER    (IDC_SETPRE,                OnSetPre)
        COMMAND_HANDLER        (IDE_MARCONAME, EN_CHANGE,    OnChange);
        COMMAND_HANDLER        (IDE_PRJNAME, EN_CHANGE,    OnPrjChange);
    END_MSG_MAP()

    TMResult OnOk(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnClose(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnChange(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSaveCode(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnPrjChange(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCreateProject(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnSetPre(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
};

#endif