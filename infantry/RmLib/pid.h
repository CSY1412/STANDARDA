/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       pid.c/h
  * @brief      pidʵ�ֺ�����������ʼ����PID���㺯����
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. ���
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2016 DJI****************************
  */
#ifndef PID_H
#define PID_H
#include "main.h"
enum PID_MODE
{
    PID_POSITION = 0,
    PID_DELTA
};

typedef __packed struct
{
    uint8_t mode;
    //PID ������
    fp32 Kp;
    fp32 Ki;
    fp32 Kd;

    fp32 max_out;  //������
    fp32 max_iout; //���������

    fp32 set;
    fp32 fdb;

    fp32 out;
    fp32 Pout;
    fp32 Iout;
    fp32 Dout;
    fp32 Dbuf[3];  //΢���� 0���� 1��һ�� 2���ϴ�
    fp32 error[3]; //����� 0���� 1��һ�� 2���ϴ�

} PidTypeDef;


extern PidTypeDef chassis_motor[4];  //����·���
extern PidTypeDef gimbal_speed[2];
extern PidTypeDef gimbal_position[2];
extern PidTypeDef chasis_follow;
extern PidTypeDef gimbal_back_yaw_pid_p;  //��̨��λyaw
extern PidTypeDef gimbal_back_pitch_pid_p;//��̨��λpitch


void PID_Init(PidTypeDef *pid, uint8_t mode,fp32 kp,fp32 ki,fp32 kd, fp32 max_out, fp32 max_iout);
fp32 PID_Calc(PidTypeDef *pid, fp32 ref, fp32 set,float outmax);
void PID_clear(PidTypeDef *pid);
void allPIDInit(void);
#endif
