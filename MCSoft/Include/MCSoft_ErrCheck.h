/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-07                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#include "MC_Include.h"
#ifndef ERRCHECK_H_H
#define ERRCHECK_H_H

typedef enum
{
	//Err_NOERR = 0x00,
	Err_OC  = 0x00, 	/* 硬件比较器过流，		FPGA判断*/
	Err_SC, 			/* 驱动片FAULT，			FPGA判断*/
	Err_HPOV,			/* 高压过压信号，		FPGA判断，对应HPOvFaultPoint*/
	Err_HPUV,			/* 高压欠压信号，		FPGA判断，对应HPUvFaultPoint*/
	Err_POS,			/* 旋变故障信号，		FPGA判断*/
	Err_UVLO,			/* 驱动欠压信号，		FPGA判断*/
	Err_SERVODIS,		/* 用户急停信号，		FPGA判断*/
	Err_LPOV,			/* 低压过压信号，		FPGA判断，对应LPOvFaultPoint*/
	Err_LPUV,			/* 低压欠压信号，		FPGA判断，对应LPUvFaultPoint*/
	Err_HPOC,			/* 高压过流信号，		CPU判断，对应HPOcFaultPoint*/
	Err_PDDOVT,			/* 功率管过温信号，		CPU判断，对应PDDOvtFaultPoint*/
	Err_MOTOROVT,		/* 电机过温信号，		CPU判断，对应MotorOvtFaultPoint*/
	Err_SOFTOC,			/* 软件判断过流信号，	CPU判断，对应PhaseOcFaultPoint*/
	Err_OVSPD,			/* 超速信号，				CPU判断，对应OvSpdFaultPoint*/
	Err_UNSPD,			/* 无感转速过低信号，	CPU判断，对应SnsLessUnSpdFaultPoint*/
	Err_HARDWARE,		/* 控制器硬件故障，		CPU判断*/
	Err_STARTUP,		/* 无感启动失败信号，	CPU判断*/
	Err_EEPROM,			/* 参数EEPROM故障，		CPU判断*/
	Err_SOFT,			/* CPU软件故障，		CPU判断*/
	Err_CALIPOS			/* 初始角标定错误		CPU判断*/
} Err_t;
typedef enum
{
	//Warn_NOERR = 0x00,
	Warn_HPOV= 0x00,		/* 高压过压信号，		CPU判断，对应HPOvWarnPoint*/
	Warn_HPUV,				/* 高压欠压信号，		CPU判断，对应HPUvWarnPoint*/
	Warn_LPOV,				/* 低压过压信号，		CPU判断，对应LPOvWarnPoint*/
	Warn_LPUV,				/* 低压欠压信号，		CPU判断，对应LPUvWarnPoint*/
	Warn_HPOC,				/* 高压过流信号，		CPU判断，对应HPOcWarnPoint*/
	Warn_PDDOVT,			/* 功率管过温信号，		CPU判断，对应PDDOvtWarnPoint*/
	Warn_MOTOROVT,		/* 电机过温信号，		CPU判断，对应MotorOvtWarnPoint*/
	Warn_SOFTOC,			/* 软件判断过流信号，	CPU判断，对应PhaseOcWarnPoint*/
	Warn_OVSPD,			/* 超速信号，				CPU判断，对应OvSpdWarnPoint*/
	Warn_UNSPD,			/* 无感转速过低信号，	CPU判断，对应SnsLessUnSpdWarnPoint*/
	Warn_EEDATAINVLID,		/* 加载失败		CPU判断*/
	Warn_EEPROMFAIL		/* 存储失败		CPU判断*/
} Warn_t;

extern bool  SignalMakeSure(Uint16 MakeSure_Id, bool  InputData,Uint16 MakesureTimes);
/*extern void UpDateCriticalSampleData(stCriticalPara* pC);
extern Uint32  MC_DoFaultCheck(stSysCfgPara* pCfg,stSysState* pState,stFaultPara* pF,stCriticalPara* pC);
extern Uint32  MC_DoWarnCheck(stSysCfgPara* pCfg,stSysState* pState,stWarnPara* pW,stCriticalPara* pC);*/
extern void MC_PostCodeErr(Uint16 ErrCode);
extern void MC_PostCodeErr(Uint16 ErrCode);
extern void MC_PostCodeWarn(Uint16 ErrCode);
#endif

