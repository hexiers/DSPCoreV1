/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����
* @������		:  	����                                                                              
* ��������		:   	2020-06-01                                                               
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#include"MCSoft_Hfi.h"
ST_HFI m1Hfi;
ST_STARTNEW m1eStart;
/**************************************************************************************************************************************************
* @������		:  		MC_HfiInit                                                       
* @��������	: 		��Ƶע��ṹ��ĳ�ʼ��
* @������		:    	����
* ��������	:   	2020-06-01
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/
void MC_HfiInit(ST_HFI* pT,stSnsLessPara* pSns)
{
	double SampleFreq_Hz = 0;
	_iq RadPerCurrCycle = 0;
	/* ����PWMƵ��*/
	SampleFreq_Hz = PWMCLKFREQ/(2*m1CtrlPara.PwmPeriod);

    pT->InjectCnt = 0;
    pT->InjectFreg  = pSns->HfiFreqHz;
    pT->InjectVol   = pSns->HfiVol;
    pT->BoundHighFreqPassHz  = pSns->BoundHighFreqPassHz;    /* �ݶ�*/
    pT->BoundLowFreqPassHz  = pSns->BoundLowFreqPassHz;    /* �ݶ�*/
    pT->LowFreqPassHz   = pSns->LowFreqPassHz;    /* �ݶ�*/
    RadPerCurrCycle = _IQdiv(_IQmpy(_IQ(PI*2),_IQ(pT->InjectFreg)),_IQ(SampleFreq_Hz));
    pT->RadPerCurrCycleQ12 =  _IQint((INT32S)RadPerCurrCycle * (1L<<12));
    pT->InjectCntMax = SampleFreq_Hz/pT->InjectFreg;
    pT->BoundHighFreqPassCoefQ14 = (INT32S)(16384*PI*2*(double)pT->BoundHighFreqPassHz/SampleFreq_Hz);
    pT->BoundLowFreqPassCoefQ14 = (INT32S)(16384*PI*2*(double)pT->BoundLowFreqPassHz/SampleFreq_Hz);
    pT->LowFreqPassCoefQ14 = (INT32S)(16384*PI*2*(double)pT->LowFreqPassHz/SampleFreq_Hz);
    pT->IqHpass = 0;
    pT->IqXsinwt = 0;
    pT->IqXsinwtLowPassX16 = 0;
    pT->HfiPos = 0;              /* ��Ƶע��λ��*/
    //pT->HfiPosCoefQ8 = 256;       /* ������smcλ��ƽ������*/
    pT->HfiSpdQ16 = 0;
    pT->KpQ16 = pSns->PllKpQ12 * 16L;// 25180
    pT->KiQ16 = pSns->PllKiQ16;
    pT->IntergTermQ16 = 0;
    //pT->Pos180 = 0;
}
/**************************************************************************************************************************************************
* @������		:  		MC_HfiCalc                                                       
* @��������	: 		��Ƶע�����㣬���յ����ж�Ƶ�ʵ��ã�����Ӧע��ֵUdCmd_V�Ŀ�����
* @������		:    	����
* ��������	:   	2020-06-03
* �޸ļ�¼	:
*                         
****************************************************************************************************************************************************/

INT16S MC_HfiCalc(ST_HFI* pT,INT16S IqX10mA)
{
    _iq iqSinwt = 0;
    _iq iqCoswt = 0;
    INT16S UdInject_V = 0;
    //INT32S Tmp = 0;
    INT32S TmpQ = 0;
    INT32S TmpD = 0;
    static INT32S staTmp0 = 0;
    static INT32S staTmp1 = 0;
    static INT32S staTmp2 = 0;
    static INT32S staTmp3 = 0;
    if(++pT->InjectCnt >= pT->InjectCntMax)
    {
        pT->InjectCnt = 0;
    }
    /* ע���ź���λ*/
    iqCoswt = _IQcos(pT->InjectCnt * (long)pT->RadPerCurrCycleQ12 << 4);
    /* ��������ź���λ*/
    iqSinwt = _IQsin((pT->InjectCnt * (long)pT->RadPerCurrCycleQ12 << 4)  + _IQ((PI*0)/180));// ����ź���λУ��0��
    pT->sinwt = iqSinwt >> 5;
    /* ����ע���ź�*/
    UdInject_V = (INT32S)pT->InjectVol*iqCoswt>> GLOBAL_Q;
    /* ע���źű���*/
    UdInject_V = LIMITMAXMIN(UdInject_V, 2000, -2000);

    /* q������ͨ�ĵ�ͨ�˲�����*/
    staTmp0 = (((INT64S)IqX10mA << GLOBAL_Q) * pT->BoundLowFreqPassCoefQ14 + (INT64S)staTmp1 * (16384 - pT->BoundLowFreqPassCoefQ14))>>14;
    /* q������ͨ�ĸ�ͨ�˲�,�ȵ�ͨ�˲�,���������ȥ*/
    staTmp1 = ((INT64S)staTmp0 * pT->BoundHighFreqPassCoefQ14 + (INT64S)staTmp1 * (16384 - pT->BoundHighFreqPassCoefQ14))>>14;
    pT->IqHpass = (staTmp0 - staTmp1) >> GLOBAL_Q;

    /* ����*/
    TmpQ = (INT32S)pT->IqHpass * (iqSinwt);

    /* ���ƺ��źŵ�ͨ�˲�*/
    staTmp2= ((INT64S)TmpQ * pT->LowFreqPassCoefQ14 + (INT64S)staTmp2 * (16384 - pT->LowFreqPassCoefQ14))>>14;
    //staTmp3= ((INT64S)staTmp2 * pT->LowFreqPassCoefQ14 + (INT64S)staTmp3 * (16384 - pT->LowFreqPassCoefQ14))>>14;
    staTmp3 = staTmp2;
    
    pT->IqXsinwt = TmpQ >> GLOBAL_Q;
    pT->IqXsinwtLowPassX16 = staTmp3 >> (GLOBAL_Q-4);
    
    //pT->IqXsinwtLowPassX16 = (-staTmp3) >> (GLOBAL_Q-4);
   
	//���������
	pT->IntergTermQ16 += pT->KiQ16 * (INT32S)pT->IqXsinwtLowPassX16;
	pT->IntergTermQ16 = LIMITMAXMIN(pT->IntergTermQ16,(400L<<16),(1L<<16));/* 1L<<16 ���Է�ֹ��������*/
    
    // PI���ڳ�����ٶ�
    pT->HfiSpdQ16 = LIMITMAXMIN((pT->KpQ16 * (INT32S)pT->IqXsinwtLowPassX16  + pT->IntergTermQ16),(400L<<16),(1L<<16));/* 1L<<16 ���Է�ֹ��������*/

    //�ٶ��ۼӳ�λ��
    pT->HfiPosQ16+= pT->HfiSpdQ16; 
    pT->HfiPos = (INT16S)(pT->HfiPosQ16 >> 16);

    /* ע�����*/
    return UdInject_V;
    
    /* ����*/
    //pT->Test1 = (INT16S)(pT->HfiPosQ16 >> 16) - m1SmcObserverExt.SmcElecPos;
    //pT->Test2 = iqSinwt >> 1;
    /* ����*/
    //TmpD =  (INT32S)(m1CurrLoopCtler.IdFdb * (Uint32)m1HalPara.BitTo10mACoef) >>12;
    //pT->IdXsinwt = (INT32S)TmpD * iqSinwt>>GLOBAL_Q;


   
}

INT16S MC_eStartCalc(ST_STARTNEW* pT)
{
    INT16S Tmp = 0;
    INT16S Tmp2 = 0;
    //�ٶ��ۼ�
    if(pT->eStartSpdQ8 < pT->eStartEndSpdQ8)
    {
        pT->eStartSpdQ8 += 4;
    }
    //λ���ۼ�
    pT->eStartPosQ8 = ((INT32S)pT->eStartPos << 8) + pT->eStartSpdQ8;
    pT->eStartPosQ8 = pT->eStartPosQ8 & 0x00FFFFFF;
    Tmp = pT->eStartPosQ8 >> 8;

    // eStartSpdQ8 = 128Q8ʱ����ȫ�ɹ۲���λ��ȷ��
    // eStartSpdQ8 = 64Q8ʱ����ȫ�ɿ���λ��ȷ��
    pT->ePosCoefQ8 = LIMITMAXMIN(((128L<<8) - pT->eStartSpdQ8)>>6,256,0);
    //λ���ں�
    Tmp2 = m1SmcObserverExt.SmcElecPos - Tmp;/* ��16λ�ֳ�����з������*/
    Tmp2 = ((INT32S)Tmp2 * (256L - pT->ePosCoefQ8))>>8;/* ����ӿ���λ�õ�����λ�õ�������,��Ϊ16λ�ֳ�*/
    // ��������λ��
    pT->eStartPos = Tmp + Tmp2;
    return 1;
    
}
void MC_eStartInit(ST_STARTNEW* pT,INT32S EndSpdQ8)
{
    pT->eStartSpdQ8 = 0;              
    pT->eStartEndSpdQ8  = EndSpdQ8;
    pT->eStartPos = 0;
    pT->ePosCoefQ8 = 256;
    pT->eStartPosQ8 = 0;
    
}
 
