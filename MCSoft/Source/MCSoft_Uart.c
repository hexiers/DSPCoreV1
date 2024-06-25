/**
 ******************************************************************************
 * Copyright (C) 2020-2030, CAMIC All Rights Reserved
 * @file	MCSoft_Uart.c
 * @author	caih
 * @version 1.0
 * @date	2020/03/02
 * @brief	Uart交互协议
 * Revision history:
 * <Author> 			<Date>			<Version>		<Description>
 *	caih				2020/03/02			1.0			首次创建
 ******************************************************************************
 */
#include "MCSoft_Uart.h"
//#include"MC_Include.h"

// #pragma DATA_SECTION(UartTxRs422,"UartTxRs422File");
//volatile struct UAR_SEND_REGS UartTxRs422;

//#pragma DATA_SECTION(UartRxRs422,"UartRxRs422File");
//volatile struct UART_REC_REGS UartRxRs422;


const Uint16 m1MotorParaSize =sizeof(m1MotorPara)/sizeof(Uint16);
const Uint16 m1CtrlParaSize =sizeof(m1CtrlPara)/sizeof(Uint16);
const Uint16 m1FaultParaSize =sizeof(m1FaultPara)/sizeof(Uint16);
const Uint16 m1WarnParaSize =sizeof(m1WarnPara)/sizeof(Uint16);
const Uint16 m1SnsLessParaSize =sizeof(m1SnsLessPara)/sizeof(Uint16);
const Uint16 m1HalParaSize =sizeof(m1HalPara)/sizeof(Uint16);
const Uint16 m1SpdFdbParaSize =sizeof(m1SpdFdbPara)/sizeof(Uint16);
const Uint16 m1SysCfgParaSize =sizeof(m1SysCfgPara)/sizeof(Uint16);
const Uint16 m1StartUpSize =sizeof(m1StartUp)/sizeof(Uint16);
const Uint16 m1CurrLoopCtlerSize =sizeof(m1CurrLoopCtler)/sizeof(Uint16);
const Uint16 m1SpdLoopCtlerSize =sizeof(m1SpdLoopCtler)/sizeof(Uint16);
const Uint16 m1BusVolDecouplerSize =sizeof(m1BusVolDecoupler)/sizeof(Uint16);
const Uint16 m1SmcObserverSize =sizeof(m1SmcObserverExt)/sizeof(Uint16);
const Uint16 m1ResolverDealModuleSize =sizeof(m1ResolverDealModule)/sizeof(Uint16);
const Uint16 m1FaultDealModuleSize =sizeof(m1FaultDealModule)/sizeof(Uint16);
const Uint16 m1CriParaSize =sizeof(m1CriPara)/sizeof(Uint16);

/* 串口接收数据缓冲区 */
Uint16 UartRxBuf[MAX_RX_LEN];
/* 串口发送数据缓冲区 */
Uint16 UartTxBuf[MAX_TX_LEN];
/* 接收帧缓冲区 */
Uint16 RxFrmBuf[RX_FRAME_MAX_LEN];
/* 接收帧内容结构体 */
ST_RXFRM stRxFrm = {0, {0, 0, 0, 0, 0, 0}};
/* 环形队列管理结构体 */
ST_RBUFMNG stRBufMng = {UartRxBuf, MAX_RX_LEN, (MAX_RX_LEN-1), 0, 0, CHK_HEAD1, 0, 0, 0, FALSE, RxFrmBuf, &stRxFrm};
/* 发送参数序号 */
Uint16 TxStructIndex = 15;

Uint16 TxScopeCnt = 0;

/* 发送参数管理结构体数组 */
const ST_TXSTRUCT stTxStructArr[] =
{
    {(void *)&m1MotorPara, (Uint16 *)&m1MotorParaSize},
    {(void *)&m1CtrlPara, (Uint16 *)&m1CtrlParaSize},
    {(void *)&m1FaultPara, (Uint16 *)&m1FaultParaSize},
    {(void *)&m1WarnPara, (Uint16 *)&m1WarnParaSize},
    {(void *)&m1SnsLessPara, (Uint16 *)&m1SnsLessParaSize},
    {(void *)&m1HalPara, (Uint16 *)&m1HalParaSize},
    {(void *)&m1SpdFdbPara, (Uint16 *)&m1SpdFdbParaSize},
    {(void *)&m1SysCfgPara, (Uint16 *)&m1SysCfgParaSize},
    {(void *)&m1StartUp, (Uint16 *)&m1StartUpSize},
    {(void *)&m1CurrLoopCtler, (Uint16 *)&m1CurrLoopCtlerSize},
    {(void *)&m1SpdLoopCtler, (Uint16 *)&m1SpdLoopCtlerSize},
    {(void *)&m1BusVolDecoupler, (Uint16 *)&m1BusVolDecouplerSize},
    {(void *)&m1SmcObserverExt, (Uint16 *)&m1SmcObserverSize},
    {(void *)&m1ResolverDealModule, (Uint16 *)&m1ResolverDealModuleSize},
    {(void *)&m1FaultDealModule, (Uint16 *)&m1FaultDealModuleSize},
    {(void *)&m1CriPara, (Uint16 *)&m1CriParaSize},        //zyu: 关键参数结构体补充
};

static void RBuf_Clr(ST_RBUFMNG *pstRBufMng, Uint16 ClrLen);
static void RBuf_ChkRxLen(ST_RBUFMNG *pstRBufMng);
static void RBuf_ClrHandled(ST_RBUFMNG *pstRBufMng);
static Uint16 RBuf_RdByte(ST_RBUFMNG *pstRBufMng, Uint16 Offset);
static bool RBuf_RdData(ST_RBUFMNG *pstRBufMng, Uint16 Offset,Uint16 Len, Uint16 *pData);
extern bool PostMessge(stMCMessage * pMsg);
static void Uart_ConvToFrm(Uint16 *pSrcAddr, Uint16 *pDstAddr, Uint16 Len);
static void Uart_Tx(Uint16 *pTxData, Uint16 Num);
static bool Uart_ChkNewFrm(ST_RBUFMNG *pstRBufMng);

/**
 *******************************************************************************
 * @fn      Uart_Rx
 * @brief   串口接收底层函数，从FPGA中读取串口接收数据，放入环形队列中
 * @param   None
 * @return  TRUE: 成功, FALSE: 失败，环形队列数据溢出
 *******************************************************************************
 */
bool Uart_Rx(void)
{
    ST_RBUFMNG *pstRBufMng = &stRBufMng;
	Uint16 RxNum = SciaRegs.SCIFFRX.bit.RXFFST;//UartRxRs422.RCR;
    bool bStatus = TRUE;
    
	while(RxNum > 0)
	{
		if(pstRBufMng->WrPos != pstRBufMng->LastPos)
		{
			/* 写指针不为最后一个位置，且与读指针间隔1个字节以上，可以写 */
			if((pstRBufMng->WrPos + 1) != pstRBufMng->RdPos)
			{
				pstRBufMng->pRxBuf[pstRBufMng->WrPos] = SciaRegs.SCIRXBUF.bit.RXDT;
				pstRBufMng->WrPos++;
				RxNum--;
			}
			else
			{
				/* 数据溢出 */
				bStatus = FALSE;
				break;
			}
		}
		else
		{
			/* 当写指针为最后一个位置，但读指针不为0位置时，可以写，写完后写指针归0 */
			if(pstRBufMng->RdPos != 0)
			{
				pstRBufMng->pRxBuf[pstRBufMng->WrPos] = SciaRegs.SCIRXBUF.bit.RXDT;
				pstRBufMng->WrPos = 0;
				RxNum--;
			}
			else
			{
				/* 数据溢出 */
				bStatus = FALSE;
				break;
			}
		}
	}
	
	return bStatus;
}

/**
 *******************************************************************************
 * @fn      SetTxStructIndex
 * @brief   设置发送参数结构序号
 * @param   Uint16 Index    发送参数结构序号
 * @return  None
 *******************************************************************************
 */
void SetTxStructIndex(Uint16 Index)
{
    TxStructIndex = Index;
}

/**
 *******************************************************************************
 * @fn      SetTxStructIndex
 * @brief   获取发送参数结构序号
 * @param   None
 * @return  Uint16 发送参数结构序号
 *******************************************************************************
 */
Uint16 GetTxStructIndex(void)
{
    return TxStructIndex;
}


/**
 *******************************************************************************
 * @fn      Uart_TxFrm
 * @brief   串口发送帧协议组包发送
 * @param   None
 * @return  None
 *******************************************************************************
 */
void Uart_TxFrm(void)
{
	Uint16 *pTxBuf = &UartTxBuf[0];
	ST_TXSTRUCT *pstTxStruct = (ST_TXSTRUCT*)&stTxStructArr[0];
	Uint16 TxStrIndex;
	Uint16 i;
	Uint16 Sum = 0;

	/* 帧头 */
	*pTxBuf++ = (TX_FRAME_HEAD2 << 8) | TX_FRAME_HEAD1;
	/* 帧长度 属性 */
	*pTxBuf++ = (TX_FRAME_BYTE_LEN << 8) | TX_FRAME_ATTR;
	
	TxStrIndex = GetTxStructIndex();

    /* caih Add @20200528: 增加示波器观测变量 */
    if(Scope_IsFinished())
    {   
        if(TxScopeCnt == 0)
        {
            TxStrIndex = 16;
        }
        
        *pTxBuf++ = TxStrIndex++;
        SetTxStructIndex(TxStrIndex);
        Scope_Read(pTxBuf, 64);
        pTxBuf += 64;
        
        TxScopeCnt++;
        if(TxScopeCnt >= 3)
        {
            TxScopeCnt = 0;
            SetTxStructIndex(15);
        }
    }
    else
    {
        /* 默认发送 */
        SetTxStructIndex(15);
    	/* 发送参数类型索引号 */
    	*pTxBuf++ = TxStrIndex;
    	/* 拷贝要发送的结构体内容 */
    	MemCpy((Uint16 *)(pstTxStruct[TxStrIndex].pTxStr), pTxBuf, *(pstTxStruct[TxStrIndex].pSize));
    	pTxBuf += *(pstTxStruct[TxStrIndex].pSize);
    	/* 剩下的数据清零 */
    	MemSet(pTxBuf, 0, (TX_FRAME_BYTE_LEN / 2 - 4 - *(pstTxStruct[TxStrIndex].pSize)));
    }
    
	/* 重新调整pTxBuf，用以计算累加和*/
	pTxBuf = &UartTxBuf[2];
	for(i = 0; i < (TX_FRAME_BYTE_LEN / 2 - 3); i++)
	{
	    Sum = Sum + (*pTxBuf & 0xFF) + ((*pTxBuf >> 8) & 0xFF);
	    pTxBuf++;
	}

    /* 帧尾与校验和 */
	*pTxBuf = ((Uint16)TX_FRAME_TAIL << 8) | (Sum & 0xFF);

    /* 调用发送底层函数执行发送 */
	pTxBuf = &UartTxBuf[0];
	Uart_Tx(pTxBuf, TX_FRAME_BYTE_LEN/2);
}

/**
 *******************************************************************************
 * @fn      Uart_Tx
 * @brief   串口发送底层函数，通过FPGA发送
 * @param   Uint16 *pTxData     指向发送内容缓冲区的指针
 *          Uint16 Num          发送数据长度(16位为单位)
 * @return  None
 *******************************************************************************
 */

Uint16 TxBuf[TX_FRAME_BYTE_LEN];
Uint16 TxBufLen = 0;
static void Uart_Tx(Uint16 *pTxData, Uint16 Num)
{
    Uint16 i = 0;
	/* 由调用程序保证之前的数据可以发完*/
    while(Num--)
    {
    	TxBuf[i++] = (*pTxData) & 0xFF;
    	TxBuf[i++] = ((*pTxData) >> 8) & 0xFF;
    	//UartTxRs422.TDR = (*pTxData) & 0xFF;
        //UartTxRs422.TDR = ((*pTxData) >> 8) & 0xFF;
        pTxData++;
    }
    TxBufLen = TX_FRAME_BYTE_LEN;
}


/**
 *******************************************************************************
 * @fn      Uart_RxFrm
 * @brief   串口接收帧处理，根据不同的命令字进行相应处理
 * @param   ST_RBUFMNG *pstRBufMng 指向环形队列管理结构体指针
 * @return  None
 *******************************************************************************
 */
void Uart_RxFrm(void)
{
    stMCMessage stPostMsg;
    ST_RBUFMNG *pstRBufMng = &stRBufMng;
    
    /* 当存在新的接收帧时 */
    if(Uart_ChkNewFrm(pstRBufMng) == TRUE)
    {
        /* 接收数据拼接转换 */
        Uart_ConvToFrm(pstRBufMng->pRxFrmBuf, (Uint16 *)(pstRBufMng->pstRxFrm), 7);

        /* 判断命令字是否合法 */
        if(CmdSTARTCURRLOOP <= pstRBufMng->pstRxFrm->CmdWord && pstRBufMng->pstRxFrm->CmdWord <= CmdDoPOSOFFCALIB2)// V1.10.2
        {
            /* 取出数据内容 */
            stPostMsg.msg = (Message_t)pstRBufMng->pstRxFrm->CmdWord;
            stPostMsg.msgPara1 = pstRBufMng->pstRxFrm->Data[0];
            stPostMsg.msgPara2 = pstRBufMng->pstRxFrm->Data[1];
            stPostMsg.msgPara3 = pstRBufMng->pstRxFrm->Data[2];
            stPostMsg.msgPara4 = pstRBufMng->pstRxFrm->Data[3];
            stPostMsg.Priority = 0;

            /* zyu Modify @20200319*/ 
            /* 数据申请指令和数据更新指令不转换成消息，而是直接回发数据*/
            if(pstRBufMng->pstRxFrm->CmdWord == CmdREQSTRUCTDATA)
            {
                /* 设置发送数据索引号 */
                SetTxStructIndex(stPostMsg.msgPara1);
            }
            else if(pstRBufMng->pstRxFrm->CmdWord == CmdREQSINGLEDATA)
            {
                /* 设置示波器观测变量 */
                Scope_Config(stPostMsg.msgPara1, stPostMsg.msgPara2, stPostMsg.msgPara3);
            }           
            else if(pstRBufMng->pstRxFrm->CmdWord == CmdSetMonitor1)
            {
                /* 设置监视1观测变量 */
                Monitor1_Config(stPostMsg.msgPara1, stPostMsg.msgPara2);
            }           
            else if(pstRBufMng->pstRxFrm->CmdWord == CmdSetMonitor2)
            {
                /* 设置监视2观测变量 */
                Monitor2_Config(stPostMsg.msgPara1, stPostMsg.msgPara2);
            }           
            else if(pstRBufMng->pstRxFrm->CmdWord == CmdUPDATEPARA) 
            {
                /* 更新RAM中的参数 */
                if(UpdateRamPara(stPostMsg.msgPara1, stPostMsg.msgPara2, stPostMsg.msgPara3) == FALSE)
                {
                    MC_NotifyCmdDisCard(CmdUPDATEPARA);
                }
                else
                {
                    /* 更新参数成功 */
                	m1CriPara.MsgBox.bit.MsgBoxUpdateParaSucc = 1;
                }
            }
	        else
	        {
	        	/* 发送消息 */
           	    PostMessge(&stPostMsg);
            }

        }
        pstRBufMng->bNewFrmFlg = FALSE;
    }
}


/**
 *******************************************************************************
 * @fn      Uart_ConvToFrm
 * @brief   将接收的数据转换为帧数据，为了解决DSP最小数据结构为16位的问题
 * @param   Uint16 *pSrcAddr 源数据指针
 *          Uint16 *pDstAddr 目标数据指针
 *          Uint16 Len       帧数据个数
 * @return  None
 *******************************************************************************
 */
static void Uart_ConvToFrm(Uint16 *pSrcAddr, Uint16 *pDstAddr, Uint16 Len)
{
    while(Len--)
    {
        *pDstAddr = (*pSrcAddr & 0xFF) | (((*(pSrcAddr + 1)) << 8) & 0xFF00);
        pSrcAddr+=2;
        pDstAddr++;
    }
}

/**
 *******************************************************************************
 * @fn      Uart_ChkNewFrm
 * @brief   检查接收环形队列是否有新的接收帧数据
 * @param   ST_RBUFMNG *pstRBufMng 指向环形队列管理结构体指针
 * @return  TRUE: 接收到新的数据帧, FALSE: 没有新的数据帧
 *******************************************************************************
 */
static bool Uart_ChkNewFrm(ST_RBUFMNG *pstRBufMng)
{
    bool bChkContinueFlg;
    INT16S sIndex;
    Uint16 FrmLen;
    Uint16 Sum;
    
    if(pstRBufMng == NULL) return FALSE;

    /* 检查环形队列中接收的数据长度 */
    RBuf_ChkRxLen(pstRBufMng);

    /* 超时后返回检查帧头 */
    if(pstRBufMng->bNewFrmFlg == FALSE &&\
       pstRBufMng->ChkCnt == 0 &&\
       pstRBufMng->eChkSts != CHK_HEAD1)
    {
        pstRBufMng->eChkSts = CHK_HEAD1;
    }

    do
    {
        bChkContinueFlg = FALSE;
        switch(pstRBufMng->eChkSts)
        {
            case CHK_HEAD1:
            {
                pstRBufMng->HandledLen = 0;

                /* 从环形队列里寻找帧头1 */
                for(sIndex = 0; sIndex < pstRBufMng->RxLen; sIndex++)
                {   
                    /* 找到帧头后切到下一个状态找帧头2 */
                    if(RBuf_RdByte(pstRBufMng, pstRBufMng->HandledLen) == RX_FRAME_HEAD1)
                    {
                        bChkContinueFlg = TRUE;
                        pstRBufMng->eChkSts = CHK_HEAD2;
                        pstRBufMng->ChkCnt = CHK_TIMEOUT;
                        break;
                    }
                    else
                    {
                        pstRBufMng->HandledLen++;
                    }
                }
                /* 将在帧头1检查过程中已经被处理的数据进行清理 */
                RBuf_ClrHandled(pstRBufMng);
            }
            break;
            case CHK_HEAD2:
            {
                if(pstRBufMng->RxLen >= 2)
                {
                    /* 帧头2正确时，切下一个状态检查属性 */
                    if(RBuf_RdByte(pstRBufMng, 1) == RX_FRAME_HEAD2)
                    {
                        pstRBufMng->eChkSts = CHK_ATTR;
                    }
                    else
                    {
                        /* 帧头2不正确时，清理之前的帧头1，重新寻找帧头1 */
                        pstRBufMng->HandledLen = 1;
                        RBuf_ClrHandled(pstRBufMng);
                        pstRBufMng->eChkSts = CHK_HEAD1;                    
                    }
                    
                    bChkContinueFlg = TRUE;
                }
            }
            break;            
            case CHK_ATTR:
            {
                if(pstRBufMng->RxLen >= 3)
                {
                    /* 属性正确时，切下一个状态检查长度 */
                    if(RBuf_RdByte(pstRBufMng, 2) == RX_FRAME_ATTR)
                    {
                        pstRBufMng->eChkSts = CHK_LEN;
                    }
                    else
                    {
                        /* 属性不正确时，清理之前的帧头1，重新寻找帧头1 */
                        pstRBufMng->HandledLen = 1;
                        RBuf_ClrHandled(pstRBufMng);
                        pstRBufMng->eChkSts = CHK_HEAD1;                    
                    }
                    
                    bChkContinueFlg = TRUE;
                }
            }
            break;
            case CHK_LEN:
            {
                if(pstRBufMng->RxLen >= 4)
                {
                    /* 长度正确时，切下一个状态检查帧尾 */
                    FrmLen = RBuf_RdByte(pstRBufMng, 3);
                    if(FrmLen == RX_FRAME_BYTE_LEN)
                    {
                        pstRBufMng->eChkSts = CHK_TAIL;
                    }
                    else
                    {
                        /* 长度不正确时，清理之前的帧头1，重新寻找帧头1 */
                        pstRBufMng->HandledLen = 1;
                        RBuf_ClrHandled(pstRBufMng);
                        pstRBufMng->eChkSts = CHK_HEAD1;
                    }
                    
                    bChkContinueFlg = TRUE;
                }            
            }
            break;
            case CHK_TAIL:
            {
                if(pstRBufMng->RxLen >= FrmLen)
                {
                    /* 帧尾正确时，切下一个状态检查校验和 */
                    if(RBuf_RdByte(pstRBufMng, (FrmLen - 1)) == RX_FRAME_TAIL)
                    {
                        pstRBufMng->eChkSts = CHK_SUM;
                    }
                    else
                    {
                        /* 帧尾不正确时，清理之前的帧头1，重新寻找帧头1 */
                        pstRBufMng->HandledLen = 1;
                        RBuf_ClrHandled(pstRBufMng);
                        pstRBufMng->eChkSts = CHK_HEAD1;
                    }
                    
                    bChkContinueFlg = TRUE;
                }
            }
            break;
            case CHK_SUM:
            {   
                /* 按帧长度读取该帧到帧数据缓冲区 */
                if(RBuf_RdData(pstRBufMng, 4, (FrmLen - 6), RxFrmBuf))
                {
                    /* 计算校验和 */
                    Sum = 0;
                    for(sIndex = 0; sIndex < FrmLen - 6; sIndex++)
                    {
                        Sum += RxFrmBuf[sIndex];
                    }

                    /* 校验和正确后，置新消息帧标志为真，等待上层提取帧数据处理 */
                    Sum = Sum & 0xFF;
                    if(Sum == RBuf_RdByte(pstRBufMng, (FrmLen - 2)))
                    {
                        pstRBufMng->bNewFrmFlg = TRUE;
                        pstRBufMng->HandledLen = FrmLen;
                        pstRBufMng->eChkSts = CHK_WAIT;
                        break;
                    }
                    else
                    {
                        /* 校验和不正确时，清理之前的帧头1，重新寻找帧头1 */
                        pstRBufMng->HandledLen = 1;
                        RBuf_ClrHandled(pstRBufMng);
                        pstRBufMng->eChkSts = CHK_HEAD1;
                    }
                    bChkContinueFlg = TRUE;
                }
            }
            break;
            case CHK_WAIT:
            {
                /* 上层处理完接收帧数据后，清理之前处理的内容，重新寻找下一个接收帧 */
                if(pstRBufMng->bNewFrmFlg == FALSE)
                {
                    RBuf_ClrHandled(pstRBufMng);
                    pstRBufMng->ChkCnt = 0;
                    pstRBufMng->eChkSts = CHK_HEAD1;
                    bChkContinueFlg = TRUE;
                }
            }
            break;
            default:
            {
               pstRBufMng->eChkSts = CHK_WAIT;
               bChkContinueFlg = TRUE; 
            }
            break;
        }
    }while(bChkContinueFlg);

    if(pstRBufMng->ChkCnt > 0)
    {
        pstRBufMng->ChkCnt--;
    }

    return(pstRBufMng->bNewFrmFlg);
}



/**
 *******************************************************************************
 * @fn      RBuf_ChkRxLen
 * @brief   检查环形队列中接收的数据长度
 * @param   ST_RBUFMNG *pstRBufMng  指向环形队列管理结构体的指针
 * @return  None
 *******************************************************************************
 */
static void RBuf_ChkRxLen(ST_RBUFMNG *pstRBufMng)
{
    if(pstRBufMng == NULL) return;

    if(pstRBufMng->WrPos >= pstRBufMng->RdPos)
    {
        pstRBufMng->RxLen = pstRBufMng->WrPos - pstRBufMng->RdPos;
    }
    else
    {
        pstRBufMng->RxLen = pstRBufMng->WrPos + pstRBufMng->BufLen - pstRBufMng->RdPos;
    }
}

/**
 *******************************************************************************
 * @fn      RBuf_RdByte
 * @brief   从环形队列读取1个数据
 * @param   ST_RBUFMNG *pstRBufMng  指向环形队列管理结构体的指针
 *          Uint16 Offset           读取的偏移位置
 * @return  Uint16                  返回读取的数据
 *******************************************************************************
 */
static Uint16 RBuf_RdByte(ST_RBUFMNG *pstRBufMng, Uint16 Offset)
{
    Uint16 Index;
    
    if(pstRBufMng == NULL || pstRBufMng->RxLen - Offset < 1) return 0;

    /* 当读位置加上偏移地址超过总长时，减去总长从头部读取 */
    Index = pstRBufMng->RdPos + Offset;
    if(Index >= pstRBufMng->BufLen)
    {
        Index -= pstRBufMng->BufLen;
    }
    
    return pstRBufMng->pRxBuf[Index];
}

/**
 *******************************************************************************
 * @fn      RBuf_RdData
 * @brief   从环形队列读取数据
 * @param   ST_RBUFMNG *pstRBufMng  指向环形队列管理结构体的指针
 *          Uint16 Offset           读取的偏移位置
 *          Uint16 Len              读取的长度
 *          Uint16 *pData           指向存放读取数据缓冲区的指针
 * @return  TRUE: 读取成功, FALSE: 读取失败
 *******************************************************************************
 */
static bool RBuf_RdData(ST_RBUFMNG *pstRBufMng, Uint16 Offset, Uint16 Len, Uint16 *pData)
{
    Uint16 Index;
    Uint16 TmpLen;
    
    if(pstRBufMng == NULL || Len == 0 || pData == NULL)
    {
        return FALSE;
    }

    /* 当读取数量大于可读数量，返回错误 */
    if(Len > pstRBufMng->RxLen - Offset)
    {
        return FALSE;
    }

    /* 当读位置加上偏移地址超过总长时，减去总长从头部读取 */
    Index = pstRBufMng->RdPos + Offset;
    if(Index >= pstRBufMng->BufLen)
    {
        Index -= pstRBufMng->BufLen;
    }

    /* 判断当前读位置到队尾剩余长度，若小于读取数，则分两次读取 */
    TmpLen = pstRBufMng->BufLen - Index;
    if(TmpLen < Len)
    {
        MemCpy((pstRBufMng->pRxBuf + Index), pData, TmpLen);
        MemCpy(pstRBufMng->pRxBuf, (pData + TmpLen), (Len - TmpLen));
    }
    else
    {
        MemCpy((pstRBufMng->pRxBuf + Index), pData, Len);
    }
    return TRUE;
}

/**
 *******************************************************************************
 * @fn      RBuf_ClrHandled
 * @brief   清除环形队列中已处理的数据
 * @param   ST_RBUFMNG *pstRBufMng  指向环形队列管理结构体的指针
 * @return  None
 *******************************************************************************
 */
static void RBuf_ClrHandled(ST_RBUFMNG *pstRBufMng)
{
    if(pstRBufMng == NULL) return;

    /* 清除环形队列中已经处理的数据，函数内部调整读指针的位置 */
    RBuf_Clr(pstRBufMng, pstRBufMng->HandledLen);

    /* 环形队列数据长度减去已清除的长度 */
    pstRBufMng->RxLen -= pstRBufMng->HandledLen;
    
    /* 已处理的数据长度清零 */
    pstRBufMng->HandledLen = 0;
}

/**
 *******************************************************************************
 * @fn      RBuf_Clr
 * @brief   环形队列内清除数据
 * @param   ST_RBUFMNG *pstRBufMng  指向环形队列管理结构体的指针
 *          Uint16 ClrLen           需要清除的长度(16位为单位)
 * @return  None
 *******************************************************************************
 */
static void RBuf_Clr(ST_RBUFMNG *pstRBufMng, Uint16 ClrLen)
{
    if(pstRBufMng == NULL) return;

    /* 检查环形队列数据长度 */
    RBuf_ChkRxLen(pstRBufMng);

    /* 若清除的长度比数据长度大，则只清除数据长度内容 */
    if(ClrLen > pstRBufMng->RxLen)
    {
        ClrLen = pstRBufMng->RxLen;
    }

    /* 读位置往后移动实现清除 */
    pstRBufMng->RdPos += ClrLen;

    /* 读位置超过队列尾后返回头部 */
    if(pstRBufMng->RdPos >=  pstRBufMng->BufLen)
    {
        pstRBufMng->RdPos -= pstRBufMng->BufLen;
    }
}

/**
 *******************************************************************************
 * @fn      MemCpy
 * @brief   数据缓存区拷贝
 * @param   Uint16 *pSrcAddr    指向源数据缓冲区的指针
 *          Uint16 *pDstAddr    指向目标数据缓冲区的指针
 *          Uint16 Len          需要拷贝的长度(16位为单位)
 * @return  None
 *******************************************************************************
 */
void MemCpy(Uint16 *pSrcAddr, Uint16 *pDstAddr, Uint16 Len)
{    
    while(Len--)
    {
        *pDstAddr++ = *pSrcAddr++;
    }
}

/**
 *******************************************************************************
 * @fn      MemSet
 * @brief   数据缓存区设置
 * @param   Uint16 *pAddr   指向要设置数据缓冲区的指针
 *          Uint16 Val      设置值
 *          Uint16 Len      需要设置的长度(16位为单位)
 * @return  None
 *******************************************************************************
 */
void MemSet(Uint16 *pAddr, Uint16 Val, Uint16 Len)
{
    while(Len--)
    {
        *pAddr++ = Val;
    }
}

/**
 *******************************************************************************
 * @fn      Uart_Init
 * @brief   发送缓冲区、接收缓冲区及接收帧缓冲区上电初始化清零
 * @param   None
 * @return  None
 *******************************************************************************
 */
void Uart_Init(void)
{
	MemSet(UartRxBuf, 0, MAX_RX_LEN);
	MemSet(UartTxBuf, 0, MAX_TX_LEN);
	MemSet(RxFrmBuf, 0, RX_FRAME_MAX_LEN);
}

