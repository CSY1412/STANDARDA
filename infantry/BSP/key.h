#ifndef __KEY_H
#define __KEY_H

#include "main.h"
/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
#define KEY4   	GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	//PE5
#define KEY3 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) //PE4
#define KEY2 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)	//PE3 
#define KEY1 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0) //PE2


void KEY_Init(void);	//IO��ʼ��
u8 KEY_Scan(u8);  		//����ɨ�躯��	

#endif
