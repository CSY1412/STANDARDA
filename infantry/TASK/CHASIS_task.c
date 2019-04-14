
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


//chasis_expspeed_t chasis_expspeed={0};//�ٶ�ƽ������
//chasis_expspeed_t key_expspeed={0};//�ٶ�ƽ������
chasis_control_t chasis_control;
chasis_control_t chasis_control_temp;


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
			if(confirmSystemStat(running))  //����ģʽ
			{	
					if (RATE_DO_EXECUTE(RATE_250_HZ, GetSysTickCnt()))  //250hz
				{
						ChasisControl();  //���̿���
				}			
			}
			else if(confirmSystemStat(self_stabilization)) //�ȶ�ģʽ
			{
				LockChasis();   //��������
			}
			
	#if COMPETETION		  //����ڱ���������ҲҪ��������
			else if(system_stat==breaking)
			{
				ChasisControl();  //���̿��� 
			}
	#else
			else if(confirmSystemStat(breaking))
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
			chasis_control.rotate=0;  //����ת��
		}
		if(control_mode==remote)//ң��ģʽ
		{
			
																	/*б��*/			
					  chasis_control.vx=first_order_filter_cali(&chassis_slow_set_vx,((cal_ch.ch0-1024)/660)*RC_CHASSIS_SPEED_X);  //ˮƽ�ٶ�����  
					  chasis_control.vy=first_order_filter_cali(&chassis_slow_set_vy,((cal_ch.ch1-1024)/660)*RC_CHASSIS_SPEED_Y); //��ֱ�ٶ�����
					 //ֹͣ�źţ�����Ҫ�������٣�ֱ�Ӽ��ٵ���
				#if SPEED_REDUCE_SLPOP
								if ((cal_ch.ch0-1024)==0)
									{
										chasis_control.vx = 0.0f;
									}
											if ((cal_ch.ch1-1024)==0)
									{
										chasis_control.vy = 0.0f;
									}
				#endif				
		}	
		
				if(control_mode==keyboard)//����ģʽ
		{				
				if(key_boad.key_W)chasis_control_temp.vy=KEY_CHASSIS_SPEED_Y;
						else if(key_boad.key_S)chasis_control_temp.vy=-KEY_CHASSIS_SPEED_Y;
							else chasis_control_temp.vy=0;
				if(key_boad.key_A)chasis_control_temp.vx=-KEY_CHASSIS_SPEED_X;
						else if(key_boad.key_D)chasis_control_temp.vx=KEY_CHASSIS_SPEED_X;
								else chasis_control_temp.vx=0;			
				if(key_boad.key_SHIFT&&key_boad.key_W)chasis_control_temp.vy=SHIFT_CHASSIS_SPEED_Y;  //shift����
					
			  chasis_control.vx=first_order_filter_cali(&chassis_slow_set_vx,chasis_control_temp.vx);  //ˮƽ�ٶ�����  
				chasis_control.vy=first_order_filter_cali(&chassis_slow_set_vy,chasis_control_temp.vy); //��ֱ�ٶ�����
					 //ֹͣ�źţ�����Ҫ�������٣�ֱ�Ӽ��ٵ���
				#if SPEED_REDUCE_SLPOP
								if ((!key_boad.key_A)&&(!key_boad.key_D))
									{
										  chasis_control.vx = 0.0f;
									}
											if ((!key_boad.key_W)&&(!key_boad.key_S))
									{
											chasis_control.vy = 0.0f;
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
		chasis_control.vx=0;
		chasis_control.vy=0;
		chasis_control.rotate=0;      
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
//			chasis_expspeed.rotate=DJI_PID_Cal(&chasis_follow_pid_p,(gimbal_motor[0].cal_angle-4096),chassis_follow_QE.out,QE_ROATE_SPEED_MAX);  //����м�ֵ
		}
		else if(key_boad.key_Q==DISSPRESS&&key_boad.key_E==PRESS)
		{
			chasis_follow_ecd_exp=MAX_CHASIS_FOLLOW_ECD;
			first_order_filter_cali(&chassis_follow_QE,chasis_follow_ecd_exp);	
//			chasis_expspeed.rotate=DJI_PID_Cal(&chasis_follow_pid_p,(gimbal_motor[0].cal_angle-4096),chassis_follow_QE.out,QE_ROATE_SPEED_MAX);  //����м�ֵ			
		}
		else
		{
			chasis_follow_ecd_exp=0;
			first_order_filter_cali(&chassis_follow_QE,chasis_follow_ecd_exp);	
//			chasis_expspeed.rotate=DJI_PID_Cal(&chasis_follow_pid_p,(gimbal_motor[0].cal_angle-4096),chassis_follow_QE.out,CHASIS_FOLLOW_ROATE_SPEED_MAX);  //����м�ֵ			
		}
		
		chasisSwingcontrol(&chasis_control.swing_angle);	

		
				
}







//ҡ��ԭ�ز���ҡ�����Ƕ�(rad)
#define SWING_NO_MOVE_ANGLE 0.7f
//ҡ�ڹ��̵����˶����Ƕ�(rad)
#define SWING_MOVE_ANGLE 0.31415926535897932384626433832795f


/**
  * @brief          ���̸�����̨����Ϊ״̬���£�����ģʽ�Ǹ�����̨�Ƕȣ�������ת�ٶȻ���ݽǶȲ���������ת�Ľ��ٶ�
  * @author         RM
  * @param[in]      angle_set��������̨���Ƶ�����ԽǶ�
  * @param[in]      chassis_move_rc_to_vector��������
  * @retval         ���ؿ�
  */

void chasisSwingcontrol(fp32 *angle_set)
{
    //ҡ�ڽǶ�������sin�������ɣ�swing_time ��sin����������ֵ
    static fp32 swing_time = 0.0f;
    //swing_time �Ǽ�������ĽǶ�
    static fp32 swing_angle = 0.0f;
    //max_angle ��sin�����ķ�ֵ
    static fp32 max_angle = SWING_NO_MOVE_ANGLE;
    //add_time ��ҡ�ڽǶȸı�Ŀ��������Խ��
    static fp32 const add_time = PI / 250.0f;
    //ʹ��ҡ�ڱ�־λ
    static uint8_t swing_flag = 0;
	
    //�ж��Ƿ�Ҫҡ��
    if (this_key_valu.key_CTRL)  //�������CTRL
    {
        if (swing_flag == 0)
        {
            swing_flag = 1;
            swing_time = 0.0f;
        }
    }
    else
    {
        swing_flag = 0;
    }

    //�жϼ��������ǲ����ڿ��Ƶ����˶����������˶���Сҡ�ڽǶ�
    if (key_boad.key_W||key_boad.key_A||
        key_boad.key_S||key_boad.key_D)
    {
        max_angle = SWING_MOVE_ANGLE;
    }
    else
    {
        max_angle = SWING_NO_MOVE_ANGLE;
    }
    //sin�������ɿ��ƽǶ�
    if (swing_flag)
    {
        swing_angle = max_angle * arm_sin_f32(swing_time);
        swing_time += add_time;
    }
    else
    {
        swing_angle = 0.0f;
    }
    //sin����������2pi
    if (swing_time > 2 * PI)
    {
        swing_time -= 2 * PI;
    }
    *angle_set = swing_angle;
}


//����ң�������������
void chassis_set_contorl()
{
    //�����ٶ�
  fp32 vx_set = 0.0f, vy_set = 0.0f, angle_set = 0.0f; 
	fp32 sin_yaw = 0.0f, cos_yaw = 0.0f;
	//��ת���Ƶ����ٶȷ��򣬱�֤ǰ����������̨�����������˶�ƽ��
	sin_yaw = arm_sin_f32(-gimbal_motor[0].cal_angle);
	cos_yaw = arm_cos_f32(-gimbal_motor[0].cal_angle);
	vx_set = cos_yaw * vx_set + sin_yaw * vy_set;
	vy_set = -sin_yaw * vx_set + cos_yaw * vy_set;
//	//���ÿ��������̨�Ƕ�
//	chassis_move_control->chassis_relative_angle_set = rad_format(angle_set);
//	//������תPID���ٶ�
//	chassis_move_control->wz_set = -PID_Calc(&chassis_move_control->chassis_angle_pid, chassis_move_control->chassis_yaw_motor->relative_angle, chassis_move_control->chassis_relative_angle_set);
//	//�ٶ��޷�
//	chassis_move_control->vx_set = fp32_constrain(chassis_move_control->vx_set, chassis_move_control->vx_min_speed, chassis_move_control->vx_max_speed);
//	chassis_move_control->vy_set = fp32_constrain(chassis_move_control->vy_set, chassis_move_control->vy_min_speed, chassis_move_control->vy_max_speed);



}
