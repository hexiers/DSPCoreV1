/*======================================================================================================
* @�ļ�����		:                                                       
* @�ļ�����		: 	���ڲ�������
* @������		:  	����                                                                              
* ��������		:   	2020-03-03
* �޸ļ�¼		:
*                         
======================================================================================================*/
#include"MCSoft_SystemInit.h"
extern bool DSPCore_CfgFwCtler(stFwCtler* pD, stCtrlPara* pC);
/**************************************************************************************************************************************************
* @������		:  		MC_InitPara                                                        
* @��������		: 		1. ����EEPROM�д洢�Ĳ����ṹ��
							2. ������ʧ�ܣ����������д洢��Ĭ�Ͽ��Ʋ���
							3. �������нṹ������سɹ�������λEepromErr�ź�
						
* @������		:    		����
* ��������		:   		2020-03-10
* �޸ļ�¼		:
*                         
****************************************************************************************************************************************************/
bool MC_InitPara(void)
{
	bool rtnLoadParaSuccess = TRUE;
	m1CriPara.Eeprom.all = 0;
	/* 1. ��������ṹ��*/
	if(LoadPara((void *)&m1MotorPara, stEEPROMParaArr, MOTOR_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromMotorPara = 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1MotorPara, stEEPROMParaArr, MOTOR_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 2. ���Ʋ����ṹ��*/
	if(LoadPara((void *)&m1CtrlPara, stEEPROMParaArr, CTRL_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromCtrlPara = 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1CtrlPara, stEEPROMParaArr, CTRL_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 3. ���ϲ����ṹ��*/
	if(LoadPara((void *)&m1FaultPara, stEEPROMParaArr, FAULT_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromFaultPara= 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1FaultPara, stEEPROMParaArr, FAULT_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 4. �澯�����ṹ��*/
	if(LoadPara((void *)&m1WarnPara, stEEPROMParaArr, WARN_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromWarnPara= 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1WarnPara, stEEPROMParaArr, WARN_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 5. ��λ�ÿ��Ʋ����ṹ��*/
	if(LoadPara((void *)&m1SnsLessPara, stEEPROMParaArr, SNSLESS_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromSnslessPara= 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1SnsLessPara, stEEPROMParaArr, SNSLESS_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 6.  HAL�����ṹ��*/
	if(LoadPara((void *)&m1HalPara, stEEPROMParaArr, HAL_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromHalPara= 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1HalPara, stEEPROMParaArr, HAL_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 7. �ٶȷ������������ṹ��*/
	if(LoadPara((void *)&m1SpdFdbPara, stEEPROMParaArr, SPDFDB_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromSpdFdbPara= 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1SpdFdbPara, stEEPROMParaArr, SPDFDB_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	/* 9. ϵͳ���ò����ṹ��*/
	if(LoadPara((void *)&m1SysCfgPara, stEEPROMParaArr, SYSCFG_PARA))
	{
		m1CriPara.Eeprom.bit.UseEepromSysCfgPara= 1;
	}
	else
	{
		LoadDefaultPara((void *)&m1SysCfgPara, stEEPROMParaArr, SYSCFG_PARA);
		rtnLoadParaSuccess = FALSE;
	}
	
	if(rtnLoadParaSuccess == FALSE)
	{
		m1CriPara.Eeprom.bit.EepromErr = 1;
	}
	return rtnLoadParaSuccess;
}
/**************************************************************************************************************************************************
* @������		:  		MC_InitMCCore                                                        
* @��������		: 		1. Ӧ�ڵ�����MC_InitPara���ٵ��øú���
							2. �ú���������ɶ�FPGAʵ�ֵ�Ӳ��IP�ĳ�ʼ��
							3. ����ʼ��ʧ���򷵻�FALSE,����FpgaFail�ź���Ϊ1
* @������		:    		����
* ��������		:   		2020-03-10
* �޸ļ�¼		:
*                         
****************************************************************************************************************************************************/
bool	MC_InitMCCore(void)
{

	bool rtnCfgMCCoreSuccess = TRUE;
	/*  1. ����MCCore�ĵ�����������*/
	if(!CfgCurrLoopCtler(&m1CurrLoopCtler,&m1CtrlPara,&m1MotorPara,&m1HalPara))
	{		
		rtnCfgMCCoreSuccess = FALSE;
	}
	else
	{
		DSPCore_CfgCurrLoopCtler(&m1CurrLoopCtler); 
	}
	/*  2. ����MCCore���ٶȻ�������*/
	if(!CfgSpdLoopCtler(&m1SpdLoopCtler,&m1CtrlPara,&m1MotorPara,&m1HalPara))
	{
		rtnCfgMCCoreSuccess = FALSE;
	}
	else
	{
		DSPCore_CfgFwCtler(&m1FwCtler,&m1CtrlPara);
	}
	/*  3. ����MCCore��ĸ�ߵ�ѹ����ģ��*/
	if(!CfgBusVolDecoupler(&m1BusVolDecoupler,&m1HalPara))
	{
		rtnCfgMCCoreSuccess = FALSE;
	}	
	/*  4. ����MCCore�Ļ�ģ�۲���*/
	//if(!CfgSmcObserver(&m1SmcObserver,&m1CtrlPara,&m1MotorPara,&m1HalPara,&m1SnsLessPara))
	//{
	//	rtnCfgMCCoreSuccess = FALSE;
	//}	
	/*  5. ����MCCore������ӿ�ģ��*/
	if(!CfgResolverDealModule(&m1ResolverDealModule,&m1CurrLoopCtler,&m1MotorPara))
	{
		rtnCfgMCCoreSuccess = FALSE;
	}	
	/*  6. ����MCCore�Ĺ��ϴ���ģ��*/
	DSPCore_CfgFaultDealModule(&m1FaultDealModule);
	if(!CfgFaultDealModule(&m1FaultDealModule,&m1FaultPara,&m1HalPara))
	{
		rtnCfgMCCoreSuccess = FALSE;
	}	
	else
	{

	}
	
	if(rtnCfgMCCoreSuccess == FALSE)
	{
		m1CriPara.GSts.bit.FpgaFailFault = 1;
	}
	return rtnCfgMCCoreSuccess;
	
}
/**************************************************************************************************************************************************
* @������		:  		MC_InitCriticalData                                                        
* @��������		: 		1.  CPU�ؼ������ĳ�ʼ��						 
* @������		:    		����
* ��������		:   		2020-03-10
* �޸ļ�¼		:
*                         
****************************************************************************************************************************************************/
bool MC_InitCriticalData(stCriticalPara* pC)
{
	bool rtn = FALSE;
	pC->IdRef10mA = 0;
	pC->IqRef10mA = 0;
	pC->SpdRef = 0;
	pC->SpdRefSlp = 0;
	pC->SpdOut = 0;
	pC->UdRef = 0;
	pC->UqRef = 0;
	pC->IdFdb = 0;
	pC->IqFdb = 0;
	pC->SpdFdb = 0;	
	pC->HpVol = 0;
	pC->IBus = 0;
	pC->Power = 0;
	pC->LpVol = 0;
	pC->TmpPdd = 0;
	pC->TmpMotor = 0;
	pC->CtrlMode = MODE_CURR;
	pC->FaultNow = 0;
	pC->FaultOccurred= 0;
	pC->WarnNow= 0;
	pC->SoftErrCode= 0; 	/* MC_PostCodeErr�����׳������������*/
	pC->SoftWarnCode = 0;
	pC->DiscardCmdMsg = CmdNOCMD;
	pC->Eeprom.all = 0;	/* EEPROM���ؽṹָʾ*/
	pC->GSts.all = 0;		/*  ȫ��״̬���ź�*/
	pC->MsgBox.all = 0;
	return rtn;
	
}

