/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore 故障阈值参数结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#ifndef FAULTPARA_H_H
#define FAULTPARA_H_H

#include"MC_Include.h"


typedef struct
{
	Uint16	HPOvFaultPoint;/* 转换成BIT值，写入FPGA*/
	Uint16	HPUvFaultPoint;/* 转换成BIT值，写入FPGA*/
	Uint16	LPOvFaultPoint;
	Uint16	LPUvFaultPoint;
	Uint16	HPOcFaultPoint;
	Uint16	PDDOvtFaultPoint;
	Uint16	PhaseOcFaultPoint;
	Uint16	OvSpdFaultPoint;
	Uint16	SnsLessUnSpdFaultPoint;
	Uint16	MotorOvtFaultPoint;

}stFaultPara;

extern   stFaultPara m1FaultPara;

#endif
