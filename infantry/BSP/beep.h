#ifndef __BEEP_H
#define __BEEP_H	 
#include "main.h"
//LED�˿ڶ���
#define BEEP_ON PBout(15)	// ����������IO 

void BEEP_Init(void);//��ʼ��
void BeepAlam(void);
void BeepON(void);
void BeepOFF(void);
void BeepNumb(int num);

#endif

















