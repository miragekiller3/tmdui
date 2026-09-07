/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.08.25
 *
 *  Copyright (C) 2009-2018 miragekiller
 */


#include "stdafx.h"
#include "accTabButton.h"


///////////////////////////////////////////////////////////////////////////////
// CAccPageTab
CAccPageTab g_accPageTab;

//HRESULT CAccPageTab::_get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val)
//{
//        
//    TM::CTuiTabButton tabBtn(sender);
//    TM::CTuiWidget wndTarget = tabBtn.GetTarget();
//    if (!wndTarget)
//        return S_FALSE;
//        
//    TM::CTuiWgt* pChild = NULL;
//    TM::CString strObjType;
//    TM::CTuiWidget wndChild(wndTarget.GetWindow(GW_CHILD));
//    while (wndChild)
//    {
//        if (!wndChild.IsWindowVisible())
//        {
//            wndChild = wndChild.GetWindow(GW_HWNDNEXT);
//            continue;
//        }
//        
//        wndChild.GetClassName(strObjType);
//        if (strObjType != TUI_CLASS_WIDGET)
//        {
//            pChild = wndChild;
//            break;
//        }
//        wndChild = wndChild.GetWindow(GW_CHILD);
//    }
//        
//    TM::CString strPageText;
//    while (pChild)
//    {
//        TM::CTuiWidget labelWnd(pChild);
//        labelWnd.GetClassName(strObjType);
//        if (strObjType == _T("static") || strObjType == _T("memo") || strObjType == _T("edit"))
//        {
//            if (labelWnd.IsWindowVisible())
//            {
//                TM::CString strName;
//                labelWnd.GetWindowText(strName);
//                strPageText += strName;
//                strPageText += _T(" ");
//            }
//        }
//        pChild = labelWnd.GetWindow(GW_HWNDNEXT);
//    }
//    
//    strPageText = strPageText.Trim();
//    val = strPageText;
//    return S_OK;
//}

HRESULT CAccPageTab::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
     HRESULT hr = CAccRadioButton::_get_accState(sender, nIndex, state);
     if (FAILED(hr))
         return hr;

     TM::CTuiButton winCkBtn(sender);
     if (winCkBtn.GetCheck())
     {
         state = (state & ~STATE_SYSTEM_CHECKED) | STATE_SYSTEM_SELECTED;         
     }

     return S_OK;
 }
