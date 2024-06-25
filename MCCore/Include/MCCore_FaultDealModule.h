/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore _FaultDealModule结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-06                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#include"MC_Include.h"
#ifndef FaultDealModule_H_H
#define FaultDealModule_H_H

typedef struct
{
	Uint16 	OVPoint;//	0	1	Cfg	R/W-65535
	Uint16 	UVPoint;//	1	1	Cfg	R/W-0
}stFaultDealModule;

extern   stFaultDealModule m1FaultDealModule;
extern bool DSPCore_CfgFaultDealModule(stFaultDealModule* pR);
#endif
