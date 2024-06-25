/**
 ******************************************************************************
 * Copyright (C) 2020-2030, CAMIC All Rights Reserved
 * @file	MCPara_Eeprom.c
 * @author	caih
 * @version 1.0
 * @date	2020/03/14
 * @brief	EEPROM驱动
 * Revision history:
 * <Author> 			<Date>			<Version>		<Description>
 *	caih				2020/03/14			1.0			首次创建
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
 * @brief   向EEPROM的地址连续写入数据，数据可超过1页
 * @param   Uint16 WrAddr	EEPROM的地址(0-0xFFF), 不要访问奇数地址!
 			Uint16 *pWrBuf	指向需要写入EEPROM的数据缓冲区
 			Uint16 WrLen	要写的数据长度(以16位字为单位)
 * @return  bool TRUE成功, FALSE失败
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
        /* 检查该页剩余可写字节 */
        PageOffset = PAGE_SIZE - (WrAddr & (PAGE_SIZE - 1));

        /* 若要写的字节数超过可写字节数，则先写剩余可写字节 */
        Len = ((WrLen << 1) > PageOffset) ? (PageOffset >> 1) : WrLen;
        EEPROM_WrBuf(WrAddr, pWrBuf, Len);

        /* 若还有剩余需要写的，则等待EEPROM空闲再写 */
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
    
    /* 对写入的数据逐个读出校验 */
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
 * @brief   在页写过程中，等待上一页的写完
 * @param   None
 * @return  bool TRUE成功, FALSE失败
 *******************************************************************************
 */
static bool EEPROM_WaitStandby(void)
{
    Uint32 Cnt = 0xFFFF;
    Uint16 Status = 0xFF;
    
    while(Cnt--)
    {
        /* 查询状态寄存器 */
        SPI_WrByte(EEPROM_RDSR_CMD);
        Status = SPI_RdByte();
        EEPROM_CS_HIGH();

        /* 空闲后返回 */
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
 * @brief   向EEPROM的地址连续写入数据，数据量不能超过访问地址在该页的剩余数据
 * @param   Uint16 WrAddr	EEPROM的地址(0-0xFFF), 不要访问奇数地址!
 			Uint16 *pWrBuf	指向需要写入EEPROM的数据缓冲区
 			Uint16 WrLen	要写的数据长度(以16位字为单位)
 * @return  bool TRUE成功, FALSE失败
 *******************************************************************************
 */
static bool EEPROM_WrBuf(Uint16 WrAddr, Uint16 *pWrBuf, Uint16 WrLen)
{
    /* 空指针和零字节时退出 */
    if(pWrBuf == NULL || WrLen == 0)
    {
        return FALSE;
    }

    /* 发送写使能命令后，需要拉高片选 */
	SPI_WrByte(EEPROM_WREN_CMD);
	EEPROM_CS_HIGH();

    /* 发送写命令 */
	SPI_WrByte(EEPROM_WRITE_CMD);

	/* 发送地址 */
	SPI_WrByte(WrAddr >> 8);
    SPI_WrByte(WrAddr);

	while(WrLen--)
	{   
	    /* 先写低8位 */
	    SPI_WrByte(*pWrBuf);
        /* 后写高8位 */
	    SPI_WrByte(((*pWrBuf++) >> 8));
	}

	/* 写操作结束后拉高片选 */
	EEPROM_CS_HIGH();
	return TRUE;
}

/**
 *******************************************************************************
 * @fn      EEPROM_RdBuf
 * @brief   从EEPROM的地址连续读出数据
 * @param   Uint16 usAddr	EEPROM的地址(0-0xFFF), 地址指针在器件内自动加, 
                            超过0xFFF后自动归零,不要访问奇数地址!
 			Uint16 *pusBuf	指向保存读出数据的缓冲区
 			Uint16 usLen	要读取的数据长度(以16位字为单位)
 * @return  bool TRUE成功, FALSE失败
 *******************************************************************************
 */
bool EEPROM_RdBuf(Uint16 RdAddr, Uint16 *pRdBuf, Uint16 RdLen)
{
    Uint16 usTmp;

    /* 空指针和零字节时退出 */
    if(pRdBuf == NULL || RdLen == 0)
    {
        return FALSE;
    }
    
    /* 发送读命令 */
    SPI_WrByte(EEPROM_READ_CMD);

    
    /* 发送读地址 */
	SPI_WrByte(RdAddr >> 8);
    SPI_WrByte(RdAddr);
    
    while(RdLen--)
    {
        usTmp = SPI_RdByte();
        usTmp |= (SPI_RdByte() << 8);
        *pRdBuf++ = usTmp;
    }

    /* 读结束后拉高片选 */
    EEPROM_CS_HIGH();
    return TRUE;
}


/**
 *******************************************************************************
 * @fn      SPI_WrByte
 * @brief   模拟SPI时序输出1字节数据
 * @param   Uint16 usData	要输出的数据
 * @return  无
 *******************************************************************************
 */
static void SPI_WrByte(Uint16 usData)
{
    Uint16 usIdx;

    /* 片选EEPROM器件 */
    EEPROM_CS_LOW();

    /* 只传输低8位 */
    usData = usData & 0xFF;
    
	for(usIdx = 7; usIdx <= 7; usIdx--)
	{
	    /* MSB高位先传输 */
		EEPROM_MOSI = ((usData >> usIdx) & 0x01) ? 1 : 0;

		WaitClks();
		/* SCLK上升沿时，EEPROM锁存SI数据 */
		EEPROM_SCK_HIGH();
		WaitClks();
		EEPROM_SCK_LOW();
	}
}

/**
 *******************************************************************************
 * @fn      SPI_RdByte
 * @brief   模拟SPI时序读取1字节数据
 * @param   无
 * @return  Uint16 读取的1字节数，高8位为0
 *******************************************************************************
 */
static Uint16 SPI_RdByte(void)
{
	Uint16 usIdx;
    Uint16 usTmp = 0;

    /* 选中器件 */
    EEPROM_CS_LOW();
    
	for(usIdx = 0; usIdx < 8; usIdx++)
	{
	    EEPROM_SCK_HIGH();
	    WaitClks();
	    
	    /* 输出数据在SCK上升沿读取 */
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
 * @brief   插入等待时间
 * @param   无
 * @return  无
 *******************************************************************************
 */
static inline void WaitClks(void)
{
	__asm(" nop");
	__asm(" nop");
	__asm(" nop");
}

