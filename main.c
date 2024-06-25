/*======================================================================================================
* @文件名称	:
* @文件描述	: 	定义
* @创建人		:  	张宇
* 创建日期		:   	2020-03-03
* 修改记录		:
* 2020-5-13：修改STS_PwrUpWait状态不转故障，为了让电源正常后自行转入IDLE
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
	if((SysCtrlRegs.WDCR & 0x80) == 0) /* 上电复位*/
	{
		EALLOW;
		Temp_WD = SysCtrlRegs.SCSR;
		SysCtrlRegs.SCSR = (Temp_WD & 0xFE);/* 启动看门狗复位*/
		SysCtrlRegs.WDCR = 0x00B8;
		EDIS;
	}
	else/* 看门狗复位*/
	{
		EALLOW;
		SysCtrlRegs.WDCR = 0x00E8;/* 关闭看门狗复位*/
		EDIS;

	}
}

float x = 32769;
int y = 0;

void main(void)
{
	Uint16 i = 0;
	y = x;

	/*  CPU初始化*/
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

  	InitSysCtrl();

	/*  按照120MHz时钟频率，初始化FLASH访问的等待周期数*/
	InitFlash120MHz();

 	DINT;
 	IER = 0x0000;
	IFR = 0x0000;
	
	PowerUpReset();

	/* 关闭所有PIE中断使能*/
 	InitPieCtrl();

	/* 初始化中断向量表*/
 	InitPieVectTable();
 	
	/* 初始化外部总线*/
	InitXintf();

	/* 初始化串口*/
	InitSci();
	
	/*  初始化SPI*/
	InitSpi();

	/* 初始化ADC*/
	InitAdc();

	/* 初始化GPIO*/
	InitGpio();

	/* 初始化eCAP模块*/
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
	/* 初始化关键数据*/
	MC_InitCriticalData(&m1CriPara);
	MC_StateInit(&m1State);
	MC_InitMsg(&m1MsgArray);
	FirInit(&SpdFdbFir, 3);
	FirInit(&IqFdbFir, 3);
	FirInit(&IdFdbFir, 3);
	FirInit(&CoreVBusFdbFir, 4);
	FirInit(&CoreSpdFdbFir, 4);
	//FirInit(&Flux_EstSpdFdbFir, 5);			// 速度平均值滤波

	/* 电机整定结构体的初始化*/
	MemClr((Uint16*)&m1MotorCalib.PhaseRes_100uohm,(Uint16*)&m1MotorCalib.RCalibVol2_10mV);
	/* 调用EEPROM数据初始化参数结构体*/
	if(!MC_InitPara())
	{
		MC_PostCodeWarn(10);
	}
	m1MotorCalib.CurrLoopSampleTime_us = m1CtrlPara.PwmPeriod * 2 /60;/* 以60mHz为PWM发波时钟*/

	MC_HfiInit(&m1Hfi,&m1SnsLessPara);

	FluxPos_Init(&m1FluxObserver,&m1MotorPara,&m1CtrlPara);

	/* 1ms中断的时间，需要根据EEPROM参数设置*/
	MC_InitBaseGenerate(&m1TimeBase,&m1CtrlPara);
	/* 软件观测器*/
	SMC_Init(&m1SmcObserverExt, &m1CtrlPara,&m1MotorPara,&m1HalPara,&m1SnsLessPara);

	/* 调用参数结构体初始化FPGA相关IP*/
	if(!MC_InitMCCore())
	{
		MC_PostCodeErr(11);
	}

	EALLOW;	
	PieVectTable.ADCINT = &ADCINT_ISR; //INT1.6
	PieVectTable.SCIRXINTA= &SCIRXINTA_ISR;//INT9.1
	PieVectTable.EPWM1_TZINT = &EPWM1_TZINT_ISR;// INT2.1
	EDIS;
	
	//配置TLV5638
	//SetTestData(0x9002 + 0x4000);

	//TZ中断在这组
	IER |= M_INT2;
	
	//ADC中断在该组
	IER |= M_INT1;
	
	//开串口接收中断在该组
	IER |= M_INT9;
	
	// 开ADC PIE级中断1.6
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;	
	
	// 开TZ PIE级中断2.1
	PieCtrlRegs.PIEIER2.bit.INTx1= 1;
	
	//开串口接收 PIE级别中断9.1
	PieCtrlRegs.PIEIER9.bit.INTx1= 1;

	ECap4Regs.ECCTL2.bit.TSCTRSTOP = 1;//启动捕获时基计数器
	ECap4Regs.ECCTL1.bit.CAPLDEN = 1;//使能捕获
	EQep1Regs.QEPCTL.bit.QPEN = 1;//计数器使能
	
	ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;//启动捕获时基计数器
	ECap1Regs.ECCTL1.bit.CAPLDEN = 1;//使能捕获

	// 开放全局中断
	EINT;   
	// 开放全局实时中断
	ERTM;	

	//开启看门狗，清除看门狗标志位17.44ms
	//SysCtrlRegs.WDCR= 0x00AB;
	for(;;)
	{
		/* 非紧急任务*/
		MC_TaskLoop();
	}

}



//===========================================================================
// No more.
//===========================================================================
