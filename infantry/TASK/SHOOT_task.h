#ifndef __SHOOT_TASK_H
#define __SHOOT_TASK_H
#include "main.h"


typedef __packed struct
{
		float exp_bullet;
    u32 time_count;
	
}shoot_t;

extern shoot_t shoot;

void SHOOT_task(void *pvParameters);   
void Judege_Shoot(void);

#endif