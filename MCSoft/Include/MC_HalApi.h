/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         
======================================================================================================*/

#include "MC_Include.h"
#ifndef MC_HALAPI_H_H
#define MC_HALAPI_H_H
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#define GetADResult(x)       ((*(Uint16*)(0x00007108+(x)))>>4)
#define  GetADResultModifyDef(x,ADGain,ADOff)    ((((INT32S)GetADResult(x)* (INT32S)(ADGain)) >> 14) +  (ADOff))

/* 用户修改*/
#define HAL_EEPROM_MOSI_GPIODATA  	GpioDataRegs.GPBDAT.bit.GPIO58
#define HAL_EEPROM_MISO_GPIODATA 	GpioDataRegs.GPBDAT.bit.GPIO60
#define HAL_EEPROM_CS_LOW 			GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1
#define HAL_EEPROM_CS_HIGH			GpioDataRegs.GPBSET.bit.GPIO61 = 1
#define HAL_EEPROM_CLK_LOW		GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1
#define HAL_EEPROM_CLK_HIGH		GpioDataRegs.GPBSET.bit.GPIO59 = 1
//===========================================================================
#define AT25320_CS GpioDataRegs.GPBDAT.bit.GPIO61
#define AT25320_OUT GpioDataRegs.GPBDAT.bit.GPIO60
#define AT25320_CLK GpioDataRegs.GPBDAT.bit.GPIO59
#define AT25320_IN GpioDataRegs.GPBDAT.bit.GPIO58
#define ANGLE_CS GpioDataRegs.GPBDAT.bit.GPIO57
#define TEST_T44 GpioDataRegs.GPBDAT.bit.GPIO40
#define TEST_T42 GpioDataRegs.GPBDAT.bit.GPIO37
#define TEST_T43 GpioDataRegs.GPBDAT.bit.GPIO34
#define A_SAMPLE GpioDataRegs.GPBDAT.bit.GPIO33
#define BUZZER GpioDataRegs.GPBDAT.bit.GPIO32
//#define CAN_STB GpioDataRegs.GPADAT.bit.GPIO29 // EPS_V4状态
#define CAN_STB GpioDataRegs.GPBDAT.bit.GPIO48
#define TEST_T41 GpioDataRegs.GPADAT.bit.GPIO22
#define TEST_T40 GpioDataRegs.GPADAT.bit.GPIO19
#define TEST_T39 GpioDataRegs.GPADAT.bit.GPIO18
#define CANID0 GpioDataRegs.GPADAT.bit.GPIO17
#define CANID1 GpioDataRegs.GPADAT.bit.GPIO16

#define TZ_OC GpioDataRegs.GPADAT.bit.GPIO12
#define TZ_SC GpioDataRegs.GPADAT.bit.GPIO13

#define TEST_T37 GpioDataRegs.GPADAT.bit.GPIO14
#define TEST_T38 GpioDataRegs.GPADAT.bit.GPIO15

#define STOPMOTOR GpioDataRegs.GPADAT.bit.GPIO11
#define LED_ERR GpioDataRegs.GPADAT.bit.GPIO10
#define LED_RUN GpioDataRegs.GPADAT.bit.GPIO9
#define DA_CS GpioDataRegs.GPADAT.bit.GPIO8
#define RELAY_OPEN GpioDataRegs.GPADAT.bit.GPIO7
#define FAN_DIS GpioDataRegs.GPADAT.bit.GPIO6


#define HA GpioDataRegs.GPADAT.bit.GPIO24
#define HB GpioDataRegs.GPADAT.bit.GPIO25
#define HC GpioDataRegs.GPADAT.bit.GPIO26

#define CANID2 GpioDataRegs.GPBDAT.bit.GPIO43
#define CANID3 GpioDataRegs.GPBDAT.bit.GPIO44
#define CANID4 GpioDataRegs.GPBDAT.bit.GPIO45
#define CANID5 GpioDataRegs.GPBDAT.bit.GPIO46
extern Uint16 HAL_GetLpVol(void);
extern Uint16 HAL_GetHpCurr(void);
extern Uint16 HAL_GetPddTemp(void);
extern Uint16 HAL_GetMotorTemp(void);
extern bool HAL_IsFpgaReady(void);
extern void HAL_ReloadFpga(void);
extern void HAL_LedToggle(void);
extern void HAL_SetCpuValid(bool st);
extern void HAL_SetInnerLoop(bool st);
extern bool HAL_IsGateDriveReady(void);
extern bool HAL_IsGateDriveFault(void);
extern void HAL_RstGateDriver(bool st);
extern void HAL_SetPreChargeRelayClose(bool sts);
extern void HAL_SetFlashHighAddr(Uint16 Haddr);
extern void HAL_RstFlash(void);
extern bool HAL_IsFlashReady(void);
extern Uint16 HAL_GetHallSector(void);
extern void HAL_RstAd2s1210(void);
extern bool HAL_IsPowerGood(void);


#endif
