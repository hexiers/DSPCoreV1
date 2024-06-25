// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:46:25 $
//###########################################################################
//
// FILE:	DSP2833x_Gpio.c
//
// TITLE:	DSP2833x General Purpose I/O Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x/DSP2823x C/C++ Header Files V1.31 $
// $Release Date: August 4, 2009 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#define GPIO_FUNC 0
#define PERP1_FUNC 1
#define PERP2_FUNC 2
#define PERP3_FUNC 3
#define OUT_DIR 1
#define IN_DIR 0

//#define VERSION_V4
#define VERSION_V5

//---------------------------------------------------------------------------
// InitGpio: 
//---------------------------------------------------------------------------
// This function initializes the Gpio to a known (default) state.
//
// For more details on configuring GPIO's as peripheral functions,
// refer to the individual peripheral examples and/or GPIO setup example. 
void InitGpio(void)
{
    EALLOW;
	//=========================================================0-15
	 //6·ȫ��PWM���
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = PERP1_FUNC;
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = PERP1_FUNC;
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = PERP1_FUNC;
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = PERP1_FUNC;
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = PERP1_FUNC;
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = PERP1_FUNC;

	// ���ȿ��������,EPS_V5Ϊ���Ե�
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = GPIO_FUNC;
	FAN_DIS = 0;//������
	GpioCtrlRegs.GPADIR.bit.GPIO6 = OUT_DIR;
	
	//�̵������������
	GpioCtrlRegs.GPAMUX1.bit.GPIO7= GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO7 = OUT_DIR;
	//RELAY_OPEN = 1;//�̵������պ�
	HAL_SetPreChargeRelayClose(FALSE);
	
	//DAƬѡ���������
	GpioCtrlRegs.GPAMUX1.bit.GPIO8= GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO8 = OUT_DIR;
	DA_CS = 1;
	
	//����ָʾ�ƿ��������
	GpioCtrlRegs.GPAMUX1.bit.GPIO9= GPIO_FUNC; 
	GpioCtrlRegs.GPADIR.bit.GPIO9 = OUT_DIR;
	LED_RUN = 0;
	
	//����ָʾ�ƿ��������
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO10 = OUT_DIR;
	LED_ERR = 0;
	
	//�û�ֹͣ����������
	GpioCtrlRegs.GPAMUX1.bit.GPIO11= GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO11 = IN_DIR;

	//����TZ����
	GpioCtrlRegs.GPAMUX1.bit.GPIO12= PERP1_FUNC;
	
	//��·TZ����
	GpioCtrlRegs.GPAMUX1.bit.GPIO13= PERP1_FUNC;
	
	//���Ե�T37
	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO14 = OUT_DIR;

	//���Ե�T38
	GpioCtrlRegs.GPAMUX1.bit.GPIO15 = GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO15 = OUT_DIR;
	
	//=========================================================16-31
	//CAN ID����������
	GpioCtrlRegs.GPAMUX2.bit.GPIO16= GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO16 = IN_DIR;
	GpioCtrlRegs.GPAMUX2.bit.GPIO17= GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO17 = IN_DIR;

	//���Ե�T39
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO18 = OUT_DIR;
	//���Ե�T40
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO19 = OUT_DIR;

	//QEP����
	GpioCtrlRegs.GPAMUX2.bit.GPIO20= PERP1_FUNC;
	GpioCtrlRegs.GPAMUX2.bit.GPIO21= PERP1_FUNC;
	GpioCtrlRegs.GPAMUX2.bit.GPIO23= PERP1_FUNC;
	//���Ե�T41����HC�ź��������������¼ԭ��
	GpioCtrlRegs.GPAMUX2.bit.GPIO22 = PERP1_FUNC;
	
	//���Ե�T41
	//GpioCtrlRegs.GPAMUX2.bit.GPIO22= GPIO_FUNC;
	//GpioCtrlRegs.GPADIR.bit.GPIO22 = OUT_DIR;
	//GPIO����
	GpioCtrlRegs.GPAMUX2.bit.GPIO24= GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO24 = IN_DIR;
	GpioCtrlRegs.GPAMUX2.bit.GPIO25= GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO25 = IN_DIR;
	GpioCtrlRegs.GPAMUX2.bit.GPIO26= GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO26 = IN_DIR;

	//CAP4
	GpioCtrlRegs.GPAMUX2.bit.GPIO27= PERP1_FUNC;

	GpioCtrlRegs.GPAQSEL2.bit.GPIO20 = 2;//6clk
	GpioCtrlRegs.GPAQSEL2.bit.GPIO21 = 2;//6clk
	GpioCtrlRegs.GPAQSEL2.bit.GPIO22 = 2;//6clk
	GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 2;//6clk
	GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 2;//6clk
	GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 2;//6clk
	GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = 2;//6clk
	GpioCtrlRegs.GPAQSEL2.bit.GPIO27 = 2;//6clk

#ifdef VERSION_V4
	//Ƭѡ6
	GpioCtrlRegs.GPAMUX2.bit.GPIO28= PERP2_FUNC;
	
	//CAN�͹��Ŀ��������
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = GPIO_FUNC;
	GpioCtrlRegs.GPADIR.bit.GPIO29 = OUT_DIR;
	CAN_STB = 0;//������͹���

#endif
#ifdef VERSION_V5
	//����A����
	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = PERP1_FUNC;
	GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;
	//����A����
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = PERP1_FUNC;

#endif
	

	//CAN�շ�
	GpioCtrlRegs.GPAMUX2.bit.GPIO30 = PERP1_FUNC;
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = PERP1_FUNC;
	GpioCtrlRegs.GPAQSEL2.bit.GPIO30 = 3;   // Asynch qual for GPIO30 (CANRXA)
	
	//=========================================================32-47
	//���������������
	GpioCtrlRegs.GPBMUX1.bit.GPIO32= GPIO_FUNC; 
	GpioCtrlRegs.GPBDIR.bit.GPIO32 = OUT_DIR;
	BUZZER = 0;//����

	//����ɼ��������������
	GpioCtrlRegs.GPBMUX1.bit.GPIO33= GPIO_FUNC; 
	GpioCtrlRegs.GPBDIR.bit.GPIO33 = OUT_DIR;
	A_SAMPLE = 1;	
	
	//���Ե�T43
	//GpioCtrlRegs.GPBMUX1.bit.GPIO34= GPIO_FUNC; 
	//GpioCtrlRegs.GPBDIR.bit.GPIO34= OUT_DIR;
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = PERP1_FUNC;//����ת�ز���

#ifdef VERSION_V4
	//����A����
	GpioCtrlRegs.GPBMUX1.bit.GPIO35= PERP1_FUNC;
	//����A����
	GpioCtrlRegs.GPBMUX1.bit.GPIO36= PERP1_FUNC;
	GpioCtrlRegs.GPBQSEL1.bit.GPIO36 = 3;

#endif
#ifdef VERSION_V5
	// Ƭѡ0
	GpioCtrlRegs.GPBMUX1.bit.GPIO36 = PERP2_FUNC;
#endif

	//���Ե�T42
	GpioCtrlRegs.GPBMUX1.bit.GPIO37= GPIO_FUNC; 
	GpioCtrlRegs.GPBDIR.bit.GPIO37= OUT_DIR;
	
	//д�ź�
	GpioCtrlRegs.GPBMUX1.bit.GPIO38= PERP3_FUNC;

	//GPIO39:V4�汾SCI�����жϽš�
	
	//���Ե�T44
	GpioCtrlRegs.GPBMUX1.bit.GPIO40= GPIO_FUNC; 
	GpioCtrlRegs.GPBDIR.bit.GPIO40= OUT_DIR;

	//�����ַ��
	GpioCtrlRegs.GPBMUX1.bit.GPIO41 = PERP3_FUNC;  // XA1
	GpioCtrlRegs.GPBMUX1.bit.GPIO42 = PERP3_FUNC;  // XA2

	//��������,Ĭ������
	GpioCtrlRegs.GPBMUX1.bit.GPIO43 = GPIO_FUNC;  // XA3
	GpioCtrlRegs.GPBMUX1.bit.GPIO44 = GPIO_FUNC;  // XA4
	GpioCtrlRegs.GPBMUX1.bit.GPIO45 = GPIO_FUNC;  // XA5
	GpioCtrlRegs.GPBMUX1.bit.GPIO46 = GPIO_FUNC;  // XA6
	GpioCtrlRegs.GPBDIR.bit.GPIO43= IN_DIR;
	GpioCtrlRegs.GPBDIR.bit.GPIO44= IN_DIR;
	GpioCtrlRegs.GPBDIR.bit.GPIO45= IN_DIR;
	GpioCtrlRegs.GPBDIR.bit.GPIO46= IN_DIR;


	//GPIO43-47δ��
	//=========================================================49-63
	//GPIO49-53δ��
	//CAN�͹��Ŀ��������

	GpioCtrlRegs.GPBMUX2.bit.GPIO48 = GPIO_FUNC;
	GpioCtrlRegs.GPBDIR.bit.GPIO48 = OUT_DIR;
	CAN_STB = 0;//������͹���

	//SPI SIMO
	GpioCtrlRegs.GPBMUX2.bit.GPIO54 = PERP1_FUNC;
	//SPI SOMI
	GpioCtrlRegs.GPBMUX2.bit.GPIO55 = PERP1_FUNC;
	//SPI CLK
	GpioCtrlRegs.GPBMUX2.bit.GPIO56 = PERP1_FUNC;

	//SPIƬѡ���
	GpioCtrlRegs.GPBMUX2.bit.GPIO57 = GPIO_FUNC;
	GpioCtrlRegs.GPBDIR.bit.GPIO57 = OUT_DIR;
	ANGLE_CS = 1;

	//����SPI SIMO���
	GpioCtrlRegs.GPBMUX2.bit.GPIO58 = GPIO_FUNC;
	GpioCtrlRegs.GPBDIR.bit.GPIO58 = OUT_DIR;
	
	AT25320_IN = 1;
	//����SPI CLK���
	GpioCtrlRegs.GPBMUX2.bit.GPIO59 = GPIO_FUNC;
	GpioCtrlRegs.GPBDIR.bit.GPIO59 = OUT_DIR;
	AT25320_CLK = 0;
	//����SPI SOMI����
	GpioCtrlRegs.GPBMUX2.bit.GPIO60 = GPIO_FUNC;
	GpioCtrlRegs.GPBDIR.bit.GPIO60 = IN_DIR;
	//����SPI Ƭѡ���
	GpioCtrlRegs.GPBMUX2.bit.GPIO61 = GPIO_FUNC;
	GpioCtrlRegs.GPBDIR.bit.GPIO61 = OUT_DIR;
	AT25320_CS = 1;

	//GPIO62-63δ��

	//=========================================================64-79
	//16λ��������
	 GpioCtrlRegs.GPCMUX1.bit.GPIO64 = PERP3_FUNC;  // XD15
	 GpioCtrlRegs.GPCMUX1.bit.GPIO65 = PERP3_FUNC;  // XD14
	 GpioCtrlRegs.GPCMUX1.bit.GPIO66 = PERP3_FUNC;  // XD13
	 GpioCtrlRegs.GPCMUX1.bit.GPIO67 = PERP3_FUNC;  // XD12
	 GpioCtrlRegs.GPCMUX1.bit.GPIO68 = PERP3_FUNC;  // XD11
	 GpioCtrlRegs.GPCMUX1.bit.GPIO69 = PERP3_FUNC;  // XD10
	 GpioCtrlRegs.GPCMUX1.bit.GPIO70 = PERP3_FUNC;  // XD19
	 GpioCtrlRegs.GPCMUX1.bit.GPIO71 = PERP3_FUNC;  // XD8
	 GpioCtrlRegs.GPCMUX1.bit.GPIO72 = PERP3_FUNC;  // XD7
	 GpioCtrlRegs.GPCMUX1.bit.GPIO73 = PERP3_FUNC;  // XD6
	 GpioCtrlRegs.GPCMUX1.bit.GPIO74 = PERP3_FUNC;  // XD5
	 GpioCtrlRegs.GPCMUX1.bit.GPIO75 = PERP3_FUNC;  // XD4
	 GpioCtrlRegs.GPCMUX1.bit.GPIO76 = PERP3_FUNC;  // XD3
	 GpioCtrlRegs.GPCMUX1.bit.GPIO77 = PERP3_FUNC;  // XD2
	 GpioCtrlRegs.GPCMUX1.bit.GPIO78 = PERP3_FUNC;  // XD1
	 GpioCtrlRegs.GPCMUX1.bit.GPIO79 = PERP3_FUNC;  // XD0
	//=========================================================80-87
	//GPIO80-87δ��


	#if EN_PWMINPUT
		GpioCtrlRegs.GPAMUX2.bit.GPIO24= PERP1_FUNC;
		GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 2;//6clkͬ��	
		
		GpioCtrlRegs.GPBMUX1.bit.GPIO34 = GPIO_FUNC;
		GpioCtrlRegs.GPBDIR.bit.GPIO34 = IN_DIR;
		
	#endif

     EDIS;
     
}
	
//===========================================================================
// End of file.
//===========================================================================
