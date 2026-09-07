#ifndef _DEMODLG_H
#define _DEMODLG_H

#include "tuires.h"
#include "resource.h"
#include "tester.h"
#include "../Design.h"

class CDlgBase
//     : public TM::CDPIAwareImplT
{
public:
    typedef CDlgBase dbase;

    BEGIN_MSG_MAP(CDlgBase)
//         COMMAND_ID_HANDLER  (IDC_ZOOMIN, OnZoomIn);
//         COMMAND_ID_HANDLER  (IDC_ZOOMOUT, OnZoomOut);
        COMMAND_ID_HANDLER  (IDC_SYSFRAME, OnSysFrame);
        COMMAND_ID_HANDLER  (IDC_SELFFRAME, OnSelfFrame);
//         CHAIN_MSG_MAP       (CDPIAwareImpl)
    END_MSG_MAP()


//     TMResult OnZoomIn(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//     {
//         int zm = _tuiApp.GetZoom();
//         _tuiApp.SetZoom(zm - 10);
//         return 0;
//     }
// 
//     TMResult OnZoomOut(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//     {
//         int zm = _tuiApp.GetZoom();
//         _tuiApp.SetZoom(zm + 10);
//         return 0;
//     }

    TMResult OnSysFrame(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        AppGetTester()->SetUseSystemFrame(true);
        return 0;
    }

    TMResult OnSelfFrame(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        AppGetTester()->SetUseSystemFrame(false);
        return 0;
    }

};

class CTestXMLDlg
    : public TM::CTuiDialogImpl<CTestXMLDlg>
    , public CDlgBase
{
public:
    static int IDD;
    HWND m_hParent;

    CTestXMLDlg() : m_hParent(0) {};
    ~CTestXMLDlg(){}

public:
    BEGIN_MSG_MAP(CTestXMLDlg)
        MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog);
        COMMAND_ID_HANDLER (IDCANCEL, OnCancel);

        MESSAGE_HANDLER (WM_DESTROY, OnDestroy);
        MESSAGE_HANDLER (WM_NCDESTROY, OnDestroy);

        ROUTE_HANDLER (WM_LBUTTONDOWN, OnHook)
        ROUTE_RANGE_HANDLER (1, 4, OnHook)
        ROUTE_RANGE_ID_HANDLER (1, 4, 0, OnHook)
        ROUTE_ID_HANDLER (0, WM_LBUTTONDOWN, OnHook)
        ROUTE_ID_RANGE_HANDLER (0, 5, WM_LBUTTONDOWN, OnHook)
        ROUTE_TAG_HANDLER (0, WM_LBUTTONDOWN, OnHook)
        ROUTE_TAG_RANGE_HANDLER (0, 5, WM_LBUTTONDOWN, OnHook)
        CHAIN_MSG_MAP(dbase)
        CHAIN_MSG_MAP(root_class)
    END_MSG_MAP()

    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        bHandled = false;
//        ShowSysFrame();
//        ShowSysFrame();
        return 0;
    }


    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
        {
            EndDialog(wID);
            return 0;
        }

    TMResult OnHook(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
        {
            bHandled = false;
            return 0;
        }
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
};    

class CDemoDlg
    : public CDialogImpl < CDemoDlg >
    , public CDlgBase
{
public:
    CTestXMLDlg m_dlgC;

public:
    enum { IDD = IDD_DIALOG2 };

    BEGIN_MSG_MAP (CDemoDlg)
        MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog);
        COMMAND_ID_HANDLER (IDCANCEL, OnCancel);
        CHAIN_MSG_MAP(dbase)
    END_MSG_MAP()

    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    int DoModal(HWND hWndParent = ::GetActiveWindow(), LPARAM dwInitParam = NULL)
    {
        ATLASSERT(m_hWnd == NULL);
        _Module.AddCreateWndData(&m_thunk.cd, (CDialogImplBaseT< CWindow >*)this);
#ifdef _DEBUG
        m_bModal = true;
#endif //_DEBUG
        return ::DialogBoxParam(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDD),
            hWndParent, (DLGPROC)StartDialogProc, dwInitParam);
    }
};




#endif