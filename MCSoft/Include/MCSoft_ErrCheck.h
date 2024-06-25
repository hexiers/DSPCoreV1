/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����
* @������		:  	����                                                                              
* ��������		:   	2020-03-07                                                                 
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#include "MC_Include.h"
#ifndef ERRCHECK_H_H
#define ERRCHECK_H_H

typedef enum
{
	//Err_NOERR = 0x00,
	Err_OC  = 0x00, 	/* Ӳ���Ƚ���������		FPGA�ж�*/
	Err_SC, 			/* ����ƬFAULT��			FPGA�ж�*/
	Err_HPOV,			/* ��ѹ��ѹ�źţ�		FPGA�жϣ���ӦHPOvFaultPoint*/
	Err_HPUV,			/* ��ѹǷѹ�źţ�		FPGA�жϣ���ӦHPUvFaultPoint*/
	Err_POS,			/* ��������źţ�		FPGA�ж�*/
	Err_UVLO,			/* ����Ƿѹ�źţ�		FPGA�ж�*/
	Err_SERVODIS,		/* �û���ͣ�źţ�		FPGA�ж�*/
	Err_LPOV,			/* ��ѹ��ѹ�źţ�		FPGA�жϣ���ӦLPOvFaultPoint*/
	Err_LPUV,			/* ��ѹǷѹ�źţ�		FPGA�жϣ���ӦLPUvFaultPoint*/
	Err_HPOC,			/* ��ѹ�����źţ�		CPU�жϣ���ӦHPOcFaultPoint*/
	Err_PDDOVT,			/* ���ʹܹ����źţ�		CPU�жϣ���ӦPDDOvtFaultPoint*/
	Err_MOTOROVT,		/* ��������źţ�		CPU�жϣ���ӦMotorOvtFaultPoint*/
	Err_SOFTOC,			/* ����жϹ����źţ�	CPU�жϣ���ӦPhaseOcFaultPoint*/
	Err_OVSPD,			/* �����źţ�				CPU�жϣ���ӦOvSpdFaultPoint*/
	Err_UNSPD,			/* �޸�ת�ٹ����źţ�	CPU�жϣ���ӦSnsLessUnSpdFaultPoint*/
	Err_HARDWARE,		/* ������Ӳ�����ϣ�		CPU�ж�*/
	Err_STARTUP,		/* �޸�����ʧ���źţ�	CPU�ж�*/
	Err_EEPROM,			/* ����EEPROM���ϣ�		CPU�ж�*/
	Err_SOFT,			/* CPU������ϣ�		CPU�ж�*/
	Err_CALIPOS			/* ��ʼ�Ǳ궨����		CPU�ж�*/
} Err_t;
typedef enum
{
	//Warn_NOERR = 0x00,
	Warn_HPOV= 0x00,		/* ��ѹ��ѹ�źţ�		CPU�жϣ���ӦHPOvWarnPoint*/
	Warn_HPUV,				/* ��ѹǷѹ�źţ�		CPU�жϣ���ӦHPUvWarnPoint*/
	Warn_LPOV,				/* ��ѹ��ѹ�źţ�		CPU�жϣ���ӦLPOvWarnPoint*/
	Warn_LPUV,				/* ��ѹǷѹ�źţ�		CPU�жϣ���ӦLPUvWarnPoint*/
	Warn_HPOC,				/* ��ѹ�����źţ�		CPU�жϣ���ӦHPOcWarnPoint*/
	Warn_PDDOVT,			/* ���ʹܹ����źţ�		CPU�жϣ���ӦPDDOvtWarnPoint*/
	Warn_MOTOROVT,		/* ��������źţ�		CPU�жϣ���ӦMotorOvtWarnPoint*/
	Warn_SOFTOC,			/* ����жϹ����źţ�	CPU�жϣ���ӦPhaseOcWarnPoint*/
	Warn_OVSPD,			/* �����źţ�				CPU�жϣ���ӦOvSpdWarnPoint*/
	Warn_UNSPD,			/* �޸�ת�ٹ����źţ�	CPU�жϣ���ӦSnsLessUnSpdWarnPoint*/
	Warn_EEDATAINVLID,		/* ����ʧ��		CPU�ж�*/
	Warn_EEPROMFAIL		/* �洢ʧ��		CPU�ж�*/
} Warn_t;

extern bool  SignalMakeSure(Uint16 MakeSure_Id, bool  InputData,Uint16 MakesureTimes);
/*extern void UpDateCriticalSampleData(stCriticalPara* pC);
extern Uint32  MC_DoFaultCheck(stSysCfgPara* pCfg,stSysState* pState,stFaultPara* pF,stCriticalPara* pC);
extern Uint32  MC_DoWarnCheck(stSysCfgPara* pCfg,stSysState* pState,stWarnPara* pW,stCriticalPara* pC);*/
extern void MC_PostCodeErr(Uint16 ErrCode);
extern void MC_PostCodeErr(Uint16 ErrCode);
extern void MC_PostCodeWarn(Uint16 ErrCode);
#endif

