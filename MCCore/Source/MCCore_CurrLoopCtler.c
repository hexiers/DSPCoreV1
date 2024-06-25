/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	MCCore DSP执行电流环运算
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-06                                                                 
* 修改记录		:   
*            
======================================================================================================*/


#include "MCCore_CurrLoopCtler.h"
#include "math.h"
/**************************************************************************************************************************************************
* @函数名		:  		
* @函数描述	: 		
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-09-27                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
void DSPCore_SwitchOffPWM(void)
{

	EPwm1Regs.AQCSFRC.bit.CSFA = 1;//强制输出低
	EPwm1Regs.AQCSFRC.bit.CSFB = 2;//强制高，经DB反向后输出低
	EPwm2Regs.AQCSFRC.bit.CSFA = 1;//强制输出低
	EPwm2Regs.AQCSFRC.bit.CSFB = 2;//强制高，经DB反向后输出低
	EPwm3Regs.AQCSFRC.bit.CSFA = 1;//强制输出低
	EPwm3Regs.AQCSFRC.bit.CSFB = 2;//强制高，经DB反向后输出低
}
/**************************************************************************************************************************************************
* @函数名		:  		
* @函数描述	: 		
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-09-27                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
void DSPCore_SwitchOnPWM(void)
{
	EPwm1Regs.AQCSFRC.bit.CSFA = 0;//强制输出无效
	EPwm1Regs.AQCSFRC.bit.CSFB = 0;//强制输出无效
	EPwm2Regs.AQCSFRC.bit.CSFA = 0;//强制输出无效
	EPwm2Regs.AQCSFRC.bit.CSFB = 0;//强制输出无效
	EPwm3Regs.AQCSFRC.bit.CSFA = 0;//强制输出无效
	EPwm3Regs.AQCSFRC.bit.CSFB = 0;//强制输出无效
}
/**************************************************************************************************************************************************
* @函数名		:  		短路故障、过流故障
* @函数描述	: 		
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-09-27                                                             
* 修改记录		:   			
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
	else/* 如果IO电平没有检测到故障，则认为发生短路(故障脉冲短)*/
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
* @函数名		:  		
* @函数描述	: 不同于原EPS的发波逻辑，为了使得 比较值越大，占空比越宽	，
							调整了动作模块，调整在PWM计数为0时，触发ADC		
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-09-27                                                             
* 修改记录		:   			
*                           
****************************************************************************************************************************************************/
void DSPCore_InitEPwm(volatile struct EPWM_REGS*p,Uint16 PwmPeriod,Uint16 DeadTime)
{
	//======================================================================
	//===============时基相关
	//时基周期
	p->TBPRD = PwmPeriod;
	//时基偏移
	p->TBPHS.half.TBPHS =0;
	//时基计数值
	p->TBCTR=0;
	p->TBCTL.bit.FREE_SOFT = 2;//仿真模式时基自由运行
	p->TBCTL.bit.PHSDIR = 1;//同步信号到来后，计数器装载后向上计数
	p->TBCTL.bit.CLKDIV = 1;//PWM周期60MHz DSPCore
	p->TBCTL.bit.HSPCLKDIV =0;
	p->TBCTL.bit.SYNCOSEL = 1;//同步信号输出选择ctr = 0
	p->TBCTL.bit.PRDLD = 0;//周期寄存器加载使用影子寄存器
	p->TBCTL.bit.PHSEN = 1;//当同步信号到来时使用相位寄存器作为计数寄存器的加载值
	p->TBCTL.bit.CTRMODE = 2;//向上向下计数模式
	//======================================================================
	//===============计数比较
	p->CMPA.half.CMPA = PwmPeriod;//默认输出0占空比
	p->CMPB = PwmPeriod;//默认输出0占空比
	p->CMPCTL.bit.LOADAMODE = 2;//周期匹配或者0均加载
	p->CMPCTL.bit.LOADBMODE = 2;//周期匹配或者0均加载
	p->CMPCTL.bit.SHDWAMODE = 0;//比较寄存器使用影子寄存器
	p->CMPCTL.bit.SHDWBMODE = 0;//比较寄存器使用影子寄存器
	//======================================================================
	//===============动作模块
	p->AQCTLA.bit.CAU = 1;//向上计数匹配时输出低DSPCore
	p->AQCTLA.bit.CAD = 2;//向下计数匹配时输出高DSPCore
	p->AQCTLA.bit.ZRO = 0;
	p->AQCTLA.bit.PRD = 0;
	p->AQCTLA.bit.CBU = 0;
	p->AQCTLA.bit.CBD = 0;
	p->AQCTLB.bit.CAU = 1;//向上计数匹配时输出低DSPCore
	p->AQCTLB.bit.CAD = 2;//向下计数匹配时输出高DSPCore
	p->AQCTLB.bit.ZRO = 0;
	p->AQCTLB.bit.PRD = 0;
	p->AQCTLB.bit.CBD = 0;
	p->AQCTLB.bit.CBU = 0;

	p->AQSFRC.bit.RLDCSF = 2;//周期匹配或者0均加载 AQCSFRC
	//连续软件强制
	DSPCore_SwitchOffPWM();
	//======================================================================
	//===============死区控制
	p->DBCTL.bit.IN_MODE = 2;//PWMA、PWMB独立作为DB输入
	p->DBCTL.bit.POLSEL = 2;//PWM输出A同向，B反向
	p->DBCTL.bit.OUT_MODE = 3;//使用前后死区
	p->DBRED = DeadTime;
	p->DBFED = DeadTime;
	//====================s==================================================
	//===============斩波功能
	p->PCCTL.bit.CHPEN = 0;//不适用斩波功能
	//======================================================================
	//===============错误联防
	p->TZSEL.bit.OSHT1 = 1;//使用TZ1的单次错误联防输入
	p->TZSEL.bit.OSHT2 = 1;//使用TZ2的单次错误联防输入
	p->TZCTL.bit.TZA = 0;//错误事件发生时，输出高阻
	p->TZCTL.bit.TZB = 0;
	p->TZEINT.bit.OST = 0;//不使能单次错误中断
	p->TZCLR.bit.OST = 1;//清除单次错误中断
	//======================================================================
	//===============事件触发
	p->ETSEL.bit.SOCAEN = 1;//使能soca信号产生
	p->ETSEL.bit.SOCASEL = 1;//时基为0时，产生ad启动信号 DSPCore
	p->ETPS.bit.SOCAPRD = 1;//产生一次事件便触发ad启动信号
}


/**************************************************************************************************************************************************
* @函数名		:  		DSPCore_CfgCurrLoopCtler                                                        
* @函数描述	: 		将CurrLoopCtler中配置相关的参数更新到DSP 的PWM模块中去
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-06                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_CfgCurrLoopCtler(stCurrLoopCtler* pD)
{
	EALLOW;
	DSPCore_InitEPwm(&EPwm1Regs,pD->PwmPeriod,pD->DeadTime);
	DSPCore_InitEPwm(&EPwm2Regs,pD->PwmPeriod,pD->DeadTime);
	DSPCore_InitEPwm(&EPwm3Regs,pD->PwmPeriod,pD->DeadTime);
	EPwm1Regs.TZEINT.bit.OST = 1;//使能PWM1的单次错误中断
	EDIS;
	return TRUE;
}
/**************************************************************************************************************************************************
* @函数名		:  		DSPCore_DoCurrLoopCtrl
* @函数描述	: 		执行电流环计算
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-09-27                                                             
* 修改记录		:   			
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
	/************获取角度*************/
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
	
	/************偏置消除*************/
	pD->IOriU= GetADResultModifyDef(AD_MAIN_CURR_U,m1HalPara.ADGain,m1HalPara.ADOff)  << 2;// 归一化到[-8192,8191]
	IFdbU = pD->MotCtl.bit.EnUCurrNeg?(pD->IOffU - pD->IOriU):(pD->IOriU -pD->IOffU);
	// V相配置在EPS中用作W相
	pD->IOriW= GetADResultModifyDef(AD_MAIN_CURR_W,m1HalPara.ADGain,m1HalPara.ADOff)  << 2;// 归一化到[-8192,8191]
	IFdbW = pD->MotCtl.bit.EnVCurrNeg?(pD->IOffW - pD->IOriW):(pD->IOriW -pD->IOffW);
	
	/************Clark正变换*************/
	pD->IFdbAlpha = IFdbU;
	pD->IFdbBeta = ((INT32S)(-IFdbU - 2* IFdbW))*18919L >> 15;

	/************Park正变换*************/
	iqTheta = (INT64S)pD->ThetaPark * PIQ16*2 >> 16; // 输出Q16格式的iqTheta
	iqCosTheta = _IQ16cos(iqTheta);	 
	iqSinTheta = _IQ16sin(iqTheta);
	pD->IdFdb =  ((INT32S)pD->IFdbAlpha*iqCosTheta + (INT32S)pD->IFdbBeta*iqSinTheta)>>16;
	pD->IqFdb =  (-(INT32S)pD->IFdbAlpha*iqSinTheta + (INT32S)pD->IFdbBeta*iqCosTheta)>>16;

	/************PI计算*************/
	pD->DeltaD = pD->IdCmd - pD->IdFdb;
	pD->IdProp =  LIMITMAXMIN(((INT32S)pD->DeltaD * pD->IdKp)>>7,8191,-8192);
	pD->IdIntergLim32 = (INT32S)pD->DeltaD * pD->IdKi
									+ ((INT32S)pD->DisChageD << 7)* pD->MotCtl.bit.CurrAntiRstEn
									+ pD->IdIntergLim32;
	pD->IdIntergLim32 = LIMITMAXMIN(pD->IdIntergLim32, 1048575, -1048576);// 限制到[-2^20,2^20]
	if((pD->IdKi == 0)||(pD->MotCtl.bit.Start == 0))/* 非控制状态，积分输出清0*/
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
	if((pD->IqKi == 0)||(pD->MotCtl.bit.Start == 0))/* 非控制状态，积分输出清0*/
	{
		pD->IqIntergLim32 = 0;
	}
	pD->IqIntergLim32 = LIMITMAXMIN(pD->IqIntergLim32, 1048575, -1048576);// 限制到[-2^20,2^20]
	pD->IqIntergLim = pD->IqIntergLim32 >> 7;
	UqPIOut_V = pD->IqProp + pD->IqIntergLim + pD->UqCmd_V;
	pD->UqPILim_V = LIMITMAXMIN(UqPIOut_V,8191,-8192);
	pD->UqOut_BITOri = LIMITMAXMIN((INT32S)pD->UqPILim_V * pBus->BusCoef >> 13,16383,-16384);
	pD->UqOut_BIT = LIMITMAXMIN(pD->UqOut_BITOri,Uq_BIT_Max,-Uq_BIT_Max);
	pD->UqActOut_V = (INT32S)pD->UqOut_BIT * pBus->VBusProtect >> 13;
	pD->DisChageQ = pD->UqActOut_V - UqPIOut_V;
	
	/************Park逆变换*************/
	pD->ThetaInvPark = pD->ThetaPark + (pD->PosModify>>3);
	iqTheta = (INT64S)pD->ThetaInvPark * PIQ16*2 >> 16; // 输出Q16格式的iqTheta
	iqCosTheta = _IQ16cos(iqTheta);	 
	iqSinTheta = _IQ16sin(iqTheta);
	pD->UAlpha_BIT = ((INT32S)pD->UdOut_BIT * iqCosTheta - (INT32S)pD->UqOut_BIT * iqSinTheta)>>16;
	pD->UBeta_BIT = ((INT32S)pD->UdOut_BIT * iqSinTheta + (INT32S)pD->UqOut_BIT * iqCosTheta)>>16;
	// 供SMC用
	pD->UAlpha_V = (INT32S)pD->UAlpha_BIT * pBus->VBusProtect >> 13;
	pD->UBeta_V = (INT32S)pD->UBeta_BIT  * pBus->VBusProtect >> 13;
	
	/************Clark逆变换*************/
	SPWMVa = pD->UAlpha_BIT;
	SPWMVb = -(pD->UAlpha_BIT >> 1) + ((INT32S)pD->UBeta_BIT * 28378L >> 15);
	SPWMVc = -(pD->UAlpha_BIT >> 1) - ((INT32S)pD->UBeta_BIT * 28378L >> 15);
	SPWMVb = LIMITMAXMIN(SPWMVb, 8191, -8192);
	SPWMVc = LIMITMAXMIN(SPWMVc, 8191, -8192);

	SPWMVa = SPWMVa * 18918L >>15;
	SPWMVb = SPWMVb * 18918L >>15;
	SPWMVc = SPWMVc * 18918L >>15;

	/************SVPWM调制*************/
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

	//********** 死区补偿*********************
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

	/************  周期解耦   *************/
	pD->PwmU_CMP = SPWMVa * pD->PwmPeriod >> 13;
	pD->PwmV_CMP = SPWMVb * pD->PwmPeriod >> 13;
	pD->PwmW_CMP = SPWMVc * pD->PwmPeriod >> 13;
	// 防止发波异常
	pD->PwmU_CMP = LIMITMAXMIN(pD->PwmU_CMP,pD->PwmPeriod - 1,0);
	pD->PwmV_CMP = LIMITMAXMIN(pD->PwmV_CMP,pD->PwmPeriod - 1,0);
	pD->PwmW_CMP = LIMITMAXMIN(pD->PwmW_CMP,pD->PwmPeriod - 1,0);

	/************比较更新*************/
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

