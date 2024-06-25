/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����
* @������		:  	����                                                                              
* ��������		:   	2020-03-07                                                                 
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#include"MCSoft_StartUp.h"
stStartup m1StartUp;
stIir StartupUsIIR;
/**************************************************************************************************************************************************
* @������		:  		IirInit                                                        
* @��������	: 		��ʼ�������˲�������ֵϵ��FilterCoef/128
* @������		:    		����                                                                            
* ��������		:   		2020-03-07                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
void IirInit(stpIir stp,INT32S FilterCoef)
{
	stp->OldValue = 0;
	stp->NewValueCoef = LIMITMAXMIN(FilterCoef, 128, 0);
	stp->OldVaueCoef = 128-stp->NewValueCoef;
}

/**************************************************************************************************************************************************
* @������		:  		IirCalc                                                        
* @��������	: 		�����˲������㣬��������ݷ�ΧӦΪ-32768,32767
* @������		:    		����                                                                            
* ��������		:   		2020-03-07                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
INT16S IirCalc(stpIir stp,INT16S DataIn)
{
	INT32S Tmp = 0;
	Tmp = DataIn * 128L;
	Tmp *= stp->NewValueCoef;
	Tmp += stp->OldVaueCoef * stp->OldValue;
	Tmp = (Tmp >>7);/* ����˲�ϵ��128��һ��Ϊ1*/
	stp->OldValue = Tmp;
	return(INT16S)(Tmp >> 7);
}

/**************************************************************************************************************************************************
* @������		:  		MC_InitStartUp                                                        
* @��������	: 		��ʼ��stStartup����
* @������		:    		����                                                                            
* ��������		:   		2020-03-07                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool MC_InitStartUp(stCtrlPara* pCtrl,stSnsLessPara* pSns,stMotorPara*pMotor,stStartup* pStartup)
{
	Uint32 SampleFreq_Hz = 0;
	pStartup->StartUpCnt = 0;
	pStartup->StartupUs = 0;
	pStartup->StartupUs1 = 0;
	pStartup->StartupUs2  = 0;
	pStartup->AlignCallTimes = 0;
	/* msΪ��λ������ʱ��*/
	pStartup->StartupTimeInms = LIMITMAXMIN(pSns->StartUpTime_ms, 32767, 1000);
	/* ������Ƶ��*/
	SampleFreq_Hz = PWMCLKFREQ/(2*pCtrl->PwmPeriod);
	/* �����ٶ��ۼӵ���Ƶ�Σ�ÿ��64���ж����ڣ�ִ��һ���ٶ��ۼ�*/
	pStartup->StartupAddTimes = (pStartup->StartupTimeInms*SampleFreq_Hz)/(1000L * 64L);
	/* Ԥ��λͨ�����ڣ��Ե���������Ϊ��λ*/
	pStartup->AlignSetTimes = pSns->AlignTime_ms*SampleFreq_Hz/(1000);
	/* �����ٶ�*/
	pStartup->StartupRpm1 = 1;
	/* ���������ٶ�*/
	pStartup->StartupRpm2 = LIMITMAXMIN(pSns->StartUpSpd, 32767, 100);
	/* ����λ���ۼӵ��ٶȵ�����ֵ*/
	pStartup->StartupSpd1Used = ( (Uint32)pStartup->StartupRpm1*pMotor->MotorPoles*(ADDNUMGAIN/SampleFreq_Hz))/60;
	//����λ���ۼӵ��ٶȵ�����ֵ
	pStartup->StartupSpd2Used = ( (Uint32)pStartup->StartupRpm2*pMotor->MotorPoles*(ADDNUMGAIN/SampleFreq_Hz))/60;
	//�ٶȵ���������
	pStartup->StartupSpdDelta = pStartup->StartupSpd2Used - pStartup->StartupSpd1Used;
	
	pStartup->HalfStartSpdUsed1P256 = pStartup->StartupSpdDelta >> 9;
	if(pStartup->HalfStartSpdUsed1P256 == 0)
	{
        pStartup->HalfStartSpdUsed1P256 = 1;
	}
	pStartup->PosCoef = 0;
	//�ٶȵ��������ۼ���������ֹ�ٶ��ۼ�ֵ̫С
	if(pStartup->StartupSpdDelta <= pStartup->StartupAddTimes)
	{
	    pStartup->StartupAccDelta = 1;
	}
	else
	{
	    pStartup->StartupAccDelta = pStartup->StartupSpdDelta/pStartup->StartupAddTimes;
	}
	pStartup->PosAdd = 0;
	pStartup->StartUpSuccess = FALSE;
	
	IirInit(&StartupUsIIR,1);	
	return TRUE;

}
/**************************************************************************************************************************************************
* @������		:  		MC_DoStartUp                                                        
* @��������	: 		1. ����������ת��λ�����У�pStartup->PosAdd[31:16],
						2. ���ݷ�Χ[0 65535]��Ӧ����[0 2PI]
						3. �������ؿ������ɵ�λ��
						4. ������ת��ɺ�StartUpSuccess�ź�ָʾ�Ƿ������ɹ�
						5. ��ת��ɺ�SubState = 3
* @������		:    		����                                                                            
* ��������		:   		2020-03-07                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
Uint16 MC_DoStartUp(stStartup* pStartup,stSysState* pState)
{
	INT32S TmpUd = 0;
	INT32S TmpUq = 0;
	//INT32S Tmp = 0;
	//INT32S ErrQ8 = 0;
	Uint16 OpenLoopPos = 0;
    //INT16S s16Tmp = 0;
	/* Ԥ��λ*/
	 if(pState->SubState == 1)
	{
		if(pStartup->AlignCallTimes++ > pStartup->AlignSetTimes)
		{
			pState->SubState = 2;
		}		
	}
	/*  I/F����*/
	 if((pState->SubState == 2)||(pState->SubState == 3))
	{
		TmpUd = m1CurrLoopCtler.UdPILim_V;
		TmpUq = m1CurrLoopCtler.UqPILim_V;

		/* λ���ۼ�*/
		pStartup->PosAdd += pStartup->StartupSpd1Used;
		pStartup->StartUpCnt ++;
		/* �ۼ����*/
		if(pStartup->PosAdd > ADDNUMGAIN)
		{
			pStartup->PosAdd -= ADDNUMGAIN;
		}
        
		/* �ٶ��ۼ�*/
		if(pStartup->StartupSpd1Used < pStartup->StartupSpd2Used)
		{
			/* ÿ64����������ִ��һ���ٶ��ۼ�*/
			if((pStartup->StartUpCnt & 63) == 63)
			{
				pStartup->StartupUs = IirCalc(&StartupUsIIR,(TmpUd*TmpUd+TmpUq*TmpUq)>>12);
				pStartup->StartupSpd1Used += pStartup->StartupAccDelta;
			}
			/* 1/4����ת��ʱ�򣬼�¼һ�κϳ������ѹ*/
			if((pStartup->StartupUs1 == 0)&&(pStartup->StartupSpd1Used > (pStartup->StartupSpd2Used >> 2)))
			{
				pStartup->StartupUs1 = pStartup->StartupUs > 0?pStartup->StartupUs:1;
			}
		}
		else/* ��ת����*/
		{			
			pState->SubState = 3;
			/* ��ת����ʱ�򣬼�¼һ�κϳ������ѹ*/
			if(pStartup->StartupUs2 == 0)
			{
				pStartup->StartupUs2 = pStartup->StartupUs > 0?pStartup->StartupUs:1;
			}
			/* ��ת����ʱ�ж��Ƿ������ɹ�*/
			if(pStartup->StartupUs2 > 2*pStartup->StartupUs1)
			{
				pStartup->StartUpSuccess = TRUE;
			}
			else
			{
				pStartup->StartUpSuccess = FALSE;
			}
		}
		#if 0 /* λ�õ�ƽ���л�*/
		// PosCoef ����ת�ٵ���������0���256
		Tmp = (pStartup->StartupSpd2Used - pStartup->StartupSpd1Used)/pStartup->HalfStartSpdUsed1P256;
		pStartup->PosCoef = LIMITMAXMIN(256-Tmp, 256, 0);
		// �������,����Ȩ����
		s16Tmp = pStartup->PosAdd >> 8;
		ErrQ8 = m1SmcObserver.SmcElecPos - s16Tmp;
		ErrQ8 = ErrQ8 * pStartup->PosCoef;
		pStartup->PosAdd += ErrQ8;
		#endif
		
		
	}
	OpenLoopPos = pStartup->PosAdd >> 8;
	return OpenLoopPos;
}
/**************************************************************************************************************************************************
* @������		:  		MC_DoStartUpCalib                                                        
* @��������	: 			1. ����������ת��λ�����У�pStartup->PosAdd[31:16],
						2. ���ݷ�Χ[0 65535]��Ӧ����[0 2PI]
						3. �������ؿ������ɵ�λ��
						4. ������ת��ɺ�StartUpSuccess�ź�ָʾ�Ƿ������ɹ�
						5. ��ת��ɺ�SubState = 16
* @������		:    		����                                                                            
* ��������		:   		2020-03-07                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
Uint16 MC_DoStartUpCalib(stStartup* pStartup,stSysState* pState)
{
	INT32S TmpUd = 0;
	INT32S TmpUq = 0;
	Uint16 OpenLoopPos = 0;
    //INT16S s16Tmp = 0;
	/* Ԥ��λ*/
	 if(pState->SubState == 14)
	{
		if(pStartup->AlignCallTimes++ > pStartup->AlignSetTimes)
		{
			pState->SubState = 15;
		}		
	}
	/*  I/F����*/
	 if((pState->SubState == 15)||(pState->SubState == 16))
	{
		TmpUd = m1CurrLoopCtler.UdPILim_V;
		TmpUq = m1CurrLoopCtler.UqPILim_V;

		/* λ���ۼ�*/
		pStartup->PosAdd += pStartup->StartupSpd1Used;
		pStartup->StartUpCnt ++;
		/* �ۼ����*/
		if(pStartup->PosAdd > ADDNUMGAIN)
		{
			pStartup->PosAdd -= ADDNUMGAIN;
		}
        
		/* �ٶ��ۼ�*/
		if(pStartup->StartupSpd1Used < pStartup->StartupSpd2Used)
		{
			/* ÿ64����������ִ��һ���ٶ��ۼ�*/
			if((pStartup->StartUpCnt & 63) == 63)
			{
				pStartup->StartupUs = IirCalc(&StartupUsIIR,(TmpUd*TmpUd+TmpUq*TmpUq)>>12);
				pStartup->StartupSpd1Used += pStartup->StartupAccDelta;
			}
			/* 1/4����ת��ʱ�򣬼�¼һ�κϳ������ѹ*/
			if((pStartup->StartupUs1 == 0)&&(pStartup->StartupSpd1Used > (pStartup->StartupSpd2Used >> 2)))
			{
				pStartup->StartupUs1 = pStartup->StartupUs > 0?pStartup->StartupUs:1;
			}
		}
		else/* ��ת����*/
		{			
			pState->SubState = 16;
			/* ��ת����ʱ�򣬼�¼һ�κϳ������ѹ*/
			if(pStartup->StartupUs2 == 0)
			{
				pStartup->StartupUs2 = pStartup->StartupUs > 0?pStartup->StartupUs:1;
			}
			/* ��ת����ʱ�ж��Ƿ������ɹ�*/
			if(pStartup->StartupUs2 > 2*pStartup->StartupUs1)
			{
				pStartup->StartUpSuccess = TRUE;
			}
			else
			{
				pStartup->StartUpSuccess = FALSE;
			}
		}
	}
	OpenLoopPos = pStartup->PosAdd >> 8;
	return OpenLoopPos;
}




