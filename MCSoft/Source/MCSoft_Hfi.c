/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-06-01                                                               
* 修改记录		:   
*                         
======================================================================================================*/
#include"MCSoft_Hfi.h"
ST_HFI m1Hfi;
ST_STARTNEW m1eStart;
/**************************************************************************************************************************************************
* @函数名		:  		MC_HfiInit                                                       
* @函数描述	: 		高频注入结构体的初始化
* @创建人		:    	张宇
* 创建日期	:   	2020-06-01
* 修改记录	:
*                         
****************************************************************************************************************************************************/
void MC_HfiInit(ST_HFI* pT,stSnsLessPara* pSns)
{
	double SampleFreq_Hz = 0;
	_iq RadPerCurrCycle = 0;
	/* 计算PWM频率*/
	SampleFreq_Hz = PWMCLKFREQ/(2*m1CtrlPara.PwmPeriod);

    pT->InjectCnt = 0;
    pT->InjectFreg  = pSns->HfiFreqHz;
    pT->InjectVol   = pSns->HfiVol;
    pT->BoundHighFreqPassHz  = pSns->BoundHighFreqPassHz;    /* 暂定*/
    pT->BoundLowFreqPassHz  = pSns->BoundLowFreqPassHz;    /* 暂定*/
    pT->LowFreqPassHz   = pSns->LowFreqPassHz;    /* 暂定*/
    RadPerCurrCycle = _IQdiv(_IQmpy(_IQ(PI*2),_IQ(pT->InjectFreg)),_IQ(SampleFreq_Hz));
    pT->RadPerCurrCycleQ12 =  _IQint((INT32S)RadPerCurrCycle * (1L<<12));
    pT->InjectCntMax = SampleFreq_Hz/pT->InjectFreg;
    pT->BoundHighFreqPassCoefQ14 = (INT32S)(16384*PI*2*(double)pT->BoundHighFreqPassHz/SampleFreq_Hz);
    pT->BoundLowFreqPassCoefQ14 = (INT32S)(16384*PI*2*(double)pT->BoundLowFreqPassHz/SampleFreq_Hz);
    pT->LowFreqPassCoefQ14 = (INT32S)(16384*PI*2*(double)pT->LowFreqPassHz/SampleFreq_Hz);
    pT->IqHpass = 0;
    pT->IqXsinwt = 0;
    pT->IqXsinwtLowPassX16 = 0;
    pT->HfiPos = 0;              /* 高频注入位置*/
    //pT->HfiPosCoefQ8 = 256;       /* 用以与smc位置平滑过渡*/
    pT->HfiSpdQ16 = 0;
    pT->KpQ16 = pSns->PllKpQ12 * 16L;// 25180
    pT->KiQ16 = pSns->PllKiQ16;
    pT->IntergTermQ16 = 0;
    //pT->Pos180 = 0;
}
/**************************************************************************************************************************************************
* @函数名		:  		MC_HfiCalc                                                       
* @函数描述	: 		高频注入运算，按照电流中断频率调用，返回应注入值UdCmd_V的控制量
* @创建人		:    	张宇
* 创建日期	:   	2020-06-03
* 修改记录	:
*                         
****************************************************************************************************************************************************/

INT16S MC_HfiCalc(ST_HFI* pT,INT16S IqX10mA)
{
    _iq iqSinwt = 0;
    _iq iqCoswt = 0;
    INT16S UdInject_V = 0;
    //INT32S Tmp = 0;
    INT32S TmpQ = 0;
    INT32S TmpD = 0;
    static INT32S staTmp0 = 0;
    static INT32S staTmp1 = 0;
    static INT32S staTmp2 = 0;
    static INT32S staTmp3 = 0;
    if(++pT->InjectCnt >= pT->InjectCntMax)
    {
        pT->InjectCnt = 0;
    }
    /* 注入信号相位*/
    iqCoswt = _IQcos(pT->InjectCnt * (long)pT->RadPerCurrCycleQ12 << 4);
    /* 解耦调制信号相位*/
    iqSinwt = _IQsin((pT->InjectCnt * (long)pT->RadPerCurrCycleQ12 << 4)  + _IQ((PI*0)/180));// 解调信号相位校正0度
    pT->sinwt = iqSinwt >> 5;
    /* 计算注入信号*/
    UdInject_V = (INT32S)pT->InjectVol*iqCoswt>> GLOBAL_Q;
    /* 注入信号保护*/
    UdInject_V = LIMITMAXMIN(UdInject_V, 2000, -2000);

    /* q电流带通的低通滤波部分*/
    staTmp0 = (((INT64S)IqX10mA << GLOBAL_Q) * pT->BoundLowFreqPassCoefQ14 + (INT64S)staTmp1 * (16384 - pT->BoundLowFreqPassCoefQ14))>>14;
    /* q电流带通的高通滤波,先低通滤波,再用输入减去*/
    staTmp1 = ((INT64S)staTmp0 * pT->BoundHighFreqPassCoefQ14 + (INT64S)staTmp1 * (16384 - pT->BoundHighFreqPassCoefQ14))>>14;
    pT->IqHpass = (staTmp0 - staTmp1) >> GLOBAL_Q;

    /* 调制*/
    TmpQ = (INT32S)pT->IqHpass * (iqSinwt);

    /* 调制后信号低通滤波*/
    staTmp2= ((INT64S)TmpQ * pT->LowFreqPassCoefQ14 + (INT64S)staTmp2 * (16384 - pT->LowFreqPassCoefQ14))>>14;
    //staTmp3= ((INT64S)staTmp2 * pT->LowFreqPassCoefQ14 + (INT64S)staTmp3 * (16384 - pT->LowFreqPassCoefQ14))>>14;
    staTmp3 = staTmp2;
    
    pT->IqXsinwt = TmpQ >> GLOBAL_Q;
    pT->IqXsinwtLowPassX16 = staTmp3 >> (GLOBAL_Q-4);
    
    //pT->IqXsinwtLowPassX16 = (-staTmp3) >> (GLOBAL_Q-4);
   
	//积分项计算
	pT->IntergTermQ16 += pT->KiQ16 * (INT32S)pT->IqXsinwtLowPassX16;
	pT->IntergTermQ16 = LIMITMAXMIN(pT->IntergTermQ16,(400L<<16),(1L<<16));/* 1L<<16 用以防止启动不了*/
    
    // PI调节出误差速度
    pT->HfiSpdQ16 = LIMITMAXMIN((pT->KpQ16 * (INT32S)pT->IqXsinwtLowPassX16  + pT->IntergTermQ16),(400L<<16),(1L<<16));/* 1L<<16 用以防止启动不了*/

    //速度累加出位置
    pT->HfiPosQ16+= pT->HfiSpdQ16; 
    pT->HfiPos = (INT16S)(pT->HfiPosQ16 >> 16);

    /* 注入输出*/
    return UdInject_V;
    
    /* 测试*/
    //pT->Test1 = (INT16S)(pT->HfiPosQ16 >> 16) - m1SmcObserverExt.SmcElecPos;
    //pT->Test2 = iqSinwt >> 1;
    /* 测试*/
    //TmpD =  (INT32S)(m1CurrLoopCtler.IdFdb * (Uint32)m1HalPara.BitTo10mACoef) >>12;
    //pT->IdXsinwt = (INT32S)TmpD * iqSinwt>>GLOBAL_Q;


   
}

INT16S MC_eStartCalc(ST_STARTNEW* pT)
{
    INT16S Tmp = 0;
    INT16S Tmp2 = 0;
    //速度累加
    if(pT->eStartSpdQ8 < pT->eStartEndSpdQ8)
    {
        pT->eStartSpdQ8 += 4;
    }
    //位移累加
    pT->eStartPosQ8 = ((INT32S)pT->eStartPos << 8) + pT->eStartSpdQ8;
    pT->eStartPosQ8 = pT->eStartPosQ8 & 0x00FFFFFF;
    Tmp = pT->eStartPosQ8 >> 8;

    // eStartSpdQ8 = 128Q8时，完全由观测器位置确定
    // eStartSpdQ8 = 64Q8时，完全由开环位置确定
    pT->ePosCoefQ8 = LIMITMAXMIN(((128L<<8) - pT->eStartSpdQ8)>>6,256,0);
    //位置融合
    Tmp2 = m1SmcObserverExt.SmcElecPos - Tmp;/* 用16位字长求出有符号误差*/
    Tmp2 = ((INT32S)Tmp2 * (256L - pT->ePosCoefQ8))>>8;/* 求出从开环位置到最终位置的修正量,存为16位字长*/
    // 生成最终位置
    pT->eStartPos = Tmp + Tmp2;
    return 1;
    
}
void MC_eStartInit(ST_STARTNEW* pT,INT32S EndSpdQ8)
{
    pT->eStartSpdQ8 = 0;              
    pT->eStartEndSpdQ8  = EndSpdQ8;
    pT->eStartPos = 0;
    pT->ePosCoefQ8 = 256;
    pT->eStartPosQ8 = 0;
    
}
 
