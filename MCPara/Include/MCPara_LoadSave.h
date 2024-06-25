/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore 配置参数结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#ifndef MCPARALOADSAVE_H_H
#define MCPARALOADSAVE_H_H

#include"MC_Include.h"

/* 存放固定码 */
#define EPRM_PARA_FIXCODE           (0x55AA)
/* 各类型参数在EEPROM中的存放位置 */
#define EPRM_MOTORPARA_ADDR         (0x000)
#define EPRM_MOTORPARA_BAK_ADDR     (0x100)
#define EPRM_CTRLPARA_ADDR          (0x200)
#define EPRM_CTRLPARA_BAK_ADDR      (0x300)
#define EPRM_FAULTRPARA_ADDR        (0x400)
#define EPRM_FAULTRPARA_BAK_ADDR    (0x500)
#define EPRM_WARNPARA_ADDR          (0x600)
#define EPRM_WARNPARA_BAK_ADDR      (0x700)
#define EPRM_SNSLESSPARA_ADDR       (0x800)
#define EPRM_SNSLESSPARA_BAK_ADDR   (0x900)
#define EPRM_HALPARA_ADDR           (0xA00)
#define EPRM_HALPARA_BAK_ADDR       (0xB00)
#define EPRM_SPDFDBPARA_ADDR        (0xC00)
#define EPRM_SPDFDBPARA_BAK_ADDR    (0xD00)
#define EPRM_SYSCFGPARA_ADDR        (0xE00)
#define EPRM_SYSCFGPARA_BAK_ADDR    (0xF00)

typedef enum
{
    UINT16_T = 0,
    INT16_T
}E_TYPE;

union INT16_TYPE
{
    Uint16 usVal;
    INT16S ssVal;
};

/* 单一参数管理结构体 */
typedef struct
{
    E_TYPE eType;
    union INT16_TYPE LowLmt;
    union INT16_TYPE UpLmt;
    union INT16_TYPE Default;
}ST_PARA_ATTR;

/* 类型参数管理结构体 */
typedef struct
{
    ST_PARA_ATTR *pstParaAttr;
    Uint16 *pSize;
    Uint16 ParaAddr;
    Uint16 ParaBakAddr;
}ST_EEPROM_PARA;

/* 更新参数管理结构体 */
typedef struct
{
    void *pPara;
    ST_PARA_ATTR *pstParaAttr;
    Uint16 *pSize;
}ST_UPDATE_PARA;

/* 各类型参数索引值 */
typedef enum
{
    MOTOR_PARA = 0,
    CTRL_PARA,
    FAULT_PARA,
    WARN_PARA,
    SNSLESS_PARA,
    HAL_PARA,
    SPDFDB_PARA,
    SYSCFG_PARA,
    EEPROM_PARA_NUM
}E_PARA_INDEX;

extern const ST_EEPROM_PARA stEEPROMParaArr[];
extern bool LoadPara(void *pPara, const ST_EEPROM_PARA *pstEPMPara, E_PARA_INDEX eParaIndex);
extern bool SavePara(void *pPara, const ST_EEPROM_PARA *pstEPMPara, E_PARA_INDEX eParaIndex);
extern bool LoadDefaultPara(Uint16 *pPara, const ST_EEPROM_PARA *pstEPMPara, E_PARA_INDEX eParaIndex);
extern bool UpdateRamPara(Uint16 Index, Uint16 SubIndex, Uint16 Val);

#endif
