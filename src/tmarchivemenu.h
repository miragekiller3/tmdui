/*
 *  @file
 *  @brief  Archive support for more types
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.6.15
 *
 *  Copyright (C) 2018 miragekiller
 */

#pragma once

#ifndef TMARCHIVEMENU_H
#define TMARCHIVEMENU_H

#include "tmarchive.h"


TM_BGN_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
// CMenuCheckArchive
struct CMenuCheckArchiveBase
{
    typedef void* section_type;
    HMENU m_hMenu;
    int m_nID;
    CMenuCheckArchiveBase(HMENU hMenu) : m_hMenu(hMenu) {}
    template <class T> void DoEntrySection(section_type& storage, const T& str) {}
    void DoLeaveSection(section_type& storage) {}
    void DoSetKey(int id) { m_nID = id; }
};

struct CMenuCheckArchiveO
: public CArchiveBinaryIImplT<CMenuCheckArchiveO, CEndianHost>
, public CMenuCheckArchiveBase
{
public:
    CMenuCheckArchiveO(HMENU hMenu) : CMenuCheckArchiveBase(hMenu) {}
    
    void DoSerializeVoid(const void* p, size_t n)
    {
        DbgAssert(0);
    }
    
    void DoSerializeString(const CString& str)
    {
        DbgAssert(0);
    }
    
    void DoSerializeInt(long n)
    {
        if (n)
            n = MF_CHECKED;
        CheckMenuItem(m_hMenu, m_nID, MF_BYCOMMAND | n);
    }
};

struct CMenuCheckArchiveI
    : public CArchiveBinaryIImplT<CCfgArchiveI, CEndianHost>
    , public CMenuCheckArchiveBase
{
public:
    CMenuCheckArchiveI(HMENU hMenu) : CMenuCheckArchiveBase(hMenu) {}
    
    void DoSerializeVoid(void* p, size_t n)
    {
        DbgAssert(0);
    }
    
    template <class T> void DoSerializeString(T& str)
    {
        DbgAssert(0);
    }
    
    template <class T> void DoSerializeInt(T& n)
    {
        MENUITEMINFO mInfo = { sizeof(mInfo) };  
        mInfo.fMask = MIIM_STATE;  
        BOOL b = GetMenuItemInfo(m_hMenu, m_nID, false, &mInfo);
        DbgAssert(b);
        n = (0 != (mInfo.fState & MFS_CHECKED));
    }
};

struct CMenuArchive : public CMenuHandle
{
    typedef CMenuCheckArchiveO  CArchiveO;
    typedef CMenuCheckArchiveI  CArchiveI;
    CMenuArchive(HMENU hMenu) : CMenuHandle(hMenu) {}
};


TM_END_NAMESPACE



#endif //TMARCHIVEMENU_H
