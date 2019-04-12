#ifndef __KEY_H
#define __KEY_H

#include "main.h"
/*下面的方式是通过直接操作库函数方式读取IO*/
#define KEY4   	GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	//PE5
#define KEY3 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) //PE4
#define KEY2 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)	//PE3 
#define KEY1 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0) //PE2


void KEY_Init(void);	//IO初始化
u8 KEY_Scan(u8);  		//按键扫描函数	

#endif
