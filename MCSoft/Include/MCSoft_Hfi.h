/**
 ******************************************************************************
 * Copyright (C) 2020-2030, CAMIC All Rights Reserved
 * @file	MCSoft_Hfi.h
 * @author	zyu
 * @version 1.0
 * @date	2020/06/01
 * @brief	
 * Revision history:
 * <Author> 			<Date>			<Version>		<Description>
 *	caih				2020/06/01			1.0			�״δ���
 ******************************************************************************
 */
 
#include"MC_Include.h"
#ifndef __MCSOFT_HFI_H
#define __MCSOFT_HFI_H

#include"MCPara_SnsLessPara.h"
/* ��Ƶע��ṹ��*/
typedef struct
{   
    INT16S InjectCnt;       /* ע������ڼ��� */
    Uint16 InjectCntMax;    /* ע������ڼ������ */
    Uint16 InjectFreg;      /* ע��Ƶ��*/
    Uint16 InjectVol;       /* ע���ѹ����һ����8192,UdCmd_V*/
    Uint16 RadPerCurrCycleQ12; /* ע��Ƶ����ÿ���������ڵĻ���*/
    Uint16 BoundLowFreqPassHz;  /* ��ͨ�˲���ֹƵ��*/
    Uint16 BoundHighFreqPassHz;  /* ��ͨ�˲���ֹƵ��*/
    Uint16 LowFreqPassHz;   /* ��ͨ�˲���ֹƵ��*/
    INT16S BoundLowFreqPassCoefQ14; /* ��֮ͨ��ͨ�˲���ǰ����ֵϵ������16384Ϊ��ĸ*/
    INT16S BoundHighFreqPassCoefQ14; /* ��֮ͨ��ͨ�˲���ǰ����ֵϵ������16384Ϊ��ĸ*/
    INT16S LowFreqPassCoefQ14;  /* ��ͨ�˲���ǰ����ֵϵ������16384Ϊ��ĸ*/
    INT16S IqHpass;
    INT16S IqXsinwt;
    INT16S IqXsinwtLowPassX16;
    INT16S HfiPosCoefQ8;       /* Ԥ��*/
    INT16S HfiPos;              /* ��Ƶע��λ��*/
    INT32S HfiPosQ16;              /* ��Ƶע��λ��*/
    INT32S HfiSpdQ16;   
    INT32S KpQ16;
    INT32S KiQ16;
    INT32S IntergTermQ16;
    INT16S Pos180;/* Ԥ��*/
    Uint16 sinwt;
    INT16S IdXsinwt;
    INT16S Test1;
    INT16S Test2;
}ST_HFI;

/* �ں�����*/
typedef struct
{   
    INT32S eStartSpdQ8;              
    INT32S eStartEndSpdQ8;             
    INT32S eStartPos;
    INT16S ePosCoefQ8;
    INT32S eStartPosQ8;

}ST_STARTNEW;
extern ST_STARTNEW m1eStart;
extern ST_HFI m1Hfi;
extern void MC_HfiInit(ST_HFI* pT,stSnsLessPara* pSns);
extern INT16S MC_HfiCalc(ST_HFI* pT,INT16S IqX10mA);
extern INT16S MC_eStartCalc(ST_STARTNEW* pT);
extern void MC_eStartInit(ST_STARTNEW* pT,INT32S EndSpdQ8);

#endif

