#ifndef __CAN_H__
#define __CAN_H__
#include "main.h"



/* CAN send and receive ID */
typedef enum
{
    CAN_CHASSIS_ALL_ID = 0x200,
    CAN_3508_M1_ID = 0x201,
    CAN_3508_M2_ID = 0x202,
    CAN_3508_M3_ID = 0x203,
    CAN_3508_M4_ID = 0x204,

    CAN_YAW_MOTOR_ID = 0x205,
    CAN_PIT_MOTOR_ID = 0x206,
    CAN_TRIGGER_MOTOR_ID = 0x207,
    CAN_GIMBAL_ALL_ID = 0x1FF,

} can_msg_id_e;

typedef struct
 {
	u16 last_angle; //��һ�εĽǶ�
	u16 angle;      //��е�Ƕ�
	int16_t rpm;  //ת��ת��
	int16_t current;//ʵ��ת�ص��� ����3508�У�
	uint8_t temperature;//����¶�    ����3508�У�
	float speed;
 }chasis_motor_t;
extern chasis_motor_t chasis_motor[4];

typedef  struct    
{        
	float cal_angle;  // �����ĽǶ�
	u16 real_angle;  //��ʵ�����������Ƕ�
	float last_angle;
	float this_angle;
	float angle_speed;
	float first_angle;
}gimbal_motor_t;
extern gimbal_motor_t gimbal_motor[4];



void CAN1_Configuration(void);
s16 transe_angle(s16 angle,u16 first_angle);
void CAN2_Configuration(void);
void Set_CM_Speed(int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq);
void Set_Gimbal_Current(int16_t gimbal_yaw_iq, int16_t gimbal_pitch_iq);
void CAN_CMD_CHASSIS_RESET_ID(void);
void Set_Gimbal_Calibration(void);
void Process3508Data(chasis_motor_t *data, u8 *message);
void Process6623Data(gimbal_motor_t* motor,u8 *message);
#endif 
