/**
 *    @file
 *    @brief
 *    CWndClipboard
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2016-12-9
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2016 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "Clipboard.h"


using namespace TM;

CDataFormat s_dfHtml(L"HTML Format");

void GetClipHDropDataInfo(CString& str, int nFormat)
{
    CDropHandle hDrop((HDROP)GetClipboardData(nFormat));
    str += L"\r\n";
    for (int i = 0; i < 16000; ++i)
    {
        TCHAR c[4096];
        if (!hDrop.DragQueryFile(i, c, 4096))
            return;
        str += c;
        str += L"\r\n";
    }
}


void GetClipTxtDataInfo(CString& str, int nFormat)
{
    HGLOBAL hSrc = GetClipboardData(nFormat);
    if (!hSrc)
        return;
    
    str += "\r\n";
    void* p = (void*)GlobalLock(hSrc);
    switch (nFormat)
    {
    case CF_OEMTEXT: 
    case CF_TEXT: str += TM_A2T((TMCStrA)p); break;
    case CF_UNICODETEXT: str += (TMCStrW)p; break;
    default:
        if (nFormat == s_dfHtml)
            str += TM_UTF2T((TMCStrA)p);
        break;
    }
    GlobalUnlock(hSrc);
}

CString GetClipText(HWND hWnd)
{
    CString str;

    if (!CountClipboardFormats())
        return str;
    
    if (!OpenClipboard(hWnd))
        return str;
    
    for (int uFormat = 0; uFormat = EnumClipboardFormats(uFormat); str += "\r\n\r\n")
    {
        str += VA2TT<TMCharT>(L"%2d", uFormat);
        str += L" - ";
        
#define CASEFMT(x) case x: str += L#x
        switch (uFormat)
        {
            CASEFMT(CF_TEXT); GetClipTxtDataInfo(str, uFormat); break;
            CASEFMT(CF_BITMAP);  break;
            CASEFMT(CF_METAFILEPICT);  break;
            CASEFMT(CF_SYLK);  break;
            CASEFMT(CF_DIF);  break;
            CASEFMT(CF_TIFF);  break;
            CASEFMT(CF_OEMTEXT); GetClipTxtDataInfo(str, uFormat); break;
            CASEFMT(CF_DIB);  break;
            CASEFMT(CF_PALETTE);  break;
            CASEFMT(CF_PENDATA);  break;
            CASEFMT(CF_RIFF);  break;
            CASEFMT(CF_WAVE);  break;
            CASEFMT(CF_UNICODETEXT); GetClipTxtDataInfo(str, uFormat); break;
            CASEFMT(CF_ENHMETAFILE);  break;
            CASEFMT(CF_HDROP); GetClipHDropDataInfo(str, uFormat); break;
            CASEFMT(CF_LOCALE);  break;
            CASEFMT(CF_MAX);  break;
            
            CASEFMT(CF_OWNERDISPLAY);  break;
            CASEFMT(CF_DSPTEXT);  break;
            CASEFMT(CF_DSPBITMAP);  break;
            CASEFMT(CF_DSPMETAFILEPICT);  break;
            CASEFMT(CF_DSPENHMETAFILE);  break;
            
            
            CASEFMT(CF_PRIVATEFIRST);  break;
            CASEFMT(CF_PRIVATELAST);  break;
            
            CASEFMT(CF_GDIOBJFIRST);  break;
            CASEFMT(CF_GDIOBJLAST);  break;
        default:
            {
                TCHAR c[1024] = L"unknown";
                GetClipboardFormatName(uFormat, c, 1024);
                str += c;
                
                if (uFormat == s_dfHtml)
                    GetClipTxtDataInfo(str, uFormat);
            }
            break;
        }
        
    } 
    CloseClipboard(); 
    
    return str;
}
