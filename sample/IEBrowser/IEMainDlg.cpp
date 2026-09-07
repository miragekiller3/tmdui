#include "stdafx.h"
#include "IEMainDlg.h"

using namespace TM;

void CIEMainDlg::DLControl (long *pFalg)
{
    Log(_T("DLControl"));    
}
    
void CIEMainDlg::BeforeNavigate2 (
    IDispatch *pDisp, VARIANT *URL, VARIANT *Flags,
    VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers,
    VARIANT_BOOL *Cancel)
{
    Log(_T("BeforeNavigate2"));
}


void CIEMainDlg::ClientToHostWindow (long *CX, long *CY)
{
    Log(_T("ClientToHostWindow"));    
}

void CIEMainDlg::CommandStateChange (long Command, VARIANT_BOOL Enable)
{
    Log(_T("CommandStateChange(%d,%d)"), Command, Enable);
}

void CIEMainDlg::DocumentComplete (IDispatch *pDisp, VARIANT *URL)
{
    Log(_T("DocumentComplete"));
}

void CIEMainDlg::DownloadBegin (VOID)
{
    Log(_T("DownloadBegin"));
}

void CIEMainDlg::DownloadComplete (VOID)
{
    Log(_T("DownloadComplete"));    
}

void CIEMainDlg::FileDownload (VARIANT_BOOL *Cancel)
{
    Log(_T("FileDownload"));    
}

void CIEMainDlg::NavigateComplete2 (IDispatch *pDisp, VARIANT *URL)
{
    Log(_T("NavigateComplete2"));    
}

void CIEMainDlg::NavigateError (
    IDispatch *pDisp, VARIANT *URL, VARIANT *TargetFrameName,
    VARIANT *StatusCode, VARIANT_BOOL *Cancel)
{
    Log(_T("NavigateError"));    
}

void CIEMainDlg::NewWindow2 (IDispatch **ppDisp, VARIANT_BOOL *Cancel)
{
    Log(_T("NewWindow2"));    
}

void CIEMainDlg::OnFullScreen (VARIANT_BOOL FullScreen)
{
    Log(_T("OnFullScreen"));    
}

void CIEMainDlg::OnMenuBar (VARIANT_BOOL MenuBar)
{
    Log(_T("OnMenuBar"));
}

void CIEMainDlg::OnQuit(VOID)
{
    Log(_T("OnQuit"));
}

void CIEMainDlg::OnStatusBar (VARIANT_BOOL StatusBar)
{
    Log(_T("OnStatusBar"));
}

void CIEMainDlg::OnTheaterMode (VARIANT_BOOL TheaterMode)
{
    Log(_T("OnTheaterMode"));
}

void CIEMainDlg::OnToolBar (VARIANT_BOOL ToolBar)
{
    Log(_T("OnToolBar"));
}

void CIEMainDlg::OnVisible (VARIANT_BOOL Visible)
{
    Log(_T("OnVisible"));
}

void CIEMainDlg::ProgressChange (long Progress, long ProgressMax)
{
    Log(_T("ProgressChange"));
}

void CIEMainDlg::PropertyChange (BSTR szProperty)
{
    Log(_T("PropertyChange:%s"), szProperty);
}

void CIEMainDlg::StatusTextChange (BSTR Text)
{
    Log(_T("StatusTextChange:%s"), Text);
}

void CIEMainDlg::TitleChange (BSTR Text)
{
    Log(_T("TitleChange:%s"), Text);
}

void CIEMainDlg::WindowClosing (VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel)
{
    Log(_T("WindowClosing"));
}

void CIEMainDlg::WindowSetHeight (long Height)
{
    Log(_T("WindowSetHeight"));
}

void CIEMainDlg::WindowSetLeft (long Left)
{
    Log(_T("WindowSetLeft"));
}

void CIEMainDlg::WindowSetResizable (VARIANT_BOOL Resizable)
{
    Log(_T("WindowSetResizable"));
}

void CIEMainDlg::WindowSetTop (long Top)
{
    Log(_T("WindowSetTop"));
}

void CIEMainDlg::WindowSetWidth (long Width)
{
    Log(_T("WindowSetWidth"));
}


TMResult CIEMainDlg::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_listBox = Item(IDC_LIST);

    OnRecreate(0,0,0,bHandled);
//    if (!m_pEventAdvise)
//        OnRecreate(0,0,0,bHandled);

//    m_wb.ShowWindow(0);
//    CTuiWidget(m_wb.GetParent()).ShowWindow(0);

    return 0;
}

TMResult CIEMainDlg::OnRecreate(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    Log(_T("Recreate"));
    
    if (m_pEventAdvise)
        DispEventUnadvise(m_pEventAdvise); 
    m_pEventAdvise.Release();
    CTuiWidget wndParent(Item(IDC_WINDOW_21028));
    CTuiButton but(Item(IDC_PUSHBUTTON_RECREATE));

    int nCheck = but.GetCheck();
    but.SetCheck(!nCheck);
    if (m_IEWnd)
        m_IEWnd.DestroyWindow();
    m_IEWnd.Create(wndParent, nCheck ? IDC_WEBVIEW_21029 : IDO_TUI_ACTIVEX_10018);

    m_wb = m_IEWnd;

    m_pEventAdvise = m_IEWnd.GetControl();
    if (m_pEventAdvise)
         DispEventAdvise(m_pEventAdvise); 
    return 0;
}



TMResult CIEMainDlg::OnOk(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TCHAR c[512];
    GetDlgItemText(IDW_EDIT_20017, c, 511);
    Log(c);

//     CComPtr<IOleControl> pole;
//     m_IEWnd.QueryControl(&pole);
//     pole->OnAmbientPropertyChange(DISPID_AMBIENT_DLCONTROL);

//     s_bTestDestroy = 501;

    m_IEWnd.Navigate(c);
    return 0;
}



