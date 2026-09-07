/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.4.29
 *
 *  Copyright (C) 2021 miragekiller
 */

#ifndef ACCHASH_H
#define ACCHASH_H


#ifndef TMDEF_H
#   include "tmdef.h"
#endif

class CAccessibleStyle;
class CAccessibleStyleItemBase;
typedef class CAccessibleObj* _HashObject;
typedef AccHash32 _HashInt;
const _HashInt hash_sel_item_0 = 0x60;
const _HashInt hash_sel_item_1 = 0x61;
const _HashInt hash_focus_item = 0x62;
const _HashInt hash_min = 0x70;
const _HashInt hash_max = INT_MAX;
struct CHash
{
    // _HashObject ref: new object: ref = 1; all return object not auto add ref.
    static _HashObject Find(_HashInt id);
    static _HashObject Find(HTUI hWnd);
    static _HashObject Make(HTUI hWnd);
    static void Erase(HTUI p);
    static TMBool IsClean();
    static _HashInt IsID(_HashInt n) { return false; } //n >= hash_min && n < hash_max; }
    //static _HashObject MakeSelItem(HTUI hWnd, long sel, CAccessibleStyleItemBase& style);
    static _HashObject MakeFocusItem(CAccessibleStyle* src, HTUI hWnd, long sel, CAccessibleStyleItemBase& style);
    static _HashObject GetFocusItem();
};

#endif //ACCHASH_H
