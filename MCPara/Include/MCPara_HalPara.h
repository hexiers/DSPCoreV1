/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore 硬件设计相关参数结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#include"MC_Include.h"
#ifndef HALPARA_H_H
#define HALPARA_H_H

typedef struct
{
	Uint16	BitTo10mVCoef;//
	Uint16	F10mVToBitCoef;//
	Uint16	BitTo10mACoef;
	Uint16	F10mAToBitCoef;//
	Uint16	RatedVolBit;
	Uint16	BitToRpmCoef;
	Uint16	UCurrOff;//
	Uint16	VCurrOff;
	Uint16	WCurrOff;//
	INT16S	UVBemfOff;
	INT16S	VWBemfOff;
	INT16S	VBusOff;
	INT16S	IBusOff;
	Uint16	ADGain;
	INT16S	ADOff;

}stHalPara;
extern   stHalPara m1HalPara;
#endif

