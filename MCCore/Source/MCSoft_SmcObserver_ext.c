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
* @������		:  	SMC_AdjustedFilter                                                        
* @��������	: 		��ģ�۲�������Ӧ�˲���
* @������		: ������
* ��������		:   2020-04-26                                                             
* �޸ļ�¼		:   			
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
* @������		:  	CalcEstI                                                        
* @��������	: 		����������
* @������		: ������
* ��������		:   2020-04-26                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
static inline void CalcEstI(stSmcObserverExt* pT)
{

#if 0 // dq������ͬʱ��ĵ��ƹ�ϵʽ
	estIalpha = _IQmpy(gsmopos, vAlpha) - _IQmpy(gsmopos, eAlpha) - _IQmpy(gsmopos,zAlpha) + _IQmpy(fsmopos,estIalpha);
	estIbeta = _IQmpy(gsmopos, vBeta) - _IQmpy(gsmopos, eBeta) - _IQmpy(gsmopos,zBeta) + _IQmpy(fsmopos,estIbeta);
#endif
#if 0 // dq���в�ͬʱ��ĵ��ƹ�ϵʽ������alpha��beta��ϵ�����  //!!
	estIalpha = -_IQmpy(gsmopos, _IQmpy(_IQmpy(omegaFltred,qOmegaToWeXLdminusLq),TmpIbeta)) \
				+_IQmpy(gsmopos, vAlpha) - _IQmpy(gsmopos, eAlpha) - _IQmpy(gsmopos,zAlpha) + _IQmpy(fsmopos,estIalpha);
	estIbeta = _IQmpy(gsmopos, _IQmpy(_IQmpy(omegaFltred,qOmegaToWeXLdminusLq),TmpIalpha)) \
				+_IQmpy(gsmopos, vBeta) - _IQmpy(gsmopos, eBeta) - _IQmpy(gsmopos,zBeta) + _IQmpy(fsmopos,estIbeta);
#endif

	//����Alpha��������	
	pT->Alpha_G_ResultQ16 = ((INT64S)(pT->UAlpha_UseQ16 - pT->eAlphaQ16 - pT->zAlphaQ16))*pT->gsmopos >>12;
	pT->Alpha_F_ResultQ16 = ((INT64S)pT->EstIalphaQ16)* pT->fsmopos >>15;
	pT->EstIalphaQ16 = pT->Alpha_F_ResultQ16 + pT->Alpha_G_ResultQ16 ;
	pT->EstIalphaQ16 = LIMITMAXMIN(pT->EstIalphaQ16, 16383L<<16, -16384L<<16);

	//����Beta��������	
	pT->Beta_G_ResultQ16 = (INT64S)(pT->UBeta_UseQ16 - pT->eBetaQ16 - pT->zBetaQ16)*pT->gsmopos >>12;
	pT->Beta_F_ResultQ16 = (INT64S)pT->EstIbetaQ16* pT->fsmopos >>15;
	pT->EstIbetaQ16 = pT->Beta_F_ResultQ16 + pT->Beta_G_ResultQ16 ;
	pT->EstIbetaQ16 = LIMITMAXMIN(pT->EstIbetaQ16, 16383L<<16, -16384L<<16);

}

/**************************************************************************************************************************************************
* @������		:  	CalcZ                                                        
* @��������	: 		���㻬ģ���Ƶ������Z
* @������		: ������
* ��������		:   2020-04-26                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
static inline void CalcZ(stSmcObserverExt* pT)
{
	//����һ���ڵ�Zalpha
	pT->zAlphaOldQ16 =  pT->zAlphaQ16;
	//���㱾���ڵ�Zalpha
	pT->IAlphaErrQ16 = pT->EstIalphaQ16 - pT->IAlpha_UseQ16;
	pT->zAlphaQ16 = ((INT64S)pT->IAlphaErrQ16) * pT->SlideGain;
	pT->zAlphaQ16 =LIMITMAXMIN(pT->zAlphaQ16, pT->SlideMax, -pT->SlideMax);

	//����һ���ڵ�Zbeta
	pT->zBetaOldQ16 =  pT->zBetaQ16;
	//���㱾���ڵ�Zbeta
	pT->IBetaErrQ16 = pT->EstIbetaQ16 - pT->IBeta_UseQ16;
	pT->zBetaQ16 = ((INT64S)pT->IBetaErrQ16) * pT->SlideGain;
	pT->zBetaQ16 =LIMITMAXMIN(pT->zBetaQ16, pT->SlideMax, -pT->SlideMax);
}


/**************************************************************************************************************************************************
* @������		:  	CalcBEMF                                                        
* @��������	: 		��Z������������Ӧ�˲��õ�������
* @������		: ������
* ��������		:   2020-04-26                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
static inline void CalcBEMF(stSmcObserverExt* pT)
{
	//INT32S Tmp=0;
	/* ���㷴���Ƶĵ�һ���˲� */
	//����һ���ڵ�eAlpha,�õ�eAlphaOld
	pT->eAlphaOldQ16 = pT->eAlphaQ16;
	//���㱾���ڵ�eAlpha
	pT->eAlphaQ16 = SMC_AdjustedFilter(pT->zAlphaQ16,pT->zAlphaOldQ16,pT->eAlphaQ16,pT->Kslf);
	
	//����һ���ڵ�eBeta,�õ�eBetaOld
	pT->eBetaOldQ16 = pT->eBetaQ16;
	//���㱾���ڵ�eBeta
	pT->eBetaQ16 = SMC_AdjustedFilter(pT->zBetaQ16,pT->zBetaOldQ16,pT->eBetaQ16,pT->Kslf);

	/* ���㷴���Ƶĵڶ����˲� */
	//����һ���ڵ�eAlphaFinal,�õ�eAlphaFinalOld
	//pT->eAlphaFinalOldQ16 = pT->eAlphaFinalQ16;
	//���㱾���ڵ�eAlphaFinal
	pT->eAlphaFinalQ16 = SMC_AdjustedFilter(pT->eAlphaQ16,pT->eAlphaOldQ16,pT->eAlphaFinalQ16,pT->Kslf);

	//����һ���ڵ�eBetaFinal,�õ�eBetaFinalOld
	//pT->eBetaFinalOldQ16 = pT->eBetaFinalQ16;
	//���㱾���ڵ�eBetaFinal
	pT->eBetaFinalQ16 = SMC_AdjustedFilter(pT->eBetaQ16,pT->eBetaOldQ16,pT->eBetaFinalQ16,pT->Kslf);
}

/**************************************************************************************************************************************************
* @������		:  	CalcTheta                                                        
* @��������	: 		�Է����������л��ת�ӵ�Ƕ�
* @������		: ������
* ��������		:   2020-04-26                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
static inline void CalcTheta(stSmcObserverExt* pT)
{	 
	//����һ�����ڵ�theta
	pT->thetaINTOld = pT->thetaINT;
	//�����޷����У������Χ-PI*65536~PI*65536
	pT->thetaQ16  =  _IQatan2(-pT->eAlphaFinalQ16, pT->eBetaFinalQ16);	
	//ת�ɶ����ʽ�����ݷ�Χ-32768~32767
	pT->thetaINT = pT->thetaQ16 * 5215 >> 15 ;
}

/**************************************************************************************************************************************************
* @������		:  	CalcElecRpm                                                        
* @��������	: 		�Ե�Ƕ���΢�ֻ�õ���ת��
* @������		: ������
* ��������		:   2020-04-26                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
static inline void CalcElecRpm(stSmcObserverExt* pT)
{	
	Uint16 i = 0 ;
	//16�����ڻ���ƽ�����
	pT->DeltaTheta[pT->ArrayIndex] = pT->thetaINT - pT->thetaINTOld;
	pT->ArrayIndex = (pT->ArrayIndex + 1)&0x000F;
	pT->DeltaThetaSum = 0;
	for(i=0;i<16;i++)
	{
		pT->DeltaThetaSum += pT->DeltaTheta[i];
	}
	//����ֵ�������ڽǶ�����
	pT->OmegaBitOldQ16 =  pT->OmegaBitQ16;
	//������ֵ�������ڽǶ�������ת��Q16��ʽ	
	pT->OmegaBitQ16 = pT->DeltaThetaSum <<12;
	//�õ��˲���ĵ����ڽǶ�����
	pT->OmegaBitFilterdQ16 = SMC_AdjustedFilter(pT->OmegaBitQ16,pT->OmegaBitOldQ16,pT->OmegaBitFilterdQ16,pT->kslfMin);
	pT->SmcElecRpm = ((INT64S)pT->OmegaBitFilterdQ16) * pT->BitPCycleToElecRpmCoef >> 25;
}


/**************************************************************************************************************************************************
* @������		:  	CalcKslf                                                        
* @��������	: 		���ݵ�ǰת�ټ�������Ӧ�˲����˲�ϵ��
* @������		: ������
* ��������		:   2020-04-26                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
static inline void CalcKslf(stSmcObserverExt* pT)
{
	pT->Kslf = 25736*((INT64S)pT->OmegaBitFilterdQ16) >>30;
	pT->Kslf = (pT->Kslf <pT->kslfMin)?pT->kslfMin:pT->Kslf;
}


/**************************************************************************************************************************************************
* @������		:  	ThetaCompesente                                                        
* @��������	: 		1.�����������˲���ɵ���λ�ӳ�
				2.��ת�ٲ���71.6�ȣ���ת�ٲ���108.4��
* @������		: ������
* ��������		:   2020-04-26                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
static inline void ThetaCompesente(stSmcObserverExt* pT)
{
	pT->thetaOffset = pT->SmcElecRpm >=0 ? 13007:19761;
	pT->SmcElecPos  = pT->thetaINT + pT->thetaOffset;
}

/**************************************************************************************************************************************************
* @������		:  	SMC_Position_Estimation                                                        
* @��������	: 		��ģ�۲�����ת��λ�ñ�ʶ
* @������		: ������
* ��������		:   2020-04-26                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
void SMC_Position_Estimation(void)
{
	/*��һ��������������*/
	CalcEstI(&m1SmcObserverExt);
	/*�ڶ�������������������Z*/
	CalcZ(&m1SmcObserverExt);
	/*����������Z�˲��õ�������*/
	CalcBEMF(&m1SmcObserverExt);
	/*���Ĳ������ݷ�������Ƕ�*/
	CalcTheta(&m1SmcObserverExt);
	/*���岽�����ݽǶ������������ת��*/
	CalcElecRpm(&m1SmcObserverExt);
	/*����������������Ӧ�˲������˲�ϵ��*/
	CalcKslf(&m1SmcObserverExt);
	/*���߲����ǶȲ���*/
	ThetaCompesente(&m1SmcObserverExt);
}

/**************************************************************************************************************************************************
* @������		:  	SMC_Init                                                        
* @��������	: 		��ģ�۲����ṹ���ʼ��				
* @������		: ������
* ��������		:   2020-04-26                                                             
* �޸ļ�¼		:   			
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
	// ����F����
	if(Rs_ohm*LoopTime_s > Ld_H)
	{
		f = 0.0;
	}
	else
	{
		f = 1-Rs_ohm*LoopTime_s / Ld_H;
	}
	// ����G����
	if(LoopTime_s > Ld_H)
	{
		g = 0.99999;
	}
	else
	{
		g = LoopTime_s/Ld_H;
	}
	// F������G����ת����
	Isnspk = (((Uint32)pHal->BitTo10mACoef * 8192)>>14)*0.01;
	gmodify = LIMITMAXMIN(0.5773*270/Isnspk,8,0);
	pT->fsmopos = (INT16S)(f * 32768.0);
	pT->gsmopos = (INT16S)(g * 4096.0*gmodify);/* 20200414��8192->4096,������һ������*/
	pT->SlideGain = pSnsLess->SmcGain;
	pT->SlideMax = 8192L << 16;
	
	//===========================================================
	// �����ٶ��˲�ϵ��
	KslfMin = (PI*pSnsLess->SpdFilterBand_Hz/SampleFreq_Hz);
	// KslfMin����ת����
	pT->kslfMin = (INT16S)(KslfMin*32768.0);
	
	//===========================================================
	//pT->BitPCycleToElecRpmCoef =  (INT16S)(SampleFreq_Hz*0.4688);
	pT->BitPCycleToElecRpmCoef =  (INT16S)(SampleFreq_Hz*0.4688)/SPDFDB_SCALE_FACTOR;
	return TRUE;
	
}

