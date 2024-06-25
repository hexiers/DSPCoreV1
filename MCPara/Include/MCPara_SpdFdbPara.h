/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义MCCore 速度修正结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#ifndef SPDFDBPARA_H_H
#define SPDFDBPARA_H_H

#include"MC_Include.h"


typedef struct
{
	Uint16 	ModifyCoef[64];

}stSpdFdbPara;
extern   stSpdFdbPara m1SpdFdbPara;


#endif
