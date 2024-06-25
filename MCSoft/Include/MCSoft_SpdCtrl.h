/*
 * MCSoft_SpdCtrl.h
 *
 *  Created on: 2020年9月8日
 *      Author: xub
 */

#include"MC_Include.h"
#ifndef MCSOFT_SPDCTRL_H_
#define MCSOFT_SPDCTRL_H_

/*
typedef struct
{
	INT16S	SpdElecCmd;		//	0	1	Ctl	R/W-0	速度指令寄存器
	INT16S	SpdKp;			//	1	1	Cfg	R/W-0
	INT16S	SpdKi;			//	2	1	Cfg	R/W-0
	INT16S	SpdOutMax;		//	3	1	Cfg	R/W-0
	INT16S	SpdOutMin;		//	4	1	Cfg	R/W-0
	INT16S	SpdIntergPreset;//	5	1	Cfg	R/W-0
	INT16S	SpdFdbElecRpm;	//	6	1	Output	R-0
	INT16S	SpdProp;		//	7	1	Output	R-0
	INT16S	SpdIntergLim;	//	8	1	Output	R-0
	INT16S	SpdPIOut;		//	9	1	Output	R-0
	INT16S	SpdLimOut;		//	10	1	Output	R-0
	INT16S	DisChargeSpd;	//	11	1	Output	R-0
	INT32S	SpdIntergLim32;	//	8	1	Output	R-0
	

}stCpuSpdLoop;

extern stCpuSpdLoop d1SpeedLoop;
extern bool SpdLoopInit(stCpuSpdLoop* pD,stCtrlPara* pCtrl,stMotorPara*pMotor,stHalPara*pHal);
extern void SpdLoopCalc(stCpuSpdLoop *p,INT32S iError,bool AntiRstEn,bool SpdCtrlEn); */
#endif /* MCSOFT_SPDCTRL_H_ */
