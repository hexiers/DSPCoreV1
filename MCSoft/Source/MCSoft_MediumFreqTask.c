/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����
* @������		:  	����                                                                              
* ��������		:   	2020-03-03                                                                 
* �޸ļ�¼		:   
*                         20201010 : ���ӷ����Ʊ궨��ʼ�ǵ�����
			20201012:�����ٶȷ����궨���ܣ�������ʱδ��
======================================================================================================*/
#include "MC_Include.h"
stSysState m1State;
stCriticalPara m1CriPara;
void MC_MainStateTask(void);
extern void MC_OtherTimeBaseGenerate(stTimeBase* pT);
extern Uint32  MC_DoFaultCheck(stSysCfgPara* pCfg,stSysState* pState,stFaultPara* pF,stCriticalPara* pC);
extern Uint32  MC_DoWarnCheck(stSysCfgPara* pCfg,stSysState* pState,stWarnPara* pW,stCriticalPara* pC);
extern void Uart_RxFrm(ST_RBUFMNG *pstRBufMng);
extern void Uart_TxFrm(void);
bool MC_SubStatePwrUpWait(stSysState* pState, stBusVolDecoupler* pBus,stFaultPara* pFault,stHalPara* pHal);
bool MC_SubStateMaintenance(stMCMessage * pMsg,stSysState* pState,stCriticalPara* pC,stFaultPara* pFault,stHalPara* pHal);
bool MC_ChangeMainState(stSysState* pState,State_t NewMainState);
extern bool Uart_Rx(ST_RBUFMNG *pstRBufMng);
extern void UpDateCriticalSampleData_MediumFreq(stCriticalPara* pC);
extern bool InitPosCalib2(stPosCalib* p);

extern Uint16 TxBuf[TX_FRAME_BYTE_LEN];
extern Uint16 TxBufLen;
void DebugDataSend(void)
{
	volatile Uint16 Temp = 0;
	volatile Uint16 i = 0;
	//���ݷ���
	//��ѯ����FIFO�Ŀ����ֽ���Ŀ
	Temp = 16 - SciaRegs.SCIFFTX.bit.TXFFST;

	//��FIFO����������ѭ������
	for(i = 0;i < Temp;i++)
	{
		SciaRegs.SCITXBUF = TxBuf[TX_FRAME_BYTE_LEN - TxBufLen];
		TxBufLen--;

		//�Ѿ�������һ֡��
		if(TxBufLen == 0)
		{
			break;
		}
	}

}
/**************************************************************************************************************************************************
* @������		:  		MC_TaskLoop                                                        
* @��������	: 		1. ѭ���������
* @������		:    	����
* ��������	:   	2020-03-14
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
void MC_TaskLoop(void)
{
	if(m1TimeBase.Every1msSet)
	{
		Uart_Rx(&stRBufMng);
		Uart_RxFrm(&stRBufMng);
		if(TxBufLen)
		{
			DebugDataSend();
		}
		m1TimeBase.Every1msSet = FALSE;
		GpioDataRegs.GPBTOGGLE.bit.GPIO55 = 1;
		DSPCore_DoBusVolDecoupler(&m1BusVolDecoupler);

		MC_MainStateTask();
		MC_OtherTimeBaseGenerate(&m1TimeBase);
	}
	if(m1TimeBase.Every10msSet)
	{
		m1TimeBase.Every10msSet = FALSE;
	}
	if(m1TimeBase.Every50msSet)
	{
		Uart_TxFrm();
		/* ������һ֡���ݺ����������Ϣ���*/
		m1CriPara.MsgBox.all = 0;
		m1TimeBase.Every50msSet = FALSE;
	}
	if(m1TimeBase.Every500msSet)
	{
		HAL_LedToggle();
		m1TimeBase.Every500msSet = FALSE;
	}
	
}

/**************************************************************************************************************************************************
* @������		:  		MC_GetHighVolReady                                                        
* @��������	: 		1. �жϸ�ѹ������Դ�Ƿ��ѹǷѹ���ϵ㣬�Ƿ�����
					2. ��Ϊ����PwrUpWait״̬�ı�Ҫ����
* @������		:    	����
* ��������	:   	2020-03-07
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
bool MC_GetHighVolReady(stBusVolDecoupler* pBus,stFaultPara* pFault,stHalPara* pHal)
{
	if((((INT32S)pBus->V_Bus * (Uint32)pHal->BitTo10mVCoef)>>12) > pFault->HPUvFaultPoint)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**************************************************************************************************************************************************
* @������		:  		MC_StateInit                                                        
* @��������	: 		1. ��ʼ��״̬����
					2. Ӧ��main ������ʼ��ʱ�����
* @������		:    	����
* ��������	:   	2020-03-07
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
void MC_StateInit(stSysState* pState)
{
	pState->MainState = STS_PwrUpWait;
	pState->SubState = 0;
	pState->StateWait = 0;
}


/**************************************************************************************************************************************************
* @������		:  		MC_MainStateTask                                                        
* @��������	: 		��״̬����
* @������		:    	����
* ��������	:   	2020-03-07
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
void MC_MainStateTask(void)
{
	bool rtnVal = FALSE;
	stMCMessage Cmsg;

	/*  ���¹ؼ��Ŀ������ݣ�Ϊ���ϼ����׼����ͬʱҲΪ�����ϴ���׼��*/
	UpDateCriticalSampleData_MediumFreq(&m1CriPara);
	
	/*  CPU����Ĺ��ϼ��*/
	m1CriPara.FaultNow = MC_DoFaultCheck(&m1SysCfgPara,&m1State,&m1FaultPara,&m1CriPara);
	/*  ���ϵ�����*/
	m1CriPara.FaultOccurred   |=  m1CriPara.FaultNow;
	/*  CPU����Ĺ��ϸ澯*/	
	m1CriPara.WarnNow = MC_DoWarnCheck(&m1SysCfgPara,&m1State,&m1WarnPara,&m1CriPara);

	/*  ״̬ת����ģʽ����������ȼ�*/
	/* �ϵ�ȴ�״̬��ת���ϣ�����������Ϣ���޹���ʱ�����������*/
	if(m1State.MainState == STS_PwrUpWait)
	{
		m1CriPara.FaultOccurred = m1CriPara.FaultNow;
	}
	else if((m1CriPara.FaultNow)&&(m1State.MainState == STS_MainTenance))
	{
		//V1.7 ά��ģʽ��PWM�������״̬תά��ģʽ0��״̬
		if((m1State.SubState == 5)||(m1State.SubState == 9)||(m1State.SubState == 11)
			||(m1State.SubState == 14)||(m1State.SubState == 15)||(m1State.SubState == 16))
		{
			MC_ChangeMainState(&m1State,STS_MainTenance);
		}
		else
		{
			//ά��ģʽ�й��ϣ�ֻҪû��PWM�������Ȼά��ԭ��״̬
		}
	}
	else if(m1CriPara.FaultNow)
	{
		MC_ChangeMainState(&m1State,STS_FaultNow);
	}
	else
	{}

	
	switch(m1State.MainState)
	{
		case STS_PwrUpWait:
			rtnVal = MC_SubStatePwrUpWait(&m1State, &m1BusVolDecoupler, &m1FaultPara, &m1HalPara);
			if(rtnVal)
			{
				if(m1SysCfgPara.SysCfgWord1.bit.EnBridgeCheck == 1)
				{
					MC_ChangeMainState(&m1State,STS_BridgeCheck);
				}
				else
				{
					MC_ChangeMainState(&m1State,STS_Idle);
				}
			}
			break;
		
		case STS_BridgeCheck:
			/* �ű��Լ칦��Ԥ��*/
			MC_ChangeMainState(&m1State,STS_Idle);
			break;
		
		case STS_Idle:
			if(ABS(m1CriPara.SpdFdb) > 2*m1CtrlPara.ZeroSpdPoint_rpm)
			{
				m1State.SubState = 0;	
			}
			else if(ABS(m1CriPara.SpdFdb) < m1CtrlPara.ZeroSpdPoint_rpm)
			{
				m1State.SubState = 1;	
			}
			else
			{
				;/* ά��ǰ��״̬*/
			}
			if(GetMessge(&Cmsg))
			{
				if(CmdGoMT == Cmsg.msg)
				{
					MC_ChangeMainState(&m1State,STS_MainTenance);
				}
				else if((CmdSTARTCURRLOOP == Cmsg.msg)||(CmdSTARTSPDLOOP == Cmsg.msg))
				{	
					/* ����ģʽ*/
					if(CmdSTARTCURRLOOP == Cmsg.msg)
					{
						m1CriPara.CtrlMode = MODE_CURR;
						m1CriPara.IdRef10mA = 0;
						m1CriPara.IqRef10mA = LIMITMAXMIN(Cmsg.msgPara1,m1MotorPara.RatedCurrentQ_10mA,
													-m1MotorPara.RatedCurrentQ_10mA);
						m1CriPara.SpdRef = 0;
					}
					/* �ٶ�ģʽ*/
					else
					{
						m1CriPara.CtrlMode = MODE_SPD;
						m1CriPara.IdRef10mA = 0;
						m1CriPara.IqRef10mA = 0;
						m1CriPara.SpdRef = LIMITMAXMIN(Cmsg.msgPara1,m1MotorPara.RatedSpd,-m1MotorPara.RatedSpd);
					}
					
					/* �������ת�����������ߵ�ǰת���Ѿ�Ϊ0 */
					if((m1SysCfgPara.SysCfgWord1.bit.EnNoZeroSpdStart)||(m1State.SubState == 1))
					{
					    // V1.6
						if((m1SysCfgPara.SysCfgWord1.bit.EnSnsLess == 1)&&(m1SysCfgPara.SysCfgWord1.bit.EnHfi == 0))
						{
							//MC_ChangeMainState(&m1State,STS_StartUp);
							MC_ChangeMainState(&m1State,STS_Run);
							
						}
						else
						{
							MC_ChangeMainState(&m1State,STS_Run);
						}
					}
					/* �ڵ�ǰת�ٷ��㣬���������������ʱ�����յ������źź�����ǰ����ָ��*/
					else
					{
						MC_NotifyCmdDisCard(Cmsg.msg);
					}
				}
				else
				{
					MC_NotifyCmdDisCard(Cmsg.msg);
				}
			}
			break;
			
		case STS_StartUp:
			/* ��Ӧͣ������*/
			if(GetMessge(&Cmsg))
			{
				if(CmdSTOPMOTOR == Cmsg.msg)
				{
					MC_ChangeMainState(&m1State,STS_Idle);
				}
				else
				{
					MC_NotifyCmdDisCard(Cmsg.msg);
				}
			}			
			/* ��ʼ��������������*/
			if(m1State.SubState == 0)
			{
				MC_InitStartUp(&m1CtrlPara, &m1SnsLessPara, &m1MotorPara, &m1StartUp);
				m1State.SubState = 1;
			}
			/* ʵʱ�������ж���ִ�У���״̬1-2 ��2->3��ת���ڸ�Ƶ������*/
			else if((m1State.SubState == 1)||(m1State.SubState == 2))
			{				
			}
			/* ���������󣬸���DoNotSwitch��StartUpSuccessȷ��״̬ת�Ʋ���*/
			else if(m1State.SubState == 3)
			{
				/* �ȴ����ϼ���߼�ת������ʧ�ܹ���*/
				if(m1StartUp.StartUpSuccess == 0)
				{				
				}
				/* ά��I/F���ƵĽ���״̬*/
				else if(m1SysCfgPara.SysCfgWord1.bit.DoNotSwitch)
				{					
				}
				/* ת��RUN״̬*/
				else
				{
					MC_ChangeMainState(&m1State,STS_Run);
				}
			}
			else
			{
				/* ������Ч��״̬*/
				MC_PostCodeErr(2);
			}
			break;
			
		case STS_Run:			
			if(GetMessge(&Cmsg))
			{
				/* ��Ӧͣ������*/
				if(CmdSTOPMOTOR == Cmsg.msg)
				{
					MC_ChangeMainState(&m1State,STS_Idle);
				}
				/* ����ģʽ�¸��µ���Ŀ��ָ��*/
				else if((CmdSTARTCURRLOOP == Cmsg.msg)&&(m1CriPara.CtrlMode == MODE_CURR))
				{
					m1CriPara.IqRef10mA = LIMITMAXMIN(Cmsg.msgPara1,m1MotorPara.RatedCurrentQ_10mA,
												-m1MotorPara.RatedCurrentQ_10mA);
					m1CriPara.IdRef10mA = 0;
				}
				/* �ٶ�ģʽ�¸����ٶ�Ŀ��ָ��*/
				else if((CmdSTARTSPDLOOP == Cmsg.msg)&&(m1CriPara.CtrlMode == MODE_SPD))
				{
					m1CriPara.SpdRef = LIMITMAXMIN(Cmsg.msgPara1,m1MotorPara.RatedSpd,-m1MotorPara.RatedSpd);
					/* �ٶ�ģʽ�£�IqRef10mA�����£��������ж��У�ȡ�ٶȻ������ΪQ������������ΪIqCmd*/
					/*  �ٶ�ģʽ�£�IdRef10mA�����£��������ж��У���FwLimOutת���ٺ���ΪIdCmd*/
				}
				else
				{
					MC_NotifyCmdDisCard(Cmsg.msg);
				}
			}			
			break;
			
		case STS_MainTenance:
			GetMessge(&Cmsg);
			/* ά��ģʽ�µ�������Ӧ�����Ӻ���������*/
			if(MC_SubStateMaintenance(&Cmsg,&m1State, &m1CriPara, &m1FaultPara, &m1HalPara))
			{
				MC_ChangeMainState(&m1State,STS_Idle);
			}
			break;
			
		case STS_FaultNow:
			if(m1CriPara.FaultNow == 0)
			{
				MC_ChangeMainState(&m1State,STS_FaultOver);		
			}
			// V1.7���ӹ�����ģʽ����ת��ά��
			if(GetMessge(&Cmsg))
			{
				if(CmdGoMT == Cmsg.msg)
				{
					MC_ChangeMainState(&m1State,STS_MainTenance);
				}
				else
				{
					MC_NotifyCmdDisCard(Cmsg.msg);
				}
			}
			break;	
			
		case STS_FaultOver:
			if(GetMessge(&Cmsg))
			{
				/* ��Ӧ�����������*/
				if(CmdCLRERR == Cmsg.msg)
				{
					m1CriPara.FaultOccurred = 0;
					MC_ChangeMainState(&m1State,STS_Idle);
				}
				/* ͨ��ָ�����*/
				else
				{
					MC_NotifyCmdDisCard(Cmsg.msg);
				}
			}	
			break;

		default:
			/* �쳣�����֧*/
			MC_PostCodeErr(3);
			break;


	}

}
/**************************************************************************************************************************************************
* @������		:  		MC_SubStatePwrUpWait                                                        
* @��������	: 		PwrUpWait����״̬����������TRUEʱ����״̬�߱�תIDLE������
* @������		:    	����
* ��������	:   	2020-03-07
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
bool MC_SubStatePwrUpWait(stSysState* pState, stBusVolDecoupler* pBus,stFaultPara* pFault,stHalPara* pHal)
{
	bool rtnVal = FALSE;
	switch(pState->SubState)
	{
		case 0:
			if(MC_GetHighVolReady( pBus, pFault, pHal))
			{
				pState->SubState = 1;
				pState->StateWait = 0;
			}
			break;
		case 1:
			if(pState->StateWait++ > 100)
			{
				pState->SubState = 0xFF;
				pState->StateWait = 0;
			}
			break;
		case 0xFF:
			rtnVal = TRUE;
			break;

		default:
			break;

	}
	HAL_SetPreChargeRelayClose(rtnVal);
	return rtnVal;
}
/**************************************************************************************************************************************************
* @������		:  		MC_SubStateMaintenance                                                        
* @��������	: 		Maintenance����״̬����
					1. ������״̬0���յ��˳�ά��ָ��ʱ������TRUE
* @������		:    	����
* ��������	:   	2020-03-07
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
bool MC_SubStateMaintenance(stMCMessage * pMsg,stSysState* pState,stCriticalPara* pC,stFaultPara* pFault,stHalPara* pHal)
{
	bool rtnExitMainten = FALSE;
	bool EepromSaveFail = FALSE;
	static INT32S UPhaseOffSet = 0;
	static INT32S VPhaseOffSet = 0;
	static INT32S UVBemfOffSet = 0;
	static INT32S VWBemfOffSet = 0;
	static Uint32 TmpPosOffSet = 0;
	static INT32S TmpVol1 = 0;
	static INT32S TmpVol2 = 0;
	INT32S TmpVol_10mV = 0;
	INT32S TmpCurr_10mA = 0;
	// V1.8ά��ģʽ��ֹͣ�������ת����ֹͣ�궨����
	if(pMsg->msg == CmdSTOPMOTOR)
	{
		pMsg->msg = CmdSTOPCALIB;
	}
	else if(pMsg->msg == CmdCLRERR)/* ��Ӧά��ģʽ�µ������������*/
	{
		m1CriPara.FaultOccurred = 0;
	}
	else
	{}			
	switch(pState->SubState)
	{
		case 0:/* ����*/
			if(CmdSAVPARA == pMsg->msg)
			{
				pState->SubState = 1;
			}
			else if(CmdDoADCCALIB == pMsg->msg)
			{
				pState->SubState = 3;
				pState->StateWait = 0;
				UPhaseOffSet = 0;
				VPhaseOffSet = 0;
				UVBemfOffSet = 0;
				VWBemfOffSet = 0;
			}
			else if(CmdDoPOSOFFCALIB == pMsg->msg)
			{
				pC->IdRef10mA = LIMITMAXMIN(pMsg->msgPara1,m1MotorPara.RatedCurrentD_10mA,0);
				pC->IqRef10mA = 0;
				pState->SubState = 5;
				pState->StateWait = 0;
				TmpPosOffSet = 0;
				m1CriPara.GSts.bit.CaliPosOffsetSucc = 0;
			}
			else if(CmdDoSPDCALIB == pMsg->msg)
			{
				pState->SubState = 7;
				pState->StateWait = 0;
				/*  �ٶȷ����궨��ʹ��������ٶȿ��Ʋ���*/
				m1SpdLoopCtler.SpdKp = 20;
				m1SpdLoopCtler.SpdKi = 1;
			}
			else if(CmdDoRANDLCALIB == pMsg->msg)// V1.5����
			{
			    m1MotorCalib.CalibCurrentD_10mA = LIMITMAXMIN(pMsg->msgPara1,m1MotorPara.RatedCurrentD_10mA,0);
				pC->IdRef10mA = m1MotorCalib.CalibCurrentD_10mA/2;
				pC->IqRef10mA = 0;
				pState->SubState = 9;
				pState->StateWait = 0;
				TmpVol1 = 0;
				TmpVol2 = 0;
			}
			else if(CmdDiagHfi == pMsg->msg)// V1.8����
			{
				pC->IdRef10mA = 500;// 5A
				pC->IqRef10mA = 0;
				pState->SubState = 11;
				pState->StateWait = 0;
			}
			else if(CmdOutMT == pMsg->msg)
			{
				rtnExitMainten = TRUE;
			}
			else if(CmdDoPOSOFFCALIB2 == pMsg->msg)
			{
				//pC->IdRef10mA = LIMITMAXMIN(pMsg->msgPara1,m1MotorPara.RatedCurrentD_10mA,0);
				//pC->IqRef10mA = 0;
				pState->SubState = 13;
				pState->StateWait = 0;
				TmpPosOffSet = 0;
				m1CriPara.GSts.bit.CaliPosOffsetSucc = 0;
			}
			else if(CmdSTOPCALIB == pMsg->msg)
			{
				/* ά�ֵ�ǰ��״̬0*/
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}
			break;
			
		case 1:/* ����д��EEPROM*/
			MC_NotifyCmdDisCard(pMsg->msg);/*  ����Ӧ����*/
			if(!SavePara((void *)&m1MotorPara, stEEPROMParaArr, MOTOR_PARA))
			{
				EepromSaveFail = TRUE;
			}
			if(!SavePara((void *)&m1CtrlPara, stEEPROMParaArr, CTRL_PARA))
			{
				EepromSaveFail = TRUE;
			}
			if(!SavePara((void *)&m1FaultPara, stEEPROMParaArr, FAULT_PARA))
			{
				EepromSaveFail = TRUE;
			}
			if(!SavePara((void *)&m1WarnPara, stEEPROMParaArr, WARN_PARA))
			{
				EepromSaveFail = TRUE;
			}
			if(!SavePara((void *)&m1SnsLessPara, stEEPROMParaArr, SNSLESS_PARA))
			{
				EepromSaveFail = TRUE;
			}
			if(!SavePara((void *)&m1HalPara, stEEPROMParaArr, HAL_PARA))
			{
				EepromSaveFail = TRUE;
			}
			if(!SavePara((void *)&m1SpdFdbPara, stEEPROMParaArr, SPDFDB_PARA))
			{
				EepromSaveFail = TRUE;
			}
			if(!SavePara((void *)&m1SysCfgPara, stEEPROMParaArr, SYSCFG_PARA))
			{
				EepromSaveFail = TRUE;
			}
			
			if(EepromSaveFail == TRUE)
			{
				pC->GSts.bit.SaveEepromFailWarn = 1;
			}
			else
			{
    			m1CriPara.MsgBox.bit.MsgBoxSaveParaSucc = 1;
			}
			pState->SubState = 0;
			break;
			
			
		case 3:/* ADC�ɼ�ֵ��У��*/		
			MC_NotifyCmdDisCard(pMsg->msg);/*  ����Ӧ����*/
			pState->StateWait ++;
			UPhaseOffSet += m1CurrLoopCtler.IOriU;
			VPhaseOffSet += m1CurrLoopCtler.IOriW;
			UVBemfOffSet += GetADResultModifyDef(AD_BEMF_UV,m1HalPara.ADGain,m1HalPara.ADOff)  << 2;
			VWBemfOffSet += GetADResultModifyDef(AD_BEMF_VW,m1HalPara.ADGain,m1HalPara.ADOff)  << 2;
			if(pState->StateWait == 32)
			{
				m1HalPara.UCurrOff = UPhaseOffSet>>5;
				m1HalPara.VCurrOff = VPhaseOffSet>>5;
				m1HalPara.UVBemfOff = UVBemfOffSet >> 5;
				m1HalPara.VWBemfOff = VWBemfOffSet >> 5;
				m1CurrLoopCtler.IOffU = m1HalPara.UCurrOff;
				m1CurrLoopCtler.IOffW = m1HalPara.VCurrOff;
				pState->SubState = 0;
			}
			break;
			
		case 5:/* ��ʼ�Ǳ궨,V1.5���Ż����ɼ�����ʱ���λ��*/			
			pState->StateWait ++;
			if(pState->StateWait < 2048)/* 0~2047*/
			{
			    TmpPosOffSet = 0;
			}
			/* ǰһ��ʱ�䲻�ɼ�λ�ã�λ��δ�ȶ�*/
			else if((pState->StateWait >= 2048)&&(pState->StateWait < 4096))/* 2048~4095*/
			{
			    TmpPosOffSet += m1ResolverDealModule.ResolverElecPos;
			}
			else
			{
				m1MotorPara.PosOff = TmpPosOffSet>>11;/* 2048���ɼ�������ƽ��*/
				m1CurrLoopCtler.PosOff = m1MotorPara.PosOff;
				pState->SubState = 0;
				m1CriPara.GSts.bit.CaliPosOffsetSucc = 1;
			}
			/* ��Ӧ�궨ֹͣ*/
			if(CmdSTOPCALIB == pMsg->msg)
			{
				pState->SubState = 0;
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}
			break;
			
		case 7:/* �ٶȷ���У��*/			
			
			m1CriPara.SpdRef = -1000;/* �궨��ת��*/
			
			//pState->StateWait ++;			
			/* ��Ӧ�궨ֹͣ*/
			if(CmdSTOPCALIB == pMsg->msg)
			{
				pState->SubState = 0;
				/*  �ٶȷ����궨��ɺ󣬻ָ��ٶȿ��Ʋ���*/
				m1SpdLoopCtler.SpdKp = m1CtrlPara.SpdInitKp;
				m1SpdLoopCtler.SpdKi = m1CtrlPara.SpdKi;
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}
			break;
			//MC_NotifyCmdDisCard(pMsg->msg);/*  ����Ӧ����*/
			//pState->SubState = 0;
			/* ������*/
			break;
			
		case 9:/* ������в���*/			
			pState->StateWait ++;
			if(pState->StateWait < 1024)/* 0~1023*/
			{
			    TmpVol1 = 0;
			    TmpVol2 = 0;
			}
			else if((pState->StateWait >= 1024)&&(pState->StateWait < 2048))/* 1024~2048*/
			{
			    TmpVol1 += m1CurrLoopCtler.UdOut_BIT;
			}
			else if((pState->StateWait >= 2048)&&(pState->StateWait < 3072))/* 2048~3071*/
			{
			    /* ��������ӱ�*/
		        pC->IdRef10mA = m1MotorCalib.CalibCurrentD_10mA ;
			}
			else if((pState->StateWait >= 3072)&&(pState->StateWait < 4096))/* 3072~4095*/
			{
			    TmpVol2 += m1CurrLoopCtler.UdOut_BIT;
			}
			else if(pState->StateWait == 4096)/* 4096*/
			{
			    /* �ȴ��ж���ִ�е����ĸ�����������*/
			}
			else/* 4096~*/
			{
			    pState->SubState = 0;
			    /* ��һ�׶ε�ѹƽ��ֵ*/
			    m1MotorCalib.RCalibVol1_10mV = ((TmpVol1>>10)*(INT32S)m1CriPara.HpVol)>>13;
			    //m1MotorCalib.RCalibVol1_10mV = (TmpVol1>>10);
			    /* �ڶ��׶ε�ѹƽ��ֵ*/
			    m1MotorCalib.RCalibVol2_10mV = ((TmpVol2>>10)*(INT32S)m1CriPara.HpVol)>>13;
			    //m1MotorCalib.RCalibVol2_10mV = (TmpVol2>>10);
			    /* ���ε�ѹ����*/
			    TmpVol_10mV = m1MotorCalib.RCalibVol2_10mV - m1MotorCalib.RCalibVol1_10mV;
			    /* ���ε�ѹ���������㵽����*/
			    TmpVol_10mV = (TmpVol_10mV * 4730) >> 13;// 1/SQRT(3)
			    /* ���������*/
			    m1MotorCalib.PhaseRes_100uohm = (2*10000*TmpVol_10mV)/m1MotorCalib.CalibCurrentD_10mA;
			    m1MotorPara.PhaseRes_100uohm = m1MotorCalib.PhaseRes_100uohm;
			    /* ��������*/
			    TmpCurr_10mA = m1MotorCalib.SampleCurr_10mA[1] - m1MotorCalib.SampleCurr_10mA[2];
			    if(TmpCurr_10mA > 100)/*��������1A������Ϊ������Ч*/
			    {
			        TmpCurr_10mA = (INT32S)m1MotorCalib.CurrLoopSampleTime_us * (INT32S)m1CriPara.HpVol/TmpCurr_10mA;
			        m1MotorCalib.PhaseIndD_uH = (TmpCurr_10mA * 2)/3;
			        m1MotorPara.PhaseIndD_uH = m1MotorCalib.PhaseIndD_uH;
			    }
			    else
			    {
			        /* �϶�������Ч�����޸�ԭ�����ֵ*/
			    }
			}
			/* ��Ӧ�궨ֹͣ*/
			if(CmdSTOPCALIB == pMsg->msg)
			{
				pState->SubState = 0;
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}
			break;
			
		case 11:/* HFI�������� V1.8����*/				
		            /* ��Ӧ�궨ֹͣ*/
			if(CmdSTOPCALIB == pMsg->msg)
			{
				pState->SubState = 0;
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}			
			break;
			
		case 13:/*   ��ת�궨��ʼ�Ƕ� : ��ʼ���궨����*/				
			MC_InitStartUp(&m1CtrlPara, &m1SnsLessPara, &m1MotorPara, &m1StartUp);
			pState->SubState = 14;
			break;
			
		case 14:/*   ��ת�궨��ʼ�Ƕ� : Ԥ��λ��״̬ת�����ж���*/				
			if(CmdSTOPCALIB == pMsg->msg)
			{
				pState->SubState = 0;
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}			
			break;
			
		case 15:/*   ��ת�궨��ʼ�Ƕ� : ��ת��״̬ת�����ж���*/				
			if(CmdSTOPCALIB == pMsg->msg)
			{
				pState->SubState = 0;
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}			
			break;
			
		case 16:/*  ��ת���趨ת��*/	
			if(m1StartUp.StartUpSuccess == 0)/* �ȴ����ϼ���߼�ת������ʧ�ܹ���*/
			{				
			}			
			else/* ת��17��״̬,��ȡ������*/
			{
				InitPosCalib2(&m1PosCalib);
				pState->SubState = 17;
			}
			break;
			
		case 17:/*   ��ȡ������*/	
			/* ���ж��м�ⷴ���ƵĹ���*/
			if(m1PosCalib.BemfCnt > BEMF_CHECK_TIMES)
			{
				pState->SubState = 0;
			}
			else
			{}/* �ȴ��������жϵ����������*/
			break;
			
		case 0xFF:			
			MC_NotifyCmdDisCard(pMsg->msg);/*  ����Ӧ����*/
			pState->SubState = 0;
			break;		

		default:
			MC_NotifyCmdDisCard(pMsg->msg);/*  ����Ӧ����*/
			pState->SubState = 0;
			break;
	}
	return rtnExitMainten;
}
/**************************************************************************************************************************************************
* @������		:  		MC_ChangeMainState                                                        
* @��������	: 		�޸���״̬��ͬʱ����״̬�ó�0
* @������		:    	����
* ��������	:   	2020-03-07
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
bool MC_ChangeMainState(stSysState* pState,State_t NewMainState)
{
	pState->MainState = NewMainState;
	/* ��״̬�ı�ʱ����״̬ǿ��Ϊ0*/
	pState->SubState = 0;
	return TRUE;
}

