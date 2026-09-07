#include "StdAfx.h"
#include "tvrender.h"
#include "../Design.h"
#include "ObjectTreeView.h"

using namespace TM;

const int INDENT = 14;


CTVCtrlRender::CTVCtrlRender()
    : m_picIcon(AppGetEditor()->GetPic(_Module.GetResourceInstance(), IDP_BMP_CONTROLS))
{
#   define CP_INSERT(x,n) m_aClassPic.insert2(TM_CS(x),n)

    CP_INSERT(TUI_CLASS_STATIC,        1);
    CP_INSERT(TUI_CLASS_EDIT,          2);
    CP_INSERT(TUI_CLASS_RICH,          2);
    CP_INSERT(TUI_CLASS_MEMO,          2);
    CP_INSERT(TUI_CLASS_GROUPBOX,      3);
    CP_INSERT(TUI_CLASS_PUSHBUTTON,    4);
    CP_INSERT(TUI_CLASS_CHECKBOX,      5);
//    CP_INSERT(TM_T("DATETIMEPICKER"),  6);
//    CP_INSERT(TM_T("MONTHCALENDAR"), 7);
//    CP_INSERT(TM_T("ipadress"), 8);
//    CP_INSERT(TM_T("custom"), 9);
//    CP_INSERT(TM_T("comboex"), 10);
    CP_INSERT(TUI_CLASS_TRACKBAR,      11);
//    CP_INSERT(TM_T("hotkey"),        12);
    CP_INSERT(TUI_CLASS_SCROLLBOX,     13);
    CP_INSERT(TUI_CLASS_TREE,          14);
    CP_INSERT(TUI_CLASS_TABBUTTON,     15);
    CP_INSERT(TUI_CLASS_ANIMATE,       16);
    CP_INSERT(TUI_CLASS_VIDEO,         16);
//    CP_INSERT(TM_T("richedit"), 17);
    CP_INSERT(TUI_CLASS_RADIOBUTTON,   18);
    CP_INSERT(TUI_CLASS_COMBOBUTTON,   19);
    CP_INSERT(TUI_CLASS_LISTBOX,       20);
//    CP_INSERT(TM_T("vscrollbar"), 21);
//    CP_INSERT(TM_T("hscrollbar"), 22);
//    CP_INSERT(TM_T("SPIN"), 23);
    CP_INSERT(TUI_CLASS_UPDOWN,        23);
    CP_INSERT(TUI_CLASS_PROGRESS,      24);
    CP_INSERT(TUI_CLASS_MENUITEM,      25);
    CP_INSERT(TUI_CLASS_MENUBAR,       26);
    CP_INSERT(TUI_CLASS_SPLITTER,      27);
    CP_INSERT(TUI_CLASS_ACTIVEX,       28);
    CP_INSERT(TUI_CLASS_WEBVIEW,       29);
    CP_INSERT(TUI_CLASS_ARROW,         30);


    m_txtDot.Create(TM_CT("."));
    m_txtDot->SetFormat(DT_SINGLELINE | DT_VCENTER);
    m_txtAdd.Create(TM_CT("+"));
    m_txtAdd->SetFormat(DT_SINGLELINE | DT_VCENTER);
    m_txtDec.Create(TM_CT("-"));
    m_txtDec->SetFormat(DT_SINGLELINE | DT_VCENTER);
    TMCharT c[] = _T(".");
    for (int i = 0; i < 10; ++i)
    {
        c[0] = '0' + i;
        m_txtNum[i].Create(c);
        m_txtNum[i]->SetFormat(DT_SINGLELINE | DT_VCENTER);
    }
    m_txtNum[9]->SetString(TM_CT("X"));
}

CTVCtrlRender::~CTVCtrlRender()
{
}

TMResult CTVCtrlRender::OnHitTest (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DefWindowProc (uMsg, wParam, lParam);
    TM::TUIHITINFO* phi = (TM::TUIHITINFO*)lParam;
    if (phi->bNC)
        return 0;
    TM::CTVItem ti ((TM::HTVITEM)phi->pTaget);
    if (ti)
    {
        int nLevel = ti.GetLevel();
        TM::CTuiRect rc;
        GetClientRect (&rc);
        int x = phi->pt.x - rc.left;
        if (x < nLevel * INDENT)
        {
            phi->dwCode = TVHT_ONITEMINDENT;
        }
        else if (x < (nLevel + 1) * INDENT)
        {
            phi->dwCode = TVHT_ONITEMBUTTON;
        }
        else
        {
            phi->dwCode = TVHT_ONITEMLABEL;
        }
    }
    
    return 0;
}

TMResult CTVCtrlRender::OnDrawItem (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TM::TUIDRAWITEM* pdi = (TM::TUIDRAWITEM*)lParam;
    IGXApp* gapp = pdi->gxApp;
    
    if (pdi->dwDrawStage == CDDS_PREPAINT)
    {
        CTVItem itm(this->GetSelectedItem());
        if (itm)
        {
            m_itmSelParent = itm.GetParent();
            m_itmSelPrev = itm.GetPrev();
            m_itmSelNext = itm.GetNext();
        }
        else
        {
            m_itmSelParent = 0;
            m_itmSelPrev = 0;
            m_itmSelNext = 0;
        }
    }
    else if (pdi->dwDrawStage == CDDS_ITEMPREPAINT)
    {
        CObjItem ti ((TM::HTVITEM)pdi->hItem);
        IObjEdit* pData = ti.GetData();
        TM::EObjType em;
        int h = ti.GetHeight();
        
        int nLevel = ti.GetLevel();
        
        IGXText* pTxt = m_txtDot;
        TMColor clrOld = 0;
        if (pData)
        {
            em = pData->GetType();
            if (em == TM::e_otPic || em == TM::e_otFrm) // EM_PIC || em == EM_FRAME)
            {
                int x = pData->GetRef();
                pTxt = m_txtNum[x > 9 ? 9 : x];
            }

            if (!pData->IsFilter())
                clrOld = gapp->SetTextColor(ColorFromSys(eColorIndexGrayText));
            else
            {
                EObjStyle dwStyle = pData->GetObjStyle();
                if (!(dwStyle & eObjStyleVisible))
                    clrOld = gapp->SetTextColor(ColorFromSys(eColorIndexBtnShadow));
                else if (dwStyle & eObjStyleDisable)
                    clrOld = gapp->SetTextColor(ColorFromSys(eColorIndexBtnFace));
            }
        }

        if (ti.GetChild())
            pTxt = ti.IsExpand() ? m_txtDec : m_txtAdd;
        
        TM::CTuiRect rc = pdi->rcItem;
        rc.SetHeight(h);
        //         if (ti.IsMouseOver())
        //             TM::gxFrameRect(rc, TMRGB(128, 0, 0));
        
        rc.left += nLevel * INDENT;
        pTxt->Paint(rc, 0, 0, gapp);
        rc.left += INDENT;
        
        if (pData)
        {
            TM::CTuiRect rcx;
            if (em == TM::e_otWgt && m_picIcon && nLevel != 2)
            {
                TM::CString strClass(pData->GetObjClassName());
                int n = m_aClassPic[strClass];
                rcx.SetLTWH(rc.left - 2, rc.top + 1, 24, 16);
                m_picIcon->Render(n, 0, rcx, 0, gapp);
                rc.left += 28;
            }
            else
            {
                rcx.SetLTWH(rc.left, rc.top + 1, h - 2, h - 2);
                if (pData->RendSample(gapp, 0, rcx, pData->eRenderForTree) >= 0)
                    rc.left += (rc.bottom - rc.top + 4);
            }
        }

        if (ti == m_itmSelParent || ti == m_itmSelPrev || ti == m_itmSelNext)
        {
            GXRect rcx(rc);
            rcx.SetWidth(40);
            gapp->FillRect(rcx, AppGetEditor()->GetColorEditSelBackground());// TMRGB(192, 255, 192));
        }
        
        CText txt(*pdi->gxText);
        if (txt)
        {
            txt->SetFormat(DT_SINGLELINE | DT_VCENTER);
            if (ti.IsSelected())
            {
                GXRect rcReal;
                txt->Paint(rc, 0, &rcReal, gapp);
                gapp->InvertRect(rcReal);
            }
            else
                txt->Paint(rc, 0, 0, gapp);
        }

        if (pData)
        {
//             IObjEdit::IEnum* rec = pData->GetFiles();
//             for (rec->Begin(); !rec->Eof(); rec->Next())
//             {
//                 rc.OffsetRectY(h);
//                 TM::CString fn(rec->Value());
//                 _DrawText (pdi->hDC, fn, -1, &rc, DT_SINGLELINE | DT_VCENTER);
//             }

            if (clrOld)
                clrOld = gapp->SetTextColor(clrOld);

        }
    }
    else
    {
        if (pdi->dwDrawStage == CDDS_PREPAINT)
        {
//             SetBkMode (pdi->hDC, TRANSPARENT);
        }
        
        bHandled = false;
    }
    
    return 0;
}

TMResult CTVCtrlRender::OnMouseSwitch (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Invalidate();
    bHandled = false;
    return 0;
}

