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
	AdcRegs.ADCTRL1.bit.RESET = 1;	//ADC模块软件复位
	for(i=0;i<100;i++)	NOP;	
	AdcRegs.ADCTRL1.bit.SUSMOD = 3;	//仿真挂起时，排序器和其他逻辑停止工作
	AdcRegs.ADCTRL1.bit.ACQ_PS = 0;	//采样窗的宽度,SH=(ACQ_PS+1)ADCCLK,最多16个。
	AdcRegs.ADCTRL1.bit.CPS = 0;	//ADC内核时钟定标不分频
	AdcRegs.ADCTRL1.bit.CONT_RUN= 0;	//0表示启动/停止模式，写1表示连续转换模式,写0每次由EV触发采集
	AdcRegs.ADCTRL1.bit.SEQ_OVRD = 0;	//28335新增，Max_Con转换完成后，转换结果从Result0开始重新存储
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;	//级联排序模式，其实无所谓,同步采样，8位就够
	//================================================================================
	//ADCREFSEL 28335新增	
	AdcRegs.ADCREFSEL.bit.REF_SEL = 2;//选择外部1.5V基准电源28335
	//================================================================================
	//ADCTRL3	
	//AdcRegs.ADCTRL3.bit.EXTREF = 2;//选择外部参考源1.5V 2812
	for(i=0;i<100;i++)	NOP;
	AdcRegs.ADCTRL3.bit.ADCBGRFDN=3;//0表示带限和参考电源关，3表示带限和参考电源开
	for(i=0;i<10000;i++)	NOP;
	AdcRegs.ADCTRL3.bit.ADCPWDN=1;//ADC内核的模拟电路的电源开
	for(i=0;i<5000;i++)	NOP;
	
	Delay_ms(50);
	
	AdcRegs.ADCTRL3.bit.ADCCLKPS=3;//内核时钟定标,ADCCLK=HSPCLK/((ADCCLKPS*2)*(CPS+1))
	AdcRegs.ADCTRL3.bit.SMODE_SEL=1;//0为顺序采样模式，1为同时采样模式
	//================================================================================
	//MAX_CONV		
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1=7;//一次自动转换时的最大转换数，8对16路
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
	AdcRegs.ADCST.bit.INT_SEQ1_CLR=1;//写1清SEQ1中断标志位，写0无影响
	AdcRegs.ADCST.bit.INT_SEQ2_CLR=1;//写1清SEQ2中断标志位，写0无影响
	//================================================================================
	//ADCTRL2
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1=1;	//PWMA触发SEQ1
	AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1=0; //不允许外部启动排序器	1
	AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ=0; //不允许PWMB启动排序器1	
	AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ2=0; //不允许PWMB启动排序器2
	
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1=1;//SEQ1中断开启
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 0;//每个序列1转换结束中断
	//AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 1;//每隔序列1转换结束中断
	AdcRegs.ADCTRL2.bit.SOC_SEQ1=0;
	AdcRegs.ADCTRL2.bit.RST_SEQ1=1;//将排序器复位
	//AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1=0;	
	///本程序未使用SEQ2
	AdcRegs.ADCTRL2.bit.SOC_SEQ2=0;
	AdcRegs.ADCTRL2.bit.RST_SEQ2=0;
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ2=0;
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ2=0;
	ADC_cal();
	//偏移微调寄存器，根据实际偏置调整
	//AdcRegs.ADCOFFTRIM.all=0;	//0x0085约0.1V
}

//===========================================================================
// End of file.
//===========================================================================
