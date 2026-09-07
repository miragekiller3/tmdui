/*
 *  @file
 *  @brief  debug function for cccessible
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.4.12
 *
 *  Copyright (C) 2021 miragekiller
 */

#pragma once

#ifndef TMFORMATACC_H
#define TMFORMATACC_H

#include "tmstring.h"
#include "tmduiacc.h"
#include "tmcom_win.h"
#include "tmplatform_win.h"
#include <oleacc.h>
#include "ia2-api\ia2_api_all.h"

#pragma comment(lib, "Oleacc.lib")

#include "uiautomation.h"


#ifdef __uiautomationcore_h__


TM_INLINE LPCTSTR DbgFormatOropertyId(PROPERTYID id)
{
#   define FORAMT_CASE(m) case m: return _T(#m); 
    
    switch (id)
    {
        FORAMT_CASE(UIA_RuntimeIdPropertyId); //    =    30000;
        FORAMT_CASE(UIA_BoundingRectanglePropertyId); //    =    30001;
        FORAMT_CASE(UIA_ProcessIdPropertyId); //    =    30002;
        FORAMT_CASE(UIA_ControlTypePropertyId); //    =    30003;
        FORAMT_CASE(UIA_LocalizedControlTypePropertyId); //    =    30004;
        FORAMT_CASE(UIA_NamePropertyId); //    =    30005;
        FORAMT_CASE(UIA_AcceleratorKeyPropertyId); //    =    30006;
        FORAMT_CASE(UIA_AccessKeyPropertyId); //    =    30007;
        FORAMT_CASE(UIA_HasKeyboardFocusPropertyId); //    =    30008;
        FORAMT_CASE(UIA_IsKeyboardFocusablePropertyId); //    =    30009;
        FORAMT_CASE(UIA_IsEnabledPropertyId); //    =    30010;
        FORAMT_CASE(UIA_AutomationIdPropertyId); //    =    30011;
        FORAMT_CASE(UIA_ClassNamePropertyId); //    =    30012;
        FORAMT_CASE(UIA_HelpTextPropertyId); //    =    30013;
        FORAMT_CASE(UIA_ClickablePointPropertyId); //    =    30014;
        FORAMT_CASE(UIA_CulturePropertyId); //    =    30015;
        FORAMT_CASE(UIA_IsControlElementPropertyId); //    =    30016;
        FORAMT_CASE(UIA_IsContentElementPropertyId); //    =    30017;
        FORAMT_CASE(UIA_LabeledByPropertyId); //    =    30018;
        FORAMT_CASE(UIA_IsPasswordPropertyId);  //    =    30019;
        FORAMT_CASE(UIA_NativeWindowHandlePropertyId);  //    =    30020;
        FORAMT_CASE(UIA_ItemTypePropertyId);  //    =    30021;
        FORAMT_CASE(UIA_IsOffscreenPropertyId);  //    =    30022;
        FORAMT_CASE(UIA_OrientationPropertyId);  //    =    30023;
        FORAMT_CASE(UIA_FrameworkIdPropertyId);  //    =    30024;
        FORAMT_CASE(UIA_IsRequiredForFormPropertyId);  //    =    30025;
        FORAMT_CASE(UIA_ItemStatusPropertyId);  //    =    30026;
        FORAMT_CASE(UIA_IsDockPatternAvailablePropertyId);  //    =    30027;
        FORAMT_CASE(UIA_IsExpandCollapsePatternAvailablePropertyId);  //    =    30028;
        FORAMT_CASE(UIA_IsGridItemPatternAvailablePropertyId);  //    =    30029;
        FORAMT_CASE(UIA_IsGridPatternAvailablePropertyId);  //    =    30030;
        FORAMT_CASE(UIA_IsInvokePatternAvailablePropertyId);  //    =    30031;
        FORAMT_CASE(UIA_IsMultipleViewPatternAvailablePropertyId);  //    =    30032;
        FORAMT_CASE(UIA_IsRangeValuePatternAvailablePropertyId);  //    =    30033;
        FORAMT_CASE(UIA_IsScrollPatternAvailablePropertyId);  //    =    30034;
        FORAMT_CASE(UIA_IsScrollItemPatternAvailablePropertyId);  //    =    30035;
        FORAMT_CASE(UIA_IsSelectionItemPatternAvailablePropertyId);  //    =    30036;
        FORAMT_CASE(UIA_IsSelectionPatternAvailablePropertyId);  //    =    30037;
        FORAMT_CASE(UIA_IsTablePatternAvailablePropertyId);  //    =    30038;
        FORAMT_CASE(UIA_IsTableItemPatternAvailablePropertyId);  //    =    30039;
        FORAMT_CASE(UIA_IsTextPatternAvailablePropertyId);  //    =    30040;
        FORAMT_CASE(UIA_IsTogglePatternAvailablePropertyId);  //    =    30041;
        FORAMT_CASE(UIA_IsTransformPatternAvailablePropertyId);  //    =    30042;
        FORAMT_CASE(UIA_IsValuePatternAvailablePropertyId);  //    =    30043;
        FORAMT_CASE(UIA_IsWindowPatternAvailablePropertyId);  //    =    30044;
        FORAMT_CASE(UIA_ValueValuePropertyId);  //    =    30045;
        FORAMT_CASE(UIA_ValueIsReadOnlyPropertyId);  //    =    30046;
        FORAMT_CASE(UIA_RangeValueValuePropertyId);  //    =    30047;
        FORAMT_CASE(UIA_RangeValueIsReadOnlyPropertyId);  //    =    30048;
        FORAMT_CASE(UIA_RangeValueMinimumPropertyId);  //    =    30049;
        FORAMT_CASE(UIA_RangeValueMaximumPropertyId);  //    =    30050;
        FORAMT_CASE(UIA_RangeValueLargeChangePropertyId);  //    =    30051;
        FORAMT_CASE(UIA_RangeValueSmallChangePropertyId);  //    =    30052;
        FORAMT_CASE(UIA_ScrollHorizontalScrollPercentPropertyId);  //    =    30053;
        FORAMT_CASE(UIA_ScrollHorizontalViewSizePropertyId);  //    =    30054;
        FORAMT_CASE(UIA_ScrollVerticalScrollPercentPropertyId);  //    =    30055;
        FORAMT_CASE(UIA_ScrollVerticalViewSizePropertyId);  //    =    30056;
        FORAMT_CASE(UIA_ScrollHorizontallyScrollablePropertyId);  //    =    30057;
        FORAMT_CASE(UIA_ScrollVerticallyScrollablePropertyId);  //    =    30058;
        FORAMT_CASE(UIA_SelectionSelectionPropertyId);  //    =    30059;
        FORAMT_CASE(UIA_SelectionCanSelectMultiplePropertyId);  //    =    30060;
        FORAMT_CASE(UIA_SelectionIsSelectionRequiredPropertyId);  //    =    30061;
        FORAMT_CASE(UIA_GridRowCountPropertyId);  //    =    30062;
        FORAMT_CASE(UIA_GridColumnCountPropertyId);  //    =    30063;
        FORAMT_CASE(UIA_GridItemRowPropertyId);  //    =    30064;
        FORAMT_CASE(UIA_GridItemColumnPropertyId);  //    =    30065;
        FORAMT_CASE(UIA_GridItemRowSpanPropertyId);  //    =    30066;
        FORAMT_CASE(UIA_GridItemColumnSpanPropertyId);  //    =    30067;
        FORAMT_CASE(UIA_GridItemContainingGridPropertyId);  //    =    30068;
        FORAMT_CASE(UIA_DockDockPositionPropertyId);  //    =    30069;
        FORAMT_CASE(UIA_ExpandCollapseExpandCollapseStatePropertyId);  //    =    30070;
        FORAMT_CASE(UIA_MultipleViewCurrentViewPropertyId);  //    =    30071;
        FORAMT_CASE(UIA_MultipleViewSupportedViewsPropertyId);  //    =    30072;
        FORAMT_CASE(UIA_WindowCanMaximizePropertyId);  //    =    30073;
        FORAMT_CASE(UIA_WindowCanMinimizePropertyId);  //    =    30074;
        FORAMT_CASE(UIA_WindowWindowVisualStatePropertyId);  //    =    30075;
        FORAMT_CASE(UIA_WindowWindowInteractionStatePropertyId);  //    =    30076;
        FORAMT_CASE(UIA_WindowIsModalPropertyId);  //    =    30077;
        FORAMT_CASE(UIA_WindowIsTopmostPropertyId);  //    =    30078;
        FORAMT_CASE(UIA_SelectionItemIsSelectedPropertyId);  //    =    30079;
        FORAMT_CASE(UIA_SelectionItemSelectionContainerPropertyId);  //    =    30080;
        FORAMT_CASE(UIA_TableRowHeadersPropertyId);  //    =    30081;
        FORAMT_CASE(UIA_TableColumnHeadersPropertyId);  //    =    30082;
        FORAMT_CASE(UIA_TableRowOrColumnMajorPropertyId);  //    =    30083;
        FORAMT_CASE(UIA_TableItemRowHeaderItemsPropertyId);  //    =    30084;
        FORAMT_CASE(UIA_TableItemColumnHeaderItemsPropertyId);  //    =    30085;
        FORAMT_CASE(UIA_ToggleToggleStatePropertyId);  //    =    30086;
        FORAMT_CASE(UIA_TransformCanMovePropertyId);  //    =    30087;
        FORAMT_CASE(UIA_TransformCanResizePropertyId);  //    =    30088;
        FORAMT_CASE(UIA_TransformCanRotatePropertyId);  //    =    30089;
        FORAMT_CASE(UIA_IsLegacyIAccessiblePatternAvailablePropertyId);  //    =    30090;
        FORAMT_CASE(UIA_LegacyIAccessibleChildIdPropertyId);  //    =    30091;
        FORAMT_CASE(UIA_LegacyIAccessibleNamePropertyId);  //    =    30092;
        FORAMT_CASE(UIA_LegacyIAccessibleValuePropertyId);  //    =    30093;
        FORAMT_CASE(UIA_LegacyIAccessibleDescriptionPropertyId);  //    =    30094;
        FORAMT_CASE(UIA_LegacyIAccessibleRolePropertyId);  //    =    30095;
        FORAMT_CASE(UIA_LegacyIAccessibleStatePropertyId);  //    =    30096;
        FORAMT_CASE(UIA_LegacyIAccessibleHelpPropertyId);  //    =    30097;
        FORAMT_CASE(UIA_LegacyIAccessibleKeyboardShortcutPropertyId);  //    =    30098;
        FORAMT_CASE(UIA_LegacyIAccessibleSelectionPropertyId);  //    =    30099;
        FORAMT_CASE(UIA_LegacyIAccessibleDefaultActionPropertyId);  //    =    30100;
        FORAMT_CASE(UIA_AriaRolePropertyId);  //    =    30101;
        FORAMT_CASE(UIA_AriaPropertiesPropertyId);  //    =    30102;
        FORAMT_CASE(UIA_IsDataValidForFormPropertyId);  //    =    30103;
        FORAMT_CASE(UIA_ControllerForPropertyId);  //    =    30104;
        FORAMT_CASE(UIA_DescribedByPropertyId);  //    =    30105;
        FORAMT_CASE(UIA_FlowsToPropertyId);  //    =    30106;
        FORAMT_CASE(UIA_ProviderDescriptionPropertyId);  //    =    30107;
        FORAMT_CASE(UIA_IsItemContainerPatternAvailablePropertyId);  //    =    30108;
        FORAMT_CASE(UIA_IsVirtualizedItemPatternAvailablePropertyId);  //    =    30109;
        FORAMT_CASE(UIA_IsSynchronizedInputPatternAvailablePropertyId);  //    =    30110;

        FORAMT_CASE(UIA_OptimizeForVisualContentPropertyId);  //    =    30111;
        FORAMT_CASE(UIA_IsObjectModelPatternAvailablePropertyId);  //    =    30112;
        FORAMT_CASE(UIA_AnnotationAnnotationTypeIdPropertyId);  //    =    30113;
        FORAMT_CASE(UIA_AnnotationAnnotationTypeNamePropertyId);  //    =    30114;
        FORAMT_CASE(UIA_AnnotationAuthorPropertyId);  //    =    30115;
        FORAMT_CASE(UIA_AnnotationDateTimePropertyId);  //    =    30116;
        FORAMT_CASE(UIA_AnnotationTargetPropertyId);  //    =    30117;
        FORAMT_CASE(UIA_IsAnnotationPatternAvailablePropertyId);  //    =    30118;
        FORAMT_CASE(UIA_IsTextPattern2AvailablePropertyId);  //    =    30119;
        FORAMT_CASE(UIA_StylesStyleIdPropertyId);  //    =    30120;
        FORAMT_CASE(UIA_StylesStyleNamePropertyId);  //    =    30121;
        FORAMT_CASE(UIA_StylesFillColorPropertyId);  //    =    30122;
        FORAMT_CASE(UIA_StylesFillPatternStylePropertyId);  //    =    30123;
        FORAMT_CASE(UIA_StylesShapePropertyId);  //    =    30124;
        FORAMT_CASE(UIA_StylesFillPatternColorPropertyId);  //    =    30125;
        FORAMT_CASE(UIA_StylesExtendedPropertiesPropertyId);  //    =    30126;
        FORAMT_CASE(UIA_IsStylesPatternAvailablePropertyId);  //    =    30127;
        FORAMT_CASE(UIA_IsSpreadsheetPatternAvailablePropertyId);  //    =    30128;
        FORAMT_CASE(UIA_SpreadsheetItemFormulaPropertyId);  //    =    30129;
        FORAMT_CASE(UIA_SpreadsheetItemAnnotationObjectsPropertyId);  //    =    30130;
        FORAMT_CASE(UIA_SpreadsheetItemAnnotationTypesPropertyId);  //    =    30131;
        FORAMT_CASE(UIA_IsSpreadsheetItemPatternAvailablePropertyId);  //    =    30132;
        FORAMT_CASE(UIA_Transform2CanZoomPropertyId);  //    =    30133;
        FORAMT_CASE(UIA_IsTransformPattern2AvailablePropertyId);  //    =    30134;
        FORAMT_CASE(UIA_LiveSettingPropertyId);  //    =    30135;
        FORAMT_CASE(UIA_IsTextChildPatternAvailablePropertyId);  //    =    30136;
        FORAMT_CASE(UIA_IsDragPatternAvailablePropertyId);  //    =    30137;
        FORAMT_CASE(UIA_DragIsGrabbedPropertyId);  //    =    30138;
        FORAMT_CASE(UIA_DragDropEffectPropertyId);  //    =    30139;
        FORAMT_CASE(UIA_DragDropEffectsPropertyId);  //    =    30140;
        FORAMT_CASE(UIA_IsDropTargetPatternAvailablePropertyId);  //    =    30141;
        FORAMT_CASE(UIA_DropTargetDropTargetEffectPropertyId);  //    =    30142;
        FORAMT_CASE(UIA_DropTargetDropTargetEffectsPropertyId);  //    =    30143;
        FORAMT_CASE(UIA_DragGrabbedItemsPropertyId);  //    =    30144;
        FORAMT_CASE(UIA_Transform2ZoomLevelPropertyId);  //    =    30145;
        FORAMT_CASE(UIA_Transform2ZoomMinimumPropertyId);  //    =    30146;
        FORAMT_CASE(UIA_Transform2ZoomMaximumPropertyId);  //    =    30147;
        FORAMT_CASE(UIA_FlowsFromPropertyId);  //    =    30148;
        FORAMT_CASE(UIA_IsTextEditPatternAvailablePropertyId);  //    =    30149;
        FORAMT_CASE(UIA_IsPeripheralPropertyId);  //    =    30150;
        FORAMT_CASE(UIA_IsCustomNavigationPatternAvailablePropertyId);  //    =    30151;
        FORAMT_CASE(UIA_PositionInSetPropertyId);  //    =    30152;
        FORAMT_CASE(UIA_SizeOfSetPropertyId);  //    =    30153;
        FORAMT_CASE(UIA_LevelPropertyId);  //    =    30154;
        FORAMT_CASE(UIA_AnnotationTypesPropertyId);  //    =    30155;
        FORAMT_CASE(UIA_AnnotationObjectsPropertyId);  //    =    30156;
        FORAMT_CASE(UIA_LandmarkTypePropertyId);  //    =    30157;
        FORAMT_CASE(UIA_LocalizedLandmarkTypePropertyId);  //    =    30158;
        FORAMT_CASE(UIA_FullDescriptionPropertyId);  //    =    30159;
        FORAMT_CASE(UIA_FillColorPropertyId);  //    =    30160;
        FORAMT_CASE(UIA_OutlineColorPropertyId);  //    =    30161;
        FORAMT_CASE(UIA_FillTypePropertyId);  //    =    30162;
        FORAMT_CASE(UIA_VisualEffectsPropertyId);  //    =    30163;
        FORAMT_CASE(UIA_OutlineThicknessPropertyId);  //    =    30164;
        FORAMT_CASE(UIA_CenterPointPropertyId);  //    =    30165;
        FORAMT_CASE(UIA_RotationPropertyId);  //    =    30166;
        FORAMT_CASE(UIA_SizePropertyId);  //    =    30167;
        FORAMT_CASE(UIA_IsSelectionPattern2AvailablePropertyId);  //    =    30168;
        FORAMT_CASE(UIA_Selection2FirstSelectedItemPropertyId);  //    =    30169;
        FORAMT_CASE(UIA_Selection2LastSelectedItemPropertyId);  //    =    30170;
        FORAMT_CASE(UIA_Selection2CurrentSelectedItemPropertyId);  //    =    30171;
        FORAMT_CASE(UIA_Selection2ItemCountPropertyId);  //    =    30172;
    }
#undef FORAMT_CASE
    
    static TM::CStdStr s_str;
    s_str.Format(L"%d", id);
    return s_str;
}

#endif // ifdef __uiautomationcore_h__


TM_INLINE LPCTSTR DbgFormatRole(long role)
{
//#   define FORAMT_CASE(m) case m: return _T(#m); 
    
#   define FORAMT_CASE(m) case ROLE_SYSTEM_##m: return TM_T(#m); 
#   define FORAMT_CAS2(m) case IA2_ROLE_##m: return TM_T(#m) TM_T("2"); 


    switch (role)
    {
        FORAMT_CASE(TITLEBAR);//            0x00000001
        FORAMT_CASE(MENUBAR);//             0x00000002
        FORAMT_CASE(SCROLLBAR);//           0x00000003
        FORAMT_CASE(GRIP);//                0x00000004
        FORAMT_CASE(SOUND);//               0x00000005
        FORAMT_CASE(CURSOR);//              0x00000006
        FORAMT_CASE(CARET);//               0x00000007
        FORAMT_CASE(ALERT);//               0x00000008
        FORAMT_CASE(WINDOW);//              0x00000009
        FORAMT_CASE(CLIENT);//              0x0000000A
        FORAMT_CASE(MENUPOPUP);//           0x0000000B
        FORAMT_CASE(MENUITEM);//            0x0000000C
        FORAMT_CASE(TOOLTIP);//             0x0000000D
        FORAMT_CASE(APPLICATION);//         0x0000000E
        FORAMT_CASE(DOCUMENT);//            0x0000000F
        FORAMT_CASE(PANE);//                0x00000010
        FORAMT_CASE(CHART);//               0x00000011
        FORAMT_CASE(DIALOG);//              0x00000012
        FORAMT_CASE(BORDER);//              0x00000013
        FORAMT_CASE(GROUPING);//            0x00000014
        FORAMT_CASE(SEPARATOR);//           0x00000015
        FORAMT_CASE(TOOLBAR);//             0x00000016
        FORAMT_CASE(STATUSBAR);//           0x00000017
        FORAMT_CASE(TABLE);//               0x00000018
        FORAMT_CASE(COLUMNHEADER);//        0x00000019
        FORAMT_CASE(ROWHEADER);//           0x0000001A
        FORAMT_CASE(COLUMN);//              0x0000001B
        FORAMT_CASE(ROW);//                 0x0000001C
        FORAMT_CASE(CELL);//                0x0000001D
        FORAMT_CASE(LINK);//                0x0000001E
        FORAMT_CASE(HELPBALLOON);//         0x0000001F
        FORAMT_CASE(CHARACTER);//           0x00000020
        FORAMT_CASE(LIST);//                0x00000021
        FORAMT_CASE(LISTITEM);//            0x00000022
        FORAMT_CASE(OUTLINE);//             0x00000023
        FORAMT_CASE(OUTLINEITEM);//         0x00000024
        FORAMT_CASE(PAGETAB);//             0x00000025
        FORAMT_CASE(PROPERTYPAGE);//        0x00000026
        FORAMT_CASE(INDICATOR);//           0x00000027
        FORAMT_CASE(GRAPHIC);//             0x00000028
        FORAMT_CASE(STATICTEXT);//          0x00000029
        FORAMT_CASE(TEXT);//                0x0000002A  // Editable, selectable, etc.
        FORAMT_CASE(PUSHBUTTON);//          0x0000002B
        FORAMT_CASE(CHECKBUTTON);//         0x0000002C
        FORAMT_CASE(RADIOBUTTON);//         0x0000002D
        FORAMT_CASE(COMBOBOX);//            0x0000002E
        FORAMT_CASE(DROPLIST);//            0x0000002F
        FORAMT_CASE(PROGRESSBAR);//         0x00000030
        FORAMT_CASE(DIAL);//                0x00000031
        FORAMT_CASE(HOTKEYFIELD);//         0x00000032
        FORAMT_CASE(SLIDER);//              0x00000033
        FORAMT_CASE(SPINBUTTON);//          0x00000034
        FORAMT_CASE(DIAGRAM);//             0x00000035
        FORAMT_CASE(ANIMATION);//           0x00000036
        FORAMT_CASE(EQUATION);//            0x00000037
        FORAMT_CASE(BUTTONDROPDOWN);//      0x00000038
        FORAMT_CASE(BUTTONMENU);//          0x00000039
        FORAMT_CASE(BUTTONDROPDOWNGRID);//  0x0000003A
        FORAMT_CASE(WHITESPACE);//          0x0000003B
        FORAMT_CASE(PAGETABLIST);//         0x0000003C
        FORAMT_CASE(CLOCK);//               0x0000003D

        FORAMT_CAS2(CANVAS);// = 0x401,
        FORAMT_CAS2(CAPTION);// = 0x402,
        FORAMT_CAS2(CHECK_MENU_ITEM);// = 0x403,
        FORAMT_CAS2(COLOR_CHOOSER);// = 0x404,
        FORAMT_CAS2(DATE_EDITOR);// = 0x405,
        FORAMT_CAS2(DESKTOP_ICON);// = 0x406,
        FORAMT_CAS2(DESKTOP_PANE);// = 0x407,
        FORAMT_CAS2(DIRECTORY_PANE);// = 0x408,
        FORAMT_CAS2(EDITBAR);// = 0x409,
        FORAMT_CAS2(EMBEDDED_OBJECT);// = 0x40a,
        FORAMT_CAS2(ENDNOTE);// = 0x40b,
        FORAMT_CAS2(FILE_CHOOSER);// = 0x40c,
        FORAMT_CAS2(FONT_CHOOSER);// = 0x40d,
        FORAMT_CAS2(FOOTER);// = 0x40e,
        FORAMT_CAS2(FOOTNOTE);// = 0x40f,
        FORAMT_CAS2(FORM);// = 0x410,
        FORAMT_CAS2(FRAME);// = 0x411,
        FORAMT_CAS2(GLASS_PANE);// = 0x412,
        FORAMT_CAS2(HEADER);// = 0x413,
        FORAMT_CAS2(HEADING);// = 0x414,
        FORAMT_CAS2(ICON);// = 0x415,
        FORAMT_CAS2(IMAGE_MAP);// = 0x416,
        FORAMT_CAS2(INPUT_METHOD_WINDOW);// = 0x417,
        FORAMT_CAS2(INTERNAL_FRAME);// = 0x418,
        FORAMT_CAS2(LABEL);// = 0x419,
        FORAMT_CAS2(LAYERED_PANE);// = 0x41a,
        FORAMT_CAS2(NOTE);// = 0x41b,
        FORAMT_CAS2(OPTION_PANE);// = 0x41c,
        FORAMT_CAS2(PAGE);// = 0x41d,
        FORAMT_CAS2(PARAGRAPH);// = 0x41e,
        FORAMT_CAS2(RADIO_MENU_ITEM);// = 0x41f,
        FORAMT_CAS2(REDUNDANT_OBJECT);// = 0x420,
        FORAMT_CAS2(ROOT_PANE);// = 0x421,
        FORAMT_CAS2(RULER);// = 0x422,
        FORAMT_CAS2(SCROLL_PANE);// = 0x423,
        FORAMT_CAS2(SECTION);// = 0x424,
        FORAMT_CAS2(SHAPE);// = 0x425,
        FORAMT_CAS2(SPLIT_PANE);// = 0x426,
        FORAMT_CAS2(TEAR_OFF_MENU);// = 0x427,
        FORAMT_CAS2(TERMINAL);// = 0x428,
        FORAMT_CAS2(TEXT_FRAME);// = 0x429,
        FORAMT_CAS2(TOGGLE_BUTTON);// = 0x42a,
        FORAMT_CAS2(VIEW_PORT);// = 0x42b,
        FORAMT_CAS2(COMPLEMENTARY_CONTENT);// = 0x42c,
        FORAMT_CAS2(LANDMARK);// = 0x42d
    }
#undef FORAMT_CASE
#undef FORAMT_CAS2
    
    static TM::CStdStr s_str;
    s_str.Format(L"%X", role);
    return s_str;
}

TM_INLINE LPCTSTR DbgFormatState(long state)
{
    static TM::CStdStr s_str;
    s_str.clear();

#   define FORAMT_CASE(x)  if (state & (STATE_SYSTEM_##x)) { s_str += L";"; s_str += TM_T(#x); }

    if (state == STATE_SYSTEM_VALID)
        return TM_T("VALID");
    
    FORAMT_CASE(UNAVAILABLE);//        0x00000001  // Disabled
    FORAMT_CASE(SELECTED);//           0x00000002
    FORAMT_CASE(FOCUSED);//            0x00000004
    FORAMT_CASE(PRESSED);//            0x00000008
    FORAMT_CASE(CHECKED);//            0x00000010
    FORAMT_CASE(MIXED);//              0x00000020  // 3-state checkbox or toolbar button
    FORAMT_CASE(READONLY);//           0x00000040
    FORAMT_CASE(HOTTRACKED);//         0x00000080
    FORAMT_CASE(DEFAULT);//            0x00000100
    FORAMT_CASE(EXPANDED);//           0x00000200
    FORAMT_CASE(COLLAPSED);//          0x00000400
    FORAMT_CASE(BUSY);//               0x00000800
    FORAMT_CASE(FLOATING);//           0x00001000  // Children "owned" not "contained" by parent
    FORAMT_CASE(MARQUEED);//           0x00002000
    FORAMT_CASE(ANIMATED);//           0x00004000
    FORAMT_CASE(INVISIBLE);//          0x00008000
    FORAMT_CASE(OFFSCREEN);//          0x00010000
    FORAMT_CASE(SIZEABLE);//           0x00020000
    FORAMT_CASE(MOVEABLE);//           0x00040000
    FORAMT_CASE(SELFVOICING);//        0x00080000
    FORAMT_CASE(FOCUSABLE);//          0x00100000
    FORAMT_CASE(SELECTABLE);//         0x00200000
    FORAMT_CASE(LINKED);//             0x00400000
    FORAMT_CASE(TRAVERSED);//          0x00800000
    FORAMT_CASE(MULTISELECTABLE);//    0x01000000  // Supports multiple selection
    FORAMT_CASE(EXTSELECTABLE);//      0x02000000  // Supports extended selection
    FORAMT_CASE(ALERT_LOW);//          0x04000000  // This information is of low priority
    FORAMT_CASE(ALERT_MEDIUM);//       0x08000000  // This information is of medium priority
    FORAMT_CASE(ALERT_HIGH);//         0x10000000  // This information is of high priority

#undef FORAMT_CASE

    return s_str.IsEmpty() ? TM_T("0") : s_str.c_str() + 1;
}

TM_INLINE LPCTSTR DbgFormatState2(AccessibleStates state)
{
    static TM::CStdStr s_str;
    s_str.clear();
    
#   define FORAMT_CAS2(x)  if (state & (IA2_STATE_##x)) { s_str += L";"; s_str += TM_T(#x) TM_T("2"); }

    FORAMT_CAS2(ACTIVE);//	= 0x1,
    FORAMT_CAS2(ARMED);//	= 0x2,
    FORAMT_CAS2(DEFUNCT);//	= 0x4,
    FORAMT_CAS2(EDITABLE);//	= 0x8,
    FORAMT_CAS2(HORIZONTAL);//	= 0x10,
    FORAMT_CAS2(ICONIFIED);//	= 0x20,
    FORAMT_CAS2(INVALID_ENTRY);//	= 0x40,
    FORAMT_CAS2(MANAGES_DESCENDANTS);//	= 0x80,
    FORAMT_CAS2(MODAL);//	= 0x100,
    FORAMT_CAS2(MULTI_LINE);//	= 0x200,
    FORAMT_CAS2(OPAQUE);//	= 0x400,
    FORAMT_CAS2(REQUIRED);//	= 0x800,
    FORAMT_CAS2(SELECTABLE_TEXT);//	= 0x1000,
    FORAMT_CAS2(SINGLE_LINE);//	= 0x2000,
    FORAMT_CAS2(STALE);//	= 0x4000,
    FORAMT_CAS2(SUPPORTS_AUTOCOMPLETION);//	= 0x8000,
    FORAMT_CAS2(TRANSIENT);//	= 0x10000,
    FORAMT_CAS2(VERTICAL);//	= 0x20000,
    FORAMT_CAS2(CHECKABLE);//	= 0x40000,
    FORAMT_CAS2(PINNED);//	= 0x80000
    
#undef FORAMT_CAS2
    
    return s_str.IsEmpty() ? TM_T("0") : s_str.c_str() + 1;
}


TM_INLINE CComPtr<IUnknown> _DbgFormatTestInterface(TM::CStringW& str, IUnknown* p, IServiceProvider* ps, const IID& iid, LPCTSTR pName)
{
    TM::CStdStr strTmp;
    CComPtr<IUnknown> pOut;
    p->QueryInterface(iid, (void**)&pOut);
    if (pOut)
    {
        strTmp.Format(L"%s(%p);", pName, pOut);
        str += strTmp;
        return pOut;
    }
    if (!ps)
        return pOut;
    ps->QueryService(IID_IAccessible, iid, (void**)&pOut);
    if (pOut)
    {
        strTmp.Format(L"service:%s(%p);", pName, pOut);
        str += strTmp;
        return pOut;
    }
    ps->QueryService(iid, iid, (void**)&pOut);
    if (pOut)
    {
        strTmp.Format(L"service2:%s(%p);", pName, pOut);
        str += strTmp;
    }

    return pOut;
}

TM_INLINE void _DbgFormatQueryInterface(IServiceProvider* ps, const IID& iid, void** pOut)
{
    if (SUCCEEDED(ps->QueryService(IID_IAccessible, iid, pOut)))
        return;
    if (SUCCEEDED(ps->QueryService(iid, iid, pOut)))
        return;
}

// ps can be 0
TM_INLINE void _DbgFormatQueryInterface(IUnknown* p, IServiceProvider* ps, const IID& iid, void** pOut)
{
    if (SUCCEEDED(p->QueryInterface(iid, pOut)))
        return;
    if (ps)
        _DbgFormatQueryInterface(ps, iid, pOut);
    else
        _DbgFormatQueryInterface(CComQIPtr<IServiceProvider>(p), iid, pOut);
}



// Chrome actually has support for iAccessible, but it is not turned on by default and needs to be operated manually on the following page.
// chrome://accessibility/
// firefox: about:config -> accessibility.AOM.enabled
// As of Windows 10.0.1903, Windows translates IAccessible2 into UIA

// https://docs.microsoft.com/zh-tw/windows/win32/winauto/using-queryservice-to-expose-a-native-object-model-interface-for-an-iaccessible-object
// support IHTMLElement2.

// Braille fails to update when moving the caret?

// https://wiki.linuxfoundation.org/accessibility/iaccessible2/comproxydll

// https://github.com/microsoft/Microsoft-UI-UIAutomation
TM_INLINE TM::CStringW DbgFormatAcc2(IUnknown* p, IServiceProvider* ps)
{
    TM::CStringW strOut;
    CComPtr<IAccessible2> p2;
    _DbgFormatQueryInterface(p, ps, TM_UUID_OF(IAccessible2), (void**)&p2);
    //p->QueryService(TM_UUID_OF(IAccessible2), &p2);
    if (p2)
    {
        TM::CStringW str;
        long nRelations = 0;
        HRESULT hr = p2->get_nRelations(&nRelations);
        str.Format(L"IAccessible2::get_nRelations(%d), ret:%X\r\n", nRelations, hr);
        strOut += str;

        strOut += L"relations:";
        for (int i = 0; i < nRelations; ++i)
        {
            CComPtr<IAccessibleRelation> relation;
            p2->get_relation(i, &relation);
            CComBSTR relationType, localizedRelationType;
            long nTargets = 0;
            CComPtr<IUnknown> target;
            if (relation)
            {
                relation->get_relationType(&relationType);
                relation->get_localizedRelationType(&localizedRelationType);
                relation->get_nTargets(&nTargets);
                relation->get_target(0, &target);
            }
            str.Format(L"%s,%s,%d,%p;", (BSTR)relationType, (BSTR)localizedRelationType, nTargets, target.p);
            strOut += str;
        }
        strOut += L"\r\n";

        long role = 0;
        hr = p2->role(&role);
        str.Format(L"IAccessible2::role(%s), ret:%X\r\n", DbgFormatRole(role), hr);
        strOut += str;

        long groupLevel = 0;
        long similarItemsInGroup = 0;
        long positionInGroup = 0;
        hr = p2->get_groupPosition(&groupLevel, &similarItemsInGroup, &positionInGroup);
        str.Format(L"IAccessible2::get_groupPosition(%d,%d,%d), ret:%X\r\n", groupLevel, similarItemsInGroup, positionInGroup, hr);
        strOut += str;

        AccessibleStates states;
        hr = p2->get_states(&states);
        str.Format(L"IAccessible2::get_states(%s), ret:%X\r\n", DbgFormatState2(states), hr);
        strOut += str;

        CComBSTR extendedRole;
        hr = p2->get_extendedRole(&extendedRole);
        str.Format(L"IAccessible2::get_extendedRole(%s), ret:%X\r\n", (BSTR)extendedRole, hr);
        strOut += str;

        CComBSTR localizedExtendedRole;
        hr = p2->get_localizedExtendedRole(&localizedExtendedRole);
        str.Format(L"IAccessible2::get_localizedExtendedRole(%s), ret:%X\r\n", (BSTR)localizedExtendedRole, hr);
        strOut += str;

        long nExtendedStates = 0;
        hr = p2->get_nExtendedStates(&nExtendedStates);
        str.Format(L"IAccessible2::get_nExtendedStates(%d), ret:%X\r\n", nExtendedStates, hr);
        strOut += str;

        long uniqueID;
        hr = p2->get_uniqueID(&uniqueID);
        str.Format(L"IAccessible2::get_uniqueID(%X), ret:%X\r\n", uniqueID, hr);
        strOut += str;

        HWND windowHandle;
        hr = p2->get_windowHandle(&windowHandle);
        str.Format(L"IAccessible2::get_windowHandle(%p), ret:%X\r\n", windowHandle, hr);
        strOut += str;

        long indexInParent;
        hr = p2->get_indexInParent(&indexInParent);
        str.Format(L"IAccessible2::get_indexInParent(%d), ret:%X\r\n", indexInParent, hr);
        strOut += str;

        CComBSTR attributes;
        hr = p2->get_attributes(&attributes);
        str.Format(L"IAccessible2::get_attributes(%s), ret:%X\r\n", (BSTR)attributes, hr);
        strOut += str;
    }

    CComPtr<IAccessible2_2> p2_2;
    _DbgFormatQueryInterface(p, ps, TM_UUID_OF(IAccessible2_2), (void**)&p2_2);
//    p->QueryService(TM_UUID_OF(IAccessible2_2), &p2_2);
    if (p2_2)
    {
        TM::CStringW str;
        HRESULT hr;

        CComPtr<IUnknown> accessible;
        long caretOffset = 0;
        hr = p2_2->get_accessibleWithCaret(&accessible, &caretOffset);
        str.Format(L"IAccessible2_2::get_accessibleWithCaret(%p,%d), ret:%X\r\n", accessible.p, caretOffset, hr);
        strOut += str;

        IUnknown* pt1[10] = {0};
        long nTargets = 10;
        hr = p2_2->get_relationTargetsOfType((BSTR)L"memberOf", 0, (IUnknown ***)&pt1, &nTargets);
        str.Format(L"IAccessible2_2::get_relationTargetsOfType(memberOf, %p, %d), ret:%X\r\n", pt1[0], nTargets, hr);
        strOut += str;

        IUnknown* pt2[10] = { 0 };
        nTargets = 10;
        hr = p2_2->get_relationTargetsOfType((BSTR)L"nodeChildOf", 0, (IUnknown ***)&pt2, &nTargets);
        str.Format(L"IAccessible2_2::get_relationTargetsOfType(nodeChildOf, %p, %d), ret:%X\r\n", pt2[0], nTargets, hr);
        strOut += str;
    }

    // IAccessibleApplication
    CComPtr<IAccessibleApplication> papp;
    _DbgFormatQueryInterface(p, ps, TM_UUID_OF(IAccessibleApplication), (void**)&papp);
//    p->QueryService(TM_UUID_OF(IAccessibleApplication), &papp);
    if (papp)
    {
        TM::CStringW str;
        HRESULT hr;
        
        CComBSTR name;
        hr = papp->get_appName(&name);
        str.Format(L"IAccessibleApplication::get_appName(%s), ret:%X\r\n", (BSTR)name, hr);
        strOut += str;

        CComBSTR version;
        hr = papp->get_appVersion(&version);
        str.Format(L"IAccessibleApplication::get_appVersion(%s), ret:%X\r\n", (BSTR)version, hr);
        strOut += str;

        name.Empty();
        hr = papp->get_toolkitName(&name);
        str.Format(L"IAccessibleApplication::get_toolkitName(%s), ret:%X\r\n", (BSTR)name, hr);
        strOut += str;

        version.Empty();
        hr = papp->get_toolkitVersion(&version);
        str.Format(L"IAccessibleApplication::get_toolkitVersion(%s), ret:%X\r\n", (BSTR)version, hr);
        strOut += str;
    }

    CComPtr<IAccessibleText> ptext;
    _DbgFormatQueryInterface(p, ps, TM_UUID_OF(IAccessibleText), (void**)&ptext);
//    p->QueryService(TM_UUID_OF(IAccessibleText), &ptext);
    if (ptext)
    {
        TM::CStringW str;
        
        long startOffset;
        long endOffset;
        CComBSTR textAttributes;
        HRESULT hr = ptext->get_attributes(0, &startOffset, &endOffset, &textAttributes);
        str.Format(L"IAccessibleText::get_attributes(%d,%d,%s), ret:%X\r\n", startOffset, endOffset, (BSTR)textAttributes, hr);
        strOut += str;

        long offset;
        hr = ptext->get_caretOffset(&offset);
        str.Format(L"IAccessibleText::get_caretOffset(%d), ret:%X\r\n", offset, hr);
        strOut += str;

        long x,y,w,h;
        hr = ptext->get_characterExtents(0, IA2_COORDTYPE_SCREEN_RELATIVE, &x, &y, &w, &h);
        str.Format(L"IAccessibleText::get_characterExtents(%d,%d,%d,%d), ret:%X\r\n", x,y,w,h, hr);
        strOut += str;

        long nSelections;
        hr = ptext->get_nSelections(&nSelections);
        str.Format(L"IAccessibleText::get_nSelections(%d), ret:%X\r\n", nSelections, hr);
        strOut += str;

        offset = 0;
        hr = ptext->get_offsetAtPoint(10, 10, IA2_COORDTYPE_PARENT_RELATIVE, &offset);
        str.Format(L"IAccessibleText::get_offsetAtPoint(%d), ret:%X\r\n", offset, hr);
        strOut += str;

        CComBSTR text;
        hr = ptext->get_text(startOffset, endOffset, &text);
        str.Format(L"IAccessibleText::get_text(%s), ret:%X\r\n", (BSTR)text, hr);
        strOut += str;

        startOffset = 0;
        endOffset = 0;
        hr = ptext->get_selection(0, &startOffset, &endOffset);
        str.Format(L"IAccessibleText::get_selection(%d,%d), ret:%X\r\n", startOffset, endOffset, hr);
        strOut += str;

        text.Empty();
        startOffset = 0;
        endOffset = 0;
        hr = ptext->get_textBeforeOffset(0, IA2_TEXT_BOUNDARY_WORD, &startOffset, &endOffset, &text);
        str.Format(L"IAccessibleText::get_textBeforeOffset(%d,%d,%s), ret:%X\r\n", startOffset, endOffset, (BSTR)text, hr);
        strOut += str;

        text.Empty();
        startOffset = 0;
        endOffset = 0;
        hr = ptext->get_textAfterOffset(0, IA2_TEXT_BOUNDARY_WORD, &startOffset, &endOffset, &text);
        str.Format(L"IAccessibleText::get_textAfterOffset(%d,%d,%s), ret:%X\r\n", startOffset, endOffset, (BSTR)text, hr);
        strOut += str;

        text.Empty();
        startOffset = 0;
        endOffset = 0;
        hr = ptext->get_textAtOffset(0, IA2_TEXT_BOUNDARY_WORD, &startOffset, &endOffset, &text);
        str.Format(L"IAccessibleText::get_textAtOffset(%d,%d,%s), ret:%X\r\n", startOffset, endOffset, (BSTR)text, hr);
        strOut += str;

        long nCharacters;
        hr = ptext->get_nCharacters(&nCharacters);
        str.Format(L"IAccessibleText::get_nCharacters(%d), ret:%X\r\n", nCharacters, hr);
        strOut += str;
    }

    CComPtr<IAccessibleAction> pAction;
    _DbgFormatQueryInterface(p, ps, TM_UUID_OF(IAccessibleAction), (void**)&pAction);
    if (pAction)
    {
        TM::CStringW str;
        
        long nActions = 0;
        HRESULT hr = pAction->nActions(&nActions);
        str.Format(L"IAccessibleAction::nActions(%d), ret:%X\r\n", nActions, hr);
        strOut += str;
        
        tm_for(int i = 0; i < nActions; ++i)
        {
            CComBSTR bstr;
            hr = pAction->get_description(i, &bstr);
            str.Format(L"IAccessibleAction::get_description(%d, %s), ret:%X\r\n", i, (BSTR)bstr, hr);
            strOut += str;
        }

        tm_for(int i = 0; i < nActions; ++i)
        {
            CComBSTR bstr;
            BSTR* bb = &bstr;
            long kb;
            hr = pAction->get_keyBinding(i, 1, &bb, &kb);
            str.Format(L"IAccessibleAction::get_keyBinding(%d, %s, %d), ret:%X\r\n", i, (BSTR)bstr, kb, hr);
            strOut += str;
        }

        tm_for(int i = 0; i < nActions; ++i)
        {
            CComBSTR bstr;
            hr = pAction->get_name(i, &bstr);
            str.Format(L"IAccessibleAction::get_name(%d, %s), ret:%X\r\n", i, (BSTR)bstr, hr);
            strOut += str;
        }

        tm_for(int i = 0; i < nActions; ++i)
        {
            CComBSTR bstr;
            hr = pAction->get_localizedName(i, &bstr);
            str.Format(L"IAccessibleAction::get_localizedName(%d, %s), ret:%X\r\n", i, (BSTR)bstr, hr);
            strOut += str;
        }
    }

    return strOut;
}


TM_INLINE LPCTSTR DbgFormatAccId(IAccessible* acc, IServiceProvider* ps)
{
    static TM::CStdStr s_str;

    CComPtr<IAccIdentity> accId;
    _DbgFormatQueryInterface(acc, ps, TM_UUID_OF(IAccIdentity), (void**)&accId);
    if (!accId)
        return 0;
    BYTE* idString = 0;
    DWORD idLen = 0;
    HRESULT res = accId->GetIdentityString(CHILDID_SELF, &idString, &idLen);
    if (res != S_OK || !idString)
        return L"GetIdentityString fail";
    static CComPtr<IAccPropServices> s_accPropServices;
    if (!s_accPropServices) {
        // Only retrieve this the first time it's needed.
        MIDL_INTERFACE("b5f8350b-0548-48b1-a6ee-88bd00b4a5e7") _CLSID_AccPropServices;
        if (s_accPropServices.CoCreateInstance(_uuidof(_CLSID_AccPropServices), NULL, CLSCTX_SERVER) != S_OK) {
            CoTaskMemFree(idString);
            return L"IAccPropServices fail";
        }
    }
    HWND hwnd;
    DWORD objId;
    DWORD childId;
    res = s_accPropServices->DecomposeHwndIdentityString(idString, idLen, &hwnd, &objId, &childId);
    CoTaskMemFree(idString);
    if (res != S_OK)
        return L"DecomposeHwndIdentityString fail";
    s_str.Format(L"(%s),%x,%d", DbgFormatWindow(hwnd), objId, childId);
    return s_str;
}

TM_INLINE VARTYPE DbgIAccessibleGetRuntimeId(SAFEARRAY* sa, int& type, int& id)
{
    if (!sa)
        return VT_EMPTY;
    VARTYPE vt = VT_EMPTY;
    SafeArrayGetVartype(sa, &vt);
    if (vt== VT_I4)
    {
        long x = 0, y = 1;
        SafeArrayGetElement(sa, &x, &type);
        SafeArrayGetElement(sa, &y, &id);
    }
    SafeArrayDestroy(sa);
    return vt;
}

#ifdef __IRawElementProviderSimple_INTERFACE_DEFINED__

TM_INLINE VARTYPE DbgIAccessibleGetRuntimeId(HUIANODE hnode, int& type, int& id)
{
    DECLARE_DLL_FUNC(dll_Uiautomationcore, HRESULT, WINAPI, UiaGetRuntimeId, (HUIANODE hnode, SAFEARRAY ** pruntimeId));
    if (!_UiaGetRuntimeId)
        return VT_EMPTY;
    SAFEARRAY* sa = 0;
    _UiaGetRuntimeId(hnode, &sa);
    return DbgIAccessibleGetRuntimeId(sa, type, id);
}

TM_INLINE VARTYPE DbgIAccessibleGetRuntimeId(IRawElementProviderFragment* provider, int& type, int& id)
{
    SAFEARRAY* var = 0;
    provider->GetRuntimeId(&var);
    return DbgIAccessibleGetRuntimeId(var, type, id);
}

TM_INLINE VARTYPE DbgIAccessibleGetRuntimeId(IRawElementProviderSimple* provider, int& type, int& id)
{
    VARIANT var;
    VariantInit(&var);
    provider->GetPropertyValue(UIA_RuntimeIdPropertyId, &var);
    if ((var.vt & VT_ARRAY) && (var.vt & VT_I4) && var.parray)
        return DbgIAccessibleGetRuntimeId(var.parray, type, id);

    CComQIPtr<IRawElementProviderFragment> frag(provider);
    if (frag)
        return DbgIAccessibleGetRuntimeId(frag, type, id);

    DECLARE_DLL_FUNC(dll_Uiautomationcore, HRESULT, WINAPI, UiaNodeFromProvider, (IRawElementProviderSimple * pProvider, HUIANODE * phnode));
    if (!_UiaNodeFromProvider)
        return VT_EMPTY;
    
    HUIANODE hnode = 0;
    _UiaNodeFromProvider(provider, &hnode);
    if (hnode)
        return DbgIAccessibleGetRuntimeId(hnode, type, id);

    return VT_EMPTY;
}

TM_INLINE TMCStr DbgFormatUIAControlTypeId(long ControlTypeId)
{
#define FMT_CTID(x) case UIA_##x##ControlTypeId: return TM_T(#x)
    switch(ControlTypeId)
    {
        FMT_CTID(Button);  // 50000
        FMT_CTID(Calendar);  // ControlTypeId	=	50001;
        FMT_CTID(CheckBox);  // ControlTypeId	=	50002;
        FMT_CTID(ComboBox);  // ControlTypeId	=	50003;
        FMT_CTID(Edit);  // ControlTypeId	=	50004;
        FMT_CTID(Hyperlink);  // ControlTypeId	=	50005;
        FMT_CTID(Image);  // ControlTypeId	=	50006;
        FMT_CTID(ListItem);  // ControlTypeId	=	50007;
        FMT_CTID(List);  // ControlTypeId	=	50008;
        FMT_CTID(Menu);  // ControlTypeId	=	50009;
        FMT_CTID(MenuBar);  // ControlTypeId	=	50010;
        FMT_CTID(MenuItem);  // ControlTypeId	=	50011;
        FMT_CTID(ProgressBar);  // ControlTypeId	=	50012;
        FMT_CTID(RadioButton);  // ControlTypeId	=	50013;
        FMT_CTID(ScrollBar);  // ControlTypeId	=	50014;
        FMT_CTID(Slider);  // ControlTypeId	=	50015;
        FMT_CTID(Spinner);  // ControlTypeId	=	50016;
        FMT_CTID(StatusBar);  // ControlTypeId	=	50017;
        FMT_CTID(Tab);  // ControlTypeId	=	50018;
        FMT_CTID(TabItem);  // ControlTypeId	=	50019;
        FMT_CTID(Text);  // ControlTypeId	=	50020;
        FMT_CTID(ToolBar);  // ControlTypeId	=	50021;
        FMT_CTID(ToolTip);  // ControlTypeId	=	50022;
        FMT_CTID(Tree);  // ControlTypeId	=	50023;
        FMT_CTID(TreeItem);  // ControlTypeId	=	50024;
        FMT_CTID(Custom);  // ControlTypeId	=	50025;
        FMT_CTID(Group);  // ControlTypeId	=	50026;
        FMT_CTID(Thumb);  // ControlTypeId	=	50027;
        FMT_CTID(DataGrid);  // ControlTypeId	=	50028;
        FMT_CTID(DataItem);  // ControlTypeId	=	50029;
        FMT_CTID(Document);  // ControlTypeId	=	50030;
        FMT_CTID(SplitButton);  // ControlTypeId	=	50031;
        FMT_CTID(Window);  // ControlTypeId	=	50032;
        FMT_CTID(Pane);  // ControlTypeId	=	50033;
        FMT_CTID(Header);  // ControlTypeId	=	50034;
        FMT_CTID(HeaderItem);  // ControlTypeId	=	50035;
        FMT_CTID(Table);  // ControlTypeId	=	50036;
        FMT_CTID(TitleBar);  // ControlTypeId	=	50037;
        FMT_CTID(Separator);  // ControlTypeId	=	50038;
        FMT_CTID(SemanticZoom);  // ControlTypeId	=	50039;
        FMT_CTID(AppBar);  // ControlTypeId	=	50040;
    }
    return TM_T("??");
}

TM_INLINE void _DbgGetProperty(TM::CStringW& strOut, IRawElementProviderSimple* p, PROPERTYID propertyId, TMCStrW strProp)
{
    CComVariant var;
    HRESULT hr = p->GetPropertyValue(propertyId, &var);
    if (var.vt == VT_EMPTY)
        return;
    if (propertyId == UIA_ControlTypePropertyId)
        strOut.AppendFormat(L"GetPropertyValue(%s: %s)\r\n", strProp, DbgFormatUIAControlTypeId(var.lVal));
    else
    {
        VariantToString(var);
        strOut.AppendFormat(L"GetPropertyValue(%s: %s)\r\n", strProp, var.bstrVal);
    }
}

TM_INLINE TM::CStringW DbgFormatUIA(IRawElementProviderSimple* p)
{
    TM::CStringW str, strTmp;
    
    if (!p)
        return str;

    HRESULT hr;

    ProviderOptions option = (ProviderOptions)0;
    hr = p->get_ProviderOptions(&option);
    str.Format(L"get_ProviderOptions(%X), ret:%X\r\n", option, hr);

#define TM_GET_UIA_PROP(prop) _DbgGetProperty(str, p, prop, TM_T(#prop))

    TM_GET_UIA_PROP(UIA_RuntimeIdPropertyId);
    TM_GET_UIA_PROP(UIA_BoundingRectanglePropertyId); //	=	30001;
    TM_GET_UIA_PROP(UIA_ProcessIdPropertyId); //	=	30002;
    TM_GET_UIA_PROP(UIA_ControlTypePropertyId); //	=	30003;
    TM_GET_UIA_PROP(UIA_LocalizedControlTypePropertyId); //	=	30004;
    TM_GET_UIA_PROP(UIA_NamePropertyId); //	=	30005;
    TM_GET_UIA_PROP(UIA_AcceleratorKeyPropertyId); //	=	30006;
    TM_GET_UIA_PROP(UIA_AccessKeyPropertyId); //	=	30007;
    TM_GET_UIA_PROP(UIA_HasKeyboardFocusPropertyId); //	=	30008;
    TM_GET_UIA_PROP(UIA_IsKeyboardFocusablePropertyId); //	=	30009;
    TM_GET_UIA_PROP(UIA_IsEnabledPropertyId); //	=	30010;
    TM_GET_UIA_PROP(UIA_AutomationIdPropertyId); //	=	30011;
    TM_GET_UIA_PROP(UIA_ClassNamePropertyId);  //=	30012;
    TM_GET_UIA_PROP(UIA_HelpTextPropertyId);  //=	30013;
    TM_GET_UIA_PROP(UIA_ClickablePointPropertyId);  //=	30014;
    TM_GET_UIA_PROP(UIA_CulturePropertyId);  //=	30015;
    TM_GET_UIA_PROP(UIA_IsControlElementPropertyId);  //=	30016;
    TM_GET_UIA_PROP(UIA_IsContentElementPropertyId);  //=	30017;
    TM_GET_UIA_PROP(UIA_LabeledByPropertyId);  //=	30018;
    TM_GET_UIA_PROP(UIA_IsPasswordPropertyId);  //=	30019;
    TM_GET_UIA_PROP(UIA_NativeWindowHandlePropertyId);  //=	30020;
    TM_GET_UIA_PROP(UIA_ItemTypePropertyId);  //=	30021;
    TM_GET_UIA_PROP(UIA_IsOffscreenPropertyId);  //=	30022;
    TM_GET_UIA_PROP(UIA_OrientationPropertyId);  //=	30023;
    TM_GET_UIA_PROP(UIA_FrameworkIdPropertyId);  //=	30024;
    TM_GET_UIA_PROP(UIA_IsRequiredForFormPropertyId);  //=	30025;
    TM_GET_UIA_PROP(UIA_ItemStatusPropertyId);  //=	30026;
    TM_GET_UIA_PROP(UIA_IsDockPatternAvailablePropertyId);  //=	30027;
    TM_GET_UIA_PROP(UIA_IsExpandCollapsePatternAvailablePropertyId);  //=	30028;
    TM_GET_UIA_PROP(UIA_IsGridItemPatternAvailablePropertyId);  //=	30029;
    TM_GET_UIA_PROP(UIA_IsGridPatternAvailablePropertyId);  //=	30030;
    TM_GET_UIA_PROP(UIA_IsInvokePatternAvailablePropertyId);  //=	30031;
    TM_GET_UIA_PROP(UIA_IsMultipleViewPatternAvailablePropertyId);  //=	30032;
    TM_GET_UIA_PROP(UIA_IsRangeValuePatternAvailablePropertyId);  //=	30033;
    TM_GET_UIA_PROP(UIA_IsScrollPatternAvailablePropertyId);  //=	30034;
    TM_GET_UIA_PROP(UIA_IsScrollItemPatternAvailablePropertyId);  //=	30035;
    TM_GET_UIA_PROP(UIA_IsSelectionItemPatternAvailablePropertyId);  //=	30036;
    TM_GET_UIA_PROP(UIA_IsSelectionPatternAvailablePropertyId);  //=	30037;
    TM_GET_UIA_PROP(UIA_IsTablePatternAvailablePropertyId);  //=	30038;
    TM_GET_UIA_PROP(UIA_IsTableItemPatternAvailablePropertyId);  //=	30039;
    TM_GET_UIA_PROP(UIA_IsTextPatternAvailablePropertyId);  //=	30040;
    TM_GET_UIA_PROP(UIA_IsTogglePatternAvailablePropertyId);  //=	30041;
    TM_GET_UIA_PROP(UIA_IsTransformPatternAvailablePropertyId);  //=	30042;
    TM_GET_UIA_PROP(UIA_IsValuePatternAvailablePropertyId);  //=	30043;
    TM_GET_UIA_PROP(UIA_IsWindowPatternAvailablePropertyId);  //=	30044;
    TM_GET_UIA_PROP(UIA_ValueValuePropertyId);  //=	30045;
    TM_GET_UIA_PROP(UIA_ValueIsReadOnlyPropertyId);  //=	30046;
    TM_GET_UIA_PROP(UIA_RangeValueValuePropertyId);  //=	30047;
    TM_GET_UIA_PROP(UIA_RangeValueIsReadOnlyPropertyId);  //=	30048;
    TM_GET_UIA_PROP(UIA_RangeValueMinimumPropertyId);  //=	30049;
    TM_GET_UIA_PROP(UIA_RangeValueMaximumPropertyId);  //=	30050;
    TM_GET_UIA_PROP(UIA_RangeValueLargeChangePropertyId);  //=	30051;
    TM_GET_UIA_PROP(UIA_RangeValueSmallChangePropertyId);  //=	30052;
    TM_GET_UIA_PROP(UIA_ScrollHorizontalScrollPercentPropertyId);  //=	30053;
    TM_GET_UIA_PROP(UIA_ScrollHorizontalViewSizePropertyId);  //=	30054;
    TM_GET_UIA_PROP(UIA_ScrollVerticalScrollPercentPropertyId);  //=	30055;
    TM_GET_UIA_PROP(UIA_ScrollVerticalViewSizePropertyId);  //=	30056;
    TM_GET_UIA_PROP(UIA_ScrollHorizontallyScrollablePropertyId);  //=	30057;
    TM_GET_UIA_PROP(UIA_ScrollVerticallyScrollablePropertyId);  //=	30058;
    TM_GET_UIA_PROP(UIA_SelectionSelectionPropertyId);  //=	30059;
    TM_GET_UIA_PROP(UIA_SelectionCanSelectMultiplePropertyId);  //=	30060;
    TM_GET_UIA_PROP(UIA_SelectionIsSelectionRequiredPropertyId);  //=	30061;
    TM_GET_UIA_PROP(UIA_GridRowCountPropertyId);  //=	30062;
    TM_GET_UIA_PROP(UIA_GridColumnCountPropertyId);  //=	30063;
    TM_GET_UIA_PROP(UIA_GridItemRowPropertyId);  //=	30064;
    TM_GET_UIA_PROP(UIA_GridItemColumnPropertyId);  //=	30065;
    TM_GET_UIA_PROP(UIA_GridItemRowSpanPropertyId);  //=	30066;
    TM_GET_UIA_PROP(UIA_GridItemColumnSpanPropertyId);  //=	30067;
    TM_GET_UIA_PROP(UIA_GridItemContainingGridPropertyId);  //=	30068;
    TM_GET_UIA_PROP(UIA_DockDockPositionPropertyId);  //=	30069;
    TM_GET_UIA_PROP(UIA_ExpandCollapseExpandCollapseStatePropertyId);  //=	30070;
    TM_GET_UIA_PROP(UIA_MultipleViewCurrentViewPropertyId);  //=	30071;
    TM_GET_UIA_PROP(UIA_MultipleViewSupportedViewsPropertyId);  //=	30072;
    TM_GET_UIA_PROP(UIA_WindowCanMaximizePropertyId);  //=	30073;
    TM_GET_UIA_PROP(UIA_WindowCanMinimizePropertyId);  //=	30074;
    TM_GET_UIA_PROP(UIA_WindowWindowVisualStatePropertyId);  //=	30075;
    TM_GET_UIA_PROP(UIA_WindowWindowInteractionStatePropertyId);  //=	30076;
    TM_GET_UIA_PROP(UIA_WindowIsModalPropertyId);  //=	30077;
    TM_GET_UIA_PROP(UIA_WindowIsTopmostPropertyId);  //=	30078;
    TM_GET_UIA_PROP(UIA_SelectionItemIsSelectedPropertyId);  //=	30079;
    TM_GET_UIA_PROP(UIA_SelectionItemSelectionContainerPropertyId);  //=	30080;
    TM_GET_UIA_PROP(UIA_TableRowHeadersPropertyId);  //=	30081;
    TM_GET_UIA_PROP(UIA_TableColumnHeadersPropertyId);  //=	30082;
    TM_GET_UIA_PROP(UIA_TableRowOrColumnMajorPropertyId);  //=	30083;
    TM_GET_UIA_PROP(UIA_TableItemRowHeaderItemsPropertyId);  //=	30084;
    TM_GET_UIA_PROP(UIA_TableItemColumnHeaderItemsPropertyId);  //=	30085;
    TM_GET_UIA_PROP(UIA_ToggleToggleStatePropertyId);  //=	30086;
    TM_GET_UIA_PROP(UIA_TransformCanMovePropertyId);  //=	30087;
    TM_GET_UIA_PROP(UIA_TransformCanResizePropertyId);  //=	30088;
    TM_GET_UIA_PROP(UIA_TransformCanRotatePropertyId);  //=	30089;
    TM_GET_UIA_PROP(UIA_IsLegacyIAccessiblePatternAvailablePropertyId);  //=	30090;
    TM_GET_UIA_PROP(UIA_LegacyIAccessibleChildIdPropertyId);  //=	30091;
    TM_GET_UIA_PROP(UIA_LegacyIAccessibleNamePropertyId);  //=	30092;
    TM_GET_UIA_PROP(UIA_LegacyIAccessibleValuePropertyId);  //=	30093;
    TM_GET_UIA_PROP(UIA_LegacyIAccessibleDescriptionPropertyId);  //=	30094;
    TM_GET_UIA_PROP(UIA_LegacyIAccessibleRolePropertyId);  //=	30095;
    TM_GET_UIA_PROP(UIA_LegacyIAccessibleStatePropertyId);  //=	30096;
    TM_GET_UIA_PROP(UIA_LegacyIAccessibleHelpPropertyId);  //=	30097;
    TM_GET_UIA_PROP(UIA_LegacyIAccessibleKeyboardShortcutPropertyId);  //=	30098;
    TM_GET_UIA_PROP(UIA_LegacyIAccessibleSelectionPropertyId);  //=	30099;
    TM_GET_UIA_PROP(UIA_LegacyIAccessibleDefaultActionPropertyId);  //=	30100;
    TM_GET_UIA_PROP(UIA_AriaRolePropertyId);  //=	30101;
    TM_GET_UIA_PROP(UIA_AriaPropertiesPropertyId);  //=	30102;
    TM_GET_UIA_PROP(UIA_IsDataValidForFormPropertyId);  //=	30103;
    TM_GET_UIA_PROP(UIA_ControllerForPropertyId);  //=	30104;
    TM_GET_UIA_PROP(UIA_DescribedByPropertyId);  //=	30105;
    TM_GET_UIA_PROP(UIA_FlowsToPropertyId);  //=	30106;
    TM_GET_UIA_PROP(UIA_ProviderDescriptionPropertyId);  //=	30107;
    TM_GET_UIA_PROP(UIA_IsItemContainerPatternAvailablePropertyId);  //=	30108;
    TM_GET_UIA_PROP(UIA_IsVirtualizedItemPatternAvailablePropertyId);  //=	30109;
    TM_GET_UIA_PROP(UIA_IsSynchronizedInputPatternAvailablePropertyId);  //=	30110;
    TM_GET_UIA_PROP(UIA_OptimizeForVisualContentPropertyId);  //=	30111;
    TM_GET_UIA_PROP(UIA_IsObjectModelPatternAvailablePropertyId);  //=	30112;
    TM_GET_UIA_PROP(UIA_AnnotationAnnotationTypeIdPropertyId);  //=	30113;
    TM_GET_UIA_PROP(UIA_AnnotationAnnotationTypeNamePropertyId);  //=	30114;
    TM_GET_UIA_PROP(UIA_AnnotationAuthorPropertyId);  //=	30115;
    TM_GET_UIA_PROP(UIA_AnnotationDateTimePropertyId);  //=	30116;
    TM_GET_UIA_PROP(UIA_AnnotationTargetPropertyId);  //=	30117;
    TM_GET_UIA_PROP(UIA_IsAnnotationPatternAvailablePropertyId);  //=	30118;
    TM_GET_UIA_PROP(UIA_IsTextPattern2AvailablePropertyId);  //=	30119;
    TM_GET_UIA_PROP(UIA_StylesStyleIdPropertyId);  //=	30120;
    TM_GET_UIA_PROP(UIA_StylesStyleNamePropertyId);  //=	30121;
    TM_GET_UIA_PROP(UIA_StylesFillColorPropertyId);  //=	30122;
    TM_GET_UIA_PROP(UIA_StylesFillPatternStylePropertyId);  //=	30123;
    TM_GET_UIA_PROP(UIA_StylesShapePropertyId);  //=	30124;
    TM_GET_UIA_PROP(UIA_StylesFillPatternColorPropertyId);  //=	30125;
    TM_GET_UIA_PROP(UIA_StylesExtendedPropertiesPropertyId);  //=	30126;
    TM_GET_UIA_PROP(UIA_IsStylesPatternAvailablePropertyId);  //=	30127;
    TM_GET_UIA_PROP(UIA_IsSpreadsheetPatternAvailablePropertyId);  //=	30128;
    TM_GET_UIA_PROP(UIA_SpreadsheetItemFormulaPropertyId);  //=	30129;
    TM_GET_UIA_PROP(UIA_SpreadsheetItemAnnotationObjectsPropertyId);  //=	30130;
    TM_GET_UIA_PROP(UIA_SpreadsheetItemAnnotationTypesPropertyId);  //=	30131;
    TM_GET_UIA_PROP(UIA_IsSpreadsheetItemPatternAvailablePropertyId);  //=	30132;
    TM_GET_UIA_PROP(UIA_Transform2CanZoomPropertyId);  //=	30133;
    TM_GET_UIA_PROP(UIA_IsTransformPattern2AvailablePropertyId);  //=	30134;
    TM_GET_UIA_PROP(UIA_LiveSettingPropertyId);  //=	30135;
    TM_GET_UIA_PROP(UIA_IsTextChildPatternAvailablePropertyId);  //=	30136;
    TM_GET_UIA_PROP(UIA_IsDragPatternAvailablePropertyId);  //=	30137;
    TM_GET_UIA_PROP(UIA_DragIsGrabbedPropertyId);  //=	30138;
    TM_GET_UIA_PROP(UIA_DragDropEffectPropertyId);  //=	30139;
    TM_GET_UIA_PROP(UIA_DragDropEffectsPropertyId);  //=	30140;
    TM_GET_UIA_PROP(UIA_IsDropTargetPatternAvailablePropertyId);  //=	30141;
    TM_GET_UIA_PROP(UIA_DropTargetDropTargetEffectPropertyId);  //=	30142;
    TM_GET_UIA_PROP(UIA_DropTargetDropTargetEffectsPropertyId);  //=	30143;
    TM_GET_UIA_PROP(UIA_DragGrabbedItemsPropertyId);  //=	30144;
    TM_GET_UIA_PROP(UIA_Transform2ZoomLevelPropertyId);  //=	30145;
    TM_GET_UIA_PROP(UIA_Transform2ZoomMinimumPropertyId);  //=	30146;
    TM_GET_UIA_PROP(UIA_Transform2ZoomMaximumPropertyId);  //=	30147;
    TM_GET_UIA_PROP(UIA_FlowsFromPropertyId);  //=	30148;
    TM_GET_UIA_PROP(UIA_IsTextEditPatternAvailablePropertyId);  //=	30149;
    TM_GET_UIA_PROP(UIA_IsPeripheralPropertyId);  //=	30150;
    TM_GET_UIA_PROP(UIA_IsCustomNavigationPatternAvailablePropertyId);  //=	30151;
    TM_GET_UIA_PROP(UIA_PositionInSetPropertyId);  //=	30152;
    TM_GET_UIA_PROP(UIA_SizeOfSetPropertyId);  //=	30153;
    TM_GET_UIA_PROP(UIA_LevelPropertyId);  //=	30154;
    TM_GET_UIA_PROP(UIA_AnnotationTypesPropertyId);  //=	30155;
    TM_GET_UIA_PROP(UIA_AnnotationObjectsPropertyId);  //=	30156;
    TM_GET_UIA_PROP(UIA_LandmarkTypePropertyId);  //=	30157;
    TM_GET_UIA_PROP(UIA_LocalizedLandmarkTypePropertyId);  //=	30158;
    TM_GET_UIA_PROP(UIA_FullDescriptionPropertyId);  //=	30159;
    TM_GET_UIA_PROP(UIA_FillColorPropertyId);  //=	30160;
    TM_GET_UIA_PROP(UIA_OutlineColorPropertyId);  //=	30161;
    TM_GET_UIA_PROP(UIA_FillTypePropertyId);  //=	30162;
    TM_GET_UIA_PROP(UIA_VisualEffectsPropertyId);  //=	30163;
    TM_GET_UIA_PROP(UIA_OutlineThicknessPropertyId);  //=	30164;
    TM_GET_UIA_PROP(UIA_CenterPointPropertyId);  //=	30165;
    TM_GET_UIA_PROP(UIA_RotationPropertyId);  //=	30166;
    TM_GET_UIA_PROP(UIA_SizePropertyId);  //=	30167;
    TM_GET_UIA_PROP(UIA_IsSelectionPattern2AvailablePropertyId);  //=	30168;
    TM_GET_UIA_PROP(UIA_Selection2FirstSelectedItemPropertyId);  //=	30169;
    TM_GET_UIA_PROP(UIA_Selection2LastSelectedItemPropertyId);  //=	30170;
    TM_GET_UIA_PROP(UIA_Selection2CurrentSelectedItemPropertyId);  //=	30171;
    TM_GET_UIA_PROP(UIA_Selection2ItemCountPropertyId);  //=	30172;
    TM_GET_UIA_PROP(UIA_HeadingLevelPropertyId);  //=	30173;
    TM_GET_UIA_PROP(UIA_IsDialogPropertyId);  //=	30174;
    
#undef TM_GET_UIA_PROP

    return str;
    
    //     try
//     {
//         CComBSTR bstrName;
//         hr = p->get_accName(*pvarChild, &bstrName);
//         strTmp.Format(L"get_accName(%s), ret:%X\r\n", (BSTR)bstrName, hr);
//         str += strTmp;
//         
//         CComBSTR bstrVal;
//         hr = p->get_accValue(*pvarChild, &bstrVal);
//         strTmp.Format(L"get_accValue(%s), ret:%X\r\n", (BSTR)bstrVal, hr);
//         str += strTmp;
//         
//         CComBSTR bstrDes;
//         hr = p->get_accDescription(*pvarChild, &bstrDes);
//         strTmp.Format(L"get_accDescription(%s), ret:%X\r\n", (BSTR)bstrDes, hr);
//         str += strTmp;
//         
//         CComVariant varRole;
//         hr = p->get_accRole(*pvarChild, &varRole);
//         if (varRole.vt == VT_BSTR)
//             strTmp.Format(L"get_accRole(str:%s), ret:%X\r\n", varRole.bstrVal, hr);
//         else
//             strTmp.Format(L"get_accRole(%s), ret:%X\r\n", DbgFormatRole(varRole.lVal), hr);
//     str += strTmp;
//     }
}

#endif

TM_INLINE VARTYPE DbgIAccessibleGetRuntimeId(IUnknown* acc, IServiceProvider* ps, int& type, int& id)
{
    CComPtr<IAccessibleEx> accEx;
    _DbgFormatQueryInterface(acc, ps, TM_UUID_OF(IAccessibleEx), (void**)&accEx);
    if (!accEx)
        return VT_EMPTY;
    SAFEARRAY* sa = 0;
    HRESULT hr = accEx->GetRuntimeId(&sa);
    return DbgIAccessibleGetRuntimeId(sa, type, id);
}

TM_INLINE LPCTSTR DbgFormatAccEx(IAccessible* acc, IServiceProvider* ps)
{
    static TM::CStdStr s_str;
    int var1, var2;
    VARTYPE vt = DbgIAccessibleGetRuntimeId(acc, ps, var1, var2);
    if (vt == VT_EMPTY)
        return 0;
    if (vt!= VT_I4)
        s_str.Format(L"unkonwn VT type: %d", vt);
    else
        s_str.Format(L"%x,%x", var1, var2);
    return s_str;
}

TM_INLINE TMCStrW DbgFormatAccUid(IDispatch* p, IServiceProvider* ps = 0)
{
    static TM::CStringW s_str;
    s_str.Format(L"0x%p", p);
    CComQIPtr<IAccessible, &IID_IAccessible> ptr(p);
    if (!ptr)
        return s_str;
    s_str += L"(";
    s_str += DbgFormatAccEx(ptr, ps);
    s_str += L")";
    return s_str;
}

TM_INLINE TM::CStringW DbgFormatAcc(IAccessible* p, VARIANT* pvarChild, TM::CString* strRolAndID = 0)
{
    TM::CStringW str, strTmp;

    if (!p)
        return str;

    DbgAssert(pvarChild);
    HRESULT hr;

//     try
    {
    CComBSTR bstrName;
    hr = p->get_accName(*pvarChild, &bstrName);
    strTmp.Format(L"get_accName(%s), ret:%X\r\n", (BSTR)bstrName, hr);
    str += strTmp;

    CComBSTR bstrVal;
    hr = p->get_accValue(*pvarChild, &bstrVal);
    strTmp.Format(L"get_accValue(%s), ret:%X\r\n", (BSTR)bstrVal, hr);
    str += strTmp;

    CComBSTR bstrDes;
    hr = p->get_accDescription(*pvarChild, &bstrDes);
    strTmp.Format(L"get_accDescription(%s), ret:%X\r\n", (BSTR)bstrDes, hr);
    str += strTmp;

    CComVariant varRole;
    hr = p->get_accRole(*pvarChild, &varRole);
    if (varRole.vt == VT_BSTR)
        strTmp.Format(L"get_accRole(str:%s), ret:%X\r\n", varRole.bstrVal, hr);
    else
        strTmp.Format(L"get_accRole(%s), ret:%X\r\n", DbgFormatRole(varRole.lVal), hr);
    str += strTmp;

    if (strRolAndID)
    {
        CComBSTR bstrID;
        hr = p->get_accDescription(CComVariant(CHILDID_GETOID), &bstrID);
        if (bstrID && (bstrID[0] <= '9'))
        {
            *strRolAndID = bstrID;
            *strRolAndID += L"(";
            *strRolAndID += DbgFormatRole(varRole.lVal);
            *strRolAndID += L")";
        }
        else
            *strRolAndID = DbgFormatRole(varRole.lVal);
    }

    CComVariant varState;
    hr = p->get_accState(*pvarChild, &varState);
//     varState.ChangeType(VT_BSTR);
    strTmp.Format(L"get_accState(%s), ret:%X\r\n", DbgFormatState(varState.lVal), hr);
    str += strTmp;

    CComBSTR bstrHelp;
    hr = p->get_accHelp(*pvarChild, &bstrHelp);
    strTmp.Format(L"get_accHelp(%s), ret:%X\r\n", (BSTR)bstrHelp, hr);
    str += strTmp;

    CComBSTR bstrSCut;
    hr = p->get_accKeyboardShortcut(*pvarChild, &bstrSCut);
    strTmp.Format(L"get_accKeyboardShortcut(%s), ret:%X\r\n", (BSTR)bstrSCut, hr);
    str += strTmp;

    CComBSTR bstrAction;
    hr = p->get_accDefaultAction(*pvarChild, &bstrAction);
    strTmp.Format(L"get_accDefaultAction(%s), ret:%X\r\n", (BSTR)bstrAction, hr);
    str += strTmp;

    LONG l = 0, t = 0, w = 0, h = 0;
    hr = p->accLocation(&l, &t, &w, &h, *pvarChild);
    strTmp.Format(L"accLocation(%d,%d,%d,%d), ret:%X\r\n", l,t,w,h, hr);
    str += strTmp;

    if (pvarChild->lVal)
        return str;

    CComPtr<IDispatch> parent;
    hr = p->get_accParent(&parent);
    strTmp.Format(L"get_accParent(%s), ret:%s\r\n", DbgFormatAccUid(parent, 0), hr);
    str += strTmp;

    LONG nCount = 0;
    hr = p->get_accChildCount(&nCount);
    strTmp.Format(L"get_accChildCount(%d), ret:%X\r\n", nCount, hr);
    str += strTmp;

    CComVariant varFocus;
    hr = p->get_accFocus(&varFocus);
    {
        CComQIPtr<IAccessible, &IID_IAccessible> ptr;
        if (varFocus.vt == VT_DISPATCH && (ptr = varFocus.pdispVal))
        {
            long l = 0, t= 0, w = 0, h = 0;
            ptr->accLocation(&l, &t, &w, &h, CComVariant(0));
            strTmp.Format(L"get_accFocus(%s, (%d,%d,%d,%d)), ret:%X\r\n", DbgFormatAccUid(varFocus.pdispVal, 0), l,t,w,h, hr);
        }
        else
            strTmp.Format(L"get_accFocus(%X), ret:%X\r\n", varFocus.lVal, hr);
    }
    str += strTmp;

    CComVariant varSel;
    hr = p->get_accSelection(&varSel);
    strTmp.Format(L"get_accSelection(%X), ret:%X\r\n", varSel.lVal, hr);
    str += strTmp;

    CComQIPtr<IEnumVARIANT> ptrVar(p);
    if (ptrVar)
    {
        ptrVar->Reset();
        CComVariant* pvar = new CComVariant[nCount];
        ULONG celt = 0;
        int n = tm_min(nCount, 10);
        hr = ptrVar->Next(n, pvar, &celt);
        if (SUCCEEDED(hr))
        {
            strTmp.Format(L"childs(%d-%d):", nCount, celt);
            str += strTmp;
            for (int i = 0; i < celt; ++i)
            {
                if (pvar->vt == VT_DISPATCH) 
                    strTmp = DbgFormatAccUid(pvar[i].pdispVal, 0);
                else
                    strTmp.Format(L"%d", pvar[i].lVal);

                str += strTmp;
                str += L",";
            }
            str += L"\r\n";
        }
        else
            str += L"IEnumVARIANT::Next() fail; \r\n";
        delete[] pvar;
    }
    else
        str += L"QueryInterface(IEnumVARIANT) fail; \r\n";

    CComQIPtr<IOleWindow> ptrOleWindow(p);
    if (ptrOleWindow)
    {
        HWND hWnd = 0;
        HRESULT hr = ptrOleWindow->GetWindow(&hWnd);
        strTmp.Format(L"IOleWindow::GetWindow(%s), ret:%X\r\n", DbgFormatWindow(hWnd), hr);
        str += strTmp;
    }

    CComQIPtr<IServiceProvider> psp(p);
    LPCTSTR idIdentity = DbgFormatAccId(p, psp);
    if (idIdentity)
    {
        strTmp.Format(L"IAccIdentity::GetIdentityString(%s)\r\n", idIdentity);
        str += strTmp;
    }

    LPCTSTR pEx = DbgFormatAccEx(p, psp);
    if (pEx)
    {
        strTmp.Format(L"IAccessibleEx::GetRuntimeId(%s)\r\n", pEx);
        str += strTmp;
    }

//     TM CAccessibleObj::QueryInterface(2132b005-c604-4354-85bd-8f2e24181b0c), HWND:000206D6, HTUI:03095E28(10002-manage-10001) x n
//     TM CAccessibleObj::QueryInterface(334d391f-0e79-3b15-c9ff-eac65dd07c42), HWND:001F035E, HTUI:02EC1348(10002-manage-10001) x n
//     TM CAccessibleObj::QueryInterface(77dd1250-139c-2bc3-bd95-900aced61be5), HWND:001F035E, HTUI:02EC1348(10002-manage-10001) x n
//     TM CAccessibleObj::QueryInterface(bfd60505-5a1f-4e41-88ba-a6fb07202da9), HWND:001F035E, HTUI:02EC1348(10002-manage-10001) x n
//     TM CAccessibleObj::QueryInterface(03fb5c57-d534-45f5-a1f4-d39556983875), HWND:001F035E, HTUI:02EC1348(10002-manage-10001) n x
//     TM CAccessibleObj::QueryInterface(2c258ae7-50dc-49ff-9d1d-2ecb9a52cdd7), HWND:001F035E, HTUI:02EC1348(10002-manage-10001) n
//     TM CAccessibleObj::QueryInterface(143715d9-a015-40ea-b695-d5cc267e36ee), HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryInterface(d6defab3-dbb9-4413-8af9-554586fdff94), HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryInterface(ea5d0de4-770d-4da0-a9f8-d7f9a140ff79), HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryInterface(816e5b3e-5523-4efc-9223-98ec4214c3a0), HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryInterface(3c169ff7-37b2-484c-b199-c3155590f316), HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryInterface(4f4f92b5-6ded-4e9b-a93f-013891b3a8b7), HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryInterface(1868091e-ab5a-415f-a02f-5c4dd0cf901d), HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryInterface(11456f96-09d1-4909-8f36-4eb74e42b93e), HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryInterface(1ac7516e-e6bb-4a69-b63f-e841904dc5a6), HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryInterface(35bd3360-1b35-4927-bae4-b10e70d99eff), HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryInterface(f2153260-232e-4474-9d0a-9f2ab153441d), HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryService(acd46652-829d-41cb-a5fc-17acf43661ac, IUnknown); HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryService(b96fdb85-7204-4724-842b-c7059dedb9d0, IUnknown); HWND:000206D6, HTUI:03095E28(10002-manage-10001) n
//     TM CAccessibleObj::QueryService(fda075cf-7c8b-498c-b514-a9cb521bbfb4, 55f9c234-79cd-4699-89e7-5518fd88bcde); HWND:000206D6, HTUI:03095E28(10002-manage-10001) nn
//     TM CAccessibleObj::QueryService(8edaa462-21f4-4c87-a012-b3cda3ab01fc, f660d979-a938-423f-b5b9-12e4028c50b0); HWND:000206D6, HTUI:03095E28(10002-manage-10001) nn

        
        


    typedef IOleWindow _IOleWindow;
    typedef IServiceProvider _IServiceProvider;
    typedef IProvideClassInfo _IProvideClassInfo;

    MIDL_INTERFACE("7852b78d-1cfd-41c1-a615-9c0c85960b5f") _IAccIdentity;
    MIDL_INTERFACE("1c733a30-2a1c-11ce-ade5-00aa0044773d") _ICallFactory;

    MIDL_INTERFACE("E89F726E-C4F4-4c19-BB19-B647D7FA8478") _IAccessible2;
    MIDL_INTERFACE("6C9430E9-299D-4E6F-BD01-A82A1E88D3FF") _IAccessible2_2;
    MIDL_INTERFACE("B70D9F59-3B5A-4dba-AB9E-22012F607DF5") _IAccessibleAction;
    MIDL_INTERFACE("D49DED83-5B25-43F4-9B95-93B44595979E") _IAccessibleApplication;
    MIDL_INTERFACE("1546D4B0-4C98-4bda-89AE-9A64748BDDE4") _IAccessibleComponent;
    MIDL_INTERFACE("C48C7FCF-4AB5-4056-AFA6-902D6E1D1149") _IAccessibleDocument;
    MIDL_INTERFACE("A59AA09A-7011-4b65-939D-32B1FB5547E3") _IAccessibleEditableText;
    MIDL_INTERFACE("01C20F2B-3DD2-400f-949F-AD00BDAB1D41") _IAccessibleHyperlink;
    MIDL_INTERFACE("6B4F8BBF-F1F2-418a-B35E-A195BC4103B9") _IAccessibleHypertext;
    MIDL_INTERFACE("CF64D89F-8287-4B44-8501-A827453A6077") _IAccessibleHypertext2;
    MIDL_INTERFACE("FE5ABB3D-615E-4f7b-909F-5F0EDA9E8DDE") _IAccessibleImage;
    MIDL_INTERFACE("7CDF86EE-C3DA-496a-BDA4-281B336E1FDC") _IAccessibleRelation;
    MIDL_INTERFACE("35AD8070-C20C-4fb4-B094-F4F7275DD469") _IAccessibleTable;
    MIDL_INTERFACE("6167f295-06f0-4cdd-a1fa-02e25153d869") _IAccessibleTable2;
    MIDL_INTERFACE("594116B1-C99F-4847-AD06-0A7A86ECE645") _IAccessibleTableCell;
    MIDL_INTERFACE("24FD2FFB-3AAD-4a08-8335-A3AD89C0FB4B") _IAccessibleText;
    MIDL_INTERFACE("9690A9CC-5C80-4DF5-852E-2D5AE4189A54") _IAccessibleText2;
    MIDL_INTERFACE("35855B5B-C566-4fd0-A7B1-E65465600394") _IAccessibleValue;

    MIDL_INTERFACE("3050f1ff-98b5-11cf-bb82-00aa00bdce0b") _IHTMLElement;
    MIDL_INTERFACE("3050f434-98b5-11cf-bb82-00aa00bdce0b") _IHTMLElement2;

    MIDL_INTERFACE("76c0dbbb-15e0-4e7b-b61b-20eeea2001e0") _IAccPropServer;
    MIDL_INTERFACE("6e26e776-04f0-495d-80e4-3330352e3169") _IAccPropServices;
    MIDL_INTERFACE("d6dd68d1-86fd-4332-8666-9abedea2d24c") _IRawElementProviderSimple;
    MIDL_INTERFACE("A0A839A9-8DA1-4A82-806A-8E0D44E79F56") _IRawElementProviderSimple2;
    MIDL_INTERFACE("f8b80ada-2c44-48d0-89be-5ff23c9cd875") _IAccessibleEx;
    MIDL_INTERFACE("620ce2a5-ab8f-40a9-86cb-de3c75599b58") _IRawElementProviderFragmentRoot;
    MIDL_INTERFACE("f7063da8-8359-439c-9297-bbc5299a7d87") _IRawElementProviderFragment;
    MIDL_INTERFACE("a407b27b-0f6d-4427-9292-473c7bf93258") _IRawElementProviderAdviseEvents;
    MIDL_INTERFACE("1d5df27c-8947-4425-b8d9-79787bb460b8") _IRawElementProviderHwndOverride;
    MIDL_INTERFACE("4fd82b78-a43e-46ac-9803-0a6969c7c183") _IProxyProviderWinEventSink;
    MIDL_INTERFACE("89592ad4-f4e0-43d5-a3b6-bad7e111b435") _IProxyProviderWinEventHandler;
    MIDL_INTERFACE("0a2a93cc-bfad-42ac-9b2e-0991fb0d3ea0") _IRawElementProviderWindowlessSite;
    MIDL_INTERFACE("33AC331B-943E-4020-B295-DB37784974A3") _IAccessibleHostingElementProviders;
    MIDL_INTERFACE("24BE0B07-D37D-487A-98CF-A13ED465E9B3") _IRawElementProviderHostingAccessibles;
    MIDL_INTERFACE("159bc72c-4ad3-485e-9637-d7052edf0146") _IDockProvider;
    MIDL_INTERFACE("d847d3a5-cab0-4a98-8c32-ecb45c59ad24") _IExpandCollapseProvider;
    MIDL_INTERFACE("b17d6187-0907-464b-a168-0ef17a1572b1") _IGridProvider;
    MIDL_INTERFACE("d02541f1-fb81-4d64-ae32-f520f8a6dbd1") _IGridItemProvider;
    MIDL_INTERFACE("54fcb24b-e18e-47a2-b4d3-eccbe77599a2") _IInvokeProvider;
    MIDL_INTERFACE("6278cab1-b556-4a1a-b4e0-418acc523201") _IMultipleViewProvider;
    MIDL_INTERFACE("36dc7aef-33e6-4691-afe1-2be7274b3d33") _IRangeValueProvider;
    MIDL_INTERFACE("2360c714-4bf1-4b26-ba65-9b21316127eb") _IScrollItemProvider;
    MIDL_INTERFACE("fb8b03af-3bdf-48d4-bd36-1a65793be168") _ISelectionProvider;
    MIDL_INTERFACE("b38b8077-1fc3-42a5-8cae-d40c2215055a") _IScrollProvider;
    MIDL_INTERFACE("2acad808-b2d4-452d-a407-91ff1ad167b2") _ISelectionItemProvider;
    MIDL_INTERFACE("29db1a06-02ce-4cf7-9b42-565d4fab20ee") _ISynchronizedInputProvider;
    MIDL_INTERFACE("9c860395-97b3-490a-b52a-858cc22af166") _ITableProvider;
    MIDL_INTERFACE("b9734fa6-771f-4d78-9c90-2517999349cd") _ITableItemProvider;
    MIDL_INTERFACE("56d00bd0-c4f4-433c-a836-1a52a57e0892") _IToggleProvider;
    MIDL_INTERFACE("6829ddc4-4f91-4ffa-b86f-bd3e2987cb4c") _ITransformProvider;
    MIDL_INTERFACE("c7935180-6fb3-4201-b174-7df73adbf64a") _IValueProvider;
    MIDL_INTERFACE("987df77b-db06-4d77-8f8a-86a9c3bb90b9") _IWindowProvider;
    MIDL_INTERFACE("e44c3566-915d-4070-99c6-047bff5a08f5") _ILegacyIAccessibleProvider;
    MIDL_INTERFACE("e747770b-39ce-4382-ab30-d8fb3f336f24") _IItemContainerProvider;
    MIDL_INTERFACE("cb98b665-2d35-4fac-ad35-f3c60d0c0b8b") _IVirtualizedItemProvider;
    MIDL_INTERFACE("3ad86ebd-f5ef-483d-bb18-b1042a475d64") _IObjectModelProvider;
    MIDL_INTERFACE("f95c7e80-bd63-4601-9782-445ebff011fc") _IAnnotationProvider;
    MIDL_INTERFACE("19b6b649-f5d7-4a6d-bdcb-129252be588a") _IStylesProvider;
    MIDL_INTERFACE("6f6b5d35-5525-4f80-b758-85473832ffc7") _ISpreadsheetProvider;
    MIDL_INTERFACE("eaed4660-7b3d-4879-a2e6-365ce603f3d0") _ISpreadsheetItemProvider;
    MIDL_INTERFACE("4758742f-7ac2-460c-bc48-09fc09308a93") _ITransformProvider2;
    MIDL_INTERFACE("6aa7bbbb-7ff9-497d-904f-d20b897929d8") _IDragProvider;
    MIDL_INTERFACE("bae82bfd-358a-481c-85a0-d8b4d90a5d61") _IDropTargetProvider;
    MIDL_INTERFACE("5347ad7b-c355-46f8-aff5-909033582f63") _ITextRangeProvider;
    MIDL_INTERFACE("3589c92c-63f3-4367-99bb-ada653b77cf2") _ITextProvider;
    MIDL_INTERFACE("0dc5e6ed-3e16-4bf1-8f9a-a979878bc195") _ITextProvider2;
    MIDL_INTERFACE("EA3605B4-3A05-400E-B5F9-4E91B40F6176") _ITextEditProvider;
    MIDL_INTERFACE("9BBCE42C-1921-4F18-89CA-DBA1910A0386") _ITextRangeProvider2;
    MIDL_INTERFACE("4c2de2b9-c88f-4f88-a111-f1d336b7d1a9") _ITextChildProvider;
    MIDL_INTERFACE("c03a7fe4-9431-409f-bed8-ae7c2299bc8d") _IUIAutomationPatternInstance;
    MIDL_INTERFACE("d97022f3-a947-465e-8b2a-ac4315fa54e8") _IUIAutomationPatternHandler;
    MIDL_INTERFACE("8609c4ec-4a1a-4d88-a357-5a66e060e1cf") _IUIAutomationRegistrar;

    str += L"interface:";
#   define _FMT_TEST(x) _DbgFormatTestInterface(str, p, psp, _uuidof(_##x), L###x);
//    _FMT_TEST(IOleWindow);
    _FMT_TEST(IProvideClassInfo);
    _FMT_TEST(IAccIdentity);
    _FMT_TEST(IServiceProvider);
    _FMT_TEST(IAccPropServer);
    _FMT_TEST(IAccPropServices);
    _FMT_TEST(IHTMLElement);
    _FMT_TEST(IHTMLElement2);
    _FMT_TEST(ICallFactory);

    _FMT_TEST(IAccessible2);
    _FMT_TEST(IAccessible2_2);
    _FMT_TEST(IAccessibleAction);
    _FMT_TEST(IAccessibleApplication);
    _FMT_TEST(IAccessibleComponent);
    _FMT_TEST(IAccessibleDocument);
    _FMT_TEST(IAccessibleEditableText);
    _FMT_TEST(IAccessibleHyperlink);
    _FMT_TEST(IAccessibleHypertext);
    _FMT_TEST(IAccessibleHypertext2);
    _FMT_TEST(IAccessibleImage);
    _FMT_TEST(IAccessibleRelation);
    _FMT_TEST(IAccessibleTable);
    _FMT_TEST(IAccessibleTable2);
    _FMT_TEST(IAccessibleTableCell);
    _FMT_TEST(IAccessibleText);
    _FMT_TEST(IAccessibleText2);
    _FMT_TEST(IAccessibleValue);

    CComPtr<IUnknown> ptrEx = _FMT_TEST(IAccessibleEx);
    if (ptrEx)
    {
#   undef _FMT_TEST
#   define _FMT_TEST(x) _DbgFormatTestInterface(str, ptrEx, 0, _uuidof(_##x), L###x);

        CComPtr<IUnknown> ptrPS = _FMT_TEST(IRawElementProviderSimple);
        _FMT_TEST(IRawElementProviderSimple2);
        _FMT_TEST(IRawElementProviderFragmentRoot);
        _FMT_TEST(IRawElementProviderFragment);
        _FMT_TEST(IRawElementProviderAdviseEvents);
        _FMT_TEST(IRawElementProviderHwndOverride);
        _FMT_TEST(IProxyProviderWinEventSink);
        _FMT_TEST(IProxyProviderWinEventHandler);
        _FMT_TEST(IRawElementProviderWindowlessSite);
        _FMT_TEST(IAccessibleHostingElementProviders);
        _FMT_TEST(IRawElementProviderHostingAccessibles);
        _FMT_TEST(IDockProvider);
        _FMT_TEST(IExpandCollapseProvider);
        _FMT_TEST(IGridProvider);
        _FMT_TEST(IGridItemProvider);
        _FMT_TEST(IInvokeProvider);
        _FMT_TEST(IMultipleViewProvider);
        _FMT_TEST(IRangeValueProvider);
        _FMT_TEST(IScrollItemProvider);
        _FMT_TEST(ISelectionProvider);
        _FMT_TEST(IScrollProvider);
        _FMT_TEST(ISelectionItemProvider);
        _FMT_TEST(ISynchronizedInputProvider);
        _FMT_TEST(ITableProvider);
        _FMT_TEST(ITableItemProvider);
        _FMT_TEST(IToggleProvider);
        _FMT_TEST(ITransformProvider);
        _FMT_TEST(IValueProvider);
        _FMT_TEST(IWindowProvider);
        _FMT_TEST(ILegacyIAccessibleProvider);
        _FMT_TEST(IItemContainerProvider);
        _FMT_TEST(IVirtualizedItemProvider);
        _FMT_TEST(IObjectModelProvider);
        _FMT_TEST(IAnnotationProvider);
        _FMT_TEST(IStylesProvider);
        _FMT_TEST(ISpreadsheetProvider);
        _FMT_TEST(ISpreadsheetItemProvider);
        _FMT_TEST(ITransformProvider2);
        _FMT_TEST(IDragProvider);
        _FMT_TEST(IDropTargetProvider);
        _FMT_TEST(ITextRangeProvider);
        _FMT_TEST(ITextProvider);
        _FMT_TEST(ITextProvider2);
        _FMT_TEST(ITextEditProvider);
        _FMT_TEST(ITextRangeProvider2);
        _FMT_TEST(ITextChildProvider);
        _FMT_TEST(IUIAutomationPatternInstance);
        _FMT_TEST(IUIAutomationPatternHandler);
        _FMT_TEST(IUIAutomationRegistrar);

//         if (ptrPS)
//         {
//             str += L"\r\nIRawElementProviderSimple:\r\n";
//             str += DbgFormatUIA(CComQIPtr<IRawElementProviderSimple>(ptrPS));
//         }
    }
#   undef _FMT_TEST

    str += L"\r\n";

    str += DbgFormatAcc2(p, psp);

    }
//     catch(...)
    {
    }

    return str;
}

TM_INLINE TM::CStringW DbgFormatAccFromEvent(HWND hWnd, LONG idObject, LONG idChild, TM::CString* strRolAndID)
{
    CComPtr<IAccessible> ptr;
    CComVariant var;
    AccessibleObjectFromEvent(hWnd, idObject, idChild, &ptr, &var);
    return DbgFormatAcc(ptr, &var, strRolAndID);
}

TM_INLINE LPCTSTR DbgFormatAccEvent(DWORD event)
{
#   define EVENT_DEFW(n,m) case n: return _T(#m); 
#   define EVENT_DEF2(n,m) case n: return _T(#m) _T("2"); 
#   define EVENT_DEFU(n,m) case n: return _T(#m) _T("U"); 
    
    switch (event)
    {
#include "tmaccevent_i.h"
    }
    
    static TM::CString s_str;
    s_str.Format(L"0x%X", event);
    return s_str;
}


#endif // TMFORMATACC_H
