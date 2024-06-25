/**
 ******************************************************************************
 * Copyright (C) 2020-2030, CAMIC All Rights Reserved
 * @file	MCSoft_Scope.c
 * @author	caih
 * @version 1.0
 * @date	2020/05/28
 * @brief	示波器功能，观测变量
 * Revision history:
 * <Author> 			<Date>			<Version>		<Description>
 *	caih				2020/05/28			1.0			首次创建
 *	zyu				    2020/06/04		    1.1			增加2路低速滚动监视功能
 
 ******************************************************************************
 */
#include "MCSoft_Scope.h"

Uint16 m1CaptureData[SCOPE_BUF_NUM];

ST_SCOPE stScope = {SCOPE_CONFIG, 0, 0, NULL, m1CaptureData, 0, 0, NULL, NULL};

/* 需要观测的结构体数组 */
const ST_SCOPE_OBJ stScopeObjArr[] = 
{
    {(void *)&m1CurrLoopCtler},
    {(void *)&m1SpdLoopCtler},
    {(void *)&m1BusVolDecoupler},
    {(void *)&m1SmcObserverExt},
    {(void *)&m1ResolverDealModule},
    {(void *)&m1FaultDealModule},
    {(void *)&m1CriPara},
    {(void *)&m1Hfi}    
};


/**
 *******************************************************************************
 * @fn      Scope_Config
 * @brief   示波器配置(不对两个Index的范围进行检查)
 * @param   Uint16 Index        需要观测的结构体的序号
            Uint16 SubIndex     需要观测的变量在结构体中的位置
            Uint16 Interval     采样间隔(以调用Scope_Run函数时间为1个周期)
 * @return  TRUE: 配置成功 FALSE: 配置失败
 *******************************************************************************
 */
bool Scope_Config(Uint16 Index, Uint16 SubIndex, Uint16 Interval)
{
    bool bStatus = FALSE;
    if(stScope.eScopeSts == SCOPE_CONFIG)
    {
        stScope.pScopeSrc = ((Uint16 *)(stScopeObjArr[Index].Obj)) + SubIndex;
        stScope.Interval = Interval;
        stScope.eScopeSts = SCOPE_SAMPLE;
        bStatus = TRUE;
    }
    return bStatus;
}
/**
 *******************************************************************************
 * @fn      Monitor1_Config V1.6
 * @brief   示波器配置(不对两个Index的范围进行检查)
 * @param   Uint16 Index        需要观测的结构体的序号
            Uint16 SubIndex     需要观测的变量在结构体中的位置
 * @return  
 *******************************************************************************
 */
void Monitor1_Config(Uint16 Index, Uint16 SubIndex)
{
    stScope.Monitor1Addr = ((Uint16 *)(stScopeObjArr[Index].Obj)) + SubIndex;
}
/**
 *******************************************************************************
 * @fn      Monitor2_Config V1.6
 * @brief   示波器配置(不对两个Index的范围进行检查)
 * @param   Uint16 Index        需要观测的结构体的序号
            Uint16 SubIndex     需要观测的变量在结构体中的位置
 * @return  
 *******************************************************************************
 */
void Monitor2_Config(Uint16 Index, Uint16 SubIndex)
{
    stScope.Monitor2Addr = ((Uint16 *)(stScopeObjArr[Index].Obj)) + SubIndex;
}
/**
 *******************************************************************************
 * @fn      Scope_Config
 * @brief   示波器运行，放在中断服务函数运行保证准时调用
 * @param   None
 * @return  None
 *******************************************************************************
 */
void Scope_Run(void)
{
    if(stScope.eScopeSts == SCOPE_SAMPLE)
    {
        stScope.IntervalCnt++;
        if(stScope.IntervalCnt == stScope.Interval)
        {
            stScope.IntervalCnt = 0;
            stScope.pScopeBuf[(stScope.SampleNum)++] = *(stScope.pScopeSrc);
            if(stScope.SampleNum >= SCOPE_BUF_NUM)
            {
                stScope.SampleNum = 0;
                stScope.eScopeSts = SCOPE_FINISH;
            }
        }
    }
}

/**
 *******************************************************************************
 * @fn      Scope_IsFinished
 * @brief   查询示波器采样是否结束
 * @param   None
 * @return  TRUE: 结束, FALSE: 未结束
 *******************************************************************************
 */
bool Scope_IsFinished(void)
{
    return (stScope.eScopeSts == SCOPE_FINISH);
}


/**
 *******************************************************************************
 * @fn      Scope_Read
 * @brief   读取示波器采样数据
 * @param   Uint16 *pData 指向采样数据保存缓冲区的指针
 * @return  None
 *******************************************************************************
 */
void Scope_Read(Uint16 *pData, Uint16 Num)
{
    MemCpy(m1CaptureData + stScope.ReadNum, pData, Num);
    stScope.ReadNum += Num;
    if(stScope.ReadNum >= SCOPE_BUF_NUM)
    {
        stScope.eScopeSts = SCOPE_CONFIG;
        stScope.ReadNum = 0;
    }
}
