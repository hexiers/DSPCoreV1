/*
 * MCSoft_FluxEst.c
 *
 *  Created on: 2020��11��16��
 *      Author: hanxun
 */

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "MCCore_FluxEst.h"
#include "math.h"
#include"MCSoft_Math.h"
/**************************************************************************************************************************************************
* @������		:  	FluxObs
* @��������	: 	�����۲����۲�ת��
* @������		:  	��Ѱ
* ��������		:   2020-11-16
* �޸ļ�¼		:
*
****************************************************************************************************************************************************/
void FluxPosEst (Flux_Est * pT, stCurrLoopCtler *pD)
{
		//********************	������ѹת��, ��Bitֵת��Ϊ��ʵֵ     *********************
	 	 pT->Theta=pT->FluxEst_Angle;
	 	 pT->CosTheta=cos(pT->Theta);
	 	 pT->SinTheta=sin(pT->Theta);

	 	 pT->UAlpha=(float)pD->UAlpha_V/8192.0f * m1CriPara.HpVol*0.01 * 0.57735027;// [-16384,16383]��Ӧ����VDC/SQRT(3)
	 	 pT->UBeta=(float)pD->UBeta_V/8192.0f * m1CriPara.HpVol*0.01* 0.57735027;
	 	 pT->IAlphaFbk_Bit = (INT32S)((INT32S)pD->IFdbAlpha * (Uint32)m1HalPara.BitTo10mACoef) >>12;
	 	 pT->IAlphaFbk= (float)pT->IAlphaFbk_Bit * 0.01;
	 	 pT->IBetaFbk_Bit = (INT32S)((INT32S)pD->IFdbBeta * (Uint32)m1HalPara.BitTo10mACoef) >>12;
	 	 pT->IBetaFbk = (float)pT->IBetaFbk_Bit * 0.01;

	 	 pT->Udaxis = pT->UAlpha * pT->CosTheta + pT->UBeta * pT->SinTheta;
	 	 pT->Uqaxis = -pT->UAlpha* pT->SinTheta + pT->UBeta * pT->CosTheta;
	 	 pT->IdFbk=	pT->IAlphaFbk*pT->CosTheta + pT->IBetaFbk*pT->SinTheta;
	 	 pT->IqFbk=	-pT->IAlphaFbk*pT->SinTheta + pT->IBetaFbk*pT->CosTheta;


	//********************  �Ƿ����ָ�����   ******************************************
	if(pT->Selfsensing_Enable==1)//{pT->IdFbk=pT->Idref;pT->IqFbk=pT->Iqref;}
	{
			//  ���ת���ж�
			pT->PAR_MotDir = (pT->FluxEst_ElecSpd>0)?1:-1;
			if(pT->FluxEst_ElecSpd==0) pT->PAR_MotDir=0;

	//******************** �۲����������£���������������б仯����  ******************************************
			//������ע��d �����
			if (fabs(pT->FluxEst_ElecSpd)> pT->PAR_Wlim)  pT->Idref=0;
			else pT->Idref = pT->Iqref / pT->PAR_Lambda * pT->PAR_MotDir;

			// ����ʱ������£�����������LambdaҪ�󣬸����𲽼�С�����ں㶨
			//20���ٶȹ۲�ֵ����һ�Σ������ٶȹ۲ⲻ׼��Ӱ��
			static INT16S x;
			float *temp = pT->Rec_ElecSpd;
			pT->Ave_we=pT->sum/20; 		//20��ƽ���ٶ�

			pT->tem=fabs(pT->FluxEst_ElecSpd)/ pT->PAR_Wlim;
			if (pT->tem> 1)  pT->tem =1;
			pT->PAR_Lambda=pT->PAR_Lambda0*(1-pT->PAR_Lambfactor*pT->tem);

			// ��е����������ʱ����𽥼�С��������Ҫ���������ڸ����²�ʹ�ܵ�е�����
			if (fabs(pT->Ave_we)> pT->PAR_Wlim)
			pT->MOT_Ls=pT->MOT_Ls0*(1-pT->PAR_LsFactor*fabs(pT->IqFbk)/pT->MOT_MaxCurr);

			//����������¶ȱ仯��ʱ��������

	//******************** 		������ѹ���� & ϵ������ 	 *************************************
	pT->Edaxis=pT->Udaxis - pT->MOT_Rs*pT->IdFbk + pT->FluxEst_ElecSpd*pT->MOT_Ls*pT->IqFbk;
	pT->Eqaxis=pT->Uqaxis - pT->MOT_Rs*pT->IqFbk - pT->FluxEst_ElecSpd*pT->MOT_Ls*pT->IdFbk;
	pT->PAR_Alpha=pT->PAR_Alpha0+2*pT->PAR_Lambda*fabs(pT->FluxEst_ElecSpd);
	//******************** 		ת�ټ��� & �Ƕȸ��� 	 *************************************
	pT->FluxEst_ElecSpd += pT->Ts_Cal*pT->PAR_Alpha*((pT->Eqaxis - pT->PAR_Lambda*pT->PAR_MotDir*pT->Edaxis)/pT->MOT_faif - pT->FluxEst_ElecSpd);
//	if(pT->FluxEst_ElecSpd>3000)
//		pT->FluxEst_ElecSpd=3000;
//	if(pT->FluxEst_ElecSpd<-3000);
//		pT->FluxEst_ElecSpd=-3000;
	pT->FluxEst_Angle += pT->Ts_Cal*pT->FluxEst_ElecSpd;
	pT->FluxEst_Angle= (pT->FluxEst_Angle*0.15915494-floor(pT->FluxEst_Angle*0.15915494))*6.28318531; //�Ƕȹ�һ����0,2pi������


	x=(x<19)?++x:0;
	pT->sum=pT->sum-*(temp+x)+pT->FluxEst_ElecSpd;
	*(temp+x)=pT->FluxEst_ElecSpd;

//	pT->Ave_we=FirCalc(&Flux_EstSpdFdbFir,(pT->FluxEst_ElecSpd/pT->MOT_MaxOmega)<<25,5);

	//---------------------   �����Թ۲���     -----------------------------------------
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
////	//---------------------   �����Թ۲���2     -----------------------------------------
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
* @������		:  		FluxPos_Init
* @��������	: 		�����۲�����ʼ��
* @������		: 		��Ѱ
* ��������		:   2020-11-16
* �޸ļ�¼		:
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
	pT->MOT_Ls0 = 350*0.000001;		//��г�ʼֵ��������Ԥ��
	pT->MOT_Rs0=0.06f;				//�����ʼֵ��������Ԥ��
	pT->PAR_LsFactor=0.3;		//������仯��
	pT->PAR_RsFactor=0;				//�������仯��

	pT->MOT_Ls = pT->MOT_Ls0;
	pT->MOT_Rs= pT->MOT_Rs0;
	pT->MOT_faif=0.018f;			//���Ŵ�������Ϊ��������
	pT->MOT_MaxCurr=20;
	pT->MOT_MaxOmega=5000*3/60*2*PI;
	SampleFreq_Hz = PWMCLKFREQ/(2*pCtrl->PwmPeriod);
	pT->Ts_Cal =1.0f/SampleFreq_Hz;
	pT->PAR_Alpha0=600;

	pT->PAR_Lambda0=4;
	pT->PAR_Lambda=pT->PAR_Lambda0;
	pT->PAR_Lambfactor=0.5;					// ����Ӧ�����仯���ֵ

	pT->PAR_MotDir=0;

	pT->Selfsensing_Enable=0;
	pT->PAR_Wlim=pT->MOT_MaxOmega*0.3;		// 30%�����ת��Ϊ�ߵ����ٽ��
	pT->VDC_V=28;							// ĸ�ߵ�ѹ
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
