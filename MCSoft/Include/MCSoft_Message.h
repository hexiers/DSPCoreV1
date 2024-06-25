/*======================================================================================================
* @文件名称	:        消息相关结构体定义                                               
* @文件描述	: 	定义MCMessage 告警阈值参数结构体
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#include"MC_Include.h"
#ifndef MCMESSAGE_H_H
#define MCMESSAGE_H_H


#define MSG_LENTHS 4



typedef struct
{
	Message_t msg;		// 消息的类型
	INT16S 	msgPara1;	// 消息的附加参数1
	INT16S 	msgPara2;	// 消息的附加参数2
	INT16S 	msgPara3;	// 消息的附加参数3
	INT16S 	msgPara4;	// 消息的附加参数4
	Uint16 	Priority;		// 消息的优先级
}stMCMessage;

typedef struct
{
	stMCMessage msgArray[MSG_LENTHS];
	Uint16 InPointer;// 指向即将放入的消息的地址
	Uint16 OutPointer;// 指向即将取出的消息的地址
}stMCMessageArray;

extern  stMCMessageArray m1MsgArray;
extern  stMCMessage m1Msg;

extern void MC_InitMsg(stMCMessageArray* pMsgArray);
extern bool GetMessge(stMCMessage * pOutMsg);
extern bool PostMessge(stMCMessage * pMsg);
extern bool MC_NotifyCmdDisCard(Message_t msg);
extern bool MC_PostStopMessge(void);

#endif
