/**
 ******************************************************************************
 * Copyright (C) 2020-2030, CAMIC All Rights Reserved
 * @file	MCPara_Eeprom.c
 * @author	caih
 * @version 1.0
 * @date	2020/03/14
 * @brief	EEPROM����
 * Revision history:
 * <Author> 			<Date>			<Version>		<Description>
 *	caih				2020/03/14			1.0			�״δ���
 ******************************************************************************
 */
#include "MCPara_Eeprom.h"
static Uint16 SPI_RdByte(void);
static void SPI_WrByte(Uint16 usData);
static inline void WaitClks(void);
static bool EEPROM_WaitStandby(void);
static bool EEPROM_WrBuf(Uint16 WrAddr, Uint16 *pWrBuf, Uint16 WrLen);

/**
 *******************************************************************************
 * @fn      EEPROM_PageWrBuf
 * @brief   ��EEPROM�ĵ�ַ����д�����ݣ����ݿɳ���1ҳ
 * @param   Uint16 WrAddr	EEPROM�ĵ�ַ(0-0xFFF), ��Ҫ����������ַ!
 			Uint16 *pWrBuf	ָ����Ҫд��EEPROM�����ݻ�����
 			Uint16 WrLen	Ҫд�����ݳ���(��16λ��Ϊ��λ)
 * @return  bool TRUE�ɹ�, FALSEʧ��
 *******************************************************************************
 */
bool EEPROM_PageWrBuf(Uint16 WrAddr, Uint16 *pWrBuf, Uint16 WrLen)
{
    Uint16 RdAddr = WrAddr;
    Uint16 *pRdBuf = pWrBuf;
    Uint16 RdLen = WrLen;
    Uint16 Len, PageOffset;
    Uint16 Index, RdData;
    
    if(pWrBuf == NULL || WrLen == 0)
    {
        return FALSE;
    }
    
    while(WrLen > 0)
    {
        /* ����ҳʣ���д�ֽ� */
        PageOffset = PAGE_SIZE - (WrAddr & (PAGE_SIZE - 1));

        /* ��Ҫд���ֽ���������д�ֽ���������дʣ���д�ֽ� */
        Len = ((WrLen << 1) > PageOffset) ? (PageOffset >> 1) : WrLen;
        EEPROM_WrBuf(WrAddr, pWrBuf, Len);

        /* ������ʣ����Ҫд�ģ���ȴ�EEPROM������д */
        WrLen -= Len;
        if(WrLen > 0)
        {
            pWrBuf += Len;
            WrAddr += (Len << 1);
        }
        if(EEPROM_WaitStandby() == FALSE)
        {
            return FALSE;
        }
    }
    
    /* ��д��������������У�� */
    for(Index = 0; Index < RdLen; Index++)
    {
        EEPROM_RdBuf(RdAddr, &RdData, 1);
        RdAddr += 2;
        if(RdData != pRdBuf[Index])
        {
            return FALSE;
        }
    }
    
    return TRUE;
}

/**
 *******************************************************************************
 * @fn      EEPROM_WaitStandby
 * @brief   ��ҳд�����У��ȴ���һҳ��д��
 * @param   None
 * @return  bool TRUE�ɹ�, FALSEʧ��
 *******************************************************************************
 */
static bool EEPROM_WaitStandby(void)
{
    Uint32 Cnt = 0xFFFF;
    Uint16 Status = 0xFF;
    
    while(Cnt--)
    {
        /* ��ѯ״̬�Ĵ��� */
        SPI_WrByte(EEPROM_RDSR_CMD);
        Status = SPI_RdByte();
        EEPROM_CS_HIGH();

        /* ���к󷵻� */
        if((Status & 0x01) == EEPROM_RDY)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/**
 *******************************************************************************
 * @fn      EEPROM_WrBuf
 * @brief   ��EEPROM�ĵ�ַ����д�����ݣ����������ܳ������ʵ�ַ�ڸ�ҳ��ʣ������
 * @param   Uint16 WrAddr	EEPROM�ĵ�ַ(0-0xFFF), ��Ҫ����������ַ!
 			Uint16 *pWrBuf	ָ����Ҫд��EEPROM�����ݻ�����
 			Uint16 WrLen	Ҫд�����ݳ���(��16λ��Ϊ��λ)
 * @return  bool TRUE�ɹ�, FALSEʧ��
 *******************************************************************************
 */
static bool EEPROM_WrBuf(Uint16 WrAddr, Uint16 *pWrBuf, Uint16 WrLen)
{
    /* ��ָ������ֽ�ʱ�˳� */
    if(pWrBuf == NULL || WrLen == 0)
    {
        return FALSE;
    }

    /* ����дʹ���������Ҫ����Ƭѡ */
	SPI_WrByte(EEPROM_WREN_CMD);
	EEPROM_CS_HIGH();

    /* ����д���� */
	SPI_WrByte(EEPROM_WRITE_CMD);

	/* ���͵�ַ */
	SPI_WrByte(WrAddr >> 8);
    SPI_WrByte(WrAddr);

	while(WrLen--)
	{   
	    /* ��д��8λ */
	    SPI_WrByte(*pWrBuf);
        /* ��д��8λ */
	    SPI_WrByte(((*pWrBuf++) >> 8));
	}

	/* д��������������Ƭѡ */
	EEPROM_CS_HIGH();
	return TRUE;
}

/**
 *******************************************************************************
 * @fn      EEPROM_RdBuf
 * @brief   ��EEPROM�ĵ�ַ������������
 * @param   Uint16 usAddr	EEPROM�ĵ�ַ(0-0xFFF), ��ַָ�����������Զ���, 
                            ����0xFFF���Զ�����,��Ҫ����������ַ!
 			Uint16 *pusBuf	ָ�򱣴�������ݵĻ�����
 			Uint16 usLen	Ҫ��ȡ�����ݳ���(��16λ��Ϊ��λ)
 * @return  bool TRUE�ɹ�, FALSEʧ��
 *******************************************************************************
 */
bool EEPROM_RdBuf(Uint16 RdAddr, Uint16 *pRdBuf, Uint16 RdLen)
{
    Uint16 usTmp;

    /* ��ָ������ֽ�ʱ�˳� */
    if(pRdBuf == NULL || RdLen == 0)
    {
        return FALSE;
    }
    
    /* ���Ͷ����� */
    SPI_WrByte(EEPROM_READ_CMD);

    
    /* ���Ͷ���ַ */
	SPI_WrByte(RdAddr >> 8);
    SPI_WrByte(RdAddr);
    
    while(RdLen--)
    {
        usTmp = SPI_RdByte();
        usTmp |= (SPI_RdByte() << 8);
        *pRdBuf++ = usTmp;
    }

    /* ������������Ƭѡ */
    EEPROM_CS_HIGH();
    return TRUE;
}


/**
 *******************************************************************************
 * @fn      SPI_WrByte
 * @brief   ģ��SPIʱ�����1�ֽ�����
 * @param   Uint16 usData	Ҫ���������
 * @return  ��
 *******************************************************************************
 */
static void SPI_WrByte(Uint16 usData)
{
    Uint16 usIdx;

    /* ƬѡEEPROM���� */
    EEPROM_CS_LOW();

    /* ֻ�����8λ */
    usData = usData & 0xFF;
    
	for(usIdx = 7; usIdx <= 7; usIdx--)
	{
	    /* MSB��λ�ȴ��� */
		EEPROM_MOSI = ((usData >> usIdx) & 0x01) ? 1 : 0;

		WaitClks();
		/* SCLK������ʱ��EEPROM����SI���� */
		EEPROM_SCK_HIGH();
		WaitClks();
		EEPROM_SCK_LOW();
	}
}

/**
 *******************************************************************************
 * @fn      SPI_RdByte
 * @brief   ģ��SPIʱ���ȡ1�ֽ�����
 * @param   ��
 * @return  Uint16 ��ȡ��1�ֽ�������8λΪ0
 *******************************************************************************
 */
static Uint16 SPI_RdByte(void)
{
	Uint16 usIdx;
    Uint16 usTmp = 0;

    /* ѡ������ */
    EEPROM_CS_LOW();
    
	for(usIdx = 0; usIdx < 8; usIdx++)
	{
	    EEPROM_SCK_HIGH();
	    WaitClks();
	    
	    /* ���������SCK�����ض�ȡ */
	    usTmp <<= 1;
	    usTmp |= EEPROM_MISO;
	    
	    EEPROM_SCK_LOW();
	    WaitClks();
	}

	return usTmp;
}

/**
 *******************************************************************************
 * @fn      WaitClks
 * @brief   ����ȴ�ʱ��
 * @param   ��
 * @return  ��
 *******************************************************************************
 */
static inline void WaitClks(void)
{
	__asm(" nop");
	__asm(" nop");
	__asm(" nop");
}

