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
 *	caih				2020/06/01			1.0			首次创建
 ******************************************************************************
 */
 
#include"MC_Include.h"
#ifndef __MCSOFT_HFI_H
#define __MCSOFT_HFI_H

#include"MCPara_SnsLessPara.h"
/* 高频注入结构体*/
typedef struct
{   
    INT16S InjectCnt;       /* 注入的周期计数 */
    Uint16 InjectCntMax;    /* 注入的周期计数最大 */
    Uint16 InjectFreg;      /* 注入频率*/
    Uint16 InjectVol;       /* 注入电压，归一化到8192,UdCmd_V*/
    Uint16 RadPerCurrCycleQ12; /* 注入频率在每个电流周期的弧度*/
    Uint16 BoundLowFreqPassHz;  /* 高通滤波截止频率*/
    Uint16 BoundHighFreqPassHz;  /* 高通滤波截止频率*/
    Uint16 LowFreqPassHz;   /* 低通滤波截止频率*/
    INT16S BoundLowFreqPassCoefQ14; /* 带通之低通滤波当前输入值系数，以16384为分母*/
    INT16S BoundHighFreqPassCoefQ14; /* 带通之高通滤波当前输入值系数，以16384为分母*/
    INT16S LowFreqPassCoefQ14;  /* 低通滤波当前输入值系数，以16384为分母*/
    INT16S IqHpass;
    INT16S IqXsinwt;
    INT16S IqXsinwtLowPassX16;
    INT16S HfiPosCoefQ8;       /* 预留*/
    INT16S HfiPos;              /* 高频注入位置*/
    INT32S HfiPosQ16;              /* 高频注入位置*/
    INT32S HfiSpdQ16;   
    INT32S KpQ16;
    INT32S KiQ16;
    INT32S IntergTermQ16;
    INT16S Pos180;/* 预留*/
    Uint16 sinwt;
    INT16S IdXsinwt;
    INT16S Test1;
    INT16S Test2;
}ST_HFI;

/* 融合启动*/
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

