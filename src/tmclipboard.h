/*
 *  @file
 *  @brief
 *    CClipboardNative:     clipboard for native
 *    CClipboard:           clipboard for framework
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.8.12
 *
 *  Copyright (C) 2020 miragekiller
 */

#pragma once

#ifndef TMCLIPBOARD_H
#define TMCLIPBOARD_H

#include "tmdrag.h"

#ifdef TM_QT
#   include <QClipboard>
#endif


TM_BGN_NAMESPACE

#ifdef TM_WIN

///////////////////////////////////////////////////////////////////////////////
// CClipboardNative
struct CClipboardNative
{
    CClipboardNative()
    {
        DbgVerify(::OpenClipboard(0));
    }
    
    ~CClipboardNative()
    {
        DbgVerify(::CloseClipboard());
    }
    
    static const CDataFormat& GetHtmlDataFormat()
    {
        static CDataFormat s_dfHtml(L"HTML Format");
        return s_dfHtml;
    }
    
    void EmptyClipboard()
    {
        DbgVerify(::EmptyClipboard());
    }

//     void AddString(CStrViewA str)
//     {
//         CStringW strx(str);
//         AddString(strx.data(), strx.GetLength());
//     }
// 
//     void AddString(CStrViewW str)
//     {
//         AddString(str.data(), str.GetLength());
//     }

    void AddString(CStrViewW str)
    {
        AddString(str.data(), str.GetLength());
    }

    void AddString(TMCStrW pstr, int n)
    {
        if (!n)
            return;
        TM::CGlobalMemHandle gmW;
        gmW.Assign(pstr, n * 2 + 2, GMEM_MOVEABLE | GMEM_DDESHARE);
        DbgVerify(SetClipboardData(CF_UNICODETEXT, gmW));
    }
    
    void AddHTML(CStrViewW str)
    {
        AddHTML(str.data(), str.GetLength());
    }

    void AddHTML(TMCStrW pstr, int n)
    {
        TM::W2UTF strUtf(pstr, n);
        n = strUtf.GetLength();
        CStdStrA strA;
        strA.Format("Version:0.9\r\n"
            "StartHTML:000000101\r\n"
            "EndHTML:%010u\r\n"
            "StartFragment:000000115\r\n"
            "EndFragment:%010u\r\n"
            "<html><body>\r\n" 
            "<!--StartFragment-->\r\n", n + 175, n + 139);
        strA += strUtf;
        strA += "<!--EndFragment-->\r\n</body>\r\n</html>";
        CGlobalMemHandle gmW;
        gmW.Assign(strA.c_str(), strA.GetLength() + 1, GMEM_MOVEABLE | GMEM_DDESHARE);
        DbgVerify(SetClipboardData(GetHtmlDataFormat(), gmW));
    }
    
    TMBool GetHTML(CString& str)
    {
        HGLOBAL hSrc = GetClipboardData(GetHtmlDataFormat());
        if (!hSrc)
            return false;
        
        CStdStrA strUtf((TMCStrA)GlobalLock(hSrc));
        GlobalUnlock(hSrc);
        
        int n1 = strUtf.Find("StartFragment:");
        int n2 = strUtf.Find("EndFragment:");
        if (n1 <= 0 || n2 <= 0)
        {
            str.clear();
            return true;
        }
        long x1 = -1; long x2 = -1;
        StrScanLong(strUtf.data() + n1 + 14, 16, x1, 10);
        StrScanLong(strUtf.data() + n2 + 12, 16, x2, 10);
        if (x1 <= 0 || x2 <= 0 || x2 > strUtf.GetLength() || x1 >= x2)
        {
            str.clear();
            return true;
        }
        
        TM::UTF2T strOut(strUtf.substr(x1, x2 - x1));
        str = std::move(strOut);
        return true;
    }
    
    TMBool GetText(CString& str)
    {
        HGLOBAL hSrc = GetClipboardData(TM_SW_UNICODE(CF_UNICODETEXT, CF_TEXT));
        if (!hSrc)
            return false;
        str = (TMCStr)GlobalLock(hSrc);
        GlobalUnlock(hSrc);
        return true;
    }
};

#endif // TM_WIN


#ifdef TM_QT

///////////////////////////////////////////////////////////////////////////////
// CClipboard
class CClipboardQt
{
public:
    QClipboard* m_clipboard;
    
    CClipboardQt()
        : m_clipboard(QApplication::clipboard())
    {
    }
    
    ~CClipboardQt()
    {
    }
    
    //     static const CDataFormat& GetHtmlDataFormat()
    //     {
    //         static CDataFormat s_dfHtml(L"HTML Format");
    //         return s_dfHtml;
    //     }
    
    void EmptyClipboard()
    {
        m_clipboard->clear();
    }
    
    void AddString(TMCStr pstr, int n)
    {
        m_clipboard->setText(TM_T2Q(pstr, n));
    }
    
    void AddHTML(TMCStr pstr, int n)
    {
        QMimeData mdata;
        mdata.setHtml(TM_T2Q(pstr, n));
        m_clipboard->setMimeData(&mdata);
    }

    void AddHTML(const CStrView& str)
    {
        AddHTML(str.data(), str.GetLength());
    }
    
    TMBool GetHTML(CString& str)
    {
        const QMimeData* data = m_clipboard->mimeData();
        if (!data->hasHtml())
            return false;
        str = TM_Q2W(data->html());
        return true;
    }
    
    TMBool GetText(CString& str)
    {
        const QMimeData* data = m_clipboard->mimeData();
        if (!data->hasText())
            return false;
        str = TM_Q2W(data->text());
        return true;
    }
};

#   define  CClipboard CClipboardQt

#else // #ifdef TM_QT

#   define  CClipboard CClipboardNative

#endif // #ifdef TM_QT else


TM_END_NAMESPACE



#endif //TMCLIPBOARD_H
