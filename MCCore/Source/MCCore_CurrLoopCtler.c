/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	MCCore DSPִ�е���������
* @������		:  	����                                                                              
* ��������		:   	2020-03-06                                                                 
* �޸ļ�¼		:   
*            
======================================================================================================*/


#include "MCCore_CurrLoopCtler.h"
#include "math.h"
/**************************************************************************************************************************************************
* @������		:  		
* @��������	: 		
* @������		:    		����                                                                            
* ��������		:   		2020-09-27                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
void DSPCore_SwitchOffPWM(void)
{

	EPwm1Regs.AQCSFRC.bit.CSFA = 1;//ǿ�������
	EPwm1Regs.AQCSFRC.bit.CSFB = 2;//ǿ�Ƹߣ���DB����������
	EPwm2Regs.AQCSFRC.bit.CSFA = 1;//ǿ�������
	EPwm2Regs.AQCSFRC.bit.CSFB = 2;//ǿ�Ƹߣ���DB����������
	EPwm3Regs.AQCSFRC.bit.CSFA = 1;//ǿ�������
	EPwm3Regs.AQCSFRC.bit.CSFB = 2;//ǿ�Ƹߣ���DB����������
}
/**************************************************************************************************************************************************
* @������		:  		
* @��������	: 		
* @������		:    		����                                                                            
* ��������		:   		2020-09-27                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
void DSPCore_SwitchOnPWM(void)
{
	EPwm1Regs.AQCSFRC.bit.CSFA = 0;//ǿ�������Ч
	EPwm1Regs.AQCSFRC.bit.CSFB = 0;//ǿ�������Ч
	EPwm2Regs.AQCSFRC.bit.CSFA = 0;//ǿ�������Ч
	EPwm2Regs.AQCSFRC.bit.CSFB = 0;//ǿ�������Ч
	EPwm3Regs.AQCSFRC.bit.CSFA = 0;//ǿ�������Ч
	EPwm3Regs.AQCSFRC.bit.CSFB = 0;//ǿ�������Ч
}
/**************************************************************************************************************************************************
* @������		:  		��·���ϡ���������
* @��������	: 		
* @������		:    		����                                                                            
* ��������		:   		2020-09-27                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
// INT2.1
interrupt void EPWM1_TZINT_ISR(void)    // EPWM-1
{
	if((TZ_OC == 0)&&(TZ_SC == 0))
	{
		m1CurrLoopCtler.ErrSignal.bit.OCERR = 1;		
		m1CurrLoopCtler.ErrSignal.bit.SCERR = 1;
	}	
	else if((TZ_OC == 0)&&(TZ_SC == 1))
	{
		m1CurrLoopCtler.ErrSignal.bit.OCERR = 1;	
		m1CurrLoopCtler.ErrSignal.bit.SCERR = 0;
	}
	else if((TZ_OC == 1)&&(TZ_SC == 0))
	{
		m1CurrLoopCtler.ErrSignal.bit.OCERR = 0;	
		m1CurrLoopCtler.ErrSignal.bit.SCERR = 1;
	}
	else/* ���IO��ƽû�м�⵽���ϣ�����Ϊ������·(���������)*/
	{
		m1CurrLoopCtler.ErrSignal.bit.OCERR = 0;	
		m1CurrLoopCtler.ErrSignal.bit.SCERR = 1;
	}
	
	DSPCore_SwitchOffPWM();

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
	EALLOW;
	EPwm1Regs.TZCLR.bit.OST = 1;
	EPwm2Regs.TZCLR.bit.OST = 1;
	EPwm3Regs.TZCLR.bit.OST = 1;
	EPwm1Regs.TZCLR.bit.INT = 1;
	EDIS;


}

/**************************************************************************************************************************************************
* @������		:  		
* @��������	: ��ͬ��ԭEPS�ķ����߼���Ϊ��ʹ�� �Ƚ�ֵԽ��ռ�ձ�Խ��	��
							�����˶���ģ�飬������PWM����Ϊ0ʱ������ADC		
* @������		:    		����                                                                            
* ��������		:   		2020-09-27                                                             
* �޸ļ�¼		:   			
*                           
****************************************************************************************************************************************************/
void DSPCore_InitEPwm(volatile struct EPWM_REGS*p,Uint16 PwmPeriod,Uint16 DeadTime)
{
	//======================================================================
	//===============ʱ�����
	//ʱ������
	p->TBPRD = PwmPeriod;
	//ʱ��ƫ��
	p->TBPHS.half.TBPHS =0;
	//ʱ������ֵ
	p->TBCTR=0;
	p->TBCTL.bit.FREE_SOFT = 2;//����ģʽʱ����������
	p->TBCTL.bit.PHSDIR = 1;//ͬ���źŵ����󣬼�����װ�غ����ϼ���
	p->TBCTL.bit.CLKDIV = 1;//PWM����60MHz DSPCore
	p->TBCTL.bit.HSPCLKDIV =0;
	p->TBCTL.bit.SYNCOSEL = 1;//ͬ���ź����ѡ��ctr = 0
	p->TBCTL.bit.PRDLD = 0;//���ڼĴ�������ʹ��Ӱ�ӼĴ���
	p->TBCTL.bit.PHSEN = 1;//��ͬ���źŵ���ʱʹ����λ�Ĵ�����Ϊ�����Ĵ����ļ���ֵ
	p->TBCTL.bit.CTRMODE = 2;//�������¼���ģʽ
	//======================================================================
	//===============�����Ƚ�
	p->CMPA.half.CMPA = PwmPeriod;//Ĭ�����0ռ�ձ�
	p->CMPB = PwmPeriod;//Ĭ�����0ռ�ձ�
	p->CMPCTL.bit.LOADAMODE = 2;//����ƥ�����0������
	p->CMPCTL.bit.LOADBMODE = 2;//����ƥ�����0������
	p->CMPCTL.bit.SHDWAMODE = 0;//�ȽϼĴ���ʹ��Ӱ�ӼĴ���
	p->CMPCTL.bit.SHDWBMODE = 0;//�ȽϼĴ���ʹ��Ӱ�ӼĴ���
	//======================================================================
	//===============����ģ��
	p->AQCTLA.bit.CAU = 1;//���ϼ���ƥ��ʱ�����DSPCore
	p->AQCTLA.bit.CAD = 2;//���¼���ƥ��ʱ�����DSPCore
	p->AQCTLA.bit.ZRO = 0;
	p->AQCTLA.bit.PRD = 0;
	p->AQCTLA.bit.CBU = 0;
	p->AQCTLA.bit.CBD = 0;
	p->AQCTLB.bit.CAU = 1;//���ϼ���ƥ��ʱ�����DSPCore
	p->AQCTLB.bit.CAD = 2;//���¼���ƥ��ʱ�����DSPCore
	p->AQCTLB.bit.ZRO = 0;
	p->AQCTLB.bit.PRD = 0;
	p->AQCTLB.bit.CBD = 0;
	p->AQCTLB.bit.CBU = 0;

	p->AQSFRC.bit.RLDCSF = 2;//����ƥ�����0������ AQCSFRC
	//�������ǿ��
	DSPCore_SwitchOffPWM();
	//======================================================================
	//===============��������
	p->DBCTL.bit.IN_MODE = 2;//PWMA��PWMB������ΪDB����
	p->DBCTL.bit.POLSEL = 2;//PWM���Aͬ��B����
	p->DBCTL.bit.OUT_MODE = 3;//ʹ��ǰ������
	p->DBRED = DeadTime;
	p->DBFED = DeadTime;
	//====================s==================================================
	//===============ն������
	p->PCCTL.bit.CHPEN = 0;//������ն������
	//======================================================================
	//===============��������
	p->TZSEL.bit.OSHT1 = 1;//ʹ��TZ1�ĵ��δ�����������
	p->TZSEL.bit.OSHT2 = 1;//ʹ��TZ2�ĵ��δ�����������
	p->TZCTL.bit.TZA = 0;//�����¼�����ʱ���������
	p->TZCTL.bit.TZB = 0;
	p->TZEINT.bit.OST = 0;//��ʹ�ܵ��δ����ж�
	p->TZCLR.bit.OST = 1;//������δ����ж�
	//======================================================================
	//===============�¼�����
	p->ETSEL.bit.SOCAEN = 1;//ʹ��soca�źŲ���
	p->ETSEL.bit.SOCASEL = 1;//ʱ��Ϊ0ʱ������ad�����ź� DSPCore
	p->ETPS.bit.SOCAPRD = 1;//����һ���¼��㴥��ad�����ź�
}


/**************************************************************************************************************************************************
* @������		:  		DSPCore_CfgCurrLoopCtler                                                        
* @��������	: 		��CurrLoopCtler��������صĲ������µ�DSP ��PWMģ����ȥ
* @������		:    		����                                                                            
* ��������		:   		2020-03-06                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_CfgCurrLoopCtler(stCurrLoopCtler* pD)
{
	EALLOW;
	DSPCore_InitEPwm(&EPwm1Regs,pD->PwmPeriod,pD->DeadTime);
	DSPCore_InitEPwm(&EPwm2Regs,pD->PwmPeriod,pD->DeadTime);
	DSPCore_InitEPwm(&EPwm3Regs,pD->PwmPeriod,pD->DeadTime);
	EPwm1Regs.TZEINT.bit.OST = 1;//ʹ��PWM1�ĵ��δ����ж�
	EDIS;
	return TRUE;
}
/**************************************************************************************************************************************************
* @������		:  		DSPCore_DoCurrLoopCtrl
* @��������	: 		ִ�е���������
* @������		:    		����                                                                            
* ��������		:   		2020-09-27                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_DoCurrLoopCtrl(stCurrLoopCtler* pD,stResolverDealModule* pR,stSmcObserverExt* pS,stBusVolDecoupler* pBus)
{
	volatile INT16S IFdbU = 0;
	volatile INT16S IFdbW = 0;
	_iq iqCosTheta = 0;
	_iq iqSinTheta = 0;
	_iq iqTheta = 0;
	INT16S UdPIOut_V = 0;
	INT16S UqPIOut_V = 0;
	INT32S Uq_BIT_Max = 0;
	INT32S SPWMVa = 0;
	INT32S SPWMVb = 0;
	INT32S SPWMVc = 0;
	INT16S Max = 0;
	INT16S Min = 0;
	INT16S VCom = 0;
	/************��ȡ�Ƕ�*************/
	if(pD->MotCtl.bit.PosSel == 0)
	{
		pD->ThetaPark = pR->ResolverElecPos - pD->PosOff;
	}
	else if(pD->MotCtl.bit.PosSel == 1)
	{
		pD->ThetaPark = pD->ManuPos;
	}
	else if(pD->MotCtl.bit.PosSel == 2)
	{
		pD->ThetaPark = pS->SmcElecPos;
	}
	else if(pD->MotCtl.bit.PosSel == 3)
		{
			pD->ThetaPark = (INT16S)(Uint16)((m1FluxObserver.FluxEst_Angle/PI)*32768L);//+m1FluxObserver.DEALTA;
		}
	else
	{
		pD->ThetaPark = pD->ManuPos;
	}	
	
	/************ƫ������*************/
	pD->IOriU= GetADResultModifyDef(AD_MAIN_CURR_U,m1HalPara.ADGain,m1HalPara.ADOff)  << 2;// ��һ����[-8192,8191]
	IFdbU = pD->MotCtl.bit.EnUCurrNeg?(pD->IOffU - pD->IOriU):(pD->IOriU -pD->IOffU);
	// V��������EPS������W��
	pD->IOriW= GetADResultModifyDef(AD_MAIN_CURR_W,m1HalPara.ADGain,m1HalPara.ADOff)  << 2;// ��һ����[-8192,8191]
	IFdbW = pD->MotCtl.bit.EnVCurrNeg?(pD->IOffW - pD->IOriW):(pD->IOriW -pD->IOffW);
	
	/************Clark���任*************/
	pD->IFdbAlpha = IFdbU;
	pD->IFdbBeta = ((INT32S)(-IFdbU - 2* IFdbW))*18919L >> 15;

	/************Park���任*************/
	iqTheta = (INT64S)pD->ThetaPark * PIQ16*2 >> 16; // ���Q16��ʽ��iqTheta
	iqCosTheta = _IQ16cos(iqTheta);	 
	iqSinTheta = _IQ16sin(iqTheta);
	pD->IdFdb =  ((INT32S)pD->IFdbAlpha*iqCosTheta + (INT32S)pD->IFdbBeta*iqSinTheta)>>16;
	pD->IqFdb =  (-(INT32S)pD->IFdbAlpha*iqSinTheta + (INT32S)pD->IFdbBeta*iqCosTheta)>>16;

	/************PI����*************/
	pD->DeltaD = pD->IdCmd - pD->IdFdb;
	pD->IdProp =  LIMITMAXMIN(((INT32S)pD->DeltaD * pD->IdKp)>>7,8191,-8192);
	pD->IdIntergLim32 = (INT32S)pD->DeltaD * pD->IdKi
									+ ((INT32S)pD->DisChageD << 7)* pD->MotCtl.bit.CurrAntiRstEn
									+ pD->IdIntergLim32;
	pD->IdIntergLim32 = LIMITMAXMIN(pD->IdIntergLim32, 1048575, -1048576);// ���Ƶ�[-2^20,2^20]
	if((pD->IdKi == 0)||(pD->MotCtl.bit.Start == 0))/* �ǿ���״̬�����������0*/
	{
		pD->IdIntergLim32 = 0;
	}
	pD->IdIntergLim = pD->IdIntergLim32 >> 7;
	UdPIOut_V = pD->IdProp + pD->IdIntergLim + pD->UdCmd_V;
	pD->UdPILim_V = LIMITMAXMIN(UdPIOut_V,8191,-8192);
	pD->UdOut_BIT = LIMITMAXMIN((INT32S)pD->UdPILim_V * pBus->BusCoef >> 13,7094,-7094);
	pD->UdActOut_V = (INT32S)pD->UdOut_BIT * pBus->VBusProtect >> 13;
	pD->DisChageD = pD->UdActOut_V - UdPIOut_V;

	Uq_BIT_Max = sqrt(8192L * 8192L - (INT32S)pD->UdOut_BIT * pD->UdOut_BIT);

/*	if(pD->MotCtl.bit.EnSpdCtrl)
	{
		pD->DeltaQ = m1SpdLoopCtler.SpdLimOut - pD->IqFdb;	
	}
	else
	{
		pD->DeltaQ = pD->IqCmd - pD->IqFdb;
	}*/
	pD->DeltaQ = pD->IqCmd - pD->IqFdb;
	pD->IqProp =  LIMITMAXMIN(((INT32S)pD->DeltaQ * pD->IqKp)>>7,8191,-8192);
	pD->IqIntergLim32 = (INT32S)pD->DeltaQ * pD->IqKi
									+ ((INT32S)pD->DisChageQ << 7)* pD->MotCtl.bit.CurrAntiRstEn
									+ pD->IqIntergLim32;
	if((pD->IqKi == 0)||(pD->MotCtl.bit.Start == 0))/* �ǿ���״̬�����������0*/
	{
		pD->IqIntergLim32 = 0;
	}
	pD->IqIntergLim32 = LIMITMAXMIN(pD->IqIntergLim32, 1048575, -1048576);// ���Ƶ�[-2^20,2^20]
	pD->IqIntergLim = pD->IqIntergLim32 >> 7;
	UqPIOut_V = pD->IqProp + pD->IqIntergLim + pD->UqCmd_V;
	pD->UqPILim_V = LIMITMAXMIN(UqPIOut_V,8191,-8192);
	pD->UqOut_BITOri = LIMITMAXMIN((INT32S)pD->UqPILim_V * pBus->BusCoef >> 13,16383,-16384);
	pD->UqOut_BIT = LIMITMAXMIN(pD->UqOut_BITOri,Uq_BIT_Max,-Uq_BIT_Max);
	pD->UqActOut_V = (INT32S)pD->UqOut_BIT * pBus->VBusProtect >> 13;
	pD->DisChageQ = pD->UqActOut_V - UqPIOut_V;
	
	/************Park��任*************/
	pD->ThetaInvPark = pD->ThetaPark + (pD->PosModify>>3);
	iqTheta = (INT64S)pD->ThetaInvPark * PIQ16*2 >> 16; // ���Q16��ʽ��iqTheta
	iqCosTheta = _IQ16cos(iqTheta);	 
	iqSinTheta = _IQ16sin(iqTheta);
	pD->UAlpha_BIT = ((INT32S)pD->UdOut_BIT * iqCosTheta - (INT32S)pD->UqOut_BIT * iqSinTheta)>>16;
	pD->UBeta_BIT = ((INT32S)pD->UdOut_BIT * iqSinTheta + (INT32S)pD->UqOut_BIT * iqCosTheta)>>16;
	// ��SMC��
	pD->UAlpha_V = (INT32S)pD->UAlpha_BIT * pBus->VBusProtect >> 13;
	pD->UBeta_V = (INT32S)pD->UBeta_BIT  * pBus->VBusProtect >> 13;
	
	/************Clark��任*************/
	SPWMVa = pD->UAlpha_BIT;
	SPWMVb = -(pD->UAlpha_BIT >> 1) + ((INT32S)pD->UBeta_BIT * 28378L >> 15);
	SPWMVc = -(pD->UAlpha_BIT >> 1) - ((INT32S)pD->UBeta_BIT * 28378L >> 15);
	SPWMVb = LIMITMAXMIN(SPWMVb, 8191, -8192);
	SPWMVc = LIMITMAXMIN(SPWMVc, 8191, -8192);

	SPWMVa = SPWMVa * 18918L >>15;
	SPWMVb = SPWMVb * 18918L >>15;
	SPWMVc = SPWMVc * 18918L >>15;

	/************SVPWM����*************/
	if(SPWMVa > SPWMVb)
	{
		Max = SPWMVa;
		Min = SPWMVb;
	}
	else
	{
		Max = SPWMVb;
		Min = SPWMVa;
	}	
	if(SPWMVc > Max)
	{
		Max = SPWMVc;
	}
	else if(SPWMVc < Min)
	{
		Min = SPWMVc;
	}
	VCom = 4096 -((Max + Min) >> 1);

	//********** ��������*********************
		INT16S TdA=pD->DeadTime;
		INT16S TdB=pD->DeadTime;
		INT16S TdC=pD->DeadTime;

		if (m1FluxObserver.TdComEn==1)
		{
			TdA=(IFdbU>0?1:-1)* (TdA<<13)/pD->PwmPeriod;
			TdB=(IFdbW>0?1:-1)* (TdA<<13)/pD->PwmPeriod;
			TdC=((-IFdbU-IFdbW)>0?1:-1)* (TdA<<13)/pD->PwmPeriod;
		}
	SPWMVa = LIMITMAXMIN(SPWMVa + VCom + TdA, 8191, 0);
	SPWMVb = LIMITMAXMIN(SPWMVb + VCom + TdB, 8191, 0);
	SPWMVc = LIMITMAXMIN(SPWMVc + VCom + TdC, 8191, 0);

	/************  ���ڽ���   *************/
	pD->PwmU_CMP = SPWMVa * pD->PwmPeriod >> 13;
	pD->PwmV_CMP = SPWMVb * pD->PwmPeriod >> 13;
	pD->PwmW_CMP = SPWMVc * pD->PwmPeriod >> 13;
	// ��ֹ�����쳣
	pD->PwmU_CMP = LIMITMAXMIN(pD->PwmU_CMP,pD->PwmPeriod - 1,0);
	pD->PwmV_CMP = LIMITMAXMIN(pD->PwmV_CMP,pD->PwmPeriod - 1,0);
	pD->PwmW_CMP = LIMITMAXMIN(pD->PwmW_CMP,pD->PwmPeriod - 1,0);

	/************�Ƚϸ���*************/
	EPwm1Regs.CMPA.half.CMPA = pD->PwmU_CMP+TdA;
	EPwm2Regs.CMPA.half.CMPA = pD->PwmV_CMP+TdB;
	EPwm3Regs.CMPA.half.CMPA = pD->PwmW_CMP+TdC;

	if(m1CurrLoopCtler.MotCtl.bit.Start)
	{
		DSPCore_SwitchOnPWM();
	}
	else
	{
		DSPCore_SwitchOffPWM();
	}

	return TRUE;
}

