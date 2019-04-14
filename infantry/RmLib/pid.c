/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       pid.c/h
  * @brief      pid实现函数，包括初始化，PID计算函数，
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. 完成
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2016 DJI****************************
  */

#include "pid.h"

PidTypeDef chassis_motor[4];  //底盘电机
PidTypeDef gimbal_speed[2];
PidTypeDef gimbal_position[2];
PidTypeDef chasis_follow;

#define LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }

void PID_Init(PidTypeDef *pid, uint8_t mode,fp32 kp,fp32 ki,fp32 kd, fp32 max_out, fp32 max_iout)
{
    if (pid == NULL)
    {
        return;
    }
    pid->mode = mode;
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->max_out = max_out;
    pid->max_iout = max_iout;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->error[0] = pid->error[1] = pid->error[2] = pid->Pout = pid->Iout = pid->Dout = pid->out = 0.0f;
}

fp32 PID_Calc(PidTypeDef *pid, fp32 ref, fp32 set,float outmax)
{
    if (pid == NULL)
    {
        return 0.0f;
    }
		pid->max_out=outmax;
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->set = set;
    pid->fdb = ref;
    pid->error[0] = set - ref;
    if (pid->mode == PID_POSITION)
    {
        pid->Pout = pid->Kp * pid->error[0];
        pid->Iout += pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - pid->error[1]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        LimitMax(pid->Iout, pid->max_iout);
        pid->out = pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    else if (pid->mode == PID_DELTA)
    {
        pid->Pout = pid->Kp * (pid->error[0] - pid->error[1]);
        pid->Iout = pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - 2.0f * pid->error[1] + pid->error[2]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        pid->out += pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    return pid->out;
}

void PID_clear(PidTypeDef *pid)
{
    if (pid == NULL)
    {
        return;
    }

    pid->error[0] = pid->error[1] = pid->error[2] = 0.0f;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->out = pid->Pout = pid->Iout = pid->Dout = 0.0f;
    pid->fdb = pid->set = 0.0f;
}




void allPIDInit(void)
{
	for(int a=0;a<4;a++)
	{
		PID_Init(&chassis_motor[a],PID_DELTA,flash_save_data_access.chasis_motor[a].kp*1000,
													 flash_save_data_access.chasis_motor[a].ki*1000,
													 flash_save_data_access.chasis_motor[a].kd*1000,
													 MAX_M3508_CURRENT,MAX_M3508_CURRENT/5);
	}	
	 PID_Init(&gimbal_speed[0],PID_DELTA,flash_save_data_access.gimbal_pitch_v.kp,
													 flash_save_data_access.gimbal_pitch_v.ki,
													 flash_save_data_access.gimbal_pitch_v.kd,
													 MAX_M6623_CURRENT,MAX_M6623_CURRENT/5);
	
	 PID_Init(&gimbal_position[0],PID_POSITION,flash_save_data_access.gimbal_pitch_p.kp,
													 flash_save_data_access.gimbal_pitch_p.ki,
													 flash_save_data_access.gimbal_pitch_p.kd,
													 YAW_ROTATE_SPEED_MAX,YAW_ROTATE_SPEED_MAX/5);
}
