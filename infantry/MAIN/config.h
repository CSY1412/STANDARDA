#ifndef __CONFIG_H
#define __CONFIG_H








#define MAX_CHASIS_FOLLOW_ECD 1200;  //QEʱ���ƫ��
#define QE_ROATE_SPEED_MAX 3500   //QE��ת�������


/*------------------------------------�������--------------------------------*/
#define KEEP_SHOOT_JUDGE_TIME 230  //�жϽ�������ģʽ����ʱ
#define ONE_BULLET_SHOOT_TIME 230  //һ���ӵ����˶���ʱ

/*------------------------------------ң��������--------------------------------*/
#define RC_DEAD_BAND 50  //ң��������ֵ
/*------------------------------------��̨����--------------------------------*/

#define MAX_YAW_OFFSET_ANGLE 60   //��̨yaw���ƫ��
#define PITCH_MAX 20.0f // ��̨pitch���    
#define PITCH_MIN -20.0f  // ��̨pitch��С   
#define YAW_ROTATE_SPEED_MAX 700  //yaw��ת�ٶȻ����ֵ  

#define GIMBAL_YAW_BACK_KP     70    //��λyawλ�û�
#define GIMBAL_PITCH_BACK_KP    1   //��λpitchλ�û�
#define PITCH_ROTATE_SPEED_MAX 400  //pitch��ת�ٶȻ����ֵ   
#define YAW_BACK_ROTATE_SPEED_MAX 350  //yaw���λ�ٶȻ����
#define PITCH_BACK_ROTATE_SPEED_MAX 210//pitch���λ�ٶȻ����
#define SLOWLING_BACK_TIME 800   //��λ��ʱ���

#define YAW_PROTECTE_OFFSET_ANGLE 10  //ƫ�ƶ�
#define REMOTE_PITCH_MOVE_SPEEDRATE 0.01  //ң����P���������
#define REMOTE_YAW_MOVE_SPEEDRATE 0.0005    //ң����Y���������
#define MOUSE_PITCH_MOVE_SPEEDRATE 0.12
#define MOUSE_YAW_MOVE_SPEEDRATE 0.004
#define MAX_M6623_CURRENT 5000
/*------------------------------------ϵͳ����--------------------------------*/
#define DEBUG_CHASIS 0  
#define DEBUG_GIMBAL 0 
#define WATCHDOG_RESET_MS 100  //���Ź�����ʱ��(ms)
#define FLASH_PASS_WORD 0x58  //flashУ������
#define THIS_CAR_TYPE INFANTRY_A    //������������


/*------------------------------------��������--------------------------------*/
#define MAX_CHASIS_POWER 80   //�����̹��� ��λ��


#define CHASSIS_ACCEL_X_NUM 0.0966666667f //һ�׵�ͨ�˲�ϵ��
#define CHASSIS_ACCEL_Y_NUM 0.03f   //0.07
#define CHASSIS_ACCEL_QE_NUM 0.15f

#define KEY_CHASSIS_SPEED_X 1.5f  //���������˶��������ƽ���ٶ�
#define KEY_CHASSIS_SPEED_Y 2.2f  //���������˶��������ǰ���ٶ�
#define RC_CHASSIS_SPEED_X 1.5f  //ң���������˶��������ƽ���ٶ�
#define RC_CHASSIS_SPEED_Y 2.0f  //ң���������˶��������ǰ���ٶ�
#define SHIFT_CHASSIS_SPEED_Y 1.0f*MAX_M3508_SPEED  //�����˶��������ǰ���ٶ�
#define CHASIS_FOLLOW_ROATE_SPEED_MAX 11  //���̸���ʱ�������ת����




/*------------------------------------���̵��--------------------------------*/
#define M3508_MOTOR_RPM_TO_VECTOR 0.000415809748903494517209f //m3508ת���ɵ����ٶ�(m/s)�ı�����
#define MAX_M3508_RPM 8100   //���3508���ת��
#define MAX_M3508_CURRENT 16384   //���3508�������
#define MAX_M3508_SPEED M3508_MOTOR_RPM_TO_VECTOR*MAX_M3508_RPM //����������ٶ�
#define CHASSIS_ROATE_SET_SCALE 0.1f  //����������ת�ٶȣ�����ǰ�������ֲ�ͬ�趨�ٶȵı�����Ȩ 0Ϊ�ڼ������ģ�����Ҫ����
#define MOTOR_DISTANCE_TO_CENTER 0.1f  //���̾������ĵľ���
#define MAX_WHEEL_SPEED 4.0f         //���̵������ٶ�

#endif