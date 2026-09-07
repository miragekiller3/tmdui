#ifndef SAVESTREAM_H
#define SAVESTREAM_H


#include "../dui/property.h"


#define CAutoSaveAnalysis CAutoAnalysisNull
//#define CAutoSaveAnalysis CAutoAnalysisTuiUser


class CPanelApp;
class CSaveFileRes;


TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CSaveParam
struct CSaveParam
{
    TMUInt32        m_nFilter;
    CIDUsedList*    m_pSaveList;
    CSaveFileRes*   m_pSaveFile;

    CSaveParam(TMUInt32 filter, CIDUsedList* pSaveList, CSaveFileRes* pSaveFile)
        : m_nFilter(filter)
        , m_pSaveList (pSaveList)
        , m_pSaveFile (pSaveFile)
    {
    }

    void RegistID(TMUInt32 uID) { if (m_pSaveList) ++(*m_pSaveList)[uID]; }
};

class CSaveStream : public TM::CDomElementPtr
{
    typedef TM::CDomElementPtr   base_class;

private:
    typedef CMap<const void*, TM::CStrPtr>              CTokenList; // check duplicate property
    CTokenList      m_aToken;
    CTuiObject*     m_pObj;
    CSaveParam&     m_sp;

    void SaveProp(const CWeakStr& name, const void* pToken, const void* pMem,  IPropItemSaver* saver, int nType);
    void SaveProp(const CWeakStr& name, const void* pToken, const void* pMem,  IPropItemSaver* saver) { SaveProp(name, pToken, pMem, saver, saver->GetType()); }

    TMBool _SaveString(const CWeakStr& name, const void* pToken, const CString& __val, int nType)
    {
        return SaveProp(name, pToken, &__val, PropMake(__val), nType), false;
    }

    TMBool _SavePropID(const CWeakStr& name, const void* pToken, TMUInt32 __val, int nType);

    TMBool _SavePropLong(const CWeakStr& name, const void* pToken, long __val, int nType)
    {
        return SaveProp(name, pToken, &__val, PropMakeLong(__val), nType), false;
    }
    
public:
    CSaveStream(IDomNode* pParent, CTuiObject* pCur, CSaveParam& sp);
    ~CSaveStream();

    void BeginSerialize(const void*) const {}
    void EndSerialize() const {}
    void ExecChilds(CTuiWgt* pParents);

    template<class T> TMBool ExecInt(const CWeakStr& name, const T& __val)
    {
        return _SavePropLong(name, &__val, __val, SS_INT);
    }

    TMBool ExecFile(const CWeakStr& name, CString& __val);

    TMBool ExecID(const CWeakStr& pName, const TMUInt32& __val)
    {
        return _SavePropID(pName, &__val, __val, SS_ID);
    }

    TMBool ExecDlg(const CWeakStr& pName, const TMUInt32& __val)
    {
        return _SavePropID(pName, &__val, __val, SS_ID);
    }

    TMBool ExecWin(const CWeakStr& pName, const TMUInt32& __val)
    {
        return _SavePropID(pName, &__val, __val, SS_ID);
    }

    TMBool ExecPic(const CWeakStr& pName, CPicPtr& __val);
    TMBool ExecFrame(const CWeakStr& pName, CFrmPtr& __val);

    template <class T>
    TMBool ExecEnum(const CWeakStr& pName, const T& __val, const void*)
    {
        return _SavePropLong(pName, &__val, __val, SS_ENUM);
    }
    
    template <class T>
    TMBool ExecMask(const CWeakStr& name, const T& __val, const void*)
    {
        return SaveProp(name, &__val, &__val, PropMakeHex(__val), SS_TEXT), false;
    }

    TMBool ExecChar(const CWeakStr& name, const wchar_t& __val)
    {
        return SaveProp(name, &__val, &__val, PropMakeChar(__val), SS_TEXT), false;
    }

    TMBool ExecText(const CWeakStr& name, const CString& __val)
    {
        return ExecClass(name, __val);
    }

    TMBool ExecOle(const CWeakStr& name, const CString& __val)
    {
        return _SaveString(name, &__val, __val, SS_OLE);
    }

    template<class T>
    TMBool ExecClass(const CWeakStr& name, const T& __val)
    {
        return SaveProp(name, tm_addressof(__val), tm_addressof(__val), PropMake(__val)), false;
    }
};




TM_END_NAMESPACE

#endif // SAVESTREAM_H