/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore _CurrLoopCtler结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-05                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#include"MC_Include.h"
#ifndef CurrLoopCtler_H_H
#define CurrLoopCtler_H_H
//AD转换结果存储的通道号
#define AD_MAIN_CURR_U 		0
#define AD_MAIN_CURR_W 		1
#define AD_BEMF_UV			2
#define AD_BEMF_VW			3
#define AD_12VP 			4
#define AD_12EXT 			5
#define AD_TEMP1 			6
#define AD_TEMP2 			7
#define AD_TEMP3 			7//第3路直接用用第2路问题
#define AD_TESTTORQ 		8//第3路采集通道用作转矩采集
#define AD_REF1V5 			9
#define AD_REF2V5 			10
#define AD_BEMF_U	 		11
#define AD_BEMF_V 			12
#define AD_BEMF_W			13
#define AD_ANGLE_SIN			14
#define AD_ANGLE_COS 			15


struct MOTCTL_BIT {
    Uint16   Start:1;
    Uint16   ResolverCfg:1;
    Uint16   PosSel:2;
    Uint16   CurrAntiRstEn:1;
    Uint16   SpdAntiRstEn:1;
    Uint16   EnSpdCtrl:1; 
    Uint16   EnBusModify:1;
    Uint16   PolesRatio:3;
    Uint16   UserSmcIn:1;
    Uint16   EnUCurrNeg:1;
    Uint16   EnVCurrNeg:1;
    Uint16   ClrErr:1;
    Uint16   UpOrDown:1;	
};
union  MOTCTL_REG {
   Uint16             all;
   struct MOTCTL_BIT  bit;
};

struct ERR_BIT {
    Uint16   OCERR:1;
    Uint16   SCERR:1;
    Uint16   OVERR:1;
    Uint16   UVERR:1;
    Uint16   POSERR:1;
    Uint16   UVLO:1;
    Uint16   ServoDis:1;
    Uint16   rsd:9;
};
union  ERR_REG {
   Uint16             all;
   struct ERR_BIT  bit;
};

struct ERRF_BIT {
    Uint16   OCERR:1;
    Uint16   SCERR:1;
    Uint16   OVERR:1;
    Uint16   UVERR:1;
    Uint16   POSERR:1;
    Uint16   UVLO:1;
    Uint16   ServoDis:1;
    Uint16   rsd:8;
    Uint16   ORERR:1;
};
union  ERRF_REG {
   Uint16             all;
   struct ERRF_BIT  bit;
};
typedef struct
{
	union  	MOTCTL_REG 	MotCtl;		    //		0	1	Ctrl	R/W-0	Motor Control Register
	union  	ERR_REG 	ErrSignal;		//		1	1	Output	R-0	Error Signal Register
	union  	ERRF_REG 	ErrFlag;		//		2	1	Output	R-0	Error Flag Register
	INT16S 				IdCmd;			//		3	1	Ctrl	R/W-0	
	INT16S				IqCmd;			//		4	1	Ctrl	R/W-0	
	INT16S				UdCmd_V;		//		5	1	Ctrl	R/W-0	
	INT16S				UqCmd_V;		//		6	1	Ctrl	R/W-0	
	INT16S				PosModify;		//		7	1	Ctrl	R/W-0	
	Uint16				ManuPos;		//		8	1	Ctrl	R/W-0	
	INT16S				IdKp;			//		9	1	Cfg	R/W-0	
	INT16S				IdKi;			//		10	1	Cfg	R/W-0	
	INT16S				IqKp;			//		11	1	Cfg	R/W-0	
	INT16S				IqKi;			//		12	1	Cfg	R/W-0	
	INT16S				PwmPeriod;		//		13	1	Cfg	R/W-3000	
	INT16S				PosOff;			//		14	1	Cfg	R/W-0	
	INT16S				DeadTime;		//		15	1	Cfg	R/W-120	
	INT16S				IOffU;			//		16	1	Cfg	R/W-8192	
	INT16S				IOffW;			//		17	1	Cfg	R/W-8192	
	INT16S				IOriU;			//		18	1	Input	R-8192	
	INT16S				IOriW;			//		19	1	Input	R-8192	
	INT16S				IFdbAlpha;		//		20	1	Output	R-0	
	INT16S				IFdbBeta;		//		21	1	Output	R-0	
	INT16S				IdFdb;			//		22	1	Output	R-0	
	INT16S				IqFdb;			//		23	1	Output	R-0	
	INT16S				DeltaD;			//		24	1	Output	R-0	
	INT16S				DeltaQ;			//		25	1	Output	R-0	
	INT16S				IdProp;			//		26	1	Output	R-0	
	INT16S				IqProp;			//		27	1	Output	R-0	
	INT16S				IdIntergLim;	//		28	1	Output	R-0	
	INT16S				IqIntergLim;	//		29	1	Output	R-0	
	INT16S				UdPILim_V;		//		30	1	Output	R-0	
	INT16S				UqPILim_V;		//		31	1	Output	R-0	
	INT16S				UdOut_BIT;		//		32	1	Output	R-0	
	INT16S				UqOut_BITOri;	//		33	1	Output	R-0	
	INT16S				UqOut_BIT;		//		34	1	Output	R-0	
	INT16S				Uq_BIT_Max;		//		35	1	Output	R-0	
	INT16S				UdActOut_V;		//		36	1	Output	R-0	
	INT16S				UqActOut_V;		//		37	1	Output	R-0	
	INT16S				DisChageD;		//		38	1	Output	R-0	
	INT16S				DisChageQ;		//		39	1	Output	R-0	
	INT16S				ThetaPark;		//		40	1	Output	R-0	
	INT16S				ThetaInvPark;	//		41	1	Output	R-0	
	INT16S				UAlpha_BIT;		//		42	1	Output	R-0	
	INT16S				UBeta_BIT;		//		43	1	Output	R-0	
	INT16S				UAlpha_V;		//		44	1	Output	R-0	
	INT16S				UBeta_V;		//		45	1	Output	R-0	
	INT16S				PwmU_CMP;		//		46	1	Output	R-0	
	INT16S				PwmV_CMP;		//		47	1	Output	R-0	
	INT16S				PwmW_CMP;		//		48	1	Output	R-0	
	INT16S				PWM_CTR;		//		49	1	Output	R-0	
	INT32S				IdIntergLim32;	//			Output	R-0	
	INT32S				IqIntergLim32;	//			Output	R-0	

}stCurrLoopCtler;

extern  stCurrLoopCtler m1CurrLoopCtler;
extern  void DSPCore_SwitchOffPWM(void);
extern  void DSPCore_SwitchOnPWM(void);
extern  bool DSPCore_CfgCurrLoopCtler(stCurrLoopCtler* pD);

#endif
