/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-07                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#include"MCSoft_StartUp.h"
stStartup m1StartUp;
stIir StartupUsIIR;
/**************************************************************************************************************************************************
* @函数名		:  		IirInit                                                        
* @函数描述	: 		初始化惯性滤波器，新值系数FilterCoef/128
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-07                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
void IirInit(stpIir stp,INT32S FilterCoef)
{
	stp->OldValue = 0;
	stp->NewValueCoef = LIMITMAXMIN(FilterCoef, 128, 0);
	stp->OldVaueCoef = 128-stp->NewValueCoef;
}

/**************************************************************************************************************************************************
* @函数名		:  		IirCalc                                                        
* @函数描述	: 		惯性滤波器计算，输入的数据范围应为-32768,32767
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-07                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
INT16S IirCalc(stpIir stp,INT16S DataIn)
{
	INT32S Tmp = 0;
	Tmp = DataIn * 128L;
	Tmp *= stp->NewValueCoef;
	Tmp += stp->OldVaueCoef * stp->OldValue;
	Tmp = (Tmp >>7);/* 针对滤波系数128归一化为1*/
	stp->OldValue = Tmp;
	return(INT16S)(Tmp >> 7);
}

/**************************************************************************************************************************************************
* @函数名		:  		MC_InitStartUp                                                        
* @函数描述	: 		初始化stStartup数据
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-07                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool MC_InitStartUp(stCtrlPara* pCtrl,stSnsLessPara* pSns,stMotorPara*pMotor,stStartup* pStartup)
{
	Uint32 SampleFreq_Hz = 0;
	pStartup->StartUpCnt = 0;
	pStartup->StartupUs = 0;
	pStartup->StartupUs1 = 0;
	pStartup->StartupUs2  = 0;
	pStartup->AlignCallTimes = 0;
	/* ms为单位的启动时间*/
	pStartup->StartupTimeInms = LIMITMAXMIN(pSns->StartUpTime_ms, 32767, 1000);
	/* 电流环频率*/
	SampleFreq_Hz = PWMCLKFREQ/(2*pCtrl->PwmPeriod);
	/* 计算速度累加的总频次，每隔64个中断周期，执行一次速度累加*/
	pStartup->StartupAddTimes = (pStartup->StartupTimeInms*SampleFreq_Hz)/(1000L * 64L);
	/* 预定位通电周期，以电流环周期为单位*/
	pStartup->AlignSetTimes = pSns->AlignTime_ms*SampleFreq_Hz/(1000);
	/* 起跳速度*/
	pStartup->StartupRpm1 = 1;
	/* 启动结束速度*/
	pStartup->StartupRpm2 = LIMITMAXMIN(pSns->StartUpSpd, 32767, 100);
	/* 用以位置累加的速度当量初值*/
	pStartup->StartupSpd1Used = ( (Uint32)pStartup->StartupRpm1*pMotor->MotorPoles*(ADDNUMGAIN/SampleFreq_Hz))/60;
	//用以位置累加的速度当量终值
	pStartup->StartupSpd2Used = ( (Uint32)pStartup->StartupRpm2*pMotor->MotorPoles*(ADDNUMGAIN/SampleFreq_Hz))/60;
	//速度当量总增量
	pStartup->StartupSpdDelta = pStartup->StartupSpd2Used - pStartup->StartupSpd1Used;
	
	pStartup->HalfStartSpdUsed1P256 = pStartup->StartupSpdDelta >> 9;
	if(pStartup->HalfStartSpdUsed1P256 == 0)
	{
        pStartup->HalfStartSpdUsed1P256 = 1;
	}
	pStartup->PosCoef = 0;
	//速度当量单次累加增量，防止速度累加值太小
	if(pStartup->StartupSpdDelta <= pStartup->StartupAddTimes)
	{
	    pStartup->StartupAccDelta = 1;
	}
	else
	{
	    pStartup->StartupAccDelta = pStartup->StartupSpdDelta/pStartup->StartupAddTimes;
	}
	pStartup->PosAdd = 0;
	pStartup->StartUpSuccess = FALSE;
	
	IirInit(&StartupUsIIR,1);	
	return TRUE;

}
/**************************************************************************************************************************************************
* @函数名		:  		MC_DoStartUp                                                        
* @函数描述	: 		1. 生产开环带转的位置序列，pStartup->PosAdd[31:16],
						2. 数据范围[0 65535]对应电气[0 2PI]
						3. 函数返回开环生成的位置
						4. 开环带转完成后StartUpSuccess信号指示是否启动成功
						5. 带转完成后SubState = 3
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-07                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
Uint16 MC_DoStartUp(stStartup* pStartup,stSysState* pState)
{
	INT32S TmpUd = 0;
	INT32S TmpUq = 0;
	//INT32S Tmp = 0;
	//INT32S ErrQ8 = 0;
	Uint16 OpenLoopPos = 0;
    //INT16S s16Tmp = 0;
	/* 预定位*/
	 if(pState->SubState == 1)
	{
		if(pStartup->AlignCallTimes++ > pStartup->AlignSetTimes)
		{
			pState->SubState = 2;
		}		
	}
	/*  I/F启动*/
	 if((pState->SubState == 2)||(pState->SubState == 3))
	{
		TmpUd = m1CurrLoopCtler.UdPILim_V;
		TmpUq = m1CurrLoopCtler.UqPILim_V;

		/* 位置累加*/
		pStartup->PosAdd += pStartup->StartupSpd1Used;
		pStartup->StartUpCnt ++;
		/* 累加溢出*/
		if(pStartup->PosAdd > ADDNUMGAIN)
		{
			pStartup->PosAdd -= ADDNUMGAIN;
		}
        
		/* 速度累加*/
		if(pStartup->StartupSpd1Used < pStartup->StartupSpd2Used)
		{
			/* 每64个电流周期执行一次速度累加*/
			if((pStartup->StartUpCnt & 63) == 63)
			{
				pStartup->StartupUs = IirCalc(&StartupUsIIR,(TmpUd*TmpUd+TmpUq*TmpUq)>>12);
				pStartup->StartupSpd1Used += pStartup->StartupAccDelta;
			}
			/* 1/4启动转速时候，记录一次合成输出电压*/
			if((pStartup->StartupUs1 == 0)&&(pStartup->StartupSpd1Used > (pStartup->StartupSpd2Used >> 2)))
			{
				pStartup->StartupUs1 = pStartup->StartupUs > 0?pStartup->StartupUs:1;
			}
		}
		else/* 带转结束*/
		{			
			pState->SubState = 3;
			/* 带转结束时候，记录一次合成输出电压*/
			if(pStartup->StartupUs2 == 0)
			{
				pStartup->StartupUs2 = pStartup->StartupUs > 0?pStartup->StartupUs:1;
			}
			/* 带转结束时判断是否启动成功*/
			if(pStartup->StartupUs2 > 2*pStartup->StartupUs1)
			{
				pStartup->StartUpSuccess = TRUE;
			}
			else
			{
				pStartup->StartUpSuccess = FALSE;
			}
		}
		#if 0 /* 位置的平滑切换*/
		// PosCoef 随着转速的提升，由0变成256
		Tmp = (pStartup->StartupSpd2Used - pStartup->StartupSpd1Used)/pStartup->HalfStartSpdUsed1P256;
		pStartup->PosCoef = LIMITMAXMIN(256-Tmp, 256, 0);
		// 计算误差,误差加权修正
		s16Tmp = pStartup->PosAdd >> 8;
		ErrQ8 = m1SmcObserver.SmcElecPos - s16Tmp;
		ErrQ8 = ErrQ8 * pStartup->PosCoef;
		pStartup->PosAdd += ErrQ8;
		#endif
		
		
	}
	OpenLoopPos = pStartup->PosAdd >> 8;
	return OpenLoopPos;
}
/**************************************************************************************************************************************************
* @函数名		:  		MC_DoStartUpCalib                                                        
* @函数描述	: 			1. 生产开环带转的位置序列，pStartup->PosAdd[31:16],
						2. 数据范围[0 65535]对应电气[0 2PI]
						3. 函数返回开环生成的位置
						4. 开环带转完成后StartUpSuccess信号指示是否启动成功
						5. 带转完成后SubState = 16
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-07                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
Uint16 MC_DoStartUpCalib(stStartup* pStartup,stSysState* pState)
{
	INT32S TmpUd = 0;
	INT32S TmpUq = 0;
	Uint16 OpenLoopPos = 0;
    //INT16S s16Tmp = 0;
	/* 预定位*/
	 if(pState->SubState == 14)
	{
		if(pStartup->AlignCallTimes++ > pStartup->AlignSetTimes)
		{
			pState->SubState = 15;
		}		
	}
	/*  I/F启动*/
	 if((pState->SubState == 15)||(pState->SubState == 16))
	{
		TmpUd = m1CurrLoopCtler.UdPILim_V;
		TmpUq = m1CurrLoopCtler.UqPILim_V;

		/* 位置累加*/
		pStartup->PosAdd += pStartup->StartupSpd1Used;
		pStartup->StartUpCnt ++;
		/* 累加溢出*/
		if(pStartup->PosAdd > ADDNUMGAIN)
		{
			pStartup->PosAdd -= ADDNUMGAIN;
		}
        
		/* 速度累加*/
		if(pStartup->StartupSpd1Used < pStartup->StartupSpd2Used)
		{
			/* 每64个电流周期执行一次速度累加*/
			if((pStartup->StartUpCnt & 63) == 63)
			{
				pStartup->StartupUs = IirCalc(&StartupUsIIR,(TmpUd*TmpUd+TmpUq*TmpUq)>>12);
				pStartup->StartupSpd1Used += pStartup->StartupAccDelta;
			}
			/* 1/4启动转速时候，记录一次合成输出电压*/
			if((pStartup->StartupUs1 == 0)&&(pStartup->StartupSpd1Used > (pStartup->StartupSpd2Used >> 2)))
			{
				pStartup->StartupUs1 = pStartup->StartupUs > 0?pStartup->StartupUs:1;
			}
		}
		else/* 带转结束*/
		{			
			pState->SubState = 16;
			/* 带转结束时候，记录一次合成输出电压*/
			if(pStartup->StartupUs2 == 0)
			{
				pStartup->StartupUs2 = pStartup->StartupUs > 0?pStartup->StartupUs:1;
			}
			/* 带转结束时判断是否启动成功*/
			if(pStartup->StartupUs2 > 2*pStartup->StartupUs1)
			{
				pStartup->StartUpSuccess = TRUE;
			}
			else
			{
				pStartup->StartUpSuccess = FALSE;
			}
		}
	}
	OpenLoopPos = pStartup->PosAdd >> 8;
	return OpenLoopPos;
}




