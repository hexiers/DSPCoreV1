/**
 ******************************************************************************
 * Copyright (C) 2020-2030, CAMIC All Rights Reserved
 * @file	MCSoft_Scope.c
 * @author	caih
 * @version 1.0
 * @date	2020/05/28
 * @brief	ʾ�������ܣ��۲����
 * Revision history:
 * <Author> 			<Date>			<Version>		<Description>
 *	caih				2020/05/28			1.0			�״δ���
 *	zyu				    2020/06/04		    1.1			����2·���ٹ������ӹ���
 
 ******************************************************************************
 */
#include "MCSoft_Scope.h"

Uint16 m1CaptureData[SCOPE_BUF_NUM];

ST_SCOPE stScope = {SCOPE_CONFIG, 0, 0, NULL, m1CaptureData, 0, 0, NULL, NULL};

/* ��Ҫ�۲�Ľṹ������ */
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
 * @brief   ʾ��������(��������Index�ķ�Χ���м��)
 * @param   Uint16 Index        ��Ҫ�۲�Ľṹ������
            Uint16 SubIndex     ��Ҫ�۲�ı����ڽṹ���е�λ��
            Uint16 Interval     �������(�Ե���Scope_Run����ʱ��Ϊ1������)
 * @return  TRUE: ���óɹ� FALSE: ����ʧ��
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
 * @brief   ʾ��������(��������Index�ķ�Χ���м��)
 * @param   Uint16 Index        ��Ҫ�۲�Ľṹ������
            Uint16 SubIndex     ��Ҫ�۲�ı����ڽṹ���е�λ��
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
 * @brief   ʾ��������(��������Index�ķ�Χ���м��)
 * @param   Uint16 Index        ��Ҫ�۲�Ľṹ������
            Uint16 SubIndex     ��Ҫ�۲�ı����ڽṹ���е�λ��
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
 * @brief   ʾ�������У������жϷ��������б�֤׼ʱ����
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
 * @brief   ��ѯʾ���������Ƿ����
 * @param   None
 * @return  TRUE: ����, FALSE: δ����
 *******************************************************************************
 */
bool Scope_IsFinished(void)
{
    return (stScope.eScopeSts == SCOPE_FINISH);
}


/**
 *******************************************************************************
 * @fn      Scope_Read
 * @brief   ��ȡʾ������������
 * @param   Uint16 *pData ָ��������ݱ��滺������ָ��
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
