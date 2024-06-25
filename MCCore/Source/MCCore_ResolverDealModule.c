/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	MCCore DSP执行旋变解码模块的访问
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-09-27                                                                 
* 修改记录		:   
*            
======================================================================================================*/

#include "MCCore_ResolverDealModule.h"


/**************************************************************************************************************************************************
* @函数名		:  		DSPCore_CfgResolverDealModule                                                        
* @函数描述	: 		
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-09-27                                                          
* 修改记录		:   			
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
* @函数名		:  		DSPCore_DoResolverDealModule
* @函数描述	: 		执行速度环计算
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-09-27                                                             
* 修改记录		:   			
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

	Ad2s_A.FAULT = 0x80;// 位置高8位
	TempPos = Ad2s_A.FAULT;
	TempPos = TempPos << 8;
	Ad2s_A.FAULT = 0x81;// 位置低8位
	TempPos  += Ad2s_A.FAULT;
	pR->ResolverOriPos = TempPos;

	Ad2s_A.FAULT = 0x82;// 速度高8位
	s16TempSpd = Ad2s_A.FAULT;
	s16TempSpd = s16TempSpd << 8;
	Ad2s_A.FAULT = 0x83;// 速度低8位
	s16TempSpd += Ad2s_A.FAULT;
	pR->SpdReg = s16TempSpd;

	s32TempSpd = s16TempSpd;

	pR->ResolverElecPos = (m1CurrLoopCtler.MotCtl.bit.PolesRatio + 1)*TempPos;// 保留低16位，高位自动截断
	s32TempSpd =  FirCalc(&CoreSpdFdbFir, s32TempSpd, 1);// 滑动平均滤波
	s32TempSpd = s32TempSpd * pR->SpdBitToRpmCoef >> 12;
	pR->ResolverElecRpm = LIMITMAXMIN(s32TempSpd * pR->SpdModifyCoef >> 13,32767,-32768);
	return TRUE;
}

