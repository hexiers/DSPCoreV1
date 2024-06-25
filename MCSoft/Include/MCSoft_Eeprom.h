/**
 ******************************************************************************
 * Copyright (C) 2020-2030, CAMIC All Rights Reserved
 * @file	MCSoft_EEPROM.h
 * @author	caih
 * @version 1.0
 * @date	2020/03/14
 * @brief	EEPROM����
 * Revision history:
 * <Author> 			<Date>			<Version>		<Description>
 *	caih				2020/03/14			1.0			�״δ���
 ******************************************************************************
 */
#include "MC_Include.h"

#ifndef __MCSOFT_EEPROM_H
#define __MCSOFT_EEPROM_H



typedef enum
{
	EEPROM_WREN_CMD = 0x06,		/* дʹ�� */
	EEPROM_WRDI_CMD = 0x04,		/* д��ֹ */
	EEPROM_RDSR_CMD = 0x05,		/* ��״̬�Ĵ��� */
	EEPROM_WRSR_CMD = 0x01,		/* д״̬�Ĵ��� */
	EEPROM_READ_CMD = 0x03,		/* ������ */
	EEPROM_WRITE_CMD = 0x02	/* д���� */
}E_EEPROM_CMD;

typedef enum
{
    EEPROM_RDY = 0,
    EEPROM_BUSY
}E_EEPROM_STS;

#define PAGE_SIZE   (32)     /* AT25320ҳ��СΪ32�ֽ� */

#define EEPROM_MOSI	       HAL_EEPROM_MOSI_GPIODATA
#define EEPROM_MISO	       HAL_EEPROM_MISO_GPIODATA

#define EEPROM_CS_LOW()	    HAL_EEPROM_CS_LOW
#define EEPROM_CS_HIGH()	HAL_EEPROM_CS_HIGH

#define EEPROM_SCK_LOW()	HAL_EEPROM_CLK_LOW//(GpioDataRegs.GPCCLEAR.bit.GPIO59 = 1)
#define EEPROM_SCK_HIGH()	HAL_EEPROM_CLK_HIGH//(GpioDataRegs.GPCSET.bit.GPIO59 = 1)

extern bool EEPROM_PageWrBuf(Uint16 WrAddr, Uint16 *pWrBuf, Uint16 WrLen);
extern bool EEPROM_RdBuf(Uint16 RdAddr, Uint16 *pRdBuf, Uint16 RdLen);

#endif

