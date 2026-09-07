/*
 *  @file
 *  @brief  text file load/save
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.12.31
 *
 *  Copyright (C) 2015 miragekiller
 */

#pragma once

#ifndef TMTXTFILE_H
#define TMTXTFILE_H

#include "tmfile.h"
#include "tmstring.h"


/**
 *  @defgroup txtfile
 */

/**
 *  @addtogroup txtfile
 *  txt file "c" function prototype
 *  @{
 */

enum ETXTFILESTYLE
{
    e_TxtFileStyleUnknown,
    e_TxtFileStyleAnsi,
    e_txtFileStyleUtf8,
    e_txtFileStyleUtf8NoBOM,
    e_txtFileStyleU16,
    e_txtFileStyleU16BE,
};


/**
 * load text file to char string.
 */
TM_INLINE bool TxtFileLoad(TMCStr filename, TM::CStringA& str)
{
    TM::CFile f;
    f.OpenExisting(filename, f.e_faRead);
    if (!f)
        return false;
    long x = f.GetSize();
    if (x)
        f.Read(str.GetBuffer(x), x);
    return true;
}

/**
 * save char string to text file.
 */
TM_INLINE bool TxtFileSave(TMCStr filename, const std::string_view& str, TM::CFile::EDisposition eisposition = TM::CFile::e_fdCreateAlways)
{
    TM::CFile f;
    f.NativeOpen(filename, f.e_faWrite, eisposition);
    if (!f)
        return false;
    f.Write(str.data(), str.length());
    return true;
}

/**
 * save char string to text file. if dest file same as str then skip.
 */
TM_INLINE bool TxtFileCheckSave(TMCStr filename, const std::string_view& str, TM::CFile::EDisposition eisposition = TM::CFile::e_fdCreateAlways)
{
    TM::CStringA strTmp;
    if (TxtFileLoad(filename, strTmp) && strTmp == str)
        return true;
    return TxtFileSave(filename, str, eisposition);
}

/**
 * load to char string from stream.
 */
TM_INLINE bool TxtFileLoadFromStream(TM::CStringA& str, IStream* stream, size_t size = 0)
{
    if (!size)
    {
        STATSTG statstg;
        if (stream->Stat(&statstg, 0) != S_OK)
            return false;
        size = statstg.cbSize.LowPart;
    }
    TMULong l = 0;
    stream->Read(str.GetBuffer(size), size, &l);
    return l == size;
}

/**
 * convert text file to special format.
 * @param[in] bCheckUtf8 read an check as utf8 if the text file have no BOM.
 */
TM_INLINE ETXTFILESTYLE TxtFileConvert(TMCStrA strIn, int nStrIn, TM::CStringW& strOut, TMBool bCheckUtf8)
{
    if (nStrIn > 1 && strIn[0] == '\xff' && strIn[1] == '\xfe') // e_txtFileStyleU16
    {
        int n = nStrIn / 2;
        EndianStrL2H(strOut.GetBuffer(n - 1), (TMCStrW)(strIn + 2), n - 1);
        return e_txtFileStyleU16;
    }
    
    if (nStrIn > 1 && strIn[0] == '\xFE' && strIn[1] == '\xff') // e_txtFileStyleU16BE
    {
        int n = nStrIn / 2;
        EndianStrB2H(strOut.GetBuffer(n - 1), (TMCStrW)(strIn + 2), n - 1);
        return e_txtFileStyleU16BE;
    }
    
    if (nStrIn > 2 && strIn[0] == '\xEF' && strIn[1] == '\xBB' && strIn[2] == '\xBF') // e_txtFileStyleUtf8
    {
        strOut = TM::UTF2W(strIn + 3, nStrIn - 3);
        return e_txtFileStyleUtf8;
    }
    
    // e_TxtFileStyleAnsi
    if (bCheckUtf8)
    {
        TM::UTFOrA2W str(strIn, nStrIn);
        strOut = std::move(str);
        return str.IsUtf() ? e_txtFileStyleUtf8NoBOM : e_TxtFileStyleAnsi;
    }

    strOut.Assign(strIn, nStrIn);
    return e_TxtFileStyleAnsi;
}

/**
 * convert text file to special format.
 * @param[in] bCheckUtf8 read as utf8 if the text file have no BOM.
 */
TM_INLINE ETXTFILESTYLE TxtFileConvert(TMCStrA strIn, int nStrIn, TM::CStringA& strOut, TMBool bCheckUtf8)
{
    if (nStrIn > 1 && strIn[0] == '\xff' && strIn[1] == '\xfe') // e_txtFileStyleU16
    {
        DbgAssert(0);
        return e_TxtFileStyleUnknown;
    }
    
    if (nStrIn > 1 && strIn[0] == '\xFE' && strIn[1] == '\xff') // e_txtFileStyleU16BE
    {
        DbgAssert(0);
        return e_TxtFileStyleUnknown;
    }
    
    if (nStrIn > 2 && strIn[0] == '\xEF' && strIn[1] == '\xBB' && strIn[2] == '\xBF') // e_txtFileStyleUtf8
    {
        strOut.assign(strIn + 3, nStrIn - 3);
        return e_txtFileStyleUtf8;
    }
    
    // e_TxtFileStyleAnsi
    if (bCheckUtf8)
    {
        strOut.assign(strIn, nStrIn);
        return e_txtFileStyleUtf8NoBOM;
    }

    strOut.Assign(strIn, nStrIn);
    return e_TxtFileStyleAnsi;
}

/**
 * load text file as special format.
 * @param[in] bCheckUtf8 read as utf8 if the text file have no BOM.
 */
TM_INLINE ETXTFILESTYLE TxtFileLoadAs(TMCStr filename, TM::CStringW& strOut, TMBool bCheckUtf8)
{
    TM::CStringA str;
    if (!TxtFileLoad(filename, str))
        return e_TxtFileStyleUnknown;

    return TxtFileConvert(str.data(), str.GetLength(), strOut, bCheckUtf8);
}

/**
 * load text file as special format.
 * @param[in] bCheckUtf8 read as utf8 if the text file have no BOM.
 */
TM_INLINE ETXTFILESTYLE TxtFileLoadAs(TMCStr filename, TM::CStringA& strOut, TMBool bCheckUtf8)
{
    TM::CStringA str;
    if (!TxtFileLoad(filename, str))
        return e_TxtFileStyleUnknown;

    return TxtFileConvert(str.data(), str.GetLength(), strOut, bCheckUtf8);
}

/**
 * save text file as special format.
 */
TM_INLINE bool TxtFileSaveAs(TM::CStringW str, ETXTFILESTYLE style, TM::CFile& f)
{
    DbgAssert(f);

    if (style == e_txtFileStyleU16)
    {
        int n = str.GetLength();
        EndianStrL2H(str.GetBuffer(), n);
        f.Write("\xFF\xFE", 2);
        f.Write(str.data(), n * 2);
        return true;
    }
    else if (style == e_txtFileStyleU16BE)
    {
        int n = str.GetLength();
        EndianStrB2H(str.GetBuffer(), n);
        f.Write("\xFE\xFF", 2);
        f.Write(str.data(), n * 2);
        return true;
    }
    else if (style == e_txtFileStyleUtf8)
    {
        TM::W2UTF u8(str);
        f.Write("\xEF\xBB\xBF", 3);
        f.Write(u8.data(), u8.GetLength());
        return true;
    }
    else if (style == e_txtFileStyleUtf8NoBOM)
    {
        TM::W2UTF u8(str);
        f.Write(u8.data(), u8.GetLength());
        return true;
    }

    TM::CStringA a8(str);
    f.Write(a8.data(), a8.GetLength());
    return true;
}

/**
 * save text file as special format.
 */
TM_INLINE bool TxtFileSaveAs(const TM::CStringW& str, ETXTFILESTYLE style, TMCStr name, TM::CFile::EDisposition eisposition = TM::CFile::e_fdCreateAlways)
{
    TM::CFile f;
    f.NativeOpen(name, TM::CFile::e_faWrite, eisposition);
    if (!f)
        return false;
    return TxtFileSaveAs(str, style, f);
}

/** @} */ // @addtogroup txtfile

#endif // end of TMTXTFILE_H


