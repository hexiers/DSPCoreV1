// TI File $Revision: /main/4 $
// Checkin $Date: July 30, 2007   14:15:53 $
//###########################################################################
//
// FILE:	DSP2833x_Adc.c
//
// TITLE:	DSP2833x ADC Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define ADC_usDELAY  5000L

//---------------------------------------------------------------------------
// InitAdc:
//---------------------------------------------------------------------------
// This function initializes ADC to a known state.
//
void InitAdc(void)
{
	unsigned int i;	
	//================================================================================
	//ADCTRL1
	AdcRegs.ADCTRL1.bit.RESET = 1;	//ADCģ�������λ
	for(i=0;i<100;i++)	NOP;	
	AdcRegs.ADCTRL1.bit.SUSMOD = 3;	//�������ʱ���������������߼�ֹͣ����
	AdcRegs.ADCTRL1.bit.ACQ_PS = 0;	//�������Ŀ��,SH=(ACQ_PS+1)ADCCLK,���16����
	AdcRegs.ADCTRL1.bit.CPS = 0;	//ADC�ں�ʱ�Ӷ��겻��Ƶ
	AdcRegs.ADCTRL1.bit.CONT_RUN= 0;	//0��ʾ����/ֹͣģʽ��д1��ʾ����ת��ģʽ,д0ÿ����EV�����ɼ�
	AdcRegs.ADCTRL1.bit.SEQ_OVRD = 0;	//28335������Max_Conת����ɺ�ת�������Result0��ʼ���´洢
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;	//��������ģʽ����ʵ����ν,ͬ��������8λ�͹�
	//================================================================================
	//ADCREFSEL 28335����	
	AdcRegs.ADCREFSEL.bit.REF_SEL = 2;//ѡ���ⲿ1.5V��׼��Դ28335
	//================================================================================
	//ADCTRL3	
	//AdcRegs.ADCTRL3.bit.EXTREF = 2;//ѡ���ⲿ�ο�Դ1.5V 2812
	for(i=0;i<100;i++)	NOP;
	AdcRegs.ADCTRL3.bit.ADCBGRFDN=3;//0��ʾ���޺Ͳο���Դ�أ�3��ʾ���޺Ͳο���Դ��
	for(i=0;i<10000;i++)	NOP;
	AdcRegs.ADCTRL3.bit.ADCPWDN=1;//ADC�ں˵�ģ���·�ĵ�Դ��
	for(i=0;i<5000;i++)	NOP;
	
	Delay_ms(50);
	
	AdcRegs.ADCTRL3.bit.ADCCLKPS=3;//�ں�ʱ�Ӷ���,ADCCLK=HSPCLK/((ADCCLKPS*2)*(CPS+1))
	AdcRegs.ADCTRL3.bit.SMODE_SEL=1;//0Ϊ˳�����ģʽ��1Ϊͬʱ����ģʽ
	//================================================================================
	//MAX_CONV		
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1=7;//һ���Զ�ת��ʱ�����ת������8��16·
	AdcRegs.ADCCHSELSEQ1.bit.CONV00=0;
	AdcRegs.ADCCHSELSEQ1.bit.CONV01=1;
	AdcRegs.ADCCHSELSEQ1.bit.CONV02=2;
	AdcRegs.ADCCHSELSEQ1.bit.CONV03=3;
	AdcRegs.ADCCHSELSEQ2.bit.CONV04=4;
	AdcRegs.ADCCHSELSEQ2.bit.CONV05=5;
	AdcRegs.ADCCHSELSEQ2.bit.CONV06=6;
	AdcRegs.ADCCHSELSEQ2.bit.CONV07=7;
	//================================================================================
	//ADC_ST_FLAG
	AdcRegs.ADCST.bit.INT_SEQ1_CLR=1;//д1��SEQ1�жϱ�־λ��д0��Ӱ��
	AdcRegs.ADCST.bit.INT_SEQ2_CLR=1;//д1��SEQ2�жϱ�־λ��д0��Ӱ��
	//================================================================================
	//ADCTRL2
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1=1;	//PWMA����SEQ1
	AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1=0; //�������ⲿ����������	1
	AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ=0; //������PWMB����������1	
	AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ2=0; //������PWMB����������2
	
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1=1;//SEQ1�жϿ���
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 0;//ÿ������1ת�������ж�
	//AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 1;//ÿ������1ת�������ж�
	AdcRegs.ADCTRL2.bit.SOC_SEQ1=0;
	AdcRegs.ADCTRL2.bit.RST_SEQ1=1;//����������λ
	//AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1=0;	
	///������δʹ��SEQ2
	AdcRegs.ADCTRL2.bit.SOC_SEQ2=0;
	AdcRegs.ADCTRL2.bit.RST_SEQ2=0;
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ2=0;
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ2=0;
	ADC_cal();
	//ƫ��΢���Ĵ���������ʵ��ƫ�õ���
	//AdcRegs.ADCOFFTRIM.all=0;	//0x0085Լ0.1V
}

//===========================================================================
// End of file.
//===========================================================================
