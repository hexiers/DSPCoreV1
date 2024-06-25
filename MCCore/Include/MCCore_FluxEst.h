/*======================================================================================================
* @�ļ�����	:
* @�ļ�����	: 	����MCCore _FluxEst�ṹ��
* @������		:  	��Ѱ
* ��������		:   	2020-03-06
* �޸ļ�¼		:
*
======================================================================================================*/

#ifndef MCCORE_INCLUDE_MCCORE_FLUXEST_H_
#define MCCORE_INCLUDE_MCCORE_FLUXEST_H_

#include"IQmathLib.h"

typedef struct
{
	float Udaxis;   		/*���������d���ѹ����ʵֵ*/
	float Uqaxis;    		/*���������q���ѹ����ʵֵ*/
	float IdFbk;   			/*���������d���������ʵֵ*/
	float IqFbk;    		/*���������q���������ʵֵ*/

//**************** �Աջ�************************************
	float UAlpha;   		/*���������alpha�����ѹ����ʵֵ*/
	float UBeta;    		/*���������beta�����ѹ����ʵֵ*/
	float IAlphaFbk;   		/*���������alpha���������ʵֵ*/
	float IBetaFbk;    		/*���������beta���������ʵֵ*/

	INT16S IAlphaFbk_Bit;   /*���������alpha�������bitֵ*/
	INT16S IBetaFbk_Bit;    /*���������beta�������bitֵ*/

	float CosTheta;			/*�Ƕȼ���ֵ*/
	float SinTheta;
	float Theta;

	float FluxEst_ElecSpd; 	/*�������������ת�� rad/s */
	float sum;
	float Ave_we;
	float Rec_ElecSpd[20]; 	/*��¼����20������ת�� rad/s */
	float FluxEst_Angle;    /*��������������Ƕ� rad*/
	float Idref;    		/*���������d��ο���������ʵֵ*/
	float Iqref;    		/*���������d��ο���������ʵֵ*/

	float Edaxis;   		/*d�������ѹ����ʵֵ*/
	float Eqaxis;    		/*q�������ѹ����ʵֵ*/

	float MOT_Rs;   		/*������裬��ʵֵ */
	float MOT_Rs0;   		/*��������ʼֵ����ʵֵ */
	float MOT_Ls;    		/*�����У���ʵֵ*/
	float MOT_Ls0;			/*�����г�ʼֵ*/
	float MOT_faif;			/*���ת�����Ŵ�������ʵֵ */
	float MOT_MaxCurr;		/*���������ֵ����ʵֵ */
	float MOT_MaxOmega;		/*���������ٶ� rad/s����ʵֵ */
	//float MOT_faif2;		/*���ת�Ӵ���ƽ��*/

	float PAR_Alpha;    	/* ��ͨ�˲������棬��ʵֵ  */
	float PAR_Alpha0;		/* ��ͨ�˲������棬��ʼֵ  */

	float PAR_Lambda0;		/* ����Ӧ��ͨ�˲������棬��ʼֵ  */
	float PAR_Lambfactor;	/* ����Ӧ��ͨ�˲�������仯��  */
	float PAR_Lambda;    	/* ����Ӧ��ͨ�˲������棬��ʵֵ  */

	float PAR_LsFactor;    	/* ��в���������������仯��  */
	float PAR_RsFactor;    	/* ��������������������仯��  */

	float PAR_MotDir;		/* ���ת��1��ת��-1��ת��0��ֹ */
	float tem;				/* �м�������жϳ������ٽ��  */
	float PAR_Wlim;			/* �������ٽ�㣬rad/s  */

	float Ts_Cal;    		/*����ʱ�䣬��*/
	float VDC_V;    		/*ĸ�ߵ�ѹ V*/

	INT16S DEALTA;					/* λ���������ʹ������۲�Ƚ� */
	INT16S Selfsensing_Enable;		/* ��λ�ü���ʹ�� */

	char TdComEn;

//	/*�����Թ۲���*/
//	float y_Alpha;
//	float y_Beta;
//	float r;
//	float px_Alpha;
//	float px_Beta;
//
//	float x_Alpha;
//	float x_Beta;
//
//	float delta_f_2;
//
//	float Com_Alpha;
//	float Com_Beta;
//
//	float fais_Alpha;
//	float fais_Beta;
//
//	/*PLL*/
//	float up;
//	float kp;
//	float ki;
//	float ui;
//	float v1;
//	float i1;
//	float Out;
//	float Out_max;
//	float Out_min;
//	float tem1;
//	float tem2;
//
//	INT16S Enable;

}Flux_Est;

extern   Flux_Est  m1FluxObserver;
extern void FluxPosEst(Flux_Est *pT, stCurrLoopCtler *pD);


#endif /* MCCORE_INCLUDE_MCCORE_FLUXEST_H_ */
