/*
 * MCSoft_FluxEst.c
 *
 *  Created on: 2020年11月16日
 *      Author: hanxun
 */

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "MCCore_FluxEst.h"
#include "math.h"
#include"MCSoft_Math.h"
/**************************************************************************************************************************************************
* @函数名		:  	FluxObs
* @函数描述	: 	磁链观测器观测转速
* @创建人		:  	韩寻
* 创建日期		:   2020-11-16
* 修改记录		:
*
****************************************************************************************************************************************************/
void FluxPosEst (Flux_Est * pT, stCurrLoopCtler *pD)
{
		//********************	电流电压转换, 将Bit值转化为真实值     *********************
	 	 pT->Theta=pT->FluxEst_Angle;
	 	 pT->CosTheta=cos(pT->Theta);
	 	 pT->SinTheta=sin(pT->Theta);

	 	 pT->UAlpha=(float)pD->UAlpha_V/8192.0f * m1CriPara.HpVol*0.01 * 0.57735027;// [-16384,16383]对应后面VDC/SQRT(3)
	 	 pT->UBeta=(float)pD->UBeta_V/8192.0f * m1CriPara.HpVol*0.01* 0.57735027;
	 	 pT->IAlphaFbk_Bit = (INT32S)((INT32S)pD->IFdbAlpha * (Uint32)m1HalPara.BitTo10mACoef) >>12;
	 	 pT->IAlphaFbk= (float)pT->IAlphaFbk_Bit * 0.01;
	 	 pT->IBetaFbk_Bit = (INT32S)((INT32S)pD->IFdbBeta * (Uint32)m1HalPara.BitTo10mACoef) >>12;
	 	 pT->IBetaFbk = (float)pT->IBetaFbk_Bit * 0.01;

	 	 pT->Udaxis = pT->UAlpha * pT->CosTheta + pT->UBeta * pT->SinTheta;
	 	 pT->Uqaxis = -pT->UAlpha* pT->SinTheta + pT->UBeta * pT->CosTheta;
	 	 pT->IdFbk=	pT->IAlphaFbk*pT->CosTheta + pT->IBetaFbk*pT->SinTheta;
	 	 pT->IqFbk=	-pT->IAlphaFbk*pT->SinTheta + pT->IBetaFbk*pT->CosTheta;


	//********************  是否采用指令电流   ******************************************
	if(pT->Selfsensing_Enable==1)//{pT->IdFbk=pT->Idref;pT->IqFbk=pT->Iqref;}
	{
			//  电机转向判断
			pT->PAR_MotDir = (pT->FluxEst_ElecSpd>0)?1:-1;
			if(pT->FluxEst_ElecSpd==0) pT->PAR_MotDir=0;

	//******************** 观测器参数更新：启动增益调整与电感变化调整  ******************************************
			//低速下注入d 轴电流
			if (fabs(pT->FluxEst_ElecSpd)> pT->PAR_Wlim)  pT->Idref=0;
			else pT->Idref = pT->Iqref / pT->PAR_Lambda * pT->PAR_MotDir;

			// 启动时增益更新，低速下增益Lambda要大，高速逐步减小，趋于恒定
			//20次速度观测值调整一次，避免速度观测不准的影响
			static INT16S x;
			float *temp = pT->Rec_ElecSpd;
			pT->Ave_we=pT->sum/20; 		//20次平均速度

			pT->tem=fabs(pT->FluxEst_ElecSpd)/ pT->PAR_Wlim;
			if (pT->tem> 1)  pT->tem =1;
			pT->PAR_Lambda=pT->PAR_Lambda0*(1-pT->PAR_Lambfactor*pT->tem);

			// 电感调整，大电流时电感逐渐减小，参数需要修正，仅在高速下蔡使能电感调整项
			if (fabs(pT->Ave_we)> pT->PAR_Wlim)
			pT->MOT_Ls=pT->MOT_Ls0*(1-pT->PAR_LsFactor*fabs(pT->IqFbk)/pT->MOT_MaxCurr);

			//电阻调整，温度变化大时修正电阻

	//******************** 		磁链电压计算 & 系数更新 	 *************************************
	pT->Edaxis=pT->Udaxis - pT->MOT_Rs*pT->IdFbk + pT->FluxEst_ElecSpd*pT->MOT_Ls*pT->IqFbk;
	pT->Eqaxis=pT->Uqaxis - pT->MOT_Rs*pT->IqFbk - pT->FluxEst_ElecSpd*pT->MOT_Ls*pT->IdFbk;
	pT->PAR_Alpha=pT->PAR_Alpha0+2*pT->PAR_Lambda*fabs(pT->FluxEst_ElecSpd);
	//******************** 		转速计算 & 角度更新 	 *************************************
	pT->FluxEst_ElecSpd += pT->Ts_Cal*pT->PAR_Alpha*((pT->Eqaxis - pT->PAR_Lambda*pT->PAR_MotDir*pT->Edaxis)/pT->MOT_faif - pT->FluxEst_ElecSpd);
//	if(pT->FluxEst_ElecSpd>3000)
//		pT->FluxEst_ElecSpd=3000;
//	if(pT->FluxEst_ElecSpd<-3000);
//		pT->FluxEst_ElecSpd=-3000;
	pT->FluxEst_Angle += pT->Ts_Cal*pT->FluxEst_ElecSpd;
	pT->FluxEst_Angle= (pT->FluxEst_Angle*0.15915494-floor(pT->FluxEst_Angle*0.15915494))*6.28318531; //角度规一到（0,2pi）区间


	x=(x<19)?++x:0;
	pT->sum=pT->sum-*(temp+x)+pT->FluxEst_ElecSpd;
	*(temp+x)=pT->FluxEst_ElecSpd;

//	pT->Ave_we=FirCalc(&Flux_EstSpdFdbFir,(pT->FluxEst_ElecSpd/pT->MOT_MaxOmega)<<25,5);

	//---------------------   非线性观测器     -----------------------------------------
//	if(pT->Enable==1)
//		{
////			pT->y_Alpha=pT->UAlpha-pT->MOT_Rs*pT->IAlphaFbk;
////			pT->y_Beta=pT->UBeta-pT->MOT_Rs*pT->IBetaFbk;
////
////			pT->delta_f_2=pT->MOT_faif*pT->MOT_faif-(pT->fais_Alpha*pT->fais_Alpha+pT->fais_Beta*pT->fais_Beta);
////
////			pT->Com_Alpha=0.5*pT->r *pT->fais_Alpha*pT->delta_f_2;
////			pT->Com_Beta=0.5*pT->r *pT->fais_Beta*pT->delta_f_2;
////
////			pT->px_Alpha=pT->y_Alpha+pT->Com_Alpha;
////			pT->px_Beta=pT->y_Beta+pT->Com_Beta;
////
////			pT->x_Alpha=pT->x_Alpha+pT->px_Alpha*pT->Ts_Cal;
////			pT->x_Beta=pT->x_Beta+pT->px_Beta*pT->Ts_Cal;
////
////			pT->fais_Alpha = pT->x_Alpha - pT->IAlphaFbk*pT->MOT_Ls;
////			pT->fais_Beta = pT->x_Beta - pT->IBetaFbk*pT->MOT_Ls;
//
////	//---------------------   非线性观测器2     -----------------------------------------
////
////				const INT16S iterations=5;
////				const float dt_iterations=pT->Ts_Cal/iterations;
////
////
////					pT->y_Alpha=pT->UAlpha-pT->MOT_Rs*pT->IAlphaFbk;
////					pT->y_Beta=pT->UBeta-pT->MOT_Rs*pT->IBetaFbk;
////
////				INT16S i=0;
////
////					for(i=0; i<iterations;i++)
////					{
////						pT->fais_Alpha = pT->x_Alpha - pT->IAlphaFbk*pT->MOT_Ls;
////						pT->fais_Beta = pT->x_Beta - pT->IBetaFbk*pT->MOT_Ls;
////						pT->delta_f_2=pT->MOT_faif2-(pT->fais_Alpha*pT->fais_Alpha+pT->fais_Beta*pT->fais_Beta);
////						float gamma=pT->r;
////						if(fabs(pT->delta_f_2) < (pT->MOT_faif2*0.2))
////							gamma=10;
////
////						pT->Com_Alpha=gamma *pT->fais_Alpha*pT->delta_f_2;
////						pT->Com_Beta=gamma *pT->fais_Beta*pT->delta_f_2;
////
////						pT->px_Alpha=pT->y_Alpha+pT->Com_Alpha;
////						pT->px_Beta=pT->y_Beta+pT->Com_Beta;
////
////						pT->x_Alpha+=pT->px_Alpha*dt_iterations;
////						pT->x_Beta+=pT->px_Beta*dt_iterations;
////					}
////
////					//pT->FluxEst_Angle=atan2(-pT->fais_Beta , pT->fais_Alpha);
////			//---------------------  PLL     -----------------------------------------
////			pT->up=(-pT->fais_Alpha*pT->SinTheta+pT->fais_Beta*pT->CosTheta)/pT->MOT_faif;
////			pT->ui=(pT->Out == pT->v1)?(pT->up*pT->ki+pT->i1):pT->i1;
////			pT->i1=pT->ui;
////			pT->v1=pT->up*pT->kp+pT->ui;
////			pT->Out=pT->v1;
////			if(pT->v1>pT->Out_max)
////				pT->Out=pT->Out_max;
////			if(pT->v1<pT->Out_min)
////				pT->Out=pT->Out_min;
////
////			pT->FluxEst_ElecSpd=pT->Out;
////			pT->FluxEst_Angle+=pT->Out*pT->Ts_Cal;
////			pT->FluxEst_Angle= (pT->FluxEst_Angle*0.15915494-floor(pT->FluxEst_Angle*0.15915494))*6.28318531;
////
////		}

	}
//	else
//		{
//			pT->FluxEst_Angle=0;
//			pT->FluxEst_ElecSpd=0;
//		}
}

/**************************************************************************************************************************************************
* @函数名		:  		FluxPos_Init
* @函数描述	: 		磁链观测器初始化
* @创建人		: 		韩寻
* 创建日期		:   2020-11-16
* 修改记录		:
*
****************************************************************************************************************************************************/
bool FluxPos_Init(Flux_Est* pT,stMotorPara*pMotor,stCtrlPara* pCtrl)
{

	float SampleFreq_Hz = 0;
	if((pT== NULL)||(pMotor == NULL)||(pCtrl==NULL))
	{
		return FALSE;
	}
	MemClr((Uint16*)&pT->Udaxis, (Uint16*)&pT->VDC_V);

	//===========================================================
	pT->MOT_Ls0 = 350*0.000001;		//电感初始值尽量往大预估
	pT->MOT_Rs0=0.06f;				//电阻初始值尽量往大预估
	pT->PAR_LsFactor=0.3;		//电感最大变化率
	pT->PAR_RsFactor=0;				//电阻最大变化率

	pT->MOT_Ls = pT->MOT_Ls0;
	pT->MOT_Rs= pT->MOT_Rs0;
	pT->MOT_faif=0.018f;			//永磁磁链，认为基本不变
	pT->MOT_MaxCurr=20;
	pT->MOT_MaxOmega=5000*3/60*2*PI;
	SampleFreq_Hz = PWMCLKFREQ/(2*pCtrl->PwmPeriod);
	pT->Ts_Cal =1.0f/SampleFreq_Hz;
	pT->PAR_Alpha0=600;

	pT->PAR_Lambda0=4;
	pT->PAR_Lambda=pT->PAR_Lambda0;
	pT->PAR_Lambfactor=0.5;					// 自适应参数变化最大值

	pT->PAR_MotDir=0;

	pT->Selfsensing_Enable=0;
	pT->PAR_Wlim=pT->MOT_MaxOmega*0.3;		// 30%的最大转速为高低速临界点
	pT->VDC_V=28;							// 母线电压
	pT->Theta=0;
	pT->DEALTA=0;
	pT->TdComEn=0;


/*	pT->r=2;
	pT->kp=20;
	pT->ki=10*pT->Ts_Cal;
	pT->Out_max=3000;
	pT->Out_min=-3000;
	pT->Enable=0;

	pT->Com_Alpha=0;
	pT->Com_Beta=0;
	pT->y_Alpha=0;
	pT->y_Beta=0;
	pT->px_Alpha=0;
	pT->px_Beta=0;
	pT->x_Alpha=0;
	pT->x_Beta=0;
	pT->delta_f_2=0;
	pT->fais_Alpha=0;
	pT->fais_Beta=0;
	pT->ui=0;
	pT->v1=0;
	pT->i1=0;
	pT->Out=0;

	pT->MOT_faif2=pT->MOT_faif*pT->MOT_faif;
*/
	return TRUE;

}
