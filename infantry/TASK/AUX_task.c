/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file    
  * @brief    辅助任务
  * @note      
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     蜗牛蜗牛跑     1. 完成
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  */

#include "AUX_task.h"


void AUX_task(void *pvParameters)
{
    u32 lastWakeTime = GetSysTickCnt();  //获取系统时间
    while(1)        //调试内容
    {
        vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));
        if (RATE_DO_EXECUTE(RATE_250_HZ, GetSysTickCnt()))  //250hz
        {
            BeepAlam();					
        }
				
				if (RATE_DO_EXECUTE(RATE_10_HZ, GetSysTickCnt()))  //闪烁绿灯
				{
						LightingGreenLED();				
				}
				
				if (RATE_DO_EXECUTE(RATE_5_HZ, GetSysTickCnt()))
				{
						LEDAlam();
				}
				
    }
}



u8 yellow_led_count=0;
u8 blue_led_count=0;
u8 red_led_count=0;
u8 green_led_count=0;
void LEDAlam(void)
{
		if(yellow_led_count>0)
		{
			LED_YELLOW=!LED_YELLOW;
			yellow_led_count--;
			if(yellow_led_count==0)
			{
				LED_YELLOW=1;
			}
		}
		
				if(blue_led_count>0)
		{
			LED_BLUE=!LED_BLUE;
			blue_led_count--;
			if(blue_led_count==0)
			{
				LED_BLUE=1;
			}
		}
		
				if(red_led_count>0)
		{
			LED_RED=!LED_RED;
			red_led_count--;
			if(red_led_count==0)
			{
				LED_RED=1;
			}
		}
		
				if(green_led_count>0)
		{
			LED_GREEN=!LED_GREEN;
			green_led_count--;
			if(green_led_count==0)
			{
				LED_GREEN=1;
			}
		}		
}

void BlueLED(u8 count)
{
		blue_led_count=count*2;
}
void YellowLED(u8 count)
{
		yellow_led_count=count*2;
}
void RedLED(u8 count)
{
		red_led_count=count*2;
}
void GreenLED(u8 count)
{
		green_led_count=count*2;
}
