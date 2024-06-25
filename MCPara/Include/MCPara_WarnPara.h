/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore 告警阈值参数结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#ifndef WARNPARA_H_H
#define WARNPARA_H_H

#include"MC_Include.h"


typedef struct
{
	Uint16	HPOvWarnPoint;
	Uint16	HPUvWarnPoint;// 用作退出PowerUpWait
	Uint16	LPOvWarnPoint;
	Uint16	LPUvWarnPoint;
	Uint16	HPOcWarnPoint;
	Uint16	PDDOvtWarnPoint;
	Uint16	PhaseOcWarnPoint;
	Uint16	OvSpdWarnPoint;
	Uint16	SnsLessUvSpdWarnPoint;
	Uint16	MotorOvtWarnPoint;
}stWarnPara;

extern   stWarnPara m1WarnPara;
#endif
