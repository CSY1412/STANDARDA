/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file    
  * @brief    ������������
  * @note      
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     ��ţ��ţ��     1. ���
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  */


#include "CONFIG_PARAM_task.h"

SemaphoreHandle_t  config_param_xSemaphore; //�ź���


void CONFIG_PARAM_Task(void* param)
{
	static u32 semaphoreGiveTime = 0;  
	semaphoreGiveTime = GetSysTickCnt();   //�ϴ��ͷ�ʱ��
	config_param_xSemaphore = xSemaphoreCreateBinary();  //����flash�ź���	
	while(1) 
	{	

			xSemaphoreTake(config_param_xSemaphore, portMAX_DELAY);
		
			if (GetSysTickCnt() - semaphoreGiveTime > 2000)//�ϴ��ͷ��ź���ʱ�����ٴ���2���дFlash����ֹƵ��д��
			{
				semaphoreGiveTime = GetSysTickCnt();
				Flash_Data_Save();  //����flash
				
			}
			else
			{
				xSemaphoreTake(config_param_xSemaphore, 100);
			}
	}
}




