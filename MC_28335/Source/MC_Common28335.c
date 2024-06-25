#include"MCSoft_DataType.h"



void Delay_us(Uint16 Time)
{
	volatile Uint16 i = 0;
	volatile Uint16 j = 0;
	for(i = 0;i <= Time;i++)
	{
		for(j = 0;j<6;j++)
		{
			
		}
	}
}
void Delay_ms(Uint16 Time)
{
	volatile Uint16 i = 0;
	for(i = 0;i <= Time;i++)
	{
		Delay_us(1000);
	}
}
