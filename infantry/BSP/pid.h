#ifndef __PID_H
#define __PID_H
#include "main.h"


#define CHASIS_MOTOR_MAX_CURRENT 8000
#define GIMBAL6623_MOTOR_MAX_CURRENT 4200
#define CHASIS_MOTOR_MAX_SPEED 7900.0f

typedef struct {
  float expectation;          
  float Err_k;			  
  float Err_k_1;		  
  float Err_k_2;		 
  float SumErr;
	float SumErr_Max;  //误差和最大
	float SumErr_Min;  //误差和最大
  float Kp;				 
  float Ti;				
  float Td;				 
  float Ouput_deltaUk;		
  float Ouput_deltaUk_Max;		
  float Ouput_deltaUk_Min;		
  float PID_Integral_Max;				
  float PID_Integral_Min;				
} PID_Struct;

extern PID_Regulator_t chasis_motor1_pid_v;
extern PID_Regulator_t chasis_motor2_pid_v;
extern PID_Regulator_t chasis_motor3_pid_v;
extern PID_Regulator_t chasis_motor4_pid_v;

extern PID_Regulator_t gimbal_pitch_pid_v;
extern PID_Regulator_t gimbal_pitch_pid_p;
extern PID_Regulator_t gimbal_yaw_pid_v;
extern PID_Regulator_t gimbal_yaw_pid_p;

extern PID_Regulator_t chasis_follow_pid_p;
extern PID_Regulator_t gimbal_back_yaw_pid_p;//复位pid


extern PID_Regulator_t gimbal_yaw_slef_aim_pid;
extern PID_Regulator_t gimbal_pitch_slef_aim_pid;


extern PID_Regulator_t chasis_motor_power_pid;


float PIDz_Calculate(PID_Struct* PID, float measured, float expect);
float PID_Calculate(PID_Struct* PID, float measured, float expect);
void PID_Init(PID_Struct *PID,float kp,float ti,float td,
	            float out_max,float out_min,
							float integrate_max,float integrate_min,
							float sumerr_max,float sumerr_min);							
void DJI_PID_Init(PID_Regulator_t* PID,float kp,float ki,float kd,
									float kpmax,float kimax,float kdmax,float outmax);							
float DJI_PID_Cal(PID_Regulator_t* PID,float measure,float exp,float outmax);
void DJI_PID_Param_Init(void);

#endif
