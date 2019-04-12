#ifndef __CHASIS_TASK_H
#define __CHASIS_TASK_H
#include "main.h"
typedef struct{
  float x;
	float y;
	float rotate;
}chasis_expspeed_t;
extern chasis_expspeed_t chasis_expspeed;

void CHASIS_task(void *pvParameters);
void ChasisControl(void);
void LockChasis(void);  //Ëø¶¨µ×ÅÌ
void ChasisRoteCal(void);

#endif