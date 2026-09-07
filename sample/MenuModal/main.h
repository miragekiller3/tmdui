#ifndef MAIN_H
#define MAIN_H

#include "Resource.h"
#include "tuires.h"
#include "../../tmduiatl.h"


class CMn : public TM::CTuiDialogImpl<CMn>
{
public:
    enum { IDD = IDD_TUI_MANAGE_10002 };
    
public:
    BEGIN_MSG_MAP           (CMain)
        COMMAND_ID_HANDLER  (IDOK,              OnCancel)
        COMMAND_ID_HANDLER  (IDCANCEL,          OnCancel)
        COMMAND_ID_HANDLER  (IDC_POP,           OnPop)
        ROUTE_HANDLER       (WM_CONTEXTMENU,    OnPop)
        CHAIN_MSG_MAP       (root_class)
    END_MSG_MAP()

    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        EndDialog(wID);
        return 0;
    }

    TMResult OnPop(...)
    {
        TMPoint pt;
        GetCursorPos (&pt);
        pt.x += 3;
        pt.y += 3;
        CMn m;
        m.DoMenuPop (m_hWnd, 0, &pt);
        m.DestroyWindow();
        return 0;
    }
};    


class CMain : public CDialogImpl<CMain>
{
public:
    enum { IDD = IDD_DIALOG1 };
    
public:
    BEGIN_MSG_MAP           (CMain)
        MESSAGE_HANDLER     (WM_CONTEXTMENU,    OnPop)
        COMMAND_ID_HANDLER  (IDCANCEL,          OnCancel)
        COMMAND_ID_HANDLER  (IDC_BUTTON1,       OnPop)
    END_MSG_MAP()
        
    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        EndDialog(wID);   
        return 0;
    }
    
    TMResult OnPop(...)
    {
        TMPoint pt;
        GetCursorPos (&pt);
        pt.x += 3;
        pt.y += 3;
        CMn m;
        m.DoMenuPop (m_hWnd, 0, &pt);
        m.DestroyWindow();
        return 0;
    }
};



#endif // MAIN_H