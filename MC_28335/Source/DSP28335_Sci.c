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
	Uart_Rx();/* ������ֹ1ms�����ܼ�ʱ��������ɵ����ݶ�ʧ*/
	//��ӦPIE�����ж�
	PieCtrlRegs.PIEACK.bit.ACK9= 1;
	//������輶�жϱ�ʶλ
	SciaRegs.SCIFFRX.bit.RXFFINTCLR= 1;

}  

void	InitSci()
{
	//SCI-A
	//=========================================
	//SCICTL1 ʹ�ܽ��պͷ��ͣ���ֹ˯�ߣ���ֹ���մ����ж�
	SciaRegs.SCICTL1.bit.RXENA = 1;
	SciaRegs.SCICTL1.bit.TXENA = 1;
	SciaRegs.SCICTL1.bit.SLEEP = 0;
	//��ʼ��ĩβ�����˳���λ
	SciaRegs.SCICTL1.bit.SWRESET = 0;
	SciaRegs.SCICTL1.bit.RXERRINTENA = 0;
	//=========================================
	//SCICTL2
	//��ֹ�����ж�
	SciaRegs.SCICTL2.bit.TXINTENA  = 0;
	//��ֹ�����ж�
	SciaRegs.SCICTL2.bit.RXBKINTENA = 0;
	//=========================================
	//SCICCR ��������֡��ʽ
	SciaRegs.SCICCR.bit.SCICHAR = 7;
	SciaRegs.SCICCR.bit.ADDRIDLE_MODE = 0;
	SciaRegs.SCICCR.bit.LOOPBKENA = 0;
	SciaRegs.SCICCR.bit.PARITYENA = 0;
	SciaRegs.SCICCR.bit.STOPBITS = 0;
	//=========================================
	//���ò�����115200
	SciaRegs.SCIHBAUD = 0x00;
	SciaRegs.SCILBAUD = 0x15;
	//=========================================
	//����FIFO
	SciaRegs.SCIFFTX.bit.TXFFIENA = 0;
	SciaRegs.SCIFFTX.bit.TXFFINTCLR = 1;
	//��ʼ��ĩβ�����˳���λ
	SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 0;
	SciaRegs.SCIFFTX.bit.SCIFFENA = 1;
	//=========================================
	//����FIFO����
	SciaRegs.SCIFFRX.bit.RXFFIENA = 1;	
	SciaRegs.SCIFFRX.bit.RXFFIL = 12;//ֻҪ����FIFO����12�����ݾͿ��Բ��������ж�
	SciaRegs.SCIFFRX.bit.RXFFIENA = 1;//ʹ�ܽ���FIFO�ж�
	SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;//�������FIFO��־
	SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1;//�������FIFO�����־
	//��ʼ��ĩβ�����˳���λ
	SciaRegs.SCIFFRX.bit.RXFIFORESET = 0;
	//=========================================
	//�˳���λ
	SciaRegs.SCICTL1.bit.SWRESET = 1;
	SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 1;  
	SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;
	// Initialize SCI-B:

	//tbd...
}
//===========================================================================
// End of file.
//===========================================================================
