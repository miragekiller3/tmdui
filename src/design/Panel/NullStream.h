/*
 *  @file
 *  @brief
 *  stream for object
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.12.17
 *
 *  Copyright (C) 2018 miragekiller
 *  All rights reserved.
 */

#ifndef NULLSTREAM_H
#define NULLSTREAM_H


#ifndef TMDEF_H
#   include "tmdef.h"
#endif


TM_BGN_NAMESPACE

class CTuiObject;

class CNullStream
{
public:
    void BeginSerialize(const void*) const {}
    void EndSerialize() const {}
    void ExecChilds(const void*) const {}
    template<class T> TMBool ExecID     (CStrView pName, T& __val) const { return false; }
    template<class T> TMBool ExecDlg    (CStrView pName, T& __val) const { return false; }
    template<class T> TMBool ExecWin    (CStrView pName, T& __val) const { return false; }
    template<class T> TMBool ExecInt    (CStrView pName, const T& __val) const { return false; }
    template<class T> TMBool ExecFile   (CStrView pName, const T& __val) const { return false; }
    template<class T> TMBool ExecPic    (CStrView pName, const T& __val) const { return false; }
    template<class T> TMBool ExecFrame  (CStrView pName, const T& __val) const { return false; }
    template<class T> TMBool ExecEff    (CStrView pName, const T& __val) const { return false; }
    template<class T> TMBool ExecEnum   (CStrView pName, const T& __val, const void*) const { return false; }
    template<class T> TMBool ExecMask   (CStrView pName, const T& __val, const void*) const { return false; }
    template<class T> TMBool ExecChar   (CStrView pName, const T& __val) const { return false; }
    template<class T> TMBool ExecText   (CStrView pName, const T& __val) const { return false; }
    template<class T> TMBool ExecMenu   (CStrView pName, const T& __val) const { return false; }
    template<class T> TMBool ExecOle    (CStrView pName, const T& __val) const { return false; }
    template<class T> TMBool ExecClass  (CStrView pName, const T& __val) const { return false; }
};

class CIdStream : public CNullStream
{
public:
    virtual void BeginSerialize (CTuiObject* p) {}
    virtual TMBool ExecID       (CStrView pName, TMUInt32& __val) = 0;
    virtual TMBool ExecDlg      (CStrView pName, TMUInt32& __val) = 0;
    virtual TMBool ExecWin      (CStrView pName, TMUInt32& __val) = 0;
    virtual TMBool ExecPic      (CStrView pName, struct CPicPtr& __val) { return false; }
    virtual TMBool ExecFrame    (CStrView pName, struct CFrmPtr& __val) { return false; }
};

TM_END_NAMESPACE



#endif //NULLSTREAM_H
