#ifndef __FLASH_H
#define __FLASH_H

#include "main.h"


#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
 
#define FLASH_USER_START_ADDR   (uint32_t)0x080E0000

#define FLASH_USER_END_ADDR     (uint32_t)0x080FFFFF



typedef __packed struct
{
  float kp;
	float ki;
	float kd;
}save_pid;


typedef __packed struct
{
	u8 password_first;
	
	save_pid chasis_motor[4];
	save_pid gimbal_yaw_p;
	save_pid gimbal_yaw_v;
	save_pid gimbal_pitch_p;
	save_pid gimbal_pitch_v;
	save_pid chasis_follow_p;
	save_pid manifold_follow_x;
	save_pid manifold_follow_y;

	float gimbal_pitch_zero;
	float gimbal_yaw_zero;
	save_pid power_control;
	u8 system_err;  //错误黑匣子
	
	u8 password_end;
	
}flash_save_data_t;


extern flash_save_data_t flash_save_data_access;
extern flash_save_data_t flash_save_data_temp;


u8 Flash_Data_Save(void);
u8 Data_Read(void);
void Load_And_Save_Param(void);
static u8 read_byte(u32 start_addr, u16 cnt);
static u8 write_byte(u32 start_addr, u16 cnt, u8 data);

#endif