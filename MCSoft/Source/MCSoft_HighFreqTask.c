/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义
* @创建人		:  	张宇                                                                              
* 创建日期	:   	2020-03-07
* 修改记录	:
*	 20200911  :   针对高电气转速，作相应修改
            		:   增加变增益功能
            20201012:增加MC_AdjMaxCurrLim和速度反馈修正，但控制时未用
                 
======================================================================================================*/
#include "MC_Include.h"

#pragma CODE_SECTION(MC_HighFreqTask,"ramfuncs");
#pragma CODE_SECTION(MC_msTimeBaseGenerate,"ramfuncs");
#pragma CODE_SECTION(DebugDataPrint,"ramfuncs");
#pragma CODE_SECTION(MC_VarSpdLoopGain,"ramfuncs");

#pragma CODE_SECTION(DSPCore_DoResolverDealModule,"ramfuncs");
#pragma CODE_SECTION(DSPCore_DoCurrLoopCtrl,"ramfuncs");
#pragma CODE_SECTION(DSPCore_DoSpdLoopCtrl,"ramfuncs");
#pragma CODE_SECTION(FeedUsrSmc,"ramfuncs");
#pragma CODE_SECTION(SMC_Position_Estimation,"ramfuncs");
#pragma CODE_SECTION(DSPCore_DoFaultDealModule,"ramfuncs");
#pragma CODE_SECTION(UpDateCriticalSampleData_HighFreq,"ramfuncs");
#pragma CODE_SECTION(DSPCore_DoFwCtrl,"ramfuncs");


#define PCross(New,Old) (((New)>=0)&&((Old) < 0))?1:0
#define NCross(New,Old) (((New)<=0)&&((Old) > 0))?1:0
//#define Round(x,y,z) (((x)+(z)-(y) >= (z))?((x)-(y)):((x)+(z)-(y)))

//INT16S TestPosErr = 0;
stTimeBase m1TimeBase;
stPosCalib m1PosCalib; 
extern Uint16 SetTestData(Uint16 Data);
extern Uint16 MC_DoStartUp(stStartup* pStartup,stSysState* pState);
extern bool DoPosCalib2(stPosCalib* p);
extern void UpDateCriticalSampleData_HighFreq(stCriticalPara* pC);
void DebugDataPrint(Uint16 TmpDataSel);
extern   bool DSPCore_DoCurrLoopCtrl(stCurrLoopCtler* pD,stResolverDealModule* pR,stSmcObserverExt* pS,stBusVolDecoupler* pBus);
extern bool DSPCore_DoFaultDealModule(stFaultDealModule* pF,stCurrLoopCtler*pD,stBusVolDecoupler* pBus,stResolverDealModule*pR);
extern Uint16 MC_DoStartUpCalib(stStartup* pStartup,stSysState* pState);
INT16S OBS1=0,OBS2=0,OBS3=0,DELTA=0;
float gama=1.4,STATUS=0;
INT32S Idcmd = 0,Iqcmd = 0;
/**************************************************************************************************************************************************
* @函数名		:  		MC_VarSpdLoopGain                                                        
* @函数描述	    : 		根据转速的误差，调整比例和积分增益,并及时刷新至FPGA和CPU的控制参数中
* @创建人		:    	张宇
* 创建日期	    :   	2020-09-11
* 修改记录	    :
*                         
****************************************************************************************************************************************************/
bool MC_VarSpdLoopGain(stSpdLoopCtler* pFPGA,INT32S SpdErr,stCtrlPara* pCtrl)
{
    static float k = 0.0f;
    static Uint16 FirstCallFlag = 0U;
    float fGain = 1.0f;
    if(FirstCallFlag == 0)
    {
        /* 首次调用*/
        FirstCallFlag = 1;
        if ((pCtrl->SpdVaryKpEndErr_rpm <= pCtrl->SpdVaryKpStartErr_rpm)\
           ||(pCtrl->SpdVaryKpGain <= 1))
        {
            k = 0.0f;
            MC_PostCodeWarn(2);
            return FALSE;
        }
        else
        {
            /* 计算每rpm转速对应的控制增量*/
            k = ((float)pCtrl->SpdVaryKpGain-1)/(float)(pCtrl->SpdVaryKpEndErr_rpm - pCtrl->SpdVaryKpStartErr_rpm);
        }     
    }
    else
    {
        fGain = LIMITMAXMIN(ABS(SpdErr),pCtrl->SpdVaryKpEndErr_rpm,pCtrl->SpdVaryKpStartErr_rpm);
        fGain -= pCtrl->SpdVaryKpStartErr_rpm;
        fGain *= k;
        fGain += 1.0f;
    }
    

	pFPGA->SpdKp = pCtrl->SpdInitKp * fGain;
	pFPGA->SpdKi = pCtrl->SpdKi * fGain;

    return TRUE;
}

/**************************************************************************************************************************************************
* @函数名		:  		MC_msTimeBaseGenerate                                                        
* @函数描述	: 		时基变量初始化函数
* @创建人		:    	张宇
* 创建日期	:   	2020-03-14
* 修改记录	:
*                         
****************************************************************************************************************************************************/
void MC_InitBaseGenerate(stTimeBase* pT,stCtrlPara* pCtrl)
{
	pT->Days = 0;
	pT->Hours = 0;/* 每ms中断数*/
	pT->Mins = 0;
	pT->Secs = 0;
	pT->ms = 0;
	pT->Every1msSet = FALSE;
	pT->Every10msSet = FALSE;
	pT->Every50msSet = FALSE;
	pT->Every500msSet = FALSE;
	pT->IntNums = 0;
	pT->IntsPerms = (PWMCLKFREQ>>1)/pCtrl->PwmPeriod/1000;

}
/**************************************************************************************************************************************************
* @函数名		:  		MC_msTimeBaseGenerate                                                        
* @函数描述	: 		产生1ms的定时时基，由高频任务调用
* @创建人		:    	张宇
* 创建日期	:   	2020-03-14
* 修改记录	:
*                         
****************************************************************************************************************************************************/
void MC_msTimeBaseGenerate(stTimeBase* pT)
{
	pT->IntNums++;
	if(pT->IntNums >= pT->IntsPerms)
	{
		pT->IntNums = 0;
		pT->Every1msSet = TRUE;
		pT->Every1msPoint = TRUE;
	}
	else
	{
	    pT->Every1msPoint = FALSE;
	}
}
/**************************************************************************************************************************************************
* @函数名		:  		MC_OtherTimeBaseGenerate                                                        
* @函数描述	: 		产生1ms以外的定时时基，由中频1ms任务调用
* @创建人		:    	张宇
* 创建日期	:   	2020-03-14
* 修改记录	:
*                         
****************************************************************************************************************************************************/
void MC_OtherTimeBaseGenerate(stTimeBase* pT)
{
	/* ms信号0~999循环计数,并每秒累加Secs*/
	pT->ms++;	
	if(pT->ms >= 1000)
	{
		pT->ms = 0;
		pT->Secs++;
	}
	/* 产生天、时、分、秒*/
	if(pT->Secs >= 60)
	{
		pT->Secs = 0;
		pT->Mins ++;
		if(pT->Mins >= 60)
		{
			pT->Mins = 0;
			pT->Hours ++;
			if (pT->Hours > 24)
			{
				pT->Hours = 0;
				pT->Days ++;
			}
		}
	}
	/* 每10ms送出信号*/
	if((pT->ms % 10) == 1)
	{
		pT->Every10msSet = TRUE;
	}
	else
	{

	}
	/* 每50ms送出信号*/
	if((pT->ms % 50) == 2)
	{
		pT->Every50msSet = TRUE;
	}
	else
	{

	}
	/* 每500ms送出信号*/
	if((pT->ms % 500) == 3)
	{
		pT->Every500msSet = TRUE;
	}
	else
	{

	}
}

/**************************************************************************************************************************************************
* @函数名		:  		MC_HighFreqTask                                                        
* @函数描述	: 		PWM停止的时候，给观测器喂数据，必须在电流环中断中调用
* @创建人		:    	张宇
* 创建日期	:   	2020-03-07
* 修改记录	:
*                         
****************************************************************************************************************************************************/

void inline FeedUsrSmc(void)
{
	INT32S Tmp1 = 0;
	INT32S Tmp2 = 0;
	static INT16S UAlpha_V_Old = 0;
	static INT16S UBeta_V_Old = 0;

	if(m1State.MainState == STS_Idle)
	{
		Tmp1 = GetADResultModifyDef(AD_BEMF_UV,m1HalPara.ADGain,m1HalPara.ADOff)  << 2;
		Tmp2 = GetADResultModifyDef(AD_BEMF_VW,m1HalPara.ADGain,m1HalPara.ADOff)  << 2;
		m1CurrLoopCtler.MotCtl.bit.UserSmcIn = 1;
		m1SmcObserverExt.UAlpha_UseQ16 = ((Tmp1 - m1HalPara.UVBemfOff)*2 +(Tmp2 - m1HalPara.VWBemfOff))*21845L ;
		m1SmcObserverExt.UBeta_UseQ16 = (Tmp2 - m1HalPara.VWBemfOff)*37834L ;
		/* 量纲可能不正确,但电流为0，不影响角度判断*/
		m1SmcObserverExt.IAlpha_UseQ16 	= (INT32S)m1CurrLoopCtler.IFdbAlpha << 16;
		m1SmcObserverExt.IBeta_UseQ16 	= (INT32S)m1CurrLoopCtler.IFdbBeta << 16;
	}
	else
	{
	    	/* 准备数据*/	
	    	m1SmcObserverExt.UAlpha_UseQ16	= (INT32S)UAlpha_V_Old << 16;
	    	m1SmcObserverExt.UBeta_UseQ16 	= (INT32S)UBeta_V_Old << 16;	
	    	m1SmcObserverExt.IAlpha_UseQ16 	= (INT32S)m1CurrLoopCtler.IFdbAlpha << 16;
	    	m1SmcObserverExt.IBeta_UseQ16 	= (INT32S)m1CurrLoopCtler.IFdbBeta << 16;
	    	/* 滞后一个周期*/
	    	UAlpha_V_Old = m1CurrLoopCtler.UAlpha_V;
	    	UBeta_V_Old = m1CurrLoopCtler.UBeta_V;
    }

//	m1FluxObserver.Udaxis=(float)m1CurrLoopCtler.UdOut_BIT/16383.0f * m1FluxObserver.VDC_V * 0.57735027;// [-16384,16383]对应后面VDC/SQRT(3)
//	m1FluxObserver.Uqaxis=(float)m1CurrLoopCtler.UqOut_BIT/16383.0f * m1FluxObserver.VDC_V * 0.57735027;
//	m1FluxObserver.IdFbk=(float)m1CriPara.IdFdb*0.01;//定点  单位 10mA
//	m1FluxObserver.IqFbk=(float)m1CriPara.IqFdb*0.01;

//	m1CurrLoopCtler.ThetaPark// [0,65535]对应电气角度【0,2pi】

}

/**************************************************************************************************************************************************
* @函数名		:  		MC_AdjMaxCurrLim                                                        
* @函数描述		: 		速度环输出最大限制的调整，防止发电
* @创建人		:    		张宇
* 创建日期		:   		2020-10-12
* 修改记录		:
*                         
****************************************************************************************************************************************************/
static inline void MC_AdjMaxCurrLim(void)
{
	INT16S TmpMax = 0;
	INT16S TmpMin = 0;

	/* 防止发电逻辑*/
	if((m1CriPara.SpdFdb > m1CtrlPara.PGenLimSpd_rpm)&&(m1CriPara.HpVol > m1CtrlPara.PGenLimVol_10mV))
	{
		TmpMax = (INT32S)((Uint32)m1HalPara.F10mAToBitCoef * (INT32S)m1MotorPara.RatedCurrentQ_10mA)>>12;
		TmpMin = (INT32S)((Uint32)m1HalPara.F10mAToBitCoef * (INT32S)m1CtrlPara.PGenLimCurr_10mA)>>12;
	}
	else if((m1CriPara.SpdFdb < -m1CtrlPara.PGenLimSpd_rpm)&&(m1CriPara.HpVol > m1CtrlPara.PGenLimVol_10mV))
	{
		TmpMax = -(INT32S)((Uint32)m1HalPara.F10mAToBitCoef * (INT32S)m1CtrlPara.PGenLimCurr_10mA)>>12;
		TmpMin = -(INT32S)((Uint32)m1HalPara.F10mAToBitCoef * (INT32S)m1MotorPara.RatedCurrentQ_10mA)>>12;
	}
	else
	{
		TmpMax = (INT32S)((Uint32)m1HalPara.F10mAToBitCoef * (INT32S)m1MotorPara.RatedCurrentQ_10mA)>>12;
		TmpMin = -(INT32S)((Uint32)m1HalPara.F10mAToBitCoef * (INT32S)m1MotorPara.RatedCurrentQ_10mA)>>12;
	}

	/* FPGA执行速度控制*/
	m1SpdLoopCtler.SpdOutMax = TmpMax;
	m1SpdLoopCtler.SpdOutMin = TmpMin;
}
/**************************************************************************************************************************************************
* @函数名		:  		MC_HighFreqTask                                                        
* @函数描述	: 		高频实时控制任务，必须在电流环中断中调用
* @创建人		:    	张宇
* 创建日期	:   	2020-03-07
* 修改记录	:
*                         
****************************************************************************************************************************************************/
void MC_HighFreqTask(void)
{
	INT16S  TmpPosModify = 0;
	//INT16S  Tmp1 = 0;
	//INT16S  Tmp2 = 0;
	INT16S TmpPoles = 0;
	//INT32S iError = 0;
	GpioDataRegs.GPBDAT.bit.GPIO54 = 1;

	MC_msTimeBaseGenerate(&m1TimeBase);

	m1CriPara.HpVol  = (INT32S)((Uint32)m1BusVolDecoupler.V_Bus*(Uint32)m1HalPara.BitTo10mVCoef)>>12;
	switch(m1State.MainState)
	{
	case STS_PwrUpWait:
		m1CurrLoopCtler.MotCtl.bit.Start = 0;
		break;

	case STS_BridgeCheck:
		m1CurrLoopCtler.MotCtl.bit.Start = 0;
		break;

	case STS_Idle:
		m1CurrLoopCtler.MotCtl.bit.Start = 0;
		m1CurrLoopCtler.IdCmd = 0;
		m1CurrLoopCtler.IqCmd = 0;
		m1SpdLoopCtler.SpdElecCmd = 0;
		m1CriPara.SpdRefSlp = 0;
		m1Hfi.IntergTermQ16 = 0;
		m1Hfi.HfiPosQ16 = 0;
		m1Hfi.HfiSpdQ16 = 0;

		m1FluxObserver.FluxEst_Angle=0;//(float)((m1ResolverDealModule.ResolverElecPos-m1CurrLoopCtler.PosOff)>>4)/4096*2*PI;
		m1FluxObserver.FluxEst_ElecSpd=0;

		STATUS=0;
		m1FluxObserver.Selfsensing_Enable=0;

		//MC_eStartInit(&m1eStart,(128L<<8));
		break;

	case STS_StartUp:
		m1CurrLoopCtler.MotCtl.bit.UserSmcIn = 0;
		/*  预定位  I/F启动*/
		if(m1State.SubState > 0)
		{
			m1CurrLoopCtler.ManuPos = MC_DoStartUp(&m1StartUp,&m1State);
			m1CurrLoopCtler.MotCtl.bit.EnSpdCtrl = 0;
			m1CurrLoopCtler.MotCtl.bit.Start = 1;
			m1CurrLoopCtler.MotCtl.bit.PosSel = 1;/* 选择ManuPos，启动位置序列*/
			m1CurrLoopCtler.IdCmd = 0;
			m1CurrLoopCtler.IqCmd = (INT32S)((INT32S)m1SnsLessPara.StartUpCurr_10mA * (Uint32)m1HalPara.F10mAToBitCoef)>>12;
			/* 预设速度环积分器电流对应开环启动电流*/
			m1SpdLoopCtler.SpdIntergPreset = (INT32S)((INT32S)m1SnsLessPara.StartUpCurr_10mA * (Uint32)m1HalPara.F10mAToBitCoef)>>12;
			/* 开环启动过程将斜坡限制值固定在启动转速*/
			m1CriPara.SpdRefSlp = m1SnsLessPara.StartUpSpd;
			m1SpdLoopCtler.SpdElecCmd = (INT32S)m1CriPara.SpdRefSlp * m1MotorPara.MotorPoles >> SPDFDB_SCALE_FACTOR_POW;
		}
		/* 在中速任务中执行启动参数的初始化*/
		else
		{}
		break;

	case STS_Run:		    
		m1CurrLoopCtler.MotCtl.bit.UserSmcIn = 0;
		STATUS=1;
		/* 计算电机电气rpm*/
		TmpPosModify =0/* m1CriPara.SpdFdb * m1MotorPara.MotorPoles*/;
		/* 计算每周期位移增量,360度电气角归一化成65536*/
		// TmpPosModify = (INT64S)TmpPosModify * 156375 * m1CtrlPara.PwmPeriod>>32;
		/* 供电流环反变换用*/
		m1CurrLoopCtler.PosModify = (INT16S)((INT64S)TmpPosModify * 1251000 * m1CtrlPara.PwmPeriod>>32);

		m1CurrLoopCtler.MotCtl.bit.EnSpdCtrl = (m1CriPara.CtrlMode == MODE_CURR)?0:1;/* 电流模式或者速度模式*/

		if(m1SysCfgPara.SysCfgWord1.bit.EnSnsLess)/* 无感控制、FPGA观测器位置*/
		{
			//m1CurrLoopCtler.MotCtl.bit.PosSel = 2;
			m1CurrLoopCtler.MotCtl.bit.PosSel = 3;
		}
		else/* 有感控制，传感器位置*/
		{
			m1FluxObserver.Selfsensing_Enable=1;

		}			
		m1CurrLoopCtler.MotCtl.bit.Start = 1;/* 使能输出*/
		if(m1CriPara.CtrlMode == MODE_CURR)
		{
			m1CurrLoopCtler.IqCmd = (INT32S)((INT32S)m1CriPara.IqRef10mA * (Uint32)m1HalPara.F10mAToBitCoef) >>12;/* 更新FPGA电流指令*/
			m1FluxObserver.Selfsensing_Enable=1;
			if(fabs(m1FluxObserver.Ave_we)>m1FluxObserver.PAR_Wlim)
			m1CurrLoopCtler.IdCmd = 0;//(INT32S)((INT32S)m1CriPara.IqRef10mA/gama/(10+m1SpdLoopCtler.SpdFdbElecRpm)* (Uint32)m1HalPara.F10mAToBitCoef)>>12;/*(1+m1FluxObserver.FluxEst_ElecSpd)*/
			else
			m1CurrLoopCtler.IdCmd=(INT32S)(fabs((INT32S)m1CriPara.IqRef10mA >>2) * (Uint32)m1HalPara.F10mAToBitCoef)>>12;
		}

		else// MODE_SPD
		{			
			/*  速度模式下，不管IdRef10mA、IqRef10mA的更新*/
			 m1CurrLoopCtler.IqCmd = m1SpdLoopCtler.SpdLimOut ;
			// m1CurrLoopCtler.IdCmd = (INT32S)((INT32S)m1CriPara.IqRef10mA/gama/(10+m1SpdLoopCtler.SpdFdbElecRpm)*m1FluxObserver.PAR_MotDir* (Uint32)m1HalPara.F10mAToBitCoef)>>12;
			 /*if(fabs(m1FluxObserver.FluxEst_ElecSpd)<m1FluxObserver.PAR_Wlim)
			 m1CurrLoopCtler.IdCmd = m1SpdLoopCtler.SpdLimOut/gama*m1FluxObserver.PAR_MotDir;//(INT32S)((INT32S)m1FwCtler.FwLimOut * (Uint32)m1HalPara.F10mAToBitCoef) >>12;
			 else
			 m1CurrLoopCtler.IdCmd = 0;*/
			 if(m1TimeBase.Every1msPoint)
			{

				/* 根据有无传感器，选择计算机械转速的极对数*/
				TmpPoles = m1SysCfgPara.SysCfgWord1.bit.EnSnsLess?m1MotorPara.MotorPoles:m1MotorPara.SnsorPoles;
				
				/* 执行机械转速指令的斜坡限制*/
				m1CriPara.SpdRefSlp = SLOPELIMIT(m1CriPara.SpdRef, m1CriPara.SpdRefSlp, (INT16S)m1CtrlPara.SpdSlpOnems, -(INT16S)m1CtrlPara.SpdSlpOnems);
				/* 将斜坡限制后的机械转速转换成带系数的电气转速*/
				m1SpdLoopCtler.SpdElecCmd  = (INT32S)m1CriPara.SpdRefSlp * TmpPoles>> SPDFDB_SCALE_FACTOR_POW;

				//MC_VarSpdLoopGain(&m1SpdLoopCtler,m1CriPara.SpdRefSlp - m1CriPara.SpdFdb,&m1CtrlPara);
				//DSPCore_DoSpdLoopCtrl(&m1SpdLoopCtler,&m1ResolverDealModule,&m1SmcObserverExt)

				MC_AdjMaxCurrLim();
			}    
		}			
		break;

	case STS_MainTenance:
		if(m1State.SubState == 5)/* 标定初始角*/
		{
			m1CurrLoopCtler.MotCtl.bit.EnSpdCtrl = 0;/* 选择电流控制模式*/
			m1CurrLoopCtler.MotCtl.bit.PosSel = 1; /* 选择ManuPos,设定0位置*/
			m1CurrLoopCtler.ManuPos = 0;
			m1CurrLoopCtler.IdCmd = (INT32S)((INT32S)m1CriPara.IdRef10mA * (Uint32)m1HalPara.F10mAToBitCoef) >>12;/* 更新FPGA电流指令*/
			m1CurrLoopCtler.IqCmd = 0;
			m1CurrLoopCtler.MotCtl.bit.Start = 1;/* 使能输出*/
			m1FluxObserver.Selfsensing_Enable=0;
		}
		else if(m1State.SubState == 7)/* 速度反馈校正*/
		{
			m1CurrLoopCtler.MotCtl.bit.EnSpdCtrl = 1;/* 选择速度控制模式*/
			m1CurrLoopCtler.MotCtl.bit.PosSel = 0; /* 选择传感器反馈*/
			 m1CurrLoopCtler.IqCmd = m1SpdLoopCtler.SpdLimOut ;/* 执行速度控制*/
			m1CurrLoopCtler.IdCmd = 0;			
			m1CurrLoopCtler.MotCtl.bit.Start = 1;/* 使能输出*/
			/* 执行机械转速指令的斜坡限制*/
			m1CriPara.SpdRefSlp = SLOPELIMIT(m1CriPara.SpdRef, m1CriPara.SpdRefSlp, (INT16S)m1CtrlPara.SpdSlpOnems, -(INT16S)m1CtrlPara.SpdSlpOnems);
			/* 将斜坡限制后的机械转速转换成带系数的电气转速*/
			m1SpdLoopCtler.SpdElecCmd  = (INT32S)m1CriPara.SpdRefSlp * m1MotorPara.MotorPoles>> SPDFDB_SCALE_FACTOR_POW;
			MC_AdjMaxCurrLim();
			/* 按电周期循环存储修正值*/
			m1SpdFdbPara.ModifyCoef[m1ResolverDealModule.ResolverElecPos >> 10 ] = LIMITMAXMIN(m1SpdLoopCtler.SpdElecCmd*8192L/m1ResolverDealModule.ResolverElecRpm,8192+100,8192-100);
		}
		else if(m1State.SubState == 9)/* 整定电阻、电感*/
		{
			m1CurrLoopCtler.MotCtl.bit.EnSpdCtrl = 0;/* 选择电流控制模式*/
			m1CurrLoopCtler.MotCtl.bit.PosSel = 1; /* 选择ManuPos,设定0位置*/
			m1CurrLoopCtler.ManuPos = 0;
			m1CurrLoopCtler.IdCmd = (INT32S)((INT32S)m1CriPara.IdRef10mA * (Uint32)m1HalPara.F10mAToBitCoef) >>12;/* 更新FPGA电流指令*/
			m1CurrLoopCtler.IqCmd = 0;
			if(m1State.StateWait < 4096)
			{
				m1CurrLoopCtler.MotCtl.bit.Start = 1;/* 使能输出*/
			}
			else
			{
				m1CurrLoopCtler.MotCtl.bit.Start = 0;/* 关闭输出*/
			}
			if(m1State.StateWait == 4096)
			{
				if(m1MotorCalib.CurrSampleCnt < 16)
				{
					m1MotorCalib.SampleCurr_10mA[m1MotorCalib.CurrSampleCnt++] = ((INT32S)m1CurrLoopCtler.IdFdb * m1HalPara.BitTo10mACoef)>>12;
				}
			}
			else
			{
				m1MotorCalib.CurrSampleCnt = 0;
			}
		}
		else if(m1State.SubState == 11)/* 整定HFI V1.8*/
		{
			m1CurrLoopCtler.MotCtl.bit.EnSpdCtrl = 0;/* 选择电流控制模式*/
			m1CurrLoopCtler.MotCtl.bit.PosSel = 1; /* 选择ManuPos,设定0位置*/
			m1CurrLoopCtler.ManuPos = 0;
			m1CurrLoopCtler.IdCmd = (INT32S)((INT32S)m1CriPara.IdRef10mA * (Uint32)m1HalPara.F10mAToBitCoef) >>12;/* 更新FPGA电流指令*/
			m1CurrLoopCtler.IqCmd = 0;
			m1CurrLoopCtler.MotCtl.bit.Start = 1;/* 使能输出*/
			m1CurrLoopCtler.UdCmd_V = MC_HfiCalc(&m1Hfi,(INT32S)((INT32S)m1CurrLoopCtler.IqFdb * (Uint32)m1HalPara.BitTo10mACoef) >>12);
		}
		/*  预定位 +  I/F启动 + 启动完成*/
		else if((m1State.SubState  == 14)||(m1State.SubState  == 15)||(m1State.SubState  == 16))
		{
			m1CurrLoopCtler.ManuPos = MC_DoStartUpCalib(&m1StartUp,&m1State);
			m1CurrLoopCtler.MotCtl.bit.EnSpdCtrl = 0;
			m1CurrLoopCtler.MotCtl.bit.Start = 1;
			m1CurrLoopCtler.MotCtl.bit.PosSel = 1;/* 选择ManuPos，启动位置序列*/
			m1CurrLoopCtler.IdCmd = 0;
			m1CurrLoopCtler.IqCmd = (INT32S)((INT32S)m1SnsLessPara.StartUpCurr_10mA * (Uint32)m1HalPara.F10mAToBitCoef)>>12;
			/* 预设速度环积分器电流对应开环启动电流*/
			//m1SpdLoopCtler.SpdIntergPreset = (INT32S)((INT32S)m1SnsLessPara.StartUpCurr_10mA * (Uint32)m1HalPara.F10mAToBitCoef)>>12;
			/* 开环启动过程将斜坡限制值固定在启动转速*/
			//m1CriPara.SpdRefSlp = m1SnsLessPara.StartUpSpd;
			//m1SpdLoopCtler.SpdElecCmd = (INT32S)m1CriPara.SpdRefSlp * m1MotorPara.MotorPoles >> SPDFDB_SCALE_FACTOR_POW;
		}
		else if(m1State.SubState  == 17)
		{	
			m1CurrLoopCtler.MotCtl.bit.Start = 0;
			DoPosCalib2(&m1PosCalib);
		}
		else
		{
			m1CurrLoopCtler.MotCtl.bit.Start = 0;/* 关闭输出*/
			m1CurrLoopCtler.UdCmd_V = 0;/* 考虑之前执行了高频注入*/
		}
		break;

	case STS_FaultNow:
		m1CurrLoopCtler.MotCtl.bit.Start = 0;
		break;	

	case STS_FaultOver:
		m1CurrLoopCtler.MotCtl.bit.Start = 0;
		break;

	default:
		break;


	}

//	OBS1=(INT16S)(Uint16)((m1FluxObserver.FluxEst_Angle/PI)*32768L)+m1FluxObserver.DEALTA;//;//m1FluxObserver.FluxEst_Angle/2/PI*4096;//
//	OBS2= m1ResolverDealModule.ResolverElecPos - m1CurrLoopCtler.PosOff;//(INT16S)m1CurrLoopCtler.ThetaPark;
//	OBS3=OBS1-OBS2;

	// =================软件Core开始===============
	DSPCore_DoResolverDealModule(&m1ResolverDealModule);
	DSPCore_DoCurrLoopCtrl(&m1CurrLoopCtler,&m1ResolverDealModule,&m1SmcObserverExt,&m1BusVolDecoupler);
	if(m1TimeBase.Every1msPoint)
	{
		DSPCore_DoSpdLoopCtrl(&m1SpdLoopCtler,&m1ResolverDealModule,&m1SmcObserverExt);
	}
	DSPCore_DoFwCtrl(&m1FwCtler,&m1CurrLoopCtler);
	FeedUsrSmc();
	SMC_Position_Estimation();



	FluxPosEst(&m1FluxObserver, &m1CurrLoopCtler);

	DSPCore_DoFaultDealModule(&m1FaultDealModule,&m1CurrLoopCtler,&m1BusVolDecoupler,&m1ResolverDealModule);
	// =================软件Core结束===============

	UpDateCriticalSampleData_HighFreq(&m1CriPara);
//	if(m1SysCfgPara.SysCfgWord1.bit.EnDacPrint)
//	{
//		DebugDataPrint(m1SysCfgPara.DA1Sel);
//	}

	/* 示波器采样 */
	Scope_Run();
			
}
bool InitPosCalib2(stPosCalib* p)
{
	MemClr((Uint16 *) p, (Uint16 *) p + sizeof(stPosCalib)/sizeof(Uint16));
	return TRUE;
}

bool DoPosCalib2(stPosCalib* p)
{
	volatile INT16S TempUV = 0;
	volatile INT16S TempVW = 0;
	static INT16S TempUVOld = 0;
	static INT16S TempVWOld = 0;
	INT16S Err1 = 0;
	INT16S Err2 = 0;
	INT16S Err3 = 0;

	//0-19周期
	if(p->BemfCnt < 20)
	{
		p->EnUVSampleBemfP = 0;
		p->EnVWSampleBemfP = 0;
		p->EnUVSampleBemfN = 0;
		p->EnVWSampleBemfN = 0;
		p->UVPCrossPos = 0;
		p->UVNCrossPos = 0;
		p->VWPCrossPos = 0;
		p->VWNCrossPos = 0;
		//GLOBAL.AlignCntStartOrDone = 0;
		TempUVOld = 0;
		TempVWOld = 0;
		p->BemfCnt++;	
	}
	//读取反电势 20~(BEMF_CHECK_TIMES-1)
	else if(p->BemfCnt < BEMF_CHECK_TIMES)
	{
		TempUV = (INT16S)(GetADResultModifyDef(AD_BEMF_UV,m1HalPara.ADGain,m1HalPara.ADOff)  << 2) - m1HalPara.UVBemfOff;
		TempVW = (INT16S)(GetADResultModifyDef(AD_BEMF_VW,m1HalPara.ADGain,m1HalPara.ADOff)  << 2) -m1HalPara.VWBemfOff;
		// 找峰值
		if((TempUV > BEMF_CHECK_THREA)&&(p->EnUVSampleBemfN == 0))
		{
			p->EnUVSampleBemfN = 1;
		}
		if((TempUV < -BEMF_CHECK_THREA)&&(p->EnUVSampleBemfP == 0))
		{
			p->EnUVSampleBemfP = 1;
		}
		if((TempVW > BEMF_CHECK_THREA)&&(p->EnVWSampleBemfN == 0))
		{
			p->EnVWSampleBemfN = 1;
		}
		if((TempVW < -BEMF_CHECK_THREA)&&(p->EnVWSampleBemfP == 0))
		{
			p->EnVWSampleBemfP = 1;
		}
		// 找过零
		if((p->EnUVSampleBemfN == 1) && NCross(TempUV,TempUVOld))
		{
			p->UVNCrossPos = m1ResolverDealModule.ResolverElecPos;
			p->EnUVSampleBemfN = 2;
		}
		if((p->EnUVSampleBemfP == 1)  && PCross(TempUV,TempUVOld))
		{
			p->UVPCrossPos = m1ResolverDealModule.ResolverElecPos;
			p->EnUVSampleBemfP = 2;
		}
		if((p->EnVWSampleBemfN == 1)  && NCross(TempVW,TempVWOld))
		{
			p->VWNCrossPos = m1ResolverDealModule.ResolverElecPos;
			p->EnVWSampleBemfN = 2;
		}
		if((p->EnVWSampleBemfP == 1)  && PCross(TempVW,TempVWOld))
		{
			p->VWPCrossPos = m1ResolverDealModule.ResolverElecPos;
			p->EnVWSampleBemfP = 2;
		}
		TempUVOld = TempUV;
		TempVWOld = TempVW;
		p->BemfCnt++;
	}
	//读反电势的周期结束
	else if(p->BemfCnt == BEMF_CHECK_TIMES)
	{
		p->BemfCnt++;			
		if(p->EnUVSampleBemfN + p->EnUVSampleBemfP 
			+  p->EnVWSampleBemfN  + p->EnVWSampleBemfP== 8)//反电势检测正确
		{
			p->UVNCrossPos -=11*ELEC30DEG;//UV N 330度
			p->UVPCrossPos -=5*ELEC30DEG;//UV P 150度
			p->VWNCrossPos -= 3*ELEC30DEG;//VW N 90度
			p->VWPCrossPos -= 9*ELEC30DEG;//VW P 270度
			Err1 = p->UVNCrossPos - p->UVPCrossPos;
			Err2 = p->UVNCrossPos - p->VWNCrossPos;
			Err3 = p->UVNCrossPos - p->VWPCrossPos;
			if((ABS(Err1) > ELEC10DEG)||(ABS(Err2) > ELEC10DEG)||(ABS(Err3) > ELEC10DEG))
			{
				p->AlignErrCode = 2;
			}
			else
			{
				m1MotorPara.PosOff = p->UVNCrossPos - (Err1 + Err2 + Err3)/4;/* 求三者之平均*/
			}
			
		}			
		else//没有检测到有效的四组反电势过零点
		{
			p->AlignErrCode = 1;
		}		
	}
	//最后一个周期以后
	else
	{
		//等待中频任务中状态机切换
	}
	return TRUE;
}

/**************************************************************************************************************************************************
* @函数名		:  		DebugDataPrint
* @函数描述	: 		设置DA打印的变量，TmpDataSel从0到9选择
* @创建人		:    	张宇
* 创建日期	:   	2020-03-07
* 修改记录	:
*
****************************************************************************************************************************************************/
void DebugDataPrint(Uint16 TmpDataSel)
{

	Uint16 Da1Output = 0;
	Uint16 Da2Output = 0;
	
	if(TmpDataSel == 0)
	{
		Da1Output = 1024;//(Uint16)m1ResolverDealModule.ResolverElecPos>>4;
		Da2Output = 2048;//(Uint16)m1SmcObserverExt.SmcElecPos >> 4;
	}
	else if(TmpDataSel == 1)
	{
		Da1Output = 1024;
		Da2Output = 2048;
	}	
	else if(TmpDataSel == 2)
	{
		Da1Output = (Uint16)OBS1 >> 4 ;
		Da2Output = (Uint16)OBS2 >> 4;
	}	
 	else if(TmpDataSel == 3)
	{
		Da1Output = m1FluxObserver.FluxEst_ElecSpd/419*2048+2048;
		Da2Output = (Uint16)m1ResolverDealModule.ResolverElecRpm >> 4;
	}	
 	else if(TmpDataSel == 4)
	{
		Da1Output = m1FluxObserver.FluxEst_Angle/2/PI*4096;
		Da2Output = (Uint16)m1CurrLoopCtler.ThetaPark>>4;
	}	
 	else if(TmpDataSel == 5)
	{
		Da1Output = (Uint16)m1CurrLoopCtler.ThetaPark>>4;
		Da2Output = (Uint16)m1SmcObserverExt.SmcElecPos >> 4;
	}	
 	else if(TmpDataSel == 6)
	{
		Da1Output = (Uint16)m1SmcObserverExt.SmcElecPos >> 4;
		Da2Output = (Uint16)m1SmcObserverExt.SmcElecRpm >> 4;
	}	
 	else if(TmpDataSel == 7)
	{
		Da1Output = (Uint16)m1CurrLoopCtler.ThetaPark >> 4;
		Da2Output = (Uint16)m1CurrLoopCtler.ThetaInvPark >> 4;
	}	
 	else if(TmpDataSel == 8)
	{
		Da1Output = (Uint16)(m1CurrLoopCtler.ThetaPark) >> 4;
		Da2Output = (Uint16)m1CurrLoopCtler.ThetaInvPark >> 4;
	}
	else if(TmpDataSel == 9)
	{
		Da1Output = 0;
		Da2Output = 0;
	}
	else if(TmpDataSel == 10)
	{
		Da1Output = m1BusVolDecoupler.V_Bus;
		Da2Output = m1BusVolDecoupler.V_Bus >> 2;
	}
	else if(TmpDataSel == 11)
	{
		Da1Output = (Uint16)m1SmcObserverExt.SmcElecPos >> 4;
		Da2Output = (Uint16)m1SmcObserverExt.SmcElecPos >> 4;
	}
	else if(TmpDataSel == 12)
	{
		Da1Output = (m1Hfi.HfiPosQ16 >> (8+4));
		Da2Output = 0;
	}
	else if(TmpDataSel == 13)
	{
		Da1Output = (m1Hfi.HfiPosQ16 >> (8+4));
		Da2Output = m1SmcObserverExt.SmcElecPos >> 4;
	}
	else if(TmpDataSel == 14)
	{
		Da1Output = m1Hfi.sinwt + 2048;
		Da2Output = m1Hfi.IqHpass * 16 + 2048;
	}
	//m1CriPara.HpVol  =  m1BusVolDecoupler.V_Bus;
	SetTestData(0x8000 + 0x4000 + (Da1Output & 0xFFF));
	SetTestData(0x0000 + 0x4000 + (Da2Output & 0xFFF));


}

