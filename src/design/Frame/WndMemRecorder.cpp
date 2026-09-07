/**
 *  @file
 *  @brief
 *  CWndMemRecorder
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025-2-13
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2025 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "WndMemRecorder.h"
#include "../Design.h"


using namespace TM;

// static const char* s_strColor[15] = 
// {
//     "black", // #000000
//     "silver", //#c0c0c0
//     "maroon", //#800000
//     "red", // #ff0000
//     "navy", //#000080
//     "blue", // #0000ff
//     "purple", //#800080
//     "fuchsia", // #ff00ff
//     "green", // #008000
//     "lime", // # 00ff00
//     "olive", // #808000
//     "yellow", // #ffff00
//     "teal", //#008080
//     "aqua", // #00ffff
//     "gray", //#808080
// //    "white", // #ffffff
// };


static const char* s_strColor[15] = 
{
"#000000",
"#808080",

"#ff0000",
"#ff0080",
"#ff8000",
"#ff8080",

"#00ff00",
"#00ff80",
"#80ff00",
"#80ff80",

"#0000ff",
"#0080ff",
"#8000ff",
"#8080ff",
};


CWndMemRecorder::CWndMemRecorder()
    : m_nMaxY(1)
{
}

CWndMemRecorder::~CWndMemRecorder()
{
}

TMResult CWndMemRecorder::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CWndMemRecorder::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CWndMemRecorder::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CWndMemRecorder::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

//CTuiMsgFilter
// BOOL CWndMemRecorder::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }

TMResult CWndMemRecorder::OnPrintClient(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    CTuiRect rc;
    GetClientRect(&rc);
    IGXApp* gapp = _gxApp;
    if (!m_ptrBk)
        m_ptrBk = AppGetEditor()->GetPic(GetModule(), IDC_TILE_BK_GRID);
    m_ptrBk->Render(0, 0, rc, 0, gapp);
    if (m_tex)
        m_tex->Blt(rc, CRect(0,0,0,0), GX_BLT_DFTSRCWH);
    return 0;
}

static void _SVG_LineTo(CString& str, int x, int y)
{
    str += L"L";
    str += TM_L2T(x);
    str += ' ';
    str += TM_L2T(y);
}

void CWndMemRecorder::OnAni(TMHAni hAni, ...)
{
//     typedef CMultiMap<int, CMemDump::key> CTempList;
//     CTempList aTemp;
//     time_t tmNow = CTime::GetCurrentTime();
// 
//     CMemDump dump;
//     at_debug_leak_dump(dump);
//     tm_for (CMemDump::CObjMap::recorder rec(dump.aMap); rec; ++rec)
//     {
//         CMemDump::value v = rec.get_data();
//         int nCount = v.second / v.first;
//         if (aTemp.size() != 0 && nCount < aTemp.begin()->first)
//             continue;
//         aTemp.insert(nCount, rec.get_key());
//         if (aTemp.size() > eObjCount)
//             aTemp.erase(aTemp.begin());
//     }
// 
//     if (m_aObj.size() > eTimeCount)
//         m_aObj.erase(m_aObj.begin());
//     CMemDump::CObjMap& aObj = m_aObj[tmNow];
//     int nNow = 0;
//     tm_for (CTempList::recorder rec(aTemp); rec; ++rec)
//     {
//         int nCount = rec.get_key();
//         aObj[rec.get_data()] = CMemDump::value(nNow += nCount, nCount);
//     }
// 
//     CTuiRect rc;
//     GetClientRect(rc);
//     int h = rc.Height();
// 
//     CStringA strSvg("<svg version=\"1.1\" "
//         "xmlns=\"http://www.w3.org/2000/svg\" "//xmlns:xlink=\"http://www.w3.org/1999/xlink\" "// x=\"0px\" y=\"0px\" "
//         "viewBox=\"0 0 2000 ");
//     strSvg += TM_L2A(h);
//     strSvg += "\" >\r\n";
//     CObjTimeList aClone(m_aObj);
//     int xOffset = 0;
//     int yMax = 0;
//     for (CObjTimeList::recorder rTime(aClone); rTime; ++rTime, ++xOffset)
//     {
//         for (CMemDump::CObjMap::recorder rObj(rTime.get_data()); rObj; ++rObj)
//         {
//             CMemDump::key   key = rObj.get_key();
//             CMemDump::value val = rObj.get_data();
//             int y = val.first;
//             
//             CString line = L"<path stroke=\"";
//             line += s_strColor[((TMLParam)key.first & (TMLParam)0xff) / 18];
//             line += "\" d=\"M0 ";
//             line += TM_L2T(y * h / m_nMaxY);
//             int x = xOffset + 1;
//             int y2 = y;
//             for (CObjTimeList::recorder rNext(rTime); ++rNext; ++x)
//             {
//                 CMemDump::CObjMap::recorder rObj2(rNext.get_data());
//                 rObj2.find(key);
//                 if (!rObj2)
//                     break;
//                 y2 = rObj2.get_data().first;
//                 rObj2.erase();
//                 if (y2 == y)
//                     continue;
//                 y2 = y;
//                 _SVG_LineTo(line, x, y2 * h / m_nMaxY);
//             }
//             _SVG_LineTo(line, x, y2 * h / m_nMaxY);
//             line += L"\" />\r\n";
//             strSvg += line;
//             tm_setmax(yMax, y2);
//         }
//     }
// 
//     strSvg += "</svg>";
//     m_nMaxY = (yMax + 1) * 1.1;
// 
//     CComPtr<IStream> stream;
//     StreamCreateFromStaticMem(strSvg.data(), strSvg.length(), &stream);
//     m_tex.Release();
//     m_tex.CreateFromStream(stream);
// 
//     Invalidate();
}
