/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-07                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#include"MC_Include.h"
#ifndef STARTUP_H_H
#define STARTUP_H_H

#define ADDNUMGAIN	(65536L<<8)//累加数的放大倍数，ADDNUMGAIN对应电气360

//惯性滤波数据结构体
typedef struct
{
	INT32S OldValue;
	INT16S NewValueCoef;
	INT16S OldVaueCoef;
}stIir,*stpIir;

typedef  struct 
{
	Uint32 StartupTimeInms;	/* 启动时间*/
	Uint16 StartupRpm1;		/* 启动开始转速*/
	Uint16 StartupRpm2;		/* 启动结束转速*/
	Uint32 StartupAddTimes;	/* 启动累计次数*/
	INT32S StartupSpd1Used;	/* 开始累计数*/
	INT32S StartupSpd2Used;	/* 结束累加数*/
	INT32S StartupSpdDelta;		/* 速度累加总量*/
	INT32S StartupAccDelta;		/*  速度累加单步量*/
	Uint32 PosAdd;			/*  位置累加寄存器*/
 	Uint32 AlignCallTimes;		/*  预定位执行周期累加数*/
	Uint32 AlignSetTimes;		/*  设定的预定位执行周期数*/
	Uint32 StartUpCnt;			/*  启动执行周期累加数*/
 	INT32S StartupUs1;		/*  启动至1/4目标速度时刻的输出电压(近似值)*/
	INT32S StartupUs2;		/*  启动至目标速度时刻的输出电压(近似值)*/
	INT32S StartupUs;			/*  启动过程输出电压实时值(近似值)*/
	bool 	StartUpSuccess;		/*  启动成功标识*/
	INT32S HalfStartSpdUsed1P256;/* 启动转速的一半等分成256份，对应的Bit*/
	INT16S PosCoef;
 
} stStartup;

extern stStartup m1StartUp;
extern stIir StartupUsIIR;
extern void IirInit(stpIir stp,INT32S FilterCoef);
extern INT16S IirCalc(stpIir stp,INT16S DataIn);
extern bool MC_InitStartUp(stCtrlPara* pCtrl,stSnsLessPara* pSns,stMotorPara*pMotor,stStartup* pStartup);
//extern Uint16 MC_DoStartUp(stStartup* pStartup,stSysState* pState);
#endif

