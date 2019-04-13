#ifndef __GIMBAL_TASK_H
#define __GIMBAL_TASK_H


#include "main.h"

typedef struct
{
	float pitchexp;
	float pitchout;
	float pitchtemp;
	float pitchadd;
}pitch_t;
extern pitch_t pitchdata;


typedef struct
{
	float yawexp;
	float yawout;
	float yawtemp;
	float yawadd;
}yaw_t;
extern yaw_t yawdata;
void GIMBAL_task(void *pvParameters);
void FirstGimbalBack(void);    //��̨�״ι�λ
void LimitAngle(float* exppitch);
void GimbalCtl(void);
void GimbalNomalCtl(void); //��̨����
void CheckUpFinishi(void);
void ProtectGimbal(void);  //��̨����
void GimbalQuickBack(void);
void CheckUpQuickBackFinishi(void);
void GimbalSlefaimCtl(void);
void KeepPitchexp(void);
#endif