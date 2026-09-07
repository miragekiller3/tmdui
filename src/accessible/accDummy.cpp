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
#include "accDummy.h"


///////////////////////////////////////////////////////////////////////////////
// CAccDummy
CAccDummy g_accDummy;

long CAccDummy::_get_accItemCount(CAccessibleWnd& sender)
{
    TM::DUMMYINFO info;
    TM::CTuiDummy(sender).GetInfo(info);
    if (info.nMode & (TM::DUMMYINFO::MODE_CHILD))// | TM::DUMMYINFO::FLAG_FOLLOW))
        return 1;
    return 0;
}

long CAccDummy::_get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet)
{
    if (nIndex == 1)
    {
        TM::DUMMYINFO info;
        TM::CTuiDummy(sender).GetInfo(info);
        if (info.nMode & (TM::DUMMYINFO::MODE_CHILD))// | TM::DUMMYINFO::FLAG_FOLLOW))
        {
            AccessibleObjectFromWindow(info.hWnd, OBJID_CLIENT, IID_IDispatch, (void **)ppRet);
            return -1;
        }
        return 0;
    }
    return 1;
}
