/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore _ResolverDealModule结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-06                                                                 
* 修改记录		:   
*                         2020-10-10: 增加初始角标定相关数据结构 和 宏定义
======================================================================================================*/
#include"MC_Include.h"
#ifndef ResolverDealModule_H_H
#define ResolverDealModule_H_H



typedef struct
{
	Uint16 BemfCnt;
	Uint16 EnUVSampleBemfP;
	Uint16 EnVWSampleBemfP;
	Uint16 EnUVSampleBemfN;
	Uint16 EnVWSampleBemfN;
	Uint16 UVPCrossPos;
	Uint16 UVNCrossPos;
	Uint16 VWPCrossPos;
	Uint16 VWNCrossPos;
	Uint16 AlignErrCode;	
} stPosCalib;

struct AD2S_REGS {
	Uint16 POSITION;
	Uint16 Rsd1;
	INT16S SPEED;
	Uint16 Rsd2;
	Uint16 Rsd3;
	Uint16 Rsd4;
	Uint16 FAULT;
	Uint16 Rsd5;
	};
extern volatile struct AD2S_REGS Ad2s_A;
typedef struct
{
	Uint16	Ad2sCfgReg;			//		0	1	Cfg	R/W-0x28
	Uint16	Ad2sExcFreqReg;		//		1	1	Cfg	R/W-0x7D
	Uint16  SpdBitToRpmCoef;    //		2	1	Cfg	R/W-7500
	Uint16  SpdModifyCoef;      //		3	1	Cfg	R/W-8192
	Uint16	ResolverOriPos;		//		4	1	Output	R-0
	Uint16	ResolverElecPos;	//		5	1	Output	R-0
	Uint16	FaultReg;			//		6	1	Output	R-0
	INT16S	SpdReg;				//		7	1	Output	R-0
	INT16S	ResolverElecRpm;	//		7	1	Output	R-0

}stResolverDealModule;
extern stPosCalib m1PosCalib; 
extern   stResolverDealModule m1ResolverDealModule;
extern bool DSPCore_CfgResolverDealModule(stResolverDealModule* pR);
extern bool DSPCore_DoResolverDealModule(stResolverDealModule* pR);
#define  BEMF_CHECK_THREA 50
#define  BEMF_CHECK_TIMES 5000
#define ELEC30DEG  (65535/12)
#define ELEC10DEG  (65535/36)
#endif
