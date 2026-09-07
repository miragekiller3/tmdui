/**
 *    @file
 *    @brief
 *    CDlgSaveLang
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2016-12-27
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2016 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgSaveLang.h"
#include "MainWnd.h"
#include "tminet.h"

using namespace TM;

CDlgSaveLang::CDlgSaveLang(CMainWnd& wnd, LPCTSTR pPath)
    : m_wndMain(wnd)
    , m_pPath (pPath)
{
}

CDlgSaveLang::~CDlgSaveLang()
{
}

TMResult CDlgSaveLang::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    return 0;
}

ESaveMode CDlgSaveLang::GetSaveMode()
{
    int mode = eSaveModeLang;
    if (TM::CTuiButton(Item(IDC_CHECKBOX_31037)).GetCheck())
        mode |= eSaveLangStringTable;
    if (TM::CTuiButton(Item(IDC_CHECKBOX_31038)).GetCheck())
        mode |= eSaveLangMenu;
    return (ESaveMode)mode;
}


struct CTranslater
{
    virtual CStringW MakeURL(const CStringW& strText) = 0;
    virtual CStringW MakeReturn(CStringA& strRet, const CStringW strText) = 0;

    CStringW Translate(const CStringW& strText)
    {
        CStringW strURL(MakeURL(strText));
        CStringA strReturn;
        GetUrlFile(strURL, strReturn);
        return MakeReturn(strReturn, strText);
    }
};

struct CTranslaterGoogle : public CTranslater
{
    CStringW strURL;
    CTranslaterGoogle(LPCWSTR strLangStr = L"zh_TW")
    {
        strURL.Format(L"http://translate.google.cn/translate_a/single?client=gtx&dt=t&dj=1&ie=UTF-8&sl=auto&tl=%s&q=", strLangStr);
    }

    virtual CStringW MakeURL(const CStringW& strText)
    {
        return strURL + strText;
    }

    virtual CStringW MakeReturn(CStringA& strRet, const CStringW strText)
    {
        int n = strRet.Find("\"trans\":\"");
        if (n < 10)
            return strText;
        n += 9;
        int n2 = strRet.Find("\",\"orig\":");
        if (n2 < n)
            return strText;
        return TM_UTF2W(strRet.substr(n, n2 - n));
    }

};


struct CSaveLan
{
    CDomDocument docNew;
    CTranslaterGoogle strURL;

    CSaveLan(LPCWSTR strLangStr = L"zh_TW")
        : strURL(strLangStr)
    {
    }

    void SaveAttribs(IDomElement* pDest, CComPtr<IDomNodeList> attribs)
    {
        for (int i = 0; ; ++i)
        {
            CDomAttributePtr ptrA(attribs->item(i));
            if (!ptrA)
                break;
            CDomStrPtr strName = ptrA->get_name();
            CStringW strVal = ptrA->get_value();
            strVal = strURL.Translate(strVal);
            pDest->setAttribute(strName, strVal);
       }
    }

    void CopyNode(IDomNode* pDest, IDomNode* pSrc)
    {
        for (IDomNode* pc = pSrc->get_firstChild(); pc; pc = pc->get_nextSibling())
        {
            CDomElementPtr ptr(pc);
            if (!ptr)
                continue;
            CDomStrView strNodeName(ptr->get_nodeName());
            IDomElement* pNew = docNew->appendChildElement(pDest, strNodeName);

            SaveAttribs(pNew, ptr->get_attributes());
            CopyNode(pNew, pc);
        }
    }
};


TMResult CDlgSaveLang::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    ESaveMode eMode = GetSaveMode();
//     if (Item(IDC_GROUPBOX_30283).IsWindowEnabled())
//     {
//         CDomDocumentPtr doc;
//         m_wndMain.GetProj()->SaveFile(0, eMode, &doc);
//         CSaveLan sl;
//         sl.CopyNode(sl.docNew, doc);
//     }
//     else
        m_wndMain.GetProj()->SaveFile(m_pPath, eMode, 0);

    EndDialog(wID);
//    DestroyWindow();
    return 0;
}

TMResult CDlgSaveLang::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{

    EndDialog(wID);
//    DestroyWindow();
    return 0;
}



