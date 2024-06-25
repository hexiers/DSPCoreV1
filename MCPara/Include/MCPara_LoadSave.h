/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����MCCore ���ò����ṹ��
* @������		:  	����                                                                              
* ��������		:   	2020-03-03                                                                 
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#ifndef MCPARALOADSAVE_H_H
#define MCPARALOADSAVE_H_H

#include"MC_Include.h"

/* ��Ź̶��� */
#define EPRM_PARA_FIXCODE           (0x55AA)
/* �����Ͳ�����EEPROM�еĴ��λ�� */
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

/* ��һ��������ṹ�� */
typedef struct
{
    E_TYPE eType;
    union INT16_TYPE LowLmt;
    union INT16_TYPE UpLmt;
    union INT16_TYPE Default;
}ST_PARA_ATTR;

/* ���Ͳ�������ṹ�� */
typedef struct
{
    ST_PARA_ATTR *pstParaAttr;
    Uint16 *pSize;
    Uint16 ParaAddr;
    Uint16 ParaBakAddr;
}ST_EEPROM_PARA;

/* ���²�������ṹ�� */
typedef struct
{
    void *pPara;
    ST_PARA_ATTR *pstParaAttr;
    Uint16 *pSize;
}ST_UPDATE_PARA;

/* �����Ͳ�������ֵ */
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
