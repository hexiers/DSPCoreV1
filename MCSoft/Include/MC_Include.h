/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	包含MC相关的所有头文件
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#ifndef MC_INCLUDE_H_H
#define MC_INCLUDE_H_H

typedef enum
{
	CmdNOCMD = 0x00,
	CmdSTARTCURRLOOP = 0x01,	/* wPara指定目标电流,单位10mA*/
	CmdSTARTSPDLOOP,			/* wPara指定目标转速,单位rpm*/
	CmdSTOPMOTOR,
	CmdREQSTRUCTDATA,
	CmdREQSINGLEDATA,
	CmdOutMT,					/* 退出维护模式*/
	CmdGoMT,					/* 进入维护模式*/
	CmdUPDATEPARA,			
	CmdSAVPARA,				/* 将参数存储至EEPROM*/
	CmdDoADCCALIB,			/*  执行模拟量偏置的校正*/
	CmdDoPOSOFFCALIB,			/* 执行电机初始角校正*/
	CmdDoSPDCALIB,			/* 执行速度反馈修正的校正*/
	CmdSTOPCALIB,				/* 停止校正*/
	CmdCLRERR,					/* 故障清0*/
	CmdDoRANDLCALIB, /* 诊断电机的电阻与电感*/
	CmdSetMonitor1,/* 设定监视1*/
	CmdSetMonitor2,/* 设定监视2*/
	CmdDiagHfi,/*整定HFI*/
	CmdDoPOSOFFCALIB2/* 旋转整定初始角*/
} Message_t;

#include "stdlib.h"
#include "IQmathLib.h"
#include"MCSoft_DataType.h"
#include"MCSoft_Message.h"
#include"MCSoft_Uart.h"
#include"MCSoft_Scope.h"
#include"MCSoft_Hfi.h"

#include"MCCore_BusVolDecoupler.h"
#include"MCCore_CurrLoopCtler.h"
#include"MCCore_FaultDealModule.h"
#include"MCCore_ResolverDealModule.h"
#include"MCCore_SmcObserver.h"
#include"MCSoft_SmcObserver_ext.h"
#include"MCCore_SpdLoopCtler.h"
#include"MCCore_RestRegister.h"

#include"MCPara_CtrlPara.h"
#include"MCPara_FaultPara.h"
#include"MCPara_HalPara.h"
#include"MCPara_MotorPara.h"
#include"MCPara_SnsLessPara.h"
#include"MCPara_SpdFdbPara.h"
#include"MCPara_SysCfgPara.h"
#include"MCPara_WarnPara.h"
#include"MCPara_Eeprom.h"
#include"MCPara_LoadSave.h"


#include"MCSoft_ErrCheck.h"
#include"MCSoft_SystemInit.h"
#include"MCSoft_StartUp.h"
#include"MCSoft_SpdCtrl.h"
//#include"MCSoft_SmcObserver_ext.h"
#include"MCSoft_Math.h"


#include"MC_HalApi.h"

#include"MCCore_FluxEst.h"



#endif
