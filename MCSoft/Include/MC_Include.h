/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����MC��ص�����ͷ�ļ�
* @������		:  	����                                                                              
* ��������		:   	2020-03-03                                                                 
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#ifndef MC_INCLUDE_H_H
#define MC_INCLUDE_H_H

typedef enum
{
	CmdNOCMD = 0x00,
	CmdSTARTCURRLOOP = 0x01,	/* wParaָ��Ŀ�����,��λ10mA*/
	CmdSTARTSPDLOOP,			/* wParaָ��Ŀ��ת��,��λrpm*/
	CmdSTOPMOTOR,
	CmdREQSTRUCTDATA,
	CmdREQSINGLEDATA,
	CmdOutMT,					/* �˳�ά��ģʽ*/
	CmdGoMT,					/* ����ά��ģʽ*/
	CmdUPDATEPARA,			
	CmdSAVPARA,				/* �������洢��EEPROM*/
	CmdDoADCCALIB,			/*  ִ��ģ����ƫ�õ�У��*/
	CmdDoPOSOFFCALIB,			/* ִ�е����ʼ��У��*/
	CmdDoSPDCALIB,			/* ִ���ٶȷ���������У��*/
	CmdSTOPCALIB,				/* ֹͣУ��*/
	CmdCLRERR,					/* ������0*/
	CmdDoRANDLCALIB, /* ��ϵ���ĵ�������*/
	CmdSetMonitor1,/* �趨����1*/
	CmdSetMonitor2,/* �趨����2*/
	CmdDiagHfi,/*����HFI*/
	CmdDoPOSOFFCALIB2/* ��ת������ʼ��*/
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
