/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore 控制参数结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         2020-10-10:在控制参数中增加弱磁参数
======================================================================================================*/

#include"MC_Include.h"
#ifndef CTRLPARA_H_H
#define CTRLPARA_H_H

typedef struct
{
	INT16S	IdKp;
	INT16S	IdKi;
	INT16S	IqKp;
	INT16S	IqKi;
	INT16S	SpdInitKp;
	INT16S	SpdVaryKpStartErr_rpm;/* 预留*/
	INT16S	SpdVaryKpEndErr_rpm;/* 预留*/
	INT16S	SpdVaryKpGain;/* 预留*/
	INT16S	SpdKi;
	INT16S	PGenLimCurr_10mA;
	INT16S	PGenLimSpd_rpm;
	Uint16	PGenLimVol_10mV;
	Uint16	PwmPeriod;
	Uint16	DeadTime;
	Uint16	CLoopsPerSpdLoopPow;/* 预留*/
	Uint16	SpdFdbFilterHz;
	Uint16	ZeroSpdPoint_rpm;
	Uint16	SpdSlpOnems;
	Uint16 	FwKp;
	Uint16 	FwKi;
}stCtrlPara;
extern   stCtrlPara m1CtrlPara;

#endif
