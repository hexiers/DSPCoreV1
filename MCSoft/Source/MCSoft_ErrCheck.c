/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-07                                                                 
* 修改记录		:   
*                         2020-10-10: 增加标定启动失败的判断
======================================================================================================*/
#include"MCSoft_ErrCheck.h"

/**************************************************************************************************************************************************
* @函数名		:  		SignalMakeSure                                                        
* @函数描述	: 		实现信号的数字滤波
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-07                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool  SignalMakeSure(Uint16 MakeSure_Id, bool  InputData,Uint16 MakesureTimes)
{
	bool RtnVal = FALSE;
	static  Uint16 MakeSureCnt[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	if(MakeSure_Id >= 32)
	{	
		/* 软件异常分支*/
		MC_PostCodeErr(1);
		return FALSE;
	}
	if(InputData)
	{
		/* 防止累加溢出*/
		if(MakeSureCnt[MakeSure_Id] < MakesureTimes)
		{
			MakeSureCnt[MakeSure_Id]++;
		}
		else
		{
			RtnVal = TRUE;
		}
	}
	else
	{
		MakeSureCnt[MakeSure_Id] = 0;
	}
	return RtnVal;
}
/**************************************************************************************************************************************************
* @函数名		:  		UpDateCriticalSampleData_MediumFreq                                                       
* @函数描述	: 		放在中频任务中，更新关键数据结构体
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-10                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
void UpDateCriticalSampleData_MediumFreq(stCriticalPara* pC)
{
	INT32S TmpSpd = 0;
	if(m1SysCfgPara.SysCfgWord1.bit.EnSnsLess)
	{
		//TmpSpd = m1SmcObserverExt.SmcElecRpm/m1MotorPara.MotorPoles;/* 软件观测器*/
		TmpSpd = (INT32S)m1SmcObserverExt.SmcElecRpm * SPDFDB_SCALE_FACTOR/m1MotorPara.MotorPoles;/* FPGA观测器*/
	}
	else /* 有传感器*/
	{   
	    /* 带系数的电气转速*/
	    TmpSpd = m1ResolverDealModule.ResolverElecRpm;
	    /* 为DSP的速度控制器反馈带系数的电气转速*/

		/* 计算机械转速，用于显示*/
	    TmpSpd = TmpSpd * SPDFDB_SCALE_FACTOR/m1MotorPara.SnsorPoles;
	}
	m1CriPara.SpdFdb  = FirCalc(&SpdFdbFir, TmpSpd, 1);

	m1CriPara.SpdOut = (INT32S)((INT32S)m1SpdLoopCtler.SpdLimOut * (Uint32)m1HalPara.BitTo10mACoef) >> 12;
	// FPGA Bug 临时放到中断中
	//m1CriPara.HpVol  = (Uint32)m1BusVolDecoupler.V_Bus*(Uint32)m1HalPara.BitTo10mVCoef>>12;
	m1CriPara.LpVol 	= HAL_GetLpVol();
	m1CriPara.IBus 	    = HAL_GetHpCurr();
	m1CriPara.TmpMotor	= HAL_GetMotorTemp();
	m1CriPara.TmpPdd 	= HAL_GetPddTemp();
	m1CriPara.UdRef     = m1CurrLoopCtler.UdActOut_V;
	m1CriPara.UqRef     = m1CurrLoopCtler.UqActOut_V;
	m1CriPara.MainState = m1State.MainState;
	m1CriPara.SubState  = m1State.SubState;
	m1CriPara.LowSpdCapture1 = *stScope.Monitor1Addr;// V1.6
	m1CriPara.LowSpdCapture2 = *stScope.Monitor2Addr;// V1.6
}

/**************************************************************************************************************************************************
* @函数名		:  		UpDateCriticalSampleData_HighFreq                                                       
* @函数描述	: 		放在中频任务中，更新关键数据结构体
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-10                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
void UpDateCriticalSampleData_HighFreq(stCriticalPara* pC)
{
	INT16S IdBitFdbFiltered = 0;
	INT16S IqBitFdbFiltered = 0;   
	IdBitFdbFiltered = (INT16S)FirCalc(&IdFdbFir, m1CurrLoopCtler.IdFdb, 1);
	IqBitFdbFiltered = (INT16S)FirCalc(&IqFdbFir, m1CurrLoopCtler.IqFdb, 1);
	m1CriPara.IdFdb = (INT32S)((INT32S)IdBitFdbFiltered * (Uint32)m1HalPara.BitTo10mACoef) >>12;
	m1CriPara.IqFdb = (INT32S)((INT32S)IqBitFdbFiltered * (Uint32)m1HalPara.BitTo10mACoef) >>12;
}
/**************************************************************************************************************************************************
* @函数名		:  		MC_DoFaultCheck                                                        
* @函数描述	: 		软件故障判断
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-10                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
Uint32  MC_DoFaultCheck(stSysCfgPara* pCfg,stSysState* pState,stFaultPara* pF,stCriticalPara* pC)
{
	Uint32 CodeReturn = 0;
	/* 直接拷贝FPGA故障字*/
	CodeReturn = m1CurrLoopCtler.ErrFlag.all & 0xFF;
	if(CodeReturn)
	{
		/* CPU发现FPGA故障后，直接清除FPGA故障,CPU会将该故障锁存*/
		m1CurrLoopCtler.MotCtl.bit.ClrErr = 1;
	}
#if 1
	/* 低压过压检测*/
	if(SignalMakeSure(Err_LPOV,pC->LpVol > pF->LPOvFaultPoint,3))
	{
		CodeReturn |= (1 << Err_LPOV);
	}
	/* 低压欠压检测*/
	if(SignalMakeSure(Err_LPUV,pC->LpVol < pF->LPUvFaultPoint,3))
	{
		CodeReturn |= (1 << Err_LPUV);
	}
	/* 高压过流检测*/
	if(SignalMakeSure(Err_HPOC,pC->IBus > pF->HPOcFaultPoint,3))
	{
		CodeReturn |= (1 << Err_HPOC);
	}
	/* 功率管过温检测*/
	if(SignalMakeSure(Err_PDDOVT,pC->TmpPdd > pF->PDDOvtFaultPoint,3))
	{
		CodeReturn |= (1 << Err_PDDOVT);
	}
	/*  电机过温检测*/
	if(SignalMakeSure(Err_MOTOROVT, pC->TmpMotor  > pF->MotorOvtFaultPoint,3))
	{
		CodeReturn |= (1 << Err_MOTOROVT);
	}	
	/* 软件相过流检测*/
	if(SignalMakeSure(Err_SOFTOC, (ABS(pC->IqFdb) > pF->PhaseOcFaultPoint) ||(ABS(pC->IdFdb) > pF->PhaseOcFaultPoint),3))
	{
		CodeReturn |= (1 << Err_SOFTOC);
	}
	/*  超速检测*/
	if(SignalMakeSure(Err_OVSPD, ABS(pC->SpdFdb)  > pF->OvSpdFaultPoint,3))
	{
		CodeReturn |= (1 << Err_OVSPD);
	}
	/*  无感转速过低检测，仅在无感控制、且不用HFI的RUN状态执行 V1.6*/
	if((pCfg->SysCfgWord1.bit.EnSnsLess)&&(pCfg->SysCfgWord1.bit.EnHfi == 0)&&(pState->MainState == STS_Run))
	{
		if(SignalMakeSure(Err_UNSPD, pC->SpdFdb  <  pF->SnsLessUnSpdFaultPoint,3))
		{
			//CodeReturn |= ((Uint32)1 << Err_UNSPD);
		}
	}
	/*  预留控制器硬件故障检测*/
	if(pC->GSts.bit.FpgaFailFault)
	{
		CodeReturn |= ((Uint32)1 << Err_HARDWARE);
	}	
	/*  无感启动失败故障检测*/
	if(((pState->MainState == STS_StartUp)&&(pState->SubState == 3))
		|| ((pState->MainState == STS_MainTenance)&&(pState->SubState == 16)))
	{
		if(m1StartUp.StartUpSuccess == FALSE)
		{
			CodeReturn |= ((Uint32)1 << Err_STARTUP);
		}
	}
	#endif
	/* EEPROM故障 作为告警*/
	/*(pC->Eeprom.bit.EepromErr)
	{
		CodeReturn |= (1 << Err_EEPROM);	
	}*/
	/* CPU软件故障*/
	if(pC->SoftErrCode)
	{
		CodeReturn |= ((Uint32)1 << Err_SOFT);
	}
	if(m1PosCalib.AlignErrCode)
	{
		CodeReturn |= ((Uint32)1 << Err_CALIPOS);
		m1PosCalib.AlignErrCode = 0;
	}
	
	return CodeReturn;
}

/**************************************************************************************************************************************************
* @函数名		:  		MC_DoWarnCheck                                                        
* @函数描述	: 		软件告警判断
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-10                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
Uint32  MC_DoWarnCheck(stSysCfgPara* pCfg,stSysState* pState,stWarnPara* pW,stCriticalPara* pC)
{
	Uint32 CodeReturn = 0;
	/* 高压过压*/
	if(pC->HpVol > pW->HPOvWarnPoint)
	{
		CodeReturn |= (1 << Warn_HPOV);
	}
	/* 高压欠压*/
	if(pC->HpVol < pW->HPUvWarnPoint)
	{
		CodeReturn |= (1 << Warn_HPUV);
	}
	/* 低压过压*/
	if(pC->LpVol > pW->LPOvWarnPoint)
	{
		CodeReturn |= (1 << Warn_LPOV);
	}
	/* 低压欠压*/
	if(pC->LpVol < pW->LPUvWarnPoint)
	{
		CodeReturn |= (1 << Warn_LPUV);
	}
	/* 高压过流*/
	if(pC->IBus > pW->HPOcWarnPoint)
	{
		CodeReturn |= (1 << Warn_HPOC);
	}
	/* 功率管过温*/
	if(pC->TmpPdd > pW->PDDOvtWarnPoint)
	{
		CodeReturn |= (1 << Warn_PDDOVT);
	}
	/* 电机过温*/
	if(pC->TmpMotor> pW->MotorOvtWarnPoint)
	{
		CodeReturn |= (1 << Warn_MOTOROVT);
	}
	/* 软件相过流检测*/
	if((ABS(pC->IqFdb) > pW->PhaseOcWarnPoint) ||(ABS(pC->IdFdb) > pW->PhaseOcWarnPoint))
	{
		CodeReturn |= (1 << Warn_SOFTOC);
	}
	/*  超速检测*/
	if( pC->SpdFdb  > pW->OvSpdWarnPoint)
	{
		CodeReturn |= (1 << Warn_OVSPD);
	}
	/*  无感转速过低检测，仅在无感控制的RUN状态执行*/
	if((pCfg->SysCfgWord1.bit.EnSnsLess)&&(pState->MainState == STS_Run))
	{
		if(pC->SpdFdb  <  pW->SnsLessUvSpdWarnPoint)
		{
			CodeReturn |= (1 << Warn_UNSPD);
		}
	}
	/*  EEPROM参数无效，使用程序默认参数*/
	if(pC->Eeprom.bit.EepromErr)
	{
		CodeReturn |= (1 << Warn_EEDATAINVLID);	
	}
	/*  EEPROM存储失败，使用程序默认参数*/
	if(pC->GSts.bit.SaveEepromFailWarn)
	{
		CodeReturn |= (1 << Warn_EEPROMFAIL);	
	}
	

	return CodeReturn;
}

/**************************************************************************************************************************************************
* @函数名		:  		MC_PostCodeErr                                                        
* @函数描述	: 		通告软件开发故障
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-07                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
void MC_PostCodeErr(Uint16 ErrCode)
{
	m1CriPara.SoftErrCode = ErrCode;
}

/**************************************************************************************************************************************************
* @函数名		:  		MC_PostCodeWarn
* @函数描述	: 		通告软件开发故障
* @创建人		:    		张宇
* 创建日期		:   		2020-03-07
* 修改记录		:
*
****************************************************************************************************************************************************/
void MC_PostCodeWarn(Uint16 ErrCode)
{
	m1CriPara.SoftWarnCode = ErrCode;
}

