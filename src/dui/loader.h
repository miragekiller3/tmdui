/*
 *  @file
 *  @brief  xml configuration file management
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.19
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef LOADER_H 
#define LOADER_H

#include "../tmxml.h"
#include "../tmsymbol.h"
#include "../tmfilename.h"
#include "../tmlist.h"
#include "../tmgdi.h"
#include "../tmcolor.h"
#include "../tmxmlstring.h"
#include "../tmdui.h"
#include "property.h"


TM_BGN_NAMESPACE

struct  IDomElement;
struct  CModule;



///////////////////////////////////////////////////////////////////////////////
// CLoadStream
class CLoadStream : public ILoaderEnv//, public CProp_obj_env
{
private:
    typedef CMap<CWeakStr, CWeakStr>   CValMap;
    
    IDomElement*    m_xml;
    CValMap         m_map;
    TMUInt32        m_dwFlag;
    CModule&        m_mdl;
    int             m_nDPI;

    void GetProp(const CWeakStr& name, void* pMem, IPropItemLoader* loader) const;

    // ILoaderEnv
    virtual CModule& GetModule() const { return m_mdl; }
    virtual int GetDPI() const { return m_nDPI; }

public:
    CLoadStream(CModule& mdl, IDomElement* tag, TMUInt32 dwFlag, int dpi);

    IDomElement* GetXml() const { return m_xml; }

    void BeginSerialize(const void*) const {}
    void EndSerialize() const {}
    void ExecChilds(HTUI hParent) const;

    template <class T> TMBool ExecInt(const CWeakStr& name, T& __val) const
    {
        return GetProp(name, &__val, PropMakeLong(__val)), false;
    }
    
    TMBool ExecID(const CWeakStr& name, TMUInt32& __val) const { return ExecInt(name, __val); }
    TMBool ExecDlg(const CWeakStr& name, TMUInt32& __val) const { return ExecInt(name, __val); }
    TMBool ExecWin(const CWeakStr& name, TMUInt32& __val) const { return ExecInt(name, __val); }

    template <class T> TMBool ExecEnum(const CWeakStr& name, T& __val, const void* p) const
    {
        return ExecInt(name, __val);
    }
    
    template <class T> TMBool ExecMask(const CWeakStr& name, T& __val, const void* p) const
    {
        return ExecInt(name, __val);
    }

    TMBool ExecChar(const CWeakStr& name, wchar_t& __val) const
    {
        return GetProp(name, &__val, PropMakeChar(__val)), false;
    }

    TMBool ExecText(const CWeakStr& name, CString& __val) const
    {
        return ExecClass(name, __val);
    }

    TMBool ExecFile(const CWeakStr& name, CString& __val) const
    {
        return ExecText(name, __val);
    }

    TMBool ExecPic(const CWeakStr& name, CPicPtr& __val) const
    {
        return ExecClass(name, __val);
    }
    
    TMBool ExecFrame(const CWeakStr& name, CFrmPtr& __val) const
    {
        return ExecClass(name, __val);
    }

    TMBool ExecOle(const CWeakStr& name, CString& __val) const
    {
        return ExecText(name, __val);
    }

    template<class T> TMBool ExecClass(const CWeakStr& name, T& __val) const
    {
        return GetProp(name, tm_addressof(__val), PropMake(__val)), false;
    }
};


TM_END_NAMESPACE


#endif // #ifndef LOADER_H
