/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file    
  * @brief    参数保存任务
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


#include "CONFIG_PARAM_task.h"

SemaphoreHandle_t  config_param_xSemaphore; //信号量


void CONFIG_PARAM_Task(void* param)
{
	static u32 semaphoreGiveTime = 0;  
	semaphoreGiveTime = GetSysTickCnt();   //上次释放时间
	config_param_xSemaphore = xSemaphoreCreateBinary();  //创建flash信号量	
	while(1) 
	{	

			xSemaphoreTake(config_param_xSemaphore, portMAX_DELAY);
		
			if (GetSysTickCnt() - semaphoreGiveTime > 2000)//上次释放信号量时间至少大于2秒才写Flash，防止频繁写入
			{
				semaphoreGiveTime = GetSysTickCnt();
				Flash_Data_Save();  //保存flash
				
			}
			else
			{
				xSemaphoreTake(config_param_xSemaphore, 100);
			}
	}
}




