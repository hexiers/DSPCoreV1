/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����MCCore _BusVolDecoupler�ṹ��
* @������		:  	����                                                                              
* ��������		:   	2020-03-06                                                                 
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#include"MC_Include.h"
#ifndef BusVolDecoupler_H_H
#define BusVolDecoupler_H_H


typedef struct
{
	INT16S 	RatedVolBit;		//	0	1	Cfg	R/W-2000	
	INT16S 	V_Bus;			//	1	1	Output	R-0	
	INT16S 	VBusProtect;		//	2	1	Output	R-0	
	INT16S 	BusCoef;			//	3	1	Output	R-0	

}stBusVolDecoupler;

extern   stBusVolDecoupler m1BusVolDecoupler;
extern bool DSPCore_CfgBusVolDecoupler(stBusVolDecoupler* pBus);
extern bool DSPCore_DoBusVolDecoupler(stBusVolDecoupler* pBus);

#endif
