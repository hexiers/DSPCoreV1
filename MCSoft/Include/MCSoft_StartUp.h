/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����
* @������		:  	����                                                                              
* ��������		:   	2020-03-07                                                                 
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#include"MC_Include.h"
#ifndef STARTUP_H_H
#define STARTUP_H_H

#define ADDNUMGAIN	(65536L<<8)//�ۼ����ķŴ�����ADDNUMGAIN��Ӧ����360

//�����˲����ݽṹ��
typedef struct
{
	INT32S OldValue;
	INT16S NewValueCoef;
	INT16S OldVaueCoef;
}stIir,*stpIir;

typedef  struct 
{
	Uint32 StartupTimeInms;	/* ����ʱ��*/
	Uint16 StartupRpm1;		/* ������ʼת��*/
	Uint16 StartupRpm2;		/* ��������ת��*/
	Uint32 StartupAddTimes;	/* �����ۼƴ���*/
	INT32S StartupSpd1Used;	/* ��ʼ�ۼ���*/
	INT32S StartupSpd2Used;	/* �����ۼ���*/
	INT32S StartupSpdDelta;		/* �ٶ��ۼ�����*/
	INT32S StartupAccDelta;		/*  �ٶ��ۼӵ�����*/
	Uint32 PosAdd;			/*  λ���ۼӼĴ���*/
 	Uint32 AlignCallTimes;		/*  Ԥ��λִ�������ۼ���*/
	Uint32 AlignSetTimes;		/*  �趨��Ԥ��λִ��������*/
	Uint32 StartUpCnt;			/*  ����ִ�������ۼ���*/
 	INT32S StartupUs1;		/*  ������1/4Ŀ���ٶ�ʱ�̵������ѹ(����ֵ)*/
	INT32S StartupUs2;		/*  ������Ŀ���ٶ�ʱ�̵������ѹ(����ֵ)*/
	INT32S StartupUs;			/*  �������������ѹʵʱֵ(����ֵ)*/
	bool 	StartUpSuccess;		/*  �����ɹ���ʶ*/
	INT32S HalfStartSpdUsed1P256;/* ����ת�ٵ�һ��ȷֳ�256�ݣ���Ӧ��Bit*/
	INT16S PosCoef;
 
} stStartup;

extern stStartup m1StartUp;
extern stIir StartupUsIIR;
extern void IirInit(stpIir stp,INT32S FilterCoef);
extern INT16S IirCalc(stpIir stp,INT16S DataIn);
extern bool MC_InitStartUp(stCtrlPara* pCtrl,stSnsLessPara* pSns,stMotorPara*pMotor,stStartup* pStartup);
//extern Uint16 MC_DoStartUp(stStartup* pStartup,stSysState* pState);
#endif

