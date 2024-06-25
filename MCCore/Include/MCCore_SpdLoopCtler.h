/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore _SpdLoopCtler结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-06                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#include"MC_Include.h"
#ifndef SpdLoopCtler_H_H
#define SpdLoopCtler_H_H

typedef struct
{
	INT16S	FwUsCmd;		//	0	1	Ctl	R/W-0	速度指令寄存器
	INT16S	FwKp;			//	1	1	Cfg	R/W-0	
	INT16S	FwKi;			//	2	1	Cfg	R/W-0	
	INT16S	FwOutMax;		//	3	1	Cfg	R/W-0	
	INT16S	FwOutMin;		//	4	1	Cfg	R/W-0	
	INT16S	FwUsFdb;	//	6	1	Output	R-0	
	INT16S	FwProp;		//	7	1	Output	R-0	
	INT16S	FwIntergLim;	//	8	1	Output	R-0	
	INT16S	FwPIOut;		//	9	1	Output	R-0	
	INT16S	FwLimOut;		//	10	1	Output	R-0	
	INT16S	DisChargeFw;	//	11	1	Output	R-0	
	INT32S	FwIntergLim32;	//		1	Output	R-0	

}stFwCtler;

typedef struct
{
	INT16S	SpdElecCmd;		//	0	1	Ctl	R/W-0	速度指令寄存器
	INT16S	SpdKp;			//	1	1	Cfg	R/W-0	
	INT16S	SpdKi;			//	2	1	Cfg	R/W-0	
	INT16S	SpdOutMax;		//	3	1	Cfg	R/W-0	
	INT16S	SpdOutMin;		//	4	1	Cfg	R/W-0	
	INT16S	SpdIntergPreset;//	5	1	Cfg	R/W-0	
	INT16S	SpdFdbElecRpm;	//	6	1	Output	R-0	
	INT16S	SpdProp;		//	7	1	Output	R-0	
	INT16S	SpdIntergLim;	//	8	1	Output	R-0	
	INT16S	SpdPIOut;		//	9	1	Output	R-0	
	INT16S	SpdLimOut;		//	10	1	Output	R-0	
	INT16S	DisChargeSpd;	//	11	1	Output	R-0	
	INT32S	SpdIntergLim32;	//		1	Output	R-0	

}stSpdLoopCtler;

extern   stFwCtler m1FwCtler;
extern   stSpdLoopCtler m1SpdLoopCtler;
extern bool DSPCore_DoSpdLoopCtrl(stSpdLoopCtler* pD,stResolverDealModule* pR,stSmcObserverExt* pS);
extern bool DSPCore_DoFwCtrl(stFwCtler* pD,stCurrLoopCtler* pC);

#endif
