/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore _BusVolDecoupler结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-06                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#include"MC_Include.h"
#ifndef BusVolDecoupler_H_H
#define BusVolDecoupler_H_H


typedef struct
{
	INT16S 	RatedVolBit;		//	0	1	Cfg	R/W-2000	
	INT16S 	V_Bus;			//	1	1	Output	R-0	
	INT16S 	VBusProtect;		//	2	1	Output	R-0	
	INT16S 	BusCoef;			//	3	1	Output	R-0	

}stBusVolDecoupler;

extern   stBusVolDecoupler m1BusVolDecoupler;
extern bool DSPCore_CfgBusVolDecoupler(stBusVolDecoupler* pBus);
extern bool DSPCore_DoBusVolDecoupler(stBusVolDecoupler* pBus);

#endif
