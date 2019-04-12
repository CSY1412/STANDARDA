#ifndef __CONFIG_H
#define __CONFIG_H


#define DEBUG_CHASIS 0  
#define DEBUG_GIMBAL 0 


#define KEEP_SHOOT_JUDGE_TIME 230  //�жϽ�������ģʽ����ʱ
#define ONE_BULLET_SHOOT_TIME 230  //һ���ӵ����˶���ʱ

#define MAX_CHASIS_POWER 80   //�����̹��� ��λ��
#define FLASH_PASS_WORD 0x58  //flashУ������
#define THIS_CAR_TYPE INFANTRY_A    //������������

#define PITCH_MAX 1200.0f // ��̨pitch���    
#define PITCHMIN -1200.0f  // ��̨pitch��С   
#define YAW_ROTATE_SPEED_MAX 200  //yaw��ת�ٶȻ����ֵ  
#define PITCHROTATESPEEDMAX 400  //pitch��ת�ٶȻ����ֵ   

#define YAWBACKROTATESPEEDMAX 140  //yaw���λ�ٶȻ����
#define PITCHBACKROTATESPEEDMAX 210//pitch���λ�ٶȻ����
#define SLOWLING_BACK_TIME 1400   //���ٻ�λ��ʱ���

#define YAWQUICKBACKROTATESPEEDMAX 240
#define PITCHQUICKBACKROTATESPEEDMAX 240
#define QUICKBACKTIME 400


#define YAWPROTECTECONDERDETA 330

#define MAXANGLEOFFSET 1500   //��̨���ƫ��

#define REMOTEPITCHMOVERATE 0.01
#define REMOTEYAWMOVERATE 0.0005

#define MOUSEPITCHMOVERATE 0.12
#define MOUSEYAWMOVERATE 0.004

#define SHIFT_SPEEDMAX 8100
#define MAX_CHASIS_FOLLOW_ECD 1200;  //QEʱ���ƫ��
#define QE_ROATE_SPEED_MAX 3500   //��ת�������
#define CHASIS_FOLLOW_ROATE_SPEED_MAX 5500  //���̸���ʱ���������
#define WATCHDOG_RESET_MS 100	/*���Ź���λʱ��*/


#endif