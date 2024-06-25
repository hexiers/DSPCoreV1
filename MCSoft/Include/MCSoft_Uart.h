/**
 ******************************************************************************
 * Copyright (C) 2020-2030, CAMIC All Rights Reserved
 * @file	MCSoft_Uart.h
 * @author	caih
 * @version 1.0
 * @date	2020/03/02
 * @brief	Uart����Э��
 * Revision history:
 * <Author> 			<Date>			<Version>		<Description>
 *	caih				2020/03/02			1.0			�״δ���
 ******************************************************************************
 */
#include"MC_Include.h"
 
#ifndef __MCSOFT_UART_H
#define __MCSOFT_UART_H


#define MAX_TX_LEN      (256)	        /* ���ͻ�������󳤶� */
#define MAX_RX_LEN      (512) 	        /* ���ջ�������󳤶� */

#define RX_FRAME_HEAD1      (0xFF)      /* ����֡ͷ1 */
#define RX_FRAME_HEAD2     	(0xFF)      /* ����֡ͷ2 */
#define RX_FRAME_ATTR       (0x11)      /* ����֡���� */
#define RX_FRAME_TAIL       (0xEE)      /* ����֡β */
#define RX_FRAME_BYTE_LEN   (0x14)      /* ����֡���ֽ��� */
#define RX_FRAME_MAX_LEN    (20)        /* ����֡��������󳤶� */
#define CHK_TIMEOUT         (40)        /* ������֡��ʱ����ֵ */

#define TX_FRAME_HEAD1      (0xFF)      /* ����֡ͷ1 */
#define TX_FRAME_HEAD2      (0xFF)      /* ����֡ͷ2 */
#define TX_FRAME_ATTR       (0x00)      /* ����֡���� */
#define TX_FRAME_BYTE_LEN   (0x88)      /* ����֡���ֽ��� */
#define TX_FRAME_TAIL       (0xEE)      /* ����֡β */

struct UAR_SEND_REGS
{
	Uint16 TDR;             /* �������ݼĴ���*/
	Uint16 TBRSR;           /* ���Ͳ��������üĴ���*/
	Uint16 TPSR;            /* ���ͷ�ʽ���üĴ���*/
	Uint16 TFR;             /* ����ģ���ʶ�Ĵ���*/
	Uint16 rsd4;
	Uint16 rsd5;
	Uint16 rsd6;
	Uint16 rsd7;
	Uint16 rsd8;
	Uint16 rsd9;
	Uint16 rsdA;
	Uint16 rsdB;
	Uint16 rsdC;
	Uint16 rsdD;
	Uint16 rsdE;
	Uint16 TSRR;            /* ����ģ�������λ�Ĵ���*/
};

struct UART_REC_REGS 
{
	Uint16 RDR;             /* �������ݼĴ���*/
	Uint16 RBRSR;           /* ���ղ���������Ĵ���*/
	Uint16 RPSR;            /* ����У�鷽ʽ���üĴ���*/
	Uint16 RFR;             /* ����ģ���ʶ�Ĵ���*/
	Uint16 RCR;             /* �������ݸ����Ĵ���*/
	Uint16 rsd15;
	Uint16 rsd16;
	Uint16 rsd17;
	Uint16 rsd18;
	Uint16 rsd19;
	Uint16 rsd1A;
	Uint16 rsd1B;
	Uint16 rsd1C;
	Uint16 rsd1D;
	Uint16 rsd1E;
	Uint16 RSRR;            /*����ģ�������λ�Ĵ���*/
};

//extern volatile struct UAR_SEND_REGS UartTxRs422;
//extern volatile struct UART_REC_REGS UartRxRs422;

/* ����������״̬��״̬ö�� */
typedef enum
{
    CHK_HEAD1 = 0,
    CHK_HEAD2,
    CHK_ATTR,
    CHK_LEN,
    CHK_TAIL,
    CHK_SUM,
    CHK_WAIT
}E_CHKSTS;

/* ����֡���ݽṹ�� */
typedef struct
{
    Uint16 CmdWord;
    Uint16 Data[6];
}ST_RXFRM;

/* ���Ͳ�������ṹ�� */
typedef struct
{
    void *pTxStr;
    Uint16 *pSize;
}ST_TXSTRUCT;

/* ���ζ��й���ṹ��*/
typedef struct
{   
    Uint16 *pRxBuf;         /* ָ����ջ�������ָ�� */
    Uint16 BufLen;          /* ���ζ�����󳤶� */
    Uint16 LastPos;         /* ���ζ�����ĩβλ�� */
    Uint16 RdPos;           /* ���ζ��ж�λ�� */
    Uint16 WrPos;           /* ���ζ���дλ�� */
    E_CHKSTS eChkSts;       /* ������״̬ */
    Uint16 ChkCnt;          /* �����մ�������ֵ */
    Uint16 HandledLen;      /* ��¼�Ѵ���ĳ��� */
    Uint16 RxLen;           /* ��¼���ζ��н������ݵĳ��� */
    bool bNewFrmFlg;        /* �µĽ���֡��־ */
    Uint16 *pRxFrmBuf;      /* ָ�����֡������ָ�� */
    ST_RXFRM *pstRxFrm;     /* ָ�����֡���ݽṹ��ָ�� */
}ST_RBUFMNG;

extern ST_RBUFMNG stRBufMng;
extern void MemCpy(Uint16 *pSrcAddr, Uint16 *pDstAddr, Uint16 Len);
extern void MemSet(Uint16 *pAddr, Uint16 Val, Uint16 Len);
extern void Uart_Init(void);

#endif

