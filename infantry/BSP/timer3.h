#ifndef __TIMER3_H
#define __TIMER3_H
#include "main.h"

void TIM3_Init(u32 arr,u32 psc);
void MicroStepMotor_Configuration(void);
void FeedMotorON(void);
void FeedMotorOFF(void);

#endif