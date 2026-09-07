/*
 *  @file
 *  @brief
 *  undo manage
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.12.17
 *
 *  Copyright (C) 2010 miragekiller
 *  All rights reserved.
 */

#ifndef GOTO_H
#define GOTO_H

class CMainWnd;

class CGoto
{
private:
    std::vector<TMUInt64>    m_aOpt;
    int                 m_nOptPos;
    TMBool              m_nOpt; // 0: normal; 1:forword; 2:back

public:
    CGoto();
    CMainWnd& GetMainWnd();
    void Clear();
    void Forword();
    void Back();
    void Add(TMUInt64 nID);
};


#endif // GOTO_H