/**
 *  @file
 *  @brief  layout
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2012.02.29
 *
 *  Copyright (C) 2012 miragekiller                               
 */

#pragma once

#ifndef LAYOUT_H
#define LAYOUT_H

#include "uiwidget.h"


TM_BGN_NAMESPACE

void LayoutChilds(HTUI hParent, const CTuiRect& rcOldP, const CTuiRect& rcNewP);

// is childs have ALIGN_HEADER;
bool HaveChildAlignScroll(HTUI hParent);

// void LayoutDeltaSize(TMULong dwLayOut, CTuiRect& rc, GXDips dx, GXDips dy);

// 
// // INT_MAX
// // INT_MIN
// struct CLinear
// {
//     enum EType { e_pixel, e_percent };
//     enum { eLinearMax = 1 << 30 };
//     EType   m_eType     : 1;
//     TMInt32 m_nValue    : 31;
// };
// 
// enum EMode
// {
//     e_anchorLeft,
//     e_anchorTop,
// 
// };
// 
// struct LO
// {
//     CTuiRect rcMargin;
//     TMUInt32 dwMode;
// };
// 
// inline int PropGetType(const LO& bd) { return SS_SIZE; }
// void PropFromText(LO& bd, const CXmlTag& xml);
// CString PropToText(const LO& bd);
// 
// 
// struct LayoutBase
// {
//     TMUInt16 dwType; // leftright, leftwidth, widthright, rightleft
//     TMUInt8 bColspan; // for table cell
//     TMUInt8 bRowspan; // for table cell
// };
// 
// struct LayoutAlign : public LayoutBase
// {
// };
// 
// struct LayoutAnchor : public LayoutBase
// {
// };
// 
// struct LayoutInline : public LayoutBase
// {
// };
// 
// struct LayoutRow : public LayoutBase
// {
// };
// 
// struct LayoutCell : public LayoutBase
// {
// 
// };
// 
// 
// struct CPosSet
// {
//     enum E_display
//     {
//         e_dp_inline = 0, // 默认。此元素会被显示为内联元素，元素前后没有换行符。 
//         e_dp_block  = 0x0001, // 此元素将显示为块级元素，此元素前后会带有换行符。 
//         e_dp_inline_block, // 行内块元素。（CSS2.1 新增的值） 
//         e_dp_list_item, // 此元素会作为列表显示。 
//         e_dp_run_in, // 此元素会根据上下文作为块级元素或内联元素显示。 
//         //     e_dp_compact, // CSS 中有值 compact，不过由于缺乏广泛支持，已经从 CSS2.1 中删除。 
//         //     e_dp_marker, // CSS 中有值 marker，不过由于缺乏广泛支持，已经从 CSS2.1 中删除。 
//         e_dp_table, // 此元素会作为块级表格来显示（类似 <table>），表格前后带有换行符。 
//         e_dp_inline_table, // 此元素会作为内联表格来显示（类似 <table>），表格前后没有换行符。 
//         e_dp_table_row_group, // 此元素会作为一个或多个行的分组来显示（类似 <tbody>）。 
//         e_dp_table_header_group, // 此元素会作为一个或多个行的分组来显示（类似 <thead>）。 
//         e_dp_table_footer_group, // 此元素会作为一个或多个行的分组来显示（类似 <tfoot>）。 
//         e_dp_table_row, // 此元素会作为一个表格行显示（类似 <tr>）。 
//         e_dp_table_column_group, // 此元素会作为一个或多个列的分组来显示（类似 <F>）。 
//         e_dp_table_column, //  此元素会作为一个单元格列显示（类似 <col>） 
//         e_dp_table_cell, // 此元素会作为一个表格单元格显示（类似 <td> 和 <th>） 
//         e_dp_table_caption, // 此元素会作为一个表格标题显示（类似 <caption>） 
//         e_dp_inherit, // 
//         e_dp_none, // 此元素不会被显示。
// 
//         e_dpMask = 0x000f,
//     };
// 
//     enum E_position
//     {
//         e_ps_static, // 默认值。没有定位，元素出现在正常的流中（忽略 top, bottom, left, right 或者 z-index 声明）。 
//         e_ps_relative   = 0x0010, // 生成相对定位的元素，相对于其正常位置进行定位。因此，"left:20" 会向元素的 LEFT 位置添加 20 像素。
//         e_ps_absolute   = 0x0020, // 生成绝对定位的元素，相对于 static 定位以外的第一个父元素进行定位。元素的位置通过 "left", "top", "right" 以及 "bottom" 属性进行规定。
//         e_ps_fixed      = 0x0030, // 生成绝对定位的元素，相对于浏览器窗口进行定位。元素的位置通过 "left", "top", "right" 以及 "bottom" 属性进行规定。
//         e_ps_inherit    = 0x0040, // 规定应该从父元素继承 position 属性的值。 
//         e_ps_mask       = 0x00f0,
//     };
// 
//     enum E_float
//     {
//         e_flt_none, // 默认值。元素不浮动，并会显示在其在文本中出现的位置。 
//         e_flt_left      = 0x0100, // 元素向左浮动。 
//         e_flt_right     = 0x0200, // 元素向右浮动。 
//         e_flt_inherit   = 0x0300, // 规定应该从父元素继承 float 属性的值。 
//         e_flt_mask      = 0x0f00,
//     };
//     
//     enum E_clear
//     {
//         e_clr_none, // 默认值。允许浮动元素出现在两侧。 
//         e_clr_left      = 0x1000, // 在左侧不允许浮动元素。 
//         e_clr_right     = 0x2000, // 在右侧不允许浮动元素。 
//         e_clr_both      = 0x3000, // 在左右两侧均不允许浮动元素。 
//         e_clr_inherit   = 0x4000, // 规定应该从父元素继承 clear 属性的值。 
//         e_clr_mask      = 0xf000,
//     };
// 
//     enum E_linear
//     {
//         e_ln_left           = 0x0010000,
//         e_ln_top            = 0x0020000,
//         e_ln_width          = 0x0040000,
//         e_ln_height         = 0x0080000,
//         e_ln_right          = 0x0100000,
//         e_ln_bottom         = 0x0200000,
//     };
// 
//     TMUInt32    m_dwFlag;
//     CLinear     m_nleft;
//     CLinear     m_nTop;
//     CLinear     m_nRight;
//     CLinear     m_nBottom;
// };

// inline int PropGetType(const CLinear& rc) { return SS_SIZE; }
// void PropFromText(CLinear& aff, const CXmlTag& xml);
// CString PropToText(const CLinear& aff);

// inline int PropGetType(const CPosSet& rc) { return SS_SIZE; }
// void PropFromText(CPosSet& var, const CXmlTag& xml);
// CString PropToText(const CPosSet& var);



TM_END_NAMESPACE

#endif // LAYOUT_H
