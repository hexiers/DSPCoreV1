#include"MC_Include.h"
#ifndef SmcObserverExt_H_H
#define SmcObserverExt_H_H

#include"IQmathLib.h"

typedef struct
{
	INT32S UAlpha_UseQ16;   /*Alpha轴电压，Q16格式*/
	INT32S UBeta_UseQ16;    /*Beta轴电压，Q16格式*/
	INT16S fsmopos;	        /*f参数*/  				
	INT16S gsmopos;			/*g参数*/  		
	INT64S Alpha_F_ResultQ16;   /*f参数相乘后的结果，Alpha轴，Q16格式*/  	
	INT64S Alpha_G_ResultQ16;	/*g参数相乘后的结果，Alpha轴，Q16格式*/  
	INT32S EstIalphaQ16;    /*Alpha轴估算电流，Q16格式*/  
	INT64S Beta_F_ResultQ16;	/*f参数相乘后的结果，Beta轴，Q16格式*/  
	INT64S Beta_G_ResultQ16;	/*f参数相乘后的结果，Beta轴，Q16格式*/  
	INT32S EstIbetaQ16;		/*Beta轴估算电流，Q16格式*/
	INT32S IAlpha_UseQ16;	/*Alpha轴电流，Q16格式*/
	INT32S IBeta_UseQ16;	/*Beta轴电流，Q16格式*/
	INT32S IAlphaErrQ16;	/*Alpha轴电流误差，Q16格式*/
	INT32S IBetaErrQ16;		/*Beta轴电流误差，Q16格式*/
	INT16S SlideGain;		/*滑模控制器增益*/					
	INT32S SlideMax;		/*滑模控制器最大输出量*/			
	INT64S zAlphaQ16;       /*当前周期的滑模控制器输出量，Alpha轴，Q16格式*/	
	INT32S zAlphaOldQ16;	/*上一周期的滑模控制器输出量，Alpha轴，Q16格式*/	
	INT32S eAlphaQ16; 		/*当前周期的反电势，Alpha轴，Q16格式*/	
	INT32S eAlphaOldQ16; 	/*上一周期的反电势，Alpha轴，Q16格式*/
	INT64S zBetaQ16; 		/*当前周期的滑模控制器输出量，Beta轴，Q16格式*/	
	INT32S zBetaOldQ16; 	/*上一周期的滑模控制器输出量，Beta轴，Q16格式*/
	INT32S eBetaQ16; 		/*当前周期的反电势，Beta轴，Q16格式*/	
	INT32S eBetaOldQ16; 	/*上一周期的反电势，Beta轴，Q16格式*/
	INT32S eAlphaFinalQ16; 	/*两次滤波后的反电势，Alpha轴，Q16格式*/	
	INT32S eBetaFinalQ16; 	/*两次滤波后的反电势，Beta轴，Q16格式*/	
	INT16S	kslfMin;		/*自适应滤波器滤波系数最小限制值*/			
	INT16S	BitPCycleToElecRpmCoef; 		/*单周期角度值增量对应电气转速的转换系数*/			
	INT32S  thetaQ16;		/*电气角度值，Q16格式*/			
	INT16S	thetaINT;		/*当前周期的电气角度值，定点格式*/			
	INT16S	thetaINTOld;	/*上一周期的电气角度值，定点格式*/				
	INT16S  DeltaTheta[16];	/*单周期电角度值增量，定点格式，用作16个周期的滑动平均滤波*/			
	INT16S  ArrayIndex;		/*数组索引号*/		
	INT32S  DeltaThetaSum;	/*16个周期的电角度增量和，定点格式*/		
	INT32S  OmegaBitQ16;	/*当前周期的电角度增量，Q16格式*/	
	INT32S  OmegaBitOldQ16;	/*上一周期的电角度增量，Q16格式*/	
	INT32S  OmegaBitFilterdQ16;	/*滤波后的电角度增量，Q16格式*/	
	INT16S	thetaOffset;		/*需补偿的角度值*/		
	INT16S	SmcElecPos; 		/*观测出的转子电角度*/			
	INT16S	Kslf;				/*滑模观测器自适应滤波器的滤波系数*/					 
	INT16S	SmcElecRpm; 		/*电气转速*/				

}stSmcObserverExt;

extern   stSmcObserverExt  m1SmcObserverExt;
extern void SMC_Position_Estimation(void);
//extern bool SMC_Init(stSmcObserverExt* pT,stCtrlPara* pCtrl,stMotorPara*pMotor,stHalPara*pHal,stSnsLessPara*pSnsLess);

#endif











