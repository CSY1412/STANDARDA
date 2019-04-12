#ifndef __DETECTION_TASK_H
#define __DETECTION_TASK_H



#include "main.h"
typedef struct
{
		u32 chasis_motor1;
		u32 chasis_motor2;
		u32 chasis_motor3;
		u32 chasis_motor4;
		u32 gimbal_yaw;
		u32 gimbal_pitch;
		u32 gesture_sensor;
		u32 rc;
		u32 aux_board;
		u16 stat;    //从低位到高为 ，1--4  底盘4个电机，5--6 yaw  pitch 电机 7位辅助控制板，8位 姿态传感器，9位接收机，  //置1 为失连
}slef_check_count_t;
extern slef_check_count_t slef_check_count;


void DETECTION_task(void *pvParameters);
void DetectConnect(u32 DT);
void ClearCount(void); //清除

#endif