/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file      监测任务
  * @brief     
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
#include "DETECTION_task.h"
slef_check_count_t slef_check_count;



/**
  * @brief    监测任务主体    
  * @author   蜗牛蜗牛跑
  * @param[in]      
  * @retval       
  */

void DETECTION_task(void *pvParameters)   
{
	u32 lastWakeTime = GetSysTickCnt();  //获取系统时间
	while(1)        //调试内容
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));
		if (RATE_DO_EXECUTE(RATE_100_HZ, GetSysTickCnt()))  //100hz
			{
				DetectConnect(GetSysTickCnt());
				
			}						
	}
}

/**
  * @brief   	   检测连接  
  * @author   	蜗牛蜗牛跑
  * @param[in]   时间戳   
  * @retval       
  */

void DetectConnect(u32 DT)
{
			if(slef_check_count.chasis_motor1==0)
			{
				slef_check_count.stat|=0x0001;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("motor1失去连接");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFFE;  //置零
			}
			
				if(slef_check_count.chasis_motor2==0)
			{
				slef_check_count.stat|=0x0002;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("motor2失去连接\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFFD;  //置零
			}
			
				 if(slef_check_count.chasis_motor3==0)
			{
				slef_check_count.stat|=0x0004;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("motor3失去连接\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFFB;  //置零
			}
	
				if(slef_check_count.chasis_motor4==0)
			{
				slef_check_count.stat|=0x0008;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("motor4失去连接\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFF7;  //置零
			}

				if(slef_check_count.gimbal_yaw==0)
			{
				slef_check_count.stat|=0x0010;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("yaw轴电机失去连接\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFEF;  //置零
			}

				if(slef_check_count.gimbal_pitch==0)
			{
				slef_check_count.stat|=0x0020;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("pitch轴电机失去连接\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFDF;  //置零
			}	
			
			 if(slef_check_count.gesture_sensor==0)
			{
				slef_check_count.stat|=0x0040;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("姿态传感器失去连接\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFFBF;  //置零
			}	
			
		 if(slef_check_count.rc==0)
			{
				slef_check_count.stat|=0x0080;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("遥控器失去连接\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFF7F;  //置零
			}

		 if(slef_check_count.aux_board==0)
			{
				slef_check_count.stat|=0x0100;
				if (RATE_DO_EXECUTE(RATE_1_HZ, DT))  //1hz
				{
					printf("辅控板失去连接\n");		
				}	
			}
			else
			{
				slef_check_count.stat&=0xFEFF;  //置零
			}				
										
				ClearCount();  //清除计数器值
}


/**
  * @brief    清零计数器     
  * @author   蜗牛蜗牛跑
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
