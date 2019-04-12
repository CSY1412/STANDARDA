#ifndef USER_LIB_H
#define USER_LIB_H
#include "main.h"

typedef  float      fp32;

typedef __packed struct
{
    fp32 input;        //输入数据
    fp32 out;          //输出数据
    fp32 min_value;    //限幅最小值
    fp32 max_value;    //限幅最大值
    fp32 frame_period; //时间间隔
} ramp_function_source_t;

typedef __packed struct
{
    fp32 input;        //输入数据
    fp32 out;          //滤波输出的数据
    fp32 num;       //滤波参数
    fp32 frame_period; //滤波的时间间隔 单位 s
} first_order_filter_type_t;

extern first_order_filter_type_t gesture_sensor_Gx;   //姿态传感器x轴速度
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
	Acc_t Acc;   //加速度       
	Gyro_t Gyro;  //角速度
	Angle_t Angle;  //欧拉角
	Magnetic_t Magnetic;//磁场
  short T;
}imu_data_t;

extern imu_data_t imu_data_access;
extern imu_data_t imu_data_temp;

//快速开方
extern fp32 invSqrt(fp32 num);

//斜波函数初始化
void ramp_init(ramp_function_source_t *ramp_source_type, fp32 frame_period, fp32 max, fp32 min);

//斜波函数计算
void ramp_calc(ramp_function_source_t *ramp_source_type, fp32 input);
//一阶滤波初始化
extern void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, fp32 frame_period, const fp32 num);
//一阶滤波计算
extern float first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, fp32 input);
//绝对限制
extern void abs_limit(fp32 *num, fp32 Limit);
//判断符号位
extern fp32 sign(fp32 value);
//浮点死区
extern fp32 fp32_deadline(fp32 Value, fp32 minValue, fp32 maxValue);
//int26死区
extern int16_t int16_deadline(int16_t Value, int16_t minValue, int16_t maxValue);
//限幅函数
extern fp32 fp32_constrain(fp32 Value, fp32 minValue, fp32 maxValue);
//限幅函数
extern int16_t int16_constrain(int16_t Value, int16_t minValue, int16_t maxValue);
//循环限幅函数
extern fp32 loop_fp32_constrain(fp32 Input, fp32 minValue, fp32 maxValue);
//角度 °限幅 180 ~ -180
extern fp32 theta_format(fp32 Ang);

//弧度格式化为-PI~PI
#define rad_format(Ang) loop_fp32_constrain((Ang), -PI, PI)

#endif
