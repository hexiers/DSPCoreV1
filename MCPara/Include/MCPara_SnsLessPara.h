/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����MCCore ��λ�ÿ�����ز����ṹ��
* @������		:  	����                                                                              
* ��������		:   	2020-03-03                                                                 
* �޸ļ�¼		:   
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
