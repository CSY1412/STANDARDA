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
		u16 stat;    //�ӵ�λ����Ϊ ��1--4  ����4�������5--6 yaw  pitch ��� 7λ�������ư壬8λ ��̬��������9λ���ջ���  //��1 Ϊʧ��
}slef_check_count_t;
extern slef_check_count_t slef_check_count;


void DETECTION_task(void *pvParameters);
void DetectConnect(u32 DT);
void ClearCount(void); //���

#endif