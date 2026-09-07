/**
 *  @file
 *  @brief
 *  CDlgDllAnalysis
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025-7-10
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2025 miragekiller                             
 *  All rights reserved  
 */

#ifndef DLGDLLANALYSIS_H
#define DLGDLLANALYSIS_H

#include "tuires.h"
#include "tmgdix.h"
#include "tmdebugext.h"


class CDlgDllAnalysis
     : public TM::CTuiWidgetImpl<CDlgDllAnalysis> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgDllAnalysis>
//     , public TM::CTuiMsgFilter
    , public TM::IColumnProxy
{
public:
    // for list 1 and header 1
    typedef TM::CVector<TM::CString>          CNameList;

    struct SECS
    {
        struct SEC
        {
            int  nSize;
            TM::CText txt;
            TM::CString strHInt;
            PIMAGE_SECTION_HEADER secImageHeader;
            
            SEC() : nSize(0), secImageHeader(0) {}
        };
        typedef TM::CMap<int, SEC>              CSecList;           // string id, SEC

        TM::CStringA strCompDll;
        CSecList aSec;
//         TMUInt64 nImageBase;

        SEC* Add(int idStr, int nSize);
        int MakeTxt();
    };

    // for list 2 and header 2
    struct SECDATA
    {
        DWORD64 hModule;
        TMLParam n[4];
        TM::CText txt[4];
        typedef TM::CTuiProcDelete<SECDATA> CAutoDel;
    };

    CNameList       m_aName;
    SECS*           m_secAll;   // first line

public:
    enum { IDD = IDC_MANAGE_DLL_ANALYSIS };
    
    BEGIN_TUIMSG_MAP (CDlgDllAnalysis)
//     BEGIN_MSG_MAP (CDlgDllAnalysis)
        MESSAGE_HANDLER     (WM_INITDIALOG,                     OnInitDialog)
        
        COMMAND_HANDLER     (IDC_BUT_EXPORT_30418, BN_CLICKED,  OnExport)
        COMMAND_HANDLER     (IDC_LISTBOX_30410, LBN_SELCHANGE,  OnSelChange2)
        COMMAND_HANDLER     (IDC_LISTBOX_30410, LBN_DBLCLK,     OnDblClk2)
        MESSAGE_HANDLER     (WM_DESTROY,                        OnDestroy)
        COMMAND_HANDLER     (IDC_LISTBOX_30407, LBN_SELCHANGE,  OnSelChange1)
//        ROUTE_ID_HANDLER    (IDC_LISTBOX_30407, WM_LBUTTONDOWN, OnSelChange1)
        COMMAND_ID_HANDLER  (IDOK,                              OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                          OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnExport(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnSelChange1(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
//    TMResult OnSelChange1(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnSelChange2(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnDblClk2(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

public:
    CDlgDllAnalysis();
    ~CDlgDllAnalysis();
    void InitModules(TM::CTuiListBox& lb);
    void InitModule(TM::CTuiListBox& lb, HMODULE* mdl, int cb);


    virtual void GetRenderText(TMLParam lpTarget, TMLParam lpHeaderItem, TM::CText& text) tm_final;
    virtual TM::CString GetHInt(TMLParam lpTarget, TMLParam lpHeaderItem) tm_final;
    virtual bool Sort(TMLParam lpTarget1, TMLParam lpTarget2, TMLParam lpHeaderItem, TMBool bSortUp) tm_final;
    virtual void Reclaim(TMLParam nTargetItemData) tm_final;

};


#endif // DLGDLLANALYSIS_H