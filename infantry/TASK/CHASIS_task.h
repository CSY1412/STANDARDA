#ifndef __CHASIS_TASK_H
#define __CHASIS_TASK_H
#include "main.h"
typedef struct{
  float vx;
	float vy;
	float rotate;
	float swing_angle;  //底盘相对角度
}chasis_control_t;
extern chasis_control_t chasis_control;
extern chasis_control_t chasis_control_temp;

void CHASIS_task(void *pvParameters);
void ChasisControl(void);
void LockChasis(void);  //锁定底盘
void ChasisRoteCal(void);
void chasisSwingcontrol(fp32 *angle_set);

#endif