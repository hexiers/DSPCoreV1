/**
 ******************************************************************************
 * Copyright (C) 2020-2030, CAMIC All Rights Reserved
 * @file	MCSoft_Uart.c
 * @author	caih
 * @version 1.0
 * @date	2020/03/02
 * @brief	Uart����Э��
 * Revision history:
 * <Author> 			<Date>			<Version>		<Description>
 *	caih				2020/03/02			1.0			�״δ���
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

/* ���ڽ������ݻ����� */
Uint16 UartRxBuf[MAX_RX_LEN];
/* ���ڷ������ݻ����� */
Uint16 UartTxBuf[MAX_TX_LEN];
/* ����֡������ */
Uint16 RxFrmBuf[RX_FRAME_MAX_LEN];
/* ����֡���ݽṹ�� */
ST_RXFRM stRxFrm = {0, {0, 0, 0, 0, 0, 0}};
/* ���ζ��й���ṹ�� */
ST_RBUFMNG stRBufMng = {UartRxBuf, MAX_RX_LEN, (MAX_RX_LEN-1), 0, 0, CHK_HEAD1, 0, 0, 0, FALSE, RxFrmBuf, &stRxFrm};
/* ���Ͳ������ */
Uint16 TxStructIndex = 15;

Uint16 TxScopeCnt = 0;

/* ���Ͳ�������ṹ������ */
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
    {(void *)&m1CriPara, (Uint16 *)&m1CriParaSize},        //zyu: �ؼ������ṹ�岹��
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
 * @brief   ���ڽ��յײ㺯������FPGA�ж�ȡ���ڽ������ݣ����뻷�ζ�����
 * @param   None
 * @return  TRUE: �ɹ�, FALSE: ʧ�ܣ����ζ����������
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
			/* дָ�벻Ϊ���һ��λ�ã������ָ����1���ֽ����ϣ�����д */
			if((pstRBufMng->WrPos + 1) != pstRBufMng->RdPos)
			{
				pstRBufMng->pRxBuf[pstRBufMng->WrPos] = SciaRegs.SCIRXBUF.bit.RXDT;
				pstRBufMng->WrPos++;
				RxNum--;
			}
			else
			{
				/* ������� */
				bStatus = FALSE;
				break;
			}
		}
		else
		{
			/* ��дָ��Ϊ���һ��λ�ã�����ָ�벻Ϊ0λ��ʱ������д��д���дָ���0 */
			if(pstRBufMng->RdPos != 0)
			{
				pstRBufMng->pRxBuf[pstRBufMng->WrPos] = SciaRegs.SCIRXBUF.bit.RXDT;
				pstRBufMng->WrPos = 0;
				RxNum--;
			}
			else
			{
				/* ������� */
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
 * @brief   ���÷��Ͳ����ṹ���
 * @param   Uint16 Index    ���Ͳ����ṹ���
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
 * @brief   ��ȡ���Ͳ����ṹ���
 * @param   None
 * @return  Uint16 ���Ͳ����ṹ���
 *******************************************************************************
 */
Uint16 GetTxStructIndex(void)
{
    return TxStructIndex;
}


/**
 *******************************************************************************
 * @fn      Uart_TxFrm
 * @brief   ���ڷ���֡Э���������
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

	/* ֡ͷ */
	*pTxBuf++ = (TX_FRAME_HEAD2 << 8) | TX_FRAME_HEAD1;
	/* ֡���� ���� */
	*pTxBuf++ = (TX_FRAME_BYTE_LEN << 8) | TX_FRAME_ATTR;
	
	TxStrIndex = GetTxStructIndex();

    /* caih Add @20200528: ����ʾ�����۲���� */
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
        /* Ĭ�Ϸ��� */
        SetTxStructIndex(15);
    	/* ���Ͳ������������� */
    	*pTxBuf++ = TxStrIndex;
    	/* ����Ҫ���͵Ľṹ������ */
    	MemCpy((Uint16 *)(pstTxStruct[TxStrIndex].pTxStr), pTxBuf, *(pstTxStruct[TxStrIndex].pSize));
    	pTxBuf += *(pstTxStruct[TxStrIndex].pSize);
    	/* ʣ�µ��������� */
    	MemSet(pTxBuf, 0, (TX_FRAME_BYTE_LEN / 2 - 4 - *(pstTxStruct[TxStrIndex].pSize)));
    }
    
	/* ���µ���pTxBuf�����Լ����ۼӺ�*/
	pTxBuf = &UartTxBuf[2];
	for(i = 0; i < (TX_FRAME_BYTE_LEN / 2 - 3); i++)
	{
	    Sum = Sum + (*pTxBuf & 0xFF) + ((*pTxBuf >> 8) & 0xFF);
	    pTxBuf++;
	}

    /* ֡β��У��� */
	*pTxBuf = ((Uint16)TX_FRAME_TAIL << 8) | (Sum & 0xFF);

    /* ���÷��͵ײ㺯��ִ�з��� */
	pTxBuf = &UartTxBuf[0];
	Uart_Tx(pTxBuf, TX_FRAME_BYTE_LEN/2);
}

/**
 *******************************************************************************
 * @fn      Uart_Tx
 * @brief   ���ڷ��͵ײ㺯����ͨ��FPGA����
 * @param   Uint16 *pTxData     ָ�������ݻ�������ָ��
 *          Uint16 Num          �������ݳ���(16λΪ��λ)
 * @return  None
 *******************************************************************************
 */

Uint16 TxBuf[TX_FRAME_BYTE_LEN];
Uint16 TxBufLen = 0;
static void Uart_Tx(Uint16 *pTxData, Uint16 Num)
{
    Uint16 i = 0;
	/* �ɵ��ó���֤֮ǰ�����ݿ��Է���*/
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
 * @brief   ���ڽ���֡�������ݲ�ͬ�������ֽ�����Ӧ����
 * @param   ST_RBUFMNG *pstRBufMng ָ���ζ��й���ṹ��ָ��
 * @return  None
 *******************************************************************************
 */
void Uart_RxFrm(void)
{
    stMCMessage stPostMsg;
    ST_RBUFMNG *pstRBufMng = &stRBufMng;
    
    /* �������µĽ���֡ʱ */
    if(Uart_ChkNewFrm(pstRBufMng) == TRUE)
    {
        /* ��������ƴ��ת�� */
        Uart_ConvToFrm(pstRBufMng->pRxFrmBuf, (Uint16 *)(pstRBufMng->pstRxFrm), 7);

        /* �ж��������Ƿ�Ϸ� */
        if(CmdSTARTCURRLOOP <= pstRBufMng->pstRxFrm->CmdWord && pstRBufMng->pstRxFrm->CmdWord <= CmdDoPOSOFFCALIB2)// V1.10.2
        {
            /* ȡ���������� */
            stPostMsg.msg = (Message_t)pstRBufMng->pstRxFrm->CmdWord;
            stPostMsg.msgPara1 = pstRBufMng->pstRxFrm->Data[0];
            stPostMsg.msgPara2 = pstRBufMng->pstRxFrm->Data[1];
            stPostMsg.msgPara3 = pstRBufMng->pstRxFrm->Data[2];
            stPostMsg.msgPara4 = pstRBufMng->pstRxFrm->Data[3];
            stPostMsg.Priority = 0;

            /* zyu Modify @20200319*/ 
            /* ��������ָ������ݸ���ָ�ת������Ϣ������ֱ�ӻط�����*/
            if(pstRBufMng->pstRxFrm->CmdWord == CmdREQSTRUCTDATA)
            {
                /* ���÷������������� */
                SetTxStructIndex(stPostMsg.msgPara1);
            }
            else if(pstRBufMng->pstRxFrm->CmdWord == CmdREQSINGLEDATA)
            {
                /* ����ʾ�����۲���� */
                Scope_Config(stPostMsg.msgPara1, stPostMsg.msgPara2, stPostMsg.msgPara3);
            }           
            else if(pstRBufMng->pstRxFrm->CmdWord == CmdSetMonitor1)
            {
                /* ���ü���1�۲���� */
                Monitor1_Config(stPostMsg.msgPara1, stPostMsg.msgPara2);
            }           
            else if(pstRBufMng->pstRxFrm->CmdWord == CmdSetMonitor2)
            {
                /* ���ü���2�۲���� */
                Monitor2_Config(stPostMsg.msgPara1, stPostMsg.msgPara2);
            }           
            else if(pstRBufMng->pstRxFrm->CmdWord == CmdUPDATEPARA) 
            {
                /* ����RAM�еĲ��� */
                if(UpdateRamPara(stPostMsg.msgPara1, stPostMsg.msgPara2, stPostMsg.msgPara3) == FALSE)
                {
                    MC_NotifyCmdDisCard(CmdUPDATEPARA);
                }
                else
                {
                    /* ���²����ɹ� */
                	m1CriPara.MsgBox.bit.MsgBoxUpdateParaSucc = 1;
                }
            }
	        else
	        {
	        	/* ������Ϣ */
           	    PostMessge(&stPostMsg);
            }

        }
        pstRBufMng->bNewFrmFlg = FALSE;
    }
}


/**
 *******************************************************************************
 * @fn      Uart_ConvToFrm
 * @brief   �����յ�����ת��Ϊ֡���ݣ�Ϊ�˽��DSP��С���ݽṹΪ16λ������
 * @param   Uint16 *pSrcAddr Դ����ָ��
 *          Uint16 *pDstAddr Ŀ������ָ��
 *          Uint16 Len       ֡���ݸ���
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
 * @brief   �����ջ��ζ����Ƿ����µĽ���֡����
 * @param   ST_RBUFMNG *pstRBufMng ָ���ζ��й���ṹ��ָ��
 * @return  TRUE: ���յ��µ�����֡, FALSE: û���µ�����֡
 *******************************************************************************
 */
static bool Uart_ChkNewFrm(ST_RBUFMNG *pstRBufMng)
{
    bool bChkContinueFlg;
    INT16S sIndex;
    Uint16 FrmLen;
    Uint16 Sum;
    
    if(pstRBufMng == NULL) return FALSE;

    /* ��黷�ζ����н��յ����ݳ��� */
    RBuf_ChkRxLen(pstRBufMng);

    /* ��ʱ�󷵻ؼ��֡ͷ */
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

                /* �ӻ��ζ�����Ѱ��֡ͷ1 */
                for(sIndex = 0; sIndex < pstRBufMng->RxLen; sIndex++)
                {   
                    /* �ҵ�֡ͷ���е���һ��״̬��֡ͷ2 */
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
                /* ����֡ͷ1���������Ѿ�����������ݽ������� */
                RBuf_ClrHandled(pstRBufMng);
            }
            break;
            case CHK_HEAD2:
            {
                if(pstRBufMng->RxLen >= 2)
                {
                    /* ֡ͷ2��ȷʱ������һ��״̬������� */
                    if(RBuf_RdByte(pstRBufMng, 1) == RX_FRAME_HEAD2)
                    {
                        pstRBufMng->eChkSts = CHK_ATTR;
                    }
                    else
                    {
                        /* ֡ͷ2����ȷʱ������֮ǰ��֡ͷ1������Ѱ��֡ͷ1 */
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
                    /* ������ȷʱ������һ��״̬��鳤�� */
                    if(RBuf_RdByte(pstRBufMng, 2) == RX_FRAME_ATTR)
                    {
                        pstRBufMng->eChkSts = CHK_LEN;
                    }
                    else
                    {
                        /* ���Բ���ȷʱ������֮ǰ��֡ͷ1������Ѱ��֡ͷ1 */
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
                    /* ������ȷʱ������һ��״̬���֡β */
                    FrmLen = RBuf_RdByte(pstRBufMng, 3);
                    if(FrmLen == RX_FRAME_BYTE_LEN)
                    {
                        pstRBufMng->eChkSts = CHK_TAIL;
                    }
                    else
                    {
                        /* ���Ȳ���ȷʱ������֮ǰ��֡ͷ1������Ѱ��֡ͷ1 */
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
                    /* ֡β��ȷʱ������һ��״̬���У��� */
                    if(RBuf_RdByte(pstRBufMng, (FrmLen - 1)) == RX_FRAME_TAIL)
                    {
                        pstRBufMng->eChkSts = CHK_SUM;
                    }
                    else
                    {
                        /* ֡β����ȷʱ������֮ǰ��֡ͷ1������Ѱ��֡ͷ1 */
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
                /* ��֡���ȶ�ȡ��֡��֡���ݻ����� */
                if(RBuf_RdData(pstRBufMng, 4, (FrmLen - 6), RxFrmBuf))
                {
                    /* ����У��� */
                    Sum = 0;
                    for(sIndex = 0; sIndex < FrmLen - 6; sIndex++)
                    {
                        Sum += RxFrmBuf[sIndex];
                    }

                    /* У�����ȷ��������Ϣ֡��־Ϊ�棬�ȴ��ϲ���ȡ֡���ݴ��� */
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
                        /* У��Ͳ���ȷʱ������֮ǰ��֡ͷ1������Ѱ��֡ͷ1 */
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
                /* �ϲ㴦�������֡���ݺ�����֮ǰ��������ݣ�����Ѱ����һ������֡ */
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
 * @brief   ��黷�ζ����н��յ����ݳ���
 * @param   ST_RBUFMNG *pstRBufMng  ָ���ζ��й���ṹ���ָ��
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
 * @brief   �ӻ��ζ��ж�ȡ1������
 * @param   ST_RBUFMNG *pstRBufMng  ָ���ζ��й���ṹ���ָ��
 *          Uint16 Offset           ��ȡ��ƫ��λ��
 * @return  Uint16                  ���ض�ȡ������
 *******************************************************************************
 */
static Uint16 RBuf_RdByte(ST_RBUFMNG *pstRBufMng, Uint16 Offset)
{
    Uint16 Index;
    
    if(pstRBufMng == NULL || pstRBufMng->RxLen - Offset < 1) return 0;

    /* ����λ�ü���ƫ�Ƶ�ַ�����ܳ�ʱ����ȥ�ܳ���ͷ����ȡ */
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
 * @brief   �ӻ��ζ��ж�ȡ����
 * @param   ST_RBUFMNG *pstRBufMng  ָ���ζ��й���ṹ���ָ��
 *          Uint16 Offset           ��ȡ��ƫ��λ��
 *          Uint16 Len              ��ȡ�ĳ���
 *          Uint16 *pData           ָ���Ŷ�ȡ���ݻ�������ָ��
 * @return  TRUE: ��ȡ�ɹ�, FALSE: ��ȡʧ��
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

    /* ����ȡ�������ڿɶ����������ش��� */
    if(Len > pstRBufMng->RxLen - Offset)
    {
        return FALSE;
    }

    /* ����λ�ü���ƫ�Ƶ�ַ�����ܳ�ʱ����ȥ�ܳ���ͷ����ȡ */
    Index = pstRBufMng->RdPos + Offset;
    if(Index >= pstRBufMng->BufLen)
    {
        Index -= pstRBufMng->BufLen;
    }

    /* �жϵ�ǰ��λ�õ���βʣ�೤�ȣ���С�ڶ�ȡ����������ζ�ȡ */
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
 * @brief   ������ζ������Ѵ��������
 * @param   ST_RBUFMNG *pstRBufMng  ָ���ζ��й���ṹ���ָ��
 * @return  None
 *******************************************************************************
 */
static void RBuf_ClrHandled(ST_RBUFMNG *pstRBufMng)
{
    if(pstRBufMng == NULL) return;

    /* ������ζ������Ѿ���������ݣ������ڲ�������ָ���λ�� */
    RBuf_Clr(pstRBufMng, pstRBufMng->HandledLen);

    /* ���ζ������ݳ��ȼ�ȥ������ĳ��� */
    pstRBufMng->RxLen -= pstRBufMng->HandledLen;
    
    /* �Ѵ�������ݳ������� */
    pstRBufMng->HandledLen = 0;
}

/**
 *******************************************************************************
 * @fn      RBuf_Clr
 * @brief   ���ζ������������
 * @param   ST_RBUFMNG *pstRBufMng  ָ���ζ��й���ṹ���ָ��
 *          Uint16 ClrLen           ��Ҫ����ĳ���(16λΪ��λ)
 * @return  None
 *******************************************************************************
 */
static void RBuf_Clr(ST_RBUFMNG *pstRBufMng, Uint16 ClrLen)
{
    if(pstRBufMng == NULL) return;

    /* ��黷�ζ������ݳ��� */
    RBuf_ChkRxLen(pstRBufMng);

    /* ������ĳ��ȱ����ݳ��ȴ���ֻ������ݳ������� */
    if(ClrLen > pstRBufMng->RxLen)
    {
        ClrLen = pstRBufMng->RxLen;
    }

    /* ��λ�������ƶ�ʵ����� */
    pstRBufMng->RdPos += ClrLen;

    /* ��λ�ó�������β�󷵻�ͷ�� */
    if(pstRBufMng->RdPos >=  pstRBufMng->BufLen)
    {
        pstRBufMng->RdPos -= pstRBufMng->BufLen;
    }
}

/**
 *******************************************************************************
 * @fn      MemCpy
 * @brief   ���ݻ���������
 * @param   Uint16 *pSrcAddr    ָ��Դ���ݻ�������ָ��
 *          Uint16 *pDstAddr    ָ��Ŀ�����ݻ�������ָ��
 *          Uint16 Len          ��Ҫ�����ĳ���(16λΪ��λ)
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
 * @brief   ���ݻ���������
 * @param   Uint16 *pAddr   ָ��Ҫ�������ݻ�������ָ��
 *          Uint16 Val      ����ֵ
 *          Uint16 Len      ��Ҫ���õĳ���(16λΪ��λ)
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
 * @brief   ���ͻ����������ջ�����������֡�������ϵ��ʼ������
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

