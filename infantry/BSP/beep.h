#ifndef __BEEP_H
#define __BEEP_H	 
#include "main.h"
//LED端口定义
#define BEEP_ON PBout(15)	// 蜂鸣器控制IO 

void BEEP_Init(void);//初始化
void BeepAlam(void);
void BeepON(void);
void BeepOFF(void);
void BeepNumb(int num);

#endif

















