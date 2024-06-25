/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����MCCore �澯��ֵ�����ṹ��
* @������		:  	����                                                                              
* ��������		:   	2020-03-03                                                                 
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#ifndef WARNPARA_H_H
#define WARNPARA_H_H

#include"MC_Include.h"


typedef struct
{
	Uint16	HPOvWarnPoint;
	Uint16	HPUvWarnPoint;// �����˳�PowerUpWait
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
