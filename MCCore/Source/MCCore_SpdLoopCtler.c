/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	MCCore DSP执行速度环运算
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-09-27                                                                 
* 修改记录		:   
				2020-10-08:增加弱磁控制初始化和计算函数
				2020-10-10:在弱磁控制结构初始化时候调用弱磁参数
*            
======================================================================================================*/

#include "MCCore_SpdLoopCtler.h"
#include "math.h"


/**************************************************************************************************************************************************
* @函数名		:  		DSPCore_CfgSpdLoopCtler                                                        
* @函数描述	: 		
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-09-27                                                          
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_CfgSpdLoopCtler(stSpdLoopCtler* pD)
{
	return TRUE;
}
/**************************************************************************************************************************************************
* @函数名		:  		DSPCore_CfgFwCtler                                                        
* @函数描述	: 		
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-10-8                                                          
* 修改记录		:   			
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
* @函数名		:  		DSPCore_DoSpdLoopCtrl
* @函数描述	: 		执行速度环计算
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-09-27                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_DoSpdLoopCtrl(stSpdLoopCtler* pD,stResolverDealModule* pR,stSmcObserverExt* pS)
{
	INT32S s32DeltaSpd = 0;
	//==========获取速度反馈================//
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
	//==========速度PI计算================//
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


	pD->SpdIntergLim32 = LIMITMAXMIN(pD->SpdIntergLim32, 1048575, -1048576);// 限制到[-2^20,2^20]
	pD->SpdIntergLim = pD->SpdIntergLim32 >> 7;
	pD->SpdPIOut = pD->SpdProp + pD->SpdIntergLim;
	//==========输出限制================//
	pD->SpdLimOut = LIMITMAXMIN(pD->SpdPIOut,pD->SpdOutMax,pD->SpdOutMin);
	//pD->DisChargeSpd =  pD->SpdLimOut  - pD->SpdPIOut;



	return TRUE;
}

/**************************************************************************************************************************************************
* @函数名		:  		DSPCore_DoFwCtrl
* @函数描述	: 			执行弱磁控制
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-10-8                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_DoFwCtrl(stFwCtler* pD,stCurrLoopCtler* pC)
{
	INT32S s32DeltaFw = 0;
	//==========获取弱磁环电压反馈================//
	pD->FwUsFdb = sqrt( (INT32S)pC->UdOut_BIT * pC->UdOut_BIT +  (INT32S)pC->UqOut_BIT * pC->UqOut_BIT);	 
	//==========弱磁PI计算================//
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
	pD->FwIntergLim32 = LIMITMAXMIN(pD->FwIntergLim32, 1048575, -1048576);// 限制到[-2^20,2^20]
	pD->FwIntergLim = pD->FwIntergLim32 >> 7;
	pD->FwPIOut = pD->FwProp + pD->FwIntergLim;
	//==========输出限制================//
	pD->FwLimOut = LIMITMAXMIN(pD->FwPIOut, pD->FwOutMax, pD->FwOutMin);
	pD->DisChargeFw =  pD->FwLimOut - pD->FwPIOut;
	return TRUE;
}

