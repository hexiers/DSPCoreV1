/*======================================================================================================
* @�ļ�����	:                                                       
* @�ļ�����	: 	����MCCore _FaultDealModule�ṹ��
* @������		:  	����                                                                              
* ��������		:   	2020-03-06                                                                 
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#include"MC_Include.h"
#ifndef FaultDealModule_H_H
#define FaultDealModule_H_H

typedef struct
{
	Uint16 	OVPoint;//	0	1	Cfg	R/W-65535
	Uint16 	UVPoint;//	1	1	Cfg	R/W-0
}stFaultDealModule;

extern   stFaultDealModule m1FaultDealModule;
extern bool DSPCore_CfgFaultDealModule(stFaultDealModule* pR);
#endif
