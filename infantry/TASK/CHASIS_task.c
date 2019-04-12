
/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file      CHASIS_task.c
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
#include "CHASIS_task.h"


chasis_expspeed_t chasis_expspeed={0};//�ٶ�ƽ������
chasis_expspeed_t key_expspeed={0};//�ٶ�ƽ������

float chasis_follow_ecd_exp=0;


/**
  * @brief    ����������     
  * @author   ��ţ��ţ��
  * @param[in]      
  * @param[in]     
  * @param[in]      
  * @param[in]     
  * @retval       
  */

void CHASIS_task(void *pvParameters)
{
	u32 lastWakeTime = GetSysTickCnt();  //��ȡϵͳʱ��
	while(1)        //��������
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));
			if(system_stat==running)  //����ģʽ
			{	
						if (RATE_DO_EXECUTE(RATE_250_HZ, GetSysTickCnt()))  //250hz
					{
							ChasisControl();  //���̿���
					}			
			}
			else if(system_stat==self_stabilization) //�ȶ�ģʽ
			{
				LockChasis();   //��������
			}
			
	#if COMPETETION		  //����ڱ���������ҲҪ��������
			else if(system_stat==breaking)
			{
				ChasisControl();  //���̿��� 
			}
	#else
			else if(system_stat==breaking)
			{
				Set_CM_Speed(0,0,0,0);
				BeepON();	
			}			
	#endif			
			else
			{
				Set_CM_Speed(0,0,0,0);  //�ͷŵ��			
			}
		
	}
}


/**
  * @brief    ���̿���ʵ�ֺ���     
  * @author   ��ţ��ţ��
  * @param[in]           
  * @retval       
  */

void ChasisControl(void)
{
		if(chasis_follow_stat==follow_gimabal)  //���������̨����״̬
		{
				ChasisRoteCal();
		}		
		if(chasis_follow_stat==self_move)    //��̨������
		{
			chasis_expspeed.rotate=0;  //����ת��
		}
		if(control_mode==remote)//ң��ģʽ
		{
																	/*б��*/			
					chasis_expspeed.x=first_order_filter_cali(&chassis_slow_set_vx,(cal_ch.ch0-1024)*7);  //ˮƽ�ٶ�����  
					chasis_expspeed.y=first_order_filter_cali(&chassis_slow_set_vy,(cal_ch.ch1-1024)*12); //��ֱ�ٶ�����
					 //ֹͣ�źţ�����Ҫ�������٣�ֱ�Ӽ��ٵ���
				#if SPEEDREDUCESLOP
								if ((cal_ch.ch0-1024)==0)
									{
											chasis_expspeed.x = 0.0f;
									}
											if ((cal_ch.ch1-1024)==0)
									{
											chasis_expspeed.y = 0.0f;
									}
				#endif				
		}			
				if(control_mode==keyboard)//ң��ģʽ
		{
																	/*б��*/			
					
				if(key_boad.key_W)key_expspeed.y=5000;else if(key_boad.key_S)key_expspeed.y=-5000;else key_expspeed.y=0;
				if(key_boad.key_A)key_expspeed.x=-5000;else if(key_boad.key_D)key_expspeed.x=5000;else key_expspeed.x=0;
				
				if(key_boad.key_SHIFT&&key_boad.key_W)key_expspeed.y=SHIFT_SPEEDMAX;  //shift����
				
			
					chasis_expspeed.x=first_order_filter_cali(&chassis_slow_set_vx,key_expspeed.x);  //ˮƽ�ٶ�����  
					chasis_expspeed.y=first_order_filter_cali(&chassis_slow_set_vy,key_expspeed.y); //��ֱ�ٶ�����
					 //ֹͣ�źţ�����Ҫ�������٣�ֱ�Ӽ��ٵ���
				#if SPEEDREDUCESLOP
								if ((cal_ch.ch0-1024)==0)
									{
											chasis_expspeed.x = 0.0f;
									}
											if ((cal_ch.ch1-1024)==0)
									{
											chasis_expspeed.y = 0.0f;
									}
				#endif				
		}	
		
}

/**
  * @brief    ��������     
  * @author   ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void LockChasis(void) 
{
		chasis_expspeed.x=0;
		chasis_expspeed.y=0;
		chasis_expspeed.rotate=0;      
}



/**
  * @brief    ������ת������     
  * @author   ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void ChasisRoteCal(void)
{
	if(key_boad.key_Q==PRESS&&key_boad.key_E==DISSPRESS)
		{
			chasis_follow_ecd_exp=-MAX_CHASIS_FOLLOW_ECD;
			first_order_filter_cali(&chassis_follow_QE,chasis_follow_ecd_exp);	
			chasis_expspeed.rotate=DJI_PID_Cal(&chasis_follow_pid_p,(gimbal_motor[0].cal_angle-4096),chassis_follow_QE.out,QE_ROATE_SPEED_MAX);  //����м�ֵ
		}
		else if(key_boad.key_Q==DISSPRESS&&key_boad.key_E==PRESS)
		{
			chasis_follow_ecd_exp=MAX_CHASIS_FOLLOW_ECD;
			first_order_filter_cali(&chassis_follow_QE,chasis_follow_ecd_exp);	
			chasis_expspeed.rotate=DJI_PID_Cal(&chasis_follow_pid_p,(gimbal_motor[0].cal_angle-4096),chassis_follow_QE.out,QE_ROATE_SPEED_MAX);  //����м�ֵ			
		}
		else
		{
			chasis_follow_ecd_exp=0;
			first_order_filter_cali(&chassis_follow_QE,chasis_follow_ecd_exp);	
			chasis_expspeed.rotate=DJI_PID_Cal(&chasis_follow_pid_p,(gimbal_motor[0].cal_angle-4096),chassis_follow_QE.out,CHASIS_FOLLOW_ROATE_SPEED_MAX);  //����м�ֵ			
		}		
				
}



