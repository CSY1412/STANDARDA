#ifndef __APP_H
#define __APP_H

#include "main.h"

#define RATE_1_HZ		1
#define RATE_2_HZ		2
#define RATE_5_HZ		5
#define RATE_10_HZ		10
#define RATE_20_HZ		20
#define RATE_25_HZ		25
#define RATE_50_HZ		50
#define RATE_100_HZ		100
#define RATE_200_HZ 	200
#define RATE_250_HZ 	250
#define RATE_500_HZ 	500
#define RATE_1000_HZ 	1000
#define RATE_DO_EXECUTE(RATE_HZ, TICK) ((TICK % (RATE_1000_HZ / RATE_HZ)) == 0)



typedef struct{
	float raw_value;   									//编码器不经处理的原始值
	float last_raw_value;								//上一次的编码器原始值
	float ecd_value;                    //经过处理后连续的编码器值
	float diff;													//两次编码器之间的差值
	int32_t temp_count;                   //计数用
	int32_t round_cnt;										//圈数
}extend_angle_t;
extern extend_angle_t extend_angle;

u32 GetSysTickCnt(void);
void BspInit(void);
void ParamInit(void);
void PrintTaskInfomation(char * name);
void ExtendAngle(volatile extend_angle_t *v, float raw_angle);
float GetMax(float a,float b,float c,float d);
float ProcessGimbalexp(float input,float max_offset);
void IWDG_Init(void);
void ChangeNowPitchAngleToExpangle(void);
void ChangeNowYawAngleToExpangle(void);
void PID_Load(void);
void RecordGimbalZeroPosition(void);
void FirstFlashRead(void);
#endif


  

