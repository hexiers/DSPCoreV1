/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	MCCore FPGA��غ���
* @������		:  	����                                                                              
* ��������		:   	2020-03-06                                                                 
* �޸ļ�¼		:   
*           20200911:����SPDFDB_SCALE_FACTOR ������Ӧ�Ը�ת��     
======================================================================================================*/
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#include "MCCore_BusVolDecoupler.h"
#include "MCCore_CurrLoopCtler.h"
#include "MCCore_FaultDealModule.h"
#include "MCCore_ResolverDealModule.h"
#include "MCCore_SmcObserver.h"
#include "MCCore_SpdLoopCtler.h"
#include "MCCore_FluxEst.h"

/* FPGA ������ģ��ṹ��*/
/*
#pragma DATA_SECTION(m1CurrLoopCtler,"CurrLoopCtlerFile");
volatile  stCurrLoopCtler m1CurrLoopCtler;

#pragma DATA_SECTION(m1SpdLoopCtler,"SpdLoopCtlerFile");
volatile  stSpdLoopCtler m1SpdLoopCtler;

#pragma DATA_SECTION(m1BusVolDecoupler,"BusVolDecouplerFile");
volatile stBusVolDecoupler m1BusVolDecoupler;

#pragma DATA_SECTION(m1SmcObserver,"SmcObserverFile");
volatile  stSmcObserver m1SmcObserver;

#pragma DATA_SECTION(m1ResolverDealModule,"ResolverDealModuleFile");
volatile  stResolverDealModule m1ResolverDealModule;

#pragma DATA_SECTION(m1FaultDealModule,"FaultDealModuleFile");
volatile  stFaultDealModule m1FaultDealModule;
*/
//#pragma DATA_SECTION(m1CurrLoopCtler,"CurrLoopCtlerFile");
  stCurrLoopCtler m1CurrLoopCtler;

//#pragma DATA_SECTION(m1SpdLoopCtler,"SpdLoopCtlerFile");
  stSpdLoopCtler m1SpdLoopCtler;
  stFwCtler m1FwCtler;

//#pragma DATA_SECTION(m1BusVolDecoupler,"BusVolDecouplerFile");
 stBusVolDecoupler m1BusVolDecoupler;

//#pragma DATA_SECTION(m1SmcObserver,"SmcObserverFile");
//  stSmcObserver m1SmcObserver;

//#pragma DATA_SECTION(m1ResolverDealModule,"ResolverDealModuleFile");
  stResolverDealModule m1ResolverDealModule;

//#pragma DATA_SECTION(m1FaultDealModule,"FaultDealModuleFile");
  stFaultDealModule m1FaultDealModule;

  stSmcObserverExt m1SmcObserverExt;

  Flux_Est m1FluxObserver;

/**************************************************************************************************************************************************
* @������		:  		CfgCurrLoopCtler                                                        
* @��������	: 		���ݲ����ṹ�����õ������������������ʼ������²��������
* @������		:    		����                                                                            
* ��������		:   		2020-03-06                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool CfgCurrLoopCtler(stCurrLoopCtler* pD,stCtrlPara* pCtrl,stMotorPara*pMotor,stHalPara*pHal)
{
	if((pD == NULL)||(pCtrl == NULL)||(pMotor == NULL)||(pHal == NULL))
	{
		return FALSE;
	}
	
	MemClr((Uint16*)pD, (Uint16*)pD+sizeof(stCurrLoopCtler)/sizeof(Uint16));// DSPCore Only

	pD->IdKp = pCtrl->IdKp;
	pD->IdKi = pCtrl->IdKi;
	pD->IqKp = pCtrl->IdKp;
	pD->IqKi = pCtrl->IdKi;
	pD->PwmPeriod = pCtrl->PwmPeriod;
	pD->PosOff = pMotor->PosOff;
	pD->DeadTime = pCtrl->DeadTime;
	pD->IOffU = pHal->UCurrOff;
	pD->IOffW = pHal->VCurrOff;
	pD->MotCtl.bit.EnUCurrNeg = 1;
	pD->MotCtl.bit.EnVCurrNeg = 1;
	/* ��ʱѡ��ᱻ�������*/
	pD->MotCtl.bit.PosSel = m1SysCfgPara.SysCfgWord1.bit.EnSnsLess?1:0;
	pD->MotCtl.bit.EnBusModify = m1SysCfgPara.SysCfgWord1.bit.EnBusMod?1:0;
	pD->MotCtl.bit.EnUCurrNeg = 1;/* ������������װ*/
	pD->MotCtl.bit.EnVCurrNeg = 1;/* ������������װ*/
	pD->MotCtl.bit.CurrAntiRstEn = 1;
	pD->MotCtl.bit.SpdAntiRstEn = 1;
	//pD->MotCtl.bit.UserSmcIn = 1;/* ѡ���û�����������Ϊ�۲�������*/
	pD->MotCtl.bit.UserSmcIn = 0;/* ѡ���ڲ�������Ϊ�۲�������*/
	return TRUE;
}
/**************************************************************************************************************************************************
* @������		:  		CfgCurrloopKpKi                                                        
* @��������	: 		����Kp Ki
* @������		:    		����                                                                            
* ��������		:   		2020-06-16                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/

bool CfgCurrloopKpKi(stCurrLoopCtler* pD,INT16S IdKp,INT16S IdKi,INT16S IqKp,INT16S IqKi)
{
	if(pD == NULL)
	{
		return FALSE;
	}
	pD->IdKp = IdKp;
	pD->IdKi = IdKi;
	pD->IqKp = IqKp;
	pD->IqKi = IqKi;
	return TRUE;
}

/**************************************************************************************************************************************************
* @������		:  		CfgSpdLoopCtler                                                        
* @��������	: 		���ݲ����ṹ�������ٶȻ��������������ʼ������²��������
* @������		:    		����                                                                            
* ��������		:   		2020-03-06                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool CfgSpdLoopCtler(stSpdLoopCtler* pD,stCtrlPara* pCtrl,stMotorPara*pMotor,stHalPara*pHal)
{
	if((pD == NULL)||(pCtrl == NULL)||(pMotor == NULL)||(pHal == NULL))
	{
		return FALSE;
	}
	MemClr((Uint16*)pD, (Uint16*)pD+sizeof(stSpdLoopCtler)/sizeof(Uint16));// DSPCore Only

	pD->SpdKp = pCtrl->SpdInitKp;
	pD->SpdKi = pCtrl->SpdKi;
	pD->SpdOutMax = ((Uint32)pHal->F10mAToBitCoef * (INT32S)pMotor->RatedCurrentQ_10mA)>>12;
	pD->SpdOutMin = 0;
	pD->SpdIntergPreset = 0;
	
	return TRUE;
}

/**************************************************************************************************************************************************
* @������		:  		CfgBusVolDecoupler                                                        
* @��������	: 		���ݲ����ṹ������ĸ�ߵ�ѹ�����ģ�飬�����ʼ�������
* @������		:    		����                                                                            
* ��������		:   		2020-03-06                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool CfgBusVolDecoupler(stBusVolDecoupler* pD,stHalPara*pHal)
{
	if((pD == NULL)||(pHal == NULL))
	{
		return FALSE;
	}
	pD->RatedVolBit = (1200L*(Uint32)pHal->F10mVToBitCoef)>>12;
	return TRUE;
}

/**************************************************************************************************************************************************
* @������		:  		CfgSmcObserver                                                        
* @��������	: 		���ݲ����ṹ�����û�ģ�۲���ģ�飬�����ʼ�������
* @������		:    		����                                                                            
* ��������		:   		2020-03-06                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
/*
bool CfgSmcObserver(stSmcObserver* pD,stCtrlPara* pCtrl,stMotorPara*pMotor,stHalPara*pHal,stSnsLessPara*pSnsLess)
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
	
	if((pD == NULL)||(pCtrl == NULL)||(pMotor == NULL)||(pHal == NULL)||(pSnsLess == NULL))
	{
		return FALSE;
	}
	
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
	Isnspk = (((Uint32)pHal->BitTo10mACoef * 8192)>>12)*0.01;
	gmodify = LIMITMAXMIN(0.5773*270/Isnspk,8,0);
	pD->fsmopos = (INT16S)(f * 32768.0);
	pD->gsmopos = (INT16S)(g * 4096.0*gmodify);
	pD->SlideGain = pSnsLess->SmcGain;
	pD->SlideMax = 8192;
	
	//===========================================================
	// �����ٶ��˲�ϵ��
	KslfMin = (PI*pSnsLess->SpdFilterBand_Hz/SampleFreq_Hz);
	// KslfMin����ת����
	pD->kslfMin = (INT16S)(KslfMin*32768.0);
	
	//===========================================================
	pD->BitPCycleToElecRpmCoef =  (INT16S)(SampleFreq_Hz*0.4688)/SPDFDB_SCALE_FACTOR;
	
	return TRUE;
}*/
/**************************************************************************************************************************************************
* @������		:  		CfgResolverDealModule                                                        
* @��������	: 		��������ӿ�ģ��
* @������		:    		����                                                                            
* ��������		:   		2020-03-06                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool CfgResolverDealModule(stResolverDealModule* pD,stCurrLoopCtler* pC,stMotorPara*pMotor)
{

	Uint16 i = 0;
	if(pD == NULL)
	{
		return FALSE;
	}

	// ���ü��Աȣ�������
	if(pMotor->MotorPoles == pMotor->SnsorPoles)
	{
		pC->MotCtl.bit.PolesRatio = 0;	
	}
	else if( pMotor->SnsorPoles == 1)
	{
		pC->MotCtl.bit.PolesRatio  = pMotor->MotorPoles -1;
	}
	else
	{
		// ��֧��
		MC_PostCodeErr(1);
		return FALSE;
	}
	
	
	// 12λ�ֱ���
	pD->Ad2sCfgReg = 0x7E;
	

    pD->SpdBitToRpmCoef = 7500L * 8000/(8192L*SPDFDB_SCALE_FACTOR) ;// 8MHz����
 	
	/*// 14λ�ֱ���
	pD->Ad2sCfgReg = 0x7D; 
	pD->SpdBitToRpmCoef = 3750;
	*/
	pD->Ad2sExcFreqReg = 0x28;
	pD->SpdModifyCoef = 8192;
	pC->MotCtl.bit.ResolverCfg = 1;
	Delay_us(10);
	pC->MotCtl.bit.ResolverCfg = 0;
	Delay_us(50000);/* AD2S1210��λ��ĸ���ʱ��Ϊ20ms@12bit,25ms@14bit*/

	for(i = 0;i<20;i++)
	{
		if((pD->FaultReg & 0x100) == 0x100)// ����ʱ�����й���
		{
			//������ת��ѹ������
			pC->MotCtl.bit.ResolverCfg = 1;
			Delay_us(10);
			pC->MotCtl.bit.ResolverCfg = 0;
			Delay_us(50000);
			//HAL_RstAd2s1210();
		}
		else
		{
			break;
		}
	}
	//�ϵ��������
	pC->MotCtl.bit.ClrErr = 1;

	return TRUE;
}

/**************************************************************************************************************************************************
* @������		:  		CfgFaultDealModule                                                        
* @��������	: 		���ù��ϼ��ģ��
* @������		:    		����                                                                            
* ��������		:   		2020-03-06                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool CfgFaultDealModule(stFaultDealModule* pD,stFaultPara* pF,stHalPara*pHal)
{


	if((pD == NULL)||(pF == NULL)||(pHal == NULL))
	{
		return FALSE;
	}
	
	pD->OVPoint = ((Uint32)pF->HPOvFaultPoint *(Uint32)pHal->F10mVToBitCoef) >> 12;
	pD->UVPoint = ((Uint32)pF->HPUvFaultPoint *(Uint32)pHal->F10mVToBitCoef) >> 12;
	return TRUE;
}

