/**
 ******************************************************************************
 * Copyright (C) 2020-2030, CAMIC All Rights Reserved
 * @file	MCSoft_Scope.h
 * @author	caih
 * @version 1.0
 * @date	2020/05/28
 * @brief	示波器功能，观测变量
 * Revision history:
 * <Author> 			<Date>			<Version>		<Description>
 *	caih				2020/05/28			1.0			首次创建
 ******************************************************************************
 */
 
#include"MC_Include.h"
#ifndef __MCSOFT_SCOPE_H
#define __MCSOFT_SCOPE_H


#define SCOPE_BUF_NUM   (192)

typedef enum
{
    SCOPE_CONFIG = 0,
    SCOPE_SAMPLE,
    SCOPE_FINISH
}E_SCOPE_STS;

typedef struct
{
    E_SCOPE_STS eScopeSts;
    Uint16 Interval;
    Uint16 IntervalCnt;
    Uint16 *pScopeSrc;
    Uint16 *pScopeBuf;
    Uint16 SampleNum;
    Uint16 ReadNum;
    Uint16 *Monitor1Addr;/* zyu V1.6 新增监视*/
    Uint16 *Monitor2Addr;/* zyu V1.6 新增监视*/
}ST_SCOPE;

typedef struct
{
    void *Obj;
}ST_SCOPE_OBJ;

extern bool Scope_Config(Uint16 Index, Uint16 SubIndex, Uint16 Interval);
extern void Monitor1_Config(Uint16 Index, Uint16 SubIndex);/* zyu V1.6 新增监视*/
extern void Monitor2_Config(Uint16 Index, Uint16 SubIndex);/* zyu V1.6 新增监视*/

extern void Scope_Run(void);
extern bool Scope_IsFinished(void);
extern void Scope_Read(Uint16 *pData, Uint16 Num);
extern ST_SCOPE stScope;
#endif

