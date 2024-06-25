/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	MCCore DSPִ���ٶȻ�����
* @������		:  	����                                                                              
* ��������		:   	2020-09-27                                                                 
* �޸ļ�¼		:   
				2020-10-08:�������ſ��Ƴ�ʼ���ͼ��㺯��
				2020-10-10:�����ſ��ƽṹ��ʼ��ʱ��������Ų���
*            
======================================================================================================*/

#include "MCCore_SpdLoopCtler.h"
#include "math.h"


/**************************************************************************************************************************************************
* @������		:  		DSPCore_CfgSpdLoopCtler                                                        
* @��������	: 		
* @������		:    		����                                                                            
* ��������		:   		2020-09-27                                                          
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_CfgSpdLoopCtler(stSpdLoopCtler* pD)
{
	return TRUE;
}
/**************************************************************************************************************************************************
* @������		:  		DSPCore_CfgFwCtler                                                        
* @��������	: 		
* @������		:    		����                                                                            
* ��������		:   		2020-10-8                                                          
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_CfgFwCtler(stFwCtler* pD, stCtrlPara* pC)
{
	MemClr((Uint16*)pD, (Uint16*)pD + sizeof(stFwCtler)/sizeof(Uint16));
	pD->FwUsCmd = 8192 - 200;
	pD->FwKp = pC->FwKp;
	pD->FwKi = pC->FwKi;
	pD->FwOutMax = 0;
	pD->FwOutMin = -m1MotorPara.RatedCurrentD_10mA;
	return TRUE;
}
/**************************************************************************************************************************************************
* @������		:  		DSPCore_DoSpdLoopCtrl
* @��������	: 		ִ���ٶȻ�����
* @������		:    		����                                                                            
* ��������		:   		2020-09-27                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_DoSpdLoopCtrl(stSpdLoopCtler* pD,stResolverDealModule* pR,stSmcObserverExt* pS)
{
	INT32S s32DeltaSpd = 0;
	//==========��ȡ�ٶȷ���================//
	if(m1CurrLoopCtler.MotCtl.bit.PosSel == 0)
	{
		pD->SpdFdbElecRpm = pR->ResolverElecRpm;
	}
	else if(m1CurrLoopCtler.MotCtl.bit.PosSel == 1)
	{
		pD->SpdFdbElecRpm = pS->SmcElecRpm;
	}
	/*else if(m1CurrLoopCtler.MotCtl.bit.PosSel == 3)
	{
		pD->SpdFdbElecRpm =m1FluxObserver.FluxEst_ElecSpd/2/PI*20;
	}*/
	else
	{
		pD->SpdFdbElecRpm = pR->ResolverElecRpm;
	}	
	//==========�ٶ�PI����================//
	s32DeltaSpd = pD->SpdElecCmd - pD->SpdFdbElecRpm;
	s32DeltaSpd = LIMITMAXMIN(s32DeltaSpd, 32767, -32768);

	pD->SpdProp = LIMITMAXMIN(s32DeltaSpd * pD->SpdKp>>7,8191,-8192);
	if(m1CurrLoopCtler.MotCtl.bit.EnSpdCtrl)
	{
		if(pD->SpdPIOut==pD->SpdLimOut)
		{pD->SpdIntergLim32 = pD->SpdIntergLim32 \
							+ pD->SpdKi * s32DeltaSpd ;}
		else
			pD->SpdIntergLim32 = pD->SpdIntergLim32 ;
			/*\
							+( (INT32S)pD->DisChargeSpd<<7) * m1CurrLoopCtler.MotCtl.bit.SpdAntiRstEn;*/
	}
	else
	{
		pD->SpdIntergLim32 = (INT32S)pD->SpdIntergPreset << 7;
	}
	if(pD->SpdKi == 0)
	{
		pD->SpdIntergLim32 = 0;
	}


	pD->SpdIntergLim32 = LIMITMAXMIN(pD->SpdIntergLim32, 1048575, -1048576);// ���Ƶ�[-2^20,2^20]
	pD->SpdIntergLim = pD->SpdIntergLim32 >> 7;
	pD->SpdPIOut = pD->SpdProp + pD->SpdIntergLim;
	//==========�������================//
	pD->SpdLimOut = LIMITMAXMIN(pD->SpdPIOut,pD->SpdOutMax,pD->SpdOutMin);
	//pD->DisChargeSpd =  pD->SpdLimOut  - pD->SpdPIOut;



	return TRUE;
}

/**************************************************************************************************************************************************
* @������		:  		DSPCore_DoFwCtrl
* @��������	: 			ִ�����ſ���
* @������		:    		����                                                                            
* ��������		:   		2020-10-8                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_DoFwCtrl(stFwCtler* pD,stCurrLoopCtler* pC)
{
	INT32S s32DeltaFw = 0;
	//==========��ȡ���Ż���ѹ����================//
	pD->FwUsFdb = sqrt( (INT32S)pC->UdOut_BIT * pC->UdOut_BIT +  (INT32S)pC->UqOut_BIT * pC->UqOut_BIT);	 
	//==========����PI����================//
	s32DeltaFw = pD->FwUsCmd - pD->FwUsFdb;
	s32DeltaFw = LIMITMAXMIN(s32DeltaFw, 32767, -32768);
	pD->FwProp = LIMITMAXMIN(s32DeltaFw * pD->FwKp>>7,8191,-8192);
	
	pD->FwIntergLim32 = pD->FwIntergLim32 \
					+ pD->FwKi * s32DeltaFw \
					+( (INT32S)pD->DisChargeFw<<7) ;
	
	if(pD->FwKi == 0)
	{
		pD->FwIntergLim32 = 0;
	}
	pD->FwIntergLim32 = LIMITMAXMIN(pD->FwIntergLim32, 1048575, -1048576);// ���Ƶ�[-2^20,2^20]
	pD->FwIntergLim = pD->FwIntergLim32 >> 7;
	pD->FwPIOut = pD->FwProp + pD->FwIntergLim;
	//==========�������================//
	pD->FwLimOut = LIMITMAXMIN(pD->FwPIOut, pD->FwOutMax, pD->FwOutMin);
	pD->DisChargeFw =  pD->FwLimOut - pD->FwPIOut;
	return TRUE;
}

