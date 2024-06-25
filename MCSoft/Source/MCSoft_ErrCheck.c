/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����
* @������		:  	����                                                                              
* ��������		:   	2020-03-07                                                                 
* �޸ļ�¼		:   
*                         2020-10-10: ���ӱ궨����ʧ�ܵ��ж�
======================================================================================================*/
#include"MCSoft_ErrCheck.h"

/**************************************************************************************************************************************************
* @������		:  		SignalMakeSure                                                        
* @��������	: 		ʵ���źŵ������˲�
* @������		:    		����                                                                            
* ��������		:   		2020-03-07                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool  SignalMakeSure(Uint16 MakeSure_Id, bool  InputData,Uint16 MakesureTimes)
{
	bool RtnVal = FALSE;
	static  Uint16 MakeSureCnt[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	if(MakeSure_Id >= 32)
	{	
		/* ����쳣��֧*/
		MC_PostCodeErr(1);
		return FALSE;
	}
	if(InputData)
	{
		/* ��ֹ�ۼ����*/
		if(MakeSureCnt[MakeSure_Id] < MakesureTimes)
		{
			MakeSureCnt[MakeSure_Id]++;
		}
		else
		{
			RtnVal = TRUE;
		}
	}
	else
	{
		MakeSureCnt[MakeSure_Id] = 0;
	}
	return RtnVal;
}
/**************************************************************************************************************************************************
* @������		:  		UpDateCriticalSampleData_MediumFreq                                                       
* @��������	: 		������Ƶ�����У����¹ؼ����ݽṹ��
* @������		:    		����                                                                            
* ��������		:   		2020-03-10                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
void UpDateCriticalSampleData_MediumFreq(stCriticalPara* pC)
{
	INT32S TmpSpd = 0;
	if(m1SysCfgPara.SysCfgWord1.bit.EnSnsLess)
	{
		//TmpSpd = m1SmcObserverExt.SmcElecRpm/m1MotorPara.MotorPoles;/* ����۲���*/
		TmpSpd = (INT32S)m1SmcObserverExt.SmcElecRpm * SPDFDB_SCALE_FACTOR/m1MotorPara.MotorPoles;/* FPGA�۲���*/
	}
	else /* �д�����*/
	{   
	    /* ��ϵ���ĵ���ת��*/
	    TmpSpd = m1ResolverDealModule.ResolverElecRpm;
	    /* ΪDSP���ٶȿ�����������ϵ���ĵ���ת��*/

		/* �����еת�٣�������ʾ*/
	    TmpSpd = TmpSpd * SPDFDB_SCALE_FACTOR/m1MotorPara.SnsorPoles;
	}
	m1CriPara.SpdFdb  = FirCalc(&SpdFdbFir, TmpSpd, 1);

	m1CriPara.SpdOut = (INT32S)((INT32S)m1SpdLoopCtler.SpdLimOut * (Uint32)m1HalPara.BitTo10mACoef) >> 12;
	// FPGA Bug ��ʱ�ŵ��ж���
	//m1CriPara.HpVol  = (Uint32)m1BusVolDecoupler.V_Bus*(Uint32)m1HalPara.BitTo10mVCoef>>12;
	m1CriPara.LpVol 	= HAL_GetLpVol();
	m1CriPara.IBus 	    = HAL_GetHpCurr();
	m1CriPara.TmpMotor	= HAL_GetMotorTemp();
	m1CriPara.TmpPdd 	= HAL_GetPddTemp();
	m1CriPara.UdRef     = m1CurrLoopCtler.UdActOut_V;
	m1CriPara.UqRef     = m1CurrLoopCtler.UqActOut_V;
	m1CriPara.MainState = m1State.MainState;
	m1CriPara.SubState  = m1State.SubState;
	m1CriPara.LowSpdCapture1 = *stScope.Monitor1Addr;// V1.6
	m1CriPara.LowSpdCapture2 = *stScope.Monitor2Addr;// V1.6
}

/**************************************************************************************************************************************************
* @������		:  		UpDateCriticalSampleData_HighFreq                                                       
* @��������	: 		������Ƶ�����У����¹ؼ����ݽṹ��
* @������		:    		����                                                                            
* ��������		:   		2020-03-10                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
void UpDateCriticalSampleData_HighFreq(stCriticalPara* pC)
{
	INT16S IdBitFdbFiltered = 0;
	INT16S IqBitFdbFiltered = 0;   
	IdBitFdbFiltered = (INT16S)FirCalc(&IdFdbFir, m1CurrLoopCtler.IdFdb, 1);
	IqBitFdbFiltered = (INT16S)FirCalc(&IqFdbFir, m1CurrLoopCtler.IqFdb, 1);
	m1CriPara.IdFdb = (INT32S)((INT32S)IdBitFdbFiltered * (Uint32)m1HalPara.BitTo10mACoef) >>12;
	m1CriPara.IqFdb = (INT32S)((INT32S)IqBitFdbFiltered * (Uint32)m1HalPara.BitTo10mACoef) >>12;
}
/**************************************************************************************************************************************************
* @������		:  		MC_DoFaultCheck                                                        
* @��������	: 		��������ж�
* @������		:    		����                                                                            
* ��������		:   		2020-03-10                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
Uint32  MC_DoFaultCheck(stSysCfgPara* pCfg,stSysState* pState,stFaultPara* pF,stCriticalPara* pC)
{
	Uint32 CodeReturn = 0;
	/* ֱ�ӿ���FPGA������*/
	CodeReturn = m1CurrLoopCtler.ErrFlag.all & 0xFF;
	if(CodeReturn)
	{
		/* CPU����FPGA���Ϻ�ֱ�����FPGA����,CPU�Ὣ�ù�������*/
		m1CurrLoopCtler.MotCtl.bit.ClrErr = 1;
	}
#if 1
	/* ��ѹ��ѹ���*/
	if(SignalMakeSure(Err_LPOV,pC->LpVol > pF->LPOvFaultPoint,3))
	{
		CodeReturn |= (1 << Err_LPOV);
	}
	/* ��ѹǷѹ���*/
	if(SignalMakeSure(Err_LPUV,pC->LpVol < pF->LPUvFaultPoint,3))
	{
		CodeReturn |= (1 << Err_LPUV);
	}
	/* ��ѹ�������*/
	if(SignalMakeSure(Err_HPOC,pC->IBus > pF->HPOcFaultPoint,3))
	{
		CodeReturn |= (1 << Err_HPOC);
	}
	/* ���ʹܹ��¼��*/
	if(SignalMakeSure(Err_PDDOVT,pC->TmpPdd > pF->PDDOvtFaultPoint,3))
	{
		CodeReturn |= (1 << Err_PDDOVT);
	}
	/*  ������¼��*/
	if(SignalMakeSure(Err_MOTOROVT, pC->TmpMotor  > pF->MotorOvtFaultPoint,3))
	{
		CodeReturn |= (1 << Err_MOTOROVT);
	}	
	/* �����������*/
	if(SignalMakeSure(Err_SOFTOC, (ABS(pC->IqFdb) > pF->PhaseOcFaultPoint) ||(ABS(pC->IdFdb) > pF->PhaseOcFaultPoint),3))
	{
		CodeReturn |= (1 << Err_SOFTOC);
	}
	/*  ���ټ��*/
	if(SignalMakeSure(Err_OVSPD, ABS(pC->SpdFdb)  > pF->OvSpdFaultPoint,3))
	{
		CodeReturn |= (1 << Err_OVSPD);
	}
	/*  �޸�ת�ٹ��ͼ�⣬�����޸п��ơ��Ҳ���HFI��RUN״ִ̬�� V1.6*/
	if((pCfg->SysCfgWord1.bit.EnSnsLess)&&(pCfg->SysCfgWord1.bit.EnHfi == 0)&&(pState->MainState == STS_Run))
	{
		if(SignalMakeSure(Err_UNSPD, pC->SpdFdb  <  pF->SnsLessUnSpdFaultPoint,3))
		{
			//CodeReturn |= ((Uint32)1 << Err_UNSPD);
		}
	}
	/*  Ԥ��������Ӳ�����ϼ��*/
	if(pC->GSts.bit.FpgaFailFault)
	{
		CodeReturn |= ((Uint32)1 << Err_HARDWARE);
	}	
	/*  �޸�����ʧ�ܹ��ϼ��*/
	if(((pState->MainState == STS_StartUp)&&(pState->SubState == 3))
		|| ((pState->MainState == STS_MainTenance)&&(pState->SubState == 16)))
	{
		if(m1StartUp.StartUpSuccess == FALSE)
		{
			CodeReturn |= ((Uint32)1 << Err_STARTUP);
		}
	}
	#endif
	/* EEPROM���� ��Ϊ�澯*/
	/*(pC->Eeprom.bit.EepromErr)
	{
		CodeReturn |= (1 << Err_EEPROM);	
	}*/
	/* CPU�������*/
	if(pC->SoftErrCode)
	{
		CodeReturn |= ((Uint32)1 << Err_SOFT);
	}
	if(m1PosCalib.AlignErrCode)
	{
		CodeReturn |= ((Uint32)1 << Err_CALIPOS);
		m1PosCalib.AlignErrCode = 0;
	}
	
	return CodeReturn;
}

/**************************************************************************************************************************************************
* @������		:  		MC_DoWarnCheck                                                        
* @��������	: 		����澯�ж�
* @������		:    		����                                                                            
* ��������		:   		2020-03-10                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
Uint32  MC_DoWarnCheck(stSysCfgPara* pCfg,stSysState* pState,stWarnPara* pW,stCriticalPara* pC)
{
	Uint32 CodeReturn = 0;
	/* ��ѹ��ѹ*/
	if(pC->HpVol > pW->HPOvWarnPoint)
	{
		CodeReturn |= (1 << Warn_HPOV);
	}
	/* ��ѹǷѹ*/
	if(pC->HpVol < pW->HPUvWarnPoint)
	{
		CodeReturn |= (1 << Warn_HPUV);
	}
	/* ��ѹ��ѹ*/
	if(pC->LpVol > pW->LPOvWarnPoint)
	{
		CodeReturn |= (1 << Warn_LPOV);
	}
	/* ��ѹǷѹ*/
	if(pC->LpVol < pW->LPUvWarnPoint)
	{
		CodeReturn |= (1 << Warn_LPUV);
	}
	/* ��ѹ����*/
	if(pC->IBus > pW->HPOcWarnPoint)
	{
		CodeReturn |= (1 << Warn_HPOC);
	}
	/* ���ʹܹ���*/
	if(pC->TmpPdd > pW->PDDOvtWarnPoint)
	{
		CodeReturn |= (1 << Warn_PDDOVT);
	}
	/* �������*/
	if(pC->TmpMotor> pW->MotorOvtWarnPoint)
	{
		CodeReturn |= (1 << Warn_MOTOROVT);
	}
	/* �����������*/
	if((ABS(pC->IqFdb) > pW->PhaseOcWarnPoint) ||(ABS(pC->IdFdb) > pW->PhaseOcWarnPoint))
	{
		CodeReturn |= (1 << Warn_SOFTOC);
	}
	/*  ���ټ��*/
	if( pC->SpdFdb  > pW->OvSpdWarnPoint)
	{
		CodeReturn |= (1 << Warn_OVSPD);
	}
	/*  �޸�ת�ٹ��ͼ�⣬�����޸п��Ƶ�RUN״ִ̬��*/
	if((pCfg->SysCfgWord1.bit.EnSnsLess)&&(pState->MainState == STS_Run))
	{
		if(pC->SpdFdb  <  pW->SnsLessUvSpdWarnPoint)
		{
			CodeReturn |= (1 << Warn_UNSPD);
		}
	}
	/*  EEPROM������Ч��ʹ�ó���Ĭ�ϲ���*/
	if(pC->Eeprom.bit.EepromErr)
	{
		CodeReturn |= (1 << Warn_EEDATAINVLID);	
	}
	/*  EEPROM�洢ʧ�ܣ�ʹ�ó���Ĭ�ϲ���*/
	if(pC->GSts.bit.SaveEepromFailWarn)
	{
		CodeReturn |= (1 << Warn_EEPROMFAIL);	
	}
	

	return CodeReturn;
}

/**************************************************************************************************************************************************
* @������		:  		MC_PostCodeErr                                                        
* @��������	: 		ͨ�������������
* @������		:    		����                                                                            
* ��������		:   		2020-03-07                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
void MC_PostCodeErr(Uint16 ErrCode)
{
	m1CriPara.SoftErrCode = ErrCode;
}

/**************************************************************************************************************************************************
* @������		:  		MC_PostCodeWarn
* @��������	: 		ͨ�������������
* @������		:    		����
* ��������		:   		2020-03-07
* �޸ļ�¼		:
*
****************************************************************************************************************************************************/
void MC_PostCodeWarn(Uint16 ErrCode)
{
	m1CriPara.SoftWarnCode = ErrCode;
}

