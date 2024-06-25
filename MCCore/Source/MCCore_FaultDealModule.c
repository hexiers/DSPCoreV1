/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	MCCore DSP执行高速故障检测相关代码
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-09-28                                                                 
* 修改记录		:   
*            
======================================================================================================*/

#include "MCCore_FaultDealModule.h"

/**************************************************************************************************************************************************
* @函数名		:  		DSPCore_CfgFaultDealModule                                                        
* @函数描述	: 		
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-09-28                                                        
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_CfgFaultDealModule(stFaultDealModule* pR)
{
	//MemClr((Uint16*)pR,(Uint16*)pR + sizeof(stFaultDealModule)/sizeof(Uint16));
	return TRUE;
}

/**************************************************************************************************************************************************
* @函数名		:  		DSPCore_DoFaultDealModule
* @函数描述	: 		执行速度环计算
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-09-27                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_DoFaultDealModule(stFaultDealModule* pF,stCurrLoopCtler*pD,stBusVolDecoupler* pBus,stResolverDealModule*pR)
{
	//Err_OC 故障信号在TZ中断中判断
	//Err_SC 故障信号在TZ中断中判断
	//Err_HPOV
	if(SignalMakeSure(Err_HPOV,pBus->V_Bus > pF->OVPoint,5))
	{
		pD->ErrSignal.bit.OVERR = 1;
	}
	else
	{
		pD->ErrSignal.bit.OVERR = 0;
	}
	
	//Err_HPUV
	if(SignalMakeSure(Err_HPUV,pBus->V_Bus < pF->UVPoint,5))
	{
		pD->ErrSignal.bit.UVERR = 1;
	}
	else
	{
		pD->ErrSignal.bit.UVERR = 0;
	}
	
	//Err_POS
	if(SignalMakeSure(Err_POS,(pR->FaultReg)&&(pD->MotCtl.bit.PosSel == 0),5))
	{
		pD->ErrSignal.bit.POSERR = 1;
	}
	else
	{
		pD->ErrSignal.bit.POSERR = 0;
	}
	//Err_UVLO
	// 无驱动欠压的信号源
	pD->ErrSignal.bit.UVLO = 0;

	//Err_SERVODIS
	if(SignalMakeSure(Err_SERVODIS,STOPMOTOR,5))
	{
		pD->ErrSignal.bit.ServoDis = 1;
	}
	else
	{
		pD->ErrSignal.bit.ServoDis = 0;
	}
	
	/* 故障锁定*/
	pD->ErrFlag.all |= pD->ErrSignal.all;

	if(m1CurrLoopCtler.MotCtl.bit.ClrErr)
	{
		pD->ErrFlag.all = 0;
		m1CurrLoopCtler.MotCtl.bit.ClrErr = 0;
	}

	// 短路故障自动清除
	pD->ErrSignal.bit.SCERR = 0;
	// 过流故障自动清除
	pD->ErrSignal.bit.OCERR = 0;

	// 故障关闭输出
	if(pD->ErrFlag.all & 0x7FFF)
	{
		pD->ErrFlag.bit.ORERR = 1;
		pD->MotCtl.bit.Start = 0;
	}
	else
	{
		pD->ErrFlag.bit.ORERR = 0;
	}

	return TRUE;
}

