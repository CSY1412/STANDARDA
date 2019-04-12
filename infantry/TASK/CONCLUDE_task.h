#ifndef __CONCLUDE_TASK_H
#define __CONCLUDE_TASK_H

#include "main.h"

typedef struct{	
	float cm_out1;
	float cm_out2;
	float cm_out3;
	float cm_out4;
	float cm_temp1;
	float cm_temp2;
	float cm_temp3;
	float cm_temp4;
	float current_offset;
}cm_motor_t;
extern cm_motor_t cm_motor;

void CONCLUDE_task(void *pvParameters);
void CalChasisAndSend(void);
void CalGimabalAndSend(void);
void MotorSpeedAnalyze(float* cm1,float *cm2,float *cm3,float *cm4);
void CurrentControl(void);
#endif