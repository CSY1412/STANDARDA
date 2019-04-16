/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file      CONCLUDE_task.c
  * @brief     �ܽ������x��y���z�᷽���ϵ��ٶ�����
							 ������PID����͹��ʿ���
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

#include "CONCLUDE_task.h"
#include "math.h"
cm_motor_t cm_motor;


/**
  * @brief         	�ܽ�������
  * @author         ��ţ��ţ��
  * @param[in]      
  * @param[in]     
  * @param[in]      
  * @param[in]     
  * @retval       
  */
void CONCLUDE_task(void *pvParameters)
{		
	u32 lastWakeTime = GetSysTickCnt();  //��ȡϵͳʱ��
	while(1)        //��������
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));

		if (RATE_DO_EXECUTE(RATE_250_HZ, GetSysTickCnt()))  //250hz
			{
					if(system_stat!=preparing)   //������׼��״̬				
				{
					CalChasisAndSend();
				}
			}

	}
			
}


/**
  * @brief         ���̵���ٶȼ���ͷ���
  * @author        ��ţ��ţ��
  * @param[in]          
  * @retval       
  */
void CalChasisAndSend(void)  
{
		
	 chassis_set_contorl(&chasis_control.vx,&chasis_control.vy,gimbal_motor[0].cal_angle);
	
	 chassis_vector_to_mecanum_wheel_speed(chasis_control.vx,chasis_control.vy,
																					chasis_control.rotate,cm_motor.cm_temp);//�ٶȺϳ�
			
		for(int i =0;i<4;i++)
	{
		  cm_motor.cm_out[i]=	PID_Calc(&chassis_motor[i],chasis_motor[i].speed,cm_motor.cm_temp[i],MAX_M3508_CURRENT);
	}	

		
	  Set_CM_Speed(cm_motor.cm_out[0],cm_motor.cm_out[1],cm_motor.cm_out[2],cm_motor.cm_out[3]);//���͵������
}

/**
  * @brief         ����ٶȷ���������
  * @author        ��ţ��ţ��
  * @param[in]     4���������ת�ٱ�����ַ    
  * @retval       
  */		
void MotorSpeedAnalyze(float *wheel_speed)
{
		    //�������ӿ�������ٶȣ�������������ٶ�
	 fp32  temp=0.0f,max_vector = 0.0f, vector_rate = 0.0f;
	
    for (int i = 0; i < 4; i++)
    {				
        temp = fabs(wheel_speed[i]);
        if (max_vector < temp)
        {
            max_vector = temp;
        }
    }

    if (max_vector > MAX_WHEEL_SPEED)
    {
        vector_rate = MAX_WHEEL_SPEED / max_vector;
        for (int i = 0; i < 4; i++)
        {
            wheel_speed[i] *= vector_rate;
        }
    }

		
		
				
}
/**
  * @brief         ��������
  * @author        ��ţ��ţ��
  * @param[in]        
  * @retval       
  */		


void CurrentControl(void)
{
		if(judge_rece_mesg.power_heatdata.chassisPower>MAX_CHASIS_POWER)  //�����ʳ��������ʱ
	 {
//	    cm_motor.current_offset=DJI_PID_Cal(&chasis_motor_power_pid,judge_rece_mesg.power_heatdata.chassisPower,
//																								MAX_CHASIS_POWER,7000);
	
		 if(cm_motor.current_offset>0)
				 cm_motor.current_offset=0;
		  
	 }
	 else
	 {
			 cm_motor.current_offset=0; //ƫ������
	 }
	 
	 	first_order_filter_cali(&chasis_power_control, cm_motor.current_offset);
}



/**
  * @brief    �����ķ���ٶȷֽ�  
  * @author   ��ţ��ţ��
  * @param[in]         
  * @retval       
  */


static void chassis_vector_to_mecanum_wheel_speed(const fp32 vx_set, const fp32 vy_set, const fp32 wz_set, fp32 * wheel_speed)
{
    //��ת��ʱ�� ������̨��ǰ��������ǰ������ 0 ��1 ��ת���ٶȱ����� �������� 2,3 ��ת���ٶȱ��
    wheel_speed[0] = vx_set + vy_set + (CHASSIS_ROATE_SET_SCALE - 1.0f)* MOTOR_DISTANCE_TO_CENTER * wz_set;
    wheel_speed[1] = vx_set - vy_set + (CHASSIS_ROATE_SET_SCALE - 1.0f) * MOTOR_DISTANCE_TO_CENTER * wz_set;
    wheel_speed[2] = -vx_set + vy_set +(-CHASSIS_ROATE_SET_SCALE - 1.0f) * MOTOR_DISTANCE_TO_CENTER * wz_set;
    wheel_speed[3] = -vx_set - vy_set +(-CHASSIS_ROATE_SET_SCALE - 1.0f) * MOTOR_DISTANCE_TO_CENTER * wz_set;
}


/**
  * @brief    ����ת�� 
  * @author   С��ʦ
  * @param[in]         
  * @retval       
  */
static float rad(float input)
{
	return input*PI/180.0f;
}

/**
  * @brief    ����ϵת�� 
  * @author   С��ʦ
  * @param[in]         
  * @retval       
  */
void chassis_set_contorl(float *exp_vx,float *exp_vy,float econder_angle)
{
		float x = *exp_vx;
		float y = *exp_vy;

			*exp_vx = y * sin(rad(econder_angle)) + x * cos(rad(econder_angle));
	    *exp_vy = y * cos(rad(econder_angle)) - x * sin(rad(econder_angle));
}
