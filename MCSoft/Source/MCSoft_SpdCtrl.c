/*
 * MCSoft_SpdCtrl.c
 *
 *  Created on: 2020Äê9ÔÂ8ÈÕ
 *      Author: xub
 */
#include"string.h"
#include "MCSoft_SpdCtrl.h"
/*
stCpuSpdLoop d1SpeedLoop;

bool SpdLoopInit(stCpuSpdLoop* pD,stCtrlPara* pCtrl,stMotorPara*pMotor,stHalPara*pHal)
{

	if((pD == NULL)||(pCtrl == NULL)||(pMotor == NULL)||(pHal == NULL))
	{
		return FALSE;
	}

	memset(pD,0,sizeof(stCpuSpdLoop));

	pD->SpdKp = pCtrl->SpdInitKp;
	pD->SpdKi = pCtrl->SpdKi;
	pD->SpdOutMax = ((Uint32)pHal->F10mAToBitCoef * (INT32S)pMotor->RatedCurrentQ_10mA)>>12;
	pD->SpdOutMin = 0;
	pD->SpdIntergPreset = 0;

	return TRUE;
}



void SpdLoopCalc(stCpuSpdLoop *p,INT32S iError,bool AntiRstEn,bool SpdCtrlEn)
{
	p->SpdProp = LIMITMAXMIN((p->SpdKp*iError >> 7),8191,-8192);

	p->DisChargeSpd = p->SpdLimOut - p->SpdPIOut;

	if(SpdCtrlEn)
	{
		p->SpdIntergLim32 = p->SpdKi * iError                                         \
						+ p->SpdIntergLim32											\
						+ ((INT32S)(AntiRstEn*(INT32S)p->DisChargeSpd) << 7);
	}
	else
	{
		p->SpdIntergLim32 = p->SpdKi * iError                                          \
						+ ((INT32S)p->SpdIntergPreset << 7)								     \
						+ ((INT32S)(AntiRstEn * (INT32S)p->DisChargeSpd) << 7);
	}

	p->SpdPIOut = p->SpdProp + LIMITMAXMIN((p->SpdIntergLim32 >> 7),8191,-8192);

	p->SpdLimOut = LIMITMAXMIN(p->SpdPIOut,p->SpdOutMax,p->SpdOutMin);


    p->SpdIntergLim = p->SpdIntergLim32 >> 7;

}
*/
