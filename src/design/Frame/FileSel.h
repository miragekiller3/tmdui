/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.2.24
 *
 *  Copyright (C) 2021 miragekiller
 */

#pragma once

#ifndef FILESEL_H
#define FILESEL_H


#include "tmwnd.h"


TM_BGN_NAMESPACE

template <class T, int nIDEdit, int nIDBut>
class CFileSelT
{
protected:
    TMCStr m_lpcstrFilter;
    TM::CString m_strFile;

    T* GetThis() { return static_cast<T*>(this); }

public:
    BEGIN_TUIMSG_MAP(CFileSelT)
        COMMAND_HANDLER(nIDBut, BN_CLICKED, OnOpenFile)
    END_TUIMSG_MAP

    TMResult OnOpenFile(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        TM::CFileOpenDialog dlg;
        int nRet = dlg.DoModal(GetThis()->GetHWND(), m_lpcstrFilter, 0, 0, 0,
            OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST); 
        if (nRet == IDOK)
        {
            m_strFile = dlg.GetPathName();
        }
        return 0;
    }
};


TM_END_NAMESPACE



#endif //FILESEL_H
