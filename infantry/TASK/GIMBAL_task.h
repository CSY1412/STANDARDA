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
void FirstGimbalBack(void);    //云台首次归位
void LimitAngle(float* exppitch);
void GimbalCtl(void);
void GimbalNomalCtl(void); //云台控制
void CheckUpFinishi(void);
void ProtectGimbal(void);  //云台保护
void GimbalQuickBack(void);
void CheckUpQuickBackFinishi(void);
void GimbalSlefaimCtl(void);
void KeepPitchexp(void);
#endif