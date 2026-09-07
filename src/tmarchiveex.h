/*
 *  @file
 *  @brief  Archive support for more types
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2017.7.4
 *
 *  Copyright (C) 2017 miragekiller
 */

#pragma once

#ifndef TMARCHIVEEX_H
#define TMARCHIVEEX_H

#include "tmgdi.h"
#include "tmstring.h"
#include "tmmonitor.h"


TM_BGN_NAMESPACE

struct CArchiveTypeBinary;
struct CArchiveTypeTxt;
struct CArchiveTypeOutput;
struct CArchiveTypeInput;
struct CArchiveTypeBinaryOutput;
struct CArchiveTypeBinaryInput;
struct CArchiveTypeTxtOutput;
struct CArchiveTypeTxtInput;


//////////////////////////////////////////////////////////////////////////
// common serialize class
// template <class T, int N>
template <class T, class V>
struct CSerializeArrayT
{
    typedef CSerializeArrayT<T, V>                  CSerializeArray;
    typedef V                                       value_type;
    enum { nCount = sizeof(T) / sizeof(value_type) };

    mutable value_type* m_pArray;
    CSerializeArrayT(T* p) : m_pArray((value_type*)p) {}

    template <class Archive> void Serialize(Archive& ar, const CArchiveTypeBinary&) const
    {
        for (int i = 0; i < nCount; ++i)
            ar & m_pArray[i];
    }

    template <class Archive> void Serialize(Archive& ar, const CArchiveTypeTxtOutput&) const
    {
        typedef typename Archive::char_type ch;
        TM::L2TT<ch> str(m_pArray[0]);
        for (int i = 1; i < nCount; ++i)
        {
            str += (ch)',';
            str += TM::L2TT<ch>(m_pArray[i]);
        }
        ar << str;
    }

    template <class Archive> void Serialize(Archive& ar, const CArchiveTypeTxtInput&) const
    {
        typedef typename Archive::char_type ch;
        CStringT<ch> s;
        ar >> s;
        if (ar.IsFail())
            return;
        CStrView str(s);
        for (int i = 0; i < nCount; ++i)
        {
            value_type x = 0;
            str = str.ScanLong(x).Mid(1);
            m_pArray[i] = x;
        }
    }
};

template <class Archive, class Tx, class V>
void ArchiveSerialize(Archive& ar, CSerializeArrayT<Tx, V>& val)
{
    typedef typename Archive::archive_type archive_type;
    val.Serialize(ar.This(), archive_type::type_all());
}

template <class Archive, class Tx, class V>
void ArchiveSerializeO(Archive& ar, const CSerializeArrayT<Tx, V>& val)
{
    typedef typename Archive::archive_type archive_type;
    val.Serialize(ar.This(), archive_type::type_all());
}

//////////////////////////////////////////////////////////////////////////
// common serialize class

// CSerializeWindowTextT
// set/restore windows text
// sample:
//     SERIALIZE_BGN_MAP(CSetUserDlg)
//         SERIALIZE_ENTRY(TM_T("Admin"))
//             SERIALIZE_VAR  (TM_T("username"), Item(IDE_USER).SerializeText())
//             SERIALIZE_VAR2 (TM_T("username"), Item(IDE_USER).SerializeText(), TM_T("bbb")))
//         SERIALIZE_LEAVE()
//     SERIALIZE_END_MAP()
//     OnInitDialog: DoSerializeI(g_cfg);
//     OnOk:DoSerializeO(g_cfg);
template <class Wnd> struct CSerializeWindowTextT
{
    Wnd m_wnd;
    CSerializeWindowTextT(Wnd wnd) : m_wnd(wnd) {}

    template <class T, class W> friend void operator << (T& archive, const CSerializeWindowTextT<W>& txt)
    {
        CString str;
        int n = txt.m_wnd.GetWindowTextLength();
        if(n)
            txt.m_wnd.GetWindowText(str.GetBuffer(n), n + 1);
        archive << str;
    }

    template <class T, class W> friend void operator >> (T& archive, CSerializeWindowTextT<W>& txt)
    {
        CString str;
        archive >> str;
        txt.m_wnd.SetWindowText(str);
    }
};


struct CSerializeWindowPlacement : public CSerializeArrayT<CRect, CRect::value_type>
{
    HWND m_hWnd;
    mutable WINDOWPLACEMENT m_wp;
    CSerializeWindowPlacement(HWND wnd)
        : CSerializeArray((CRect*)&m_wp.rcNormalPosition)
        , m_hWnd(wnd)
    {
    }

    template <class T> friend void operator << (T& archive, const CSerializeWindowPlacement& val)
    {
        val.m_wp.length = sizeof(val.m_wp);
        GetWindowPlacement(val.m_hWnd, &val.m_wp);
        archive & (CSerializeWindowPlacement::CSerializeArray&)val;
    }

    template <class T> friend void operator >> (const T& archive, CSerializeWindowPlacement& val)
    {
        archive & (CSerializeWindowPlacement::CSerializeArray&)val;
        if (archive.IsFail())
            return;
        CMonitor::CalcValideRect(val.m_wp.rcNormalPosition, false, false);
        CWindowAT(val.m_hWnd).DisableDPIChangeResize();
        SetWindowPos(val.m_hWnd, 0,
            val.m_wp.rcNormalPosition.left, val.m_wp.rcNormalPosition.top,
            val.m_wp.rcNormalPosition.right - val.m_wp.rcNormalPosition.left,
            val.m_wp.rcNormalPosition.bottom - val.m_wp.rcNormalPosition.top,
            SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
        CWindowAT(val.m_hWnd).EnableDPIChangeResize();
    }
};


TM_END_NAMESPACE



#endif //TMARCHIVEEX_H
