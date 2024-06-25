/*======================================================================================================
* @�ļ�����	:        ��Ϣ��غ���                                               
* @�ļ�����	: 	����
* @������		:  	����                                                                              
* ��������		:   	2020-03-07                                                                 
* �޸ļ�¼		:   
*                         
======================================================================================================*/
#include"MCSoft_Message.h"


stMCMessageArray m1MsgArray;
/**************************************************************************************************************************************************
* @������		:  		MC_InitMsg                                                        
* @��������	: 		�����Ϣ���У�ͬʱ����Ϣ��ڳ���ָ��ָ��0
* @������		:    		����                                                                            
* ��������		:   		2020-03-07                                                             
* �޸ļ�¼		:   			
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
* @������		:  		GetMessge                                                        
* @��������	: 		����Ϣ����ȡ��Ϣ��Ϊ�շ���FALSE
* @������		:    		����                                                                            
* ��������		:   		2020-03-07                                                             
* �޸ļ�¼		:   			
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
		/* �������ݿ������ָ�븳ֵ����ָֹ��ָ�����Ϣ�������ݱ��½��յ���Ϣ�ƻ�*/
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
* @������		:  		PostMessge                                                        
* @��������	: 		����Ϣ���з�����Ϣ��Ϊ������FALSE���������������ϵ�һ����Ϣ
* @������		:    		����                                                                            
* ��������		:   		2020-03-07                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool PostMessge(stMCMessage * pMsg)
{
	/* ���½��յ���Ϣ���������գ�������Ϣ��������*/
	m1MsgArray.msgArray[m1MsgArray.InPointer].msg = pMsg->msg;
	m1MsgArray.msgArray[m1MsgArray.InPointer].msgPara1 = pMsg->msgPara1;
	m1MsgArray.msgArray[m1MsgArray.InPointer].msgPara2 = pMsg->msgPara2;
	m1MsgArray.msgArray[m1MsgArray.InPointer].msgPara3 = pMsg->msgPara3;
	m1MsgArray.msgArray[m1MsgArray.InPointer].msgPara4 = pMsg->msgPara4;
	m1MsgArray.msgArray[m1MsgArray.InPointer].Priority = pMsg->Priority;

	/*  ������βָ��*/
	m1MsgArray.InPointer++;
	if(m1MsgArray.InPointer >= MSG_LENTHS)
	{
		m1MsgArray.InPointer = 0;
	}
	
	/* ��ָ��ͷβ���ʱ���������ϵ�һ����Ϣ*/
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
* @������		:  		MC_NotifyCmdDisCard                                                        
* @��������	: 		��ȡ��Ϣ��û��ִ�У�ͨ�溯��
* @������		:    		����                                                                            
* ��������		:   		2020-03-07                                                             
* �޸ļ�¼		:   			
*                         
****************************************************************************************************************************************************/
bool MC_NotifyCmdDisCard(Message_t msg)
{
	// ����ӣ���ǰָ�����
	m1CriPara.DiscardCmdMsg = msg;
	if(msg != CmdNOCMD)
	{
		m1CriPara.MsgBox.bit.MsgBoxCmdNoResponse = 1;
	}
	return TRUE;
}

/**************************************************************************************************************************************************
* @������		:  		MC_NotifyCmdDisCard                                                        
* @��������	: 		��ȡ��Ϣ��û��ִ�У�ͨ�溯��
* @������		:    		����                                                                            
* ��������		:   		2020-03-07                                                             
* �޸ļ�¼		:   			
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

