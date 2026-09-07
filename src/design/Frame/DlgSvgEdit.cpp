/**
 *  @file
 *  @brief
 *  CDlgSvgEdit
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021-10-20
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2021 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgSvgEdit.h"

using namespace TM;


CDlgSvgEdit::CDlgSvgEdit()
{
}

CDlgSvgEdit::~CDlgSvgEdit()
{
}

void CDlgSvgEdit::InitNode(TM::IDomNode* nodeParent, TM::CTVItem itmParent)
{
    if (!nodeParent)
        return;
    
    for (TM::IDomNode* node = nodeParent->get_firstChild(); node; node = node->get_nextSibling())
    {
        
        CDomElementPtr ele(node);
        if (!ele)
            continue;
        CDomStrPtr name(node->get_nodeName());
        CDomStrPtr id = ele->getAttribute(L"id");
        CTVItem itm = itmParent.AppendChild(id + L"(" + name + L")");
        itm.SetData(node);
        if (name == L"style")
        {
            TM::CDomTextPtr nodeAni = node->get_firstChild();
            CDomStrView value = nodeAni->get_data();
            CStrView line;
            for (size_t i = 0; i = value.Split(i, L"@keyframes", line);)
            {
                CStrView n;
                line.Split(0, L'{', n);
                n = n.Trim();
                if (n.IsEmpty())
                    continue;
                CTVItem itm = m_tvAni.AppendChild(CString(n));
                itm.SetHint(line);
            }
//             if (m_nodeAni) // merge CSS node
//                 m_doc->removeChild(m_nodeAni);
            m_nodeAni = nodeAni;
            itm.SetState(TVIS_HIDE | TVIS_CHECKED, TVIS_HIDE | TVIS_CHECKED);
            continue;
        }
        
        itm.SetState(TVIS_CHECKED | TVIS_NODRAWTEXT, TVIS_CHECKED | TVIS_NODRAWTEXT);
        itm.CreateBindItemWindow(IDC_WINDOW_33034);
        InitNode(node, itm);
    }
}

void CDlgSvgEdit::SyncTreeObj(IDomElement* domParent, TM::CTVItem itmParent)
{
    IDomElement* prev = 0;
    for (TM::CTVItem itm(itmParent.GetChild()); itm; itm = itm.GetNext())
    {
        IDomElement* pNow = (IDomElement*)itm.GetData();
        if (itm.GetState() & TVIS_CHECKED)
        {
            if (prev)
                DomSetZorder(pNow, prev, eZOrderDown);
            else
                DomSetZorder(pNow, domParent, eZOrderChildFirst);
            prev = pNow;
            SyncTreeObj(pNow, itm);
        }
        else
            m_docTemp->appendChild(pNow);
    }
}

void CDlgSvgEdit::SyncTreeAni()
{
}

void CDlgSvgEdit::ReloadPic()
{
    CString str;
    m_doc->transformNode(str, true, 2);
    TM::W2UTF stru8(str);
    TxtFileSave(L"c:\\testtemp.svg", stru8);
    CComPtr<IStream> stream;
    StreamCreateFromMem(stru8, stru8.GetLength(), &stream);
    m_img.Release();
    m_img.CreateFromStream(stream);
    int n = m_img.GetFrameCount();
    m_tex.clear();
    for (int i = 0; i < n; ++i)
    {
        TM::CImage img;
        stream->Seek(LLINT(0), 0, 0);
        img.CreateFromStream(stream);
        img.SetCurrentFrame(i);
        TM::CTexture tex;
        tex.CreateFromImage(img);
        m_tex.push_back(tex);
    }
}

TMResult CDlgSvgEdit::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_doc.LoadFromFile(m_strFile);
    CTuiTree tree1(Item(IDC_TREE_33016));
    CTuiTree tree2(Item(IDC_TREE_33018));
    m_tvObj = tree1.GetRootItem();
    m_tvAni = tree2.GetRootItem();
    m_nodeObj = m_doc->selectSingleNode(L"SVG");
    InitNode(m_nodeObj, m_tvObj);
    m_tvObj.ExpandAll();
    m_tvAni.ExpandAll();
    tree1.RegisterEventSink(this);
    tree2.RegisterEventSink(this);
    ReloadPic();
    SetTimer(500);

//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgSvgEdit::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgSvgEdit::OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    if (!m_tex.size())
        return 0;
    CTuiWidget wnd(pMsg->hWnd);
    CTuiRect rc;
    wnd.GetClientRect(&rc);
    int n = sqrt(m_tex.size());
    int w = rc.Width() / n;
    int h = rc.Height() / n;
    for (int i = 0; i < m_tex.size(); ++i)
    {
        int x = i % n;
        int y = i / n;
        CTuiRect rcx;
        rcx.SetLTWH(rc.Left() + x * w, rc.Top() + y * h, w, h);
        m_tex[i]->Blt(rcx, CRect(0,0,0,0), GX_BLT_DFTSRCWH);
    }

    return 0;
}

TMResult CDlgSvgEdit::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgSvgEdit::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

//CTuiMsgFilter
// BOOL CDlgSvgEdit::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }

// ITVEvent
void CDlgSvgEdit::OnStateChanged (HTVITEM hItem, TMUInt uOld, TMUInt uNew)
{
    TMUInt uXor = uOld ^ uNew;
    if (uXor & TVIS_CHECKED)
    {
        void* p = CTVItem(hItem).GetData();
        if (p)
            SyncTreeObj(m_nodeObj, m_tvObj);
        else
            SyncTreeAni();
        ReloadPic();
    }
}

void CDlgSvgEdit::OnTimer(...)
{
    ++m_nCur;
    if (m_nCur >= m_tex.size())
        m_nCur = 0;
    Item(IDC_WINDOW_33040).Invalidate();
}
