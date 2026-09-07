/*
 *  @file
 *  @brief  Archive support
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2017.6.29
 *
 *  Copyright (C) 2017 miragekiller
 */

#pragma once

#ifndef TMARCHIVE_H
#define TMARCHIVE_H

#   include "tmstring.h"


TM_BGN_NAMESPACE

// O: output; save object to file;
// I: input:  restore obejct from file;

// CDomDocument::CArchiveX, CFile::CArchiveX, CCfg::CArchiveX, CRegKey::CArchiveX

// user can overload ArchiveSerialize(), operator <<, operator >> to add self class support

///////////////////////////////////////////////////////////////////////////
// base class of all archive

// archive type
struct CArchiveTypeBinary   { enum { eBinary = true }; };
struct CArchiveTypeTxt      { enum { eBinary = false }; };
struct CArchiveTypeOutput   { enum { eSave = true }; };
struct CArchiveTypeInput    { enum { eSave = false }; };
struct CArchiveTypeBinaryOutput : public CArchiveTypeBinary, public CArchiveTypeOutput {};
struct CArchiveTypeBinaryInput  : public CArchiveTypeBinary, public CArchiveTypeInput {};
struct CArchiveTypeTxtOutput : public CArchiveTypeTxt, public CArchiveTypeOutput {};
struct CArchiveTypeTxtInput  : public CArchiveTypeTxt, public CArchiveTypeInput {};

// CArchiveBaseT
// operator & will call operator << and operator >>.
// add serialize for self class:
//   template <class T> void ArchiveSerialize(CArchiveBaseT<T>& ar, CMyClass& c) { ... }
//   template <class T> void ArchiveSerialize(T& ar, CMyClass& c) { ... }
template <class Archive> struct CArchiveBaseT
{
    typedef Archive  archive_type;
    enum { eDefaultStringBufferSize = 2048 }; // for char c[n]; archive >> c;

    bool m_bFail;

    CArchiveBaseT() : m_bFail (false) {}

    archive_type& This() const { return *const_cast<archive_type*>(static_cast<const archive_type*>(this)); };

    template<class T> archive_type& operator & (T& __x) const { return this->This() & __x; }

    void SetStat(TMBool bSuccess) { m_bFail = (0 == bSuccess); }
    bool IsFail() const { return m_bFail; }

    // for text archive.
    template <class S, class T> void EntrySection(S& storage, const T& str) { this->This().DoEntrySection(storage, str); }
    template <class S> void LeaveSection(S& storage) { this->This().DoLeaveSection(storage); }
    template <class T> void SetKey(const T& str) { this->This().DoSetKey(str); }
};

// default serialize
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, bool& __x)           { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, TMInt8& __x)         { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, TMUInt8& __x)        { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, TMInt16& __x)        { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, TMUInt16& __x)       { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, TMInt32& __x)        { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, TMUInt32& __x)       { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, TMLong& __x)         { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, TMULong& __x)        { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, TMInt64& __x)        { ar.This().DoSerializeI64(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, TMUInt64& __x)       { ar.This().DoSerializeI64(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, float& __x)          { ar.This().DoSerializeFloat(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, double& __x)         { ar.This().DoSerializeFloat(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, TMCStrA& __x)        { ar.This().DoSerializeString(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, TMCStrW& __x)        { ar.This().DoSerializeString(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, CStringA& __x)       { ar.This().DoSerializeString(__x); }
template <class Archive> void ArchiveSerialize(CArchiveBaseT<Archive>& ar, CStringW& __x)       { ar.This().DoSerializeString(__x); }

template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, bool __x)           { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, TMInt8 __x)         { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, TMUInt8 __x)        { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, TMInt16 __x)        { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, TMUInt16 __x)       { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, TMInt32 __x)        { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, TMUInt32 __x)       { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, TMLong __x)         { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, TMULong __x)        { ar.This().DoSerializeInt(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, TMInt64 __x)        { ar.This().DoSerializeI64(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, TMUInt64 __x)       { ar.This().DoSerializeI64(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, float __x)          { ar.This().DoSerializeFloat(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, double __x)         { ar.This().DoSerializeFloat(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, TMCStrA __x)        { ar.This().DoSerializeString(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, TMCStrW __x)        { ar.This().DoSerializeString(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, const std::string_view& __x)    { ar.This().DoSerializeString(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, const std::wstring_view& __x)   { ar.This().DoSerializeString(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, const std::string& __x)         { ar.This().DoSerializeString(__x); }
template <class Archive> void ArchiveSerializeO(CArchiveBaseT<Archive>& ar, const std::wstring& __x)        { ar.This().DoSerializeString(__x); }


///////////////////////////////////////////////////////////////////////////
// base class of all archive
// CArchiveBaseOT & CArchiveBaseIT
// add serialize for self class:
//   template <class T> void operator << (CArchiveBaseOT<T>& ar, const CMyClass& c) { ... }
//   template <class T> void operator << (T& ar, const CMyClass& c) { ... }
//   template <class T> void operator >> (const CArchiveBaseOT<T>& ar, CMyClass& c) { ... }
//   template <class T> void operator >> (const T& ar, CMyClass& c) { ... }
template <class Archive> struct CArchiveBaseOT : public CArchiveBaseT<Archive>
{
    enum { eSave = true };
    enum { eLoad = false };
    typedef CArchiveTypeOutput                              type_direction;
    typedef typename CArchiveBaseT<Archive>::archive_type   archive_type;

    template<class T> archive_type& operator &  (const T& __x) const { (this->This()) << __x; return this->This(); }
    template<class T> archive_type& operator << (const T& __x) const { ArchiveSerializeO(this->This(), __x); return this->This(); }
};

template <class Archive> struct CArchiveBaseIT : public CArchiveBaseT<Archive>
{
    enum { eSave = false };
    enum { eLoad = true };
    typedef CArchiveTypeInput                               type_direction;
    typedef typename CArchiveBaseT<Archive>::archive_type   archive_type;


    template<class T> archive_type& operator &  (const T& __x) const { (this->This()) >> const_cast<T&>(__x); return this->This(); }
    template<class T> archive_type& operator >> (T& __x) const { ArchiveSerialize(this->This(), __x); return this->This(); }
};


///////////////////////////////////////////////////////////////////////////
// base class of binary archive
// for mem, file
// CArchiveBinaryOImplT & CArchiveBinaryIImplT
// Endian: CEndianNet, CEndianLittle, CEndianHost;
template <class Archive, class Endian> struct CArchiveBinaryOImplT : public CArchiveBaseOT<Archive>
{
public:
    enum { eBinary = true };
    typedef CArchiveTypeBinary                      type_format;
    typedef CArchiveTypeBinaryOutput                type_all;
    typedef Endian                                  endian_type;

    void DoSerializeString(const TM::A2UTF& vol)
    {
        TMUInt32 n = vol.GetLength();
        (*this) & (n);
        this->This().DoSerializeVoid(vol.c_str(), n);
    }

    void DoSerializeString(const TM::W2UTF& vol)
    {
        TMUInt32 n = vol.GetLength();
        (*this) & (n);
        this->This().DoSerializeVoid(vol.c_str(), n);
    }

    void DoSerializeFloat(float __x)
    {
        DbgAssertS(sizeof(float) == sizeof(TMUInt32));
        DoSerializeInt(*(TMUInt32*)&__x);
    }

    void DoSerializeFloat(double __x)
    {
        DbgAssertS(sizeof(double) == sizeof(TMUInt64));
        DoSerializeI64(*(TMUInt64*)&__x);
    }

    template <class T> void DoSerializeInt(T __x)
    {
        __x = endian_type::FromHost(__x);
        this->This().DoSerializeVoid(&__x, sizeof(T));
    }

    template <class T> void DoSerializeI64(T __x)
    {
        __x = endian_type::FromHost(__x);
        this->This().DoSerializeVoid(&__x, sizeof(T));
    }
};

template <class Archive, class Endian> struct CArchiveBinaryIImplT : CArchiveBaseIT<Archive>
{
public:
    enum { eBinary = true };
    typedef CArchiveTypeBinary                      type_format;
    typedef CArchiveTypeBinaryInput                 type_all;
    typedef Endian                                  endian_type;

private:
    void _Read(void* p, size_t n) const { this->This().DoSerializeVoid(p, n); }

public:
    void DoSerializeString(CStringA& vol) const
    {
        TMUInt32 n;
        (*this) & (n);
        CStringA str;
        _Read(str.GetBuffer(n), n);
        TM::UTF2A ua(str);
        vol = ua;
    }

    void DoSerializeString(CStringW& vol) const
    {
        TMUInt32 n;
        (*this) & (n);
        CStringA str;
        _Read(str.GetBuffer(n), n);
        vol = TM::UTF2W(str);
    }

    void DoSerializeString(TMStrA pVol) const
    {
        TMUInt32 n;
        (*this) & (n);
        CStringA str;
        _Read(str.GetBuffer(n), n);
        TM::UTF2A(str).CopyTo(pVol);
    }

    void DoSerializeString(TMStrW pVol) const
    {
        TMUInt32 n;
        (*this) & (n);
        CStringA str;
        _Read(str.GetBuffer(n), n);
        TM::UTF2W(str).CopyTo(pVol);
    }

    void DoSerializeFloat(float& __x)
    {
        DbgAssertS(sizeof(float) == sizeof(TMUInt32));
        DoSerializeInt(*(TMUInt32*)&__x);
    }

    void DoSerializeFloat(double& __x)
    {
        DbgAssertS(sizeof(double) == sizeof(TMUInt64));
        DoSerializeI64(*(TMUInt64*)&__x);
    }

    template <class T> void DoSerializeInt(T& __x)
    {
        this->This().DoSerializeVoid(&__x, sizeof(T));
        __x = endian_type::ToHost(__x);
    }

    template <class T> void DoSerializeI64(T& __x)
    {
        this->This().DoSerializeVoid(&__x, sizeof(T));
        __x = endian_type::ToHost(__x);
    }
};


///////////////////////////////////////////////////////////////////////////
// proxy of binary archive
// CArchiveBinaryOT & CArchiveBinaryIT
template <class T> struct CBinaryArciveFileT
{
    typedef void* section_type;
    T& m_file;
    CBinaryArciveFileT(T& f) : m_file(f) {}

    void DoEntrySection(section_type& storage, TMCStr) {}
    void DoLeaveSection(section_type& storage) {}
    void DoSetKey(TMCStr) const {}
};

template <class T, class Endian>
struct CArchiveBinaryOT
    : public CArchiveBinaryOImplT<CArchiveBinaryOT<T, Endian>, Endian>
    , public CBinaryArciveFileT<T>
{
    using CBinaryArciveFileT<T>::m_file;

    CArchiveBinaryOT(T& f) : CBinaryArciveFileT<T>(f) {}

    void DoSerializeVoid(const void* p, size_t n)
    {
        m_file.Write(p, n);
    }
};

template <class T, class Endian>
struct CArchiveBinaryIT
    : public CArchiveBinaryIImplT<CArchiveBinaryIT<T, Endian>, Endian>
    , public CBinaryArciveFileT<T>
{
    using CBinaryArciveFileT<T>::m_file;

    CArchiveBinaryIT(T& f) : CBinaryArciveFileT<T>(f) {}

    void DoSerializeVoid(void* p, size_t n)
    {
        m_file.Read(p, n);
    }
};


///////////////////////////////////////////////////////////////////////////
// base class of text archive
// for ini file, reg, DOM
// CArchiveTxtOImplT & CArchiveTxtIImplT
template <class Archive, class ch>
struct CArchiveTxtOImplT : public CArchiveBaseOT<Archive>
{
public:
    enum { eBinary = false };
    typedef ch                                      char_type;
    typedef CArchiveTypeTxt                         type_format;
    typedef CArchiveTypeTxtOutput                   type_all;

    void DoSerializeInt(long __x)
    {
        this->This().DoSerializeString(L2TT<char_type>(__x));
    }

    void DoSerializeI64(long __x) { DbgAssert(0); } // don't support 64 bit

    void DoSerializeFloat(double __x)
    {
        this->This().DoSerializeString(F2TT<char_type>(__x));
    }
};

template <class Archive, class ch>
struct CArchiveTxtIImplT : public CArchiveBaseIT<Archive>
{
public:
    enum { eBinary = false };
    typedef CArchiveTypeTxt                         type_format;
    typedef CArchiveTypeTxtInput                    type_all;
    typedef ch                                      char_type;
    typedef typename string_traits<ch>::other_char      other_char;
    using CArchiveBaseIT<Archive>::eDefaultStringBufferSize;

    template <class T>
    void DoSerializeInt(T& __x)
    {
        CStringT<char_type> str;
        this->This().DoSerializeString(str);
        if (!this->IsFail())
            __x = str.ToLong(__x);
    }

    template <class T> void DoSerializeI64(T& __x) { DbgAssert(0); }  // don't support 64 bit

    template <class T>
    void DoSerializeFloat(T& __x)
    {
        CStringT<char_type> str;
        this->This().DoSerializeString(str);
        if (!this->IsFail())
            __x = str.ToDouble(__x);
    }

//     void DoSerializeString(CStringW& str)
//     {
//         CStringT<char_type> strRef;
//         int n = this->This().DoSerializeString(strRef.GetBuffer(eDefaultStringBufferSize));
//         if (!this->IsFail())
//         {
//             strRef.SetLength(n);
//             strRef.MoveTo(str);
//         }
//     }
//
//     void DoSerializeString(CStringA& str)
//     {
//         CStringT<char_type> strRef;
//         int n = this->This().DoSerializeString(strRef.GetBuffer(eDefaultStringBufferSize));
//         if (!this->IsFail())
//         {
//             strRef.SetLength(n);
//             strRef.MoveTo(str);
//         }
//     }

//     template <class T>
//     void _DoSerializeString(CStringT<T>& str)
//     {
//         CStringT<char_type> strRef;
//         int n = this->This().DoSerializeString(strRef.GetBuffer(eDefaultStringBufferSize));
//         if (!this->IsFail())
//         {
//             strRef.SetLength(n);
//             strRef.MoveTo(str);
//         }
//     }

//     void DoSerializeString(std::basic_string<char_type>& str)
//     {
//         CStringT<char_type> strRef;
//         int n = this->This().DoSerializeString(strRef.GetBuffer(eDefaultStringBufferSize));
//         if (!this->IsFail())
//         {
//             strRef.SetLength(n);
//             strRef.Swap(str);
//         }
//     }
//
//     void DoSerializeString(std::basic_string<other_char>& str)
//     {
//         CStringT<char_type> str2;
//         this->This().DoSerializeString(str2);
//         if (!this->IsFail())
//             str2.CopyTo(str);
//     }

//     void DoSerializeString(CStringW& str)
//     {
//         _DoSerializeString(str);
//     }
//
//     void DoSerializeString(CStringA& str)
//     {
//         _DoSerializeString(str);
//     }

    template <class T>
    void _DoSerializeString(CStringT<T>& str)
    {
        CStringT<char_type> str2;
        this->This().DoSerializeString(str2);
        if (!this->IsFail())
            str2.CopyTo(str);
    }

//    template <>
    void _DoSerializeString(CStringT<char_type>& str)
    {
        CStringT<char_type> strRef;
        int n = this->This().DoSerializeString(strRef.GetBuffer(eDefaultStringBufferSize));
        if (!this->IsFail())
        {
            strRef.SetLength(n);
            strRef.swap(str);
        }
    }

    void DoSerializeString(CStringW& str)
    {
        _DoSerializeString(str);
    }

    void DoSerializeString(CStringA& str)
    {
        _DoSerializeString(str);
    }

    void DoSerializeString(char_type* str)
    {
        CStringT<char_type> str2;
        this->This().DoSerializeString(str2);
        if (!this->IsFail())
            str2.CopyTo(str);
    }

    void DoSerializeString(other_char* str)
    {
        CStringT<char_type> str2;
        this->This().DoSerializeString(str2);
        if (!this->IsFail())
            str2.CopyTo(str);
    }
};


///////////////////////////////////////////////////////////////////////////
// archive for default value
template <class T>
struct CArchiveTxtDefaultProxyT : public CArchiveBaseIT<CArchiveTxtDefaultProxyT<T> >
{
    typedef CArchiveTxtDefaultProxyT<T> archive_type;
    const T& m_val;
    CArchiveTxtDefaultProxyT(const T& val) : m_val (val) {}
    template <class P>
    archive_type& operator >> (P& __x) const { __x = m_val; return this->This(); }
};

template <class T>
CArchiveTxtDefaultProxyT<T> ArchiveMakeDefaulProxy(const T& p) { return CArchiveTxtDefaultProxyT<T>(p); }



///////////////////////////////////////////////////////////////////////////
// DDX for archive
#define SERIALIZE_BGN_MAP2(Func, thisClass) \
    template <class T> void Func##Input(T& file) { typename T::CArchiveI ar(file); Func(ar); } \
    template <class T> void Func##Output(T& file) { typename T::CArchiveO ar(file); Func(ar); } \
    template <class T> void Func(T& archive) { typedef typename T::section_type section_type;


#define SERIALIZE_BGN_MAP(thisClass) SERIALIZE_BGN_MAP2(DoSerialize, thisClass)


#define SERIALIZE_END_MAP()             }

#define SERIALIZE_ENTRY(section)        { section_type _temp; archive.EntrySection(_temp, section);
#define SERIALIZE_LEAVE()               archive.LeaveSection(_temp); }
#define SERIALIZE_VAR(key,var)          archive.SetKey(key); archive & var;
#define SERIALIZE_VAR2(key,var,dft)     archive.SetKey(key); archive & var; if (archive.IsFail()) ArchiveMakeDefaulProxy(dft) >> var;

#define SERIALIZE_CHAIN_MAP(base)       base::DoSerialze(archive);




TM_END_NAMESPACE



#endif //TMARCHIVE_HATNUMERIC_H
