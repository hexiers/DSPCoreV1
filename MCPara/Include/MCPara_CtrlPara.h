/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����MCCore ���Ʋ����ṹ��
* @������		:  	����                                                                              
* ��������		:   	2020-03-03                                                                 
* �޸ļ�¼		:   
*                         2020-10-10:�ڿ��Ʋ������������Ų���
======================================================================================================*/

#include"MC_Include.h"
#ifndef CTRLPARA_H_H
#define CTRLPARA_H_H

typedef struct
{
	INT16S	IdKp;
	INT16S	IdKi;
	INT16S	IqKp;
	INT16S	IqKi;
	INT16S	SpdInitKp;
	INT16S	SpdVaryKpStartErr_rpm;/* Ԥ��*/
	INT16S	SpdVaryKpEndErr_rpm;/* Ԥ��*/
	INT16S	SpdVaryKpGain;/* Ԥ��*/
	INT16S	SpdKi;
	INT16S	PGenLimCurr_10mA;
	INT16S	PGenLimSpd_rpm;
	Uint16	PGenLimVol_10mV;
	Uint16	PwmPeriod;
	Uint16	DeadTime;
	Uint16	CLoopsPerSpdLoopPow;/* Ԥ��*/
	Uint16	SpdFdbFilterHz;
	Uint16	ZeroSpdPoint_rpm;
	Uint16	SpdSlpOnems;
	Uint16 	FwKp;
	Uint16 	FwKi;
}stCtrlPara;
extern   stCtrlPara m1CtrlPara;

#endif
