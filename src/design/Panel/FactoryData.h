///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//factorydata.h


#ifndef     _FACTORY_LIST_DATA 
#define     _FACTORY_LIST_DATA


TM_BGN_NAMESPACE

class CTuiObject;
class CView;

TM_END_NAMESPACE


class CPanelApp;



class CObjData : public IObjEdit
{
public:
    TM::CTuiWidget  m_hWnd;
    TM::EObjType    m_eType;
    CPanelApp*      m_app;
    
public:
    TMBool          m_bFilter; // is match filter text
    TMBool          m_bSpyMode; // is spy mode

protected:
    CObjData()
        : m_bFilter (true)
        , m_bSpyMode(false)
    {
    }

public:
    virtual ~CObjData() {}

    virtual void Release();

    CPanelApp* GetApp() { return m_app; }


    virtual TMUInt64           GetId() tm_override;
    virtual void GetIdStr(TM::CString& str) { str.Format(L"%d", GetId()); }

    virtual TM::CString ID2Text(TM::CString* pStrUser = 0) tm_override;

    virtual IObjEdit* GetAncestorItem() { return this; }

    int GetRef();
    
    TM::CString GetObjClassName() tm_override;

    virtual TM::CString GetDebugInfo() tm_override
    {
        TM::CTuiObject* p = GetProp();
        if (p)
            return p->GetDebugInfo();
        return TM::CString();
    }

    void InvalidateObj();

    TMBool IsFilter() { return m_bFilter; }

    virtual TM::EObjType GetType()  { return m_eType; }

//     virtual TMBool RendThumbnail(IGXApp* gapp, const TM::CTuiRect& rc, EThumb eThumb);// { return false; }
    virtual int RendSample(IGXApp* gapp, int nState, const TM::CTuiRect& rc, ERender eRender);

    virtual TMBool GetObjRect(TM::CTuiRect& rc)
    {
        if (m_hWnd)
        {
            m_hWnd.GetWindowRect(&rc);
            return true;
        }
        return false; 
    }

    virtual EObjStyle GetObjStyle() { return m_hWnd ? (EObjStyle)(m_hWnd.GetStyle() & eObjStyleMask) : eObjStyleVisible; }

    virtual TMBool MoveItem(IObjEdit* pPos, EMoveParam mp);

    virtual void EditProp(TMBool bAddMode);
    
    virtual TM::IDomElement* SaveTo(TM::IDomNode* pParent);

    virtual void OnLoad(TM::IDomElement* pOld, TM::IDomElement* pNew);

    virtual TMBool _Match(const TM::CStrView& strItemTxt, const TM::CStrView& strFilter, int& nRootLevel) tm_override;


    virtual TM::CTuiObject* GetProp() = 0;
    virtual TMBool SetObjRect(const TM::CTuiRect& rc);
};


typedef TM::CMultiSet<TM::CString> CIDList;
typedef CIDList::recorder           CIDRec;


#endif
