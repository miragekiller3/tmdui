/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.10.16
 *
 *  Copyright (C) 2020 miragekiller
 */

#pragma once

#ifndef UIHOTKEY_H
#define UIHOTKEY_H

#include "uiwidget.h"
#include "input.h"


TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CTuiHotKeyImpl
class TM_NO_VTABLE CTuiHotKeyImpl
    : public CTuiWgt
    , public CHotKeyWinT<CTuiHotKeyImpl, CPropHotKeyID, CTuiHotKey::eCountHotKey>
{
private:
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_CLASS  ("Hotkey0", m_Hotkey[0])
        OBJ_PROP_ID     ("ID0",     m_Hotkey[0].m_id)
        OBJ_PROP_CLASS  ("Hotkey1", m_Hotkey[1])
        OBJ_PROP_ID     ("ID1",     m_Hotkey[1].m_id)
        OBJ_PROP_CLASS  ("Hotkey2", m_Hotkey[2])
        OBJ_PROP_ID     ("ID2",     m_Hotkey[2].m_id)
        OBJ_PROP_CLASS  ("Hotkey3", m_Hotkey[3])
        OBJ_PROP_ID     ("ID3",     m_Hotkey[3].m_id)
        OBJ_PROP_CLASS  ("Hotkey4", m_Hotkey[4])
        OBJ_PROP_ID     ("ID4",     m_Hotkey[4].m_id)
        OBJ_PROP_CLASS  ("Hotkey5", m_Hotkey[5])
        OBJ_PROP_ID     ("ID5",     m_Hotkey[5].m_id)
        OBJ_PROP_CLASS  ("Hotkey6", m_Hotkey[6])
        OBJ_PROP_ID     ("ID6",     m_Hotkey[6].m_id)
        OBJ_PROP_CLASS  ("Hotkey7", m_Hotkey[7])
        OBJ_PROP_ID     ("ID7",     m_Hotkey[7].m_id)
        OBJ_PROP_CLASS  ("Hotkey8", m_Hotkey[8])
        OBJ_PROP_ID     ("ID8",     m_Hotkey[8].m_id)
        OBJ_PROP_CLASS  ("Hotkey9", m_Hotkey[9])
        OBJ_PROP_ID     ("ID9",     m_Hotkey[9].m_id)
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiHotKeyImpl)
        CHAIN_MSG_MAP   (CHotKeyWin)
        CHAIN_MSG_MAP   (CTuiWgt)
    END_TUIMSG_MAP()
};


TM_END_NAMESPACE

#endif // UIHOTKEY_H
