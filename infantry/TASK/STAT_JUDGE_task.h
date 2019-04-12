#ifndef __STAT_JUDGE_TASK_H
#define __STAT_JUDGE_TASK_H
#include "main.h"

#define PRESS 1
#define DISSPRESS 0


extern u32 last_back_time;

typedef enum{
preparing,  ///准备
running,   // 正常运动
self_stabilization, //稳定
breaking,   //硬件故障
}system_stat_e;
extern system_stat_e system_stat;

typedef enum{
free_and_preparing,  //释放状态
control,    //控制状态
}gimabal_main_stat_e;  
extern gimabal_main_stat_e gimabal_main_stat; //云台主模式

typedef enum{
self_aim,
small_god_symbal,
big_god_symbal,
back_posion,  //回位模式
slowing,   //慢速模式
normal,	
}gimabal_aux_mode_e;   //云台模式
extern gimabal_aux_mode_e gimabal_aux_mode;  //子模式

typedef enum{
follow_gimabal,  //跟随云台
self_move,    //不跟随云台
}chasis_follow_stat_e;
extern chasis_follow_stat_e chasis_follow_stat;

typedef enum{
raw_contrl,  //开环模式
pid_contrl,  //PID闭环模式
}chasis_motor_stat_e;
extern chasis_motor_stat_e chasis_motor_stat;

typedef enum{
remote, 
keyboard,  
}control_mode_e;
extern control_mode_e control_mode;

typedef enum{
point, 
keep,  
}shoot_mode_e;  //点射和持续发射
extern shoot_mode_e shoot_mode;

typedef enum{
off=0,
on=1,
}fricmotor_stat_e; //摩擦轮状态
extern fricmotor_stat_e fricmotor_stat;


void STAT_JUDGE_task(void *pvParameters);
void JudgeSystemStat(void);
void JudgeFricMotor(void);
void ShootControl(void);
void JudgeGimabalStat(void);
void JudgeChasisStat(void);
void JudgeCalibration(void);
void MicroStepMotorControl(void);
void KEYFunction(void);
void SetSystemStat(system_stat_e enu);
void SetGimabalMainStat(gimabal_main_stat_e enu);

#endif