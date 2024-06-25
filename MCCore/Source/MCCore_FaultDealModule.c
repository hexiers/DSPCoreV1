/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	MCCore DSPִ�и��ٹ��ϼ����ش���
* @������		:  	����                                                                              
* ��������		:   	2020-09-28                                                                 
* �޸ļ�¼		:   
*            
======================================================================================================*/

#include "MCCore_FaultDealModule.h"

/**************************************************************************************************************************************************
* @������		:  		DSPCore_CfgFaultDealModule                                                        
* @��������	: 		
* @������		:    		����                                                                            
* ��������		:   		2020-09-28                                                        
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_CfgFaultDealModule(stFaultDealModule* pR)
{
	//MemClr((Uint16*)pR,(Uint16*)pR + sizeof(stFaultDealModule)/sizeof(Uint16));
	return TRUE;
}

/**************************************************************************************************************************************************
* @������		:  		DSPCore_DoFaultDealModule
* @��������	: 		ִ���ٶȻ�����
* @������		:    		����                                                                            
* ��������		:   		2020-09-27                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_DoFaultDealModule(stFaultDealModule* pF,stCurrLoopCtler*pD,stBusVolDecoupler* pBus,stResolverDealModule*pR)
{
	//Err_OC �����ź���TZ�ж����ж�
	//Err_SC �����ź���TZ�ж����ж�
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
	// ������Ƿѹ���ź�Դ
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
	
	/* ��������*/
	pD->ErrFlag.all |= pD->ErrSignal.all;

	if(m1CurrLoopCtler.MotCtl.bit.ClrErr)
	{
		pD->ErrFlag.all = 0;
		m1CurrLoopCtler.MotCtl.bit.ClrErr = 0;
	}

	// ��·�����Զ����
	pD->ErrSignal.bit.SCERR = 0;
	// ���������Զ����
	pD->ErrSignal.bit.OCERR = 0;

	// ���Ϲر����
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

