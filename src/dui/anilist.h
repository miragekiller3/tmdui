/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.01.04
 *
 *  Copyright (C) 2018 miragekiller
 */

#pragma once

#ifndef ANILIST_H
#define ANILIST_H

#include "tmlist.h"
#include "tmdui.h"


TM_BGN_NAMESPACE


class _CAniImpl;

class CWidgetAniList
{
private:
    struct CAniList : public CMap<int, _CAniImpl*>
    {
    private:
        CAniList() {}
        ~CAniList();
        friend CWidgetAniList;
        TM_SELF_ALLOC_F(CAniList);
        TM_IF_DBG(int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf) { return IC_Size(*this); });
    };
    CAniList* m_aAni;
    friend _CAniImpl;
    
private:
    _CAniImpl*& AddAni(EHitID nHitID);
    void CheckErase();
    void AddMixAni(HTUI hWnd, EHitID nHitID, EButStates nOldState, EButStates nNewState);
    
public:
    CWidgetAniList() : m_aAni(0) {}
    ~CWidgetAniList() { DbgAssert(!m_aAni); }
    void ClearAni();
    void PreDestroy();
    
    void AddScrollShowHideAni(EHitObj nObjID, HTUI hWnd, TMBool bShow);

    void AddButtonAni(const TUIHITINFO& self, const TUIHITINFO* pold, const TUIHITINFO* pnew, TMBool bExtMouseDown, int nCurCheck);
    void AddScrollAni(const TUIHITINFO& self, const TUIHITINFO* pold, const TUIHITINFO* pnew, TMBool bHotSB);
    void AddSysButAni(const TUIHITINFO& self, EButStates nOldState, EButStates nNewState, int nCurCheck);
    TMBool AnimateWindow(HTUI hWnd, TMULong dwTime, TMULong dwFlags, const CTuiRect* rcFrom, const CTuiRect* rcTo, IEase* pEase);

    void AniRender(IGXApp* gapp, int nObjID, ITuiPic* pPic, int nState, EButStates states, const CTuiRect& rc);
    void AniRenderScroll(IGXApp* gapp, ITuiPic* pPic, const SCROLLSTATE& state, const CTuiRect& rc);
    TMBool IsInAnimation(EHitID nObjID);
};



TM_END_NAMESPACE



#endif //ANILIST_H
