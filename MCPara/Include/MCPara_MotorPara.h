/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����MCCore ��������ṹ��
* @������		:  	����                                                                              
* ��������		:   	2020-03-03                                                                 
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#ifndef MOTORPARA_H_H
#define MOTORPARA_H_H

#include"MC_Include.h"

typedef struct
{
	INT16S	PhaseRes_100uohm;
	INT16S	PhaseIndD_uH;
	//INT16S	RotFluxfactor_Wb;
	//INT16S	RatedAngSpd;
	INT16S	MotorPoles;
	INT16S	SnsorPoles;// MotorPolesOverResolverPoles
	INT16S	RatedCurrentQ_10mA;
	INT16S	RatedCurrentD_10mA;
	Uint16	PosOff;
	INT16S	RatedSpd;
}stMotorPara;

typedef struct
{
	INT16S	PhaseRes_100uohm;
	INT16S	PhaseIndD_uH;
	//INT16S	MotorPoles;
	//INT16S	SnsorPoles;
	//INT16S	CalibCurrentQ_10mA;
	INT16S	CalibCurrentD_10mA;
	Uint16  CurrSampleCnt;
	Uint16  CurrLoopSampleTime_us;
	//Uint16	PosOff;
	//INT16S	RatedSpd;
	INT16S SampleCurr_10mA[16];
	INT32S RCalibVol1_10mV;
	INT32S RCalibVol2_10mV;
}stMotorCalib;

extern   stMotorPara m1MotorPara;
extern   stMotorCalib m1MotorCalib;

#endif
