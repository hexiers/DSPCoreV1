#include"MC_Include.h"
#ifndef SmcObserverExt_H_H
#define SmcObserverExt_H_H

#include"IQmathLib.h"

typedef struct
{
	INT32S UAlpha_UseQ16;   /*Alpha���ѹ��Q16��ʽ*/
	INT32S UBeta_UseQ16;    /*Beta���ѹ��Q16��ʽ*/
	INT16S fsmopos;	        /*f����*/  				
	INT16S gsmopos;			/*g����*/  		
	INT64S Alpha_F_ResultQ16;   /*f������˺�Ľ����Alpha�ᣬQ16��ʽ*/  	
	INT64S Alpha_G_ResultQ16;	/*g������˺�Ľ����Alpha�ᣬQ16��ʽ*/  
	INT32S EstIalphaQ16;    /*Alpha����������Q16��ʽ*/  
	INT64S Beta_F_ResultQ16;	/*f������˺�Ľ����Beta�ᣬQ16��ʽ*/  
	INT64S Beta_G_ResultQ16;	/*f������˺�Ľ����Beta�ᣬQ16��ʽ*/  
	INT32S EstIbetaQ16;		/*Beta����������Q16��ʽ*/
	INT32S IAlpha_UseQ16;	/*Alpha�������Q16��ʽ*/
	INT32S IBeta_UseQ16;	/*Beta�������Q16��ʽ*/
	INT32S IAlphaErrQ16;	/*Alpha�������Q16��ʽ*/
	INT32S IBetaErrQ16;		/*Beta�������Q16��ʽ*/
	INT16S SlideGain;		/*��ģ����������*/					
	INT32S SlideMax;		/*��ģ��������������*/			
	INT64S zAlphaQ16;       /*��ǰ���ڵĻ�ģ�������������Alpha�ᣬQ16��ʽ*/	
	INT32S zAlphaOldQ16;	/*��һ���ڵĻ�ģ�������������Alpha�ᣬQ16��ʽ*/	
	INT32S eAlphaQ16; 		/*��ǰ���ڵķ����ƣ�Alpha�ᣬQ16��ʽ*/	
	INT32S eAlphaOldQ16; 	/*��һ���ڵķ����ƣ�Alpha�ᣬQ16��ʽ*/
	INT64S zBetaQ16; 		/*��ǰ���ڵĻ�ģ�������������Beta�ᣬQ16��ʽ*/	
	INT32S zBetaOldQ16; 	/*��һ���ڵĻ�ģ�������������Beta�ᣬQ16��ʽ*/
	INT32S eBetaQ16; 		/*��ǰ���ڵķ����ƣ�Beta�ᣬQ16��ʽ*/	
	INT32S eBetaOldQ16; 	/*��һ���ڵķ����ƣ�Beta�ᣬQ16��ʽ*/
	INT32S eAlphaFinalQ16; 	/*�����˲���ķ����ƣ�Alpha�ᣬQ16��ʽ*/	
	INT32S eBetaFinalQ16; 	/*�����˲���ķ����ƣ�Beta�ᣬQ16��ʽ*/	
	INT16S	kslfMin;		/*����Ӧ�˲����˲�ϵ����С����ֵ*/			
	INT16S	BitPCycleToElecRpmCoef; 		/*�����ڽǶ�ֵ������Ӧ����ת�ٵ�ת��ϵ��*/			
	INT32S  thetaQ16;		/*�����Ƕ�ֵ��Q16��ʽ*/			
	INT16S	thetaINT;		/*��ǰ���ڵĵ����Ƕ�ֵ�������ʽ*/			
	INT16S	thetaINTOld;	/*��һ���ڵĵ����Ƕ�ֵ�������ʽ*/				
	INT16S  DeltaTheta[16];	/*�����ڵ�Ƕ�ֵ�����������ʽ������16�����ڵĻ���ƽ���˲�*/			
	INT16S  ArrayIndex;		/*����������*/		
	INT32S  DeltaThetaSum;	/*16�����ڵĵ�Ƕ������ͣ������ʽ*/		
	INT32S  OmegaBitQ16;	/*��ǰ���ڵĵ�Ƕ�������Q16��ʽ*/	
	INT32S  OmegaBitOldQ16;	/*��һ���ڵĵ�Ƕ�������Q16��ʽ*/	
	INT32S  OmegaBitFilterdQ16;	/*�˲���ĵ�Ƕ�������Q16��ʽ*/	
	INT16S	thetaOffset;		/*�貹���ĽǶ�ֵ*/		
	INT16S	SmcElecPos; 		/*�۲����ת�ӵ�Ƕ�*/			
	INT16S	Kslf;				/*��ģ�۲�������Ӧ�˲������˲�ϵ��*/					 
	INT16S	SmcElecRpm; 		/*����ת��*/				

}stSmcObserverExt;

extern   stSmcObserverExt  m1SmcObserverExt;
extern void SMC_Position_Estimation(void);
//extern bool SMC_Init(stSmcObserverExt* pT,stCtrlPara* pCtrl,stMotorPara*pMotor,stHalPara*pHal,stSnsLessPara*pSnsLess);

#endif











