/*======================================================================================================
* @????	:
* @????	: 	??????????????
* @???		:  	??
* ????	:   2020-03-03
* ????	:
*
======================================================================================================*/
 
#include"MC_Include.h"
#ifndef __MCSOFT_MATH_H
#define __MCSOFT_MATH_H


typedef struct
{
	Uint16 FifoLen;
	Uint16 Power;
	INT32S Sum;
	INT32S Fifo[128]; //2^7=128
	Uint32 pCnt;
}stFir,*stpFir;

extern void FirInit(stpFir stp,Uint16 FilterPower);
extern INT32S FirCalc(stpFir stp,INT32S DataIn,Uint16 EnDiv);
extern stFir SpdFdbFir;
extern stFir IqFdbFir;
extern stFir IdFdbFir;
extern stFir CoreVBusFdbFir;
extern stFir CoreSpdFdbFir;

#endif

