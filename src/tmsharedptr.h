/*
 *  @file
 *  @brief  shared_ptr, weak_ptr support
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2022.2.24
 *
 *  Copyright (C) 2022 miragekiller
 */

#pragma once

#ifndef TMSHAREDPTR_H
#define TMSHAREDPTR_H

#include "tmdebug.h"
#include <memory>


TM_BGN_NAMESPACE


#ifdef __cpp_lib_enable_shared_from_this

template<class T> using _EnableSharedFromThis = std::enable_shared_from_this<T>;

struct _hack_std_sptr_count
{
    TM_NO_COPYABLE(_hack_std_sptr_count);
public:
#if defined(TM_MSC)
    void* _Ptr;
    std::_Ref_count_base* _Rep;
    TMRefInt AddRef() { DbgAssert(_Rep); return _Rep->_Incref(), 1; }
    TMRefInt Release() { DbgAssert(_Rep); return _Rep->_Decref(), 1; }
#elif defined(TM_GNUC)
    void* _M_ptr;
    std::_Sp_counted_base<std::__default_lock_policy>* _M_refcount;
    TMRefInt AddRef() { DbgAssert(_M_refcount); return _M_refcount->_M_add_ref_copy(), 1; }
    TMRefInt Release() { DbgAssert(_M_refcount); return _M_refcount->_M_release(), 1; }
#endif
};

template<class T> TMRefInt sptrAddRef(_EnableSharedFromThis<T>& ptr)
{
    _hack_std_sptr_count* p = (_hack_std_sptr_count*)&ptr;
    return p->AddRef();
}

template<class T> TMRefInt sptrRelease(_EnableSharedFromThis<T>& ptr)
{
    _hack_std_sptr_count* p = (_hack_std_sptr_count*)&ptr;
    return p->Release();
}

#else // __cpp_lib_enable_shared_from_this

template<class T> class _EnableSharedFromThis;
template<class T> TMRefInt sptrAddRef(_EnableSharedFromThis<T>& ptr);
template<class T> TMRefInt sptrRelease(_EnableSharedFromThis<T>& ptr);

#endif // __cpp_lib_enable_shared_from_this else



TM_END_NAMESPACE


#endif //TMSHAREDPTR_H
