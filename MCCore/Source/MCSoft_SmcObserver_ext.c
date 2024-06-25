#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "MCSoft_SmcObserver_ext.h"
extern void MemClr(Uint16 *StartAddr, Uint16 *EndAddr);
#pragma CODE_SECTION(SMC_Position_Estimation,"ramfuncs");
#pragma CODE_SECTION(CalcEstI,"ramfuncs");
#pragma CODE_SECTION(CalcZ,"ramfuncs");
#pragma CODE_SECTION(CalcBEMF,"ramfuncs");
#pragma CODE_SECTION(CalcTheta,"ramfuncs");
#pragma CODE_SECTION(CalcElecRpm,"ramfuncs");
#pragma CODE_SECTION(CalcKslf,"ramfuncs");
#pragma CODE_SECTION(ThetaCompesente,"ramfuncs");
#pragma CODE_SECTION(SMC_AdjustedFilter,"ramfuncs");



/**************************************************************************************************************************************************
* @函数名		:  	SMC_AdjustedFilter                                                        
* @函数描述	: 		滑模观测器自适应滤波器
* @创建人		: 刘启进
* 创建日期		:   2020-04-26                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
static inline INT32S SMC_AdjustedFilter(INT32S DataInNew,INT32S DataInOld,INT32S DataOutOld,INT16S Kslf)
{
	INT32S Tmp = 0;
	INT32S RtnVaule = 0;
	Tmp = ((INT64S)(DataInNew + DataInOld - (DataOutOld <<1)))* Kslf >>15;
	RtnVaule = Tmp + DataOutOld;
	return RtnVaule;
}


/**************************************************************************************************************************************************
* @函数名		:  	CalcEstI                                                        
* @函数描述	: 		计算估算电流
* @创建人		: 刘启进
* 创建日期		:   2020-04-26                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
static inline void CalcEstI(stSmcObserverExt* pT)
{

#if 0 // dq轴电感相同时候的递推关系式
	estIalpha = _IQmpy(gsmopos, vAlpha) - _IQmpy(gsmopos, eAlpha) - _IQmpy(gsmopos,zAlpha) + _IQmpy(fsmopos,estIalpha);
	estIbeta = _IQmpy(gsmopos, vBeta) - _IQmpy(gsmopos, eBeta) - _IQmpy(gsmopos,zBeta) + _IQmpy(fsmopos,estIbeta);
#endif
#if 0 // dq轴电感不同时候的递推关系式，增加alpha、beta耦合电流项  //!!
	estIalpha = -_IQmpy(gsmopos, _IQmpy(_IQmpy(omegaFltred,qOmegaToWeXLdminusLq),TmpIbeta)) \
				+_IQmpy(gsmopos, vAlpha) - _IQmpy(gsmopos, eAlpha) - _IQmpy(gsmopos,zAlpha) + _IQmpy(fsmopos,estIalpha);
	estIbeta = _IQmpy(gsmopos, _IQmpy(_IQmpy(omegaFltred,qOmegaToWeXLdminusLq),TmpIalpha)) \
				+_IQmpy(gsmopos, vBeta) - _IQmpy(gsmopos, eBeta) - _IQmpy(gsmopos,zBeta) + _IQmpy(fsmopos,estIbeta);
#endif

	//计算Alpha轴估算电流	
	pT->Alpha_G_ResultQ16 = ((INT64S)(pT->UAlpha_UseQ16 - pT->eAlphaQ16 - pT->zAlphaQ16))*pT->gsmopos >>12;
	pT->Alpha_F_ResultQ16 = ((INT64S)pT->EstIalphaQ16)* pT->fsmopos >>15;
	pT->EstIalphaQ16 = pT->Alpha_F_ResultQ16 + pT->Alpha_G_ResultQ16 ;
	pT->EstIalphaQ16 = LIMITMAXMIN(pT->EstIalphaQ16, 16383L<<16, -16384L<<16);

	//计算Beta轴估算电流	
	pT->Beta_G_ResultQ16 = (INT64S)(pT->UBeta_UseQ16 - pT->eBetaQ16 - pT->zBetaQ16)*pT->gsmopos >>12;
	pT->Beta_F_ResultQ16 = (INT64S)pT->EstIbetaQ16* pT->fsmopos >>15;
	pT->EstIbetaQ16 = pT->Beta_F_ResultQ16 + pT->Beta_G_ResultQ16 ;
	pT->EstIbetaQ16 = LIMITMAXMIN(pT->EstIbetaQ16, 16383L<<16, -16384L<<16);

}

/**************************************************************************************************************************************************
* @函数名		:  	CalcZ                                                        
* @函数描述	: 		计算滑模控制的输出量Z
* @创建人		: 刘启进
* 创建日期		:   2020-04-26                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
static inline void CalcZ(stSmcObserverExt* pT)
{
	//存上一周期的Zalpha
	pT->zAlphaOldQ16 =  pT->zAlphaQ16;
	//计算本周期的Zalpha
	pT->IAlphaErrQ16 = pT->EstIalphaQ16 - pT->IAlpha_UseQ16;
	pT->zAlphaQ16 = ((INT64S)pT->IAlphaErrQ16) * pT->SlideGain;
	pT->zAlphaQ16 =LIMITMAXMIN(pT->zAlphaQ16, pT->SlideMax, -pT->SlideMax);

	//存上一周期的Zbeta
	pT->zBetaOldQ16 =  pT->zBetaQ16;
	//计算本周期的Zbeta
	pT->IBetaErrQ16 = pT->EstIbetaQ16 - pT->IBeta_UseQ16;
	pT->zBetaQ16 = ((INT64S)pT->IBetaErrQ16) * pT->SlideGain;
	pT->zBetaQ16 =LIMITMAXMIN(pT->zBetaQ16, pT->SlideMax, -pT->SlideMax);
}


/**************************************************************************************************************************************************
* @函数名		:  	CalcBEMF                                                        
* @函数描述	: 		对Z进行两次自适应滤波得到反电势
* @创建人		: 刘启进
* 创建日期		:   2020-04-26                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
static inline void CalcBEMF(stSmcObserverExt* pT)
{
	//INT32S Tmp=0;
	/* 计算反电势的第一次滤波 */
	//存上一周期的eAlpha,得到eAlphaOld
	pT->eAlphaOldQ16 = pT->eAlphaQ16;
	//计算本周期的eAlpha
	pT->eAlphaQ16 = SMC_AdjustedFilter(pT->zAlphaQ16,pT->zAlphaOldQ16,pT->eAlphaQ16,pT->Kslf);
	
	//存上一周期的eBeta,得到eBetaOld
	pT->eBetaOldQ16 = pT->eBetaQ16;
	//计算本周期的eBeta
	pT->eBetaQ16 = SMC_AdjustedFilter(pT->zBetaQ16,pT->zBetaOldQ16,pT->eBetaQ16,pT->Kslf);

	/* 计算反电势的第二次滤波 */
	//存上一周期的eAlphaFinal,得到eAlphaFinalOld
	//pT->eAlphaFinalOldQ16 = pT->eAlphaFinalQ16;
	//计算本周期的eAlphaFinal
	pT->eAlphaFinalQ16 = SMC_AdjustedFilter(pT->eAlphaQ16,pT->eAlphaOldQ16,pT->eAlphaFinalQ16,pT->Kslf);

	//存上一周期的eBetaFinal,得到eBetaFinalOld
	//pT->eBetaFinalOldQ16 = pT->eBetaFinalQ16;
	//计算本周期的eBetaFinal
	pT->eBetaFinalQ16 = SMC_AdjustedFilter(pT->eBetaQ16,pT->eBetaOldQ16,pT->eBetaFinalQ16,pT->Kslf);
}

/**************************************************************************************************************************************************
* @函数名		:  	CalcTheta                                                        
* @函数描述	: 		对反电势求反正切获得转子电角度
* @创建人		: 刘启进
* 创建日期		:   2020-04-26                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
static inline void CalcTheta(stSmcObserverExt* pT)
{	 
	//存上一个周期的theta
	pT->thetaINTOld = pT->thetaINT;
	//四象限反正切，输出范围-PI*65536~PI*65536
	pT->thetaQ16  =  _IQatan2(-pT->eAlphaFinalQ16, pT->eBetaFinalQ16);	
	//转成定点格式，数据范围-32768~32767
	pT->thetaINT = pT->thetaQ16 * 5215 >> 15 ;
}

/**************************************************************************************************************************************************
* @函数名		:  	CalcElecRpm                                                        
* @函数描述	: 		对电角度求微分获得电气转速
* @创建人		: 刘启进
* 创建日期		:   2020-04-26                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
static inline void CalcElecRpm(stSmcObserverExt* pT)
{	
	Uint16 i = 0 ;
	//16个周期滑动平均求和
	pT->DeltaTheta[pT->ArrayIndex] = pT->thetaINT - pT->thetaINTOld;
	pT->ArrayIndex = (pT->ArrayIndex + 1)&0x000F;
	pT->DeltaThetaSum = 0;
	for(i=0;i<16;i++)
	{
		pT->DeltaThetaSum += pT->DeltaTheta[i];
	}
	//存老值，单周期角度增量
	pT->OmegaBitOldQ16 =  pT->OmegaBitQ16;
	//计算新值，单周期角度增量，转成Q16格式	
	pT->OmegaBitQ16 = pT->DeltaThetaSum <<12;
	//得到滤波后的单周期角度增量
	pT->OmegaBitFilterdQ16 = SMC_AdjustedFilter(pT->OmegaBitQ16,pT->OmegaBitOldQ16,pT->OmegaBitFilterdQ16,pT->kslfMin);
	pT->SmcElecRpm = ((INT64S)pT->OmegaBitFilterdQ16) * pT->BitPCycleToElecRpmCoef >> 25;
}


/**************************************************************************************************************************************************
* @函数名		:  	CalcKslf                                                        
* @函数描述	: 		根据当前转速计算自适应滤波器滤波系数
* @创建人		: 刘启进
* 创建日期		:   2020-04-26                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
static inline void CalcKslf(stSmcObserverExt* pT)
{
	pT->Kslf = 25736*((INT64S)pT->OmegaBitFilterdQ16) >>30;
	pT->Kslf = (pT->Kslf <pT->kslfMin)?pT->kslfMin:pT->Kslf;
}


/**************************************************************************************************************************************************
* @函数名		:  	ThetaCompesente                                                        
* @函数描述	: 		1.补偿由两次滤波造成的相位延迟
				2.正转速补偿71.6度，负转速补偿108.4度
* @创建人		: 刘启进
* 创建日期		:   2020-04-26                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
static inline void ThetaCompesente(stSmcObserverExt* pT)
{
	pT->thetaOffset = pT->SmcElecRpm >=0 ? 13007:19761;
	pT->SmcElecPos  = pT->thetaINT + pT->thetaOffset;
}

/**************************************************************************************************************************************************
* @函数名		:  	SMC_Position_Estimation                                                        
* @函数描述	: 		滑模观测器，转子位置辨识
* @创建人		: 刘启进
* 创建日期		:   2020-04-26                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
void SMC_Position_Estimation(void)
{
	/*第一步，计算估算电流*/
	CalcEstI(&m1SmcObserverExt);
	/*第二步，根据误差电流计算Z*/
	CalcZ(&m1SmcObserverExt);
	/*第三步，对Z滤波得到反电势*/
	CalcBEMF(&m1SmcObserverExt);
	/*第四步，根据反电势求角度*/
	CalcTheta(&m1SmcObserverExt);
	/*第五步，根据角度增量计算电气转速*/
	CalcElecRpm(&m1SmcObserverExt);
	/*第六步，更新自适应滤波器的滤波系数*/
	CalcKslf(&m1SmcObserverExt);
	/*第七步，角度补偿*/
	ThetaCompesente(&m1SmcObserverExt);
}

/**************************************************************************************************************************************************
* @函数名		:  	SMC_Init                                                        
* @函数描述	: 		滑模观测器结构体初始化				
* @创建人		: 刘启进
* 创建日期		:   2020-04-26                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool SMC_Init(stSmcObserverExt* pT,stCtrlPara* pCtrl,stMotorPara*pMotor,stHalPara*pHal,stSnsLessPara*pSnsLess)
{
	double Ld_H= 0;
	double Rs_ohm = 0;
	double LoopTime_s = 0;
	double SampleFreq_Hz = 0;
	double f = 0;
	double g = 0;
	double gmodify = 1.0;
	double Isnspk = 10.0;
	double KslfMin = 0.0;
	
	if((pT== NULL)||(pCtrl == NULL)||(pMotor == NULL)||(pHal == NULL)||(pSnsLess == NULL))
	{
		return FALSE;
	}
	MemClr((Uint16*)&pT->UAlpha_UseQ16, (Uint16*)&pT->SmcElecRpm);
	
	//===========================================================
	Ld_H = (double)pMotor->PhaseIndD_uH * 0.000001f;
	Rs_ohm = (double)pMotor->PhaseRes_100uohm * 0.0001f;
	SampleFreq_Hz = PWMCLKFREQ/(2*pCtrl->PwmPeriod);
	LoopTime_s =1.0/SampleFreq_Hz;
	// 计算F参数
	if(Rs_ohm*LoopTime_s > Ld_H)
	{
		f = 0.0;
	}
	else
	{
		f = 1-Rs_ohm*LoopTime_s / Ld_H;
	}
	// 计算G参数
	if(LoopTime_s > Ld_H)
	{
		g = 0.99999;
	}
	else
	{
		g = LoopTime_s/Ld_H;
	}
	// F参数、G参数转定点
	Isnspk = (((Uint32)pHal->BitTo10mACoef * 8192)>>14)*0.01;
	gmodify = LIMITMAXMIN(0.5773*270/Isnspk,8,0);
	pT->fsmopos = (INT16S)(f * 32768.0);
	pT->gsmopos = (INT16S)(g * 4096.0*gmodify);/* 20200414由8192->4096,订正归一化错误*/
	pT->SlideGain = pSnsLess->SmcGain;
	pT->SlideMax = 8192L << 16;
	
	//===========================================================
	// 计算速度滤波系数
	KslfMin = (PI*pSnsLess->SpdFilterBand_Hz/SampleFreq_Hz);
	// KslfMin参数转定点
	pT->kslfMin = (INT16S)(KslfMin*32768.0);
	
	//===========================================================
	//pT->BitPCycleToElecRpmCoef =  (INT16S)(SampleFreq_Hz*0.4688);
	pT->BitPCycleToElecRpmCoef =  (INT16S)(SampleFreq_Hz*0.4688)/SPDFDB_SCALE_FACTOR;
	return TRUE;
	
}

