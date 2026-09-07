#ifndef HELLO_WORLD_H
#define HELLO_WORLD_H 1

#include "tuires.h"

extern CComModule _Module;

class CWclDemoDlg : public TM::    CTuiDialogImpl<CWclDemoDlg>
{
public:
    enum { IDD = IDD_MANAGE_1000 };

    BEGIN_MSG_MAP           (CWclDemoDlg)
        MESSAGE_HANDLER     (WM_INITDIALOG,   OnInitDialog)
        MESSAGE_HANDLER     (WM_CLOSE,          OnClose)
        MESSAGE_HANDLER     (WM_LBUTTONDOWN,  OnLButtonDown)
        MESSAGE_HANDLER     (WM_LBUTTONUP,    OnLButtonUp)
        MESSAGE_HANDLER     (WM_MOUSEMOVE,    OnMouseMove)

        COMMAND_ID_HANDLER  (IDC_SKIN,  OnChangeSkin)
        COMMAND_ID_HANDLER  (IDC_LANG,  OnChangeLanguage)
        COMMAND_ID_HANDLER  (IDC_CLOSE, OnCloseDlg)
        CHAIN_MSG_MAP       (root_class)
    END_MSG_MAP()

protected:
    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_bLButtonDown = FALSE;
        CenterWindow();
        return 0;
    }

    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        PostQuitMessage(0);
        return 0;
    }

    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_bLButtonDown = TRUE;
        SetCapture();
        GetCursorPos(&m_ptMouse);
        bHandled = FALSE;
        return 0;
    }

    LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        ReleaseCapture();
        m_bLButtonDown = FALSE;
        bHandled = FALSE;
        return 0;
    }

    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (m_bLButtonDown)
        {
            POINT pt;
            GetCursorPos(&pt);

            int dx = pt.x - m_ptMouse.x;
            int dy = pt.y - m_ptMouse.y;

            RECT rc;
            GetWindowRect(&rc);
            MoveWindow(rc.left+dx, rc.top+dy, rc.right-rc.left, rc.bottom-rc.top);

            m_ptMouse.x = pt.x;
            m_ptMouse.y = pt.y;
        }

        bHandled = FALSE;
        return 0;
    }

    TMResult OnChangeSkin(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
//         ::ShowWindow(GetHWND(), false);
//         m_tuiWindow.MoveWindow(0,0,100,100);
//         return 0;

        static BOOL bSkinDefault = TRUE;
        if (bSkinDefault)
            _tuiApp.LoadXml(_Module.m_hInst, 0, L"Skin1.xml");
        else
            _tuiApp.LoadXml(_Module.m_hInst, 0, L"Skin2.xml");

        bSkinDefault = !bSkinDefault;
        return 0;
    }

    TMResult OnChangeLanguage(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        static BOOL bLanguageDefault = TRUE;
        if (bLanguageDefault)
            _tuiApp.LoadXml(_Module.m_hInst, 0, L"Lang2.xml");
        else
            _tuiApp.LoadXml(_Module.m_hInst, 0, L"Lang1.xml");

        bLanguageDefault = !bLanguageDefault;
        return 0;
    }

    TMResult OnCloseDlg(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        PostQuitMessage(0);
        return 0;
    }

private:
    BOOL m_bLButtonDown;
    POINT m_ptMouse;
};

#endif  // HELLO_WORLD_H