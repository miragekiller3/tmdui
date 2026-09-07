/*
 *  @file
 *  @brief CLocal
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.2.27
 *
 *  Copyright (C) 2016 miragekiller
 */

#pragma once

#ifndef TMLOCAL_H
#define TMLOCAL_H

#include "tmtchar.h"


TM_BGN_NAMESPACE


struct CLocal
{
    ATLocal  m_lcID;

    CLocal(ATLocal lcid= 0) : m_lcID(lcid) {}

    operator ATLocal () const { return m_lcID; }

    bool IsRTL() const
    {
        TMUShort pid = (m_lcID & 0x3ff); //PRIMARYLANGID(m_lcID);
        return pid == 0x01 || pid == 0x0d; // LANG_ARABIC || LANG_HEBREW ;
    }

    TMBool IsDBCS() const
    {   
        switch (m_lcID & 0x3ff)
        {
        case 0x04: // LANG_CHINESE
        case 0x11: // LANG_JAPANESE
        case 0x12: // LANG_KOREAN
            return true;
        }
        return false;
    }

//     void Set(CStrView str)
//     {
//         m_lcID = str.ToLong();
//     }

    void Set(long lid)
    {
        m_lcID = lid;
    }

    TMInt8 ConvertSZ(TMUInt8 code) const
    {
        if (IsRTL())
        {
            switch (code)
            {
            case WMSZ_TOPLEFT: return WMSZ_TOPRIGHT;
            case WMSZ_TOPRIGHT: return WMSZ_TOPLEFT;
            case WMSZ_BOTTOMLEFT: return WMSZ_BOTTOMRIGHT;
            case WMSZ_BOTTOMRIGHT: return WMSZ_BOTTOMLEFT;
            case WMSZ_LEFT: return WMSZ_RIGHT;
            case WMSZ_RIGHT: return WMSZ_LEFT;
            }
        }
        return code;
    }

    TMWParam ConvertSC(TMWParam wp) const
    {
        if ((wp & 0xFFF0) == SC_SIZE && IsRTL())
        {
            switch (wp & 0x000F)
            {
            case WMSZ_TOPLEFT:      return SC_SIZE | WMSZ_TOPRIGHT;
            case WMSZ_TOPRIGHT:     return SC_SIZE | WMSZ_TOPLEFT;
            case WMSZ_BOTTOMLEFT:   return SC_SIZE | WMSZ_BOTTOMRIGHT;
            case WMSZ_BOTTOMRIGHT:  return SC_SIZE | WMSZ_BOTTOMLEFT;
            case WMSZ_LEFT:         return SC_SIZE | WMSZ_RIGHT;
            case WMSZ_RIGHT:        return SC_SIZE | WMSZ_LEFT;
            }
        }
        return wp;
    }

    int ConvertKey(int key) const 
    {
        if (IsRTL())
        {
            switch (key)
            {
            case VK_LEFT: return VK_RIGHT;
            case VK_RIGHT: return VK_LEFT;
            }
        }
        return key;
    }

    //  zh-cn; en-gb; ...
    int ToString(TMStr buf)
    {
        DbgAssert(buf);
#ifdef TM_WIN
        int ccBuf = GetLocaleInfo(m_lcID, LOCALE_SISO639LANGNAME, buf, 9);
        buf[ccBuf-1] = '-';
        ccBuf += GetLocaleInfo(m_lcID, LOCALE_SISO3166CTRYNAME, buf + ccBuf, 9);
        return ccBuf - 1;
#else
        TMCStr code = TM_T("und");

        switch (m_lcID)
        {
        case 0x0404: code = TM_T("zh-tw"); break;
        case 0x0804: code = TM_T("zh-cn"); break;
        case 0x0c04: code = TM_T("zh-hk"); break;
        case 0x1004: code = TM_T("zh-sg"); break;
        case 0x0409: code = TM_T("en-us"); break;
        case 0x0809: code = TM_T("en-gb"); break;
        case 0x0c09: code = TM_T("en-au"); break;
        case 0x1009: code = TM_T("en-ca"); break;
        case 0x0407: code = TM_T("de-de"); break;
        case 0x040c: code = TM_T("fr-fr"); break;
        case 0x0410: code = TM_T("it-it"); break;
        case 0x0411: code = TM_T("ja-jp"); break;
        case 0x0412: code = TM_T("ko-kr"); break;
        case 0x0419: code = TM_T("ru-ru"); break;
        case 0x0416: code = TM_T("pt-br"); break;
        case 0x0816: code = TM_T("pt-pt"); break;
        case 0x040a: code = TM_T("es-es"); break;
        case 0x041d: code = TM_T("sv-se"); break;
        case 0x041f: code = TM_T("tr-tr"); break;
        case 0x0415: code = TM_T("pl-pl"); break;
        case 0x0405: code = TM_T("cs-cz"); break;
        case 0x0408: code = TM_T("el-gr"); break;
        case 0x040d: code = TM_T("he-il"); break;
        case 0x0401: code = TM_T("ar-sa"); break;
        case 0x0422: code = TM_T("uk-ua"); break;
        case 0x0425: code = TM_T("et-ee"); break;
        case 0x0424: code = TM_T("sl-si"); break;
        default:
            switch (m_lcID & 0x3ff)
            {
            case 0x01: code = TM_T("ar-sa"); break;
            case 0x04: code = TM_T("zh-cn"); break;
            case 0x05: code = TM_T("cs-cz"); break;
            case 0x07: code = TM_T("de-de"); break;
            case 0x08: code = TM_T("el-gr"); break;
            case 0x09: code = TM_T("en-us"); break;
            case 0x0a: code = TM_T("es-es"); break;
            case 0x0c: code = TM_T("fr-fr"); break;
            case 0x0d: code = TM_T("he-il"); break;
            case 0x10: code = TM_T("it-it"); break;
            case 0x11: code = TM_T("ja-jp"); break;
            case 0x12: code = TM_T("ko-kr"); break;
            case 0x15: code = TM_T("pl-pl"); break;
            case 0x16: code = TM_T("pt-br"); break;
            case 0x19: code = TM_T("ru-ru"); break;
            case 0x1d: code = TM_T("sv-se"); break;
            case 0x1f: code = TM_T("tr-tr"); break;
            case 0x22: code = TM_T("uk-ua"); break;
            }
            break;
        }

        int n = StrCopy(buf, 6, code);
        return n;
#endif
    }


//     void ConvertPT(int wMgr, TMPoint& pt)
//     {
//         if (IsRTL())
//             pt.x = wMgr - pt.x;
//     }
};


TM_END_NAMESPACE



#endif // TMLOCAL_H
