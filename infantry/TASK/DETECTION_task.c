/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file      �������
  * @brief     
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
#include "DETECTION_task.h"
slef_check_count_t slef_check_count;



/**
  * @brief    �����������    
  * @author   ��ţ��ţ��
  * @param[in]      
  * @retval       
  */

void DETECTION_task(void *pvParameters)   
{
	u32 lastWakeTime = GetSysTickCnt();  //��ȡϵͳʱ��
	while(1)        //��������
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));
		if (RATE_DO_EXECUTE(RATE_100_HZ, GetSysTickCnt()))  //100hz
			{
				DetectConnect(GetSysTickCnt());
				
			}						
	}
}

/**
  * @brief   	   �������  
  * @author   	��ţ��ţ��
  * @param[in]   ʱ���   
  * @retval       
  */

void DetectConnect(u32 DT)
{
			if(slef_check_count.chasis_motor1==0)
			{
				slef_check_count.stat|=0x0001;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("motor1ʧȥ����");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFFE;  //����
			}
			
				if(slef_check_count.chasis_motor2==0)
			{
				slef_check_count.stat|=0x0002;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("motor2ʧȥ����\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFFD;  //����
			}
			
				 if(slef_check_count.chasis_motor3==0)
			{
				slef_check_count.stat|=0x0004;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("motor3ʧȥ����\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFFB;  //����
			}
	
				if(slef_check_count.chasis_motor4==0)
			{
				slef_check_count.stat|=0x0008;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("motor4ʧȥ����\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFF7;  //����
			}

				if(slef_check_count.gimbal_yaw==0)
			{
				slef_check_count.stat|=0x0010;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("yaw����ʧȥ����\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFEF;  //����
			}

				if(slef_check_count.gimbal_pitch==0)
			{
				slef_check_count.stat|=0x0020;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("pitch����ʧȥ����\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFDF;  //����
			}	
			
			 if(slef_check_count.gesture_sensor==0)
			{
				slef_check_count.stat|=0x0040;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("��̬������ʧȥ����\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFBF;  //����
			}	
			
		 if(slef_check_count.rc==0)
			{
				slef_check_count.stat|=0x0080;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("ң����ʧȥ����\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFF7F;  //����
			}

		 if(slef_check_count.aux_board==0)
			{
				slef_check_count.stat|=0x0100;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("���ذ�ʧȥ����\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFEFF;  //����
			}				
										
				ClearCount();  //���������ֵ
}


/**
  * @brief    ���������     
  * @author   ��ţ��ţ��
  * @param[in]          
  * @retval       
  */

void ClearCount(void)
{
		slef_check_count.chasis_motor1=0;
		slef_check_count.chasis_motor2=0;
		slef_check_count.chasis_motor3=0;
		slef_check_count.chasis_motor4=0;
		slef_check_count.gimbal_yaw=0;
		slef_check_count.gimbal_pitch=0;
		slef_check_count.gesture_sensor=0;
		slef_check_count.rc=0;
		slef_check_count.aux_board=0;	
}
