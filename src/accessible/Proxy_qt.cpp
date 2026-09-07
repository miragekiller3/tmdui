/**
 *  @file
 *  @brief
 *  MSAA Proxy object for QT implementing class.<br>
 *  Generate MSAA object for TMDui and handle event with QT system;
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.08.08
 *
 *  Copyright (C) 2009-2020 miragekiller
 */

#include "StdAfx.h"
#include "tmdef.h"

#ifdef TM_QT

#include <QAccessibleInterface.h>
#include "Proxy.h"

class CAccessibleObj_wrap : public QAccessibleInterface
{
private:
    CComPtr<CAccessibleObj> m_ptr;
    int                     m_nChildID;

private:
    QAccessibleInterface* Var2Interface(VARIANT& var)
    {
        if (vt != VT_DISPATCH)
            return CreateInstance(m_ptr, vt.lVal);
        else
            return CreateInstance(static_cast<CAccessibleObj*>(var.pdispVal));
    }

    CAccessibleObj_wrap(CAccessibleObj* ptr, int nChildID)
        : m_ptr(ptr)
        , m_nChildID(nChildID)
    {
    }

    ~CAccessibleObj_wrap()
    {
        if (m_nChildID == CHILDID_SELF)
            m_ptr->m_wrap = 0;
    }

public:
    static QAccessibleInterface* CreateInstance(CAccessibleObj* ptr, int nChildID = CHILDID_SELF)
    {
        if (nChildID)
            return new CAccessibleObj_wrap(ptr, nChildID);
        if (!ptr->m_wrap)
            ptr->m_wrap = new CAccessibleObj_wrap(ptr, CHILDID_SELF);
        return ptr->m_wrap;
    }

    // QAccessibleInterface
    // check for valid pointers
    virtual bool isValid() const tm_override { return true; }
    virtual QObject *object() const tm_override
    {
        return m_ptr->m_owner;
    }

    virtual QWindow *window() const tm_override
    {
        return m_ptr->m_owner;
    }
    
    // relations
//     virtual QVector<QPair<QAccessibleInterface*, QAccessible::Relation> > relations(QAccessible::Relation match = QAccessible::AllRelations) const tm_override;
    virtual QAccessibleInterface* focusChild() const tm_override
    {
        CComVariant var;
        m_ptr->get_accFocus(&var);
        return Var2Interface(var);
    }
    
    virtual QAccessibleInterface *childAt(int x, int y) const tm_override
    {
        CComVariant var;
        m_ptr->accHitTest(x, y, &var);
        return Var2Interface(var);
    }
    
    // navigation, hierarchy
    virtual QAccessibleInterface* parent() const tm_override
    {
        if (m_nChildID != CHILDID_SELF)
            return CreateInstance(m_ptr, CHILDID_SELF);
        CComPtr<IDispatch> ptr;
        m_ptr->get_accParent(&ptr);
        return CreateInstance(static_cast<CAccessibleObj*>(ptr.p), CHILDID_SELF);
    }

    virtual QAccessibleInterface* child(int index) const tm_override
    {
        CComPtr<IDispatch> ptr;
        m_ptr->get_accChild(CComVariant(index), &ptr);
        if (ptr)
            return CreateInstance(static_cast<CAccessibleObj*>(ptr.p), CHILDID_SELF);
        if (index > childCount())
            return 0;
        return CreateInstance(m_ptr, index);
    }

    virtual int childCount() const tm_override
    {
        long count = 0;
        m_ptr->get_accChildCount(&count);
        return count;
    }

    virtual int indexOfChild(const QAccessibleInterface *) const tm_override
    {
        DbgAssert(0);
        return -1;
    }
    
    // properties and state
    virtual QString text(QAccessible::Text t) const tm_override
    {
        CComBSTR bstr;
        switch (t)
        {
        case QAccessible::Name:
            m_ptr->get_accName(CComVariant(CHILDID_SELF), &bstr);
            return TM_T2Q(bstr);

        case QAccessible::Description:
            m_ptr->get_accDescription(CComVariant(CHILDID_SELF), &bstr);
            return TM_T2Q(bstr);

        case QAccessible::Value:
            m_ptr->get_accValue(CComVariant(CHILDID_SELF), &bstr);
            return TM_T2Q(bstr);

        case QAccessible::Help:
            m_ptr->get_accHelp(CComVariant(CHILDID_SELF), &bstr);
            return TM_T2Q(bstr);

        case QAccessible::Accelerator:
            m_ptr->get_accKeyboardShortcut(CComVariant(CHILDID_SELF), &bstr);
            return TM_T2Q(bstr);

        case QAccessible::DebugDescription:
        case QAccessible::UserText:
        default:
            return QString();
        }
    }

    virtual void setText(QAccessible::Text t, const QString &text) tm_override
    {
        DbgAssert(0);
    }

    virtual QRect rect() const tm_override
    {
        long l = 0, t = 0, w = 0, h = 0;
        m_ptr->accLocation(&l, &t, &w, &h, CComVariant(m_nChildID));
        TM::CRect rc;
        rc.SetLTWH(l, t, w, h);
        return rc;
    }

    virtual QAccessible::Role role() const tm_override
    {
        CComVariant role;
        m_ptr->get_accRole(CComVariant(m_nChildID), &role);
        return (QAccessible::Role)role.lVal;
    }

    virtual QAccessible::State state() const tm_override
    {
        CComVariant state;
        m_ptr->get_accState(CComVariant(m_nChildID), &state);
        return (QAccessible::State)state.lVal;
    }
    
//     virtual void virtual_hook(int id, void *data);
    
    virtual void *interface_cast(QAccessible::InterfaceType)
    { return Q_NULLPTR; }
};

QAccessibleInterface* GetAccObjectQT(TM::HTUI hWgt)
{
    DbgAssert(hWgt);
    
    ACC_TRACE(L"GetAccObject(%s, wParam:%X, lParam:%X)", _acc_dinfo2(hWgt).c_str(), wParam, lParam);
    
    CAccessibleObj *pIAccessible = CAccessibleWrapper::GetAccClient(hWgt);
    return CAccessibleObj_wrap::CreateInstance(pIAccessible);
}

#endif

