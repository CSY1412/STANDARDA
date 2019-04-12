/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file    SHOOT_task.c 
  * @brief    ������� 
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


#include "SHOOT_task.h"

shoot_t shoot;
//�������ź������


/**
  * @brief					����������
  * @author         ��ţ��ţ��
  * @param[in]         
  * @retval       
  */

#define ONE_BULLET_SHOOT_TIME 220

void SHOOT_task(void *pvParameters)    //״̬������
{
	u32 lastWakeTime = GetSysTickCnt();  //��ȡϵͳʱ��	

	while(1)        //��������
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));
				
		if (RATE_DO_EXECUTE(RATE_1000_HZ, GetSysTickCnt())) 
			{				
					Judege_Shoot();
			}
	}
}


void Judege_Shoot(void)
{
		 if(shoot_mode==point)  //����ģʽ
		 {
				if(shoot.exp_bullet>0)
					{
						  FeedMotorON();//
							vTaskDelay(ONE_BULLET_SHOOT_TIME);
							shoot.exp_bullet--;  //����
					}
					else
					{
							 FeedMotorOFF();					
					}
		 }
		 
		 		 if(shoot_mode==keep)  //����ģʽ
		 {
						  FeedMotorON(); //��������	
		 }
		 
		 
}

