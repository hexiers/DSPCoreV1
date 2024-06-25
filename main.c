/*======================================================================================================
* @�ļ�����	:
* @�ļ�����	: 	����
* @������		:  	����
* ��������		:   	2020-03-03
* �޸ļ�¼		:
* 2020-5-13���޸�STS_PwrUpWait״̬��ת���ϣ�Ϊ���õ�Դ����������ת��IDLE
*
======================================================================================================*/
#include "DSP2833x_Device.h"     /*  DSP2833x Headerfile Include File*/
#include "DSP2833x_Examples.h"   /*  DSP2833x Examples Include File*/
#include "MC_Include.h"
extern void MC_TaskLoop(void);
extern void MC_InitBaseGenerate(stTimeBase* pT,stCtrlPara* pCtrl);
extern void MC_StateInit(stSysState* pState);
extern bool SMC_Init(stSmcObserverExt* pT,stCtrlPara* pCtrl,stMotorPara*pMotor,stHalPara*pHal,stSnsLessPara*pSnsLess);

void PowerUpReset(void)
{
	Uint16 Temp_WD = 0;
	if((SysCtrlRegs.WDCR & 0x80) == 0) /* �ϵ縴λ*/
	{
		EALLOW;
		Temp_WD = SysCtrlRegs.SCSR;
		SysCtrlRegs.SCSR = (Temp_WD & 0xFE);/* �������Ź���λ*/
		SysCtrlRegs.WDCR = 0x00B8;
		EDIS;
	}
	else/* ���Ź���λ*/
	{
		EALLOW;
		SysCtrlRegs.WDCR = 0x00E8;/* �رտ��Ź���λ*/
		EDIS;

	}
}

float x = 32769;
int y = 0;

void main(void)
{
	Uint16 i = 0;
	y = x;

	/*  CPU��ʼ��*/
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

  	InitSysCtrl();

	/*  ����120MHzʱ��Ƶ�ʣ���ʼ��FLASH���ʵĵȴ�������*/
	InitFlash120MHz();

 	DINT;
 	IER = 0x0000;
	IFR = 0x0000;
	
	PowerUpReset();

	/* �ر�����PIE�ж�ʹ��*/
 	InitPieCtrl();

	/* ��ʼ���ж�������*/
 	InitPieVectTable();
 	
	/* ��ʼ���ⲿ����*/
	InitXintf();

	/* ��ʼ������*/
	InitSci();
	
	/*  ��ʼ��SPI*/
	InitSpi();

	/* ��ʼ��ADC*/
	InitAdc();

	/* ��ʼ��GPIO*/
	InitGpio();

	/* ��ʼ��eCAPģ��*/
	InitECap();

	Uart_Init();

	for(i=0;i<300;i++)
	{
		Delay_us(1000);
	    if(HAL_IsFpgaReady())
	    {
	        Delay_us(20000);
	        break;
	    }
	}
    if(i >= 300)
    {
        MC_PostCodeErr(4);
    }
	/* ��ʼ���ؼ�����*/
	MC_InitCriticalData(&m1CriPara);
	MC_StateInit(&m1State);
	MC_InitMsg(&m1MsgArray);
	FirInit(&SpdFdbFir, 3);
	FirInit(&IqFdbFir, 3);
	FirInit(&IdFdbFir, 3);
	FirInit(&CoreVBusFdbFir, 4);
	FirInit(&CoreSpdFdbFir, 4);
	//FirInit(&Flux_EstSpdFdbFir, 5);			// �ٶ�ƽ��ֵ�˲�

	/* ��������ṹ��ĳ�ʼ��*/
	MemClr((Uint16*)&m1MotorCalib.PhaseRes_100uohm,(Uint16*)&m1MotorCalib.RCalibVol2_10mV);
	/* ����EEPROM���ݳ�ʼ�������ṹ��*/
	if(!MC_InitPara())
	{
		MC_PostCodeWarn(10);
	}
	m1MotorCalib.CurrLoopSampleTime_us = m1CtrlPara.PwmPeriod * 2 /60;/* ��60mHzΪPWM����ʱ��*/

	MC_HfiInit(&m1Hfi,&m1SnsLessPara);

	FluxPos_Init(&m1FluxObserver,&m1MotorPara,&m1CtrlPara);

	/* 1ms�жϵ�ʱ�䣬��Ҫ����EEPROM��������*/
	MC_InitBaseGenerate(&m1TimeBase,&m1CtrlPara);
	/* ����۲���*/
	SMC_Init(&m1SmcObserverExt, &m1CtrlPara,&m1MotorPara,&m1HalPara,&m1SnsLessPara);

	/* ���ò����ṹ���ʼ��FPGA���IP*/
	if(!MC_InitMCCore())
	{
		MC_PostCodeErr(11);
	}

	EALLOW;	
	PieVectTable.ADCINT = &ADCINT_ISR; //INT1.6
	PieVectTable.SCIRXINTA= &SCIRXINTA_ISR;//INT9.1
	PieVectTable.EPWM1_TZINT = &EPWM1_TZINT_ISR;// INT2.1
	EDIS;
	
	//����TLV5638
	//SetTestData(0x9002 + 0x4000);

	//TZ�ж�������
	IER |= M_INT2;
	
	//ADC�ж��ڸ���
	IER |= M_INT1;
	
	//�����ڽ����ж��ڸ���
	IER |= M_INT9;
	
	// ��ADC PIE���ж�1.6
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;	
	
	// ��TZ PIE���ж�2.1
	PieCtrlRegs.PIEIER2.bit.INTx1= 1;
	
	//�����ڽ��� PIE�����ж�9.1
	PieCtrlRegs.PIEIER9.bit.INTx1= 1;

	ECap4Regs.ECCTL2.bit.TSCTRSTOP = 1;//��������ʱ��������
	ECap4Regs.ECCTL1.bit.CAPLDEN = 1;//ʹ�ܲ���
	EQep1Regs.QEPCTL.bit.QPEN = 1;//������ʹ��
	
	ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;//��������ʱ��������
	ECap1Regs.ECCTL1.bit.CAPLDEN = 1;//ʹ�ܲ���

	// ����ȫ���ж�
	EINT;   
	// ����ȫ��ʵʱ�ж�
	ERTM;	

	//�������Ź���������Ź���־λ17.44ms
	//SysCtrlRegs.WDCR= 0x00AB;
	for(;;)
	{
		/* �ǽ�������*/
		MC_TaskLoop();
	}

}



//===========================================================================
// No more.
//===========================================================================
