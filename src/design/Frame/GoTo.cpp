/*
 *  @file
 *  @brief
 *  undo manage
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.12.17
 *
 *  Copyright (C) 2010 miragekiller
 *  All rights reserved.
 */

#include "StdAfx.h"
#include "Goto.h"
#include "MainWnd.h"
#include "ProjMgr.h"

using namespace TM;

CGoto::CGoto()
    : m_nOptPos (-1)
    , m_nOpt(0)
{

}

CMainWnd& CGoto::GetMainWnd()
{
    return *TM_OTHIS(CMainWnd, m_goto);
}

void CGoto::Clear()
{
    m_nOptPos = -1;
    m_nOpt = 0;
    m_aOpt.clear();
}

void CGoto::Forword()
{
    int nSize = m_aOpt.size();
    if (m_nOptPos<0 || m_nOptPos>=nSize-1)
        return;
    m_nOpt = 1;
    ++m_nOptPos;
    TMUInt64 n = m_aOpt[m_nOptPos];
    GetMainWnd().GetPanel().Goto(n);
    m_nOpt = 0;
}

void CGoto::Back()
{
    if (m_nOptPos<1)
        return;
    
    m_nOpt = 2;
    --m_nOptPos;
    TMUInt64 n = m_aOpt[m_nOptPos];
    GetMainWnd().GetPanel().Goto(n);
    m_nOpt = 0;
}

void CGoto::Add(TMUInt64 nID)
{
    if (m_nOpt != 1 && m_nOpt != 2)
    {
        ++m_nOptPos;
        m_aOpt.resize(m_nOptPos+1);
        m_aOpt[m_nOptPos] = nID;
    }
}


