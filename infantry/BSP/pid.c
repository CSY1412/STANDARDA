#include "pid.h"

PID_Regulator_t chasis_motor1_pid_v;
PID_Regulator_t chasis_motor2_pid_v;
PID_Regulator_t chasis_motor3_pid_v;
PID_Regulator_t chasis_motor4_pid_v;

PID_Regulator_t gimbal_pitch_pid_v;
PID_Regulator_t gimbal_pitch_pid_p;
PID_Regulator_t gimbal_yaw_pid_v;
PID_Regulator_t gimbal_yaw_pid_p;

PID_Regulator_t gimbal_back_yaw_pid_p;//复位pid

PID_Regulator_t chasis_follow_pid_p;

PID_Regulator_t gimbal_yaw_slef_aim_pid;
PID_Regulator_t gimbal_pitch_slef_aim_pid;


PID_Regulator_t chasis_motor_power_pid;



void DJI_PID_Param_Init(void)
{
	DJI_PID_Init(&chasis_motor1_pid_v,0,0,0,16384,2000,2000,CHASIS_MOTOR_MAX_CURRENT);   //底盘电机
	DJI_PID_Init(&chasis_motor2_pid_v,0,0,0,16384,2000,2000,CHASIS_MOTOR_MAX_CURRENT);
	DJI_PID_Init(&chasis_motor3_pid_v,0,0,0,16384,2000,2000,CHASIS_MOTOR_MAX_CURRENT);
	DJI_PID_Init(&chasis_motor4_pid_v,0,0,0,16384,2000,2000,CHASIS_MOTOR_MAX_CURRENT);
	
	DJI_PID_Init(&gimbal_pitch_pid_v,0,0,0,4500,3000,4000,GIMBAL6623_MOTOR_MAX_CURRENT); 
	DJI_PID_Init(&gimbal_pitch_pid_p,0,0,0,4500,3000,4000,GIMBAL6623_MOTOR_MAX_CURRENT); 
	
	DJI_PID_Init(&gimbal_yaw_pid_v,0,0,0,4500,3000,4000,GIMBAL6623_MOTOR_MAX_CURRENT);   
	DJI_PID_Init(&gimbal_yaw_pid_p,0,0,0,4500,3000,4000,GIMBAL6623_MOTOR_MAX_CURRENT);
  
	DJI_PID_Init(&gimbal_back_yaw_pid_p,0,0,0,4500,3000,4000,GIMBAL6623_MOTOR_MAX_CURRENT); //复位PID
  
	DJI_PID_Init(&chasis_follow_pid_p,0,0,0,16384,2000,2000,CHASIS_MOTOR_MAX_CURRENT);   //底盘电机
	
	DJI_PID_Init(&gimbal_pitch_slef_aim_pid,0,0,0,5000,5000,5000,GIMBAL6623_MOTOR_MAX_CURRENT);
	DJI_PID_Init(&gimbal_yaw_slef_aim_pid,0,0,0,5000,5000,5000,GIMBAL6623_MOTOR_MAX_CURRENT);
	
	DJI_PID_Init(&chasis_motor_power_pid,0,0,0,5000,5000,5000,CHASIS_MOTOR_MAX_CURRENT);

}


void DJI_PID_Init(PID_Regulator_t* PID,float kp,float ki,float kd,
									float kpmax,float kimax,float kdmax,float outmax)
{
		PID->ref=0;
		PID->fdb=0;
		PID->err[0]=0;
		PID->err[1]=0;	
		PID-> kp_offset=kp;
		PID-> ki_offset=ki;
		PID-> kd_offset=kd;
		PID->componentKp=0;
		PID->componentKi=0;
		PID->componentKd=0;
		PID->componentKpMax=kpmax;
		PID->componentKiMax=kimax;
		PID->componentKdMax=kdmax;
		PID->output=0;
		PID->outputMax=outmax;
		PID->kp_offset=0;
		PID->ki_offset=0;
		PID->kd_offset=0;	
}

float DJI_PID_Cal(PID_Regulator_t* PID,float measure,float exp,float outmax)
{
		PID->ref=exp;
		PID->fdb=measure;
		PID->outputMax=outmax;
		PID_Calc(PID);
		return PID->output;
}



/*************************
      初始化PID参数
*************************/
void PID_Init(PID_Struct *PID,float kp,float ki,float kd,
	            float out_max,float out_min,
							float integrate_max,float integrate_min,
							float sumerr_max,float sumerr_min)
{
  PID->expectation       = 0.0;            //遥控给的期望值
  PID->Err_k			       = 0.0;            //当前误差值e(k)
  PID->Err_k_1		       = 0.0;            //k-1时刻误差值e(k-1)
  PID->Err_k_2		       = 0.0;            //k-2时刻误差值e(k-2)
  PID->SumErr              = 0.0;		//误差和
	
  PID->Kp				   = kp;           //比例系数，通过串口在线调PID参数再写入Flash
  PID->Ti				   = ki;           //积分系数，通过串口在线调PID参数再写入Flash
  PID->Td				   = kd;           //微分系数，通过串口在线调PID参数再写入Flash
  PID->Ouput_deltaUk       = 0.0;		    //PID计算后的输出量U(k) - U(k-1)
  PID->Ouput_deltaUk_Max   = out_max;		    //限制输出量最大值
  PID->Ouput_deltaUk_Min   = out_min;		    //限制输出量最小值
  PID->PID_Integral_Max    = integrate_max;		     //限制积分项最大值
  PID->PID_Integral_Min    = integrate_min;			//限制积分项最小值
	PID->SumErr_Max=sumerr_max;
	PID->SumErr_Min=sumerr_min;
	
}


void PidInit(void)
{
	
	
	
}

/***************************************************************
PID计算-增量式
***************************************************************/
float PIDz_Calculate(PID_Struct* PID, float measured, float expect)
{
     float K_p = PID->Kp;
     float T_d = PID->Td;
     float T_i = PID->Ti;
     float increment;
    PID->Err_k=PID->Err_k_1;
    PID->Err_k_1=PID->Err_k_2;
    PID->Err_k_2=(expect-measured)*10;
    increment = (K_p+T_i+T_d)*PID->Err_k_2-(K_p+2*T_d)*PID->Err_k_1+T_d*PID->Err_k;
    PID->Ouput_deltaUk+=increment;
    if(PID->Ouput_deltaUk > PID->Ouput_deltaUk_Max)
    {
        PID->Ouput_deltaUk = PID->Ouput_deltaUk_Max;
    }
    if(PID->Ouput_deltaUk < PID->Ouput_deltaUk_Min)
    {
        PID->Ouput_deltaUk = PID->Ouput_deltaUk_Min;
    }
    
  return PID->Ouput_deltaUk;
}

/***************************************************************
PID计算-位置式
***************************************************************/
float PID_Calculate(PID_Struct* PID, float measured, float expect)
{
  float Value_Proportion;  
  float Value_Integral;		
  float Value_Derivative;	

  PID->expectation =  expect;
  PID->Err_k = (PID->expectation - measured)*10;
  PID->SumErr+= PID->Err_k;
	if(PID->SumErr > PID->SumErr_Max )
	{
		PID->SumErr=PID->SumErr_Max;
	}
	if(PID->SumErr < PID->SumErr_Min )
	{
		PID->SumErr=-5000;
	}
  //P I D
  Value_Proportion    = 10*PID->Kp * PID->Err_k;
  Value_Integral      =  PID->SumErr * PID->Ti;	
  Value_Derivative  = PID->Kp * PID->Td * (PID->Err_k - PID->Err_k_1);

  if(Value_Integral > PID->PID_Integral_Max)
  {
    PID->SumErr -= PID->Err_k;
	Value_Integral = PID->PID_Integral_Max;
  }
  if(Value_Integral < PID->PID_Integral_Min)
  {
  	PID->SumErr -= PID->Err_k;
    Value_Integral = PID->PID_Integral_Min;
  }
  
  PID->Ouput_deltaUk = Value_Proportion +  Value_Integral + Value_Derivative;

  if(PID->Ouput_deltaUk > PID->Ouput_deltaUk_Max)
  {PID->Ouput_deltaUk = PID->Ouput_deltaUk_Max;}
  if(PID->Ouput_deltaUk < PID->Ouput_deltaUk_Min)
  {PID->Ouput_deltaUk = PID->Ouput_deltaUk_Min;}

  PID->Err_k_1 = PID->Err_k;	  //保存k-1次误差值
  
  return PID->Ouput_deltaUk;
}