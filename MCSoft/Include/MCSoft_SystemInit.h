/*======================================================================================================
* @文件名称	:                                                       
* @文件描述	: 	定义
* @创建人		:  	张宇                                                                              
* 创建日期		:   	2020-03-03                                                                 
* 修改记录		:   
*                         
======================================================================================================*/

#include"MC_Include.h"
#ifndef SYSSTS_H_H
#define SYSSTS_H_H

#include"MCSoft_Message.h"
#include"MCSoft_SystemInit.h"

/* 系统状态枚举变量*/
typedef enum
{
	STS_PwrUpWait = 0x0001, 
	STS_BridgeCheck,
	STS_Idle,
	STS_StartUp,//
	STS_Run,//
	STS_MainTenance,
	STS_FaultNow,
	STS_FaultOver
} State_t;

/* 控制模式枚举变量*/
typedef enum
{
	MODE_CURR = 0x0001, 
	MODE_SPD
} mode_t;

/* 系统时间结构体*/
typedef struct
{
	Uint16 	Days;
	Uint16 	Hours;/* 每ms中断数*/
	Uint16 	Mins;
	Uint16 	Secs;
	Uint16 	ms;
	bool	 	Every1msPoint;
	bool	 	Every1msSet;
	bool 		Every10msSet;
	bool 		Every50msSet;
	bool 		Every500msSet;
	Uint16 	IntNums;
	Uint16 	IntsPerms;
	
}stTimeBase;

/* 系统状态结构体*/
typedef struct
{
	State_t 	MainState;	/*  主状态字*/
	Uint16	SubState;	/*  子状态字*/
	Uint16	StateWait;	/*  状态运行的周期计算*/
	
}stSysState;

/* EEPROM加载结构指示结构体*/
struct EEPROM_BIT {
	Uint16   UseEepromMotorPara:1;
	Uint16   UseEepromCtrlPara:1;
	Uint16   UseEepromFaultPara:1;
	Uint16   UseEepromWarnPara:1;
	Uint16   UseEepromSnslessPara:1;
	Uint16   UseEepromHalPara:1;
	Uint16   UseEepromSpdFdbPara:1;
	Uint16   UseEepromSysCfgPara:1;
	Uint16   Rsd:7;
	Uint16   EepromErr:1;
};
union  EEPROM_REG {
   Uint16             all;
   struct EEPROM_BIT  bit;
};

/*  全局状态量信号*/
struct GLOSTS_BIT {
	Uint16   FpgaFailFault:1;
	Uint16   SaveEepromFailWarn:1;
	Uint16   CaliPosOffsetSucc:1;

	Uint16   Rsd:13;

};
union  GLOSTS_REG {
   Uint16             all;
   struct GLOSTS_BIT  bit;
};
/* 上位机MessageBox通告消息*/
struct MSGBOX_BIT {
	Uint16   MsgBoxSaveParaSucc:1;
	Uint16   MsgBoxUpdateParaSucc:1;
	Uint16   MsgBoxCmdNoResponse:1;

	Uint16   Rsd:13;

};
union  MSGBOX_REG {
   Uint16             all;
   struct MSGBOX_BIT  bit;
};
/* 关键变量及状态字*/
typedef struct
{
	INT16S 	IdRef10mA;
	INT16S	IqRef10mA;
	INT16S	SpdRef;
	INT16S	SpdRefSlp;
	INT16S	SpdOut;
	INT16S	UdRef;
	INT16S	UqRef;
	INT16S 	IdFdb;
	INT16S	IqFdb;
	INT16S	SpdFdb;	
	Uint16  HpVol;
	INT16S	IBus;
	INT16S	Power;
	INT16S	LpVol;
	INT16S	TmpPdd;
	INT16S	TmpMotor;
	mode_t	CtrlMode;
	Uint32	FaultNow;
	Uint32	FaultOccurred;
	Uint32	WarnNow;
	Uint16	SoftErrCode; 		/* MC_PostCodeErr函数抛出的软件故障字*/
	Uint16	SoftWarnCode; 		/* MC_PostCodeWarn函数抛出的软件故障字*/
	Message_t  DiscardCmdMsg; 	/* MC_NotifyCmdDisCard函数通告的丢弃指令信号*/
	union  EEPROM_REG  Eeprom;	/* EEPROM加载结构指示*/
	union  GLOSTS_REG  GSts;	/*  全局状态量信号*/
	union  MSGBOX_REG  MsgBox;  /* 相应的事件发生时，向上位机发送一次标志，随后清0*/
	State_t	MainState;			/* 将状态结构体数据拷贝至此，以便上传 */
	Uint16	SubState;			/* 将状态结构体数据拷贝至此，以便上传*/
	INT16S LowSpdCapture1;
	INT16S LowSpdCapture2;
}stCriticalPara;

extern stTimeBase m1TimeBase;
extern stSysState m1State;
extern stCriticalPara m1CriPara;

extern bool MC_InitPara(void);
extern bool MC_InitMCCore(void);
extern bool MC_InitCriticalData(stCriticalPara* pC);

extern bool CfgSpdLoopCtler(stSpdLoopCtler* pD,stCtrlPara* pCtrl,stMotorPara*pMotor,stHalPara*pHal);
extern bool CfgBusVolDecoupler(stBusVolDecoupler* pD,stHalPara*pHal);
extern bool CfgCurrLoopCtler(stCurrLoopCtler* pD,stCtrlPara* pCtrl,stMotorPara*pMotor,stHalPara*pHal);
extern bool CfgCurrloopKpKi(stCurrLoopCtler* pD,INT16S IdKp,INT16S IdKi,INT16S IqKp,INT16S IqKi);
extern bool CfgFaultDealModule(stFaultDealModule* pD,stFaultPara* pF,stHalPara*pHal);
extern bool CfgResolverDealModule(stResolverDealModule* pD,stCurrLoopCtler* pC,stMotorPara*pMotor);
//extern bool CfgSmcObserver(stSmcObserver* pD,stCtrlPara* pCtrl,stMotorPara*pMotor,stHalPara*pHal,stSnsLessPara*pSnsLess);

extern void Delay_ms(Uint16 Time);
extern void Delay_us(Uint16 Time);
extern bool MC_SubStatePwrUpWait(stSysState* pState, stBusVolDecoupler* pBus,stFaultPara* pFault,stHalPara* pHal);

#endif
