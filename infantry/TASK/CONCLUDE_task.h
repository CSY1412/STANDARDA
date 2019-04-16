#ifndef __CONCLUDE_TASK_H
#define __CONCLUDE_TASK_H

#include "main.h"

typedef struct{	
	float cm_out[4];
	float cm_temp[4];
	float current_offset;
}cm_motor_t;
extern cm_motor_t cm_motor;

void CONCLUDE_task(void *pvParameters);
void CalChasisAndSend(void);
void CalGimabalAndSend(void);
void MotorSpeedAnalyze(float *wheel_speed);
void CurrentControl(void);
static void chassis_vector_to_mecanum_wheel_speed(const fp32 vx_set, const fp32 vy_set, const fp32 wz_set, fp32 * wheel_speed);
void MotorSpeedAnalyze(float *wheel_speed);
void chassis_set_contorl(float *exp_vx,float *exp_vy,float econder_angle);
#endif