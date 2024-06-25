/*======================================================================================================
* @????	:                                                       
* @????	: 	MCCore ???????
* @???		:  	??                                                                              
* ????		:   	2020-03-03                                                                 
* ????		:   
			2020-10-10:在控制参数中增加弱磁参数
*                         
======================================================================================================*/


#include"MCPara_LoadSave.h"
Uint16 CalChkSum(Uint16 *pBuf, Uint16 Len);

/* ??????????? */
stMotorPara m1MotorPara;
stCtrlPara m1CtrlPara;
stFaultPara m1FaultPara;
stWarnPara m1WarnPara;
stSnsLessPara m1SnsLessPara;
stHalPara m1HalPara;
stSpdFdbPara m1SpdFdbPara;
stSysCfgPara m1SysCfgPara;
extern 	const Uint16 m1MotorParaSize;
extern 	const Uint16 m1CtrlParaSize;
extern 	const Uint16 m1FaultParaSize;
extern 	const Uint16 m1WarnParaSize;
extern 	const Uint16 m1SnsLessParaSize;
extern 	const Uint16 m1HalParaSize;
extern 	const Uint16 m1SpdFdbParaSize;
extern 	const Uint16 m1SysCfgParaSize;
extern 	const Uint16 m1StartUpSize;
extern 	const Uint16 m1CurrLoopCtlerSize;
extern 	const Uint16 m1SpdLoopCtlerSize;
extern 	const Uint16 m1BusVolDecouplerSize;
extern 	const Uint16 m1SmcObserverSize;
extern 	const Uint16 m1ResolverDealModuleSize;
extern 	const Uint16 m1FaultDealModuleSize;
extern 	const Uint16 m1CriParaSize;

stMotorCalib m1MotorCalib;


const ST_PARA_ATTR stMotorParaAttr[]= //m1MotorParaSize] =
{
	{INT16_T, 0, 32767, 855},   // ??? 85.5mohm 5kW???
	{INT16_T, 0, 32767, 180},   // ??? 180uH 5kW???
	{INT16_T, 1, 64, 2},        // ???
	{INT16_T, 1, 64, 2},         // ???
	{INT16_T, 0, 32767, 3000},  // ??Q?? 5kW???
	{INT16_T, 0, 32767, 1000},  // ??D?? ??????
	{UINT16_T, 0, 65535, 0},    // ???
	{INT16_T, 0, 32767, 10000}  // ????
};

const ST_PARA_ATTR stCtrlParaAttr[]=//m1CtrlParaSize] =
{
	{INT16_T, 0, 16383, 100},
	{INT16_T, 0, 16383, 10},
	{INT16_T, 0, 16383, 100},
	{INT16_T, 0, 16383, 10},
	{INT16_T, 0, 16383, 100},  
	{INT16_T, 0, 1000, 100},
	{INT16_T, 0, 1000, 300},
	{INT16_T, 0, 32767, 1},//积分
	{INT16_T, 0, 16383, 1},
	{INT16_T, 0, 1000, 10},//???????
	{INT16_T, 0, 1000, 100},//???????
	{UINT16_T, 0, 65535, 27000},//?????????
	{UINT16_T, 750, 6000, 3000}, /* PWM???????,750??40KHz,3000??10kHz,6000??5kHz*/
	{UINT16_T, 0, 1023, 120}, 
	{UINT16_T, 0, 6, 4}, 
	{UINT16_T, 0, 1000, 15}, 
	{UINT16_T, 0, 1000, 10}, 
	{UINT16_T, 1, 1000, 2},/* 速度指令斜坡*/
	{UINT16_T, 1, 1000, 2},/* FwKp*/
	{UINT16_T, 1, 1000, 1},/* FwKi*/

};

const ST_PARA_ATTR stFaultParaAttr[]=//m1FaultParaSize] =
{
	{UINT16_T, 0, 65535, 32000},
	{UINT16_T, 0, 65535, 22000},
	{UINT16_T, 0, 65535, 4000},
	{UINT16_T, 0, 65535, 1800},
	{UINT16_T, 0, 32767, 1000},
	{UINT16_T, 0, 32767, 9000},
	{UINT16_T, 0, 32767, 1000},
	{UINT16_T, 0, 32767, 13000},
	{UINT16_T, 0, 32767, 800},
	{UINT16_T, 0, 32767, 9000}   
};

const ST_PARA_ATTR stWarnParaAttr[]=//m1WarnParaSize] =
{
	{UINT16_T, 0, 65535, 30000},
	{UINT16_T, 0, 65535, 25000},
	{UINT16_T, 0, 65535, 3500},
	{UINT16_T, 0, 65535, 2000},
	{UINT16_T, 0, 32767, 800},
	{UINT16_T, 0, 32767, 8000},
	{UINT16_T, 0, 32767, 800},
	{UINT16_T, 0, 32767, 10000},
	{UINT16_T, 0, 32767, 1000},
	{UINT16_T, 0, 32767, 7000}    
};

const ST_PARA_ATTR stSnsLessParaAttr[]=//m1SnsLessParaSize] =
{
	{UINT16_T, 0, 32767, 1500},
	{UINT16_T, 0, 32767, 1000},
	{UINT16_T, 0, 32767, 2000},
	{UINT16_T, 0, 256, 32},
	{UINT16_T, 0, 256, 10},
	{UINT16_T, 0, 32767, 200},   
	{UINT16_T, 0, 32767, 800},   //V 1.8
	{UINT16_T, 0, 32767, 600},   //V 1.8   
	{UINT16_T, 0, 32767, 200},   //V 1.8  
	{UINT16_T, 0, 32767, 3000},  //V 1.8   
	{UINT16_T, 0, 32767, 200},   //V 1.8  
	{UINT16_T, 0, 32767, 16479},  //V 1.8  
	{UINT16_T, 0, 32767, 14238},    //V 1.8
	{UINT16_T, 0, 32767, 1},    //V 1.8
	{UINT16_T, 0, 32767, 1}    //V 1.8 
};

const ST_PARA_ATTR stHalParaAttr[]=//m1HalParaSize] =
{
	{UINT16_T, 0, 65535, 825},    // BitTo10mVCoef
	{UINT16_T, 0, 65535, 20336},     // F10mVToBitCoef
	{UINT16_T, 0, 65535, 12001},    // BitTo10mACoef
	{UINT16_T, 0, 65535, 1398},    // F10mAToBitCoef
	{UINT16_T, 1024, 32767, 5957}, // RatedVolBit
	{UINT16_T, 0, 0, 0},            /*??*/
	{UINT16_T, 0, 65535, 8192},
	{UINT16_T, 0, 65535, 8192},
	{UINT16_T, 0, 65535, 8192},
	{UINT16_T, 0, 65535, 8192},
	{UINT16_T, 0, 65535, 8192},
	{INT16_T, -32768, 32767, 0},
	{INT16_T, -32768, 32767, 0},
	{UINT16_T, 15872, 16896, 16384},	
	{INT16_T, -512, 511, 0}   
};

const ST_PARA_ATTR stSpdFdbParaAttr[]=//m1SpdFdbParaSize] =
{
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192},
		{UINT16_T, 8192, 32767, 8192}
};

const ST_PARA_ATTR stSysCfgParaAttr[]=//m1SysCfgParaSize] =
{
	{UINT16_T, 0, 65535, 0},
	{UINT16_T, 0, 65535, 0},   
	{UINT16_T, 0, 65535, 0},    // DA1Sel
	{UINT16_T, 0, 65535, 0},    // DA2Sel
};


const ST_EEPROM_PARA stEEPROMParaArr[] =
{
	{(ST_PARA_ATTR *)&stMotorParaAttr, (Uint16 *)&m1MotorParaSize, EPRM_MOTORPARA_ADDR, EPRM_MOTORPARA_BAK_ADDR},
	{(ST_PARA_ATTR *)&stCtrlParaAttr, (Uint16 *)&m1CtrlParaSize, EPRM_CTRLPARA_ADDR, EPRM_CTRLPARA_BAK_ADDR},
	{(ST_PARA_ATTR *)&stFaultParaAttr, (Uint16 *)&m1FaultParaSize, EPRM_FAULTRPARA_ADDR, EPRM_FAULTRPARA_BAK_ADDR},
	{(ST_PARA_ATTR *)&stWarnParaAttr, (Uint16 *)&m1WarnParaSize, EPRM_WARNPARA_ADDR, EPRM_WARNPARA_BAK_ADDR},
	{(ST_PARA_ATTR *)&stSnsLessParaAttr, (Uint16 *)&m1SnsLessParaSize, EPRM_SNSLESSPARA_ADDR, EPRM_SNSLESSPARA_BAK_ADDR},
	{(ST_PARA_ATTR *)&stHalParaAttr, (Uint16 *)&m1HalParaSize, EPRM_HALPARA_ADDR, EPRM_HALPARA_BAK_ADDR},
	{(ST_PARA_ATTR *)&stSpdFdbParaAttr, (Uint16 *)&m1SpdFdbParaSize, EPRM_SPDFDBPARA_ADDR, EPRM_SPDFDBPARA_BAK_ADDR},
	{(ST_PARA_ATTR *)&stSysCfgParaAttr, (Uint16 *)&m1SysCfgParaSize, EPRM_SYSCFGPARA_ADDR, EPRM_SPDFDBPARA_BAK_ADDR}
};

const ST_UPDATE_PARA stUpdateParaArr[] = 
{
    {(void *)&m1MotorPara, (ST_PARA_ATTR *)&stMotorParaAttr, (Uint16 *)&m1MotorParaSize},
    {(void *)&m1CtrlPara, (ST_PARA_ATTR *)&stCtrlParaAttr, (Uint16 *)&m1CtrlParaSize},
    {(void *)&m1FaultPara, (ST_PARA_ATTR *)&stFaultParaAttr, (Uint16 *)&m1FaultParaSize},
    {(void *)&m1WarnPara, (ST_PARA_ATTR *)&stWarnParaAttr, (Uint16 *)&m1WarnParaSize},
    {(void *)&m1SnsLessPara, (ST_PARA_ATTR *)&stSnsLessParaAttr, (Uint16 *)&m1SnsLessParaSize},
    {(void *)&m1HalPara, (ST_PARA_ATTR *)&stHalParaAttr, (Uint16 *)&m1HalParaSize},
    {(void *)&m1SpdFdbPara, (ST_PARA_ATTR *)&stSpdFdbParaAttr, (Uint16 *)&m1SpdFdbParaSize},
    {(void *)&m1SysCfgPara, (ST_PARA_ATTR *)&stSysCfgParaAttr, (Uint16 *)&m1SysCfgParaSize}
};

/**
 *******************************************************************************
 * @fn      ChkEpmParaValid
 * @brief   ??EEPROM????????
 * @param   Uint16 *pPara	                ???????????????
 			ST_PARA_ATTR *pstParaAttr       ???????????????
 			Uint16 Len                      ????????(16????)
 * @return  bool TRUE??, FALSE??
 *******************************************************************************
 */
static bool ChkEpmParaValid(Uint16 *pPara, ST_PARA_ATTR *pstParaAttr, Uint16 Len)
{
    Uint16 Index;
    bool bStatus = TRUE;
    
    if(pPara == NULL || pstParaAttr == NULL)
    {
        return FALSE;
    }
    
    for(Index = 0; Index < Len; Index++)
    {
        if(pstParaAttr[Index].eType == UINT16_T)
        {
            if((*(Uint16 *)pPara) >= pstParaAttr[Index].LowLmt.usVal && (*(Uint16 *)pPara) <= pstParaAttr[Index].UpLmt.usVal)
            {
                pPara++;
            }
            else
            {
                bStatus = FALSE;
                break;
            }
        }
        else
        {
            if((*(INT16S *)pPara) >= pstParaAttr[Index].LowLmt.ssVal && (*(INT16S *)pPara) <= pstParaAttr[Index].UpLmt.ssVal)
            {
                pPara++;
            }
            else
            {
                bStatus = FALSE;
                break;
            }
        }
    }
    
    return bStatus;
}

/**
 *******************************************************************************
 * @fn      ChkUpdateParaValid
 * @brief   ??????????????????
 * @param   Uint16 *pVal	                ???????
 			ST_PARA_ATTR *pstParaAttr       ???????????????
 			Uint16 Index                    ?????????????
 * @return  bool TRUE??, FALSE??
 *******************************************************************************
 */
static bool ChkUpdateParaValid(Uint16 *pVal, ST_PARA_ATTR *pstParaAttr, Uint16 Index)
{
    bool bStatus = TRUE;
    
    if(pVal == NULL || pstParaAttr == NULL)
    {
        return FALSE;
    }

    /* ????????????????? */
    if(pstParaAttr[Index].eType == UINT16_T)
    {
        if((*(Uint16 *)pVal) < pstParaAttr[Index].LowLmt.usVal || (*(Uint16 *)pVal) > pstParaAttr[Index].UpLmt.usVal)
        {
            bStatus = FALSE;
        }
    }
    else
    {
        if((*(INT16S *)pVal) < pstParaAttr[Index].LowLmt.ssVal || (*(INT16S *)pVal) > pstParaAttr[Index].UpLmt.ssVal)
        {
            bStatus = FALSE;
        }
    }
    return bStatus;
}

/**
 *******************************************************************************
 * @fn      UpdateRamPara
 * @brief   ????????????RAM?????
 * @param   Uint16 Index	      ????????
 			Uint16 SubIndex       ?????????????
 			Uint16 Val            ??????
 * @return  bool TRUE??, FALSE??
 *******************************************************************************
 */
bool UpdateRamPara(Uint16 Index, Uint16 SubIndex, Uint16 Val)
{
    bool bStatus = FALSE;
    ST_UPDATE_PARA *pUpdatePara = (ST_UPDATE_PARA *)&stUpdateParaArr[0];
    ST_PARA_ATTR *pstParaAttr;
    
    if(Index < EEPROM_PARA_NUM)
    {
        if(SubIndex < *(pUpdatePara[Index].pSize))
        {
            pstParaAttr = pUpdatePara[Index].pstParaAttr;
            
            /* ?????????????,????????? */
            if(ChkUpdateParaValid(&Val, pstParaAttr, SubIndex) == TRUE)
            {
                *((Uint16 *)(pUpdatePara[Index].pPara) + SubIndex) = Val;
                bStatus = TRUE;
            }
        }
    }

    return bStatus;
}

/**
 *******************************************************************************
 * @fn      SavePara
 * @brief   ?????EEPROM
 * @param   void *pPara	                        ??????????????
 			const ST_EEPROM_PARA *pstEPMPara    ???????????
 			E_PARA_INDEX eParaIndex             ????????????
 * @return  bool TRUE??, FALSE??
 *******************************************************************************
 */
bool SavePara(void *pPara, const ST_EEPROM_PARA *pstEPMPara, E_PARA_INDEX eParaIndex)
{
    bool bStatus = FALSE;
    Uint16 Sum;
    Uint16 FixCode = EPRM_PARA_FIXCODE;
    
    if(pPara == NULL || pstEPMPara == NULL)
    {
        return bStatus;
    }
    /* ??????? */
    if(!EEPROM_PageWrBuf(pstEPMPara[eParaIndex].ParaAddr, &FixCode, 1))
	{
		return bStatus;
	}
	/* ??????????*/
    Sum = CalChkSum((Uint16 *)pPara, (*(pstEPMPara[eParaIndex].pSize)));
    if(!EEPROM_PageWrBuf(pstEPMPara[eParaIndex].ParaAddr + 2, (Uint16 *)pPara, *(pstEPMPara[eParaIndex].pSize)))
    {
		return bStatus;
	}
	if(!EEPROM_PageWrBuf(pstEPMPara[eParaIndex].ParaAddr + 2 + (*(pstEPMPara[eParaIndex].pSize)) * 2, &Sum, 1))
    {
		return bStatus;
	}

    /* ???????? */
    if(!EEPROM_PageWrBuf(pstEPMPara[eParaIndex].ParaBakAddr, &FixCode, 1))
	{
		return bStatus;
	}
	/* ???????????*/
    if(!EEPROM_PageWrBuf(pstEPMPara[eParaIndex].ParaBakAddr + 2, (Uint16 *)pPara, *(pstEPMPara[eParaIndex].pSize)))
    {
		return bStatus;
	}
	if(!EEPROM_PageWrBuf(pstEPMPara[eParaIndex].ParaBakAddr + 2 + (*(pstEPMPara[eParaIndex].pSize)) * 2, &Sum, 1))
    {
		return bStatus;
	}

	/* ?????????,??TRUE */
	bStatus = TRUE;
    return bStatus;
}


/**
 *******************************************************************************
 * @fn      LoadPara
 * @brief   ?EEPROM?????
 * @param   void *pPara	                        ??????????????
 			const ST_EEPROM_PARA *pstEPMPara    ???????????
 			E_PARA_INDEX eParaIndex             ????????????
 * @return  bool TRUE??, FALSE??
 *******************************************************************************
 */
bool LoadPara(void *pPara, const ST_EEPROM_PARA *pstEPMPara, E_PARA_INDEX eParaIndex)
{
    bool bStatus = FALSE;
    Uint16 FixCode, ChkSum;
    Uint16 *pstPara = (Uint16 *)pPara;
    
    if(pPara == NULL || pstEPMPara == NULL)
    {
        return bStatus;
    }
    
    /* ???????? */
    EEPROM_RdBuf(pstEPMPara[eParaIndex].ParaAddr, &FixCode, 1);
    if(FixCode == EPRM_PARA_FIXCODE)
    {
        /* ??????? */
        EEPROM_RdBuf(pstEPMPara[eParaIndex].ParaAddr + 2, pstPara, *(pstEPMPara[eParaIndex].pSize));
        /* ???????? */
        EEPROM_RdBuf((pstEPMPara[eParaIndex].ParaAddr + 2 + ((*(pstEPMPara[eParaIndex].pSize)) << 1)), &ChkSum, 1);

        /* ???????????? */
        if(ChkSum == CalChkSum(pstPara, *(pstEPMPara[eParaIndex].pSize)))
        {
            /* ????????(???) */
            if(ChkEpmParaValid(pstPara, pstEPMPara[eParaIndex].pstParaAttr, *(pstEPMPara[eParaIndex].pSize)) == TRUE)
            {
                /* ?????,??????????????? */
                bStatus = TRUE;
            }
        }
    }

    /* ????????,?????? */
    if(bStatus == FALSE)
    {
        /* ????? */
        EEPROM_RdBuf(pstEPMPara[eParaIndex].ParaBakAddr, &FixCode, 1);
        if(FixCode == EPRM_PARA_FIXCODE)
        {
            /* ???????? */
            EEPROM_RdBuf(pstEPMPara[eParaIndex].ParaBakAddr + 2, pstPara, *(pstEPMPara[eParaIndex].pSize));
            /* ????????? */
            EEPROM_RdBuf((pstEPMPara[eParaIndex].ParaBakAddr + 2 + ((*(pstEPMPara[eParaIndex].pSize)) << 1)), &ChkSum, 1);
            /* ????????????? */
            if(ChkSum == CalChkSum(pstPara, *(pstEPMPara[eParaIndex].pSize)))
            {
                /* ????????(???) */
                if(ChkEpmParaValid(pstPara, pstEPMPara[eParaIndex].pstParaAttr, *(pstEPMPara[eParaIndex].pSize)) == TRUE)
                {
                    bStatus = TRUE;
                }
            }
        }
    }

    return bStatus;
}

/**
 *******************************************************************************
 * @fn      LoadDefaultPara
 * @brief   ??????
 * @param   Uint16 *pPara	                    ????????????????
 			const ST_EEPROM_PARA *pstEPMPara    ???????????
 			E_PARA_INDEX eParaIndex             ????????????
 * @return  bool TRUE??, FALSE??
 *******************************************************************************
 */
bool LoadDefaultPara(Uint16 *pPara, const ST_EEPROM_PARA *pstEPMPara, E_PARA_INDEX eParaIndex)
{
    Uint16 Index;
    ST_PARA_ATTR *pstPara;

    if(pPara == NULL || pstEPMPara == NULL)
    {
        return FALSE;
    }
    pstPara = pstEPMPara[eParaIndex].pstParaAttr;

    /* ????????????,???16??????,???? */
    for(Index = 0; Index < *(pstEPMPara[eParaIndex].pSize); Index++)
    {
        if(pstEPMPara[eParaIndex].pstParaAttr[Index].eType == UINT16_T)
        {
            *(Uint16 *)pPara = pstPara[Index].Default.usVal;
            pPara++;
        }
        else
        {
            *(INT16S *)pPara = pstPara[Index].Default.ssVal;
            pPara++;
        }
    }
    return TRUE;
}


/**
 *******************************************************************************
 * @fn      CalChkSum
 * @brief   ??16????
 * @param   Uint16 *pBuf	???????????????
 			Uint16 Len	    ???????(?16?????)
 * @return  Uint16 Sum      16????
 *******************************************************************************
 */
Uint16 CalChkSum(Uint16 *pBuf, Uint16 Len)
{
    Uint16 Sum = 0;
    while(Len--)
    {
        Sum += (*pBuf & 0xFF) + ((*pBuf >> 8) & 0xFF);
        pBuf++;
    }
    return Sum;
}




