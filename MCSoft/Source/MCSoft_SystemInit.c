/*======================================================================================================
* @文件名称		:                                                       
* @文件描述		: 	用于参数定义
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03
* 修改记录		:
*                         
======================================================================================================*/
#include"MCSoft_SystemInit.h"
extern bool DSPCore_CfgFwCtler(stFwCtler* pD, stCtrlPara* pC);
/**************************************************************************************************************************************************
* @函数名		:  		MC_InitPara                                                        
* @函数描述		: 		1. 加载EEPROM中存储的参数结构体
							2. 若加载失败，则调用软件中存储的默认控制参数
							3. 若非所有结构体均加载成功，则置位EepromErr信号
						
* @创建人		:    		张宇
* 创建日期		:   		2020-03-10
* 修改记录		:
*                         
****************************************************************************************************************************************************/
bool MC_InitPara(void)
{
	bool rtnLoadParaSuccess = TRUE;
	m1CriPara.Eeprom.all = 0;
	/* 1. 电机参数结构体*/
	if(LoadPara((void *)&m1MotorPara, stEEPROMParaArr, MOTOR_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromMotorPara = 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1MotorPara, stEEPROMParaArr, MOTOR_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 2. 控制参数结构体*/
	if(LoadPara((void *)&m1CtrlPara, stEEPROMParaArr, CTRL_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromCtrlPara = 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1CtrlPara, stEEPROMParaArr, CTRL_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 3. 故障参数结构体*/
	if(LoadPara((void *)&m1FaultPara, stEEPROMParaArr, FAULT_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromFaultPara= 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1FaultPara, stEEPROMParaArr, FAULT_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 4. 告警参数结构体*/
	if(LoadPara((void *)&m1WarnPara, stEEPROMParaArr, WARN_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromWarnPara= 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1WarnPara, stEEPROMParaArr, WARN_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 5. 无位置控制参数结构体*/
	if(LoadPara((void *)&m1SnsLessPara, stEEPROMParaArr, SNSLESS_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromSnslessPara= 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1SnsLessPara, stEEPROMParaArr, SNSLESS_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 6.  HAL参数结构体*/
	if(LoadPara((void *)&m1HalPara, stEEPROMParaArr, HAL_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromHalPara= 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1HalPara, stEEPROMParaArr, HAL_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 7. 速度反馈修正参数结构体*/
	if(LoadPara((void *)&m1SpdFdbPara, stEEPROMParaArr, SPDFDB_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromSpdFdbPara= 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1SpdFdbPara, stEEPROMParaArr, SPDFDB_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 9. 系统配置参数结构体*/
	if(LoadPara((void *)&m1SysCfgPara, stEEPROMParaArr, SYSCFG_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromSysCfgPara= 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1SysCfgPara, stEEPROMParaArr, SYSCFG_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	
	if(rtnLoadParaSuccess == FALSE)
	{
		m1CriPara.Eeprom.bit.EepromErr = 1;
	}
	return rtnLoadParaSuccess;
}
/**************************************************************************************************************************************************
* @函数名		:  		MC_InitMCCore                                                        
* @函数描述		: 		1. 应在调用完MC_InitPara后，再调用该函数
							2. 该函数用于完成对FPGA实现的硬件IP的初始化
							3. 若初始化失败则返回FALSE,并将FpgaFail信号置为1
* @创建人		:    		张宇
* 创建日期		:   		2020-03-10
* 修改记录		:
*                         
****************************************************************************************************************************************************/
bool	MC_InitMCCore(void)
{

	bool rtnCfgMCCoreSuccess = TRUE;
	/*  1. 配置MCCore的电流环控制器*/
	if(!CfgCurrLoopCtler(&m1CurrLoopCtler,&m1CtrlPara,&m1MotorPara,&m1HalPara))
	{		
		rtnCfgMCCoreSuccess = FALSE;
	}
	else
	{
		DSPCore_CfgCurrLoopCtler(&m1CurrLoopCtler); 
	}
	/*  2. 配置MCCore的速度环控制器*/
	if(!CfgSpdLoopCtler(&m1SpdLoopCtler,&m1CtrlPara,&m1MotorPara,&m1HalPara))
	{
		rtnCfgMCCoreSuccess = FALSE;
	}
	else
	{
		DSPCore_CfgFwCtler(&m1FwCtler,&m1CtrlPara);
	}
	/*  3. 配置MCCore的母线电压解耦模块*/
	if(!CfgBusVolDecoupler(&m1BusVolDecoupler,&m1HalPara))
	{
		rtnCfgMCCoreSuccess = FALSE;
	}	
	/*  4. 配置MCCore的滑模观测器*/
	//if(!CfgSmcObserver(&m1SmcObserver,&m1CtrlPara,&m1MotorPara,&m1HalPara,&m1SnsLessPara))
	//{
	//	rtnCfgMCCoreSuccess = FALSE;
	//}	
	/*  5. 配置MCCore的旋变接口模块*/
	if(!CfgResolverDealModule(&m1ResolverDealModule,&m1CurrLoopCtler,&m1MotorPara))
	{
		rtnCfgMCCoreSuccess = FALSE;
	}	
	/*  6. 配置MCCore的故障处理模块*/
	DSPCore_CfgFaultDealModule(&m1FaultDealModule);
	if(!CfgFaultDealModule(&m1FaultDealModule,&m1FaultPara,&m1HalPara))
	{
		rtnCfgMCCoreSuccess = FALSE;
	}	
	else
	{

	}
	
	if(rtnCfgMCCoreSuccess == FALSE)
	{
		m1CriPara.GSts.bit.FpgaFailFault = 1;
	}
	return rtnCfgMCCoreSuccess;
	
}
/**************************************************************************************************************************************************
* @函数名		:  		MC_InitCriticalData                                                        
* @函数描述		: 		1.  CPU关键变量的初始化						 
* @创建人		:    		张宇
* 创建日期		:   		2020-03-10
* 修改记录		:
*                         
****************************************************************************************************************************************************/
bool MC_InitCriticalData(stCriticalPara* pC)
{
	bool rtn = FALSE;
	pC->IdRef10mA = 0;
	pC->IqRef10mA = 0;
	pC->SpdRef = 0;
	pC->SpdRefSlp = 0;
	pC->SpdOut = 0;
	pC->UdRef = 0;
	pC->UqRef = 0;
	pC->IdFdb = 0;
	pC->IqFdb = 0;
	pC->SpdFdb = 0;	
	pC->HpVol = 0;
	pC->IBus = 0;
	pC->Power = 0;
	pC->LpVol = 0;
	pC->TmpPdd = 0;
	pC->TmpMotor = 0;
	pC->CtrlMode = MODE_CURR;
	pC->FaultNow = 0;
	pC->FaultOccurred= 0;
	pC->WarnNow= 0;
	pC->SoftErrCode= 0; 	/* MC_PostCodeErr函数抛出的软件故障字*/
	pC->SoftWarnCode = 0;
	pC->DiscardCmdMsg = CmdNOCMD;
	pC->Eeprom.all = 0;	/* EEPROM加载结构指示*/
	pC->GSts.all = 0;		/*  全局状态量信号*/
	pC->MsgBox.all = 0;
	return rtn;
	
}

