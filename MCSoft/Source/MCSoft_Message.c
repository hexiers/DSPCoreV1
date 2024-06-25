/*======================================================================================================
* @文件名称	:        消息相关函数                                               
* @文件描述	: 	定义
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-07                                                                 
* 修改记录		:   
*                         
======================================================================================================*/
#include"MCSoft_Message.h"


stMCMessageArray m1MsgArray;
/**************************************************************************************************************************************************
* @函数名		:  		MC_InitMsg                                                        
* @函数描述	: 		清空消息队列，同时将消息入口出口指针指向0
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-07                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
void MC_InitMsg(stMCMessageArray* pMsgArray)
{ 
	Uint16 i = 0;
	for(i = 0;i<MSG_LENTHS;i++)
	{
		pMsgArray->msgArray[i].msg = CmdNOCMD;
	}
	pMsgArray->InPointer = 0;
	pMsgArray->OutPointer = 0;
}

/**************************************************************************************************************************************************
* @函数名		:  		GetMessge                                                        
* @函数描述	: 		从消息队列取消息，为空返回FALSE
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-07                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool GetMessge(stMCMessage * pOutMsg)
{
	if(m1MsgArray.InPointer == m1MsgArray.OutPointer)
	{
		pOutMsg->msg = CmdNOCMD;
		pOutMsg->msgPara1 = 0;
		pOutMsg->msgPara2 = 0;
		pOutMsg->msgPara3 = 0;
		pOutMsg->msgPara4 = 0;
		pOutMsg->Priority = 0;

		return FALSE;
	}
	else
	{
		/* 采用数据拷贝替代指针赋值，防止指针指向的消息队列数据被新接收的消息破坏*/
		pOutMsg->msg = m1MsgArray.msgArray[m1MsgArray.OutPointer].msg;
		pOutMsg->msgPara1 = m1MsgArray.msgArray[m1MsgArray.OutPointer].msgPara1;
		pOutMsg->msgPara2 = m1MsgArray.msgArray[m1MsgArray.OutPointer].msgPara2;
		pOutMsg->msgPara3 = m1MsgArray.msgArray[m1MsgArray.OutPointer].msgPara3;
		pOutMsg->msgPara4 = m1MsgArray.msgArray[m1MsgArray.OutPointer].msgPara4;
		pOutMsg->Priority = m1MsgArray.msgArray[m1MsgArray.OutPointer].Priority;
		m1MsgArray.OutPointer++;
		if(m1MsgArray.OutPointer >= MSG_LENTHS)
		{
			m1MsgArray.OutPointer = 0;
		}
		return TRUE;
	}		
}
/**************************************************************************************************************************************************
* @函数名		:  		PostMessge                                                        
* @函数描述	: 		向消息队列发送消息，为满返回FALSE，丢弃队列中最老的一个消息
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-07                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool PostMessge(stMCMessage * pMsg)
{
	/* 最新接收的消息无条件接收，即便消息队列已满*/
	m1MsgArray.msgArray[m1MsgArray.InPointer].msg = pMsg->msg;
	m1MsgArray.msgArray[m1MsgArray.InPointer].msgPara1 = pMsg->msgPara1;
	m1MsgArray.msgArray[m1MsgArray.InPointer].msgPara2 = pMsg->msgPara2;
	m1MsgArray.msgArray[m1MsgArray.InPointer].msgPara3 = pMsg->msgPara3;
	m1MsgArray.msgArray[m1MsgArray.InPointer].msgPara4 = pMsg->msgPara4;
	m1MsgArray.msgArray[m1MsgArray.InPointer].Priority = pMsg->Priority;

	/*  调整队尾指针*/
	m1MsgArray.InPointer++;
	if(m1MsgArray.InPointer >= MSG_LENTHS)
	{
		m1MsgArray.InPointer = 0;
	}
	
	/* 当指针头尾相接时，丢弃最老的一条消息*/
	if(m1MsgArray.InPointer == m1MsgArray.OutPointer)
	{
		m1MsgArray.OutPointer++;
		if(m1MsgArray.OutPointer >= MSG_LENTHS)
		{
			m1MsgArray.OutPointer = 0;
		}
		return FALSE;
	}
	else
	{
		return TRUE;
	}
			
}
/**************************************************************************************************************************************************
* @函数名		:  		MC_NotifyCmdDisCard                                                        
* @函数描述	: 		读取消息后，没有执行，通告函数
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-07                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool MC_NotifyCmdDisCard(Message_t msg)
{
	// 待添加，当前指令被丢弃
	m1CriPara.DiscardCmdMsg = msg;
	if(msg != CmdNOCMD)
	{
		m1CriPara.MsgBox.bit.MsgBoxCmdNoResponse = 1;
	}
	return TRUE;
}

/**************************************************************************************************************************************************
* @函数名		:  		MC_NotifyCmdDisCard                                                        
* @函数描述	: 		读取消息后，没有执行，通告函数
* @创建人		:    		张宇                                                                            
* 创建日期		:   		2020-03-07                                                             
* 修改记录		:   			
*                         
****************************************************************************************************************************************************/
bool MC_PostStopMessge(void)
{
	stMCMessage msg;
	msg.msg = CmdSTOPMOTOR;
	msg.msgPara1 = 0;
	msg.msgPara2 = 0;
	msg.msgPara3 = 0;
	msg.msgPara4 = 0;
	msg.Priority = 0;
	PostMessge(&msg);
	return TRUE;
}

