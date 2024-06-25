/**
 ******************************************************************************
 * Copyright (C) 2020-2030, CAMIC All Rights Reserved
 * @file	MCSoft_Uart.h
 * @author	caih
 * @version 1.0
 * @date	2020/03/02
 * @brief	Uart交互协议
 * Revision history:
 * <Author> 			<Date>			<Version>		<Description>
 *	caih				2020/03/02			1.0			首次创建
 ******************************************************************************
 */
#include"MC_Include.h"
 
#ifndef __MCSOFT_UART_H
#define __MCSOFT_UART_H


#define MAX_TX_LEN      (256)	        /* 发送缓冲区最大长度 */
#define MAX_RX_LEN      (512) 	        /* 接收缓冲区最大长度 */

#define RX_FRAME_HEAD1      (0xFF)      /* 接收帧头1 */
#define RX_FRAME_HEAD2     	(0xFF)      /* 接收帧头2 */
#define RX_FRAME_ATTR       (0x11)      /* 接收帧属性 */
#define RX_FRAME_TAIL       (0xEE)      /* 接收帧尾 */
#define RX_FRAME_BYTE_LEN   (0x14)      /* 接收帧长字节数 */
#define RX_FRAME_MAX_LEN    (20)        /* 接收帧缓冲区最大长度 */
#define CHK_TIMEOUT         (40)        /* 检查接收帧超时计数值 */

#define TX_FRAME_HEAD1      (0xFF)      /* 发送帧头1 */
#define TX_FRAME_HEAD2      (0xFF)      /* 发送帧头2 */
#define TX_FRAME_ATTR       (0x00)      /* 发送帧属性 */
#define TX_FRAME_BYTE_LEN   (0x88)      /* 发送帧长字节数 */
#define TX_FRAME_TAIL       (0xEE)      /* 发送帧尾 */

struct UAR_SEND_REGS
{
	Uint16 TDR;             /* 发送数据寄存器*/
	Uint16 TBRSR;           /* 发送波特率设置寄存器*/
	Uint16 TPSR;            /* 发送方式设置寄存器*/
	Uint16 TFR;             /* 发送模块标识寄存器*/
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
	Uint16 TSRR;            /* 发送模块软件复位寄存器*/
};

struct UART_REC_REGS 
{
	Uint16 RDR;             /* 接收数据寄存器*/
	Uint16 RBRSR;           /* 接收波特率社这寄存器*/
	Uint16 RPSR;            /* 接收校验方式设置寄存器*/
	Uint16 RFR;             /* 接收模块标识寄存器*/
	Uint16 RCR;             /* 接收数据个数寄存器*/
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
	Uint16 RSRR;            /*接收模块软件复位寄存器*/
};

//extern volatile struct UAR_SEND_REGS UartTxRs422;
//extern volatile struct UART_REC_REGS UartRxRs422;

/* 检查接收有限状态机状态枚举 */
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

/* 接收帧内容结构体 */
typedef struct
{
    Uint16 CmdWord;
    Uint16 Data[6];
}ST_RXFRM;

/* 发送参数管理结构体 */
typedef struct
{
    void *pTxStr;
    Uint16 *pSize;
}ST_TXSTRUCT;

/* 环形队列管理结构体*/
typedef struct
{   
    Uint16 *pRxBuf;         /* 指向接收缓冲区的指针 */
    Uint16 BufLen;          /* 环形队列最大长度 */
    Uint16 LastPos;         /* 环形队列最末尾位置 */
    Uint16 RdPos;           /* 环形队列读位置 */
    Uint16 WrPos;           /* 环形队列写位置 */
    E_CHKSTS eChkSts;       /* 检查接收状态 */
    Uint16 ChkCnt;          /* 检查接收次数计数值 */
    Uint16 HandledLen;      /* 记录已处理的长度 */
    Uint16 RxLen;           /* 记录环形队列接收数据的长度 */
    bool bNewFrmFlg;        /* 新的接收帧标志 */
    Uint16 *pRxFrmBuf;      /* 指向接收帧缓冲区指针 */
    ST_RXFRM *pstRxFrm;     /* 指向接收帧内容结构体指针 */
}ST_RBUFMNG;

extern ST_RBUFMNG stRBufMng;
extern void MemCpy(Uint16 *pSrcAddr, Uint16 *pDstAddr, Uint16 Len);
extern void MemSet(Uint16 *pAddr, Uint16 Val, Uint16 Len);
extern void Uart_Init(void);

#endif

