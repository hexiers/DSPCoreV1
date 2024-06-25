/*======================================================================================================
* @�ļ�����	:        ��Ϣ��ؽṹ�嶨��                                               
* @�ļ�����	: 	����MCMessage �澯��ֵ�����ṹ��
* @������		:  	����                                                                              
* ��������		:   	2020-03-03                                                                 
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#include"MC_Include.h"
#ifndef MCMESSAGE_H_H
#define MCMESSAGE_H_H


#define MSG_LENTHS 4



typedef struct
{
	Message_t msg;		// ��Ϣ������
	INT16S 	msgPara1;	// ��Ϣ�ĸ��Ӳ���1
	INT16S 	msgPara2;	// ��Ϣ�ĸ��Ӳ���2
	INT16S 	msgPara3;	// ��Ϣ�ĸ��Ӳ���3
	INT16S 	msgPara4;	// ��Ϣ�ĸ��Ӳ���4
	Uint16 	Priority;		// ��Ϣ�����ȼ�
}stMCMessage;

typedef struct
{
	stMCMessage msgArray[MSG_LENTHS];
	Uint16 InPointer;// ָ�򼴽��������Ϣ�ĵ�ַ
	Uint16 OutPointer;// ָ�򼴽�ȡ������Ϣ�ĵ�ַ
}stMCMessageArray;

extern  stMCMessageArray m1MsgArray;
extern  stMCMessage m1Msg;

extern void MC_InitMsg(stMCMessageArray* pMsgArray);
extern bool GetMessge(stMCMessage * pOutMsg);
extern bool PostMessge(stMCMessage * pMsg);
extern bool MC_NotifyCmdDisCard(Message_t msg);
extern bool MC_PostStopMessge(void);

#endif
