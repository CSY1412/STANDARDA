#ifndef USER_LIB_H
#define USER_LIB_H
#include "main.h"

typedef  float      fp32;

typedef __packed struct
{
    fp32 input;        //��������
    fp32 out;          //�������
    fp32 min_value;    //�޷���Сֵ
    fp32 max_value;    //�޷����ֵ
    fp32 frame_period; //ʱ����
} ramp_function_source_t;

typedef __packed struct
{
    fp32 input;        //��������
    fp32 out;          //�˲����������
    fp32 num;       //�˲�����
    fp32 frame_period; //�˲���ʱ���� ��λ s
} first_order_filter_type_t;

extern first_order_filter_type_t gesture_sensor_Gx;   //��̬������x���ٶ�
extern first_order_filter_type_t chassis_slow_set_vx; 
extern first_order_filter_type_t chassis_slow_set_vy;   
extern first_order_filter_type_t chassis_follow_QE;
extern first_order_filter_type_t chasis_power_control;




typedef __packed struct
{
    uint8_t  AccFlag;
    int16_t x;
    int16_t y;
    int16_t z;
}Acc_t;

typedef __packed struct
{
    uint8_t  GyroFlag;
    int16_t x;
    int16_t y;
    int16_t z;
}Gyro_t;

typedef __packed struct
{
    uint8_t AngleFlag;
    float roll;
    float pitch;
    float yaw;
		float expand_yaw;
}Angle_t;


typedef __packed struct
{
    uint8_t MagneticFlag;
    float x;
    float y;
    float z;
}Magnetic_t;


typedef __packed struct
{
	Acc_t Acc;   //���ٶ�       
	Gyro_t Gyro;  //���ٶ�
	Angle_t Angle;  //ŷ����
	Magnetic_t Magnetic;//�ų�
  short T;
}imu_data_t;

extern imu_data_t imu_data_access;
extern imu_data_t imu_data_temp;

//���ٿ���
extern fp32 invSqrt(fp32 num);

//б��������ʼ��
void ramp_init(ramp_function_source_t *ramp_source_type, fp32 frame_period, fp32 max, fp32 min);

//б����������
void ramp_calc(ramp_function_source_t *ramp_source_type, fp32 input);
//һ���˲���ʼ��
extern void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, fp32 frame_period, const fp32 num);
//һ���˲�����
extern float first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, fp32 input);
//��������
extern void abs_limit(fp32 *num, fp32 Limit);
//�жϷ���λ
extern fp32 sign(fp32 value);
//��������
extern fp32 fp32_deadline(fp32 Value, fp32 minValue, fp32 maxValue);
//int26����
extern int16_t int16_deadline(int16_t Value, int16_t minValue, int16_t maxValue);
//�޷�����
extern fp32 fp32_constrain(fp32 Value, fp32 minValue, fp32 maxValue);
//�޷�����
extern int16_t int16_constrain(int16_t Value, int16_t minValue, int16_t maxValue);
//ѭ���޷�����
extern fp32 loop_fp32_constrain(fp32 Input, fp32 minValue, fp32 maxValue);
//�Ƕ� ���޷� 180 ~ -180
extern fp32 theta_format(fp32 Ang);

//���ȸ�ʽ��Ϊ-PI~PI
#define rad_format(Ang) loop_fp32_constrain((Ang), -PI, PI)

#endif
