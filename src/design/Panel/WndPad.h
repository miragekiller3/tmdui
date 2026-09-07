/**
 *    @file
 *    @brief
 *    CWndPad
 *
 *
 *    @author 
 *    @date    2014-11-13
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2014 miragekiller                               
 *    All rights reserved  
 */

#ifndef WNDPAD_H
#define WNDPAD_H

#include "Panel.h"
#include "EditStream.h"
#include "ResourceID.h"
#include "ResFile.h"


class CPanelApp
    : public TM::CView
    , public IProj
{
public:
    TM_SELF_ALLOC(CPanelApp);

public:
    HWND            m_hRealWnd;
    HINSTANCE       m_hResWin;
    IProjMgr*       m_pEditor;
    TM::CEditStream m_editStream;
    CIDMgr          m_idMgr;
    TM::CString     m_strXmlFileName;
    CResFile        m_fileRC;
    CQRcFile        m_fileQRC;
    TM::CModule*    m_pmCurrent;

public:
    CPanelApp(PANELINFO& info);
    virtual ~CPanelApp() {}
    IObjEdit* GetDataByObjID(TMUInt32 id);
    TM::CModule* GetEditorModule() { return m_pmCurrent ? m_pmCurrent : GetModule(HEDITOR); }
    static TM::CString FormatAcc(IAccessible* pacc, CComVariant valChild);
    static HRESULT AccFromPoint(IAccessible* root, POINT pt, bool bClientPT, IAccessible** ppAcc, VARIANT* pvarChild);
    static HRESULT AccFromPoint(HWND hwndPoint, POINT pt, bool bClientPT, IAccessible** ppAcc, VARIANT* pvarChild);

private:
    void CreateEditorModule();

private: // CView
    virtual void InternalSetCapture(const TM::CTuiWgt* hWgt) tm_override;
    virtual void InternalReleaseCapture() tm_override;
    virtual TMBool InternalIsCapture(const TM::CTuiWgt* hWgt) tm_override;
    virtual void InternalSetFocus(const TM::CTuiWgt* hWgt) tm_override;
    virtual TMBool InternalIsWindowActive(const TM::CTuiWgt* hWgt) tm_override;
    virtual TM::CPoint InternalGetScreenOrg(TM::HTUI hWgt) tm_override;

public:
    TMBool LoadXmlFile(const TM::CString& strXml, TMBool bMerge);
    TMBool SaveXml(const TM::CString& strPath, TM::CStringW& str, CIDUsedList* aID, ESaveMode eMode);
    void SaveSkin(LPCTSTR pFileName, TMUInt bLang, TM::IDomDocument** xmlOut);

public: // IProj
    virtual void Release() tm_override;
    virtual TMCStr* GetObjRootNames() tm_override;  // &(TMCStr[3]);
    virtual TMResult TrackMenuPop(HWND hWnd, TMPoint pt, HMENU hMenuRoot, IObjEdit* pSel) tm_override; // return: command need main window process. 
    virtual void ModifyUser(LPCTSTR pNameOld, const TM::CStrView& pNameNew) tm_override;
    virtual TMCStr GetClassByIndex (TM::EObjType eType, int n) tm_override;
    virtual void GetDeviceNames(TMCStr** ppDisplay, TMCStr** ppText) tm_override;

    virtual void ResetUserIds(const TM::CStrView& strUSer, int idStart) tm_override;
    virtual int GetUsers(CPopIDList& aMacro) tm_override;
    virtual void MergeUsers(CPopIDList& aMacro) tm_override;

    virtual TM::CString GetPreSearchID(CPreSearchID& aList) tm_override;
    virtual void LoadLanguage(LPCTSTR pFileName) tm_override;
    virtual TMBool SaveFile(LPCTSTR pFileName, ESaveMode eMode, TM::IDomDocument** xmlOut) tm_override;
    virtual TM::CString LoadFromFile(LPCTSTR pFileName, TMBool bMerge) tm_override;
    virtual TM::CString SetObjectIDMacro(const TM::CStrView& pOld, const TM::CString& strNew) tm_override;
    virtual IObjEdit* SyncAncestor(TM::IDomNode* ptrOld, TM::IDomNode* ptrNew) tm_final;
    virtual IObjEdit* PasteTo(IObjEdit* pParent, TM::IDomNode* ptr) tm_override;
    virtual void GetModuleList(CModuleListH* aListH, CModuleListS* aListS) tm_override;
//     virtual void SpySetEmbed(int nMode) tm_override {}
//     virtual void TrackFocusObject(CMainWnd& wnd, TMBool bShow) tm_final;
    virtual void AppendSysInfo(TM::CString& str, ESysInfo eType) tm_override;

    virtual void TesterRTL(TMBool bKeep) tm_override;
    virtual void TesterLanguage(LPCTSTR p) tm_override;
    virtual void TesterChangeDevice(int bDisplay, int id) tm_override;


public:
    IProjMgr* GetProjMgr() { return m_pEditor; }

};


//extern TM::CPanelApp*   s_pView;
// void InitPanelApp();


#endif // WNDPAD_H