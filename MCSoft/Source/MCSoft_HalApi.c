/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����
* @������		:  	����                                                                              
* ��������		:   	2020-03-09                                                                
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#include"MC_HalApi.h"
/**************************************************************************************************************************************************
* @������		:  		HAL_GetLpVol                                                       
* @��������	: 		������10mVΪ��λ�Ŀ��Ƶ�ѹֵ
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
Uint16 HAL_GetLpVol(void)
{
	/* �û����Ӵ���*/
	return 2800;
}

/**************************************************************************************************************************************************
* @������		:  		HAL_GetHpCurr                                                       
* @��������	: 		������10mAΪ��λ��ĸ�ߵ���ֵ
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
Uint16 HAL_GetHpCurr(void)
{
	/* �û����Ӵ���*/
	return 0;
}

/**************************************************************************************************************************************************
* @������		:  		HAL_GetPddTemp                                                       
* @��������	: 		������0.01��Ϊ��λ�Ĺ��ʹ��¶�
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
Uint16 HAL_GetPddTemp(void)
{
	INT16S TemX100 = 0;
	volatile Uint16 Temp = 0;
	Uint16 TempBit = 0;
	static INT16S V[26]  = {4096,4039,3934,3812,3678,3535,3431,3214,2983,2773,2576,2385,2191,1999,1806,1606,1485,1330,1186,1069,971,879,784,687,602,553};
	static INT16S T[26] = {-15,-10,-5,0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110};

	TempBit = GetADResultModifyDef(AD_TEMP1,m1HalPara.ADGain,m1HalPara.ADOff);
	//��ϵ�ѹ�¶ȹ�ϵ
	if(TempBit >= V[0])
	{
		TemX100 = 100 * T[0];
	}
	else if(TempBit <= V[25])
	{
		TemX100 = 100 * T[25];
	}
	else
	{
		for(Temp = 1;Temp < 25;Temp++)
		{
			if(TempBit > V[Temp])
			{
				break;
			}
		}
		TemX100 = (500 * (V[Temp-1]-TempBit))/(V[Temp-1] - V[Temp]) + 100 * T[Temp-1];
	}
	return TemX100;
	
}
/**************************************************************************************************************************************************
* @������		:  		HAL_GetMotorTemp                                                       
* @��������	: 		������0.01��Ϊ��λ�ĵ���¶�
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
Uint16 HAL_GetMotorTemp(void)
{
	/* �û����Ӵ���*/
	return 1000;
	
}

/**************************************************************************************************************************************************
* @������		:  		HAL_IsFpgaReady
* @��������	: 		����FPGA�Ƿ��PROM������ɣ�ֻ��FPGA������ɺ�CPU�ſ���ͨ���������߶�FPGA����
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
bool HAL_IsFpgaReady(void)
{
		return FALSE;
}

/**************************************************************************************************************************************************
* @������		:  		HAL_ReloadFpga
* @��������	: 		��λFPGA�����г�ʼ������
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
void HAL_ReloadFpga(void)
{
	GpioDataRegs.GPADAT.bit.GPIO10 = 1;
	Delay_ms(10);
	GpioDataRegs.GPADAT.bit.GPIO10 = 0;
}

/**************************************************************************************************************************************************
* @������		:  		HAL_LedToggle
* @��������	: 		����LED�����ź�
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
void HAL_LedToggle(void)
{
	LED_RUN = ~LED_RUN;
}

/**************************************************************************************************************************************************
* @������		:  		HAL_SetCpuValid
* @��������	: 		�Ƿ�����CPU��Ч
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
void HAL_SetCpuValid(bool st)
{
	GpioDataRegs.GPADAT.bit.GPIO14 = st ? 1 : 0;
}
/**************************************************************************************************************************************************
* @������		:  		HAL_SetInnerLoop
* @��������	: 		����CPU����FPGA�����ڻ����ƣ�0ΪFPGA���ƣ�1ΪCPU����
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
void HAL_SetInnerLoop(bool st) 
{
	GpioDataRegs.GPADAT.bit.GPIO15 = st ? 1 : 0;
}
/**************************************************************************************************************************************************
* @������		:  		HAL_IsGateDriveReady
* @��������	: 		��������оƬ�Ƿ�׼���ã�TRUE��׼���ã�FALSE��δ׼����
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
bool HAL_IsGateDriveReady(void)
{
	if(GpioDataRegs.GPADAT.bit.GPIO16)
		return TRUE;
	else
		return FALSE;
}

/**************************************************************************************************************************************************
* @������		:  		HAL_IsGateDriveFault
* @��������	: 		��������оƬ�Ƿ���ϣ�TRUE�����ϣ�FALSEδ����
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
bool HAL_IsGateDriveFault(void)
{
	if(GpioDataRegs.GPADAT.bit.GPIO17)
		return FALSE;
	else
		return TRUE;
}

/**************************************************************************************************************************************************
* @������		:  		HAL_RstGateDriver
* @��������	: 		��������оƬ��λ
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
void HAL_RstGateDriver(bool st)
{
	GpioDataRegs.GPADAT.bit.GPIO18= 1;
	Delay_ms(10);
	GpioDataRegs.GPADAT.bit.GPIO18 = 0;
}
/**************************************************************************************************************************************************
* @������		:  		HAL_SetPreChargeRelayClose                                                       
* @��������	: 		����Ԥ���̵����Ƿ�պϣ��ϵ��򿪣�Ԥ�����ɺ�պϡ�
* @������		:    	����
* ��������	:   	2020-10-09
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
void HAL_SetPreChargeRelayClose(bool sts)
{
	GpioDataRegs.GPADAT.bit.GPIO7 = sts ? 0 : 1;
}

/**************************************************************************************************************************************************
* @������		:  		HAL_SetFlashHighAddr
* @��������	: 		���ò���FLASH�ĸ�λ��ַ��
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
void HAL_SetFlashHighAddr(Uint16 Haddr)
{
	GpioDataRegs.GPADAT.bit.GPIO20= (Haddr & BIT0) ? 1 : 0;
	GpioDataRegs.GPADAT.bit.GPIO21= (Haddr & BIT1) ? 1 : 0;
	GpioDataRegs.GPADAT.bit.GPIO22= (Haddr & BIT2) ? 1 : 0;
}

/**************************************************************************************************************************************************
* @������		:  		HAL_RstFlash
* @��������	: 		��λ����FLASH
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
void HAL_RstFlash(void)
{
	GpioDataRegs.GPADAT.bit.GPIO23= 0;
	Delay_ms(10);
	GpioDataRegs.GPADAT.bit.GPIO23= 1;
}

/**************************************************************************************************************************************************
* @������		:  		HAL_IsFlashReady
* @��������	: 		���ز���FLASH�Ƿ�׼���ã�
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
bool HAL_IsFlashReady(void)
{
	if(GpioDataRegs.GPADAT.bit.GPIO24)
		return TRUE;
	else
		return FALSE;
}

/**************************************************************************************************************************************************
* @������		:  		HAL_GetHallSector
* @��������	: 		���ػ����ź�
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
Uint16 HAL_GetHallSector(void)
{
	return (GpioDataRegs.GPADAT.all >> 25) & 0x07;
	
}

/**************************************************************************************************************************************************
* @������		:  		HAL_RstAd2s1210
* @��������	: 		��λAD2S1210оƬ
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
void HAL_RstAd2s1210(void)
{
	GpioDataRegs.GPBDAT.bit.GPIO32= 0;
	Delay_ms(10);
	GpioDataRegs.GPBDAT.bit.GPIO32= 1;
}

/**************************************************************************************************************************************************
* @������		:  		HAL_IsPowerGood
* @��������	: 		FPGA�ĵ�Դ�Ƿ�������TRUE������FALSE������
* @������		:    	����
* ��������	:   	2020-03-09
* �޸ļ�¼	:
*
****************************************************************************************************************************************************/
bool HAL_IsPowerGood(void)
{
	if(GpioDataRegs.GPBDAT.bit.GPIO48)
		return TRUE;
	else
		return FALSE;
}

