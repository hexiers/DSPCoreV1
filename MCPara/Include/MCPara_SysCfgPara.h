/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore 配置参数结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#ifndef SYSCFGPARA_H_H
#define SYSCFGPARA_H_H

#include"MC_Include.h"


struct SysCfgWord1_BIT {
	Uint16  	EnBridgeCheck:1;//
	Uint16   	EnSnsLess:1;//
	Uint16   	EnDacPrint:1;
	Uint16  	EnUsrSpdLoopPara:1;
	Uint16   	EnUsrCurrLoopPara:1;
	Uint16   	EnNoZeroSpdStart:1;//
	Uint16 	    DoNotSwitch:1;/* 开环启动完不转闭环*/
	Uint16   	EnBusMod:1;/* 母线电压修正功能开关*/
	Uint16   	EnHfi:1;/* 高频注入开启开关*/
	Uint16   	rsvd:7;
};

union  SysCfgWord1_REG {
   Uint16             all;
   struct SysCfgWord1_BIT  bit;
};

typedef struct
{
	union  SysCfgWord1_REG SysCfgWord1;
	Uint16 SysCfgWord2;
	Uint16 DA1Sel;
	Uint16 DA2Sel;
	
}stSysCfgPara;

extern   stSysCfgPara m1SysCfgPara;

#endif
