/*======================================================================================================
* @文件名称	:
* @文件描述	: 	定义MCCore _FluxEst结构体
* @创建人		:  	韩寻
* 创建日期		:   	2020-03-06
* 修改记录		:
*
======================================================================================================*/

#ifndef MCCORE_INCLUDE_MCCORE_FLUXEST_H_
#define MCCORE_INCLUDE_MCCORE_FLUXEST_H_

#include"IQmathLib.h"

typedef struct
{
	float Udaxis;   		/*输入变量：d轴电压，真实值*/
	float Uqaxis;    		/*输入变量：q轴电压，真实值*/
	float IdFbk;   			/*输入变量：d轴电流，真实值*/
	float IqFbk;    		/*输入变量：q轴电流，真实值*/

//**************** 自闭环************************************
	float UAlpha;   		/*输入变量：alpha轴轴电压，真实值*/
	float UBeta;    		/*输入变量：beta轴轴电压，真实值*/
	float IAlphaFbk;   		/*输入变量：alpha轴电流，真实值*/
	float IBetaFbk;    		/*输入变量：beta轴电流，真实值*/

	INT16S IAlphaFbk_Bit;   /*输入变量：alpha轴电流，bit值*/
	INT16S IBetaFbk_Bit;    /*输入变量：beta轴电流，bit值*/

	float CosTheta;			/*角度计算值*/
	float SinTheta;
	float Theta;

	float FluxEst_ElecSpd; 	/*输出变量：电气转速 rad/s */
	float sum;
	float Ave_we;
	float Rec_ElecSpd[20]; 	/*记录变量20个电气转速 rad/s */
	float FluxEst_Angle;    /*输出变量：电气角度 rad*/
	float Idref;    		/*输出变量：d轴参考电流，真实值*/
	float Iqref;    		/*输出变量：d轴参考电流，真实值*/

	float Edaxis;   		/*d轴磁链电压，真实值*/
	float Eqaxis;    		/*q轴磁链电压，真实值*/

	float MOT_Rs;   		/*电机电阻，真实值 */
	float MOT_Rs0;   		/*电机电阻初始值，真实值 */
	float MOT_Ls;    		/*电机电感，真实值*/
	float MOT_Ls0;			/*电机电感初始值*/
	float MOT_faif;			/*电机转子永磁磁链，真实值 */
	float MOT_MaxCurr;		/*电机最大电流值，真实值 */
	float MOT_MaxOmega;		/*电机最大电角速度 rad/s，真实值 */
	//float MOT_faif2;		/*电机转子磁链平方*/

	float PAR_Alpha;    	/* 低通滤波器增益，真实值  */
	float PAR_Alpha0;		/* 低通滤波器增益，初始值  */

	float PAR_Lambda0;		/* 自适应高通滤波器增益，初始值  */
	float PAR_Lambfactor;	/* 自适应高通滤波器增益变化率  */
	float PAR_Lambda;    	/* 自适应高通滤波器增益，真实值  */

	float PAR_LsFactor;    	/* 电感参数修正，电感最大变化率  */
	float PAR_RsFactor;    	/* 电阻参数修正，电阻最大变化率  */

	float PAR_MotDir;		/* 电机转向，1正转，-1反转，0静止 */
	float tem;				/* 中间变量，判断超低速临界点  */
	float PAR_Wlim;			/* 超低速临界点，rad/s  */

	float Ts_Cal;    		/*计算时间，秒*/
	float VDC_V;    		/*母线电压 V*/

	INT16S DEALTA;					/* 位置误差，用作和传感器观测比较 */
	INT16S Selfsensing_Enable;		/* 无位置计算使能 */

	char TdComEn;

//	/*非线性观测器*/
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
