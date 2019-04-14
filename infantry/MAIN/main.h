#ifndef __MAIN_H
#define __MAIN_H


/*---------------整车配置---------------*/
#include "config.h"

/*---------------操作系统头文件----------*/
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

/*---------------32库头文件--------------*/
#include "sys.h"
#include "delay.h"
#include "usart.h"

/*---------------算法头文件--------------*/
#include "app.h"
#include "arm_math.h"
#include "user_lib.h"
#include "filter.h"
#include "maths.h"
#include "pid.h"
#include <math.h>
#include "stdio.h"
#include "fifo.h"
#include "filter.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


/*---------------外设头文件--------------*/
#include "led.h"
#include "key.h"
#include "myiic.h"
#include "usart2.h"
#include "can.h"
#include "usart1.h"
#include "usart3.h"
#include "usart4.h"
#include "usart5.h"
#include "usart6.h"
#include "fricmotor.h"
#include "beep.h"
#include "timer3.h"
#include "flash.h"


/*---------------任务头文件--------------*/
#include "AUX_task.h"
#include "GIMBAL_task.h"
#include "CHASIS_task.h"
#include "STAT_JUDGE_task.h"
#include "CONCLUDE_task.h"
#include "DETECTION_task.h"
#include "IO_Task.h"
#include "CONFIG_PARAM_task.h"
#include "SHOOT_task.h"
#include "REFERENCE_task.h"



/*---------------任务句柄--------------*/
extern TaskHandle_t STAT_JUDGETask_Handler;
extern TaskHandle_t CHASISTask_Handler;
extern TaskHandle_t GIMBALTask_Handler;
extern TaskHandle_t CONCLUDETask_Handler;
extern TaskHandle_t AUXTask_Handler;
extern TaskHandle_t UploadTask_Handler;
extern TaskHandle_t CONFIG_PARAM_Task_Handler;
extern TaskHandle_t REFERENCE_Task_Handler;
extern TaskHandle_t SHOOT_Task_Handler;


void start_task(void *pvParameters);//任务函数
void vApplicationIdleHook( void );

#endif