/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����MCCore�õ����������ͺ����ݳ����Լ��������ݴ����
* @������		:  	����                                                                              
* ��������		:   	2020-03-03                                                                 
* �޸ļ�¼		:   
*                   
    20200911:������Ըߵ���ת���£��ٶȷ�������Сϵ�����Լ��ݸ�ת�ٵ��
======================================================================================================*/
#ifndef DATATYPE_H_H
#define DATATYPE_H_H


typedef unsigned int       		Uint16;
typedef unsigned long      	Uint32;
typedef unsigned long long 	Uint64;
typedef unsigned int		INT16U;
typedef signed int			INT16S;
typedef signed int			int16;
typedef signed long			INT32S;
typedef signed long long		INT64S;
typedef float              		float32;
typedef long double        		float64;
typedef int  				bool;
typedef unsigned char		UCHAR;


#define BIT0    0x0001
#define BIT1    0x0002
#define BIT2    0x0004
#define BIT3    0x0008
#define BIT4    0x0010
#define BIT5    0x0020
#define BIT6    0x0040
#define BIT7    0x0080
#define BIT8    0x0100
#define BIT9    0x0200
#define BIT10   0x0400
#define BIT11   0x0800
#define BIT12   0x1000
#define BIT13   0x2000
#define BIT14   0x4000
#define BIT15   0x8000

#define TRUE   	1
#define FALSE   0
#define NULL 	0

#define PWMCLKFREQ   	60000000L
#define PI   				3.1415926
#define PIQ16   		205887L
#define LIMITMAXMIN(DataIn,MaxLim,MinLim)   (((DataIn)<(MinLim) ? (MinLim):((DataIn)>(MaxLim)?(MaxLim):(DataIn))))
#define SLOPELIMIT(New,Old,PSlope,NSlope)  ((((New) - (Old)) > (PSlope) ? ((Old) + (PSlope)):(((New) - (Old)) < (NSlope))?((Old) + (NSlope)):(New)))
#define  ABS(x) (((x)>=0)?(x):(-(x)))

/* �̶��궨��*/
#define FPGA_SPD	1
#define DSP_SPD		2

/* ============================================================================================*/
/* �ɱ�궨��*/
#define SPDFDB_SCALE_FACTOR_POW 2// �ٶȷ���������Сϵ�����ݣ���Ӧ1/4 
#define SPD_MODE 	FPGA_SPD



/* ============================================================================================*/
/* �̶��궨��*/
#define SPDFDB_SCALE_FACTOR (1L << SPDFDB_SCALE_FACTOR_POW)// �ٶȷ���������Сϵ��
#endif

