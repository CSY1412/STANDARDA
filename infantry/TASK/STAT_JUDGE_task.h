#ifndef __STAT_JUDGE_TASK_H
#define __STAT_JUDGE_TASK_H
#include "main.h"

#define PRESS 1
#define DISSPRESS 0


extern u32 last_back_time;

typedef enum{
preparing,  ///׼��
running,   // �����˶�
self_stabilization, //�ȶ�
breaking,   //Ӳ������
}system_stat_e;
extern system_stat_e system_stat;

typedef enum{
free_and_preparing,  //�ͷ�״̬
control,    //����״̬
}gimabal_main_stat_e;  
extern gimabal_main_stat_e gimabal_main_stat; //��̨��ģʽ

typedef enum{
self_aim,
small_god_symbal,
big_god_symbal,
back_posion,  //��λģʽ
slowing,   //����ģʽ
normal,	
}gimabal_aux_mode_e;   //��̨ģʽ
extern gimabal_aux_mode_e gimabal_aux_mode;  //��ģʽ

typedef enum{
follow_gimabal,  //������̨
self_move,    //��������̨
}chasis_follow_stat_e;
extern chasis_follow_stat_e chasis_follow_stat;

typedef enum{
raw_contrl,  //����ģʽ
pid_contrl,  //PID�ջ�ģʽ
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
}shoot_mode_e;  //����ͳ�������
extern shoot_mode_e shoot_mode;

typedef enum{
off=0,
on=1,
}fricmotor_stat_e; //Ħ����״̬
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