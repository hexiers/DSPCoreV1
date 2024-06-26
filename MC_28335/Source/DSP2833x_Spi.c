// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:46:44 $
//###########################################################################
//
// FILE:   DSP2833x_Spi.c
//
// TITLE:  DSP2833x SPI Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x/DSP2823x C/C++ Header Files V1.31 $
// $Release Date: August 4, 2009 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#define  Spi_Ready_Def()  (SpiaRegs.SPISTS.bit.INT_FLAG)

//---------------------------------------------------------------------------
// InitSPI: 
//---------------------------------------------------------------------------
// This function initializes the SPI(s) to a known state.
//
void InitSpi(void)
{
	//=======================================
    	//配置控制寄存器设置
	//复位SPI，退出之前需要释放
	SpiaRegs.SPICCR.bit.SPISWRESET = 0;
	//SPICLK空闲态为高
	SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;//SPICLK空闲态为高
	//数据长16位
	SpiaRegs.SPICCR.bit.SPICHAR = 15;
	//关闭SPI自测试模式
	SpiaRegs.SPICCR.bit.SPILBK = 0;
	//=======================================
	//操作控制寄存器设置
	//禁止接收一处标志位中断
	SpiaRegs.SPICTL.bit.OVERRUNINTENA = 0;
	//时钟信号不延时半个周期，上升沿锁数
	//SpiaRegs.SPICTL.bit.CLK_PHASE = 0;//A1333手册规定其工作在3模式，即该位为1，但实际工作时位置信息发生跳变
	SpiaRegs.SPICTL.bit.CLK_PHASE = 1;
	//SPI配置成主模式
	SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;
	//使能发送
	SpiaRegs.SPICTL.bit.TALK = 1;
	//禁止发送/接收中断
	SpiaRegs.SPICTL.bit.SPIINTENA = 0;
	//=======================================
	//波特率的设置
	//波特率 = LSPCLK/（SPIBRR+1）= 20/4 = 5MHz
	//波特率 = LSPCLK/（SPIBRR+1）= 20/6 = 
	SpiaRegs.SPIBRR = 0x06;
	//=======================================
	//退出初始状态
	SpiaRegs.SPICCR.bit.SPISWRESET = 1;
 
}

Uint16 SpiTrans(Uint16 Data)
{
	Uint16 Temp = 0;
	ANGLE_CS = 0;	
	//NOP7;
	//发送数据
	SpiaRegs.SPITXBUF = Data;
	//NOP7;
	//等待SPI空闲
	while(Spi_Ready_Def() == 0);
	ANGLE_CS = 1;
	//读数据
	Temp = (SpiaRegs.SPIRXBUF);// & (0x00FF));
	//NOP7;
	return Temp;
}
Uint16 SetTestData(Uint16 Data)
{
	Uint16 Temp = 0;
	DA_CS = 0;	
	NOP7;
	//发送数据
	SpiaRegs.SPITXBUF = Data;
	//NOP7;
	//等待SPI空闲
	while(Spi_Ready_Def() == 0);
	DA_CS = 1;
	//读数据
	Temp = (SpiaRegs.SPIRXBUF);// & (0x00FF));
	//NOP7;
	return Temp;
}

//---------------------------------------------------------------------------
// Example: InitSpiGpio: 
//---------------------------------------------------------------------------
// This function initializes GPIO pins to function as SPI pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.  
// 
// Caution: 
// For each SPI peripheral
// Only one GPIO pin should be enabled for SPISOMO operation.
// Only one GPIO pin should be enabled for SPISOMI operation. 
// Only one GPIO pin should be enabled for SPICLKA operation. 
// Only one GPIO pin should be enabled for SPISTEA operation. 
// Comment out other unwanted lines.

void InitSpiGpio()
{

   InitSpiaGpio();
}

void InitSpiaGpio()
{

   EALLOW;
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user.  
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // Enable pull-up on GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;   // Enable pull-up on GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;   // Enable pull-up on GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;   // Enable pull-up on GPIO19 (SPISTEA)


//    GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0;   // Enable pull-up on GPIO54 (SPISIMOA)
//    GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;   // Enable pull-up on GPIO55 (SPISOMIA)
//    GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;   // Enable pull-up on GPIO56 (SPICLKA)
//    GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0;   // Enable pull-up on GPIO57 (SPISTEA)

/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // Asynch input GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)

//    GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3; // Asynch input GPIO16 (SPISIMOA)
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3; // Asynch input GPIO17 (SPISOMIA)
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO18 (SPICLKA)
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 3; // Asynch input GPIO19 (SPISTEA)

    
/* Configure SPI-A pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be SPI functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1; // Configure GPIO16 as SPISIMOA
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1; // Configure GPIO17 as SPISOMIA
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1; // Configure GPIO18 as SPICLKA
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1; // Configure GPIO19 as SPISTEA

//    GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1; // Configure GPIO54 as SPISIMOA
//    GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1; // Configure GPIO55 as SPISOMIA
//    GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1; // Configure GPIO56 as SPICLKA
//    GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 1; // Configure GPIO57 as SPISTEA

    EDIS;
}

//===========================================================================
// End of file.
//===========================================================================
