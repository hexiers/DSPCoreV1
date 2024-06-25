/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	MCCore DSPִ���������ģ��ķ���
* @������		:  	����                                                                              
* ��������		:   	2020-09-27                                                                 
* �޸ļ�¼		:   
*            
======================================================================================================*/

#include "MCCore_ResolverDealModule.h"


/**************************************************************************************************************************************************
* @������		:  		DSPCore_CfgResolverDealModule                                                        
* @��������	: 		
* @������		:    		����                                                                            
* ��������		:   		2020-09-27                                                          
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_CfgResolverDealModule(stResolverDealModule* pR)
{
	Ad2s_A.FAULT = 0x91;
	Ad2s_A.FAULT = pR->Ad2sExcFreqReg;
	Ad2s_A.FAULT = 0x92;
	Ad2s_A.FAULT = pR->Ad2sCfgReg;

	Ad2s_A.FAULT = 0xF0;
	Ad2s_A.FAULT = 0;
	
	Delay_us(100);
	Ad2s_A.FAULT = 0xFF;
	return TRUE;
}
/**************************************************************************************************************************************************
* @������		:  		DSPCore_DoResolverDealModule
* @��������	: 		ִ���ٶȻ�����
* @������		:    		����                                                                            
* ��������		:   		2020-09-27                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool DSPCore_DoResolverDealModule(stResolverDealModule* pR)
{
	Uint16 TempPos = 0;
	INT16S s16TempSpd = 0;
	INT32S s32TempSpd = 0;
	
	A_SAMPLE = 0;
	Delay_us(2);
	A_SAMPLE = 1;	

	Ad2s_A.FAULT = 0xFF;
	pR->FaultReg = Ad2s_A.FAULT;

	Ad2s_A.FAULT = 0x80;// λ�ø�8λ
	TempPos = Ad2s_A.FAULT;
	TempPos = TempPos << 8;
	Ad2s_A.FAULT = 0x81;// λ�õ�8λ
	TempPos  += Ad2s_A.FAULT;
	pR->ResolverOriPos = TempPos;

	Ad2s_A.FAULT = 0x82;// �ٶȸ�8λ
	s16TempSpd = Ad2s_A.FAULT;
	s16TempSpd = s16TempSpd << 8;
	Ad2s_A.FAULT = 0x83;// �ٶȵ�8λ
	s16TempSpd += Ad2s_A.FAULT;
	pR->SpdReg = s16TempSpd;

	s32TempSpd = s16TempSpd;

	pR->ResolverElecPos = (m1CurrLoopCtler.MotCtl.bit.PolesRatio + 1)*TempPos;// ������16λ����λ�Զ��ض�
	s32TempSpd =  FirCalc(&CoreSpdFdbFir, s32TempSpd, 1);// ����ƽ���˲�
	s32TempSpd = s32TempSpd * pR->SpdBitToRpmCoef >> 12;
	pR->ResolverElecRpm = LIMITMAXMIN(s32TempSpd * pR->SpdModifyCoef >> 13,32767,-32768);
	return TRUE;
}

