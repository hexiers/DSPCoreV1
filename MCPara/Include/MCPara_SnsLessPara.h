/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore 无位置控制相关参数结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#ifndef SNSLESSPARA_H_H
#define SNSLESSPARA_H_H

#include"MC_Include.h"


typedef struct
{
	Uint16	StartUpSpd;
	Uint16	StartUpTime_ms;//
	Uint16	StartUpCurr_10mA;
	Uint16	SmcGain;
	Uint16 	SpdFilterBand_Hz;
	Uint16 	AlignTime_ms;

    Uint16 HfiFreqHz; // V1.8
    Uint16 HfiVol; // V1.8
    Uint16 BoundHighFreqPassHz; // V1.8
    Uint16 BoundLowFreqPassHz; // V1.8
    Uint16 LowFreqPassHz; // V1.8
    Uint16 PllKpQ12; // V1.8
    Uint16 PllKiQ16; // V1.8
    Uint16 CurrKp; // V1.8
    Uint16 CurrKi; // V1.8
}stSnsLessPara;

extern   stSnsLessPara m1SnsLessPara;

#endif
