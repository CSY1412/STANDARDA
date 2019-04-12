#ifndef __AUX_TASK_H
#define __AUX_TASK_H
#include "main.h"



extern u8 yellow_led_count;
extern u8 blue_led_count;
extern u8 red_led_count;
extern u8 green_led_count;

void AUX_task(void *pvParameters);
void LEDAlam(void);
void BlueLED(u8 count);
void YellowLED(u8 count);
void RedLED(u8 count);
void GreenLED(u8 count);

#endif