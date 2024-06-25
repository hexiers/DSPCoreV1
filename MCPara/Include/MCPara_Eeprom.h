/**
 ******************************************************************************
 * Copyright (C) 2020-2030, CAMIC All Rights Reserved
 * @file	MCPara_EEPROM.h
 * @author	caih
 * @version 1.0
 * @date	2020/03/14
 * @brief	EEPROM驱动
 * Revision history:
 * <Author> 			<Date>			<Version>		<Description>
 *	caih				2020/03/14			1.0			首次创建
 ******************************************************************************
 */
 
#ifndef __MCSOFT_EEPROM_H
#define __MCSOFT_EEPROM_H

#include "MC_Include.h"

typedef enum
{
	EEPROM_WREN_CMD = 0x06,		/* 写使能 */
	EEPROM_WRDI_CMD = 0x04,		/* 写禁止 */
	EEPROM_RDSR_CMD = 0x05,		/* 读状态寄存器 */
	EEPROM_WRSR_CMD = 0x01,		/* 写状态寄存器 */
	EEPROM_READ_CMD = 0x03,		/* 读操作 */
	EEPROM_WRITE_CMD = 0x02	    /* 写操作 */
}E_EEPROM_CMD;

typedef enum
{
    EEPROM_RDY = 0,             /* 空闲状态 */
    EEPROM_BUSY                 /* 忙状态 */
}E_EEPROM_STS;

#define PAGE_SIZE   (32)     /* AT25320页大小为32字节 */

/* IO模拟SPI管脚定义及操作 */
#define EEPROM_MOSI	       HAL_EEPROM_MOSI_GPIODATA
#define EEPROM_MISO	       HAL_EEPROM_MISO_GPIODATA

#define EEPROM_CS_LOW()	    HAL_EEPROM_CS_LOW
#define EEPROM_CS_HIGH()	HAL_EEPROM_CS_HIGH

#define EEPROM_SCK_LOW()	HAL_EEPROM_CLK_LOW
#define EEPROM_SCK_HIGH()	HAL_EEPROM_CLK_HIGH

extern bool EEPROM_PageWrBuf(Uint16 WrAddr, Uint16 *pWrBuf, Uint16 WrLen);
extern bool EEPROM_RdBuf(Uint16 RdAddr, Uint16 *pRdBuf, Uint16 RdLen);

#endif

