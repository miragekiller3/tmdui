/*
 *  @file
 *  @brief
 *  text file load/save
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2017.06.15
 *
 *  Copyright (C) 2017 miragekiller
 */

#pragma once

#ifndef TMTXTSEARCH_H
#define TMTXTSEARCH_H

#include "tmstring.h"
#include "tmlist.h"


TM_BGN_NAMESPACE


class ITxtSeachSink
{
public:
    virtual int OnChar(const CStringA& text, int nPos, const void* param) = 0; // return skip char number; 0: don't skip; -1: stop
    virtual void OnFind(const CStringA& text, int nPos, const CStringA& key, const void* param) = 0;

};


class CTxtSearch
{
private:
    typedef CSet<CStringA> CKeyList;
    typedef TMUInt32 UMASK;

    int m_maxStoreWordLength;
    int m_minStoreWordLength;

    //存放每个字符在对应位数上是否存在敏感词，以及哪几位是敏感词
    UMASK m_fastPositionCheck[UCHAR_MAX + 1];
    UMASK m_fastLengthCheck[UCHAR_MAX + 1];
    CKeyList* m_words[UCHAR_MAX + 1];

public:
    CTxtSearch()
        : m_maxStoreWordLength(0)
        , m_minStoreWordLength(INT_MAX)
    {
        MemZeroT(m_fastPositionCheck);
        MemZeroT(m_fastLengthCheck);
        MemZeroT(m_words);
    }

    ~CTxtSearch()
    {
        for (int i = 0; i < tm_countof(m_words); ++i)
            delete m_words[i];
    }

    inline UMASK ll(int n) const
    {
        return (UMASK(1)) << (n % sizeof(UMASK));
    }

    void AddKeyword(const CStringA& word)
    {
        int nLength = word.GetLength();
        DbgAssert(nLength);

        tm_setmax(m_maxStoreWordLength, nLength);
        tm_setmin(m_minStoreWordLength, nLength);

        for (int i = 0; i < nLength; i++)
            m_fastPositionCheck[word[i]] |= ll(i);

        //记录以某个字开头的关键字的长度信息，左移位数长度为该字符串长度减一
        m_fastLengthCheck[word[0]] |= ll(nLength - 1);

        if (!m_words[word[0]])
            m_words[word[0]] = new CKeyList();

        m_words[word[0]]->insert(word);
    }

    void Search(const CStringA& text, ITxtSeachSink* pSink, const void* param)
    {
        int nLength = text.GetLength();
        for (int index = 0; index < nLength; ++index)
        {
            int ret = pSink->OnChar(text, index, param);
            if (ret == -1)
                return;
            if (ret)
            {
                index += (ret - 1);
                continue;
            }

            char begin = text[index];
            //首先判断当前字符是否是某关键字的第一个字符，不是时就继续向下遍历
            if ((m_fastPositionCheck[begin] & 1) == 0)
                continue;

            //此时已经判定，当前的这个字符会出现在关键词的第一位上
            //在判断
//            int jump = 1;
            int nMin = tm_min(m_maxStoreWordLength, nLength - index - 1);
            for (int j = 0; j <= nMin; j++)
            {
                char current = text[index + j];
                
                //判断当前字符是否会出现在关键字的对应位上，实现快速判断
                if ((m_fastPositionCheck[current] & ll(j)) == 0)
                    break;

               
                //当判决的长度大于关键字的最小长度时，当前的截取字符串有可能会是关键字，要做详细判定
                if ((j + 1 < m_minStoreWordLength) || ((m_fastLengthCheck[begin] & ll(j)) == 0))
                    continue;

                CKeyList* dict = m_words[begin];
                DbgAssert(dict);
                CStringA sub = text.substr(index, j + 1);

                //在字典中搜索判断，得出结论。同时给出跳转位数，供下一次跳转用
                if (dict->find(sub) != dict->end())
                {
                    pSink->OnFind(text, index, sub, param);
                    //index += j;
                }
            }
        }
    }
};


class CCppFileSeachSinkImpl : public ITxtSeachSink
{
public:
    int m_nLine;
    int m_nLineIndex;
    bool m_bCommet;         // "/* xxx */"
    bool m_bCommetLine;     // "// xxx"

public:
    CCppFileSeachSinkImpl()
    {
        Init();
    }

    void Init()
    {
        m_bCommet = false;
        m_bCommetLine = false;
        m_nLine = 1;
        m_nLineIndex = 0;
    }

    virtual int OnChar(const CStringA& text, int nPos, const void* param)
    {
        char c = text[nPos];

        switch (c)
        {
        case '\r':
            {
                m_bCommetLine = false;
                ++m_nLine;
                int nSkip = text[nPos + 1] == '\n' ? 2 : 1;
                m_nLineIndex = nPos + nSkip;
                return nSkip;
            }

        case '\n':
            {
                m_bCommetLine = false;
                ++m_nLine;
                int nSkip = text[nPos + 1] == '\r' ? 2 : 1;
                m_nLineIndex = nPos + nSkip;
                return nSkip;
            }
        }

        if (m_bCommet)
        {
            if (c == '*' && text[nPos + 1] == '/')
            {
                m_bCommet = false;
                return 2;
            }
            return 1;
        }

        if (m_bCommetLine)
            return 1;

        if (c == '/')
        {
            char cn = text[nPos + 1];
            if (cn == '*')
                return m_bCommet = false, 2;
            else if (cn == '/')
                return m_bCommetLine = true, 2;
        }

        return 0;
    }
};



TM_END_NAMESPACE


#endif // TMTXTSEARCH_H
