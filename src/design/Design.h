#ifndef EDITOR_H
#define EDITOR_H


#include "tmlist.h"
#include "tmtime.h"


#ifdef TUI_EDIT_MODE
#   define EDITOR_API       __declspec(dllexport)
#   define EDITOR_IMPORT
#else
#   define EDITOR_API
#   define EDITOR_IMPORT    __declspec(dllimport)
#endif

#   define HEDITOR          ((TMHModule)0) //-1)

class CMainWnd;
class IProj;
class IObjEdit;
class IPropBox;
class IProjMgr;
struct IAccessible;

const int CTRL_WIDTH        = 200;
const int CTRL_HEIGHT       = 20;
const int CTRL_COMHEIGHT    = 50;

#define DRAG_WND_CLASS      _T("widget drag wnd class")
#define DRAG_TV_ITEM        _T("widget drag tv item")

#define _DInfo_AppendOrderUp(str)    str.Append(L"\r\n  order display : " L"up item is Z top") 
#define _DInfo_AppendOrderDown(str)  str.Append(L"\r\n  order display : " L"down item is Z top") 

inline void Str2Html(TM::CString& str)
{
    str.Replace(L"<", L"&lt;");
    str.insert(0, L"<pre>");
    str.append(L"</pre>");
}


#define EDIT_MODE TM::EIDTYPE
typedef EZOrder EMoveParam;

enum EPlugInMode
{
    ePlugInModeUnknown = 0,
    ePlugInModeEditor,
    ePlugInModeActiveX,
    ePlugInModeSpy,
};

#define objEditMgr      ((IObjEdit*)(TM::e_otWnd + 1))
#define objEditFrm      ((IObjEdit*)(TM::e_otFrm + 1))
#define objEditPic      ((IObjEdit*)(TM::e_otPic + 1))
#define objEditMax      ((IObjEdit*)10)
#define objLevelRoot    2 // mgr,frame,pic root level. 

// for QT
#define OT_IMAGE        ((TM::EObjType)(TM::e_otWgt + 1))
#define OT_FONT         ((TM::EObjType)(OT_IMAGE + 1))
#define OT_ARRAYDATA    ((TM::EObjType)(OT_FONT + 1))


TM_BGN_NAMESPACE
class CView;
enum EPopable;
TM_END_NAMESPACE

struct CTuiAppTester : public TM::CTuiApp
{
    virtual TM::CView& GetView() = 0;
};

CTuiAppTester* AppGetTester();
TM::CTuiApp* AppGetEditor();
TM::CView* AppGetDoc(); // only for edit mode, 
inline TM::CTuiApp* AppGetSpy() { return AppGetTester(); } // only spy mode

// cycle == 0: the message only in, no out
void SpyMessage(HWND hWnd, TM::TUIMSG* pMsg, TMResult* lr, int& step, const double* cycle);
void SpyFunction(LPCTSTR pStr, TM::TUIMSG* pMsg, TMResult* lr, int& step, double cycle);

static LPCTSTR const strFmtAPP    =  L"(%p)APP(%d) ";
static LPCTSTR const strFmtWGT    =  L"(%p)WGT(%d) ";
static LPCTSTR const strFmtFUN    =  L"(%p)FUN(%d) ";
static LPCTSTR const strFmtQPP    =  L"(%p)APP ";
static LPCTSTR const strFmtQT     =  L"(%p)EVT %s ";    // <step> (QObject*)EVT event (classname)objname
static LPCTSTR const strFmtQPOST  =  L"(%p)PST %s ";    // <step> (QObject*)PST event (classname)objname
static LPCTSTR const strFmtSET    =  L"(%p)SET ";       // <step> (QObject*)SET propname (classname)objname: propvalue  
static LPCTSTR const strFmtGET    =  L"(%p)GET ";       // <step> (QObject*)GET propname (classname)objname: propvalue  
static LPCTSTR const strFmtQF     =  L"(%p)FUN ";       // <step> (QObject*)FUN funcname (classname)objname: p1 p2 p3; ret:r  

struct CSpyMsg
{
    enum EType { eWnd, eWidget, eLog, eFunc, eHookEvent, eEnd };
    enum EMode { eOnce, eIn, eOut };
    TM::CCycle cycle;
//     int  oid;
    bool bBlock;        // fill by SpyMsgFunc; the message need blocked;

    CSpyMsg() : /*oid(_o),*/ bBlock(false) {}
    void AppendCycle(TM::CString& str) { str.AppendFormat(L"(%d us)", cycle.Elapse().ToMicroSecond()); }

    virtual void ToItemString(TM::CString& str, TM::CString& strHint, EMode eMode) = 0;
};

// the function will fill bBlock
void SpyMsgFunc(CSpyMsg& sm, CSpyMsg::EMode eMode, CSpyMsg::EType eType, int tid = 0);


struct PANELINFO
{
    HWND hWnd;
    HINSTANCE hResWin;
    IProjMgr* pMainWnd;
};
IProj* CreateProj(PANELINFO& info);
IProj* CreateSpyProj(PANELINFO& info);
IProj* CreateQtProj(PANELINFO& info);


//used to identify if this ID is abandoned; id/count
typedef TM::CMap<TMUInt64, int> CIDUsedList;

enum ESaveMode
{
    eSaveModeNormal         = 0,
    eSaveModeSel            = 0x01,
    eSaveModeTest           = 0x02,
    eSaveModeSkin           = 0x03,
    eSaveModeLang           = 0x04,
    eSaveModeMask           = 0x0f,
    
    // only for eSaveModeLang
    eSaveLangStringTable    = 0x10,
    eSaveLangMenu           = 0x20,
};

enum ESysInfo
{
    eSysInfoRender,
};

enum EObjStyle
{
    eObjStyleNone           = 0,
    eObjStyleVisible        = WS_VISIBLE,
    eObjStyleDisable        = WS_DISABLED,
    EObjStyleTabstop        = WS_TABSTOP,
    eObjStyleMask           = eObjStyleVisible | eObjStyleDisable | EObjStyleTabstop,
};
TM_ENUM_2_FLAG(EObjStyle);


struct CPreSearchID
{
    typedef TM::CSet<TM::CStringA> CMacoList;
    typedef TM::CMap<TM::CStringA, CMacoList> CParentList;
    CParentList aID;
    CMacoList   aDel;
};

// typedef CMap<TMUInt32, CIDList> CParentList; // if win id used, remove it all parent id.

typedef TM::CVector<std::pair<TM::CString, int> > CPopIDList;

// IProject is a new APP.
class IProj
{
public:
    virtual void Release() = 0;
   
    // proj settings
    virtual TMCStr* GetObjRootNames() = 0;// { return 0; }  // &(TMCStr[3]);
    virtual void GetDeviceNames(TMCStr** ppDisplay, TMCStr** ppText) = 0; // null end
    virtual TMCStr GetClassByIndex(TM::EObjType eType, int n) = 0;
    typedef TM::CVector<TMHModule>   CModuleListH;
    typedef TM::CVector<TM::CString> CModuleListS;
    virtual void GetModuleList(CModuleListH* aListH, CModuleListS* aListS) = 0;

    virtual TMResult TrackMenuPop(HWND hWnd, TMPoint pt, HMENU hMenuRoot, IObjEdit* pSel) = 0; // pSel can be objEditXXX
    virtual void ModifyUser(LPCTSTR pNameOld, const TM::CStrView& pNameNew) = 0;
    virtual void LoadLanguage(LPCTSTR pFileName) = 0;
    virtual TMBool SaveFile(LPCTSTR pFileName, ESaveMode eMode, TM::IDomDocument** xmlOut = 0) = 0;
    virtual TM::CString LoadFromFile(LPCTSTR pFileName, TMBool bMerge) = 0;
    static TMUInt32 GetFileVersion();
    
    virtual void ResetUserIds(const TM::CStrView& strUser, int idStart) = 0;
    virtual int GetUsers(CPopIDList& aMacro) = 0;
    virtual void MergeUsers(CPopIDList& aMacro) = 0; // merge all macro to first item.

    virtual TM::CString GetPreSearchID(CPreSearchID& aList) = 0;
    virtual TM::CString SetObjectIDMacro(const TM::CStrView& pOld, const TM::CString& strNew) = 0; // return: err string

    virtual IObjEdit* SyncAncestor(TM::IDomNode* ptrOld, TM::IDomNode* ptrNew) = 0;
    virtual IObjEdit* PasteTo(IObjEdit* pParent, TM::IDomNode* ptr) = 0;

    virtual void EnableSpyMessage(CSpyMsg::EType eFlag, BOOL bEnable) {}
    virtual void SetSpyObjMessage(IObjEdit* obj, BOOL bTree) {} // obj == 0: clear setting;
    virtual void TrackFocusObject(TMBool bEnalbe) {}

    virtual void AppendSysInfo(TM::CString& str, ESysInfo eType) = 0;

    virtual void TesterRTL(TMBool bKeep) = 0;
    virtual void TesterLanguage(LPCTSTR p) = 0;
    virtual void TesterChangeDevice(int bDisplay, int id) = 0;



    // spy project
//     virtual void SpySetEmbed(int nMode) {}
};


class ITreeItem
{
private:
    // if subitem show, the rootlevel->subitem all show
    virtual TMBool _Match(const TM::CStrView& strItemTxt, const TM::CStrView& strFilter, int& nRootLevel) = 0;

public:
    TMBool Match(const TM::CStrView& strItemTxt, const TM::CStrView& strFilter, int& nRootLevel)
    {
        if (!strFilter)
            return _Match(strItemTxt, strFilter, nRootLevel);

        TM::CStrView str;
        for (size_t n = 0; n = strFilter.Split(n, ';', str);)
        {
            if (_Match(strItemTxt, str, nRootLevel))
                return true;
        }
        return false;
    }

    virtual void Release() = 0;
};

class IObjEdit : public ITreeItem
{
public:
    enum ERender { eRenderForView, eRenderForTree, eRenderThumbAll, eRenderThumbNoChild };

protected:
    TM::HTVITEM  m_hItemTree;

protected:
    ~IObjEdit() {}
    
public:
    IObjEdit() : m_hItemTree(0) {}

    TM::HTVITEM GetTvData()
    {
#ifdef TM_DBG
        void* p = m_hItemTree ? TM::CTVItem(m_hItemTree).GetData() : 0;
        if (p)
            DbgAssert(p == this);
#endif
        return m_hItemTree;
    }
    
    void SetTvData(TM::HTVITEM data)
    {
#ifdef TM_DBG
        void* p = TM::CTVItem(data).GetData();
        if (p)
            DbgAssert(p == this);
#endif
        DbgAssert(!m_hItemTree);
        m_hItemTree = data;
    }

public:
    virtual int GetRef() = 0;
    virtual TMUInt64 GetId() = 0;
    virtual void GetIdStr(TM::CString& str) = 0;
    virtual TM::CString ID2Text(TM::CString* pStrUser = 0) = 0; // return: strMacro
    virtual TM::EObjType GetType() = 0;
    virtual IObjEdit* GetAncestorItem() = 0;
    virtual TM::CString GetObjClassName() = 0;
    virtual TM::CString GetDebugInfo() = 0;
    virtual TM::CString GetAccInfo() { return TM::CString(); }
    virtual bool IsSameAcc(IAccessible* acc) { return false; }
    virtual int RendSample(IGXApp* gapp, int nState, const TM::CTuiRect& rc, ERender eRender) = 0; // return: current frame; error:-1;
    virtual TMBool GetObjRect(TM::CTuiRect& rc) = 0;
    virtual TMBool SetObjRect(const TM::CTuiRect& rc) = 0;
    virtual EObjStyle GetObjStyle() = 0;

    virtual TMBool IsFilter() = 0;
    virtual TM::IDomElement* SaveTo(TM::IDomNode* pParent) = 0;
    virtual void OnLoad(TM::IDomElement* pOld, TM::IDomElement* pNew) = 0;
    virtual void InvalidateObj() = 0;

    virtual void EditProp(TMBool bAddMode) = 0;

    virtual TMBool MoveItem(IObjEdit* pPos, EMoveParam mp) = 0;
    virtual void DeleteAfter5s() {}
};

// {
//     ePopableNone, ePopableButton, ePopableComb
// };

class IPropBox  : public ITreeItem
{
public:
    bool m_bSameVal;
    bool m_bDisable;
    
public:
    IPropBox()
        : m_bDisable (false)
        , m_bSameVal (false)
    {
    }
    
    virtual void DrawName (const TM::CRectF& rc, TMBool bSelected) = 0;
    virtual void DrawValue (const TM::CRectF& rc) = 0;
    virtual TMUInt32 GetLocateID() = 0;
    virtual TM::CStrView GetPropHint() = 0;
    //     virtual void HitTest(TM::TUIHITINFO* hi) {};
    virtual int Matching(const TM::CStrView& str) = 0;

    virtual int RemoveOwner(IObjEdit* pOwner) = 0; // return: new owner size
    
    // return: is same value;
    virtual TMBool T2Editer(TM::CString& str) = 0;
    virtual TMBool Editer2T(const TM::CString& str) = 0;
    
    // str: in/out
    virtual TMBool ShowPopup(TM::CString& str) { return false; }
    
    virtual TM::EPopable InitPopable(TMBool bSelect) = 0;// { return ePopableNone; } //0: not pop, 1:button; 2:combo button
    
    static int ItemMidle(const TM::CRect& rc)
    {
        return rc.Left() + rc.Width() * 2 / 5;
    }
};


class IProjMgr
{
public:
    virtual void Invalidate() = 0;
    virtual void GetPadInfo(TM::CTuiPoint& pt, int& dpi) = 0;
    virtual void Alert(TMCStr pFmt, ...) = 0;
    virtual void OnTrackFocus(void* pNewObj, const TM::CRect& rc, const TM::CString& strAlert) = 0;

    // item
    virtual IObjEdit* ItemCreate(IObjEdit* pParent, const CDomStrView& pClassName, TMRect* prc = 0) = 0;
    virtual void ItemAdd(IObjEdit* pPos, EZOrder mp, IObjEdit* pData, TM::EObjType eType) = 0;
    virtual void ItemRemove(IObjEdit* pPos) = 0;
    virtual void ItemMoveOrAdd(IObjEdit* pPos, EZOrder mp, IObjEdit* pData) = 0;
    virtual IObjEdit* ItemGetParent(IObjEdit* pItem) = 0;
    virtual IObjEdit* ItemGetChild(IObjEdit* pItem) = 0;
    virtual IObjEdit* ItemGetNext(IObjEdit* pItem) = 0;
    virtual IObjEdit* ItemGetSelectedData(TMBool bGetOwner) = 0;
    virtual TMUInt ItemGetState(IObjEdit* pItem) = 0;

    // prop
    virtual TMBool PropInitPopupID(const CPopIDList& aID) = 0;
    virtual TMBool PropShowPopupString(TM::CString& str) = 0;
    virtual TMBool PropShowPopupFont(LOGFONT& lf) = 0;
    virtual TMBool PropShowPopupColor(TMColor& clr) = 0;
    virtual TMBool PropShowPopupFile(TM::CString& str) = 0;
    virtual TMBool PropShowPopupOLE(TM::CString& str) = 0;
    virtual int PropPopupMenu(HMENU hMenu) = 0;
    virtual void PropSetCurrentText(LPCTSTR pStr) = 0;
    virtual TMBool PropIsFocus() = 0;
    virtual void PropAdd(const TM::CStrView& pName, IPropBox* pBox) = 0;
    virtual TM::CStringW PropBegin(IObjEdit* p) = 0;
    virtual void PropEnd(IObjEdit* p, const TM::CStringW& key) = 0;

};




#endif // EDITOR_H