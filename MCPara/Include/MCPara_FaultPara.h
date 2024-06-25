/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����MCCore ������ֵ�����ṹ��
* @������		:  	����                                                                              
* ��������		:   	2020-03-03                                                                 
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#ifndef FAULTPARA_H_H
#define FAULTPARA_H_H

#include"MC_Include.h"


typedef struct
{
	Uint16	HPOvFaultPoint;/* ת����BITֵ��д��FPGA*/
	Uint16	HPUvFaultPoint;/* ת����BITֵ��д��FPGA*/
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
