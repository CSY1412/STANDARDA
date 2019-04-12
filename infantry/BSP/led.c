#include "led.h"


void LED_Configuration(void)
{
  GPIO_InitTypeDef gpio;
    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE,ENABLE);
	
  gpio.GPIO_Pin = GPIO_Pin_13 ;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&gpio);

	gpio.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_Init(GPIOE,&gpio); 
	
	
	PCout(13)=1;  //绿色
	PEout(4)=1;   //红色
	PEout(5)=1;   //蓝色
	PEout(6)=1;   //黄色
	 
}


void SetLED(u8 val)  //低4位有效
{
	if((val&0x01)!=0)
		PCout(13)=0;  //绿色
	else
		PCout(13)=1;  //绿色
	if((val&0x02)!=0)
		PEout(6)=0;  //绿色
	else
		PEout(6)=1;  //绿色
	if((val&0x04)!=0)
		PEout(5)=0;  //绿色
	else
		PEout(5)=1;  //绿色
	if((val&0x08)!=0)
		PEout(4)=0;  //绿色
	else
		PEout(4)=1;  //绿色
}

void FlowLED(u8 flowspeed)
{
			u8 led=0x03;
			for(u8 i=0;i<3;i++)
		{
			SetLED(led);
			delay_ms(flowspeed);
			led=led<<1;
		
		}
			for(u8 i=0;i<5;i++)
		{
			SetLED(led);
			delay_ms(flowspeed);
			led=led>>1;		
		}

}


void LightingGreenLED(void)
{
		static _Bool green=0;
		green=!green;
		PCout(13)=green;
}
