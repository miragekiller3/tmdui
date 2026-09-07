#include "stdafx.h"
#include "main.h"
//#include "../../../tmgdix.h"

// #include "athtml.cpp"

using namespace TM;



// static CMain::Register<0> s_callback_CMain;


//  class CAAA;
// 
// TM::CTuiPtr<CAAA> s_ptr;


TMResult CMain::OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    EndDialog(wID);
    return 0;
}

void CMain::Create()
{
    CRect rc(0, 0, 640, 480);
    m_hTest.Create(0, rc);
    this->CreateByHWND(m_hTest);
    this->DoDialogBox(0);
}
