/*
 *  @file
 *  @brief  LZW encode, decode
 *
 *  @author miragekiller <3916345933@qq.com>
 *
 *  Copyright (C) 2012 miragekiller
 */

#include "tmlzw.h"
#include "tmcom.h"

// const TMUInt16 s_uLZWEmpty = 0xffff;

class CLZWDecode
{
    ISequentialStream* m_pIn;
    ISequentialStream* m_pOut;
    TMUInt16 m_nCurSize;
    TMUInt16 m_nBitsLeft;
    TMUInt8 m_bCurBit;
//     TMUInt16 m_nTmpBit;
    static const TMUInt16 cs_wMask[13];

public:
    CLZWDecode(ISequentialStream* pIn, ISequentialStream* pOut)
        : m_pIn (pIn)
        , m_pOut (pOut)
//         , m_nTmpBit (s_uLZWEmpty)
    {
    }

    TMUInt8 ReadByte()
    {
        TMUInt8 b = 0;
        m_pIn->Read(&b, 1, 0);
        return b;
    }

    void WriteLine(TMUInt8 *buf, int linelen)
    {
        m_pOut->Write(buf, linelen, 0);
    }

    TMUInt16 NextCode()
    {
        if (m_nBitsLeft == 0)
        {
            m_bCurBit = ReadByte();
            m_nBitsLeft = 8;
        }
        
        TMUInt16 wRet = m_bCurBit >> (8 - m_nBitsLeft);
        while (m_nCurSize > m_nBitsLeft)
        {
            m_bCurBit = ReadByte();
            wRet |= m_bCurBit << m_nBitsLeft;
            m_nBitsLeft += 8;
        }
        m_nBitsLeft -= m_nCurSize;
        DbgAssert(m_nCurSize < tm_countof(cs_wMask));
        return wRet & cs_wMask[m_nCurSize];
    }

    TMBool decoder()
    {
        enum { TABLE_SIZE = 16384 };//4096 };

        TMUInt8 InitCodeSize = ReadByte();
        if (InitCodeSize < 2 || InitCodeSize > 9)
            return false;

        TMUInt16 Prefix     [TABLE_SIZE];
        TMUInt8  OutStack   [TABLE_SIZE];
        TMUInt8  Suffix     [TABLE_SIZE];

        // init exp
        m_nBitsLeft = 0;
        m_nCurSize = InitCodeSize + 1;
        TMUInt16 ClearCode = 1 << InitCodeSize;
        TMUInt16 EndCode = ClearCode + 1;
        TMUInt16 FirstEntry = ClearCode + 2;        // Index of first free entry in table
        TMUInt16 NextEntry = ClearCode + 2;            // Index of next free entry in table

        TMUInt16 PrevCode;                            // Previous Code
        for (TMUInt16 Code; (Code = NextCode()) != EndCode; PrevCode = Code)
        {
            if (Code == ClearCode)
            {
                m_nCurSize = InitCodeSize + 1;        // Reset m_nCurSize
                NextEntry = FirstEntry;                // Reset Translation Table
                continue;                            // restart, to get another code
            }

            int OutIndex = 0;                        // Reset Output Stack
            TMUInt16 OutCode;                        // Code to output
            if (Code < NextEntry)                    // CODE IS IN TABLE
                OutCode = Code;                        // Set code to output.
            else
            {
                ++OutIndex;                            // Keep "first" character of previous output.
                OutCode = PrevCode;                    // Set PrevCode to be output
            }
            
            while (OutCode >= FirstEntry)
            {
                DbgAssert(OutIndex <= TABLE_SIZE);
                ++OutIndex;
                OutStack[TABLE_SIZE - OutIndex] = Suffix[OutCode];    // Add suffix to Output Stack
                OutCode = Prefix[OutCode];            // Loop with preffix
            }
            
            ++OutIndex;
            OutStack[TABLE_SIZE - OutIndex] = (TMUInt8)OutCode;
            DbgAssert(OutIndex <= TABLE_SIZE);
            
            // ADD NEW ENTRY TO TABLE (PrevCode + OutCode)
            // (EXCEPT IF PREVIOUS CODE WAS A CLEARCODE)
            if (PrevCode != ClearCode)
            {
                Prefix[NextEntry] = PrevCode;
                Suffix[NextEntry] = (TMUInt8)OutCode;

                ++NextEntry;
                DbgAssert(NextEntry <= TABLE_SIZE);
                
                // INCREASE m_nCurSize IF NextEntry IS INVALID WITH CURRENT m_nCurSize
                if (NextEntry >= (1 << m_nCurSize))
                {
                    if (m_nCurSize < 12)
                        ++m_nCurSize;
                    else
                    {}                // Do nothing. Maybe next is Clear Code.
                }
            }
            
            WriteLine(&(OutStack[TABLE_SIZE - OutIndex]), OutIndex);
        }

      return true;
   }

};

const TMUInt16 CLZWDecode::cs_wMask[13] = {
   0,
   0x0001, 0x0003,
   0x0007, 0x000F,
   0x001F, 0x003F,
   0x007F, 0x00FF,
   0x01FF, 0x03FF,
   0x07FF, 0x0FFF
};

TMBool LzwDecode(ISequentialStream* pIn, ISequentialStream* pOut)
{
    CLZWDecode lz(pIn, pOut);
    return lz.decoder();
}
