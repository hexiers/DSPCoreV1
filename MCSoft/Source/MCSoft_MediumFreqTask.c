/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         20201010 : 增加反电势标定初始角的内容
			20201012:增加速度反馈标定功能，但控制时未用
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
	//数据发送
	//查询发送FIFO的空余字节数目
	Temp = 16 - SciaRegs.SCIFFTX.bit.TXFFST;

	//以FIFO可用字数做循环次数
	for(i = 0;i < Temp;i++)
	{
		SciaRegs.SCITXBUF = TxBuf[TX_FRAME_BYTE_LEN - TxBufLen];
		TxBufLen--;

		//已经发送完一帧后
		if(TxBufLen == 0)
		{
			break;
		}
	}

}
/**************************************************************************************************************************************************
* @函数名		:  		MC_TaskLoop                                                        
* @函数描述	: 		1. 循环任务调度
* @创建人		:    	张宇
* 创建日期	:   	2020-03-14
* 修改记录	:
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
		/* 发送完一帧数据后，立即清除消息标记*/
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
* @函数名		:  		MC_GetHighVolReady                                                        
* @函数描述	: 		1. 判断高压动力电源是否高压欠压故障点，是返回正
					2. 作为结束PwrUpWait状态的必要条件
* @创建人		:    	张宇
* 创建日期	:   	2020-03-07
* 修改记录	:
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
* @函数名		:  		MC_StateInit                                                        
* @函数描述	: 		1. 初始化状态机器
					2. 应在main 函数初始化时候调用
* @创建人		:    	张宇
* 创建日期	:   	2020-03-07
* 修改记录	:
*                         
****************************************************************************************************************************************************/
void MC_StateInit(stSysState* pState)
{
	pState->MainState = STS_PwrUpWait;
	pState->SubState = 0;
	pState->StateWait = 0;
}


/**************************************************************************************************************************************************
* @函数名		:  		MC_MainStateTask                                                        
* @函数描述	: 		主状态管理
* @创建人		:    	张宇
* 创建日期	:   	2020-03-07
* 修改记录	:
*                         
****************************************************************************************************************************************************/
void MC_MainStateTask(void)
{
	bool rtnVal = FALSE;
	stMCMessage Cmsg;

	/*  更新关键的控制数据，为故障检测做准备，同时也为数据上传做准备*/
	UpDateCriticalSampleData_MediumFreq(&m1CriPara);
	
	/*  CPU处理的故障检测*/
	m1CriPara.FaultNow = MC_DoFaultCheck(&m1SysCfgPara,&m1State,&m1FaultPara,&m1CriPara);
	/*  故障的锁存*/
	m1CriPara.FaultOccurred   |=  m1CriPara.FaultNow;
	/*  CPU处理的故障告警*/	
	m1CriPara.WarnNow = MC_DoWarnCheck(&m1SysCfgPara,&m1State,&m1WarnPara,&m1CriPara);

	/*  状态转故障模式具有最高优先级*/
	/* 上电等待状态不转故障，仅报故障信息，无故障时自行清除故障*/
	if(m1State.MainState == STS_PwrUpWait)
	{
		m1CriPara.FaultOccurred = m1CriPara.FaultNow;
	}
	else if((m1CriPara.FaultNow)&&(m1State.MainState == STS_MainTenance))
	{
		//V1.7 维护模式有PWM输出的子状态转维护模式0子状态
		if((m1State.SubState == 5)||(m1State.SubState == 9)||(m1State.SubState == 11)
			||(m1State.SubState == 14)||(m1State.SubState == 15)||(m1State.SubState == 16))
		{
			MC_ChangeMainState(&m1State,STS_MainTenance);
		}
		else
		{
			//维护模式有故障，只要没有PWM输出，仍然维持原子状态
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
			/* 桥臂自检功能预留*/
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
				;/* 维持前子状态*/
			}
			if(GetMessge(&Cmsg))
			{
				if(CmdGoMT == Cmsg.msg)
				{
					MC_ChangeMainState(&m1State,STS_MainTenance);
				}
				else if((CmdSTARTCURRLOOP == Cmsg.msg)||(CmdSTARTSPDLOOP == Cmsg.msg))
				{	
					/* 电流模式*/
					if(CmdSTARTCURRLOOP == Cmsg.msg)
					{
						m1CriPara.CtrlMode = MODE_CURR;
						m1CriPara.IdRef10mA = 0;
						m1CriPara.IqRef10mA = LIMITMAXMIN(Cmsg.msgPara1,m1MotorPara.RatedCurrentQ_10mA,
													-m1MotorPara.RatedCurrentQ_10mA);
						m1CriPara.SpdRef = 0;
					}
					/* 速度模式*/
					else
					{
						m1CriPara.CtrlMode = MODE_SPD;
						m1CriPara.IdRef10mA = 0;
						m1CriPara.IqRef10mA = 0;
						m1CriPara.SpdRef = LIMITMAXMIN(Cmsg.msgPara1,m1MotorPara.RatedSpd,-m1MotorPara.RatedSpd);
					}
					
					/* 允许非零转速启动，或者当前转速已经为0 */
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
					/* 在当前转速非零，但不允许非零启动时，接收到启动信号后丢弃当前启动指令*/
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
			/* 响应停机命令*/
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
			/* 初始化开环启动数据*/
			if(m1State.SubState == 0)
			{
				MC_InitStartUp(&m1CtrlPara, &m1SnsLessPara, &m1MotorPara, &m1StartUp);
				m1State.SubState = 1;
			}
			/* 实时任务在中断中执行，子状态1-2 和2->3的转移在高频任务中*/
			else if((m1State.SubState == 1)||(m1State.SubState == 2))
			{				
			}
			/* 启动结束后，根据DoNotSwitch、StartUpSuccess确定状态转移操作*/
			else if(m1State.SubState == 3)
			{
				/* 等待故障检测逻辑转入启动失败故障*/
				if(m1StartUp.StartUpSuccess == 0)
				{				
				}
				/* 维持I/F控制的结束状态*/
				else if(m1SysCfgPara.SysCfgWord1.bit.DoNotSwitch)
				{					
				}
				/* 转入RUN状态*/
				else
				{
					MC_ChangeMainState(&m1State,STS_Run);
				}
			}
			else
			{
				/* 其它无效子状态*/
				MC_PostCodeErr(2);
			}
			break;
			
		case STS_Run:			
			if(GetMessge(&Cmsg))
			{
				/* 响应停机命令*/
				if(CmdSTOPMOTOR == Cmsg.msg)
				{
					MC_ChangeMainState(&m1State,STS_Idle);
				}
				/* 电流模式下更新电流目标指令*/
				else if((CmdSTARTCURRLOOP == Cmsg.msg)&&(m1CriPara.CtrlMode == MODE_CURR))
				{
					m1CriPara.IqRef10mA = LIMITMAXMIN(Cmsg.msgPara1,m1MotorPara.RatedCurrentQ_10mA,
												-m1MotorPara.RatedCurrentQ_10mA);
					m1CriPara.IdRef10mA = 0;
				}
				/* 速度模式下更新速度目标指令*/
				else if((CmdSTARTSPDLOOP == Cmsg.msg)&&(m1CriPara.CtrlMode == MODE_SPD))
				{
					m1CriPara.SpdRef = LIMITMAXMIN(Cmsg.msgPara1,m1MotorPara.RatedSpd,-m1MotorPara.RatedSpd);
					/* 速度模式下，IqRef10mA不更新，而是在中断中，取速度环输出作为Q电流给定，作为IqCmd*/
					/*  速度模式下，IdRef10mA不更新，而是在中断中，用FwLimOut转量纲后，作为IdCmd*/
				}
				else
				{
					MC_NotifyCmdDisCard(Cmsg.msg);
				}
			}			
			break;
			
		case STS_MainTenance:
			GetMessge(&Cmsg);
			/* 维护模式下的命令响应均在子函数里面做*/
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
			// V1.7增加故障中模式可以转入维护
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
				/* 响应清除故障命令*/
				if(CmdCLRERR == Cmsg.msg)
				{
					m1CriPara.FaultOccurred = 0;
					MC_ChangeMainState(&m1State,STS_Idle);
				}
				/* 通告指令被丢弃*/
				else
				{
					MC_NotifyCmdDisCard(Cmsg.msg);
				}
			}	
			break;

		default:
			/* 异常程序分支*/
			MC_PostCodeErr(3);
			break;


	}

}
/**************************************************************************************************************************************************
* @函数名		:  		MC_SubStatePwrUpWait                                                        
* @函数描述	: 		PwrUpWait的子状态管理，当返回TRUE时，主状态具备转IDLE的条件
* @创建人		:    	张宇
* 创建日期	:   	2020-03-07
* 修改记录	:
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
* @函数名		:  		MC_SubStateMaintenance                                                        
* @函数描述	: 		Maintenance的子状态管理
					1. 当在子状态0接收到退出维护指令时，返回TRUE
* @创建人		:    	张宇
* 创建日期	:   	2020-03-07
* 修改记录	:
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
	// V1.8维护模式，停止电机命令转换的停止标定命令
	if(pMsg->msg == CmdSTOPMOTOR)
	{
		pMsg->msg = CmdSTOPCALIB;
	}
	else if(pMsg->msg == CmdCLRERR)/* 响应维护模式下的清除故障命令*/
	{
		m1CriPara.FaultOccurred = 0;
	}
	else
	{}			
	switch(pState->SubState)
	{
		case 0:/* 空闲*/
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
				/*  速度反馈标定，使用最柔和速度控制参数*/
				m1SpdLoopCtler.SpdKp = 20;
				m1SpdLoopCtler.SpdKi = 1;
			}
			else if(CmdDoRANDLCALIB == pMsg->msg)// V1.5新增
			{
			    m1MotorCalib.CalibCurrentD_10mA = LIMITMAXMIN(pMsg->msgPara1,m1MotorPara.RatedCurrentD_10mA,0);
				pC->IdRef10mA = m1MotorCalib.CalibCurrentD_10mA/2;
				pC->IqRef10mA = 0;
				pState->SubState = 9;
				pState->StateWait = 0;
				TmpVol1 = 0;
				TmpVol2 = 0;
			}
			else if(CmdDiagHfi == pMsg->msg)// V1.8新增
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
				/* 维持当前子状态0*/
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}
			break;
			
		case 1:/* 参数写入EEPROM*/
			MC_NotifyCmdDisCard(pMsg->msg);/*  不响应命令*/
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
			
			
		case 3:/* ADC采集值的校正*/		
			MC_NotifyCmdDisCard(pMsg->msg);/*  不响应命令*/
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
			
		case 5:/* 初始角标定,V1.5有优化，采集后半段时间的位置*/			
			pState->StateWait ++;
			if(pState->StateWait < 2048)/* 0~2047*/
			{
			    TmpPosOffSet = 0;
			}
			/* 前一段时间不采集位置，位置未稳定*/
			else if((pState->StateWait >= 2048)&&(pState->StateWait < 4096))/* 2048~4095*/
			{
			    TmpPosOffSet += m1ResolverDealModule.ResolverElecPos;
			}
			else
			{
				m1MotorPara.PosOff = TmpPosOffSet>>11;/* 2048个采集周期求平均*/
				m1CurrLoopCtler.PosOff = m1MotorPara.PosOff;
				pState->SubState = 0;
				m1CriPara.GSts.bit.CaliPosOffsetSucc = 1;
			}
			/* 响应标定停止*/
			if(CmdSTOPCALIB == pMsg->msg)
			{
				pState->SubState = 0;
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}
			break;
			
		case 7:/* 速度反馈校正*/			
			
			m1CriPara.SpdRef = -1000;/* 标定用转速*/
			
			//pState->StateWait ++;			
			/* 响应标定停止*/
			if(CmdSTOPCALIB == pMsg->msg)
			{
				pState->SubState = 0;
				/*  速度反馈标定完成后，恢复速度控制参数*/
				m1SpdLoopCtler.SpdKp = m1CtrlPara.SpdInitKp;
				m1SpdLoopCtler.SpdKi = m1CtrlPara.SpdKi;
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}
			break;
			//MC_NotifyCmdDisCard(pMsg->msg);/*  不响应命令*/
			//pState->SubState = 0;
			/* 待开发*/
			break;
			
		case 9:/* 相电阻电感测试*/			
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
			    /* 输出电流加倍*/
		        pC->IdRef10mA = m1MotorCalib.CalibCurrentD_10mA ;
			}
			else if((pState->StateWait >= 3072)&&(pState->StateWait < 4096))/* 3072~4095*/
			{
			    TmpVol2 += m1CurrLoopCtler.UdOut_BIT;
			}
			else if(pState->StateWait == 4096)/* 4096*/
			{
			    /* 等待中断中执行电流的高速连续采样*/
			}
			else/* 4096~*/
			{
			    pState->SubState = 0;
			    /* 第一阶段电压平均值*/
			    m1MotorCalib.RCalibVol1_10mV = ((TmpVol1>>10)*(INT32S)m1CriPara.HpVol)>>13;
			    //m1MotorCalib.RCalibVol1_10mV = (TmpVol1>>10);
			    /* 第二阶段电压平均值*/
			    m1MotorCalib.RCalibVol2_10mV = ((TmpVol2>>10)*(INT32S)m1CriPara.HpVol)>>13;
			    //m1MotorCalib.RCalibVol2_10mV = (TmpVol2>>10);
			    /* 二段电压增量*/
			    TmpVol_10mV = m1MotorCalib.RCalibVol2_10mV - m1MotorCalib.RCalibVol1_10mV;
			    /* 二段电压增量，换算到相上*/
			    TmpVol_10mV = (TmpVol_10mV * 4730) >> 13;// 1/SQRT(3)
			    /* 计算相电阻*/
			    m1MotorCalib.PhaseRes_100uohm = (2*10000*TmpVol_10mV)/m1MotorCalib.CalibCurrentD_10mA;
			    m1MotorPara.PhaseRes_100uohm = m1MotorCalib.PhaseRes_100uohm;
			    /* 计算相电感*/
			    TmpCurr_10mA = m1MotorCalib.SampleCurr_10mA[1] - m1MotorCalib.SampleCurr_10mA[2];
			    if(TmpCurr_10mA > 100)/*增量大于1A，才认为整定有效*/
			    {
			        TmpCurr_10mA = (INT32S)m1MotorCalib.CurrLoopSampleTime_us * (INT32S)m1CriPara.HpVol/TmpCurr_10mA;
			        m1MotorCalib.PhaseIndD_uH = (TmpCurr_10mA * 2)/3;
			        m1MotorPara.PhaseIndD_uH = m1MotorCalib.PhaseIndD_uH;
			    }
			    else
			    {
			        /* 认定整定无效，不修改原来电感值*/
			    }
			}
			/* 响应标定停止*/
			if(CmdSTOPCALIB == pMsg->msg)
			{
				pState->SubState = 0;
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}
			break;
			
		case 11:/* HFI参数整定 V1.8新增*/				
		            /* 响应标定停止*/
			if(CmdSTOPCALIB == pMsg->msg)
			{
				pState->SubState = 0;
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}			
			break;
			
		case 13:/*   旋转标定初始角度 : 初始化标定参数*/				
			MC_InitStartUp(&m1CtrlPara, &m1SnsLessPara, &m1MotorPara, &m1StartUp);
			pState->SubState = 14;
			break;
			
		case 14:/*   旋转标定初始角度 : 预定位，状态转移在中断中*/				
			if(CmdSTOPCALIB == pMsg->msg)
			{
				pState->SubState = 0;
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}			
			break;
			
		case 15:/*   旋转标定初始角度 : 带转，状态转移在中断中*/				
			if(CmdSTOPCALIB == pMsg->msg)
			{
				pState->SubState = 0;
			}
			else
			{
				MC_NotifyCmdDisCard(pMsg->msg);
			}			
			break;
			
		case 16:/*  带转至设定转速*/	
			if(m1StartUp.StartUpSuccess == 0)/* 等待故障检测逻辑转入启动失败故障*/
			{				
			}			
			else/* 转入17子状态,读取反电势*/
			{
				InitPosCalib2(&m1PosCalib);
				pState->SubState = 17;
			}
			break;
			
		case 17:/*   读取反电势*/	
			/* 在中断中检测反电势的过零*/
			if(m1PosCalib.BemfCnt > BEMF_CHECK_TIMES)
			{
				pState->SubState = 0;
			}
			else
			{}/* 等待反电势判断的周期走完成*/
			break;
			
		case 0xFF:			
			MC_NotifyCmdDisCard(pMsg->msg);/*  不响应命令*/
			pState->SubState = 0;
			break;		

		default:
			MC_NotifyCmdDisCard(pMsg->msg);/*  不响应命令*/
			pState->SubState = 0;
			break;
	}
	return rtnExitMainten;
}
/**************************************************************************************************************************************************
* @函数名		:  		MC_ChangeMainState                                                        
* @函数描述	: 		修改主状态，同时将子状态置成0
* @创建人		:    	张宇
* 创建日期	:   	2020-03-07
* 修改记录	:
*                         
****************************************************************************************************************************************************/
bool MC_ChangeMainState(stSysState* pState,State_t NewMainState)
{
	pState->MainState = NewMainState;
	/* 主状态改变时，子状态强制为0*/
	pState->SubState = 0;
	return TRUE;
}

