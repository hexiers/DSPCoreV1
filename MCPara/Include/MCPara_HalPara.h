/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����MCCore Ӳ�������ز����ṹ��
* @������		:  	����                                                                              
* ��������		:   	2020-03-03                                                                 
* �޸ļ�¼		:   
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

