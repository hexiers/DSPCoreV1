// TI File $Revision: /main/2 $
// Checkin $Date: March 1, 2007   16:06:07 $
//###########################################################################
//
// FILE:	DSP2833x_Sci.c
//
// TITLE:	DSP2833x SCI Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x/DSP2823x C/C++ Header Files V1.31 $
// $Release Date: August 4, 2009 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
extern bool Uart_Rx(void);
interrupt void SCIRXINTA_ISR(void)     // SCI-A
{
	//DataRev();
	Uart_Rx();/* 用来防止1ms任务不能及时收数据造成的数据丢失*/
	//响应PIE级别中断
	PieCtrlRegs.PIEACK.bit.ACK9= 1;
	//清除外设级中断标识位
	SciaRegs.SCIFFRX.bit.RXFFINTCLR= 1;

}  

void	InitSci()
{
	//SCI-A
	//=========================================
	//SCICTL1 使能接收和发送，禁止睡眠，禁止接收错误中断
	SciaRegs.SCICTL1.bit.RXENA = 1;
	SciaRegs.SCICTL1.bit.TXENA = 1;
	SciaRegs.SCICTL1.bit.SLEEP = 0;
	//初始化末尾必须退出复位
	SciaRegs.SCICTL1.bit.SWRESET = 0;
	SciaRegs.SCICTL1.bit.RXERRINTENA = 0;
	//=========================================
	//SCICTL2
	//禁止发送中断
	SciaRegs.SCICTL2.bit.TXINTENA  = 0;
	//禁止接收中断
	SciaRegs.SCICTL2.bit.RXBKINTENA = 0;
	//=========================================
	//SCICCR 配置数据帧格式
	SciaRegs.SCICCR.bit.SCICHAR = 7;
	SciaRegs.SCICCR.bit.ADDRIDLE_MODE = 0;
	SciaRegs.SCICCR.bit.LOOPBKENA = 0;
	SciaRegs.SCICCR.bit.PARITYENA = 0;
	SciaRegs.SCICCR.bit.STOPBITS = 0;
	//=========================================
	//配置波特率115200
	SciaRegs.SCIHBAUD = 0x00;
	SciaRegs.SCILBAUD = 0x15;
	//=========================================
	//发送FIFO
	SciaRegs.SCIFFTX.bit.TXFFIENA = 0;
	SciaRegs.SCIFFTX.bit.TXFFINTCLR = 1;
	//初始化末尾必须退出复位
	SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 0;
	SciaRegs.SCIFFTX.bit.SCIFFENA = 1;
	//=========================================
	//接收FIFO开启
	SciaRegs.SCIFFRX.bit.RXFFIENA = 1;	
	SciaRegs.SCIFFRX.bit.RXFFIL = 12;//只要接收FIFO中有12个数据就可以产生接收中断
	SciaRegs.SCIFFRX.bit.RXFFIENA = 1;//使能接收FIFO中断
	SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;//清除接收FIFO标志
	SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1;//清除接收FIFO溢出标志
	//初始化末尾必须退出复位
	SciaRegs.SCIFFRX.bit.RXFIFORESET = 0;
	//=========================================
	//退出复位
	SciaRegs.SCICTL1.bit.SWRESET = 1;
	SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 1;  
	SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;
	// Initialize SCI-B:

	//tbd...
}
//===========================================================================
// End of file.
//===========================================================================
